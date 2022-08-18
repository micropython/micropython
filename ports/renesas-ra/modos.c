/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2022 Renesas Electronics Corporation
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

#include "py/runtime.h"
#include "uart.h"

bool mp_os_dupterm_is_builtin_stream(mp_const_obj_t stream) {
    const mp_obj_type_t *type = mp_obj_get_type(stream);
    return type == &machine_uart_type;
}

void mp_os_dupterm_stream_detached_attached(mp_obj_t stream_detached, mp_obj_t stream_attached) {
    if (mp_obj_get_type(stream_detached) == &machine_uart_type) {
        uart_attach_to_repl(MP_OBJ_TO_PTR(stream_detached), false);
    }

    if (mp_obj_get_type(stream_attached) == &machine_uart_type) {
        uart_attach_to_repl(MP_OBJ_TO_PTR(stream_attached), true);
    }
}
