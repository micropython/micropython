/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2023 Damien P. George
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
#include "py/mperrno.h"
#include "py/ringbuf.h"

#include "hardware/irq.h"
#include "hardware/uart.h"
#include "hardware/regs/uart.h"
#include "pico/mutex.h"

#define DEFAULT_UART_BAUDRATE (115200)
#define DEFAULT_UART_BITS (8)
#define DEFAULT_UART_STOP (1)

#ifdef MICROPY_HW_UART_NO_DEFAULT_PINS
// With no default I2C, need to require the pin args.
#define MICROPY_UART_PINS_ARG_OPTS MP_ARG_REQUIRED
#else
// Most boards do not require pin args.
#define MICROPY_UART_PINS_ARG_OPTS 0
#endif

// UART 0 default pins
#if !defined(MICROPY_HW_UART0_TX)
#define MICROPY_HW_UART0_TX (0)
#define MICROPY_HW_UART0_RX (1)
#define MICROPY_HW_UART0_CTS (2)
#define MICROPY_HW_UART0_RTS (3)
#endif

// UART 1 default pins
#if !defined(MICROPY_HW_UART1_TX)
#define MICROPY_HW_UART1_TX (4)
#define MICROPY_HW_UART1_RX (5)
#define MICROPY_HW_UART1_CTS (6)
#define MICROPY_HW_UART1_RTS (7)
#endif

#define DEFAULT_BUFFER_SIZE (256)
#define MIN_BUFFER_SIZE  (32)
#define MAX_BUFFER_SIZE  (32766)

#define IS_VALID_PERIPH(uart, pin)  (((((pin) + 4) & 8) >> 3) == (uart))
#define IS_VALID_TX(uart, pin)      (((pin) & 3) == 0 && IS_VALID_PERIPH(uart, pin))
#define IS_VALID_RX(uart, pin)      (((pin) & 3) == 1 && IS_VALID_PERIPH(uart, pin))
#define IS_VALID_CTS(uart, pin)     (((pin) & 3) == 2 && IS_VALID_PERIPH(uart, pin))
#define IS_VALID_RTS(uart, pin)     (((pin) & 3) == 3 && IS_VALID_PERIPH(uart, pin))

#define UART_INVERT_TX (1)
#define UART_INVERT_RX (2)
#define UART_INVERT_MASK (UART_INVERT_TX | UART_INVERT_RX)

#define UART_HWCONTROL_CTS  (1)
#define UART_HWCONTROL_RTS  (2)

static mutex_t write_mutex_0;
static mutex_t write_mutex_1;
static mutex_t read_mutex_0;
static mutex_t read_mutex_1;

auto_init_mutex(write_mutex_0);
auto_init_mutex(write_mutex_1);
auto_init_mutex(read_mutex_0);
auto_init_mutex(read_mutex_1);

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    uart_inst_t *const uart;
    uint8_t uart_id;
    uint32_t baudrate;
    uint8_t bits;
    uart_parity_t parity;
    uint8_t stop;
    uint8_t tx;
    uint8_t rx;
    uint8_t cts;
    uint8_t rts;
    uint16_t timeout;       // timeout waiting for first char (in ms)
    uint16_t timeout_char;  // timeout waiting between chars (in ms)
    uint8_t invert;
    uint8_t flow;
    ringbuf_t read_buffer;
    mutex_t *read_mutex;
    ringbuf_t write_buffer;
    mutex_t *write_mutex;
} machine_uart_obj_t;

static machine_uart_obj_t machine_uart_obj[] = {
    {{&machine_uart_type}, uart0, 0, 0, DEFAULT_UART_BITS, UART_PARITY_NONE, DEFAULT_UART_STOP,
     MICROPY_HW_UART0_TX, MICROPY_HW_UART0_RX, MICROPY_HW_UART0_CTS, MICROPY_HW_UART0_RTS,
     0, 0, 0, 0, {NULL, 1, 0, 0}, &read_mutex_0, {NULL, 1, 0, 0}, &write_mutex_0},
    {{&machine_uart_type}, uart1, 1, 0, DEFAULT_UART_BITS, UART_PARITY_NONE, DEFAULT_UART_STOP,
     MICROPY_HW_UART1_TX, MICROPY_HW_UART1_RX, MICROPY_HW_UART1_CTS, MICROPY_HW_UART1_RTS,
     0, 0, 0, 0, {NULL, 1, 0, 0}, &read_mutex_1, {NULL, 1, 0, 0}, &write_mutex_1},
};

static const char *_parity_name[] = {"None", "0", "1"};
static const char *_invert_name[] = {"None", "INV_TX", "INV_RX", "INV_TX|INV_RX"};

/******************************************************************************/
// IRQ and buffer handling

static inline bool write_mutex_try_lock(machine_uart_obj_t *u) {
    return mutex_enter_timeout_ms(u->write_mutex, 0);
}

static inline void write_mutex_unlock(machine_uart_obj_t *u) {
    mutex_exit(u->write_mutex);
}

static inline bool read_mutex_try_lock(machine_uart_obj_t *u) {
    return mutex_enter_timeout_ms(u->read_mutex, 0);
}

static inline void read_mutex_unlock(machine_uart_obj_t *u) {
    mutex_exit(u->read_mutex);
}

// take all bytes from the fifo and store them in the buffer
static void uart_drain_rx_fifo(machine_uart_obj_t *self) {
    if (read_mutex_try_lock(self)) {
        while (uart_is_readable(self->uart) && ringbuf_free(&self->read_buffer) > 0) {
            // Get a byte from uart and put into the buffer. Every entry from
            // the FIFO is accompanied by 4 error bits, that may be used for
            // error handling.
            uint16_t c = uart_get_hw(self->uart)->dr;
            if (c & UART_UARTDR_OE_BITS) {
                // Overrun Error: We missed at least one byte. Not much we can do here.
            }
            if (c & UART_UARTDR_BE_BITS) {
                // Break Error: RX was held low for longer than one character
                // (11 bits). We did *not* read the zero byte that we seemed to
                // read from dr.
                continue;
            }
            if (c & UART_UARTDR_PE_BITS) {
                // Parity Error: The byte we read is invalid.
            }
            if (c & UART_UARTDR_FE_BITS) {
                // Framing Error: We did not receive a valid stop bit.
            }

            ringbuf_put(&(self->read_buffer), c);
        }
        read_mutex_unlock(self);
    }
}

// take bytes from the buffer and put them into the UART FIFO
// Re-entrancy: quit if an instance already running
static void uart_fill_tx_fifo(machine_uart_obj_t *self) {
    if (write_mutex_try_lock(self)) {
        while (uart_is_writable(self->uart) && ringbuf_avail(&self->write_buffer) > 0) {
            // get a byte from the buffer and put it into the uart
            uart_get_hw(self->uart)->dr = ringbuf_get(&(self->write_buffer));
        }
        write_mutex_unlock(self);
    }
}

static inline void uart_service_interrupt(machine_uart_obj_t *self) {
    if (uart_get_hw(self->uart)->mis & (UART_UARTMIS_RXMIS_BITS | UART_UARTMIS_RTMIS_BITS)) { // rx interrupt?
        // clear all interrupt bits but tx
        uart_get_hw(self->uart)->icr = UART_UARTICR_BITS & (~UART_UARTICR_TXIC_BITS);
        uart_drain_rx_fifo(self);
    }
    if (uart_get_hw(self->uart)->mis & UART_UARTMIS_TXMIS_BITS) { // tx interrupt?
        // clear all interrupt bits but rx
        uart_get_hw(self->uart)->icr = UART_UARTICR_BITS & ~(UART_UARTICR_RXIC_BITS | UART_UARTICR_RTIC_BITS);
        uart_fill_tx_fifo(self);
    }
}

static void uart0_irq_handler(void) {
    uart_service_interrupt(&machine_uart_obj[0]);
}

static void uart1_irq_handler(void) {
    uart_service_interrupt(&machine_uart_obj[1]);
}

/******************************************************************************/
// MicroPython bindings for UART

#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_INV_TX), MP_ROM_INT(UART_INVERT_TX) }, \
    { MP_ROM_QSTR(MP_QSTR_INV_RX), MP_ROM_INT(UART_INVERT_RX) }, \
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(UART_HWCONTROL_CTS) }, \
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(UART_HWCONTROL_RTS) }, \

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=%s, stop=%u, tx=%d, rx=%d, "
        "txbuf=%d, rxbuf=%d, timeout=%u, timeout_char=%u, invert=%s)",
        self->uart_id, self->baudrate, self->bits, _parity_name[self->parity],
        self->stop, self->tx, self->rx, self->write_buffer.size - 1, self->read_buffer.size - 1,
        self->timeout, self->timeout_char, _invert_name[self->invert]);
}

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_tx, ARG_rx, ARG_cts, ARG_rts,
           ARG_timeout, ARG_timeout_char, ARG_invert, ARG_flow, ARG_txbuf, ARG_rxbuf};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_INT(-1)} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_tx, MICROPY_UART_PINS_ARG_OPTS | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_rx, MICROPY_UART_PINS_ARG_OPTS | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_cts, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_rts, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_invert, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_txbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // Parse args.
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
            self->parity = UART_PARITY_NONE;
        } else if (mp_obj_get_int(args[ARG_parity].u_obj) & 1) {
            self->parity = UART_PARITY_ODD;
        } else {
            self->parity = UART_PARITY_EVEN;
        }
    }

    // Set stop bits if configured.
    if (args[ARG_stop].u_int > 0) {
        self->stop = args[ARG_stop].u_int;
    }

    // Set TX/RX pins if configured.
    if (args[ARG_tx].u_obj != mp_const_none) {
        int tx = mp_hal_get_pin_obj(args[ARG_tx].u_obj);
        if (!IS_VALID_TX(self->uart_id, tx)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad TX pin"));
        }
        self->tx = tx;
    }
    if (args[ARG_rx].u_obj != mp_const_none) {
        int rx = mp_hal_get_pin_obj(args[ARG_rx].u_obj);
        if (!IS_VALID_RX(self->uart_id, rx)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad RX pin"));
        }
        self->rx = rx;
    }

    // Set CTS/RTS pins if configured.
    if (args[ARG_cts].u_obj != mp_const_none) {
        int cts = mp_hal_get_pin_obj(args[ARG_cts].u_obj);
        if (!IS_VALID_CTS(self->uart_id, cts)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad CTS pin"));
        }
        self->cts = cts;
    }
    if (args[ARG_rts].u_obj != mp_const_none) {
        int rts = mp_hal_get_pin_obj(args[ARG_rts].u_obj);
        if (!IS_VALID_RTS(self->uart_id, rts)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad RTS pin"));
        }
        self->rts = rts;
    }

    // Set timeout if configured.
    if (args[ARG_timeout].u_int >= 0) {
        self->timeout = args[ARG_timeout].u_int;
    }

    // Set timeout_char if configured.
    if (args[ARG_timeout_char].u_int >= 0) {
        self->timeout_char = args[ARG_timeout_char].u_int;
    }

    // Set line inversion if configured.
    if (args[ARG_invert].u_int >= 0) {
        if (args[ARG_invert].u_int & ~UART_INVERT_MASK) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad inversion mask"));
        }
        self->invert = args[ARG_invert].u_int;
    }

    // Set hardware flow control if configured.
    if (args[ARG_flow].u_int >= 0) {
        if (args[ARG_flow].u_int & ~(UART_HWCONTROL_CTS | UART_HWCONTROL_RTS)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad hardware flow control mask"));
        }
        self->flow = args[ARG_flow].u_int;
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

    // Initialise the UART peripheral if any arguments given, or it was not initialised previously.
    if (n_args > 0 || kw_args->used > 0 || self->baudrate == 0) {
        if (self->baudrate == 0) {
            self->baudrate = DEFAULT_UART_BAUDRATE;
        }

        // Make sure timeout_char is at least as long as a whole character (13 bits to be safe).
        uint32_t min_timeout_char = 13000 / self->baudrate + 1;
        if (self->timeout_char < min_timeout_char) {
            self->timeout_char = min_timeout_char;
        }

        uart_init(self->uart, self->baudrate);
        uart_set_format(self->uart, self->bits, self->stop, self->parity);
        __dsb(); // make sure UARTLCR_H register is written to
        uart_set_fifo_enabled(self->uart, true);
        __dsb(); // make sure UARTLCR_H register is written to
        gpio_set_function(self->tx, GPIO_FUNC_UART);
        gpio_set_function(self->rx, GPIO_FUNC_UART);
        if (self->invert & UART_INVERT_RX) {
            gpio_set_inover(self->rx, GPIO_OVERRIDE_INVERT);
        }
        if (self->invert & UART_INVERT_TX) {
            gpio_set_outover(self->tx, GPIO_OVERRIDE_INVERT);
        }

        // Set hardware flow control if configured.
        if (self->flow & UART_HWCONTROL_CTS) {
            gpio_set_function(self->cts, GPIO_FUNC_UART);
        }
        if (self->flow & UART_HWCONTROL_RTS) {
            gpio_set_function(self->rts, GPIO_FUNC_UART);
        }
        uart_set_hw_flow(self->uart, self->flow & UART_HWCONTROL_CTS, self->flow & UART_HWCONTROL_RTS);

        // Allocate the RX/TX buffers.
        ringbuf_alloc(&(self->read_buffer), rxbuf_len + 1);
        MP_STATE_PORT(rp2_uart_rx_buffer[self->uart_id]) = self->read_buffer.buf;

        ringbuf_alloc(&(self->write_buffer), txbuf_len + 1);
        MP_STATE_PORT(rp2_uart_tx_buffer[self->uart_id]) = self->write_buffer.buf;

        // Set the irq handler.
        if (self->uart_id == 0) {
            irq_set_exclusive_handler(UART0_IRQ, uart0_irq_handler);
            irq_set_enabled(UART0_IRQ, true);
        } else {
            irq_set_exclusive_handler(UART1_IRQ, uart1_irq_handler);
            irq_set_enabled(UART1_IRQ, true);
        }

        // Enable the uart irq; this macro sets the rx irq level to 4.
        uart_set_irq_enables(self->uart, true, true);
    }
}

static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Get UART bus.
    int uart_id = mp_obj_get_int(args[0]);
    if (uart_id < 0 || uart_id >= MP_ARRAY_SIZE(machine_uart_obj)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) doesn't exist"), uart_id);
    }

    // Get static peripheral object.
    machine_uart_obj_t *self = (machine_uart_obj_t *)&machine_uart_obj[uart_id];

    // Initialise the UART peripheral.
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    uart_deinit(self->uart);
    if (self->uart_id == 0) {
        irq_set_enabled(UART0_IRQ, false);
    } else {
        irq_set_enabled(UART1_IRQ, false);
    }
    self->baudrate = 0;
    MP_STATE_PORT(rp2_uart_rx_buffer[self->uart_id]) = NULL;
    MP_STATE_PORT(rp2_uart_tx_buffer[self->uart_id]) = NULL;
}

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    // get all bytes from the fifo first
    uart_drain_rx_fifo(self);
    return ringbuf_avail(&self->read_buffer);
}

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    return ringbuf_avail(&self->write_buffer) == 0
           && (uart_get_hw(self->uart)->fr & UART_UARTFR_TXFE_BITS);
}

static void mp_machine_uart_sendbreak(machine_uart_obj_t *self) {
    uart_set_break(self->uart, true);
    mp_hal_delay_us(13000000 / self->baudrate + 1);
    uart_set_break(self->uart, false);
}

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t start = mp_hal_ticks_ms();
    mp_uint_t timeout = self->timeout;
    uint8_t *dest = buf_in;

    for (size_t i = 0; i < size; i++) {
        // Wait for the first/next character
        while (ringbuf_avail(&self->read_buffer) == 0) {
            if (uart_is_readable(self->uart)) {
                // Force a few incoming bytes to the buffer
                uart_drain_rx_fifo(self);
                break;
            }
            mp_uint_t elapsed = mp_hal_ticks_ms() - start;
            if (elapsed > timeout) {  // timed out
                if (i <= 0) {
                    *errcode = MP_EAGAIN;
                    return MP_STREAM_ERROR;
                } else {
                    return i;
                }
            }
            mp_event_handle_nowait();
        }
        *dest++ = ringbuf_get(&(self->read_buffer));
        start = mp_hal_ticks_ms(); // Inter-character timeout
        timeout = self->timeout_char;
    }
    return size;
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t start = mp_hal_ticks_ms();
    mp_uint_t timeout = self->timeout;
    const uint8_t *src = buf_in;
    size_t i = 0;

    // Put as many bytes as possible into the transmit buffer.
    while (i < size && ringbuf_free(&(self->write_buffer)) > 0) {
        ringbuf_put(&(self->write_buffer), *src++);
        ++i;
    }

    // Kickstart the UART transmit.
    uart_fill_tx_fifo(self);

    // Send the next characters while busy waiting.
    while (i < size) {
        // Wait for the first/next character to be sent.
        while (ringbuf_free(&(self->write_buffer)) == 0) {
            mp_uint_t elapsed = mp_hal_ticks_ms() - start;
            if (elapsed > timeout) {  // timed out
                if (i <= 0) {
                    *errcode = MP_EAGAIN;
                    return MP_STREAM_ERROR;
                } else {
                    return i;
                }
            }
            mp_event_wait_ms(timeout - elapsed);
        }
        ringbuf_put(&(self->write_buffer), *src++);
        ++i;
        start = mp_hal_ticks_ms(); // Inter-character timeout
        timeout = self->timeout_char;
        uart_fill_tx_fifo(self);
    }

    // Just in case the fifo was drained during refill of the ringbuf.
    return size;
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = self_in;
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && (uart_is_readable(self->uart) || ringbuf_avail(&self->read_buffer) > 0)) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && ringbuf_free(&self->write_buffer) > 0) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else if (request == MP_STREAM_FLUSH) {
        // The timeout is estimated using the buffer size and the baudrate.
        // Take the worst case assumptions at 13 bit symbol size times 2.
        uint64_t timeout = time_us_64() +
            (uint64_t)(33 + self->write_buffer.size) * 13000000ll * 2 / self->baudrate;
        while (1) {
            if (mp_machine_uart_txdone(self)) {
                return 0;
            }
            uint64_t now = time_us_64();
            if (now >= timeout) {
                break;
            }
            mp_event_handle_nowait();
        }
        *errcode = MP_ETIMEDOUT;
        ret = MP_STREAM_ERROR;
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

MP_REGISTER_ROOT_POINTER(void *rp2_uart_rx_buffer[2]);
MP_REGISTER_ROOT_POINTER(void *rp2_uart_tx_buffer[2]);
