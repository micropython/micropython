/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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

// This config enables almost all possible features such that it can be used
// for coverage testing.

// Set base feature level.
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)

#define MICROPY_VFS                    (1)
#define MICROPY_PY_UOS_VFS             (1)

// Disable some features that come enabled by default with the feature level.
#define MICROPY_PY_BUILTINS_EXECFILE            (0)
#define MICROPY_PY_SYS_STDIO_BUFFER             (0)
#define MICROPY_PY_USELECT                      (0)

// Enable additional features.
#define MICROPY_DEBUG_PARSE_RULE_NAME  (1)
#define MICROPY_TRACKED_ALLOC           (1)
#define MICROPY_FLOAT_HIGH_QUALITY_HASH (1)
#define MICROPY_ENABLE_SCHEDULER       (1)
#define MICROPY_READER_VFS             (1)
#define MICROPY_REPL_EMACS_WORDS_MOVE  (1)
#define MICROPY_REPL_EMACS_EXTRA_WORDS_MOVE (1)
#define MICROPY_WARNINGS_CATEGORY      (1)
#define MICROPY_MODULE_GETATTR         (1)
#define MICROPY_PY_DELATTR_SETATTR     (1)
#define MICROPY_PY_ALL_INPLACE_SPECIAL_METHODS (1)
#define MICROPY_PY_REVERSE_SPECIAL_METHODS (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW_ITEMSIZE (1)
#define MICROPY_PY_BUILTINS_NEXT2      (1)
#define MICROPY_PY_BUILTINS_RANGE_BINOP (1)
#define MICROPY_PY_BUILTINS_HELP       (1)
#define MICROPY_PY_BUILTINS_HELP_MODULES (1)
#define MICROPY_PY_SYS_GETSIZEOF       (1)
#define MICROPY_PY_SYS_TRACEBACKLIMIT  (1)
#define MICROPY_PY_MATH_FACTORIAL      (1)
#define MICROPY_PY_URANDOM_EXTRA_FUNCS (1)
#define MICROPY_PY_IO_BUFFEREDWRITER (1)
#define MICROPY_PY_UASYNCIO            (1)
#define MICROPY_PY_URANDOM_SEED_INIT_FUNC (mp_urandom_seed_init())
#define MICROPY_PY_URE_DEBUG           (1)
#define MICROPY_PY_URE_MATCH_GROUPS    (1)
#define MICROPY_PY_URE_MATCH_SPAN_START_END (1)
#define MICROPY_PY_URE_SUB             (1)
#define MICROPY_VFS_POSIX              (1)
#define MICROPY_FATFS_USE_LABEL        (1)
#define MICROPY_FF_MKFS_FAT32          (1)
#define MICROPY_PY_FRAMEBUF            (1)
#define MICROPY_PY_COLLECTIONS_NAMEDTUPLE__ASDICT (1)
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT (1)
#define MICROPY_PY_STRUCT              (0) // uses shared-bindings struct
#define MICROPY_PY_UCRYPTOLIB          (1)
#define MICROPY_PY_UCRYPTOLIB_CTR      (1)
#define MICROPY_PY_MICROPYTHON_HEAP_LOCKED (1)
