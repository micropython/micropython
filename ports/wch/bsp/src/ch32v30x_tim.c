/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_tim.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the TIM firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "ch32v30x_tim.h"
#include "ch32v30x_rcc.h"

/* TIM registers bit mask */
#define SMCFGR_ETR_Mask    ((uint16_t)0x00FF)
#define CHCTLR_Offset      ((uint16_t)0x0018)
#define CCER_CCE_Set       ((uint16_t)0x0001)
#define CCER_CCNE_Set      ((uint16_t)0x0004)

static void TI1_Config(TIM_TypeDef *TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter);
static void TI2_Config(TIM_TypeDef *TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter);
static void TI3_Config(TIM_TypeDef *TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter);
static void TI4_Config(TIM_TypeDef *TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter);

/*********************************************************************
 * @fn      TIM_DeInit
 *
 * @brief   Deinitializes the TIMx peripheral registers to their default
 *        reset values.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *
 * @return  none
 */
void TIM_DeInit(TIM_TypeDef *TIMx)
{
    if(TIMx == TIM1)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, DISABLE);
    }
    else if(TIMx == TIM8)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM8, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM8, DISABLE);
    }
    else if(TIMx == TIM9)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM9, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM9, DISABLE);
    }
    else if(TIMx == TIM10)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM10, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM10, DISABLE);
    }
    else if(TIMx == TIM2)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, DISABLE);
    }
    else if(TIMx == TIM3)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, DISABLE);
    }
    else if(TIMx == TIM4)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM4, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM4, DISABLE);
    }
    else if(TIMx == TIM5)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM5, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM5, DISABLE);
    }
    else if(TIMx == TIM6)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM6, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM6, DISABLE);
    }
    else if(TIMx == TIM7)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM7, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM7, DISABLE);
    }
}

/*********************************************************************
 * @fn      TIM_TimeBaseInit
 *
 * @brief   Initializes the TIMx Time Base Unit peripheral according to
 *        the specified parameters in the TIM_TimeBaseInitStruct.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_TimeBaseInitStruct - pointer to a TIM_TimeBaseInitTypeDef
 *        structure.
 *
 * @return  none
 */
void TIM_TimeBaseInit(TIM_TypeDef *TIMx, TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct)
{
    uint16_t tmpcr1 = 0;

    tmpcr1 = TIMx->CTLR1;

    if((TIMx == TIM1) || (TIMx == TIM2) || (TIMx == TIM3) || (TIMx == TIM4) ||
       (TIMx == TIM5) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10))
    {
        tmpcr1 &= (uint16_t)(~((uint16_t)(TIM_DIR | TIM_CMS)));
        tmpcr1 |= (uint32_t)TIM_TimeBaseInitStruct->TIM_CounterMode;
    }

    if((TIMx != TIM6) && (TIMx != TIM7))
    {
        tmpcr1 &= (uint16_t)(~((uint16_t)TIM_CTLR1_CKD));
        tmpcr1 |= (uint32_t)TIM_TimeBaseInitStruct->TIM_ClockDivision;
    }

    TIMx->CTLR1 = tmpcr1;
    TIMx->ATRLR = TIM_TimeBaseInitStruct->TIM_Period;
    TIMx->PSC = TIM_TimeBaseInitStruct->TIM_Prescaler;

    if((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10))
    {
        TIMx->RPTCR = TIM_TimeBaseInitStruct->TIM_RepetitionCounter;
    }

    TIMx->SWEVGR = TIM_PSCReloadMode_Immediate;
}

/*********************************************************************
 * @fn      TIM_OC1Init
 *
 * @brief   Initializes the TIMx Channel1 according to the specified
 *        parameters in the TIM_OCInitStruct.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCInitStruct - pointer to a TIM_OCInitTypeDef structure.
 *
 * @return  none
 */
void TIM_OC1Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct)
{
    uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

    TIMx->CCER &= (uint16_t)(~(uint16_t)TIM_CC1E);
    tmpccer = TIMx->CCER;
    tmpcr2 = TIMx->CTLR2;
    tmpccmrx = TIMx->CHCTLR1;
    tmpccmrx &= (uint16_t)(~((uint16_t)TIM_OC1M));
    tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CC1S));
    tmpccmrx |= TIM_OCInitStruct->TIM_OCMode;
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CC1P));
    tmpccer |= TIM_OCInitStruct->TIM_OCPolarity;
    tmpccer |= TIM_OCInitStruct->TIM_OutputState;

    if((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10))
    {
        tmpccer &= (uint16_t)(~((uint16_t)TIM_CC1NP));
        tmpccer |= TIM_OCInitStruct->TIM_OCNPolarity;

        tmpccer &= (uint16_t)(~((uint16_t)TIM_CC1NE));
        tmpccer |= TIM_OCInitStruct->TIM_OutputNState;

        tmpcr2 &= (uint16_t)(~((uint16_t)TIM_OIS1));
        tmpcr2 &= (uint16_t)(~((uint16_t)TIM_OIS1N));

        tmpcr2 |= TIM_OCInitStruct->TIM_OCIdleState;
        tmpcr2 |= TIM_OCInitStruct->TIM_OCNIdleState;
    }

    TIMx->CTLR2 = tmpcr2;
    TIMx->CHCTLR1 = tmpccmrx;
    TIMx->CH1CVR = TIM_OCInitStruct->TIM_Pulse;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @fn      TIM_OC2Init
 *
 * @brief   Initializes the TIMx Channel2 according to the specified
 *        parameters in the TIM_OCInitStruct.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCInitStruct - pointer to a TIM_OCInitTypeDef structure.
 *
 * @return  none
 */
void TIM_OC2Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct)
{
    uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

    TIMx->CCER &= (uint16_t)(~((uint16_t)TIM_CC2E));
    tmpccer = TIMx->CCER;
    tmpcr2 = TIMx->CTLR2;
    tmpccmrx = TIMx->CHCTLR1;
    tmpccmrx &= (uint16_t)(~((uint16_t)TIM_OC2M));
    tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CC2S));
    tmpccmrx |= (uint16_t)(TIM_OCInitStruct->TIM_OCMode << 8);
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CC2P));
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCPolarity << 4);
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputState << 4);

    if((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10))
    {
        tmpccer &= (uint16_t)(~((uint16_t)TIM_CC2NP));
        tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCNPolarity << 4);
        tmpccer &= (uint16_t)(~((uint16_t)TIM_CC2NE));
        tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputNState << 4);

        tmpcr2 &= (uint16_t)(~((uint16_t)TIM_OIS2));
        tmpcr2 &= (uint16_t)(~((uint16_t)TIM_OIS2N));
        tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCIdleState << 2);
        tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCNIdleState << 2);
    }

    TIMx->CTLR2 = tmpcr2;
    TIMx->CHCTLR1 = tmpccmrx;
    TIMx->CH2CVR = TIM_OCInitStruct->TIM_Pulse;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @fn      TIM_OC3Init
 *
 * @brief   Initializes the TIMx Channel3 according to the specified
 *        parameters in the TIM_OCInitStruct.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCInitStruct - pointer to a TIM_OCInitTypeDef structure.
 *
 * @return  none
 */
void TIM_OC3Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct)
{
    uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

    TIMx->CCER &= (uint16_t)(~((uint16_t)TIM_CC3E));
    tmpccer = TIMx->CCER;
    tmpcr2 = TIMx->CTLR2;
    tmpccmrx = TIMx->CHCTLR2;
    tmpccmrx &= (uint16_t)(~((uint16_t)TIM_OC3M));
    tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CC3S));
    tmpccmrx |= TIM_OCInitStruct->TIM_OCMode;
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CC3P));
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCPolarity << 8);
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputState << 8);

    if((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10))
    {
        tmpccer &= (uint16_t)(~((uint16_t)TIM_CC3NP));
        tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCNPolarity << 8);
        tmpccer &= (uint16_t)(~((uint16_t)TIM_CC3NE));
        tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputNState << 8);
        tmpcr2 &= (uint16_t)(~((uint16_t)TIM_OIS3));
        tmpcr2 &= (uint16_t)(~((uint16_t)TIM_OIS3N));
        tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCIdleState << 4);
        tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCNIdleState << 4);
    }

    TIMx->CTLR2 = tmpcr2;
    TIMx->CHCTLR2 = tmpccmrx;
    TIMx->CH3CVR = TIM_OCInitStruct->TIM_Pulse;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @fn      TIM_OC4Init
 *
 * @brief   Initializes the TIMx Channel4 according to the specified
 *        parameters in the TIM_OCInitStruct.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCInitStruct - pointer to a TIM_OCInitTypeDef structure.
 *
 * @return  none
 */
void TIM_OC4Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct)
{
    uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

    TIMx->CCER &= (uint16_t)(~((uint16_t)TIM_CC4E));
    tmpccer = TIMx->CCER;
    tmpcr2 = TIMx->CTLR2;
    tmpccmrx = TIMx->CHCTLR2;
    tmpccmrx &= (uint16_t)(~((uint16_t)TIM_OC4M));
    tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CC4S));
    tmpccmrx |= (uint16_t)(TIM_OCInitStruct->TIM_OCMode << 8);
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CC4P));
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCPolarity << 12);
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputState << 12);

    if((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10))
    {
        tmpcr2 &= (uint16_t)(~((uint16_t)TIM_OIS4));
        tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCIdleState << 6);
    }

    TIMx->CTLR2 = tmpcr2;
    TIMx->CHCTLR2 = tmpccmrx;
    TIMx->CH4CVR = TIM_OCInitStruct->TIM_Pulse;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @fn      TIM_ICInit
 *
 * @brief   IInitializes the TIM peripheral according to the specified
 *        parameters in the TIM_ICInitStruct.
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_ICInitStruct - pointer to a TIM_ICInitTypeDef structure.
 *
 * @return  none
 */
void TIM_ICInit(TIM_TypeDef *TIMx, TIM_ICInitTypeDef *TIM_ICInitStruct)
{
    if(TIM_ICInitStruct->TIM_Channel == TIM_Channel_1)
    {
        TI1_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity,
                   TIM_ICInitStruct->TIM_ICSelection,
                   TIM_ICInitStruct->TIM_ICFilter);
        TIM_SetIC1Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
    }
    else if(TIM_ICInitStruct->TIM_Channel == TIM_Channel_2)
    {
        TI2_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity,
                   TIM_ICInitStruct->TIM_ICSelection,
                   TIM_ICInitStruct->TIM_ICFilter);
        TIM_SetIC2Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
    }
    else if(TIM_ICInitStruct->TIM_Channel == TIM_Channel_3)
    {
        TI3_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity,
                   TIM_ICInitStruct->TIM_ICSelection,
                   TIM_ICInitStruct->TIM_ICFilter);
        TIM_SetIC3Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
    }
    else
    {
        TI4_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity,
                   TIM_ICInitStruct->TIM_ICSelection,
                   TIM_ICInitStruct->TIM_ICFilter);
        TIM_SetIC4Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
    }
}

/*********************************************************************
 * @fn      TIM_PWMIConfig
 *
 * @brief   Configures the TIM peripheral according to the specified
 *        parameters in the TIM_ICInitStruct to measure an external
 *        PWM signal.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_ICInitStruct - pointer to a TIM_ICInitTypeDef structure.
 *
 * @return  none
 */
void TIM_PWMIConfig(TIM_TypeDef *TIMx, TIM_ICInitTypeDef *TIM_ICInitStruct)
{
    uint16_t icoppositepolarity = TIM_ICPolarity_Rising;
    uint16_t icoppositeselection = TIM_ICSelection_DirectTI;

    if(TIM_ICInitStruct->TIM_ICPolarity == TIM_ICPolarity_Rising)
    {
        icoppositepolarity = TIM_ICPolarity_Falling;
    }
    else
    {
        icoppositepolarity = TIM_ICPolarity_Rising;
    }

    if(TIM_ICInitStruct->TIM_ICSelection == TIM_ICSelection_DirectTI)
    {
        icoppositeselection = TIM_ICSelection_IndirectTI;
    }
    else
    {
        icoppositeselection = TIM_ICSelection_DirectTI;
    }

    if(TIM_ICInitStruct->TIM_Channel == TIM_Channel_1)
    {
        TI1_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity, TIM_ICInitStruct->TIM_ICSelection,
                   TIM_ICInitStruct->TIM_ICFilter);
        TIM_SetIC1Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
        TI2_Config(TIMx, icoppositepolarity, icoppositeselection, TIM_ICInitStruct->TIM_ICFilter);
        TIM_SetIC2Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
    }
    else
    {
        TI2_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity, TIM_ICInitStruct->TIM_ICSelection,
                   TIM_ICInitStruct->TIM_ICFilter);
        TIM_SetIC2Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
        TI1_Config(TIMx, icoppositepolarity, icoppositeselection, TIM_ICInitStruct->TIM_ICFilter);
        TIM_SetIC1Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
    }
}

/*********************************************************************
 * @fn      TIM_BDTRConfig
 *
 * @brief   Configures the: Break feature, dead time, Lock level, the OSSI,
 *      the OSSR State and the AOE(automatic output enable).
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_BDTRInitStruct - pointer to a TIM_BDTRInitTypeDef structure.
 *
 * @return  none
 */
void TIM_BDTRConfig(TIM_TypeDef *TIMx, TIM_BDTRInitTypeDef *TIM_BDTRInitStruct)
{
    TIMx->BDTR = (uint32_t)TIM_BDTRInitStruct->TIM_OSSRState | TIM_BDTRInitStruct->TIM_OSSIState |
                 TIM_BDTRInitStruct->TIM_LOCKLevel | TIM_BDTRInitStruct->TIM_DeadTime |
                 TIM_BDTRInitStruct->TIM_Break | TIM_BDTRInitStruct->TIM_BreakPolarity |
                 TIM_BDTRInitStruct->TIM_AutomaticOutput;
}

/*********************************************************************
 * @fn      TIM_TimeBaseStructInit
 *
 * @brief   Fills each TIM_TimeBaseInitStruct member with its default value.
 *
 * @param   TIM_TimeBaseInitStruct - pointer to a TIM_TimeBaseInitTypeDef structure.
 *
 * @return  none
 */
void TIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct)
{
    TIM_TimeBaseInitStruct->TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStruct->TIM_Prescaler = 0x0000;
    TIM_TimeBaseInitStruct->TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct->TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct->TIM_RepetitionCounter = 0x0000;
}

/*********************************************************************
 * @fn      TIM_OCStructInit
 *
 * @brief   Fills each TIM_OCInitStruct member with its default value.
 *
 * @param   TIM_OCInitStruct - pointer to a TIM_OCInitTypeDef structure.
 *
 * @return  none
 */
void TIM_OCStructInit(TIM_OCInitTypeDef *TIM_OCInitStruct)
{
    TIM_OCInitStruct->TIM_OCMode = TIM_OCMode_Timing;
    TIM_OCInitStruct->TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStruct->TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStruct->TIM_Pulse = 0x0000;
    TIM_OCInitStruct->TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStruct->TIM_OCNPolarity = TIM_OCPolarity_High;
    TIM_OCInitStruct->TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStruct->TIM_OCNIdleState = TIM_OCNIdleState_Reset;
}

/*********************************************************************
 * @fn      TIM_ICStructInit
 *
 * @brief   Fills each TIM_ICInitStruct member with its default value.
 *
 * @param   TIM_ICInitStruct - pointer to a TIM_ICInitTypeDef structure.
 *
 * @return  none
 */
void TIM_ICStructInit(TIM_ICInitTypeDef *TIM_ICInitStruct)
{
    TIM_ICInitStruct->TIM_Channel = TIM_Channel_1;
    TIM_ICInitStruct->TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStruct->TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStruct->TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStruct->TIM_ICFilter = 0x00;
}

/*********************************************************************
 * @fn      TIM_BDTRStructInit
 *
 * @brief   Fills each TIM_BDTRInitStruct member with its default value.
 *
 * @param   TIM_BDTRInitStruct - pointer to a TIM_BDTRInitTypeDef structure.
 *
 * @return  none
 */
void TIM_BDTRStructInit(TIM_BDTRInitTypeDef *TIM_BDTRInitStruct)
{
    TIM_BDTRInitStruct->TIM_OSSRState = TIM_OSSRState_Disable;
    TIM_BDTRInitStruct->TIM_OSSIState = TIM_OSSIState_Disable;
    TIM_BDTRInitStruct->TIM_LOCKLevel = TIM_LOCKLevel_OFF;
    TIM_BDTRInitStruct->TIM_DeadTime = 0x00;
    TIM_BDTRInitStruct->TIM_Break = TIM_Break_Disable;
    TIM_BDTRInitStruct->TIM_BreakPolarity = TIM_BreakPolarity_Low;
    TIM_BDTRInitStruct->TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
}

/*********************************************************************
 * @fn      TIM_Cmd
 *
 * @brief   Enables or disables the specified TIM peripheral.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void TIM_Cmd(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->CTLR1 |= TIM_CEN;
    }
    else
    {
        TIMx->CTLR1 &= (uint16_t)(~((uint16_t)TIM_CEN));
    }
}

/*********************************************************************
 * @fn      TIM_CtrlPWMOutputs
 *
 * @brief   Enables or disables the TIM peripheral Main Outputs.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void TIM_CtrlPWMOutputs(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->BDTR |= TIM_MOE;
    }
    else
    {
        TIMx->BDTR &= (uint16_t)(~((uint16_t)TIM_MOE));
    }
}

/*********************************************************************
 * @fn      TIM_ITConfig
 *
 * @brief   Enables or disables the specified TIM interrupts.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_IT - specifies the TIM interrupts sources to be enabled or disabled.
 *            TIM_IT_Update - TIM update Interrupt source.
 *            TIM_IT_CC1 - TIM Capture Compare 1 Interrupt source.
 *            TIM_IT_CC2 - TIM Capture Compare 2 Interrupt source
 *            TIM_IT_CC3 - TIM Capture Compare 3 Interrupt source.
 *            TIM_IT_CC4 - TIM Capture Compare 4 Interrupt source.
 *            TIM_IT_COM - TIM Commutation Interrupt source.
 *            TIM_IT_Trigger - TIM Trigger Interrupt source.
 *            TIM_IT_Break - TIM Break Interrupt source.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void TIM_ITConfig(TIM_TypeDef *TIMx, uint16_t TIM_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->DMAINTENR |= TIM_IT;
    }
    else
    {
        TIMx->DMAINTENR &= (uint16_t)~TIM_IT;
    }
}

void TIM_GenerateEvent(TIM_TypeDef *TIMx, uint16_t TIM_EventSource)
{
    TIMx->SWEVGR = TIM_EventSource;
}

/*********************************************************************
 * @fn      TIM_DMAConfig
 *
 * @brief   Configures the TIMx's DMA interface.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_DMABase: DMA Base address.
 *            TIM_DMABase_CR.
 *            TIM_DMABase_CR2.
 *            TIM_DMABase_SMCR.
 *            TIM_DMABase_DIER.
 *            TIM1_DMABase_SR.
 *            TIM_DMABase_EGR.
 *            TIM_DMABase_CCMR1.
 *            TIM_DMABase_CCMR2.
 *            TIM_DMABase_CCER.
 *            TIM_DMABase_CNT.
 *            TIM_DMABase_PSC.
 *            TIM_DMABase_CCR1.
 *            TIM_DMABase_CCR2.
 *            TIM_DMABase_CCR3.
 *            TIM_DMABase_CCR4.
 *            TIM_DMABase_BDTR.
 *            TIM_DMABase_DCR.
 *          TIM_DMABurstLength - DMA Burst length.
 *            TIM_DMABurstLength_1Transfer.
 *            TIM_DMABurstLength_18Transfers.
 *
 * @return  none
 */
void TIM_DMAConfig(TIM_TypeDef *TIMx, uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength)
{
    TIMx->DMACFGR = TIM_DMABase | TIM_DMABurstLength;
}

/*********************************************************************
 * @fn      TIM_DMACmd
 *
 * @brief   Enables or disables the TIMx's DMA Requests.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_DMASource - specifies the DMA Request sources.
 *            TIM_DMA_Update - TIM update Interrupt source.
 *            TIM_DMA_CC1 - TIM Capture Compare 1 DMA source.
 *            TIM_DMA_CC2 - TIM Capture Compare 2 DMA source.
 *            TIM_DMA_CC3 - TIM Capture Compare 3 DMA source.
 *            TIM_DMA_CC4 - TIM Capture Compare 4 DMA source.
 *            TIM_DMA_COM - TIM Commutation DMA source.
 *            TIM_DMA_Trigger - TIM Trigger DMA source.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void TIM_DMACmd(TIM_TypeDef *TIMx, uint16_t TIM_DMASource, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->DMAINTENR |= TIM_DMASource;
    }
    else
    {
        TIMx->DMAINTENR &= (uint16_t)~TIM_DMASource;
    }
}

/*********************************************************************
 * @fn      TIM_InternalClockConfig
 *
 * @brief   Configures the TIMx internal Clock.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *
 * @return  none
 */
void TIM_InternalClockConfig(TIM_TypeDef *TIMx)
{
    TIMx->SMCFGR &= (uint16_t)(~((uint16_t)TIM_SMS));
}

/*********************************************************************
 * @fn      TIM_ITRxExternalClockConfig
 *
 * @brief   Configures the TIMx Internal Trigger as External Clock.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_InputTriggerSource: Trigger source.
 *            TIM_TS_ITR0 - Internal Trigger 0.
 *            TIM_TS_ITR1 - Internal Trigger 1.
 *            TIM_TS_ITR2 - Internal Trigger 2.
 *            TIM_TS_ITR3 - Internal Trigger 3.
 *
 * @return  none
 */
void TIM_ITRxExternalClockConfig(TIM_TypeDef *TIMx, uint16_t TIM_InputTriggerSource)
{
    TIM_SelectInputTrigger(TIMx, TIM_InputTriggerSource);
    TIMx->SMCFGR |= TIM_SlaveMode_External1;
}

/*********************************************************************
 * @fn      TIM_TIxExternalClockConfig
 *
 * @brief   Configures the TIMx Trigger as External Clock.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_TIxExternalCLKSource - Trigger source.
 *            TIM_TIxExternalCLK1Source_TI1ED - TI1 Edge Detector.
 *            TIM_TIxExternalCLK1Source_TI1 - Filtered Timer Input 1.
 *            TIM_TIxExternalCLK1Source_TI2 - Filtered Timer Input 2.
 *          TIM_ICPolarity - specifies the TIx Polarity.
 *             TIM_ICPolarity_Rising.
 *             TIM_ICPolarity_Falling.
 *             TIM_DMA_COM - TIM Commutation DMA source.
 *             TIM_DMA_Trigger - TIM Trigger DMA source.
 *          ICFilter - specifies the filter value.
 *             This parameter must be a value between 0x0 and 0xF.
 *
 * @return  none
 */
void TIM_TIxExternalClockConfig(TIM_TypeDef *TIMx, uint16_t TIM_TIxExternalCLKSource,
                                uint16_t TIM_ICPolarity, uint16_t ICFilter)
{
    if(TIM_TIxExternalCLKSource == TIM_TIxExternalCLK1Source_TI2)
    {
        TI2_Config(TIMx, TIM_ICPolarity, TIM_ICSelection_DirectTI, ICFilter);
    }
    else
    {
        TI1_Config(TIMx, TIM_ICPolarity, TIM_ICSelection_DirectTI, ICFilter);
    }

    TIM_SelectInputTrigger(TIMx, TIM_TIxExternalCLKSource);
    TIMx->SMCFGR |= TIM_SlaveMode_External1;
}

/*********************************************************************
 * @fn      TIM_ETRClockMode1Config
 *
 * @brief   Configures the External clock Mode1.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_ExtTRGPrescaler - The external Trigger Prescaler.
 *            TIM_ExtTRGPSC_OFF - ETRP Prescaler OFF.
 *            TIM_ExtTRGPSC_DIV2 - ETRP frequency divided by 2.
 *            TIM_ExtTRGPSC_DIV4 - ETRP frequency divided by 4.
 *            TIM_ExtTRGPSC_DIV8 - ETRP frequency divided by 8.
 *          TIM_ExtTRGPolarity - The external Trigger Polarity.
 *            TIM_ExtTRGPolarity_Inverted - active low or falling edge active.
 *            TIM_ExtTRGPolarity_NonInverted - active high or rising edge active.
 *          ExtTRGFilter - External Trigger Filter.
 *             This parameter must be a value between 0x0 and 0xF.
 *
 * @return  none
 */
void TIM_ETRClockMode1Config(TIM_TypeDef *TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                             uint16_t ExtTRGFilter)
{
    uint16_t tmpsmcr = 0;

    TIM_ETRConfig(TIMx, TIM_ExtTRGPrescaler, TIM_ExtTRGPolarity, ExtTRGFilter);
    tmpsmcr = TIMx->SMCFGR;
    tmpsmcr &= (uint16_t)(~((uint16_t)TIM_SMS));
    tmpsmcr |= TIM_SlaveMode_External1;
    tmpsmcr &= (uint16_t)(~((uint16_t)TIM_TS));
    tmpsmcr |= TIM_TS_ETRF;
    TIMx->SMCFGR = tmpsmcr;
}

/*********************************************************************
 * @fn      TIM_ETRClockMode2Config
 *
 * @brief   Configures the External clock Mode2.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_ExtTRGPrescaler - The external Trigger Prescaler.
 *            TIM_ExtTRGPSC_OFF - ETRP Prescaler OFF.
 *            TIM_ExtTRGPSC_DIV2 - ETRP frequency divided by 2.
 *            TIM_ExtTRGPSC_DIV4 - ETRP frequency divided by 4.
 *            TIM_ExtTRGPSC_DIV8 - ETRP frequency divided by 8.
 *          TIM_ExtTRGPolarity - The external Trigger Polarity.
 *            TIM_ExtTRGPolarity_Inverted - active low or falling edge active.
 *            TIM_ExtTRGPolarity_NonInverted - active high or rising edge active.
 *          ExtTRGFilter - External Trigger Filter.
 *            This parameter must be a value between 0x0 and 0xF.
 *
 * @return  none
 */
void TIM_ETRClockMode2Config(TIM_TypeDef *TIMx, uint16_t TIM_ExtTRGPrescaler,
                             uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter)
{
    TIM_ETRConfig(TIMx, TIM_ExtTRGPrescaler, TIM_ExtTRGPolarity, ExtTRGFilter);
    TIMx->SMCFGR |= TIM_ECE;
}

/*********************************************************************
 * @fn      TIM_ETRConfig
 *
 * @brief   Configures the TIMx External Trigger (ETR).
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_ExtTRGPrescaler - The external Trigger Prescaler.
 *            TIM_ExtTRGPSC_OFF - ETRP Prescaler OFF.
 *            TIM_ExtTRGPSC_DIV2 - ETRP frequency divided by 2.
 *            TIM_ExtTRGPSC_DIV4 - ETRP frequency divided by 4.
 *            TIM_ExtTRGPSC_DIV8 - ETRP frequency divided by 8.
 *          TIM_ExtTRGPolarity - The external Trigger Polarity.
 *            TIM_ExtTRGPolarity_Inverted - active low or falling edge active.
 *            TIM_ExtTRGPolarity_NonInverted - active high or rising edge active.
 *          ExtTRGFilter - External Trigger Filter.
 *            This parameter must be a value between 0x0 and 0xF.
 *
 * @return  none
 */
void TIM_ETRConfig(TIM_TypeDef *TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                   uint16_t ExtTRGFilter)
{
    uint16_t tmpsmcr = 0;

    tmpsmcr = TIMx->SMCFGR;
    tmpsmcr &= SMCFGR_ETR_Mask;
    tmpsmcr |= (uint16_t)(TIM_ExtTRGPrescaler | (uint16_t)(TIM_ExtTRGPolarity | (uint16_t)(ExtTRGFilter << (uint16_t)8)));
    TIMx->SMCFGR = tmpsmcr;
}

/*********************************************************************
 * @fn      TIM_PrescalerConfig
 *
 * @brief   Configures the TIMx Prescaler.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          Prescaler - specifies the Prescaler Register value.
 *          TIM_PSCReloadMode - specifies the TIM Prescaler Reload mode.
 *            TIM_PSCReloadMode - specifies the TIM Prescaler Reload mode.
 *            TIM_PSCReloadMode_Update - The Prescaler is loaded at the update event.
 *            TIM_PSCReloadMode_Immediate - The Prescaler is loaded immediately.
 *
 * @return  none
 */
void TIM_PrescalerConfig(TIM_TypeDef *TIMx, uint16_t Prescaler, uint16_t TIM_PSCReloadMode)
{
    TIMx->PSC = Prescaler;
    TIMx->SWEVGR = TIM_PSCReloadMode;
}

/*********************************************************************
 * @fn      TIM_CounterModeConfig
 *
 * @brief   Specifies the TIMx Counter Mode to be used.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_CounterMode - specifies the Counter Mode to be used.
 *            TIM_CounterMode_Up - TIM Up Counting Mode.
 *            TIM_CounterMode_Down - TIM Down Counting Mode.
 *            TIM_CounterMode_CenterAligned1 - TIM Center Aligned Mode1.
 *            TIM_CounterMode_CenterAligned2 - TIM Center Aligned Mode2.
 *            TIM_CounterMode_CenterAligned3 - TIM Center Aligned Mode3.
 *
 * @return  none
 */
void TIM_CounterModeConfig(TIM_TypeDef *TIMx, uint16_t TIM_CounterMode)
{
    uint16_t tmpcr1 = 0;

    tmpcr1 = TIMx->CTLR1;
    tmpcr1 &= (uint16_t)(~((uint16_t)(TIM_DIR | TIM_CMS)));
    tmpcr1 |= TIM_CounterMode;
    TIMx->CTLR1 = tmpcr1;
}

/*********************************************************************
 * @fn      TIM_SelectInputTrigger
 *
 * @brief   Selects the Input Trigger source.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_InputTriggerSource - The Input Trigger source.
 *            TIM_TS_ITR0 - Internal Trigger 0.
 *            TIM_TS_ITR1 - Internal Trigger 1.
 *            TIM_TS_ITR2 - Internal Trigger 2.
 *            TIM_TS_ITR3 - Internal Trigger 3.
 *            TIM_TS_TI1F_ED - TI1 Edge Detector.
 *            TIM_TS_TI1FP1 - Filtered Timer Input 1.
 *            TIM_TS_TI2FP2 - Filtered Timer Input 2.
 *            TIM_TS_ETRF - External Trigger input.
 *
 * @return  none
 */
void TIM_SelectInputTrigger(TIM_TypeDef *TIMx, uint16_t TIM_InputTriggerSource)
{
    uint16_t tmpsmcr = 0;

    tmpsmcr = TIMx->SMCFGR;
    tmpsmcr &= (uint16_t)(~((uint16_t)TIM_TS));
    tmpsmcr |= TIM_InputTriggerSource;
    TIMx->SMCFGR = tmpsmcr;
}

/*********************************************************************
 * @fn      TIM_EncoderInterfaceConfig
 *
 * @brief   Configures the TIMx Encoder Interface.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_EncoderMode - specifies the TIMx Encoder Mode.
 *            TIM_EncoderMode_TI1 - Counter counts on TI1FP1 edge depending
 *        on TI2FP2 level.
 *            TIM_EncoderMode_TI2 - Counter counts on TI2FP2 edge depending
 *        on TI1FP1 level.
 *            TIM_EncoderMode_TI12 - Counter counts on both TI1FP1 and
 *        TI2FP2 edges depending.
 *          TIM_IC1Polarity - specifies the IC1 Polarity.
 *            TIM_ICPolarity_Falling - IC Falling edge.
 *            TTIM_ICPolarity_Rising - IC Rising edge.
 *          TIM_IC2Polarity - specifies the IC2 Polarity.
 *            TIM_ICPolarity_Falling - IC Falling edge.
 *            TIM_ICPolarity_Rising - IC Rising edge.
 *
 * @return  none
 */
void TIM_EncoderInterfaceConfig(TIM_TypeDef *TIMx, uint16_t TIM_EncoderMode,
                                uint16_t TIM_IC1Polarity, uint16_t TIM_IC2Polarity)
{
    uint16_t tmpsmcr = 0;
    uint16_t tmpccmr1 = 0;
    uint16_t tmpccer = 0;

    tmpsmcr = TIMx->SMCFGR;
    tmpccmr1 = TIMx->CHCTLR1;
    tmpccer = TIMx->CCER;
    tmpsmcr &= (uint16_t)(~((uint16_t)TIM_SMS));
    tmpsmcr |= TIM_EncoderMode;
    tmpccmr1 &= (uint16_t)(((uint16_t) ~((uint16_t)TIM_CC1S)) & (uint16_t)(~((uint16_t)TIM_CC2S)));
    tmpccmr1 |= TIM_CC1S_0 | TIM_CC2S_0;
    tmpccer &= (uint16_t)(((uint16_t) ~((uint16_t)TIM_CC1P)) & ((uint16_t) ~((uint16_t)TIM_CC2P)));
    tmpccer |= (uint16_t)(TIM_IC1Polarity | (uint16_t)(TIM_IC2Polarity << (uint16_t)4));
    TIMx->SMCFGR = tmpsmcr;
    TIMx->CHCTLR1 = tmpccmr1;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @fn      TIM_ForcedOC1Config
 *
 * @brief   Forces the TIMx output 1 waveform to active or inactive level.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_ForcedAction - specifies the forced Action to be set to the
 *        output waveform.
 *            TIM_ForcedAction_Active - Force active level on OC1REF.
 *            TIM_ForcedAction_InActive - Force inactive level on OC1REF.
 *
 * @return  none
 */
void TIM_ForcedOC1Config(TIM_TypeDef *TIMx, uint16_t TIM_ForcedAction)
{
    uint16_t tmpccmr1 = 0;

    tmpccmr1 = TIMx->CHCTLR1;
    tmpccmr1 &= (uint16_t) ~((uint16_t)TIM_OC1M);
    tmpccmr1 |= TIM_ForcedAction;
    TIMx->CHCTLR1 = tmpccmr1;
}

/*********************************************************************
 * @fn      TIM_ForcedOC2Config
 *
 * @brief   Forces the TIMx output 2 waveform to active or inactive level.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_ForcedAction - specifies the forced Action to be set to the
 *        output waveform.
 *            TIM_ForcedAction_Active - Force active level on OC2REF.
 *            TIM_ForcedAction_InActive - Force inactive level on OC2REF.
 *
 * @return  none
 */
void TIM_ForcedOC2Config(TIM_TypeDef *TIMx, uint16_t TIM_ForcedAction)
{
    uint16_t tmpccmr1 = 0;

    tmpccmr1 = TIMx->CHCTLR1;
    tmpccmr1 &= (uint16_t) ~((uint16_t)TIM_OC2M);
    tmpccmr1 |= (uint16_t)(TIM_ForcedAction << 8);
    TIMx->CHCTLR1 = tmpccmr1;
}

/*********************************************************************
 * @fn      TIM_ForcedOC3Config
 *
 * @brief   Forces the TIMx output 3 waveform to active or inactive level.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_ForcedAction - specifies the forced Action to be set to the
 *        output waveform.
 *            TIM_ForcedAction_Active - Force active level on OC3REF.
 *            TIM_ForcedAction_InActive - Force inactive level on OC3REF.
 *
 * @return  none
 */
void TIM_ForcedOC3Config(TIM_TypeDef *TIMx, uint16_t TIM_ForcedAction)
{
    uint16_t tmpccmr2 = 0;

    tmpccmr2 = TIMx->CHCTLR2;
    tmpccmr2 &= (uint16_t) ~((uint16_t)TIM_OC3M);
    tmpccmr2 |= TIM_ForcedAction;
    TIMx->CHCTLR2 = tmpccmr2;
}

/*********************************************************************
 * @fn      TIM_ForcedOC4Config
 *
 * @brief   Forces the TIMx output 4 waveform to active or inactive level.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_ForcedAction - specifies the forced Action to be set to the
 *        output waveform.
 *            TIM_ForcedAction_Active - Force active level on OC4REF.
 *            TIM_ForcedAction_InActive - Force inactive level on OC4REF.
 *
 * @return  none
 */
void TIM_ForcedOC4Config(TIM_TypeDef *TIMx, uint16_t TIM_ForcedAction)
{
    uint16_t tmpccmr2 = 0;

    tmpccmr2 = TIMx->CHCTLR2;
    tmpccmr2 &= (uint16_t) ~((uint16_t)TIM_OC4M);
    tmpccmr2 |= (uint16_t)(TIM_ForcedAction << 8);
    TIMx->CHCTLR2 = tmpccmr2;
}

/*********************************************************************
 * @fn      TIM_ARRPreloadConfig
 *
 * @brief   Enables or disables TIMx peripheral Preload register on ARR.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void TIM_ARRPreloadConfig(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->CTLR1 |= TIM_ARPE;
    }
    else
    {
        TIMx->CTLR1 &= (uint16_t) ~((uint16_t)TIM_ARPE);
    }
}

/*********************************************************************
 * @fn      TIM_SelectCOM
 *
 * @brief   Selects the TIM peripheral Commutation event.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void TIM_SelectCOM(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->CTLR2 |= TIM_CCUS;
    }
    else
    {
        TIMx->CTLR2 &= (uint16_t) ~((uint16_t)TIM_CCUS);
    }
}

/*********************************************************************
 * @fn      TIM_SelectCCDMA
 *
 * @brief   Selects the TIMx peripheral Capture Compare DMA source.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void TIM_SelectCCDMA(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->CTLR2 |= TIM_CCDS;
    }
    else
    {
        TIMx->CTLR2 &= (uint16_t) ~((uint16_t)TIM_CCDS);
    }
}

/*********************************************************************
 * @fn      TIM_CCPreloadControl
 *
 * @brief   DSets or Resets the TIM peripheral Capture Compare Preload Control bit.
 *        reset values (Affects also the I2Ss).
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void TIM_CCPreloadControl(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->CTLR2 |= TIM_CCPC;
    }
    else
    {
        TIMx->CTLR2 &= (uint16_t) ~((uint16_t)TIM_CCPC);
    }
}

/*********************************************************************
 * @fn      TIM_OC1PreloadConfig
 *
 * @brief   Enables or disables the TIMx peripheral Preload register on CCR1.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCPreload - new state of the TIMx peripheral Preload register.
 *            TIM_OCPreload_Enable.
 *            TIM_OCPreload_Disable.
 *
 * @return  none
 */
void TIM_OC1PreloadConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPreload)
{
    uint16_t tmpccmr1 = 0;

    tmpccmr1 = TIMx->CHCTLR1;
    tmpccmr1 &= (uint16_t) ~((uint16_t)TIM_OC1PE);
    tmpccmr1 |= TIM_OCPreload;
    TIMx->CHCTLR1 = tmpccmr1;
}

/*********************************************************************
 * @fn      TIM_OC2PreloadConfig
 *
 * @brief   Enables or disables the TIMx peripheral Preload register on CCR2.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCPreload - new state of the TIMx peripheral Preload register.
 *            TIM_OCPreload_Enable.
 *            TIM_OCPreload_Disable.
 *
 * @return  none
 */
void TIM_OC2PreloadConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPreload)
{
    uint16_t tmpccmr1 = 0;

    tmpccmr1 = TIMx->CHCTLR1;
    tmpccmr1 &= (uint16_t) ~((uint16_t)TIM_OC2PE);
    tmpccmr1 |= (uint16_t)(TIM_OCPreload << 8);
    TIMx->CHCTLR1 = tmpccmr1;
}

/*********************************************************************
 * @fn      TIM_OC3PreloadConfig
 *
 * @brief   Enables or disables the TIMx peripheral Preload register on CCR3.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCPreload - new state of the TIMx peripheral Preload register.
 *            TIM_OCPreload_Enable.
 *            TIM_OCPreload_Disable.
 *
 * @return  none
 */
void TIM_OC3PreloadConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPreload)
{
    uint16_t tmpccmr2 = 0;

    tmpccmr2 = TIMx->CHCTLR2;
    tmpccmr2 &= (uint16_t) ~((uint16_t)TIM_OC3PE);
    tmpccmr2 |= TIM_OCPreload;
    TIMx->CHCTLR2 = tmpccmr2;
}

/*********************************************************************
 * @fn      TIM_OC4PreloadConfig
 *
 * @brief   Enables or disables the TIMx peripheral Preload register on CCR4.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCPreload - new state of the TIMx peripheral Preload register.
 *            TIM_OCPreload_Enable.
 *            TIM_OCPreload_Disable.
 *
 * @return  none
 */
void TIM_OC4PreloadConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPreload)
{
    uint16_t tmpccmr2 = 0;

    tmpccmr2 = TIMx->CHCTLR2;
    tmpccmr2 &= (uint16_t) ~((uint16_t)TIM_OC4PE);
    tmpccmr2 |= (uint16_t)(TIM_OCPreload << 8);
    TIMx->CHCTLR2 = tmpccmr2;
}

/*********************************************************************
 * @fn      TIM_OC1FastConfig
 *
 * @brief   Configures the TIMx Output Compare 1 Fast feature.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCFast - new state of the Output Compare Fast Enable Bit.
 *            TIM_OCFast_Enable - TIM output compare fast enable.
 *            TIM_OCFast_Disable - TIM output compare fast disable.
 *
 * @return  none
 */
void TIM_OC1FastConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCFast)
{
    uint16_t tmpccmr1 = 0;

    tmpccmr1 = TIMx->CHCTLR1;
    tmpccmr1 &= (uint16_t) ~((uint16_t)TIM_OC1FE);
    tmpccmr1 |= TIM_OCFast;
    TIMx->CHCTLR1 = tmpccmr1;
}

/*********************************************************************
 * @fn      TIM_OC2FastConfig
 *
 * @brief   Configures the TIMx Output Compare 2 Fast feature.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCFast - new state of the Output Compare Fast Enable Bit.
 *            TIM_OCFast_Enable - TIM output compare fast enable.
 *            TIM_OCFast_Disable - TIM output compare fast disable.
 *
 * @return  none
 */
void TIM_OC2FastConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCFast)
{
    uint16_t tmpccmr1 = 0;

    tmpccmr1 = TIMx->CHCTLR1;
    tmpccmr1 &= (uint16_t) ~((uint16_t)TIM_OC2FE);
    tmpccmr1 |= (uint16_t)(TIM_OCFast << 8);
    TIMx->CHCTLR1 = tmpccmr1;
}

/*********************************************************************
 * @fn      TIM_OC3FastConfig
 *
 * @brief   Configures the TIMx Output Compare 3 Fast feature.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCFast - new state of the Output Compare Fast Enable Bit.
 *            TIM_OCFast_Enable - TIM output compare fast enable.
 *            TIM_OCFast_Disable - TIM output compare fast disable.
 *
 * @return  none
 */
void TIM_OC3FastConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCFast)
{
    uint16_t tmpccmr2 = 0;

    tmpccmr2 = TIMx->CHCTLR2;
    tmpccmr2 &= (uint16_t) ~((uint16_t)TIM_OC3FE);
    tmpccmr2 |= TIM_OCFast;
    TIMx->CHCTLR2 = tmpccmr2;
}

/*********************************************************************
 * @fn      TIM_OC4FastConfig
 *
 * @brief   Configures the TIMx Output Compare 4 Fast feature.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCFast - new state of the Output Compare Fast Enable Bit.
 *            TIM_OCFast_Enable - TIM output compare fast enable.
 *            TIM_OCFast_Disable - TIM output compare fast disable.
 *
 * @return  none
 */
void TIM_OC4FastConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCFast)
{
    uint16_t tmpccmr2 = 0;

    tmpccmr2 = TIMx->CHCTLR2;
    tmpccmr2 &= (uint16_t) ~((uint16_t)TIM_OC4FE);
    tmpccmr2 |= (uint16_t)(TIM_OCFast << 8);
    TIMx->CHCTLR2 = tmpccmr2;
}

/*********************************************************************
 * @fn      TIM_ClearOC1Ref
 *
 * @brief   Clears or safeguards the OCREF1 signal on an external event.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCClear - new state of the Output Compare Clear Enable Bit.
 *            TIM_OCClear_Enable - TIM Output clear enable.
 *            TIM_OCClear_Disable - TIM Output clear disable.
 *
 * @return  none
 */
void TIM_ClearOC1Ref(TIM_TypeDef *TIMx, uint16_t TIM_OCClear)
{
    uint16_t tmpccmr1 = 0;

    tmpccmr1 = TIMx->CHCTLR1;
    tmpccmr1 &= (uint16_t) ~((uint16_t)TIM_OC1CE);
    tmpccmr1 |= TIM_OCClear;
    TIMx->CHCTLR1 = tmpccmr1;
}

/*********************************************************************
 * @fn      TIM_ClearOC2Ref
 *
 * @brief   Clears or safeguards the OCREF2 signal on an external event.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCClear - new state of the Output Compare Clear Enable Bit.
 *            TIM_OCClear_Enable - TIM Output clear enable.
 *            TIM_OCClear_Disable - TIM Output clear disable.
 *
 * @return  none
 */
void TIM_ClearOC2Ref(TIM_TypeDef *TIMx, uint16_t TIM_OCClear)
{
    uint16_t tmpccmr1 = 0;

    tmpccmr1 = TIMx->CHCTLR1;
    tmpccmr1 &= (uint16_t) ~((uint16_t)TIM_OC2CE);
    tmpccmr1 |= (uint16_t)(TIM_OCClear << 8);
    TIMx->CHCTLR1 = tmpccmr1;
}

/*********************************************************************
 * @fn      TIM_ClearOC3Ref
 *
 * @brief   Clears or safeguards the OCREF3 signal on an external event.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCClear - new state of the Output Compare Clear Enable Bit.
 *            TIM_OCClear_Enable - TIM Output clear enable.
 *            TIM_OCClear_Disable - TIM Output clear disable.
 *
 * @return  none
 */
void TIM_ClearOC3Ref(TIM_TypeDef *TIMx, uint16_t TIM_OCClear)
{
    uint16_t tmpccmr2 = 0;

    tmpccmr2 = TIMx->CHCTLR2;
    tmpccmr2 &= (uint16_t) ~((uint16_t)TIM_OC3CE);
    tmpccmr2 |= TIM_OCClear;
    TIMx->CHCTLR2 = tmpccmr2;
}

/*********************************************************************
 * @fn      TIM_ClearOC4Ref
 *
 * @brief   Clears or safeguards the OCREF4 signal on an external event.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCClear - new state of the Output Compare Clear Enable Bit.
 *            TIM_OCClear_Enable - TIM Output clear enable.
 *            TIM_OCClear_Disable - TIM Output clear disable.
 *
 * @return  none
 */
void TIM_ClearOC4Ref(TIM_TypeDef *TIMx, uint16_t TIM_OCClear)
{
    uint16_t tmpccmr2 = 0;

    tmpccmr2 = TIMx->CHCTLR2;
    tmpccmr2 &= (uint16_t) ~((uint16_t)TIM_OC4CE);
    tmpccmr2 |= (uint16_t)(TIM_OCClear << 8);
    TIMx->CHCTLR2 = tmpccmr2;
}

/*********************************************************************
 * @fn      TIM_OC1PolarityConfig
 *
 * @brief   Configures the TIMx channel 1 polarity.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCPolarity - specifies the OC1 Polarity.
 *            TIM_OCPolarity_High - Output Compare active high.
 *            TIM_OCPolarity_Low - Output Compare active low.
 *
 * @return  none
 */
void TIM_OC1PolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPolarity)
{
    uint16_t tmpccer = 0;

    tmpccer = TIMx->CCER;
    tmpccer &= (uint16_t) ~((uint16_t)TIM_CC1P);
    tmpccer |= TIM_OCPolarity;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @fn      TIM_OC1NPolarityConfig
 *
 * @brief   Configures the TIMx channel 1 polarity.
 *
 * @param   TIMx - where x can be 1 to select the TIM peripheral.
 *          TIM_OCNPolarity - specifies the OC1N Polarity.
 *            TIM_OCNPolarity_High - Output Compare active high.
 *            TIM_OCNPolarity_Low - Output Compare active low.
 *
 * @return  none
 */
void TIM_OC1NPolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCNPolarity)
{
    uint16_t tmpccer = 0;

    tmpccer = TIMx->CCER;
    tmpccer &= (uint16_t) ~((uint16_t)TIM_CC1NP);
    tmpccer |= TIM_OCNPolarity;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @fn      TIM_OC2PolarityConfig
 *
 * @brief   Configures the TIMx channel 2 polarity.
 *
 * @param   TIMx - where x can be 1 to 4 to select the TIM peripheral.
 *          TIM_OCPolarity - specifies the OC2 Polarity.
 *            TIM_OCPolarity_High - Output Compare active high.
 *            TIM_OCPolarity_Low - Output Compare active low.
 *
 * @return  none
 */
void TIM_OC2PolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPolarity)
{
    uint16_t tmpccer = 0;

    tmpccer = TIMx->CCER;
    tmpccer &= (uint16_t) ~((uint16_t)TIM_CC2P);
    tmpccer |= (uint16_t)(TIM_OCPolarity << 4);
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @fn      TIM_OC2NPolarityConfig
 *
 * @brief   Configures the TIMx channel 2 polarity.
 *
 * @param   TIMx - where x can be 1 to select the TIM peripheral.
 *          TIM_OCNPolarity - specifies the OC1N Polarity.
 *            TIM_OCNPolarity_High - Output Compare active high.
 *            TIM_OCNPolarity_Low - Output Compare active low.
 *
 * @return  none
 */
void TIM_OC2NPolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCNPolarity)
{
    uint16_t tmpccer = 0;

    tmpccer = TIMx->CCER;
    tmpccer &= (uint16_t) ~((uint16_t)TIM_CC2NP);
    tmpccer |= (uint16_t)(TIM_OCNPolarity << 4);
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @fn      TIM_OC3PolarityConfig
 *
 * @brief   Configures the TIMx Channel 3 polarity.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_OCPolarit - specifies the OC3 Polarity.
 *            TIM_OCPolarity_High - Output Compare active high.
 *            TIM_OCPolarity_Low - Output Compare active low.
 *
 * @return  none
 */
void TIM_OC3PolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPolarity)
{
    uint16_t tmpccer = 0;

    tmpccer = TIMx->CCER;
    tmpccer &= (uint16_t) ~((uint16_t)TIM_CC3P);
    tmpccer |= (uint16_t)(TIM_OCPolarity << 8);
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @fn      TIM_OC3NPolarityConfig
 *
 * @brief   Configures the TIMx Channel 3N polarity.
 *
 * @param   TIMx - where x can be 1 to select the TIM peripheral.
 *          TIM_OCNPolarity - specifies the OC2N Polarity.
 *            TIM_OCNPolarity_High - Output Compare active high.
 *            TIM_OCNPolarity_Low - Output Compare active low.
 *
 * @return  none
 */
void TIM_OC3NPolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCNPolarity)
{
    uint16_t tmpccer = 0;

    tmpccer = TIMx->CCER;
    tmpccer &= (uint16_t) ~((uint16_t)TIM_CC3NP);
    tmpccer |= (uint16_t)(TIM_OCNPolarity << 8);
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @fn      TIM_OC4PolarityConfig
 *
 * @brief   Configures the TIMx Channel 4 polarity.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_OCPolarit - specifies the OC3 Polarity.
 *            TIM_OCPolarity_High - Output Compare active high.
 *            TIM_OCPolarity_Low - Output Compare active low.
 *
 * @return  none
 */
void TIM_OC4PolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPolarity)
{
    uint16_t tmpccer = 0;

    tmpccer = TIMx->CCER;
    tmpccer &= (uint16_t) ~((uint16_t)TIM_CC4P);
    tmpccer |= (uint16_t)(TIM_OCPolarity << 12);
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @fn      TIM_CCxCmd
 *
 * @brief   Enables or disables the TIM Capture Compare Channel x.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_Channel - specifies the TIM Channel.
 *            TIM_Channel_1 - TIM Channel 1.
 *            TIM_Channel_2 - TIM Channel 2.
 *            TIM_Channel_3 - TIM Channel 3.
 *            TIM_Channel_4 - TIM Channel 4.
 *          TIM_CCx - specifies the TIM Channel CCxE bit new state.
 *            TIM_CCx_Enable.
 *            TIM_CCx_Disable.
 *
 * @return  none
 */
void TIM_CCxCmd(TIM_TypeDef *TIMx, uint16_t TIM_Channel, uint16_t TIM_CCx)
{
    uint16_t tmp = 0;

    tmp = CCER_CCE_Set << TIM_Channel;
    TIMx->CCER &= (uint16_t)~tmp;
    TIMx->CCER |= (uint16_t)(TIM_CCx << TIM_Channel);
}

/*********************************************************************
 * @fn      TIM_CCxNCmd
 *
 * @brief   Enables or disables the TIM Capture Compare Channel xN.
 *
 * @param   TIMx - where x can be 1 select the TIM peripheral.
 *          TIM_Channel - specifies the TIM Channel.
 *            TIM_Channel_1 - TIM Channel 1.
 *            TIM_Channel_2 - TIM Channel 2.
 *            TIM_Channel_3 - TIM Channel 3.
 *          TIM_CCxN - specifies the TIM Channel CCxNE bit new state.
 *            TIM_CCxN_Enable.
 *            TIM_CCxN_Disable.
 *
 * @return  none
 */
void TIM_CCxNCmd(TIM_TypeDef *TIMx, uint16_t TIM_Channel, uint16_t TIM_CCxN)
{
    uint16_t tmp = 0;

    tmp = CCER_CCNE_Set << TIM_Channel;
    TIMx->CCER &= (uint16_t)~tmp;
    TIMx->CCER |= (uint16_t)(TIM_CCxN << TIM_Channel);
}

/*********************************************************************
 * @fn      TIM_SelectOCxM
 *
 * @brief   Selects the TIM Output Compare Mode.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_Channel - specifies the TIM Channel.
 *            TIM_Channel_1 - TIM Channel 1.
 *            TIM_Channel_2 - TIM Channel 2.
 *            TIM_Channel_3 - TIM Channel 3.
 *            TIM_Channel_4 - TIM Channel 4.
 *          TIM_OCMode - specifies the TIM Output Compare Mode.
 *            TIM_OCMode_Timing.
 *            TIM_OCMode_Active.
 *            TIM_OCMode_Toggle.
 *            TIM_OCMode_PWM1.
 *            TIM_OCMode_PWM2.
 *            TIM_ForcedAction_Active.
 *            TIM_ForcedAction_InActive.
 *
 * @return  none
 */
void TIM_SelectOCxM(TIM_TypeDef *TIMx, uint16_t TIM_Channel, uint16_t TIM_OCMode)
{
    uint32_t tmp = 0;
    uint16_t tmp1 = 0;

    tmp = (uint32_t)TIMx;
    tmp += CHCTLR_Offset;
    tmp1 = CCER_CCE_Set << (uint16_t)TIM_Channel;
    TIMx->CCER &= (uint16_t)~tmp1;

    if((TIM_Channel == TIM_Channel_1) || (TIM_Channel == TIM_Channel_3))
    {
        tmp += (TIM_Channel >> 1);
        *(__IO uint32_t *)tmp &= (uint32_t) ~((uint32_t)TIM_OC1M);
        *(__IO uint32_t *)tmp |= TIM_OCMode;
    }
    else
    {
        tmp += (uint16_t)(TIM_Channel - (uint16_t)4) >> (uint16_t)1;
        *(__IO uint32_t *)tmp &= (uint32_t) ~((uint32_t)TIM_OC2M);
        *(__IO uint32_t *)tmp |= (uint16_t)(TIM_OCMode << 8);
    }
}

/*********************************************************************
 * @fn      TIM_UpdateDisableConfig
 *
 * @brief   Enables or Disables the TIMx Update event.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void TIM_UpdateDisableConfig(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->CTLR1 |= TIM_UDIS;
    }
    else
    {
        TIMx->CTLR1 &= (uint16_t) ~((uint16_t)TIM_UDIS);
    }
}

/*********************************************************************
 * @fn      TIM_UpdateRequestConfig
 *
 * @brief   Configures the TIMx Update Request Interrupt source.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_UpdateSource - specifies the Update source.
 *            TIM_UpdateSource_Regular.
 *            TIM_UpdateSource_Global.
 *
 * @return  none
 */
void TIM_UpdateRequestConfig(TIM_TypeDef *TIMx, uint16_t TIM_UpdateSource)
{
    if(TIM_UpdateSource != TIM_UpdateSource_Global)
    {
        TIMx->CTLR1 |= TIM_URS;
    }
    else
    {
        TIMx->CTLR1 &= (uint16_t) ~((uint16_t)TIM_URS);
    }
}

/*********************************************************************
 * @fn      TIM_SelectHallSensor
 *
 * @brief   Enables or disables the TIMx's Hall sensor interface.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void TIM_SelectHallSensor(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->CTLR2 |= TIM_TI1S;
    }
    else
    {
        TIMx->CTLR2 &= (uint16_t) ~((uint16_t)TIM_TI1S);
    }
}

/*********************************************************************
 * @fn      TIM_SelectOnePulseMode
 *
 * @brief   Selects the TIMx's One Pulse Mode.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_OPMode - specifies the OPM Mode to be used.
 *            TIM_OPMode_Single.
 *            TIM_OPMode_Repetitive.
 *
 * @return  none
 */
void TIM_SelectOnePulseMode(TIM_TypeDef *TIMx, uint16_t TIM_OPMode)
{
    TIMx->CTLR1 &= (uint16_t) ~((uint16_t)TIM_OPM);
    TIMx->CTLR1 |= TIM_OPMode;
}

/*********************************************************************
 * @fn      TIM_SelectOutputTrigger
 *
 * @brief   Selects the TIMx Trigger Output Mode.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_TRGOSource - specifies the Trigger Output source.
 *            TIM_TRGOSource_Reset -  The UG bit in the TIM_EGR register is
 *        used as the trigger output (TRGO).
 *            TIM_TRGOSource_Enable - The Counter Enable CEN is used as the
 *        trigger output (TRGO).
 *            TIM_TRGOSource_Update - The update event is selected as the
 *        trigger output (TRGO).
 *            TIM_TRGOSource_OC1 - The trigger output sends a positive pulse
 *        when the CC1IF flag is to be set, as soon as a capture or compare match occurs (TRGO).
 *            TIM_TRGOSource_OC1Ref - OC1REF signal is used as the trigger output (TRGO).
 *            TIM_TRGOSource_OC2Ref - OC2REF signal is used as the trigger output (TRGO).
 *            TIM_TRGOSource_OC3Ref - OC3REF signal is used as the trigger output (TRGO).
 *            TIM_TRGOSource_OC4Ref - OC4REF signal is used as the trigger output (TRGO).
 *
 * @return  none
 */
void TIM_SelectOutputTrigger(TIM_TypeDef *TIMx, uint16_t TIM_TRGOSource)
{
    TIMx->CTLR2 &= (uint16_t) ~((uint16_t)TIM_MMS);
    TIMx->CTLR2 |= TIM_TRGOSource;
}

/*********************************************************************
 * @fn      TIM_SelectSlaveMode
 *
 * @brief   Selects the TIMx Slave Mode.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_SlaveMode - specifies the Timer Slave Mode.
 *            TIM_SlaveMode_Reset - Rising edge of the selected trigger
 *        signal (TRGI) re-initializes.
 *            TIM_SlaveMode_Gated - The counter clock is enabled when the
 *        trigger signal (TRGI) is high.
 *            TIM_SlaveMode_Trigger - The counter starts at a rising edge
 *        of the trigger TRGI.
 *            TIM_SlaveMode_External1 - Rising edges of the selected trigger
 *        (TRGI) clock the counter.
 *
 * @return  none
 */
void TIM_SelectSlaveMode(TIM_TypeDef *TIMx, uint16_t TIM_SlaveMode)
{
    TIMx->SMCFGR &= (uint16_t) ~((uint16_t)TIM_SMS);
    TIMx->SMCFGR |= TIM_SlaveMode;
}

/*********************************************************************
 * @fn      TIM_SelectMasterSlaveMode
 *
 * @brief   Sets or Resets the TIMx Master/Slave Mode.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_MasterSlaveMode - specifies the Timer Master Slave Mode.
 *            TIM_MasterSlaveMode_Enable - synchronization between the current
 *        timer and its slaves (through TRGO).
 *            TIM_MasterSlaveMode_Disable - No action.
 *
 * @return  none
 */
void TIM_SelectMasterSlaveMode(TIM_TypeDef *TIMx, uint16_t TIM_MasterSlaveMode)
{
    TIMx->SMCFGR &= (uint16_t) ~((uint16_t)TIM_MSM);
    TIMx->SMCFGR |= TIM_MasterSlaveMode;
}

/*********************************************************************
 * @fn      TIM_SetCounter
 *
 * @brief   Sets the TIMx Counter Register value.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          Counter - specifies the Counter register new value.
 *
 * @return  none
 */
void TIM_SetCounter(TIM_TypeDef *TIMx, uint16_t Counter)
{
    TIMx->CNT = Counter;
}

/*********************************************************************
 * @fn      TIM_SetAutoreload
 *
 * @brief   Sets the TIMx Autoreload Register value.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          Autoreload - specifies the Autoreload register new value.
 *
 * @return  none
 */
void TIM_SetAutoreload(TIM_TypeDef *TIMx, uint16_t Autoreload)
{
    TIMx->ATRLR = Autoreload;
}

/*********************************************************************
 * @fn      TIM_SetCompare1
 *
 * @brief   Sets the TIMx Capture Compare1 Register value.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          Compare1 - specifies the Capture Compare1 register new value.
 *
 * @return  none
 */
void TIM_SetCompare1(TIM_TypeDef *TIMx, uint16_t Compare1)
{
    TIMx->CH1CVR = Compare1;
}

/*********************************************************************
 * @fn      TIM_SetCompare2
 *
 * @brief   Sets the TIMx Capture Compare2 Register value.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          Compare1 - specifies the Capture Compare1 register new value.
 *
 * @return  none
 */
void TIM_SetCompare2(TIM_TypeDef *TIMx, uint16_t Compare2)
{
    TIMx->CH2CVR = Compare2;
}

/*********************************************************************
 * @fn      TIM_SetCompare3
 *
 * @brief   Sets the TIMx Capture Compare3 Register value.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          Compare1 - specifies the Capture Compare1 register new value.
 *
 * @return  none
 */
void TIM_SetCompare3(TIM_TypeDef *TIMx, uint16_t Compare3)
{
    TIMx->CH3CVR = Compare3;
}

/*********************************************************************
 * @fn      TIM_SetCompare4
 *
 * @brief   Sets the TIMx Capture Compare4 Register value.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          Compare1 - specifies the Capture Compare1 register new value.
 *
 * @return  none
 */
void TIM_SetCompare4(TIM_TypeDef *TIMx, uint16_t Compare4)
{
    TIMx->CH4CVR = Compare4;
}

/*********************************************************************
 * @fn      TIM_SetIC1Prescaler
 *
 * @brief   Sets the TIMx Input Capture 1 prescaler.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_ICPSC - specifies the Input Capture1 prescaler new value.
 *            TIM_ICPSC_DIV1 - no prescaler.
 *            TIM_ICPSC_DIV2 - capture is done once every 2 events.
 *            TIM_ICPSC_DIV4 - capture is done once every 4 events.
 *            TIM_ICPSC_DIV8 - capture is done once every 8 events.
 *
 * @return  none
 */
void TIM_SetIC1Prescaler(TIM_TypeDef *TIMx, uint16_t TIM_ICPSC)
{
    TIMx->CHCTLR1 &= (uint16_t) ~((uint16_t)TIM_IC1PSC);
    TIMx->CHCTLR1 |= TIM_ICPSC;
}

/*********************************************************************
 * @fn      TIM_SetIC2Prescaler
 *
 * @brief   Sets the TIMx Input Capture 2 prescaler.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_ICPSC - specifies the Input Capture1 prescaler new value.
 *            TIM_ICPSC_DIV1 - no prescaler.
 *            TIM_ICPSC_DIV2 - capture is done once every 2 events.
 *            TIM_ICPSC_DIV4 - capture is done once every 4 events.
 *            TIM_ICPSC_DIV8 - capture is done once every 8 events.
 *
 * @return  none
 */
void TIM_SetIC2Prescaler(TIM_TypeDef *TIMx, uint16_t TIM_ICPSC)
{
    TIMx->CHCTLR1 &= (uint16_t) ~((uint16_t)TIM_IC2PSC);
    TIMx->CHCTLR1 |= (uint16_t)(TIM_ICPSC << 8);
}

/*********************************************************************
 * @fn      TIM_SetIC3Prescaler
 *
 * @brief   Sets the TIMx Input Capture 3 prescaler.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_ICPSC - specifies the Input Capture1 prescaler new value.
 *            TIM_ICPSC_DIV1 - no prescaler.
 *            TIM_ICPSC_DIV2 - capture is done once every 2 events.
 *            TIM_ICPSC_DIV4 - capture is done once every 4 events.
 *            TIM_ICPSC_DIV8 - capture is done once every 8 events.
 *
 * @return  none
 */
void TIM_SetIC3Prescaler(TIM_TypeDef *TIMx, uint16_t TIM_ICPSC)
{
    TIMx->CHCTLR2 &= (uint16_t) ~((uint16_t)TIM_IC3PSC);
    TIMx->CHCTLR2 |= TIM_ICPSC;
}

/*********************************************************************
 * @fn      TIM_SetIC4Prescaler
 *
 * @brief   Sets the TIMx Input Capture 4 prescaler.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_ICPSC - specifies the Input Capture1 prescaler new value.
 *            TIM_ICPSC_DIV1 - no prescaler.
 *            TIM_ICPSC_DIV2 - capture is done once every 2 events.
 *            TIM_ICPSC_DIV4 - capture is done once every 4 events.
 *            TIM_ICPSC_DIV8 - capture is done once every 8 events.
 *
 * @return  none
 */
void TIM_SetIC4Prescaler(TIM_TypeDef *TIMx, uint16_t TIM_ICPSC)
{
    TIMx->CHCTLR2 &= (uint16_t) ~((uint16_t)TIM_IC4PSC);
    TIMx->CHCTLR2 |= (uint16_t)(TIM_ICPSC << 8);
}

/*********************************************************************
 * @fn      TIM_SetClockDivision
 *
 * @brief   Sets the TIMx Clock Division value.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_CKD - specifies the clock division value.
 *            TIM_CKD_DIV1 - TDTS = Tck_tim.
 *            TIM_CKD_DIV2 - TDTS = 2*Tck_tim.
 *            TIM_CKD_DIV4 - TDTS = 4*Tck_tim.
 *
 * @return  none
 */
void TIM_SetClockDivision(TIM_TypeDef *TIMx, uint16_t TIM_CKD)
{
    TIMx->CTLR1 &= (uint16_t) ~((uint16_t)TIM_CTLR1_CKD);
    TIMx->CTLR1 |= TIM_CKD;
}

/*********************************************************************
 * @fn      TIM_GetCapture1
 *
 * @brief   Gets the TIMx Input Capture 1 value.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *
 * @return  TIMx->CH1CVR - Capture Compare 1 Register value.
 */
uint16_t TIM_GetCapture1(TIM_TypeDef *TIMx)
{
    return TIMx->CH1CVR;
}

/*********************************************************************
 * @fn      TIM_GetCapture2
 *
 * @brief   Gets the TIMx Input Capture 2 value.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *
 * @return  TIMx->CH2CVR - Capture Compare 2 Register value.
 */
uint16_t TIM_GetCapture2(TIM_TypeDef *TIMx)
{
    return TIMx->CH2CVR;
}

/*********************************************************************
 * @fn      TIM_GetCapture3
 *
 * @brief   Gets the TIMx Input Capture 3 value.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *
 * @return  TIMx->CH3CVR - Capture Compare 3 Register value.
 */
uint16_t TIM_GetCapture3(TIM_TypeDef *TIMx)
{
    return TIMx->CH3CVR;
}

/*********************************************************************
 * @fn      TIM_GetCapture4
 *
 * @brief   Gets the TIMx Input Capture 4 value.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *
 * @return  TIMx->CH4CVR - Capture Compare 4 Register value.
 */
uint16_t TIM_GetCapture4(TIM_TypeDef *TIMx)
{
    return TIMx->CH4CVR;
}

/*********************************************************************
 * @fn      TIM_GetCounter
 *
 * @brief   Gets the TIMx Counter value.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *
 * @return  TIMx->CNT - Counter Register value.
 */
uint16_t TIM_GetCounter(TIM_TypeDef *TIMx)
{
    return TIMx->CNT;
}

/*********************************************************************
 * @fn      TIM_GetPrescaler
 *
 * @brief   Gets the TIMx Prescaler value.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *
 * @return  TIMx->PSC - Prescaler Register value.
 */
uint16_t TIM_GetPrescaler(TIM_TypeDef *TIMx)
{
    return TIMx->PSC;
}

/*********************************************************************
 * @fn      TIM_GetFlagStatus
 *
 * @brief   Checks whether the specified TIM flag is set or not.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_FLAG - specifies the flag to check.
 *            TIM_FLAG_Update - TIM update Flag.
 *            TIM_FLAG_CC1 - TIM Capture Compare 1 Flag.
 *            TIM_FLAG_CC2 - TIM Capture Compare 2 Flag.
 *            TIM_FLAG_CC3 - TIM Capture Compare 3 Flag.
 *            TIM_FLAG_CC4 - TIM Capture Compare 4 Flag.
 *            TIM_FLAG_COM - TIM Commutation Flag.
 *            TIM_FLAG_Trigger - TIM Trigger Flag.
 *            TIM_FLAG_Break - TIM Break Flag.
 *            TIM_FLAG_CC1OF - TIM Capture Compare 1 overcapture Flag.
 *            TIM_FLAG_CC2OF - TIM Capture Compare 2 overcapture Flag.
 *            TIM_FLAG_CC3OF - TIM Capture Compare 3 overcapture Flag.
 *            TIM_FLAG_CC4OF - TIM Capture Compare 4 overcapture Flag.
 *
 * @return  none
 */
FlagStatus TIM_GetFlagStatus(TIM_TypeDef *TIMx, uint16_t TIM_FLAG)
{
    ITStatus bitstatus = RESET;

    if((TIMx->INTFR & TIM_FLAG) != (uint16_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      TIM_ClearFlag
 *
 * @brief   Clears the TIMx's pending flags.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_FLAG - specifies the flag to check.
 *            TIM_FLAG_Update - TIM update Flag.
 *            TIM_FLAG_CC1 - TIM Capture Compare 1 Flag.
 *            TIM_FLAG_CC2 - TIM Capture Compare 2 Flag.
 *            TIM_FLAG_CC3 - TIM Capture Compare 3 Flag.
 *            TIM_FLAG_CC4 - TIM Capture Compare 4 Flag.
 *            TIM_FLAG_COM - TIM Commutation Flag.
 *            TIM_FLAG_Trigger - TIM Trigger Flag.
 *            TIM_FLAG_Break - TIM Break Flag.
 *            TIM_FLAG_CC1OF - TIM Capture Compare 1 overcapture Flag.
 *            TIM_FLAG_CC2OF - TIM Capture Compare 2 overcapture Flag.
 *            TIM_FLAG_CC3OF - TIM Capture Compare 3 overcapture Flag.
 *            TIM_FLAG_CC4OF - TIM Capture Compare 4 overcapture Flag.
 *
 * @return  none
 */
void TIM_ClearFlag(TIM_TypeDef *TIMx, uint16_t TIM_FLAG)
{
    TIMx->INTFR = (uint16_t)~TIM_FLAG;
}

/*********************************************************************
 * @fn      TIM_GetITStatus
 *
 * @brief   Checks whether the TIM interrupt has occurred or not.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_IT - specifies the TIM interrupt source to check.
 *            TIM_IT_Update - TIM update Interrupt source.
 *            TIM_IT_CC1 - TIM Capture Compare 1 Interrupt source.
 *            TIM_IT_CC2 - TIM Capture Compare 2 Interrupt source.
 *            TIM_IT_CC3 - TIM Capture Compare 3 Interrupt source.
 *            TIM_IT_CC4 - TIM Capture Compare 4 Interrupt source.
 *            TIM_IT_COM - TIM Commutation Interrupt source.
 *            TIM_IT_Trigger - TIM Trigger Interrupt source.
 *            TIM_IT_Break - TIM Break Interrupt source.
 *
 * @return  none
 */
ITStatus TIM_GetITStatus(TIM_TypeDef *TIMx, uint16_t TIM_IT)
{
    ITStatus bitstatus = RESET;
    uint16_t itstatus = 0x0, itenable = 0x0;

    itstatus = TIMx->INTFR & TIM_IT;

    itenable = TIMx->DMAINTENR & TIM_IT;
    if((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      TIM_ClearITPendingBit
 *
 * @brief   Clears the TIMx's interrupt pending bits.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          TIM_IT - specifies the TIM interrupt source to check.
 *            TIM_IT_Update - TIM update Interrupt source.
 *            TIM_IT_CC1 - TIM Capture Compare 1 Interrupt source.
 *            TIM_IT_CC2 - TIM Capture Compare 2 Interrupt source.
 *            TIM_IT_CC3 - TIM Capture Compare 3 Interrupt source.
 *            TIM_IT_CC4 - TIM Capture Compare 4 Interrupt source.
 *            TIM_IT_COM - TIM Commutation Interrupt source.
 *            TIM_IT_Trigger - TIM Trigger Interrupt source.
 *            TIM_IT_Break - TIM Break Interrupt source.
 *
 * @return  none
 */
void TIM_ClearITPendingBit(TIM_TypeDef *TIMx, uint16_t TIM_IT)
{
    TIMx->INTFR = (uint16_t)~TIM_IT;
}

/*********************************************************************
 * @fn      TI1_Config
 *
 * @brief   Configure the TI1 as Input.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          IM_ICPolarity - The Input Polarity.
 *             TIM_ICPolarity_Rising.
 *             TIM_ICPolarity_Falling.
 *          TIM_ICSelection - specifies the input to be used.
 *             TIM_ICSelection_DirectTI - TIM Input 1 is selected to be
 *        connected to IC1.
 *             TIM_ICSelection_IndirectTI - TIM Input 1 is selected to be
 *        connected to IC2.
 *             TIM_ICSelection_TRC - TIM Input 1 is selected to be connected
 *        to TRC.
 *          TIM_ICFilter - Specifies the Input Capture Filter.
 *            This parameter must be a value between 0x00 and 0x0F.
 *
 * @return  none
 */
static void TI1_Config(TIM_TypeDef *TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
    uint16_t tmpccmr1 = 0, tmpccer = 0;

    TIMx->CCER &= (uint16_t) ~((uint16_t)TIM_CC1E);
    tmpccmr1 = TIMx->CHCTLR1;
    tmpccer = TIMx->CCER;
    tmpccmr1 &= (uint16_t)(((uint16_t) ~((uint16_t)TIM_CC1S)) & ((uint16_t) ~((uint16_t)TIM_IC1F)));
    tmpccmr1 |= (uint16_t)(TIM_ICSelection | (uint16_t)(TIM_ICFilter << (uint16_t)4));

    if((TIMx == TIM1) || (TIMx == TIM2) || (TIMx == TIM3) || (TIMx == TIM4) ||
       (TIMx == TIM5) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10))
    {
        tmpccer &= (uint16_t) ~((uint16_t)(TIM_CC1P));
        tmpccer |= (uint16_t)(TIM_ICPolarity | (uint16_t)TIM_CC1E);
    }
    else
    {
        tmpccer &= (uint16_t) ~((uint16_t)(TIM_CC1P | TIM_CC1NP));
        tmpccer |= (uint16_t)(TIM_ICPolarity | (uint16_t)TIM_CC1E);
    }

    TIMx->CHCTLR1 = tmpccmr1;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @fn      TI2_Config
 *
 * @brief   Configure the TI2 as Input.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          IM_ICPolarity - The Input Polarity.
 *             TIM_ICPolarity_Rising.
 *             TIM_ICPolarity_Falling.
 *          TIM_ICSelection - specifies the input to be used.
 *             TIM_ICSelection_DirectTI - TIM Input 1 is selected to be
 *        connected to IC1.
 *             TIM_ICSelection_IndirectTI - TIM Input 1 is selected to be
 *        connected to IC2.
 *             TIM_ICSelection_TRC - TIM Input 1 is selected to be connected
 *        to TRC.
 *          TIM_ICFilter - Specifies the Input Capture Filter.
 *            This parameter must be a value between 0x00 and 0x0F.
 *
 * @return  none
 */
static void TI2_Config(TIM_TypeDef *TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
    uint16_t tmpccmr1 = 0, tmpccer = 0, tmp = 0;

    TIMx->CCER &= (uint16_t) ~((uint16_t)TIM_CC2E);
    tmpccmr1 = TIMx->CHCTLR1;
    tmpccer = TIMx->CCER;
    tmp = (uint16_t)(TIM_ICPolarity << 4);
    tmpccmr1 &= (uint16_t)(((uint16_t) ~((uint16_t)TIM_CC2S)) & ((uint16_t) ~((uint16_t)TIM_IC2F)));
    tmpccmr1 |= (uint16_t)(TIM_ICFilter << 12);
    tmpccmr1 |= (uint16_t)(TIM_ICSelection << 8);

    if((TIMx == TIM1) || (TIMx == TIM2) || (TIMx == TIM3) || (TIMx == TIM4) ||
       (TIMx == TIM5) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10))
    {
        tmpccer &= (uint16_t) ~((uint16_t)(TIM_CC2P));
        tmpccer |= (uint16_t)(tmp | (uint16_t)TIM_CC2E);
    }
    else
    {
        tmpccer &= (uint16_t) ~((uint16_t)(TIM_CC2P | TIM_CC2NP));
        tmpccer |= (uint16_t)(TIM_ICPolarity | (uint16_t)TIM_CC2E);
    }

    TIMx->CHCTLR1 = tmpccmr1;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @fn      TI3_Config
 *
 * @brief   Configure the TI3 as Input.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          IM_ICPolarity - The Input Polarity.
 *             TIM_ICPolarity_Rising.
 *             TIM_ICPolarity_Falling.
 *          TIM_ICSelection - specifies the input to be used.
 *             TIM_ICSelection_DirectTI - TIM Input 1 is selected to be
 *        connected to IC1.
 *             TIM_ICSelection_IndirectTI - TIM Input 1 is selected to be
 *        connected to IC2.
 *             TIM_ICSelection_TRC - TIM Input 1 is selected to be connected
 *        to TRC.
 *          TIM_ICFilter - Specifies the Input Capture Filter.
 *            This parameter must be a value between 0x00 and 0x0F.
 *
 * @return  none
 */
static void TI3_Config(TIM_TypeDef *TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
    uint16_t tmpccmr2 = 0, tmpccer = 0, tmp = 0;

    TIMx->CCER &= (uint16_t) ~((uint16_t)TIM_CC3E);
    tmpccmr2 = TIMx->CHCTLR2;
    tmpccer = TIMx->CCER;
    tmp = (uint16_t)(TIM_ICPolarity << 8);
    tmpccmr2 &= (uint16_t)(((uint16_t) ~((uint16_t)TIM_CC3S)) & ((uint16_t) ~((uint16_t)TIM_IC3F)));
    tmpccmr2 |= (uint16_t)(TIM_ICSelection | (uint16_t)(TIM_ICFilter << (uint16_t)4));

    if((TIMx == TIM1) || (TIMx == TIM2) || (TIMx == TIM3) || (TIMx == TIM4) ||
       (TIMx == TIM5) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10))
    {
        tmpccer &= (uint16_t) ~((uint16_t)(TIM_CC3P));
        tmpccer |= (uint16_t)(tmp | (uint16_t)TIM_CC3E);
    }
    else
    {
        tmpccer &= (uint16_t) ~((uint16_t)(TIM_CC3P | TIM_CC3NP));
        tmpccer |= (uint16_t)(TIM_ICPolarity | (uint16_t)TIM_CC3E);
    }

    TIMx->CHCTLR2 = tmpccmr2;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @fn      TI4_Config
 *
 * @brief   Configure the TI4 as Input.
 *
 * @param   TIMx - where x can be 1 to 4 select the TIM peripheral.
 *          IM_ICPolarity - The Input Polarity.
 *             TIM_ICPolarity_Rising.
 *             TIM_ICPolarity_Falling.
 *          TIM_ICSelection - specifies the input to be used.
 *             TIM_ICSelection_DirectTI - TIM Input 1 is selected to be
 *        connected to IC1.
 *             TIM_ICSelection_IndirectTI - TIM Input 1 is selected to be
 *        connected to IC2.
 *             TIM_ICSelection_TRC - TIM Input 1 is selected to be connected
 *        to TRC.
 *          TIM_ICFilter - Specifies the Input Capture Filter.
 *            This parameter must be a value between 0x00 and 0x0F.
 *
 * @return  none
 */
static void TI4_Config(TIM_TypeDef *TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
    uint16_t tmpccmr2 = 0, tmpccer = 0, tmp = 0;

    TIMx->CCER &= (uint16_t) ~((uint16_t)TIM_CC4E);
    tmpccmr2 = TIMx->CHCTLR2;
    tmpccer = TIMx->CCER;
    tmp = (uint16_t)(TIM_ICPolarity << 12);
    tmpccmr2 &= (uint16_t)((uint16_t)(~(uint16_t)TIM_CC4S) & ((uint16_t) ~((uint16_t)TIM_IC4F)));
    tmpccmr2 |= (uint16_t)(TIM_ICSelection << 8);
    tmpccmr2 |= (uint16_t)(TIM_ICFilter << 12);

    if((TIMx == TIM1) || (TIMx == TIM2) || (TIMx == TIM3) || (TIMx == TIM4) ||
       (TIMx == TIM5) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10))
    {
        tmpccer &= (uint16_t) ~((uint16_t)(TIM_CC4P));
        tmpccer |= (uint16_t)(tmp | (uint16_t)TIM_CC4E);
    }
    else
    {
        tmpccer &= (uint16_t) ~((uint16_t)(TIM_CC3P | TIM_CC4NP));
        tmpccer |= (uint16_t)(TIM_ICPolarity | (uint16_t)TIM_CC4E);
    }

    TIMx->CHCTLR2 = tmpccmr2;
    TIMx->CCER = tmpccer;
}
