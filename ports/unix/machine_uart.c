/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
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

// UART/Serial support for Linux using /dev/ttyS*, /dev/ttyUSB*, /dev/ttyACM*
// Provides machine.UART functionality for Raspberry Pi and other Linux SBCs

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/stream.h"
#include "extmod/modmachine.h"

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    int fd;
    uint32_t baudrate;
    uint8_t databits;
    uint8_t parity;    // 0=None, 1=Even, 2=Odd
    uint8_t stopbits;  // 1 or 2
    uint8_t flow;      // 0=None, 1=RTS/CTS, 2=XON/XOFF
    uint16_t timeout_ms;
    uint16_t txbuf;
    uint16_t rxbuf;
    char device[64];
} machine_uart_obj_t;

static void machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "UART('%s', baudrate=%u, bits=%u, parity=%s, stop=%u, flow=%s)",
        self->device,
        self->baudrate,
        self->databits,
        (self->parity == 0) ? "None" : (self->parity == 1) ? "Even" : "Odd",
        self->stopbits,
        (self->flow == 0) ? "None" : (self->flow == 1) ? "RTS/CTS" : "XON/XOFF");
}

static int configure_uart(machine_uart_obj_t *self) {
    struct termios tio;

    // Get current settings
    if (tcgetattr(self->fd, &tio) < 0) {
        return -1;
    }

    // Set baud rate
    speed_t speed;
    switch (self->baudrate) {
        case 9600: speed = B9600; break;
        case 19200: speed = B19200; break;
        case 38400: speed = B38400; break;
        case 57600: speed = B57600; break;
        case 115200: speed = B115200; break;
        case 230400: speed = B230400; break;
        case 460800: speed = B460800; break;
        case 500000: speed = B500000; break;
        case 576000: speed = B576000; break;
        case 921600: speed = B921600; break;
        case 1000000: speed = B1000000; break;
        case 1152000: speed = B1152000; break;
        case 1500000: speed = B1500000; break;
        case 2000000: speed = B2000000; break;
        case 2500000: speed = B2500000; break;
        case 3000000: speed = B3000000; break;
        case 3500000: speed = B3500000; break;
        case 4000000: speed = B4000000; break;
        default:
            // Unsupported baud rate
            return -1;
    }

    cfsetispeed(&tio, speed);
    cfsetospeed(&tio, speed);

    // Configure data bits
    tio.c_cflag &= ~CSIZE;
    switch (self->databits) {
        case 5: tio.c_cflag |= CS5; break;
        case 6: tio.c_cflag |= CS6; break;
        case 7: tio.c_cflag |= CS7; break;
        case 8: tio.c_cflag |= CS8; break;
        default:
            return -1;
    }

    // Configure parity
    if (self->parity == 0) {
        tio.c_cflag &= ~PARENB;  // No parity
    } else if (self->parity == 1) {
        tio.c_cflag |= PARENB;   // Enable parity
        tio.c_cflag &= ~PARODD;  // Even parity
    } else if (self->parity == 2) {
        tio.c_cflag |= PARENB;   // Enable parity
        tio.c_cflag |= PARODD;   // Odd parity
    }

    // Configure stop bits
    if (self->stopbits == 1) {
        tio.c_cflag &= ~CSTOPB;  // 1 stop bit
    } else if (self->stopbits == 2) {
        tio.c_cflag |= CSTOPB;   // 2 stop bits
    }

    // Configure flow control
    if (self->flow == 0) {
        // No flow control
        tio.c_cflag &= ~CRTSCTS;
        tio.c_iflag &= ~(IXON | IXOFF | IXANY);
    } else if (self->flow == 1) {
        // Hardware flow control (RTS/CTS)
        tio.c_cflag |= CRTSCTS;
        tio.c_iflag &= ~(IXON | IXOFF | IXANY);
    } else if (self->flow == 2) {
        // Software flow control (XON/XOFF)
        tio.c_cflag &= ~CRTSCTS;
        tio.c_iflag |= (IXON | IXOFF);
    }

    // Raw mode
    tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tio.c_oflag &= ~OPOST;

    // Non-blocking reads
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 0;

    // Enable receiver, ignore modem control lines
    tio.c_cflag |= (CREAD | CLOCAL);

    // Apply settings
    if (tcsetattr(self->fd, TCSANOW, &tio) < 0) {
        return -1;
    }

    return 0;
}

static mp_obj_t machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Parse arguments
    enum { ARG_id, ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_flow, ARG_timeout, ARG_txbuf, ARG_rxbuf };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 115200} },
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_parity, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_stop, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_txbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 256} },
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 256} },
    };

    mp_arg_val_t parsed_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(allowed_args), allowed_args, parsed_args);

    // Create UART object
    machine_uart_obj_t *self = mp_obj_malloc(machine_uart_obj_t, &machine_uart_type);

    // Get device name/ID
    if (mp_obj_is_int(parsed_args[ARG_id].u_obj)) {
        // Numeric ID: 0 = /dev/ttyS0, 1 = /dev/ttyS1, etc.
        int uart_id = mp_obj_get_int(parsed_args[ARG_id].u_obj);
        snprintf(self->device, sizeof(self->device), "/dev/ttyS%d", uart_id);
    } else {
        // String device name
        const char *dev = mp_obj_str_get_str(parsed_args[ARG_id].u_obj);
        // If it doesn't start with /, assume it's just the device name
        if (dev[0] == '/') {
            snprintf(self->device, sizeof(self->device), "%s", dev);
        } else {
            snprintf(self->device, sizeof(self->device), "/dev/%s", dev);
        }
    }

    self->baudrate = parsed_args[ARG_baudrate].u_int;
    self->databits = parsed_args[ARG_bits].u_int;

    // Parse parity
    if (parsed_args[ARG_parity].u_obj == mp_const_none) {
        self->parity = 0;  // None
    } else if (mp_obj_is_int(parsed_args[ARG_parity].u_obj)) {
        self->parity = mp_obj_get_int(parsed_args[ARG_parity].u_obj);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid parity"));
    }

    self->stopbits = parsed_args[ARG_stop].u_int;
    self->flow = parsed_args[ARG_flow].u_int;
    self->timeout_ms = parsed_args[ARG_timeout].u_int;
    self->txbuf = parsed_args[ARG_txbuf].u_int;
    self->rxbuf = parsed_args[ARG_rxbuf].u_int;

    // Open serial device
    self->fd = open(self->device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (self->fd < 0) {
        mp_raise_OSError(errno);
    }

    // Configure UART
    if (configure_uart(self) < 0) {
        close(self->fd);
        mp_raise_OSError(errno);
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_uart_any(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    int bytes_available = 0;
    if (ioctl(self->fd, FIONREAD, &bytes_available) < 0) {
        return MP_OBJ_NEW_SMALL_INT(0);
    }

    return MP_OBJ_NEW_SMALL_INT(bytes_available);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_any_obj, machine_uart_any);

static mp_obj_t machine_uart_read(size_t n_args, const mp_obj_t *args) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t len = -1;
    if (n_args > 1) {
        len = mp_obj_get_int(args[1]);
    }

    // Determine how many bytes to read
    if (len == -1) {
        // Read all available
        int bytes_available = 0;
        if (ioctl(self->fd, FIONREAD, &bytes_available) >= 0 && bytes_available > 0) {
            len = bytes_available;
        } else {
            len = 0;
        }
    }

    if (len == 0) {
        return mp_const_none;
    }

    vstr_t vstr;
    vstr_init_len(&vstr, len);

    // Use select for timeout
    fd_set readfds;
    struct timeval tv;
    FD_ZERO(&readfds);
    FD_SET(self->fd, &readfds);
    tv.tv_sec = self->timeout_ms / 1000;
    tv.tv_usec = (self->timeout_ms % 1000) * 1000;

    int ret = select(self->fd + 1, &readfds, NULL, NULL, &tv);
    if (ret <= 0) {
        vstr_clear(&vstr);
        return mp_const_none;
    }

    ssize_t bytes_read = read(self->fd, vstr.buf, len);
    if (bytes_read < 0) {
        vstr_clear(&vstr);
        mp_raise_OSError(errno);
    }

    if (bytes_read < len) {
        vstr.len = bytes_read;
    }

    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_uart_read_obj, 1, 2, machine_uart_read);

static mp_obj_t machine_uart_readinto(size_t n_args, const mp_obj_t *args) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);

    // Use select for timeout
    fd_set readfds;
    struct timeval tv;
    FD_ZERO(&readfds);
    FD_SET(self->fd, &readfds);
    tv.tv_sec = self->timeout_ms / 1000;
    tv.tv_usec = (self->timeout_ms % 1000) * 1000;

    int ret = select(self->fd + 1, &readfds, NULL, NULL, &tv);
    if (ret <= 0) {
        return MP_OBJ_NEW_SMALL_INT(0);
    }

    ssize_t bytes_read = read(self->fd, bufinfo.buf, bufinfo.len);
    if (bytes_read < 0) {
        mp_raise_OSError(errno);
    }

    return MP_OBJ_NEW_SMALL_INT(bytes_read);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_uart_readinto_obj, 2, 3, machine_uart_readinto);

static mp_obj_t machine_uart_write(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    ssize_t bytes_written = write(self->fd, bufinfo.buf, bufinfo.len);
    if (bytes_written < 0) {
        mp_raise_OSError(errno);
    }

    // Wait for transmission to complete
    tcdrain(self->fd);

    return MP_OBJ_NEW_SMALL_INT(bytes_written);
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_uart_write_obj, machine_uart_write);

static mp_obj_t machine_uart_deinit(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->fd >= 0) {
        close(self->fd);
        self->fd = -1;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_deinit_obj, machine_uart_deinit);

static mp_obj_t machine_uart_sendbreak(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    tcsendbreak(self->fd, 0);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_sendbreak_obj, machine_uart_sendbreak);

static mp_obj_t machine_uart_flush(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    tcflush(self->fd, TCIOFLUSH);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_flush_obj, machine_uart_flush);

static const mp_rom_map_elem_t machine_uart_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&machine_uart_any_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&machine_uart_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&machine_uart_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&machine_uart_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_uart_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendbreak), MP_ROM_PTR(&machine_uart_sendbreak_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&machine_uart_flush_obj) },

    // Constants for parity
    { MP_ROM_QSTR(MP_QSTR_PARITY_NONE), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_PARITY_EVEN), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_PARITY_ODD), MP_ROM_INT(2) },

    // Constants for flow control
    { MP_ROM_QSTR(MP_QSTR_FLOW_NONE), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_FLOW_RTS_CTS), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_FLOW_XON_XOFF), MP_ROM_INT(2) },
};
static MP_DEFINE_CONST_DICT(machine_uart_locals_dict, machine_uart_locals_dict_table);

static mp_uint_t machine_uart_read_stream(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    ssize_t ret = read(self->fd, buf_in, size);
    if (ret < 0) {
        *errcode = errno;
        return MP_STREAM_ERROR;
    }

    return ret;
}

static mp_uint_t machine_uart_write_stream(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    ssize_t ret = write(self->fd, buf_in, size);
    if (ret < 0) {
        *errcode = errno;
        return MP_STREAM_ERROR;
    }

    return ret;
}

static mp_uint_t machine_uart_ioctl_stream(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    switch (request) {
        case MP_STREAM_POLL: {
            mp_uint_t ret = 0;
            if (arg & MP_STREAM_POLL_RD) {
                int bytes_available = 0;
                if (ioctl(self->fd, FIONREAD, &bytes_available) >= 0 && bytes_available > 0) {
                    ret |= MP_STREAM_POLL_RD;
                }
            }
            if (arg & MP_STREAM_POLL_WR) {
                // Always writable (could check TIOCOUTQ for more accurate check)
                ret |= MP_STREAM_POLL_WR;
            }
            return ret;
        }
        case MP_STREAM_FLUSH:
            tcdrain(self->fd);
            return 0;
        case MP_STREAM_CLOSE:
            if (self->fd >= 0) {
                close(self->fd);
                self->fd = -1;
            }
            return 0;
    }

    *errcode = MP_EINVAL;
    return MP_STREAM_ERROR;
}

static const mp_stream_p_t machine_uart_stream_p = {
    .read = machine_uart_read_stream,
    .write = machine_uart_write_stream,
    .ioctl = machine_uart_ioctl_stream,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_uart_type,
    MP_QSTR_UART,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    make_new, machine_uart_make_new,
    print, machine_uart_print,
    protocol, &machine_uart_stream_p,
    locals_dict, &machine_uart_locals_dict
);
