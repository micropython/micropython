/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include "common-hal/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/busio/UART.h"

#include "ets_sys.h"
#include "espuart.h"

#include "py/nlr.h"

// UartDev is defined and initialized in rom code.
extern UartDevice UartDev;

void common_hal_busio_uart_construct(busio_uart_obj_t *self,
        const mcu_pin_obj_t * tx, const mcu_pin_obj_t * rx, uint32_t baudrate,
        uint8_t bits, uart_parity_t parity, uint8_t stop, uint32_t timeout,
        uint8_t receiver_buffer_size) {
    if (rx != NULL || tx != &pin_GPIO2) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Only tx supported on UART1 (GPIO2)."));
    }
    // set baudrate
    UartDev.baut_rate = baudrate;

    // set data bits
    switch (bits) {
        case 5:
            UartDev.data_bits = UART_FIVE_BITS;
            break;
        case 6:
            UartDev.data_bits = UART_SIX_BITS;
            break;
        case 7:
            UartDev.data_bits = UART_SEVEN_BITS;
            break;
        case 8:
            UartDev.data_bits = UART_EIGHT_BITS;
            break;
        default:
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid data bits"));
            break;
    }

    if (parity == PARITY_NONE) {
        UartDev.parity = UART_NONE_BITS;
        UartDev.exist_parity = UART_STICK_PARITY_DIS;
    } else {
        UartDev.exist_parity = UART_STICK_PARITY_EN;
        if (parity == PARITY_ODD) {
            UartDev.parity = UART_ODD_BITS;
        } else {
            UartDev.parity = UART_EVEN_BITS;
        }
    }

    switch (stop) {
        case 1:
            UartDev.stop_bits = UART_ONE_STOP_BIT;
            break;
        case 2:
            UartDev.stop_bits = UART_TWO_STOP_BIT;
            break;
        default:
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid stop bits"));
            break;
    }

    uart_setup(UART1);
}

bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
    return self->deinited;
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
    if (common_hal_busio_uart_deinited(self)) {
        return;
    }
    PIN_FUNC_SELECT(FUNC_U1TXD_BK, 0);
    self->deinited = true;
}

size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    return 0;
}

// Write characters.
size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    // write the data
    for (size_t i = 0; i < len; ++i) {
        uart_tx_one_char(UART1, *data++);
    }

    // return number of bytes written
    return len;
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    return 0;
}

bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    return true;
}
