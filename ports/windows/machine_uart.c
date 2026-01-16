/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 MicroPython contributors
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

#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/runtime.h"

// Flow control flags for Python API (user combines with |).
#define UART_HWCONTROL_RTS (1)
#define UART_HWCONTROL_CTS (2)

// COM port name max length: "COM256\0" = 7 chars, use 16 for safety.
#define UART_PORT_NAME_MAX (16)

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    HANDLE handle;              // COM port handle
    uint8_t uart_id;            // Port number (1-255)
    char port_name[UART_PORT_NAME_MAX];
    uint32_t baudrate;
    uint8_t bits;               // 5, 6, 7, 8
    uint8_t parity;             // 0=none, 1=odd, 2=even
    uint8_t stop;               // 1 or 2
    uint8_t flow;               // RTS/CTS flags
    uint16_t timeout;           // Read timeout (ms)
    uint16_t timeout_char;      // Inter-char timeout (ms)
    uint16_t rxbuf;             // RX buffer size
    uint16_t txbuf;             // TX buffer size
} machine_uart_obj_t;

// UART.list() - enumerate available COM ports from registry
static mp_obj_t machine_uart_list(void) {
    mp_obj_t list = mp_obj_new_list(0, NULL);

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "HARDWARE\\DEVICEMAP\\SERIALCOMM",
        0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return list;  // Empty list if no ports
    }

    // Registry value names are typically short (e.g. "\Device\Serial0").
    char valueName[256];
    char portName[UART_PORT_NAME_MAX];
    DWORD valueNameSize, portNameSize, type;
    DWORD index = 0;
    LONG result;

    while (1) {
        valueNameSize = sizeof(valueName);
        portNameSize = sizeof(portName);

        result = RegEnumValueA(hKey, index++, valueName, &valueNameSize,
            NULL, &type, (LPBYTE)portName, &portNameSize);

        if (result == ERROR_NO_MORE_ITEMS) {
            break;
        }
        if (result != ERROR_SUCCESS) {
            // Skip entries that fail (e.g. buffer too small) and continue
            continue;
        }

        if (type == REG_SZ) {
            // portNameSize includes null terminator
            mp_obj_list_append(list, mp_obj_new_str(portName, portNameSize - 1));
        }
    }

    RegCloseKey(hKey);
    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_0(machine_uart_list_obj, machine_uart_list);

// Class constants
#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(UART_HWCONTROL_RTS) }, \
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(UART_HWCONTROL_CTS) }, \
    { MP_ROM_QSTR(MP_QSTR_list), MP_ROM_PTR(&machine_uart_list_obj) }, \

static const char *_parity_name[] = {"None", "ODD", "EVEN"};
#define PARITY_NAME(p) (_parity_name[(p) < 3 ? (p) : 0])

// Forward declarations
static void uart_deinit_internal(machine_uart_obj_t *self);
static bool uart_init_internal(machine_uart_obj_t *self);

/******************************************************************************/
// Helper functions

static DWORD get_baud_rate(uint32_t baudrate) {
    // Win32 supports arbitrary baud rates via CBR_* or direct value
    return baudrate;
}

static BYTE get_byte_size(uint8_t bits) {
    if (bits >= 5 && bits <= 8) {
        return bits;
    }
    return 8;
}

static BYTE get_parity(uint8_t parity) {
    switch (parity) {
        case 1:
            return ODDPARITY;
        case 2:
            return EVENPARITY;
        default:
            return NOPARITY;
    }
}

static BYTE get_stop_bits(uint8_t stop) {
    switch (stop) {
        case 2:
            return TWOSTOPBITS;
        default:
            return ONESTOPBIT;
    }
}

static void uart_deinit_internal(machine_uart_obj_t *self) {
    if (self->handle != INVALID_HANDLE_VALUE) {
        CloseHandle(self->handle);
        self->handle = INVALID_HANDLE_VALUE;
    }
}

static bool uart_init_internal(machine_uart_obj_t *self) {
    // Close if already open
    uart_deinit_internal(self);

    // Build port path (for COM10+ need \\.\ prefix)
    char port_path[24];
    snprintf(port_path, sizeof(port_path), "\\\\.\\%s", self->port_name);

    // Open COM port
    self->handle = CreateFileA(
        port_path,
        GENERIC_READ | GENERIC_WRITE,
        0,              // No sharing
        NULL,           // Default security
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
        );

    if (self->handle == INVALID_HANDLE_VALUE) {
        return false;
    }

    // Configure DCB
    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);

    if (!GetCommState(self->handle, &dcb)) {
        uart_deinit_internal(self);
        return false;
    }

    dcb.BaudRate = get_baud_rate(self->baudrate);
    dcb.ByteSize = get_byte_size(self->bits);
    dcb.Parity = get_parity(self->parity);
    dcb.StopBits = get_stop_bits(self->stop);

    // Flow control
    dcb.fOutxCtsFlow = (self->flow & UART_HWCONTROL_CTS) ? TRUE : FALSE;
    dcb.fRtsControl = (self->flow & UART_HWCONTROL_RTS) ? RTS_CONTROL_HANDSHAKE : RTS_CONTROL_ENABLE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    // DTR disabled by default to avoid resetting devices (e.g. Arduino).
    dcb.fDtrControl = DTR_CONTROL_DISABLE;

    if (!SetCommState(self->handle, &dcb)) {
        uart_deinit_internal(self);
        return false;
    }

    // Configure timeouts
    COMMTIMEOUTS timeouts = {0};
    if (self->timeout == 0 && self->timeout_char == 0) {
        // Non-blocking: return immediately
        timeouts.ReadIntervalTimeout = MAXDWORD;
        timeouts.ReadTotalTimeoutMultiplier = 0;
        timeouts.ReadTotalTimeoutConstant = 0;
    } else {
        // Blocking with timeout
        timeouts.ReadIntervalTimeout = self->timeout_char;
        timeouts.ReadTotalTimeoutMultiplier = 0;
        timeouts.ReadTotalTimeoutConstant = self->timeout;
    }
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;

    if (!SetCommTimeouts(self->handle, &timeouts)) {
        uart_deinit_internal(self);
        return false;
    }

    // Set buffer sizes (SetupComm failure is non-critical)
    SetupComm(self->handle, self->rxbuf, self->txbuf);

    // Clear buffers
    PurgeComm(self->handle, PURGE_RXCLEAR | PURGE_TXCLEAR);

    return true;
}

/******************************************************************************/
// MicroPython bindings

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    (void)kind;
    mp_printf(print, "UART(%s, baudrate=%u, bits=%u, parity=%s, stop=%u, "
        "timeout=%u, timeout_char=%u, flow=%u, rxbuf=%u, txbuf=%u)",
        self->port_name,
        self->baudrate,
        self->bits,
        PARITY_NAME(self->parity),
        self->stop,
        self->timeout,
        self->timeout_char,
        self->flow,
        self->rxbuf,
        self->txbuf);
}

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_timeout, ARG_timeout_char, ARG_flow, ARG_rxbuf, ARG_txbuf };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(-1)} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_txbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Update parameters if provided
    if (args[ARG_baudrate].u_int > 0) {
        self->baudrate = args[ARG_baudrate].u_int;
    }
    if (args[ARG_bits].u_int >= 5 && args[ARG_bits].u_int <= 8) {
        self->bits = args[ARG_bits].u_int;
    }
    if (args[ARG_parity].u_obj != MP_OBJ_NEW_SMALL_INT(-1)) {
        if (args[ARG_parity].u_obj == mp_const_none) {
            self->parity = 0;
        } else {
            mp_int_t parity = mp_obj_get_int(args[ARG_parity].u_obj);
            if (parity >= 0 && parity <= 2) {
                self->parity = parity;
            }
        }
    }
    if (args[ARG_stop].u_int >= 1 && args[ARG_stop].u_int <= 2) {
        self->stop = args[ARG_stop].u_int;
    }
    if (args[ARG_timeout].u_int >= 0) {
        self->timeout = args[ARG_timeout].u_int;
    }
    if (args[ARG_timeout_char].u_int >= 0) {
        self->timeout_char = args[ARG_timeout_char].u_int;
    }
    if (args[ARG_flow].u_int >= 0) {
        self->flow = args[ARG_flow].u_int;
    }
    if (args[ARG_rxbuf].u_int >= 0) {
        self->rxbuf = args[ARG_rxbuf].u_int;
    }
    if (args[ARG_txbuf].u_int >= 0) {
        self->txbuf = args[ARG_txbuf].u_int;
    }

    // Initialize/reinitialize the port
    if (!uart_init_internal(self)) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("could not open port %s"), self->port_name);
    }
}

static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Create new UART object
    machine_uart_obj_t *self = mp_obj_malloc(machine_uart_obj_t, &machine_uart_type);
    self->handle = INVALID_HANDLE_VALUE;

    // Get port identifier (number or string)
    if (mp_obj_is_str(args[0])) {
        // String: "COM3", "COM10", etc.
        const char *port_str = mp_obj_str_get_str(args[0]);
        snprintf(self->port_name, sizeof(self->port_name), "%s", port_str);
        // Extract number if possible (validate it's a digit after COM)
        if (strncmp(port_str, "COM", 3) == 0 && port_str[3] >= '0' && port_str[3] <= '9') {
            int port_num = atoi(port_str + 3);
            self->uart_id = (port_num > 0 && port_num <= 255) ? port_num : 0;
        } else {
            self->uart_id = 0;
        }
    } else {
        // Integer: 1 -> COM1, 3 -> COM3
        self->uart_id = mp_obj_get_int(args[0]);
        snprintf(self->port_name, sizeof(self->port_name), "COM%d", self->uart_id);
    }

    // Set defaults
    self->baudrate = 115200;
    self->bits = 8;
    self->parity = 0;
    self->stop = 1;
    self->timeout = 0;
    self->timeout_char = 0;
    self->flow = 0;
    self->rxbuf = 256;
    self->txbuf = 256;

    // Process remaining arguments
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    uart_deinit_internal(self);
}

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    if (self->handle == INVALID_HANDLE_VALUE) {
        return 0;
    }

    COMSTAT comStat;
    DWORD errors;
    if (ClearCommError(self->handle, &errors, &comStat)) {
        return comStat.cbInQue;
    }
    return 0;
}

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    if (self->handle == INVALID_HANDLE_VALUE) {
        return true;
    }

    COMSTAT comStat;
    DWORD errors;
    if (ClearCommError(self->handle, &errors, &comStat)) {
        return comStat.cbOutQue == 0;
    }
    return true;
}

#if MICROPY_PY_MACHINE_UART_SENDBREAK
static void mp_machine_uart_sendbreak(machine_uart_obj_t *self) {
    if (self->handle == INVALID_HANDLE_VALUE) {
        return;
    }
    SetCommBreak(self->handle);
    // Break duration: 13 bit periods (like rp2 port).
    mp_hal_delay_us(13 * 1000000 / self->baudrate + 1);
    ClearCommBreak(self->handle);
}
#endif

#if MICROPY_PY_MACHINE_UART_READCHAR_WRITECHAR
static mp_int_t mp_machine_uart_readchar(machine_uart_obj_t *self) {
    if (self->handle == INVALID_HANDLE_VALUE) {
        return -1;
    }

    uint8_t c;
    DWORD bytesRead;
    if (ReadFile(self->handle, &c, 1, &bytesRead, NULL) && bytesRead == 1) {
        return c;
    }
    return -1;
}

static void mp_machine_uart_writechar(machine_uart_obj_t *self, uint16_t data) {
    if (self->handle == INVALID_HANDLE_VALUE) {
        mp_raise_OSError(MP_EIO);
    }

    uint8_t c = data & 0xFF;
    DWORD bytesWritten;
    if (!WriteFile(self->handle, &c, 1, &bytesWritten, NULL) || bytesWritten != 1) {
        mp_raise_OSError(MP_EIO);
    }
}
#endif

/******************************************************************************/
// Stream protocol

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->handle == INVALID_HANDLE_VALUE) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }

    DWORD bytesRead;
    if (!ReadFile(self->handle, buf_in, size, &bytesRead, NULL)) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }

    if (bytesRead == 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    return bytesRead;
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->handle == INVALID_HANDLE_VALUE) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }

    DWORD bytesWritten;
    if (!WriteFile(self->handle, buf_in, size, &bytesWritten, NULL)) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }

    return bytesWritten;
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->handle == INVALID_HANDLE_VALUE) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }

    switch (request) {
        case MP_STREAM_POLL: {
            mp_uint_t ret = 0;
            uintptr_t flags = arg;

            if (flags & MP_STREAM_POLL_RD) {
                COMSTAT comStat;
                DWORD errors;
                if (ClearCommError(self->handle, &errors, &comStat) && comStat.cbInQue > 0) {
                    ret |= MP_STREAM_POLL_RD;
                }
            }

            if (flags & MP_STREAM_POLL_WR) {
                // Always ready to write
                ret |= MP_STREAM_POLL_WR;
            }

            return ret;
        }

        case MP_STREAM_FLUSH: {
            if (!FlushFileBuffers(self->handle)) {
                *errcode = MP_EIO;
                return MP_STREAM_ERROR;
            }
            return 0;
        }

        default:
            *errcode = MP_EINVAL;
            return MP_STREAM_ERROR;
    }
}
