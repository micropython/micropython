/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Jeff Epler
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

#include <mpconfigport.h>

#define MICROPY_FLOAT_HIGH_QUALITY_HASH (1)

#undef MICROPY_PY_IO_FILEIO
#define MICROPY_PY_IO_FILEIO        (0)

#undef MICROPY_PY_BUILTINS_INPUT
#define MICROPY_PY_BUILTINS_INPUT   (0)

#undef MICROPY_PY_IO_FILEIO
#define MICROPY_PY_IO_FILEIO        (0)

#undef MICROPY_PY_UCTYPES
#define MICROPY_PY_UCTYPES          (0)

#undef MICROPY_PY_UZLIB
#define MICROPY_PY_UZLIB            (0)

#undef MICROPY_PY_UBINASCII_CRC32
#define MICROPY_PY_UBINASCII_CRC32  (0)

#undef MICROPY_PY_USELECT_POSIX
#define MICROPY_PY_USELECT_POSIX    (0)

#undef MICROPY_PY_USELECT_DEF
#define MICROPY_PY_USELECT_DEF /* nothing */

#undef MICROPY_PY_STRUCT
#define MICROPY_PY_STRUCT           (0)

#undef MICROPY_PY_ARRAY
#define MICROPY_PY_ARRAY            (0)

#undef MICROPY_MODULE_FROZEN_STR
#define MICROPY_MODULE_FROZEN_STR   (0)

#undef MICROPY_PORT_BUILTINS
#define MICROPY_PORT_BUILTINS /* nothing */
