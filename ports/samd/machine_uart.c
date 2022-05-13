/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2022 Robert Hammelrath
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
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/stream.h"
#include "py/ringbuf.h"
#include "sam.h"
#include "utils.h"
#include "pin_cap.h"
#include "clock_config.h"

#define DEFAULT_UART_BAUDRATE (115200)
#define DEFAULT_BUFFER_SIZE (256)
#define MIN_BUFFER_SIZE  (32)
#define MAX_BUFFER_SIZE  (32766)
#define USART_BUFFER_TX  (0)

Sercom *uart_inst[] = SERCOM_INSTS;

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    uint8_t id;
    uint32_t baudrate;
    uint8_t bits;
    uint8_t parity;
    uint8_t stop;
    uint8_t tx;
    sercom_pad_config_t tx_pad_config;
    uint8_t rx;
    sercom_pad_config_t rx_pad_config;
    uint16_t timeout;       // timeout waiting for first char (in ms)
    uint16_t timeout_char;  // timeout waiting between chars (in ms)
    bool new;
    ringbuf_t read_buffer;
    #if USART_BUFFER_TX
    ringbuf_t write_buffer;
    #endif
} machine_uart_obj_t;

extern const mp_obj_type_t machine_uart_type;

machine_uart_obj_t *uart_table[SERCOM_INST_NUM] = {};

STATIC const char *_parity_name[] = {"None", "", "0", "1"};  // Is defined as 0, 2, 3

// Irq handler

// take all bytes from the fifo and store them in the buffer
STATIC void uart_drain_rx_fifo(machine_uart_obj_t *self, Sercom *uart) {
    while (uart->USART.INTFLAG.bit.RXC != 0 && ringbuf_free(&self->read_buffer) > 0) {
        // get a byte from uart and put into the buffer
        ringbuf_put(&(self->read_buffer), uart->USART.DATA.bit.DATA);
    }
}

void common_uart_irq_handler(int uart_id) {
    machine_uart_obj_t *self = uart_table[uart_id];
    // Handle IRQ
    if (self != NULL) {
        Sercom *uart = uart_inst[self->id];
        if (uart->USART.INTFLAG.bit.RXC != 0) {
            // Now handler the incoming data
            uart_drain_rx_fifo(self, uart);
        } else if (uart->USART.INTFLAG.bit.DRE != 0) {
            // handle the outgoing data
        } else {
            // Disable the other interrupts, if set by error
            uart->USART.INTENCLR.reg = (uint8_t) ~(SERCOM_USART_INTENCLR_DRE | SERCOM_USART_INTENCLR_RXC);
        }
    }
}

STATIC void machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=%s, stop=%u, "
        "timeout=%u, timeout_char=%u, rxbuf=%d)",
        self->id, self->baudrate, self->bits, _parity_name[self->parity],
        self->stop + 1, self->timeout, self->timeout_char, self->read_buffer.size - 1);
}

STATIC mp_obj_t machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_tx, ARG_rx,
           ARG_timeout, ARG_timeout_char, ARG_rxbuf, ARG_txbuf};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_INT(-1)} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_tx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_rx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        #if USART_BUFFER_TX
        { MP_QSTR_txbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        #endif
    };

    // Parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Set baudrate if configured.
    if (args[ARG_baudrate].u_int > 0) {
        self->baudrate = args[ARG_baudrate].u_int;
    }

    // Set bits if configured.
    if (args[ARG_bits].u_int > 0) {
        self->bits = args[ARG_bits].u_int;
    }

    // Set parity if configured.
    if (args[ARG_parity].u_obj != MP_OBJ_NEW_SMALL_INT(-1)) {
        if (args[ARG_parity].u_obj == mp_const_none) {
            self->parity = 0;
        } else if (mp_obj_get_int(args[ARG_parity].u_obj) & 1) {
            self->parity = 1;  // odd
        } else {
            self->parity = 2;  // even
        }
    }

    // Set stop bits if configured.
    if (args[ARG_stop].u_int > 0) {
        self->stop = (args[ARG_stop].u_int - 1) & 1;
    }

    // Set TX/RX pins if configured.
    if (args[ARG_tx].u_obj != mp_const_none) {
        self->tx = mp_hal_get_pin_obj(args[ARG_tx].u_obj);
    }
    if (args[ARG_rx].u_obj != mp_const_none) {
        self->rx = mp_hal_get_pin_obj(args[ARG_rx].u_obj);
    }

    // Set timeout if configured.
    if (args[ARG_timeout].u_int >= 0) {
        self->timeout = args[ARG_timeout].u_int;
    }

    // Set timeout_char if configured.
    if (args[ARG_timeout_char].u_int >= 0) {
        self->timeout_char = args[ARG_timeout_char].u_int;
    }

    // Set the RX buffer size if configured.
    size_t rxbuf_len = DEFAULT_BUFFER_SIZE;
    if (args[ARG_rxbuf].u_int > 0) {
        rxbuf_len = args[ARG_rxbuf].u_int;
        if (rxbuf_len < MIN_BUFFER_SIZE) {
            rxbuf_len = MIN_BUFFER_SIZE;
        } else if (rxbuf_len > MAX_BUFFER_SIZE) {
            mp_raise_ValueError(MP_ERROR_TEXT("rxbuf too large"));
        }
    }

    #if USART_BUFFER_TX
    // Set the TX buffer size if configured.
    size_t txbuf_len = DEFAULT_BUFFER_SIZE;
    if (args[ARG_txbuf].u_int > 0) {
        txbuf_len = args[ARG_txbuf].u_int;
        if (txbuf_len < MIN_BUFFER_SIZE) {
            txbuf_len = MIN_BUFFER_SIZE;
        } else if (txbuf_len > MAX_BUFFER_SIZE) {
            mp_raise_ValueError(MP_ERROR_TEXT("txbuf too large"));
        }
    }
    #endif
    // Initialise the UART peripheral if any arguments given, or it was not initialised previously.
    if (n_args > 0 || kw_args->used > 0 || self->new) {
        self->new = false;

        // Check the rx/tx pin assignments
        if (self->tx == 0xff || self->rx == 0xff || (self->tx / 4) != (self->rx / 4)) {
            mp_raise_ValueError(MP_ERROR_TEXT("Non-matching or missing rx/tx"));
        }
        self->rx_pad_config = is_sercom_n(self->rx, self->id);
        self->tx_pad_config = is_sercom_n(self->tx, self->id);

        // Make sure timeout_char is at least as long as a whole character (13 bits to be safe).
        uint32_t min_timeout_char = 13000 / self->baudrate + 1;
        if (self->timeout_char < min_timeout_char) {
            self->timeout_char = min_timeout_char;
        }

        // Allocate the RX/TX buffers.
        ringbuf_alloc(&(self->read_buffer), rxbuf_len + 1);
        MP_STATE_PORT(samd_uart_rx_buffer[self->id]) = self->read_buffer.buf;

        #if USART_BUFFER_TX
        ringbuf_alloc(&(self->write_buffer), txbuf_len + 1);
        MP_STATE_PORT(samd_uart_tx_buffer[self->id]) = self->write_buffer.buf;
        #endif

        // Step 1: Configure the Pin mux.
        mp_hal_set_pin_mux(self->rx, self->rx_pad_config.alt_fct);
        mp_hal_set_pin_mux(self->tx, self->tx_pad_config.alt_fct);

        // Next: Set up the clocks
        enable_sercom_clock(self->id);

        // Next: Configure the USART
        Sercom *uart = uart_inst[self->id];
        // Reset (clear) the peripheral registers.
        while (uart->USART.SYNCBUSY.bit.SWRST) {
        }
        uart->USART.CTRLA.bit.SWRST = 1; // Reset all Registers, disable peripheral
        while (uart->USART.SYNCBUSY.bit.SWRST) {
        }

        uint8_t txpo = self->tx_pad_config.pad_nr;
        #if defined(MCU_SAMD21)
        if (self->tx_pad_config.pad_nr == 2) { // Map pad 2 to TXPO = 1
            txpo = 1;
        }
        #endif

        uart->USART.CTRLA.reg =
            SERCOM_USART_CTRLA_DORD // Data order
            | SERCOM_USART_CTRLA_FORM(self->parity != 0 ? 1 : 0)  // Enable parity or not
            | SERCOM_USART_CTRLA_RXPO(self->rx_pad_config.pad_nr) // Set Pad#
            | SERCOM_USART_CTRLA_TXPO(txpo) // Set Pad#
            | SERCOM_USART_CTRLA_MODE(1) // USART with internal clock
        ;
        uart->USART.CTRLB.reg =
            SERCOM_USART_CTRLB_RXEN   // Enable Rx & Tx
            | SERCOM_USART_CTRLB_TXEN
            | ((self->parity & 1) << SERCOM_USART_CTRLB_PMODE_Pos)
            | (self->stop << SERCOM_USART_CTRLB_SBMODE_Pos)
            | SERCOM_USART_CTRLB_CHSIZE((self->bits & 7) | (self->bits & 1))
        ;
        while (uart->USART.SYNCBUSY.bit.CTRLB) {
        }

        // USART is driven by the clock of GCLK Generator 2, freq in bus_freq
        // baud rate; 65536 * (1 - 16 * 115200/bus_freq)
        uint32_t baud = 65536 - ((uint64_t)(65536 * 16) * self->baudrate + bus_freq / 2) / bus_freq;
        uart->USART.BAUD.bit.BAUD = baud; // Set Baud

        // Enable RXC interrupt
        uart->USART.INTENSET.bit.RXC = 1;
        #if defined(MCU_SAMD21)
        NVIC_EnableIRQ(SERCOM0_IRQn + self->id);
        #elif defined(MCU_SAMD51)
        NVIC_EnableIRQ(SERCOM0_0_IRQn + 4 * self->id + 2);
        #endif

        uart->USART.CTRLA.bit.ENABLE = 1; // Enable the peripheral
        // Wait for the Registers to update.
        while (uart->USART.SYNCBUSY.bit.ENABLE) {
        }
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Get UART bus.
    int uart_id = mp_obj_get_int(args[0]);
    if (uart_id < 0 || uart_id > SERCOM_INST_NUM) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) doesn't exist"), uart_id);
    }

    // Create the UART object and fill it with defaults.
    machine_uart_obj_t *self = mp_obj_malloc(machine_uart_obj_t, &machine_uart_type);
    self->id = uart_id;
    self->baudrate = DEFAULT_UART_BAUDRATE;
    self->bits = 8;
    self->stop = 0;
    self->timeout = 1;
    self->timeout_char = 1;
    self->tx = 0xff;
    self->rx = 0xff;
    self->new = true;
    uart_table[uart_id] = self;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    return machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);
}

// uart.init(baud, [kwargs])
STATIC mp_obj_t machine_uart_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_uart_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_uart_init_obj, 1, machine_uart_init);

STATIC mp_obj_t machine_uart_deinit(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    Sercom *uart = uart_inst[self->id];
    // clear table entry of uart
    uart_table[self->id] = NULL;
    // Disable interrupts
    uart->USART.INTENCLR.reg = 0xff;
    MP_STATE_PORT(samd_uart_rx_buffer[self->id]) = NULL;
    #if USART_BUFFER_TX
    MP_STATE_PORT(samd_uart_tx_buffer[self->id]) = NULL;
    #endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_deinit_obj, machine_uart_deinit);

STATIC mp_obj_t machine_uart_any(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // get all bytes from the fifo first
    uart_drain_rx_fifo(self, uart_inst[self->id]);
    return MP_OBJ_NEW_SMALL_INT(ringbuf_avail(&self->read_buffer));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_any_obj, machine_uart_any);

void uart_deinit_all(void) {
    for (int i = 0; i < SERCOM_INST_NUM; i++) {
        if (uart_table[i] != NULL) {
            machine_uart_deinit((mp_obj_t)uart_table[i]);
        }
    }
}

STATIC const mp_rom_map_elem_t machine_uart_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_uart_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_uart_deinit_obj) },

    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&machine_uart_any_obj) },

    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_uart_locals_dict, machine_uart_locals_dict_table);

STATIC mp_uint_t machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint64_t t = mp_hal_ticks_ms() + self->timeout;
    uint64_t timeout_char = self->timeout_char;
    uint8_t *dest = buf_in;
    Sercom *uart = uart_inst[self->id];

    // t.b.d. Cater timeout for timer wrap after 50 days.
    for (size_t i = 0; i < size; i++) {
        // Wait for the first/next character
        while (ringbuf_avail(&self->read_buffer) == 0) {
            if (uart->USART.INTFLAG.bit.RXC != 0) {
                // Force a few incoming bytes to the buffer
                uart_drain_rx_fifo(self, uart);
                break;
            }
            if (mp_hal_ticks_ms() > t) {  // timed out
                if (i <= 0) {
                    *errcode = MP_EAGAIN;
                    return MP_STREAM_ERROR;
                } else {
                    return i;
                }
            }
            MICROPY_EVENT_POLL_HOOK
        }
        *dest++ = ringbuf_get(&(self->read_buffer));
        t = mp_hal_ticks_ms() + timeout_char;
    }
    return size;
}

STATIC mp_uint_t machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t remaining = size;
    const uint8_t *src = buf_in;
    Sercom *uart = uart_inst[self->id];

    while (remaining--) {
        while (!(uart->USART.INTFLAG.bit.DRE)) {
        }
        uart->USART.DATA.bit.DATA = *src;
        src += 1;
    }

    return size;
}

STATIC mp_uint_t machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    machine_uart_obj_t *self = self_in;
    mp_uint_t ret;
    Sercom *uart = uart_inst[self->id];
    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && (uart->USART.INTFLAG.bit.RXC != 0 || ringbuf_avail(&self->read_buffer) > 0)) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && (uart->USART.INTFLAG.bit.DRE != 0)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

STATIC const mp_stream_p_t uart_stream_p = {
    .read = machine_uart_read,
    .write = machine_uart_write,
    .ioctl = machine_uart_ioctl,
    .is_text = false,
};

const mp_obj_type_t machine_uart_type = {
    { &mp_type_type },
    .name = MP_QSTR_UART,
    .print = machine_uart_print,
    .make_new = machine_uart_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &uart_stream_p,
    .locals_dict = (mp_obj_dict_t *)&machine_uart_locals_dict,
};
