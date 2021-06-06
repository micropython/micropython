/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Meurisse D. (MCHobby)
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
#ifndef MICROPY_INCLUDED_SAMD_MACHINE_ADC_H
#define MICROPY_INCLUDED_SAMD_MACHINE_ADC_H

#define ADC_WIDTH_12BIT 12

#define gpio_num_t uint8_t 
#define adc_bits_width_t uint8_t
#define cpu_pin_t uint8_t
#define io_group_t uint8_t
#define io_pin_t uint8_t
#define muxpos_pin_t unsigned long


const mp_obj_type_t machine_adc_type;

typedef struct _madc_obj_t {
    mp_obj_base_t base;
    gpio_num_t gpio_id;
    cpu_pin_t cpu_pin;
    io_group_t io_group;
    io_pin_t io_pin;
    muxpos_pin_t muxpos_pin;
} madc_obj_t;


// see Pg 21, 6.1 Multiplexed Signals from SAMD21 datasheet
STATIC const madc_obj_t madc_obj[] = {
    {{&machine_adc_type}, PIN_PA08, 11, 0,8, ADC_INPUTCTRL_MUXPOS_PIN16}, //Trinket 0
    {{&machine_adc_type}, PIN_PA02,  3, 0,2, ADC_INPUTCTRL_MUXPOS_PIN0},  //.. 1
    //.. Mounting ADC on Pin 2 Kills MicroPython!
    // {{&machine_adc_type}, PIN_PA09, 12, 0,9, ADC_INPUTCTRL_MUXPOS_PIN17}, //.. 2
    {{NULL}, -1,-1, -1,-1, -1 },                                             //.. 2
    {{&machine_adc_type}, PIN_PA07,  8, 0,7, ADC_INPUTCTRL_MUXPOS_PIN7},  //.. 3
    {{&machine_adc_type}, PIN_PA06,  7, 0,6, ADC_INPUTCTRL_MUXPOS_PIN6},  //.. 4
    
};

gpio_num_t machine_pin_get_id(mp_obj_t pin_in);

#endif // MICROPY_INCLUDED_SAMD_MACHINE_PIN_H
