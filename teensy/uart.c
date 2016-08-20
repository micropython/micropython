/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "bufhelper.h"
#include "irq.h"
#include "pybioctl.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "uart.h"

#include "kinetis.h"

#define PARITY_NONE     0x00
#define PARITY_EVEN     (UART_C1_PE)
#define PARITY_ODD      (UART_C1_PE | UART_C1_PT)
#define PARITY_MASK     (UART_C1_PE | UART_C1_PT)

#define HW_FLOW_NONE    0
#define HW_FLOW_RTS     UART_MODEM_RXRTSE
#define HW_FLOW_CTS     UART_MODEM_TXCTSE
#define HW_FLOW_MASK    (HW_FLOW_CTS | HW_FLOW_RTS)

struct _pyb_uart_obj_t {
    mp_obj_base_t       base;
    enum IRQ_NUMBER_t   irqn;
    uint32_t            baudrate;
    uint8_t             uart_id;
    uint8_t             bits;
    uint8_t             parity;
    uint8_t             stop;
    uint8_t             flow;
    bool                is_enabled;
    uint16_t            timeout;        // timeout waiting for first char
    uint16_t            timeout_char;   // timeout waiting between chars
    uint16_t            read_buf_len;   // len in chars; buf can hold len-1 chars
    volatile uint16_t   read_buf_head;  // indexes first empty slot
    uint16_t            read_buf_tail;  // indexes first full slot (not full if equals head)
    byte               *read_buf;       // byte or uint16_t, depending on char size
    KINETISK_UART_t    *uart;
};

STATIC bool uart_init(pyb_uart_obj_t *self, mp_obj_t pins_list) {

    mp_obj_t tx_pin = mp_const_none;
    mp_obj_t rx_pin = mp_const_none;
    mp_obj_t rts_pin = mp_const_none;
    mp_obj_t cts_pin = mp_const_none;
    uint8_t fn = AF_FN_UART;
    uint8_t unit;
    uint32_t clock = F_BUS;
    KINETISK_UART_t *uart;
    bool has_fifo = false;

    switch (self->uart_id) {

        #if defined(MICROPY_HW_UART0_NUM)
        case MICROPY_HW_UART0_NUM:
            SIM_SCGC4 |= SIM_SCGC4_UART0;   // Turn on the clock
            uart = &KINETISK_UART0;
            self->irqn = IRQ_UART0_STATUS;
            unit = 0;
            has_fifo = true;
            clock = F_CPU;
            #if defined(MICROPY_HW_UART0_TX)
            tx_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART0_TX);
            #endif
            #if defined(MICROPY_HW_UART0_RX)
            rx_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART0_RX);
            #endif
            #if defined(MICROPY_HW_UART0_RTS)
            rts_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART0_RTS);
            #endif
            #if defined(MICROPY_HW_UART0_CTS)
            cts_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART0_CTS);
            #endif
            break;
        #endif

        #if defined(MICROPY_HW_UART1_NUM)
        case MICROPY_HW_UART1_NUM:
            SIM_SCGC4 |= SIM_SCGC4_UART1;   // Turn on the clock
            uart = &KINETISK_UART1;
            self->irqn = IRQ_UART1_STATUS;
            unit = 1;
            clock = F_CPU;
            has_fifo = true;
            #if defined(MICROPY_HW_UART1_TX)
            tx_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART1_TX);
            #endif
            #if defined(MICROPY_HW_UART1_RX)
            rx_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART1_RX);
            #endif
            #if defined(MICROPY_HW_UART1_RTS)
            rts_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART1_RTS);
            #endif
            #if defined(MICROPY_HW_UART1_CTS)
            cts_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART1_CTS);
            #endif
            break;
        #endif

        #if defined(MICROPY_HW_UART2_NUM)
        case MICROPY_HW_UART2_NUM:
            SIM_SCGC4 |= SIM_SCGC4_UART2;   // Turn on the clock
            uart = &KINETISK_UART2;
            self->irqn = IRQ_UART2_STATUS;
            unit = 2;
            #if defined(MICROPY_HW_UART2_TX)
            tx_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART2_TX);
            #endif
            #if defined(MICROPY_HW_UART2_RX)
            rx_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART2_RX);
            #endif
            #if defined(MICROPY_HW_UART2_RTS)
            rts_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART2_RTS);
            #endif
            #if defined(MICROPY_HW_UART2_CTS)
            cts_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART2_CTS);
            #endif
            break;
        #endif

        #if defined(MICROPY_HW_UART3_NUM)
        case MICROPY_HW_UART3_NUM:
            SIM_SCGC4 |= SIM_SCGC4_UART3;   // Turn on the clock
            uart = &KINETISK_UART3;
            self->irqn = IRQ_UART3_STATUS;
            unit = 3;
            #if defined(MICROPY_HW_UART3_TX)
            tx_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART3_TX);
            #endif
            #if defined(MICROPY_HW_UART3_RX)
            rx_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART3_RX);
            #endif
            #if defined(MICROPY_HW_UART3_RTS)
            rts_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART3_RTS);
            #endif
            #if defined(MICROPY_HW_UART3_CTS)
            cts_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART3_CTS);
            #endif
            break;
        #endif

        #if defined(MICROPY_HW_UART4_NUM)
        case MICROPY_HW_UART4_NUM:
            SIM_SCGC1 |= SIM_SCGC1_UART4;   // Turn on the clock
            uart = &KINETISK_UART4;
            self->irqn = IRQ_UART4_STATUS;
            unit = 4;
            #if defined(MICROPY_HW_UART4_TX)
            tx_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART4_TX);
            #endif
            #if defined(MICROPY_HW_UART4_RX)
            rx_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART4_RX);
            #endif
            #if defined(MICROPY_HW_UART4_RTS)
            rts_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART4_RTS);
            #endif
            #if defined(MICROPY_HW_UART4_CTS)
            cts_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART4_CTS);
            #endif
            break;
        #endif

        #if defined(MICROPY_HW_UART5_NUM)
        case MICROPY_HW_UART5_NUM:
            SIM_SCGC1 |= SIM_SCGC1_UART5;   // Turn on the clock
            uart = &KINETISK_UART5;
            self->irqn = IRQ_UART5_STATUS;
            unit = 5;
            #if defined(MICROPY_HW_UART5_TX)
            tx_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART5_TX);
            #endif
            #if defined(MICROPY_HW_UART5_RX)
            rx_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART5_RX);
            #endif
            #if defined(MICROPY_HW_UART5_RTS)
            rts_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART5_RTS);
            #endif
            #if defined(MICROPY_HW_UART5_CTS)
            cts_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_UART5_CTS);
            #endif
            break;
        #endif

#if 0   // Need to figure out the clocking for this.

        #if defined(MICROPY_HW_LPUART0_NUM)
        case MICROPY_HW_LPUART0_NUM:
            SIM_SCGC2 |= SIM_SCGC2_LPUART0;   // Turn on the clock
            uart = &KINETISK_LPUART0;
            self->irqn = IRQ_LPUART0;
            fn = AF_PIN_FN_LPUART;
            unit = 0;
            #if defined(MICROPY_HW_LPUART0_TX)
            tx_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_LPUART0_TX);
            #endif
            #if defined(MICROPY_HW_LPUART0_RX)
            rx_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_LPUART0_RX);
            #endif
            #if defined(MICROPY_HW_LPUART0_RTS)
            rts_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_LPUART0_RTS);
            #endif
            #if defined(MICROPY_HW_LPUART0_CTS)
            cts_pin = MP_OBJ_FROM_PTR(&MICROPY_HW_LPUART0_CTS);
            #endif
            break;
        #endif
#endif

        default:
            // UART does not exist or is not configured for this board
            return false;
    }

    if (pins_list != MP_OBJ_NULL && pins_list != mp_const_none) {
        // pins were provided - override the defaults
        mp_uint_t n_pins;
        mp_obj_t *pin;
        mp_obj_get_array(pins_list, &n_pins, &pin);
        switch (n_pins) {
            case 4:
                rts_pin = pin[3];
                cts_pin = pin[4];
                // fall thru
            case 2:
                tx_pin = pin[0];
                rx_pin = pin[1];
                break;
            default:
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                                                   "expecting pins to have len 2 or 4"));
        }
    }
    if (pins_list != mp_const_none) {
        // init GPIO
        mp_hal_gpio_clock_enable(GPIO_Port);
        GPIO_InitTypeDef init;
        init.Mode = GPIO_MODE_AF_PP;
        init.Speed = GPIO_SPEED_HIGH;

        if (tx_pin != mp_const_none) {
            init.Pull = GPIO_NOPULL;    // TX is an output
            mp_hal_gpio_set_af(pin_find(tx_pin), &init, fn, unit);
        }
        if (rx_pin != mp_const_none) {
            init.Pull = GPIO_PULLUP;    // RX is an input
            mp_hal_gpio_set_af(pin_find(rx_pin), &init, fn, unit);
        }
        if (rts_pin != mp_const_none) {
            init.Pull = GPIO_NOPULL;    // RTS is an output
            mp_hal_gpio_set_af(pin_find(rts_pin), &init, fn, unit);
        }
        if (cts_pin != mp_const_none) {
            init.Pull = GPIO_PULLUP;    // CTS is an input
            mp_hal_gpio_set_af(pin_find(cts_pin), &init, fn, unit);
        }
    }
    self->uart = uart;

    // UART0 & UART1 are clocked from the core clock (F_CPU)
    // remaining are clocked from the bus clock (F_BUS)

    uint32_t divisor = ((clock * 2) + (self->baudrate >> 1)) / self->baudrate;

    uart->BDH = (divisor >> 13) & 0x1F;
    uart->BDL = (divisor >> 5) & 0xFF;
    uart->C4 = divisor & 0x1F;

    if (has_fifo) {
        uart->C1 = UART_C1_ILT;
        uart->TWFIFO = 2; // tx watermark, causes S1_TDRE to set
        // In theory, we can also use the IDLE interrupt to wake us up and cover
        // the case where fewer characters than RWFIFO have been received,
        // but there are races with that. So to simplify things for right now
        // I'm just going to set RWFIFO to 1.
        uart->RWFIFO = 1; // rx watermark, causes S1_RDRF to set
        uart->PFIFO = UART_PFIFO_TXFE | UART_PFIFO_RXFE;
    } else {
        uart->C1 = 0;
        uart->PFIFO = 0;
    }
    uart->C2 = UART_C2_TE | UART_C2_RE;

    uart->MODEM &= ~HW_FLOW_MASK;
    uart->MODEM |= (self->flow & HW_FLOW_MASK);

    // 7E1 7O1 8N1 8N2 8E1 8O1 9N1 9E1 9O1
    // 3.5/3.6: 8E2 8O2

    uint8_t c1 = uart->C1;
    c1 &= ~(UART_C1_M | PARITY_MASK);
    c1 |= self->parity & PARITY_MASK;
    if (self->bits == 9) {    // includes parity
        c1 |= UART_C1_M;
    }
    uart->C1 = c1;

    if (self->bits == 8 && self->parity == PARITY_NONE && self->stop == 2) {
        // 8N2 is 9-bit with 9th bit always 1
        uart->C1 |= UART_C1_M;
        uart->C3 |= UART_C3_T8;
    }

    // Teensy 3.5 & 3.6 support 2 stop bits.

    self->is_enabled = true;

    return true;
}

STATIC bool wait_on_flag_until_timeout(KINETISK_UART_t *uart, uint8_t flag, bool status, uint32_t timeout) {
    uint32_t tickstart = mp_hal_ticks_ms();

    while (((uart->S1 & flag) != 0) != status) {
        if (timeout != HAL_MAX_DELAY) {
            if (timeout == 0 || (mp_hal_ticks_ms() - tickstart) > timeout) {
                return false;
            }
        }
    }
    return true;
}

STATIC bool uart_wait_txe(pyb_uart_obj_t *self, uint32_t timeout) {
    return wait_on_flag_until_timeout(self->uart, UART_S1_TDRE, true, timeout);
}

STATIC bool uart_wait_tc(pyb_uart_obj_t *self, uint32_t timeout) {
    return wait_on_flag_until_timeout(self->uart, UART_S1_TC, true, timeout);
}

mp_uint_t uart_rx_any(pyb_uart_obj_t *self) {
    int buffer_bytes = self->read_buf_head - self->read_buf_tail;
    if (buffer_bytes < 0) {
        return buffer_bytes + self->read_buf_len;
    }
    if (buffer_bytes > 0) {
        return buffer_bytes;
    }
    return (self->uart->S1 & UART_S1_RDRF) != 0;
}

// Waits at most timeout milliseconds for at least 1 char to become ready for
// reading (from buf or for direct reading).
// Returns true if something available, false if not.
STATIC bool uart_rx_wait(pyb_uart_obj_t *self, uint32_t timeout) {
    KINETISK_UART_t *uart = self->uart;
    uint32_t start = mp_hal_ticks_ms();
    for (;;) {
        if (self->read_buf_tail != self->read_buf_head || (uart->S1 & UART_S1_RDRF) != 0) {
            return true; // have at least 1 char ready for reading
        }
        if (mp_hal_ticks_ms() - start >= timeout) {
            return false; // timeout
        }
        __WFI();
    }
}

int uart_rx_char(pyb_uart_obj_t *self) {
    // This function assumes that a character is already available.
    KINETISK_UART_t *uart = self->uart;
    if (self->read_buf_tail == self->read_buf_head) {
        // unbuffered
        return self->uart->D;
    }
    int data = self->read_buf[self->read_buf_tail];
    self->read_buf_tail = (self->read_buf_tail + 1) % self->read_buf_len;
    if (uart->S1 & UART_S1_RDRF) {
        // UART was stalled by flow control; re-enable IRQ now that we have
        // room in the buffer
        uart->C2 |= UART_C2_RIE;
    }
    return data;
}

void uart_irq_handler(mp_uint_t uart_idx) {
    pyb_uart_obj_t *self = MP_STATE_PORT(pyb_uart_obj_all)[uart_idx];
    if (self == NULL) {
        // UART object has not been set, so we can't do anything, not
        // even disable the IRQ.  This should never happen.
        return;
    }
    KINETISK_UART_t *uart = self->uart;
    while (uart->S1 & UART_S1_RDRF) {
        uint16_t next_head = (self->read_buf_head + 1) % self->read_buf_len;
        if (next_head != self->read_buf_tail) {
            self->read_buf[self->read_buf_head] = uart->D;
            self->read_buf_head = next_head;
        } else {
            // No room: leave char in buf and disable interrupt
            uart->C2 &= ~UART_C2_RIE;
            return;
        }
    }
}

void uart0_status_isr(void) {
    uart_irq_handler(0);
}

void uart1_status_isr(void) {
    uart_irq_handler(1);
}

void uart2_status_isr(void) {
    uart_irq_handler(2);
}

void uart3_status_isr(void) {
    uart_irq_handler(3);
}

void uart4_status_isr(void) {
    uart_irq_handler(4);
}

void uart5_status_isr(void) {
    uart_irq_handler(5);
}


STATIC int uart_tx_data(pyb_uart_obj_t *self, const uint8_t *data, uint16_t len) {
    mp_uint_t   timeout;

    if (self->flow & HW_FLOW_CTS) {
        // CTS can hold off transmission for an arbitrarily long time. Apply
        // the overall timeout rather than the character timeout.
        timeout = self->timeout;
    } else {
        // The timeout specified here is for waiting for the TX data register to
        // become empty (ie between chars), as well as for the final char to be
        // completely transferred.  The default value for timeout_char is long
        // enough for 1 char, but we need to double it to wait for the last char
        // to be transferred to the data register, and then to be transmitted.
        timeout = 2 * self->timeout_char;
    }

    while (len > 0) {
        if (!uart_wait_txe(self, timeout)) {
            return MP_ETIMEDOUT;
        }
        self->uart->D = *data++;
        len--;
    }
    if (!uart_wait_tc(self, timeout)) {
        return MP_ETIMEDOUT;
    }
    return 0;
}

STATIC void uart_tx_char(pyb_uart_obj_t *uart_obj, int c) {
    uint8_t ch = c;
    uart_tx_data(uart_obj, &ch, 1);
}

void uart_tx_strn(pyb_uart_obj_t *uart_obj, const char *str, uint len) {
    uart_tx_data(uart_obj, (uint8_t*)str, len);
}

void uart_tx_strn_cooked(pyb_uart_obj_t *uart_obj, const char *str, uint len) {
    for (const char *top = str + len; str < top; str++) {
        if (*str == '\n') {
            uart_tx_char(uart_obj, '\r');
        }
        uart_tx_char(uart_obj, *str);
    }
}

STATIC void pyb_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_uart_obj_t *self = self_in;
    if (!self->is_enabled) {
        mp_printf(print, "UART(%u)", self->uart_id);
        return;
    }
    KINETISK_UART_t *uart = self->uart;

    mp_int_t bits = self->bits;
    const char *parity = "None";
    if (uart->C1 & UART_C1_PE) {
        parity = uart->C1 & UART_C1_PT ? "1" : "0";
        bits--;
    }
    mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=%s",
        self->uart_id, self->baudrate, bits, parity);

    if (uart->MODEM & (UART_MODEM_RXRTSE | UART_MODEM_TXCTSE)) {
        mp_printf(print, ", flow=");
        if (uart->MODEM & UART_MODEM_RXRTSE) {
            mp_printf(print, "RTS%s", uart->MODEM & UART_MODEM_TXCTSE ? "|CTS" : "");
        } else {
            mp_printf(print, "CTS");
        }
    }
    mp_printf(print, ", stop=%u, timeout=%u, timeout_char=%u, read_buf_len=%u)",
        self->stop,
        self->timeout, self->timeout_char,
        self->read_buf_len == 0 ? 0 : self->read_buf_len - 1); // -1 to adjust for usable length of buffer
}

STATIC mp_obj_t pyb_uart_init_helper(pyb_uart_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 9600} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = HW_FLOW_NONE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_read_buf_len, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 64} },
        { MP_QSTR_pins, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    struct {
        mp_arg_val_t baudrate, bits, parity, stop, flow, timeout, timeout_char, read_buf_len, pins;
    } args;
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, (mp_arg_val_t*)&args);

    // baudrate
    self->baudrate = args.baudrate.u_int;

    // parity
    mp_int_t bits = args.bits.u_int;
    if (args.parity.u_obj == mp_const_none) {
        self->parity = PARITY_NONE;
    } else {
        mp_int_t parity = mp_obj_get_int(args.parity.u_obj);
        self->parity = (parity & 1) ? PARITY_ODD : PARITY_EVEN;
        bits += 1; // Freescale convention has bits including parity
    }

    // number of bits
    if (bits < 8 || bits > 10) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "unsupported combination of bits and parity"));
    }
    self->stop = args.stop.u_int;
    if (self->stop != 1) { // We only support 8N2
        if (bits != 8 || self->stop != 2 || self->parity != PARITY_NONE ) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "unsupported combination of bits, parity, and stop"));
        }
    }
    self->bits = bits;

    // flow control
    self->flow = args.flow.u_int;

    // init UART (if it fails, it's because the port doesn't exist)
    if (!uart_init(self, args.pins.u_obj)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "UART(%d) does not exist", self->uart_id));
    }
    KINETISK_UART_t *uart = self->uart;

    // set timeout
    self->timeout = args.timeout.u_int;

    // set timeout_char
    // make sure it is at least as long as a whole character (13 bits to be safe)
    self->timeout_char = args.timeout_char.u_int;
    uint32_t min_timeout_char = 13000 / self->baudrate + 1;
    if (self->timeout_char < min_timeout_char) {
        self->timeout_char = min_timeout_char;
    }

    // setup the read buffer
    m_del(byte, self->read_buf, self->read_buf_len);
    self->read_buf_head = 0;
    self->read_buf_tail = 0;
    if (args.read_buf_len.u_int <= 0) {
        // no read buffer
        self->read_buf_len = 0;
        self->read_buf = NULL;
        NVIC_DISABLE_IRQ(self->irqn);
        uart->C2 &= ~UART_C2_RIE;
    } else {
        // read buffer using interrupts
        self->read_buf_len = args.read_buf_len.u_int + 1; // +1 to adjust for usable length of buffer
        self->read_buf = m_new(byte, self->read_buf_len);
        uart->C2 |= UART_C2_RIE;
        NVIC_SET_PRIORITY(self->irqn, IRQ_PRI_UART);
        NVIC_ENABLE_IRQ(self->irqn);
    }

    // compute actual baudrate that was configured
    // (this formula assumes UART_OVERSAMPLING_16)
    uint32_t actual_baudrate;
    if (   uart == &KINETISK_UART0
        || uart == &KINETISK_UART1) {
        actual_baudrate = F_CPU;
    } else {
        actual_baudrate = F_BUS;
    }
    actual_baudrate *= 2;

    uint32_t divisor = uart->BDH & 0x1f;
    divisor <<= 8;
    divisor |= uart->BDL;
    divisor <<= 5;
    divisor |= (uart->C4 & 0x1f);
    actual_baudrate /= divisor;

    // check we could set the baudrate within 5%
    uint32_t baudrate_diff;
    if (actual_baudrate > self->baudrate) {
        baudrate_diff = actual_baudrate - self->baudrate;
    } else {
        baudrate_diff = self->baudrate - actual_baudrate;
    }
    self->baudrate = actual_baudrate; // remember actual baudrate for printing
    if (20 * baudrate_diff > self->baudrate) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "set baudrate %d is not within 5%% of desired value", actual_baudrate));
    }

    return mp_const_none;
}

// uart = UART(num, baud=9600, bits=8, parity=None, stop=1, flow=None, timeout=1000, timeout_char=0, read_buf_len=64, *, pins=(TX, RX, RTS, CTS))
STATIC mp_obj_t pyb_uart_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    int uart_id = mp_obj_get_int(args[0]);
    int uart_idx = uart_id - MICROPY_HW_UART0_NUM;
    if (uart_idx < 0 || uart_idx >= MICROPY_HW_NUM_UARTS) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "UART(%d) does not exist", uart_id));
    }

    pyb_uart_obj_t *self;
    if (MP_STATE_PORT(pyb_uart_obj_all)[uart_idx] == NULL) {
        // create new UART object
        self = m_new0(pyb_uart_obj_t, 1);
        self->base.type = &pyb_uart_type;
        self->uart_id = uart_id;
        MP_STATE_PORT(pyb_uart_obj_all)[uart_idx] = self;
    } else {
        // reference existing UART object
        self = MP_STATE_PORT(pyb_uart_obj_all)[uart_idx];
    }

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_uart_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return self;
}

// uart.init(baud=9600, bits=8, parity=None, stop=1, flow=None, timeout=1000, timeout_char=0, read_buf_len=64, *, pins=(TX, RX, RTS, CTS))
STATIC mp_obj_t pyb_uart_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_uart_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_uart_init_obj, 1, pyb_uart_init);

// uart.deinit()
STATIC mp_obj_t pyb_uart_deinit(mp_obj_t self_in) {
    pyb_uart_obj_t *self = self_in;
    self->is_enabled = false;

    KINETISK_UART_t *uart = self->uart;

    NVIC_DISABLE_IRQ(self->irqn);
    uart->C2 = 0;
    uart->PFIFO = 0;
    uart->CFIFO = UART_CFIFO_TXFLUSH | UART_CFIFO_RXFLUSH;

    switch (self->uart_id) {

        #if defined(MICROPY_HW_UART0_NUM)
        case MICROPY_HW_UART0_NUM:
            SIM_SCGC4 &= ~SIM_SCGC4_UART0;  // Turn off the clock
            break;
        #endif

        #if defined(MICROPY_HW_UART1_NUM)
        case MICROPY_HW_UART1_NUM:
            SIM_SCGC4 &= ~SIM_SCGC4_UART1;   // Turn off the clock
            break;
        #endif

        #if defined(MICROPY_HW_UART2_NUM)
        case MICROPY_HW_UART2_NUM:
            SIM_SCGC4 &= ~SIM_SCGC4_UART2;   // Turn off the clock
            break;
        #endif

        #if defined(MICROPY_HW_UART3_NUM)
        case MICROPY_HW_UART3_NUM:
            SIM_SCGC4 &= ~SIM_SCGC4_UART3;   // Turn off the clock
            break;
        #endif

        #if defined(MICROPY_HW_UART4_NUM)
        case MICROPY_HW_UART4_NUM:
            SIM_SCGC1 &= ~SIM_SCGC1_UART4;   // Turn off the clock
            break;
        #endif

        #if defined(MICROPY_HW_UART5_NUM)
        case MICROPY_HW_UART5_NUM:
            SIM_SCGC1 &= ~SIM_SCGC1_UART5;   // Turn off the clock
            break;
        #endif

        #if defined(MICROPY_HW_LPUART0_NUM)
        case MICROPY_HW_LPUART0_NUM:
            SIM_SCGC2 &= ~SIM_SCGC2_LPUART0;   // Turn off the clock
            break;
        #endif
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_deinit_obj, pyb_uart_deinit);

// chars_avail = uart.any()
STATIC mp_obj_t pyb_uart_any(mp_obj_t self_in) {
    pyb_uart_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(uart_rx_any(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_any_obj, pyb_uart_any);

// uart.writechar(char)
STATIC mp_obj_t pyb_uart_writechar(mp_obj_t self_in, mp_obj_t char_in) {
    pyb_uart_obj_t *self = self_in;

    // get the character to write (might be 9 bits)
    uint16_t data = mp_obj_get_int(char_in);

    // write the character
    int status;
    if (uart_wait_txe(self, self->timeout)) {
        status = uart_tx_data(self, (uint8_t*)&data, 1);
    } else {
        status = MP_ETIMEDOUT;
    }

    if (status != 0) {
        mp_raise_OSError(status);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_uart_writechar_obj, pyb_uart_writechar);

// char = uart.readchar()
STATIC mp_obj_t pyb_uart_readchar(mp_obj_t self_in) {
    pyb_uart_obj_t *self = self_in;
    if (uart_rx_wait(self, self->timeout)) {
        return MP_OBJ_NEW_SMALL_INT(uart_rx_char(self));
    } else {
        // return -1 on timeout
        return MP_OBJ_NEW_SMALL_INT(-1);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_readchar_obj, pyb_uart_readchar);

// uart.sendbreak()
STATIC mp_obj_t pyb_uart_sendbreak(mp_obj_t self_in) {
    pyb_uart_obj_t *self = self_in;
    KINETISK_UART_t *uart = self->uart;
    uart->C2 |= UART_C2_SBK;
    uart->C2 &= ~UART_C2_SBK;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_sendbreak_obj, pyb_uart_sendbreak);

STATIC const mp_map_elem_t pyb_uart_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&pyb_uart_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&pyb_uart_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_any), (mp_obj_t)&pyb_uart_any_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&mp_stream_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readall), (mp_obj_t)&mp_stream_readall_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readline), (mp_obj_t)&mp_stream_unbuffered_readline_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto), (mp_obj_t)&mp_stream_readinto_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_stream_write_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_writechar), (mp_obj_t)&pyb_uart_writechar_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readchar), (mp_obj_t)&pyb_uart_readchar_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sendbreak), (mp_obj_t)&pyb_uart_sendbreak_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_RTS), MP_OBJ_NEW_SMALL_INT(HW_FLOW_RTS) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_CTS), MP_OBJ_NEW_SMALL_INT(HW_FLOW_CTS) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_uart_locals_dict, pyb_uart_locals_dict_table);

STATIC mp_uint_t pyb_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    pyb_uart_obj_t *self = self_in;
    byte *buf = buf_in;

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }

    // wait for first char to become available
    if (!uart_rx_wait(self, self->timeout)) {
        // return EAGAIN error to indicate non-blocking (then read() method returns None)
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    // read the data
    byte *orig_buf = buf;
    for (;;) {
        int data = uart_rx_char(self);
        *buf++ = data;
        if (--size == 0 || !uart_rx_wait(self, self->timeout_char)) {
            // return number of bytes read
            return buf - orig_buf;
        }
    }
}

STATIC mp_uint_t pyb_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    pyb_uart_obj_t *self = self_in;
    const byte *buf = buf_in;

    // wait to be able to write the first character. EAGAIN causes write to return None
    if (!uart_wait_txe(self, self->timeout)) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    // write the data
    int rc = uart_tx_data(self, (const uint8_t *)buf, size);
    if (rc == 0 || rc == MP_ETIMEDOUT) {
        // return number of bytes written
        return size;
    }

    *errcode = rc;
    return MP_STREAM_ERROR;
}

STATIC mp_uint_t pyb_uart_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    pyb_uart_obj_t *self = self_in;
    mp_uint_t ret;
    if (request == MP_IOCTL_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_IOCTL_POLL_RD) && uart_rx_any(self)) {
            // rx data is available
            ret |= MP_IOCTL_POLL_RD;
        }
        if ((flags & MP_IOCTL_POLL_WR) && (self->uart->S1 & UART_S1_TDRE) != 0) {
            // tx space is available
            ret |= MP_IOCTL_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

STATIC const mp_stream_p_t uart_stream_p = {
    .read = pyb_uart_read,
    .write = pyb_uart_write,
    .ioctl = pyb_uart_ioctl,
    .is_text = false,
};

const mp_obj_type_t pyb_uart_type = {
    { &mp_type_type },
    .name = MP_QSTR_UART,
    .print = pyb_uart_print,
    .make_new = pyb_uart_make_new,
    .getiter = mp_identity,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &uart_stream_p,
    .locals_dict = (mp_obj_t)&pyb_uart_locals_dict,
};
