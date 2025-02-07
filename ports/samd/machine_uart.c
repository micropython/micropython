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

// This file is never compiled standalone, it's included directly from
// extmod/machine_uart.c via MICROPY_PY_MACHINE_UART_INCLUDEFILE.

#include "py/mphal.h"
#include "py/ringbuf.h"
#include "samd_soc.h"
#include "pin_af.h"
#include "genhdr/pins.h"
#include "shared/runtime/softtimer.h"

#define DEFAULT_UART_BAUDRATE (115200)
#define DEFAULT_BUFFER_SIZE (256)
#define MIN_BUFFER_SIZE  (32)
#define MAX_BUFFER_SIZE  (32766)
#define FLOW_CONTROL_RTS (1)
#define FLOW_CONTROL_CTS (2)

#if MICROPY_PY_MACHINE_UART_IRQ
#define UART_IRQ_RXIDLE (4096)
#define RXIDLE_TIMER_MIN (1)
#define MP_UART_ALLOWED_FLAGS (SERCOM_USART_INTFLAG_RXC | SERCOM_USART_INTFLAG_TXC | UART_IRQ_RXIDLE)

#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_IRQ_RX), MP_ROM_INT(SERCOM_USART_INTFLAG_RXC) }, \
    { MP_ROM_QSTR(MP_QSTR_IRQ_RXIDLE), MP_ROM_INT(UART_IRQ_RXIDLE) }, \
    { MP_ROM_QSTR(MP_QSTR_IRQ_TXIDLE), MP_ROM_INT(SERCOM_USART_INTFLAG_TXC) }, \

enum {
    RXIDLE_INACTIVE,
    RXIDLE_STANDBY,
    RXIDLE_ARMED,
    RXIDLE_ALERT,
};
#else
#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS
#endif

typedef struct _soft_timer_entry_extended_t {
    soft_timer_entry_t base;
    void *context;
} soft_timer_entry_extended_t;

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    uint8_t id;
    uint32_t baudrate;
    uint8_t bits;
    uint8_t parity;
    uint8_t stop;
    uint8_t flow_control;
    uint8_t tx;
    uint8_t rx;
    sercom_pad_config_t tx_pad_config;
    sercom_pad_config_t rx_pad_config;
    #if MICROPY_HW_UART_RTSCTS
    uint8_t rts;
    uint8_t cts;
    sercom_pad_config_t rts_pad_config;
    sercom_pad_config_t cts_pad_config;
    #endif
    uint16_t timeout;       // timeout waiting for first char (in ms)
    uint16_t timeout_char;  // timeout waiting between chars (in ms)
    bool new;
    ringbuf_t read_buffer;
    #if MICROPY_HW_UART_TXBUF
    ringbuf_t write_buffer;
    #endif
    #if MICROPY_PY_MACHINE_UART_IRQ
    uint16_t mp_irq_trigger;   // user IRQ trigger mask
    uint16_t mp_irq_flags;     // user IRQ active IRQ flags
    mp_irq_obj_t *mp_irq_obj;  // user IRQ object
    soft_timer_entry_extended_t rxidle_timer;
    uint8_t rxidle_state;
    uint16_t rxidle_ms;
    #endif
} machine_uart_obj_t;

static const char *_parity_name[] = {"None", "", "0", "1"};  // Is defined as 0, 2, 3

// Irq handler

// take all bytes from the fifo and store them in the buffer
static void uart_drain_rx_fifo(machine_uart_obj_t *self, Sercom *uart) {
    uint8_t bits = self->bits;
    while (uart->USART.INTFLAG.bit.RXC != 0) {
        if (ringbuf_free(&self->read_buffer) >= (bits <= 8 ? 1 : 2)) {
            // get a word from uart and put into the buffer
            if (bits <= 8) {
                ringbuf_put(&(self->read_buffer), uart->USART.DATA.bit.DATA);
            } else {
                uint16_t data = uart->USART.DATA.bit.DATA;
                ringbuf_put(&(self->read_buffer), data);
                ringbuf_put(&(self->read_buffer), data >> 8);
            }
        } else {
            // if the buffer is full, disable the RX interrupt
            // allowing RTS to come up. It will be re-enabled by the next read
            uart->USART.INTENCLR.reg = SERCOM_USART_INTENSET_RXC;
            break;
        }
    }
}

void common_uart_irq_handler(int uart_id) {
    machine_uart_obj_t *self = MP_STATE_PORT(sercom_table[uart_id]);
    // Handle IRQ
    if (self != NULL) {
        Sercom *uart = sercom_instance[self->id];
        #if MICROPY_PY_MACHINE_UART_IRQ
        uint16_t mp_irq_flags = 0;
        #endif
        if (uart->USART.INTFLAG.bit.RXC != 0) {
            // Now handler the incoming data
            uart_drain_rx_fifo(self, uart);
            #if MICROPY_PY_MACHINE_UART_IRQ
            if (ringbuf_avail(&self->read_buffer) > 0) {
                if (self->mp_irq_trigger & UART_IRQ_RXIDLE) {
                    if (self->rxidle_state != RXIDLE_INACTIVE) {
                        if (self->rxidle_state == RXIDLE_STANDBY) {
                            self->rxidle_timer.base.mode = SOFT_TIMER_MODE_PERIODIC;
                            soft_timer_insert(&self->rxidle_timer.base, self->rxidle_ms);
                        }
                        self->rxidle_state = RXIDLE_ALERT;
                    }
                } else {
                    mp_irq_flags = SERCOM_USART_INTFLAG_RXC;
                }
            }
            #endif
        } else if (uart->USART.INTFLAG.bit.DRE != 0) {
            #if MICROPY_HW_UART_TXBUF
            // handle the outgoing data
            if (ringbuf_avail(&self->write_buffer) > 0) {
                if (self->bits <= 8) {
                    uart->USART.DATA.bit.DATA = ringbuf_get(&self->write_buffer);
                } else {
                    uart->USART.DATA.bit.DATA =
                        ringbuf_get(&self->write_buffer) | (ringbuf_get(&self->write_buffer) << 8);
                }
            } else {
                #if MICROPY_PY_MACHINE_UART_IRQ
                // Set the TXIDLE flag
                mp_irq_flags |= SERCOM_USART_INTFLAG_TXC;
                #endif
                // Stop the DRE interrupt if there is no more data
                uart->USART.INTENCLR.reg = SERCOM_USART_INTENCLR_DRE;
            }
            #endif
        }
        // Disable the other interrupts, if set by error
        uart->USART.INTENCLR.reg = (uint8_t) ~(SERCOM_USART_INTENCLR_DRE | SERCOM_USART_INTENCLR_RXC);

        #if MICROPY_PY_MACHINE_UART_IRQ
        // Check the flags to see if the uart user handler should be called
        // The handler for RXIDLE is called in the timer callback
        if (self->mp_irq_trigger & mp_irq_flags) {
            self->mp_irq_flags = mp_irq_flags;
            mp_irq_handler(self->mp_irq_obj);
        }
        #endif
    }
}

#if MICROPY_PY_MACHINE_UART_IRQ
static void uart_soft_timer_callback(soft_timer_entry_t *self) {
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
#endif

// Configure the Sercom device
static void machine_sercom_configure(machine_uart_obj_t *self) {
    Sercom *uart = sercom_instance[self->id];

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
    } else
    #endif
    if (self->tx_pad_config.pad_nr != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid UART pin"));
    }
    #if MICROPY_HW_UART_RTSCTS
    if ((self->flow_control & FLOW_CONTROL_RTS) && self->rts_pad_config.pad_nr == 2) {
        txpo = 2;
        mp_hal_set_pin_mux(self->rts, self->rts_pad_config.alt_fct);
    }
    if ((self->flow_control & FLOW_CONTROL_CTS) && self->cts_pad_config.pad_nr == 3) {
        txpo = 2;
        mp_hal_set_pin_mux(self->cts, self->cts_pad_config.alt_fct);
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

    // USART is driven by the clock of GCLK Generator 2, freq by get_peripheral_freq()
    // baud rate; 65536 * (1 - 16 * 115200/bus_freq)
    uint32_t baud = 65536 - ((uint64_t)(65536 * 16) * self->baudrate + get_peripheral_freq() / 2) / get_peripheral_freq();
    uart->USART.BAUD.bit.BAUD = baud; // Set Baud

    sercom_register_irq(self->id, &common_uart_irq_handler);

    // Enable RXC interrupt
    uart->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC;
    #if defined(MCU_SAMD21)
    NVIC_EnableIRQ(SERCOM0_IRQn + self->id);
    #elif defined(MCU_SAMD51)
    NVIC_EnableIRQ(SERCOM0_0_IRQn + 4 * self->id + 2);
    #endif
    #if MICROPY_HW_UART_TXBUF
    // Enable DRE interrupt
    // SAMD21 has just 1 IRQ for all USART events, so no need for an additional NVIC enable
    #if defined(MCU_SAMD51)
    NVIC_EnableIRQ(SERCOM0_0_IRQn + 4 * self->id + 0);
    #endif
    #endif

    sercom_enable(uart, 1);
}

void machine_uart_set_baudrate(mp_obj_t self_in, uint32_t baudrate) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->baudrate = baudrate;
    machine_sercom_configure(self);
}

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t rxbuf_len = self->read_buffer.size - 1;
    #if MICROPY_HW_UART_TXBUF
    size_t txbuf_len = self->write_buffer.size - 1;
    #endif
    if (self->bits > 8) {
        rxbuf_len /= 2;
        #if MICROPY_HW_UART_TXBUF
        txbuf_len /= 2;
        #endif
    }

    mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=%s, stop=%u, "
        "tx=\"%q\", rx=\"%q\", timeout=%u, timeout_char=%u, rxbuf=%d"
        #if MICROPY_HW_UART_TXBUF
        ", txbuf=%d"
        #endif
        #if MICROPY_HW_UART_RTSCTS
        ", rts=%q, cts=%q"
        #endif
        #if MICROPY_PY_MACHINE_UART_IRQ
        ", irq=%d"
        #endif
        ")",
        self->id, self->baudrate, self->bits, _parity_name[self->parity],
        self->stop + 1, pin_find_by_id(self->tx)->name, pin_find_by_id(self->rx)->name,
        self->timeout, self->timeout_char, rxbuf_len
        #if MICROPY_HW_UART_TXBUF
        , txbuf_len
        #endif
        #if MICROPY_HW_UART_RTSCTS
        , self->rts != 0xff ? pin_find_by_id(self->rts)->name : MP_QSTR_None
        , self->cts != 0xff ? pin_find_by_id(self->cts)->name : MP_QSTR_None
        #endif
        #if MICROPY_PY_MACHINE_UART_IRQ
        , self->mp_irq_trigger
        #endif
        );
}

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_tx, ARG_rx,
           ARG_timeout, ARG_timeout_char, ARG_rxbuf, ARG_txbuf, ARG_rts, ARG_cts };
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
        { MP_QSTR_txbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        #if MICROPY_HW_UART_RTSCTS
        { MP_QSTR_rts, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_cts, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
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
    self->flow_control = 0;
    #if MICROPY_HW_UART_RTSCTS
    // Set RTS/CTS pins if configured.
    if (args[ARG_rts].u_obj != mp_const_none) {
        self->rts = mp_hal_get_pin_obj(args[ARG_rts].u_obj);
        self->rts_pad_config = get_sercom_config(self->rts, self->id);
        self->flow_control = FLOW_CONTROL_RTS;
    }
    if (args[ARG_cts].u_obj != mp_const_none) {
        self->cts = mp_hal_get_pin_obj(args[ARG_cts].u_obj);
        self->cts_pad_config = get_sercom_config(self->cts, self->id);
        self->flow_control |= FLOW_CONTROL_CTS;
    }
    // rts only flow control is not allowed. Otherwise the state of the
    // cts pin is undefined.
    if (self->flow_control == FLOW_CONTROL_RTS) {
        mp_raise_ValueError(MP_ERROR_TEXT("cts missing for flow control"));
    }
    #endif

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

    #if MICROPY_HW_UART_TXBUF
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

    // Double the buffer lengths for 9 bit transfer
    if (self->bits > 8) {
        rxbuf_len *= 2;
        #if MICROPY_HW_UART_TXBUF
        txbuf_len *= 2;
        #endif
    }
    // Initialise the UART peripheral if any arguments given, or it was not initialised previously.
    if (n_args > 0 || kw_args->used > 0 || self->new) {
        self->new = false;

        // Check the rx/tx pin assignments
        if (self->tx == 0xff || self->rx == 0xff || (self->tx / 4) != (self->rx / 4)) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid or missing rx/tx"));
        }
        self->rx_pad_config = get_sercom_config(self->rx, self->id);
        self->tx_pad_config = get_sercom_config(self->tx, self->id);

        // Make sure timeout_char is at least as long as a whole character (13 bits to be safe).
        uint32_t min_timeout_char = 13000 / self->baudrate + 1;
        if (self->timeout_char < min_timeout_char) {
            self->timeout_char = min_timeout_char;
        }

        // Allocate the RX/TX buffers.
        ringbuf_alloc(&(self->read_buffer), rxbuf_len + 1);

        #if MICROPY_HW_UART_TXBUF
        ringbuf_alloc(&(self->write_buffer), txbuf_len + 1);
        #endif

        // Step 1: Configure the Pin mux.
        mp_hal_set_pin_mux(self->rx, self->rx_pad_config.alt_fct);
        mp_hal_set_pin_mux(self->tx, self->tx_pad_config.alt_fct);

        // Next: Set up the clocks
        enable_sercom_clock(self->id);

        // Configure the sercom module
        machine_sercom_configure(self);
    }
}

static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, MICROPY_HW_DEFAULT_UART_ID < 0 ? 1 : 0, MP_OBJ_FUN_ARGS_MAX, true);

    // Get UART bus.
    int uart_id = MICROPY_HW_DEFAULT_UART_ID;

    if (n_args > 0) {
        uart_id = mp_obj_get_int(args[0]);
        n_args--;
        args++;
    }
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
    #if defined(pin_TX) && defined(pin_RX)
    // Initialize with the default pins
    self->tx = mp_hal_get_pin_obj((mp_obj_t)pin_TX);
    self->rx = mp_hal_get_pin_obj((mp_obj_t)pin_RX);
    #else
    // Have to be defined
    self->tx = 0xff;
    self->rx = 0xff;
    #endif
    #if MICROPY_HW_UART_RTSCTS
    self->rts = 0xff;
    self->cts = 0xff;
    #endif
    #if MICROPY_PY_MACHINE_UART_IRQ
    self->mp_irq_obj = NULL;
    self->rxidle_state = RXIDLE_INACTIVE;
    #endif
    self->new = true;
    MP_STATE_PORT(sercom_table[uart_id]) = self;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_uart_init_helper(self, n_args, args, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    // Check if it is the active object.
    if (MP_STATE_PORT(sercom_table)[self->id] == self) {
        Sercom *uart = sercom_instance[self->id];
        // Disable interrupts and de-register the IRQ
        if (uart) {
            uart->USART.INTENCLR.reg = 0xff;
            sercom_register_irq(self->id, NULL);
            sercom_enable(uart, 0);
        }
    }
}

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    return ringbuf_avail(&self->read_buffer);
}

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    Sercom *uart = sercom_instance[self->id];

    return uart->USART.INTFLAG.bit.DRE
           #if MICROPY_HW_UART_TXBUF
           && ringbuf_avail(&self->write_buffer) == 0
           #endif
           && uart->USART.INTFLAG.bit.TXC;
}

static void mp_machine_uart_sendbreak(machine_uart_obj_t *self) {
    uint32_t break_time_us = 13 * 1000000 / self->baudrate;

    // Wait for the tx buffer to drain.
    #if MICROPY_HW_UART_TXBUF
    while (ringbuf_avail(&self->write_buffer) > 0) {
        MICROPY_EVENT_POLL_HOOK
    }
    #endif
    // Wait for the TX queue & register to clear
    // Since the flags are not safe, just wait sufficiently long.
    mp_hal_delay_us(2 * break_time_us);
    // Disable MUX
    PORT->Group[self->tx / 32].PINCFG[self->tx % 32].bit.PMUXEN = 0;
    // Set TX pin to low for break time
    mp_hal_pin_low(self->tx);
    mp_hal_delay_us(break_time_us);
    mp_hal_pin_high(self->tx);
    // Enable Mux again
    mp_hal_set_pin_mux(self->tx, self->tx_pad_config.alt_fct);
}

#if MICROPY_PY_MACHINE_UART_IRQ

// Configure the timer used for IRQ_RXIDLE
static void uart_irq_configure_timer(machine_uart_obj_t *self, mp_uint_t trigger) {
    self->rxidle_state = RXIDLE_INACTIVE;

    if (trigger & UART_IRQ_RXIDLE) {
        // The RXIDLE event is always a soft IRQ.
        self->mp_irq_obj->ishard = false;
        mp_int_t ms = 13000 / self->baudrate + 1;
        if (ms < RXIDLE_TIMER_MIN) {
            ms = RXIDLE_TIMER_MIN;
        }
        self->rxidle_ms = ms;
        self->rxidle_timer.context = self;
        soft_timer_static_init(
            &self->rxidle_timer.base,
            SOFT_TIMER_MODE_PERIODIC,
            ms,
            uart_soft_timer_callback
            );
        self->rxidle_state = RXIDLE_STANDBY;
    }
}

static mp_uint_t uart_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    uart_irq_configure_timer(self, new_trigger);
    self->mp_irq_trigger = new_trigger;
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

static const mp_irq_methods_t uart_irq_methods = {
    .trigger = uart_irq_trigger,
    .info = uart_irq_info,
};

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
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("trigger 0x%04x unsupported"), not_supported);
        }
        uart_irq_configure_timer(self, trigger);

        self->mp_irq_obj->handler = handler;
        self->mp_irq_obj->ishard = args[MP_IRQ_ARG_INIT_hard].u_bool;
        self->mp_irq_trigger = trigger;
    }

    return self->mp_irq_obj;
}

#endif

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    Sercom *uart = sercom_instance[self->id];
    uint64_t t = mp_hal_ticks_ms_64() + self->timeout;
    uint64_t timeout_char = self->timeout_char;
    uint8_t *dest = buf_in;

    // Check that size is even for 9 bit transfers.
    if ((self->bits >= 9) && (size & 1)) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }
    for (size_t i = 0; i < size;) {
        // Wait for the first/next character
        while (ringbuf_avail(&self->read_buffer) == 0) {
            if (mp_hal_ticks_ms_64() > t) {  // timed out
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
        i++;
        if (self->bits >= 9 && i < size) {
            *dest++ = ringbuf_get(&(self->read_buffer));
            i++;
        }
        t = mp_hal_ticks_ms_64() + timeout_char;
        // (Re-)Enable RXC interrupt
        if ((uart->USART.INTENSET.reg & SERCOM_USART_INTENSET_RXC) == 0) {
            uart->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC;
        }
    }
    return size;
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t i = 0;
    const uint8_t *src = buf_in;
    Sercom *uart = sercom_instance[self->id];
    uint64_t t = mp_hal_ticks_ms_64() + self->timeout;
    uint8_t bits = self->bits;
    // Check that size is even for 9 bit transfers.
    if ((bits >= 9) && (size & 1)) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }

    #if MICROPY_HW_UART_TXBUF

    #if MICROPY_PY_MACHINE_UART_IRQ
    // Prefill the FIFO to get rid of the initial IRQ_TXIDLE event
    // Do not care for 9 Bit transfer here since the UART is not yet started.
    while (i < size && ringbuf_free(&(self->write_buffer)) > 0) {
        ringbuf_put(&(self->write_buffer), *src++);
        i++;
    }
    uart->USART.INTENSET.reg = SERCOM_USART_INTENSET_DRE; // kick off the IRQ
    #endif
    while (i < size) {
        // Wait for the first/next character to be sent.
        while (ringbuf_free(&(self->write_buffer)) == 0) {
            if (mp_hal_ticks_ms_64() > t) {  // timed out
                if (i <= 0) {
                    *errcode = MP_EAGAIN;
                    return MP_STREAM_ERROR;
                } else {
                    return i;
                }
            }
            MICROPY_EVENT_POLL_HOOK
        }
        if (bits >= 9) {
            mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
            ringbuf_put(&(self->write_buffer), *src++);
            ringbuf_put(&(self->write_buffer), *src++);
            i += 2;
            MICROPY_END_ATOMIC_SECTION(atomic_state);
        } else {
            ringbuf_put(&(self->write_buffer), *src++);
            i += 1;
        }
        uart->USART.INTENSET.reg = SERCOM_USART_INTENSET_DRE; // kick off the IRQ
    }

    #else

    while (i < size) {
        while (!(uart->USART.INTFLAG.bit.DRE)) {
            if (mp_hal_ticks_ms_64() > t) {  // timed out
                if (i <= 0) {
                    *errcode = MP_EAGAIN;
                    return MP_STREAM_ERROR;
                } else {
                    return i;
                }
            }
            MICROPY_EVENT_POLL_HOOK
        }
        if (self->bits > 8 && i < (size - 1)) {
            uart->USART.DATA.bit.DATA = *(uint16_t *)src;
            i++;
            src += 2;
        } else {
            uart->USART.DATA.bit.DATA = *src++;
        }
        i++;
    }
    #endif
    return size;
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = self_in;
    mp_uint_t ret;
    Sercom *uart = sercom_instance[self->id];
    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && (uart->USART.INTFLAG.bit.RXC != 0 || ringbuf_avail(&self->read_buffer) > 0)) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && (uart->USART.INTFLAG.bit.DRE != 0
                                            #if MICROPY_HW_UART_TXBUF
                                            || ringbuf_avail(&self->write_buffer) > 0
                                            #endif
                                            )) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else if (request == MP_STREAM_FLUSH) {
        // The timeout is defined by the buffer size and the baudrate.
        // Take the worst case assumptions at 13 bit symbol size times 2.
        uint64_t timeout = mp_hal_ticks_ms_64() + (3
            #if MICROPY_HW_UART_TXBUF
            + self->write_buffer.size
            #endif
            ) * 13000 * 2 / self->baudrate;
        do {
            if (mp_machine_uart_txdone(self)) {
                return 0;
            }
            MICROPY_EVENT_POLL_HOOK
        } while (mp_hal_ticks_ms_64() < timeout);
        *errcode = MP_ETIMEDOUT;
        ret = MP_STREAM_ERROR;
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}
