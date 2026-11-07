/*!
    \file    gd32f4xx_iref.c
    \brief   IREF driver

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

#include "gd32f4xx_iref.h"

/*!
    \brief      deinitialize IREF
    \param[in]  none
    \param[out] none
    \retval     none
*/
void iref_deinit(void)
{
    rcu_periph_reset_enable(RCU_IREFRST);
    rcu_periph_reset_disable(RCU_IREFRST);
}

/*!
    \brief    enable IREF
    \param[in]  none
    \param[out] none
    \retval     none
*/
void iref_enable(void)
{
    IREF_CTL |= IREF_CTL_CREN;
}

/*!
    \brief      disable IREF
    \param[in]  none
    \param[out] none
    \retval     none
*/
void iref_disable(void)
{
    IREF_CTL &= ~IREF_CTL_CREN;
}

/*!
    \brief      set IREF mode
    \param[in]  step
      \arg        IREF_MODE_LOW_POWER: 1uA step
      \arg        IREF_MODE_HIGH_CURRENT: 8uA step
    \param[out] none
    \retval     none
*/
void iref_mode_set(uint32_t step)
{
    IREF_CTL &= ~IREF_CTL_SSEL;
    IREF_CTL |= step;
}

/*!
    \brief      set IREF precision_trim_value
    \param[in]  precisiontrim
      \arg        IREF_CUR_PRECISION_TRIM_X(x=0..31): (-15+ x)%
    \param[out] none
    \retval     none
*/
void iref_precision_trim_value_set(uint32_t precisiontrim)
{
    IREF_CTL &= ~IREF_CTL_CPT;
    IREF_CTL |= precisiontrim;
}

/*!
    \brief      set IREF sink mode
    \param[in]  sinkmode
      \arg        IREF_SOURCE_CURRENT : source current.
      \arg        IREF_SINK_CURRENT: sink current
    \param[out] none
    \retval     none
*/
void iref_sink_set(uint32_t sinkmode)
{
    IREF_CTL &= ~IREF_CTL_SCMOD;
    IREF_CTL |= sinkmode;
}

/*!
    \brief      set IREF step data
    \param[in]  stepdata
      \arg        IREF_CUR_STEP_DATA_X:(x=0..63): step*x
    \param[out] none
    \retval     none
*/

void iref_step_data_config(uint32_t stepdata)
{
    IREF_CTL &= ~IREF_CTL_CSDT;
    IREF_CTL |= stepdata;
}
