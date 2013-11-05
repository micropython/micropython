#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpyconfig.h"
#include "runtime.h"
#include "bc.h"

// (value) stack grows down (to be compatible with native code when passing pointers to the stack), top element is pointed to
// exception stack grows up, top element is pointed to

#define DECODE_UINT do { unum = *ip++; if (unum > 127) { unum = ((unum & 0x3f) << 8) | (*ip++); } } while (0)
#define DECODE_ULABEL do { unum = (ip[0] | (ip[1] << 8)); ip += 2; } while (0)
#define DECODE_SLABEL do { unum = (ip[0] | (ip[1] << 8)) - 0x8000; ip += 2; } while (0)
#define DECODE_QSTR do { qstr = *ip++; if (qstr > 127) { qstr = ((qstr & 0x3f) << 8) | (*ip++); } } while (0)
#define PUSH(val) *--sp = (val)
#define POP() (*sp++)

// args are in reverse order in array
py_obj_t py_execute_byte_code(const byte *code, const py_obj_t *args, uint n_args) {
    py_obj_t state[18]; // TODO allocate properly
    // init args
    for (int i = 0; i < n_args; i++) {
        assert(i < 8);
        state[i] = args[n_args - 1 - i];
    }
    py_obj_t *sp = &state[18];
    const byte *ip = code;
    if (py_execute_byte_code_2(&ip, &state[0], &sp)) {
        // it shouldn't yield
        assert(0);
    }
    // TODO check fails if, eg, return from within for loop
    //assert(sp == &state[17]);
    return *sp;
}

// fastn has items in normal order
// sp points to top of stack which grows down
bool py_execute_byte_code_2(const byte **ip_in_out, py_obj_t *fastn, py_obj_t **sp_in_out) {
    // careful: be sure to declare volatile any variables read in the exception handler (written is ok, I think)

    const byte *ip = *ip_in_out;
    py_obj_t *sp = *sp_in_out;
    machine_uint_t unum;
    qstr qstr;
    py_obj_t obj1, obj2;
    py_obj_t fast0 = fastn[0], fast1 = fastn[1], fast2 = fastn[2];
    nlr_buf_t nlr;

    // on the exception stack we store (ip, sp) for each block
    machine_uint_t exc_stack[8];
    machine_uint_t *volatile exc_sp = &exc_stack[-1]; // stack grows up, exc_sp points to top of stack

    // outer exception handling loop
    for (;;) {
        if (nlr_push(&nlr) == 0) {
            // loop to execute byte code
            for (;;) {
                int op = *ip++;
                switch (op) {
                    case PYBC_LOAD_CONST_FALSE:
                        PUSH(py_const_false);
                        break;

                    case PYBC_LOAD_CONST_NONE:
                        PUSH(py_const_none);
                        break;

                    case PYBC_LOAD_CONST_TRUE:
                        PUSH(py_const_true);
                        break;

                    case PYBC_LOAD_CONST_SMALL_INT:
                        unum = (ip[0] | (ip[1] << 8) | (ip[2] << 16)) - 0x800000;
                        ip += 3;
                        PUSH((py_obj_t)(unum << 1 | 1));
                        break;

                    case PYBC_LOAD_CONST_DEC:
                        DECODE_QSTR;
                        PUSH(rt_load_const_dec(qstr));
                        break;

                    case PYBC_LOAD_CONST_ID:
                        DECODE_QSTR;
                        PUSH(rt_load_const_str(qstr)); // TODO
                        break;

                    case PYBC_LOAD_CONST_STRING:
                        DECODE_QSTR;
                        PUSH(rt_load_const_str(qstr));
                        break;

                    case PYBC_LOAD_FAST_0:
                        PUSH(fast0);
                        break;

                    case PYBC_LOAD_FAST_1:
                        PUSH(fast1);
                        break;

                    case PYBC_LOAD_FAST_2:
                        PUSH(fast2);
                        break;

                    case PYBC_LOAD_FAST_N:
                        DECODE_UINT;
                        PUSH(fastn[unum]);
                        break;

                    case PYBC_LOAD_NAME:
                        DECODE_QSTR;
                        PUSH(rt_load_name(qstr));
                        break;

                    case PYBC_LOAD_GLOBAL:
                        DECODE_QSTR;
                        PUSH(rt_load_global(qstr));
                        break;

                    case PYBC_LOAD_ATTR:
                        DECODE_QSTR;
                        *sp = rt_load_attr(*sp, qstr);
                        break;

                    case PYBC_LOAD_METHOD:
                        DECODE_QSTR;
                        sp -= 1;
                        rt_load_method(sp[1], qstr, sp);
                        break;

                    case PYBC_LOAD_BUILD_CLASS:
                        PUSH(rt_load_build_class());
                        break;

                    case PYBC_STORE_FAST_0:
                        fast0 = POP();
                        break;

                    case PYBC_STORE_FAST_1:
                        fast1 = POP();
                        break;

                    case PYBC_STORE_FAST_2:
                        fast2 = POP();
                        break;

                    case PYBC_STORE_FAST_N:
                        DECODE_UINT;
                        fastn[unum] = POP();
                        break;

                    case PYBC_STORE_NAME:
                        DECODE_QSTR;
                        rt_store_name(qstr, POP());
                        break;

                    case PYBC_STORE_GLOBAL:
                        DECODE_QSTR;
                        rt_store_global(qstr, POP());
                        break;

                    case PYBC_STORE_ATTR:
                        DECODE_QSTR;
                        rt_store_attr(sp[0], qstr, sp[1]);
                        sp += 2;
                        break;

                    case PYBC_STORE_SUBSCR:
                        rt_store_subscr(sp[1], sp[0], sp[2]);
                        sp += 3;
                        break;

                    case PYBC_DUP_TOP:
                        obj1 = *sp;
                        PUSH(obj1);
                        break;

                    case PYBC_DUP_TOP_TWO:
                        sp -= 2;
                        sp[0] = sp[2];
                        sp[1] = sp[3];
                        break;

                    case PYBC_POP_TOP:
                        ++sp;
                        break;

                    case PYBC_ROT_TWO:
                        obj1 = sp[0];
                        sp[0] = sp[1];
                        sp[1] = obj1;
                        break;

                    case PYBC_ROT_THREE:
                        obj1 = sp[0];
                        sp[0] = sp[1];
                        sp[1] = sp[2];
                        sp[2] = obj1;
                        break;

                    case PYBC_JUMP:
                        DECODE_SLABEL;
                        ip += unum;
                        break;

                    case PYBC_POP_JUMP_IF_TRUE:
                        DECODE_SLABEL;
                        if (rt_is_true(POP())) {
                            ip += unum;
                        }
                        break;

                    case PYBC_POP_JUMP_IF_FALSE:
                        DECODE_SLABEL;
                        if (!rt_is_true(POP())) {
                            ip += unum;
                        }
                        break;

                        /* we are trying to get away without using this opcode
                    case PYBC_SETUP_LOOP:
                        DECODE_UINT;
                        // push_block(PYBC_SETUP_LOOP, ip + unum, sp)
                        break;
                        */

                    case PYBC_SETUP_EXCEPT:
                        DECODE_ULABEL; // except labels are always forward
                        *++exc_sp = (machine_uint_t)ip + unum;
                        *++exc_sp = (machine_uint_t)sp;
                        break;

                    case PYBC_END_FINALLY:
                        // not implemented
                        // if TOS is an exception, reraises the exception (3 values on TOS)
                        // if TOS is an integer, does something else
                        // if TOS is None, just pops it and continues
                        // else error
                        assert(0);
                        break;

                    case PYBC_GET_ITER:
                        *sp = rt_getiter(*sp);
                        break;

                    case PYBC_FOR_ITER:
                        DECODE_ULABEL; // the jump offset if iteration finishes; for labels are always forward
                        obj1 = rt_iternext(*sp);
                        if (obj1 == py_const_stop_iteration) {
                            ++sp; // pop the exhausted iterator
                            ip += unum; // jump to after for-block
                        } else {
                            PUSH(obj1); // push the next iteration value
                        }
                        break;

                    case PYBC_POP_BLOCK:
                        // pops block and restores the stack
                        assert(0);
                        break;

                    case PYBC_POP_EXCEPT:
                        // TODO need to work out how blocks work etc
                        // pops block, checks it's an exception block, and restores the stack, saving the 3 exception values to local threadstate
                        assert(exc_sp >= &exc_stack[0]);
                        //sp = (py_obj_t*)(*exc_sp--);
                        //exc_sp--; // discard ip
                        exc_sp -= 2;
                        //sp += 3; // pop 3 exception values
                        break;

                    case PYBC_UNARY_OP:
                        unum = *ip++;
                        *sp = rt_unary_op(unum, *sp);
                        break;

                    case PYBC_BINARY_OP:
                        unum = *ip++;
                        obj2 = POP();
                        obj1 = *sp;
                        *sp = rt_binary_op(unum, obj1, obj2);
                        break;

                    case PYBC_COMPARE_OP:
                        unum = *ip++;
                        obj2 = POP();
                        obj1 = *sp;
                        *sp = rt_compare_op(unum, obj1, obj2);
                        break;

                    case PYBC_BUILD_TUPLE:
                        DECODE_UINT;
                        obj1 = rt_build_tuple(unum, sp);
                        sp += unum - 1;
                        *sp = obj1;
                        break;

                    case PYBC_BUILD_LIST:
                        DECODE_UINT;
                        obj1 = rt_build_list(unum, sp);
                        sp += unum - 1;
                        *sp = obj1;
                        break;

                    case PYBC_LIST_APPEND:
                        DECODE_UINT;
                        // I think it's guaranteed by the compiler that sp[unum] is a list
                        rt_list_append(sp[unum], sp[0]);
                        sp++;
                        break;

                    case PYBC_BUILD_MAP:
                        DECODE_UINT;
                        PUSH(rt_build_map(unum));
                        break;

                    case PYBC_STORE_MAP:
                        sp += 2;
                        rt_store_map(sp[0], sp[-2], sp[-1]);
                        break;

                    case PYBC_MAP_ADD:
                        DECODE_UINT;
                        // I think it's guaranteed by the compiler that sp[unum + 1] is a map
                        rt_store_map(sp[unum + 1], sp[0], sp[1]);
                        sp += 2;
                        break;

                    case PYBC_BUILD_SET:
                        DECODE_UINT;
                        obj1 = rt_build_set(unum, sp);
                        sp += unum - 1;
                        *sp = obj1;
                        break;

                    case PYBC_SET_ADD:
                        DECODE_UINT;
                        // I think it's guaranteed by the compiler that sp[unum] is a set
                        rt_store_set(sp[unum], sp[0]);
                        sp++;
                        break;

                    case PYBC_MAKE_FUNCTION:
                        DECODE_UINT;
                        PUSH(rt_make_function_from_id(unum));
                        break;

                    case PYBC_CALL_FUNCTION:
                        DECODE_UINT;
                        assert((unum & 0xff00) == 0); // n_keyword
                        unum &= 0xff; // n_positional
                        sp += unum;
                        *sp = rt_call_function_n(*sp, unum, sp - unum);
                        break;

                    case PYBC_CALL_METHOD:
                        DECODE_UINT;
                        assert((unum & 0xff00) == 0); // n_keyword
                        unum &= 0xff;
                        obj1 = rt_call_method_n(unum, sp);
                        sp += unum + 1;
                        *sp = obj1;
                        break;

                    case PYBC_RETURN_VALUE:
                        nlr_pop();
                        *sp_in_out = sp;
                        assert(exc_sp == &exc_stack[-1]);
                        return false;

                    case PYBC_YIELD_VALUE:
                        nlr_pop();
                        *ip_in_out = ip;
                        fastn[0] = fast0;
                        fastn[1] = fast1;
                        fastn[2] = fast2;
                        *sp_in_out = sp;
                        return true;

                    default:
                        printf("code %p, byte code 0x%02x not implemented\n", ip, op);
                        assert(0);
                        nlr_pop();
                        return false;
                }
            }

        } else {
            // exception occurred

            if (exc_sp >= &exc_stack[0]) {
                // catch exception and pass to byte code
                sp = (py_obj_t*)(exc_sp[0]);
                ip = (const byte*)(exc_sp[-1]);
                // push(traceback, exc-val, exc-type)
                PUSH(py_const_none);
                PUSH(nlr.ret_val);
                PUSH(py_const_none);
            } else {
                // re-raise exception
                // TODO what to do if this is a generator??
                nlr_jump(nlr.ret_val);
            }
        }
    }
}
