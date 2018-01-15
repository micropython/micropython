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

#include "supervisor/serial.h"

#include "py/obj.h"
#include "py/runtime.h"
#include "mphalport.h"
#include "pins.h"
#include "hal_uart.h"

#if (MICROPY_PY_BLE_NUS)
#include "ble_uart.h"
#endif

void serial_init(void) {

#if MICROPY_PY_BLE_NUS
    ble_uart_init0();
    while (!ble_uart_enabled()) {
        ;
    }
#else
  hal_uart_init_t param =
  {
      .id           = 0,
      .rx_pin       = &MICROPY_HW_UART1_RX,
      .tx_pin       = &MICROPY_HW_UART1_TX,
      .rts_pin      = NULL,
      .cts_pin      = NULL,
      .flow_control = MICROPY_HW_UART1_HWFC ? true : false,
      .use_parity   = false,
      .baud_rate    = HAL_UART_BAUD_115K2,
      .irq_priority = 6,
      .irq_num      = UARTE0_UART0_IRQn
  };

  hal_uart_init( UART_BASE(0), &param);
#endif
}


bool serial_connected(void) {
    return true;
}

char serial_read(void) {
    return (char) mp_hal_stdin_rx_chr();
}

bool serial_bytes_available(void) {
    return mp_hal_stdin_any();
}

void serial_write(const char* text) {
  mp_hal_stdout_tx_str(text);
}

