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

// UART1 is a different peripheral than the rest so it is hardcoded below.
#if BCM_VERSION == 2711
#define NUM_UART (6)
STATIC ARM_UART_PL011_Type *uart[NUM_UART] = {UART0, NULL, UART2, UART3, UART4, UART5};
#else
#define NUM_UART (2)
STATIC ARM_UART_PL011_Type *uart[NUM_UART] = {UART0, NULL};
#endif

typedef enum {
    STATUS_FREE = 0,
    STATUS_BUSY,
    STATUS_NEVER_RESET
} uart_status_t;

static uart_status_t uart_status[NUM_UART];
static busio_uart_obj_t *active_uart[NUM_UART];

void reset_uart(void) {
    bool any_pl011_active = false;
    for (uint8_t num = 0; num < NUM_UART; num++) {
        if (uart_status[num] == STATUS_BUSY) {
            if (num == 1) {
                UART1->IER_b.DATA_READY = false;
                UART1->CNTL = 0;
                COMPLETE_MEMORY_READS;
                AUX->ENABLES_b.UART_1 = false;
            } else {
                ARM_UART_PL011_Type *pl011 = uart[num];
                pl011->CR = 0;
            }
            active_uart[num] = NULL;
            uart_status[num] = STATUS_FREE;
        } else {
            any_pl011_active = any_pl011_active || (num != 1 && uart_status[num] == STATUS_NEVER_RESET);
        }
    }
    if (!any_pl011_active) {
        BP_DisableIRQ(UART_IRQn);
    }
    COMPLETE_MEMORY_READS;
    if (AUX->ENABLES == 0) {
        BP_DisableIRQ(AUX_IRQn);
    }
}

STATIC void fetch_all_from_fifo(busio_uart_obj_t *self) {
    if (self->uart_id == 1) {
        while (UART1->STAT_b.DATA_READY && ringbuf_num_empty(&self->ringbuf) > 0) {
            int c = UART1->IO_b.DATA;
            if (self->sigint_enabled && c == mp_interrupt_char) {
                mp_sched_keyboard_interrupt();
                continue;
            }
            ringbuf_put(&self->ringbuf, c);
        }
    } else {
        ARM_UART_PL011_Type *pl011 = uart[self->uart_id];
        while (!pl011->FR_b.RXFE && ringbuf_num_empty(&self->ringbuf) > 0) {
            int c = pl011->DR_b.DATA;
            if (self->sigint_enabled && c == mp_interrupt_char) {
                mp_sched_keyboard_interrupt();
                continue;
            }
            ringbuf_put(&self->ringbuf, c);
        }
    }
}

void UART1_IRQHandler(void) {
    fetch_all_from_fifo(active_uart[1]);
    // We couldn't read all pending data (overrun) so clear the FIFO so that the interrupt
    // can finish.
    if (UART1->STAT_b.DATA_READY) {
        UART1->IIR_b.DATA_READY = 1;
    }
}

void pl011_IRQHandler(uint8_t index) {
    fetch_all_from_fifo(active_uart[index]);
    // Clear the interrupt in case we weren't able to clear it by emptying the
    // FIFO. (This won't clear the FIFO.)
    ARM_UART_PL011_Type *pl011 = uart[index];
    pl011->ICR = UART0_ICR_RXIC_Msk;
}

void UART0_IRQHandler(void) {
    pl011_IRQHandler(0);
}

#if BCM_VERSION == 2711
void UART2_IRQHandler(void) {
    pl011_IRQHandler(2);
}
void UART3_IRQHandler(void) {
    pl011_IRQHandler(3);
}
void UART4_IRQHandler(void) {
    pl011_IRQHandler(4);
}
void UART5_IRQHandler(void) {
    pl011_IRQHandler(5);
}
#endif

void common_hal_busio_uart_never_reset(busio_uart_obj_t *self) {
    uart_status[self->uart_id] = STATUS_NEVER_RESET;
    common_hal_never_reset_pin(self->tx_pin);
    common_hal_never_reset_pin(self->rx_pin);
    common_hal_never_reset_pin(self->cts_pin);
    common_hal_never_reset_pin(self->rts_pin);
}

void common_hal_busio_uart_construct(busio_uart_obj_t *self,
    const mcu_pin_obj_t *tx, const mcu_pin_obj_t *rx,
    const mcu_pin_obj_t *rts, const mcu_pin_obj_t *cts,
    const mcu_pin_obj_t *rs485_dir, bool rs485_invert,
    uint32_t baudrate, uint8_t bits, busio_uart_parity_t parity, uint8_t stop,
    mp_float_t timeout, uint16_t receiver_buffer_size, byte *receiver_buffer,
    bool sigint_enabled) {

    mp_arg_validate_int_max(bits, 8, MP_QSTR_bits);
    mp_arg_validate_int_min(receiver_buffer_size, 1, MP_QSTR_receiver_buffer_size);

    if ((rs485_dir != NULL) || (rs485_invert)) {
        mp_raise_NotImplementedError(translate("RS485 Not yet supported on this device"));
    }

    size_t instance_index = NUM_UART;
    BP_Function_Enum tx_alt = 0;
    BP_Function_Enum rx_alt = 0;
    BP_Function_Enum rts_alt = 0;
    BP_Function_Enum cts_alt = 0;
    for (size_t i = 0; i < NUM_UART; i++) {
        if (uart_status[i] != STATUS_FREE) {
            continue;
        }
        if (tx != NULL) {
            if (!pin_find_alt(tx, PIN_FUNCTION_UART, i, UART_FUNCTION_TXD, &tx_alt)) {
                continue;
            }
            if (rts != NULL && !pin_find_alt(rts, PIN_FUNCTION_UART, i, UART_FUNCTION_RTS, &rts_alt)) {
                continue;
            }
        }
        if (rx != NULL) {
            if (!pin_find_alt(rx, PIN_FUNCTION_UART, i, UART_FUNCTION_RXD, &rx_alt)) {
                continue;
            }
            if (cts != NULL && !pin_find_alt(cts, PIN_FUNCTION_UART, i, UART_FUNCTION_CTS, &cts_alt)) {
                continue;
            }
        }
        instance_index = i;
        break;
    }
    if (instance_index == NUM_UART) {
        raise_ValueError_invalid_pins();
    }

    self->rx_pin = rx;
    self->tx_pin = tx;
    self->rts_pin = rts;
    self->cts_pin = cts;
    self->sigint_enabled = sigint_enabled;

    if (rx != NULL) {
        // Use the provided buffer when given.
        if (receiver_buffer != NULL) {
            ringbuf_init(&self->ringbuf, receiver_buffer, receiver_buffer_size);
        } else {
            // Initially allocate the UART's buffer in the long-lived part of the
            // heap. UARTs are generally long-lived objects, but the "make long-
            // lived" machinery is incapable of moving internal pointers like
            // self->buffer, so do it manually.  (However, as long as internal
            // pointers like this are NOT moved, allocating the buffer
            // in the long-lived pool is not strictly necessary)
            if (!ringbuf_alloc(&self->ringbuf, receiver_buffer_size, true)) {
                m_malloc_fail(receiver_buffer_size);
            }
        }
    }

    active_uart[self->uart_id] = self;

    ARM_UART_PL011_Type *pl011 = uart[self->uart_id];

    if (self->uart_id == 1) {
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
        common_hal_busio_uart_set_baudrate(self, baudrate);
        if (tx != NULL) {
            UART1->CNTL |= UART1_CNTL_TX_ENABLE_Msk;
        }
        if (rx != NULL) {
            UART1->CNTL |= UART1_CNTL_RX_ENABLE_Msk;
        }
    } else {
        // Ensure the UART is disabled as we configure it.
        pl011->CR_b.UARTEN = false;
        pl011->IMSC = 0;
        pl011->ICR = 0x3ff;

        common_hal_busio_uart_set_baudrate(self, baudrate);

        uint32_t line_control = UART0_LCR_H_FEN_Msk;
        line_control |= (bits - 5) << UART0_LCR_H_WLEN_Pos;
        if (stop == 2) {
            line_control |= UART0_LCR_H_STP2_Msk;
        }
        if (parity != BUSIO_UART_PARITY_NONE) {
            line_control |= UART0_LCR_H_PEN_Msk;
        }
        if (parity == BUSIO_UART_PARITY_EVEN) {
            line_control |= UART0_LCR_H_EPS_Msk;
        }
        pl011->LCR_H = line_control;

        uint32_t control = UART0_CR_UARTEN_Msk;
        if (tx != NULL) {
            control |= UART0_CR_TXE_Msk;
        }
        if (rx != NULL) {
            control |= UART0_CR_RXE_Msk;
        }
        if (cts != NULL) {
            control |= UART0_CR_CTSEN_Msk;
        }
        if (rts != NULL) {
            control |= UART0_CR_RTSEN_Msk;
        }
        pl011->CR = control;
    }

    // Setup the pins after waiting for UART stuff
    COMPLETE_MEMORY_READS;
    if (tx != NULL) {
        gpio_set_pull(tx->number, BP_PULL_NONE);
        gpio_set_function(tx->number, tx_alt);
    }
    if (rx != NULL) {
        gpio_set_pull(rx->number, BP_PULL_NONE);
        gpio_set_function(rx->number, rx_alt);
    }
    if (rts != NULL) {
        gpio_set_pull(rts->number, BP_PULL_NONE);
        gpio_set_function(rts->number, rts_alt);
    }
    if (cts != NULL) {
        gpio_set_pull(cts->number, BP_PULL_NONE);
        gpio_set_function(cts->number, cts_alt);
    }

    // Turn on interrupts
    COMPLETE_MEMORY_READS;
    if (self->uart_id == 1) {
        UART1->IER_b.DATA_READY = true;
        // Never disable this in case the SPIs are used. They can each be
        // disabled at the peripheral itself.
        BP_EnableIRQ(AUX_IRQn);
    } else {
        pl011->IMSC_b.RXIM = true;
        // Never disable this in case the other PL011 UARTs are used.
        BP_EnableIRQ(UART_IRQn);
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
    } else {
        ARM_UART_PL011_Type *pl011 = uart[self->uart_id];
        pl011->CR = 0;
    }
    active_uart[self->uart_id] = NULL;
    ringbuf_deinit(&self->ringbuf);
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

    COMPLETE_MEMORY_READS;
    ARM_UART_PL011_Type *pl011 = uart[self->uart_id];
    for (size_t i = 0; i < len; i++) {
        if (self->uart_id == 1) {
            // Wait for the FIFO to have space.
            while (!UART1->STAT_b.TX_READY) {
                RUN_BACKGROUND_TASKS;
            }
            UART1->IO = data[i];
        } else {
            while (pl011->FR_b.TXFF) {
                RUN_BACKGROUND_TASKS;
            }
            pl011->DR_b.DATA = data[i];
        }
    }
    // Wait for the data to be shifted out
    if (self->uart_id == 1) {
        while (!UART1->STAT_b.TX_DONE) {
            RUN_BACKGROUND_TASKS;
        }
    } else {
        while (pl011->FR_b.BUSY) {
            RUN_BACKGROUND_TASKS;
        }
    }
    COMPLETE_MEMORY_READS;
    return len;
}

STATIC void disable_interrupt(busio_uart_obj_t *self) {
    if (self->uart_id == 1) {
        UART1->IER_b.DATA_READY = false;
    }
}

STATIC void enable_interrupt(busio_uart_obj_t *self) {
    if (self->uart_id == 1) {
        UART1->IER_b.DATA_READY = true;
    }
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
    disable_interrupt(self);

    // Copy as much received data as available, up to len bytes.
    size_t total_read = ringbuf_get_n(&self->ringbuf, data, len);

    // Check if we still need to read more data.
    if (len > total_read) {
        len -= total_read;
        uint64_t start_ticks = supervisor_ticks_ms64();
        // Busy-wait until timeout or until we've read enough chars.
        while (len > 0 && (supervisor_ticks_ms64() - start_ticks < self->timeout_ms)) {
            fetch_all_from_fifo(self);
            size_t additional_read = ringbuf_get_n(&self->ringbuf, data + total_read, len);
            len -= additional_read;
            total_read += additional_read;
            if (additional_read > 0) {
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
    fetch_all_from_fifo(self);

    // Re-enable irq.
    enable_interrupt(self);

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
    if (self->uart_id == 1) {
        uint32_t source_clock = vcmailbox_get_clock_rate_measured(VCMAILBOX_CLOCK_CORE);
        UART1->BAUD = ((source_clock / (baudrate * 8)) - 1);
    } else {
        ARM_UART_PL011_Type *pl011 = uart[self->uart_id];
        bool reenable = false;
        if (pl011->CR_b.UARTEN) {
            pl011->CR_b.UARTEN = false;
            reenable = true;
        }
        uint32_t source_clock = vcmailbox_get_clock_rate_measured(VCMAILBOX_CLOCK_UART);
        uint32_t divisor = 16 * baudrate;
        pl011->IBRD = source_clock / divisor;
        // The fractional divisor is 64ths.
        uint32_t remainder = source_clock % divisor;
        uint32_t per_tick = (divisor / 64) + 1;
        uint32_t adjust = 0;
        if (remainder % per_tick > 0) {
            adjust = 1;
        }
        pl011->FBRD = remainder / per_tick + adjust;
        if (reenable) {
            pl011->CR_b.UARTEN = true;
        }
    }
    self->baudrate = baudrate;
}

mp_float_t common_hal_busio_uart_get_timeout(busio_uart_obj_t *self) {
    return (mp_float_t)(self->timeout_ms / 1000.0L);
}

void common_hal_busio_uart_set_timeout(busio_uart_obj_t *self, mp_float_t timeout) {
    self->timeout_ms = timeout * 1000;
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    fetch_all_from_fifo(self);
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
    return !uart[self->uart_id]->FR_b.TXFF;
}
