/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
 * Copyright (c) 2014-2015 Paul Sokolovsky
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
#include "py/objfun.h"
#include "py/runtime.h"
#include "py/bc0.h"
#include "py/profile.h"

// *FORMAT-OFF*

#if 0
#if MICROPY_PY_THREAD
#define TRACE_PREFIX mp_printf(&mp_plat_print, "ts=%p sp=%d ", mp_thread_get_state(), (int)(sp - &code_state->state[0] + 1))
#else
#define TRACE_PREFIX mp_printf(&mp_plat_print, "sp=%d ", (int)(sp - &code_state->state[0] + 1))
#endif
#define TRACE(ip) TRACE_PREFIX; mp_bytecode_print2(&mp_plat_print, ip, 1, code_state->fun_bc->child_table, &code_state->fun_bc->context->constants);
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

#define DECODE_ULABEL \
    size_t ulab; \
    do { \
        if (ip[0] & 0x80) { \
            ulab = ((ip[0] & 0x7f) | (ip[1] << 7)); \
            ip += 2; \
        } else { \
            ulab = ip[0]; \
            ip += 1; \
        } \
    } while (0)

#define DECODE_SLABEL \
    size_t slab; \
    do { \
        if (ip[0] & 0x80) { \
            slab = ((ip[0] & 0x7f) | (ip[1] << 7)) - 0x4000; \
            ip += 2; \
        } else { \
            slab = ip[0] - 0x40; \
            ip += 1; \
        } \
    } while (0)

#if MICROPY_EMIT_BYTECODE_USES_QSTR_TABLE

#define DECODE_QSTR \
    DECODE_UINT; \
    qstr qst = qstr_table[unum]

#else

#define DECODE_QSTR \
    DECODE_UINT; \
    qstr qst = unum;

#endif

#define DECODE_PTR \
    DECODE_UINT; \
    void *ptr = (void *)(uintptr_t)code_state->fun_bc->child_table[unum]

#define DECODE_OBJ \
    DECODE_UINT; \
    mp_obj_t obj = (mp_obj_t)code_state->fun_bc->context->constants.obj_table[unum]

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
    exc_sp->val_sp = MP_TAGPTR_MAKE(sp, ((with_or_finally) << 1)); \
    exc_sp->prev_exc = NULL; \
} while (0)

#define POP_EXC_BLOCK() \
    exc_sp--; /* pop back to previous exception handler */ \
    CLEAR_SYS_EXC_INFO() /* just clear sys.exc_info(), not compliant, but it shouldn't be used in 1st place */

#define CANCEL_ACTIVE_FINALLY(sp) do { \
    if (mp_obj_is_small_int(sp[-1])) { \
        /* Stack: (..., prev_dest_ip, prev_cause, dest_ip) */ \
        /* Cancel the unwind through the previous finally, replace with current one */ \
        sp[-2] = sp[0]; \
        sp -= 2; \
    } else { \
        assert(sp[-1] == mp_const_none || mp_obj_is_exception_instance(sp[-1])); \
        /* Stack: (..., None/exception, dest_ip) */ \
        /* Silence the finally's exception value (may be None or an exception) */ \
        sp[-1] = sp[0]; \
        --sp; \
    } \
} while (0)

#if MICROPY_PY_SYS_SETTRACE

#define FRAME_SETUP() do { \
    assert(code_state != code_state->prev_state); \
    MP_STATE_THREAD(current_code_state) = code_state; \
    assert(code_state != code_state->prev_state); \
} while(0)

#define FRAME_ENTER() do { \
    assert(code_state != code_state->prev_state); \
    code_state->prev_state = MP_STATE_THREAD(current_code_state); \
    assert(code_state != code_state->prev_state); \
    if (!mp_prof_is_executing) { \
        mp_prof_frame_enter(code_state); \
    } \
} while(0)

#define FRAME_LEAVE() do { \
    assert(code_state != code_state->prev_state); \
    MP_STATE_THREAD(current_code_state) = code_state->prev_state; \
    assert(code_state != code_state->prev_state); \
} while(0)

#define FRAME_UPDATE() do { \
    assert(MP_STATE_THREAD(current_code_state) == code_state); \
    if (!mp_prof_is_executing) { \
        code_state->frame = MP_OBJ_TO_PTR(mp_prof_frame_update(code_state)); \
    } \
} while(0)

#define TRACE_TICK(current_ip, current_sp, is_exception) do { \
    assert(code_state != code_state->prev_state); \
    assert(MP_STATE_THREAD(current_code_state) == code_state); \
    if (!mp_prof_is_executing && code_state->frame && MP_STATE_THREAD(prof_trace_callback)) { \
        MP_PROF_INSTR_DEBUG_PRINT(code_state->ip); \
    } \
    if (!mp_prof_is_executing && code_state->frame && code_state->frame->callback) { \
        mp_prof_instr_tick(code_state, is_exception); \
    } \
} while(0)

#else // MICROPY_PY_SYS_SETTRACE
#define FRAME_SETUP()
#define FRAME_ENTER()
#define FRAME_LEAVE()
#define FRAME_UPDATE()
#define TRACE_TICK(current_ip, current_sp, is_exception)
#endif // MICROPY_PY_SYS_SETTRACE

// fastn has items in reverse order (fastn[0] is local[0], fastn[-1] is local[1], etc)
// sp points to bottom of stack which grows up
// returns:
//  MP_VM_RETURN_NORMAL, sp valid, return value in *sp
//  MP_VM_RETURN_YIELD, ip, sp valid, yielded value in *sp
//  MP_VM_RETURN_EXCEPTION, exception in state[0]
mp_vm_return_kind_t MICROPY_WRAP_MP_EXECUTE_BYTECODE(mp_execute_bytecode)(mp_code_state_t *code_state, volatile mp_obj_t inject_exc) {

#define SELECTIVE_EXC_IP (0)
// When disabled, code_state->ip is updated unconditionally during op
// dispatch, and this is subsequently used in the exception handler
// (either NLR jump or direct RAISE). This is good for code size because it
// happens in a single place but is more work than necessary, as many opcodes
// cannot raise. Enabling SELECTIVE_EXC_IP means that code_state->ip
// is "selectively" updated only during handling of opcodes that might raise.
// This costs about 360 bytes on PYBV11 for a 1-3% performance gain (e.g. 3%
// in bm_fft.py). On rp2040, there is zero code size diff for a 0-1% gain.
// (Both with computed goto enabled).
#if SELECTIVE_EXC_IP
// Note: Because ip has already been advanced by one byte in the dispatch, the
// value of ip here is one byte past the last opcode.
#define MARK_EXC_IP_SELECTIVE() { code_state->ip = ip; }
// No need to update in dispatch.
#define MARK_EXC_IP_GLOBAL()
#else
#define MARK_EXC_IP_SELECTIVE()
// Immediately before dispatch, save the current ip, which will be the opcode
// about to be dispatched.
#define MARK_EXC_IP_GLOBAL() { code_state->ip = ip; }
#endif

#if MICROPY_OPT_COMPUTED_GOTO
    #include "py/vmentrytable.h"
    #define DISPATCH() do { \
        TRACE(ip); \
        MARK_EXC_IP_GLOBAL(); \
        TRACE_TICK(ip, sp, false); \
        goto *entry_table[*ip++]; \
    } while (0)
    #define DISPATCH_WITH_PEND_EXC_CHECK() goto pending_exception_check
    #define ENTRY(op) entry_##op
    #define ENTRY_DEFAULT entry_default
#else
    #define DISPATCH() goto dispatch_loop
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
FRAME_ENTER();

#if MICROPY_STACKLESS
run_code_state_from_return: ;
#endif
FRAME_SETUP();

    // Pointers which are constant for particular invocation of mp_execute_bytecode()
    mp_obj_t * /*const*/ fastn;
    mp_exc_stack_t * /*const*/ exc_stack;
    {
        size_t n_state = code_state->n_state;
        fastn = &code_state->state[n_state - 1];
        exc_stack = (mp_exc_stack_t*)(code_state->state + n_state);
    }

    // variables that are visible to the exception handler (declared volatile)
    mp_exc_stack_t *volatile exc_sp = MP_CODE_STATE_EXC_SP_IDX_TO_PTR(exc_stack, code_state->exc_sp_idx); // stack grows up, exc_sp points to top of stack

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
            #if MICROPY_EMIT_BYTECODE_USES_QSTR_TABLE
            const qstr_short_t *qstr_table = code_state->fun_bc->context->constants.qstr_table;
            #endif
            mp_obj_t obj_shared;
            MICROPY_VM_HOOK_INIT

            // If we have exception to inject, now that we finish setting up
            // execution context, raise it. This works as if MP_BC_RAISE_OBJ
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
                TRACE_TICK(ip, sp, false);
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
                    mp_uint_t num = 0;
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
                            mp_obj_t obj = mp_obj_new_exception_msg(&mp_type_NameError, MP_ERROR_TEXT("local variable referenced before assignment"));
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

                ENTRY(MP_BC_LOAD_NAME): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    PUSH(mp_load_name(qst));
                    DISPATCH();
                }

                ENTRY(MP_BC_LOAD_GLOBAL): {
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    PUSH(mp_load_global(qst));
                    DISPATCH();
                }

                ENTRY(MP_BC_LOAD_ATTR): {
                    FRAME_UPDATE();
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    mp_obj_t top = TOP();
                    mp_obj_t obj;
                    #if MICROPY_OPT_LOAD_ATTR_FAST_PATH
                    // For the specific case of an instance type, it implements .attr
                    // and forwards to its members map. Attribute lookups on instance
                    // types are extremely common, so avoid all the other checks and
                    // calls that normally happen first.
                    mp_map_elem_t *elem = NULL;
                    if (mp_obj_is_instance_type(mp_obj_get_type(top))) {
                        mp_obj_instance_t *self = MP_OBJ_TO_PTR(top);
                        elem = mp_map_lookup(&self->members, MP_OBJ_NEW_QSTR(qst), MP_MAP_LOOKUP);
                    }
                    if (elem) {
                        obj = elem->value;
                    } else
                    #endif
                    {
                        obj = mp_load_attr(top, qst);
                    }
                    SET_TOP(obj);
                    DISPATCH();
                }

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

                ENTRY(MP_BC_STORE_ATTR): {
                    FRAME_UPDATE();
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    mp_store_attr(sp[0], qst, sp[-1]);
                    sp -= 2;
                    DISPATCH();
                }

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
                    DECODE_ULABEL;
                    if (mp_obj_is_true(TOP())) {
                        ip += ulab;
                    } else {
                        sp--;
                    }
                    DISPATCH_WITH_PEND_EXC_CHECK();
                }

                ENTRY(MP_BC_JUMP_IF_FALSE_OR_POP): {
                    DECODE_ULABEL;
                    if (mp_obj_is_true(TOP())) {
                        sp--;
                    } else {
                        ip += ulab;
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
                    } else if (mp_obj_is_small_int(TOP())) {
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
                            if (exc_sp->handler >= ip) {
                                // Found a finally handler that isn't active; run it.
                                // Getting here the stack looks like:
                                //     (..., X, dest_ip)
                                // where X is pointed to by exc_sp->val_sp and in the case
                                // of a "with" block contains the context manager info.
                                assert(&sp[-1] == MP_TAGPTR_PTR(exc_sp->val_sp));
                                // We're going to run "finally" code as a coroutine
                                // (not calling it recursively). Set up a sentinel
                                // on the stack so it can return back to us when it is
                                // done (when WITH_CLEANUP or END_FINALLY reached).
                                // The sentinel is the number of exception handlers left to
                                // unwind, which is a non-negative integer.
                                PUSH(MP_OBJ_NEW_SMALL_INT(unum));
                                ip = exc_sp->handler;
                                goto dispatch_loop;
                            } else {
                                // Found a finally handler that is already active; cancel it.
                                CANCEL_ACTIVE_FINALLY(sp);
                            }
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
                    assert(exc_sp >= exc_stack);
                    POP_EXC_BLOCK();
                    if (TOP() == mp_const_none) {
                        sp--;
                    } else if (mp_obj_is_small_int(TOP())) {
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
                        *(sp - MP_OBJ_ITER_BUF_NSLOTS + 1) = MP_OBJ_NULL;
                        *(sp - MP_OBJ_ITER_BUF_NSLOTS + 2) = obj;
                    }
                    DISPATCH();
                }

                ENTRY(MP_BC_FOR_ITER): {
                    FRAME_UPDATE();
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_ULABEL; // the jump offset if iteration finishes; for labels are always forward
                    code_state->sp = sp;
                    mp_obj_t obj;
                    if (*(sp - MP_OBJ_ITER_BUF_NSLOTS + 1) == MP_OBJ_NULL) {
                        obj = *(sp - MP_OBJ_ITER_BUF_NSLOTS + 2);
                    } else {
                        obj = MP_OBJ_FROM_PTR(&sp[-MP_OBJ_ITER_BUF_NSLOTS + 1]);
                    }
                    mp_obj_t value = mp_iternext_allow_raise(obj);
                    if (value == MP_OBJ_STOP_ITERATION) {
                        sp -= MP_OBJ_ITER_BUF_NSLOTS; // pop the exhausted iterator
                        ip += ulab; // jump to after for-block
                    } else {
                        PUSH(value); // push the next iteration value
                        #if MICROPY_PY_SYS_SETTRACE
                        // LINE event should trigger for every iteration so invalidate last trigger
                        if (code_state->frame) {
                            code_state->frame->lineno = 0;
                        }
                        #endif
                    }
                    DISPATCH();
                }

                ENTRY(MP_BC_POP_EXCEPT_JUMP): {
                    assert(exc_sp >= exc_stack);
                    POP_EXC_BLOCK();
                    DECODE_ULABEL;
                    ip += ulab;
                    DISPATCH_WITH_PEND_EXC_CHECK();
                }

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
                    mp_obj_t step = mp_const_none;
                    if (*ip++ == 3) {
                        // 3-argument slice includes step
                        step = POP();
                    }
                    mp_obj_t stop = POP();
                    mp_obj_t start = TOP();
                    SET_TOP(mp_obj_new_slice(start, stop, step));
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
                    PUSH(mp_make_function_from_proto_fun(ptr, code_state->fun_bc->context, NULL));
                    DISPATCH();
                }

                ENTRY(MP_BC_MAKE_FUNCTION_DEFARGS): {
                    DECODE_PTR;
                    // Stack layout: def_tuple def_dict <- TOS
                    sp -= 1;
                    SET_TOP(mp_make_function_from_proto_fun(ptr, code_state->fun_bc->context, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_MAKE_CLOSURE): {
                    DECODE_PTR;
                    size_t n_closed_over = *ip++;
                    // Stack layout: closed_overs <- TOS
                    sp -= n_closed_over - 1;
                    SET_TOP(mp_make_closure_from_proto_fun(ptr, code_state->fun_bc->context, n_closed_over, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_MAKE_CLOSURE_DEFARGS): {
                    DECODE_PTR;
                    size_t n_closed_over = *ip++;
                    // Stack layout: def_tuple def_dict closed_overs <- TOS
                    sp -= 2 + n_closed_over - 1;
                    SET_TOP(mp_make_closure_from_proto_fun(ptr, code_state->fun_bc->context, 0x100 | n_closed_over, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_CALL_FUNCTION): {
                    FRAME_UPDATE();
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    // unum & 0xff == n_positional
                    // (unum >> 8) & 0xff == n_keyword
                    sp -= (unum & 0xff) + ((unum >> 7) & 0x1fe);
                    #if MICROPY_STACKLESS
                    if (mp_obj_get_type(*sp) == &mp_type_fun_bc) {
                        code_state->ip = ip;
                        code_state->sp = sp;
                        code_state->exc_sp_idx = MP_CODE_STATE_EXC_SP_IDX_FROM_PTR(exc_stack, exc_sp);
                        mp_code_state_t *new_state = mp_obj_fun_bc_prepare_codestate(*sp, unum & 0xff, (unum >> 8) & 0xff, sp + 1);
                        #if !MICROPY_ENABLE_PYSTACK
                        if (new_state == NULL) {
                            // Couldn't allocate codestate on heap: in the strict case raise
                            // an exception, otherwise just fall through to stack allocation.
                            #if MICROPY_STACKLESS_STRICT
                        deep_recursion_error:
                            mp_raise_recursion_depth();
                            #endif
                        } else
                        #endif
                        {
                            new_state->prev = code_state;
                            code_state = new_state;
                            nlr_pop();
                            goto run_code_state;
                        }
                    }
                    #endif
                    SET_TOP(mp_call_function_n_kw(*sp, unum & 0xff, (unum >> 8) & 0xff, sp + 1));
                    DISPATCH();
                }

                ENTRY(MP_BC_CALL_FUNCTION_VAR_KW): {
                    FRAME_UPDATE();
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    // unum & 0xff == n_positional
                    // (unum >> 8) & 0xff == n_keyword
                    // We have following stack layout here:
                    // fun arg0 arg1 ... kw0 val0 kw1 val1 ... bitmap <- TOS
                    sp -= (unum & 0xff) + ((unum >> 7) & 0x1fe) + 1;
                    #if MICROPY_STACKLESS
                    if (mp_obj_get_type(*sp) == &mp_type_fun_bc) {
                        code_state->ip = ip;
                        code_state->sp = sp;
                        code_state->exc_sp_idx = MP_CODE_STATE_EXC_SP_IDX_FROM_PTR(exc_stack, exc_sp);

                        mp_call_args_t out_args;
                        mp_call_prepare_args_n_kw_var(false, unum, sp, &out_args);

                        mp_code_state_t *new_state = mp_obj_fun_bc_prepare_codestate(out_args.fun,
                            out_args.n_args, out_args.n_kw, out_args.args);
                        #if !MICROPY_ENABLE_PYSTACK
                        // Freeing args at this point does not follow a LIFO order so only do it if
                        // pystack is not enabled.  For pystack, they are freed when code_state is.
                        mp_nonlocal_free(out_args.args, out_args.n_alloc * sizeof(mp_obj_t));
                        #endif
                        #if !MICROPY_ENABLE_PYSTACK
                        if (new_state == NULL) {
                            // Couldn't allocate codestate on heap: in the strict case raise
                            // an exception, otherwise just fall through to stack allocation.
                            #if MICROPY_STACKLESS_STRICT
                            goto deep_recursion_error;
                            #endif
                        } else
                        #endif
                        {
                            new_state->prev = code_state;
                            code_state = new_state;
                            nlr_pop();
                            goto run_code_state;
                        }
                    }
                    #endif
                    SET_TOP(mp_call_method_n_kw_var(false, unum, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_CALL_METHOD): {
                    FRAME_UPDATE();
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    // unum & 0xff == n_positional
                    // (unum >> 8) & 0xff == n_keyword
                    sp -= (unum & 0xff) + ((unum >> 7) & 0x1fe) + 1;
                    #if MICROPY_STACKLESS
                    if (mp_obj_get_type(*sp) == &mp_type_fun_bc) {
                        code_state->ip = ip;
                        code_state->sp = sp;
                        code_state->exc_sp_idx = MP_CODE_STATE_EXC_SP_IDX_FROM_PTR(exc_stack, exc_sp);

                        size_t n_args = unum & 0xff;
                        size_t n_kw = (unum >> 8) & 0xff;
                        int adjust = (sp[1] == MP_OBJ_NULL) ? 0 : 1;

                        mp_code_state_t *new_state = mp_obj_fun_bc_prepare_codestate(*sp, n_args + adjust, n_kw, sp + 2 - adjust);
                        #if !MICROPY_ENABLE_PYSTACK
                        if (new_state == NULL) {
                            // Couldn't allocate codestate on heap: in the strict case raise
                            // an exception, otherwise just fall through to stack allocation.
                            #if MICROPY_STACKLESS_STRICT
                            goto deep_recursion_error;
                            #endif
                        } else
                        #endif
                        {
                            new_state->prev = code_state;
                            code_state = new_state;
                            nlr_pop();
                            goto run_code_state;
                        }
                    }
                    #endif
                    SET_TOP(mp_call_method_n_kw(unum & 0xff, (unum >> 8) & 0xff, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_CALL_METHOD_VAR_KW): {
                    FRAME_UPDATE();
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_UINT;
                    // unum & 0xff == n_positional
                    // (unum >> 8) & 0xff == n_keyword
                    // We have following stack layout here:
                    // fun self arg0 arg1 ... kw0 val0 kw1 val1 ... bitmap <- TOS
                    sp -= (unum & 0xff) + ((unum >> 7) & 0x1fe) + 2;
                    #if MICROPY_STACKLESS
                    if (mp_obj_get_type(*sp) == &mp_type_fun_bc) {
                        code_state->ip = ip;
                        code_state->sp = sp;
                        code_state->exc_sp_idx = MP_CODE_STATE_EXC_SP_IDX_FROM_PTR(exc_stack, exc_sp);

                        mp_call_args_t out_args;
                        mp_call_prepare_args_n_kw_var(true, unum, sp, &out_args);

                        mp_code_state_t *new_state = mp_obj_fun_bc_prepare_codestate(out_args.fun,
                            out_args.n_args, out_args.n_kw, out_args.args);
                        #if !MICROPY_ENABLE_PYSTACK
                        // Freeing args at this point does not follow a LIFO order so only do it if
                        // pystack is not enabled.  For pystack, they are freed when code_state is.
                        mp_nonlocal_free(out_args.args, out_args.n_alloc * sizeof(mp_obj_t));
                        #endif
                        #if !MICROPY_ENABLE_PYSTACK
                        if (new_state == NULL) {
                            // Couldn't allocate codestate on heap: in the strict case raise
                            // an exception, otherwise just fall through to stack allocation.
                            #if MICROPY_STACKLESS_STRICT
                            goto deep_recursion_error;
                            #endif
                        } else
                        #endif
                        {
                            new_state->prev = code_state;
                            code_state = new_state;
                            nlr_pop();
                            goto run_code_state;
                        }
                    }
                    #endif
                    SET_TOP(mp_call_method_n_kw_var(true, unum, sp));
                    DISPATCH();
                }

                ENTRY(MP_BC_RETURN_VALUE):
                    MARK_EXC_IP_SELECTIVE();
unwind_return:
                    // Search for and execute finally handlers that aren't already active
                    while (exc_sp >= exc_stack) {
                        if (MP_TAGPTR_TAG1(exc_sp->val_sp)) {
                            if (exc_sp->handler >= ip) {
                                // Found a finally handler that isn't active; run it.
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
                                goto dispatch_loop;
                            } else {
                                // Found a finally handler that is already active; cancel it.
                                CANCEL_ACTIVE_FINALLY(sp);
                            }
                        }
                        POP_EXC_BLOCK();
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
                        goto run_code_state_from_return;
                    }
                    #endif
                    FRAME_LEAVE();
                    return MP_VM_RETURN_NORMAL;

                ENTRY(MP_BC_RAISE_LAST): {
                    MARK_EXC_IP_SELECTIVE();
                    // search for the inner-most previous exception, to reraise it
                    mp_obj_t obj = MP_OBJ_NULL;
                    for (mp_exc_stack_t *e = exc_sp; e >= exc_stack; --e) {
                        if (e->prev_exc != NULL) {
                            obj = MP_OBJ_FROM_PTR(e->prev_exc);
                            break;
                        }
                    }
                    if (obj == MP_OBJ_NULL) {
                        obj = mp_obj_new_exception_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("no active exception to reraise"));
                    }
                    RAISE(obj);
                }

                ENTRY(MP_BC_RAISE_OBJ): {
                    MARK_EXC_IP_SELECTIVE();
                    mp_obj_t obj = mp_make_raise_obj(TOP());
                    RAISE(obj);
                }

                ENTRY(MP_BC_RAISE_FROM): {
                    MARK_EXC_IP_SELECTIVE();
                    mp_obj_t from_value = POP();
                    if (from_value != mp_const_none) {
                        mp_warning(NULL, "exception chaining not supported");
                    }
                    mp_obj_t obj = mp_make_raise_obj(TOP());
                    RAISE(obj);
                }

                ENTRY(MP_BC_YIELD_VALUE):
yield:
                    nlr_pop();
                    code_state->ip = ip;
                    code_state->sp = sp;
                    code_state->exc_sp_idx = MP_CODE_STATE_EXC_SP_IDX_FROM_PTR(exc_stack, exc_sp);
                    FRAME_LEAVE();
                    return MP_VM_RETURN_YIELD;

                ENTRY(MP_BC_YIELD_FROM): {
                    MARK_EXC_IP_SELECTIVE();
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
                        // The generator has finished, and returned a value via StopIteration
                        // Replace exhausted generator with the returned value
                        SET_TOP(ret_value);
                        // If we injected GeneratorExit downstream, then even
                        // if it was swallowed, we re-raise GeneratorExit
                        if (t_exc != MP_OBJ_NULL && mp_obj_exception_match(t_exc, MP_OBJ_FROM_PTR(&mp_type_GeneratorExit))) {
                            mp_obj_t raise_t = mp_make_raise_obj(t_exc);
                            RAISE(raise_t);
                        }
                        DISPATCH();
                    } else {
                        assert(ret_kind == MP_VM_RETURN_EXCEPTION);
                        assert(!mp_obj_exception_match(ret_value, MP_OBJ_FROM_PTR(&mp_type_StopIteration)));
                        // Pop exhausted gen
                        sp--;
                        RAISE(ret_value);
                    }
                }

                ENTRY(MP_BC_IMPORT_NAME): {
                    FRAME_UPDATE();
                    MARK_EXC_IP_SELECTIVE();
                    DECODE_QSTR;
                    mp_obj_t obj = POP();
                    SET_TOP(mp_import_name(qst, obj, TOP()));
                    DISPATCH();
                }

                ENTRY(MP_BC_IMPORT_FROM): {
                    FRAME_UPDATE();
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
                    PUSH(MP_OBJ_NEW_SMALL_INT((mp_int_t)ip[-1] - MP_BC_LOAD_CONST_SMALL_INT_MULTI - MP_BC_LOAD_CONST_SMALL_INT_MULTI_EXCESS));
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
                    if (ip[-1] < MP_BC_LOAD_CONST_SMALL_INT_MULTI + MP_BC_LOAD_CONST_SMALL_INT_MULTI_NUM) {
                        PUSH(MP_OBJ_NEW_SMALL_INT((mp_int_t)ip[-1] - MP_BC_LOAD_CONST_SMALL_INT_MULTI - MP_BC_LOAD_CONST_SMALL_INT_MULTI_EXCESS));
                        DISPATCH();
                    } else if (ip[-1] < MP_BC_LOAD_FAST_MULTI + MP_BC_LOAD_FAST_MULTI_NUM) {
                        obj_shared = fastn[MP_BC_LOAD_FAST_MULTI - (mp_int_t)ip[-1]];
                        goto load_check;
                    } else if (ip[-1] < MP_BC_STORE_FAST_MULTI + MP_BC_STORE_FAST_MULTI_NUM) {
                        fastn[MP_BC_STORE_FAST_MULTI - (mp_int_t)ip[-1]] = POP();
                        DISPATCH();
                    } else if (ip[-1] < MP_BC_UNARY_OP_MULTI + MP_BC_UNARY_OP_MULTI_NUM) {
                        SET_TOP(mp_unary_op(ip[-1] - MP_BC_UNARY_OP_MULTI, TOP()));
                        DISPATCH();
                    } else if (ip[-1] < MP_BC_BINARY_OP_MULTI + MP_BC_BINARY_OP_MULTI_NUM) {
                        mp_obj_t rhs = POP();
                        mp_obj_t lhs = TOP();
                        SET_TOP(mp_binary_op(ip[-1] - MP_BC_BINARY_OP_MULTI, lhs, rhs));
                        DISPATCH();
                    } else
                #endif // MICROPY_OPT_COMPUTED_GOTO
                {
                    mp_obj_t obj = mp_obj_new_exception_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("opcode"));
                    nlr_pop();
                    code_state->state[0] = obj;
                    FRAME_LEAVE();
                    return MP_VM_RETURN_EXCEPTION;
                }

                #if !MICROPY_OPT_COMPUTED_GOTO
                } // switch
                #endif

pending_exception_check:
                // We've just done a branch, use this as a convenient point to
                // run periodic code/checks and/or bounce the GIL.. i.e.
                // not _every_ instruction but on average a branch should
                // occur every few instructions.
                MICROPY_VM_HOOK_LOOP

                // Check for pending exceptions or scheduled tasks to run.
                // Note: it's safe to just call mp_handle_pending(true), but
                // we can inline the check for the common case where there is
                // neither.
                if (
                #if MICROPY_ENABLE_SCHEDULER
                #if MICROPY_PY_THREAD
                    // Scheduler + threading: Scheduler and pending exceptions are independent, check both.
                    MP_STATE_VM(sched_state) == MP_SCHED_PENDING || MP_STATE_THREAD(mp_pending_exception) != MP_OBJ_NULL
                #else
                    // Scheduler + non-threading: Optimisation: pending exception sets sched_state, only check sched_state.
                    MP_STATE_VM(sched_state) == MP_SCHED_PENDING
                #endif
                #else
                    // No scheduler: Just check pending exception.
                    MP_STATE_THREAD(mp_pending_exception) != MP_OBJ_NULL
                #endif
                #if MICROPY_ENABLE_VM_ABORT
                    // Check if the VM should abort execution.
                    || MP_STATE_VM(vm_abort)
                #endif
                ) {
                    MARK_EXC_IP_SELECTIVE();
                    mp_handle_pending(true);
                }

                #if MICROPY_PY_THREAD_GIL
                #if MICROPY_PY_THREAD_GIL_VM_DIVISOR
                // Don't bounce the GIL too frequently (default every 32 branches).
                if (--gil_divisor == 0)
                #endif
                {
                    #if MICROPY_PY_THREAD_GIL_VM_DIVISOR
                    gil_divisor = MICROPY_PY_THREAD_GIL_VM_DIVISOR;
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

            #if MICROPY_PY_SYS_SETTRACE
            // Exceptions are traced here
            if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(((mp_obj_base_t*)nlr.ret_val)->type), MP_OBJ_FROM_PTR(&mp_type_Exception))) {
                TRACE_TICK(code_state->ip, code_state->sp, true /* yes, it's an exception */);
            }
            #endif

#if MICROPY_STACKLESS
unwind_loop:
#endif
            // Set traceback info (file and line number) where the exception occurred, but not for:
            // - constant GeneratorExit object, because it's const
            // - exceptions re-raised by END_FINALLY
            // - exceptions re-raised explicitly by "raise"
            if (nlr.ret_val != &mp_const_GeneratorExit_obj
                && *code_state->ip != MP_BC_END_FINALLY
                && *code_state->ip != MP_BC_RAISE_LAST) {
                const byte *ip = code_state->fun_bc->bytecode;
                MP_BC_PRELUDE_SIG_DECODE(ip);
                MP_BC_PRELUDE_SIZE_DECODE(ip);
                const byte *line_info_top = ip + n_info;
                const byte *bytecode_start = ip + n_info + n_cell;
                size_t bc = code_state->ip - bytecode_start;
                qstr block_name = mp_decode_uint_value(ip);
                for (size_t i = 0; i < 1 + n_pos_args + n_kwonly_args; ++i) {
                    ip = mp_decode_uint_skip(ip);
                }
                #if MICROPY_EMIT_BYTECODE_USES_QSTR_TABLE
                block_name = code_state->fun_bc->context->constants.qstr_table[block_name];
                qstr source_file = code_state->fun_bc->context->constants.qstr_table[0];
                #else
                qstr source_file = code_state->fun_bc->context->constants.source_file;
                #endif
                size_t source_line = mp_bytecode_get_source_line(ip, line_info_top, bc);
                mp_obj_exception_add_traceback(MP_OBJ_FROM_PTR(nlr.ret_val), source_file, source_line, block_name);
            }

            while (exc_sp >= exc_stack && exc_sp->handler <= code_state->ip) {

                // nested exception

                assert(exc_sp >= exc_stack);

                // TODO make a proper message for nested exception
                // at the moment we are just raising the very last exception (the one that caused the nested exception)

                // move up to previous exception handler
                POP_EXC_BLOCK();
            }

            if (exc_sp >= exc_stack) {
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
                size_t n_state = code_state->n_state;
                fastn = &code_state->state[n_state - 1];
                exc_stack = (mp_exc_stack_t*)(code_state->state + n_state);
                // variables that are visible to the exception handler (declared volatile)
                exc_sp = MP_CODE_STATE_EXC_SP_IDX_TO_PTR(exc_stack, code_state->exc_sp_idx); // stack grows up, exc_sp points to top of stack
                goto unwind_loop;

            #endif
            } else {
                // propagate exception to higher level
                // Note: ip and sp don't have usable values at this point
                code_state->state[0] = MP_OBJ_FROM_PTR(nlr.ret_val); // put exception here because sp is invalid
                FRAME_LEAVE();
                return MP_VM_RETURN_EXCEPTION;
            }
        }
    }
}
