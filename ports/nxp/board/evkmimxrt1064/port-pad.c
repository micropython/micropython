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
#include <stdio.h>

#include "py/obj.h" 

#include "fsl_iomuxc.h"
#include "pybpinmux.h"

#define IOMUX(pad, port, muxRegisterWord, muxModeWord, inputRegisterWord, inputDaisyWord, configRegisterWord) \
{ \
    {&pinmux_type}, \
    .pinmux_name = MP_QSTR_IOMUXC_## pad ## _ ## port, \
    .muxRegister = muxRegisterWord,\
    .muxMode = muxModeWord, \
    .inputRegister = inputRegisterWord, \
    .inputDaisy = inputDaisyWord, \
    .configRegister = configRegisterWord \
}

#define PORT(port, pinmuxArray) \
{ \
    {&port_type}, \
    .port_name = MP_QSTR_## port, \
    .pinmux_num = sizeof(pinmuxArray)/sizeof(pinmux_obj_t), \
    .pinmux = pinmuxArray, \
}

const pinmux_obj_t LPI2C1_SCL_pinmux [] = {
    IOMUX(GPIO_AD_B1_00, LPI2C1_SCL, 0x401F80FCU, 0x3U, 0x401F84CCU, 0x1U, 0x401F82ECU),
    IOMUX(GPIO_SD_B1_04, LPI2C1_SCL, 0x401F81E4U, 0x2U, 0x401F84CCU, 0x0U, 0x401F83D4U),
};

const port_obj_t LPI2C1_SCL_obj = PORT(LPI2C1_SCL, LPI2C1_SCL_pinmux);

const pinmux_obj_t LPI2C1_SDA_pinmux [] = {
    IOMUX(GPIO_AD_B1_01, LPI2C1_SDA, 0x401F8100U, 0x3U, 0x401F84D0U, 0x1U, 0x401F82F0U),
    IOMUX(GPIO_SD_B1_05, LPI2C1_SDA, 0x401F81E8U, 0x2U, 0x401F84D0U, 0x0U, 0x401F83D8U),
};

const port_obj_t LPI2C1_SDA_obj = PORT(LPI2C1_SDA, LPI2C1_SDA_pinmux);

const port_obj_t * const Module_LPI2C1[] = 
{
    &LPI2C1_SCL_obj,
    &LPI2C1_SDA_obj,
};


STATIC const mp_rom_map_elem_t port_board_pinmux_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_LPI2C1_SCL), MP_ROM_PTR(&LPI2C1_SCL_obj) },
    { MP_ROM_QSTR(MP_QSTR_LPI2C1_SDA), MP_ROM_PTR(&LPI2C1_SDA_obj) },
};
MP_DEFINE_CONST_DICT(port_board_pinmux_locals_dict, port_board_pinmux_locals_dict_table);