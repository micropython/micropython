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
#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "peripherals/broadcom/cpu.h"
#include "peripherals/broadcom/defines.h"
#include "peripherals/broadcom/gpio.h"
#include "peripherals/broadcom/interrupts.h"
#include "peripherals/broadcom/vcmailbox.h"

#define NO_PIN 0xff

#define UART_INST(uart) (((uart) ? uart1 : uart0))

typedef enum {
    STATUS_FREE = 0,
    STATUS_BUSY,
    STATUS_NEVER_RESET
} uart_status_t;

// The Broadcom chips have two different types of UARTs. UART1 is the "mini-UART"
// that is most available so we've implemented it first. The ARM PL011 UART
// support will be added later. We set NUM_UARTS to 2 here so that we can match
// the indexing even though UART0 isn't supported yet. We currently use this
// UART for debugging so we don't support user use of UART yet.
#define NUM_UARTS 2

static uart_status_t uart_status[NUM_UARTS];

void reset_uart(void) {
    for (uint8_t num = 0; num < NUM_UARTS; num++) {
        if (uart_status[num] == STATUS_BUSY) {
            uart_status[num] = STATUS_FREE;
        }
    }
}

static busio_uart_obj_t *active_uarts[NUM_UARTS];

void UART1_IRQHandler(void) {
    while (UART1->STAT_b.DATA_READY && ringbuf_num_empty(&active_uarts[1]->ringbuf) > 0) {
        ringbuf_put(&active_uarts[1]->ringbuf, (uint8_t)UART1->IO_b.DATA);
    }
    // We couldn't read all pending data (overrun) so clear the FIFO so that the interrupt
    // can finish.
    if (UART1->STAT_b.DATA_READY) {
        UART1->IIR_b.DATA_READY = 1;
    }
}

void common_hal_busio_uart_never_reset(busio_uart_obj_t *self) {
    uart_status[self->uart_id] = STATUS_NEVER_RESET;
}

void common_hal_busio_uart_construct(busio_uart_obj_t *self,
    const mcu_pin_obj_t *tx, const mcu_pin_obj_t *rx,
    const mcu_pin_obj_t *rts, const mcu_pin_obj_t *cts,
    const mcu_pin_obj_t *rs485_dir, bool rs485_invert,
    uint32_t baudrate, uint8_t bits, busio_uart_parity_t parity, uint8_t stop,
    mp_float_t timeout, uint16_t receiver_buffer_size, byte *receiver_buffer,
    bool sigint_enabled) {

    if (bits > 8) {
        mp_raise_ValueError(translate("Invalid word/bit length"));
    }

    if (receiver_buffer_size == 0) {
        mp_raise_ValueError(translate("Invalid buffer size"));
    }

    if ((rs485_dir != NULL) || (rs485_invert)) {
        mp_raise_NotImplementedError(translate("RS485 Not yet supported on this device"));
    }

    if (tx == &pin_GPIO14) {
        if (rx == &pin_GPIO15) {
            self->uart_id = 1;
        }
    }

    self->rx_pin = rx;
    self->tx_pin = tx;

    if (rx != NULL) {
        if (receiver_buffer != NULL) {
            self->ringbuf = (ringbuf_t) { receiver_buffer, receiver_buffer_size };
        } else {
            // Initially allocate the UART's buffer in the long-lived part of the
            // heap. UARTs are generally long-lived objects, but the "make long-
            // lived" machinery is incapable of moving internal pointers like
            // self->buffer, so do it manually.  (However, as long as internal
            // pointers like this are NOT moved, allocating the buffer
            // in the long-lived pool is not strictly necessary)
            // (This is a macro.)
            if (!ringbuf_alloc(&self->ringbuf, receiver_buffer_size, true)) {
                mp_raise_msg(&mp_type_MemoryError, translate("Failed to allocate RX buffer"));
            }
        }
    }

    if (self->uart_id == 1) {
        active_uarts[1] = self;
        AUX->ENABLES_b.UART_1 = true;

        UART1->IER = 0;
        UART1->CNTL = 0;
        if (bits == 8) {
            UART1->LCR_b.DATA_SIZE = UART1_LCR_DATA_SIZE_MODE_8BIT;
        } else if (bits == 7) {
            UART1->LCR_b.DATA_SIZE = UART1_LCR_DATA_SIZE_MODE_7BIT;
        }
        UART1->MCR = 0;
        UART1->IER = 0;

        // Clear interrupts
        UART1->IIR = 0xff;
        uint32_t source_clock = vcmailbox_get_clock_rate_measured(VCMAILBOX_CLOCK_CORE);
        UART1->BAUD = ((source_clock / (baudrate * 8)) - 1);
        if (tx != NULL) {
            UART1->CNTL |= UART1_CNTL_TX_ENABLE_Msk;
            gpio_set_pull(14, BP_PULL_NONE);
            gpio_set_function(14, GPIO_GPFSEL1_FSEL14_TXD1);
        }
        if (rx != NULL) {
            UART1->CNTL |= UART1_CNTL_RX_ENABLE_Msk;
            gpio_set_pull(15, BP_PULL_NONE);
            gpio_set_function(15, GPIO_GPFSEL1_FSEL15_RXD1);
        }
        UART1->IER_b.DATA_READY = true;
        // Never disable this in case the SPIs are used. They can each be
        // disabled at the peripheral itself.
        BP_EnableIRQ(AUX_IRQn);
    }
}

bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
    return self->tx_pin == NULL && self->rx_pin == NULL;
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
    if (common_hal_busio_uart_deinited(self)) {
        return;
    }
    if (self->uart_id == 1) {
        UART1->IER_b.DATA_READY = false;
        UART1->CNTL = 0;
        AUX->ENABLES_b.UART_1 = false;
        active_uarts[1] = NULL;
    }
    ringbuf_free(&self->ringbuf);
    uart_status[self->uart_id] = STATUS_FREE;
    common_hal_reset_pin(self->tx_pin);
    common_hal_reset_pin(self->rx_pin);
    common_hal_reset_pin(self->cts_pin);
    common_hal_reset_pin(self->rts_pin);
    self->tx_pin = NULL;
    self->rx_pin = NULL;
    self->cts_pin = NULL;
    self->rts_pin = NULL;
}

// Write characters.
size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    if (self->tx_pin == NULL) {
        mp_raise_ValueError(translate("No TX pin"));
    }

    if (self->uart_id == 1) {
        COMPLETE_MEMORY_READS;
        for (size_t i = 0; i < len; i++) {
            // Wait for the FIFO to have space.
            while (!UART1->STAT_b.TX_READY) {
                RUN_BACKGROUND_TASKS;
            }
            UART1->IO = data[i];
        }
        COMPLETE_MEMORY_READS;
        return len;
    }

    return 0;
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
    COMPLETE_MEMORY_READS;

    // Prevent conflict with uart irq.
    if (self->uart_id == 1) {
        UART1->IER_b.DATA_READY = false;
    }

    // Copy as much received data as available, up to len bytes.
    size_t total_read = ringbuf_get_n(&self->ringbuf, data, len);

    // Check if we still need to read more data.
    if (len > total_read) {
        len -= total_read;
        uint64_t start_ticks = supervisor_ticks_ms64();
        // Busy-wait until timeout or until we've read enough chars.
        while (len > 0 && (supervisor_ticks_ms64() - start_ticks < self->timeout_ms)) {
            if (UART1->STAT_b.DATA_READY) {
                // Read and advance.
                data[total_read] = UART1->IO_b.DATA;

                // Adjust the counters.
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
        }
    }

    // Now that we've emptied the ringbuf some, fill it up with anything in the
    // FIFO. This ensures that we'll empty the FIFO as much as possible and
    // reset the interrupt when we catch up.
    while (UART1->STAT_b.DATA_READY && ringbuf_num_empty(&self->ringbuf) > 0) {
        ringbuf_put(&self->ringbuf, (uint8_t)UART1->IO_b.DATA);
    }

    // Re-enable irq.
    if (self->uart_id == 1) {
        UART1->IER_b.DATA_READY = true;
    }

    COMPLETE_MEMORY_READS;
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
}

mp_float_t common_hal_busio_uart_get_timeout(busio_uart_obj_t *self) {
    return (mp_float_t)(self->timeout_ms / 1000.0L);
}

void common_hal_busio_uart_set_timeout(busio_uart_obj_t *self, mp_float_t timeout) {
    self->timeout_ms = timeout * 1000;
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    return ringbuf_num_filled(&self->ringbuf);
}

void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self) {
    ringbuf_clear(&self->ringbuf);
}

bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    if (self->tx_pin == NULL) {
        return false;
    }
    if (self->uart_id == 1) {
        return UART1->STAT_b.TX_READY;
    }
    return false;
}
