/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries LLC
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

#include "components/driver/include/driver/uart.h"

#include "mpconfigport.h"
#include "lib/utils/interrupt_char.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "supervisor/shared/translate.h"
#include "supervisor/shared/tick.h"

void uart_reset(void) {
    for (uart_port_t num = 0; num < UART_NUM_MAX; num++) {
        // Ignore the UART used by the IDF.
        #ifdef CONFIG_CONSOLE_UART_NUM
        if (num == CONFIG_CONSOLE_UART_NUM) {
            continue;
        }
        #endif
        if (uart_is_driver_installed(num)) {
            uart_driver_delete(num);
        }
    }
}

void common_hal_busio_uart_construct(busio_uart_obj_t *self,
    const mcu_pin_obj_t *tx, const mcu_pin_obj_t *rx,
    const mcu_pin_obj_t *rts, const mcu_pin_obj_t *cts,
    const mcu_pin_obj_t *rs485_dir, bool rs485_invert,
    uint32_t baudrate, uint8_t bits, busio_uart_parity_t parity, uint8_t stop,
    mp_float_t timeout, uint16_t receiver_buffer_size, byte *receiver_buffer,
    bool sigint_enabled) {

    if (bits > 8) {
        mp_raise_NotImplementedError(translate("bytes > 8 bits not supported"));
    }

    bool have_tx = tx != NULL;
    bool have_rx = rx != NULL;
    bool have_rts = rts != NULL;
    bool have_cts = cts != NULL;
    bool have_rs485_dir = rs485_dir != NULL;
    if (!have_tx && !have_rx) {
        mp_raise_ValueError(translate("tx and rx cannot both be None"));
    }

    // Filter for sane settings for RS485
    if (have_rs485_dir) {
        if (have_rts || have_cts) {
            mp_raise_ValueError(translate("Cannot specify RTS or CTS in RS485 mode"));
        }
    } else if (rs485_invert) {
        mp_raise_ValueError(translate("RS485 inversion specified when not in RS485 mode"));
    }

    self->timeout_ms = timeout * 1000;

    self->uart_num = UART_NUM_MAX;
    for (uart_port_t num = 0; num < UART_NUM_MAX; num++) {
        if (!uart_is_driver_installed(num)) {
            self->uart_num = num;
        }
    }
    if (self->uart_num == UART_NUM_MAX) {
        mp_raise_ValueError(translate("All UART peripherals are in use"));
    }

    uart_mode_t mode = UART_MODE_UART;
    uart_hw_flowcontrol_t flow_control = UART_HW_FLOWCTRL_DISABLE;
    if (have_rs485_dir) {
        mode = UART_MODE_RS485_HALF_DUPLEX;
        if (!rs485_invert) {
            uart_set_line_inverse(self->uart_num, UART_SIGNAL_DTR_INV);
        }
    } else if (have_rts && have_cts) {
        flow_control = UART_HW_FLOWCTRL_CTS_RTS;
    } else if (have_rts) {
        flow_control = UART_HW_FLOWCTRL_RTS;
    } else if (have_rts) {
        flow_control = UART_HW_FLOWCTRL_CTS;
    }

    if (receiver_buffer_size <= UART_FIFO_LEN) {
        receiver_buffer_size = UART_FIFO_LEN + 8;
    }

    uint8_t rx_threshold = UART_FIFO_LEN - 8;
    // Install the driver before we change the settings.
    if (uart_driver_install(self->uart_num, receiver_buffer_size, 0, 0, NULL, 0) != ESP_OK ||
        uart_set_mode(self->uart_num, mode) != ESP_OK) {
        mp_raise_ValueError(translate("Could not initialize UART"));
    }
    uart_set_hw_flow_ctrl(self->uart_num, flow_control, rx_threshold);

    // Set baud rate
    common_hal_busio_uart_set_baudrate(self, baudrate);

    uart_word_length_t word_length = UART_DATA_8_BITS;
    switch (bits) {
        // Shared bindings prevents data < 7 bits.
        // case 5:
        //     word_length = UART_DATA_5_BITS;
        //     break;
        // case 6:
        //     word_length = UART_DATA_6_BITS;
        //     break;
        case 7:
            word_length = UART_DATA_7_BITS;
            break;
        case 8:
            word_length = UART_DATA_8_BITS;
            break;
        default:
            // Won't hit this because shared-bindings limits to 7-9 bits. We error on 9 above.
            break;
    }
    uart_set_word_length(self->uart_num, word_length);

    uart_parity_t parity_mode = UART_PARITY_DISABLE;
    switch (parity) {
        case BUSIO_UART_PARITY_NONE:
            parity_mode = UART_PARITY_DISABLE;
            break;
        case BUSIO_UART_PARITY_EVEN:
            parity_mode = UART_PARITY_EVEN;
            break;
        case BUSIO_UART_PARITY_ODD:
            parity_mode = UART_PARITY_ODD;
            break;
        default:
            // Won't reach here because the input is an enum that is completely handled.
            break;
    }
    uart_set_parity(self->uart_num, parity_mode);

    // Stop is 1 or 2 always.
    uart_stop_bits_t stop_bits = UART_STOP_BITS_1;
    if (stop == 2) {
        stop_bits = UART_STOP_BITS_2;
    }
    uart_set_stop_bits(self->uart_num, stop_bits);

    self->tx_pin = NULL;
    self->rx_pin = NULL;
    self->rts_pin = NULL;
    self->cts_pin = NULL;
    int tx_num = -1;
    int rx_num = -1;
    int rts_num = -1;
    int cts_num = -1;
    if (have_tx) {
        claim_pin(tx);
        self->tx_pin = tx;
        tx_num = tx->number;
    }

    if (have_rx) {
        claim_pin(rx);
        self->rx_pin = rx;
        rx_num = rx->number;
    }

    if (have_rts) {
        claim_pin(rts);
        self->rts_pin = rts;
        rts_num = rts->number;
    }

    if (have_cts) {
        claim_pin(cts);
        self->cts_pin = cts;
        cts_num = cts->number;
    }

    if (have_rs485_dir) {
        claim_pin(rs485_dir);
        // RTS is used for RS485 direction.
        self->rts_pin = rs485_dir;
        rts_num = rs485_dir->number;
    }
    if (uart_set_pin(self->uart_num, tx_num, rx_num, rts_num, cts_num) != ESP_OK) {
        mp_raise_ValueError(translate("Invalid pins"));
    }
}

bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
    return self->rx_pin == NULL && self->tx_pin == NULL;
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
    if (common_hal_busio_uart_deinited(self)) {
        return;
    }
    uart_driver_delete(self->uart_num);

    common_hal_reset_pin(self->rx_pin);
    common_hal_reset_pin(self->tx_pin);
    common_hal_reset_pin(self->rts_pin);
    common_hal_reset_pin(self->cts_pin);
    self->rx_pin = NULL;
    self->tx_pin = NULL;
    self->cts_pin = NULL;
    self->rts_pin = NULL;
}

// Read characters.
size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    if (self->rx_pin == NULL) {
        mp_raise_ValueError(translate("No RX pin"));
    }
    if (len == 0) {
        // Nothing to read.
        return 0;
    }

    size_t total_read = 0;
    uint64_t start_ticks = supervisor_ticks_ms64();

    // Busy-wait until timeout or until we've read enough chars.
    while (supervisor_ticks_ms64() - start_ticks <= self->timeout_ms) {
        // Read as many chars as we can right now, up to len.
        size_t num_read = uart_read_bytes(self->uart_num, data, len, 0);
        if (num_read < 0) {
            break;
        }

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
            start_ticks = supervisor_ticks_ms64();
        }
        RUN_BACKGROUND_TASKS;
        // Allow user to break out of a timeout with a KeyboardInterrupt.
        if (mp_hal_is_interrupted()) {
            break;
        }
        // If we are zero timeout, make sure we don't loop again (in the event
        // we read in under 1ms)
        if (self->timeout_ms == 0) {
            break;
        }
    }

    if (total_read == 0) {
        *errcode = EAGAIN;
        return MP_STREAM_ERROR;
    }

    return total_read;
}

// Write characters.
size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    if (self->tx_pin == NULL) {
        mp_raise_ValueError(translate("No TX pin"));
    }

    size_t left_to_write = len;
    while (left_to_write > 0) {
        int count = uart_tx_chars(self->uart_num, (const char *)data, left_to_write);
        if (count < 0) {
            *errcode = MP_EAGAIN;
            return MP_STREAM_ERROR;
        }
        left_to_write -= count;
        data += count;
        RUN_BACKGROUND_TASKS;
    }
    while (uart_wait_tx_done(self->uart_num, 0) == ESP_ERR_TIMEOUT) {
        RUN_BACKGROUND_TASKS;
    }

    return len;
}

uint32_t common_hal_busio_uart_get_baudrate(busio_uart_obj_t *self) {
    uint32_t baudrate;
    uart_get_baudrate(self->uart_num, &baudrate);
    return baudrate;
}

void common_hal_busio_uart_set_baudrate(busio_uart_obj_t *self, uint32_t baudrate) {
    if (baudrate > UART_BITRATE_MAX ||
        uart_set_baudrate(self->uart_num, baudrate) != ESP_OK) {
        mp_raise_ValueError(translate("Unsupported baudrate"));
    }
}

mp_float_t common_hal_busio_uart_get_timeout(busio_uart_obj_t *self) {
    return (mp_float_t)(self->timeout_ms / 1000.0f);
}

void common_hal_busio_uart_set_timeout(busio_uart_obj_t *self, mp_float_t timeout) {
    self->timeout_ms = timeout * 1000;
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    size_t count;
    uart_get_buffered_data_len(self->uart_num, &count);
    return count;
}

void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self) {
    uart_flush(self->uart_num);
}

// True if there are no characters still to be written.
bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    if (self->tx_pin == NULL) {
        return false;
    }
    return uart_wait_tx_done(self->uart_num, 0) != ESP_ERR_TIMEOUT;
}
