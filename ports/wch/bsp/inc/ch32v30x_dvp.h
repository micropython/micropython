/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_dvp.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for the 
*                      DVP firmware library.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/ 
#ifndef __CH32V30x_DVP_H
#define __CH32V30x_DVP_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* DVP Data Mode */
typedef enum
{
 Video_Mode = 0,
 JPEG_Mode,
}DVP_Data_ModeTypeDef;


/* DVP DMA */
typedef enum
{
 DVP_DMA_Disable = 0,
 DVP_DMA_Enable,
}DVP_DMATypeDef;

/* DVP FLAG and FIFO Reset */
typedef enum
{
 DVP_FLAG_FIFO_RESET_Disable = 0,
 DVP_FLAG_FIFO_RESET_Enable,
}DVP_FLAG_FIFO_RESETTypeDef;

/* DVP RX Reset */
typedef enum
{
 DVP_RX_RESET_Disable = 0,
 DVP_RX_RESET_Enable,
}DVP_RX_RESETTypeDef;



void DVP_INTCfg( uint8_t s,  uint8_t i );
void DVP_Mode( uint8_t s,  DVP_Data_ModeTypeDef i);
void DVP_Cfg( DVP_DMATypeDef s,  DVP_FLAG_FIFO_RESETTypeDef i, DVP_RX_RESETTypeDef j);



#ifdef __cplusplus
}
#endif

#endif 






