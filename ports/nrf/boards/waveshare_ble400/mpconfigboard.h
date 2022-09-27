/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Pooya Moradi
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

#define MICROPY_HW_BOARD_NAME       "Waveshare BLE400"
#define MICROPY_HW_MCU_NAME         "NRF51822"
#define MICROPY_PY_SYS_PLATFORM     "nrf51"

#define MICROPY_PY_MACHINE_UART     (1)
#define MICROPY_PY_MACHINE_HW_SPI   (1)
#define MICROPY_PY_MACHINE_TIMER    (1)
#define MICROPY_PY_MACHINE_RTCOUNTER (1)
#define MICROPY_PY_MACHINE_I2C      (1)
#define MICROPY_PY_MACHINE_ADC      (1)
#define MICROPY_PY_MACHINE_TEMP     (1)

#define MICROPY_HW_ENABLE_RNG       (1)

// Enabling the Virtual File System
/* #define MICROPY_VFS                 (0) */

// Attach the corresponding jumpers
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_SWITCH_COUNT     (2)
#define MICROPY_HW_KEY1             (16) // KEY1
#define MICROPY_HW_KEY2             (17) // KEY2

// Attach the corresponding jumpers
#define MICROPY_HW_HAS_LED          (1)
#define MICROPY_HW_LED_COUNT        (5)
#define MICROPY_HW_LED_PULLUP       (0)
#define MICROPY_HW_LED0             (18) // LED0
#define MICROPY_HW_LED1             (19) // LED1
#define MICROPY_HW_LED2             (20) // LED2
#define MICROPY_HW_LED3             (21) // LED3
#define MICROPY_HW_LED4             (22) // LED4

// UART config
// NOTE: nRF51 can only support 1 UART. On BLE400 motherboard, there are two
// sets of UART pins broken out but they should not be used at the same time.

// UART broken out to CP2102 (to USB port)
// Attach the 4 corresponsing jumpers to route to CP2102
#define MICROPY_HW_UART1_RX         (11)
#define MICROPY_HW_UART1_TX         (9)
#define MICROPY_HW_UART1_CTS        (10)
#define MICROPY_HW_UART1_RTS        (8)
#define MICROPY_HW_UART1_HWFC       (0)

// UART broken out independently (this auxiliary set should be connected to
// external chips like FTDI or Black Magic Probe's bonus UART_to_USB)
// Detach the 4 corresponsing jumpers to disconnect from CP2102
#define MICROPY_HW_UART_aux_RX         (5)
#define MICROPY_HW_UART_aux_TX         (6)
#define MICROPY_HW_UART_aux_CTS        (7)
#define MICROPY_HW_UART_aux_RTS        (12)
#define MICROPY_HW_UART_aux_HWFC       (0)

// SPI0 config
// Breakout SPI pins
#define MICROPY_HW_SPI0_NAME        "SPI0"
#define MICROPY_HW_SPI0_NSS         (30)
#define MICROPY_HW_SPI0_SCK         (25)
#define MICROPY_HW_SPI0_MOSI        (24)
#define MICROPY_HW_SPI0_MISO        (23)

// I2C config
// Breakout I2C pins
#define MICROPY_HW_I2C0_NAME        "I2C0"
#define MICROPY_HW_I2C0_SMBA        (2)
#define MICROPY_HW_I2C0_SCL         (1)
#define MICROPY_HW_I2C0_SDA         (0)


#define HELP_TEXT_BOARD_LED         "0,1,2,3,4"
