// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

// machine.UART for the UEFI port. This file is *included* into extmod/machine_uart.c via
// MICROPY_PY_MACHINE_UART_INCLUDEFILE. Each id is a slot of the serial driver layer
// (uefi_serial.c): an index, or text matched against the port's device path. Two
// objects for the same id share the port (and its RX ring), as does the REPL when the
// REPL is on that port. See docs/serial-uart-design.md.
//
// A port that is unplugged leaves its objects dead (OSError ENODEV) even if it comes
// back: the program must call init() (or make a new UART) to take it again. Each object
// remembers the slot's open generation to tell a re-opened port from its own.

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/stream.h"
#include "uefi_port.h"
#include "uefi_serial.h"

#define UART_RTS (1)
#define UART_CTS (2)

#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(UART_RTS) }, \
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(UART_CTS) }, \

struct _machine_uart_obj_t {
    mp_obj_base_t base;
    uint8_t slot;
    uint32_t gen;           // slot generation this object configured; 0 = not open
    mp_uint_t timeout;      // ms: wait for the first character, and for write progress
    mp_uint_t timeout_char; // ms: wait between characters
};

static mp_uefi_serial_t *uart_port(machine_uart_obj_t *self) {
    return &mp_uefi_serial_slots[self->slot];
}

// Is this object's port still the one it configured, owned, and plugged in?
static bool uart_alive(machine_uart_obj_t *self) {
    mp_uefi_serial_t *s = uart_port(self);
    return self->gen != 0 && s->gen == self->gen && mp_uefi_serial_usable(s);
}

static mp_uefi_serial_t *uart_checked(machine_uart_obj_t *self) {
    if (!uart_alive(self)) {
        mp_raise_OSError(MP_ENODEV);
    }
    return uart_port(self);
}

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uefi_serial_t *s = uart_port(self);
    if (!uart_alive(self)) {
        mp_printf(print, "UART(%u, closed)", self->slot);
        return;
    }
    mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=%s, stop=%u, flow=%u, "
        "timeout=%u, timeout_char=%u, rxbuf=%u)",
        self->slot, (unsigned)s->baud, s->bits,
        s->parity == EFI_SERIAL_EVEN_PARITY ? "0" : s->parity == EFI_SERIAL_ODD_PARITY ? "1" : "None",
        s->stop == EFI_SERIAL_TWO_STOP_BITS ? 2 : 1, s->rtscts ? UART_RTS | UART_CTS : 0,
        (unsigned)self->timeout, (unsigned)self->timeout_char, s->rx.size);
}

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_flow, ARG_timeout, ARG_timeout_char, ARG_rxbuf };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Validate everything before touching the port. -1 / -2 below mean "not given".
    mp_int_t baud = args[ARG_baudrate].u_int;
    if (baud == 0 || baud < -1) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid baudrate"));
    }
    mp_int_t bits = args[ARG_bits].u_int;
    if (bits != -1 && (bits < 5 || bits > 8)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid bits"));
    }
    int parity = -1;
    if (args[ARG_parity].u_obj == mp_const_none) {
        parity = EFI_SERIAL_NO_PARITY;
    } else if (args[ARG_parity].u_obj != MP_OBJ_NULL) {
        mp_int_t p = mp_obj_get_int(args[ARG_parity].u_obj);
        if (p != 0 && p != 1) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid parity"));
        }
        parity = p == 0 ? EFI_SERIAL_EVEN_PARITY : EFI_SERIAL_ODD_PARITY;
    }
    int stop = -1;
    if (args[ARG_stop].u_int != -1) {
        if (args[ARG_stop].u_int != 1 && args[ARG_stop].u_int != 2) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid stop"));
        }
        stop = args[ARG_stop].u_int == 1 ? EFI_SERIAL_ONE_STOP_BIT : EFI_SERIAL_TWO_STOP_BITS;
    }
    // flow: None = RTS/CTS if the driver has it; 0 = off; RTS|CTS = on. SerialIo has a
    // single flow-control bit, so RTS or CTS alone can't be honoured.
    int flow = -2;
    if (args[ARG_flow].u_obj == mp_const_none) {
        flow = -1;
    } else if (args[ARG_flow].u_obj != MP_OBJ_NULL) {
        mp_int_t f = mp_obj_get_int(args[ARG_flow].u_obj);
        if (f != 0 && f != (UART_RTS | UART_CTS)) {
            mp_raise_ValueError(MP_ERROR_TEXT("flow must be 0, None or UART.RTS | UART.CTS"));
        }
        flow = f != 0;
    }
    if (args[ARG_rxbuf].u_int == 0 || args[ARG_rxbuf].u_int < -1) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid rxbuf"));
    }

    mp_uefi_serial_t *s = uart_port(self);
    if (!s->present || mp_uefi_serial_rescan_pending()) {
        mp_uefi_serial_scan();
    }
    // Already open (by the REPL, another object, or this one)? Then join it and only
    // change the settings that were given.
    bool was_open = s->owned && mp_uefi_serial_usable(s);
    size_t rxbuf = args[ARG_rxbuf].u_int > 0 ? (size_t)args[ARG_rxbuf].u_int
        : s->rx.buf != NULL ? s->rx.size : MP_UEFI_SERIAL_RXBUF_DEFAULT;
    EFI_STATUS st = mp_uefi_serial_open(s, rxbuf);
    if (EFI_ERROR(st)) {
        mp_raise_OSError(st == EFI_NOT_FOUND ? MP_ENODEV : MP_ENOMEM);
    }
    self->gen = s->gen;
    if (!was_open || baud > 0 || bits > 0 || parity >= 0 || stop >= 0 || flow != -2) {
        EFI_SERIAL_IO_MODE *m = s->io->Mode;
        uint32_t b = baud > 0 ? (uint32_t)baud : was_open ? s->baud
            : (m != NULL && m->BaudRate != 0) ? (uint32_t)m->BaudRate : 115200;
        st = mp_uefi_serial_configure(s, b,
            bits > 0 ? bits : was_open ? s->bits : 8,
            parity >= 0 ? parity : was_open ? s->parity : EFI_SERIAL_NO_PARITY,
            stop >= 0 ? stop : was_open ? s->stop : EFI_SERIAL_ONE_STOP_BIT,
            flow != -2 ? flow : was_open ? s->rtscts : -1);
        if (EFI_ERROR(st)) {
            if (!was_open) {
                mp_uefi_serial_close(s);
                self->gen = 0;
            }
            mp_raise_OSError(st == EFI_NOT_FOUND ? MP_ENODEV : MP_EINVAL);
        }
    }
    if (args[ARG_timeout].u_int >= 0) {
        self->timeout = args[ARG_timeout].u_int;
    }
    if (args[ARG_timeout_char].u_int >= 0) {
        self->timeout_char = args[ARG_timeout_char].u_int;
    }
    // timeout_char is at least one character time, rounded up to whole ms.
    mp_uint_t min_char = (mp_uefi_serial_char_bits(s) * 1000 + s->baud - 1) / s->baud;
    if (self->timeout_char < min_char) {
        self->timeout_char = min_char;
    }
}

static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    mp_uefi_serial_scan();
    int slot;
    if (mp_obj_is_str(args[0])) {
        slot = mp_uefi_serial_find_text(mp_obj_str_get_str(args[0]));
        if (slot == -2) {
            mp_raise_ValueError(MP_ERROR_TEXT("firmware can't name UART device paths"));
        }
    } else {
        slot = mp_uefi_serial_find_index(mp_obj_get_int(args[0]));
    }
    if (slot < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("UART doesn't exist"));
    }
    machine_uart_obj_t *self = mp_obj_malloc(machine_uart_obj_t, type);
    self->slot = slot;
    self->gen = 0;
    self->timeout = 0;
    self->timeout_char = 0;
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);
    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    mp_uefi_serial_t *s = uart_port(self);
    // The REPL's port stays open for the REPL; other objects sharing a port lose it.
    if (self->gen == s->gen && !s->repl) {
        mp_uefi_serial_close(s);
    }
    self->gen = 0;
}

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    return mp_uefi_serial_any(uart_checked(self));
}

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    return mp_uefi_serial_txdone(uart_checked(self));
}

// rp2 semantics: wait up to `timeout` for the first character, then up to
// `timeout_char` between characters; return what arrived, or EAGAIN (None) if nothing.
static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t *buf = buf_in;
    size_t got = 0;
    mp_uint_t timeout = self->timeout;
    mp_uint_t start = mp_hal_ticks_ms();
    while (got < size) {
        if (!uart_alive(self)) {
            if (got > 0) {
                break;
            }
            *errcode = MP_ENODEV;
            return MP_STREAM_ERROR;
        }
        size_t n = mp_uefi_serial_read(uart_port(self), buf + got, size - got);
        if (n > 0) {
            got += n;
            start = mp_hal_ticks_ms();
            timeout = self->timeout_char;
            continue;
        }
        mp_uint_t elapsed = mp_hal_ticks_ms() - start;
        if (elapsed >= timeout) {
            break;
        }
        mp_event_wait_ms(timeout - elapsed);   // the serial poll wakes us when data lands
    }
    if (got == 0 && size > 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }
    return got;
}

// Write until done, or no progress for `timeout` (at least `timeout_char`) ms; returns
// what was written, or EAGAIN (None) if nothing could be.
static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!uart_alive(self)) {
        *errcode = MP_ENODEV;
        return MP_STREAM_ERROR;
    }
    EFI_STATUS err;
    mp_uint_t stall = self->timeout > self->timeout_char ? self->timeout : self->timeout_char;
    size_t n = mp_uefi_serial_write(uart_port(self), buf_in, size, stall, &err);
    if (n == 0 && size > 0) {
        *errcode = err == EFI_NOT_FOUND ? MP_ENODEV
            : (EFI_ERROR(err) && err != EFI_TIMEOUT) ? MP_EIO : MP_EAGAIN;
        return MP_STREAM_ERROR;
    }
    return n;
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uefi_serial_t *s = uart_port(self);
    if (request == MP_STREAM_POLL) {
        if (!uart_alive(self)) {
            return MP_STREAM_POLL_ERR;    // wakes asyncio waiters; their read raises ENODEV
        }
        mp_uint_t ret = 0;
        if ((arg & MP_STREAM_POLL_RD) && mp_uefi_serial_any(s) > 0) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((arg & MP_STREAM_POLL_WR) && mp_uefi_serial_writable(s)) {
            ret |= MP_STREAM_POLL_WR;
        }
        return ret;
    } else if (request == MP_STREAM_FLUSH) {
        mp_uint_t start = mp_hal_ticks_ms();
        while (uart_alive(self) && !mp_uefi_serial_txdone(s)) {
            if (mp_hal_ticks_ms() - start > 1000 + self->timeout) {
                *errcode = MP_ETIMEDOUT;
                return MP_STREAM_ERROR;
            }
            mp_event_wait_ms(1);
        }
        if (!uart_alive(self)) {
            *errcode = MP_ENODEV;
            return MP_STREAM_ERROR;
        }
        return 0;
    } else if (request == MP_STREAM_CLOSE) {
        mp_machine_uart_deinit(self);
        return 0;
    }
    *errcode = MP_EINVAL;
    return MP_STREAM_ERROR;
}
