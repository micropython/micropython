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

// Disable some features that come enabled by default with the feature level.
#define MICROPY_OPT_MPZ_BITWISE                 (0)
#define MICROPY_OPT_MATH_FACTORIAL              (0)
#define MICROPY_MODULE_ATTR_DELEGATION          (0)
#define MICROPY_MODULE_BUILTIN_INIT             (0)
#define MICROPY_ENABLE_SCHEDULER                (0)
#define MICROPY_PY_BUILTINS_EXECFILE            (0)
#define MICROPY_PY_MATH_CONSTANTS               (0)
#define MICROPY_PY_MATH_FACTORIAL               (0)
#define MICROPY_PY_SYS_PS1_PS2                  (0)
#define MICROPY_PY_SYS_STDIO_BUFFER             (0)
#define MICROPY_PY_USELECT                      (0)
#define MICROPY_PY_UASYNCIO                     (0)
#define MICROPY_PY_URE_SUB                      (0)
#define MICROPY_PY_URANDOM_EXTRA_FUNCS          (0)
#define MICROPY_PY_FRAMEBUF                     (0)
