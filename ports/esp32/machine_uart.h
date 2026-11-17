/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2025 Damien P. George
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

#ifndef MICROPY_INCLUDED_ESP32_MACHINE_UART_H
#define MICROPY_INCLUDED_ESP32_MACHINE_UART_H

#include "driver/uart.h"
#include "soc/uart_pins.h"

// Default UART pins. A board may override any of these in its mpconfigboard.h
// to match the pins it actually breaks out, consistent with the existing
// MICROPY_HW_SPIn_* and MICROPY_HW_I2Cn_* board defaults. Pins left at
// UART_PIN_NO_CHANGE keep the IDF/hardware default routing.

// UART0 is normally the REPL/console. Keep the hardware default pins, which is
// what the IDF assigns per chip (U0TXD_GPIO_NUM / U0RXD_GPIO_NUM).
#ifndef MICROPY_HW_UART0_TX
#define MICROPY_HW_UART0_TX (UART_PIN_NO_CHANGE)
#define MICROPY_HW_UART0_RX (UART_PIN_NO_CHANGE)
#endif

// UART1 historical MicroPython default pins.
#ifndef MICROPY_HW_UART1_TX
#if CONFIG_IDF_TARGET_ESP32 && CONFIG_SPIRAM
// On ESP32 pins 9 and 10 are normally used by the SPIRAM bus, so when SPIRAM is
// enabled fall back to pins that do not collide with it.
#define MICROPY_HW_UART1_TX (5)
#define MICROPY_HW_UART1_RX (4)
#else
#define MICROPY_HW_UART1_TX (10)
#define MICROPY_HW_UART1_RX (9)
#endif
#endif

#if SOC_UART_HP_NUM > 2
// UART2 historical MicroPython default pins.
#ifndef MICROPY_HW_UART2_TX
#define MICROPY_HW_UART2_TX (17)
#define MICROPY_HW_UART2_RX (16)
#endif
#endif

#if SOC_UART_HP_NUM > 3
// UART3 (e.g. ESP32-P4) has no historical or IOMUX default pins; leave it at
// UART_PIN_NO_CHANGE so a board can assign it via MICROPY_HW_UART3_TX/RX.
#ifndef MICROPY_HW_UART3_TX
#define MICROPY_HW_UART3_TX (UART_PIN_NO_CHANGE)
#define MICROPY_HW_UART3_RX (UART_PIN_NO_CHANGE)
#endif
#endif

#if SOC_UART_HP_NUM > 4
#ifndef MICROPY_HW_UART4_TX
#define MICROPY_HW_UART4_TX (UART_PIN_NO_CHANGE)
#define MICROPY_HW_UART4_RX (UART_PIN_NO_CHANGE)
#endif
#endif

#if SOC_UART_LP_NUM >= 1
// LP UART: use the IDF per-chip IOMUX pins, which are correct on all chips
// (e.g. 5/4 on C5/C6 but 14/15 on P4).
#ifndef MICROPY_HW_LP_UART0_TX
#define MICROPY_HW_LP_UART0_TX (LP_U0TXD_GPIO_NUM)
#define MICROPY_HW_LP_UART0_RX (LP_U0RXD_GPIO_NUM)
#endif
#endif

#endif // MICROPY_INCLUDED_ESP32_MACHINE_UART_H
