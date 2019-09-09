/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
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
#ifndef _PYB_PINMUX_H_
#define _PYB_PINMUX_H_

#if defined (MICROPY_PY_PINMUX) && MICROPY_PY_PINMUX
typedef struct _pinmux_obj_t {
    mp_obj_base_t base;
    qstr pinmux_name;
    const uint32_t muxRegister;
    const uint32_t muxMode;
    const uint32_t inputRegister;
    const uint32_t inputDaisy;
    const uint32_t configRegister;
}pinmux_obj_t;

typedef struct _port_t{
    mp_obj_base_t base;
    qstr port_name;
    uint8_t pinmux_num;
    const pinmux_obj_t *pinmux;
}port_obj_t;

#include "port-pad.h"

extern const mp_obj_type_t pinmux_type;
extern const mp_obj_type_t port_type;
extern const mp_obj_type_t board_ports_obj_type;

extern const mp_obj_dict_t port_board_pinmux_locals_dict;
#endif /*MICROPY_PY_PINMUX*/
#endif