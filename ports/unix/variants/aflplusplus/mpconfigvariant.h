/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

// Set base feature level.
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)

#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_LONGLONG)

// Disable features known to affect host environment, access arbitrary addresses, or
// execute arbitrary code
#define MICROPY_VFS_POSIX_WRITABLE (0)
#define MICROPY_PY_STRUCT_UNSAFE_TYPECODES (0)
#define MICROPY_PY_UCTYPES (0)
#define MICROPY_PERSISTENT_CODE_LOAD   (0)
// https://github.com/micropython/micropython/issues/17818
#define MICROPY_PY_WEBSOCKET (0)
// https://github.com/micropython/micropython/issues/17714
#define MICROPY_PY_MACHINE (0)

#define MICROPY_EMIT_X64 (0)
#define MICROPY_EMIT_X86 (0)
#define MICROPY_EMIT_THUMB (0)
#define MICROPY_EMIT_INLINE_THUMB (0)
#define MICROPY_EMIT_INLINE_THUMB_FLOAT (0)
#define MICROPY_EMIT_ARM (0)
#define MICROPY_EMIT_XTENSA (0)
#define MICROPY_EMIT_INLINE_XTENSA (0)
#define MICROPY_EMIT_XTENSAWIN (0)
#define MICROPY_EMIT_RV32 (0)
#define MICROPY_EMIT_INLINE_RV32 (0)

#define MICROPY_EMIT_NATIVE_DEBUG (1)
#define MICROPY_EMIT_NATIVE_DEBUG_PRINTER (&mp_stderr_print)


// Enable extra Unix features.
#include "../mpconfigvariant_common.h"
