/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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
#include "py/mphal.h"

#if defined(STM32F0) || defined(STM32H7) || defined(STM32L0) || defined(STM32L4) || defined(STM32WB)
#define ADC_V2 (1)
#else
#define ADC_V2 (0)
#endif

#if defined(STM32F4) || defined(STM32L4)
#define ADCx_COMMON ADC_COMMON_REGISTER(0)
#elif defined(STM32F7)
#define ADCx_COMMON ADC123_COMMON
#endif

#if defined(STM32F0) || defined(STM32L0)
#define ADC_STAB_DELAY_US (1)
#define ADC_TEMPSENSOR_DELAY_US (10)
#elif defined(STM32L4)
#define ADC_STAB_DELAY_US (10)
#elif defined(STM32WB)
#define ADC_STAB_DELAY_US (1)
#endif

#if defined(STM32F0)
#define ADC_SAMPLETIME_DEFAULT      ADC_SAMPLETIME_13CYCLES_5
#define ADC_SAMPLETIME_DEFAULT_INT  ADC_SAMPLETIME_239CYCLES_5
#elif defined(STM32F4) || defined(STM32F7)
#define ADC_SAMPLETIME_DEFAULT      ADC_SAMPLETIME_15CYCLES
#define ADC_SAMPLETIME_DEFAULT_INT  ADC_SAMPLETIME_480CYCLES
#elif defined(STM32H7)
#define ADC_SAMPLETIME_DEFAULT      ADC_SAMPLETIME_8CYCLES_5
#define ADC_SAMPLETIME_DEFAULT_INT  ADC_SAMPLETIME_387CYCLES_5
#elif defined(STM32L0)
#define ADC_SAMPLETIME_DEFAULT      ADC_SAMPLETIME_12CYCLES_5
#define ADC_SAMPLETIME_DEFAULT_INT  ADC_SAMPLETIME_160CYCLES_5
#elif defined(STM32L4) || defined(STM32WB)
#define ADC_SAMPLETIME_DEFAULT      ADC_SAMPLETIME_12CYCLES_5
#define ADC_SAMPLETIME_DEFAULT_INT  ADC_SAMPLETIME_247CYCLES_5
#endif

// Timeout for waiting for end-of-conversion
#define ADC_EOC_TIMEOUT_MS (10)

// This is a synthesised channel representing the maximum ADC reading (useful to scale other channels)
#define ADC_CHANNEL_VREF (0xffff)

static inline void adc_stabilisation_delay_us(uint32_t us) {
    mp_hal_delay_us(us + 1);
}

STATIC void adc_wait_eoc(ADC_TypeDef *adc, int32_t timeout_ms) {
    uint32_t t0 = mp_hal_ticks_ms();
    #if ADC_V2
    while (!(adc->ISR & ADC_ISR_EOC))
    #else
    while (!(adc->SR & ADC_SR_EOC))
    #endif
    {
        if (mp_hal_ticks_ms() - t0 > timeout_ms) {
            break; // timeout
        }
    }
}

#if defined(STM32H7)
STATIC const uint8_t adc_cr_to_bits_table[] = {16, 14, 12, 10, 8, 8, 8, 8};
#else
STATIC const uint8_t adc_cr_to_bits_table[] = {12, 10, 8, 6};
#endif

STATIC void adc_config(ADC_TypeDef *adc, uint32_t bits) {
    // Configure ADC clock source and enable ADC clock
    #if defined(STM32L4) || defined(STM32WB)
    __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_SYSCLK);
    __HAL_RCC_ADC_CLK_ENABLE();
    #else
    if (adc == ADC1) {
        #if defined(STM32H7)
        __HAL_RCC_ADC12_CLK_ENABLE();
        #else
        __HAL_RCC_ADC1_CLK_ENABLE();
        #endif
    }
    #if defined(ADC2)
    if (adc == ADC2) {
        #if defined(STM32H7)
        __HAL_RCC_ADC12_CLK_ENABLE();
        #else
        __HAL_RCC_ADC2_CLK_ENABLE();
        #endif
    }
    #endif
    #if defined(ADC3)
    if (adc == ADC3) {
        __HAL_RCC_ADC3_CLK_ENABLE();
    }
    #endif
    #endif

    // Configure clock mode
    #if defined(STM32F0)
    adc->CFGR2 = 2 << ADC_CFGR2_CKMODE_Pos; // PCLK/4 (synchronous clock mode)
    #elif defined(STM32F4) || defined(STM32F7) || defined(STM32L4)
    ADCx_COMMON->CCR = 0; // ADCPR=PCLK/2
    #elif defined(STM32H7)
    ADC12_COMMON->CCR = 3 << ADC_CCR_CKMODE_Pos;
    ADC3_COMMON->CCR = 3 << ADC_CCR_CKMODE_Pos;
    #elif defined(STM32L0)
    ADC1_COMMON->CCR = 0; // ADCPR=PCLK/2
    #elif defined(STM32WB)
    ADC1_COMMON->CCR = 0 << ADC_CCR_PRESC_Pos | 0 << ADC_CCR_CKMODE_Pos; // PRESC=1, MODE=ASYNC
    #endif

    #if defined(STM32H7) || defined(STM32L4) || defined(STM32WB)
    if (adc->CR & ADC_CR_DEEPPWD) {
        adc->CR = 0; // disable deep powerdown
    }
    #endif

    #if defined(STM32H7) || defined(STM32L0) || defined(STM32L4) || defined(STM32WB)
    if (!(adc->CR & ADC_CR_ADVREGEN)) {
        adc->CR = ADC_CR_ADVREGEN; // enable VREG
        #if defined(STM32H7)
        mp_hal_delay_us(10); // T_ADCVREG_STUP
        #elif defined(STM32L4) || defined(STM32WB)
        mp_hal_delay_us(20); // T_ADCVREG_STUP
        #endif
    }
    #endif

    #if ADC_V2
    if (adc->CR == 0) {
        // ADC hasn't been enabled so calibrate it
        adc->CR |= ADC_CR_ADCAL;
        while (adc->CR & ADC_CR_ADCAL) {
        }
    }

    if (adc->CR & ADC_CR_ADEN) {
        // ADC enabled, need to disable it to change configuration
        if (adc->CR & ADC_CR_ADSTART) {
            adc->CR |= ADC_CR_ADSTP;
            while (adc->CR & ADC_CR_ADSTP) {
            }
        }
        adc->CR |= ADC_CR_ADDIS;
        while (adc->CR & ADC_CR_ADDIS) {
        }
    }
    #endif

    // Find resolution, defaulting to last element in table
    uint32_t res;
    for (res = 0; res <= MP_ARRAY_SIZE(adc_cr_to_bits_table); ++res) {
        if (adc_cr_to_bits_table[res] == bits) {
            break;
        }
    }

    #if defined(STM32F0) || defined(STM32L0)

    uint32_t cfgr1_clr = ADC_CFGR1_CONT | ADC_CFGR1_EXTEN | ADC_CFGR1_ALIGN | ADC_CFGR1_RES | ADC_CFGR1_DMAEN;
    uint32_t cfgr1 = res << ADC_CFGR1_RES_Pos;
    adc->CFGR1 = (adc->CFGR1 & ~cfgr1_clr) | cfgr1;

    #elif defined(STM32F4) || defined(STM32F7)

    uint32_t cr1_clr = ADC_CR1_RES;
    uint32_t cr1 = res << ADC_CR1_RES_Pos;
    adc->CR1 = (adc->CR1 & ~cr1_clr) | cr1;
    uint32_t cr2_clr = ADC_CR2_EXTEN | ADC_CR2_ALIGN | ADC_CR2_DMA | ADC_CR2_CONT;
    uint32_t cr2 = 0;
    adc->CR2 = (adc->CR2 & ~cr2_clr) | cr2;
    adc->SQR1 = 1 << ADC_SQR1_L_Pos; // 1 conversion in regular sequence

    #elif defined(STM32H7) || defined(STM32L4) || defined(STM32WB)

    uint32_t cfgr_clr = ADC_CFGR_CONT | ADC_CFGR_EXTEN | ADC_CFGR_RES;
    #if defined(STM32H7)
    cfgr_clr |= ADC_CFGR_DMNGT;
    #else
    cfgr_clr |= ADC_CFGR_ALIGN | ADC_CFGR_DMAEN;
    #endif
    uint32_t cfgr = res << ADC_CFGR_RES_Pos;
    adc->CFGR = (adc->CFGR & ~cfgr_clr) | cfgr;

    #endif
}

STATIC int adc_get_bits(ADC_TypeDef *adc) {
    #if defined(STM32F0) || defined(STM32L0)
    uint32_t res = (adc->CFGR1 & ADC_CFGR1_RES) >> ADC_CFGR1_RES_Pos;
    #elif defined(STM32F4) || defined(STM32F7)
    uint32_t res = (adc->CR1 & ADC_CR1_RES) >> ADC_CR1_RES_Pos;
    #elif defined(STM32H7) || defined(STM32L4) || defined(STM32WB)
    uint32_t res = (adc->CFGR & ADC_CFGR_RES) >> ADC_CFGR_RES_Pos;
    #endif
    return adc_cr_to_bits_table[res];
}

STATIC void adc_config_channel(ADC_TypeDef *adc, uint32_t channel, uint32_t sample_time) {
    #if ADC_V2
    if (!(adc->CR & ADC_CR_ADEN)) {
        if (adc->CR & 0x3f) {
            // Cannot enable ADC with CR!=0
            return;
        }
        adc->ISR = ADC_ISR_ADRDY; // clear ADRDY
        adc->CR |= ADC_CR_ADEN;
        adc_stabilisation_delay_us(ADC_STAB_DELAY_US);
        while (!(adc->ISR & ADC_ISR_ADRDY)) {
        }
    }
    #else
    if (!(adc->CR2 & ADC_CR2_ADON)) {
        adc->CR2 |= ADC_CR2_ADON;
        adc_stabilisation_delay_us(ADC_STAB_DELAY_US);
    }
    #endif

    #if defined(STM32F0) || defined(STM32L0)

    if (channel == ADC_CHANNEL_VREFINT) {
        ADC1_COMMON->CCR |= ADC_CCR_VREFEN;
    } else if (channel == ADC_CHANNEL_TEMPSENSOR) {
        ADC1_COMMON->CCR |= ADC_CCR_TSEN;
        adc_stabilisation_delay_us(ADC_TEMPSENSOR_DELAY_US);
    #if defined(ADC_CHANNEL_VBAT)
    } else if (channel == ADC_CHANNEL_VBAT) {
        ADC1_COMMON->CCR |= ADC_CCR_VBATEN;
    #endif
    }
    adc->SMPR = sample_time << ADC_SMPR_SMP_Pos; // select sample time
    adc->CHSELR = 1 << channel; // select channel for conversion

    #elif defined(STM32F4) || defined(STM32F7)

    if (channel == ADC_CHANNEL_VREFINT || channel == ADC_CHANNEL_TEMPSENSOR) {
        ADCx_COMMON->CCR = (ADCx_COMMON->CCR & ~ADC_CCR_VBATE) | ADC_CCR_TSVREFE;
        if (channel == ADC_CHANNEL_TEMPSENSOR) {
            adc_stabilisation_delay_us(ADC_TEMPSENSOR_DELAY_US);
        }
    } else if (channel == ADC_CHANNEL_VBAT) {
        ADCx_COMMON->CCR |= ADC_CCR_VBATE;
    }

    adc->SQR3 = (channel & 0x1f) << ADC_SQR3_SQ1_Pos; // select channel for first conversion

    __IO uint32_t *smpr;
    if (channel <= 9) {
        smpr = &adc->SMPR2;
    } else {
        smpr = &adc->SMPR1;
        channel -= 10;
    }
    *smpr = (*smpr & ~(7 << (channel * 3))) | sample_time << (channel * 3); // select sample time

    #elif defined(STM32H7) || defined(STM32L4) || defined(STM32WB)

    #if defined(STM32H7)
    adc->PCSEL |= 1 << channel;
    ADC_Common_TypeDef *adc_common = adc == ADC3 ? ADC3_COMMON : ADC12_COMMON;
    #elif defined(STM32L4)
    ADC_Common_TypeDef *adc_common = ADCx_COMMON;
    #elif defined(STM32WB)
    ADC_Common_TypeDef *adc_common = ADC1_COMMON;
    #endif
    if (channel == ADC_CHANNEL_VREFINT) {
        adc_common->CCR |= ADC_CCR_VREFEN;
    } else if (channel == ADC_CHANNEL_TEMPSENSOR) {
        adc_common->CCR |= ADC_CCR_TSEN;
        adc_stabilisation_delay_us(ADC_TEMPSENSOR_DELAY_US);
    } else if (channel == ADC_CHANNEL_VBAT) {
        adc_common->CCR |= ADC_CCR_VBATEN;
    }
    adc->SQR1 = (channel & 0x1f) << ADC_SQR1_SQ1_Pos | (1 - 1) << ADC_SQR1_L_Pos;
    __IO uint32_t *smpr;
    if (channel <= 9) {
        smpr = &adc->SMPR1;
    } else {
        smpr = &adc->SMPR2;
        channel -= 10;
    }
    *smpr = (*smpr & ~(7 << (channel * 3))) | sample_time << (channel * 3); // select sample time

    #endif
}

STATIC uint32_t adc_read_channel(ADC_TypeDef *adc) {
    #if ADC_V2
    adc->CR |= ADC_CR_ADSTART;
    #else
    adc->CR2 |= ADC_CR2_SWSTART;
    #endif
    adc_wait_eoc(adc, ADC_EOC_TIMEOUT_MS);
    uint32_t value = adc->DR;
    return value;
}

STATIC uint32_t adc_config_and_read_u16(ADC_TypeDef *adc, uint32_t channel, uint32_t sample_time) {
    if (channel == ADC_CHANNEL_VREF) {
        return 0xffff;
    }

    adc_config_channel(adc, channel, sample_time);
    uint32_t raw = adc_read_channel(adc);
    uint32_t bits = adc_get_bits(adc);
    // Scale raw reading to 16 bit value using a Taylor expansion (for 8 <= bits <= 16)
    #if defined(STM32H7)
    if (bits < 8) {
        // For 6 and 7 bits
        return raw << (16 - bits) | raw << (16 - 2 * bits) | raw >> (3 * bits - 16);
    }
    #endif
    return raw << (16 - bits) | raw >> (2 * bits - 16);
}

/******************************************************************************/
// MicroPython bindings for machine.ADC

const mp_obj_type_t machine_adc_type;

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    ADC_TypeDef *adc;
    uint32_t channel;
    uint32_t sample_time;
} machine_adc_obj_t;

STATIC void machine_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    unsigned adc_id = 1;
    #if defined(ADC2)
    if (self->adc == ADC2) {
        adc_id = 2;
    }
    #endif
    #if defined(ADC3)
    if (self->adc == ADC3) {
        adc_id = 3;
    }
    #endif
    mp_printf(print, "<ADC%u channel=%u>", adc_id, self->channel);
}

// ADC(id)
STATIC mp_obj_t machine_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_obj_t source = all_args[0];

    uint32_t channel;
    uint32_t sample_time = ADC_SAMPLETIME_DEFAULT;
    ADC_TypeDef *adc;
    if (mp_obj_is_int(source)) {
        adc = ADC1;
        channel = mp_obj_get_int(source);
        if (channel == ADC_CHANNEL_VREFINT
            || channel == ADC_CHANNEL_TEMPSENSOR
            #if defined(ADC_CHANNEL_VBAT)
            || channel == ADC_CHANNEL_VBAT
            #endif
            ) {
            sample_time = ADC_SAMPLETIME_DEFAULT_INT;
        }
    } else {
        const pin_obj_t *pin = pin_find(source);
        if (pin->adc_num & PIN_ADC1) {
            adc = ADC1;
        #if defined(ADC2)
        } else if (pin->adc_num & PIN_ADC2) {
            adc = ADC2;
        #endif
        #if defined(ADC2)
        } else if (pin->adc_num & PIN_ADC3) {
            adc = ADC3;
        #endif
        } else {
            // No ADC function on given pin
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%q) does not have ADC capabilities"), pin->name);
        }
        channel = pin->adc_channel;

        // Configure the GPIO pin in ADC mode
        mp_hal_pin_config(pin, MP_HAL_PIN_MODE_ADC, MP_HAL_PIN_PULL_NONE, 0);
    }

    adc_config(adc, 12);

    machine_adc_obj_t *o = m_new_obj(machine_adc_obj_t);
    o->base.type = &machine_adc_type;
    o->adc = adc;
    o->channel = channel;
    o->sample_time = sample_time;

    return MP_OBJ_FROM_PTR(o);
}

// read_u16()
STATIC mp_obj_t machine_adc_read_u16(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(adc_config_and_read_u16(self->adc, self->channel, self->sample_time));
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_u16_obj, machine_adc_read_u16);

STATIC const mp_rom_map_elem_t machine_adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read_u16), MP_ROM_PTR(&machine_adc_read_u16_obj) },

    { MP_ROM_QSTR(MP_QSTR_VREF), MP_ROM_INT(ADC_CHANNEL_VREF) },
    { MP_ROM_QSTR(MP_QSTR_CORE_VREF), MP_ROM_INT(ADC_CHANNEL_VREFINT) },
    { MP_ROM_QSTR(MP_QSTR_CORE_TEMP), MP_ROM_INT(ADC_CHANNEL_TEMPSENSOR) },
    #if defined(ADC_CHANNEL_VBAT)
    { MP_ROM_QSTR(MP_QSTR_CORE_VBAT), MP_ROM_INT(ADC_CHANNEL_VBAT) },
    #endif
};
STATIC MP_DEFINE_CONST_DICT(machine_adc_locals_dict, machine_adc_locals_dict_table);

const mp_obj_type_t machine_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = machine_adc_print,
    .make_new = machine_adc_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_adc_locals_dict,
};
