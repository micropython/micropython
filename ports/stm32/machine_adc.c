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

// This file is never compiled standalone, it's included directly from
// extmod/machine_adc.c via MICROPY_PY_MACHINE_ADC_INCLUDEFILE.

#include "py/mphal.h"
#include "adc.h"

#if defined(STM32F0) || defined(STM32G0) || defined(STM32G4) || defined(STM32H5) || defined(STM32H7) || defined(STM32L0) || defined(STM32L4) || defined(STM32N6) || defined(STM32WB) || defined(STM32WL)
#define ADC_V2 (1)
#else
#define ADC_V2 (0)
#endif

#if defined(STM32F4)
#define ADCx_COMMON ADC_COMMON_REGISTER(0)
#elif defined(STM32F7)
#define ADCx_COMMON ADC123_COMMON
#elif defined(STM32L4)
#define ADCx_COMMON __LL_ADC_COMMON_INSTANCE(0)
#endif

#if defined(STM32F0) || defined(STM32G0) || defined(STM32L0) || defined(STM32L1) || defined(STM32WL)
#define ADC_STAB_DELAY_US (1)
#define ADC_TEMPSENSOR_DELAY_US (10)
#elif defined(STM32G4)
#define ADC_STAB_DELAY_US (20)
#elif defined(STM32H5)
// Stabilization delay = 1 conversion cycle
// ADC clk = PDIV / 16 = 250 MHz / 16 = 15.625 MHz -> 64 ns -> select 1 us
#define ADC_STAB_DELAY_US (1)
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
#elif defined(STM32G4)
#define ADC_SAMPLETIME_DEFAULT      ADC_SAMPLETIME_12CYCLES_5
#define ADC_SAMPLETIME_DEFAULT_INT  ADC_SAMPLETIME_247CYCLES_5
#elif defined(STM32H5)
// Worst case sampling time: slow channel, 12 bits, 680 ohms -> 165 ns
// ADC clk = PDIV / 16 = 250 MHz / 16 = 15.625 MHz -> 64 ns -> 2.57 cycles -> select 6.5 cycles
#define ADC_SAMPLETIME_DEFAULT      ADC_SAMPLETIME_6CYCLES_5
#define ADC_SAMPLETIME_DEFAULT_INT  ADC_SAMPLETIME_247CYCLES_5
#elif defined(STM32H7)
#define ADC_SAMPLETIME_DEFAULT      ADC_SAMPLETIME_8CYCLES_5
#define ADC_SAMPLETIME_DEFAULT_INT  ADC_SAMPLETIME_387CYCLES_5
#elif defined(STM32G0) || defined(STM32L0) || defined(STM32WL)
#define ADC_SAMPLETIME_DEFAULT      ADC_SAMPLETIME_12CYCLES_5
#define ADC_SAMPLETIME_DEFAULT_INT  ADC_SAMPLETIME_160CYCLES_5
#elif defined(STM32L1)
#define ADC_SAMPLETIME_DEFAULT      ADC_SAMPLETIME_16CYCLES
#define ADC_SAMPLETIME_DEFAULT_INT  ADC_SAMPLETIME_384CYCLES
#elif defined(STM32L4) || defined(STM32WB)
#define ADC_SAMPLETIME_DEFAULT      ADC_SAMPLETIME_12CYCLES_5
#define ADC_SAMPLETIME_DEFAULT_INT  ADC_SAMPLETIME_247CYCLES_5
#elif defined(STM32N6)
#define ADC_SAMPLETIME_DEFAULT      ADC_SAMPLETIME_11CYCLES_5
#define ADC_SAMPLETIME_DEFAULT_INT  ADC_SAMPLETIME_246CYCLES_5
#endif

// Timeout for waiting for end-of-conversion
#define ADC_EOC_TIMEOUT_MS (10)

// Channel IDs for machine.ADC object
typedef enum _machine_adc_internal_ch_t {
    // Regular external ADC inputs (0..19)
    MACHINE_ADC_EXT_CH_0 = 0,
    MACHINE_ADC_EXT_CH_19 = 19,

    // Internal ADC channels (256..)
    MACHINE_ADC_INT_CH_VREFINT = 256,
    MACHINE_ADC_INT_CH_TEMPSENSOR,
    #if defined(ADC_CHANNEL_VBAT)
    MACHINE_ADC_INT_CH_VBAT,
    #endif
    #if defined(ADC_CHANNEL_VDDCORE)
    MACHINE_ADC_INT_CH_VDDCORE,
    #endif

    // This is a synthesised channel representing the maximum ADC reading (useful to scale other channels)
    MACHINE_ADC_CH_VREF = 0xffff // 0xffff for backward compatibility
} machine_adc_internal_ch_t;

// Convert machine_adc_internal_ch_t value to STM32 library ADC channel literal.
// This function is required as literals are uint32_t types that don't map with MP_ROM_INT (31 bit signed).
static uint32_t adc_ll_channel(uint32_t channel_id) {
    uint32_t adc_ll_ch;
    switch (channel_id) {
        // external channels map 1:1
        case MACHINE_ADC_EXT_CH_0 ... MACHINE_ADC_EXT_CH_19:
            adc_ll_ch = channel_id;
            break;

        // internal channels are converted to STM32 ADC defines
        case MACHINE_ADC_INT_CH_VREFINT:
            adc_ll_ch = ADC_CHANNEL_VREFINT;
            break;
        case MACHINE_ADC_INT_CH_TEMPSENSOR:
            #if defined(STM32G4)
            adc_ll_ch = ADC_CHANNEL_TEMPSENSOR_ADC1;
            #elif defined(STM32N6)
            adc_ll_ch = ADC_CHANNEL_0; // TODO
            #else
            adc_ll_ch = ADC_CHANNEL_TEMPSENSOR;
            #endif
            break;
        #if defined(ADC_CHANNEL_VBAT)
        case MACHINE_ADC_INT_CH_VBAT:
            adc_ll_ch = ADC_CHANNEL_VBAT;
            break;
        #endif
        #if defined(ADC_CHANNEL_VDDCORE)
        case MACHINE_ADC_INT_CH_VDDCORE:
            adc_ll_ch = ADC_CHANNEL_VDDCORE;
            break;
        #endif

        // To save code memory for costly error handling, default to Vref for unknown channels
        default:
            adc_ll_ch = ADC_CHANNEL_VREFINT;
            break;
    };
    return adc_ll_ch;
}

static inline void adc_stabilisation_delay_us(uint32_t us) {
    mp_hal_delay_us(us + 1);
}

static void adc_wait_eoc(ADC_TypeDef *adc, int32_t timeout_ms) {
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
static const uint8_t adc_cr_to_bits_table[] = {16, 14, 12, 10, 8, 8, 8, 8};
#else
static const uint8_t adc_cr_to_bits_table[] = {12, 10, 8, 6};
#endif

void adc_config(ADC_TypeDef *adc, uint32_t bits) {
    // Configure ADC clock source and enable ADC clock
    #if defined(STM32G0) || defined(STM32L4) || defined(STM32WB) || defined(STM32WL)
    __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_SYSCLK);
    __HAL_RCC_ADC_CLK_ENABLE();
    #else
    if (adc == ADC1) {
        #if defined(STM32H5)
        __HAL_RCC_ADC_CLK_ENABLE();
        #elif defined(STM32G4) || defined(STM32H7) || defined(STM32N6)
        __HAL_RCC_ADC12_CLK_ENABLE();
        #else
        __HAL_RCC_ADC1_CLK_ENABLE();
        #endif
    }
    #if defined(ADC2)
    if (adc == ADC2) {
        #if defined(STM32H5)
        __HAL_RCC_ADC_CLK_ENABLE();
        #elif defined(STM32G4) || defined(STM32H7) || defined(STM32N6)
        __HAL_RCC_ADC12_CLK_ENABLE();
        #else
        __HAL_RCC_ADC2_CLK_ENABLE();
        #endif
    }
    #endif
    #if defined(ADC3)
    if (adc == ADC3) {
        #if defined(ADC345_COMMON)
        __HAL_RCC_ADC345_CLK_ENABLE();
        #else
        __HAL_RCC_ADC3_CLK_ENABLE();
        #endif
    }
    #endif
    #endif

    // Configure clock mode
    #if defined(STM32F0)
    adc->CFGR2 = 2 << ADC_CFGR2_CKMODE_Pos; // PCLK/4 (synchronous clock mode)
    #elif defined(STM32F4) || defined(STM32F7) || defined(STM32L4)
    ADCx_COMMON->CCR = 0; // ADCPR=PCLK/2
    #elif defined(STM32G4) || defined(STM32H5)
    ADC12_COMMON->CCR = 7 << ADC_CCR_PRESC_Pos; // PCLK/16 (asynchronous clock mode)
    #elif defined(STM32H7A3xx) || defined(STM32H7A3xxQ) || defined(STM32H7B3xx) || defined(STM32H7B3xxQ)
    ADC12_COMMON->CCR = 3 << ADC_CCR_CKMODE_Pos;
    #elif defined(STM32H7)
    ADC12_COMMON->CCR = 3 << ADC_CCR_CKMODE_Pos;
    ADC3_COMMON->CCR = 3 << ADC_CCR_CKMODE_Pos;
    #elif defined(STM32L0)
    ADC1_COMMON->CCR = 0; // ADCPR=PCLK/2
    #elif defined(STM32L1)
    ADC1_COMMON->CCR = 1 << ADC_CCR_ADCPRE_Pos; // ADCPRE=2
    #elif defined(STM32WB)
    ADC1_COMMON->CCR = 0 << ADC_CCR_PRESC_Pos | 0 << ADC_CCR_CKMODE_Pos; // PRESC=1, MODE=ASYNC
    #elif defined(STM32WL)
    ADC_COMMON->CCR = 0 << ADC_CCR_PRESC_Pos; // PRESC=1
    #endif

    #if defined(STM32G4) || defined(STM32H5) || defined(STM32H7) || defined(STM32L4) || defined(STM32N6) || defined(STM32WB)
    if (adc->CR & ADC_CR_DEEPPWD) {
        adc->CR = 0; // disable deep powerdown
    }
    #endif

    #if defined(STM32G4) || defined(STM32H5) || defined(STM32H7) || defined(STM32L0) || defined(STM32L4) || defined(STM32WB) || defined(STM32WL)
    if (!(adc->CR & ADC_CR_ADVREGEN)) {
        adc->CR = ADC_CR_ADVREGEN; // enable VREG
        #if defined(STM32H7)
        mp_hal_delay_us(10); // T_ADCVREG_STUP
        #elif defined(STM32G4) || defined(STM32L4) || defined(STM32WB)
        mp_hal_delay_us(20); // T_ADCVREG_STUP
        #endif
    }
    #endif

    #if ADC_V2
    if (!(adc->CR & ADC_CR_ADEN)) {
        // ADC isn't enabled so calibrate it now
        #if defined(STM32F0) || defined(STM32G0) || defined(STM32L0) || defined(STM32WL)
        LL_ADC_StartCalibration(adc);
        #elif defined(STM32G4) || defined(STM32H5) || defined(STM32L4) || defined(STM32WB)
        LL_ADC_StartCalibration(adc, LL_ADC_SINGLE_ENDED);
        #elif defined(STM32N6)
        ADC_HandleTypeDef hadc;
        hadc.Instance = adc;
        hadc.State = 0;
        HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
        #else
        LL_ADC_StartCalibration(adc, LL_ADC_CALIB_OFFSET_LINEARITY, LL_ADC_SINGLE_ENDED);
        #endif
        while (LL_ADC_IsCalibrationOnGoing(adc)) {
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

    #elif defined(STM32H5) || defined(STM32H7) || defined(STM32L4) || defined(STM32WB)

    uint32_t cfgr_clr = ADC_CFGR_CONT | ADC_CFGR_EXTEN | ADC_CFGR_RES;
    #if defined(STM32H7)
    cfgr_clr |= ADC_CFGR_DMNGT;
    #else
    cfgr_clr |= ADC_CFGR_ALIGN | ADC_CFGR_DMAEN;
    #endif
    uint32_t cfgr = res << ADC_CFGR_RES_Pos;
    adc->CFGR = (adc->CFGR & ~cfgr_clr) | cfgr;

    #elif defined(STM32N6)

    uint32_t cfgr1_clr = ADC_CFGR1_CONT | ADC_CFGR1_EXTEN;
    uint32_t cfgr1 = res << ADC_CFGR1_RES_Pos;
    adc->CFGR1 = (adc->CFGR1 & ~cfgr1_clr) | cfgr1;

    #endif
}

static int adc_get_bits(ADC_TypeDef *adc) {
    #if defined(STM32F0) || defined(STM32G0) || defined(STM32L0) || defined(STM32N6) || defined(STM32WL)
    uint32_t res = (adc->CFGR1 & ADC_CFGR1_RES) >> ADC_CFGR1_RES_Pos;
    #elif defined(STM32F4) || defined(STM32F7) || defined(STM32L1)
    uint32_t res = (adc->CR1 & ADC_CR1_RES) >> ADC_CR1_RES_Pos;
    #elif defined(STM32G4) || defined(STM32H5) || defined(STM32H7) || defined(STM32L4) || defined(STM32WB)
    uint32_t res = (adc->CFGR & ADC_CFGR_RES) >> ADC_CFGR_RES_Pos;
    #endif
    return adc_cr_to_bits_table[res];
}

static void adc_config_channel(ADC_TypeDef *adc, uint32_t channel, uint32_t sample_time) {
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

    #if defined(STM32F0) || defined(STM32G0) || defined(STM32L0)

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
    #if defined(STM32G0)
    adc->SMPR = sample_time << ADC_SMPR_SMP1_Pos; // select sample time from SMP1 (default)
    #else
    adc->SMPR = sample_time << ADC_SMPR_SMP_Pos; // select sample time
    #endif
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

    #elif defined(STM32L1)

    ADC_Common_TypeDef *adc_common = ADC1_COMMON;
    if (channel == ADC_CHANNEL_VREFINT || channel == ADC_CHANNEL_TEMPSENSOR) {
        adc_common->CCR |= ADC_CCR_TSVREFE;
        if (channel == ADC_CHANNEL_TEMPSENSOR) {
            adc_stabilisation_delay_us(ADC_TEMPSENSOR_DELAY_US);
        }
    }

    adc->SQR1 = (1 - 1) << ADC_SQR1_L_Pos;
    adc->SQR5 = (channel & 0x1f) << ADC_SQR5_SQ1_Pos;

    __IO uint32_t *smpr;
    if (channel >= 20) {
        smpr = &adc->SMPR1;
        channel -= 20;
    } else if (channel >= 10) {
        smpr = &adc->SMPR2;
        channel -= 10;
    } else {
        smpr = &adc->SMPR3;
    }
    *smpr = (*smpr & ~(7 << (channel * 3))) | sample_time << (channel * 3); // select sample time

    #elif defined(STM32G4) || defined(STM32H5) || defined(STM32H7) || defined(STM32L4) || defined(STM32N6) || defined(STM32WB)

    #if defined(STM32G4) || defined(STM32H5) || defined(STM32H7A3xx) || defined(STM32H7A3xxQ) || defined(STM32H7B3xx) || defined(STM32H7B3xxQ) || defined(STM32N6)
    ADC_Common_TypeDef *adc_common = ADC12_COMMON;
    #elif defined(STM32H7)
    #if defined(ADC_VER_V5_V90)
    if (adc != ADC3) {
        adc->PCSEL_RES0 |= 1 << channel;
    }
    #else
    adc->PCSEL |= 1 << channel;
    #endif
    ADC_Common_TypeDef *adc_common = adc == ADC3 ? ADC3_COMMON : ADC12_COMMON;
    #elif defined(STM32L4)
    ADC_Common_TypeDef *adc_common = ADCx_COMMON;
    #elif defined(STM32WB)
    ADC_Common_TypeDef *adc_common = ADC1_COMMON;
    #endif
    if (channel == ADC_CHANNEL_VREFINT) {
        adc_common->CCR |= ADC_CCR_VREFEN;
        #if !defined(STM32N6)
    #if defined(STM32G4)
    } else if (channel == ADC_CHANNEL_TEMPSENSOR_ADC1) {
        adc_common->CCR |= ADC_CCR_VSENSESEL;
    #else
    } else if (channel == ADC_CHANNEL_TEMPSENSOR) {
        adc_common->CCR |= ADC_CCR_TSEN;
    #endif
        adc_stabilisation_delay_us(ADC_TEMPSENSOR_DELAY_US);
        #endif
    } else if (channel == ADC_CHANNEL_VBAT) {
        #if defined(STM32G4)
        adc_common->CCR |= ADC_CCR_VBATSEL;
        #else
        adc_common->CCR |= ADC_CCR_VBATEN;
        #endif
    #if defined(STM32H5) || defined(STM32N6)
    } else if (channel == ADC_CHANNEL_VDDCORE) {
        adc->OR |= ADC_OR_OP0; // Enable Vddcore channel on ADC2
    #endif
    }
    #if defined(STM32G4) || defined(STM32H5) || defined(STM32N6) || defined(STM32WB)
    // MCU uses encoded literals for internal channels -> extract ADC channel for following code
    if (__LL_ADC_IS_CHANNEL_INTERNAL(channel)) {
        channel = __LL_ADC_CHANNEL_TO_DECIMAL_NB(channel);
    }
    adc->DIFSEL &= ~(1 << channel); // Set channel to Single-ended.
    #endif
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

static uint32_t adc_read_channel(ADC_TypeDef *adc) {
    uint32_t value;
    #if defined(STM32G4) || defined(STM32WB)
    // For STM32G4 errata 2.7.7 / STM32WB errata 2.7.1:
    // "Wrong ADC result if conversion done late after calibration or previous conversion"
    // states an incorrect reading is returned if more than 1ms has elapsed since the last
    // reading or calibration. According to the errata, this can be avoided by performing
    // two consecutive ADC conversions and keeping the second result.
    // Note: On STM32WB55 @ 64Mhz each ADC read takes ~ 3us.
    for (uint8_t i = 0; i < 2; i++)
    #endif
    {
        #if ADC_V2
        adc->CR |= ADC_CR_ADSTART;
        #else
        adc->CR2 |= ADC_CR2_SWSTART;
        #endif
        adc_wait_eoc(adc, ADC_EOC_TIMEOUT_MS);
        value = adc->DR;
    }
    return value;
}

uint32_t adc_config_and_read_u16(ADC_TypeDef *adc, uint32_t channel, uint32_t sample_time) {
    if (channel == MACHINE_ADC_CH_VREF) {
        return 0xffff;
    }

    // Map internal channel_id to STM32 ADC driver value/literal.
    channel = adc_ll_channel(channel);

    // Select, configure and read the channel.
    adc_config_channel(adc, channel, sample_time);
    uint32_t raw = adc_read_channel(adc);

    // If VBAT was sampled then deselect it to prevent battery drain.
    adc_deselect_vbat(adc, channel);

    // Scale raw reading to 16 bit value using a Taylor expansion (for bits <= 16).
    uint32_t bits = adc_get_bits(adc);
    if (bits < 8) {
        // For 6 and 7 bits
        return raw << (16 - bits) | raw << (16 - 2 * bits) | raw >> (3 * bits - 16);
    }
    return raw << (16 - bits) | raw >> (2 * bits - 16);
}

/******************************************************************************/
// MicroPython bindings for machine.ADC

#if !BUILDING_MBOOT

#if defined(ADC_CHANNEL_VBAT)
#define MICROPY_PY_MACHINE_ADC_CLASS_CONSTANTS_CORE_VBAT \
    { MP_ROM_QSTR(MP_QSTR_CORE_VBAT), MP_ROM_INT(MACHINE_ADC_INT_CH_VBAT) },
#else
#define MICROPY_PY_MACHINE_ADC_CLASS_CONSTANTS_CORE_VBAT
#endif

#if defined(ADC_CHANNEL_VDDCORE)
#define MICROPY_PY_MACHINE_ADC_CLASS_CONSTANTS_CORE_VDD \
    { MP_ROM_QSTR(MP_QSTR_CORE_VDD), MP_ROM_INT(MACHINE_ADC_INT_CH_VDDCORE) },
#else
#define MICROPY_PY_MACHINE_ADC_CLASS_CONSTANTS_CORE_VDD
#endif

#define MICROPY_PY_MACHINE_ADC_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_VREF), MP_ROM_INT(MACHINE_ADC_CH_VREF) }, \
    { MP_ROM_QSTR(MP_QSTR_CORE_VREF), MP_ROM_INT(MACHINE_ADC_INT_CH_VREFINT) }, \
    { MP_ROM_QSTR(MP_QSTR_CORE_TEMP), MP_ROM_INT(MACHINE_ADC_INT_CH_TEMPSENSOR) }, \
    MICROPY_PY_MACHINE_ADC_CLASS_CONSTANTS_CORE_VBAT \
    MICROPY_PY_MACHINE_ADC_CLASS_CONSTANTS_CORE_VDD \

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    ADC_TypeDef *adc;
    uint32_t channel; // one of machine_adc_internal_ch_t
    uint32_t sample_time;
} machine_adc_obj_t;

static void mp_machine_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
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
static mp_obj_t mp_machine_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_obj_t source = all_args[0];

    uint32_t channel;
    uint32_t sample_time = ADC_SAMPLETIME_DEFAULT;
    ADC_TypeDef *adc;
    if (mp_obj_is_int(source)) {
        channel = mp_obj_get_int(source);
        #if defined(STM32WL)
        adc = ADC;
        #elif defined(STM32H5)
        // on STM32H5 vbat and vddcore channels are on ADC2
        if (channel == MACHINE_ADC_INT_CH_VBAT || channel == MACHINE_ADC_INT_CH_VDDCORE) {
            adc = ADC2;
        } else {
            adc = ADC1;
        }
        #else
        adc = ADC1;
        #endif
        if (channel == MACHINE_ADC_INT_CH_VREFINT || channel == MACHINE_ADC_INT_CH_TEMPSENSOR
            #if defined(ADC_CHANNEL_VBAT)
            || channel == MACHINE_ADC_INT_CH_VBAT
            #endif
            #if defined(ADC_CHANNEL_VDDCORE)
            || channel == MACHINE_ADC_INT_CH_VDDCORE
            #endif
            ) {
            sample_time = ADC_SAMPLETIME_DEFAULT_INT;
        }
    } else {
        const machine_pin_obj_t *pin = pin_find(source);
        if (pin->adc_num & PIN_ADC1) {
            #if defined(STM32WL)
            adc = ADC;
            #else
            adc = ADC1;
            #endif
        #if defined(ADC2)
        } else if (pin->adc_num & PIN_ADC2) {
            adc = ADC2;
        #endif
        #if defined(ADC3)
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

    machine_adc_obj_t *o = mp_obj_malloc(machine_adc_obj_t, &machine_adc_type);
    o->adc = adc;
    o->channel = channel;
    o->sample_time = sample_time;

    return MP_OBJ_FROM_PTR(o);
}

// read_u16()
static mp_int_t mp_machine_adc_read_u16(machine_adc_obj_t *self) {
    return adc_config_and_read_u16(self->adc, self->channel, self->sample_time);
}

#endif
