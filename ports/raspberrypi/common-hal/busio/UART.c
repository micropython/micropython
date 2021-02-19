/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 microDev
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

#include "shared-bindings/busio/UART.h"

#include "py/stream.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "supervisor/shared/tick.h"
#include "lib/utils/interrupt_char.h"

#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"

#define NO_PIN 0xff

#define UART_INST(uart) (((uart) ? uart1 : uart0))

#define TX  0
#define RX  1
#define CTS 2
#define RTS 3

typedef enum {
    STATUS_FREE = 0,
    STATUS_IN_USE,
    STATUS_NEVER_RESET
} uart_status_t;

static uart_status_t uart_status[2];

void uart_reset(void) {
    for (uint8_t num = 0; num < 2; num++) {
        if (uart_status[num] == STATUS_IN_USE) {
            uart_status[num] = STATUS_FREE;
            uart_deinit(UART_INST(num));
        }
    }
}

void never_reset_uart(uint8_t num) {
    uart_status[num] = STATUS_NEVER_RESET;
}

static uint8_t get_free_uart() {
    uint8_t num;
    for (num = 0; num < 2; num++) {
        if (uart_status[num] == STATUS_FREE) {
            break;
        }
        if (num) {
            mp_raise_RuntimeError(translate("All UART peripherals in use"));
        }
    }
    return num;
}

static uint8_t pin_init(const uint8_t uart, const mcu_pin_obj_t * pin, const uint8_t pin_type) {
    if (pin == NULL) {
        return NO_PIN;
    }
    if (!(((pin->number & 3) == pin_type) && ((((pin->number + 4) & 8) >> 3) == uart))) {
        mp_raise_ValueError(translate("Invalid pins"));
    }
    gpio_set_function(pin->number, GPIO_FUNC_UART);
    return pin->number;
}

void common_hal_busio_uart_construct(busio_uart_obj_t *self,
        const mcu_pin_obj_t * tx, const mcu_pin_obj_t * rx,
        const mcu_pin_obj_t * rts, const mcu_pin_obj_t * cts,
        const mcu_pin_obj_t * rs485_dir, bool rs485_invert,
        uint32_t baudrate, uint8_t bits, busio_uart_parity_t parity, uint8_t stop,
        mp_float_t timeout, uint16_t receiver_buffer_size, byte* receiver_buffer,
        bool sigint_enabled) {

    if ((rs485_dir != NULL) || (rs485_invert)) {
        mp_raise_NotImplementedError(translate("RS485 is not supported on this board"));
    }

    uint8_t uart_id = get_free_uart();

    self->tx_pin = pin_init(uart_id, tx, TX);
    self->rx_pin = pin_init(uart_id, rx, RX);
    self->cts_pin = pin_init(uart_id, cts, CTS);
    self->rts_pin = pin_init(uart_id, rts, RTS);

    self->uart = UART_INST(uart_id);
    self->baudrate = baudrate;
    self->timeout_ms = timeout * 1000;

    uart_init(self->uart, self->baudrate);
    uart_set_fifo_enabled(self->uart, true);
    uart_set_format(self->uart, bits, stop, parity);
    uart_set_hw_flow(self->uart, (cts != NULL), (rts != NULL));
}

bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
    return self->tx_pin == NO_PIN && self->rx_pin == NO_PIN;
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
    if (common_hal_busio_uart_deinited(self)) {
        return;
    }
    uart_deinit(self->uart);
    reset_pin_number(self->tx_pin);
    reset_pin_number(self->rx_pin);
    reset_pin_number(self->cts_pin);
    reset_pin_number(self->rts_pin);
    self->tx_pin = NO_PIN;
    self->rx_pin = NO_PIN;
    self->cts_pin = NO_PIN;
    self->rts_pin = NO_PIN;
}

// Write characters.
size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    if (self->tx_pin == NO_PIN) {
        mp_raise_ValueError(translate("No TX pin"));
    }

    while (len > 0) {
        if (uart_is_writable(self->uart)) {
            // Write and advance.
            uart_get_hw(self->uart)->dr = *data++;
            // Decrease how many chars left to write.
            len--;
        }
        RUN_BACKGROUND_TASKS;
    }

    return len;
}

// Read characters.
size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    if (self->rx_pin == NO_PIN) {
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
        if (uart_is_readable(self->uart)) {
            // Read and advance.
            *data++ = uart_get_hw(self->uart)->dr;

            // Decrease how many chars left to read.
            len--;
            total_read++;

            // Reset the timeout on every character read.
            start_ticks = supervisor_ticks_ms64();
        }

        RUN_BACKGROUND_TASKS;

        // Allow user to break out of a timeout with a KeyboardInterrupt.
        if (mp_hal_is_interrupted()) {
            break;
        }

        // Don't need to read any more: data buf is full.
        if (len == 0) {
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

uint32_t common_hal_busio_uart_get_baudrate(busio_uart_obj_t *self) {
    return self->baudrate;
}

void common_hal_busio_uart_set_baudrate(busio_uart_obj_t *self, uint32_t baudrate) {
    self->baudrate = baudrate;
    uart_set_baudrate(self->uart, baudrate);
}

mp_float_t common_hal_busio_uart_get_timeout(busio_uart_obj_t *self) {
    return (mp_float_t) (self->timeout_ms / 1000.0f);
}

void common_hal_busio_uart_set_timeout(busio_uart_obj_t *self, mp_float_t timeout) {
    self->timeout_ms = timeout * 1000;
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    return uart_is_readable(self->uart);
}

void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self) {}

// True if there are no characters still to be written.
bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    if (self->tx_pin == NO_PIN) {
        return false;
    }
    return uart_is_writable(self->uart);
}
