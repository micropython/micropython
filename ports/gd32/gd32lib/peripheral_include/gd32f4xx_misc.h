/*!
    \file    gd32f4xx_misc.h
    \brief   definitions for the MISC
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

#ifndef GD32F4XX_MISC_H
#define GD32F4XX_MISC_H

#include "gd32f4xx.h"

/* constants definitions */
/* set the RAM and FLASH base address */
#define NVIC_VECTTAB_RAM            ((uint32_t)0x20000000)     /*!< RAM base address */
#define NVIC_VECTTAB_FLASH          ((uint32_t)0x08000000)     /*!< Flash base address */

/* set the NVIC vector table offset mask */
#define NVIC_VECTTAB_OFFSET_MASK    ((uint32_t)0x1FFFFF80)

/* the register key mask, if you want to do the write operation, you should write 0x5FA to VECTKEY bits */
#define NVIC_AIRCR_VECTKEY_MASK     ((uint32_t)0x05FA0000)

/* priority group - define the pre-emption priority and the subpriority */
#define NVIC_PRIGROUP_PRE0_SUB4     ((uint32_t)0x700)          /*!< 0 bits for pre-emption priority 4 bits for subpriority */
#define NVIC_PRIGROUP_PRE1_SUB3     ((uint32_t)0x600)          /*!< 1 bits for pre-emption priority 3 bits for subpriority */
#define NVIC_PRIGROUP_PRE2_SUB2     ((uint32_t)0x500)          /*!< 2 bits for pre-emption priority 2 bits for subpriority */
#define NVIC_PRIGROUP_PRE3_SUB1     ((uint32_t)0x400)          /*!< 3 bits for pre-emption priority 1 bits for subpriority */
#define NVIC_PRIGROUP_PRE4_SUB0     ((uint32_t)0x300)          /*!< 4 bits for pre-emption priority 0 bits for subpriority */

/* choose the method to enter or exit the lowpower mode */
#define SCB_SCR_SLEEPONEXIT         ((uint8_t)0x02)            /*!< choose the the system whether enter low power mode by exiting from ISR */
#define SCB_SCR_SLEEPDEEP           ((uint8_t)0x04)            /*!< choose the the system enter the DEEPSLEEP mode or SLEEP mode */
#define SCB_SCR_SEVONPEND           ((uint8_t)0x10)            /*!< choose the interrupt source that can wake up the lowpower mode */

#define SCB_LPM_SLEEP_EXIT_ISR      SCB_SCR_SLEEPONEXIT
#define SCB_LPM_DEEPSLEEP           SCB_SCR_SLEEPDEEP
#define SCB_LPM_WAKE_BY_ALL_INT     SCB_SCR_SEVONPEND

/* choose the systick clock source */
#define SYSTICK_CLKSOURCE_HCLK_DIV8 ((uint32_t)0xFFFFFFFBU)    /*!< systick clock source is from HCLK/8 */
#define SYSTICK_CLKSOURCE_HCLK      ((uint32_t)0x00000004U)    /*!< systick clock source is from HCLK */

/* function declarations */
/* set the priority group */
void nvic_priority_group_set(uint32_t nvic_prigroup);

/* enable NVIC request */
void nvic_irq_enable(IRQn_Type nvic_irq, uint8_t nvic_irq_pre_priority, uint8_t nvic_irq_sub_priority);
/* disable NVIC request */
void nvic_irq_disable(IRQn_Type nvic_irq);

/* set the NVIC vector table base address */
void nvic_vector_table_set(uint32_t nvic_vict_tab, uint32_t offset);

/* set the state of the low power mode */
void system_lowpower_set(uint8_t lowpower_mode);
/* reset the state of the low power mode */
void system_lowpower_reset(uint8_t lowpower_mode);

/* set the systick clock source */
void systick_clksource_set(uint32_t systick_clksource);

#endif /* GD32F4XX_MISC_H */
