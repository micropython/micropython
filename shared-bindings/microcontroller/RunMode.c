/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "shared-bindings/microcontroller/RunMode.h"

//| class RunMode:
//|     """run state of the microcontroller"""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define the run mode of the microcontroller and
//|         CircuitPython."""
//|
//|     NORMAL: RunMode
//|     """Run CircuitPython as normal.
//|
//|     :type microcontroller.RunMode:"""
//|
//|     SAFE_MODE: RunMode
//|     """Run CircuitPython in safe mode. User code will not be run and the
//|     file system will be writeable over USB.
//|
//|     :type microcontroller.RunMode:"""
//|
//|     BOOTLOADER: RunMode
//|     """Run the bootloader.
//|
//|     :type microcontroller.RunMode:"""
//|
const mp_obj_type_t mcu_runmode_type;

const mcu_runmode_obj_t mcu_runmode_normal_obj = {
    { &mcu_runmode_type },
};

const mcu_runmode_obj_t mcu_runmode_safe_mode_obj = {
    { &mcu_runmode_type },
};

const mcu_runmode_obj_t mcu_runmode_bootloader_obj = {
    { &mcu_runmode_type },
};

STATIC const mp_rom_map_elem_t mcu_runmode_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR_NORMAL),     MP_ROM_PTR(&mcu_runmode_normal_obj)},
    {MP_ROM_QSTR(MP_QSTR_SAFE_MODE),  MP_ROM_PTR(&mcu_runmode_safe_mode_obj)},
    {MP_ROM_QSTR(MP_QSTR_BOOTLOADER), MP_ROM_PTR(&mcu_runmode_bootloader_obj)},
};
STATIC MP_DEFINE_CONST_DICT(mcu_runmode_locals_dict, mcu_runmode_locals_dict_table);

STATIC void mcu_runmode_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr runmode = MP_QSTR_NORMAL;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&mcu_runmode_safe_mode_obj)) {
        runmode = MP_QSTR_SAFE_MODE;
    } else if (MP_OBJ_TO_PTR(self_in) ==
               MP_ROM_PTR(&mcu_runmode_bootloader_obj)) {
        runmode = MP_QSTR_BOOTLOADER;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_microcontroller, MP_QSTR_RunMode,
        runmode);
}

const mp_obj_type_t mcu_runmode_type = {
    { &mp_type_type },
    .name = MP_QSTR_RunMode,
    .print = mcu_runmode_print,
    .locals_dict = (mp_obj_t)&mcu_runmode_locals_dict,
};
