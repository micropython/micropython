/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
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

// F4 Series
#ifdef STM32F401xE
#include "stm32f4/stm32f401xe/clocks.h"
#endif
#ifdef STM32F411xE
#include "stm32f4/stm32f411xe/clocks.h"
#endif
#ifdef STM32F412Zx
#include "stm32f4/stm32f412zx/clocks.h"
#endif
#ifdef STM32F405xx
#include "stm32f4/stm32f405xx/clocks.h"
#endif
#ifdef STM32F407xx
#include "stm32f4/stm32f407xx/clocks.h"
#endif

// F7 Series
#ifdef STM32F746xx
#include "stm32f7/stm32f746xx/clocks.h"
#endif
#ifdef STM32F767xx
#include "stm32f7/stm32f767xx/clocks.h"
#endif

// H7 Series
#ifdef STM32H743xx
#include "stm32h7/stm32h743xx/clocks.h"
#endif

void stm32_peripherals_clocks_init(void);
