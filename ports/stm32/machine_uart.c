/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2023 Damien P. George
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_uart.c via MICROPY_PY_MACHINE_UART_INCLUDEFILE.

#include <string.h>

#include "py/mperrno.h"
#include "py/mphal.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/runtime/mpirq.h"
#include "uart.h"
#include "irq.h"
#include "pendsv.h"

#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(UART_HWCONTROL_RTS) }, \
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(UART_HWCONTROL_CTS) }, \
    { MP_ROM_QSTR(MP_QSTR_IRQ_RXIDLE), MP_ROM_INT(UART_FLAG_IDLE) }, \
    { MP_ROM_QSTR(MP_QSTR_IRQ_RX), MP_ROM_INT(UART_FLAG_RXNE) }, \

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->is_enabled) {
        #if defined(LPUART1)
        if (self->uart_id == PYB_LPUART_1) {
            mp_printf(print, "UART('LP1')");
        } else
        #elif defined(LPUART2)
        if (self->uart_id == PYB_LPUART_2) {
            mp_printf(print, "UART('LP2')");
        } else
        #endif
        {
            mp_printf(print, "UART(%u)", self->uart_id);
        }
    } else {
        mp_int_t bits;
        uint32_t cr1 = self->uartx->CR1;
        #if defined(UART_CR1_M1)
        if (cr1 & UART_CR1_M1) {
            bits = 7;
        } else if (cr1 & UART_CR1_M0) {
            bits = 9;
        } else {
            bits = 8;
        }
        #else
        if (cr1 & USART_CR1_M) {
            bits = 9;
        } else {
            bits = 8;
        }
        #endif
        if (cr1 & USART_CR1_PCE) {
            bits -= 1;
        }
        #if defined(LPUART1)
        if (self->uart_id == PYB_LPUART_1) {
            mp_printf(print, "UART('LP1', baudrate=%u, bits=%u, parity=",
                uart_get_baudrate(self), bits);
        } else
        #endif
        #if defined(LPUART2)
        if (self->uart_id == PYB_LPUART_2) {
            mp_printf(print, "UART('LP2', baudrate=%u, bits=%u, parity=",
                uart_get_baudrate(self), bits);
        } else
        #endif
        {
            mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=",
                self->uart_id, uart_get_baudrate(self), bits);
        }
        if (!(cr1 & USART_CR1_PCE)) {
            mp_print_str(print, "None");
        } else if (!(cr1 & USART_CR1_PS)) {
            mp_print_str(print, "0");
        } else {
            mp_print_str(print, "1");
        }
        uint32_t cr2 = self->uartx->CR2;
        mp_printf(print, ", stop=%u, flow=",
            ((cr2 >> USART_CR2_STOP_Pos) & 3) == 0 ? 1 : 2);
        uint32_t cr3 = self->uartx->CR3;
        if (!(cr3 & (USART_CR3_CTSE | USART_CR3_RTSE))) {
            mp_print_str(print, "0");
        } else {
            if (cr3 & USART_CR3_RTSE) {
                mp_print_str(print, "RTS");
                if (cr3 & USART_CR3_CTSE) {
                    mp_print_str(print, "|");
                }
            }
            if (cr3 & USART_CR3_CTSE) {
                mp_print_str(print, "CTS");
            }
        }
        mp_printf(print, ", timeout=%u, timeout_char=%u, rxbuf=%u",
            self->timeout, self->timeout_char,
            self->read_buf_len == 0 ? 0 : self->read_buf_len - 1); // -1 to adjust for usable length of buffer
        if (self->mp_irq_trigger != 0) {
            mp_printf(print, "; irq=0x%x", self->mp_irq_trigger);
        }
        mp_print_str(print, ")");
    }
}

/// \method init(baudrate, bits=8, parity=None, stop=1, *, timeout=1000, timeout_char=0, flow=0, read_buf_len=64)
///
/// Initialise the UART bus with the given parameters:
///
///   - `baudrate` is the clock rate.
///   - `bits` is the number of bits per byte, 7, 8 or 9.
///   - `parity` is the parity, `None`, 0 (even) or 1 (odd).
///   - `stop` is the number of stop bits, 1 or 2.
///   - `timeout` is the timeout in milliseconds to wait for the first character.
///   - `timeout_char` is the timeout in milliseconds to wait between characters.
///   - `flow` is RTS | CTS where RTS == 256, CTS == 512
///   - `read_buf_len` is the character length of the read buffer (0 to disable).
static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 9600} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = UART_HWCONTROL_NONE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_read_buf_len, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 64} }, // legacy
    };

    // parse args
    struct {
        mp_arg_val_t baudrate, bits, parity, stop, flow, timeout, timeout_char, rxbuf, read_buf_len;
    } args;
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, (mp_arg_val_t *)&args);

    // baudrate
    uint32_t baudrate = args.baudrate.u_int;

    // parity
    uint32_t bits = args.bits.u_int;
    uint32_t parity;
    if (args.parity.u_obj == mp_const_none) {
        parity = UART_PARITY_NONE;
    } else {
        mp_int_t p = mp_obj_get_int(args.parity.u_obj);
        parity = (p & 1) ? UART_PARITY_ODD : UART_PARITY_EVEN;
        bits += 1; // STs convention has bits including parity
    }

    // number of bits
    if (bits == 8) {
        bits = UART_WORDLENGTH_8B;
    } else if (bits == 9) {
        bits = UART_WORDLENGTH_9B;
    #ifdef UART_WORDLENGTH_7B
    } else if (bits == 7) {
        bits = UART_WORDLENGTH_7B;
    #endif
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("unsupported combination of bits and parity"));
    }

    // stop bits
    uint32_t stop;
    switch (args.stop.u_int) {
        case 1:
            stop = UART_STOPBITS_1;
            break;
        default:
            stop = UART_STOPBITS_2;
            break;
    }

    // flow control
    uint32_t flow = args.flow.u_int;

    // Save attach_to_repl setting because uart_init will disable it.
    bool attach_to_repl = self->attached_to_repl;

    // init UART (if it fails, it's because the port doesn't exist)
    if (!uart_init(self, baudrate, bits, parity, stop, flow)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) doesn't exist"), self->uart_id);
    }

    // Restore attach_to_repl setting so UART still works if attached to dupterm.
    uart_attach_to_repl(self, attach_to_repl);

    // set timeout
    self->timeout = args.timeout.u_int;

    // set timeout_char
    // make sure it is at least as long as a whole character (13 bits to be safe)
    // minimum value is 2ms because sys-tick has a resolution of only 1ms
    self->timeout_char = args.timeout_char.u_int;
    uint32_t min_timeout_char = 13000 / baudrate + 2;
    if (self->timeout_char < min_timeout_char) {
        self->timeout_char = min_timeout_char;
    }

    if (self->is_static) {
        // Static UARTs have fixed memory for the rxbuf and can't be reconfigured.
        if (args.rxbuf.u_int >= 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("UART is static and rxbuf can't be changed"));
        }
        uart_set_rxbuf(self, self->read_buf_len, self->read_buf);
    } else {
        // setup the read buffer
        m_del(byte, self->read_buf, self->read_buf_len << self->char_width);
        if (args.rxbuf.u_int >= 0) {
            // rxbuf overrides legacy read_buf_len
            args.read_buf_len.u_int = args.rxbuf.u_int;
        }
        if (args.read_buf_len.u_int <= 0) {
            // no read buffer
            uart_set_rxbuf(self, 0, NULL);
        } else {
            // read buffer using interrupts
            size_t len = args.read_buf_len.u_int + 1; // +1 to adjust for usable length of buffer
            uint8_t *buf = m_new(byte, len << self->char_width);
            uart_set_rxbuf(self, len, buf);
        }
    }

    // compute actual baudrate that was configured
    uint32_t actual_baudrate = uart_get_baudrate(self);

    // check we could set the baudrate within 5%
    uint32_t baudrate_diff;
    if (actual_baudrate > baudrate) {
        baudrate_diff = actual_baudrate - baudrate;
    } else {
        baudrate_diff = baudrate - actual_baudrate;
    }
    if (20 * baudrate_diff > actual_baudrate) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("set baudrate %d is not within 5%% of desired value"), actual_baudrate);
    }
}

/// \classmethod \constructor(bus, ...)
///
/// Construct a UART object on the given bus.  `bus` can be 1-6, or 'XA', 'XB', 'YA', or 'YB'.
/// With no additional parameters, the UART object is created but not
/// initialised (it has the settings from the last initialisation of
/// the bus, if any).  If extra arguments are given, the bus is initialised.
/// See `init` for parameters of initialisation.
///
/// The physical pins of the UART buses are:
///
///   - `UART(4)` is on `XA`: `(TX, RX) = (X1, X2) = (PA0, PA1)`
///   - `UART(1)` is on `XB`: `(TX, RX) = (X9, X10) = (PB6, PB7)`
///   - `UART(6)` is on `YA`: `(TX, RX) = (Y1, Y2) = (PC6, PC7)`
///   - `UART(3)` is on `YB`: `(TX, RX) = (Y9, Y10) = (PB10, PB11)`
///   - `UART(2)` is on: `(TX, RX) = (X3, X4) = (PA2, PA3)`
static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // work out port
    int uart_id = 0;
    if (mp_obj_is_str(args[0])) {
        const char *port = mp_obj_str_get_str(args[0]);
        if (0) {
        #ifdef MICROPY_HW_UART1_NAME
        } else if (strcmp(port, MICROPY_HW_UART1_NAME) == 0) {
            uart_id = PYB_UART_1;
        #endif
        #ifdef MICROPY_HW_UART2_NAME
        } else if (strcmp(port, MICROPY_HW_UART2_NAME) == 0) {
            uart_id = PYB_UART_2;
        #endif
        #ifdef MICROPY_HW_UART3_NAME
        } else if (strcmp(port, MICROPY_HW_UART3_NAME) == 0) {
            uart_id = PYB_UART_3;
        #endif
        #ifdef MICROPY_HW_UART4_NAME
        } else if (strcmp(port, MICROPY_HW_UART4_NAME) == 0) {
            uart_id = PYB_UART_4;
        #endif
        #ifdef MICROPY_HW_UART5_NAME
        } else if (strcmp(port, MICROPY_HW_UART5_NAME) == 0) {
            uart_id = PYB_UART_5;
        #endif
        #ifdef MICROPY_HW_UART6_NAME
        } else if (strcmp(port, MICROPY_HW_UART6_NAME) == 0) {
            uart_id = PYB_UART_6;
        #endif
        #ifdef MICROPY_HW_UART7_NAME
        } else if (strcmp(port, MICROPY_HW_UART7_NAME) == 0) {
            uart_id = PYB_UART_7;
        #endif
        #ifdef MICROPY_HW_UART8_NAME
        } else if (strcmp(port, MICROPY_HW_UART8_NAME) == 0) {
            uart_id = PYB_UART_8;
        #endif
        #ifdef MICROPY_HW_UART9_NAME
        } else if (strcmp(port, MICROPY_HW_UART9_NAME) == 0) {
            uart_id = PYB_UART_9;
        #endif
        #ifdef MICROPY_HW_UART10_NAME
        } else if (strcmp(port, MICROPY_HW_UART10_NAME) == 0) {
            uart_id = PYB_UART_10;
        #endif
        #ifdef MICROPY_HW_LPUART1_NAME
        } else if (strcmp(port, MICROPY_HW_LPUART1_NAME) == 0) {
            uart_id = PYB_LPUART_1;
        #endif
        #ifdef MICROPY_HW_LPUART2_NAME
        } else if (strcmp(port, MICROPY_HW_LPUART2_NAME) == 0) {
            uart_id = PYB_LPUART_2;
        #endif
        #ifdef LPUART1
        } else if (strcmp(port, "LP1") == 0 && uart_exists(PYB_LPUART_1)) {
            uart_id = PYB_LPUART_1;
        #endif
        #ifdef LPUART2
        } else if (strcmp(port, "LP2") == 0 && uart_exists(PYB_LPUART_2)) {
            uart_id = PYB_LPUART_2;
        #endif
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%s) doesn't exist"), port);
        }
    } else {
        uart_id = mp_obj_get_int(args[0]);
        if (!uart_exists(uart_id)) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) doesn't exist"), uart_id);
        }
    }

    // check if the UART is reserved for system use or not
    if (MICROPY_HW_UART_IS_RESERVED(uart_id)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) is reserved"), uart_id);
    }

    machine_uart_obj_t *self;
    if (MP_STATE_PORT(machine_uart_obj_all)[uart_id - 1] == NULL) {
        // create new UART object
        self = m_new0(machine_uart_obj_t, 1);
        self->base.type = &machine_uart_type;
        self->uart_id = uart_id;
        MP_STATE_PORT(machine_uart_obj_all)[uart_id - 1] = self;
    } else {
        // reference existing UART object
        self = MP_STATE_PORT(machine_uart_obj_all)[uart_id - 1];
    }

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        mp_machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(self);
}

// Turn off the UART bus.
static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    uart_deinit(self);
}

// Return number of characters waiting.
static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    return uart_rx_any(self);
}

// Since uart.write() waits up to the last byte, uart.txdone() always returns True.
static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    (void)self;
    return true;
}

// Send a break condition.
static void mp_machine_uart_sendbreak(machine_uart_obj_t *self) {
    #if defined(STM32F0) || defined(STM32F7) || defined(STM32G0) || defined(STM32G4) || defined(STM32H5) || defined(STM32H7) || defined(STM32L0) || defined(STM32L4) || defined(STM32WB) || defined(STM32WL)
    self->uartx->RQR = USART_RQR_SBKRQ; // write-only register
    #else
    self->uartx->CR1 |= USART_CR1_SBK;
    #endif
}

// Write a single character on the bus.  `data` is an integer to write.
// The `data` can be up to 9 bits.
static void mp_machine_uart_writechar(machine_uart_obj_t *self, uint16_t data) {
    // write the character
    int errcode;
    if (uart_tx_wait(self, self->timeout)) {
        uart_tx_data(self, &data, 1, &errcode);
    } else {
        errcode = MP_ETIMEDOUT;
    }

    if (errcode != 0) {
        mp_raise_OSError(errcode);
    }
}

// Receive a single character on the bus.
// Return value: The character read, as an integer.  Returns -1 on timeout.
static mp_int_t mp_machine_uart_readchar(machine_uart_obj_t *self) {
    if (uart_rx_wait(self, self->timeout)) {
        return uart_rx_char(self);
    } else {
        // return -1 on timeout
        return -1;
    }
}

static mp_irq_obj_t *mp_machine_uart_irq(machine_uart_obj_t *self, bool any_args, mp_arg_val_t *args) {
    if (self->mp_irq_obj == NULL) {
        self->mp_irq_trigger = 0;
        self->mp_irq_obj = mp_irq_new(&uart_irq_methods, MP_OBJ_FROM_PTR(self));
    }

    if (any_args) {
        // Check the handler
        mp_obj_t handler = args[MP_IRQ_ARG_INIT_handler].u_obj;
        if (handler != mp_const_none && !mp_obj_is_callable(handler)) {
            mp_raise_ValueError(MP_ERROR_TEXT("handler must be None or callable"));
        }

        // Check the trigger
        mp_uint_t trigger = args[MP_IRQ_ARG_INIT_trigger].u_int;
        mp_uint_t not_supported = trigger & ~MP_UART_ALLOWED_FLAGS;
        if (trigger != 0 && not_supported) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("trigger 0x%08x unsupported"), not_supported);
        }

        // Reconfigure user IRQs
        uart_irq_config(self, false);
        self->mp_irq_obj->handler = handler;
        self->mp_irq_obj->ishard = args[MP_IRQ_ARG_INIT_hard].u_bool;
        self->mp_irq_trigger = trigger;
        uart_irq_config(self, true);
    }

    return self->mp_irq_obj;
}

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    byte *buf = buf_in;

    // check that size is a multiple of character width
    if (size & self->char_width) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }

    // convert byte size to char size
    size >>= self->char_width;

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
        if (self->char_width == CHAR_WIDTH_9BIT) {
            *(uint16_t *)buf = data;
            buf += 2;
        } else {
            *buf++ = data;
        }
        if (--size == 0 || !uart_rx_wait(self, self->timeout_char)) {
            // return number of bytes read
            return buf - orig_buf;
        }
    }
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const byte *buf = buf_in;

    // check that size is a multiple of character width
    if (size & self->char_width) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }

    // wait to be able to write the first character. EAGAIN causes write to return None
    if (!uart_tx_wait(self, self->timeout)) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    // write the data
    size_t num_tx = uart_tx_data(self, buf, size >> self->char_width, errcode);

    if (*errcode == 0 || *errcode == MP_ETIMEDOUT) {
        // return number of bytes written, even if there was a timeout
        *errcode = 0;
        return num_tx << self->char_width;
    } else {
        return MP_STREAM_ERROR;
    }
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && uart_rx_any(self)) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && uart_tx_avail(self)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else if (request == MP_STREAM_FLUSH) {
        // Since uart.write() waits up to the last byte, uart.flush() always succeeds.
        ret = 0;
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}
