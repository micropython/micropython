/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Damien P. George
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

#include "py/emitglue.h"
#include "py/objtype.h"
#include "py/objfun.h"
#include "py/runtime.h"
#include "py/bc0.h"
#include "py/profile.h"

#if MICROPY_PY_SYS_SETTRACE_DUAL_VM

#if !MICROPY_PY_SYS_SETTRACE
#error MICROPY_PY_SYS_SETTRACE_DUAL_VM requires !MICROPY_PY_SYS_SETTRACE
#endif

static mp_vm_return_kind_t mp_execute_bytecode_standard(mp_code_state_t *code_state, volatile mp_obj_t inject_exc);
static mp_vm_return_kind_t mp_execute_bytecode_tracing(mp_code_state_t *code_state, volatile mp_obj_t inject_exc);

mp_vm_return_kind_t MICROPY_WRAP_MP_EXECUTE_BYTECODE(mp_execute_bytecode)(mp_code_state_t * code_state, volatile mp_obj_t inject_exc) {
    // Select the VM based on whether there is a tracing function installed or not.
    if (MP_STATE_THREAD(prof_trace_callback) == MP_OBJ_NULL) {
        return mp_execute_bytecode_standard(code_state, inject_exc);
    } else {
        return mp_execute_bytecode_tracing(code_state, inject_exc);
    }
}

#define MICROPY_VM_IS_STATIC (1)

// Include the VM source code with settrace disabled.
#undef MICROPY_PY_SYS_SETTRACE
#define MICROPY_PY_SYS_SETTRACE (0)
#undef MICROPY_WRAP_MP_EXECUTE_BYTECODE
#define MICROPY_WRAP_MP_EXECUTE_BYTECODE(f) (f##_standard)
#include "py/vm.c"

#undef FRAME_SETUP
#undef FRAME_ENTER
#undef FRAME_LEAVE
#undef FRAME_UPDATE
#undef FRAME_UPDATE_ALWAYS
#undef TRACE_TICK

// Include the VM source code with settrace enabled.
#undef MICROPY_PY_SYS_SETTRACE
#define MICROPY_PY_SYS_SETTRACE (1)
#undef MICROPY_WRAP_MP_EXECUTE_BYTECODE
#define MICROPY_WRAP_MP_EXECUTE_BYTECODE(f) (f##_tracing)
#include "py/vm.c"

#endif // MICROPY_PY_SYS_SETTRACE_DUAL_VM
