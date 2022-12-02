/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Philipp Ebensberger
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

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    adc_config_t adc_config;
    uint8_t id;
    uint8_t avg;
    uint8_t bits;
    uint8_t vref;
} machine_adc_obj_t;

#define DEFAULT_ADC_BITS    12
#define DEFAULT_ADC_AVG     16

#if defined(MCU_SAMD21)
static uint8_t adc_vref_table[] = {
    ADC_REFCTRL_REFSEL_INT1V_Val, ADC_REFCTRL_REFSEL_INTVCC0_Val,
    ADC_REFCTRL_REFSEL_INTVCC1_Val, ADC_REFCTRL_REFSEL_AREFA_Val, ADC_REFCTRL_REFSEL_AREFB_Val
};
#if MICROPY_HW_ADC_VREF
#define DEFAULT_ADC_VREF    MICROPY_HW_ADC_VREF
#else
#define DEFAULT_ADC_VREF    (3)
#endif

#define ADC_EVSYS_CHANNEL    0

#elif defined(MCU_SAMD51)

static uint8_t adc_vref_table[] = {
    ADC_REFCTRL_REFSEL_INTREF_Val, ADC_REFCTRL_REFSEL_INTVCC1_Val,
    ADC_REFCTRL_REFSEL_INTVCC0_Val, ADC_REFCTRL_REFSEL_AREFA_Val,
    ADC_REFCTRL_REFSEL_AREFB_Val, ADC_REFCTRL_REFSEL_AREFC_Val
};
#if MICROPY_HW_ADC_VREF
#define DEFAULT_ADC_VREF    MICROPY_HW_ADC_VREF
#else
#define DEFAULT_ADC_VREF    (3)
#endif

#endif  // defined(MCU_SAMD21)

Adc *const adc_bases[] = ADC_INSTS;
uint32_t busy_flags = 0;
bool init_flags[2] = {false, false};
static void adc_init(machine_adc_obj_t *self);
static uint8_t resolution[] = {
    ADC_CTRLB_RESSEL_8BIT_Val, ADC_CTRLB_RESSEL_10BIT_Val, ADC_CTRLB_RESSEL_12BIT_Val
};

// Calculate the floor value of log2(n)
mp_int_t log2i(mp_int_t num) {
    mp_int_t res = 0;
    for (; num > 1; num >>= 1) {
        res += 1;
    }
    return res;
}

STATIC void adc_obj_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    (void)kind;
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(o);

    mp_printf(print, "ADC(%s, device=%u, channel=%u, bits=%u, average=%u, vref=%d)",
        pin_name(self->id), self->adc_config.device,
        self->adc_config.channel, self->bits, 1 << self->avg, self->vref);
}

STATIC mp_obj_t adc_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_bits, ARG_average, ARG_vref };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_bits,     MP_ARG_INT, {.u_int = DEFAULT_ADC_BITS} },
        { MP_QSTR_average,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_ADC_AVG} },
        { MP_QSTR_vref,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_ADC_VREF} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Unpack and check, whther the pin has ADC capability
    int id = mp_hal_get_pin_obj(args[ARG_id].u_obj);
    adc_config_t adc_config = get_adc_config(id, busy_flags);

    // Now that we have a valid device and channel, create and populate the ADC instance
    machine_adc_obj_t *self = mp_obj_malloc(machine_adc_obj_t, &machine_adc_type);
    self->id = id;
    self->adc_config = adc_config;
    self->bits = DEFAULT_ADC_BITS;
    uint16_t bits = args[ARG_bits].u_int;
    if (bits >= 8 && bits <= 12) {
        self->bits = bits;
    }
    uint32_t avg = log2i(args[ARG_average].u_int);
    self->avg = (avg <= 10 ? avg : 10);

    uint8_t vref = args[ARG_vref].u_int;
    if (0 <= vref && vref < sizeof(adc_vref_table)) {
        self->vref = vref;
    }

    // flag the device/channel as being in use.
    busy_flags |= (1 << (self->adc_config.device * 16 + self->adc_config.channel));
    init_flags[self->adc_config.device] = false;

    adc_init(self);

    return MP_OBJ_FROM_PTR(self);
}

// read_u16()
STATIC mp_obj_t machine_adc_read_u16(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    Adc *adc = adc_bases[self->adc_config.device];
    // Set the reference voltage. Default: external AREFA.
    adc->REFCTRL.reg = adc_vref_table[self->vref];
    // Set Input channel and resolution
    // Select the pin as positive input and gnd as negative input reference, non-diff mode by default
    adc->INPUTCTRL.reg = ADC_INPUTCTRL_MUXNEG_GND | self->adc_config.channel;
    // set resolution. Scale 8-16 to 0 - 4 for table access.
    adc->CTRLB.bit.RESSEL = resolution[(self->bits - 8) / 2];
    // Measure input voltage
    adc->SWTRIG.bit.START = 1;
    while (adc->INTFLAG.bit.RESRDY == 0) {
    }
    // Get and return the result
    return MP_OBJ_NEW_SMALL_INT(adc->RESULT.reg * (65536 / (1 << self->bits)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_u16_obj, machine_adc_read_u16);

// deinit() : release the ADC channel
STATIC mp_obj_t machine_adc_deinit(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);

    busy_flags &= ~((1 << (self->adc_config.device * 16 + self->adc_config.channel)));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_deinit_obj, machine_adc_deinit);

void adc_deinit_all(void) {
    busy_flags = 0;
    init_flags[0] = 0;
    init_flags[1] = 0;
}

STATIC const mp_rom_map_elem_t adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read_u16), MP_ROM_PTR(&machine_adc_read_u16_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_adc_deinit_obj) },
};

STATIC MP_DEFINE_CONST_DICT(adc_locals_dict, adc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_adc_type,
    MP_QSTR_ADC,
    MP_TYPE_FLAG_NONE,
    make_new, adc_obj_make_new,
    print, adc_obj_print,
    locals_dict, &adc_locals_dict
    );

static void adc_init(machine_adc_obj_t *self) {
    // ADC & clock init is done only once per ADC
    if (init_flags[self->adc_config.device] == false) {
        Adc *adc = adc_bases[self->adc_config.device];

        init_flags[self->adc_config.device] = true;

        #if defined(MCU_SAMD21)
        // Configuration SAMD21
        // Enable APBD clocks and PCHCTRL clocks; GCLK2 at 48 MHz
        PM->APBCMASK.reg |= PM_APBCMASK_ADC;
        GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_ID_ADC;
        while (GCLK->STATUS.bit.SYNCBUSY) {
        }
        // Reset ADC registers
        adc->CTRLA.bit.SWRST = 1;
        while (adc->CTRLA.bit.SWRST) {
        }
        // Get the calibration data
        uint32_t bias = (*((uint32_t *)ADC_FUSES_BIASCAL_ADDR) & ADC_FUSES_BIASCAL_Msk) >> ADC_FUSES_BIASCAL_Pos;
        uint32_t linearity = (*((uint32_t *)ADC_FUSES_LINEARITY_0_ADDR) & ADC_FUSES_LINEARITY_0_Msk) >> ADC_FUSES_LINEARITY_0_Pos;
        linearity |= ((*((uint32_t *)ADC_FUSES_LINEARITY_1_ADDR) & ADC_FUSES_LINEARITY_1_Msk) >> ADC_FUSES_LINEARITY_1_Pos) << 5;
        /* Write the calibration data. */
        ADC->CALIB.reg = ADC_CALIB_BIAS_CAL(bias) | ADC_CALIB_LINEARITY_CAL(linearity);
        // Divide 48MHz clock by 32 to obtain 1.5 MHz clock to adc
        adc->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV32;
        // Select external AREFA as reference voltage.
        adc->REFCTRL.reg = adc_vref_table[self->vref];
        // Average: Accumulate samples and scale them down accordingly
        adc->AVGCTRL.reg = self->avg | ADC_AVGCTRL_ADJRES(self->avg);
        // Enable ADC and wait to be ready
        adc->CTRLA.bit.ENABLE = 1;
        while (adc->STATUS.bit.SYNCBUSY) {
        }

        #elif defined(MCU_SAMD51)
        // Configuration SAMD51
        // Enable APBD clocks and PCHCTRL clocks; GCLK2 at 48 MHz
        if (self->adc_config.device == 0) {
            GCLK->PCHCTRL[ADC0_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK2 | GCLK_PCHCTRL_CHEN;
            MCLK->APBDMASK.bit.ADC0_ = 1;
        } else {
            GCLK->PCHCTRL[ADC1_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK2 | GCLK_PCHCTRL_CHEN;
            MCLK->APBDMASK.bit.ADC1_ = 1;
        }
        // Reset ADC registers
        adc->CTRLA.bit.SWRST = 1;
        while (adc->CTRLA.bit.SWRST) {
        }
        // Get the calibration data
        uint32_t biascomp;
        uint32_t biasr2r;
        uint32_t biasrefbuf;
        if (self->adc_config.device == 0) {
            biascomp = (*((uint32_t *)ADC0_FUSES_BIASCOMP_ADDR) & ADC0_FUSES_BIASCOMP_Msk) >> ADC0_FUSES_BIASCOMP_Pos;
            biasr2r = (*((uint32_t *)ADC0_FUSES_BIASR2R_ADDR) & ADC0_FUSES_BIASR2R_Msk) >> ADC0_FUSES_BIASR2R_Pos;
            biasrefbuf = (*((uint32_t *)ADC0_FUSES_BIASREFBUF_ADDR) & ADC0_FUSES_BIASREFBUF_Msk) >> ADC0_FUSES_BIASREFBUF_Pos;
        } else {
            biascomp = (*((uint32_t *)ADC1_FUSES_BIASCOMP_ADDR) & ADC1_FUSES_BIASCOMP_Msk) >> ADC1_FUSES_BIASCOMP_Pos;
            biasr2r = (*((uint32_t *)ADC1_FUSES_BIASR2R_ADDR) & ADC1_FUSES_BIASR2R_Msk) >> ADC1_FUSES_BIASR2R_Pos;
            biasrefbuf = (*((uint32_t *)ADC1_FUSES_BIASREFBUF_ADDR) & ADC1_FUSES_BIASREFBUF_Msk) >> ADC1_FUSES_BIASREFBUF_Pos;
        }
        /* Write the calibration data. */
        adc->CALIB.reg = ADC_CALIB_BIASCOMP(biascomp) | ADC_CALIB_BIASR2R(biasr2r) | ADC_CALIB_BIASREFBUF(biasrefbuf);
        // Divide 48MHz clock by 32 to obtain 1.5 MHz clock to adc
        adc->CTRLA.reg = ADC_CTRLA_PRESCALER_DIV32;
        // Set the reference voltage. Default: external AREFA.
        adc->REFCTRL.reg = adc_vref_table[self->vref];
        // Average: Accumulate samples and scale them down accordingly
        adc->AVGCTRL.reg = self->avg | ADC_AVGCTRL_ADJRES(self->avg);
        // Enable ADC and wait to be ready
        adc->CTRLA.bit.ENABLE = 1;
        while (adc->SYNCBUSY.bit.ENABLE) {
        }

        #endif
    }
    // Set the port as given in self->id as ADC
    mp_hal_set_pin_mux(self->id, ALT_FCT_ADC);
}
