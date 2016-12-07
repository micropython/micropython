/**
  ******************************************************************************
  * @file    stm32f746xx.h
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    23-September-2016
  * @brief   CMSIS Cortex-M7 Device Peripheral Access Layer Header File.
  *
  *          This file contains:
  *           - Data structures and the address mapping for all peripherals
  *           - Peripheral's registers declarations and bits definition
  *           - Macros to access peripheral's registers hardware
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/** @addtogroup CMSIS_Device
  * @{
  */

/** @addtogroup stm32f746xx
  * @{
  */

#ifndef __STM32F746xx_H
#define __STM32F746xx_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/** @addtogroup Configuration_section_for_CMSIS
  * @{
  */

/**
 * @brief STM32F7xx Interrupt Number Definition, according to the selected device
 *        in @ref Library_configuration_section
 */
typedef enum
{
/******  Cortex-M7 Processor Exceptions Numbers ****************************************************************/
  NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                                          */
  MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M7 Memory Management Interrupt                           */
  BusFault_IRQn               = -11,    /*!< 5 Cortex-M7 Bus Fault Interrupt                                   */
  UsageFault_IRQn             = -10,    /*!< 6 Cortex-M7 Usage Fault Interrupt                                 */
  SVCall_IRQn                 = -5,     /*!< 11 Cortex-M7 SV Call Interrupt                                    */
  DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M7 Debug Monitor Interrupt                              */
  PendSV_IRQn                 = -2,     /*!< 14 Cortex-M7 Pend SV Interrupt                                    */
  SysTick_IRQn                = -1,     /*!< 15 Cortex-M7 System Tick Interrupt                                */
/******  STM32 specific Interrupt Numbers **********************************************************************/
  WWDG_IRQn                   = 0,      /*!< Window WatchDog Interrupt                                         */
  PVD_IRQn                    = 1,      /*!< PVD through EXTI Line detection Interrupt                         */
  TAMP_STAMP_IRQn             = 2,      /*!< Tamper and TimeStamp interrupts through the EXTI line             */
  RTC_WKUP_IRQn               = 3,      /*!< RTC Wakeup interrupt through the EXTI line                        */
  FLASH_IRQn                  = 4,      /*!< FLASH global Interrupt                                            */
  RCC_IRQn                    = 5,      /*!< RCC global Interrupt                                              */
  EXTI0_IRQn                  = 6,      /*!< EXTI Line0 Interrupt                                              */
  EXTI1_IRQn                  = 7,      /*!< EXTI Line1 Interrupt                                              */
  EXTI2_IRQn                  = 8,      /*!< EXTI Line2 Interrupt                                              */
  EXTI3_IRQn                  = 9,      /*!< EXTI Line3 Interrupt                                              */
  EXTI4_IRQn                  = 10,     /*!< EXTI Line4 Interrupt                                              */
  DMA1_Stream0_IRQn           = 11,     /*!< DMA1 Stream 0 global Interrupt                                    */
  DMA1_Stream1_IRQn           = 12,     /*!< DMA1 Stream 1 global Interrupt                                    */
  DMA1_Stream2_IRQn           = 13,     /*!< DMA1 Stream 2 global Interrupt                                    */
  DMA1_Stream3_IRQn           = 14,     /*!< DMA1 Stream 3 global Interrupt                                    */
  DMA1_Stream4_IRQn           = 15,     /*!< DMA1 Stream 4 global Interrupt                                    */
  DMA1_Stream5_IRQn           = 16,     /*!< DMA1 Stream 5 global Interrupt                                    */
  DMA1_Stream6_IRQn           = 17,     /*!< DMA1 Stream 6 global Interrupt                                    */
  ADC_IRQn                    = 18,     /*!< ADC1, ADC2 and ADC3 global Interrupts                             */
  CAN1_TX_IRQn                = 19,     /*!< CAN1 TX Interrupt                                                 */
  CAN1_RX0_IRQn               = 20,     /*!< CAN1 RX0 Interrupt                                                */
  CAN1_RX1_IRQn               = 21,     /*!< CAN1 RX1 Interrupt                                                */
  CAN1_SCE_IRQn               = 22,     /*!< CAN1 SCE Interrupt                                                */
  EXTI9_5_IRQn                = 23,     /*!< External Line[9:5] Interrupts                                     */
  TIM1_BRK_TIM9_IRQn          = 24,     /*!< TIM1 Break interrupt and TIM9 global interrupt                    */
  TIM1_UP_TIM10_IRQn          = 25,     /*!< TIM1 Update Interrupt and TIM10 global interrupt                  */
  TIM1_TRG_COM_TIM11_IRQn     = 26,     /*!< TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
  TIM1_CC_IRQn                = 27,     /*!< TIM1 Capture Compare Interrupt                                    */
  TIM2_IRQn                   = 28,     /*!< TIM2 global Interrupt                                             */
  TIM3_IRQn                   = 29,     /*!< TIM3 global Interrupt                                             */
  TIM4_IRQn                   = 30,     /*!< TIM4 global Interrupt                                             */
  I2C1_EV_IRQn                = 31,     /*!< I2C1 Event Interrupt                                              */
  I2C1_ER_IRQn                = 32,     /*!< I2C1 Error Interrupt                                              */
  I2C2_EV_IRQn                = 33,     /*!< I2C2 Event Interrupt                                              */
  I2C2_ER_IRQn                = 34,     /*!< I2C2 Error Interrupt                                              */
  SPI1_IRQn                   = 35,     /*!< SPI1 global Interrupt                                             */
  SPI2_IRQn                   = 36,     /*!< SPI2 global Interrupt                                             */
  USART1_IRQn                 = 37,     /*!< USART1 global Interrupt                                           */
  USART2_IRQn                 = 38,     /*!< USART2 global Interrupt                                           */
  USART3_IRQn                 = 39,     /*!< USART3 global Interrupt                                           */
  EXTI15_10_IRQn              = 40,     /*!< External Line[15:10] Interrupts                                   */
  RTC_Alarm_IRQn              = 41,     /*!< RTC Alarm (A and B) through EXTI Line Interrupt                   */
  OTG_FS_WKUP_IRQn            = 42,     /*!< USB OTG FS Wakeup through EXTI line interrupt                     */
  TIM8_BRK_TIM12_IRQn         = 43,     /*!< TIM8 Break Interrupt and TIM12 global interrupt                   */
  TIM8_UP_TIM13_IRQn          = 44,     /*!< TIM8 Update Interrupt and TIM13 global interrupt                  */
  TIM8_TRG_COM_TIM14_IRQn     = 45,     /*!< TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
  TIM8_CC_IRQn                = 46,     /*!< TIM8 Capture Compare Interrupt                                    */
  DMA1_Stream7_IRQn           = 47,     /*!< DMA1 Stream7 Interrupt                                            */
  FMC_IRQn                    = 48,     /*!< FMC global Interrupt                                              */
  SDMMC1_IRQn                 = 49,     /*!< SDMMC1 global Interrupt                                           */
  TIM5_IRQn                   = 50,     /*!< TIM5 global Interrupt                                             */
  SPI3_IRQn                   = 51,     /*!< SPI3 global Interrupt                                             */
  UART4_IRQn                  = 52,     /*!< UART4 global Interrupt                                            */
  UART5_IRQn                  = 53,     /*!< UART5 global Interrupt                                            */
  TIM6_DAC_IRQn               = 54,     /*!< TIM6 global and DAC1&2 underrun error  interrupts                 */
  TIM7_IRQn                   = 55,     /*!< TIM7 global interrupt                                             */
  DMA2_Stream0_IRQn           = 56,     /*!< DMA2 Stream 0 global Interrupt                                    */
  DMA2_Stream1_IRQn           = 57,     /*!< DMA2 Stream 1 global Interrupt                                    */
  DMA2_Stream2_IRQn           = 58,     /*!< DMA2 Stream 2 global Interrupt                                    */
  DMA2_Stream3_IRQn           = 59,     /*!< DMA2 Stream 3 global Interrupt                                    */
  DMA2_Stream4_IRQn           = 60,     /*!< DMA2 Stream 4 global Interrupt                                    */
  ETH_IRQn                    = 61,     /*!< Ethernet global Interrupt                                         */
  ETH_WKUP_IRQn               = 62,     /*!< Ethernet Wakeup through EXTI line Interrupt                       */
  CAN2_TX_IRQn                = 63,     /*!< CAN2 TX Interrupt                                                 */
  CAN2_RX0_IRQn               = 64,     /*!< CAN2 RX0 Interrupt                                                */
  CAN2_RX1_IRQn               = 65,     /*!< CAN2 RX1 Interrupt                                                */
  CAN2_SCE_IRQn               = 66,     /*!< CAN2 SCE Interrupt                                                */
  OTG_FS_IRQn                 = 67,     /*!< USB OTG FS global Interrupt                                       */
  DMA2_Stream5_IRQn           = 68,     /*!< DMA2 Stream 5 global interrupt                                    */
  DMA2_Stream6_IRQn           = 69,     /*!< DMA2 Stream 6 global interrupt                                    */
  DMA2_Stream7_IRQn           = 70,     /*!< DMA2 Stream 7 global interrupt                                    */
  USART6_IRQn                 = 71,     /*!< USART6 global interrupt                                           */
  I2C3_EV_IRQn                = 72,     /*!< I2C3 event interrupt                                              */
  I2C3_ER_IRQn                = 73,     /*!< I2C3 error interrupt                                              */
  OTG_HS_EP1_OUT_IRQn         = 74,     /*!< USB OTG HS End Point 1 Out global interrupt                       */
  OTG_HS_EP1_IN_IRQn          = 75,     /*!< USB OTG HS End Point 1 In global interrupt                        */
  OTG_HS_WKUP_IRQn            = 76,     /*!< USB OTG HS Wakeup through EXTI interrupt                          */
  OTG_HS_IRQn                 = 77,     /*!< USB OTG HS global interrupt                                       */
  DCMI_IRQn                   = 78,     /*!< DCMI global interrupt                                             */
  RNG_IRQn                    = 80,     /*!< RNG global interrupt                                              */
  FPU_IRQn                    = 81,     /*!< FPU global interrupt                                              */
  UART7_IRQn                  = 82,     /*!< UART7 global interrupt                                            */
  UART8_IRQn                  = 83,     /*!< UART8 global interrupt                                            */
  SPI4_IRQn                   = 84,     /*!< SPI4 global Interrupt                                             */
  SPI5_IRQn                   = 85,     /*!< SPI5 global Interrupt                                             */
  SPI6_IRQn                   = 86,     /*!< SPI6 global Interrupt                                             */
  SAI1_IRQn                   = 87,     /*!< SAI1 global Interrupt                                             */
  LTDC_IRQn                   = 88,     /*!< LTDC global Interrupt                                             */
  LTDC_ER_IRQn                = 89,     /*!< LTDC Error global Interrupt                                       */
  DMA2D_IRQn                  = 90,     /*!< DMA2D global Interrupt                                            */
  SAI2_IRQn                   = 91,     /*!< SAI2 global Interrupt                                             */
  QUADSPI_IRQn                = 92,     /*!< Quad SPI global interrupt                                         */
  LPTIM1_IRQn                 = 93,     /*!< LP TIM1 interrupt                                                 */
  CEC_IRQn                    = 94,     /*!< HDMI-CEC global Interrupt                                         */
  I2C4_EV_IRQn                = 95,     /*!< I2C4 Event Interrupt                                              */
  I2C4_ER_IRQn                = 96,     /*!< I2C4 Error Interrupt                                              */
  SPDIF_RX_IRQn               = 97,     /*!< SPDIF-RX global Interrupt                                         */
} IRQn_Type;

/**
  * @}
  */

/**
 * @brief Configuration of the Cortex-M7 Processor and Core Peripherals
 */
#define __CM7_REV                 0x0001U  /*!< Cortex-M7 revision r0p1                       */
#define __MPU_PRESENT             1       /*!< CM7 provides an MPU                           */
#define __NVIC_PRIO_BITS          4       /*!< CM7 uses 4 Bits for the Priority Levels       */
#define __Vendor_SysTickConfig    0       /*!< Set to 1 if different SysTick Config is used  */
#define __FPU_PRESENT             1       /*!< FPU present                                   */
#define __ICACHE_PRESENT          1       /*!< CM7 instruction cache present                 */
#define __DCACHE_PRESENT          1       /*!< CM7 data cache present                        */
#include "core_cm7.h"                     /*!< Cortex-M7 processor and core peripherals      */


#include "system_stm32f7xx.h"
#include <stdint.h>

/** @addtogroup Peripheral_registers_structures
  * @{
  */

/**
  * @brief Analog to Digital Converter
  */

typedef struct
{
  __IO uint32_t SR;     /*!< ADC status register,                         Address offset: 0x00 */
  __IO uint32_t CR1;    /*!< ADC control register 1,                      Address offset: 0x04 */
  __IO uint32_t CR2;    /*!< ADC control register 2,                      Address offset: 0x08 */
  __IO uint32_t SMPR1;  /*!< ADC sample time register 1,                  Address offset: 0x0C */
  __IO uint32_t SMPR2;  /*!< ADC sample time register 2,                  Address offset: 0x10 */
  __IO uint32_t JOFR1;  /*!< ADC injected channel data offset register 1, Address offset: 0x14 */
  __IO uint32_t JOFR2;  /*!< ADC injected channel data offset register 2, Address offset: 0x18 */
  __IO uint32_t JOFR3;  /*!< ADC injected channel data offset register 3, Address offset: 0x1C */
  __IO uint32_t JOFR4;  /*!< ADC injected channel data offset register 4, Address offset: 0x20 */
  __IO uint32_t HTR;    /*!< ADC watchdog higher threshold register,      Address offset: 0x24 */
  __IO uint32_t LTR;    /*!< ADC watchdog lower threshold register,       Address offset: 0x28 */
  __IO uint32_t SQR1;   /*!< ADC regular sequence register 1,             Address offset: 0x2C */
  __IO uint32_t SQR2;   /*!< ADC regular sequence register 2,             Address offset: 0x30 */
  __IO uint32_t SQR3;   /*!< ADC regular sequence register 3,             Address offset: 0x34 */
  __IO uint32_t JSQR;   /*!< ADC injected sequence register,              Address offset: 0x38*/
  __IO uint32_t JDR1;   /*!< ADC injected data register 1,                Address offset: 0x3C */
  __IO uint32_t JDR2;   /*!< ADC injected data register 2,                Address offset: 0x40 */
  __IO uint32_t JDR3;   /*!< ADC injected data register 3,                Address offset: 0x44 */
  __IO uint32_t JDR4;   /*!< ADC injected data register 4,                Address offset: 0x48 */
  __IO uint32_t DR;     /*!< ADC regular data register,                   Address offset: 0x4C */
} ADC_TypeDef;

typedef struct
{
  __IO uint32_t CSR;    /*!< ADC Common status register,                  Address offset: ADC1 base address + 0x300 */
  __IO uint32_t CCR;    /*!< ADC common control register,                 Address offset: ADC1 base address + 0x304 */
  __IO uint32_t CDR;    /*!< ADC common regular data register for dual
                             AND triple modes,                            Address offset: ADC1 base address + 0x308 */
} ADC_Common_TypeDef;


/**
  * @brief Controller Area Network TxMailBox
  */

typedef struct
{
  __IO uint32_t TIR;  /*!< CAN TX mailbox identifier register */
  __IO uint32_t TDTR; /*!< CAN mailbox data length control and time stamp register */
  __IO uint32_t TDLR; /*!< CAN mailbox data low register */
  __IO uint32_t TDHR; /*!< CAN mailbox data high register */
} CAN_TxMailBox_TypeDef;

/**
  * @brief Controller Area Network FIFOMailBox
  */

typedef struct
{
  __IO uint32_t RIR;  /*!< CAN receive FIFO mailbox identifier register */
  __IO uint32_t RDTR; /*!< CAN receive FIFO mailbox data length control and time stamp register */
  __IO uint32_t RDLR; /*!< CAN receive FIFO mailbox data low register */
  __IO uint32_t RDHR; /*!< CAN receive FIFO mailbox data high register */
} CAN_FIFOMailBox_TypeDef;

/**
  * @brief Controller Area Network FilterRegister
  */

typedef struct
{
  __IO uint32_t FR1; /*!< CAN Filter bank register 1 */
  __IO uint32_t FR2; /*!< CAN Filter bank register 1 */
} CAN_FilterRegister_TypeDef;

/**
  * @brief Controller Area Network
  */

typedef struct
{
  __IO uint32_t              MCR;                 /*!< CAN master control register,         Address offset: 0x00          */
  __IO uint32_t              MSR;                 /*!< CAN master status register,          Address offset: 0x04          */
  __IO uint32_t              TSR;                 /*!< CAN transmit status register,        Address offset: 0x08          */
  __IO uint32_t              RF0R;                /*!< CAN receive FIFO 0 register,         Address offset: 0x0C          */
  __IO uint32_t              RF1R;                /*!< CAN receive FIFO 1 register,         Address offset: 0x10          */
  __IO uint32_t              IER;                 /*!< CAN interrupt enable register,       Address offset: 0x14          */
  __IO uint32_t              ESR;                 /*!< CAN error status register,           Address offset: 0x18          */
  __IO uint32_t              BTR;                 /*!< CAN bit timing register,             Address offset: 0x1C          */
  uint32_t                   RESERVED0[88];       /*!< Reserved, 0x020 - 0x17F                                            */
  CAN_TxMailBox_TypeDef      sTxMailBox[3];       /*!< CAN Tx MailBox,                      Address offset: 0x180 - 0x1AC */
  CAN_FIFOMailBox_TypeDef    sFIFOMailBox[2];     /*!< CAN FIFO MailBox,                    Address offset: 0x1B0 - 0x1CC */
  uint32_t                   RESERVED1[12];       /*!< Reserved, 0x1D0 - 0x1FF                                            */
  __IO uint32_t              FMR;                 /*!< CAN filter master register,          Address offset: 0x200         */
  __IO uint32_t              FM1R;                /*!< CAN filter mode register,            Address offset: 0x204         */
  uint32_t                   RESERVED2;           /*!< Reserved, 0x208                                                    */
  __IO uint32_t              FS1R;                /*!< CAN filter scale register,           Address offset: 0x20C         */
  uint32_t                   RESERVED3;           /*!< Reserved, 0x210                                                    */
  __IO uint32_t              FFA1R;               /*!< CAN filter FIFO assignment register, Address offset: 0x214         */
  uint32_t                   RESERVED4;           /*!< Reserved, 0x218                                                    */
  __IO uint32_t              FA1R;                /*!< CAN filter activation register,      Address offset: 0x21C         */
  uint32_t                   RESERVED5[8];        /*!< Reserved, 0x220-0x23F                                              */
  CAN_FilterRegister_TypeDef sFilterRegister[28]; /*!< CAN Filter Register,                 Address offset: 0x240-0x31C   */
} CAN_TypeDef;

/**
  * @brief HDMI-CEC
  */

typedef struct
{
  __IO uint32_t CR;           /*!< CEC control register,                                       Address offset:0x00 */
  __IO uint32_t CFGR;         /*!< CEC configuration register,                                 Address offset:0x04 */
  __IO uint32_t TXDR;         /*!< CEC Tx data register ,                                      Address offset:0x08 */
  __IO uint32_t RXDR;         /*!< CEC Rx Data Register,                                       Address offset:0x0C */
  __IO uint32_t ISR;          /*!< CEC Interrupt and Status Register,                          Address offset:0x10 */
  __IO uint32_t IER;          /*!< CEC interrupt enable register,                              Address offset:0x14 */
}CEC_TypeDef;

/**
  * @brief CRC calculation unit
  */

typedef struct
{
  __IO uint32_t  DR;          /*!< CRC Data register,                           Address offset: 0x00 */
  __IO uint8_t   IDR;         /*!< CRC Independent data register,               Address offset: 0x04 */
  uint8_t        RESERVED0;   /*!< Reserved, 0x05                                                    */
  uint16_t       RESERVED1;   /*!< Reserved, 0x06                                                    */
  __IO uint32_t  CR;          /*!< CRC Control register,                        Address offset: 0x08 */
  uint32_t       RESERVED2;   /*!< Reserved,                                                    0x0C */
  __IO uint32_t  INIT;        /*!< Initial CRC value register,                  Address offset: 0x10 */
  __IO uint32_t  POL;         /*!< CRC polynomial register,                     Address offset: 0x14 */
} CRC_TypeDef;

/**
  * @brief Digital to Analog Converter
  */

typedef struct
{
  __IO uint32_t CR;       /*!< DAC control register,                                    Address offset: 0x00 */
  __IO uint32_t SWTRIGR;  /*!< DAC software trigger register,                           Address offset: 0x04 */
  __IO uint32_t DHR12R1;  /*!< DAC channel1 12-bit right-aligned data holding register, Address offset: 0x08 */
  __IO uint32_t DHR12L1;  /*!< DAC channel1 12-bit left aligned data holding register,  Address offset: 0x0C */
  __IO uint32_t DHR8R1;   /*!< DAC channel1 8-bit right aligned data holding register,  Address offset: 0x10 */
  __IO uint32_t DHR12R2;  /*!< DAC channel2 12-bit right aligned data holding register, Address offset: 0x14 */
  __IO uint32_t DHR12L2;  /*!< DAC channel2 12-bit left aligned data holding register,  Address offset: 0x18 */
  __IO uint32_t DHR8R2;   /*!< DAC channel2 8-bit right-aligned data holding register,  Address offset: 0x1C */
  __IO uint32_t DHR12RD;  /*!< Dual DAC 12-bit right-aligned data holding register,     Address offset: 0x20 */
  __IO uint32_t DHR12LD;  /*!< DUAL DAC 12-bit left aligned data holding register,      Address offset: 0x24 */
  __IO uint32_t DHR8RD;   /*!< DUAL DAC 8-bit right aligned data holding register,      Address offset: 0x28 */
  __IO uint32_t DOR1;     /*!< DAC channel1 data output register,                       Address offset: 0x2C */
  __IO uint32_t DOR2;     /*!< DAC channel2 data output register,                       Address offset: 0x30 */
  __IO uint32_t SR;       /*!< DAC status register,                                     Address offset: 0x34 */
} DAC_TypeDef;


/**
  * @brief Debug MCU
  */

typedef struct
{
  __IO uint32_t IDCODE;  /*!< MCU device ID code,               Address offset: 0x00 */
  __IO uint32_t CR;      /*!< Debug MCU configuration register, Address offset: 0x04 */
  __IO uint32_t APB1FZ;  /*!< Debug MCU APB1 freeze register,   Address offset: 0x08 */
  __IO uint32_t APB2FZ;  /*!< Debug MCU APB2 freeze register,   Address offset: 0x0C */
}DBGMCU_TypeDef;

/**
  * @brief DCMI
  */

typedef struct
{
  __IO uint32_t CR;       /*!< DCMI control register 1,                       Address offset: 0x00 */
  __IO uint32_t SR;       /*!< DCMI status register,                          Address offset: 0x04 */
  __IO uint32_t RISR;     /*!< DCMI raw interrupt status register,            Address offset: 0x08 */
  __IO uint32_t IER;      /*!< DCMI interrupt enable register,                Address offset: 0x0C */
  __IO uint32_t MISR;     /*!< DCMI masked interrupt status register,         Address offset: 0x10 */
  __IO uint32_t ICR;      /*!< DCMI interrupt clear register,                 Address offset: 0x14 */
  __IO uint32_t ESCR;     /*!< DCMI embedded synchronization code register,   Address offset: 0x18 */
  __IO uint32_t ESUR;     /*!< DCMI embedded synchronization unmask register, Address offset: 0x1C */
  __IO uint32_t CWSTRTR;  /*!< DCMI crop window start,                        Address offset: 0x20 */
  __IO uint32_t CWSIZER;  /*!< DCMI crop window size,                         Address offset: 0x24 */
  __IO uint32_t DR;       /*!< DCMI data register,                            Address offset: 0x28 */
} DCMI_TypeDef;

/**
  * @brief DMA Controller
  */

typedef struct
{
  __IO uint32_t CR;     /*!< DMA stream x configuration register      */
  __IO uint32_t NDTR;   /*!< DMA stream x number of data register     */
  __IO uint32_t PAR;    /*!< DMA stream x peripheral address register */
  __IO uint32_t M0AR;   /*!< DMA stream x memory 0 address register   */
  __IO uint32_t M1AR;   /*!< DMA stream x memory 1 address register   */
  __IO uint32_t FCR;    /*!< DMA stream x FIFO control register       */
} DMA_Stream_TypeDef;

typedef struct
{
  __IO uint32_t LISR;   /*!< DMA low interrupt status register,      Address offset: 0x00 */
  __IO uint32_t HISR;   /*!< DMA high interrupt status register,     Address offset: 0x04 */
  __IO uint32_t LIFCR;  /*!< DMA low interrupt flag clear register,  Address offset: 0x08 */
  __IO uint32_t HIFCR;  /*!< DMA high interrupt flag clear register, Address offset: 0x0C */
} DMA_TypeDef;

/**
  * @brief DMA2D Controller
  */

typedef struct
{
  __IO uint32_t CR;            /*!< DMA2D Control Register,                         Address offset: 0x00 */
  __IO uint32_t ISR;           /*!< DMA2D Interrupt Status Register,                Address offset: 0x04 */
  __IO uint32_t IFCR;          /*!< DMA2D Interrupt Flag Clear Register,            Address offset: 0x08 */
  __IO uint32_t FGMAR;         /*!< DMA2D Foreground Memory Address Register,       Address offset: 0x0C */
  __IO uint32_t FGOR;          /*!< DMA2D Foreground Offset Register,               Address offset: 0x10 */
  __IO uint32_t BGMAR;         /*!< DMA2D Background Memory Address Register,       Address offset: 0x14 */
  __IO uint32_t BGOR;          /*!< DMA2D Background Offset Register,               Address offset: 0x18 */
  __IO uint32_t FGPFCCR;       /*!< DMA2D Foreground PFC Control Register,          Address offset: 0x1C */
  __IO uint32_t FGCOLR;        /*!< DMA2D Foreground Color Register,                Address offset: 0x20 */
  __IO uint32_t BGPFCCR;       /*!< DMA2D Background PFC Control Register,          Address offset: 0x24 */
  __IO uint32_t BGCOLR;        /*!< DMA2D Background Color Register,                Address offset: 0x28 */
  __IO uint32_t FGCMAR;        /*!< DMA2D Foreground CLUT Memory Address Register,  Address offset: 0x2C */
  __IO uint32_t BGCMAR;        /*!< DMA2D Background CLUT Memory Address Register,  Address offset: 0x30 */
  __IO uint32_t OPFCCR;        /*!< DMA2D Output PFC Control Register,              Address offset: 0x34 */
  __IO uint32_t OCOLR;         /*!< DMA2D Output Color Register,                    Address offset: 0x38 */
  __IO uint32_t OMAR;          /*!< DMA2D Output Memory Address Register,           Address offset: 0x3C */
  __IO uint32_t OOR;           /*!< DMA2D Output Offset Register,                   Address offset: 0x40 */
  __IO uint32_t NLR;           /*!< DMA2D Number of Line Register,                  Address offset: 0x44 */
  __IO uint32_t LWR;           /*!< DMA2D Line Watermark Register,                  Address offset: 0x48 */
  __IO uint32_t AMTCR;         /*!< DMA2D AHB Master Timer Configuration Register,  Address offset: 0x4C */
  uint32_t      RESERVED[236]; /*!< Reserved, 0x50-0x3FF */
  __IO uint32_t FGCLUT[256];   /*!< DMA2D Foreground CLUT,                          Address offset:400-7FF */
  __IO uint32_t BGCLUT[256];   /*!< DMA2D Background CLUT,                          Address offset:800-BFF */
} DMA2D_TypeDef;


/**
  * @brief Ethernet MAC
  */

typedef struct
{
  __IO uint32_t MACCR;
  __IO uint32_t MACFFR;
  __IO uint32_t MACHTHR;
  __IO uint32_t MACHTLR;
  __IO uint32_t MACMIIAR;
  __IO uint32_t MACMIIDR;
  __IO uint32_t MACFCR;
  __IO uint32_t MACVLANTR;             /*    8 */
  uint32_t      RESERVED0[2];
  __IO uint32_t MACRWUFFR;             /*   11 */
  __IO uint32_t MACPMTCSR;
  uint32_t      RESERVED1[2];
  __IO uint32_t MACSR;                 /*   15 */
  __IO uint32_t MACIMR;
  __IO uint32_t MACA0HR;
  __IO uint32_t MACA0LR;
  __IO uint32_t MACA1HR;
  __IO uint32_t MACA1LR;
  __IO uint32_t MACA2HR;
  __IO uint32_t MACA2LR;
  __IO uint32_t MACA3HR;
  __IO uint32_t MACA3LR;               /*   24 */
  uint32_t      RESERVED2[40];
  __IO uint32_t MMCCR;                 /*   65 */
  __IO uint32_t MMCRIR;
  __IO uint32_t MMCTIR;
  __IO uint32_t MMCRIMR;
  __IO uint32_t MMCTIMR;               /*   69 */
  uint32_t      RESERVED3[14];
  __IO uint32_t MMCTGFSCCR;            /*   84 */
  __IO uint32_t MMCTGFMSCCR;
  uint32_t      RESERVED4[5];
  __IO uint32_t MMCTGFCR;
  uint32_t      RESERVED5[10];
  __IO uint32_t MMCRFCECR;
  __IO uint32_t MMCRFAECR;
  uint32_t      RESERVED6[10];
  __IO uint32_t MMCRGUFCR;
  uint32_t      RESERVED7[334];
  __IO uint32_t PTPTSCR;
  __IO uint32_t PTPSSIR;
  __IO uint32_t PTPTSHR;
  __IO uint32_t PTPTSLR;
  __IO uint32_t PTPTSHUR;
  __IO uint32_t PTPTSLUR;
  __IO uint32_t PTPTSAR;
  __IO uint32_t PTPTTHR;
  __IO uint32_t PTPTTLR;
  __IO uint32_t RESERVED8;
  __IO uint32_t PTPTSSR;
  uint32_t      RESERVED9[565];
  __IO uint32_t DMABMR;
  __IO uint32_t DMATPDR;
  __IO uint32_t DMARPDR;
  __IO uint32_t DMARDLAR;
  __IO uint32_t DMATDLAR;
  __IO uint32_t DMASR;
  __IO uint32_t DMAOMR;
  __IO uint32_t DMAIER;
  __IO uint32_t DMAMFBOCR;
  __IO uint32_t DMARSWTR;
  uint32_t      RESERVED10[8];
  __IO uint32_t DMACHTDR;
  __IO uint32_t DMACHRDR;
  __IO uint32_t DMACHTBAR;
  __IO uint32_t DMACHRBAR;
} ETH_TypeDef;

/**
  * @brief External Interrupt/Event Controller
  */

typedef struct
{
  __IO uint32_t IMR;    /*!< EXTI Interrupt mask register,            Address offset: 0x00 */
  __IO uint32_t EMR;    /*!< EXTI Event mask register,                Address offset: 0x04 */
  __IO uint32_t RTSR;   /*!< EXTI Rising trigger selection register,  Address offset: 0x08 */
  __IO uint32_t FTSR;   /*!< EXTI Falling trigger selection register, Address offset: 0x0C */
  __IO uint32_t SWIER;  /*!< EXTI Software interrupt event register,  Address offset: 0x10 */
  __IO uint32_t PR;     /*!< EXTI Pending register,                   Address offset: 0x14 */
} EXTI_TypeDef;

/**
  * @brief FLASH Registers
  */

typedef struct
{
  __IO uint32_t ACR;      /*!< FLASH access control register,     Address offset: 0x00 */
  __IO uint32_t KEYR;     /*!< FLASH key register,                Address offset: 0x04 */
  __IO uint32_t OPTKEYR;  /*!< FLASH option key register,         Address offset: 0x08 */
  __IO uint32_t SR;       /*!< FLASH status register,             Address offset: 0x0C */
  __IO uint32_t CR;       /*!< FLASH control register,            Address offset: 0x10 */
  __IO uint32_t OPTCR;    /*!< FLASH option control register ,    Address offset: 0x14 */
  __IO uint32_t OPTCR1;   /*!< FLASH option control register 1 ,  Address offset: 0x18 */
} FLASH_TypeDef;



/**
  * @brief Flexible Memory Controller
  */

typedef struct
{
  __IO uint32_t BTCR[8];    /*!< NOR/PSRAM chip-select control register(BCR) and chip-select timing register(BTR), Address offset: 0x00-1C */
} FMC_Bank1_TypeDef;

/**
  * @brief Flexible Memory Controller Bank1E
  */

typedef struct
{
  __IO uint32_t BWTR[7];    /*!< NOR/PSRAM write timing registers, Address offset: 0x104-0x11C */
} FMC_Bank1E_TypeDef;

/**
  * @brief Flexible Memory Controller Bank3
  */

typedef struct
{
  __IO uint32_t PCR;        /*!< NAND Flash control register,                       Address offset: 0x80 */
  __IO uint32_t SR;         /*!< NAND Flash FIFO status and interrupt register,     Address offset: 0x84 */
  __IO uint32_t PMEM;       /*!< NAND Flash Common memory space timing register,    Address offset: 0x88 */
  __IO uint32_t PATT;       /*!< NAND Flash Attribute memory space timing register, Address offset: 0x8C */
  uint32_t      RESERVED0;  /*!< Reserved, 0x90                                                          */
  __IO uint32_t ECCR;       /*!< NAND Flash ECC result registers,                   Address offset: 0x94 */
} FMC_Bank3_TypeDef;

/**
  * @brief Flexible Memory Controller Bank5_6
  */

typedef struct
{
  __IO uint32_t SDCR[2];        /*!< SDRAM Control registers ,      Address offset: 0x140-0x144  */
  __IO uint32_t SDTR[2];        /*!< SDRAM Timing registers ,       Address offset: 0x148-0x14C  */
  __IO uint32_t SDCMR;       /*!< SDRAM Command Mode register,    Address offset: 0x150  */
  __IO uint32_t SDRTR;       /*!< SDRAM Refresh Timer register,   Address offset: 0x154  */
  __IO uint32_t SDSR;        /*!< SDRAM Status register,          Address offset: 0x158  */
} FMC_Bank5_6_TypeDef;


/**
  * @brief General Purpose I/O
  */

typedef struct
{
  __IO uint32_t MODER;    /*!< GPIO port mode register,               Address offset: 0x00      */
  __IO uint32_t OTYPER;   /*!< GPIO port output type register,        Address offset: 0x04      */
  __IO uint32_t OSPEEDR;  /*!< GPIO port output speed register,       Address offset: 0x08      */
  __IO uint32_t PUPDR;    /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
  __IO uint32_t IDR;      /*!< GPIO port input data register,         Address offset: 0x10      */
  __IO uint32_t ODR;      /*!< GPIO port output data register,        Address offset: 0x14      */
  __IO uint32_t BSRR;     /*!< GPIO port bit set/reset register,      Address offset: 0x18      */
  __IO uint32_t LCKR;     /*!< GPIO port configuration lock register, Address offset: 0x1C      */
  __IO uint32_t AFR[2];   /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
} GPIO_TypeDef;

/**
  * @brief System configuration controller
  */

typedef struct
{
  __IO uint32_t MEMRMP;       /*!< SYSCFG memory remap register,                      Address offset: 0x00      */
  __IO uint32_t PMC;          /*!< SYSCFG peripheral mode configuration register,     Address offset: 0x04      */
  __IO uint32_t EXTICR[4];    /*!< SYSCFG external interrupt configuration registers, Address offset: 0x08-0x14 */
  uint32_t      RESERVED[2];  /*!< Reserved, 0x18-0x1C                                                          */
  __IO uint32_t CMPCR;        /*!< SYSCFG Compensation cell control register,         Address offset: 0x20      */
} SYSCFG_TypeDef;

/**
  * @brief Inter-integrated Circuit Interface
  */

typedef struct
{
  __IO uint32_t CR1;      /*!< I2C Control register 1,            Address offset: 0x00 */
  __IO uint32_t CR2;      /*!< I2C Control register 2,            Address offset: 0x04 */
  __IO uint32_t OAR1;     /*!< I2C Own address 1 register,        Address offset: 0x08 */
  __IO uint32_t OAR2;     /*!< I2C Own address 2 register,        Address offset: 0x0C */
  __IO uint32_t TIMINGR;  /*!< I2C Timing register,               Address offset: 0x10 */
  __IO uint32_t TIMEOUTR; /*!< I2C Timeout register,              Address offset: 0x14 */
  __IO uint32_t ISR;      /*!< I2C Interrupt and status register, Address offset: 0x18 */
  __IO uint32_t ICR;      /*!< I2C Interrupt clear register,      Address offset: 0x1C */
  __IO uint32_t PECR;     /*!< I2C PEC register,                  Address offset: 0x20 */
  __IO uint32_t RXDR;     /*!< I2C Receive data register,         Address offset: 0x24 */
  __IO uint32_t TXDR;     /*!< I2C Transmit data register,        Address offset: 0x28 */
} I2C_TypeDef;

/**
  * @brief Independent WATCHDOG
  */

typedef struct
{
  __IO uint32_t KR;   /*!< IWDG Key register,       Address offset: 0x00 */
  __IO uint32_t PR;   /*!< IWDG Prescaler register, Address offset: 0x04 */
  __IO uint32_t RLR;  /*!< IWDG Reload register,    Address offset: 0x08 */
  __IO uint32_t SR;   /*!< IWDG Status register,    Address offset: 0x0C */
  __IO uint32_t WINR; /*!< IWDG Window register,    Address offset: 0x10 */
} IWDG_TypeDef;


/**
  * @brief LCD-TFT Display Controller
  */

typedef struct
{
  uint32_t      RESERVED0[2];  /*!< Reserved, 0x00-0x04 */
  __IO uint32_t SSCR;          /*!< LTDC Synchronization Size Configuration Register,    Address offset: 0x08 */
  __IO uint32_t BPCR;          /*!< LTDC Back Porch Configuration Register,              Address offset: 0x0C */
  __IO uint32_t AWCR;          /*!< LTDC Active Width Configuration Register,            Address offset: 0x10 */
  __IO uint32_t TWCR;          /*!< LTDC Total Width Configuration Register,             Address offset: 0x14 */
  __IO uint32_t GCR;           /*!< LTDC Global Control Register,                        Address offset: 0x18 */
  uint32_t      RESERVED1[2];  /*!< Reserved, 0x1C-0x20 */
  __IO uint32_t SRCR;          /*!< LTDC Shadow Reload Configuration Register,           Address offset: 0x24 */
  uint32_t      RESERVED2[1];  /*!< Reserved, 0x28 */
  __IO uint32_t BCCR;          /*!< LTDC Background Color Configuration Register,        Address offset: 0x2C */
  uint32_t      RESERVED3[1];  /*!< Reserved, 0x30 */
  __IO uint32_t IER;           /*!< LTDC Interrupt Enable Register,                      Address offset: 0x34 */
  __IO uint32_t ISR;           /*!< LTDC Interrupt Status Register,                      Address offset: 0x38 */
  __IO uint32_t ICR;           /*!< LTDC Interrupt Clear Register,                       Address offset: 0x3C */
  __IO uint32_t LIPCR;         /*!< LTDC Line Interrupt Position Configuration Register, Address offset: 0x40 */
  __IO uint32_t CPSR;          /*!< LTDC Current Position Status Register,               Address offset: 0x44 */
  __IO uint32_t CDSR;         /*!< LTDC Current Display Status Register,                 Address offset: 0x48 */
} LTDC_TypeDef;

/**
  * @brief LCD-TFT Display layer x Controller
  */

typedef struct
{
  __IO uint32_t CR;            /*!< LTDC Layerx Control Register                                  Address offset: 0x84 */
  __IO uint32_t WHPCR;         /*!< LTDC Layerx Window Horizontal Position Configuration Register Address offset: 0x88 */
  __IO uint32_t WVPCR;         /*!< LTDC Layerx Window Vertical Position Configuration Register   Address offset: 0x8C */
  __IO uint32_t CKCR;          /*!< LTDC Layerx Color Keying Configuration Register               Address offset: 0x90 */
  __IO uint32_t PFCR;          /*!< LTDC Layerx Pixel Format Configuration Register               Address offset: 0x94 */
  __IO uint32_t CACR;          /*!< LTDC Layerx Constant Alpha Configuration Register             Address offset: 0x98 */
  __IO uint32_t DCCR;          /*!< LTDC Layerx Default Color Configuration Register              Address offset: 0x9C */
  __IO uint32_t BFCR;          /*!< LTDC Layerx Blending Factors Configuration Register           Address offset: 0xA0 */
  uint32_t      RESERVED0[2];  /*!< Reserved */
  __IO uint32_t CFBAR;         /*!< LTDC Layerx Color Frame Buffer Address Register               Address offset: 0xAC */
  __IO uint32_t CFBLR;         /*!< LTDC Layerx Color Frame Buffer Length Register                Address offset: 0xB0 */
  __IO uint32_t CFBLNR;        /*!< LTDC Layerx ColorFrame Buffer Line Number Register            Address offset: 0xB4 */
  uint32_t      RESERVED1[3];  /*!< Reserved */
  __IO uint32_t CLUTWR;        /*!< LTDC Layerx CLUT Write Register                               Address offset: 0x144 */

} LTDC_Layer_TypeDef;

/**
  * @brief Power Control
  */

typedef struct
{
  __IO uint32_t CR1;   /*!< PWR power control register 1,        Address offset: 0x00 */
  __IO uint32_t CSR1;  /*!< PWR power control/status register 2, Address offset: 0x04 */
  __IO uint32_t CR2;   /*!< PWR power control register 2,        Address offset: 0x08 */
  __IO uint32_t CSR2;  /*!< PWR power control/status register 2, Address offset: 0x0C */
} PWR_TypeDef;


/**
  * @brief Reset and Clock Control
  */

typedef struct
{
  __IO uint32_t CR;            /*!< RCC clock control register,                                  Address offset: 0x00 */
  __IO uint32_t PLLCFGR;       /*!< RCC PLL configuration register,                              Address offset: 0x04 */
  __IO uint32_t CFGR;          /*!< RCC clock configuration register,                            Address offset: 0x08 */
  __IO uint32_t CIR;           /*!< RCC clock interrupt register,                                Address offset: 0x0C */
  __IO uint32_t AHB1RSTR;      /*!< RCC AHB1 peripheral reset register,                          Address offset: 0x10 */
  __IO uint32_t AHB2RSTR;      /*!< RCC AHB2 peripheral reset register,                          Address offset: 0x14 */
  __IO uint32_t AHB3RSTR;      /*!< RCC AHB3 peripheral reset register,                          Address offset: 0x18 */
  uint32_t      RESERVED0;     /*!< Reserved, 0x1C                                                                    */
  __IO uint32_t APB1RSTR;      /*!< RCC APB1 peripheral reset register,                          Address offset: 0x20 */
  __IO uint32_t APB2RSTR;      /*!< RCC APB2 peripheral reset register,                          Address offset: 0x24 */
  uint32_t      RESERVED1[2];  /*!< Reserved, 0x28-0x2C                                                               */
  __IO uint32_t AHB1ENR;       /*!< RCC AHB1 peripheral clock register,                          Address offset: 0x30 */
  __IO uint32_t AHB2ENR;       /*!< RCC AHB2 peripheral clock register,                          Address offset: 0x34 */
  __IO uint32_t AHB3ENR;       /*!< RCC AHB3 peripheral clock register,                          Address offset: 0x38 */
  uint32_t      RESERVED2;     /*!< Reserved, 0x3C                                                                    */
  __IO uint32_t APB1ENR;       /*!< RCC APB1 peripheral clock enable register,                   Address offset: 0x40 */
  __IO uint32_t APB2ENR;       /*!< RCC APB2 peripheral clock enable register,                   Address offset: 0x44 */
  uint32_t      RESERVED3[2];  /*!< Reserved, 0x48-0x4C                                                               */
  __IO uint32_t AHB1LPENR;     /*!< RCC AHB1 peripheral clock enable in low power mode register, Address offset: 0x50 */
  __IO uint32_t AHB2LPENR;     /*!< RCC AHB2 peripheral clock enable in low power mode register, Address offset: 0x54 */
  __IO uint32_t AHB3LPENR;     /*!< RCC AHB3 peripheral clock enable in low power mode register, Address offset: 0x58 */
  uint32_t      RESERVED4;     /*!< Reserved, 0x5C                                                                    */
  __IO uint32_t APB1LPENR;     /*!< RCC APB1 peripheral clock enable in low power mode register, Address offset: 0x60 */
  __IO uint32_t APB2LPENR;     /*!< RCC APB2 peripheral clock enable in low power mode register, Address offset: 0x64 */
  uint32_t      RESERVED5[2];  /*!< Reserved, 0x68-0x6C                                                               */
  __IO uint32_t BDCR;          /*!< RCC Backup domain control register,                          Address offset: 0x70 */
  __IO uint32_t CSR;           /*!< RCC clock control & status register,                         Address offset: 0x74 */
  uint32_t      RESERVED6[2];  /*!< Reserved, 0x78-0x7C                                                               */
  __IO uint32_t SSCGR;         /*!< RCC spread spectrum clock generation register,               Address offset: 0x80 */
  __IO uint32_t PLLI2SCFGR;    /*!< RCC PLLI2S configuration register,                           Address offset: 0x84 */
  __IO uint32_t PLLSAICFGR;    /*!< RCC PLLSAI configuration register,                           Address offset: 0x88 */
  __IO uint32_t DCKCFGR1;      /*!< RCC Dedicated Clocks configuration register1,                 Address offset: 0x8C */
  __IO uint32_t DCKCFGR2;      /*!< RCC Dedicated Clocks configuration register 2,               Address offset: 0x90 */

} RCC_TypeDef;

/**
  * @brief Real-Time Clock
  */

typedef struct
{
  __IO uint32_t TR;         /*!< RTC time register,                                         Address offset: 0x00 */
  __IO uint32_t DR;         /*!< RTC date register,                                         Address offset: 0x04 */
  __IO uint32_t CR;         /*!< RTC control register,                                      Address offset: 0x08 */
  __IO uint32_t ISR;        /*!< RTC initialization and status register,                    Address offset: 0x0C */
  __IO uint32_t PRER;       /*!< RTC prescaler register,                                    Address offset: 0x10 */
  __IO uint32_t WUTR;       /*!< RTC wakeup timer register,                                 Address offset: 0x14 */
       uint32_t reserved;   /*!< Reserved  */
  __IO uint32_t ALRMAR;     /*!< RTC alarm A register,                                      Address offset: 0x1C */
  __IO uint32_t ALRMBR;     /*!< RTC alarm B register,                                      Address offset: 0x20 */
  __IO uint32_t WPR;        /*!< RTC write protection register,                             Address offset: 0x24 */
  __IO uint32_t SSR;        /*!< RTC sub second register,                                   Address offset: 0x28 */
  __IO uint32_t SHIFTR;     /*!< RTC shift control register,                                Address offset: 0x2C */
  __IO uint32_t TSTR;       /*!< RTC time stamp time register,                              Address offset: 0x30 */
  __IO uint32_t TSDR;       /*!< RTC time stamp date register,                              Address offset: 0x34 */
  __IO uint32_t TSSSR;      /*!< RTC time-stamp sub second register,                        Address offset: 0x38 */
  __IO uint32_t CALR;       /*!< RTC calibration register,                                  Address offset: 0x3C */
  __IO uint32_t TAMPCR;     /*!< RTC tamper configuration register,                         Address offset: 0x40 */
  __IO uint32_t ALRMASSR;   /*!< RTC alarm A sub second register,                           Address offset: 0x44 */
  __IO uint32_t ALRMBSSR;   /*!< RTC alarm B sub second register,                           Address offset: 0x48 */
  __IO uint32_t OR;         /*!< RTC option register,                                       Address offset: 0x4C */
  __IO uint32_t BKP0R;      /*!< RTC backup register 0,                                     Address offset: 0x50 */
  __IO uint32_t BKP1R;      /*!< RTC backup register 1,                                     Address offset: 0x54 */
  __IO uint32_t BKP2R;      /*!< RTC backup register 2,                                     Address offset: 0x58 */
  __IO uint32_t BKP3R;      /*!< RTC backup register 3,                                     Address offset: 0x5C */
  __IO uint32_t BKP4R;      /*!< RTC backup register 4,                                     Address offset: 0x60 */
  __IO uint32_t BKP5R;      /*!< RTC backup register 5,                                     Address offset: 0x64 */
  __IO uint32_t BKP6R;      /*!< RTC backup register 6,                                     Address offset: 0x68 */
  __IO uint32_t BKP7R;      /*!< RTC backup register 7,                                     Address offset: 0x6C */
  __IO uint32_t BKP8R;      /*!< RTC backup register 8,                                     Address offset: 0x70 */
  __IO uint32_t BKP9R;      /*!< RTC backup register 9,                                     Address offset: 0x74 */
  __IO uint32_t BKP10R;     /*!< RTC backup register 10,                                    Address offset: 0x78 */
  __IO uint32_t BKP11R;     /*!< RTC backup register 11,                                    Address offset: 0x7C */
  __IO uint32_t BKP12R;     /*!< RTC backup register 12,                                    Address offset: 0x80 */
  __IO uint32_t BKP13R;     /*!< RTC backup register 13,                                    Address offset: 0x84 */
  __IO uint32_t BKP14R;     /*!< RTC backup register 14,                                    Address offset: 0x88 */
  __IO uint32_t BKP15R;     /*!< RTC backup register 15,                                    Address offset: 0x8C */
  __IO uint32_t BKP16R;     /*!< RTC backup register 16,                                    Address offset: 0x90 */
  __IO uint32_t BKP17R;     /*!< RTC backup register 17,                                    Address offset: 0x94 */
  __IO uint32_t BKP18R;     /*!< RTC backup register 18,                                    Address offset: 0x98 */
  __IO uint32_t BKP19R;     /*!< RTC backup register 19,                                    Address offset: 0x9C */
  __IO uint32_t BKP20R;     /*!< RTC backup register 20,                                    Address offset: 0xA0 */
  __IO uint32_t BKP21R;     /*!< RTC backup register 21,                                    Address offset: 0xA4 */
  __IO uint32_t BKP22R;     /*!< RTC backup register 22,                                    Address offset: 0xA8 */
  __IO uint32_t BKP23R;     /*!< RTC backup register 23,                                    Address offset: 0xAC */
  __IO uint32_t BKP24R;     /*!< RTC backup register 24,                                    Address offset: 0xB0 */
  __IO uint32_t BKP25R;     /*!< RTC backup register 25,                                    Address offset: 0xB4 */
  __IO uint32_t BKP26R;     /*!< RTC backup register 26,                                    Address offset: 0xB8 */
  __IO uint32_t BKP27R;     /*!< RTC backup register 27,                                    Address offset: 0xBC */
  __IO uint32_t BKP28R;     /*!< RTC backup register 28,                                    Address offset: 0xC0 */
  __IO uint32_t BKP29R;     /*!< RTC backup register 29,                                    Address offset: 0xC4 */
  __IO uint32_t BKP30R;     /*!< RTC backup register 30,                                    Address offset: 0xC8 */
  __IO uint32_t BKP31R;     /*!< RTC backup register 31,                                    Address offset: 0xCC */
} RTC_TypeDef;


/**
  * @brief Serial Audio Interface
  */

typedef struct
{
  __IO uint32_t GCR;      /*!< SAI global configuration register,        Address offset: 0x00 */
} SAI_TypeDef;

typedef struct
{
  __IO uint32_t CR1;      /*!< SAI block x configuration register 1,     Address offset: 0x04 */
  __IO uint32_t CR2;      /*!< SAI block x configuration register 2,     Address offset: 0x08 */
  __IO uint32_t FRCR;     /*!< SAI block x frame configuration register, Address offset: 0x0C */
  __IO uint32_t SLOTR;    /*!< SAI block x slot register,                Address offset: 0x10 */
  __IO uint32_t IMR;      /*!< SAI block x interrupt mask register,      Address offset: 0x14 */
  __IO uint32_t SR;       /*!< SAI block x status register,              Address offset: 0x18 */
  __IO uint32_t CLRFR;    /*!< SAI block x clear flag register,          Address offset: 0x1C */
  __IO uint32_t DR;       /*!< SAI block x data register,                Address offset: 0x20 */
} SAI_Block_TypeDef;

/**
  * @brief SPDIF-RX Interface
  */

typedef struct
{
  __IO uint32_t   CR;           /*!< Control register,                   Address offset: 0x00 */
  __IO uint32_t   IMR;          /*!< Interrupt mask register,            Address offset: 0x04 */
  __IO uint32_t   SR;           /*!< Status register,                    Address offset: 0x08 */
  __IO uint32_t   IFCR;         /*!< Interrupt Flag Clear register,      Address offset: 0x0C */
  __IO uint32_t   DR;           /*!< Data input register,                Address offset: 0x10 */
  __IO uint32_t   CSR;          /*!< Channel Status register,            Address offset: 0x14 */
  __IO uint32_t   DIR;          /*!< Debug Information register,         Address offset: 0x18 */
} SPDIFRX_TypeDef;

/**
  * @brief SD host Interface
  */

typedef struct
{
  __IO uint32_t POWER;          /*!< SDMMC power control register,    Address offset: 0x00 */
  __IO uint32_t CLKCR;          /*!< SDMMClock control register,     Address offset: 0x04 */
  __IO uint32_t ARG;            /*!< SDMMC argument register,         Address offset: 0x08 */
  __IO uint32_t CMD;            /*!< SDMMC command register,          Address offset: 0x0C */
  __I uint32_t  RESPCMD;        /*!< SDMMC command response register, Address offset: 0x10 */
  __I uint32_t  RESP1;          /*!< SDMMC response 1 register,       Address offset: 0x14 */
  __I uint32_t  RESP2;          /*!< SDMMC response 2 register,       Address offset: 0x18 */
  __I uint32_t  RESP3;          /*!< SDMMC response 3 register,       Address offset: 0x1C */
  __I uint32_t  RESP4;          /*!< SDMMC response 4 register,       Address offset: 0x20 */
  __IO uint32_t DTIMER;         /*!< SDMMC data timer register,       Address offset: 0x24 */
  __IO uint32_t DLEN;           /*!< SDMMC data length register,      Address offset: 0x28 */
  __IO uint32_t DCTRL;          /*!< SDMMC data control register,     Address offset: 0x2C */
  __I uint32_t  DCOUNT;         /*!< SDMMC data counter register,     Address offset: 0x30 */
  __I uint32_t  STA;            /*!< SDMMC status register,           Address offset: 0x34 */
  __IO uint32_t ICR;            /*!< SDMMC interrupt clear register,  Address offset: 0x38 */
  __IO uint32_t MASK;           /*!< SDMMC mask register,             Address offset: 0x3C */
  uint32_t      RESERVED0[2];   /*!< Reserved, 0x40-0x44                                  */
  __I uint32_t  FIFOCNT;        /*!< SDMMC FIFO counter register,     Address offset: 0x48 */
  uint32_t      RESERVED1[13];  /*!< Reserved, 0x4C-0x7C                                  */
  __IO uint32_t FIFO;           /*!< SDMMC data FIFO register,        Address offset: 0x80 */
} SDMMC_TypeDef;

/**
  * @brief Serial Peripheral Interface
  */

typedef struct
{
  __IO uint32_t CR1;        /*!< SPI control register 1 (not used in I2S mode),      Address offset: 0x00 */
  __IO uint32_t CR2;        /*!< SPI control register 2,                             Address offset: 0x04 */
  __IO uint32_t SR;         /*!< SPI status register,                                Address offset: 0x08 */
  __IO uint32_t DR;         /*!< SPI data register,                                  Address offset: 0x0C */
  __IO uint32_t CRCPR;      /*!< SPI CRC polynomial register (not used in I2S mode), Address offset: 0x10 */
  __IO uint32_t RXCRCR;     /*!< SPI RX CRC register (not used in I2S mode),         Address offset: 0x14 */
  __IO uint32_t TXCRCR;     /*!< SPI TX CRC register (not used in I2S mode),         Address offset: 0x18 */
  __IO uint32_t I2SCFGR;    /*!< SPI_I2S configuration register,                     Address offset: 0x1C */
  __IO uint32_t I2SPR;      /*!< SPI_I2S prescaler register,                         Address offset: 0x20 */
} SPI_TypeDef;

/**
  * @brief QUAD Serial Peripheral Interface
  */

typedef struct
{
  __IO uint32_t CR;       /*!< QUADSPI Control register,                           Address offset: 0x00 */
  __IO uint32_t DCR;      /*!< QUADSPI Device Configuration register,              Address offset: 0x04 */
  __IO uint32_t SR;       /*!< QUADSPI Status register,                            Address offset: 0x08 */
  __IO uint32_t FCR;      /*!< QUADSPI Flag Clear register,                        Address offset: 0x0C */
  __IO uint32_t DLR;      /*!< QUADSPI Data Length register,                       Address offset: 0x10 */
  __IO uint32_t CCR;      /*!< QUADSPI Communication Configuration register,       Address offset: 0x14 */
  __IO uint32_t AR;       /*!< QUADSPI Address register,                           Address offset: 0x18 */
  __IO uint32_t ABR;      /*!< QUADSPI Alternate Bytes register,                   Address offset: 0x1C */
  __IO uint32_t DR;       /*!< QUADSPI Data register,                              Address offset: 0x20 */
  __IO uint32_t PSMKR;    /*!< QUADSPI Polling Status Mask register,               Address offset: 0x24 */
  __IO uint32_t PSMAR;    /*!< QUADSPI Polling Status Match register,              Address offset: 0x28 */
  __IO uint32_t PIR;      /*!< QUADSPI Polling Interval register,                  Address offset: 0x2C */
  __IO uint32_t LPTR;     /*!< QUADSPI Low Power Timeout register,                 Address offset: 0x30 */
} QUADSPI_TypeDef;

/**
  * @brief TIM
  */

typedef struct
{
  __IO uint32_t CR1;         /*!< TIM control register 1,              Address offset: 0x00 */
  __IO uint32_t CR2;         /*!< TIM control register 2,              Address offset: 0x04 */
  __IO uint32_t SMCR;        /*!< TIM slave mode control register,     Address offset: 0x08 */
  __IO uint32_t DIER;        /*!< TIM DMA/interrupt enable register,   Address offset: 0x0C */
  __IO uint32_t SR;          /*!< TIM status register,                 Address offset: 0x10 */
  __IO uint32_t EGR;         /*!< TIM event generation register,       Address offset: 0x14 */
  __IO uint32_t CCMR1;       /*!< TIM capture/compare mode register 1, Address offset: 0x18 */
  __IO uint32_t CCMR2;       /*!< TIM capture/compare mode register 2, Address offset: 0x1C */
  __IO uint32_t CCER;        /*!< TIM capture/compare enable register, Address offset: 0x20 */
  __IO uint32_t CNT;         /*!< TIM counter register,                Address offset: 0x24 */
  __IO uint32_t PSC;         /*!< TIM prescaler,                       Address offset: 0x28 */
  __IO uint32_t ARR;         /*!< TIM auto-reload register,            Address offset: 0x2C */
  __IO uint32_t RCR;         /*!< TIM repetition counter register,     Address offset: 0x30 */
  __IO uint32_t CCR1;        /*!< TIM capture/compare register 1,      Address offset: 0x34 */
  __IO uint32_t CCR2;        /*!< TIM capture/compare register 2,      Address offset: 0x38 */
  __IO uint32_t CCR3;        /*!< TIM capture/compare register 3,      Address offset: 0x3C */
  __IO uint32_t CCR4;        /*!< TIM capture/compare register 4,      Address offset: 0x40 */
  __IO uint32_t BDTR;        /*!< TIM break and dead-time register,    Address offset: 0x44 */
  __IO uint32_t DCR;         /*!< TIM DMA control register,            Address offset: 0x48 */
  __IO uint32_t DMAR;        /*!< TIM DMA address for full transfer,   Address offset: 0x4C */
  __IO uint32_t OR;          /*!< TIM option register,                 Address offset: 0x50 */
  __IO uint32_t CCMR3;       /*!< TIM capture/compare mode register 3,      Address offset: 0x54 */
  __IO uint32_t CCR5;        /*!< TIM capture/compare mode register5,       Address offset: 0x58 */
  __IO uint32_t CCR6;        /*!< TIM capture/compare mode register6,       Address offset: 0x5C */

} TIM_TypeDef;

/**
  * @brief LPTIMIMER
  */
typedef struct
{
  __IO uint32_t ISR;      /*!< LPTIM Interrupt and Status register,                Address offset: 0x00 */
  __IO uint32_t ICR;      /*!< LPTIM Interrupt Clear register,                     Address offset: 0x04 */
  __IO uint32_t IER;      /*!< LPTIM Interrupt Enable register,                    Address offset: 0x08 */
  __IO uint32_t CFGR;     /*!< LPTIM Configuration register,                       Address offset: 0x0C */
  __IO uint32_t CR;       /*!< LPTIM Control register,                             Address offset: 0x10 */
  __IO uint32_t CMP;      /*!< LPTIM Compare register,                             Address offset: 0x14 */
  __IO uint32_t ARR;      /*!< LPTIM Autoreload register,                          Address offset: 0x18 */
  __IO uint32_t CNT;      /*!< LPTIM Counter register,                             Address offset: 0x1C */
} LPTIM_TypeDef;


/**
  * @brief Universal Synchronous Asynchronous Receiver Transmitter
  */

typedef struct
{
  __IO uint32_t CR1;    /*!< USART Control register 1,                 Address offset: 0x00 */
  __IO uint32_t CR2;    /*!< USART Control register 2,                 Address offset: 0x04 */
  __IO uint32_t CR3;    /*!< USART Control register 3,                 Address offset: 0x08 */
  __IO uint32_t BRR;    /*!< USART Baud rate register,                 Address offset: 0x0C */
  __IO uint32_t GTPR;   /*!< USART Guard time and prescaler register,  Address offset: 0x10 */
  __IO uint32_t RTOR;   /*!< USART Receiver Time Out register,         Address offset: 0x14 */
  __IO uint32_t RQR;    /*!< USART Request register,                   Address offset: 0x18 */
  __IO uint32_t ISR;    /*!< USART Interrupt and status register,      Address offset: 0x1C */
  __IO uint32_t ICR;    /*!< USART Interrupt flag Clear register,      Address offset: 0x20 */
  __IO uint32_t RDR;    /*!< USART Receive Data register,              Address offset: 0x24 */
  __IO uint32_t TDR;    /*!< USART Transmit Data register,             Address offset: 0x28 */
} USART_TypeDef;


/**
  * @brief Window WATCHDOG
  */

typedef struct
{
  __IO uint32_t CR;   /*!< WWDG Control register,       Address offset: 0x00 */
  __IO uint32_t CFR;  /*!< WWDG Configuration register, Address offset: 0x04 */
  __IO uint32_t SR;   /*!< WWDG Status register,        Address offset: 0x08 */
} WWDG_TypeDef;


/**
  * @brief RNG
  */

typedef struct
{
  __IO uint32_t CR;  /*!< RNG control register, Address offset: 0x00 */
  __IO uint32_t SR;  /*!< RNG status register,  Address offset: 0x04 */
  __IO uint32_t DR;  /*!< RNG data register,    Address offset: 0x08 */
} RNG_TypeDef;

/**
  * @}
  */

/**
  * @brief USB_OTG_Core_Registers
  */
typedef struct
{
 __IO uint32_t GOTGCTL;               /*!< USB_OTG Control and Status Register          000h */
  __IO uint32_t GOTGINT;              /*!< USB_OTG Interrupt Register                   004h */
  __IO uint32_t GAHBCFG;              /*!< Core AHB Configuration Register              008h */
  __IO uint32_t GUSBCFG;              /*!< Core USB Configuration Register              00Ch */
  __IO uint32_t GRSTCTL;              /*!< Core Reset Register                          010h */
  __IO uint32_t GINTSTS;              /*!< Core Interrupt Register                      014h */
  __IO uint32_t GINTMSK;              /*!< Core Interrupt Mask Register                 018h */
  __IO uint32_t GRXSTSR;              /*!< Receive Sts Q Read Register                  01Ch */
  __IO uint32_t GRXSTSP;              /*!< Receive Sts Q Read & POP Register            020h */
  __IO uint32_t GRXFSIZ;              /*!< Receive FIFO Size Register                   024h */
  __IO uint32_t DIEPTXF0_HNPTXFSIZ;   /*!< EP0 / Non Periodic Tx FIFO Size Register     028h */
  __IO uint32_t HNPTXSTS;             /*!< Non Periodic Tx FIFO/Queue Sts reg           02Ch */
  uint32_t Reserved30[2];             /*!< Reserved                                     030h */
  __IO uint32_t GCCFG;                /*!< General Purpose IO Register                  038h */
  __IO uint32_t CID;                  /*!< User ID Register                             03Ch */
  uint32_t  Reserved5[3];             /*!< Reserved                                040h-048h */
  __IO uint32_t GHWCFG3;              /*!< User HW config3                              04Ch */
  uint32_t  Reserved6;                /*!< Reserved                                     050h */
  __IO uint32_t GLPMCFG;              /*!< LPM Register                                 054h */
  __IO uint32_t GPWRDN;               /*!< Power Down Register                          058h */
  __IO uint32_t GDFIFOCFG;            /*!< DFIFO Software Config Register               05Ch */
   __IO uint32_t GADPCTL;             /*!< ADP Timer, Control and Status Register       60Ch */
    uint32_t  Reserved43[39];         /*!< Reserved                                058h-0FFh */
  __IO uint32_t HPTXFSIZ;             /*!< Host Periodic Tx FIFO Size Reg               100h */
  __IO uint32_t DIEPTXF[0x0F];        /*!< dev Periodic Transmit FIFO */
} USB_OTG_GlobalTypeDef;


/**
  * @brief USB_OTG_device_Registers
  */
typedef struct
{
  __IO uint32_t DCFG;            /*!< dev Configuration Register   800h */
  __IO uint32_t DCTL;            /*!< dev Control Register         804h */
  __IO uint32_t DSTS;            /*!< dev Status Register (RO)     808h */
  uint32_t Reserved0C;           /*!< Reserved                     80Ch */
  __IO uint32_t DIEPMSK;         /*!< dev IN Endpoint Mask         810h */
  __IO uint32_t DOEPMSK;         /*!< dev OUT Endpoint Mask        814h */
  __IO uint32_t DAINT;           /*!< dev All Endpoints Itr Reg    818h */
  __IO uint32_t DAINTMSK;        /*!< dev All Endpoints Itr Mask   81Ch */
  uint32_t  Reserved20;          /*!< Reserved                     820h */
  uint32_t Reserved9;            /*!< Reserved                     824h */
  __IO uint32_t DVBUSDIS;        /*!< dev VBUS discharge Register  828h */
  __IO uint32_t DVBUSPULSE;      /*!< dev VBUS Pulse Register      82Ch */
  __IO uint32_t DTHRCTL;         /*!< dev threshold                830h */
  __IO uint32_t DIEPEMPMSK;      /*!< dev empty msk                834h */
  __IO uint32_t DEACHINT;        /*!< dedicated EP interrupt       838h */
  __IO uint32_t DEACHMSK;        /*!< dedicated EP msk             83Ch */
  uint32_t Reserved40;           /*!< dedicated EP mask            840h */
  __IO uint32_t DINEP1MSK;       /*!< dedicated EP mask            844h */
  uint32_t  Reserved44[15];      /*!< Reserved                 844-87Ch */
  __IO uint32_t DOUTEP1MSK;      /*!< dedicated EP msk             884h */
} USB_OTG_DeviceTypeDef;


/**
  * @brief USB_OTG_IN_Endpoint-Specific_Register
  */
typedef struct
{
  __IO uint32_t DIEPCTL;           /*!< dev IN Endpoint Control Reg    900h + (ep_num * 20h) + 00h */
  uint32_t Reserved04;             /*!< Reserved                       900h + (ep_num * 20h) + 04h */
  __IO uint32_t DIEPINT;           /*!< dev IN Endpoint Itr Reg        900h + (ep_num * 20h) + 08h */
  uint32_t Reserved0C;             /*!< Reserved                       900h + (ep_num * 20h) + 0Ch */
  __IO uint32_t DIEPTSIZ;          /*!< IN Endpoint Txfer Size         900h + (ep_num * 20h) + 10h */
  __IO uint32_t DIEPDMA;           /*!< IN Endpoint DMA Address Reg    900h + (ep_num * 20h) + 14h */
  __IO uint32_t DTXFSTS;           /*!< IN Endpoint Tx FIFO Status Reg 900h + (ep_num * 20h) + 18h */
  uint32_t Reserved18;             /*!< Reserved  900h+(ep_num*20h)+1Ch-900h+ (ep_num * 20h) + 1Ch */
} USB_OTG_INEndpointTypeDef;


/**
  * @brief USB_OTG_OUT_Endpoint-Specific_Registers
  */
typedef struct
{
  __IO uint32_t DOEPCTL;       /*!< dev OUT Endpoint Control Reg           B00h + (ep_num * 20h) + 00h */
  uint32_t Reserved04;         /*!< Reserved                               B00h + (ep_num * 20h) + 04h */
  __IO uint32_t DOEPINT;       /*!< dev OUT Endpoint Itr Reg               B00h + (ep_num * 20h) + 08h */
  uint32_t Reserved0C;         /*!< Reserved                               B00h + (ep_num * 20h) + 0Ch */
  __IO uint32_t DOEPTSIZ;      /*!< dev OUT Endpoint Txfer Size            B00h + (ep_num * 20h) + 10h */
  __IO uint32_t DOEPDMA;       /*!< dev OUT Endpoint DMA Address           B00h + (ep_num * 20h) + 14h */
  uint32_t Reserved18[2];      /*!< Reserved B00h + (ep_num * 20h) + 18h - B00h + (ep_num * 20h) + 1Ch */
} USB_OTG_OUTEndpointTypeDef;


/**
  * @brief USB_OTG_Host_Mode_Register_Structures
  */
typedef struct
{
  __IO uint32_t HCFG;             /*!< Host Configuration Register          400h */
  __IO uint32_t HFIR;             /*!< Host Frame Interval Register         404h */
  __IO uint32_t HFNUM;            /*!< Host Frame Nbr/Frame Remaining       408h */
  uint32_t Reserved40C;           /*!< Reserved                             40Ch */
  __IO uint32_t HPTXSTS;          /*!< Host Periodic Tx FIFO/ Queue Status  410h */
  __IO uint32_t HAINT;            /*!< Host All Channels Interrupt Register 414h */
  __IO uint32_t HAINTMSK;         /*!< Host All Channels Interrupt Mask     418h */
} USB_OTG_HostTypeDef;

/**
  * @brief USB_OTG_Host_Channel_Specific_Registers
  */
typedef struct
{
  __IO uint32_t HCCHAR;           /*!< Host Channel Characteristics Register    500h */
  __IO uint32_t HCSPLT;           /*!< Host Channel Split Control Register      504h */
  __IO uint32_t HCINT;            /*!< Host Channel Interrupt Register          508h */
  __IO uint32_t HCINTMSK;         /*!< Host Channel Interrupt Mask Register     50Ch */
  __IO uint32_t HCTSIZ;           /*!< Host Channel Transfer Size Register      510h */
  __IO uint32_t HCDMA;            /*!< Host Channel DMA Address Register        514h */
  uint32_t Reserved[2];           /*!< Reserved                                      */
} USB_OTG_HostChannelTypeDef;
/**
  * @}
  */




/** @addtogroup Peripheral_memory_map
  * @{
  */
#define RAMITCM_BASE           0x00000000U /*!< Base address of : 16KB RAM reserved for CPU execution/instruction accessible over ITCM  */
#define FLASHITCM_BASE         0x00200000U /*!< Base address of : (up to 1 MB) embedded FLASH memory  accessible over ITCM              */
#define FLASHAXI_BASE          0x08000000U /*!< Base address of : (up to 1 MB) embedded FLASH memory accessible over AXI                */
#define RAMDTCM_BASE           0x20000000U /*!< Base address of : 64KB system data RAM accessible over DTCM                             */
#define PERIPH_BASE            0x40000000U /*!< Base address of : AHB/ABP Peripherals                                                   */
#define BKPSRAM_BASE           0x40024000U /*!< Base address of : Backup SRAM(4 KB)                                                     */
#define QSPI_BASE              0x90000000U /*!< Base address of : QSPI memories  accessible over AXI                                    */
#define FMC_R_BASE             0xA0000000U /*!< Base address of : FMC Control registers                                                 */
#define QSPI_R_BASE            0xA0001000U /*!< Base address of : QSPI Control  registers                                               */
#define SRAM1_BASE             0x20010000U /*!< Base address of : 240KB RAM1 accessible over AXI/AHB                                    */
#define SRAM2_BASE             0x2004C000U /*!< Base address of : 16KB RAM2 accessible over AXI/AHB                                     */
#define FLASH_END              0x080FFFFFU /*!< FLASH end address */

/* Legacy define */
#define FLASH_BASE     FLASHAXI_BASE

/*!< Peripheral memory map */
#define APB1PERIPH_BASE        PERIPH_BASE
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000U)
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000U)
#define AHB2PERIPH_BASE       (PERIPH_BASE + 0x10000000U)

/*!< APB1 peripherals */
#define TIM2_BASE             (APB1PERIPH_BASE + 0x0000U)
#define TIM3_BASE             (APB1PERIPH_BASE + 0x0400U)
#define TIM4_BASE             (APB1PERIPH_BASE + 0x0800U)
#define TIM5_BASE             (APB1PERIPH_BASE + 0x0C00U)
#define TIM6_BASE             (APB1PERIPH_BASE + 0x1000U)
#define TIM7_BASE             (APB1PERIPH_BASE + 0x1400U)
#define TIM12_BASE            (APB1PERIPH_BASE + 0x1800U)
#define TIM13_BASE            (APB1PERIPH_BASE + 0x1C00U)
#define TIM14_BASE            (APB1PERIPH_BASE + 0x2000U)
#define LPTIM1_BASE           (APB1PERIPH_BASE + 0x2400U)
#define RTC_BASE              (APB1PERIPH_BASE + 0x2800U)
#define WWDG_BASE             (APB1PERIPH_BASE + 0x2C00U)
#define IWDG_BASE             (APB1PERIPH_BASE + 0x3000U)
#define SPI2_BASE             (APB1PERIPH_BASE + 0x3800U)
#define SPI3_BASE             (APB1PERIPH_BASE + 0x3C00U)
#define SPDIFRX_BASE          (APB1PERIPH_BASE + 0x4000U)
#define USART2_BASE           (APB1PERIPH_BASE + 0x4400U)
#define USART3_BASE           (APB1PERIPH_BASE + 0x4800U)
#define UART4_BASE            (APB1PERIPH_BASE + 0x4C00U)
#define UART5_BASE            (APB1PERIPH_BASE + 0x5000U)
#define I2C1_BASE             (APB1PERIPH_BASE + 0x5400U)
#define I2C2_BASE             (APB1PERIPH_BASE + 0x5800U)
#define I2C3_BASE             (APB1PERIPH_BASE + 0x5C00U)
#define I2C4_BASE             (APB1PERIPH_BASE + 0x6000U)
#define CAN1_BASE             (APB1PERIPH_BASE + 0x6400U)
#define CAN2_BASE             (APB1PERIPH_BASE + 0x6800U)
#define CEC_BASE              (APB1PERIPH_BASE + 0x6C00U)
#define PWR_BASE              (APB1PERIPH_BASE + 0x7000U)
#define DAC_BASE              (APB1PERIPH_BASE + 0x7400U)
#define UART7_BASE            (APB1PERIPH_BASE + 0x7800U)
#define UART8_BASE            (APB1PERIPH_BASE + 0x7C00U)

/*!< APB2 peripherals */
#define TIM1_BASE             (APB2PERIPH_BASE + 0x0000U)
#define TIM8_BASE             (APB2PERIPH_BASE + 0x0400U)
#define USART1_BASE           (APB2PERIPH_BASE + 0x1000U)
#define USART6_BASE           (APB2PERIPH_BASE + 0x1400U)
#define ADC1_BASE             (APB2PERIPH_BASE + 0x2000U)
#define ADC2_BASE             (APB2PERIPH_BASE + 0x2100U)
#define ADC3_BASE             (APB2PERIPH_BASE + 0x2200U)
#define ADC_BASE              (APB2PERIPH_BASE + 0x2300U)
#define SDMMC1_BASE           (APB2PERIPH_BASE + 0x2C00U)
#define SPI1_BASE             (APB2PERIPH_BASE + 0x3000U)
#define SPI4_BASE             (APB2PERIPH_BASE + 0x3400U)
#define SYSCFG_BASE           (APB2PERIPH_BASE + 0x3800U)
#define EXTI_BASE             (APB2PERIPH_BASE + 0x3C00U)
#define TIM9_BASE             (APB2PERIPH_BASE + 0x4000U)
#define TIM10_BASE            (APB2PERIPH_BASE + 0x4400U)
#define TIM11_BASE            (APB2PERIPH_BASE + 0x4800U)
#define SPI5_BASE             (APB2PERIPH_BASE + 0x5000U)
#define SPI6_BASE             (APB2PERIPH_BASE + 0x5400U)
#define SAI1_BASE             (APB2PERIPH_BASE + 0x5800U)
#define SAI2_BASE             (APB2PERIPH_BASE + 0x5C00U)
#define SAI1_Block_A_BASE     (SAI1_BASE + 0x004U)
#define SAI1_Block_B_BASE     (SAI1_BASE + 0x024U)
#define SAI2_Block_A_BASE     (SAI2_BASE + 0x004U)
#define SAI2_Block_B_BASE     (SAI2_BASE + 0x024U)
#define LTDC_BASE             (APB2PERIPH_BASE + 0x6800U)
#define LTDC_Layer1_BASE      (LTDC_BASE + 0x84U)
#define LTDC_Layer2_BASE      (LTDC_BASE + 0x104U)
/*!< AHB1 peripherals */
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000U)
#define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400U)
#define GPIOC_BASE            (AHB1PERIPH_BASE + 0x0800U)
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00U)
#define GPIOE_BASE            (AHB1PERIPH_BASE + 0x1000U)
#define GPIOF_BASE            (AHB1PERIPH_BASE + 0x1400U)
#define GPIOG_BASE            (AHB1PERIPH_BASE + 0x1800U)
#define GPIOH_BASE            (AHB1PERIPH_BASE + 0x1C00U)
#define GPIOI_BASE            (AHB1PERIPH_BASE + 0x2000U)
#define GPIOJ_BASE            (AHB1PERIPH_BASE + 0x2400U)
#define GPIOK_BASE            (AHB1PERIPH_BASE + 0x2800U)
#define CRC_BASE              (AHB1PERIPH_BASE + 0x3000U)
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800U)
#define FLASH_R_BASE          (AHB1PERIPH_BASE + 0x3C00U)
#define UID_BASE              0x1FF0F420U                   /*!< Unique device ID register base address */
#define FLASHSIZE_BASE        0x1FF0F442U                   /*!< FLASH Size register base address */
#define PACKAGESIZE_BASE      0x1FFF7BF0U                   /*!< Package size register base address */
#define DMA1_BASE             (AHB1PERIPH_BASE + 0x6000U)
#define DMA1_Stream0_BASE     (DMA1_BASE + 0x010U)
#define DMA1_Stream1_BASE     (DMA1_BASE + 0x028U)
#define DMA1_Stream2_BASE     (DMA1_BASE + 0x040U)
#define DMA1_Stream3_BASE     (DMA1_BASE + 0x058U)
#define DMA1_Stream4_BASE     (DMA1_BASE + 0x070U)
#define DMA1_Stream5_BASE     (DMA1_BASE + 0x088U)
#define DMA1_Stream6_BASE     (DMA1_BASE + 0x0A0U)
#define DMA1_Stream7_BASE     (DMA1_BASE + 0x0B8U)
#define DMA2_BASE             (AHB1PERIPH_BASE + 0x6400U)
#define DMA2_Stream0_BASE     (DMA2_BASE + 0x010U)
#define DMA2_Stream1_BASE     (DMA2_BASE + 0x028U)
#define DMA2_Stream2_BASE     (DMA2_BASE + 0x040U)
#define DMA2_Stream3_BASE     (DMA2_BASE + 0x058U)
#define DMA2_Stream4_BASE     (DMA2_BASE + 0x070U)
#define DMA2_Stream5_BASE     (DMA2_BASE + 0x088U)
#define DMA2_Stream6_BASE     (DMA2_BASE + 0x0A0U)
#define DMA2_Stream7_BASE     (DMA2_BASE + 0x0B8U)
#define ETH_BASE              (AHB1PERIPH_BASE + 0x8000U)
#define ETH_MAC_BASE          (ETH_BASE)
#define ETH_MMC_BASE          (ETH_BASE + 0x0100U)
#define ETH_PTP_BASE          (ETH_BASE + 0x0700U)
#define ETH_DMA_BASE          (ETH_BASE + 0x1000U)
#define DMA2D_BASE            (AHB1PERIPH_BASE + 0xB000U)
/*!< AHB2 peripherals */
#define DCMI_BASE             (AHB2PERIPH_BASE + 0x50000U)
#define RNG_BASE              (AHB2PERIPH_BASE + 0x60800U)
/*!< FMC Bankx registers base address */
#define FMC_Bank1_R_BASE      (FMC_R_BASE + 0x0000U)
#define FMC_Bank1E_R_BASE     (FMC_R_BASE + 0x0104U)
#define FMC_Bank3_R_BASE      (FMC_R_BASE + 0x0080U)
#define FMC_Bank5_6_R_BASE    (FMC_R_BASE + 0x0140U)

/* Debug MCU registers base address */
#define DBGMCU_BASE           0xE0042000U

/*!< USB registers base address */
#define USB_OTG_HS_PERIPH_BASE               0x40040000U
#define USB_OTG_FS_PERIPH_BASE               0x50000000U

#define USB_OTG_GLOBAL_BASE                  0x000U
#define USB_OTG_DEVICE_BASE                  0x800U
#define USB_OTG_IN_ENDPOINT_BASE             0x900U
#define USB_OTG_OUT_ENDPOINT_BASE            0xB00U
#define USB_OTG_EP_REG_SIZE                  0x20U
#define USB_OTG_HOST_BASE                    0x400U
#define USB_OTG_HOST_PORT_BASE               0x440U
#define USB_OTG_HOST_CHANNEL_BASE            0x500U
#define USB_OTG_HOST_CHANNEL_SIZE            0x20U
#define USB_OTG_PCGCCTL_BASE                 0xE00U
#define USB_OTG_FIFO_BASE                    0x1000U
#define USB_OTG_FIFO_SIZE                    0x1000U

/**
  * @}
  */

/** @addtogroup Peripheral_declaration
  * @{
  */
#define TIM2                ((TIM_TypeDef *) TIM2_BASE)
#define TIM3                ((TIM_TypeDef *) TIM3_BASE)
#define TIM4                ((TIM_TypeDef *) TIM4_BASE)
#define TIM5                ((TIM_TypeDef *) TIM5_BASE)
#define TIM6                ((TIM_TypeDef *) TIM6_BASE)
#define TIM7                ((TIM_TypeDef *) TIM7_BASE)
#define TIM12               ((TIM_TypeDef *) TIM12_BASE)
#define TIM13               ((TIM_TypeDef *) TIM13_BASE)
#define TIM14               ((TIM_TypeDef *) TIM14_BASE)
#define LPTIM1              ((LPTIM_TypeDef *) LPTIM1_BASE)
#define RTC                 ((RTC_TypeDef *) RTC_BASE)
#define WWDG                ((WWDG_TypeDef *) WWDG_BASE)
#define IWDG                ((IWDG_TypeDef *) IWDG_BASE)
#define SPI2                ((SPI_TypeDef *) SPI2_BASE)
#define SPI3                ((SPI_TypeDef *) SPI3_BASE)
#define SPDIFRX             ((SPDIFRX_TypeDef *) SPDIFRX_BASE)
#define USART2              ((USART_TypeDef *) USART2_BASE)
#define USART3              ((USART_TypeDef *) USART3_BASE)
#define UART4               ((USART_TypeDef *) UART4_BASE)
#define UART5               ((USART_TypeDef *) UART5_BASE)
#define I2C1                ((I2C_TypeDef *) I2C1_BASE)
#define I2C2                ((I2C_TypeDef *) I2C2_BASE)
#define I2C3                ((I2C_TypeDef *) I2C3_BASE)
#define I2C4                ((I2C_TypeDef *) I2C4_BASE)
#define CAN1                ((CAN_TypeDef *) CAN1_BASE)
#define CAN2                ((CAN_TypeDef *) CAN2_BASE)
#define CEC                 ((CEC_TypeDef *) CEC_BASE)
#define PWR                 ((PWR_TypeDef *) PWR_BASE)
#define DAC                 ((DAC_TypeDef *) DAC_BASE)
#define UART7               ((USART_TypeDef *) UART7_BASE)
#define UART8               ((USART_TypeDef *) UART8_BASE)
#define TIM1                ((TIM_TypeDef *) TIM1_BASE)
#define TIM8                ((TIM_TypeDef *) TIM8_BASE)
#define USART1              ((USART_TypeDef *) USART1_BASE)
#define USART6              ((USART_TypeDef *) USART6_BASE)
#define ADC                 ((ADC_Common_TypeDef *) ADC_BASE)
#define ADC1                ((ADC_TypeDef *) ADC1_BASE)
#define ADC2                ((ADC_TypeDef *) ADC2_BASE)
#define ADC3                ((ADC_TypeDef *) ADC3_BASE)
#define SDMMC1              ((SDMMC_TypeDef *) SDMMC1_BASE)
#define SPI1                ((SPI_TypeDef *) SPI1_BASE)
#define SPI4                ((SPI_TypeDef *) SPI4_BASE)
#define SYSCFG              ((SYSCFG_TypeDef *) SYSCFG_BASE)
#define EXTI                ((EXTI_TypeDef *) EXTI_BASE)
#define TIM9                ((TIM_TypeDef *) TIM9_BASE)
#define TIM10               ((TIM_TypeDef *) TIM10_BASE)
#define TIM11               ((TIM_TypeDef *) TIM11_BASE)
#define SPI5                ((SPI_TypeDef *) SPI5_BASE)
#define SPI6                ((SPI_TypeDef *) SPI6_BASE)
#define SAI1                ((SAI_TypeDef *) SAI1_BASE)
#define SAI2                ((SAI_TypeDef *) SAI2_BASE)
#define SAI1_Block_A        ((SAI_Block_TypeDef *)SAI1_Block_A_BASE)
#define SAI1_Block_B        ((SAI_Block_TypeDef *)SAI1_Block_B_BASE)
#define SAI2_Block_A        ((SAI_Block_TypeDef *)SAI2_Block_A_BASE)
#define SAI2_Block_B        ((SAI_Block_TypeDef *)SAI2_Block_B_BASE)
#define LTDC                ((LTDC_TypeDef *)LTDC_BASE)
#define LTDC_Layer1         ((LTDC_Layer_TypeDef *)LTDC_Layer1_BASE)
#define LTDC_Layer2         ((LTDC_Layer_TypeDef *)LTDC_Layer2_BASE)
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE               ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOF               ((GPIO_TypeDef *) GPIOF_BASE)
#define GPIOG               ((GPIO_TypeDef *) GPIOG_BASE)
#define GPIOH               ((GPIO_TypeDef *) GPIOH_BASE)
#define GPIOI               ((GPIO_TypeDef *) GPIOI_BASE)
#define GPIOJ               ((GPIO_TypeDef *) GPIOJ_BASE)
#define GPIOK               ((GPIO_TypeDef *) GPIOK_BASE)
#define CRC                 ((CRC_TypeDef *) CRC_BASE)
#define RCC                 ((RCC_TypeDef *) RCC_BASE)
#define FLASH               ((FLASH_TypeDef *) FLASH_R_BASE)
#define DMA1                ((DMA_TypeDef *) DMA1_BASE)
#define DMA1_Stream0        ((DMA_Stream_TypeDef *) DMA1_Stream0_BASE)
#define DMA1_Stream1        ((DMA_Stream_TypeDef *) DMA1_Stream1_BASE)
#define DMA1_Stream2        ((DMA_Stream_TypeDef *) DMA1_Stream2_BASE)
#define DMA1_Stream3        ((DMA_Stream_TypeDef *) DMA1_Stream3_BASE)
#define DMA1_Stream4        ((DMA_Stream_TypeDef *) DMA1_Stream4_BASE)
#define DMA1_Stream5        ((DMA_Stream_TypeDef *) DMA1_Stream5_BASE)
#define DMA1_Stream6        ((DMA_Stream_TypeDef *) DMA1_Stream6_BASE)
#define DMA1_Stream7        ((DMA_Stream_TypeDef *) DMA1_Stream7_BASE)
#define DMA2                ((DMA_TypeDef *) DMA2_BASE)
#define DMA2_Stream0        ((DMA_Stream_TypeDef *) DMA2_Stream0_BASE)
#define DMA2_Stream1        ((DMA_Stream_TypeDef *) DMA2_Stream1_BASE)
#define DMA2_Stream2        ((DMA_Stream_TypeDef *) DMA2_Stream2_BASE)
#define DMA2_Stream3        ((DMA_Stream_TypeDef *) DMA2_Stream3_BASE)
#define DMA2_Stream4        ((DMA_Stream_TypeDef *) DMA2_Stream4_BASE)
#define DMA2_Stream5        ((DMA_Stream_TypeDef *) DMA2_Stream5_BASE)
#define DMA2_Stream6        ((DMA_Stream_TypeDef *) DMA2_Stream6_BASE)
#define DMA2_Stream7        ((DMA_Stream_TypeDef *) DMA2_Stream7_BASE)
#define ETH                 ((ETH_TypeDef *) ETH_BASE)
#define DMA2D               ((DMA2D_TypeDef *)DMA2D_BASE)
#define DCMI                ((DCMI_TypeDef *) DCMI_BASE)
#define RNG                 ((RNG_TypeDef *) RNG_BASE)
#define FMC_Bank1           ((FMC_Bank1_TypeDef *) FMC_Bank1_R_BASE)
#define FMC_Bank1E          ((FMC_Bank1E_TypeDef *) FMC_Bank1E_R_BASE)
#define FMC_Bank3           ((FMC_Bank3_TypeDef *) FMC_Bank3_R_BASE)
#define FMC_Bank5_6         ((FMC_Bank5_6_TypeDef *) FMC_Bank5_6_R_BASE)
#define QUADSPI             ((QUADSPI_TypeDef *) QSPI_R_BASE)
#define DBGMCU              ((DBGMCU_TypeDef *) DBGMCU_BASE)
#define USB_OTG_FS          ((USB_OTG_GlobalTypeDef *) USB_OTG_FS_PERIPH_BASE)
#define USB_OTG_HS          ((USB_OTG_GlobalTypeDef *) USB_OTG_HS_PERIPH_BASE)

/**
  * @}
  */

/** @addtogroup Exported_constants
  * @{
  */

  /** @addtogroup Peripheral_Registers_Bits_Definition
  * @{
  */

/******************************************************************************/
/*                         Peripheral Registers_Bits_Definition               */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*                        Analog to Digital Converter                         */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for ADC_SR register  ********************/
#define  ADC_SR_AWD                          0x00000001U        /*!<Analog watchdog flag                                 */
#define  ADC_SR_EOC                          0x00000002U        /*!<End of conversion                                    */
#define  ADC_SR_JEOC                         0x00000004U        /*!<Injected channel end of conversion                   */
#define  ADC_SR_JSTRT                        0x00000008U        /*!<Injected channel Start flag                          */
#define  ADC_SR_STRT                         0x00000010U        /*!<Regular channel Start flag                           */
#define  ADC_SR_OVR                          0x00000020U        /*!<Overrun flag                                         */

/*******************  Bit definition for ADC_CR1 register  ********************/
#define  ADC_CR1_AWDCH                       0x0000001FU        /*!<AWDCH[4:0] bits (Analog watchdog channel select bits) */
#define  ADC_CR1_AWDCH_0                     0x00000001U        /*!<Bit 0 */
#define  ADC_CR1_AWDCH_1                     0x00000002U        /*!<Bit 1 */
#define  ADC_CR1_AWDCH_2                     0x00000004U        /*!<Bit 2 */
#define  ADC_CR1_AWDCH_3                     0x00000008U        /*!<Bit 3 */
#define  ADC_CR1_AWDCH_4                     0x00000010U        /*!<Bit 4 */
#define  ADC_CR1_EOCIE                       0x00000020U        /*!<Interrupt enable for EOC                             */
#define  ADC_CR1_AWDIE                       0x00000040U        /*!<AAnalog Watchdog interrupt enable                    */
#define  ADC_CR1_JEOCIE                      0x00000080U        /*!<Interrupt enable for injected channels               */
#define  ADC_CR1_SCAN                        0x00000100U        /*!<Scan mode */
#define  ADC_CR1_AWDSGL                      0x00000200U        /*!<Enable the watchdog on a single channel in scan mode */
#define  ADC_CR1_JAUTO                       0x00000400U        /*!<Automatic injected group conversion                  */
#define  ADC_CR1_DISCEN                      0x00000800U        /*!<Discontinuous mode on regular channels               */
#define  ADC_CR1_JDISCEN                     0x00001000U        /*!<Discontinuous mode on injected channels              */
#define  ADC_CR1_DISCNUM                     0x0000E000U        /*!<DISCNUM[2:0] bits (Discontinuous mode channel count) */
#define  ADC_CR1_DISCNUM_0                   0x00002000U        /*!<Bit 0 */
#define  ADC_CR1_DISCNUM_1                   0x00004000U        /*!<Bit 1 */
#define  ADC_CR1_DISCNUM_2                   0x00008000U        /*!<Bit 2 */
#define  ADC_CR1_JAWDEN                      0x00400000U        /*!<Analog watchdog enable on injected channels          */
#define  ADC_CR1_AWDEN                       0x00800000U        /*!<Analog watchdog enable on regular channels           */
#define  ADC_CR1_RES                         0x03000000U        /*!<RES[2:0] bits (Resolution)                           */
#define  ADC_CR1_RES_0                       0x01000000U        /*!<Bit 0 */
#define  ADC_CR1_RES_1                       0x02000000U        /*!<Bit 1 */
#define  ADC_CR1_OVRIE                       0x04000000U         /*!<overrun interrupt enable */

/*******************  Bit definition for ADC_CR2 register  ********************/
#define  ADC_CR2_ADON                        0x00000001U        /*!<A/D Converter ON / OFF                                       */
#define  ADC_CR2_CONT                        0x00000002U        /*!<Continuous Conversion                                        */
#define  ADC_CR2_DMA                         0x00000100U        /*!<Direct Memory access mode                                    */
#define  ADC_CR2_DDS                         0x00000200U        /*!<DMA disable selection (Single ADC)                           */
#define  ADC_CR2_EOCS                        0x00000400U        /*!<End of conversion selection                                  */
#define  ADC_CR2_ALIGN                       0x00000800U        /*!<Data Alignment                                               */
#define  ADC_CR2_JEXTSEL                     0x000F0000U        /*!<JEXTSEL[3:0] bits (External event select for injected group) */
#define  ADC_CR2_JEXTSEL_0                   0x00010000U        /*!<Bit 0 */
#define  ADC_CR2_JEXTSEL_1                   0x00020000U        /*!<Bit 1 */
#define  ADC_CR2_JEXTSEL_2                   0x00040000U        /*!<Bit 2 */
#define  ADC_CR2_JEXTSEL_3                   0x00080000U        /*!<Bit 3 */
#define  ADC_CR2_JEXTEN                      0x00300000U        /*!<JEXTEN[1:0] bits (External Trigger Conversion mode for injected channelsp) */
#define  ADC_CR2_JEXTEN_0                    0x00100000U        /*!<Bit 0 */
#define  ADC_CR2_JEXTEN_1                    0x00200000U        /*!<Bit 1 */
#define  ADC_CR2_JSWSTART                    0x00400000U        /*!<Start Conversion of injected channels */
#define  ADC_CR2_EXTSEL                      0x0F000000U        /*!<EXTSEL[3:0] bits (External Event Select for regular group) */
#define  ADC_CR2_EXTSEL_0                    0x01000000U        /*!<Bit 0 */
#define  ADC_CR2_EXTSEL_1                    0x02000000U        /*!<Bit 1 */
#define  ADC_CR2_EXTSEL_2                    0x04000000U        /*!<Bit 2 */
#define  ADC_CR2_EXTSEL_3                    0x08000000U        /*!<Bit 3 */
#define  ADC_CR2_EXTEN                       0x30000000U        /*!<EXTEN[1:0] bits (External Trigger Conversion mode for regular channelsp) */
#define  ADC_CR2_EXTEN_0                     0x10000000U        /*!<Bit 0 */
#define  ADC_CR2_EXTEN_1                     0x20000000U        /*!<Bit 1 */
#define  ADC_CR2_SWSTART                     0x40000000U        /*!<Start Conversion of regular channels */

/******************  Bit definition for ADC_SMPR1 register  *******************/
#define  ADC_SMPR1_SMP10                     0x00000007U        /*!<SMP10[2:0] bits (Channel 10 Sample time selection) */
#define  ADC_SMPR1_SMP10_0                   0x00000001U        /*!<Bit 0 */
#define  ADC_SMPR1_SMP10_1                   0x00000002U        /*!<Bit 1 */
#define  ADC_SMPR1_SMP10_2                   0x00000004U        /*!<Bit 2 */
#define  ADC_SMPR1_SMP11                     0x00000038U        /*!<SMP11[2:0] bits (Channel 11 Sample time selection) */
#define  ADC_SMPR1_SMP11_0                   0x00000008U        /*!<Bit 0 */
#define  ADC_SMPR1_SMP11_1                   0x00000010U        /*!<Bit 1 */
#define  ADC_SMPR1_SMP11_2                   0x00000020U        /*!<Bit 2 */
#define  ADC_SMPR1_SMP12                     0x000001C0U        /*!<SMP12[2:0] bits (Channel 12 Sample time selection) */
#define  ADC_SMPR1_SMP12_0                   0x00000040U        /*!<Bit 0 */
#define  ADC_SMPR1_SMP12_1                   0x00000080U        /*!<Bit 1 */
#define  ADC_SMPR1_SMP12_2                   0x00000100U        /*!<Bit 2 */
#define  ADC_SMPR1_SMP13                     0x00000E00U        /*!<SMP13[2:0] bits (Channel 13 Sample time selection) */
#define  ADC_SMPR1_SMP13_0                   0x00000200U        /*!<Bit 0 */
#define  ADC_SMPR1_SMP13_1                   0x00000400U        /*!<Bit 1 */
#define  ADC_SMPR1_SMP13_2                   0x00000800U        /*!<Bit 2 */
#define  ADC_SMPR1_SMP14                     0x00007000U        /*!<SMP14[2:0] bits (Channel 14 Sample time selection) */
#define  ADC_SMPR1_SMP14_0                   0x00001000U        /*!<Bit 0 */
#define  ADC_SMPR1_SMP14_1                   0x00002000U        /*!<Bit 1 */
#define  ADC_SMPR1_SMP14_2                   0x00004000U        /*!<Bit 2 */
#define  ADC_SMPR1_SMP15                     0x00038000U        /*!<SMP15[2:0] bits (Channel 15 Sample time selection) */
#define  ADC_SMPR1_SMP15_0                   0x00008000U        /*!<Bit 0 */
#define  ADC_SMPR1_SMP15_1                   0x00010000U        /*!<Bit 1 */
#define  ADC_SMPR1_SMP15_2                   0x00020000U        /*!<Bit 2 */
#define  ADC_SMPR1_SMP16                     0x001C0000U        /*!<SMP16[2:0] bits (Channel 16 Sample time selection) */
#define  ADC_SMPR1_SMP16_0                   0x00040000U        /*!<Bit 0 */
#define  ADC_SMPR1_SMP16_1                   0x00080000U        /*!<Bit 1 */
#define  ADC_SMPR1_SMP16_2                   0x00100000U        /*!<Bit 2 */
#define  ADC_SMPR1_SMP17                     0x00E00000U        /*!<SMP17[2:0] bits (Channel 17 Sample time selection) */
#define  ADC_SMPR1_SMP17_0                   0x00200000U        /*!<Bit 0 */
#define  ADC_SMPR1_SMP17_1                   0x00400000U        /*!<Bit 1 */
#define  ADC_SMPR1_SMP17_2                   0x00800000U        /*!<Bit 2 */
#define  ADC_SMPR1_SMP18                     0x07000000U        /*!<SMP18[2:0] bits (Channel 18 Sample time selection) */
#define  ADC_SMPR1_SMP18_0                   0x01000000U        /*!<Bit 0 */
#define  ADC_SMPR1_SMP18_1                   0x02000000U        /*!<Bit 1 */
#define  ADC_SMPR1_SMP18_2                   0x04000000U        /*!<Bit 2 */

/******************  Bit definition for ADC_SMPR2 register  *******************/
#define  ADC_SMPR2_SMP0                      0x00000007U        /*!<SMP0[2:0] bits (Channel 0 Sample time selection) */
#define  ADC_SMPR2_SMP0_0                    0x00000001U        /*!<Bit 0 */
#define  ADC_SMPR2_SMP0_1                    0x00000002U        /*!<Bit 1 */
#define  ADC_SMPR2_SMP0_2                    0x00000004U        /*!<Bit 2 */
#define  ADC_SMPR2_SMP1                      0x00000038U        /*!<SMP1[2:0] bits (Channel 1 Sample time selection) */
#define  ADC_SMPR2_SMP1_0                    0x00000008U        /*!<Bit 0 */
#define  ADC_SMPR2_SMP1_1                    0x00000010U        /*!<Bit 1 */
#define  ADC_SMPR2_SMP1_2                    0x00000020U        /*!<Bit 2 */
#define  ADC_SMPR2_SMP2                      0x000001C0U        /*!<SMP2[2:0] bits (Channel 2 Sample time selection) */
#define  ADC_SMPR2_SMP2_0                    0x00000040U        /*!<Bit 0 */
#define  ADC_SMPR2_SMP2_1                    0x00000080U        /*!<Bit 1 */
#define  ADC_SMPR2_SMP2_2                    0x00000100U        /*!<Bit 2 */
#define  ADC_SMPR2_SMP3                      0x00000E00U        /*!<SMP3[2:0] bits (Channel 3 Sample time selection) */
#define  ADC_SMPR2_SMP3_0                    0x00000200U        /*!<Bit 0 */
#define  ADC_SMPR2_SMP3_1                    0x00000400U        /*!<Bit 1 */
#define  ADC_SMPR2_SMP3_2                    0x00000800U        /*!<Bit 2 */
#define  ADC_SMPR2_SMP4                      0x00007000U        /*!<SMP4[2:0] bits (Channel 4 Sample time selection) */
#define  ADC_SMPR2_SMP4_0                    0x00001000U        /*!<Bit 0 */
#define  ADC_SMPR2_SMP4_1                    0x00002000U        /*!<Bit 1 */
#define  ADC_SMPR2_SMP4_2                    0x00004000U        /*!<Bit 2 */
#define  ADC_SMPR2_SMP5                      0x00038000U        /*!<SMP5[2:0] bits (Channel 5 Sample time selection) */
#define  ADC_SMPR2_SMP5_0                    0x00008000U        /*!<Bit 0 */
#define  ADC_SMPR2_SMP5_1                    0x00010000U        /*!<Bit 1 */
#define  ADC_SMPR2_SMP5_2                    0x00020000U        /*!<Bit 2 */
#define  ADC_SMPR2_SMP6                      0x001C0000U        /*!<SMP6[2:0] bits (Channel 6 Sample time selection) */
#define  ADC_SMPR2_SMP6_0                    0x00040000U        /*!<Bit 0 */
#define  ADC_SMPR2_SMP6_1                    0x00080000U        /*!<Bit 1 */
#define  ADC_SMPR2_SMP6_2                    0x00100000U        /*!<Bit 2 */
#define  ADC_SMPR2_SMP7                      0x00E00000U        /*!<SMP7[2:0] bits (Channel 7 Sample time selection) */
#define  ADC_SMPR2_SMP7_0                    0x00200000U        /*!<Bit 0 */
#define  ADC_SMPR2_SMP7_1                    0x00400000U        /*!<Bit 1 */
#define  ADC_SMPR2_SMP7_2                    0x00800000U        /*!<Bit 2 */
#define  ADC_SMPR2_SMP8                      0x07000000U        /*!<SMP8[2:0] bits (Channel 8 Sample time selection) */
#define  ADC_SMPR2_SMP8_0                    0x01000000U        /*!<Bit 0 */
#define  ADC_SMPR2_SMP8_1                    0x02000000U        /*!<Bit 1 */
#define  ADC_SMPR2_SMP8_2                    0x04000000U        /*!<Bit 2 */
#define  ADC_SMPR2_SMP9                      0x38000000U        /*!<SMP9[2:0] bits (Channel 9 Sample time selection) */
#define  ADC_SMPR2_SMP9_0                    0x08000000U        /*!<Bit 0 */
#define  ADC_SMPR2_SMP9_1                    0x10000000U        /*!<Bit 1 */
#define  ADC_SMPR2_SMP9_2                    0x20000000U        /*!<Bit 2 */

/******************  Bit definition for ADC_JOFR1 register  *******************/
#define  ADC_JOFR1_JOFFSET1                  0x0FFFU            /*!<Data offset for injected channel 1 */

/******************  Bit definition for ADC_JOFR2 register  *******************/
#define  ADC_JOFR2_JOFFSET2                  0x0FFFU            /*!<Data offset for injected channel 2 */

/******************  Bit definition for ADC_JOFR3 register  *******************/
#define  ADC_JOFR3_JOFFSET3                  0x0FFFU            /*!<Data offset for injected channel 3 */

/******************  Bit definition for ADC_JOFR4 register  *******************/
#define  ADC_JOFR4_JOFFSET4                  0x0FFFU            /*!<Data offset for injected channel 4 */

/*******************  Bit definition for ADC_HTR register  ********************/
#define  ADC_HTR_HT                          0x0FFFU            /*!<Analog watchdog high threshold */

/*******************  Bit definition for ADC_LTR register  ********************/
#define  ADC_LTR_LT                          0x0FFFU            /*!<Analog watchdog low threshold */

/*******************  Bit definition for ADC_SQR1 register  *******************/
#define  ADC_SQR1_SQ13                       0x0000001FU        /*!<SQ13[4:0] bits (13th conversion in regular sequence) */
#define  ADC_SQR1_SQ13_0                     0x00000001U        /*!<Bit 0 */
#define  ADC_SQR1_SQ13_1                     0x00000002U        /*!<Bit 1 */
#define  ADC_SQR1_SQ13_2                     0x00000004U        /*!<Bit 2 */
#define  ADC_SQR1_SQ13_3                     0x00000008U        /*!<Bit 3 */
#define  ADC_SQR1_SQ13_4                     0x00000010U        /*!<Bit 4 */
#define  ADC_SQR1_SQ14                       0x000003E0U        /*!<SQ14[4:0] bits (14th conversion in regular sequence) */
#define  ADC_SQR1_SQ14_0                     0x00000020U        /*!<Bit 0 */
#define  ADC_SQR1_SQ14_1                     0x00000040U        /*!<Bit 1 */
#define  ADC_SQR1_SQ14_2                     0x00000080U        /*!<Bit 2 */
#define  ADC_SQR1_SQ14_3                     0x00000100U        /*!<Bit 3 */
#define  ADC_SQR1_SQ14_4                     0x00000200U        /*!<Bit 4 */
#define  ADC_SQR1_SQ15                       0x00007C00U        /*!<SQ15[4:0] bits (15th conversion in regular sequence) */
#define  ADC_SQR1_SQ15_0                     0x00000400U        /*!<Bit 0 */
#define  ADC_SQR1_SQ15_1                     0x00000800U        /*!<Bit 1 */
#define  ADC_SQR1_SQ15_2                     0x00001000U        /*!<Bit 2 */
#define  ADC_SQR1_SQ15_3                     0x00002000U        /*!<Bit 3 */
#define  ADC_SQR1_SQ15_4                     0x00004000U        /*!<Bit 4 */
#define  ADC_SQR1_SQ16                       0x000F8000U        /*!<SQ16[4:0] bits (16th conversion in regular sequence) */
#define  ADC_SQR1_SQ16_0                     0x00008000U        /*!<Bit 0 */
#define  ADC_SQR1_SQ16_1                     0x00010000U        /*!<Bit 1 */
#define  ADC_SQR1_SQ16_2                     0x00020000U        /*!<Bit 2 */
#define  ADC_SQR1_SQ16_3                     0x00040000U        /*!<Bit 3 */
#define  ADC_SQR1_SQ16_4                     0x00080000U        /*!<Bit 4 */
#define  ADC_SQR1_L                          0x00F00000U        /*!<L[3:0] bits (Regular channel sequence length) */
#define  ADC_SQR1_L_0                        0x00100000U        /*!<Bit 0 */
#define  ADC_SQR1_L_1                        0x00200000U        /*!<Bit 1 */
#define  ADC_SQR1_L_2                        0x00400000U        /*!<Bit 2 */
#define  ADC_SQR1_L_3                        0x00800000U        /*!<Bit 3 */

/*******************  Bit definition for ADC_SQR2 register  *******************/
#define  ADC_SQR2_SQ7                        0x0000001FU        /*!<SQ7[4:0] bits (7th conversion in regular sequence) */
#define  ADC_SQR2_SQ7_0                      0x00000001U        /*!<Bit 0 */
#define  ADC_SQR2_SQ7_1                      0x00000002U        /*!<Bit 1 */
#define  ADC_SQR2_SQ7_2                      0x00000004U        /*!<Bit 2 */
#define  ADC_SQR2_SQ7_3                      0x00000008U        /*!<Bit 3 */
#define  ADC_SQR2_SQ7_4                      0x00000010U        /*!<Bit 4 */
#define  ADC_SQR2_SQ8                        0x000003E0U        /*!<SQ8[4:0] bits (8th conversion in regular sequence) */
#define  ADC_SQR2_SQ8_0                      0x00000020U        /*!<Bit 0 */
#define  ADC_SQR2_SQ8_1                      0x00000040U        /*!<Bit 1 */
#define  ADC_SQR2_SQ8_2                      0x00000080U        /*!<Bit 2 */
#define  ADC_SQR2_SQ8_3                      0x00000100U        /*!<Bit 3 */
#define  ADC_SQR2_SQ8_4                      0x00000200U        /*!<Bit 4 */
#define  ADC_SQR2_SQ9                        0x00007C00U        /*!<SQ9[4:0] bits (9th conversion in regular sequence) */
#define  ADC_SQR2_SQ9_0                      0x00000400U        /*!<Bit 0 */
#define  ADC_SQR2_SQ9_1                      0x00000800U        /*!<Bit 1 */
#define  ADC_SQR2_SQ9_2                      0x00001000U        /*!<Bit 2 */
#define  ADC_SQR2_SQ9_3                      0x00002000U        /*!<Bit 3 */
#define  ADC_SQR2_SQ9_4                      0x00004000U        /*!<Bit 4 */
#define  ADC_SQR2_SQ10                       0x000F8000U        /*!<SQ10[4:0] bits (10th conversion in regular sequence) */
#define  ADC_SQR2_SQ10_0                     0x00008000U        /*!<Bit 0 */
#define  ADC_SQR2_SQ10_1                     0x00010000U        /*!<Bit 1 */
#define  ADC_SQR2_SQ10_2                     0x00020000U        /*!<Bit 2 */
#define  ADC_SQR2_SQ10_3                     0x00040000U        /*!<Bit 3 */
#define  ADC_SQR2_SQ10_4                     0x00080000U        /*!<Bit 4 */
#define  ADC_SQR2_SQ11                       0x01F00000U        /*!<SQ11[4:0] bits (11th conversion in regular sequence) */
#define  ADC_SQR2_SQ11_0                     0x00100000U        /*!<Bit 0 */
#define  ADC_SQR2_SQ11_1                     0x00200000U        /*!<Bit 1 */
#define  ADC_SQR2_SQ11_2                     0x00400000U        /*!<Bit 2 */
#define  ADC_SQR2_SQ11_3                     0x00800000U        /*!<Bit 3 */
#define  ADC_SQR2_SQ11_4                     0x01000000U        /*!<Bit 4 */
#define  ADC_SQR2_SQ12                       0x3E000000U        /*!<SQ12[4:0] bits (12th conversion in regular sequence) */
#define  ADC_SQR2_SQ12_0                     0x02000000U        /*!<Bit 0 */
#define  ADC_SQR2_SQ12_1                     0x04000000U        /*!<Bit 1 */
#define  ADC_SQR2_SQ12_2                     0x08000000U        /*!<Bit 2 */
#define  ADC_SQR2_SQ12_3                     0x10000000U        /*!<Bit 3 */
#define  ADC_SQR2_SQ12_4                     0x20000000U        /*!<Bit 4 */

/*******************  Bit definition for ADC_SQR3 register  *******************/
#define  ADC_SQR3_SQ1                        0x0000001FU        /*!<SQ1[4:0] bits (1st conversion in regular sequence) */
#define  ADC_SQR3_SQ1_0                      0x00000001U        /*!<Bit 0 */
#define  ADC_SQR3_SQ1_1                      0x00000002U        /*!<Bit 1 */
#define  ADC_SQR3_SQ1_2                      0x00000004U        /*!<Bit 2 */
#define  ADC_SQR3_SQ1_3                      0x00000008U        /*!<Bit 3 */
#define  ADC_SQR3_SQ1_4                      0x00000010U        /*!<Bit 4 */
#define  ADC_SQR3_SQ2                        0x000003E0U        /*!<SQ2[4:0] bits (2nd conversion in regular sequence) */
#define  ADC_SQR3_SQ2_0                      0x00000020U        /*!<Bit 0 */
#define  ADC_SQR3_SQ2_1                      0x00000040U        /*!<Bit 1 */
#define  ADC_SQR3_SQ2_2                      0x00000080U        /*!<Bit 2 */
#define  ADC_SQR3_SQ2_3                      0x00000100U        /*!<Bit 3 */
#define  ADC_SQR3_SQ2_4                      0x00000200U        /*!<Bit 4 */
#define  ADC_SQR3_SQ3                        0x00007C00U        /*!<SQ3[4:0] bits (3rd conversion in regular sequence) */
#define  ADC_SQR3_SQ3_0                      0x00000400U        /*!<Bit 0 */
#define  ADC_SQR3_SQ3_1                      0x00000800U        /*!<Bit 1 */
#define  ADC_SQR3_SQ3_2                      0x00001000U        /*!<Bit 2 */
#define  ADC_SQR3_SQ3_3                      0x00002000U        /*!<Bit 3 */
#define  ADC_SQR3_SQ3_4                      0x00004000U        /*!<Bit 4 */
#define  ADC_SQR3_SQ4                        0x000F8000U        /*!<SQ4[4:0] bits (4th conversion in regular sequence) */
#define  ADC_SQR3_SQ4_0                      0x00008000U        /*!<Bit 0 */
#define  ADC_SQR3_SQ4_1                      0x00010000U        /*!<Bit 1 */
#define  ADC_SQR3_SQ4_2                      0x00020000U        /*!<Bit 2 */
#define  ADC_SQR3_SQ4_3                      0x00040000U        /*!<Bit 3 */
#define  ADC_SQR3_SQ4_4                      0x00080000U        /*!<Bit 4 */
#define  ADC_SQR3_SQ5                        0x01F00000U        /*!<SQ5[4:0] bits (5th conversion in regular sequence) */
#define  ADC_SQR3_SQ5_0                      0x00100000U        /*!<Bit 0 */
#define  ADC_SQR3_SQ5_1                      0x00200000U        /*!<Bit 1 */
#define  ADC_SQR3_SQ5_2                      0x00400000U        /*!<Bit 2 */
#define  ADC_SQR3_SQ5_3                      0x00800000U        /*!<Bit 3 */
#define  ADC_SQR3_SQ5_4                      0x01000000U        /*!<Bit 4 */
#define  ADC_SQR3_SQ6                        0x3E000000U        /*!<SQ6[4:0] bits (6th conversion in regular sequence) */
#define  ADC_SQR3_SQ6_0                      0x02000000U        /*!<Bit 0 */
#define  ADC_SQR3_SQ6_1                      0x04000000U        /*!<Bit 1 */
#define  ADC_SQR3_SQ6_2                      0x08000000U        /*!<Bit 2 */
#define  ADC_SQR3_SQ6_3                      0x10000000U        /*!<Bit 3 */
#define  ADC_SQR3_SQ6_4                      0x20000000U        /*!<Bit 4 */

/*******************  Bit definition for ADC_JSQR register  *******************/
#define  ADC_JSQR_JSQ1                       0x0000001FU        /*!<JSQ1[4:0] bits (1st conversion in injected sequence) */
#define  ADC_JSQR_JSQ1_0                     0x00000001U        /*!<Bit 0 */
#define  ADC_JSQR_JSQ1_1                     0x00000002U        /*!<Bit 1 */
#define  ADC_JSQR_JSQ1_2                     0x00000004U        /*!<Bit 2 */
#define  ADC_JSQR_JSQ1_3                     0x00000008U        /*!<Bit 3 */
#define  ADC_JSQR_JSQ1_4                     0x00000010U        /*!<Bit 4 */
#define  ADC_JSQR_JSQ2                       0x000003E0U        /*!<JSQ2[4:0] bits (2nd conversion in injected sequence) */
#define  ADC_JSQR_JSQ2_0                     0x00000020U        /*!<Bit 0 */
#define  ADC_JSQR_JSQ2_1                     0x00000040U        /*!<Bit 1 */
#define  ADC_JSQR_JSQ2_2                     0x00000080U        /*!<Bit 2 */
#define  ADC_JSQR_JSQ2_3                     0x00000100U        /*!<Bit 3 */
#define  ADC_JSQR_JSQ2_4                     0x00000200U        /*!<Bit 4 */
#define  ADC_JSQR_JSQ3                       0x00007C00U        /*!<JSQ3[4:0] bits (3rd conversion in injected sequence) */
#define  ADC_JSQR_JSQ3_0                     0x00000400U        /*!<Bit 0 */
#define  ADC_JSQR_JSQ3_1                     0x00000800U        /*!<Bit 1 */
#define  ADC_JSQR_JSQ3_2                     0x00001000U        /*!<Bit 2 */
#define  ADC_JSQR_JSQ3_3                     0x00002000U        /*!<Bit 3 */
#define  ADC_JSQR_JSQ3_4                     0x00004000U        /*!<Bit 4 */
#define  ADC_JSQR_JSQ4                       0x000F8000U        /*!<JSQ4[4:0] bits (4th conversion in injected sequence) */
#define  ADC_JSQR_JSQ4_0                     0x00008000U        /*!<Bit 0 */
#define  ADC_JSQR_JSQ4_1                     0x00010000U        /*!<Bit 1 */
#define  ADC_JSQR_JSQ4_2                     0x00020000U        /*!<Bit 2 */
#define  ADC_JSQR_JSQ4_3                     0x00040000U        /*!<Bit 3 */
#define  ADC_JSQR_JSQ4_4                     0x00080000U        /*!<Bit 4 */
#define  ADC_JSQR_JL                         0x00300000U        /*!<JL[1:0] bits (Injected Sequence length) */
#define  ADC_JSQR_JL_0                       0x00100000U        /*!<Bit 0 */
#define  ADC_JSQR_JL_1                       0x00200000U        /*!<Bit 1 */

/*******************  Bit definition for ADC_JDR1 register  *******************/
#define  ADC_JDR1_JDATA                      ((uint16_t)0xFFFFU)            /*!<Injected data */

/*******************  Bit definition for ADC_JDR2 register  *******************/
#define  ADC_JDR2_JDATA                      ((uint16_t)0xFFFFU)            /*!<Injected data */

/*******************  Bit definition for ADC_JDR3 register  *******************/
#define  ADC_JDR3_JDATA                      ((uint16_t)0xFFFFU)            /*!<Injected data */

/*******************  Bit definition for ADC_JDR4 register  *******************/
#define  ADC_JDR4_JDATA                      ((uint16_t)0xFFFFU)            /*!<Injected data */

/********************  Bit definition for ADC_DR register  ********************/
#define  ADC_DR_DATA                         0x0000FFFFU        /*!<Regular data */
#define  ADC_DR_ADC2DATA                     0xFFFF0000U        /*!<ADC2 data */

/*******************  Bit definition for ADC_CSR register  ********************/
#define  ADC_CSR_AWD1                        0x00000001U        /*!<ADC1 Analog watchdog flag               */
#define  ADC_CSR_EOC1                        0x00000002U        /*!<ADC1 End of conversion                  */
#define  ADC_CSR_JEOC1                       0x00000004U        /*!<ADC1 Injected channel end of conversion */
#define  ADC_CSR_JSTRT1                      0x00000008U        /*!<ADC1 Injected channel Start flag        */
#define  ADC_CSR_STRT1                       0x00000010U        /*!<ADC1 Regular channel Start flag         */
#define  ADC_CSR_OVR1                        0x00000020U        /*!<ADC1 Overrun flag                       */
#define  ADC_CSR_AWD2                        0x00000100U        /*!<ADC2 Analog watchdog flag               */
#define  ADC_CSR_EOC2                        0x00000200U        /*!<ADC2 End of conversion                  */
#define  ADC_CSR_JEOC2                       0x00000400U        /*!<ADC2 Injected channel end of conversion */
#define  ADC_CSR_JSTRT2                      0x00000800U        /*!<ADC2 Injected channel Start flag        */
#define  ADC_CSR_STRT2                       0x00001000U        /*!<ADC2 Regular channel Start flag         */
#define  ADC_CSR_OVR2                        0x00002000U        /*!<ADC2 Overrun flag                       */
#define  ADC_CSR_AWD3                        0x00010000U        /*!<ADC3 Analog watchdog flag               */
#define  ADC_CSR_EOC3                        0x00020000U        /*!<ADC3 End of conversion                  */
#define  ADC_CSR_JEOC3                       0x00040000U        /*!<ADC3 Injected channel end of conversion */
#define  ADC_CSR_JSTRT3                      0x00080000U        /*!<ADC3 Injected channel Start flag        */
#define  ADC_CSR_STRT3                       0x00100000U        /*!<ADC3 Regular channel Start flag         */
#define  ADC_CSR_OVR3                        0x00200000U        /*!<ADC3 Overrun flag                       */

/* Legacy defines */
#define  ADC_CSR_DOVR1                       ADC_CSR_OVR1
#define  ADC_CSR_DOVR2                       ADC_CSR_OVR2
#define  ADC_CSR_DOVR3                       ADC_CSR_OVR3


/*******************  Bit definition for ADC_CCR register  ********************/
#define  ADC_CCR_MULTI                       0x0000001FU        /*!<MULTI[4:0] bits (Multi-ADC mode selection) */
#define  ADC_CCR_MULTI_0                     0x00000001U        /*!<Bit 0 */
#define  ADC_CCR_MULTI_1                     0x00000002U        /*!<Bit 1 */
#define  ADC_CCR_MULTI_2                     0x00000004U        /*!<Bit 2 */
#define  ADC_CCR_MULTI_3                     0x00000008U        /*!<Bit 3 */
#define  ADC_CCR_MULTI_4                     0x00000010U        /*!<Bit 4 */
#define  ADC_CCR_DELAY                       0x00000F00U        /*!<DELAY[3:0] bits (Delay between 2 sampling phases) */
#define  ADC_CCR_DELAY_0                     0x00000100U        /*!<Bit 0 */
#define  ADC_CCR_DELAY_1                     0x00000200U        /*!<Bit 1 */
#define  ADC_CCR_DELAY_2                     0x00000400U        /*!<Bit 2 */
#define  ADC_CCR_DELAY_3                     0x00000800U        /*!<Bit 3 */
#define  ADC_CCR_DDS                         0x00002000U        /*!<DMA disable selection (Multi-ADC mode) */
#define  ADC_CCR_DMA                         0x0000C000U        /*!<DMA[1:0] bits (Direct Memory Access mode for multimode) */
#define  ADC_CCR_DMA_0                       0x00004000U        /*!<Bit 0 */
#define  ADC_CCR_DMA_1                       0x00008000U        /*!<Bit 1 */
#define  ADC_CCR_ADCPRE                      0x00030000U        /*!<ADCPRE[1:0] bits (ADC prescaler) */
#define  ADC_CCR_ADCPRE_0                    0x00010000U        /*!<Bit 0 */
#define  ADC_CCR_ADCPRE_1                    0x00020000U        /*!<Bit 1 */
#define  ADC_CCR_VBATE                       0x00400000U        /*!<VBAT Enable */
#define  ADC_CCR_TSVREFE                     0x00800000U        /*!<Temperature Sensor and VREFINT Enable */

/*******************  Bit definition for ADC_CDR register  ********************/
#define  ADC_CDR_DATA1                       0x0000FFFFU         /*!<1st data of a pair of regular conversions */
#define  ADC_CDR_DATA2                       0xFFFF0000U         /*!<2nd data of a pair of regular conversions */

/******************************************************************************/
/*                                                                            */
/*                         Controller Area Network                            */
/*                                                                            */
/******************************************************************************/
/*!<CAN control and status registers */
/*******************  Bit definition for CAN_MCR register  ********************/
#define  CAN_MCR_INRQ                        0x00000001U        /*!<Initialization Request            */
#define  CAN_MCR_SLEEP                       0x00000002U        /*!<Sleep Mode Request                */
#define  CAN_MCR_TXFP                        0x00000004U        /*!<Transmit FIFO Priority            */
#define  CAN_MCR_RFLM                        0x00000008U        /*!<Receive FIFO Locked Mode          */
#define  CAN_MCR_NART                        0x00000010U        /*!<No Automatic Retransmission       */
#define  CAN_MCR_AWUM                        0x00000020U        /*!<Automatic Wakeup Mode             */
#define  CAN_MCR_ABOM                        0x00000040U        /*!<Automatic Bus-Off Management      */
#define  CAN_MCR_TTCM                        0x00000080U        /*!<Time Triggered Communication Mode */
#define  CAN_MCR_RESET                       0x00008000U        /*!<bxCAN software master reset       */

/*******************  Bit definition for CAN_MSR register  ********************/
#define  CAN_MSR_INAK                        0x00000001U        /*!<Initialization Acknowledge  */
#define  CAN_MSR_SLAK                        0x00000002U        /*!<Sleep Acknowledge           */
#define  CAN_MSR_ERRI                        0x00000004U        /*!<Error Interrupt             */
#define  CAN_MSR_WKUI                        0x00000008U        /*!<Wakeup Interrupt            */
#define  CAN_MSR_SLAKI                       0x00000010U        /*!<Sleep Acknowledge Interrupt */
#define  CAN_MSR_TXM                         0x00000100U        /*!<Transmit Mode               */
#define  CAN_MSR_RXM                         0x00000200U        /*!<Receive Mode                */
#define  CAN_MSR_SAMP                        0x00000400U        /*!<Last Sample Point           */
#define  CAN_MSR_RX                          0x00000800U        /*!<CAN Rx Signal               */

/*******************  Bit definition for CAN_TSR register  ********************/
#define  CAN_TSR_RQCP0                       0x00000001U        /*!<Request Completed Mailbox0      */
#define  CAN_TSR_TXOK0                       0x00000002U        /*!<Transmission OK of Mailbox0     */
#define  CAN_TSR_ALST0                       0x00000004U        /*!<Arbitration Lost for Mailbox0   */
#define  CAN_TSR_TERR0                       0x00000008U        /*!<Transmission Error of Mailbox0  */
#define  CAN_TSR_ABRQ0                       0x00000080U        /*!<Abort Request for Mailbox0      */
#define  CAN_TSR_RQCP1                       0x00000100U        /*!<Request Completed Mailbox1      */
#define  CAN_TSR_TXOK1                       0x00000200U        /*!<Transmission OK of Mailbox1     */
#define  CAN_TSR_ALST1                       0x00000400U        /*!<Arbitration Lost for Mailbox1   */
#define  CAN_TSR_TERR1                       0x00000800U        /*!<Transmission Error of Mailbox1  */
#define  CAN_TSR_ABRQ1                       0x00008000U        /*!<Abort Request for Mailbox 1     */
#define  CAN_TSR_RQCP2                       0x00010000U        /*!<Request Completed Mailbox2      */
#define  CAN_TSR_TXOK2                       0x00020000U        /*!<Transmission OK of Mailbox 2    */
#define  CAN_TSR_ALST2                       0x00040000U        /*!<Arbitration Lost for mailbox 2  */
#define  CAN_TSR_TERR2                       0x00080000U        /*!<Transmission Error of Mailbox 2 */
#define  CAN_TSR_ABRQ2                       0x00800000U        /*!<Abort Request for Mailbox 2     */
#define  CAN_TSR_CODE                        0x03000000U        /*!<Mailbox Code                    */

#define  CAN_TSR_TME                         0x1C000000U        /*!<TME[2:0] bits */
#define  CAN_TSR_TME0                        0x04000000U        /*!<Transmit Mailbox 0 Empty */
#define  CAN_TSR_TME1                        0x08000000U        /*!<Transmit Mailbox 1 Empty */
#define  CAN_TSR_TME2                        0x10000000U        /*!<Transmit Mailbox 2 Empty */

#define  CAN_TSR_LOW                         0xE0000000U        /*!<LOW[2:0] bits */
#define  CAN_TSR_LOW0                        0x20000000U        /*!<Lowest Priority Flag for Mailbox 0 */
#define  CAN_TSR_LOW1                        0x40000000U        /*!<Lowest Priority Flag for Mailbox 1 */
#define  CAN_TSR_LOW2                        0x80000000U        /*!<Lowest Priority Flag for Mailbox 2 */

/*******************  Bit definition for CAN_RF0R register  *******************/
#define  CAN_RF0R_FMP0                       0x00000003U        /*!<FIFO 0 Message Pending        */
#define  CAN_RF0R_FULL0                      0x00000008U        /*!<FIFO 0 Full                   */
#define  CAN_RF0R_FOVR0                      0x00000010U        /*!<FIFO 0 Overrun                */
#define  CAN_RF0R_RFOM0                      0x00000020U        /*!<Release FIFO 0 Output Mailbox */

/*******************  Bit definition for CAN_RF1R register  *******************/
#define  CAN_RF1R_FMP1                       0x00000003U        /*!<FIFO 1 Message Pending        */
#define  CAN_RF1R_FULL1                      0x00000008U        /*!<FIFO 1 Full                   */
#define  CAN_RF1R_FOVR1                      0x00000010U        /*!<FIFO 1 Overrun                */
#define  CAN_RF1R_RFOM1                      0x00000020U        /*!<Release FIFO 1 Output Mailbox */

/********************  Bit definition for CAN_IER register  *******************/
#define  CAN_IER_TMEIE                       0x00000001U        /*!<Transmit Mailbox Empty Interrupt Enable */
#define  CAN_IER_FMPIE0                      0x00000002U        /*!<FIFO Message Pending Interrupt Enable   */
#define  CAN_IER_FFIE0                       0x00000004U        /*!<FIFO Full Interrupt Enable              */
#define  CAN_IER_FOVIE0                      0x00000008U        /*!<FIFO Overrun Interrupt Enable           */
#define  CAN_IER_FMPIE1                      0x00000010U        /*!<FIFO Message Pending Interrupt Enable   */
#define  CAN_IER_FFIE1                       0x00000020U        /*!<FIFO Full Interrupt Enable              */
#define  CAN_IER_FOVIE1                      0x00000040U        /*!<FIFO Overrun Interrupt Enable           */
#define  CAN_IER_EWGIE                       0x00000100U        /*!<Error Warning Interrupt Enable          */
#define  CAN_IER_EPVIE                       0x00000200U        /*!<Error Passive Interrupt Enable          */
#define  CAN_IER_BOFIE                       0x00000400U        /*!<Bus-Off Interrupt Enable                */
#define  CAN_IER_LECIE                       0x00000800U        /*!<Last Error Code Interrupt Enable        */
#define  CAN_IER_ERRIE                       0x00008000U        /*!<Error Interrupt Enable                  */
#define  CAN_IER_WKUIE                       0x00010000U        /*!<Wakeup Interrupt Enable                 */
#define  CAN_IER_SLKIE                       0x00020000U        /*!<Sleep Interrupt Enable                  */

/********************  Bit definition for CAN_ESR register  *******************/
#define  CAN_ESR_EWGF                        0x00000001U        /*!<Error Warning Flag */
#define  CAN_ESR_EPVF                        0x00000002U        /*!<Error Passive Flag */
#define  CAN_ESR_BOFF                        0x00000004U        /*!<Bus-Off Flag */

#define  CAN_ESR_LEC                         0x00000070U        /*!<LEC[2:0] bits (Last Error Code) */
#define  CAN_ESR_LEC_0                       0x00000010U        /*!<Bit 0 */
#define  CAN_ESR_LEC_1                       0x00000020U        /*!<Bit 1 */
#define  CAN_ESR_LEC_2                       0x00000040U        /*!<Bit 2 */

#define  CAN_ESR_TEC                         0x00FF0000U        /*!<Least significant byte of the 9-bit Transmit Error Counter */
#define  CAN_ESR_REC                         0xFF000000U        /*!<Receive Error Counter */

/*******************  Bit definition for CAN_BTR register  ********************/
#define  CAN_BTR_BRP                         0x000003FFU        /*!<Baud Rate Prescaler           */
#define  CAN_BTR_TS1                         0x000F0000U        /*!<Time Segment 1                */
#define  CAN_BTR_TS1_0                       0x00010000U        /*!<Bit 0 */
#define  CAN_BTR_TS1_1                       0x00020000U        /*!<Bit 1 */
#define  CAN_BTR_TS1_2                       0x00040000U        /*!<Bit 2 */
#define  CAN_BTR_TS1_3                       0x00080000U        /*!<Bit 3 */
#define  CAN_BTR_TS2                         0x00700000U        /*!<Time Segment 2                */
#define  CAN_BTR_TS2_0                       0x00100000U        /*!<Bit 0 */
#define  CAN_BTR_TS2_1                       0x00200000U        /*!<Bit 1 */
#define  CAN_BTR_TS2_2                       0x00400000U        /*!<Bit 2 */
#define  CAN_BTR_SJW                         0x03000000U        /*!<Resynchronization Jump Width  */
#define  CAN_BTR_SJW_0                       0x01000000U        /*!<Bit 0 */
#define  CAN_BTR_SJW_1                       0x02000000U        /*!<Bit 1 */
#define  CAN_BTR_LBKM                        0x40000000U        /*!<Loop Back Mode (Debug)        */
#define  CAN_BTR_SILM                        0x80000000U        /*!<Silent Mode                   */

/*!<Mailbox registers */
/******************  Bit definition for CAN_TI0R register  ********************/
#define  CAN_TI0R_TXRQ                       0x00000001U        /*!<Transmit Mailbox Request                   */
#define  CAN_TI0R_RTR                        0x00000002U        /*!<Remote Transmission Request                */
#define  CAN_TI0R_IDE                        0x00000004U        /*!<Identifier Extension                       */
#define  CAN_TI0R_EXID                       0x001FFFF8U        /*!<Extended Identifier                        */
#define  CAN_TI0R_STID                       0xFFE00000U        /*!<Standard Identifier or Extended Identifier */

/******************  Bit definition for CAN_TDT0R register  *******************/
#define  CAN_TDT0R_DLC                       0x0000000FU        /*!<Data Length Code     */
#define  CAN_TDT0R_TGT                       0x00000100U        /*!<Transmit Global Time */
#define  CAN_TDT0R_TIME                      0xFFFF0000U        /*!<Message Time Stamp   */

/******************  Bit definition for CAN_TDL0R register  *******************/
#define  CAN_TDL0R_DATA0                     0x000000FFU        /*!<Data byte 0 */
#define  CAN_TDL0R_DATA1                     0x0000FF00U        /*!<Data byte 1 */
#define  CAN_TDL0R_DATA2                     0x00FF0000U        /*!<Data byte 2 */
#define  CAN_TDL0R_DATA3                     0xFF000000U        /*!<Data byte 3 */

/******************  Bit definition for CAN_TDH0R register  *******************/
#define  CAN_TDH0R_DATA4                     0x000000FFU        /*!<Data byte 4 */
#define  CAN_TDH0R_DATA5                     0x0000FF00U        /*!<Data byte 5 */
#define  CAN_TDH0R_DATA6                     0x00FF0000U        /*!<Data byte 6 */
#define  CAN_TDH0R_DATA7                     0xFF000000U        /*!<Data byte 7 */

/*******************  Bit definition for CAN_TI1R register  *******************/
#define  CAN_TI1R_TXRQ                       0x00000001U        /*!<Transmit Mailbox Request                   */
#define  CAN_TI1R_RTR                        0x00000002U        /*!<Remote Transmission Request                */
#define  CAN_TI1R_IDE                        0x00000004U        /*!<Identifier Extension                       */
#define  CAN_TI1R_EXID                       0x001FFFF8U        /*!<Extended Identifier                        */
#define  CAN_TI1R_STID                       0xFFE00000U        /*!<Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_TDT1R register  ******************/
#define  CAN_TDT1R_DLC                       0x0000000FU        /*!<Data Length Code     */
#define  CAN_TDT1R_TGT                       0x00000100U        /*!<Transmit Global Time */
#define  CAN_TDT1R_TIME                      0xFFFF0000U        /*!<Message Time Stamp   */

/*******************  Bit definition for CAN_TDL1R register  ******************/
#define  CAN_TDL1R_DATA0                     0x000000FFU        /*!<Data byte 0 */
#define  CAN_TDL1R_DATA1                     0x0000FF00U        /*!<Data byte 1 */
#define  CAN_TDL1R_DATA2                     0x00FF0000U        /*!<Data byte 2 */
#define  CAN_TDL1R_DATA3                     0xFF000000U        /*!<Data byte 3 */

/*******************  Bit definition for CAN_TDH1R register  ******************/
#define  CAN_TDH1R_DATA4                     0x000000FFU        /*!<Data byte 4 */
#define  CAN_TDH1R_DATA5                     0x0000FF00U        /*!<Data byte 5 */
#define  CAN_TDH1R_DATA6                     0x00FF0000U        /*!<Data byte 6 */
#define  CAN_TDH1R_DATA7                     0xFF000000U        /*!<Data byte 7 */

/*******************  Bit definition for CAN_TI2R register  *******************/
#define  CAN_TI2R_TXRQ                       0x00000001U        /*!<Transmit Mailbox Request                   */
#define  CAN_TI2R_RTR                        0x00000002U        /*!<Remote Transmission Request                */
#define  CAN_TI2R_IDE                        0x00000004U        /*!<Identifier Extension                       */
#define  CAN_TI2R_EXID                       0x001FFFF8U        /*!<Extended identifier                        */
#define  CAN_TI2R_STID                       0xFFE00000U        /*!<Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_TDT2R register  ******************/
#define  CAN_TDT2R_DLC                       0x0000000FU        /*!<Data Length Code      */
#define  CAN_TDT2R_TGT                       0x00000100U        /*!<Transmit Global Time  */
#define  CAN_TDT2R_TIME                      0xFFFF0000U        /*!<Message Time Stamp    */

/*******************  Bit definition for CAN_TDL2R register  ******************/
#define  CAN_TDL2R_DATA0                     0x000000FFU        /*!<Data byte 0 */
#define  CAN_TDL2R_DATA1                     0x0000FF00U        /*!<Data byte 1 */
#define  CAN_TDL2R_DATA2                     0x00FF0000U        /*!<Data byte 2 */
#define  CAN_TDL2R_DATA3                     0xFF000000U        /*!<Data byte 3 */

/*******************  Bit definition for CAN_TDH2R register  ******************/
#define  CAN_TDH2R_DATA4                     0x000000FFU        /*!<Data byte 4 */
#define  CAN_TDH2R_DATA5                     0x0000FF00U        /*!<Data byte 5 */
#define  CAN_TDH2R_DATA6                     0x00FF0000U        /*!<Data byte 6 */
#define  CAN_TDH2R_DATA7                     0xFF000000U        /*!<Data byte 7 */

/*******************  Bit definition for CAN_RI0R register  *******************/
#define  CAN_RI0R_RTR                        0x00000002U        /*!<Remote Transmission Request                */
#define  CAN_RI0R_IDE                        0x00000004U        /*!<Identifier Extension                       */
#define  CAN_RI0R_EXID                       0x001FFFF8U        /*!<Extended Identifier                        */
#define  CAN_RI0R_STID                       0xFFE00000U        /*!<Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_RDT0R register  ******************/
#define  CAN_RDT0R_DLC                       0x0000000FU        /*!<Data Length Code */
#define  CAN_RDT0R_FMI                       0x0000FF00U        /*!<Filter Match Index */
#define  CAN_RDT0R_TIME                      0xFFFF0000U        /*!<Message Time Stamp */

/*******************  Bit definition for CAN_RDL0R register  ******************/
#define  CAN_RDL0R_DATA0                     0x000000FFU        /*!<Data byte 0 */
#define  CAN_RDL0R_DATA1                     0x0000FF00U        /*!<Data byte 1 */
#define  CAN_RDL0R_DATA2                     0x00FF0000U        /*!<Data byte 2 */
#define  CAN_RDL0R_DATA3                     0xFF000000U        /*!<Data byte 3 */

/*******************  Bit definition for CAN_RDH0R register  ******************/
#define  CAN_RDH0R_DATA4                     0x000000FFU        /*!<Data byte 4 */
#define  CAN_RDH0R_DATA5                     0x0000FF00U        /*!<Data byte 5 */
#define  CAN_RDH0R_DATA6                     0x00FF0000U        /*!<Data byte 6 */
#define  CAN_RDH0R_DATA7                     0xFF000000U        /*!<Data byte 7 */

/*******************  Bit definition for CAN_RI1R register  *******************/
#define  CAN_RI1R_RTR                        0x00000002U        /*!<Remote Transmission Request                */
#define  CAN_RI1R_IDE                        0x00000004U        /*!<Identifier Extension                       */
#define  CAN_RI1R_EXID                       0x001FFFF8U        /*!<Extended identifier                        */
#define  CAN_RI1R_STID                       0xFFE00000U        /*!<Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_RDT1R register  ******************/
#define  CAN_RDT1R_DLC                       0x0000000FU        /*!<Data Length Code   */
#define  CAN_RDT1R_FMI                       0x0000FF00U        /*!<Filter Match Index */
#define  CAN_RDT1R_TIME                      0xFFFF0000U        /*!<Message Time Stamp */

/*******************  Bit definition for CAN_RDL1R register  ******************/
#define  CAN_RDL1R_DATA0                     0x000000FFU        /*!<Data byte 0 */
#define  CAN_RDL1R_DATA1                     0x0000FF00U        /*!<Data byte 1 */
#define  CAN_RDL1R_DATA2                     0x00FF0000U        /*!<Data byte 2 */
#define  CAN_RDL1R_DATA3                     0xFF000000U        /*!<Data byte 3 */

/*******************  Bit definition for CAN_RDH1R register  ******************/
#define  CAN_RDH1R_DATA4                     0x000000FFU        /*!<Data byte 4 */
#define  CAN_RDH1R_DATA5                     0x0000FF00U        /*!<Data byte 5 */
#define  CAN_RDH1R_DATA6                     0x00FF0000U        /*!<Data byte 6 */
#define  CAN_RDH1R_DATA7                     0xFF000000U        /*!<Data byte 7 */

/*!<CAN filter registers */
/*******************  Bit definition for CAN_FMR register  ********************/
#define  CAN_FMR_FINIT                       ((uint8_t)0x01U)               /*!<Filter Init Mode */
#define  CAN_FMR_CAN2SB                      0x00003F00U        /*!<CAN2 start bank */

/*******************  Bit definition for CAN_FM1R register  *******************/
#define  CAN_FM1R_FBM                        0x3FFFU            /*!<Filter Mode */
#define  CAN_FM1R_FBM0                       0x0001U            /*!<Filter Init Mode bit 0  */
#define  CAN_FM1R_FBM1                       0x0002U            /*!<Filter Init Mode bit 1  */
#define  CAN_FM1R_FBM2                       0x0004U            /*!<Filter Init Mode bit 2  */
#define  CAN_FM1R_FBM3                       0x0008U            /*!<Filter Init Mode bit 3  */
#define  CAN_FM1R_FBM4                       0x0010U            /*!<Filter Init Mode bit 4  */
#define  CAN_FM1R_FBM5                       0x0020U            /*!<Filter Init Mode bit 5  */
#define  CAN_FM1R_FBM6                       0x0040U            /*!<Filter Init Mode bit 6  */
#define  CAN_FM1R_FBM7                       0x0080U            /*!<Filter Init Mode bit 7  */
#define  CAN_FM1R_FBM8                       0x0100U            /*!<Filter Init Mode bit 8  */
#define  CAN_FM1R_FBM9                       0x0200U            /*!<Filter Init Mode bit 9  */
#define  CAN_FM1R_FBM10                      0x0400U            /*!<Filter Init Mode bit 10 */
#define  CAN_FM1R_FBM11                      0x0800U            /*!<Filter Init Mode bit 11 */
#define  CAN_FM1R_FBM12                      0x1000U            /*!<Filter Init Mode bit 12 */
#define  CAN_FM1R_FBM13                      0x2000U            /*!<Filter Init Mode bit 13 */

/*******************  Bit definition for CAN_FS1R register  *******************/
#define  CAN_FS1R_FSC                        0x00003FFFU        /*!<Filter Scale Configuration        */
#define  CAN_FS1R_FSC0                       0x00000001U        /*!<Filter Scale Configuration bit 0  */
#define  CAN_FS1R_FSC1                       0x00000002U        /*!<Filter Scale Configuration bit 1  */
#define  CAN_FS1R_FSC2                       0x00000004U        /*!<Filter Scale Configuration bit 2  */
#define  CAN_FS1R_FSC3                       0x00000008U        /*!<Filter Scale Configuration bit 3  */
#define  CAN_FS1R_FSC4                       0x00000010U        /*!<Filter Scale Configuration bit 4  */
#define  CAN_FS1R_FSC5                       0x00000020U        /*!<Filter Scale Configuration bit 5  */
#define  CAN_FS1R_FSC6                       0x00000040U        /*!<Filter Scale Configuration bit 6  */
#define  CAN_FS1R_FSC7                       0x00000080U        /*!<Filter Scale Configuration bit 7  */
#define  CAN_FS1R_FSC8                       0x00000100U        /*!<Filter Scale Configuration bit 8  */
#define  CAN_FS1R_FSC9                       0x00000200U        /*!<Filter Scale Configuration bit 9  */
#define  CAN_FS1R_FSC10                      0x00000400U        /*!<Filter Scale Configuration bit 10 */
#define  CAN_FS1R_FSC11                      0x00000800U        /*!<Filter Scale Configuration bit 11 */
#define  CAN_FS1R_FSC12                      0x00001000U        /*!<Filter Scale Configuration bit 12 */
#define  CAN_FS1R_FSC13                      0x00002000U        /*!<Filter Scale Configuration bit 13 */

/******************  Bit definition for CAN_FFA1R register  *******************/
#define  CAN_FFA1R_FFA                       0x00003FFFU        /*!<Filter FIFO Assignment */
#define  CAN_FFA1R_FFA0                      0x00000001U        /*!<Filter FIFO Assignment for Filter 0 */
#define  CAN_FFA1R_FFA1                      0x00000002U        /*!<Filter FIFO Assignment for Filter 1 */
#define  CAN_FFA1R_FFA2                      0x00000004U        /*!<Filter FIFO Assignment for Filter 2 */
#define  CAN_FFA1R_FFA3                      0x00000008U        /*!<Filter FIFO Assignment for Filter 3 */
#define  CAN_FFA1R_FFA4                      0x00000010U        /*!<Filter FIFO Assignment for Filter 4 */
#define  CAN_FFA1R_FFA5                      0x00000020U        /*!<Filter FIFO Assignment for Filter 5 */
#define  CAN_FFA1R_FFA6                      0x00000040U        /*!<Filter FIFO Assignment for Filter 6 */
#define  CAN_FFA1R_FFA7                      0x00000080U        /*!<Filter FIFO Assignment for Filter 7 */
#define  CAN_FFA1R_FFA8                      0x00000100U        /*!<Filter FIFO Assignment for Filter 8 */
#define  CAN_FFA1R_FFA9                      0x00000200U        /*!<Filter FIFO Assignment for Filter 9 */
#define  CAN_FFA1R_FFA10                     0x00000400U        /*!<Filter FIFO Assignment for Filter 10 */
#define  CAN_FFA1R_FFA11                     0x00000800U        /*!<Filter FIFO Assignment for Filter 11 */
#define  CAN_FFA1R_FFA12                     0x00001000U        /*!<Filter FIFO Assignment for Filter 12 */
#define  CAN_FFA1R_FFA13                     0x00002000U        /*!<Filter FIFO Assignment for Filter 13 */

/*******************  Bit definition for CAN_FA1R register  *******************/
#define  CAN_FA1R_FACT                       0x00003FFFU        /*!<Filter Active    */
#define  CAN_FA1R_FACT0                      0x00000001U        /*!<Filter 0 Active  */
#define  CAN_FA1R_FACT1                      0x00000002U        /*!<Filter 1 Active  */
#define  CAN_FA1R_FACT2                      0x00000004U        /*!<Filter 2 Active  */
#define  CAN_FA1R_FACT3                      0x00000008U        /*!<Filter 3 Active  */
#define  CAN_FA1R_FACT4                      0x00000010U        /*!<Filter 4 Active  */
#define  CAN_FA1R_FACT5                      0x00000020U        /*!<Filter 5 Active  */
#define  CAN_FA1R_FACT6                      0x00000040U        /*!<Filter 6 Active  */
#define  CAN_FA1R_FACT7                      0x00000080U        /*!<Filter 7 Active  */
#define  CAN_FA1R_FACT8                      0x00000100U        /*!<Filter 8 Active  */
#define  CAN_FA1R_FACT9                      0x00000200U        /*!<Filter 9 Active  */
#define  CAN_FA1R_FACT10                     0x00000400U        /*!<Filter 10 Active */
#define  CAN_FA1R_FACT11                     0x00000800U        /*!<Filter 11 Active */
#define  CAN_FA1R_FACT12                     0x00001000U        /*!<Filter 12 Active */
#define  CAN_FA1R_FACT13                     0x00002000U        /*!<Filter 13 Active */

/*******************  Bit definition for CAN_F0R1 register  *******************/
#define  CAN_F0R1_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F0R1_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F0R1_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F0R1_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F0R1_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F0R1_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F0R1_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F0R1_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F0R1_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F0R1_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F0R1_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F0R1_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F0R1_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F0R1_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F0R1_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F0R1_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F0R1_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F0R1_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F0R1_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F0R1_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F0R1_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F0R1_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F0R1_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F0R1_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F0R1_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F0R1_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F0R1_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F0R1_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F0R1_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F0R1_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F0R1_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F0R1_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F1R1 register  *******************/
#define  CAN_F1R1_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F1R1_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F1R1_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F1R1_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F1R1_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F1R1_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F1R1_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F1R1_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F1R1_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F1R1_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F1R1_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F1R1_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F1R1_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F1R1_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F1R1_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F1R1_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F1R1_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F1R1_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F1R1_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F1R1_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F1R1_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F1R1_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F1R1_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F1R1_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F1R1_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F1R1_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F1R1_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F1R1_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F1R1_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F1R1_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F1R1_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F1R1_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F2R1 register  *******************/
#define  CAN_F2R1_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F2R1_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F2R1_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F2R1_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F2R1_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F2R1_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F2R1_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F2R1_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F2R1_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F2R1_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F2R1_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F2R1_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F2R1_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F2R1_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F2R1_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F2R1_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F2R1_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F2R1_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F2R1_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F2R1_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F2R1_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F2R1_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F2R1_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F2R1_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F2R1_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F2R1_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F2R1_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F2R1_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F2R1_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F2R1_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F2R1_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F2R1_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F3R1 register  *******************/
#define  CAN_F3R1_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F3R1_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F3R1_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F3R1_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F3R1_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F3R1_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F3R1_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F3R1_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F3R1_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F3R1_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F3R1_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F3R1_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F3R1_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F3R1_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F3R1_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F3R1_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F3R1_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F3R1_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F3R1_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F3R1_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F3R1_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F3R1_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F3R1_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F3R1_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F3R1_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F3R1_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F3R1_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F3R1_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F3R1_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F3R1_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F3R1_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F3R1_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F4R1 register  *******************/
#define  CAN_F4R1_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F4R1_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F4R1_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F4R1_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F4R1_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F4R1_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F4R1_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F4R1_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F4R1_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F4R1_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F4R1_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F4R1_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F4R1_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F4R1_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F4R1_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F4R1_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F4R1_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F4R1_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F4R1_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F4R1_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F4R1_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F4R1_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F4R1_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F4R1_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F4R1_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F4R1_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F4R1_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F4R1_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F4R1_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F4R1_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F4R1_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F4R1_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F5R1 register  *******************/
#define  CAN_F5R1_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F5R1_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F5R1_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F5R1_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F5R1_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F5R1_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F5R1_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F5R1_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F5R1_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F5R1_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F5R1_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F5R1_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F5R1_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F5R1_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F5R1_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F5R1_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F5R1_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F5R1_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F5R1_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F5R1_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F5R1_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F5R1_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F5R1_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F5R1_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F5R1_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F5R1_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F5R1_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F5R1_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F5R1_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F5R1_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F5R1_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F5R1_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F6R1 register  *******************/
#define  CAN_F6R1_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F6R1_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F6R1_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F6R1_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F6R1_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F6R1_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F6R1_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F6R1_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F6R1_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F6R1_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F6R1_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F6R1_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F6R1_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F6R1_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F6R1_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F6R1_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F6R1_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F6R1_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F6R1_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F6R1_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F6R1_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F6R1_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F6R1_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F6R1_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F6R1_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F6R1_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F6R1_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F6R1_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F6R1_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F6R1_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F6R1_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F6R1_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F7R1 register  *******************/
#define  CAN_F7R1_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F7R1_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F7R1_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F7R1_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F7R1_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F7R1_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F7R1_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F7R1_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F7R1_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F7R1_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F7R1_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F7R1_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F7R1_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F7R1_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F7R1_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F7R1_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F7R1_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F7R1_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F7R1_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F7R1_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F7R1_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F7R1_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F7R1_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F7R1_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F7R1_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F7R1_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F7R1_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F7R1_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F7R1_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F7R1_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F7R1_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F7R1_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F8R1 register  *******************/
#define  CAN_F8R1_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F8R1_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F8R1_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F8R1_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F8R1_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F8R1_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F8R1_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F8R1_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F8R1_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F8R1_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F8R1_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F8R1_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F8R1_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F8R1_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F8R1_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F8R1_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F8R1_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F8R1_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F8R1_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F8R1_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F8R1_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F8R1_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F8R1_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F8R1_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F8R1_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F8R1_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F8R1_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F8R1_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F8R1_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F8R1_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F8R1_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F8R1_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F9R1 register  *******************/
#define  CAN_F9R1_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F9R1_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F9R1_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F9R1_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F9R1_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F9R1_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F9R1_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F9R1_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F9R1_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F9R1_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F9R1_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F9R1_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F9R1_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F9R1_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F9R1_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F9R1_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F9R1_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F9R1_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F9R1_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F9R1_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F9R1_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F9R1_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F9R1_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F9R1_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F9R1_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F9R1_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F9R1_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F9R1_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F9R1_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F9R1_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F9R1_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F9R1_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F10R1 register  ******************/
#define  CAN_F10R1_FB0                       0x00000001U        /*!<Filter bit 0 */
#define  CAN_F10R1_FB1                       0x00000002U        /*!<Filter bit 1 */
#define  CAN_F10R1_FB2                       0x00000004U        /*!<Filter bit 2 */
#define  CAN_F10R1_FB3                       0x00000008U        /*!<Filter bit 3 */
#define  CAN_F10R1_FB4                       0x00000010U        /*!<Filter bit 4 */
#define  CAN_F10R1_FB5                       0x00000020U        /*!<Filter bit 5 */
#define  CAN_F10R1_FB6                       0x00000040U        /*!<Filter bit 6 */
#define  CAN_F10R1_FB7                       0x00000080U        /*!<Filter bit 7 */
#define  CAN_F10R1_FB8                       0x00000100U        /*!<Filter bit 8 */
#define  CAN_F10R1_FB9                       0x00000200U        /*!<Filter bit 9 */
#define  CAN_F10R1_FB10                      0x00000400U        /*!<Filter bit 10 */
#define  CAN_F10R1_FB11                      0x00000800U        /*!<Filter bit 11 */
#define  CAN_F10R1_FB12                      0x00001000U        /*!<Filter bit 12 */
#define  CAN_F10R1_FB13                      0x00002000U        /*!<Filter bit 13 */
#define  CAN_F10R1_FB14                      0x00004000U        /*!<Filter bit 14 */
#define  CAN_F10R1_FB15                      0x00008000U        /*!<Filter bit 15 */
#define  CAN_F10R1_FB16                      0x00010000U        /*!<Filter bit 16 */
#define  CAN_F10R1_FB17                      0x00020000U        /*!<Filter bit 17 */
#define  CAN_F10R1_FB18                      0x00040000U        /*!<Filter bit 18 */
#define  CAN_F10R1_FB19                      0x00080000U        /*!<Filter bit 19 */
#define  CAN_F10R1_FB20                      0x00100000U        /*!<Filter bit 20 */
#define  CAN_F10R1_FB21                      0x00200000U        /*!<Filter bit 21 */
#define  CAN_F10R1_FB22                      0x00400000U        /*!<Filter bit 22 */
#define  CAN_F10R1_FB23                      0x00800000U        /*!<Filter bit 23 */
#define  CAN_F10R1_FB24                      0x01000000U        /*!<Filter bit 24 */
#define  CAN_F10R1_FB25                      0x02000000U        /*!<Filter bit 25 */
#define  CAN_F10R1_FB26                      0x04000000U        /*!<Filter bit 26 */
#define  CAN_F10R1_FB27                      0x08000000U        /*!<Filter bit 27 */
#define  CAN_F10R1_FB28                      0x10000000U        /*!<Filter bit 28 */
#define  CAN_F10R1_FB29                      0x20000000U        /*!<Filter bit 29 */
#define  CAN_F10R1_FB30                      0x40000000U        /*!<Filter bit 30 */
#define  CAN_F10R1_FB31                      0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F11R1 register  ******************/
#define  CAN_F11R1_FB0                       0x00000001U        /*!<Filter bit 0 */
#define  CAN_F11R1_FB1                       0x00000002U        /*!<Filter bit 1 */
#define  CAN_F11R1_FB2                       0x00000004U        /*!<Filter bit 2 */
#define  CAN_F11R1_FB3                       0x00000008U        /*!<Filter bit 3 */
#define  CAN_F11R1_FB4                       0x00000010U        /*!<Filter bit 4 */
#define  CAN_F11R1_FB5                       0x00000020U        /*!<Filter bit 5 */
#define  CAN_F11R1_FB6                       0x00000040U        /*!<Filter bit 6 */
#define  CAN_F11R1_FB7                       0x00000080U        /*!<Filter bit 7 */
#define  CAN_F11R1_FB8                       0x00000100U        /*!<Filter bit 8 */
#define  CAN_F11R1_FB9                       0x00000200U        /*!<Filter bit 9 */
#define  CAN_F11R1_FB10                      0x00000400U        /*!<Filter bit 10 */
#define  CAN_F11R1_FB11                      0x00000800U        /*!<Filter bit 11 */
#define  CAN_F11R1_FB12                      0x00001000U        /*!<Filter bit 12 */
#define  CAN_F11R1_FB13                      0x00002000U        /*!<Filter bit 13 */
#define  CAN_F11R1_FB14                      0x00004000U        /*!<Filter bit 14 */
#define  CAN_F11R1_FB15                      0x00008000U        /*!<Filter bit 15 */
#define  CAN_F11R1_FB16                      0x00010000U        /*!<Filter bit 16 */
#define  CAN_F11R1_FB17                      0x00020000U        /*!<Filter bit 17 */
#define  CAN_F11R1_FB18                      0x00040000U        /*!<Filter bit 18 */
#define  CAN_F11R1_FB19                      0x00080000U        /*!<Filter bit 19 */
#define  CAN_F11R1_FB20                      0x00100000U        /*!<Filter bit 20 */
#define  CAN_F11R1_FB21                      0x00200000U        /*!<Filter bit 21 */
#define  CAN_F11R1_FB22                      0x00400000U        /*!<Filter bit 22 */
#define  CAN_F11R1_FB23                      0x00800000U        /*!<Filter bit 23 */
#define  CAN_F11R1_FB24                      0x01000000U        /*!<Filter bit 24 */
#define  CAN_F11R1_FB25                      0x02000000U        /*!<Filter bit 25 */
#define  CAN_F11R1_FB26                      0x04000000U        /*!<Filter bit 26 */
#define  CAN_F11R1_FB27                      0x08000000U        /*!<Filter bit 27 */
#define  CAN_F11R1_FB28                      0x10000000U        /*!<Filter bit 28 */
#define  CAN_F11R1_FB29                      0x20000000U        /*!<Filter bit 29 */
#define  CAN_F11R1_FB30                      0x40000000U        /*!<Filter bit 30 */
#define  CAN_F11R1_FB31                      0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F12R1 register  ******************/
#define  CAN_F12R1_FB0                       0x00000001U        /*!<Filter bit 0 */
#define  CAN_F12R1_FB1                       0x00000002U        /*!<Filter bit 1 */
#define  CAN_F12R1_FB2                       0x00000004U        /*!<Filter bit 2 */
#define  CAN_F12R1_FB3                       0x00000008U        /*!<Filter bit 3 */
#define  CAN_F12R1_FB4                       0x00000010U        /*!<Filter bit 4 */
#define  CAN_F12R1_FB5                       0x00000020U        /*!<Filter bit 5 */
#define  CAN_F12R1_FB6                       0x00000040U        /*!<Filter bit 6 */
#define  CAN_F12R1_FB7                       0x00000080U        /*!<Filter bit 7 */
#define  CAN_F12R1_FB8                       0x00000100U        /*!<Filter bit 8 */
#define  CAN_F12R1_FB9                       0x00000200U        /*!<Filter bit 9 */
#define  CAN_F12R1_FB10                      0x00000400U        /*!<Filter bit 10 */
#define  CAN_F12R1_FB11                      0x00000800U        /*!<Filter bit 11 */
#define  CAN_F12R1_FB12                      0x00001000U        /*!<Filter bit 12 */
#define  CAN_F12R1_FB13                      0x00002000U        /*!<Filter bit 13 */
#define  CAN_F12R1_FB14                      0x00004000U        /*!<Filter bit 14 */
#define  CAN_F12R1_FB15                      0x00008000U        /*!<Filter bit 15 */
#define  CAN_F12R1_FB16                      0x00010000U        /*!<Filter bit 16 */
#define  CAN_F12R1_FB17                      0x00020000U        /*!<Filter bit 17 */
#define  CAN_F12R1_FB18                      0x00040000U        /*!<Filter bit 18 */
#define  CAN_F12R1_FB19                      0x00080000U        /*!<Filter bit 19 */
#define  CAN_F12R1_FB20                      0x00100000U        /*!<Filter bit 20 */
#define  CAN_F12R1_FB21                      0x00200000U        /*!<Filter bit 21 */
#define  CAN_F12R1_FB22                      0x00400000U        /*!<Filter bit 22 */
#define  CAN_F12R1_FB23                      0x00800000U        /*!<Filter bit 23 */
#define  CAN_F12R1_FB24                      0x01000000U        /*!<Filter bit 24 */
#define  CAN_F12R1_FB25                      0x02000000U        /*!<Filter bit 25 */
#define  CAN_F12R1_FB26                      0x04000000U        /*!<Filter bit 26 */
#define  CAN_F12R1_FB27                      0x08000000U        /*!<Filter bit 27 */
#define  CAN_F12R1_FB28                      0x10000000U        /*!<Filter bit 28 */
#define  CAN_F12R1_FB29                      0x20000000U        /*!<Filter bit 29 */
#define  CAN_F12R1_FB30                      0x40000000U        /*!<Filter bit 30 */
#define  CAN_F12R1_FB31                      0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F13R1 register  ******************/
#define  CAN_F13R1_FB0                       0x00000001U        /*!<Filter bit 0 */
#define  CAN_F13R1_FB1                       0x00000002U        /*!<Filter bit 1 */
#define  CAN_F13R1_FB2                       0x00000004U        /*!<Filter bit 2 */
#define  CAN_F13R1_FB3                       0x00000008U        /*!<Filter bit 3 */
#define  CAN_F13R1_FB4                       0x00000010U        /*!<Filter bit 4 */
#define  CAN_F13R1_FB5                       0x00000020U        /*!<Filter bit 5 */
#define  CAN_F13R1_FB6                       0x00000040U        /*!<Filter bit 6 */
#define  CAN_F13R1_FB7                       0x00000080U        /*!<Filter bit 7 */
#define  CAN_F13R1_FB8                       0x00000100U        /*!<Filter bit 8 */
#define  CAN_F13R1_FB9                       0x00000200U        /*!<Filter bit 9 */
#define  CAN_F13R1_FB10                      0x00000400U        /*!<Filter bit 10 */
#define  CAN_F13R1_FB11                      0x00000800U        /*!<Filter bit 11 */
#define  CAN_F13R1_FB12                      0x00001000U        /*!<Filter bit 12 */
#define  CAN_F13R1_FB13                      0x00002000U        /*!<Filter bit 13 */
#define  CAN_F13R1_FB14                      0x00004000U        /*!<Filter bit 14 */
#define  CAN_F13R1_FB15                      0x00008000U        /*!<Filter bit 15 */
#define  CAN_F13R1_FB16                      0x00010000U        /*!<Filter bit 16 */
#define  CAN_F13R1_FB17                      0x00020000U        /*!<Filter bit 17 */
#define  CAN_F13R1_FB18                      0x00040000U        /*!<Filter bit 18 */
#define  CAN_F13R1_FB19                      0x00080000U        /*!<Filter bit 19 */
#define  CAN_F13R1_FB20                      0x00100000U        /*!<Filter bit 20 */
#define  CAN_F13R1_FB21                      0x00200000U        /*!<Filter bit 21 */
#define  CAN_F13R1_FB22                      0x00400000U        /*!<Filter bit 22 */
#define  CAN_F13R1_FB23                      0x00800000U        /*!<Filter bit 23 */
#define  CAN_F13R1_FB24                      0x01000000U        /*!<Filter bit 24 */
#define  CAN_F13R1_FB25                      0x02000000U        /*!<Filter bit 25 */
#define  CAN_F13R1_FB26                      0x04000000U        /*!<Filter bit 26 */
#define  CAN_F13R1_FB27                      0x08000000U        /*!<Filter bit 27 */
#define  CAN_F13R1_FB28                      0x10000000U        /*!<Filter bit 28 */
#define  CAN_F13R1_FB29                      0x20000000U        /*!<Filter bit 29 */
#define  CAN_F13R1_FB30                      0x40000000U        /*!<Filter bit 30 */
#define  CAN_F13R1_FB31                      0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F0R2 register  *******************/
#define  CAN_F0R2_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F0R2_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F0R2_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F0R2_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F0R2_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F0R2_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F0R2_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F0R2_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F0R2_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F0R2_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F0R2_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F0R2_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F0R2_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F0R2_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F0R2_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F0R2_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F0R2_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F0R2_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F0R2_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F0R2_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F0R2_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F0R2_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F0R2_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F0R2_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F0R2_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F0R2_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F0R2_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F0R2_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F0R2_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F0R2_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F0R2_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F0R2_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F1R2 register  *******************/
#define  CAN_F1R2_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F1R2_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F1R2_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F1R2_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F1R2_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F1R2_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F1R2_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F1R2_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F1R2_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F1R2_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F1R2_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F1R2_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F1R2_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F1R2_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F1R2_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F1R2_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F1R2_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F1R2_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F1R2_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F1R2_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F1R2_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F1R2_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F1R2_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F1R2_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F1R2_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F1R2_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F1R2_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F1R2_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F1R2_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F1R2_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F1R2_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F1R2_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F2R2 register  *******************/
#define  CAN_F2R2_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F2R2_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F2R2_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F2R2_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F2R2_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F2R2_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F2R2_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F2R2_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F2R2_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F2R2_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F2R2_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F2R2_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F2R2_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F2R2_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F2R2_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F2R2_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F2R2_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F2R2_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F2R2_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F2R2_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F2R2_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F2R2_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F2R2_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F2R2_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F2R2_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F2R2_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F2R2_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F2R2_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F2R2_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F2R2_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F2R2_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F2R2_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F3R2 register  *******************/
#define  CAN_F3R2_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F3R2_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F3R2_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F3R2_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F3R2_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F3R2_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F3R2_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F3R2_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F3R2_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F3R2_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F3R2_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F3R2_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F3R2_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F3R2_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F3R2_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F3R2_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F3R2_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F3R2_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F3R2_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F3R2_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F3R2_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F3R2_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F3R2_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F3R2_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F3R2_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F3R2_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F3R2_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F3R2_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F3R2_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F3R2_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F3R2_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F3R2_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F4R2 register  *******************/
#define  CAN_F4R2_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F4R2_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F4R2_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F4R2_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F4R2_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F4R2_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F4R2_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F4R2_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F4R2_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F4R2_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F4R2_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F4R2_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F4R2_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F4R2_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F4R2_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F4R2_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F4R2_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F4R2_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F4R2_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F4R2_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F4R2_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F4R2_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F4R2_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F4R2_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F4R2_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F4R2_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F4R2_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F4R2_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F4R2_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F4R2_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F4R2_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F4R2_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F5R2 register  *******************/
#define  CAN_F5R2_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F5R2_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F5R2_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F5R2_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F5R2_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F5R2_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F5R2_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F5R2_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F5R2_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F5R2_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F5R2_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F5R2_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F5R2_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F5R2_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F5R2_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F5R2_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F5R2_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F5R2_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F5R2_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F5R2_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F5R2_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F5R2_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F5R2_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F5R2_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F5R2_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F5R2_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F5R2_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F5R2_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F5R2_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F5R2_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F5R2_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F5R2_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F6R2 register  *******************/
#define  CAN_F6R2_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F6R2_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F6R2_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F6R2_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F6R2_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F6R2_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F6R2_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F6R2_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F6R2_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F6R2_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F6R2_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F6R2_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F6R2_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F6R2_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F6R2_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F6R2_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F6R2_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F6R2_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F6R2_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F6R2_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F6R2_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F6R2_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F6R2_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F6R2_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F6R2_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F6R2_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F6R2_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F6R2_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F6R2_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F6R2_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F6R2_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F6R2_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F7R2 register  *******************/
#define  CAN_F7R2_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F7R2_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F7R2_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F7R2_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F7R2_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F7R2_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F7R2_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F7R2_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F7R2_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F7R2_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F7R2_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F7R2_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F7R2_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F7R2_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F7R2_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F7R2_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F7R2_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F7R2_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F7R2_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F7R2_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F7R2_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F7R2_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F7R2_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F7R2_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F7R2_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F7R2_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F7R2_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F7R2_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F7R2_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F7R2_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F7R2_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F7R2_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F8R2 register  *******************/
#define  CAN_F8R2_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F8R2_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F8R2_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F8R2_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F8R2_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F8R2_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F8R2_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F8R2_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F8R2_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F8R2_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F8R2_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F8R2_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F8R2_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F8R2_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F8R2_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F8R2_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F8R2_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F8R2_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F8R2_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F8R2_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F8R2_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F8R2_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F8R2_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F8R2_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F8R2_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F8R2_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F8R2_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F8R2_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F8R2_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F8R2_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F8R2_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F8R2_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F9R2 register  *******************/
#define  CAN_F9R2_FB0                        0x00000001U        /*!<Filter bit 0 */
#define  CAN_F9R2_FB1                        0x00000002U        /*!<Filter bit 1 */
#define  CAN_F9R2_FB2                        0x00000004U        /*!<Filter bit 2 */
#define  CAN_F9R2_FB3                        0x00000008U        /*!<Filter bit 3 */
#define  CAN_F9R2_FB4                        0x00000010U        /*!<Filter bit 4 */
#define  CAN_F9R2_FB5                        0x00000020U        /*!<Filter bit 5 */
#define  CAN_F9R2_FB6                        0x00000040U        /*!<Filter bit 6 */
#define  CAN_F9R2_FB7                        0x00000080U        /*!<Filter bit 7 */
#define  CAN_F9R2_FB8                        0x00000100U        /*!<Filter bit 8 */
#define  CAN_F9R2_FB9                        0x00000200U        /*!<Filter bit 9 */
#define  CAN_F9R2_FB10                       0x00000400U        /*!<Filter bit 10 */
#define  CAN_F9R2_FB11                       0x00000800U        /*!<Filter bit 11 */
#define  CAN_F9R2_FB12                       0x00001000U        /*!<Filter bit 12 */
#define  CAN_F9R2_FB13                       0x00002000U        /*!<Filter bit 13 */
#define  CAN_F9R2_FB14                       0x00004000U        /*!<Filter bit 14 */
#define  CAN_F9R2_FB15                       0x00008000U        /*!<Filter bit 15 */
#define  CAN_F9R2_FB16                       0x00010000U        /*!<Filter bit 16 */
#define  CAN_F9R2_FB17                       0x00020000U        /*!<Filter bit 17 */
#define  CAN_F9R2_FB18                       0x00040000U        /*!<Filter bit 18 */
#define  CAN_F9R2_FB19                       0x00080000U        /*!<Filter bit 19 */
#define  CAN_F9R2_FB20                       0x00100000U        /*!<Filter bit 20 */
#define  CAN_F9R2_FB21                       0x00200000U        /*!<Filter bit 21 */
#define  CAN_F9R2_FB22                       0x00400000U        /*!<Filter bit 22 */
#define  CAN_F9R2_FB23                       0x00800000U        /*!<Filter bit 23 */
#define  CAN_F9R2_FB24                       0x01000000U        /*!<Filter bit 24 */
#define  CAN_F9R2_FB25                       0x02000000U        /*!<Filter bit 25 */
#define  CAN_F9R2_FB26                       0x04000000U        /*!<Filter bit 26 */
#define  CAN_F9R2_FB27                       0x08000000U        /*!<Filter bit 27 */
#define  CAN_F9R2_FB28                       0x10000000U        /*!<Filter bit 28 */
#define  CAN_F9R2_FB29                       0x20000000U        /*!<Filter bit 29 */
#define  CAN_F9R2_FB30                       0x40000000U        /*!<Filter bit 30 */
#define  CAN_F9R2_FB31                       0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F10R2 register  ******************/
#define  CAN_F10R2_FB0                       0x00000001U        /*!<Filter bit 0 */
#define  CAN_F10R2_FB1                       0x00000002U        /*!<Filter bit 1 */
#define  CAN_F10R2_FB2                       0x00000004U        /*!<Filter bit 2 */
#define  CAN_F10R2_FB3                       0x00000008U        /*!<Filter bit 3 */
#define  CAN_F10R2_FB4                       0x00000010U        /*!<Filter bit 4 */
#define  CAN_F10R2_FB5                       0x00000020U        /*!<Filter bit 5 */
#define  CAN_F10R2_FB6                       0x00000040U        /*!<Filter bit 6 */
#define  CAN_F10R2_FB7                       0x00000080U        /*!<Filter bit 7 */
#define  CAN_F10R2_FB8                       0x00000100U        /*!<Filter bit 8 */
#define  CAN_F10R2_FB9                       0x00000200U        /*!<Filter bit 9 */
#define  CAN_F10R2_FB10                      0x00000400U        /*!<Filter bit 10 */
#define  CAN_F10R2_FB11                      0x00000800U        /*!<Filter bit 11 */
#define  CAN_F10R2_FB12                      0x00001000U        /*!<Filter bit 12 */
#define  CAN_F10R2_FB13                      0x00002000U        /*!<Filter bit 13 */
#define  CAN_F10R2_FB14                      0x00004000U        /*!<Filter bit 14 */
#define  CAN_F10R2_FB15                      0x00008000U        /*!<Filter bit 15 */
#define  CAN_F10R2_FB16                      0x00010000U        /*!<Filter bit 16 */
#define  CAN_F10R2_FB17                      0x00020000U        /*!<Filter bit 17 */
#define  CAN_F10R2_FB18                      0x00040000U        /*!<Filter bit 18 */
#define  CAN_F10R2_FB19                      0x00080000U        /*!<Filter bit 19 */
#define  CAN_F10R2_FB20                      0x00100000U        /*!<Filter bit 20 */
#define  CAN_F10R2_FB21                      0x00200000U        /*!<Filter bit 21 */
#define  CAN_F10R2_FB22                      0x00400000U        /*!<Filter bit 22 */
#define  CAN_F10R2_FB23                      0x00800000U        /*!<Filter bit 23 */
#define  CAN_F10R2_FB24                      0x01000000U        /*!<Filter bit 24 */
#define  CAN_F10R2_FB25                      0x02000000U        /*!<Filter bit 25 */
#define  CAN_F10R2_FB26                      0x04000000U        /*!<Filter bit 26 */
#define  CAN_F10R2_FB27                      0x08000000U        /*!<Filter bit 27 */
#define  CAN_F10R2_FB28                      0x10000000U        /*!<Filter bit 28 */
#define  CAN_F10R2_FB29                      0x20000000U        /*!<Filter bit 29 */
#define  CAN_F10R2_FB30                      0x40000000U        /*!<Filter bit 30 */
#define  CAN_F10R2_FB31                      0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F11R2 register  ******************/
#define  CAN_F11R2_FB0                       0x00000001U        /*!<Filter bit 0 */
#define  CAN_F11R2_FB1                       0x00000002U        /*!<Filter bit 1 */
#define  CAN_F11R2_FB2                       0x00000004U        /*!<Filter bit 2 */
#define  CAN_F11R2_FB3                       0x00000008U        /*!<Filter bit 3 */
#define  CAN_F11R2_FB4                       0x00000010U        /*!<Filter bit 4 */
#define  CAN_F11R2_FB5                       0x00000020U        /*!<Filter bit 5 */
#define  CAN_F11R2_FB6                       0x00000040U        /*!<Filter bit 6 */
#define  CAN_F11R2_FB7                       0x00000080U        /*!<Filter bit 7 */
#define  CAN_F11R2_FB8                       0x00000100U        /*!<Filter bit 8 */
#define  CAN_F11R2_FB9                       0x00000200U        /*!<Filter bit 9 */
#define  CAN_F11R2_FB10                      0x00000400U        /*!<Filter bit 10 */
#define  CAN_F11R2_FB11                      0x00000800U        /*!<Filter bit 11 */
#define  CAN_F11R2_FB12                      0x00001000U        /*!<Filter bit 12 */
#define  CAN_F11R2_FB13                      0x00002000U        /*!<Filter bit 13 */
#define  CAN_F11R2_FB14                      0x00004000U        /*!<Filter bit 14 */
#define  CAN_F11R2_FB15                      0x00008000U        /*!<Filter bit 15 */
#define  CAN_F11R2_FB16                      0x00010000U        /*!<Filter bit 16 */
#define  CAN_F11R2_FB17                      0x00020000U        /*!<Filter bit 17 */
#define  CAN_F11R2_FB18                      0x00040000U        /*!<Filter bit 18 */
#define  CAN_F11R2_FB19                      0x00080000U        /*!<Filter bit 19 */
#define  CAN_F11R2_FB20                      0x00100000U        /*!<Filter bit 20 */
#define  CAN_F11R2_FB21                      0x00200000U        /*!<Filter bit 21 */
#define  CAN_F11R2_FB22                      0x00400000U        /*!<Filter bit 22 */
#define  CAN_F11R2_FB23                      0x00800000U        /*!<Filter bit 23 */
#define  CAN_F11R2_FB24                      0x01000000U        /*!<Filter bit 24 */
#define  CAN_F11R2_FB25                      0x02000000U        /*!<Filter bit 25 */
#define  CAN_F11R2_FB26                      0x04000000U        /*!<Filter bit 26 */
#define  CAN_F11R2_FB27                      0x08000000U        /*!<Filter bit 27 */
#define  CAN_F11R2_FB28                      0x10000000U        /*!<Filter bit 28 */
#define  CAN_F11R2_FB29                      0x20000000U        /*!<Filter bit 29 */
#define  CAN_F11R2_FB30                      0x40000000U        /*!<Filter bit 30 */
#define  CAN_F11R2_FB31                      0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F12R2 register  ******************/
#define  CAN_F12R2_FB0                       0x00000001U        /*!<Filter bit 0 */
#define  CAN_F12R2_FB1                       0x00000002U        /*!<Filter bit 1 */
#define  CAN_F12R2_FB2                       0x00000004U        /*!<Filter bit 2 */
#define  CAN_F12R2_FB3                       0x00000008U        /*!<Filter bit 3 */
#define  CAN_F12R2_FB4                       0x00000010U        /*!<Filter bit 4 */
#define  CAN_F12R2_FB5                       0x00000020U        /*!<Filter bit 5 */
#define  CAN_F12R2_FB6                       0x00000040U        /*!<Filter bit 6 */
#define  CAN_F12R2_FB7                       0x00000080U        /*!<Filter bit 7 */
#define  CAN_F12R2_FB8                       0x00000100U        /*!<Filter bit 8 */
#define  CAN_F12R2_FB9                       0x00000200U        /*!<Filter bit 9 */
#define  CAN_F12R2_FB10                      0x00000400U        /*!<Filter bit 10 */
#define  CAN_F12R2_FB11                      0x00000800U        /*!<Filter bit 11 */
#define  CAN_F12R2_FB12                      0x00001000U        /*!<Filter bit 12 */
#define  CAN_F12R2_FB13                      0x00002000U        /*!<Filter bit 13 */
#define  CAN_F12R2_FB14                      0x00004000U        /*!<Filter bit 14 */
#define  CAN_F12R2_FB15                      0x00008000U        /*!<Filter bit 15 */
#define  CAN_F12R2_FB16                      0x00010000U        /*!<Filter bit 16 */
#define  CAN_F12R2_FB17                      0x00020000U        /*!<Filter bit 17 */
#define  CAN_F12R2_FB18                      0x00040000U        /*!<Filter bit 18 */
#define  CAN_F12R2_FB19                      0x00080000U        /*!<Filter bit 19 */
#define  CAN_F12R2_FB20                      0x00100000U        /*!<Filter bit 20 */
#define  CAN_F12R2_FB21                      0x00200000U        /*!<Filter bit 21 */
#define  CAN_F12R2_FB22                      0x00400000U        /*!<Filter bit 22 */
#define  CAN_F12R2_FB23                      0x00800000U        /*!<Filter bit 23 */
#define  CAN_F12R2_FB24                      0x01000000U        /*!<Filter bit 24 */
#define  CAN_F12R2_FB25                      0x02000000U        /*!<Filter bit 25 */
#define  CAN_F12R2_FB26                      0x04000000U        /*!<Filter bit 26 */
#define  CAN_F12R2_FB27                      0x08000000U        /*!<Filter bit 27 */
#define  CAN_F12R2_FB28                      0x10000000U        /*!<Filter bit 28 */
#define  CAN_F12R2_FB29                      0x20000000U        /*!<Filter bit 29 */
#define  CAN_F12R2_FB30                      0x40000000U        /*!<Filter bit 30 */
#define  CAN_F12R2_FB31                      0x80000000U        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F13R2 register  ******************/
#define  CAN_F13R2_FB0                       0x00000001U        /*!<Filter bit 0 */
#define  CAN_F13R2_FB1                       0x00000002U        /*!<Filter bit 1 */
#define  CAN_F13R2_FB2                       0x00000004U        /*!<Filter bit 2 */
#define  CAN_F13R2_FB3                       0x00000008U        /*!<Filter bit 3 */
#define  CAN_F13R2_FB4                       0x00000010U        /*!<Filter bit 4 */
#define  CAN_F13R2_FB5                       0x00000020U        /*!<Filter bit 5 */
#define  CAN_F13R2_FB6                       0x00000040U        /*!<Filter bit 6 */
#define  CAN_F13R2_FB7                       0x00000080U        /*!<Filter bit 7 */
#define  CAN_F13R2_FB8                       0x00000100U        /*!<Filter bit 8 */
#define  CAN_F13R2_FB9                       0x00000200U        /*!<Filter bit 9 */
#define  CAN_F13R2_FB10                      0x00000400U        /*!<Filter bit 10 */
#define  CAN_F13R2_FB11                      0x00000800U        /*!<Filter bit 11 */
#define  CAN_F13R2_FB12                      0x00001000U        /*!<Filter bit 12 */
#define  CAN_F13R2_FB13                      0x00002000U        /*!<Filter bit 13 */
#define  CAN_F13R2_FB14                      0x00004000U        /*!<Filter bit 14 */
#define  CAN_F13R2_FB15                      0x00008000U        /*!<Filter bit 15 */
#define  CAN_F13R2_FB16                      0x00010000U        /*!<Filter bit 16 */
#define  CAN_F13R2_FB17                      0x00020000U        /*!<Filter bit 17 */
#define  CAN_F13R2_FB18                      0x00040000U        /*!<Filter bit 18 */
#define  CAN_F13R2_FB19                      0x00080000U        /*!<Filter bit 19 */
#define  CAN_F13R2_FB20                      0x00100000U        /*!<Filter bit 20 */
#define  CAN_F13R2_FB21                      0x00200000U        /*!<Filter bit 21 */
#define  CAN_F13R2_FB22                      0x00400000U        /*!<Filter bit 22 */
#define  CAN_F13R2_FB23                      0x00800000U        /*!<Filter bit 23 */
#define  CAN_F13R2_FB24                      0x01000000U        /*!<Filter bit 24 */
#define  CAN_F13R2_FB25                      0x02000000U        /*!<Filter bit 25 */
#define  CAN_F13R2_FB26                      0x04000000U        /*!<Filter bit 26 */
#define  CAN_F13R2_FB27                      0x08000000U        /*!<Filter bit 27 */
#define  CAN_F13R2_FB28                      0x10000000U        /*!<Filter bit 28 */
#define  CAN_F13R2_FB29                      0x20000000U        /*!<Filter bit 29 */
#define  CAN_F13R2_FB30                      0x40000000U        /*!<Filter bit 30 */
#define  CAN_F13R2_FB31                      0x80000000U        /*!<Filter bit 31 */

/******************************************************************************/
/*                                                                            */
/*                                 HDMI-CEC (CEC)                             */
/*                                                                            */
/******************************************************************************/

/*******************  Bit definition for CEC_CR register  *********************/
#define  CEC_CR_CECEN                        0x00000001U       /*!< CEC Enable                         */
#define  CEC_CR_TXSOM                        0x00000002U       /*!< CEC Tx Start Of Message            */
#define  CEC_CR_TXEOM                        0x00000004U       /*!< CEC Tx End Of Message              */

/*******************  Bit definition for CEC_CFGR register  *******************/
#define  CEC_CFGR_SFT                        0x00000007U       /*!< CEC Signal Free Time               */
#define  CEC_CFGR_RXTOL                      0x00000008U       /*!< CEC Tolerance                      */
#define  CEC_CFGR_BRESTP                     0x00000010U       /*!< CEC Rx Stop                        */
#define  CEC_CFGR_BREGEN                     0x00000020U       /*!< CEC Bit Rising Error generation    */
#define  CEC_CFGR_LBPEGEN                    0x00000040U       /*!< CEC Long Period Error generation   */
#define  CEC_CFGR_BRDNOGEN                   0x00000080U       /*!< CEC Broadcast no Error generation  */
#define  CEC_CFGR_SFTOPT                     0x00000100U       /*!< CEC Signal Free Time optional      */
#define  CEC_CFGR_OAR                        0x7FFF0000U       /*!< CEC Own Address                    */
#define  CEC_CFGR_LSTN                       0x80000000U       /*!< CEC Listen mode                    */

/*******************  Bit definition for CEC_TXDR register  *******************/
#define  CEC_TXDR_TXD                        0x000000FFU       /*!< CEC Tx Data                        */

/*******************  Bit definition for CEC_RXDR register  *******************/
#define  CEC_TXDR_RXD                        0x000000FFU       /*!< CEC Rx Data                        */

/*******************  Bit definition for CEC_ISR register  ********************/
#define  CEC_ISR_RXBR                        0x00000001U       /*!< CEC Rx-Byte Received                   */
#define  CEC_ISR_RXEND                       0x00000002U       /*!< CEC End Of Reception                   */
#define  CEC_ISR_RXOVR                       0x00000004U       /*!< CEC Rx-Overrun                         */
#define  CEC_ISR_BRE                         0x00000008U       /*!< CEC Rx Bit Rising Error                */
#define  CEC_ISR_SBPE                        0x00000010U       /*!< CEC Rx Short Bit period Error          */
#define  CEC_ISR_LBPE                        0x00000020U       /*!< CEC Rx Long Bit period Error           */
#define  CEC_ISR_RXACKE                      0x00000040U       /*!< CEC Rx Missing Acknowledge             */
#define  CEC_ISR_ARBLST                      0x00000080U       /*!< CEC Arbitration Lost                   */
#define  CEC_ISR_TXBR                        0x00000100U       /*!< CEC Tx Byte Request                    */
#define  CEC_ISR_TXEND                       0x00000200U       /*!< CEC End of Transmission                */
#define  CEC_ISR_TXUDR                       0x00000400U       /*!< CEC Tx-Buffer Underrun                 */
#define  CEC_ISR_TXERR                       0x00000800U       /*!< CEC Tx-Error                           */
#define  CEC_ISR_TXACKE                      0x00001000U       /*!< CEC Tx Missing Acknowledge             */

/*******************  Bit definition for CEC_IER register  ********************/
#define  CEC_IER_RXBRIE                      0x00000001U       /*!< CEC Rx-Byte Received IT Enable         */
#define  CEC_IER_RXENDIE                     0x00000002U       /*!< CEC End Of Reception IT Enable         */
#define  CEC_IER_RXOVRIE                     0x00000004U       /*!< CEC Rx-Overrun IT Enable               */
#define  CEC_IER_BREIE                       0x00000008U       /*!< CEC Rx Bit Rising Error IT Enable      */
#define  CEC_IER_SBPEIE                      0x00000010U       /*!< CEC Rx Short Bit period Error IT Enable*/
#define  CEC_IER_LBPEIE                      0x00000020U       /*!< CEC Rx Long Bit period Error IT Enable */
#define  CEC_IER_RXACKEIE                    0x00000040U       /*!< CEC Rx Missing Acknowledge IT Enable   */
#define  CEC_IER_ARBLSTIE                    0x00000080U       /*!< CEC Arbitration Lost IT Enable         */
#define  CEC_IER_TXBRIE                      0x00000100U       /*!< CEC Tx Byte Request  IT Enable         */
#define  CEC_IER_TXENDIE                     0x00000200U       /*!< CEC End of Transmission IT Enable      */
#define  CEC_IER_TXUDRIE                     0x00000400U       /*!< CEC Tx-Buffer Underrun IT Enable       */
#define  CEC_IER_TXERRIE                     0x00000800U       /*!< CEC Tx-Error IT Enable                 */
#define  CEC_IER_TXACKEIE                    0x00001000U       /*!< CEC Tx Missing Acknowledge IT Enable   */

/******************************************************************************/
/*                                                                            */
/*                          CRC calculation unit                              */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for CRC_DR register  *********************/
#define  CRC_DR_DR                           0xFFFFFFFFU     /*!< Data register bits */

/*******************  Bit definition for CRC_IDR register  ********************/
#define  CRC_IDR_IDR                         0x000000FFU     /*!< General-purpose 8-bit data register bits */

/********************  Bit definition for CRC_CR register  ********************/
#define  CRC_CR_RESET                        0x00000001U     /*!< RESET the CRC computation unit bit */
#define  CRC_CR_POLYSIZE                     0x00000018U     /*!< Polynomial size bits               */
#define  CRC_CR_POLYSIZE_0                   0x00000008U     /*!< Polynomial size bit 0              */
#define  CRC_CR_POLYSIZE_1                   0x00000010U     /*!< Polynomial size bit 1              */
#define  CRC_CR_REV_IN                       0x00000060U     /*!< REV_IN Reverse Input Data bits     */
#define  CRC_CR_REV_IN_0                     0x00000020U     /*!< Bit 0 */
#define  CRC_CR_REV_IN_1                     0x00000040U     /*!< Bit 1 */
#define  CRC_CR_REV_OUT                      0x00000080U     /*!< REV_OUT Reverse Output Data bits   */

/*******************  Bit definition for CRC_INIT register  *******************/
#define  CRC_INIT_INIT                       0xFFFFFFFFU     /*!< Initial CRC value bits         */

/*******************  Bit definition for CRC_POL register  ********************/
#define  CRC_POL_POL                         0xFFFFFFFFU     /*!< Coefficients of the polynomial */


/******************************************************************************/
/*                                                                            */
/*                      Digital to Analog Converter                           */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for DAC_CR register  ********************/
#define  DAC_CR_EN1                          0x00000001U        /*!<DAC channel1 enable                         */
#define  DAC_CR_BOFF1                        0x00000002U        /*!<DAC channel1 output buffer disable          */
#define  DAC_CR_TEN1                         0x00000004U        /*!<DAC channel1 Trigger enable                 */
#define  DAC_CR_TSEL1                        0x00000038U        /*!<TSEL1[2:0] (DAC channel1 Trigger selection) */
#define  DAC_CR_TSEL1_0                      0x00000008U        /*!<Bit 0 */
#define  DAC_CR_TSEL1_1                      0x00000010U        /*!<Bit 1 */
#define  DAC_CR_TSEL1_2                      0x00000020U        /*!<Bit 2 */
#define  DAC_CR_WAVE1                        0x000000C0U        /*!<WAVE1[1:0] (DAC channel1 noise/triangle wave generation enablEU) */
#define  DAC_CR_WAVE1_0                      0x00000040U        /*!<Bit 0 */
#define  DAC_CR_WAVE1_1                      0x00000080U        /*!<Bit 1 */
#define  DAC_CR_MAMP1                        0x00000F00U        /*!<MAMP1[3:0] (DAC channel1 Mask/Amplitude selector) */
#define  DAC_CR_MAMP1_0                      0x00000100U        /*!<Bit 0 */
#define  DAC_CR_MAMP1_1                      0x00000200U        /*!<Bit 1 */
#define  DAC_CR_MAMP1_2                      0x00000400U        /*!<Bit 2 */
#define  DAC_CR_MAMP1_3                      0x00000800U        /*!<Bit 3 */
#define  DAC_CR_DMAEN1                       0x00001000U        /*!<DAC channel1 DMA enable                     */
#define  DAC_CR_DMAUDRIE1                    0x00002000U        /*!<DAC channel1 DMA underrun interrupt enable  */
#define  DAC_CR_EN2                          0x00010000U        /*!<DAC channel2 enable                         */
#define  DAC_CR_BOFF2                        0x00020000U        /*!<DAC channel2 output buffer disable          */
#define  DAC_CR_TEN2                         0x00040000U        /*!<DAC channel2 Trigger enable                 */
#define  DAC_CR_TSEL2                        0x00380000U        /*!<TSEL2[2:0] (DAC channel2 Trigger selection) */
#define  DAC_CR_TSEL2_0                      0x00080000U        /*!<Bit 0 */
#define  DAC_CR_TSEL2_1                      0x00100000U        /*!<Bit 1 */
#define  DAC_CR_TSEL2_2                      0x00200000U        /*!<Bit 2 */
#define  DAC_CR_WAVE2                        0x00C00000U        /*!<WAVE2[1:0] (DAC channel2 noise/triangle wave generation enable) */
#define  DAC_CR_WAVE2_0                      0x00400000U        /*!<Bit 0 */
#define  DAC_CR_WAVE2_1                      0x00800000U        /*!<Bit 1 */
#define  DAC_CR_MAMP2                        0x0F000000U        /*!<MAMP2[3:0] (DAC channel2 Mask/Amplitude selector) */
#define  DAC_CR_MAMP2_0                      0x01000000U        /*!<Bit 0 */
#define  DAC_CR_MAMP2_1                      0x02000000U        /*!<Bit 1 */
#define  DAC_CR_MAMP2_2                      0x04000000U        /*!<Bit 2 */
#define  DAC_CR_MAMP2_3                      0x08000000U        /*!<Bit 3 */
#define  DAC_CR_DMAEN2                       0x10000000U        /*!<DAC channel2 DMA enable                    */
#define  DAC_CR_DMAUDRIE2                    0x20000000U        /*!<DAC channel2 DMA underrun interrupt enable */

/*****************  Bit definition for DAC_SWTRIGR register  ******************/
#define  DAC_SWTRIGR_SWTRIG1                 0x01U               /*!<DAC channel1 software trigger */
#define  DAC_SWTRIGR_SWTRIG2                 0x02U               /*!<DAC channel2 software trigger */

/*****************  Bit definition for DAC_DHR12R1 register  ******************/
#define  DAC_DHR12R1_DACC1DHR                0x0FFFU            /*!<DAC channel1 12-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12L1 register  ******************/
#define  DAC_DHR12L1_DACC1DHR                0xFFF0U            /*!<DAC channel1 12-bit Left aligned data */

/******************  Bit definition for DAC_DHR8R1 register  ******************/
#define  DAC_DHR8R1_DACC1DHR                 0xFFU               /*!<DAC channel1 8-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12R2 register  ******************/
#define  DAC_DHR12R2_DACC2DHR                0x0FFFU            /*!<DAC channel2 12-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12L2 register  ******************/
#define  DAC_DHR12L2_DACC2DHR                0xFFF0U            /*!<DAC channel2 12-bit Left aligned data */

/******************  Bit definition for DAC_DHR8R2 register  ******************/
#define  DAC_DHR8R2_DACC2DHR                 0xFFU               /*!<DAC channel2 8-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12RD register  ******************/
#define  DAC_DHR12RD_DACC1DHR                0x00000FFFU        /*!<DAC channel1 12-bit Right aligned data */
#define  DAC_DHR12RD_DACC2DHR                0x0FFF0000U        /*!<DAC channel2 12-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12LD register  ******************/
#define  DAC_DHR12LD_DACC1DHR                0x0000FFF0U        /*!<DAC channel1 12-bit Left aligned data */
#define  DAC_DHR12LD_DACC2DHR                0xFFF00000U        /*!<DAC channel2 12-bit Left aligned data */

/******************  Bit definition for DAC_DHR8RD register  ******************/
#define  DAC_DHR8RD_DACC1DHR                 0x00FFU            /*!<DAC channel1 8-bit Right aligned data */
#define  DAC_DHR8RD_DACC2DHR                 0xFF00U            /*!<DAC channel2 8-bit Right aligned data */

/*******************  Bit definition for DAC_DOR1 register  *******************/
#define  DAC_DOR1_DACC1DOR                   0x0FFFU            /*!<DAC channel1 data output */

/*******************  Bit definition for DAC_DOR2 register  *******************/
#define  DAC_DOR2_DACC2DOR                   0x0FFFU            /*!<DAC channel2 data output */

/********************  Bit definition for DAC_SR register  ********************/
#define  DAC_SR_DMAUDR1                      0x00002000U        /*!<DAC channel1 DMA underrun flag */
#define  DAC_SR_DMAUDR2                      0x20000000U        /*!<DAC channel2 DMA underrun flag */


/******************************************************************************/
/*                                                                            */
/*                                 Debug MCU                                  */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*                                    DCMI                                    */
/*                                                                            */
/******************************************************************************/
/********************  Bits definition for DCMI_CR register  ******************/
#define DCMI_CR_CAPTURE                      0x00000001U
#define DCMI_CR_CM                           0x00000002U
#define DCMI_CR_CROP                         0x00000004U
#define DCMI_CR_JPEG                         0x00000008U
#define DCMI_CR_ESS                          0x00000010U
#define DCMI_CR_PCKPOL                       0x00000020U
#define DCMI_CR_HSPOL                        0x00000040U
#define DCMI_CR_VSPOL                        0x00000080U
#define DCMI_CR_FCRC_0                       0x00000100U
#define DCMI_CR_FCRC_1                       0x00000200U
#define DCMI_CR_EDM_0                        0x00000400U
#define DCMI_CR_EDM_1                        0x00000800U
#define DCMI_CR_CRE                          0x00001000U
#define DCMI_CR_ENABLE                       0x00004000U
#define DCMI_CR_BSM                          0x00030000U
#define DCMI_CR_BSM_0                        0x00010000U
#define DCMI_CR_BSM_1                        0x00020000U
#define DCMI_CR_OEBS                         0x00040000U
#define DCMI_CR_LSM                          0x00080000U
#define DCMI_CR_OELS                         0x00100000U

/********************  Bits definition for DCMI_SR register  ******************/
#define DCMI_SR_HSYNC                        0x00000001U
#define DCMI_SR_VSYNC                        0x00000002U
#define DCMI_SR_FNE                          0x00000004U

/********************  Bits definition for DCMI_RIS register   ****************/
#define DCMI_RIS_FRAME_RIS                   0x00000001U
#define DCMI_RIS_OVR_RIS                     0x00000002U
#define DCMI_RIS_ERR_RIS                     0x00000004U
#define DCMI_RIS_VSYNC_RIS                   0x00000008U
#define DCMI_RIS_LINE_RIS                    0x00000010U

/* Legacy defines */
#define DCMI_RISR_FRAME_RIS                  DCMI_RIS_FRAME_RIS
#define DCMI_RISR_OVF_RIS                    DCMI_RIS_OVR_RIS
#define DCMI_RISR_ERR_RIS                    DCMI_RIS_ERR_RIS
#define DCMI_RISR_VSYNC_RIS                  DCMI_RIS_VSYNC_RIS
#define DCMI_RISR_LINE_RIS                   DCMI_RIS_LINE_RIS

/********************  Bits definition for DCMI_IER register  *****************/
#define DCMI_IER_FRAME_IE                    0x00000001U
#define DCMI_IER_OVR_IE                      0x00000002U
#define DCMI_IER_ERR_IE                      0x00000004U
#define DCMI_IER_VSYNC_IE                    0x00000008U
#define DCMI_IER_LINE_IE                     0x00000010U

/* Legacy define */
#define DCMI_IER_OVF_IE                      DCMI_IER_OVR_IE

/********************  Bits definition for DCMI_MIS register  *****************/
#define DCMI_MIS_FRAME_MIS                   0x00000001U
#define DCMI_MIS_OVR_MIS                     0x00000002U
#define DCMI_MIS_ERR_MIS                     0x00000004U
#define DCMI_MIS_VSYNC_MIS                   0x00000008U
#define DCMI_MIS_LINE_MIS                    0x00000010U

/* Legacy defines */
#define DCMI_MISR_FRAME_MIS                  DCMI_MIS_FRAME_MIS
#define DCMI_MISR_OVF_MIS                    DCMI_MIS_OVR_MIS
#define DCMI_MISR_ERR_MIS                    DCMI_MIS_ERR_MIS
#define DCMI_MISR_VSYNC_MIS                  DCMI_MIS_VSYNC_MIS
#define DCMI_MISR_LINE_MIS                   DCMI_MIS_LINE_MIS

/********************  Bits definition for DCMI_ICR register  *****************/
#define DCMI_ICR_FRAME_ISC                   0x00000001U
#define DCMI_ICR_OVR_ISC                     0x00000002U
#define DCMI_ICR_ERR_ISC                     0x00000004U
#define DCMI_ICR_VSYNC_ISC                   0x00000008U
#define DCMI_ICR_LINE_ISC                    0x00000010U

/* Legacy defines */
#define DCMI_ICR_OVF_ISC                     DCMI_ICR_OVR_ISC

/********************  Bits definition for DCMI_ESCR register  ******************/
#define DCMI_ESCR_FSC                        0x000000FFU
#define DCMI_ESCR_LSC                        0x0000FF00U
#define DCMI_ESCR_LEC                        0x00FF0000U
#define DCMI_ESCR_FEC                        0xFF000000U

/********************  Bits definition for DCMI_ESUR register  ******************/
#define DCMI_ESUR_FSU                        0x000000FFU
#define DCMI_ESUR_LSU                        0x0000FF00U
#define DCMI_ESUR_LEU                        0x00FF0000U
#define DCMI_ESUR_FEU                        0xFF000000U

/********************  Bits definition for DCMI_CWSTRT register  ******************/
#define DCMI_CWSTRT_HOFFCNT                  0x00003FFFU
#define DCMI_CWSTRT_VST                      0x1FFF0000U

/********************  Bits definition for DCMI_CWSIZE register  ******************/
#define DCMI_CWSIZE_CAPCNT                   0x00003FFFU
#define DCMI_CWSIZE_VLINE                    0x3FFF0000U

/********************  Bits definition for DCMI_DR register  ******************/
#define DCMI_DR_BYTE0                        0x000000FFU
#define DCMI_DR_BYTE1                        0x0000FF00U
#define DCMI_DR_BYTE2                        0x00FF0000U
#define DCMI_DR_BYTE3                        0xFF000000U

/******************************************************************************/
/*                                                                            */
/*                             DMA Controller                                 */
/*                                                                            */
/******************************************************************************/
/********************  Bits definition for DMA_SxCR register  *****************/
#define DMA_SxCR_CHSEL                       0x0E000000U
#define DMA_SxCR_CHSEL_0                     0x02000000U
#define DMA_SxCR_CHSEL_1                     0x04000000U
#define DMA_SxCR_CHSEL_2                     0x08000000U
#define DMA_SxCR_MBURST                      0x01800000U
#define DMA_SxCR_MBURST_0                    0x00800000U
#define DMA_SxCR_MBURST_1                    0x01000000U
#define DMA_SxCR_PBURST                      0x00600000U
#define DMA_SxCR_PBURST_0                    0x00200000U
#define DMA_SxCR_PBURST_1                    0x00400000U
#define DMA_SxCR_CT                          0x00080000U
#define DMA_SxCR_DBM                         0x00040000U
#define DMA_SxCR_PL                          0x00030000U
#define DMA_SxCR_PL_0                        0x00010000U
#define DMA_SxCR_PL_1                        0x00020000U
#define DMA_SxCR_PINCOS                      0x00008000U
#define DMA_SxCR_MSIZE                       0x00006000U
#define DMA_SxCR_MSIZE_0                     0x00002000U
#define DMA_SxCR_MSIZE_1                     0x00004000U
#define DMA_SxCR_PSIZE                       0x00001800U
#define DMA_SxCR_PSIZE_0                     0x00000800U
#define DMA_SxCR_PSIZE_1                     0x00001000U
#define DMA_SxCR_MINC                        0x00000400U
#define DMA_SxCR_PINC                        0x00000200U
#define DMA_SxCR_CIRC                        0x00000100U
#define DMA_SxCR_DIR                         0x000000C0U
#define DMA_SxCR_DIR_0                       0x00000040U
#define DMA_SxCR_DIR_1                       0x00000080U
#define DMA_SxCR_PFCTRL                      0x00000020U
#define DMA_SxCR_TCIE                        0x00000010U
#define DMA_SxCR_HTIE                        0x00000008U
#define DMA_SxCR_TEIE                        0x00000004U
#define DMA_SxCR_DMEIE                       0x00000002U
#define DMA_SxCR_EN                          0x00000001U

/********************  Bits definition for DMA_SxCNDTR register  **************/
#define DMA_SxNDT                            0x0000FFFFU
#define DMA_SxNDT_0                          0x00000001U
#define DMA_SxNDT_1                          0x00000002U
#define DMA_SxNDT_2                          0x00000004U
#define DMA_SxNDT_3                          0x00000008U
#define DMA_SxNDT_4                          0x00000010U
#define DMA_SxNDT_5                          0x00000020U
#define DMA_SxNDT_6                          0x00000040U
#define DMA_SxNDT_7                          0x00000080U
#define DMA_SxNDT_8                          0x00000100U
#define DMA_SxNDT_9                          0x00000200U
#define DMA_SxNDT_10                         0x00000400U
#define DMA_SxNDT_11                         0x00000800U
#define DMA_SxNDT_12                         0x00001000U
#define DMA_SxNDT_13                         0x00002000U
#define DMA_SxNDT_14                         0x00004000U
#define DMA_SxNDT_15                         0x00008000U

/********************  Bits definition for DMA_SxFCR register  ****************/
#define DMA_SxFCR_FEIE                       0x00000080U
#define DMA_SxFCR_FS                         0x00000038U
#define DMA_SxFCR_FS_0                       0x00000008U
#define DMA_SxFCR_FS_1                       0x00000010U
#define DMA_SxFCR_FS_2                       0x00000020U
#define DMA_SxFCR_DMDIS                      0x00000004U
#define DMA_SxFCR_FTH                        0x00000003U
#define DMA_SxFCR_FTH_0                      0x00000001U
#define DMA_SxFCR_FTH_1                      0x00000002U

/********************  Bits definition for DMA_LISR register  *****************/
#define DMA_LISR_TCIF3                       0x08000000U
#define DMA_LISR_HTIF3                       0x04000000U
#define DMA_LISR_TEIF3                       0x02000000U
#define DMA_LISR_DMEIF3                      0x01000000U
#define DMA_LISR_FEIF3                       0x00400000U
#define DMA_LISR_TCIF2                       0x00200000U
#define DMA_LISR_HTIF2                       0x00100000U
#define DMA_LISR_TEIF2                       0x00080000U
#define DMA_LISR_DMEIF2                      0x00040000U
#define DMA_LISR_FEIF2                       0x00010000U
#define DMA_LISR_TCIF1                       0x00000800U
#define DMA_LISR_HTIF1                       0x00000400U
#define DMA_LISR_TEIF1                       0x00000200U
#define DMA_LISR_DMEIF1                      0x00000100U
#define DMA_LISR_FEIF1                       0x00000040U
#define DMA_LISR_TCIF0                       0x00000020U
#define DMA_LISR_HTIF0                       0x00000010U
#define DMA_LISR_TEIF0                       0x00000008U
#define DMA_LISR_DMEIF0                      0x00000004U
#define DMA_LISR_FEIF0                       0x00000001U

/********************  Bits definition for DMA_HISR register  *****************/
#define DMA_HISR_TCIF7                       0x08000000U
#define DMA_HISR_HTIF7                       0x04000000U
#define DMA_HISR_TEIF7                       0x02000000U
#define DMA_HISR_DMEIF7                      0x01000000U
#define DMA_HISR_FEIF7                       0x00400000U
#define DMA_HISR_TCIF6                       0x00200000U
#define DMA_HISR_HTIF6                       0x00100000U
#define DMA_HISR_TEIF6                       0x00080000U
#define DMA_HISR_DMEIF6                      0x00040000U
#define DMA_HISR_FEIF6                       0x00010000U
#define DMA_HISR_TCIF5                       0x00000800U
#define DMA_HISR_HTIF5                       0x00000400U
#define DMA_HISR_TEIF5                       0x00000200U
#define DMA_HISR_DMEIF5                      0x00000100U
#define DMA_HISR_FEIF5                       0x00000040U
#define DMA_HISR_TCIF4                       0x00000020U
#define DMA_HISR_HTIF4                       0x00000010U
#define DMA_HISR_TEIF4                       0x00000008U
#define DMA_HISR_DMEIF4                      0x00000004U
#define DMA_HISR_FEIF4                       0x00000001U

/********************  Bits definition for DMA_LIFCR register  ****************/
#define DMA_LIFCR_CTCIF3                     0x08000000U
#define DMA_LIFCR_CHTIF3                     0x04000000U
#define DMA_LIFCR_CTEIF3                     0x02000000U
#define DMA_LIFCR_CDMEIF3                    0x01000000U
#define DMA_LIFCR_CFEIF3                     0x00400000U
#define DMA_LIFCR_CTCIF2                     0x00200000U
#define DMA_LIFCR_CHTIF2                     0x00100000U
#define DMA_LIFCR_CTEIF2                     0x00080000U
#define DMA_LIFCR_CDMEIF2                    0x00040000U
#define DMA_LIFCR_CFEIF2                     0x00010000U
#define DMA_LIFCR_CTCIF1                     0x00000800U
#define DMA_LIFCR_CHTIF1                     0x00000400U
#define DMA_LIFCR_CTEIF1                     0x00000200U
#define DMA_LIFCR_CDMEIF1                    0x00000100U
#define DMA_LIFCR_CFEIF1                     0x00000040U
#define DMA_LIFCR_CTCIF0                     0x00000020U
#define DMA_LIFCR_CHTIF0                     0x00000010U
#define DMA_LIFCR_CTEIF0                     0x00000008U
#define DMA_LIFCR_CDMEIF0                    0x00000004U
#define DMA_LIFCR_CFEIF0                     0x00000001U

/********************  Bits definition for DMA_HIFCR  register  ****************/
#define DMA_HIFCR_CTCIF7                     0x08000000U
#define DMA_HIFCR_CHTIF7                     0x04000000U
#define DMA_HIFCR_CTEIF7                     0x02000000U
#define DMA_HIFCR_CDMEIF7                    0x01000000U
#define DMA_HIFCR_CFEIF7                     0x00400000U
#define DMA_HIFCR_CTCIF6                     0x00200000U
#define DMA_HIFCR_CHTIF6                     0x00100000U
#define DMA_HIFCR_CTEIF6                     0x00080000U
#define DMA_HIFCR_CDMEIF6                    0x00040000U
#define DMA_HIFCR_CFEIF6                     0x00010000U
#define DMA_HIFCR_CTCIF5                     0x00000800U
#define DMA_HIFCR_CHTIF5                     0x00000400U
#define DMA_HIFCR_CTEIF5                     0x00000200U
#define DMA_HIFCR_CDMEIF5                    0x00000100U
#define DMA_HIFCR_CFEIF5                     0x00000040U
#define DMA_HIFCR_CTCIF4                     0x00000020U
#define DMA_HIFCR_CHTIF4                     0x00000010U
#define DMA_HIFCR_CTEIF4                     0x00000008U
#define DMA_HIFCR_CDMEIF4                    0x00000004U
#define DMA_HIFCR_CFEIF4                     0x00000001U

/******************************************************************************/
/*                                                                            */
/*                         AHB Master DMA2D Controller (DMA2D)                */
/*                                                                            */
/******************************************************************************/

/********************  Bit definition for DMA2D_CR register  ******************/

#define DMA2D_CR_START                     0x00000001U               /*!< Start transfer                          */
#define DMA2D_CR_SUSP                      0x00000002U               /*!< Suspend transfer                        */
#define DMA2D_CR_ABORT                     0x00000004U               /*!< Abort transfer                          */
#define DMA2D_CR_TEIE                      0x00000100U               /*!< Transfer Error Interrupt Enable         */
#define DMA2D_CR_TCIE                      0x00000200U               /*!< Transfer Complete Interrupt Enable      */
#define DMA2D_CR_TWIE                      0x00000400U               /*!< Transfer Watermark Interrupt Enable     */
#define DMA2D_CR_CAEIE                     0x00000800U               /*!< CLUT Access Error Interrupt Enable      */
#define DMA2D_CR_CTCIE                     0x00001000U               /*!< CLUT Transfer Complete Interrupt Enable */
#define DMA2D_CR_CEIE                      0x00002000U               /*!< Configuration Error Interrupt Enable    */
#define DMA2D_CR_MODE                      0x00030000U               /*!< DMA2D Mode[1:0]                         */
#define DMA2D_CR_MODE_0                    0x00010000U               /*!< DMA2D Mode bit 0                        */
#define DMA2D_CR_MODE_1                    0x00020000U               /*!< DMA2D Mode bit 1                        */

/********************  Bit definition for DMA2D_ISR register  *****************/

#define DMA2D_ISR_TEIF                     0x00000001U               /*!< Transfer Error Interrupt Flag         */
#define DMA2D_ISR_TCIF                     0x00000002U               /*!< Transfer Complete Interrupt Flag      */
#define DMA2D_ISR_TWIF                     0x00000004U               /*!< Transfer Watermark Interrupt Flag     */
#define DMA2D_ISR_CAEIF                    0x00000008U               /*!< CLUT Access Error Interrupt Flag      */
#define DMA2D_ISR_CTCIF                    0x00000010U               /*!< CLUT Transfer Complete Interrupt Flag */
#define DMA2D_ISR_CEIF                     0x00000020U               /*!< Configuration Error Interrupt Flag    */

/********************  Bit definition for DMA2D_IFCR register  ****************/

#define DMA2D_IFCR_CTEIF                   0x00000001U               /*!< Clears Transfer Error Interrupt Flag         */
#define DMA2D_IFCR_CTCIF                   0x00000002U               /*!< Clears Transfer Complete Interrupt Flag      */
#define DMA2D_IFCR_CTWIF                   0x00000004U               /*!< Clears Transfer Watermark Interrupt Flag     */
#define DMA2D_IFCR_CAECIF                  0x00000008U               /*!< Clears CLUT Access Error Interrupt Flag      */
#define DMA2D_IFCR_CCTCIF                  0x00000010U               /*!< Clears CLUT Transfer Complete Interrupt Flag */
#define DMA2D_IFCR_CCEIF                   0x00000020U               /*!< Clears Configuration Error Interrupt Flag    */

/* Legacy defines */
#define DMA2D_IFSR_CTEIF                   DMA2D_IFCR_CTEIF                     /*!< Clears Transfer Error Interrupt Flag         */
#define DMA2D_IFSR_CTCIF                   DMA2D_IFCR_CTCIF                     /*!< Clears Transfer Complete Interrupt Flag      */
#define DMA2D_IFSR_CTWIF                   DMA2D_IFCR_CTWIF                     /*!< Clears Transfer Watermark Interrupt Flag     */
#define DMA2D_IFSR_CCAEIF                  DMA2D_IFCR_CAECIF                    /*!< Clears CLUT Access Error Interrupt Flag      */
#define DMA2D_IFSR_CCTCIF                  DMA2D_IFCR_CCTCIF                    /*!< Clears CLUT Transfer Complete Interrupt Flag */
#define DMA2D_IFSR_CCEIF                   DMA2D_IFCR_CCEIF                     /*!< Clears Configuration Error Interrupt Flag    */

/********************  Bit definition for DMA2D_FGMAR register  ***************/

#define DMA2D_FGMAR_MA                     0xFFFFFFFFU               /*!< Memory Address */

/********************  Bit definition for DMA2D_FGOR register  ****************/

#define DMA2D_FGOR_LO                      0x00003FFFU               /*!< Line Offset */

/********************  Bit definition for DMA2D_BGMAR register  ***************/

#define DMA2D_BGMAR_MA                     0xFFFFFFFFU               /*!< Memory Address */

/********************  Bit definition for DMA2D_BGOR register  ****************/

#define DMA2D_BGOR_LO                      0x00003FFFU               /*!< Line Offset */

/********************  Bit definition for DMA2D_FGPFCCR register  *************/

#define DMA2D_FGPFCCR_CM                   0x0000000FU               /*!< Input color mode CM[3:0] */
#define DMA2D_FGPFCCR_CM_0                 0x00000001U               /*!< Input color mode CM bit 0 */
#define DMA2D_FGPFCCR_CM_1                 0x00000002U               /*!< Input color mode CM bit 1 */
#define DMA2D_FGPFCCR_CM_2                 0x00000004U               /*!< Input color mode CM bit 2 */
#define DMA2D_FGPFCCR_CM_3                 0x00000008U               /*!< Input color mode CM bit 3 */
#define DMA2D_FGPFCCR_CCM                  0x00000010U               /*!< CLUT Color mode */
#define DMA2D_FGPFCCR_START                0x00000020U               /*!< Start */
#define DMA2D_FGPFCCR_CS                   0x0000FF00U               /*!< CLUT size */
#define DMA2D_FGPFCCR_AM                   0x00030000U               /*!< Alpha mode AM[1:0] */
#define DMA2D_FGPFCCR_AM_0                 0x00010000U               /*!< Alpha mode AM bit 0 */
#define DMA2D_FGPFCCR_AM_1                 0x00020000U               /*!< Alpha mode AM bit 1 */
#define DMA2D_FGPFCCR_ALPHA                0xFF000000U               /*!< Alpha value */

/********************  Bit definition for DMA2D_FGCOLR register  **************/

#define DMA2D_FGCOLR_BLUE                  0x000000FFU               /*!< Blue Value */
#define DMA2D_FGCOLR_GREEN                 0x0000FF00U               /*!< Green Value */
#define DMA2D_FGCOLR_RED                   0x00FF0000U               /*!< Red Value */

/********************  Bit definition for DMA2D_BGPFCCR register  *************/

#define DMA2D_BGPFCCR_CM                   0x0000000FU               /*!< Input color mode CM[3:0] */
#define DMA2D_BGPFCCR_CM_0                 0x00000001U               /*!< Input color mode CM bit 0 */
#define DMA2D_BGPFCCR_CM_1                 0x00000002U               /*!< Input color mode CM bit 1 */
#define DMA2D_BGPFCCR_CM_2                 0x00000004U               /*!< Input color mode CM bit 2 */
#define DMA2D_FGPFCCR_CM_3                 0x00000008U               /*!< Input color mode CM bit 3 */
#define DMA2D_BGPFCCR_CCM                  0x00000010U               /*!< CLUT Color mode */
#define DMA2D_BGPFCCR_START                0x00000020U               /*!< Start */
#define DMA2D_BGPFCCR_CS                   0x0000FF00U               /*!< CLUT size */
#define DMA2D_BGPFCCR_AM                   0x00030000U               /*!< Alpha mode AM[1:0] */
#define DMA2D_BGPFCCR_AM_0                 0x00010000U               /*!< Alpha mode AM bit 0 */
#define DMA2D_BGPFCCR_AM_1                 0x00020000U               /*!< Alpha mode AM bit 1 */
#define DMA2D_BGPFCCR_ALPHA                0xFF000000U               /*!< background Input Alpha value */

/********************  Bit definition for DMA2D_BGCOLR register  **************/

#define DMA2D_BGCOLR_BLUE                  0x000000FFU               /*!< Blue Value */
#define DMA2D_BGCOLR_GREEN                 0x0000FF00U               /*!< Green Value */
#define DMA2D_BGCOLR_RED                   0x00FF0000U               /*!< Red Value */

/********************  Bit definition for DMA2D_FGCMAR register  **************/

#define DMA2D_FGCMAR_MA                    0xFFFFFFFFU               /*!< Memory Address */

/********************  Bit definition for DMA2D_BGCMAR register  **************/

#define DMA2D_BGCMAR_MA                    0xFFFFFFFFU               /*!< Memory Address */

/********************  Bit definition for DMA2D_OPFCCR register  **************/

#define DMA2D_OPFCCR_CM                    0x00000007U               /*!< Color mode CM[2:0] */
#define DMA2D_OPFCCR_CM_0                  0x00000001U               /*!< Color mode CM bit 0 */
#define DMA2D_OPFCCR_CM_1                  0x00000002U               /*!< Color mode CM bit 1 */
#define DMA2D_OPFCCR_CM_2                  0x00000004U               /*!< Color mode CM bit 2 */

/********************  Bit definition for DMA2D_OCOLR register  ***************/

/*!<Mode_ARGB8888/RGB888 */

#define DMA2D_OCOLR_BLUE_1                 0x000000FFU               /*!< BLUE Value */
#define DMA2D_OCOLR_GREEN_1                0x0000FF00U               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_1                  0x00FF0000U               /*!< Red Value */
#define DMA2D_OCOLR_ALPHA_1                0xFF000000U               /*!< Alpha Channel Value */

/*!<Mode_RGB565 */
#define DMA2D_OCOLR_BLUE_2                 0x0000001FU               /*!< BLUE Value */
#define DMA2D_OCOLR_GREEN_2                0x000007E0U               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_2                  0x0000F800U               /*!< Red Value */

/*!<Mode_ARGB1555 */
#define DMA2D_OCOLR_BLUE_3                 0x0000001FU               /*!< BLUE Value */
#define DMA2D_OCOLR_GREEN_3                0x000003E0U               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_3                  0x00007C00U               /*!< Red Value */
#define DMA2D_OCOLR_ALPHA_3                0x00008000U               /*!< Alpha Channel Value */

/*!<Mode_ARGB4444 */
#define DMA2D_OCOLR_BLUE_4                 0x0000000FU               /*!< BLUE Value */
#define DMA2D_OCOLR_GREEN_4                0x000000F0U               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_4                  0x00000F00U               /*!< Red Value */
#define DMA2D_OCOLR_ALPHA_4                0x0000F000U               /*!< Alpha Channel Value */

/********************  Bit definition for DMA2D_OMAR register  ****************/

#define DMA2D_OMAR_MA                      0xFFFFFFFFU               /*!< Memory Address */

/********************  Bit definition for DMA2D_OOR register  *****************/

#define DMA2D_OOR_LO                       0x00003FFFU               /*!< Line Offset */

/********************  Bit definition for DMA2D_NLR register  *****************/

#define DMA2D_NLR_NL                       0x0000FFFFU               /*!< Number of Lines */
#define DMA2D_NLR_PL                       0x3FFF0000U               /*!< Pixel per Lines */

/********************  Bit definition for DMA2D_LWR register  *****************/

#define DMA2D_LWR_LW                       0x0000FFFFU               /*!< Line Watermark */

/********************  Bit definition for DMA2D_AMTCR register  ***************/

#define DMA2D_AMTCR_EN                     0x00000001U               /*!< Enable */
#define DMA2D_AMTCR_DT                     0x0000FF00U               /*!< Dead Time */


/********************  Bit definition for DMA2D_FGCLUT register  **************/

/********************  Bit definition for DMA2D_BGCLUT register  **************/

/******************************************************************************/
/*                                                                            */
/*                    External Interrupt/Event Controller                     */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for EXTI_IMR register  *******************/
#define  EXTI_IMR_MR0                        0x00000001U        /*!< Interrupt Mask on line 0 */
#define  EXTI_IMR_MR1                        0x00000002U        /*!< Interrupt Mask on line 1 */
#define  EXTI_IMR_MR2                        0x00000004U        /*!< Interrupt Mask on line 2 */
#define  EXTI_IMR_MR3                        0x00000008U        /*!< Interrupt Mask on line 3 */
#define  EXTI_IMR_MR4                        0x00000010U        /*!< Interrupt Mask on line 4 */
#define  EXTI_IMR_MR5                        0x00000020U        /*!< Interrupt Mask on line 5 */
#define  EXTI_IMR_MR6                        0x00000040U        /*!< Interrupt Mask on line 6 */
#define  EXTI_IMR_MR7                        0x00000080U        /*!< Interrupt Mask on line 7 */
#define  EXTI_IMR_MR8                        0x00000100U        /*!< Interrupt Mask on line 8 */
#define  EXTI_IMR_MR9                        0x00000200U        /*!< Interrupt Mask on line 9 */
#define  EXTI_IMR_MR10                       0x00000400U        /*!< Interrupt Mask on line 10 */
#define  EXTI_IMR_MR11                       0x00000800U        /*!< Interrupt Mask on line 11 */
#define  EXTI_IMR_MR12                       0x00001000U        /*!< Interrupt Mask on line 12 */
#define  EXTI_IMR_MR13                       0x00002000U        /*!< Interrupt Mask on line 13 */
#define  EXTI_IMR_MR14                       0x00004000U        /*!< Interrupt Mask on line 14 */
#define  EXTI_IMR_MR15                       0x00008000U        /*!< Interrupt Mask on line 15 */
#define  EXTI_IMR_MR16                       0x00010000U        /*!< Interrupt Mask on line 16 */
#define  EXTI_IMR_MR17                       0x00020000U        /*!< Interrupt Mask on line 17 */
#define  EXTI_IMR_MR18                       0x00040000U        /*!< Interrupt Mask on line 18 */
#define  EXTI_IMR_MR19                       0x00080000U        /*!< Interrupt Mask on line 19 */
#define  EXTI_IMR_MR20                       0x00100000U        /*!< Interrupt Mask on line 20 */
#define  EXTI_IMR_MR21                       0x00200000U        /*!< Interrupt Mask on line 21 */
#define  EXTI_IMR_MR22                       0x00400000U        /*!< Interrupt Mask on line 22 */
#define  EXTI_IMR_MR23                       0x00800000U        /*!< Interrupt Mask on line 23 */

/* Reference Defines */
#define  EXTI_IMR_IM0                        EXTI_IMR_MR0
#define  EXTI_IMR_IM1                        EXTI_IMR_MR1
#define  EXTI_IMR_IM2                        EXTI_IMR_MR2
#define  EXTI_IMR_IM3                        EXTI_IMR_MR3
#define  EXTI_IMR_IM4                        EXTI_IMR_MR4
#define  EXTI_IMR_IM5                        EXTI_IMR_MR5
#define  EXTI_IMR_IM6                        EXTI_IMR_MR6
#define  EXTI_IMR_IM7                        EXTI_IMR_MR7
#define  EXTI_IMR_IM8                        EXTI_IMR_MR8
#define  EXTI_IMR_IM9                        EXTI_IMR_MR9
#define  EXTI_IMR_IM10                       EXTI_IMR_MR10
#define  EXTI_IMR_IM11                       EXTI_IMR_MR11
#define  EXTI_IMR_IM12                       EXTI_IMR_MR12
#define  EXTI_IMR_IM13                       EXTI_IMR_MR13
#define  EXTI_IMR_IM14                       EXTI_IMR_MR14
#define  EXTI_IMR_IM15                       EXTI_IMR_MR15
#define  EXTI_IMR_IM16                       EXTI_IMR_MR16
#define  EXTI_IMR_IM17                       EXTI_IMR_MR17
#define  EXTI_IMR_IM18                       EXTI_IMR_MR18
#define  EXTI_IMR_IM19                       EXTI_IMR_MR19
#define  EXTI_IMR_IM20                       EXTI_IMR_MR20
#define  EXTI_IMR_IM21                       EXTI_IMR_MR21
#define  EXTI_IMR_IM22                       EXTI_IMR_MR22
#define  EXTI_IMR_IM23                       EXTI_IMR_MR23

#define  EXTI_IMR_IM                         0x00FFFFFFU        /*!< Interrupt Mask All */

/*******************  Bit definition for EXTI_EMR register  *******************/
#define  EXTI_EMR_MR0                        0x00000001U        /*!< Event Mask on line 0 */
#define  EXTI_EMR_MR1                        0x00000002U        /*!< Event Mask on line 1 */
#define  EXTI_EMR_MR2                        0x00000004U        /*!< Event Mask on line 2 */
#define  EXTI_EMR_MR3                        0x00000008U        /*!< Event Mask on line 3 */
#define  EXTI_EMR_MR4                        0x00000010U        /*!< Event Mask on line 4 */
#define  EXTI_EMR_MR5                        0x00000020U        /*!< Event Mask on line 5 */
#define  EXTI_EMR_MR6                        0x00000040U        /*!< Event Mask on line 6 */
#define  EXTI_EMR_MR7                        0x00000080U        /*!< Event Mask on line 7 */
#define  EXTI_EMR_MR8                        0x00000100U        /*!< Event Mask on line 8 */
#define  EXTI_EMR_MR9                        0x00000200U        /*!< Event Mask on line 9 */
#define  EXTI_EMR_MR10                       0x00000400U        /*!< Event Mask on line 10 */
#define  EXTI_EMR_MR11                       0x00000800U        /*!< Event Mask on line 11 */
#define  EXTI_EMR_MR12                       0x00001000U        /*!< Event Mask on line 12 */
#define  EXTI_EMR_MR13                       0x00002000U        /*!< Event Mask on line 13 */
#define  EXTI_EMR_MR14                       0x00004000U        /*!< Event Mask on line 14 */
#define  EXTI_EMR_MR15                       0x00008000U        /*!< Event Mask on line 15 */
#define  EXTI_EMR_MR16                       0x00010000U        /*!< Event Mask on line 16 */
#define  EXTI_EMR_MR17                       0x00020000U        /*!< Event Mask on line 17 */
#define  EXTI_EMR_MR18                       0x00040000U        /*!< Event Mask on line 18 */
#define  EXTI_EMR_MR19                       0x00080000U        /*!< Event Mask on line 19 */
#define  EXTI_EMR_MR20                       0x00100000U        /*!< Event Mask on line 20 */
#define  EXTI_EMR_MR21                       0x00200000U        /*!< Event Mask on line 21 */
#define  EXTI_EMR_MR22                       0x00400000U        /*!< Event Mask on line 22 */
#define  EXTI_EMR_MR23                       0x00800000U        /*!< Event Mask on line 23 */

/* Reference Defines */
#define  EXTI_EMR_EM0                        EXTI_EMR_MR0
#define  EXTI_EMR_EM1                        EXTI_EMR_MR1
#define  EXTI_EMR_EM2                        EXTI_EMR_MR2
#define  EXTI_EMR_EM3                        EXTI_EMR_MR3
#define  EXTI_EMR_EM4                        EXTI_EMR_MR4
#define  EXTI_EMR_EM5                        EXTI_EMR_MR5
#define  EXTI_EMR_EM6                        EXTI_EMR_MR6
#define  EXTI_EMR_EM7                        EXTI_EMR_MR7
#define  EXTI_EMR_EM8                        EXTI_EMR_MR8
#define  EXTI_EMR_EM9                        EXTI_EMR_MR9
#define  EXTI_EMR_EM10                       EXTI_EMR_MR10
#define  EXTI_EMR_EM11                       EXTI_EMR_MR11
#define  EXTI_EMR_EM12                       EXTI_EMR_MR12
#define  EXTI_EMR_EM13                       EXTI_EMR_MR13
#define  EXTI_EMR_EM14                       EXTI_EMR_MR14
#define  EXTI_EMR_EM15                       EXTI_EMR_MR15
#define  EXTI_EMR_EM16                       EXTI_EMR_MR16
#define  EXTI_EMR_EM17                       EXTI_EMR_MR17
#define  EXTI_EMR_EM18                       EXTI_EMR_MR18
#define  EXTI_EMR_EM19                       EXTI_EMR_MR19
#define  EXTI_EMR_EM20                       EXTI_EMR_MR20
#define  EXTI_EMR_EM21                       EXTI_EMR_MR21
#define  EXTI_EMR_EM22                       EXTI_EMR_MR22
#define  EXTI_EMR_EM23                       EXTI_EMR_MR23


/******************  Bit definition for EXTI_RTSR register  *******************/
#define  EXTI_RTSR_TR0                       0x00000001U        /*!< Rising trigger event configuration bit of line 0 */
#define  EXTI_RTSR_TR1                       0x00000002U        /*!< Rising trigger event configuration bit of line 1 */
#define  EXTI_RTSR_TR2                       0x00000004U        /*!< Rising trigger event configuration bit of line 2 */
#define  EXTI_RTSR_TR3                       0x00000008U        /*!< Rising trigger event configuration bit of line 3 */
#define  EXTI_RTSR_TR4                       0x00000010U        /*!< Rising trigger event configuration bit of line 4 */
#define  EXTI_RTSR_TR5                       0x00000020U        /*!< Rising trigger event configuration bit of line 5 */
#define  EXTI_RTSR_TR6                       0x00000040U        /*!< Rising trigger event configuration bit of line 6 */
#define  EXTI_RTSR_TR7                       0x00000080U        /*!< Rising trigger event configuration bit of line 7 */
#define  EXTI_RTSR_TR8                       0x00000100U        /*!< Rising trigger event configuration bit of line 8 */
#define  EXTI_RTSR_TR9                       0x00000200U        /*!< Rising trigger event configuration bit of line 9 */
#define  EXTI_RTSR_TR10                      0x00000400U        /*!< Rising trigger event configuration bit of line 10 */
#define  EXTI_RTSR_TR11                      0x00000800U        /*!< Rising trigger event configuration bit of line 11 */
#define  EXTI_RTSR_TR12                      0x00001000U        /*!< Rising trigger event configuration bit of line 12 */
#define  EXTI_RTSR_TR13                      0x00002000U        /*!< Rising trigger event configuration bit of line 13 */
#define  EXTI_RTSR_TR14                      0x00004000U        /*!< Rising trigger event configuration bit of line 14 */
#define  EXTI_RTSR_TR15                      0x00008000U        /*!< Rising trigger event configuration bit of line 15 */
#define  EXTI_RTSR_TR16                      0x00010000U        /*!< Rising trigger event configuration bit of line 16 */
#define  EXTI_RTSR_TR17                      0x00020000U        /*!< Rising trigger event configuration bit of line 17 */
#define  EXTI_RTSR_TR18                      0x00040000U        /*!< Rising trigger event configuration bit of line 18 */
#define  EXTI_RTSR_TR19                      0x00080000U        /*!< Rising trigger event configuration bit of line 19 */
#define  EXTI_RTSR_TR20                      0x00100000U        /*!< Rising trigger event configuration bit of line 20 */
#define  EXTI_RTSR_TR21                      0x00200000U        /*!< Rising trigger event configuration bit of line 21 */
#define  EXTI_RTSR_TR22                      0x00400000U        /*!< Rising trigger event configuration bit of line 22 */
#define  EXTI_RTSR_TR23                      0x00800000U        /*!< Rising trigger event configuration bit of line 23 */

/******************  Bit definition for EXTI_FTSR register  *******************/
#define  EXTI_FTSR_TR0                       0x00000001U        /*!< Falling trigger event configuration bit of line 0 */
#define  EXTI_FTSR_TR1                       0x00000002U        /*!< Falling trigger event configuration bit of line 1 */
#define  EXTI_FTSR_TR2                       0x00000004U        /*!< Falling trigger event configuration bit of line 2 */
#define  EXTI_FTSR_TR3                       0x00000008U        /*!< Falling trigger event configuration bit of line 3 */
#define  EXTI_FTSR_TR4                       0x00000010U        /*!< Falling trigger event configuration bit of line 4 */
#define  EXTI_FTSR_TR5                       0x00000020U        /*!< Falling trigger event configuration bit of line 5 */
#define  EXTI_FTSR_TR6                       0x00000040U        /*!< Falling trigger event configuration bit of line 6 */
#define  EXTI_FTSR_TR7                       0x00000080U        /*!< Falling trigger event configuration bit of line 7 */
#define  EXTI_FTSR_TR8                       0x00000100U        /*!< Falling trigger event configuration bit of line 8 */
#define  EXTI_FTSR_TR9                       0x00000200U        /*!< Falling trigger event configuration bit of line 9 */
#define  EXTI_FTSR_TR10                      0x00000400U        /*!< Falling trigger event configuration bit of line 10 */
#define  EXTI_FTSR_TR11                      0x00000800U        /*!< Falling trigger event configuration bit of line 11 */
#define  EXTI_FTSR_TR12                      0x00001000U        /*!< Falling trigger event configuration bit of line 12 */
#define  EXTI_FTSR_TR13                      0x00002000U        /*!< Falling trigger event configuration bit of line 13 */
#define  EXTI_FTSR_TR14                      0x00004000U        /*!< Falling trigger event configuration bit of line 14 */
#define  EXTI_FTSR_TR15                      0x00008000U        /*!< Falling trigger event configuration bit of line 15 */
#define  EXTI_FTSR_TR16                      0x00010000U        /*!< Falling trigger event configuration bit of line 16 */
#define  EXTI_FTSR_TR17                      0x00020000U        /*!< Falling trigger event configuration bit of line 17 */
#define  EXTI_FTSR_TR18                      0x00040000U        /*!< Falling trigger event configuration bit of line 18 */
#define  EXTI_FTSR_TR19                      0x00080000U        /*!< Falling trigger event configuration bit of line 19 */
#define  EXTI_FTSR_TR20                      0x00100000U        /*!< Falling trigger event configuration bit of line 20 */
#define  EXTI_FTSR_TR21                      0x00200000U        /*!< Falling trigger event configuration bit of line 21 */
#define  EXTI_FTSR_TR22                      0x00400000U        /*!< Falling trigger event configuration bit of line 22 */
#define  EXTI_FTSR_TR23                      0x00800000U        /*!< Falling trigger event configuration bit of line 23 */

/******************  Bit definition for EXTI_SWIER register  ******************/
#define  EXTI_SWIER_SWIER0                   0x00000001U        /*!< Software Interrupt on line 0 */
#define  EXTI_SWIER_SWIER1                   0x00000002U        /*!< Software Interrupt on line 1 */
#define  EXTI_SWIER_SWIER2                   0x00000004U        /*!< Software Interrupt on line 2 */
#define  EXTI_SWIER_SWIER3                   0x00000008U        /*!< Software Interrupt on line 3 */
#define  EXTI_SWIER_SWIER4                   0x00000010U        /*!< Software Interrupt on line 4 */
#define  EXTI_SWIER_SWIER5                   0x00000020U        /*!< Software Interrupt on line 5 */
#define  EXTI_SWIER_SWIER6                   0x00000040U        /*!< Software Interrupt on line 6 */
#define  EXTI_SWIER_SWIER7                   0x00000080U        /*!< Software Interrupt on line 7 */
#define  EXTI_SWIER_SWIER8                   0x00000100U        /*!< Software Interrupt on line 8 */
#define  EXTI_SWIER_SWIER9                   0x00000200U        /*!< Software Interrupt on line 9 */
#define  EXTI_SWIER_SWIER10                  0x00000400U        /*!< Software Interrupt on line 10 */
#define  EXTI_SWIER_SWIER11                  0x00000800U        /*!< Software Interrupt on line 11 */
#define  EXTI_SWIER_SWIER12                  0x00001000U        /*!< Software Interrupt on line 12 */
#define  EXTI_SWIER_SWIER13                  0x00002000U        /*!< Software Interrupt on line 13 */
#define  EXTI_SWIER_SWIER14                  0x00004000U        /*!< Software Interrupt on line 14 */
#define  EXTI_SWIER_SWIER15                  0x00008000U        /*!< Software Interrupt on line 15 */
#define  EXTI_SWIER_SWIER16                  0x00010000U        /*!< Software Interrupt on line 16 */
#define  EXTI_SWIER_SWIER17                  0x00020000U        /*!< Software Interrupt on line 17 */
#define  EXTI_SWIER_SWIER18                  0x00040000U        /*!< Software Interrupt on line 18 */
#define  EXTI_SWIER_SWIER19                  0x00080000U        /*!< Software Interrupt on line 19 */
#define  EXTI_SWIER_SWIER20                  0x00100000U        /*!< Software Interrupt on line 20 */
#define  EXTI_SWIER_SWIER21                  0x00200000U        /*!< Software Interrupt on line 21 */
#define  EXTI_SWIER_SWIER22                  0x00400000U        /*!< Software Interrupt on line 22 */
#define  EXTI_SWIER_SWIER23                  0x00800000U        /*!< Software Interrupt on line 23 */

/*******************  Bit definition for EXTI_PR register  ********************/
#define  EXTI_PR_PR0                         0x00000001U        /*!< Pending bit for line 0 */
#define  EXTI_PR_PR1                         0x00000002U        /*!< Pending bit for line 1 */
#define  EXTI_PR_PR2                         0x00000004U        /*!< Pending bit for line 2 */
#define  EXTI_PR_PR3                         0x00000008U        /*!< Pending bit for line 3 */
#define  EXTI_PR_PR4                         0x00000010U        /*!< Pending bit for line 4 */
#define  EXTI_PR_PR5                         0x00000020U        /*!< Pending bit for line 5 */
#define  EXTI_PR_PR6                         0x00000040U        /*!< Pending bit for line 6 */
#define  EXTI_PR_PR7                         0x00000080U        /*!< Pending bit for line 7 */
#define  EXTI_PR_PR8                         0x00000100U        /*!< Pending bit for line 8 */
#define  EXTI_PR_PR9                         0x00000200U        /*!< Pending bit for line 9 */
#define  EXTI_PR_PR10                        0x00000400U        /*!< Pending bit for line 10 */
#define  EXTI_PR_PR11                        0x00000800U        /*!< Pending bit for line 11 */
#define  EXTI_PR_PR12                        0x00001000U        /*!< Pending bit for line 12 */
#define  EXTI_PR_PR13                        0x00002000U        /*!< Pending bit for line 13 */
#define  EXTI_PR_PR14                        0x00004000U        /*!< Pending bit for line 14 */
#define  EXTI_PR_PR15                        0x00008000U        /*!< Pending bit for line 15 */
#define  EXTI_PR_PR16                        0x00010000U        /*!< Pending bit for line 16 */
#define  EXTI_PR_PR17                        0x00020000U        /*!< Pending bit for line 17 */
#define  EXTI_PR_PR18                        0x00040000U        /*!< Pending bit for line 18 */
#define  EXTI_PR_PR19                        0x00080000U        /*!< Pending bit for line 19 */
#define  EXTI_PR_PR20                        0x00100000U        /*!< Pending bit for line 20 */
#define  EXTI_PR_PR21                        0x00200000U        /*!< Pending bit for line 21 */
#define  EXTI_PR_PR22                        0x00400000U        /*!< Pending bit for line 22 */
#define  EXTI_PR_PR23                        0x00800000U        /*!< Pending bit for line 23 */

/******************************************************************************/
/*                                                                            */
/*                                    FLASH                                   */
/*                                                                            */
/******************************************************************************/
/*
* @brief FLASH Total Sectors Number
*/
#define FLASH_SECTOR_TOTAL  8

/*******************  Bits definition for FLASH_ACR register  *****************/
#define FLASH_ACR_LATENCY                    0x0000000FU
#define FLASH_ACR_LATENCY_0WS                0x00000000U
#define FLASH_ACR_LATENCY_1WS                0x00000001U
#define FLASH_ACR_LATENCY_2WS                0x00000002U
#define FLASH_ACR_LATENCY_3WS                0x00000003U
#define FLASH_ACR_LATENCY_4WS                0x00000004U
#define FLASH_ACR_LATENCY_5WS                0x00000005U
#define FLASH_ACR_LATENCY_6WS                0x00000006U
#define FLASH_ACR_LATENCY_7WS                0x00000007U
#define FLASH_ACR_LATENCY_8WS                0x00000008U
#define FLASH_ACR_LATENCY_9WS                0x00000009U
#define FLASH_ACR_LATENCY_10WS               0x0000000AU
#define FLASH_ACR_LATENCY_11WS               0x0000000BU
#define FLASH_ACR_LATENCY_12WS               0x0000000CU
#define FLASH_ACR_LATENCY_13WS               0x0000000DU
#define FLASH_ACR_LATENCY_14WS               0x0000000EU
#define FLASH_ACR_LATENCY_15WS               0x0000000FU
#define FLASH_ACR_PRFTEN                     0x00000100U
#define FLASH_ACR_ARTEN                      0x00000200U
#define FLASH_ACR_ARTRST                     0x00000800U

/*******************  Bits definition for FLASH_SR register  ******************/
#define FLASH_SR_EOP                         0x00000001U
#define FLASH_SR_OPERR                       0x00000002U
#define FLASH_SR_WRPERR                      0x00000010U
#define FLASH_SR_PGAERR                      0x00000020U
#define FLASH_SR_PGPERR                      0x00000040U
#define FLASH_SR_ERSERR                      0x00000080U
#define FLASH_SR_BSY                         0x00010000U

/*******************  Bits definition for FLASH_CR register  ******************/
#define FLASH_CR_PG                          0x00000001U
#define FLASH_CR_SER                         0x00000002U
#define FLASH_CR_MER                         0x00000004U
#define FLASH_CR_SNB                         0x00000078U
#define FLASH_CR_SNB_0                       0x00000008U
#define FLASH_CR_SNB_1                       0x00000010U
#define FLASH_CR_SNB_2                       0x00000020U
#define FLASH_CR_SNB_3                       0x00000040U
#define FLASH_CR_PSIZE                       0x00000300U
#define FLASH_CR_PSIZE_0                     0x00000100U
#define FLASH_CR_PSIZE_1                     0x00000200U
#define FLASH_CR_STRT                        0x00010000U
#define FLASH_CR_EOPIE                       0x01000000U
#define FLASH_CR_ERRIE                       0x02000000U
#define FLASH_CR_LOCK                        0x80000000U

/*******************  Bits definition for FLASH_OPTCR register  ***************/
#define FLASH_OPTCR_OPTLOCK                 0x00000001U
#define FLASH_OPTCR_OPTSTRT                 0x00000002U
#define FLASH_OPTCR_BOR_LEV                 0x0000000CU
#define FLASH_OPTCR_BOR_LEV_0               0x00000004U
#define FLASH_OPTCR_BOR_LEV_1               0x00000008U
#define FLASH_OPTCR_WWDG_SW                 0x00000010U
#define FLASH_OPTCR_IWDG_SW                 0x00000020U
#define FLASH_OPTCR_nRST_STOP               0x00000040U
#define FLASH_OPTCR_nRST_STDBY              0x00000080U
#define FLASH_OPTCR_RDP                     0x0000FF00U
#define FLASH_OPTCR_RDP_0                   0x00000100U
#define FLASH_OPTCR_RDP_1                   0x00000200U
#define FLASH_OPTCR_RDP_2                   0x00000400U
#define FLASH_OPTCR_RDP_3                   0x00000800U
#define FLASH_OPTCR_RDP_4                   0x00001000U
#define FLASH_OPTCR_RDP_5                   0x00002000U
#define FLASH_OPTCR_RDP_6                   0x00004000U
#define FLASH_OPTCR_RDP_7                   0x00008000U
#define FLASH_OPTCR_nWRP                    0x00FF0000U
#define FLASH_OPTCR_nWRP_0                  0x00010000U
#define FLASH_OPTCR_nWRP_1                  0x00020000U
#define FLASH_OPTCR_nWRP_2                  0x00040000U
#define FLASH_OPTCR_nWRP_3                  0x00080000U
#define FLASH_OPTCR_nWRP_4                  0x00100000U
#define FLASH_OPTCR_nWRP_5                  0x00200000U
#define FLASH_OPTCR_nWRP_6                  0x00400000U
#define FLASH_OPTCR_nWRP_7                  0x00800000U
#define FLASH_OPTCR_IWDG_STDBY              0x40000000U
#define FLASH_OPTCR_IWDG_STOP               0x80000000U

/*******************  Bits definition for FLASH_OPTCR1 register  ***************/
#define FLASH_OPTCR1_BOOT_ADD0              0x0000FFFFU
#define FLASH_OPTCR1_BOOT_ADD1              0xFFFF0000U


/******************************************************************************/
/*                                                                            */
/*                          Flexible Memory Controller                        */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for FMC_BCR1 register  *******************/
#define  FMC_BCR1_MBKEN                     0x00000001U        /*!<Memory bank enable bit                 */
#define  FMC_BCR1_MUXEN                     0x00000002U        /*!<Address/data multiplexing enable bit   */
#define  FMC_BCR1_MTYP                      0x0000000CU        /*!<MTYP[1:0] bits (Memory type)           */
#define  FMC_BCR1_MTYP_0                    0x00000004U        /*!<Bit 0 */
#define  FMC_BCR1_MTYP_1                    0x00000008U        /*!<Bit 1 */
#define  FMC_BCR1_MWID                      0x00000030U        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FMC_BCR1_MWID_0                    0x00000010U        /*!<Bit 0 */
#define  FMC_BCR1_MWID_1                    0x00000020U        /*!<Bit 1 */
#define  FMC_BCR1_FACCEN                    0x00000040U        /*!<Flash access enable        */
#define  FMC_BCR1_BURSTEN                   0x00000100U        /*!<Burst enable bit           */
#define  FMC_BCR1_WAITPOL                   0x00000200U        /*!<Wait signal polarity bit   */
#define  FMC_BCR1_WRAPMOD                   0x00000400U        /*!<Wrapped burst mode support */
#define  FMC_BCR1_WAITCFG                   0x00000800U        /*!<Wait timing configuration  */
#define  FMC_BCR1_WREN                      0x00001000U        /*!<Write enable bit           */
#define  FMC_BCR1_WAITEN                    0x00002000U        /*!<Wait enable bit            */
#define  FMC_BCR1_EXTMOD                    0x00004000U        /*!<Extended mode enable       */
#define  FMC_BCR1_ASYNCWAIT                 0x00008000U        /*!<Asynchronous wait          */
#define  FMC_BCR1_CPSIZE                    0x00070000U        /*!<CRAM page size             */
#define  FMC_BCR1_CPSIZE_0                  0x00010000U        /*!<Bit 0 */
#define  FMC_BCR1_CPSIZE_1                  0x00020000U        /*!<Bit 1 */
#define  FMC_BCR1_CPSIZE_2                  0x00040000U        /*!<Bit 2 */
#define  FMC_BCR1_CBURSTRW                  0x00080000U        /*!<Write burst enable         */
#define  FMC_BCR1_CCLKEN                    0x00100000U        /*!<Continous clock enable     */
#define  FMC_BCR1_WFDIS                     0x00200000U        /*!<Write FIFO Disable         */

/******************  Bit definition for FMC_BCR2 register  *******************/
#define  FMC_BCR2_MBKEN                     0x00000001U        /*!<Memory bank enable bit                 */
#define  FMC_BCR2_MUXEN                     0x00000002U        /*!<Address/data multiplexing enable bit   */
#define  FMC_BCR2_MTYP                      0x0000000CU        /*!<MTYP[1:0] bits (Memory type)           */
#define  FMC_BCR2_MTYP_0                    0x00000004U        /*!<Bit 0 */
#define  FMC_BCR2_MTYP_1                    0x00000008U        /*!<Bit 1 */
#define  FMC_BCR2_MWID                      0x00000030U        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FMC_BCR2_MWID_0                    0x00000010U        /*!<Bit 0 */
#define  FMC_BCR2_MWID_1                    0x00000020U        /*!<Bit 1 */
#define  FMC_BCR2_FACCEN                    0x00000040U        /*!<Flash access enable        */
#define  FMC_BCR2_BURSTEN                   0x00000100U        /*!<Burst enable bit           */
#define  FMC_BCR2_WAITPOL                   0x00000200U        /*!<Wait signal polarity bit   */
#define  FMC_BCR2_WRAPMOD                   0x00000400U        /*!<Wrapped burst mode support */
#define  FMC_BCR2_WAITCFG                   0x00000800U        /*!<Wait timing configuration  */
#define  FMC_BCR2_WREN                      0x00001000U        /*!<Write enable bit           */
#define  FMC_BCR2_WAITEN                    0x00002000U        /*!<Wait enable bit            */
#define  FMC_BCR2_EXTMOD                    0x00004000U        /*!<Extended mode enable       */
#define  FMC_BCR2_ASYNCWAIT                 0x00008000U        /*!<Asynchronous wait          */
#define  FMC_BCR2_CPSIZE                    0x00070000U        /*!<CRAM page size             */
#define  FMC_BCR2_CPSIZE_0                  0x00010000U        /*!<Bit 0 */
#define  FMC_BCR2_CPSIZE_1                  0x00020000U        /*!<Bit 1 */
#define  FMC_BCR2_CPSIZE_2                  0x00040000U        /*!<Bit 2 */
#define  FMC_BCR2_CBURSTRW                  0x00080000U        /*!<Write burst enable         */

/******************  Bit definition for FMC_BCR3 register  *******************/
#define  FMC_BCR3_MBKEN                     0x00000001U        /*!<Memory bank enable bit                 */
#define  FMC_BCR3_MUXEN                     0x00000002U        /*!<Address/data multiplexing enable bit   */
#define  FMC_BCR3_MTYP                      0x0000000CU        /*!<MTYP[1:0] bits (Memory type)           */
#define  FMC_BCR3_MTYP_0                    0x00000004U        /*!<Bit 0 */
#define  FMC_BCR3_MTYP_1                    0x00000008U        /*!<Bit 1 */
#define  FMC_BCR3_MWID                      0x00000030U        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FMC_BCR3_MWID_0                    0x00000010U        /*!<Bit 0 */
#define  FMC_BCR3_MWID_1                    0x00000020U        /*!<Bit 1 */
#define  FMC_BCR3_FACCEN                    0x00000040U        /*!<Flash access enable        */
#define  FMC_BCR3_BURSTEN                   0x00000100U        /*!<Burst enable bit           */
#define  FMC_BCR3_WAITPOL                   0x00000200U        /*!<Wait signal polarity bit   */
#define  FMC_BCR3_WRAPMOD                   0x00000400U        /*!<Wrapped burst mode support */
#define  FMC_BCR3_WAITCFG                   0x00000800U        /*!<Wait timing configuration  */
#define  FMC_BCR3_WREN                      0x00001000U        /*!<Write enable bit           */
#define  FMC_BCR3_WAITEN                    0x00002000U        /*!<Wait enable bit            */
#define  FMC_BCR3_EXTMOD                    0x00004000U        /*!<Extended mode enable       */
#define  FMC_BCR3_ASYNCWAIT                 0x00008000U        /*!<Asynchronous wait          */
#define  FMC_BCR3_CPSIZE                    0x00070000U        /*!<CRAM page size             */
#define  FMC_BCR3_CPSIZE_0                  0x00010000U        /*!<Bit 0 */
#define  FMC_BCR3_CPSIZE_1                  0x00020000U        /*!<Bit 1 */
#define  FMC_BCR3_CPSIZE_2                  0x00040000U        /*!<Bit 2 */
#define  FMC_BCR3_CBURSTRW                  0x00080000U        /*!<Write burst enable         */

/******************  Bit definition for FMC_BCR4 register  *******************/
#define  FMC_BCR4_MBKEN                     0x00000001U        /*!<Memory bank enable bit                 */
#define  FMC_BCR4_MUXEN                     0x00000002U        /*!<Address/data multiplexing enable bit   */
#define  FMC_BCR4_MTYP                      0x0000000CU        /*!<MTYP[1:0] bits (Memory type)           */
#define  FMC_BCR4_MTYP_0                    0x00000004U        /*!<Bit 0 */
#define  FMC_BCR4_MTYP_1                    0x00000008U        /*!<Bit 1 */
#define  FMC_BCR4_MWID                      0x00000030U        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FMC_BCR4_MWID_0                    0x00000010U        /*!<Bit 0 */
#define  FMC_BCR4_MWID_1                    0x00000020U        /*!<Bit 1 */
#define  FMC_BCR4_FACCEN                    0x00000040U        /*!<Flash access enable        */
#define  FMC_BCR4_BURSTEN                   0x00000100U        /*!<Burst enable bit           */
#define  FMC_BCR4_WAITPOL                   0x00000200U        /*!<Wait signal polarity bit   */
#define  FMC_BCR4_WRAPMOD                   0x00000400U        /*!<Wrapped burst mode support */
#define  FMC_BCR4_WAITCFG                   0x00000800U        /*!<Wait timing configuration  */
#define  FMC_BCR4_WREN                      0x00001000U        /*!<Write enable bit           */
#define  FMC_BCR4_WAITEN                    0x00002000U        /*!<Wait enable bit            */
#define  FMC_BCR4_EXTMOD                    0x00004000U        /*!<Extended mode enable       */
#define  FMC_BCR4_ASYNCWAIT                 0x00008000U        /*!<Asynchronous wait          */
#define  FMC_BCR4_CPSIZE                    0x00070000U        /*!<CRAM page size             */
#define  FMC_BCR4_CPSIZE_0                  0x00010000U        /*!<Bit 0 */
#define  FMC_BCR4_CPSIZE_1                  0x00020000U        /*!<Bit 1 */
#define  FMC_BCR4_CPSIZE_2                  0x00040000U        /*!<Bit 2 */
#define  FMC_BCR4_CBURSTRW                  0x00080000U        /*!<Write burst enable         */

/******************  Bit definition for FMC_BTR1 register  ******************/
#define  FMC_BTR1_ADDSET                    0x0000000FU        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BTR1_ADDSET_0                  0x00000001U        /*!<Bit 0 */
#define  FMC_BTR1_ADDSET_1                  0x00000002U        /*!<Bit 1 */
#define  FMC_BTR1_ADDSET_2                  0x00000004U        /*!<Bit 2 */
#define  FMC_BTR1_ADDSET_3                  0x00000008U        /*!<Bit 3 */
#define  FMC_BTR1_ADDHLD                    0x000000F0U        /*!<ADDHLD[3:0] bits (Address-hold phase duration)  */
#define  FMC_BTR1_ADDHLD_0                  0x00000010U        /*!<Bit 0 */
#define  FMC_BTR1_ADDHLD_1                  0x00000020U        /*!<Bit 1 */
#define  FMC_BTR1_ADDHLD_2                  0x00000040U        /*!<Bit 2 */
#define  FMC_BTR1_ADDHLD_3                  0x00000080U        /*!<Bit 3 */
#define  FMC_BTR1_DATAST                    0x0000FF00U        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BTR1_DATAST_0                  0x00000100U        /*!<Bit 0 */
#define  FMC_BTR1_DATAST_1                  0x00000200U        /*!<Bit 1 */
#define  FMC_BTR1_DATAST_2                  0x00000400U        /*!<Bit 2 */
#define  FMC_BTR1_DATAST_3                  0x00000800U        /*!<Bit 3 */
#define  FMC_BTR1_DATAST_4                  0x00001000U        /*!<Bit 4 */
#define  FMC_BTR1_DATAST_5                  0x00002000U        /*!<Bit 5 */
#define  FMC_BTR1_DATAST_6                  0x00004000U        /*!<Bit 6 */
#define  FMC_BTR1_DATAST_7                  0x00008000U        /*!<Bit 7 */
#define  FMC_BTR1_BUSTURN                   0x000F0000U        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BTR1_BUSTURN_0                 0x00010000U        /*!<Bit 0 */
#define  FMC_BTR1_BUSTURN_1                 0x00020000U        /*!<Bit 1 */
#define  FMC_BTR1_BUSTURN_2                 0x00040000U        /*!<Bit 2 */
#define  FMC_BTR1_BUSTURN_3                 0x00080000U        /*!<Bit 3 */
#define  FMC_BTR1_CLKDIV                    0x00F00000U        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BTR1_CLKDIV_0                  0x00100000U        /*!<Bit 0 */
#define  FMC_BTR1_CLKDIV_1                  0x00200000U        /*!<Bit 1 */
#define  FMC_BTR1_CLKDIV_2                  0x00400000U        /*!<Bit 2 */
#define  FMC_BTR1_CLKDIV_3                  0x00800000U        /*!<Bit 3 */
#define  FMC_BTR1_DATLAT                    0x0F000000U        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BTR1_DATLAT_0                  0x01000000U        /*!<Bit 0 */
#define  FMC_BTR1_DATLAT_1                  0x02000000U        /*!<Bit 1 */
#define  FMC_BTR1_DATLAT_2                  0x04000000U        /*!<Bit 2 */
#define  FMC_BTR1_DATLAT_3                  0x08000000U        /*!<Bit 3 */
#define  FMC_BTR1_ACCMOD                    0x30000000U        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BTR1_ACCMOD_0                  0x10000000U        /*!<Bit 0 */
#define  FMC_BTR1_ACCMOD_1                  0x20000000U        /*!<Bit 1 */

/******************  Bit definition for FMC_BTR2 register  *******************/
#define  FMC_BTR2_ADDSET                    0x0000000FU        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BTR2_ADDSET_0                  0x00000001U        /*!<Bit 0 */
#define  FMC_BTR2_ADDSET_1                  0x00000002U        /*!<Bit 1 */
#define  FMC_BTR2_ADDSET_2                  0x00000004U        /*!<Bit 2 */
#define  FMC_BTR2_ADDSET_3                  0x00000008U        /*!<Bit 3 */
#define  FMC_BTR2_ADDHLD                    0x000000F0U        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BTR2_ADDHLD_0                  0x00000010U        /*!<Bit 0 */
#define  FMC_BTR2_ADDHLD_1                  0x00000020U        /*!<Bit 1 */
#define  FMC_BTR2_ADDHLD_2                  0x00000040U        /*!<Bit 2 */
#define  FMC_BTR2_ADDHLD_3                  0x00000080U        /*!<Bit 3 */
#define  FMC_BTR2_DATAST                    0x0000FF00U        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BTR2_DATAST_0                  0x00000100U        /*!<Bit 0 */
#define  FMC_BTR2_DATAST_1                  0x00000200U        /*!<Bit 1 */
#define  FMC_BTR2_DATAST_2                  0x00000400U        /*!<Bit 2 */
#define  FMC_BTR2_DATAST_3                  0x00000800U        /*!<Bit 3 */
#define  FMC_BTR2_DATAST_4                  0x00001000U        /*!<Bit 4 */
#define  FMC_BTR2_DATAST_5                  0x00002000U        /*!<Bit 5 */
#define  FMC_BTR2_DATAST_6                  0x00004000U        /*!<Bit 6 */
#define  FMC_BTR2_DATAST_7                  0x00008000U        /*!<Bit 7 */
#define  FMC_BTR2_BUSTURN                   0x000F0000U        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BTR2_BUSTURN_0                 0x00010000U        /*!<Bit 0 */
#define  FMC_BTR2_BUSTURN_1                 0x00020000U        /*!<Bit 1 */
#define  FMC_BTR2_BUSTURN_2                 0x00040000U        /*!<Bit 2 */
#define  FMC_BTR2_BUSTURN_3                 0x00080000U        /*!<Bit 3 */
#define  FMC_BTR2_CLKDIV                    0x00F00000U        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BTR2_CLKDIV_0                  0x00100000U        /*!<Bit 0 */
#define  FMC_BTR2_CLKDIV_1                  0x00200000U        /*!<Bit 1 */
#define  FMC_BTR2_CLKDIV_2                  0x00400000U        /*!<Bit 2 */
#define  FMC_BTR2_CLKDIV_3                  0x00800000U        /*!<Bit 3 */
#define  FMC_BTR2_DATLAT                    0x0F000000U        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BTR2_DATLAT_0                  0x01000000U        /*!<Bit 0 */
#define  FMC_BTR2_DATLAT_1                  0x02000000U        /*!<Bit 1 */
#define  FMC_BTR2_DATLAT_2                  0x04000000U        /*!<Bit 2 */
#define  FMC_BTR2_DATLAT_3                  0x08000000U        /*!<Bit 3 */
#define  FMC_BTR2_ACCMOD                    0x30000000U        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BTR2_ACCMOD_0                  0x10000000U        /*!<Bit 0 */
#define  FMC_BTR2_ACCMOD_1                  0x20000000U        /*!<Bit 1 */

/*******************  Bit definition for FMC_BTR3 register  *******************/
#define  FMC_BTR3_ADDSET                    0x0000000FU        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BTR3_ADDSET_0                  0x00000001U        /*!<Bit 0 */
#define  FMC_BTR3_ADDSET_1                  0x00000002U        /*!<Bit 1 */
#define  FMC_BTR3_ADDSET_2                  0x00000004U        /*!<Bit 2 */
#define  FMC_BTR3_ADDSET_3                  0x00000008U        /*!<Bit 3 */
#define  FMC_BTR3_ADDHLD                    0x000000F0U        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BTR3_ADDHLD_0                  0x00000010U        /*!<Bit 0 */
#define  FMC_BTR3_ADDHLD_1                  0x00000020U        /*!<Bit 1 */
#define  FMC_BTR3_ADDHLD_2                  0x00000040U        /*!<Bit 2 */
#define  FMC_BTR3_ADDHLD_3                  0x00000080U        /*!<Bit 3 */
#define  FMC_BTR3_DATAST                    0x0000FF00U        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BTR3_DATAST_0                  0x00000100U        /*!<Bit 0 */
#define  FMC_BTR3_DATAST_1                  0x00000200U        /*!<Bit 1 */
#define  FMC_BTR3_DATAST_2                  0x00000400U        /*!<Bit 2 */
#define  FMC_BTR3_DATAST_3                  0x00000800U        /*!<Bit 3 */
#define  FMC_BTR3_DATAST_4                  0x00001000U        /*!<Bit 4 */
#define  FMC_BTR3_DATAST_5                  0x00002000U        /*!<Bit 5 */
#define  FMC_BTR3_DATAST_6                  0x00004000U        /*!<Bit 6 */
#define  FMC_BTR3_DATAST_7                  0x00008000U        /*!<Bit 7 */
#define  FMC_BTR3_BUSTURN                   0x000F0000U        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BTR3_BUSTURN_0                 0x00010000U        /*!<Bit 0 */
#define  FMC_BTR3_BUSTURN_1                 0x00020000U        /*!<Bit 1 */
#define  FMC_BTR3_BUSTURN_2                 0x00040000U        /*!<Bit 2 */
#define  FMC_BTR3_BUSTURN_3                 0x00080000U        /*!<Bit 3 */
#define  FMC_BTR3_CLKDIV                    0x00F00000U        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BTR3_CLKDIV_0                  0x00100000U        /*!<Bit 0 */
#define  FMC_BTR3_CLKDIV_1                  0x00200000U        /*!<Bit 1 */
#define  FMC_BTR3_CLKDIV_2                  0x00400000U        /*!<Bit 2 */
#define  FMC_BTR3_CLKDIV_3                  0x00800000U        /*!<Bit 3 */
#define  FMC_BTR3_DATLAT                    0x0F000000U        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BTR3_DATLAT_0                  0x01000000U        /*!<Bit 0 */
#define  FMC_BTR3_DATLAT_1                  0x02000000U        /*!<Bit 1 */
#define  FMC_BTR3_DATLAT_2                  0x04000000U        /*!<Bit 2 */
#define  FMC_BTR3_DATLAT_3                  0x08000000U        /*!<Bit 3 */
#define  FMC_BTR3_ACCMOD                    0x30000000U        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BTR3_ACCMOD_0                  0x10000000U        /*!<Bit 0 */
#define  FMC_BTR3_ACCMOD_1                  0x20000000U        /*!<Bit 1 */

/******************  Bit definition for FMC_BTR4 register  *******************/
#define  FMC_BTR4_ADDSET                    0x0000000FU        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BTR4_ADDSET_0                  0x00000001U        /*!<Bit 0 */
#define  FMC_BTR4_ADDSET_1                  0x00000002U        /*!<Bit 1 */
#define  FMC_BTR4_ADDSET_2                  0x00000004U        /*!<Bit 2 */
#define  FMC_BTR4_ADDSET_3                  0x00000008U        /*!<Bit 3 */
#define  FMC_BTR4_ADDHLD                    0x000000F0U        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BTR4_ADDHLD_0                  0x00000010U        /*!<Bit 0 */
#define  FMC_BTR4_ADDHLD_1                  0x00000020U        /*!<Bit 1 */
#define  FMC_BTR4_ADDHLD_2                  0x00000040U        /*!<Bit 2 */
#define  FMC_BTR4_ADDHLD_3                  0x00000080U        /*!<Bit 3 */
#define  FMC_BTR4_DATAST                    0x0000FF00U        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BTR4_DATAST_0                  0x00000100U        /*!<Bit 0 */
#define  FMC_BTR4_DATAST_1                  0x00000200U        /*!<Bit 1 */
#define  FMC_BTR4_DATAST_2                  0x00000400U        /*!<Bit 2 */
#define  FMC_BTR4_DATAST_3                  0x00000800U        /*!<Bit 3 */
#define  FMC_BTR4_DATAST_4                  0x00001000U        /*!<Bit 4 */
#define  FMC_BTR4_DATAST_5                  0x00002000U        /*!<Bit 5 */
#define  FMC_BTR4_DATAST_6                  0x00004000U        /*!<Bit 6 */
#define  FMC_BTR4_DATAST_7                  0x00008000U        /*!<Bit 7 */
#define  FMC_BTR4_BUSTURN                   0x000F0000U        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BTR4_BUSTURN_0                 0x00010000U        /*!<Bit 0 */
#define  FMC_BTR4_BUSTURN_1                 0x00020000U        /*!<Bit 1 */
#define  FMC_BTR4_BUSTURN_2                 0x00040000U        /*!<Bit 2 */
#define  FMC_BTR4_BUSTURN_3                 0x00080000U        /*!<Bit 3 */
#define  FMC_BTR4_CLKDIV                    0x00F00000U        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BTR4_CLKDIV_0                  0x00100000U        /*!<Bit 0 */
#define  FMC_BTR4_CLKDIV_1                  0x00200000U        /*!<Bit 1 */
#define  FMC_BTR4_CLKDIV_2                  0x00400000U        /*!<Bit 2 */
#define  FMC_BTR4_CLKDIV_3                  0x00800000U        /*!<Bit 3 */
#define  FMC_BTR4_DATLAT                    0x0F000000U        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BTR4_DATLAT_0                  0x01000000U        /*!<Bit 0 */
#define  FMC_BTR4_DATLAT_1                  0x02000000U        /*!<Bit 1 */
#define  FMC_BTR4_DATLAT_2                  0x04000000U        /*!<Bit 2 */
#define  FMC_BTR4_DATLAT_3                  0x08000000U        /*!<Bit 3 */
#define  FMC_BTR4_ACCMOD                    0x30000000U        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BTR4_ACCMOD_0                  0x10000000U        /*!<Bit 0 */
#define  FMC_BTR4_ACCMOD_1                  0x20000000U        /*!<Bit 1 */

/******************  Bit definition for FMC_BWTR1 register  ******************/
#define  FMC_BWTR1_ADDSET                   0x0000000FU        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BWTR1_ADDSET_0                 0x00000001U        /*!<Bit 0 */
#define  FMC_BWTR1_ADDSET_1                 0x00000002U        /*!<Bit 1 */
#define  FMC_BWTR1_ADDSET_2                 0x00000004U        /*!<Bit 2 */
#define  FMC_BWTR1_ADDSET_3                 0x00000008U        /*!<Bit 3 */
#define  FMC_BWTR1_ADDHLD                   0x000000F0U        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BWTR1_ADDHLD_0                 0x00000010U        /*!<Bit 0 */
#define  FMC_BWTR1_ADDHLD_1                 0x00000020U        /*!<Bit 1 */
#define  FMC_BWTR1_ADDHLD_2                 0x00000040U        /*!<Bit 2 */
#define  FMC_BWTR1_ADDHLD_3                 0x00000080U        /*!<Bit 3 */
#define  FMC_BWTR1_DATAST                   0x0000FF00U        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BWTR1_DATAST_0                 0x00000100U        /*!<Bit 0 */
#define  FMC_BWTR1_DATAST_1                 0x00000200U        /*!<Bit 1 */
#define  FMC_BWTR1_DATAST_2                 0x00000400U        /*!<Bit 2 */
#define  FMC_BWTR1_DATAST_3                 0x00000800U        /*!<Bit 3 */
#define  FMC_BWTR1_DATAST_4                 0x00001000U        /*!<Bit 4 */
#define  FMC_BWTR1_DATAST_5                 0x00002000U        /*!<Bit 5 */
#define  FMC_BWTR1_DATAST_6                 0x00004000U        /*!<Bit 6 */
#define  FMC_BWTR1_DATAST_7                 0x00008000U        /*!<Bit 7 */
#define  FMC_BWTR1_BUSTURN                  0x000F0000U        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BWTR1_BUSTURN_0                0x00010000U        /*!<Bit 0 */
#define  FMC_BWTR1_BUSTURN_1                0x00020000U        /*!<Bit 1 */
#define  FMC_BWTR1_BUSTURN_2                0x00040000U        /*!<Bit 2 */
#define  FMC_BWTR1_BUSTURN_3                0x00080000U        /*!<Bit 3 */
#define  FMC_BWTR1_ACCMOD                   0x30000000U        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BWTR1_ACCMOD_0                 0x10000000U        /*!<Bit 0 */
#define  FMC_BWTR1_ACCMOD_1                 0x20000000U        /*!<Bit 1 */

/******************  Bit definition for FMC_BWTR2 register  ******************/
#define  FMC_BWTR2_ADDSET                   0x0000000FU        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BWTR2_ADDSET_0                 0x00000001U        /*!<Bit 0 */
#define  FMC_BWTR2_ADDSET_1                 0x00000002U        /*!<Bit 1 */
#define  FMC_BWTR2_ADDSET_2                 0x00000004U        /*!<Bit 2 */
#define  FMC_BWTR2_ADDSET_3                 0x00000008U        /*!<Bit 3 */
#define  FMC_BWTR2_ADDHLD                   0x000000F0U        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BWTR2_ADDHLD_0                 0x00000010U        /*!<Bit 0 */
#define  FMC_BWTR2_ADDHLD_1                 0x00000020U        /*!<Bit 1 */
#define  FMC_BWTR2_ADDHLD_2                 0x00000040U        /*!<Bit 2 */
#define  FMC_BWTR2_ADDHLD_3                 0x00000080U        /*!<Bit 3 */
#define  FMC_BWTR2_DATAST                   0x0000FF00U        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BWTR2_DATAST_0                 0x00000100U        /*!<Bit 0 */
#define  FMC_BWTR2_DATAST_1                 0x00000200U        /*!<Bit 1 */
#define  FMC_BWTR2_DATAST_2                 0x00000400U        /*!<Bit 2 */
#define  FMC_BWTR2_DATAST_3                 0x00000800U        /*!<Bit 3 */
#define  FMC_BWTR2_DATAST_4                 0x00001000U        /*!<Bit 4 */
#define  FMC_BWTR2_DATAST_5                 0x00002000U        /*!<Bit 5 */
#define  FMC_BWTR2_DATAST_6                 0x00004000U        /*!<Bit 6 */
#define  FMC_BWTR2_DATAST_7                 0x00008000U        /*!<Bit 7 */
#define  FMC_BWTR2_BUSTURN                  0x000F0000U        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BWTR2_BUSTURN_0                0x00010000U        /*!<Bit 0 */
#define  FMC_BWTR2_BUSTURN_1                0x00020000U        /*!<Bit 1 */
#define  FMC_BWTR2_BUSTURN_2                0x00040000U        /*!<Bit 2 */
#define  FMC_BWTR2_BUSTURN_3                0x00080000U        /*!<Bit 3 */
#define  FMC_BWTR2_ACCMOD                   0x30000000U        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BWTR2_ACCMOD_0                 0x10000000U        /*!<Bit 0 */
#define  FMC_BWTR2_ACCMOD_1                 0x20000000U        /*!<Bit 1 */

/******************  Bit definition for FMC_BWTR3 register  ******************/
#define  FMC_BWTR3_ADDSET                   0x0000000FU        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BWTR3_ADDSET_0                 0x00000001U        /*!<Bit 0 */
#define  FMC_BWTR3_ADDSET_1                 0x00000002U        /*!<Bit 1 */
#define  FMC_BWTR3_ADDSET_2                 0x00000004U        /*!<Bit 2 */
#define  FMC_BWTR3_ADDSET_3                 0x00000008U        /*!<Bit 3 */
#define  FMC_BWTR3_ADDHLD                   0x000000F0U        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BWTR3_ADDHLD_0                 0x00000010U        /*!<Bit 0 */
#define  FMC_BWTR3_ADDHLD_1                 0x00000020U        /*!<Bit 1 */
#define  FMC_BWTR3_ADDHLD_2                 0x00000040U        /*!<Bit 2 */
#define  FMC_BWTR3_ADDHLD_3                 0x00000080U        /*!<Bit 3 */
#define  FMC_BWTR3_DATAST                   0x0000FF00U        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BWTR3_DATAST_0                 0x00000100U        /*!<Bit 0 */
#define  FMC_BWTR3_DATAST_1                 0x00000200U        /*!<Bit 1 */
#define  FMC_BWTR3_DATAST_2                 0x00000400U        /*!<Bit 2 */
#define  FMC_BWTR3_DATAST_3                 0x00000800U        /*!<Bit 3 */
#define  FMC_BWTR3_DATAST_4                 0x00001000U        /*!<Bit 4 */
#define  FMC_BWTR3_DATAST_5                 0x00002000U        /*!<Bit 5 */
#define  FMC_BWTR3_DATAST_6                 0x00004000U        /*!<Bit 6 */
#define  FMC_BWTR3_DATAST_7                 0x00008000U        /*!<Bit 7 */
#define  FMC_BWTR3_BUSTURN                  0x000F0000U        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BWTR3_BUSTURN_0                0x00010000U        /*!<Bit 0 */
#define  FMC_BWTR3_BUSTURN_1                0x00020000U        /*!<Bit 1 */
#define  FMC_BWTR3_BUSTURN_2                0x00040000U        /*!<Bit 2 */
#define  FMC_BWTR3_BUSTURN_3                0x00080000U        /*!<Bit 3 */
#define  FMC_BWTR3_ACCMOD                   0x30000000U        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BWTR3_ACCMOD_0                 0x10000000U        /*!<Bit 0 */
#define  FMC_BWTR3_ACCMOD_1                 0x20000000U        /*!<Bit 1 */

/******************  Bit definition for FMC_BWTR4 register  ******************/
#define  FMC_BWTR4_ADDSET                   0x0000000FU        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BWTR4_ADDSET_0                 0x00000001U        /*!<Bit 0 */
#define  FMC_BWTR4_ADDSET_1                 0x00000002U        /*!<Bit 1 */
#define  FMC_BWTR4_ADDSET_2                 0x00000004U        /*!<Bit 2 */
#define  FMC_BWTR4_ADDSET_3                 0x00000008U        /*!<Bit 3 */
#define  FMC_BWTR4_ADDHLD                   0x000000F0U        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BWTR4_ADDHLD_0                 0x00000010U        /*!<Bit 0 */
#define  FMC_BWTR4_ADDHLD_1                 0x00000020U        /*!<Bit 1 */
#define  FMC_BWTR4_ADDHLD_2                 0x00000040U        /*!<Bit 2 */
#define  FMC_BWTR4_ADDHLD_3                 0x00000080U        /*!<Bit 3 */
#define  FMC_BWTR4_DATAST                   0x0000FF00U        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BWTR4_DATAST_0                 0x00000100U        /*!<Bit 0 */
#define  FMC_BWTR4_DATAST_1                 0x00000200U        /*!<Bit 1 */
#define  FMC_BWTR4_DATAST_2                 0x00000400U        /*!<Bit 2 */
#define  FMC_BWTR4_DATAST_3                 0x00000800U        /*!<Bit 3 */
#define  FMC_BWTR4_DATAST_4                 0x00001000U        /*!<Bit 4 */
#define  FMC_BWTR4_DATAST_5                 0x00002000U        /*!<Bit 5 */
#define  FMC_BWTR4_DATAST_6                 0x00004000U        /*!<Bit 6 */
#define  FMC_BWTR4_DATAST_7                 0x00008000U        /*!<Bit 7 */
#define  FMC_BWTR4_BUSTURN                  0x000F0000U        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BWTR4_BUSTURN_0                0x00010000U        /*!<Bit 0 */
#define  FMC_BWTR4_BUSTURN_1                0x00020000U        /*!<Bit 1 */
#define  FMC_BWTR4_BUSTURN_2                0x00040000U        /*!<Bit 2 */
#define  FMC_BWTR4_BUSTURN_3                0x00080000U        /*!<Bit 3 */
#define  FMC_BWTR4_ACCMOD                   0x30000000U        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BWTR4_ACCMOD_0                 0x10000000U        /*!<Bit 0 */
#define  FMC_BWTR4_ACCMOD_1                 0x20000000U        /*!<Bit 1 */

/******************  Bit definition for FMC_PCR register  *******************/
#define  FMC_PCR_PWAITEN                   0x00000002U        /*!<Wait feature enable bit                   */
#define  FMC_PCR_PBKEN                     0x00000004U        /*!<PC Card/NAND Flash memory bank enable bit */
#define  FMC_PCR_PTYP                      0x00000008U        /*!<Memory type                               */
#define  FMC_PCR_PWID                      0x00000030U        /*!<PWID[1:0] bits (NAND Flash databus width) */
#define  FMC_PCR_PWID_0                    0x00000010U        /*!<Bit 0 */
#define  FMC_PCR_PWID_1                    0x00000020U        /*!<Bit 1 */
#define  FMC_PCR_ECCEN                     0x00000040U        /*!<ECC computation logic enable bit          */
#define  FMC_PCR_TCLR                      0x00001E00U        /*!<TCLR[3:0] bits (CLE to RE delay)          */
#define  FMC_PCR_TCLR_0                    0x00000200U        /*!<Bit 0 */
#define  FMC_PCR_TCLR_1                    0x00000400U        /*!<Bit 1 */
#define  FMC_PCR_TCLR_2                    0x00000800U        /*!<Bit 2 */
#define  FMC_PCR_TCLR_3                    0x00001000U        /*!<Bit 3 */
#define  FMC_PCR_TAR                       0x0001E000U        /*!<TAR[3:0] bits (ALE to RE delay)           */
#define  FMC_PCR_TAR_0                     0x00002000U        /*!<Bit 0 */
#define  FMC_PCR_TAR_1                     0x00004000U        /*!<Bit 1 */
#define  FMC_PCR_TAR_2                     0x00008000U        /*!<Bit 2 */
#define  FMC_PCR_TAR_3                     0x00010000U        /*!<Bit 3 */
#define  FMC_PCR_ECCPS                     0x000E0000U        /*!<ECCPS[2:0] bits (ECC page size)           */
#define  FMC_PCR_ECCPS_0                   0x00020000U        /*!<Bit 0 */
#define  FMC_PCR_ECCPS_1                   0x00040000U        /*!<Bit 1 */
#define  FMC_PCR_ECCPS_2                   0x00080000U        /*!<Bit 2 */

/*******************  Bit definition for FMC_SR register  *******************/
#define  FMC_SR_IRS                        0x01U              /*!<Interrupt Rising Edge status                */
#define  FMC_SR_ILS                        0x02U              /*!<Interrupt Level status                      */
#define  FMC_SR_IFS                        0x04U              /*!<Interrupt Falling Edge status               */
#define  FMC_SR_IREN                       0x08U              /*!<Interrupt Rising Edge detection Enable bit  */
#define  FMC_SR_ILEN                       0x10U              /*!<Interrupt Level detection Enable bit        */
#define  FMC_SR_IFEN                       0x20U              /*!<Interrupt Falling Edge detection Enable bit */
#define  FMC_SR_FEMPT                      0x40U              /*!<FIFO empty                                  */

/******************  Bit definition for FMC_PMEM register  ******************/
#define  FMC_PMEM_MEMSET3                  0x000000FFU        /*!<MEMSET3[7:0] bits (Common memory 3 setup time) */
#define  FMC_PMEM_MEMSET3_0                0x00000001U        /*!<Bit 0 */
#define  FMC_PMEM_MEMSET3_1                0x00000002U        /*!<Bit 1 */
#define  FMC_PMEM_MEMSET3_2                0x00000004U        /*!<Bit 2 */
#define  FMC_PMEM_MEMSET3_3                0x00000008U        /*!<Bit 3 */
#define  FMC_PMEM_MEMSET3_4                0x00000010U        /*!<Bit 4 */
#define  FMC_PMEM_MEMSET3_5                0x00000020U        /*!<Bit 5 */
#define  FMC_PMEM_MEMSET3_6                0x00000040U        /*!<Bit 6 */
#define  FMC_PMEM_MEMSET3_7                0x00000080U        /*!<Bit 7 */
#define  FMC_PMEM_MEMWAIT3                 0x0000FF00U        /*!<MEMWAIT3[7:0] bits (Common memory 3 wait time) */
#define  FMC_PMEM_MEMWAIT3_0               0x00000100U        /*!<Bit 0 */
#define  FMC_PMEM_MEMWAIT3_1               0x00000200U        /*!<Bit 1 */
#define  FMC_PMEM_MEMWAIT3_2               0x00000400U        /*!<Bit 2 */
#define  FMC_PMEM_MEMWAIT3_3               0x00000800U        /*!<Bit 3 */
#define  FMC_PMEM_MEMWAIT3_4               0x00001000U        /*!<Bit 4 */
#define  FMC_PMEM_MEMWAIT3_5               0x00002000U        /*!<Bit 5 */
#define  FMC_PMEM_MEMWAIT3_6               0x00004000U        /*!<Bit 6 */
#define  FMC_PMEM_MEMWAIT3_7               0x00008000U        /*!<Bit 7 */
#define  FMC_PMEM_MEMHOLD3                 0x00FF0000U        /*!<MEMHOLD3[7:0] bits (Common memory 3 hold time) */
#define  FMC_PMEM_MEMHOLD3_0               0x00010000U        /*!<Bit 0 */
#define  FMC_PMEM_MEMHOLD3_1               0x00020000U        /*!<Bit 1 */
#define  FMC_PMEM_MEMHOLD3_2               0x00040000U        /*!<Bit 2 */
#define  FMC_PMEM_MEMHOLD3_3               0x00080000U        /*!<Bit 3 */
#define  FMC_PMEM_MEMHOLD3_4               0x00100000U        /*!<Bit 4 */
#define  FMC_PMEM_MEMHOLD3_5               0x00200000U        /*!<Bit 5 */
#define  FMC_PMEM_MEMHOLD3_6               0x00400000U        /*!<Bit 6 */
#define  FMC_PMEM_MEMHOLD3_7               0x00800000U        /*!<Bit 7 */
#define  FMC_PMEM_MEMHIZ3                  0xFF000000U        /*!<MEMHIZ3[7:0] bits (Common memory 3 databus HiZ time) */
#define  FMC_PMEM_MEMHIZ3_0                0x01000000U        /*!<Bit 0 */
#define  FMC_PMEM_MEMHIZ3_1                0x02000000U        /*!<Bit 1 */
#define  FMC_PMEM_MEMHIZ3_2                0x04000000U        /*!<Bit 2 */
#define  FMC_PMEM_MEMHIZ3_3                0x08000000U        /*!<Bit 3 */
#define  FMC_PMEM_MEMHIZ3_4                0x10000000U        /*!<Bit 4 */
#define  FMC_PMEM_MEMHIZ3_5                0x20000000U        /*!<Bit 5 */
#define  FMC_PMEM_MEMHIZ3_6                0x40000000U        /*!<Bit 6 */
#define  FMC_PMEM_MEMHIZ3_7                0x80000000U        /*!<Bit 7 */

/******************  Bit definition for FMC_PATT register  ******************/
#define  FMC_PATT_ATTSET3                  0x000000FFU        /*!<ATTSET3[7:0] bits (Attribute memory 3 setup time) */
#define  FMC_PATT_ATTSET3_0                0x00000001U        /*!<Bit 0 */
#define  FMC_PATT_ATTSET3_1                0x00000002U        /*!<Bit 1 */
#define  FMC_PATT_ATTSET3_2                0x00000004U        /*!<Bit 2 */
#define  FMC_PATT_ATTSET3_3                0x00000008U        /*!<Bit 3 */
#define  FMC_PATT_ATTSET3_4                0x00000010U        /*!<Bit 4 */
#define  FMC_PATT_ATTSET3_5                0x00000020U        /*!<Bit 5 */
#define  FMC_PATT_ATTSET3_6                0x00000040U        /*!<Bit 6 */
#define  FMC_PATT_ATTSET3_7                0x00000080U        /*!<Bit 7 */
#define  FMC_PATT_ATTWAIT3                 0x0000FF00U        /*!<ATTWAIT3[7:0] bits (Attribute memory 3 wait time) */
#define  FMC_PATT_ATTWAIT3_0               0x00000100U        /*!<Bit 0 */
#define  FMC_PATT_ATTWAIT3_1               0x00000200U        /*!<Bit 1 */
#define  FMC_PATT_ATTWAIT3_2               0x00000400U        /*!<Bit 2 */
#define  FMC_PATT_ATTWAIT3_3               0x00000800U        /*!<Bit 3 */
#define  FMC_PATT_ATTWAIT3_4               0x00001000U        /*!<Bit 4 */
#define  FMC_PATT_ATTWAIT3_5               0x00002000U        /*!<Bit 5 */
#define  FMC_PATT_ATTWAIT3_6               0x00004000U        /*!<Bit 6 */
#define  FMC_PATT_ATTWAIT3_7               0x00008000U        /*!<Bit 7 */
#define  FMC_PATT_ATTHOLD3                 0x00FF0000U        /*!<ATTHOLD3[7:0] bits (Attribute memory 3 hold time) */
#define  FMC_PATT_ATTHOLD3_0               0x00010000U        /*!<Bit 0 */
#define  FMC_PATT_ATTHOLD3_1               0x00020000U        /*!<Bit 1 */
#define  FMC_PATT_ATTHOLD3_2               0x00040000U        /*!<Bit 2 */
#define  FMC_PATT_ATTHOLD3_3               0x00080000U        /*!<Bit 3 */
#define  FMC_PATT_ATTHOLD3_4               0x00100000U        /*!<Bit 4 */
#define  FMC_PATT_ATTHOLD3_5               0x00200000U        /*!<Bit 5 */
#define  FMC_PATT_ATTHOLD3_6               0x00400000U        /*!<Bit 6 */
#define  FMC_PATT_ATTHOLD3_7               0x00800000U        /*!<Bit 7 */
#define  FMC_PATT_ATTHIZ3                  0xFF000000U        /*!<ATTHIZ3[7:0] bits (Attribute memory 3 databus HiZ time) */
#define  FMC_PATT_ATTHIZ3_0                0x01000000U        /*!<Bit 0 */
#define  FMC_PATT_ATTHIZ3_1                0x02000000U        /*!<Bit 1 */
#define  FMC_PATT_ATTHIZ3_2                0x04000000U        /*!<Bit 2 */
#define  FMC_PATT_ATTHIZ3_3                0x08000000U        /*!<Bit 3 */
#define  FMC_PATT_ATTHIZ3_4                0x10000000U        /*!<Bit 4 */
#define  FMC_PATT_ATTHIZ3_5                0x20000000U        /*!<Bit 5 */
#define  FMC_PATT_ATTHIZ3_6                0x40000000U        /*!<Bit 6 */
#define  FMC_PATT_ATTHIZ3_7                0x80000000U        /*!<Bit 7 */

/******************  Bit definition for FMC_ECCR register  ******************/
#define  FMC_ECCR_ECC3                     0xFFFFFFFFU        /*!<ECC result */

/******************  Bit definition for FMC_SDCR1 register  ******************/
#define  FMC_SDCR1_NC                       0x00000003U        /*!<NC[1:0] bits (Number of column bits) */
#define  FMC_SDCR1_NC_0                     0x00000001U        /*!<Bit 0 */
#define  FMC_SDCR1_NC_1                     0x00000002U        /*!<Bit 1 */
#define  FMC_SDCR1_NR                       0x0000000CU        /*!<NR[1:0] bits (Number of row bits) */
#define  FMC_SDCR1_NR_0                     0x00000004U        /*!<Bit 0 */
#define  FMC_SDCR1_NR_1                     0x00000008U        /*!<Bit 1 */
#define  FMC_SDCR1_MWID                     0x00000030U        /*!<NR[1:0] bits (Number of row bits) */
#define  FMC_SDCR1_MWID_0                   0x00000010U        /*!<Bit 0 */
#define  FMC_SDCR1_MWID_1                   0x00000020U        /*!<Bit 1 */
#define  FMC_SDCR1_NB                       0x00000040U        /*!<Number of internal bank */
#define  FMC_SDCR1_CAS                      0x00000180U        /*!<CAS[1:0] bits (CAS latency) */
#define  FMC_SDCR1_CAS_0                    0x00000080U        /*!<Bit 0 */
#define  FMC_SDCR1_CAS_1                    0x00000100U        /*!<Bit 1 */
#define  FMC_SDCR1_WP                       0x00000200U        /*!<Write protection */
#define  FMC_SDCR1_SDCLK                    0x00000C00U        /*!<SDRAM clock configuration */
#define  FMC_SDCR1_SDCLK_0                  0x00000400U        /*!<Bit 0 */
#define  FMC_SDCR1_SDCLK_1                  0x00000800U        /*!<Bit 1 */
#define  FMC_SDCR1_RBURST                   0x00001000U        /*!<Read burst */
#define  FMC_SDCR1_RPIPE                    0x00006000U        /*!<Write protection */
#define  FMC_SDCR1_RPIPE_0                  0x00002000U        /*!<Bit 0 */
#define  FMC_SDCR1_RPIPE_1                  0x00004000U        /*!<Bit 1 */

/******************  Bit definition for FMC_SDCR2 register  ******************/
#define  FMC_SDCR2_NC                       0x00000003U        /*!<NC[1:0] bits (Number of column bits) */
#define  FMC_SDCR2_NC_0                     0x00000001U        /*!<Bit 0 */
#define  FMC_SDCR2_NC_1                     0x00000002U        /*!<Bit 1 */
#define  FMC_SDCR2_NR                       0x0000000CU        /*!<NR[1:0] bits (Number of row bits) */
#define  FMC_SDCR2_NR_0                     0x00000004U        /*!<Bit 0 */
#define  FMC_SDCR2_NR_1                     0x00000008U        /*!<Bit 1 */
#define  FMC_SDCR2_MWID                     0x00000030U        /*!<NR[1:0] bits (Number of row bits) */
#define  FMC_SDCR2_MWID_0                   0x00000010U        /*!<Bit 0 */
#define  FMC_SDCR2_MWID_1                   0x00000020U        /*!<Bit 1 */
#define  FMC_SDCR2_NB                       0x00000040U        /*!<Number of internal bank */
#define  FMC_SDCR2_CAS                      0x00000180U        /*!<CAS[1:0] bits (CAS latency) */
#define  FMC_SDCR2_CAS_0                    0x00000080U        /*!<Bit 0 */
#define  FMC_SDCR2_CAS_1                    0x00000100U        /*!<Bit 1 */
#define  FMC_SDCR2_WP                       0x00000200U        /*!<Write protection */
#define  FMC_SDCR2_SDCLK                    0x00000C00U        /*!<SDCLK[1:0] (SDRAM clock configuration) */
#define  FMC_SDCR2_SDCLK_0                  0x00000400U        /*!<Bit 0 */
#define  FMC_SDCR2_SDCLK_1                  0x00000800U        /*!<Bit 1 */
#define  FMC_SDCR2_RBURST                   0x00001000U        /*!<Read burst */
#define  FMC_SDCR2_RPIPE                    0x00006000U        /*!<RPIPE[1:0](Read pipe) */
#define  FMC_SDCR2_RPIPE_0                  0x00002000U        /*!<Bit 0 */
#define  FMC_SDCR2_RPIPE_1                  0x00004000U        /*!<Bit 1 */

/******************  Bit definition for FMC_SDTR1 register  ******************/
#define  FMC_SDTR1_TMRD                     0x0000000FU        /*!<TMRD[3:0] bits (Load mode register to active) */
#define  FMC_SDTR1_TMRD_0                   0x00000001U        /*!<Bit 0 */
#define  FMC_SDTR1_TMRD_1                   0x00000002U        /*!<Bit 1 */
#define  FMC_SDTR1_TMRD_2                   0x00000004U        /*!<Bit 2 */
#define  FMC_SDTR1_TMRD_3                   0x00000008U        /*!<Bit 3 */
#define  FMC_SDTR1_TXSR                     0x000000F0U        /*!<TXSR[3:0] bits (Exit self refresh) */
#define  FMC_SDTR1_TXSR_0                   0x00000010U        /*!<Bit 0 */
#define  FMC_SDTR1_TXSR_1                   0x00000020U        /*!<Bit 1 */
#define  FMC_SDTR1_TXSR_2                   0x00000040U        /*!<Bit 2 */
#define  FMC_SDTR1_TXSR_3                   0x00000080U        /*!<Bit 3 */
#define  FMC_SDTR1_TRAS                     0x00000F00U        /*!<TRAS[3:0] bits (Self refresh time) */
#define  FMC_SDTR1_TRAS_0                   0x00000100U        /*!<Bit 0 */
#define  FMC_SDTR1_TRAS_1                   0x00000200U        /*!<Bit 1 */
#define  FMC_SDTR1_TRAS_2                   0x00000400U        /*!<Bit 2 */
#define  FMC_SDTR1_TRAS_3                   0x00000800U        /*!<Bit 3 */
#define  FMC_SDTR1_TRC                      0x0000F000U        /*!<TRC[2:0] bits (Row cycle delay) */
#define  FMC_SDTR1_TRC_0                    0x00001000U        /*!<Bit 0 */
#define  FMC_SDTR1_TRC_1                    0x00002000U        /*!<Bit 1 */
#define  FMC_SDTR1_TRC_2                    0x00004000U        /*!<Bit 2 */
#define  FMC_SDTR1_TWR                      0x000F0000U        /*!<TRC[2:0] bits (Write recovery delay) */
#define  FMC_SDTR1_TWR_0                    0x00010000U        /*!<Bit 0 */
#define  FMC_SDTR1_TWR_1                    0x00020000U        /*!<Bit 1 */
#define  FMC_SDTR1_TWR_2                    0x00040000U        /*!<Bit 2 */
#define  FMC_SDTR1_TRP                      0x00F00000U        /*!<TRP[2:0] bits (Row precharge delay) */
#define  FMC_SDTR1_TRP_0                    0x00100000U        /*!<Bit 0 */
#define  FMC_SDTR1_TRP_1                    0x00200000U        /*!<Bit 1 */
#define  FMC_SDTR1_TRP_2                    0x00400000U        /*!<Bit 2 */
#define  FMC_SDTR1_TRCD                     0x0F000000U        /*!<TRP[2:0] bits (Row to column delay) */
#define  FMC_SDTR1_TRCD_0                   0x01000000U        /*!<Bit 0 */
#define  FMC_SDTR1_TRCD_1                   0x02000000U        /*!<Bit 1 */
#define  FMC_SDTR1_TRCD_2                   0x04000000U        /*!<Bit 2 */

/******************  Bit definition for FMC_SDTR2 register  ******************/
#define  FMC_SDTR2_TMRD                     0x0000000FU        /*!<TMRD[3:0] bits (Load mode register to active) */
#define  FMC_SDTR2_TMRD_0                   0x00000001U        /*!<Bit 0 */
#define  FMC_SDTR2_TMRD_1                   0x00000002U        /*!<Bit 1 */
#define  FMC_SDTR2_TMRD_2                   0x00000004U        /*!<Bit 2 */
#define  FMC_SDTR2_TMRD_3                   0x00000008U        /*!<Bit 3 */
#define  FMC_SDTR2_TXSR                     0x000000F0U        /*!<TXSR[3:0] bits (Exit self refresh) */
#define  FMC_SDTR2_TXSR_0                   0x00000010U        /*!<Bit 0 */
#define  FMC_SDTR2_TXSR_1                   0x00000020U        /*!<Bit 1 */
#define  FMC_SDTR2_TXSR_2                   0x00000040U        /*!<Bit 2 */
#define  FMC_SDTR2_TXSR_3                   0x00000080U        /*!<Bit 3 */
#define  FMC_SDTR2_TRAS                     0x00000F00U        /*!<TRAS[3:0] bits (Self refresh time) */
#define  FMC_SDTR2_TRAS_0                   0x00000100U        /*!<Bit 0 */
#define  FMC_SDTR2_TRAS_1                   0x00000200U        /*!<Bit 1 */
#define  FMC_SDTR2_TRAS_2                   0x00000400U        /*!<Bit 2 */
#define  FMC_SDTR2_TRAS_3                   0x00000800U        /*!<Bit 3 */
#define  FMC_SDTR2_TRC                      0x0000F000U        /*!<TRC[2:0] bits (Row cycle delay) */
#define  FMC_SDTR2_TRC_0                    0x00001000U        /*!<Bit 0 */
#define  FMC_SDTR2_TRC_1                    0x00002000U        /*!<Bit 1 */
#define  FMC_SDTR2_TRC_2                    0x00004000U        /*!<Bit 2 */
#define  FMC_SDTR2_TWR                      0x000F0000U        /*!<TRC[2:0] bits (Write recovery delay) */
#define  FMC_SDTR2_TWR_0                    0x00010000U        /*!<Bit 0 */
#define  FMC_SDTR2_TWR_1                    0x00020000U        /*!<Bit 1 */
#define  FMC_SDTR2_TWR_2                    0x00040000U        /*!<Bit 2 */
#define  FMC_SDTR2_TRP                      0x00F00000U        /*!<TRP[2:0] bits (Row precharge delay) */
#define  FMC_SDTR2_TRP_0                    0x00100000U        /*!<Bit 0 */
#define  FMC_SDTR2_TRP_1                    0x00200000U        /*!<Bit 1 */
#define  FMC_SDTR2_TRP_2                    0x00400000U        /*!<Bit 2 */
#define  FMC_SDTR2_TRCD                     0x0F000000U        /*!<TRP[2:0] bits (Row to column delay) */
#define  FMC_SDTR2_TRCD_0                   0x01000000U        /*!<Bit 0 */
#define  FMC_SDTR2_TRCD_1                   0x02000000U        /*!<Bit 1 */
#define  FMC_SDTR2_TRCD_2                   0x04000000U        /*!<Bit 2 */

/******************  Bit definition for FMC_SDCMR register  ******************/
#define  FMC_SDCMR_MODE                     0x00000007U        /*!<MODE[2:0] bits (Command mode) */
#define  FMC_SDCMR_MODE_0                   0x00000001U        /*!<Bit 0 */
#define  FMC_SDCMR_MODE_1                   0x00000002U        /*!<Bit 1 */
#define  FMC_SDCMR_MODE_2                   0x00000003U        /*!<Bit 2 */
#define  FMC_SDCMR_CTB2                     0x00000008U        /*!<Command target 2 */
#define  FMC_SDCMR_CTB1                     0x00000010U        /*!<Command target 1 */
#define  FMC_SDCMR_NRFS                     0x000001E0U        /*!<NRFS[3:0] bits (Number of auto-refresh) */
#define  FMC_SDCMR_NRFS_0                   0x00000020U        /*!<Bit 0 */
#define  FMC_SDCMR_NRFS_1                   0x00000040U        /*!<Bit 1 */
#define  FMC_SDCMR_NRFS_2                   0x00000080U        /*!<Bit 2 */
#define  FMC_SDCMR_NRFS_3                   0x00000100U        /*!<Bit 3 */
#define  FMC_SDCMR_MRD                      0x003FFE00U        /*!<MRD[12:0] bits (Mode register definition) */

/******************  Bit definition for FMC_SDRTR register  ******************/
#define  FMC_SDRTR_CRE                      0x00000001U        /*!<Clear refresh error flag */
#define  FMC_SDRTR_COUNT                    0x00003FFEU        /*!<COUNT[12:0] bits (Refresh timer count) */
#define  FMC_SDRTR_REIE                     0x00004000U        /*!<RES interupt enable */

/******************  Bit definition for FMC_SDSR register  ******************/
#define  FMC_SDSR_RE                        0x00000001U        /*!<Refresh error flag */
#define  FMC_SDSR_MODES1                    0x00000006U        /*!<MODES1[1:0]bits (Status mode for bank 1) */
#define  FMC_SDSR_MODES1_0                  0x00000002U        /*!<Bit 0 */
#define  FMC_SDSR_MODES1_1                  0x00000004U        /*!<Bit 1 */
#define  FMC_SDSR_MODES2                    0x00000018U        /*!<MODES2[1:0]bits (Status mode for bank 2) */
#define  FMC_SDSR_MODES2_0                  0x00000008U        /*!<Bit 0 */
#define  FMC_SDSR_MODES2_1                  0x00000010U        /*!<Bit 1 */
#define  FMC_SDSR_BUSY                      0x00000020U        /*!<Busy status */

/******************************************************************************/
/*                                                                            */
/*                            General Purpose I/O                             */
/*                                                                            */
/******************************************************************************/
/******************  Bits definition for GPIO_MODER register  *****************/
#define GPIO_MODER_MODER0                    0x00000003U
#define GPIO_MODER_MODER0_0                  0x00000001U
#define GPIO_MODER_MODER0_1                  0x00000002U
#define GPIO_MODER_MODER1                    0x0000000CU
#define GPIO_MODER_MODER1_0                  0x00000004U
#define GPIO_MODER_MODER1_1                  0x00000008U
#define GPIO_MODER_MODER2                    0x00000030U
#define GPIO_MODER_MODER2_0                  0x00000010U
#define GPIO_MODER_MODER2_1                  0x00000020U
#define GPIO_MODER_MODER3                    0x000000C0U
#define GPIO_MODER_MODER3_0                  0x00000040U
#define GPIO_MODER_MODER3_1                  0x00000080U
#define GPIO_MODER_MODER4                    0x00000300U
#define GPIO_MODER_MODER4_0                  0x00000100U
#define GPIO_MODER_MODER4_1                  0x00000200U
#define GPIO_MODER_MODER5                    0x00000C00U
#define GPIO_MODER_MODER5_0                  0x00000400U
#define GPIO_MODER_MODER5_1                  0x00000800U
#define GPIO_MODER_MODER6                    0x00003000U
#define GPIO_MODER_MODER6_0                  0x00001000U
#define GPIO_MODER_MODER6_1                  0x00002000U
#define GPIO_MODER_MODER7                    0x0000C000U
#define GPIO_MODER_MODER7_0                  0x00004000U
#define GPIO_MODER_MODER7_1                  0x00008000U
#define GPIO_MODER_MODER8                    0x00030000U
#define GPIO_MODER_MODER8_0                  0x00010000U
#define GPIO_MODER_MODER8_1                  0x00020000U
#define GPIO_MODER_MODER9                    0x000C0000U
#define GPIO_MODER_MODER9_0                  0x00040000U
#define GPIO_MODER_MODER9_1                  0x00080000U
#define GPIO_MODER_MODER10                   0x00300000U
#define GPIO_MODER_MODER10_0                 0x00100000U
#define GPIO_MODER_MODER10_1                 0x00200000U
#define GPIO_MODER_MODER11                   0x00C00000U
#define GPIO_MODER_MODER11_0                 0x00400000U
#define GPIO_MODER_MODER11_1                 0x00800000U
#define GPIO_MODER_MODER12                   0x03000000U
#define GPIO_MODER_MODER12_0                 0x01000000U
#define GPIO_MODER_MODER12_1                 0x02000000U
#define GPIO_MODER_MODER13                   0x0C000000U
#define GPIO_MODER_MODER13_0                 0x04000000U
#define GPIO_MODER_MODER13_1                 0x08000000U
#define GPIO_MODER_MODER14                   0x30000000U
#define GPIO_MODER_MODER14_0                 0x10000000U
#define GPIO_MODER_MODER14_1                 0x20000000U
#define GPIO_MODER_MODER15                   0xC0000000U
#define GPIO_MODER_MODER15_0                 0x40000000U
#define GPIO_MODER_MODER15_1                 0x80000000U

/******************  Bits definition for GPIO_OTYPER register  ****************/
#define GPIO_OTYPER_OT_0                     0x00000001U
#define GPIO_OTYPER_OT_1                     0x00000002U
#define GPIO_OTYPER_OT_2                     0x00000004U
#define GPIO_OTYPER_OT_3                     0x00000008U
#define GPIO_OTYPER_OT_4                     0x00000010U
#define GPIO_OTYPER_OT_5                     0x00000020U
#define GPIO_OTYPER_OT_6                     0x00000040U
#define GPIO_OTYPER_OT_7                     0x00000080U
#define GPIO_OTYPER_OT_8                     0x00000100U
#define GPIO_OTYPER_OT_9                     0x00000200U
#define GPIO_OTYPER_OT_10                    0x00000400U
#define GPIO_OTYPER_OT_11                    0x00000800U
#define GPIO_OTYPER_OT_12                    0x00001000U
#define GPIO_OTYPER_OT_13                    0x00002000U
#define GPIO_OTYPER_OT_14                    0x00004000U
#define GPIO_OTYPER_OT_15                    0x00008000U

/******************  Bits definition for GPIO_OSPEEDR register  ***************/
#define GPIO_OSPEEDER_OSPEEDR0               0x00000003U
#define GPIO_OSPEEDER_OSPEEDR0_0             0x00000001U
#define GPIO_OSPEEDER_OSPEEDR0_1             0x00000002U
#define GPIO_OSPEEDER_OSPEEDR1               0x0000000CU
#define GPIO_OSPEEDER_OSPEEDR1_0             0x00000004U
#define GPIO_OSPEEDER_OSPEEDR1_1             0x00000008U
#define GPIO_OSPEEDER_OSPEEDR2               0x00000030U
#define GPIO_OSPEEDER_OSPEEDR2_0             0x00000010U
#define GPIO_OSPEEDER_OSPEEDR2_1             0x00000020U
#define GPIO_OSPEEDER_OSPEEDR3               0x000000C0U
#define GPIO_OSPEEDER_OSPEEDR3_0             0x00000040U
#define GPIO_OSPEEDER_OSPEEDR3_1             0x00000080U
#define GPIO_OSPEEDER_OSPEEDR4               0x00000300U
#define GPIO_OSPEEDER_OSPEEDR4_0             0x00000100U
#define GPIO_OSPEEDER_OSPEEDR4_1             0x00000200U
#define GPIO_OSPEEDER_OSPEEDR5               0x00000C00U
#define GPIO_OSPEEDER_OSPEEDR5_0             0x00000400U
#define GPIO_OSPEEDER_OSPEEDR5_1             0x00000800U
#define GPIO_OSPEEDER_OSPEEDR6               0x00003000U
#define GPIO_OSPEEDER_OSPEEDR6_0             0x00001000U
#define GPIO_OSPEEDER_OSPEEDR6_1             0x00002000U
#define GPIO_OSPEEDER_OSPEEDR7               0x0000C000U
#define GPIO_OSPEEDER_OSPEEDR7_0             0x00004000U
#define GPIO_OSPEEDER_OSPEEDR7_1             0x00008000U
#define GPIO_OSPEEDER_OSPEEDR8               0x00030000U
#define GPIO_OSPEEDER_OSPEEDR8_0             0x00010000U
#define GPIO_OSPEEDER_OSPEEDR8_1             0x00020000U
#define GPIO_OSPEEDER_OSPEEDR9               0x000C0000U
#define GPIO_OSPEEDER_OSPEEDR9_0             0x00040000U
#define GPIO_OSPEEDER_OSPEEDR9_1             0x00080000U
#define GPIO_OSPEEDER_OSPEEDR10              0x00300000U
#define GPIO_OSPEEDER_OSPEEDR10_0            0x00100000U
#define GPIO_OSPEEDER_OSPEEDR10_1            0x00200000U
#define GPIO_OSPEEDER_OSPEEDR11              0x00C00000U
#define GPIO_OSPEEDER_OSPEEDR11_0            0x00400000U
#define GPIO_OSPEEDER_OSPEEDR11_1            0x00800000U
#define GPIO_OSPEEDER_OSPEEDR12              0x03000000U
#define GPIO_OSPEEDER_OSPEEDR12_0            0x01000000U
#define GPIO_OSPEEDER_OSPEEDR12_1            0x02000000U
#define GPIO_OSPEEDER_OSPEEDR13              0x0C000000U
#define GPIO_OSPEEDER_OSPEEDR13_0            0x04000000U
#define GPIO_OSPEEDER_OSPEEDR13_1            0x08000000U
#define GPIO_OSPEEDER_OSPEEDR14              0x30000000U
#define GPIO_OSPEEDER_OSPEEDR14_0            0x10000000U
#define GPIO_OSPEEDER_OSPEEDR14_1            0x20000000U
#define GPIO_OSPEEDER_OSPEEDR15              0xC0000000U
#define GPIO_OSPEEDER_OSPEEDR15_0            0x40000000U
#define GPIO_OSPEEDER_OSPEEDR15_1            0x80000000U

/******************  Bits definition for GPIO_PUPDR register  *****************/
#define GPIO_PUPDR_PUPDR0                    0x00000003U
#define GPIO_PUPDR_PUPDR0_0                  0x00000001U
#define GPIO_PUPDR_PUPDR0_1                  0x00000002U
#define GPIO_PUPDR_PUPDR1                    0x0000000CU
#define GPIO_PUPDR_PUPDR1_0                  0x00000004U
#define GPIO_PUPDR_PUPDR1_1                  0x00000008U
#define GPIO_PUPDR_PUPDR2                    0x00000030U
#define GPIO_PUPDR_PUPDR2_0                  0x00000010U
#define GPIO_PUPDR_PUPDR2_1                  0x00000020U
#define GPIO_PUPDR_PUPDR3                    0x000000C0U
#define GPIO_PUPDR_PUPDR3_0                  0x00000040U
#define GPIO_PUPDR_PUPDR3_1                  0x00000080U
#define GPIO_PUPDR_PUPDR4                    0x00000300U
#define GPIO_PUPDR_PUPDR4_0                  0x00000100U
#define GPIO_PUPDR_PUPDR4_1                  0x00000200U
#define GPIO_PUPDR_PUPDR5                    0x00000C00U
#define GPIO_PUPDR_PUPDR5_0                  0x00000400U
#define GPIO_PUPDR_PUPDR5_1                  0x00000800U
#define GPIO_PUPDR_PUPDR6                    0x00003000U
#define GPIO_PUPDR_PUPDR6_0                  0x00001000U
#define GPIO_PUPDR_PUPDR6_1                  0x00002000U
#define GPIO_PUPDR_PUPDR7                    0x0000C000U
#define GPIO_PUPDR_PUPDR7_0                  0x00004000U
#define GPIO_PUPDR_PUPDR7_1                  0x00008000U
#define GPIO_PUPDR_PUPDR8                    0x00030000U
#define GPIO_PUPDR_PUPDR8_0                  0x00010000U
#define GPIO_PUPDR_PUPDR8_1                  0x00020000U
#define GPIO_PUPDR_PUPDR9                    0x000C0000U
#define GPIO_PUPDR_PUPDR9_0                  0x00040000U
#define GPIO_PUPDR_PUPDR9_1                  0x00080000U
#define GPIO_PUPDR_PUPDR10                   0x00300000U
#define GPIO_PUPDR_PUPDR10_0                 0x00100000U
#define GPIO_PUPDR_PUPDR10_1                 0x00200000U
#define GPIO_PUPDR_PUPDR11                   0x00C00000U
#define GPIO_PUPDR_PUPDR11_0                 0x00400000U
#define GPIO_PUPDR_PUPDR11_1                 0x00800000U
#define GPIO_PUPDR_PUPDR12                   0x03000000U
#define GPIO_PUPDR_PUPDR12_0                 0x01000000U
#define GPIO_PUPDR_PUPDR12_1                 0x02000000U
#define GPIO_PUPDR_PUPDR13                   0x0C000000U
#define GPIO_PUPDR_PUPDR13_0                 0x04000000U
#define GPIO_PUPDR_PUPDR13_1                 0x08000000U
#define GPIO_PUPDR_PUPDR14                   0x30000000U
#define GPIO_PUPDR_PUPDR14_0                 0x10000000U
#define GPIO_PUPDR_PUPDR14_1                 0x20000000U
#define GPIO_PUPDR_PUPDR15                   0xC0000000U
#define GPIO_PUPDR_PUPDR15_0                 0x40000000U
#define GPIO_PUPDR_PUPDR15_1                 0x80000000U

/******************  Bits definition for GPIO_IDR register  *******************/
#define GPIO_IDR_IDR_0                       0x00000001U
#define GPIO_IDR_IDR_1                       0x00000002U
#define GPIO_IDR_IDR_2                       0x00000004U
#define GPIO_IDR_IDR_3                       0x00000008U
#define GPIO_IDR_IDR_4                       0x00000010U
#define GPIO_IDR_IDR_5                       0x00000020U
#define GPIO_IDR_IDR_6                       0x00000040U
#define GPIO_IDR_IDR_7                       0x00000080U
#define GPIO_IDR_IDR_8                       0x00000100U
#define GPIO_IDR_IDR_9                       0x00000200U
#define GPIO_IDR_IDR_10                      0x00000400U
#define GPIO_IDR_IDR_11                      0x00000800U
#define GPIO_IDR_IDR_12                      0x00001000U
#define GPIO_IDR_IDR_13                      0x00002000U
#define GPIO_IDR_IDR_14                      0x00004000U
#define GPIO_IDR_IDR_15                      0x00008000U

/******************  Bits definition for GPIO_ODR register  *******************/
#define GPIO_ODR_ODR_0                       0x00000001U
#define GPIO_ODR_ODR_1                       0x00000002U
#define GPIO_ODR_ODR_2                       0x00000004U
#define GPIO_ODR_ODR_3                       0x00000008U
#define GPIO_ODR_ODR_4                       0x00000010U
#define GPIO_ODR_ODR_5                       0x00000020U
#define GPIO_ODR_ODR_6                       0x00000040U
#define GPIO_ODR_ODR_7                       0x00000080U
#define GPIO_ODR_ODR_8                       0x00000100U
#define GPIO_ODR_ODR_9                       0x00000200U
#define GPIO_ODR_ODR_10                      0x00000400U
#define GPIO_ODR_ODR_11                      0x00000800U
#define GPIO_ODR_ODR_12                      0x00001000U
#define GPIO_ODR_ODR_13                      0x00002000U
#define GPIO_ODR_ODR_14                      0x00004000U
#define GPIO_ODR_ODR_15                      0x00008000U

/******************  Bits definition for GPIO_BSRR register  ******************/
#define GPIO_BSRR_BS_0                       0x00000001U
#define GPIO_BSRR_BS_1                       0x00000002U
#define GPIO_BSRR_BS_2                       0x00000004U
#define GPIO_BSRR_BS_3                       0x00000008U
#define GPIO_BSRR_BS_4                       0x00000010U
#define GPIO_BSRR_BS_5                       0x00000020U
#define GPIO_BSRR_BS_6                       0x00000040U
#define GPIO_BSRR_BS_7                       0x00000080U
#define GPIO_BSRR_BS_8                       0x00000100U
#define GPIO_BSRR_BS_9                       0x00000200U
#define GPIO_BSRR_BS_10                      0x00000400U
#define GPIO_BSRR_BS_11                      0x00000800U
#define GPIO_BSRR_BS_12                      0x00001000U
#define GPIO_BSRR_BS_13                      0x00002000U
#define GPIO_BSRR_BS_14                      0x00004000U
#define GPIO_BSRR_BS_15                      0x00008000U
#define GPIO_BSRR_BR_0                       0x00010000U
#define GPIO_BSRR_BR_1                       0x00020000U
#define GPIO_BSRR_BR_2                       0x00040000U
#define GPIO_BSRR_BR_3                       0x00080000U
#define GPIO_BSRR_BR_4                       0x00100000U
#define GPIO_BSRR_BR_5                       0x00200000U
#define GPIO_BSRR_BR_6                       0x00400000U
#define GPIO_BSRR_BR_7                       0x00800000U
#define GPIO_BSRR_BR_8                       0x01000000U
#define GPIO_BSRR_BR_9                       0x02000000U
#define GPIO_BSRR_BR_10                      0x04000000U
#define GPIO_BSRR_BR_11                      0x08000000U
#define GPIO_BSRR_BR_12                      0x10000000U
#define GPIO_BSRR_BR_13                      0x20000000U
#define GPIO_BSRR_BR_14                      0x40000000U
#define GPIO_BSRR_BR_15                      0x80000000U

/****************** Bit definition for GPIO_LCKR register *********************/
#define GPIO_LCKR_LCK0                       0x00000001U
#define GPIO_LCKR_LCK1                       0x00000002U
#define GPIO_LCKR_LCK2                       0x00000004U
#define GPIO_LCKR_LCK3                       0x00000008U
#define GPIO_LCKR_LCK4                       0x00000010U
#define GPIO_LCKR_LCK5                       0x00000020U
#define GPIO_LCKR_LCK6                       0x00000040U
#define GPIO_LCKR_LCK7                       0x00000080U
#define GPIO_LCKR_LCK8                       0x00000100U
#define GPIO_LCKR_LCK9                       0x00000200U
#define GPIO_LCKR_LCK10                      0x00000400U
#define GPIO_LCKR_LCK11                      0x00000800U
#define GPIO_LCKR_LCK12                      0x00001000U
#define GPIO_LCKR_LCK13                      0x00002000U
#define GPIO_LCKR_LCK14                      0x00004000U
#define GPIO_LCKR_LCK15                      0x00008000U
#define GPIO_LCKR_LCKK                       0x00010000U


/******************************************************************************/
/*                                                                            */
/*                      Inter-integrated Circuit Interface (I2C)              */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for I2C_CR1 register  *******************/
#define  I2C_CR1_PE                          0x00000001U        /*!< Peripheral enable                   */
#define  I2C_CR1_TXIE                        0x00000002U        /*!< TX interrupt enable                 */
#define  I2C_CR1_RXIE                        0x00000004U        /*!< RX interrupt enable                 */
#define  I2C_CR1_ADDRIE                      0x00000008U        /*!< Address match interrupt enable      */
#define  I2C_CR1_NACKIE                      0x00000010U        /*!< NACK received interrupt enable      */
#define  I2C_CR1_STOPIE                      0x00000020U        /*!< STOP detection interrupt enable     */
#define  I2C_CR1_TCIE                        0x00000040U        /*!< Transfer complete interrupt enable  */
#define  I2C_CR1_ERRIE                       0x00000080U        /*!< Errors interrupt enable             */
#define  I2C_CR1_DNF                         0x00000F00U        /*!< Digital noise filter                */
#define  I2C_CR1_ANFOFF                      0x00001000U        /*!< Analog noise filter OFF             */
#define  I2C_CR1_TXDMAEN                     0x00004000U        /*!< DMA transmission requests enable    */
#define  I2C_CR1_RXDMAEN                     0x00008000U        /*!< DMA reception requests enable       */
#define  I2C_CR1_SBC                         0x00010000U        /*!< Slave byte control                  */
#define  I2C_CR1_NOSTRETCH                   0x00020000U        /*!< Clock stretching disable            */
#define  I2C_CR1_GCEN                        0x00080000U        /*!< General call enable                 */
#define  I2C_CR1_SMBHEN                      0x00100000U        /*!< SMBus host address enable           */
#define  I2C_CR1_SMBDEN                      0x00200000U        /*!< SMBus device default address enable */
#define  I2C_CR1_ALERTEN                     0x00400000U        /*!< SMBus alert enable                  */
#define  I2C_CR1_PECEN                       0x00800000U        /*!< PEC enable                          */

/* Legacy define */
#define  I2C_CR1_DFN                         I2C_CR1_DNF                   /*!< Digital noise filter                */

/******************  Bit definition for I2C_CR2 register  ********************/
#define  I2C_CR2_SADD                        0x000003FFU        /*!< Slave address (master mode)                             */
#define  I2C_CR2_RD_WRN                      0x00000400U        /*!< Transfer direction (master mode)                        */
#define  I2C_CR2_ADD10                       0x00000800U        /*!< 10-bit addressing mode (master mode)                    */
#define  I2C_CR2_HEAD10R                     0x00001000U        /*!< 10-bit address header only read direction (master mode) */
#define  I2C_CR2_START                       0x00002000U        /*!< START generation                                        */
#define  I2C_CR2_STOP                        0x00004000U        /*!< STOP generation (master mode)                           */
#define  I2C_CR2_NACK                        0x00008000U        /*!< NACK generation (slave mode)                            */
#define  I2C_CR2_NBYTES                      0x00FF0000U        /*!< Number of bytes                                         */
#define  I2C_CR2_RELOAD                      0x01000000U        /*!< NBYTES reload mode                                      */
#define  I2C_CR2_AUTOEND                     0x02000000U        /*!< Automatic end mode (master mode)                        */
#define  I2C_CR2_PECBYTE                     0x04000000U        /*!< Packet error checking byte                              */

/*******************  Bit definition for I2C_OAR1 register  ******************/
#define  I2C_OAR1_OA1                        0x000003FFU        /*!< Interface own address 1   */
#define  I2C_OAR1_OA1MODE                    0x00000400U        /*!< Own address 1 10-bit mode */
#define  I2C_OAR1_OA1EN                      0x00008000U        /*!< Own address 1 enable      */

/*******************  Bit definition for I2C_OAR2 register  ******************/
#define  I2C_OAR2_OA2                        0x000000FEU        /*!< Interface own address 2 */
#define  I2C_OAR2_OA2MSK                     0x00000700U        /*!< Own address 2 masks     */
#define  I2C_OAR2_OA2NOMASK                  0x00000000U        /*!< No mask */
#define  I2C_OAR2_OA2MASK01                  0x00000100U        /*!< OA2[1] is masked, Only OA2[7:2] are compared */
#define  I2C_OAR2_OA2MASK02                  0x00000200U        /*!< OA2[2:1] is masked, Only OA2[7:3] are compared */
#define  I2C_OAR2_OA2MASK03                  0x00000300U        /*!< OA2[3:1] is masked, Only OA2[7:4] are compared */
#define  I2C_OAR2_OA2MASK04                  0x00000400U        /*!< OA2[4:1] is masked, Only OA2[7:5] are compared */
#define  I2C_OAR2_OA2MASK05                  0x00000500U        /*!< OA2[5:1] is masked, Only OA2[7:6] are compared */
#define  I2C_OAR2_OA2MASK06                  0x00000600U        /*!< OA2[6:1] is masked, Only OA2[7] are compared */
#define  I2C_OAR2_OA2MASK07                  0x00000700U        /*!< OA2[7:1] is masked, No comparison is done */
#define  I2C_OAR2_OA2EN                      0x00008000U        /*!< Own address 2 enable    */

/*******************  Bit definition for I2C_TIMINGR register *******************/
#define  I2C_TIMINGR_SCLL                    0x000000FFU        /*!< SCL low period (master mode)  */
#define  I2C_TIMINGR_SCLH                    0x0000FF00U        /*!< SCL high period (master mode) */
#define  I2C_TIMINGR_SDADEL                  0x000F0000U        /*!< Data hold time                */
#define  I2C_TIMINGR_SCLDEL                  0x00F00000U        /*!< Data setup time               */
#define  I2C_TIMINGR_PRESC                   0xF0000000U        /*!< Timings prescaler             */

/******************* Bit definition for I2C_TIMEOUTR register *******************/
#define  I2C_TIMEOUTR_TIMEOUTA               0x00000FFFU        /*!< Bus timeout A                 */
#define  I2C_TIMEOUTR_TIDLE                  0x00001000U        /*!< Idle clock timeout detection  */
#define  I2C_TIMEOUTR_TIMOUTEN               0x00008000U        /*!< Clock timeout enable          */
#define  I2C_TIMEOUTR_TIMEOUTB               0x0FFF0000U        /*!< Bus timeout B                 */
#define  I2C_TIMEOUTR_TEXTEN                 0x80000000U        /*!< Extended clock timeout enable */

/******************  Bit definition for I2C_ISR register  *********************/
#define  I2C_ISR_TXE                         0x00000001U        /*!< Transmit data register empty    */
#define  I2C_ISR_TXIS                        0x00000002U        /*!< Transmit interrupt status       */
#define  I2C_ISR_RXNE                        0x00000004U        /*!< Receive data register not empty */
#define  I2C_ISR_ADDR                        0x00000008U        /*!< Address matched (slave mode)    */
#define  I2C_ISR_NACKF                       0x00000010U        /*!< NACK received flag              */
#define  I2C_ISR_STOPF                       0x00000020U        /*!< STOP detection flag             */
#define  I2C_ISR_TC                          0x00000040U        /*!< Transfer complete (master mode) */
#define  I2C_ISR_TCR                         0x00000080U        /*!< Transfer complete reload        */
#define  I2C_ISR_BERR                        0x00000100U        /*!< Bus error                       */
#define  I2C_ISR_ARLO                        0x00000200U        /*!< Arbitration lost                */
#define  I2C_ISR_OVR                         0x00000400U        /*!< Overrun/Underrun                */
#define  I2C_ISR_PECERR                      0x00000800U        /*!< PEC error in reception          */
#define  I2C_ISR_TIMEOUT                     0x00001000U        /*!< Timeout or Tlow detection flag  */
#define  I2C_ISR_ALERT                       0x00002000U        /*!< SMBus alert                     */
#define  I2C_ISR_BUSY                        0x00008000U        /*!< Bus busy                        */
#define  I2C_ISR_DIR                         0x00010000U        /*!< Transfer direction (slave mode) */
#define  I2C_ISR_ADDCODE                     0x00FE0000U        /*!< Address match code (slave mode) */

/******************  Bit definition for I2C_ICR register  *********************/
#define  I2C_ICR_ADDRCF                      0x00000008U        /*!< Address matched clear flag      */
#define  I2C_ICR_NACKCF                      0x00000010U        /*!< NACK clear flag                 */
#define  I2C_ICR_STOPCF                      0x00000020U        /*!< STOP detection clear flag       */
#define  I2C_ICR_BERRCF                      0x00000100U        /*!< Bus error clear flag            */
#define  I2C_ICR_ARLOCF                      0x00000200U        /*!< Arbitration lost clear flag     */
#define  I2C_ICR_OVRCF                       0x00000400U        /*!< Overrun/Underrun clear flag     */
#define  I2C_ICR_PECCF                       0x00000800U        /*!< PAC error clear flag            */
#define  I2C_ICR_TIMOUTCF                    0x00001000U        /*!< Timeout clear flag              */
#define  I2C_ICR_ALERTCF                     0x00002000U        /*!< Alert clear flag                */

/******************  Bit definition for I2C_PECR register  *********************/
#define  I2C_PECR_PEC                        0x000000FFU        /*!< PEC register        */

/******************  Bit definition for I2C_RXDR register  *********************/
#define  I2C_RXDR_RXDATA                     0x000000FFU        /*!< 8-bit receive data  */

/******************  Bit definition for I2C_TXDR register  *********************/
#define  I2C_TXDR_TXDATA                     0x000000FFU        /*!< 8-bit transmit data */


/******************************************************************************/
/*                                                                            */
/*                           Independent WATCHDOG                             */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for IWDG_KR register  ********************/
#define  IWDG_KR_KEY                         0xFFFFU            /*!<Key value (write only, read 0000h)  */

/*******************  Bit definition for IWDG_PR register  ********************/
#define  IWDG_PR_PR                          0x07U               /*!<PR[2:0] (Prescaler divider)         */
#define  IWDG_PR_PR_0                        0x01U               /*!<Bit 0 */
#define  IWDG_PR_PR_1                        0x02U               /*!<Bit 1 */
#define  IWDG_PR_PR_2                        0x04U               /*!<Bit 2 */

/*******************  Bit definition for IWDG_RLR register  *******************/
#define  IWDG_RLR_RL                         0x0FFFU            /*!<Watchdog counter reload value        */

/*******************  Bit definition for IWDG_SR register  ********************/
#define  IWDG_SR_PVU                         0x01U               /*!< Watchdog prescaler value update */
#define  IWDG_SR_RVU                         0x02U               /*!< Watchdog counter reload value update */
#define  IWDG_SR_WVU                         0x04U               /*!< Watchdog counter window value update */

/*******************  Bit definition for IWDG_KR register  ********************/
#define  IWDG_WINR_WIN                       0x0FFFU             /*!< Watchdog counter window value */

/******************************************************************************/
/*                                                                            */
/*                      LCD-TFT Display Controller (LTDC)                     */
/*                                                                            */
/******************************************************************************/

/********************  Bit definition for LTDC_SSCR register  *****************/

#define LTDC_SSCR_VSH                       0x000007FFU              /*!< Vertical Synchronization Height  */
#define LTDC_SSCR_HSW                       0x0FFF0000U              /*!< Horizontal Synchronization Width */

/********************  Bit definition for LTDC_BPCR register  *****************/

#define LTDC_BPCR_AVBP                      0x000007FFU              /*!< Accumulated Vertical Back Porch   */
#define LTDC_BPCR_AHBP                      0x0FFF0000U              /*!< Accumulated Horizontal Back Porch */

/********************  Bit definition for LTDC_AWCR register  *****************/

#define LTDC_AWCR_AAH                       0x000007FFU              /*!< Accumulated Active heigh */
#define LTDC_AWCR_AAW                       0x0FFF0000U              /*!< Accumulated Active Width */

/********************  Bit definition for LTDC_TWCR register  *****************/

#define LTDC_TWCR_TOTALH                    0x000007FFU              /*!< Total Heigh */
#define LTDC_TWCR_TOTALW                    0x0FFF0000U              /*!< Total Width */

/********************  Bit definition for LTDC_GCR register  ******************/

#define LTDC_GCR_LTDCEN                     0x00000001U              /*!< LCD-TFT controller enable bit       */
#define LTDC_GCR_DBW                        0x00000070U              /*!< Dither Blue Width                   */
#define LTDC_GCR_DGW                        0x00000700U              /*!< Dither Green Width                  */
#define LTDC_GCR_DRW                        0x00007000U              /*!< Dither Red Width                    */
#define LTDC_GCR_DEN                        0x00010000U              /*!< Dither Enable                       */
#define LTDC_GCR_PCPOL                      0x10000000U              /*!< Pixel Clock Polarity                */
#define LTDC_GCR_DEPOL                      0x20000000U              /*!< Data Enable Polarity                */
#define LTDC_GCR_VSPOL                      0x40000000U              /*!< Vertical Synchronization Polarity   */
#define LTDC_GCR_HSPOL                      0x80000000U              /*!< Horizontal Synchronization Polarity */

/* Legacy define */
#define LTDC_GCR_DTEN                       LTDC_GCR_DEN

/********************  Bit definition for LTDC_SRCR register  *****************/

#define LTDC_SRCR_IMR                      0x00000001U               /*!< Immediate Reload         */
#define LTDC_SRCR_VBR                      0x00000002U               /*!< Vertical Blanking Reload */

/********************  Bit definition for LTDC_BCCR register  *****************/

#define LTDC_BCCR_BCBLUE                    0x000000FFU              /*!< Background Blue value  */
#define LTDC_BCCR_BCGREEN                   0x0000FF00U              /*!< Background Green value */
#define LTDC_BCCR_BCRED                     0x00FF0000U              /*!< Background Red value   */

/********************  Bit definition for LTDC_IER register  ******************/

#define LTDC_IER_LIE                        0x00000001U              /*!< Line Interrupt Enable            */
#define LTDC_IER_FUIE                       0x00000002U              /*!< FIFO Underrun Interrupt Enable   */
#define LTDC_IER_TERRIE                     0x00000004U              /*!< Transfer Error Interrupt Enable  */
#define LTDC_IER_RRIE                       0x00000008U              /*!< Register Reload interrupt enable */

/********************  Bit definition for LTDC_ISR register  ******************/

#define LTDC_ISR_LIF                        0x00000001U              /*!< Line Interrupt Flag */
#define LTDC_ISR_FUIF                       0x00000002U              /*!< FIFO Underrun Interrupt Flag */
#define LTDC_ISR_TERRIF                     0x00000004U              /*!< Transfer Error Interrupt Flag */
#define LTDC_ISR_RRIF                       0x00000008U              /*!< Register Reload interrupt Flag */

/********************  Bit definition for LTDC_ICR register  ******************/

#define LTDC_ICR_CLIF                       0x00000001U              /*!< Clears the Line Interrupt Flag */
#define LTDC_ICR_CFUIF                      0x00000002U              /*!< Clears the FIFO Underrun Interrupt Flag */
#define LTDC_ICR_CTERRIF                    0x00000004U              /*!< Clears the Transfer Error Interrupt Flag */
#define LTDC_ICR_CRRIF                      0x00000008U              /*!< Clears Register Reload interrupt Flag */

/********************  Bit definition for LTDC_LIPCR register  ****************/

#define LTDC_LIPCR_LIPOS                    0x000007FFU              /*!< Line Interrupt Position */

/********************  Bit definition for LTDC_CPSR register  *****************/

#define LTDC_CPSR_CYPOS                     0x0000FFFFU              /*!< Current Y Position */
#define LTDC_CPSR_CXPOS                     0xFFFF0000U              /*!< Current X Position */

/********************  Bit definition for LTDC_CDSR register  *****************/

#define LTDC_CDSR_VDES                      0x00000001U              /*!< Vertical Data Enable Status       */
#define LTDC_CDSR_HDES                      0x00000002U              /*!< Horizontal Data Enable Status     */
#define LTDC_CDSR_VSYNCS                    0x00000004U              /*!< Vertical Synchronization Status   */
#define LTDC_CDSR_HSYNCS                    0x00000008U              /*!< Horizontal Synchronization Status */

/********************  Bit definition for LTDC_LxCR register  *****************/

#define LTDC_LxCR_LEN                       0x00000001U              /*!< Layer Enable              */
#define LTDC_LxCR_COLKEN                    0x00000002U              /*!< Color Keying Enable       */
#define LTDC_LxCR_CLUTEN                    0x00000010U              /*!< Color Lockup Table Enable */

/********************  Bit definition for LTDC_LxWHPCR register  **************/

#define LTDC_LxWHPCR_WHSTPOS                0x00000FFFU              /*!< Window Horizontal Start Position */
#define LTDC_LxWHPCR_WHSPPOS                0xFFFF0000U              /*!< Window Horizontal Stop Position  */

/********************  Bit definition for LTDC_LxWVPCR register  **************/

#define LTDC_LxWVPCR_WVSTPOS                0x00000FFFU              /*!< Window Vertical Start Position */
#define LTDC_LxWVPCR_WVSPPOS                0xFFFF0000U              /*!< Window Vertical Stop Position  */

/********************  Bit definition for LTDC_LxCKCR register  ***************/

#define LTDC_LxCKCR_CKBLUE                  0x000000FFU              /*!< Color Key Blue value  */
#define LTDC_LxCKCR_CKGREEN                 0x0000FF00U              /*!< Color Key Green value */
#define LTDC_LxCKCR_CKRED                   0x00FF0000U              /*!< Color Key Red value   */

/********************  Bit definition for LTDC_LxPFCR register  ***************/

#define LTDC_LxPFCR_PF                      0x00000007U              /*!< Pixel Format */

/********************  Bit definition for LTDC_LxCACR register  ***************/

#define LTDC_LxCACR_CONSTA                  0x000000FFU              /*!< Constant Alpha */

/********************  Bit definition for LTDC_LxDCCR register  ***************/

#define LTDC_LxDCCR_DCBLUE                  0x000000FFU              /*!< Default Color Blue  */
#define LTDC_LxDCCR_DCGREEN                 0x0000FF00U              /*!< Default Color Green */
#define LTDC_LxDCCR_DCRED                   0x00FF0000U              /*!< Default Color Red   */
#define LTDC_LxDCCR_DCALPHA                 0xFF000000U              /*!< Default Color Alpha */

/********************  Bit definition for LTDC_LxBFCR register  ***************/

#define LTDC_LxBFCR_BF2                     0x00000007U              /*!< Blending Factor 2 */
#define LTDC_LxBFCR_BF1                     0x00000700U              /*!< Blending Factor 1 */

/********************  Bit definition for LTDC_LxCFBAR register  **************/

#define LTDC_LxCFBAR_CFBADD                 0xFFFFFFFFU              /*!< Color Frame Buffer Start Address */

/********************  Bit definition for LTDC_LxCFBLR register  **************/

#define LTDC_LxCFBLR_CFBLL                  0x00001FFFU              /*!< Color Frame Buffer Line Length    */
#define LTDC_LxCFBLR_CFBP                   0x1FFF0000U              /*!< Color Frame Buffer Pitch in bytes */

/********************  Bit definition for LTDC_LxCFBLNR register  *************/

#define LTDC_LxCFBLNR_CFBLNBR               0x000007FFU              /*!< Frame Buffer Line Number */

/********************  Bit definition for LTDC_LxCLUTWR register  *************/

#define LTDC_LxCLUTWR_BLUE                  0x000000FFU              /*!< Blue value   */
#define LTDC_LxCLUTWR_GREEN                 0x0000FF00U              /*!< Green value  */
#define LTDC_LxCLUTWR_RED                   0x00FF0000U              /*!< Red value    */
#define LTDC_LxCLUTWR_CLUTADD               0xFF000000U              /*!< CLUT address */

/******************************************************************************/
/*                                                                            */
/*                             Power Control                                  */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for PWR_CR1 register  ********************/
#define  PWR_CR1_LPDS                         0x00000001U     /*!< Low-Power Deepsleep                 */
#define  PWR_CR1_PDDS                         0x00000002U     /*!< Power Down Deepsleep                */
#define  PWR_CR1_CSBF                         0x00000008U     /*!< Clear Standby Flag                  */
#define  PWR_CR1_PVDE                         0x00000010U     /*!< Power Voltage Detector Enable       */
#define  PWR_CR1_PLS                          0x000000E0U     /*!< PLS[2:0] bits (PVD Level Selection) */
#define  PWR_CR1_PLS_0                        0x00000020U     /*!< Bit 0 */
#define  PWR_CR1_PLS_1                        0x00000040U     /*!< Bit 1 */
#define  PWR_CR1_PLS_2                        0x00000080U     /*!< Bit 2 */

/*!< PVD level configuration */
#define  PWR_CR1_PLS_LEV0                     0x00000000U     /*!< PVD level 0 */
#define  PWR_CR1_PLS_LEV1                     0x00000020U     /*!< PVD level 1 */
#define  PWR_CR1_PLS_LEV2                     0x00000040U     /*!< PVD level 2 */
#define  PWR_CR1_PLS_LEV3                     0x00000060U     /*!< PVD level 3 */
#define  PWR_CR1_PLS_LEV4                     0x00000080U     /*!< PVD level 4 */
#define  PWR_CR1_PLS_LEV5                     0x000000A0U     /*!< PVD level 5 */
#define  PWR_CR1_PLS_LEV6                     0x000000C0U     /*!< PVD level 6 */
#define  PWR_CR1_PLS_LEV7                     0x000000E0U     /*!< PVD level 7 */
#define  PWR_CR1_DBP                          0x00000100U     /*!< Disable Backup Domain write protection                     */
#define  PWR_CR1_FPDS                         0x00000200U     /*!< Flash power down in Stop mode                              */
#define  PWR_CR1_LPUDS                        0x00000400U     /*!< Low-power regulator in deepsleep under-drive mode          */
#define  PWR_CR1_MRUDS                        0x00000800U     /*!< Main regulator in deepsleep under-drive mode               */
#define  PWR_CR1_ADCDC1                       0x00002000U     /*!< Refer to AN4073 on how to use this bit */
#define  PWR_CR1_VOS                          0x0000C000U     /*!< VOS[1:0] bits (Regulator voltage scaling output selection) */
#define  PWR_CR1_VOS_0                        0x00004000U     /*!< Bit 0 */
#define  PWR_CR1_VOS_1                        0x00008000U     /*!< Bit 1 */
#define  PWR_CR1_ODEN                         0x00010000U     /*!< Over Drive enable                   */
#define  PWR_CR1_ODSWEN                       0x00020000U     /*!< Over Drive switch enabled           */
#define  PWR_CR1_UDEN                         0x000C0000U     /*!< Under Drive enable in stop mode     */
#define  PWR_CR1_UDEN_0                       0x00040000U     /*!< Bit 0                               */
#define  PWR_CR1_UDEN_1                       0x00080000U     /*!< Bit 1                               */

/*******************  Bit definition for PWR_CSR1 register  ********************/
#define  PWR_CSR1_WUIF                        0x00000001U     /*!< Wake up internal Flag                            */
#define  PWR_CSR1_SBF                         0x00000002U     /*!< Standby Flag                                     */
#define  PWR_CSR1_PVDO                        0x00000004U     /*!< PVD Output                                       */
#define  PWR_CSR1_BRR                         0x00000008U     /*!< Backup regulator ready                           */
#define  PWR_CSR1_EIWUP                       0x00000100U     /*!< Enable internal wakeup                           */
#define  PWR_CSR1_BRE                         0x00000200U     /*!< Backup regulator enable                          */
#define  PWR_CSR1_VOSRDY                      0x00004000U     /*!< Regulator voltage scaling output selection ready */
#define  PWR_CSR1_ODRDY                       0x00010000U     /*!< Over Drive generator ready                       */
#define  PWR_CSR1_ODSWRDY                     0x00020000U     /*!< Over Drive Switch ready                          */
#define  PWR_CSR1_UDRDY                       0x000C0000U     /*!< Under Drive ready                                */

/* Legacy define */
#define  PWR_CSR1_UDSWRDY                     PWR_CSR1_UDRDY

/********************  Bit definition for PWR_CR2 register  ********************/
#define  PWR_CR2_CWUPF1                         0x00000001U     /*!< Clear Wakeup Pin Flag for PA0      */
#define  PWR_CR2_CWUPF2                         0x00000002U     /*!< Clear Wakeup Pin Flag for PA2      */
#define  PWR_CR2_CWUPF3                         0x00000004U     /*!< Clear Wakeup Pin Flag for PC1      */
#define  PWR_CR2_CWUPF4                         0x00000008U     /*!< Clear Wakeup Pin Flag for PC13     */
#define  PWR_CR2_CWUPF5                         0x00000010U     /*!< Clear Wakeup Pin Flag for PI8      */
#define  PWR_CR2_CWUPF6                         0x00000020U     /*!< Clear Wakeup Pin Flag for PI11     */
#define  PWR_CR2_WUPP1                          0x00000100U     /*!< Wakeup Pin Polarity bit for PA0    */
#define  PWR_CR2_WUPP2                          0x00000200U     /*!< Wakeup Pin Polarity bit for PA2    */
#define  PWR_CR2_WUPP3                          0x00000400U     /*!< Wakeup Pin Polarity bit for PC1    */
#define  PWR_CR2_WUPP4                          0x00000800U     /*!< Wakeup Pin Polarity bit for PC13   */
#define  PWR_CR2_WUPP5                          0x00001000U     /*!< Wakeup Pin Polarity bit for PI8    */
#define  PWR_CR2_WUPP6                          0x00002000U     /*!< Wakeup Pin Polarity bit for PI11   */

/*******************  Bit definition for PWR_CSR2 register  ********************/
#define  PWR_CSR2_WUPF1                         0x00000001U     /*!< Wakeup Pin Flag for PA0            */
#define  PWR_CSR2_WUPF2                         0x00000002U     /*!< Wakeup Pin Flag for PA2            */
#define  PWR_CSR2_WUPF3                         0x00000004U     /*!< Wakeup Pin Flag for PC1            */
#define  PWR_CSR2_WUPF4                         0x00000008U     /*!< Wakeup Pin Flag for PC13           */
#define  PWR_CSR2_WUPF5                         0x00000010U     /*!< Wakeup Pin Flag for PI8            */
#define  PWR_CSR2_WUPF6                         0x00000020U     /*!< Wakeup Pin Flag for PI11           */
#define  PWR_CSR2_EWUP1                         0x00000100U     /*!< Enable Wakeup Pin PA0              */
#define  PWR_CSR2_EWUP2                         0x00000200U     /*!< Enable Wakeup Pin PA2              */
#define  PWR_CSR2_EWUP3                         0x00000400U     /*!< Enable Wakeup Pin PC1              */
#define  PWR_CSR2_EWUP4                         0x00000800U     /*!< Enable Wakeup Pin PC13             */
#define  PWR_CSR2_EWUP5                         0x00001000U     /*!< Enable Wakeup Pin PI8              */
#define  PWR_CSR2_EWUP6                         0x00002000U     /*!< Enable Wakeup Pin PI11             */

/******************************************************************************/
/*                                                                            */
/*                                    QUADSPI                                 */
/*                                                                            */
/******************************************************************************/
/* QUADSPI IP version */
#define QSPI1_V1_0
/*****************  Bit definition for QUADSPI_CR register  *******************/
#define  QUADSPI_CR_EN                           0x00000001U            /*!< Enable                            */
#define  QUADSPI_CR_ABORT                        0x00000002U            /*!< Abort request                     */
#define  QUADSPI_CR_DMAEN                        0x00000004U            /*!< DMA Enable                        */
#define  QUADSPI_CR_TCEN                         0x00000008U            /*!< Timeout Counter Enable            */
#define  QUADSPI_CR_SSHIFT                       0x00000010U            /*!< Sample Shift                      */
#define  QUADSPI_CR_DFM                          0x00000040U            /*!< Dual Flash Mode                   */
#define  QUADSPI_CR_FSEL                         0x00000080U            /*!< Flash Select                      */
#define  QUADSPI_CR_FTHRES                       0x00001F00U            /*!< FTHRES[4:0] FIFO Level            */
#define  QUADSPI_CR_FTHRES_0                     0x00000100U            /*!< Bit 0 */
#define  QUADSPI_CR_FTHRES_1                     0x00000200U            /*!< Bit 1 */
#define  QUADSPI_CR_FTHRES_2                     0x00000400U            /*!< Bit 2 */
#define  QUADSPI_CR_FTHRES_3                     0x00000800U            /*!< Bit 3 */
#define  QUADSPI_CR_FTHRES_4                     0x00001000U            /*!< Bit 4 */
#define  QUADSPI_CR_TEIE                         0x00010000U            /*!< Transfer Error Interrupt Enable    */
#define  QUADSPI_CR_TCIE                         0x00020000U            /*!< Transfer Complete Interrupt Enable */
#define  QUADSPI_CR_FTIE                         0x00040000U            /*!< FIFO Threshold Interrupt Enable    */
#define  QUADSPI_CR_SMIE                         0x00080000U            /*!< Status Match Interrupt Enable      */
#define  QUADSPI_CR_TOIE                         0x00100000U            /*!< TimeOut Interrupt Enable           */
#define  QUADSPI_CR_APMS                         0x00400000U            /*!< Bit 1                              */
#define  QUADSPI_CR_PMM                          0x00800000U            /*!< Polling Match Mode                 */
#define  QUADSPI_CR_PRESCALER                    0xFF000000U            /*!< PRESCALER[7:0] Clock prescaler     */
#define  QUADSPI_CR_PRESCALER_0                  0x01000000U            /*!< Bit 0 */
#define  QUADSPI_CR_PRESCALER_1                  0x02000000U            /*!< Bit 1 */
#define  QUADSPI_CR_PRESCALER_2                  0x04000000U            /*!< Bit 2 */
#define  QUADSPI_CR_PRESCALER_3                  0x08000000U            /*!< Bit 3 */
#define  QUADSPI_CR_PRESCALER_4                  0x10000000U            /*!< Bit 4 */
#define  QUADSPI_CR_PRESCALER_5                  0x20000000U            /*!< Bit 5 */
#define  QUADSPI_CR_PRESCALER_6                  0x40000000U            /*!< Bit 6 */
#define  QUADSPI_CR_PRESCALER_7                  0x80000000U            /*!< Bit 7 */

/*****************  Bit definition for QUADSPI_DCR register  ******************/
#define  QUADSPI_DCR_CKMODE                      0x00000001U            /*!< Mode 0 / Mode 3                 */
#define  QUADSPI_DCR_CSHT                        0x00000700U            /*!< CSHT[2:0]: ChipSelect High Time */
#define  QUADSPI_DCR_CSHT_0                      0x00000100U            /*!< Bit 0 */
#define  QUADSPI_DCR_CSHT_1                      0x00000200U            /*!< Bit 1 */
#define  QUADSPI_DCR_CSHT_2                      0x00000400U            /*!< Bit 2 */
#define  QUADSPI_DCR_FSIZE                       0x001F0000U            /*!< FSIZE[4:0]: Flash Size          */
#define  QUADSPI_DCR_FSIZE_0                     0x00010000U            /*!< Bit 0 */
#define  QUADSPI_DCR_FSIZE_1                     0x00020000U            /*!< Bit 1 */
#define  QUADSPI_DCR_FSIZE_2                     0x00040000U            /*!< Bit 2 */
#define  QUADSPI_DCR_FSIZE_3                     0x00080000U            /*!< Bit 3 */
#define  QUADSPI_DCR_FSIZE_4                     0x00100000U            /*!< Bit 4 */

/******************  Bit definition for QUADSPI_SR register  *******************/
#define  QUADSPI_SR_TEF                          0x00000001U             /*!< Transfer Error Flag    */
#define  QUADSPI_SR_TCF                          0x00000002U             /*!< Transfer Complete Flag */
#define  QUADSPI_SR_FTF                          0x00000004U             /*!< FIFO Threshlod Flag    */
#define  QUADSPI_SR_SMF                          0x00000008U             /*!< Status Match Flag      */
#define  QUADSPI_SR_TOF                          0x00000010U             /*!< Timeout Flag           */
#define  QUADSPI_SR_BUSY                         0x00000020U             /*!< Busy                   */
#define  QUADSPI_SR_FLEVEL                       0x00001F00U             /*!< FIFO Threshlod Flag    */
#define  QUADSPI_SR_FLEVEL_0                     0x00000100U             /*!< Bit 0 */
#define  QUADSPI_SR_FLEVEL_1                     0x00000200U             /*!< Bit 1 */
#define  QUADSPI_SR_FLEVEL_2                     0x00000400U             /*!< Bit 2 */
#define  QUADSPI_SR_FLEVEL_3                     0x00000800U             /*!< Bit 3 */
#define  QUADSPI_SR_FLEVEL_4                     0x00001000U             /*!< Bit 4 */

/******************  Bit definition for QUADSPI_FCR register  ******************/
#define  QUADSPI_FCR_CTEF                        0x00000001U             /*!< Clear Transfer Error Flag    */
#define  QUADSPI_FCR_CTCF                        0x00000002U             /*!< Clear Transfer Complete Flag */
#define  QUADSPI_FCR_CSMF                        0x00000008U             /*!< Clear Status Match Flag      */
#define  QUADSPI_FCR_CTOF                        0x00000010U             /*!< Clear Timeout Flag           */

/******************  Bit definition for QUADSPI_DLR register  ******************/
#define  QUADSPI_DLR_DL                          0xFFFFFFFFU             /*!< DL[31:0]: Data Length */

/******************  Bit definition for QUADSPI_CCR register  ******************/
#define  QUADSPI_CCR_INSTRUCTION                  0x000000FFU            /*!< INSTRUCTION[7:0]: Instruction    */
#define  QUADSPI_CCR_INSTRUCTION_0                0x00000001U            /*!< Bit 0 */
#define  QUADSPI_CCR_INSTRUCTION_1                0x00000002U            /*!< Bit 1 */
#define  QUADSPI_CCR_INSTRUCTION_2                0x00000004U            /*!< Bit 2 */
#define  QUADSPI_CCR_INSTRUCTION_3                0x00000008U            /*!< Bit 3 */
#define  QUADSPI_CCR_INSTRUCTION_4                0x00000010U            /*!< Bit 4 */
#define  QUADSPI_CCR_INSTRUCTION_5                0x00000020U            /*!< Bit 5 */
#define  QUADSPI_CCR_INSTRUCTION_6                0x00000040U            /*!< Bit 6 */
#define  QUADSPI_CCR_INSTRUCTION_7                0x00000080U            /*!< Bit 7 */
#define  QUADSPI_CCR_IMODE                        0x00000300U            /*!< IMODE[1:0]: Instruction Mode      */
#define  QUADSPI_CCR_IMODE_0                      0x00000100U            /*!< Bit 0 */
#define  QUADSPI_CCR_IMODE_1                      0x00000200U            /*!< Bit 1 */
#define  QUADSPI_CCR_ADMODE                       0x00000C00U            /*!< ADMODE[1:0]: Address Mode         */
#define  QUADSPI_CCR_ADMODE_0                     0x00000400U            /*!< Bit 0 */
#define  QUADSPI_CCR_ADMODE_1                     0x00000800U            /*!< Bit 1 */
#define  QUADSPI_CCR_ADSIZE                       0x00003000U            /*!< ADSIZE[1:0]: Address Size         */
#define  QUADSPI_CCR_ADSIZE_0                     0x00001000U            /*!< Bit 0 */
#define  QUADSPI_CCR_ADSIZE_1                     0x00002000U            /*!< Bit 1 */
#define  QUADSPI_CCR_ABMODE                       0x0000C000U            /*!< ABMODE[1:0]: Alternate Bytes Mode */
#define  QUADSPI_CCR_ABMODE_0                     0x00004000U            /*!< Bit 0 */
#define  QUADSPI_CCR_ABMODE_1                     0x00008000U            /*!< Bit 1 */
#define  QUADSPI_CCR_ABSIZE                       0x00030000U            /*!< ABSIZE[1:0]: Instruction Mode     */
#define  QUADSPI_CCR_ABSIZE_0                     0x00010000U            /*!< Bit 0 */
#define  QUADSPI_CCR_ABSIZE_1                     0x00020000U            /*!< Bit 1 */
#define  QUADSPI_CCR_DCYC                         0x007C0000U            /*!< DCYC[4:0]: Dummy Cycles           */
#define  QUADSPI_CCR_DCYC_0                       0x00040000U            /*!< Bit 0 */
#define  QUADSPI_CCR_DCYC_1                       0x00080000U            /*!< Bit 1 */
#define  QUADSPI_CCR_DCYC_2                       0x00100000U            /*!< Bit 2 */
#define  QUADSPI_CCR_DCYC_3                       0x00200000U            /*!< Bit 3 */
#define  QUADSPI_CCR_DCYC_4                       0x00400000U            /*!< Bit 4 */
#define  QUADSPI_CCR_DMODE                        0x03000000U            /*!< DMODE[1:0]: Data Mode              */
#define  QUADSPI_CCR_DMODE_0                      0x01000000U            /*!< Bit 0 */
#define  QUADSPI_CCR_DMODE_1                      0x02000000U            /*!< Bit 1 */
#define  QUADSPI_CCR_FMODE                        0x0C000000U            /*!< FMODE[1:0]: Functional Mode        */
#define  QUADSPI_CCR_FMODE_0                      0x04000000U            /*!< Bit 0 */
#define  QUADSPI_CCR_FMODE_1                      0x08000000U            /*!< Bit 1 */
#define  QUADSPI_CCR_SIOO                         0x10000000U            /*!< SIOO: Send Instruction Only Once Mode */
#define  QUADSPI_CCR_DHHC                         0x40000000U            /*!< DHHC: Delay Half Hclk Cycle           */
#define  QUADSPI_CCR_DDRM                         0x80000000U            /*!< DDRM: Double Data Rate Mode           */
/******************  Bit definition for QUADSPI_AR register  *******************/
#define  QUADSPI_AR_ADDRESS                       0xFFFFFFFFU            /*!< ADDRESS[31:0]: Address */

/******************  Bit definition for QUADSPI_ABR register  ******************/
#define  QUADSPI_ABR_ALTERNATE                    0xFFFFFFFFU            /*!< ALTERNATE[31:0]: Alternate Bytes */

/******************  Bit definition for QUADSPI_DR register  *******************/
#define  QUADSPI_DR_DATA                          0xFFFFFFFFU            /*!< DATA[31:0]: Data */

/******************  Bit definition for QUADSPI_PSMKR register  ****************/
#define  QUADSPI_PSMKR_MASK                       0xFFFFFFFFU            /*!< MASK[31:0]: Status Mask */

/******************  Bit definition for QUADSPI_PSMAR register  ****************/
#define  QUADSPI_PSMAR_MATCH                      0xFFFFFFFFU            /*!< MATCH[31:0]: Status Match */

/******************  Bit definition for QUADSPI_PIR register  *****************/
#define  QUADSPI_PIR_INTERVAL                     0x0000FFFFU            /*!< INTERVAL[15:0]: Polling Interval */

/******************  Bit definition for QUADSPI_LPTR register  *****************/
#define  QUADSPI_LPTR_TIMEOUT                     0x0000FFFFU            /*!< TIMEOUT[15:0]: Timeout period */

/******************************************************************************/
/*                                                                            */
/*                         Reset and Clock Control            */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for RCC_CR register  ********************/
#define  RCC_CR_HSION                        0x00000001U
#define  RCC_CR_HSIRDY                       0x00000002U
#define  RCC_CR_HSITRIM                      0x000000F8U
#define  RCC_CR_HSITRIM_0                    0x00000008U /*!<Bit 0 */
#define  RCC_CR_HSITRIM_1                    0x00000010U /*!<Bit 1 */
#define  RCC_CR_HSITRIM_2                    0x00000020U /*!<Bit 2 */
#define  RCC_CR_HSITRIM_3                    0x00000040U /*!<Bit 3 */
#define  RCC_CR_HSITRIM_4                    0x00000080U /*!<Bit 4 */
#define  RCC_CR_HSICAL                       0x0000FF00U
#define  RCC_CR_HSICAL_0                     0x00000100U /*!<Bit 0 */
#define  RCC_CR_HSICAL_1                     0x00000200U /*!<Bit 1 */
#define  RCC_CR_HSICAL_2                     0x00000400U /*!<Bit 2 */
#define  RCC_CR_HSICAL_3                     0x00000800U /*!<Bit 3 */
#define  RCC_CR_HSICAL_4                     0x00001000U /*!<Bit 4 */
#define  RCC_CR_HSICAL_5                     0x00002000U /*!<Bit 5 */
#define  RCC_CR_HSICAL_6                     0x00004000U /*!<Bit 6 */
#define  RCC_CR_HSICAL_7                     0x00008000U /*!<Bit 7 */
#define  RCC_CR_HSEON                        0x00010000U
#define  RCC_CR_HSERDY                       0x00020000U
#define  RCC_CR_HSEBYP                       0x00040000U
#define  RCC_CR_CSSON                        0x00080000U
#define  RCC_CR_PLLON                        0x01000000U
#define  RCC_CR_PLLRDY                       0x02000000U
#define  RCC_CR_PLLI2SON                     0x04000000U
#define  RCC_CR_PLLI2SRDY                    0x08000000U
#define  RCC_CR_PLLSAION                     0x10000000U
#define  RCC_CR_PLLSAIRDY                    0x20000000U

/********************  Bit definition for RCC_PLLCFGR register  ***************/
#define  RCC_PLLCFGR_PLLM                    0x0000003FU
#define  RCC_PLLCFGR_PLLM_0                  0x00000001U
#define  RCC_PLLCFGR_PLLM_1                  0x00000002U
#define  RCC_PLLCFGR_PLLM_2                  0x00000004U
#define  RCC_PLLCFGR_PLLM_3                  0x00000008U
#define  RCC_PLLCFGR_PLLM_4                  0x00000010U
#define  RCC_PLLCFGR_PLLM_5                  0x00000020U
#define  RCC_PLLCFGR_PLLN                    0x00007FC0U
#define  RCC_PLLCFGR_PLLN_0                  0x00000040U
#define  RCC_PLLCFGR_PLLN_1                  0x00000080U
#define  RCC_PLLCFGR_PLLN_2                  0x00000100U
#define  RCC_PLLCFGR_PLLN_3                  0x00000200U
#define  RCC_PLLCFGR_PLLN_4                  0x00000400U
#define  RCC_PLLCFGR_PLLN_5                  0x00000800U
#define  RCC_PLLCFGR_PLLN_6                  0x00001000U
#define  RCC_PLLCFGR_PLLN_7                  0x00002000U
#define  RCC_PLLCFGR_PLLN_8                  0x00004000U
#define  RCC_PLLCFGR_PLLP                    0x00030000U
#define  RCC_PLLCFGR_PLLP_0                  0x00010000U
#define  RCC_PLLCFGR_PLLP_1                  0x00020000U
#define  RCC_PLLCFGR_PLLSRC                  0x00400000U
#define  RCC_PLLCFGR_PLLSRC_HSE              0x00400000U
#define  RCC_PLLCFGR_PLLSRC_HSI              0x00000000U
#define  RCC_PLLCFGR_PLLQ                    0x0F000000U
#define  RCC_PLLCFGR_PLLQ_0                  0x01000000U
#define  RCC_PLLCFGR_PLLQ_1                  0x02000000U
#define  RCC_PLLCFGR_PLLQ_2                  0x04000000U
#define  RCC_PLLCFGR_PLLQ_3                  0x08000000U


/********************  Bit definition for RCC_CFGR register  ******************/
/*!< SW configuration */
#define  RCC_CFGR_SW                         0x00000003U        /*!< SW[1:0] bits (System clock Switch) */
#define  RCC_CFGR_SW_0                       0x00000001U        /*!< Bit 0 */
#define  RCC_CFGR_SW_1                       0x00000002U        /*!< Bit 1 */
#define  RCC_CFGR_SW_HSI                     0x00000000U        /*!< HSI selected as system clock */
#define  RCC_CFGR_SW_HSE                     0x00000001U        /*!< HSE selected as system clock */
#define  RCC_CFGR_SW_PLL                     0x00000002U        /*!< PLL selected as system clock */

/*!< SWS configuration */
#define  RCC_CFGR_SWS                        0x0000000CU        /*!< SWS[1:0] bits (System Clock Switch Status) */
#define  RCC_CFGR_SWS_0                      0x00000004U        /*!< Bit 0 */
#define  RCC_CFGR_SWS_1                      0x00000008U        /*!< Bit 1 */
#define  RCC_CFGR_SWS_HSI                    0x00000000U        /*!< HSI oscillator used as system clock */
#define  RCC_CFGR_SWS_HSE                    0x00000004U        /*!< HSE oscillator used as system clock */
#define  RCC_CFGR_SWS_PLL                    0x00000008U        /*!< PLL used as system clock */

/*!< HPRE configuration */
#define  RCC_CFGR_HPRE                       0x000000F0U        /*!< HPRE[3:0] bits (AHB prescaler) */
#define  RCC_CFGR_HPRE_0                     0x00000010U        /*!< Bit 0 */
#define  RCC_CFGR_HPRE_1                     0x00000020U        /*!< Bit 1 */
#define  RCC_CFGR_HPRE_2                     0x00000040U        /*!< Bit 2 */
#define  RCC_CFGR_HPRE_3                     0x00000080U        /*!< Bit 3 */

#define  RCC_CFGR_HPRE_DIV1                  0x00000000U        /*!< SYSCLK not divided */
#define  RCC_CFGR_HPRE_DIV2                  0x00000080U        /*!< SYSCLK divided by 2 */
#define  RCC_CFGR_HPRE_DIV4                  0x00000090U        /*!< SYSCLK divided by 4 */
#define  RCC_CFGR_HPRE_DIV8                  0x000000A0U        /*!< SYSCLK divided by 8 */
#define  RCC_CFGR_HPRE_DIV16                 0x000000B0U        /*!< SYSCLK divided by 16 */
#define  RCC_CFGR_HPRE_DIV64                 0x000000C0U        /*!< SYSCLK divided by 64 */
#define  RCC_CFGR_HPRE_DIV128                0x000000D0U        /*!< SYSCLK divided by 128 */
#define  RCC_CFGR_HPRE_DIV256                0x000000E0U        /*!< SYSCLK divided by 256 */
#define  RCC_CFGR_HPRE_DIV512                0x000000F0U        /*!< SYSCLK divided by 512 */

/*!< PPRE1 configuration */
#define  RCC_CFGR_PPRE1                      0x00001C00U        /*!< PRE1[2:0] bits (APB1 prescaler) */
#define  RCC_CFGR_PPRE1_0                    0x00000400U        /*!< Bit 0 */
#define  RCC_CFGR_PPRE1_1                    0x00000800U        /*!< Bit 1 */
#define  RCC_CFGR_PPRE1_2                    0x00001000U        /*!< Bit 2 */

#define  RCC_CFGR_PPRE1_DIV1                 0x00000000U        /*!< HCLK not divided */
#define  RCC_CFGR_PPRE1_DIV2                 0x00001000U        /*!< HCLK divided by 2 */
#define  RCC_CFGR_PPRE1_DIV4                 0x00001400U        /*!< HCLK divided by 4 */
#define  RCC_CFGR_PPRE1_DIV8                 0x00001800U        /*!< HCLK divided by 8 */
#define  RCC_CFGR_PPRE1_DIV16                0x00001C00U        /*!< HCLK divided by 16 */

/*!< PPRE2 configuration */
#define  RCC_CFGR_PPRE2                      0x0000E000U        /*!< PRE2[2:0] bits (APB2 prescaler) */
#define  RCC_CFGR_PPRE2_0                    0x00002000U        /*!< Bit 0 */
#define  RCC_CFGR_PPRE2_1                    0x00004000U        /*!< Bit 1 */
#define  RCC_CFGR_PPRE2_2                    0x00008000U        /*!< Bit 2 */

#define  RCC_CFGR_PPRE2_DIV1                 0x00000000U        /*!< HCLK not divided */
#define  RCC_CFGR_PPRE2_DIV2                 0x00008000U        /*!< HCLK divided by 2 */
#define  RCC_CFGR_PPRE2_DIV4                 0x0000A000U        /*!< HCLK divided by 4 */
#define  RCC_CFGR_PPRE2_DIV8                 0x0000C000U        /*!< HCLK divided by 8 */
#define  RCC_CFGR_PPRE2_DIV16                0x0000E000U        /*!< HCLK divided by 16 */

/*!< RTCPRE configuration */
#define  RCC_CFGR_RTCPRE                     0x001F0000U
#define  RCC_CFGR_RTCPRE_0                   0x00010000U
#define  RCC_CFGR_RTCPRE_1                   0x00020000U
#define  RCC_CFGR_RTCPRE_2                   0x00040000U
#define  RCC_CFGR_RTCPRE_3                   0x00080000U
#define  RCC_CFGR_RTCPRE_4                   0x00100000U

/*!< MCO1 configuration */
#define  RCC_CFGR_MCO1                       0x00600000U
#define  RCC_CFGR_MCO1_0                     0x00200000U
#define  RCC_CFGR_MCO1_1                     0x00400000U

#define  RCC_CFGR_I2SSRC                     0x00800000U

#define  RCC_CFGR_MCO1PRE                    0x07000000U
#define  RCC_CFGR_MCO1PRE_0                  0x01000000U
#define  RCC_CFGR_MCO1PRE_1                  0x02000000U
#define  RCC_CFGR_MCO1PRE_2                  0x04000000U

#define  RCC_CFGR_MCO2PRE                    0x38000000U
#define  RCC_CFGR_MCO2PRE_0                  0x08000000U
#define  RCC_CFGR_MCO2PRE_1                  0x10000000U
#define  RCC_CFGR_MCO2PRE_2                  0x20000000U

#define  RCC_CFGR_MCO2                       0xC0000000U
#define  RCC_CFGR_MCO2_0                     0x40000000U
#define  RCC_CFGR_MCO2_1                     0x80000000U

/********************  Bit definition for RCC_CIR register  *******************/
#define  RCC_CIR_LSIRDYF                     0x00000001U
#define  RCC_CIR_LSERDYF                     0x00000002U
#define  RCC_CIR_HSIRDYF                     0x00000004U
#define  RCC_CIR_HSERDYF                     0x00000008U
#define  RCC_CIR_PLLRDYF                     0x00000010U
#define  RCC_CIR_PLLI2SRDYF                  0x00000020U
#define  RCC_CIR_PLLSAIRDYF                  0x00000040U
#define  RCC_CIR_CSSF                        0x00000080U
#define  RCC_CIR_LSIRDYIE                    0x00000100U
#define  RCC_CIR_LSERDYIE                    0x00000200U
#define  RCC_CIR_HSIRDYIE                    0x00000400U
#define  RCC_CIR_HSERDYIE                    0x00000800U
#define  RCC_CIR_PLLRDYIE                    0x00001000U
#define  RCC_CIR_PLLI2SRDYIE                 0x00002000U
#define  RCC_CIR_PLLSAIRDYIE                 0x00004000U
#define  RCC_CIR_LSIRDYC                     0x00010000U
#define  RCC_CIR_LSERDYC                     0x00020000U
#define  RCC_CIR_HSIRDYC                     0x00040000U
#define  RCC_CIR_HSERDYC                     0x00080000U
#define  RCC_CIR_PLLRDYC                     0x00100000U
#define  RCC_CIR_PLLI2SRDYC                  0x00200000U
#define  RCC_CIR_PLLSAIRDYC                  0x00400000U
#define  RCC_CIR_CSSC                        0x00800000U

/********************  Bit definition for RCC_AHB1RSTR register  **************/
#define  RCC_AHB1RSTR_GPIOARST               0x00000001U
#define  RCC_AHB1RSTR_GPIOBRST               0x00000002U
#define  RCC_AHB1RSTR_GPIOCRST               0x00000004U
#define  RCC_AHB1RSTR_GPIODRST               0x00000008U
#define  RCC_AHB1RSTR_GPIOERST               0x00000010U
#define  RCC_AHB1RSTR_GPIOFRST               0x00000020U
#define  RCC_AHB1RSTR_GPIOGRST               0x00000040U
#define  RCC_AHB1RSTR_GPIOHRST               0x00000080U
#define  RCC_AHB1RSTR_GPIOIRST               0x00000100U
#define  RCC_AHB1RSTR_GPIOJRST               0x00000200U
#define  RCC_AHB1RSTR_GPIOKRST               0x00000400U
#define  RCC_AHB1RSTR_CRCRST                 0x00001000U
#define  RCC_AHB1RSTR_DMA1RST                0x00200000U
#define  RCC_AHB1RSTR_DMA2RST                0x00400000U
#define  RCC_AHB1RSTR_DMA2DRST               0x00800000U
#define  RCC_AHB1RSTR_ETHMACRST              0x02000000U
#define  RCC_AHB1RSTR_OTGHRST                0x20000000U

/********************  Bit definition for RCC_AHB2RSTR register  **************/
#define  RCC_AHB2RSTR_DCMIRST                0x00000001U
#define  RCC_AHB2RSTR_RNGRST                 0x00000040U
#define  RCC_AHB2RSTR_OTGFSRST               0x00000080U

/********************  Bit definition for RCC_AHB3RSTR register  **************/

#define  RCC_AHB3RSTR_FMCRST                 0x00000001U
#define  RCC_AHB3RSTR_QSPIRST                0x00000002U

/********************  Bit definition for RCC_APB1RSTR register  **************/
#define  RCC_APB1RSTR_TIM2RST                0x00000001U
#define  RCC_APB1RSTR_TIM3RST                0x00000002U
#define  RCC_APB1RSTR_TIM4RST                0x00000004U
#define  RCC_APB1RSTR_TIM5RST                0x00000008U
#define  RCC_APB1RSTR_TIM6RST                0x00000010U
#define  RCC_APB1RSTR_TIM7RST                0x00000020U
#define  RCC_APB1RSTR_TIM12RST               0x00000040U
#define  RCC_APB1RSTR_TIM13RST               0x00000080U
#define  RCC_APB1RSTR_TIM14RST               0x00000100U
#define  RCC_APB1RSTR_LPTIM1RST              0x00000200U
#define  RCC_APB1RSTR_WWDGRST                0x00000800U
#define  RCC_APB1RSTR_SPI2RST                0x00004000U
#define  RCC_APB1RSTR_SPI3RST                0x00008000U
#define  RCC_APB1RSTR_SPDIFRXRST             0x00010000U
#define  RCC_APB1RSTR_USART2RST              0x00020000U
#define  RCC_APB1RSTR_USART3RST              0x00040000U
#define  RCC_APB1RSTR_UART4RST               0x00080000U
#define  RCC_APB1RSTR_UART5RST               0x00100000U
#define  RCC_APB1RSTR_I2C1RST                0x00200000U
#define  RCC_APB1RSTR_I2C2RST                0x00400000U
#define  RCC_APB1RSTR_I2C3RST                0x00800000U
#define  RCC_APB1RSTR_I2C4RST                0x01000000U
#define  RCC_APB1RSTR_CAN1RST                0x02000000U
#define  RCC_APB1RSTR_CAN2RST                0x04000000U
#define  RCC_APB1RSTR_CECRST                 0x08000000U
#define  RCC_APB1RSTR_PWRRST                 0x10000000U
#define  RCC_APB1RSTR_DACRST                 0x20000000U
#define  RCC_APB1RSTR_UART7RST               0x40000000U
#define  RCC_APB1RSTR_UART8RST               0x80000000U

/********************  Bit definition for RCC_APB2RSTR register  **************/
#define  RCC_APB2RSTR_TIM1RST                0x00000001U
#define  RCC_APB2RSTR_TIM8RST                0x00000002U
#define  RCC_APB2RSTR_USART1RST              0x00000010U
#define  RCC_APB2RSTR_USART6RST              0x00000020U
#define  RCC_APB2RSTR_ADCRST                 0x00000100U
#define  RCC_APB2RSTR_SDMMC1RST              0x00000800U
#define  RCC_APB2RSTR_SPI1RST                0x00001000U
#define  RCC_APB2RSTR_SPI4RST                0x00002000U
#define  RCC_APB2RSTR_SYSCFGRST              0x00004000U
#define  RCC_APB2RSTR_TIM9RST                0x00010000U
#define  RCC_APB2RSTR_TIM10RST               0x00020000U
#define  RCC_APB2RSTR_TIM11RST               0x00040000U
#define  RCC_APB2RSTR_SPI5RST                0x00100000U
#define  RCC_APB2RSTR_SPI6RST                0x00200000U
#define  RCC_APB2RSTR_SAI1RST                0x00400000U
#define  RCC_APB2RSTR_SAI2RST                0x00800000U
#define  RCC_APB2RSTR_LTDCRST                0x04000000U

/********************  Bit definition for RCC_AHB1ENR register  ***************/
#define  RCC_AHB1ENR_GPIOAEN                 0x00000001U
#define  RCC_AHB1ENR_GPIOBEN                 0x00000002U
#define  RCC_AHB1ENR_GPIOCEN                 0x00000004U
#define  RCC_AHB1ENR_GPIODEN                 0x00000008U
#define  RCC_AHB1ENR_GPIOEEN                 0x00000010U
#define  RCC_AHB1ENR_GPIOFEN                 0x00000020U
#define  RCC_AHB1ENR_GPIOGEN                 0x00000040U
#define  RCC_AHB1ENR_GPIOHEN                 0x00000080U
#define  RCC_AHB1ENR_GPIOIEN                 0x00000100U
#define  RCC_AHB1ENR_GPIOJEN                 0x00000200U
#define  RCC_AHB1ENR_GPIOKEN                 0x00000400U
#define  RCC_AHB1ENR_CRCEN                   0x00001000U
#define  RCC_AHB1ENR_BKPSRAMEN               0x00040000U
#define  RCC_AHB1ENR_DTCMRAMEN               0x00100000U
#define  RCC_AHB1ENR_DMA1EN                  0x00200000U
#define  RCC_AHB1ENR_DMA2EN                  0x00400000U
#define  RCC_AHB1ENR_DMA2DEN                 0x00800000U
#define  RCC_AHB1ENR_ETHMACEN                0x02000000U
#define  RCC_AHB1ENR_ETHMACTXEN              0x04000000U
#define  RCC_AHB1ENR_ETHMACRXEN              0x08000000U
#define  RCC_AHB1ENR_ETHMACPTPEN             0x10000000U
#define  RCC_AHB1ENR_OTGHSEN                 0x20000000U
#define  RCC_AHB1ENR_OTGHSULPIEN             0x40000000U

/********************  Bit definition for RCC_AHB2ENR register  ***************/
#define  RCC_AHB2ENR_DCMIEN                  0x00000001U
#define  RCC_AHB2ENR_RNGEN                   0x00000040U
#define  RCC_AHB2ENR_OTGFSEN                 0x00000080U

/********************  Bit definition for RCC_AHB3ENR register  ***************/
#define  RCC_AHB3ENR_FMCEN                  0x00000001U
#define  RCC_AHB3ENR_QSPIEN                 0x00000002U

/********************  Bit definition for RCC_APB1ENR register  ***************/
#define  RCC_APB1ENR_TIM2EN                  0x00000001U
#define  RCC_APB1ENR_TIM3EN                  0x00000002U
#define  RCC_APB1ENR_TIM4EN                  0x00000004U
#define  RCC_APB1ENR_TIM5EN                  0x00000008U
#define  RCC_APB1ENR_TIM6EN                  0x00000010U
#define  RCC_APB1ENR_TIM7EN                  0x00000020U
#define  RCC_APB1ENR_TIM12EN                 0x00000040U
#define  RCC_APB1ENR_TIM13EN                 0x00000080U
#define  RCC_APB1ENR_TIM14EN                 0x00000100U
#define  RCC_APB1ENR_LPTIM1EN                0x00000200U
#define  RCC_APB1ENR_WWDGEN                  0x00000800U
#define  RCC_APB1ENR_SPI2EN                  0x00004000U
#define  RCC_APB1ENR_SPI3EN                  0x00008000U
#define  RCC_APB1ENR_SPDIFRXEN               0x00010000U
#define  RCC_APB1ENR_USART2EN                0x00020000U
#define  RCC_APB1ENR_USART3EN                0x00040000U
#define  RCC_APB1ENR_UART4EN                 0x00080000U
#define  RCC_APB1ENR_UART5EN                 0x00100000U
#define  RCC_APB1ENR_I2C1EN                  0x00200000U
#define  RCC_APB1ENR_I2C2EN                  0x00400000U
#define  RCC_APB1ENR_I2C3EN                  0x00800000U
#define  RCC_APB1ENR_I2C4EN                  0x01000000U
#define  RCC_APB1ENR_CAN1EN                  0x02000000U
#define  RCC_APB1ENR_CAN2EN                  0x04000000U
#define  RCC_APB1ENR_CECEN                   0x08000000U
#define  RCC_APB1ENR_PWREN                   0x10000000U
#define  RCC_APB1ENR_DACEN                   0x20000000U
#define  RCC_APB1ENR_UART7EN                 0x40000000U
#define  RCC_APB1ENR_UART8EN                 0x80000000U

/********************  Bit definition for RCC_APB2ENR register  ***************/
#define  RCC_APB2ENR_TIM1EN                  0x00000001U
#define  RCC_APB2ENR_TIM8EN                  0x00000002U
#define  RCC_APB2ENR_USART1EN                0x00000010U
#define  RCC_APB2ENR_USART6EN                0x00000020U
#define  RCC_APB2ENR_ADC1EN                  0x00000100U
#define  RCC_APB2ENR_ADC2EN                  0x00000200U
#define  RCC_APB2ENR_ADC3EN                  0x00000400U
#define  RCC_APB2ENR_SDMMC1EN                0x00000800U
#define  RCC_APB2ENR_SPI1EN                  0x00001000U
#define  RCC_APB2ENR_SPI4EN                  0x00002000U
#define  RCC_APB2ENR_SYSCFGEN                0x00004000U
#define  RCC_APB2ENR_TIM9EN                  0x00010000U
#define  RCC_APB2ENR_TIM10EN                 0x00020000U
#define  RCC_APB2ENR_TIM11EN                 0x00040000U
#define  RCC_APB2ENR_SPI5EN                  0x00100000U
#define  RCC_APB2ENR_SPI6EN                  0x00200000U
#define  RCC_APB2ENR_SAI1EN                  0x00400000U
#define  RCC_APB2ENR_SAI2EN                  0x00800000U
#define  RCC_APB2ENR_LTDCEN                  0x04000000U

/********************  Bit definition for RCC_AHB1LPENR register  *************/
#define  RCC_AHB1LPENR_GPIOALPEN             0x00000001U
#define  RCC_AHB1LPENR_GPIOBLPEN             0x00000002U
#define  RCC_AHB1LPENR_GPIOCLPEN             0x00000004U
#define  RCC_AHB1LPENR_GPIODLPEN             0x00000008U
#define  RCC_AHB1LPENR_GPIOELPEN             0x00000010U
#define  RCC_AHB1LPENR_GPIOFLPEN             0x00000020U
#define  RCC_AHB1LPENR_GPIOGLPEN             0x00000040U
#define  RCC_AHB1LPENR_GPIOHLPEN             0x00000080U
#define  RCC_AHB1LPENR_GPIOILPEN             0x00000100U
#define  RCC_AHB1LPENR_GPIOJLPEN             0x00000200U
#define  RCC_AHB1LPENR_GPIOKLPEN             0x00000400U
#define  RCC_AHB1LPENR_CRCLPEN               0x00001000U
#define  RCC_AHB1LPENR_AXILPEN               0x00002000U
#define  RCC_AHB1LPENR_FLITFLPEN             0x00008000U
#define  RCC_AHB1LPENR_SRAM1LPEN             0x00010000U
#define  RCC_AHB1LPENR_SRAM2LPEN             0x00020000U
#define  RCC_AHB1LPENR_BKPSRAMLPEN           0x00040000U
#define  RCC_AHB1LPENR_DTCMLPEN              0x00100000U
#define  RCC_AHB1LPENR_DMA1LPEN              0x00200000U
#define  RCC_AHB1LPENR_DMA2LPEN              0x00400000U
#define  RCC_AHB1LPENR_DMA2DLPEN             0x00800000U
#define  RCC_AHB1LPENR_ETHMACLPEN            0x02000000U
#define  RCC_AHB1LPENR_ETHMACTXLPEN          0x04000000U
#define  RCC_AHB1LPENR_ETHMACRXLPEN          0x08000000U
#define  RCC_AHB1LPENR_ETHMACPTPLPEN         0x10000000U
#define  RCC_AHB1LPENR_OTGHSLPEN             0x20000000U
#define  RCC_AHB1LPENR_OTGHSULPILPEN         0x40000000U

/********************  Bit definition for RCC_AHB2LPENR register  *************/
#define  RCC_AHB2LPENR_DCMILPEN              0x00000001U
#define  RCC_AHB2LPENR_RNGLPEN               0x00000040U
#define  RCC_AHB2LPENR_OTGFSLPEN             0x00000080U

/********************  Bit definition for RCC_AHB3LPENR register  *************/
#define  RCC_AHB3LPENR_FMCLPEN               0x00000001U
#define  RCC_AHB3LPENR_QSPILPEN              0x00000002U
/********************  Bit definition for RCC_APB1LPENR register  *************/
#define  RCC_APB1LPENR_TIM2LPEN              0x00000001U
#define  RCC_APB1LPENR_TIM3LPEN              0x00000002U
#define  RCC_APB1LPENR_TIM4LPEN              0x00000004U
#define  RCC_APB1LPENR_TIM5LPEN              0x00000008U
#define  RCC_APB1LPENR_TIM6LPEN              0x00000010U
#define  RCC_APB1LPENR_TIM7LPEN              0x00000020U
#define  RCC_APB1LPENR_TIM12LPEN             0x00000040U
#define  RCC_APB1LPENR_TIM13LPEN             0x00000080U
#define  RCC_APB1LPENR_TIM14LPEN             0x00000100U
#define  RCC_APB1LPENR_LPTIM1LPEN            0x00000200U
#define  RCC_APB1LPENR_WWDGLPEN              0x00000800U
#define  RCC_APB1LPENR_SPI2LPEN              0x00004000U
#define  RCC_APB1LPENR_SPI3LPEN              0x00008000U
#define  RCC_APB1LPENR_SPDIFRXLPEN           0x00010000U
#define  RCC_APB1LPENR_USART2LPEN            0x00020000U
#define  RCC_APB1LPENR_USART3LPEN            0x00040000U
#define  RCC_APB1LPENR_UART4LPEN             0x00080000U
#define  RCC_APB1LPENR_UART5LPEN             0x00100000U
#define  RCC_APB1LPENR_I2C1LPEN              0x00200000U
#define  RCC_APB1LPENR_I2C2LPEN              0x00400000U
#define  RCC_APB1LPENR_I2C3LPEN              0x00800000U
#define  RCC_APB1LPENR_I2C4LPEN              0x01000000U
#define  RCC_APB1LPENR_CAN1LPEN              0x02000000U
#define  RCC_APB1LPENR_CAN2LPEN              0x04000000U
#define  RCC_APB1LPENR_CECLPEN               0x08000000U
#define  RCC_APB1LPENR_PWRLPEN               0x10000000U
#define  RCC_APB1LPENR_DACLPEN               0x20000000U
#define  RCC_APB1LPENR_UART7LPEN             0x40000000U
#define  RCC_APB1LPENR_UART8LPEN             0x80000000U

/********************  Bit definition for RCC_APB2LPENR register  *************/
#define  RCC_APB2LPENR_TIM1LPEN              0x00000001U
#define  RCC_APB2LPENR_TIM8LPEN              0x00000002U
#define  RCC_APB2LPENR_USART1LPEN            0x00000010U
#define  RCC_APB2LPENR_USART6LPEN            0x00000020U
#define  RCC_APB2LPENR_ADC1LPEN              0x00000100U
#define  RCC_APB2LPENR_ADC2LPEN              0x00000200U
#define  RCC_APB2LPENR_ADC3LPEN              0x00000400U
#define  RCC_APB2LPENR_SDMMC1LPEN            0x00000800U
#define  RCC_APB2LPENR_SPI1LPEN              0x00001000U
#define  RCC_APB2LPENR_SPI4LPEN              0x00002000U
#define  RCC_APB2LPENR_SYSCFGLPEN            0x00004000U
#define  RCC_APB2LPENR_TIM9LPEN              0x00010000U
#define  RCC_APB2LPENR_TIM10LPEN             0x00020000U
#define  RCC_APB2LPENR_TIM11LPEN             0x00040000U
#define  RCC_APB2LPENR_SPI5LPEN              0x00100000U
#define  RCC_APB2LPENR_SPI6LPEN              0x00200000U
#define  RCC_APB2LPENR_SAI1LPEN              0x00400000U
#define  RCC_APB2LPENR_SAI2LPEN              0x00800000U
#define  RCC_APB2LPENR_LTDCLPEN              0x04000000U

/********************  Bit definition for RCC_BDCR register  ******************/
#define  RCC_BDCR_LSEON                      0x00000001U
#define  RCC_BDCR_LSERDY                     0x00000002U
#define  RCC_BDCR_LSEBYP                     0x00000004U
#define  RCC_BDCR_LSEDRV                     0x00000018U
#define  RCC_BDCR_LSEDRV_0                   0x00000008U
#define  RCC_BDCR_LSEDRV_1                   0x00000010U
#define  RCC_BDCR_RTCSEL                     0x00000300U
#define  RCC_BDCR_RTCSEL_0                   0x00000100U
#define  RCC_BDCR_RTCSEL_1                   0x00000200U
#define  RCC_BDCR_RTCEN                      0x00008000U
#define  RCC_BDCR_BDRST                      0x00010000U

/********************  Bit definition for RCC_CSR register  *******************/
#define  RCC_CSR_LSION                       0x00000001U
#define  RCC_CSR_LSIRDY                      0x00000002U
#define  RCC_CSR_RMVF                        0x01000000U
#define  RCC_CSR_BORRSTF                     0x02000000U
#define  RCC_CSR_PINRSTF                     0x04000000U
#define  RCC_CSR_PORRSTF                     0x08000000U
#define  RCC_CSR_SFTRSTF                     0x10000000U
#define  RCC_CSR_IWDGRSTF                    0x20000000U
#define  RCC_CSR_WWDGRSTF                    0x40000000U
#define  RCC_CSR_LPWRRSTF                    0x80000000U

/********************  Bit definition for RCC_SSCGR register  *****************/
#define  RCC_SSCGR_MODPER                    0x00001FFFU
#define  RCC_SSCGR_INCSTEP                   0x0FFFE000U
#define  RCC_SSCGR_SPREADSEL                 0x40000000U
#define  RCC_SSCGR_SSCGEN                    0x80000000U

/********************  Bit definition for RCC_PLLI2SCFGR register  ************/
#define  RCC_PLLI2SCFGR_PLLI2SN              0x00007FC0U
#define  RCC_PLLI2SCFGR_PLLI2SN_0            0x00000040U
#define  RCC_PLLI2SCFGR_PLLI2SN_1            0x00000080U
#define  RCC_PLLI2SCFGR_PLLI2SN_2            0x00000100U
#define  RCC_PLLI2SCFGR_PLLI2SN_3            0x00000200U
#define  RCC_PLLI2SCFGR_PLLI2SN_4            0x00000400U
#define  RCC_PLLI2SCFGR_PLLI2SN_5            0x00000800U
#define  RCC_PLLI2SCFGR_PLLI2SN_6            0x00001000U
#define  RCC_PLLI2SCFGR_PLLI2SN_7            0x00002000U
#define  RCC_PLLI2SCFGR_PLLI2SN_8            0x00004000U
#define  RCC_PLLI2SCFGR_PLLI2SP              0x00030000U
#define  RCC_PLLI2SCFGR_PLLI2SP_0            0x00010000U
#define  RCC_PLLI2SCFGR_PLLI2SP_1            0x00020000U
#define  RCC_PLLI2SCFGR_PLLI2SQ              0x0F000000U
#define  RCC_PLLI2SCFGR_PLLI2SQ_0            0x01000000U
#define  RCC_PLLI2SCFGR_PLLI2SQ_1            0x02000000U
#define  RCC_PLLI2SCFGR_PLLI2SQ_2            0x04000000U
#define  RCC_PLLI2SCFGR_PLLI2SQ_3            0x08000000U
#define  RCC_PLLI2SCFGR_PLLI2SR              0x70000000U
#define  RCC_PLLI2SCFGR_PLLI2SR_0            0x10000000U
#define  RCC_PLLI2SCFGR_PLLI2SR_1            0x20000000U
#define  RCC_PLLI2SCFGR_PLLI2SR_2            0x40000000U

/********************  Bit definition for RCC_PLLSAICFGR register  ************/
#define  RCC_PLLSAICFGR_PLLSAIN              0x00007FC0U
#define  RCC_PLLSAICFGR_PLLSAIN_0            0x00000040U
#define  RCC_PLLSAICFGR_PLLSAIN_1            0x00000080U
#define  RCC_PLLSAICFGR_PLLSAIN_2            0x00000100U
#define  RCC_PLLSAICFGR_PLLSAIN_3            0x00000200U
#define  RCC_PLLSAICFGR_PLLSAIN_4            0x00000400U
#define  RCC_PLLSAICFGR_PLLSAIN_5            0x00000800U
#define  RCC_PLLSAICFGR_PLLSAIN_6            0x00001000U
#define  RCC_PLLSAICFGR_PLLSAIN_7            0x00002000U
#define  RCC_PLLSAICFGR_PLLSAIN_8            0x00004000U
#define  RCC_PLLSAICFGR_PLLSAIP              0x00030000U
#define  RCC_PLLSAICFGR_PLLSAIP_0            0x00010000U
#define  RCC_PLLSAICFGR_PLLSAIP_1            0x00020000U
#define  RCC_PLLSAICFGR_PLLSAIQ              0x0F000000U
#define  RCC_PLLSAICFGR_PLLSAIQ_0            0x01000000U
#define  RCC_PLLSAICFGR_PLLSAIQ_1            0x02000000U
#define  RCC_PLLSAICFGR_PLLSAIQ_2            0x04000000U
#define  RCC_PLLSAICFGR_PLLSAIQ_3            0x08000000U
#define  RCC_PLLSAICFGR_PLLSAIR              0x70000000U
#define  RCC_PLLSAICFGR_PLLSAIR_0            0x10000000U
#define  RCC_PLLSAICFGR_PLLSAIR_1            0x20000000U
#define  RCC_PLLSAICFGR_PLLSAIR_2            0x40000000U

/********************  Bit definition for RCC_DCKCFGR1 register  ***************/
#define  RCC_DCKCFGR1_PLLI2SDIVQ             0x0000001FU
#define  RCC_DCKCFGR1_PLLI2SDIVQ_0           0x00000001U
#define  RCC_DCKCFGR1_PLLI2SDIVQ_1           0x00000002U
#define  RCC_DCKCFGR1_PLLI2SDIVQ_2           0x00000004U
#define  RCC_DCKCFGR1_PLLI2SDIVQ_3           0x00000008U
#define  RCC_DCKCFGR1_PLLI2SDIVQ_4           0x00000010U

#define  RCC_DCKCFGR1_PLLSAIDIVQ             0x00001F00U
#define  RCC_DCKCFGR1_PLLSAIDIVQ_0           0x00000100U
#define  RCC_DCKCFGR1_PLLSAIDIVQ_1           0x00000200U
#define  RCC_DCKCFGR1_PLLSAIDIVQ_2           0x00000400U
#define  RCC_DCKCFGR1_PLLSAIDIVQ_3           0x00000800U
#define  RCC_DCKCFGR1_PLLSAIDIVQ_4           0x00001000U

#define  RCC_DCKCFGR1_PLLSAIDIVR             0x00030000U
#define  RCC_DCKCFGR1_PLLSAIDIVR_0           0x00010000U
#define  RCC_DCKCFGR1_PLLSAIDIVR_1           0x00020000U

#define  RCC_DCKCFGR1_SAI1SEL                0x00300000U
#define  RCC_DCKCFGR1_SAI1SEL_0              0x00100000U
#define  RCC_DCKCFGR1_SAI1SEL_1              0x00200000U

#define  RCC_DCKCFGR1_SAI2SEL                0x00C00000U
#define  RCC_DCKCFGR1_SAI2SEL_0              0x00400000U
#define  RCC_DCKCFGR1_SAI2SEL_1              0x00800000U

#define  RCC_DCKCFGR1_TIMPRE                 0x01000000U

/********************  Bit definition for RCC_DCKCFGR2 register  ***************/
#define  RCC_DCKCFGR2_USART1SEL              0x00000003U
#define  RCC_DCKCFGR2_USART1SEL_0            0x00000001U
#define  RCC_DCKCFGR2_USART1SEL_1            0x00000002U
#define  RCC_DCKCFGR2_USART2SEL              0x0000000CU
#define  RCC_DCKCFGR2_USART2SEL_0            0x00000004U
#define  RCC_DCKCFGR2_USART2SEL_1            0x00000008U
#define  RCC_DCKCFGR2_USART3SEL              0x00000030U
#define  RCC_DCKCFGR2_USART3SEL_0            0x00000010U
#define  RCC_DCKCFGR2_USART3SEL_1            0x00000020U
#define  RCC_DCKCFGR2_UART4SEL               0x000000C0U
#define  RCC_DCKCFGR2_UART4SEL_0             0x00000040U
#define  RCC_DCKCFGR2_UART4SEL_1             0x00000080U
#define  RCC_DCKCFGR2_UART5SEL               0x00000300U
#define  RCC_DCKCFGR2_UART5SEL_0             0x00000100U
#define  RCC_DCKCFGR2_UART5SEL_1             0x00000200U
#define  RCC_DCKCFGR2_USART6SEL              0x00000C00U
#define  RCC_DCKCFGR2_USART6SEL_0            0x00000400U
#define  RCC_DCKCFGR2_USART6SEL_1            0x00000800U
#define  RCC_DCKCFGR2_UART7SEL               0x00003000U
#define  RCC_DCKCFGR2_UART7SEL_0             0x00001000U
#define  RCC_DCKCFGR2_UART7SEL_1             0x00002000U
#define  RCC_DCKCFGR2_UART8SEL               0x0000C000U
#define  RCC_DCKCFGR2_UART8SEL_0             0x00004000U
#define  RCC_DCKCFGR2_UART8SEL_1             0x00008000U
#define  RCC_DCKCFGR2_I2C1SEL                0x00030000U
#define  RCC_DCKCFGR2_I2C1SEL_0              0x00010000U
#define  RCC_DCKCFGR2_I2C1SEL_1              0x00020000U
#define  RCC_DCKCFGR2_I2C2SEL                0x000C0000U
#define  RCC_DCKCFGR2_I2C2SEL_0              0x00040000U
#define  RCC_DCKCFGR2_I2C2SEL_1              0x00080000U
#define  RCC_DCKCFGR2_I2C3SEL                0x00300000U
#define  RCC_DCKCFGR2_I2C3SEL_0              0x00100000U
#define  RCC_DCKCFGR2_I2C3SEL_1              0x00200000U
#define  RCC_DCKCFGR2_I2C4SEL                0x00C00000U
#define  RCC_DCKCFGR2_I2C4SEL_0              0x00400000U
#define  RCC_DCKCFGR2_I2C4SEL_1              0x00800000U
#define  RCC_DCKCFGR2_LPTIM1SEL              0x03000000U
#define  RCC_DCKCFGR2_LPTIM1SEL_0            0x01000000U
#define  RCC_DCKCFGR2_LPTIM1SEL_1            0x02000000U
#define  RCC_DCKCFGR2_CECSEL                 0x04000000U
#define  RCC_DCKCFGR2_CK48MSEL               0x08000000U
#define  RCC_DCKCFGR2_SDMMC1SEL              0x10000000U

/******************************************************************************/
/*                                                                            */
/*                                    RNG                                     */
/*                                                                            */
/******************************************************************************/
/********************  Bits definition for RNG_CR register  *******************/
#define RNG_CR_RNGEN                         0x00000004U
#define RNG_CR_IE                            0x00000008U

/********************  Bits definition for RNG_SR register  *******************/
#define RNG_SR_DRDY                          0x00000001U
#define RNG_SR_CECS                          0x00000002U
#define RNG_SR_SECS                          0x00000004U
#define RNG_SR_CEIS                          0x00000020U
#define RNG_SR_SEIS                          0x00000040U

/******************************************************************************/
/*                                                                            */
/*                           Real-Time Clock (RTC)                            */
/*                                                                            */
/******************************************************************************/
/********************  Bits definition for RTC_TR register  *******************/
#define RTC_TR_PM                            0x00400000U
#define RTC_TR_HT                            0x00300000U
#define RTC_TR_HT_0                          0x00100000U
#define RTC_TR_HT_1                          0x00200000U
#define RTC_TR_HU                            0x000F0000U
#define RTC_TR_HU_0                          0x00010000U
#define RTC_TR_HU_1                          0x00020000U
#define RTC_TR_HU_2                          0x00040000U
#define RTC_TR_HU_3                          0x00080000U
#define RTC_TR_MNT                           0x00007000U
#define RTC_TR_MNT_0                         0x00001000U
#define RTC_TR_MNT_1                         0x00002000U
#define RTC_TR_MNT_2                         0x00004000U
#define RTC_TR_MNU                           0x00000F00U
#define RTC_TR_MNU_0                         0x00000100U
#define RTC_TR_MNU_1                         0x00000200U
#define RTC_TR_MNU_2                         0x00000400U
#define RTC_TR_MNU_3                         0x00000800U
#define RTC_TR_ST                            0x00000070U
#define RTC_TR_ST_0                          0x00000010U
#define RTC_TR_ST_1                          0x00000020U
#define RTC_TR_ST_2                          0x00000040U
#define RTC_TR_SU                            0x0000000FU
#define RTC_TR_SU_0                          0x00000001U
#define RTC_TR_SU_1                          0x00000002U
#define RTC_TR_SU_2                          0x00000004U
#define RTC_TR_SU_3                          0x00000008U

/********************  Bits definition for RTC_DR register  *******************/
#define RTC_DR_YT                            0x00F00000U
#define RTC_DR_YT_0                          0x00100000U
#define RTC_DR_YT_1                          0x00200000U
#define RTC_DR_YT_2                          0x00400000U
#define RTC_DR_YT_3                          0x00800000U
#define RTC_DR_YU                            0x000F0000U
#define RTC_DR_YU_0                          0x00010000U
#define RTC_DR_YU_1                          0x00020000U
#define RTC_DR_YU_2                          0x00040000U
#define RTC_DR_YU_3                          0x00080000U
#define RTC_DR_WDU                           0x0000E000U
#define RTC_DR_WDU_0                         0x00002000U
#define RTC_DR_WDU_1                         0x00004000U
#define RTC_DR_WDU_2                         0x00008000U
#define RTC_DR_MT                            0x00001000U
#define RTC_DR_MU                            0x00000F00U
#define RTC_DR_MU_0                          0x00000100U
#define RTC_DR_MU_1                          0x00000200U
#define RTC_DR_MU_2                          0x00000400U
#define RTC_DR_MU_3                          0x00000800U
#define RTC_DR_DT                            0x00000030U
#define RTC_DR_DT_0                          0x00000010U
#define RTC_DR_DT_1                          0x00000020U
#define RTC_DR_DU                            0x0000000FU
#define RTC_DR_DU_0                          0x00000001U
#define RTC_DR_DU_1                          0x00000002U
#define RTC_DR_DU_2                          0x00000004U
#define RTC_DR_DU_3                          0x00000008U

/********************  Bits definition for RTC_CR register  *******************/
#define RTC_CR_ITSE                          0x01000000U
#define RTC_CR_COE                           0x00800000U
#define RTC_CR_OSEL                          0x00600000U
#define RTC_CR_OSEL_0                        0x00200000U
#define RTC_CR_OSEL_1                        0x00400000U
#define RTC_CR_POL                           0x00100000U
#define RTC_CR_COSEL                         0x00080000U
#define RTC_CR_BKP                           0x00040000U
#define RTC_CR_SUB1H                         0x00020000U
#define RTC_CR_ADD1H                         0x00010000U
#define RTC_CR_TSIE                          0x00008000U
#define RTC_CR_WUTIE                         0x00004000U
#define RTC_CR_ALRBIE                        0x00002000U
#define RTC_CR_ALRAIE                        0x00001000U
#define RTC_CR_TSE                           0x00000800U
#define RTC_CR_WUTE                          0x00000400U
#define RTC_CR_ALRBE                         0x00000200U
#define RTC_CR_ALRAE                         0x00000100U
#define RTC_CR_FMT                           0x00000040U
#define RTC_CR_BYPSHAD                       0x00000020U
#define RTC_CR_REFCKON                       0x00000010U
#define RTC_CR_TSEDGE                        0x00000008U
#define RTC_CR_WUCKSEL                       0x00000007U
#define RTC_CR_WUCKSEL_0                     0x00000001U
#define RTC_CR_WUCKSEL_1                     0x00000002U
#define RTC_CR_WUCKSEL_2                     0x00000004U

/* Legacy define */
#define RTC_CR_BCK                           RTC_CR_BKP

/********************  Bits definition for RTC_ISR register  ******************/
#define RTC_ISR_ITSF                         0x00020000U
#define RTC_ISR_RECALPF                      0x00010000U
#define RTC_ISR_TAMP3F                       0x00008000U
#define RTC_ISR_TAMP2F                       0x00004000U
#define RTC_ISR_TAMP1F                       0x00002000U
#define RTC_ISR_TSOVF                        0x00001000U
#define RTC_ISR_TSF                          0x00000800U
#define RTC_ISR_WUTF                         0x00000400U
#define RTC_ISR_ALRBF                        0x00000200U
#define RTC_ISR_ALRAF                        0x00000100U
#define RTC_ISR_INIT                         0x00000080U
#define RTC_ISR_INITF                        0x00000040U
#define RTC_ISR_RSF                          0x00000020U
#define RTC_ISR_INITS                        0x00000010U
#define RTC_ISR_SHPF                         0x00000008U
#define RTC_ISR_WUTWF                        0x00000004U
#define RTC_ISR_ALRBWF                       0x00000002U
#define RTC_ISR_ALRAWF                       0x00000001U

/********************  Bits definition for RTC_PRER register  *****************/
#define RTC_PRER_PREDIV_A                    0x007F0000U
#define RTC_PRER_PREDIV_S                    0x00007FFFU

/********************  Bits definition for RTC_WUTR register  *****************/
#define RTC_WUTR_WUT                         0x0000FFFFU

/********************  Bits definition for RTC_ALRMAR register  ***************/
#define RTC_ALRMAR_MSK4                      0x80000000U
#define RTC_ALRMAR_WDSEL                     0x40000000U
#define RTC_ALRMAR_DT                        0x30000000U
#define RTC_ALRMAR_DT_0                      0x10000000U
#define RTC_ALRMAR_DT_1                      0x20000000U
#define RTC_ALRMAR_DU                        0x0F000000U
#define RTC_ALRMAR_DU_0                      0x01000000U
#define RTC_ALRMAR_DU_1                      0x02000000U
#define RTC_ALRMAR_DU_2                      0x04000000U
#define RTC_ALRMAR_DU_3                      0x08000000U
#define RTC_ALRMAR_MSK3                      0x00800000U
#define RTC_ALRMAR_PM                        0x00400000U
#define RTC_ALRMAR_HT                        0x00300000U
#define RTC_ALRMAR_HT_0                      0x00100000U
#define RTC_ALRMAR_HT_1                      0x00200000U
#define RTC_ALRMAR_HU                        0x000F0000U
#define RTC_ALRMAR_HU_0                      0x00010000U
#define RTC_ALRMAR_HU_1                      0x00020000U
#define RTC_ALRMAR_HU_2                      0x00040000U
#define RTC_ALRMAR_HU_3                      0x00080000U
#define RTC_ALRMAR_MSK2                      0x00008000U
#define RTC_ALRMAR_MNT                       0x00007000U
#define RTC_ALRMAR_MNT_0                     0x00001000U
#define RTC_ALRMAR_MNT_1                     0x00002000U
#define RTC_ALRMAR_MNT_2                     0x00004000U
#define RTC_ALRMAR_MNU                       0x00000F00U
#define RTC_ALRMAR_MNU_0                     0x00000100U
#define RTC_ALRMAR_MNU_1                     0x00000200U
#define RTC_ALRMAR_MNU_2                     0x00000400U
#define RTC_ALRMAR_MNU_3                     0x00000800U
#define RTC_ALRMAR_MSK1                      0x00000080U
#define RTC_ALRMAR_ST                        0x00000070U
#define RTC_ALRMAR_ST_0                      0x00000010U
#define RTC_ALRMAR_ST_1                      0x00000020U
#define RTC_ALRMAR_ST_2                      0x00000040U
#define RTC_ALRMAR_SU                        0x0000000FU
#define RTC_ALRMAR_SU_0                      0x00000001U
#define RTC_ALRMAR_SU_1                      0x00000002U
#define RTC_ALRMAR_SU_2                      0x00000004U
#define RTC_ALRMAR_SU_3                      0x00000008U

/********************  Bits definition for RTC_ALRMBR register  ***************/
#define RTC_ALRMBR_MSK4                      0x80000000U
#define RTC_ALRMBR_WDSEL                     0x40000000U
#define RTC_ALRMBR_DT                        0x30000000U
#define RTC_ALRMBR_DT_0                      0x10000000U
#define RTC_ALRMBR_DT_1                      0x20000000U
#define RTC_ALRMBR_DU                        0x0F000000U
#define RTC_ALRMBR_DU_0                      0x01000000U
#define RTC_ALRMBR_DU_1                      0x02000000U
#define RTC_ALRMBR_DU_2                      0x04000000U
#define RTC_ALRMBR_DU_3                      0x08000000U
#define RTC_ALRMBR_MSK3                      0x00800000U
#define RTC_ALRMBR_PM                        0x00400000U
#define RTC_ALRMBR_HT                        0x00300000U
#define RTC_ALRMBR_HT_0                      0x00100000U
#define RTC_ALRMBR_HT_1                      0x00200000U
#define RTC_ALRMBR_HU                        0x000F0000U
#define RTC_ALRMBR_HU_0                      0x00010000U
#define RTC_ALRMBR_HU_1                      0x00020000U
#define RTC_ALRMBR_HU_2                      0x00040000U
#define RTC_ALRMBR_HU_3                      0x00080000U
#define RTC_ALRMBR_MSK2                      0x00008000U
#define RTC_ALRMBR_MNT                       0x00007000U
#define RTC_ALRMBR_MNT_0                     0x00001000U
#define RTC_ALRMBR_MNT_1                     0x00002000U
#define RTC_ALRMBR_MNT_2                     0x00004000U
#define RTC_ALRMBR_MNU                       0x00000F00U
#define RTC_ALRMBR_MNU_0                     0x00000100U
#define RTC_ALRMBR_MNU_1                     0x00000200U
#define RTC_ALRMBR_MNU_2                     0x00000400U
#define RTC_ALRMBR_MNU_3                     0x00000800U
#define RTC_ALRMBR_MSK1                      0x00000080U
#define RTC_ALRMBR_ST                        0x00000070U
#define RTC_ALRMBR_ST_0                      0x00000010U
#define RTC_ALRMBR_ST_1                      0x00000020U
#define RTC_ALRMBR_ST_2                      0x00000040U
#define RTC_ALRMBR_SU                        0x0000000FU
#define RTC_ALRMBR_SU_0                      0x00000001U
#define RTC_ALRMBR_SU_1                      0x00000002U
#define RTC_ALRMBR_SU_2                      0x00000004U
#define RTC_ALRMBR_SU_3                      0x00000008U

/********************  Bits definition for RTC_WPR register  ******************/
#define RTC_WPR_KEY                          0x000000FFU

/********************  Bits definition for RTC_SSR register  ******************/
#define RTC_SSR_SS                           0x0000FFFFU

/********************  Bits definition for RTC_SHIFTR register  ***************/
#define RTC_SHIFTR_SUBFS                     0x00007FFFU
#define RTC_SHIFTR_ADD1S                     0x80000000U

/********************  Bits definition for RTC_TSTR register  *****************/
#define RTC_TSTR_PM                          0x00400000U
#define RTC_TSTR_HT                          0x00300000U
#define RTC_TSTR_HT_0                        0x00100000U
#define RTC_TSTR_HT_1                        0x00200000U
#define RTC_TSTR_HU                          0x000F0000U
#define RTC_TSTR_HU_0                        0x00010000U
#define RTC_TSTR_HU_1                        0x00020000U
#define RTC_TSTR_HU_2                        0x00040000U
#define RTC_TSTR_HU_3                        0x00080000U
#define RTC_TSTR_MNT                         0x00007000U
#define RTC_TSTR_MNT_0                       0x00001000U
#define RTC_TSTR_MNT_1                       0x00002000U
#define RTC_TSTR_MNT_2                       0x00004000U
#define RTC_TSTR_MNU                         0x00000F00U
#define RTC_TSTR_MNU_0                       0x00000100U
#define RTC_TSTR_MNU_1                       0x00000200U
#define RTC_TSTR_MNU_2                       0x00000400U
#define RTC_TSTR_MNU_3                       0x00000800U
#define RTC_TSTR_ST                          0x00000070U
#define RTC_TSTR_ST_0                        0x00000010U
#define RTC_TSTR_ST_1                        0x00000020U
#define RTC_TSTR_ST_2                        0x00000040U
#define RTC_TSTR_SU                          0x0000000FU
#define RTC_TSTR_SU_0                        0x00000001U
#define RTC_TSTR_SU_1                        0x00000002U
#define RTC_TSTR_SU_2                        0x00000004U
#define RTC_TSTR_SU_3                        0x00000008U

/********************  Bits definition for RTC_TSDR register  *****************/
#define RTC_TSDR_WDU                         0x0000E000U
#define RTC_TSDR_WDU_0                       0x00002000U
#define RTC_TSDR_WDU_1                       0x00004000U
#define RTC_TSDR_WDU_2                       0x00008000U
#define RTC_TSDR_MT                          0x00001000U
#define RTC_TSDR_MU                          0x00000F00U
#define RTC_TSDR_MU_0                        0x00000100U
#define RTC_TSDR_MU_1                        0x00000200U
#define RTC_TSDR_MU_2                        0x00000400U
#define RTC_TSDR_MU_3                        0x00000800U
#define RTC_TSDR_DT                          0x00000030U
#define RTC_TSDR_DT_0                        0x00000010U
#define RTC_TSDR_DT_1                        0x00000020U
#define RTC_TSDR_DU                          0x0000000FU
#define RTC_TSDR_DU_0                        0x00000001U
#define RTC_TSDR_DU_1                        0x00000002U
#define RTC_TSDR_DU_2                        0x00000004U
#define RTC_TSDR_DU_3                        0x00000008U

/********************  Bits definition for RTC_TSSSR register  ****************/
#define RTC_TSSSR_SS                         0x0000FFFFU

/********************  Bits definition for RTC_CAL register  *****************/
#define RTC_CALR_CALP                        0x00008000U
#define RTC_CALR_CALW8                       0x00004000U
#define RTC_CALR_CALW16                      0x00002000U
#define RTC_CALR_CALM                        0x000001FFU
#define RTC_CALR_CALM_0                      0x00000001U
#define RTC_CALR_CALM_1                      0x00000002U
#define RTC_CALR_CALM_2                      0x00000004U
#define RTC_CALR_CALM_3                      0x00000008U
#define RTC_CALR_CALM_4                      0x00000010U
#define RTC_CALR_CALM_5                      0x00000020U
#define RTC_CALR_CALM_6                      0x00000040U
#define RTC_CALR_CALM_7                      0x00000080U
#define RTC_CALR_CALM_8                      0x00000100U

/********************  Bits definition for RTC_TAMPCR register  ****************/
#define RTC_TAMPCR_TAMP3MF                   0x01000000U
#define RTC_TAMPCR_TAMP3NOERASE              0x00800000U
#define RTC_TAMPCR_TAMP3IE                   0x00400000U
#define RTC_TAMPCR_TAMP2MF                   0x00200000U
#define RTC_TAMPCR_TAMP2NOERASE              0x00100000U
#define RTC_TAMPCR_TAMP2IE                   0x00080000U
#define RTC_TAMPCR_TAMP1MF                   0x00040000U
#define RTC_TAMPCR_TAMP1NOERASE              0x00020000U
#define RTC_TAMPCR_TAMP1IE                   0x00010000U
#define RTC_TAMPCR_TAMPPUDIS                 0x00008000U
#define RTC_TAMPCR_TAMPPRCH                  0x00006000U
#define RTC_TAMPCR_TAMPPRCH_0                0x00002000U
#define RTC_TAMPCR_TAMPPRCH_1                0x00004000U
#define RTC_TAMPCR_TAMPFLT                   0x00001800U
#define RTC_TAMPCR_TAMPFLT_0                 0x00000800U
#define RTC_TAMPCR_TAMPFLT_1                 0x00001000U
#define RTC_TAMPCR_TAMPFREQ                  0x00000700U
#define RTC_TAMPCR_TAMPFREQ_0                0x00000100U
#define RTC_TAMPCR_TAMPFREQ_1                0x00000200U
#define RTC_TAMPCR_TAMPFREQ_2                0x00000400U
#define RTC_TAMPCR_TAMPTS                    0x00000080U
#define RTC_TAMPCR_TAMP3TRG                  0x00000040U
#define RTC_TAMPCR_TAMP3E                    0x00000020U
#define RTC_TAMPCR_TAMP2TRG                  0x00000010U
#define RTC_TAMPCR_TAMP2E                    0x00000008U
#define RTC_TAMPCR_TAMPIE                    0x00000004U
#define RTC_TAMPCR_TAMP1TRG                  0x00000002U
#define RTC_TAMPCR_TAMP1E                    0x00000001U

/* Legacy defines */
#define RTC_TAMPCR_TAMP3_TRG                  RTC_TAMPCR_TAMP3TRG
#define RTC_TAMPCR_TAMP2_TRG                  RTC_TAMPCR_TAMP2TRG
#define RTC_TAMPCR_TAMP1_TRG                  RTC_TAMPCR_TAMP1TRG

/********************  Bits definition for RTC_ALRMASSR register  *************/
#define RTC_ALRMASSR_MASKSS                  0x0F000000U
#define RTC_ALRMASSR_MASKSS_0                0x01000000U
#define RTC_ALRMASSR_MASKSS_1                0x02000000U
#define RTC_ALRMASSR_MASKSS_2                0x04000000U
#define RTC_ALRMASSR_MASKSS_3                0x08000000U
#define RTC_ALRMASSR_SS                      0x00007FFFU

/********************  Bits definition for RTC_ALRMBSSR register  *************/
#define RTC_ALRMBSSR_MASKSS                  0x0F000000U
#define RTC_ALRMBSSR_MASKSS_0                0x01000000U
#define RTC_ALRMBSSR_MASKSS_1                0x02000000U
#define RTC_ALRMBSSR_MASKSS_2                0x04000000U
#define RTC_ALRMBSSR_MASKSS_3                0x08000000U
#define RTC_ALRMBSSR_SS                      0x00007FFFU

/********************  Bits definition for RTC_OR register  ****************/
#define RTC_OR_TSINSEL                       0x00000006U
#define RTC_OR_TSINSEL_0                     0x00000002U
#define RTC_OR_TSINSEL_1                     0x00000004U
#define RTC_OR_ALARMTYPE                     0x00000008U

/********************  Bits definition for RTC_BKP0R register  ****************/
#define RTC_BKP0R                            0xFFFFFFFFU

/********************  Bits definition for RTC_BKP1R register  ****************/
#define RTC_BKP1R                            0xFFFFFFFFU

/********************  Bits definition for RTC_BKP2R register  ****************/
#define RTC_BKP2R                            0xFFFFFFFFU

/********************  Bits definition for RTC_BKP3R register  ****************/
#define RTC_BKP3R                            0xFFFFFFFFU

/********************  Bits definition for RTC_BKP4R register  ****************/
#define RTC_BKP4R                            0xFFFFFFFFU

/********************  Bits definition for RTC_BKP5R register  ****************/
#define RTC_BKP5R                            0xFFFFFFFFU

/********************  Bits definition for RTC_BKP6R register  ****************/
#define RTC_BKP6R                            0xFFFFFFFFU

/********************  Bits definition for RTC_BKP7R register  ****************/
#define RTC_BKP7R                            0xFFFFFFFFU

/********************  Bits definition for RTC_BKP8R register  ****************/
#define RTC_BKP8R                            0xFFFFFFFFU

/********************  Bits definition for RTC_BKP9R register  ****************/
#define RTC_BKP9R                            0xFFFFFFFFU

/********************  Bits definition for RTC_BKP10R register  ***************/
#define RTC_BKP10R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP11R register  ***************/
#define RTC_BKP11R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP12R register  ***************/
#define RTC_BKP12R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP13R register  ***************/
#define RTC_BKP13R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP14R register  ***************/
#define RTC_BKP14R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP15R register  ***************/
#define RTC_BKP15R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP16R register  ***************/
#define RTC_BKP16R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP17R register  ***************/
#define RTC_BKP17R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP18R register  ***************/
#define RTC_BKP18R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP19R register  ***************/
#define RTC_BKP19R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP20R register  ***************/
#define RTC_BKP20R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP21R register  ***************/
#define RTC_BKP21R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP22R register  ***************/
#define RTC_BKP22R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP23R register  ***************/
#define RTC_BKP23R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP24R register  ***************/
#define RTC_BKP24R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP25R register  ***************/
#define RTC_BKP25R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP26R register  ***************/
#define RTC_BKP26R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP27R register  ***************/
#define RTC_BKP27R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP28R register  ***************/
#define RTC_BKP28R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP29R register  ***************/
#define RTC_BKP29R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP30R register  ***************/
#define RTC_BKP30R                           0xFFFFFFFFU

/********************  Bits definition for RTC_BKP31R register  ***************/
#define RTC_BKP31R                           0xFFFFFFFFU

/******************** Number of backup registers ******************************/
#define RTC_BKP_NUMBER                       0x00000020U


/******************************************************************************/
/*                                                                            */
/*                          Serial Audio Interface                            */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for SAI_GCR register  *******************/
#define  SAI_GCR_SYNCIN                      0x00000003U        /*!<SYNCIN[1:0] bits (Synchronization Inputs)   */
#define  SAI_GCR_SYNCIN_0                    0x00000001U        /*!<Bit 0 */
#define  SAI_GCR_SYNCIN_1                    0x00000002U        /*!<Bit 1 */

#define  SAI_GCR_SYNCOUT                     0x00000030U        /*!<SYNCOUT[1:0] bits (Synchronization Outputs) */
#define  SAI_GCR_SYNCOUT_0                   0x00000010U        /*!<Bit 0 */
#define  SAI_GCR_SYNCOUT_1                   0x00000020U        /*!<Bit 1 */

/*******************  Bit definition for SAI_xCR1 register  *******************/
#define  SAI_xCR1_MODE                       0x00000003U        /*!<MODE[1:0] bits (Audio Block Mode)           */
#define  SAI_xCR1_MODE_0                     0x00000001U        /*!<Bit 0 */
#define  SAI_xCR1_MODE_1                     0x00000002U        /*!<Bit 1 */

#define  SAI_xCR1_PRTCFG                     0x0000000CU        /*!<PRTCFG[1:0] bits (Protocol Configuration)   */
#define  SAI_xCR1_PRTCFG_0                   0x00000004U        /*!<Bit 0 */
#define  SAI_xCR1_PRTCFG_1                   0x00000008U        /*!<Bit 1 */

#define  SAI_xCR1_DS                         0x000000E0U        /*!<DS[1:0] bits (Data Size) */
#define  SAI_xCR1_DS_0                       0x00000020U        /*!<Bit 0 */
#define  SAI_xCR1_DS_1                       0x00000040U        /*!<Bit 1 */
#define  SAI_xCR1_DS_2                       0x00000080U        /*!<Bit 2 */

#define  SAI_xCR1_LSBFIRST                   0x00000100U        /*!<LSB First Configuration  */
#define  SAI_xCR1_CKSTR                      0x00000200U        /*!<ClocK STRobing edge      */

#define  SAI_xCR1_SYNCEN                     0x00000C00U        /*!<SYNCEN[1:0](SYNChronization ENable) */
#define  SAI_xCR1_SYNCEN_0                   0x00000400U        /*!<Bit 0 */
#define  SAI_xCR1_SYNCEN_1                   0x00000800U        /*!<Bit 1 */

#define  SAI_xCR1_MONO                       0x00001000U        /*!<Mono mode                  */
#define  SAI_xCR1_OUTDRIV                    0x00002000U        /*!<Output Drive               */
#define  SAI_xCR1_SAIEN                      0x00010000U        /*!<Audio Block enable         */
#define  SAI_xCR1_DMAEN                      0x00020000U        /*!<DMA enable                 */
#define  SAI_xCR1_NODIV                      0x00080000U        /*!<No Divider Configuration   */

#define  SAI_xCR1_MCKDIV                     0x00F00000U        /*!<MCKDIV[3:0] (Master ClocK Divider)  */
#define  SAI_xCR1_MCKDIV_0                   0x00100000U        /*!<Bit 0  */
#define  SAI_xCR1_MCKDIV_1                   0x00200000U        /*!<Bit 1  */
#define  SAI_xCR1_MCKDIV_2                   0x00400000U        /*!<Bit 2  */
#define  SAI_xCR1_MCKDIV_3                   0x00800000U        /*!<Bit 3  */

/*******************  Bit definition for SAI_xCR2 register  *******************/
#define  SAI_xCR2_FTH                        0x00000007U        /*!<FTH[2:0](Fifo THreshold)  */
#define  SAI_xCR2_FTH_0                      0x00000001U        /*!<Bit 0 */
#define  SAI_xCR2_FTH_1                      0x00000002U        /*!<Bit 1 */
#define  SAI_xCR2_FTH_2                      0x00000004U        /*!<Bit 2 */

#define  SAI_xCR2_FFLUSH                     0x00000008U        /*!<Fifo FLUSH                       */
#define  SAI_xCR2_TRIS                       0x00000010U        /*!<TRIState Management on data line */
#define  SAI_xCR2_MUTE                       0x00000020U        /*!<Mute mode                        */
#define  SAI_xCR2_MUTEVAL                    0x00000040U        /*!<Muate value                      */

#define  SAI_xCR2_MUTECNT                    0x00001F80U       /*!<MUTECNT[5:0] (MUTE counter) */
#define  SAI_xCR2_MUTECNT_0                  0x00000080U        /*!<Bit 0 */
#define  SAI_xCR2_MUTECNT_1                  0x00000100U        /*!<Bit 1 */
#define  SAI_xCR2_MUTECNT_2                  0x00000200U        /*!<Bit 2 */
#define  SAI_xCR2_MUTECNT_3                  0x00000400U        /*!<Bit 3 */
#define  SAI_xCR2_MUTECNT_4                  0x00000800U        /*!<Bit 4 */
#define  SAI_xCR2_MUTECNT_5                  0x00001000U        /*!<Bit 5 */

#define  SAI_xCR2_CPL                        0x00002000U        /*!< Complement Bit             */

#define  SAI_xCR2_COMP                       0x0000C000U        /*!<COMP[1:0] (Companding mode) */
#define  SAI_xCR2_COMP_0                     0x00004000U        /*!<Bit 0 */
#define  SAI_xCR2_COMP_1                     0x00008000U        /*!<Bit 1 */

/******************  Bit definition for SAI_xFRCR register  *******************/
#define  SAI_xFRCR_FRL                       0x000000FFU        /*!<FRL[1:0](Frame length)  */
#define  SAI_xFRCR_FRL_0                     0x00000001U        /*!<Bit 0 */
#define  SAI_xFRCR_FRL_1                     0x00000002U        /*!<Bit 1 */
#define  SAI_xFRCR_FRL_2                     0x00000004U        /*!<Bit 2 */
#define  SAI_xFRCR_FRL_3                     0x00000008U        /*!<Bit 3 */
#define  SAI_xFRCR_FRL_4                     0x00000010U        /*!<Bit 4 */
#define  SAI_xFRCR_FRL_5                     0x00000020U        /*!<Bit 5 */
#define  SAI_xFRCR_FRL_6                     0x00000040U        /*!<Bit 6 */
#define  SAI_xFRCR_FRL_7                     0x00000080U        /*!<Bit 7 */

#define  SAI_xFRCR_FSALL                     0x00007F00U        /*!<FRL[1:0] (Frame synchronization active level length)  */
#define  SAI_xFRCR_FSALL_0                   0x00000100U        /*!<Bit 0 */
#define  SAI_xFRCR_FSALL_1                   0x00000200U        /*!<Bit 1 */
#define  SAI_xFRCR_FSALL_2                   0x00000400U        /*!<Bit 2 */
#define  SAI_xFRCR_FSALL_3                   0x00000800U        /*!<Bit 3 */
#define  SAI_xFRCR_FSALL_4                   0x00001000U        /*!<Bit 4 */
#define  SAI_xFRCR_FSALL_5                   0x00002000U        /*!<Bit 5 */
#define  SAI_xFRCR_FSALL_6                   0x00004000U        /*!<Bit 6 */

#define  SAI_xFRCR_FSDEF                     0x00010000U        /*!<Frame Synchronization Definition  */
#define  SAI_xFRCR_FSPOL                     0x00020000U        /*!<Frame Synchronization POLarity    */
#define  SAI_xFRCR_FSOFF                     0x00040000U        /*!<Frame Synchronization OFFset      */

/* Legacy define */
#define  SAI_xFRCR_FSPO                      SAI_xFRCR_FSPOL

/******************  Bit definition for SAI_xSLOTR register  *******************/
#define  SAI_xSLOTR_FBOFF                    0x0000001FU        /*!<FRL[4:0](First Bit Offset)  */
#define  SAI_xSLOTR_FBOFF_0                  0x00000001U        /*!<Bit 0 */
#define  SAI_xSLOTR_FBOFF_1                  0x00000002U        /*!<Bit 1 */
#define  SAI_xSLOTR_FBOFF_2                  0x00000004U        /*!<Bit 2 */
#define  SAI_xSLOTR_FBOFF_3                  0x00000008U        /*!<Bit 3 */
#define  SAI_xSLOTR_FBOFF_4                  0x00000010U        /*!<Bit 4 */

#define  SAI_xSLOTR_SLOTSZ                   0x000000C0U        /*!<SLOTSZ[1:0] (Slot size)  */
#define  SAI_xSLOTR_SLOTSZ_0                 0x00000040U        /*!<Bit 0 */
#define  SAI_xSLOTR_SLOTSZ_1                 0x00000080U        /*!<Bit 1 */

#define  SAI_xSLOTR_NBSLOT                   0x00000F00U        /*!<NBSLOT[3:0] (Number of Slot in audio Frame)  */
#define  SAI_xSLOTR_NBSLOT_0                 0x00000100U        /*!<Bit 0 */
#define  SAI_xSLOTR_NBSLOT_1                 0x00000200U        /*!<Bit 1 */
#define  SAI_xSLOTR_NBSLOT_2                 0x00000400U        /*!<Bit 2 */
#define  SAI_xSLOTR_NBSLOT_3                 0x00000800U        /*!<Bit 3 */

#define  SAI_xSLOTR_SLOTEN                   0xFFFF0000U        /*!<SLOTEN[15:0] (Slot Enable)  */

/*******************  Bit definition for SAI_xIMR register  *******************/
#define  SAI_xIMR_OVRUDRIE                   0x00000001U        /*!<Overrun underrun interrupt enable                              */
#define  SAI_xIMR_MUTEDETIE                  0x00000002U        /*!<Mute detection interrupt enable                                */
#define  SAI_xIMR_WCKCFGIE                   0x00000004U        /*!<Wrong Clock Configuration interrupt enable                     */
#define  SAI_xIMR_FREQIE                     0x00000008U        /*!<FIFO request interrupt enable                                  */
#define  SAI_xIMR_CNRDYIE                    0x00000010U        /*!<Codec not ready interrupt enable                               */
#define  SAI_xIMR_AFSDETIE                   0x00000020U        /*!<Anticipated frame synchronization detection interrupt enable   */
#define  SAI_xIMR_LFSDETIE                   0x00000040U        /*!<Late frame synchronization detection interrupt enable          */

/********************  Bit definition for SAI_xSR register  *******************/
#define  SAI_xSR_OVRUDR                      0x00000001U         /*!<Overrun underrun                               */
#define  SAI_xSR_MUTEDET                     0x00000002U         /*!<Mute detection                                 */
#define  SAI_xSR_WCKCFG                      0x00000004U         /*!<Wrong Clock Configuration                      */
#define  SAI_xSR_FREQ                        0x00000008U         /*!<FIFO request                                   */
#define  SAI_xSR_CNRDY                       0x00000010U         /*!<Codec not ready                                */
#define  SAI_xSR_AFSDET                      0x00000020U         /*!<Anticipated frame synchronization detection    */
#define  SAI_xSR_LFSDET                      0x00000040U         /*!<Late frame synchronization detection           */

#define  SAI_xSR_FLVL                        0x00070000U         /*!<FLVL[2:0] (FIFO Level Threshold)               */
#define  SAI_xSR_FLVL_0                      0x00010000U         /*!<Bit 0 */
#define  SAI_xSR_FLVL_1                      0x00020000U         /*!<Bit 1 */
#define  SAI_xSR_FLVL_2                      0x00040000U         /*!<Bit 2 */

/******************  Bit definition for SAI_xCLRFR register  ******************/
#define  SAI_xCLRFR_COVRUDR                  0x00000001U        /*!<Clear Overrun underrun                               */
#define  SAI_xCLRFR_CMUTEDET                 0x00000002U        /*!<Clear Mute detection                                 */
#define  SAI_xCLRFR_CWCKCFG                  0x00000004U        /*!<Clear Wrong Clock Configuration                      */
#define  SAI_xCLRFR_CFREQ                    0x00000008U        /*!<Clear FIFO request                                   */
#define  SAI_xCLRFR_CCNRDY                   0x00000010U        /*!<Clear Codec not ready                                */
#define  SAI_xCLRFR_CAFSDET                  0x00000020U        /*!<Clear Anticipated frame synchronization detection    */
#define  SAI_xCLRFR_CLFSDET                  0x00000040U        /*!<Clear Late frame synchronization detection           */

/******************  Bit definition for SAI_xDR register  *********************/
#define  SAI_xDR_DATA                        0xFFFFFFFFU

/******************************************************************************/
/*                                                                            */
/*                              SPDIF-RX Interface                            */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for SPDIF_CR register  *******************/
#define  SPDIFRX_CR_SPDIFEN                0x00000003U        /*!<Peripheral Block Enable                      */
#define  SPDIFRX_CR_RXDMAEN                0x00000004U        /*!<Receiver DMA Enable for data flow            */
#define  SPDIFRX_CR_RXSTEO                 0x00000008U        /*!<Stereo Mode                                  */
#define  SPDIFRX_CR_DRFMT                  0x00000030U        /*!<RX Data format                               */
#define  SPDIFRX_CR_PMSK                   0x00000040U        /*!<Mask Parity error bit                        */
#define  SPDIFRX_CR_VMSK                   0x00000080U        /*!<Mask of Validity bit                         */
#define  SPDIFRX_CR_CUMSK                  0x00000100U        /*!<Mask of channel status and user bits         */
#define  SPDIFRX_CR_PTMSK                  0x00000200U        /*!<Mask of Preamble Type bits                   */
#define  SPDIFRX_CR_CBDMAEN                0x00000400U        /*!<Control Buffer DMA ENable for control flow   */
#define  SPDIFRX_CR_CHSEL                  0x00000800U        /*!<Channel Selection                            */
#define  SPDIFRX_CR_NBTR                   0x00003000U        /*!<Maximum allowed re-tries during synchronization phase */
#define  SPDIFRX_CR_WFA                    0x00004000U        /*!<Wait For Activity     */
#define  SPDIFRX_CR_INSEL                  0x00070000U        /*!<SPDIF input selection */

/*******************  Bit definition for SPDIFRX_IMR register  *******************/
#define  SPDIFRX_IMR_RXNEIE                0x00000001U        /*!<RXNE interrupt enable                              */
#define  SPDIFRX_IMR_CSRNEIE               0x00000002U        /*!<Control Buffer Ready Interrupt Enable              */
#define  SPDIFRX_IMR_PERRIE                0x00000004U        /*!<Parity error interrupt enable                      */
#define  SPDIFRX_IMR_OVRIE                 0x00000008U        /*!<Overrun error Interrupt Enable                     */
#define  SPDIFRX_IMR_SBLKIE                0x00000010U        /*!<Synchronization Block Detected Interrupt Enable    */
#define  SPDIFRX_IMR_SYNCDIE               0x00000020U        /*!<Synchronization Done                               */
#define  SPDIFRX_IMR_IFEIE                 0x00000040U        /*!<Serial Interface Error Interrupt Enable            */

/*******************  Bit definition for SPDIFRX_SR register  *******************/
#define  SPDIFRX_SR_RXNE                   0x00000001U       /*!<Read data register not empty                          */
#define  SPDIFRX_SR_CSRNE                  0x00000002U       /*!<The Control Buffer register is not empty              */
#define  SPDIFRX_SR_PERR                   0x00000004U       /*!<Parity error                                          */
#define  SPDIFRX_SR_OVR                    0x00000008U       /*!<Overrun error                                         */
#define  SPDIFRX_SR_SBD                    0x00000010U       /*!<Synchronization Block Detected                        */
#define  SPDIFRX_SR_SYNCD                  0x00000020U       /*!<Synchronization Done                                  */
#define  SPDIFRX_SR_FERR                   0x00000040U       /*!<Framing error                                         */
#define  SPDIFRX_SR_SERR                   0x00000080U       /*!<Synchronization error                                 */
#define  SPDIFRX_SR_TERR                   0x00000100U       /*!<Time-out error                                        */
#define  SPDIFRX_SR_WIDTH5                 0x7FFF0000U       /*!<Duration of 5 symbols counted with spdif_clk          */

/*******************  Bit definition for SPDIFRX_IFCR register  *******************/
#define  SPDIFRX_IFCR_PERRCF               0x00000004U       /*!<Clears the Parity error flag                         */
#define  SPDIFRX_IFCR_OVRCF                0x00000008U       /*!<Clears the Overrun error flag                        */
#define  SPDIFRX_IFCR_SBDCF                0x00000010U       /*!<Clears the Synchronization Block Detected flag       */
#define  SPDIFRX_IFCR_SYNCDCF              0x00000020U       /*!<Clears the Synchronization Done flag                 */

/*******************  Bit definition for SPDIFRX_DR register  (DRFMT = 0b00 case) *******************/
#define  SPDIFRX_DR0_DR                    0x00FFFFFFU        /*!<Data value            */
#define  SPDIFRX_DR0_PE                    0x01000000U        /*!<Parity Error bit      */
#define  SPDIFRX_DR0_V                     0x02000000U        /*!<Validity bit          */
#define  SPDIFRX_DR0_U                     0x04000000U        /*!<User bit              */
#define  SPDIFRX_DR0_C                     0x08000000U        /*!<Channel Status bit    */
#define  SPDIFRX_DR0_PT                    0x30000000U        /*!<Preamble Type         */

/*******************  Bit definition for SPDIFRX_DR register  (DRFMT = 0b01 case) *******************/
#define  SPDIFRX_DR1_DR                    0xFFFFFF00U        /*!<Data value            */
#define  SPDIFRX_DR1_PT                    0x00000030U        /*!<Preamble Type         */
#define  SPDIFRX_DR1_C                     0x00000008U        /*!<Channel Status bit    */
#define  SPDIFRX_DR1_U                     0x00000004U        /*!<User bit              */
#define  SPDIFRX_DR1_V                     0x00000002U        /*!<Validity bit          */
#define  SPDIFRX_DR1_PE                    0x00000001U        /*!<Parity Error bit      */

/*******************  Bit definition for SPDIFRX_DR register  (DRFMT = 0b10 case) *******************/
#define  SPDIFRX_DR1_DRNL1                 0xFFFF0000U        /*!<Data value Channel B      */
#define  SPDIFRX_DR1_DRNL2                 0x0000FFFFU        /*!<Data value Channel A      */

/*******************  Bit definition for SPDIFRX_CSR register   *******************/
#define  SPDIFRX_CSR_USR                   0x0000FFFFU        /*!<User data information           */
#define  SPDIFRX_CSR_CS                    0x00FF0000U        /*!<Channel A status information    */
#define  SPDIFRX_CSR_SOB                   0x01000000U        /*!<Start Of Block                  */

/*******************  Bit definition for SPDIFRX_DIR register    *******************/
#define  SPDIFRX_DIR_THI                   0x000013FFU        /*!<Threshold LOW      */
#define  SPDIFRX_DIR_TLO                   0x1FFF0000U        /*!<Threshold HIGH     */

/******************************************************************************/
/*                                                                            */
/*                          SD host Interface                                 */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for SDMMC_POWER register  ******************/
#define  SDMMC_POWER_PWRCTRL                  0x03U               /*!<PWRCTRL[1:0] bits (Power supply control bits) */
#define  SDMMC_POWER_PWRCTRL_0                0x01U               /*!<Bit 0 */
#define  SDMMC_POWER_PWRCTRL_1                0x02U               /*!<Bit 1 */

/******************  Bit definition for SDMMC_CLKCR register  ******************/
#define  SDMMC_CLKCR_CLKDIV                   0x00FFU            /*!<Clock divide factor             */
#define  SDMMC_CLKCR_CLKEN                    0x0100U            /*!<Clock enable bit                */
#define  SDMMC_CLKCR_PWRSAV                   0x0200U            /*!<Power saving configuration bit  */
#define  SDMMC_CLKCR_BYPASS                   0x0400U            /*!<Clock divider bypass enable bit */

#define  SDMMC_CLKCR_WIDBUS                   0x1800U            /*!<WIDBUS[1:0] bits (Wide bus mode enable bit) */
#define  SDMMC_CLKCR_WIDBUS_0                 0x0800U            /*!<Bit 0 */
#define  SDMMC_CLKCR_WIDBUS_1                 0x1000U            /*!<Bit 1 */

#define  SDMMC_CLKCR_NEGEDGE                  0x2000U            /*!<SDMMC_CK dephasing selection bit */
#define  SDMMC_CLKCR_HWFC_EN                  0x4000U            /*!<HW Flow Control enable          */

/*******************  Bit definition for SDMMC_ARG register  *******************/
#define  SDMMC_ARG_CMDARG                     0xFFFFFFFFU            /*!<Command argument */

/*******************  Bit definition for SDMMC_CMD register  *******************/
#define  SDMMC_CMD_CMDINDEX                   0x003FU            /*!<Command Index                               */

#define  SDMMC_CMD_WAITRESP                   0x00C0U            /*!<WAITRESP[1:0] bits (Wait for response bits) */
#define  SDMMC_CMD_WAITRESP_0                 0x0040U            /*!< Bit 0 */
#define  SDMMC_CMD_WAITRESP_1                 0x0080U            /*!< Bit 1 */

#define  SDMMC_CMD_WAITINT                    0x0100U            /*!<CPSM Waits for Interrupt Request                               */
#define  SDMMC_CMD_WAITPEND                   0x0200U            /*!<CPSM Waits for ends of data transfer (CmdPend internal signal) */
#define  SDMMC_CMD_CPSMEN                     0x0400U            /*!<Command path state machine (CPSM) Enable bit                   */
#define  SDMMC_CMD_SDIOSUSPEND                0x0800U            /*!<SD I/O suspend command                                         */

/*****************  Bit definition for SDMMC_RESPCMD register  *****************/
#define  SDMMC_RESPCMD_RESPCMD                0x3FU               /*!<Response command index */

/******************  Bit definition for SDMMC_RESP0 register  ******************/
#define  SDMMC_RESP0_CARDSTATUS0              0xFFFFFFFFU        /*!<Card Status */

/******************  Bit definition for SDMMC_RESP1 register  ******************/
#define  SDMMC_RESP1_CARDSTATUS1              0xFFFFFFFFU        /*!<Card Status */

/******************  Bit definition for SDMMC_RESP2 register  ******************/
#define  SDMMC_RESP2_CARDSTATUS2              0xFFFFFFFFU        /*!<Card Status */

/******************  Bit definition for SDMMC_RESP3 register  ******************/
#define  SDMMC_RESP3_CARDSTATUS3              0xFFFFFFFFU        /*!<Card Status */

/******************  Bit definition for SDMMC_RESP4 register  ******************/
#define  SDMMC_RESP4_CARDSTATUS4              0xFFFFFFFFU        /*!<Card Status */

/******************  Bit definition for SDMMC_DTIMER register  *****************/
#define  SDMMC_DTIMER_DATATIME                0xFFFFFFFFU        /*!<Data timeout period. */

/******************  Bit definition for SDMMC_DLEN register  *******************/
#define  SDMMC_DLEN_DATALENGTH                0x01FFFFFFU        /*!<Data length value    */

/******************  Bit definition for SDMMC_DCTRL register  ******************/
#define  SDMMC_DCTRL_DTEN                     0x0001U            /*!<Data transfer enabled bit         */
#define  SDMMC_DCTRL_DTDIR                    0x0002U            /*!<Data transfer direction selection */
#define  SDMMC_DCTRL_DTMODE                   0x0004U            /*!<Data transfer mode selection      */
#define  SDMMC_DCTRL_DMAEN                    0x0008U            /*!<DMA enabled bit                   */

#define  SDMMC_DCTRL_DBLOCKSIZE               0x00F0U            /*!<DBLOCKSIZE[3:0] bits (Data block size) */
#define  SDMMC_DCTRL_DBLOCKSIZE_0             0x0010U            /*!<Bit 0 */
#define  SDMMC_DCTRL_DBLOCKSIZE_1             0x0020U            /*!<Bit 1 */
#define  SDMMC_DCTRL_DBLOCKSIZE_2             0x0040U            /*!<Bit 2 */
#define  SDMMC_DCTRL_DBLOCKSIZE_3             0x0080U            /*!<Bit 3 */

#define  SDMMC_DCTRL_RWSTART                  0x0100U            /*!<Read wait start         */
#define  SDMMC_DCTRL_RWSTOP                   0x0200U            /*!<Read wait stop          */
#define  SDMMC_DCTRL_RWMOD                    0x0400U            /*!<Read wait mode          */
#define  SDMMC_DCTRL_SDIOEN                   0x0800U            /*!<SD I/O enable functions */

/******************  Bit definition for SDMMC_DCOUNT register  *****************/
#define  SDMMC_DCOUNT_DATACOUNT               0x01FFFFFFU        /*!<Data count value */

/******************  Bit definition for SDMMC_STA registe  ********************/
#define  SDMMC_STA_CCRCFAIL                   0x00000001U        /*!<Command response received (CRC check failed)  */
#define  SDMMC_STA_DCRCFAIL                   0x00000002U        /*!<Data block sent/received (CRC check failed)   */
#define  SDMMC_STA_CTIMEOUT                   0x00000004U        /*!<Command response timeout                      */
#define  SDMMC_STA_DTIMEOUT                   0x00000008U        /*!<Data timeout                                  */
#define  SDMMC_STA_TXUNDERR                   0x00000010U        /*!<Transmit FIFO underrun error                  */
#define  SDMMC_STA_RXOVERR                    0x00000020U        /*!<Received FIFO overrun error                   */
#define  SDMMC_STA_CMDREND                    0x00000040U        /*!<Command response received (CRC check passed)  */
#define  SDMMC_STA_CMDSENT                    0x00000080U        /*!<Command sent (no response required)           */
#define  SDMMC_STA_DATAEND                    0x00000100U        /*!<Data end (data counter, SDIDCOUNT, is zero)   */
#define  SDMMC_STA_DBCKEND                    0x00000400U        /*!<Data block sent/received (CRC check passed)   */
#define  SDMMC_STA_CMDACT                     0x00000800U        /*!<Command transfer in progress                  */
#define  SDMMC_STA_TXACT                      0x00001000U        /*!<Data transmit in progress                     */
#define  SDMMC_STA_RXACT                      0x00002000U        /*!<Data receive in progress                      */
#define  SDMMC_STA_TXFIFOHE                   0x00004000U        /*!<Transmit FIFO Half Empty: at least 8 words can be written into the FIFO */
#define  SDMMC_STA_RXFIFOHF                   0x00008000U        /*!<Receive FIFO Half Full: there are at least 8 words in the FIFO */
#define  SDMMC_STA_TXFIFOF                    0x00010000U        /*!<Transmit FIFO full                            */
#define  SDMMC_STA_RXFIFOF                    0x00020000U        /*!<Receive FIFO full                             */
#define  SDMMC_STA_TXFIFOE                    0x00040000U        /*!<Transmit FIFO empty                           */
#define  SDMMC_STA_RXFIFOE                    0x00080000U        /*!<Receive FIFO empty                            */
#define  SDMMC_STA_TXDAVL                     0x00100000U        /*!<Data available in transmit FIFO               */
#define  SDMMC_STA_RXDAVL                     0x00200000U        /*!<Data available in receive FIFO                */
#define  SDMMC_STA_SDIOIT                     0x00400000U        /*!<SDMMC interrupt received                       */

/*******************  Bit definition for SDMMC_ICR register  *******************/
#define  SDMMC_ICR_CCRCFAILC                  0x00000001U        /*!<CCRCFAIL flag clear bit */
#define  SDMMC_ICR_DCRCFAILC                  0x00000002U        /*!<DCRCFAIL flag clear bit */
#define  SDMMC_ICR_CTIMEOUTC                  0x00000004U        /*!<CTIMEOUT flag clear bit */
#define  SDMMC_ICR_DTIMEOUTC                  0x00000008U        /*!<DTIMEOUT flag clear bit */
#define  SDMMC_ICR_TXUNDERRC                  0x00000010U        /*!<TXUNDERR flag clear bit */
#define  SDMMC_ICR_RXOVERRC                   0x00000020U        /*!<RXOVERR flag clear bit  */
#define  SDMMC_ICR_CMDRENDC                   0x00000040U        /*!<CMDREND flag clear bit  */
#define  SDMMC_ICR_CMDSENTC                   0x00000080U        /*!<CMDSENT flag clear bit  */
#define  SDMMC_ICR_DATAENDC                   0x00000100U        /*!<DATAEND flag clear bit  */
#define  SDMMC_ICR_DBCKENDC                   0x00000400U        /*!<DBCKEND flag clear bit  */
#define  SDMMC_ICR_SDIOITC                    0x00400000U        /*!<SDMMCIT flag clear bit   */

/******************  Bit definition for SDMMC_MASK register  *******************/
#define  SDMMC_MASK_CCRCFAILIE                0x00000001U        /*!<Command CRC Fail Interrupt Enable          */
#define  SDMMC_MASK_DCRCFAILIE                0x00000002U        /*!<Data CRC Fail Interrupt Enable             */
#define  SDMMC_MASK_CTIMEOUTIE                0x00000004U        /*!<Command TimeOut Interrupt Enable           */
#define  SDMMC_MASK_DTIMEOUTIE                0x00000008U        /*!<Data TimeOut Interrupt Enable              */
#define  SDMMC_MASK_TXUNDERRIE                0x00000010U        /*!<Tx FIFO UnderRun Error Interrupt Enable    */
#define  SDMMC_MASK_RXOVERRIE                 0x00000020U        /*!<Rx FIFO OverRun Error Interrupt Enable     */
#define  SDMMC_MASK_CMDRENDIE                 0x00000040U        /*!<Command Response Received Interrupt Enable */
#define  SDMMC_MASK_CMDSENTIE                 0x00000080U        /*!<Command Sent Interrupt Enable              */
#define  SDMMC_MASK_DATAENDIE                 0x00000100U        /*!<Data End Interrupt Enable                  */
#define  SDMMC_MASK_DBCKENDIE                 0x00000400U        /*!<Data Block End Interrupt Enable            */
#define  SDMMC_MASK_CMDACTIE                  0x00000800U        /*!<CCommand Acting Interrupt Enable           */
#define  SDMMC_MASK_TXACTIE                   0x00001000U        /*!<Data Transmit Acting Interrupt Enable      */
#define  SDMMC_MASK_RXACTIE                   0x00002000U        /*!<Data receive acting interrupt enabled      */
#define  SDMMC_MASK_TXFIFOHEIE                0x00004000U        /*!<Tx FIFO Half Empty interrupt Enable        */
#define  SDMMC_MASK_RXFIFOHFIE                0x00008000U        /*!<Rx FIFO Half Full interrupt Enable         */
#define  SDMMC_MASK_TXFIFOFIE                 0x00010000U        /*!<Tx FIFO Full interrupt Enable              */
#define  SDMMC_MASK_RXFIFOFIE                 0x00020000U        /*!<Rx FIFO Full interrupt Enable              */
#define  SDMMC_MASK_TXFIFOEIE                 0x00040000U        /*!<Tx FIFO Empty interrupt Enable             */
#define  SDMMC_MASK_RXFIFOEIE                 0x00080000U        /*!<Rx FIFO Empty interrupt Enable             */
#define  SDMMC_MASK_TXDAVLIE                  0x00100000U        /*!<Data available in Tx FIFO interrupt Enable */
#define  SDMMC_MASK_RXDAVLIE                  0x00200000U        /*!<Data available in Rx FIFO interrupt Enable */
#define  SDMMC_MASK_SDIOITIE                  0x00400000U        /*!<SDMMC Mode Interrupt Received interrupt Enable */

/*****************  Bit definition for SDMMC_FIFOCNT register  *****************/
#define  SDMMC_FIFOCNT_FIFOCOUNT              0x00FFFFFFU        /*!<Remaining number of words to be written to or read from the FIFO */

/******************  Bit definition for SDMMC_FIFO register  *******************/
#define  SDMMC_FIFO_FIFODATA                  0xFFFFFFFFU        /*!<Receive and transmit FIFO data */

/******************************************************************************/
/*                                                                            */
/*                        Serial Peripheral Interface (SPI)                   */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for SPI_CR1 register  ********************/
#define  SPI_CR1_CPHA                        0x00000001U        /*!< Clock Phase                        */
#define  SPI_CR1_CPOL                        0x00000002U        /*!< Clock Polarity                     */
#define  SPI_CR1_MSTR                        0x00000004U        /*!< Master Selection                   */
#define  SPI_CR1_BR                          0x00000038U        /*!< BR[2:0] bits (Baud Rate Control)   */
#define  SPI_CR1_BR_0                        0x00000008U        /*!< Bit 0 */
#define  SPI_CR1_BR_1                        0x00000010U        /*!< Bit 1 */
#define  SPI_CR1_BR_2                        0x00000020U        /*!< Bit 2 */
#define  SPI_CR1_SPE                         0x00000040U        /*!< SPI Enable                          */
#define  SPI_CR1_LSBFIRST                    0x00000080U        /*!< Frame Format                        */
#define  SPI_CR1_SSI                         0x00000100U        /*!< Internal slave select               */
#define  SPI_CR1_SSM                         0x00000200U        /*!< Software slave management           */
#define  SPI_CR1_RXONLY                      0x00000400U        /*!< Receive only                        */
#define  SPI_CR1_CRCL                        0x00000800U        /*!< CRC Length                          */
#define  SPI_CR1_CRCNEXT                     0x00001000U        /*!< Transmit CRC next                   */
#define  SPI_CR1_CRCEN                       0x00002000U        /*!< Hardware CRC calculation enable     */
#define  SPI_CR1_BIDIOE                      0x00004000U        /*!< Output enable in bidirectional mode */
#define  SPI_CR1_BIDIMODE                    0x00008000U        /*!< Bidirectional data mode enable      */

/*******************  Bit definition for SPI_CR2 register  ********************/
#define  SPI_CR2_RXDMAEN                     0x00000001U        /*!< Rx Buffer DMA Enable                 */
#define  SPI_CR2_TXDMAEN                     0x00000002U        /*!< Tx Buffer DMA Enable                 */
#define  SPI_CR2_SSOE                        0x00000004U        /*!< SS Output Enable                     */
#define  SPI_CR2_NSSP                        0x00000008U        /*!< NSS pulse management Enable          */
#define  SPI_CR2_FRF                         0x00000010U        /*!< Frame Format Enable                  */
#define  SPI_CR2_ERRIE                       0x00000020U        /*!< Error Interrupt Enable               */
#define  SPI_CR2_RXNEIE                      0x00000040U        /*!< RX buffer Not Empty Interrupt Enable */
#define  SPI_CR2_TXEIE                       0x00000080U        /*!< Tx buffer Empty Interrupt Enable     */
#define  SPI_CR2_DS                          0x00000F00U        /*!< DS[3:0] Data Size                    */
#define  SPI_CR2_DS_0                        0x00000100U        /*!< Bit 0 */
#define  SPI_CR2_DS_1                        0x00000200U        /*!< Bit 1 */
#define  SPI_CR2_DS_2                        0x00000400U        /*!< Bit 2 */
#define  SPI_CR2_DS_3                        0x00000800U        /*!< Bit 3 */
#define  SPI_CR2_FRXTH                       0x00001000U        /*!< FIFO reception Threshold           */
#define  SPI_CR2_LDMARX                      0x00002000U        /*!< Last DMA transfer for reception    */
#define  SPI_CR2_LDMATX                      0x00004000U        /*!< Last DMA transfer for transmission */

/********************  Bit definition for SPI_SR register  ********************/
#define  SPI_SR_RXNE                         0x00000001U        /*!< Receive buffer Not Empty  */
#define  SPI_SR_TXE                          0x00000002U        /*!< Transmit buffer Empty     */
#define  SPI_SR_CHSIDE                       0x00000004U        /*!< Channel side              */
#define  SPI_SR_UDR                          0x00000008U        /*!< Underrun flag             */
#define  SPI_SR_CRCERR                       0x00000010U        /*!< CRC Error flag            */
#define  SPI_SR_MODF                         0x00000020U        /*!< Mode fault                */
#define  SPI_SR_OVR                          0x00000040U        /*!< Overrun flag              */
#define  SPI_SR_BSY                          0x00000080U        /*!< Busy flag                 */
#define  SPI_SR_FRE                          0x00000100U        /*!< TI frame format error     */
#define  SPI_SR_FRLVL                        0x00000600U        /*!< FIFO Reception Level      */
#define  SPI_SR_FRLVL_0                      0x00000200U        /*!< Bit 0 */
#define  SPI_SR_FRLVL_1                      0x00000400U        /*!< Bit 1 */
#define  SPI_SR_FTLVL                        0x00001800U        /*!< FIFO Transmission Level   */
#define  SPI_SR_FTLVL_0                      0x00000800U        /*!< Bit 0 */
#define  SPI_SR_FTLVL_1                      0x00001000U        /*!< Bit 1 */

/********************  Bit definition for SPI_DR register  ********************/
#define  SPI_DR_DR                           0xFFFFU            /*!< Data Register */

/*******************  Bit definition for SPI_CRCPR register  ******************/
#define  SPI_CRCPR_CRCPOLY                   0xFFFFU            /*!< CRC polynomial register */

/******************  Bit definition for SPI_RXCRCR register  ******************/
#define  SPI_RXCRCR_RXCRC                    0xFFFFU            /*!< Rx CRC Register */

/******************  Bit definition for SPI_TXCRCR register  ******************/
#define  SPI_TXCRCR_TXCRC                    0xFFFFU            /*!< Tx CRC Register */

/******************  Bit definition for SPI_I2SCFGR register  *****************/
#define  SPI_I2SCFGR_CHLEN                   0x00000001U        /*!<Channel length (number of bits per audio channel) */
#define  SPI_I2SCFGR_DATLEN                  0x00000006U        /*!<DATLEN[1:0] bits (Data length to be transferred)  */
#define  SPI_I2SCFGR_DATLEN_0                0x00000002U        /*!<Bit 0 */
#define  SPI_I2SCFGR_DATLEN_1                0x00000004U        /*!<Bit 1 */
#define  SPI_I2SCFGR_CKPOL                   0x00000008U        /*!<steady state clock polarity                       */
#define  SPI_I2SCFGR_I2SSTD                  0x00000030U        /*!<I2SSTD[1:0] bits (I2S standard selection)         */
#define  SPI_I2SCFGR_I2SSTD_0                0x00000010U        /*!<Bit 0 */
#define  SPI_I2SCFGR_I2SSTD_1                0x00000020U        /*!<Bit 1 */
#define  SPI_I2SCFGR_PCMSYNC                 0x00000080U        /*!<PCM frame synchronization                         */
#define  SPI_I2SCFGR_I2SCFG                  0x00000300U        /*!<I2SCFG[1:0] bits (I2S configuration mode)         */
#define  SPI_I2SCFGR_I2SCFG_0                0x00000100U        /*!<Bit 0 */
#define  SPI_I2SCFGR_I2SCFG_1                0x00000200U        /*!<Bit 1 */
#define  SPI_I2SCFGR_I2SE                    0x00000400U        /*!<I2S Enable                                        */
#define  SPI_I2SCFGR_I2SMOD                  0x00000800U        /*!<I2S mode selection                                */
#define  SPI_I2SCFGR_ASTRTEN                 0x00001000U        /*!<Asynchronous start enable                        */

/******************  Bit definition for SPI_I2SPR register  *******************/
#define  SPI_I2SPR_I2SDIV                    0x00FFU            /*!<I2S Linear prescaler         */
#define  SPI_I2SPR_ODD                       0x0100U            /*!<Odd factor for the prescaler */
#define  SPI_I2SPR_MCKOE                     0x0200U            /*!<Master Clock Output Enable   */


/******************************************************************************/
/*                                                                            */
/*                                 SYSCFG                                     */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for SYSCFG_MEMRMP register  ***************/
#define SYSCFG_MEMRMP_MEM_BOOT         0x00000001U /*!< Boot information after Reset */


#define SYSCFG_MEMRMP_SWP_FMC          0x00000C00U /*!< FMC Memory Mapping swapping */
#define SYSCFG_MEMRMP_SWP_FMC_0        0x00000400U
#define SYSCFG_MEMRMP_SWP_FMC_1        0x00000800U

/******************  Bit definition for SYSCFG_PMC register  ******************/

#define SYSCFG_PMC_ADCxDC2              0x00070000U /*!< Refer to AN4073 on how to use this bit  */
#define SYSCFG_PMC_ADC1DC2              0x00010000U /*!< Refer to AN4073 on how to use this bit  */
#define SYSCFG_PMC_ADC2DC2              0x00020000U /*!< Refer to AN4073 on how to use this bit  */
#define SYSCFG_PMC_ADC3DC2              0x00040000U /*!< Refer to AN4073 on how to use this bit  */

#define SYSCFG_PMC_MII_RMII_SEL         0x00800000U /*!<Ethernet PHY interface selection */

/*****************  Bit definition for SYSCFG_EXTICR1 register  ***************/
#define SYSCFG_EXTICR1_EXTI0            0x000FU /*!<EXTI 0 configuration */
#define SYSCFG_EXTICR1_EXTI1            0x00F0U /*!<EXTI 1 configuration */
#define SYSCFG_EXTICR1_EXTI2            0x0F00U /*!<EXTI 2 configuration */
#define SYSCFG_EXTICR1_EXTI3            0xF000U /*!<EXTI 3 configuration */
/**
  * @brief   EXTI0 configuration
  */
#define SYSCFG_EXTICR1_EXTI0_PA         0x0000U /*!<PA[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PB         0x0001U /*!<PB[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PC         0x0002U /*!<PC[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PD         0x0003U /*!<PD[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PE         0x0004U /*!<PE[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PF         0x0005U /*!<PF[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PG         0x0006U /*!<PG[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PH         0x0007U /*!<PH[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PI         0x0008U /*!<PI[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PJ         0x0009U /*!<PJ[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PK         0x000AU /*!<PK[0] pin */

/**
  * @brief   EXTI1 configuration
  */
#define SYSCFG_EXTICR1_EXTI1_PA         0x0000U /*!<PA[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PB         0x0010U /*!<PB[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PC         0x0020U /*!<PC[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PD         0x0030U /*!<PD[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PE         0x0040U /*!<PE[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PF         0x0050U /*!<PF[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PG         0x0060U /*!<PG[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PH         0x0070U /*!<PH[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PI         0x0080U /*!<PI[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PJ         0x0090U /*!<PJ[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PK         0x00A0U /*!<PK[1] pin */

/**
  * @brief   EXTI2 configuration
  */
#define SYSCFG_EXTICR1_EXTI2_PA         0x0000U /*!<PA[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PB         0x0100U /*!<PB[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PC         0x0200U /*!<PC[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PD         0x0300U /*!<PD[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PE         0x0400U /*!<PE[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PF         0x0500U /*!<PF[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PG         0x0600U /*!<PG[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PH         0x0700U /*!<PH[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PI         0x0800U /*!<PI[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PJ         0x0900U /*!<PJ[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PK         0x0A00U /*!<PK[2] pin */

/**
  * @brief   EXTI3 configuration
  */
#define SYSCFG_EXTICR1_EXTI3_PA         0x0000U /*!<PA[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PB         0x1000U /*!<PB[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PC         0x2000U /*!<PC[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PD         0x3000U /*!<PD[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PE         0x4000U /*!<PE[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PF         0x5000U /*!<PF[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PG         0x6000U /*!<PG[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PH         0x7000U /*!<PH[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PI         0x8000U /*!<PI[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PJ         0x9000U /*!<PJ[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PK         0xA000U /*!<PK[3] pin */

/*****************  Bit definition for SYSCFG_EXTICR2 register  ***************/
#define SYSCFG_EXTICR2_EXTI4            0x000FU /*!<EXTI 4 configuration */
#define SYSCFG_EXTICR2_EXTI5            0x00F0U /*!<EXTI 5 configuration */
#define SYSCFG_EXTICR2_EXTI6            0x0F00U /*!<EXTI 6 configuration */
#define SYSCFG_EXTICR2_EXTI7            0xF000U /*!<EXTI 7 configuration */
/**
  * @brief   EXTI4 configuration
  */
#define SYSCFG_EXTICR2_EXTI4_PA         0x0000U /*!<PA[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PB         0x0001U /*!<PB[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PC         0x0002U /*!<PC[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PD         0x0003U /*!<PD[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PE         0x0004U /*!<PE[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PF         0x0005U /*!<PF[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PG         0x0006U /*!<PG[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PH         0x0007U /*!<PH[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PI         0x0008U /*!<PI[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PJ         0x0009U /*!<PJ[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PK         0x000AU /*!<PK[4] pin */

/**
  * @brief   EXTI5 configuration
  */
#define SYSCFG_EXTICR2_EXTI5_PA         0x0000U /*!<PA[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PB         0x0010U /*!<PB[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PC         0x0020U /*!<PC[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PD         0x0030U /*!<PD[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PE         0x0040U /*!<PE[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PF         0x0050U /*!<PF[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PG         0x0060U /*!<PG[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PH         0x0070U /*!<PH[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PI         0x0080U /*!<PI[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PJ         0x0090U /*!<PJ[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PK         0x00A0U /*!<PK[5] pin */

/**
  * @brief   EXTI6 configuration
  */
#define SYSCFG_EXTICR2_EXTI6_PA         0x0000U /*!<PA[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PB         0x0100U /*!<PB[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PC         0x0200U /*!<PC[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PD         0x0300U /*!<PD[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PE         0x0400U /*!<PE[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PF         0x0500U /*!<PF[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PG         0x0600U /*!<PG[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PH         0x0700U /*!<PH[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PI         0x0800U /*!<PI[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PJ         0x0900U /*!<PJ[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PK         0x0A00U /*!<PK[6] pin */

/**
  * @brief   EXTI7 configuration
  */
#define SYSCFG_EXTICR2_EXTI7_PA         0x0000U /*!<PA[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PB         0x1000U /*!<PB[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PC         0x2000U /*!<PC[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PD         0x3000U /*!<PD[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PE         0x4000U /*!<PE[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PF         0x5000U /*!<PF[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PG         0x6000U /*!<PG[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PH         0x7000U /*!<PH[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PI         0x8000U /*!<PI[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PJ         0x9000U /*!<PJ[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PK         0xA000U /*!<PK[7] pin */

/*****************  Bit definition for SYSCFG_EXTICR3 register  ***************/
#define SYSCFG_EXTICR3_EXTI8            0x000FU /*!<EXTI 8 configuration */
#define SYSCFG_EXTICR3_EXTI9            0x00F0U /*!<EXTI 9 configuration */
#define SYSCFG_EXTICR3_EXTI10           0x0F00U /*!<EXTI 10 configuration */
#define SYSCFG_EXTICR3_EXTI11           0xF000U /*!<EXTI 11 configuration */

/**
  * @brief   EXTI8 configuration
  */
#define SYSCFG_EXTICR3_EXTI8_PA         0x0000U /*!<PA[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PB         0x0001U /*!<PB[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PC         0x0002U /*!<PC[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PD         0x0003U /*!<PD[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PE         0x0004U /*!<PE[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PF         0x0005U /*!<PF[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PG         0x0006U /*!<PG[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PH         0x0007U /*!<PH[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PI         0x0008U /*!<PI[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PJ         0x0009U /*!<PJ[8] pin */

/**
  * @brief   EXTI9 configuration
  */
#define SYSCFG_EXTICR3_EXTI9_PA         0x0000U /*!<PA[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PB         0x0010U /*!<PB[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PC         0x0020U /*!<PC[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PD         0x0030U /*!<PD[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PE         0x0040U /*!<PE[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PF         0x0050U /*!<PF[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PG         0x0060U /*!<PG[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PH         0x0070U /*!<PH[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PI         0x0080U /*!<PI[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PJ         0x0090U /*!<PJ[9] pin */

/**
  * @brief   EXTI10 configuration
  */
#define SYSCFG_EXTICR3_EXTI10_PA        0x0000U /*!<PA[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PB        0x0100U /*!<PB[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PC        0x0200U /*!<PC[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PD        0x0300U /*!<PD[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PE        0x0400U /*!<PE[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PF        0x0500U /*!<PF[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PG        0x0600U /*!<PG[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PH        0x0700U /*!<PH[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PI        0x0800U /*!<PI[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PJ        0x0900U /*!<PJ[10] pin */

/**
  * @brief   EXTI11 configuration
  */
#define SYSCFG_EXTICR3_EXTI11_PA        0x0000U /*!<PA[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PB        0x1000U /*!<PB[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PC        0x2000U /*!<PC[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PD        0x3000U /*!<PD[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PE        0x4000U /*!<PE[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PF        0x5000U /*!<PF[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PG        0x6000U /*!<PG[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PH        0x7000U /*!<PH[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PI        0x8000U /*!<PI[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PJ        0x9000U /*!<PJ[11] pin */


/*****************  Bit definition for SYSCFG_EXTICR4 register  ***************/
#define SYSCFG_EXTICR4_EXTI12           0x000FU /*!<EXTI 12 configuration */
#define SYSCFG_EXTICR4_EXTI13           0x00F0U /*!<EXTI 13 configuration */
#define SYSCFG_EXTICR4_EXTI14           0x0F00U /*!<EXTI 14 configuration */
#define SYSCFG_EXTICR4_EXTI15           0xF000U /*!<EXTI 15 configuration */
/**
  * @brief   EXTI12 configuration
  */
#define SYSCFG_EXTICR4_EXTI12_PA        0x0000U /*!<PA[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PB        0x0001U /*!<PB[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PC        0x0002U /*!<PC[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PD        0x0003U /*!<PD[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PE        0x0004U /*!<PE[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PF        0x0005U /*!<PF[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PG        0x0006U /*!<PG[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PH        0x0007U /*!<PH[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PI        0x0008U /*!<PI[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PJ        0x0009U /*!<PJ[12] pin */

/**
  * @brief   EXTI13 configuration
  */
#define SYSCFG_EXTICR4_EXTI13_PA        0x0000U /*!<PA[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PB        0x0010U /*!<PB[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PC        0x0020U /*!<PC[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PD        0x0030U /*!<PD[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PE        0x0040U /*!<PE[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PF        0x0050U /*!<PF[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PG        0x0060U /*!<PG[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PH        0x0070U /*!<PH[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PI        0x0080U /*!<PI[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PJ        0x0090U /*!<PJ[13] pin */

/**
  * @brief   EXTI14 configuration
  */
#define SYSCFG_EXTICR4_EXTI14_PA        0x0000U /*!<PA[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PB        0x0100U /*!<PB[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PC        0x0200U /*!<PC[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PD        0x0300U /*!<PD[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PE        0x0400U /*!<PE[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PF        0x0500U /*!<PF[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PG        0x0600U /*!<PG[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PH        0x0700U /*!<PH[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PI        0x0800U /*!<PI[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PJ        0x0900U /*!<PJ[14] pin */

/**
  * @brief   EXTI15 configuration
  */
#define SYSCFG_EXTICR4_EXTI15_PA        0x0000U /*!<PA[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PB        0x1000U /*!<PB[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PC        0x2000U /*!<PC[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PD        0x3000U /*!<PD[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PE        0x4000U /*!<PE[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PF        0x5000U /*!<PF[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PG        0x6000U /*!<PG[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PH        0x7000U /*!<PH[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PI        0x8000U /*!<PI[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PJ        0x9000U /*!<PJ[15] pin */


/******************  Bit definition for SYSCFG_CMPCR register  ****************/
#define SYSCFG_CMPCR_CMP_PD             0x00000001U /*!<Compensation cell power-down */
#define SYSCFG_CMPCR_READY              0x00000100U /*!<Compensation cell ready flag */

/******************************************************************************/
/*                                                                            */
/*                                    TIM                                     */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for TIM_CR1 register  ********************/
#define  TIM_CR1_CEN                         0x0001U            /*!<Counter enable        */
#define  TIM_CR1_UDIS                        0x0002U            /*!<Update disable        */
#define  TIM_CR1_URS                         0x0004U            /*!<Update request source */
#define  TIM_CR1_OPM                         0x0008U            /*!<One pulse mode        */
#define  TIM_CR1_DIR                         0x0010U            /*!<Direction             */

#define  TIM_CR1_CMS                         0x0060U            /*!<CMS[1:0] bits (Center-aligned mode selection) */
#define  TIM_CR1_CMS_0                       0x0020U            /*!<Bit 0 */
#define  TIM_CR1_CMS_1                       0x0040U            /*!<Bit 1 */

#define  TIM_CR1_ARPE                        0x0080U            /*!<Auto-reload preload enable     */

#define  TIM_CR1_CKD                         0x0300U            /*!<CKD[1:0] bits (clock division) */
#define  TIM_CR1_CKD_0                       0x0100U            /*!<Bit 0 */
#define  TIM_CR1_CKD_1                       0x0200U            /*!<Bit 1 */
#define  TIM_CR1_UIFREMAP                    0x0800U            /*!<UIF status bit */

/*******************  Bit definition for TIM_CR2 register  ********************/
#define  TIM_CR2_CCPC                        0x00000001U            /*!<Capture/Compare Preloaded Control        */
#define  TIM_CR2_CCUS                        0x00000004U            /*!<Capture/Compare Control Update Selection */
#define  TIM_CR2_CCDS                        0x00000008U            /*!<Capture/Compare DMA Selection            */

#define  TIM_CR2_OIS5                        0x00010000U            /*!<Output Idle state 4 (OC4 output) */
#define  TIM_CR2_OIS6                        0x00040000U            /*!<Output Idle state 4 (OC4 output) */

#define  TIM_CR2_MMS                         0x0070U           /*!<MMS[2:0] bits (Master Mode Selection) */
#define  TIM_CR2_MMS_0                       0x0010U           /*!<Bit 0 */
#define  TIM_CR2_MMS_1                       0x0020U           /*!<Bit 1 */
#define  TIM_CR2_MMS_2                       0x0040U           /*!<Bit 2 */

#define  TIM_CR2_MMS2                        0x00F00000U            /*!<MMS[2:0] bits (Master Mode Selection) */
#define  TIM_CR2_MMS2_0                      0x00100000U            /*!<Bit 0 */
#define  TIM_CR2_MMS2_1                      0x00200000U            /*!<Bit 1 */
#define  TIM_CR2_MMS2_2                      0x00400000U            /*!<Bit 2 */
#define  TIM_CR2_MMS2_3                      0x00800000U            /*!<Bit 2 */

#define  TIM_CR2_TI1S                        0x0080U            /*!<TI1 Selection */
#define  TIM_CR2_OIS1                        0x0100U            /*!<Output Idle state 1 (OC1 output)  */
#define  TIM_CR2_OIS1N                       0x0200U            /*!<Output Idle state 1 (OC1N output) */
#define  TIM_CR2_OIS2                        0x0400U            /*!<Output Idle state 2 (OC2 output)  */
#define  TIM_CR2_OIS2N                       0x0800U            /*!<Output Idle state 2 (OC2N output) */
#define  TIM_CR2_OIS3                        0x1000U            /*!<Output Idle state 3 (OC3 output)  */
#define  TIM_CR2_OIS3N                       0x2000U            /*!<Output Idle state 3 (OC3N output) */
#define  TIM_CR2_OIS4                        0x4000U            /*!<Output Idle state 4 (OC4 output)  */

/*******************  Bit definition for TIM_SMCR register  *******************/
#define  TIM_SMCR_SMS                        0x00010007U            /*!<SMS[2:0] bits (Slave mode selection)    */
#define  TIM_SMCR_SMS_0                      0x00000001U            /*!<Bit 0 */
#define  TIM_SMCR_SMS_1                      0x00000002U            /*!<Bit 1 */
#define  TIM_SMCR_SMS_2                      0x00000004U            /*!<Bit 2 */
#define  TIM_SMCR_SMS_3                      0x00010000U            /*!<Bit 3 */
#define  TIM_SMCR_OCCS                       0x00000008U            /*!< OCREF clear selection */

#define  TIM_SMCR_TS                         0x0070U            /*!<TS[2:0] bits (Trigger selection)        */
#define  TIM_SMCR_TS_0                       0x0010U            /*!<Bit 0 */
#define  TIM_SMCR_TS_1                       0x0020U            /*!<Bit 1 */
#define  TIM_SMCR_TS_2                       0x0040U            /*!<Bit 2 */

#define  TIM_SMCR_MSM                        0x0080U            /*!<Master/slave mode                       */

#define  TIM_SMCR_ETF                        0x0F00U            /*!<ETF[3:0] bits (External trigger filter) */
#define  TIM_SMCR_ETF_0                      0x0100U            /*!<Bit 0 */
#define  TIM_SMCR_ETF_1                      0x0200U            /*!<Bit 1 */
#define  TIM_SMCR_ETF_2                      0x0400U            /*!<Bit 2 */
#define  TIM_SMCR_ETF_3                      0x0800U            /*!<Bit 3 */

#define  TIM_SMCR_ETPS                       0x3000U            /*!<ETPS[1:0] bits (External trigger prescaler) */
#define  TIM_SMCR_ETPS_0                     0x1000U            /*!<Bit 0 */
#define  TIM_SMCR_ETPS_1                     0x2000U            /*!<Bit 1 */

#define  TIM_SMCR_ECE                        0x4000U            /*!<External clock enable     */
#define  TIM_SMCR_ETP                        0x8000U            /*!<External trigger polarity */

/*******************  Bit definition for TIM_DIER register  *******************/
#define  TIM_DIER_UIE                        0x0001U            /*!<Update interrupt enable */
#define  TIM_DIER_CC1IE                      0x0002U            /*!<Capture/Compare 1 interrupt enable   */
#define  TIM_DIER_CC2IE                      0x0004U            /*!<Capture/Compare 2 interrupt enable   */
#define  TIM_DIER_CC3IE                      0x0008U            /*!<Capture/Compare 3 interrupt enable   */
#define  TIM_DIER_CC4IE                      0x0010U            /*!<Capture/Compare 4 interrupt enable   */
#define  TIM_DIER_COMIE                      0x0020U            /*!<COM interrupt enable                 */
#define  TIM_DIER_TIE                        0x0040U            /*!<Trigger interrupt enable             */
#define  TIM_DIER_BIE                        0x0080U            /*!<Break interrupt enable               */
#define  TIM_DIER_UDE                        0x0100U            /*!<Update DMA request enable            */
#define  TIM_DIER_CC1DE                      0x0200U            /*!<Capture/Compare 1 DMA request enable */
#define  TIM_DIER_CC2DE                      0x0400U            /*!<Capture/Compare 2 DMA request enable */
#define  TIM_DIER_CC3DE                      0x0800U            /*!<Capture/Compare 3 DMA request enable */
#define  TIM_DIER_CC4DE                      0x1000U            /*!<Capture/Compare 4 DMA request enable */
#define  TIM_DIER_COMDE                      0x2000U            /*!<COM DMA request enable               */
#define  TIM_DIER_TDE                        0x4000U            /*!<Trigger DMA request enable           */

/********************  Bit definition for TIM_SR register  ********************/
#define  TIM_SR_UIF                          0x0001U            /*!<Update interrupt Flag              */
#define  TIM_SR_CC1IF                        0x0002U            /*!<Capture/Compare 1 interrupt Flag   */
#define  TIM_SR_CC2IF                        0x0004U            /*!<Capture/Compare 2 interrupt Flag   */
#define  TIM_SR_CC3IF                        0x0008U            /*!<Capture/Compare 3 interrupt Flag   */
#define  TIM_SR_CC4IF                        0x0010U            /*!<Capture/Compare 4 interrupt Flag   */
#define  TIM_SR_COMIF                        0x0020U            /*!<COM interrupt Flag                 */
#define  TIM_SR_TIF                          0x0040U            /*!<Trigger interrupt Flag             */
#define  TIM_SR_BIF                          0x0080U            /*!<Break interrupt Flag               */
#define  TIM_SR_B2IF                         0x0100U            /*!<Break2 interrupt Flag               */
#define  TIM_SR_CC1OF                        0x0200U            /*!<Capture/Compare 1 Overcapture Flag */
#define  TIM_SR_CC2OF                        0x0400U            /*!<Capture/Compare 2 Overcapture Flag */
#define  TIM_SR_CC3OF                        0x0800U            /*!<Capture/Compare 3 Overcapture Flag */
#define  TIM_SR_CC4OF                        0x1000U            /*!<Capture/Compare 4 Overcapture Flag */

/*******************  Bit definition for TIM_EGR register  ********************/
#define  TIM_EGR_UG                          0x00000001U               /*!<Update Generation                         */
#define  TIM_EGR_CC1G                        0x00000002U               /*!<Capture/Compare 1 Generation              */
#define  TIM_EGR_CC2G                        0x00000004U               /*!<Capture/Compare 2 Generation              */
#define  TIM_EGR_CC3G                        0x00000008U               /*!<Capture/Compare 3 Generation              */
#define  TIM_EGR_CC4G                        0x00000010U               /*!<Capture/Compare 4 Generation              */
#define  TIM_EGR_COMG                        0x00000020U               /*!<Capture/Compare Control Update Generation */
#define  TIM_EGR_TG                          0x00000040U               /*!<Trigger Generation                        */
#define  TIM_EGR_BG                          0x00000080U               /*!<Break Generation                          */
#define  TIM_EGR_B2G                         0x00000100U              /*!<Break2 Generation                          */

/******************  Bit definition for TIM_CCMR1 register  *******************/
#define  TIM_CCMR1_CC1S                      0x00000003U            /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
#define  TIM_CCMR1_CC1S_0                    0x00000001U            /*!<Bit 0 */
#define  TIM_CCMR1_CC1S_1                    0x00000002U            /*!<Bit 1 */

#define  TIM_CCMR1_OC1FE                     0x00000004U            /*!<Output Compare 1 Fast enable                 */
#define  TIM_CCMR1_OC1PE                     0x00000008U            /*!<Output Compare 1 Preload enable              */

#define  TIM_CCMR1_OC1M                      0x00010070U            /*!<OC1M[2:0] bits (Output Compare 1 Mode)       */
#define  TIM_CCMR1_OC1M_0                    0x00000010U            /*!<Bit 0 */
#define  TIM_CCMR1_OC1M_1                    0x00000020U            /*!<Bit 1 */
#define  TIM_CCMR1_OC1M_2                    0x00000040U            /*!<Bit 2 */
#define  TIM_CCMR1_OC1M_3                    0x00010000U            /*!<Bit 3 */

#define  TIM_CCMR1_OC1CE                     0x00000080U            /*!<Output Compare 1Clear Enable                 */

#define  TIM_CCMR1_CC2S                      0x00000300U            /*!<CC2S[1:0] bits (Capture/Compare 2 Selection) */
#define  TIM_CCMR1_CC2S_0                    0x00000100U            /*!<Bit 0 */
#define  TIM_CCMR1_CC2S_1                    0x00000200U            /*!<Bit 1 */

#define  TIM_CCMR1_OC2FE                     0x00000400U            /*!<Output Compare 2 Fast enable                 */
#define  TIM_CCMR1_OC2PE                     0x00000800U            /*!<Output Compare 2 Preload enable              */

#define  TIM_CCMR1_OC2M                      0x01007000U            /*!<OC2M[2:0] bits (Output Compare 2 Mode)       */
#define  TIM_CCMR1_OC2M_0                    0x00001000U            /*!<Bit 0 */
#define  TIM_CCMR1_OC2M_1                    0x00002000U            /*!<Bit 1 */
#define  TIM_CCMR1_OC2M_2                    0x00004000U            /*!<Bit 2 */
#define  TIM_CCMR1_OC2M_3                    0x01000000U            /*!<Bit 3 */

#define  TIM_CCMR1_OC2CE                     0x00008000U            /*!<Output Compare 2 Clear Enable */

/*----------------------------------------------------------------------------*/

#define  TIM_CCMR1_IC1PSC                    0x000CU            /*!<IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
#define  TIM_CCMR1_IC1PSC_0                  0x0004U            /*!<Bit 0 */
#define  TIM_CCMR1_IC1PSC_1                  0x0008U            /*!<Bit 1 */

#define  TIM_CCMR1_IC1F                      0x00F0U            /*!<IC1F[3:0] bits (Input Capture 1 Filter)      */
#define  TIM_CCMR1_IC1F_0                    0x0010U            /*!<Bit 0 */
#define  TIM_CCMR1_IC1F_1                    0x0020U            /*!<Bit 1 */
#define  TIM_CCMR1_IC1F_2                    0x0040U            /*!<Bit 2 */
#define  TIM_CCMR1_IC1F_3                    0x0080U            /*!<Bit 3 */

#define  TIM_CCMR1_IC2PSC                    0x0C00U            /*!<IC2PSC[1:0] bits (Input Capture 2 Prescaler)  */
#define  TIM_CCMR1_IC2PSC_0                  0x0400U            /*!<Bit 0 */
#define  TIM_CCMR1_IC2PSC_1                  0x0800U            /*!<Bit 1 */

#define  TIM_CCMR1_IC2F                      0xF000U            /*!<IC2F[3:0] bits (Input Capture 2 Filter)       */
#define  TIM_CCMR1_IC2F_0                    0x1000U            /*!<Bit 0 */
#define  TIM_CCMR1_IC2F_1                    0x2000U            /*!<Bit 1 */
#define  TIM_CCMR1_IC2F_2                    0x4000U            /*!<Bit 2 */
#define  TIM_CCMR1_IC2F_3                    0x8000U            /*!<Bit 3 */

/******************  Bit definition for TIM_CCMR2 register  *******************/
#define  TIM_CCMR2_CC3S                      0x00000003U        /*!<CC3S[1:0] bits (Capture/Compare 3 Selection)  */
#define  TIM_CCMR2_CC3S_0                    0x00000001U        /*!<Bit 0 */
#define  TIM_CCMR2_CC3S_1                    0x00000002U        /*!<Bit 1 */

#define  TIM_CCMR2_OC3FE                     0x00000004U        /*!<Output Compare 3 Fast enable           */
#define  TIM_CCMR2_OC3PE                     0x00000008U        /*!<Output Compare 3 Preload enable        */

#define  TIM_CCMR2_OC3M                      0x00010070U        /*!<OC3M[2:0] bits (Output Compare 3 Mode) */
#define  TIM_CCMR2_OC3M_0                    0x00000010U        /*!<Bit 0 */
#define  TIM_CCMR2_OC3M_1                    0x00000020U        /*!<Bit 1 */
#define  TIM_CCMR2_OC3M_2                    0x00000040U        /*!<Bit 2 */
#define  TIM_CCMR2_OC3M_3                    0x00010000U        /*!<Bit 3 */



#define  TIM_CCMR2_OC3CE                     0x00000080U        /*!<Output Compare 3 Clear Enable */

#define  TIM_CCMR2_CC4S                      0x00000300U        /*!<CC4S[1:0] bits (Capture/Compare 4 Selection) */
#define  TIM_CCMR2_CC4S_0                    0x00000100U        /*!<Bit 0 */
#define  TIM_CCMR2_CC4S_1                    0x00000200U        /*!<Bit 1 */

#define  TIM_CCMR2_OC4FE                     0x00000400U        /*!<Output Compare 4 Fast enable    */
#define  TIM_CCMR2_OC4PE                     0x00000800U        /*!<Output Compare 4 Preload enable */

#define  TIM_CCMR2_OC4M                      0x01007000U        /*!<OC4M[2:0] bits (Output Compare 4 Mode) */
#define  TIM_CCMR2_OC4M_0                    0x00001000U        /*!<Bit 0 */
#define  TIM_CCMR2_OC4M_1                    0x00002000U        /*!<Bit 1 */
#define  TIM_CCMR2_OC4M_2                    0x00004000U        /*!<Bit 2 */
#define  TIM_CCMR2_OC4M_3                    0x01000000U        /*!<Bit 3 */

#define  TIM_CCMR2_OC4CE                     0x8000U            /*!<Output Compare 4 Clear Enable */

/*----------------------------------------------------------------------------*/

#define  TIM_CCMR2_IC3PSC                    0x000CU            /*!<IC3PSC[1:0] bits (Input Capture 3 Prescaler) */
#define  TIM_CCMR2_IC3PSC_0                  0x0004U            /*!<Bit 0 */
#define  TIM_CCMR2_IC3PSC_1                  0x0008U            /*!<Bit 1 */

#define  TIM_CCMR2_IC3F                      0x00F0U            /*!<IC3F[3:0] bits (Input Capture 3 Filter) */
#define  TIM_CCMR2_IC3F_0                    0x0010U            /*!<Bit 0 */
#define  TIM_CCMR2_IC3F_1                    0x0020U            /*!<Bit 1 */
#define  TIM_CCMR2_IC3F_2                    0x0040U            /*!<Bit 2 */
#define  TIM_CCMR2_IC3F_3                    0x0080U            /*!<Bit 3 */

#define  TIM_CCMR2_IC4PSC                    0x0C00U            /*!<IC4PSC[1:0] bits (Input Capture 4 Prescaler) */
#define  TIM_CCMR2_IC4PSC_0                  0x0400U            /*!<Bit 0 */
#define  TIM_CCMR2_IC4PSC_1                  0x0800U            /*!<Bit 1 */

#define  TIM_CCMR2_IC4F                      0xF000U            /*!<IC4F[3:0] bits (Input Capture 4 Filter) */
#define  TIM_CCMR2_IC4F_0                    0x1000U            /*!<Bit 0 */
#define  TIM_CCMR2_IC4F_1                    0x2000U            /*!<Bit 1 */
#define  TIM_CCMR2_IC4F_2                    0x4000U            /*!<Bit 2 */
#define  TIM_CCMR2_IC4F_3                    0x8000U            /*!<Bit 3 */

/*******************  Bit definition for TIM_CCER register  *******************/
#define  TIM_CCER_CC1E                       0x00000001U            /*!<Capture/Compare 1 output enable */
#define  TIM_CCER_CC1P                       0x00000002U            /*!<Capture/Compare 1 output Polarity */
#define  TIM_CCER_CC1NE                      0x00000004U            /*!<Capture/Compare 1 Complementary output enable */
#define  TIM_CCER_CC1NP                      0x00000008U            /*!<Capture/Compare 1 Complementary output Polarity */
#define  TIM_CCER_CC2E                       0x00000010U            /*!<Capture/Compare 2 output enable */
#define  TIM_CCER_CC2P                       0x00000020U            /*!<Capture/Compare 2 output Polarity */
#define  TIM_CCER_CC2NE                      0x00000040U            /*!<Capture/Compare 2 Complementary output enable */
#define  TIM_CCER_CC2NP                      0x00000080U            /*!<Capture/Compare 2 Complementary output Polarity */
#define  TIM_CCER_CC3E                       0x00000100U            /*!<Capture/Compare 3 output enable */
#define  TIM_CCER_CC3P                       0x00000200U            /*!<Capture/Compare 3 output Polarity */
#define  TIM_CCER_CC3NE                      0x00000400U            /*!<Capture/Compare 3 Complementary output enable */
#define  TIM_CCER_CC3NP                      0x00000800U            /*!<Capture/Compare 3 Complementary output Polarity */
#define  TIM_CCER_CC4E                       0x00001000U            /*!<Capture/Compare 4 output enable */
#define  TIM_CCER_CC4P                       0x00002000U            /*!<Capture/Compare 4 output Polarity */
#define  TIM_CCER_CC4NP                      0x00008000U            /*!<Capture/Compare 4 Complementary output Polarity */
#define  TIM_CCER_CC5E                       0x00010000U            /*!<Capture/Compare 5 output enable */
#define  TIM_CCER_CC5P                       0x00020000U            /*!<Capture/Compare 5 output Polarity */
#define  TIM_CCER_CC6E                       0x00100000U            /*!<Capture/Compare 6 output enable */
#define  TIM_CCER_CC6P                       0x00200000U            /*!<Capture/Compare 6 output Polarity */


/*******************  Bit definition for TIM_CNT register  ********************/
#define  TIM_CNT_CNT                         0xFFFFU            /*!<Counter Value            */

/*******************  Bit definition for TIM_PSC register  ********************/
#define  TIM_PSC_PSC                         0xFFFFU            /*!<Prescaler Value          */

/*******************  Bit definition for TIM_ARR register  ********************/
#define  TIM_ARR_ARR                         0xFFFFU            /*!<actual auto-reload Value */

/*******************  Bit definition for TIM_RCR register  ********************/
#define  TIM_RCR_REP                         ((uint8_t)0xFFU)               /*!<Repetition Counter Value */

/*******************  Bit definition for TIM_CCR1 register  *******************/
#define  TIM_CCR1_CCR1                       0xFFFFU            /*!<Capture/Compare 1 Value  */

/*******************  Bit definition for TIM_CCR2 register  *******************/
#define  TIM_CCR2_CCR2                       0xFFFFU            /*!<Capture/Compare 2 Value  */

/*******************  Bit definition for TIM_CCR3 register  *******************/
#define  TIM_CCR3_CCR3                       0xFFFFU            /*!<Capture/Compare 3 Value  */

/*******************  Bit definition for TIM_CCR4 register  *******************/
#define  TIM_CCR4_CCR4                       0xFFFFU            /*!<Capture/Compare 4 Value  */

/*******************  Bit definition for TIM_BDTR register  *******************/
#define  TIM_BDTR_DTG                        0x000000FFU            /*!<DTG[0:7] bits (Dead-Time Generator set-up) */
#define  TIM_BDTR_DTG_0                      0x00000001U            /*!<Bit 0 */
#define  TIM_BDTR_DTG_1                      0x00000002U            /*!<Bit 1 */
#define  TIM_BDTR_DTG_2                      0x00000004U            /*!<Bit 2 */
#define  TIM_BDTR_DTG_3                      0x00000008U            /*!<Bit 3 */
#define  TIM_BDTR_DTG_4                      0x00000010U            /*!<Bit 4 */
#define  TIM_BDTR_DTG_5                      0x00000020U            /*!<Bit 5 */
#define  TIM_BDTR_DTG_6                      0x00000040U            /*!<Bit 6 */
#define  TIM_BDTR_DTG_7                      0x00000080U            /*!<Bit 7 */

#define  TIM_BDTR_LOCK                       0x00000300U            /*!<LOCK[1:0] bits (Lock Configuration) */
#define  TIM_BDTR_LOCK_0                     0x00000100U            /*!<Bit 0 */
#define  TIM_BDTR_LOCK_1                     0x00000200U            /*!<Bit 1 */

#define  TIM_BDTR_OSSI                       0x00000400U            /*!<Off-State Selection for Idle mode */
#define  TIM_BDTR_OSSR                       0x00000800U            /*!<Off-State Selection for Run mode  */
#define  TIM_BDTR_BKE                        0x00001000U            /*!<Break enable                      */
#define  TIM_BDTR_BKP                        0x00002000U            /*!<Break Polarity                    */
#define  TIM_BDTR_AOE                        0x00004000U            /*!<Automatic Output enable           */
#define  TIM_BDTR_MOE                        0x00008000U            /*!<Main Output enable                */
#define  TIM_BDTR_BKF                        0x000F0000U            /*!<Break Filter for Break1 */
#define  TIM_BDTR_BK2F                       0x00F00000U            /*!<Break Filter for Break2 */
#define  TIM_BDTR_BK2E                       0x01000000U            /*!<Break enable for Break2 */
#define  TIM_BDTR_BK2P                       0x02000000U            /*!<Break Polarity for Break2 */

/*******************  Bit definition for TIM_DCR register  ********************/
#define  TIM_DCR_DBA                         0x001FU            /*!<DBA[4:0] bits (DMA Base Address) */
#define  TIM_DCR_DBA_0                       0x0001U            /*!<Bit 0 */
#define  TIM_DCR_DBA_1                       0x0002U            /*!<Bit 1 */
#define  TIM_DCR_DBA_2                       0x0004U            /*!<Bit 2 */
#define  TIM_DCR_DBA_3                       0x0008U            /*!<Bit 3 */
#define  TIM_DCR_DBA_4                       0x0010U            /*!<Bit 4 */

#define  TIM_DCR_DBL                         0x1F00U            /*!<DBL[4:0] bits (DMA Burst Length) */
#define  TIM_DCR_DBL_0                       0x0100U            /*!<Bit 0 */
#define  TIM_DCR_DBL_1                       0x0200U            /*!<Bit 1 */
#define  TIM_DCR_DBL_2                       0x0400U            /*!<Bit 2 */
#define  TIM_DCR_DBL_3                       0x0800U            /*!<Bit 3 */
#define  TIM_DCR_DBL_4                       0x1000U            /*!<Bit 4 */

/*******************  Bit definition for TIM_DMAR register  *******************/
#define  TIM_DMAR_DMAB                       0xFFFFU            /*!<DMA register for burst accesses                    */

/*******************  Bit definition for TIM_OR regiter  *********************/
#define TIM_OR_TI4_RMP                       0x00C0U            /*!<TI4_RMP[1:0] bits (TIM5 Input 4 remap)             */
#define TIM_OR_TI4_RMP_0                     0x0040U            /*!<Bit 0 */
#define TIM_OR_TI4_RMP_1                     0x0080U            /*!<Bit 1 */
#define TIM_OR_ITR1_RMP                      0x0C00U            /*!<ITR1_RMP[1:0] bits (TIM2 Internal trigger 1 remap) */
#define TIM_OR_ITR1_RMP_0                    0x0400U            /*!<Bit 0 */
#define TIM_OR_ITR1_RMP_1                    0x0800U            /*!<Bit 1 */

/******************  Bit definition for TIM_CCMR3 register  *******************/
#define  TIM_CCMR3_OC5FE                     0x00000004U            /*!<Output Compare 5 Fast enable */
#define  TIM_CCMR3_OC5PE                     0x00000008U            /*!<Output Compare 5 Preload enable */

#define  TIM_CCMR3_OC5M                      0x00010070U            /*!<OC5M[2:0] bits (Output Compare 5 Mode) */
#define  TIM_CCMR3_OC5M_0                    0x00000010U            /*!<Bit 0 */
#define  TIM_CCMR3_OC5M_1                    0x00000020U            /*!<Bit 1 */
#define  TIM_CCMR3_OC5M_2                    0x00000040U            /*!<Bit 2 */
#define  TIM_CCMR3_OC5M_3                    0x00010000U            /*!<Bit 3 */

#define  TIM_CCMR3_OC5CE                     0x00000080U            /*!<Output Compare 5 Clear Enable */

#define  TIM_CCMR3_OC6FE                     0x00000400U            /*!<Output Compare 4 Fast enable */
#define  TIM_CCMR3_OC6PE                     0x00000800U            /*!<Output Compare 4 Preload enable */

#define  TIM_CCMR3_OC6M                      0x01007000U            /*!<OC4M[2:0] bits (Output Compare 4 Mode) */
#define  TIM_CCMR3_OC6M_0                    0x00001000U            /*!<Bit 0 */
#define  TIM_CCMR3_OC6M_1                    0x00002000U            /*!<Bit 1 */
#define  TIM_CCMR3_OC6M_2                    0x00004000U            /*!<Bit 2 */
#define  TIM_CCMR3_OC6M_3                    0x01000000U            /*!<Bit 3 */

#define  TIM_CCMR3_OC6CE                     0x00008000U            /*!<Output Compare 4 Clear Enable */

/*******************  Bit definition for TIM_CCR5 register  *******************/
#define  TIM_CCR5_CCR5                       0xFFFFFFFFU        /*!<Capture/Compare 5 Value */
#define  TIM_CCR5_GC5C1                      0x20000000U        /*!<Group Channel 5 and Channel 1 */
#define  TIM_CCR5_GC5C2                      0x40000000U        /*!<Group Channel 5 and Channel 2 */
#define  TIM_CCR5_GC5C3                      0x80000000U        /*!<Group Channel 5 and Channel 3 */

/*******************  Bit definition for TIM_CCR6 register  *******************/
#define  TIM_CCR6_CCR6                       ((uint16_t)0xFFFFU)            /*!<Capture/Compare 6 Value */


/******************************************************************************/
/*                                                                            */
/*                         Low Power Timer (LPTIM)                            */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for LPTIM_ISR register  *******************/
#define  LPTIM_ISR_CMPM                         0x00000001U            /*!< Compare match                       */
#define  LPTIM_ISR_ARRM                         0x00000002U            /*!< Autoreload match                    */
#define  LPTIM_ISR_EXTTRIG                      0x00000004U            /*!< External trigger edge event         */
#define  LPTIM_ISR_CMPOK                        0x00000008U            /*!< Compare register update OK          */
#define  LPTIM_ISR_ARROK                        0x00000010U            /*!< Autoreload register update OK       */
#define  LPTIM_ISR_UP                           0x00000020U            /*!< Counter direction change down to up */
#define  LPTIM_ISR_DOWN                         0x00000040U            /*!< Counter direction change up to down */

/******************  Bit definition for LPTIM_ICR register  *******************/
#define  LPTIM_ICR_CMPMCF                       0x00000001U            /*!< Compare match Clear Flag                       */
#define  LPTIM_ICR_ARRMCF                       0x00000002U            /*!< Autoreload match Clear Flag                    */
#define  LPTIM_ICR_EXTTRIGCF                    0x00000004U            /*!< External trigger edge event Clear Flag         */
#define  LPTIM_ICR_CMPOKCF                      0x00000008U            /*!< Compare register update OK Clear Flag          */
#define  LPTIM_ICR_ARROKCF                      0x00000010U            /*!< Autoreload register update OK Clear Flag       */
#define  LPTIM_ICR_UPCF                         0x00000020U            /*!< Counter direction change down to up Clear Flag */
#define  LPTIM_ICR_DOWNCF                       0x00000040U            /*!< Counter direction change up to down Clear Flag */

/******************  Bit definition for LPTIM_IER register *******************/
#define  LPTIM_IER_CMPMIE                       0x00000001U            /*!< Compare match Interrupt Enable                       */
#define  LPTIM_IER_ARRMIE                       0x00000002U            /*!< Autoreload match Interrupt Enable                    */
#define  LPTIM_IER_EXTTRIGIE                    0x00000004U            /*!< External trigger edge event Interrupt Enable         */
#define  LPTIM_IER_CMPOKIE                      0x00000008U            /*!< Compare register update OK Interrupt Enable          */
#define  LPTIM_IER_ARROKIE                      0x00000010U            /*!< Autoreload register update OK Interrupt Enable       */
#define  LPTIM_IER_UPIE                         0x00000020U            /*!< Counter direction change down to up Interrupt Enable */
#define  LPTIM_IER_DOWNIE                       0x00000040U            /*!< Counter direction change up to down Interrupt Enable */

/******************  Bit definition for LPTIM_CFGR register*******************/
#define  LPTIM_CFGR_CKSEL                       0x00000001U             /*!< Clock selector */

#define  LPTIM_CFGR_CKPOL                       0x00000006U             /*!< CKPOL[1:0] bits (Clock polarity) */
#define  LPTIM_CFGR_CKPOL_0                     0x00000002U             /*!< Bit 0 */
#define  LPTIM_CFGR_CKPOL_1                     0x00000004U             /*!< Bit 1 */

#define  LPTIM_CFGR_CKFLT                       0x00000018U             /*!< CKFLT[1:0] bits (Configurable digital filter for external clock) */
#define  LPTIM_CFGR_CKFLT_0                     0x00000008U             /*!< Bit 0 */
#define  LPTIM_CFGR_CKFLT_1                     0x00000010U             /*!< Bit 1 */

#define  LPTIM_CFGR_TRGFLT                      0x000000C0U             /*!< TRGFLT[1:0] bits (Configurable digital filter for trigger) */
#define  LPTIM_CFGR_TRGFLT_0                    0x00000040U             /*!< Bit 0 */
#define  LPTIM_CFGR_TRGFLT_1                    0x00000080U             /*!< Bit 1 */

#define  LPTIM_CFGR_PRESC                       0x00000E00U             /*!< PRESC[2:0] bits (Clock prescaler) */
#define  LPTIM_CFGR_PRESC_0                     0x00000200U             /*!< Bit 0 */
#define  LPTIM_CFGR_PRESC_1                     0x00000400U             /*!< Bit 1 */
#define  LPTIM_CFGR_PRESC_2                     0x00000800U             /*!< Bit 2 */

#define  LPTIM_CFGR_TRIGSEL                     0x0000E000U             /*!< TRIGSEL[2:0]] bits (Trigger selector) */
#define  LPTIM_CFGR_TRIGSEL_0                   0x00002000U             /*!< Bit 0 */
#define  LPTIM_CFGR_TRIGSEL_1                   0x00004000U             /*!< Bit 1 */
#define  LPTIM_CFGR_TRIGSEL_2                   0x00008000U             /*!< Bit 2 */

#define  LPTIM_CFGR_TRIGEN                      0x00060000U             /*!< TRIGEN[1:0] bits (Trigger enable and polarity) */
#define  LPTIM_CFGR_TRIGEN_0                    0x00020000U             /*!< Bit 0 */
#define  LPTIM_CFGR_TRIGEN_1                    0x00040000U             /*!< Bit 1 */

#define  LPTIM_CFGR_TIMOUT                      0x00080000U             /*!< Timout enable           */
#define  LPTIM_CFGR_WAVE                        0x00100000U             /*!< Waveform shape          */
#define  LPTIM_CFGR_WAVPOL                      0x00200000U             /*!< Waveform shape polarity */
#define  LPTIM_CFGR_PRELOAD                     0x00400000U             /*!< Reg update mode         */
#define  LPTIM_CFGR_COUNTMODE                   0x00800000U             /*!< Counter mode enable     */
#define  LPTIM_CFGR_ENC                         0x01000000U             /*!< Encoder mode enable     */

/******************  Bit definition for LPTIM_CR register  ********************/
#define  LPTIM_CR_ENABLE                        0x00000001U             /*!< LPTIMer enable                 */
#define  LPTIM_CR_SNGSTRT                       0x00000002U             /*!< Timer start in single mode     */
#define  LPTIM_CR_CNTSTRT                       0x00000004U             /*!< Timer start in continuous mode */

/******************  Bit definition for LPTIM_CMP register *******************/
#define  LPTIM_CMP_CMP                          0x0000FFFFU             /*!< Compare register     */

/******************  Bit definition for LPTIM_ARR register *******************/
#define  LPTIM_ARR_ARR                          0x0000FFFFU             /*!< Auto reload register */

/******************  Bit definition for LPTIM_CNT register *******************/
#define  LPTIM_CNT_CNT                          0x0000FFFFU             /*!< Counter register     */
/******************************************************************************/
/*                                                                            */
/*      Universal Synchronous Asynchronous Receiver Transmitter (USART)       */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for USART_CR1 register  *******************/
#define  USART_CR1_UE                        0x00000001U            /*!< USART Enable                                    */
#define  USART_CR1_RE                        0x00000004U            /*!< Receiver Enable                                 */
#define  USART_CR1_TE                        0x00000008U            /*!< Transmitter Enable                              */
#define  USART_CR1_IDLEIE                    0x00000010U            /*!< IDLE Interrupt Enable                           */
#define  USART_CR1_RXNEIE                    0x00000020U            /*!< RXNE Interrupt Enable                           */
#define  USART_CR1_TCIE                      0x00000040U            /*!< Transmission Complete Interrupt Enable          */
#define  USART_CR1_TXEIE                     0x00000080U            /*!< TXE Interrupt Enable                            */
#define  USART_CR1_PEIE                      0x00000100U            /*!< PE Interrupt Enable                             */
#define  USART_CR1_PS                        0x00000200U            /*!< Parity Selection                                */
#define  USART_CR1_PCE                       0x00000400U            /*!< Parity Control Enable                           */
#define  USART_CR1_WAKE                      0x00000800U            /*!< Receiver Wakeup method                          */
#define  USART_CR1_M                         0x10001000U            /*!< Word length                                     */
#define  USART_CR1_M_0                       0x00001000U            /*!< Word length - Bit 0                             */
#define  USART_CR1_MME                       0x00002000U            /*!< Mute Mode Enable                                */
#define  USART_CR1_CMIE                      0x00004000U            /*!< Character match interrupt enable                */
#define  USART_CR1_OVER8                     0x00008000U            /*!< Oversampling by 8-bit or 16-bit mode            */
#define  USART_CR1_DEDT                      0x001F0000U            /*!< DEDT[4:0] bits (Driver Enable Deassertion Time) */
#define  USART_CR1_DEDT_0                    0x00010000U            /*!< Bit 0 */
#define  USART_CR1_DEDT_1                    0x00020000U            /*!< Bit 1 */
#define  USART_CR1_DEDT_2                    0x00040000U            /*!< Bit 2 */
#define  USART_CR1_DEDT_3                    0x00080000U            /*!< Bit 3 */
#define  USART_CR1_DEDT_4                    0x00100000U            /*!< Bit 4 */
#define  USART_CR1_DEAT                      0x03E00000U            /*!< DEAT[4:0] bits (Driver Enable Assertion Time)   */
#define  USART_CR1_DEAT_0                    0x00200000U            /*!< Bit 0 */
#define  USART_CR1_DEAT_1                    0x00400000U            /*!< Bit 1 */
#define  USART_CR1_DEAT_2                    0x00800000U            /*!< Bit 2 */
#define  USART_CR1_DEAT_3                    0x01000000U            /*!< Bit 3 */
#define  USART_CR1_DEAT_4                    0x02000000U            /*!< Bit 4 */
#define  USART_CR1_RTOIE                     0x04000000U            /*!< Receive Time Out interrupt enable */
#define  USART_CR1_EOBIE                     0x08000000U            /*!< End of Block interrupt enable     */
#define  USART_CR1_M_1                       0x10000000U            /*!< Word length - Bit 1               */

/******************  Bit definition for USART_CR2 register  *******************/
#define  USART_CR2_ADDM7                     0x00000010U            /*!< 7-bit or 4-bit Address Detection       */
#define  USART_CR2_LBDL                      0x00000020U            /*!< LIN Break Detection Length             */
#define  USART_CR2_LBDIE                     0x00000040U            /*!< LIN Break Detection Interrupt Enable   */
#define  USART_CR2_LBCL                      0x00000100U            /*!< Last Bit Clock pulse                   */
#define  USART_CR2_CPHA                      0x00000200U            /*!< Clock Phase                            */
#define  USART_CR2_CPOL                      0x00000400U            /*!< Clock Polarity                         */
#define  USART_CR2_CLKEN                     0x00000800U            /*!< Clock Enable                           */
#define  USART_CR2_STOP                      0x00003000U            /*!< STOP[1:0] bits (STOP bits)             */
#define  USART_CR2_STOP_0                    0x00001000U            /*!< Bit 0 */
#define  USART_CR2_STOP_1                    0x00002000U            /*!< Bit 1 */
#define  USART_CR2_LINEN                     0x00004000U            /*!< LIN mode enable                        */
#define  USART_CR2_SWAP                      0x00008000U            /*!< SWAP TX/RX pins                        */
#define  USART_CR2_RXINV                     0x00010000U            /*!< RX pin active level inversion          */
#define  USART_CR2_TXINV                     0x00020000U            /*!< TX pin active level inversion          */
#define  USART_CR2_DATAINV                   0x00040000U            /*!< Binary data inversion                  */
#define  USART_CR2_MSBFIRST                  0x00080000U            /*!< Most Significant Bit First             */
#define  USART_CR2_ABREN                     0x00100000U            /*!< Auto Baud-Rate Enable                  */
#define  USART_CR2_ABRMODE                   0x00600000U            /*!< ABRMOD[1:0] bits (Auto Baud-Rate Mode) */
#define  USART_CR2_ABRMODE_0                 0x00200000U            /*!< Bit 0 */
#define  USART_CR2_ABRMODE_1                 0x00400000U            /*!< Bit 1 */
#define  USART_CR2_RTOEN                     0x00800000U            /*!< Receiver Time-Out enable  */
#define  USART_CR2_ADD                       0xFF000000U            /*!< Address of the USART node */

/******************  Bit definition for USART_CR3 register  *******************/
#define  USART_CR3_EIE                       0x00000001U            /*!< Error Interrupt Enable                         */
#define  USART_CR3_IREN                      0x00000002U            /*!< IrDA mode Enable                               */
#define  USART_CR3_IRLP                      0x00000004U            /*!< IrDA Low-Power                                 */
#define  USART_CR3_HDSEL                     0x00000008U            /*!< Half-Duplex Selection                          */
#define  USART_CR3_NACK                      0x00000010U            /*!< SmartCard NACK enable                          */
#define  USART_CR3_SCEN                      0x00000020U            /*!< SmartCard mode enable                          */
#define  USART_CR3_DMAR                      0x00000040U            /*!< DMA Enable Receiver                            */
#define  USART_CR3_DMAT                      0x00000080U            /*!< DMA Enable Transmitter                         */
#define  USART_CR3_RTSE                      0x00000100U            /*!< RTS Enable                                     */
#define  USART_CR3_CTSE                      0x00000200U            /*!< CTS Enable                                     */
#define  USART_CR3_CTSIE                     0x00000400U            /*!< CTS Interrupt Enable                           */
#define  USART_CR3_ONEBIT                    0x00000800U            /*!< One sample bit method enable                   */
#define  USART_CR3_OVRDIS                    0x00001000U            /*!< Overrun Disable                                */
#define  USART_CR3_DDRE                      0x00002000U            /*!< DMA Disable on Reception Error                 */
#define  USART_CR3_DEM                       0x00004000U            /*!< Driver Enable Mode                             */
#define  USART_CR3_DEP                       0x00008000U            /*!< Driver Enable Polarity Selection               */
#define  USART_CR3_SCARCNT                   0x000E0000U            /*!< SCARCNT[2:0] bits (SmartCard Auto-Retry Count) */
#define  USART_CR3_SCARCNT_0                 0x00020000U            /*!< Bit 0 */
#define  USART_CR3_SCARCNT_1                 0x00040000U            /*!< Bit 1 */
#define  USART_CR3_SCARCNT_2                 0x00080000U            /*!< Bit 2 */


/******************  Bit definition for USART_BRR register  *******************/
#define  USART_BRR_DIV_FRACTION              0x000FU                /*!< Fraction of USARTDIV */
#define  USART_BRR_DIV_MANTISSA              0xFFF0U                /*!< Mantissa of USARTDIV */

/******************  Bit definition for USART_GTPR register  ******************/
#define  USART_GTPR_PSC                      0x00FFU                /*!< PSC[7:0] bits (Prescaler value) */
#define  USART_GTPR_GT                       0xFF00U                /*!< GT[7:0] bits (Guard time value) */


/*******************  Bit definition for USART_RTOR register  *****************/
#define  USART_RTOR_RTO                      0x00FFFFFFU            /*!< Receiver Time Out Value */
#define  USART_RTOR_BLEN                     0xFF000000U            /*!< Block Length */

/*******************  Bit definition for USART_RQR register  ******************/
#define  USART_RQR_ABRRQ                     0x0001U                /*!< Auto-Baud Rate Request      */
#define  USART_RQR_SBKRQ                     0x0002U                /*!< Send Break Request          */
#define  USART_RQR_MMRQ                      0x0004U                /*!< Mute Mode Request           */
#define  USART_RQR_RXFRQ                     0x0008U                /*!< Receive Data flush Request  */
#define  USART_RQR_TXFRQ                     0x0010U                /*!< Transmit data flush Request */

/*******************  Bit definition for USART_ISR register  ******************/
#define  USART_ISR_PE                        0x00000001U            /*!< Parity Error                        */
#define  USART_ISR_FE                        0x00000002U            /*!< Framing Error                       */
#define  USART_ISR_NE                        0x00000004U            /*!< Noise detected Flag                 */
#define  USART_ISR_ORE                       0x00000008U            /*!< OverRun Error                       */
#define  USART_ISR_IDLE                      0x00000010U            /*!< IDLE line detected                  */
#define  USART_ISR_RXNE                      0x00000020U            /*!< Read Data Register Not Empty        */
#define  USART_ISR_TC                        0x00000040U            /*!< Transmission Complete               */
#define  USART_ISR_TXE                       0x00000080U            /*!< Transmit Data Register Empty        */
#define  USART_ISR_LBDF                      0x00000100U            /*!< LIN Break Detection Flag            */
#define  USART_ISR_CTSIF                     0x00000200U            /*!< CTS interrupt flag                  */
#define  USART_ISR_CTS                       0x00000400U            /*!< CTS flag                            */
#define  USART_ISR_RTOF                      0x00000800U            /*!< Receiver Time Out                   */
#define  USART_ISR_EOBF                      0x00001000U            /*!< End Of Block Flag                   */
#define  USART_ISR_ABRE                      0x00004000U            /*!< Auto-Baud Rate Error                */
#define  USART_ISR_ABRF                      0x00008000U            /*!< Auto-Baud Rate Flag                 */
#define  USART_ISR_BUSY                      0x00010000U            /*!< Busy Flag                           */
#define  USART_ISR_CMF                       0x00020000U            /*!< Character Match Flag                */
#define  USART_ISR_SBKF                      0x00040000U            /*!< Send Break Flag                     */
#define  USART_ISR_RWU                       0x00080000U            /*!< Receive Wake Up from mute mode Flag */
#define  USART_ISR_WUF                       0x00100000U            /*!< Wake Up from stop mode Flag         */
#define  USART_ISR_TEACK                     0x00200000U            /*!< Transmit Enable Acknowledge Flag    */
#define  USART_ISR_REACK                     0x00400000U            /*!< Receive Enable Acknowledge Flag     */

/* Legacy define */
#define  USART_ISR_LBD                       USART_ISR_LBDF

/*******************  Bit definition for USART_ICR register  ******************/
#define  USART_ICR_PECF                      0x00000001U            /*!< Parity Error Clear Flag             */
#define  USART_ICR_FECF                      0x00000002U            /*!< Framing Error Clear Flag            */
#define  USART_ICR_NCF                       0x00000004U            /*!< Noise detected Clear Flag           */
#define  USART_ICR_ORECF                     0x00000008U            /*!< OverRun Error Clear Flag            */
#define  USART_ICR_IDLECF                    0x00000010U            /*!< IDLE line detected Clear Flag       */
#define  USART_ICR_TCCF                      0x00000040U            /*!< Transmission Complete Clear Flag    */
#define  USART_ICR_LBDCF                     0x00000100U            /*!< LIN Break Detection Clear Flag      */
#define  USART_ICR_CTSCF                     0x00000200U            /*!< CTS Interrupt Clear Flag            */
#define  USART_ICR_RTOCF                     0x00000800U            /*!< Receiver Time Out Clear Flag        */
#define  USART_ICR_EOBCF                     0x00001000U            /*!< End Of Block Clear Flag             */
#define  USART_ICR_CMCF                      0x00020000U            /*!< Character Match Clear Flag          */
#define  USART_ICR_WUCF                      0x00100000U            /*!< Wake Up from stop mode Clear Flag   */

/*******************  Bit definition for USART_RDR register  ******************/
#define  USART_RDR_RDR                       0x01FFU                /*!< RDR[8:0] bits (Receive Data value) */

/*******************  Bit definition for USART_TDR register  ******************/
#define  USART_TDR_TDR                       0x01FFU                /*!< TDR[8:0] bits (Transmit Data value) */

/******************************************************************************/
/*                                                                            */
/*                            Window WATCHDOG                                 */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for WWDG_CR register  ********************/
#define  WWDG_CR_T                            0x7FU                 /*!<T[6:0] bits (7-Bit counter (MSB to LSB)) */
#define  WWDG_CR_T_0                          0x01U                 /*!<Bit 0 */
#define  WWDG_CR_T_1                          0x02U                 /*!<Bit 1 */
#define  WWDG_CR_T_2                          0x04U                 /*!<Bit 2 */
#define  WWDG_CR_T_3                          0x08U                 /*!<Bit 3 */
#define  WWDG_CR_T_4                          0x10U                 /*!<Bit 4 */
#define  WWDG_CR_T_5                          0x20U                 /*!<Bit 5 */
#define  WWDG_CR_T_6                          0x40U                 /*!<Bit 6 */

/* Legacy defines */
#define  WWDG_CR_T0                           WWDG_CR_T_0                      /*!<Bit 0 */
#define  WWDG_CR_T1                           WWDG_CR_T_1                      /*!<Bit 1 */
#define  WWDG_CR_T2                           WWDG_CR_T_2                      /*!<Bit 2 */
#define  WWDG_CR_T3                           WWDG_CR_T_3                      /*!<Bit 3 */
#define  WWDG_CR_T4                           WWDG_CR_T_4                      /*!<Bit 4 */
#define  WWDG_CR_T5                           WWDG_CR_T_5                      /*!<Bit 5 */
#define  WWDG_CR_T6                           WWDG_CR_T_6                      /*!<Bit 6 */

#define  WWDG_CR_WDGA                        0x80U                  /*!<Activation bit */

/*******************  Bit definition for WWDG_CFR register  *******************/
#define  WWDG_CFR_W                          0x007FU                /*!<W[6:0] bits (7-bit window value) */
#define  WWDG_CFR_W_0                        0x0001U                /*!<Bit 0 */
#define  WWDG_CFR_W_1                        0x0002U                /*!<Bit 1 */
#define  WWDG_CFR_W_2                        0x0004U                /*!<Bit 2 */
#define  WWDG_CFR_W_3                        0x0008U                /*!<Bit 3 */
#define  WWDG_CFR_W_4                        0x0010U                /*!<Bit 4 */
#define  WWDG_CFR_W_5                        0x0020U                /*!<Bit 5 */
#define  WWDG_CFR_W_6                        0x0040U                /*!<Bit 6 */

/* Legacy defines */
#define  WWDG_CFR_W0                         WWDG_CFR_W_0                      /*!<Bit 0 */
#define  WWDG_CFR_W1                         WWDG_CFR_W_1                      /*!<Bit 1 */
#define  WWDG_CFR_W2                         WWDG_CFR_W_2                      /*!<Bit 2 */
#define  WWDG_CFR_W3                         WWDG_CFR_W_3                      /*!<Bit 3 */
#define  WWDG_CFR_W4                         WWDG_CFR_W_4                      /*!<Bit 4 */
#define  WWDG_CFR_W5                         WWDG_CFR_W_5                      /*!<Bit 5 */
#define  WWDG_CFR_W6                         WWDG_CFR_W_6                      /*!<Bit 6 */

#define  WWDG_CFR_WDGTB                      0x0180U                /*!<WDGTB[1:0] bits (Timer Base) */
#define  WWDG_CFR_WDGTB_0                    0x0080U                /*!<Bit 0 */
#define  WWDG_CFR_WDGTB_1                    0x0100U                /*!<Bit 1 */

/* Legacy defines */
#define  WWDG_CFR_WDGTB0                     WWDG_CFR_WDGTB_0                  /*!<Bit 0 */
#define  WWDG_CFR_WDGTB1                     WWDG_CFR_WDGTB_1                  /*!<Bit 1 */

#define  WWDG_CFR_EWI                        0x0200U               /*!<Early Wakeup Interrupt */

/*******************  Bit definition for WWDG_SR register  ********************/
#define  WWDG_SR_EWIF                        0x01U                  /*!<Early Wakeup Interrupt Flag */

/******************************************************************************/
/*                                                                            */
/*                                DBG                                         */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for DBGMCU_IDCODE register  *************/
#define  DBGMCU_IDCODE_DEV_ID                0x00000FFFU
#define  DBGMCU_IDCODE_REV_ID                0xFFFF0000U

/********************  Bit definition for DBGMCU_CR register  *****************/
#define  DBGMCU_CR_DBG_SLEEP                 0x00000001U
#define  DBGMCU_CR_DBG_STOP                  0x00000002U
#define  DBGMCU_CR_DBG_STANDBY               0x00000004U
#define  DBGMCU_CR_TRACE_IOEN                0x00000020U

#define  DBGMCU_CR_TRACE_MODE                0x000000C0U
#define  DBGMCU_CR_TRACE_MODE_0              0x00000040U /*!<Bit 0 */
#define  DBGMCU_CR_TRACE_MODE_1              0x00000080U /*!<Bit 1 */

/********************  Bit definition for DBGMCU_APB1_FZ register  ************/
#define  DBGMCU_APB1_FZ_DBG_TIM2_STOP            0x00000001U
#define  DBGMCU_APB1_FZ_DBG_TIM3_STOP            0x00000002U
#define  DBGMCU_APB1_FZ_DBG_TIM4_STOP            0x00000004U
#define  DBGMCU_APB1_FZ_DBG_TIM5_STOP            0x00000008U
#define  DBGMCU_APB1_FZ_DBG_TIM6_STOP            0x00000010U
#define  DBGMCU_APB1_FZ_DBG_TIM7_STOP            0x00000020U
#define  DBGMCU_APB1_FZ_DBG_TIM12_STOP           0x00000040U
#define  DBGMCU_APB1_FZ_DBG_TIM13_STOP           0x00000080U
#define  DBGMCU_APB1_FZ_DBG_TIM14_STOP           0x00000100U
#define  DBGMCU_APB1_FZ_DBG_RTC_STOP             0x00000400U
#define  DBGMCU_APB1_FZ_DBG_WWDG_STOP            0x00000800U
#define  DBGMCU_APB1_FZ_DBG_IWDG_STOP            0x00001000U
#define  DBGMCU_APB1_FZ_DBG_I2C1_SMBUS_TIMEOUT   0x00200000U
#define  DBGMCU_APB1_FZ_DBG_I2C2_SMBUS_TIMEOUT   0x00400000U
#define  DBGMCU_APB1_FZ_DBG_I2C3_SMBUS_TIMEOUT   0x00800000U
#define  DBGMCU_APB1_FZ_DBG_CAN1_STOP            0x02000000U
#define  DBGMCU_APB1_FZ_DBG_CAN2_STOP            0x04000000U

/********************  Bit definition for DBGMCU_APB2_FZ register  ************/
#define  DBGMCU_APB2_FZ_DBG_TIM1_STOP            0x00000001U
#define  DBGMCU_APB2_FZ_DBG_TIM8_STOP            0x00000002U
#define  DBGMCU_APB2_FZ_DBG_TIM9_STOP            0x00010000U
#define  DBGMCU_APB2_FZ_DBG_TIM10_STOP           0x00020000U
#define  DBGMCU_APB2_FZ_DBG_TIM11_STOP           0x00040000U

/******************************************************************************/
/*                                                                            */
/*                Ethernet MAC Registers bits definitions                     */
/*                                                                            */
/******************************************************************************/
/* Bit definition for Ethernet MAC Control Register register */
#define ETH_MACCR_WD              0x00800000U  /* Watchdog disable */
#define ETH_MACCR_JD              0x00400000U  /* Jabber disable */
#define ETH_MACCR_IFG             0x000E0000U  /* Inter-frame gap */
#define ETH_MACCR_IFG_96Bit       0x00000000U  /* Minimum IFG between frames during transmission is 96Bit */
#define ETH_MACCR_IFG_88Bit       0x00020000U  /* Minimum IFG between frames during transmission is 88Bit */
#define ETH_MACCR_IFG_80Bit       0x00040000U  /* Minimum IFG between frames during transmission is 80Bit */
#define ETH_MACCR_IFG_72Bit       0x00060000U  /* Minimum IFG between frames during transmission is 72Bit */
#define ETH_MACCR_IFG_64Bit       0x00080000U  /* Minimum IFG between frames during transmission is 64Bit */
#define ETH_MACCR_IFG_56Bit       0x000A0000U  /* Minimum IFG between frames during transmission is 56Bit */
#define ETH_MACCR_IFG_48Bit       0x000C0000U  /* Minimum IFG between frames during transmission is 48Bit */
#define ETH_MACCR_IFG_40Bit       0x000E0000U  /* Minimum IFG between frames during transmission is 40Bit */
#define ETH_MACCR_CSD             0x00010000U  /* Carrier sense disable (during transmission) */
#define ETH_MACCR_FES             0x00004000U  /* Fast ethernet speed */
#define ETH_MACCR_ROD             0x00002000U  /* Receive own disable */
#define ETH_MACCR_LM              0x00001000U  /* loopback mode */
#define ETH_MACCR_DM              0x00000800U  /* Duplex mode */
#define ETH_MACCR_IPCO            0x00000400U  /* IP Checksum offload */
#define ETH_MACCR_RD              0x00000200U  /* Retry disable */
#define ETH_MACCR_APCS            0x00000080U  /* Automatic Pad/CRC stripping */
#define ETH_MACCR_BL              0x00000060U  /* Back-off limit: random integer number (r) of slot time delays before rescheduling
                                                       a transmission attempt during retries after a collision: 0 =< r <2^k */
#define ETH_MACCR_BL_10           0x00000000U  /* k = min (n, 10) */
#define ETH_MACCR_BL_8            0x00000020U  /* k = min (n, 8) */
#define ETH_MACCR_BL_4            0x00000040U  /* k = min (n, 4) */
#define ETH_MACCR_BL_1            0x00000060U  /* k = min (n, 1) */
#define ETH_MACCR_DC              0x00000010U  /* Defferal check */
#define ETH_MACCR_TE              0x00000008U  /* Transmitter enable */
#define ETH_MACCR_RE              0x00000004U  /* Receiver enable */

/* Bit definition for Ethernet MAC Frame Filter Register */
#define ETH_MACFFR_RA             0x80000000U  /* Receive all */
#define ETH_MACFFR_HPF            0x00000400U  /* Hash or perfect filter */
#define ETH_MACFFR_SAF            0x00000200U  /* Source address filter enable */
#define ETH_MACFFR_SAIF           0x00000100U  /* SA inverse filtering */
#define ETH_MACFFR_PCF            0x000000C0U  /* Pass control frames: 3 cases */
#define ETH_MACFFR_PCF_BlockAll                0x00000040U  /* MAC filters all control frames from reaching the application */
#define ETH_MACFFR_PCF_ForwardAll              0x00000080U  /* MAC forwards all control frames to application even if they fail the Address Filter */
#define ETH_MACFFR_PCF_ForwardPassedAddrFilter 0x000000C0U  /* MAC forwards control frames that pass the Address Filter. */
#define ETH_MACFFR_BFD             0x00000020U  /* Broadcast frame disable */
#define ETH_MACFFR_PAM             0x00000010U  /* Pass all mutlicast */
#define ETH_MACFFR_DAIF            0x00000008U  /* DA Inverse filtering */
#define ETH_MACFFR_HM              0x00000004U  /* Hash multicast */
#define ETH_MACFFR_HU              0x00000002U  /* Hash unicast */
#define ETH_MACFFR_PM              0x00000001U  /* Promiscuous mode */

/* Bit definition for Ethernet MAC Hash Table High Register */
#define ETH_MACHTHR_HTH            0xFFFFFFFFU  /* Hash table high */

/* Bit definition for Ethernet MAC Hash Table Low Register */
#define ETH_MACHTLR_HTL            0xFFFFFFFFU  /* Hash table low */

/* Bit definition for Ethernet MAC MII Address Register */
#define ETH_MACMIIAR_PA            0x0000F800U  /* Physical layer address */
#define ETH_MACMIIAR_MR            0x000007C0U  /* MII register in the selected PHY */
#define ETH_MACMIIAR_CR            0x0000001CU  /* CR clock range: 6 cases */
#define ETH_MACMIIAR_CR_Div42      0x00000000U  /* HCLK:60-100 MHz; MDC clock= HCLK/42 */
#define ETH_MACMIIAR_CR_Div62      0x00000004U  /* HCLK:100-150 MHz; MDC clock= HCLK/62 */
#define ETH_MACMIIAR_CR_Div16      0x00000008U  /* HCLK:20-35 MHz; MDC clock= HCLK/16 */
#define ETH_MACMIIAR_CR_Div26      0x0000000CU  /* HCLK:35-60 MHz; MDC clock= HCLK/26 */
#define ETH_MACMIIAR_CR_Div102     0x00000010U  /* HCLK:150-168 MHz; MDC clock= HCLK/102 */
#define ETH_MACMIIAR_MW            0x00000002U  /* MII write */
#define ETH_MACMIIAR_MB            0x00000001U  /* MII busy */

/* Bit definition for Ethernet MAC MII Data Register */
#define ETH_MACMIIDR_MD            0x0000FFFFU  /* MII data: read/write data from/to PHY */

/* Bit definition for Ethernet MAC Flow Control Register */
#define ETH_MACFCR_PT              0xFFFF0000U  /* Pause time */
#define ETH_MACFCR_ZQPD            0x00000080U  /* Zero-quanta pause disable */
#define ETH_MACFCR_PLT             0x00000030U  /* Pause low threshold: 4 cases */
#define ETH_MACFCR_PLT_Minus4      0x00000000U  /* Pause time minus 4 slot times */
#define ETH_MACFCR_PLT_Minus28     0x00000010U  /* Pause time minus 28 slot times */
#define ETH_MACFCR_PLT_Minus144    0x00000020U  /* Pause time minus 144 slot times */
#define ETH_MACFCR_PLT_Minus256    0x00000030U  /* Pause time minus 256 slot times */
#define ETH_MACFCR_UPFD            0x00000008U  /* Unicast pause frame detect */
#define ETH_MACFCR_RFCE            0x00000004U  /* Receive flow control enable */
#define ETH_MACFCR_TFCE            0x00000002U  /* Transmit flow control enable */
#define ETH_MACFCR_FCBBPA          0x00000001U  /* Flow control busy/backpressure activate */

/* Bit definition for Ethernet MAC VLAN Tag Register */
#define ETH_MACVLANTR_VLANTC       0x00010000U  /* 12-bit VLAN tag comparison */
#define ETH_MACVLANTR_VLANTI       0x0000FFFFU  /* VLAN tag identifier (for receive frames) */

/* Bit definition for Ethernet MAC Remote Wake-UpFrame Filter Register */
#define ETH_MACRWUFFR_D            0xFFFFFFFFU  /* Wake-up frame filter register data */
/* Eight sequential Writes to this address (offset 0x28) will write all Wake-UpFrame Filter Registers.
   Eight sequential Reads from this address (offset 0x28) will read all Wake-UpFrame Filter Registers. */
/* Wake-UpFrame Filter Reg0 : Filter 0 Byte Mask
   Wake-UpFrame Filter Reg1 : Filter 1 Byte Mask
   Wake-UpFrame Filter Reg2 : Filter 2 Byte Mask
   Wake-UpFrame Filter Reg3 : Filter 3 Byte Mask
   Wake-UpFrame Filter Reg4 : RSVD - Filter3 Command - RSVD - Filter2 Command -
                              RSVD - Filter1 Command - RSVD - Filter0 Command
   Wake-UpFrame Filter Re5 : Filter3 Offset - Filter2 Offset - Filter1 Offset - Filter0 Offset
   Wake-UpFrame Filter Re6 : Filter1 CRC16 - Filter0 CRC16
   Wake-UpFrame Filter Re7 : Filter3 CRC16 - Filter2 CRC16 */

/* Bit definition for Ethernet MAC PMT Control and Status Register */
#define ETH_MACPMTCSR_WFFRPR       0x80000000U  /* Wake-Up Frame Filter Register Pointer Reset */
#define ETH_MACPMTCSR_GU           0x00000200U  /* Global Unicast */
#define ETH_MACPMTCSR_WFR          0x00000040U  /* Wake-Up Frame Received */
#define ETH_MACPMTCSR_MPR          0x00000020U  /* Magic Packet Received */
#define ETH_MACPMTCSR_WFE          0x00000004U  /* Wake-Up Frame Enable */
#define ETH_MACPMTCSR_MPE          0x00000002U  /* Magic Packet Enable */
#define ETH_MACPMTCSR_PD           0x00000001U  /* Power Down */

/* Bit definition for Ethernet MAC Status Register */
#define ETH_MACSR_TSTS      0x00000200U  /* Time stamp trigger status */
#define ETH_MACSR_MMCTS     0x00000040U  /* MMC transmit status */
#define ETH_MACSR_MMMCRS    0x00000020U  /* MMC receive status */
#define ETH_MACSR_MMCS      0x00000010U  /* MMC status */
#define ETH_MACSR_PMTS      0x00000008U  /* PMT status */

/* Bit definition for Ethernet MAC Interrupt Mask Register */
#define ETH_MACIMR_TSTIM     0x00000200U  /* Time stamp trigger interrupt mask */
#define ETH_MACIMR_PMTIM     0x00000008U  /* PMT interrupt mask */

/* Bit definition for Ethernet MAC Address0 High Register */
#define ETH_MACA0HR_MACA0H   0x0000FFFFU  /* MAC address0 high */

/* Bit definition for Ethernet MAC Address0 Low Register */
#define ETH_MACA0LR_MACA0L   0xFFFFFFFFU  /* MAC address0 low */

/* Bit definition for Ethernet MAC Address1 High Register */
#define ETH_MACA1HR_AE       0x80000000U  /* Address enable */
#define ETH_MACA1HR_SA       0x40000000U  /* Source address */
#define ETH_MACA1HR_MBC      0x3F000000U  /* Mask byte control: bits to mask for comparison of the MAC Address bytes */
  #define ETH_MACA1HR_MBC_HBits15_8    0x20000000U  /* Mask MAC Address high reg bits [15:8] */
  #define ETH_MACA1HR_MBC_HBits7_0     0x10000000U  /* Mask MAC Address high reg bits [7:0] */
  #define ETH_MACA1HR_MBC_LBits31_24   0x08000000U  /* Mask MAC Address low reg bits [31:24] */
  #define ETH_MACA1HR_MBC_LBits23_16   0x04000000U  /* Mask MAC Address low reg bits [23:16] */
  #define ETH_MACA1HR_MBC_LBits15_8    0x02000000U  /* Mask MAC Address low reg bits [15:8] */
  #define ETH_MACA1HR_MBC_LBits7_0     0x01000000U  /* Mask MAC Address low reg bits [7:0] */
#define ETH_MACA1HR_MACA1H            0x0000FFFFU  /* MAC address1 high */

/* Bit definition for Ethernet MAC Address1 Low Register */
#define ETH_MACA1LR_MACA1L   0xFFFFFFFFU  /* MAC address1 low */

/* Bit definition for Ethernet MAC Address2 High Register */
#define ETH_MACA2HR_AE       0x80000000U  /* Address enable */
#define ETH_MACA2HR_SA       0x40000000U  /* Source address */
#define ETH_MACA2HR_MBC      0x3F000000U  /* Mask byte control */
  #define ETH_MACA2HR_MBC_HBits15_8    0x20000000U  /* Mask MAC Address high reg bits [15:8] */
  #define ETH_MACA2HR_MBC_HBits7_0     0x10000000U  /* Mask MAC Address high reg bits [7:0] */
  #define ETH_MACA2HR_MBC_LBits31_24   0x08000000U  /* Mask MAC Address low reg bits [31:24] */
  #define ETH_MACA2HR_MBC_LBits23_16   0x04000000U  /* Mask MAC Address low reg bits [23:16] */
  #define ETH_MACA2HR_MBC_LBits15_8    0x02000000U  /* Mask MAC Address low reg bits [15:8] */
  #define ETH_MACA2HR_MBC_LBits7_0     0x01000000U  /* Mask MAC Address low reg bits [70] */
#define ETH_MACA2HR_MACA2H   0x0000FFFFU  /* MAC address1 high */

/* Bit definition for Ethernet MAC Address2 Low Register */
#define ETH_MACA2LR_MACA2L   0xFFFFFFFFU  /* MAC address2 low */

/* Bit definition for Ethernet MAC Address3 High Register */
#define ETH_MACA3HR_AE       0x80000000U  /* Address enable */
#define ETH_MACA3HR_SA       0x40000000U  /* Source address */
#define ETH_MACA3HR_MBC      0x3F000000U  /* Mask byte control */
  #define ETH_MACA3HR_MBC_HBits15_8    0x20000000U  /* Mask MAC Address high reg bits [15:8] */
  #define ETH_MACA3HR_MBC_HBits7_0     0x10000000U  /* Mask MAC Address high reg bits [7:0] */
  #define ETH_MACA3HR_MBC_LBits31_24   0x08000000U  /* Mask MAC Address low reg bits [31:24] */
  #define ETH_MACA3HR_MBC_LBits23_16   0x04000000U  /* Mask MAC Address low reg bits [23:16] */
  #define ETH_MACA3HR_MBC_LBits15_8    0x02000000U  /* Mask MAC Address low reg bits [15:8] */
  #define ETH_MACA3HR_MBC_LBits7_0     0x01000000U  /* Mask MAC Address low reg bits [70] */
#define ETH_MACA3HR_MACA3H   0x0000FFFFU  /* MAC address3 high */

/* Bit definition for Ethernet MAC Address3 Low Register */
#define ETH_MACA3LR_MACA3L   0xFFFFFFFFU  /* MAC address3 low */

/******************************************************************************/
/*                Ethernet MMC Registers bits definition                      */
/******************************************************************************/

/* Bit definition for Ethernet MMC Contol Register */
#define ETH_MMCCR_MCFHP      0x00000020U  /* MMC counter Full-Half preset */
#define ETH_MMCCR_MCP        0x00000010U  /* MMC counter preset */
#define ETH_MMCCR_MCF        0x00000008U  /* MMC Counter Freeze */
#define ETH_MMCCR_ROR        0x00000004U  /* Reset on Read */
#define ETH_MMCCR_CSR        0x00000002U  /* Counter Stop Rollover */
#define ETH_MMCCR_CR         0x00000001U  /* Counters Reset */

/* Bit definition for Ethernet MMC Receive Interrupt Register */
#define ETH_MMCRIR_RGUFS     0x00020000U  /* Set when Rx good unicast frames counter reaches half the maximum value */
#define ETH_MMCRIR_RFAES     0x00000040U  /* Set when Rx alignment error counter reaches half the maximum value */
#define ETH_MMCRIR_RFCES     0x00000020U  /* Set when Rx crc error counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Transmit Interrupt Register */
#define ETH_MMCTIR_TGFS      0x00200000U  /* Set when Tx good frame count counter reaches half the maximum value */
#define ETH_MMCTIR_TGFMSCS   0x00008000U  /* Set when Tx good multi col counter reaches half the maximum value */
#define ETH_MMCTIR_TGFSCS    0x00004000U  /* Set when Tx good single col counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Receive Interrupt Mask Register */
#define ETH_MMCRIMR_RGUFM    0x00020000U  /* Mask the interrupt when Rx good unicast frames counter reaches half the maximum value */
#define ETH_MMCRIMR_RFAEM    0x00000040U  /* Mask the interrupt when when Rx alignment error counter reaches half the maximum value */
#define ETH_MMCRIMR_RFCEM    0x00000020U  /* Mask the interrupt when Rx crc error counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Transmit Interrupt Mask Register */
#define ETH_MMCTIMR_TGFM     0x00200000U  /* Mask the interrupt when Tx good frame count counter reaches half the maximum value */
#define ETH_MMCTIMR_TGFMSCM  0x00008000U  /* Mask the interrupt when Tx good multi col counter reaches half the maximum value */
#define ETH_MMCTIMR_TGFSCM   0x00004000U  /* Mask the interrupt when Tx good single col counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Transmitted Good Frames after Single Collision Counter Register */
#define ETH_MMCTGFSCCR_TGFSCC     0xFFFFFFFFU  /* Number of successfully transmitted frames after a single collision in Half-duplex mode. */

/* Bit definition for Ethernet MMC Transmitted Good Frames after More than a Single Collision Counter Register */
#define ETH_MMCTGFMSCCR_TGFMSCC   0xFFFFFFFFU  /* Number of successfully transmitted frames after more than a single collision in Half-duplex mode. */

/* Bit definition for Ethernet MMC Transmitted Good Frames Counter Register */
#define ETH_MMCTGFCR_TGFC    0xFFFFFFFFU  /* Number of good frames transmitted. */

/* Bit definition for Ethernet MMC Received Frames with CRC Error Counter Register */
#define ETH_MMCRFCECR_RFCEC  0xFFFFFFFFU  /* Number of frames received with CRC error. */

/* Bit definition for Ethernet MMC Received Frames with Alignement Error Counter Register */
#define ETH_MMCRFAECR_RFAEC  0xFFFFFFFFU  /* Number of frames received with alignment (dribble) error */

/* Bit definition for Ethernet MMC Received Good Unicast Frames Counter Register */
#define ETH_MMCRGUFCR_RGUFC  0xFFFFFFFFU  /* Number of good unicast frames received. */

/******************************************************************************/
/*               Ethernet PTP Registers bits definition                       */
/******************************************************************************/

/* Bit definition for Ethernet PTP Time Stamp Contol Register */
#define ETH_PTPTSCR_TSCNT       0x00030000U  /* Time stamp clock node type */
#define ETH_PTPTSSR_TSSMRME     0x00008000U  /* Time stamp snapshot for message relevant to master enable */
#define ETH_PTPTSSR_TSSEME      0x00004000U  /* Time stamp snapshot for event message enable */
#define ETH_PTPTSSR_TSSIPV4FE   0x00002000U  /* Time stamp snapshot for IPv4 frames enable */
#define ETH_PTPTSSR_TSSIPV6FE   0x00001000U  /* Time stamp snapshot for IPv6 frames enable */
#define ETH_PTPTSSR_TSSPTPOEFE  0x00000800U  /* Time stamp snapshot for PTP over ethernet frames enable */
#define ETH_PTPTSSR_TSPTPPSV2E  0x00000400U  /* Time stamp PTP packet snooping for version2 format enable */
#define ETH_PTPTSSR_TSSSR       0x00000200U  /* Time stamp Sub-seconds rollover */
#define ETH_PTPTSSR_TSSARFE     0x00000100U  /* Time stamp snapshot for all received frames enable */

#define ETH_PTPTSCR_TSARU       0x00000020U  /* Addend register update */
#define ETH_PTPTSCR_TSITE       0x00000010U  /* Time stamp interrupt trigger enable */
#define ETH_PTPTSCR_TSSTU       0x00000008U  /* Time stamp update */
#define ETH_PTPTSCR_TSSTI       0x00000004U  /* Time stamp initialize */
#define ETH_PTPTSCR_TSFCU       0x00000002U  /* Time stamp fine or coarse update */
#define ETH_PTPTSCR_TSE         0x00000001U  /* Time stamp enable */

/* Bit definition for Ethernet PTP Sub-Second Increment Register */
#define ETH_PTPSSIR_STSSI      0x000000FFU   /* System time Sub-second increment value */

/* Bit definition for Ethernet PTP Time Stamp High Register */
#define ETH_PTPTSHR_STS        0xFFFFFFFFU   /* System Time second */

/* Bit definition for Ethernet PTP Time Stamp Low Register */
#define ETH_PTPTSLR_STPNS      0x80000000U  /* System Time Positive or negative time */
#define ETH_PTPTSLR_STSS       0x7FFFFFFFU  /* System Time sub-seconds */

/* Bit definition for Ethernet PTP Time Stamp High Update Register */
#define ETH_PTPTSHUR_TSUS      0xFFFFFFFFU  /* Time stamp update seconds */

/* Bit definition for Ethernet PTP Time Stamp Low Update Register */
#define ETH_PTPTSLUR_TSUPNS    0x80000000U  /* Time stamp update Positive or negative time */
#define ETH_PTPTSLUR_TSUSS     0x7FFFFFFFU  /* Time stamp update sub-seconds */

/* Bit definition for Ethernet PTP Time Stamp Addend Register */
#define ETH_PTPTSAR_TSA        0xFFFFFFFFU  /* Time stamp addend */

/* Bit definition for Ethernet PTP Target Time High Register */
#define ETH_PTPTTHR_TTSH       0xFFFFFFFFU  /* Target time stamp high */

/* Bit definition for Ethernet PTP Target Time Low Register */
#define ETH_PTPTTLR_TTSL       0xFFFFFFFFU  /* Target time stamp low */

/* Bit definition for Ethernet PTP Time Stamp Status Register */
#define ETH_PTPTSSR_TSTTR      0x00000020U  /* Time stamp target time reached */
#define ETH_PTPTSSR_TSSO       0x00000010U  /* Time stamp seconds overflow */

/******************************************************************************/
/*                 Ethernet DMA Registers bits definition                     */
/******************************************************************************/

/* Bit definition for Ethernet DMA Bus Mode Register */
#define ETH_DMABMR_AAB        0x02000000U  /* Address-Aligned beats */
#define ETH_DMABMR_FPM        0x01000000U  /* 4xPBL mode */
#define ETH_DMABMR_USP        0x00800000U  /* Use separate PBL */
#define ETH_DMABMR_RDP        0x007E0000U  /* RxDMA PBL */
  #define ETH_DMABMR_RDP_1Beat    0x00020000U  /* maximum number of beats to be transferred in one RxDMA transaction is 1 */
  #define ETH_DMABMR_RDP_2Beat    0x00040000U  /* maximum number of beats to be transferred in one RxDMA transaction is 2 */
  #define ETH_DMABMR_RDP_4Beat    0x00080000U  /* maximum number of beats to be transferred in one RxDMA transaction is 4 */
  #define ETH_DMABMR_RDP_8Beat    0x00100000U  /* maximum number of beats to be transferred in one RxDMA transaction is 8 */
  #define ETH_DMABMR_RDP_16Beat   0x00200000U  /* maximum number of beats to be transferred in one RxDMA transaction is 16 */
  #define ETH_DMABMR_RDP_32Beat   0x00400000U  /* maximum number of beats to be transferred in one RxDMA transaction is 32 */
  #define ETH_DMABMR_RDP_4xPBL_4Beat   0x01020000U  /* maximum number of beats to be transferred in one RxDMA transaction is 4 */
  #define ETH_DMABMR_RDP_4xPBL_8Beat   0x01040000U  /* maximum number of beats to be transferred in one RxDMA transaction is 8 */
  #define ETH_DMABMR_RDP_4xPBL_16Beat  0x01080000U  /* maximum number of beats to be transferred in one RxDMA transaction is 16 */
  #define ETH_DMABMR_RDP_4xPBL_32Beat  0x01100000U  /* maximum number of beats to be transferred in one RxDMA transaction is 32 */
  #define ETH_DMABMR_RDP_4xPBL_64Beat  0x01200000U  /* maximum number of beats to be transferred in one RxDMA transaction is 64 */
  #define ETH_DMABMR_RDP_4xPBL_128Beat 0x01400000U  /* maximum number of beats to be transferred in one RxDMA transaction is 128 */
#define ETH_DMABMR_FB        0x00010000U  /* Fixed Burst */
#define ETH_DMABMR_RTPR      0x0000C000U  /* Rx Tx priority ratio */
  #define ETH_DMABMR_RTPR_1_1     0x00000000U  /* Rx Tx priority ratio */
  #define ETH_DMABMR_RTPR_2_1     0x00004000U  /* Rx Tx priority ratio */
  #define ETH_DMABMR_RTPR_3_1     0x00008000U  /* Rx Tx priority ratio */
  #define ETH_DMABMR_RTPR_4_1     0x0000C000U  /* Rx Tx priority ratio */
#define ETH_DMABMR_PBL    0x00003F00U  /* Programmable burst length */
  #define ETH_DMABMR_PBL_1Beat    0x00000100U  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 1 */
  #define ETH_DMABMR_PBL_2Beat    0x00000200U  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 2 */
  #define ETH_DMABMR_PBL_4Beat    0x00000400U  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
  #define ETH_DMABMR_PBL_8Beat    0x00000800U  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
  #define ETH_DMABMR_PBL_16Beat   0x00001000U  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
  #define ETH_DMABMR_PBL_32Beat   0x00002000U  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
  #define ETH_DMABMR_PBL_4xPBL_4Beat   0x01000100U  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
  #define ETH_DMABMR_PBL_4xPBL_8Beat   0x01000200U  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
  #define ETH_DMABMR_PBL_4xPBL_16Beat  0x01000400U  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
  #define ETH_DMABMR_PBL_4xPBL_32Beat  0x01000800U  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
  #define ETH_DMABMR_PBL_4xPBL_64Beat  0x01001000U  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 64 */
  #define ETH_DMABMR_PBL_4xPBL_128Beat 0x01002000U  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 128 */
#define ETH_DMABMR_EDE       0x00000080U  /* Enhanced Descriptor Enable */
#define ETH_DMABMR_DSL       0x0000007CU  /* Descriptor Skip Length */
#define ETH_DMABMR_DA        0x00000002U  /* DMA arbitration scheme */
#define ETH_DMABMR_SR        0x00000001U  /* Software reset */

/* Bit definition for Ethernet DMA Transmit Poll Demand Register */
#define ETH_DMATPDR_TPD      0xFFFFFFFFU  /* Transmit poll demand */

/* Bit definition for Ethernet DMA Receive Poll Demand Register */
#define ETH_DMARPDR_RPD      0xFFFFFFFFU  /* Receive poll demand  */

/* Bit definition for Ethernet DMA Receive Descriptor List Address Register */
#define ETH_DMARDLAR_SRL     0xFFFFFFFFU  /* Start of receive list */

/* Bit definition for Ethernet DMA Transmit Descriptor List Address Register */
#define ETH_DMATDLAR_STL     0xFFFFFFFFU  /* Start of transmit list */

/* Bit definition for Ethernet DMA Status Register */
#define ETH_DMASR_TSTS       0x20000000U  /* Time-stamp trigger status */
#define ETH_DMASR_PMTS       0x10000000U  /* PMT status */
#define ETH_DMASR_MMCS       0x08000000U  /* MMC status */
#define ETH_DMASR_EBS        0x03800000U  /* Error bits status */
  /* combination with EBS[2:0] for GetFlagStatus function */
  #define ETH_DMASR_EBS_DescAccess      0x02000000U  /* Error bits 0-data buffer, 1-desc. access */
  #define ETH_DMASR_EBS_ReadTransf      0x01000000U  /* Error bits 0-write trnsf, 1-read transfr */
  #define ETH_DMASR_EBS_DataTransfTx    0x00800000U  /* Error bits 0-Rx DMA, 1-Tx DMA */
#define ETH_DMASR_TPS         0x00700000U  /* Transmit process state */
  #define ETH_DMASR_TPS_Stopped         0x00000000U  /* Stopped - Reset or Stop Tx Command issued  */
  #define ETH_DMASR_TPS_Fetching        0x00100000U  /* Running - fetching the Tx descriptor */
  #define ETH_DMASR_TPS_Waiting         0x00200000U  /* Running - waiting for status */
  #define ETH_DMASR_TPS_Reading         0x00300000U  /* Running - reading the data from host memory */
  #define ETH_DMASR_TPS_Suspended       0x00600000U  /* Suspended - Tx Descriptor unavailabe */
  #define ETH_DMASR_TPS_Closing         0x00700000U  /* Running - closing Rx descriptor */
#define ETH_DMASR_RPS         0x000E0000U  /* Receive process state */
  #define ETH_DMASR_RPS_Stopped         0x00000000U  /* Stopped - Reset or Stop Rx Command issued */
  #define ETH_DMASR_RPS_Fetching        0x00020000U  /* Running - fetching the Rx descriptor */
  #define ETH_DMASR_RPS_Waiting         0x00060000U  /* Running - waiting for packet */
  #define ETH_DMASR_RPS_Suspended       0x00080000U  /* Suspended - Rx Descriptor unavailable */
  #define ETH_DMASR_RPS_Closing         0x000A0000U  /* Running - closing descriptor */
  #define ETH_DMASR_RPS_Queuing         0x000E0000U  /* Running - queuing the recieve frame into host memory */
#define ETH_DMASR_NIS        0x00010000U  /* Normal interrupt summary */
#define ETH_DMASR_AIS        0x00008000U  /* Abnormal interrupt summary */
#define ETH_DMASR_ERS        0x00004000U  /* Early receive status */
#define ETH_DMASR_FBES       0x00002000U  /* Fatal bus error status */
#define ETH_DMASR_ETS        0x00000400U  /* Early transmit status */
#define ETH_DMASR_RWTS       0x00000200U  /* Receive watchdog timeout status */
#define ETH_DMASR_RPSS       0x00000100U  /* Receive process stopped status */
#define ETH_DMASR_RBUS       0x00000080U  /* Receive buffer unavailable status */
#define ETH_DMASR_RS         0x00000040U  /* Receive status */
#define ETH_DMASR_TUS        0x00000020U  /* Transmit underflow status */
#define ETH_DMASR_ROS        0x00000010U  /* Receive overflow status */
#define ETH_DMASR_TJTS       0x00000008U  /* Transmit jabber timeout status */
#define ETH_DMASR_TBUS       0x00000004U  /* Transmit buffer unavailable status */
#define ETH_DMASR_TPSS       0x00000002U  /* Transmit process stopped status */
#define ETH_DMASR_TS         0x00000001U  /* Transmit status */

/* Bit definition for Ethernet DMA Operation Mode Register */
#define ETH_DMAOMR_DTCEFD    0x04000000U  /* Disable Dropping of TCP/IP checksum error frames */
#define ETH_DMAOMR_RSF       0x02000000U  /* Receive store and forward */
#define ETH_DMAOMR_DFRF      0x01000000U  /* Disable flushing of received frames */
#define ETH_DMAOMR_TSF       0x00200000U  /* Transmit store and forward */
#define ETH_DMAOMR_FTF       0x00100000U  /* Flush transmit FIFO */
#define ETH_DMAOMR_TTC       0x0001C000U  /* Transmit threshold control */
  #define ETH_DMAOMR_TTC_64Bytes       0x00000000U  /* threshold level of the MTL Transmit FIFO is 64 Bytes */
  #define ETH_DMAOMR_TTC_128Bytes      0x00004000U  /* threshold level of the MTL Transmit FIFO is 128 Bytes */
  #define ETH_DMAOMR_TTC_192Bytes      0x00008000U  /* threshold level of the MTL Transmit FIFO is 192 Bytes */
  #define ETH_DMAOMR_TTC_256Bytes      0x0000C000U  /* threshold level of the MTL Transmit FIFO is 256 Bytes */
  #define ETH_DMAOMR_TTC_40Bytes       0x00010000U  /* threshold level of the MTL Transmit FIFO is 40 Bytes */
  #define ETH_DMAOMR_TTC_32Bytes       0x00014000U  /* threshold level of the MTL Transmit FIFO is 32 Bytes */
  #define ETH_DMAOMR_TTC_24Bytes       0x00018000U  /* threshold level of the MTL Transmit FIFO is 24 Bytes */
  #define ETH_DMAOMR_TTC_16Bytes       0x0001C000U  /* threshold level of the MTL Transmit FIFO is 16 Bytes */
#define ETH_DMAOMR_ST        0x00002000U  /* Start/stop transmission command */
#define ETH_DMAOMR_FEF       0x00000080U  /* Forward error frames */
#define ETH_DMAOMR_FUGF      0x00000040U  /* Forward undersized good frames */
#define ETH_DMAOMR_RTC       0x00000018U  /* receive threshold control */
  #define ETH_DMAOMR_RTC_64Bytes       0x00000000U  /* threshold level of the MTL Receive FIFO is 64 Bytes */
  #define ETH_DMAOMR_RTC_32Bytes       0x00000008U  /* threshold level of the MTL Receive FIFO is 32 Bytes */
  #define ETH_DMAOMR_RTC_96Bytes       0x00000010U  /* threshold level of the MTL Receive FIFO is 96 Bytes */
  #define ETH_DMAOMR_RTC_128Bytes      0x00000018U  /* threshold level of the MTL Receive FIFO is 128 Bytes */
#define ETH_DMAOMR_OSF       0x00000004U  /* operate on second frame */
#define ETH_DMAOMR_SR        0x00000002U  /* Start/stop receive */

/* Bit definition for Ethernet DMA Interrupt Enable Register */
#define ETH_DMAIER_NISE      0x00010000U  /* Normal interrupt summary enable */
#define ETH_DMAIER_AISE      0x00008000U  /* Abnormal interrupt summary enable */
#define ETH_DMAIER_ERIE      0x00004000U  /* Early receive interrupt enable */
#define ETH_DMAIER_FBEIE     0x00002000U  /* Fatal bus error interrupt enable */
#define ETH_DMAIER_ETIE      0x00000400U  /* Early transmit interrupt enable */
#define ETH_DMAIER_RWTIE     0x00000200U  /* Receive watchdog timeout interrupt enable */
#define ETH_DMAIER_RPSIE     0x00000100U  /* Receive process stopped interrupt enable */
#define ETH_DMAIER_RBUIE     0x00000080U  /* Receive buffer unavailable interrupt enable */
#define ETH_DMAIER_RIE       0x00000040U  /* Receive interrupt enable */
#define ETH_DMAIER_TUIE      0x00000020U  /* Transmit Underflow interrupt enable */
#define ETH_DMAIER_ROIE      0x00000010U  /* Receive Overflow interrupt enable */
#define ETH_DMAIER_TJTIE     0x00000008U  /* Transmit jabber timeout interrupt enable */
#define ETH_DMAIER_TBUIE     0x00000004U  /* Transmit buffer unavailable interrupt enable */
#define ETH_DMAIER_TPSIE     0x00000002U  /* Transmit process stopped interrupt enable */
#define ETH_DMAIER_TIE       0x00000001U  /* Transmit interrupt enable */

/* Bit definition for Ethernet DMA Missed Frame and Buffer Overflow Counter Register */
#define ETH_DMAMFBOCR_OFOC   0x10000000U  /* Overflow bit for FIFO overflow counter */
#define ETH_DMAMFBOCR_MFA    0x0FFE0000U  /* Number of frames missed by the application */
#define ETH_DMAMFBOCR_OMFC   0x00010000U  /* Overflow bit for missed frame counter */
#define ETH_DMAMFBOCR_MFC    0x0000FFFFU  /* Number of frames missed by the controller */

/* Bit definition for Ethernet DMA Current Host Transmit Descriptor Register */
#define ETH_DMACHTDR_HTDAP   0xFFFFFFFFU  /* Host transmit descriptor address pointer */

/* Bit definition for Ethernet DMA Current Host Receive Descriptor Register */
#define ETH_DMACHRDR_HRDAP   0xFFFFFFFFU  /* Host receive descriptor address pointer */

/* Bit definition for Ethernet DMA Current Host Transmit Buffer Address Register */
#define ETH_DMACHTBAR_HTBAP  0xFFFFFFFFU  /* Host transmit buffer address pointer */

/* Bit definition for Ethernet DMA Current Host Receive Buffer Address Register */
#define ETH_DMACHRBAR_HRBAP  0xFFFFFFFFU  /* Host receive buffer address pointer */

/******************************************************************************/
/*                                                                            */
/*                                       USB_OTG                              */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for USB_OTG_GOTGCTL register  ********************/
#define USB_OTG_GOTGCTL_SRQSCS                  0x00000001U         /*!< Session request success */
#define USB_OTG_GOTGCTL_SRQ                     0x00000002U         /*!< Session request */
#define USB_OTG_GOTGCTL_VBVALOEN                0x00000004U         /*!< VBUS valid override enable */
#define USB_OTG_GOTGCTL_VBVALOVAL               0x00000008U         /*!< VBUS valid override value */
#define USB_OTG_GOTGCTL_AVALOEN                 0x00000010U         /*!< A-peripheral session valid override enable */
#define USB_OTG_GOTGCTL_AVALOVAL                0x00000020U         /*!< A-peripheral session valid override value */
#define USB_OTG_GOTGCTL_BVALOEN                 0x00000040U         /*!< B-peripheral session valid override enable */
#define USB_OTG_GOTGCTL_BVALOVAL                0x00000080U         /*!< B-peripheral session valid override value  */
#define USB_OTG_GOTGCTL_HNGSCS                  0x00000100U         /*!< Host set HNP enable */
#define USB_OTG_GOTGCTL_HNPRQ                   0x00000200U         /*!< HNP request */
#define USB_OTG_GOTGCTL_HSHNPEN                 0x00000400U         /*!< Host set HNP enable */
#define USB_OTG_GOTGCTL_DHNPEN                  0x00000800U         /*!< Device HNP enabled */
#define USB_OTG_GOTGCTL_EHEN                    0x00001000U         /*!< Embedded host enable */
#define USB_OTG_GOTGCTL_CIDSTS                  0x00010000U         /*!< Connector ID status */
#define USB_OTG_GOTGCTL_DBCT                    0x00020000U         /*!< Long/short debounce time */
#define USB_OTG_GOTGCTL_ASVLD                   0x00040000U         /*!< A-session valid  */
#define USB_OTG_GOTGCTL_BSESVLD                 0x00080000U         /*!< B-session valid */
#define USB_OTG_GOTGCTL_OTGVER                  0x00100000U         /*!< OTG version  */

/********************  Bit definition for USB_OTG_HCFG register  ********************/
#define USB_OTG_HCFG_FSLSPCS                 0x00000003U            /*!< FS/LS PHY clock select  */
#define USB_OTG_HCFG_FSLSPCS_0               0x00000001U            /*!<Bit 0 */
#define USB_OTG_HCFG_FSLSPCS_1               0x00000002U            /*!<Bit 1 */
#define USB_OTG_HCFG_FSLSS                   0x00000004U            /*!< FS- and LS-only support */

/********************  Bit definition for USB_OTG_DCFG register  ********************/
#define USB_OTG_DCFG_DSPD                    0x00000003U            /*!< Device speed */
#define USB_OTG_DCFG_DSPD_0                  0x00000001U            /*!<Bit 0 */
#define USB_OTG_DCFG_DSPD_1                  0x00000002U            /*!<Bit 1 */
#define USB_OTG_DCFG_NZLSOHSK                0x00000004U            /*!< Nonzero-length status OUT handshake */

#define USB_OTG_DCFG_DAD                     0x000007F0U            /*!< Device address */
#define USB_OTG_DCFG_DAD_0                   0x00000010U            /*!<Bit 0 */
#define USB_OTG_DCFG_DAD_1                   0x00000020U            /*!<Bit 1 */
#define USB_OTG_DCFG_DAD_2                   0x00000040U            /*!<Bit 2 */
#define USB_OTG_DCFG_DAD_3                   0x00000080U            /*!<Bit 3 */
#define USB_OTG_DCFG_DAD_4                   0x00000100U            /*!<Bit 4 */
#define USB_OTG_DCFG_DAD_5                   0x00000200U            /*!<Bit 5 */
#define USB_OTG_DCFG_DAD_6                   0x00000400U            /*!<Bit 6 */

#define USB_OTG_DCFG_PFIVL                   0x00001800U            /*!< Periodic (micro)frame interval */
#define USB_OTG_DCFG_PFIVL_0                 0x00000800U            /*!<Bit 0 */
#define USB_OTG_DCFG_PFIVL_1                 0x00001000U            /*!<Bit 1 */

#define USB_OTG_DCFG_PERSCHIVL               0x03000000U            /*!< Periodic scheduling interval */
#define USB_OTG_DCFG_PERSCHIVL_0             0x01000000U            /*!<Bit 0 */
#define USB_OTG_DCFG_PERSCHIVL_1             0x02000000U            /*!<Bit 1 */

/********************  Bit definition for USB_OTG_PCGCR register  ********************/
#define USB_OTG_PCGCR_STPPCLK                 0x00000001U            /*!< Stop PHY clock */
#define USB_OTG_PCGCR_GATEHCLK                0x00000002U            /*!< Gate HCLK */
#define USB_OTG_PCGCR_PHYSUSP                 0x00000010U            /*!< PHY suspended */

/********************  Bit definition for USB_OTG_GOTGINT register  ********************/
#define USB_OTG_GOTGINT_SEDET                   0x00000004U            /*!< Session end detected                   */
#define USB_OTG_GOTGINT_SRSSCHG                 0x00000100U            /*!< Session request success status change  */
#define USB_OTG_GOTGINT_HNSSCHG                 0x00000200U            /*!< Host negotiation success status change */
#define USB_OTG_GOTGINT_HNGDET                  0x00020000U            /*!< Host negotiation detected              */
#define USB_OTG_GOTGINT_ADTOCHG                 0x00040000U            /*!< A-device timeout change                */
#define USB_OTG_GOTGINT_DBCDNE                  0x00080000U            /*!< Debounce done                          */
#define USB_OTG_GOTGINT_IDCHNG                  0x00100000U            /*!< Change in ID pin input value           */

/********************  Bit definition for USB_OTG_DCTL register  ********************/
#define USB_OTG_DCTL_RWUSIG                  0x00000001U            /*!< Remote wakeup signaling */
#define USB_OTG_DCTL_SDIS                    0x00000002U            /*!< Soft disconnect         */
#define USB_OTG_DCTL_GINSTS                  0x00000004U            /*!< Global IN NAK status    */
#define USB_OTG_DCTL_GONSTS                  0x00000008U            /*!< Global OUT NAK status   */

#define USB_OTG_DCTL_TCTL                    0x00000070U            /*!< Test control */
#define USB_OTG_DCTL_TCTL_0                  0x00000010U            /*!<Bit 0 */
#define USB_OTG_DCTL_TCTL_1                  0x00000020U            /*!<Bit 1 */
#define USB_OTG_DCTL_TCTL_2                  0x00000040U            /*!<Bit 2 */
#define USB_OTG_DCTL_SGINAK                  0x00000080U            /*!< Set global IN NAK         */
#define USB_OTG_DCTL_CGINAK                  0x00000100U            /*!< Clear global IN NAK       */
#define USB_OTG_DCTL_SGONAK                  0x00000200U            /*!< Set global OUT NAK        */
#define USB_OTG_DCTL_CGONAK                  0x00000400U            /*!< Clear global OUT NAK      */
#define USB_OTG_DCTL_POPRGDNE                0x00000800U            /*!< Power-on programming done */

/********************  Bit definition for USB_OTG_HFIR register  ********************/
#define USB_OTG_HFIR_FRIVL                   0x0000FFFFU            /*!< Frame interval */

/********************  Bit definition for USB_OTG_HFNUM register  ********************/
#define USB_OTG_HFNUM_FRNUM                   0x0000FFFFU            /*!< Frame number         */
#define USB_OTG_HFNUM_FTREM                   0xFFFF0000U            /*!< Frame time remaining */

/********************  Bit definition for USB_OTG_DSTS register  ********************/
#define USB_OTG_DSTS_SUSPSTS                 0x00000001U            /*!< Suspend status   */

#define USB_OTG_DSTS_ENUMSPD                 0x00000006U            /*!< Enumerated speed */
#define USB_OTG_DSTS_ENUMSPD_0               0x00000002U            /*!<Bit 0 */
#define USB_OTG_DSTS_ENUMSPD_1               0x00000004U            /*!<Bit 1 */
#define USB_OTG_DSTS_EERR                    0x00000008U            /*!< Erratic error     */
#define USB_OTG_DSTS_FNSOF                   0x003FFF00U            /*!< Frame number of the received SOF */

/********************  Bit definition for USB_OTG_GAHBCFG register  ********************/
#define USB_OTG_GAHBCFG_GINT                    0x00000001U            /*!< Global interrupt mask */
#define USB_OTG_GAHBCFG_HBSTLEN                 0x0000001EU            /*!< Burst length/type */
#define USB_OTG_GAHBCFG_HBSTLEN_0               0x00000002U            /*!<Bit 0 */
#define USB_OTG_GAHBCFG_HBSTLEN_1               0x00000004U            /*!<Bit 1 */
#define USB_OTG_GAHBCFG_HBSTLEN_2               0x00000008U            /*!<Bit 2 */
#define USB_OTG_GAHBCFG_HBSTLEN_3               0x00000010U            /*!<Bit 3 */
#define USB_OTG_GAHBCFG_DMAEN                   0x00000020U            /*!< DMA enable */
#define USB_OTG_GAHBCFG_TXFELVL                 0x00000080U            /*!< TxFIFO empty level */
#define USB_OTG_GAHBCFG_PTXFELVL                0x00000100U            /*!< Periodic TxFIFO empty level */

/********************  Bit definition for USB_OTG_GUSBCFG register  ********************/
#define USB_OTG_GUSBCFG_TOCAL                   0x00000007U            /*!< FS timeout calibration */
#define USB_OTG_GUSBCFG_TOCAL_0                 0x00000001U            /*!<Bit 0 */
#define USB_OTG_GUSBCFG_TOCAL_1                 0x00000002U            /*!<Bit 1 */
#define USB_OTG_GUSBCFG_TOCAL_2                 0x00000004U            /*!<Bit 2 */
#define USB_OTG_GUSBCFG_PHYSEL                  0x00000040U            /*!< USB 2.0 high-speed ULPI PHY or USB 1.1 full-speed serial transceiver select */
#define USB_OTG_GUSBCFG_SRPCAP                  0x00000100U            /*!< SRP-capable */
#define USB_OTG_GUSBCFG_HNPCAP                  0x00000200U            /*!< HNP-capable */
#define USB_OTG_GUSBCFG_TRDT                    0x00003C00U            /*!< USB turnaround time */
#define USB_OTG_GUSBCFG_TRDT_0                  0x00000400U            /*!<Bit 0 */
#define USB_OTG_GUSBCFG_TRDT_1                  0x00000800U            /*!<Bit 1 */
#define USB_OTG_GUSBCFG_TRDT_2                  0x00001000U            /*!<Bit 2 */
#define USB_OTG_GUSBCFG_TRDT_3                  0x00002000U            /*!<Bit 3 */
#define USB_OTG_GUSBCFG_PHYLPCS                 0x00008000U            /*!< PHY Low-power clock select */
#define USB_OTG_GUSBCFG_ULPIFSLS                0x00020000U            /*!< ULPI FS/LS select               */
#define USB_OTG_GUSBCFG_ULPIAR                  0x00040000U            /*!< ULPI Auto-resume                */
#define USB_OTG_GUSBCFG_ULPICSM                 0x00080000U            /*!< ULPI Clock SuspendM             */
#define USB_OTG_GUSBCFG_ULPIEVBUSD              0x00100000U            /*!< ULPI External VBUS Drive        */
#define USB_OTG_GUSBCFG_ULPIEVBUSI              0x00200000U            /*!< ULPI external VBUS indicator    */
#define USB_OTG_GUSBCFG_TSDPS                   0x00400000U            /*!< TermSel DLine pulsing selection */
#define USB_OTG_GUSBCFG_PCCI                    0x00800000U            /*!< Indicator complement            */
#define USB_OTG_GUSBCFG_PTCI                    0x01000000U            /*!< Indicator pass through          */
#define USB_OTG_GUSBCFG_ULPIIPD                 0x02000000U            /*!< ULPI interface protect disable  */
#define USB_OTG_GUSBCFG_FHMOD                   0x20000000U            /*!< Forced host mode                */
#define USB_OTG_GUSBCFG_FDMOD                   0x40000000U            /*!< Forced peripheral mode          */
#define USB_OTG_GUSBCFG_CTXPKT                  0x80000000U            /*!< Corrupt Tx packet               */

/********************  Bit definition for USB_OTG_GRSTCTL register  ********************/
#define USB_OTG_GRSTCTL_CSRST                   0x00000001U            /*!< Core soft reset          */
#define USB_OTG_GRSTCTL_HSRST                   0x00000002U            /*!< HCLK soft reset          */
#define USB_OTG_GRSTCTL_FCRST                   0x00000004U            /*!< Host frame counter reset */
#define USB_OTG_GRSTCTL_RXFFLSH                 0x00000010U            /*!< RxFIFO flush             */
#define USB_OTG_GRSTCTL_TXFFLSH                 0x00000020U            /*!< TxFIFO flush             */
#define USB_OTG_GRSTCTL_TXFNUM                  0x000007C0U            /*!< TxFIFO number */
#define USB_OTG_GRSTCTL_TXFNUM_0                0x00000040U            /*!<Bit 0 */
#define USB_OTG_GRSTCTL_TXFNUM_1                0x00000080U            /*!<Bit 1 */
#define USB_OTG_GRSTCTL_TXFNUM_2                0x00000100U            /*!<Bit 2 */
#define USB_OTG_GRSTCTL_TXFNUM_3                0x00000200U            /*!<Bit 3 */
#define USB_OTG_GRSTCTL_TXFNUM_4                0x00000400U            /*!<Bit 4 */
#define USB_OTG_GRSTCTL_DMAREQ                  0x40000000U            /*!< DMA request signal */
#define USB_OTG_GRSTCTL_AHBIDL                  0x80000000U            /*!< AHB master idle */

/********************  Bit definition for USB_OTG_DIEPMSK register  ********************/
#define USB_OTG_DIEPMSK_XFRCM                   0x00000001U            /*!< Transfer completed interrupt mask                 */
#define USB_OTG_DIEPMSK_EPDM                    0x00000002U            /*!< Endpoint disabled interrupt mask                  */
#define USB_OTG_DIEPMSK_TOM                     0x00000008U            /*!< Timeout condition mask (nonisochronous endpoints) */
#define USB_OTG_DIEPMSK_ITTXFEMSK               0x00000010U            /*!< IN token received when TxFIFO empty mask          */
#define USB_OTG_DIEPMSK_INEPNMM                 0x00000020U            /*!< IN token received with EP mismatch mask           */
#define USB_OTG_DIEPMSK_INEPNEM                 0x00000040U            /*!< IN endpoint NAK effective mask                    */
#define USB_OTG_DIEPMSK_TXFURM                  0x00000100U            /*!< FIFO underrun mask                                */
#define USB_OTG_DIEPMSK_BIM                     0x00000200U            /*!< BNA interrupt mask                                */

/********************  Bit definition for USB_OTG_HPTXSTS register  ********************/
#define USB_OTG_HPTXSTS_PTXFSAVL                0x0000FFFFU            /*!< Periodic transmit data FIFO space available     */
#define USB_OTG_HPTXSTS_PTXQSAV                 0x00FF0000U            /*!< Periodic transmit request queue space available */
#define USB_OTG_HPTXSTS_PTXQSAV_0               0x00010000U            /*!<Bit 0 */
#define USB_OTG_HPTXSTS_PTXQSAV_1               0x00020000U            /*!<Bit 1 */
#define USB_OTG_HPTXSTS_PTXQSAV_2               0x00040000U            /*!<Bit 2 */
#define USB_OTG_HPTXSTS_PTXQSAV_3               0x00080000U            /*!<Bit 3 */
#define USB_OTG_HPTXSTS_PTXQSAV_4               0x00100000U            /*!<Bit 4 */
#define USB_OTG_HPTXSTS_PTXQSAV_5               0x00200000U            /*!<Bit 5 */
#define USB_OTG_HPTXSTS_PTXQSAV_6               0x00400000U            /*!<Bit 6 */
#define USB_OTG_HPTXSTS_PTXQSAV_7               0x00800000U            /*!<Bit 7 */

#define USB_OTG_HPTXSTS_PTXQTOP                 0xFF000000U            /*!< Top of the periodic transmit request queue */
#define USB_OTG_HPTXSTS_PTXQTOP_0               0x01000000U            /*!<Bit 0 */
#define USB_OTG_HPTXSTS_PTXQTOP_1               0x02000000U            /*!<Bit 1 */
#define USB_OTG_HPTXSTS_PTXQTOP_2               0x04000000U            /*!<Bit 2 */
#define USB_OTG_HPTXSTS_PTXQTOP_3               0x08000000U            /*!<Bit 3 */
#define USB_OTG_HPTXSTS_PTXQTOP_4               0x10000000U            /*!<Bit 4 */
#define USB_OTG_HPTXSTS_PTXQTOP_5               0x20000000U            /*!<Bit 5 */
#define USB_OTG_HPTXSTS_PTXQTOP_6               0x40000000U            /*!<Bit 6 */
#define USB_OTG_HPTXSTS_PTXQTOP_7               0x80000000U            /*!<Bit 7 */

/********************  Bit definition for USB_OTG_HAINT register  ********************/
#define USB_OTG_HAINT_HAINT                     0x0000FFFFU            /*!< Channel interrupts */

/********************  Bit definition for USB_OTG_DOEPMSK register  ********************/
#define USB_OTG_DOEPMSK_XFRCM                   0x00000001U            /*!< Transfer completed interrupt mask */
#define USB_OTG_DOEPMSK_EPDM                    0x00000002U            /*!< Endpoint disabled interrupt mask               */
#define USB_OTG_DOEPMSK_STUPM                   0x00000008U            /*!< SETUP phase done mask                          */
#define USB_OTG_DOEPMSK_OTEPDM                  0x00000010U            /*!< OUT token received when endpoint disabled mask */
#define USB_OTG_DOEPMSK_OTEPSPRM                0x00000020U            /*!< Status Phase Received mask                     */
#define USB_OTG_DOEPMSK_B2BSTUP                 0x00000040U            /*!< Back-to-back SETUP packets received mask       */
#define USB_OTG_DOEPMSK_OPEM                    0x00000100U            /*!< OUT packet error mask                          */
#define USB_OTG_DOEPMSK_BOIM                    0x00000200U            /*!< BNA interrupt mask                             */

/********************  Bit definition for USB_OTG_GINTSTS register  ********************/
#define USB_OTG_GINTSTS_CMOD                    0x00000001U            /*!< Current mode of operation                      */
#define USB_OTG_GINTSTS_MMIS                    0x00000002U            /*!< Mode mismatch interrupt                        */
#define USB_OTG_GINTSTS_OTGINT                  0x00000004U            /*!< OTG interrupt                                  */
#define USB_OTG_GINTSTS_SOF                     0x00000008U            /*!< Start of frame                                 */
#define USB_OTG_GINTSTS_RXFLVL                  0x00000010U            /*!< RxFIFO nonempty                                */
#define USB_OTG_GINTSTS_NPTXFE                  0x00000020U            /*!< Nonperiodic TxFIFO empty                       */
#define USB_OTG_GINTSTS_GINAKEFF                0x00000040U            /*!< Global IN nonperiodic NAK effective            */
#define USB_OTG_GINTSTS_BOUTNAKEFF              0x00000080U            /*!< Global OUT NAK effective                       */
#define USB_OTG_GINTSTS_ESUSP                   0x00000400U            /*!< Early suspend                                  */
#define USB_OTG_GINTSTS_USBSUSP                 0x00000800U            /*!< USB suspend                                    */
#define USB_OTG_GINTSTS_USBRST                  0x00001000U            /*!< USB reset                                      */
#define USB_OTG_GINTSTS_ENUMDNE                 0x00002000U            /*!< Enumeration done                               */
#define USB_OTG_GINTSTS_ISOODRP                 0x00004000U            /*!< Isochronous OUT packet dropped interrupt       */
#define USB_OTG_GINTSTS_EOPF                    0x00008000U            /*!< End of periodic frame interrupt                */
#define USB_OTG_GINTSTS_IEPINT                  0x00040000U            /*!< IN endpoint interrupt                          */
#define USB_OTG_GINTSTS_OEPINT                  0x00080000U            /*!< OUT endpoint interrupt                         */
#define USB_OTG_GINTSTS_IISOIXFR                0x00100000U            /*!< Incomplete isochronous IN transfer             */
#define USB_OTG_GINTSTS_PXFR_INCOMPISOOUT       0x00200000U            /*!< Incomplete periodic transfer                   */
#define USB_OTG_GINTSTS_DATAFSUSP               0x00400000U            /*!< Data fetch suspended                           */
#define USB_OTG_GINTSTS_RSTDET                  0x00800000U            /*!< Reset detected interrupt                       */
#define USB_OTG_GINTSTS_HPRTINT                 0x01000000U            /*!< Host port interrupt                            */
#define USB_OTG_GINTSTS_HCINT                   0x02000000U            /*!< Host channels interrupt                        */
#define USB_OTG_GINTSTS_PTXFE                   0x04000000U            /*!< Periodic TxFIFO empty                          */
#define USB_OTG_GINTSTS_LPMINT                  0x08000000U            /*!< LPM interrupt                                  */
#define USB_OTG_GINTSTS_CIDSCHG                 0x10000000U            /*!< Connector ID status change                     */
#define USB_OTG_GINTSTS_DISCINT                 0x20000000U            /*!< Disconnect detected interrupt                  */
#define USB_OTG_GINTSTS_SRQINT                  0x40000000U            /*!< Session request/new session detected interrupt */
#define USB_OTG_GINTSTS_WKUINT                  0x80000000U            /*!< Resume/remote wakeup detected interrupt        */

/********************  Bit definition for USB_OTG_GINTMSK register  ********************/
#define USB_OTG_GINTMSK_MMISM                   0x00000002U            /*!< Mode mismatch interrupt mask                        */
#define USB_OTG_GINTMSK_OTGINT                  0x00000004U            /*!< OTG interrupt mask                                  */
#define USB_OTG_GINTMSK_SOFM                    0x00000008U            /*!< Start of frame mask                                 */
#define USB_OTG_GINTMSK_RXFLVLM                 0x00000010U            /*!< Receive FIFO nonempty mask                          */
#define USB_OTG_GINTMSK_NPTXFEM                 0x00000020U            /*!< Nonperiodic TxFIFO empty mask                       */
#define USB_OTG_GINTMSK_GINAKEFFM               0x00000040U            /*!< Global nonperiodic IN NAK effective mask            */
#define USB_OTG_GINTMSK_GONAKEFFM               0x00000080U            /*!< Global OUT NAK effective mask                       */
#define USB_OTG_GINTMSK_ESUSPM                  0x00000400U            /*!< Early suspend mask                                  */
#define USB_OTG_GINTMSK_USBSUSPM                0x00000800U            /*!< USB suspend mask                                    */
#define USB_OTG_GINTMSK_USBRST                  0x00001000U            /*!< USB reset mask                                      */
#define USB_OTG_GINTMSK_ENUMDNEM                0x00002000U            /*!< Enumeration done mask                               */
#define USB_OTG_GINTMSK_ISOODRPM                0x00004000U            /*!< Isochronous OUT packet dropped interrupt mask       */
#define USB_OTG_GINTMSK_EOPFM                   0x00008000U            /*!< End of periodic frame interrupt mask                */
#define USB_OTG_GINTMSK_EPMISM                  0x00020000U            /*!< Endpoint mismatch interrupt mask                    */
#define USB_OTG_GINTMSK_IEPINT                  0x00040000U            /*!< IN endpoints interrupt mask                         */
#define USB_OTG_GINTMSK_OEPINT                  0x00080000U            /*!< OUT endpoints interrupt mask                        */
#define USB_OTG_GINTMSK_IISOIXFRM               0x00100000U            /*!< Incomplete isochronous IN transfer mask             */
#define USB_OTG_GINTMSK_PXFRM_IISOOXFRM         0x00200000U            /*!< Incomplete periodic transfer mask                   */
#define USB_OTG_GINTMSK_FSUSPM                  0x00400000U            /*!< Data fetch suspended mask                           */
#define USB_OTG_GINTMSK_RSTDEM                  0x00800000U            /*!< Reset detected interrupt mask                       */
#define USB_OTG_GINTMSK_PRTIM                   0x01000000U            /*!< Host port interrupt mask                            */
#define USB_OTG_GINTMSK_HCIM                    0x02000000U            /*!< Host channels interrupt mask                        */
#define USB_OTG_GINTMSK_PTXFEM                  0x04000000U            /*!< Periodic TxFIFO empty mask                          */
#define USB_OTG_GINTMSK_LPMINTM                 0x08000000U            /*!< LPM interrupt Mask                                  */
#define USB_OTG_GINTMSK_CIDSCHGM                0x10000000U            /*!< Connector ID status change mask                     */
#define USB_OTG_GINTMSK_DISCINT                 0x20000000U            /*!< Disconnect detected interrupt mask                  */
#define USB_OTG_GINTMSK_SRQIM                   0x40000000U            /*!< Session request/new session detected interrupt mask */
#define USB_OTG_GINTMSK_WUIM                    0x80000000U            /*!< Resume/remote wakeup detected interrupt mask        */

/********************  Bit definition for USB_OTG_DAINT register  ********************/
#define USB_OTG_DAINT_IEPINT                  0x0000FFFFU            /*!< IN endpoint interrupt bits  */
#define USB_OTG_DAINT_OEPINT                  0xFFFF0000U            /*!< OUT endpoint interrupt bits */

/********************  Bit definition for USB_OTG_HAINTMSK register  ********************/
#define USB_OTG_HAINTMSK_HAINTM                  0x0000FFFFU            /*!< Channel interrupt mask */

/********************  Bit definition for USB_OTG_GRXSTSP register  ********************/
#define USB_OTG_GRXSTSP_EPNUM                    0x0000000FU            /*!< IN EP interrupt mask bits  */
#define USB_OTG_GRXSTSP_BCNT                     0x00007FF0U            /*!< OUT EP interrupt mask bits */
#define USB_OTG_GRXSTSP_DPID                     0x00018000U            /*!< OUT EP interrupt mask bits */
#define USB_OTG_GRXSTSP_PKTSTS                   0x001E0000U            /*!< OUT EP interrupt mask bits */

/********************  Bit definition for USB_OTG_DAINTMSK register  ********************/
#define USB_OTG_DAINTMSK_IEPM                    0x0000FFFFU            /*!< IN EP interrupt mask bits */
#define USB_OTG_DAINTMSK_OEPM                    0xFFFF0000U            /*!< OUT EP interrupt mask bits */

/********************  Bit definition for OTG register  ********************/

#define USB_OTG_CHNUM                   0x0000000FU            /*!< Channel number */
#define USB_OTG_CHNUM_0                 0x00000001U            /*!<Bit 0 */
#define USB_OTG_CHNUM_1                 0x00000002U            /*!<Bit 1 */
#define USB_OTG_CHNUM_2                 0x00000004U            /*!<Bit 2 */
#define USB_OTG_CHNUM_3                 0x00000008U            /*!<Bit 3 */
#define USB_OTG_BCNT                    0x00007FF0U            /*!< Byte count */

#define USB_OTG_DPID                    0x00018000U            /*!< Data PID */
#define USB_OTG_DPID_0                  0x00008000U            /*!<Bit 0 */
#define USB_OTG_DPID_1                  0x00010000U            /*!<Bit 1 */

#define USB_OTG_PKTSTS                  0x001E0000U            /*!< Packet status */
#define USB_OTG_PKTSTS_0                0x00020000U            /*!<Bit 0 */
#define USB_OTG_PKTSTS_1                0x00040000U            /*!<Bit 1 */
#define USB_OTG_PKTSTS_2                0x00080000U            /*!<Bit 2 */
#define USB_OTG_PKTSTS_3                0x00100000U            /*!<Bit 3 */

#define USB_OTG_EPNUM                   0x0000000FU            /*!< Endpoint number */
#define USB_OTG_EPNUM_0                 0x00000001U            /*!<Bit 0 */
#define USB_OTG_EPNUM_1                 0x00000002U            /*!<Bit 1 */
#define USB_OTG_EPNUM_2                 0x00000004U            /*!<Bit 2 */
#define USB_OTG_EPNUM_3                 0x00000008U            /*!<Bit 3 */

#define USB_OTG_FRMNUM                  0x01E00000U            /*!< Frame number */
#define USB_OTG_FRMNUM_0                0x00200000U            /*!<Bit 0 */
#define USB_OTG_FRMNUM_1                0x00400000U            /*!<Bit 1 */
#define USB_OTG_FRMNUM_2                0x00800000U            /*!<Bit 2 */
#define USB_OTG_FRMNUM_3                0x01000000U            /*!<Bit 3 */

/********************  Bit definition for OTG register  ********************/

#define USB_OTG_CHNUM                   0x0000000FU            /*!< Channel number */
#define USB_OTG_CHNUM_0                 0x00000001U            /*!<Bit 0 */
#define USB_OTG_CHNUM_1                 0x00000002U            /*!<Bit 1 */
#define USB_OTG_CHNUM_2                 0x00000004U            /*!<Bit 2 */
#define USB_OTG_CHNUM_3                 0x00000008U            /*!<Bit 3 */
#define USB_OTG_BCNT                    0x00007FF0U            /*!< Byte count */

#define USB_OTG_DPID                    0x00018000U            /*!< Data PID */
#define USB_OTG_DPID_0                  0x00008000U            /*!<Bit 0 */
#define USB_OTG_DPID_1                  0x00010000U            /*!<Bit 1 */

#define USB_OTG_PKTSTS                  0x001E0000U            /*!< Packet status */
#define USB_OTG_PKTSTS_0                0x00020000U            /*!<Bit 0 */
#define USB_OTG_PKTSTS_1                0x00040000U            /*!<Bit 1 */
#define USB_OTG_PKTSTS_2                0x00080000U            /*!<Bit 2 */
#define USB_OTG_PKTSTS_3                0x00100000U            /*!<Bit 3 */

#define USB_OTG_EPNUM                   0x0000000FU            /*!< Endpoint number */
#define USB_OTG_EPNUM_0                 0x00000001U            /*!<Bit 0 */
#define USB_OTG_EPNUM_1                 0x00000002U            /*!<Bit 1 */
#define USB_OTG_EPNUM_2                 0x00000004U            /*!<Bit 2 */
#define USB_OTG_EPNUM_3                 0x00000008U            /*!<Bit 3 */

#define USB_OTG_FRMNUM                  0x01E00000U            /*!< Frame number */
#define USB_OTG_FRMNUM_0                0x00200000U            /*!<Bit 0 */
#define USB_OTG_FRMNUM_1                0x00400000U            /*!<Bit 1 */
#define USB_OTG_FRMNUM_2                0x00800000U            /*!<Bit 2 */
#define USB_OTG_FRMNUM_3                0x01000000U            /*!<Bit 3 */

/********************  Bit definition for USB_OTG_GRXFSIZ register  ********************/
#define USB_OTG_GRXFSIZ_RXFD            0x0000FFFFU            /*!< RxFIFO depth */

/********************  Bit definition for USB_OTG_DVBUSDIS register  ********************/
#define USB_OTG_DVBUSDIS_VBUSDT         0x0000FFFFU            /*!< Device VBUS discharge time */

/********************  Bit definition for OTG register  ********************/
#define USB_OTG_NPTXFSA                 0x0000FFFFU            /*!< Nonperiodic transmit RAM start address */
#define USB_OTG_NPTXFD                  0xFFFF0000U            /*!< Nonperiodic TxFIFO depth               */
#define USB_OTG_TX0FSA                  0x0000FFFFU            /*!< Endpoint 0 transmit RAM start address  */
#define USB_OTG_TX0FD                   0xFFFF0000U            /*!< Endpoint 0 TxFIFO depth                */

/********************  Bit definition for USB_OTG_DVBUSPULSE register  ********************/
#define USB_OTG_DVBUSPULSE_DVBUSP                0x00000FFFU            /*!< Device VBUS pulsing time */

/********************  Bit definition for USB_OTG_GNPTXSTS register  ********************/
#define USB_OTG_GNPTXSTS_NPTXFSAV                0x0000FFFFU            /*!< Nonperiodic TxFIFO space available */

#define USB_OTG_GNPTXSTS_NPTQXSAV                0x00FF0000U            /*!< Nonperiodic transmit request queue space available */
#define USB_OTG_GNPTXSTS_NPTQXSAV_0              0x00010000U            /*!<Bit 0 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_1              0x00020000U            /*!<Bit 1 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_2              0x00040000U            /*!<Bit 2 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_3              0x00080000U            /*!<Bit 3 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_4              0x00100000U            /*!<Bit 4 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_5              0x00200000U            /*!<Bit 5 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_6              0x00400000U            /*!<Bit 6 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_7              0x00800000U            /*!<Bit 7 */

#define USB_OTG_GNPTXSTS_NPTXQTOP                0x7F000000U            /*!< Top of the nonperiodic transmit request queue */
#define USB_OTG_GNPTXSTS_NPTXQTOP_0              0x01000000U            /*!<Bit 0 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_1              0x02000000U            /*!<Bit 1 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_2              0x04000000U            /*!<Bit 2 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_3              0x08000000U            /*!<Bit 3 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_4              0x10000000U            /*!<Bit 4 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_5              0x20000000U            /*!<Bit 5 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_6              0x40000000U            /*!<Bit 6 */

/********************  Bit definition for USB_OTG_DTHRCTL register  ********************/
#define USB_OTG_DTHRCTL_NONISOTHREN             0x00000001U            /*!< Nonisochronous IN endpoints threshold enable */
#define USB_OTG_DTHRCTL_ISOTHREN                0x00000002U            /*!< ISO IN endpoint threshold enable */

#define USB_OTG_DTHRCTL_TXTHRLEN                0x000007FCU            /*!< Transmit threshold length */
#define USB_OTG_DTHRCTL_TXTHRLEN_0              0x00000004U            /*!<Bit 0 */
#define USB_OTG_DTHRCTL_TXTHRLEN_1              0x00000008U            /*!<Bit 1 */
#define USB_OTG_DTHRCTL_TXTHRLEN_2              0x00000010U            /*!<Bit 2 */
#define USB_OTG_DTHRCTL_TXTHRLEN_3              0x00000020U            /*!<Bit 3 */
#define USB_OTG_DTHRCTL_TXTHRLEN_4              0x00000040U            /*!<Bit 4 */
#define USB_OTG_DTHRCTL_TXTHRLEN_5              0x00000080U            /*!<Bit 5 */
#define USB_OTG_DTHRCTL_TXTHRLEN_6              0x00000100U            /*!<Bit 6 */
#define USB_OTG_DTHRCTL_TXTHRLEN_7              0x00000200U            /*!<Bit 7 */
#define USB_OTG_DTHRCTL_TXTHRLEN_8              0x00000400U            /*!<Bit 8 */
#define USB_OTG_DTHRCTL_RXTHREN                 0x00010000U            /*!< Receive threshold enable */

#define USB_OTG_DTHRCTL_RXTHRLEN                0x03FE0000U            /*!< Receive threshold length */
#define USB_OTG_DTHRCTL_RXTHRLEN_0              0x00020000U            /*!<Bit 0 */
#define USB_OTG_DTHRCTL_RXTHRLEN_1              0x00040000U            /*!<Bit 1 */
#define USB_OTG_DTHRCTL_RXTHRLEN_2              0x00080000U            /*!<Bit 2 */
#define USB_OTG_DTHRCTL_RXTHRLEN_3              0x00100000U            /*!<Bit 3 */
#define USB_OTG_DTHRCTL_RXTHRLEN_4              0x00200000U            /*!<Bit 4 */
#define USB_OTG_DTHRCTL_RXTHRLEN_5              0x00400000U            /*!<Bit 5 */
#define USB_OTG_DTHRCTL_RXTHRLEN_6              0x00800000U            /*!<Bit 6 */
#define USB_OTG_DTHRCTL_RXTHRLEN_7              0x01000000U            /*!<Bit 7 */
#define USB_OTG_DTHRCTL_RXTHRLEN_8              0x02000000U            /*!<Bit 8 */
#define USB_OTG_DTHRCTL_ARPEN                   0x08000000U            /*!< Arbiter parking enable */

/********************  Bit definition for USB_OTG_DIEPEMPMSK register  ********************/
#define USB_OTG_DIEPEMPMSK_INEPTXFEM            0x0000FFFFU         /*!< IN EP Tx FIFO empty interrupt mask bits */

/********************  Bit definition for USB_OTG_DEACHINT register  ********************/
#define USB_OTG_DEACHINT_IEP1INT                0x00000002U           /*!< IN endpoint 1interrupt bit   */
#define USB_OTG_DEACHINT_OEP1INT                0x00020000U           /*!< OUT endpoint 1 interrupt bit */

/********************  Bit definition for USB_OTG_GCCFG register  ********************/
#define USB_OTG_GCCFG_PWRDWN                 0x00010000U              /*!< Power down */
#define USB_OTG_GCCFG_VBDEN                  0x00200000U              /*!< USB VBUS Detection Enable */

/********************  Bit definition for USB_OTG_GPWRDN) register  ********************/
#define USB_OTG_GPWRDN_ADPMEN                 0x00000001U             /*!< ADP module enable */
#define USB_OTG_GPWRDN_ADPIF                  0x00800000U             /*!< ADP Interrupt flag */

/********************  Bit definition for USB_OTG_DEACHINTMSK register  ********************/
#define USB_OTG_DEACHINTMSK_IEP1INTM          0x00000002U            /*!< IN Endpoint 1 interrupt mask bit  */
#define USB_OTG_DEACHINTMSK_OEP1INTM          0x00020000U            /*!< OUT Endpoint 1 interrupt mask bit */

/********************  Bit definition for USB_OTG_CID register  ********************/
#define USB_OTG_CID_PRODUCT_ID               0xFFFFFFFFU            /*!< Product ID field */

/********************  Bit definition for USB_OTG_GLPMCFG register  ********************/
#define  USB_OTG_GLPMCFG_LPMEN               0x00000001U            /*!< LPM support enable                                     */
#define  USB_OTG_GLPMCFG_LPMACK              0x00000002U            /*!< LPM Token acknowledge enable                           */
#define  USB_OTG_GLPMCFG_BESL                0x0000003CU            /*!< BESL value received with last ACKed LPM Token          */
#define  USB_OTG_GLPMCFG_REMWAKE             0x00000040U            /*!< bRemoteWake value received with last ACKed LPM Token   */
#define  USB_OTG_GLPMCFG_L1SSEN              0x00000080U            /*!< L1 shallow sleep enable                                */
#define  USB_OTG_GLPMCFG_BESLTHRS            0x00000F00U            /*!< BESL threshold                                         */
#define  USB_OTG_GLPMCFG_L1DSEN              0x00001000U            /*!< L1 deep sleep enable                                   */
#define  USB_OTG_GLPMCFG_LPMRSP              0x00006000U            /*!< LPM response                                           */
#define  USB_OTG_GLPMCFG_SLPSTS              0x00008000U            /*!< Port sleep status                                      */
#define  USB_OTG_GLPMCFG_L1RSMOK             0x00010000U            /*!< Sleep State Resume OK                                  */
#define  USB_OTG_GLPMCFG_LPMCHIDX            0x001E0000U            /*!< LPM Channel Index                                      */
#define  USB_OTG_GLPMCFG_LPMRCNT             0x00E00000U            /*!< LPM retry count                                        */
#define  USB_OTG_GLPMCFG_SNDLPM              0x01000000U            /*!< Send LPM transaction                                   */
#define  USB_OTG_GLPMCFG_LPMRCNTSTS          0x0E000000U            /*!< LPM retry count status                                 */
#define  USB_OTG_GLPMCFG_ENBESL              0x10000000U            /*!< Enable best effort service latency                     */

/********************  Bit definition for USB_OTG_DIEPEACHMSK1 register  ********************/
#define USB_OTG_DIEPEACHMSK1_XFRCM           0x00000001U            /*!< Transfer completed interrupt mask                 */
#define USB_OTG_DIEPEACHMSK1_EPDM            0x00000002U            /*!< Endpoint disabled interrupt mask                  */
#define USB_OTG_DIEPEACHMSK1_TOM             0x00000008U            /*!< Timeout condition mask (nonisochronous endpoints) */
#define USB_OTG_DIEPEACHMSK1_ITTXFEMSK       0x00000010U            /*!< IN token received when TxFIFO empty mask          */
#define USB_OTG_DIEPEACHMSK1_INEPNMM         0x00000020U            /*!< IN token received with EP mismatch mask           */
#define USB_OTG_DIEPEACHMSK1_INEPNEM         0x00000040U            /*!< IN endpoint NAK effective mask                    */
#define USB_OTG_DIEPEACHMSK1_TXFURM          0x00000100U            /*!< FIFO underrun mask                                */
#define USB_OTG_DIEPEACHMSK1_BIM             0x00000200U            /*!< BNA interrupt mask                                */
#define USB_OTG_DIEPEACHMSK1_NAKM            0x00002000U            /*!< NAK interrupt mask                                */

/********************  Bit definition for USB_OTG_HPRT register  ********************/
#define USB_OTG_HPRT_PCSTS                   0x00000001U            /*!< Port connect status        */
#define USB_OTG_HPRT_PCDET                   0x00000002U            /*!< Port connect detected      */
#define USB_OTG_HPRT_PENA                    0x00000004U            /*!< Port enable                */
#define USB_OTG_HPRT_PENCHNG                 0x00000008U            /*!< Port enable/disable change */
#define USB_OTG_HPRT_POCA                    0x00000010U            /*!< Port overcurrent active    */
#define USB_OTG_HPRT_POCCHNG                 0x00000020U            /*!< Port overcurrent change    */
#define USB_OTG_HPRT_PRES                    0x00000040U            /*!< Port resume                */
#define USB_OTG_HPRT_PSUSP                   0x00000080U            /*!< Port suspend               */
#define USB_OTG_HPRT_PRST                    0x00000100U            /*!< Port reset                 */

#define USB_OTG_HPRT_PLSTS                   0x00000C00U            /*!< Port line status           */
#define USB_OTG_HPRT_PLSTS_0                 0x00000400U            /*!<Bit 0 */
#define USB_OTG_HPRT_PLSTS_1                 0x00000800U            /*!<Bit 1 */
#define USB_OTG_HPRT_PPWR                    0x00001000U            /*!< Port power                 */

#define USB_OTG_HPRT_PTCTL                   0x0001E000U            /*!< Port test control          */
#define USB_OTG_HPRT_PTCTL_0                 0x00002000U            /*!<Bit 0 */
#define USB_OTG_HPRT_PTCTL_1                 0x00004000U            /*!<Bit 1 */
#define USB_OTG_HPRT_PTCTL_2                 0x00008000U            /*!<Bit 2 */
#define USB_OTG_HPRT_PTCTL_3                 0x00010000U            /*!<Bit 3 */

#define USB_OTG_HPRT_PSPD                    0x00060000U            /*!< Port speed                 */
#define USB_OTG_HPRT_PSPD_0                  0x00020000U            /*!<Bit 0 */
#define USB_OTG_HPRT_PSPD_1                  0x00040000U            /*!<Bit 1 */

/********************  Bit definition for USB_OTG_DOEPEACHMSK1 register  ********************/
#define USB_OTG_DOEPEACHMSK1_XFRCM               0x00000001U            /*!< Transfer completed interrupt mask         */
#define USB_OTG_DOEPEACHMSK1_EPDM                0x00000002U            /*!< Endpoint disabled interrupt mask          */
#define USB_OTG_DOEPEACHMSK1_TOM                 0x00000008U            /*!< Timeout condition mask                    */
#define USB_OTG_DOEPEACHMSK1_ITTXFEMSK           0x00000010U            /*!< IN token received when TxFIFO empty mask  */
#define USB_OTG_DOEPEACHMSK1_INEPNMM             0x00000020U            /*!< IN token received with EP mismatch mask   */
#define USB_OTG_DOEPEACHMSK1_INEPNEM             0x00000040U            /*!< IN endpoint NAK effective mask            */
#define USB_OTG_DOEPEACHMSK1_TXFURM              0x00000100U            /*!< OUT packet error mask                     */
#define USB_OTG_DOEPEACHMSK1_BIM                 0x00000200U            /*!< BNA interrupt mask                        */
#define USB_OTG_DOEPEACHMSK1_BERRM               0x00001000U            /*!< Bubble error interrupt mask               */
#define USB_OTG_DOEPEACHMSK1_NAKM                0x00002000U            /*!< NAK interrupt mask                        */
#define USB_OTG_DOEPEACHMSK1_NYETM               0x00004000U            /*!< NYET interrupt mask                       */

/********************  Bit definition for USB_OTG_HPTXFSIZ register  ********************/
#define USB_OTG_HPTXFSIZ_PTXSA                   0x0000FFFFU            /*!< Host periodic TxFIFO start address            */
#define USB_OTG_HPTXFSIZ_PTXFD                   0xFFFF0000U            /*!< Host periodic TxFIFO depth                    */

/********************  Bit definition for USB_OTG_DIEPCTL register  ********************/
#define USB_OTG_DIEPCTL_MPSIZ                   0x000007FFU            /*!< Maximum packet size              */
#define USB_OTG_DIEPCTL_USBAEP                  0x00008000U            /*!< USB active endpoint              */
#define USB_OTG_DIEPCTL_EONUM_DPID              0x00010000U            /*!< Even/odd frame                   */
#define USB_OTG_DIEPCTL_NAKSTS                  0x00020000U            /*!< NAK status                       */

#define USB_OTG_DIEPCTL_EPTYP                   0x000C0000U            /*!< Endpoint type                    */
#define USB_OTG_DIEPCTL_EPTYP_0                 0x00040000U            /*!<Bit 0 */
#define USB_OTG_DIEPCTL_EPTYP_1                 0x00080000U            /*!<Bit 1 */
#define USB_OTG_DIEPCTL_STALL                   0x00200000U            /*!< STALL handshake                  */

#define USB_OTG_DIEPCTL_TXFNUM                  0x03C00000U            /*!< TxFIFO number                    */
#define USB_OTG_DIEPCTL_TXFNUM_0                0x00400000U            /*!<Bit 0 */
#define USB_OTG_DIEPCTL_TXFNUM_1                0x00800000U            /*!<Bit 1 */
#define USB_OTG_DIEPCTL_TXFNUM_2                0x01000000U            /*!<Bit 2 */
#define USB_OTG_DIEPCTL_TXFNUM_3                0x02000000U            /*!<Bit 3 */
#define USB_OTG_DIEPCTL_CNAK                    0x04000000U            /*!< Clear NAK                        */
#define USB_OTG_DIEPCTL_SNAK                    0x08000000U            /*!< Set NAK */
#define USB_OTG_DIEPCTL_SD0PID_SEVNFRM          0x10000000U            /*!< Set DATA0 PID                    */
#define USB_OTG_DIEPCTL_SODDFRM                 0x20000000U            /*!< Set odd frame                    */
#define USB_OTG_DIEPCTL_EPDIS                   0x40000000U            /*!< Endpoint disable                 */
#define USB_OTG_DIEPCTL_EPENA                   0x80000000U            /*!< Endpoint enable                  */

/********************  Bit definition for USB_OTG_HCCHAR register  ********************/
#define USB_OTG_HCCHAR_MPSIZ                   0x000007FFU            /*!< Maximum packet size */

#define USB_OTG_HCCHAR_EPNUM                   0x00007800U            /*!< Endpoint number */
#define USB_OTG_HCCHAR_EPNUM_0                 0x00000800U            /*!<Bit 0 */
#define USB_OTG_HCCHAR_EPNUM_1                 0x00001000U            /*!<Bit 1 */
#define USB_OTG_HCCHAR_EPNUM_2                 0x00002000U            /*!<Bit 2 */
#define USB_OTG_HCCHAR_EPNUM_3                 0x00004000U            /*!<Bit 3 */
#define USB_OTG_HCCHAR_EPDIR                   0x00008000U            /*!< Endpoint direction */
#define USB_OTG_HCCHAR_LSDEV                   0x00020000U            /*!< Low-speed device */

#define USB_OTG_HCCHAR_EPTYP                   0x000C0000U            /*!< Endpoint type */
#define USB_OTG_HCCHAR_EPTYP_0                 0x00040000U            /*!<Bit 0 */
#define USB_OTG_HCCHAR_EPTYP_1                 0x00080000U            /*!<Bit 1 */

#define USB_OTG_HCCHAR_MC                      0x00300000U            /*!< Multi Count (MC) / Error Count (EC) */
#define USB_OTG_HCCHAR_MC_0                    0x00100000U            /*!<Bit 0 */
#define USB_OTG_HCCHAR_MC_1                    0x00200000U            /*!<Bit 1 */

#define USB_OTG_HCCHAR_DAD                     0x1FC00000U            /*!< Device address */
#define USB_OTG_HCCHAR_DAD_0                   0x00400000U            /*!<Bit 0 */
#define USB_OTG_HCCHAR_DAD_1                   0x00800000U            /*!<Bit 1 */
#define USB_OTG_HCCHAR_DAD_2                   0x01000000U            /*!<Bit 2 */
#define USB_OTG_HCCHAR_DAD_3                   0x02000000U            /*!<Bit 3 */
#define USB_OTG_HCCHAR_DAD_4                   0x04000000U            /*!<Bit 4 */
#define USB_OTG_HCCHAR_DAD_5                   0x08000000U            /*!<Bit 5 */
#define USB_OTG_HCCHAR_DAD_6                   0x10000000U            /*!<Bit 6 */
#define USB_OTG_HCCHAR_ODDFRM                  0x20000000U            /*!< Odd frame */
#define USB_OTG_HCCHAR_CHDIS                   0x40000000U            /*!< Channel disable */
#define USB_OTG_HCCHAR_CHENA                   0x80000000U            /*!< Channel enable */

/********************  Bit definition for USB_OTG_HCSPLT register  ********************/

#define USB_OTG_HCSPLT_PRTADDR                 0x0000007FU            /*!< Port address */
#define USB_OTG_HCSPLT_PRTADDR_0               0x00000001U            /*!<Bit 0 */
#define USB_OTG_HCSPLT_PRTADDR_1               0x00000002U            /*!<Bit 1 */
#define USB_OTG_HCSPLT_PRTADDR_2               0x00000004U            /*!<Bit 2 */
#define USB_OTG_HCSPLT_PRTADDR_3               0x00000008U            /*!<Bit 3 */
#define USB_OTG_HCSPLT_PRTADDR_4               0x00000010U            /*!<Bit 4 */
#define USB_OTG_HCSPLT_PRTADDR_5               0x00000020U            /*!<Bit 5 */
#define USB_OTG_HCSPLT_PRTADDR_6               0x00000040U            /*!<Bit 6 */

#define USB_OTG_HCSPLT_HUBADDR                 0x00003F80U            /*!< Hub address */
#define USB_OTG_HCSPLT_HUBADDR_0               0x00000080U            /*!<Bit 0 */
#define USB_OTG_HCSPLT_HUBADDR_1               0x00000100U            /*!<Bit 1 */
#define USB_OTG_HCSPLT_HUBADDR_2               0x00000200U            /*!<Bit 2 */
#define USB_OTG_HCSPLT_HUBADDR_3               0x00000400U            /*!<Bit 3 */
#define USB_OTG_HCSPLT_HUBADDR_4               0x00000800U            /*!<Bit 4 */
#define USB_OTG_HCSPLT_HUBADDR_5               0x00001000U            /*!<Bit 5 */
#define USB_OTG_HCSPLT_HUBADDR_6               0x00002000U            /*!<Bit 6 */

#define USB_OTG_HCSPLT_XACTPOS                 0x0000C000U            /*!< XACTPOS */
#define USB_OTG_HCSPLT_XACTPOS_0               0x00004000U            /*!<Bit 0 */
#define USB_OTG_HCSPLT_XACTPOS_1               0x00008000U            /*!<Bit 1 */
#define USB_OTG_HCSPLT_COMPLSPLT               0x00010000U            /*!< Do complete split */
#define USB_OTG_HCSPLT_SPLITEN                 0x80000000U            /*!< Split enable */

/********************  Bit definition for USB_OTG_HCINT register  ********************/
#define USB_OTG_HCINT_XFRC                    0x00000001U            /*!< Transfer completed */
#define USB_OTG_HCINT_CHH                     0x00000002U            /*!< Channel halted */
#define USB_OTG_HCINT_AHBERR                  0x00000004U            /*!< AHB error */
#define USB_OTG_HCINT_STALL                   0x00000008U            /*!< STALL response received interrupt */
#define USB_OTG_HCINT_NAK                     0x00000010U            /*!< NAK response received interrupt */
#define USB_OTG_HCINT_ACK                     0x00000020U            /*!< ACK response received/transmitted interrupt */
#define USB_OTG_HCINT_NYET                    0x00000040U            /*!< Response received interrupt */
#define USB_OTG_HCINT_TXERR                   0x00000080U            /*!< Transaction error */
#define USB_OTG_HCINT_BBERR                   0x00000100U            /*!< Babble error */
#define USB_OTG_HCINT_FRMOR                   0x00000200U            /*!< Frame overrun */
#define USB_OTG_HCINT_DTERR                   0x00000400U            /*!< Data toggle error */

/********************  Bit definition for USB_OTG_DIEPINT register  ********************/
#define USB_OTG_DIEPINT_XFRC                    0x00000001U            /*!< Transfer completed interrupt */
#define USB_OTG_DIEPINT_EPDISD                  0x00000002U            /*!< Endpoint disabled interrupt */
#define USB_OTG_DIEPINT_TOC                     0x00000008U            /*!< Timeout condition */
#define USB_OTG_DIEPINT_ITTXFE                  0x00000010U            /*!< IN token received when TxFIFO is empty */
#define USB_OTG_DIEPINT_INEPNE                  0x00000040U            /*!< IN endpoint NAK effective */
#define USB_OTG_DIEPINT_TXFE                    0x00000080U            /*!< Transmit FIFO empty */
#define USB_OTG_DIEPINT_TXFIFOUDRN              0x00000100U            /*!< Transmit Fifo Underrun */
#define USB_OTG_DIEPINT_BNA                     0x00000200U            /*!< Buffer not available interrupt */
#define USB_OTG_DIEPINT_PKTDRPSTS               0x00000800U            /*!< Packet dropped status */
#define USB_OTG_DIEPINT_BERR                    0x00001000U            /*!< Babble error interrupt */
#define USB_OTG_DIEPINT_NAK                     0x00002000U            /*!< NAK interrupt */

/********************  Bit definition for USB_OTG_HCINTMSK register  ********************/
#define USB_OTG_HCINTMSK_XFRCM                   0x00000001U            /*!< Transfer completed mask */
#define USB_OTG_HCINTMSK_CHHM                    0x00000002U            /*!< Channel halted mask */
#define USB_OTG_HCINTMSK_AHBERR                  0x00000004U            /*!< AHB error */
#define USB_OTG_HCINTMSK_STALLM                  0x00000008U            /*!< STALL response received interrupt mask */
#define USB_OTG_HCINTMSK_NAKM                    0x00000010U            /*!< NAK response received interrupt mask */
#define USB_OTG_HCINTMSK_ACKM                    0x00000020U            /*!< ACK response received/transmitted interrupt mask */
#define USB_OTG_HCINTMSK_NYET                    0x00000040U            /*!< response received interrupt mask */
#define USB_OTG_HCINTMSK_TXERRM                  0x00000080U            /*!< Transaction error mask */
#define USB_OTG_HCINTMSK_BBERRM                  0x00000100U            /*!< Babble error mask */
#define USB_OTG_HCINTMSK_FRMORM                  0x00000200U            /*!< Frame overrun mask */
#define USB_OTG_HCINTMSK_DTERRM                  0x00000400U            /*!< Data toggle error mask */

/********************  Bit definition for USB_OTG_DIEPTSIZ register  ********************/

#define USB_OTG_DIEPTSIZ_XFRSIZ                  0x0007FFFFU            /*!< Transfer size */
#define USB_OTG_DIEPTSIZ_PKTCNT                  0x1FF80000U            /*!< Packet count */
#define USB_OTG_DIEPTSIZ_MULCNT                  0x60000000U            /*!< Packet count */
/********************  Bit definition for USB_OTG_HCTSIZ register  ********************/
#define USB_OTG_HCTSIZ_XFRSIZ                    0x0007FFFFU            /*!< Transfer size */
#define USB_OTG_HCTSIZ_PKTCNT                    0x1FF80000U            /*!< Packet count */
#define USB_OTG_HCTSIZ_DOPING                    0x80000000U            /*!< Do PING */
#define USB_OTG_HCTSIZ_DPID                      0x60000000U            /*!< Data PID */
#define USB_OTG_HCTSIZ_DPID_0                    0x20000000U            /*!<Bit 0 */
#define USB_OTG_HCTSIZ_DPID_1                    0x40000000U            /*!<Bit 1 */

/********************  Bit definition for USB_OTG_DIEPDMA register  ********************/
#define USB_OTG_DIEPDMA_DMAADDR                  0xFFFFFFFFU            /*!< DMA address */

/********************  Bit definition for USB_OTG_HCDMA register  ********************/
#define USB_OTG_HCDMA_DMAADDR                    0xFFFFFFFFU            /*!< DMA address */

/********************  Bit definition for USB_OTG_DTXFSTS register  ********************/
#define USB_OTG_DTXFSTS_INEPTFSAV                0x0000FFFFU            /*!< IN endpoint TxFIFO space available */

/********************  Bit definition for USB_OTG_DIEPTXF register  ********************/
#define USB_OTG_DIEPTXF_INEPTXSA                 0x0000FFFFU            /*!< IN endpoint FIFOx transmit RAM start address */
#define USB_OTG_DIEPTXF_INEPTXFD                 0xFFFF0000U            /*!< IN endpoint TxFIFO depth */

/********************  Bit definition for USB_OTG_DOEPCTL register  ********************/
#define USB_OTG_DOEPCTL_MPSIZ                     0x000007FFU            /*!< Maximum packet size */          /*!<Bit 1 */
#define USB_OTG_DOEPCTL_USBAEP                    0x00008000U            /*!< USB active endpoint */
#define USB_OTG_DOEPCTL_NAKSTS                    0x00020000U            /*!< NAK status */
#define USB_OTG_DOEPCTL_SD0PID_SEVNFRM            0x10000000U            /*!< Set DATA0 PID */
#define USB_OTG_DOEPCTL_SODDFRM                   0x20000000U            /*!< Set odd frame */
#define USB_OTG_DOEPCTL_EPTYP                     0x000C0000U            /*!< Endpoint type */
#define USB_OTG_DOEPCTL_EPTYP_0                   0x00040000U            /*!<Bit 0 */
#define USB_OTG_DOEPCTL_EPTYP_1                   0x00080000U            /*!<Bit 1 */
#define USB_OTG_DOEPCTL_SNPM                      0x00100000U            /*!< Snoop mode */
#define USB_OTG_DOEPCTL_STALL                     0x00200000U            /*!< STALL handshake */
#define USB_OTG_DOEPCTL_CNAK                      0x04000000U            /*!< Clear NAK */
#define USB_OTG_DOEPCTL_SNAK                      0x08000000U            /*!< Set NAK */
#define USB_OTG_DOEPCTL_EPDIS                     0x40000000U            /*!< Endpoint disable */
#define USB_OTG_DOEPCTL_EPENA                     0x80000000U            /*!< Endpoint enable */

/********************  Bit definition for USB_OTG_DOEPINT register  ********************/
#define USB_OTG_DOEPINT_XFRC                    0x00000001U            /*!< Transfer completed interrupt */
#define USB_OTG_DOEPINT_EPDISD                  0x00000002U            /*!< Endpoint disabled interrupt */
#define USB_OTG_DOEPINT_STUP                    0x00000008U            /*!< SETUP phase done */
#define USB_OTG_DOEPINT_OTEPDIS                 0x00000010U            /*!< OUT token received when endpoint disabled */
#define USB_OTG_DOEPINT_OTEPSPR                 0x00000020U            /*!< Status Phase Received For Control Write */
#define USB_OTG_DOEPINT_B2BSTUP                 0x00000040U            /*!< Back-to-back SETUP packets received */
#define USB_OTG_DOEPINT_NYET                    0x00004000U            /*!< NYET interrupt */

/********************  Bit definition for USB_OTG_DOEPTSIZ register  ********************/
#define USB_OTG_DOEPTSIZ_XFRSIZ                  0x0007FFFFU            /*!< Transfer size */
#define USB_OTG_DOEPTSIZ_PKTCNT                  0x1FF80000U            /*!< Packet count */

#define USB_OTG_DOEPTSIZ_STUPCNT                 0x60000000U            /*!< SETUP packet count */
#define USB_OTG_DOEPTSIZ_STUPCNT_0               0x20000000U            /*!<Bit 0 */
#define USB_OTG_DOEPTSIZ_STUPCNT_1               0x40000000U            /*!<Bit 1 */

/********************  Bit definition for PCGCCTL register  ********************/
#define USB_OTG_PCGCCTL_STOPCLK                 0x00000001U            /*!< SETUP packet count */
#define USB_OTG_PCGCCTL_GATECLK                 0x00000002U            /*!<Bit 0 */
#define USB_OTG_PCGCCTL_PHYSUSP                 0x00000010U            /*!<Bit 1 */




/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup Exported_macros
  * @{
  */

/******************************* ADC Instances ********************************/
#define IS_ADC_ALL_INSTANCE(__INSTANCE__) (((__INSTANCE__) == ADC1) || \
                                       ((__INSTANCE__) == ADC2) || \
                                       ((__INSTANCE__) == ADC3))

/******************************* CAN Instances ********************************/
#define IS_CAN_ALL_INSTANCE(__INSTANCE__) (((__INSTANCE__) == CAN1) || \
                                           ((__INSTANCE__) == CAN2))
/******************************* CRC Instances ********************************/
#define IS_CRC_ALL_INSTANCE(__INSTANCE__) ((__INSTANCE__) == CRC)

/******************************* DAC Instances ********************************/
#define IS_DAC_ALL_INSTANCE(__INSTANCE__) ((__INSTANCE__) == DAC)

/******************************* DCMI Instances *******************************/
#define IS_DCMI_ALL_INSTANCE(__INSTANCE__) ((__INSTANCE__) == DCMI)


/******************************* DMA2D Instances *******************************/
#define IS_DMA2D_ALL_INSTANCE(__INSTANCE__) ((__INSTANCE__) == DMA2D)

/******************************** DMA Instances *******************************/
#define IS_DMA_STREAM_ALL_INSTANCE(__INSTANCE__) (((__INSTANCE__) == DMA1_Stream0) || \
                                              ((__INSTANCE__) == DMA1_Stream1) || \
                                              ((__INSTANCE__) == DMA1_Stream2) || \
                                              ((__INSTANCE__) == DMA1_Stream3) || \
                                              ((__INSTANCE__) == DMA1_Stream4) || \
                                              ((__INSTANCE__) == DMA1_Stream5) || \
                                              ((__INSTANCE__) == DMA1_Stream6) || \
                                              ((__INSTANCE__) == DMA1_Stream7) || \
                                              ((__INSTANCE__) == DMA2_Stream0) || \
                                              ((__INSTANCE__) == DMA2_Stream1) || \
                                              ((__INSTANCE__) == DMA2_Stream2) || \
                                              ((__INSTANCE__) == DMA2_Stream3) || \
                                              ((__INSTANCE__) == DMA2_Stream4) || \
                                              ((__INSTANCE__) == DMA2_Stream5) || \
                                              ((__INSTANCE__) == DMA2_Stream6) || \
                                              ((__INSTANCE__) == DMA2_Stream7))

/******************************* GPIO Instances *******************************/
#define IS_GPIO_ALL_INSTANCE(__INSTANCE__) (((__INSTANCE__) == GPIOA) || \
                                            ((__INSTANCE__) == GPIOB) || \
                                            ((__INSTANCE__) == GPIOC) || \
                                            ((__INSTANCE__) == GPIOD) || \
                                            ((__INSTANCE__) == GPIOE) || \
                                            ((__INSTANCE__) == GPIOF) || \
                                            ((__INSTANCE__) == GPIOG) || \
                                            ((__INSTANCE__) == GPIOH) || \
                                            ((__INSTANCE__) == GPIOI) || \
                                            ((__INSTANCE__) == GPIOJ) || \
                                            ((__INSTANCE__) == GPIOK))

#define IS_GPIO_AF_INSTANCE(__INSTANCE__)   (((__INSTANCE__) == GPIOA) || \
                                             ((__INSTANCE__) == GPIOB) || \
                                             ((__INSTANCE__) == GPIOC) || \
                                             ((__INSTANCE__) == GPIOD) || \
                                             ((__INSTANCE__) == GPIOE) || \
                                             ((__INSTANCE__) == GPIOF) || \
                                             ((__INSTANCE__) == GPIOG) || \
                                             ((__INSTANCE__) == GPIOH) || \
                                             ((__INSTANCE__) == GPIOI) || \
                                             ((__INSTANCE__) == GPIOJ) || \
                                             ((__INSTANCE__) == GPIOK))

/****************************** CEC Instances *********************************/
#define IS_CEC_ALL_INSTANCE(__INSTANCE__) ((__INSTANCE__) == CEC)

/****************************** QSPI Instances *********************************/
#define IS_QSPI_ALL_INSTANCE(__INSTANCE__) ((__INSTANCE__) == QUADSPI)


/******************************** I2C Instances *******************************/
#define IS_I2C_ALL_INSTANCE(__INSTANCE__) (((__INSTANCE__) == I2C1) || \
                                           ((__INSTANCE__) == I2C2) || \
                                           ((__INSTANCE__) == I2C3) || \
                                           ((__INSTANCE__) == I2C4))

/******************************** I2S Instances *******************************/
#define IS_I2S_ALL_INSTANCE(__INSTANCE__)  (((__INSTANCE__) == SPI1) || \
                                            ((__INSTANCE__) == SPI2) || \
                                            ((__INSTANCE__) == SPI3))

/******************************* LPTIM Instances ********************************/
#define IS_LPTIM_INSTANCE(__INSTANCE__) ((__INSTANCE__) == LPTIM1)

/****************************** LTDC Instances ********************************/
#define IS_LTDC_ALL_INSTANCE(__INSTANCE__)  ((__INSTANCE__) == LTDC)




/******************************* RNG Instances ********************************/
#define IS_RNG_ALL_INSTANCE(__INSTANCE__)  ((__INSTANCE__) == RNG)

/****************************** RTC Instances *********************************/
#define IS_RTC_ALL_INSTANCE(__INSTANCE__)  ((__INSTANCE__) == RTC)

/******************************* SAI Instances ********************************/
#define IS_SAI_ALL_INSTANCE(__PERIPH__) (((__PERIPH__) == SAI1_Block_A) || \
                                         ((__PERIPH__) == SAI1_Block_B) || \
                                         ((__PERIPH__) == SAI2_Block_A) || \
                                         ((__PERIPH__) == SAI2_Block_B))
/* Legacy define */
#define IS_SAI_BLOCK_PERIPH IS_SAI_ALL_INSTANCE

/******************************** SDMMC Instances *******************************/
#define IS_SDMMC_ALL_INSTANCE(__INSTANCE__) ((__INSTANCE__) == SDMMC1)

/****************************** SPDIFRX Instances *********************************/
#define IS_SPDIFRX_ALL_INSTANCE(__INSTANCE__) ((__INSTANCE__) == SPDIFRX)

/******************************** SPI Instances *******************************/
#define IS_SPI_ALL_INSTANCE(__INSTANCE__) (((__INSTANCE__) == SPI1) || \
                                           ((__INSTANCE__) == SPI2) || \
                                           ((__INSTANCE__) == SPI3) || \
                                           ((__INSTANCE__) == SPI4) || \
                                           ((__INSTANCE__) == SPI5) || \
                                           ((__INSTANCE__) == SPI6))

/****************** TIM Instances : All supported instances *******************/
#define IS_TIM_INSTANCE(__INSTANCE__) (((__INSTANCE__) == TIM1)   || \
                                   ((__INSTANCE__) == TIM2)   || \
                                   ((__INSTANCE__) == TIM3)   || \
                                   ((__INSTANCE__) == TIM4)   || \
                                   ((__INSTANCE__) == TIM5)   || \
                                   ((__INSTANCE__) == TIM6)   || \
                                   ((__INSTANCE__) == TIM7)   || \
                                   ((__INSTANCE__) == TIM8)   || \
                                   ((__INSTANCE__) == TIM9)   || \
                                   ((__INSTANCE__) == TIM10)  || \
                                   ((__INSTANCE__) == TIM11)  || \
                                   ((__INSTANCE__) == TIM12)  || \
                                   ((__INSTANCE__) == TIM13)  || \
                                   ((__INSTANCE__) == TIM14))

/************* TIM Instances : at least 1 capture/compare channel *************/
#define IS_TIM_CC1_INSTANCE(__INSTANCE__)   (((__INSTANCE__) == TIM1)  || \
                                         ((__INSTANCE__) == TIM2)  || \
                                         ((__INSTANCE__) == TIM3)  || \
                                         ((__INSTANCE__) == TIM4)  || \
                                         ((__INSTANCE__) == TIM5)  || \
                                         ((__INSTANCE__) == TIM8)  || \
                                         ((__INSTANCE__) == TIM9)  || \
                                         ((__INSTANCE__) == TIM10) || \
                                         ((__INSTANCE__) == TIM11) || \
                                         ((__INSTANCE__) == TIM12) || \
                                         ((__INSTANCE__) == TIM13) || \
                                         ((__INSTANCE__) == TIM14))

/************ TIM Instances : at least 2 capture/compare channels *************/
#define IS_TIM_CC2_INSTANCE(__INSTANCE__)   (((__INSTANCE__) == TIM1)  || \
                                         ((__INSTANCE__) == TIM2)  || \
                                         ((__INSTANCE__) == TIM3)  || \
                                         ((__INSTANCE__) == TIM4)  || \
                                         ((__INSTANCE__) == TIM5)  || \
                                         ((__INSTANCE__) == TIM8)  || \
                                         ((__INSTANCE__) == TIM9)  || \
                                         ((__INSTANCE__) == TIM12))

/************ TIM Instances : at least 3 capture/compare channels *************/
#define IS_TIM_CC3_INSTANCE(__INSTANCE__)   (((__INSTANCE__) == TIM1) || \
                                         ((__INSTANCE__) == TIM2) || \
                                         ((__INSTANCE__) == TIM3) || \
                                         ((__INSTANCE__) == TIM4) || \
                                         ((__INSTANCE__) == TIM5) || \
                                         ((__INSTANCE__) == TIM8))

/************ TIM Instances : at least 4 capture/compare channels *************/
#define IS_TIM_CC4_INSTANCE(__INSTANCE__) (((__INSTANCE__) == TIM1) || \
                                       ((__INSTANCE__) == TIM2) || \
                                       ((__INSTANCE__) == TIM3) || \
                                       ((__INSTANCE__) == TIM4) || \
                                       ((__INSTANCE__) == TIM5) || \
                                       ((__INSTANCE__) == TIM8))

/****************** TIM Instances : supporting combined 3-phase PWM mode ******/
#define IS_TIM_COMBINED3PHASEPWM_INSTANCE(__INSTANCE__) \
                                       (((__INSTANCE__) == TIM1)    || \
                                        ((__INSTANCE__) == TIM8))

/****************** TIM Instances : supporting OCxREF clear *******************/
#define IS_TIM_OCXREF_CLEAR_INSTANCE(__INSTANCE__)\
                                  (((__INSTANCE__) == TIM1)    || \
                                   ((__INSTANCE__) == TIM2)    || \
                                   ((__INSTANCE__) == TIM3)    || \
                                   ((__INSTANCE__) == TIM4)    || \
                                   ((__INSTANCE__) == TIM8))

/****************** TIM Instances : supporting external clock mode 1 for TIX inputs*/
#define IS_TIM_CLOCKSOURCE_TIX_INSTANCE(__INSTANCE__)\
                                                 (((__INSTANCE__) == TIM1)    || \
                                                  ((__INSTANCE__) == TIM2)    || \
                                                  ((__INSTANCE__) == TIM3)    || \
                                                  ((__INSTANCE__) == TIM4)    || \
                                                  ((__INSTANCE__) == TIM5)    || \
                                                  ((__INSTANCE__) == TIM8))

/****************** TIM Instances : supporting internal trigger inputs(ITRX) *******/
#define IS_TIM_CLOCKSOURCE_ITRX_INSTANCE(__INSTANCE__)\
                                                   (((__INSTANCE__) == TIM1)    || \
                                                    ((__INSTANCE__) == TIM2)    || \
                                                    ((__INSTANCE__) == TIM3)    || \
                                                    ((__INSTANCE__) == TIM4)    || \
                                                    ((__INSTANCE__) == TIM5)    || \
                                                    ((__INSTANCE__) == TIM8))
/****************** TIM Instances : at least 5 capture/compare channels *******/
#define IS_TIM_CC5_INSTANCE(__INSTANCE__)\
  (((__INSTANCE__) == TIM1)    || \
   ((__INSTANCE__) == TIM8) )

/****************** TIM Instances : at least 6 capture/compare channels *******/
#define IS_TIM_CC6_INSTANCE(__INSTANCE__)\
  (((__INSTANCE__) == TIM1)    || \
   ((__INSTANCE__) == TIM8))


/******************** TIM Instances : Advanced-control timers *****************/
#define IS_TIM_ADVANCED_INSTANCE(__INSTANCE__) (((__INSTANCE__) == TIM1) || \
                                            ((__INSTANCE__) == TIM8))

/****************** TIM Instances : supporting 2 break inputs *****************/
#define IS_TIM_BREAK_INSTANCE(__INSTANCE__)\
  (((__INSTANCE__) == TIM1)    || \
   ((__INSTANCE__) == TIM8))

/******************* TIM Instances : Timer input XOR function *****************/
#define IS_TIM_XOR_INSTANCE(__INSTANCE__)   (((__INSTANCE__) == TIM1) || \
                                         ((__INSTANCE__) == TIM2) || \
                                         ((__INSTANCE__) == TIM3) || \
                                         ((__INSTANCE__) == TIM4) || \
                                         ((__INSTANCE__) == TIM5) || \
                                         ((__INSTANCE__) == TIM8))

/****************** TIM Instances : DMA requests generation (UDE) *************/
#define IS_TIM_DMA_INSTANCE(__INSTANCE__) (((__INSTANCE__) == TIM1) || \
                                       ((__INSTANCE__) == TIM2) || \
                                       ((__INSTANCE__) == TIM3) || \
                                       ((__INSTANCE__) == TIM4) || \
                                       ((__INSTANCE__) == TIM5) || \
                                       ((__INSTANCE__) == TIM6) || \
                                       ((__INSTANCE__) == TIM7) || \
                                       ((__INSTANCE__) == TIM8))

/************ TIM Instances : DMA requests generation (CCxDE) *****************/
#define IS_TIM_DMA_CC_INSTANCE(__INSTANCE__) (((__INSTANCE__) == TIM1) || \
                                          ((__INSTANCE__) == TIM2) || \
                                          ((__INSTANCE__) == TIM3) || \
                                          ((__INSTANCE__) == TIM4) || \
                                          ((__INSTANCE__) == TIM5) || \
                                          ((__INSTANCE__) == TIM8))

/************ TIM Instances : DMA requests generation (COMDE) *****************/
#define IS_TIM_CCDMA_INSTANCE(__INSTANCE__)  (((__INSTANCE__) == TIM1) || \
                                          ((__INSTANCE__) == TIM2) || \
                                          ((__INSTANCE__) == TIM3) || \
                                          ((__INSTANCE__) == TIM4) || \
                                          ((__INSTANCE__) == TIM5) || \
                                          ((__INSTANCE__) == TIM8))

/******************** TIM Instances : DMA burst feature ***********************/
#define IS_TIM_DMABURST_INSTANCE(__INSTANCE__)  (((__INSTANCE__) == TIM1) || \
                                             ((__INSTANCE__) == TIM2) || \
                                             ((__INSTANCE__) == TIM3) || \
                                             ((__INSTANCE__) == TIM4) || \
                                             ((__INSTANCE__) == TIM5) || \
                                             ((__INSTANCE__) == TIM8))

/****** TIM Instances : master mode available (TIMx_CR2.MMS available )********/
#define IS_TIM_MASTER_INSTANCE(__INSTANCE__) (((__INSTANCE__) == TIM1) || \
                                          ((__INSTANCE__) == TIM2) || \
                                          ((__INSTANCE__) == TIM3) || \
                                          ((__INSTANCE__) == TIM4) || \
                                          ((__INSTANCE__) == TIM5) || \
                                          ((__INSTANCE__) == TIM6) || \
                                          ((__INSTANCE__) == TIM7) || \
                                          ((__INSTANCE__) == TIM8) || \
                                          ((__INSTANCE__) == TIM13) || \
                                          ((__INSTANCE__) == TIM14))

/*********** TIM Instances : Slave mode available (TIMx_SMCR available )*******/
#define IS_TIM_SLAVE_INSTANCE(__INSTANCE__) (((__INSTANCE__) == TIM1) || \
                                         ((__INSTANCE__) == TIM2) || \
                                         ((__INSTANCE__) == TIM3) || \
                                         ((__INSTANCE__) == TIM4) || \
                                         ((__INSTANCE__) == TIM5) || \
                                         ((__INSTANCE__) == TIM8) || \
                                         ((__INSTANCE__) == TIM9) || \
                                         ((__INSTANCE__) == TIM12))

/********************** TIM Instances : 32 bit Counter ************************/
#define IS_TIM_32B_COUNTER_INSTANCE(__INSTANCE__)(((__INSTANCE__) == TIM2) || \
                                              ((__INSTANCE__) == TIM5))

/***************** TIM Instances : external trigger input available ************/
#define IS_TIM_ETR_INSTANCE(__INSTANCE__)  (((__INSTANCE__) == TIM1) || \
                                        ((__INSTANCE__) == TIM2) || \
                                        ((__INSTANCE__) == TIM3) || \
                                        ((__INSTANCE__) == TIM4) || \
                                        ((__INSTANCE__) == TIM5) || \
                                        ((__INSTANCE__) == TIM8))

/****************** TIM Instances : remapping capability **********************/
#define IS_TIM_REMAP_INSTANCE(__INSTANCE__) (((__INSTANCE__) == TIM2)  || \
                                         ((__INSTANCE__) == TIM5)  || \
                                         ((__INSTANCE__) == TIM11))

/******************* TIM Instances : output(s) available **********************/
#define IS_TIM_CCX_INSTANCE(__INSTANCE__, __CHANNEL__) \
    ((((__INSTANCE__) == TIM1) &&                  \
     (((__CHANNEL__) == TIM_CHANNEL_1) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_2) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_3) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_4)))           \
    ||                                         \
    (((__INSTANCE__) == TIM2) &&                   \
     (((__CHANNEL__) == TIM_CHANNEL_1) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_2) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_3) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_4)))           \
    ||                                         \
    (((__INSTANCE__) == TIM3) &&                   \
     (((__CHANNEL__) == TIM_CHANNEL_1) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_2) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_3) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_4)))           \
    ||                                         \
    (((__INSTANCE__) == TIM4) &&                   \
     (((__CHANNEL__) == TIM_CHANNEL_1) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_2) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_3) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_4)))           \
    ||                                         \
    (((__INSTANCE__) == TIM5) &&                   \
     (((__CHANNEL__) == TIM_CHANNEL_1) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_2) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_3) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_4)))           \
    ||                                         \
    (((__INSTANCE__) == TIM8) &&                   \
     (((__CHANNEL__) == TIM_CHANNEL_1) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_2) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_3) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_4)))           \
    ||                                         \
    (((__INSTANCE__) == TIM9) &&                   \
     (((__CHANNEL__) == TIM_CHANNEL_1) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_2)))           \
    ||                                         \
    (((__INSTANCE__) == TIM10) &&                  \
     (((__CHANNEL__) == TIM_CHANNEL_1)))           \
    ||                                         \
    (((__INSTANCE__) == TIM11) &&                  \
     (((__CHANNEL__) == TIM_CHANNEL_1)))           \
    ||                                         \
    (((__INSTANCE__) == TIM12) &&                  \
     (((__CHANNEL__) == TIM_CHANNEL_1) ||          \
      ((__CHANNEL__) == TIM_CHANNEL_2)))           \
    ||                                         \
    (((__INSTANCE__) == TIM13) &&                  \
     (((__CHANNEL__) == TIM_CHANNEL_1)))           \
    ||                                         \
    (((__INSTANCE__) == TIM14) &&                  \
     (((__CHANNEL__) == TIM_CHANNEL_1))))

/************ TIM Instances : complementary output(s) available ***************/
#define IS_TIM_CCXN_INSTANCE(__INSTANCE__, __CHANNEL__) \
   ((((__INSTANCE__) == TIM1) &&                    \
     (((__CHANNEL__) == TIM_CHANNEL_1) ||           \
      ((__CHANNEL__) == TIM_CHANNEL_2) ||           \
      ((__CHANNEL__) == TIM_CHANNEL_3)))            \
    ||                                          \
    (((__INSTANCE__) == TIM8) &&                    \
     (((__CHANNEL__) == TIM_CHANNEL_1) ||           \
      ((__CHANNEL__) == TIM_CHANNEL_2) ||           \
      ((__CHANNEL__) == TIM_CHANNEL_3))))

/****************** TIM Instances : supporting ADC triggering through TRGO2 ***/
#define IS_TIM_TRGO2_INSTANCE(__INSTANCE__)\
  (((__INSTANCE__) == TIM1)    || \
   ((__INSTANCE__) == TIM8) )

/****************** TIM Instances : supporting synchronization ****************/
#define IS_TIM_SYNCHRO_INSTANCE(__INSTANCE__)\
    (((__INSTANCE__) == TIM1)    || \
     ((__INSTANCE__) == TIM2)    || \
     ((__INSTANCE__) == TIM3)    || \
     ((__INSTANCE__) == TIM4)    || \
     ((__INSTANCE__) == TIM5)    || \
     ((__INSTANCE__) == TIM6)    || \
     ((__INSTANCE__) == TIM7)    || \
     ((__INSTANCE__) == TIM8))

/******************** USART Instances : Synchronous mode **********************/
#define IS_USART_INSTANCE(__INSTANCE__) (((__INSTANCE__) == USART1) || \
                                     ((__INSTANCE__) == USART2) || \
                                     ((__INSTANCE__) == USART3) || \
                                     ((__INSTANCE__) == USART6))

/******************** UART Instances : Asynchronous mode **********************/
#define IS_UART_INSTANCE(__INSTANCE__) (((__INSTANCE__) == USART1) || \
                                    ((__INSTANCE__) == USART2) || \
                                    ((__INSTANCE__) == USART3) || \
                                    ((__INSTANCE__) == UART4)  || \
                                    ((__INSTANCE__) == UART5)  || \
                                    ((__INSTANCE__) == USART6) || \
                                    ((__INSTANCE__) == UART7)  || \
                                    ((__INSTANCE__) == UART8))

/****************** UART Instances : Driver Enable *****************/
#define IS_UART_DRIVER_ENABLE_INSTANCE(__INSTANCE__) (((__INSTANCE__) == USART1) || \
                                    ((__INSTANCE__) == USART2) || \
                                    ((__INSTANCE__) == USART3) || \
                                    ((__INSTANCE__) == UART4)  || \
                                    ((__INSTANCE__) == UART5)  || \
                                    ((__INSTANCE__) == USART6) || \
                                    ((__INSTANCE__) == UART7)  || \
                                    ((__INSTANCE__) == UART8))

/****************** UART Instances : Hardware Flow control ********************/
#define IS_UART_HWFLOW_INSTANCE(__INSTANCE__) (((__INSTANCE__) == USART1) || \
                                    ((__INSTANCE__) == USART2) || \
                                    ((__INSTANCE__) == USART3) || \
                                    ((__INSTANCE__) == UART4)  || \
                                    ((__INSTANCE__) == UART5)  || \
                                    ((__INSTANCE__) == USART6) || \
                                    ((__INSTANCE__) == UART7)  || \
                                    ((__INSTANCE__) == UART8))

/********************* UART Instances : Smart card mode ***********************/
#define IS_SMARTCARD_INSTANCE(__INSTANCE__) (((__INSTANCE__) == USART1) || \
                                         ((__INSTANCE__) == USART2) || \
                                         ((__INSTANCE__) == USART3) || \
                                         ((__INSTANCE__) == USART6))

/*********************** UART Instances : IRDA mode ***************************/
#define IS_IRDA_INSTANCE(__INSTANCE__) (((__INSTANCE__) == USART1) || \
                                    ((__INSTANCE__) == USART2) || \
                                    ((__INSTANCE__) == USART3) || \
                                    ((__INSTANCE__) == UART4)  || \
                                    ((__INSTANCE__) == UART5)  || \
                                    ((__INSTANCE__) == USART6) || \
                                    ((__INSTANCE__) == UART7)  || \
                                    ((__INSTANCE__) == UART8))

/****************************** IWDG Instances ********************************/
#define IS_IWDG_ALL_INSTANCE(__INSTANCE__)  ((__INSTANCE__) == IWDG)

/****************************** WWDG Instances ********************************/
#define IS_WWDG_ALL_INSTANCE(__INSTANCE__)  ((__INSTANCE__) == WWDG)


/******************************************************************************/
/*  For a painless codes migration between the STM32F7xx device product       */
/*  lines, the aliases defined below are put in place to overcome the         */
/*  differences in the interrupt handlers and IRQn definitions.               */
/*  No need to update developed interrupt code when moving across             */
/*  product lines within the same STM32F7 Family                              */
/******************************************************************************/

/* Aliases for __IRQn */
#define HASH_RNG_IRQn              RNG_IRQn

/* Aliases for __IRQHandler */
#define HASH_RNG_IRQHandler        RNG_IRQHandler

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __STM32F746xx_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
