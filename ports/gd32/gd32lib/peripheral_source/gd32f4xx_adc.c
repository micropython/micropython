/*!
    \file    gd32f4xx_adc.c
    \brief   ADC driver

    \version 2026-02-05, V3.3.3, firmware for GD32F4xx
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32f4xx_adc.h"

#define ROUTINE_TRIGGER_MODE                        ((uint32_t)28U)
#define INSERTED_TRIGGER_MODE                       ((uint32_t)20U)
/* discontinuous mode macro*/
#define ADC_CHANNEL_LENGTH_SUBTRACT_ONE             ((uint8_t)1U)

/* ADC routine channel macro */
#define ADC_ROUTINE_CHANNEL_RANK_SIX                ((uint8_t)6U)
#define ADC_ROUTINE_CHANNEL_RANK_TWELVE             ((uint8_t)12U)
#define ADC_ROUTINE_CHANNEL_RANK_SIXTEEN            ((uint8_t)16U)
#define ADC_ROUTINE_CHANNEL_RANK_LENGTH             ((uint8_t)5U)

/* ADC sampling time macro */
#define ADC_CHANNEL_SAMPLE_TEN                      ((uint8_t)10U)
#define ADC_CHANNEL_SAMPLE_EIGHTEEN                 ((uint8_t)18U)
#define ADC_CHANNEL_SAMPLE_LENGTH                   ((uint8_t)3U)

/* ADC inserted channel macro */
#define ADC_INSERTED_CHANNEL_RANK_LENGTH            ((uint8_t)5U)
#define ADC_INSERTED_CHANNEL_SHIFT_LENGTH           ((uint8_t)15U)

/* ADC inserted channel offset macro */
#define ADC_OFFSET_LENGTH                           ((uint8_t)3U)
#define ADC_OFFSET_SHIFT_LENGTH                     ((uint8_t)4U)

/*!
    \brief    reset ADC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_deinit(void)
{
    rcu_periph_reset_enable(RCU_ADCRST);
    rcu_periph_reset_disable(RCU_ADCRST);
}

/*!
    \brief        configure the ADC clock for all the ADCs
    \param[in]    prescaler: configure ADCs prescaler ratio
                  only one parameter can be selected which is shown as below:
      \arg        ADC_ADCCK_PCLK2_DIV2: PCLK2 div2
      \arg        ADC_ADCCK_PCLK2_DIV4: PCLK2 div4
      \arg        ADC_ADCCK_PCLK2_DIV6: PCLK2 div6
      \arg        ADC_ADCCK_PCLK2_DIV8: PCLK2 div8
      \arg        ADC_ADCCK_HCLK_DIV5: HCLK div5
      \arg        ADC_ADCCK_HCLK_DIV6: HCLK div6
      \arg        ADC_ADCCK_HCLK_DIV10: HCLK div10
      \arg        ADC_ADCCK_HCLK_DIV20: HCLK div20
    \param[out]   none
    \retval       none
*/
void adc_clock_config(uint32_t prescaler)
{
    ADC_SYNCCTL &= ~((uint32_t)ADC_SYNCCTL_ADCCK);
    ADC_SYNCCTL |= (uint32_t) prescaler;
}

/*!
    \brief        enable or disable ADC special function
    \param[in]    adc_periph: ADCx,x=0,1,2
    \param[in]    function: the function to config
                  only one parameter can be selected which is shown as below:
    \arg          ADC_SCAN_MODE: scan mode select
    \arg          ADC_INSERTED_CHANNEL_AUTO: inserted sequence convert automatically
    \arg          ADC_CONTINUOUS_MODE: continuous mode select
    \param[in]    newvalue: ENABLE or DISABLE
    \param[out]   none
    \retval       none
*/
void adc_special_function_config(uint32_t adc_periph, uint32_t function, ControlStatus newvalue)
{
    if(newvalue) {
        if(0U != (function & ADC_SCAN_MODE)) {
            /* enable scan mode */
            ADC_CTL0(adc_periph) |= ADC_SCAN_MODE;
        }
        if(0U != (function & ADC_INSERTED_CHANNEL_AUTO)) {
            /* enable inserted sequence convert automatically */
            ADC_CTL0(adc_periph) |= ADC_INSERTED_CHANNEL_AUTO;
        }
        if(0U != (function & ADC_CONTINUOUS_MODE)) {
            /* enable continuous mode */
            ADC_CTL1(adc_periph) |= ADC_CONTINUOUS_MODE;
        }
    } else {
        if(0U != (function & ADC_SCAN_MODE)) {
            /* disable scan mode */
            ADC_CTL0(adc_periph) &= ~ADC_SCAN_MODE;
        }
        if(0U != (function & ADC_INSERTED_CHANNEL_AUTO)) {
            /* disable inserted sequence convert automatically */
            ADC_CTL0(adc_periph) &= ~ADC_INSERTED_CHANNEL_AUTO;
        }
        if(0U != (function & ADC_CONTINUOUS_MODE)) {
            /* disable continuous mode */
            ADC_CTL1(adc_periph) &= ~ADC_CONTINUOUS_MODE;
        }
    }
}

/*!
    \brief        configure ADC data alignment
    \param[in]    adc_periph: ADCx,x=0,1,2
    \param[in]    data_alignment: data alignment select
                  only one parameter can be selected which is shown as below:
    \arg          ADC_DATAALIGN_RIGHT: LSB alignment
    \arg          ADC_DATAALIGN_LEFT: MSB alignment
    \param[out]   none
    \retval       none
*/
void adc_data_alignment_config(uint32_t adc_periph, uint32_t data_alignment)
{
    if(ADC_DATAALIGN_RIGHT != data_alignment) {
        /* MSB alignment */
        ADC_CTL1(adc_periph) |= ADC_CTL1_DAL;
    } else {
        /* LSB alignment */
        ADC_CTL1(adc_periph) &= ~((uint32_t)ADC_CTL1_DAL);
    }
}

/*!
    \brief        enable ADC interface
    \param[in]    adc_periph: ADCx,x=0,1,2
    \param[out]   none
    \retval       none
*/
void adc_enable(uint32_t adc_periph)
{
    if(RESET == (ADC_CTL1(adc_periph) & ADC_CTL1_ADCON)) {
        /* enable ADC */
        ADC_CTL1(adc_periph) |= (uint32_t)ADC_CTL1_ADCON;
    }
}

/*!
    \brief        disable ADC interface
    \param[in]    adc_periph: ADCx,x=0,1,2
    \param[out]   none
    \retval       none
*/
void adc_disable(uint32_t adc_periph)
{
    /* disable ADC */
    ADC_CTL1(adc_periph) &= ~((uint32_t)ADC_CTL1_ADCON);
}

/*!
    \brief        ADC calibration and reset calibration
    \param[in]    adc_periph: ADCx,x=0,1,2
    \param[out]   none
    \retval       none
*/
void adc_calibration_enable(uint32_t adc_periph)
{
    /* reset the selected ADC calibration registers */
    ADC_CTL1(adc_periph) |= (uint32_t) ADC_CTL1_RSTCLB;
    /* check the RSTCLB bit state */
    while(RESET != (ADC_CTL1(adc_periph) & ADC_CTL1_RSTCLB)) {
    }
    /* enable ADC calibration process */
    ADC_CTL1(adc_periph) |= ADC_CTL1_CLB;
    /* check the CLB bit state */
    while(RESET != (ADC_CTL1(adc_periph) & ADC_CTL1_CLB)) {
    }
}

/*!
    \brief        configure temperature sensor and internal reference voltage channel or VBAT channel function
    \param[in]    function: temperature sensor and internal reference voltage channel or VBAT channel
                  only one parameter can be selected which is shown as below:
    \arg          ADC_VBAT_CHANNEL_SWITCH: channel 18 (1/4 voltate of external battery) switch of ADC0
    \arg          ADC_TEMP_VREF_CHANNEL_SWITCH: channel 16 (temperature sensor) and 17 (internal reference voltage) switch of ADC0
    \param[in]    newvalue: ENABLE or DISABLE
    \param[out]   none
    \retval       none
*/
void adc_channel_16_to_18(uint32_t function, ControlStatus newvalue)
{
    if(newvalue) {
        if(RESET != (function & ADC_VBAT_CHANNEL_SWITCH)) {
            /* enable ADC0 Vbat channel */
            ADC_SYNCCTL |= ADC_VBAT_CHANNEL_SWITCH;
        }
        if(RESET != (function & ADC_TEMP_VREF_CHANNEL_SWITCH)) {
            /* enable ADC0 Vref and Temperature channel */
            ADC_SYNCCTL |= ADC_TEMP_VREF_CHANNEL_SWITCH;
        }
    } else {
        if(RESET != (function & ADC_VBAT_CHANNEL_SWITCH)) {
            /* disable ADC0 Vbat channel  */
            ADC_SYNCCTL &= ~ADC_VBAT_CHANNEL_SWITCH;
        }
        if(RESET != (function & ADC_TEMP_VREF_CHANNEL_SWITCH)) {
            /* disable ADC0 Vref and Temperature channel */
            ADC_SYNCCTL &= ~ADC_TEMP_VREF_CHANNEL_SWITCH;
        }
    }
}

/*!
    \brief        configure ADC resolution
    \param[in]    adc_periph: ADCx,x=0,1,2
    \param[in]    resolution: ADC resolution
                  only one parameter can be selected which is shown as below:
    \arg          ADC_RESOLUTION_12B: 12-bit ADC resolution
    \arg          ADC_RESOLUTION_10B: 10-bit ADC resolution
    \arg          ADC_RESOLUTION_8B: 8-bit ADC resolution
    \arg          ADC_RESOLUTION_6B: 6-bit ADC resolution
    \param[out]   none
    \retval       none
*/
void adc_resolution_config(uint32_t adc_periph, uint32_t resolution)
{
    ADC_CTL0(adc_periph) &= ~((uint32_t)ADC_CTL0_DRES);
    ADC_CTL0(adc_periph) |= (uint32_t)resolution;
}

/*!
    \brief        configure ADC oversample mode
    \param[in]    adc_periph: ADCx,x=0,1,2
    \param[in]    mode: ADC oversampling mode
                  only one parameter can be selected which is shown as below:
    \arg          ADC_OVERSAMPLING_ALL_CONVERT: all oversampled conversions for a channel are done consecutively after a trigger
    \arg          ADC_OVERSAMPLING_ONE_CONVERT: each oversampled conversion for a channel needs a trigger
    \param[in]    shift: ADC oversampling shift
                  only one parameter can be selected which is shown as below:
    \arg          ADC_OVERSAMPLING_SHIFT_NONE: no oversampling shift
    \arg          ADC_OVERSAMPLING_SHIFT_1B: 1-bit oversampling shift
    \arg          ADC_OVERSAMPLING_SHIFT_2B: 2-bit oversampling shift
    \arg          ADC_OVERSAMPLING_SHIFT_3B: 3-bit oversampling shift
    \arg          ADC_OVERSAMPLING_SHIFT_4B: 3-bit oversampling shift
    \arg          ADC_OVERSAMPLING_SHIFT_5B: 5-bit oversampling shift
    \arg          ADC_OVERSAMPLING_SHIFT_6B: 6-bit oversampling shift
    \arg          ADC_OVERSAMPLING_SHIFT_7B: 7-bit oversampling shift
    \arg          ADC_OVERSAMPLING_SHIFT_8B: 8-bit oversampling shift
    \param[in]    ratio: ADC oversampling ratio
                  only one parameter can be selected which is shown as below:
    \arg          ADC_OVERSAMPLING_RATIO_MUL2: oversampling ratio multiple 2
    \arg          ADC_OVERSAMPLING_RATIO_MUL4: oversampling ratio multiple 4
    \arg          ADC_OVERSAMPLING_RATIO_MUL8: oversampling ratio multiple 8
    \arg          ADC_OVERSAMPLING_RATIO_MUL16: oversampling ratio multiple 16
    \arg          ADC_OVERSAMPLING_RATIO_MUL32: oversampling ratio multiple 32
    \arg          ADC_OVERSAMPLING_RATIO_MUL64: oversampling ratio multiple 64
    \arg          ADC_OVERSAMPLING_RATIO_MUL128: oversampling ratio multiple 128
    \arg          ADC_OVERSAMPLING_RATIO_MUL256: oversampling ratio multiple 256
    \param[out]   none
    \retval       none
*/
void adc_oversample_mode_config(uint32_t adc_periph, uint32_t mode, uint16_t shift, uint8_t ratio)
{
    if(ADC_OVERSAMPLING_ONE_CONVERT == mode) {
        ADC_OVSAMPCTL(adc_periph) |= (uint32_t)ADC_OVSAMPCTL_TOVS;
    } else {
        ADC_OVSAMPCTL(adc_periph) &= ~((uint32_t)ADC_OVSAMPCTL_TOVS);
    }
    /* config the shift and ratio */
    ADC_OVSAMPCTL(adc_periph) &= ~((uint32_t)(ADC_OVSAMPCTL_OVSR | ADC_OVSAMPCTL_OVSS));
    ADC_OVSAMPCTL(adc_periph) |= ((uint32_t)shift | (uint32_t)ratio);
}

/*!
    \brief        enable ADC oversample mode
    \param[in]    adc_periph: ADCx,x=0,1,2
    \param[out]   none
    \retval       none
*/
void adc_oversample_mode_enable(uint32_t adc_periph)
{
    ADC_OVSAMPCTL(adc_periph) |= ADC_OVSAMPCTL_OVSEN;
}

/*!
    \brief        disable ADC oversample mode
    \param[in]    adc_periph: ADCx,x=0,1,2
    \param[out]   none
    \retval       none
*/
void adc_oversample_mode_disable(uint32_t adc_periph)
{
    ADC_OVSAMPCTL(adc_periph) &= ~((uint32_t)ADC_OVSAMPCTL_OVSEN);
}

/*!
    \brief        enable DMA request
    \param[in]    adc_periph: ADCx,x=0,1,2
    \param[out]   none
    \retval       none
*/
void adc_dma_mode_enable(uint32_t adc_periph)
{
    /* enable DMA request */
    ADC_CTL1(adc_periph) |= (uint32_t)(ADC_CTL1_DMA);
}

/*!
    \brief      disable DMA request
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[out] none
    \retval     none
*/
void adc_dma_mode_disable(uint32_t adc_periph)
{
    /* disable DMA request */
    ADC_CTL1(adc_periph) &= ~((uint32_t)ADC_CTL1_DMA);
}

/*!
    \brief    when DMA=1, the DMA engine issues a request at end of each routine conversion
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[out] none
    \retval     none
*/
void adc_dma_request_after_last_enable(uint32_t adc_periph)
{
    ADC_CTL1(adc_periph) |= (uint32_t)(ADC_CTL1_DDM);
}

/*!
    \brief    the DMA engine is disabled after the end of transfer signal from DMA controller is detected
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[out] none
    \retval     none
*/
void adc_dma_request_after_last_disable(uint32_t adc_periph)
{
    ADC_CTL1(adc_periph) &= ~((uint32_t)ADC_CTL1_DDM);
}

/*!
    \brief        configure ADC discontinuous mode
    \param[in]    adc_periph: ADCx,x=0,1,2
    \param[in]    adc_sequence: select the sequence
                  only one parameter can be selected which is shown as below:
    \arg          ADC_ROUTINE_CHANNEL: routine sequence
    \arg          ADC_INSERTED_CHANNEL: inserted sequence
    \arg          ADC_CHANNEL_DISCON_DISABLE: disable discontinuous mode of routine & inserted channel
    \param[in]    length: number of conversions in discontinuous mode,the number can be 1..8
                  for routine sequence ,the number has no effect for inserted sequence
    \param[out]   none
    \retval       none
*/
void adc_discontinuous_mode_config(uint32_t adc_periph, uint8_t adc_sequence, uint8_t length)
{
    /* disable discontinuous mode of routine & inserted channel */
    ADC_CTL0(adc_periph) &= ~((uint32_t)(ADC_CTL0_DISRC | ADC_CTL0_DISIC));
    switch(adc_sequence) {
    case ADC_ROUTINE_CHANNEL:
        /* config the number of conversions in discontinuous mode  */
        ADC_CTL0(adc_periph) &= ~((uint32_t)ADC_CTL0_DISNUM);
        if((length <= 8U) && (length >= 1U)) {
            ADC_CTL0(adc_periph) |= CTL0_DISNUM(((uint32_t)length - ADC_CHANNEL_LENGTH_SUBTRACT_ONE));
        }
        /* enable routine sequence discontinuous mode */
        ADC_CTL0(adc_periph) |= (uint32_t)ADC_CTL0_DISRC;
        break;
    case ADC_INSERTED_CHANNEL:
        /* enable inserted sequence discontinuous mode */
        ADC_CTL0(adc_periph) |= (uint32_t)ADC_CTL0_DISIC;
        break;
    case ADC_CHANNEL_DISCON_DISABLE:
    /* disable discontinuous mode of routine & inserted channel */
    default:
        break;
    }
}

/*!
    \brief        configure the length of routine sequence or inserted sequence
    \param[in]    adc_periph: ADCx,x=0,1,2
    \param[in]    adc_sequence: select the sequence
                  only one parameter can be selected which is shown as below:
    \arg          ADC_ROUTINE_CHANNEL: routine sequence
    \arg          ADC_INSERTED_CHANNEL: inserted sequence
    \param[in]    length: the length of the channel
                  routine channel 1-16
                  inserted channel 1-4
    \param[out]   none
    \retval       none
*/
void adc_channel_length_config(uint32_t adc_periph, uint8_t adc_sequence, uint32_t length)
{
    switch(adc_sequence) {
    case ADC_ROUTINE_CHANNEL:
        if((length >= 1U) && (length <= 16U)) {
            ADC_RSQ0(adc_periph) &= ~((uint32_t)ADC_RSQ0_RL);
            ADC_RSQ0(adc_periph) |= RSQ0_RL((uint32_t)(length - ADC_CHANNEL_LENGTH_SUBTRACT_ONE));
        }
        break;
    case ADC_INSERTED_CHANNEL:
        if((length >= 1U) && (length <= 4U)) {
            ADC_ISQ(adc_periph) &= ~((uint32_t)ADC_ISQ_IL);
            ADC_ISQ(adc_periph) |= ISQ_IL((uint32_t)(length - ADC_CHANNEL_LENGTH_SUBTRACT_ONE));
        }
        break;
    default:
        break;
    }
}

/*!
    \brief        configure ADC routine channel
    \param[in]    adc_periph: ADCx,x=0,1,2
    \param[in]    rank: the routine sequence rank,this parameter must be between 0 to 15
    \param[in]    adc_channel: the selected ADC channel
                  only one parameter can be selected which is shown as below:
    \arg          ADC_CHANNEL_x(x=0..18): ADC channelx
    \param[in]    sample_time: the sample time value
                  only one parameter can be selected which is shown as below:
    \arg          ADC_SAMPLETIME_3: 3 cycles
    \arg          ADC_SAMPLETIME_15: 15 cycles
    \arg          ADC_SAMPLETIME_28: 28 cycles
    \arg          ADC_SAMPLETIME_56: 56 cycles
    \arg          ADC_SAMPLETIME_84: 84 cycles
    \arg          ADC_SAMPLETIME_112: 112 cycles
    \arg          ADC_SAMPLETIME_144: 144 cycles
    \arg          ADC_SAMPLETIME_480: 480 cycles
    \param[out]   none
    \retval       none
*/
void adc_routine_channel_config(uint32_t adc_periph, uint8_t rank, uint8_t adc_channel, uint32_t sample_time)
{
    uint32_t rsq, sampt;

    /* ADC routine sequence config */
    if(rank < ADC_ROUTINE_CHANNEL_RANK_SIX) {
        /* the routine sequence rank is smaller than six */
        rsq = ADC_RSQ2(adc_periph);
        rsq &=  ~((uint32_t)(ADC_RSQX_RSQN << (ADC_ROUTINE_CHANNEL_RANK_LENGTH * rank)));
        /* the channel number is written to these bits to select a channel as the nth conversion in the routine sequence */
        rsq |= ((uint32_t)adc_channel << (ADC_ROUTINE_CHANNEL_RANK_LENGTH * rank));
        ADC_RSQ2(adc_periph) = rsq;
    } else if(rank < ADC_ROUTINE_CHANNEL_RANK_TWELVE) {
        /* the routine sequence rank is smaller than twelve */
        rsq = ADC_RSQ1(adc_periph);
        rsq &= ~((uint32_t)(ADC_RSQX_RSQN << (ADC_ROUTINE_CHANNEL_RANK_LENGTH * (rank - ADC_ROUTINE_CHANNEL_RANK_SIX))));
        /* the channel number is written to these bits to select a channel as the nth conversion in the routine sequence */
        rsq |= ((uint32_t)adc_channel << (ADC_ROUTINE_CHANNEL_RANK_LENGTH * (rank - ADC_ROUTINE_CHANNEL_RANK_SIX)));
        ADC_RSQ1(adc_periph) = rsq;
    } else if(rank < ADC_ROUTINE_CHANNEL_RANK_SIXTEEN) {
        /* the routine sequence rank is smaller than sixteen */
        rsq = ADC_RSQ0(adc_periph);
        rsq &= ~((uint32_t)(ADC_RSQX_RSQN << (ADC_ROUTINE_CHANNEL_RANK_LENGTH * (rank - ADC_ROUTINE_CHANNEL_RANK_TWELVE))));
        /* the channel number is written to these bits to select a channel as the nth conversion in the routine sequence */
        rsq |= ((uint32_t)adc_channel << (ADC_ROUTINE_CHANNEL_RANK_LENGTH * (rank - ADC_ROUTINE_CHANNEL_RANK_TWELVE)));
        ADC_RSQ0(adc_periph) = rsq;
    } else {
    }

    /* ADC sampling time config */
    if(adc_channel < ADC_CHANNEL_SAMPLE_TEN) {
        /* the routine sequence rank is smaller than ten */
        sampt = ADC_SAMPT1(adc_periph);
        sampt &= ~((uint32_t)(ADC_SAMPTX_SPTN << (ADC_CHANNEL_SAMPLE_LENGTH * adc_channel)));
        /* channel sample time set*/
        sampt |= (uint32_t)(sample_time << (ADC_CHANNEL_SAMPLE_LENGTH * adc_channel));
        ADC_SAMPT1(adc_periph) = sampt;
    } else if(adc_channel <= ADC_CHANNEL_SAMPLE_EIGHTEEN) {
        /* the routine sequence rank is smaller than eighteen */
        sampt = ADC_SAMPT0(adc_periph);
        sampt &= ~((uint32_t)(ADC_SAMPTX_SPTN << (ADC_CHANNEL_SAMPLE_LENGTH * (adc_channel - ADC_CHANNEL_SAMPLE_TEN))));
        /* channel sample time set*/
        sampt |= (uint32_t)(sample_time << (ADC_CHANNEL_SAMPLE_LENGTH * (adc_channel - ADC_CHANNEL_SAMPLE_TEN)));
        ADC_SAMPT0(adc_periph) = sampt;
    } else {
    }
}

/*!
    \brief    configure ADC inserted channel
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  rank: the inserted sequence rank,this parameter must be between 0 to 3
    \param[in]  adc_channel: the selected ADC channel
                only one parameter can be selected which is shown as below:
    \arg        ADC_CHANNEL_x(x=0..18): ADC Channelx
    \param[in]  sample_time: The sample time value
                only one parameter can be selected which is shown as below:
    \arg        ADC_SAMPLETIME_3: 3 cycles
    \arg        ADC_SAMPLETIME_15: 15 cycles
    \arg        ADC_SAMPLETIME_28: 28 cycles
    \arg        ADC_SAMPLETIME_56: 56 cycles
    \arg        ADC_SAMPLETIME_84: 84 cycles
    \arg        ADC_SAMPLETIME_112: 112 cycles
    \arg        ADC_SAMPLETIME_144: 144 cycles
    \arg        ADC_SAMPLETIME_480: 480 cycles
    \param[out] none
    \retval     none
*/
void adc_inserted_channel_config(uint32_t adc_periph, uint8_t rank, uint8_t adc_channel, uint32_t sample_time)
{
    uint8_t inserted_length;
    uint32_t isq, sampt;

    /* get inserted sequence length */
    inserted_length = (uint8_t)GET_BITS(ADC_ISQ(adc_periph), 20U, 21U);
    /* the channel number is written to these bits to select a channel as the nth conversion in the inserted sequence */
    if(rank < 4U) {
        isq = ADC_ISQ(adc_periph);
        isq &= ~((uint32_t)(ADC_ISQ_ISQN << (ADC_INSERTED_CHANNEL_SHIFT_LENGTH - (inserted_length - rank) * ADC_INSERTED_CHANNEL_RANK_LENGTH)));
        isq |= ((uint32_t)adc_channel << (ADC_INSERTED_CHANNEL_SHIFT_LENGTH - (inserted_length - rank) * ADC_INSERTED_CHANNEL_RANK_LENGTH));
        ADC_ISQ(adc_periph) = isq;
    }

    /* ADC sampling time config */
    if(adc_channel < ADC_CHANNEL_SAMPLE_TEN) {
        /* the inserted sequence rank is smaller than ten */
        sampt = ADC_SAMPT1(adc_periph);
        sampt &= ~((uint32_t)(ADC_SAMPTX_SPTN << (ADC_CHANNEL_SAMPLE_LENGTH * adc_channel)));
        /* channel sample time set*/
        sampt |= (uint32_t) sample_time << (ADC_CHANNEL_SAMPLE_LENGTH * adc_channel);
        ADC_SAMPT1(adc_periph) = sampt;
    } else if(adc_channel <= ADC_CHANNEL_SAMPLE_EIGHTEEN) {
        /* the inserted sequence rank is smaller than eighteen */
        sampt = ADC_SAMPT0(adc_periph);
        sampt &= ~((uint32_t)(ADC_SAMPTX_SPTN << (ADC_CHANNEL_SAMPLE_LENGTH * (adc_channel - ADC_CHANNEL_SAMPLE_TEN))));
        /* channel sample time set*/
        sampt |= ((uint32_t)sample_time << (ADC_CHANNEL_SAMPLE_LENGTH * (adc_channel - ADC_CHANNEL_SAMPLE_TEN)));
        ADC_SAMPT0(adc_periph) = sampt;
    } else {
    }
}

/*!
    \brief      configure ADC inserted channel offset
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  inserted_channel : insert channel select
                only one parameter can be selected which is shown as below:
    \arg        ADC_INSERTED_CHANNEL_0: inserted channel0
    \arg        ADC_INSERTED_CHANNEL_1: inserted channel1
    \arg        ADC_INSERTED_CHANNEL_2: inserted channel2
    \arg        ADC_INSERTED_CHANNEL_3: inserted channel3
    \param[in]  offset : the offset data
    \param[out] none
    \retval     none
*/
void adc_inserted_channel_offset_config(uint32_t adc_periph, uint8_t inserted_channel, uint16_t offset)
{
    uint8_t inserted_length;
    uint32_t num = 0U;

    inserted_length = (uint8_t)GET_BITS(ADC_ISQ(adc_periph), 20U, 21U);
    num = ((uint32_t)ADC_OFFSET_LENGTH - ((uint32_t)inserted_length - (uint32_t)inserted_channel));

    if(num <= ADC_OFFSET_LENGTH) {
        /* calculate the offset of the register */
        num = num * ADC_OFFSET_SHIFT_LENGTH;
        /* config the offset of the selected channels */
        REG32((adc_periph) + 0x14U + num) = IOFFX_IOFF((uint32_t)offset);
    }
}

/*!
    \brief      configure ADC external trigger source
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  adc_sequence: select the sequence
                only one parameter can be selected which is shown as below:
    \arg        ADC_ROUTINE_CHANNEL: routine sequence
    \arg        ADC_INSERTED_CHANNEL: inserted sequence
    \param[in]  external_trigger_source: routine or inserted sequence trigger source
                for routine sequence:
                only one parameter can be selected which is shown as below:
    \arg        ADC_EXTTRIG_ROUTINE_T0_CH0: external trigger timer 0 CC0 event select for routine sequence
    \arg        ADC_EXTTRIG_ROUTINE_T0_CH1: external trigger timer 0 CC1 event select for routine sequence
    \arg        ADC_EXTTRIG_ROUTINE_T0_CH2: external trigger timer 0 CC2 event select for routine sequence
    \arg        ADC_EXTTRIG_ROUTINE_T1_CH1: external trigger timer 1 CC1 event select for routine sequence
    \arg        ADC_EXTTRIG_ROUTINE_T1_CH2: external trigger timer 1 CC2 event select for routine sequence
    \arg        ADC_EXTTRIG_ROUTINE_T1_CH3: external trigger timer 1 CC3 event select for routine sequence
    \arg        ADC_EXTTRIG_ROUTINE_T1_TRGO: external trigger timer 1 TRGO event select for routine sequence
    \arg        ADC_EXTTRIG_ROUTINE_T2_CH0 : external trigger timer 2 CC0 event select for routine sequence
    \arg        ADC_EXTTRIG_ROUTINE_T2_TRGO : external trigger timer 2 TRGO event select for routine sequence
    \arg        ADC_EXTTRIG_ROUTINE_T3_CH3: external trigger timer 3 CC3 event select for routine sequence
    \arg        ADC_EXTTRIG_ROUTINE_T4_CH0: external trigger timer 4 CC0 event select for routine sequence
    \arg        ADC_EXTTRIG_ROUTINE_T4_CH1: external trigger timer 4 CC1 event select for routine sequence
    \arg        ADC_EXTTRIG_ROUTINE_T4_CH2: external trigger timer 4 CC2 event select for routine sequence
    \arg        ADC_EXTTRIG_ROUTINE_T7_CH0: external trigger timer 7 CC0 event select for routine sequence
    \arg        ADC_EXTTRIG_ROUTINE_T7_TRGO: external trigger timer 7 TRGO event select for routine sequence
    \arg        ADC_EXTTRIG_ROUTINE_EXTI_11: external trigger extiline 11 select for routine sequence
                for inserted sequence:
                only one parameter can be selected which is shown as below:
    \arg        ADC_EXTTRIG_INSERTED_T0_CH3: timer0 capture compare 3
    \arg        ADC_EXTTRIG_INSERTED_T0_TRGO: timer0 TRGO event
    \arg        ADC_EXTTRIG_INSERTED_T1_CH0: timer1 capture compare 0
    \arg        ADC_EXTTRIG_INSERTED_T1_TRGO: timer1 TRGO event
    \arg        ADC_EXTTRIG_INSERTED_T2_CH1: timer2 capture compare 1
    \arg        ADC_EXTTRIG_INSERTED_T2_CH3: timer2 capture compare 3
    \arg        ADC_EXTTRIG_INSERTED_T3_CH0: timer3 capture compare 0
    \arg        ADC_EXTTRIG_INSERTED_T3_CH1: timer3 capture compare 1
    \arg        ADC_EXTTRIG_INSERTED_T3_CH2: timer3 capture compare 2
    \arg        ADC_EXTTRIG_INSERTED_T3_TRGO: timer3 capture compare TRGO
    \arg        ADC_EXTTRIG_INSERTED_T4_CH3: timer4 capture compare 3
    \arg        ADC_EXTTRIG_INSERTED_T4_TRGO: timer4 capture compare TRGO
    \arg        ADC_EXTTRIG_INSERTED_T7_CH1: timer7 capture compare 1
    \arg        ADC_EXTTRIG_INSERTED_T7_CH2: timer7 capture compare 2
    \arg        ADC_EXTTRIG_INSERTED_T7_CH3: timer7 capture compare 3
    \arg        ADC_EXTTRIG_INSERTED_EXTI_15: external interrupt line 15
    \param[out] none
    \retval     none
*/
void adc_external_trigger_source_config(uint32_t adc_periph, uint8_t adc_sequence, uint32_t external_trigger_source)
{
    switch(adc_sequence) {
    case ADC_ROUTINE_CHANNEL:
        /* configure ADC routine sequence external trigger source */
        ADC_CTL1(adc_periph) &= ~((uint32_t)ADC_CTL1_ETSRC);
        ADC_CTL1(adc_periph) |= (uint32_t)external_trigger_source;
        break;
    case ADC_INSERTED_CHANNEL:
        /* configure ADC inserted sequence external trigger source */
        ADC_CTL1(adc_periph) &= ~((uint32_t)ADC_CTL1_ETSIC);
        ADC_CTL1(adc_periph) |= (uint32_t)external_trigger_source;
        break;
    default:
        break;
    }
}

/*!
    \brief      enable ADC external trigger
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  adc_sequence: select the sequence
                only one parameter can be selected which is shown as below:
    \arg        ADC_ROUTINE_CHANNEL: routine sequence
    \arg        ADC_INSERTED_CHANNEL: inserted sequence
    \param[in]  trigger_mode: external trigger mode
                only one parameter can be selected which is shown as below:
    \arg        EXTERNAL_TRIGGER_DISABLE: external trigger disable
    \arg        EXTERNAL_TRIGGER_RISING: rising edge of external trigger
    \arg        EXTERNAL_TRIGGER_FALLING: falling edge of external trigger
    \arg        EXTERNAL_TRIGGER_RISING_FALLING: rising and falling edge of external trigger
    \param[out] none
    \retval     none
*/
void adc_external_trigger_config(uint32_t adc_periph, uint8_t adc_sequence, uint32_t trigger_mode)
{
    switch(adc_sequence) {
    case ADC_ROUTINE_CHANNEL:
        /* configure ADC routine sequence external trigger mode */
        ADC_CTL1(adc_periph) &= ~((uint32_t)ADC_CTL1_ETMRC);
        ADC_CTL1(adc_periph) |= (uint32_t)(trigger_mode << ROUTINE_TRIGGER_MODE);
        break;
    case ADC_INSERTED_CHANNEL:
        /* configure ADC inserted sequence external trigger mode */
        ADC_CTL1(adc_periph) &=  ~((uint32_t)ADC_CTL1_ETMIC);
        ADC_CTL1(adc_periph) |= (uint32_t)(trigger_mode << INSERTED_TRIGGER_MODE);
        break;
    default:
        break;
    }
}

/*!
    \brief      enable ADC software trigger
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  adc_sequence: select the sequence
                only one parameter can be selected which is shown as below:
    \arg        ADC_ROUTINE_CHANNEL: routine sequence
    \arg        ADC_INSERTED_CHANNEL: inserted sequence
    \param[out] none
    \retval     none
*/
void adc_software_trigger_enable(uint32_t adc_periph, uint8_t adc_sequence)
{
    switch(adc_sequence) {
    case ADC_ROUTINE_CHANNEL:
        /* enable ADC routine sequence software trigger */
        ADC_CTL1(adc_periph) |= (uint32_t)ADC_CTL1_SWRCST;
        break;
    case ADC_INSERTED_CHANNEL:
        /* enable ADC inserted sequence software trigger */
        ADC_CTL1(adc_periph) |= (uint32_t)ADC_CTL1_SWICST;
        break;
    default:
        break;
    }
}

/*!
    \brief      configure end of conversion mode
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  end_selection: end of conversion mode
                only one parameter can be selected which is shown as below:
    \arg        ADC_EOC_SET_SEQUENCE: only at the end of a sequence of routine conversions, the EOC bit is set.Overflow detection is disabled unless DMA=1.
    \arg        ADC_EOC_SET_CONVERSION: at the end of each routine conversion, the EOC bit is set.Overflow is detected automatically.
    \param[out] none
    \retval     none
*/
void adc_end_of_conversion_config(uint32_t adc_periph, uint8_t end_selection)
{
    switch(end_selection) {
    case ADC_EOC_SET_SEQUENCE:
        /* only at the end of a sequence of routine conversions, the EOC bit is set */
        ADC_CTL1(adc_periph) &= ~((uint32_t)ADC_CTL1_EOCM);
        break;
    case ADC_EOC_SET_CONVERSION:
        /* at the end of each routine conversion, the EOC bit is set.Overflow is detected automatically */
        ADC_CTL1(adc_periph) |= (uint32_t)(ADC_CTL1_EOCM);
        break;
    default:
        break;
    }
}

/*!
    \brief      read ADC routine data register
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  none
    \param[out] none
    \retval     the conversion value
*/
uint16_t adc_routine_data_read(uint32_t adc_periph)
{
    return (uint16_t)(ADC_RDATA(adc_periph));
}

/*!
    \brief      read ADC inserted data register
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  inserted_channel : insert channel select
                only one parameter can be selected which is shown as below:
    \arg        ADC_INSERTED_CHANNEL_0: inserted channel0
    \arg        ADC_INSERTED_CHANNEL_1: inserted channel1
    \arg        ADC_INSERTED_CHANNEL_2: inserted channel2
    \arg        ADC_INSERTED_CHANNEL_3: inserted channel3
    \param[out] none
    \retval     the conversion value
*/
uint16_t adc_inserted_data_read(uint32_t adc_periph, uint8_t inserted_channel)
{
    uint32_t idata;
    /* read the data of the selected channel */
    switch(inserted_channel) {
    case ADC_INSERTED_CHANNEL_0:
        /* read the data of channel 0 */
        idata = ADC_IDATA0(adc_periph);
        break;
    case ADC_INSERTED_CHANNEL_1:
        /* read the data of channel 1 */
        idata = ADC_IDATA1(adc_periph);
        break;
    case ADC_INSERTED_CHANNEL_2:
        /* read the data of channel 2 */
        idata = ADC_IDATA2(adc_periph);
        break;
    case ADC_INSERTED_CHANNEL_3:
        /* read the data of channel 3 */
        idata = ADC_IDATA3(adc_periph);
        break;
    default:
        idata = 0U;
        break;
    }
    return (uint16_t)idata;
}

/*!
    \brief      disable ADC analog watchdog single channel
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[out] none
    \retval     none
*/
void adc_watchdog_single_channel_disable(uint32_t adc_periph)
{
    ADC_CTL0(adc_periph) &= ~((uint32_t)ADC_CTL0_WDSC);
}

/*!
    \brief      enable ADC analog watchdog single channel
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  adc_channel: the selected ADC channel
                only one parameter can be selected which is shown as below:
    \arg        ADC_CHANNEL_x: ADC Channelx(x=0..18)
    \param[out] none
    \retval     none
*/
void adc_watchdog_single_channel_enable(uint32_t adc_periph, uint8_t adc_channel)
{
    ADC_CTL0(adc_periph) &= ~((uint32_t)ADC_CTL0_WDCHSEL);

    /* analog watchdog channel select */
    ADC_CTL0(adc_periph) |= (uint32_t)adc_channel;
    ADC_CTL0(adc_periph) |= (uint32_t) ADC_CTL0_WDSC;
}

/*!
    \brief      configure ADC analog watchdog sequence channel
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  adc_sequence: the sequence use analog watchdog
                only one parameter can be selected which is shown as below:
    \arg        ADC_ROUTINE_CHANNEL: routine sequence
    \arg        ADC_INSERTED_CHANNEL: inserted sequence
    \arg        ADC_ROUTINE_INSERTED_CHANNEL: both routine and inserted sequence
    \param[out] none
    \retval     none
*/
void adc_watchdog_sequence_channel_enable(uint32_t adc_periph, uint8_t adc_sequence)
{
    ADC_CTL0(adc_periph) &= ~((uint32_t)(ADC_CTL0_RWDEN | ADC_CTL0_IWDEN | ADC_CTL0_WDSC));
    /* select the sequence */
    switch(adc_sequence) {
    case ADC_ROUTINE_CHANNEL:
        /* routine channel analog watchdog enable */
        ADC_CTL0(adc_periph) |= (uint32_t) ADC_CTL0_RWDEN;
        break;
    case ADC_INSERTED_CHANNEL:
        /* inserted channel analog watchdog enable */
        ADC_CTL0(adc_periph) |= (uint32_t) ADC_CTL0_IWDEN;
        break;
    case ADC_ROUTINE_INSERTED_CHANNEL:
        /* routine and inserted channel analog watchdog enable */
        ADC_CTL0(adc_periph) |= (uint32_t)(ADC_CTL0_RWDEN | ADC_CTL0_IWDEN);
        break;
    default:
        break;
    }
}

/*!
    \brief      disable ADC analog watchdog
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  adc_sequence: the sequence use analog watchdog
                only one parameter can be selected which is shown as below:
    \arg        ADC_ROUTINE_CHANNEL: routine sequence
    \arg        ADC_INSERTED_CHANNEL: inserted sequence
    \arg        ADC_ROUTINE_INSERTED_CHANNEL: both routine and inserted sequence
    \param[out] none
    \retval     none
*/
void adc_watchdog_disable(uint32_t adc_periph, uint8_t adc_sequence)
{
    /* select the sequence */
    switch(adc_sequence) {
    case ADC_ROUTINE_CHANNEL:
        /* disable ADC analog watchdog routine sequence */
        ADC_CTL0(adc_periph) &=  ~((uint32_t)ADC_CTL0_RWDEN);
        break;
    case ADC_INSERTED_CHANNEL:
        /* disable ADC analog watchdog inserted sequence */
        ADC_CTL0(adc_periph) &=  ~((uint32_t)ADC_CTL0_IWDEN);
        break;
    case ADC_ROUTINE_INSERTED_CHANNEL:
        /* disable ADC analog watchdog routine and inserted sequence */
        ADC_CTL0(adc_periph) &=  ~((uint32_t)(ADC_CTL0_RWDEN | ADC_CTL0_IWDEN));
        break;
    default:
        break;
    }
}

/*!
    \brief      configure ADC analog watchdog threshold
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  low_threshold: analog watchdog low threshold,0..4095
    \param[in]  high_threshold: analog watchdog high threshold,0..4095
    \param[out] none
    \retval     none
*/
void adc_watchdog_threshold_config(uint32_t adc_periph, uint16_t low_threshold, uint16_t high_threshold)
{
    /* configure ADC analog watchdog low threshold */
    ADC_WDLT(adc_periph) = (uint32_t)WDLT_WDLT(low_threshold);
    /* configure ADC analog watchdog high threshold */
    ADC_WDHT(adc_periph) = (uint32_t)WDHT_WDHT(high_threshold);
}

/*!
    \brief      get the ADC flag bits
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  adc_flag: the adc flag bits
                only one parameter can be selected which is shown as below:
    \arg        ADC_FLAG_WDE: analog watchdog event flag
    \arg        ADC_FLAG_EOC: end of sequence conversion flag
    \arg        ADC_FLAG_EOIC: end of inserted sequence conversion flag
    \arg        ADC_FLAG_STIC: start flag of inserted sequence
    \arg        ADC_FLAG_STRC: start flag of routine sequence
    \arg        ADC_FLAG_ROVF: routine data register overflow flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus adc_flag_get(uint32_t adc_periph, uint32_t adc_flag)
{
    FlagStatus reval = RESET;
    if(ADC_STAT(adc_periph) & adc_flag) {
        reval = SET;
    }
    return reval;

}

/*!
    \brief      clear the ADC flag bits
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  adc_flag: the adc flag bits
                only one parameter can be selected which is shown as below:
    \arg        ADC_FLAG_WDE: analog watchdog event flag
    \arg        ADC_FLAG_EOC: end of sequence conversion flag
    \arg        ADC_FLAG_EOIC: end of inserted sequence conversion flag
    \arg        ADC_FLAG_STIC: start flag of inserted sequence
    \arg        ADC_FLAG_STRC: start flag of routine sequence
    \arg        ADC_FLAG_ROVF: routine data register overflow flag
    \param[out] none
    \retval     none
*/
void adc_flag_clear(uint32_t adc_periph, uint32_t adc_flag)
{
    ADC_STAT(adc_periph) &= ~((uint32_t)adc_flag);
}

/*!
    \brief      get the bit state of ADCx software start conversion
    \param[in]  adc_periph: ADCx, x=0,1,2 only one among these parameters can be selected
    \param[in]  none
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus adc_routine_software_startconv_flag_get(uint32_t adc_periph)
{
    FlagStatus reval = RESET;
    if((uint32_t)RESET != (ADC_STAT(adc_periph) & ADC_STAT_STRC)) {
        reval = SET;
    }
    return reval;
}

/*!
    \brief      get the bit state of ADCx software inserted channel start conversion
    \param[in]  adc_periph: ADCx, x=0,1,2 only one among these parameters can be selected
    \param[in]  none
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus adc_inserted_software_startconv_flag_get(uint32_t adc_periph)
{
    FlagStatus reval = RESET;
    if((uint32_t)RESET != (ADC_STAT(adc_periph) & ADC_STAT_STIC)) {
        reval = SET;
    }
    return reval;
}

/*!
    \brief      get the ADC interrupt bits
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  adc_interrupt: the adc interrupt bits
                only one parameter can be selected which is shown as below:
    \arg        ADC_INT_FLAG_WDE: analog watchdog interrupt
    \arg        ADC_INT_FLAG_EOC: end of sequence conversion interrupt
    \arg        ADC_INT_FLAG_EOIC: end of inserted sequence conversion interrupt
    \arg        ADC_INT_FLAG_ROVF: routine data register overflow interrupt
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus adc_interrupt_flag_get(uint32_t adc_periph, uint32_t adc_interrupt)
{
    FlagStatus interrupt_flag = RESET;
    uint32_t state;
    /* check the interrupt bits */
    switch(adc_interrupt) {
    case ADC_INT_FLAG_WDE:
        /* get the ADC analog watchdog interrupt bits */
        state = ADC_STAT(adc_periph) & ADC_STAT_WDE;
        if((ADC_CTL0(adc_periph) & ADC_CTL0_WDEIE) && state) {
            interrupt_flag = SET;
        }
        break;
    case ADC_INT_FLAG_EOC:
        /* get the ADC end of sequence conversion interrupt bits */
        state = ADC_STAT(adc_periph) & ADC_STAT_EOC;
        if((ADC_CTL0(adc_periph) & ADC_CTL0_EOCIE) && state) {
            interrupt_flag = SET;
        }
        break;
    case ADC_INT_FLAG_EOIC:
        /* get the ADC end of inserted sequence conversion interrupt bits */
        state = ADC_STAT(adc_periph) & ADC_STAT_EOIC;
        if((ADC_CTL0(adc_periph) & ADC_CTL0_EOICIE) && state) {
            interrupt_flag = SET;
        }
        break;
    case ADC_INT_FLAG_ROVF:
        /* get the ADC routine data register overflow interrupt bits */
        state = ADC_STAT(adc_periph) & ADC_STAT_ROVF;
        if((ADC_CTL0(adc_periph) & ADC_CTL0_ROVFIE) && state) {
            interrupt_flag = SET;
        }
        break;
    default:
        break;
    }
    return interrupt_flag;
}

/*!
    \brief      clear the ADC flag
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  adc_interrupt: the adc status flag
                only one parameter can be selected which is shown as below:
    \arg        ADC_INT_FLAG_WDE: analog watchdog interrupt
    \arg        ADC_INT_FLAG_EOC: end of sequence conversion interrupt
    \arg        ADC_INT_FLAG_EOIC: end of inserted sequence conversion interrupt
    \arg        ADC_INT_FLAG_ROVF: routine data register overflow interrupt
    \param[out] none
    \retval     none
*/
void adc_interrupt_flag_clear(uint32_t adc_periph, uint32_t adc_interrupt)
{
    ADC_STAT(adc_periph) &= ~((uint32_t)adc_interrupt);
}

/*!
    \brief      enable ADC interrupt
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  adc_interrupt: the adc interrupt flag
                only one parameter can be selected which is shown as below:
    \arg        ADC_INT_WDE: analog watchdog interrupt flag
    \arg        ADC_INT_EOC: end of sequence conversion interrupt flag
    \arg        ADC_INT_EOIC: end of inserted sequence conversion interrupt flag
    \arg        ADC_INT_ROVF: routine data register overflow interrupt flag
    \param[out] none
    \retval     none
*/
void adc_interrupt_enable(uint32_t adc_periph, uint32_t adc_interrupt)
{
    switch(adc_interrupt) {
    case ADC_INT_WDE:
        /* enable analog watchdog interrupt */
        ADC_CTL0(adc_periph) |= (uint32_t) ADC_CTL0_WDEIE;
        break;
    case ADC_INT_EOC:
        /* enable end of sequence conversion interrupt */
        ADC_CTL0(adc_periph) |= (uint32_t) ADC_CTL0_EOCIE;
        break;
    case ADC_INT_EOIC:
        /* enable end of inserted sequence conversion interrupt */
        ADC_CTL0(adc_periph) |= (uint32_t) ADC_CTL0_EOICIE;
        break;
    case ADC_INT_ROVF:
        ADC_CTL0(adc_periph) |= (uint32_t) ADC_CTL0_ROVFIE;
        break;
    default:
        break;
    }
}

/*!
    \brief      disable ADC interrupt
    \param[in]  adc_periph: ADCx,x=0,1,2
    \param[in]  adc_flag: the adc interrupt flag
                only one parameter can be selected which is shown as below:
      \arg      ADC_INT_WDE: analog watchdog interrupt flag
      \arg      ADC_INT_EOC: end of sequence conversion interrupt flag
      \arg      ADC_INT_EOIC: end of inserted sequence conversion interrupt flag
      \arg      ADC_INT_ROVF: routine data register overflow interrupt flag
    \param[out] none
    \retval     none
*/
void adc_interrupt_disable(uint32_t adc_periph, uint32_t adc_interrupt)
{
    switch(adc_interrupt) {
    /* select the interrupt source */
    case ADC_INT_WDE:
        ADC_CTL0(adc_periph) &= ~((uint32_t)ADC_CTL0_WDEIE);
        break;
    case ADC_INT_EOC:
        ADC_CTL0(adc_periph) &= ~((uint32_t)ADC_CTL0_EOCIE);
        break;
    case ADC_INT_EOIC:
        ADC_CTL0(adc_periph) &= ~((uint32_t)ADC_CTL0_EOICIE);
        break;
    case ADC_INT_ROVF:
        ADC_CTL0(adc_periph) &= ~((uint32_t)ADC_CTL0_ROVFIE);
        break;
    default:
        break;
    }
}

/*!
    \brief      configure the ADC sync mode
    \param[in]  sync_mode: ADC sync mode
                only one parameter can be selected which is shown as below:
    \arg        ADC_SYNC_MODE_INDEPENDENT: all the ADCs work independently
    \arg        ADC_DAUL_ROUTINE_PARALLEL_INSERTED_PARALLEL: ADC0 and ADC1 work in combined routine parallel & inserted parallel mode
    \arg        ADC_DAUL_ROUTINE_PARALLEL_INSERTED_ROTATION: ADC0 and ADC1 work in combined routine parallel & trigger rotation mode
    \arg        ADC_DAUL_INSERTED_PARALLEL: ADC0 and ADC1 work in inserted parallel mode
    \arg        ADC_DAUL_ROUTINE_PARALLEL: ADC0 and ADC1 work in routine parallel mode
    \arg        ADC_DAUL_ROUTINE_FOLLOW_UP: ADC0 and ADC1 work in follow-up mode
    \arg        ADC_DAUL_INSERTED_TRRIGGER_ROTATION: ADC0 and ADC1 work in trigger rotation mode
    \arg        ADC_ALL_ROUTINE_PARALLEL_INSERTED_PARALLEL: all ADCs work in combined routine parallel & inserted parallel mode
    \arg        ADC_ALL_ROUTINE_PARALLEL_INSERTED_ROTATION: all ADCs work in combined routine parallel & trigger rotation mode
    \arg        ADC_ALL_INSERTED_PARALLEL: all ADCs work in inserted parallel mode
    \arg        ADC_ALL_ROUTINE_PARALLEL: all ADCs work in routine parallel mode
    \arg        ADC_ALL_ROUTINE_FOLLOW_UP: all ADCs work in follow-up mode
    \arg        ADC_ALL_INSERTED_TRRIGGER_ROTATION: all ADCs work in trigger rotation mode
    \param[out] none
    \retval     none
*/
void adc_sync_mode_config(uint32_t sync_mode)
{
    ADC_SYNCCTL &= ~(ADC_SYNCCTL_SYNCM);
    ADC_SYNCCTL |= sync_mode;
}

/*!
    \brief      configure the delay between 2 sampling phases in ADC sync modes
    \param[in]  sample_delay:  the delay between 2 sampling phases in ADC sync modes
                only one parameter can be selected which is shown as below:
      \arg      ADC_SYNC_DELAY_xCYCLE: x=5..20,the delay between 2 sampling phases in ADC sync modes is x ADC clock cycles
    \param[out] none
    \retval     none
*/
void adc_sync_delay_config(uint32_t sample_delay)
{
    ADC_SYNCCTL &= ~(ADC_SYNCCTL_SYNCDLY);
    ADC_SYNCCTL |= sample_delay;
}

/*!
    \brief      configure ADC sync DMA mode selection
    \param[in]  dma_mode:  ADC sync DMA mode
                only one parameter can be selected which is shown as below:
    \arg        ADC_SYNC_DMA_DISABLE: ADC sync DMA disabled
    \arg        ADC_SYNC_DMA_MODE0: ADC sync DMA mode 0
    \arg        ADC_SYNC_DMA_MODE1: ADC sync DMA mode 1
    \param[out] none
    \retval     none
*/
void adc_sync_dma_config(uint32_t dma_mode)
{
    ADC_SYNCCTL &= ~(ADC_SYNCCTL_SYNCDMA);
    ADC_SYNCCTL |= dma_mode;
}

/*!
    \brief      configure ADC sync DMA engine is disabled after the end of transfer signal from DMA controller is detected
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_sync_dma_request_after_last_enable(void)
{
    ADC_SYNCCTL |= ADC_SYNCCTL_SYNCDDM;
}

/*!
    \brief      configure ADC sync DMA engine issues requests according to the SYNCDMA bits
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_sync_dma_request_after_last_disable(void)
{
    ADC_SYNCCTL &= ~(ADC_SYNCCTL_SYNCDDM);
}

/*!
    \brief      read ADC sync routine data register
    \param[in]  none
    \param[out] none
    \retval     sync routine data
*/
uint32_t adc_sync_routine_data_read(void)
{
    return (uint32_t)ADC_SYNCDATA;
}
