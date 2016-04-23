/*
 * File:   interrupts_irq.h
 * Author: mat1
 *
 * Created on May 26, 2015, 1:11 PM
 */

#ifndef INTERRUPTS_IRQ_H
#define	INTERRUPTS_IRQ_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "static_assert.h"

typedef enum hal_irq_t {
#if defined(STM32F10X_MD) || defined(STM32F10X_HD)
    __All_irq = 0,
    SysInterrupt_SysTick,
    SysInterrupt_TIM1_CC_IRQ,
    SysInterrupt_TIM1_Compare1,
    SysInterrupt_TIM1_Compare2,
    SysInterrupt_TIM1_Compare3,
    SysInterrupt_TIM1_Compare4,
    SysInterrupt_TIM2_IRQ,
    SysInterrupt_TIM2_Compare1,
    SysInterrupt_TIM2_Compare2,
    SysInterrupt_TIM2_Compare3,
    SysInterrupt_TIM2_Compare4,
    SysInterrupt_TIM2_Update,
    SysInterrupt_TIM2_Trigger,
    SysInterrupt_TIM3_IRQ,
    SysInterrupt_TIM3_Compare1,
    SysInterrupt_TIM3_Compare2,
    SysInterrupt_TIM3_Compare3,
    SysInterrupt_TIM3_Compare4,
    SysInterrupt_TIM3_Update,
    SysInterrupt_TIM3_Trigger,
    SysInterrupt_TIM4_IRQ,
    SysInterrupt_TIM4_Compare1,
    SysInterrupt_TIM4_Compare2,
    SysInterrupt_TIM4_Compare3,
    SysInterrupt_TIM4_Compare4,
    SysInterrupt_TIM4_Update,
    SysInterrupt_TIM4_Trigger,
    __Last_irq = SysInterrupt_TIM4_Trigger+1


#elif defined(STM32F2XX)
    __All_irq = 0,
    SysInterrupt_SysTick,
    SysInterrupt_TIM1_CC_IRQ,
    SysInterrupt_TIM1_Compare1,
    SysInterrupt_TIM1_Compare2,
    SysInterrupt_TIM1_Compare3,
    SysInterrupt_TIM1_Compare4,
    SysInterrupt_TIM2_IRQ,
    SysInterrupt_TIM2_Compare1,
    SysInterrupt_TIM2_Compare2,
    SysInterrupt_TIM2_Compare3,
    SysInterrupt_TIM2_Compare4,
    SysInterrupt_TIM2_Update,
    SysInterrupt_TIM2_Trigger,
    SysInterrupt_TIM3_IRQ,
    SysInterrupt_TIM3_Compare1,
    SysInterrupt_TIM3_Compare2,
    SysInterrupt_TIM3_Compare3,
    SysInterrupt_TIM3_Compare4,
    SysInterrupt_TIM3_Update,
    SysInterrupt_TIM3_Trigger,
    SysInterrupt_TIM4_IRQ,
    SysInterrupt_TIM4_Compare1,
    SysInterrupt_TIM4_Compare2,
    SysInterrupt_TIM4_Compare3,
    SysInterrupt_TIM4_Compare4,
    SysInterrupt_TIM4_Update,
    SysInterrupt_TIM4_Trigger,
    SysInterrupt_TIM5_IRQ,
    SysInterrupt_TIM5_Compare1,
    SysInterrupt_TIM5_Compare2,
    SysInterrupt_TIM5_Compare3,
    SysInterrupt_TIM5_Compare4,
    SysInterrupt_TIM5_Update,
    SysInterrupt_TIM5_Trigger,
    SysInterrupt_TIM6_DAC_IRQ,
    SysInterrupt_TIM6_Update,
    SysInterrupt_TIM7_IRQ,
    SysInterrupt_TIM7_Update,
    SysInterrupt_TIM1_BRK_TIM9_IRQ,
    SysInterrupt_TIM1_Break,
    SysInterrupt_TIM9_Compare1,
    SysInterrupt_TIM9_Compare2,
    SysInterrupt_TIM9_Update,
    SysInterrupt_TIM9_Trigger,
    SysInterrupt_TIM1_UP_TIM10_IRQ,
    SysInterrupt_TIM1_Update,
    SysInterrupt_TIM10_Compare,
    SysInterrupt_TIM10_Update,
    SysInterrupt_TIM8_BRK_TIM12_IRQ,
    SysInterrupt_TIM8_Break,
    SysInterrupt_TIM12_Compare1,
    SysInterrupt_TIM12_Compare2,
    SysInterrupt_TIM12_Update,
    SysInterrupt_TIM12_Trigger,
    SysInterrupt_TIM8_UP_TIM13_IRQ,
    SysInterrupt_TIM8_Update,
    SysInterrupt_TIM13_Compare,
    SysInterrupt_TIM13_Update,
    SysInterrupt_TIM8_TRG_COM_TIM14_IRQ,
    SysInterrupt_TIM8_Trigger,
    SysInterrupt_TIM14_COM,
    SysInterrupt_TIM14_Compare,
    SysInterrupt_TIM14_Update,
    SysInterrupt_TIM8_IRQ,
    SysInterrupt_TIM8_Compare1,
    SysInterrupt_TIM8_Compare2,
    SysInterrupt_TIM8_Compare3,
    SysInterrupt_TIM8_Compare4,
    SysInterrupt_TIM1_TRG_COM_TIM11_IRQ,
    SysInterrupt_TIM1_Trigger,
    SysInterrupt_TIM1_COM,
    SysInterrupt_TIM11_Compare,
    SysInterrupt_TIM11_Update,
    SysInterrupt_ADC_IRQ,
    SysInterrupt_CAN2_TX_IRQ,
    SysInterrupt_CAN2_RX0_IRQ,
    SysInterrupt_CAN2_RX1_IRQ,
    SysInterrupt_CAN2_SCE_IRQ,
    SysInterrupt_CAN1_TX_IRQ,
    SysInterrupt_CAN1_RX0_IRQ,
    SysInterrupt_CAN1_RX1_IRQ,
    SysInterrupt_CAN1_SCE_IRQ,
    __Last_irq = SysInterrupt_CAN1_SCE_IRQ + 1,
#else
    __Last_irq = 0
#endif
} hal_irq_t;

#if defined(STM32F2XX)
STATIC_ASSERT(__last_irq_is_set, __Last_irq==SysInterrupt_CAN1_SCE_IRQ+1);
STATIC_ASSERT(__last_irq_is_set, __Last_irq==83);
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* INTERRUPTS_IRQ_H */

