/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_adc.c via MICROPY_PY_MACHINE_ADC_INCLUDEFILE.

#include "py/mphal.h"

#include "analog_config.h"
#include "adc.h"
#include "sys_ctrl_adc.h"

#define ADC_CHANNEL_TEMP_SENSOR (6)
#define ADC_CHANNEL_INT_VREF (7)

static const uint8_t adc_instance_table[4] = {
    ADC_INSTANCE_ADC12_0,
    ADC_INSTANCE_ADC12_1,
    ADC_INSTANCE_ADC12_2,
    ADC_INSTANCE_ADC24_0,
};

static ADC_Type *const adc_regs_table[4] = {
    (ADC_Type *)ADC120_BASE,
    (ADC_Type *)ADC121_BASE,
    (ADC_Type *)ADC122_BASE,
    (ADC_Type *)ADC24_BASE,
};

static bool adc_inited[4];
static conv_info_t adc_conv_info_table[4];

static void adc_nvic_config(unsigned int irq) {
    NVIC_ClearPendingIRQ(irq);
    NVIC_SetPriority(irq, IRQ_PRI_ADC);
    NVIC_EnableIRQ(irq);
}

static void adc_init(uint8_t adc_periph) {
    if (adc_inited[adc_periph]) {
        return;
    }

    ADC_Type *regs = adc_regs_table[adc_periph];
    uint8_t adc_instance = adc_instance_table[adc_periph];
    conv_info_t *conv_info = &adc_conv_info_table[adc_periph];

    adc_set_clk_control(adc_instance, true);
    enable_cmp_periph_clk();
    analog_config_vbat_reg2();
    analog_config_cmp_reg2();
    adc_set_differential_ctrl(adc_instance, false);
    adc_set_comparator_ctrl(adc_instance, true, 2);

    if (adc_instance == ADC_INSTANCE_ADC24_0) {
        enable_adc24();
        set_adc24_output_rate(0);
        set_adc24_bias(0);
    } else {
        adc_set_sample_width(regs, 32);
    }

    adc_set_clk_div(regs, 4);
    adc_set_avg_sample(regs, 32);
    adc_set_n_shift_bit(regs, 1, 1);
    adc_set_single_ch_scan_mode(regs, conv_info);
    adc_unmask_interrupt(regs);

    adc_nvic_config(ADC120_DONE0_IRQ_IRQn);
    adc_nvic_config(ADC120_DONE1_IRQ_IRQn);
    adc_nvic_config(ADC121_DONE0_IRQ_IRQn);
    adc_nvic_config(ADC121_DONE1_IRQ_IRQn);
    adc_nvic_config(ADC122_DONE0_IRQ_IRQn);
    adc_nvic_config(ADC122_DONE1_IRQ_IRQn);
}

static uint16_t adc_config_and_read_u16(unsigned int adc_periph, uint32_t channel) {
    ADC_Type *adc_regs = adc_regs_table[adc_periph];
    conv_info_t *conv_info = &adc_conv_info_table[adc_periph];

    conv_info->status = ADC_CONV_STAT_NONE;
    conv_info->mode = ADC_CONV_MODE_SINGLE_SHOT;

    // Select channel and start conversion.
    adc_init_channel_select(adc_regs, channel);
    adc_set_single_ch_scan_mode(adc_regs, conv_info);
    adc_enable_single_shot_conv(adc_regs);

    // Wait for conversion to complete.
    while (!(conv_info->status & ADC_CONV_STAT_COMPLETE)) {
        __WFE();
    }

    return conv_info->sampled_value;
}

static void adc_done0_irq_handler_helper(unsigned int adc_periph) {
    adc_done0_irq_handler(adc_regs_table[adc_periph], &adc_conv_info_table[adc_periph]);
}

static void adc_done1_irq_handler_helper(unsigned int adc_periph) {
    adc_done1_irq_handler(adc_regs_table[adc_periph], &adc_conv_info_table[adc_periph]);
}

void ADC120_DONE0_IRQHandler(void) {
    adc_done0_irq_handler_helper(0);
    __SEV();
}

void ADC120_DONE1_IRQHandler(void) {
    adc_done1_irq_handler_helper(0);
    __SEV();
}

void ADC121_DONE0_IRQHandler(void) {
    adc_done0_irq_handler_helper(1);
    __SEV();
}

void ADC121_DONE1_IRQHandler(void) {
    adc_done1_irq_handler_helper(1);
    __SEV();
}

void ADC122_DONE0_IRQHandler(void) {
    adc_done0_irq_handler_helper(2);
    __SEV();
}

void ADC122_DONE1_IRQHandler(void) {
    adc_done1_irq_handler_helper(2);
    __SEV();
}

/******************************************************************************/
// MicroPython bindings for machine.ADC

#define MICROPY_PY_MACHINE_ADC_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_CORE_TEMP), MP_ROM_INT(ADC_CHANNEL_TEMP_SENSOR) }, \
    { MP_ROM_QSTR(MP_QSTR_CORE_VREF), MP_ROM_INT(ADC_CHANNEL_INT_VREF) }, \

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    uint8_t adc_periph;
    uint8_t adc_channel;
} machine_adc_obj_t;

static void mp_machine_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<ADC12%u channel=%u>", self->adc_periph, self->adc_channel);
}

// ADC(id)
static mp_obj_t mp_machine_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_obj_t source = all_args[0];

    uint8_t adc_periph;
    uint8_t adc_channel;
    const machine_pin_obj_t *pin = NULL;

    if (mp_obj_is_int(source)) {
        // Get and validate channel number.
        adc_channel = mp_obj_get_int(source);
        if (adc_channel < ADC_CHANNEL_TEMP_SENSOR) {
            adc_periph = 0;
        } else if (adc_channel == ADC_CHANNEL_TEMP_SENSOR || adc_channel == ADC_CHANNEL_INT_VREF) {
            adc_periph = 2;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid channel"));
        }
    } else {
        // Get GPIO and check it has ADC capabilities.
        pin = mp_hal_get_pin_obj(source);
        if (pin->adc12_periph <= 2 && pin->adc12_channel <= 5) {
            // Select the ADC12 peripheral and channel.
            adc_periph = pin->adc12_periph;
            adc_channel = pin->adc12_channel;
            // Configure the pad for analog input.
            pinconf_set(pin->port, pin->pin, PINMUX_ALTERNATE_FUNCTION_7, PADCTRL_READ_ENABLE);
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("Pin doesn't have ADC capabilities"));
        }
    }

    // Initialise the ADC peripheral.
    adc_init(adc_periph);

    // Create ADC object.
    machine_adc_obj_t *o = mp_obj_malloc(machine_adc_obj_t, &machine_adc_type);
    o->adc_periph = adc_periph;
    o->adc_channel = adc_channel;

    return MP_OBJ_FROM_PTR(o);
}

// read_u16()
static mp_int_t mp_machine_adc_read_u16(machine_adc_obj_t *self) {
    return adc_config_and_read_u16(self->adc_periph, self->adc_channel);
}
