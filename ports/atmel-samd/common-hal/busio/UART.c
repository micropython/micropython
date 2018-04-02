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

#include "tick.h"

#include "hpl_sercom_config.h"
#include "peripheral_clk_config.h"

#include "hal/include/hal_gpio.h"
#include "hal/include/hal_usart_async.h"
#include "hal/include/hpl_usart_async.h"

#include "peripherals.h"
#include "pins.h"

// Do-nothing callback needed so that usart_async code will enable rx interrupts.
// See comment below re usart_async_register_callback()
static void usart_async_rxc_callback(const struct usart_async_descriptor *const descr) {
    // Nothing needs to be done by us.
}

void common_hal_busio_uart_construct(busio_uart_obj_t *self,
        const mcu_pin_obj_t * tx, const mcu_pin_obj_t * rx, uint32_t baudrate,
        uint8_t bits, uart_parity_t parity, uint8_t stop, uint32_t timeout,
        uint8_t receiver_buffer_size) {
    Sercom* sercom = NULL;
    uint8_t sercom_index = 255; // Unset index
    uint32_t rx_pinmux = 0;
    uint8_t rx_pad = 255; // Unset pad
    uint32_t tx_pinmux = 0;
    uint8_t tx_pad = 255; // Unset pad

    if (bits > 8) {
        mp_raise_NotImplementedError("bytes > 8 bits not supported");
    }

    bool have_tx = tx != mp_const_none;
    bool have_rx = rx != mp_const_none;
    if (!have_tx && !have_rx) {
        mp_raise_ValueError("tx and rx cannot both be None");
    }

    self->baudrate = baudrate;
    self->character_bits = bits;
    self->timeout_ms = timeout;

    // This assignment is only here because the usart_async routines take a *const argument.
    struct usart_async_descriptor * const usart_desc_p = (struct usart_async_descriptor * const) &self->usart_desc;

    for (int i = 0; i < NUM_SERCOMS_PER_PIN; i++) {
        Sercom* potential_sercom = NULL;
        if (have_tx) {
            sercom_index = tx->sercom[i].index;
            if (sercom_index >= SERCOM_INST_NUM) {
                continue;
            }
            potential_sercom = sercom_insts[sercom_index];
            if (potential_sercom->USART.CTRLA.bit.ENABLE != 0 ||
                !(tx->sercom[i].pad == 0 ||
                  tx->sercom[i].pad == 2)) {
                continue;
            }
            tx_pinmux = PINMUX(tx->pin, (i == 0) ? MUX_C : MUX_D);
            tx_pad = tx->sercom[i].pad;
            if (rx == mp_const_none) {
                sercom = potential_sercom;
                break;
            }
        }
        for (int j = 0; j < NUM_SERCOMS_PER_PIN; j++) {
            if (((!have_tx && rx->sercom[j].index < SERCOM_INST_NUM &&
                  sercom_insts[rx->sercom[j].index]->USART.CTRLA.bit.ENABLE == 0) ||
                 sercom_index == rx->sercom[j].index) &&
                rx->sercom[j].pad != tx_pad) {
                rx_pinmux = PINMUX(rx->pin, (j == 0) ? MUX_C : MUX_D);
                rx_pad = rx->sercom[j].pad;
                sercom = sercom_insts[rx->sercom[j].index];
                sercom_index = rx->sercom[j].index;
                break;
            }
        }
        if (sercom != NULL) {
            break;
        }
    }
    if (sercom == NULL) {
        mp_raise_ValueError("Invalid pins");
    }
    if (!have_tx) {
        tx_pad = 0;
        if (rx_pad == 0) {
            tx_pad = 2;
        }
    }
    if (!have_rx) {
        rx_pad = (tx_pad + 1) % 4;
    }

    // Set up clocks on SERCOM.
    samd_peripherals_sercom_clock_init(sercom, sercom_index);

    if (rx && receiver_buffer_size > 0) {
        self->buffer_length = receiver_buffer_size;
        self->buffer = (uint8_t *) gc_alloc(self->buffer_length * sizeof(uint8_t), false, false);
        if (self->buffer == NULL) {
            common_hal_busio_uart_deinit(self);
            mp_raise_msg(&mp_type_MemoryError, "Failed to allocate RX buffer");
        }
    } else {
        self->buffer_length = 0;
        self->buffer = NULL;
    }

    if (usart_async_init(usart_desc_p, sercom, self->buffer, self->buffer_length, NULL) != ERR_NONE) {
        mp_raise_ValueError("Could not initialize UART");
    }

    // usart_async_init() sets a number of defaults based on a prototypical SERCOM
    // which don't necessarily match what we need. After calling it, set the values
    // specific to this instantiation of UART.

    // Set pads computed for this SERCOM.
    // TXPO:
    // 0x0: TX pad 0; no RTS/CTS
    // 0x1: TX pad 2; no RTS/CTS
    // 0x2: TX pad 0; RTS: pad 2, CTS: pad 3 (not used by us right now)
    // So divide by 2 to map pad to value.
    hri_sercomusart_write_CTRLA_TXPO_bf(sercom, tx_pad / 2);
    // RXPO:
    // 0x0: RX pad 0
    // 0x1: RX pad 1
    // 0x2: RX pad 2
    // 0x3: RX pad 3
    hri_sercomusart_write_CTRLA_RXPO_bf(sercom, rx_pad);

    // Enable tx and/or rx based on whether the pins were specified.
    hri_sercomusart_write_CTRLB_TXEN_bit(sercom, have_tx);
    hri_sercomusart_write_CTRLB_RXEN_bit(sercom, have_rx);

    // Set parity, baud rate, stop bits, etc. 9-bit bytes not supported.
    usart_async_set_parity(usart_desc_p, parity == PARITY_NONE ? USART_PARITY_NONE :
                           (parity == PARITY_ODD ? USART_PARITY_ODD : USART_PARITY_EVEN));
    usart_async_set_stopbits(usart_desc_p, stop == 1 ? USART_STOP_BITS_ONE : USART_STOP_BITS_TWO);
    // This field is 0 for 8 bits, 5, 6, 7 for 5, 6, 7 bits. 1 for 9 bits, but we don't support that.
    usart_async_set_character_size(usart_desc_p, bits % 8);
    common_hal_busio_uart_set_baudrate(self, baudrate);

    // Turn on rx interrupt handling. The UART async driver has its own set of internal callbacks,
    // which are set up by uart_async_init(). These in turn can call user-specified callbacks.
    // In fact, the actual interrupts are not enabled unless we set up a user-specified callback.
    // This is confusing. It's explained in the Atmel START User Guide -> Implementation Description ->
    // Different read function behavior in some asynchronous drivers. As of this writing:
    // http://start.atmel.com/static/help/index.html?GUID-79201A5A-226F-4FBB-B0B8-AB0BE0554836
    // Look at the ASFv4 code example for async USART.
    usart_async_register_callback(usart_desc_p, USART_ASYNC_RXC_CB, usart_async_rxc_callback);


    if (have_tx) {
        gpio_set_pin_direction(tx->pin, GPIO_DIRECTION_OUT);
        gpio_set_pin_pull_mode(tx->pin, GPIO_PULL_OFF);
        gpio_set_pin_function(tx->pin, tx_pinmux);
        self->tx_pin  = tx->pin;
        claim_pin(tx);
    } else {
        self->tx_pin = NO_PIN;
    }

    if (have_rx) {
        gpio_set_pin_direction(rx->pin, GPIO_DIRECTION_IN);
        gpio_set_pin_pull_mode(rx->pin, GPIO_PULL_OFF);
        gpio_set_pin_function(rx->pin, rx_pinmux);
        self->rx_pin  = rx->pin;
        claim_pin(rx);
    } else {
        self->rx_pin = NO_PIN;
    }

    usart_async_enable(usart_desc_p);
}

bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
    return self->rx_pin == NO_PIN && self->tx_pin == NO_PIN;
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
    if (common_hal_busio_uart_deinited(self)) {
        return;
    }
    // This assignment is only here because the usart_async routines take a *const argument.
    struct usart_async_descriptor * const usart_desc_p = (struct usart_async_descriptor * const) &self->usart_desc;
    usart_async_disable(usart_desc_p);
    usart_async_deinit(usart_desc_p);
    reset_pin(self->rx_pin);
    reset_pin(self->tx_pin);
    self->rx_pin = NO_PIN;
    self->tx_pin = NO_PIN;
}

// Read characters.
size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    if (self->rx_pin == NO_PIN) {
        mp_raise_ValueError("No RX pin");
    }

    // This assignment is only here because the usart_async routines take a *const argument.
    struct usart_async_descriptor * const usart_desc_p = (struct usart_async_descriptor * const) &self->usart_desc;

    if (len == 0) {
        // Nothing to read.
        return 0;
    }

    struct io_descriptor *io;
    usart_async_get_io_descriptor(usart_desc_p, &io);

    size_t total_read = 0;
    uint64_t start_ticks = ticks_ms;

    // Busy-wait until timeout or until we've read enough chars.
    while (ticks_ms - start_ticks < self->timeout_ms) {
        // Read as many chars as we can right now, up to len.
        size_t num_read = io_read(io, data, len);

        // Advance pointer in data buffer, and decrease how many chars left to read.
        data += num_read;
        len -= num_read;
        total_read += num_read;
        if (len == 0) {
            // Don't need to read any more: data buf is full.
            break;
        }
        if (num_read > 0) {
            // Reset the timeout on every character read.
            start_ticks = ticks_ms;
        }
#ifdef MICROPY_VM_HOOK_LOOP
        MICROPY_VM_HOOK_LOOP
#endif
    }

    return total_read;
}

// Write characters.
size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    if (self->tx_pin == NO_PIN) {
        mp_raise_ValueError("No TX pin");
    }

    // This assignment is only here because the usart_async routines take a *const argument.
    struct usart_async_descriptor * const usart_desc_p = (struct usart_async_descriptor * const) &self->usart_desc;

    struct io_descriptor *io;
    usart_async_get_io_descriptor(usart_desc_p, &io);

    if (io_write(io, data, len) < 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    // Wait until write is complete or timeout.
    bool done = false;
    uint64_t start_ticks = ticks_ms;
    // Busy-wait for timeout.
    while (ticks_ms - start_ticks < self->timeout_ms) {
        if (usart_async_is_tx_empty(usart_desc_p)) {
            done = true;
            break;
        }
        #ifdef MICROPY_VM_HOOK_LOOP
            MICROPY_VM_HOOK_LOOP
        #endif
    }

    if (!done) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    struct usart_async_status async_status;
    // Could return ERR_BUSY, but if that's true there's already a problem.
    usart_async_get_status(usart_desc_p, &async_status);
    return async_status.txcnt;
}

uint32_t common_hal_busio_uart_get_baudrate(busio_uart_obj_t *self) {
    return self->baudrate;
}

void common_hal_busio_uart_set_baudrate(busio_uart_obj_t *self, uint32_t baudrate) {
    // This assignment is only here because the usart_async routines take a *const argument.
    struct usart_async_descriptor * const usart_desc_p = (struct usart_async_descriptor * const) &self->usart_desc;
    usart_async_set_baud_rate(usart_desc_p,
                              // Samples and ARITHMETIC vs FRACTIONAL must correspond to USART_SAMPR in
                              // hpl_sercom_config.h.
                              _usart_async_calculate_baud_rate(baudrate,  // e.g. 9600 baud
                                                               PROTOTYPE_SERCOM_USART_ASYNC_CLOCK_FREQUENCY,
                                                               16,   // samples
                                                               USART_BAUDRATE_ASYNCH_ARITHMETIC,
                                                               0  // fraction - not used for ARITHMETIC
                                                               ));
    self->baudrate = baudrate;
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    return self->buffer_size;
}

bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    if (self->tx_pin == NO_PIN) {
        return false;
    }
    // This assignment is only here because the usart_async routines take a *const argument.
    const struct _usart_async_device * const usart_device_p =
        (struct _usart_async_device * const) &self->usart_desc.device;
    return _usart_async_is_byte_sent(usart_device_p);
}
