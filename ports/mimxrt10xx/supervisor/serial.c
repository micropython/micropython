/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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

#include "py/mphal.h"
#include <string.h>
#include "supervisor/serial.h"

#include "fsl_clock.h"
#include "fsl_lpuart.h"

// static LPUART_Type *uart_instance = LPUART1; // evk
static LPUART_Type *uart_instance = LPUART4; // feather 1011
// static LPUART_Type *uart_instance = LPUART2; // feather 1062

static uint32_t UartSrcFreq(void) {
    uint32_t freq;

    /* To make it simple, we assume default PLL and divider settings, and the only variable
         from application is use PLL3 source or OSC source */
    /* PLL3 div6 80M */
    if (CLOCK_GetMux(kCLOCK_UartMux) == 0) {
        freq = (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / 6U) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
    } else {
        freq = CLOCK_GetOscFreq() / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
    }

    return freq;
}

void serial_init(void) {
    lpuart_config_t config;

    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps = 115200;
    config.enableTx = true;
    config.enableRx = true;

    LPUART_Init(uart_instance, &config, UartSrcFreq());
}

bool serial_connected(void) {
    return true;
}

char serial_read(void) {
    uint8_t data;

    LPUART_ReadBlocking(uart_instance, &data, sizeof(data));

    return data;
}

bool serial_bytes_available(void) {
    return LPUART_GetStatusFlags(uart_instance) & kLPUART_RxDataRegFullFlag;
}

void serial_write(const char *text) {
    LPUART_WriteBlocking(uart_instance, (uint8_t *)text, strlen(text));
}

void serial_write_substring(const char *text, uint32_t len) {
    if (len == 0) {
        return;
    }

    LPUART_WriteBlocking(uart_instance, (uint8_t *)text, len);
}
