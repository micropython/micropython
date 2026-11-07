/*!
    \file    gd32f4xx_misc.c
    \brief   MISC driver
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

#include "gd32f4xx_misc.h"

/*!
    \brief    set the priority group
    \param[in]  nvic_prigroup: the NVIC priority group
      \arg        NVIC_PRIGROUP_PRE0_SUB4:0 bits for pre-emption priority 4 bits for subpriority
      \arg        NVIC_PRIGROUP_PRE1_SUB3:1 bits for pre-emption priority 3 bits for subpriority
      \arg        NVIC_PRIGROUP_PRE2_SUB2:2 bits for pre-emption priority 2 bits for subpriority
      \arg        NVIC_PRIGROUP_PRE3_SUB1:3 bits for pre-emption priority 1 bits for subpriority
      \arg        NVIC_PRIGROUP_PRE4_SUB0:4 bits for pre-emption priority 0 bits for subpriority
    \param[out] none
    \retval     none
*/
void nvic_priority_group_set(uint32_t nvic_prigroup)
{
    /* set the priority group value */
    SCB->AIRCR = NVIC_AIRCR_VECTKEY_MASK | nvic_prigroup;
}

/*!
    \brief    enable NVIC request
    \param[in]  nvic_irq: the NVIC interrupt request, detailed in IRQn_Type
    \param[in]  nvic_irq_pre_priority: the pre-emption priority needed to set
    \param[in]  nvic_irq_sub_priority: the subpriority needed to set
    \param[out] none
    \retval     none
*/
void nvic_irq_enable(IRQn_Type nvic_irq, uint8_t nvic_irq_pre_priority,
                     uint8_t nvic_irq_sub_priority)
{
    uint32_t nvic_prigroup, nvic_priority;

    /* check current priority group */
    switch(SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) {
    case NVIC_PRIGROUP_PRE0_SUB4:
    case NVIC_PRIGROUP_PRE1_SUB3:
    case NVIC_PRIGROUP_PRE2_SUB2:
    case NVIC_PRIGROUP_PRE3_SUB1:
    case NVIC_PRIGROUP_PRE4_SUB0:
        break;
    default:
        nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
        break;
    }

    /* get the priority group value */
    nvic_prigroup = NVIC_GetPriorityGrouping();

    /* encoding the pre-emption, subpriority priority */
    nvic_priority = NVIC_EncodePriority(nvic_prigroup, (uint32_t)nvic_irq_pre_priority, (uint32_t)nvic_irq_sub_priority);
    /* set priority */
    NVIC_SetPriority(nvic_irq, nvic_priority);

    /* enable the selected IRQ */
    NVIC_EnableIRQ(nvic_irq);
}

/*!
    \brief    disable NVIC request
    \param[in]  nvic_irq: the NVIC interrupt request, detailed in IRQn_Type
    \param[out] none
    \retval     none
*/
void nvic_irq_disable(IRQn_Type nvic_irq)
{
    /* disable the selected IRQ.*/
    NVIC_DisableIRQ(nvic_irq);
}

/*!
    \brief    set the NVIC vector table base address
    \param[in]  nvic_vict_tab: the RAM or FLASH base address
      \arg        NVIC_VECTTAB_RAM: RAM base address
      \are        NVIC_VECTTAB_FLASH: Flash base address
    \param[in]  offset: Vector Table offset
    \param[out] none
    \retval     none
*/
void nvic_vector_table_set(uint32_t nvic_vict_tab, uint32_t offset)
{
    SCB->VTOR = nvic_vict_tab | (offset & NVIC_VECTTAB_OFFSET_MASK);
    __DSB();
}

/*!
    \brief    set the state of the low power mode
    \param[in]  lowpower_mode: the low power mode state
      \arg        SCB_LPM_SLEEP_EXIT_ISR: if chose this para, the system always enter low power
                    mode by exiting from ISR
      \arg        SCB_LPM_DEEPSLEEP: if chose this para, the system will enter the DEEPSLEEP mode
      \arg        SCB_LPM_WAKE_BY_ALL_INT: if chose this para, the lowpower mode can be woke up
                    by all the enable and disable interrupts
    \param[out] none
    \retval     none
*/
void system_lowpower_set(uint8_t lowpower_mode)
{
    SCB->SCR |= (uint32_t)lowpower_mode;
}

/*!
    \brief    reset the state of the low power mode
    \param[in]  lowpower_mode: the low power mode state
      \arg        SCB_LPM_SLEEP_EXIT_ISR: if chose this para, the system will exit low power
                    mode by exiting from ISR
      \arg        SCB_LPM_DEEPSLEEP: if chose this para, the system will enter the SLEEP mode
      \arg        SCB_LPM_WAKE_BY_ALL_INT: if chose this para, the lowpower mode only can be
                    woke up by the enable interrupts
    \param[out] none
    \retval     none
*/
void system_lowpower_reset(uint8_t lowpower_mode)
{
    SCB->SCR &= (~(uint32_t)lowpower_mode);
}

/*!
    \brief    set the systick clock source
    \param[in]  systick_clksource: the systick clock source needed to choose
      \arg        SYSTICK_CLKSOURCE_HCLK: systick clock source is from HCLK
      \arg        SYSTICK_CLKSOURCE_HCLK_DIV8: systick clock source is from HCLK/8
    \param[out] none
    \retval     none
*/

void systick_clksource_set(uint32_t systick_clksource)
{
    if(SYSTICK_CLKSOURCE_HCLK == systick_clksource) {
        /* set the systick clock source from HCLK */
        SysTick->CTRL |= SYSTICK_CLKSOURCE_HCLK;
    } else {
        /* set the systick clock source from HCLK/8 */
        SysTick->CTRL &= SYSTICK_CLKSOURCE_HCLK_DIV8;
    }
}
