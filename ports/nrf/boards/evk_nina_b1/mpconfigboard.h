/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Roland van Straten
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

// Datasheet for board:
// https://www.u-blox.com/sites/default/files/EVK-NINA-B1_UserGuide_%28UBX-15028120%29.pdf
#define MICROPY_HW_BOARD_NAME       "EVK_NINA_B1"
#define MICROPY_HW_MCU_NAME         "NRF52832"
#define MICROPY_PY_SYS_PLATFORM     "nrf52"

#define MICROPY_PY_MACHINE_UART     (1)
#define MICROPY_PY_MACHINE_HW_SPI   (1)
#define MICROPY_PY_MACHINE_TIMER    (1)
#define MICROPY_PY_MACHINE_RTCOUNTER (1)
#define MICROPY_PY_MACHINE_I2C      (1)
#define MICROPY_PY_MACHINE_ADC      (1)
#define MICROPY_PY_MACHINE_TEMP     (1)

#define MICROPY_HW_ENABLE_RNG       (1)

#define MICROPY_HW_HAS_LED          (1)
#define MICROPY_HW_LED_TRICOLOR     (1)
#define MICROPY_HW_LED_PULLUP       (1)

#define MICROPY_HW_LED_RED          (8)   // LED1 DS8 Red
#define MICROPY_HW_LED_GREEN        (16)  // LED2 DS8 Green
#define MICROPY_HW_LED_BLUE         (18)  // LED3 DS8 Blue
// LEDs conflict with UART
//#define MICROPY_HW_LED4           (7)   // DS1 Green
//#define MICROPY_HW_LED5           (31)  // DS2 Orange
//#define MICROPY_HW_LED6           (5)   // DS7 Green
//#define MICROPY_HW_LED7           (6)   // DS8 Orange

// UART config
#define MICROPY_HW_UART1_RX         (5)
#define MICROPY_HW_UART1_TX         (6)
#define MICROPY_HW_UART1_CTS        (7)
#define MICROPY_HW_UART1_RTS        (31)
#define MICROPY_HW_UART1_HWFC       (1)

// SPI0 config
#define MICROPY_HW_SPI0_NAME        "SPI0"
#define MICROPY_HW_SPI0_SCK         (14)
#define MICROPY_HW_SPI0_MOSI        (13)
#define MICROPY_HW_SPI0_MISO        (12)

#define MICROPY_HW_PWM0_NAME        "PWM0"
#define MICROPY_HW_PWM1_NAME        "PWM1"
#define MICROPY_HW_PWM2_NAME        "PWM2"

#define HELP_TEXT_BOARD_LED         "1,2,3"
