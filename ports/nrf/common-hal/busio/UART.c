/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/busio/UART.h"

#include "mpconfigport.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "supervisor/shared/translate.h"

#include "tick.h"
#include "nrfx_uart.h"

static nrfx_uart_t _uart = NRFX_UART_INSTANCE(0);

// expression to examine, and return value in case of failing
#define _VERIFY_ERR(_exp, _ret) \
    do {\
      uint32_t _err = (_exp);\
      if (NRFX_SUCCESS != _err ) {\
        mp_raise_msg_varg(&mp_type_AssertionError, translate("error = 0x%08lX "), _err);\
        return _ret;\
      }\
    }while(0)

static uint32_t get_nrf_baud (uint32_t baudrate);


static void uart_callback_irq (const nrfx_uart_event_t * event, void * context) {
    busio_uart_obj_t* self = (busio_uart_obj_t*) context;

    switch ( event->type ) {
        case NRFX_UART_EVT_TX_DONE:
        case NRFX_UART_EVT_RX_DONE:
            self->xferred_bytes = event->data.rxtx.bytes;
        break;

        default:
            self->xferred_bytes = -(event->data.error.rxtx.bytes);
        break;
    }
}


void common_hal_busio_uart_construct (busio_uart_obj_t *self,
                                      const mcu_pin_obj_t * tx, const mcu_pin_obj_t * rx, uint32_t baudrate,
                                      uint8_t bits, uart_parity_t parity, uint8_t stop, uint32_t timeout,
                                      uint8_t receiver_buffer_size) {
#ifndef NRF52840_XXAA
    mp_raise_NotImplementedError(translate("busio.UART not yet implemented"));
#else
    if ( parity == PARITY_ODD ) {
        mp_raise_ValueError(translate("busio.UART odd parity is not supported"));
    }

    if ( (tx == mp_const_none) || (rx == mp_const_none) ) {
        mp_raise_ValueError(translate("Invalid pins"));
    }

    if ( receiver_buffer_size == 0 ) {
        mp_raise_ValueError(translate("Invalid buffer size"));
    }

    nrfx_uart_config_t config = {
        .pseltxd = tx->number,
        .pseltxd = rx->number,
        .pselcts = NRF_UART_PSEL_DISCONNECTED,
        .pselrts = NRF_UART_PSEL_DISCONNECTED,
        .p_context = self,
        .hwfc = NRF_UART_HWFC_DISABLED,
        .parity = (parity == PARITY_NONE) ? NRF_UART_PARITY_EXCLUDED : NRF_UART_PARITY_INCLUDED,
        .baudrate = get_nrf_baud(baudrate),
        .interrupt_priority = 7
    };

    nrfx_uart_uninit(&_uart);
    _VERIFY_ERR(nrfx_uart_init(&_uart, &config, uart_callback_irq),);
    nrfx_uart_rx_enable(&_uart);

    self->buffer_length = receiver_buffer_size;
    self->buffer = (uint8_t *) gc_alloc(self->buffer_length * sizeof(uint8_t), false, false);
    if ( self->buffer == NULL ) {
        nrfx_uart_uninit(&_uart);
        mp_raise_msg(&mp_type_MemoryError, translate("Failed to allocate RX buffer"));
    }

    self->baudrate = baudrate;
    self->timeout_ms = timeout;
#endif
}

bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
#ifndef NRF52840_XXAA
    mp_raise_NotImplementedError(translate("busio.UART not yet implemented"));
#else
    return (nrf_uart_rx_pin_get(_uart.p_reg) == NRF_UART_PSEL_DISCONNECTED) ||
           (nrf_uart_tx_pin_get(_uart.p_reg) == NRF_UART_PSEL_DISCONNECTED);
#endif
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
#ifndef NRF52840_XXAA
    mp_raise_NotImplementedError(translate("busio.UART not yet implemented"));
#else
    if ( !common_hal_busio_uart_deinited(self) ) {
        nrfx_uart_uninit(&_uart);
        gc_free(self->buffer);
    }
#endif
}

// Read characters.
size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
#ifndef NRF52840_XXAA
    mp_raise_NotImplementedError(translate("busio.UART not yet implemented"));
    return 0;
#else



    if ( (*errcode) == NRFX_SUCCESS ) {
        (*errcode) = 0;
    }

    return 0;
#endif
}

// Write characters.
size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
#ifndef NRF52840_XXAA
    mp_raise_NotImplementedError(translate("busio.UART not yet implemented"));
    return 0;
#else
    self->xferred_bytes = 0;

    (*errcode) = nrfx_uart_tx(&_uart, data, len);
    _VERIFY_ERR(*errcode, MP_STREAM_ERROR);
    (*errcode) = 0;

    uint64_t start_ticks = ticks_ms;
    while ( (0 == self->xferred_bytes) && (ticks_ms - start_ticks < self->timeout_ms) ) {
#ifdef MICROPY_VM_HOOK_LOOP
        MICROPY_VM_HOOK_LOOP
#endif
    }

    if ( self->xferred_bytes <= 0 ) {
        mp_raise_msg_varg(&mp_type_AssertionError, translate("failed"));
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    return len;
#endif
}

uint32_t common_hal_busio_uart_get_baudrate(busio_uart_obj_t *self) {
#ifndef NRF52840_XXAA
    mp_raise_NotImplementedError(translate("busio.UART not yet implemented"));
#endif
    return self->baudrate;
}

void common_hal_busio_uart_set_baudrate(busio_uart_obj_t *self, uint32_t baudrate) {
#ifndef NRF52840_XXAA
    mp_raise_NotImplementedError(translate("busio.UART not yet implemented"));
#else
    self->baudrate = baudrate;
    nrf_uart_baudrate_set(_uart.p_reg, get_nrf_baud(baudrate));
#endif
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
#ifndef NRF52840_XXAA
    mp_raise_NotImplementedError(translate("busio.UART not yet implemented"));
#else

#endif
    return 0;
}

bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
#ifndef NRF52840_XXAA
    mp_raise_NotImplementedError(translate("busio.UART not yet implemented"));
    return false;
#else
    return !nrfx_uart_tx_in_progress(&_uart);
#endif
}

static uint32_t get_nrf_baud (uint32_t baudrate)
{
    if ( baudrate <= 1200 ) {
        return NRF_UART_BAUDRATE_1200;
    }
    else if ( baudrate <= 2400 ) {
        return NRF_UART_BAUDRATE_2400;
    }
    else if ( baudrate <= 4800 ) {
        return NRF_UART_BAUDRATE_4800;
    }
    else if ( baudrate <= 9600 ) {
        return NRF_UART_BAUDRATE_9600;
    }
    else if ( baudrate <= 14400 ) {
        return NRF_UART_BAUDRATE_14400;
    }
    else if ( baudrate <= 19200 ) {
        return NRF_UART_BAUDRATE_19200;
    }
    else if ( baudrate <= 28800 ) {
        return NRF_UART_BAUDRATE_28800;
    }
    else if ( baudrate <= 38400 ) {
        return NRF_UART_BAUDRATE_38400;
    }
    else if ( baudrate <= 57600 ) {
        return NRF_UART_BAUDRATE_57600;
    }
    else if ( baudrate <= 76800 ) {
        return NRF_UART_BAUDRATE_76800;
    }
    else if ( baudrate <= 115200 ) {
        return NRF_UART_BAUDRATE_115200;
    }
    else if ( baudrate <= 230400 ) {
        return NRF_UART_BAUDRATE_230400;
    }
    else if ( baudrate <= 250000 ) {
        return NRF_UART_BAUDRATE_250000;
    }
    else if ( baudrate <= 460800 ) {
        return NRF_UART_BAUDRATE_460800;
    }
    else if ( baudrate <= 921600 ) {
        return NRF_UART_BAUDRATE_921600;
    }
    else {
        return NRF_UART_BAUDRATE_1000000;
    }
}
