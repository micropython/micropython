/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Anson Mansfield
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

#ifndef _PICO_REPLACEMENT_ASSERT_H
#define _PICO_REPLACEMENT_ASSERT_H

#ifdef _PICO_ASSERT_H
#undef invalid_params_if
#undef valid_params_if
#undef hard_assert_if
#undef invalid_params_if_and_return
#undef PARAM_ASSERTIONS_ENABLED
#else
// mask out pico/assert.h using its include guard
#define _PICO_ASSERT_H
#endif

#ifndef __ASSEMBLER__
#include <stdbool.h>
#include <assert.h>
#endif

#ifndef PARAM_ASSERTIONS_ENABLE_ALL
#define PARAM_ASSERTIONS_ENABLE_ALL 0
#endif

#ifndef PARAM_ASSERTIONS_DISABLE_ALL
#define PARAM_ASSERTIONS_DISABLE_ALL 0
#endif

#define PARAM_ASSERTIONS_ENABLED(x) ((PARAM_ASSERTIONS_ENABLED_##x || PARAM_ASSERTIONS_ENABLE_ALL) && !PARAM_ASSERTIONS_DISABLE_ALL)

#define invalid_params_if(x, test) ({if (PARAM_ASSERTIONS_ENABLED(x) && (test)) { pico_param_fault(#x, __FILE__, __LINE__, #test); }})
#define valid_params_if(x, test) ({if (PARAM_ASSERTIONS_ENABLED(x) && !(test)) { pico_param_fault(#x, __FILE__, __LINE__, #test); }})
#define hard_assert_if(x, test) ({if (PARAM_ASSERTIONS_ENABLED(x) && (test)) { pico_hard_fault(#x, __FILE__, __LINE__, #test); }})
#define invalid_params_if_and_return(x, test, rc) ({ if (test) { return rc; }})

#ifndef __ASSEMBLER__
void pico_param_fault(char *, char *, unsigned int, char *);
void pico_hard_fault(char *, char *, unsigned int, char *);
#endif
#endif // _PICO_REPLACEMENT_ASSERT_H
