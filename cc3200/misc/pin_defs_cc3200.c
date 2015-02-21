/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#include "py/mpconfig.h"
#include "py/obj.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "gpio.h"
#include "pin.h"
#include "pybpin.h"
#include MICROPY_HAL_H


// Returns the pin mode. This value returned by this macro should be one of:
// GPIO_DIR_MODE_IN or GPIO_DIR_MODE_OUT
uint32_t pin_get_mode(const pin_obj_t *self) {
    return MAP_GPIODirModeGet(self->port, self->bit);
}

uint32_t pin_get_type(const pin_obj_t *self) {

    uint32_t strenght;
    uint32_t type;

    MAP_PinConfigGet(self->pin_num, &strenght, &type);

    return type;
}

uint32_t pin_get_strenght (const pin_obj_t *self) {

    uint32_t strenght;
    uint32_t type;

    MAP_PinConfigGet(self->pin_num, &strenght, &type);

    return strenght;
}

