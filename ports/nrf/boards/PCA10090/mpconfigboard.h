/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Glenn Ruben Bakke
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

#define PCA10090

#define MICROPY_HW_BOARD_NAME       "PCA10090"
#define MICROPY_HW_MCU_NAME         "NRF9160"
#define MICROPY_PY_SYS_PLATFORM     "nrf9160-DK"

#define MICROPY_PY_MACHINE_UART     (1)
#define MICROPY_PY_MACHINE_TIMER_NRF (0)
#define MICROPY_PY_MACHINE_RTCOUNTER (1)
#define MICROPY_PY_MACHINE_I2C      (1)
#define MICROPY_PY_MACHINE_ADC      (0)
#define MICROPY_PY_MACHINE_TEMP     (0)

#define MICROPY_MBFS                (0)

#define MICROPY_HW_HAS_LED          (1)
#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_HAS_FLASH        (0)
#define MICROPY_HW_HAS_SDCARD       (0)
#define MICROPY_HW_HAS_MMA7660      (0)
#define MICROPY_HW_HAS_LIS3DSH      (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_ENABLE_RNG       (0)
#define MICROPY_HW_ENABLE_RTC       (0)
#define MICROPY_HW_ENABLE_TIMER     (0)
#define MICROPY_HW_ENABLE_SERVO     (0)
#define MICROPY_HW_ENABLE_DAC       (0)
#define MICROPY_HW_ENABLE_CAN       (0)

#define MICROPY_HW_LED_COUNT        (4)
#define MICROPY_HW_LED_PULLUP       (0)

#define MICROPY_HW_LED1             (2) // LED1
#define MICROPY_HW_LED2             (3) // LED2
#define MICROPY_HW_LED3             (4) // LED3
#define MICROPY_HW_LED4             (5) // LED4

// UART config
// VCOM0
#define MICROPY_HW_UART1_RX         (28)
#define MICROPY_HW_UART1_TX         (29)
#define MICROPY_HW_UART1_CTS        (26)
#define MICROPY_HW_UART1_RTS        (27)
#define MICROPY_HW_UART1_HWFC       (1)

/*
// VCOM2
#define MICROPY_HW_UART1_RX         (0)
#define MICROPY_HW_UART1_TX         (1)
#define MICROPY_HW_UART1_CTS        (15)
#define MICROPY_HW_UART1_RTS        (14)
#define MICROPY_HW_UART1_HWFC       (1)
*/

// SPI0 config
#define MICROPY_HW_SPI0_NAME        "SPI0"

#define MICROPY_HW_SPI0_SCK         (13)
#define MICROPY_HW_SPI0_MOSI        (11)
#define MICROPY_HW_SPI0_MISO        (12)

#define HELP_TEXT_BOARD_LED         "1,2,3,4"
