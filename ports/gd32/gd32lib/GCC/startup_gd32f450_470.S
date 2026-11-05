;/*!
;    \file    startup_gd32f450_470.S
;    \brief   start up file
;
;    \version 2025-07-31, V3.3.2, firmware for GD32F4xx
;*/
;
;/*
; * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
; * Copyright (c) 2026, GigaDevice Semiconductor Inc.
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the License); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an AS IS BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */
;/* This file refers the CMSIS standard, some adjustments are made according to GigaDevice chips */

  .syntax unified
  .cpu cortex-m4
  .fpu softvfp
  .thumb

.global  Default_Handler

/* necessary symbols defined in linker script to initialize data */
.word  _sidata
.word  _sdata
.word  _edata
.word  _sbss
.word  _ebss

  .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function

/* reset Handler */
Reset_Handler:
  movs r1, #0
  b DataInit

CopyData:
  ldr r3, =_sidata
  ldr r3, [r3, r1]
  str r3, [r0, r1]
  adds r1, r1, #4

DataInit:
  ldr r0, =_sdata
  ldr r3, =_edata
  adds r2, r0, r1
  cmp r2, r3
  bcc CopyData
  ldr r2, =_sbss
  b Zerobss

FillZerobss:
  movs r3, #0
  str r3, [r2], #4

Zerobss:
  ldr r3, = _ebss
  cmp r2, r3
  bcc FillZerobss
/* Call SystemInit function */
  bl  SystemInit
/* Call static constructors */
  bl __libc_init_array
/*Call the main function */
  bl main
  bx lr
.size Reset_Handler, .-Reset_Handler

    .section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b Infinite_Loop
  .size Default_Handler, .-Default_Handler

   .section  .vectors,"a",%progbits
   .global __gVectors

__gVectors:
                    .word _sp                                     /* Top of Stack */
                    .word Reset_Handler                           /* Reset Handler */
                    .word NMI_Handler                             /* NMI Handler */
                    .word HardFault_Handler                       /* Hard Fault Handler */
                    .word MemManage_Handler                       /* MPU Fault Handler */
                    .word BusFault_Handler                        /* Bus Fault Handler */
                    .word UsageFault_Handler                      /* Usage Fault Handler */
                    .word 0                                       /* Reserved */
                    .word 0                                       /* Reserved */
                    .word 0                                       /* Reserved */
                    .word 0                                       /* Reserved */
                    .word SVC_Handler                             /* SVCall Handler */
                    .word DebugMon_Handler                        /* Debug Monitor Handler */
                    .word 0                                       /* Reserved */
                    .word PendSV_Handler                          /* PendSV Handler */
                    .word SysTick_Handler                         /* SysTick Handler */

                    /* External interrupts handler */
                    .word WWDGT_IRQHandler                        /* Vector Number 16,Window Watchdog Timer */
                    .word LVD_IRQHandler                          /* Vector Number 17,LVD through EXTI Line detect */
                    .word TAMPER_STAMP_IRQHandler                 /* Vector Number 18,Tamper and TimeStamp through EXTI Line detect */
                    .word RTC_WKUP_IRQHandler                     /* Vector Number 19,RTC Wakeup through EXTI Line */
                    .word FMC_IRQHandler                          /* Vector Number 20,FMC */
                    .word RCU_CTC_IRQHandler                      /* Vector Number 21,RCU and CTC */
                    .word EXTI0_IRQHandler                        /* Vector Number 22,EXTI Line 0 */
                    .word EXTI1_IRQHandler                        /* Vector Number 23,EXTI Line 1 */
                    .word EXTI2_IRQHandler                        /* Vector Number 24,EXTI Line 2 */
                    .word EXTI3_IRQHandler                        /* Vector Number 25,EXTI Line 3 */
                    .word EXTI4_IRQHandler                        /* Vector Number 26,EXTI Line 4 */
                    .word DMA0_Channel0_IRQHandler                /* Vector Number 27,DMA0 Channel0 */
                    .word DMA0_Channel1_IRQHandler                /* Vector Number 28,DMA0 Channel1 */
                    .word DMA0_Channel2_IRQHandler                /* Vector Number 29,DMA0 Channel2 */
                    .word DMA0_Channel3_IRQHandler                /* Vector Number 30,DMA0 Channel3 */
                    .word DMA0_Channel4_IRQHandler                /* Vector Number 31,DMA0 Channel4 */
                    .word DMA0_Channel5_IRQHandler                /* Vector Number 32,DMA0 Channel5 */
                    .word DMA0_Channel6_IRQHandler                /* Vector Number 33,DMA0 Channel6 */
                    .word ADC_IRQHandler                          /* Vector Number 34,ADC */
                    .word CAN0_TX_IRQHandler                      /* Vector Number 35,CAN0 TX */
                    .word CAN0_RX0_IRQHandler                     /* Vector Number 36,CAN0 RX0 */
                    .word CAN0_RX1_IRQHandler                     /* Vector Number 37,CAN0 RX1 */
                    .word CAN0_EWMC_IRQHandler                    /* Vector Number 38,CAN0 EWMC */
                    .word EXTI5_9_IRQHandler                      /* Vector Number 39,EXTI5 to EXTI9 */
                    .word TIMER0_BRK_TIMER8_IRQHandler            /* Vector Number 40,TIMER0 Break and TIMER8 */
                    .word TIMER0_UP_TIMER9_IRQHandler             /* Vector Number 41,TIMER0 Update and TIMER9 */
                    .word TIMER0_TRG_CMT_TIMER10_IRQHandler       /* Vector Number 42,TIMER0 Trigger and Commutation and TIMER10 */
                    .word TIMER0_Channel_IRQHandler               /* Vector Number 43,TIMER0 Capture Compare */
                    .word TIMER1_IRQHandler                       /* Vector Number 44,TIMER1 */
                    .word TIMER2_IRQHandler                       /* Vector Number 45,TIMER2 */
                    .word TIMER3_IRQHandler                       /* Vector Number 46,TIMER3 */
                    .word I2C0_EV_IRQHandler                      /* Vector Number 47,I2C0 Event */
                    .word I2C0_ER_IRQHandler                      /* Vector Number 48,I2C0 Error */
                    .word I2C1_EV_IRQHandler                      /* Vector Number 49,I2C1 Event */
                    .word I2C1_ER_IRQHandler                      /* Vector Number 50,I2C1 Error */
                    .word SPI0_IRQHandler                         /* Vector Number 51,SPI0 */
                    .word SPI1_IRQHandler                         /* Vector Number 52,SPI1 */
                    .word USART0_IRQHandler                       /* Vector Number 53,USART0 */
                    .word USART1_IRQHandler                       /* Vector Number 54,USART1 */
                    .word USART2_IRQHandler                       /* Vector Number 55,USART2 */
                    .word EXTI10_15_IRQHandler                    /* Vector Number 56,EXTI10 to EXTI15 */
                    .word RTC_Alarm_IRQHandler                    /* Vector Number 57,RTC Alarm */
                    .word USBFS_WKUP_IRQHandler                   /* Vector Number 58,USBFS Wakeup */
                    .word TIMER7_BRK_TIMER11_IRQHandler           /* Vector Number 59,TIMER7 Break and TIMER11 */
                    .word TIMER7_UP_TIMER12_IRQHandler            /* Vector Number 60,TIMER7 Update and TIMER12 */
                    .word TIMER7_TRG_CMT_TIMER13_IRQHandler       /* Vector Number 61,TIMER7 Trigger and Commutation and TIMER13 */
                    .word TIMER7_Channel_IRQHandler               /* Vector Number 62,TIMER7 Channel Capture Compare */
                    .word DMA0_Channel7_IRQHandler                /* Vector Number 63,DMA0 Channel7 */
                    .word EXMC_IRQHandler                         /* Vector Number 64,EXMC */
                    .word SDIO_IRQHandler                         /* Vector Number 65,SDIO */
                    .word TIMER4_IRQHandler                       /* Vector Number 66,TIMER4 */
                    .word SPI2_IRQHandler                         /* Vector Number 67,SPI2 */
                    .word UART3_IRQHandler                        /* Vector Number 68,UART3 */
                    .word UART4_IRQHandler                        /* Vector Number 69,UART4 */
                    .word TIMER5_DAC_IRQHandler                   /* Vector Number 70,TIMER5 and DAC0 DAC1 Underrun error */
                    .word TIMER6_IRQHandler                       /* Vector Number 71,TIMER6 */
                    .word DMA1_Channel0_IRQHandler                /* Vector Number 72,DMA1 Channel0 */
                    .word DMA1_Channel1_IRQHandler                /* Vector Number 73,DMA1 Channel1 */
                    .word DMA1_Channel2_IRQHandler                /* Vector Number 74,DMA1 Channel2 */
                    .word DMA1_Channel3_IRQHandler                /* Vector Number 75,DMA1 Channel3 */
                    .word DMA1_Channel4_IRQHandler                /* Vector Number 76,DMA1 Channel4 */
                    .word ENET_IRQHandler                         /* Vector Number 77,Ethernet */
                    .word ENET_WKUP_IRQHandler                    /* Vector Number 78,Ethernet Wakeup through EXTI Line */
                    .word CAN1_TX_IRQHandler                      /* Vector Number 79,CAN1 TX */
                    .word CAN1_RX0_IRQHandler                     /* Vector Number 80,CAN1 RX0 */
                    .word CAN1_RX1_IRQHandler                     /* Vector Number 81,CAN1 RX1 */
                    .word CAN1_EWMC_IRQHandler                    /* Vector Number 82,CAN1 EWMC */
                    .word USBFS_IRQHandler                        /* Vector Number 83,USBFS */
                    .word DMA1_Channel5_IRQHandler                /* Vector Number 84,DMA1 Channel5 */
                    .word DMA1_Channel6_IRQHandler                /* Vector Number 85,DMA1 Channel6 */
                    .word DMA1_Channel7_IRQHandler                /* Vector Number 86,DMA1 Channel7 */
                    .word USART5_IRQHandler                       /* Vector Number 87,USART5 */
                    .word I2C2_EV_IRQHandler                      /* Vector Number 88,I2C2 Event */
                    .word I2C2_ER_IRQHandler                      /* Vector Number 89,I2C2 Error */
                    .word USBHS_EP1_Out_IRQHandler                /* Vector Number 90,USBHS Endpoint 1 Out */
                    .word USBHS_EP1_In_IRQHandler                 /* Vector Number 91,USBHS Endpoint 1 in */
                    .word USBHS_WKUP_IRQHandler                   /* Vector Number 92,USBHS Wakeup through EXTI Line */
                    .word USBHS_IRQHandler                        /* Vector Number 93,USBHS */
                    .word DCI_IRQHandler                          /* Vector Number 94,DCI */
                    .word 0                                       /* Vector Number 95,Reserved */
                    .word TRNG_IRQHandler                         /* Vector Number 96,TRNG */
                    .word FPU_IRQHandler                          /* Vector Number 97,FPU */
                    .word UART6_IRQHandler                        /* Vector Number 98,UART6 */
                    .word UART7_IRQHandler                        /* Vector Number 99,UART7 */
                    .word SPI3_IRQHandler                         /* Vector Number 100,SPI3 */
                    .word SPI4_IRQHandler                         /* Vector Number 101,SPI4 */
                    .word SPI5_IRQHandler                         /* Vector Number 102,SPI5 */
                    .word 0                                       /* Vector Number 103,Reserved */
                    .word TLI_IRQHandler                          /* Vector Number 104,TLI */
                    .word TLI_ER_IRQHandler                       /* Vector Number 105,TLI Error */
                    .word IPA_IRQHandler                          /* Vector Number 106,IPA */

  .size   __gVectors, . - __gVectors

  .weak NMI_Handler
  .thumb_set NMI_Handler,Default_Handler

  .weak HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler

  .weak MemManage_Handler
  .thumb_set MemManage_Handler,Default_Handler

  .weak BusFault_Handler
  .thumb_set BusFault_Handler,Default_Handler

  .weak UsageFault_Handler
  .thumb_set UsageFault_Handler,Default_Handler

  .weak SVC_Handler
  .thumb_set SVC_Handler,Default_Handler

  .weak DebugMon_Handler
  .thumb_set DebugMon_Handler,Default_Handler

  .weak PendSV_Handler
  .thumb_set PendSV_Handler,Default_Handler

  .weak SysTick_Handler
  .thumb_set SysTick_Handler,Default_Handler

  .weak WWDGT_IRQHandler
  .thumb_set WWDGT_IRQHandler,Default_Handler

  .weak LVD_IRQHandler
  .thumb_set LVD_IRQHandler,Default_Handler

  .weak TAMPER_STAMP_IRQHandler
  .thumb_set TAMPER_STAMP_IRQHandler,Default_Handler

  .weak RTC_WKUP_IRQHandler
  .thumb_set RTC_WKUP_IRQHandler,Default_Handler

  .weak FMC_IRQHandler
  .thumb_set FMC_IRQHandler,Default_Handler

  .weak RCU_CTC_IRQHandler
  .thumb_set RCU_CTC_IRQHandler,Default_Handler

  .weak EXTI0_IRQHandler
  .thumb_set EXTI0_IRQHandler,Default_Handler

  .weak EXTI1_IRQHandler
  .thumb_set EXTI1_IRQHandler,Default_Handler

  .weak EXTI2_IRQHandler
  .thumb_set EXTI2_IRQHandler,Default_Handler

  .weak EXTI3_IRQHandler
  .thumb_set EXTI3_IRQHandler,Default_Handler

  .weak EXTI4_IRQHandler
  .thumb_set EXTI4_IRQHandler,Default_Handler

  .weak DMA0_Channel0_IRQHandler
  .thumb_set DMA0_Channel0_IRQHandler,Default_Handler

  .weak DMA0_Channel1_IRQHandler
  .thumb_set DMA0_Channel1_IRQHandler,Default_Handler

  .weak DMA0_Channel2_IRQHandler
  .thumb_set DMA0_Channel2_IRQHandler,Default_Handler

  .weak DMA0_Channel3_IRQHandler
  .thumb_set DMA0_Channel3_IRQHandler,Default_Handler

  .weak DMA0_Channel4_IRQHandler
  .thumb_set DMA0_Channel4_IRQHandler,Default_Handler

  .weak DMA0_Channel5_IRQHandler
  .thumb_set DMA0_Channel5_IRQHandler,Default_Handler

  .weak DMA0_Channel6_IRQHandler
  .thumb_set DMA0_Channel6_IRQHandler,Default_Handler

  .weak ADC_IRQHandler
  .thumb_set ADC_IRQHandler,Default_Handler

  .weak CAN0_TX_IRQHandler
  .thumb_set CAN0_TX_IRQHandler,Default_Handler

  .weak CAN0_RX0_IRQHandler
  .thumb_set CAN0_RX0_IRQHandler,Default_Handler

  .weak CAN0_RX1_IRQHandler
  .thumb_set CAN0_RX1_IRQHandler,Default_Handler

  .weak CAN0_EWMC_IRQHandler
  .thumb_set CAN0_EWMC_IRQHandler,Default_Handler

  .weak EXTI5_9_IRQHandler
  .thumb_set EXTI5_9_IRQHandler,Default_Handler

  .weak TIMER0_BRK_TIMER8_IRQHandler
  .thumb_set TIMER0_BRK_TIMER8_IRQHandler,Default_Handler

  .weak TIMER0_UP_TIMER9_IRQHandler
  .thumb_set TIMER0_UP_TIMER9_IRQHandler,Default_Handler

  .weak TIMER0_TRG_CMT_TIMER10_IRQHandler
  .thumb_set TIMER0_TRG_CMT_TIMER10_IRQHandler,Default_Handler

  .weak TIMER0_Channel_IRQHandler
  .thumb_set TIMER0_Channel_IRQHandler,Default_Handler

  .weak TIMER1_IRQHandler
  .thumb_set TIMER1_IRQHandler,Default_Handler

  .weak TIMER2_IRQHandler
  .thumb_set TIMER2_IRQHandler,Default_Handler

  .weak TIMER3_IRQHandler
  .thumb_set TIMER3_IRQHandler,Default_Handler

  .weak I2C0_EV_IRQHandler
  .thumb_set I2C0_EV_IRQHandler,Default_Handler

  .weak I2C0_ER_IRQHandler
  .thumb_set I2C0_ER_IRQHandler,Default_Handler

  .weak I2C1_EV_IRQHandler
  .thumb_set I2C1_EV_IRQHandler,Default_Handler

  .weak I2C1_ER_IRQHandler
  .thumb_set I2C1_ER_IRQHandler,Default_Handler

  .weak SPI0_IRQHandler
  .thumb_set SPI0_IRQHandler,Default_Handler

  .weak SPI1_IRQHandler
  .thumb_set SPI1_IRQHandler,Default_Handler

  .weak USART0_IRQHandler
  .thumb_set USART0_IRQHandler,Default_Handler

  .weak USART1_IRQHandler
  .thumb_set USART1_IRQHandler,Default_Handler

  .weak USART2_IRQHandler
  .thumb_set USART2_IRQHandler,Default_Handler

  .weak EXTI10_15_IRQHandler
  .thumb_set EXTI10_15_IRQHandler,Default_Handler

  .weak RTC_Alarm_IRQHandler
  .thumb_set RTC_Alarm_IRQHandler,Default_Handler

  .weak USBFS_WKUP_IRQHandler
  .thumb_set USBFS_WKUP_IRQHandler,Default_Handler

  .weak TIMER7_BRK_TIMER11_IRQHandler
  .thumb_set TIMER7_BRK_TIMER11_IRQHandler,Default_Handler

  .weak TIMER7_UP_TIMER12_IRQHandler
  .thumb_set TIMER7_UP_TIMER12_IRQHandler,Default_Handler

  .weak TIMER7_TRG_CMT_TIMER13_IRQHandler
  .thumb_set TIMER7_TRG_CMT_TIMER13_IRQHandler,Default_Handler

  .weak TIMER7_Channel_IRQHandler
  .thumb_set TIMER7_Channel_IRQHandler,Default_Handler

  .weak DMA0_Channel7_IRQHandler
  .thumb_set DMA0_Channel7_IRQHandler,Default_Handler

  .weak EXMC_IRQHandler
  .thumb_set EXMC_IRQHandler,Default_Handler

  .weak SDIO_IRQHandler
  .thumb_set SDIO_IRQHandler,Default_Handler

  .weak TIMER4_IRQHandler
  .thumb_set TIMER4_IRQHandler,Default_Handler

  .weak SPI2_IRQHandler
  .thumb_set SPI2_IRQHandler,Default_Handler

  .weak UART3_IRQHandler
  .thumb_set UART3_IRQHandler,Default_Handler

  .weak UART4_IRQHandler
  .thumb_set UART4_IRQHandler,Default_Handler

  .weak TIMER5_DAC_IRQHandler
  .thumb_set TIMER5_DAC_IRQHandler,Default_Handler

  .weak TIMER6_IRQHandler
  .thumb_set TIMER6_IRQHandler,Default_Handler

  .weak DMA1_Channel0_IRQHandler
  .thumb_set DMA1_Channel0_IRQHandler,Default_Handler

  .weak DMA1_Channel1_IRQHandler
  .thumb_set DMA1_Channel1_IRQHandler,Default_Handler

  .weak DMA1_Channel2_IRQHandler
  .thumb_set DMA1_Channel2_IRQHandler,Default_Handler

  .weak DMA1_Channel3_IRQHandler
  .thumb_set DMA1_Channel3_IRQHandler,Default_Handler

  .weak DMA1_Channel4_IRQHandler
  .thumb_set DMA1_Channel4_IRQHandler,Default_Handler

  .weak ENET_IRQHandler
  .thumb_set ENET_IRQHandler,Default_Handler

  .weak ENET_WKUP_IRQHandler
  .thumb_set ENET_WKUP_IRQHandler,Default_Handler

  .weak CAN1_TX_IRQHandler
  .thumb_set CAN1_TX_IRQHandler,Default_Handler

  .weak CAN1_RX0_IRQHandler
  .thumb_set CAN1_RX0_IRQHandler,Default_Handler

  .weak CAN1_RX1_IRQHandler
  .thumb_set CAN1_RX1_IRQHandler,Default_Handler

  .weak CAN1_EWMC_IRQHandler
  .thumb_set CAN1_EWMC_IRQHandler,Default_Handler

  .weak USBFS_IRQHandler
  .thumb_set USBFS_IRQHandler,Default_Handler

  .weak DMA1_Channel5_IRQHandler
  .thumb_set DMA1_Channel5_IRQHandler,Default_Handler

  .weak DMA1_Channel6_IRQHandler
  .thumb_set DMA1_Channel6_IRQHandler,Default_Handler

  .weak DMA1_Channel7_IRQHandler
  .thumb_set DMA1_Channel7_IRQHandler,Default_Handler

  .weak USART5_IRQHandler
  .thumb_set USART5_IRQHandler,Default_Handler

  .weak I2C2_EV_IRQHandler
  .thumb_set I2C2_EV_IRQHandler,Default_Handler

  .weak I2C2_ER_IRQHandler
  .thumb_set I2C2_ER_IRQHandler,Default_Handler

  .weak USBHS_EP1_Out_IRQHandler
  .thumb_set USBHS_EP1_Out_IRQHandler,Default_Handler

  .weak USBHS_EP1_In_IRQHandler
  .thumb_set USBHS_EP1_In_IRQHandler,Default_Handler

  .weak USBHS_WKUP_IRQHandler
  .thumb_set USBHS_WKUP_IRQHandler,Default_Handler

  .weak USBHS_IRQHandler
  .thumb_set USBHS_IRQHandler,Default_Handler

  .weak DCI_IRQHandler
  .thumb_set DCI_IRQHandler,Default_Handler

  .weak TRNG_IRQHandler
  .thumb_set TRNG_IRQHandler,Default_Handler

  .weak FPU_IRQHandler
  .thumb_set FPU_IRQHandler,Default_Handler

  .weak UART6_IRQHandler
  .thumb_set UART6_IRQHandler,Default_Handler

  .weak UART7_IRQHandler
  .thumb_set UART7_IRQHandler,Default_Handler

  .weak SPI3_IRQHandler
  .thumb_set SPI3_IRQHandler,Default_Handler

  .weak SPI4_IRQHandler
  .thumb_set SPI4_IRQHandler,Default_Handler

  .weak SPI5_IRQHandler
  .thumb_set SPI5_IRQHandler,Default_Handler

  .weak TLI_IRQHandler
  .thumb_set TLI_IRQHandler,Default_Handler

  .weak TLI_ER_IRQHandler
  .thumb_set TLI_ER_IRQHandler,Default_Handler

  .weak IPA_IRQHandler
  .thumb_set IPA_IRQHandler,Default_Handler
