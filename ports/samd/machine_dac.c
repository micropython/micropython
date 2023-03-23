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

#include "py/runtime.h"

#if MICROPY_PY_MACHINE_DAC

#include <stdint.h>
#include "py/obj.h"
#include "py/mphal.h"

#include "sam.h"
#include "pin_af.h"
#include "modmachine.h"

typedef struct _dac_obj_t {
    mp_obj_base_t base;
    uint8_t id;
    mp_hal_pin_obj_t gpio_id;
    uint8_t vref;
} dac_obj_t;

static dac_obj_t dac_obj[] = {
    #if defined(MCU_SAMD21)
    {{&machine_dac_type}, 0, PIN_PA02},
    #elif defined(MCU_SAMD51)
    {{&machine_dac_type}, 0, PIN_PA02},
    {{&machine_dac_type}, 1, PIN_PA05},
    #endif
};
Dac *const dac_bases[] = DAC_INSTS;

#if defined(MCU_SAMD21)

#define MAX_DAC_VALUE       (1023)
#define DEFAULT_DAC_VREF    (1)
#define MAX_DAC_VREF        (2)

#elif defined(MCU_SAMD51)

// According to Errata 2.9.2, VDDANA as ref value is not available. However it worked
// in tests. So I keep the selection here but set the default to Aref, which is usually
// connected at the Board to VDDANA
static uint8_t dac_vref_table[] = {
    DAC_CTRLB_REFSEL_INTREF_Val, DAC_CTRLB_REFSEL_VDDANA_Val,
    DAC_CTRLB_REFSEL_VREFPU_Val, DAC_CTRLB_REFSEL_VREFPB_Val
};
#define MAX_DAC_VALUE       (4095)
#define DEFAULT_DAC_VREF    (2)
#define MAX_DAC_VREF        (3)
static bool dac_init[2] = {false, false};

#endif


static mp_obj_t dac_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw,
    const mp_obj_t *all_args) {

    enum { ARG_id, ARG_vref };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_vref,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_DAC_VREF} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t id = args[ARG_id].u_int;
    dac_obj_t *self = NULL;
    if (0 <= id && id < MP_ARRAY_SIZE(dac_obj)) {
        self = &dac_obj[id];
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid id for DAC"));
    }

    uint8_t vref = args[ARG_vref].u_int;
    if (0 <= vref && vref <= MAX_DAC_VREF) {
        self->vref = vref;
    }

    Dac *dac = dac_bases[0]; // Just one DAC register block

    // initialize DAC

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
    dac->CTRLB.reg = DAC_CTRLB_EOEN | DAC_CTRLB_REFSEL(self->vref);
    // Enable DAC and wait to be ready
    dac->CTRLA.bit.ENABLE = 1;
    while (dac->STATUS.bit.SYNCBUSY) {
    }

    #elif defined(MCU_SAMD51)

    // Configuration SAMD51
    // Enable APBD clocks and PCHCTRL clocks; GCLK3 at 8 MHz

    if (!(dac_init[0] | dac_init[1])) {
        MCLK->APBDMASK.reg |= MCLK_APBDMASK_DAC;
        GCLK->PCHCTRL[DAC_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK3 | \
            GCLK_PCHCTRL_CHEN;

        // Reset DAC registers
        dac->CTRLA.bit.SWRST = 1;
        while (dac->CTRLA.bit.SWRST) {
        }
        dac->CTRLB.reg = DAC_CTRLB_REFSEL(dac_vref_table[self->vref]);

    }

    // Modify DAC config - requires disabling see Section 47.6.2.3 of data sheet
    if (!dac_init[self->id]) {
        // Disable DAC and wait
        dac->CTRLA.bit.ENABLE = 0;
        while (dac->SYNCBUSY.bit.ENABLE) {
        }

        // Modify configuration
        dac->DACCTRL[self->id].reg = DAC_DACCTRL_ENABLE | \
            DAC_DACCTRL_REFRESH(2) | DAC_DACCTRL_CCTRL_CC12M;
        dac->DATA[self->id].reg = 0;
        dac_init[self->id] = true;

        // Enable DAC and wait
        dac->CTRLA.bit.ENABLE = 1;
        while (dac->SYNCBUSY.bit.ENABLE) {
        }
    }

    #endif

    // Set the port as given in self->gpio_id as DAC
    mp_hal_set_pin_mux(self->gpio_id, ALT_FCT_DAC);

    return MP_OBJ_FROM_PTR(self);
}

static void dac_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    dac_obj_t *self = self_in;
    mp_printf(print, "DAC(%u, Pin=%q, vref=%d)", self->id, pin_find_by_id(self->gpio_id)->name, self->vref);
}

static mp_obj_t dac_write(mp_obj_t self_in, mp_obj_t value_in) {
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

static const mp_rom_map_elem_t dac_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&dac_write_obj) },
};

static MP_DEFINE_CONST_DICT(dac_locals_dict, dac_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_dac_type,
    MP_QSTR_DAC,
    MP_TYPE_FLAG_NONE,
    make_new, dac_make_new,
    print, dac_print,
    locals_dict, &dac_locals_dict
    );

#endif
