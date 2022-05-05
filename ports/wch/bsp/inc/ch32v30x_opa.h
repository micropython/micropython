/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_opa.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for the 
*                      OPA firmware library.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/ 
#ifndef __CH32V30x_OPA_H
#define __CH32V30x_OPA_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

#define OPA_PSEL_OFFSET 3
#define OPA_NSEL_OFFSET 2
#define OPA_MODE_OFFSET 1


/* OPA member enumeration */
typedef enum
{
   OPA1=0,
   OPA2,
   OPA3,
   OPA4
}OPA_Num_TypeDef;

/* OPA PSEL enumeration */
typedef enum
{
   CHP0=0,
   CHP1
}OPA_PSEL_TypeDef;

/* OPA NSEL enumeration */
typedef enum
{
   CHN0=0,
   CHN1
}OPA_NSEL_TypeDef;

/* OPA Mode enumeration */
typedef enum
{
   OUT_IO_ADC=0,
   OUT_IO
}OPA_Mode_TypeDef;

/* OPA Init Structure definition */
typedef struct
{
  OPA_Num_TypeDef   OPA_NUM;      /* Specifies the members of OPA */
  OPA_PSEL_TypeDef  PSEL;         /* Specifies the positive channel of OPA */
  OPA_NSEL_TypeDef  NSEL;         /* Specifies the negative channel of OPA */
  OPA_Mode_TypeDef  Mode;         /* Specifies the mode of OPA */
}OPA_InitTypeDef;


void OPA_DeInit(void);
void OPA_Init(OPA_InitTypeDef* OPA_InitStruct);
void OPA_StructInit(OPA_InitTypeDef* OPA_InitStruct);
void OPA_Cmd(OPA_Num_TypeDef OPA_NUM, FunctionalState NewState);

#ifdef __cplusplus
}
#endif

#endif 

