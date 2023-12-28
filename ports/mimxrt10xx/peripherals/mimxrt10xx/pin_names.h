/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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

// OK to include more than once because FORMAT_PIN may be different.

#ifdef MIMXRT1011_SERIES
#include "MIMXRT1011/pin_names.h"
#elif defined(MIMXRT1021_SERIES)
#include "MIMXRT1021/pin_names.h"
#elif defined(MIMXRT1042_SERIES)
#include "MIMXRT1042/pin_names.h"
#elif defined(MIMXRT1052_SERIES)
#include "MIMXRT1052/pin_names.h"
#elif defined(MIMXRT1062_SERIES)
#include "MIMXRT1062/pin_names.h"
#elif defined(MIMXRT1176_cm7_SERIES)
#include "MIMXRT1176/pin_names.h"
#endif
