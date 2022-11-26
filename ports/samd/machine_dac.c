/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nick Moore
 * Copyright (c) 2022 Robert Hammelrath
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

#include <stdint.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "sam.h"
#include "pin_af.h"
#include "modmachine.h"

typedef struct _dac_obj_t {
    mp_obj_base_t base;
    uint8_t id;
    mp_hal_pin_obj_t gpio_id;
} dac_obj_t;

STATIC const dac_obj_t dac_obj[] = {
    #if defined(MCU_SAMD21)
    {{&machine_dac_type}, 0, PIN_PA02},
    #elif defined(MCU_SAMD51)
    {{&machine_dac_type}, 0, PIN_PA02},
    {{&machine_dac_type}, 1, PIN_PA05},
    #endif
};
Dac *const dac_bases[] = DAC_INSTS;

#if defined(MCU_SAMD21)
#define MAX_DAC_VALUE  (1023)
#elif defined(MCU_SAMD51)
#define MAX_DAC_VALUE  (4095)
static bool dac_init = false;
#endif


STATIC mp_obj_t dac_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw,
    const mp_obj_t *args) {

    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    uint8_t id = mp_obj_get_int(args[0]);
    const dac_obj_t *self = NULL;
    if (0 <= id && id <= MP_ARRAY_SIZE(dac_obj)) {
        self = &dac_obj[id];
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid Pin for DAC"));
    }

    Dac *dac = dac_bases[0]; // Just one DAC

    // Init DAC
    #if defined(MCU_SAMD21)

    // Configuration SAMD21
    // Enable APBC clocks and PCHCTRL clocks; GCLK3 at 1 MHz
    PM->APBCMASK.reg |= PM_APBCMASK_DAC;
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK3 | GCLK_CLKCTRL_ID_DAC;
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }
    // Reset DAC registers
    dac->CTRLA.bit.SWRST = 1;
    while (dac->CTRLA.bit.SWRST) {
    }
    dac->CTRLB.reg = DAC_CTRLB_EOEN | DAC_CTRLB_REFSEL(DAC_CTRLB_REFSEL_AVCC_Val);
    // Enable DAC and wait to be ready
    dac->CTRLA.bit.ENABLE = 1;
    while (dac->STATUS.bit.SYNCBUSY) {
    }

    #elif defined(MCU_SAMD51)

    // Configuration SAMD51
    // Enable APBD clocks and PCHCTRL clocks; GCLK3 at 8 MHz
    if (!dac_init) {
        dac_init = true;
        MCLK->APBDMASK.reg |= MCLK_APBDMASK_DAC;
        GCLK->PCHCTRL[DAC_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK3 | GCLK_PCHCTRL_CHEN;

        // Reset DAC registers
        dac->CTRLA.bit.SWRST = 1;
        while (dac->CTRLA.bit.SWRST) {
        }
        dac->CTRLB.reg = DAC_CTRLB_REFSEL(DAC_CTRLB_REFSEL_VDDANA_Val);
    } else {
        dac->CTRLA.bit.ENABLE = 0;
        while (dac->SYNCBUSY.bit.ENABLE) {
        }
    }
    dac->DACCTRL[self->id].reg = DAC_DACCTRL_ENABLE | DAC_DACCTRL_REFRESH(2) | DAC_DACCTRL_CCTRL_CC12M;

    // Enable DAC and wait to be ready
    dac->CTRLA.bit.ENABLE = 1;
    while (dac->SYNCBUSY.bit.ENABLE) {
    }
    #endif

    // Set the port as given in self->gpio_id as DAC
    mp_hal_set_pin_mux(self->gpio_id, ALT_FCT_DAC);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void dac_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    dac_obj_t *self = self_in;
    mp_printf(print, "DAC(%u, Pin=%s)", self->id, pin_name(self->gpio_id));
}

STATIC mp_obj_t dac_write(mp_obj_t self_in, mp_obj_t value_in) {
    Dac *dac = dac_bases[0]; // Just one DAC
    int value = mp_obj_get_int(value_in);
    if (value < 0 || value > MAX_DAC_VALUE) {
        mp_raise_ValueError(MP_ERROR_TEXT("value out of range"));
    }
    #if defined(MCU_SAMD21)
    dac->DATA.reg = value;
    #elif defined(MCU_SAMD51)
    dac_obj_t *self = self_in;
    dac->DATA[self->id].reg = value;
    #endif

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(dac_write_obj, dac_write);

STATIC const mp_rom_map_elem_t dac_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&dac_write_obj) },
};

STATIC MP_DEFINE_CONST_DICT(dac_locals_dict, dac_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_dac_type,
    MP_QSTR_DAC,
    MP_TYPE_FLAG_NONE,
    make_new, dac_make_new,
    print, dac_print,
    locals_dict, &dac_locals_dict
    );
