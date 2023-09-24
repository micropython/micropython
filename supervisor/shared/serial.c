/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include <stdarg.h>
#include <string.h>

#include "py/mpconfig.h"
#include "py/mphal.h"

#include "supervisor/shared/cpu.h"
#include "supervisor/shared/display.h"
#include "shared-bindings/terminalio/Terminal.h"
#include "supervisor/serial.h"
#include "supervisor/usb.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/usb_cdc/__init__.h"

#if CIRCUITPY_SERIAL_BLE
#include "supervisor/shared/bluetooth/serial.h"
#endif

#if CIRCUITPY_USB
#include "tusb.h"
#endif

#if CIRCUITPY_WEB_WORKFLOW
#include "supervisor/shared/web_workflow/websocket.h"
#endif

#if CIRCUITPY_CONSOLE_UART
#include "py/mpprint.h"
#include "shared-bindings/busio/UART.h"

busio_uart_obj_t console_uart;
// on Espressif, the receive buffer must be larger than the hardware FIFO length. See uart_driver_install().
#if defined(SOC_UART_FIFO_LEN)
byte console_uart_rx_buf[SOC_UART_FIFO_LEN + 1];
#else
byte console_uart_rx_buf[64];
#endif
#endif

#if CIRCUITPY_USB || CIRCUITPY_CONSOLE_UART
// Flag to note whether this is the first write after connection.
// Delay slightly on the first write to allow time for the host to set up things,
// including turning off echo mode.
static bool _first_write_done = false;
#endif

#if CIRCUITPY_USB_VENDOR
bool tud_vendor_connected(void);
#endif

// Set to true to temporarily discard writes to the console only.
static bool _serial_console_write_disabled;

// Set to true to temporarily discard writes to the display terminal only.
static bool _serial_display_write_disabled;

#if CIRCUITPY_CONSOLE_UART
STATIC void console_uart_print_strn(void *env, const char *str, size_t len) {
    (void)env;
    int uart_errcode;
    common_hal_busio_uart_write(&console_uart, (const uint8_t *)str, len, &uart_errcode);
}

const mp_print_t console_uart_print = {NULL, console_uart_print_strn};
#endif

int console_uart_printf(const char *fmt, ...) {
    #if CIRCUITPY_CONSOLE_UART
    // Skip prints that occur before console serial is started. It's better than
    // crashing.
    if (common_hal_busio_uart_deinited(&console_uart)) {
        return 0;
    }
    va_list ap;
    va_start(ap, fmt);
    int ret = mp_vprintf(&console_uart_print, fmt, ap);
    va_end(ap);
    return ret;
    #else
    return 0;
    #endif
}

MP_WEAK void port_serial_early_init(void) {
}

MP_WEAK void port_serial_init(void) {
}

MP_WEAK bool port_serial_connected(void) {
    return false;
}

MP_WEAK char port_serial_read(void) {
    return -1;
}

MP_WEAK bool port_serial_bytes_available(void) {
    return false;
}

MP_WEAK void port_serial_write_substring(const char *text, uint32_t length) {
    (void)text;
    (void)length;
}

void serial_early_init(void) {
    // Set up console UART, if enabled.

    #if CIRCUITPY_CONSOLE_UART
    console_uart.base.type = &busio_uart_type;

    const mcu_pin_obj_t *console_rx = MP_OBJ_TO_PTR(CIRCUITPY_CONSOLE_UART_RX);
    const mcu_pin_obj_t *console_tx = MP_OBJ_TO_PTR(CIRCUITPY_CONSOLE_UART_TX);

    common_hal_busio_uart_construct(&console_uart, console_tx, console_rx, NULL, NULL, NULL,
        false, CIRCUITPY_CONSOLE_UART_BAUDRATE, 8, BUSIO_UART_PARITY_NONE, 1, 1.0f, sizeof(console_uart_rx_buf),
        console_uart_rx_buf, true);
    common_hal_busio_uart_never_reset(&console_uart);

    // Do an initial print so that we can confirm the serial output is working.
    console_uart_printf("Serial console setup\r\n");
    #endif

    port_serial_early_init();
}

void serial_init(void) {
    #if CIRCUITPY_USB || CIRCUITPY_CONSOLE_UART
    _first_write_done = false;
    #endif

    port_serial_init();
}

bool serial_connected(void) {
    #if CIRCUITPY_USB_VENDOR
    if (tud_vendor_connected()) {
        return true;
    }
    #endif

    #if CIRCUITPY_CONSOLE_UART
    return true;
    #endif

    #if CIRCUITPY_SERIAL_BLE
    if (ble_serial_connected()) {
        return true;
    }
    #endif

    #if CIRCUITPY_USB_CDC
    if (usb_cdc_console_enabled() && tud_cdc_connected()) {
        return true;
    }
    #elif CIRCUITPY_USB
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


    if (port_serial_connected()) {
        return true;
    }
    return false;
}

char serial_read(void) {
    #if CIRCUITPY_USB_VENDOR
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

    #if CIRCUITPY_SERIAL_BLE
    if (ble_serial_available() > 0) {
        return ble_serial_read_char();
    }
    #endif

    #if CIRCUITPY_WEB_WORKFLOW
    if (websocket_available()) {
        char c = websocket_read_char();
        if (c != -1) {
            return c;
        }
    }
    #endif

    #if CIRCUITPY_USB_KEYBOARD_WORKFLOW
    if (usb_keyboard_chars_available() > 0) {
        return usb_keyboard_read_char();
    }
    #endif

    if (port_serial_bytes_available() > 0) {
        return port_serial_read();
    }

    #if CIRCUITPY_USB_CDC
    if (!usb_cdc_console_enabled()) {
        return -1;
    }
    #endif
    #if CIRCUITPY_USB
    return (char)tud_cdc_read_char();
    #endif

    return -1;
}

bool serial_bytes_available(void) {
    #if CIRCUITPY_USB_VENDOR
    if (tud_vendor_connected() && tud_vendor_available() > 0) {
        return true;
    }
    #endif

    #if CIRCUITPY_CONSOLE_UART
    if (common_hal_busio_uart_rx_characters_available(&console_uart)) {
        return true;
    }
    #endif

    #if CIRCUITPY_SERIAL_BLE
    if (ble_serial_available()) {
        return true;
    }
    #endif

    #if CIRCUITPY_WEB_WORKFLOW
    if (websocket_available()) {
        return true;
    }
    #endif

    #if CIRCUITPY_USB_KEYBOARD_WORKFLOW
    if (usb_keyboard_chars_available() > 0) {
        return true;
    }
    #endif

    #if CIRCUITPY_USB_CDC
    if (usb_cdc_console_enabled() && tud_cdc_available() > 0) {
        return true;
    }
    #endif
    #if CIRCUITPY_USB
    if (tud_cdc_available() > 0) {
        return true;
    }
    #endif

    if (port_serial_bytes_available() > 0) {
        return true;
    }
    return false;
}

void serial_write_substring(const char *text, uint32_t length) {
    if (length == 0) {
        return;
    }

    #if CIRCUITPY_TERMINALIO
    int errcode;
    if (!_serial_display_write_disabled) {
        common_hal_terminalio_terminal_write(&supervisor_terminal, (const uint8_t *)text, length, &errcode);
    }
    #endif

    if (_serial_console_write_disabled) {
        return;
    }

    #if CIRCUITPY_USB_VENDOR
    if (tud_vendor_connected()) {
        tud_vendor_write(text, length);
    }
    #endif

    #if CIRCUITPY_CONSOLE_UART
    if (!_first_write_done) {
        mp_hal_delay_ms(50);
        _first_write_done = true;
    }
    int uart_errcode;
    common_hal_busio_uart_write(&console_uart, (const uint8_t *)text, length, &uart_errcode);
    #endif

    #if CIRCUITPY_SERIAL_BLE
    ble_serial_write(text, length);
    #endif

    #if CIRCUITPY_WEB_WORKFLOW
    websocket_write(text, length);
    #endif

    #if CIRCUITPY_USB_CDC
    if (!usb_cdc_console_enabled()) {
        return;
    }
    #endif

    #if CIRCUITPY_USB
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

    port_serial_write_substring(text, length);
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
