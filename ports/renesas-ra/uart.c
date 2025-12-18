/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2021 Renesas Electronics Corporation
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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/ringbuf.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/runtime/mpirq.h"
#include "uart.h"
#include "irq.h"
#include "pendsv.h"

typedef int (*KEYEX_CB)(uint32_t d);

extern void MP_NORETURN __fatal_error(const char *msg);
#if MICROPY_KBD_EXCEPTION
extern int mp_interrupt_char;
static KEYEX_CB keyex_cb[MICROPY_HW_MAX_UART] = {(KEYEX_CB)NULL};

static int chk_kbd_interrupt(int d) {
    if (d == mp_interrupt_char) {
        mp_sched_keyboard_interrupt();
        return 1;
    } else {
        return 0;
    }
}

static void set_kbd_interrupt(uint32_t ch, void *keyex) {
    ra_sci_rxirq_disable(ch);
    keyex_cb[ch] = (KEYEX_CB)keyex;
    ra_sci_rxirq_enable(ch);
}

#endif

static void uart_rx_cb(uint32_t ch, int d) {
    machine_uart_obj_t *self = MP_STATE_PORT(machine_uart_obj_all)[ch];
    if (self == NULL) {
        // UART object has not been set, so we can't do anything, not
        // even disable the IRQ.  This should never happen.
        return;
    }
    #if defined(MICROPY_HW_UART_REPL)
    if (ch == MICROPY_HW_UART_REPL) {
        #if MICROPY_KBD_EXCEPTION
        if (keyex_cb[ch]) {
            (*keyex_cb[ch])(d);
        }
        #endif

        #if MICROPY_HW_ENABLE_UART_REPL
        ringbuf_put(&stdin_ringbuf, d);
        #endif
    }
    #endif

    if ((self->mp_irq_trigger & UART_IRQ_RXIDLE) && (self->rxidle_state != RXIDLE_INACTIVE)) {
        if (self->rxidle_state == RXIDLE_STANDBY) {
            self->rxidle_timer.base.mode = SOFT_TIMER_MODE_PERIODIC;
            soft_timer_insert(&self->rxidle_timer.base, self->rxidle_ms);
        }
        self->rxidle_state = RXIDLE_ALERT;
    }
    // Check the flags to see if the user handler should be called
    if (self->mp_irq_trigger & UART_IRQ_RX) {
        self->mp_irq_flags = UART_IRQ_RX;
        mp_irq_handler(self->mp_irq_obj);
    }
}

void uart_soft_timer_callback(soft_timer_entry_t *self) {
    machine_uart_obj_t *uart = ((soft_timer_entry_extended_t *)self)->context;
    if (uart->rxidle_state == RXIDLE_ALERT) {
        // At the first call, just switch the state
        uart->rxidle_state = RXIDLE_ARMED;
    } else if (uart->rxidle_state == RXIDLE_ARMED) {
        // At the second call, run the irq callback and stop the timer
        // by setting the mode to SOFT_TIMER_MODE_ONE_SHOT.
        // Calling soft_timer_remove() would fail here.
        self->mode = SOFT_TIMER_MODE_ONE_SHOT;
        uart->rxidle_state = RXIDLE_STANDBY;
        uart->mp_irq_flags = UART_IRQ_RXIDLE;
        mp_irq_handler(uart->mp_irq_obj);
    }
}

void uart_init0(void) {
}

// unregister all interrupt sources
void uart_deinit_all(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(MP_STATE_PORT(machine_uart_obj_all)); i++) {
        machine_uart_obj_t *uart_obj = MP_STATE_PORT(machine_uart_obj_all)[i];
        if (uart_obj != NULL && !uart_obj->is_static) {
            uart_deinit(uart_obj);
            MP_STATE_PORT(machine_uart_obj_all)[i] = NULL;
        }
    }
}

bool uart_exists(int uart_id) {
    if (uart_id > MP_ARRAY_SIZE(MP_STATE_PORT(machine_uart_obj_all))) {
        // safeguard against machine_uart_obj_all array being configured too small
        return false;
    }
    switch (uart_id) {
        #if defined(MICROPY_HW_UART0_TX) && defined(MICROPY_HW_UART0_RX)
        case HW_UART_0:
            return true;
        #endif

        #if defined(MICROPY_HW_UART1_TX) && defined(MICROPY_HW_UART1_RX)
        case HW_UART_1:
            return true;
        #endif

        #if defined(MICROPY_HW_UART2_TX) && defined(MICROPY_HW_UART2_RX)
        case HW_UART_2:
            return true;
        #endif

        #if defined(MICROPY_HW_UART3_TX) && defined(MICROPY_HW_UART3_RX)
        case HW_UART_3:
            return true;
        #endif

        #if defined(MICROPY_HW_UART4_TX) && defined(MICROPY_HW_UART4_RX)
        case HW_UART_4:
            return true;
        #endif

        #if defined(MICROPY_HW_UART5_TX) && defined(MICROPY_HW_UART5_RX)
        case HW_UART_5:
            return true;
        #endif

        #if defined(MICROPY_HW_UART6_TX) && defined(MICROPY_HW_UART6_RX)
        case HW_UART_6:
            return true;
        #endif

        #if defined(MICROPY_HW_UART7_TX) && defined(MICROPY_HW_UART7_RX)
        case HW_UART_7:
            return true;
        #endif

        #if defined(MICROPY_HW_UART8_TX) && defined(MICROPY_HW_UART8_RX)
        case HW_UART_8:
            return true;
        #endif

        #if defined(MICROPY_HW_UART9_TX) && defined(MICROPY_HW_UART9_RX)
        case HW_UART_9:
            return true;
        #endif

        default:
            return false;
    }
}

// assumes Init parameters have been set up correctly
bool uart_init(machine_uart_obj_t *uart_obj,
    uint32_t baudrate, uint32_t bits, uint32_t parity, uint32_t stop, uint32_t flow) {
    uart_obj->baudrate = (uint32_t)baudrate;
    uart_obj->bits = (uint8_t)bits;
    uart_obj->parity = (uint8_t)parity;
    uart_obj->stop = (uint8_t)stop;
    uart_obj->flow = (uint8_t)flow;

    const machine_pin_obj_t *pins[4] = {0};

    switch (uart_obj->uart_id) {
        #if defined(MICROPY_HW_UART0_TX) && defined(MICROPY_HW_UART0_RX)
        case HW_UART_0:
            pins[0] = MICROPY_HW_UART0_TX;
            pins[1] = MICROPY_HW_UART0_RX;
            #if defined(MICROPY_HW_UART0_RTS)
            if (flow) {
                pins[2] = MICROPY_HW_UART0_RTS;
            }
            #endif
            #if defined(MICROPY_HW_UART0_CTS)
            if (flow) {
                pins[3] = MICROPY_HW_UART0_CTS;
            }
            #endif
            break;
        #endif

        #if defined(MICROPY_HW_UART1_TX) && defined(MICROPY_HW_UART1_RX)
        case HW_UART_1:
            pins[0] = MICROPY_HW_UART1_TX;
            pins[1] = MICROPY_HW_UART1_RX;
            #if defined(MICROPY_HW_UART1_RTS)
            if (flow) {
                pins[2] = MICROPY_HW_UART1_RTS;
            }
            #endif
            #if defined(MICROPY_HW_UART1_CTS)
            if (flow) {
                pins[3] = MICROPY_HW_UART1_CTS;
            }
            #endif
            break;
        #endif

        #if defined(MICROPY_HW_UART2_TX) && defined(MICROPY_HW_UART2_RX)
        case HW_UART_2:
            pins[0] = MICROPY_HW_UART2_TX;
            pins[1] = MICROPY_HW_UART2_RX;
            #if defined(MICROPY_HW_UART2_RTS)
            if (flow) {
                pins[2] = MICROPY_HW_UART2_RTS;
            }
            #endif
            #if defined(MICROPY_HW_UART2_CTS)
            if (flow) {
                pins[3] = MICROPY_HW_UART2_CTS;
            }
            #endif
            break;
        #endif

        #if defined(MICROPY_HW_UART3_TX) && defined(MICROPY_HW_UART3_RX)
        case HW_UART_3:
            pins[0] = MICROPY_HW_UART3_TX;
            pins[1] = MICROPY_HW_UART3_RX;
            #if defined(MICROPY_HW_UART3_RTS)
            if (flow) {
                pins[2] = MICROPY_HW_UART3_RTS;
            }
            #endif
            #if defined(MICROPY_HW_UART3_CTS)
            if (flow) {
                pins[3] = MICROPY_HW_UART3_CTS;
            }
            #endif
            break;
        #endif

        #if defined(MICROPY_HW_UART4_TX) && defined(MICROPY_HW_UART4_RX)
        case HW_UART_4:
            pins[0] = MICROPY_HW_UART4_TX;
            pins[1] = MICROPY_HW_UART4_RX;
            #if defined(MICROPY_HW_UART4_RTS)
            if (flow) {
                pins[2] = MICROPY_HW_UART4_RTS;
            }
            #endif
            #if defined(MICROPY_HW_UART4_CTS)
            if (flow) {
                pins[3] = MICROPY_HW_UART4_CTS;
            }
            #endif
            break;
        #endif

        #if defined(MICROPY_HW_UART5_TX) && defined(MICROPY_HW_UART5_RX)
        case HW_UART_5:
            pins[0] = MICROPY_HW_UART5_TX;
            pins[1] = MICROPY_HW_UART5_RX;
            #if defined(MICROPY_HW_UART5_RTS)
            if (flow) {
                pins[2] = MICROPY_HW_UART5_RTS;
            }
            #endif
            #if defined(MICROPY_HW_UART5_CTS)
            if (flow) {
                pins[3] = MICROPY_HW_UART5_CTS;
            }
            #endif
            break;
        #endif

        #if defined(MICROPY_HW_UART6_TX) && defined(MICROPY_HW_UART6_RX)
        case HW_UART_6:
            pins[0] = MICROPY_HW_UART6_TX;
            pins[1] = MICROPY_HW_UART6_RX;
            #if defined(MICROPY_HW_UART6_RTS)
            if (flow) {
                pins[2] = MICROPY_HW_UART6_RTS;
            }
            #endif
            #if defined(MICROPY_HW_UART6_CTS)
            if (flow) {
                pins[3] = MICROPY_HW_UART6_CTS;
            }
            #endif
            break;
        #endif

        #if defined(MICROPY_HW_UART7_TX) && defined(MICROPY_HW_UART7_RX)
        case HW_UART_7:
            pins[0] = MICROPY_HW_UART7_TX;
            pins[1] = MICROPY_HW_UART7_RX;
            #if defined(MICROPY_HW_UART7_RTS)
            if (flow) {
                pins[2] = MICROPY_HW_UART7_RTS;
            }
            #endif
            #if defined(MICROPY_HW_UART7_CTS)
            if (flow) {
                pins[3] = MICROPY_HW_UART7_CTS;
            }
            #endif
            break;
        #endif

        #if defined(MICROPY_HW_UART8_TX) && defined(MICROPY_HW_UART8_RX)
        case HW_UART_8:
            pins[0] = MICROPY_HW_UART8_TX;
            pins[1] = MICROPY_HW_UART8_RX;
            #if defined(MICROPY_HW_UART8_RTS)
            if (flow) {
                pins[2] = MICROPY_HW_UART8_RTS;
            }
            #endif
            #if defined(MICROPY_HW_UART8_CTS)
            if (flow) {
                pins[3] = MICROPY_HW_UART8_CTS;
            }
            #endif
            break;
        #endif

        #if defined(MICROPY_HW_UART9_TX) && defined(MICROPY_HW_UART9_RX)
        case HW_UART_9:
            pins[0] = MICROPY_HW_UART9_TX;
            pins[1] = MICROPY_HW_UART9_RX;
            #if defined(MICROPY_HW_UART9_RTS)
            if (flow) {
                pins[2] = MICROPY_HW_UART9_RTS;
            }
            #endif
            #if defined(MICROPY_HW_UART9_CTS)
            if (flow) {
                pins[3] = MICROPY_HW_UART9_CTS;
            }
            #endif
            break;
        #endif

        default:
            // UART does not exist or is not configured for this board
            return false;
    }
    uart_obj->tx = pins[0];
    uart_obj->rx = pins[1];
    uart_obj->rts = pins[2];
    uart_obj->cts = pins[3];

    if (flow && (uart_obj->rts != 0) && (uart_obj->cts != 0)) {
        ra_sci_init_with_flow(uart_obj->uart_id, (uint32_t)uart_obj->tx->pin, (uint32_t)uart_obj->rx->pin, baudrate, bits, parity, stop, flow, (uint32_t)uart_obj->rts->pin, (uint32_t)uart_obj->cts->pin);
    } else {
        ra_sci_init(uart_obj->uart_id, (uint32_t)uart_obj->tx->pin, (uint32_t)uart_obj->rx->pin, baudrate, bits, parity, stop, flow);
    }
    ra_sci_rx_set_callback((int)uart_obj->uart_id, (SCI_CB)uart_rx_cb);
    uart_obj->is_enabled = true;
    uart_obj->attached_to_repl = false;

    if (bits == 9 && parity == UART_PARITY_NONE) {
        uart_obj->char_mask = 0x1ff;
        uart_obj->char_width = CHAR_WIDTH_9BIT;
    } else {
        if (bits == 9 || parity == UART_PARITY_NONE) {
            uart_obj->char_mask = 0xff;
        } else {
            uart_obj->char_mask = 0x7f;
        }
        uart_obj->char_width = CHAR_WIDTH_8BIT;
    }

    uart_obj->mp_irq_trigger = 0;
    uart_obj->mp_irq_obj = NULL;

    return true;
}

void uart_irq_config(machine_uart_obj_t *self, bool enable) {
    if (self->mp_irq_trigger) {
        if (enable) {
            ra_sci_rxirq_enable(self->uart_id);
        } else {
            ra_sci_rxirq_disable(self->uart_id);
        }
    }
}

void uart_set_rxbuf(machine_uart_obj_t *self, size_t len, void *buf) {
    // len = 0 (no interrupt) is not supported. static buf is used.
    self->read_buf_len = len;
    self->read_buf = buf;
    if (len) {
        int ch = (int)self->uart_id;
        ra_sci_rxfifo_set(ch, (uint8_t *)buf, (uint32_t)len);
    }
}

void uart_deinit(machine_uart_obj_t *self) {
    self->is_enabled = false;
    ra_sci_deinit(self->uart_id);
}

void uart_attach_to_repl(machine_uart_obj_t *self, bool attached) {
    self->attached_to_repl = attached;
    #if MICROPY_KBD_EXCEPTION
    if (attached) {
        set_kbd_interrupt((int)self->uart_id, (SCI_CB)chk_kbd_interrupt);
    } else {
        set_kbd_interrupt((int)self->uart_id, (SCI_CB)NULL);
    }
    #endif
}

mp_uint_t uart_rx_any(machine_uart_obj_t *self) {
    int ch = (int)self->uart_id;
    return ra_sci_rx_any(ch);
}

mp_uint_t uart_tx_avail(machine_uart_obj_t *self) {
    int ch = (int)self->uart_id;
    return ra_sci_tx_wait(ch);
}

mp_uint_t uart_tx_busy(machine_uart_obj_t *self) {
    int ch = (int)self->uart_id;
    return ra_sci_tx_busy(ch);
}

mp_uint_t uart_tx_txbuf(machine_uart_obj_t *self) {
    int ch = (int)self->uart_id;
    return ra_sci_tx_bufsize(ch);
}

// Waits at most timeout milliseconds for at least 1 char to become ready for
// reading (from buf or for direct reading).
// Returns true if something available, false if not.
bool uart_rx_wait(machine_uart_obj_t *self, uint32_t timeout) {
    int ch = (int)self->uart_id;
    uint32_t start = HAL_GetTick();
    for (;;) {
        if (ra_sci_rx_any(ch)) {
            return true;
        }
        if (HAL_GetTick() - start >= timeout) {
            return false; // timeout
        }
        mp_event_wait_ms(1);
    }
}

// assumes there is a character available
int uart_rx_char(machine_uart_obj_t *self) {
    int ch = (int)self->uart_id;
    return ra_sci_rx_ch(ch);
}

// Waits at most timeout milliseconds for TX register to become empty.
// Returns true if can write, false if can't.
bool uart_tx_wait(machine_uart_obj_t *self, uint32_t timeout) {
    uint32_t start = HAL_GetTick();
    for (;;) {
        if (uart_tx_avail(self)) {
            return true;
        }
        if (HAL_GetTick() - start >= timeout) {
            return false; // timeout
        }
        mp_event_wait_ms(1);
    }
}

// src - a pointer to the data to send (16-bit aligned for 9-bit chars)
// num_chars - number of characters to send (9-bit chars count for 2 bytes from src)
// *errcode - returns 0 for success, MP_Exxx on error
// returns the number of characters sent (valid even if there was an error)
size_t uart_tx_data(machine_uart_obj_t *self, const void *src_in, size_t num_chars, int *errcode) {
    int ch = (int)self->uart_id;
    uint8_t *d8 = (uint8_t *)src_in;
    uint16_t *d16 = (uint16_t *)src_in;
    if (num_chars == 0) {
        *errcode = 0;
        return 0;
    }
    int i;
    if (self->char_width == CHAR_WIDTH_9BIT) {
        for (i = 0; i < (int)num_chars; i++) {
            ra_sci_tx_ch(ch, (int)*d16++);
        }
    } else {
        for (i = 0; i < (int)num_chars; i++) {
            ra_sci_tx_ch(ch, (int)*d8++);
        }
    }

    *errcode = 0;
    return (size_t)num_chars;
}

void uart_tx_strn(machine_uart_obj_t *uart_obj, const char *str, uint len) {
    int errcode;
    uart_tx_data(uart_obj, str, len, &errcode);
}

static mp_uint_t uart_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uart_irq_config(self, false);
    uart_irq_configure_timer(self, new_trigger);
    self->mp_irq_trigger = new_trigger;
    uart_irq_config(self, true);
    return 0;
}

static mp_uint_t uart_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (info_type == MP_IRQ_INFO_FLAGS) {
        return self->mp_irq_flags;
    } else if (info_type == MP_IRQ_INFO_TRIGGERS) {
        return self->mp_irq_trigger;
    }
    return 0;
}

const mp_irq_methods_t uart_irq_methods = {
    .trigger = uart_irq_trigger,
    .info = uart_irq_info,
};
