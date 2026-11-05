/*!
    \file    gd32f4xx_dac.c
    \brief   DAC driver

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

#include "gd32f4xx_dac.h"

/* DAC register bit offset */
#define OUT1_REG_OFFSET           ((uint32_t)0x00000010U)
#define DH_12BIT_OFFSET           ((uint32_t)0x00000010U)
#define DH_8BIT_OFFSET            ((uint32_t)0x00000008U)

#define DAC_STAT_FLAG_MASK0       (DAC_FLAG_DDUDR0 | DAC_FLAG_DDUDR1)
#define DAC_INT_EN_MASK0          (DAC_INT_DDUDR0 | DAC_INT_DDUDR1)
#define DAC_INT_FLAG_MASK0        (DAC_INT_FLAG_DDUDR0 | DAC_INT_FLAG_DDUDR1)

/*!
    \brief      deinitialize DAC
    \param[in]  dac_periph: DACx(x=0)
    \param[out] none
    \retval     none
*/
void dac_deinit(uint32_t dac_periph)
{
    switch(dac_periph){
        case DAC0:
            /* reset DAC0 */
            rcu_periph_reset_enable(RCU_DACRST);
            rcu_periph_reset_disable(RCU_DACRST);
            break;
        default:
            break;
    }
}

/*!
    \brief      enable DAC
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_enable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_CTL0(dac_periph) |= (uint32_t)DAC_CTL0_DEN0;
    }else if(DAC_OUT1 == dac_out){
        DAC_CTL0(dac_periph) |= (uint32_t)DAC_CTL0_DEN1;
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      disable DAC
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_disable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DEN0);
    }else if(DAC_OUT1 == dac_out){
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DEN1);
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      enable DAC DMA function
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_dma_enable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_CTL0(dac_periph) |= (uint32_t)DAC_CTL0_DDMAEN0;
    }else if(DAC_OUT1 == dac_out){
        DAC_CTL0(dac_periph) |= (uint32_t)DAC_CTL0_DDMAEN1;
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      disable DAC DMA function
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_dma_disable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DDMAEN0);
    }else if(DAC_OUT1 == dac_out){
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DDMAEN1);
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      enable DAC output buffer
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_output_buffer_enable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DBOFF0);
    }else if(DAC_OUT1 == dac_out){
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DBOFF1);
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      disable DAC output buffer
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_output_buffer_disable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_CTL0(dac_periph) |= (uint32_t)DAC_CTL0_DBOFF0;
    }else if(DAC_OUT1 == dac_out){
        DAC_CTL0(dac_periph) |= (uint32_t)DAC_CTL0_DBOFF1;
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      get DAC output value
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0,1)
    \param[out] none
    \retval     DAC output data: 0~4095
*/
uint16_t dac_output_value_get(uint32_t dac_periph, uint8_t dac_out)
{
    uint16_t data = 0U;

    if(DAC_OUT0 == dac_out){
        /* store the DACx_OUT0 output value */
        data = (uint16_t)DAC_OUT0_DO(dac_periph);
    }else if(DAC_OUT1 == dac_out){
        /* store the DACx_OUT1 output value */
        data = (uint16_t)DAC_OUT1_DO(dac_periph);
    }else{
        /* illegal parameters */
    }

    return data;
}

/*!
    \brief      set DAC data holding register value
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0,1)
    \param[in]  dac_align: DAC data alignment mode
                only one parameter can be selected which is shown as below:
      \arg        DAC_ALIGN_12B_R: 12-bit right-aligned data
      \arg        DAC_ALIGN_12B_L: 12-bit left-aligned data
      \arg        DAC_ALIGN_8B_R: 8-bit right-aligned data
    \param[in]  data: data to be loaded(0~4095)
    \param[out] none
    \retval     none
*/
void dac_data_set(uint32_t dac_periph, uint8_t dac_out, uint32_t dac_align, uint16_t data)
{
    /* DAC_OUT0 data alignment */
    if(DAC_OUT0 == dac_out){
        switch(dac_align){
        /* 12-bit right-aligned data */
        case DAC_ALIGN_12B_R:
            DAC_OUT0_R12DH(dac_periph) = data;
            break;
        /* 12-bit left-aligned data */
        case DAC_ALIGN_12B_L:
            DAC_OUT0_L12DH(dac_periph) = data;
            break;
        /* 8-bit right-aligned data */
        case DAC_ALIGN_8B_R:
            DAC_OUT0_R8DH(dac_periph) = data;
            break;
        default:
            break;
        }
    }else if(DAC_OUT1 == dac_out){
        /* DAC_OUT1 data alignment */
        switch(dac_align){
        /* 12-bit right-aligned data */
        case DAC_ALIGN_12B_R:
            DAC_OUT1_R12DH(dac_periph) = data;
            break;
        /* 12-bit left-aligned data */
        case DAC_ALIGN_12B_L:
            DAC_OUT1_L12DH(dac_periph) = data;
            break;
        /* 8-bit right-aligned data */
        case DAC_ALIGN_8B_R:
            DAC_OUT1_R8DH(dac_periph) = data;
            break;
        default:
            break;
        }
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      enable DAC trigger
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_trigger_enable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_CTL0(dac_periph) |= (uint32_t)DAC_CTL0_DTEN0;
    }else if(DAC_OUT1 == dac_out){
        DAC_CTL0(dac_periph) |= (uint32_t)DAC_CTL0_DTEN1;
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      disable DAC trigger
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_trigger_disable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DTEN0);
    }else if(DAC_OUT1 == dac_out){
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DTEN1);
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      configure DAC trigger source
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0,1)
    \param[in]  triggersource: external trigger of DAC
                only one parameter can be selected which is shown as below:
      \arg        DAC_TRIGGER_T1_TRGO: TIMER1 TRGO
      \arg        DAC_TRIGGER_T3_TRGO: TIMER3 TRGO
      \arg        DAC_TRIGGER_T4_TRGO: TIMER4 TRGO
      \arg        DAC_TRIGGER_T5_TRGO: TIMER5 TRGO
      \arg        DAC_TRIGGER_T6_TRGO: TIMER6 TRGO
      \arg        DAC_TRIGGER_T7_TRGO: TIMER7 TRGO
      \arg        DAC_TRIGGER_EXTI_9: EXTI interrupt line9 event
      \arg        DAC_TRIGGER_SOFTWARE: software trigger
    \param[out] none
    \retval     none
*/
void dac_trigger_source_config(uint32_t dac_periph, uint8_t dac_out, uint32_t triggersource)
{
    if(DAC_OUT0 == dac_out){
        /* configure DACx_OUT0 trigger source */
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DTSEL0);
        DAC_CTL0(dac_periph) |= triggersource;
    }else if(DAC_OUT1 == dac_out){
        /* configure DACx_OUT1 trigger source */
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DTSEL1);
        DAC_CTL0(dac_periph) |= (triggersource << OUT1_REG_OFFSET);
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      enable DAC software trigger
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0,1)
    \retval     none
*/
void dac_software_trigger_enable(uint32_t dac_periph, uint8_t dac_out)
{
    if(DAC_OUT0 == dac_out){
        DAC_SWT(dac_periph) |= (uint32_t)DAC_SWT_SWTR0;
    }else if(DAC_OUT1 == dac_out){
        DAC_SWT(dac_periph) |= (uint32_t)DAC_SWT_SWTR1;
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      configure DAC wave mode
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0,1)
    \param[in]  wave_mode: DAC wave mode
                only one parameter can be selected which is shown as below:
      \arg        DAC_WAVE_DISABLE: wave mode disable
      \arg        DAC_WAVE_MODE_LFSR: LFSR noise mode
      \arg        DAC_WAVE_MODE_TRIANGLE: triangle noise mode
    \param[out] none
    \retval     none
*/
void dac_wave_mode_config(uint32_t dac_periph, uint8_t dac_out, uint32_t wave_mode)
{
    if(DAC_OUT0 == dac_out){
        /* configure DACx_OUT0 wave mode */
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DWM0);
        DAC_CTL0(dac_periph) |= wave_mode;
    }else if(DAC_OUT1 == dac_out){
        /* configure DACx_OUT1 wave mode */
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DWM1);
        DAC_CTL0(dac_periph) |= (wave_mode << OUT1_REG_OFFSET);
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      configure DAC LFSR noise mode
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0,1)
    \param[in]  unmask_bits: LFSR noise unmask bits
                only one parameter can be selected which is shown as below:
      \arg        DAC_LFSR_BIT0: unmask the LFSR bit0
      \arg        DAC_LFSR_BITS1_0: unmask the LFSR bits[1:0]
      \arg        DAC_LFSR_BITS2_0: unmask the LFSR bits[2:0]
      \arg        DAC_LFSR_BITS3_0: unmask the LFSR bits[3:0]
      \arg        DAC_LFSR_BITS4_0: unmask the LFSR bits[4:0]
      \arg        DAC_LFSR_BITS5_0: unmask the LFSR bits[5:0]
      \arg        DAC_LFSR_BITS6_0: unmask the LFSR bits[6:0]
      \arg        DAC_LFSR_BITS7_0: unmask the LFSR bits[7:0]
      \arg        DAC_LFSR_BITS8_0: unmask the LFSR bits[8:0]
      \arg        DAC_LFSR_BITS9_0: unmask the LFSR bits[9:0]
      \arg        DAC_LFSR_BITS10_0: unmask the LFSR bits[10:0]
      \arg        DAC_LFSR_BITS11_0: unmask the LFSR bits[11:0]
    \param[out] none
    \retval     none
*/
void dac_lfsr_noise_config(uint32_t dac_periph, uint8_t dac_out, uint32_t unmask_bits)
{
    if(DAC_OUT0 == dac_out){
        /* configure DACx_OUT0 LFSR noise mode */
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DWBW0);
        DAC_CTL0(dac_periph) |= unmask_bits;
    }else if(DAC_OUT1 == dac_out){
        /* configure DACx_OUT1 LFSR noise mode */
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DWBW1);
        DAC_CTL0(dac_periph) |= (unmask_bits << OUT1_REG_OFFSET);
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      configure DAC triangle noise mode
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_out: DAC_OUTx(x=0,1)
    \param[in]  amplitude: the amplitude of the triangle
                only one parameter can be selected which is shown as below:
      \arg        DAC_TRIANGLE_AMPLITUDE_1: triangle amplitude is 1
      \arg        DAC_TRIANGLE_AMPLITUDE_3: triangle amplitude is 3
      \arg        DAC_TRIANGLE_AMPLITUDE_7: triangle amplitude is 7
      \arg        DAC_TRIANGLE_AMPLITUDE_15: triangle amplitude is 15
      \arg        DAC_TRIANGLE_AMPLITUDE_31: triangle amplitude is 31
      \arg        DAC_TRIANGLE_AMPLITUDE_63: triangle amplitude is 63
      \arg        DAC_TRIANGLE_AMPLITUDE_127: triangle amplitude is 127
      \arg        DAC_TRIANGLE_AMPLITUDE_255: triangle amplitude is 255
      \arg        DAC_TRIANGLE_AMPLITUDE_511: triangle amplitude is 511
      \arg        DAC_TRIANGLE_AMPLITUDE_1023: triangle amplitude is 1023
      \arg        DAC_TRIANGLE_AMPLITUDE_2047: triangle amplitude is 2047
      \arg        DAC_TRIANGLE_AMPLITUDE_4095: triangle amplitude is 4095
    \param[out] none
    \retval     none
*/
void dac_triangle_noise_config(uint32_t dac_periph, uint8_t dac_out, uint32_t amplitude)
{
    if(DAC_OUT0 == dac_out){
        /* configure DACx_OUT0 triangle noise mode */
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DWBW0);
        DAC_CTL0(dac_periph) |= amplitude;
    }else if(DAC_OUT1 == dac_out){
        /* configure DACx_OUT1 triangle noise mode */
        DAC_CTL0(dac_periph) &= (uint32_t)(~DAC_CTL0_DWBW1);
        DAC_CTL0(dac_periph) |= (amplitude << OUT1_REG_OFFSET);
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      enable DAC concurrent mode
    \param[in]  dac_periph: DACx(x=0)
    \param[out] none
    \retval     none
*/
void dac_concurrent_enable(uint32_t dac_periph)
{
    uint32_t ctl = 0U;

    ctl = (uint32_t)(DAC_CTL0_DEN0 | DAC_CTL0_DEN1);
    DAC_CTL0(dac_periph) |= (uint32_t)ctl;
}

/*!
    \brief      disable DAC concurrent mode
    \param[in]  dac_periph: DACx(x=0)
    \param[out] none
    \retval     none
*/
void dac_concurrent_disable(uint32_t dac_periph)
{
    uint32_t ctl = 0U;

    ctl = (uint32_t)(DAC_CTL0_DEN0 | DAC_CTL0_DEN1);
    DAC_CTL0(dac_periph) &= (uint32_t)(~ctl);
}

/*!
    \brief      enable DAC concurrent software trigger
    \param[in]  dac_periph: DACx(x=0)
    \param[out] none
    \retval     none
*/
void dac_concurrent_software_trigger_enable(uint32_t dac_periph)
{
    uint32_t swt = 0U;

    swt = (uint32_t)(DAC_SWT_SWTR0 | DAC_SWT_SWTR1);
    DAC_SWT(dac_periph) |= (uint32_t)swt; 
}

/*!
    \brief      enable DAC concurrent buffer function
    \param[in]  dac_periph: DACx(x=0)
    \param[out] none
    \retval     none
*/
void dac_concurrent_output_buffer_enable(uint32_t dac_periph)
{
    uint32_t ctl = 0U;

    ctl = (uint32_t)(DAC_CTL0_DBOFF0 | DAC_CTL0_DBOFF1);
    DAC_CTL0(dac_periph) &= (uint32_t)(~ctl);
}

/*!
    \brief      disable DAC concurrent buffer function
    \param[in]  dac_periph: DACx(x=0)
    \param[out] none
    \retval     none
*/
void dac_concurrent_output_buffer_disable(uint32_t dac_periph)
{
    uint32_t ctl = 0U;

    ctl = (uint32_t)(DAC_CTL0_DBOFF0 | DAC_CTL0_DBOFF1);
    DAC_CTL0(dac_periph) |= (uint32_t)ctl;
}

/*!
    \brief      set DAC concurrent mode data holding register value
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  dac_align: DAC data alignment mode
                only one parameter can be selected which is shown as below:
      \arg        DAC_ALIGN_12B_R: 12-bit right-aligned data
      \arg        DAC_ALIGN_12B_L: 12-bit left-aligned data
      \arg        DAC_ALIGN_8B_R: 8-bit right-aligned data
    \param[in]  data0: data to be loaded(0~4095)
    \param[in]  data1: data to be loaded(0~4095)
    \param[out] none
    \retval     none
*/
void dac_concurrent_data_set(uint32_t dac_periph, uint32_t dac_align, uint16_t data0, uint16_t data1)
{
    uint32_t data = 0U;

    switch(dac_align){
    /* 12-bit right-aligned data */
    case DAC_ALIGN_12B_R:
        data = (uint32_t)(((uint32_t)data1 << DH_12BIT_OFFSET) | data0);
        DACC_R12DH(dac_periph) = (uint32_t)data;
        break;
    /* 12-bit left-aligned data */
    case DAC_ALIGN_12B_L:
        data = (uint32_t)(((uint32_t)data1 << DH_12BIT_OFFSET) | data0);
        DACC_L12DH(dac_periph) = (uint32_t)data;
        break;
    /* 8-bit right-aligned data */
    case DAC_ALIGN_8B_R:
        data = (uint32_t)(((uint32_t)data1 << DH_8BIT_OFFSET) | data0);
        DACC_R8DH(dac_periph) = (uint32_t)data;
        break;
    default:
        break;
    }
}

/*!
    \brief      get the DAC flag
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  flag: the DAC status flags, only one parameter can be selected which is shown
                as below:
      \arg        DAC_FLAG_DDUDR0: DACx_OUT0 DMA underrun flag
      \arg        DAC_FLAG_DDUDR1: DACx_OUT1 DMA underrun flag
    \param[out] none
    \retval     the state of DAC bit(SET or RESET)
*/
FlagStatus dac_flag_get(uint32_t dac_periph, uint32_t flag)
{
    FlagStatus status = RESET;

    if(flag & DAC_STAT_FLAG_MASK0){
        /* check DAC_STAT0 flag */
        if(RESET != (DAC_STAT0(dac_periph) & flag)){
            status = SET;
        }else{
            status = RESET;
        }
    }else{
        /* illegal parameters */
    }
    return status;
}

/*!
    \brief      clear the DAC flag
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  flag: DAC flag
                one or more parameter can be selected which are shown as below:
      \arg        DAC_FLAG_DDUDR0: DACx_OUT0 DMA underrun flag
      \arg        DAC_FLAG_DDUDR1: DACx_OUT1 DMA underrun flag
    \param[out] none
    \retval     none
*/
void dac_flag_clear(uint32_t dac_periph, uint32_t flag)
{
    if(flag & DAC_STAT_FLAG_MASK0){
        /* check DAC_STAT0 flag */
        DAC_STAT0(dac_periph) = (uint32_t)(flag & DAC_STAT_FLAG_MASK0);
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      enable DAC interrupt(DAC DMA underrun interrupt)
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  interrupt: the DAC interrupt
                one or more parameter can be selected which are shown as below:
      \arg        DAC_INT_DDUDR0: DACx_OUT0 DMA underrun interrupt
      \arg        DAC_INT_DDUDR1: DACx_OUT1 DMA underrun interrupt
    \param[out] none
    \retval     none
*/
void dac_interrupt_enable(uint32_t dac_periph, uint32_t interrupt)
{
    if(interrupt & DAC_INT_EN_MASK0){
        /* enable underrun interrupt */
        DAC_CTL0(dac_periph) |= (uint32_t)(interrupt & DAC_INT_EN_MASK0);
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      disable DAC interrupt(DAC DMA underrun interrupt)
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  interrupt: the DAC interrupt
                one and more parameter can be selected which are shown as below:
      \arg        DAC_INT_DDUDR0: DACx_OUT0 DMA underrun interrupt
      \arg        DAC_INT_DDUDR1: DACx_OUT1 DMA underrun interrupt
    \param[out] none
    \retval     none
*/
void dac_interrupt_disable(uint32_t dac_periph, uint32_t interrupt)
{
    if(interrupt & DAC_INT_EN_MASK0){
        /* disable underrun interrupt */
        DAC_CTL0(dac_periph) &= (uint32_t)(~(interrupt & DAC_INT_EN_MASK0));
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      get the DAC interrupt flag(DAC DMA underrun interrupt flag)
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  int_flag: DAC interrupt flag
                only one parameter can be selected which is shown as below:
      \arg        DAC_INT_FLAG_DDUDR0: DACx_OUT0 DMA underrun interrupt flag
      \arg        DAC_INT_FLAG_DDUDR1: DACx_OUT1 DMA underrun interrupt flag
    \param[out] none
    \retval     the state of DAC interrupt flag(SET or RESET)
*/
FlagStatus dac_interrupt_flag_get(uint32_t dac_periph, uint32_t int_flag)
{
    uint32_t reg1 = 0U, reg2 = 0U;
    FlagStatus status = RESET;

    if(int_flag & DAC_INT_FLAG_MASK0){
        /* check underrun interrupt int_flag */
        reg1 = DAC_STAT0(dac_periph) & int_flag;
        reg2 = DAC_CTL0(dac_periph) & int_flag;
    }else{
        /* illegal parameters */
    }

    /*get DAC interrupt flag status */
    if((RESET != reg1) && (RESET != reg2)){
       status = SET;
    }else{
       status = RESET;
    }
    return status;
}

/*!
    \brief      clear the DAC interrupt flag(DAC DMA underrun interrupt flag)
    \param[in]  dac_periph: DACx(x=0)
    \param[in]  int_flag: DAC interrupt flag
                one or more parameter can be selected which are shown as below:
      \arg        DAC_INT_FLAG_DDUDR0: DACx_OUT0 DMA underrun interrupt flag
      \arg        DAC_INT_FLAG_DDUDR1: DACx_OUT1 DMA underrun interrupt flag
    \param[out] none
    \retval     none
*/
void dac_interrupt_flag_clear(uint32_t dac_periph, uint32_t int_flag)
{
    /* clear underrun interrupt int_flag */
    if(int_flag & DAC_INT_FLAG_MASK0){
        DAC_STAT0(dac_periph) = (uint32_t)(int_flag & DAC_INT_FLAG_MASK0);
    }else{
        /* illegal parameters */
    }
}
