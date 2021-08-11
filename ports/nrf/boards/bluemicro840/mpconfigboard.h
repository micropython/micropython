/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
 * Copyright (c) 2021 Pierre Constantineau
 *
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

#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "BlueMicro840"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define BOARD_HAS_CRYSTAL 1

#define MICROPY_HW_LED_STATUS       (&pin_P1_04)  // RED

#define DEFAULT_I2C_BUS_SCL         (&pin_P0_17)  // 0.17 - same position as Pro Micro
#define DEFAULT_I2C_BUS_SDA         (&pin_P0_15)  // 0.15 - same position as Pro Micro

#define DEFAULT_SPI_BUS_SCK         (&pin_P0_24)  // 0.24 - same position as Pro Micro
#define DEFAULT_SPI_BUS_MOSI        (&pin_P0_10)  // 0.10 - same position as Pro Micro
#define DEFAULT_SPI_BUS_MISO        (&pin_P0_09)  // 0.09 - same position as Pro Micro

#define DEFAULT_UART_BUS_RX         (&pin_P0_08)  // 0.08 - same position as Pro Micro
#define DEFAULT_UART_BUS_TX         (&pin_P0_06)  // 0.06 - same position as Pro Micro
