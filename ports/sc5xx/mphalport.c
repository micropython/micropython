/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Analog Devices, Inc.
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

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "extmod/misc.h"
#include "bm_uart.h"
#include "pin.h"

// defined in main
extern BM_UART uart0;

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    unsigned char c = 0;
    while (uart_read_byte(&uart0, &c) != UART_SUCCESS);
    return c;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    uart_write_block(&uart0, (unsigned char *)str, len);
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

// Efficiently convert "\n" to "\r\n"
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    const char *last = str;
    while (len--) {
        if (*str == '\n') {
            if (str > last) {
                mp_hal_stdout_tx_strn(last, str - last);
            }
            mp_hal_stdout_tx_strn("\r\n", 2);
            ++str;
            last = str;
        } else {
            ++str;
        }
    }
    if (str > last) {
        mp_hal_stdout_tx_strn(last, str - last);
    }
}

void mp_hal_gpio_init(pin_gpio_t *gpio, uint32_t pin, uint32_t mode, uint32_t af) {
    uint32_t pin_mask = (1 << pin);
    switch (mode) {
    case GPIO_MODE_INPUT:
        gpio -> DIR_CLR = pin_mask;
        gpio -> INEN_SET = pin_mask;
        break;
    case GPIO_MODE_OUTPUT_PP:
        gpio -> DIR_SET = pin_mask;
        gpio -> INEN_CLR = pin_mask;
        gpio -> FER_CLR = pin_mask;
        break;
    case GPIO_MODE_AF_PP:
        gpio -> FER_SET = pin_mask;
        gpio -> MUX &= ~(0x3 << (pin * 2));
        gpio -> MUX |= ((af & 0x3) << (pin * 2));
    }
}

void mp_hal_pin_config(mp_hal_pin_obj_t pin_obj, uint32_t mode, uint32_t pull, uint32_t alt) {
    pin_gpio_t *gpio = pin_obj -> gpio;
    uint32_t pin = pin_obj -> pin;
    if (mode != MP_HAL_PIN_MODE_INVALID)
        mp_hal_gpio_init(gpio, pin, mode, alt);
    else
        mp_raise_ValueError("Unsupported pin mode!");
}

bool mp_hal_pin_config_alt(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint8_t fn, uint8_t unit) {
    const pin_af_obj_t *af = pin_find_af(pin, fn, unit);
    if (af == NULL) {
        return false;
    }
    mp_hal_pin_config(pin, mode, pull, af->idx);
    return true;
}

void extint_register_pin(const void *pin, uint32_t mode, int hard_irq, mp_obj_t callback_obj) {
    mp_raise_NotImplementedError(NULL);
}
