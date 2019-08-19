/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "pin_remap.h"

/// \moduleref machine
/// \class PinMapper - control I/O pins's remap


void pin_remap_init0(void) {
    __HAL_RCC_AFIO_CLK_ENABLE();

    #if MICROPY_HW_DEBUG == 2 
    __HAL_AFIO_REMAP_SWJ_NOJTAG();
    #elif MICROPY_HW_DEBUG == 4
    __HAL_AFIO_REMAP_SWJ_DISABLE();
    #elif MICROPY_HW_DEBUG == 1
    __HAL_AFIO_REMAP_SWJ_NONJTRST();
    #elif defined(MICROPY_HW_DEBUG) && MICROPY_HW_DEBUG == 0
    __HAL_AFIO_REMAP_SWJ_ENABLE();
    #endif
}

uint32_t pin_remap_config(uint8_t periph, int8_t value) {
    uint32_t pos = periph & 0x1f;
    uint32_t mask = ((periph >> 8) & 0x07);

    if (value == -1) {
        uint32_t cfg = (
                    #ifdef STM32F103xG
                        ( (periph&0x8000 ? AFIO->MAPR2 : AFIO->MAPR) >> pos )
                    #else
                        (AFIO->MAPR >> pos)
                    #endif
                ) & mask;
        
        return cfg;
    } else {
        if ( (periph & 0x8000) == 0) {
            AFIO->MAPR &=  (~mask)      << pos;
            AFIO->MAPR |= ( (value&mask)<< pos | AFIO_MAPR_SWJ_CFG_Msk );
        }
        #ifdef STM32F103xG
        else {
            AFIO->MAPR2 &=  (~mask)     << pos;
            AFIO->MAPR2 |= ((value&mask)<< pos | AFIO_MAPR_SWJ_CFG_Msk);
        }
        #endif

        return 0;
    }
}

/// \method remap(periph, remap)
/// Returns the periph remap config or set
STATIC mp_obj_t pin_remap_remap(size_t n_args, const mp_obj_t *args) {
    printf("n_args: %d\n", n_args);
    mp_arg_check_num(n_args, 0, 2, 3, false);

    uint8_t periph = mp_obj_get_int(args[1]);
    int8_t value = -1;
    if (n_args == 3) {
        value = mp_obj_get_int(args[2]) & 3;
    }
    uint32_t cfg = pin_remap_config(periph, value);
    if (n_args == 2) {
        return MP_OBJ_NEW_SMALL_INT(cfg);
    } else {
        return mp_const_none;
    }  
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_remap_remapfun_obj, 2, 3, pin_remap_remap);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(pin_remap_obj, MP_ROM_PTR(&pin_remap_remapfun_obj));


STATIC const mp_rom_map_elem_t pin_remap_locals_dict_table[] = {
    // class methods
    { MP_ROM_QSTR(MP_QSTR_remap),  MP_ROM_PTR(&pin_remap_obj) },

    // class constants for remap value
    { MP_ROM_QSTR(MP_QSTR_VAL_NONE),  MP_ROM_INT(REMAP_VAL_NONE) },
    { MP_ROM_QSTR(MP_QSTR_VAL_PART1), MP_ROM_INT(REMAP_VAL_PART1) },
    { MP_ROM_QSTR(MP_QSTR_VAL_PART2), MP_ROM_INT(REMAP_VAL_PART2) },
    { MP_ROM_QSTR(MP_QSTR_VAL_FULL),  MP_ROM_INT(REMAP_VAL_FULL) },

    // class contants for pheriph pins
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_SPI1),         MP_ROM_INT(REMAP_PIN_SPI1) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_I2C1),         MP_ROM_INT(REMAP_PIN_I2C1) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_USART1),       MP_ROM_INT(REMAP_PIN_USART1) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_USART2),       MP_ROM_INT(REMAP_PIN_USART2) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_USART3),       MP_ROM_INT(REMAP_PIN_USART3) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_TIM1),         MP_ROM_INT(REMAP_PIN_TIM1) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_TIM2),         MP_ROM_INT(REMAP_PIN_TIM2) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_TIM3),         MP_ROM_INT(REMAP_PIN_TIM3) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_TIM4),         MP_ROM_INT(REMAP_PIN_TIM4) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_CAN),          MP_ROM_INT(REMAP_PIN_CAN) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_PD01),         MP_ROM_INT(REMAP_PIN_PD01) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_TIM5CH_4),     MP_ROM_INT(REMAP_PIN_TIM5CH_4) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_ADC1_ETRGINJ), MP_ROM_INT(REMAP_PIN_ADC1_ETRGINJ) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_ADC1_ETRGREG), MP_ROM_INT(REMAP_PIN_ADC1_ETRGREG) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_ADC2_ETRGINJ), MP_ROM_INT(REMAP_PIN_ADC2_ETRGINJ) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_ADC2_ETRGREG), MP_ROM_INT(REMAP_PIN_ADC2_ETRGREG) },

#ifdef STM32F103xG
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_TIM9),      MP_ROM_INT(REMAP_PIN_TIM9) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_TIM10),     MP_ROM_INT(REMAP_PIN_TIM10) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_TIM11),     MP_ROM_INT(REMAP_PIN_TIM11) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_TIM13),     MP_ROM_INT(REMAP_PIN_TIM13) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_TIM14),     MP_ROM_INT(REMAP_PIN_TIM14) },
    { MP_ROM_QSTR(MP_QSTR_REMAP_PIN_FSMC_NADV), MP_ROM_INT(REMAP_PIN_FSMC_NADV) },
#endif
};
STATIC MP_DEFINE_CONST_DICT(pin_remap_locals_dict, pin_remap_locals_dict_table);

const mp_obj_type_t pin_remap_type = {
    { &mp_type_type },
    .name = MP_QSTR_PinRemap,
    .locals_dict = (mp_obj_dict_t*)&pin_remap_locals_dict,
};