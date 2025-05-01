/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2024 Damien P. George
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

#include <stdint.h>

// options to control how MicroPython is built

#define MICROPY_CONFIG_ROM_LEVEL    (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)

#if defined(__ARM_ARCH_ISA_ARM)
#define MICROPY_EMIT_ARM            (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)
#elif defined(__ARM_ARCH_ISA_THUMB)
#define MICROPY_EMIT_THUMB          (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)
#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p) | 1))
#elif defined(__riscv)
#define MICROPY_EMIT_RV32           (1)
#endif

#define MICROPY_MALLOC_USES_ALLOCATED_SIZE (1)
#define MICROPY_PERSISTENT_CODE_LOAD (1)
#define MICROPY_MEM_STATS           (1)
#define MICROPY_READER_VFS          (1)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_WARNINGS            (1)
#define MICROPY_PY_SYS_PLATFORM     "qemu"
#define MICROPY_PY_SYS_STDIO_BUFFER (0)
#define MICROPY_PY_SELECT           (0)
#define MICROPY_PY_TIME             (0)
#define MICROPY_PY_ASYNCIO          (0)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE "ports/qemu/modmachine.c"
#define MICROPY_PY_MACHINE_RESET    (1)
#define MICROPY_PY_MACHINE_PIN_BASE (1)
#define MICROPY_VFS                 (1)
#define MICROPY_VFS_ROM             (1)

// type definitions for the specific machine

#define MP_SSIZE_MAX (0x7fffffff)

#define UINT_FMT "%lu"
#define INT_FMT "%ld"

typedef int32_t mp_int_t; // must be pointer size
typedef uint32_t mp_uint_t; // must be pointer size
typedef long mp_off_t;

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

// We need an implementation of the log2 function which is not a macro.
#define MP_NEED_LOG2 (1)

#define MP_STATE_PORT MP_STATE_VM
