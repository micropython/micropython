/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Glenn Ruben Bakke
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

#include <errno.h>
#include <string.h>

#include "py/mpstate.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "hal_uart.h"

#define UART_INSTANCE   UART_BASE(0)

#if (MICROPY_PY_BLE_NUS == 0)
int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        if ( hal_uart_available(UART_INSTANCE) ) {
          uint8_t ch;
          hal_uart_char_read(UART_INSTANCE, &ch);
          return (int) ch;
        }
    }

    return 0;
}

bool mp_hal_stdin_any(void) {
  return hal_uart_available(UART_INSTANCE);
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {

//    #ifdef MICROPY_HW_LED_TX
//    gpio_toggle_pin_level(MICROPY_HW_LED_TX);
//    #endif
//
//    #ifdef CIRCUITPY_BOOT_OUTPUT_FILE
//    if (boot_output_file != NULL) {
//        UINT bytes_written = 0;
//        f_write(boot_output_file, str, len, &bytes_written);
//    }
//    #endif

    if ( hal_uart_inited(UART_INSTANCE) ) {
        while(len--) {
            hal_uart_char_write(UART_INSTANCE, *str++);
        }
    }
}

void mp_hal_stdout_tx_strn_cooked(const char *str, mp_uint_t len) {
  while(len--){
    if (*str == '\n') {
      hal_uart_char_write(UART_INSTANCE, '\r');
    }
    hal_uart_char_write(UART_INSTANCE, *str++);
  }
}
#endif

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}
