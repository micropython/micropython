/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Glenn Ruben Bakke
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
#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"

#if MICROPY_PY_MACHINE_ADC

#include "adc.h"

#if NRF51
    #include "nrfx_adc.h"
#else
    #include "nrfx_saadc.h"
#endif

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    uint8_t       id;
#if NRF51
    uint8_t       ain;
#endif
} machine_adc_obj_t;

STATIC const machine_adc_obj_t machine_adc_obj[] = {
#if NRF51
    {{&machine_adc_type}, .id = 0, .ain = NRF_ADC_CONFIG_INPUT_0},
    {{&machine_adc_type}, .id = 1, .ain = NRF_ADC_CONFIG_INPUT_1},
    {{&machine_adc_type}, .id = 2, .ain = NRF_ADC_CONFIG_INPUT_2},
    {{&machine_adc_type}, .id = 3, .ain = NRF_ADC_CONFIG_INPUT_3},
    {{&machine_adc_type}, .id = 4, .ain = NRF_ADC_CONFIG_INPUT_4},
    {{&machine_adc_type}, .id = 5, .ain = NRF_ADC_CONFIG_INPUT_5},
    {{&machine_adc_type}, .id = 6, .ain = NRF_ADC_CONFIG_INPUT_6},
    {{&machine_adc_type}, .id = 7, .ain = NRF_ADC_CONFIG_INPUT_7},
#else
    {{&machine_adc_type}, .id = 0},
    {{&machine_adc_type}, .id = 1},
    {{&machine_adc_type}, .id = 2},
    {{&machine_adc_type}, .id = 3},
    {{&machine_adc_type}, .id = 4},
    {{&machine_adc_type}, .id = 5},
    {{&machine_adc_type}, .id = 6},
    {{&machine_adc_type}, .id = 7},
#endif
};

#if defined(NRF52_SERIES)
STATIC void saadc_event_handler(nrfx_saadc_evt_t const * p_event) {
    (void)p_event;
}
#endif

void adc_init0(void) {
#if defined(NRF52_SERIES)
    const nrfx_saadc_config_t config = {
        .resolution         = NRF_SAADC_RESOLUTION_8BIT,
        .oversample         = NRF_SAADC_OVERSAMPLE_DISABLED,
        .interrupt_priority = 6,
        .low_power_mode     = false
    };

    nrfx_saadc_init(&config, saadc_event_handler);
#endif
}

STATIC int adc_find(mp_obj_t id) {
    // given an integer id
    int adc_id = mp_obj_get_int(id);

    int adc_idx = adc_id;

    if (adc_idx >= 0 && adc_idx <= MP_ARRAY_SIZE(machine_adc_obj)
        && machine_adc_obj[adc_idx].id != (uint8_t)-1) {
        return adc_idx;
    }
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
        "ADC(%d) does not exist", adc_id));
}

/// \method __str__()
/// Return a string describing the ADC object.
STATIC void machine_adc_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    machine_adc_obj_t *self = o;

    mp_printf(print, "ADC(%u)", self->id);
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

// for make_new
STATIC mp_obj_t machine_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(-1) } },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int adc_id = adc_find(args[ARG_id].u_obj);
    const machine_adc_obj_t *self = &machine_adc_obj[adc_id];

#if defined(NRF52_SERIES)
    const nrf_saadc_channel_config_t config = {
        .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
        .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
        .gain       = NRF_SAADC_GAIN1_4,
        .reference  = NRF_SAADC_REFERENCE_VDD4,
        .acq_time   = NRF_SAADC_ACQTIME_3US,
        .mode       = NRF_SAADC_MODE_SINGLE_ENDED,
        .burst      = NRF_SAADC_BURST_DISABLED,
        .pin_p      = self->id, // 0 - 7
        .pin_n      = NRF_SAADC_INPUT_DISABLED
    };

    nrfx_saadc_channel_init(self->id, &config);
#endif

    return MP_OBJ_FROM_PTR(self);
}

int16_t machine_adc_value_read(machine_adc_obj_t * adc_obj) {

#if NRF51
    nrf_adc_value_t value = 0;

    nrfx_adc_channel_t channel_config = {
        .config.resolution = NRF_ADC_CONFIG_RES_8BIT,
        .config.input      = NRF_ADC_CONFIG_SCALING_INPUT_TWO_THIRDS,
        .config.reference  = NRF_ADC_CONFIG_REF_VBG,
        .config.input      = adc_obj->ain,
        .config.extref     = ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos // Currently not defined in nrfx/hal.
    };

    nrfx_adc_sample_convert(&channel_config, &value);
#else // NRF52
    nrf_saadc_value_t value = 0;

    nrfx_saadc_sample_convert(adc_obj->id, &value);
#endif
    return value;
}


/// \method value()
/// Read adc level.
mp_obj_t machine_adc_value(mp_obj_t self_in) {
    machine_adc_obj_t *self = self_in;

    int16_t value = machine_adc_value_read(self);

    return MP_OBJ_NEW_SMALL_INT(value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_adc_value_obj, machine_adc_value);

#if NRF51

#define ADC_REF_VOLTAGE_IN_MILLIVOLT (1200) // Reference voltage in mV (1.2V).
#define ADC_PRE_SCALING_MULTIPLIER   (3)    // VDD 1/3 prescaling as input. Hence, multiplied by 3 to get the value of the battery voltage.

#else // NRF52

#define ADC_REF_VOLTAGE_IN_MILLIVOLT (600)  // Reference voltage in mV (0.6V).
#define ADC_PRE_SCALING_MULTIPLIER   (6)    // VDD 1/6 prescaling as input. Hence, multiplied by 6 to get the value of the battery voltage.

#endif

#define DIODE_VOLT_DROP_MILLIVOLT    (270)  // Voltage drop over diode.

#define BATTERY_MILLIVOLT(VALUE) \
        ((((VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLT) / 255) * ADC_PRE_SCALING_MULTIPLIER)

static uint8_t battery_level_in_percent(const uint16_t mvolts)
{
    uint8_t battery_level;

    if (mvolts >= 3000) {
        battery_level = 100;
    } else if (mvolts > 2900) {
        battery_level = 100 - ((3000 - mvolts) * 58) / 100;
    } else if (mvolts > 2740) {
        battery_level = 42 - ((2900 - mvolts) * 24) / 160;
    } else if (mvolts > 2440) {
        battery_level = 18 - ((2740 - mvolts) * 12) / 300;
    } else if (mvolts > 2100) {
        battery_level = 6 - ((2440 - mvolts) * 6) / 340;
    } else {
        battery_level = 0;
    }

    return battery_level;
}

/// \method battery_level()
/// Get battery level in percentage.
mp_obj_t machine_adc_battery_level(void) {

#if NRF51
    nrf_adc_value_t value = 0;

    nrfx_adc_channel_t channel_config = {
        .config.resolution = NRF_ADC_CONFIG_RES_8BIT,
        .config.input      = NRF_ADC_CONFIG_SCALING_SUPPLY_ONE_THIRD,
        .config.reference  = NRF_ADC_CONFIG_REF_VBG,
        .config.input      = NRF_ADC_CONFIG_INPUT_DISABLED,
        .config.extref     = ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos // Currently not defined in nrfx/hal.
    };

    nrfx_adc_sample_convert(&channel_config, &value);
#else // NRF52
    nrf_saadc_value_t value = 0;

    const nrf_saadc_channel_config_t config = {
        .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
        .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
        .gain       = NRF_SAADC_GAIN1_6,
        .reference  = NRF_SAADC_REFERENCE_INTERNAL,
        .acq_time   = NRF_SAADC_ACQTIME_3US,
        .mode       = NRF_SAADC_MODE_SINGLE_ENDED,
        .burst      = NRF_SAADC_BURST_DISABLED,
        .pin_p      = NRF_SAADC_INPUT_VDD,
        .pin_n      = NRF_SAADC_INPUT_DISABLED
    };

    nrfx_saadc_channel_init(0, &config);
    nrfx_saadc_sample_convert(0, &value);
#endif

    uint16_t batt_lvl_in_milli_volts = BATTERY_MILLIVOLT(value) + DIODE_VOLT_DROP_MILLIVOLT;
    uint16_t batt_in_percent = battery_level_in_percent(batt_lvl_in_milli_volts);

    return MP_OBJ_NEW_SMALL_INT(batt_in_percent);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_machine_adc_battery_level_obj, machine_adc_battery_level);

STATIC const mp_rom_map_elem_t machine_adc_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&mp_machine_adc_value_obj) },

    // class methods
    { MP_ROM_QSTR(MP_QSTR_battery_level), MP_ROM_PTR(&mp_machine_adc_battery_level_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_adc_locals_dict, machine_adc_locals_dict_table);

const mp_obj_type_t machine_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .make_new = machine_adc_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_adc_locals_dict,
    .print = machine_adc_print,
};

#endif // MICROPY_PY_MACHINE_ADC
