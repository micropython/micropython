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
#include "nrfx_uarte.h"
#include <string.h>

static nrfx_uarte_t _uart = NRFX_UARTE_INSTANCE(0);

// expression to examine, and return value in case of failing
#define _VERIFY_ERR(_exp) \
    do {\
      uint32_t _err = (_exp);\
      if (NRFX_SUCCESS != _err ) {\
        mp_raise_msg_varg(&mp_type_AssertionError, translate("error = 0x%08lX "), _err);\
      }\
    }while(0)

static uint32_t get_nrf_baud (uint32_t baudrate);

static void uart_callback_irq (const nrfx_uarte_event_t * event, void * context) {
    busio_uart_obj_t* self = (busio_uart_obj_t*) context;

    switch ( event->type ) {
        case NRFX_UART_EVT_TX_DONE:
        break;

        case NRFX_UART_EVT_RX_DONE:
            self->rx_count += event->data.rxtx.bytes;
            self->receiving = false;
        break;

        default:
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
    if ( (tx == mp_const_none) || (rx == mp_const_none) ) {
        mp_raise_ValueError(translate("Invalid pins"));
    }

    if ( receiver_buffer_size == 0 ) {
        mp_raise_ValueError(translate("Invalid buffer size"));
    }

    if ( parity == PARITY_ODD ) {
        mp_raise_ValueError(translate("busio.UART odd parity is not supported"));
    }

    nrfx_uarte_config_t config = {
        .pseltxd = tx->number,
        .pselrxd = rx->number,
        .pselcts = NRF_UART_PSEL_DISCONNECTED,
        .pselrts = NRF_UART_PSEL_DISCONNECTED,
        .p_context = self,
        .hwfc = NRF_UART_HWFC_DISABLED,
        .parity = (parity == PARITY_NONE) ? NRF_UART_PARITY_EXCLUDED : NRF_UART_PARITY_INCLUDED,
        .baudrate = get_nrf_baud(baudrate),
        .interrupt_priority = 7
    };

    nrfx_uarte_uninit(&_uart);
    _VERIFY_ERR(nrfx_uarte_init(&_uart, &config, uart_callback_irq));

    // Init buffer for rx
    self->buffer = (uint8_t *) gc_alloc(receiver_buffer_size, false, false);
    if ( !self->buffer ) {
        nrfx_uarte_uninit(&_uart);
        mp_raise_msg(&mp_type_MemoryError, translate("Failed to allocate RX buffer"));
    }
    self->bufsize = receiver_buffer_size;

    self->baudrate = baudrate;
    self->timeout_ms = timeout;

//    nrfx_uart_rx_enable(&_uart);
//
//    self->receiving = true;
//    _VERIFY_ERR(nrfx_uart_rx(&_uart, self->buffer, self->bufsize));
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
        nrfx_uarte_uninit(&_uart);
        gc_free(self->buffer);
    }
#endif
}

static size_t get_rx_data (busio_uart_obj_t *self, uint8_t *data, size_t len) {
    // up to max received
    const size_t cnt = MIN(self->rx_count, len);

    memcpy(data, self->buffer, cnt);
    self->rx_count -= cnt;

    // shift buffer if we didn't consume it all
    if ( self->rx_count ) {
        memmove(self->buffer, self->buffer + cnt, self->rx_count);
    }

    return cnt;
}

// Read characters.
size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
#ifndef NRF52840_XXAA
    mp_raise_NotImplementedError(translate("busio.UART not yet implemented"));
    return 0;
#else

    size_t remain = len;
#if 0
    uint64_t start_ticks = ticks_ms;

    // nrfx_uart doesn't provide API to check number of bytes received so far for the on going reception.
    // we have to abort the current transfer to get rx_count updated !!!
    if ( self->receiving ) {
        nrfx_uart_rx_abort(&_uart);
        while ( self->receiving ) {
        }
    }

    size_t cnt = get_rx_data(self, data, remain);
    data += cnt;
    remain -= cnt;

    if ( self->timeout_ms ) {
        do {
            if ( remain == 0 ) {
                break;
            }

            // no data, no transfer, start with only 1 byte each so that we could know when data is available
            if ( !self->rx_count && !self->receiving ) {
                self->receiving = true;
                _VERIFY_ERR(nrfx_uart_rx(&_uart, self->buffer, 1));
            }

            if ( self->rx_count ) {
                *data++ = self->buffer[0];
                remain--;
                self->rx_count--;
            }

#ifdef MICROPY_VM_HOOK_LOOP
            MICROPY_VM_HOOK_LOOP
#endif

        } while ( ticks_ms - start_ticks < self->timeout_ms );
    }

    // abort oon-going 1 byte transfer
    if ( self->receiving ) {
        nrfx_uart_rx_abort(&_uart);
        while ( self->receiving ) {
        }
    }

    // queue full buffer transfer
    self->receiving = true;
    _VERIFY_ERR(nrfx_uart_rx(&_uart, self->buffer + self->rx_count, self->bufsize - self->rx_count));
#endif
    return len - remain;

#endif
}

// Write characters.
size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
#ifndef NRF52840_XXAA
    mp_raise_NotImplementedError(translate("busio.UART not yet implemented"));
    return 0;
#else
    if ( len == 0 ) return 0;

    (*errcode) = nrfx_uarte_tx(&_uart, data, len);
    _VERIFY_ERR(*errcode);
    (*errcode) = 0;

    uint64_t start_ticks = ticks_ms;
    while ( nrfx_uarte_tx_in_progress(&_uart) && (ticks_ms - start_ticks < self->timeout_ms) ) {
#ifdef MICROPY_VM_HOOK_LOOP
        MICROPY_VM_HOOK_LOOP
#endif
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
//return self->rx_count + (nrfx_uart_rx_ready(&_uart) ? 1 : 0);
    return false;
#endif
}

void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self) {
    mp_raise_NotImplementedError(translate("busio.UART not yet implemented"));
}

bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
#ifndef NRF52840_XXAA
    mp_raise_NotImplementedError(translate("busio.UART not yet implemented"));
    return false;
#else
//    return !nrfx_uart_tx_in_progress(&_uart);
    return true;
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
