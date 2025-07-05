/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Linaro Limited
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

#include "mpconfigport.h"

#undef MICROPY_PY_ARRAY
#define MICROPY_PY_ARRAY (1)
#undef MICROPY_PY_ASYNC_AWAIT
#define MICROPY_PY_ASYNC_AWAIT (1)
#define MICROPY_PY_ASYNCIO (1)
#undef MICROPY_PY_BUILTINS_FILTER
#define MICROPY_PY_BUILTINS_FILTER (1)
#define MICROPY_PY_BUILTINS_HELP_MODULES (1)
#define MICROPY_PY_BUILTINS_INPUT (1)
#undef MICROPY_PY_BUILTINS_REVERSED
#define MICROPY_PY_BUILTINS_REVERSED (1)
#undef MICROPY_PY_CMATH
#define MICROPY_PY_CMATH (1)
#undef MICROPY_PY_COLLECTIONS
#define MICROPY_PY_COLLECTIONS (1)
#define MICROPY_PY_COLLECTIONS_DEQUE (1)
#define MICROPY_PY_IO (1)
#define MICROPY_PY_JSON (1)
#define MICROPY_PY_MICROPYTHON_RINGIO (1)
#define MICROPY_PY_RANDOM (1)
#define MICROPY_PY_RANDOM_EXTRA_FUNCS (1) // for random.uniform
#define MICROPY_PY_RE (1)
#define MICROPY_PY_RE_MATCH_GROUPS (1)
#define MICROPY_PY_RE_SPAN_START_END (1)
#define MICROPY_PY_SELECT (1)
#undef MICROPY_PY_STRUCT
#define MICROPY_PY_STRUCT (1)
#define MICROPY_PY_SYS_MODULES (1)
#define MICROPY_PY_UCTYPES (1)

// This is not enabled currently, pending merge of PR #17615
#define MICROPY_HW_ENABLE_ADC (0)
#if MICROPY_HW_ENABLE_ADC
#define MICROPY_PY_MACHINE_ADC (1)
#define MICROPY_PY_MACHINE_ADC_INCLUDEFILE "ports/zephyr/machine_adc.c"
#define MICROPY_PY_MACHINE_ADC_READ (1)
#define MICROPY_PY_MACHINE_ADC_READ_UV (1)
#endif
