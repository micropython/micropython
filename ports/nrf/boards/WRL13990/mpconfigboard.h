/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#define MICROPY_HW_BOARD_NAME       "SPARKFUN WRL13990"
#define MICROPY_HW_MCU_NAME         "NRF52832"
#define MICROPY_PY_SYS_PLATFORM     "nrf52-DK"

#define MICROPY_PY_MACHINE_UART     (1)
#define MICROPY_PY_MACHINE_HW_PWM   (1)
#define MICROPY_PY_MACHINE_HW_SPI   (1)
#define MICROPY_PY_MACHINE_TIMER    (1)
#define MICROPY_PY_MACHINE_RTCOUNTER (1)
#define MICROPY_PY_MACHINE_I2C      (1)
#define MICROPY_PY_MACHINE_ADC      (1)
#define MICROPY_PY_MACHINE_TEMP     (1)
#define MICROPY_PY_RANDOM_HW_RNG    (1)

// options to check out
//#define MICROPY_PY_BLE				(0)
//#define MICROPY_HW_HAS_SDCARD		(0)


#define MICROPY_HW_HAS_LED          (1)
#define MICROPY_HW_LED_COUNT        (1)
#define MICROPY_HW_LED_PULLUP       (0) 

#define MICROPY_HW_LED1             (7) // LED1
//#define MICROPY_HW_LED2             (18) // LED2
//#define MICROPY_HW_LED3             (19) // LED3
//#define MICROPY_HW_LED4             (20) // LED4

// UART config
#define MICROPY_HW_UART1_HWFC       (0)
#define MICROPY_HW_UART1_RX         (26)
#define MICROPY_HW_UART1_TX         (27)
//#define MICROPY_HW_UART1_RX         (8)	// Default by Laird/Nordic
//#define MICROPY_HW_UART1_TX         (6)
//#define MICROPY_HW_UART1_CTS        (7)
//#define MICROPY_HW_UART1_RTS        (5)

// SPI0 config
#define MICROPY_HW_SPI0_NAME        "SPI0"
#define MICROPY_HW_SPI0_SCK         (25)
#define MICROPY_HW_SPI0_MOSI        (23)
#define MICROPY_HW_SPI0_MISO        (24)

#define MICROPY_HW_PWM0_NAME        "PWM0"
#define MICROPY_HW_PWM1_NAME        "PWM1"
#define MICROPY_HW_PWM2_NAME        "PWM2"

#define HELP_TEXT_BOARD_LED         "1"

// EXTRA FLASH (as used by Laird cs)
// Connect Macronix MX25R4035F (4MBit) or MX25R8035F (8Mbit)
#define MICROPY_HW_SPI1_NAME    	"SPI1"
#define MICROPY_HW_SPI1_MISO		(20)
#define MICROPY_HW_SPI1_CLK			(16)
#define MICROPY_HW_SPI1_MOSI		(14)
#define MICROPY_HW_SPI1_CS			(12)
