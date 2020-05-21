/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Nick Moore for Adafruit Industries
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

#include "py/obj.h"
#include "common-hal/watchdog/__init__.h"
#include "common-hal/watchdog/WatchDogTimer.h"

STATIC const mp_rom_map_elem_t watchdog_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR_WatchDogTimer), MP_ROM_PTR(&watchdog_watchdogtimer_type) },
};
STATIC MP_DEFINE_CONST_DICT(watchdog_locals_dict, watchdog_locals_dict_table);

const mp_obj_type_t watchdog_type = {
    { &mp_type_type },
    .name = MP_QSTR_watchdog,
    // .make_new = watchdog_watchdogtimer_make_new,
    .locals_dict = (mp_obj_dict_t*)&watchdog_locals_dict,
};

