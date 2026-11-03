// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdarg.h>
#include <string.h>

#include "py/mpconfig.h"
#include "py/mphal.h"
#include "py/mpprint.h"

#include "supervisor/shared/cpu.h"
#include "supervisor/shared/display.h"
#include "shared-bindings/terminalio/Terminal.h"
#include "supervisor/shared/serial.h"
#include "shared-bindings/microcontroller/Pin.h"

#if CIRCUITPY_BLE_SERIAL_SERVICE
#include "supervisor/shared/bluetooth/serial.h"
#endif

#if CIRCUITPY_USB_DEVICE
#include "shared-module/usb_cdc/__init__.h"
#endif

#if CIRCUITPY_TINYUSB
#include "supervisor/usb.h"
#include "tusb.h"
#endif

#if CIRCUITPY_WEB_WORKFLOW
#include "supervisor/shared/web_workflow/websocket.h"
#include "supervisor/shared/web_workflow/display_stream.h"
#endif

#if CIRCUITPY_CONSOLE_UART
#include "shared-bindings/busio/UART.h"

busio_uart_obj_t console_uart;
// on Espressif, the receive buffer must be larger than the hardware FIFO length. See uart_driver_install().
#if defined(SOC_UART_FIFO_LEN)
byte console_uart_rx_buf[SOC_UART_FIFO_LEN + 1];
#else
byte console_uart_rx_buf[64];
#endif
#endif

#if CIRCUITPY_USB_DEVICE || CIRCUITPY_CONSOLE_UART
// Flag to note whether this is the first write after connection.
// Delay slightly on the first write to allow time for the host to set up things,
// including turning off echo mode.
static bool _first_write_done = false;
#endif

#if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_VENDOR
bool tud_vendor_connected(void);
#endif

// Set to true to temporarily discard writes to the console only.
static bool _serial_console_write_disabled;

// Set to true to temporarily discard writes to the display terminal only.
static bool _serial_display_write_disabled;

// Indicates that serial console has been early initialized.
static bool _serial_console_early_inited = false;

#if CIRCUITPY_CONSOLE_UART

// All output to the console uart comes through this inner write function. It ensures that all
// lines are terminated with a "cooked" '\r\n' sequence. Lines that are already cooked are sent
// on unchanged.
static void inner_console_uart_write_cb(void *env, const char *str, size_t len) {
    (void)env;
    int uart_errcode;
    bool last_cr = false;
    while (len > 0) {
        size_t i = 0;
        if (str[0] == '\n' && !last_cr) {
            common_hal_busio_uart_write(&console_uart, (const uint8_t *)"\r", 1, &uart_errcode);
            i = 1;
        }
        // Lump all characters on the next line together.
        while ((last_cr || str[i] != '\n') && i < len) {
            last_cr = str[i] == '\r';
            i++;
        }
        common_hal_busio_uart_write(&console_uart, (const uint8_t *)str, i, &uart_errcode);
        str = &str[i];
        len -= i;
    }
}

#if CIRCUITPY_CONSOLE_UART_TIMESTAMP
static const mp_print_t inner_console_uart_write = {NULL, inner_console_uart_write_cb};
static uint32_t console_uart_write_prev_time = 0;
static bool console_uart_write_prev_nl = true;

static inline void console_uart_write_timestamp(void) {
    uint32_t now = supervisor_ticks_ms32();
    uint32_t delta = now - console_uart_write_prev_time;
    console_uart_write_prev_time = now;
    mp_printf(&inner_console_uart_write,
        "%01lu.%03lu(%01lu.%03lu): ",
        now / 1000, now % 1000, delta / 1000, delta % 1000);
}
#endif

static size_t console_uart_write(const char *str, size_t len) {
    // Ignore writes if console uart is not yet initialized.
    if (!_serial_console_early_inited) {
        return len;
    }

    if (!_first_write_done) {
        mp_hal_delay_ms(50);
        _first_write_done = true;
    }

    // There may be multiple newlines in the string, split at newlines.
    int remaining_len = len;
    while (remaining_len > 0) {
        #if CIRCUITPY_CONSOLE_UART_TIMESTAMP
        if (console_uart_write_prev_nl) {
            console_uart_write_timestamp();
            console_uart_write_prev_nl = false;
        }
        #endif
        int print_len = 0;
        while (print_len < remaining_len) {
            if (str[print_len++] == '\n') {
                #if CIRCUITPY_CONSOLE_UART_TIMESTAMP
                console_uart_write_prev_nl = true;
                #endif
                break;
            }
        }
        inner_console_uart_write_cb(NULL, str, print_len);
        str += print_len;
        remaining_len -= print_len;
    }
    return len;
}

static void console_uart_write_cb(void *env, const char *str, size_t len) {
    (void)env;
    console_uart_write(str, len);
}

const mp_print_t console_uart_print = {NULL, console_uart_write_cb};
#endif

MP_WEAK void board_serial_early_init(void) {
}

MP_WEAK void board_serial_init(void) {
}

MP_WEAK bool board_serial_connected(void) {
    return false;
}

MP_WEAK char board_serial_read(void) {
    return -1;
}

MP_WEAK uint32_t board_serial_bytes_available(void) {
    return 0;
}

MP_WEAK void board_serial_write_substring(const char *text, uint32_t length) {
    (void)text;
    (void)length;
}

void serial_early_init(void) {
    // Ignore duplicate calls to initialize allowing port-specific code to
    // call this function early.
    if (_serial_console_early_inited) {
        return;
    }

    #if CIRCUITPY_CONSOLE_UART
    // Set up console UART, if enabled.
    console_uart.base.type = &busio_uart_type;

    const mcu_pin_obj_t *console_rx = MP_OBJ_TO_PTR(CIRCUITPY_CONSOLE_UART_RX);
    const mcu_pin_obj_t *console_tx = MP_OBJ_TO_PTR(CIRCUITPY_CONSOLE_UART_TX);

    common_hal_busio_uart_construct(&console_uart, console_tx, console_rx, NULL, NULL, NULL,
        false, CIRCUITPY_CONSOLE_UART_BAUDRATE, 8, BUSIO_UART_PARITY_NONE, 1, 1.0f, sizeof(console_uart_rx_buf),
        console_uart_rx_buf, true);
    #if CIRCUITPY_BULK_RESET
    common_hal_busio_uart_never_reset(&console_uart);
    #endif
    #endif

    board_serial_early_init();

    #if CIRCUITPY_PORT_SERIAL
    port_serial_early_init();
    #endif

    _serial_console_early_inited = true;

    // Do an initial print so that we can confirm the serial output is working.
    CIRCUITPY_CONSOLE_UART_PRINTF("Serial console setup\n");
}

void serial_init(void) {
    #if CIRCUITPY_USB_DEVICE || CIRCUITPY_CONSOLE_UART
    _first_write_done = false;
    #endif

    board_serial_init();

    #if CIRCUITPY_PORT_SERIAL
    port_serial_init();
    #endif
}

bool serial_connected(void) {
    #if CIRCUITPY_TINYUSB && CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_VENDOR
    if (tud_vendor_connected()) {
        return true;
    }
    #endif

    #if CIRCUITPY_CONSOLE_UART
    return true;
    #endif

    #if CIRCUITPY_BLE_SERIAL_SERVICE
    if (ble_serial_connected()) {
        return true;
    }
    #endif

    #if CIRCUITPY_TINYUSB && CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_CDC
    if (usb_cdc_console_enabled() && tud_cdc_connected()) {
        return true;
    }
    #elif CIRCUITPY_TINYUSB && CIRCUITPY_USB_DEVICE
    if (tud_cdc_connected()) {
        return true;
    }
    #endif

    #if CIRCUITPY_WEB_WORKFLOW
    if (websocket_connected()) {
        return true;
    }
    #endif

    #if CIRCUITPY_TERMINALIO
    if (supervisor_terminal_started()) {
        return true;
    }
    #endif


    if (board_serial_connected()) {
        return true;
    }


    #if CIRCUITPY_PORT_SERIAL
    if (port_serial_connected()) {
        return true;
    }
    #endif

    return false;
}

char serial_read(void) {
    #if CIRCUITPY_TINYUSB && CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_VENDOR
    if (tud_vendor_connected() && tud_vendor_available() > 0) {
        char tiny_buffer;
        tud_vendor_read(&tiny_buffer, 1);
        return tiny_buffer;
    }
    #endif

    #if CIRCUITPY_CONSOLE_UART
    if (common_hal_busio_uart_rx_characters_available(&console_uart)) {
        int uart_errcode;
        char text;
        common_hal_busio_uart_read(&console_uart, (uint8_t *)&text, 1, &uart_errcode);
        return text;
    }
    #endif

    #if CIRCUITPY_BLE_SERIAL_SERVICE
    if (ble_serial_available() > 0) {
        return ble_serial_read_char();
    }
    #endif

    #if CIRCUITPY_WEB_WORKFLOW
    if (websocket_available()) {
        int c = websocket_read_char();
        if (c != -1) {
            return c;
        }
    }
    #endif

    #if CIRCUITPY_WEB_WORKFLOW && CIRCUITPY_DISPLAYIO
    if (display_stream_available()) {
        return display_stream_read_char();
    }
    #endif

    #if CIRCUITPY_USB_KEYBOARD_WORKFLOW
    if (usb_keyboard_chars_available() > 0) {
        return usb_keyboard_read_char();
    }
    #endif

    if (board_serial_bytes_available() > 0) {
        return board_serial_read();
    }


    #if CIRCUITPY_PORT_SERIAL
    if (port_serial_bytes_available() > 0) {
        return port_serial_read();
    }
    #endif

    #if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_CDC
    if (!usb_cdc_console_enabled()) {
        return -1;
    }
    #endif
    #if CIRCUITPY_TINYUSB && CIRCUITPY_USB_DEVICE
    return (char)tud_cdc_read_char();
    #endif

    return -1;
}

uint32_t serial_bytes_available(void) {
    // There may be multiple serial input channels, so sum the count from all.
    uint32_t count = 0;

    #if CIRCUITPY_TINYUSB && CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_VENDOR
    if (tud_vendor_connected()) {
        count += tud_vendor_available();
    }
    #endif

    #if CIRCUITPY_CONSOLE_UART
    count += common_hal_busio_uart_rx_characters_available(&console_uart);
    #endif

    #if CIRCUITPY_BLE_SERIAL_SERVICE
    count += ble_serial_available();
    #endif

    #if CIRCUITPY_WEB_WORKFLOW
    count += websocket_available();
    #endif

    #if CIRCUITPY_WEB_WORKFLOW && CIRCUITPY_DISPLAYIO
    count += display_stream_available();
    #endif

    #if CIRCUITPY_USB_KEYBOARD_WORKFLOW
    count += usb_keyboard_chars_available();
    #endif

    #if CIRCUITPY_TINYUSB && CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_CDC
    if (usb_cdc_console_enabled()) {
        count += tud_cdc_available();
    }
    #endif

    // Board-specific serial input.
    count += board_serial_bytes_available();

    #if CIRCUITPY_PORT_SERIAL
    // Port-specific serial input.
    count += port_serial_bytes_available();
    #endif

    return count;
}

uint32_t serial_write_substring(const char *text, uint32_t length) {
    if (length == 0) {
        return 0;
    }

    // See https://github.com/micropython/micropython/pull/11850 for the motivation for returning
    // the number of chars written.

    // Assume that unless otherwise reported, we sent all that we got.
    uint32_t length_sent = length;

    #if CIRCUITPY_TERMINALIO
    int errcode;
    if (!_serial_display_write_disabled) {
        length_sent = common_hal_terminalio_terminal_write(&supervisor_terminal, (const uint8_t *)text, length, &errcode);
    }
    #endif

    if (_serial_console_write_disabled) {
        return length_sent;
    }

    #if CIRCUITPY_TINYUSB && CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_VENDOR
    if (tud_vendor_connected()) {
        length_sent = tud_vendor_write(text, length);
    }
    #endif

    #if CIRCUITPY_CONSOLE_UART
    length_sent = console_uart_write(text, length);
    #endif

    #if CIRCUITPY_BLE_SERIAL_SERVICE
    ble_serial_write(text, length);
    #endif

    #if CIRCUITPY_WEB_WORKFLOW
    websocket_write(text, length);
    #endif

    #if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_CDC
    if (!usb_cdc_console_enabled()) {
        return length;
    }
    #endif

    #if CIRCUITPY_TINYUSB && CIRCUITPY_USB_DEVICE
    // Delay the very first write
    if (tud_cdc_connected() && !_first_write_done) {
        mp_hal_delay_ms(50);
        _first_write_done = true;
    }
    uint32_t count = 0;
    if (tud_cdc_connected()) {
        while (count < length) {
            count += tud_cdc_write(text + count, length - count);
            // If we're in an interrupt, then don't wait for more room. Queue up what we can.
            if (cpu_interrupt_active()) {
                break;
            }
            usb_background();
        }
    }
    #endif

    board_serial_write_substring(text, length);

    #if CIRCUITPY_PORT_SERIAL
    port_serial_write_substring(text, length);
    #endif

    return length_sent;
}

void serial_write(const char *text) {
    serial_write_substring(text, strlen(text));
}

bool serial_console_write_disable(bool disabled) {
    bool now = _serial_console_write_disabled;
    _serial_console_write_disabled = disabled;
    return now;
}

bool serial_display_write_disable(bool disabled) {
    bool now = _serial_display_write_disabled;
    _serial_display_write_disabled = disabled;
    return now;
}

// A general purpose hex/ascii dump function for arbitrary area of memory.
void print_hexdump(const mp_print_t *printer, const char *prefix, const uint8_t *buf, size_t len) {
    size_t i;
    for (i = 0; i < len; ++i) {
        // print hex digit
        if (i % 32 == 0) {
            mp_printf(printer, "%s0x%04x:", prefix, i);
        }
        if (i % 32 == 16) {
            mp_printf(printer, " : ");
        } else if (i % 4 == 0) {
            mp_printf(printer, " ");
        }
        mp_printf(printer, "%02x", buf[i]);
        // print ascii chars for this line
        if (i % 32 == 31) {
            size_t k = i - 31;
            mp_printf(printer, " : ");
            for (size_t j = 0; j < 32; ++j) {
                if (j == 16) {
                    mp_printf(printer, " ");
                }
                if (buf[k + j] >= 32 && buf[k + j] < 127) {
                    mp_printf(printer, "%c", buf[k + j]);
                } else {
                    mp_printf(printer, ".");
                }
            }
            mp_printf(printer, "\n");
        }
    }
    if (i % 32 != 0) {
        // For a final line of less than 32 bytes, pad with spaces
        i -= i % 32;
        for (size_t j = len % 32; j < 32; ++j) {
            if (j % 32 == 16) {
                mp_printf(printer, "   ");
            } else if (j % 4 == 0) {
                mp_printf(printer, " ");
            }
            mp_printf(printer, "  ");
        }
        // Print ascii chars for the last line fragment
        mp_printf(printer, " : ");
        for (size_t j = 0; j < len % 32; ++j) {
            if (j == 16) {
                mp_printf(printer, " ");
            }
            if (buf[i + j] >= 32 && buf[i + j] < 127) {
                mp_printf(printer, "%c", buf[i + j]);
            } else {
                mp_printf(printer, ".");
            }
        }
        mp_printf(printer, "\n");
    }
}

int console_uart_printf(const char *fmt, ...) {
    #if CIRCUITPY_CONSOLE_UART
    va_list args;
    va_start(args, fmt);
    int ret = mp_vprintf(&console_uart_print, fmt, args);
    va_end(args);
    return ret;
    #else
    return 0;
    #endif
}
