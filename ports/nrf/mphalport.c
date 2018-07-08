/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Glenn Ruben Bakke
 * Copyright (c) 2018 Artur Pacholec
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

#include <string.h>

#include "mphalport.h"

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len);

#if (MICROPY_PY_BLE_NUS == 0)
int mp_hal_stdin_rx_chr(void) {
    uint8_t data = 0;

    while (!nrfx_uart_rx_ready(&serial_instance));

    const nrfx_err_t err = nrfx_uart_rx(&serial_instance, &data, sizeof(data));
    if (err == NRFX_SUCCESS)
        NRFX_ASSERT(err);

    return data;
}

bool mp_hal_stdin_any(void) {
    return nrfx_uart_rx_ready(&serial_instance);
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    if (len == 0)
        return;

    const nrfx_err_t err = nrfx_uart_tx(&serial_instance, (uint8_t*)str, len);
    if (err == NRFX_SUCCESS)
        NRFX_ASSERT(err);
}

void mp_hal_stdout_tx_strn_cooked(const char *str, mp_uint_t len) {
    const char cr = '\r';

    while (len--) {
        if (*str == '\n')
            mp_hal_stdout_tx_strn(&cr, sizeof(cr));

        mp_hal_stdout_tx_strn(str++, sizeof(char));
    }
}
#endif

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}
