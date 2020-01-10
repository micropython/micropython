/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nick Moore
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

#include "esp_log.h"

#include "soc/sens_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc.h"

#include "driver/gpio.h"
#include "driver/dac.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"
#include "mphalport.h"

typedef struct _mdac_obj_t {
    mp_obj_base_t base;
    gpio_num_t gpio_id;
    dac_channel_t dac_id;
    int clk_8m_div;
    int frequency_step;
} mdac_obj_t;

STATIC mp_obj_t mdac_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw,
        const mp_obj_t *args) {

    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    gpio_num_t pin_id = machine_pin_get_id(args[0]);
    if (pin_id != GPIO_NUM_25 && pin_id != GPIO_NUM_26) mp_raise_ValueError("invalid Pin for DAC");

    mdac_obj_t *self = m_new_obj(mdac_obj_t);
    self->base.type = &machine_dac_type;

    self->gpio_id = pin_id;
    if (pin_id == GPIO_NUM_25) {
        self->dac_id = DAC_CHANNEL_1;
    } else {
        self->dac_id = DAC_CHANNEL_2;
    }
    self->clk_8m_div = 0;
    self->frequency_step = 1;

    esp_err_t err = dac_output_enable(self->dac_id);
    if (err == ESP_OK) {
        err = dac_output_voltage(self->dac_id, 0);
    }
    if (err == ESP_OK) return MP_OBJ_FROM_PTR(self);
    mp_raise_ValueError("Parameter Error");
}

STATIC void mdac_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mdac_obj_t *self = self_in;
    mp_printf(print, "DAC(Pin(%u))", self->gpio_id);
}

STATIC mp_obj_t mdac_write(mp_obj_t self_in, mp_obj_t value_in) {
    mdac_obj_t *self = self_in;
    int value = mp_obj_get_int(value_in);
    if (value < 0 || value > 255) mp_raise_ValueError("Value out of range");

    esp_err_t err = dac_output_voltage(self->dac_id, value);
    if (err == ESP_OK) return mp_const_none;
    mp_raise_ValueError("Parameter Error");
}
MP_DEFINE_CONST_FUN_OBJ_2(mdac_write_obj, mdac_write);

/*
 * Enable cosine waveform generator on a DAC channel
 */
STATIC mp_obj_t mdac_cosine_enable(mp_obj_t self_in) {
    mdac_obj_t *self = self_in;
    // Enable tone generator common to both channels
    SET_PERI_REG_MASK(SENS_SAR_DAC_CTRL1_REG, SENS_SW_TONE_EN);
    switch(self->dac_id) {
        case DAC_CHANNEL_1:
            // Enable / connect tone tone generator on / to this channel
            SET_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN1_M);
            // Invert MSB, otherwise part of waveform will have inverted
            SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_INV1, 2, SENS_DAC_INV1_S);
            break;
        case DAC_CHANNEL_2:
            SET_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN2_M);
            SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_INV2, 2, SENS_DAC_INV2_S);
            break;
        default :
            mp_raise_ValueError("Parameter Error");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mdac_cosine_enable_obj, mdac_cosine_enable);

/*
 * Disable cosine waveform generator on a DAC channel
 */
STATIC mp_obj_t mdac_cosine_disable(mp_obj_t self_in) {
    mdac_obj_t *self = self_in;
    switch(self->dac_id) {
        case DAC_CHANNEL_1:
            // disable / connect tone tone generator on / to this channel
            CLEAR_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN1_M);
            break;
        case DAC_CHANNEL_2:
            CLEAR_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN2_M);
            break;
        default :
            mp_raise_ValueError("Parameter Error");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mdac_cosine_disable_obj, mdac_cosine_disable);

/*
 * Set frequency steps of internal CW generator common to both DAC channels
 *
 * frequency_step: range 0x0001 - 0xFFFF
 *
 */
STATIC mp_obj_t mdac_frequency_step(mp_obj_t self_in, mp_obj_t frequency_step_in) {
    mdac_obj_t *self = self_in;
    int frequency_step = mp_obj_get_int(frequency_step_in);
    if (frequency_step < 1 || frequency_step > 0xffff) mp_raise_ValueError("Frequency_step out of range");
    self->frequency_step = frequency_step;
    SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL1_REG, SENS_SW_FSTEP, frequency_step, SENS_SW_FSTEP_S);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mdac_frequency_step_obj, mdac_frequency_step);

/*
 * Set the RTC 8 MHz clock divider. This can be dangerous if other code uses the 8 MHz clock.
 * 
 * clk_8m_div: range 0b000 - 0b111
 */
STATIC mp_obj_t mdac_rtc_clk_div(mp_obj_t self_in, mp_obj_t clk_8m_div_in) {
    mdac_obj_t *self = self_in;
    int clk_8m_div = mp_obj_get_int(clk_8m_div_in);
    if (clk_8m_div < 0 || clk_8m_div > 0b111) mp_raise_ValueError("Cl_8m_div out of range");
    self->clk_8m_div = clk_8m_div;
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL, clk_8m_div);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mdac_rtc_clk_div_obj, mdac_rtc_clk_div);
    
/*
 * Scale output of a DAC channel using two bit pattern:
 *
 * - 00: no scale
 * - 01: scale to 1/2
 * - 10: scale to 1/4
 * - 11: scale to 1/8
 *
 */
STATIC mp_obj_t mdac_scale_set(mp_obj_t self_in, mp_obj_t scale_in) {
    mdac_obj_t *self = self_in;
    int scale = mp_obj_get_int(scale_in);
    if (scale < 0 || scale > 0b11) mp_raise_ValueError("Scale out of range");
    switch(self->dac_id) {
        case DAC_CHANNEL_1:
            SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_SCALE1, scale, SENS_DAC_SCALE1_S);
            break;
        case DAC_CHANNEL_2:
            SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_SCALE2, scale, SENS_DAC_SCALE2_S);
            break;
        default :
            mp_raise_ValueError("Parameter Error");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mdac_scale_set_obj, mdac_scale_set);

/*
 * Offset output of a DAC channel
 *
 * Range 0x00 - 0xFF
 *
 */
STATIC mp_obj_t mdac_offset_set(mp_obj_t self_in, mp_obj_t offset_in) {
    mdac_obj_t *self = self_in;
    int offset = mp_obj_get_int(offset_in);
    if (offset < 0 || offset > 0xFF) mp_raise_ValueError("Offset out of range");
    switch(self->dac_id) {
        case DAC_CHANNEL_1:
            SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_DC1, offset, SENS_DAC_DC1_S);
            break;
        case DAC_CHANNEL_2:
            SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_DC2, offset, SENS_DAC_DC2_S);
            break;
        default :
            mp_raise_ValueError("Parameter Error");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mdac_offset_set_obj, mdac_offset_set);

/*
 * Invert output pattern of a DAC channel
 *
 * - 00: does not invert any bits,
 * - 01: inverts all bits,
 * - 10: inverts MSB,
 * - 11: inverts all bits except for MSB
 *
 */
STATIC mp_obj_t mdac_invert_set(mp_obj_t self_in, mp_obj_t invert_in) {
    mdac_obj_t *self = self_in;
    int invert = mp_obj_get_int(invert_in);
    if (invert < 0 || invert > 0b11) mp_raise_ValueError("Invert out of range");
    switch(self->dac_id) {
        case DAC_CHANNEL_1:
            SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_INV1, invert, SENS_DAC_INV1_S);
            break;
        case DAC_CHANNEL_2:
            SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_INV2, invert, SENS_DAC_INV2_S);
            break;
        default :
            mp_raise_ValueError("Parameter Error");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mdac_invert_set_obj, mdac_invert_set);

/*
 * Get the current frequency
 */
STATIC mp_obj_t mdac_frequency_get(mp_obj_t self_in) {
    mdac_obj_t *self = self_in;
    return mp_obj_new_int(RTC_FAST_CLK_FREQ_APPROX / (1 + self->clk_8m_div) * (float) self->frequency_step / 65536);
}
MP_DEFINE_CONST_FUN_OBJ_1(mdac_frequency_get_obj, mdac_frequency_get);

STATIC const mp_rom_map_elem_t mdac_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mdac_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_cosine_enable), MP_ROM_PTR(&mdac_cosine_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_cosine_disable), MP_ROM_PTR(&mdac_cosine_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency_step), MP_ROM_PTR(&mdac_frequency_step_obj) },
    { MP_ROM_QSTR(MP_QSTR_rtc_clk_div), MP_ROM_PTR(&mdac_rtc_clk_div_obj) },
    { MP_ROM_QSTR(MP_QSTR_scale_set), MP_ROM_PTR(&mdac_scale_set_obj) },
    { MP_ROM_QSTR(MP_QSTR_offset_set), MP_ROM_PTR(&mdac_offset_set_obj) },
    { MP_ROM_QSTR(MP_QSTR_invert_set), MP_ROM_PTR(&mdac_invert_set_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency_get), MP_ROM_PTR(&mdac_frequency_get_obj) }
};

STATIC MP_DEFINE_CONST_DICT(mdac_locals_dict, mdac_locals_dict_table);

const mp_obj_type_t machine_dac_type = {
    { &mp_type_type },
    .name = MP_QSTR_DAC,
    .print = mdac_print,
    .make_new = mdac_make_new,
    .locals_dict = (mp_obj_t)&mdac_locals_dict,
};
