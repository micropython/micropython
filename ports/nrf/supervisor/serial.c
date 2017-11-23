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
#include "uart.h"

void serial_init(void) {
  uart_init0();

  mp_obj_t args[2] = {
      MP_OBJ_NEW_SMALL_INT(0),
      MP_OBJ_NEW_SMALL_INT(115200),
  };
  MP_STATE_PORT(pyb_stdio_uart) = machine_hard_uart_type.make_new((mp_obj_t)&machine_hard_uart_type, MP_ARRAY_SIZE(args), 0, args);
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

