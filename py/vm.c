#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "mpyconfig.h"
#include "runtime.h"
#include "bc.h"

#define DECODE_UINT do { unum = *ip++; if (unum > 127) { unum = ((unum & 0x3f) << 8) | (*ip++); } } while (0)
#define DECODE_QSTR do { qstr = *ip++; if (qstr > 127) { qstr = ((qstr & 0x3f) << 8) | (*ip++); } } while (0)
#define PUSH(val) *--sp = (val)
#define POP() (*sp++)

// args are in reverse order in array
py_obj_t py_execute_byte_code(const byte *code, uint len, const py_obj_t *args, uint n_args) {
    const byte *ip = code;
    py_obj_t stack[10];
    py_obj_t *sp = &stack[10]; // stack grows down, sp points to top of stack
    machine_uint_t unum;
    machine_int_t snum;
    qstr qstr;
    py_obj_t obj1, obj2;
    py_obj_t fast0 = NULL, fast1 = NULL, fast2 = NULL, fastn[4] = {NULL, NULL, NULL, NULL};

    // init args
    for (int i = 0; i < n_args; i++) {
        if (i == 0) {
            fast0 = args[n_args - 1];
        } else if (i == 1) {
            fast1 = args[n_args - 2];
        } else if (i == 2) {
            fast2 = args[n_args - 3];
        } else {
            assert(i - 3 < 4);
            fastn[i - 3] = args[n_args - 1 - i];
        }
    }

    // execute byte code
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
                snum = ip[0] | (ip[1] << 8);
                if (snum & 0x8000) {
                    snum |= ~0xffff;
                }
                ip += 2;
                PUSH((py_obj_t)(snum << 1 | 1));
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
                PUSH(fastn[unum - 3]);
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
                fastn[unum - 3] = POP();
                break;

            case PYBC_STORE_NAME:
                DECODE_QSTR;
                rt_store_name(qstr, POP());
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

            case PYBC_ROT_THREE:
                obj1 = sp[0];
                sp[0] = sp[1];
                sp[1] = sp[2];
                sp[2] = obj1;
                break;

            case PYBC_JUMP:
                DECODE_UINT;
                ip = code + unum;
                break;

            case PYBC_POP_JUMP_IF_FALSE:
                DECODE_UINT;
                if (!rt_is_true(POP())) {
                    ip = code + unum;
                }
                break;

            case PYBC_SETUP_LOOP:
                DECODE_UINT;
                break;

            case PYBC_POP_BLOCK:
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

            case PYBC_BUILD_LIST:
                DECODE_UINT;
                obj1 = rt_build_list(unum, sp);
                sp += unum - 1;
                *sp = obj1;
                break;

            case PYBC_BUILD_MAP:
                DECODE_UINT;
                PUSH(rt_build_map(unum));
                break;

            case PYBC_STORE_MAP:
                sp += 2;
                rt_store_map(sp[0], sp[-2], sp[-1]);
                break;

            case PYBC_BUILD_SET:
                DECODE_UINT;
                obj1 = rt_build_set(unum, sp);
                sp += unum - 1;
                *sp = obj1;
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
                return *sp;

            default:
                printf("code %p, offset %u, byte code 0x%02x not implemented\n", code, (uint)(ip - code), op);
                assert(0);
                return py_const_none;
        }
    }
}
