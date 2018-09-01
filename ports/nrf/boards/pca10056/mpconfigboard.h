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

#define MICROPY_HW_BOARD_NAME       "PCA10056 nRF52840-DK"
#define MICROPY_HW_MCU_NAME         "nRF52840"
#define MICROPY_PY_SYS_PLATFORM     "nRF52840-DK"

// See legend on bottom of board
#define MICROPY_HW_UART_RX          NRF_GPIO_PIN_MAP(0, 8)
#define MICROPY_HW_UART_TX          NRF_GPIO_PIN_MAP(0, 6)
#define MICROPY_HW_UART_HWFC        (0)

#define PORT_HEAP_SIZE              (128 * 1024)
#define CIRCUITPY_AUTORELOAD_DELAY_MS 500

// Temp (could be removed) 0: usb cdc (default), 1 : hwuart (jlink)
#define CFG_HWUART_FOR_SERIAL        0

#define DEFAULT_I2C_BUS_SCL         (&pin_P0_27)
#define DEFAULT_I2C_BUS_SDA         (&pin_P0_26)

#define DEFAULT_SPI_BUS_SCK         (&pin_P1_15)
#define DEFAULT_SPI_BUS_MOSI        (&pin_P1_13)
#define DEFAULT_SPI_BUS_MISO        (&pin_P1_14)

#define DEFAULT_UART_BUS_RX         (&pin_P1_01)
#define DEFAULT_UART_BUS_TX         (&pin_P1_02)
