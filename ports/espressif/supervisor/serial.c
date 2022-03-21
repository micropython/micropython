/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
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

// This file will only be used when CIRCUITPY_USB is 0. See
// supervisor/supervisor.mk for the rule that applies.

#include <stdarg.h>
#include <string.h>

#include "py/mpconfig.h"
#include "supervisor/shared/cpu.h"
#include "supervisor/shared/display.h"
#include "shared-bindings/terminalio/Terminal.h"
#include "supervisor/serial.h"
#include "shared-bindings/microcontroller/Pin.h"

#if CIRCUITPY_SERIAL_BLE
#include "supervisor/shared/bluetooth/serial.h"
#endif

#if defined(CIRCUITPY_DEBUG_UART_TX) || defined(CIRCUITPY_DEBUG_UART_RX)
#include "py/mpprint.h"
#include "shared-bindings/busio/UART.h"
busio_uart_obj_t debug_uart;
byte buf_array[64];
#endif

#if CIRCUITPY_ESP_USB_SERIAL_JTAG
#include "supervisor/usb_serial_jtag.h"
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
    #if CIRCUITPY_ESP_USB_SERIAL_JTAG
    usb_serial_jtag_init();
    #endif
}

bool serial_connected(void) {
    #if defined(CIRCUITPY_DEBUG_UART_TX) && defined(CIRCUITPY_DEBUG_UART_RX)
    return true;
    #endif

    #if CIRCUITPY_SERIAL_BLE
    if (ble_serial_connected()) {
        return true;
    }
    #endif

    #if CIRCUITPY_ESP_USB_SERIAL_JTAG
    if (usb_serial_jtag_connected()) {
        return true;
    }
    #endif

    return false;
}

char serial_read(void) {

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

    #if CIRCUITPY_ESP_USB_SERIAL_JTAG
    if (usb_serial_jtag_bytes_available() > 0) {
        return usb_serial_jtag_read_char();
    }
    #endif
    return -1;
}

bool serial_bytes_available(void) {
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

    #if CIRCUITPY_ESP_USB_SERIAL_JTAG
    if (usb_serial_jtag_bytes_available()) {
        return true;
    }
    #endif

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

    #if defined(CIRCUITPY_DEBUG_UART_TX)
    int uart_errcode;

    common_hal_busio_uart_write(&debug_uart, (const uint8_t *)text, length, &uart_errcode);
    #endif

    #if CIRCUITPY_SERIAL_BLE
    ble_serial_write(text, length);
    #endif

    #if CIRCUITPY_ESP_USB_SERIAL_JTAG
    usb_serial_jtag_write(text, length);
    #endif
}

void serial_write(const char *text) {
    serial_write_substring(text, strlen(text));
}
