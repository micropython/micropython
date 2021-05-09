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

#define gpio_num_t uint8_t // make code compatible with ESP32 version
// #define PORTPA_SHIFT(value) value & 0x3F   // extract shift counter from machine_pin_obj_t->port_shift
// #define PORTPA_NR(value) (value & 0xC0)>>6 // extract the port number from machine_pin_obj_t->port_shift
// #define PORT_PA(pin_obj) _UL_(1)<<((pin_obj->port_shift) & 0x3F) // PORT_PAxx 32 bit value calculated from shift counter
#define adc1_channel_t uint8_t 

// TODO: ESP32 - adapt to SAMD21
#define GPIO_NUM_11  11
#define ADC1_CHANNEL_0  0
#define ADC_WIDTH_12BIT 12 

#define adc_bits_width_t uint8_t

const mp_obj_type_t machine_adc_type;

//typedef struct _machine_pin_obj_t {
//    mp_obj_base_t base;
//    gpio_num_t id;
    // port_shift = 0bXXYYYYYY : X for port#, Y for number left shift to calculate PORT_PA of the pin
//    uint8_t port_shift;
//} machine_pin_obj_t;

typedef struct _madc_obj_t {
    mp_obj_base_t base;
    gpio_num_t gpio_id;
    adc1_channel_t adc1_id;
} madc_obj_t;


STATIC const madc_obj_t madc_obj[] = {
    {{&machine_adc_type}, GPIO_NUM_11, ADC1_CHANNEL_0},
};

gpio_num_t machine_pin_get_id(mp_obj_t pin_in);

#endif // MICROPY_INCLUDED_SAMD_MACHINE_PIN_H
