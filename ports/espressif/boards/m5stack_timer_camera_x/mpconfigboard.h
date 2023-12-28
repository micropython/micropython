/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Bill Sideris, independently providing these changes.
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

#define MICROPY_HW_BOARD_NAME       "M5Stack Timer Camera X"
#define MICROPY_HW_MCU_NAME         "ESP32"

#define MICROPY_HW_LED_STATUS       (&pin_GPIO2)

#define CIRCUITPY_BOARD_I2C         (3)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO13, .sda = &pin_GPIO4}, \
                                     {.scl = &pin_GPIO14, .sda = &pin_GPIO12}, \
                                     {.scl = &pin_GPIO23, .sda = &pin_GPIO25}}

// Ext. Port
// BM8563
// Camera sensor

// UART pins attached to the USB-serial converter chip
#define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO1)
#define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO3)

#define CIRCUITPY_I2C_ALLOW_INTERNAL_PULL_UP (1)
