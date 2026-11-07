/*!
    \file    gd32f4xx_ctc.c
    \brief   CTC driver

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

#include "gd32f4xx_ctc.h"

#define CTC_FLAG_MASK            ((uint32_t)0x00000700U)

/* CTC register bit offset */
#define CTC_TRIMVALUE_OFFSET     ((uint32_t)8U)
#define CTC_TRIM_VALUE_OFFSET    ((uint32_t)8U)
#define CTC_REFCAP_OFFSET        ((uint32_t)16U)
#define CTC_LIMIT_VALUE_OFFSET   ((uint32_t)16U)

/*!
    \brief    reset CTC clock trim controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ctc_deinit(void)
{
    /* reset CTC */
    rcu_periph_reset_enable(RCU_CTCRST);
    rcu_periph_reset_disable(RCU_CTCRST);
}

/*!
    \brief    enable CTC trim counter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ctc_counter_enable(void)
{
    CTC_CTL0 |= (uint32_t)CTC_CTL0_CNTEN;
}

/*!
    \brief    disable CTC trim counter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ctc_counter_disable(void)
{
    CTC_CTL0 &= (uint32_t)(~CTC_CTL0_CNTEN);
}

/*!
    \brief    configure the IRC48M trim value
    \param[in]  ctc_trim_value: 8-bit IRC48M trim value
      \arg        0x00 - 0x3F
    \param[out] none
    \retval     none
*/
void ctc_irc48m_trim_value_config(uint8_t trim_value)
{
    /* clear TRIMVALUE bits */
    CTC_CTL0 &= (~(uint32_t)CTC_CTL0_TRIMVALUE);
    /* set TRIMVALUE bits */
    CTC_CTL0 |= ((uint32_t)trim_value << CTC_TRIM_VALUE_OFFSET);
}

/*!
    \brief    generate software reference source sync pulse
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ctc_software_refsource_pulse_generate(void)
{
    CTC_CTL0 |= (uint32_t)CTC_CTL0_SWREFPUL;
}

/*!
    \brief    configure hardware automatically trim mode
    \param[in]  hardmode:
                only one parameter can be selected which is shown as below:
      \arg        CTC_HARDWARE_TRIM_MODE_ENABLE: hardware automatically trim mode enable
      \arg        CTC_HARDWARE_TRIM_MODE_DISABLE: hardware automatically trim mode disable
    \param[out] none
    \retval     none
*/
void ctc_hardware_trim_mode_config(uint32_t hardmode)
{
    CTC_CTL0 &= (uint32_t)(~CTC_CTL0_AUTOTRIM);
    CTC_CTL0 |= (uint32_t)hardmode;
}

/*!
    \brief    configure reference signal source polarity
    \param[in]  polarity:
                only one parameter can be selected which is shown as below:
      \arg        CTC_REFSOURCE_POLARITY_FALLING: reference signal source polarity is falling edge
      \arg        CTC_REFSOURCE_POLARITY_RISING: reference signal source polarity is rising edge
    \param[out] none
    \retval     none
*/
void ctc_refsource_polarity_config(uint32_t polarity)
{
    CTC_CTL1 &= (uint32_t)(~CTC_CTL1_REFPOL);
    CTC_CTL1 |= (uint32_t)polarity;
}

/*!
    \brief    select reference signal source
    \param[in]  refs:
                only one parameter can be selected which is shown as below:
      \arg        CTC_REFSOURCE_GPIO: GPIO is selected
      \arg        CTC_REFSOURCE_LXTAL: LXTAL is selected
    \param[out] none
    \retval     none
*/
void ctc_refsource_signal_select(uint32_t refs)
{
    CTC_CTL1 &= (uint32_t)(~CTC_CTL1_REFSEL);
    CTC_CTL1 |= (uint32_t)refs;
}

/*!
    \brief    configure reference signal source prescaler
    \param[in]  prescaler:
                only one parameter can be selected which is shown as below:
      \arg        CTC_REFSOURCE_PSC_OFF: reference signal not divided
      \arg        CTC_REFSOURCE_PSC_DIV2: reference signal divided by 2
      \arg        CTC_REFSOURCE_PSC_DIV4: reference signal divided by 4
      \arg        CTC_REFSOURCE_PSC_DIV8: reference signal divided by 8
      \arg        CTC_REFSOURCE_PSC_DIV16: reference signal divided by 16
      \arg        CTC_REFSOURCE_PSC_DIV32: reference signal divided by 32
      \arg        CTC_REFSOURCE_PSC_DIV64: reference signal divided by 64
      \arg        CTC_REFSOURCE_PSC_DIV128: reference signal divided by 128
    \param[out] none
    \retval     none
*/
void ctc_refsource_prescaler_config(uint32_t prescaler)
{
    CTC_CTL1 &= (uint32_t)(~CTC_CTL1_REFPSC);
    CTC_CTL1 |= (uint32_t)prescaler;
}

/*!
    \brief    configure clock trim base limit value
    \param[in]  limit_value: 8-bit clock trim base limit value
      \arg        0x00 - 0xFF
    \param[out] none
    \retval     none
*/
void ctc_clock_limit_value_config(uint8_t limit_value)
{
    CTC_CTL1 &= (uint32_t)(~CTC_CTL1_CKLIM);
    CTC_CTL1 |= (uint32_t)((uint32_t)limit_value << CTC_LIMIT_VALUE_OFFSET);
}

/*!
    \brief    configure CTC counter reload value
    \param[in]  reload_value: 16-bit CTC counter reload value
      \arg        0x0000 - 0xFFFF
    \param[out] none
    \retval     none
*/
void ctc_counter_reload_value_config(uint16_t reload_value)
{
    CTC_CTL1 &= (uint32_t)(~CTC_CTL1_RLVALUE);
    CTC_CTL1 |= (uint32_t)reload_value;
}

/*!
    \brief    read CTC counter capture value when reference sync pulse occurred
    \param[in]  none
    \param[out] none
    \retval     the 16-bit CTC counter capture value
*/
uint16_t ctc_counter_capture_value_read(void)
{
    uint16_t capture_value = 0U;
    capture_value = (uint16_t)((CTC_STAT & CTC_STAT_REFCAP) >> CTC_REFCAP_OFFSET);
    return (capture_value);
}

/*!
    \brief    read CTC trim counter direction when reference sync pulse occurred
    \param[in]  none
    \param[out] none
    \retval     FlagStatus: SET or RESET
      \arg        SET: CTC trim counter direction is down-counting
      \arg        RESET: CTC trim counter direction is up-counting
*/
FlagStatus ctc_counter_direction_read(void)
{
    FlagStatus status;
    if(RESET != (CTC_STAT & CTC_STAT_REFDIR)) {
        status = SET;
    } else {
        status = RESET;
    }
    return status;
}

/*!
    \brief    read CTC counter reload value
    \param[in]  none
    \param[out] none
    \retval     the 16-bit CTC counter reload value
*/
uint16_t ctc_counter_reload_value_read(void)
{
    uint16_t reload_value = 0U;
    reload_value = (uint16_t)(CTC_CTL1 & CTC_CTL1_RLVALUE);
    return (reload_value);
}

/*!
    \brief    read the IRC48M trim value
    \param[in]  none
    \param[out] none
    \retval     the 8-bit IRC48M trim value
*/
uint8_t ctc_irc48m_trim_value_read(void)
{
    uint8_t trim_value = 0U;
    trim_value = (uint8_t)((CTC_CTL0 & CTC_CTL0_TRIMVALUE) >> CTC_TRIMVALUE_OFFSET);
    return (trim_value);
}

/*!
    \brief    enable the CTC interrupt
    \param[in]  interrupt: CTC interrupt enable
                one or more parameters can be selected which are shown as below:
      \arg        CTC_INT_CKOK: clock trim OK interrupt enable
      \arg        CTC_INT_CKWARN: clock trim warning interrupt enable
      \arg        CTC_INT_ERR: error interrupt enable
      \arg        CTC_INT_EREF: expect reference interrupt enable
    \param[out] none
    \retval     none
*/
void ctc_interrupt_enable(uint32_t interrupt)
{
    CTC_CTL0 |= (uint32_t)interrupt;
}

/*!
    \brief    disable the CTC interrupt
    \param[in]  interrupt: CTC interrupt enable source
                one or more parameters can be selected which are shown as below:
      \arg        CTC_INT_CKOK: clock trim OK interrupt enable
      \arg        CTC_INT_CKWARN: clock trim warning interrupt enable
      \arg        CTC_INT_ERR: error interrupt enable
      \arg        CTC_INT_EREF: expect reference interrupt enable
    \param[out] none
    \retval     none
*/
void ctc_interrupt_disable(uint32_t interrupt)
{
    CTC_CTL0 &= (uint32_t)(~interrupt);
}

/*!
    \brief    get CTC interrupt flag
    \param[in]  int_flag: the CTC interrupt flag
                only one parameter can be selected which is shown as below:
      \arg        CTC_INT_FLAG_CKOK: clock trim OK interrupt
      \arg        CTC_INT_FLAG_CKWARN: clock trim warning interrupt
      \arg        CTC_INT_FLAG_ERR: error interrupt
      \arg        CTC_INT_FLAG_EREF: expect reference interrupt
      \arg        CTC_INT_FLAG_CKERR: clock trim error bit interrupt
      \arg        CTC_INT_FLAG_REFMISS: reference sync pulse miss interrupt
      \arg        CTC_INT_FLAG_TRIMERR: trim value error interrupt
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus ctc_interrupt_flag_get(uint32_t int_flag)
{
    FlagStatus status;
    uint32_t interrupt_flag = 0U;
    uint32_t intenable = 0U;
    /* check whether the interrupt is enabled */
    if(RESET != (int_flag & CTC_FLAG_MASK)) {
        intenable = CTC_CTL0 & CTC_CTL0_ERRIE;
    } else {
        intenable = CTC_CTL0 & int_flag;
    }

    /* get interrupt flag status */
    interrupt_flag = CTC_STAT & int_flag;

    if(interrupt_flag && intenable) {
        status = SET;
    } else {
        status = RESET;
    }
    return status;
}

/*!
    \brief    clear CTC interrupt flag
    \param[in]  int_flag: the CTC interrupt flag
                only one parameter can be selected which is shown as below:
      \arg        CTC_INT_FLAG_CKOK: clock trim OK interrupt
      \arg        CTC_INT_FLAG_CKWARN: clock trim warning interrupt
      \arg        CTC_INT_FLAG_ERR: error interrupt
      \arg        CTC_INT_FLAG_EREF: expect reference interrupt
      \arg        CTC_INT_FLAG_CKERR: clock trim error bit interrupt
      \arg        CTC_INT_FLAG_REFMISS: reference sync pulse miss interrupt
      \arg        CTC_INT_FLAG_TRIMERR: trim value error interrupt
    \param[out] none
    \retval     none
*/
void ctc_interrupt_flag_clear(uint32_t int_flag)
{
    if(RESET != (int_flag & CTC_FLAG_MASK)) {
        CTC_INTC |= CTC_INTC_ERRIC;
    } else {
        CTC_INTC |= int_flag;
    }
}

/*!
    \brief    get CTC flag
    \param[in]  flag: the CTC flag
                only one parameter can be selected which is shown as below:
      \arg        CTC_FLAG_CKOK: clock trim OK flag
      \arg        CTC_FLAG_CKWARN: clock trim warning flag
      \arg        CTC_FLAG_ERR: error flag
      \arg        CTC_FLAG_EREF: expect reference flag
      \arg        CTC_FLAG_CKERR: clock trim error bit
      \arg        CTC_FLAG_REFMISS: reference sync pulse miss
      \arg        CTC_FLAG_TRIMERR: trim value error bit
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus ctc_flag_get(uint32_t flag)
{
    FlagStatus status;
    if(RESET != (CTC_STAT & flag)) {
        status = SET;
    } else {
        status = RESET;
    }
    return status;
}

/*!
    \brief    clear CTC flag
    \param[in]  flag: the CTC flag
                only one parameter can be selected which is shown as below:
      \arg        CTC_FLAG_CKOK: clock trim OK flag
      \arg        CTC_FLAG_CKWARN: clock trim warning flag
      \arg        CTC_FLAG_ERR: error flag
      \arg        CTC_FLAG_EREF: expect reference flag
      \arg        CTC_FLAG_CKERR: clock trim error bit
      \arg        CTC_FLAG_REFMISS: reference sync pulse miss
      \arg        CTC_FLAG_TRIMERR: trim value error bit
    \param[out] none
    \retval     none
*/
void ctc_flag_clear(uint32_t flag)
{
    if(RESET != (flag & CTC_FLAG_MASK)) {
        CTC_INTC |= CTC_INTC_ERRIC;
    } else {
        CTC_INTC |= flag;
    }
}
