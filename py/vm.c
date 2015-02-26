/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "py/mpstate.h"
#include "py/nlr.h"
#include "py/emitglue.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "py/bc0.h"
#include "py/bc.h"

#if 0
#define TRACE(ip) printf("sp=" INT_FMT " ", sp - code_state->sp); mp_bytecode_print2(ip, 1);
#else
#define TRACE(ip)
#endif

// Value stack grows up (this makes it incompatible with native C stack, but
// makes sure that arguments to functions are in natural order arg1..argN
// (Python semantics mandates left-to-right evaluation order, including for
// function arguments). Stack pointer is pre-incremented and points at the
// top element.
// Exception stack also grows up, top element is also pointed at.

// Exception stack unwind reasons (WHY_* in CPython-speak)
// TODO perhaps compress this to RETURN=0, JUMP>0, with number of unwinds
// left to do encoded in the JUMP number
typedef enum {
    UNWIND_RETURN = 1,
    UNWIND_JUMP,
} mp_unwind_reason_t;

#define DECODE_UINT \
    mp_uint_t unum = 0; \
    do { \
        unum = (unum << 7) + (*ip & 0x7f); \
    } while ((*ip++ & 0x80) != 0)
#define DECODE_ULABEL mp_uint_t ulab = (ip[0] | (ip[1] << 8)); ip += 2
#define DECODE_SLABEL mp_uint_t slab = (ip[0] | (ip[1] << 8)) - 0x8000; ip += 2
#define DECODE_QSTR qstr qst = 0; \
    do { \
        qst = (qst << 7) + (*ip & 0x7f); \
    } while ((*ip++ & 0x80) != 0)
#define DECODE_PTR \
    ip = (byte*)(((mp_uint_t)ip + sizeof(mp_uint_t) - 1) & (~(sizeof(mp_uint_t) - 1))); /* align ip */ \
    void *ptr = (void*)*(mp_uint_t*)ip; \
    ip += sizeof(mp_uint_t)
#define PUSH(val) *++sp = (val)
#define POP() (*sp--)
#define TOP() (*sp)
#define SET_TOP(val) *sp = (val)

#define PUSH_EXC_BLOCK(with_or_finally) do { \
    DECODE_ULABEL; /* except labels are always forward */ \
    ++exc_sp; \
    exc_sp->handler = ip + ulab; \
    exc_sp->val_sp = MP_TAGPTR_MAKE(sp, ((with_or_finally) << 1) | currently_in_except_block); \
    exc_sp->prev_exc = MP_OBJ_NULL; \
    currently_in_except_block = 0; /* in a try block now */ \
} while (0)

#define POP_EXC_BLOCK() \
    currently_in_except_block = MP_TAGPTR_TAG0(exc_sp->val_sp); /* restore previous state */ \
    exc_sp--; /* pop back to previous exception handler */

// fastn has items in reverse order (fastn[0] is local[0], fastn[-1] is local[1], etc)
// sp points to bottom of stack which grows up
// returns:
//  MP_VM_RETURN_NORMAL, sp valid, return value in *sp
//  MP_VM_RETURN_YIELD, ip, sp valid, yielded value in *sp
//  MP_VM_RETURN_EXCEPTION, exception in fastn[0]
mp_vm_return_kind_t mp_execute_bytecode(mp_code_state *code_state, volatile mp_obj_t inject_exc) {
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
    } while(0)
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
    #define RAISE(o) do { nlr_pop(); nlr.ret_val = o; goto exception_handler; } while(0)

    // Pointers which are constant for particular invocation of mp_execute_bytecode()
    mp_obj_t *const fastn = &code_state->state[code_state->n_state - 1];
    mp_exc_stack_t *const exc_stack = (mp_exc_stack_t*)(code_state->state + code_state->n_state);

    // variables that are visible to the exception handler (declared volatile)
    volatile bool currently_in_except_block = MP_TAGPTR_TAG0(code_state->exc_sp); // 0 or 1, to detect nested exceptions
    mp_exc_stack_t *volatile exc_sp = MP_TAGPTR_PTR(code_state->exc_sp); // stack grows up, exc_sp points to top of stack

    // outer exception handling loop
    for (;;) {
        nlr_buf_t nlr;
outer_dispatch_loop:
        if (nlr_push(&nlr) == 0) {
            // local variables that are not visible to the exception handler
            const byte *ip = code_state->ip;
            mp_obj_t *sp = code_state->sp;
            mp_obj_t obj_shared;

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

                ENTRY(MP_BC_LOAD_CONST_ELLIPSIS):
                    PUSH((mp_obj_t)&mp_const_ellipsis_obj);
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

                ENTRY(MP_BC_LOAD_CONST_BYTES): {
                    DECODE_QSTR;
                    PUSH(mp_load_const_bytes(qst));
                    DISPATCH();
                }

                ENTRY(MP_BC_LOAD_CONST_STRING): {
                    DECODE_QSTR;
                    PUSH(mp_load_const_str(qst));
                    DISPATCH();
                }

                ENTRY(MP_BC_LOAD_CONST_OBJ): {
                    DECODE_PTR;
                    PUSH(ptr);
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
                    if (x < MP_STATE_CTX(dict_locals)->map.alloc && MP_STATE_CTX(dict_locals)->map.table[x].key == key) {
                        PUSH(MP_STATE_CTX(dict_locals)->map.table[x].value);
                    } else {
                        mp_map_elem_t *elem = mp_map_lookup(&MP_STATE_CTX(dict_locals)->map, MP_OBJ_NEW_QSTR(qst), MP_MAP_LOOKUP);
                        if (elem != NULL) {
                            *(byte*)ip = (elem - &MP_STATE_CTX(dict_locals)->map.table[0]) & 0xff;
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
                    if (x < MP_STATE_CTX(dict_globals)->map.alloc && MP_STATE_CTX(dict_globals)->map.table[x].key == key) {
                        PUSH(MP_STATE_CTX(dict_globals)->map.table[x].value);
                    } else {
                        mp_map_elem_t *elem = mp_map_lookup(&MP_STATE_CTX(dict_globals)->map, MP_OBJ_NEW_QSTR(qst), MP_MAP_LOOKUP);
                        if (elem != NULL) {
                            *(byte*)ip = (elem - &MP_STATE_CTX(dict_globals)->map.table[0]) & 0xff;
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
                    if (mp_obj_get_type(top)->load_attr == mp_obj_instance_load_attr) {
                        mp_obj_instance_t *self = top;
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
                // This caching code works with MICROPY_PY_BUILTINS_PROPERTY enabled because
                // if the attr exists in self->members then it can't be a property.  A
                // consequence of this is that we can't use MP_MAP_LOOKUP_ADD_IF_NOT_FOUND
                // in the fast-path below, because that store could override a property.
                ENTRY(MP_BC_STORE_ATTR): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    mp_obj_t top = TOP();
                    if (mp_obj_get_type(top)->store_attr == mp_obj_instance_store_attr && sp[-1] != MP_OBJ_NULL) {
                        mp_obj_instance_t *self = top;
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
                    mp_obj_t obj = TOP();
                    SET_TOP(mp_load_attr(obj, MP_QSTR___exit__));
                    mp_load_method(obj, MP_QSTR___enter__, sp + 1);
                    mp_obj_t ret = mp_call_method_n_kw(0, 0, sp + 1);
                    PUSH_EXC_BLOCK(1);
                    PUSH(ret);
                    DISPATCH();
                }

                ENTRY(MP_BC_WITH_CLEANUP): {
                    MARK_EXC_IP_SELECTIVE();
                    // Arriving here, there's "exception control block" on top of stack,
                    // and __exit__ bound method underneath it. Bytecode calls __exit__,
                    // and "deletes" it off stack, shifting "exception control block"
                    // to its place.
                    static const mp_obj_t no_exc[] = {mp_const_none, mp_const_none, mp_const_none};
                    if (TOP() == mp_const_none) {
                        sp--;
                        mp_obj_t obj = TOP();
                        SET_TOP(mp_const_none);
                        mp_call_function_n_kw(obj, 3, 0, no_exc);
                    } else if (MP_OBJ_IS_SMALL_INT(TOP())) {
                        mp_obj_t cause = TOP();
                        switch (MP_OBJ_SMALL_INT_VALUE(cause)) {
                            case UNWIND_RETURN:
                                mp_call_function_n_kw(sp[-2], 3, 0, no_exc);
                                break;
                            case UNWIND_JUMP:
                            with_cleanup_no_other_choice:
                                mp_call_function_n_kw(sp[-3], 3, 0, no_exc);
                                // Pop __exit__ boundmethod at sp[-3]
                                sp[-3] = sp[-2];
                                break;
                            default:
                                assert(0);
                                goto with_cleanup_no_other_choice; // to help flow control analysis
                        }
                        sp[-2] = sp[-1]; // copy retval down
                        sp[-1] = sp[0]; // copy cause down
                        sp--; // discard top value (was cause)
                    } else if (mp_obj_is_exception_type(TOP())) {
                        // Need to pass (sp[0], sp[-1], sp[-2]) as arguments so must reverse the
                        // order of these on the value stack (don't want to create a temporary
                        // array because it increases stack footprint of the VM).
                        mp_obj_t obj = sp[-2];
                        sp[-2] = sp[0];
                        sp[0] = obj;
                        mp_obj_t ret_value = mp_call_function_n_kw(sp[-3], 3, 0, &sp[-2]);
                        if (mp_obj_is_true(ret_value)) {
                            // This is what CPython does
                            //PUSH(MP_OBJ_NEW_SMALL_INT(UNWIND_SILENCED));
                            // But what we need to do is - pop exception from value stack...
                            sp -= 4;
                            // ... pop "with" exception handler, and signal END_FINALLY
                            // to just execute finally handler normally (by pushing None
                            // on value stack)
                            assert(exc_sp >= exc_stack);
                            POP_EXC_BLOCK();
                            PUSH(mp_const_none);
                        } else {
                            // Pop __exit__ boundmethod at sp[-3], remembering that top 3 values
                            // are reversed.
                            sp[-3] = sp[0];
                            obj = sp[-2];
                            sp[-2] = sp[-1];
                            sp[-1] = obj;
                            sp--;
                        }
                    } else {
                        assert(0);
                    }
                    DISPATCH();
                }

                ENTRY(MP_BC_UNWIND_JUMP): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_SLABEL;
                    PUSH((void*)(ip + slab)); // push destination ip for jump
                    PUSH((void*)(mp_uint_t)(*ip)); // push number of exception handlers to unwind (0x80 bit set if we also need to pop stack)
unwind_jump:;
                    mp_uint_t unum = (mp_uint_t)POP(); // get number of exception handlers to unwind
                    while ((unum & 0x7f) > 0) {
                        unum -= 1;
                        assert(exc_sp >= exc_stack);
                        if (MP_TAGPTR_TAG1(exc_sp->val_sp)) {
                            // We're going to run "finally" code as a coroutine
                            // (not calling it recursively). Set up a sentinel
                            // on a stack so it can return back to us when it is
                            // done (when END_FINALLY reached).
                            PUSH((void*)unum); // push number of exception handlers left to unwind
                            PUSH(MP_OBJ_NEW_SMALL_INT(UNWIND_JUMP)); // push sentinel
                            ip = exc_sp->handler; // get exception handler byte code address
                            exc_sp--; // pop exception handler
                            goto dispatch_loop; // run the exception handler
                        }
                        exc_sp--;
                    }
                    ip = (const byte*)POP(); // pop destination ip for jump
                    if (unum != 0) {
                        sp--;
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
                    // not fully implemented
                    // if TOS is an exception, reraises the exception (3 values on TOS)
                    // if TOS is None, just pops it and continues
                    // if TOS is an integer, does something else
                    // else error
                    if (mp_obj_is_exception_type(TOP())) {
                        RAISE(sp[-1]);
                    }
                    if (TOP() == mp_const_none) {
                        sp--;
                    } else if (MP_OBJ_IS_SMALL_INT(TOP())) {
                        // We finished "finally" coroutine and now dispatch back
                        // to our caller, based on TOS value
                        mp_unwind_reason_t reason = MP_OBJ_SMALL_INT_VALUE(POP());
                        switch (reason) {
                            case UNWIND_RETURN:
                                goto unwind_return;
                            case UNWIND_JUMP:
                                goto unwind_jump;
                        }
                        assert(0);
                    } else {
                        assert(0);
                    }
                    DISPATCH();

                ENTRY(MP_BC_GET_ITER):
                    MARK_EXC_IP_SELECTIVE();
                    SET_TOP(mp_getiter(TOP()));
                    DISPATCH();

                ENTRY(MP_BC_FOR_ITER): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_ULABEL; // the jump offset if iteration finishes; for labels are always forward
                    code_state->sp = sp;
                    assert(TOP());
                    mp_obj_t value = mp_iternext_allow_raise(TOP());
                    if (value == MP_OBJ_STOP_ITERATION) {
                        --sp; // pop the exhausted iterator
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
                    // TODO need to work out how blocks work etc
                    // pops block, checks it's an exception block, and restores the stack, saving the 3 exception values to local threadstate
                    assert(exc_sp >= exc_stack);
                    assert(currently_in_except_block);
                    //sp = (mp_obj_t*)(*exc_sp--);
                    //exc_sp--; // discard ip
                    POP_EXC_BLOCK();
                    //sp -= 3; // pop 3 exception values
                    DISPATCH();

                ENTRY(MP_BC_NOT):
                    if (TOP() == mp_const_true) {
                        SET_TOP(mp_const_false);
                    } else {
                        SET_TOP(mp_const_true);
                    }
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

                ENTRY(MP_BC_LIST_APPEND): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    // I think it's guaranteed by the compiler that sp[unum] is a list
                    mp_obj_list_append(sp[-unum], sp[0]);
                    sp--;
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

                ENTRY(MP_BC_MAP_ADD): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    // I think it's guaranteed by the compiler that sp[-unum - 1] is a map
                    mp_obj_dict_store(sp[-unum - 1], sp[0], sp[-1]);
                    sp -= 2;
                    DISPATCH();
                }

#if MICROPY_PY_BUILTINS_SET
                ENTRY(MP_BC_BUILD_SET): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    sp -= unum - 1;
                    SET_TOP(mp_obj_new_set(unum, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_SET_ADD): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    // I think it's guaranteed by the compiler that sp[-unum] is a set
                    mp_obj_set_store(sp[-unum], sp[0]);
                    sp--;
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
                    mp_uint_t n_closed_over = *ip++;
                    // Stack layout: closed_overs <- TOS
                    sp -= n_closed_over - 1;
                    SET_TOP(mp_make_closure_from_raw_code(ptr, n_closed_over, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_MAKE_CLOSURE_DEFARGS): {
                    DECODE_PTR;
                    mp_uint_t n_closed_over = *ip++;
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
                    SET_TOP(mp_call_function_n_kw(*sp, unum & 0xff, (unum >> 8) & 0xff, sp + 1));
                    DISPATCH();
                }

                ENTRY(MP_BC_CALL_FUNCTION_VAR_KW): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    // unum & 0xff == n_positional
                    // (unum >> 8) & 0xff == n_keyword
                    // We have folowing stack layout here:
                    // fun arg0 arg1 ... kw0 val0 kw1 val1 ... seq dict <- TOS
                    sp -= (unum & 0xff) + ((unum >> 7) & 0x1fe) + 2;
                    SET_TOP(mp_call_method_n_kw_var(false, unum, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_CALL_METHOD): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    // unum & 0xff == n_positional
                    // (unum >> 8) & 0xff == n_keyword
                    sp -= (unum & 0xff) + ((unum >> 7) & 0x1fe) + 1;
                    SET_TOP(mp_call_method_n_kw(unum & 0xff, (unum >> 8) & 0xff, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_CALL_METHOD_VAR_KW): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    // unum & 0xff == n_positional
                    // (unum >> 8) & 0xff == n_keyword
                    // We have folowing stack layout here:
                    // fun self arg0 arg1 ... kw0 val0 kw1 val1 ... seq dict <- TOS
                    sp -= (unum & 0xff) + ((unum >> 7) & 0x1fe) + 3;
                    SET_TOP(mp_call_method_n_kw_var(true, unum, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_RETURN_VALUE):
                    MARK_EXC_IP_SELECTIVE();
unwind_return:
                    while (exc_sp >= exc_stack) {
                        if (MP_TAGPTR_TAG1(exc_sp->val_sp)) {
                            // We're going to run "finally" code as a coroutine
                            // (not calling it recursively). Set up a sentinel
                            // on a stack so it can return back to us when it is
                            // done (when END_FINALLY reached).
                            PUSH(MP_OBJ_NEW_SMALL_INT(UNWIND_RETURN));
                            ip = exc_sp->handler;
                            // We don't need to do anything with sp, finally is just
                            // syntactic sugar for sequential execution??
                            // sp =
                            exc_sp--;
                            goto dispatch_loop;
                        }
                        exc_sp--;
                    }
                    nlr_pop();
                    code_state->sp = sp;
                    assert(exc_sp == exc_stack - 1);
                    return MP_VM_RETURN_NORMAL;

                ENTRY(MP_BC_RAISE_VARARGS): {
                    MARK_EXC_IP_SELECTIVE();
                    mp_uint_t unum = *ip++;
                    mp_obj_t obj;
                    assert(unum <= 1);
                    if (unum == 0) {
                        // search for the inner-most previous exception, to reraise it
                        obj = MP_OBJ_NULL;
                        for (mp_exc_stack_t *e = exc_sp; e >= exc_stack; e--) {
                            if (e->prev_exc != MP_OBJ_NULL) {
                                obj = e->prev_exc;
                                break;
                            }
                        }
                        if (obj == MP_OBJ_NULL) {
                            obj = mp_obj_new_exception_msg(&mp_type_RuntimeError, "No active exception to reraise");
                            RAISE(obj);
                        }
                    } else {
                        obj = POP();
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
#define GENERATOR_EXIT_IF_NEEDED(t) if (t != MP_OBJ_NULL && EXC_MATCH(t, &mp_type_GeneratorExit)) { RAISE(t); }
                    mp_vm_return_kind_t ret_kind;
                    mp_obj_t send_value = POP();
                    mp_obj_t t_exc = MP_OBJ_NULL;
                    mp_obj_t ret_value;
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
                    }
                    if (ret_kind == MP_VM_RETURN_NORMAL) {
                        // Pop exhausted gen
                        sp--;
                        if (ret_value == MP_OBJ_NULL) {
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
                    }
                    if (ret_kind == MP_VM_RETURN_EXCEPTION) {
                        // Pop exhausted gen
                        sp--;
                        if (EXC_MATCH(ret_value, &mp_type_StopIteration)) {
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
                    } else if (ip[-1] < MP_BC_UNARY_OP_MULTI + 5) {
                        SET_TOP(mp_unary_op(ip[-1] - MP_BC_UNARY_OP_MULTI, TOP()));
                        DISPATCH();
                    } else if (ip[-1] < MP_BC_BINARY_OP_MULTI + 35) {
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
                if (MP_STATE_VM(mp_pending_exception) != MP_OBJ_NULL) {
                    MARK_EXC_IP_SELECTIVE();
                    mp_obj_t obj = MP_STATE_VM(mp_pending_exception);
                    MP_STATE_VM(mp_pending_exception) = MP_OBJ_NULL;
                    RAISE(obj);
                }

            } // for loop

        } else {
exception_handler:
            // exception occurred

            #if SELECTIVE_EXC_IP
            // with selective ip, we store the ip 1 byte past the opcode, so move ptr back
            code_state->ip -= 1;
            #endif

            // check if it's a StopIteration within a for block
            if (*code_state->ip == MP_BC_FOR_ITER && mp_obj_is_subclass_fast(mp_obj_get_type(nlr.ret_val), &mp_type_StopIteration)) {
                const byte *ip = code_state->ip + 1;
                DECODE_ULABEL; // the jump offset if iteration finishes; for labels are always forward
                code_state->ip = ip + ulab; // jump to after for-block
                code_state->sp -= 1; // pop the exhausted iterator
                goto outer_dispatch_loop; // continue with dispatch loop
            }

            // set file and line number that the exception occurred at
            // TODO: don't set traceback for exceptions re-raised by END_FINALLY.
            // But consider how to handle nested exceptions.
            // TODO need a better way of not adding traceback to constant objects (right now, just GeneratorExit_obj and MemoryError_obj)
            if (mp_obj_is_exception_instance(nlr.ret_val) && nlr.ret_val != &mp_const_GeneratorExit_obj && nlr.ret_val != &mp_const_MemoryError_obj) {
                const byte *ip = code_state->code_info;
                mp_uint_t code_info_size = mp_decode_uint(&ip);
                qstr block_name = mp_decode_uint(&ip);
                qstr source_file = mp_decode_uint(&ip);
                mp_uint_t bc = code_state->ip - code_state->code_info - code_info_size;
                mp_uint_t source_line = 1;
                mp_uint_t c;
                while ((c = *ip)) {
                    mp_uint_t b, l;
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
                mp_obj_exception_add_traceback(nlr.ret_val, source_file, source_line, block_name);
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
                // push(traceback, exc-val, exc-type)
                PUSH(mp_const_none);
                PUSH(nlr.ret_val);
                PUSH(mp_obj_get_type(nlr.ret_val));
                code_state->sp = sp;

            } else {
                // propagate exception to higher level
                // TODO what to do about ip and sp? they don't really make sense at this point
                fastn[0] = nlr.ret_val; // must put exception here because sp is invalid
                return MP_VM_RETURN_EXCEPTION;
            }
        }
    }
}
