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

/*
 * Note: DEBUG_UART currently only works on STM32 and nRF.
 * Enabling on another platform will cause a crash.
 */

#if defined(CIRCUITPY_DEBUG_UART_TX) || defined(CIRCUITPY_DEBUG_UART_RX)
#include "py/mpprint.h"
#include "shared-bindings/busio/UART.h"
busio_uart_obj_t debug_uart;
byte buf_array[64];
#endif

#if CIRCUITPY_USB_VENDOR
bool tud_vendor_connected(void);
#endif

#if defined(CIRCUITPY_DEBUG_UART_TX)
STATIC void debug_uart_print_strn(void *env, const char *str, size_t len) {
    (void)env;
    int uart_errcode;
    common_hal_busio_uart_write(&debug_uart, (const uint8_t *)str, len, &uart_errcode);
}

const mp_print_t debug_uart_print = {NULL, debug_uart_print_strn};
#endif

int debug_uart_printf(const char *fmt, ...) {
    #if defined(CIRCUITPY_DEBUG_UART_TX)
    // Skip prints that occur before debug serial is started. It's better than
    // crashing.
    if (common_hal_busio_uart_deinited(&debug_uart)) {
        return 0;
    }
    va_list ap;
    va_start(ap, fmt);
    int ret = mp_vprintf(&debug_uart_print, fmt, ap);
    va_end(ap);
    return ret;
    #else
    return 0;
    #endif
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
    #if defined(CIRCUITPY_DEBUG_UART_TX) || defined(CIRCUITPY_DEBUG_UART_RX)
    debug_uart.base.type = &busio_uart_type;

    #if defined(CIRCUITPY_DEBUG_UART_RX)
    const mcu_pin_obj_t *rx = MP_OBJ_TO_PTR(CIRCUITPY_DEBUG_UART_RX);
    #else
    const mcu_pin_obj_t *rx = NULL;
    #endif

    #if defined(CIRCUITPY_DEBUG_UART_TX)
    const mcu_pin_obj_t *tx = MP_OBJ_TO_PTR(CIRCUITPY_DEBUG_UART_TX);
    #else
    const mcu_pin_obj_t *tx = NULL;
    #endif

    common_hal_busio_uart_construct(&debug_uart, tx, rx, NULL, NULL, NULL,
        false, 115200, 8, BUSIO_UART_PARITY_NONE, 1, 1.0f, 64,
        buf_array, true);
    common_hal_busio_uart_never_reset(&debug_uart);

    // Do an initial print so that we can confirm the serial output is working.
    debug_uart_printf("Serial debug setup\r\n");
    #endif
}

void serial_init(void) {
    port_serial_init();
}

bool serial_connected(void) {
    #if CIRCUITPY_USB_VENDOR
    if (tud_vendor_connected()) {
        return true;
    }
    #endif

    #if defined(CIRCUITPY_DEBUG_UART_TX) && defined(CIRCUITPY_DEBUG_UART_RX)
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

    #if defined(CIRCUITPY_DEBUG_UART_RX)
    if (common_hal_busio_uart_rx_characters_available(&debug_uart)) {
        int uart_errcode;
        char text;
        common_hal_busio_uart_read(&debug_uart, (uint8_t *)&text, 1, &uart_errcode);
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
        return websocket_read_char();
    }
    #endif

    #if CIRCUITPY_USB_CDC
    if (!usb_cdc_console_enabled()) {
        return -1;
    }
    #endif
    #if CIRCUITPY_USB
    return (char)tud_cdc_read_char();
    #endif

    if (port_serial_bytes_available() > 0) {
        return port_serial_read();
    }
    return -1;
}

bool serial_bytes_available(void) {
    #if CIRCUITPY_USB_VENDOR
    if (tud_vendor_connected() && tud_vendor_available() > 0) {
        return true;
    }
    #endif

    #if defined(CIRCUITPY_DEBUG_UART_RX)
    if (common_hal_busio_uart_rx_characters_available(&debug_uart)) {
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
    common_hal_terminalio_terminal_write(&supervisor_terminal, (const uint8_t *)text, length, &errcode);
    #endif

    #if CIRCUITPY_USB_VENDOR
    if (tud_vendor_connected()) {
        tud_vendor_write(text, length);
    }
    #endif

    #if defined(CIRCUITPY_DEBUG_UART_TX)
    int uart_errcode;

    common_hal_busio_uart_write(&debug_uart, (const uint8_t *)text, length, &uart_errcode);
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
    uint32_t count = 0;
    while (count < length && tud_cdc_connected()) {
        count += tud_cdc_write(text + count, length - count);
        // If we're in an interrupt, then don't wait for more room. Queue up what we can.
        if (cpu_interrupt_active()) {
            break;
        }
        usb_background();
    }
    #endif

    port_serial_write_substring(text, length);
}

void serial_write(const char *text) {
    serial_write_substring(text, strlen(text));
}
