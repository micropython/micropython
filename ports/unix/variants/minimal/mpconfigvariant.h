/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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

// options to control how MicroPython is built

#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_MINIMUM)

// Disable native emitters.
#define MICROPY_EMIT_X86 (0)
#define MICROPY_EMIT_X64 (0)
#define MICROPY_EMIT_THUMB (0)
#define MICROPY_EMIT_ARM (0)

// Tune the parser to use less RAM by default.
#define MICROPY_ALLOC_QSTR_CHUNK_INIT (64)
#define MICROPY_ALLOC_PARSE_RULE_INIT (8)
#define MICROPY_ALLOC_PARSE_RULE_INC  (8)
#define MICROPY_ALLOC_PARSE_RESULT_INIT (8)
#define MICROPY_ALLOC_PARSE_RESULT_INC (8)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT (64)

// Enable features that are not enabled by default with the minimum config.
#define MICROPY_COMP_CONST_FOLDING (1)
#define MICROPY_COMP_CONST_LITERAL (1)
#define MICROPY_COMP_CONST_TUPLE (1)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (1)
#define MICROPY_ENABLE_COMPILER (1)
#define MICROPY_ENABLE_EXTERNAL_IMPORT (1)
#define MICROPY_FULL_CHECKS (1)
#define MICROPY_HELPER_REPL (1)
#define MICROPY_KBD_EXCEPTION (1)
#define MICROPY_MODULE_GETATTR (1)
#define MICROPY_MULTIPLE_INHERITANCE (1)
#define MICROPY_PY_ASSIGN_EXPR (1)
#define MICROPY_PY_ASYNC_AWAIT (1)
#define MICROPY_PY_ATTRTUPLE (1)
#define MICROPY_PY_BUILTINS_DICT_FROMKEYS (1)
#define MICROPY_PY_BUILTINS_RANGE_ATTRS (1)
#define MICROPY_PY_GENERATOR_PEND_THROW (1)

// Enable just the sys and os built-in modules.
#define MICROPY_PY_SYS (1)
#define MICROPY_PY_OS (1)
