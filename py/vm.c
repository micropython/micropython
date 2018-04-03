/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "py/emitglue.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "py/bc0.h"
#include "py/bc.h"

#if 0
#define TRACE(ip) printf("sp=%d ", (int)(sp - &code_state->state[0] + 1)); mp_bytecode_print2(ip, 1, code_state->fun_bc->const_table);
#else
#define TRACE(ip)
#endif

// Value stack grows up (this makes it incompatible with native C stack, but
// makes sure that arguments to functions are in natural order arg1..argN
// (Python semantics mandates left-to-right evaluation order, including for
// function arguments). Stack pointer is pre-incremented and points at the
// top element.
// Exception stack also grows up, top element is also pointed at.

#define DECODE_UINT \
    mp_uint_t unum = 0; \
    do { \
        unum = (unum << 7) + (*ip & 0x7f); \
    } while ((*ip++ & 0x80) != 0)
#define DECODE_ULABEL size_t ulab = (ip[0] | (ip[1] << 8)); ip += 2
#define DECODE_SLABEL size_t slab = (ip[0] | (ip[1] << 8)) - 0x8000; ip += 2

#if MICROPY_PERSISTENT_CODE

#define DECODE_QSTR \
    qstr qst = ip[0] | ip[1] << 8; \
    ip += 2;
#define DECODE_PTR \
    DECODE_UINT; \
    void *ptr = (void*)(uintptr_t)code_state->fun_bc->const_table[unum]
#define DECODE_OBJ \
    DECODE_UINT; \
    mp_obj_t obj = (mp_obj_t)code_state->fun_bc->const_table[unum]

#else

#define DECODE_QSTR qstr qst = 0; \
    do { \
        qst = (qst << 7) + (*ip & 0x7f); \
    } while ((*ip++ & 0x80) != 0)
#define DECODE_PTR \
    ip = (byte*)MP_ALIGN(ip, sizeof(void*)); \
    void *ptr = *(void**)ip; \
    ip += sizeof(void*)
#define DECODE_OBJ \
    ip = (byte*)MP_ALIGN(ip, sizeof(mp_obj_t)); \
    mp_obj_t obj = *(mp_obj_t*)ip; \
    ip += sizeof(mp_obj_t)

#endif

#define PUSH(val) *++sp = (val)
#define POP() (*sp--)
#define TOP() (*sp)
#define SET_TOP(val) *sp = (val)

#if MICROPY_PY_SYS_EXC_INFO
#define CLEAR_SYS_EXC_INFO() MP_STATE_VM(cur_exception) = NULL;
#else
#define CLEAR_SYS_EXC_INFO()
#endif

#define PUSH_EXC_BLOCK(with_or_finally) do { \
    DECODE_ULABEL; /* except labels are always forward */ \
    ++exc_sp; \
    exc_sp->handler = ip + ulab; \
    exc_sp->val_sp = MP_TAGPTR_MAKE(sp, ((with_or_finally) << 1) | currently_in_except_block); \
    exc_sp->prev_exc = NULL; \
    currently_in_except_block = 0; /* in a try block now */ \
} while (0)

#define POP_EXC_BLOCK() \
    currently_in_except_block = MP_TAGPTR_TAG0(exc_sp->val_sp); /* restore previous state */ \
    exc_sp--; /* pop back to previous exception handler */ \
    CLEAR_SYS_EXC_INFO() /* just clear sys.exc_info(), not compliant, but it shouldn't be used in 1st place */

// fastn has items in reverse order (fastn[0] is local[0], fastn[-1] is local[1], etc)
// sp points to bottom of stack which grows up
// returns:
//  MP_VM_RETURN_NORMAL, sp valid, return value in *sp
//  MP_VM_RETURN_YIELD, ip, sp valid, yielded value in *sp
//  MP_VM_RETURN_EXCEPTION, exception in fastn[0]
mp_vm_return_kind_t mp_execute_bytecode(mp_code_state_t *code_state, volatile mp_obj_t inject_exc) {
#define SELECTIVE_EXC_IP (0)
#if SELECTIVE_EXC_IP
#define MARK_EXC_IP_SELECTIVE() { code_state->ip = ip; } /* stores ip 1 byte past last opcode */
#define MARK_EXC_IP_GLOBAL()
#else
#define MARK_EXC_IP_SELECTIVE()
#define MARK_EXC_IP_GLOBAL() { code_state->ip = ip; } /* stores ip pointing to last opcode */
#endif
#if MICROPY_OPT_COMPUTED_GOTO
    #include "py/vmentrytable.h"
    #define DISPATCH() do { \
        TRACE(ip); \
        MARK_EXC_IP_GLOBAL(); \
        goto *entry_table[*ip++]; \
    } while (0)
    #define DISPATCH_WITH_PEND_EXC_CHECK() goto pending_exception_check
    #define ENTRY(op) entry_##op
    #define ENTRY_DEFAULT entry_default
#else
    #define DISPATCH() break
    #define DISPATCH_WITH_PEND_EXC_CHECK() goto pending_exception_check
    #define ENTRY(op) case op
    #define ENTRY_DEFAULT default
#endif

    // nlr_raise needs to be implemented as a goto, so that the C compiler's flow analyser
    // sees that it's possible for us to jump from the dispatch loop to the exception
    // handler.  Without this, the code may have a different stack layout in the dispatch
    // loop and the exception handler, leading to very obscure bugs.
    #define RAISE(o) do { nlr_pop(); nlr.ret_val = MP_OBJ_TO_PTR(o); goto exception_handler; } while (0)

#if MICROPY_STACKLESS
run_code_state: ;
#endif
    // Pointers which are constant for particular invocation of mp_execute_bytecode()
    mp_obj_t * /*const*/ fastn;
    mp_exc_stack_t * /*const*/ exc_stack;
    {
        size_t n_state = mp_decode_uint_value(code_state->fun_bc->bytecode);
        fastn = &code_state->state[n_state - 1];
        exc_stack = (mp_exc_stack_t*)(code_state->state + n_state);
    }

    // variables that are visible to the exception handler (declared volatile)
    volatile bool currently_in_except_block = MP_TAGPTR_TAG0(code_state->exc_sp); // 0 or 1, to detect nested exceptions
    mp_exc_stack_t *volatile exc_sp = MP_TAGPTR_PTR(code_state->exc_sp); // stack grows up, exc_sp points to top of stack

    #if MICROPY_PY_THREAD_GIL && MICROPY_PY_THREAD_GIL_VM_DIVISOR
    // This needs to be volatile and outside the VM loop so it persists across handling
    // of any exceptions.  Otherwise it's possible that the VM never gives up the GIL.
    volatile int gil_divisor = MICROPY_PY_THREAD_GIL_VM_DIVISOR;
    #endif

    // outer exception handling loop
    for (;;) {
        nlr_buf_t nlr;
outer_dispatch_loop:
        if (nlr_push(&nlr) == 0) {
            // local variables that are not visible to the exception handler
            const byte *ip = code_state->ip;
            mp_obj_t *sp = code_state->sp;
            mp_obj_t obj_shared;
            MICROPY_VM_HOOK_INIT

            // If we have exception to inject, now that we finish setting up
            // execution context, raise it. This works as if RAISE_VARARGS
            // bytecode was executed.
            // Injecting exc into yield from generator is a special case,
            // handled by MP_BC_YIELD_FROM itself
            if (inject_exc != MP_OBJ_NULL && *ip != MP_BC_YIELD_FROM) {
                mp_obj_t exc = inject_exc;
                inject_exc = MP_OBJ_NULL;
                exc = mp_make_raise_obj(exc);
                RAISE(exc);
            }

            // loop to execute byte code
            for (;;) {
dispatch_loop:
#if MICROPY_OPT_COMPUTED_GOTO
                DISPATCH();
#else
                TRACE(ip);
                MARK_EXC_IP_GLOBAL();
                switch (*ip++) {
#endif

                ENTRY(MP_BC_LOAD_CONST_FALSE):
                    PUSH(mp_const_false);
                    DISPATCH();

                ENTRY(MP_BC_LOAD_CONST_NONE):
                    PUSH(mp_const_none);
                    DISPATCH();

                ENTRY(MP_BC_LOAD_CONST_TRUE):
                    PUSH(mp_const_true);
                    DISPATCH();

                ENTRY(MP_BC_LOAD_CONST_SMALL_INT): {
                    mp_int_t num = 0;
                    if ((ip[0] & 0x40) != 0) {
                        // Number is negative
                        num--;
                    }
                    do {
                        num = (num << 7) | (*ip & 0x7f);
                    } while ((*ip++ & 0x80) != 0);
                    PUSH(MP_OBJ_NEW_SMALL_INT(num));
                    DISPATCH();
                }

                ENTRY(MP_BC_LOAD_CONST_STRING): {
                    DECODE_QSTR;
                    PUSH(MP_OBJ_NEW_QSTR(qst));
                    DISPATCH();
                }

                ENTRY(MP_BC_LOAD_CONST_OBJ): {
                    DECODE_OBJ;
                    PUSH(obj);
                    DISPATCH();
                }

                ENTRY(MP_BC_LOAD_NULL):
                    PUSH(MP_OBJ_NULL);
                    DISPATCH();

                ENTRY(MP_BC_LOAD_FAST_N): {
                    DECODE_UINT;
                    obj_shared = fastn[-unum];
                    load_check:
                    if (obj_shared == MP_OBJ_NULL) {
                        local_name_error: {
                            MARK_EXC_IP_SELECTIVE();
                            mp_obj_t obj = mp_obj_new_exception_msg(&mp_type_NameError, "local variable referenced before assignment");
                            RAISE(obj);
                        }
                    }
                    PUSH(obj_shared);
                    DISPATCH();
                }

                ENTRY(MP_BC_LOAD_DEREF): {
                    DECODE_UINT;
                    obj_shared = mp_obj_cell_get(fastn[-unum]);
                    goto load_check;
                }

                #if !MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE
                ENTRY(MP_BC_LOAD_NAME): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    PUSH(mp_load_name(qst));
                    DISPATCH();
                }
                #else
                ENTRY(MP_BC_LOAD_NAME): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    mp_obj_t key = MP_OBJ_NEW_QSTR(qst);
                    mp_uint_t x = *ip;
                    if (x < mp_locals_get()->map.alloc && mp_locals_get()->map.table[x].key == key) {
                        PUSH(mp_locals_get()->map.table[x].value);
                    } else {
                        mp_map_elem_t *elem = mp_map_lookup(&mp_locals_get()->map, MP_OBJ_NEW_QSTR(qst), MP_MAP_LOOKUP);
                        if (elem != NULL) {
                            *(byte*)ip = (elem - &mp_locals_get()->map.table[0]) & 0xff;
                            PUSH(elem->value);
                        } else {
                            PUSH(mp_load_name(MP_OBJ_QSTR_VALUE(key)));
                        }
                    }
                    ip++;
                    DISPATCH();
                }
                #endif

                #if !MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE
                ENTRY(MP_BC_LOAD_GLOBAL): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    PUSH(mp_load_global(qst));
                    DISPATCH();
                }
                #else
                ENTRY(MP_BC_LOAD_GLOBAL): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    mp_obj_t key = MP_OBJ_NEW_QSTR(qst);
                    mp_uint_t x = *ip;
                    if (x < mp_globals_get()->map.alloc && mp_globals_get()->map.table[x].key == key) {
                        PUSH(mp_globals_get()->map.table[x].value);
                    } else {
                        mp_map_elem_t *elem = mp_map_lookup(&mp_globals_get()->map, MP_OBJ_NEW_QSTR(qst), MP_MAP_LOOKUP);
                        if (elem != NULL) {
                            *(byte*)ip = (elem - &mp_globals_get()->map.table[0]) & 0xff;
                            PUSH(elem->value);
                        } else {
                            PUSH(mp_load_global(MP_OBJ_QSTR_VALUE(key)));
                        }
                    }
                    ip++;
                    DISPATCH();
                }
                #endif

                #if !MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE
                ENTRY(MP_BC_LOAD_ATTR): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    SET_TOP(mp_load_attr(TOP(), qst));
                    DISPATCH();
                }
                #else
                ENTRY(MP_BC_LOAD_ATTR): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    mp_obj_t top = TOP();
                    if (mp_obj_get_type(top)->attr == mp_obj_instance_attr) {
                        mp_obj_instance_t *self = MP_OBJ_TO_PTR(top);
                        mp_uint_t x = *ip;
                        mp_obj_t key = MP_OBJ_NEW_QSTR(qst);
                        mp_map_elem_t *elem;
                        if (x < self->members.alloc && self->members.table[x].key == key) {
                            elem = &self->members.table[x];
                        } else {
                            elem = mp_map_lookup(&self->members, key, MP_MAP_LOOKUP);
                            if (elem != NULL) {
                                *(byte*)ip = elem - &self->members.table[0];
                            } else {
                                goto load_attr_cache_fail;
                            }
                        }
                        SET_TOP(elem->value);
                        ip++;
                        DISPATCH();
                    }
                load_attr_cache_fail:
                    SET_TOP(mp_load_attr(top, qst));
                    ip++;
                    DISPATCH();
                }
                #endif

                ENTRY(MP_BC_LOAD_METHOD): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    mp_load_method(*sp, qst, sp);
                    sp += 1;
                    DISPATCH();
                }

                ENTRY(MP_BC_LOAD_SUPER_METHOD): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    sp -= 1;
                    mp_load_super_method(qst, sp - 1);
                    DISPATCH();
                }

                ENTRY(MP_BC_LOAD_BUILD_CLASS):
                    MARK_EXC_IP_SELECTIVE();
                    PUSH(mp_load_build_class());
                    DISPATCH();

                ENTRY(MP_BC_LOAD_SUBSCR): {
                    MARK_EXC_IP_SELECTIVE();
                    mp_obj_t index = POP();
                    SET_TOP(mp_obj_subscr(TOP(), index, MP_OBJ_SENTINEL));
                    DISPATCH();
                }

                ENTRY(MP_BC_STORE_FAST_N): {
                    DECODE_UINT;
                    fastn[-unum] = POP();
                    DISPATCH();
                }

                ENTRY(MP_BC_STORE_DEREF): {
                    DECODE_UINT;
                    mp_obj_cell_set(fastn[-unum], POP());
                    DISPATCH();
                }

                ENTRY(MP_BC_STORE_NAME): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    mp_store_name(qst, POP());
                    DISPATCH();
                }

                ENTRY(MP_BC_STORE_GLOBAL): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    mp_store_global(qst, POP());
                    DISPATCH();
                }

                #if !MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE
                ENTRY(MP_BC_STORE_ATTR): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    mp_store_attr(sp[0], qst, sp[-1]);
                    sp -= 2;
                    DISPATCH();
                }
                #else
                // This caching code works with MICROPY_PY_BUILTINS_PROPERTY and/or
                // MICROPY_PY_DESCRIPTORS enabled because if the attr exists in
                // self->members then it can't be a property or have descriptors.  A
                // consequence of this is that we can't use MP_MAP_LOOKUP_ADD_IF_NOT_FOUND
                // in the fast-path below, because that store could override a property.
                ENTRY(MP_BC_STORE_ATTR): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    mp_obj_t top = TOP();
                    if (mp_obj_get_type(top)->attr == mp_obj_instance_attr && sp[-1] != MP_OBJ_NULL) {
                        mp_obj_instance_t *self = MP_OBJ_TO_PTR(top);
                        mp_uint_t x = *ip;
                        mp_obj_t key = MP_OBJ_NEW_QSTR(qst);
                        mp_map_elem_t *elem;
                        if (x < self->members.alloc && self->members.table[x].key == key) {
                            elem = &self->members.table[x];
                        } else {
                            elem = mp_map_lookup(&self->members, key, MP_MAP_LOOKUP);
                            if (elem != NULL) {
                                *(byte*)ip = elem - &self->members.table[0];
                            } else {
                                goto store_attr_cache_fail;
                            }
                        }
                        elem->value = sp[-1];
                        sp -= 2;
                        ip++;
                        DISPATCH();
                    }
                store_attr_cache_fail:
                    mp_store_attr(sp[0], qst, sp[-1]);
                    sp -= 2;
                    ip++;
                    DISPATCH();
                }
                #endif

                ENTRY(MP_BC_STORE_SUBSCR):
                    MARK_EXC_IP_SELECTIVE();
                    mp_obj_subscr(sp[-1], sp[0], sp[-2]);
                    sp -= 3;
                    DISPATCH();

                ENTRY(MP_BC_DELETE_FAST): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    if (fastn[-unum] == MP_OBJ_NULL) {
                        goto local_name_error;
                    }
                    fastn[-unum] = MP_OBJ_NULL;
                    DISPATCH();
                }

                ENTRY(MP_BC_DELETE_DEREF): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    if (mp_obj_cell_get(fastn[-unum]) == MP_OBJ_NULL) {
                        goto local_name_error;
                    }
                    mp_obj_cell_set(fastn[-unum], MP_OBJ_NULL);
                    DISPATCH();
                }

                ENTRY(MP_BC_DELETE_NAME): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    mp_delete_name(qst);
                    DISPATCH();
                }

                ENTRY(MP_BC_DELETE_GLOBAL): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    mp_delete_global(qst);
                    DISPATCH();
                }

                ENTRY(MP_BC_DUP_TOP): {
                    mp_obj_t top = TOP();
                    PUSH(top);
                    DISPATCH();
                }

                ENTRY(MP_BC_DUP_TOP_TWO):
                    sp += 2;
                    sp[0] = sp[-2];
                    sp[-1] = sp[-3];
                    DISPATCH();

                ENTRY(MP_BC_POP_TOP):
                    sp -= 1;
                    DISPATCH();

                ENTRY(MP_BC_ROT_TWO): {
                    mp_obj_t top = sp[0];
                    sp[0] = sp[-1];
                    sp[-1] = top;
                    DISPATCH();
                }

                ENTRY(MP_BC_ROT_THREE): {
                    mp_obj_t top = sp[0];
                    sp[0] = sp[-1];
                    sp[-1] = sp[-2];
                    sp[-2] = top;
                    DISPATCH();
                }

                ENTRY(MP_BC_JUMP): {
                    DECODE_SLABEL;
                    ip += slab;
                    DISPATCH_WITH_PEND_EXC_CHECK();
                }

                ENTRY(MP_BC_POP_JUMP_IF_TRUE): {
                    DECODE_SLABEL;
                    if (mp_obj_is_true(POP())) {
                        ip += slab;
                    }
                    DISPATCH_WITH_PEND_EXC_CHECK();
                }

                ENTRY(MP_BC_POP_JUMP_IF_FALSE): {
                    DECODE_SLABEL;
                    if (!mp_obj_is_true(POP())) {
                        ip += slab;
                    }
                    DISPATCH_WITH_PEND_EXC_CHECK();
                }

                ENTRY(MP_BC_JUMP_IF_TRUE_OR_POP): {
                    DECODE_SLABEL;
                    if (mp_obj_is_true(TOP())) {
                        ip += slab;
                    } else {
                        sp--;
                    }
                    DISPATCH_WITH_PEND_EXC_CHECK();
                }

                ENTRY(MP_BC_JUMP_IF_FALSE_OR_POP): {
                    DECODE_SLABEL;
                    if (mp_obj_is_true(TOP())) {
                        sp--;
                    } else {
                        ip += slab;
                    }
                    DISPATCH_WITH_PEND_EXC_CHECK();
                }

                ENTRY(MP_BC_SETUP_WITH): {
                    MARK_EXC_IP_SELECTIVE();
                    // stack: (..., ctx_mgr)
                    mp_obj_t obj = TOP();
                    mp_load_method(obj, MP_QSTR___exit__, sp);
                    mp_load_method(obj, MP_QSTR___enter__, sp + 2);
                    mp_obj_t ret = mp_call_method_n_kw(0, 0, sp + 2);
                    sp += 1;
                    PUSH_EXC_BLOCK(1);
                    PUSH(ret);
                    // stack: (..., __exit__, ctx_mgr, as_value)
                    DISPATCH();
                }

                ENTRY(MP_BC_WITH_CLEANUP): {
                    MARK_EXC_IP_SELECTIVE();
                    // Arriving here, there's "exception control block" on top of stack,
                    // and __exit__ method (with self) underneath it. Bytecode calls __exit__,
                    // and "deletes" it off stack, shifting "exception control block"
                    // to its place.
                    // The bytecode emitter ensures that there is enough space on the Python
                    // value stack to hold the __exit__ method plus an additional 4 entries.
                    if (TOP() == mp_const_none) {
                        // stack: (..., __exit__, ctx_mgr, None)
                        sp[1] = mp_const_none;
                        sp[2] = mp_const_none;
                        sp -= 2;
                        mp_call_method_n_kw(3, 0, sp);
                        SET_TOP(mp_const_none);
                    } else if (MP_OBJ_IS_SMALL_INT(TOP())) {
                        // Getting here there are two distinct cases:
                        //  - unwind return, stack: (..., __exit__, ctx_mgr, ret_val, SMALL_INT(-1))
                        //  - unwind jump, stack:   (..., __exit__, ctx_mgr, dest_ip, SMALL_INT(num_exc))
                        // For both cases we do exactly the same thing.
                        mp_obj_t data = sp[-1];
                        mp_obj_t cause = sp[0];
                        sp[-1] = mp_const_none;
                        sp[0] = mp_const_none;
                        sp[1] = mp_const_none;
                        mp_call_method_n_kw(3, 0, sp - 3);
                        sp[-3] = data;
                        sp[-2] = cause;
                        sp -= 2; // we removed (__exit__, ctx_mgr)
                    } else {
                        assert(mp_obj_is_exception_instance(TOP()));
                        // stack: (..., __exit__, ctx_mgr, exc_instance)
                        // Need to pass (exc_type, exc_instance, None) as arguments to __exit__.
                        sp[1] = sp[0];
                        sp[0] = MP_OBJ_FROM_PTR(mp_obj_get_type(sp[0]));
                        sp[2] = mp_const_none;
                        sp -= 2;
                        mp_obj_t ret_value = mp_call_method_n_kw(3, 0, sp);
                        if (mp_obj_is_true(ret_value)) {
                            // We need to silence/swallow the exception.  This is done
                            // by popping the exception and the __exit__ handler and
                            // replacing it with None, which signals END_FINALLY to just
                            // execute the finally handler normally.
                            SET_TOP(mp_const_none);
                            assert(exc_sp >= exc_stack);
                            POP_EXC_BLOCK();
                        } else {
                            // We need to re-raise the exception.  We pop __exit__ handler
                            // by copying the exception instance down to the new top-of-stack.
                            sp[0] = sp[3];
                        }
                    }
                    DISPATCH();
                }

                ENTRY(MP_BC_UNWIND_JUMP): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_SLABEL;
                    PUSH((mp_obj_t)(mp_uint_t)(uintptr_t)(ip + slab)); // push destination ip for jump
                    PUSH((mp_obj_t)(mp_uint_t)(*ip)); // push number of exception handlers to unwind (0x80 bit set if we also need to pop stack)
unwind_jump:;
                    mp_uint_t unum = (mp_uint_t)POP(); // get number of exception handlers to unwind
                    while ((unum & 0x7f) > 0) {
                        unum -= 1;
                        assert(exc_sp >= exc_stack);
                        if (MP_TAGPTR_TAG1(exc_sp->val_sp)) {
                            // Getting here the stack looks like:
                            //     (..., X, dest_ip)
                            // where X is pointed to by exc_sp->val_sp and in the case
                            // of a "with" block contains the context manager info.
                            // We're going to run "finally" code as a coroutine
                            // (not calling it recursively). Set up a sentinel
                            // on the stack so it can return back to us when it is
                            // done (when WITH_CLEANUP or END_FINALLY reached).
                            // The sentinel is the number of exception handlers left to
                            // unwind, which is a non-negative integer.
                            PUSH(MP_OBJ_NEW_SMALL_INT(unum));
                            ip = exc_sp->handler; // get exception handler byte code address
                            exc_sp--; // pop exception handler
                            goto dispatch_loop; // run the exception handler
                        }
                        POP_EXC_BLOCK();
                    }
                    ip = (const byte*)MP_OBJ_TO_PTR(POP()); // pop destination ip for jump
                    if (unum != 0) {
                        // pop the exhausted iterator
                        sp -= MP_OBJ_ITER_BUF_NSLOTS;
                    }
                    DISPATCH_WITH_PEND_EXC_CHECK();
                }

                // matched against: POP_BLOCK or POP_EXCEPT (anything else?)
                ENTRY(MP_BC_SETUP_EXCEPT):
                ENTRY(MP_BC_SETUP_FINALLY): {
                    MARK_EXC_IP_SELECTIVE();
                    #if SELECTIVE_EXC_IP
                    PUSH_EXC_BLOCK((code_state->ip[-1] == MP_BC_SETUP_FINALLY) ? 1 : 0);
                    #else
                    PUSH_EXC_BLOCK((code_state->ip[0] == MP_BC_SETUP_FINALLY) ? 1 : 0);
                    #endif
                    DISPATCH();
                }

                ENTRY(MP_BC_END_FINALLY):
                    MARK_EXC_IP_SELECTIVE();
                    // if TOS is None, just pops it and continues
                    // if TOS is an integer, finishes coroutine and returns control to caller
                    // if TOS is an exception, reraises the exception
                    if (TOP() == mp_const_none) {
                        sp--;
                    } else if (MP_OBJ_IS_SMALL_INT(TOP())) {
                        // We finished "finally" coroutine and now dispatch back
                        // to our caller, based on TOS value
                        mp_int_t cause = MP_OBJ_SMALL_INT_VALUE(POP());
                        if (cause < 0) {
                            // A negative cause indicates unwind return
                            goto unwind_return;
                        } else {
                            // Otherwise it's an unwind jump and we must push as a raw
                            // number the number of exception handlers to unwind
                            PUSH((mp_obj_t)cause);
                            goto unwind_jump;
                        }
                    } else {
                        assert(mp_obj_is_exception_instance(TOP()));
                        RAISE(TOP());
                    }
                    DISPATCH();

                ENTRY(MP_BC_GET_ITER):
                    MARK_EXC_IP_SELECTIVE();
                    SET_TOP(mp_getiter(TOP(), NULL));
                    DISPATCH();

                // An iterator for a for-loop takes MP_OBJ_ITER_BUF_NSLOTS slots on
                // the Python value stack.  These slots are either used to store the
                // iterator object itself, or the first slot is MP_OBJ_NULL and
                // the second slot holds a reference to the iterator object.
                ENTRY(MP_BC_GET_ITER_STACK): {
                    MARK_EXC_IP_SELECTIVE();
                    mp_obj_t obj = TOP();
                    mp_obj_iter_buf_t *iter_buf = (mp_obj_iter_buf_t*)sp;
                    sp += MP_OBJ_ITER_BUF_NSLOTS - 1;
                    obj = mp_getiter(obj, iter_buf);
                    if (obj != MP_OBJ_FROM_PTR(iter_buf)) {
                        // Iterator didn't use the stack so indicate that with MP_OBJ_NULL.
                        sp[-MP_OBJ_ITER_BUF_NSLOTS + 1] = MP_OBJ_NULL;
                        sp[-MP_OBJ_ITER_BUF_NSLOTS + 2] = obj;
                    }
                    DISPATCH();
                }

                ENTRY(MP_BC_FOR_ITER): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_ULABEL; // the jump offset if iteration finishes; for labels are always forward
                    code_state->sp = sp;
                    mp_obj_t obj;
                    if (sp[-MP_OBJ_ITER_BUF_NSLOTS + 1] == MP_OBJ_NULL) {
                        obj = sp[-MP_OBJ_ITER_BUF_NSLOTS + 2];
                    } else {
                        obj = MP_OBJ_FROM_PTR(&sp[-MP_OBJ_ITER_BUF_NSLOTS + 1]);
                    }
                    mp_obj_t value = mp_iternext_allow_raise(obj);
                    if (value == MP_OBJ_STOP_ITERATION) {
                        sp -= MP_OBJ_ITER_BUF_NSLOTS; // pop the exhausted iterator
                        ip += ulab; // jump to after for-block
                    } else {
                        PUSH(value); // push the next iteration value
                    }
                    DISPATCH();
                }

                // matched against: SETUP_EXCEPT, SETUP_FINALLY, SETUP_WITH
                ENTRY(MP_BC_POP_BLOCK):
                    // we are exiting an exception handler, so pop the last one of the exception-stack
                    assert(exc_sp >= exc_stack);
                    POP_EXC_BLOCK();
                    DISPATCH();

                // matched against: SETUP_EXCEPT
                ENTRY(MP_BC_POP_EXCEPT):
                    assert(exc_sp >= exc_stack);
                    assert(currently_in_except_block);
                    POP_EXC_BLOCK();
                    DISPATCH();

                ENTRY(MP_BC_BUILD_TUPLE): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    sp -= unum - 1;
                    SET_TOP(mp_obj_new_tuple(unum, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_BUILD_LIST): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    sp -= unum - 1;
                    SET_TOP(mp_obj_new_list(unum, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_BUILD_MAP): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    PUSH(mp_obj_new_dict(unum));
                    DISPATCH();
                }

                ENTRY(MP_BC_STORE_MAP):
                    MARK_EXC_IP_SELECTIVE();
                    sp -= 2;
                    mp_obj_dict_store(sp[0], sp[2], sp[1]);
                    DISPATCH();

#if MICROPY_PY_BUILTINS_SET
                ENTRY(MP_BC_BUILD_SET): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    sp -= unum - 1;
                    SET_TOP(mp_obj_new_set(unum, sp));
                    DISPATCH();
                }
#endif

#if MICROPY_PY_BUILTINS_SLICE
                ENTRY(MP_BC_BUILD_SLICE): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    if (unum == 2) {
                        mp_obj_t stop = POP();
                        mp_obj_t start = TOP();
                        SET_TOP(mp_obj_new_slice(start, stop, mp_const_none));
                    } else {
                        mp_obj_t step = POP();
                        mp_obj_t stop = POP();
                        mp_obj_t start = TOP();
                        SET_TOP(mp_obj_new_slice(start, stop, step));
                    }
                    DISPATCH();
                }
#endif

                ENTRY(MP_BC_STORE_COMP): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    mp_obj_t obj = sp[-(unum >> 2)];
                    if ((unum & 3) == 0) {
                        mp_obj_list_append(obj, sp[0]);
                        sp--;
                    } else if (!MICROPY_PY_BUILTINS_SET || (unum & 3) == 1) {
                        mp_obj_dict_store(obj, sp[0], sp[-1]);
                        sp -= 2;
                    #if MICROPY_PY_BUILTINS_SET
                    } else {
                        mp_obj_set_store(obj, sp[0]);
                        sp--;
                    #endif
                    }
                    DISPATCH();
                }

                ENTRY(MP_BC_UNPACK_SEQUENCE): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    mp_unpack_sequence(sp[0], unum, sp);
                    sp += unum - 1;
                    DISPATCH();
                }

                ENTRY(MP_BC_UNPACK_EX): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    mp_unpack_ex(sp[0], unum, sp);
                    sp += (unum & 0xff) + ((unum >> 8) & 0xff);
                    DISPATCH();
                }

                ENTRY(MP_BC_MAKE_FUNCTION): {
                    DECODE_PTR;
                    PUSH(mp_make_function_from_raw_code(ptr, MP_OBJ_NULL, MP_OBJ_NULL));
                    DISPATCH();
                }

                ENTRY(MP_BC_MAKE_FUNCTION_DEFARGS): {
                    DECODE_PTR;
                    // Stack layout: def_tuple def_dict <- TOS
                    mp_obj_t def_dict = POP();
                    SET_TOP(mp_make_function_from_raw_code(ptr, TOP(), def_dict));
                    DISPATCH();
                }

                ENTRY(MP_BC_MAKE_CLOSURE): {
                    DECODE_PTR;
                    size_t n_closed_over = *ip++;
                    // Stack layout: closed_overs <- TOS
                    sp -= n_closed_over - 1;
                    SET_TOP(mp_make_closure_from_raw_code(ptr, n_closed_over, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_MAKE_CLOSURE_DEFARGS): {
                    DECODE_PTR;
                    size_t n_closed_over = *ip++;
                    // Stack layout: def_tuple def_dict closed_overs <- TOS
                    sp -= 2 + n_closed_over - 1;
                    SET_TOP(mp_make_closure_from_raw_code(ptr, 0x100 | n_closed_over, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_CALL_FUNCTION): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    // unum & 0xff == n_positional
                    // (unum >> 8) & 0xff == n_keyword
                    sp -= (unum & 0xff) + ((unum >> 7) & 0x1fe);
                    #if MICROPY_STACKLESS
                    if (mp_obj_get_type(*sp) == &mp_type_fun_bc) {
                        code_state->ip = ip;
                        code_state->sp = sp;
                        code_state->exc_sp = MP_TAGPTR_MAKE(exc_sp, currently_in_except_block);
                        mp_code_state_t *new_state = mp_obj_fun_bc_prepare_codestate(*sp, unum & 0xff, (unum >> 8) & 0xff, sp + 1);
                        if (new_state) {
                            new_state->prev = code_state;
                            code_state = new_state;
                            nlr_pop();
                            goto run_code_state;
                        }
                        #if MICROPY_STACKLESS_STRICT
                        else {
                        deep_recursion_error:
                            mp_raise_recursion_depth();
                        }
                        #else
                        // If we couldn't allocate codestate on heap, in
                        // non non-strict case fall thru to stack allocation.
                        #endif
                    }
                    #endif
                    SET_TOP(mp_call_function_n_kw(*sp, unum & 0xff, (unum >> 8) & 0xff, sp + 1));
                    DISPATCH();
                }

                ENTRY(MP_BC_CALL_FUNCTION_VAR_KW): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    // unum & 0xff == n_positional
                    // (unum >> 8) & 0xff == n_keyword
                    // We have following stack layout here:
                    // fun arg0 arg1 ... kw0 val0 kw1 val1 ... seq dict <- TOS
                    sp -= (unum & 0xff) + ((unum >> 7) & 0x1fe) + 2;
                    #if MICROPY_STACKLESS
                    if (mp_obj_get_type(*sp) == &mp_type_fun_bc) {
                        code_state->ip = ip;
                        code_state->sp = sp;
                        code_state->exc_sp = MP_TAGPTR_MAKE(exc_sp, currently_in_except_block);

                        mp_call_args_t out_args;
                        mp_call_prepare_args_n_kw_var(false, unum, sp, &out_args);

                        mp_code_state_t *new_state = mp_obj_fun_bc_prepare_codestate(out_args.fun,
                            out_args.n_args, out_args.n_kw, out_args.args);
                        #if !MICROPY_ENABLE_PYSTACK
                        // Freeing args at this point does not follow a LIFO order so only do it if
                        // pystack is not enabled.  For pystack, they are freed when code_state is.
                        mp_nonlocal_free(out_args.args, out_args.n_alloc * sizeof(mp_obj_t));
                        #endif
                        if (new_state) {
                            new_state->prev = code_state;
                            code_state = new_state;
                            nlr_pop();
                            goto run_code_state;
                        }
                        #if MICROPY_STACKLESS_STRICT
                        else {
                            goto deep_recursion_error;
                        }
                        #else
                        // If we couldn't allocate codestate on heap, in
                        // non non-strict case fall thru to stack allocation.
                        #endif
                    }
                    #endif
                    SET_TOP(mp_call_method_n_kw_var(false, unum, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_CALL_METHOD): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    // unum & 0xff == n_positional
                    // (unum >> 8) & 0xff == n_keyword
                    sp -= (unum & 0xff) + ((unum >> 7) & 0x1fe) + 1;
                    #if MICROPY_STACKLESS
                    if (mp_obj_get_type(*sp) == &mp_type_fun_bc) {
                        code_state->ip = ip;
                        code_state->sp = sp;
                        code_state->exc_sp = MP_TAGPTR_MAKE(exc_sp, currently_in_except_block);

                        size_t n_args = unum & 0xff;
                        size_t n_kw = (unum >> 8) & 0xff;
                        int adjust = (sp[1] == MP_OBJ_NULL) ? 0 : 1;

                        mp_code_state_t *new_state = mp_obj_fun_bc_prepare_codestate(*sp, n_args + adjust, n_kw, sp + 2 - adjust);
                        if (new_state) {
                            new_state->prev = code_state;
                            code_state = new_state;
                            nlr_pop();
                            goto run_code_state;
                        }
                        #if MICROPY_STACKLESS_STRICT
                        else {
                            goto deep_recursion_error;
                        }
                        #else
                        // If we couldn't allocate codestate on heap, in
                        // non non-strict case fall thru to stack allocation.
                        #endif
                    }
                    #endif
                    SET_TOP(mp_call_method_n_kw(unum & 0xff, (unum >> 8) & 0xff, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_CALL_METHOD_VAR_KW): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    // unum & 0xff == n_positional
                    // (unum >> 8) & 0xff == n_keyword
                    // We have following stack layout here:
                    // fun self arg0 arg1 ... kw0 val0 kw1 val1 ... seq dict <- TOS
                    sp -= (unum & 0xff) + ((unum >> 7) & 0x1fe) + 3;
                    #if MICROPY_STACKLESS
                    if (mp_obj_get_type(*sp) == &mp_type_fun_bc) {
                        code_state->ip = ip;
                        code_state->sp = sp;
                        code_state->exc_sp = MP_TAGPTR_MAKE(exc_sp, currently_in_except_block);

                        mp_call_args_t out_args;
                        mp_call_prepare_args_n_kw_var(true, unum, sp, &out_args);

                        mp_code_state_t *new_state = mp_obj_fun_bc_prepare_codestate(out_args.fun,
                            out_args.n_args, out_args.n_kw, out_args.args);
                        #if !MICROPY_ENABLE_PYSTACK
                        // Freeing args at this point does not follow a LIFO order so only do it if
                        // pystack is not enabled.  For pystack, they are freed when code_state is.
                        mp_nonlocal_free(out_args.args, out_args.n_alloc * sizeof(mp_obj_t));
                        #endif
                        if (new_state) {
                            new_state->prev = code_state;
                            code_state = new_state;
                            nlr_pop();
                            goto run_code_state;
                        }
                        #if MICROPY_STACKLESS_STRICT
                        else {
                            goto deep_recursion_error;
                        }
                        #else
                        // If we couldn't allocate codestate on heap, in
                        // non non-strict case fall thru to stack allocation.
                        #endif
                    }
                    #endif
                    SET_TOP(mp_call_method_n_kw_var(true, unum, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_RETURN_VALUE):
                    MARK_EXC_IP_SELECTIVE();
                    // These next 3 lines pop a try-finally exception handler, if one
                    // is there on the exception stack.  Without this the finally block
                    // is executed a second time when the return is executed, because
                    // the try-finally exception handler is still on the stack.
                    // TODO Possibly find a better way to handle this case.
                    if (currently_in_except_block) {
                        POP_EXC_BLOCK();
                    }
unwind_return:
                    while (exc_sp >= exc_stack) {
                        if (MP_TAGPTR_TAG1(exc_sp->val_sp)) {
                            // Getting here the stack looks like:
                            //     (..., X, [iter0, iter1, ...,] ret_val)
                            // where X is pointed to by exc_sp->val_sp and in the case
                            // of a "with" block contains the context manager info.
                            // There may be 0 or more for-iterators between X and the
                            // return value, and these must be removed before control can
                            // pass to the finally code.  We simply copy the ret_value down
                            // over these iterators, if they exist.  If they don't then the
                            // following is a null operation.
                            mp_obj_t *finally_sp = MP_TAGPTR_PTR(exc_sp->val_sp);
                            finally_sp[1] = sp[0];
                            sp = &finally_sp[1];
                            // We're going to run "finally" code as a coroutine
                            // (not calling it recursively). Set up a sentinel
                            // on a stack so it can return back to us when it is
                            // done (when WITH_CLEANUP or END_FINALLY reached).
                            PUSH(MP_OBJ_NEW_SMALL_INT(-1));
                            ip = exc_sp->handler;
                            exc_sp--;
                            goto dispatch_loop;
                        }
                        exc_sp--;
                    }
                    nlr_pop();
                    code_state->sp = sp;
                    assert(exc_sp == exc_stack - 1);
                    MICROPY_VM_HOOK_RETURN
                    #if MICROPY_STACKLESS
                    if (code_state->prev != NULL) {
                        mp_obj_t res = *sp;
                        mp_globals_set(code_state->old_globals);
                        mp_code_state_t *new_code_state = code_state->prev;
                        #if MICROPY_ENABLE_PYSTACK
                        // Free code_state, and args allocated by mp_call_prepare_args_n_kw_var
                        // (The latter is implicitly freed when using pystack due to its LIFO nature.)
                        // The sizeof in the following statement does not include the size of the variable
                        // part of the struct.  This arg is anyway not used if pystack is enabled.
                        mp_nonlocal_free(code_state, sizeof(mp_code_state_t));
                        #endif
                        code_state = new_code_state;
                        *code_state->sp = res;
                        goto run_code_state;
                    }
                    #endif
                    return MP_VM_RETURN_NORMAL;

                ENTRY(MP_BC_RAISE_VARARGS): {
                    MARK_EXC_IP_SELECTIVE();
                    mp_uint_t unum = *ip;
                    mp_obj_t obj;
                    if (unum == 2) {
                        mp_warning("exception chaining not supported");
                        // ignore (pop) "from" argument
                        sp--;
                    }
                    if (unum == 0) {
                        // search for the inner-most previous exception, to reraise it
                        obj = MP_OBJ_NULL;
                        for (mp_exc_stack_t *e = exc_sp; e >= exc_stack; e--) {
                            if (e->prev_exc != NULL) {
                                obj = MP_OBJ_FROM_PTR(e->prev_exc);
                                break;
                            }
                        }
                        if (obj == MP_OBJ_NULL) {
                            obj = mp_obj_new_exception_msg(&mp_type_RuntimeError, "no active exception to reraise");
                            RAISE(obj);
                        }
                    } else {
                        obj = TOP();
                    }
                    obj = mp_make_raise_obj(obj);
                    RAISE(obj);
                }

                ENTRY(MP_BC_YIELD_VALUE):
yield:
                    nlr_pop();
                    code_state->ip = ip;
                    code_state->sp = sp;
                    code_state->exc_sp = MP_TAGPTR_MAKE(exc_sp, currently_in_except_block);
                    return MP_VM_RETURN_YIELD;

                ENTRY(MP_BC_YIELD_FROM): {
                    MARK_EXC_IP_SELECTIVE();
//#define EXC_MATCH(exc, type) MP_OBJ_IS_TYPE(exc, type)
#define EXC_MATCH(exc, type) mp_obj_exception_match(exc, type)
#define GENERATOR_EXIT_IF_NEEDED(t) if (t != MP_OBJ_NULL && EXC_MATCH(t, MP_OBJ_FROM_PTR(&mp_type_GeneratorExit))) { RAISE(t); }
                    mp_vm_return_kind_t ret_kind;
                    mp_obj_t send_value = POP();
                    mp_obj_t t_exc = MP_OBJ_NULL;
                    mp_obj_t ret_value;
                    code_state->sp = sp; // Save sp because it's needed if mp_resume raises StopIteration
                    if (inject_exc != MP_OBJ_NULL) {
                        t_exc = inject_exc;
                        inject_exc = MP_OBJ_NULL;
                        ret_kind = mp_resume(TOP(), MP_OBJ_NULL, t_exc, &ret_value);
                    } else {
                        ret_kind = mp_resume(TOP(), send_value, MP_OBJ_NULL, &ret_value);
                    }

                    if (ret_kind == MP_VM_RETURN_YIELD) {
                        ip--;
                        PUSH(ret_value);
                        goto yield;
                    } else if (ret_kind == MP_VM_RETURN_NORMAL) {
                        // Pop exhausted gen
                        sp--;
                        if (ret_value == MP_OBJ_STOP_ITERATION) {
                            // Optimize StopIteration
                            // TODO: get StopIteration's value
                            PUSH(mp_const_none);
                        } else {
                            PUSH(ret_value);
                        }

                        // If we injected GeneratorExit downstream, then even
                        // if it was swallowed, we re-raise GeneratorExit
                        GENERATOR_EXIT_IF_NEEDED(t_exc);
                        DISPATCH();
                    } else {
                        assert(ret_kind == MP_VM_RETURN_EXCEPTION);
                        // Pop exhausted gen
                        sp--;
                        if (EXC_MATCH(ret_value, MP_OBJ_FROM_PTR(&mp_type_StopIteration))) {
                            PUSH(mp_obj_exception_get_value(ret_value));
                            // If we injected GeneratorExit downstream, then even
                            // if it was swallowed, we re-raise GeneratorExit
                            GENERATOR_EXIT_IF_NEEDED(t_exc);
                            DISPATCH();
                        } else {
                            RAISE(ret_value);
                        }
                    }
                }

                ENTRY(MP_BC_IMPORT_NAME): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    mp_obj_t obj = POP();
                    SET_TOP(mp_import_name(qst, obj, TOP()));
                    DISPATCH();
                }

                ENTRY(MP_BC_IMPORT_FROM): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    mp_obj_t obj = mp_import_from(TOP(), qst);
                    PUSH(obj);
                    DISPATCH();
                }

                ENTRY(MP_BC_IMPORT_STAR):
                    MARK_EXC_IP_SELECTIVE();
                    mp_import_all(POP());
                    DISPATCH();

#if MICROPY_OPT_COMPUTED_GOTO
                ENTRY(MP_BC_LOAD_CONST_SMALL_INT_MULTI):
                    PUSH(MP_OBJ_NEW_SMALL_INT((mp_int_t)ip[-1] - MP_BC_LOAD_CONST_SMALL_INT_MULTI - 16));
                    DISPATCH();

                ENTRY(MP_BC_LOAD_FAST_MULTI):
                    obj_shared = fastn[MP_BC_LOAD_FAST_MULTI - (mp_int_t)ip[-1]];
                    goto load_check;

                ENTRY(MP_BC_STORE_FAST_MULTI):
                    fastn[MP_BC_STORE_FAST_MULTI - (mp_int_t)ip[-1]] = POP();
                    DISPATCH();

                ENTRY(MP_BC_UNARY_OP_MULTI):
                    MARK_EXC_IP_SELECTIVE();
                    SET_TOP(mp_unary_op(ip[-1] - MP_BC_UNARY_OP_MULTI, TOP()));
                    DISPATCH();

                ENTRY(MP_BC_BINARY_OP_MULTI): {
                    MARK_EXC_IP_SELECTIVE();
                    mp_obj_t rhs = POP();
                    mp_obj_t lhs = TOP();
                    SET_TOP(mp_binary_op(ip[-1] - MP_BC_BINARY_OP_MULTI, lhs, rhs));
                    DISPATCH();
                }

                ENTRY_DEFAULT:
                    MARK_EXC_IP_SELECTIVE();
#else
                ENTRY_DEFAULT:
                    if (ip[-1] < MP_BC_LOAD_CONST_SMALL_INT_MULTI + 64) {
                        PUSH(MP_OBJ_NEW_SMALL_INT((mp_int_t)ip[-1] - MP_BC_LOAD_CONST_SMALL_INT_MULTI - 16));
                        DISPATCH();
                    } else if (ip[-1] < MP_BC_LOAD_FAST_MULTI + 16) {
                        obj_shared = fastn[MP_BC_LOAD_FAST_MULTI - (mp_int_t)ip[-1]];
                        goto load_check;
                    } else if (ip[-1] < MP_BC_STORE_FAST_MULTI + 16) {
                        fastn[MP_BC_STORE_FAST_MULTI - (mp_int_t)ip[-1]] = POP();
                        DISPATCH();
                    } else if (ip[-1] < MP_BC_UNARY_OP_MULTI + 7) {
                        SET_TOP(mp_unary_op(ip[-1] - MP_BC_UNARY_OP_MULTI, TOP()));
                        DISPATCH();
                    } else if (ip[-1] < MP_BC_BINARY_OP_MULTI + 36) {
                        mp_obj_t rhs = POP();
                        mp_obj_t lhs = TOP();
                        SET_TOP(mp_binary_op(ip[-1] - MP_BC_BINARY_OP_MULTI, lhs, rhs));
                        DISPATCH();
                    } else
#endif
                {
                    mp_obj_t obj = mp_obj_new_exception_msg(&mp_type_NotImplementedError, "byte code not implemented");
                    nlr_pop();
                    fastn[0] = obj;
                    return MP_VM_RETURN_EXCEPTION;
                }

#if !MICROPY_OPT_COMPUTED_GOTO
                } // switch
#endif

pending_exception_check:
                MICROPY_VM_HOOK_LOOP

                #if MICROPY_ENABLE_SCHEDULER
                // This is an inlined variant of mp_handle_pending
                if (MP_STATE_VM(sched_state) == MP_SCHED_PENDING) {
                    MARK_EXC_IP_SELECTIVE();
                    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
                    mp_obj_t obj = MP_STATE_VM(mp_pending_exception);
                    if (obj != MP_OBJ_NULL) {
                        MP_STATE_VM(mp_pending_exception) = MP_OBJ_NULL;
                        if (!mp_sched_num_pending()) {
                            MP_STATE_VM(sched_state) = MP_SCHED_IDLE;
                        }
                        MICROPY_END_ATOMIC_SECTION(atomic_state);
                        RAISE(obj);
                    }
                    mp_handle_pending_tail(atomic_state);
                }
                #else
                // This is an inlined variant of mp_handle_pending
                if (MP_STATE_VM(mp_pending_exception) != MP_OBJ_NULL) {
                    MARK_EXC_IP_SELECTIVE();
                    mp_obj_t obj = MP_STATE_VM(mp_pending_exception);
                    MP_STATE_VM(mp_pending_exception) = MP_OBJ_NULL;
                    RAISE(obj);
                }
                #endif

                #if MICROPY_PY_THREAD_GIL
                #if MICROPY_PY_THREAD_GIL_VM_DIVISOR
                if (--gil_divisor == 0) {
                    gil_divisor = MICROPY_PY_THREAD_GIL_VM_DIVISOR;
                #else
                {
                #endif
                    #if MICROPY_ENABLE_SCHEDULER
                    // can only switch threads if the scheduler is unlocked
                    if (MP_STATE_VM(sched_state) == MP_SCHED_IDLE)
                    #endif
                    {
                    MP_THREAD_GIL_EXIT();
                    MP_THREAD_GIL_ENTER();
                    }
                }
                #endif

            } // for loop

        } else {
exception_handler:
            // exception occurred

            #if MICROPY_PY_SYS_EXC_INFO
            MP_STATE_VM(cur_exception) = nlr.ret_val;
            #endif

            #if SELECTIVE_EXC_IP
            // with selective ip, we store the ip 1 byte past the opcode, so move ptr back
            code_state->ip -= 1;
            #endif

            if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(((mp_obj_base_t*)nlr.ret_val)->type), MP_OBJ_FROM_PTR(&mp_type_StopIteration))) {
                if (code_state->ip) {
                    // check if it's a StopIteration within a for block
                    if (*code_state->ip == MP_BC_FOR_ITER) {
                        const byte *ip = code_state->ip + 1;
                        DECODE_ULABEL; // the jump offset if iteration finishes; for labels are always forward
                        code_state->ip = ip + ulab; // jump to after for-block
                        code_state->sp -= MP_OBJ_ITER_BUF_NSLOTS; // pop the exhausted iterator
                        goto outer_dispatch_loop; // continue with dispatch loop
                    } else if (*code_state->ip == MP_BC_YIELD_FROM) {
                        // StopIteration inside yield from call means return a value of
                        // yield from, so inject exception's value as yield from's result
                        // (Instead of stack pop then push we just replace exhausted gen with value)
                        *code_state->sp = mp_obj_exception_get_value(MP_OBJ_FROM_PTR(nlr.ret_val));
                        code_state->ip++; // yield from is over, move to next instruction
                        goto outer_dispatch_loop; // continue with dispatch loop
                    }
                }
            }

#if MICROPY_STACKLESS
unwind_loop:
#endif
            // set file and line number that the exception occurred at
            // TODO: don't set traceback for exceptions re-raised by END_FINALLY.
            // But consider how to handle nested exceptions.
            if (nlr.ret_val != &mp_const_GeneratorExit_obj) {
                const byte *ip = code_state->fun_bc->bytecode;
                ip = mp_decode_uint_skip(ip); // skip n_state
                ip = mp_decode_uint_skip(ip); // skip n_exc_stack
                ip++; // skip scope_params
                ip++; // skip n_pos_args
                ip++; // skip n_kwonly_args
                ip++; // skip n_def_pos_args
                size_t bc = code_state->ip - ip;
                size_t code_info_size = mp_decode_uint_value(ip);
                ip = mp_decode_uint_skip(ip); // skip code_info_size
                bc -= code_info_size;
                #if MICROPY_PERSISTENT_CODE
                qstr block_name = ip[0] | (ip[1] << 8);
                qstr source_file = ip[2] | (ip[3] << 8);
                ip += 4;
                #else
                qstr block_name = mp_decode_uint_value(ip);
                ip = mp_decode_uint_skip(ip);
                qstr source_file = mp_decode_uint_value(ip);
                ip = mp_decode_uint_skip(ip);
                #endif
                size_t source_line = 1;
                size_t c;
                while ((c = *ip)) {
                    size_t b, l;
                    if ((c & 0x80) == 0) {
                        // 0b0LLBBBBB encoding
                        b = c & 0x1f;
                        l = c >> 5;
                        ip += 1;
                    } else {
                        // 0b1LLLBBBB 0bLLLLLLLL encoding (l's LSB in second byte)
                        b = c & 0xf;
                        l = ((c << 4) & 0x700) | ip[1];
                        ip += 2;
                    }
                    if (bc >= b) {
                        bc -= b;
                        source_line += l;
                    } else {
                        // found source line corresponding to bytecode offset
                        break;
                    }
                }
                mp_obj_exception_add_traceback(MP_OBJ_FROM_PTR(nlr.ret_val), source_file, source_line, block_name);
            }

            while (currently_in_except_block) {
                // nested exception

                assert(exc_sp >= exc_stack);

                // TODO make a proper message for nested exception
                // at the moment we are just raising the very last exception (the one that caused the nested exception)

                // move up to previous exception handler
                POP_EXC_BLOCK();
            }

            if (exc_sp >= exc_stack) {
                // set flag to indicate that we are now handling an exception
                currently_in_except_block = 1;

                // catch exception and pass to byte code
                code_state->ip = exc_sp->handler;
                mp_obj_t *sp = MP_TAGPTR_PTR(exc_sp->val_sp);
                // save this exception in the stack so it can be used in a reraise, if needed
                exc_sp->prev_exc = nlr.ret_val;
                // push exception object so it can be handled by bytecode
                PUSH(MP_OBJ_FROM_PTR(nlr.ret_val));
                code_state->sp = sp;

            #if MICROPY_STACKLESS
            } else if (code_state->prev != NULL) {
                mp_globals_set(code_state->old_globals);
                mp_code_state_t *new_code_state = code_state->prev;
                #if MICROPY_ENABLE_PYSTACK
                // Free code_state, and args allocated by mp_call_prepare_args_n_kw_var
                // (The latter is implicitly freed when using pystack due to its LIFO nature.)
                // The sizeof in the following statement does not include the size of the variable
                // part of the struct.  This arg is anyway not used if pystack is enabled.
                mp_nonlocal_free(code_state, sizeof(mp_code_state_t));
                #endif
                code_state = new_code_state;
                size_t n_state = mp_decode_uint_value(code_state->fun_bc->bytecode);
                fastn = &code_state->state[n_state - 1];
                exc_stack = (mp_exc_stack_t*)(code_state->state + n_state);
                // variables that are visible to the exception handler (declared volatile)
                currently_in_except_block = MP_TAGPTR_TAG0(code_state->exc_sp); // 0 or 1, to detect nested exceptions
                exc_sp = MP_TAGPTR_PTR(code_state->exc_sp); // stack grows up, exc_sp points to top of stack
                goto unwind_loop;

            #endif
            } else {
                // propagate exception to higher level
                // TODO what to do about ip and sp? they don't really make sense at this point
                fastn[0] = MP_OBJ_FROM_PTR(nlr.ret_val); // must put exception here because sp is invalid
                return MP_VM_RETURN_EXCEPTION;
            }
        }
    }
}
