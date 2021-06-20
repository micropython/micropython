/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ha Thach for Adafruit Industries
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

#include "lib/utils/interrupt_char.h"
#include "py/mpconfig.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "supervisor/shared/translate.h"

#include "nrfx_uarte.h"
#include "nrf_gpio.h"
#include <string.h>

// expression to examine, and return value in case of failing
#define _VERIFY_ERR(_exp) \
    do { \
        uint32_t _err = (_exp); \
        if (NRFX_SUCCESS != _err) { \
            mp_raise_msg_varg(&mp_type_RuntimeError, translate("error = 0x%08lX"), _err); \
        } \
    } while (0)

static nrfx_uarte_t nrfx_uartes[] = {
    #if NRFX_CHECK(NRFX_UARTE0_ENABLED)
    NRFX_UARTE_INSTANCE(0),
    #endif
    #if NRFX_CHECK(NRFX_UARTE1_ENABLED)
    NRFX_UARTE_INSTANCE(1),
    #endif
};

static uint32_t get_nrf_baud(uint32_t baudrate) {

    static const struct {
        const uint32_t boundary;
        nrf_uarte_baudrate_t uarte_baudraute;
    } baudrate_map[] = {
        { 1200, NRF_UARTE_BAUDRATE_1200 },
        { 2400, NRF_UARTE_BAUDRATE_2400 },
        { 4800, NRF_UARTE_BAUDRATE_4800 },
        { 9600, NRF_UARTE_BAUDRATE_9600 },
        { 14400, NRF_UARTE_BAUDRATE_14400 },
        { 19200, NRF_UARTE_BAUDRATE_19200 },
        { 28800, NRF_UARTE_BAUDRATE_28800 },
        { 31250, NRF_UARTE_BAUDRATE_31250 },
        { 38400, NRF_UARTE_BAUDRATE_38400 },
        { 56000, NRF_UARTE_BAUDRATE_56000 },
        { 57600, NRF_UARTE_BAUDRATE_57600 },
        { 76800, NRF_UARTE_BAUDRATE_76800 },
        { 115200, NRF_UARTE_BAUDRATE_115200 },
        { 230400, NRF_UARTE_BAUDRATE_230400 },
        { 250000, NRF_UARTE_BAUDRATE_250000 },
        { 460800, NRF_UARTE_BAUDRATE_460800 },
        { 921600, NRF_UARTE_BAUDRATE_921600 },
        { 0, NRF_UARTE_BAUDRATE_1000000 },
    };

    size_t i = 0;
    uint32_t boundary;
    do {
        boundary = baudrate_map[i].boundary;
        if (baudrate <= boundary || boundary == 0) {
            return baudrate_map[i].uarte_baudraute;
        }
        i++;
    } while (true);
}

static void uart_callback_irq(const nrfx_uarte_event_t *event, void *context) {
    busio_uart_obj_t *self = (busio_uart_obj_t *)context;

    switch (event->type) {
        case NRFX_UARTE_EVT_RX_DONE:
            if (ringbuf_num_empty(&self->ringbuf) >= event->data.rxtx.bytes) {
                ringbuf_put_n(&self->ringbuf, event->data.rxtx.p_data, event->data.rxtx.bytes);
                // keep receiving
                (void)nrfx_uarte_rx(self->uarte, &self->rx_char, 1);
            } else {
                // receive buffer full, suspend
                self->rx_paused = true;
                nrf_gpio_pin_write(self->rts_pin_number, true);
            }

            break;

        case NRFX_UARTE_EVT_TX_DONE:
            // nothing to do
            break;

        case NRFX_UARTE_EVT_ERROR:
            // Possible Error source is Overrun, Parity, Framing, Break
            // uint32_t errsrc = event->data.error.error_mask;

            ringbuf_put_n(&self->ringbuf, event->data.error.rxtx.p_data, event->data.error.rxtx.bytes);

            // Keep receiving
            (void)nrfx_uarte_rx(self->uarte, &self->rx_char, 1);
            break;

        default:
            break;
    }
}

void uart_reset(void) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(nrfx_uartes); i++) {
        nrfx_uarte_uninit(&nrfx_uartes[i]);
    }
}

void common_hal_busio_uart_construct(busio_uart_obj_t *self,
    const mcu_pin_obj_t *tx, const mcu_pin_obj_t *rx,
    const mcu_pin_obj_t *rts, const mcu_pin_obj_t *cts,
    const mcu_pin_obj_t *rs485_dir, bool rs485_invert,
    uint32_t baudrate, uint8_t bits, busio_uart_parity_t parity, uint8_t stop,
    mp_float_t timeout, uint16_t receiver_buffer_size, byte *receiver_buffer,
    bool sigint_enabled) {

    if (bits != 8) {
        mp_raise_ValueError(translate("Invalid word/bit length"));
    }

    if ((rs485_dir != NULL) || (rs485_invert)) {
        mp_raise_ValueError(translate("RS485 Not yet supported on this device"));
    }

    // Find a free UART peripheral.
    self->uarte = NULL;
    for (size_t i = 0; i < MP_ARRAY_SIZE(nrfx_uartes); i++) {
        if ((nrfx_uartes[i].p_reg->ENABLE & UARTE_ENABLE_ENABLE_Msk) == 0) {
            self->uarte = &nrfx_uartes[i];
            break;
        }
    }

    if (self->uarte == NULL) {
        mp_raise_ValueError(translate("All UART peripherals are in use"));
    }

    if ((tx == NULL) && (rx == NULL)) {
        mp_raise_ValueError(translate("tx and rx cannot both be None"));
    }

    if (receiver_buffer_size == 0) {
        mp_raise_ValueError(translate("Invalid buffer size"));
    }

    if (parity == BUSIO_UART_PARITY_ODD) {
        mp_raise_ValueError(translate("Odd parity is not supported"));
    }

    bool hwfc = rts != NULL || cts != NULL;

    nrfx_uarte_config_t config = {
        .pseltxd = (tx == NULL) ? NRF_UARTE_PSEL_DISCONNECTED : tx->number,
        .pselrxd = (rx == NULL) ? NRF_UARTE_PSEL_DISCONNECTED : rx->number,
        .pselcts = (cts == NULL) ? NRF_UARTE_PSEL_DISCONNECTED : cts->number,
        .pselrts = (rts == NULL) ? NRF_UARTE_PSEL_DISCONNECTED : rts->number,
        .p_context = self,
        .baudrate = get_nrf_baud(baudrate),
        .interrupt_priority = 7,
        .hal_cfg = {
            .hwfc = hwfc ? NRF_UARTE_HWFC_ENABLED : NRF_UARTE_HWFC_DISABLED,
            .parity = (parity == BUSIO_UART_PARITY_NONE) ? NRF_UARTE_PARITY_EXCLUDED : NRF_UARTE_PARITY_INCLUDED
        }
    };

    _VERIFY_ERR(nrfx_uarte_init(self->uarte, &config, uart_callback_irq));

    // Init buffer for rx
    if (rx != NULL) {
        // Initially allocate the UART's buffer in the long-lived part of the
        // heap.  UARTs are generally long-lived objects, but the "make long-
        // lived" machinery is incapable of moving internal pointers like
        // self->buffer, so do it manually.  (However, as long as internal
        // pointers like this are NOT moved, allocating the buffer
        // in the long-lived pool is not strictly necessary)
        // (This is a macro.)
        if (!ringbuf_alloc(&self->ringbuf, receiver_buffer_size, true)) {
            nrfx_uarte_uninit(self->uarte);
            mp_raise_msg(&mp_type_MemoryError, translate("Failed to allocate RX buffer"));
        }

        self->rx_pin_number = rx->number;
        claim_pin(rx);
    }

    if (tx != NULL) {
        self->tx_pin_number = tx->number;
        claim_pin(tx);
    } else {
        self->tx_pin_number = NO_PIN;
    }

    if (rts != NULL) {
        self->rts_pin_number = rts->number;
        claim_pin(rts);
    } else {
        self->rts_pin_number = NO_PIN;
    }

    if (cts != NULL) {
        self->cts_pin_number = cts->number;
        claim_pin(cts);
    } else {
        self->cts_pin_number = NO_PIN;
    }

    self->baudrate = baudrate;
    self->timeout_ms = timeout * 1000;

    self->rx_paused = false;

    // Initial wait for incoming byte
    _VERIFY_ERR(nrfx_uarte_rx(self->uarte, &self->rx_char, 1));
}

bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
    return self->rx_pin_number == NO_PIN;
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
    if (!common_hal_busio_uart_deinited(self)) {
        nrfx_uarte_uninit(self->uarte);
        reset_pin_number(self->tx_pin_number);
        reset_pin_number(self->rx_pin_number);
        reset_pin_number(self->rts_pin_number);
        reset_pin_number(self->cts_pin_number);
        self->tx_pin_number = NO_PIN;
        self->rx_pin_number = NO_PIN;
        self->rts_pin_number = NO_PIN;
        self->cts_pin_number = NO_PIN;
        ringbuf_free(&self->ringbuf);
    }
}

// Read characters.
size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    if (nrf_uarte_rx_pin_get(self->uarte->p_reg) == NRF_UARTE_PSEL_DISCONNECTED) {
        mp_raise_ValueError(translate("No RX pin"));
    }

    uint64_t start_ticks = supervisor_ticks_ms64();

    // check removed to reduce code size
    /*
    if (len > ringbuf_capacity(&self->ringbuf)) {
        mp_raise_ValueError(translate("Reading >receiver_buffer_size bytes is not supported"));
    }
    */

    // Wait for all bytes received or timeout
    while ((ringbuf_num_filled(&self->ringbuf) < len) && (supervisor_ticks_ms64() - start_ticks < self->timeout_ms)) {
        RUN_BACKGROUND_TASKS;
        // Allow user to break out of a timeout with a KeyboardInterrupt.
        if (mp_hal_is_interrupted()) {
            return 0;
        }
    }

    // prevent conflict with uart irq
    NVIC_DisableIRQ(nrfx_get_irq_number(self->uarte->p_reg));

    // Copy as much received data as available, up to len bytes.
    size_t rx_bytes = ringbuf_get_n(&self->ringbuf, data, len);

    // restart reader, if stopped
    if (self->rx_paused) {
        // the character that did not fit in ringbuf is in rx_char
        ringbuf_put_n(&self->ringbuf, &self->rx_char, 1);
        // keep receiving
        (void)nrfx_uarte_rx(self->uarte, &self->rx_char, 1);
        nrf_gpio_pin_write(self->rts_pin_number, false);
        self->rx_paused = false;
    }

    NVIC_EnableIRQ(nrfx_get_irq_number(self->uarte->p_reg));

    return rx_bytes;
}

// Write characters.
size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    if (nrf_uarte_tx_pin_get(self->uarte->p_reg) == NRF_UARTE_PSEL_DISCONNECTED) {
        mp_raise_ValueError(translate("No TX pin"));
    }

    if (len == 0) {
        return 0;
    }

    // EasyDMA can only access SRAM
    uint8_t *tx_buf = (uint8_t *)data;
    if (!nrfx_is_in_ram(data)) {
        // TODO: If this is not too big, we could allocate it on the stack.
        tx_buf = (uint8_t *)gc_alloc(len, false, false);
        memcpy(tx_buf, data, len);
    }

    (*errcode) = nrfx_uarte_tx(self->uarte, tx_buf, len);
    _VERIFY_ERR(*errcode);
    (*errcode) = 0;

    // Wait for write to complete.
    while (nrfx_uarte_tx_in_progress(self->uarte)) {
        RUN_BACKGROUND_TASKS;
    }

    if (!nrfx_is_in_ram(data)) {
        gc_free(tx_buf);
    }

    return len;
}

uint32_t common_hal_busio_uart_get_baudrate(busio_uart_obj_t *self) {
    return self->baudrate;
}

void common_hal_busio_uart_set_baudrate(busio_uart_obj_t *self, uint32_t baudrate) {
    self->baudrate = baudrate;
    nrf_uarte_baudrate_set(self->uarte->p_reg, get_nrf_baud(baudrate));
}

mp_float_t common_hal_busio_uart_get_timeout(busio_uart_obj_t *self) {
    return (mp_float_t)(self->timeout_ms / 1000.0f);
}

void common_hal_busio_uart_set_timeout(busio_uart_obj_t *self, mp_float_t timeout) {
    self->timeout_ms = timeout * 1000;
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    return ringbuf_num_filled(&self->ringbuf);
}

void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self) {
    // prevent conflict with uart irq
    NVIC_DisableIRQ(nrfx_get_irq_number(self->uarte->p_reg));
    ringbuf_clear(&self->ringbuf);
    NVIC_EnableIRQ(nrfx_get_irq_number(self->uarte->p_reg));
}

bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    return !nrfx_uarte_tx_in_progress(self->uarte);
}
