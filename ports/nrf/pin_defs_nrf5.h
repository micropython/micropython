/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

// This file contains pin definitions that are specific to the nrf port.
// This file should only ever be #included by pin.h and not directly.

#include "nrf_gpio.h"

enum {
    PORT_A,
    PORT_B,
};

enum {
    AF_FN_UART,
    AF_FN_SPI,
};

enum {
    AF_PIN_TYPE_UART_TX = 0,
    AF_PIN_TYPE_UART_RX,
    AF_PIN_TYPE_UART_CTS,
    AF_PIN_TYPE_UART_RTS,

    AF_PIN_TYPE_SPI_MOSI = 0,
    AF_PIN_TYPE_SPI_MISO,
    AF_PIN_TYPE_SPI_SCK,
    AF_PIN_TYPE_SPI_NSS,
};

#if defined(NRF51) || defined(NRF52_SERIES)
#define PIN_DEFS_PORT_AF_UNION \
    NRF_UART_Type *UART;
//  NRF_SPI_Type  *SPIM;
//  NRF_SPIS_Type *SPIS;
#elif defined(NRF91_SERIES)
#define PIN_DEFS_PORT_AF_UNION \
    NRF_UARTE_Type *UART;
#endif

enum {
    PIN_ADC1 = (1 << 0),
};

typedef NRF_GPIO_Type pin_gpio_t;
