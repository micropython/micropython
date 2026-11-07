/*!
    \file    gd32f4xx.h
    \brief   general definitions for GD32F4xx
    
    \version 2026-02-05, V3.3.3, firmware for GD32F4xx  
*/

/* Copyright (c) 2012 ARM LIMITED
   Copyright (c) 2026, GigaDevice Semiconductor Inc.

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

/* This file refers the CMSIS standard, some adjustments are made according to GigaDevice chips */

#ifndef GD32F4XX_H
#define GD32F4XX_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* define GD32F4xx */
#if !defined (GD32F450)  && !defined (GD32F405) && !defined (GD32F407) && !defined (GD32F470)  && !defined (GD32F425) && !defined (GD32F427)
  /* #define GD32F450 */
  /* #define GD32F405 */
  /* #define GD32F407 */
  /* #define GD32F470 */
  /* #define GD32F425 */
  /* #define GD32F427 */
#endif /* define GD32F4xx */
   
#if !defined (GD32F450)  && !defined (GD32F405) && !defined (GD32F407) && !defined (GD32F470)  && !defined (GD32F425) && !defined (GD32F427)
 #error "Please select the target GD32F4xx device in gd32f4xx.h file"
#endif /* undefine GD32F4xx tip */

/* define value of high speed crystal oscillator (HXTAL) in Hz */
#if !defined  (HXTAL_VALUE)
#define HXTAL_VALUE    ((uint32_t)25000000)
#endif /* high speed crystal oscillator value */

/* define startup timeout value of high speed crystal oscillator (HXTAL) */
#if !defined  (HXTAL_STARTUP_TIMEOUT)
#define HXTAL_STARTUP_TIMEOUT   ((uint16_t)0xFFFF)
#endif /* high speed crystal oscillator startup timeout */

/* define value of internal 16MHz RC oscillator (IRC16M) in Hz */
#if !defined  (IRC16M_VALUE) 
#define IRC16M_VALUE  ((uint32_t)16000000)
#endif /* internal 16MHz RC oscillator value */

/* define startup timeout value of internal 16MHz RC oscillator (IRC16M) */
#if !defined  (IRC16M_STARTUP_TIMEOUT)
#define IRC16M_STARTUP_TIMEOUT   ((uint16_t)0x0500)
#endif /* internal 16MHz RC oscillator startup timeout */

/* define value of internal 32KHz RC oscillator(IRC32K) in Hz */
#if !defined  (IRC32K_VALUE) 
#define IRC32K_VALUE  ((uint32_t)32000)
#endif /* internal 32KHz RC oscillator value */

/* define value of low speed crystal oscillator (LXTAL)in Hz */
#if !defined  (LXTAL_VALUE) 
#define LXTAL_VALUE  ((uint32_t)32768)
#endif /* low speed crystal oscillator value */

/* I2S external clock in selection */
//#define I2S_EXTERNAL_CLOCK_IN          (uint32_t)12288000U

/* GD32F4xx firmware library version number V1.0 */
#define __GD32F4xx_STDPERIPH_VERSION_MAIN   (0x03) /*!< [31:24] main version     */
#define __GD32F4xx_STDPERIPH_VERSION_SUB1   (0x03) /*!< [23:16] sub1 version     */
#define __GD32F4xx_STDPERIPH_VERSION_SUB2   (0x03) /*!< [15:8]  sub2 version     */
#define __GD32F4xx_STDPERIPH_VERSION_RC     (0x00) /*!< [7:0]  release candidate */ 
#define __GD32F4xx_STDPERIPH_VERSION        ((__GD32F4xx_STDPERIPH_VERSION_MAIN << 24)\
                                            |(__GD32F4xx_STDPERIPH_VERSION_SUB1 << 16)\
                                            |(__GD32F4xx_STDPERIPH_VERSION_SUB2 << 8)\
                                            |(__GD32F4xx_STDPERIPH_VERSION_RC))

/* configuration of the cortex-M4 processor and core peripherals */
#define __CM4_REV                 0x0001   /*!< core revision r0p1                                       */
#define __MPU_PRESENT             1        /*!< GD32F4xx provide MPU                                     */
#define __NVIC_PRIO_BITS          4        /*!< GD32F4xx uses 4 bits for the priority levels             */
#define __Vendor_SysTickConfig    0        /*!< set to 1 if different sysTick config is used             */
#define __FPU_PRESENT             1        /*!< FPU present                                              */
/* define interrupt number */
typedef enum IRQn
{
    /* cortex-M4 processor exceptions numbers */
    NonMaskableInt_IRQn          = -14,    /*!< non maskable interrupt                                   */
    HardFault_IRQn               = -13,    /*!< hard-fault interrupt                                     */
    MemoryManagement_IRQn        = -12,    /*!< 4 cortex-M4 memory management interrupt                  */
    BusFault_IRQn                = -11,    /*!< 5 cortex-M4 bus fault interrupt                          */
    UsageFault_IRQn              = -10,    /*!< 6 cortex-M4 usage fault interrupt                        */
    SVCall_IRQn                  = -5,     /*!< 11 cortex-M4 SV call interrupt                           */
    DebugMonitor_IRQn            = -4,     /*!< 12 cortex-M4 debug monitor interrupt                     */
    PendSV_IRQn                  = -2,     /*!< 14 cortex-M4 pend SV interrupt                           */
    SysTick_IRQn                 = -1,     /*!< 15 cortex-M4 system tick interrupt                       */
    /* interruput numbers */
    WWDGT_IRQn                   = 0,      /*!< window watchdog timer interrupt                          */
    LVD_IRQn                     = 1,      /*!< LVD through EXTI line detect interrupt                   */
    TAMPER_STAMP_IRQn            = 2,      /*!< tamper and timestamp through EXTI line detect            */
    RTC_WKUP_IRQn                = 3,      /*!< RTC wakeup through EXTI line interrupt                   */
    FMC_IRQn                     = 4,      /*!< FMC interrupt                                            */
    RCU_CTC_IRQn                 = 5,      /*!< RCU and CTC interrupt                                    */
    EXTI0_IRQn                   = 6,      /*!< EXTI line 0 interrupts                                   */
    EXTI1_IRQn                   = 7,      /*!< EXTI line 1 interrupts                                   */
    EXTI2_IRQn                   = 8,      /*!< EXTI line 2 interrupts                                   */
    EXTI3_IRQn                   = 9,      /*!< EXTI line 3 interrupts                                   */
    EXTI4_IRQn                   = 10,     /*!< EXTI line 4 interrupts                                   */
    DMA0_Channel0_IRQn           = 11,     /*!< DMA0 channel0 Interrupt                                  */
    DMA0_Channel1_IRQn           = 12,     /*!< DMA0 channel1 Interrupt                                  */
    DMA0_Channel2_IRQn           = 13,     /*!< DMA0 channel2 interrupt                                  */
    DMA0_Channel3_IRQn           = 14,     /*!< DMA0 channel3 interrupt                                  */
    DMA0_Channel4_IRQn           = 15,     /*!< DMA0 channel4 interrupt                                  */
    DMA0_Channel5_IRQn           = 16,     /*!< DMA0 channel5 interrupt                                  */
    DMA0_Channel6_IRQn           = 17,     /*!< DMA0 channel6 interrupt                                  */
    ADC_IRQn                     = 18,     /*!< ADC interrupt                                            */
    CAN0_TX_IRQn                 = 19,     /*!< CAN0 TX interrupt                                        */
    CAN0_RX0_IRQn                = 20,     /*!< CAN0 RX0 interrupt                                       */
    CAN0_RX1_IRQn                = 21,     /*!< CAN0 RX1 interrupt                                       */
    CAN0_EWMC_IRQn               = 22,     /*!< CAN0 EWMC interrupt                                      */
    EXTI5_9_IRQn                 = 23,     /*!< EXTI[9:5] interrupts                                     */
    TIMER0_BRK_TIMER8_IRQn       = 24,     /*!< TIMER0 break and TIMER8 interrupts                       */
    TIMER0_UP_TIMER9_IRQn        = 25,     /*!< TIMER0 update and TIMER9 interrupts                      */
    TIMER0_TRG_CMT_TIMER10_IRQn  = 26,     /*!< TIMER0 trigger and commutation  and TIMER10 interrupts   */
    TIMER0_Channel_IRQn          = 27,     /*!< TIMER0 channel capture compare interrupt                 */
    TIMER1_IRQn                  = 28,     /*!< TIMER1 interrupt                                         */
    TIMER2_IRQn                  = 29,     /*!< TIMER2 interrupt                                         */
    TIMER3_IRQn                  = 30,     /*!< TIMER3 interrupts                                        */
    I2C0_EV_IRQn                 = 31,     /*!< I2C0 event interrupt                                     */
    I2C0_ER_IRQn                 = 32,     /*!< I2C0 error interrupt                                     */
    I2C1_EV_IRQn                 = 33,     /*!< I2C1 event interrupt                                     */
    I2C1_ER_IRQn                 = 34,     /*!< I2C1 error interrupt                                     */
    SPI0_IRQn                    = 35,     /*!< SPI0 interrupt                                           */
    SPI1_IRQn                    = 36,     /*!< SPI1 interrupt                                           */
    USART0_IRQn                  = 37,     /*!< USART0 interrupt                                         */
    USART1_IRQn                  = 38,     /*!< USART1 interrupt                                         */
    USART2_IRQn                  = 39,     /*!< USART2 interrupt                                         */
    EXTI10_15_IRQn               = 40,     /*!< EXTI[15:10] interrupts                                   */
    RTC_Alarm_IRQn               = 41,     /*!< RTC alarm interrupt                                      */
    USBFS_WKUP_IRQn              = 42,     /*!< USBFS wakeup interrupt                                   */
    TIMER7_BRK_TIMER11_IRQn      = 43,     /*!< TIMER7 break and TIMER11 interrupts                      */
    TIMER7_UP_TIMER12_IRQn       = 44,     /*!< TIMER7 update and TIMER12 interrupts                     */
    TIMER7_TRG_CMT_TIMER13_IRQn  = 45,     /*!< TIMER7 trigger and commutation and TIMER13 interrupts    */
    TIMER7_Channel_IRQn          = 46,     /*!< TIMER7 channel capture compare interrupt                 */
    DMA0_Channel7_IRQn           = 47,     /*!< DMA0 channel7 interrupt                                  */
    
#if defined (GD32F450) || defined (GD32F470)
    EXMC_IRQn                    = 48,     /*!< EXMC interrupt                                           */
    SDIO_IRQn                    = 49,     /*!< SDIO interrupt                                           */
    TIMER4_IRQn                  = 50,     /*!< TIMER4 interrupt                                         */
    SPI2_IRQn                    = 51,     /*!< SPI2 interrupt                                           */
    UART3_IRQn                   = 52,     /*!< UART3 interrupt                                          */
    UART4_IRQn                   = 53,     /*!< UART4 interrupt                                          */
    TIMER5_DAC_IRQn              = 54,     /*!< TIMER5 and DAC0 DAC1 underrun error interrupts           */
    TIMER6_IRQn                  = 55,     /*!< TIMER6 interrupt                                         */
    DMA1_Channel0_IRQn           = 56,     /*!< DMA1 channel0 interrupt                                  */
    DMA1_Channel1_IRQn           = 57,     /*!< DMA1 channel1 interrupt                                  */
    DMA1_Channel2_IRQn           = 58,     /*!< DMA1 channel2 interrupt                                  */
    DMA1_Channel3_IRQn           = 59,     /*!< DMA1 channel3 interrupt                                  */
    DMA1_Channel4_IRQn           = 60,     /*!< DMA1 channel4 interrupt                                  */
    ENET_IRQn                    = 61,     /*!< ENET interrupt                                           */
    ENET_WKUP_IRQn               = 62,     /*!< ENET wakeup through EXTI line interrupt                  */
    CAN1_TX_IRQn                 = 63,     /*!< CAN1 TX interrupt                                        */
    CAN1_RX0_IRQn                = 64,     /*!< CAN1 RX0 interrupt                                       */
    CAN1_RX1_IRQn                = 65,     /*!< CAN1 RX1 interrupt                                       */
    CAN1_EWMC_IRQn               = 66,     /*!< CAN1 EWMC interrupt                                      */
    USBFS_IRQn                   = 67,     /*!< USBFS interrupt                                          */
    DMA1_Channel5_IRQn           = 68,     /*!< DMA1 channel5 interrupt                                  */
    DMA1_Channel6_IRQn           = 69,     /*!< DMA1 channel6 interrupt                                  */
    DMA1_Channel7_IRQn           = 70,     /*!< DMA1 channel7 interrupt                                  */
    USART5_IRQn                  = 71,     /*!< USART5 interrupt                                         */
    I2C2_EV_IRQn                 = 72,     /*!< I2C2 event interrupt                                     */
    I2C2_ER_IRQn                 = 73,     /*!< I2C2 error interrupt                                     */
    USBHS_EP1_Out_IRQn           = 74,     /*!< USBHS endpoint 1 out interrupt                           */
    USBHS_EP1_In_IRQn            = 75,     /*!< USBHS endpoint 1 in interrupt                            */
    USBHS_WKUP_IRQn              = 76,     /*!< USBHS wakeup through EXTI line interrupt                 */
    USBHS_IRQn                   = 77,     /*!< USBHS interrupt                                          */
    DCI_IRQn                     = 78,     /*!< DCI interrupt                                            */
    TRNG_IRQn                    = 80,     /*!< TRNG interrupt                                           */
    FPU_IRQn                     = 81,     /*!< FPU interrupt                                            */
    UART6_IRQn                   = 82,     /*!< UART6 interrupt                                          */
    UART7_IRQn                   = 83,     /*!< UART7 interrupt                                          */
    SPI3_IRQn                    = 84,     /*!< SPI3 interrupt                                           */
    SPI4_IRQn                    = 85,     /*!< SPI4 interrupt                                           */
    SPI5_IRQn                    = 86,     /*!< SPI5 interrupt                                           */
    TLI_IRQn                     = 88,     /*!< TLI interrupt                                            */
    TLI_ER_IRQn                  = 89,     /*!< TLI error interrupt                                      */
    IPA_IRQn                     = 90,     /*!< IPA interrupt                                            */
#endif /* GD32F450 and GD32F470 */

#if defined (GD32F405) || defined (GD32F425)
    SDIO_IRQn                    = 49,     /*!< SDIO interrupt                                           */
    TIMER4_IRQn                  = 50,     /*!< TIMER4 interrupt                                         */
    SPI2_IRQn                    = 51,     /*!< SPI2 interrupt                                           */
    UART3_IRQn                   = 52,     /*!< UART3 interrupt                                          */
    UART4_IRQn                   = 53,     /*!< UART4 interrupt                                          */
    TIMER5_DAC_IRQn              = 54,     /*!< TIMER5 and DAC0 DAC1 underrun error interrupts           */
    TIMER6_IRQn                  = 55,     /*!< TIMER6 interrupt                                         */
    DMA1_Channel0_IRQn           = 56,     /*!< DMA1 channel0 interrupt                                  */
    DMA1_Channel1_IRQn           = 57,     /*!< DMA1 channel1 interrupt                                  */
    DMA1_Channel2_IRQn           = 58,     /*!< DMA1 channel2 interrupt                                  */
    DMA1_Channel3_IRQn           = 59,     /*!< DMA1 channel3 interrupt                                  */
    DMA1_Channel4_IRQn           = 60,     /*!< DMA1 channel4 interrupt                                  */
    CAN1_TX_IRQn                 = 63,     /*!< CAN1 TX interrupt                                        */
    CAN1_RX0_IRQn                = 64,     /*!< CAN1 RX0 interrupt                                       */
    CAN1_RX1_IRQn                = 65,     /*!< CAN1 RX1 interrupt                                       */
    CAN1_EWMC_IRQn               = 66,     /*!< CAN1 EWMC interrupt                                      */
    USBFS_IRQn                   = 67,     /*!< USBFS interrupt                                          */
    DMA1_Channel5_IRQn           = 68,     /*!< DMA1 channel5 interrupt                                  */
    DMA1_Channel6_IRQn           = 69,     /*!< DMA1 channel6 interrupt                                  */
    DMA1_Channel7_IRQn           = 70,     /*!< DMA1 channel7 interrupt                                  */
    USART5_IRQn                  = 71,     /*!< USART5 interrupt                                         */
    I2C2_EV_IRQn                 = 72,     /*!< I2C2 event interrupt                                     */
    I2C2_ER_IRQn                 = 73,     /*!< I2C2 error interrupt                                     */
    USBHS_EP1_Out_IRQn           = 74,     /*!< USBHS endpoint 1 Out interrupt                           */
    USBHS_EP1_In_IRQn            = 75,     /*!< USBHS endpoint 1 in interrupt                            */
    USBHS_WKUP_IRQn              = 76,     /*!< USBHS wakeup through EXTI line interrupt                 */
    USBHS_IRQn                   = 77,     /*!< USBHS interrupt                                          */
    DCI_IRQn                     = 78,     /*!< DCI interrupt                                            */
    TRNG_IRQn                    = 80,     /*!< TRNG interrupt                                           */
    FPU_IRQn                     = 81,     /*!< FPU interrupt                                            */
#endif /* GD32F405 and GD32F425 */

#if defined (GD32F407) || defined (GD32F427)
    EXMC_IRQn                    = 48,     /*!< EXMC interrupt                                           */
    SDIO_IRQn                    = 49,     /*!< SDIO interrupt                                           */
    TIMER4_IRQn                  = 50,     /*!< TIMER4 interrupt                                         */
    SPI2_IRQn                    = 51,     /*!< SPI2 interrupt                                           */
    UART3_IRQn                   = 52,     /*!< UART3 interrupt                                          */
    UART4_IRQn                   = 53,     /*!< UART4 interrupt                                          */
    TIMER5_DAC_IRQn              = 54,     /*!< TIMER5 and DAC0 DAC1 underrun error interrupts           */
    TIMER6_IRQn                  = 55,     /*!< TIMER6 interrupt                                         */
    DMA1_Channel0_IRQn           = 56,     /*!< DMA1 channel0 interrupt                                  */
    DMA1_Channel1_IRQn           = 57,     /*!< DMA1 channel1 interrupt                                  */
    DMA1_Channel2_IRQn           = 58,     /*!< DMA1 channel2 interrupt                                  */
    DMA1_Channel3_IRQn           = 59,     /*!< DMA1 channel3 interrupt                                  */
    DMA1_Channel4_IRQn           = 60,     /*!< DMA1 channel4 interrupt                                  */
    ENET_IRQn                    = 61,     /*!< ENET interrupt                                           */
    ENET_WKUP_IRQn               = 62,     /*!< ENET wakeup through EXTI line interrupt                  */
    CAN1_TX_IRQn                 = 63,     /*!< CAN1 TX interrupt                                        */
    CAN1_RX0_IRQn                = 64,     /*!< CAN1 RX0 interrupt                                       */
    CAN1_RX1_IRQn                = 65,     /*!< CAN1 RX1 interrupt                                       */
    CAN1_EWMC_IRQn               = 66,     /*!< CAN1 EWMC interrupt                                      */
    USBFS_IRQn                   = 67,     /*!< USBFS interrupt                                          */
    DMA1_Channel5_IRQn           = 68,     /*!< DMA1 channel5 interrupt                                  */
    DMA1_Channel6_IRQn           = 69,     /*!< DMA1 channel6 interrupt                                  */
    DMA1_Channel7_IRQn           = 70,     /*!< DMA1 channel7 interrupt                                  */
    USART5_IRQn                  = 71,     /*!< USART5 interrupt                                         */
    I2C2_EV_IRQn                 = 72,     /*!< I2C2 event interrupt                                     */
    I2C2_ER_IRQn                 = 73,     /*!< I2C2 error interrupt                                     */
    USBHS_EP1_Out_IRQn           = 74,     /*!< USBHS endpoint 1 out interrupt                           */
    USBHS_EP1_In_IRQn            = 75,     /*!< USBHS endpoint 1 in interrupt                            */
    USBHS_WKUP_IRQn              = 76,     /*!< USBHS wakeup through EXTI line interrupt                 */
    USBHS_IRQn                   = 77,     /*!< USBHS interrupt                                          */
    DCI_IRQn                     = 78,     /*!< DCI interrupt                                            */
    TRNG_IRQn                    = 80,     /*!< TRNG interrupt                                           */
    FPU_IRQn                     = 81,     /*!< FPU interrupt                                            */
#endif /* GD32F407 and GD32F427 */

} IRQn_Type;

/* includes */
#include "core_cm4.h"
#include "system_gd32f4xx.h"
#include <stdint.h>

/* enum definitions */
typedef enum {DISABLE = 0, ENABLE = !DISABLE} EventStatus, ControlStatus;
typedef enum {RESET = 0, SET = !RESET} FlagStatus;
typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrStatus;

/* bit operations */
#define REG32(addr)                  (*(volatile uint32_t *)(uint32_t)(addr))
#define REG16(addr)                  (*(volatile uint16_t *)(uint32_t)(addr))
#define REG8(addr)                   (*(volatile uint8_t *)(uint32_t)(addr))
#define BIT(x)                       ((uint32_t)((uint32_t)0x01U<<(x)))
#define BITS(start, end)             ((0xFFFFFFFFUL << (start)) & (0xFFFFFFFFUL >> (31U - (uint32_t)(end)))) 
#define GET_BITS(regval, start, end) (((regval) & BITS((start),(end))) >> (start))

/* main flash and SRAM memory map */
#define FLASH_BASE            ((uint32_t)0x08000000U)        /*!< main FLASH base address          */
#define TCMSRAM_BASE          ((uint32_t)0x10000000U)        /*!< TCMSRAM(64KB) base address       */
#define OPTION_BASE           ((uint32_t)0x1FFEC000U)        /*!< Option bytes base address        */
#define SRAM_BASE             ((uint32_t)0x20000000U)        /*!< SRAM0 base address               */

/* peripheral memory map */
#define APB1_BUS_BASE         ((uint32_t)0x40000000U)        /*!< apb1 base address                */
#define APB2_BUS_BASE         ((uint32_t)0x40010000U)        /*!< apb2 base address                */
#define AHB1_BUS_BASE         ((uint32_t)0x40020000U)        /*!< ahb1 base address                */
#define AHB2_BUS_BASE         ((uint32_t)0x50000000U)        /*!< ahb2 base address                */

/* EXMC memory map */
#define EXMC_BASE             ((uint32_t)0xA0000000U)        /*!< EXMC register base address       */

/* advanced peripheral bus 1 memory map */
#define TIMER_BASE            (APB1_BUS_BASE + 0x00000000U)  /*!< TIMER base address               */
#define RTC_BASE              (APB1_BUS_BASE + 0x00002800U)  /*!< RTC base address                 */
#define WWDGT_BASE            (APB1_BUS_BASE + 0x00002C00U)  /*!< WWDGT base address               */
#define FWDGT_BASE            (APB1_BUS_BASE + 0x00003000U)  /*!< FWDGT base address               */
#define I2S_ADD_BASE          (APB1_BUS_BASE + 0x00003400U)  /*!< I2S1_add base address            */
#define SPI_BASE              (APB1_BUS_BASE + 0x00003800U)  /*!< SPI base address                 */
#define USART_BASE            (APB1_BUS_BASE + 0x00004400U)  /*!< USART base address               */
#define I2C_BASE              (APB1_BUS_BASE + 0x00005400U)  /*!< I2C base address                 */
#define CAN_BASE              (APB1_BUS_BASE + 0x00006400U)  /*!< CAN base address                 */
#define CTC_BASE              (APB1_BUS_BASE + 0x00006C00U)  /*!< CTC base address                 */
#define PMU_BASE              (APB1_BUS_BASE + 0x00007000U)  /*!< PMU base address                 */
#define DAC_BASE              (APB1_BUS_BASE + 0x00007400U)  /*!< DAC base address                 */
#define IREF_BASE             (APB1_BUS_BASE + 0x0000C400U)  /*!< IREF base address                */

/* advanced peripheral bus 2 memory map */
#define TLI_BASE              (APB2_BUS_BASE + 0x00006800U)  /*!< TLI base address                 */
#define SYSCFG_BASE           (APB2_BUS_BASE + 0x00003800U)  /*!< SYSCFG base address              */
#define EXTI_BASE             (APB2_BUS_BASE + 0x00003C00U)  /*!< EXTI base address                */
#define SDIO_BASE             (APB2_BUS_BASE + 0x00002C00U)  /*!< SDIO base address                */
#define ADC_BASE              (APB2_BUS_BASE + 0x00002000U)  /*!< ADC base address                 */
/* advanced high performance bus 1 memory map */
#define GPIO_BASE             (AHB1_BUS_BASE + 0x00000000U)  /*!< GPIO base address                */
#define CRC_BASE              (AHB1_BUS_BASE + 0x00003000U)  /*!< CRC base address                 */
#define RCU_BASE              (AHB1_BUS_BASE + 0x00003800U)  /*!< RCU base address                 */
#define FMC_BASE              (AHB1_BUS_BASE + 0x00003C00U)  /*!< FMC base address                 */
#define BKPSRAM_BASE          (AHB1_BUS_BASE + 0x00004000U)  /*!< BKPSRAM base address             */
#define DMA_BASE              (AHB1_BUS_BASE + 0x00006000U)  /*!< DMA base address                 */
#define ENET_BASE             (AHB1_BUS_BASE + 0x00008000U)  /*!< ENET base address                */
#define IPA_BASE              (AHB1_BUS_BASE + 0x0000B000U)  /*!< IPA base address                 */
#define USBHS_BASE            (AHB1_BUS_BASE + 0x00020000U)  /*!< USBHS base address               */

/* advanced high performance bus 2 memory map */
#define USBFS_BASE            (AHB2_BUS_BASE + 0x00000000U)  /*!< USBFS base address               */
#define DCI_BASE              (AHB2_BUS_BASE + 0x00050000U)  /*!< DCI base address                 */
#define TRNG_BASE             (AHB2_BUS_BASE + 0x00060800U)  /*!< TRNG base address                */
/* option byte and debug memory map */
#define OB_BASE               ((uint32_t)0x1FFEC000U)        /*!< OB base address                  */
#define DBG_BASE              ((uint32_t)0xE0042000U)        /*!< DBG base address                 */

/* define marco USE_STDPERIPH_DRIVER */
#if !defined  USE_STDPERIPH_DRIVER
#define USE_STDPERIPH_DRIVER
#endif 
#ifdef USE_STDPERIPH_DRIVER
#include "gd32f4xx_libopt.h"
#endif /* USE_STDPERIPH_DRIVER */

#ifdef __cplusplus
}
#endif
#endif 
