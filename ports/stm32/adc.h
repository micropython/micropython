/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_ADC_H
#define MICROPY_INCLUDED_STM32_ADC_H

#if !BUILDING_MBOOT
extern const mp_obj_type_t pyb_adc_type;
extern const mp_obj_type_t pyb_adc_all_type;
#endif

void adc_config(ADC_TypeDef *adc, uint32_t bits);
uint32_t adc_config_and_read_u16(ADC_TypeDef *adc, uint32_t channel, uint32_t sample_time);

#if defined(ADC_CHANNEL_VBAT)

static inline void adc_deselect_vbat(ADC_TypeDef *adc, uint32_t channel) {
    (void)adc;

    if (channel == ADC_CHANNEL_VBAT) {
        ADC_Common_TypeDef *adc_common;

        #if defined(STM32F0) || defined(STM32G0) || defined(STM32WB)
        adc_common = ADC1_COMMON;
        #elif defined(STM32F4)
        adc_common = ADC_COMMON_REGISTER(0);
        #elif defined(STM32F7)
        adc_common = ADC123_COMMON;
        #elif defined(STM32G4) || defined(STM32H5)
        adc_common = ADC12_COMMON;
        #elif defined(STM32H7A3xx) || defined(STM32H7A3xxQ) || defined(STM32H7B3xx) || defined(STM32H7B3xxQ)
        adc_common = ADC12_COMMON;
        #elif defined(STM32H7)
        adc_common = adc == ADC3 ? ADC3_COMMON : ADC12_COMMON;
        #elif defined(STM32L4)
        adc_common = __LL_ADC_COMMON_INSTANCE(0);
        #elif defined(STM32WL)
        adc_common = ADC_COMMON;
        #endif

        adc_common->CCR &= ~LL_ADC_PATH_INTERNAL_VBAT;
    }
}

#else

static inline void adc_deselect_vbat(ADC_TypeDef *adc, uint32_t channel) {
    (void)adc;
    (void)channel;
}

#endif

#endif // MICROPY_INCLUDED_STM32_ADC_H
