/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
 * Copyright (c) 2019 Analog Devices, Inc.
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

#define MICROPY_HW_BOARD_NAME       "SAM"
#define MICROPY_HW_MCU_NAME         "ADSP-SC589"
#define MICROPY_PY_SYS_PLATFORM     "sam"

#define MICROPY_PY_MACHINE_UART     (0)
#define MICROPY_PY_MACHINE_HW_PWM   (0)
#define MICROPY_PY_MACHINE_HW_SPI   (0)
#define MICROPY_PY_MACHINE_TIMER    (0)
#define MICROPY_PY_MACHINE_RTCOUNTER (0)
#define MICROPY_PY_MACHINE_I2C      (0)
#define MICROPY_PY_MACHINE_ADC      (0)
#define MICROPY_PY_MACHINE_TEMP     (0)
#define MICROPY_PY_RANDOM_HW_RNG    (0)

#define MICROPY_HW_HAS_LED          (0)
#define MICROPY_HW_LED_COUNT        (3)
#define MICROPY_HW_LED_PULLUP       (0)

#define MICROPY_HW_LED1             (1) // LED1
#define MICROPY_HW_LED2             (2) // LED2
#define MICROPY_HW_LED3             (3) // LED3

#define MICROPY_HW_HAS_SDCARD       (1)
#define MICROPY_HW_SDCARD_MOUNT_AT_BOOT (MICROPY_HW_HAS_SDCARD)
