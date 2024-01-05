/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Glenn Ruben Bakke
 * Copyright (c) 2018 Ayke van Laethem
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

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/ringbuf.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "shared/runtime/interrupt_char.h"

#include "uart.h"
#include "nrf.h"

#if NRFX_UART_ENABLED
#include "nrfx_uart.h"
#else
#include "nrfx_uarte.h"
#endif

typedef struct _machine_uart_buf_t {
    uint8_t tx_buf[1];
    uint8_t rx_buf[1];
    uint8_t rx_ringbuf_array[64];
    volatile ringbuf_t rx_ringbuf;
} machine_uart_buf_t;

#if NRFX_UARTE_ENABLED

#define nrfx_uart_t               nrfx_uarte_t
#define nrfx_uart_config_t        nrfx_uarte_config_t

#define nrfx_uart_rx              nrfx_uarte_rx
#define nrfx_uart_tx              nrfx_uarte_tx
#define nrfx_uart_tx_in_progress  nrfx_uarte_tx_in_progress
#define nrfx_uart_init            nrfx_uarte_init
#define nrfx_uart_event_t         nrfx_uarte_event_t
#define NRFX_UART_INSTANCE        NRFX_UARTE_INSTANCE

#define NRF_UART_HWFC_ENABLED     NRF_UARTE_HWFC_ENABLED
#define NRF_UART_HWFC_DISABLED    NRF_UARTE_HWFC_DISABLED
#define NRF_UART_PARITY_EXCLUDED  NRF_UARTE_PARITY_EXCLUDED
#define NRFX_UART_EVT_RX_DONE     NRFX_UARTE_EVT_RX_DONE
#define NRFX_UART_EVT_ERROR       NRFX_UARTE_EVT_ERROR

#define NRF_UART_BAUDRATE_1200    NRF_UARTE_BAUDRATE_1200
#define NRF_UART_BAUDRATE_2400    NRF_UARTE_BAUDRATE_2400
#define NRF_UART_BAUDRATE_4800    NRF_UARTE_BAUDRATE_4800
#define NRF_UART_BAUDRATE_9600    NRF_UARTE_BAUDRATE_9600
#define NRF_UART_BAUDRATE_14400   NRF_UARTE_BAUDRATE_14400
#define NRF_UART_BAUDRATE_19200   NRF_UARTE_BAUDRATE_19200
#define NRF_UART_BAUDRATE_28800   NRF_UARTE_BAUDRATE_28800
#define NRF_UART_BAUDRATE_38400   NRF_UARTE_BAUDRATE_38400
#define NRF_UART_BAUDRATE_57600   NRF_UARTE_BAUDRATE_57600
#define NRF_UART_BAUDRATE_76800   NRF_UARTE_BAUDRATE_76800
#define NRF_UART_BAUDRATE_115200  NRF_UARTE_BAUDRATE_115200
#define NRF_UART_BAUDRATE_230400  NRF_UARTE_BAUDRATE_230400
#define NRF_UART_BAUDRATE_250000  NRF_UARTE_BAUDRATE_250000
#define NRF_UART_BAUDRATE_1000000 NRF_UARTE_BAUDRATE_1000000

#endif

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    const nrfx_uart_t *p_uart;       // Driver instance
    machine_uart_buf_t buf;
    uint16_t timeout;       // timeout waiting for first char (in ms)
    uint16_t timeout_char;  // timeout waiting between chars (in ms)
} machine_uart_obj_t;

static const nrfx_uart_t instance0 = NRFX_UART_INSTANCE(0);

static machine_uart_obj_t machine_uart_obj[] = {
    {{&machine_uart_type}, .p_uart = &instance0}
};

void uart_init0(void) {
}

static int uart_find(mp_obj_t id) {
    // given an integer id
    int uart_id = mp_obj_get_int(id);
    if (uart_id >= 0 && uart_id < MP_ARRAY_SIZE(machine_uart_obj)) {
        return uart_id;
    }
    mp_raise_ValueError(MP_ERROR_TEXT("UART doesn't exist"));
}

static void uart_event_handler(nrfx_uart_event_t const *p_event, void *p_context) {
    machine_uart_obj_t *self = p_context;
    if (p_event->type == NRFX_UART_EVT_RX_DONE) {
        nrfx_uart_rx(self->p_uart, &self->buf.rx_buf[0], 1);
        int chr = self->buf.rx_buf[0];
        #if !MICROPY_PY_BLE_NUS && MICROPY_KBD_EXCEPTION
        if (chr == mp_interrupt_char) {
            self->buf.rx_ringbuf.iget = 0;
            self->buf.rx_ringbuf.iput = 0;
            mp_sched_keyboard_interrupt();
        } else
        #endif
        {
            ringbuf_put((ringbuf_t *)&self->buf.rx_ringbuf, chr);
        }
    } else if (p_event->type == NRFX_UART_EVT_ERROR) {
        // Perform a read to unlock UART in case of an error
        nrfx_uart_rx(self->p_uart, &self->buf.rx_buf[0], 1);
    }
}

bool uart_rx_any(machine_uart_obj_t *self) {
    return self->buf.rx_ringbuf.iput != self->buf.rx_ringbuf.iget;
}

int uart_rx_char(machine_uart_obj_t *self) {
    return ringbuf_get((ringbuf_t *)&self->buf.rx_ringbuf);
}

static nrfx_err_t uart_tx_char(machine_uart_obj_t *self, int c) {
    while (nrfx_uart_tx_in_progress(self->p_uart)) {
        ;
    }
    self->buf.tx_buf[0] = c;
    return nrfx_uart_tx(self->p_uart, &self->buf.tx_buf[0], 1);
}


void uart_tx_strn(machine_uart_obj_t *uart_obj, const char *str, uint len) {
    for (const char *top = str + len; str < top; str++) {
        uart_tx_char(uart_obj, *str);
    }
}

void uart_tx_strn_cooked(machine_uart_obj_t *uart_obj, const char *str, uint len) {
    for (const char *top = str + len; str < top; str++) {
        if (*str == '\n') {
            uart_tx_char(uart_obj, '\r');
        }
        uart_tx_char(uart_obj, *str);
    }
}

/******************************************************************************/
/* MicroPython bindings                                                      */

// The UART class doesn't have any constants for this port.
#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_printf(print, "UART(0)");
}

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // Parse args (none supported at this stage).
    mp_arg_parse_all(n_args, pos_args, kw_args, 0, NULL, NULL);
}

// UART(id, baudrate)
//
// Initialise the UART bus with the given parameters:
//   - `id`is bus id.
//   - `baudrate` is the clock rate.
static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_baudrate, ARG_timeout, ARG_timeout_char };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_baudrate, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 9600} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get static peripheral object
    int uart_id = uart_find(args[ARG_id].u_obj);
    machine_uart_obj_t *self = &machine_uart_obj[uart_id];

    nrfx_uart_config_t config;

    // flow control
    #if MICROPY_HW_UART1_HWFC
    config.hal_cfg.hwfc = NRF_UART_HWFC_ENABLED;
    #else
    config.hal_cfg.hwfc = NRF_UART_HWFC_DISABLED;
    #endif

    config.hal_cfg.parity = NRF_UART_PARITY_EXCLUDED;

    // Higher priority than pin interrupts, otherwise printing exceptions from
    // interrupt handlers gets stuck.
    config.interrupt_priority = NRFX_GPIOTE_DEFAULT_CONFIG_IRQ_PRIORITY - 1;

    // These baudrates are not supported, it seems.
    if (args[ARG_baudrate].u_int < 1200 || args[ARG_baudrate].u_int > 1000000) {
        mp_raise_ValueError(MP_ERROR_TEXT("UART baudrate not supported"));
    }

    // Magic: calculate 'baudrate' register from the input number.
    // Every value listed in the datasheet will be converted to the
    // correct register value, except for 192600. I believe the value
    // listed in the nrf52 datasheet (0x0EBED000) is incorrectly rounded
    // and should be 0x0EBEE000, as the nrf51 datasheet lists the
    // nonrounded value 0x0EBEDFA4.
    // Some background:
    // https://devzone.nordicsemi.com/f/nordic-q-a/391/uart-baudrate-register-values/2046#2046
    config.baudrate = args[ARG_baudrate].u_int / 400 * (uint32_t)(400ULL * (uint64_t)UINT32_MAX / 16000000ULL);
    config.baudrate = (config.baudrate + 0x800) & 0xffffff000; // rounding

    config.pseltxd = MICROPY_HW_UART1_TX;
    config.pselrxd = MICROPY_HW_UART1_RX;

    #if MICROPY_HW_UART1_HWFC
    config.pselrts = MICROPY_HW_UART1_RTS;
    config.pselcts = MICROPY_HW_UART1_CTS;
    #endif
    self->timeout = args[ARG_timeout].u_int;
    self->timeout_char = args[ARG_timeout_char].u_int;

    // Set context to this instance of UART
    config.p_context = (void *)self;

    // Initialise ring buffer
    self->buf.rx_ringbuf.buf = self->buf.rx_ringbuf_array;
    self->buf.rx_ringbuf.size = sizeof(self->buf.rx_ringbuf_array);
    self->buf.rx_ringbuf.iget = 0;
    self->buf.rx_ringbuf.iput = 0;

    // Enable event callback and start asynchronous receive
    nrfx_uart_init(self->p_uart, &config, uart_event_handler);
    nrfx_uart_rx(self->p_uart, &self->buf.rx_buf[0], 1);

    #if NRFX_UART_ENABLED
    nrfx_uart_rx_enable(self->p_uart);
    #endif

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    (void)self;
}

// Write a single character on the bus.  `data` is an integer to write.
static void mp_machine_uart_writechar(machine_uart_obj_t *self, uint16_t data) {
    nrfx_err_t err = uart_tx_char(self, data);
    if (err != NRFX_SUCCESS) {
        mp_hal_raise(err);
    }
}

// Receive a single character on the bus.
// Return value: The character read, as an integer.  Returns -1 on timeout.
static mp_int_t mp_machine_uart_readchar(machine_uart_obj_t *self) {
    return uart_rx_char(self);
}

// uart.any()
static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    return ringbuf_avail((ringbuf_t *)&self->buf.rx_ringbuf);
}

// uart.txdone()
static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    return !nrfx_uart_tx_in_progress(self->p_uart);
}

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = self_in;
    byte *buf = buf_in;
    uint32_t t = self->timeout + mp_hal_ticks_ms();

    // read the data
    for (size_t i = 0; i < size; i++) {
        while (!uart_rx_any(self)) {
            if ((int32_t)(mp_hal_ticks_ms() - t) >= 0) { // timed out
                if (i == 0) {
                    *errcode = MP_EAGAIN;
                    return MP_STREAM_ERROR;
                } else {
                    return i;
                }
            }
            MICROPY_EVENT_POLL_HOOK;
        }
        buf[i] = uart_rx_char(self);
        t = self->timeout_char + mp_hal_ticks_ms();
    }

    return size;
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = self_in;

    nrfx_err_t err = nrfx_uart_tx(self->p_uart, buf_in, size);
    if (err == NRFX_SUCCESS) {
        while (nrfx_uart_tx_in_progress(self->p_uart)) {
            MICROPY_EVENT_POLL_HOOK;
        }
        // return number of bytes written
        return size;
    } else {
        *errcode = mp_hal_status_to_errno_table[err];
        return MP_STREAM_ERROR;
    }
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = self_in;
    (void)self;
    mp_uint_t ret = 0;

    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        if ((flags & MP_STREAM_POLL_RD) && uart_rx_any(self) != 0) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && !nrfx_uart_tx_in_progress(self->p_uart)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else if (request == MP_STREAM_FLUSH) {
        while (nrfx_uart_tx_in_progress(self->p_uart)) {
            MICROPY_EVENT_POLL_HOOK;
        }
        return 0;
    }
    return MP_STREAM_ERROR;
}
