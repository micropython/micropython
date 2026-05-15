/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Andrew Leech
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

#include "py/mpconfig.h"

#if MICROPY_PY_MACHINE_BACKUP_MEMORY

#include "py/obj.h"
#include "py/objarray.h"

// Validate port configuration at compile time.
#if !MICROPY_PY_BUILTINS_MEMORYVIEW
#error "backup_memory requires MICROPY_PY_BUILTINS_MEMORYVIEW"
#endif
#if MICROPY_HW_BACKUP_MEMORY_ITEMSIZE != 1 && MICROPY_HW_BACKUP_MEMORY_ITEMSIZE != 4
#error "MICROPY_HW_BACKUP_MEMORY_ITEMSIZE must be 1 or 4"
#endif

// Select memoryview typecode based on item size. The 0x80 flag marks the
// memoryview as read-write.
#if MICROPY_HW_BACKUP_MEMORY_ITEMSIZE == 1
#define BACKUP_MEMORY_TYPECODE ('B' | 0x80)
#elif MICROPY_HW_BACKUP_MEMORY_ITEMSIZE == 4
#define BACKUP_MEMORY_TYPECODE ('I' | 0x80)
#endif

const MP_DEFINE_MEMORYVIEW_OBJ(machine_backup_memory_obj,
    BACKUP_MEMORY_TYPECODE, 0,
    MICROPY_HW_BACKUP_MEMORY_BYTES / MICROPY_HW_BACKUP_MEMORY_ITEMSIZE,
    MICROPY_HW_BACKUP_MEMORY_ADDR);

#endif // MICROPY_PY_MACHINE_BACKUP_MEMORY
