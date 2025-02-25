/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
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

// Board and hardware specific configuration
#define MICROPY_HW_MCU_NAME                     "PSoC63"
#define MICROPY_HW_BOARD_NAME                   "CY8CPROTO-063-BLE"

#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT     "CY8C-063-BLE"

#define MICROPY_GC_HEAP_SIZE                    (64 * 1024) // 64 KB

#define MICROPY_PY_MACHINE_SPI_SLAVE            (1)

// Board specific configurations
#define MAX_UART              3
#define MAX_TIMER             32
#define MAX_SPI               2
#define MAX_I2C               3
#define MAX_PWM_OBJS          28
#define MICROPY_HW_MAX_I2S    1
