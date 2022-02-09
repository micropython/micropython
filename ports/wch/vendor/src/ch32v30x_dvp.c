/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_dvp.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the DVP firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "ch32v30x_dvp.h"

/*********************************************************************
 * @fn      DVP_INTCfg
 *
 * @brief   DVP interrupt configuration
 *
 * @param   s - interrupt enable
 *             ENABLE
 *             DISABLE
 *          i - interrupt type
 *             RB_DVP_IE_STP_FRM
 *             RB_DVP_IE_FIFO_OV
 *             RB_DVP_IE_FRM_DONE
 *             RB_DVP_IE_ROW_DONE
 *             RB_DVP_IE_STR_FRM
 *
 * @return  none
 */
void DVP_INTCfg(uint8_t s, uint8_t i)
{
    if(s)
    {
        DVP->IER |= i;
    }
    else
    {
        DVP->IER &= ~i;
    }
}

/*********************************************************************
 * @fn      DVP_Mode
 *
 * @brief   DVP mode
 *
 * @param   s - data bit width
 *             RB_DVP_D8_MOD
 *             RB_DVP_D10_MOD
 *             RB_DVP_D12_MOD
 *          i - interrupt type
 *             Video_Mode
 *             JPEG_Mode
 *
 * @return  none
 */
void DVP_Mode(uint8_t s, DVP_Data_ModeTypeDef i)
{
    DVP->CR0 &= ~RB_DVP_MSK_DAT_MOD;

    if(s)
    {
        DVP->CR0 |= s;
    }
    else
    {
        DVP->CR0 &= ~(3 << 4);
    }

    if(i)
    {
        DVP->CR0 |= RB_DVP_JPEG;
    }
    else
    {
        DVP->CR0 &= ~RB_DVP_JPEG;
    }
}

/*********************************************************************
 * @fn      DVP_Cfg
 *
 * @brief   DVP configuration
 *
 * @param   s - DMA enable control
 *            DVP_DMA_Enable
 *            DVP_DMA_Disable
 *          i - DVP all clear
 *            DVP_FLAG_FIFO_RESET_Enable
 *            DVP_FLAG_FIFO_RESET_Disable
 *          j - receive reset enable
 *            DVP_RX_RESET_Enable
 *            DVP_RX_RESET_Disable
 *
 * @return  none
 */
void DVP_Cfg(DVP_DMATypeDef s, DVP_FLAG_FIFO_RESETTypeDef i, DVP_RX_RESETTypeDef j)
{
    switch(s)
    {
        case DVP_DMA_Enable:
            DVP->CR1 |= RB_DVP_DMA_EN;
            break;
        case DVP_DMA_Disable:
            DVP->CR1 &= ~RB_DVP_DMA_EN;
            break;
        default:
            break;
    }

    switch(i)
    {
        case DVP_RX_RESET_Enable:
            DVP->CR1 |= RB_DVP_ALL_CLR;
            break;
        case DVP_RX_RESET_Disable:
            DVP->CR1 &= ~RB_DVP_ALL_CLR;
            break;
        default:
            break;
    }

    switch(j)
    {
        case DVP_RX_RESET_Enable:
            DVP->CR1 |= RB_DVP_RCV_CLR;
            break;
        case DVP_RX_RESET_Disable:
            DVP->CR1 &= ~RB_DVP_RCV_CLR;
            break;
        default:
            break;
    }
}
