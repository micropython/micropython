/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
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

#ifndef MICROPY_INCLUDED_MACHINE_ADCBLOCK_H
#define MICROPY_INCLUDED_MACHINE_ADCBLOCK_H

// These should go into the specific BSP
#define DEFAULT_ADC_BITS        12
#define ADC_BLOCK_CHANNEL_MAX   6
#define DEFAULT_ADC_ACQ_NS      1000

#define ADCBLOCK0               (0)
#define ADCBLOCK_CHANNEL_MAX    (1)
#define MAX_BLOCKS              (1)
#define MAX_CHANNELS            (6)

typedef struct _machine_adc_obj_t machine_adc_obj_t; /* Forward declaration of adc_obj */

typedef struct _machine_adcblock_obj_t {
    mp_obj_base_t base;
    cyhal_adc_t adc_obj;
    uint8_t id;
    uint8_t bits;
    machine_adc_obj_t *channel[ADC_BLOCK_CHANNEL_MAX];
} machine_adcblock_obj_t;

typedef struct
{
    uint16_t block_id;
    uint16_t channel;
    uint32_t pin;
}adc_block_channel_pin_map_t;

void mod_adcblock_deinit(void);
#endif // MICROPY_INCLUDED_MACHINE_ADCBLOCK_H
