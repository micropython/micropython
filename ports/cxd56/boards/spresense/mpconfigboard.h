/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2019 Sony Semiconductor Solutions Corporation
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

#define MICROPY_HW_BOARD_NAME "SPRESENSE"
#define MICROPY_HW_MCU_NAME "CXD5602"

#define DEFAULT_I2C_BUS_SCL (&pin_I2C0_BCK)
#define DEFAULT_I2C_BUS_SDA (&pin_I2C0_BDT)

#define DEFAULT_SPI_BUS_SCK (&pin_SPI4_SCK)
#define DEFAULT_SPI_BUS_MISO (&pin_SPI4_MISO)
#define DEFAULT_SPI_BUS_MOSI (&pin_SPI4_MOSI)

#define DEFAULT_UART_BUS_RX (&pin_UART2_RXD)
#define DEFAULT_UART_BUS_TX (&pin_UART2_TXD)

#define MICROPY_PY_REVERSE_SPECIAL_METHODS          (1)
