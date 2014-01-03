#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "obj.h"
#include "runtime.h"
#include "bc0.h"
#include "bc.h"

// (value) stack grows down (to be compatible with native code when passing pointers to the stack), top element is pointed to
// exception stack grows up, top element is pointed to

#define DECODE_UINT do { unum = *ip++; if (unum > 127) { unum = ((unum & 0x3f) << 8) | (*ip++); } } while (0)
#define DECODE_ULABEL do { unum = (ip[0] | (ip[1] << 8)); ip += 2; } while (0)
#define DECODE_SLABEL do { unum = (ip[0] | (ip[1] << 8)) - 0x8000; ip += 2; } while (0)
#define DECODE_QSTR do { qstr = *ip++; if (qstr > 127) { qstr = ((qstr & 0x3f) << 8) | (*ip++); } } while (0)
#define PUSH(val) *--sp = (val)
#define POP() (*sp++)
#define TOP() (*sp)
#define SET_TOP(val) *sp = (val)

// args are in reverse order in array
mp_obj_t mp_execute_byte_code(const byte *code, const mp_obj_t *args, uint n_args, uint n_state) {
    mp_obj_t temp_state[10]; // TODO allocate properly
    mp_obj_t *state = &temp_state[0];
    mp_obj_t *sp = &state[10];
    if (n_state > 10) {
        state = m_new(mp_obj_t, n_state);
        sp = &state[n_state];
    }
    // init args
    for (int i = 0; i < n_args; i++) {
        assert(i < 8);
        state[i] = args[n_args - 1 - i];
    }
    const byte *ip = code;

    // execute prelude to make any cells (closed over variables)
    {
        for (uint n_local = *ip++; n_local > 0; n_local--) {
            uint local_num = *ip++;
            if (local_num < n_args) {
                state[local_num] = mp_obj_new_cell(state[local_num]);
            } else {
                state[local_num] = mp_obj_new_cell(MP_OBJ_NULL);
            }
        }
    }

    // execute the byte code
    if (mp_execute_byte_code_2(&ip, &state[0], &sp)) {
        // it shouldn't yield
        assert(0);
    }

    // TODO check fails if, eg, return from within for loop
    //assert(sp == &state[17]);
    return *sp;
}

// fastn has items in normal order
// sp points to top of stack which grows down
bool mp_execute_byte_code_2(const byte **ip_in_out, mp_obj_t *fastn, mp_obj_t **sp_in_out) {
    // careful: be sure to declare volatile any variables read in the exception handler (written is ok, I think)

    const byte *ip = *ip_in_out;
    mp_obj_t *sp = *sp_in_out;
    machine_uint_t unum;
    qstr qstr;
    mp_obj_t obj1, obj2;
    mp_obj_t fast0 = fastn[0], fast1 = fastn[1], fast2 = fastn[2];
    nlr_buf_t nlr;

    volatile machine_uint_t currently_in_except_block = 0; // 0 or 1, to detect nested exceptions
    machine_uint_t exc_stack[8]; // on the exception stack we store (ip, sp | X) for each block, X = previous value of currently_in_except_block
    machine_uint_t *volatile exc_sp = &exc_stack[0] - 1; // stack grows up, exc_sp points to top of stack

    // outer exception handling loop
    for (;;) {
        if (nlr_push(&nlr) == 0) {
            // loop to execute byte code
            for (;;) {
                int op = *ip++;
                switch (op) {
                    case MP_BC_LOAD_CONST_FALSE:
                        PUSH(mp_const_false);
                        break;

                    case MP_BC_LOAD_CONST_NONE:
                        PUSH(mp_const_none);
                        break;

                    case MP_BC_LOAD_CONST_TRUE:
                        PUSH(mp_const_true);
                        break;

                    case MP_BC_LOAD_CONST_SMALL_INT:
                        unum = (ip[0] | (ip[1] << 8) | (ip[2] << 16)) - 0x800000;
                        ip += 3;
                        PUSH((mp_obj_t)(unum << 1 | 1));
                        break;

                    case MP_BC_LOAD_CONST_DEC:
                        DECODE_QSTR;
                        PUSH(rt_load_const_dec(qstr));
                        break;

                    case MP_BC_LOAD_CONST_ID:
                        DECODE_QSTR;
                        PUSH(rt_load_const_str(qstr)); // TODO
                        break;

                    case MP_BC_LOAD_CONST_BYTES:
                        DECODE_QSTR;
                        PUSH(rt_load_const_str(qstr)); // TODO
                        break;

                    case MP_BC_LOAD_CONST_STRING:
                        DECODE_QSTR;
                        PUSH(rt_load_const_str(qstr));
                        break;

                    case MP_BC_LOAD_FAST_0:
                        PUSH(fast0);
                        break;

                    case MP_BC_LOAD_FAST_1:
                        PUSH(fast1);
                        break;

                    case MP_BC_LOAD_FAST_2:
                        PUSH(fast2);
                        break;

                    case MP_BC_LOAD_FAST_N:
                        DECODE_UINT;
                        PUSH(fastn[unum]);
                        break;

                    case MP_BC_LOAD_DEREF:
                        DECODE_UINT;
                        PUSH(rt_get_cell(fastn[unum]));
                        break;

                    case MP_BC_LOAD_NAME:
                        DECODE_QSTR;
                        PUSH(rt_load_name(qstr));
                        break;

                    case MP_BC_LOAD_GLOBAL:
                        DECODE_QSTR;
                        PUSH(rt_load_global(qstr));
                        break;

                    case MP_BC_LOAD_ATTR:
                        DECODE_QSTR;
                        SET_TOP(rt_load_attr(TOP(), qstr));
                        break;

                    case MP_BC_LOAD_METHOD:
                        DECODE_QSTR;
                        sp -= 1;
                        rt_load_method(sp[1], qstr, sp);
                        break;

                    case MP_BC_LOAD_BUILD_CLASS:
                        PUSH(rt_load_build_class());
                        break;

                    case MP_BC_STORE_FAST_0:
                        fast0 = POP();
                        break;

                    case MP_BC_STORE_FAST_1:
                        fast1 = POP();
                        break;

                    case MP_BC_STORE_FAST_2:
                        fast2 = POP();
                        break;

                    case MP_BC_STORE_FAST_N:
                        DECODE_UINT;
                        fastn[unum] = POP();
                        break;

                    case MP_BC_STORE_DEREF:
                        DECODE_UINT;
                        rt_set_cell(fastn[unum], POP());
                        break;

                    case MP_BC_STORE_NAME:
                        DECODE_QSTR;
                        rt_store_name(qstr, POP());
                        break;

                    case MP_BC_STORE_GLOBAL:
                        DECODE_QSTR;
                        rt_store_global(qstr, POP());
                        break;

                    case MP_BC_STORE_ATTR:
                        DECODE_QSTR;
                        rt_store_attr(sp[0], qstr, sp[1]);
                        sp += 2;
                        break;

                    case MP_BC_STORE_SUBSCR:
                        rt_store_subscr(sp[1], sp[0], sp[2]);
                        sp += 3;
                        break;

                    case MP_BC_DUP_TOP:
                        obj1 = TOP();
                        PUSH(obj1);
                        break;

                    case MP_BC_DUP_TOP_TWO:
                        sp -= 2;
                        sp[0] = sp[2];
                        sp[1] = sp[3];
                        break;

                    case MP_BC_POP_TOP:
                        ++sp;
                        break;

                    case MP_BC_ROT_TWO:
                        obj1 = sp[0];
                        sp[0] = sp[1];
                        sp[1] = obj1;
                        break;

                    case MP_BC_ROT_THREE:
                        obj1 = sp[0];
                        sp[0] = sp[1];
                        sp[1] = sp[2];
                        sp[2] = obj1;
                        break;

                    case MP_BC_JUMP:
                        DECODE_SLABEL;
                        ip += unum;
                        break;

                    case MP_BC_POP_JUMP_IF_TRUE:
                        DECODE_SLABEL;
                        if (rt_is_true(POP())) {
                            ip += unum;
                        }
                        break;

                    case MP_BC_POP_JUMP_IF_FALSE:
                        DECODE_SLABEL;
                        if (!rt_is_true(POP())) {
                            ip += unum;
                        }
                        break;

                    case MP_BC_JUMP_IF_TRUE_OR_POP:
                        DECODE_SLABEL;
                        if (rt_is_true(TOP())) {
                            ip += unum;
                        } else {
                            sp++;
                        }
                        break;

                    case MP_BC_JUMP_IF_FALSE_OR_POP:
                        DECODE_SLABEL;
                        if (rt_is_true(TOP())) {
                            sp++;
                        } else {
                            ip += unum;
                        }
                        break;

                        /* we are trying to get away without using this opcode
                    case MP_BC_SETUP_LOOP:
                        DECODE_UINT;
                        // push_block(MP_BC_SETUP_LOOP, ip + unum, sp)
                        break;
                        */

                    // matched against: POP_BLOCK or POP_EXCEPT (anything else?)
                    case MP_BC_SETUP_EXCEPT:
                        DECODE_ULABEL; // except labels are always forward
                        *++exc_sp = (machine_uint_t)ip + unum;
                        *++exc_sp = (((machine_uint_t)sp) | currently_in_except_block);
                        currently_in_except_block = 0; // in a try block now
                        break;

                    case MP_BC_END_FINALLY:
                        // not implemented
                        // if TOS is an exception, reraises the exception (3 values on TOS)
                        // if TOS is an integer, does something else
                        // if TOS is None, just pops it and continues
                        // else error
                        assert(0);
                        break;

                    case MP_BC_GET_ITER:
                        SET_TOP(rt_getiter(TOP()));
                        break;

                    case MP_BC_FOR_ITER:
                        DECODE_ULABEL; // the jump offset if iteration finishes; for labels are always forward
                        obj1 = rt_iternext(TOP());
                        if (obj1 == mp_const_stop_iteration) {
                            ++sp; // pop the exhausted iterator
                            ip += unum; // jump to after for-block
                        } else {
                            PUSH(obj1); // push the next iteration value
                        }
                        break;

                    // matched against: SETUP_EXCEPT, SETUP_FINALLY, SETUP_WITH
                    case MP_BC_POP_BLOCK:
                        // we are exiting an exception handler, so pop the last one of the exception-stack
                        assert(exc_sp >= &exc_stack[0]);
                        currently_in_except_block = (exc_sp[0] & 1); // restore previous state
                        exc_sp -= 2; // pop back to previous exception handler
                        break;

                    // matched againts: SETUP_EXCEPT
                    case MP_BC_POP_EXCEPT:
                        // TODO need to work out how blocks work etc
                        // pops block, checks it's an exception block, and restores the stack, saving the 3 exception values to local threadstate
                        assert(exc_sp >= &exc_stack[0]);
                        //sp = (mp_obj_t*)(*exc_sp--);
                        //exc_sp--; // discard ip
                        currently_in_except_block = (exc_sp[0] & 1); // restore previous state
                        exc_sp -= 2; // pop back to previous exception handler
                        //sp += 3; // pop 3 exception values
                        break;

                    case MP_BC_UNARY_OP:
                        unum = *ip++;
                        SET_TOP(rt_unary_op(unum, TOP()));
                        break;

                    case MP_BC_BINARY_OP:
                        unum = *ip++;
                        obj2 = POP();
                        obj1 = TOP();
                        SET_TOP(rt_binary_op(unum, obj1, obj2));
                        break;

                    case MP_BC_COMPARE_OP:
                        unum = *ip++;
                        obj2 = POP();
                        obj1 = TOP();
                        SET_TOP(rt_compare_op(unum, obj1, obj2));
                        break;

                    case MP_BC_BUILD_TUPLE:
                        DECODE_UINT;
                        obj1 = rt_build_tuple(unum, sp);
                        sp += unum - 1;
                        SET_TOP(obj1);
                        break;

                    case MP_BC_BUILD_LIST:
                        DECODE_UINT;
                        obj1 = rt_build_list(unum, sp);
                        sp += unum - 1;
                        SET_TOP(obj1);
                        break;

                    case MP_BC_LIST_APPEND:
                        DECODE_UINT;
                        // I think it's guaranteed by the compiler that sp[unum] is a list
                        rt_list_append(sp[unum], sp[0]);
                        sp++;
                        break;

                    case MP_BC_BUILD_MAP:
                        DECODE_UINT;
                        PUSH(rt_build_map(unum));
                        break;

                    case MP_BC_STORE_MAP:
                        sp += 2;
                        rt_store_map(sp[0], sp[-2], sp[-1]);
                        break;

                    case MP_BC_MAP_ADD:
                        DECODE_UINT;
                        // I think it's guaranteed by the compiler that sp[unum + 1] is a map
                        rt_store_map(sp[unum + 1], sp[0], sp[1]);
                        sp += 2;
                        break;

                    case MP_BC_BUILD_SET:
                        DECODE_UINT;
                        obj1 = rt_build_set(unum, sp);
                        sp += unum - 1;
                        SET_TOP(obj1);
                        break;

                    case MP_BC_SET_ADD:
                        DECODE_UINT;
                        // I think it's guaranteed by the compiler that sp[unum] is a set
                        rt_store_set(sp[unum], sp[0]);
                        sp++;
                        break;

                    case MP_BC_BUILD_SLICE:
                        DECODE_UINT;
                        if (unum == 2) {
                            obj2 = POP();
                            obj1 = TOP();
                            SET_TOP(mp_obj_new_slice(obj1, obj2, NULL));
                        } else {
                            printf("3-argument slice is not supported\n");
                            assert(0);
                        }
                        break;

                    case MP_BC_UNPACK_SEQUENCE:
                        DECODE_UINT;
                        rt_unpack_sequence(sp[0], unum, sp - unum + 1);
                        sp -= unum - 1;
                        break;

                    case MP_BC_MAKE_FUNCTION:
                        DECODE_UINT;
                        PUSH(rt_make_function_from_id(unum));
                        break;

                    case MP_BC_MAKE_CLOSURE:
                        DECODE_UINT;
                        obj1 = POP();
                        PUSH(rt_make_closure_from_id(unum, obj1));
                        break;

                    case MP_BC_CALL_FUNCTION:
                        DECODE_UINT;
                        assert((unum & 0xff00) == 0); // n_keyword
                        unum &= 0xff; // n_positional
                        sp += unum;
                        *sp = rt_call_function_n(*sp, unum, sp - unum);
                        break;

                    case MP_BC_CALL_METHOD:
                        DECODE_UINT;
                        if ((unum & 0xff00) == 0) {
                            // no keywords
                            unum &= 0xff;
                            obj1 = rt_call_method_n(unum, sp);
                            sp += unum + 1;
                        } else {
                            // keywords
                            obj1 = rt_call_method_n_kw(unum & 0xff, (unum >> 8) & 0xff, sp);
                            sp += (unum & 0xff) + ((unum >> 7) & 0x1fe) + 1;
                        }
                        SET_TOP(obj1);
                        break;

                    case MP_BC_RETURN_VALUE:
                        nlr_pop();
                        *sp_in_out = sp;
                        assert(exc_sp == &exc_stack[0] - 1);
                        return false;

                    case MP_BC_YIELD_VALUE:
                        nlr_pop();
                        *ip_in_out = ip;
                        fastn[0] = fast0;
                        fastn[1] = fast1;
                        fastn[2] = fast2;
                        *sp_in_out = sp;
                        return true;

                    case MP_BC_IMPORT_NAME:
                        DECODE_QSTR;
                        obj1 = POP();
                        SET_TOP(rt_import_name(qstr, obj1, TOP()));
                        break;

                    case MP_BC_IMPORT_FROM:
                        DECODE_QSTR;
                        obj1 = rt_import_from(TOP(), qstr);
                        PUSH(obj1);
                        break;

                    default:
                        printf("code %p, byte code 0x%02x not implemented\n", ip, op);
                        assert(0);
                        nlr_pop();
                        return false;
                }
            }

        } else {
            // exception occurred

            while (currently_in_except_block) {
                // nested exception

                assert(exc_sp >= &exc_stack[0]);

                // TODO make a proper message for nested exception
                // at the moment we are just raising the very last exception (the one that caused the nested exception)

                // move up to previous exception handler
                currently_in_except_block = (exc_sp[0] & 1); // restore previous state
                exc_sp -= 2; // pop back to previous exception handler
            }

            if (exc_sp >= &exc_stack[0]) {
                // set flag to indicate that we are now handling an exception
                currently_in_except_block = 1;

                // catch exception and pass to byte code
                sp = (mp_obj_t*)(exc_sp[0] & (~((machine_uint_t)1)));
                ip = (const byte*)(exc_sp[-1]);
                // push(traceback, exc-val, exc-type)
                PUSH(mp_const_none);
                PUSH(nlr.ret_val);
                PUSH(nlr.ret_val); // TODO should be type(nlr.ret_val), I think...

            } else {
                // re-raise exception to higher level
                // TODO what to do if this is a generator??
                nlr_jump(nlr.ret_val);
            }
        }
    }
}
