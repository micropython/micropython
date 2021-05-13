/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Noralf Trønnes
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

#include "bindings/samd/Clock.h"
#include "samd/clocks.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

//| class Clock:
//|     """Identifies a clock on the microcontroller.
//|
//|        They are fixed by the hardware so they cannot be constructed on demand. Instead, use
//|        ``samd.clock`` to reference the desired clock."""
//|

STATIC void samd_clock_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    samd_clock_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "%q.%q.%q", MP_QSTR_samd, MP_QSTR_clock, self->name);
}

//|     enabled: bool
//|     """Is the clock enabled? (read-only)"""
//|
STATIC mp_obj_t samd_clock_get_enabled(mp_obj_t self_in) {
    samd_clock_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(clock_get_enabled(self->type, self->index));
}

MP_DEFINE_CONST_FUN_OBJ_1(samd_clock_get_enabled_obj, samd_clock_get_enabled);

const mp_obj_property_t samd_clock_enabled_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&samd_clock_get_enabled_obj,
              MP_ROM_NONE,
              MP_ROM_NONE,},
};

//|     parent: Union[Clock, None]
//|     """Clock parent. (read-only)"""
//|
STATIC mp_obj_t samd_clock_get_parent(mp_obj_t self_in) {
    samd_clock_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t p_type, p_index;
    if (!clock_get_parent(self->type, self->index, &p_type, &p_index)) {
        return mp_const_none;
    }

    const mp_map_t *samd_map = &samd_clock_globals.map;
    for (uint8_t i = 0; i < samd_map->alloc; i++) {
        samd_clock_obj_t *iter = samd_map->table[i].value;
        if (iter->type == p_type && iter->index == p_index) {
            return iter;
        }
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(samd_clock_get_parent_obj, samd_clock_get_parent);

const mp_obj_property_t samd_clock_parent_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&samd_clock_get_parent_obj,
              MP_ROM_NONE,
              MP_ROM_NONE,},
};

//|     frequency: int
//|     """Clock frequency in Herz. (read-only)"""
//|
STATIC mp_obj_t samd_clock_get_frequency(mp_obj_t self_in) {
    samd_clock_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(clock_get_frequency(self->type, self->index));
}

MP_DEFINE_CONST_FUN_OBJ_1(samd_clock_get_frequency_obj, samd_clock_get_frequency);

const mp_obj_property_t samd_clock_frequency_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&samd_clock_get_frequency_obj,
              MP_ROM_NONE,
              MP_ROM_NONE,},
};

//|     calibration: int
//|     """Clock calibration. Not all clocks can be calibrated."""
//|
STATIC mp_obj_t samd_clock_get_calibration(mp_obj_t self_in) {
    samd_clock_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(clock_get_calibration(self->type, self->index));
}

MP_DEFINE_CONST_FUN_OBJ_1(samd_clock_get_calibration_obj, samd_clock_get_calibration);

STATIC mp_obj_t samd_clock_set_calibration(mp_obj_t self_in, mp_obj_t calibration) {
    samd_clock_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret = clock_set_calibration(self->type, self->index, mp_obj_get_int(calibration));
    if (ret == -2) {
        mp_raise_AttributeError(translate("calibration is read only"));
    }
    if (ret == -1) {
        mp_raise_ValueError(translate("calibration is out of range"));
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(samd_clock_set_calibration_obj, samd_clock_set_calibration);

const mp_obj_property_t samd_clock_calibration_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&samd_clock_get_calibration_obj,
              (mp_obj_t)&samd_clock_set_calibration_obj,
              MP_ROM_NONE,},
};

STATIC const mp_rom_map_elem_t samd_clock_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_enabled), MP_ROM_PTR(&samd_clock_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_parent), MP_ROM_PTR(&samd_clock_parent_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&samd_clock_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_calibration), MP_ROM_PTR(&samd_clock_calibration_obj) },
};

STATIC MP_DEFINE_CONST_DICT(samd_clock_locals_dict, samd_clock_locals_dict_table);

const mp_obj_type_t samd_clock_type = {
    { &mp_type_type },
    .name = MP_QSTR_Clock,
    .print = samd_clock_print,
    .locals_dict = (mp_obj_t)&samd_clock_locals_dict,
};

#ifdef SAMD21

#ifdef SAMD21_EXPOSE_ALL_CLOCKS
CLOCK_SOURCE(XOSC);
CLOCK_SOURCE(GCLKIN);
CLOCK_SOURCE(GCLKGEN1);
CLOCK_SOURCE(OSCULP32K);
#endif
CLOCK_SOURCE(OSC32K);
CLOCK_SOURCE(XOSC32K);
#ifdef SAMD21_EXPOSE_ALL_CLOCKS
CLOCK_SOURCE(OSC8M);
CLOCK_SOURCE(DFLL48M);
CLOCK_SOURCE(DPLL96M);

CLOCK_GCLK_(SYSCTRL, DFLL48);
CLOCK_GCLK_(SYSCTRL, FDPLL);
CLOCK_GCLK_(SYSCTRL, FDPLL32K);
CLOCK_GCLK(WDT);
#endif
CLOCK_GCLK(RTC);
#ifdef SAMD21_EXPOSE_ALL_CLOCKS
CLOCK_GCLK(EIC);
CLOCK_GCLK(USB);
CLOCK_GCLK_(EVSYS, 0);
CLOCK_GCLK_(EVSYS, 1);
CLOCK_GCLK_(EVSYS, 2);
CLOCK_GCLK_(EVSYS, 3);
CLOCK_GCLK_(EVSYS, 4);
CLOCK_GCLK_(EVSYS, 5);
CLOCK_GCLK_(EVSYS, 6);
CLOCK_GCLK_(EVSYS, 7);
CLOCK_GCLK_(EVSYS, 8);
CLOCK_GCLK_(EVSYS, 9);
CLOCK_GCLK_(EVSYS, 10);
CLOCK_GCLK_(EVSYS, 11);
CLOCK(SERCOMx_SLOW, 1, 19);
CLOCK_GCLK_(SERCOM0, CORE);
CLOCK_GCLK_(SERCOM1, CORE);
CLOCK_GCLK_(SERCOM2, CORE);
CLOCK_GCLK_(SERCOM3, CORE);
CLOCK_GCLK_(SERCOM4, CORE);
CLOCK_GCLK_(SERCOM5, CORE);
CLOCK(TCC0_TCC1, 1, 26);
CLOCK(TCC2_TCC3, 1, 27);
CLOCK(TC4_TC5, 1, 28);
CLOCK(TC6_TC7, 1, 29);
CLOCK_GCLK(ADC);
CLOCK_GCLK_(AC, DIG);
CLOCK_GCLK_(AC, ANA);
CLOCK_GCLK(DAC);
CLOCK_GCLK(PTC);
CLOCK_GCLK_(I2S, 0);
CLOCK_GCLK_(I2S, 1);

CLOCK(SYSTICK, 2, 0);
#endif

STATIC const mp_rom_map_elem_t samd_clock_global_dict_table[] = {
    #ifdef SAMD21_EXPOSE_ALL_CLOCKS
    CLOCK_ENTRY(XOSC),
    CLOCK_ENTRY(GCLKIN),
    CLOCK_ENTRY(GCLKGEN1),
    CLOCK_ENTRY(OSCULP32K),
    #endif
    CLOCK_ENTRY(OSC32K),
    CLOCK_ENTRY(XOSC32K),
    #ifdef SAMD21_EXPOSE_ALL_CLOCKS
    CLOCK_ENTRY(OSC8M),
    CLOCK_ENTRY(DFLL48M),
    CLOCK_ENTRY(DPLL96M),
    CLOCK_ENTRY_(SYSCTRL, DFLL48),
    CLOCK_ENTRY_(SYSCTRL, FDPLL),
    CLOCK_ENTRY_(SYSCTRL, FDPLL32K),
    CLOCK_ENTRY(WDT),
    #endif
    CLOCK_ENTRY(RTC),
    #ifdef SAMD21_EXPOSE_ALL_CLOCKS
    CLOCK_ENTRY(EIC),
    CLOCK_ENTRY(USB),
    CLOCK_ENTRY_(EVSYS, 0),
    CLOCK_ENTRY_(EVSYS, 1),
    CLOCK_ENTRY_(EVSYS, 2),
    CLOCK_ENTRY_(EVSYS, 3),
    CLOCK_ENTRY_(EVSYS, 4),
    CLOCK_ENTRY_(EVSYS, 5),
    CLOCK_ENTRY_(EVSYS, 6),
    CLOCK_ENTRY_(EVSYS, 7),
    CLOCK_ENTRY_(EVSYS, 8),
    CLOCK_ENTRY_(EVSYS, 9),
    CLOCK_ENTRY_(EVSYS, 10),
    CLOCK_ENTRY_(EVSYS, 11),
    CLOCK_ENTRY(SERCOMx_SLOW),
    CLOCK_ENTRY_(SERCOM0, CORE),
    CLOCK_ENTRY_(SERCOM1, CORE),
    CLOCK_ENTRY_(SERCOM2, CORE),
    CLOCK_ENTRY_(SERCOM3, CORE),
    CLOCK_ENTRY_(SERCOM4, CORE),
    CLOCK_ENTRY_(SERCOM5, CORE),
    CLOCK_ENTRY(TCC0_TCC1),
    CLOCK_ENTRY(TCC2_TCC3),
    CLOCK_ENTRY(TC4_TC5),
    CLOCK_ENTRY(TC6_TC7),
    CLOCK_ENTRY(ADC),
    CLOCK_ENTRY_(AC, DIG),
    CLOCK_ENTRY_(AC, ANA),
    CLOCK_ENTRY(DAC),
    CLOCK_ENTRY(PTC),
    CLOCK_ENTRY_(I2S, 0),
    CLOCK_ENTRY_(I2S, 1),

    CLOCK_ENTRY(SYSTICK),
    #endif
};
MP_DEFINE_CONST_DICT(samd_clock_globals, samd_clock_global_dict_table);

#endif // SAMD21

#ifdef SAM_D5X_E5X



#include <instance/can0.h>
#include <instance/can1.h>
#include <instance/i2s.h>
#include <instance/sdhc1.h>
#include <instance/sercom6.h>
#include <instance/sercom7.h>
#include <instance/tcc4.h>

CLOCK_SOURCE(XOSC0);
CLOCK_SOURCE(XOSC1);
CLOCK_SOURCE(GCLKIN);
CLOCK_SOURCE(GCLKGEN1);
CLOCK_SOURCE(OSCULP32K);
CLOCK_SOURCE(XOSC32K);
CLOCK_SOURCE(DFLL);
CLOCK_SOURCE(DPLL0);
CLOCK_SOURCE(DPLL1);

CLOCK_GCLK_(OSCCTRL, DFLL48);
CLOCK_GCLK_(OSCCTRL, FDPLL0);
CLOCK_GCLK_(OSCCTRL, FDPLL1);
CLOCK_GCLK_(OSCCTRL, FDPLL032K); // GCLK_OSCCTRL_FDPLL1_32K, GCLK_SDHC0_SLOW, GCLK_SDHC1_SLOW, GCLK_SERCOM[0..7]_SLOW
CLOCK_GCLK(EIC);
CLOCK_GCLK_(FREQM, MSR);
// 6: GCLK_FREQM_REF
CLOCK_GCLK_(SERCOM0, CORE);
CLOCK_GCLK_(SERCOM1, CORE);
CLOCK(TC0_TC1, 1, 9);
CLOCK_GCLK(USB);
CLOCK_GCLK_(EVSYS, 0);
CLOCK_GCLK_(EVSYS, 1);
CLOCK_GCLK_(EVSYS, 2);
CLOCK_GCLK_(EVSYS, 3);
CLOCK_GCLK_(EVSYS, 4);
CLOCK_GCLK_(EVSYS, 5);
CLOCK_GCLK_(EVSYS, 6);
CLOCK_GCLK_(EVSYS, 7);
CLOCK_GCLK_(EVSYS, 8);
CLOCK_GCLK_(EVSYS, 9);
CLOCK_GCLK_(EVSYS, 10);
CLOCK_GCLK_(EVSYS, 11);
CLOCK_GCLK_(SERCOM2, CORE);
CLOCK_GCLK_(SERCOM3, CORE);
CLOCK(TCC0_TCC1, 1, 25);
CLOCK(TC2_TC3, 1, 26);
CLOCK_GCLK(CAN0);
CLOCK_GCLK(CAN1);
CLOCK(TCC2_TCC3, 1, 29);
CLOCK(TC4_TC5, 1, 30);
// CLOCK_GCLK(PDEC);
// CLOCK_GCLK(AC);
// CLOCK_GCLK(CCL);
CLOCK_GCLK_(SERCOM4, CORE);
CLOCK_GCLK_(SERCOM5, CORE);
CLOCK_GCLK_(SERCOM6, CORE);
CLOCK_GCLK_(SERCOM7, CORE);
CLOCK_GCLK(TCC4);
CLOCK(TC6_TC7, 1, 39);
CLOCK_GCLK(ADC0);
CLOCK_GCLK(ADC1);
CLOCK_GCLK(DAC);
CLOCK_GCLK_(I2S, 0);
CLOCK_GCLK_(I2S, 1);
// CLOCK_GCLK(SDHC0);
// CLOCK_GCLK(SDHC1);
// 47: GCLK_CM4_TRACE

CLOCK(SYSTICK, 2, 0);
CLOCK(CPU, 2, 1);
CLOCK(RTC, 2, 2);


STATIC const mp_rom_map_elem_t samd_clock_global_dict_table[] = {
    CLOCK_ENTRY(XOSC0),
    CLOCK_ENTRY(XOSC1),
    CLOCK_ENTRY(GCLKIN),
    CLOCK_ENTRY(GCLKGEN1),
    CLOCK_ENTRY(OSCULP32K),
    CLOCK_ENTRY(XOSC32K),
    CLOCK_ENTRY(DFLL),
    CLOCK_ENTRY(DPLL0),
    CLOCK_ENTRY(DPLL1),

    CLOCK_ENTRY_(OSCCTRL, DFLL48),
    CLOCK_ENTRY_(OSCCTRL, FDPLL0),
    CLOCK_ENTRY_(OSCCTRL, FDPLL1),
    CLOCK_ENTRY_(OSCCTRL, FDPLL032K),
    CLOCK_ENTRY(EIC),
    CLOCK_ENTRY_(FREQM, MSR),
    CLOCK_ENTRY_(SERCOM0, CORE),
    CLOCK_ENTRY_(SERCOM1, CORE),
    CLOCK_ENTRY(TC0_TC1),
    CLOCK_ENTRY(USB),
    CLOCK_ENTRY_(EVSYS, 0),
    CLOCK_ENTRY_(EVSYS, 1),
    CLOCK_ENTRY_(EVSYS, 2),
    CLOCK_ENTRY_(EVSYS, 3),
    CLOCK_ENTRY_(EVSYS, 4),
    CLOCK_ENTRY_(EVSYS, 5),
    CLOCK_ENTRY_(EVSYS, 6),
    CLOCK_ENTRY_(EVSYS, 7),
    CLOCK_ENTRY_(EVSYS, 8),
    CLOCK_ENTRY_(EVSYS, 9),
    CLOCK_ENTRY_(EVSYS, 10),
    CLOCK_ENTRY_(EVSYS, 11),
    CLOCK_ENTRY_(SERCOM2, CORE),
    CLOCK_ENTRY_(SERCOM3, CORE),
    CLOCK_ENTRY(TCC0_TCC1),
    CLOCK_ENTRY(TC2_TC3),
    CLOCK_ENTRY(CAN0),
    CLOCK_ENTRY(CAN1),
    CLOCK_ENTRY(TCC2_TCC3),
    CLOCK_ENTRY(TC4_TC5),
    // CLOCK_ENTRY(PDEC),
    // CLOCK_ENTRY(AC),
    // CLOCK_ENTRY(CCL),
    CLOCK_ENTRY_(SERCOM4, CORE),
    CLOCK_ENTRY_(SERCOM5, CORE),
    CLOCK_ENTRY_(SERCOM6, CORE),
    CLOCK_ENTRY_(SERCOM7, CORE),
    CLOCK_ENTRY(TCC4),
    CLOCK_ENTRY(TC6_TC7),
    CLOCK_ENTRY(ADC0),
    CLOCK_ENTRY(ADC1),
    CLOCK_ENTRY(DAC),
    CLOCK_ENTRY_(I2S, 0),
    CLOCK_ENTRY_(I2S, 1),
    // CLOCK_ENTRY(SDHC0),
    // CLOCK_ENTRY(SDHC1),

    CLOCK_ENTRY(SYSTICK),
    CLOCK_ENTRY(CPU),
    CLOCK_ENTRY(RTC),
};
MP_DEFINE_CONST_DICT(samd_clock_globals, samd_clock_global_dict_table);

#endif // SAMD51
