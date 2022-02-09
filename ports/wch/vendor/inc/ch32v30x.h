/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : CH32V30x Device Peripheral Access Layer Header File.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/   
#ifndef __CH32V30x_H
#define __CH32V30x_H

#ifdef __cplusplus
 extern "C" {
#endif 

//#define CH32V30x_D8              /* CH32V303 */
#define CH32V30x_D8C             /* CH32V307-CH32V305 */
  
#define __MPU_PRESENT             0 /* Other CH32 devices does not provide an MPU */
#define __Vendor_SysTickConfig    0 /* Set to 1 if different SysTick Config is used */	 
	 
#define HSE_VALUE    ((uint32_t)8000000) /* Value of the External oscillator in Hz */

/* In the following line adjust the External High Speed oscillator (HSE) Startup Timeout value */
#define HSE_STARTUP_TIMEOUT   ((uint16_t)0x1000) /* Time out for HSE start up */

#define HSI_VALUE    ((uint32_t)8000000) /* Value of the Internal oscillator in Hz */

/* Interrupt Number Definition, according to the selected device */	 
typedef enum IRQn
{
 /******  RISC-V Processor Exceptions Numbers *******************************************************/
  NonMaskableInt_IRQn         = 2,       /* 2 Non Maskable Interrupt                             */
  EXC_IRQn                    = 3,       /* 3 Exception Interrupt                                */
  Ecall_M_Mode_IRQn           = 5,       /* 5 Ecall M Mode Interrupt                             */
  Ecall_U_Mode_IRQn           = 8,       /* 8 Ecall U Mode Interrupt                             */
  Break_Point_IRQn            = 9,       /* 9 Break Point Interrupt                              */
  SysTicK_IRQn                = 12,      /* 12 System timer Interrupt                            */
  Software_IRQn               = 14,      /* 14 software Interrupt                                */

 /******  RISC-V specific Interrupt Numbers *********************************************************/
  WWDG_IRQn                   = 16,      /* Window WatchDog Interrupt                            */
  PVD_IRQn                    = 17,      /* PVD through EXTI Line detection Interrupt            */
  TAMPER_IRQn                 = 18,      /* Tamper Interrupt                                     */
  RTC_IRQn                    = 19,      /* RTC global Interrupt                                 */
  FLASH_IRQn                  = 20,      /* FLASH global Interrupt                               */
  RCC_IRQn                    = 21,      /* RCC global Interrupt                                 */
  EXTI0_IRQn                  = 22,      /* EXTI Line0 Interrupt                                 */
  EXTI1_IRQn                  = 23,      /* EXTI Line1 Interrupt                                 */
  EXTI2_IRQn                  = 24,      /* EXTI Line2 Interrupt                                 */
  EXTI3_IRQn                  = 25,      /* EXTI Line3 Interrupt                                 */
  EXTI4_IRQn                  = 26,      /* EXTI Line4 Interrupt                                 */
  DMA1_Channel1_IRQn          = 27,      /* DMA1 Channel 1 global Interrupt                      */
  DMA1_Channel2_IRQn          = 28,      /* DMA1 Channel 2 global Interrupt                      */
  DMA1_Channel3_IRQn          = 29,      /* DMA1 Channel 3 global Interrupt                      */
  DMA1_Channel4_IRQn          = 30,      /* DMA1 Channel 4 global Interrupt                      */
  DMA1_Channel5_IRQn          = 31,      /* DMA1 Channel 5 global Interrupt                      */
  DMA1_Channel6_IRQn          = 32,      /* DMA1 Channel 6 global Interrupt                      */
  DMA1_Channel7_IRQn          = 33,      /* DMA1 Channel 7 global Interrupt                      */
  ADC_IRQn                    = 34,      /* ADC1 and ADC2 global Interrupt                       */
  USB_HP_CAN1_TX_IRQn         = 35,      /* USB Device High Priority or CAN1 TX Interrupts       */
  USB_LP_CAN1_RX0_IRQn        = 36,      /* USB Device Low Priority or CAN1 RX0 Interrupts       */
  CAN1_RX1_IRQn               = 37,      /* CAN1 RX1 Interrupt                                   */
  CAN1_SCE_IRQn               = 38,      /* CAN1 SCE Interrupt                                   */
  EXTI9_5_IRQn                = 39,      /* External Line[9:5] Interrupts                        */
  TIM1_BRK_IRQn               = 40,      /* TIM1 Break Interrupt                                 */
  TIM1_UP_IRQn                = 41,      /* TIM1 Update Interrupt                                */
  TIM1_TRG_COM_IRQn           = 42,      /* TIM1 Trigger and Commutation Interrupt               */
  TIM1_CC_IRQn                = 43,      /* TIM1 Capture Compare Interrupt                       */
  TIM2_IRQn                   = 44,      /* TIM2 global Interrupt                                */
  TIM3_IRQn                   = 45,      /* TIM3 global Interrupt                                */
  TIM4_IRQn                   = 46,      /* TIM4 global Interrupt                                */
  I2C1_EV_IRQn                = 47,      /* I2C1 Event Interrupt                                 */
  I2C1_ER_IRQn                = 48,      /* I2C1 Error Interrupt                                 */
  I2C2_EV_IRQn                = 49,      /* I2C2 Event Interrupt                                 */
  I2C2_ER_IRQn                = 50,      /* I2C2 Error Interrupt                                 */
  SPI1_IRQn                   = 51,      /* SPI1 global Interrupt                                */
  SPI2_IRQn                   = 52,      /* SPI2 global Interrupt                                */
  USART1_IRQn                 = 53,      /* USART1 global Interrupt                              */
  USART2_IRQn                 = 54,      /* USART2 global Interrupt                              */
  USART3_IRQn                 = 55,      /* USART3 global Interrupt                              */
  EXTI15_10_IRQn              = 56,      /* External Line[15:10] Interrupts                      */
  RTCAlarm_IRQn               = 57,      /* RTC Alarm through EXTI Line Interrupt                */

#ifdef CH32V30x_D8
  TIM8_BRK_IRQn               = 59,      /* TIM8 Break Interrupt                                 */
  TIM8_UP_IRQn                = 60,      /* TIM8 Update Interrupt                                */
  TIM8_TRG_COM_IRQn           = 61,      /* TIM8 Trigger and Commutation Interrupt               */
  TIM8_CC_IRQn                = 62,      /* TIM8 Capture Compare Interrupt                       */
  RNG_IRQn                    = 63,      /* RNG global Interrupt                                 */
  FSMC_IRQn                   = 64,      /* FSMC global Interrupt                                */
  SDIO_IRQn                   = 65,      /* SDIO global Interrupt                                */
  TIM5_IRQn                   = 66,      /* TIM5 global Interrupt                                */
  SPI3_IRQn                   = 67,      /* SPI3 global Interrupt                                */
  UART4_IRQn                  = 68,      /* UART4 global Interrupt                               */
  UART5_IRQn                  = 69,      /* UART5 global Interrupt                               */
  TIM6_IRQn                   = 70,      /* TIM6 global Interrupt                                */
  TIM7_IRQn                   = 71,      /* TIM7 global Interrupt                                */
  DMA2_Channel1_IRQn          = 72,      /* DMA2 Channel 1 global Interrupt                      */
  DMA2_Channel2_IRQn          = 73,      /* DMA2 Channel 2 global Interrupt                      */
  DMA2_Channel3_IRQn          = 74,      /* DMA2 Channel 3 global Interrupt                      */
  DMA2_Channel4_IRQn          = 75,      /* DMA2 Channel 4 global Interrupt                      */
  DMA2_Channel5_IRQn          = 76,      /* DMA2 Channel 5 global Interrupt                      */
  OTG_FS_IRQn                 = 83,      /* OTGFS global Interrupt                               */
  UART6_IRQn                  = 87,      /* UART6 global Interrupt                               */
  UART7_IRQn                  = 88,      /* UART7 global Interrupt                               */
  UART8_IRQn                  = 89,      /* UART8 global Interrupt                               */
  TIM9_BRK_IRQn               = 90,      /* TIM9 Break Interrupt                                 */
  TIM9_UP_IRQn                = 91,      /* TIM9 Update Interrupt                                */
  TIM9_TRG_COM_IRQn           = 92,      /* TIM9 Trigger and Commutation Interrupt               */
  TIM9_CC_IRQn                = 93,      /* TIM9 Capture Compare Interrupt                       */
  TIM10_BRK_IRQn              = 94,      /* TIM10 Break Interrupt                                */
  TIM10_UP_IRQn               = 95,      /* TIM10 Update Interrupt                               */
  TIM10_TRG_COM_IRQn          = 96,      /* TIM10 Trigger and Commutation Interrupt              */
  TIM10_CC_IRQn               = 97,      /* TIM10 Capture Compare Interrupt                      */
  DMA2_Channel6_IRQn          = 98,      /* DMA2 Channel 6 global Interrupt                      */
  DMA2_Channel7_IRQn          = 99,      /* DMA2 Channel 7 global Interrupt                      */
  DMA2_Channel8_IRQn          = 100,     /* DMA2 Channel 8 global Interrupt                      */
  DMA2_Channel9_IRQn          = 101,     /* DMA2 Channel 9 global Interrupt                      */
  DMA2_Channel10_IRQn         = 102,     /* DMA2 Channel 10 global Interrupt                     */
  DMA2_Channel11_IRQn         = 103,     /* DMA2 Channel 11 global Interrupt                     */

#endif

#ifdef CH32V30x_D8C
  TIM8_BRK_IRQn               = 59,      /* TIM8 Break Interrupt                                 */
  TIM8_UP_IRQn                = 60,      /* TIM8 Update Interrupt                                */
  TIM8_TRG_COM_IRQn           = 61,      /* TIM8 Trigger and Commutation Interrupt               */
  TIM8_CC_IRQn                = 62,      /* TIM8 Capture Compare Interrupt                       */
  RNG_IRQn                    = 63,      /* RNG global Interrupt                                 */
  FSMC_IRQn                   = 64,      /* FSMC global Interrupt                                */
  SDIO_IRQn                   = 65,      /* SDIO global Interrupt                                */
  TIM5_IRQn                   = 66,      /* TIM5 global Interrupt                                */
  SPI3_IRQn                   = 67,      /* SPI3 global Interrupt                                */
  UART4_IRQn                  = 68,      /* UART4 global Interrupt                               */
  UART5_IRQn                  = 69,      /* UART5 global Interrupt                               */
  TIM6_IRQn                   = 70,      /* TIM6 global Interrupt                                */
  TIM7_IRQn                   = 71,      /* TIM7 global Interrupt                                */
  DMA2_Channel1_IRQn          = 72,      /* DMA2 Channel 1 global Interrupt                      */
  DMA2_Channel2_IRQn          = 73,      /* DMA2 Channel 2 global Interrupt                      */
  DMA2_Channel3_IRQn          = 74,      /* DMA2 Channel 3 global Interrupt                      */
  DMA2_Channel4_IRQn          = 75,      /* DMA2 Channel 4 global Interrupt                      */
  DMA2_Channel5_IRQn          = 76,      /* DMA2 Channel 5 global Interrupt                      */
  ETH_IRQn                    = 77,      /* ETH global Interrupt                                 */
  ETH_WKUP_IRQn               = 78,      /* ETH WakeUp Interrupt                                 */
  CAN2_TX_IRQn                = 79,      /* CAN2 TX Interrupts                                   */
  CAN2_RX0_IRQn               = 80,      /* CAN2 RX0 Interrupts                                  */
  CAN2_RX1_IRQn               = 81,      /* CAN2 RX1 Interrupt                                   */
  CAN2_SCE_IRQn               = 82,      /* CAN2 SCE Interrupt                                   */
  OTG_FS_IRQn                 = 83,      /* OTGFS global Interrupt                               */
  USBHSWakeup_IRQn            = 84,      /* USBHS WakeUp Interrupt                               */
  USBHS_IRQn                  = 85,      /* USBHS global Interrupt                               */
  DVP_IRQn                    = 86,      /* DVP global Interrupt                                 */
  UART6_IRQn                  = 87,      /* UART6 global Interrupt                               */
  UART7_IRQn                  = 88,      /* UART7 global Interrupt                               */
  UART8_IRQn                  = 89,      /* UART8 global Interrupt                               */
  TIM9_BRK_IRQn               = 90,      /* TIM9 Break Interrupt                                 */
  TIM9_UP_IRQn                = 91,      /* TIM9 Update Interrupt                                */
  TIM9_TRG_COM_IRQn           = 92,      /* TIM9 Trigger and Commutation Interrupt               */
  TIM9_CC_IRQn                = 93,      /* TIM9 Capture Compare Interrupt                       */
  TIM10_BRK_IRQn              = 94,      /* TIM10 Break Interrupt                                */
  TIM10_UP_IRQn               = 95,      /* TIM10 Update Interrupt                               */
  TIM10_TRG_COM_IRQn          = 96,      /* TIM10 Trigger and Commutation Interrupt              */
  TIM10_CC_IRQn               = 97,      /* TIM10 Capture Compare Interrupt                      */
  DMA2_Channel6_IRQn          = 98,      /* DMA2 Channel 6 global Interrupt                      */
  DMA2_Channel7_IRQn          = 99,      /* DMA2 Channel 7 global Interrupt                      */
  DMA2_Channel8_IRQn          = 100,     /* DMA2 Channel 8 global Interrupt                      */
  DMA2_Channel9_IRQn          = 101,     /* DMA2 Channel 9 global Interrupt                      */
  DMA2_Channel10_IRQn         = 102,     /* DMA2 Channel 10 global Interrupt                     */
  DMA2_Channel11_IRQn         = 103,     /* DMA2 Channel 11 global Interrupt                     */

#endif
} IRQn_Type;

#define HardFault_IRQn   EXC_IRQn
#define ADC1_2_IRQn      ADC_IRQn


#include <stdint.h>
#include "core_riscv.h"
#include "system_ch32v30x.h"


/* Standard Peripheral Library old definitions (maintained for legacy purpose) */
#define HSI_Value            HSI_VALUE 
#define HSE_Value            HSE_VALUE
#define HSEStartUp_TimeOut   HSE_STARTUP_TIMEOUT

/* Analog to Digital Converter */
typedef struct
{
  __IO uint32_t STATR;
  __IO uint32_t CTLR1;
  __IO uint32_t CTLR2;
  __IO uint32_t SAMPTR1;
  __IO uint32_t SAMPTR2;
  __IO uint32_t IOFR1;
  __IO uint32_t IOFR2;
  __IO uint32_t IOFR3;
  __IO uint32_t IOFR4;
  __IO uint32_t WDHTR;
  __IO uint32_t WDLTR;
  __IO uint32_t RSQR1;
  __IO uint32_t RSQR2;
  __IO uint32_t RSQR3;
  __IO uint32_t ISQR;
  __IO uint32_t IDATAR1;
  __IO uint32_t IDATAR2;
  __IO uint32_t IDATAR3;
  __IO uint32_t IDATAR4;
  __IO uint32_t RDATAR;
} ADC_TypeDef;

/* Backup Registers */
typedef struct
{
  uint32_t  RESERVED0;
  __IO uint16_t DATAR1;
  uint16_t  RESERVED1;
  __IO uint16_t DATAR2;
  uint16_t  RESERVED2;
  __IO uint16_t DATAR3;
  uint16_t  RESERVED3;
  __IO uint16_t DATAR4;
  uint16_t  RESERVED4;
  __IO uint16_t DATAR5;
  uint16_t  RESERVED5;
  __IO uint16_t DATAR6;
  uint16_t  RESERVED6;
  __IO uint16_t DATAR7;
  uint16_t  RESERVED7;
  __IO uint16_t DATAR8;
  uint16_t  RESERVED8;
  __IO uint16_t DATAR9;
  uint16_t  RESERVED9;
  __IO uint16_t DATAR10;
  uint16_t  RESERVED10; 
  __IO uint16_t OCTLR;
  uint16_t  RESERVED11;
  __IO uint16_t TPCTLR;
  uint16_t  RESERVED12;
  __IO uint16_t TPCSR;
  uint16_t  RESERVED13[5];
  __IO uint16_t DATAR11;
  uint16_t  RESERVED14;
  __IO uint16_t DATAR12;
  uint16_t  RESERVED15;
  __IO uint16_t DATAR13;
  uint16_t  RESERVED16;
  __IO uint16_t DATAR14;
  uint16_t  RESERVED17;
  __IO uint16_t DATAR15;
  uint16_t  RESERVED18;
  __IO uint16_t DATAR16;
  uint16_t  RESERVED19;
  __IO uint16_t DATAR17;
  uint16_t  RESERVED20;
  __IO uint16_t DATAR18;
  uint16_t  RESERVED21;
  __IO uint16_t DATAR19;
  uint16_t  RESERVED22;
  __IO uint16_t DATAR20;
  uint16_t  RESERVED23;
  __IO uint16_t DATAR21;
  uint16_t  RESERVED24;
  __IO uint16_t DATAR22;
  uint16_t  RESERVED25;
  __IO uint16_t DATAR23;
  uint16_t  RESERVED26;
  __IO uint16_t DATAR24;
  uint16_t  RESERVED27;
  __IO uint16_t DATAR25;
  uint16_t  RESERVED28;
  __IO uint16_t DATAR26;
  uint16_t  RESERVED29;
  __IO uint16_t DATAR27;
  uint16_t  RESERVED30;
  __IO uint16_t DATAR28;
  uint16_t  RESERVED31;
  __IO uint16_t DATAR29;
  uint16_t  RESERVED32;
  __IO uint16_t DATAR30;
  uint16_t  RESERVED33; 
  __IO uint16_t DATAR31;
  uint16_t  RESERVED34;
  __IO uint16_t DATAR32;
  uint16_t  RESERVED35;
  __IO uint16_t DATAR33;
  uint16_t  RESERVED36;
  __IO uint16_t DATAR34;
  uint16_t  RESERVED37;
  __IO uint16_t DATAR35;
  uint16_t  RESERVED38;
  __IO uint16_t DATAR36;
  uint16_t  RESERVED39;
  __IO uint16_t DATAR37;
  uint16_t  RESERVED40;
  __IO uint16_t DATAR38;
  uint16_t  RESERVED41;
  __IO uint16_t DATAR39;
  uint16_t  RESERVED42;
  __IO uint16_t DATAR40;
  uint16_t  RESERVED43;
  __IO uint16_t DATAR41;
  uint16_t  RESERVED44;
  __IO uint16_t DATAR42;
  uint16_t  RESERVED45;    
} BKP_TypeDef;

/* Controller Area Network TxMailBox */
typedef struct
{
  __IO uint32_t TXMIR;
  __IO uint32_t TXMDTR;
  __IO uint32_t TXMDLR;
  __IO uint32_t TXMDHR;
} CAN_TxMailBox_TypeDef;

/* Controller Area Network FIFOMailBox */ 
typedef struct
{
  __IO uint32_t RXMIR;
  __IO uint32_t RXMDTR;
  __IO uint32_t RXMDLR;
  __IO uint32_t RXMDHR;
} CAN_FIFOMailBox_TypeDef;

/* Controller Area Network FilterRegister */  
typedef struct
{
  __IO uint32_t FR1;
  __IO uint32_t FR2;
} CAN_FilterRegister_TypeDef;

/* Controller Area Network */  
typedef struct
{
  __IO uint32_t CTLR;
  __IO uint32_t STATR;
  __IO uint32_t TSTATR;
  __IO uint32_t RFIFO0;
  __IO uint32_t RFIFO1;
  __IO uint32_t INTENR;
  __IO uint32_t ERRSR;
  __IO uint32_t BTIMR;
  uint32_t  RESERVED0[88];
  CAN_TxMailBox_TypeDef sTxMailBox[3];
  CAN_FIFOMailBox_TypeDef sFIFOMailBox[2];
  uint32_t  RESERVED1[12];
  __IO uint32_t FCTLR;
  __IO uint32_t FMCFGR;
  uint32_t  RESERVED2;
  __IO uint32_t FSCFGR;
  uint32_t  RESERVED3;
  __IO uint32_t FAFIFOR;
  uint32_t  RESERVED4;
  __IO uint32_t FWR;
  uint32_t  RESERVED5[8];
  CAN_FilterRegister_TypeDef sFilterRegister[28];
} CAN_TypeDef;

/* CRC Calculation Unit */
typedef struct
{
  __IO uint32_t DATAR;
  __IO uint8_t  IDATAR;
  uint8_t   RESERVED0;
  uint16_t  RESERVED1;
  __IO uint32_t CTLR;
} CRC_TypeDef;

/* Digital to Analog Converter */
typedef struct
{
  __IO uint32_t CTLR;
  __IO uint32_t SWTR;
  __IO uint32_t R12BDHR1;
  __IO uint32_t L12BDHR1;
  __IO uint32_t R8BDHR1;
  __IO uint32_t R12BDHR2;
  __IO uint32_t L12BDHR2;
  __IO uint32_t R8BDHR2;
  __IO uint32_t RD12BDHR;
  __IO uint32_t LD12BDHR;
  __IO uint32_t RD8BDHR;
  __IO uint32_t DOR1;
  __IO uint32_t DOR2;
} DAC_TypeDef;

/* DMA Channel Controller */
typedef struct
{
  __IO uint32_t CFGR;
  __IO uint32_t CNTR;
  __IO uint32_t PADDR;
  __IO uint32_t MADDR;
} DMA_Channel_TypeDef;

/* DMA Controller */
typedef struct
{
  __IO uint32_t INTFR;
  __IO uint32_t INTFCR;
} DMA_TypeDef;

/* External Interrupt/Event Controller */
typedef struct
{
  __IO uint32_t INTENR; 
  __IO uint32_t EVENR;   
  __IO uint32_t RTENR;   
  __IO uint32_t FTENR;   
  __IO uint32_t SWIEVR;  
  __IO uint32_t INTFR;   
} EXTI_TypeDef;

/* FLASH Registers */
typedef struct
{
  __IO uint32_t ACTLR;
  __IO uint32_t KEYR;
  __IO uint32_t OBKEYR;
  __IO uint32_t STATR;
  __IO uint32_t CTLR;
  __IO uint32_t ADDR;
  __IO uint32_t RESERVED;
  __IO uint32_t OBR;
  __IO uint32_t WPR;
  __IO uint32_t MODEKEYR;
} FLASH_TypeDef;

/* Option Bytes Registers */  
typedef struct
{
  __IO uint16_t RDPR;
  __IO uint16_t USER;
  __IO uint16_t Data0;
  __IO uint16_t Data1;
  __IO uint16_t WRPR0;
  __IO uint16_t WRPR1;
  __IO uint16_t WRPR2;
  __IO uint16_t WRPR3;
} OB_TypeDef;

/* FSMC Bank1 Registers */ 
typedef struct
{
  __IO uint32_t BTCR[8];
} FSMC_Bank1_TypeDef;

/* FSMC Bank1E Registers */ 
typedef struct
{
  __IO uint32_t BWTR[7];
} FSMC_Bank1E_TypeDef;

/* FSMC Bank2 Registers */ 
typedef struct
{
  __IO uint32_t PCR2;
  __IO uint32_t SR2;
  __IO uint32_t PMEM2;
  __IO uint32_t PATT2;
  uint32_t  RESERVED0;
  __IO uint32_t ECCR2;
} FSMC_Bank2_TypeDef;

/* General Purpose I/O */
typedef struct
{
  __IO uint32_t CFGLR;
  __IO uint32_t CFGHR;
  __IO uint32_t INDR;
  __IO uint32_t OUTDR;
  __IO uint32_t BSHR;
  __IO uint32_t BCR;
  __IO uint32_t LCKR;
} GPIO_TypeDef;

/* Alternate Function I/O */
typedef struct
{
  __IO uint32_t ECR;
  __IO uint32_t PCFR1;
  __IO uint32_t EXTICR[4];
  uint32_t RESERVED0;
  __IO uint32_t PCFR2;  
} AFIO_TypeDef;

/* Inter Integrated Circuit Interface */
typedef struct
{
  __IO uint16_t CTLR1;
  uint16_t  RESERVED0;
  __IO uint16_t CTLR2;
  uint16_t  RESERVED1;
  __IO uint16_t OADDR1;
  uint16_t  RESERVED2;
  __IO uint16_t OADDR2;
  uint16_t  RESERVED3;
  __IO uint16_t DATAR;
  uint16_t  RESERVED4;
  __IO uint16_t STAR1;
  uint16_t  RESERVED5;
  __IO uint16_t STAR2;
  uint16_t  RESERVED6;
  __IO uint16_t CKCFGR;
  uint16_t  RESERVED7;
  __IO uint16_t RTR;
  uint16_t  RESERVED8;
} I2C_TypeDef;

/* Independent WatchDog */
typedef struct
{
  __IO uint32_t CTLR;
  __IO uint32_t PSCR;
  __IO uint32_t RLDR;
  __IO uint32_t STATR;
} IWDG_TypeDef;

/* Power Control */
typedef struct
{
  __IO uint32_t CTLR;
  __IO uint32_t CSR;
} PWR_TypeDef;

/* Reset and Clock Control */
typedef struct
{
  __IO uint32_t CTLR;
  __IO uint32_t CFGR0;
  __IO uint32_t INTR;
  __IO uint32_t APB2PRSTR;
  __IO uint32_t APB1PRSTR;
  __IO uint32_t AHBPCENR;
  __IO uint32_t APB2PCENR;
  __IO uint32_t APB1PCENR;
  __IO uint32_t BDCTLR;
  __IO uint32_t RSTSCKR;

  __IO uint32_t AHBRSTR;
  __IO uint32_t CFGR2;
} RCC_TypeDef;

/* Real-Time Clock */
typedef struct
{
  __IO uint16_t CTLRH;
  uint16_t  RESERVED0;
  __IO uint16_t CTLRL;
  uint16_t  RESERVED1;
  __IO uint16_t PSCRH;
  uint16_t  RESERVED2;
  __IO uint16_t PSCRL;
  uint16_t  RESERVED3;
  __IO uint16_t DIVH;
  uint16_t  RESERVED4;
  __IO uint16_t DIVL;
  uint16_t  RESERVED5;
  __IO uint16_t CNTH;
  uint16_t  RESERVED6;
  __IO uint16_t CNTL;
  uint16_t  RESERVED7;
  __IO uint16_t ALRMH;
  uint16_t  RESERVED8;
  __IO uint16_t ALRML;
  uint16_t  RESERVED9;
} RTC_TypeDef;

/* SDIO Registers */ 
typedef struct
{
  __IO uint32_t POWER;
  __IO uint32_t CLKCR;
  __IO uint32_t ARG;
  __IO uint32_t CMD;
  __I uint32_t RESPCMD;
  __I uint32_t RESP1;
  __I uint32_t RESP2;
  __I uint32_t RESP3;
  __I uint32_t RESP4;
  __IO uint32_t DTIMER;
  __IO uint32_t DLEN;
  __IO uint32_t DCTRL;
  __I uint32_t DCOUNT;
  __I uint32_t STA;
  __IO uint32_t ICR;
  __IO uint32_t MASK;
  uint32_t  RESERVED0[2];
  __I uint32_t FIFOCNT;
  uint32_t  RESERVED1[13];
  __IO uint32_t FIFO;
} SDIO_TypeDef;

/* Serial Peripheral Interface */
typedef struct
{
  __IO uint16_t CTLR1;
  uint16_t  RESERVED0;
  __IO uint16_t CTLR2;
  uint16_t  RESERVED1;
  __IO uint16_t STATR;
  uint16_t  RESERVED2;
  __IO uint16_t DATAR;
  uint16_t  RESERVED3;
  __IO uint16_t CRCR;
  uint16_t  RESERVED4;
  __IO uint16_t RCRCR;
  uint16_t  RESERVED5;
  __IO uint16_t TCRCR;
  uint16_t  RESERVED6;
  __IO uint16_t I2SCFGR;
  uint16_t  RESERVED7;
  __IO uint16_t I2SPR;
  uint16_t  RESERVED8;  
} SPI_TypeDef;

/* TIM */
typedef struct
{
  __IO uint16_t CTLR1;
  uint16_t  RESERVED0;
  __IO uint16_t CTLR2;
  uint16_t  RESERVED1;
  __IO uint16_t SMCFGR;
  uint16_t  RESERVED2;
  __IO uint16_t DMAINTENR;
  uint16_t  RESERVED3;
  __IO uint16_t INTFR;
  uint16_t  RESERVED4;
  __IO uint16_t SWEVGR;
  uint16_t  RESERVED5;
  __IO uint16_t CHCTLR1;
  uint16_t  RESERVED6;
  __IO uint16_t CHCTLR2;
  uint16_t  RESERVED7;
  __IO uint16_t CCER;
  uint16_t  RESERVED8;
  __IO uint16_t CNT;
  uint16_t  RESERVED9;
  __IO uint16_t PSC;
  uint16_t  RESERVED10;
  __IO uint16_t ATRLR;
  uint16_t  RESERVED11;
  __IO uint16_t RPTCR;
  uint16_t  RESERVED12;
  __IO uint16_t CH1CVR;
  uint16_t  RESERVED13;
  __IO uint16_t CH2CVR;
  uint16_t  RESERVED14;
  __IO uint16_t CH3CVR;
  uint16_t  RESERVED15;
  __IO uint16_t CH4CVR;
  uint16_t  RESERVED16;
  __IO uint16_t BDTR;
  uint16_t  RESERVED17;
  __IO uint16_t DMACFGR;
  uint16_t  RESERVED18;
  __IO uint16_t DMAADR;
  uint16_t  RESERVED19;
} TIM_TypeDef;

/* Universal Synchronous Asynchronous Receiver Transmitter */
typedef struct
{
  __IO uint16_t STATR;
  uint16_t  RESERVED0;
  __IO uint16_t DATAR;
  uint16_t  RESERVED1;
  __IO uint16_t BRR;
  uint16_t  RESERVED2;
  __IO uint16_t CTLR1;
  uint16_t  RESERVED3;
  __IO uint16_t CTLR2;
  uint16_t  RESERVED4;
  __IO uint16_t CTLR3;
  uint16_t  RESERVED5;
  __IO uint16_t GPR;
  uint16_t  RESERVED6;
} USART_TypeDef;

/* Window WatchDog */
typedef struct
{
  __IO uint32_t CTLR;
  __IO uint32_t CFGR;
  __IO uint32_t STATR;
} WWDG_TypeDef;

/* Enhanced Registers */
typedef struct
{
  __IO uint32_t EXTEN_CTR;
} EXTEN_TypeDef;

/* OPA Registers */
typedef struct
{
  __IO uint32_t CR;
} OPA_TypeDef;

/* RNG Registers */
typedef struct
{
  __IO uint32_t CR;
  __IO uint32_t SR;
  __IO uint32_t DR;
} RNG_TypeDef;

/* DVP Registers */
typedef struct
{
  __IO uint8_t CR0;
  __IO uint8_t CR1;
  __IO uint8_t IER;
  __IO uint8_t Reserved0;        
  __IO uint16_t ROW_NUM;
  __IO uint16_t COL_NUM;
  __IO uint32_t DMA_BUF0;
  __IO uint32_t DMA_BUF1;
  __IO uint8_t IFR;
  __IO uint8_t STATUS;
  __IO uint16_t Reserved1;            
  __IO uint16_t ROW_CNT;
  __IO uint16_t Reserved2;           
  __IO uint16_t HOFFCNT;
  __IO uint16_t VST;
  __IO uint16_t CAPCNT;
  __IO uint16_t VLINE;
  __IO uint32_t DR;
} DVP_TypeDef;

/* USBHS Registers */
typedef struct
{
  __IO uint8_t  CONTROL;
  __IO uint8_t  HOST_CTRL;
  __IO uint8_t  INT_EN;
  __IO uint8_t  DEV_AD;
  __IO uint16_t FRAME_NO;
  __IO uint8_t  SUSPEND;
  __IO uint8_t  RESERVED0;
  __IO uint8_t  SPEED_TYPE;
  __IO uint8_t  MIS_ST;
  __IO uint8_t  INT_FG;
  __IO uint8_t  INT_ST;
  __IO uint16_t RX_LEN;
  __IO uint16_t RESERVED1;
  __IO uint32_t ENDP_CONFIG;
  __IO uint32_t ENDP_TYPE;
  __IO uint32_t BUF_MODE;
  __IO uint32_t UEP0_DMA;               
  __IO uint32_t UEP1_RX_DMA;       
  __IO uint32_t UEP2_RX_DMA;       
  __IO uint32_t UEP3_RX_DMA;       
  __IO uint32_t UEP4_RX_DMA;       
  __IO uint32_t UEP5_RX_DMA;       
  __IO uint32_t UEP6_RX_DMA;       
  __IO uint32_t UEP7_RX_DMA;       
  __IO uint32_t UEP8_RX_DMA;      
  __IO uint32_t UEP9_RX_DMA;       
  __IO uint32_t UEP10_RX_DMA;      
  __IO uint32_t UEP11_RX_DMA;      
  __IO uint32_t UEP12_RX_DMA;      
  __IO uint32_t UEP13_RX_DMA;      
  __IO uint32_t UEP14_RX_DMA;      
  __IO uint32_t UEP15_RX_DMA;      
  __IO uint32_t UEP1_TX_DMA;       
  __IO uint32_t UEP2_TX_DMA;       
  __IO uint32_t UEP3_TX_DMA;       
  __IO uint32_t UEP4_TX_DMA;       
  __IO uint32_t UEP5_TX_DMA;      
  __IO uint32_t UEP6_TX_DMA;      
  __IO uint32_t UEP7_TX_DMA;       
  __IO uint32_t UEP8_TX_DMA;      
  __IO uint32_t UEP9_TX_DMA;       
  __IO uint32_t UEP10_TX_DMA;      
  __IO uint32_t UEP11_TX_DMA;      
  __IO uint32_t UEP12_TX_DMA;    
  __IO uint32_t UEP13_TX_DMA;      
  __IO uint32_t UEP14_TX_DMA;     
  __IO uint32_t UEP15_TX_DMA;      
  __IO uint16_t UEP0_MAX_LEN;
  __IO uint16_t RESERVED2;
  __IO uint16_t UEP1_MAX_LEN;
  __IO uint16_t RESERVED3;
  __IO uint16_t UEP2_MAX_LEN;
  __IO uint16_t RESERVED4;
  __IO uint16_t UEP3_MAX_LEN;
  __IO uint16_t RESERVED5;
  __IO uint16_t UEP4_MAX_LEN;
  __IO uint16_t RESERVED6;
  __IO uint16_t UEP5_MAX_LEN;
  __IO uint16_t RESERVED7;
  __IO uint16_t UEP6_MAX_LEN;
  __IO uint16_t RESERVED8;
  __IO uint16_t UEP7_MAX_LEN;
  __IO uint16_t RESERVED9;
  __IO uint16_t UEP8_MAX_LEN;
  __IO uint16_t RESERVED10;
  __IO uint16_t UEP9_MAX_LEN;
  __IO uint16_t RESERVED11;
  __IO uint16_t UEP10_MAX_LEN;
  __IO uint16_t RESERVED12;
  __IO uint16_t UEP11_MAX_LEN;
  __IO uint16_t RESERVED13;
  __IO uint16_t UEP12_MAX_LEN;
  __IO uint16_t RESERVED14;
  __IO uint16_t UEP13_MAX_LEN;
  __IO uint16_t RESERVED15;
  __IO uint16_t UEP14_MAX_LEN;
  __IO uint16_t RESERVED16;
  __IO uint16_t UEP15_MAX_LEN;
  __IO uint16_t RESERVED17;
  __IO uint16_t UEP0_TX_LEN;
  __IO uint8_t  UEP0_TX_CTRL;
  __IO uint8_t  UEP0_RX_CTRL;
  __IO uint16_t UEP1_TX_LEN;
  __IO uint8_t  UEP1_TX_CTRL;
  __IO uint8_t  UEP1_RX_CTRL;
  __IO uint16_t UEP2_TX_LEN;
  __IO uint8_t  UEP2_TX_CTRL;
  __IO uint8_t  UEP2_RX_CTRL;
  __IO uint16_t UEP3_TX_LEN;
  __IO uint8_t  UEP3_TX_CTRL;
  __IO uint8_t  UEP3_RX_CTRL;
  __IO uint16_t UEP4_TX_LEN;
  __IO uint8_t  UEP4_TX_CTRL;
  __IO uint8_t  UEP4_RX_CTRL;
  __IO uint16_t UEP5_TX_LEN;
  __IO uint8_t  UEP5_TX_CTRL;
  __IO uint8_t  UEP5_RX_CTRL;
  __IO uint16_t UEP6_TX_LEN;
  __IO uint8_t  UEP6_TX_CTRL;
  __IO uint8_t  UEP6_RX_CTRL;
  __IO uint16_t UEP7_TX_LEN;
  __IO uint8_t  UEP7_TX_CTRL;
  __IO uint8_t  UEP7_RX_CTRL;
  __IO uint16_t UEP8_TX_LEN;
  __IO uint8_t  UEP8_TX_CTRL;
  __IO uint8_t  UEP8_RX_CTRL;
  __IO uint16_t UEP9_TX_LEN;
  __IO uint8_t  UEP9_TX_CTRL;
  __IO uint8_t  UEP9_RX_CTRL;
  __IO uint16_t UEP10_TX_LEN;
  __IO uint8_t  UEP10_TX_CTRL;
  __IO uint8_t  UEP10_RX_CTRL;
  __IO uint16_t UEP11_TX_LEN;
  __IO uint8_t  UEP11_TX_CTRL;
  __IO uint8_t  UEP11_RX_CTRL;
  __IO uint16_t UEP12_TX_LEN;
  __IO uint8_t  UEP12_TX_CTRL;
  __IO uint8_t  UEP12_RX_CTRL;
  __IO uint16_t UEP13_TX_LEN;
  __IO uint8_t  UEP13_TX_CTRL;
  __IO uint8_t  UEP13_RX_CTRL;
  __IO uint16_t UEP14_TX_LEN;
  __IO uint8_t  UEP14_TX_CTRL;
  __IO uint8_t  UEP14_RX_CTRL;
  __IO uint16_t UEP15_TX_LEN;
  __IO uint8_t  UEP15_TX_CTRL;
  __IO uint8_t  UEP15_RX_CTRL;
} USBHSD_TypeDef;

typedef struct  __attribute__((packed))
{
    __IO uint8_t  CONTROL;
    __IO uint8_t  HOST_CTRL;
    __IO uint8_t  INT_EN;
    __IO uint8_t  DEV_AD;
    __IO uint16_t FRAME_NO;
    __IO uint8_t  SUSPEND;
    __IO uint8_t  RESERVED0;
    __IO uint8_t  SPEED_TYPE;
    __IO uint8_t  MIS_ST;
    __IO uint8_t  INT_FG;
    __IO uint8_t  INT_ST;
    __IO uint16_t RX_LEN;
    __IO uint16_t RESERVED1;
    __IO uint32_t HOST_EP_CONFIG;
    __IO uint32_t HOST_EP_TYPE;
    __IO uint32_t RESERVED2;
    __IO uint32_t RESERVED3;
    __IO uint32_t RESERVED4;
    __IO uint32_t HOST_RX_DMA;
    __IO uint32_t RESERVED5;
    __IO uint32_t RESERVED6;
    __IO uint32_t RESERVED7;
    __IO uint32_t RESERVED8;
    __IO uint32_t RESERVED9;
    __IO uint32_t RESERVED10;
    __IO uint32_t RESERVED11;
    __IO uint32_t RESERVED12;
    __IO uint32_t RESERVED13;
    __IO uint32_t RESERVED14;
    __IO uint32_t RESERVED15;
    __IO uint32_t RESERVED16;
    __IO uint32_t RESERVED17;
    __IO uint32_t RESERVED18;
    __IO uint32_t RESERVED19;
    __IO uint32_t HOST_TX_DMA;
    __IO uint32_t RESERVED20;
    __IO uint32_t RESERVED21;
    __IO uint32_t RESERVED22;
    __IO uint32_t RESERVED23;
    __IO uint32_t RESERVED24;
    __IO uint32_t RESERVED25;
    __IO uint32_t RESERVED26;
    __IO uint32_t RESERVED27;
    __IO uint32_t RESERVED28;
    __IO uint32_t RESERVED29;
    __IO uint32_t RESERVED30;
    __IO uint32_t RESERVED31;
    __IO uint32_t RESERVED32;
    __IO uint32_t RESERVED33;
    __IO uint16_t HOST_RX_MAX_LEN;
    __IO uint16_t RESERVED34;
    __IO uint32_t RESERVED35;
    __IO uint32_t RESERVED36;
    __IO uint32_t RESERVED37;
    __IO uint32_t RESERVED38;
    __IO uint32_t RESERVED39;
    __IO uint32_t RESERVED40;
    __IO uint32_t RESERVED41;
    __IO uint32_t RESERVED42;
    __IO uint32_t RESERVED43;
    __IO uint32_t RESERVED44;
    __IO uint32_t RESERVED45;
    __IO uint32_t RESERVED46;
    __IO uint32_t RESERVED47;
    __IO uint32_t RESERVED48;
    __IO uint32_t RESERVED49;
    __IO uint8_t  HOST_EP_PID;
    __IO uint8_t  RESERVED50;
    __IO uint8_t  RESERVED51;
    __IO uint8_t  HOST_RX_CTRL;
    __IO uint16_t HOST_TX_LEN;
    __IO uint8_t  HOST_TX_CTRL;
    __IO uint8_t  RESERVED52;
    __IO uint16_t HOST_SPLIT_DATA;
} USBHSH_TypeDef;


/* USBOTG_FS Registers */
typedef struct
{
   __IO uint8_t  BASE_CTRL;
   __IO uint8_t  UDEV_CTRL;
   __IO uint8_t  INT_EN;
   __IO uint8_t  DEV_ADDR;
   __IO uint8_t  Reserve0;
   __IO uint8_t  MIS_ST;
   __IO uint8_t  INT_FG;
   __IO uint8_t  INT_ST;
   __IO uint16_t RX_LEN;
   __IO uint16_t Reserve1;
   __IO uint8_t  UEP4_1_MOD;
   __IO uint8_t  UEP2_3_MOD;
   __IO uint8_t  UEP5_6_MOD;
   __IO uint8_t  UEP7_MOD;
   __IO uint32_t UEP0_DMA;
   __IO uint32_t UEP1_DMA;
   __IO uint32_t UEP2_DMA;
   __IO uint32_t UEP3_DMA;
   __IO uint32_t UEP4_DMA;
   __IO uint32_t UEP5_DMA;
   __IO uint32_t UEP6_DMA;
   __IO uint32_t UEP7_DMA;
   __IO uint16_t UEP0_TX_LEN;
   __IO uint8_t  UEP0_TX_CTRL;
   __IO uint8_t  UEP0_RX_CTRL;
   __IO uint16_t UEP1_TX_LEN;
   __IO uint8_t  UEP1_TX_CTRL;
   __IO uint8_t  UEP1_RX_CTRL;
   __IO uint16_t UEP2_TX_LEN;
   __IO uint8_t  UEP2_TX_CTRL;
   __IO uint8_t  UEP2_RX_CTRL;
   __IO uint16_t UEP3_TX_LEN;
   __IO uint8_t  UEP3_TX_CTRL;
   __IO uint8_t  UEP3_RX_CTRL;
   __IO uint16_t UEP4_TX_LEN;
   __IO uint8_t  UEP4_TX_CTRL;
   __IO uint8_t  UEP4_RX_CTRL;
   __IO uint16_t UEP5_TX_LEN;
   __IO uint8_t  UEP5_TX_CTRL;
   __IO uint8_t  UEP5_RX_CTRL;
   __IO uint16_t UEP6_TX_LEN;
   __IO uint8_t  UEP6_TX_CTRL;
   __IO uint8_t  UEP6_RX_CTRL;
   __IO uint16_t UEP7_TX_LEN;
   __IO uint8_t  UEP7_TX_CTRL;
   __IO uint8_t  UEP7_RX_CTRL;
   __IO uint32_t Reserve2;
   __IO uint32_t OTG_CR;
   __IO uint32_t OTG_SR;
}USBOTG_FS_TypeDef;

typedef struct  __attribute__((packed))
{
   __IO uint8_t   BASE_CTRL;
   __IO uint8_t   HOST_CTRL;
   __IO uint8_t   INT_EN;
   __IO uint8_t   DEV_ADDR;
   __IO uint8_t   Reserve0;
   __IO uint8_t   MIS_ST;
   __IO uint8_t   INT_FG;
   __IO uint8_t   INT_ST;
   __IO uint16_t  RX_LEN;
   __IO uint16_t  Reserve1;
   __IO uint8_t   Reserve2;
   __IO uint8_t   HOST_EP_MOD;
   __IO uint16_t  Reserve3;
   __IO uint32_t  Reserve4;
   __IO uint32_t  Reserve5;
   __IO uint32_t  HOST_RX_DMA;
   __IO uint32_t  HOST_TX_DMA;
   __IO uint32_t  Reserve6;
   __IO uint32_t  Reserve7;
   __IO uint32_t  Reserve8;
   __IO uint32_t  Reserve9;
   __IO uint32_t  Reserve10;
   __IO uint16_t  Reserve11;
   __IO uint16_t  HOST_SETUP;
   __IO uint8_t   HOST_EP_PID;
   __IO uint8_t   Reserve12;
   __IO uint8_t   Reserve13;
   __IO uint8_t   HOST_RX_CTRL;
   __IO uint16_t  HOST_TX_LEN;
   __IO uint8_t   HOST_TX_CTRL;
   __IO uint8_t   Reserve14;
   __IO uint32_t  Reserve15;
   __IO uint32_t  Reserve16;
   __IO uint32_t  Reserve17;
   __IO uint32_t  Reserve18;
   __IO uint32_t  Reserve19;
   __IO uint32_t  OTG_CR;
   __IO uint32_t  OTG_SR;
}USBOTGH_FS_TypeDef;

/* Ethernet MAC */
typedef struct
{
  __IO uint32_t MACCR;
  __IO uint32_t MACFFR;
  __IO uint32_t MACHTHR;
  __IO uint32_t MACHTLR;
  __IO uint32_t MACMIIAR;
  __IO uint32_t MACMIIDR;
  __IO uint32_t MACFCR;
  __IO uint32_t MACVLANTR;
       uint32_t RESERVED0[2];
  __IO uint32_t MACRWUFFR;
  __IO uint32_t MACPMTCSR;
       uint32_t RESERVED1[2];
  __IO uint32_t MACSR;
  __IO uint32_t MACIMR;
  __IO uint32_t MACA0HR;
  __IO uint32_t MACA0LR;
  __IO uint32_t MACA1HR;
  __IO uint32_t MACA1LR;
  __IO uint32_t MACA2HR;
  __IO uint32_t MACA2LR;
  __IO uint32_t MACA3HR;
  __IO uint32_t MACA3LR;
       uint32_t RESERVED2[40];
  __IO uint32_t MMCCR;
  __IO uint32_t MMCRIR;
  __IO uint32_t MMCTIR;
  __IO uint32_t MMCRIMR;
  __IO uint32_t MMCTIMR;
       uint32_t RESERVED3[14];
  __IO uint32_t MMCTGFSCCR;
  __IO uint32_t MMCTGFMSCCR;
       uint32_t RESERVED4[5];
  __IO uint32_t MMCTGFCR;
       uint32_t RESERVED5[10];
  __IO uint32_t MMCRFCECR;
  __IO uint32_t MMCRFAECR;
       uint32_t RESERVED6[10];
  __IO uint32_t MMCRGUFCR;
       uint32_t RESERVED7[334];
  __IO uint32_t PTPTSCR;
  __IO uint32_t PTPSSIR;
  __IO uint32_t PTPTSHR;
  __IO uint32_t PTPTSLR;
  __IO uint32_t PTPTSHUR;
  __IO uint32_t PTPTSLUR;
  __IO uint32_t PTPTSAR;
  __IO uint32_t PTPTTHR;
  __IO uint32_t PTPTTLR;
       uint32_t RESERVED8[567];
  __IO uint32_t DMABMR;
  __IO uint32_t DMATPDR;
  __IO uint32_t DMARPDR;
  __IO uint32_t DMARDLAR;
  __IO uint32_t DMATDLAR;
  __IO uint32_t DMASR;
  __IO uint32_t DMAOMR;
  __IO uint32_t DMAIER;
  __IO uint32_t DMAMFBOCR;
       uint32_t RESERVED9[9];
  __IO uint32_t DMACHTDR;
  __IO uint32_t DMACHRDR;
  __IO uint32_t DMACHTBAR;
  __IO uint32_t DMACHRBAR;
} ETH_TypeDef;



/* Peripheral memory map */
#define FLASH_BASE            ((uint32_t)0x08000000) /* FLASH base address in the alias region */
#define SRAM_BASE             ((uint32_t)0x20000000) /* SRAM base address in the alias region */
#define PERIPH_BASE           ((uint32_t)0x40000000) /* Peripheral base address in the alias region */

#define FSMC_R_BASE           ((uint32_t)0xA0000000) /* FSMC registers base address */


#define APB1PERIPH_BASE       (PERIPH_BASE)
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000)
#define AHBPERIPH_BASE        (PERIPH_BASE + 0x20000)

#define TIM2_BASE             (APB1PERIPH_BASE + 0x0000)
#define TIM3_BASE             (APB1PERIPH_BASE + 0x0400)
#define TIM4_BASE             (APB1PERIPH_BASE + 0x0800)
#define TIM5_BASE             (APB1PERIPH_BASE + 0x0C00)
#define TIM6_BASE             (APB1PERIPH_BASE + 0x1000)
#define TIM7_BASE             (APB1PERIPH_BASE + 0x1400)
#define UART6_BASE            (APB1PERIPH_BASE + 0x1800)
#define UART7_BASE            (APB1PERIPH_BASE + 0x1C00)
#define UART8_BASE            (APB1PERIPH_BASE + 0x2000)
#define RTC_BASE              (APB1PERIPH_BASE + 0x2800)
#define WWDG_BASE             (APB1PERIPH_BASE + 0x2C00)
#define IWDG_BASE             (APB1PERIPH_BASE + 0x3000)
#define SPI2_BASE             (APB1PERIPH_BASE + 0x3800)
#define SPI3_BASE             (APB1PERIPH_BASE + 0x3C00)
#define USART2_BASE           (APB1PERIPH_BASE + 0x4400)
#define USART3_BASE           (APB1PERIPH_BASE + 0x4800)
#define UART4_BASE            (APB1PERIPH_BASE + 0x4C00)
#define UART5_BASE            (APB1PERIPH_BASE + 0x5000)
#define I2C1_BASE             (APB1PERIPH_BASE + 0x5400)
#define I2C2_BASE             (APB1PERIPH_BASE + 0x5800)
#define CAN1_BASE             (APB1PERIPH_BASE + 0x6400)
#define CAN2_BASE             (APB1PERIPH_BASE + 0x6800)
#define BKP_BASE              (APB1PERIPH_BASE + 0x6C00)
#define PWR_BASE              (APB1PERIPH_BASE + 0x7000)
#define DAC_BASE              (APB1PERIPH_BASE + 0x7400)

#define AFIO_BASE             (APB2PERIPH_BASE + 0x0000)
#define EXTI_BASE             (APB2PERIPH_BASE + 0x0400)
#define GPIOA_BASE            (APB2PERIPH_BASE + 0x0800)
#define GPIOB_BASE            (APB2PERIPH_BASE + 0x0C00)
#define GPIOC_BASE            (APB2PERIPH_BASE + 0x1000)
#define GPIOD_BASE            (APB2PERIPH_BASE + 0x1400)
#define GPIOE_BASE            (APB2PERIPH_BASE + 0x1800)
#define GPIOF_BASE            (APB2PERIPH_BASE + 0x1C00)
#define GPIOG_BASE            (APB2PERIPH_BASE + 0x2000)
#define ADC1_BASE             (APB2PERIPH_BASE + 0x2400)
#define ADC2_BASE             (APB2PERIPH_BASE + 0x2800)
#define TIM1_BASE             (APB2PERIPH_BASE + 0x2C00)
#define SPI1_BASE             (APB2PERIPH_BASE + 0x3000)
#define TIM8_BASE             (APB2PERIPH_BASE + 0x3400)
#define USART1_BASE           (APB2PERIPH_BASE + 0x3800)
#define ADC3_BASE             (APB2PERIPH_BASE + 0x3C00)
#define TIM15_BASE            (APB2PERIPH_BASE + 0x4000)
#define TIM16_BASE            (APB2PERIPH_BASE + 0x4400)
#define TIM17_BASE            (APB2PERIPH_BASE + 0x4800)
#define TIM9_BASE             (APB2PERIPH_BASE + 0x4C00)
#define TIM10_BASE            (APB2PERIPH_BASE + 0x5000)
#define TIM11_BASE            (APB2PERIPH_BASE + 0x5400)
#define SDIO_BASE             (APB2PERIPH_BASE + 0x8000)

#define DMA1_BASE             (AHBPERIPH_BASE + 0x0000)
#define DMA1_Channel1_BASE    (AHBPERIPH_BASE + 0x0008)
#define DMA1_Channel2_BASE    (AHBPERIPH_BASE + 0x001C)
#define DMA1_Channel3_BASE    (AHBPERIPH_BASE + 0x0030)
#define DMA1_Channel4_BASE    (AHBPERIPH_BASE + 0x0044)
#define DMA1_Channel5_BASE    (AHBPERIPH_BASE + 0x0058)
#define DMA1_Channel6_BASE    (AHBPERIPH_BASE + 0x006C)
#define DMA1_Channel7_BASE    (AHBPERIPH_BASE + 0x0080)
#define DMA2_BASE             (AHBPERIPH_BASE + 0x0400)
#define DMA2_Channel1_BASE    (AHBPERIPH_BASE + 0x0408)
#define DMA2_Channel2_BASE    (AHBPERIPH_BASE + 0x041C)
#define DMA2_Channel3_BASE    (AHBPERIPH_BASE + 0x0430)
#define DMA2_Channel4_BASE    (AHBPERIPH_BASE + 0x0444)
#define DMA2_Channel5_BASE    (AHBPERIPH_BASE + 0x0458)
#define DMA2_Channel6_BASE    (AHBPERIPH_BASE + 0x046C)
#define DMA2_Channel7_BASE    (AHBPERIPH_BASE + 0x0480)
#define DMA2_Channel8_BASE    (AHBPERIPH_BASE + 0x0490)
#define DMA2_Channel9_BASE    (AHBPERIPH_BASE + 0x04A0)
#define DMA2_Channel10_BASE   (AHBPERIPH_BASE + 0x04B0)
#define DMA2_Channel11_BASE   (AHBPERIPH_BASE + 0x04C0)
#define DMA2_EXTEN_BASE       (AHBPERIPH_BASE + 0x04D0)
#define RCC_BASE              (AHBPERIPH_BASE + 0x1000)
#define FLASH_R_BASE          (AHBPERIPH_BASE + 0x2000)
#define CRC_BASE              (AHBPERIPH_BASE + 0x3000)
#define USBHS_BASE            (AHBPERIPH_BASE + 0x3400)
#define EXTEN_BASE            (AHBPERIPH_BASE + 0x3800)
#define OPA_BASE              (AHBPERIPH_BASE + 0x3804)
#define RNG_BASE              (AHBPERIPH_BASE + 0x3C00)

#define ETH_BASE              (AHBPERIPH_BASE + 0x8000)
#define ETH_MAC_BASE          (ETH_BASE)
#define ETH_MMC_BASE          (ETH_BASE + 0x0100)
#define ETH_PTP_BASE          (ETH_BASE + 0x0700)
#define ETH_DMA_BASE          (ETH_BASE + 0x1000)

#define USBFS_BASE            ((uint32_t)0x50000000)
#define DVP_BASE              ((uint32_t)0x50050000)

#define FSMC_Bank1_R_BASE     (FSMC_R_BASE + 0x0000) 
#define FSMC_Bank1E_R_BASE    (FSMC_R_BASE + 0x0104) 
#define FSMC_Bank2_R_BASE     (FSMC_R_BASE + 0x0060) 

#define OB_BASE               ((uint32_t)0x1FFFF800)

/* Peripheral declaration */
#define TIM2                ((TIM_TypeDef *) TIM2_BASE)
#define TIM3                ((TIM_TypeDef *) TIM3_BASE)
#define TIM4                ((TIM_TypeDef *) TIM4_BASE)
#define TIM5                ((TIM_TypeDef *) TIM5_BASE)
#define TIM6                ((TIM_TypeDef *) TIM6_BASE)
#define TIM7                ((TIM_TypeDef *) TIM7_BASE)
#define UART6               ((USART_TypeDef *) UART6_BASE)
#define UART7               ((USART_TypeDef *) UART7_BASE)
#define UART8               ((USART_TypeDef *) UART8_BASE)
#define RTC                 ((RTC_TypeDef *) RTC_BASE)
#define WWDG                ((WWDG_TypeDef *) WWDG_BASE)
#define IWDG                ((IWDG_TypeDef *) IWDG_BASE)
#define SPI2                ((SPI_TypeDef *) SPI2_BASE)
#define SPI3                ((SPI_TypeDef *) SPI3_BASE)
#define USART2              ((USART_TypeDef *) USART2_BASE)
#define USART3              ((USART_TypeDef *) USART3_BASE)
#define UART4               ((USART_TypeDef *) UART4_BASE)
#define UART5               ((USART_TypeDef *) UART5_BASE)
#define I2C1                ((I2C_TypeDef *) I2C1_BASE)
#define I2C2                ((I2C_TypeDef *) I2C2_BASE)
#define CAN1                ((CAN_TypeDef *) CAN1_BASE)
#define CAN2                ((CAN_TypeDef *) CAN2_BASE)
#define BKP                 ((BKP_TypeDef *) BKP_BASE)
#define PWR                 ((PWR_TypeDef *) PWR_BASE)
#define DAC                 ((DAC_TypeDef *) DAC_BASE)

#define AFIO                ((AFIO_TypeDef *) AFIO_BASE)
#define EXTI                ((EXTI_TypeDef *) EXTI_BASE)
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE               ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOF               ((GPIO_TypeDef *) GPIOF_BASE)
#define GPIOG               ((GPIO_TypeDef *) GPIOG_BASE)
#define ADC1                ((ADC_TypeDef *) ADC1_BASE)
#define ADC2                ((ADC_TypeDef *) ADC2_BASE)
#define TKey1               ((ADC_TypeDef *) ADC1_BASE)
#define TKey2               ((ADC_TypeDef *) ADC2_BASE)
#define TIM1                ((TIM_TypeDef *) TIM1_BASE)
#define SPI1                ((SPI_TypeDef *) SPI1_BASE)
#define TIM8                ((TIM_TypeDef *) TIM8_BASE)
#define USART1              ((USART_TypeDef *) USART1_BASE)
#define ADC3                ((ADC_TypeDef *) ADC3_BASE)
#define TIM15               ((TIM_TypeDef *) TIM15_BASE)
#define TIM16               ((TIM_TypeDef *) TIM16_BASE)
#define TIM17               ((TIM_TypeDef *) TIM17_BASE)
#define TIM9                ((TIM_TypeDef *) TIM9_BASE)
#define TIM10               ((TIM_TypeDef *) TIM10_BASE)
#define TIM11               ((TIM_TypeDef *) TIM11_BASE)
#define SDIO                ((SDIO_TypeDef *) SDIO_BASE)

#define DMA1                ((DMA_TypeDef *) DMA1_BASE)
#define DMA2                ((DMA_TypeDef *) DMA2_BASE)
#define DMA2_EXTEN          ((DMA_TypeDef *) DMA2_EXTEN_BASE)
#define DMA1_Channel1       ((DMA_Channel_TypeDef *) DMA1_Channel1_BASE)
#define DMA1_Channel2       ((DMA_Channel_TypeDef *) DMA1_Channel2_BASE)
#define DMA1_Channel3       ((DMA_Channel_TypeDef *) DMA1_Channel3_BASE)
#define DMA1_Channel4       ((DMA_Channel_TypeDef *) DMA1_Channel4_BASE)
#define DMA1_Channel5       ((DMA_Channel_TypeDef *) DMA1_Channel5_BASE)
#define DMA1_Channel6       ((DMA_Channel_TypeDef *) DMA1_Channel6_BASE)
#define DMA1_Channel7       ((DMA_Channel_TypeDef *) DMA1_Channel7_BASE)
#define DMA2_Channel1       ((DMA_Channel_TypeDef *) DMA2_Channel1_BASE)
#define DMA2_Channel2       ((DMA_Channel_TypeDef *) DMA2_Channel2_BASE)
#define DMA2_Channel3       ((DMA_Channel_TypeDef *) DMA2_Channel3_BASE)
#define DMA2_Channel4       ((DMA_Channel_TypeDef *) DMA2_Channel4_BASE)
#define DMA2_Channel5       ((DMA_Channel_TypeDef *) DMA2_Channel5_BASE)
#define DMA2_Channel6       ((DMA_Channel_TypeDef *) DMA2_Channel6_BASE)
#define DMA2_Channel7       ((DMA_Channel_TypeDef *) DMA2_Channel7_BASE)
#define DMA2_Channel8       ((DMA_Channel_TypeDef *) DMA2_Channel8_BASE)
#define DMA2_Channel9       ((DMA_Channel_TypeDef *) DMA2_Channel9_BASE)
#define DMA2_Channel10      ((DMA_Channel_TypeDef *) DMA2_Channel10_BASE)
#define DMA2_Channel11      ((DMA_Channel_TypeDef *) DMA2_Channel11_BASE)
#define RCC                 ((RCC_TypeDef *) RCC_BASE)
#define FLASH               ((FLASH_TypeDef *) FLASH_R_BASE)
#define CRC                 ((CRC_TypeDef *) CRC_BASE)
#define USBHSD              ((USBHSD_TypeDef *) USBHS_BASE)
#define USBHSH              ((USBHSH_TypeDef *) USBHS_BASE)
#define USBOTG_FS           ((USBOTG_FS_TypeDef *)USBFS_BASE)
#define USBOTG_H_FS         ((USBOTGH_FS_TypeDef *)USBFS_BASE)
#define EXTEN               ((EXTEN_TypeDef *) EXTEN_BASE)
#define OPA                 ((OPA_TypeDef *) OPA_BASE)
#define RNG                 ((RNG_TypeDef *) RNG_BASE)
#define ETH                 ((ETH_TypeDef *) ETH_BASE)

#define DVP                 ((DVP_TypeDef *) DVP_BASE)

#define FSMC_Bank1          ((FSMC_Bank1_TypeDef *) FSMC_Bank1_R_BASE)
#define FSMC_Bank1E         ((FSMC_Bank1E_TypeDef *) FSMC_Bank1E_R_BASE)
#define FSMC_Bank2          ((FSMC_Bank2_TypeDef *) FSMC_Bank2_R_BASE)

#define OB                  ((OB_TypeDef *) OB_BASE)

/******************************************************************************/
/*                         Peripheral Registers Bits Definition               */
/******************************************************************************/

/******************************************************************************/
/*                        Analog to Digital Converter                         */
/******************************************************************************/

/********************  Bit definition for ADC_STATR register  ********************/
#define  ADC_AWD                        ((uint8_t)0x01)               /* Analog watchdog flag */
#define  ADC_EOC                        ((uint8_t)0x02)               /* End of conversion */
#define  ADC_JEOC                       ((uint8_t)0x04)               /* Injected channel end of conversion */
#define  ADC_JSTRT                      ((uint8_t)0x08)               /* Injected channel Start flag */
#define  ADC_STRT                       ((uint8_t)0x10)               /* Regular channel Start flag */

/*******************  Bit definition for ADC_CTLR1 register  ********************/
#define  ADC_AWDCH                      ((uint32_t)0x0000001F)        /* AWDCH[4:0] bits (Analog watchdog channel select bits) */
#define  ADC_AWDCH_0                    ((uint32_t)0x00000001)        /* Bit 0 */
#define  ADC_AWDCH_1                    ((uint32_t)0x00000002)        /* Bit 1 */
#define  ADC_AWDCH_2                    ((uint32_t)0x00000004)        /* Bit 2 */
#define  ADC_AWDCH_3                    ((uint32_t)0x00000008)        /* Bit 3 */
#define  ADC_AWDCH_4                    ((uint32_t)0x00000010)        /* Bit 4 */

#define  ADC_EOCIE                      ((uint32_t)0x00000020)        /* Interrupt enable for EOC */
#define  ADC_AWDIE                      ((uint32_t)0x00000040)        /* Analog Watchdog interrupt enable */
#define  ADC_JEOCIE                     ((uint32_t)0x00000080)        /* Interrupt enable for injected channels */
#define  ADC_SCAN                       ((uint32_t)0x00000100)        /* Scan mode */
#define  ADC_AWDSGL                     ((uint32_t)0x00000200)        /* Enable the watchdog on a single channel in scan mode */
#define  ADC_JAUTO                      ((uint32_t)0x00000400)        /* Automatic injected group conversion */
#define  ADC_DISCEN                     ((uint32_t)0x00000800)        /* Discontinuous mode on regular channels */
#define  ADC_JDISCEN                    ((uint32_t)0x00001000)        /* Discontinuous mode on injected channels */

#define  ADC_DISCNUM                    ((uint32_t)0x0000E000)        /* DISCNUM[2:0] bits (Discontinuous mode channel count) */
#define  ADC_DISCNUM_0                  ((uint32_t)0x00002000)        /* Bit 0 */
#define  ADC_DISCNUM_1                  ((uint32_t)0x00004000)        /* Bit 1 */
#define  ADC_DISCNUM_2                  ((uint32_t)0x00008000)        /* Bit 2 */

#define  ADC_DUALMOD                    ((uint32_t)0x000F0000)        /* DUALMOD[3:0] bits (Dual mode selection) */
#define  ADC_DUALMOD_0                  ((uint32_t)0x00010000)        /* Bit 0 */
#define  ADC_DUALMOD_1                  ((uint32_t)0x00020000)        /* Bit 1 */
#define  ADC_DUALMOD_2                  ((uint32_t)0x00040000)        /* Bit 2 */
#define  ADC_DUALMOD_3                  ((uint32_t)0x00080000)        /* Bit 3 */

#define  ADC_JAWDEN                     ((uint32_t)0x00400000)        /* Analog watchdog enable on injected channels */
#define  ADC_AWDEN                      ((uint32_t)0x00800000)        /* Analog watchdog enable on regular channels */

/*******************  Bit definition for ADC_CTLR2 register  ********************/
#define  ADC_ADON                       ((uint32_t)0x00000001)        /* A/D Converter ON / OFF */
#define  ADC_CONT                       ((uint32_t)0x00000002)        /* Continuous Conversion */
#define  ADC_CAL                        ((uint32_t)0x00000004)        /* A/D Calibration */
#define  ADC_RSTCAL                     ((uint32_t)0x00000008)        /* Reset Calibration */
#define  ADC_DMA                        ((uint32_t)0x00000100)        /* Direct Memory access mode */
#define  ADC_ALIGN                      ((uint32_t)0x00000800)        /* Data Alignment */

#define  ADC_JEXTSEL                    ((uint32_t)0x00007000)        /* JEXTSEL[2:0] bits (External event select for injected group) */
#define  ADC_JEXTSEL_0                  ((uint32_t)0x00001000)        /* Bit 0 */
#define  ADC_JEXTSEL_1                  ((uint32_t)0x00002000)        /* Bit 1 */
#define  ADC_JEXTSEL_2                  ((uint32_t)0x00004000)        /* Bit 2 */

#define  ADC_JEXTTRIG                   ((uint32_t)0x00008000)        /* External Trigger Conversion mode for injected channels */

#define  ADC_EXTSEL                     ((uint32_t)0x000E0000)        /* EXTSEL[2:0] bits (External Event Select for regular group) */
#define  ADC_EXTSEL_0                   ((uint32_t)0x00020000)        /* Bit 0 */
#define  ADC_EXTSEL_1                   ((uint32_t)0x00040000)        /* Bit 1 */
#define  ADC_EXTSEL_2                   ((uint32_t)0x00080000)        /* Bit 2 */

#define  ADC_EXTTRIG                    ((uint32_t)0x00100000)        /* External Trigger Conversion mode for regular channels */
#define  ADC_JSWSTART                   ((uint32_t)0x00200000)        /* Start Conversion of injected channels */
#define  ADC_SWSTART                    ((uint32_t)0x00400000)        /* Start Conversion of regular channels */
#define  ADC_TSVREFE                    ((uint32_t)0x00800000)        /* Temperature Sensor and VREFINT Enable */

/******************  Bit definition for ADC_SAMPTR1 register  *******************/
#define  ADC_SMP10                      ((uint32_t)0x00000007)        /* SMP10[2:0] bits (Channel 10 Sample time selection) */
#define  ADC_SMP10_0                    ((uint32_t)0x00000001)        /* Bit 0 */
#define  ADC_SMP10_1                    ((uint32_t)0x00000002)        /* Bit 1 */
#define  ADC_SMP10_2                    ((uint32_t)0x00000004)        /* Bit 2 */

#define  ADC_SMP11                      ((uint32_t)0x00000038)        /* SMP11[2:0] bits (Channel 11 Sample time selection) */
#define  ADC_SMP11_0                    ((uint32_t)0x00000008)        /* Bit 0 */
#define  ADC_SMP11_1                    ((uint32_t)0x00000010)        /* Bit 1 */
#define  ADC_SMP11_2                    ((uint32_t)0x00000020)        /* Bit 2 */

#define  ADC_SMP12                      ((uint32_t)0x000001C0)        /* SMP12[2:0] bits (Channel 12 Sample time selection) */
#define  ADC_SMP12_0                    ((uint32_t)0x00000040)        /* Bit 0 */
#define  ADC_SMP12_1                    ((uint32_t)0x00000080)        /* Bit 1 */
#define  ADC_SMP12_2                    ((uint32_t)0x00000100)        /* Bit 2 */

#define  ADC_SMP13                      ((uint32_t)0x00000E00)        /* SMP13[2:0] bits (Channel 13 Sample time selection) */
#define  ADC_SMP13_0                    ((uint32_t)0x00000200)        /* Bit 0 */
#define  ADC_SMP13_1                    ((uint32_t)0x00000400)        /* Bit 1 */
#define  ADC_SMP13_2                    ((uint32_t)0x00000800)        /* Bit 2 */

#define  ADC_SMP14                      ((uint32_t)0x00007000)        /* SMP14[2:0] bits (Channel 14 Sample time selection) */
#define  ADC_SMP14_0                    ((uint32_t)0x00001000)        /* Bit 0 */
#define  ADC_SMP14_1                    ((uint32_t)0x00002000)        /* Bit 1 */
#define  ADC_SMP14_2                    ((uint32_t)0x00004000)        /* Bit 2 */

#define  ADC_SMP15                      ((uint32_t)0x00038000)        /* SMP15[2:0] bits (Channel 15 Sample time selection) */
#define  ADC_SMP15_0                    ((uint32_t)0x00008000)        /* Bit 0 */
#define  ADC_SMP15_1                    ((uint32_t)0x00010000)        /* Bit 1 */
#define  ADC_SMP15_2                    ((uint32_t)0x00020000)        /* Bit 2 */

#define  ADC_SMP16                      ((uint32_t)0x001C0000)        /* SMP16[2:0] bits (Channel 16 Sample time selection) */
#define  ADC_SMP16_0                    ((uint32_t)0x00040000)        /* Bit 0 */
#define  ADC_SMP16_1                    ((uint32_t)0x00080000)        /* Bit 1 */
#define  ADC_SMP16_2                    ((uint32_t)0x00100000)        /* Bit 2 */

#define  ADC_SMP17                      ((uint32_t)0x00E00000)        /* SMP17[2:0] bits (Channel 17 Sample time selection) */
#define  ADC_SMP17_0                    ((uint32_t)0x00200000)        /* Bit 0 */
#define  ADC_SMP17_1                    ((uint32_t)0x00400000)        /* Bit 1 */
#define  ADC_SMP17_2                    ((uint32_t)0x00800000)        /* Bit 2 */

/******************  Bit definition for ADC_SAMPTR2 register  *******************/
#define  ADC_SMP0                       ((uint32_t)0x00000007)        /* SMP0[2:0] bits (Channel 0 Sample time selection) */
#define  ADC_SMP0_0                     ((uint32_t)0x00000001)        /* Bit 0 */
#define  ADC_SMP0_1                     ((uint32_t)0x00000002)        /* Bit 1 */
#define  ADC_SMP0_2                     ((uint32_t)0x00000004)        /* Bit 2 */

#define  ADC_SMP1                       ((uint32_t)0x00000038)        /* SMP1[2:0] bits (Channel 1 Sample time selection) */
#define  ADC_SMP1_0                     ((uint32_t)0x00000008)        /* Bit 0 */
#define  ADC_SMP1_1                     ((uint32_t)0x00000010)        /* Bit 1 */
#define  ADC_SMP1_2                     ((uint32_t)0x00000020)        /* Bit 2 */

#define  ADC_SMP2                       ((uint32_t)0x000001C0)        /* SMP2[2:0] bits (Channel 2 Sample time selection) */
#define  ADC_SMP2_0                     ((uint32_t)0x00000040)        /* Bit 0 */
#define  ADC_SMP2_1                     ((uint32_t)0x00000080)        /* Bit 1 */
#define  ADC_SMP2_2                     ((uint32_t)0x00000100)        /* Bit 2 */

#define  ADC_SMP3                       ((uint32_t)0x00000E00)        /* SMP3[2:0] bits (Channel 3 Sample time selection) */
#define  ADC_SMP3_0                     ((uint32_t)0x00000200)        /* Bit 0 */
#define  ADC_SMP3_1                     ((uint32_t)0x00000400)        /* Bit 1 */
#define  ADC_SMP3_2                     ((uint32_t)0x00000800)        /* Bit 2 */

#define  ADC_SMP4                       ((uint32_t)0x00007000)        /* SMP4[2:0] bits (Channel 4 Sample time selection) */
#define  ADC_SMP4_0                     ((uint32_t)0x00001000)        /* Bit 0 */
#define  ADC_SMP4_1                     ((uint32_t)0x00002000)        /* Bit 1 */
#define  ADC_SMP4_2                     ((uint32_t)0x00004000)        /* Bit 2 */

#define  ADC_SMP5                       ((uint32_t)0x00038000)        /* SMP5[2:0] bits (Channel 5 Sample time selection) */
#define  ADC_SMP5_0                     ((uint32_t)0x00008000)        /* Bit 0 */
#define  ADC_SMP5_1                     ((uint32_t)0x00010000)        /* Bit 1 */
#define  ADC_SMP5_2                     ((uint32_t)0x00020000)        /* Bit 2 */

#define  ADC_SMP6                       ((uint32_t)0x001C0000)        /* SMP6[2:0] bits (Channel 6 Sample time selection) */
#define  ADC_SMP6_0                     ((uint32_t)0x00040000)        /* Bit 0 */
#define  ADC_SMP6_1                     ((uint32_t)0x00080000)        /* Bit 1 */
#define  ADC_SMP6_2                     ((uint32_t)0x00100000)        /* Bit 2 */

#define  ADC_SMP7                       ((uint32_t)0x00E00000)        /* SMP7[2:0] bits (Channel 7 Sample time selection) */
#define  ADC_SMP7_0                     ((uint32_t)0x00200000)        /* Bit 0 */
#define  ADC_SMP7_1                     ((uint32_t)0x00400000)        /* Bit 1 */
#define  ADC_SMP7_2                     ((uint32_t)0x00800000)        /* Bit 2 */

#define  ADC_SMP8                       ((uint32_t)0x07000000)        /* SMP8[2:0] bits (Channel 8 Sample time selection) */
#define  ADC_SMP8_0                     ((uint32_t)0x01000000)        /* Bit 0 */
#define  ADC_SMP8_1                     ((uint32_t)0x02000000)        /* Bit 1 */
#define  ADC_SMP8_2                     ((uint32_t)0x04000000)        /* Bit 2 */

#define  ADC_SMP9                       ((uint32_t)0x38000000)        /* SMP9[2:0] bits (Channel 9 Sample time selection) */
#define  ADC_SMP9_0                     ((uint32_t)0x08000000)        /* Bit 0 */
#define  ADC_SMP9_1                     ((uint32_t)0x10000000)        /* Bit 1 */
#define  ADC_SMP9_2                     ((uint32_t)0x20000000)        /* Bit 2 */

/******************  Bit definition for ADC_IOFR1 register  *******************/
#define  ADC_JOFFSET1                   ((uint16_t)0x0FFF)            /* Data offset for injected channel 1 */

/******************  Bit definition for ADC_IOFR2 register  *******************/
#define  ADC_JOFFSET2                   ((uint16_t)0x0FFF)            /* Data offset for injected channel 2 */

/******************  Bit definition for ADC_IOFR3 register  *******************/
#define  ADC_JOFFSET3                   ((uint16_t)0x0FFF)            /* Data offset for injected channel 3 */

/******************  Bit definition for ADC_IOFR4 register  *******************/
#define  ADC_JOFFSET4                   ((uint16_t)0x0FFF)            /* Data offset for injected channel 4 */

/*******************  Bit definition for ADC_WDHTR register  ********************/
#define  ADC_HT                         ((uint16_t)0x0FFF)            /* Analog watchdog high threshold */

/*******************  Bit definition for ADC_WDLTR register  ********************/
#define  ADC_LT                         ((uint16_t)0x0FFF)            /* Analog watchdog low threshold */

/*******************  Bit definition for ADC_RSQR1 register  *******************/
#define  ADC_SQ13                       ((uint32_t)0x0000001F)        /* SQ13[4:0] bits (13th conversion in regular sequence) */
#define  ADC_SQ13_0                     ((uint32_t)0x00000001)        /* Bit 0 */
#define  ADC_SQ13_1                     ((uint32_t)0x00000002)        /* Bit 1 */
#define  ADC_SQ13_2                     ((uint32_t)0x00000004)        /* Bit 2 */
#define  ADC_SQ13_3                     ((uint32_t)0x00000008)        /* Bit 3 */
#define  ADC_SQ13_4                     ((uint32_t)0x00000010)        /* Bit 4 */

#define  ADC_SQ14                       ((uint32_t)0x000003E0)        /* SQ14[4:0] bits (14th conversion in regular sequence) */
#define  ADC_SQ14_0                     ((uint32_t)0x00000020)        /* Bit 0 */
#define  ADC_SQ14_1                     ((uint32_t)0x00000040)        /* Bit 1 */
#define  ADC_SQ14_2                     ((uint32_t)0x00000080)        /* Bit 2 */
#define  ADC_SQ14_3                     ((uint32_t)0x00000100)        /* Bit 3 */
#define  ADC_SQ14_4                     ((uint32_t)0x00000200)        /* Bit 4 */

#define  ADC_SQ15                       ((uint32_t)0x00007C00)        /* SQ15[4:0] bits (15th conversion in regular sequence) */
#define  ADC_SQ15_0                     ((uint32_t)0x00000400)        /* Bit 0 */
#define  ADC_SQ15_1                     ((uint32_t)0x00000800)        /* Bit 1 */
#define  ADC_SQ15_2                     ((uint32_t)0x00001000)        /* Bit 2 */
#define  ADC_SQ15_3                     ((uint32_t)0x00002000)        /* Bit 3 */
#define  ADC_SQ15_4                     ((uint32_t)0x00004000)        /* Bit 4 */

#define  ADC_SQ16                       ((uint32_t)0x000F8000)        /* SQ16[4:0] bits (16th conversion in regular sequence) */
#define  ADC_SQ16_0                     ((uint32_t)0x00008000)        /* Bit 0 */
#define  ADC_SQ16_1                     ((uint32_t)0x00010000)        /* Bit 1 */
#define  ADC_SQ16_2                     ((uint32_t)0x00020000)        /* Bit 2 */
#define  ADC_SQ16_3                     ((uint32_t)0x00040000)        /* Bit 3 */
#define  ADC_SQ16_4                     ((uint32_t)0x00080000)        /* Bit 4 */

#define  ADC_L                          ((uint32_t)0x00F00000)        /* L[3:0] bits (Regular channel sequence length) */
#define  ADC_L_0                        ((uint32_t)0x00100000)        /* Bit 0 */
#define  ADC_L_1                        ((uint32_t)0x00200000)        /* Bit 1 */
#define  ADC_L_2                        ((uint32_t)0x00400000)        /* Bit 2 */
#define  ADC_L_3                        ((uint32_t)0x00800000)        /* Bit 3 */

/*******************  Bit definition for ADC_RSQR2 register  *******************/
#define  ADC_SQ7                        ((uint32_t)0x0000001F)        /* SQ7[4:0] bits (7th conversion in regular sequence) */
#define  ADC_SQ7_0                      ((uint32_t)0x00000001)        /* Bit 0 */
#define  ADC_SQ7_1                      ((uint32_t)0x00000002)        /* Bit 1 */
#define  ADC_SQ7_2                      ((uint32_t)0x00000004)        /* Bit 2 */
#define  ADC_SQ7_3                      ((uint32_t)0x00000008)        /* Bit 3 */
#define  ADC_SQ7_4                      ((uint32_t)0x00000010)        /* Bit 4 */

#define  ADC_SQ8                        ((uint32_t)0x000003E0)        /* SQ8[4:0] bits (8th conversion in regular sequence) */
#define  ADC_SQ8_0                      ((uint32_t)0x00000020)        /* Bit 0 */
#define  ADC_SQ8_1                      ((uint32_t)0x00000040)        /* Bit 1 */
#define  ADC_SQ8_2                      ((uint32_t)0x00000080)        /* Bit 2 */
#define  ADC_SQ8_3                      ((uint32_t)0x00000100)        /* Bit 3 */
#define  ADC_SQ8_4                      ((uint32_t)0x00000200)        /* Bit 4 */

#define  ADC_SQ9                        ((uint32_t)0x00007C00)        /* SQ9[4:0] bits (9th conversion in regular sequence) */
#define  ADC_SQ9_0                      ((uint32_t)0x00000400)        /* Bit 0 */
#define  ADC_SQ9_1                      ((uint32_t)0x00000800)        /* Bit 1 */
#define  ADC_SQ9_2                      ((uint32_t)0x00001000)        /* Bit 2 */
#define  ADC_SQ9_3                      ((uint32_t)0x00002000)        /* Bit 3 */
#define  ADC_SQ9_4                      ((uint32_t)0x00004000)        /* Bit 4 */

#define  ADC_SQ10                       ((uint32_t)0x000F8000)        /* SQ10[4:0] bits (10th conversion in regular sequence) */
#define  ADC_SQ10_0                     ((uint32_t)0x00008000)        /* Bit 0 */
#define  ADC_SQ10_1                     ((uint32_t)0x00010000)        /* Bit 1 */
#define  ADC_SQ10_2                     ((uint32_t)0x00020000)        /* Bit 2 */
#define  ADC_SQ10_3                     ((uint32_t)0x00040000)        /* Bit 3 */
#define  ADC_SQ10_4                     ((uint32_t)0x00080000)        /* Bit 4 */

#define  ADC_SQ11                       ((uint32_t)0x01F00000)        /* SQ11[4:0] bits (11th conversion in regular sequence) */
#define  ADC_SQ11_0                     ((uint32_t)0x00100000)        /* Bit 0 */
#define  ADC_SQ11_1                     ((uint32_t)0x00200000)        /* Bit 1 */
#define  ADC_SQ11_2                     ((uint32_t)0x00400000)        /* Bit 2 */
#define  ADC_SQ11_3                     ((uint32_t)0x00800000)        /* Bit 3 */
#define  ADC_SQ11_4                     ((uint32_t)0x01000000)        /* Bit 4 */

#define  ADC_SQ12                       ((uint32_t)0x3E000000)        /* SQ12[4:0] bits (12th conversion in regular sequence) */
#define  ADC_SQ12_0                     ((uint32_t)0x02000000)        /* Bit 0 */
#define  ADC_SQ12_1                     ((uint32_t)0x04000000)        /* Bit 1 */
#define  ADC_SQ12_2                     ((uint32_t)0x08000000)        /* Bit 2 */
#define  ADC_SQ12_3                     ((uint32_t)0x10000000)        /* Bit 3 */
#define  ADC_SQ12_4                     ((uint32_t)0x20000000)        /* Bit 4 */

/*******************  Bit definition for ADC_RSQR3 register  *******************/
#define  ADC_SQ1                        ((uint32_t)0x0000001F)        /* SQ1[4:0] bits (1st conversion in regular sequence) */
#define  ADC_SQ1_0                      ((uint32_t)0x00000001)        /* Bit 0 */
#define  ADC_SQ1_1                      ((uint32_t)0x00000002)        /* Bit 1 */
#define  ADC_SQ1_2                      ((uint32_t)0x00000004)        /* Bit 2 */
#define  ADC_SQ1_3                      ((uint32_t)0x00000008)        /* Bit 3 */
#define  ADC_SQ1_4                      ((uint32_t)0x00000010)        /* Bit 4 */

#define  ADC_SQ2                        ((uint32_t)0x000003E0)        /* SQ2[4:0] bits (2nd conversion in regular sequence) */
#define  ADC_SQ2_0                      ((uint32_t)0x00000020)        /* Bit 0 */
#define  ADC_SQ2_1                      ((uint32_t)0x00000040)        /* Bit 1 */
#define  ADC_SQ2_2                      ((uint32_t)0x00000080)        /* Bit 2 */
#define  ADC_SQ2_3                      ((uint32_t)0x00000100)        /* Bit 3 */
#define  ADC_SQ2_4                      ((uint32_t)0x00000200)        /* Bit 4 */

#define  ADC_SQ3                        ((uint32_t)0x00007C00)        /* SQ3[4:0] bits (3rd conversion in regular sequence) */
#define  ADC_SQ3_0                      ((uint32_t)0x00000400)        /* Bit 0 */
#define  ADC_SQ3_1                      ((uint32_t)0x00000800)        /* Bit 1 */
#define  ADC_SQ3_2                      ((uint32_t)0x00001000)        /* Bit 2 */
#define  ADC_SQ3_3                      ((uint32_t)0x00002000)        /* Bit 3 */
#define  ADC_SQ3_4                      ((uint32_t)0x00004000)        /* Bit 4 */

#define  ADC_SQ4                        ((uint32_t)0x000F8000)        /* SQ4[4:0] bits (4th conversion in regular sequence) */
#define  ADC_SQ4_0                      ((uint32_t)0x00008000)        /* Bit 0 */
#define  ADC_SQ4_1                      ((uint32_t)0x00010000)        /* Bit 1 */
#define  ADC_SQ4_2                      ((uint32_t)0x00020000)        /* Bit 2 */
#define  ADC_SQ4_3                      ((uint32_t)0x00040000)        /* Bit 3 */
#define  ADC_SQ4_4                      ((uint32_t)0x00080000)        /* Bit 4 */

#define  ADC_SQ5                        ((uint32_t)0x01F00000)        /* SQ5[4:0] bits (5th conversion in regular sequence) */
#define  ADC_SQ5_0                      ((uint32_t)0x00100000)        /* Bit 0 */
#define  ADC_SQ5_1                      ((uint32_t)0x00200000)        /* Bit 1 */
#define  ADC_SQ5_2                      ((uint32_t)0x00400000)        /* Bit 2 */
#define  ADC_SQ5_3                      ((uint32_t)0x00800000)        /* Bit 3 */
#define  ADC_SQ5_4                      ((uint32_t)0x01000000)        /* Bit 4 */

#define  ADC_SQ6                        ((uint32_t)0x3E000000)        /* SQ6[4:0] bits (6th conversion in regular sequence) */
#define  ADC_SQ6_0                      ((uint32_t)0x02000000)        /* Bit 0 */
#define  ADC_SQ6_1                      ((uint32_t)0x04000000)        /* Bit 1 */
#define  ADC_SQ6_2                      ((uint32_t)0x08000000)        /* Bit 2 */
#define  ADC_SQ6_3                      ((uint32_t)0x10000000)        /* Bit 3 */
#define  ADC_SQ6_4                      ((uint32_t)0x20000000)        /* Bit 4 */

/*******************  Bit definition for ADC_ISQR register  *******************/
#define  ADC_JSQ1                       ((uint32_t)0x0000001F)        /* JSQ1[4:0] bits (1st conversion in injected sequence) */  
#define  ADC_JSQ1_0                     ((uint32_t)0x00000001)        /* Bit 0 */
#define  ADC_JSQ1_1                     ((uint32_t)0x00000002)        /* Bit 1 */
#define  ADC_JSQ1_2                     ((uint32_t)0x00000004)        /* Bit 2 */
#define  ADC_JSQ1_3                     ((uint32_t)0x00000008)        /* Bit 3 */
#define  ADC_JSQ1_4                     ((uint32_t)0x00000010)        /* Bit 4 */

#define  ADC_JSQ2                       ((uint32_t)0x000003E0)        /* JSQ2[4:0] bits (2nd conversion in injected sequence) */
#define  ADC_JSQ2_0                     ((uint32_t)0x00000020)        /* Bit 0 */
#define  ADC_JSQ2_1                     ((uint32_t)0x00000040)        /* Bit 1 */
#define  ADC_JSQ2_2                     ((uint32_t)0x00000080)        /* Bit 2 */
#define  ADC_JSQ2_3                     ((uint32_t)0x00000100)        /* Bit 3 */
#define  ADC_JSQ2_4                     ((uint32_t)0x00000200)        /* Bit 4 */

#define  ADC_JSQ3                       ((uint32_t)0x00007C00)        /* JSQ3[4:0] bits (3rd conversion in injected sequence) */
#define  ADC_JSQ3_0                     ((uint32_t)0x00000400)        /* Bit 0 */
#define  ADC_JSQ3_1                     ((uint32_t)0x00000800)        /* Bit 1 */
#define  ADC_JSQ3_2                     ((uint32_t)0x00001000)        /* Bit 2 */
#define  ADC_JSQ3_3                     ((uint32_t)0x00002000)        /* Bit 3 */
#define  ADC_JSQ3_4                     ((uint32_t)0x00004000)        /* Bit 4 */

#define  ADC_JSQ4                       ((uint32_t)0x000F8000)        /* JSQ4[4:0] bits (4th conversion in injected sequence) */
#define  ADC_JSQ4_0                     ((uint32_t)0x00008000)        /* Bit 0 */
#define  ADC_JSQ4_1                     ((uint32_t)0x00010000)        /* Bit 1 */
#define  ADC_JSQ4_2                     ((uint32_t)0x00020000)        /* Bit 2 */
#define  ADC_JSQ4_3                     ((uint32_t)0x00040000)        /* Bit 3 */
#define  ADC_JSQ4_4                     ((uint32_t)0x00080000)        /* Bit 4 */

#define  ADC_JL                         ((uint32_t)0x00300000)        /* JL[1:0] bits (Injected Sequence length) */
#define  ADC_JL_0                       ((uint32_t)0x00100000)        /* Bit 0 */
#define  ADC_JL_1                       ((uint32_t)0x00200000)        /* Bit 1 */

/*******************  Bit definition for ADC_IDATAR1 register  *******************/
#define  ADC_IDATAR1_JDATA              ((uint16_t)0xFFFF)            /* Injected data */

/*******************  Bit definition for ADC_IDATAR2 register  *******************/
#define  ADC_IDATAR2_JDATA              ((uint16_t)0xFFFF)            /* Injected data */

/*******************  Bit definition for ADC_IDATAR3 register  *******************/
#define  ADC_IDATAR3_JDATA              ((uint16_t)0xFFFF)            /* Injected data */

/*******************  Bit definition for ADC_IDATAR4 register  *******************/
#define  ADC_IDATAR4_JDATA              ((uint16_t)0xFFFF)            /* Injected data */

/********************  Bit definition for ADC_RDATAR register  ********************/
#define  ADC_RDATAR_DATA                ((uint32_t)0x0000FFFF)        /* Regular data */
#define  ADC_RDATAR_ADC2DATA            ((uint32_t)0xFFFF0000)        /* ADC2 data */

/******************************************************************************/
/*                            Backup registers                                */
/******************************************************************************/

/*******************  Bit definition for BKP_DATAR1 register  ********************/
#define  BKP_DATAR1_D                           ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR2 register  ********************/
#define  BKP_DATAR2_D                           ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR3 register  ********************/
#define  BKP_DATAR3_D                           ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR4 register  ********************/
#define  BKP_DATAR4_D                           ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR5 register  ********************/
#define  BKP_DATAR5_D                           ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR6 register  ********************/
#define  BKP_DATAR6_D                           ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR7 register  ********************/
#define  BKP_DATAR7_D                           ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR8 register  ********************/
#define  BKP_DATAR8_D                           ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR9 register  ********************/
#define  BKP_DATAR9_D                           ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR10 register  *******************/
#define  BKP_DATAR10_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR11 register  *******************/
#define  BKP_DATAR11_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR12 register  *******************/
#define  BKP_DATAR12_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR13 register  *******************/
#define  BKP_DATAR13_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR14 register  *******************/
#define  BKP_DATAR14_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR15 register  *******************/
#define  BKP_DATAR15_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR16 register  *******************/
#define  BKP_DATAR16_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR17 register  *******************/
#define  BKP_DATAR17_D                          ((uint16_t)0xFFFF)     /* Backup data */

/******************  Bit definition for BKP_DATAR18 register  ********************/
#define  BKP_DATAR18_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR19 register  *******************/
#define  BKP_DATAR19_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR20 register  *******************/
#define  BKP_DATAR20_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR21 register  *******************/
#define  BKP_DATAR21_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR22 register  *******************/
#define  BKP_DATAR22_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR23 register  *******************/
#define  BKP_DATAR23_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR24 register  *******************/
#define  BKP_DATAR24_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR25 register  *******************/
#define  BKP_DATAR25_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR26 register  *******************/
#define  BKP_DATAR26_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR27 register  *******************/
#define  BKP_DATAR27_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR28 register  *******************/
#define  BKP_DATAR28_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR29 register  *******************/
#define  BKP_DATAR29_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR30 register  *******************/
#define  BKP_DATAR30_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR31 register  *******************/
#define  BKP_DATAR31_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR32 register  *******************/
#define  BKP_DATAR32_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR33 register  *******************/
#define  BKP_DATAR33_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR34 register  *******************/
#define  BKP_DATAR34_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR35 register  *******************/
#define  BKP_DATAR35_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR36 register  *******************/
#define  BKP_DATAR36_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR37 register  *******************/
#define  BKP_DATAR37_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR38 register  *******************/
#define  BKP_DATAR38_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR39 register  *******************/
#define  BKP_DATAR39_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR40 register  *******************/
#define  BKP_DATAR40_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR41 register  *******************/
#define  BKP_DATAR41_D                          ((uint16_t)0xFFFF)     /* Backup data */

/*******************  Bit definition for BKP_DATAR42 register  *******************/
#define  BKP_DATAR42_D                          ((uint16_t)0xFFFF)     /* Backup data */

/******************  Bit definition for BKP_OCTLR register  *******************/
#define  BKP_CAL                                ((uint16_t)0x007F)     /* Calibration value */
#define  BKP_CCO                                ((uint16_t)0x0080)     /* Calibration Clock Output */
#define  BKP_ASOE                               ((uint16_t)0x0100)     /* Alarm or Second Output Enable */
#define  BKP_ASOS                               ((uint16_t)0x0200)     /* Alarm or Second Output Selection */

/********************  Bit definition for BKP_TPCTLR register  ********************/
#define  BKP_TPE                                ((uint8_t)0x01)        /* TAMPER pin enable */
#define  BKP_TPAL                               ((uint8_t)0x02)        /* TAMPER pin active level */

/*******************  Bit definition for BKP_TPCSR register  ********************/
#define  BKP_CTE                                ((uint16_t)0x0001)     /* Clear Tamper event */
#define  BKP_CTI                                ((uint16_t)0x0002)     /* Clear Tamper Interrupt */
#define  BKP_TPIE                               ((uint16_t)0x0004)     /* TAMPER Pin interrupt enable */
#define  BKP_TEF                                ((uint16_t)0x0100)     /* Tamper Event Flag */
#define  BKP_TIF                                ((uint16_t)0x0200)     /* Tamper Interrupt Flag */

/******************************************************************************/
/*                         Controller Area Network                            */
/******************************************************************************/

/*******************  Bit definition for CAN_CTLR register  ********************/
#define  CAN_CTLR_INRQ                       ((uint16_t)0x0001)            /* Initialization Request */
#define  CAN_CTLR_SLEEP                      ((uint16_t)0x0002)            /* Sleep Mode Request */
#define  CAN_CTLR_TXFP                       ((uint16_t)0x0004)            /* Transmit FIFO Priority */
#define  CAN_CTLR_RFLM                       ((uint16_t)0x0008)            /* Receive FIFO Locked Mode */
#define  CAN_CTLR_NART                       ((uint16_t)0x0010)            /* No Automatic Retransmission */
#define  CAN_CTLR_AWUM                       ((uint16_t)0x0020)            /* Automatic Wakeup Mode */
#define  CAN_CTLR_ABOM                       ((uint16_t)0x0040)            /* Automatic Bus-Off Management */
#define  CAN_CTLR_TTCM                       ((uint16_t)0x0080)            /* Time Triggered Communication Mode */
#define  CAN_CTLR_RESET                      ((uint16_t)0x8000)            /* CAN software master reset */

/*******************  Bit definition for CAN_STATR register  ********************/
#define  CAN_STATR_INAK                      ((uint16_t)0x0001)            /* Initialization Acknowledge */
#define  CAN_STATR_SLAK                      ((uint16_t)0x0002)            /* Sleep Acknowledge */
#define  CAN_STATR_ERRI                      ((uint16_t)0x0004)            /* Error Interrupt */
#define  CAN_STATR_WKUI                      ((uint16_t)0x0008)            /* Wakeup Interrupt */
#define  CAN_STATR_SLAKI                     ((uint16_t)0x0010)            /* Sleep Acknowledge Interrupt */
#define  CAN_STATR_TXM                       ((uint16_t)0x0100)            /* Transmit Mode */
#define  CAN_STATR_RXM                       ((uint16_t)0x0200)            /* Receive Mode */
#define  CAN_STATR_SAMP                      ((uint16_t)0x0400)            /* Last Sample Point */
#define  CAN_STATR_RX                        ((uint16_t)0x0800)            /* CAN Rx Signal */

/*******************  Bit definition for CAN_TSTATR register  ********************/
#define  CAN_TSTATR_RQCP0                    ((uint32_t)0x00000001)        /* Request Completed Mailbox0 */
#define  CAN_TSTATR_TXOK0                    ((uint32_t)0x00000002)        /* Transmission OK of Mailbox0 */
#define  CAN_TSTATR_ALST0                    ((uint32_t)0x00000004)        /* Arbitration Lost for Mailbox0 */
#define  CAN_TSTATR_TERR0                    ((uint32_t)0x00000008)        /* Transmission Error of Mailbox0 */
#define  CAN_TSTATR_ABRQ0                    ((uint32_t)0x00000080)        /* Abort Request for Mailbox0 */
#define  CAN_TSTATR_RQCP1                    ((uint32_t)0x00000100)        /* Request Completed Mailbox1 */
#define  CAN_TSTATR_TXOK1                    ((uint32_t)0x00000200)        /* Transmission OK of Mailbox1 */
#define  CAN_TSTATR_ALST1                    ((uint32_t)0x00000400)        /* Arbitration Lost for Mailbox1 */
#define  CAN_TSTATR_TERR1                    ((uint32_t)0x00000800)        /* Transmission Error of Mailbox1 */
#define  CAN_TSTATR_ABRQ1                    ((uint32_t)0x00008000)        /* Abort Request for Mailbox 1 */
#define  CAN_TSTATR_RQCP2                    ((uint32_t)0x00010000)        /* Request Completed Mailbox2 */
#define  CAN_TSTATR_TXOK2                    ((uint32_t)0x00020000)        /* Transmission OK of Mailbox 2 */
#define  CAN_TSTATR_ALST2                    ((uint32_t)0x00040000)        /* Arbitration Lost for mailbox 2 */
#define  CAN_TSTATR_TERR2                    ((uint32_t)0x00080000)        /* Transmission Error of Mailbox 2 */
#define  CAN_TSTATR_ABRQ2                    ((uint32_t)0x00800000)        /* Abort Request for Mailbox 2 */
#define  CAN_TSTATR_CODE                     ((uint32_t)0x03000000)        /* Mailbox Code */

#define  CAN_TSTATR_TME                      ((uint32_t)0x1C000000)        /* TME[2:0] bits */
#define  CAN_TSTATR_TME0                     ((uint32_t)0x04000000)        /* Transmit Mailbox 0 Empty */
#define  CAN_TSTATR_TME1                     ((uint32_t)0x08000000)        /* Transmit Mailbox 1 Empty */
#define  CAN_TSTATR_TME2                     ((uint32_t)0x10000000)        /* Transmit Mailbox 2 Empty */

#define  CAN_TSTATR_LOW                      ((uint32_t)0xE0000000)        /* LOW[2:0] bits */
#define  CAN_TSTATR_LOW0                     ((uint32_t)0x20000000)        /* Lowest Priority Flag for Mailbox 0 */
#define  CAN_TSTATR_LOW1                     ((uint32_t)0x40000000)        /* Lowest Priority Flag for Mailbox 1 */
#define  CAN_TSTATR_LOW2                     ((uint32_t)0x80000000)        /* Lowest Priority Flag for Mailbox 2 */

/*******************  Bit definition for CAN_RFIFO0 register  *******************/
#define  CAN_RFIFO0_FMP0                     ((uint8_t)0x03)               /* FIFO 0 Message Pending */
#define  CAN_RFIFO0_FULL0                    ((uint8_t)0x08)               /* FIFO 0 Full */
#define  CAN_RFIFO0_FOVR0                    ((uint8_t)0x10)               /* FIFO 0 Overrun */
#define  CAN_RFIFO0_RFOM0                    ((uint8_t)0x20)               /* Release FIFO 0 Output Mailbox */

/*******************  Bit definition for CAN_RFIFO1 register  *******************/
#define  CAN_RFIFO1_FMP1                     ((uint8_t)0x03)               /* FIFO 1 Message Pending */
#define  CAN_RFIFO1_FULL1                    ((uint8_t)0x08)               /* FIFO 1 Full */
#define  CAN_RFIFO1_FOVR1                    ((uint8_t)0x10)               /* FIFO 1 Overrun */
#define  CAN_RFIFO1_RFOM1                    ((uint8_t)0x20)               /* Release FIFO 1 Output Mailbox */

/********************  Bit definition for CAN_INTENR register  *******************/
#define  CAN_INTENR_TMEIE                    ((uint32_t)0x00000001)        /* Transmit Mailbox Empty Interrupt Enable */
#define  CAN_INTENR_FMPIE0                   ((uint32_t)0x00000002)        /* FIFO Message Pending Interrupt Enable */
#define  CAN_INTENR_FFIE0                    ((uint32_t)0x00000004)        /* FIFO Full Interrupt Enable */
#define  CAN_INTENR_FOVIE0                   ((uint32_t)0x00000008)        /* FIFO Overrun Interrupt Enable */
#define  CAN_INTENR_FMPIE1                   ((uint32_t)0x00000010)        /* FIFO Message Pending Interrupt Enable */
#define  CAN_INTENR_FFIE1                    ((uint32_t)0x00000020)        /* FIFO Full Interrupt Enable */
#define  CAN_INTENR_FOVIE1                   ((uint32_t)0x00000040)        /* FIFO Overrun Interrupt Enable */
#define  CAN_INTENR_EWGIE                    ((uint32_t)0x00000100)        /* Error Warning Interrupt Enable */
#define  CAN_INTENR_EPVIE                    ((uint32_t)0x00000200)        /* Error Passive Interrupt Enable */
#define  CAN_INTENR_BOFIE                    ((uint32_t)0x00000400)        /* Bus-Off Interrupt Enable */
#define  CAN_INTENR_LECIE                    ((uint32_t)0x00000800)        /* Last Error Code Interrupt Enable */
#define  CAN_INTENR_ERRIE                    ((uint32_t)0x00008000)        /* Error Interrupt Enable */
#define  CAN_INTENR_WKUIE                    ((uint32_t)0x00010000)        /* Wakeup Interrupt Enable */
#define  CAN_INTENR_SLKIE                    ((uint32_t)0x00020000)        /* Sleep Interrupt Enable */

/********************  Bit definition for CAN_ERRSR register  *******************/
#define  CAN_ERRSR_EWGF                      ((uint32_t)0x00000001)        /* Error Warning Flag */
#define  CAN_ERRSR_EPVF                      ((uint32_t)0x00000002)        /* Error Passive Flag */
#define  CAN_ERRSR_BOFF                      ((uint32_t)0x00000004)        /* Bus-Off Flag */

#define  CAN_ERRSR_LEC                       ((uint32_t)0x00000070)        /* LEC[2:0] bits (Last Error Code) */
#define  CAN_ERRSR_LEC_0                     ((uint32_t)0x00000010)        /* Bit 0 */
#define  CAN_ERRSR_LEC_1                     ((uint32_t)0x00000020)        /* Bit 1 */
#define  CAN_ERRSR_LEC_2                     ((uint32_t)0x00000040)        /* Bit 2 */

#define  CAN_ERRSR_TEC                       ((uint32_t)0x00FF0000)        /* Least significant byte of the 9-bit Transmit Error Counter */
#define  CAN_ERRSR_REC                       ((uint32_t)0xFF000000)        /* Receive Error Counter */

/*******************  Bit definition for CAN_BTIMR register  ********************/
#define  CAN_BTIMR_BRP                       ((uint32_t)0x000003FF)        /* Baud Rate Prescaler */
#define  CAN_BTIMR_TS1                       ((uint32_t)0x000F0000)        /* Time Segment 1 */
#define  CAN_BTIMR_TS2                       ((uint32_t)0x00700000)        /* Time Segment 2 */
#define  CAN_BTIMR_SJW                       ((uint32_t)0x03000000)        /* Resynchronization Jump Width */
#define  CAN_BTIMR_LBKM                      ((uint32_t)0x40000000)        /* Loop Back Mode (Debug) */
#define  CAN_BTIMR_SILM                      ((uint32_t)0x80000000)        /* Silent Mode */

/******************  Bit definition for CAN_TXMI0R register  ********************/
#define  CAN_TXMI0R_TXRQ                     ((uint32_t)0x00000001)        /* Transmit Mailbox Request */
#define  CAN_TXMI0R_RTR                      ((uint32_t)0x00000002)        /* Remote Transmission Request */
#define  CAN_TXMI0R_IDE                      ((uint32_t)0x00000004)        /* Identifier Extension */
#define  CAN_TXMI0R_EXID                     ((uint32_t)0x001FFFF8)        /* Extended Identifier */
#define  CAN_TXMI0R_STID                     ((uint32_t)0xFFE00000)        /* Standard Identifier or Extended Identifier */

/******************  Bit definition for CAN_TXMDT0R register  *******************/
#define  CAN_TXMDT0R_DLC                     ((uint32_t)0x0000000F)        /* Data Length Code */
#define  CAN_TXMDT0R_TGT                     ((uint32_t)0x00000100)        /* Transmit Global Time */
#define  CAN_TXMDT0R_TIME                    ((uint32_t)0xFFFF0000)        /* Message Time Stamp */

/******************  Bit definition for CAN_TXMDL0R register  *******************/
#define  CAN_TXMDL0R_DATA0                   ((uint32_t)0x000000FF)        /* Data byte 0 */
#define  CAN_TXMDL0R_DATA1                   ((uint32_t)0x0000FF00)        /* Data byte 1 */
#define  CAN_TXMDL0R_DATA2                   ((uint32_t)0x00FF0000)        /* Data byte 2 */
#define  CAN_TXMDL0R_DATA3                   ((uint32_t)0xFF000000)        /* Data byte 3 */

/******************  Bit definition for CAN_TXMDH0R register  *******************/
#define  CAN_TXMDH0R_DATA4                   ((uint32_t)0x000000FF)        /* Data byte 4 */
#define  CAN_TXMDH0R_DATA5                   ((uint32_t)0x0000FF00)        /* Data byte 5 */
#define  CAN_TXMDH0R_DATA6                   ((uint32_t)0x00FF0000)        /* Data byte 6 */
#define  CAN_TXMDH0R_DATA7                   ((uint32_t)0xFF000000)        /* Data byte 7 */

/*******************  Bit definition for CAN_TXMI1R register  *******************/
#define  CAN_TXMI1R_TXRQ                     ((uint32_t)0x00000001)        /* Transmit Mailbox Request */
#define  CAN_TXMI1R_RTR                      ((uint32_t)0x00000002)        /* Remote Transmission Request */
#define  CAN_TXMI1R_IDE                      ((uint32_t)0x00000004)        /* Identifier Extension */
#define  CAN_TXMI1R_EXID                     ((uint32_t)0x001FFFF8)        /* Extended Identifier */
#define  CAN_TXMI1R_STID                     ((uint32_t)0xFFE00000)        /* Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_TXMDT1R register  ******************/
#define  CAN_TXMDT1R_DLC                     ((uint32_t)0x0000000F)        /* Data Length Code */
#define  CAN_TXMDT1R_TGT                     ((uint32_t)0x00000100)        /* Transmit Global Time */
#define  CAN_TXMDT1R_TIME                    ((uint32_t)0xFFFF0000)        /* Message Time Stamp */

/*******************  Bit definition for CAN_TXMDL1R register  ******************/
#define  CAN_TXMDL1R_DATA0                   ((uint32_t)0x000000FF)        /* Data byte 0 */
#define  CAN_TXMDL1R_DATA1                   ((uint32_t)0x0000FF00)        /* Data byte 1 */
#define  CAN_TXMDL1R_DATA2                   ((uint32_t)0x00FF0000)        /* Data byte 2 */
#define  CAN_TXMDL1R_DATA3                   ((uint32_t)0xFF000000)        /* Data byte 3 */

/*******************  Bit definition for CAN_TXMDH1R register  ******************/
#define  CAN_TXMDH1R_DATA4                   ((uint32_t)0x000000FF)        /* Data byte 4 */
#define  CAN_TXMDH1R_DATA5                   ((uint32_t)0x0000FF00)        /* Data byte 5 */
#define  CAN_TXMDH1R_DATA6                   ((uint32_t)0x00FF0000)        /* Data byte 6 */
#define  CAN_TXMDH1R_DATA7                   ((uint32_t)0xFF000000)        /* Data byte 7 */

/*******************  Bit definition for CAN_TXMI2R register  *******************/
#define  CAN_TXMI2R_TXRQ                     ((uint32_t)0x00000001)        /* Transmit Mailbox Request */
#define  CAN_TXMI2R_RTR                      ((uint32_t)0x00000002)        /* Remote Transmission Request */
#define  CAN_TXMI2R_IDE                      ((uint32_t)0x00000004)        /* Identifier Extension */
#define  CAN_TXMI2R_EXID                     ((uint32_t)0x001FFFF8)        /* Extended identifier */
#define  CAN_TXMI2R_STID                     ((uint32_t)0xFFE00000)        /* Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_TXMDT2R register  ******************/  
#define  CAN_TXMDT2R_DLC                     ((uint32_t)0x0000000F)        /* Data Length Code */
#define  CAN_TXMDT2R_TGT                     ((uint32_t)0x00000100)        /* Transmit Global Time */
#define  CAN_TXMDT2R_TIME                    ((uint32_t)0xFFFF0000)        /* Message Time Stamp */

/*******************  Bit definition for CAN_TXMDL2R register  ******************/
#define  CAN_TXMDL2R_DATA0                   ((uint32_t)0x000000FF)        /* Data byte 0 */
#define  CAN_TXMDL2R_DATA1                   ((uint32_t)0x0000FF00)        /* Data byte 1 */
#define  CAN_TXMDL2R_DATA2                   ((uint32_t)0x00FF0000)        /* Data byte 2 */
#define  CAN_TXMDL2R_DATA3                   ((uint32_t)0xFF000000)        /* Data byte 3 */

/*******************  Bit definition for CAN_TXMDH2R register  ******************/
#define  CAN_TXMDH2R_DATA4                   ((uint32_t)0x000000FF)        /* Data byte 4 */
#define  CAN_TXMDH2R_DATA5                   ((uint32_t)0x0000FF00)        /* Data byte 5 */
#define  CAN_TXMDH2R_DATA6                   ((uint32_t)0x00FF0000)        /* Data byte 6 */
#define  CAN_TXMDH2R_DATA7                   ((uint32_t)0xFF000000)        /* Data byte 7 */

/*******************  Bit definition for CAN_RXMI0R register  *******************/
#define  CAN_RXMI0R_RTR                      ((uint32_t)0x00000002)        /* Remote Transmission Request */
#define  CAN_RXMI0R_IDE                      ((uint32_t)0x00000004)        /* Identifier Extension */
#define  CAN_RXMI0R_EXID                     ((uint32_t)0x001FFFF8)        /* Extended Identifier */
#define  CAN_RXMI0R_STID                     ((uint32_t)0xFFE00000)        /* Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_RXMDT0R register  ******************/
#define  CAN_RXMDT0R_DLC                     ((uint32_t)0x0000000F)        /* Data Length Code */
#define  CAN_RXMDT0R_FMI                     ((uint32_t)0x0000FF00)        /* Filter Match Index */
#define  CAN_RXMDT0R_TIME                    ((uint32_t)0xFFFF0000)        /* Message Time Stamp */

/*******************  Bit definition for CAN_RXMDL0R register  ******************/
#define  CAN_RXMDL0R_DATA0                   ((uint32_t)0x000000FF)        /* Data byte 0 */
#define  CAN_RXMDL0R_DATA1                   ((uint32_t)0x0000FF00)        /* Data byte 1 */
#define  CAN_RXMDL0R_DATA2                   ((uint32_t)0x00FF0000)        /* Data byte 2 */
#define  CAN_RXMDL0R_DATA3                   ((uint32_t)0xFF000000)        /* Data byte 3 */

/*******************  Bit definition for CAN_RXMDH0R register  ******************/
#define  CAN_RXMDH0R_DATA4                   ((uint32_t)0x000000FF)        /* Data byte 4 */
#define  CAN_RXMDH0R_DATA5                   ((uint32_t)0x0000FF00)        /* Data byte 5 */
#define  CAN_RXMDH0R_DATA6                   ((uint32_t)0x00FF0000)        /* Data byte 6 */
#define  CAN_RXMDH0R_DATA7                   ((uint32_t)0xFF000000)        /* Data byte 7 */

/*******************  Bit definition for CAN_RXMI1R register  *******************/
#define  CAN_RXMI1R_RTR                      ((uint32_t)0x00000002)        /* Remote Transmission Request */
#define  CAN_RXMI1R_IDE                      ((uint32_t)0x00000004)        /* Identifier Extension */
#define  CAN_RXMI1R_EXID                     ((uint32_t)0x001FFFF8)        /* Extended identifier */
#define  CAN_RXMI1R_STID                     ((uint32_t)0xFFE00000)        /* Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_RXMDT1R register  ******************/
#define  CAN_RXMDT1R_DLC                     ((uint32_t)0x0000000F)        /* Data Length Code */
#define  CAN_RXMDT1R_FMI                     ((uint32_t)0x0000FF00)        /* Filter Match Index */
#define  CAN_RXMDT1R_TIME                    ((uint32_t)0xFFFF0000)        /* Message Time Stamp */

/*******************  Bit definition for CAN_RXMDL1R register  ******************/
#define  CAN_RXMDL1R_DATA0                   ((uint32_t)0x000000FF)        /* Data byte 0 */
#define  CAN_RXMDL1R_DATA1                   ((uint32_t)0x0000FF00)        /* Data byte 1 */
#define  CAN_RXMDL1R_DATA2                   ((uint32_t)0x00FF0000)        /* Data byte 2 */
#define  CAN_RXMDL1R_DATA3                   ((uint32_t)0xFF000000)        /* Data byte 3 */

/*******************  Bit definition for CAN_RXMDH1R register  ******************/
#define  CAN_RXMDH1R_DATA4                   ((uint32_t)0x000000FF)        /* Data byte 4 */
#define  CAN_RXMDH1R_DATA5                   ((uint32_t)0x0000FF00)        /* Data byte 5 */
#define  CAN_RXMDH1R_DATA6                   ((uint32_t)0x00FF0000)        /* Data byte 6 */
#define  CAN_RXMDH1R_DATA7                   ((uint32_t)0xFF000000)        /* Data byte 7 */

/*******************  Bit definition for CAN_FCTLR register  ********************/
#define  CAN_FCTLR_FINIT                     ((uint8_t)0x01)               /* Filter Init Mode */

/*******************  Bit definition for CAN_FMCFGR register  *******************/
#define  CAN_FMCFGR_FBM                      ((uint16_t)0x3FFF)            /* Filter Mode */
#define  CAN_FMCFGR_FBM0                     ((uint16_t)0x0001)            /* Filter Init Mode bit 0 */
#define  CAN_FMCFGR_FBM1                     ((uint16_t)0x0002)            /* Filter Init Mode bit 1 */
#define  CAN_FMCFGR_FBM2                     ((uint16_t)0x0004)            /* Filter Init Mode bit 2 */
#define  CAN_FMCFGR_FBM3                     ((uint16_t)0x0008)            /* Filter Init Mode bit 3 */
#define  CAN_FMCFGR_FBM4                     ((uint16_t)0x0010)            /* Filter Init Mode bit 4 */
#define  CAN_FMCFGR_FBM5                     ((uint16_t)0x0020)            /* Filter Init Mode bit 5 */
#define  CAN_FMCFGR_FBM6                     ((uint16_t)0x0040)            /* Filter Init Mode bit 6 */
#define  CAN_FMCFGR_FBM7                     ((uint16_t)0x0080)            /* Filter Init Mode bit 7 */
#define  CAN_FMCFGR_FBM8                     ((uint16_t)0x0100)            /* Filter Init Mode bit 8 */
#define  CAN_FMCFGR_FBM9                     ((uint16_t)0x0200)            /* Filter Init Mode bit 9 */
#define  CAN_FMCFGR_FBM10                    ((uint16_t)0x0400)            /* Filter Init Mode bit 10 */
#define  CAN_FMCFGR_FBM11                    ((uint16_t)0x0800)            /* Filter Init Mode bit 11 */
#define  CAN_FMCFGR_FBM12                    ((uint16_t)0x1000)            /* Filter Init Mode bit 12 */
#define  CAN_FMCFGR_FBM13                    ((uint16_t)0x2000)            /* Filter Init Mode bit 13 */

/*******************  Bit definition for CAN_FSCFGR register  *******************/
#define  CAN_FSCFGR_FSC                      ((uint16_t)0x3FFF)            /* Filter Scale Configuration */
#define  CAN_FSCFGR_FSC0                     ((uint16_t)0x0001)            /* Filter Scale Configuration bit 0 */
#define  CAN_FSCFGR_FSC1                     ((uint16_t)0x0002)            /* Filter Scale Configuration bit 1 */
#define  CAN_FSCFGR_FSC2                     ((uint16_t)0x0004)            /* Filter Scale Configuration bit 2 */
#define  CAN_FSCFGR_FSC3                     ((uint16_t)0x0008)            /* Filter Scale Configuration bit 3 */
#define  CAN_FSCFGR_FSC4                     ((uint16_t)0x0010)            /* Filter Scale Configuration bit 4 */
#define  CAN_FSCFGR_FSC5                     ((uint16_t)0x0020)            /* Filter Scale Configuration bit 5 */
#define  CAN_FSCFGR_FSC6                     ((uint16_t)0x0040)            /* Filter Scale Configuration bit 6 */
#define  CAN_FSCFGR_FSC7                     ((uint16_t)0x0080)            /* Filter Scale Configuration bit 7 */
#define  CAN_FSCFGR_FSC8                     ((uint16_t)0x0100)            /* Filter Scale Configuration bit 8 */
#define  CAN_FSCFGR_FSC9                     ((uint16_t)0x0200)            /* Filter Scale Configuration bit 9 */
#define  CAN_FSCFGR_FSC10                    ((uint16_t)0x0400)            /* Filter Scale Configuration bit 10 */
#define  CAN_FSCFGR_FSC11                    ((uint16_t)0x0800)            /* Filter Scale Configuration bit 11 */
#define  CAN_FSCFGR_FSC12                    ((uint16_t)0x1000)            /* Filter Scale Configuration bit 12 */
#define  CAN_FSCFGR_FSC13                    ((uint16_t)0x2000)            /* Filter Scale Configuration bit 13 */

/******************  Bit definition for CAN_FAFIFOR register  *******************/
#define  CAN_FAFIFOR_FFA                     ((uint16_t)0x3FFF)            /* Filter FIFO Assignment */
#define  CAN_FAFIFOR_FFA0                    ((uint16_t)0x0001)            /* Filter FIFO Assignment for Filter 0 */
#define  CAN_FAFIFOR_FFA1                    ((uint16_t)0x0002)            /* Filter FIFO Assignment for Filter 1 */
#define  CAN_FAFIFOR_FFA2                    ((uint16_t)0x0004)            /* Filter FIFO Assignment for Filter 2 */
#define  CAN_FAFIFOR_FFA3                    ((uint16_t)0x0008)            /* Filter FIFO Assignment for Filter 3 */
#define  CAN_FAFIFOR_FFA4                    ((uint16_t)0x0010)            /* Filter FIFO Assignment for Filter 4 */
#define  CAN_FAFIFOR_FFA5                    ((uint16_t)0x0020)            /* Filter FIFO Assignment for Filter 5 */
#define  CAN_FAFIFOR_FFA6                    ((uint16_t)0x0040)            /* Filter FIFO Assignment for Filter 6 */
#define  CAN_FAFIFOR_FFA7                    ((uint16_t)0x0080)            /* Filter FIFO Assignment for Filter 7 */
#define  CAN_FAFIFOR_FFA8                    ((uint16_t)0x0100)            /* Filter FIFO Assignment for Filter 8 */
#define  CAN_FAFIFOR_FFA9                    ((uint16_t)0x0200)            /* Filter FIFO Assignment for Filter 9 */
#define  CAN_FAFIFOR_FFA10                   ((uint16_t)0x0400)            /* Filter FIFO Assignment for Filter 10 */
#define  CAN_FAFIFOR_FFA11                   ((uint16_t)0x0800)            /* Filter FIFO Assignment for Filter 11 */
#define  CAN_FAFIFOR_FFA12                   ((uint16_t)0x1000)            /* Filter FIFO Assignment for Filter 12 */
#define  CAN_FAFIFOR_FFA13                   ((uint16_t)0x2000)            /* Filter FIFO Assignment for Filter 13 */

/*******************  Bit definition for CAN_FWR register  *******************/
#define  CAN_FWR_FACT                        ((uint16_t)0x3FFF)            /* Filter Active */
#define  CAN_FWR_FACT0                       ((uint16_t)0x0001)            /* Filter 0 Active */
#define  CAN_FWR_FACT1                       ((uint16_t)0x0002)            /* Filter 1 Active */
#define  CAN_FWR_FACT2                       ((uint16_t)0x0004)            /* Filter 2 Active */
#define  CAN_FWR_FACT3                       ((uint16_t)0x0008)            /* Filter 3 Active */
#define  CAN_FWR_FACT4                       ((uint16_t)0x0010)            /* Filter 4 Active */
#define  CAN_FWR_FACT5                       ((uint16_t)0x0020)            /* Filter 5 Active */
#define  CAN_FWR_FACT6                       ((uint16_t)0x0040)            /* Filter 6 Active */
#define  CAN_FWR_FACT7                       ((uint16_t)0x0080)            /* Filter 7 Active */
#define  CAN_FWR_FACT8                       ((uint16_t)0x0100)            /* Filter 8 Active */
#define  CAN_FWR_FACT9                       ((uint16_t)0x0200)            /* Filter 9 Active */
#define  CAN_FWR_FACT10                      ((uint16_t)0x0400)            /* Filter 10 Active */
#define  CAN_FWR_FACT11                      ((uint16_t)0x0800)            /* Filter 11 Active */
#define  CAN_FWR_FACT12                      ((uint16_t)0x1000)            /* Filter 12 Active */
#define  CAN_FWR_FACT13                      ((uint16_t)0x2000)            /* Filter 13 Active */

/*******************  Bit definition for CAN_F0R1 register  *******************/
#define  CAN_F0R1_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F0R1_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F0R1_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F0R1_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F0R1_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F0R1_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F0R1_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F0R1_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F0R1_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F0R1_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F0R1_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F0R1_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F0R1_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F0R1_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F0R1_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F0R1_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F0R1_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F0R1_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F0R1_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F0R1_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F0R1_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F0R1_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F0R1_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F0R1_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F0R1_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F0R1_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F0R1_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F0R1_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F0R1_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F0R1_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F0R1_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F0R1_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F1R1 register  *******************/
#define  CAN_F1R1_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F1R1_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F1R1_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F1R1_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F1R1_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F1R1_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F1R1_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F1R1_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F1R1_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F1R1_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F1R1_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F1R1_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F1R1_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F1R1_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F1R1_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F1R1_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F1R1_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F1R1_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F1R1_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F1R1_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F1R1_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F1R1_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F1R1_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F1R1_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F1R1_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F1R1_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F1R1_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F1R1_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F1R1_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F1R1_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F1R1_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F1R1_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F2R1 register  *******************/
#define  CAN_F2R1_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F2R1_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F2R1_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F2R1_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F2R1_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F2R1_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F2R1_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F2R1_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F2R1_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F2R1_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F2R1_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F2R1_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F2R1_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F2R1_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F2R1_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F2R1_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F2R1_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F2R1_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F2R1_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F2R1_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F2R1_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F2R1_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F2R1_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F2R1_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F2R1_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F2R1_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F2R1_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F2R1_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F2R1_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F2R1_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F2R1_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F2R1_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F3R1 register  *******************/
#define  CAN_F3R1_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F3R1_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F3R1_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F3R1_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F3R1_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F3R1_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F3R1_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F3R1_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F3R1_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F3R1_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F3R1_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F3R1_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F3R1_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F3R1_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F3R1_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F3R1_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F3R1_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F3R1_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F3R1_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F3R1_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F3R1_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F3R1_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F3R1_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F3R1_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F3R1_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F3R1_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F3R1_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F3R1_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F3R1_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F3R1_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F3R1_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F3R1_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F4R1 register  *******************/
#define  CAN_F4R1_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F4R1_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F4R1_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F4R1_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F4R1_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F4R1_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F4R1_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F4R1_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F4R1_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F4R1_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F4R1_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F4R1_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F4R1_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F4R1_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F4R1_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F4R1_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F4R1_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F4R1_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F4R1_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F4R1_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F4R1_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F4R1_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F4R1_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F4R1_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F4R1_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F4R1_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F4R1_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F4R1_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F4R1_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F4R1_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F4R1_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F4R1_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F5R1 register  *******************/
#define  CAN_F5R1_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F5R1_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F5R1_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F5R1_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F5R1_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F5R1_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F5R1_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F5R1_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F5R1_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F5R1_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F5R1_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F5R1_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F5R1_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F5R1_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F5R1_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F5R1_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F5R1_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F5R1_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F5R1_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F5R1_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F5R1_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F5R1_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F5R1_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F5R1_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F5R1_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F5R1_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F5R1_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F5R1_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F5R1_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F5R1_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F5R1_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F5R1_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F6R1 register  *******************/
#define  CAN_F6R1_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F6R1_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F6R1_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F6R1_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F6R1_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F6R1_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F6R1_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F6R1_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F6R1_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F6R1_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F6R1_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F6R1_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F6R1_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F6R1_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F6R1_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F6R1_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F6R1_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F6R1_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F6R1_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F6R1_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F6R1_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F6R1_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F6R1_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F6R1_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F6R1_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F6R1_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F6R1_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F6R1_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F6R1_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F6R1_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F6R1_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F6R1_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F7R1 register  *******************/
#define  CAN_F7R1_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F7R1_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F7R1_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F7R1_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F7R1_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F7R1_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F7R1_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F7R1_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F7R1_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F7R1_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F7R1_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F7R1_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F7R1_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F7R1_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F7R1_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F7R1_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F7R1_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F7R1_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F7R1_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F7R1_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F7R1_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F7R1_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F7R1_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F7R1_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F7R1_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F7R1_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F7R1_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F7R1_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F7R1_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F7R1_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F7R1_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F7R1_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F8R1 register  *******************/
#define  CAN_F8R1_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F8R1_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F8R1_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F8R1_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F8R1_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F8R1_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F8R1_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F8R1_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F8R1_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F8R1_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F8R1_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F8R1_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F8R1_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F8R1_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F8R1_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F8R1_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F8R1_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F8R1_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F8R1_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F8R1_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F8R1_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F8R1_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F8R1_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F8R1_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F8R1_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F8R1_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F8R1_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F8R1_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F8R1_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F8R1_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F8R1_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F8R1_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F9R1 register  *******************/
#define  CAN_F9R1_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F9R1_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F9R1_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F9R1_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F9R1_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F9R1_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F9R1_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F9R1_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F9R1_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F9R1_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F9R1_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F9R1_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F9R1_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F9R1_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F9R1_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F9R1_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F9R1_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F9R1_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F9R1_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F9R1_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F9R1_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F9R1_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F9R1_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F9R1_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F9R1_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F9R1_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F9R1_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F9R1_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F9R1_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F9R1_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F9R1_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F9R1_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F10R1 register  ******************/
#define  CAN_F10R1_FB0                       ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F10R1_FB1                       ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F10R1_FB2                       ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F10R1_FB3                       ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F10R1_FB4                       ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F10R1_FB5                       ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F10R1_FB6                       ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F10R1_FB7                       ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F10R1_FB8                       ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F10R1_FB9                       ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F10R1_FB10                      ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F10R1_FB11                      ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F10R1_FB12                      ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F10R1_FB13                      ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F10R1_FB14                      ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F10R1_FB15                      ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F10R1_FB16                      ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F10R1_FB17                      ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F10R1_FB18                      ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F10R1_FB19                      ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F10R1_FB20                      ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F10R1_FB21                      ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F10R1_FB22                      ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F10R1_FB23                      ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F10R1_FB24                      ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F10R1_FB25                      ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F10R1_FB26                      ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F10R1_FB27                      ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F10R1_FB28                      ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F10R1_FB29                      ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F10R1_FB30                      ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F10R1_FB31                      ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F11R1 register  ******************/
#define  CAN_F11R1_FB0                       ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F11R1_FB1                       ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F11R1_FB2                       ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F11R1_FB3                       ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F11R1_FB4                       ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F11R1_FB5                       ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F11R1_FB6                       ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F11R1_FB7                       ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F11R1_FB8                       ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F11R1_FB9                       ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F11R1_FB10                      ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F11R1_FB11                      ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F11R1_FB12                      ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F11R1_FB13                      ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F11R1_FB14                      ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F11R1_FB15                      ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F11R1_FB16                      ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F11R1_FB17                      ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F11R1_FB18                      ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F11R1_FB19                      ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F11R1_FB20                      ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F11R1_FB21                      ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F11R1_FB22                      ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F11R1_FB23                      ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F11R1_FB24                      ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F11R1_FB25                      ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F11R1_FB26                      ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F11R1_FB27                      ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F11R1_FB28                      ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F11R1_FB29                      ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F11R1_FB30                      ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F11R1_FB31                      ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F12R1 register  ******************/
#define  CAN_F12R1_FB0                       ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F12R1_FB1                       ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F12R1_FB2                       ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F12R1_FB3                       ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F12R1_FB4                       ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F12R1_FB5                       ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F12R1_FB6                       ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F12R1_FB7                       ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F12R1_FB8                       ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F12R1_FB9                       ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F12R1_FB10                      ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F12R1_FB11                      ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F12R1_FB12                      ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F12R1_FB13                      ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F12R1_FB14                      ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F12R1_FB15                      ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F12R1_FB16                      ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F12R1_FB17                      ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F12R1_FB18                      ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F12R1_FB19                      ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F12R1_FB20                      ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F12R1_FB21                      ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F12R1_FB22                      ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F12R1_FB23                      ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F12R1_FB24                      ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F12R1_FB25                      ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F12R1_FB26                      ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F12R1_FB27                      ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F12R1_FB28                      ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F12R1_FB29                      ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F12R1_FB30                      ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F12R1_FB31                      ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F13R1 register  ******************/
#define  CAN_F13R1_FB0                       ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F13R1_FB1                       ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F13R1_FB2                       ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F13R1_FB3                       ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F13R1_FB4                       ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F13R1_FB5                       ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F13R1_FB6                       ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F13R1_FB7                       ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F13R1_FB8                       ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F13R1_FB9                       ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F13R1_FB10                      ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F13R1_FB11                      ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F13R1_FB12                      ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F13R1_FB13                      ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F13R1_FB14                      ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F13R1_FB15                      ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F13R1_FB16                      ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F13R1_FB17                      ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F13R1_FB18                      ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F13R1_FB19                      ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F13R1_FB20                      ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F13R1_FB21                      ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F13R1_FB22                      ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F13R1_FB23                      ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F13R1_FB24                      ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F13R1_FB25                      ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F13R1_FB26                      ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F13R1_FB27                      ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F13R1_FB28                      ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F13R1_FB29                      ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F13R1_FB30                      ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F13R1_FB31                      ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F0R2 register  *******************/
#define  CAN_F0R2_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F0R2_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F0R2_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F0R2_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F0R2_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F0R2_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F0R2_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F0R2_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F0R2_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F0R2_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F0R2_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F0R2_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F0R2_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F0R2_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F0R2_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F0R2_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F0R2_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F0R2_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F0R2_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F0R2_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F0R2_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F0R2_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F0R2_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F0R2_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F0R2_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F0R2_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F0R2_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F0R2_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F0R2_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F0R2_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F0R2_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F0R2_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F1R2 register  *******************/
#define  CAN_F1R2_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F1R2_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F1R2_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F1R2_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F1R2_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F1R2_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F1R2_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F1R2_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F1R2_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F1R2_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F1R2_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F1R2_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F1R2_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F1R2_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F1R2_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F1R2_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F1R2_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F1R2_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F1R2_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F1R2_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F1R2_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F1R2_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F1R2_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F1R2_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F1R2_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F1R2_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F1R2_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F1R2_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F1R2_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F1R2_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F1R2_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F1R2_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F2R2 register  *******************/
#define  CAN_F2R2_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F2R2_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F2R2_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F2R2_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F2R2_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F2R2_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F2R2_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F2R2_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F2R2_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F2R2_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F2R2_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F2R2_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F2R2_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F2R2_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F2R2_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F2R2_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F2R2_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F2R2_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F2R2_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F2R2_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F2R2_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F2R2_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F2R2_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F2R2_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F2R2_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F2R2_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F2R2_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F2R2_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F2R2_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F2R2_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F2R2_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F2R2_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F3R2 register  *******************/
#define  CAN_F3R2_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F3R2_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F3R2_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F3R2_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F3R2_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F3R2_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F3R2_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F3R2_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F3R2_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F3R2_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F3R2_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F3R2_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F3R2_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F3R2_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F3R2_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F3R2_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F3R2_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F3R2_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F3R2_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F3R2_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F3R2_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F3R2_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F3R2_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F3R2_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F3R2_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F3R2_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F3R2_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F3R2_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F3R2_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F3R2_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F3R2_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F3R2_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F4R2 register  *******************/
#define  CAN_F4R2_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F4R2_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F4R2_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F4R2_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F4R2_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F4R2_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F4R2_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F4R2_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F4R2_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F4R2_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F4R2_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F4R2_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F4R2_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F4R2_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F4R2_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F4R2_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F4R2_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F4R2_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F4R2_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F4R2_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F4R2_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F4R2_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F4R2_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F4R2_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F4R2_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F4R2_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F4R2_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F4R2_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F4R2_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F4R2_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F4R2_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F4R2_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F5R2 register  *******************/
#define  CAN_F5R2_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F5R2_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F5R2_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F5R2_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F5R2_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F5R2_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F5R2_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F5R2_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F5R2_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F5R2_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F5R2_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F5R2_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F5R2_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F5R2_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F5R2_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F5R2_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F5R2_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F5R2_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F5R2_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F5R2_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F5R2_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F5R2_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F5R2_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F5R2_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F5R2_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F5R2_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F5R2_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F5R2_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F5R2_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F5R2_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F5R2_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F5R2_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F6R2 register  *******************/
#define  CAN_F6R2_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F6R2_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F6R2_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F6R2_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F6R2_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F6R2_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F6R2_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F6R2_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F6R2_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F6R2_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F6R2_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F6R2_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F6R2_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F6R2_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F6R2_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F6R2_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F6R2_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F6R2_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F6R2_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F6R2_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F6R2_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F6R2_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F6R2_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F6R2_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F6R2_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F6R2_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F6R2_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F6R2_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F6R2_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F6R2_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F6R2_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F6R2_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F7R2 register  *******************/
#define  CAN_F7R2_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F7R2_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F7R2_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F7R2_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F7R2_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F7R2_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F7R2_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F7R2_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F7R2_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F7R2_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F7R2_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F7R2_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F7R2_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F7R2_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F7R2_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F7R2_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F7R2_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F7R2_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F7R2_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F7R2_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F7R2_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F7R2_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F7R2_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F7R2_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F7R2_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F7R2_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F7R2_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F7R2_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F7R2_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F7R2_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F7R2_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F7R2_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F8R2 register  *******************/
#define  CAN_F8R2_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F8R2_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F8R2_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F8R2_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F8R2_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F8R2_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F8R2_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F8R2_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F8R2_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F8R2_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F8R2_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F8R2_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F8R2_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F8R2_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F8R2_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F8R2_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F8R2_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F8R2_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F8R2_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F8R2_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F8R2_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F8R2_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F8R2_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F8R2_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F8R2_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F8R2_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F8R2_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F8R2_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F8R2_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F8R2_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F8R2_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F8R2_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F9R2 register  *******************/
#define  CAN_F9R2_FB0                        ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F9R2_FB1                        ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F9R2_FB2                        ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F9R2_FB3                        ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F9R2_FB4                        ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F9R2_FB5                        ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F9R2_FB6                        ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F9R2_FB7                        ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F9R2_FB8                        ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F9R2_FB9                        ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F9R2_FB10                       ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F9R2_FB11                       ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F9R2_FB12                       ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F9R2_FB13                       ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F9R2_FB14                       ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F9R2_FB15                       ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F9R2_FB16                       ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F9R2_FB17                       ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F9R2_FB18                       ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F9R2_FB19                       ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F9R2_FB20                       ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F9R2_FB21                       ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F9R2_FB22                       ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F9R2_FB23                       ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F9R2_FB24                       ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F9R2_FB25                       ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F9R2_FB26                       ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F9R2_FB27                       ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F9R2_FB28                       ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F9R2_FB29                       ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F9R2_FB30                       ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F9R2_FB31                       ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F10R2 register  ******************/
#define  CAN_F10R2_FB0                       ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F10R2_FB1                       ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F10R2_FB2                       ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F10R2_FB3                       ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F10R2_FB4                       ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F10R2_FB5                       ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F10R2_FB6                       ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F10R2_FB7                       ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F10R2_FB8                       ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F10R2_FB9                       ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F10R2_FB10                      ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F10R2_FB11                      ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F10R2_FB12                      ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F10R2_FB13                      ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F10R2_FB14                      ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F10R2_FB15                      ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F10R2_FB16                      ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F10R2_FB17                      ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F10R2_FB18                      ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F10R2_FB19                      ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F10R2_FB20                      ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F10R2_FB21                      ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F10R2_FB22                      ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F10R2_FB23                      ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F10R2_FB24                      ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F10R2_FB25                      ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F10R2_FB26                      ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F10R2_FB27                      ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F10R2_FB28                      ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F10R2_FB29                      ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F10R2_FB30                      ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F10R2_FB31                      ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F11R2 register  ******************/
#define  CAN_F11R2_FB0                       ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F11R2_FB1                       ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F11R2_FB2                       ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F11R2_FB3                       ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F11R2_FB4                       ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F11R2_FB5                       ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F11R2_FB6                       ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F11R2_FB7                       ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F11R2_FB8                       ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F11R2_FB9                       ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F11R2_FB10                      ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F11R2_FB11                      ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F11R2_FB12                      ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F11R2_FB13                      ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F11R2_FB14                      ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F11R2_FB15                      ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F11R2_FB16                      ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F11R2_FB17                      ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F11R2_FB18                      ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F11R2_FB19                      ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F11R2_FB20                      ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F11R2_FB21                      ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F11R2_FB22                      ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F11R2_FB23                      ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F11R2_FB24                      ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F11R2_FB25                      ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F11R2_FB26                      ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F11R2_FB27                      ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F11R2_FB28                      ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F11R2_FB29                      ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F11R2_FB30                      ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F11R2_FB31                      ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F12R2 register  ******************/
#define  CAN_F12R2_FB0                       ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F12R2_FB1                       ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F12R2_FB2                       ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F12R2_FB3                       ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F12R2_FB4                       ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F12R2_FB5                       ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F12R2_FB6                       ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F12R2_FB7                       ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F12R2_FB8                       ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F12R2_FB9                       ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F12R2_FB10                      ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F12R2_FB11                      ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F12R2_FB12                      ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F12R2_FB13                      ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F12R2_FB14                      ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F12R2_FB15                      ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F12R2_FB16                      ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F12R2_FB17                      ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F12R2_FB18                      ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F12R2_FB19                      ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F12R2_FB20                      ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F12R2_FB21                      ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F12R2_FB22                      ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F12R2_FB23                      ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F12R2_FB24                      ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F12R2_FB25                      ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F12R2_FB26                      ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F12R2_FB27                      ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F12R2_FB28                      ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F12R2_FB29                      ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F12R2_FB30                      ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F12R2_FB31                      ((uint32_t)0x80000000)        /* Filter bit 31 */

/*******************  Bit definition for CAN_F13R2 register  ******************/
#define  CAN_F13R2_FB0                       ((uint32_t)0x00000001)        /* Filter bit 0 */
#define  CAN_F13R2_FB1                       ((uint32_t)0x00000002)        /* Filter bit 1 */
#define  CAN_F13R2_FB2                       ((uint32_t)0x00000004)        /* Filter bit 2 */
#define  CAN_F13R2_FB3                       ((uint32_t)0x00000008)        /* Filter bit 3 */
#define  CAN_F13R2_FB4                       ((uint32_t)0x00000010)        /* Filter bit 4 */
#define  CAN_F13R2_FB5                       ((uint32_t)0x00000020)        /* Filter bit 5 */
#define  CAN_F13R2_FB6                       ((uint32_t)0x00000040)        /* Filter bit 6 */
#define  CAN_F13R2_FB7                       ((uint32_t)0x00000080)        /* Filter bit 7 */
#define  CAN_F13R2_FB8                       ((uint32_t)0x00000100)        /* Filter bit 8 */
#define  CAN_F13R2_FB9                       ((uint32_t)0x00000200)        /* Filter bit 9 */
#define  CAN_F13R2_FB10                      ((uint32_t)0x00000400)        /* Filter bit 10 */
#define  CAN_F13R2_FB11                      ((uint32_t)0x00000800)        /* Filter bit 11 */
#define  CAN_F13R2_FB12                      ((uint32_t)0x00001000)        /* Filter bit 12 */
#define  CAN_F13R2_FB13                      ((uint32_t)0x00002000)        /* Filter bit 13 */
#define  CAN_F13R2_FB14                      ((uint32_t)0x00004000)        /* Filter bit 14 */
#define  CAN_F13R2_FB15                      ((uint32_t)0x00008000)        /* Filter bit 15 */
#define  CAN_F13R2_FB16                      ((uint32_t)0x00010000)        /* Filter bit 16 */
#define  CAN_F13R2_FB17                      ((uint32_t)0x00020000)        /* Filter bit 17 */
#define  CAN_F13R2_FB18                      ((uint32_t)0x00040000)        /* Filter bit 18 */
#define  CAN_F13R2_FB19                      ((uint32_t)0x00080000)        /* Filter bit 19 */
#define  CAN_F13R2_FB20                      ((uint32_t)0x00100000)        /* Filter bit 20 */
#define  CAN_F13R2_FB21                      ((uint32_t)0x00200000)        /* Filter bit 21 */
#define  CAN_F13R2_FB22                      ((uint32_t)0x00400000)        /* Filter bit 22 */
#define  CAN_F13R2_FB23                      ((uint32_t)0x00800000)        /* Filter bit 23 */
#define  CAN_F13R2_FB24                      ((uint32_t)0x01000000)        /* Filter bit 24 */
#define  CAN_F13R2_FB25                      ((uint32_t)0x02000000)        /* Filter bit 25 */
#define  CAN_F13R2_FB26                      ((uint32_t)0x04000000)        /* Filter bit 26 */
#define  CAN_F13R2_FB27                      ((uint32_t)0x08000000)        /* Filter bit 27 */
#define  CAN_F13R2_FB28                      ((uint32_t)0x10000000)        /* Filter bit 28 */
#define  CAN_F13R2_FB29                      ((uint32_t)0x20000000)        /* Filter bit 29 */
#define  CAN_F13R2_FB30                      ((uint32_t)0x40000000)        /* Filter bit 30 */
#define  CAN_F13R2_FB31                      ((uint32_t)0x80000000)        /* Filter bit 31 */



/******************************************************************************/
/*                          CRC Calculation Unit                              */
/******************************************************************************/

/*******************  Bit definition for CRC_DATAR register  *********************/
#define  CRC_DATAR_DR                           ((uint32_t)0xFFFFFFFF) /* Data register bits */


/*******************  Bit definition for CRC_IDATAR register  ********************/
#define  CRC_IDR_IDATAR                         ((uint8_t)0xFF)        /* General-purpose 8-bit data register bits */


/********************  Bit definition for CRC_CTLR register  ********************/
#define  CRC_CTLR_RESET                         ((uint8_t)0x01)        /* RESET bit */

/******************************************************************************/
/*                      Digital to Analog Converter                           */
/******************************************************************************/

/********************  Bit definition for DAC_CTLR register  ********************/
#define  DAC_EN1                          ((uint32_t)0x00000001)        /* DAC channel1 enable */
#define  DAC_BOFF1                        ((uint32_t)0x00000002)        /* DAC channel1 output buffer disable */
#define  DAC_TEN1                         ((uint32_t)0x00000004)        /* DAC channel1 Trigger enable */

#define  DAC_TSEL1                        ((uint32_t)0x00000038)        /* TSEL1[2:0] (DAC channel1 Trigger selection) */
#define  DAC_TSEL1_0                      ((uint32_t)0x00000008)        /* Bit 0 */
#define  DAC_TSEL1_1                      ((uint32_t)0x00000010)        /* Bit 1 */
#define  DAC_TSEL1_2                      ((uint32_t)0x00000020)        /* Bit 2 */

#define  DAC_WAVE1                        ((uint32_t)0x000000C0)        /* WAVE1[1:0] (DAC channel1 noise/triangle wave generation enable) */
#define  DAC_WAVE1_0                      ((uint32_t)0x00000040)        /* Bit 0 */
#define  DAC_WAVE1_1                      ((uint32_t)0x00000080)        /* Bit 1 */

#define  DAC_MAMP1                        ((uint32_t)0x00000F00)        /* MAMP1[3:0] (DAC channel1 Mask/Amplitude selector) */
#define  DAC_MAMP1_0                      ((uint32_t)0x00000100)        /* Bit 0 */
#define  DAC_MAMP1_1                      ((uint32_t)0x00000200)        /* Bit 1 */
#define  DAC_MAMP1_2                      ((uint32_t)0x00000400)        /* Bit 2 */
#define  DAC_MAMP1_3                      ((uint32_t)0x00000800)        /* Bit 3 */

#define  DAC_DMAEN1                       ((uint32_t)0x00001000)        /* DAC channel1 DMA enable */
#define  DAC_EN2                          ((uint32_t)0x00010000)        /* DAC channel2 enable */
#define  DAC_BOFF2                        ((uint32_t)0x00020000)        /* DAC channel2 output buffer disable */
#define  DAC_TEN2                         ((uint32_t)0x00040000)        /* DAC channel2 Trigger enable */

#define  DAC_TSEL2                        ((uint32_t)0x00380000)        /* TSEL2[2:0] (DAC channel2 Trigger selection) */
#define  DAC_TSEL2_0                      ((uint32_t)0x00080000)        /* Bit 0 */
#define  DAC_TSEL2_1                      ((uint32_t)0x00100000)        /* Bit 1 */
#define  DAC_TSEL2_2                      ((uint32_t)0x00200000)        /* Bit 2 */

#define  DAC_WAVE2                        ((uint32_t)0x00C00000)        /* WAVE2[1:0] (DAC channel2 noise/triangle wave generation enable) */
#define  DAC_WAVE2_0                      ((uint32_t)0x00400000)        /* Bit 0 */
#define  DAC_WAVE2_1                      ((uint32_t)0x00800000)        /* Bit 1 */

#define  DAC_MAMP2                        ((uint32_t)0x0F000000)        /* MAMP2[3:0] (DAC channel2 Mask/Amplitude selector) */
#define  DAC_MAMP2_0                      ((uint32_t)0x01000000)        /* Bit 0 */
#define  DAC_MAMP2_1                      ((uint32_t)0x02000000)        /* Bit 1 */
#define  DAC_MAMP2_2                      ((uint32_t)0x04000000)        /* Bit 2 */
#define  DAC_MAMP2_3                      ((uint32_t)0x08000000)        /* Bit 3 */

#define  DAC_DMAEN2                       ((uint32_t)0x10000000)        /* DAC channel2 DMA enabled */

/*****************  Bit definition for DAC_SWTR register  ******************/
#define  DAC_SWTRIG1                      ((uint8_t)0x01)               /* DAC channel1 software trigger */
#define  DAC_SWTRIG2                      ((uint8_t)0x02)               /* DAC channel2 software trigger */

/*****************  Bit definition for DAC_R12BDHR1 register  ******************/
#define  DAC_DHR12R1                      ((uint16_t)0x0FFF)            /* DAC channel1 12-bit Right aligned data */

/*****************  Bit definition for DAC_L12BDHR1 register  ******************/
#define  DAC_DHR12L1                      ((uint16_t)0xFFF0)            /* DAC channel1 12-bit Left aligned data */

/******************  Bit definition for DAC_R8BDHR1 register  ******************/
#define  DAC_DHR8R1                       ((uint8_t)0xFF)               /* DAC channel1 8-bit Right aligned data */

/*****************  Bit definition for DAC_R12BDHR2 register  ******************/
#define  DAC_DHR12R2                      ((uint16_t)0x0FFF)            /* DAC channel2 12-bit Right aligned data */

/*****************  Bit definition for DAC_L12BDHR2 register  ******************/
#define  DAC_DHR12L2                      ((uint16_t)0xFFF0)            /* DAC channel2 12-bit Left aligned data */

/******************  Bit definition for DAC_R8BDHR2 register  ******************/
#define  DAC_DHR8R2                       ((uint8_t)0xFF)               /* DAC channel2 8-bit Right aligned data */

/*****************  Bit definition for DAC_RD12BDHR register  ******************/
#define  DAC_RD12BDHR_DACC1DHR            ((uint32_t)0x00000FFF)        /* DAC channel1 12-bit Right aligned data */
#define  DAC_RD12BDHR_DACC2DHR            ((uint32_t)0x0FFF0000)        /* DAC channel2 12-bit Right aligned data */

/*****************  Bit definition for DAC_LD12BDHR register  ******************/
#define  DAC_LD12BDHR_DACC1DHR            ((uint32_t)0x0000FFF0)        /* DAC channel1 12-bit Left aligned data */
#define  DAC_LD12BDHR_DACC2DHR            ((uint32_t)0xFFF00000)        /* DAC channel2 12-bit Left aligned data */

/******************  Bit definition for DAC_RD8BDHR register  ******************/
#define  DAC_RD8BDHR_DACC1DHR             ((uint16_t)0x00FF)            /* DAC channel1 8-bit Right aligned data */
#define  DAC_RD8BDHR_DACC2DHR             ((uint16_t)0xFF00)            /* DAC channel2 8-bit Right aligned data */

/*******************  Bit definition for DAC_DOR1 register  *******************/
#define  DAC_DACC1DOR                     ((uint16_t)0x0FFF)            /* DAC channel1 data output */

/*******************  Bit definition for DAC_DOR2 register  *******************/
#define  DAC_DACC2DOR                     ((uint16_t)0x0FFF)            /* DAC channel2 data output */

/******************************************************************************/
/*                             DMA Controller                                 */
/******************************************************************************/

/*******************  Bit definition for DMA_INTFR register  ********************/
#define  DMA_GIF1                           ((uint32_t)0x00000001)        /* Channel 1 Global interrupt flag */
#define  DMA_TCIF1                          ((uint32_t)0x00000002)        /* Channel 1 Transfer Complete flag */
#define  DMA_HTIF1                          ((uint32_t)0x00000004)        /* Channel 1 Half Transfer flag */
#define  DMA_TEIF1                          ((uint32_t)0x00000008)        /* Channel 1 Transfer Error flag */
#define  DMA_GIF2                           ((uint32_t)0x00000010)        /* Channel 2 Global interrupt flag */
#define  DMA_TCIF2                          ((uint32_t)0x00000020)        /* Channel 2 Transfer Complete flag */
#define  DMA_HTIF2                          ((uint32_t)0x00000040)        /* Channel 2 Half Transfer flag */
#define  DMA_TEIF2                          ((uint32_t)0x00000080)        /* Channel 2 Transfer Error flag */
#define  DMA_GIF3                           ((uint32_t)0x00000100)        /* Channel 3 Global interrupt flag */
#define  DMA_TCIF3                          ((uint32_t)0x00000200)        /* Channel 3 Transfer Complete flag */
#define  DMA_HTIF3                          ((uint32_t)0x00000400)        /* Channel 3 Half Transfer flag */
#define  DMA_TEIF3                          ((uint32_t)0x00000800)        /* Channel 3 Transfer Error flag */
#define  DMA_GIF4                           ((uint32_t)0x00001000)        /* Channel 4 Global interrupt flag */
#define  DMA_TCIF4                          ((uint32_t)0x00002000)        /* Channel 4 Transfer Complete flag */
#define  DMA_HTIF4                          ((uint32_t)0x00004000)        /* Channel 4 Half Transfer flag */
#define  DMA_TEIF4                          ((uint32_t)0x00008000)        /* Channel 4 Transfer Error flag */
#define  DMA_GIF5                           ((uint32_t)0x00010000)        /* Channel 5 Global interrupt flag */
#define  DMA_TCIF5                          ((uint32_t)0x00020000)        /* Channel 5 Transfer Complete flag */
#define  DMA_HTIF5                          ((uint32_t)0x00040000)        /* Channel 5 Half Transfer flag */
#define  DMA_TEIF5                          ((uint32_t)0x00080000)        /* Channel 5 Transfer Error flag */
#define  DMA_GIF6                           ((uint32_t)0x00100000)        /* Channel 6 Global interrupt flag */
#define  DMA_TCIF6                          ((uint32_t)0x00200000)        /* Channel 6 Transfer Complete flag */
#define  DMA_HTIF6                          ((uint32_t)0x00400000)        /* Channel 6 Half Transfer flag */
#define  DMA_TEIF6                          ((uint32_t)0x00800000)        /* Channel 6 Transfer Error flag */
#define  DMA_GIF7                           ((uint32_t)0x01000000)        /* Channel 7 Global interrupt flag */
#define  DMA_TCIF7                          ((uint32_t)0x02000000)        /* Channel 7 Transfer Complete flag */
#define  DMA_HTIF7                          ((uint32_t)0x04000000)        /* Channel 7 Half Transfer flag */
#define  DMA_TEIF7                          ((uint32_t)0x08000000)        /* Channel 7 Transfer Error flag */

#define  DMA_GIF8                           ((uint32_t)0x00000001)        /* Channel 8 Global interrupt flag */
#define  DMA_TCIF8                          ((uint32_t)0x00000002)        /* Channel 8 Transfer Complete flag */
#define  DMA_HTIF8                          ((uint32_t)0x00000004)        /* Channel 8 Half Transfer flag */
#define  DMA_TEIF8                          ((uint32_t)0x00000008)        /* Channel 8 Transfer Error flag */
#define  DMA_GIF9                           ((uint32_t)0x00000010)        /* Channel 9 Global interrupt flag */
#define  DMA_TCIF9                          ((uint32_t)0x00000020)        /* Channel 9 Transfer Complete flag */
#define  DMA_HTIF9                          ((uint32_t)0x00000040)        /* Channel 9 Half Transfer flag */
#define  DMA_TEIF9                          ((uint32_t)0x00000080)        /* Channel 9 Transfer Error flag */
#define  DMA_GIF10                          ((uint32_t)0x00000100)        /* Channel 10 Global interrupt flag */
#define  DMA_TCIF10                         ((uint32_t)0x00000200)        /* Channel 10 Transfer Complete flag */
#define  DMA_HTIF10                         ((uint32_t)0x00000400)        /* Channel 10 Half Transfer flag */
#define  DMA_TEIF10                         ((uint32_t)0x00000800)        /* Channel 10 Transfer Error flag */
#define  DMA_GIF11                          ((uint32_t)0x00001000)        /* Channel 11 Global interrupt flag */
#define  DMA_TCIF11                         ((uint32_t)0x00002000)        /* Channel 11 Transfer Complete flag */
#define  DMA_HTIF11                         ((uint32_t)0x00004000)        /* Channel 11 Half Transfer flag */
#define  DMA_TEIF11                         ((uint32_t)0x00008000)        /* Channel 11 Transfer Error flag */

/*******************  Bit definition for DMA_INTFCR register  *******************/
#define  DMA_CGIF1                          ((uint32_t)0x00000001)        /* Channel 1 Global interrupt clear */
#define  DMA_CTCIF1                         ((uint32_t)0x00000002)        /* Channel 1 Transfer Complete clear */
#define  DMA_CHTIF1                         ((uint32_t)0x00000004)        /* Channel 1 Half Transfer clear */
#define  DMA_CTEIF1                         ((uint32_t)0x00000008)        /* Channel 1 Transfer Error clear */
#define  DMA_CGIF2                          ((uint32_t)0x00000010)        /* Channel 2 Global interrupt clear */
#define  DMA_CTCIF2                         ((uint32_t)0x00000020)        /* Channel 2 Transfer Complete clear */
#define  DMA_CHTIF2                         ((uint32_t)0x00000040)        /* Channel 2 Half Transfer clear */
#define  DMA_CTEIF2                         ((uint32_t)0x00000080)        /* Channel 2 Transfer Error clear */
#define  DMA_CGIF3                          ((uint32_t)0x00000100)        /* Channel 3 Global interrupt clear */
#define  DMA_CTCIF3                         ((uint32_t)0x00000200)        /* Channel 3 Transfer Complete clear */
#define  DMA_CHTIF3                         ((uint32_t)0x00000400)        /* Channel 3 Half Transfer clear */
#define  DMA_CTEIF3                         ((uint32_t)0x00000800)        /* Channel 3 Transfer Error clear */
#define  DMA_CGIF4                          ((uint32_t)0x00001000)        /* Channel 4 Global interrupt clear */
#define  DMA_CTCIF4                         ((uint32_t)0x00002000)        /* Channel 4 Transfer Complete clear */
#define  DMA_CHTIF4                         ((uint32_t)0x00004000)        /* Channel 4 Half Transfer clear */
#define  DMA_CTEIF4                         ((uint32_t)0x00008000)        /* Channel 4 Transfer Error clear */
#define  DMA_CGIF5                          ((uint32_t)0x00010000)        /* Channel 5 Global interrupt clear */
#define  DMA_CTCIF5                         ((uint32_t)0x00020000)        /* Channel 5 Transfer Complete clear */
#define  DMA_CHTIF5                         ((uint32_t)0x00040000)        /* Channel 5 Half Transfer clear */
#define  DMA_CTEIF5                         ((uint32_t)0x00080000)        /* Channel 5 Transfer Error clear */
#define  DMA_CGIF6                          ((uint32_t)0x00100000)        /* Channel 6 Global interrupt clear */
#define  DMA_CTCIF6                         ((uint32_t)0x00200000)        /* Channel 6 Transfer Complete clear */
#define  DMA_CHTIF6                         ((uint32_t)0x00400000)        /* Channel 6 Half Transfer clear */
#define  DMA_CTEIF6                         ((uint32_t)0x00800000)        /* Channel 6 Transfer Error clear */
#define  DMA_CGIF7                          ((uint32_t)0x01000000)        /* Channel 7 Global interrupt clear */
#define  DMA_CTCIF7                         ((uint32_t)0x02000000)        /* Channel 7 Transfer Complete clear */
#define  DMA_CHTIF7                         ((uint32_t)0x04000000)        /* Channel 7 Half Transfer clear */
#define  DMA_CTEIF7                         ((uint32_t)0x08000000)        /* Channel 7 Transfer Error clear */

/*******************  Bit definition for DMA_CFGR1 register  *******************/
#define  DMA_CFGR1_EN                       ((uint16_t)0x0001)            /* Channel enable*/
#define  DMA_CFGR1_TCIE                     ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFGR1_HTIE                     ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFGR1_TEIE                     ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFGR1_DIR                      ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFGR1_CIRC                     ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFGR1_PINC                     ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFGR1_MINC                     ((uint16_t)0x0080)            /* Memory increment mode */

#define  DMA_CFGR1_PSIZE                    ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFGR1_PSIZE_0                  ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFGR1_PSIZE_1                  ((uint16_t)0x0200)            /* Bit 1 */

#define  DMA_CFGR1_MSIZE                    ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFGR1_MSIZE_0                  ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFGR1_MSIZE_1                  ((uint16_t)0x0800)            /* Bit 1 */

#define  DMA_CFGR1_PL                       ((uint16_t)0x3000)            /* PL[1:0] bits(Channel Priority level) */
#define  DMA_CFGR1_PL_0                     ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFGR1_PL_1                     ((uint16_t)0x2000)            /* Bit 1 */

#define  DMA_CFGR1_MEM2MEM                  ((uint16_t)0x4000)            /* Memory to memory mode */

/*******************  Bit definition for DMA_CFGR2 register  *******************/
#define  DMA_CFGR2_EN                       ((uint16_t)0x0001)            /* Channel enable */
#define  DMA_CFGR2_TCIE                     ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFGR2_HTIE                     ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFGR2_TEIE                     ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFGR2_DIR                      ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFGR2_CIRC                     ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFGR2_PINC                     ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFGR2_MINC                     ((uint16_t)0x0080)            /* Memory increment mode */

#define  DMA_CFGR2_PSIZE                    ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFGR2_PSIZE_0                  ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFGR2_PSIZE_1                  ((uint16_t)0x0200)            /* Bit 1 */

#define  DMA_CFGR2_MSIZE                    ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFGR2_MSIZE_0                  ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFGR2_MSIZE_1                  ((uint16_t)0x0800)            /* Bit 1 */

#define  DMA_CFGR2_PL                       ((uint16_t)0x3000)            /* PL[1:0] bits (Channel Priority level) */
#define  DMA_CFGR2_PL_0                     ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFGR2_PL_1                     ((uint16_t)0x2000)            /* Bit 1 */

#define  DMA_CFGR2_MEM2MEM                  ((uint16_t)0x4000)            /* Memory to memory mode */

/*******************  Bit definition for DMA_CFGR3 register  *******************/
#define  DMA_CFGR3_EN                       ((uint16_t)0x0001)            /* Channel enable */
#define  DMA_CFGR3_TCIE                     ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFGR3_HTIE                     ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFGR3_TEIE                     ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFGR3_DIR                      ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFGR3_CIRC                     ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFGR3_PINC                     ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFGR3_MINC                     ((uint16_t)0x0080)            /* Memory increment mode */

#define  DMA_CFGR3_PSIZE                    ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFGR3_PSIZE_0                  ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFGR3_PSIZE_1                  ((uint16_t)0x0200)            /* Bit 1 */

#define  DMA_CFGR3_MSIZE                    ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFGR3_MSIZE_0                  ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFGR3_MSIZE_1                  ((uint16_t)0x0800)            /* Bit 1 */

#define  DMA_CFGR3_PL                       ((uint16_t)0x3000)            /* PL[1:0] bits (Channel Priority level) */
#define  DMA_CFGR3_PL_0                     ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFGR3_PL_1                     ((uint16_t)0x2000)            /* Bit 1 */

#define  DMA_CFGR3_MEM2MEM                  ((uint16_t)0x4000)            /* Memory to memory mode */

/*******************  Bit definition for DMA_CFG4 register  *******************/
#define  DMA_CFG4_EN                        ((uint16_t)0x0001)            /* Channel enable */
#define  DMA_CFG4_TCIE                      ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFG4_HTIE                      ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFG4_TEIE                      ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFG4_DIR                       ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFG4_CIRC                      ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFG4_PINC                      ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFG4_MINC                      ((uint16_t)0x0080)            /* Memory increment mode */

#define  DMA_CFG4_PSIZE                     ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFG4_PSIZE_0                   ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFG4_PSIZE_1                   ((uint16_t)0x0200)            /* Bit 1 */
             
#define  DMA_CFG4_MSIZE                     ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFG4_MSIZE_0                   ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFG4_MSIZE_1                   ((uint16_t)0x0800)            /* Bit 1 */
             
#define  DMA_CFG4_PL                        ((uint16_t)0x3000)            /* PL[1:0] bits (Channel Priority level) */
#define  DMA_CFG4_PL_0                      ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFG4_PL_1                      ((uint16_t)0x2000)            /* Bit 1 */
             
#define  DMA_CFG4_MEM2MEM                   ((uint16_t)0x4000)            /* Memory to memory mode */

/******************  Bit definition for DMA_CFG5 register  *******************/
#define  DMA_CFG5_EN                        ((uint16_t)0x0001)            /* Channel enable */
#define  DMA_CFG5_TCIE                      ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFG5_HTIE                      ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFG5_TEIE                      ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFG5_DIR                       ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFG5_CIRC                      ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFG5_PINC                      ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFG5_MINC                      ((uint16_t)0x0080)            /* Memory increment mode */
             
#define  DMA_CFG5_PSIZE                     ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFG5_PSIZE_0                   ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFG5_PSIZE_1                   ((uint16_t)0x0200)            /* Bit 1 */
             
#define  DMA_CFG5_MSIZE                     ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFG5_MSIZE_0                   ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFG5_MSIZE_1                   ((uint16_t)0x0800)            /* Bit 1 */
             
#define  DMA_CFG5_PL                        ((uint16_t)0x3000)            /* PL[1:0] bits (Channel Priority level) */
#define  DMA_CFG5_PL_0                      ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFG5_PL_1                      ((uint16_t)0x2000)            /* Bit 1 */
             
#define  DMA_CFG5_MEM2MEM                   ((uint16_t)0x4000)            /* Memory to memory mode enable */

/*******************  Bit definition for DMA_CFG6 register  *******************/
#define  DMA_CFG6_EN                        ((uint16_t)0x0001)            /* Channel enable */
#define  DMA_CFG6_TCIE                      ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFG6_HTIE                      ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFG6_TEIE                      ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFG6_DIR                       ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFG6_CIRC                      ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFG6_PINC                      ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFG6_MINC                      ((uint16_t)0x0080)            /* Memory increment mode */
             
#define  DMA_CFG6_PSIZE                     ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFG6_PSIZE_0                   ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFG6_PSIZE_1                   ((uint16_t)0x0200)            /* Bit 1 */
             
#define  DMA_CFG6_MSIZE                     ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFG6_MSIZE_0                   ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFG6_MSIZE_1                   ((uint16_t)0x0800)            /* Bit 1 */
             
#define  DMA_CFG6_PL                        ((uint16_t)0x3000)            /* PL[1:0] bits (Channel Priority level) */
#define  DMA_CFG6_PL_0                      ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFG6_PL_1                      ((uint16_t)0x2000)            /* Bit 1 */
             
#define  DMA_CFG6_MEM2MEM                   ((uint16_t)0x4000)            /* Memory to memory mode */

/*******************  Bit definition for DMA_CFG7 register  *******************/
#define  DMA_CFG7_EN                        ((uint16_t)0x0001)            /* Channel enable */
#define  DMA_CFG7_TCIE                      ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFG7_HTIE                      ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFG7_TEIE                      ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFG7_DIR                       ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFG7_CIRC                      ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFG7_PINC                      ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFG7_MINC                      ((uint16_t)0x0080)            /* Memory increment mode */
             
#define  DMA_CFG7_PSIZE                     ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFG7_PSIZE_0                   ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFG7_PSIZE_1                   ((uint16_t)0x0200)            /* Bit 1 */
             
#define  DMA_CFG7_MSIZE                     ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFG7_MSIZE_0                   ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFG7_MSIZE_1                   ((uint16_t)0x0800)            /* Bit 1 */
             
#define  DMA_CFG7_PL                        ((uint16_t)0x3000)            /* PL[1:0] bits (Channel Priority level) */
#define  DMA_CFG7_PL_0                      ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFG7_PL_1                      ((uint16_t)0x2000)            /* Bit 1 */
             
#define  DMA_CFG7_MEM2MEM                   ((uint16_t)0x4000)            /* Memory to memory mode enable */

/******************  Bit definition for DMA_CNTR1 register  ******************/
#define  DMA_CNTR1_NDT                      ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR2 register  ******************/
#define  DMA_CNTR2_NDT                      ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR3 register  ******************/
#define  DMA_CNTR3_NDT                      ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR4 register  ******************/
#define  DMA_CNTR4_NDT                      ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR5 register  ******************/
#define  DMA_CNTR5_NDT                      ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR6 register  ******************/
#define  DMA_CNTR6_NDT                      ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR7 register  ******************/
#define  DMA_CNTR7_NDT                      ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_PADDR1 register  *******************/
#define  DMA_PADDR1_PA                      ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_PADDR2 register  *******************/
#define  DMA_PADDR2_PA                      ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_PADDR3 register  *******************/
#define  DMA_PADDR3_PA                      ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_PADDR4 register  *******************/
#define  DMA_PADDR4_PA                      ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_PADDR5 register  *******************/
#define  DMA_PADDR5_PA                      ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_PADDR6 register  *******************/
#define  DMA_PADDR6_PA                      ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_PADDR7 register  *******************/
#define  DMA_PADDR7_PA                      ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_MADDR1 register  *******************/
#define  DMA_MADDR1_MA                      ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_MADDR2 register  *******************/
#define  DMA_MADDR2_MA                      ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_MADDR3 register  *******************/
#define  DMA_MADDR3_MA                      ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_MADDR4 register  *******************/
#define  DMA_MADDR4_MA                      ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_MADDR5 register  *******************/
#define  DMA_MADDR5_MA                      ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_MADDR6 register  *******************/
#define  DMA_MADDR6_MA                      ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_MADDR7 register  *******************/
#define  DMA_MADDR7_MA                      ((uint32_t)0xFFFFFFFF)        /* Memory Address */


/******************************************************************************/
/*                    External Interrupt/Event Controller                     */
/******************************************************************************/

/*******************  Bit definition for EXTI_INTENR register  *******************/
#define  EXTI_INTENR_MR0                        ((uint32_t)0x00000001)        /* Interrupt Mask on line 0 */
#define  EXTI_INTENR_MR1                        ((uint32_t)0x00000002)        /* Interrupt Mask on line 1 */
#define  EXTI_INTENR_MR2                        ((uint32_t)0x00000004)        /* Interrupt Mask on line 2 */
#define  EXTI_INTENR_MR3                        ((uint32_t)0x00000008)        /* Interrupt Mask on line 3 */
#define  EXTI_INTENR_MR4                        ((uint32_t)0x00000010)        /* Interrupt Mask on line 4 */
#define  EXTI_INTENR_MR5                        ((uint32_t)0x00000020)        /* Interrupt Mask on line 5 */
#define  EXTI_INTENR_MR6                        ((uint32_t)0x00000040)        /* Interrupt Mask on line 6 */
#define  EXTI_INTENR_MR7                        ((uint32_t)0x00000080)        /* Interrupt Mask on line 7 */
#define  EXTI_INTENR_MR8                        ((uint32_t)0x00000100)        /* Interrupt Mask on line 8 */
#define  EXTI_INTENR_MR9                        ((uint32_t)0x00000200)        /* Interrupt Mask on line 9 */
#define  EXTI_INTENR_MR10                       ((uint32_t)0x00000400)        /* Interrupt Mask on line 10 */
#define  EXTI_INTENR_MR11                       ((uint32_t)0x00000800)        /* Interrupt Mask on line 11 */
#define  EXTI_INTENR_MR12                       ((uint32_t)0x00001000)        /* Interrupt Mask on line 12 */
#define  EXTI_INTENR_MR13                       ((uint32_t)0x00002000)        /* Interrupt Mask on line 13 */
#define  EXTI_INTENR_MR14                       ((uint32_t)0x00004000)        /* Interrupt Mask on line 14 */
#define  EXTI_INTENR_MR15                       ((uint32_t)0x00008000)        /* Interrupt Mask on line 15 */
#define  EXTI_INTENR_MR16                       ((uint32_t)0x00010000)        /* Interrupt Mask on line 16 */
#define  EXTI_INTENR_MR17                       ((uint32_t)0x00020000)        /* Interrupt Mask on line 17 */
#define  EXTI_INTENR_MR18                       ((uint32_t)0x00040000)        /* Interrupt Mask on line 18 */
#define  EXTI_INTENR_MR19                       ((uint32_t)0x00080000)        /* Interrupt Mask on line 19 */

/*******************  Bit definition for EXTI_EVENR register  *******************/
#define  EXTI_EVENR_MR0                         ((uint32_t)0x00000001)        /* Event Mask on line 0 */
#define  EXTI_EVENR_MR1                         ((uint32_t)0x00000002)        /* Event Mask on line 1 */
#define  EXTI_EVENR_MR2                         ((uint32_t)0x00000004)        /* Event Mask on line 2 */
#define  EXTI_EVENR_MR3                         ((uint32_t)0x00000008)        /* Event Mask on line 3 */
#define  EXTI_EVENR_MR4                         ((uint32_t)0x00000010)        /* Event Mask on line 4 */
#define  EXTI_EVENR_MR5                         ((uint32_t)0x00000020)        /* Event Mask on line 5 */
#define  EXTI_EVENR_MR6                         ((uint32_t)0x00000040)        /* Event Mask on line 6 */
#define  EXTI_EVENR_MR7                         ((uint32_t)0x00000080)        /* Event Mask on line 7 */
#define  EXTI_EVENR_MR8                         ((uint32_t)0x00000100)        /* Event Mask on line 8 */
#define  EXTI_EVENR_MR9                         ((uint32_t)0x00000200)        /* Event Mask on line 9 */
#define  EXTI_EVENR_MR10                        ((uint32_t)0x00000400)        /* Event Mask on line 10 */
#define  EXTI_EVENR_MR11                        ((uint32_t)0x00000800)        /* Event Mask on line 11 */
#define  EXTI_EVENR_MR12                        ((uint32_t)0x00001000)        /* Event Mask on line 12 */
#define  EXTI_EVENR_MR13                        ((uint32_t)0x00002000)        /* Event Mask on line 13 */
#define  EXTI_EVENR_MR14                        ((uint32_t)0x00004000)        /* Event Mask on line 14 */
#define  EXTI_EVENR_MR15                        ((uint32_t)0x00008000)        /* Event Mask on line 15 */
#define  EXTI_EVENR_MR16                        ((uint32_t)0x00010000)        /* Event Mask on line 16 */
#define  EXTI_EVENR_MR17                        ((uint32_t)0x00020000)        /* Event Mask on line 17 */
#define  EXTI_EVENR_MR18                        ((uint32_t)0x00040000)        /* Event Mask on line 18 */
#define  EXTI_EVENR_MR19                        ((uint32_t)0x00080000)        /* Event Mask on line 19 */

/******************  Bit definition for EXTI_RTENR register  *******************/
#define  EXTI_RTENR_TR0                         ((uint32_t)0x00000001)        /* Rising trigger event configuration bit of line 0 */
#define  EXTI_RTENR_TR1                         ((uint32_t)0x00000002)        /* Rising trigger event configuration bit of line 1 */
#define  EXTI_RTENR_TR2                         ((uint32_t)0x00000004)        /* Rising trigger event configuration bit of line 2 */
#define  EXTI_RTENR_TR3                         ((uint32_t)0x00000008)        /* Rising trigger event configuration bit of line 3 */
#define  EXTI_RTENR_TR4                         ((uint32_t)0x00000010)        /* Rising trigger event configuration bit of line 4 */
#define  EXTI_RTENR_TR5                         ((uint32_t)0x00000020)        /* Rising trigger event configuration bit of line 5 */
#define  EXTI_RTENR_TR6                         ((uint32_t)0x00000040)        /* Rising trigger event configuration bit of line 6 */
#define  EXTI_RTENR_TR7                         ((uint32_t)0x00000080)        /* Rising trigger event configuration bit of line 7 */
#define  EXTI_RTENR_TR8                         ((uint32_t)0x00000100)        /* Rising trigger event configuration bit of line 8 */
#define  EXTI_RTENR_TR9                         ((uint32_t)0x00000200)        /* Rising trigger event configuration bit of line 9 */
#define  EXTI_RTENR_TR10                        ((uint32_t)0x00000400)        /* Rising trigger event configuration bit of line 10 */
#define  EXTI_RTENR_TR11                        ((uint32_t)0x00000800)        /* Rising trigger event configuration bit of line 11 */
#define  EXTI_RTENR_TR12                        ((uint32_t)0x00001000)        /* Rising trigger event configuration bit of line 12 */
#define  EXTI_RTENR_TR13                        ((uint32_t)0x00002000)        /* Rising trigger event configuration bit of line 13 */
#define  EXTI_RTENR_TR14                        ((uint32_t)0x00004000)        /* Rising trigger event configuration bit of line 14 */
#define  EXTI_RTENR_TR15                        ((uint32_t)0x00008000)        /* Rising trigger event configuration bit of line 15 */
#define  EXTI_RTENR_TR16                        ((uint32_t)0x00010000)        /* Rising trigger event configuration bit of line 16 */
#define  EXTI_RTENR_TR17                        ((uint32_t)0x00020000)        /* Rising trigger event configuration bit of line 17 */
#define  EXTI_RTENR_TR18                        ((uint32_t)0x00040000)        /* Rising trigger event configuration bit of line 18 */
#define  EXTI_RTENR_TR19                        ((uint32_t)0x00080000)        /* Rising trigger event configuration bit of line 19 */

/******************  Bit definition for EXTI_FTENR register  *******************/
#define  EXTI_FTENR_TR0                         ((uint32_t)0x00000001)        /* Falling trigger event configuration bit of line 0 */
#define  EXTI_FTENR_TR1                         ((uint32_t)0x00000002)        /* Falling trigger event configuration bit of line 1 */
#define  EXTI_FTENR_TR2                         ((uint32_t)0x00000004)        /* Falling trigger event configuration bit of line 2 */
#define  EXTI_FTENR_TR3                         ((uint32_t)0x00000008)        /* Falling trigger event configuration bit of line 3 */
#define  EXTI_FTENR_TR4                         ((uint32_t)0x00000010)        /* Falling trigger event configuration bit of line 4 */
#define  EXTI_FTENR_TR5                         ((uint32_t)0x00000020)        /* Falling trigger event configuration bit of line 5 */
#define  EXTI_FTENR_TR6                         ((uint32_t)0x00000040)        /* Falling trigger event configuration bit of line 6 */
#define  EXTI_FTENR_TR7                         ((uint32_t)0x00000080)        /* Falling trigger event configuration bit of line 7 */
#define  EXTI_FTENR_TR8                         ((uint32_t)0x00000100)        /* Falling trigger event configuration bit of line 8 */
#define  EXTI_FTENR_TR9                         ((uint32_t)0x00000200)        /* Falling trigger event configuration bit of line 9 */
#define  EXTI_FTENR_TR10                        ((uint32_t)0x00000400)        /* Falling trigger event configuration bit of line 10 */
#define  EXTI_FTENR_TR11                        ((uint32_t)0x00000800)        /* Falling trigger event configuration bit of line 11 */
#define  EXTI_FTENR_TR12                        ((uint32_t)0x00001000)        /* Falling trigger event configuration bit of line 12 */
#define  EXTI_FTENR_TR13                        ((uint32_t)0x00002000)        /* Falling trigger event configuration bit of line 13 */
#define  EXTI_FTENR_TR14                        ((uint32_t)0x00004000)        /* Falling trigger event configuration bit of line 14 */
#define  EXTI_FTENR_TR15                        ((uint32_t)0x00008000)        /* Falling trigger event configuration bit of line 15 */
#define  EXTI_FTENR_TR16                        ((uint32_t)0x00010000)        /* Falling trigger event configuration bit of line 16 */
#define  EXTI_FTENR_TR17                        ((uint32_t)0x00020000)        /* Falling trigger event configuration bit of line 17 */
#define  EXTI_FTENR_TR18                        ((uint32_t)0x00040000)        /* Falling trigger event configuration bit of line 18 */
#define  EXTI_FTENR_TR19                        ((uint32_t)0x00080000)        /* Falling trigger event configuration bit of line 19 */

/******************  Bit definition for EXTI_SWIEVR register  ******************/
#define  EXTI_SWIEVR_SWIEVR0                    ((uint32_t)0x00000001)        /* Software Interrupt on line 0 */
#define  EXTI_SWIEVR_SWIEVR1                    ((uint32_t)0x00000002)        /* Software Interrupt on line 1 */
#define  EXTI_SWIEVR_SWIEVR2                    ((uint32_t)0x00000004)        /* Software Interrupt on line 2 */
#define  EXTI_SWIEVR_SWIEVR3                    ((uint32_t)0x00000008)        /* Software Interrupt on line 3 */
#define  EXTI_SWIEVR_SWIEVR4                    ((uint32_t)0x00000010)        /* Software Interrupt on line 4 */
#define  EXTI_SWIEVR_SWIEVR5                    ((uint32_t)0x00000020)        /* Software Interrupt on line 5 */
#define  EXTI_SWIEVR_SWIEVR6                    ((uint32_t)0x00000040)        /* Software Interrupt on line 6 */
#define  EXTI_SWIEVR_SWIEVR7                    ((uint32_t)0x00000080)        /* Software Interrupt on line 7 */
#define  EXTI_SWIEVR_SWIEVR8                    ((uint32_t)0x00000100)        /* Software Interrupt on line 8 */
#define  EXTI_SWIEVR_SWIEVR9                    ((uint32_t)0x00000200)        /* Software Interrupt on line 9 */
#define  EXTI_SWIEVR_SWIEVR10                   ((uint32_t)0x00000400)        /* Software Interrupt on line 10 */
#define  EXTI_SWIEVR_SWIEVR11                   ((uint32_t)0x00000800)        /* Software Interrupt on line 11 */
#define  EXTI_SWIEVR_SWIEVR12                   ((uint32_t)0x00001000)        /* Software Interrupt on line 12 */
#define  EXTI_SWIEVR_SWIEVR13                   ((uint32_t)0x00002000)        /* Software Interrupt on line 13 */
#define  EXTI_SWIEVR_SWIEVR14                   ((uint32_t)0x00004000)        /* Software Interrupt on line 14 */
#define  EXTI_SWIEVR_SWIEVR15                   ((uint32_t)0x00008000)        /* Software Interrupt on line 15 */
#define  EXTI_SWIEVR_SWIEVR16                   ((uint32_t)0x00010000)        /* Software Interrupt on line 16 */
#define  EXTI_SWIEVR_SWIEVR17                   ((uint32_t)0x00020000)        /* Software Interrupt on line 17 */
#define  EXTI_SWIEVR_SWIEVR18                   ((uint32_t)0x00040000)        /* Software Interrupt on line 18 */
#define  EXTI_SWIEVR_SWIEVR19                   ((uint32_t)0x00080000)        /* Software Interrupt on line 19 */

/*******************  Bit definition for EXTI_INTFR register  ********************/
#define  EXTI_INTF_INTF0                        ((uint32_t)0x00000001)        /* Pending bit for line 0 */
#define  EXTI_INTF_INTF1                        ((uint32_t)0x00000002)        /* Pending bit for line 1 */
#define  EXTI_INTF_INTF2                        ((uint32_t)0x00000004)        /* Pending bit for line 2 */
#define  EXTI_INTF_INTF3                        ((uint32_t)0x00000008)        /* Pending bit for line 3 */
#define  EXTI_INTF_INTF4                        ((uint32_t)0x00000010)        /* Pending bit for line 4 */
#define  EXTI_INTF_INTF5                        ((uint32_t)0x00000020)        /* Pending bit for line 5 */
#define  EXTI_INTF_INTF6                        ((uint32_t)0x00000040)        /* Pending bit for line 6 */
#define  EXTI_INTF_INTF7                        ((uint32_t)0x00000080)        /* Pending bit for line 7 */
#define  EXTI_INTF_INTF8                        ((uint32_t)0x00000100)        /* Pending bit for line 8 */
#define  EXTI_INTF_INTF9                        ((uint32_t)0x00000200)        /* Pending bit for line 9 */
#define  EXTI_INTF_INTF10                       ((uint32_t)0x00000400)        /* Pending bit for line 10 */
#define  EXTI_INTF_INTF11                       ((uint32_t)0x00000800)        /* Pending bit for line 11 */
#define  EXTI_INTF_INTF12                       ((uint32_t)0x00001000)        /* Pending bit for line 12 */
#define  EXTI_INTF_INTF13                       ((uint32_t)0x00002000)        /* Pending bit for line 13 */
#define  EXTI_INTF_INTF14                       ((uint32_t)0x00004000)        /* Pending bit for line 14 */
#define  EXTI_INTF_INTF15                       ((uint32_t)0x00008000)        /* Pending bit for line 15 */
#define  EXTI_INTF_INTF16                       ((uint32_t)0x00010000)        /* Pending bit for line 16 */
#define  EXTI_INTF_INTF17                       ((uint32_t)0x00020000)        /* Pending bit for line 17 */
#define  EXTI_INTF_INTF18                       ((uint32_t)0x00040000)        /* Pending bit for line 18 */
#define  EXTI_INTF_INTF19                       ((uint32_t)0x00080000)        /* Pending bit for line 19 */

/******************************************************************************/
/*                      FLASH and Option Bytes Registers                      */
/******************************************************************************/


/*******************  Bit definition for FLASH_ACTLR register  ******************/

/******************  Bit definition for FLASH_KEYR register  ******************/
#define  FLASH_KEYR_FKEYR                      ((uint32_t)0xFFFFFFFF)        /* FPEC Key */

/*****************  Bit definition for FLASH_OBKEYR register  ****************/
#define  FLASH_OBKEYR_OBKEYR                   ((uint32_t)0xFFFFFFFF)        /* Option Byte Key */

/******************  Bit definition for FLASH_STATR register  *******************/
#define  FLASH_STATR_BSY                       ((uint8_t)0x01)               /* Busy */
#define  FLASH_STATR_PGERR                     ((uint8_t)0x04)               /* Programming Error */
#define  FLASH_STATR_WRPRTERR                  ((uint8_t)0x10)               /* Write Protection Error */
#define  FLASH_STATR_EOP                       ((uint8_t)0x20)               /* End of operation */

/*******************  Bit definition for FLASH_CTLR register  *******************/
#define  FLASH_CTLR_PG                         ((uint32_t)0x00000001)        /* Programming */
#define  FLASH_CTLR_PER                        ((uint32_t)0x00000002)        /* Sector Erase 4K */
#define  FLASH_CTLR_MER                        ((uint32_t)0x00000004)        /* Mass Erase */
#define  FLASH_CTLR_OPTPG                      ((uint32_t)0x00000010)        /* Option Byte Programming */
#define  FLASH_CTLR_OPTER                      ((uint32_t)0x00000020)        /* Option Byte Erase */
#define  FLASH_CTLR_STRT                       ((uint32_t)0x00000040)        /* Start */
#define  FLASH_CTLR_LOCK                       ((uint32_t)0x00000080)        /* Lock */
#define  FLASH_CTLR_OPTWRE                     ((uint32_t)0x00000200)        /* Option Bytes Write Enable */
#define  FLASH_CTLR_ERRIE                      ((uint32_t)0x00000400)        /* Error Interrupt Enable */
#define  FLASH_CTLR_EOPIE                      ((uint32_t)0x00001000)        /* End of operation interrupt enable */
#define  FLASH_CTLR_FAST_LOCK                  ((uint32_t)0x00008000)        /* Fast Lock */
#define  FLASH_CTLR_PAGE_PG                    ((uint32_t)0x00010000)        /* Page Programming 256Byte */
#define  FLASH_CTLR_PAGE_ER                    ((uint32_t)0x00020000)        /* Page Erase 256Byte */
#define  FLASH_CTLR_PAGE_BER32                 ((uint32_t)0x00040000)        /* Block Erase 32K */
#define  FLASH_CTLR_PAGE_BER64                 ((uint32_t)0x00080000)        /* Block Erase 64K */
#define  FLASH_CTLR_PG_STRT                    ((uint32_t)0x00200000)        /* Page Programming Start */

/*******************  Bit definition for FLASH_ADDR register  *******************/
#define  FLASH_ADDR_FAR                        ((uint32_t)0xFFFFFFFF)        /* Flash Address */

/******************  Bit definition for FLASH_OBR register  *******************/
#define  FLASH_OBR_OPTERR                      ((uint16_t)0x0001)            /* Option Byte Error */
#define  FLASH_OBR_RDPRT                       ((uint16_t)0x0002)            /* Read protection */

#define  FLASH_OBR_USER                        ((uint16_t)0x03FC)            /* User Option Bytes */
#define  FLASH_OBR_WDG_SW                      ((uint16_t)0x0004)            /* WDG_SW */
#define  FLASH_OBR_nRST_STOP                   ((uint16_t)0x0008)            /* nRST_STOP */
#define  FLASH_OBR_nRST_STDBY                  ((uint16_t)0x0010)            /* nRST_STDBY */
#define  FLASH_OBR_BFB2                        ((uint16_t)0x0020)            /* BFB2 */

/******************  Bit definition for FLASH_WPR register  ******************/
#define  FLASH_WPR_WRP                         ((uint32_t)0xFFFFFFFF)        /* Write Protect */

/******************  Bit definition for FLASH_RDPR register  *******************/
#define  FLASH_RDPR_RDPR                       ((uint32_t)0x000000FF)        /* Read protection option byte */
#define  FLASH_RDPR_nRDPR                      ((uint32_t)0x0000FF00)        /* Read protection complemented option byte */

/******************  Bit definition for FLASH_USER register  ******************/
#define  FLASH_USER_USER                       ((uint32_t)0x00FF0000)        /* User option byte */
#define  FLASH_USER_nUSER                      ((uint32_t)0xFF000000)        /* User complemented option byte */

/******************  Bit definition for FLASH_Data0 register  *****************/
#define  FLASH_Data0_Data0                     ((uint32_t)0x000000FF)        /* User data storage option byte */
#define  FLASH_Data0_nData0                    ((uint32_t)0x0000FF00)        /* User data storage complemented option byte */

/******************  Bit definition for FLASH_Data1 register  *****************/
#define  FLASH_Data1_Data1                     ((uint32_t)0x00FF0000)        /* User data storage option byte */
#define  FLASH_Data1_nData1                    ((uint32_t)0xFF000000)        /* User data storage complemented option byte */

/******************  Bit definition for FLASH_WRPR0 register  ******************/
#define  FLASH_WRPR0_WRPR0                     ((uint32_t)0x000000FF)        /* Flash memory write protection option bytes */
#define  FLASH_WRPR0_nWRPR0                    ((uint32_t)0x0000FF00)        /* Flash memory write protection complemented option bytes */

/******************  Bit definition for FLASH_WRPR1 register  ******************/
#define  FLASH_WRPR1_WRPR1                     ((uint32_t)0x00FF0000)        /* Flash memory write protection option bytes */
#define  FLASH_WRPR1_nWRPR1                    ((uint32_t)0xFF000000)        /* Flash memory write protection complemented option bytes */

/******************  Bit definition for FLASH_WRPR2 register  ******************/
#define  FLASH_WRPR2_WRPR2                     ((uint32_t)0x000000FF)        /* Flash memory write protection option bytes */
#define  FLASH_WRPR2_nWRPR2                    ((uint32_t)0x0000FF00)        /* Flash memory write protection complemented option bytes */

/******************  Bit definition for FLASH_WRPR3 register  ******************/
#define  FLASH_WRPR3_WRPR3                     ((uint32_t)0x00FF0000)        /* Flash memory write protection option bytes */
#define  FLASH_WRPR3_nWRPR3                    ((uint32_t)0xFF000000)        /* Flash memory write protection complemented option bytes */

/******************************************************************************/
/*                General Purpose and Alternate Function I/O                  */
/******************************************************************************/

/*******************  Bit definition for GPIO_CFGLR register  *******************/
#define  GPIO_CFGLR_MODE                       ((uint32_t)0x33333333)        /* Port x mode bits */

#define  GPIO_CFGLR_MODE0                      ((uint32_t)0x00000003)        /* MODE0[1:0] bits (Port x mode bits, pin 0) */
#define  GPIO_CFGLR_MODE0_0                    ((uint32_t)0x00000001)        /* Bit 0 */
#define  GPIO_CFGLR_MODE0_1                    ((uint32_t)0x00000002)        /* Bit 1 */

#define  GPIO_CFGLR_MODE1                      ((uint32_t)0x00000030)        /* MODE1[1:0] bits (Port x mode bits, pin 1) */
#define  GPIO_CFGLR_MODE1_0                    ((uint32_t)0x00000010)        /* Bit 0 */
#define  GPIO_CFGLR_MODE1_1                    ((uint32_t)0x00000020)        /* Bit 1 */

#define  GPIO_CFGLR_MODE2                      ((uint32_t)0x00000300)        /* MODE2[1:0] bits (Port x mode bits, pin 2) */
#define  GPIO_CFGLR_MODE2_0                    ((uint32_t)0x00000100)        /* Bit 0 */
#define  GPIO_CFGLR_MODE2_1                    ((uint32_t)0x00000200)        /* Bit 1 */

#define  GPIO_CFGLR_MODE3                      ((uint32_t)0x00003000)        /* MODE3[1:0] bits (Port x mode bits, pin 3) */
#define  GPIO_CFGLR_MODE3_0                    ((uint32_t)0x00001000)        /* Bit 0 */
#define  GPIO_CFGLR_MODE3_1                    ((uint32_t)0x00002000)        /* Bit 1 */

#define  GPIO_CFGLR_MODE4                      ((uint32_t)0x00030000)        /* MODE4[1:0] bits (Port x mode bits, pin 4) */
#define  GPIO_CFGLR_MODE4_0                    ((uint32_t)0x00010000)        /* Bit 0 */
#define  GPIO_CFGLR_MODE4_1                    ((uint32_t)0x00020000)        /* Bit 1 */

#define  GPIO_CFGLR_MODE5                      ((uint32_t)0x00300000)        /* MODE5[1:0] bits (Port x mode bits, pin 5) */
#define  GPIO_CFGLR_MODE5_0                    ((uint32_t)0x00100000)        /* Bit 0 */
#define  GPIO_CFGLR_MODE5_1                    ((uint32_t)0x00200000)        /* Bit 1 */

#define  GPIO_CFGLR_MODE6                      ((uint32_t)0x03000000)        /* MODE6[1:0] bits (Port x mode bits, pin 6) */
#define  GPIO_CFGLR_MODE6_0                    ((uint32_t)0x01000000)        /* Bit 0 */
#define  GPIO_CFGLR_MODE6_1                    ((uint32_t)0x02000000)        /* Bit 1 */

#define  GPIO_CFGLR_MODE7                      ((uint32_t)0x30000000)        /* MODE7[1:0] bits (Port x mode bits, pin 7) */
#define  GPIO_CFGLR_MODE7_0                    ((uint32_t)0x10000000)        /* Bit 0 */
#define  GPIO_CFGLR_MODE7_1                    ((uint32_t)0x20000000)        /* Bit 1 */

#define  GPIO_CFGLR_CNF                        ((uint32_t)0xCCCCCCCC)        /* Port x configuration bits */

#define  GPIO_CFGLR_CNF0                       ((uint32_t)0x0000000C)        /* CNF0[1:0] bits (Port x configuration bits, pin 0) */
#define  GPIO_CFGLR_CNF0_0                     ((uint32_t)0x00000004)        /* Bit 0 */
#define  GPIO_CFGLR_CNF0_1                     ((uint32_t)0x00000008)        /* Bit 1 */

#define  GPIO_CFGLR_CNF1                       ((uint32_t)0x000000C0)        /* CNF1[1:0] bits (Port x configuration bits, pin 1) */
#define  GPIO_CFGLR_CNF1_0                     ((uint32_t)0x00000040)        /* Bit 0 */
#define  GPIO_CFGLR_CNF1_1                     ((uint32_t)0x00000080)        /* Bit 1 */

#define  GPIO_CFGLR_CNF2                       ((uint32_t)0x00000C00)        /* CNF2[1:0] bits (Port x configuration bits, pin 2) */
#define  GPIO_CFGLR_CNF2_0                     ((uint32_t)0x00000400)        /* Bit 0 */
#define  GPIO_CFGLR_CNF2_1                     ((uint32_t)0x00000800)        /* Bit 1 */

#define  GPIO_CFGLR_CNF3                       ((uint32_t)0x0000C000)        /* CNF3[1:0] bits (Port x configuration bits, pin 3) */
#define  GPIO_CFGLR_CNF3_0                     ((uint32_t)0x00004000)        /* Bit 0 */
#define  GPIO_CFGLR_CNF3_1                     ((uint32_t)0x00008000)        /* Bit 1 */

#define  GPIO_CFGLR_CNF4                       ((uint32_t)0x000C0000)        /* CNF4[1:0] bits (Port x configuration bits, pin 4) */
#define  GPIO_CFGLR_CNF4_0                     ((uint32_t)0x00040000)        /* Bit 0 */
#define  GPIO_CFGLR_CNF4_1                     ((uint32_t)0x00080000)        /* Bit 1 */

#define  GPIO_CFGLR_CNF5                       ((uint32_t)0x00C00000)        /* CNF5[1:0] bits (Port x configuration bits, pin 5) */
#define  GPIO_CFGLR_CNF5_0                     ((uint32_t)0x00400000)        /* Bit 0 */
#define  GPIO_CFGLR_CNF5_1                     ((uint32_t)0x00800000)        /* Bit 1 */

#define  GPIO_CFGLR_CNF6                       ((uint32_t)0x0C000000)        /* CNF6[1:0] bits (Port x configuration bits, pin 6) */
#define  GPIO_CFGLR_CNF6_0                     ((uint32_t)0x04000000)        /* Bit 0 */
#define  GPIO_CFGLR_CNF6_1                     ((uint32_t)0x08000000)        /* Bit 1 */

#define  GPIO_CFGLR_CNF7                       ((uint32_t)0xC0000000)        /* CNF7[1:0] bits (Port x configuration bits, pin 7) */
#define  GPIO_CFGLR_CNF7_0                     ((uint32_t)0x40000000)        /* Bit 0 */
#define  GPIO_CFGLR_CNF7_1                     ((uint32_t)0x80000000)        /* Bit 1 */

/*******************  Bit definition for GPIO_CFGHR register  *******************/
#define  GPIO_CFGHR_MODE                       ((uint32_t)0x33333333)        /* Port x mode bits */

#define  GPIO_CFGHR_MODE8                      ((uint32_t)0x00000003)        /* MODE8[1:0] bits (Port x mode bits, pin 8) */
#define  GPIO_CFGHR_MODE8_0                    ((uint32_t)0x00000001)        /* Bit 0 */
#define  GPIO_CFGHR_MODE8_1                    ((uint32_t)0x00000002)        /* Bit 1 */

#define  GPIO_CFGHR_MODE9                      ((uint32_t)0x00000030)        /* MODE9[1:0] bits (Port x mode bits, pin 9) */
#define  GPIO_CFGHR_MODE9_0                    ((uint32_t)0x00000010)        /* Bit 0 */
#define  GPIO_CFGHR_MODE9_1                    ((uint32_t)0x00000020)        /* Bit 1 */

#define  GPIO_CFGHR_MODE10                     ((uint32_t)0x00000300)        /* MODE10[1:0] bits (Port x mode bits, pin 10) */
#define  GPIO_CFGHR_MODE10_0                   ((uint32_t)0x00000100)        /* Bit 0 */
#define  GPIO_CFGHR_MODE10_1                   ((uint32_t)0x00000200)        /* Bit 1 */

#define  GPIO_CFGHR_MODE11                     ((uint32_t)0x00003000)        /* MODE11[1:0] bits (Port x mode bits, pin 11) */
#define  GPIO_CFGHR_MODE11_0                   ((uint32_t)0x00001000)        /* Bit 0 */
#define  GPIO_CFGHR_MODE11_1                   ((uint32_t)0x00002000)        /* Bit 1 */

#define  GPIO_CFGHR_MODE12                     ((uint32_t)0x00030000)        /* MODE12[1:0] bits (Port x mode bits, pin 12) */
#define  GPIO_CFGHR_MODE12_0                   ((uint32_t)0x00010000)        /* Bit 0 */
#define  GPIO_CFGHR_MODE12_1                   ((uint32_t)0x00020000)        /* Bit 1 */

#define  GPIO_CFGHR_MODE13                     ((uint32_t)0x00300000)        /* MODE13[1:0] bits (Port x mode bits, pin 13) */
#define  GPIO_CFGHR_MODE13_0                   ((uint32_t)0x00100000)        /* Bit 0 */
#define  GPIO_CFGHR_MODE13_1                   ((uint32_t)0x00200000)        /* Bit 1 */

#define  GPIO_CFGHR_MODE14                     ((uint32_t)0x03000000)        /* MODE14[1:0] bits (Port x mode bits, pin 14) */
#define  GPIO_CFGHR_MODE14_0                   ((uint32_t)0x01000000)        /* Bit 0 */
#define  GPIO_CFGHR_MODE14_1                   ((uint32_t)0x02000000)        /* Bit 1 */

#define  GPIO_CFGHR_MODE15                     ((uint32_t)0x30000000)        /* MODE15[1:0] bits (Port x mode bits, pin 15) */
#define  GPIO_CFGHR_MODE15_0                   ((uint32_t)0x10000000)        /* Bit 0 */
#define  GPIO_CFGHR_MODE15_1                   ((uint32_t)0x20000000)        /* Bit 1 */

#define  GPIO_CFGHR_CNF                        ((uint32_t)0xCCCCCCCC)        /* Port x configuration bits */

#define  GPIO_CFGHR_CNF8                       ((uint32_t)0x0000000C)        /* CNF8[1:0] bits (Port x configuration bits, pin 8) */
#define  GPIO_CFGHR_CNF8_0                     ((uint32_t)0x00000004)        /* Bit 0 */
#define  GPIO_CFGHR_CNF8_1                     ((uint32_t)0x00000008)        /* Bit 1 */

#define  GPIO_CFGHR_CNF9                       ((uint32_t)0x000000C0)        /* CNF9[1:0] bits (Port x configuration bits, pin 9) */
#define  GPIO_CFGHR_CNF9_0                     ((uint32_t)0x00000040)        /* Bit 0 */
#define  GPIO_CFGHR_CNF9_1                     ((uint32_t)0x00000080)        /* Bit 1 */

#define  GPIO_CFGHR_CNF10                      ((uint32_t)0x00000C00)        /* CNF10[1:0] bits (Port x configuration bits, pin 10) */
#define  GPIO_CFGHR_CNF10_0                    ((uint32_t)0x00000400)        /* Bit 0 */
#define  GPIO_CFGHR_CNF10_1                    ((uint32_t)0x00000800)        /* Bit 1 */

#define  GPIO_CFGHR_CNF11                      ((uint32_t)0x0000C000)        /* CNF11[1:0] bits (Port x configuration bits, pin 11) */
#define  GPIO_CFGHR_CNF11_0                    ((uint32_t)0x00004000)        /* Bit 0 */
#define  GPIO_CFGHR_CNF11_1                    ((uint32_t)0x00008000)        /* Bit 1 */

#define  GPIO_CFGHR_CNF12                      ((uint32_t)0x000C0000)        /* CNF12[1:0] bits (Port x configuration bits, pin 12) */
#define  GPIO_CFGHR_CNF12_0                    ((uint32_t)0x00040000)        /* Bit 0 */
#define  GPIO_CFGHR_CNF12_1                    ((uint32_t)0x00080000)        /* Bit 1 */

#define  GPIO_CFGHR_CNF13                      ((uint32_t)0x00C00000)        /* CNF13[1:0] bits (Port x configuration bits, pin 13) */
#define  GPIO_CFGHR_CNF13_0                    ((uint32_t)0x00400000)        /* Bit 0 */
#define  GPIO_CFGHR_CNF13_1                    ((uint32_t)0x00800000)        /* Bit 1 */

#define  GPIO_CFGHR_CNF14                      ((uint32_t)0x0C000000)        /* CNF14[1:0] bits (Port x configuration bits, pin 14) */
#define  GPIO_CFGHR_CNF14_0                    ((uint32_t)0x04000000)        /* Bit 0 */
#define  GPIO_CFGHR_CNF14_1                    ((uint32_t)0x08000000)        /* Bit 1 */

#define  GPIO_CFGHR_CNF15                      ((uint32_t)0xC0000000)        /* CNF15[1:0] bits (Port x configuration bits, pin 15) */
#define  GPIO_CFGHR_CNF15_0                    ((uint32_t)0x40000000)        /* Bit 0 */
#define  GPIO_CFGHR_CNF15_1                    ((uint32_t)0x80000000)        /* Bit 1 */

/*******************  Bit definition for GPIO_INDR register  *******************/
#define GPIO_INDR_IDR0                         ((uint16_t)0x0001)            /* Port input data, bit 0 */
#define GPIO_INDR_IDR1                         ((uint16_t)0x0002)            /* Port input data, bit 1 */
#define GPIO_INDR_IDR2                         ((uint16_t)0x0004)            /* Port input data, bit 2 */
#define GPIO_INDR_IDR3                         ((uint16_t)0x0008)            /* Port input data, bit 3 */
#define GPIO_INDR_IDR4                         ((uint16_t)0x0010)            /* Port input data, bit 4 */
#define GPIO_INDR_IDR5                         ((uint16_t)0x0020)            /* Port input data, bit 5 */
#define GPIO_INDR_IDR6                         ((uint16_t)0x0040)            /* Port input data, bit 6 */
#define GPIO_INDR_IDR7                         ((uint16_t)0x0080)            /* Port input data, bit 7 */
#define GPIO_INDR_IDR8                         ((uint16_t)0x0100)            /* Port input data, bit 8 */
#define GPIO_INDR_IDR9                         ((uint16_t)0x0200)            /* Port input data, bit 9 */
#define GPIO_INDR_IDR10                        ((uint16_t)0x0400)            /* Port input data, bit 10 */
#define GPIO_INDR_IDR11                        ((uint16_t)0x0800)            /* Port input data, bit 11 */
#define GPIO_INDR_IDR12                        ((uint16_t)0x1000)            /* Port input data, bit 12 */
#define GPIO_INDR_IDR13                        ((uint16_t)0x2000)            /* Port input data, bit 13 */
#define GPIO_INDR_IDR14                        ((uint16_t)0x4000)            /* Port input data, bit 14 */
#define GPIO_INDR_IDR15                        ((uint16_t)0x8000)            /* Port input data, bit 15 */

/*******************  Bit definition for GPIO_OUTDR register  *******************/
#define GPIO_OUTDR_ODR0                        ((uint16_t)0x0001)            /* Port output data, bit 0 */
#define GPIO_OUTDR_ODR1                        ((uint16_t)0x0002)            /* Port output data, bit 1 */
#define GPIO_OUTDR_ODR2                        ((uint16_t)0x0004)            /* Port output data, bit 2 */
#define GPIO_OUTDR_ODR3                        ((uint16_t)0x0008)            /* Port output data, bit 3 */
#define GPIO_OUTDR_ODR4                        ((uint16_t)0x0010)            /* Port output data, bit 4 */
#define GPIO_OUTDR_ODR5                        ((uint16_t)0x0020)            /* Port output data, bit 5 */
#define GPIO_OUTDR_ODR6                        ((uint16_t)0x0040)            /* Port output data, bit 6 */
#define GPIO_OUTDR_ODR7                        ((uint16_t)0x0080)            /* Port output data, bit 7 */
#define GPIO_OUTDR_ODR8                        ((uint16_t)0x0100)            /* Port output data, bit 8 */
#define GPIO_OUTDR_ODR9                        ((uint16_t)0x0200)            /* Port output data, bit 9 */
#define GPIO_OUTDR_ODR10                       ((uint16_t)0x0400)            /* Port output data, bit 10 */
#define GPIO_OUTDR_ODR11                       ((uint16_t)0x0800)            /* Port output data, bit 11 */
#define GPIO_OUTDR_ODR12                       ((uint16_t)0x1000)            /* Port output data, bit 12 */
#define GPIO_OUTDR_ODR13                       ((uint16_t)0x2000)            /* Port output data, bit 13 */
#define GPIO_OUTDR_ODR14                       ((uint16_t)0x4000)            /* Port output data, bit 14 */
#define GPIO_OUTDR_ODR15                       ((uint16_t)0x8000)            /* Port output data, bit 15 */

/******************  Bit definition for GPIO_BSHR register  *******************/
#define GPIO_BSHR_BS0                          ((uint32_t)0x00000001)        /* Port x Set bit 0 */
#define GPIO_BSHR_BS1                          ((uint32_t)0x00000002)        /* Port x Set bit 1 */
#define GPIO_BSHR_BS2                          ((uint32_t)0x00000004)        /* Port x Set bit 2 */
#define GPIO_BSHR_BS3                          ((uint32_t)0x00000008)        /* Port x Set bit 3 */
#define GPIO_BSHR_BS4                          ((uint32_t)0x00000010)        /* Port x Set bit 4 */
#define GPIO_BSHR_BS5                          ((uint32_t)0x00000020)        /* Port x Set bit 5 */
#define GPIO_BSHR_BS6                          ((uint32_t)0x00000040)        /* Port x Set bit 6 */
#define GPIO_BSHR_BS7                          ((uint32_t)0x00000080)        /* Port x Set bit 7 */
#define GPIO_BSHR_BS8                          ((uint32_t)0x00000100)        /* Port x Set bit 8 */
#define GPIO_BSHR_BS9                          ((uint32_t)0x00000200)        /* Port x Set bit 9 */
#define GPIO_BSHR_BS10                         ((uint32_t)0x00000400)        /* Port x Set bit 10 */
#define GPIO_BSHR_BS11                         ((uint32_t)0x00000800)        /* Port x Set bit 11 */
#define GPIO_BSHR_BS12                         ((uint32_t)0x00001000)        /* Port x Set bit 12 */
#define GPIO_BSHR_BS13                         ((uint32_t)0x00002000)        /* Port x Set bit 13 */
#define GPIO_BSHR_BS14                         ((uint32_t)0x00004000)        /* Port x Set bit 14 */
#define GPIO_BSHR_BS15                         ((uint32_t)0x00008000)        /* Port x Set bit 15 */

#define GPIO_BSHR_BR0                          ((uint32_t)0x00010000)        /* Port x Reset bit 0 */
#define GPIO_BSHR_BR1                          ((uint32_t)0x00020000)        /* Port x Reset bit 1 */
#define GPIO_BSHR_BR2                          ((uint32_t)0x00040000)        /* Port x Reset bit 2 */
#define GPIO_BSHR_BR3                          ((uint32_t)0x00080000)        /* Port x Reset bit 3 */
#define GPIO_BSHR_BR4                          ((uint32_t)0x00100000)        /* Port x Reset bit 4 */
#define GPIO_BSHR_BR5                          ((uint32_t)0x00200000)        /* Port x Reset bit 5 */
#define GPIO_BSHR_BR6                          ((uint32_t)0x00400000)        /* Port x Reset bit 6 */
#define GPIO_BSHR_BR7                          ((uint32_t)0x00800000)        /* Port x Reset bit 7 */
#define GPIO_BSHR_BR8                          ((uint32_t)0x01000000)        /* Port x Reset bit 8 */
#define GPIO_BSHR_BR9                          ((uint32_t)0x02000000)        /* Port x Reset bit 9 */
#define GPIO_BSHR_BR10                         ((uint32_t)0x04000000)        /* Port x Reset bit 10 */
#define GPIO_BSHR_BR11                         ((uint32_t)0x08000000)        /* Port x Reset bit 11 */
#define GPIO_BSHR_BR12                         ((uint32_t)0x10000000)        /* Port x Reset bit 12 */
#define GPIO_BSHR_BR13                         ((uint32_t)0x20000000)        /* Port x Reset bit 13 */
#define GPIO_BSHR_BR14                         ((uint32_t)0x40000000)        /* Port x Reset bit 14 */
#define GPIO_BSHR_BR15                         ((uint32_t)0x80000000)        /* Port x Reset bit 15 */

/*******************  Bit definition for GPIO_BCR register  *******************/
#define GPIO_BCR_BR0                           ((uint16_t)0x0001)            /* Port x Reset bit 0 */
#define GPIO_BCR_BR1                           ((uint16_t)0x0002)            /* Port x Reset bit 1 */
#define GPIO_BCR_BR2                           ((uint16_t)0x0004)            /* Port x Reset bit 2 */
#define GPIO_BCR_BR3                           ((uint16_t)0x0008)            /* Port x Reset bit 3 */
#define GPIO_BCR_BR4                           ((uint16_t)0x0010)            /* Port x Reset bit 4 */
#define GPIO_BCR_BR5                           ((uint16_t)0x0020)            /* Port x Reset bit 5 */
#define GPIO_BCR_BR6                           ((uint16_t)0x0040)            /* Port x Reset bit 6 */
#define GPIO_BCR_BR7                           ((uint16_t)0x0080)            /* Port x Reset bit 7 */
#define GPIO_BCR_BR8                           ((uint16_t)0x0100)            /* Port x Reset bit 8 */
#define GPIO_BCR_BR9                           ((uint16_t)0x0200)            /* Port x Reset bit 9 */
#define GPIO_BCR_BR10                          ((uint16_t)0x0400)            /* Port x Reset bit 10 */
#define GPIO_BCR_BR11                          ((uint16_t)0x0800)            /* Port x Reset bit 11 */
#define GPIO_BCR_BR12                          ((uint16_t)0x1000)            /* Port x Reset bit 12 */
#define GPIO_BCR_BR13                          ((uint16_t)0x2000)            /* Port x Reset bit 13 */
#define GPIO_BCR_BR14                          ((uint16_t)0x4000)            /* Port x Reset bit 14 */
#define GPIO_BCR_BR15                          ((uint16_t)0x8000)            /* Port x Reset bit 15 */

/******************  Bit definition for GPIO_LCKR register  *******************/
#define GPIO_LCK0                              ((uint32_t)0x00000001)        /* Port x Lock bit 0 */
#define GPIO_LCK1                              ((uint32_t)0x00000002)        /* Port x Lock bit 1 */
#define GPIO_LCK2                              ((uint32_t)0x00000004)        /* Port x Lock bit 2 */
#define GPIO_LCK3                              ((uint32_t)0x00000008)        /* Port x Lock bit 3 */
#define GPIO_LCK4                              ((uint32_t)0x00000010)        /* Port x Lock bit 4 */
#define GPIO_LCK5                              ((uint32_t)0x00000020)        /* Port x Lock bit 5 */
#define GPIO_LCK6                              ((uint32_t)0x00000040)        /* Port x Lock bit 6 */
#define GPIO_LCK7                              ((uint32_t)0x00000080)        /* Port x Lock bit 7 */
#define GPIO_LCK8                              ((uint32_t)0x00000100)        /* Port x Lock bit 8 */
#define GPIO_LCK9                              ((uint32_t)0x00000200)        /* Port x Lock bit 9 */
#define GPIO_LCK10                             ((uint32_t)0x00000400)        /* Port x Lock bit 10 */
#define GPIO_LCK11                             ((uint32_t)0x00000800)        /* Port x Lock bit 11 */
#define GPIO_LCK12                             ((uint32_t)0x00001000)        /* Port x Lock bit 12 */
#define GPIO_LCK13                             ((uint32_t)0x00002000)        /* Port x Lock bit 13 */
#define GPIO_LCK14                             ((uint32_t)0x00004000)        /* Port x Lock bit 14 */
#define GPIO_LCK15                             ((uint32_t)0x00008000)        /* Port x Lock bit 15 */
#define GPIO_LCKK                              ((uint32_t)0x00010000)        /* Lock key */


/******************  Bit definition for AFIO_ECR register  *******************/
#define AFIO_ECR_PIN                           ((uint8_t)0x0F)               /* PIN[3:0] bits (Pin selection) */
#define AFIO_ECR_PIN_0                         ((uint8_t)0x01)               /* Bit 0 */
#define AFIO_ECR_PIN_1                         ((uint8_t)0x02)               /* Bit 1 */
#define AFIO_ECR_PIN_2                         ((uint8_t)0x04)               /* Bit 2 */
#define AFIO_ECR_PIN_3                         ((uint8_t)0x08)               /* Bit 3 */

#define AFIO_ECR_PIN_PX0                       ((uint8_t)0x00)               /* Pin 0 selected */
#define AFIO_ECR_PIN_PX1                       ((uint8_t)0x01)               /* Pin 1 selected */
#define AFIO_ECR_PIN_PX2                       ((uint8_t)0x02)               /* Pin 2 selected */
#define AFIO_ECR_PIN_PX3                       ((uint8_t)0x03)               /* Pin 3 selected */
#define AFIO_ECR_PIN_PX4                       ((uint8_t)0x04)               /* Pin 4 selected */
#define AFIO_ECR_PIN_PX5                       ((uint8_t)0x05)               /* Pin 5 selected */
#define AFIO_ECR_PIN_PX6                       ((uint8_t)0x06)               /* Pin 6 selected */
#define AFIO_ECR_PIN_PX7                       ((uint8_t)0x07)               /* Pin 7 selected */
#define AFIO_ECR_PIN_PX8                       ((uint8_t)0x08)               /* Pin 8 selected */
#define AFIO_ECR_PIN_PX9                       ((uint8_t)0x09)               /* Pin 9 selected */
#define AFIO_ECR_PIN_PX10                      ((uint8_t)0x0A)               /* Pin 10 selected */
#define AFIO_ECR_PIN_PX11                      ((uint8_t)0x0B)               /* Pin 11 selected */
#define AFIO_ECR_PIN_PX12                      ((uint8_t)0x0C)               /* Pin 12 selected */
#define AFIO_ECR_PIN_PX13                      ((uint8_t)0x0D)               /* Pin 13 selected */
#define AFIO_ECR_PIN_PX14                      ((uint8_t)0x0E)               /* Pin 14 selected */
#define AFIO_ECR_PIN_PX15                      ((uint8_t)0x0F)               /* Pin 15 selected */

#define AFIO_ECR_PORT                          ((uint8_t)0x70)               /* PORT[2:0] bits (Port selection) */
#define AFIO_ECR_PORT_0                        ((uint8_t)0x10)               /* Bit 0 */
#define AFIO_ECR_PORT_1                        ((uint8_t)0x20)               /* Bit 1 */
#define AFIO_ECR_PORT_2                        ((uint8_t)0x40)               /* Bit 2 */

#define AFIO_ECR_PORT_PA                       ((uint8_t)0x00)               /* Port A selected */
#define AFIO_ECR_PORT_PB                       ((uint8_t)0x10)               /* Port B selected */
#define AFIO_ECR_PORT_PC                       ((uint8_t)0x20)               /* Port C selected */
#define AFIO_ECR_PORT_PD                       ((uint8_t)0x30)               /* Port D selected */
#define AFIO_ECR_PORT_PE                       ((uint8_t)0x40)               /* Port E selected */

#define AFIO_ECR_EVOE                          ((uint8_t)0x80)               /* Event Output Enable */

/******************  Bit definition for AFIO_PCFR1register  *******************/
#define AFIO_PCFR1_SPI1_REMAP                  ((uint32_t)0x00000001)        /* SPI1 remapping */
#define AFIO_PCFR1_I2C1_REMAP                  ((uint32_t)0x00000002)        /* I2C1 remapping */
#define AFIO_PCFR1_USART1_REMAP                ((uint32_t)0x00000004)        /* USART1 remapping */
#define AFIO_PCFR1_USART2_REMAP                ((uint32_t)0x00000008)        /* USART2 remapping */

#define AFIO_PCFR1_USART3_REMAP                ((uint32_t)0x00000030)        /* USART3_REMAP[1:0] bits (USART3 remapping) */
#define AFIO_PCFR1_USART3_REMAP_0              ((uint32_t)0x00000010)        /* Bit 0 */
#define AFIO_PCFR1_USART3_REMAP_1              ((uint32_t)0x00000020)        /* Bit 1 */

#define AFIO_PCFR1_USART3_REMAP_NOREMAP        ((uint32_t)0x00000000)        /* No remap (TX/PB10, RX/PB11, CK/PB12, CTS/PB13, RTS/PB14) */
#define AFIO_PCFR1_USART3_REMAP_PARTIALREMAP   ((uint32_t)0x00000010)        /* Partial remap (TX/PC10, RX/PC11, CK/PC12, CTS/PB13, RTS/PB14) */
#define AFIO_PCFR1_USART3_REMAP_FULLREMAP      ((uint32_t)0x00000030)        /* Full remap (TX/PD8, RX/PD9, CK/PD10, CTS/PD11, RTS/PD12) */

#define AFIO_PCFR1_TIM1_REMAP                  ((uint32_t)0x000000C0)        /* TIM1_REMAP[1:0] bits (TIM1 remapping) */
#define AFIO_PCFR1_TIM1_REMAP_0                ((uint32_t)0x00000040)        /* Bit 0 */
#define AFIO_PCFR1_TIM1_REMAP_1                ((uint32_t)0x00000080)        /* Bit 1 */

#define AFIO_PCFR1_TIM1_REMAP_NOREMAP          ((uint32_t)0x00000000)        /* No remap (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PB12, CH1N/PB13, CH2N/PB14, CH3N/PB15) */
#define AFIO_PCFR1_TIM1_REMAP_PARTIALREMAP     ((uint32_t)0x00000040)        /* Partial remap (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PA6, CH1N/PA7, CH2N/PB0, CH3N/PB1) */
#define AFIO_PCFR1_TIM1_REMAP_FULLREMAP        ((uint32_t)0x000000C0)        /* Full remap (ETR/PE7, CH1/PE9, CH2/PE11, CH3/PE13, CH4/PE14, BKIN/PE15, CH1N/PE8, CH2N/PE10, CH3N/PE12) */

#define AFIO_PCFR1_TIM2_REMAP                  ((uint32_t)0x00000300)        /* TIM2_REMAP[1:0] bits (TIM2 remapping) */
#define AFIO_PCFR1_TIM2_REMAP_0                ((uint32_t)0x00000100)        /* Bit 0 */
#define AFIO_PCFR1_TIM2_REMAP_1                ((uint32_t)0x00000200)        /* Bit 1 */

#define AFIO_PCFR1_TIM2_REMAP_NOREMAP          ((uint32_t)0x00000000)        /* No remap (CH1/ETR/PA0, CH2/PA1, CH3/PA2, CH4/PA3) */
#define AFIO_PCFR1_TIM2_REMAP_PARTIALREMAP1    ((uint32_t)0x00000100)        /* Partial remap (CH1/ETR/PA15, CH2/PB3, CH3/PA2, CH4/PA3) */
#define AFIO_PCFR1_TIM2_REMAP_PARTIALREMAP2    ((uint32_t)0x00000200)        /* Partial remap (CH1/ETR/PA0, CH2/PA1, CH3/PB10, CH4/PB11) */
#define AFIO_PCFR1_TIM2_REMAP_FULLREMAP        ((uint32_t)0x00000300)        /* Full remap (CH1/ETR/PA15, CH2/PB3, CH3/PB10, CH4/PB11) */

#define AFIO_PCFR1_TIM3_REMAP                  ((uint32_t)0x00000C00)        /* TIM3_REMAP[1:0] bits (TIM3 remapping) */
#define AFIO_PCFR1_TIM3_REMAP_0                ((uint32_t)0x00000400)        /* Bit 0 */
#define AFIO_PCFR1_TIM3_REMAP_1                ((uint32_t)0x00000800)        /* Bit 1 */

#define AFIO_PCFR1_TIM3_REMAP_NOREMAP          ((uint32_t)0x00000000)        /* No remap (CH1/PA6, CH2/PA7, CH3/PB0, CH4/PB1) */
#define AFIO_PCFR1_TIM3_REMAP_PARTIALREMAP     ((uint32_t)0x00000800)        /* Partial remap (CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1) */
#define AFIO_PCFR1_TIM3_REMAP_FULLREMAP        ((uint32_t)0x00000C00)        /* Full remap (CH1/PC6, CH2/PC7, CH3/PC8, CH4/PC9) */

#define AFIO_PCFR1_TIM4_REMAP                  ((uint32_t)0x00001000)        /* TIM4_REMAP bit (TIM4 remapping) */

#define AFIO_PCFR1_CAN_REMAP                   ((uint32_t)0x00006000)        /* CAN_REMAP[1:0] bits (CAN Alternate function remapping) */
#define AFIO_PCFR1_CAN_REMAP_0                 ((uint32_t)0x00002000)        /* Bit 0 */
#define AFIO_PCFR1_CAN_REMAP_1                 ((uint32_t)0x00004000)        /* Bit 1 */

#define AFIO_PCFR1_CAN_REMAP_REMAP1            ((uint32_t)0x00000000)        /* CANRX mapped to PA11, CANTX mapped to PA12 */
#define AFIO_PCFR1_CAN_REMAP_REMAP2            ((uint32_t)0x00004000)        /* CANRX mapped to PB8, CANTX mapped to PB9 */
#define AFIO_PCFR1_CAN_REMAP_REMAP3            ((uint32_t)0x00006000)        /* CANRX mapped to PD0, CANTX mapped to PD1 */

#define AFIO_PCFR1_PD01_REMAP                  ((uint32_t)0x00008000)        /* Port D0/Port D1 mapping on OSC_IN/OSC_OUT */
#define AFIO_PCFR1_TIM5CH4_IREMAP              ((uint32_t)0x00010000)        /* TIM5 Channel4 Internal Remap */
#define AFIO_PCFR1_ADC1_ETRGINJ_REMAP          ((uint32_t)0x00020000)        /* ADC 1 External Trigger Injected Conversion remapping */
#define AFIO_PCFR1_ADC1_ETRGREG_REMAP          ((uint32_t)0x00040000)        /* ADC 1 External Trigger Regular Conversion remapping */
#define AFIO_PCFR1_ADC2_ETRGINJ_REMAP          ((uint32_t)0x00080000)        /* ADC 2 External Trigger Injected Conversion remapping */
#define AFIO_PCFR1_ADC2_ETRGREG_REMAP          ((uint32_t)0x00100000)        /* ADC 2 External Trigger Regular Conversion remapping */

#define AFIO_PCFR1_SWJ_CFG                     ((uint32_t)0x07000000)        /* SWJ_CFG[2:0] bits (Serial Wire JTAG configuration) */
#define AFIO_PCFR1_SWJ_CFG_0                   ((uint32_t)0x01000000)        /* Bit 0 */
#define AFIO_PCFR1_SWJ_CFG_1                   ((uint32_t)0x02000000)        /* Bit 1 */
#define AFIO_PCFR1_SWJ_CFG_2                   ((uint32_t)0x04000000)        /* Bit 2 */

#define AFIO_PCFR1_SWJ_CFG_RESET               ((uint32_t)0x00000000)        /* Full SWJ (JTAG-DP + SW-DP) : Reset State */
#define AFIO_PCFR1_SWJ_CFG_NOJNTRST            ((uint32_t)0x01000000)        /* Full SWJ (JTAG-DP + SW-DP) but without JNTRST */
#define AFIO_PCFR1_SWJ_CFG_JTAGDISABLE         ((uint32_t)0x02000000)        /* JTAG-DP Disabled and SW-DP Enabled */
#define AFIO_PCFR1_SWJ_CFG_DISABLE             ((uint32_t)0x04000000)        /* JTAG-DP Disabled and SW-DP Disabled */

/*****************  Bit definition for AFIO_EXTICR1 register  *****************/
#define AFIO_EXTICR1_EXTI0                     ((uint16_t)0x000F)            /* EXTI 0 configuration */
#define AFIO_EXTICR1_EXTI1                     ((uint16_t)0x00F0)            /* EXTI 1 configuration */
#define AFIO_EXTICR1_EXTI2                     ((uint16_t)0x0F00)            /* EXTI 2 configuration */
#define AFIO_EXTICR1_EXTI3                     ((uint16_t)0xF000)            /* EXTI 3 configuration */

#define AFIO_EXTICR1_EXTI0_PA                  ((uint16_t)0x0000)            /* PA[0] pin */
#define AFIO_EXTICR1_EXTI0_PB                  ((uint16_t)0x0001)            /* PB[0] pin */
#define AFIO_EXTICR1_EXTI0_PC                  ((uint16_t)0x0002)            /* PC[0] pin */
#define AFIO_EXTICR1_EXTI0_PD                  ((uint16_t)0x0003)            /* PD[0] pin */
#define AFIO_EXTICR1_EXTI0_PE                  ((uint16_t)0x0004)            /* PE[0] pin */
#define AFIO_EXTICR1_EXTI0_PF                  ((uint16_t)0x0005)            /* PF[0] pin */
#define AFIO_EXTICR1_EXTI0_PG                  ((uint16_t)0x0006)            /* PG[0] pin */

#define AFIO_EXTICR1_EXTI1_PA                  ((uint16_t)0x0000)            /* PA[1] pin */
#define AFIO_EXTICR1_EXTI1_PB                  ((uint16_t)0x0010)            /* PB[1] pin */
#define AFIO_EXTICR1_EXTI1_PC                  ((uint16_t)0x0020)            /* PC[1] pin */
#define AFIO_EXTICR1_EXTI1_PD                  ((uint16_t)0x0030)            /* PD[1] pin */
#define AFIO_EXTICR1_EXTI1_PE                  ((uint16_t)0x0040)            /* PE[1] pin */
#define AFIO_EXTICR1_EXTI1_PF                  ((uint16_t)0x0050)            /* PF[1] pin */
#define AFIO_EXTICR1_EXTI1_PG                  ((uint16_t)0x0060)            /* PG[1] pin */
 
#define AFIO_EXTICR1_EXTI2_PA                  ((uint16_t)0x0000)            /* PA[2] pin */
#define AFIO_EXTICR1_EXTI2_PB                  ((uint16_t)0x0100)            /* PB[2] pin */
#define AFIO_EXTICR1_EXTI2_PC                  ((uint16_t)0x0200)            /* PC[2] pin */
#define AFIO_EXTICR1_EXTI2_PD                  ((uint16_t)0x0300)            /* PD[2] pin */
#define AFIO_EXTICR1_EXTI2_PE                  ((uint16_t)0x0400)            /* PE[2] pin */
#define AFIO_EXTICR1_EXTI2_PF                  ((uint16_t)0x0500)            /* PF[2] pin */
#define AFIO_EXTICR1_EXTI2_PG                  ((uint16_t)0x0600)            /* PG[2] pin */

#define AFIO_EXTICR1_EXTI3_PA                  ((uint16_t)0x0000)            /* PA[3] pin */
#define AFIO_EXTICR1_EXTI3_PB                  ((uint16_t)0x1000)            /* PB[3] pin */
#define AFIO_EXTICR1_EXTI3_PC                  ((uint16_t)0x2000)            /* PC[3] pin */
#define AFIO_EXTICR1_EXTI3_PD                  ((uint16_t)0x3000)            /* PD[3] pin */
#define AFIO_EXTICR1_EXTI3_PE                  ((uint16_t)0x4000)            /* PE[3] pin */
#define AFIO_EXTICR1_EXTI3_PF                  ((uint16_t)0x5000)            /* PF[3] pin */
#define AFIO_EXTICR1_EXTI3_PG                  ((uint16_t)0x6000)            /* PG[3] pin */

/*****************  Bit definition for AFIO_EXTICR2 register  *****************/
#define AFIO_EXTICR2_EXTI4                     ((uint16_t)0x000F)            /* EXTI 4 configuration */
#define AFIO_EXTICR2_EXTI5                     ((uint16_t)0x00F0)            /* EXTI 5 configuration */
#define AFIO_EXTICR2_EXTI6                     ((uint16_t)0x0F00)            /* EXTI 6 configuration */
#define AFIO_EXTICR2_EXTI7                     ((uint16_t)0xF000)            /* EXTI 7 configuration */

#define AFIO_EXTICR2_EXTI4_PA                  ((uint16_t)0x0000)            /* PA[4] pin */
#define AFIO_EXTICR2_EXTI4_PB                  ((uint16_t)0x0001)            /* PB[4] pin */
#define AFIO_EXTICR2_EXTI4_PC                  ((uint16_t)0x0002)            /* PC[4] pin */
#define AFIO_EXTICR2_EXTI4_PD                  ((uint16_t)0x0003)            /* PD[4] pin */
#define AFIO_EXTICR2_EXTI4_PE                  ((uint16_t)0x0004)            /* PE[4] pin */
#define AFIO_EXTICR2_EXTI4_PF                  ((uint16_t)0x0005)            /* PF[4] pin */
#define AFIO_EXTICR2_EXTI4_PG                  ((uint16_t)0x0006)            /* PG[4] pin */

#define AFIO_EXTICR2_EXTI5_PA                  ((uint16_t)0x0000)            /* PA[5] pin */
#define AFIO_EXTICR2_EXTI5_PB                  ((uint16_t)0x0010)            /* PB[5] pin */
#define AFIO_EXTICR2_EXTI5_PC                  ((uint16_t)0x0020)            /* PC[5] pin */
#define AFIO_EXTICR2_EXTI5_PD                  ((uint16_t)0x0030)            /* PD[5] pin */
#define AFIO_EXTICR2_EXTI5_PE                  ((uint16_t)0x0040)            /* PE[5] pin */
#define AFIO_EXTICR2_EXTI5_PF                  ((uint16_t)0x0050)            /* PF[5] pin */
#define AFIO_EXTICR2_EXTI5_PG                  ((uint16_t)0x0060)            /* PG[5] pin */

#define AFIO_EXTICR2_EXTI6_PA                  ((uint16_t)0x0000)            /* PA[6] pin */
#define AFIO_EXTICR2_EXTI6_PB                  ((uint16_t)0x0100)            /* PB[6] pin */
#define AFIO_EXTICR2_EXTI6_PC                  ((uint16_t)0x0200)            /* PC[6] pin */
#define AFIO_EXTICR2_EXTI6_PD                  ((uint16_t)0x0300)            /* PD[6] pin */
#define AFIO_EXTICR2_EXTI6_PE                  ((uint16_t)0x0400)            /* PE[6] pin */
#define AFIO_EXTICR2_EXTI6_PF                  ((uint16_t)0x0500)            /* PF[6] pin */
#define AFIO_EXTICR2_EXTI6_PG                  ((uint16_t)0x0600)            /* PG[6] pin */

#define AFIO_EXTICR2_EXTI7_PA                  ((uint16_t)0x0000)            /* PA[7] pin */
#define AFIO_EXTICR2_EXTI7_PB                  ((uint16_t)0x1000)            /* PB[7] pin */
#define AFIO_EXTICR2_EXTI7_PC                  ((uint16_t)0x2000)            /* PC[7] pin */
#define AFIO_EXTICR2_EXTI7_PD                  ((uint16_t)0x3000)            /* PD[7] pin */
#define AFIO_EXTICR2_EXTI7_PE                  ((uint16_t)0x4000)            /* PE[7] pin */
#define AFIO_EXTICR2_EXTI7_PF                  ((uint16_t)0x5000)            /* PF[7] pin */
#define AFIO_EXTICR2_EXTI7_PG                  ((uint16_t)0x6000)            /* PG[7] pin */

/*****************  Bit definition for AFIO_EXTICR3 register  *****************/
#define AFIO_EXTICR3_EXTI8                     ((uint16_t)0x000F)            /* EXTI 8 configuration */
#define AFIO_EXTICR3_EXTI9                     ((uint16_t)0x00F0)            /* EXTI 9 configuration */
#define AFIO_EXTICR3_EXTI10                    ((uint16_t)0x0F00)            /* EXTI 10 configuration */
#define AFIO_EXTICR3_EXTI11                    ((uint16_t)0xF000)            /* EXTI 11 configuration */

#define AFIO_EXTICR3_EXTI8_PA                  ((uint16_t)0x0000)            /* PA[8] pin */
#define AFIO_EXTICR3_EXTI8_PB                  ((uint16_t)0x0001)            /* PB[8] pin */
#define AFIO_EXTICR3_EXTI8_PC                  ((uint16_t)0x0002)            /* PC[8] pin */
#define AFIO_EXTICR3_EXTI8_PD                  ((uint16_t)0x0003)            /* PD[8] pin */
#define AFIO_EXTICR3_EXTI8_PE                  ((uint16_t)0x0004)            /* PE[8] pin */
#define AFIO_EXTICR3_EXTI8_PF                  ((uint16_t)0x0005)            /* PF[8] pin */
#define AFIO_EXTICR3_EXTI8_PG                  ((uint16_t)0x0006)            /* PG[8] pin */

#define AFIO_EXTICR3_EXTI9_PA                  ((uint16_t)0x0000)            /* PA[9] pin */
#define AFIO_EXTICR3_EXTI9_PB                  ((uint16_t)0x0010)            /* PB[9] pin */
#define AFIO_EXTICR3_EXTI9_PC                  ((uint16_t)0x0020)            /* PC[9] pin */
#define AFIO_EXTICR3_EXTI9_PD                  ((uint16_t)0x0030)            /* PD[9] pin */
#define AFIO_EXTICR3_EXTI9_PE                  ((uint16_t)0x0040)            /* PE[9] pin */
#define AFIO_EXTICR3_EXTI9_PF                  ((uint16_t)0x0050)            /* PF[9] pin */
#define AFIO_EXTICR3_EXTI9_PG                  ((uint16_t)0x0060)            /* PG[9] pin */
 
#define AFIO_EXTICR3_EXTI10_PA                 ((uint16_t)0x0000)            /* PA[10] pin */
#define AFIO_EXTICR3_EXTI10_PB                 ((uint16_t)0x0100)            /* PB[10] pin */
#define AFIO_EXTICR3_EXTI10_PC                 ((uint16_t)0x0200)            /* PC[10] pin */
#define AFIO_EXTICR3_EXTI10_PD                 ((uint16_t)0x0300)            /* PD[10] pin */
#define AFIO_EXTICR3_EXTI10_PE                 ((uint16_t)0x0400)            /* PE[10] pin */
#define AFIO_EXTICR3_EXTI10_PF                 ((uint16_t)0x0500)            /* PF[10] pin */
#define AFIO_EXTICR3_EXTI10_PG                 ((uint16_t)0x0600)            /* PG[10] pin */

#define AFIO_EXTICR3_EXTI11_PA                 ((uint16_t)0x0000)            /* PA[11] pin */
#define AFIO_EXTICR3_EXTI11_PB                 ((uint16_t)0x1000)            /* PB[11] pin */
#define AFIO_EXTICR3_EXTI11_PC                 ((uint16_t)0x2000)            /* PC[11] pin */
#define AFIO_EXTICR3_EXTI11_PD                 ((uint16_t)0x3000)            /* PD[11] pin */
#define AFIO_EXTICR3_EXTI11_PE                 ((uint16_t)0x4000)            /* PE[11] pin */
#define AFIO_EXTICR3_EXTI11_PF                 ((uint16_t)0x5000)            /* PF[11] pin */
#define AFIO_EXTICR3_EXTI11_PG                 ((uint16_t)0x6000)            /* PG[11] pin */

/*****************  Bit definition for AFIO_EXTICR4 register  *****************/
#define AFIO_EXTICR4_EXTI12                    ((uint16_t)0x000F)            /* EXTI 12 configuration */
#define AFIO_EXTICR4_EXTI13                    ((uint16_t)0x00F0)            /* EXTI 13 configuration */
#define AFIO_EXTICR4_EXTI14                    ((uint16_t)0x0F00)            /* EXTI 14 configuration */
#define AFIO_EXTICR4_EXTI15                    ((uint16_t)0xF000)            /* EXTI 15 configuration */

#define AFIO_EXTICR4_EXTI12_PA                 ((uint16_t)0x0000)            /* PA[12] pin */
#define AFIO_EXTICR4_EXTI12_PB                 ((uint16_t)0x0001)            /* PB[12] pin */
#define AFIO_EXTICR4_EXTI12_PC                 ((uint16_t)0x0002)            /* PC[12] pin */
#define AFIO_EXTICR4_EXTI12_PD                 ((uint16_t)0x0003)            /* PD[12] pin */
#define AFIO_EXTICR4_EXTI12_PE                 ((uint16_t)0x0004)            /* PE[12] pin */
#define AFIO_EXTICR4_EXTI12_PF                 ((uint16_t)0x0005)            /* PF[12] pin */
#define AFIO_EXTICR4_EXTI12_PG                 ((uint16_t)0x0006)            /* PG[12] pin */

#define AFIO_EXTICR4_EXTI13_PA                 ((uint16_t)0x0000)            /* PA[13] pin */
#define AFIO_EXTICR4_EXTI13_PB                 ((uint16_t)0x0010)            /* PB[13] pin */
#define AFIO_EXTICR4_EXTI13_PC                 ((uint16_t)0x0020)            /* PC[13] pin */
#define AFIO_EXTICR4_EXTI13_PD                 ((uint16_t)0x0030)            /* PD[13] pin */
#define AFIO_EXTICR4_EXTI13_PE                 ((uint16_t)0x0040)            /* PE[13] pin */
#define AFIO_EXTICR4_EXTI13_PF                 ((uint16_t)0x0050)            /* PF[13] pin */
#define AFIO_EXTICR4_EXTI13_PG                 ((uint16_t)0x0060)            /* PG[13] pin */

#define AFIO_EXTICR4_EXTI14_PA                 ((uint16_t)0x0000)            /* PA[14] pin */
#define AFIO_EXTICR4_EXTI14_PB                 ((uint16_t)0x0100)            /* PB[14] pin */
#define AFIO_EXTICR4_EXTI14_PC                 ((uint16_t)0x0200)            /* PC[14] pin */
#define AFIO_EXTICR4_EXTI14_PD                 ((uint16_t)0x0300)            /* PD[14] pin */
#define AFIO_EXTICR4_EXTI14_PE                 ((uint16_t)0x0400)            /* PE[14] pin */
#define AFIO_EXTICR4_EXTI14_PF                 ((uint16_t)0x0500)            /* PF[14] pin */
#define AFIO_EXTICR4_EXTI14_PG                 ((uint16_t)0x0600)            /* PG[14] pin */

#define AFIO_EXTICR4_EXTI15_PA                 ((uint16_t)0x0000)            /* PA[15] pin */
#define AFIO_EXTICR4_EXTI15_PB                 ((uint16_t)0x1000)            /* PB[15] pin */
#define AFIO_EXTICR4_EXTI15_PC                 ((uint16_t)0x2000)            /* PC[15] pin */
#define AFIO_EXTICR4_EXTI15_PD                 ((uint16_t)0x3000)            /* PD[15] pin */
#define AFIO_EXTICR4_EXTI15_PE                 ((uint16_t)0x4000)            /* PE[15] pin */
#define AFIO_EXTICR4_EXTI15_PF                 ((uint16_t)0x5000)            /* PF[15] pin */
#define AFIO_EXTICR4_EXTI15_PG                 ((uint16_t)0x6000)            /* PG[15] pin */

/******************************************************************************/
/*                           Independent WATCHDOG                             */
/******************************************************************************/

/*******************  Bit definition for IWDG_CTLR register  ********************/
#define  IWDG_KEY                              ((uint16_t)0xFFFF)            /* Key value (write only, read 0000h) */

/*******************  Bit definition for IWDG_PSCR register  ********************/
#define  IWDG_PR                               ((uint8_t)0x07)               /* PR[2:0] (Prescaler divider) */
#define  IWDG_PR_0                             ((uint8_t)0x01)               /* Bit 0 */
#define  IWDG_PR_1                             ((uint8_t)0x02)               /* Bit 1 */
#define  IWDG_PR_2                             ((uint8_t)0x04)               /* Bit 2 */

/*******************  Bit definition for IWDG_RLDR register  *******************/
#define  IWDG_RL                               ((uint16_t)0x0FFF)            /* Watchdog counter reload value */

/*******************  Bit definition for IWDG_STATR register  ********************/
#define  IWDG_PVU                              ((uint8_t)0x01)               /* Watchdog prescaler value update */
#define  IWDG_RVU                              ((uint8_t)0x02)               /* Watchdog counter reload value update */

/******************************************************************************/
/*                      Inter-integrated Circuit Interface                    */
/******************************************************************************/

/*******************  Bit definition for I2C_CTLR1 register  ********************/
#define  I2C_CTLR1_PE                          ((uint16_t)0x0001)            /* Peripheral Enable */
#define  I2C_CTLR1_SMBUS                       ((uint16_t)0x0002)            /* SMBus Mode */
#define  I2C_CTLR1_SMBTYPE                     ((uint16_t)0x0008)            /* SMBus Type */
#define  I2C_CTLR1_ENARP                       ((uint16_t)0x0010)            /* ARP Enable */
#define  I2C_CTLR1_ENPEC                       ((uint16_t)0x0020)            /* PEC Enable */
#define  I2C_CTLR1_ENGC                        ((uint16_t)0x0040)            /* General Call Enable */
#define  I2C_CTLR1_NOSTRETCH                   ((uint16_t)0x0080)            /* Clock Stretching Disable (Slave mode) */
#define  I2C_CTLR1_START                       ((uint16_t)0x0100)            /* Start Generation */
#define  I2C_CTLR1_STOP                        ((uint16_t)0x0200)            /* Stop Generation */
#define  I2C_CTLR1_ACK                         ((uint16_t)0x0400)            /* Acknowledge Enable */
#define  I2C_CTLR1_POS                         ((uint16_t)0x0800)            /* Acknowledge/PEC Position (for data reception) */
#define  I2C_CTLR1_PEC                         ((uint16_t)0x1000)            /* Packet Error Checking */
#define  I2C_CTLR1_ALERT                       ((uint16_t)0x2000)            /* SMBus Alert */
#define  I2C_CTLR1_SWRST                       ((uint16_t)0x8000)            /* Software Reset */

/*******************  Bit definition for I2C_CTLR2 register  ********************/
#define  I2C_CTLR2_FREQ                        ((uint16_t)0x003F)            /* FREQ[5:0] bits (Peripheral Clock Frequency) */
#define  I2C_CTLR2_FREQ_0                      ((uint16_t)0x0001)            /* Bit 0 */
#define  I2C_CTLR2_FREQ_1                      ((uint16_t)0x0002)            /* Bit 1 */
#define  I2C_CTLR2_FREQ_2                      ((uint16_t)0x0004)            /* Bit 2 */
#define  I2C_CTLR2_FREQ_3                      ((uint16_t)0x0008)            /* Bit 3 */
#define  I2C_CTLR2_FREQ_4                      ((uint16_t)0x0010)            /* Bit 4 */
#define  I2C_CTLR2_FREQ_5                      ((uint16_t)0x0020)            /* Bit 5 */

#define  I2C_CTLR2_ITERREN                     ((uint16_t)0x0100)            /* Error Interrupt Enable */
#define  I2C_CTLR2_ITEVTEN                     ((uint16_t)0x0200)            /* Event Interrupt Enable */
#define  I2C_CTLR2_ITBUFEN                     ((uint16_t)0x0400)            /* Buffer Interrupt Enable */
#define  I2C_CTLR2_DMAEN                       ((uint16_t)0x0800)            /* DMA Requests Enable */
#define  I2C_CTLR2_LAST                        ((uint16_t)0x1000)            /* DMA Last Transfer */

/*******************  Bit definition for I2C_OADDR1 register  *******************/
#define  I2C_OADDR1_ADD1_7                     ((uint16_t)0x00FE)            /* Interface Address */
#define  I2C_OADDR1_ADD8_9                     ((uint16_t)0x0300)            /* Interface Address */

#define  I2C_OADDR1_ADD0                       ((uint16_t)0x0001)            /* Bit 0 */
#define  I2C_OADDR1_ADD1                       ((uint16_t)0x0002)            /* Bit 1 */
#define  I2C_OADDR1_ADD2                       ((uint16_t)0x0004)            /* Bit 2 */
#define  I2C_OADDR1_ADD3                       ((uint16_t)0x0008)            /* Bit 3 */
#define  I2C_OADDR1_ADD4                       ((uint16_t)0x0010)            /* Bit 4 */
#define  I2C_OADDR1_ADD5                       ((uint16_t)0x0020)            /* Bit 5 */
#define  I2C_OADDR1_ADD6                       ((uint16_t)0x0040)            /* Bit 6 */
#define  I2C_OADDR1_ADD7                       ((uint16_t)0x0080)            /* Bit 7 */
#define  I2C_OADDR1_ADD8                       ((uint16_t)0x0100)            /* Bit 8 */
#define  I2C_OADDR1_ADD9                       ((uint16_t)0x0200)            /* Bit 9 */

#define  I2C_OADDR1_ADDMODE                    ((uint16_t)0x8000)            /* Addressing Mode (Slave mode) */

/*******************  Bit definition for I2C_OADDR2 register  *******************/
#define  I2C_OADDR2_ENDUAL                     ((uint8_t)0x01)               /* Dual addressing mode enable */
#define  I2C_OADDR2_ADD2                       ((uint8_t)0xFE)               /* Interface address */

/********************  Bit definition for I2C_DATAR register  ********************/
#define  I2C_DR_DATAR                          ((uint8_t)0xFF)               /* 8-bit Data Register */

/*******************  Bit definition for I2C_STAR1 register  ********************/
#define  I2C_STAR1_SB                          ((uint16_t)0x0001)            /* Start Bit (Master mode) */
#define  I2C_STAR1_ADDR                        ((uint16_t)0x0002)            /* Address sent (master mode)/matched (slave mode) */
#define  I2C_STAR1_BTF                         ((uint16_t)0x0004)            /* Byte Transfer Finished */
#define  I2C_STAR1_ADD10                       ((uint16_t)0x0008)            /* 10-bit header sent (Master mode) */
#define  I2C_STAR1_STOPF                       ((uint16_t)0x0010)            /* Stop detection (Slave mode) */
#define  I2C_STAR1_RXNE                        ((uint16_t)0x0040)            /* Data Register not Empty (receivers) */
#define  I2C_STAR1_TXE                         ((uint16_t)0x0080)            /* Data Register Empty (transmitters) */
#define  I2C_STAR1_BERR                        ((uint16_t)0x0100)            /* Bus Error */
#define  I2C_STAR1_ARLO                        ((uint16_t)0x0200)            /* Arbitration Lost (master mode) */
#define  I2C_STAR1_AF                          ((uint16_t)0x0400)            /* Acknowledge Failure */
#define  I2C_STAR1_OVR                         ((uint16_t)0x0800)            /* Overrun/Underrun */
#define  I2C_STAR1_PECERR                      ((uint16_t)0x1000)            /* PEC Error in reception */
#define  I2C_STAR1_TIMEOUT                     ((uint16_t)0x4000)            /* Timeout or Tlow Error */
#define  I2C_STAR1_SMBALERT                    ((uint16_t)0x8000)            /* SMBus Alert */

/*******************  Bit definition for I2C_STAR2 register  ********************/
#define  I2C_STAR2_MSL                         ((uint16_t)0x0001)            /* Master/Slave */
#define  I2C_STAR2_BUSY                        ((uint16_t)0x0002)            /* Bus Busy */
#define  I2C_STAR2_TRA                         ((uint16_t)0x0004)            /* Transmitter/Receiver */
#define  I2C_STAR2_GENCALL                     ((uint16_t)0x0010)            /* General Call Address (Slave mode) */
#define  I2C_STAR2_SMBDEFAULT                  ((uint16_t)0x0020)            /* SMBus Device Default Address (Slave mode) */
#define  I2C_STAR2_SMBHOST                     ((uint16_t)0x0040)            /* SMBus Host Header (Slave mode) */
#define  I2C_STAR2_DUALF                       ((uint16_t)0x0080)            /* Dual Flag (Slave mode) */
#define  I2C_STAR2_PEC                         ((uint16_t)0xFF00)            /* Packet Error Checking Register */

/*******************  Bit definition for I2C_CKCFGR register  ********************/
#define  I2C_CKCFGR_CCR                        ((uint16_t)0x0FFF)            /* Clock Control Register in Fast/Standard mode (Master mode) */
#define  I2C_CKCFGR_DUTY                       ((uint16_t)0x4000)            /* Fast Mode Duty Cycle */
#define  I2C_CKCFGR_FS                         ((uint16_t)0x8000)            /* I2C Master Mode Selection */

/******************  Bit definition for I2C_RTR register  *******************/
#define  I2C_RTR_TRISE                         ((uint8_t)0x3F)               /* Maximum Rise Time in Fast/Standard mode (Master mode) */


/******************************************************************************/
/*                             Power Control                                  */
/******************************************************************************/

/********************  Bit definition for PWR_CTLR register  ********************/
#define  PWR_CTLR_LPDS                         ((uint16_t)0x0001)     /* Low-Power Deepsleep */
#define  PWR_CTLR_PDDS                         ((uint16_t)0x0002)     /* Power Down Deepsleep */
#define  PWR_CTLR_CWUF                         ((uint16_t)0x0004)     /* Clear Wakeup Flag */
#define  PWR_CTLR_CSBF                         ((uint16_t)0x0008)     /* Clear Standby Flag */
#define  PWR_CTLR_PVDE                         ((uint16_t)0x0010)     /* Power Voltage Detector Enable */

#define  PWR_CTLR_PLS                          ((uint16_t)0x00E0)     /* PLS[2:0] bits (PVD Level Selection) */
#define  PWR_CTLR_PLS_0                        ((uint16_t)0x0020)     /* Bit 0 */
#define  PWR_CTLR_PLS_1                        ((uint16_t)0x0040)     /* Bit 1 */
#define  PWR_CTLR_PLS_2                        ((uint16_t)0x0080)     /* Bit 2 */

#define  PWR_CTLR_PLS_2V2                      ((uint16_t)0x0000)     /* PVD level 2.2V */
#define  PWR_CTLR_PLS_2V3                      ((uint16_t)0x0020)     /* PVD level 2.3V */
#define  PWR_CTLR_PLS_2V4                      ((uint16_t)0x0040)     /* PVD level 2.4V */
#define  PWR_CTLR_PLS_2V5                      ((uint16_t)0x0060)     /* PVD level 2.5V */
#define  PWR_CTLR_PLS_2V6                      ((uint16_t)0x0080)     /* PVD level 2.6V */
#define  PWR_CTLR_PLS_2V7                      ((uint16_t)0x00A0)     /* PVD level 2.7V */
#define  PWR_CTLR_PLS_2V8                      ((uint16_t)0x00C0)     /* PVD level 2.8V */
#define  PWR_CTLR_PLS_2V9                      ((uint16_t)0x00E0)     /* PVD level 2.9V */

#define  PWR_CTLR_DBP                          ((uint16_t)0x0100)     /* Disable Backup Domain write protection */


/*******************  Bit definition for PWR_CSR register  ********************/
#define  PWR_CSR_WUF                           ((uint16_t)0x0001)     /* Wakeup Flag */
#define  PWR_CSR_SBF                           ((uint16_t)0x0002)     /* Standby Flag */
#define  PWR_CSR_PVDO                          ((uint16_t)0x0004)     /* PVD Output */
#define  PWR_CSR_EWUP                          ((uint16_t)0x0100)     /* Enable WKUP pin */



/******************************************************************************/
/*                         Reset and Clock Control                            */
/******************************************************************************/

/********************  Bit definition for RCC_CTLR register  ********************/
#define  RCC_HSION                       ((uint32_t)0x00000001)        /* Internal High Speed clock enable */
#define  RCC_HSIRDY                      ((uint32_t)0x00000002)        /* Internal High Speed clock ready flag */
#define  RCC_HSITRIM                     ((uint32_t)0x000000F8)        /* Internal High Speed clock trimming */
#define  RCC_HSICAL                      ((uint32_t)0x0000FF00)        /* Internal High Speed clock Calibration */
#define  RCC_HSEON                       ((uint32_t)0x00010000)        /* External High Speed clock enable */
#define  RCC_HSERDY                      ((uint32_t)0x00020000)        /* External High Speed clock ready flag */
#define  RCC_HSEBYP                      ((uint32_t)0x00040000)        /* External High Speed clock Bypass */
#define  RCC_CSSON                       ((uint32_t)0x00080000)        /* Clock Security System enable */
#define  RCC_PLLON                       ((uint32_t)0x01000000)        /* PLL enable */
#define  RCC_PLLRDY                      ((uint32_t)0x02000000)        /* PLL clock ready flag */


/*******************  Bit definition for RCC_CFGR0 register  *******************/
#define  RCC_SW                          ((uint32_t)0x00000003)        /* SW[1:0] bits (System clock Switch) */
#define  RCC_SW_0                        ((uint32_t)0x00000001)        /* Bit 0 */
#define  RCC_SW_1                        ((uint32_t)0x00000002)        /* Bit 1 */

#define  RCC_SW_HSI                      ((uint32_t)0x00000000)        /* HSI selected as system clock */
#define  RCC_SW_HSE                      ((uint32_t)0x00000001)        /* HSE selected as system clock */
#define  RCC_SW_PLL                      ((uint32_t)0x00000002)        /* PLL selected as system clock */

#define  RCC_SWS                         ((uint32_t)0x0000000C)        /* SWS[1:0] bits (System Clock Switch Status) */
#define  RCC_SWS_0                       ((uint32_t)0x00000004)        /* Bit 0 */
#define  RCC_SWS_1                       ((uint32_t)0x00000008)        /* Bit 1 */

#define  RCC_SWS_HSI                     ((uint32_t)0x00000000)        /* HSI oscillator used as system clock */
#define  RCC_SWS_HSE                     ((uint32_t)0x00000004)        /* HSE oscillator used as system clock */
#define  RCC_SWS_PLL                     ((uint32_t)0x00000008)        /* PLL used as system clock */

#define  RCC_HPRE                        ((uint32_t)0x000000F0)        /* HPRE[3:0] bits (AHB prescaler) */
#define  RCC_HPRE_0                      ((uint32_t)0x00000010)        /* Bit 0 */
#define  RCC_HPRE_1                      ((uint32_t)0x00000020)        /* Bit 1 */
#define  RCC_HPRE_2                      ((uint32_t)0x00000040)        /* Bit 2 */
#define  RCC_HPRE_3                      ((uint32_t)0x00000080)        /* Bit 3 */

#define  RCC_HPRE_DIV1                   ((uint32_t)0x00000000)        /* SYSCLK not divided */
#define  RCC_HPRE_DIV2                   ((uint32_t)0x00000080)        /* SYSCLK divided by 2 */
#define  RCC_HPRE_DIV4                   ((uint32_t)0x00000090)        /* SYSCLK divided by 4 */
#define  RCC_HPRE_DIV8                   ((uint32_t)0x000000A0)        /* SYSCLK divided by 8 */
#define  RCC_HPRE_DIV16                  ((uint32_t)0x000000B0)        /* SYSCLK divided by 16 */
#define  RCC_HPRE_DIV64                  ((uint32_t)0x000000C0)        /* SYSCLK divided by 64 */
#define  RCC_HPRE_DIV128                 ((uint32_t)0x000000D0)        /* SYSCLK divided by 128 */
#define  RCC_HPRE_DIV256                 ((uint32_t)0x000000E0)        /* SYSCLK divided by 256 */
#define  RCC_HPRE_DIV512                 ((uint32_t)0x000000F0)        /* SYSCLK divided by 512 */

#define  RCC_PPRE1                       ((uint32_t)0x00000700)        /* PRE1[2:0] bits (APB1 prescaler) */
#define  RCC_PPRE1_0                     ((uint32_t)0x00000100)        /* Bit 0 */
#define  RCC_PPRE1_1                     ((uint32_t)0x00000200)        /* Bit 1 */
#define  RCC_PPRE1_2                     ((uint32_t)0x00000400)        /* Bit 2 */

#define  RCC_PPRE1_DIV1                  ((uint32_t)0x00000000)        /* HCLK not divided */
#define  RCC_PPRE1_DIV2                  ((uint32_t)0x00000400)        /* HCLK divided by 2 */
#define  RCC_PPRE1_DIV4                  ((uint32_t)0x00000500)        /* HCLK divided by 4 */
#define  RCC_PPRE1_DIV8                  ((uint32_t)0x00000600)        /* HCLK divided by 8 */
#define  RCC_PPRE1_DIV16                 ((uint32_t)0x00000700)        /* HCLK divided by 16 */

#define  RCC_PPRE2                       ((uint32_t)0x00003800)        /* PRE2[2:0] bits (APB2 prescaler) */
#define  RCC_PPRE2_0                     ((uint32_t)0x00000800)        /* Bit 0 */
#define  RCC_PPRE2_1                     ((uint32_t)0x00001000)        /* Bit 1 */
#define  RCC_PPRE2_2                     ((uint32_t)0x00002000)        /* Bit 2 */

#define  RCC_PPRE2_DIV1                  ((uint32_t)0x00000000)        /* HCLK not divided */
#define  RCC_PPRE2_DIV2                  ((uint32_t)0x00002000)        /* HCLK divided by 2 */
#define  RCC_PPRE2_DIV4                  ((uint32_t)0x00002800)        /* HCLK divided by 4 */
#define  RCC_PPRE2_DIV8                  ((uint32_t)0x00003000)        /* HCLK divided by 8 */
#define  RCC_PPRE2_DIV16                 ((uint32_t)0x00003800)        /* HCLK divided by 16 */

#define  RCC_ADCPRE                      ((uint32_t)0x0000C000)        /* ADCPRE[1:0] bits (ADC prescaler) */
#define  RCC_ADCPRE_0                    ((uint32_t)0x00004000)        /* Bit 0 */
#define  RCC_ADCPRE_1                    ((uint32_t)0x00008000)        /* Bit 1 */

#define  RCC_ADCPRE_DIV2                 ((uint32_t)0x00000000)        /* PCLK2 divided by 2 */
#define  RCC_ADCPRE_DIV4                 ((uint32_t)0x00004000)        /* PCLK2 divided by 4 */
#define  RCC_ADCPRE_DIV6                 ((uint32_t)0x00008000)        /* PCLK2 divided by 6 */
#define  RCC_ADCPRE_DIV8                 ((uint32_t)0x0000C000)        /* PCLK2 divided by 8 */

#define  RCC_PLLSRC                      ((uint32_t)0x00010000)        /* PLL entry clock source */

#define  RCC_PLLXTPRE                    ((uint32_t)0x00020000)        /* HSE divider for PLL entry */

#define  RCC_PLLMULL                     ((uint32_t)0x003C0000)        /* PLLMUL[3:0] bits (PLL multiplication factor) */
#define  RCC_PLLMULL_0                   ((uint32_t)0x00040000)        /* Bit 0 */
#define  RCC_PLLMULL_1                   ((uint32_t)0x00080000)        /* Bit 1 */
#define  RCC_PLLMULL_2                   ((uint32_t)0x00100000)        /* Bit 2 */
#define  RCC_PLLMULL_3                   ((uint32_t)0x00200000)        /* Bit 3 */
 
#define  RCC_PLLSRC_HSI_Div2             ((uint32_t)0x00000000)        /* HSI clock divided by 2 selected as PLL entry clock source */
#define  RCC_PLLSRC_HSE                  ((uint32_t)0x00010000)        /* HSE clock selected as PLL entry clock source */
 
#define  RCC_PLLXTPRE_HSE                ((uint32_t)0x00000000)        /* HSE clock not divided for PLL entry */
#define  RCC_PLLXTPRE_HSE_Div2           ((uint32_t)0x00020000)        /* HSE clock divided by 2 for PLL entry */

/* for other CH32V30x */
#define  RCC_PLLMULL2                    ((uint32_t)0x00000000)        /* PLL input clock*2 */
#define  RCC_PLLMULL3                    ((uint32_t)0x00040000)        /* PLL input clock*3 */
#define  RCC_PLLMULL4                    ((uint32_t)0x00080000)        /* PLL input clock*4 */
#define  RCC_PLLMULL5                    ((uint32_t)0x000C0000)        /* PLL input clock*5 */
#define  RCC_PLLMULL6                    ((uint32_t)0x00100000)        /* PLL input clock*6 */
#define  RCC_PLLMULL7                    ((uint32_t)0x00140000)        /* PLL input clock*7 */
#define  RCC_PLLMULL8                    ((uint32_t)0x00180000)        /* PLL input clock*8 */
#define  RCC_PLLMULL9                    ((uint32_t)0x001C0000)        /* PLL input clock*9 */
#define  RCC_PLLMULL10                   ((uint32_t)0x00200000)        /* PLL input clock10 */
#define  RCC_PLLMULL11                   ((uint32_t)0x00240000)        /* PLL input clock*11 */
#define  RCC_PLLMULL12                   ((uint32_t)0x00280000)        /* PLL input clock*12 */
#define  RCC_PLLMULL13                   ((uint32_t)0x002C0000)        /* PLL input clock*13 */
#define  RCC_PLLMULL14                   ((uint32_t)0x00300000)        /* PLL input clock*14 */
#define  RCC_PLLMULL15                   ((uint32_t)0x00340000)        /* PLL input clock*15 */
#define  RCC_PLLMULL16                   ((uint32_t)0x00380000)        /* PLL input clock*16 */
#define  RCC_PLLMULL18                   ((uint32_t)0x003C0000)        /* PLL input clock*18 */
/* for CH32V307 */
#define  RCC_PLLMULL18_EXTEN             ((uint32_t)0x00000000)        /* PLL input clock*18 */
#define  RCC_PLLMULL3_EXTEN              ((uint32_t)0x00040000)        /* PLL input clock*3 */
#define  RCC_PLLMULL4_EXTEN              ((uint32_t)0x00080000)        /* PLL input clock*4 */
#define  RCC_PLLMULL5_EXTEN              ((uint32_t)0x000C0000)        /* PLL input clock*5 */
#define  RCC_PLLMULL6_EXTEN              ((uint32_t)0x00100000)        /* PLL input clock*6 */
#define  RCC_PLLMULL7_EXTEN              ((uint32_t)0x00140000)        /* PLL input clock*7 */
#define  RCC_PLLMULL8_EXTEN              ((uint32_t)0x00180000)        /* PLL input clock*8 */
#define  RCC_PLLMULL9_EXTEN              ((uint32_t)0x001C0000)        /* PLL input clock*9 */
#define  RCC_PLLMULL10_EXTEN             ((uint32_t)0x00200000)        /* PLL input clock10 */
#define  RCC_PLLMULL11_EXTEN             ((uint32_t)0x00240000)        /* PLL input clock*11 */
#define  RCC_PLLMULL12_EXTEN             ((uint32_t)0x00280000)        /* PLL input clock*12 */
#define  RCC_PLLMULL13_EXTEN             ((uint32_t)0x002C0000)        /* PLL input clock*13 */
#define  RCC_PLLMULL14_EXTEN             ((uint32_t)0x00300000)        /* PLL input clock*14 */
#define  RCC_PLLMULL6_5_EXTEN            ((uint32_t)0x00340000)        /* PLL input clock*6.5 */
#define  RCC_PLLMULL15_EXTEN             ((uint32_t)0x00380000)        /* PLL input clock*15 */
#define  RCC_PLLMULL16_EXTEN             ((uint32_t)0x003C0000)        /* PLL input clock*16 */

#define  RCC_USBPRE                      ((uint32_t)0x00400000)        /* USB Device prescaler */

#define  RCC_CFGR0_MCO                   ((uint32_t)0x07000000)        /* MCO[2:0] bits (Microcontroller Clock Output) */
#define  RCC_MCO_0                       ((uint32_t)0x01000000)        /* Bit 0 */
#define  RCC_MCO_1                       ((uint32_t)0x02000000)        /* Bit 1 */
#define  RCC_MCO_2                       ((uint32_t)0x04000000)        /* Bit 2 */

#define  RCC_MCO_NOCLOCK                 ((uint32_t)0x00000000)        /* No clock */
#define  RCC_CFGR0_MCO_SYSCLK            ((uint32_t)0x04000000)        /* System clock selected as MCO source */
#define  RCC_CFGR0_MCO_HSI               ((uint32_t)0x05000000)        /* HSI clock selected as MCO source */
#define  RCC_CFGR0_MCO_HSE               ((uint32_t)0x06000000)        /* HSE clock selected as MCO source  */
#define  RCC_CFGR0_MCO_PLL               ((uint32_t)0x07000000)        /* PLL clock divided by 2 selected as MCO source */

/*******************  Bit definition for RCC_INTR register  ********************/
#define  RCC_LSIRDYF                     ((uint32_t)0x00000001)        /* LSI Ready Interrupt flag */
#define  RCC_LSERDYF                     ((uint32_t)0x00000002)        /* LSE Ready Interrupt flag */
#define  RCC_HSIRDYF                     ((uint32_t)0x00000004)        /* HSI Ready Interrupt flag */
#define  RCC_HSERDYF                     ((uint32_t)0x00000008)        /* HSE Ready Interrupt flag */
#define  RCC_PLLRDYF                     ((uint32_t)0x00000010)        /* PLL Ready Interrupt flag */
#define  RCC_CSSF                        ((uint32_t)0x00000080)        /* Clock Security System Interrupt flag */
#define  RCC_LSIRDYIE                    ((uint32_t)0x00000100)        /* LSI Ready Interrupt Enable */
#define  RCC_LSERDYIE                    ((uint32_t)0x00000200)        /* LSE Ready Interrupt Enable */
#define  RCC_HSIRDYIE                    ((uint32_t)0x00000400)        /* HSI Ready Interrupt Enable */
#define  RCC_HSERDYIE                    ((uint32_t)0x00000800)        /* HSE Ready Interrupt Enable */
#define  RCC_PLLRDYIE                    ((uint32_t)0x00001000)        /* PLL Ready Interrupt Enable */
#define  RCC_LSIRDYC                     ((uint32_t)0x00010000)        /* LSI Ready Interrupt Clear */
#define  RCC_LSERDYC                     ((uint32_t)0x00020000)        /* LSE Ready Interrupt Clear */
#define  RCC_HSIRDYC                     ((uint32_t)0x00040000)        /* HSI Ready Interrupt Clear */
#define  RCC_HSERDYC                     ((uint32_t)0x00080000)        /* HSE Ready Interrupt Clear */
#define  RCC_PLLRDYC                     ((uint32_t)0x00100000)        /* PLL Ready Interrupt Clear */
#define  RCC_CSSC                        ((uint32_t)0x00800000)        /* Clock Security System Interrupt Clear */


/*****************  Bit definition for RCC_APB2PRSTR register  *****************/
#define  RCC_AFIORST                     ((uint32_t)0x00000001)        /* Alternate Function I/O reset */
#define  RCC_IOPARST                     ((uint32_t)0x00000004)        /* I/O port A reset */
#define  RCC_IOPBRST                     ((uint32_t)0x00000008)        /* I/O port B reset */
#define  RCC_IOPCRST                     ((uint32_t)0x00000010)        /* I/O port C reset */
#define  RCC_IOPDRST                     ((uint32_t)0x00000020)        /* I/O port D reset */
#define  RCC_ADC1RST                     ((uint32_t)0x00000200)        /* ADC 1 interface reset */


#define  RCC_ADC2RST                     ((uint32_t)0x00000400)        /* ADC 2 interface reset */


#define  RCC_TIM1RST                     ((uint32_t)0x00000800)        /* TIM1 Timer reset */
#define  RCC_SPI1RST                     ((uint32_t)0x00001000)        /* SPI 1 reset */
#define  RCC_USART1RST                   ((uint32_t)0x00004000)        /* USART1 reset */

#define  RCC_IOPERST                     ((uint32_t)0x00000040)        /* I/O port E reset */

/*****************  Bit definition for RCC_APB1PRSTR register  *****************/
#define  RCC_TIM2RST                     ((uint32_t)0x00000001)        /* Timer 2 reset */
#define  RCC_TIM3RST                     ((uint32_t)0x00000002)        /* Timer 3 reset */
#define  RCC_WWDGRST                     ((uint32_t)0x00000800)        /* Window Watchdog reset */
#define  RCC_USART2RST                   ((uint32_t)0x00020000)        /* USART 2 reset */
#define  RCC_I2C1RST                     ((uint32_t)0x00200000)        /* I2C 1 reset */
  
#define  RCC_CAN1RST                     ((uint32_t)0x02000000)        /* CAN1 reset */


#define  RCC_BKPRST                      ((uint32_t)0x08000000)        /* Backup interface reset */
#define  RCC_PWRRST                      ((uint32_t)0x10000000)        /* Power interface reset */


#define  RCC_TIM4RST                     ((uint32_t)0x00000004)        /* Timer 4 reset */
#define  RCC_SPI2RST                     ((uint32_t)0x00004000)        /* SPI 2 reset */
#define  RCC_USART3RST                   ((uint32_t)0x00040000)        /* USART 3 reset */
#define  RCC_I2C2RST                     ((uint32_t)0x00400000)        /* I2C 2 reset */

#define  RCC_USBRST                      ((uint32_t)0x00800000)        /* USB Device reset */

/******************  Bit definition for RCC_AHBPCENR register  ******************/
#define  RCC_DMA1EN                      ((uint16_t)0x0001)            /* DMA1 clock enable */
#define  RCC_SRAMEN                      ((uint16_t)0x0004)            /* SRAM interface clock enable */
#define  RCC_FLITFEN                     ((uint16_t)0x0010)            /* FLITF clock enable */
#define  RCC_CRCEN                       ((uint16_t)0x0040)            /* CRC clock enable */
#define  RCC_USBHD                       ((uint16_t)0x1000)

/******************  Bit definition for RCC_APB2PCENR register  *****************/
#define  RCC_AFIOEN                      ((uint32_t)0x00000001)         /* Alternate Function I/O clock enable */
#define  RCC_IOPAEN                      ((uint32_t)0x00000004)         /* I/O port A clock enable */
#define  RCC_IOPBEN                      ((uint32_t)0x00000008)         /* I/O port B clock enable */
#define  RCC_IOPCEN                      ((uint32_t)0x00000010)         /* I/O port C clock enable */
#define  RCC_IOPDEN                      ((uint32_t)0x00000020)         /* I/O port D clock enable */
#define  RCC_ADC1EN                      ((uint32_t)0x00000200)         /* ADC 1 interface clock enable */

#define  RCC_ADC2EN                      ((uint32_t)0x00000400)         /* ADC 2 interface clock enable */


#define  RCC_TIM1EN                      ((uint32_t)0x00000800)         /* TIM1 Timer clock enable */
#define  RCC_SPI1EN                      ((uint32_t)0x00001000)         /* SPI 1 clock enable */
#define  RCC_USART1EN                    ((uint32_t)0x00004000)         /* USART1 clock enable */

/*****************  Bit definition for RCC_APB1PCENR register  ******************/
#define  RCC_TIM2EN                      ((uint32_t)0x00000001)        /* Timer 2 clock enabled*/
#define  RCC_TIM3EN                      ((uint32_t)0x00000002)        /* Timer 3 clock enable */
#define  RCC_WWDGEN                      ((uint32_t)0x00000800)        /* Window Watchdog clock enable */
#define  RCC_USART2EN                    ((uint32_t)0x00020000)        /* USART 2 clock enable */
#define  RCC_I2C1EN                      ((uint32_t)0x00200000)        /* I2C 1 clock enable */

#define  RCC_BKPEN                       ((uint32_t)0x08000000)        /* Backup interface clock enable */
#define  RCC_PWREN                       ((uint32_t)0x10000000)        /* Power interface clock enable */


#define  RCC_USBEN                       ((uint32_t)0x00800000)        /* USB Device clock enable */

/*******************  Bit definition for RCC_BDCTLR register  *******************/
#define  RCC_LSEON                       ((uint32_t)0x00000001)        /* External Low Speed oscillator enable */
#define  RCC_LSERDY                      ((uint32_t)0x00000002)        /* External Low Speed oscillator Ready */
#define  RCC_LSEBYP                      ((uint32_t)0x00000004)        /* External Low Speed oscillator Bypass */

#define  RCC_RTCSEL                      ((uint32_t)0x00000300)        /* RTCSEL[1:0] bits (RTC clock source selection) */
#define  RCC_RTCSEL_0                    ((uint32_t)0x00000100)        /* Bit 0 */
#define  RCC_RTCSEL_1                    ((uint32_t)0x00000200)        /* Bit 1 */

#define  RCC_RTCSEL_NOCLOCK              ((uint32_t)0x00000000)        /* No clock */
#define  RCC_RTCSEL_LSE                  ((uint32_t)0x00000100)        /* LSE oscillator clock used as RTC clock */
#define  RCC_RTCSEL_LSI                  ((uint32_t)0x00000200)        /* LSI oscillator clock used as RTC clock */
#define  RCC_RTCSEL_HSE                  ((uint32_t)0x00000300)        /* HSE oscillator clock divided by 128 used as RTC clock */

#define  RCC_RTCEN                       ((uint32_t)0x00008000)        /* RTC clock enable */
#define  RCC_BDRST                       ((uint32_t)0x00010000)        /* Backup domain software reset  */

/*******************  Bit definition for RCC_RSTSCKR register  ********************/  
#define  RCC_LSION                       ((uint32_t)0x00000001)        /* Internal Low Speed oscillator enable */
#define  RCC_LSIRDY                      ((uint32_t)0x00000002)        /* Internal Low Speed oscillator Ready */
#define  RCC_RMVF                        ((uint32_t)0x01000000)        /* Remove reset flag */
#define  RCC_PINRSTF                     ((uint32_t)0x04000000)        /* PIN reset flag */
#define  RCC_PORRSTF                     ((uint32_t)0x08000000)        /* POR/PDR reset flag */
#define  RCC_SFTRSTF                     ((uint32_t)0x10000000)        /* Software Reset flag */
#define  RCC_IWDGRSTF                    ((uint32_t)0x20000000)        /* Independent Watchdog reset flag */
#define  RCC_WWDGRSTF                    ((uint32_t)0x40000000)        /* Window watchdog reset flag */
#define  RCC_LPWRRSTF                    ((uint32_t)0x80000000)        /* Low-Power reset flag */

/******************************************************************************/
/*                                    RNG                                     */
/******************************************************************************/
/********************  Bit definition for RNG_CR register  *******************/
#define  RNG_CR_RNGEN                         ((uint32_t)0x00000004)
#define  RNG_CR_IE                            ((uint32_t)0x00000008)

/********************  Bit definition for RNG_SR register  *******************/
#define  RNG_SR_DRDY                          ((uint32_t)0x00000001)
#define  RNG_SR_CECS                          ((uint32_t)0x00000002)
#define  RNG_SR_SECS                          ((uint32_t)0x00000004)
#define  RNG_SR_CEIS                          ((uint32_t)0x00000020)
#define  RNG_SR_SEIS                          ((uint32_t)0x00000040)

/******************************************************************************/
/*                             Real-Time Clock                                */
/******************************************************************************/

/*******************  Bit definition for RTC_CTLRH register  ********************/
#define  RTC_CTLRH_SECIE                     ((uint8_t)0x01)               /* Second Interrupt Enable */
#define  RTC_CTLRH_ALRIE                     ((uint8_t)0x02)               /* Alarm Interrupt Enable */
#define  RTC_CTLRH_OWIE                      ((uint8_t)0x04)               /* OverfloW Interrupt Enable */

/*******************  Bit definition for RTC_CTLRL register  ********************/
#define  RTC_CTLRL_SECF                      ((uint8_t)0x01)               /* Second Flag */
#define  RTC_CTLRL_ALRF                      ((uint8_t)0x02)               /* Alarm Flag */
#define  RTC_CTLRL_OWF                       ((uint8_t)0x04)               /* OverfloW Flag */
#define  RTC_CTLRL_RSF                       ((uint8_t)0x08)               /* Registers Synchronized Flag */
#define  RTC_CTLRL_CNF                       ((uint8_t)0x10)               /* Configuration Flag */
#define  RTC_CTLRL_RTOFF                     ((uint8_t)0x20)               /* RTC operation OFF */

/*******************  Bit definition for RTC_PSCH register  *******************/
#define  RTC_PSCH_PRL                        ((uint16_t)0x000F)            /* RTC Prescaler Reload Value High */

/*******************  Bit definition for RTC_PRLL register  *******************/
#define  RTC_PSCL_PRL                        ((uint16_t)0xFFFF)            /* RTC Prescaler Reload Value Low */

/*******************  Bit definition for RTC_DIVH register  *******************/
#define  RTC_DIVH_RTC_DIV                    ((uint16_t)0x000F)            /* RTC Clock Divider High */

/*******************  Bit definition for RTC_DIVL register  *******************/
#define  RTC_DIVL_RTC_DIV                    ((uint16_t)0xFFFF)            /* RTC Clock Divider Low */

/*******************  Bit definition for RTC_CNTH register  *******************/
#define  RTC_CNTH_RTC_CNT                    ((uint16_t)0xFFFF)            /* RTC Counter High */

/*******************  Bit definition for RTC_CNTL register  *******************/
#define  RTC_CNTL_RTC_CNT                    ((uint16_t)0xFFFF)            /* RTC Counter Low */

/*******************  Bit definition for RTC_ALRMH register  *******************/
#define  RTC_ALRMH_RTC_ALRM                  ((uint16_t)0xFFFF)            /* RTC Alarm High */

/*******************  Bit definition for RTC_ALRML register  *******************/
#define  RTC_ALRML_RTC_ALRM                  ((uint16_t)0xFFFF)            /* RTC Alarm Low */

/******************************************************************************/
/*                        Serial Peripheral Interface                         */
/******************************************************************************/

/*******************  Bit definition for SPI_CTLR1 register  ********************/
#define  SPI_CTLR1_CPHA                      ((uint16_t)0x0001)            /* Clock Phase */
#define  SPI_CTLR1_CPOL                      ((uint16_t)0x0002)            /* Clock Polarity */
#define  SPI_CTLR1_MSTR                      ((uint16_t)0x0004)            /* Master Selection */

#define  SPI_CTLR1_BR                        ((uint16_t)0x0038)            /* BR[2:0] bits (Baud Rate Control) */
#define  SPI_CTLR1_BR_0                      ((uint16_t)0x0008)            /* Bit 0 */
#define  SPI_CTLR1_BR_1                      ((uint16_t)0x0010)            /* Bit 1 */
#define  SPI_CTLR1_BR_2                      ((uint16_t)0x0020)            /* Bit 2 */

#define  SPI_CTLR1_SPE                       ((uint16_t)0x0040)            /* SPI Enable */
#define  SPI_CTLR1_LSBFIRST                  ((uint16_t)0x0080)            /* Frame Format */
#define  SPI_CTLR1_SSI                       ((uint16_t)0x0100)            /* Internal slave select */
#define  SPI_CTLR1_SSM                       ((uint16_t)0x0200)            /* Software slave management */
#define  SPI_CTLR1_RXONLY                    ((uint16_t)0x0400)            /* Receive only */
#define  SPI_CTLR1_DFF                       ((uint16_t)0x0800)            /* Data Frame Format */
#define  SPI_CTLR1_CRCNEXT                   ((uint16_t)0x1000)            /* Transmit CRC next */
#define  SPI_CTLR1_CRCEN                     ((uint16_t)0x2000)            /* Hardware CRC calculation enable */
#define  SPI_CTLR1_BIDIOE                    ((uint16_t)0x4000)            /* Output enable in bidirectional mode */
#define  SPI_CTLR1_BIDIMODE                  ((uint16_t)0x8000)            /* Bidirectional data mode enable */

/*******************  Bit definition for SPI_CTLR2 register  ********************/
#define  SPI_CTLR2_RXDMAEN                   ((uint8_t)0x01)               /* Rx Buffer DMA Enable */
#define  SPI_CTLR2_TXDMAEN                   ((uint8_t)0x02)               /* Tx Buffer DMA Enable */
#define  SPI_CTLR2_SSOE                      ((uint8_t)0x04)               /* SS Output Enable */
#define  SPI_CTLR2_ERRIE                     ((uint8_t)0x20)               /* Error Interrupt Enable */
#define  SPI_CTLR2_RXNEIE                    ((uint8_t)0x40)               /* RX buffer Not Empty Interrupt Enable */
#define  SPI_CTLR2_TXEIE                     ((uint8_t)0x80)               /* Tx buffer Empty Interrupt Enable */

/********************  Bit definition for SPI_STATR register  ********************/
#define  SPI_STATR_RXNE                      ((uint8_t)0x01)               /* Receive buffer Not Empty */
#define  SPI_STATR_TXE                       ((uint8_t)0x02)               /* Transmit buffer Empty */
#define  SPI_STATR_CHSIDE                    ((uint8_t)0x04)               /* Channel side */
#define  SPI_STATR_UDR                       ((uint8_t)0x08)               /* Underrun flag */
#define  SPI_STATR_CRCERR                    ((uint8_t)0x10)               /* CRC Error flag */
#define  SPI_STATR_MODF                      ((uint8_t)0x20)               /* Mode fault */
#define  SPI_STATR_OVR                       ((uint8_t)0x40)               /* Overrun flag */
#define  SPI_STATR_BSY                       ((uint8_t)0x80)               /* Busy flag */

/********************  Bit definition for SPI_DATAR register  ********************/
#define  SPI_DATAR_DR                        ((uint16_t)0xFFFF)            /* Data Register */

/*******************  Bit definition for SPI_CRCR register  ******************/
#define  SPI_CRCR_CRCPOLY                    ((uint16_t)0xFFFF)            /* CRC polynomial register */

/******************  Bit definition for SPI_RCRCR register  ******************/
#define  SPI_RCRCR_RXCRC                     ((uint16_t)0xFFFF)            /* Rx CRC Register */

/******************  Bit definition for SPI_TCRCR register  ******************/
#define  SPI_TCRCR_TXCRC                     ((uint16_t)0xFFFF)            /* Tx CRC Register */

/******************  Bit definition for SPI_I2SCFGR register  *****************/
#define  SPI_I2SCFGR_CHLEN                   ((uint16_t)0x0001)            /* Channel length (number of bits per audio channel) */

#define  SPI_I2SCFGR_DATLEN                  ((uint16_t)0x0006)            /* DATLEN[1:0] bits (Data length to be transferred) */
#define  SPI_I2SCFGR_DATLEN_0                ((uint16_t)0x0002)            /* Bit 0 */
#define  SPI_I2SCFGR_DATLEN_1                ((uint16_t)0x0004)            /* Bit 1 */

#define  SPI_I2SCFGR_CKPOL                   ((uint16_t)0x0008)            /* steady state clock polarity */

#define  SPI_I2SCFGR_I2SSTD                  ((uint16_t)0x0030)            /* I2SSTD[1:0] bits (I2S standard selection) */
#define  SPI_I2SCFGR_I2SSTD_0                ((uint16_t)0x0010)            /* Bit 0 */
#define  SPI_I2SCFGR_I2SSTD_1                ((uint16_t)0x0020)            /* Bit 1 */

#define  SPI_I2SCFGR_PCMSYNC                 ((uint16_t)0x0080)            /* PCM frame synchronization */

#define  SPI_I2SCFGR_I2SCFG                  ((uint16_t)0x0300)            /* I2SCFG[1:0] bits (I2S configuration mode) */
#define  SPI_I2SCFGR_I2SCFG_0                ((uint16_t)0x0100)            /* Bit 0 */
#define  SPI_I2SCFGR_I2SCFG_1                ((uint16_t)0x0200)            /* Bit 1 */

#define  SPI_I2SCFGR_I2SE                    ((uint16_t)0x0400)            /* I2S Enable */
#define  SPI_I2SCFGR_I2SMOD                  ((uint16_t)0x0800)            /* I2S mode selection */

/******************  Bit definition for SPI_I2SPR register  *******************/
#define  SPI_I2SPR_I2SDIV                    ((uint16_t)0x00FF)            /* I2S Linear prescaler */
#define  SPI_I2SPR_ODD                       ((uint16_t)0x0100)            /* Odd factor for the prescaler */
#define  SPI_I2SPR_MCKOE                     ((uint16_t)0x0200)            /* Master Clock Output Enable */

/******************************************************************************/
/*                                    TIM                                     */
/******************************************************************************/

/*******************  Bit definition for TIM_CTLR1 register  ********************/
#define  TIM_CEN                         ((uint16_t)0x0001)            /* Counter enable */
#define  TIM_UDIS                        ((uint16_t)0x0002)            /* Update disable */
#define  TIM_URS                         ((uint16_t)0x0004)            /* Update request source */
#define  TIM_OPM                         ((uint16_t)0x0008)            /* One pulse mode */
#define  TIM_DIR                         ((uint16_t)0x0010)            /* Direction */

#define  TIM_CMS                         ((uint16_t)0x0060)            /* CMS[1:0] bits (Center-aligned mode selection) */
#define  TIM_CMS_0                       ((uint16_t)0x0020)            /* Bit 0 */
#define  TIM_CMS_1                       ((uint16_t)0x0040)            /* Bit 1 */

#define  TIM_ARPE                        ((uint16_t)0x0080)            /* Auto-reload preload enable */

#define  TIM_CTLR1_CKD                   ((uint16_t)0x0300)            /* CKD[1:0] bits (clock division) */
#define  TIM_CKD_0                       ((uint16_t)0x0100)            /* Bit 0 */
#define  TIM_CKD_1                       ((uint16_t)0x0200)            /* Bit 1 */

/*******************  Bit definition for TIM_CTLR2 register  ********************/
#define  TIM_CCPC                        ((uint16_t)0x0001)            /* Capture/Compare Preloaded Control */
#define  TIM_CCUS                        ((uint16_t)0x0004)            /* Capture/Compare Control Update Selection */
#define  TIM_CCDS                        ((uint16_t)0x0008)            /* Capture/Compare DMA Selection */

#define  TIM_MMS                         ((uint16_t)0x0070)            /* MMS[2:0] bits (Master Mode Selection) */
#define  TIM_MMS_0                       ((uint16_t)0x0010)            /* Bit 0 */
#define  TIM_MMS_1                       ((uint16_t)0x0020)            /* Bit 1 */
#define  TIM_MMS_2                       ((uint16_t)0x0040)            /* Bit 2 */

#define  TIM_TI1S                        ((uint16_t)0x0080)            /* TI1 Selection */
#define  TIM_OIS1                        ((uint16_t)0x0100)            /* Output Idle state 1 (OC1 output) */
#define  TIM_OIS1N                       ((uint16_t)0x0200)            /* Output Idle state 1 (OC1N output) */
#define  TIM_OIS2                        ((uint16_t)0x0400)            /* Output Idle state 2 (OC2 output) */
#define  TIM_OIS2N                       ((uint16_t)0x0800)            /* Output Idle state 2 (OC2N output) */
#define  TIM_OIS3                        ((uint16_t)0x1000)            /* Output Idle state 3 (OC3 output) */
#define  TIM_OIS3N                       ((uint16_t)0x2000)            /* Output Idle state 3 (OC3N output) */
#define  TIM_OIS4                        ((uint16_t)0x4000)            /* Output Idle state 4 (OC4 output) */

/*******************  Bit definition for TIM_SMCFGR register  *******************/
#define  TIM_SMS                         ((uint16_t)0x0007)            /* SMS[2:0] bits (Slave mode selection) */
#define  TIM_SMS_0                       ((uint16_t)0x0001)            /* Bit 0 */
#define  TIM_SMS_1                       ((uint16_t)0x0002)            /* Bit 1 */
#define  TIM_SMS_2                       ((uint16_t)0x0004)            /* Bit 2 */

#define  TIM_TS                          ((uint16_t)0x0070)            /* TS[2:0] bits (Trigger selection) */
#define  TIM_TS_0                        ((uint16_t)0x0010)            /* Bit 0 */
#define  TIM_TS_1                        ((uint16_t)0x0020)            /* Bit 1 */
#define  TIM_TS_2                        ((uint16_t)0x0040)            /* Bit 2 */

#define  TIM_MSM                         ((uint16_t)0x0080)            /* Master/slave mode */
 
#define  TIM_ETF                         ((uint16_t)0x0F00)            /* ETF[3:0] bits (External trigger filter) */
#define  TIM_ETF_0                       ((uint16_t)0x0100)            /* Bit 0 */
#define  TIM_ETF_1                       ((uint16_t)0x0200)            /* Bit 1 */
#define  TIM_ETF_2                       ((uint16_t)0x0400)            /* Bit 2 */
#define  TIM_ETF_3                       ((uint16_t)0x0800)            /* Bit 3 */

#define  TIM_ETPS                        ((uint16_t)0x3000)            /* ETPS[1:0] bits (External trigger prescaler) */
#define  TIM_ETPS_0                      ((uint16_t)0x1000)            /* Bit 0 */
#define  TIM_ETPS_1                      ((uint16_t)0x2000)            /* Bit 1 */
 
#define  TIM_ECE                         ((uint16_t)0x4000)            /* External clock enable */
#define  TIM_ETP                         ((uint16_t)0x8000)            /* External trigger polarity */

/*******************  Bit definition for TIM_DMAINTENR register  *******************/
#define  TIM_UIE                         ((uint16_t)0x0001)            /* Update interrupt enable */
#define  TIM_CC1IE                       ((uint16_t)0x0002)            /* Capture/Compare 1 interrupt enable */
#define  TIM_CC2IE                       ((uint16_t)0x0004)            /* Capture/Compare 2 interrupt enable */
#define  TIM_CC3IE                       ((uint16_t)0x0008)            /* Capture/Compare 3 interrupt enable */
#define  TIM_CC4IE                       ((uint16_t)0x0010)            /* Capture/Compare 4 interrupt enable */
#define  TIM_COMIE                       ((uint16_t)0x0020)            /* COM interrupt enable */
#define  TIM_TIE                         ((uint16_t)0x0040)            /* Trigger interrupt enable */
#define  TIM_BIE                         ((uint16_t)0x0080)            /* Break interrupt enable */
#define  TIM_UDE                         ((uint16_t)0x0100)            /* Update DMA request enable */
#define  TIM_CC1DE                       ((uint16_t)0x0200)            /* Capture/Compare 1 DMA request enable */
#define  TIM_CC2DE                       ((uint16_t)0x0400)            /* Capture/Compare 2 DMA request enable */
#define  TIM_CC3DE                       ((uint16_t)0x0800)            /* Capture/Compare 3 DMA request enable */
#define  TIM_CC4DE                       ((uint16_t)0x1000)            /* Capture/Compare 4 DMA request enable */
#define  TIM_COMDE                       ((uint16_t)0x2000)            /* COM DMA request enable */
#define  TIM_TDE                         ((uint16_t)0x4000)            /* Trigger DMA request enable */

/********************  Bit definition for TIM_INTFR register  ********************/
#define  TIM_UIF                         ((uint16_t)0x0001)            /* Update interrupt Flag */
#define  TIM_CC1IF                       ((uint16_t)0x0002)            /* Capture/Compare 1 interrupt Flag */
#define  TIM_CC2IF                       ((uint16_t)0x0004)            /* Capture/Compare 2 interrupt Flag */
#define  TIM_CC3IF                       ((uint16_t)0x0008)            /* Capture/Compare 3 interrupt Flag */
#define  TIM_CC4IF                       ((uint16_t)0x0010)            /* Capture/Compare 4 interrupt Flag */
#define  TIM_COMIF                       ((uint16_t)0x0020)            /* COM interrupt Flag */
#define  TIM_TIF                         ((uint16_t)0x0040)            /* Trigger interrupt Flag */
#define  TIM_BIF                         ((uint16_t)0x0080)            /* Break interrupt Flag */
#define  TIM_CC1OF                       ((uint16_t)0x0200)            /* Capture/Compare 1 Overcapture Flag */
#define  TIM_CC2OF                       ((uint16_t)0x0400)            /* Capture/Compare 2 Overcapture Flag */
#define  TIM_CC3OF                       ((uint16_t)0x0800)            /* Capture/Compare 3 Overcapture Flag */
#define  TIM_CC4OF                       ((uint16_t)0x1000)            /* Capture/Compare 4 Overcapture Flag */

/*******************  Bit definition for TIM_SWEVGR register  ********************/
#define  TIM_UG                          ((uint8_t)0x01)               /* Update Generation */
#define  TIM_CC1G                        ((uint8_t)0x02)               /* Capture/Compare 1 Generation */
#define  TIM_CC2G                        ((uint8_t)0x04)               /* Capture/Compare 2 Generation */
#define  TIM_CC3G                        ((uint8_t)0x08)               /* Capture/Compare 3 Generation */
#define  TIM_CC4G                        ((uint8_t)0x10)               /* Capture/Compare 4 Generation */
#define  TIM_COMG                        ((uint8_t)0x20)               /* Capture/Compare Control Update Generation */
#define  TIM_TG                          ((uint8_t)0x40)               /* Trigger Generation */
#define  TIM_BG                          ((uint8_t)0x80)               /* Break Generation */

/******************  Bit definition for TIM_CHCTLR1 register  *******************/
#define  TIM_CC1S                        ((uint16_t)0x0003)            /* CC1S[1:0] bits (Capture/Compare 1 Selection) */
#define  TIM_CC1S_0                      ((uint16_t)0x0001)            /* Bit 0 */
#define  TIM_CC1S_1                      ((uint16_t)0x0002)            /* Bit 1 */

#define  TIM_OC1FE                       ((uint16_t)0x0004)            /* Output Compare 1 Fast enable */
#define  TIM_OC1PE                       ((uint16_t)0x0008)            /* Output Compare 1 Preload enable */

#define  TIM_OC1M                        ((uint16_t)0x0070)            /* OC1M[2:0] bits (Output Compare 1 Mode) */
#define  TIM_OC1M_0                      ((uint16_t)0x0010)            /* Bit 0 */
#define  TIM_OC1M_1                      ((uint16_t)0x0020)            /* Bit 1 */
#define  TIM_OC1M_2                      ((uint16_t)0x0040)            /* Bit 2 */

#define  TIM_OC1CE                       ((uint16_t)0x0080)            /* Output Compare 1Clear Enable */

#define  TIM_CC2S                        ((uint16_t)0x0300)            /* CC2S[1:0] bits (Capture/Compare 2 Selection) */
#define  TIM_CC2S_0                      ((uint16_t)0x0100)            /* Bit 0 */
#define  TIM_CC2S_1                      ((uint16_t)0x0200)            /* Bit 1 */

#define  TIM_OC2FE                       ((uint16_t)0x0400)            /* Output Compare 2 Fast enable */
#define  TIM_OC2PE                       ((uint16_t)0x0800)            /* Output Compare 2 Preload enable */

#define  TIM_OC2M                        ((uint16_t)0x7000)            /* OC2M[2:0] bits (Output Compare 2 Mode) */
#define  TIM_OC2M_0                      ((uint16_t)0x1000)            /* Bit 0 */
#define  TIM_OC2M_1                      ((uint16_t)0x2000)            /* Bit 1 */
#define  TIM_OC2M_2                      ((uint16_t)0x4000)            /* Bit 2 */

#define  TIM_OC2CE                       ((uint16_t)0x8000)            /* Output Compare 2 Clear Enable */


#define  TIM_IC1PSC                      ((uint16_t)0x000C)            /* IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
#define  TIM_IC1PSC_0                    ((uint16_t)0x0004)            /* Bit 0 */
#define  TIM_IC1PSC_1                    ((uint16_t)0x0008)            /* Bit 1 */

#define  TIM_IC1F                        ((uint16_t)0x00F0)            /* IC1F[3:0] bits (Input Capture 1 Filter) */
#define  TIM_IC1F_0                      ((uint16_t)0x0010)            /* Bit 0 */
#define  TIM_IC1F_1                      ((uint16_t)0x0020)            /* Bit 1 */
#define  TIM_IC1F_2                      ((uint16_t)0x0040)            /* Bit 2 */
#define  TIM_IC1F_3                      ((uint16_t)0x0080)            /* Bit 3 */

#define  TIM_IC2PSC                      ((uint16_t)0x0C00)            /* IC2PSC[1:0] bits (Input Capture 2 Prescaler) */
#define  TIM_IC2PSC_0                    ((uint16_t)0x0400)            /* Bit 0 */
#define  TIM_IC2PSC_1                    ((uint16_t)0x0800)            /* Bit 1 */

#define  TIM_IC2F                        ((uint16_t)0xF000)            /* IC2F[3:0] bits (Input Capture 2 Filter) */
#define  TIM_IC2F_0                      ((uint16_t)0x1000)            /* Bit 0 */
#define  TIM_IC2F_1                      ((uint16_t)0x2000)            /* Bit 1 */
#define  TIM_IC2F_2                      ((uint16_t)0x4000)            /* Bit 2 */
#define  TIM_IC2F_3                      ((uint16_t)0x8000)            /* Bit 3 */

/******************  Bit definition for TIM_CHCTLR2 register  *******************/
#define  TIM_CC3S                        ((uint16_t)0x0003)            /* CC3S[1:0] bits (Capture/Compare 3 Selection) */
#define  TIM_CC3S_0                      ((uint16_t)0x0001)            /* Bit 0 */
#define  TIM_CC3S_1                      ((uint16_t)0x0002)            /* Bit 1 */

#define  TIM_OC3FE                       ((uint16_t)0x0004)            /* Output Compare 3 Fast enable */
#define  TIM_OC3PE                       ((uint16_t)0x0008)            /* Output Compare 3 Preload enable */

#define  TIM_OC3M                        ((uint16_t)0x0070)            /* OC3M[2:0] bits (Output Compare 3 Mode) */
#define  TIM_OC3M_0                      ((uint16_t)0x0010)            /* Bit 0 */
#define  TIM_OC3M_1                      ((uint16_t)0x0020)            /* Bit 1 */
#define  TIM_OC3M_2                      ((uint16_t)0x0040)            /* Bit 2 */

#define  TIM_OC3CE                       ((uint16_t)0x0080)            /* Output Compare 3 Clear Enable */

#define  TIM_CC4S                        ((uint16_t)0x0300)            /* CC4S[1:0] bits (Capture/Compare 4 Selection) */
#define  TIM_CC4S_0                      ((uint16_t)0x0100)            /* Bit 0 */
#define  TIM_CC4S_1                      ((uint16_t)0x0200)            /* Bit 1 */

#define  TIM_OC4FE                       ((uint16_t)0x0400)            /* Output Compare 4 Fast enable */
#define  TIM_OC4PE                       ((uint16_t)0x0800)            /* Output Compare 4 Preload enable */

#define  TIM_OC4M                        ((uint16_t)0x7000)            /* OC4M[2:0] bits (Output Compare 4 Mode) */
#define  TIM_OC4M_0                      ((uint16_t)0x1000)            /* Bit 0 */
#define  TIM_OC4M_1                      ((uint16_t)0x2000)            /* Bit 1 */
#define  TIM_OC4M_2                      ((uint16_t)0x4000)            /* Bit 2 */

#define  TIM_OC4CE                       ((uint16_t)0x8000)            /* Output Compare 4 Clear Enable */


#define  TIM_IC3PSC                      ((uint16_t)0x000C)            /* IC3PSC[1:0] bits (Input Capture 3 Prescaler) */
#define  TIM_IC3PSC_0                    ((uint16_t)0x0004)            /* Bit 0 */
#define  TIM_IC3PSC_1                    ((uint16_t)0x0008)            /* Bit 1 */

#define  TIM_IC3F                        ((uint16_t)0x00F0)            /* IC3F[3:0] bits (Input Capture 3 Filter) */
#define  TIM_IC3F_0                      ((uint16_t)0x0010)            /* Bit 0 */
#define  TIM_IC3F_1                      ((uint16_t)0x0020)            /* Bit 1 */
#define  TIM_IC3F_2                      ((uint16_t)0x0040)            /* Bit 2 */
#define  TIM_IC3F_3                      ((uint16_t)0x0080)            /* Bit 3 */

#define  TIM_IC4PSC                      ((uint16_t)0x0C00)            /* IC4PSC[1:0] bits (Input Capture 4 Prescaler) */
#define  TIM_IC4PSC_0                    ((uint16_t)0x0400)            /* Bit 0 */
#define  TIM_IC4PSC_1                    ((uint16_t)0x0800)            /* Bit 1 */

#define  TIM_IC4F                        ((uint16_t)0xF000)            /* IC4F[3:0] bits (Input Capture 4 Filter) */
#define  TIM_IC4F_0                      ((uint16_t)0x1000)            /* Bit 0 */
#define  TIM_IC4F_1                      ((uint16_t)0x2000)            /* Bit 1 */
#define  TIM_IC4F_2                      ((uint16_t)0x4000)            /* Bit 2 */
#define  TIM_IC4F_3                      ((uint16_t)0x8000)            /* Bit 3 */

/*******************  Bit definition for TIM_CCER register  *******************/
#define  TIM_CC1E                        ((uint16_t)0x0001)            /* Capture/Compare 1 output enable */
#define  TIM_CC1P                        ((uint16_t)0x0002)            /* Capture/Compare 1 output Polarity */
#define  TIM_CC1NE                       ((uint16_t)0x0004)            /* Capture/Compare 1 Complementary output enable */
#define  TIM_CC1NP                       ((uint16_t)0x0008)            /* Capture/Compare 1 Complementary output Polarity */
#define  TIM_CC2E                        ((uint16_t)0x0010)            /* Capture/Compare 2 output enable */
#define  TIM_CC2P                        ((uint16_t)0x0020)            /* Capture/Compare 2 output Polarity */
#define  TIM_CC2NE                       ((uint16_t)0x0040)            /* Capture/Compare 2 Complementary output enable */
#define  TIM_CC2NP                       ((uint16_t)0x0080)            /* Capture/Compare 2 Complementary output Polarity */
#define  TIM_CC3E                        ((uint16_t)0x0100)            /* Capture/Compare 3 output enable */
#define  TIM_CC3P                        ((uint16_t)0x0200)            /* Capture/Compare 3 output Polarity */
#define  TIM_CC3NE                       ((uint16_t)0x0400)            /* Capture/Compare 3 Complementary output enable */
#define  TIM_CC3NP                       ((uint16_t)0x0800)            /* Capture/Compare 3 Complementary output Polarity */
#define  TIM_CC4E                        ((uint16_t)0x1000)            /* Capture/Compare 4 output enable */
#define  TIM_CC4P                        ((uint16_t)0x2000)            /* Capture/Compare 4 output Polarity */
#define  TIM_CC4NP                       ((uint16_t)0x8000)            /* Capture/Compare 4 Complementary output Polarity */

/*******************  Bit definition for TIM_CNT register  ********************/
#define  TIM_CNT                         ((uint16_t)0xFFFF)            /* Counter Value */

/*******************  Bit definition for TIM_PSC register  ********************/
#define  TIM_PSC                         ((uint16_t)0xFFFF)            /* Prescaler Value */

/*******************  Bit definition for TIM_ATRLR register  ********************/
#define  TIM_ARR                         ((uint16_t)0xFFFF)            /* actual auto-reload Value */

/*******************  Bit definition for TIM_RPTCR register  ********************/
#define  TIM_REP                         ((uint8_t)0xFF)               /* Repetition Counter Value */

/*******************  Bit definition for TIM_CH1CVR register  *******************/
#define  TIM_CCR1                        ((uint16_t)0xFFFF)            /* Capture/Compare 1 Value */

/*******************  Bit definition for TIM_CH2CVR register  *******************/
#define  TIM_CCR2                        ((uint16_t)0xFFFF)            /* Capture/Compare 2 Value */

/*******************  Bit definition for TIM_CH3CVR register  *******************/
#define  TIM_CCR3                        ((uint16_t)0xFFFF)            /* Capture/Compare 3 Value */

/*******************  Bit definition for TIM_CH4CVR register  *******************/
#define  TIM_CCR4                        ((uint16_t)0xFFFF)            /* Capture/Compare 4 Value */

/*******************  Bit definition for TIM_BDTR register  *******************/
#define  TIM_DTG                         ((uint16_t)0x00FF)            /* DTG[0:7] bits (Dead-Time Generator set-up) */
#define  TIM_DTG_0                       ((uint16_t)0x0001)            /* Bit 0 */
#define  TIM_DTG_1                       ((uint16_t)0x0002)            /* Bit 1 */
#define  TIM_DTG_2                       ((uint16_t)0x0004)            /* Bit 2 */
#define  TIM_DTG_3                       ((uint16_t)0x0008)            /* Bit 3 */
#define  TIM_DTG_4                       ((uint16_t)0x0010)            /* Bit 4 */
#define  TIM_DTG_5                       ((uint16_t)0x0020)            /* Bit 5 */
#define  TIM_DTG_6                       ((uint16_t)0x0040)            /* Bit 6 */
#define  TIM_DTG_7                       ((uint16_t)0x0080)            /* Bit 7 */

#define  TIM_LOCK                        ((uint16_t)0x0300)            /* LOCK[1:0] bits (Lock Configuration) */
#define  TIM_LOCK_0                      ((uint16_t)0x0100)            /* Bit 0 */
#define  TIM_LOCK_1                      ((uint16_t)0x0200)            /* Bit 1 */
 
#define  TIM_OSSI                        ((uint16_t)0x0400)            /* Off-State Selection for Idle mode */
#define  TIM_OSSR                        ((uint16_t)0x0800)            /* Off-State Selection for Run mode */
#define  TIM_BKE                         ((uint16_t)0x1000)            /* Break enable */
#define  TIM_BKP                         ((uint16_t)0x2000)            /* Break Polarity */
#define  TIM_AOE                         ((uint16_t)0x4000)            /* Automatic Output enable */
#define  TIM_MOE                         ((uint16_t)0x8000)            /* Main Output enable */

/*******************  Bit definition for TIM_DMACFGR register  ********************/
#define  TIM_DBA                         ((uint16_t)0x001F)            /* DBA[4:0] bits (DMA Base Address) */
#define  TIM_DBA_0                       ((uint16_t)0x0001)            /* Bit 0 */
#define  TIM_DBA_1                       ((uint16_t)0x0002)            /* Bit 1 */
#define  TIM_DBA_2                       ((uint16_t)0x0004)            /* Bit 2 */
#define  TIM_DBA_3                       ((uint16_t)0x0008)            /* Bit 3 */
#define  TIM_DBA_4                       ((uint16_t)0x0010)            /* Bit 4 */

#define  TIM_DBL                         ((uint16_t)0x1F00)            /* DBL[4:0] bits (DMA Burst Length) */
#define  TIM_DBL_0                       ((uint16_t)0x0100)            /* Bit 0 */
#define  TIM_DBL_1                       ((uint16_t)0x0200)            /* Bit 1 */
#define  TIM_DBL_2                       ((uint16_t)0x0400)            /* Bit 2 */
#define  TIM_DBL_3                       ((uint16_t)0x0800)            /* Bit 3 */
#define  TIM_DBL_4                       ((uint16_t)0x1000)            /* Bit 4 */

/*******************  Bit definition for TIM_DMAADR register  *******************/
#define  TIM_DMAR_DMAB                   ((uint16_t)0xFFFF)            /* DMA register for burst accesses */

/******************************************************************************/
/*         Universal Synchronous Asynchronous Receiver Transmitter            */
/******************************************************************************/

/*******************  Bit definition for USART_STATR register  *******************/
#define  USART_STATR_PE                        ((uint16_t)0x0001)            /* Parity Error */
#define  USART_STATR_FE                        ((uint16_t)0x0002)            /* Framing Error */
#define  USART_STATR_NE                        ((uint16_t)0x0004)            /* Noise Error Flag */
#define  USART_STATR_ORE                       ((uint16_t)0x0008)            /* OverRun Error */
#define  USART_STATR_IDLE                      ((uint16_t)0x0010)            /* IDLE line detected */
#define  USART_STATR_RXNE                      ((uint16_t)0x0020)            /* Read Data Register Not Empty */
#define  USART_STATR_TC                        ((uint16_t)0x0040)            /* Transmission Complete */
#define  USART_STATR_TXE                       ((uint16_t)0x0080)            /* Transmit Data Register Empty */
#define  USART_STATR_LBD                       ((uint16_t)0x0100)            /* LIN Break Detection Flag */
#define  USART_STATR_CTS                       ((uint16_t)0x0200)            /* CTS Flag */

/*******************  Bit definition for USART_DATAR register  *******************/
#define  USART_DATAR_DR                        ((uint16_t)0x01FF)            /* Data value */

/******************  Bit definition for USART_BRR register  *******************/
#define  USART_BRR_DIV_Fraction                ((uint16_t)0x000F)            /* Fraction of USARTDIV */
#define  USART_BRR_DIV_Mantissa                ((uint16_t)0xFFF0)            /* Mantissa of USARTDIV */

/******************  Bit definition for USART_CTLR1 register  *******************/
#define  USART_CTLR1_SBK                       ((uint16_t)0x0001)            /* Send Break */
#define  USART_CTLR1_RWU                       ((uint16_t)0x0002)            /* Receiver wakeup */
#define  USART_CTLR1_RE                        ((uint16_t)0x0004)            /* Receiver Enable */
#define  USART_CTLR1_TE                        ((uint16_t)0x0008)            /* Transmitter Enable */
#define  USART_CTLR1_IDLEIE                    ((uint16_t)0x0010)            /* IDLE Interrupt Enable */
#define  USART_CTLR1_RXNEIE                    ((uint16_t)0x0020)            /* RXNE Interrupt Enable */
#define  USART_CTLR1_TCIE                      ((uint16_t)0x0040)            /* Transmission Complete Interrupt Enable */
#define  USART_CTLR1_TXEIE                     ((uint16_t)0x0080)            /* PE Interrupt Enable */
#define  USART_CTLR1_PEIE                      ((uint16_t)0x0100)            /* PE Interrupt Enable */
#define  USART_CTLR1_PS                        ((uint16_t)0x0200)            /* Parity Selection */
#define  USART_CTLR1_PCE                       ((uint16_t)0x0400)            /* Parity Control Enable */
#define  USART_CTLR1_WAKE                      ((uint16_t)0x0800)            /* Wakeup method */
#define  USART_CTLR1_M                         ((uint16_t)0x1000)            /* Word length */
#define  USART_CTLR1_UE                        ((uint16_t)0x2000)            /* USART Enable */
#define  USART_CTLR1_OVER8                     ((uint16_t)0x8000)            /* USART Oversmapling 8-bits */

/******************  Bit definition for USART_CTLR2 register  *******************/
#define  USART_CTLR2_ADD                       ((uint16_t)0x000F)            /* Address of the USART node */
#define  USART_CTLR2_LBDL                      ((uint16_t)0x0020)            /* LIN Break Detection Length */
#define  USART_CTLR2_LBDIE                     ((uint16_t)0x0040)            /* LIN Break Detection Interrupt Enable */
#define  USART_CTLR2_LBCL                      ((uint16_t)0x0100)            /* Last Bit Clock pulse */
#define  USART_CTLR2_CPHA                      ((uint16_t)0x0200)            /* Clock Phase */
#define  USART_CTLR2_CPOL                      ((uint16_t)0x0400)            /* Clock Polarity */
#define  USART_CTLR2_CLKEN                     ((uint16_t)0x0800)            /* Clock Enable */

#define  USART_CTLR2_STOP                      ((uint16_t)0x3000)            /* STOP[1:0] bits (STOP bits) */
#define  USART_CTLR2_STOP_0                    ((uint16_t)0x1000)            /* Bit 0 */
#define  USART_CTLR2_STOP_1                    ((uint16_t)0x2000)            /* Bit 1 */

#define  USART_CTLR2_LINEN                     ((uint16_t)0x4000)            /* LIN mode enable */

/******************  Bit definition for USART_CTLR3 register  *******************/
#define  USART_CTLR3_EIE                       ((uint16_t)0x0001)            /* Error Interrupt Enable */
#define  USART_CTLR3_IREN                      ((uint16_t)0x0002)            /* IrDA mode Enable */
#define  USART_CTLR3_IRLP                      ((uint16_t)0x0004)            /* IrDA Low-Power */
#define  USART_CTLR3_HDSEL                     ((uint16_t)0x0008)            /* Half-Duplex Selection */
#define  USART_CTLR3_NACK                      ((uint16_t)0x0010)            /* Smartcard NACK enable */
#define  USART_CTLR3_SCEN                      ((uint16_t)0x0020)            /* Smartcard mode enable */
#define  USART_CTLR3_DMAR                      ((uint16_t)0x0040)            /* DMA Enable Receiver */
#define  USART_CTLR3_DMAT                      ((uint16_t)0x0080)            /* DMA Enable Transmitter */
#define  USART_CTLR3_RTSE                      ((uint16_t)0x0100)            /* RTS Enable */
#define  USART_CTLR3_CTSE                      ((uint16_t)0x0200)            /* CTS Enable */
#define  USART_CTLR3_CTSIE                     ((uint16_t)0x0400)            /* CTS Interrupt Enable */
#define  USART_CTLR3_ONEBIT                    ((uint16_t)0x0800)            /* One Bit method */

/******************  Bit definition for USART_GPR register  ******************/
#define  USART_GPR_PSC                         ((uint16_t)0x00FF)            /* PSC[7:0] bits (Prescaler value) */
#define  USART_GPR_PSC_0                       ((uint16_t)0x0001)            /* Bit 0 */
#define  USART_GPR_PSC_1                       ((uint16_t)0x0002)            /* Bit 1 */
#define  USART_GPR_PSC_2                       ((uint16_t)0x0004)            /* Bit 2 */
#define  USART_GPR_PSC_3                       ((uint16_t)0x0008)            /* Bit 3 */
#define  USART_GPR_PSC_4                       ((uint16_t)0x0010)            /* Bit 4 */
#define  USART_GPR_PSC_5                       ((uint16_t)0x0020)            /* Bit 5 */
#define  USART_GPR_PSC_6                       ((uint16_t)0x0040)            /* Bit 6 */
#define  USART_GPR_PSC_7                       ((uint16_t)0x0080)            /* Bit 7 */

#define  USART_GPR_GT                          ((uint16_t)0xFF00)            /* Guard time value */

/******************************************************************************/
/*                            Window WATCHDOG                                 */
/******************************************************************************/

/*******************  Bit definition for WWDG_CTLR register  ********************/
#define  WWDG_CTLR_T                           ((uint8_t)0x7F)               /* T[6:0] bits (7-Bit counter (MSB to LSB)) */
#define  WWDG_CTLR_T0                          ((uint8_t)0x01)               /* Bit 0 */
#define  WWDG_CTLR_T1                          ((uint8_t)0x02)               /* Bit 1 */
#define  WWDG_CTLR_T2                          ((uint8_t)0x04)               /* Bit 2 */
#define  WWDG_CTLR_T3                          ((uint8_t)0x08)               /* Bit 3 */
#define  WWDG_CTLR_T4                          ((uint8_t)0x10)               /* Bit 4 */
#define  WWDG_CTLR_T5                          ((uint8_t)0x20)               /* Bit 5 */
#define  WWDG_CTLR_T6                          ((uint8_t)0x40)               /* Bit 6 */

#define  WWDG_CTLR_WDGA                        ((uint8_t)0x80)               /* Activation bit */

/*******************  Bit definition for WWDG_CFGR register  *******************/
#define  WWDG_CFGR_W                           ((uint16_t)0x007F)            /* W[6:0] bits (7-bit window value) */
#define  WWDG_CFGR_W0                          ((uint16_t)0x0001)            /* Bit 0 */
#define  WWDG_CFGR_W1                          ((uint16_t)0x0002)            /* Bit 1 */
#define  WWDG_CFGR_W2                          ((uint16_t)0x0004)            /* Bit 2 */
#define  WWDG_CFGR_W3                          ((uint16_t)0x0008)            /* Bit 3 */
#define  WWDG_CFGR_W4                          ((uint16_t)0x0010)            /* Bit 4 */
#define  WWDG_CFGR_W5                          ((uint16_t)0x0020)            /* Bit 5 */
#define  WWDG_CFGR_W6                          ((uint16_t)0x0040)            /* Bit 6 */

#define  WWDG_CFGR_WDGTB                       ((uint16_t)0x0180)            /* WDGTB[1:0] bits (Timer Base) */
#define  WWDG_CFGR_WDGTB0                      ((uint16_t)0x0080)            /* Bit 0 */
#define  WWDG_CFGR_WDGTB1                      ((uint16_t)0x0100)            /* Bit 1 */

#define  WWDG_CFGR_EWI                         ((uint16_t)0x0200)            /* Early Wakeup Interrupt */

/*******************  Bit definition for WWDG_STATR register  ********************/
#define  WWDG_STATR_EWIF                       ((uint8_t)0x01)               /* Early Wakeup Interrupt Flag */

/******************************************************************************/
/*                          ENHANCED FUNNCTION                                */
/******************************************************************************/

/****************************  Enhanced register  *****************************/
#define  EXTEN_USBD_LS                         ((uint32_t)0x00000001)         /* Bit 0 */
#define  EXTEN_USBD_PU_EN                      ((uint32_t)0x00000002)         /* Bit 1 */
#define  EXTEN_ETH_10M_EN                      ((uint32_t)0x00000004)         /* Bit 2 */
#define  EXTEN_ETH_RGMII_SEL                   ((uint32_t)0x00000008)         /* Bit 3 */
#define  EXTEN_PLL_HSI_PRE                     ((uint32_t)0x00000010)         /* Bit 4 */
#define  EXTEN_LOCKUP_EN                       ((uint32_t)0x00000040)         /* Bit 5 */
#define  EXTEN_LOCKUP_RSTF                     ((uint32_t)0x00000080)         /* Bit 7 */

#define  EXTEN_ULLDO_TRIM                      ((uint32_t)0x00000300)         /* ULLDO_TRIM[1:0] bits */
#define  EXTEN_ULLDO_TRIM0                     ((uint32_t)0x00000100)         /* Bit 0 */
#define  EXTEN_ULLDO_TRIM1                     ((uint32_t)0x00000200)         /* Bit 1 */

#define  EXTEN_LDO_TRIM                        ((uint32_t)0x00000C00)         /* LDO_TRIM[1:0] bits */
#define  EXTEN_LDO_TRIM0                       ((uint32_t)0x00000400)         /* Bit 0 */
#define  EXTEN_LDO_TRIM1                       ((uint32_t)0x00000800)         /* Bit 1 */


/******************************************************************************/
/*                                  DVP                                       */
/******************************************************************************/

/*******************  Bit definition for DVP_CR0 register  ********************/
#define RB_DVP_ENABLE			0x01					// RW, DVP enable
#define RB_DVP_V_POLAR			0x02					// RW, DVP VSYNC polarity control: 1 = invert, 0 = not invert
#define	RB_DVP_H_POLAR			0x04					// RW, DVP HSYNC polarity control: 1 = invert, 0 = not invert
#define	RB_DVP_P_POLAR			0x08					// RW, DVP PCLK polarity control: 1 = invert, 0 = not invert
#define RB_DVP_MSK_DAT_MOD		0x30					
#define 	RB_DVP_D8_MOD			0x00				// RW, DVP 8bits data mode
#define		RB_DVP_D10_MOD			0x10				// RW, DVP 10bits data mode
#define		RB_DVP_D12_MOD			0x20				// RW, DVP 12bits data mode
#define	RB_DVP_JPEG				0x40					// RW, DVP JPEG mode

/*******************  Bit definition for DVP_CR1 register  ********************/
#define RB_DVP_DMA_EN			0x01					// RW, DVP dma enable
#define RB_DVP_ALL_CLR			0x02					// RW, DVP all clear, high action
#define	RB_DVP_RCV_CLR			0x04					// RW, DVP receive logic clear, high action
#define RB_DVP_BUF_TOG			0x08					// RW, DVP bug toggle by software, write 1 to toggle, ignored writing 0
#define RB_DVP_CM				0x10					// RW, DVP capture mode
#define	RB_DVP_CROP				0x20					// RW, DVP Crop feature enable
#define RB_DVP_FCRC				0xC0					// RW, DVP frame capture rate control: 
#define		DVP_RATE_100P		0x00					//00 = every frame captured (100%) 
#define		DVP_RATE_50P		0x40					//01 = every alternate frame captured (50%)
#define		DVP_RATE_25P		0x80					//10 = one frame in four frame captured (25%)

/*******************  Bit definition for DVP_IER register  ********************/
#define	RB_DVP_IE_STR_FRM		0x01					// RW, DVP frame start interrupt enable
#define	RB_DVP_IE_ROW_DONE		0x02					// RW, DVP row received done interrupt enable
#define RB_DVP_IE_FRM_DONE		0x04					// RW, DVP frame received done interrupt enable
#define	RB_DVP_IE_FIFO_OV		0x08					// RW, DVP receive fifo overflow interrupt enable	
#define RB_DVP_IE_STP_FRM		0x10					// RW, DVP frame stop interrupt enable				

/*******************  Bit definition for DVP_IFR register  ********************/
#define RB_DVP_IF_STR_FRM	    0x01				    // RW1, interrupt flag for DVP frame start
#define RB_DVP_IF_ROW_DONE		0x02				    // RW1, interrupt flag for DVP row receive done
#define RB_DVP_IF_FRM_DONE		0x04				    // RW1, interrupt flag for DVP frame receive done
#define RB_DVP_IF_FIFO_OV		0x08				    // RW1, interrupt flag for DVP receive fifo overflow
#define RB_DVP_IF_STP_FRM		0x10				    // RW1, interrupt flag for DVP frame stop

/*******************  Bit definition for DVP_STATUS register  ********************/
#define RB_DVP_FIFO_RDY			0x01					// RO, DVP receive fifo ready
#define RB_DVP_FIFO_FULL		0x02					// RO, DVP receive fifo full
#define RB_DVP_FIFO_OV			0x04					// RO, DVP receive fifo overflow
#define RB_DVP_MSK_FIFO_CNT		0x70					// RO, DVP receive fifo count



#include "ch32v30x_conf.h"


#ifdef __cplusplus
}
#endif

#endif 




