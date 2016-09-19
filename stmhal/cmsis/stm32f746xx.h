/**
  ******************************************************************************
  * @file    stm32f746xx.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-June-2015
  * @brief   CMSIS STM32F746xx Device Peripheral Access Layer Header File.
  *
  *          This file contains:
  *           - Data structures and the address mapping for all peripherals
  *           - Peripheral's registers declarations and bits definition
  *           - Macros to access peripheral's registers hardware
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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
typedef enum IRQn
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
  SDMMC1_IRQn                 = 49,     /*!< SDMMC1 global Interrupt                                             */
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
  SPDIF_RX_IRQn               = 97      /*!< SPDIF-RX global Interrupt                                         */
} IRQn_Type;

/**
  * @}
  */

/**
 * @brief Configuration of the Cortex-M7 Processor and Core Peripherals
 */
#define __CM7_REV                 0x0000   /*!< Cortex-M7 revision r0p1                       */
#define __MPU_PRESENT             1       /*!< CM7 provides an MPU                           */
#define __NVIC_PRIO_BITS          4       /*!< CM7 uses 4 Bits for the Priority Levels       */
#define __Vendor_SysTickConfig    0       /*!< Set to 1 if different SysTick Config is used  */
#define __FPU_PRESENT             1       /*!< FPU present                                   */
#define __ICACHE_PRESENT          1       /*!< CM7 instruction cache present                 */
#define __DCACHE_PRESENT          1       /*!< CM7 data cache present                        */
#include "core_cm7.h"                 /*!< Cortex-M7 processor and core peripherals      */


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
  __IO uint32_t CLUTWR;         /*!< LTDC Layerx CLUT Write Register                              Address offset: 0x144 */

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
  __IO uint32_t OR;       /*!< LPTIM Option register,                              Address offset: 0x20 */
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
#define RAMITCM_BASE           ((uint32_t)0x00000000) /*!< Base address of :16KB RAM reserved for CPU execution/instruction accessible over ITCM   */
#define FLASHITCM_BASE         ((uint32_t)0x00200000) /*!< Base address of :(up to 1 MB) embedded FLASH memory  accessible over ITCM               */
#define FLASHAXI_BASE          ((uint32_t)0x08000000) /*!< Base address of : (up to 1 MB) embedded FLASH memory accessible over AXI                */
#define RAMDTCM_BASE           ((uint32_t)0x20000000) /*!< Base address of : 64KB system data RAM accessible over DTCM                             */
#define SRAM1_BASE             ((uint32_t)0x20010000) /*!< Base address of : 240KB RAM1 accessible over AXI/AHB                                    */
#define SRAM2_BASE             ((uint32_t)0x2004C000) /*!< Base address of : 16KB RAM2 accessible over AXI/AHB                                     */
#define PERIPH_BASE            ((uint32_t)0x40000000) /*!< Base address of : AHB/ABP Peripherals                                                   */
#define BKPSRAM_BASE           ((uint32_t)0x40024000) /*!< Base address of : Backup SRAM(4 KB)                                                     */
#define QSPI_BASE              ((uint32_t)0x90000000) /*!< Base address of : QSPI memories  accessible over AXI                                    */
#define FMC_R_BASE             ((uint32_t)0xA0000000) /*!< Base address of : FMC Control registers                                                 */
#define QSPI_R_BASE            ((uint32_t)0xA0001000) /*!< Base address of : QSPI Control  registers                                               */
#define FLASH_END              ((uint32_t)0x080FFFFF) /*!< FLASH end address */

/* Legacy define */
#define FLASH_BASE     FLASHAXI_BASE

/*!< Peripheral memory map */
#define APB1PERIPH_BASE        PERIPH_BASE
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000)
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000)
#define AHB2PERIPH_BASE       (PERIPH_BASE + 0x10000000)

/*!< APB1 peripherals */
#define TIM2_BASE             (APB1PERIPH_BASE + 0x0000)
#define TIM3_BASE             (APB1PERIPH_BASE + 0x0400)
#define TIM4_BASE             (APB1PERIPH_BASE + 0x0800)
#define TIM5_BASE             (APB1PERIPH_BASE + 0x0C00)
#define TIM6_BASE             (APB1PERIPH_BASE + 0x1000)
#define TIM7_BASE             (APB1PERIPH_BASE + 0x1400)
#define TIM12_BASE            (APB1PERIPH_BASE + 0x1800)
#define TIM13_BASE            (APB1PERIPH_BASE + 0x1C00)
#define TIM14_BASE            (APB1PERIPH_BASE + 0x2000)
#define LPTIM1_BASE           (APB1PERIPH_BASE + 0x2400)
#define RTC_BASE              (APB1PERIPH_BASE + 0x2800)
#define WWDG_BASE             (APB1PERIPH_BASE + 0x2C00)
#define IWDG_BASE             (APB1PERIPH_BASE + 0x3000)
#define SPI2_BASE             (APB1PERIPH_BASE + 0x3800)
#define SPI3_BASE             (APB1PERIPH_BASE + 0x3C00)
#define SPDIFRX_BASE          (APB1PERIPH_BASE + 0x4000)
#define USART2_BASE           (APB1PERIPH_BASE + 0x4400)
#define USART3_BASE           (APB1PERIPH_BASE + 0x4800)
#define UART4_BASE            (APB1PERIPH_BASE + 0x4C00)
#define UART5_BASE            (APB1PERIPH_BASE + 0x5000)
#define I2C1_BASE             (APB1PERIPH_BASE + 0x5400)
#define I2C2_BASE             (APB1PERIPH_BASE + 0x5800)
#define I2C3_BASE             (APB1PERIPH_BASE + 0x5C00)
#define I2C4_BASE             (APB1PERIPH_BASE + 0x6000)
#define CAN1_BASE             (APB1PERIPH_BASE + 0x6400)
#define CAN2_BASE             (APB1PERIPH_BASE + 0x6800)
#define CEC_BASE              (APB1PERIPH_BASE + 0x6C00)
#define PWR_BASE              (APB1PERIPH_BASE + 0x7000)
#define DAC_BASE              (APB1PERIPH_BASE + 0x7400)
#define UART7_BASE            (APB1PERIPH_BASE + 0x7800)
#define UART8_BASE            (APB1PERIPH_BASE + 0x7C00)

/*!< APB2 peripherals */
#define TIM1_BASE             (APB2PERIPH_BASE + 0x0000)
#define TIM8_BASE             (APB2PERIPH_BASE + 0x0400)
#define USART1_BASE           (APB2PERIPH_BASE + 0x1000)
#define USART6_BASE           (APB2PERIPH_BASE + 0x1400)
#define ADC1_BASE             (APB2PERIPH_BASE + 0x2000)
#define ADC2_BASE             (APB2PERIPH_BASE + 0x2100)
#define ADC3_BASE             (APB2PERIPH_BASE + 0x2200)
#define ADC_BASE              (APB2PERIPH_BASE + 0x2300)
#define SDMMC1_BASE           (APB2PERIPH_BASE + 0x2C00)
#define SPI1_BASE             (APB2PERIPH_BASE + 0x3000)
#define SPI4_BASE             (APB2PERIPH_BASE + 0x3400)
#define SYSCFG_BASE           (APB2PERIPH_BASE + 0x3800)
#define EXTI_BASE             (APB2PERIPH_BASE + 0x3C00)
#define TIM9_BASE             (APB2PERIPH_BASE + 0x4000)
#define TIM10_BASE            (APB2PERIPH_BASE + 0x4400)
#define TIM11_BASE            (APB2PERIPH_BASE + 0x4800)
#define SPI5_BASE             (APB2PERIPH_BASE + 0x5000)
#define SPI6_BASE             (APB2PERIPH_BASE + 0x5400)
#define SAI1_BASE             (APB2PERIPH_BASE + 0x5800)
#define SAI2_BASE             (APB2PERIPH_BASE + 0x5C00)
#define SAI1_Block_A_BASE     (SAI1_BASE + 0x004)
#define SAI1_Block_B_BASE     (SAI1_BASE + 0x024)
#define SAI2_Block_A_BASE     (SAI2_BASE + 0x004)
#define SAI2_Block_B_BASE     (SAI2_BASE + 0x024)
#define LTDC_BASE             (APB2PERIPH_BASE + 0x6800)
#define LTDC_Layer1_BASE      (LTDC_BASE + 0x84)
#define LTDC_Layer2_BASE      (LTDC_BASE + 0x104)
/*!< AHB1 peripherals */
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000)
#define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400)
#define GPIOC_BASE            (AHB1PERIPH_BASE + 0x0800)
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00)
#define GPIOE_BASE            (AHB1PERIPH_BASE + 0x1000)
#define GPIOF_BASE            (AHB1PERIPH_BASE + 0x1400)
#define GPIOG_BASE            (AHB1PERIPH_BASE + 0x1800)
#define GPIOH_BASE            (AHB1PERIPH_BASE + 0x1C00)
#define GPIOI_BASE            (AHB1PERIPH_BASE + 0x2000)
#define GPIOJ_BASE            (AHB1PERIPH_BASE + 0x2400)
#define GPIOK_BASE            (AHB1PERIPH_BASE + 0x2800)
#define CRC_BASE              (AHB1PERIPH_BASE + 0x3000)
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800)
#define FLASH_R_BASE          (AHB1PERIPH_BASE + 0x3C00)
#define DMA1_BASE             (AHB1PERIPH_BASE + 0x6000)
#define DMA1_Stream0_BASE     (DMA1_BASE + 0x010)
#define DMA1_Stream1_BASE     (DMA1_BASE + 0x028)
#define DMA1_Stream2_BASE     (DMA1_BASE + 0x040)
#define DMA1_Stream3_BASE     (DMA1_BASE + 0x058)
#define DMA1_Stream4_BASE     (DMA1_BASE + 0x070)
#define DMA1_Stream5_BASE     (DMA1_BASE + 0x088)
#define DMA1_Stream6_BASE     (DMA1_BASE + 0x0A0)
#define DMA1_Stream7_BASE     (DMA1_BASE + 0x0B8)
#define DMA2_BASE             (AHB1PERIPH_BASE + 0x6400)
#define DMA2_Stream0_BASE     (DMA2_BASE + 0x010)
#define DMA2_Stream1_BASE     (DMA2_BASE + 0x028)
#define DMA2_Stream2_BASE     (DMA2_BASE + 0x040)
#define DMA2_Stream3_BASE     (DMA2_BASE + 0x058)
#define DMA2_Stream4_BASE     (DMA2_BASE + 0x070)
#define DMA2_Stream5_BASE     (DMA2_BASE + 0x088)
#define DMA2_Stream6_BASE     (DMA2_BASE + 0x0A0)
#define DMA2_Stream7_BASE     (DMA2_BASE + 0x0B8)
#define ETH_BASE              (AHB1PERIPH_BASE + 0x8000)
#define ETH_MAC_BASE          (ETH_BASE)
#define ETH_MMC_BASE          (ETH_BASE + 0x0100)
#define ETH_PTP_BASE          (ETH_BASE + 0x0700)
#define ETH_DMA_BASE          (ETH_BASE + 0x1000)
#define DMA2D_BASE            (AHB1PERIPH_BASE + 0xB000)
/*!< AHB2 peripherals */
#define DCMI_BASE             (AHB2PERIPH_BASE + 0x50000)
#define RNG_BASE              (AHB2PERIPH_BASE + 0x60800)
/*!< FMC Bankx registers base address */
#define FMC_Bank1_R_BASE      (FMC_R_BASE + 0x0000)
#define FMC_Bank1E_R_BASE     (FMC_R_BASE + 0x0104)
#define FMC_Bank3_R_BASE      (FMC_R_BASE + 0x0080)
#define FMC_Bank5_6_R_BASE    (FMC_R_BASE + 0x0140)

/* Debug MCU registers base address */
#define DBGMCU_BASE           ((uint32_t )0xE0042000)

/*!< USB registers base address */
#define USB_OTG_HS_PERIPH_BASE               ((uint32_t )0x40040000)
#define USB_OTG_FS_PERIPH_BASE               ((uint32_t )0x50000000)

#define USB_OTG_GLOBAL_BASE                  ((uint32_t )0x000)
#define USB_OTG_DEVICE_BASE                  ((uint32_t )0x800)
#define USB_OTG_IN_ENDPOINT_BASE             ((uint32_t )0x900)
#define USB_OTG_OUT_ENDPOINT_BASE            ((uint32_t )0xB00)
#define USB_OTG_EP_REG_SIZE                  ((uint32_t )0x20)
#define USB_OTG_HOST_BASE                    ((uint32_t )0x400)
#define USB_OTG_HOST_PORT_BASE               ((uint32_t )0x440)
#define USB_OTG_HOST_CHANNEL_BASE            ((uint32_t )0x500)
#define USB_OTG_HOST_CHANNEL_SIZE            ((uint32_t )0x20)
#define USB_OTG_PCGCCTL_BASE                 ((uint32_t )0xE00)
#define USB_OTG_FIFO_BASE                    ((uint32_t )0x1000)
#define USB_OTG_FIFO_SIZE                    ((uint32_t )0x1000)

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
#define  ADC_SR_AWD                          ((uint32_t)0x00000001)        /*!<Analog watchdog flag                                 */
#define  ADC_SR_EOC                          ((uint32_t)0x00000002)        /*!<End of conversion                                    */
#define  ADC_SR_JEOC                         ((uint32_t)0x00000004)        /*!<Injected channel end of conversion                   */
#define  ADC_SR_JSTRT                        ((uint32_t)0x00000008)        /*!<Injected channel Start flag                          */
#define  ADC_SR_STRT                         ((uint32_t)0x00000010)        /*!<Regular channel Start flag                           */
#define  ADC_SR_OVR                          ((uint32_t)0x00000020)        /*!<Overrun flag                                         */

/*******************  Bit definition for ADC_CR1 register  ********************/
#define  ADC_CR1_AWDCH                       ((uint32_t)0x0000001F)        /*!<AWDCH[4:0] bits (Analog watchdog channel select bits) */
#define  ADC_CR1_AWDCH_0                     ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  ADC_CR1_AWDCH_1                     ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  ADC_CR1_AWDCH_2                     ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  ADC_CR1_AWDCH_3                     ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  ADC_CR1_AWDCH_4                     ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  ADC_CR1_EOCIE                       ((uint32_t)0x00000020)        /*!<Interrupt enable for EOC                             */
#define  ADC_CR1_AWDIE                       ((uint32_t)0x00000040)        /*!<AAnalog Watchdog interrupt enable                    */
#define  ADC_CR1_JEOCIE                      ((uint32_t)0x00000080)        /*!<Interrupt enable for injected channels               */
#define  ADC_CR1_SCAN                        ((uint32_t)0x00000100)        /*!<Scan mode */
#define  ADC_CR1_AWDSGL                      ((uint32_t)0x00000200)        /*!<Enable the watchdog on a single channel in scan mode */
#define  ADC_CR1_JAUTO                       ((uint32_t)0x00000400)        /*!<Automatic injected group conversion                  */
#define  ADC_CR1_DISCEN                      ((uint32_t)0x00000800)        /*!<Discontinuous mode on regular channels               */
#define  ADC_CR1_JDISCEN                     ((uint32_t)0x00001000)        /*!<Discontinuous mode on injected channels              */
#define  ADC_CR1_DISCNUM                     ((uint32_t)0x0000E000)        /*!<DISCNUM[2:0] bits (Discontinuous mode channel count) */
#define  ADC_CR1_DISCNUM_0                   ((uint32_t)0x00002000)        /*!<Bit 0 */
#define  ADC_CR1_DISCNUM_1                   ((uint32_t)0x00004000)        /*!<Bit 1 */
#define  ADC_CR1_DISCNUM_2                   ((uint32_t)0x00008000)        /*!<Bit 2 */
#define  ADC_CR1_JAWDEN                      ((uint32_t)0x00400000)        /*!<Analog watchdog enable on injected channels          */
#define  ADC_CR1_AWDEN                       ((uint32_t)0x00800000)        /*!<Analog watchdog enable on regular channels           */
#define  ADC_CR1_RES                         ((uint32_t)0x03000000)        /*!<RES[2:0] bits (Resolution)                           */
#define  ADC_CR1_RES_0                       ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  ADC_CR1_RES_1                       ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  ADC_CR1_OVRIE                       ((uint32_t)0x04000000)         /*!<overrun interrupt enable */

/*******************  Bit definition for ADC_CR2 register  ********************/
#define  ADC_CR2_ADON                        ((uint32_t)0x00000001)        /*!<A/D Converter ON / OFF                                       */
#define  ADC_CR2_CONT                        ((uint32_t)0x00000002)        /*!<Continuous Conversion                                        */
#define  ADC_CR2_DMA                         ((uint32_t)0x00000100)        /*!<Direct Memory access mode                                    */
#define  ADC_CR2_DDS                         ((uint32_t)0x00000200)        /*!<DMA disable selection (Single ADC)                           */
#define  ADC_CR2_EOCS                        ((uint32_t)0x00000400)        /*!<End of conversion selection                                  */
#define  ADC_CR2_ALIGN                       ((uint32_t)0x00000800)        /*!<Data Alignment                                               */
#define  ADC_CR2_JEXTSEL                     ((uint32_t)0x000F0000)        /*!<JEXTSEL[3:0] bits (External event select for injected group) */
#define  ADC_CR2_JEXTSEL_0                   ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  ADC_CR2_JEXTSEL_1                   ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  ADC_CR2_JEXTSEL_2                   ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  ADC_CR2_JEXTSEL_3                   ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  ADC_CR2_JEXTEN                      ((uint32_t)0x00300000)        /*!<JEXTEN[1:0] bits (External Trigger Conversion mode for injected channelsp) */
#define  ADC_CR2_JEXTEN_0                    ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  ADC_CR2_JEXTEN_1                    ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  ADC_CR2_JSWSTART                    ((uint32_t)0x00400000)        /*!<Start Conversion of injected channels */
#define  ADC_CR2_EXTSEL                      ((uint32_t)0x0F000000)        /*!<EXTSEL[3:0] bits (External Event Select for regular group) */
#define  ADC_CR2_EXTSEL_0                    ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  ADC_CR2_EXTSEL_1                    ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  ADC_CR2_EXTSEL_2                    ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  ADC_CR2_EXTSEL_3                    ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  ADC_CR2_EXTEN                       ((uint32_t)0x30000000)        /*!<EXTEN[1:0] bits (External Trigger Conversion mode for regular channelsp) */
#define  ADC_CR2_EXTEN_0                     ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  ADC_CR2_EXTEN_1                     ((uint32_t)0x20000000)        /*!<Bit 1 */
#define  ADC_CR2_SWSTART                     ((uint32_t)0x40000000)        /*!<Start Conversion of regular channels */

/******************  Bit definition for ADC_SMPR1 register  *******************/
#define  ADC_SMPR1_SMP10                     ((uint32_t)0x00000007)        /*!<SMP10[2:0] bits (Channel 10 Sample time selection) */
#define  ADC_SMPR1_SMP10_0                   ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP10_1                   ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP10_2                   ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  ADC_SMPR1_SMP11                     ((uint32_t)0x00000038)        /*!<SMP11[2:0] bits (Channel 11 Sample time selection) */
#define  ADC_SMPR1_SMP11_0                   ((uint32_t)0x00000008)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP11_1                   ((uint32_t)0x00000010)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP11_2                   ((uint32_t)0x00000020)        /*!<Bit 2 */
#define  ADC_SMPR1_SMP12                     ((uint32_t)0x000001C0)        /*!<SMP12[2:0] bits (Channel 12 Sample time selection) */
#define  ADC_SMPR1_SMP12_0                   ((uint32_t)0x00000040)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP12_1                   ((uint32_t)0x00000080)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP12_2                   ((uint32_t)0x00000100)        /*!<Bit 2 */
#define  ADC_SMPR1_SMP13                     ((uint32_t)0x00000E00)        /*!<SMP13[2:0] bits (Channel 13 Sample time selection) */
#define  ADC_SMPR1_SMP13_0                   ((uint32_t)0x00000200)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP13_1                   ((uint32_t)0x00000400)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP13_2                   ((uint32_t)0x00000800)        /*!<Bit 2 */
#define  ADC_SMPR1_SMP14                     ((uint32_t)0x00007000)        /*!<SMP14[2:0] bits (Channel 14 Sample time selection) */
#define  ADC_SMPR1_SMP14_0                   ((uint32_t)0x00001000)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP14_1                   ((uint32_t)0x00002000)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP14_2                   ((uint32_t)0x00004000)        /*!<Bit 2 */
#define  ADC_SMPR1_SMP15                     ((uint32_t)0x00038000)        /*!<SMP15[2:0] bits (Channel 15 Sample time selection) */
#define  ADC_SMPR1_SMP15_0                   ((uint32_t)0x00008000)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP15_1                   ((uint32_t)0x00010000)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP15_2                   ((uint32_t)0x00020000)        /*!<Bit 2 */
#define  ADC_SMPR1_SMP16                     ((uint32_t)0x001C0000)        /*!<SMP16[2:0] bits (Channel 16 Sample time selection) */
#define  ADC_SMPR1_SMP16_0                   ((uint32_t)0x00040000)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP16_1                   ((uint32_t)0x00080000)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP16_2                   ((uint32_t)0x00100000)        /*!<Bit 2 */
#define  ADC_SMPR1_SMP17                     ((uint32_t)0x00E00000)        /*!<SMP17[2:0] bits (Channel 17 Sample time selection) */
#define  ADC_SMPR1_SMP17_0                   ((uint32_t)0x00200000)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP17_1                   ((uint32_t)0x00400000)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP17_2                   ((uint32_t)0x00800000)        /*!<Bit 2 */
#define  ADC_SMPR1_SMP18                     ((uint32_t)0x07000000)        /*!<SMP18[2:0] bits (Channel 18 Sample time selection) */
#define  ADC_SMPR1_SMP18_0                   ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  ADC_SMPR1_SMP18_1                   ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  ADC_SMPR1_SMP18_2                   ((uint32_t)0x04000000)        /*!<Bit 2 */

/******************  Bit definition for ADC_SMPR2 register  *******************/
#define  ADC_SMPR2_SMP0                      ((uint32_t)0x00000007)        /*!<SMP0[2:0] bits (Channel 0 Sample time selection) */
#define  ADC_SMPR2_SMP0_0                    ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP0_1                    ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP0_2                    ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP1                      ((uint32_t)0x00000038)        /*!<SMP1[2:0] bits (Channel 1 Sample time selection) */
#define  ADC_SMPR2_SMP1_0                    ((uint32_t)0x00000008)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP1_1                    ((uint32_t)0x00000010)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP1_2                    ((uint32_t)0x00000020)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP2                      ((uint32_t)0x000001C0)        /*!<SMP2[2:0] bits (Channel 2 Sample time selection) */
#define  ADC_SMPR2_SMP2_0                    ((uint32_t)0x00000040)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP2_1                    ((uint32_t)0x00000080)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP2_2                    ((uint32_t)0x00000100)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP3                      ((uint32_t)0x00000E00)        /*!<SMP3[2:0] bits (Channel 3 Sample time selection) */
#define  ADC_SMPR2_SMP3_0                    ((uint32_t)0x00000200)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP3_1                    ((uint32_t)0x00000400)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP3_2                    ((uint32_t)0x00000800)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP4                      ((uint32_t)0x00007000)        /*!<SMP4[2:0] bits (Channel 4 Sample time selection) */
#define  ADC_SMPR2_SMP4_0                    ((uint32_t)0x00001000)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP4_1                    ((uint32_t)0x00002000)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP4_2                    ((uint32_t)0x00004000)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP5                      ((uint32_t)0x00038000)        /*!<SMP5[2:0] bits (Channel 5 Sample time selection) */
#define  ADC_SMPR2_SMP5_0                    ((uint32_t)0x00008000)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP5_1                    ((uint32_t)0x00010000)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP5_2                    ((uint32_t)0x00020000)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP6                      ((uint32_t)0x001C0000)        /*!<SMP6[2:0] bits (Channel 6 Sample time selection) */
#define  ADC_SMPR2_SMP6_0                    ((uint32_t)0x00040000)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP6_1                    ((uint32_t)0x00080000)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP6_2                    ((uint32_t)0x00100000)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP7                      ((uint32_t)0x00E00000)        /*!<SMP7[2:0] bits (Channel 7 Sample time selection) */
#define  ADC_SMPR2_SMP7_0                    ((uint32_t)0x00200000)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP7_1                    ((uint32_t)0x00400000)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP7_2                    ((uint32_t)0x00800000)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP8                      ((uint32_t)0x07000000)        /*!<SMP8[2:0] bits (Channel 8 Sample time selection) */
#define  ADC_SMPR2_SMP8_0                    ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP8_1                    ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP8_2                    ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  ADC_SMPR2_SMP9                      ((uint32_t)0x38000000)        /*!<SMP9[2:0] bits (Channel 9 Sample time selection) */
#define  ADC_SMPR2_SMP9_0                    ((uint32_t)0x08000000)        /*!<Bit 0 */
#define  ADC_SMPR2_SMP9_1                    ((uint32_t)0x10000000)        /*!<Bit 1 */
#define  ADC_SMPR2_SMP9_2                    ((uint32_t)0x20000000)        /*!<Bit 2 */

/******************  Bit definition for ADC_JOFR1 register  *******************/
#define  ADC_JOFR1_JOFFSET1                  ((uint32_t)0x0FFF)            /*!<Data offset for injected channel 1 */

/******************  Bit definition for ADC_JOFR2 register  *******************/
#define  ADC_JOFR2_JOFFSET2                  ((uint32_t)0x0FFF)            /*!<Data offset for injected channel 2 */

/******************  Bit definition for ADC_JOFR3 register  *******************/
#define  ADC_JOFR3_JOFFSET3                  ((uint32_t)0x0FFF)            /*!<Data offset for injected channel 3 */

/******************  Bit definition for ADC_JOFR4 register  *******************/
#define  ADC_JOFR4_JOFFSET4                  ((uint32_t)0x0FFF)            /*!<Data offset for injected channel 4 */

/*******************  Bit definition for ADC_HTR register  ********************/
#define  ADC_HTR_HT                          ((uint32_t)0x0FFF)            /*!<Analog watchdog high threshold */

/*******************  Bit definition for ADC_LTR register  ********************/
#define  ADC_LTR_LT                          ((uint32_t)0x0FFF)            /*!<Analog watchdog low threshold */

/*******************  Bit definition for ADC_SQR1 register  *******************/
#define  ADC_SQR1_SQ13                       ((uint32_t)0x0000001F)        /*!<SQ13[4:0] bits (13th conversion in regular sequence) */
#define  ADC_SQR1_SQ13_0                     ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  ADC_SQR1_SQ13_1                     ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  ADC_SQR1_SQ13_2                     ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  ADC_SQR1_SQ13_3                     ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  ADC_SQR1_SQ13_4                     ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  ADC_SQR1_SQ14                       ((uint32_t)0x000003E0)        /*!<SQ14[4:0] bits (14th conversion in regular sequence) */
#define  ADC_SQR1_SQ14_0                     ((uint32_t)0x00000020)        /*!<Bit 0 */
#define  ADC_SQR1_SQ14_1                     ((uint32_t)0x00000040)        /*!<Bit 1 */
#define  ADC_SQR1_SQ14_2                     ((uint32_t)0x00000080)        /*!<Bit 2 */
#define  ADC_SQR1_SQ14_3                     ((uint32_t)0x00000100)        /*!<Bit 3 */
#define  ADC_SQR1_SQ14_4                     ((uint32_t)0x00000200)        /*!<Bit 4 */
#define  ADC_SQR1_SQ15                       ((uint32_t)0x00007C00)        /*!<SQ15[4:0] bits (15th conversion in regular sequence) */
#define  ADC_SQR1_SQ15_0                     ((uint32_t)0x00000400)        /*!<Bit 0 */
#define  ADC_SQR1_SQ15_1                     ((uint32_t)0x00000800)        /*!<Bit 1 */
#define  ADC_SQR1_SQ15_2                     ((uint32_t)0x00001000)        /*!<Bit 2 */
#define  ADC_SQR1_SQ15_3                     ((uint32_t)0x00002000)        /*!<Bit 3 */
#define  ADC_SQR1_SQ15_4                     ((uint32_t)0x00004000)        /*!<Bit 4 */
#define  ADC_SQR1_SQ16                       ((uint32_t)0x000F8000)        /*!<SQ16[4:0] bits (16th conversion in regular sequence) */
#define  ADC_SQR1_SQ16_0                     ((uint32_t)0x00008000)        /*!<Bit 0 */
#define  ADC_SQR1_SQ16_1                     ((uint32_t)0x00010000)        /*!<Bit 1 */
#define  ADC_SQR1_SQ16_2                     ((uint32_t)0x00020000)        /*!<Bit 2 */
#define  ADC_SQR1_SQ16_3                     ((uint32_t)0x00040000)        /*!<Bit 3 */
#define  ADC_SQR1_SQ16_4                     ((uint32_t)0x00080000)        /*!<Bit 4 */
#define  ADC_SQR1_L                          ((uint32_t)0x00F00000)        /*!<L[3:0] bits (Regular channel sequence length) */
#define  ADC_SQR1_L_0                        ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  ADC_SQR1_L_1                        ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  ADC_SQR1_L_2                        ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  ADC_SQR1_L_3                        ((uint32_t)0x00800000)        /*!<Bit 3 */

/*******************  Bit definition for ADC_SQR2 register  *******************/
#define  ADC_SQR2_SQ7                        ((uint32_t)0x0000001F)        /*!<SQ7[4:0] bits (7th conversion in regular sequence) */
#define  ADC_SQR2_SQ7_0                      ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  ADC_SQR2_SQ7_1                      ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  ADC_SQR2_SQ7_2                      ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  ADC_SQR2_SQ7_3                      ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  ADC_SQR2_SQ7_4                      ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  ADC_SQR2_SQ8                        ((uint32_t)0x000003E0)        /*!<SQ8[4:0] bits (8th conversion in regular sequence) */
#define  ADC_SQR2_SQ8_0                      ((uint32_t)0x00000020)        /*!<Bit 0 */
#define  ADC_SQR2_SQ8_1                      ((uint32_t)0x00000040)        /*!<Bit 1 */
#define  ADC_SQR2_SQ8_2                      ((uint32_t)0x00000080)        /*!<Bit 2 */
#define  ADC_SQR2_SQ8_3                      ((uint32_t)0x00000100)        /*!<Bit 3 */
#define  ADC_SQR2_SQ8_4                      ((uint32_t)0x00000200)        /*!<Bit 4 */
#define  ADC_SQR2_SQ9                        ((uint32_t)0x00007C00)        /*!<SQ9[4:0] bits (9th conversion in regular sequence) */
#define  ADC_SQR2_SQ9_0                      ((uint32_t)0x00000400)        /*!<Bit 0 */
#define  ADC_SQR2_SQ9_1                      ((uint32_t)0x00000800)        /*!<Bit 1 */
#define  ADC_SQR2_SQ9_2                      ((uint32_t)0x00001000)        /*!<Bit 2 */
#define  ADC_SQR2_SQ9_3                      ((uint32_t)0x00002000)        /*!<Bit 3 */
#define  ADC_SQR2_SQ9_4                      ((uint32_t)0x00004000)        /*!<Bit 4 */
#define  ADC_SQR2_SQ10                       ((uint32_t)0x000F8000)        /*!<SQ10[4:0] bits (10th conversion in regular sequence) */
#define  ADC_SQR2_SQ10_0                     ((uint32_t)0x00008000)        /*!<Bit 0 */
#define  ADC_SQR2_SQ10_1                     ((uint32_t)0x00010000)        /*!<Bit 1 */
#define  ADC_SQR2_SQ10_2                     ((uint32_t)0x00020000)        /*!<Bit 2 */
#define  ADC_SQR2_SQ10_3                     ((uint32_t)0x00040000)        /*!<Bit 3 */
#define  ADC_SQR2_SQ10_4                     ((uint32_t)0x00080000)        /*!<Bit 4 */
#define  ADC_SQR2_SQ11                       ((uint32_t)0x01F00000)        /*!<SQ11[4:0] bits (11th conversion in regular sequence) */
#define  ADC_SQR2_SQ11_0                     ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  ADC_SQR2_SQ11_1                     ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  ADC_SQR2_SQ11_2                     ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  ADC_SQR2_SQ11_3                     ((uint32_t)0x00800000)        /*!<Bit 3 */
#define  ADC_SQR2_SQ11_4                     ((uint32_t)0x01000000)        /*!<Bit 4 */
#define  ADC_SQR2_SQ12                       ((uint32_t)0x3E000000)        /*!<SQ12[4:0] bits (12th conversion in regular sequence) */
#define  ADC_SQR2_SQ12_0                     ((uint32_t)0x02000000)        /*!<Bit 0 */
#define  ADC_SQR2_SQ12_1                     ((uint32_t)0x04000000)        /*!<Bit 1 */
#define  ADC_SQR2_SQ12_2                     ((uint32_t)0x08000000)        /*!<Bit 2 */
#define  ADC_SQR2_SQ12_3                     ((uint32_t)0x10000000)        /*!<Bit 3 */
#define  ADC_SQR2_SQ12_4                     ((uint32_t)0x20000000)        /*!<Bit 4 */

/*******************  Bit definition for ADC_SQR3 register  *******************/
#define  ADC_SQR3_SQ1                        ((uint32_t)0x0000001F)        /*!<SQ1[4:0] bits (1st conversion in regular sequence) */
#define  ADC_SQR3_SQ1_0                      ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  ADC_SQR3_SQ1_1                      ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  ADC_SQR3_SQ1_2                      ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  ADC_SQR3_SQ1_3                      ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  ADC_SQR3_SQ1_4                      ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  ADC_SQR3_SQ2                        ((uint32_t)0x000003E0)        /*!<SQ2[4:0] bits (2nd conversion in regular sequence) */
#define  ADC_SQR3_SQ2_0                      ((uint32_t)0x00000020)        /*!<Bit 0 */
#define  ADC_SQR3_SQ2_1                      ((uint32_t)0x00000040)        /*!<Bit 1 */
#define  ADC_SQR3_SQ2_2                      ((uint32_t)0x00000080)        /*!<Bit 2 */
#define  ADC_SQR3_SQ2_3                      ((uint32_t)0x00000100)        /*!<Bit 3 */
#define  ADC_SQR3_SQ2_4                      ((uint32_t)0x00000200)        /*!<Bit 4 */
#define  ADC_SQR3_SQ3                        ((uint32_t)0x00007C00)        /*!<SQ3[4:0] bits (3rd conversion in regular sequence) */
#define  ADC_SQR3_SQ3_0                      ((uint32_t)0x00000400)        /*!<Bit 0 */
#define  ADC_SQR3_SQ3_1                      ((uint32_t)0x00000800)        /*!<Bit 1 */
#define  ADC_SQR3_SQ3_2                      ((uint32_t)0x00001000)        /*!<Bit 2 */
#define  ADC_SQR3_SQ3_3                      ((uint32_t)0x00002000)        /*!<Bit 3 */
#define  ADC_SQR3_SQ3_4                      ((uint32_t)0x00004000)        /*!<Bit 4 */
#define  ADC_SQR3_SQ4                        ((uint32_t)0x000F8000)        /*!<SQ4[4:0] bits (4th conversion in regular sequence) */
#define  ADC_SQR3_SQ4_0                      ((uint32_t)0x00008000)        /*!<Bit 0 */
#define  ADC_SQR3_SQ4_1                      ((uint32_t)0x00010000)        /*!<Bit 1 */
#define  ADC_SQR3_SQ4_2                      ((uint32_t)0x00020000)        /*!<Bit 2 */
#define  ADC_SQR3_SQ4_3                      ((uint32_t)0x00040000)        /*!<Bit 3 */
#define  ADC_SQR3_SQ4_4                      ((uint32_t)0x00080000)        /*!<Bit 4 */
#define  ADC_SQR3_SQ5                        ((uint32_t)0x01F00000)        /*!<SQ5[4:0] bits (5th conversion in regular sequence) */
#define  ADC_SQR3_SQ5_0                      ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  ADC_SQR3_SQ5_1                      ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  ADC_SQR3_SQ5_2                      ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  ADC_SQR3_SQ5_3                      ((uint32_t)0x00800000)        /*!<Bit 3 */
#define  ADC_SQR3_SQ5_4                      ((uint32_t)0x01000000)        /*!<Bit 4 */
#define  ADC_SQR3_SQ6                        ((uint32_t)0x3E000000)        /*!<SQ6[4:0] bits (6th conversion in regular sequence) */
#define  ADC_SQR3_SQ6_0                      ((uint32_t)0x02000000)        /*!<Bit 0 */
#define  ADC_SQR3_SQ6_1                      ((uint32_t)0x04000000)        /*!<Bit 1 */
#define  ADC_SQR3_SQ6_2                      ((uint32_t)0x08000000)        /*!<Bit 2 */
#define  ADC_SQR3_SQ6_3                      ((uint32_t)0x10000000)        /*!<Bit 3 */
#define  ADC_SQR3_SQ6_4                      ((uint32_t)0x20000000)        /*!<Bit 4 */

/*******************  Bit definition for ADC_JSQR register  *******************/
#define  ADC_JSQR_JSQ1                       ((uint32_t)0x0000001F)        /*!<JSQ1[4:0] bits (1st conversion in injected sequence) */
#define  ADC_JSQR_JSQ1_0                     ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  ADC_JSQR_JSQ1_1                     ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  ADC_JSQR_JSQ1_2                     ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  ADC_JSQR_JSQ1_3                     ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  ADC_JSQR_JSQ1_4                     ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  ADC_JSQR_JSQ2                       ((uint32_t)0x000003E0)        /*!<JSQ2[4:0] bits (2nd conversion in injected sequence) */
#define  ADC_JSQR_JSQ2_0                     ((uint32_t)0x00000020)        /*!<Bit 0 */
#define  ADC_JSQR_JSQ2_1                     ((uint32_t)0x00000040)        /*!<Bit 1 */
#define  ADC_JSQR_JSQ2_2                     ((uint32_t)0x00000080)        /*!<Bit 2 */
#define  ADC_JSQR_JSQ2_3                     ((uint32_t)0x00000100)        /*!<Bit 3 */
#define  ADC_JSQR_JSQ2_4                     ((uint32_t)0x00000200)        /*!<Bit 4 */
#define  ADC_JSQR_JSQ3                       ((uint32_t)0x00007C00)        /*!<JSQ3[4:0] bits (3rd conversion in injected sequence) */
#define  ADC_JSQR_JSQ3_0                     ((uint32_t)0x00000400)        /*!<Bit 0 */
#define  ADC_JSQR_JSQ3_1                     ((uint32_t)0x00000800)        /*!<Bit 1 */
#define  ADC_JSQR_JSQ3_2                     ((uint32_t)0x00001000)        /*!<Bit 2 */
#define  ADC_JSQR_JSQ3_3                     ((uint32_t)0x00002000)        /*!<Bit 3 */
#define  ADC_JSQR_JSQ3_4                     ((uint32_t)0x00004000)        /*!<Bit 4 */
#define  ADC_JSQR_JSQ4                       ((uint32_t)0x000F8000)        /*!<JSQ4[4:0] bits (4th conversion in injected sequence) */
#define  ADC_JSQR_JSQ4_0                     ((uint32_t)0x00008000)        /*!<Bit 0 */
#define  ADC_JSQR_JSQ4_1                     ((uint32_t)0x00010000)        /*!<Bit 1 */
#define  ADC_JSQR_JSQ4_2                     ((uint32_t)0x00020000)        /*!<Bit 2 */
#define  ADC_JSQR_JSQ4_3                     ((uint32_t)0x00040000)        /*!<Bit 3 */
#define  ADC_JSQR_JSQ4_4                     ((uint32_t)0x00080000)        /*!<Bit 4 */
#define  ADC_JSQR_JL                         ((uint32_t)0x00300000)        /*!<JL[1:0] bits (Injected Sequence length) */
#define  ADC_JSQR_JL_0                       ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  ADC_JSQR_JL_1                       ((uint32_t)0x00200000)        /*!<Bit 1 */

/*******************  Bit definition for ADC_JDR1 register  *******************/
#define  ADC_JDR1_JDATA                      ((uint16_t)0xFFFF)            /*!<Injected data */

/*******************  Bit definition for ADC_JDR2 register  *******************/
#define  ADC_JDR2_JDATA                      ((uint16_t)0xFFFF)            /*!<Injected data */

/*******************  Bit definition for ADC_JDR3 register  *******************/
#define  ADC_JDR3_JDATA                      ((uint16_t)0xFFFF)            /*!<Injected data */

/*******************  Bit definition for ADC_JDR4 register  *******************/
#define  ADC_JDR4_JDATA                      ((uint16_t)0xFFFF)            /*!<Injected data */

/********************  Bit definition for ADC_DR register  ********************/
#define  ADC_DR_DATA                         ((uint32_t)0x0000FFFF)        /*!<Regular data */
#define  ADC_DR_ADC2DATA                     ((uint32_t)0xFFFF0000)        /*!<ADC2 data */

/*******************  Bit definition for ADC_CSR register  ********************/
#define  ADC_CSR_AWD1                        ((uint32_t)0x00000001)        /*!<ADC1 Analog watchdog flag               */
#define  ADC_CSR_EOC1                        ((uint32_t)0x00000002)        /*!<ADC1 End of conversion                  */
#define  ADC_CSR_JEOC1                       ((uint32_t)0x00000004)        /*!<ADC1 Injected channel end of conversion */
#define  ADC_CSR_JSTRT1                      ((uint32_t)0x00000008)        /*!<ADC1 Injected channel Start flag        */
#define  ADC_CSR_STRT1                       ((uint32_t)0x00000010)        /*!<ADC1 Regular channel Start flag         */
#define  ADC_CSR_DOVR1                       ((uint32_t)0x00000020)        /*!<ADC1 DMA overrun  flag                  */
#define  ADC_CSR_AWD2                        ((uint32_t)0x00000100)        /*!<ADC2 Analog watchdog flag               */
#define  ADC_CSR_EOC2                        ((uint32_t)0x00000200)        /*!<ADC2 End of conversion                  */
#define  ADC_CSR_JEOC2                       ((uint32_t)0x00000400)        /*!<ADC2 Injected channel end of conversion */
#define  ADC_CSR_JSTRT2                      ((uint32_t)0x00000800)        /*!<ADC2 Injected channel Start flag        */
#define  ADC_CSR_STRT2                       ((uint32_t)0x00001000)        /*!<ADC2 Regular channel Start flag         */
#define  ADC_CSR_DOVR2                       ((uint32_t)0x00002000)        /*!<ADC2 DMA overrun  flag                  */
#define  ADC_CSR_AWD3                        ((uint32_t)0x00010000)        /*!<ADC3 Analog watchdog flag               */
#define  ADC_CSR_EOC3                        ((uint32_t)0x00020000)        /*!<ADC3 End of conversion                  */
#define  ADC_CSR_JEOC3                       ((uint32_t)0x00040000)        /*!<ADC3 Injected channel end of conversion */
#define  ADC_CSR_JSTRT3                      ((uint32_t)0x00080000)        /*!<ADC3 Injected channel Start flag        */
#define  ADC_CSR_STRT3                       ((uint32_t)0x00100000)        /*!<ADC3 Regular channel Start flag         */
#define  ADC_CSR_DOVR3                       ((uint32_t)0x00200000)        /*!<ADC3 DMA overrun  flag                  */

/*******************  Bit definition for ADC_CCR register  ********************/
#define  ADC_CCR_MULTI                       ((uint32_t)0x0000001F)        /*!<MULTI[4:0] bits (Multi-ADC mode selection) */
#define  ADC_CCR_MULTI_0                     ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  ADC_CCR_MULTI_1                     ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  ADC_CCR_MULTI_2                     ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  ADC_CCR_MULTI_3                     ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  ADC_CCR_MULTI_4                     ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  ADC_CCR_DELAY                       ((uint32_t)0x00000F00)        /*!<DELAY[3:0] bits (Delay between 2 sampling phases) */
#define  ADC_CCR_DELAY_0                     ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  ADC_CCR_DELAY_1                     ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  ADC_CCR_DELAY_2                     ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  ADC_CCR_DELAY_3                     ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  ADC_CCR_DDS                         ((uint32_t)0x00002000)        /*!<DMA disable selection (Multi-ADC mode) */
#define  ADC_CCR_DMA                         ((uint32_t)0x0000C000)        /*!<DMA[1:0] bits (Direct Memory Access mode for multimode) */
#define  ADC_CCR_DMA_0                       ((uint32_t)0x00004000)        /*!<Bit 0 */
#define  ADC_CCR_DMA_1                       ((uint32_t)0x00008000)        /*!<Bit 1 */
#define  ADC_CCR_ADCPRE                      ((uint32_t)0x00030000)        /*!<ADCPRE[1:0] bits (ADC prescaler) */
#define  ADC_CCR_ADCPRE_0                    ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  ADC_CCR_ADCPRE_1                    ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  ADC_CCR_VBATE                       ((uint32_t)0x00400000)        /*!<VBAT Enable */
#define  ADC_CCR_TSVREFE                     ((uint32_t)0x00800000)        /*!<Temperature Sensor and VREFINT Enable */

/*******************  Bit definition for ADC_CDR register  ********************/
#define  ADC_CDR_DATA1                      ((uint32_t)0x0000FFFF)         /*!<1st data of a pair of regular conversions */
#define  ADC_CDR_DATA2                      ((uint32_t)0xFFFF0000)         /*!<2nd data of a pair of regular conversions */

/******************************************************************************/
/*                                                                            */
/*                         Controller Area Network                            */
/*                                                                            */
/******************************************************************************/
/*!<CAN control and status registers */
/*******************  Bit definition for CAN_MCR register  ********************/
#define  CAN_MCR_INRQ                        ((uint32_t)0x00000001)        /*!<Initialization Request            */
#define  CAN_MCR_SLEEP                       ((uint32_t)0x00000002)        /*!<Sleep Mode Request                */
#define  CAN_MCR_TXFP                        ((uint32_t)0x00000004)        /*!<Transmit FIFO Priority            */
#define  CAN_MCR_RFLM                        ((uint32_t)0x00000008)        /*!<Receive FIFO Locked Mode          */
#define  CAN_MCR_NART                        ((uint32_t)0x00000010)        /*!<No Automatic Retransmission       */
#define  CAN_MCR_AWUM                        ((uint32_t)0x00000020)        /*!<Automatic Wakeup Mode             */
#define  CAN_MCR_ABOM                        ((uint32_t)0x00000040)        /*!<Automatic Bus-Off Management      */
#define  CAN_MCR_TTCM                        ((uint32_t)0x00000080)        /*!<Time Triggered Communication Mode */
#define  CAN_MCR_RESET                       ((uint32_t)0x00008000)        /*!<bxCAN software master reset       */

/*******************  Bit definition for CAN_MSR register  ********************/
#define  CAN_MSR_INAK                        ((uint32_t)0x00000001)        /*!<Initialization Acknowledge  */
#define  CAN_MSR_SLAK                        ((uint32_t)0x00000002)        /*!<Sleep Acknowledge           */
#define  CAN_MSR_ERRI                        ((uint32_t)0x00000004)        /*!<Error Interrupt             */
#define  CAN_MSR_WKUI                        ((uint32_t)0x00000008)        /*!<Wakeup Interrupt            */
#define  CAN_MSR_SLAKI                       ((uint32_t)0x00000010)        /*!<Sleep Acknowledge Interrupt */
#define  CAN_MSR_TXM                         ((uint32_t)0x00000100)        /*!<Transmit Mode               */
#define  CAN_MSR_RXM                         ((uint32_t)0x00000200)        /*!<Receive Mode                */
#define  CAN_MSR_SAMP                        ((uint32_t)0x00000400)        /*!<Last Sample Point           */
#define  CAN_MSR_RX                          ((uint32_t)0x00000800)        /*!<CAN Rx Signal               */

/*******************  Bit definition for CAN_TSR register  ********************/
#define  CAN_TSR_RQCP0                       ((uint32_t)0x00000001)        /*!<Request Completed Mailbox0      */
#define  CAN_TSR_TXOK0                       ((uint32_t)0x00000002)        /*!<Transmission OK of Mailbox0     */
#define  CAN_TSR_ALST0                       ((uint32_t)0x00000004)        /*!<Arbitration Lost for Mailbox0   */
#define  CAN_TSR_TERR0                       ((uint32_t)0x00000008)        /*!<Transmission Error of Mailbox0  */
#define  CAN_TSR_ABRQ0                       ((uint32_t)0x00000080)        /*!<Abort Request for Mailbox0      */
#define  CAN_TSR_RQCP1                       ((uint32_t)0x00000100)        /*!<Request Completed Mailbox1      */
#define  CAN_TSR_TXOK1                       ((uint32_t)0x00000200)        /*!<Transmission OK of Mailbox1     */
#define  CAN_TSR_ALST1                       ((uint32_t)0x00000400)        /*!<Arbitration Lost for Mailbox1   */
#define  CAN_TSR_TERR1                       ((uint32_t)0x00000800)        /*!<Transmission Error of Mailbox1  */
#define  CAN_TSR_ABRQ1                       ((uint32_t)0x00008000)        /*!<Abort Request for Mailbox 1     */
#define  CAN_TSR_RQCP2                       ((uint32_t)0x00010000)        /*!<Request Completed Mailbox2      */
#define  CAN_TSR_TXOK2                       ((uint32_t)0x00020000)        /*!<Transmission OK of Mailbox 2    */
#define  CAN_TSR_ALST2                       ((uint32_t)0x00040000)        /*!<Arbitration Lost for mailbox 2  */
#define  CAN_TSR_TERR2                       ((uint32_t)0x00080000)        /*!<Transmission Error of Mailbox 2 */
#define  CAN_TSR_ABRQ2                       ((uint32_t)0x00800000)        /*!<Abort Request for Mailbox 2     */
#define  CAN_TSR_CODE                        ((uint32_t)0x03000000)        /*!<Mailbox Code                    */

#define  CAN_TSR_TME                         ((uint32_t)0x1C000000)        /*!<TME[2:0] bits */
#define  CAN_TSR_TME0                        ((uint32_t)0x04000000)        /*!<Transmit Mailbox 0 Empty */
#define  CAN_TSR_TME1                        ((uint32_t)0x08000000)        /*!<Transmit Mailbox 1 Empty */
#define  CAN_TSR_TME2                        ((uint32_t)0x10000000)        /*!<Transmit Mailbox 2 Empty */

#define  CAN_TSR_LOW                         ((uint32_t)0xE0000000)        /*!<LOW[2:0] bits */
#define  CAN_TSR_LOW0                        ((uint32_t)0x20000000)        /*!<Lowest Priority Flag for Mailbox 0 */
#define  CAN_TSR_LOW1                        ((uint32_t)0x40000000)        /*!<Lowest Priority Flag for Mailbox 1 */
#define  CAN_TSR_LOW2                        ((uint32_t)0x80000000)        /*!<Lowest Priority Flag for Mailbox 2 */

/*******************  Bit definition for CAN_RF0R register  *******************/
#define  CAN_RF0R_FMP0                       ((uint32_t)0x00000003)        /*!<FIFO 0 Message Pending        */
#define  CAN_RF0R_FULL0                      ((uint32_t)0x00000008)        /*!<FIFO 0 Full                   */
#define  CAN_RF0R_FOVR0                      ((uint32_t)0x00000010)        /*!<FIFO 0 Overrun                */
#define  CAN_RF0R_RFOM0                      ((uint32_t)0x00000020)        /*!<Release FIFO 0 Output Mailbox */

/*******************  Bit definition for CAN_RF1R register  *******************/
#define  CAN_RF1R_FMP1                       ((uint32_t)0x00000003)        /*!<FIFO 1 Message Pending        */
#define  CAN_RF1R_FULL1                      ((uint32_t)0x00000008)        /*!<FIFO 1 Full                   */
#define  CAN_RF1R_FOVR1                      ((uint32_t)0x00000010)        /*!<FIFO 1 Overrun                */
#define  CAN_RF1R_RFOM1                      ((uint32_t)0x00000020)        /*!<Release FIFO 1 Output Mailbox */

/********************  Bit definition for CAN_IER register  *******************/
#define  CAN_IER_TMEIE                       ((uint32_t)0x00000001)        /*!<Transmit Mailbox Empty Interrupt Enable */
#define  CAN_IER_FMPIE0                      ((uint32_t)0x00000002)        /*!<FIFO Message Pending Interrupt Enable   */
#define  CAN_IER_FFIE0                       ((uint32_t)0x00000004)        /*!<FIFO Full Interrupt Enable              */
#define  CAN_IER_FOVIE0                      ((uint32_t)0x00000008)        /*!<FIFO Overrun Interrupt Enable           */
#define  CAN_IER_FMPIE1                      ((uint32_t)0x00000010)        /*!<FIFO Message Pending Interrupt Enable   */
#define  CAN_IER_FFIE1                       ((uint32_t)0x00000020)        /*!<FIFO Full Interrupt Enable              */
#define  CAN_IER_FOVIE1                      ((uint32_t)0x00000040)        /*!<FIFO Overrun Interrupt Enable           */
#define  CAN_IER_EWGIE                       ((uint32_t)0x00000100)        /*!<Error Warning Interrupt Enable          */
#define  CAN_IER_EPVIE                       ((uint32_t)0x00000200)        /*!<Error Passive Interrupt Enable          */
#define  CAN_IER_BOFIE                       ((uint32_t)0x00000400)        /*!<Bus-Off Interrupt Enable                */
#define  CAN_IER_LECIE                       ((uint32_t)0x00000800)        /*!<Last Error Code Interrupt Enable        */
#define  CAN_IER_ERRIE                       ((uint32_t)0x00008000)        /*!<Error Interrupt Enable                  */
#define  CAN_IER_WKUIE                       ((uint32_t)0x00010000)        /*!<Wakeup Interrupt Enable                 */
#define  CAN_IER_SLKIE                       ((uint32_t)0x00020000)        /*!<Sleep Interrupt Enable                  */

/********************  Bit definition for CAN_ESR register  *******************/
#define  CAN_ESR_EWGF                        ((uint32_t)0x00000001)        /*!<Error Warning Flag */
#define  CAN_ESR_EPVF                        ((uint32_t)0x00000002)        /*!<Error Passive Flag */
#define  CAN_ESR_BOFF                        ((uint32_t)0x00000004)        /*!<Bus-Off Flag */

#define  CAN_ESR_LEC                         ((uint32_t)0x00000070)        /*!<LEC[2:0] bits (Last Error Code) */
#define  CAN_ESR_LEC_0                       ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  CAN_ESR_LEC_1                       ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  CAN_ESR_LEC_2                       ((uint32_t)0x00000040)        /*!<Bit 2 */

#define  CAN_ESR_TEC                         ((uint32_t)0x00FF0000)        /*!<Least significant byte of the 9-bit Transmit Error Counter */
#define  CAN_ESR_REC                         ((uint32_t)0xFF000000)        /*!<Receive Error Counter */

/*******************  Bit definition for CAN_BTR register  ********************/
#define  CAN_BTR_BRP                         ((uint32_t)0x000003FF)        /*!<Baud Rate Prescaler           */
#define  CAN_BTR_TS1                         ((uint32_t)0x000F0000)        /*!<Time Segment 1                */
#define  CAN_BTR_TS1_0                       ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  CAN_BTR_TS1_1                       ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  CAN_BTR_TS1_2                       ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  CAN_BTR_TS1_3                       ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  CAN_BTR_TS2                         ((uint32_t)0x00700000)        /*!<Time Segment 2                */
#define  CAN_BTR_TS2_0                       ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  CAN_BTR_TS2_1                       ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  CAN_BTR_TS2_2                       ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  CAN_BTR_SJW                         ((uint32_t)0x03000000)        /*!<Resynchronization Jump Width  */
#define  CAN_BTR_SJW_0                       ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  CAN_BTR_SJW_1                       ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  CAN_BTR_LBKM                        ((uint32_t)0x40000000)        /*!<Loop Back Mode (Debug)        */
#define  CAN_BTR_SILM                        ((uint32_t)0x80000000)        /*!<Silent Mode                   */

/*!<Mailbox registers */
/******************  Bit definition for CAN_TI0R register  ********************/
#define  CAN_TI0R_TXRQ                       ((uint32_t)0x00000001)        /*!<Transmit Mailbox Request                   */
#define  CAN_TI0R_RTR                        ((uint32_t)0x00000002)        /*!<Remote Transmission Request                */
#define  CAN_TI0R_IDE                        ((uint32_t)0x00000004)        /*!<Identifier Extension                       */
#define  CAN_TI0R_EXID                       ((uint32_t)0x001FFFF8)        /*!<Extended Identifier                        */
#define  CAN_TI0R_STID                       ((uint32_t)0xFFE00000)        /*!<Standard Identifier or Extended Identifier */

/******************  Bit definition for CAN_TDT0R register  *******************/
#define  CAN_TDT0R_DLC                       ((uint32_t)0x0000000F)        /*!<Data Length Code     */
#define  CAN_TDT0R_TGT                       ((uint32_t)0x00000100)        /*!<Transmit Global Time */
#define  CAN_TDT0R_TIME                      ((uint32_t)0xFFFF0000)        /*!<Message Time Stamp   */

/******************  Bit definition for CAN_TDL0R register  *******************/
#define  CAN_TDL0R_DATA0                     ((uint32_t)0x000000FF)        /*!<Data byte 0 */
#define  CAN_TDL0R_DATA1                     ((uint32_t)0x0000FF00)        /*!<Data byte 1 */
#define  CAN_TDL0R_DATA2                     ((uint32_t)0x00FF0000)        /*!<Data byte 2 */
#define  CAN_TDL0R_DATA3                     ((uint32_t)0xFF000000)        /*!<Data byte 3 */

/******************  Bit definition for CAN_TDH0R register  *******************/
#define  CAN_TDH0R_DATA4                     ((uint32_t)0x000000FF)        /*!<Data byte 4 */
#define  CAN_TDH0R_DATA5                     ((uint32_t)0x0000FF00)        /*!<Data byte 5 */
#define  CAN_TDH0R_DATA6                     ((uint32_t)0x00FF0000)        /*!<Data byte 6 */
#define  CAN_TDH0R_DATA7                     ((uint32_t)0xFF000000)        /*!<Data byte 7 */

/*******************  Bit definition for CAN_TI1R register  *******************/
#define  CAN_TI1R_TXRQ                       ((uint32_t)0x00000001)        /*!<Transmit Mailbox Request                   */
#define  CAN_TI1R_RTR                        ((uint32_t)0x00000002)        /*!<Remote Transmission Request                */
#define  CAN_TI1R_IDE                        ((uint32_t)0x00000004)        /*!<Identifier Extension                       */
#define  CAN_TI1R_EXID                       ((uint32_t)0x001FFFF8)        /*!<Extended Identifier                        */
#define  CAN_TI1R_STID                       ((uint32_t)0xFFE00000)        /*!<Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_TDT1R register  ******************/
#define  CAN_TDT1R_DLC                       ((uint32_t)0x0000000F)        /*!<Data Length Code     */
#define  CAN_TDT1R_TGT                       ((uint32_t)0x00000100)        /*!<Transmit Global Time */
#define  CAN_TDT1R_TIME                      ((uint32_t)0xFFFF0000)        /*!<Message Time Stamp   */

/*******************  Bit definition for CAN_TDL1R register  ******************/
#define  CAN_TDL1R_DATA0                     ((uint32_t)0x000000FF)        /*!<Data byte 0 */
#define  CAN_TDL1R_DATA1                     ((uint32_t)0x0000FF00)        /*!<Data byte 1 */
#define  CAN_TDL1R_DATA2                     ((uint32_t)0x00FF0000)        /*!<Data byte 2 */
#define  CAN_TDL1R_DATA3                     ((uint32_t)0xFF000000)        /*!<Data byte 3 */

/*******************  Bit definition for CAN_TDH1R register  ******************/
#define  CAN_TDH1R_DATA4                     ((uint32_t)0x000000FF)        /*!<Data byte 4 */
#define  CAN_TDH1R_DATA5                     ((uint32_t)0x0000FF00)        /*!<Data byte 5 */
#define  CAN_TDH1R_DATA6                     ((uint32_t)0x00FF0000)        /*!<Data byte 6 */
#define  CAN_TDH1R_DATA7                     ((uint32_t)0xFF000000)        /*!<Data byte 7 */

/*******************  Bit definition for CAN_TI2R register  *******************/
#define  CAN_TI2R_TXRQ                       ((uint32_t)0x00000001)        /*!<Transmit Mailbox Request                   */
#define  CAN_TI2R_RTR                        ((uint32_t)0x00000002)        /*!<Remote Transmission Request                */
#define  CAN_TI2R_IDE                        ((uint32_t)0x00000004)        /*!<Identifier Extension                       */
#define  CAN_TI2R_EXID                       ((uint32_t)0x001FFFF8)        /*!<Extended identifier                        */
#define  CAN_TI2R_STID                       ((uint32_t)0xFFE00000)        /*!<Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_TDT2R register  ******************/
#define  CAN_TDT2R_DLC                       ((uint32_t)0x0000000F)        /*!<Data Length Code      */
#define  CAN_TDT2R_TGT                       ((uint32_t)0x00000100)        /*!<Transmit Global Time  */
#define  CAN_TDT2R_TIME                      ((uint32_t)0xFFFF0000)        /*!<Message Time Stamp    */

/*******************  Bit definition for CAN_TDL2R register  ******************/
#define  CAN_TDL2R_DATA0                     ((uint32_t)0x000000FF)        /*!<Data byte 0 */
#define  CAN_TDL2R_DATA1                     ((uint32_t)0x0000FF00)        /*!<Data byte 1 */
#define  CAN_TDL2R_DATA2                     ((uint32_t)0x00FF0000)        /*!<Data byte 2 */
#define  CAN_TDL2R_DATA3                     ((uint32_t)0xFF000000)        /*!<Data byte 3 */

/*******************  Bit definition for CAN_TDH2R register  ******************/
#define  CAN_TDH2R_DATA4                     ((uint32_t)0x000000FF)        /*!<Data byte 4 */
#define  CAN_TDH2R_DATA5                     ((uint32_t)0x0000FF00)        /*!<Data byte 5 */
#define  CAN_TDH2R_DATA6                     ((uint32_t)0x00FF0000)        /*!<Data byte 6 */
#define  CAN_TDH2R_DATA7                     ((uint32_t)0xFF000000)        /*!<Data byte 7 */

/*******************  Bit definition for CAN_RI0R register  *******************/
#define  CAN_RI0R_RTR                        ((uint32_t)0x00000002)        /*!<Remote Transmission Request                */
#define  CAN_RI0R_IDE                        ((uint32_t)0x00000004)        /*!<Identifier Extension                       */
#define  CAN_RI0R_EXID                       ((uint32_t)0x001FFFF8)        /*!<Extended Identifier                        */
#define  CAN_RI0R_STID                       ((uint32_t)0xFFE00000)        /*!<Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_RDT0R register  ******************/
#define  CAN_RDT0R_DLC                       ((uint32_t)0x0000000F)        /*!<Data Length Code */
#define  CAN_RDT0R_FMI                       ((uint32_t)0x0000FF00)        /*!<Filter Match Index */
#define  CAN_RDT0R_TIME                      ((uint32_t)0xFFFF0000)        /*!<Message Time Stamp */

/*******************  Bit definition for CAN_RDL0R register  ******************/
#define  CAN_RDL0R_DATA0                     ((uint32_t)0x000000FF)        /*!<Data byte 0 */
#define  CAN_RDL0R_DATA1                     ((uint32_t)0x0000FF00)        /*!<Data byte 1 */
#define  CAN_RDL0R_DATA2                     ((uint32_t)0x00FF0000)        /*!<Data byte 2 */
#define  CAN_RDL0R_DATA3                     ((uint32_t)0xFF000000)        /*!<Data byte 3 */

/*******************  Bit definition for CAN_RDH0R register  ******************/
#define  CAN_RDH0R_DATA4                     ((uint32_t)0x000000FF)        /*!<Data byte 4 */
#define  CAN_RDH0R_DATA5                     ((uint32_t)0x0000FF00)        /*!<Data byte 5 */
#define  CAN_RDH0R_DATA6                     ((uint32_t)0x00FF0000)        /*!<Data byte 6 */
#define  CAN_RDH0R_DATA7                     ((uint32_t)0xFF000000)        /*!<Data byte 7 */

/*******************  Bit definition for CAN_RI1R register  *******************/
#define  CAN_RI1R_RTR                        ((uint32_t)0x00000002)        /*!<Remote Transmission Request                */
#define  CAN_RI1R_IDE                        ((uint32_t)0x00000004)        /*!<Identifier Extension                       */
#define  CAN_RI1R_EXID                       ((uint32_t)0x001FFFF8)        /*!<Extended identifier                        */
#define  CAN_RI1R_STID                       ((uint32_t)0xFFE00000)        /*!<Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_RDT1R register  ******************/
#define  CAN_RDT1R_DLC                       ((uint32_t)0x0000000F)        /*!<Data Length Code   */
#define  CAN_RDT1R_FMI                       ((uint32_t)0x0000FF00)        /*!<Filter Match Index */
#define  CAN_RDT1R_TIME                      ((uint32_t)0xFFFF0000)        /*!<Message Time Stamp */

/*******************  Bit definition for CAN_RDL1R register  ******************/
#define  CAN_RDL1R_DATA0                     ((uint32_t)0x000000FF)        /*!<Data byte 0 */
#define  CAN_RDL1R_DATA1                     ((uint32_t)0x0000FF00)        /*!<Data byte 1 */
#define  CAN_RDL1R_DATA2                     ((uint32_t)0x00FF0000)        /*!<Data byte 2 */
#define  CAN_RDL1R_DATA3                     ((uint32_t)0xFF000000)        /*!<Data byte 3 */

/*******************  Bit definition for CAN_RDH1R register  ******************/
#define  CAN_RDH1R_DATA4                     ((uint32_t)0x000000FF)        /*!<Data byte 4 */
#define  CAN_RDH1R_DATA5                     ((uint32_t)0x0000FF00)        /*!<Data byte 5 */
#define  CAN_RDH1R_DATA6                     ((uint32_t)0x00FF0000)        /*!<Data byte 6 */
#define  CAN_RDH1R_DATA7                     ((uint32_t)0xFF000000)        /*!<Data byte 7 */

/*!<CAN filter registers */
/*******************  Bit definition for CAN_FMR register  ********************/
#define  CAN_FMR_FINIT                       ((uint8_t)0x01)               /*!<Filter Init Mode */
#define  CAN_FMR_CAN2SB                      ((uint32_t)0x00003F00)        /*!<CAN2 start bank */

/*******************  Bit definition for CAN_FM1R register  *******************/
#define  CAN_FM1R_FBM                        ((uint32_t)0x3FFF)            /*!<Filter Mode */
#define  CAN_FM1R_FBM0                       ((uint32_t)0x0001)            /*!<Filter Init Mode bit 0  */
#define  CAN_FM1R_FBM1                       ((uint32_t)0x0002)            /*!<Filter Init Mode bit 1  */
#define  CAN_FM1R_FBM2                       ((uint32_t)0x0004)            /*!<Filter Init Mode bit 2  */
#define  CAN_FM1R_FBM3                       ((uint32_t)0x0008)            /*!<Filter Init Mode bit 3  */
#define  CAN_FM1R_FBM4                       ((uint32_t)0x0010)            /*!<Filter Init Mode bit 4  */
#define  CAN_FM1R_FBM5                       ((uint32_t)0x0020)            /*!<Filter Init Mode bit 5  */
#define  CAN_FM1R_FBM6                       ((uint32_t)0x0040)            /*!<Filter Init Mode bit 6  */
#define  CAN_FM1R_FBM7                       ((uint32_t)0x0080)            /*!<Filter Init Mode bit 7  */
#define  CAN_FM1R_FBM8                       ((uint32_t)0x0100)            /*!<Filter Init Mode bit 8  */
#define  CAN_FM1R_FBM9                       ((uint32_t)0x0200)            /*!<Filter Init Mode bit 9  */
#define  CAN_FM1R_FBM10                      ((uint32_t)0x0400)            /*!<Filter Init Mode bit 10 */
#define  CAN_FM1R_FBM11                      ((uint32_t)0x0800)            /*!<Filter Init Mode bit 11 */
#define  CAN_FM1R_FBM12                      ((uint32_t)0x1000)            /*!<Filter Init Mode bit 12 */
#define  CAN_FM1R_FBM13                      ((uint32_t)0x2000)            /*!<Filter Init Mode bit 13 */

/*******************  Bit definition for CAN_FS1R register  *******************/
#define  CAN_FS1R_FSC                        ((uint32_t)0x00003FFF)        /*!<Filter Scale Configuration        */
#define  CAN_FS1R_FSC0                       ((uint32_t)0x00000001)        /*!<Filter Scale Configuration bit 0  */
#define  CAN_FS1R_FSC1                       ((uint32_t)0x00000002)        /*!<Filter Scale Configuration bit 1  */
#define  CAN_FS1R_FSC2                       ((uint32_t)0x00000004)        /*!<Filter Scale Configuration bit 2  */
#define  CAN_FS1R_FSC3                       ((uint32_t)0x00000008)        /*!<Filter Scale Configuration bit 3  */
#define  CAN_FS1R_FSC4                       ((uint32_t)0x00000010)        /*!<Filter Scale Configuration bit 4  */
#define  CAN_FS1R_FSC5                       ((uint32_t)0x00000020)        /*!<Filter Scale Configuration bit 5  */
#define  CAN_FS1R_FSC6                       ((uint32_t)0x00000040)        /*!<Filter Scale Configuration bit 6  */
#define  CAN_FS1R_FSC7                       ((uint32_t)0x00000080)        /*!<Filter Scale Configuration bit 7  */
#define  CAN_FS1R_FSC8                       ((uint32_t)0x00000100)        /*!<Filter Scale Configuration bit 8  */
#define  CAN_FS1R_FSC9                       ((uint32_t)0x00000200)        /*!<Filter Scale Configuration bit 9  */
#define  CAN_FS1R_FSC10                      ((uint32_t)0x00000400)        /*!<Filter Scale Configuration bit 10 */
#define  CAN_FS1R_FSC11                      ((uint32_t)0x00000800)        /*!<Filter Scale Configuration bit 11 */
#define  CAN_FS1R_FSC12                      ((uint32_t)0x00001000)        /*!<Filter Scale Configuration bit 12 */
#define  CAN_FS1R_FSC13                      ((uint32_t)0x00002000)        /*!<Filter Scale Configuration bit 13 */

/******************  Bit definition for CAN_FFA1R register  *******************/
#define  CAN_FFA1R_FFA                       ((uint32_t)0x00003FFF)        /*!<Filter FIFO Assignment */
#define  CAN_FFA1R_FFA0                      ((uint32_t)0x00000001)        /*!<Filter FIFO Assignment for Filter 0 */
#define  CAN_FFA1R_FFA1                      ((uint32_t)0x00000002)        /*!<Filter FIFO Assignment for Filter 1 */
#define  CAN_FFA1R_FFA2                      ((uint32_t)0x00000004)        /*!<Filter FIFO Assignment for Filter 2 */
#define  CAN_FFA1R_FFA3                      ((uint32_t)0x00000008)        /*!<Filter FIFO Assignment for Filter 3 */
#define  CAN_FFA1R_FFA4                      ((uint32_t)0x00000010)        /*!<Filter FIFO Assignment for Filter 4 */
#define  CAN_FFA1R_FFA5                      ((uint32_t)0x00000020)        /*!<Filter FIFO Assignment for Filter 5 */
#define  CAN_FFA1R_FFA6                      ((uint32_t)0x00000040)        /*!<Filter FIFO Assignment for Filter 6 */
#define  CAN_FFA1R_FFA7                      ((uint32_t)0x00000080)        /*!<Filter FIFO Assignment for Filter 7 */
#define  CAN_FFA1R_FFA8                      ((uint32_t)0x00000100)        /*!<Filter FIFO Assignment for Filter 8 */
#define  CAN_FFA1R_FFA9                      ((uint32_t)0x00000200)        /*!<Filter FIFO Assignment for Filter 9 */
#define  CAN_FFA1R_FFA10                     ((uint32_t)0x00000400)        /*!<Filter FIFO Assignment for Filter 10 */
#define  CAN_FFA1R_FFA11                     ((uint32_t)0x00000800)        /*!<Filter FIFO Assignment for Filter 11 */
#define  CAN_FFA1R_FFA12                     ((uint32_t)0x00001000)        /*!<Filter FIFO Assignment for Filter 12 */
#define  CAN_FFA1R_FFA13                     ((uint32_t)0x00002000)        /*!<Filter FIFO Assignment for Filter 13 */

/*******************  Bit definition for CAN_FA1R register  *******************/
#define  CAN_FA1R_FACT                       ((uint32_t)0x00003FFF)        /*!<Filter Active    */
#define  CAN_FA1R_FACT0                      ((uint32_t)0x00000001)        /*!<Filter 0 Active  */
#define  CAN_FA1R_FACT1                      ((uint32_t)0x00000002)        /*!<Filter 1 Active  */
#define  CAN_FA1R_FACT2                      ((uint32_t)0x00000004)        /*!<Filter 2 Active  */
#define  CAN_FA1R_FACT3                      ((uint32_t)0x00000008)        /*!<Filter 3 Active  */
#define  CAN_FA1R_FACT4                      ((uint32_t)0x00000010)        /*!<Filter 4 Active  */
#define  CAN_FA1R_FACT5                      ((uint32_t)0x00000020)        /*!<Filter 5 Active  */
#define  CAN_FA1R_FACT6                      ((uint32_t)0x00000040)        /*!<Filter 6 Active  */
#define  CAN_FA1R_FACT7                      ((uint32_t)0x00000080)        /*!<Filter 7 Active  */
#define  CAN_FA1R_FACT8                      ((uint32_t)0x00000100)        /*!<Filter 8 Active  */
#define  CAN_FA1R_FACT9                      ((uint32_t)0x00000200)        /*!<Filter 9 Active  */
#define  CAN_FA1R_FACT10                     ((uint32_t)0x00000400)        /*!<Filter 10 Active */
#define  CAN_FA1R_FACT11                     ((uint32_t)0x00000800)        /*!<Filter 11 Active */
#define  CAN_FA1R_FACT12                     ((uint32_t)0x00001000)        /*!<Filter 12 Active */
#define  CAN_FA1R_FACT13                     ((uint32_t)0x00002000)        /*!<Filter 13 Active */

/*******************  Bit definition for CAN_F0R1 register  *******************/
#define  CAN_F0R1_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F0R1_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F0R1_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F0R1_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F0R1_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F0R1_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F0R1_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F0R1_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F0R1_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F0R1_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F0R1_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F0R1_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F0R1_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F0R1_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F0R1_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F0R1_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F0R1_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F0R1_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F0R1_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F0R1_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F0R1_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F0R1_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F0R1_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F0R1_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F0R1_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F0R1_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F0R1_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F0R1_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F0R1_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F0R1_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F0R1_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F0R1_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F1R1 register  *******************/
#define  CAN_F1R1_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F1R1_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F1R1_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F1R1_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F1R1_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F1R1_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F1R1_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F1R1_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F1R1_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F1R1_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F1R1_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F1R1_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F1R1_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F1R1_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F1R1_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F1R1_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F1R1_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F1R1_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F1R1_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F1R1_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F1R1_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F1R1_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F1R1_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F1R1_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F1R1_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F1R1_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F1R1_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F1R1_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F1R1_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F1R1_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F1R1_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F1R1_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F2R1 register  *******************/
#define  CAN_F2R1_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F2R1_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F2R1_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F2R1_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F2R1_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F2R1_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F2R1_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F2R1_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F2R1_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F2R1_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F2R1_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F2R1_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F2R1_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F2R1_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F2R1_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F2R1_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F2R1_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F2R1_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F2R1_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F2R1_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F2R1_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F2R1_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F2R1_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F2R1_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F2R1_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F2R1_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F2R1_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F2R1_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F2R1_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F2R1_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F2R1_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F2R1_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F3R1 register  *******************/
#define  CAN_F3R1_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F3R1_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F3R1_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F3R1_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F3R1_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F3R1_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F3R1_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F3R1_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F3R1_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F3R1_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F3R1_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F3R1_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F3R1_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F3R1_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F3R1_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F3R1_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F3R1_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F3R1_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F3R1_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F3R1_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F3R1_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F3R1_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F3R1_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F3R1_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F3R1_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F3R1_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F3R1_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F3R1_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F3R1_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F3R1_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F3R1_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F3R1_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F4R1 register  *******************/
#define  CAN_F4R1_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F4R1_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F4R1_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F4R1_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F4R1_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F4R1_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F4R1_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F4R1_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F4R1_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F4R1_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F4R1_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F4R1_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F4R1_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F4R1_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F4R1_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F4R1_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F4R1_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F4R1_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F4R1_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F4R1_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F4R1_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F4R1_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F4R1_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F4R1_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F4R1_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F4R1_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F4R1_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F4R1_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F4R1_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F4R1_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F4R1_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F4R1_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F5R1 register  *******************/
#define  CAN_F5R1_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F5R1_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F5R1_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F5R1_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F5R1_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F5R1_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F5R1_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F5R1_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F5R1_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F5R1_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F5R1_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F5R1_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F5R1_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F5R1_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F5R1_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F5R1_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F5R1_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F5R1_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F5R1_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F5R1_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F5R1_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F5R1_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F5R1_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F5R1_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F5R1_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F5R1_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F5R1_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F5R1_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F5R1_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F5R1_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F5R1_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F5R1_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F6R1 register  *******************/
#define  CAN_F6R1_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F6R1_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F6R1_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F6R1_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F6R1_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F6R1_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F6R1_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F6R1_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F6R1_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F6R1_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F6R1_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F6R1_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F6R1_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F6R1_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F6R1_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F6R1_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F6R1_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F6R1_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F6R1_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F6R1_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F6R1_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F6R1_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F6R1_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F6R1_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F6R1_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F6R1_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F6R1_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F6R1_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F6R1_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F6R1_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F6R1_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F6R1_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F7R1 register  *******************/
#define  CAN_F7R1_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F7R1_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F7R1_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F7R1_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F7R1_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F7R1_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F7R1_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F7R1_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F7R1_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F7R1_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F7R1_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F7R1_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F7R1_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F7R1_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F7R1_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F7R1_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F7R1_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F7R1_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F7R1_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F7R1_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F7R1_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F7R1_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F7R1_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F7R1_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F7R1_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F7R1_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F7R1_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F7R1_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F7R1_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F7R1_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F7R1_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F7R1_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F8R1 register  *******************/
#define  CAN_F8R1_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F8R1_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F8R1_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F8R1_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F8R1_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F8R1_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F8R1_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F8R1_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F8R1_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F8R1_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F8R1_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F8R1_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F8R1_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F8R1_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F8R1_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F8R1_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F8R1_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F8R1_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F8R1_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F8R1_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F8R1_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F8R1_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F8R1_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F8R1_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F8R1_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F8R1_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F8R1_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F8R1_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F8R1_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F8R1_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F8R1_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F8R1_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F9R1 register  *******************/
#define  CAN_F9R1_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F9R1_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F9R1_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F9R1_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F9R1_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F9R1_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F9R1_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F9R1_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F9R1_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F9R1_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F9R1_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F9R1_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F9R1_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F9R1_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F9R1_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F9R1_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F9R1_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F9R1_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F9R1_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F9R1_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F9R1_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F9R1_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F9R1_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F9R1_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F9R1_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F9R1_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F9R1_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F9R1_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F9R1_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F9R1_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F9R1_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F9R1_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F10R1 register  ******************/
#define  CAN_F10R1_FB0                       ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F10R1_FB1                       ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F10R1_FB2                       ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F10R1_FB3                       ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F10R1_FB4                       ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F10R1_FB5                       ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F10R1_FB6                       ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F10R1_FB7                       ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F10R1_FB8                       ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F10R1_FB9                       ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F10R1_FB10                      ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F10R1_FB11                      ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F10R1_FB12                      ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F10R1_FB13                      ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F10R1_FB14                      ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F10R1_FB15                      ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F10R1_FB16                      ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F10R1_FB17                      ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F10R1_FB18                      ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F10R1_FB19                      ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F10R1_FB20                      ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F10R1_FB21                      ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F10R1_FB22                      ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F10R1_FB23                      ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F10R1_FB24                      ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F10R1_FB25                      ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F10R1_FB26                      ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F10R1_FB27                      ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F10R1_FB28                      ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F10R1_FB29                      ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F10R1_FB30                      ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F10R1_FB31                      ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F11R1 register  ******************/
#define  CAN_F11R1_FB0                       ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F11R1_FB1                       ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F11R1_FB2                       ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F11R1_FB3                       ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F11R1_FB4                       ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F11R1_FB5                       ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F11R1_FB6                       ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F11R1_FB7                       ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F11R1_FB8                       ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F11R1_FB9                       ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F11R1_FB10                      ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F11R1_FB11                      ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F11R1_FB12                      ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F11R1_FB13                      ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F11R1_FB14                      ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F11R1_FB15                      ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F11R1_FB16                      ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F11R1_FB17                      ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F11R1_FB18                      ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F11R1_FB19                      ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F11R1_FB20                      ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F11R1_FB21                      ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F11R1_FB22                      ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F11R1_FB23                      ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F11R1_FB24                      ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F11R1_FB25                      ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F11R1_FB26                      ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F11R1_FB27                      ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F11R1_FB28                      ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F11R1_FB29                      ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F11R1_FB30                      ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F11R1_FB31                      ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F12R1 register  ******************/
#define  CAN_F12R1_FB0                       ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F12R1_FB1                       ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F12R1_FB2                       ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F12R1_FB3                       ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F12R1_FB4                       ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F12R1_FB5                       ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F12R1_FB6                       ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F12R1_FB7                       ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F12R1_FB8                       ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F12R1_FB9                       ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F12R1_FB10                      ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F12R1_FB11                      ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F12R1_FB12                      ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F12R1_FB13                      ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F12R1_FB14                      ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F12R1_FB15                      ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F12R1_FB16                      ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F12R1_FB17                      ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F12R1_FB18                      ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F12R1_FB19                      ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F12R1_FB20                      ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F12R1_FB21                      ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F12R1_FB22                      ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F12R1_FB23                      ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F12R1_FB24                      ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F12R1_FB25                      ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F12R1_FB26                      ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F12R1_FB27                      ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F12R1_FB28                      ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F12R1_FB29                      ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F12R1_FB30                      ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F12R1_FB31                      ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F13R1 register  ******************/
#define  CAN_F13R1_FB0                       ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F13R1_FB1                       ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F13R1_FB2                       ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F13R1_FB3                       ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F13R1_FB4                       ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F13R1_FB5                       ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F13R1_FB6                       ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F13R1_FB7                       ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F13R1_FB8                       ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F13R1_FB9                       ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F13R1_FB10                      ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F13R1_FB11                      ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F13R1_FB12                      ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F13R1_FB13                      ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F13R1_FB14                      ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F13R1_FB15                      ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F13R1_FB16                      ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F13R1_FB17                      ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F13R1_FB18                      ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F13R1_FB19                      ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F13R1_FB20                      ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F13R1_FB21                      ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F13R1_FB22                      ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F13R1_FB23                      ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F13R1_FB24                      ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F13R1_FB25                      ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F13R1_FB26                      ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F13R1_FB27                      ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F13R1_FB28                      ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F13R1_FB29                      ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F13R1_FB30                      ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F13R1_FB31                      ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F0R2 register  *******************/
#define  CAN_F0R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F0R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F0R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F0R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F0R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F0R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F0R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F0R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F0R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F0R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F0R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F0R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F0R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F0R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F0R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F0R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F0R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F0R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F0R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F0R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F0R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F0R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F0R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F0R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F0R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F0R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F0R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F0R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F0R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F0R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F0R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F0R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F1R2 register  *******************/
#define  CAN_F1R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F1R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F1R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F1R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F1R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F1R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F1R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F1R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F1R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F1R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F1R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F1R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F1R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F1R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F1R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F1R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F1R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F1R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F1R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F1R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F1R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F1R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F1R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F1R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F1R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F1R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F1R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F1R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F1R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F1R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F1R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F1R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F2R2 register  *******************/
#define  CAN_F2R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F2R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F2R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F2R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F2R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F2R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F2R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F2R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F2R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F2R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F2R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F2R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F2R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F2R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F2R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F2R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F2R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F2R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F2R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F2R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F2R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F2R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F2R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F2R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F2R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F2R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F2R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F2R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F2R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F2R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F2R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F2R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F3R2 register  *******************/
#define  CAN_F3R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F3R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F3R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F3R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F3R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F3R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F3R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F3R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F3R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F3R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F3R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F3R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F3R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F3R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F3R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F3R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F3R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F3R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F3R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F3R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F3R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F3R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F3R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F3R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F3R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F3R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F3R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F3R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F3R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F3R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F3R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F3R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F4R2 register  *******************/
#define  CAN_F4R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F4R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F4R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F4R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F4R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F4R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F4R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F4R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F4R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F4R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F4R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F4R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F4R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F4R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F4R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F4R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F4R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F4R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F4R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F4R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F4R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F4R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F4R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F4R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F4R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F4R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F4R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F4R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F4R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F4R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F4R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F4R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F5R2 register  *******************/
#define  CAN_F5R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F5R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F5R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F5R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F5R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F5R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F5R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F5R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F5R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F5R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F5R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F5R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F5R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F5R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F5R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F5R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F5R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F5R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F5R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F5R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F5R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F5R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F5R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F5R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F5R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F5R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F5R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F5R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F5R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F5R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F5R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F5R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F6R2 register  *******************/
#define  CAN_F6R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F6R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F6R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F6R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F6R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F6R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F6R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F6R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F6R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F6R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F6R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F6R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F6R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F6R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F6R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F6R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F6R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F6R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F6R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F6R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F6R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F6R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F6R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F6R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F6R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F6R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F6R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F6R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F6R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F6R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F6R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F6R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F7R2 register  *******************/
#define  CAN_F7R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F7R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F7R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F7R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F7R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F7R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F7R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F7R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F7R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F7R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F7R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F7R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F7R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F7R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F7R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F7R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F7R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F7R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F7R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F7R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F7R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F7R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F7R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F7R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F7R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F7R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F7R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F7R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F7R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F7R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F7R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F7R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F8R2 register  *******************/
#define  CAN_F8R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F8R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F8R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F8R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F8R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F8R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F8R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F8R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F8R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F8R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F8R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F8R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F8R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F8R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F8R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F8R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F8R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F8R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F8R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F8R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F8R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F8R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F8R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F8R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F8R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F8R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F8R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F8R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F8R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F8R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F8R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F8R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F9R2 register  *******************/
#define  CAN_F9R2_FB0                        ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F9R2_FB1                        ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F9R2_FB2                        ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F9R2_FB3                        ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F9R2_FB4                        ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F9R2_FB5                        ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F9R2_FB6                        ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F9R2_FB7                        ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F9R2_FB8                        ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F9R2_FB9                        ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F9R2_FB10                       ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F9R2_FB11                       ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F9R2_FB12                       ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F9R2_FB13                       ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F9R2_FB14                       ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F9R2_FB15                       ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F9R2_FB16                       ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F9R2_FB17                       ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F9R2_FB18                       ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F9R2_FB19                       ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F9R2_FB20                       ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F9R2_FB21                       ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F9R2_FB22                       ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F9R2_FB23                       ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F9R2_FB24                       ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F9R2_FB25                       ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F9R2_FB26                       ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F9R2_FB27                       ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F9R2_FB28                       ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F9R2_FB29                       ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F9R2_FB30                       ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F9R2_FB31                       ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F10R2 register  ******************/
#define  CAN_F10R2_FB0                       ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F10R2_FB1                       ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F10R2_FB2                       ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F10R2_FB3                       ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F10R2_FB4                       ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F10R2_FB5                       ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F10R2_FB6                       ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F10R2_FB7                       ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F10R2_FB8                       ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F10R2_FB9                       ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F10R2_FB10                      ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F10R2_FB11                      ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F10R2_FB12                      ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F10R2_FB13                      ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F10R2_FB14                      ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F10R2_FB15                      ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F10R2_FB16                      ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F10R2_FB17                      ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F10R2_FB18                      ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F10R2_FB19                      ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F10R2_FB20                      ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F10R2_FB21                      ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F10R2_FB22                      ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F10R2_FB23                      ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F10R2_FB24                      ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F10R2_FB25                      ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F10R2_FB26                      ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F10R2_FB27                      ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F10R2_FB28                      ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F10R2_FB29                      ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F10R2_FB30                      ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F10R2_FB31                      ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F11R2 register  ******************/
#define  CAN_F11R2_FB0                       ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F11R2_FB1                       ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F11R2_FB2                       ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F11R2_FB3                       ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F11R2_FB4                       ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F11R2_FB5                       ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F11R2_FB6                       ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F11R2_FB7                       ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F11R2_FB8                       ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F11R2_FB9                       ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F11R2_FB10                      ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F11R2_FB11                      ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F11R2_FB12                      ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F11R2_FB13                      ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F11R2_FB14                      ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F11R2_FB15                      ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F11R2_FB16                      ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F11R2_FB17                      ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F11R2_FB18                      ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F11R2_FB19                      ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F11R2_FB20                      ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F11R2_FB21                      ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F11R2_FB22                      ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F11R2_FB23                      ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F11R2_FB24                      ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F11R2_FB25                      ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F11R2_FB26                      ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F11R2_FB27                      ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F11R2_FB28                      ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F11R2_FB29                      ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F11R2_FB30                      ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F11R2_FB31                      ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F12R2 register  ******************/
#define  CAN_F12R2_FB0                       ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F12R2_FB1                       ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F12R2_FB2                       ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F12R2_FB3                       ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F12R2_FB4                       ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F12R2_FB5                       ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F12R2_FB6                       ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F12R2_FB7                       ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F12R2_FB8                       ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F12R2_FB9                       ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F12R2_FB10                      ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F12R2_FB11                      ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F12R2_FB12                      ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F12R2_FB13                      ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F12R2_FB14                      ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F12R2_FB15                      ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F12R2_FB16                      ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F12R2_FB17                      ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F12R2_FB18                      ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F12R2_FB19                      ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F12R2_FB20                      ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F12R2_FB21                      ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F12R2_FB22                      ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F12R2_FB23                      ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F12R2_FB24                      ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F12R2_FB25                      ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F12R2_FB26                      ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F12R2_FB27                      ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F12R2_FB28                      ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F12R2_FB29                      ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F12R2_FB30                      ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F12R2_FB31                      ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/*******************  Bit definition for CAN_F13R2 register  ******************/
#define  CAN_F13R2_FB0                       ((uint32_t)0x00000001)        /*!<Filter bit 0 */
#define  CAN_F13R2_FB1                       ((uint32_t)0x00000002)        /*!<Filter bit 1 */
#define  CAN_F13R2_FB2                       ((uint32_t)0x00000004)        /*!<Filter bit 2 */
#define  CAN_F13R2_FB3                       ((uint32_t)0x00000008)        /*!<Filter bit 3 */
#define  CAN_F13R2_FB4                       ((uint32_t)0x00000010)        /*!<Filter bit 4 */
#define  CAN_F13R2_FB5                       ((uint32_t)0x00000020)        /*!<Filter bit 5 */
#define  CAN_F13R2_FB6                       ((uint32_t)0x00000040)        /*!<Filter bit 6 */
#define  CAN_F13R2_FB7                       ((uint32_t)0x00000080)        /*!<Filter bit 7 */
#define  CAN_F13R2_FB8                       ((uint32_t)0x00000100)        /*!<Filter bit 8 */
#define  CAN_F13R2_FB9                       ((uint32_t)0x00000200)        /*!<Filter bit 9 */
#define  CAN_F13R2_FB10                      ((uint32_t)0x00000400)        /*!<Filter bit 10 */
#define  CAN_F13R2_FB11                      ((uint32_t)0x00000800)        /*!<Filter bit 11 */
#define  CAN_F13R2_FB12                      ((uint32_t)0x00001000)        /*!<Filter bit 12 */
#define  CAN_F13R2_FB13                      ((uint32_t)0x00002000)        /*!<Filter bit 13 */
#define  CAN_F13R2_FB14                      ((uint32_t)0x00004000)        /*!<Filter bit 14 */
#define  CAN_F13R2_FB15                      ((uint32_t)0x00008000)        /*!<Filter bit 15 */
#define  CAN_F13R2_FB16                      ((uint32_t)0x00010000)        /*!<Filter bit 16 */
#define  CAN_F13R2_FB17                      ((uint32_t)0x00020000)        /*!<Filter bit 17 */
#define  CAN_F13R2_FB18                      ((uint32_t)0x00040000)        /*!<Filter bit 18 */
#define  CAN_F13R2_FB19                      ((uint32_t)0x00080000)        /*!<Filter bit 19 */
#define  CAN_F13R2_FB20                      ((uint32_t)0x00100000)        /*!<Filter bit 20 */
#define  CAN_F13R2_FB21                      ((uint32_t)0x00200000)        /*!<Filter bit 21 */
#define  CAN_F13R2_FB22                      ((uint32_t)0x00400000)        /*!<Filter bit 22 */
#define  CAN_F13R2_FB23                      ((uint32_t)0x00800000)        /*!<Filter bit 23 */
#define  CAN_F13R2_FB24                      ((uint32_t)0x01000000)        /*!<Filter bit 24 */
#define  CAN_F13R2_FB25                      ((uint32_t)0x02000000)        /*!<Filter bit 25 */
#define  CAN_F13R2_FB26                      ((uint32_t)0x04000000)        /*!<Filter bit 26 */
#define  CAN_F13R2_FB27                      ((uint32_t)0x08000000)        /*!<Filter bit 27 */
#define  CAN_F13R2_FB28                      ((uint32_t)0x10000000)        /*!<Filter bit 28 */
#define  CAN_F13R2_FB29                      ((uint32_t)0x20000000)        /*!<Filter bit 29 */
#define  CAN_F13R2_FB30                      ((uint32_t)0x40000000)        /*!<Filter bit 30 */
#define  CAN_F13R2_FB31                      ((uint32_t)0x80000000)        /*!<Filter bit 31 */

/******************************************************************************/
/*                                                                            */
/*                                 HDMI-CEC (CEC)                             */
/*                                                                            */
/******************************************************************************/

/*******************  Bit definition for CEC_CR register  *********************/
#define  CEC_CR_CECEN                        ((uint32_t)0x00000001)       /*!< CEC Enable                         */
#define  CEC_CR_TXSOM                        ((uint32_t)0x00000002)       /*!< CEC Tx Start Of Message            */
#define  CEC_CR_TXEOM                        ((uint32_t)0x00000004)       /*!< CEC Tx End Of Message              */

/*******************  Bit definition for CEC_CFGR register  *******************/
#define  CEC_CFGR_SFT                        ((uint32_t)0x00000007)       /*!< CEC Signal Free Time               */
#define  CEC_CFGR_RXTOL                      ((uint32_t)0x00000008)       /*!< CEC Tolerance                      */
#define  CEC_CFGR_BRESTP                     ((uint32_t)0x00000010)       /*!< CEC Rx Stop                        */
#define  CEC_CFGR_BREGEN                     ((uint32_t)0x00000020)       /*!< CEC Bit Rising Error generation    */
#define  CEC_CFGR_LBPEGEN                    ((uint32_t)0x00000040)       /*!< CEC Long Period Error generation   */
#define  CEC_CFGR_BRDNOGEN                   ((uint32_t)0x00000080)       /*!< CEC Broadcast no Error generation  */
#define  CEC_CFGR_SFTOPT                     ((uint32_t)0x00000100)       /*!< CEC Signal Free Time optional      */
#define  CEC_CFGR_OAR                        ((uint32_t)0x7FFF0000)       /*!< CEC Own Address                    */
#define  CEC_CFGR_LSTN                       ((uint32_t)0x80000000)       /*!< CEC Listen mode                    */

/*******************  Bit definition for CEC_TXDR register  *******************/
#define  CEC_TXDR_TXD                        ((uint32_t)0x000000FF)       /*!< CEC Tx Data                        */

/*******************  Bit definition for CEC_RXDR register  *******************/
#define  CEC_TXDR_RXD                        ((uint32_t)0x000000FF)       /*!< CEC Rx Data                        */

/*******************  Bit definition for CEC_ISR register  ********************/
#define  CEC_ISR_RXBR                        ((uint32_t)0x00000001)       /*!< CEC Rx-Byte Received                   */
#define  CEC_ISR_RXEND                       ((uint32_t)0x00000002)       /*!< CEC End Of Reception                   */
#define  CEC_ISR_RXOVR                       ((uint32_t)0x00000004)       /*!< CEC Rx-Overrun                         */
#define  CEC_ISR_BRE                         ((uint32_t)0x00000008)       /*!< CEC Rx Bit Rising Error                */
#define  CEC_ISR_SBPE                        ((uint32_t)0x00000010)       /*!< CEC Rx Short Bit period Error          */
#define  CEC_ISR_LBPE                        ((uint32_t)0x00000020)       /*!< CEC Rx Long Bit period Error           */
#define  CEC_ISR_RXACKE                      ((uint32_t)0x00000040)       /*!< CEC Rx Missing Acknowledge             */
#define  CEC_ISR_ARBLST                      ((uint32_t)0x00000080)       /*!< CEC Arbitration Lost                   */
#define  CEC_ISR_TXBR                        ((uint32_t)0x00000100)       /*!< CEC Tx Byte Request                    */
#define  CEC_ISR_TXEND                       ((uint32_t)0x00000200)       /*!< CEC End of Transmission                */
#define  CEC_ISR_TXUDR                       ((uint32_t)0x00000400)       /*!< CEC Tx-Buffer Underrun                 */
#define  CEC_ISR_TXERR                       ((uint32_t)0x00000800)       /*!< CEC Tx-Error                           */
#define  CEC_ISR_TXACKE                      ((uint32_t)0x00001000)       /*!< CEC Tx Missing Acknowledge             */

/*******************  Bit definition for CEC_IER register  ********************/
#define  CEC_IER_RXBRIE                      ((uint32_t)0x00000001)       /*!< CEC Rx-Byte Received IT Enable         */
#define  CEC_IER_RXENDIE                     ((uint32_t)0x00000002)       /*!< CEC End Of Reception IT Enable         */
#define  CEC_IER_RXOVRIE                     ((uint32_t)0x00000004)       /*!< CEC Rx-Overrun IT Enable               */
#define  CEC_IER_BREIE                       ((uint32_t)0x00000008)       /*!< CEC Rx Bit Rising Error IT Enable      */
#define  CEC_IER_SBPEIE                      ((uint32_t)0x00000010)       /*!< CEC Rx Short Bit period Error IT Enable*/
#define  CEC_IER_LBPEIE                      ((uint32_t)0x00000020)       /*!< CEC Rx Long Bit period Error IT Enable */
#define  CEC_IER_RXACKEIE                    ((uint32_t)0x00000040)       /*!< CEC Rx Missing Acknowledge IT Enable   */
#define  CEC_IER_ARBLSTIE                    ((uint32_t)0x00000080)       /*!< CEC Arbitration Lost IT Enable         */
#define  CEC_IER_TXBRIE                      ((uint32_t)0x00000100)       /*!< CEC Tx Byte Request  IT Enable         */
#define  CEC_IER_TXENDIE                     ((uint32_t)0x00000200)       /*!< CEC End of Transmission IT Enable      */
#define  CEC_IER_TXUDRIE                     ((uint32_t)0x00000400)       /*!< CEC Tx-Buffer Underrun IT Enable       */
#define  CEC_IER_TXERRIE                     ((uint32_t)0x00000800)       /*!< CEC Tx-Error IT Enable                 */
#define  CEC_IER_TXACKEIE                    ((uint32_t)0x00001000)       /*!< CEC Tx Missing Acknowledge IT Enable   */

/******************************************************************************/
/*                                                                            */
/*                          CRC calculation unit                              */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for CRC_DR register  *********************/
#define  CRC_DR_DR                           ((uint32_t)0xFFFFFFFF)     /*!< Data register bits */

/*******************  Bit definition for CRC_IDR register  ********************/
#define  CRC_IDR_IDR                         ((uint32_t)0x000000FF)     /*!< General-purpose 8-bit data register bits */

/********************  Bit definition for CRC_CR register  ********************/
#define  CRC_CR_RESET                        ((uint32_t)0x00000001)     /*!< RESET the CRC computation unit bit */
#define  CRC_CR_POLYSIZE                     ((uint32_t)0x00000018)     /*!< Polynomial size bits               */
#define  CRC_CR_POLYSIZE_0                   ((uint32_t)0x00000008)     /*!< Polynomial size bit 0              */
#define  CRC_CR_POLYSIZE_1                   ((uint32_t)0x00000010)     /*!< Polynomial size bit 1              */
#define  CRC_CR_REV_IN                       ((uint32_t)0x00000060)     /*!< REV_IN Reverse Input Data bits     */
#define  CRC_CR_REV_IN_0                     ((uint32_t)0x00000020)     /*!< Bit 0 */
#define  CRC_CR_REV_IN_1                     ((uint32_t)0x00000040)     /*!< Bit 1 */
#define  CRC_CR_REV_OUT                      ((uint32_t)0x00000080)     /*!< REV_OUT Reverse Output Data bits   */

/*******************  Bit definition for CRC_INIT register  *******************/
#define  CRC_INIT_INIT                       ((uint32_t)0xFFFFFFFF)     /*!< Initial CRC value bits         */

/*******************  Bit definition for CRC_POL register  ********************/
#define  CRC_POL_POL                         ((uint32_t)0xFFFFFFFF)     /*!< Coefficients of the polynomial */

/******************************************************************************/
/*                                                                            */
/*                      Digital to Analog Converter                           */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for DAC_CR register  ********************/
#define  DAC_CR_EN1                          ((uint32_t)0x00000001)        /*!<DAC channel1 enable                         */
#define  DAC_CR_BOFF1                        ((uint32_t)0x00000002)        /*!<DAC channel1 output buffer disable          */
#define  DAC_CR_TEN1                         ((uint32_t)0x00000004)        /*!<DAC channel1 Trigger enable                 */

#define  DAC_CR_TSEL1                        ((uint32_t)0x00000038)        /*!<TSEL1[2:0] (DAC channel1 Trigger selection) */
#define  DAC_CR_TSEL1_0                      ((uint32_t)0x00000008)        /*!<Bit 0 */
#define  DAC_CR_TSEL1_1                      ((uint32_t)0x00000010)        /*!<Bit 1 */
#define  DAC_CR_TSEL1_2                      ((uint32_t)0x00000020)        /*!<Bit 2 */

#define  DAC_CR_WAVE1                        ((uint32_t)0x000000C0)        /*!<WAVE1[1:0] (DAC channel1 noise/triangle wave generation enable) */
#define  DAC_CR_WAVE1_0                      ((uint32_t)0x00000040)        /*!<Bit 0 */
#define  DAC_CR_WAVE1_1                      ((uint32_t)0x00000080)        /*!<Bit 1 */

#define  DAC_CR_MAMP1                        ((uint32_t)0x00000F00)        /*!<MAMP1[3:0] (DAC channel1 Mask/Amplitude selector) */
#define  DAC_CR_MAMP1_0                      ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  DAC_CR_MAMP1_1                      ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  DAC_CR_MAMP1_2                      ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  DAC_CR_MAMP1_3                      ((uint32_t)0x00000800)        /*!<Bit 3 */

#define  DAC_CR_DMAEN1                       ((uint32_t)0x00001000)        /*!<DAC channel1 DMA enable                     */
#define  DAC_CR_EN2                          ((uint32_t)0x00010000)        /*!<DAC channel2 enable                         */
#define  DAC_CR_BOFF2                        ((uint32_t)0x00020000)        /*!<DAC channel2 output buffer disable          */
#define  DAC_CR_TEN2                         ((uint32_t)0x00040000)        /*!<DAC channel2 Trigger enable                 */

#define  DAC_CR_TSEL2                        ((uint32_t)0x00380000)        /*!<TSEL2[2:0] (DAC channel2 Trigger selection) */
#define  DAC_CR_TSEL2_0                      ((uint32_t)0x00080000)        /*!<Bit 0 */
#define  DAC_CR_TSEL2_1                      ((uint32_t)0x00100000)        /*!<Bit 1 */
#define  DAC_CR_TSEL2_2                      ((uint32_t)0x00200000)        /*!<Bit 2 */

#define  DAC_CR_WAVE2                        ((uint32_t)0x00C00000)        /*!<WAVE2[1:0] (DAC channel2 noise/triangle wave generation enable) */
#define  DAC_CR_WAVE2_0                      ((uint32_t)0x00400000)        /*!<Bit 0 */
#define  DAC_CR_WAVE2_1                      ((uint32_t)0x00800000)        /*!<Bit 1 */

#define  DAC_CR_MAMP2                        ((uint32_t)0x0F000000)        /*!<MAMP2[3:0] (DAC channel2 Mask/Amplitude selector) */
#define  DAC_CR_MAMP2_0                      ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  DAC_CR_MAMP2_1                      ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  DAC_CR_MAMP2_2                      ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  DAC_CR_MAMP2_3                      ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  DAC_CR_DMAEN2                       ((uint32_t)0x10000000)        /*!<DAC channel2 DMA enabled */

/*****************  Bit definition for DAC_SWTRIGR register  ******************/
#define  DAC_SWTRIGR_SWTRIG1                 ((uint32_t)0x01)               /*!<DAC channel1 software trigger */
#define  DAC_SWTRIGR_SWTRIG2                 ((uint32_t)0x02)               /*!<DAC channel2 software trigger */

/*****************  Bit definition for DAC_DHR12R1 register  ******************/
#define  DAC_DHR12R1_DACC1DHR                ((uint32_t)0x0FFF)            /*!<DAC channel1 12-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12L1 register  ******************/
#define  DAC_DHR12L1_DACC1DHR                ((uint32_t)0xFFF0)            /*!<DAC channel1 12-bit Left aligned data */

/******************  Bit definition for DAC_DHR8R1 register  ******************/
#define  DAC_DHR8R1_DACC1DHR                 ((uint32_t)0xFF)               /*!<DAC channel1 8-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12R2 register  ******************/
#define  DAC_DHR12R2_DACC2DHR                ((uint32_t)0x0FFF)            /*!<DAC channel2 12-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12L2 register  ******************/
#define  DAC_DHR12L2_DACC2DHR                ((uint32_t)0xFFF0)            /*!<DAC channel2 12-bit Left aligned data */

/******************  Bit definition for DAC_DHR8R2 register  ******************/
#define  DAC_DHR8R2_DACC2DHR                 ((uint32_t)0xFF)               /*!<DAC channel2 8-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12RD register  ******************/
#define  DAC_DHR12RD_DACC1DHR                ((uint32_t)0x00000FFF)        /*!<DAC channel1 12-bit Right aligned data */
#define  DAC_DHR12RD_DACC2DHR                ((uint32_t)0x0FFF0000)        /*!<DAC channel2 12-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12LD register  ******************/
#define  DAC_DHR12LD_DACC1DHR                ((uint32_t)0x0000FFF0)        /*!<DAC channel1 12-bit Left aligned data */
#define  DAC_DHR12LD_DACC2DHR                ((uint32_t)0xFFF00000)        /*!<DAC channel2 12-bit Left aligned data */

/******************  Bit definition for DAC_DHR8RD register  ******************/
#define  DAC_DHR8RD_DACC1DHR                 ((uint32_t)0x00FF)            /*!<DAC channel1 8-bit Right aligned data */
#define  DAC_DHR8RD_DACC2DHR                 ((uint32_t)0xFF00)            /*!<DAC channel2 8-bit Right aligned data */

/*******************  Bit definition for DAC_DOR1 register  *******************/
#define  DAC_DOR1_DACC1DOR                   ((uint32_t)0x0FFF)            /*!<DAC channel1 data output */

/*******************  Bit definition for DAC_DOR2 register  *******************/
#define  DAC_DOR2_DACC2DOR                   ((uint32_t)0x0FFF)            /*!<DAC channel2 data output */

/********************  Bit definition for DAC_SR register  ********************/
#define  DAC_SR_DMAUDR1                      ((uint32_t)0x00002000)        /*!<DAC channel1 DMA underrun flag */
#define  DAC_SR_DMAUDR2                      ((uint32_t)0x20000000)        /*!<DAC channel2 DMA underrun flag */

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
#define DCMI_CR_CAPTURE                      ((uint32_t)0x00000001)
#define DCMI_CR_CM                           ((uint32_t)0x00000002)
#define DCMI_CR_CROP                         ((uint32_t)0x00000004)
#define DCMI_CR_JPEG                         ((uint32_t)0x00000008)
#define DCMI_CR_ESS                          ((uint32_t)0x00000010)
#define DCMI_CR_PCKPOL                       ((uint32_t)0x00000020)
#define DCMI_CR_HSPOL                        ((uint32_t)0x00000040)
#define DCMI_CR_VSPOL                        ((uint32_t)0x00000080)
#define DCMI_CR_FCRC_0                       ((uint32_t)0x00000100)
#define DCMI_CR_FCRC_1                       ((uint32_t)0x00000200)
#define DCMI_CR_EDM_0                        ((uint32_t)0x00000400)
#define DCMI_CR_EDM_1                        ((uint32_t)0x00000800)
#define DCMI_CR_CRE                          ((uint32_t)0x00001000)
#define DCMI_CR_ENABLE                       ((uint32_t)0x00004000)
#define DCMI_CR_BSM                          ((uint32_t)0x00030000)
#define DCMI_CR_BSM_0                        ((uint32_t)0x00010000)
#define DCMI_CR_BSM_1                        ((uint32_t)0x00020000)
#define DCMI_CR_OEBS                         ((uint32_t)0x00040000)
#define DCMI_CR_LSM                          ((uint32_t)0x00080000)
#define DCMI_CR_OELS                         ((uint32_t)0x00100000)

/********************  Bits definition for DCMI_SR register  ******************/
#define DCMI_SR_HSYNC                        ((uint32_t)0x00000001)
#define DCMI_SR_VSYNC                        ((uint32_t)0x00000002)
#define DCMI_SR_FNE                          ((uint32_t)0x00000004)

/********************  Bits definition for DCMI_RISR register  ****************/
#define DCMI_RISR_FRAME_RIS                  ((uint32_t)0x00000001)
#define DCMI_RISR_OVF_RIS                    ((uint32_t)0x00000002)
#define DCMI_RISR_ERR_RIS                    ((uint32_t)0x00000004)
#define DCMI_RISR_VSYNC_RIS                  ((uint32_t)0x00000008)
#define DCMI_RISR_LINE_RIS                   ((uint32_t)0x00000010)

/********************  Bits definition for DCMI_IER register  *****************/
#define DCMI_IER_FRAME_IE                    ((uint32_t)0x00000001)
#define DCMI_IER_OVF_IE                      ((uint32_t)0x00000002)
#define DCMI_IER_ERR_IE                      ((uint32_t)0x00000004)
#define DCMI_IER_VSYNC_IE                    ((uint32_t)0x00000008)
#define DCMI_IER_LINE_IE                     ((uint32_t)0x00000010)

/********************  Bits definition for DCMI_MISR register  ****************/
#define DCMI_MISR_FRAME_MIS                  ((uint32_t)0x00000001)
#define DCMI_MISR_OVF_MIS                    ((uint32_t)0x00000002)
#define DCMI_MISR_ERR_MIS                    ((uint32_t)0x00000004)
#define DCMI_MISR_VSYNC_MIS                  ((uint32_t)0x00000008)
#define DCMI_MISR_LINE_MIS                   ((uint32_t)0x00000010)

/********************  Bits definition for DCMI_ICR register  *****************/
#define DCMI_ICR_FRAME_ISC                   ((uint32_t)0x00000001)
#define DCMI_ICR_OVF_ISC                     ((uint32_t)0x00000002)
#define DCMI_ICR_ERR_ISC                     ((uint32_t)0x00000004)
#define DCMI_ICR_VSYNC_ISC                   ((uint32_t)0x00000008)
#define DCMI_ICR_LINE_ISC                    ((uint32_t)0x00000010)

/******************************************************************************/
/*                                                                            */
/*                             DMA Controller                                 */
/*                                                                            */
/******************************************************************************/
/********************  Bits definition for DMA_SxCR register  *****************/
#define DMA_SxCR_CHSEL                       ((uint32_t)0x0E000000)
#define DMA_SxCR_CHSEL_0                     ((uint32_t)0x02000000)
#define DMA_SxCR_CHSEL_1                     ((uint32_t)0x04000000)
#define DMA_SxCR_CHSEL_2                     ((uint32_t)0x08000000)
#define DMA_SxCR_MBURST                      ((uint32_t)0x01800000)
#define DMA_SxCR_MBURST_0                    ((uint32_t)0x00800000)
#define DMA_SxCR_MBURST_1                    ((uint32_t)0x01000000)
#define DMA_SxCR_PBURST                      ((uint32_t)0x00600000)
#define DMA_SxCR_PBURST_0                    ((uint32_t)0x00200000)
#define DMA_SxCR_PBURST_1                    ((uint32_t)0x00400000)
#define DMA_SxCR_ACK                         ((uint32_t)0x00100000)
#define DMA_SxCR_CT                          ((uint32_t)0x00080000)
#define DMA_SxCR_DBM                         ((uint32_t)0x00040000)
#define DMA_SxCR_PL                          ((uint32_t)0x00030000)
#define DMA_SxCR_PL_0                        ((uint32_t)0x00010000)
#define DMA_SxCR_PL_1                        ((uint32_t)0x00020000)
#define DMA_SxCR_PINCOS                      ((uint32_t)0x00008000)
#define DMA_SxCR_MSIZE                       ((uint32_t)0x00006000)
#define DMA_SxCR_MSIZE_0                     ((uint32_t)0x00002000)
#define DMA_SxCR_MSIZE_1                     ((uint32_t)0x00004000)
#define DMA_SxCR_PSIZE                       ((uint32_t)0x00001800)
#define DMA_SxCR_PSIZE_0                     ((uint32_t)0x00000800)
#define DMA_SxCR_PSIZE_1                     ((uint32_t)0x00001000)
#define DMA_SxCR_MINC                        ((uint32_t)0x00000400)
#define DMA_SxCR_PINC                        ((uint32_t)0x00000200)
#define DMA_SxCR_CIRC                        ((uint32_t)0x00000100)
#define DMA_SxCR_DIR                         ((uint32_t)0x000000C0)
#define DMA_SxCR_DIR_0                       ((uint32_t)0x00000040)
#define DMA_SxCR_DIR_1                       ((uint32_t)0x00000080)
#define DMA_SxCR_PFCTRL                      ((uint32_t)0x00000020)
#define DMA_SxCR_TCIE                        ((uint32_t)0x00000010)
#define DMA_SxCR_HTIE                        ((uint32_t)0x00000008)
#define DMA_SxCR_TEIE                        ((uint32_t)0x00000004)
#define DMA_SxCR_DMEIE                       ((uint32_t)0x00000002)
#define DMA_SxCR_EN                          ((uint32_t)0x00000001)

/********************  Bits definition for DMA_SxCNDTR register  **************/
#define DMA_SxNDT                            ((uint32_t)0x0000FFFF)
#define DMA_SxNDT_0                          ((uint32_t)0x00000001)
#define DMA_SxNDT_1                          ((uint32_t)0x00000002)
#define DMA_SxNDT_2                          ((uint32_t)0x00000004)
#define DMA_SxNDT_3                          ((uint32_t)0x00000008)
#define DMA_SxNDT_4                          ((uint32_t)0x00000010)
#define DMA_SxNDT_5                          ((uint32_t)0x00000020)
#define DMA_SxNDT_6                          ((uint32_t)0x00000040)
#define DMA_SxNDT_7                          ((uint32_t)0x00000080)
#define DMA_SxNDT_8                          ((uint32_t)0x00000100)
#define DMA_SxNDT_9                          ((uint32_t)0x00000200)
#define DMA_SxNDT_10                         ((uint32_t)0x00000400)
#define DMA_SxNDT_11                         ((uint32_t)0x00000800)
#define DMA_SxNDT_12                         ((uint32_t)0x00001000)
#define DMA_SxNDT_13                         ((uint32_t)0x00002000)
#define DMA_SxNDT_14                         ((uint32_t)0x00004000)
#define DMA_SxNDT_15                         ((uint32_t)0x00008000)

/********************  Bits definition for DMA_SxFCR register  ****************/
#define DMA_SxFCR_FEIE                       ((uint32_t)0x00000080)
#define DMA_SxFCR_FS                         ((uint32_t)0x00000038)
#define DMA_SxFCR_FS_0                       ((uint32_t)0x00000008)
#define DMA_SxFCR_FS_1                       ((uint32_t)0x00000010)
#define DMA_SxFCR_FS_2                       ((uint32_t)0x00000020)
#define DMA_SxFCR_DMDIS                      ((uint32_t)0x00000004)
#define DMA_SxFCR_FTH                        ((uint32_t)0x00000003)
#define DMA_SxFCR_FTH_0                      ((uint32_t)0x00000001)
#define DMA_SxFCR_FTH_1                      ((uint32_t)0x00000002)

/********************  Bits definition for DMA_LISR register  *****************/
#define DMA_LISR_TCIF3                       ((uint32_t)0x08000000)
#define DMA_LISR_HTIF3                       ((uint32_t)0x04000000)
#define DMA_LISR_TEIF3                       ((uint32_t)0x02000000)
#define DMA_LISR_DMEIF3                      ((uint32_t)0x01000000)
#define DMA_LISR_FEIF3                       ((uint32_t)0x00400000)
#define DMA_LISR_TCIF2                       ((uint32_t)0x00200000)
#define DMA_LISR_HTIF2                       ((uint32_t)0x00100000)
#define DMA_LISR_TEIF2                       ((uint32_t)0x00080000)
#define DMA_LISR_DMEIF2                      ((uint32_t)0x00040000)
#define DMA_LISR_FEIF2                       ((uint32_t)0x00010000)
#define DMA_LISR_TCIF1                       ((uint32_t)0x00000800)
#define DMA_LISR_HTIF1                       ((uint32_t)0x00000400)
#define DMA_LISR_TEIF1                       ((uint32_t)0x00000200)
#define DMA_LISR_DMEIF1                      ((uint32_t)0x00000100)
#define DMA_LISR_FEIF1                       ((uint32_t)0x00000040)
#define DMA_LISR_TCIF0                       ((uint32_t)0x00000020)
#define DMA_LISR_HTIF0                       ((uint32_t)0x00000010)
#define DMA_LISR_TEIF0                       ((uint32_t)0x00000008)
#define DMA_LISR_DMEIF0                      ((uint32_t)0x00000004)
#define DMA_LISR_FEIF0                       ((uint32_t)0x00000001)

/********************  Bits definition for DMA_HISR register  *****************/
#define DMA_HISR_TCIF7                       ((uint32_t)0x08000000)
#define DMA_HISR_HTIF7                       ((uint32_t)0x04000000)
#define DMA_HISR_TEIF7                       ((uint32_t)0x02000000)
#define DMA_HISR_DMEIF7                      ((uint32_t)0x01000000)
#define DMA_HISR_FEIF7                       ((uint32_t)0x00400000)
#define DMA_HISR_TCIF6                       ((uint32_t)0x00200000)
#define DMA_HISR_HTIF6                       ((uint32_t)0x00100000)
#define DMA_HISR_TEIF6                       ((uint32_t)0x00080000)
#define DMA_HISR_DMEIF6                      ((uint32_t)0x00040000)
#define DMA_HISR_FEIF6                       ((uint32_t)0x00010000)
#define DMA_HISR_TCIF5                       ((uint32_t)0x00000800)
#define DMA_HISR_HTIF5                       ((uint32_t)0x00000400)
#define DMA_HISR_TEIF5                       ((uint32_t)0x00000200)
#define DMA_HISR_DMEIF5                      ((uint32_t)0x00000100)
#define DMA_HISR_FEIF5                       ((uint32_t)0x00000040)
#define DMA_HISR_TCIF4                       ((uint32_t)0x00000020)
#define DMA_HISR_HTIF4                       ((uint32_t)0x00000010)
#define DMA_HISR_TEIF4                       ((uint32_t)0x00000008)
#define DMA_HISR_DMEIF4                      ((uint32_t)0x00000004)
#define DMA_HISR_FEIF4                       ((uint32_t)0x00000001)

/********************  Bits definition for DMA_LIFCR register  ****************/
#define DMA_LIFCR_CTCIF3                     ((uint32_t)0x08000000)
#define DMA_LIFCR_CHTIF3                     ((uint32_t)0x04000000)
#define DMA_LIFCR_CTEIF3                     ((uint32_t)0x02000000)
#define DMA_LIFCR_CDMEIF3                    ((uint32_t)0x01000000)
#define DMA_LIFCR_CFEIF3                     ((uint32_t)0x00400000)
#define DMA_LIFCR_CTCIF2                     ((uint32_t)0x00200000)
#define DMA_LIFCR_CHTIF2                     ((uint32_t)0x00100000)
#define DMA_LIFCR_CTEIF2                     ((uint32_t)0x00080000)
#define DMA_LIFCR_CDMEIF2                    ((uint32_t)0x00040000)
#define DMA_LIFCR_CFEIF2                     ((uint32_t)0x00010000)
#define DMA_LIFCR_CTCIF1                     ((uint32_t)0x00000800)
#define DMA_LIFCR_CHTIF1                     ((uint32_t)0x00000400)
#define DMA_LIFCR_CTEIF1                     ((uint32_t)0x00000200)
#define DMA_LIFCR_CDMEIF1                    ((uint32_t)0x00000100)
#define DMA_LIFCR_CFEIF1                     ((uint32_t)0x00000040)
#define DMA_LIFCR_CTCIF0                     ((uint32_t)0x00000020)
#define DMA_LIFCR_CHTIF0                     ((uint32_t)0x00000010)
#define DMA_LIFCR_CTEIF0                     ((uint32_t)0x00000008)
#define DMA_LIFCR_CDMEIF0                    ((uint32_t)0x00000004)
#define DMA_LIFCR_CFEIF0                     ((uint32_t)0x00000001)

/********************  Bits definition for DMA_HIFCR  register  ****************/
#define DMA_HIFCR_CTCIF7                     ((uint32_t)0x08000000)
#define DMA_HIFCR_CHTIF7                     ((uint32_t)0x04000000)
#define DMA_HIFCR_CTEIF7                     ((uint32_t)0x02000000)
#define DMA_HIFCR_CDMEIF7                    ((uint32_t)0x01000000)
#define DMA_HIFCR_CFEIF7                     ((uint32_t)0x00400000)
#define DMA_HIFCR_CTCIF6                     ((uint32_t)0x00200000)
#define DMA_HIFCR_CHTIF6                     ((uint32_t)0x00100000)
#define DMA_HIFCR_CTEIF6                     ((uint32_t)0x00080000)
#define DMA_HIFCR_CDMEIF6                    ((uint32_t)0x00040000)
#define DMA_HIFCR_CFEIF6                     ((uint32_t)0x00010000)
#define DMA_HIFCR_CTCIF5                     ((uint32_t)0x00000800)
#define DMA_HIFCR_CHTIF5                     ((uint32_t)0x00000400)
#define DMA_HIFCR_CTEIF5                     ((uint32_t)0x00000200)
#define DMA_HIFCR_CDMEIF5                    ((uint32_t)0x00000100)
#define DMA_HIFCR_CFEIF5                     ((uint32_t)0x00000040)
#define DMA_HIFCR_CTCIF4                     ((uint32_t)0x00000020)
#define DMA_HIFCR_CHTIF4                     ((uint32_t)0x00000010)
#define DMA_HIFCR_CTEIF4                     ((uint32_t)0x00000008)
#define DMA_HIFCR_CDMEIF4                    ((uint32_t)0x00000004)
#define DMA_HIFCR_CFEIF4                     ((uint32_t)0x00000001)

/******************************************************************************/
/*                                                                            */
/*                         AHB Master DMA2D Controller (DMA2D)                */
/*                                                                            */
/******************************************************************************/

/********************  Bit definition for DMA2D_CR register  ******************/

#define DMA2D_CR_START                     ((uint32_t)0x00000001)               /*!< Start transfer                          */
#define DMA2D_CR_SUSP                      ((uint32_t)0x00000002)               /*!< Suspend transfer                        */
#define DMA2D_CR_ABORT                     ((uint32_t)0x00000004)               /*!< Abort transfer                          */
#define DMA2D_CR_TEIE                      ((uint32_t)0x00000100)               /*!< Transfer Error Interrupt Enable         */
#define DMA2D_CR_TCIE                      ((uint32_t)0x00000200)               /*!< Transfer Complete Interrupt Enable      */
#define DMA2D_CR_TWIE                      ((uint32_t)0x00000400)               /*!< Transfer Watermark Interrupt Enable     */
#define DMA2D_CR_CAEIE                     ((uint32_t)0x00000800)               /*!< CLUT Access Error Interrupt Enable      */
#define DMA2D_CR_CTCIE                     ((uint32_t)0x00001000)               /*!< CLUT Transfer Complete Interrupt Enable */
#define DMA2D_CR_CEIE                      ((uint32_t)0x00002000)               /*!< Configuration Error Interrupt Enable    */
#define DMA2D_CR_MODE                      ((uint32_t)0x00030000)               /*!< DMA2D Mode                              */

/********************  Bit definition for DMA2D_ISR register  *****************/

#define DMA2D_ISR_TEIF                     ((uint32_t)0x00000001)               /*!< Transfer Error Interrupt Flag         */
#define DMA2D_ISR_TCIF                     ((uint32_t)0x00000002)               /*!< Transfer Complete Interrupt Flag      */
#define DMA2D_ISR_TWIF                     ((uint32_t)0x00000004)               /*!< Transfer Watermark Interrupt Flag     */
#define DMA2D_ISR_CAEIF                    ((uint32_t)0x00000008)               /*!< CLUT Access Error Interrupt Flag      */
#define DMA2D_ISR_CTCIF                    ((uint32_t)0x00000010)               /*!< CLUT Transfer Complete Interrupt Flag */
#define DMA2D_ISR_CEIF                     ((uint32_t)0x00000020)               /*!< Configuration Error Interrupt Flag    */

/********************  Bit definition for DMA2D_IFSR register  ****************/

#define DMA2D_IFSR_CTEIF                   ((uint32_t)0x00000001)               /*!< Clears Transfer Error Interrupt Flag         */
#define DMA2D_IFSR_CTCIF                   ((uint32_t)0x00000002)               /*!< Clears Transfer Complete Interrupt Flag      */
#define DMA2D_IFSR_CTWIF                   ((uint32_t)0x00000004)               /*!< Clears Transfer Watermark Interrupt Flag     */
#define DMA2D_IFSR_CCAEIF                  ((uint32_t)0x00000008)               /*!< Clears CLUT Access Error Interrupt Flag      */
#define DMA2D_IFSR_CCTCIF                  ((uint32_t)0x00000010)               /*!< Clears CLUT Transfer Complete Interrupt Flag */
#define DMA2D_IFSR_CCEIF                   ((uint32_t)0x00000020)               /*!< Clears Configuration Error Interrupt Flag    */

/********************  Bit definition for DMA2D_FGMAR register  ***************/

#define DMA2D_FGMAR_MA                     ((uint32_t)0xFFFFFFFF)               /*!< Memory Address */

/********************  Bit definition for DMA2D_FGOR register  ****************/

#define DMA2D_FGOR_LO                      ((uint32_t)0x00003FFF)               /*!< Line Offset */

/********************  Bit definition for DMA2D_BGMAR register  ***************/

#define DMA2D_BGMAR_MA                     ((uint32_t)0xFFFFFFFF)               /*!< Memory Address */

/********************  Bit definition for DMA2D_BGOR register  ****************/

#define DMA2D_BGOR_LO                      ((uint32_t)0x00003FFF)               /*!< Line Offset */

/********************  Bit definition for DMA2D_FGPFCCR register  *************/

#define DMA2D_FGPFCCR_CM                   ((uint32_t)0x0000000F)               /*!< Color mode      */
#define DMA2D_FGPFCCR_CCM                  ((uint32_t)0x00000010)               /*!< CLUT Color mode */
#define DMA2D_FGPFCCR_START                ((uint32_t)0x00000020)               /*!< Start           */
#define DMA2D_FGPFCCR_CS                   ((uint32_t)0x0000FF00)               /*!< CLUT size       */
#define DMA2D_FGPFCCR_AM                   ((uint32_t)0x00030000)               /*!< Alpha mode      */
#define DMA2D_FGPFCCR_ALPHA                ((uint32_t)0xFF000000)               /*!< Alpha value     */

/********************  Bit definition for DMA2D_FGCOLR register  **************/

#define DMA2D_FGCOLR_BLUE                  ((uint32_t)0x000000FF)               /*!< Blue Value  */
#define DMA2D_FGCOLR_GREEN                 ((uint32_t)0x0000FF00)               /*!< Green Value */
#define DMA2D_FGCOLR_RED                   ((uint32_t)0x00FF0000)               /*!< Red Value   */

/********************  Bit definition for DMA2D_BGPFCCR register  *************/

#define DMA2D_BGPFCCR_CM                   ((uint32_t)0x0000000F)               /*!< Color mode      */
#define DMA2D_BGPFCCR_CCM                  ((uint32_t)0x00000010)               /*!< CLUT Color mode */
#define DMA2D_BGPFCCR_START                ((uint32_t)0x00000020)               /*!< Start           */
#define DMA2D_BGPFCCR_CS                   ((uint32_t)0x0000FF00)               /*!< CLUT size       */
#define DMA2D_BGPFCCR_AM                   ((uint32_t)0x00030000)               /*!< Alpha Mode      */
#define DMA2D_BGPFCCR_ALPHA                ((uint32_t)0xFF000000)               /*!< Alpha value     */

/********************  Bit definition for DMA2D_BGCOLR register  **************/

#define DMA2D_BGCOLR_BLUE                  ((uint32_t)0x000000FF)               /*!< Blue Value  */
#define DMA2D_BGCOLR_GREEN                 ((uint32_t)0x0000FF00)               /*!< Green Value */
#define DMA2D_BGCOLR_RED                   ((uint32_t)0x00FF0000)               /*!< Red Value   */

/********************  Bit definition for DMA2D_FGCMAR register  **************/

#define DMA2D_FGCMAR_MA                    ((uint32_t)0xFFFFFFFF)               /*!< Memory Address */

/********************  Bit definition for DMA2D_BGCMAR register  **************/

#define DMA2D_BGCMAR_MA                    ((uint32_t)0xFFFFFFFF)               /*!< Memory Address */

/********************  Bit definition for DMA2D_OPFCCR register  **************/

#define DMA2D_OPFCCR_CM                    ((uint32_t)0x00000007)               /*!< Color mode */

/********************  Bit definition for DMA2D_OCOLR register  ***************/

/*!<Mode_ARGB8888/RGB888 */

#define DMA2D_OCOLR_BLUE_1                 ((uint32_t)0x000000FF)               /*!< BLUE Value          */
#define DMA2D_OCOLR_GREEN_1                ((uint32_t)0x0000FF00)               /*!< GREEN Value         */
#define DMA2D_OCOLR_RED_1                  ((uint32_t)0x00FF0000)               /*!< Red Value           */
#define DMA2D_OCOLR_ALPHA_1                ((uint32_t)0xFF000000)               /*!< Alpha Channel Value */

/*!<Mode_RGB565 */
#define DMA2D_OCOLR_BLUE_2                 ((uint32_t)0x0000001F)               /*!< BLUE Value   */
#define DMA2D_OCOLR_GREEN_2                ((uint32_t)0x000007E0)               /*!< GREEN Value  */
#define DMA2D_OCOLR_RED_2                  ((uint32_t)0x0000F800)               /*!< Red Value    */

/*!<Mode_ARGB1555 */
#define DMA2D_OCOLR_BLUE_3                 ((uint32_t)0x0000001F)               /*!< BLUE Value          */
#define DMA2D_OCOLR_GREEN_3                ((uint32_t)0x000003E0)               /*!< GREEN Value         */
#define DMA2D_OCOLR_RED_3                  ((uint32_t)0x00007C00)               /*!< Red Value           */
#define DMA2D_OCOLR_ALPHA_3                ((uint32_t)0x00008000)               /*!< Alpha Channel Value */

/*!<Mode_ARGB4444 */
#define DMA2D_OCOLR_BLUE_4                 ((uint32_t)0x0000000F)               /*!< BLUE Value          */
#define DMA2D_OCOLR_GREEN_4                ((uint32_t)0x000000F0)               /*!< GREEN Value         */
#define DMA2D_OCOLR_RED_4                  ((uint32_t)0x00000F00)               /*!< Red Value           */
#define DMA2D_OCOLR_ALPHA_4                ((uint32_t)0x0000F000)               /*!< Alpha Channel Value */

/********************  Bit definition for DMA2D_OMAR register  ****************/

#define DMA2D_OMAR_MA                      ((uint32_t)0xFFFFFFFF)               /*!< Memory Address */

/********************  Bit definition for DMA2D_OOR register  *****************/

#define DMA2D_OOR_LO                       ((uint32_t)0x00003FFF)               /*!< Line Offset */

/********************  Bit definition for DMA2D_NLR register  *****************/

#define DMA2D_NLR_NL                       ((uint32_t)0x0000FFFF)               /*!< Number of Lines */
#define DMA2D_NLR_PL                       ((uint32_t)0x3FFF0000)               /*!< Pixel per Lines */

/********************  Bit definition for DMA2D_LWR register  *****************/

#define DMA2D_LWR_LW                       ((uint32_t)0x0000FFFF)               /*!< Line Watermark */

/********************  Bit definition for DMA2D_AMTCR register  ***************/

#define DMA2D_AMTCR_EN                     ((uint32_t)0x00000001)               /*!< Enable    */
#define DMA2D_AMTCR_DT                     ((uint32_t)0x0000FF00)               /*!< Dead Time */



/********************  Bit definition for DMA2D_FGCLUT register  **************/

/********************  Bit definition for DMA2D_BGCLUT register  **************/


/******************************************************************************/
/*                                                                            */
/*                    External Interrupt/Event Controller                     */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for EXTI_IMR register  *******************/
#define  EXTI_IMR_MR0                        ((uint32_t)0x00000001)        /*!< Interrupt Mask on line 0 */
#define  EXTI_IMR_MR1                        ((uint32_t)0x00000002)        /*!< Interrupt Mask on line 1 */
#define  EXTI_IMR_MR2                        ((uint32_t)0x00000004)        /*!< Interrupt Mask on line 2 */
#define  EXTI_IMR_MR3                        ((uint32_t)0x00000008)        /*!< Interrupt Mask on line 3 */
#define  EXTI_IMR_MR4                        ((uint32_t)0x00000010)        /*!< Interrupt Mask on line 4 */
#define  EXTI_IMR_MR5                        ((uint32_t)0x00000020)        /*!< Interrupt Mask on line 5 */
#define  EXTI_IMR_MR6                        ((uint32_t)0x00000040)        /*!< Interrupt Mask on line 6 */
#define  EXTI_IMR_MR7                        ((uint32_t)0x00000080)        /*!< Interrupt Mask on line 7 */
#define  EXTI_IMR_MR8                        ((uint32_t)0x00000100)        /*!< Interrupt Mask on line 8 */
#define  EXTI_IMR_MR9                        ((uint32_t)0x00000200)        /*!< Interrupt Mask on line 9 */
#define  EXTI_IMR_MR10                       ((uint32_t)0x00000400)        /*!< Interrupt Mask on line 10 */
#define  EXTI_IMR_MR11                       ((uint32_t)0x00000800)        /*!< Interrupt Mask on line 11 */
#define  EXTI_IMR_MR12                       ((uint32_t)0x00001000)        /*!< Interrupt Mask on line 12 */
#define  EXTI_IMR_MR13                       ((uint32_t)0x00002000)        /*!< Interrupt Mask on line 13 */
#define  EXTI_IMR_MR14                       ((uint32_t)0x00004000)        /*!< Interrupt Mask on line 14 */
#define  EXTI_IMR_MR15                       ((uint32_t)0x00008000)        /*!< Interrupt Mask on line 15 */
#define  EXTI_IMR_MR16                       ((uint32_t)0x00010000)        /*!< Interrupt Mask on line 16 */
#define  EXTI_IMR_MR17                       ((uint32_t)0x00020000)        /*!< Interrupt Mask on line 17 */
#define  EXTI_IMR_MR18                       ((uint32_t)0x00040000)        /*!< Interrupt Mask on line 18 */
#define  EXTI_IMR_MR19                       ((uint32_t)0x00080000)        /*!< Interrupt Mask on line 19 */
#define  EXTI_IMR_MR20                       ((uint32_t)0x00100000)        /*!< Interrupt Mask on line 20 */
#define  EXTI_IMR_MR21                       ((uint32_t)0x00200000)        /*!< Interrupt Mask on line 21 */
#define  EXTI_IMR_MR22                       ((uint32_t)0x00400000)        /*!< Interrupt Mask on line 22 */
#define  EXTI_IMR_MR23                       ((uint32_t)0x00800000)        /*!< Interrupt Mask on line 23 */

/*******************  Bit definition for EXTI_EMR register  *******************/
#define  EXTI_EMR_MR0                        ((uint32_t)0x00000001)        /*!< Event Mask on line 0 */
#define  EXTI_EMR_MR1                        ((uint32_t)0x00000002)        /*!< Event Mask on line 1 */
#define  EXTI_EMR_MR2                        ((uint32_t)0x00000004)        /*!< Event Mask on line 2 */
#define  EXTI_EMR_MR3                        ((uint32_t)0x00000008)        /*!< Event Mask on line 3 */
#define  EXTI_EMR_MR4                        ((uint32_t)0x00000010)        /*!< Event Mask on line 4 */
#define  EXTI_EMR_MR5                        ((uint32_t)0x00000020)        /*!< Event Mask on line 5 */
#define  EXTI_EMR_MR6                        ((uint32_t)0x00000040)        /*!< Event Mask on line 6 */
#define  EXTI_EMR_MR7                        ((uint32_t)0x00000080)        /*!< Event Mask on line 7 */
#define  EXTI_EMR_MR8                        ((uint32_t)0x00000100)        /*!< Event Mask on line 8 */
#define  EXTI_EMR_MR9                        ((uint32_t)0x00000200)        /*!< Event Mask on line 9 */
#define  EXTI_EMR_MR10                       ((uint32_t)0x00000400)        /*!< Event Mask on line 10 */
#define  EXTI_EMR_MR11                       ((uint32_t)0x00000800)        /*!< Event Mask on line 11 */
#define  EXTI_EMR_MR12                       ((uint32_t)0x00001000)        /*!< Event Mask on line 12 */
#define  EXTI_EMR_MR13                       ((uint32_t)0x00002000)        /*!< Event Mask on line 13 */
#define  EXTI_EMR_MR14                       ((uint32_t)0x00004000)        /*!< Event Mask on line 14 */
#define  EXTI_EMR_MR15                       ((uint32_t)0x00008000)        /*!< Event Mask on line 15 */
#define  EXTI_EMR_MR16                       ((uint32_t)0x00010000)        /*!< Event Mask on line 16 */
#define  EXTI_EMR_MR17                       ((uint32_t)0x00020000)        /*!< Event Mask on line 17 */
#define  EXTI_EMR_MR18                       ((uint32_t)0x00040000)        /*!< Event Mask on line 18 */
#define  EXTI_EMR_MR19                       ((uint32_t)0x00080000)        /*!< Event Mask on line 19 */
#define  EXTI_EMR_MR20                       ((uint32_t)0x00100000)        /*!< Event Mask on line 20 */
#define  EXTI_EMR_MR21                       ((uint32_t)0x00200000)        /*!< Event Mask on line 21 */
#define  EXTI_EMR_MR22                       ((uint32_t)0x00400000)        /*!< Event Mask on line 22 */
#define  EXTI_EMR_MR23                       ((uint32_t)0x00800000)        /*!< Event Mask on line 23 */

/******************  Bit definition for EXTI_RTSR register  *******************/
#define  EXTI_RTSR_TR0                       ((uint32_t)0x00000001)        /*!< Rising trigger event configuration bit of line 0 */
#define  EXTI_RTSR_TR1                       ((uint32_t)0x00000002)        /*!< Rising trigger event configuration bit of line 1 */
#define  EXTI_RTSR_TR2                       ((uint32_t)0x00000004)        /*!< Rising trigger event configuration bit of line 2 */
#define  EXTI_RTSR_TR3                       ((uint32_t)0x00000008)        /*!< Rising trigger event configuration bit of line 3 */
#define  EXTI_RTSR_TR4                       ((uint32_t)0x00000010)        /*!< Rising trigger event configuration bit of line 4 */
#define  EXTI_RTSR_TR5                       ((uint32_t)0x00000020)        /*!< Rising trigger event configuration bit of line 5 */
#define  EXTI_RTSR_TR6                       ((uint32_t)0x00000040)        /*!< Rising trigger event configuration bit of line 6 */
#define  EXTI_RTSR_TR7                       ((uint32_t)0x00000080)        /*!< Rising trigger event configuration bit of line 7 */
#define  EXTI_RTSR_TR8                       ((uint32_t)0x00000100)        /*!< Rising trigger event configuration bit of line 8 */
#define  EXTI_RTSR_TR9                       ((uint32_t)0x00000200)        /*!< Rising trigger event configuration bit of line 9 */
#define  EXTI_RTSR_TR10                      ((uint32_t)0x00000400)        /*!< Rising trigger event configuration bit of line 10 */
#define  EXTI_RTSR_TR11                      ((uint32_t)0x00000800)        /*!< Rising trigger event configuration bit of line 11 */
#define  EXTI_RTSR_TR12                      ((uint32_t)0x00001000)        /*!< Rising trigger event configuration bit of line 12 */
#define  EXTI_RTSR_TR13                      ((uint32_t)0x00002000)        /*!< Rising trigger event configuration bit of line 13 */
#define  EXTI_RTSR_TR14                      ((uint32_t)0x00004000)        /*!< Rising trigger event configuration bit of line 14 */
#define  EXTI_RTSR_TR15                      ((uint32_t)0x00008000)        /*!< Rising trigger event configuration bit of line 15 */
#define  EXTI_RTSR_TR16                      ((uint32_t)0x00010000)        /*!< Rising trigger event configuration bit of line 16 */
#define  EXTI_RTSR_TR17                      ((uint32_t)0x00020000)        /*!< Rising trigger event configuration bit of line 17 */
#define  EXTI_RTSR_TR18                      ((uint32_t)0x00040000)        /*!< Rising trigger event configuration bit of line 18 */
#define  EXTI_RTSR_TR19                      ((uint32_t)0x00080000)        /*!< Rising trigger event configuration bit of line 19 */
#define  EXTI_RTSR_TR20                      ((uint32_t)0x00100000)        /*!< Rising trigger event configuration bit of line 20 */
#define  EXTI_RTSR_TR21                      ((uint32_t)0x00200000)        /*!< Rising trigger event configuration bit of line 21 */
#define  EXTI_RTSR_TR22                      ((uint32_t)0x00400000)        /*!< Rising trigger event configuration bit of line 22 */
#define  EXTI_RTSR_TR23                      ((uint32_t)0x00800000)        /*!< Rising trigger event configuration bit of line 23 */

/******************  Bit definition for EXTI_FTSR register  *******************/
#define  EXTI_FTSR_TR0                       ((uint32_t)0x00000001)        /*!< Falling trigger event configuration bit of line 0 */
#define  EXTI_FTSR_TR1                       ((uint32_t)0x00000002)        /*!< Falling trigger event configuration bit of line 1 */
#define  EXTI_FTSR_TR2                       ((uint32_t)0x00000004)        /*!< Falling trigger event configuration bit of line 2 */
#define  EXTI_FTSR_TR3                       ((uint32_t)0x00000008)        /*!< Falling trigger event configuration bit of line 3 */
#define  EXTI_FTSR_TR4                       ((uint32_t)0x00000010)        /*!< Falling trigger event configuration bit of line 4 */
#define  EXTI_FTSR_TR5                       ((uint32_t)0x00000020)        /*!< Falling trigger event configuration bit of line 5 */
#define  EXTI_FTSR_TR6                       ((uint32_t)0x00000040)        /*!< Falling trigger event configuration bit of line 6 */
#define  EXTI_FTSR_TR7                       ((uint32_t)0x00000080)        /*!< Falling trigger event configuration bit of line 7 */
#define  EXTI_FTSR_TR8                       ((uint32_t)0x00000100)        /*!< Falling trigger event configuration bit of line 8 */
#define  EXTI_FTSR_TR9                       ((uint32_t)0x00000200)        /*!< Falling trigger event configuration bit of line 9 */
#define  EXTI_FTSR_TR10                      ((uint32_t)0x00000400)        /*!< Falling trigger event configuration bit of line 10 */
#define  EXTI_FTSR_TR11                      ((uint32_t)0x00000800)        /*!< Falling trigger event configuration bit of line 11 */
#define  EXTI_FTSR_TR12                      ((uint32_t)0x00001000)        /*!< Falling trigger event configuration bit of line 12 */
#define  EXTI_FTSR_TR13                      ((uint32_t)0x00002000)        /*!< Falling trigger event configuration bit of line 13 */
#define  EXTI_FTSR_TR14                      ((uint32_t)0x00004000)        /*!< Falling trigger event configuration bit of line 14 */
#define  EXTI_FTSR_TR15                      ((uint32_t)0x00008000)        /*!< Falling trigger event configuration bit of line 15 */
#define  EXTI_FTSR_TR16                      ((uint32_t)0x00010000)        /*!< Falling trigger event configuration bit of line 16 */
#define  EXTI_FTSR_TR17                      ((uint32_t)0x00020000)        /*!< Falling trigger event configuration bit of line 17 */
#define  EXTI_FTSR_TR18                      ((uint32_t)0x00040000)        /*!< Falling trigger event configuration bit of line 18 */
#define  EXTI_FTSR_TR19                      ((uint32_t)0x00080000)        /*!< Falling trigger event configuration bit of line 19 */
#define  EXTI_FTSR_TR20                      ((uint32_t)0x00100000)        /*!< Falling trigger event configuration bit of line 20 */
#define  EXTI_FTSR_TR21                      ((uint32_t)0x00200000)        /*!< Falling trigger event configuration bit of line 21 */
#define  EXTI_FTSR_TR22                      ((uint32_t)0x00400000)        /*!< Falling trigger event configuration bit of line 22 */
#define  EXTI_FTSR_TR23                      ((uint32_t)0x00800000)        /*!< Falling trigger event configuration bit of line 23 */

/******************  Bit definition for EXTI_SWIER register  ******************/
#define  EXTI_SWIER_SWIER0                   ((uint32_t)0x00000001)        /*!< Software Interrupt on line 0 */
#define  EXTI_SWIER_SWIER1                   ((uint32_t)0x00000002)        /*!< Software Interrupt on line 1 */
#define  EXTI_SWIER_SWIER2                   ((uint32_t)0x00000004)        /*!< Software Interrupt on line 2 */
#define  EXTI_SWIER_SWIER3                   ((uint32_t)0x00000008)        /*!< Software Interrupt on line 3 */
#define  EXTI_SWIER_SWIER4                   ((uint32_t)0x00000010)        /*!< Software Interrupt on line 4 */
#define  EXTI_SWIER_SWIER5                   ((uint32_t)0x00000020)        /*!< Software Interrupt on line 5 */
#define  EXTI_SWIER_SWIER6                   ((uint32_t)0x00000040)        /*!< Software Interrupt on line 6 */
#define  EXTI_SWIER_SWIER7                   ((uint32_t)0x00000080)        /*!< Software Interrupt on line 7 */
#define  EXTI_SWIER_SWIER8                   ((uint32_t)0x00000100)        /*!< Software Interrupt on line 8 */
#define  EXTI_SWIER_SWIER9                   ((uint32_t)0x00000200)        /*!< Software Interrupt on line 9 */
#define  EXTI_SWIER_SWIER10                  ((uint32_t)0x00000400)        /*!< Software Interrupt on line 10 */
#define  EXTI_SWIER_SWIER11                  ((uint32_t)0x00000800)        /*!< Software Interrupt on line 11 */
#define  EXTI_SWIER_SWIER12                  ((uint32_t)0x00001000)        /*!< Software Interrupt on line 12 */
#define  EXTI_SWIER_SWIER13                  ((uint32_t)0x00002000)        /*!< Software Interrupt on line 13 */
#define  EXTI_SWIER_SWIER14                  ((uint32_t)0x00004000)        /*!< Software Interrupt on line 14 */
#define  EXTI_SWIER_SWIER15                  ((uint32_t)0x00008000)        /*!< Software Interrupt on line 15 */
#define  EXTI_SWIER_SWIER16                  ((uint32_t)0x00010000)        /*!< Software Interrupt on line 16 */
#define  EXTI_SWIER_SWIER17                  ((uint32_t)0x00020000)        /*!< Software Interrupt on line 17 */
#define  EXTI_SWIER_SWIER18                  ((uint32_t)0x00040000)        /*!< Software Interrupt on line 18 */
#define  EXTI_SWIER_SWIER19                  ((uint32_t)0x00080000)        /*!< Software Interrupt on line 19 */
#define  EXTI_SWIER_SWIER20                  ((uint32_t)0x00100000)        /*!< Software Interrupt on line 20 */
#define  EXTI_SWIER_SWIER21                  ((uint32_t)0x00200000)        /*!< Software Interrupt on line 21 */
#define  EXTI_SWIER_SWIER22                  ((uint32_t)0x00400000)        /*!< Software Interrupt on line 22 */
#define  EXTI_SWIER_SWIER23                  ((uint32_t)0x00800000)        /*!< Software Interrupt on line 23 */

/*******************  Bit definition for EXTI_PR register  ********************/
#define  EXTI_PR_PR0                         ((uint32_t)0x00000001)        /*!< Pending bit for line 0 */
#define  EXTI_PR_PR1                         ((uint32_t)0x00000002)        /*!< Pending bit for line 1 */
#define  EXTI_PR_PR2                         ((uint32_t)0x00000004)        /*!< Pending bit for line 2 */
#define  EXTI_PR_PR3                         ((uint32_t)0x00000008)        /*!< Pending bit for line 3 */
#define  EXTI_PR_PR4                         ((uint32_t)0x00000010)        /*!< Pending bit for line 4 */
#define  EXTI_PR_PR5                         ((uint32_t)0x00000020)        /*!< Pending bit for line 5 */
#define  EXTI_PR_PR6                         ((uint32_t)0x00000040)        /*!< Pending bit for line 6 */
#define  EXTI_PR_PR7                         ((uint32_t)0x00000080)        /*!< Pending bit for line 7 */
#define  EXTI_PR_PR8                         ((uint32_t)0x00000100)        /*!< Pending bit for line 8 */
#define  EXTI_PR_PR9                         ((uint32_t)0x00000200)        /*!< Pending bit for line 9 */
#define  EXTI_PR_PR10                        ((uint32_t)0x00000400)        /*!< Pending bit for line 10 */
#define  EXTI_PR_PR11                        ((uint32_t)0x00000800)        /*!< Pending bit for line 11 */
#define  EXTI_PR_PR12                        ((uint32_t)0x00001000)        /*!< Pending bit for line 12 */
#define  EXTI_PR_PR13                        ((uint32_t)0x00002000)        /*!< Pending bit for line 13 */
#define  EXTI_PR_PR14                        ((uint32_t)0x00004000)        /*!< Pending bit for line 14 */
#define  EXTI_PR_PR15                        ((uint32_t)0x00008000)        /*!< Pending bit for line 15 */
#define  EXTI_PR_PR16                        ((uint32_t)0x00010000)        /*!< Pending bit for line 16 */
#define  EXTI_PR_PR17                        ((uint32_t)0x00020000)        /*!< Pending bit for line 17 */
#define  EXTI_PR_PR18                        ((uint32_t)0x00040000)        /*!< Pending bit for line 18 */
#define  EXTI_PR_PR19                        ((uint32_t)0x00080000)        /*!< Pending bit for line 19 */
#define  EXTI_PR_PR20                        ((uint32_t)0x00100000)        /*!< Pending bit for line 20 */
#define  EXTI_PR_PR21                        ((uint32_t)0x00200000)        /*!< Pending bit for line 21 */
#define  EXTI_PR_PR22                        ((uint32_t)0x00400000)        /*!< Pending bit for line 22 */
#define  EXTI_PR_PR23                        ((uint32_t)0x00800000)        /*!< Pending bit for line 23 */

/******************************************************************************/
/*                                                                            */
/*                                    FLASH                                   */
/*                                                                            */
/******************************************************************************/
/*******************  Bits definition for FLASH_ACR register  *****************/
#define FLASH_ACR_LATENCY                    ((uint32_t)0x0000000F)
#define FLASH_ACR_LATENCY_0WS                ((uint32_t)0x00000000)
#define FLASH_ACR_LATENCY_1WS                ((uint32_t)0x00000001)
#define FLASH_ACR_LATENCY_2WS                ((uint32_t)0x00000002)
#define FLASH_ACR_LATENCY_3WS                ((uint32_t)0x00000003)
#define FLASH_ACR_LATENCY_4WS                ((uint32_t)0x00000004)
#define FLASH_ACR_LATENCY_5WS                ((uint32_t)0x00000005)
#define FLASH_ACR_LATENCY_6WS                ((uint32_t)0x00000006)
#define FLASH_ACR_LATENCY_7WS                ((uint32_t)0x00000007)
#define FLASH_ACR_LATENCY_8WS                ((uint32_t)0x00000008)
#define FLASH_ACR_LATENCY_9WS                ((uint32_t)0x00000009)
#define FLASH_ACR_LATENCY_10WS               ((uint32_t)0x0000000A)
#define FLASH_ACR_LATENCY_11WS               ((uint32_t)0x0000000B)
#define FLASH_ACR_LATENCY_12WS               ((uint32_t)0x0000000C)
#define FLASH_ACR_LATENCY_13WS               ((uint32_t)0x0000000D)
#define FLASH_ACR_LATENCY_14WS               ((uint32_t)0x0000000E)
#define FLASH_ACR_LATENCY_15WS               ((uint32_t)0x0000000F)
#define FLASH_ACR_PRFTEN                     ((uint32_t)0x00000100)
#define FLASH_ACR_ARTEN                      ((uint32_t)0x00000200)
#define FLASH_ACR_ARTRST                     ((uint32_t)0x00000800)

/*******************  Bits definition for FLASH_SR register  ******************/
#define FLASH_SR_EOP                         ((uint32_t)0x00000001)
#define FLASH_SR_OPERR                       ((uint32_t)0x00000002)
#define FLASH_SR_WRPERR                      ((uint32_t)0x00000010)
#define FLASH_SR_PGAERR                      ((uint32_t)0x00000020)
#define FLASH_SR_PGPERR                      ((uint32_t)0x00000040)
#define FLASH_SR_ERSERR                      ((uint32_t)0x00000080)
#define FLASH_SR_BSY                         ((uint32_t)0x00010000)

/*******************  Bits definition for FLASH_CR register  ******************/
#define FLASH_CR_PG                          ((uint32_t)0x00000001)
#define FLASH_CR_SER                         ((uint32_t)0x00000002)
#define FLASH_CR_MER                         ((uint32_t)0x00000004)
#define FLASH_CR_SNB                         ((uint32_t)0x00000078)
#define FLASH_CR_SNB_0                       ((uint32_t)0x00000008)
#define FLASH_CR_SNB_1                       ((uint32_t)0x00000010)
#define FLASH_CR_SNB_2                       ((uint32_t)0x00000020)
#define FLASH_CR_SNB_3                       ((uint32_t)0x00000040)
#define FLASH_CR_PSIZE                       ((uint32_t)0x00000300)
#define FLASH_CR_PSIZE_0                     ((uint32_t)0x00000100)
#define FLASH_CR_PSIZE_1                     ((uint32_t)0x00000200)
#define FLASH_CR_STRT                        ((uint32_t)0x00010000)
#define FLASH_CR_EOPIE                       ((uint32_t)0x01000000)
#define FLASH_CR_ERRIE                       ((uint32_t)0x02000000)
#define FLASH_CR_LOCK                        ((uint32_t)0x80000000)

/*******************  Bits definition for FLASH_OPTCR register  ***************/
#define FLASH_OPTCR_OPTLOCK                 ((uint32_t)0x00000001)
#define FLASH_OPTCR_OPTSTRT                 ((uint32_t)0x00000002)
#define FLASH_OPTCR_BOR_LEV                 ((uint32_t)0x0000000C)
#define FLASH_OPTCR_BOR_LEV_0               ((uint32_t)0x00000004)
#define FLASH_OPTCR_BOR_LEV_1               ((uint32_t)0x00000008)
#define FLASH_OPTCR_WWDG_SW                 ((uint32_t)0x00000010)
#define FLASH_OPTCR_IWDG_SW                 ((uint32_t)0x00000020)
#define FLASH_OPTCR_nRST_STOP               ((uint32_t)0x00000040)
#define FLASH_OPTCR_nRST_STDBY              ((uint32_t)0x00000080)
#define FLASH_OPTCR_RDP                     ((uint32_t)0x0000FF00)
#define FLASH_OPTCR_RDP_0                   ((uint32_t)0x00000100)
#define FLASH_OPTCR_RDP_1                   ((uint32_t)0x00000200)
#define FLASH_OPTCR_RDP_2                   ((uint32_t)0x00000400)
#define FLASH_OPTCR_RDP_3                   ((uint32_t)0x00000800)
#define FLASH_OPTCR_RDP_4                   ((uint32_t)0x00001000)
#define FLASH_OPTCR_RDP_5                   ((uint32_t)0x00002000)
#define FLASH_OPTCR_RDP_6                   ((uint32_t)0x00004000)
#define FLASH_OPTCR_RDP_7                   ((uint32_t)0x00008000)
#define FLASH_OPTCR_nWRP                    ((uint32_t)0x00FF0000)
#define FLASH_OPTCR_nWRP_0                  ((uint32_t)0x00010000)
#define FLASH_OPTCR_nWRP_1                  ((uint32_t)0x00020000)
#define FLASH_OPTCR_nWRP_2                  ((uint32_t)0x00040000)
#define FLASH_OPTCR_nWRP_3                  ((uint32_t)0x00080000)
#define FLASH_OPTCR_nWRP_4                  ((uint32_t)0x00100000)
#define FLASH_OPTCR_nWRP_5                  ((uint32_t)0x00200000)
#define FLASH_OPTCR_nWRP_6                  ((uint32_t)0x00400000)
#define FLASH_OPTCR_nWRP_7                  ((uint32_t)0x00800000)
#define FLASH_OPTCR_IWDG_STDBY              ((uint32_t)0x40000000)
#define FLASH_OPTCR_IWDG_STOP               ((uint32_t)0x80000000)

/*******************  Bits definition for FLASH_OPTCR1 register  ***************/
#define FLASH_OPTCR1_BOOT_ADD0              ((uint32_t)0x0000FFFF)
#define FLASH_OPTCR1_BOOT_ADD1              ((uint32_t)0xFFFF0000)



/******************************************************************************/
/*                                                                            */
/*                          Flexible Memory Controller                        */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for FMC_BCR1 register  *******************/
#define  FMC_BCR1_MBKEN                     ((uint32_t)0x00000001)        /*!<Memory bank enable bit                 */
#define  FMC_BCR1_MUXEN                     ((uint32_t)0x00000002)        /*!<Address/data multiplexing enable bit   */

#define  FMC_BCR1_MTYP                      ((uint32_t)0x0000000C)        /*!<MTYP[1:0] bits (Memory type)           */
#define  FMC_BCR1_MTYP_0                    ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  FMC_BCR1_MTYP_1                    ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  FMC_BCR1_MWID                      ((uint32_t)0x00000030)        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FMC_BCR1_MWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BCR1_MWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FMC_BCR1_FACCEN                    ((uint32_t)0x00000040)        /*!<Flash access enable        */
#define  FMC_BCR1_BURSTEN                   ((uint32_t)0x00000100)        /*!<Burst enable bit           */
#define  FMC_BCR1_WAITPOL                   ((uint32_t)0x00000200)        /*!<Wait signal polarity bit   */
#define  FMC_BCR1_WRAPMOD                   ((uint32_t)0x00000400)        /*!<Wrapped burst mode support */
#define  FMC_BCR1_WAITCFG                   ((uint32_t)0x00000800)        /*!<Wait timing configuration  */
#define  FMC_BCR1_WREN                      ((uint32_t)0x00001000)        /*!<Write enable bit           */
#define  FMC_BCR1_WAITEN                    ((uint32_t)0x00002000)        /*!<Wait enable bit            */
#define  FMC_BCR1_EXTMOD                    ((uint32_t)0x00004000)        /*!<Extended mode enable       */
#define  FMC_BCR1_ASYNCWAIT                 ((uint32_t)0x00008000)        /*!<Asynchronous wait          */
#define  FMC_BCR1_CPSIZE                     ((uint32_t)0x00070000)        /*!<CRAM page size             */
#define  FMC_BCR1_CPSIZE_0                   ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_BCR1_CPSIZE_1                   ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_BCR1_CPSIZE_2                   ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_BCR1_CBURSTRW                  ((uint32_t)0x00080000)        /*!<Write burst enable         */
#define  FMC_BCR1_CCLKEN                    ((uint32_t)0x00100000)        /*!<Continous clock enable     */
#define  FMC_BCR1_WFDIS                     ((uint32_t)0x00200000)        /*!<Write FIFO Disable         */

/******************  Bit definition for FMC_BCR2 register  *******************/
#define  FMC_BCR2_MBKEN                     ((uint32_t)0x00000001)        /*!<Memory bank enable bit                 */
#define  FMC_BCR2_MUXEN                     ((uint32_t)0x00000002)        /*!<Address/data multiplexing enable bit   */

#define  FMC_BCR2_MTYP                      ((uint32_t)0x0000000C)        /*!<MTYP[1:0] bits (Memory type)           */
#define  FMC_BCR2_MTYP_0                    ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  FMC_BCR2_MTYP_1                    ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  FMC_BCR2_MWID                      ((uint32_t)0x00000030)        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FMC_BCR2_MWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BCR2_MWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FMC_BCR2_FACCEN                    ((uint32_t)0x00000040)        /*!<Flash access enable        */
#define  FMC_BCR2_BURSTEN                   ((uint32_t)0x00000100)        /*!<Burst enable bit           */
#define  FMC_BCR2_WAITPOL                   ((uint32_t)0x00000200)        /*!<Wait signal polarity bit   */
#define  FMC_BCR2_WRAPMOD                   ((uint32_t)0x00000400)        /*!<Wrapped burst mode support */
#define  FMC_BCR2_WAITCFG                   ((uint32_t)0x00000800)        /*!<Wait timing configuration  */
#define  FMC_BCR2_WREN                      ((uint32_t)0x00001000)        /*!<Write enable bit           */
#define  FMC_BCR2_WAITEN                    ((uint32_t)0x00002000)        /*!<Wait enable bit            */
#define  FMC_BCR2_EXTMOD                    ((uint32_t)0x00004000)        /*!<Extended mode enable       */
#define  FMC_BCR2_ASYNCWAIT                 ((uint32_t)0x00008000)        /*!<Asynchronous wait          */
#define  FMC_BCR2_CPSIZE                     ((uint32_t)0x00070000)        /*!<CRAM page size             */
#define  FMC_BCR2_CPSIZE_0                   ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_BCR2_CPSIZE_1                   ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_BCR2_CPSIZE_2                   ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_BCR2_CBURSTRW                  ((uint32_t)0x00080000)        /*!<Write burst enable         */

/******************  Bit definition for FMC_BCR3 register  *******************/
#define  FMC_BCR3_MBKEN                     ((uint32_t)0x00000001)        /*!<Memory bank enable bit                 */
#define  FMC_BCR3_MUXEN                     ((uint32_t)0x00000002)        /*!<Address/data multiplexing enable bit   */

#define  FMC_BCR3_MTYP                      ((uint32_t)0x0000000C)        /*!<MTYP[1:0] bits (Memory type)           */
#define  FMC_BCR3_MTYP_0                    ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  FMC_BCR3_MTYP_1                    ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  FMC_BCR3_MWID                      ((uint32_t)0x00000030)        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FMC_BCR3_MWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BCR3_MWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FMC_BCR3_FACCEN                    ((uint32_t)0x00000040)        /*!<Flash access enable        */
#define  FMC_BCR3_BURSTEN                   ((uint32_t)0x00000100)        /*!<Burst enable bit           */
#define  FMC_BCR3_WAITPOL                   ((uint32_t)0x00000200)        /*!<Wait signal polarity bit   */
#define  FMC_BCR3_WRAPMOD                   ((uint32_t)0x00000400)        /*!<Wrapped burst mode support */
#define  FMC_BCR3_WAITCFG                   ((uint32_t)0x00000800)        /*!<Wait timing configuration  */
#define  FMC_BCR3_WREN                      ((uint32_t)0x00001000)        /*!<Write enable bit           */
#define  FMC_BCR3_WAITEN                    ((uint32_t)0x00002000)        /*!<Wait enable bit            */
#define  FMC_BCR3_EXTMOD                    ((uint32_t)0x00004000)        /*!<Extended mode enable       */
#define  FMC_BCR3_ASYNCWAIT                 ((uint32_t)0x00008000)        /*!<Asynchronous wait          */
#define  FMC_BCR3_CPSIZE                     ((uint32_t)0x00070000)        /*!<CRAM page size             */
#define  FMC_BCR3_CPSIZE_0                   ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_BCR3_CPSIZE_1                   ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_BCR3_CPSIZE_2                   ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_BCR3_CBURSTRW                  ((uint32_t)0x00080000)        /*!<Write burst enable         */

/******************  Bit definition for FMC_BCR4 register  *******************/
#define  FMC_BCR4_MBKEN                     ((uint32_t)0x00000001)        /*!<Memory bank enable bit                 */
#define  FMC_BCR4_MUXEN                     ((uint32_t)0x00000002)        /*!<Address/data multiplexing enable bit   */

#define  FMC_BCR4_MTYP                      ((uint32_t)0x0000000C)        /*!<MTYP[1:0] bits (Memory type)           */
#define  FMC_BCR4_MTYP_0                    ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  FMC_BCR4_MTYP_1                    ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  FMC_BCR4_MWID                      ((uint32_t)0x00000030)        /*!<MWID[1:0] bits (Memory data bus width) */
#define  FMC_BCR4_MWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BCR4_MWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FMC_BCR4_FACCEN                    ((uint32_t)0x00000040)        /*!<Flash access enable        */
#define  FMC_BCR4_BURSTEN                   ((uint32_t)0x00000100)        /*!<Burst enable bit           */
#define  FMC_BCR4_WAITPOL                   ((uint32_t)0x00000200)        /*!<Wait signal polarity bit   */
#define  FMC_BCR4_WRAPMOD                   ((uint32_t)0x00000400)        /*!<Wrapped burst mode support */
#define  FMC_BCR4_WAITCFG                   ((uint32_t)0x00000800)        /*!<Wait timing configuration  */
#define  FMC_BCR4_WREN                      ((uint32_t)0x00001000)        /*!<Write enable bit           */
#define  FMC_BCR4_WAITEN                    ((uint32_t)0x00002000)        /*!<Wait enable bit            */
#define  FMC_BCR4_EXTMOD                    ((uint32_t)0x00004000)        /*!<Extended mode enable       */
#define  FMC_BCR4_ASYNCWAIT                 ((uint32_t)0x00008000)        /*!<Asynchronous wait          */
#define  FMC_BCR4_CPSIZE                     ((uint32_t)0x00070000)        /*!<CRAM page size             */
#define  FMC_BCR4_CPSIZE_0                   ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_BCR4_CPSIZE_1                   ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_BCR4_CPSIZE_2                   ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_BCR4_CBURSTRW                  ((uint32_t)0x00080000)        /*!<Write burst enable         */

/******************  Bit definition for FMC_BTR1 register  ******************/
#define  FMC_BTR1_ADDSET                    ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BTR1_ADDSET_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_BTR1_ADDSET_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_BTR1_ADDSET_2                  ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_BTR1_ADDSET_3                  ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_BTR1_ADDHLD                    ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration)  */
#define  FMC_BTR1_ADDHLD_0                  ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BTR1_ADDHLD_1                  ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_BTR1_ADDHLD_2                  ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_BTR1_ADDHLD_3                  ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_BTR1_DATAST                    ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BTR1_DATAST_0                  ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_BTR1_DATAST_1                  ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_BTR1_DATAST_2                  ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_BTR1_DATAST_3                  ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_BTR1_DATAST_4                  ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_BTR1_DATAST_5                  ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_BTR1_DATAST_6                  ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_BTR1_DATAST_7                  ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_BTR1_BUSTURN                   ((uint32_t)0x000F0000)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BTR1_BUSTURN_0                 ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_BTR1_BUSTURN_1                 ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_BTR1_BUSTURN_2                 ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_BTR1_BUSTURN_3                 ((uint32_t)0x00080000)        /*!<Bit 3 */

#define  FMC_BTR1_CLKDIV                    ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BTR1_CLKDIV_0                  ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FMC_BTR1_CLKDIV_1                  ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FMC_BTR1_CLKDIV_2                  ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FMC_BTR1_CLKDIV_3                  ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FMC_BTR1_DATLAT                    ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BTR1_DATLAT_0                  ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_BTR1_DATLAT_1                  ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_BTR1_DATLAT_2                  ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_BTR1_DATLAT_3                  ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FMC_BTR1_ACCMOD                    ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BTR1_ACCMOD_0                  ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FMC_BTR1_ACCMOD_1                  ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FMC_BTR2 register  *******************/
#define  FMC_BTR2_ADDSET                    ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BTR2_ADDSET_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_BTR2_ADDSET_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_BTR2_ADDSET_2                  ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_BTR2_ADDSET_3                  ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_BTR2_ADDHLD                    ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BTR2_ADDHLD_0                  ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BTR2_ADDHLD_1                  ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_BTR2_ADDHLD_2                  ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_BTR2_ADDHLD_3                  ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_BTR2_DATAST                    ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BTR2_DATAST_0                  ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_BTR2_DATAST_1                  ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_BTR2_DATAST_2                  ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_BTR2_DATAST_3                  ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_BTR2_DATAST_4                  ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_BTR2_DATAST_5                  ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_BTR2_DATAST_6                  ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_BTR2_DATAST_7                  ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_BTR2_BUSTURN                   ((uint32_t)0x000F0000)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BTR2_BUSTURN_0                 ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_BTR2_BUSTURN_1                 ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_BTR2_BUSTURN_2                 ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_BTR2_BUSTURN_3                 ((uint32_t)0x00080000)        /*!<Bit 3 */

#define  FMC_BTR2_CLKDIV                    ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BTR2_CLKDIV_0                  ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FMC_BTR2_CLKDIV_1                  ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FMC_BTR2_CLKDIV_2                  ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FMC_BTR2_CLKDIV_3                  ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FMC_BTR2_DATLAT                    ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BTR2_DATLAT_0                  ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_BTR2_DATLAT_1                  ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_BTR2_DATLAT_2                  ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_BTR2_DATLAT_3                  ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FMC_BTR2_ACCMOD                    ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BTR2_ACCMOD_0                  ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FMC_BTR2_ACCMOD_1                  ((uint32_t)0x20000000)        /*!<Bit 1 */

/*******************  Bit definition for FMC_BTR3 register  *******************/
#define  FMC_BTR3_ADDSET                    ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BTR3_ADDSET_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_BTR3_ADDSET_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_BTR3_ADDSET_2                  ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_BTR3_ADDSET_3                  ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_BTR3_ADDHLD                    ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BTR3_ADDHLD_0                  ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BTR3_ADDHLD_1                  ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_BTR3_ADDHLD_2                  ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_BTR3_ADDHLD_3                  ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_BTR3_DATAST                    ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BTR3_DATAST_0                  ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_BTR3_DATAST_1                  ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_BTR3_DATAST_2                  ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_BTR3_DATAST_3                  ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_BTR3_DATAST_4                  ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_BTR3_DATAST_5                  ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_BTR3_DATAST_6                  ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_BTR3_DATAST_7                  ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_BTR3_BUSTURN                   ((uint32_t)0x000F0000)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BTR3_BUSTURN_0                 ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_BTR3_BUSTURN_1                 ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_BTR3_BUSTURN_2                 ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_BTR3_BUSTURN_3                 ((uint32_t)0x00080000)        /*!<Bit 3 */

#define  FMC_BTR3_CLKDIV                    ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BTR3_CLKDIV_0                  ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FMC_BTR3_CLKDIV_1                  ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FMC_BTR3_CLKDIV_2                  ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FMC_BTR3_CLKDIV_3                  ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FMC_BTR3_DATLAT                    ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BTR3_DATLAT_0                  ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_BTR3_DATLAT_1                  ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_BTR3_DATLAT_2                  ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_BTR3_DATLAT_3                  ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FMC_BTR3_ACCMOD                    ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BTR3_ACCMOD_0                  ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FMC_BTR3_ACCMOD_1                  ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FMC_BTR4 register  *******************/
#define  FMC_BTR4_ADDSET                    ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BTR4_ADDSET_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_BTR4_ADDSET_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_BTR4_ADDSET_2                  ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_BTR4_ADDSET_3                  ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_BTR4_ADDHLD                    ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BTR4_ADDHLD_0                  ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BTR4_ADDHLD_1                  ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_BTR4_ADDHLD_2                  ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_BTR4_ADDHLD_3                  ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_BTR4_DATAST                    ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BTR4_DATAST_0                  ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_BTR4_DATAST_1                  ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_BTR4_DATAST_2                  ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_BTR4_DATAST_3                  ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_BTR4_DATAST_4                  ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_BTR4_DATAST_5                  ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_BTR4_DATAST_6                  ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_BTR4_DATAST_7                  ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_BTR4_BUSTURN                   ((uint32_t)0x000F0000)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BTR4_BUSTURN_0                 ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_BTR4_BUSTURN_1                 ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_BTR4_BUSTURN_2                 ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_BTR4_BUSTURN_3                 ((uint32_t)0x00080000)        /*!<Bit 3 */

#define  FMC_BTR4_CLKDIV                    ((uint32_t)0x00F00000)        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define  FMC_BTR4_CLKDIV_0                  ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FMC_BTR4_CLKDIV_1                  ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FMC_BTR4_CLKDIV_2                  ((uint32_t)0x00400000)        /*!<Bit 2 */
#define  FMC_BTR4_CLKDIV_3                  ((uint32_t)0x00800000)        /*!<Bit 3 */

#define  FMC_BTR4_DATLAT                    ((uint32_t)0x0F000000)        /*!<DATLA[3:0] bits (Data latency) */
#define  FMC_BTR4_DATLAT_0                  ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_BTR4_DATLAT_1                  ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_BTR4_DATLAT_2                  ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_BTR4_DATLAT_3                  ((uint32_t)0x08000000)        /*!<Bit 3 */

#define  FMC_BTR4_ACCMOD                    ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BTR4_ACCMOD_0                  ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FMC_BTR4_ACCMOD_1                  ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FMC_BWTR1 register  ******************/
#define  FMC_BWTR1_ADDSET                   ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BWTR1_ADDSET_0                 ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_BWTR1_ADDSET_1                 ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_BWTR1_ADDSET_2                 ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_BWTR1_ADDSET_3                 ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_BWTR1_ADDHLD                   ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BWTR1_ADDHLD_0                 ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BWTR1_ADDHLD_1                 ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_BWTR1_ADDHLD_2                 ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_BWTR1_ADDHLD_3                 ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_BWTR1_DATAST                   ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BWTR1_DATAST_0                 ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_BWTR1_DATAST_1                 ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_BWTR1_DATAST_2                 ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_BWTR1_DATAST_3                 ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_BWTR1_DATAST_4                 ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_BWTR1_DATAST_5                 ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_BWTR1_DATAST_6                 ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_BWTR1_DATAST_7                 ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_BWTR1_BUSTURN                  ((uint32_t)0x000F0000)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BWTR1_BUSTURN_0                ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_BWTR1_BUSTURN_1                ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_BWTR1_BUSTURN_2                ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_BWTR1_BUSTURN_3                ((uint32_t)0x00080000)        /*!<Bit 3 */

#define  FMC_BWTR1_ACCMOD                   ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BWTR1_ACCMOD_0                 ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FMC_BWTR1_ACCMOD_1                 ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FMC_BWTR2 register  ******************/
#define  FMC_BWTR2_ADDSET                   ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BWTR2_ADDSET_0                 ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_BWTR2_ADDSET_1                 ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_BWTR2_ADDSET_2                 ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_BWTR2_ADDSET_3                 ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_BWTR2_ADDHLD                   ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BWTR2_ADDHLD_0                 ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BWTR2_ADDHLD_1                 ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_BWTR2_ADDHLD_2                 ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_BWTR2_ADDHLD_3                 ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_BWTR2_DATAST                   ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BWTR2_DATAST_0                 ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_BWTR2_DATAST_1                 ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_BWTR2_DATAST_2                 ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_BWTR2_DATAST_3                 ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_BWTR2_DATAST_4                 ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_BWTR2_DATAST_5                 ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_BWTR2_DATAST_6                 ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_BWTR2_DATAST_7                 ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_BWTR2_BUSTURN                  ((uint32_t)0x000F0000)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BWTR2_BUSTURN_0                ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_BWTR2_BUSTURN_1                ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_BWTR2_BUSTURN_2                ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_BWTR2_BUSTURN_3                ((uint32_t)0x00080000)        /*!<Bit 3 */

#define  FMC_BWTR2_ACCMOD                   ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BWTR2_ACCMOD_0                 ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FMC_BWTR2_ACCMOD_1                 ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FMC_BWTR3 register  ******************/
#define  FMC_BWTR3_ADDSET                   ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BWTR3_ADDSET_0                 ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_BWTR3_ADDSET_1                 ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_BWTR3_ADDSET_2                 ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_BWTR3_ADDSET_3                 ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_BWTR3_ADDHLD                   ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BWTR3_ADDHLD_0                 ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BWTR3_ADDHLD_1                 ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_BWTR3_ADDHLD_2                 ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_BWTR3_ADDHLD_3                 ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_BWTR3_DATAST                   ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BWTR3_DATAST_0                 ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_BWTR3_DATAST_1                 ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_BWTR3_DATAST_2                 ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_BWTR3_DATAST_3                 ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_BWTR3_DATAST_4                 ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_BWTR3_DATAST_5                 ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_BWTR3_DATAST_6                 ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_BWTR3_DATAST_7                 ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_BWTR3_BUSTURN                  ((uint32_t)0x000F0000)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BWTR3_BUSTURN_0                ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_BWTR3_BUSTURN_1                ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_BWTR3_BUSTURN_2                ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_BWTR3_BUSTURN_3                ((uint32_t)0x00080000)        /*!<Bit 3 */

#define  FMC_BWTR3_ACCMOD                   ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BWTR3_ACCMOD_0                 ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FMC_BWTR3_ACCMOD_1                 ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FMC_BWTR4 register  ******************/
#define  FMC_BWTR4_ADDSET                   ((uint32_t)0x0000000F)        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define  FMC_BWTR4_ADDSET_0                 ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_BWTR4_ADDSET_1                 ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_BWTR4_ADDSET_2                 ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_BWTR4_ADDSET_3                 ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_BWTR4_ADDHLD                   ((uint32_t)0x000000F0)        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define  FMC_BWTR4_ADDHLD_0                 ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_BWTR4_ADDHLD_1                 ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_BWTR4_ADDHLD_2                 ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_BWTR4_ADDHLD_3                 ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_BWTR4_DATAST                   ((uint32_t)0x0000FF00)        /*!<DATAST [3:0] bits (Data-phase duration) */
#define  FMC_BWTR4_DATAST_0                 ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_BWTR4_DATAST_1                 ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_BWTR4_DATAST_2                 ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_BWTR4_DATAST_3                 ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_BWTR4_DATAST_4                 ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_BWTR4_DATAST_5                 ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_BWTR4_DATAST_6                 ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_BWTR4_DATAST_7                 ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_BWTR4_BUSTURN                  ((uint32_t)0x000F0000)        /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define  FMC_BWTR4_BUSTURN_0                ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_BWTR4_BUSTURN_1                ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_BWTR4_BUSTURN_2                ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_BWTR4_BUSTURN_3                ((uint32_t)0x00080000)        /*!<Bit 3 */

#define  FMC_BWTR4_ACCMOD                   ((uint32_t)0x30000000)        /*!<ACCMOD[1:0] bits (Access mode) */
#define  FMC_BWTR4_ACCMOD_0                 ((uint32_t)0x10000000)        /*!<Bit 0 */
#define  FMC_BWTR4_ACCMOD_1                 ((uint32_t)0x20000000)        /*!<Bit 1 */

/******************  Bit definition for FMC_PCR register  *******************/
#define  FMC_PCR_PWAITEN                   ((uint32_t)0x00000002)        /*!<Wait feature enable bit                   */
#define  FMC_PCR_PBKEN                     ((uint32_t)0x00000004)        /*!<PC Card/NAND Flash memory bank enable bit */
#define  FMC_PCR_PTYP                      ((uint32_t)0x00000008)        /*!<Memory type                               */

#define  FMC_PCR_PWID                      ((uint32_t)0x00000030)        /*!<PWID[1:0] bits (NAND Flash databus width) */
#define  FMC_PCR_PWID_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_PCR_PWID_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FMC_PCR_ECCEN                     ((uint32_t)0x00000040)        /*!<ECC computation logic enable bit          */

#define  FMC_PCR_TCLR                      ((uint32_t)0x00001E00)        /*!<TCLR[3:0] bits (CLE to RE delay)          */
#define  FMC_PCR_TCLR_0                    ((uint32_t)0x00000200)        /*!<Bit 0 */
#define  FMC_PCR_TCLR_1                    ((uint32_t)0x00000400)        /*!<Bit 1 */
#define  FMC_PCR_TCLR_2                    ((uint32_t)0x00000800)        /*!<Bit 2 */
#define  FMC_PCR_TCLR_3                    ((uint32_t)0x00001000)        /*!<Bit 3 */

#define  FMC_PCR_TAR                       ((uint32_t)0x0001E000)        /*!<TAR[3:0] bits (ALE to RE delay)           */
#define  FMC_PCR_TAR_0                     ((uint32_t)0x00002000)        /*!<Bit 0 */
#define  FMC_PCR_TAR_1                     ((uint32_t)0x00004000)        /*!<Bit 1 */
#define  FMC_PCR_TAR_2                     ((uint32_t)0x00008000)        /*!<Bit 2 */
#define  FMC_PCR_TAR_3                     ((uint32_t)0x00010000)        /*!<Bit 3 */

#define  FMC_PCR_ECCPS                     ((uint32_t)0x000E0000)        /*!<ECCPS[2:0] bits (ECC page size)           */
#define  FMC_PCR_ECCPS_0                   ((uint32_t)0x00020000)        /*!<Bit 0 */
#define  FMC_PCR_ECCPS_1                   ((uint32_t)0x00040000)        /*!<Bit 1 */
#define  FMC_PCR_ECCPS_2                   ((uint32_t)0x00080000)        /*!<Bit 2 */

/*******************  Bit definition for FMC_SR register  *******************/
#define  FMC_SR_IRS                        ((uint32_t)0x01)              /*!<Interrupt Rising Edge status                */
#define  FMC_SR_ILS                        ((uint32_t)0x02)              /*!<Interrupt Level status                      */
#define  FMC_SR_IFS                        ((uint32_t)0x04)              /*!<Interrupt Falling Edge status               */
#define  FMC_SR_IREN                       ((uint32_t)0x08)              /*!<Interrupt Rising Edge detection Enable bit  */
#define  FMC_SR_ILEN                       ((uint32_t)0x10)              /*!<Interrupt Level detection Enable bit        */
#define  FMC_SR_IFEN                       ((uint32_t)0x20)              /*!<Interrupt Falling Edge detection Enable bit */
#define  FMC_SR_FEMPT                      ((uint32_t)0x40)              /*!<FIFO empty                                  */

/******************  Bit definition for FMC_PMEM register  ******************/
#define  FMC_PMEM_MEMSET3                  ((uint32_t)0x000000FF)        /*!<MEMSET3[7:0] bits (Common memory 3 setup time) */
#define  FMC_PMEM_MEMSET3_0                ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_PMEM_MEMSET3_1                ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_PMEM_MEMSET3_2                ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_PMEM_MEMSET3_3                ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  FMC_PMEM_MEMSET3_4                ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  FMC_PMEM_MEMSET3_5                ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  FMC_PMEM_MEMSET3_6                ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  FMC_PMEM_MEMSET3_7                ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  FMC_PMEM_MEMWAIT3                 ((uint32_t)0x0000FF00)        /*!<MEMWAIT3[7:0] bits (Common memory 3 wait time) */
#define  FMC_PMEM_MEMWAIT3_0               ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_PMEM_MEMWAIT3_1               ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_PMEM_MEMWAIT3_2               ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_PMEM_MEMWAIT3_3               ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_PMEM_MEMWAIT3_4               ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_PMEM_MEMWAIT3_5               ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_PMEM_MEMWAIT3_6               ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_PMEM_MEMWAIT3_7               ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_PMEM_MEMHOLD3                 ((uint32_t)0x00FF0000)        /*!<MEMHOLD3[7:0] bits (Common memory 3 hold time) */
#define  FMC_PMEM_MEMHOLD3_0               ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_PMEM_MEMHOLD3_1               ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_PMEM_MEMHOLD3_2               ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_PMEM_MEMHOLD3_3               ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  FMC_PMEM_MEMHOLD3_4               ((uint32_t)0x00100000)        /*!<Bit 4 */
#define  FMC_PMEM_MEMHOLD3_5               ((uint32_t)0x00200000)        /*!<Bit 5 */
#define  FMC_PMEM_MEMHOLD3_6               ((uint32_t)0x00400000)        /*!<Bit 6 */
#define  FMC_PMEM_MEMHOLD3_7               ((uint32_t)0x00800000)        /*!<Bit 7 */

#define  FMC_PMEM_MEMHIZ3                  ((uint32_t)0xFF000000)        /*!<MEMHIZ3[7:0] bits (Common memory 3 databus HiZ time) */
#define  FMC_PMEM_MEMHIZ3_0                ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_PMEM_MEMHIZ3_1                ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_PMEM_MEMHIZ3_2                ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_PMEM_MEMHIZ3_3                ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  FMC_PMEM_MEMHIZ3_4                ((uint32_t)0x10000000)        /*!<Bit 4 */
#define  FMC_PMEM_MEMHIZ3_5                ((uint32_t)0x20000000)        /*!<Bit 5 */
#define  FMC_PMEM_MEMHIZ3_6                ((uint32_t)0x40000000)        /*!<Bit 6 */
#define  FMC_PMEM_MEMHIZ3_7                ((uint32_t)0x80000000)        /*!<Bit 7 */

/******************  Bit definition for FMC_PATT register  ******************/
#define  FMC_PATT_ATTSET3                  ((uint32_t)0x000000FF)        /*!<ATTSET3[7:0] bits (Attribute memory 3 setup time) */
#define  FMC_PATT_ATTSET3_0                ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_PATT_ATTSET3_1                ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_PATT_ATTSET3_2                ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_PATT_ATTSET3_3                ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  FMC_PATT_ATTSET3_4                ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  FMC_PATT_ATTSET3_5                ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  FMC_PATT_ATTSET3_6                ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  FMC_PATT_ATTSET3_7                ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  FMC_PATT_ATTWAIT3                 ((uint32_t)0x0000FF00)        /*!<ATTWAIT3[7:0] bits (Attribute memory 3 wait time) */
#define  FMC_PATT_ATTWAIT3_0               ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_PATT_ATTWAIT3_1               ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_PATT_ATTWAIT3_2               ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_PATT_ATTWAIT3_3               ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  FMC_PATT_ATTWAIT3_4               ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  FMC_PATT_ATTWAIT3_5               ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  FMC_PATT_ATTWAIT3_6               ((uint32_t)0x00004000)        /*!<Bit 6 */
#define  FMC_PATT_ATTWAIT3_7               ((uint32_t)0x00008000)        /*!<Bit 7 */

#define  FMC_PATT_ATTHOLD3                 ((uint32_t)0x00FF0000)        /*!<ATTHOLD3[7:0] bits (Attribute memory 3 hold time) */
#define  FMC_PATT_ATTHOLD3_0               ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_PATT_ATTHOLD3_1               ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_PATT_ATTHOLD3_2               ((uint32_t)0x00040000)        /*!<Bit 2 */
#define  FMC_PATT_ATTHOLD3_3               ((uint32_t)0x00080000)        /*!<Bit 3 */
#define  FMC_PATT_ATTHOLD3_4               ((uint32_t)0x00100000)        /*!<Bit 4 */
#define  FMC_PATT_ATTHOLD3_5               ((uint32_t)0x00200000)        /*!<Bit 5 */
#define  FMC_PATT_ATTHOLD3_6               ((uint32_t)0x00400000)        /*!<Bit 6 */
#define  FMC_PATT_ATTHOLD3_7               ((uint32_t)0x00800000)        /*!<Bit 7 */

#define  FMC_PATT_ATTHIZ3                  ((uint32_t)0xFF000000)        /*!<ATTHIZ3[7:0] bits (Attribute memory 3 databus HiZ time) */
#define  FMC_PATT_ATTHIZ3_0                ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_PATT_ATTHIZ3_1                ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_PATT_ATTHIZ3_2                ((uint32_t)0x04000000)        /*!<Bit 2 */
#define  FMC_PATT_ATTHIZ3_3                ((uint32_t)0x08000000)        /*!<Bit 3 */
#define  FMC_PATT_ATTHIZ3_4                ((uint32_t)0x10000000)        /*!<Bit 4 */
#define  FMC_PATT_ATTHIZ3_5                ((uint32_t)0x20000000)        /*!<Bit 5 */
#define  FMC_PATT_ATTHIZ3_6                ((uint32_t)0x40000000)        /*!<Bit 6 */
#define  FMC_PATT_ATTHIZ3_7                ((uint32_t)0x80000000)        /*!<Bit 7 */

/******************  Bit definition for FMC_ECCR register  ******************/
#define  FMC_ECCR_ECC3                     ((uint32_t)0xFFFFFFFF)        /*!<ECC result */

/******************  Bit definition for FMC_SDCR1 register  ******************/
#define  FMC_SDCR1_NC                       ((uint32_t)0x00000003)        /*!<NC[1:0] bits (Number of column bits) */
#define  FMC_SDCR1_NC_0                     ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_SDCR1_NC_1                     ((uint32_t)0x00000002)        /*!<Bit 1 */

#define  FMC_SDCR1_NR                       ((uint32_t)0x0000000C)        /*!<NR[1:0] bits (Number of row bits) */
#define  FMC_SDCR1_NR_0                     ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  FMC_SDCR1_NR_1                     ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  FMC_SDCR1_MWID                     ((uint32_t)0x00000030)        /*!<NR[1:0] bits (Number of row bits) */
#define  FMC_SDCR1_MWID_0                   ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_SDCR1_MWID_1                   ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FMC_SDCR1_NB                       ((uint32_t)0x00000040)        /*!<Number of internal bank */

#define  FMC_SDCR1_CAS                      ((uint32_t)0x00000180)        /*!<CAS[1:0] bits (CAS latency) */
#define  FMC_SDCR1_CAS_0                    ((uint32_t)0x00000080)        /*!<Bit 0 */
#define  FMC_SDCR1_CAS_1                    ((uint32_t)0x00000100)        /*!<Bit 1 */

#define  FMC_SDCR1_WP                       ((uint32_t)0x00000200)        /*!<Write protection */

#define  FMC_SDCR1_SDCLK                    ((uint32_t)0x00000C00)        /*!<SDRAM clock configuration */
#define  FMC_SDCR1_SDCLK_0                  ((uint32_t)0x00000400)        /*!<Bit 0 */
#define  FMC_SDCR1_SDCLK_1                  ((uint32_t)0x00000800)        /*!<Bit 1 */

#define  FMC_SDCR1_RBURST                   ((uint32_t)0x00001000)        /*!<Read burst */

#define  FMC_SDCR1_RPIPE                    ((uint32_t)0x00006000)        /*!<Write protection */
#define  FMC_SDCR1_RPIPE_0                  ((uint32_t)0x00002000)        /*!<Bit 0 */
#define  FMC_SDCR1_RPIPE_1                  ((uint32_t)0x00004000)        /*!<Bit 1 */

/******************  Bit definition for FMC_SDCR2 register  ******************/
#define  FMC_SDCR2_NC                       ((uint32_t)0x00000003)        /*!<NC[1:0] bits (Number of column bits) */
#define  FMC_SDCR2_NC_0                     ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_SDCR2_NC_1                     ((uint32_t)0x00000002)        /*!<Bit 1 */

#define  FMC_SDCR2_NR                       ((uint32_t)0x0000000C)        /*!<NR[1:0] bits (Number of row bits) */
#define  FMC_SDCR2_NR_0                     ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  FMC_SDCR2_NR_1                     ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  FMC_SDCR2_MWID                     ((uint32_t)0x00000030)        /*!<NR[1:0] bits (Number of row bits) */
#define  FMC_SDCR2_MWID_0                   ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_SDCR2_MWID_1                   ((uint32_t)0x00000020)        /*!<Bit 1 */

#define  FMC_SDCR2_NB                       ((uint32_t)0x00000040)        /*!<Number of internal bank */

#define  FMC_SDCR2_CAS                      ((uint32_t)0x00000180)        /*!<CAS[1:0] bits (CAS latency) */
#define  FMC_SDCR2_CAS_0                    ((uint32_t)0x00000080)        /*!<Bit 0 */
#define  FMC_SDCR2_CAS_1                    ((uint32_t)0x00000100)        /*!<Bit 1 */

#define  FMC_SDCR2_WP                       ((uint32_t)0x00000200)        /*!<Write protection */

#define  FMC_SDCR2_SDCLK                    ((uint32_t)0x00000C00)        /*!<SDCLK[1:0] (SDRAM clock configuration) */
#define  FMC_SDCR2_SDCLK_0                  ((uint32_t)0x00000400)        /*!<Bit 0 */
#define  FMC_SDCR2_SDCLK_1                  ((uint32_t)0x00000800)        /*!<Bit 1 */

#define  FMC_SDCR2_RBURST                   ((uint32_t)0x00001000)        /*!<Read burst */

#define  FMC_SDCR2_RPIPE                    ((uint32_t)0x00006000)        /*!<RPIPE[1:0](Read pipe) */
#define  FMC_SDCR2_RPIPE_0                  ((uint32_t)0x00002000)        /*!<Bit 0 */
#define  FMC_SDCR2_RPIPE_1                  ((uint32_t)0x00004000)        /*!<Bit 1 */

/******************  Bit definition for FMC_SDTR1 register  ******************/
#define  FMC_SDTR1_TMRD                     ((uint32_t)0x0000000F)        /*!<TMRD[3:0] bits (Load mode register to active) */
#define  FMC_SDTR1_TMRD_0                   ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_SDTR1_TMRD_1                   ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_SDTR1_TMRD_2                   ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_SDTR1_TMRD_3                   ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_SDTR1_TXSR                     ((uint32_t)0x000000F0)        /*!<TXSR[3:0] bits (Exit self refresh) */
#define  FMC_SDTR1_TXSR_0                   ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_SDTR1_TXSR_1                   ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_SDTR1_TXSR_2                   ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_SDTR1_TXSR_3                   ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_SDTR1_TRAS                     ((uint32_t)0x00000F00)        /*!<TRAS[3:0] bits (Self refresh time) */
#define  FMC_SDTR1_TRAS_0                   ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_SDTR1_TRAS_1                   ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_SDTR1_TRAS_2                   ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_SDTR1_TRAS_3                   ((uint32_t)0x00000800)        /*!<Bit 3 */

#define  FMC_SDTR1_TRC                      ((uint32_t)0x0000F000)        /*!<TRC[2:0] bits (Row cycle delay) */
#define  FMC_SDTR1_TRC_0                    ((uint32_t)0x00001000)        /*!<Bit 0 */
#define  FMC_SDTR1_TRC_1                    ((uint32_t)0x00002000)        /*!<Bit 1 */
#define  FMC_SDTR1_TRC_2                    ((uint32_t)0x00004000)        /*!<Bit 2 */

#define  FMC_SDTR1_TWR                      ((uint32_t)0x000F0000)        /*!<TRC[2:0] bits (Write recovery delay) */
#define  FMC_SDTR1_TWR_0                    ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_SDTR1_TWR_1                    ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_SDTR1_TWR_2                    ((uint32_t)0x00040000)        /*!<Bit 2 */

#define  FMC_SDTR1_TRP                      ((uint32_t)0x00F00000)        /*!<TRP[2:0] bits (Row precharge delay) */
#define  FMC_SDTR1_TRP_0                    ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FMC_SDTR1_TRP_1                    ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FMC_SDTR1_TRP_2                    ((uint32_t)0x00400000)        /*!<Bit 2 */

#define  FMC_SDTR1_TRCD                     ((uint32_t)0x0F000000)        /*!<TRP[2:0] bits (Row to column delay) */
#define  FMC_SDTR1_TRCD_0                   ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_SDTR1_TRCD_1                   ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_SDTR1_TRCD_2                   ((uint32_t)0x04000000)        /*!<Bit 2 */

/******************  Bit definition for FMC_SDTR2 register  ******************/
#define  FMC_SDTR2_TMRD                     ((uint32_t)0x0000000F)        /*!<TMRD[3:0] bits (Load mode register to active) */
#define  FMC_SDTR2_TMRD_0                   ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_SDTR2_TMRD_1                   ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_SDTR2_TMRD_2                   ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  FMC_SDTR2_TMRD_3                   ((uint32_t)0x00000008)        /*!<Bit 3 */

#define  FMC_SDTR2_TXSR                     ((uint32_t)0x000000F0)        /*!<TXSR[3:0] bits (Exit self refresh) */
#define  FMC_SDTR2_TXSR_0                   ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  FMC_SDTR2_TXSR_1                   ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  FMC_SDTR2_TXSR_2                   ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  FMC_SDTR2_TXSR_3                   ((uint32_t)0x00000080)        /*!<Bit 3 */

#define  FMC_SDTR2_TRAS                     ((uint32_t)0x00000F00)        /*!<TRAS[3:0] bits (Self refresh time) */
#define  FMC_SDTR2_TRAS_0                   ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  FMC_SDTR2_TRAS_1                   ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  FMC_SDTR2_TRAS_2                   ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  FMC_SDTR2_TRAS_3                   ((uint32_t)0x00000800)        /*!<Bit 3 */

#define  FMC_SDTR2_TRC                      ((uint32_t)0x0000F000)        /*!<TRC[2:0] bits (Row cycle delay) */
#define  FMC_SDTR2_TRC_0                    ((uint32_t)0x00001000)        /*!<Bit 0 */
#define  FMC_SDTR2_TRC_1                    ((uint32_t)0x00002000)        /*!<Bit 1 */
#define  FMC_SDTR2_TRC_2                    ((uint32_t)0x00004000)        /*!<Bit 2 */

#define  FMC_SDTR2_TWR                      ((uint32_t)0x000F0000)        /*!<TRC[2:0] bits (Write recovery delay) */
#define  FMC_SDTR2_TWR_0                    ((uint32_t)0x00010000)        /*!<Bit 0 */
#define  FMC_SDTR2_TWR_1                    ((uint32_t)0x00020000)        /*!<Bit 1 */
#define  FMC_SDTR2_TWR_2                    ((uint32_t)0x00040000)        /*!<Bit 2 */

#define  FMC_SDTR2_TRP                      ((uint32_t)0x00F00000)        /*!<TRP[2:0] bits (Row precharge delay) */
#define  FMC_SDTR2_TRP_0                    ((uint32_t)0x00100000)        /*!<Bit 0 */
#define  FMC_SDTR2_TRP_1                    ((uint32_t)0x00200000)        /*!<Bit 1 */
#define  FMC_SDTR2_TRP_2                    ((uint32_t)0x00400000)        /*!<Bit 2 */

#define  FMC_SDTR2_TRCD                     ((uint32_t)0x0F000000)        /*!<TRP[2:0] bits (Row to column delay) */
#define  FMC_SDTR2_TRCD_0                   ((uint32_t)0x01000000)        /*!<Bit 0 */
#define  FMC_SDTR2_TRCD_1                   ((uint32_t)0x02000000)        /*!<Bit 1 */
#define  FMC_SDTR2_TRCD_2                   ((uint32_t)0x04000000)        /*!<Bit 2 */

/******************  Bit definition for FMC_SDCMR register  ******************/
#define  FMC_SDCMR_MODE                     ((uint32_t)0x00000007)        /*!<MODE[2:0] bits (Command mode) */
#define  FMC_SDCMR_MODE_0                   ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  FMC_SDCMR_MODE_1                   ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  FMC_SDCMR_MODE_2                   ((uint32_t)0x00000003)        /*!<Bit 2 */

#define  FMC_SDCMR_CTB2                     ((uint32_t)0x00000008)        /*!<Command target 2 */

#define  FMC_SDCMR_CTB1                     ((uint32_t)0x00000010)        /*!<Command target 1 */

#define  FMC_SDCMR_NRFS                     ((uint32_t)0x000001E0)        /*!<NRFS[3:0] bits (Number of auto-refresh) */
#define  FMC_SDCMR_NRFS_0                   ((uint32_t)0x00000020)        /*!<Bit 0 */
#define  FMC_SDCMR_NRFS_1                   ((uint32_t)0x00000040)        /*!<Bit 1 */
#define  FMC_SDCMR_NRFS_2                   ((uint32_t)0x00000080)        /*!<Bit 2 */
#define  FMC_SDCMR_NRFS_3                   ((uint32_t)0x00000100)        /*!<Bit 3 */

#define  FMC_SDCMR_MRD                      ((uint32_t)0x003FFE00)        /*!<MRD[12:0] bits (Mode register definition) */

/******************  Bit definition for FMC_SDRTR register  ******************/
#define  FMC_SDRTR_CRE                      ((uint32_t)0x00000001)        /*!<Clear refresh error flag */

#define  FMC_SDRTR_COUNT                    ((uint32_t)0x00003FFE)        /*!<COUNT[12:0] bits (Refresh timer count) */

#define  FMC_SDRTR_REIE                     ((uint32_t)0x00004000)        /*!<RES interupt enable */

/******************  Bit definition for FMC_SDSR register  ******************/
#define  FMC_SDSR_RE                        ((uint32_t)0x00000001)        /*!<Refresh error flag */

#define  FMC_SDSR_MODES1                    ((uint32_t)0x00000006)        /*!<MODES1[1:0]bits (Status mode for bank 1) */
#define  FMC_SDSR_MODES1_0                  ((uint32_t)0x00000002)        /*!<Bit 0 */
#define  FMC_SDSR_MODES1_1                  ((uint32_t)0x00000004)        /*!<Bit 1 */

#define  FMC_SDSR_MODES2                    ((uint32_t)0x00000018)        /*!<MODES2[1:0]bits (Status mode for bank 2) */
#define  FMC_SDSR_MODES2_0                  ((uint32_t)0x00000008)        /*!<Bit 0 */
#define  FMC_SDSR_MODES2_1                  ((uint32_t)0x00000010)        /*!<Bit 1 */

#define  FMC_SDSR_BUSY                      ((uint32_t)0x00000020)        /*!<Busy status */

/******************************************************************************/
/*                                                                            */
/*                            General Purpose I/O                             */
/*                                                                            */
/******************************************************************************/
/******************  Bits definition for GPIO_MODER register  *****************/
#define GPIO_MODER_MODER0                    ((uint32_t)0x00000003)
#define GPIO_MODER_MODER0_0                  ((uint32_t)0x00000001)
#define GPIO_MODER_MODER0_1                  ((uint32_t)0x00000002)

#define GPIO_MODER_MODER1                    ((uint32_t)0x0000000C)
#define GPIO_MODER_MODER1_0                  ((uint32_t)0x00000004)
#define GPIO_MODER_MODER1_1                  ((uint32_t)0x00000008)

#define GPIO_MODER_MODER2                    ((uint32_t)0x00000030)
#define GPIO_MODER_MODER2_0                  ((uint32_t)0x00000010)
#define GPIO_MODER_MODER2_1                  ((uint32_t)0x00000020)

#define GPIO_MODER_MODER3                    ((uint32_t)0x000000C0)
#define GPIO_MODER_MODER3_0                  ((uint32_t)0x00000040)
#define GPIO_MODER_MODER3_1                  ((uint32_t)0x00000080)

#define GPIO_MODER_MODER4                    ((uint32_t)0x00000300)
#define GPIO_MODER_MODER4_0                  ((uint32_t)0x00000100)
#define GPIO_MODER_MODER4_1                  ((uint32_t)0x00000200)

#define GPIO_MODER_MODER5                    ((uint32_t)0x00000C00)
#define GPIO_MODER_MODER5_0                  ((uint32_t)0x00000400)
#define GPIO_MODER_MODER5_1                  ((uint32_t)0x00000800)

#define GPIO_MODER_MODER6                    ((uint32_t)0x00003000)
#define GPIO_MODER_MODER6_0                  ((uint32_t)0x00001000)
#define GPIO_MODER_MODER6_1                  ((uint32_t)0x00002000)

#define GPIO_MODER_MODER7                    ((uint32_t)0x0000C000)
#define GPIO_MODER_MODER7_0                  ((uint32_t)0x00004000)
#define GPIO_MODER_MODER7_1                  ((uint32_t)0x00008000)

#define GPIO_MODER_MODER8                    ((uint32_t)0x00030000)
#define GPIO_MODER_MODER8_0                  ((uint32_t)0x00010000)
#define GPIO_MODER_MODER8_1                  ((uint32_t)0x00020000)

#define GPIO_MODER_MODER9                    ((uint32_t)0x000C0000)
#define GPIO_MODER_MODER9_0                  ((uint32_t)0x00040000)
#define GPIO_MODER_MODER9_1                  ((uint32_t)0x00080000)

#define GPIO_MODER_MODER10                   ((uint32_t)0x00300000)
#define GPIO_MODER_MODER10_0                 ((uint32_t)0x00100000)
#define GPIO_MODER_MODER10_1                 ((uint32_t)0x00200000)

#define GPIO_MODER_MODER11                   ((uint32_t)0x00C00000)
#define GPIO_MODER_MODER11_0                 ((uint32_t)0x00400000)
#define GPIO_MODER_MODER11_1                 ((uint32_t)0x00800000)

#define GPIO_MODER_MODER12                   ((uint32_t)0x03000000)
#define GPIO_MODER_MODER12_0                 ((uint32_t)0x01000000)
#define GPIO_MODER_MODER12_1                 ((uint32_t)0x02000000)

#define GPIO_MODER_MODER13                   ((uint32_t)0x0C000000)
#define GPIO_MODER_MODER13_0                 ((uint32_t)0x04000000)
#define GPIO_MODER_MODER13_1                 ((uint32_t)0x08000000)

#define GPIO_MODER_MODER14                   ((uint32_t)0x30000000)
#define GPIO_MODER_MODER14_0                 ((uint32_t)0x10000000)
#define GPIO_MODER_MODER14_1                 ((uint32_t)0x20000000)

#define GPIO_MODER_MODER15                   ((uint32_t)0xC0000000)
#define GPIO_MODER_MODER15_0                 ((uint32_t)0x40000000)
#define GPIO_MODER_MODER15_1                 ((uint32_t)0x80000000)

/******************  Bits definition for GPIO_OTYPER register  ****************/
#define GPIO_OTYPER_OT_0                     ((uint32_t)0x00000001)
#define GPIO_OTYPER_OT_1                     ((uint32_t)0x00000002)
#define GPIO_OTYPER_OT_2                     ((uint32_t)0x00000004)
#define GPIO_OTYPER_OT_3                     ((uint32_t)0x00000008)
#define GPIO_OTYPER_OT_4                     ((uint32_t)0x00000010)
#define GPIO_OTYPER_OT_5                     ((uint32_t)0x00000020)
#define GPIO_OTYPER_OT_6                     ((uint32_t)0x00000040)
#define GPIO_OTYPER_OT_7                     ((uint32_t)0x00000080)
#define GPIO_OTYPER_OT_8                     ((uint32_t)0x00000100)
#define GPIO_OTYPER_OT_9                     ((uint32_t)0x00000200)
#define GPIO_OTYPER_OT_10                    ((uint32_t)0x00000400)
#define GPIO_OTYPER_OT_11                    ((uint32_t)0x00000800)
#define GPIO_OTYPER_OT_12                    ((uint32_t)0x00001000)
#define GPIO_OTYPER_OT_13                    ((uint32_t)0x00002000)
#define GPIO_OTYPER_OT_14                    ((uint32_t)0x00004000)
#define GPIO_OTYPER_OT_15                    ((uint32_t)0x00008000)

/******************  Bits definition for GPIO_OSPEEDR register  ***************/
#define GPIO_OSPEEDER_OSPEEDR0               ((uint32_t)0x00000003)
#define GPIO_OSPEEDER_OSPEEDR0_0             ((uint32_t)0x00000001)
#define GPIO_OSPEEDER_OSPEEDR0_1             ((uint32_t)0x00000002)

#define GPIO_OSPEEDER_OSPEEDR1               ((uint32_t)0x0000000C)
#define GPIO_OSPEEDER_OSPEEDR1_0             ((uint32_t)0x00000004)
#define GPIO_OSPEEDER_OSPEEDR1_1             ((uint32_t)0x00000008)

#define GPIO_OSPEEDER_OSPEEDR2               ((uint32_t)0x00000030)
#define GPIO_OSPEEDER_OSPEEDR2_0             ((uint32_t)0x00000010)
#define GPIO_OSPEEDER_OSPEEDR2_1             ((uint32_t)0x00000020)

#define GPIO_OSPEEDER_OSPEEDR3               ((uint32_t)0x000000C0)
#define GPIO_OSPEEDER_OSPEEDR3_0             ((uint32_t)0x00000040)
#define GPIO_OSPEEDER_OSPEEDR3_1             ((uint32_t)0x00000080)

#define GPIO_OSPEEDER_OSPEEDR4               ((uint32_t)0x00000300)
#define GPIO_OSPEEDER_OSPEEDR4_0             ((uint32_t)0x00000100)
#define GPIO_OSPEEDER_OSPEEDR4_1             ((uint32_t)0x00000200)

#define GPIO_OSPEEDER_OSPEEDR5               ((uint32_t)0x00000C00)
#define GPIO_OSPEEDER_OSPEEDR5_0             ((uint32_t)0x00000400)
#define GPIO_OSPEEDER_OSPEEDR5_1             ((uint32_t)0x00000800)

#define GPIO_OSPEEDER_OSPEEDR6               ((uint32_t)0x00003000)
#define GPIO_OSPEEDER_OSPEEDR6_0             ((uint32_t)0x00001000)
#define GPIO_OSPEEDER_OSPEEDR6_1             ((uint32_t)0x00002000)

#define GPIO_OSPEEDER_OSPEEDR7               ((uint32_t)0x0000C000)
#define GPIO_OSPEEDER_OSPEEDR7_0             ((uint32_t)0x00004000)
#define GPIO_OSPEEDER_OSPEEDR7_1             ((uint32_t)0x00008000)

#define GPIO_OSPEEDER_OSPEEDR8               ((uint32_t)0x00030000)
#define GPIO_OSPEEDER_OSPEEDR8_0             ((uint32_t)0x00010000)
#define GPIO_OSPEEDER_OSPEEDR8_1             ((uint32_t)0x00020000)

#define GPIO_OSPEEDER_OSPEEDR9               ((uint32_t)0x000C0000)
#define GPIO_OSPEEDER_OSPEEDR9_0             ((uint32_t)0x00040000)
#define GPIO_OSPEEDER_OSPEEDR9_1             ((uint32_t)0x00080000)

#define GPIO_OSPEEDER_OSPEEDR10              ((uint32_t)0x00300000)
#define GPIO_OSPEEDER_OSPEEDR10_0            ((uint32_t)0x00100000)
#define GPIO_OSPEEDER_OSPEEDR10_1            ((uint32_t)0x00200000)

#define GPIO_OSPEEDER_OSPEEDR11              ((uint32_t)0x00C00000)
#define GPIO_OSPEEDER_OSPEEDR11_0            ((uint32_t)0x00400000)
#define GPIO_OSPEEDER_OSPEEDR11_1            ((uint32_t)0x00800000)

#define GPIO_OSPEEDER_OSPEEDR12              ((uint32_t)0x03000000)
#define GPIO_OSPEEDER_OSPEEDR12_0            ((uint32_t)0x01000000)
#define GPIO_OSPEEDER_OSPEEDR12_1            ((uint32_t)0x02000000)

#define GPIO_OSPEEDER_OSPEEDR13              ((uint32_t)0x0C000000)
#define GPIO_OSPEEDER_OSPEEDR13_0            ((uint32_t)0x04000000)
#define GPIO_OSPEEDER_OSPEEDR13_1            ((uint32_t)0x08000000)

#define GPIO_OSPEEDER_OSPEEDR14              ((uint32_t)0x30000000)
#define GPIO_OSPEEDER_OSPEEDR14_0            ((uint32_t)0x10000000)
#define GPIO_OSPEEDER_OSPEEDR14_1            ((uint32_t)0x20000000)

#define GPIO_OSPEEDER_OSPEEDR15              ((uint32_t)0xC0000000)
#define GPIO_OSPEEDER_OSPEEDR15_0            ((uint32_t)0x40000000)
#define GPIO_OSPEEDER_OSPEEDR15_1            ((uint32_t)0x80000000)

/******************  Bits definition for GPIO_PUPDR register  *****************/
#define GPIO_PUPDR_PUPDR0                    ((uint32_t)0x00000003)
#define GPIO_PUPDR_PUPDR0_0                  ((uint32_t)0x00000001)
#define GPIO_PUPDR_PUPDR0_1                  ((uint32_t)0x00000002)

#define GPIO_PUPDR_PUPDR1                    ((uint32_t)0x0000000C)
#define GPIO_PUPDR_PUPDR1_0                  ((uint32_t)0x00000004)
#define GPIO_PUPDR_PUPDR1_1                  ((uint32_t)0x00000008)

#define GPIO_PUPDR_PUPDR2                    ((uint32_t)0x00000030)
#define GPIO_PUPDR_PUPDR2_0                  ((uint32_t)0x00000010)
#define GPIO_PUPDR_PUPDR2_1                  ((uint32_t)0x00000020)

#define GPIO_PUPDR_PUPDR3                    ((uint32_t)0x000000C0)
#define GPIO_PUPDR_PUPDR3_0                  ((uint32_t)0x00000040)
#define GPIO_PUPDR_PUPDR3_1                  ((uint32_t)0x00000080)

#define GPIO_PUPDR_PUPDR4                    ((uint32_t)0x00000300)
#define GPIO_PUPDR_PUPDR4_0                  ((uint32_t)0x00000100)
#define GPIO_PUPDR_PUPDR4_1                  ((uint32_t)0x00000200)

#define GPIO_PUPDR_PUPDR5                    ((uint32_t)0x00000C00)
#define GPIO_PUPDR_PUPDR5_0                  ((uint32_t)0x00000400)
#define GPIO_PUPDR_PUPDR5_1                  ((uint32_t)0x00000800)

#define GPIO_PUPDR_PUPDR6                    ((uint32_t)0x00003000)
#define GPIO_PUPDR_PUPDR6_0                  ((uint32_t)0x00001000)
#define GPIO_PUPDR_PUPDR6_1                  ((uint32_t)0x00002000)

#define GPIO_PUPDR_PUPDR7                    ((uint32_t)0x0000C000)
#define GPIO_PUPDR_PUPDR7_0                  ((uint32_t)0x00004000)
#define GPIO_PUPDR_PUPDR7_1                  ((uint32_t)0x00008000)

#define GPIO_PUPDR_PUPDR8                    ((uint32_t)0x00030000)
#define GPIO_PUPDR_PUPDR8_0                  ((uint32_t)0x00010000)
#define GPIO_PUPDR_PUPDR8_1                  ((uint32_t)0x00020000)

#define GPIO_PUPDR_PUPDR9                    ((uint32_t)0x000C0000)
#define GPIO_PUPDR_PUPDR9_0                  ((uint32_t)0x00040000)
#define GPIO_PUPDR_PUPDR9_1                  ((uint32_t)0x00080000)

#define GPIO_PUPDR_PUPDR10                   ((uint32_t)0x00300000)
#define GPIO_PUPDR_PUPDR10_0                 ((uint32_t)0x00100000)
#define GPIO_PUPDR_PUPDR10_1                 ((uint32_t)0x00200000)

#define GPIO_PUPDR_PUPDR11                   ((uint32_t)0x00C00000)
#define GPIO_PUPDR_PUPDR11_0                 ((uint32_t)0x00400000)
#define GPIO_PUPDR_PUPDR11_1                 ((uint32_t)0x00800000)

#define GPIO_PUPDR_PUPDR12                   ((uint32_t)0x03000000)
#define GPIO_PUPDR_PUPDR12_0                 ((uint32_t)0x01000000)
#define GPIO_PUPDR_PUPDR12_1                 ((uint32_t)0x02000000)

#define GPIO_PUPDR_PUPDR13                   ((uint32_t)0x0C000000)
#define GPIO_PUPDR_PUPDR13_0                 ((uint32_t)0x04000000)
#define GPIO_PUPDR_PUPDR13_1                 ((uint32_t)0x08000000)

#define GPIO_PUPDR_PUPDR14                   ((uint32_t)0x30000000)
#define GPIO_PUPDR_PUPDR14_0                 ((uint32_t)0x10000000)
#define GPIO_PUPDR_PUPDR14_1                 ((uint32_t)0x20000000)

#define GPIO_PUPDR_PUPDR15                   ((uint32_t)0xC0000000)
#define GPIO_PUPDR_PUPDR15_0                 ((uint32_t)0x40000000)
#define GPIO_PUPDR_PUPDR15_1                 ((uint32_t)0x80000000)

/******************  Bits definition for GPIO_IDR register  *******************/
#define GPIO_IDR_IDR_0                       ((uint32_t)0x00000001)
#define GPIO_IDR_IDR_1                       ((uint32_t)0x00000002)
#define GPIO_IDR_IDR_2                       ((uint32_t)0x00000004)
#define GPIO_IDR_IDR_3                       ((uint32_t)0x00000008)
#define GPIO_IDR_IDR_4                       ((uint32_t)0x00000010)
#define GPIO_IDR_IDR_5                       ((uint32_t)0x00000020)
#define GPIO_IDR_IDR_6                       ((uint32_t)0x00000040)
#define GPIO_IDR_IDR_7                       ((uint32_t)0x00000080)
#define GPIO_IDR_IDR_8                       ((uint32_t)0x00000100)
#define GPIO_IDR_IDR_9                       ((uint32_t)0x00000200)
#define GPIO_IDR_IDR_10                      ((uint32_t)0x00000400)
#define GPIO_IDR_IDR_11                      ((uint32_t)0x00000800)
#define GPIO_IDR_IDR_12                      ((uint32_t)0x00001000)
#define GPIO_IDR_IDR_13                      ((uint32_t)0x00002000)
#define GPIO_IDR_IDR_14                      ((uint32_t)0x00004000)
#define GPIO_IDR_IDR_15                      ((uint32_t)0x00008000)

/******************  Bits definition for GPIO_ODR register  *******************/
#define GPIO_ODR_ODR_0                       ((uint32_t)0x00000001)
#define GPIO_ODR_ODR_1                       ((uint32_t)0x00000002)
#define GPIO_ODR_ODR_2                       ((uint32_t)0x00000004)
#define GPIO_ODR_ODR_3                       ((uint32_t)0x00000008)
#define GPIO_ODR_ODR_4                       ((uint32_t)0x00000010)
#define GPIO_ODR_ODR_5                       ((uint32_t)0x00000020)
#define GPIO_ODR_ODR_6                       ((uint32_t)0x00000040)
#define GPIO_ODR_ODR_7                       ((uint32_t)0x00000080)
#define GPIO_ODR_ODR_8                       ((uint32_t)0x00000100)
#define GPIO_ODR_ODR_9                       ((uint32_t)0x00000200)
#define GPIO_ODR_ODR_10                      ((uint32_t)0x00000400)
#define GPIO_ODR_ODR_11                      ((uint32_t)0x00000800)
#define GPIO_ODR_ODR_12                      ((uint32_t)0x00001000)
#define GPIO_ODR_ODR_13                      ((uint32_t)0x00002000)
#define GPIO_ODR_ODR_14                      ((uint32_t)0x00004000)
#define GPIO_ODR_ODR_15                      ((uint32_t)0x00008000)

/******************  Bits definition for GPIO_BSRR register  ******************/
#define GPIO_BSRR_BS_0                       ((uint32_t)0x00000001)
#define GPIO_BSRR_BS_1                       ((uint32_t)0x00000002)
#define GPIO_BSRR_BS_2                       ((uint32_t)0x00000004)
#define GPIO_BSRR_BS_3                       ((uint32_t)0x00000008)
#define GPIO_BSRR_BS_4                       ((uint32_t)0x00000010)
#define GPIO_BSRR_BS_5                       ((uint32_t)0x00000020)
#define GPIO_BSRR_BS_6                       ((uint32_t)0x00000040)
#define GPIO_BSRR_BS_7                       ((uint32_t)0x00000080)
#define GPIO_BSRR_BS_8                       ((uint32_t)0x00000100)
#define GPIO_BSRR_BS_9                       ((uint32_t)0x00000200)
#define GPIO_BSRR_BS_10                      ((uint32_t)0x00000400)
#define GPIO_BSRR_BS_11                      ((uint32_t)0x00000800)
#define GPIO_BSRR_BS_12                      ((uint32_t)0x00001000)
#define GPIO_BSRR_BS_13                      ((uint32_t)0x00002000)
#define GPIO_BSRR_BS_14                      ((uint32_t)0x00004000)
#define GPIO_BSRR_BS_15                      ((uint32_t)0x00008000)
#define GPIO_BSRR_BR_0                       ((uint32_t)0x00010000)
#define GPIO_BSRR_BR_1                       ((uint32_t)0x00020000)
#define GPIO_BSRR_BR_2                       ((uint32_t)0x00040000)
#define GPIO_BSRR_BR_3                       ((uint32_t)0x00080000)
#define GPIO_BSRR_BR_4                       ((uint32_t)0x00100000)
#define GPIO_BSRR_BR_5                       ((uint32_t)0x00200000)
#define GPIO_BSRR_BR_6                       ((uint32_t)0x00400000)
#define GPIO_BSRR_BR_7                       ((uint32_t)0x00800000)
#define GPIO_BSRR_BR_8                       ((uint32_t)0x01000000)
#define GPIO_BSRR_BR_9                       ((uint32_t)0x02000000)
#define GPIO_BSRR_BR_10                      ((uint32_t)0x04000000)
#define GPIO_BSRR_BR_11                      ((uint32_t)0x08000000)
#define GPIO_BSRR_BR_12                      ((uint32_t)0x10000000)
#define GPIO_BSRR_BR_13                      ((uint32_t)0x20000000)
#define GPIO_BSRR_BR_14                      ((uint32_t)0x40000000)
#define GPIO_BSRR_BR_15                      ((uint32_t)0x80000000)

/****************** Bit definition for GPIO_LCKR register *********************/
#define GPIO_LCKR_LCK0                       ((uint32_t)0x00000001)
#define GPIO_LCKR_LCK1                       ((uint32_t)0x00000002)
#define GPIO_LCKR_LCK2                       ((uint32_t)0x00000004)
#define GPIO_LCKR_LCK3                       ((uint32_t)0x00000008)
#define GPIO_LCKR_LCK4                       ((uint32_t)0x00000010)
#define GPIO_LCKR_LCK5                       ((uint32_t)0x00000020)
#define GPIO_LCKR_LCK6                       ((uint32_t)0x00000040)
#define GPIO_LCKR_LCK7                       ((uint32_t)0x00000080)
#define GPIO_LCKR_LCK8                       ((uint32_t)0x00000100)
#define GPIO_LCKR_LCK9                       ((uint32_t)0x00000200)
#define GPIO_LCKR_LCK10                      ((uint32_t)0x00000400)
#define GPIO_LCKR_LCK11                      ((uint32_t)0x00000800)
#define GPIO_LCKR_LCK12                      ((uint32_t)0x00001000)
#define GPIO_LCKR_LCK13                      ((uint32_t)0x00002000)
#define GPIO_LCKR_LCK14                      ((uint32_t)0x00004000)
#define GPIO_LCKR_LCK15                      ((uint32_t)0x00008000)
#define GPIO_LCKR_LCKK                       ((uint32_t)0x00010000)

/******************************************************************************/
/*                                                                            */
/*                      Inter-integrated Circuit Interface (I2C)              */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for I2C_CR1 register  *******************/
#define  I2C_CR1_PE                          ((uint32_t)0x00000001)        /*!< Peripheral enable                   */
#define  I2C_CR1_TXIE                        ((uint32_t)0x00000002)        /*!< TX interrupt enable                 */
#define  I2C_CR1_RXIE                        ((uint32_t)0x00000004)        /*!< RX interrupt enable                 */
#define  I2C_CR1_ADDRIE                      ((uint32_t)0x00000008)        /*!< Address match interrupt enable      */
#define  I2C_CR1_NACKIE                      ((uint32_t)0x00000010)        /*!< NACK received interrupt enable      */
#define  I2C_CR1_STOPIE                      ((uint32_t)0x00000020)        /*!< STOP detection interrupt enable     */
#define  I2C_CR1_TCIE                        ((uint32_t)0x00000040)        /*!< Transfer complete interrupt enable  */
#define  I2C_CR1_ERRIE                       ((uint32_t)0x00000080)        /*!< Errors interrupt enable             */
#define  I2C_CR1_DFN                         ((uint32_t)0x00000F00)        /*!< Digital noise filter                */
#define  I2C_CR1_ANFOFF                      ((uint32_t)0x00001000)        /*!< Analog noise filter OFF             */
#define  I2C_CR1_SWRST                       ((uint32_t)0x00002000)        /*!< Software reset                      */
#define  I2C_CR1_TXDMAEN                     ((uint32_t)0x00004000)        /*!< DMA transmission requests enable    */
#define  I2C_CR1_RXDMAEN                     ((uint32_t)0x00008000)        /*!< DMA reception requests enable       */
#define  I2C_CR1_SBC                         ((uint32_t)0x00010000)        /*!< Slave byte control                  */
#define  I2C_CR1_NOSTRETCH                   ((uint32_t)0x00020000)        /*!< Clock stretching disable            */
#define  I2C_CR1_WUPEN                       ((uint32_t)0x00040000)        /*!< Wakeup from STOP enable             */
#define  I2C_CR1_GCEN                        ((uint32_t)0x00080000)        /*!< General call enable                 */
#define  I2C_CR1_SMBHEN                      ((uint32_t)0x00100000)        /*!< SMBus host address enable           */
#define  I2C_CR1_SMBDEN                      ((uint32_t)0x00200000)        /*!< SMBus device default address enable */
#define  I2C_CR1_ALERTEN                     ((uint32_t)0x00400000)        /*!< SMBus alert enable                  */
#define  I2C_CR1_PECEN                       ((uint32_t)0x00800000)        /*!< PEC enable                          */

/******************  Bit definition for I2C_CR2 register  ********************/
#define  I2C_CR2_SADD                        ((uint32_t)0x000003FF)        /*!< Slave address (master mode)                             */
#define  I2C_CR2_RD_WRN                      ((uint32_t)0x00000400)        /*!< Transfer direction (master mode)                        */
#define  I2C_CR2_ADD10                       ((uint32_t)0x00000800)        /*!< 10-bit addressing mode (master mode)                    */
#define  I2C_CR2_HEAD10R                     ((uint32_t)0x00001000)        /*!< 10-bit address header only read direction (master mode) */
#define  I2C_CR2_START                       ((uint32_t)0x00002000)        /*!< START generation                                        */
#define  I2C_CR2_STOP                        ((uint32_t)0x00004000)        /*!< STOP generation (master mode)                           */
#define  I2C_CR2_NACK                        ((uint32_t)0x00008000)        /*!< NACK generation (slave mode)                            */
#define  I2C_CR2_NBYTES                      ((uint32_t)0x00FF0000)        /*!< Number of bytes                                         */
#define  I2C_CR2_RELOAD                      ((uint32_t)0x01000000)        /*!< NBYTES reload mode                                      */
#define  I2C_CR2_AUTOEND                     ((uint32_t)0x02000000)        /*!< Automatic end mode (master mode)                        */
#define  I2C_CR2_PECBYTE                     ((uint32_t)0x04000000)        /*!< Packet error checking byte                              */

/*******************  Bit definition for I2C_OAR1 register  ******************/
#define  I2C_OAR1_OA1                        ((uint32_t)0x000003FF)        /*!< Interface own address 1   */
#define  I2C_OAR1_OA1MODE                    ((uint32_t)0x00000400)        /*!< Own address 1 10-bit mode */
#define  I2C_OAR1_OA1EN                      ((uint32_t)0x00008000)        /*!< Own address 1 enable      */

/*******************  Bit definition for I2C_OAR2 register  ******************/
#define  I2C_OAR2_OA2                        ((uint32_t)0x000000FE)        /*!< Interface own address 2 */
#define  I2C_OAR2_OA2MSK                     ((uint32_t)0x00000700)        /*!< Own address 2 masks     */
#define  I2C_OAR2_OA2NOMASK                  ((uint32_t)0x00000000)        /*!< No mask */
#define  I2C_OAR2_OA2MASK01                  ((uint32_t)0x00000100)        /*!< OA2[1] is masked, Only OA2[7:2] are compared */
#define  I2C_OAR2_OA2MASK02                  ((uint32_t)0x00000200)        /*!< OA2[2:1] is masked, Only OA2[7:3] are compared */
#define  I2C_OAR2_OA2MASK03                  ((uint32_t)0x00000300)        /*!< OA2[3:1] is masked, Only OA2[7:4] are compared */
#define  I2C_OAR2_OA2MASK04                  ((uint32_t)0x00000400)        /*!< OA2[4:1] is masked, Only OA2[7:5] are compared */
#define  I2C_OAR2_OA2MASK05                  ((uint32_t)0x00000500)        /*!< OA2[5:1] is masked, Only OA2[7:6] are compared */
#define  I2C_OAR2_OA2MASK06                  ((uint32_t)0x00000600)        /*!< OA2[6:1] is masked, Only OA2[7] are compared */
#define  I2C_OAR2_OA2MASK07                  ((uint32_t)0x00000700)        /*!< OA2[7:1] is masked, No comparison is done */
#define  I2C_OAR2_OA2EN                      ((uint32_t)0x00008000)        /*!< Own address 2 enable    */

/*******************  Bit definition for I2C_TIMINGR register *******************/
#define  I2C_TIMINGR_SCLL                    ((uint32_t)0x000000FF)        /*!< SCL low period (master mode)  */
#define  I2C_TIMINGR_SCLH                    ((uint32_t)0x0000FF00)        /*!< SCL high period (master mode) */
#define  I2C_TIMINGR_SDADEL                  ((uint32_t)0x000F0000)        /*!< Data hold time                */
#define  I2C_TIMINGR_SCLDEL                  ((uint32_t)0x00F00000)        /*!< Data setup time               */
#define  I2C_TIMINGR_PRESC                   ((uint32_t)0xF0000000)        /*!< Timings prescaler             */

/******************* Bit definition for I2C_TIMEOUTR register *******************/
#define  I2C_TIMEOUTR_TIMEOUTA               ((uint32_t)0x00000FFF)        /*!< Bus timeout A                 */
#define  I2C_TIMEOUTR_TIDLE                  ((uint32_t)0x00001000)        /*!< Idle clock timeout detection  */
#define  I2C_TIMEOUTR_TIMOUTEN               ((uint32_t)0x00008000)        /*!< Clock timeout enable          */
#define  I2C_TIMEOUTR_TIMEOUTB               ((uint32_t)0x0FFF0000)        /*!< Bus timeout B                 */
#define  I2C_TIMEOUTR_TEXTEN                 ((uint32_t)0x80000000)        /*!< Extended clock timeout enable */

/******************  Bit definition for I2C_ISR register  *********************/
#define  I2C_ISR_TXE                         ((uint32_t)0x00000001)        /*!< Transmit data register empty    */
#define  I2C_ISR_TXIS                        ((uint32_t)0x00000002)        /*!< Transmit interrupt status       */
#define  I2C_ISR_RXNE                        ((uint32_t)0x00000004)        /*!< Receive data register not empty */
#define  I2C_ISR_ADDR                        ((uint32_t)0x00000008)        /*!< Address matched (slave mode)    */
#define  I2C_ISR_NACKF                       ((uint32_t)0x00000010)        /*!< NACK received flag              */
#define  I2C_ISR_STOPF                       ((uint32_t)0x00000020)        /*!< STOP detection flag             */
#define  I2C_ISR_TC                          ((uint32_t)0x00000040)        /*!< Transfer complete (master mode) */
#define  I2C_ISR_TCR                         ((uint32_t)0x00000080)        /*!< Transfer complete reload        */
#define  I2C_ISR_BERR                        ((uint32_t)0x00000100)        /*!< Bus error                       */
#define  I2C_ISR_ARLO                        ((uint32_t)0x00000200)        /*!< Arbitration lost                */
#define  I2C_ISR_OVR                         ((uint32_t)0x00000400)        /*!< Overrun/Underrun                */
#define  I2C_ISR_PECERR                      ((uint32_t)0x00000800)        /*!< PEC error in reception          */
#define  I2C_ISR_TIMEOUT                     ((uint32_t)0x00001000)        /*!< Timeout or Tlow detection flag  */
#define  I2C_ISR_ALERT                       ((uint32_t)0x00002000)        /*!< SMBus alert                     */
#define  I2C_ISR_BUSY                        ((uint32_t)0x00008000)        /*!< Bus busy                        */
#define  I2C_ISR_DIR                         ((uint32_t)0x00010000)        /*!< Transfer direction (slave mode) */
#define  I2C_ISR_ADDCODE                     ((uint32_t)0x00FE0000)        /*!< Address match code (slave mode) */

/******************  Bit definition for I2C_ICR register  *********************/
#define  I2C_ICR_ADDRCF                      ((uint32_t)0x00000008)        /*!< Address matched clear flag      */
#define  I2C_ICR_NACKCF                      ((uint32_t)0x00000010)        /*!< NACK clear flag                 */
#define  I2C_ICR_STOPCF                      ((uint32_t)0x00000020)        /*!< STOP detection clear flag       */
#define  I2C_ICR_BERRCF                      ((uint32_t)0x00000100)        /*!< Bus error clear flag            */
#define  I2C_ICR_ARLOCF                      ((uint32_t)0x00000200)        /*!< Arbitration lost clear flag     */
#define  I2C_ICR_OVRCF                       ((uint32_t)0x00000400)        /*!< Overrun/Underrun clear flag     */
#define  I2C_ICR_PECCF                       ((uint32_t)0x00000800)        /*!< PAC error clear flag            */
#define  I2C_ICR_TIMOUTCF                    ((uint32_t)0x00001000)        /*!< Timeout clear flag              */
#define  I2C_ICR_ALERTCF                     ((uint32_t)0x00002000)        /*!< Alert clear flag                */

/******************  Bit definition for I2C_PECR register  *********************/
#define  I2C_PECR_PEC                        ((uint32_t)0x000000FF)        /*!< PEC register        */

/******************  Bit definition for I2C_RXDR register  *********************/
#define  I2C_RXDR_RXDATA                     ((uint32_t)0x000000FF)        /*!< 8-bit receive data  */

/******************  Bit definition for I2C_TXDR register  *********************/
#define  I2C_TXDR_TXDATA                     ((uint32_t)0x000000FF)        /*!< 8-bit transmit data */


/******************************************************************************/
/*                                                                            */
/*                           Independent WATCHDOG                             */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for IWDG_KR register  ********************/
#define  IWDG_KR_KEY                         ((uint32_t)0xFFFF)            /*!<Key value (write only, read 0000h)  */

/*******************  Bit definition for IWDG_PR register  ********************/
#define  IWDG_PR_PR                          ((uint32_t)0x07)               /*!<PR[2:0] (Prescaler divider)         */
#define  IWDG_PR_PR_0                        ((uint32_t)0x01)               /*!<Bit 0 */
#define  IWDG_PR_PR_1                        ((uint32_t)0x02)               /*!<Bit 1 */
#define  IWDG_PR_PR_2                        ((uint32_t)0x04)               /*!<Bit 2 */

/*******************  Bit definition for IWDG_RLR register  *******************/
#define  IWDG_RLR_RL                         ((uint32_t)0x0FFF)            /*!<Watchdog counter reload value        */

/*******************  Bit definition for IWDG_SR register  ********************/
#define  IWDG_SR_PVU                         ((uint32_t)0x01)               /*!< Watchdog prescaler value update */
#define  IWDG_SR_RVU                         ((uint32_t)0x02)               /*!< Watchdog counter reload value update */
#define  IWDG_SR_WVU                         ((uint32_t)0x04)               /*!< Watchdog counter window value update */

/*******************  Bit definition for IWDG_KR register  ********************/
#define  IWDG_WINR_WIN                       ((uint32_t)0x0FFF)             /*!< Watchdog counter window value */

/******************************************************************************/
/*                                                                            */
/*                      LCD-TFT Display Controller (LTDC)                     */
/*                                                                            */
/******************************************************************************/

/********************  Bit definition for LTDC_SSCR register  *****************/

#define LTDC_SSCR_VSH                       ((uint32_t)0x000007FF)              /*!< Vertical Synchronization Height  */
#define LTDC_SSCR_HSW                       ((uint32_t)0x0FFF0000)              /*!< Horizontal Synchronization Width */

/********************  Bit definition for LTDC_BPCR register  *****************/

#define LTDC_BPCR_AVBP                      ((uint32_t)0x000007FF)              /*!< Accumulated Vertical Back Porch   */
#define LTDC_BPCR_AHBP                      ((uint32_t)0x0FFF0000)              /*!< Accumulated Horizontal Back Porch */

/********************  Bit definition for LTDC_AWCR register  *****************/

#define LTDC_AWCR_AAH                       ((uint32_t)0x000007FF)              /*!< Accumulated Active heigh */
#define LTDC_AWCR_AAW                       ((uint32_t)0x0FFF0000)              /*!< Accumulated Active Width */

/********************  Bit definition for LTDC_TWCR register  *****************/

#define LTDC_TWCR_TOTALH                    ((uint32_t)0x000007FF)              /*!< Total Heigh */
#define LTDC_TWCR_TOTALW                    ((uint32_t)0x0FFF0000)              /*!< Total Width */

/********************  Bit definition for LTDC_GCR register  ******************/

#define LTDC_GCR_LTDCEN                     ((uint32_t)0x00000001)              /*!< LCD-TFT controller enable bit       */
#define LTDC_GCR_DBW                        ((uint32_t)0x00000070)              /*!< Dither Blue Width                   */
#define LTDC_GCR_DGW                        ((uint32_t)0x00000700)              /*!< Dither Green Width                  */
#define LTDC_GCR_DRW                        ((uint32_t)0x00007000)              /*!< Dither Red Width                    */
#define LTDC_GCR_DTEN                       ((uint32_t)0x00010000)              /*!< Dither Enable                       */
#define LTDC_GCR_PCPOL                      ((uint32_t)0x10000000)              /*!< Pixel Clock Polarity                */
#define LTDC_GCR_DEPOL                      ((uint32_t)0x20000000)              /*!< Data Enable Polarity                */
#define LTDC_GCR_VSPOL                      ((uint32_t)0x40000000)              /*!< Vertical Synchronization Polarity   */
#define LTDC_GCR_HSPOL                      ((uint32_t)0x80000000)              /*!< Horizontal Synchronization Polarity */

/********************  Bit definition for LTDC_SRCR register  *****************/

#define LTDC_SRCR_IMR                      ((uint32_t)0x00000001)               /*!< Immediate Reload         */
#define LTDC_SRCR_VBR                      ((uint32_t)0x00000002)               /*!< Vertical Blanking Reload */

/********************  Bit definition for LTDC_BCCR register  *****************/

#define LTDC_BCCR_BCBLUE                    ((uint32_t)0x000000FF)              /*!< Background Blue value  */
#define LTDC_BCCR_BCGREEN                   ((uint32_t)0x0000FF00)              /*!< Background Green value */
#define LTDC_BCCR_BCRED                     ((uint32_t)0x00FF0000)              /*!< Background Red value   */

/********************  Bit definition for LTDC_IER register  ******************/

#define LTDC_IER_LIE                        ((uint32_t)0x00000001)              /*!< Line Interrupt Enable            */
#define LTDC_IER_FUIE                       ((uint32_t)0x00000002)              /*!< FIFO Underrun Interrupt Enable   */
#define LTDC_IER_TERRIE                     ((uint32_t)0x00000004)              /*!< Transfer Error Interrupt Enable  */
#define LTDC_IER_RRIE                       ((uint32_t)0x00000008)              /*!< Register Reload interrupt enable */

/********************  Bit definition for LTDC_ISR register  ******************/

#define LTDC_ISR_LIF                        ((uint32_t)0x00000001)              /*!< Line Interrupt Flag */
#define LTDC_ISR_FUIF                       ((uint32_t)0x00000002)              /*!< FIFO Underrun Interrupt Flag */
#define LTDC_ISR_TERRIF                     ((uint32_t)0x00000004)              /*!< Transfer Error Interrupt Flag */
#define LTDC_ISR_RRIF                       ((uint32_t)0x00000008)              /*!< Register Reload interrupt Flag */

/********************  Bit definition for LTDC_ICR register  ******************/

#define LTDC_ICR_CLIF                       ((uint32_t)0x00000001)              /*!< Clears the Line Interrupt Flag */
#define LTDC_ICR_CFUIF                      ((uint32_t)0x00000002)              /*!< Clears the FIFO Underrun Interrupt Flag */
#define LTDC_ICR_CTERRIF                    ((uint32_t)0x00000004)              /*!< Clears the Transfer Error Interrupt Flag */
#define LTDC_ICR_CRRIF                      ((uint32_t)0x00000008)              /*!< Clears Register Reload interrupt Flag */

/********************  Bit definition for LTDC_LIPCR register  ****************/

#define LTDC_LIPCR_LIPOS                    ((uint32_t)0x000007FF)              /*!< Line Interrupt Position */

/********************  Bit definition for LTDC_CPSR register  *****************/

#define LTDC_CPSR_CYPOS                     ((uint32_t)0x0000FFFF)              /*!< Current Y Position */
#define LTDC_CPSR_CXPOS                     ((uint32_t)0xFFFF0000)              /*!< Current X Position */

/********************  Bit definition for LTDC_CDSR register  *****************/

#define LTDC_CDSR_VDES                      ((uint32_t)0x00000001)              /*!< Vertical Data Enable Status       */
#define LTDC_CDSR_HDES                      ((uint32_t)0x00000002)              /*!< Horizontal Data Enable Status     */
#define LTDC_CDSR_VSYNCS                    ((uint32_t)0x00000004)              /*!< Vertical Synchronization Status   */
#define LTDC_CDSR_HSYNCS                    ((uint32_t)0x00000008)              /*!< Horizontal Synchronization Status */

/********************  Bit definition for LTDC_LxCR register  *****************/

#define LTDC_LxCR_LEN                       ((uint32_t)0x00000001)              /*!< Layer Enable              */
#define LTDC_LxCR_COLKEN                    ((uint32_t)0x00000002)              /*!< Color Keying Enable       */
#define LTDC_LxCR_CLUTEN                    ((uint32_t)0x00000010)              /*!< Color Lockup Table Enable */

/********************  Bit definition for LTDC_LxWHPCR register  **************/

#define LTDC_LxWHPCR_WHSTPOS                ((uint32_t)0x00000FFF)              /*!< Window Horizontal Start Position */
#define LTDC_LxWHPCR_WHSPPOS                ((uint32_t)0xFFFF0000)              /*!< Window Horizontal Stop Position  */

/********************  Bit definition for LTDC_LxWVPCR register  **************/

#define LTDC_LxWVPCR_WVSTPOS                ((uint32_t)0x00000FFF)              /*!< Window Vertical Start Position */
#define LTDC_LxWVPCR_WVSPPOS                ((uint32_t)0xFFFF0000)              /*!< Window Vertical Stop Position  */

/********************  Bit definition for LTDC_LxCKCR register  ***************/

#define LTDC_LxCKCR_CKBLUE                  ((uint32_t)0x000000FF)              /*!< Color Key Blue value  */
#define LTDC_LxCKCR_CKGREEN                 ((uint32_t)0x0000FF00)              /*!< Color Key Green value */
#define LTDC_LxCKCR_CKRED                   ((uint32_t)0x00FF0000)              /*!< Color Key Red value   */

/********************  Bit definition for LTDC_LxPFCR register  ***************/

#define LTDC_LxPFCR_PF                      ((uint32_t)0x00000007)              /*!< Pixel Format */

/********************  Bit definition for LTDC_LxCACR register  ***************/

#define LTDC_LxCACR_CONSTA                  ((uint32_t)0x000000FF)              /*!< Constant Alpha */

/********************  Bit definition for LTDC_LxDCCR register  ***************/

#define LTDC_LxDCCR_DCBLUE                  ((uint32_t)0x000000FF)              /*!< Default Color Blue  */
#define LTDC_LxDCCR_DCGREEN                 ((uint32_t)0x0000FF00)              /*!< Default Color Green */
#define LTDC_LxDCCR_DCRED                   ((uint32_t)0x00FF0000)              /*!< Default Color Red   */
#define LTDC_LxDCCR_DCALPHA                 ((uint32_t)0xFF000000)              /*!< Default Color Alpha */

/********************  Bit definition for LTDC_LxBFCR register  ***************/

#define LTDC_LxBFCR_BF2                     ((uint32_t)0x00000007)              /*!< Blending Factor 2 */
#define LTDC_LxBFCR_BF1                     ((uint32_t)0x00000700)              /*!< Blending Factor 1 */

/********************  Bit definition for LTDC_LxCFBAR register  **************/

#define LTDC_LxCFBAR_CFBADD                 ((uint32_t)0xFFFFFFFF)              /*!< Color Frame Buffer Start Address */

/********************  Bit definition for LTDC_LxCFBLR register  **************/

#define LTDC_LxCFBLR_CFBLL                  ((uint32_t)0x00001FFF)              /*!< Color Frame Buffer Line Length    */
#define LTDC_LxCFBLR_CFBP                   ((uint32_t)0x1FFF0000)              /*!< Color Frame Buffer Pitch in bytes */

/********************  Bit definition for LTDC_LxCFBLNR register  *************/

#define LTDC_LxCFBLNR_CFBLNBR               ((uint32_t)0x000007FF)              /*!< Frame Buffer Line Number */

/********************  Bit definition for LTDC_LxCLUTWR register  *************/

#define LTDC_LxCLUTWR_BLUE                  ((uint32_t)0x000000FF)              /*!< Blue value   */
#define LTDC_LxCLUTWR_GREEN                 ((uint32_t)0x0000FF00)              /*!< Green value  */
#define LTDC_LxCLUTWR_RED                   ((uint32_t)0x00FF0000)              /*!< Red value    */
#define LTDC_LxCLUTWR_CLUTADD               ((uint32_t)0xFF000000)              /*!< CLUT address */


/******************************************************************************/
/*                                                                            */
/*                             Power Control                                  */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for PWR_CR1 register  ********************/
#define  PWR_CR1_LPDS                         ((uint32_t)0x00000001)     /*!< Low-Power Deepsleep                 */
#define  PWR_CR1_PDDS                         ((uint32_t)0x00000002)     /*!< Power Down Deepsleep                */
#define  PWR_CR1_CSBF                         ((uint32_t)0x00000008)     /*!< Clear Standby Flag                  */
#define  PWR_CR1_PVDE                         ((uint32_t)0x00000010)     /*!< Power Voltage Detector Enable       */

#define  PWR_CR1_PLS                          ((uint32_t)0x000000E0)     /*!< PLS[2:0] bits (PVD Level Selection) */
#define  PWR_CR1_PLS_0                        ((uint32_t)0x00000020)     /*!< Bit 0 */
#define  PWR_CR1_PLS_1                        ((uint32_t)0x00000040)     /*!< Bit 1 */
#define  PWR_CR1_PLS_2                        ((uint32_t)0x00000080)     /*!< Bit 2 */

/*!< PVD level configuration */
#define  PWR_CR1_PLS_LEV0                     ((uint32_t)0x00000000)     /*!< PVD level 0 */
#define  PWR_CR1_PLS_LEV1                     ((uint32_t)0x00000020)     /*!< PVD level 1 */
#define  PWR_CR1_PLS_LEV2                     ((uint32_t)0x00000040)     /*!< PVD level 2 */
#define  PWR_CR1_PLS_LEV3                     ((uint32_t)0x00000060)     /*!< PVD level 3 */
#define  PWR_CR1_PLS_LEV4                     ((uint32_t)0x00000080)     /*!< PVD level 4 */
#define  PWR_CR1_PLS_LEV5                     ((uint32_t)0x000000A0)     /*!< PVD level 5 */
#define  PWR_CR1_PLS_LEV6                     ((uint32_t)0x000000C0)     /*!< PVD level 6 */
#define  PWR_CR1_PLS_LEV7                     ((uint32_t)0x000000E0)     /*!< PVD level 7 */

#define  PWR_CR1_DBP                          ((uint32_t)0x00000100)     /*!< Disable Backup Domain write protection                     */
#define  PWR_CR1_FPDS                         ((uint32_t)0x00000200)     /*!< Flash power down in Stop mode                              */

#define  PWR_CR1_LPUDS                        ((uint32_t)0x00000400)     /*!< Low-power regulator in deepsleep under-drive mode          */
#define  PWR_CR1_MRUDS                        ((uint32_t)0x00000800)     /*!< Main regulator in deepsleep under-drive mode               */

#define  PWR_CR1_ADCDC1                       ((uint32_t)0x00002000)     /*!< Refer to AN4073 on how to use this bit */

#define  PWR_CR1_VOS                          ((uint32_t)0x0000C000)     /*!< VOS[1:0] bits (Regulator voltage scaling output selection) */
#define  PWR_CR1_VOS_0                        ((uint32_t)0x00004000)     /*!< Bit 0 */
#define  PWR_CR1_VOS_1                        ((uint32_t)0x00008000)     /*!< Bit 1 */

#define  PWR_CR1_ODEN                         ((uint32_t)0x00010000)     /*!< Over Drive enable                   */
#define  PWR_CR1_ODSWEN                       ((uint32_t)0x00020000)     /*!< Over Drive switch enabled           */
#define  PWR_CR1_UDEN                         ((uint32_t)0x000C0000)     /*!< Under Drive enable in stop mode     */
#define  PWR_CR1_UDEN_0                       ((uint32_t)0x00040000)     /*!< Bit 0                               */
#define  PWR_CR1_UDEN_1                       ((uint32_t)0x00080000)     /*!< Bit 1                               */

/*******************  Bit definition for PWR_CSR1 register  ********************/
#define  PWR_CSR1_WUIF                        ((uint32_t)0x00000001)     /*!< Wake up internal Flag                            */
#define  PWR_CSR1_SBF                         ((uint32_t)0x00000002)     /*!< Standby Flag                                     */
#define  PWR_CSR1_PVDO                        ((uint32_t)0x00000004)     /*!< PVD Output                                       */
#define  PWR_CSR1_BRR                         ((uint32_t)0x00000008)     /*!< Backup regulator ready                           */
#define  PWR_CSR1_BRE                         ((uint32_t)0x00000200)     /*!< Backup regulator enable                          */
#define  PWR_CSR1_VOSRDY                      ((uint32_t)0x00004000)     /*!< Regulator voltage scaling output selection ready */

#define  PWR_CSR1_ODRDY                       ((uint32_t)0x00010000)     /*!< Over Drive generator ready                       */
#define  PWR_CSR1_ODSWRDY                     ((uint32_t)0x00020000)     /*!< Over Drive Switch ready                          */
#define  PWR_CSR1_UDSWRDY                     ((uint32_t)0x000C0000)     /*!< Under Drive ready                                */

/********************  Bit definition for PWR_CR2 register  ********************/
#define  PWR_CR2_CWUPF1                         ((uint32_t)0x00000001)     /*!< Clear Wakeup Pin Flag for PA0      */
#define  PWR_CR2_CWUPF2                         ((uint32_t)0x00000002)     /*!< Clear Wakeup Pin Flag for PA2      */
#define  PWR_CR2_CWUPF3                         ((uint32_t)0x00000004)     /*!< Clear Wakeup Pin Flag for PC1      */
#define  PWR_CR2_CWUPF4                         ((uint32_t)0x00000008)     /*!< Clear Wakeup Pin Flag for PC13     */
#define  PWR_CR2_CWUPF5                         ((uint32_t)0x00000010)     /*!< Clear Wakeup Pin Flag for PI8      */
#define  PWR_CR2_CWUPF6                         ((uint32_t)0x00000020)     /*!< Clear Wakeup Pin Flag for PI11     */

#define  PWR_CR2_WUPP1                          ((uint32_t)0x00000100)     /*!< Wakeup Pin Polarity bit for PA0    */
#define  PWR_CR2_WUPP2                          ((uint32_t)0x00000200)     /*!< Wakeup Pin Polarity bit for PA2    */
#define  PWR_CR2_WUPP3                          ((uint32_t)0x00000400)     /*!< Wakeup Pin Polarity bit for PC1    */
#define  PWR_CR2_WUPP4                          ((uint32_t)0x00000800)     /*!< Wakeup Pin Polarity bit for PC13   */
#define  PWR_CR2_WUPP5                          ((uint32_t)0x00001000)     /*!< Wakeup Pin Polarity bit for PI8    */
#define  PWR_CR2_WUPP6                          ((uint32_t)0x00002000)     /*!< Wakeup Pin Polarity bit for PI11   */

/*******************  Bit definition for PWR_CSR2 register  ********************/
#define  PWR_CSR2_WUPF1                         ((uint32_t)0x00000001)     /*!< Wakeup Pin Flag for PA0            */
#define  PWR_CSR2_WUPF2                         ((uint32_t)0x00000002)     /*!< Wakeup Pin Flag for PA2            */
#define  PWR_CSR2_WUPF3                         ((uint32_t)0x00000004)     /*!< Wakeup Pin Flag for PC1            */
#define  PWR_CSR2_WUPF4                         ((uint32_t)0x00000008)     /*!< Wakeup Pin Flag for PC13           */
#define  PWR_CSR2_WUPF5                         ((uint32_t)0x00000010)     /*!< Wakeup Pin Flag for PI8            */
#define  PWR_CSR2_WUPF6                         ((uint32_t)0x00000020)     /*!< Wakeup Pin Flag for PI11           */

#define  PWR_CSR2_EWUP1                         ((uint32_t)0x00000100)     /*!< Enable Wakeup Pin PA0              */
#define  PWR_CSR2_EWUP2                         ((uint32_t)0x00000200)     /*!< Enable Wakeup Pin PA2              */
#define  PWR_CSR2_EWUP3                         ((uint32_t)0x00000400)     /*!< Enable Wakeup Pin PC1              */
#define  PWR_CSR2_EWUP4                         ((uint32_t)0x00000800)     /*!< Enable Wakeup Pin PC13             */
#define  PWR_CSR2_EWUP5                         ((uint32_t)0x00001000)     /*!< Enable Wakeup Pin PI8              */
#define  PWR_CSR2_EWUP6                         ((uint32_t)0x00002000)     /*!< Enable Wakeup Pin PI11             */

/******************************************************************************/
/*                                                                            */
/*                                    QUADSPI                                 */
/*                                                                            */
/******************************************************************************/
/*****************  Bit definition for QUADSPI_CR register  *******************/
#define  QUADSPI_CR_EN                           ((uint32_t)0x00000001)            /*!< Enable                            */
#define  QUADSPI_CR_ABORT                        ((uint32_t)0x00000002)            /*!< Abort request                     */
#define  QUADSPI_CR_DMAEN                        ((uint32_t)0x00000004)            /*!< DMA Enable                        */
#define  QUADSPI_CR_TCEN                         ((uint32_t)0x00000008)            /*!< Timeout Counter Enable            */
#define  QUADSPI_CR_SSHIFT                       ((uint32_t)0x00000010)            /*!< Sample Shift                      */
#define  QUADSPI_CR_DFM                          ((uint32_t)0x00000040)            /*!< Dual Flash Mode                   */
#define  QUADSPI_CR_FSEL                         ((uint32_t)0x00000080)            /*!< Flash Select                      */
#define  QUADSPI_CR_FTHRES                       ((uint32_t)0x00000F00)            /*!< FTHRES[3:0] FIFO Level            */
#define  QUADSPI_CR_FTHRES_0                     ((uint32_t)0x00000100)            /*!< Bit 0 */
#define  QUADSPI_CR_FTHRES_1                     ((uint32_t)0x00000200)            /*!< Bit 1 */
#define  QUADSPI_CR_FTHRES_2                     ((uint32_t)0x00000400)            /*!< Bit 2 */
#define  QUADSPI_CR_FTHRES_3                     ((uint32_t)0x00000800)            /*!< Bit 3 */
#define  QUADSPI_CR_TEIE                         ((uint32_t)0x00010000)            /*!< Transfer Error Interrupt Enable    */
#define  QUADSPI_CR_TCIE                         ((uint32_t)0x00020000)            /*!< Transfer Complete Interrupt Enable */
#define  QUADSPI_CR_FTIE                         ((uint32_t)0x00040000)            /*!< FIFO Threshold Interrupt Enable    */
#define  QUADSPI_CR_SMIE                         ((uint32_t)0x00080000)            /*!< Status Match Interrupt Enable      */
#define  QUADSPI_CR_TOIE                         ((uint32_t)0x00100000)            /*!< TimeOut Interrupt Enable           */
#define  QUADSPI_CR_APMS                         ((uint32_t)0x00400000)            /*!< Bit 1                              */
#define  QUADSPI_CR_PMM                          ((uint32_t)0x00800000)            /*!< Polling Match Mode                 */
#define  QUADSPI_CR_PRESCALER                    ((uint32_t)0xFF000000)            /*!< PRESCALER[7:0] Clock prescaler     */
#define  QUADSPI_CR_PRESCALER_0                  ((uint32_t)0x01000000)            /*!< Bit 0 */
#define  QUADSPI_CR_PRESCALER_1                  ((uint32_t)0x02000000)            /*!< Bit 1 */
#define  QUADSPI_CR_PRESCALER_2                  ((uint32_t)0x04000000)            /*!< Bit 2 */
#define  QUADSPI_CR_PRESCALER_3                  ((uint32_t)0x08000000)            /*!< Bit 3 */
#define  QUADSPI_CR_PRESCALER_4                  ((uint32_t)0x10000000)            /*!< Bit 4 */
#define  QUADSPI_CR_PRESCALER_5                  ((uint32_t)0x20000000)            /*!< Bit 5 */
#define  QUADSPI_CR_PRESCALER_6                  ((uint32_t)0x40000000)            /*!< Bit 6 */
#define  QUADSPI_CR_PRESCALER_7                  ((uint32_t)0x80000000)            /*!< Bit 7 */

/*****************  Bit definition for QUADSPI_DCR register  ******************/
#define  QUADSPI_DCR_CKMODE                      ((uint32_t)0x00000001)            /*!< Mode 0 / Mode 3                 */
#define  QUADSPI_DCR_CSHT                        ((uint32_t)0x00000700)            /*!< CSHT[2:0]: ChipSelect High Time */
#define  QUADSPI_DCR_CSHT_0                      ((uint32_t)0x00000100)            /*!< Bit 0 */
#define  QUADSPI_DCR_CSHT_1                      ((uint32_t)0x00000200)            /*!< Bit 1 */
#define  QUADSPI_DCR_CSHT_2                      ((uint32_t)0x00000400)            /*!< Bit 2 */
#define  QUADSPI_DCR_FSIZE                       ((uint32_t)0x001F0000)            /*!< FSIZE[4:0]: Flash Size          */
#define  QUADSPI_DCR_FSIZE_0                     ((uint32_t)0x00010000)            /*!< Bit 0 */
#define  QUADSPI_DCR_FSIZE_1                     ((uint32_t)0x00020000)            /*!< Bit 1 */
#define  QUADSPI_DCR_FSIZE_2                     ((uint32_t)0x00040000)            /*!< Bit 2 */
#define  QUADSPI_DCR_FSIZE_3                     ((uint32_t)0x00080000)            /*!< Bit 3 */
#define  QUADSPI_DCR_FSIZE_4                     ((uint32_t)0x00100000)            /*!< Bit 4 */

/******************  Bit definition for QUADSPI_SR register  *******************/
#define  QUADSPI_SR_TEF                          ((uint32_t)0x00000001)             /*!< Transfer Error Flag    */
#define  QUADSPI_SR_TCF                          ((uint32_t)0x00000002)             /*!< Transfer Complete Flag */
#define  QUADSPI_SR_FTF                          ((uint32_t)0x00000004)             /*!< FIFO Threshlod Flag    */
#define  QUADSPI_SR_SMF                          ((uint32_t)0x00000008)             /*!< Status Match Flag      */
#define  QUADSPI_SR_TOF                          ((uint32_t)0x00000010)             /*!< Timeout Flag           */
#define  QUADSPI_SR_BUSY                         ((uint32_t)0x00000020)             /*!< Busy                   */
#define  QUADSPI_SR_FLEVEL                       ((uint32_t)0x00001F00)             /*!< FIFO Threshlod Flag    */
#define  QUADSPI_SR_FLEVEL_0                     ((uint32_t)0x00000100)             /*!< Bit 0 */
#define  QUADSPI_SR_FLEVEL_1                     ((uint32_t)0x00000200)             /*!< Bit 1 */
#define  QUADSPI_SR_FLEVEL_2                     ((uint32_t)0x00000400)             /*!< Bit 2 */
#define  QUADSPI_SR_FLEVEL_3                     ((uint32_t)0x00000800)             /*!< Bit 3 */
#define  QUADSPI_SR_FLEVEL_4                     ((uint32_t)0x00001000)             /*!< Bit 4 */

/******************  Bit definition for QUADSPI_FCR register  ******************/
#define  QUADSPI_FCR_CTEF                        ((uint32_t)0x00000001)             /*!< Clear Transfer Error Flag    */
#define  QUADSPI_FCR_CTCF                        ((uint32_t)0x00000002)             /*!< Clear Transfer Complete Flag */
#define  QUADSPI_FCR_CSMF                        ((uint32_t)0x00000008)             /*!< Clear Status Match Flag      */
#define  QUADSPI_FCR_CTOF                        ((uint32_t)0x00000010)             /*!< Clear Timeout Flag           */

/******************  Bit definition for QUADSPI_DLR register  ******************/
#define  QUADSPI_DLR_DL                        ((uint32_t)0xFFFFFFFF)               /*!< DL[31:0]: Data Length */

/******************  Bit definition for QUADSPI_CCR register  ******************/
#define  QUADSPI_CCR_INSTRUCTION                  ((uint32_t)0x000000FF)            /*!< INSTRUCTION[7:0]: Instruction    */
#define  QUADSPI_CCR_INSTRUCTION_0                ((uint32_t)0x00000001)            /*!< Bit 0 */
#define  QUADSPI_CCR_INSTRUCTION_1                ((uint32_t)0x00000002)            /*!< Bit 1 */
#define  QUADSPI_CCR_INSTRUCTION_2                ((uint32_t)0x00000004)            /*!< Bit 2 */
#define  QUADSPI_CCR_INSTRUCTION_3                ((uint32_t)0x00000008)            /*!< Bit 3 */
#define  QUADSPI_CCR_INSTRUCTION_4                ((uint32_t)0x00000010)            /*!< Bit 4 */
#define  QUADSPI_CCR_INSTRUCTION_5                ((uint32_t)0x00000020)            /*!< Bit 5 */
#define  QUADSPI_CCR_INSTRUCTION_6                ((uint32_t)0x00000040)            /*!< Bit 6 */
#define  QUADSPI_CCR_INSTRUCTION_7                ((uint32_t)0x00000080)            /*!< Bit 7 */
#define  QUADSPI_CCR_IMODE                        ((uint32_t)0x00000300)            /*!< IMODE[1:0]: Instruction Mode      */
#define  QUADSPI_CCR_IMODE_0                      ((uint32_t)0x00000100)            /*!< Bit 0 */
#define  QUADSPI_CCR_IMODE_1                      ((uint32_t)0x00000200)            /*!< Bit 1 */
#define  QUADSPI_CCR_ADMODE                       ((uint32_t)0x00000C00)            /*!< ADMODE[1:0]: Address Mode         */
#define  QUADSPI_CCR_ADMODE_0                     ((uint32_t)0x00000400)            /*!< Bit 0 */
#define  QUADSPI_CCR_ADMODE_1                     ((uint32_t)0x00000800)            /*!< Bit 1 */
#define  QUADSPI_CCR_ADSIZE                       ((uint32_t)0x00003000)            /*!< ADSIZE[1:0]: Address Size         */
#define  QUADSPI_CCR_ADSIZE_0                     ((uint32_t)0x00001000)            /*!< Bit 0 */
#define  QUADSPI_CCR_ADSIZE_1                     ((uint32_t)0x00002000)            /*!< Bit 1 */
#define  QUADSPI_CCR_ABMODE                       ((uint32_t)0x0000C000)            /*!< ABMODE[1:0]: Alternate Bytes Mode */
#define  QUADSPI_CCR_ABMODE_0                     ((uint32_t)0x00004000)            /*!< Bit 0 */
#define  QUADSPI_CCR_ABMODE_1                     ((uint32_t)0x00008000)            /*!< Bit 1 */
#define  QUADSPI_CCR_ABSIZE                       ((uint32_t)0x00030000)            /*!< ABSIZE[1:0]: Instruction Mode     */
#define  QUADSPI_CCR_ABSIZE_0                     ((uint32_t)0x00010000)            /*!< Bit 0 */
#define  QUADSPI_CCR_ABSIZE_1                     ((uint32_t)0x00020000)            /*!< Bit 1 */
#define  QUADSPI_CCR_DCYC                         ((uint32_t)0x007C0000)            /*!< DCYC[4:0]: Dummy Cycles           */
#define  QUADSPI_CCR_DCYC_0                       ((uint32_t)0x00040000)            /*!< Bit 0 */
#define  QUADSPI_CCR_DCYC_1                       ((uint32_t)0x00080000)            /*!< Bit 1 */
#define  QUADSPI_CCR_DCYC_2                       ((uint32_t)0x00100000)            /*!< Bit 2 */
#define  QUADSPI_CCR_DCYC_3                       ((uint32_t)0x00200000)            /*!< Bit 3 */
#define  QUADSPI_CCR_DCYC_4                       ((uint32_t)0x00400000)            /*!< Bit 4 */
#define  QUADSPI_CCR_DMODE                        ((uint32_t)0x03000000)            /*!< DMODE[1:0]: Data Mode              */
#define  QUADSPI_CCR_DMODE_0                      ((uint32_t)0x01000000)            /*!< Bit 0 */
#define  QUADSPI_CCR_DMODE_1                      ((uint32_t)0x02000000)            /*!< Bit 1 */
#define  QUADSPI_CCR_FMODE                        ((uint32_t)0x0C000000)            /*!< FMODE[1:0]: Functional Mode        */
#define  QUADSPI_CCR_FMODE_0                      ((uint32_t)0x04000000)            /*!< Bit 0 */
#define  QUADSPI_CCR_FMODE_1                      ((uint32_t)0x08000000)            /*!< Bit 1 */
#define  QUADSPI_CCR_SIOO                         ((uint32_t)0x10000000)            /*!< SIOO: Send Instruction Only Once Mode */
#define  QUADSPI_CCR_DHHC                         ((uint32_t)0x40000000)            /*!< DHHC: Delay Half Hclk Cycle           */
#define  QUADSPI_CCR_DDRM                         ((uint32_t)0x80000000)            /*!< DDRM: Double Data Rate Mode           */
/******************  Bit definition for QUADSPI_AR register  *******************/
#define  QUADSPI_AR_ADDRESS                       ((uint32_t)0xFFFFFFFF)            /*!< ADDRESS[31:0]: Address */

/******************  Bit definition for QUADSPI_ABR register  ******************/
#define  QUADSPI_ABR_ALTERNATE                    ((uint32_t)0xFFFFFFFF)            /*!< ALTERNATE[31:0]: Alternate Bytes */

/******************  Bit definition for QUADSPI_DR register  *******************/
#define  QUADSPI_DR_DATA                          ((uint32_t)0xFFFFFFFF)            /*!< DATA[31:0]: Data */

/******************  Bit definition for QUADSPI_PSMKR register  ****************/
#define  QUADSPI_PSMKR_MASK                       ((uint32_t)0xFFFFFFFF)            /*!< MASK[31:0]: Status Mask */

/******************  Bit definition for QUADSPI_PSMAR register  ****************/
#define  QUADSPI_PSMAR_MATCH                      ((uint32_t)0xFFFFFFFF)            /*!< MATCH[31:0]: Status Match */

/******************  Bit definition for QUADSPI_PIR register  *****************/
#define  QUADSPI_PIR_INTERVAL                     ((uint32_t)0x0000FFFF)            /*!< INTERVAL[15:0]: Polling Interval */

/******************  Bit definition for QUADSPI_LPTR register  *****************/
#define  QUADSPI_LPTR_TIMEOUT                     ((uint32_t)0x0000FFFF)            /*!< TIMEOUT[15:0]: Timeout period */

/******************************************************************************/
/*                                                                            */
/*                         Reset and Clock Control            */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for RCC_CR register  ********************/
#define  RCC_CR_HSION                        ((uint32_t)0x00000001)
#define  RCC_CR_HSIRDY                       ((uint32_t)0x00000002)

#define  RCC_CR_HSITRIM                      ((uint32_t)0x000000F8)
#define  RCC_CR_HSITRIM_0                    ((uint32_t)0x00000008) /*!<Bit 0 */
#define  RCC_CR_HSITRIM_1                    ((uint32_t)0x00000010) /*!<Bit 1 */
#define  RCC_CR_HSITRIM_2                    ((uint32_t)0x00000020) /*!<Bit 2 */
#define  RCC_CR_HSITRIM_3                    ((uint32_t)0x00000040) /*!<Bit 3 */
#define  RCC_CR_HSITRIM_4                    ((uint32_t)0x00000080) /*!<Bit 4 */

#define  RCC_CR_HSICAL                       ((uint32_t)0x0000FF00)
#define  RCC_CR_HSICAL_0                     ((uint32_t)0x00000100) /*!<Bit 0 */
#define  RCC_CR_HSICAL_1                     ((uint32_t)0x00000200) /*!<Bit 1 */
#define  RCC_CR_HSICAL_2                     ((uint32_t)0x00000400) /*!<Bit 2 */
#define  RCC_CR_HSICAL_3                     ((uint32_t)0x00000800) /*!<Bit 3 */
#define  RCC_CR_HSICAL_4                     ((uint32_t)0x00001000) /*!<Bit 4 */
#define  RCC_CR_HSICAL_5                     ((uint32_t)0x00002000) /*!<Bit 5 */
#define  RCC_CR_HSICAL_6                     ((uint32_t)0x00004000) /*!<Bit 6 */
#define  RCC_CR_HSICAL_7                     ((uint32_t)0x00008000) /*!<Bit 7 */

#define  RCC_CR_HSEON                        ((uint32_t)0x00010000)
#define  RCC_CR_HSERDY                       ((uint32_t)0x00020000)
#define  RCC_CR_HSEBYP                       ((uint32_t)0x00040000)
#define  RCC_CR_CSSON                        ((uint32_t)0x00080000)
#define  RCC_CR_PLLON                        ((uint32_t)0x01000000)
#define  RCC_CR_PLLRDY                       ((uint32_t)0x02000000)
#define  RCC_CR_PLLI2SON                     ((uint32_t)0x04000000)
#define  RCC_CR_PLLI2SRDY                    ((uint32_t)0x08000000)
#define  RCC_CR_PLLSAION                     ((uint32_t)0x10000000)
#define  RCC_CR_PLLSAIRDY                    ((uint32_t)0x20000000)

/********************  Bit definition for RCC_PLLCFGR register  ***************/
#define  RCC_PLLCFGR_PLLM                    ((uint32_t)0x0000003F)
#define  RCC_PLLCFGR_PLLM_0                  ((uint32_t)0x00000001)
#define  RCC_PLLCFGR_PLLM_1                  ((uint32_t)0x00000002)
#define  RCC_PLLCFGR_PLLM_2                  ((uint32_t)0x00000004)
#define  RCC_PLLCFGR_PLLM_3                  ((uint32_t)0x00000008)
#define  RCC_PLLCFGR_PLLM_4                  ((uint32_t)0x00000010)
#define  RCC_PLLCFGR_PLLM_5                  ((uint32_t)0x00000020)

#define  RCC_PLLCFGR_PLLN                     ((uint32_t)0x00007FC0)
#define  RCC_PLLCFGR_PLLN_0                   ((uint32_t)0x00000040)
#define  RCC_PLLCFGR_PLLN_1                   ((uint32_t)0x00000080)
#define  RCC_PLLCFGR_PLLN_2                   ((uint32_t)0x00000100)
#define  RCC_PLLCFGR_PLLN_3                   ((uint32_t)0x00000200)
#define  RCC_PLLCFGR_PLLN_4                   ((uint32_t)0x00000400)
#define  RCC_PLLCFGR_PLLN_5                   ((uint32_t)0x00000800)
#define  RCC_PLLCFGR_PLLN_6                   ((uint32_t)0x00001000)
#define  RCC_PLLCFGR_PLLN_7                   ((uint32_t)0x00002000)
#define  RCC_PLLCFGR_PLLN_8                   ((uint32_t)0x00004000)

#define  RCC_PLLCFGR_PLLP                    ((uint32_t)0x00030000)
#define  RCC_PLLCFGR_PLLP_0                  ((uint32_t)0x00010000)
#define  RCC_PLLCFGR_PLLP_1                  ((uint32_t)0x00020000)

#define  RCC_PLLCFGR_PLLSRC                  ((uint32_t)0x00400000)
#define  RCC_PLLCFGR_PLLSRC_HSE              ((uint32_t)0x00400000)
#define  RCC_PLLCFGR_PLLSRC_HSI              ((uint32_t)0x00000000)

#define  RCC_PLLCFGR_PLLQ                    ((uint32_t)0x0F000000)
#define  RCC_PLLCFGR_PLLQ_0                  ((uint32_t)0x01000000)
#define  RCC_PLLCFGR_PLLQ_1                  ((uint32_t)0x02000000)
#define  RCC_PLLCFGR_PLLQ_2                  ((uint32_t)0x04000000)
#define  RCC_PLLCFGR_PLLQ_3                  ((uint32_t)0x08000000)

/********************  Bit definition for RCC_CFGR register  ******************/
/*!< SW configuration */
#define  RCC_CFGR_SW                         ((uint32_t)0x00000003)        /*!< SW[1:0] bits (System clock Switch) */
#define  RCC_CFGR_SW_0                       ((uint32_t)0x00000001)        /*!< Bit 0 */
#define  RCC_CFGR_SW_1                       ((uint32_t)0x00000002)        /*!< Bit 1 */

#define  RCC_CFGR_SW_HSI                     ((uint32_t)0x00000000)        /*!< HSI selected as system clock */
#define  RCC_CFGR_SW_HSE                     ((uint32_t)0x00000001)        /*!< HSE selected as system clock */
#define  RCC_CFGR_SW_PLL                     ((uint32_t)0x00000002)        /*!< PLL selected as system clock */

/*!< SWS configuration */
#define  RCC_CFGR_SWS                        ((uint32_t)0x0000000C)        /*!< SWS[1:0] bits (System Clock Switch Status) */
#define  RCC_CFGR_SWS_0                      ((uint32_t)0x00000004)        /*!< Bit 0 */
#define  RCC_CFGR_SWS_1                      ((uint32_t)0x00000008)        /*!< Bit 1 */

#define  RCC_CFGR_SWS_HSI                    ((uint32_t)0x00000000)        /*!< HSI oscillator used as system clock */
#define  RCC_CFGR_SWS_HSE                    ((uint32_t)0x00000004)        /*!< HSE oscillator used as system clock */
#define  RCC_CFGR_SWS_PLL                    ((uint32_t)0x00000008)        /*!< PLL used as system clock */

/*!< HPRE configuration */
#define  RCC_CFGR_HPRE                       ((uint32_t)0x000000F0)        /*!< HPRE[3:0] bits (AHB prescaler) */
#define  RCC_CFGR_HPRE_0                     ((uint32_t)0x00000010)        /*!< Bit 0 */
#define  RCC_CFGR_HPRE_1                     ((uint32_t)0x00000020)        /*!< Bit 1 */
#define  RCC_CFGR_HPRE_2                     ((uint32_t)0x00000040)        /*!< Bit 2 */
#define  RCC_CFGR_HPRE_3                     ((uint32_t)0x00000080)        /*!< Bit 3 */

#define  RCC_CFGR_HPRE_DIV1                  ((uint32_t)0x00000000)        /*!< SYSCLK not divided */
#define  RCC_CFGR_HPRE_DIV2                  ((uint32_t)0x00000080)        /*!< SYSCLK divided by 2 */
#define  RCC_CFGR_HPRE_DIV4                  ((uint32_t)0x00000090)        /*!< SYSCLK divided by 4 */
#define  RCC_CFGR_HPRE_DIV8                  ((uint32_t)0x000000A0)        /*!< SYSCLK divided by 8 */
#define  RCC_CFGR_HPRE_DIV16                 ((uint32_t)0x000000B0)        /*!< SYSCLK divided by 16 */
#define  RCC_CFGR_HPRE_DIV64                 ((uint32_t)0x000000C0)        /*!< SYSCLK divided by 64 */
#define  RCC_CFGR_HPRE_DIV128                ((uint32_t)0x000000D0)        /*!< SYSCLK divided by 128 */
#define  RCC_CFGR_HPRE_DIV256                ((uint32_t)0x000000E0)        /*!< SYSCLK divided by 256 */
#define  RCC_CFGR_HPRE_DIV512                ((uint32_t)0x000000F0)        /*!< SYSCLK divided by 512 */

/*!< PPRE1 configuration */
#define  RCC_CFGR_PPRE1                      ((uint32_t)0x00001C00)        /*!< PRE1[2:0] bits (APB1 prescaler) */
#define  RCC_CFGR_PPRE1_0                    ((uint32_t)0x00000400)        /*!< Bit 0 */
#define  RCC_CFGR_PPRE1_1                    ((uint32_t)0x00000800)        /*!< Bit 1 */
#define  RCC_CFGR_PPRE1_2                    ((uint32_t)0x00001000)        /*!< Bit 2 */

#define  RCC_CFGR_PPRE1_DIV1                 ((uint32_t)0x00000000)        /*!< HCLK not divided */
#define  RCC_CFGR_PPRE1_DIV2                 ((uint32_t)0x00001000)        /*!< HCLK divided by 2 */
#define  RCC_CFGR_PPRE1_DIV4                 ((uint32_t)0x00001400)        /*!< HCLK divided by 4 */
#define  RCC_CFGR_PPRE1_DIV8                 ((uint32_t)0x00001800)        /*!< HCLK divided by 8 */
#define  RCC_CFGR_PPRE1_DIV16                ((uint32_t)0x00001C00)        /*!< HCLK divided by 16 */

/*!< PPRE2 configuration */
#define  RCC_CFGR_PPRE2                      ((uint32_t)0x0000E000)        /*!< PRE2[2:0] bits (APB2 prescaler) */
#define  RCC_CFGR_PPRE2_0                    ((uint32_t)0x00002000)        /*!< Bit 0 */
#define  RCC_CFGR_PPRE2_1                    ((uint32_t)0x00004000)        /*!< Bit 1 */
#define  RCC_CFGR_PPRE2_2                    ((uint32_t)0x00008000)        /*!< Bit 2 */

#define  RCC_CFGR_PPRE2_DIV1                 ((uint32_t)0x00000000)        /*!< HCLK not divided */
#define  RCC_CFGR_PPRE2_DIV2                 ((uint32_t)0x00008000)        /*!< HCLK divided by 2 */
#define  RCC_CFGR_PPRE2_DIV4                 ((uint32_t)0x0000A000)        /*!< HCLK divided by 4 */
#define  RCC_CFGR_PPRE2_DIV8                 ((uint32_t)0x0000C000)        /*!< HCLK divided by 8 */
#define  RCC_CFGR_PPRE2_DIV16                ((uint32_t)0x0000E000)        /*!< HCLK divided by 16 */

/*!< RTCPRE configuration */
#define  RCC_CFGR_RTCPRE                     ((uint32_t)0x001F0000)
#define  RCC_CFGR_RTCPRE_0                   ((uint32_t)0x00010000)
#define  RCC_CFGR_RTCPRE_1                   ((uint32_t)0x00020000)
#define  RCC_CFGR_RTCPRE_2                   ((uint32_t)0x00040000)
#define  RCC_CFGR_RTCPRE_3                   ((uint32_t)0x00080000)
#define  RCC_CFGR_RTCPRE_4                   ((uint32_t)0x00100000)

/*!< MCO1 configuration */
#define  RCC_CFGR_MCO1                       ((uint32_t)0x00600000)
#define  RCC_CFGR_MCO1_0                     ((uint32_t)0x00200000)
#define  RCC_CFGR_MCO1_1                     ((uint32_t)0x00400000)

#define  RCC_CFGR_I2SSRC                     ((uint32_t)0x00800000)

#define  RCC_CFGR_MCO1PRE                    ((uint32_t)0x07000000)
#define  RCC_CFGR_MCO1PRE_0                  ((uint32_t)0x01000000)
#define  RCC_CFGR_MCO1PRE_1                  ((uint32_t)0x02000000)
#define  RCC_CFGR_MCO1PRE_2                  ((uint32_t)0x04000000)

#define  RCC_CFGR_MCO2PRE                    ((uint32_t)0x38000000)
#define  RCC_CFGR_MCO2PRE_0                  ((uint32_t)0x08000000)
#define  RCC_CFGR_MCO2PRE_1                  ((uint32_t)0x10000000)
#define  RCC_CFGR_MCO2PRE_2                  ((uint32_t)0x20000000)

#define  RCC_CFGR_MCO2                       ((uint32_t)0xC0000000)
#define  RCC_CFGR_MCO2_0                     ((uint32_t)0x40000000)
#define  RCC_CFGR_MCO2_1                     ((uint32_t)0x80000000)

/********************  Bit definition for RCC_CIR register  *******************/
#define  RCC_CIR_LSIRDYF                     ((uint32_t)0x00000001)
#define  RCC_CIR_LSERDYF                     ((uint32_t)0x00000002)
#define  RCC_CIR_HSIRDYF                     ((uint32_t)0x00000004)
#define  RCC_CIR_HSERDYF                     ((uint32_t)0x00000008)
#define  RCC_CIR_PLLRDYF                     ((uint32_t)0x00000010)
#define  RCC_CIR_PLLI2SRDYF                  ((uint32_t)0x00000020)
#define  RCC_CIR_PLLSAIRDYF                  ((uint32_t)0x00000040)
#define  RCC_CIR_CSSF                        ((uint32_t)0x00000080)
#define  RCC_CIR_LSIRDYIE                    ((uint32_t)0x00000100)
#define  RCC_CIR_LSERDYIE                    ((uint32_t)0x00000200)
#define  RCC_CIR_HSIRDYIE                    ((uint32_t)0x00000400)
#define  RCC_CIR_HSERDYIE                    ((uint32_t)0x00000800)
#define  RCC_CIR_PLLRDYIE                    ((uint32_t)0x00001000)
#define  RCC_CIR_PLLI2SRDYIE                 ((uint32_t)0x00002000)
#define  RCC_CIR_PLLSAIRDYIE                 ((uint32_t)0x00004000)
#define  RCC_CIR_LSIRDYC                     ((uint32_t)0x00010000)
#define  RCC_CIR_LSERDYC                     ((uint32_t)0x00020000)
#define  RCC_CIR_HSIRDYC                     ((uint32_t)0x00040000)
#define  RCC_CIR_HSERDYC                     ((uint32_t)0x00080000)
#define  RCC_CIR_PLLRDYC                     ((uint32_t)0x00100000)
#define  RCC_CIR_PLLI2SRDYC                  ((uint32_t)0x00200000)
#define  RCC_CIR_PLLSAIRDYC                  ((uint32_t)0x00400000)
#define  RCC_CIR_CSSC                        ((uint32_t)0x00800000)

/********************  Bit definition for RCC_AHB1RSTR register  **************/
#define  RCC_AHB1RSTR_GPIOARST               ((uint32_t)0x00000001)
#define  RCC_AHB1RSTR_GPIOBRST               ((uint32_t)0x00000002)
#define  RCC_AHB1RSTR_GPIOCRST               ((uint32_t)0x00000004)
#define  RCC_AHB1RSTR_GPIODRST               ((uint32_t)0x00000008)
#define  RCC_AHB1RSTR_GPIOERST               ((uint32_t)0x00000010)
#define  RCC_AHB1RSTR_GPIOFRST               ((uint32_t)0x00000020)
#define  RCC_AHB1RSTR_GPIOGRST               ((uint32_t)0x00000040)
#define  RCC_AHB1RSTR_GPIOHRST               ((uint32_t)0x00000080)
#define  RCC_AHB1RSTR_GPIOIRST               ((uint32_t)0x00000100)
#define  RCC_AHB1RSTR_GPIOJRST               ((uint32_t)0x00000200)
#define  RCC_AHB1RSTR_GPIOKRST               ((uint32_t)0x00000400)
#define  RCC_AHB1RSTR_CRCRST                 ((uint32_t)0x00001000)
#define  RCC_AHB1RSTR_DMA1RST                ((uint32_t)0x00200000)
#define  RCC_AHB1RSTR_DMA2RST                ((uint32_t)0x00400000)
#define  RCC_AHB1RSTR_DMA2DRST               ((uint32_t)0x00800000)
#define  RCC_AHB1RSTR_ETHMACRST              ((uint32_t)0x02000000)
#define  RCC_AHB1RSTR_OTGHRST                ((uint32_t)0x20000000)

/********************  Bit definition for RCC_AHB2RSTR register  **************/
#define  RCC_AHB2RSTR_DCMIRST                ((uint32_t)0x00000001)
#define  RCC_AHB2RSTR_RNGRST                 ((uint32_t)0x00000040)
#define  RCC_AHB2RSTR_OTGFSRST               ((uint32_t)0x00000080)

/********************  Bit definition for RCC_AHB3RSTR register  **************/

#define  RCC_AHB3RSTR_FMCRST                ((uint32_t)0x00000001)
#define  RCC_AHB3RSTR_QSPIRST               ((uint32_t)0x00000002)

/********************  Bit definition for RCC_APB1RSTR register  **************/
#define  RCC_APB1RSTR_TIM2RST                ((uint32_t)0x00000001)
#define  RCC_APB1RSTR_TIM3RST                ((uint32_t)0x00000002)
#define  RCC_APB1RSTR_TIM4RST                ((uint32_t)0x00000004)
#define  RCC_APB1RSTR_TIM5RST                ((uint32_t)0x00000008)
#define  RCC_APB1RSTR_TIM6RST                ((uint32_t)0x00000010)
#define  RCC_APB1RSTR_TIM7RST                ((uint32_t)0x00000020)
#define  RCC_APB1RSTR_TIM12RST               ((uint32_t)0x00000040)
#define  RCC_APB1RSTR_TIM13RST               ((uint32_t)0x00000080)
#define  RCC_APB1RSTR_TIM14RST               ((uint32_t)0x00000100)
#define  RCC_APB1RSTR_LPTIM1RST              ((uint32_t)0x00000200)
#define  RCC_APB1RSTR_WWDGRST                ((uint32_t)0x00000800)
#define  RCC_APB1RSTR_SPI2RST                ((uint32_t)0x00004000)
#define  RCC_APB1RSTR_SPI3RST                ((uint32_t)0x00008000)
#define  RCC_APB1RSTR_SPDIFRXRST             ((uint32_t)0x00010000)
#define  RCC_APB1RSTR_USART2RST              ((uint32_t)0x00020000)
#define  RCC_APB1RSTR_USART3RST              ((uint32_t)0x00040000)
#define  RCC_APB1RSTR_UART4RST               ((uint32_t)0x00080000)
#define  RCC_APB1RSTR_UART5RST               ((uint32_t)0x00100000)
#define  RCC_APB1RSTR_I2C1RST                ((uint32_t)0x00200000)
#define  RCC_APB1RSTR_I2C2RST                ((uint32_t)0x00400000)
#define  RCC_APB1RSTR_I2C3RST                ((uint32_t)0x00800000)
#define  RCC_APB1RSTR_I2C4RST                ((uint32_t)0x01000000)
#define  RCC_APB1RSTR_CAN1RST                ((uint32_t)0x02000000)
#define  RCC_APB1RSTR_CAN2RST                ((uint32_t)0x04000000)
#define  RCC_APB1RSTR_CECRST                 ((uint32_t)0x08000000)
#define  RCC_APB1RSTR_PWRRST                 ((uint32_t)0x10000000)
#define  RCC_APB1RSTR_DACRST                 ((uint32_t)0x20000000)
#define  RCC_APB1RSTR_UART7RST               ((uint32_t)0x40000000)
#define  RCC_APB1RSTR_UART8RST               ((uint32_t)0x80000000)

/********************  Bit definition for RCC_APB2RSTR register  **************/
#define  RCC_APB2RSTR_TIM1RST                ((uint32_t)0x00000001)
#define  RCC_APB2RSTR_TIM8RST                ((uint32_t)0x00000002)
#define  RCC_APB2RSTR_USART1RST              ((uint32_t)0x00000010)
#define  RCC_APB2RSTR_USART6RST              ((uint32_t)0x00000020)
#define  RCC_APB2RSTR_ADCRST                 ((uint32_t)0x00000100)
#define  RCC_APB2RSTR_SDMMC1RST              ((uint32_t)0x00000800)
#define  RCC_APB2RSTR_SPI1RST                ((uint32_t)0x00001000)
#define  RCC_APB2RSTR_SPI4RST                ((uint32_t)0x00002000)
#define  RCC_APB2RSTR_SYSCFGRST              ((uint32_t)0x00004000)
#define  RCC_APB2RSTR_TIM9RST                ((uint32_t)0x00010000)
#define  RCC_APB2RSTR_TIM10RST               ((uint32_t)0x00020000)
#define  RCC_APB2RSTR_TIM11RST               ((uint32_t)0x00040000)
#define  RCC_APB2RSTR_SPI5RST                ((uint32_t)0x00100000)
#define  RCC_APB2RSTR_SPI6RST                ((uint32_t)0x00200000)
#define  RCC_APB2RSTR_SAI1RST                ((uint32_t)0x00400000)
#define  RCC_APB2RSTR_SAI2RST                ((uint32_t)0x00800000)
#define  RCC_APB2RSTR_LTDCRST                ((uint32_t)0x04000000)

/********************  Bit definition for RCC_AHB1ENR register  ***************/
#define  RCC_AHB1ENR_GPIOAEN                 ((uint32_t)0x00000001)
#define  RCC_AHB1ENR_GPIOBEN                 ((uint32_t)0x00000002)
#define  RCC_AHB1ENR_GPIOCEN                 ((uint32_t)0x00000004)
#define  RCC_AHB1ENR_GPIODEN                 ((uint32_t)0x00000008)
#define  RCC_AHB1ENR_GPIOEEN                 ((uint32_t)0x00000010)
#define  RCC_AHB1ENR_GPIOFEN                 ((uint32_t)0x00000020)
#define  RCC_AHB1ENR_GPIOGEN                 ((uint32_t)0x00000040)
#define  RCC_AHB1ENR_GPIOHEN                 ((uint32_t)0x00000080)
#define  RCC_AHB1ENR_GPIOIEN                 ((uint32_t)0x00000100)
#define  RCC_AHB1ENR_GPIOJEN                 ((uint32_t)0x00000200)
#define  RCC_AHB1ENR_GPIOKEN                 ((uint32_t)0x00000400)
#define  RCC_AHB1ENR_CRCEN                   ((uint32_t)0x00001000)
#define  RCC_AHB1ENR_BKPSRAMEN               ((uint32_t)0x00040000)
#define  RCC_AHB1ENR_DTCMRAMEN               ((uint32_t)0x00100000)
#define  RCC_AHB1ENR_DMA1EN                  ((uint32_t)0x00200000)
#define  RCC_AHB1ENR_DMA2EN                  ((uint32_t)0x00400000)
#define  RCC_AHB1ENR_DMA2DEN                 ((uint32_t)0x00800000)
#define  RCC_AHB1ENR_ETHMACEN                ((uint32_t)0x02000000)
#define  RCC_AHB1ENR_ETHMACTXEN              ((uint32_t)0x04000000)
#define  RCC_AHB1ENR_ETHMACRXEN              ((uint32_t)0x08000000)
#define  RCC_AHB1ENR_ETHMACPTPEN             ((uint32_t)0x10000000)
#define  RCC_AHB1ENR_OTGHSEN                 ((uint32_t)0x20000000)
#define  RCC_AHB1ENR_OTGHSULPIEN             ((uint32_t)0x40000000)

/********************  Bit definition for RCC_AHB2ENR register  ***************/
#define  RCC_AHB2ENR_DCMIEN                  ((uint32_t)0x00000001)
#define  RCC_AHB2ENR_RNGEN                   ((uint32_t)0x00000040)
#define  RCC_AHB2ENR_OTGFSEN                 ((uint32_t)0x00000080)

/********************  Bit definition for RCC_AHB3ENR register  ***************/

#define  RCC_AHB3ENR_FMCEN                  ((uint32_t)0x00000001)
#define  RCC_AHB3ENR_QSPIEN                 ((uint32_t)0x00000002)

/********************  Bit definition for RCC_APB1ENR register  ***************/
#define  RCC_APB1ENR_TIM2EN                  ((uint32_t)0x00000001)
#define  RCC_APB1ENR_TIM3EN                  ((uint32_t)0x00000002)
#define  RCC_APB1ENR_TIM4EN                  ((uint32_t)0x00000004)
#define  RCC_APB1ENR_TIM5EN                  ((uint32_t)0x00000008)
#define  RCC_APB1ENR_TIM6EN                  ((uint32_t)0x00000010)
#define  RCC_APB1ENR_TIM7EN                  ((uint32_t)0x00000020)
#define  RCC_APB1ENR_TIM12EN                 ((uint32_t)0x00000040)
#define  RCC_APB1ENR_TIM13EN                 ((uint32_t)0x00000080)
#define  RCC_APB1ENR_TIM14EN                 ((uint32_t)0x00000100)
#define  RCC_APB1ENR_LPTIM1EN                ((uint32_t)0x00000200)
#define  RCC_APB1ENR_WWDGEN                  ((uint32_t)0x00000800)
#define  RCC_APB1ENR_SPI2EN                  ((uint32_t)0x00004000)
#define  RCC_APB1ENR_SPI3EN                  ((uint32_t)0x00008000)
#define  RCC_APB1ENR_SPDIFRXEN               ((uint32_t)0x00010000)
#define  RCC_APB1ENR_USART2EN                ((uint32_t)0x00020000)
#define  RCC_APB1ENR_USART3EN                ((uint32_t)0x00040000)
#define  RCC_APB1ENR_UART4EN                 ((uint32_t)0x00080000)
#define  RCC_APB1ENR_UART5EN                 ((uint32_t)0x00100000)
#define  RCC_APB1ENR_I2C1EN                  ((uint32_t)0x00200000)
#define  RCC_APB1ENR_I2C2EN                  ((uint32_t)0x00400000)
#define  RCC_APB1ENR_I2C3EN                  ((uint32_t)0x00800000)
#define  RCC_APB1ENR_I2C4EN                  ((uint32_t)0x01000000)
#define  RCC_APB1ENR_CAN1EN                  ((uint32_t)0x02000000)
#define  RCC_APB1ENR_CAN2EN                  ((uint32_t)0x04000000)
#define  RCC_APB1ENR_CECEN                   ((uint32_t)0x08000000)
#define  RCC_APB1ENR_PWREN                   ((uint32_t)0x10000000)
#define  RCC_APB1ENR_DACEN                   ((uint32_t)0x20000000)
#define  RCC_APB1ENR_UART7EN                 ((uint32_t)0x40000000)
#define  RCC_APB1ENR_UART8EN                 ((uint32_t)0x80000000)

/********************  Bit definition for RCC_APB2ENR register  ***************/
#define  RCC_APB2ENR_TIM1EN                  ((uint32_t)0x00000001)
#define  RCC_APB2ENR_TIM8EN                  ((uint32_t)0x00000002)
#define  RCC_APB2ENR_USART1EN                ((uint32_t)0x00000010)
#define  RCC_APB2ENR_USART6EN                ((uint32_t)0x00000020)
#define  RCC_APB2ENR_ADC1EN                  ((uint32_t)0x00000100)
#define  RCC_APB2ENR_ADC2EN                  ((uint32_t)0x00000200)
#define  RCC_APB2ENR_ADC3EN                  ((uint32_t)0x00000400)
#define  RCC_APB2ENR_SDMMC1EN                ((uint32_t)0x00000800)
#define  RCC_APB2ENR_SPI1EN                  ((uint32_t)0x00001000)
#define  RCC_APB2ENR_SPI4EN                  ((uint32_t)0x00002000)
#define  RCC_APB2ENR_SYSCFGEN                ((uint32_t)0x00004000)
#define  RCC_APB2ENR_TIM9EN                  ((uint32_t)0x00010000)
#define  RCC_APB2ENR_TIM10EN                 ((uint32_t)0x00020000)
#define  RCC_APB2ENR_TIM11EN                 ((uint32_t)0x00040000)
#define  RCC_APB2ENR_SPI5EN                  ((uint32_t)0x00100000)
#define  RCC_APB2ENR_SPI6EN                  ((uint32_t)0x00200000)
#define  RCC_APB2ENR_SAI1EN                  ((uint32_t)0x00400000)
#define  RCC_APB2ENR_SAI2EN                  ((uint32_t)0x00800000)
#define  RCC_APB2ENR_LTDCEN                  ((uint32_t)0x04000000)

/********************  Bit definition for RCC_AHB1LPENR register  *************/
#define  RCC_AHB1LPENR_GPIOALPEN             ((uint32_t)0x00000001)
#define  RCC_AHB1LPENR_GPIOBLPEN             ((uint32_t)0x00000002)
#define  RCC_AHB1LPENR_GPIOCLPEN             ((uint32_t)0x00000004)
#define  RCC_AHB1LPENR_GPIODLPEN             ((uint32_t)0x00000008)
#define  RCC_AHB1LPENR_GPIOELPEN             ((uint32_t)0x00000010)
#define  RCC_AHB1LPENR_GPIOFLPEN             ((uint32_t)0x00000020)
#define  RCC_AHB1LPENR_GPIOGLPEN             ((uint32_t)0x00000040)
#define  RCC_AHB1LPENR_GPIOHLPEN             ((uint32_t)0x00000080)
#define  RCC_AHB1LPENR_GPIOILPEN             ((uint32_t)0x00000100)
#define  RCC_AHB1LPENR_GPIOJLPEN             ((uint32_t)0x00000200)
#define  RCC_AHB1LPENR_GPIOKLPEN             ((uint32_t)0x00000400)

#define  RCC_AHB1LPENR_CRCLPEN               ((uint32_t)0x00001000)
#define  RCC_AHB1LPENR_AXILPEN               ((uint32_t)0x00002000)
#define  RCC_AHB1LPENR_FLITFLPEN             ((uint32_t)0x00008000)
#define  RCC_AHB1LPENR_SRAM1LPEN             ((uint32_t)0x00010000)
#define  RCC_AHB1LPENR_SRAM2LPEN             ((uint32_t)0x00020000)
#define  RCC_AHB1LPENR_BKPSRAMLPEN           ((uint32_t)0x00040000)
#define  RCC_AHB1LPENR_DTCMLPEN              ((uint32_t)0x00100000)
#define  RCC_AHB1LPENR_DMA1LPEN              ((uint32_t)0x00200000)
#define  RCC_AHB1LPENR_DMA2LPEN              ((uint32_t)0x00400000)
#define  RCC_AHB1LPENR_DMA2DLPEN             ((uint32_t)0x00800000)
#define  RCC_AHB1LPENR_ETHMACLPEN            ((uint32_t)0x02000000)
#define  RCC_AHB1LPENR_ETHMACTXLPEN          ((uint32_t)0x04000000)
#define  RCC_AHB1LPENR_ETHMACRXLPEN          ((uint32_t)0x08000000)
#define  RCC_AHB1LPENR_ETHMACPTPLPEN         ((uint32_t)0x10000000)
#define  RCC_AHB1LPENR_OTGHSLPEN             ((uint32_t)0x20000000)
#define  RCC_AHB1LPENR_OTGHSULPILPEN         ((uint32_t)0x40000000)

/********************  Bit definition for RCC_AHB2LPENR register  *************/
#define  RCC_AHB2LPENR_DCMILPEN              ((uint32_t)0x00000001)
#define  RCC_AHB2LPENR_RNGLPEN               ((uint32_t)0x00000040)
#define  RCC_AHB2LPENR_OTGFSLPEN             ((uint32_t)0x00000080)

/********************  Bit definition for RCC_AHB3LPENR register  *************/
#define  RCC_AHB3LPENR_FMCLPEN              ((uint32_t)0x00000001)
#define  RCC_AHB3LPENR_QSPILPEN             ((uint32_t)0x00000002)
/********************  Bit definition for RCC_APB1LPENR register  *************/
#define  RCC_APB1LPENR_TIM2LPEN              ((uint32_t)0x00000001)
#define  RCC_APB1LPENR_TIM3LPEN              ((uint32_t)0x00000002)
#define  RCC_APB1LPENR_TIM4LPEN              ((uint32_t)0x00000004)
#define  RCC_APB1LPENR_TIM5LPEN              ((uint32_t)0x00000008)
#define  RCC_APB1LPENR_TIM6LPEN              ((uint32_t)0x00000010)
#define  RCC_APB1LPENR_TIM7LPEN              ((uint32_t)0x00000020)
#define  RCC_APB1LPENR_TIM12LPEN             ((uint32_t)0x00000040)
#define  RCC_APB1LPENR_TIM13LPEN             ((uint32_t)0x00000080)
#define  RCC_APB1LPENR_TIM14LPEN             ((uint32_t)0x00000100)
#define  RCC_APB1LPENR_LPTIM1LPEN            ((uint32_t)0x00000200)
#define  RCC_APB1LPENR_WWDGLPEN              ((uint32_t)0x00000800)
#define  RCC_APB1LPENR_SPI2LPEN              ((uint32_t)0x00004000)
#define  RCC_APB1LPENR_SPI3LPEN              ((uint32_t)0x00008000)
#define  RCC_APB1LPENR_SPDIFRXLPEN           ((uint32_t)0x00010000)
#define  RCC_APB1LPENR_USART2LPEN            ((uint32_t)0x00020000)
#define  RCC_APB1LPENR_USART3LPEN            ((uint32_t)0x00040000)
#define  RCC_APB1LPENR_UART4LPEN             ((uint32_t)0x00080000)
#define  RCC_APB1LPENR_UART5LPEN             ((uint32_t)0x00100000)
#define  RCC_APB1LPENR_I2C1LPEN              ((uint32_t)0x00200000)
#define  RCC_APB1LPENR_I2C2LPEN              ((uint32_t)0x00400000)
#define  RCC_APB1LPENR_I2C3LPEN              ((uint32_t)0x00800000)
#define  RCC_APB1LPENR_I2C4LPEN              ((uint32_t)0x01000000)
#define  RCC_APB1LPENR_CAN1LPEN              ((uint32_t)0x02000000)
#define  RCC_APB1LPENR_CAN2LPEN              ((uint32_t)0x04000000)
#define  RCC_APB1LPENR_CECLPEN               ((uint32_t)0x08000000)
#define  RCC_APB1LPENR_PWRLPEN               ((uint32_t)0x10000000)
#define  RCC_APB1LPENR_DACLPEN               ((uint32_t)0x20000000)
#define  RCC_APB1LPENR_UART7LPEN             ((uint32_t)0x40000000)
#define  RCC_APB1LPENR_UART8LPEN             ((uint32_t)0x80000000)

/********************  Bit definition for RCC_APB2LPENR register  *************/
#define  RCC_APB2LPENR_TIM1LPEN              ((uint32_t)0x00000001)
#define  RCC_APB2LPENR_TIM8LPEN              ((uint32_t)0x00000002)
#define  RCC_APB2LPENR_USART1LPEN            ((uint32_t)0x00000010)
#define  RCC_APB2LPENR_USART6LPEN            ((uint32_t)0x00000020)
#define  RCC_APB2LPENR_ADC1LPEN              ((uint32_t)0x00000100)
#define  RCC_APB2LPENR_ADC2LPEN              ((uint32_t)0x00000200)
#define  RCC_APB2LPENR_ADC3LPEN              ((uint32_t)0x00000400)
#define  RCC_APB2LPENR_SDMMC1LPEN            ((uint32_t)0x00000800)
#define  RCC_APB2LPENR_SPI1LPEN              ((uint32_t)0x00001000)
#define  RCC_APB2LPENR_SPI4LPEN              ((uint32_t)0x00002000)
#define  RCC_APB2LPENR_SYSCFGLPEN            ((uint32_t)0x00004000)
#define  RCC_APB2LPENR_TIM9LPEN              ((uint32_t)0x00010000)
#define  RCC_APB2LPENR_TIM10LPEN             ((uint32_t)0x00020000)
#define  RCC_APB2LPENR_TIM11LPEN             ((uint32_t)0x00040000)
#define  RCC_APB2LPENR_SPI5LPEN              ((uint32_t)0x00100000)
#define  RCC_APB2LPENR_SPI6LPEN              ((uint32_t)0x00200000)
#define  RCC_APB2LPENR_SAI1LPEN              ((uint32_t)0x00400000)
#define  RCC_APB2LPENR_SAI2LPEN              ((uint32_t)0x00800000)
#define  RCC_APB2LPENR_LTDCLPEN              ((uint32_t)0x04000000)

/********************  Bit definition for RCC_BDCR register  ******************/
#define  RCC_BDCR_LSEON                      ((uint32_t)0x00000001)
#define  RCC_BDCR_LSERDY                     ((uint32_t)0x00000002)
#define  RCC_BDCR_LSEBYP                     ((uint32_t)0x00000004)
#define  RCC_BDCR_LSEDRV                     ((uint32_t)0x00000018)
#define  RCC_BDCR_LSEDRV_0                   ((uint32_t)0x00000008)
#define  RCC_BDCR_LSEDRV_1                   ((uint32_t)0x00000010)
#define  RCC_BDCR_RTCSEL                     ((uint32_t)0x00000300)
#define  RCC_BDCR_RTCSEL_0                   ((uint32_t)0x00000100)
#define  RCC_BDCR_RTCSEL_1                   ((uint32_t)0x00000200)
#define  RCC_BDCR_RTCEN                      ((uint32_t)0x00008000)
#define  RCC_BDCR_BDRST                      ((uint32_t)0x00010000)

/********************  Bit definition for RCC_CSR register  *******************/
#define  RCC_CSR_LSION                       ((uint32_t)0x00000001)
#define  RCC_CSR_LSIRDY                      ((uint32_t)0x00000002)
#define  RCC_CSR_RMVF                        ((uint32_t)0x01000000)
#define  RCC_CSR_BORRSTF                     ((uint32_t)0x02000000)
#define  RCC_CSR_PINRSTF                     ((uint32_t)0x04000000)
#define  RCC_CSR_PORRSTF                     ((uint32_t)0x08000000)
#define  RCC_CSR_SFTRSTF                     ((uint32_t)0x10000000)
#define  RCC_CSR_IWDGRSTF                    ((uint32_t)0x20000000)
#define  RCC_CSR_WWDGRSTF                    ((uint32_t)0x40000000)
#define  RCC_CSR_LPWRRSTF                    ((uint32_t)0x80000000)

/********************  Bit definition for RCC_SSCGR register  *****************/
#define  RCC_SSCGR_MODPER                    ((uint32_t)0x00001FFF)
#define  RCC_SSCGR_INCSTEP                   ((uint32_t)0x0FFFE000)
#define  RCC_SSCGR_SPREADSEL                 ((uint32_t)0x40000000)
#define  RCC_SSCGR_SSCGEN                    ((uint32_t)0x80000000)

/********************  Bit definition for RCC_PLLI2SCFGR register  ************/
#define  RCC_PLLI2SCFGR_PLLI2SN              ((uint32_t)0x00007FC0)
#define  RCC_PLLI2SCFGR_PLLI2SN_0            ((uint32_t)0x00000040)
#define  RCC_PLLI2SCFGR_PLLI2SN_1            ((uint32_t)0x00000080)
#define  RCC_PLLI2SCFGR_PLLI2SN_2            ((uint32_t)0x00000100)
#define  RCC_PLLI2SCFGR_PLLI2SN_3            ((uint32_t)0x00000200)
#define  RCC_PLLI2SCFGR_PLLI2SN_4            ((uint32_t)0x00000400)
#define  RCC_PLLI2SCFGR_PLLI2SN_5            ((uint32_t)0x00000800)
#define  RCC_PLLI2SCFGR_PLLI2SN_6            ((uint32_t)0x00001000)
#define  RCC_PLLI2SCFGR_PLLI2SN_7            ((uint32_t)0x00002000)
#define  RCC_PLLI2SCFGR_PLLI2SN_8            ((uint32_t)0x00004000)

#define  RCC_PLLI2SCFGR_PLLI2SP              ((uint32_t)0x00030000)
#define  RCC_PLLI2SCFGR_PLLI2SP_0            ((uint32_t)0x00010000)
#define  RCC_PLLI2SCFGR_PLLI2SP_1            ((uint32_t)0x00020000)

#define  RCC_PLLI2SCFGR_PLLI2SQ              ((uint32_t)0x0F000000)
#define  RCC_PLLI2SCFGR_PLLI2SQ_0            ((uint32_t)0x01000000)
#define  RCC_PLLI2SCFGR_PLLI2SQ_1            ((uint32_t)0x02000000)
#define  RCC_PLLI2SCFGR_PLLI2SQ_2            ((uint32_t)0x04000000)
#define  RCC_PLLI2SCFGR_PLLI2SQ_3            ((uint32_t)0x08000000)

#define  RCC_PLLI2SCFGR_PLLI2SR              ((uint32_t)0x70000000)
#define  RCC_PLLI2SCFGR_PLLI2SR_0            ((uint32_t)0x10000000)
#define  RCC_PLLI2SCFGR_PLLI2SR_1            ((uint32_t)0x20000000)
#define  RCC_PLLI2SCFGR_PLLI2SR_2            ((uint32_t)0x40000000)

/********************  Bit definition for RCC_PLLSAICFGR register  ************/
#define  RCC_PLLSAICFGR_PLLSAIN              ((uint32_t)0x00007FC0)
#define  RCC_PLLSAICFGR_PLLSAIN_0            ((uint32_t)0x00000040)
#define  RCC_PLLSAICFGR_PLLSAIN_1            ((uint32_t)0x00000080)
#define  RCC_PLLSAICFGR_PLLSAIN_2            ((uint32_t)0x00000100)
#define  RCC_PLLSAICFGR_PLLSAIN_3            ((uint32_t)0x00000200)
#define  RCC_PLLSAICFGR_PLLSAIN_4            ((uint32_t)0x00000400)
#define  RCC_PLLSAICFGR_PLLSAIN_5            ((uint32_t)0x00000800)
#define  RCC_PLLSAICFGR_PLLSAIN_6            ((uint32_t)0x00001000)
#define  RCC_PLLSAICFGR_PLLSAIN_7            ((uint32_t)0x00002000)
#define  RCC_PLLSAICFGR_PLLSAIN_8            ((uint32_t)0x00004000)

#define  RCC_PLLSAICFGR_PLLSAIP              ((uint32_t)0x00030000)
#define  RCC_PLLSAICFGR_PLLSAIP_0            ((uint32_t)0x00010000)
#define  RCC_PLLSAICFGR_PLLSAIP_1            ((uint32_t)0x00020000)

#define  RCC_PLLSAICFGR_PLLSAIQ              ((uint32_t)0x0F000000)
#define  RCC_PLLSAICFGR_PLLSAIQ_0            ((uint32_t)0x01000000)
#define  RCC_PLLSAICFGR_PLLSAIQ_1            ((uint32_t)0x02000000)
#define  RCC_PLLSAICFGR_PLLSAIQ_2            ((uint32_t)0x04000000)
#define  RCC_PLLSAICFGR_PLLSAIQ_3            ((uint32_t)0x08000000)

#define  RCC_PLLSAICFGR_PLLSAIR              ((uint32_t)0x70000000)
#define  RCC_PLLSAICFGR_PLLSAIR_0            ((uint32_t)0x10000000)
#define  RCC_PLLSAICFGR_PLLSAIR_1            ((uint32_t)0x20000000)
#define  RCC_PLLSAICFGR_PLLSAIR_2            ((uint32_t)0x40000000)

/********************  Bit definition for RCC_DCKCFGR1 register  ***************/
#define  RCC_DCKCFGR1_PLLI2SDIVQ              ((uint32_t)0x0000001F)
#define  RCC_DCKCFGR1_PLLI2SDIVQ_0            ((uint32_t)0x00000001)
#define  RCC_DCKCFGR1_PLLI2SDIVQ_1            ((uint32_t)0x00000002)
#define  RCC_DCKCFGR1_PLLI2SDIVQ_2            ((uint32_t)0x00000004)
#define  RCC_DCKCFGR1_PLLI2SDIVQ_3            ((uint32_t)0x00000008)
#define  RCC_DCKCFGR1_PLLI2SDIVQ_4            ((uint32_t)0x00000010)

#define  RCC_DCKCFGR1_PLLSAIDIVQ              ((uint32_t)0x00001F00)
#define  RCC_DCKCFGR1_PLLSAIDIVQ_0            ((uint32_t)0x00000100)
#define  RCC_DCKCFGR1_PLLSAIDIVQ_1            ((uint32_t)0x00000200)
#define  RCC_DCKCFGR1_PLLSAIDIVQ_2            ((uint32_t)0x00000400)
#define  RCC_DCKCFGR1_PLLSAIDIVQ_3            ((uint32_t)0x00000800)
#define  RCC_DCKCFGR1_PLLSAIDIVQ_4            ((uint32_t)0x00001000)

#define  RCC_DCKCFGR1_PLLSAIDIVR              ((uint32_t)0x00030000)
#define  RCC_DCKCFGR1_PLLSAIDIVR_0            ((uint32_t)0x00010000)
#define  RCC_DCKCFGR1_PLLSAIDIVR_1            ((uint32_t)0x00020000)

#define  RCC_DCKCFGR1_SAI1SEL                 ((uint32_t)0x00300000)
#define  RCC_DCKCFGR1_SAI1SEL_0               ((uint32_t)0x00100000)
#define  RCC_DCKCFGR1_SAI1SEL_1               ((uint32_t)0x00200000)

#define  RCC_DCKCFGR1_SAI2SEL                 ((uint32_t)0x00C00000)
#define  RCC_DCKCFGR1_SAI2SEL_0               ((uint32_t)0x00400000)
#define  RCC_DCKCFGR1_SAI2SEL_1               ((uint32_t)0x00800000)

#define  RCC_DCKCFGR1_TIMPRE                  ((uint32_t)0x01000000)

/********************  Bit definition for RCC_DCKCFGR2 register  ***************/
#define  RCC_DCKCFGR2_USART1SEL              ((uint32_t)0x00000003)
#define  RCC_DCKCFGR2_USART1SEL_0            ((uint32_t)0x00000001)
#define  RCC_DCKCFGR2_USART1SEL_1            ((uint32_t)0x00000002)
#define  RCC_DCKCFGR2_USART2SEL              ((uint32_t)0x0000000C)
#define  RCC_DCKCFGR2_USART2SEL_0            ((uint32_t)0x00000004)
#define  RCC_DCKCFGR2_USART2SEL_1            ((uint32_t)0x00000008)
#define  RCC_DCKCFGR2_USART3SEL              ((uint32_t)0x00000030)
#define  RCC_DCKCFGR2_USART3SEL_0            ((uint32_t)0x00000010)
#define  RCC_DCKCFGR2_USART3SEL_1            ((uint32_t)0x00000020)
#define  RCC_DCKCFGR2_UART4SEL               ((uint32_t)0x000000C0)
#define  RCC_DCKCFGR2_UART4SEL_0             ((uint32_t)0x00000040)
#define  RCC_DCKCFGR2_UART4SEL_1             ((uint32_t)0x00000080)
#define  RCC_DCKCFGR2_UART5SEL               ((uint32_t)0x00000300)
#define  RCC_DCKCFGR2_UART5SEL_0             ((uint32_t)0x00000100)
#define  RCC_DCKCFGR2_UART5SEL_1             ((uint32_t)0x00000200)
#define  RCC_DCKCFGR2_USART6SEL              ((uint32_t)0x00000C00)
#define  RCC_DCKCFGR2_USART6SEL_0            ((uint32_t)0x00000400)
#define  RCC_DCKCFGR2_USART6SEL_1            ((uint32_t)0x00000800)
#define  RCC_DCKCFGR2_UART7SEL               ((uint32_t)0x00003000)
#define  RCC_DCKCFGR2_UART7SEL_0             ((uint32_t)0x00001000)
#define  RCC_DCKCFGR2_UART7SEL_1             ((uint32_t)0x00002000)
#define  RCC_DCKCFGR2_UART8SEL               ((uint32_t)0x0000C000)
#define  RCC_DCKCFGR2_UART8SEL_0             ((uint32_t)0x00004000)
#define  RCC_DCKCFGR2_UART8SEL_1             ((uint32_t)0x00008000)
#define  RCC_DCKCFGR2_I2C1SEL                ((uint32_t)0x00030000)
#define  RCC_DCKCFGR2_I2C1SEL_0              ((uint32_t)0x00010000)
#define  RCC_DCKCFGR2_I2C1SEL_1              ((uint32_t)0x00020000)
#define  RCC_DCKCFGR2_I2C2SEL                ((uint32_t)0x000C0000)
#define  RCC_DCKCFGR2_I2C2SEL_0              ((uint32_t)0x00040000)
#define  RCC_DCKCFGR2_I2C2SEL_1              ((uint32_t)0x00080000)
#define  RCC_DCKCFGR2_I2C3SEL                ((uint32_t)0x00300000)
#define  RCC_DCKCFGR2_I2C3SEL_0              ((uint32_t)0x00100000)
#define  RCC_DCKCFGR2_I2C3SEL_1              ((uint32_t)0x00200000)
#define  RCC_DCKCFGR2_I2C4SEL                ((uint32_t)0x00C00000)
#define  RCC_DCKCFGR2_I2C4SEL_0              ((uint32_t)0x00400000)
#define  RCC_DCKCFGR2_I2C4SEL_1              ((uint32_t)0x00800000)
#define  RCC_DCKCFGR2_LPTIM1SEL              ((uint32_t)0x03000000)
#define  RCC_DCKCFGR2_LPTIM1SEL_0            ((uint32_t)0x01000000)
#define  RCC_DCKCFGR2_LPTIM1SEL_1            ((uint32_t)0x02000000)
#define  RCC_DCKCFGR2_CECSEL                 ((uint32_t)0x04000000)
#define  RCC_DCKCFGR2_CK48MSEL               ((uint32_t)0x08000000)
#define  RCC_DCKCFGR2_SDMMC1SEL              ((uint32_t)0x10000000)

/******************************************************************************/
/*                                                                            */
/*                                    RNG                                     */
/*                                                                            */
/******************************************************************************/
/********************  Bits definition for RNG_CR register  *******************/
#define RNG_CR_RNGEN                         ((uint32_t)0x00000004)
#define RNG_CR_IE                            ((uint32_t)0x00000008)

/********************  Bits definition for RNG_SR register  *******************/
#define RNG_SR_DRDY                          ((uint32_t)0x00000001)
#define RNG_SR_CECS                          ((uint32_t)0x00000002)
#define RNG_SR_SECS                          ((uint32_t)0x00000004)
#define RNG_SR_CEIS                          ((uint32_t)0x00000020)
#define RNG_SR_SEIS                          ((uint32_t)0x00000040)

/******************************************************************************/
/*                                                                            */
/*                           Real-Time Clock (RTC)                            */
/*                                                                            */
/******************************************************************************/
/********************  Bits definition for RTC_TR register  *******************/
#define RTC_TR_PM                            ((uint32_t)0x00400000)
#define RTC_TR_HT                            ((uint32_t)0x00300000)
#define RTC_TR_HT_0                          ((uint32_t)0x00100000)
#define RTC_TR_HT_1                          ((uint32_t)0x00200000)
#define RTC_TR_HU                            ((uint32_t)0x000F0000)
#define RTC_TR_HU_0                          ((uint32_t)0x00010000)
#define RTC_TR_HU_1                          ((uint32_t)0x00020000)
#define RTC_TR_HU_2                          ((uint32_t)0x00040000)
#define RTC_TR_HU_3                          ((uint32_t)0x00080000)
#define RTC_TR_MNT                           ((uint32_t)0x00007000)
#define RTC_TR_MNT_0                         ((uint32_t)0x00001000)
#define RTC_TR_MNT_1                         ((uint32_t)0x00002000)
#define RTC_TR_MNT_2                         ((uint32_t)0x00004000)
#define RTC_TR_MNU                           ((uint32_t)0x00000F00)
#define RTC_TR_MNU_0                         ((uint32_t)0x00000100)
#define RTC_TR_MNU_1                         ((uint32_t)0x00000200)
#define RTC_TR_MNU_2                         ((uint32_t)0x00000400)
#define RTC_TR_MNU_3                         ((uint32_t)0x00000800)
#define RTC_TR_ST                            ((uint32_t)0x00000070)
#define RTC_TR_ST_0                          ((uint32_t)0x00000010)
#define RTC_TR_ST_1                          ((uint32_t)0x00000020)
#define RTC_TR_ST_2                          ((uint32_t)0x00000040)
#define RTC_TR_SU                            ((uint32_t)0x0000000F)
#define RTC_TR_SU_0                          ((uint32_t)0x00000001)
#define RTC_TR_SU_1                          ((uint32_t)0x00000002)
#define RTC_TR_SU_2                          ((uint32_t)0x00000004)
#define RTC_TR_SU_3                          ((uint32_t)0x00000008)

/********************  Bits definition for RTC_DR register  *******************/
#define RTC_DR_YT                            ((uint32_t)0x00F00000)
#define RTC_DR_YT_0                          ((uint32_t)0x00100000)
#define RTC_DR_YT_1                          ((uint32_t)0x00200000)
#define RTC_DR_YT_2                          ((uint32_t)0x00400000)
#define RTC_DR_YT_3                          ((uint32_t)0x00800000)
#define RTC_DR_YU                            ((uint32_t)0x000F0000)
#define RTC_DR_YU_0                          ((uint32_t)0x00010000)
#define RTC_DR_YU_1                          ((uint32_t)0x00020000)
#define RTC_DR_YU_2                          ((uint32_t)0x00040000)
#define RTC_DR_YU_3                          ((uint32_t)0x00080000)
#define RTC_DR_WDU                           ((uint32_t)0x0000E000)
#define RTC_DR_WDU_0                         ((uint32_t)0x00002000)
#define RTC_DR_WDU_1                         ((uint32_t)0x00004000)
#define RTC_DR_WDU_2                         ((uint32_t)0x00008000)
#define RTC_DR_MT                            ((uint32_t)0x00001000)
#define RTC_DR_MU                            ((uint32_t)0x00000F00)
#define RTC_DR_MU_0                          ((uint32_t)0x00000100)
#define RTC_DR_MU_1                          ((uint32_t)0x00000200)
#define RTC_DR_MU_2                          ((uint32_t)0x00000400)
#define RTC_DR_MU_3                          ((uint32_t)0x00000800)
#define RTC_DR_DT                            ((uint32_t)0x00000030)
#define RTC_DR_DT_0                          ((uint32_t)0x00000010)
#define RTC_DR_DT_1                          ((uint32_t)0x00000020)
#define RTC_DR_DU                            ((uint32_t)0x0000000F)
#define RTC_DR_DU_0                          ((uint32_t)0x00000001)
#define RTC_DR_DU_1                          ((uint32_t)0x00000002)
#define RTC_DR_DU_2                          ((uint32_t)0x00000004)
#define RTC_DR_DU_3                          ((uint32_t)0x00000008)

/********************  Bits definition for RTC_CR register  *******************/
#define RTC_CR_ITSE                          ((uint32_t)0x01000000)
#define RTC_CR_COE                           ((uint32_t)0x00800000)
#define RTC_CR_OSEL                          ((uint32_t)0x00600000)
#define RTC_CR_OSEL_0                        ((uint32_t)0x00200000)
#define RTC_CR_OSEL_1                        ((uint32_t)0x00400000)
#define RTC_CR_POL                           ((uint32_t)0x00100000)
#define RTC_CR_COSEL                         ((uint32_t)0x00080000)
#define RTC_CR_BCK                           ((uint32_t)0x00040000)
#define RTC_CR_SUB1H                         ((uint32_t)0x00020000)
#define RTC_CR_ADD1H                         ((uint32_t)0x00010000)
#define RTC_CR_TSIE                          ((uint32_t)0x00008000)
#define RTC_CR_WUTIE                         ((uint32_t)0x00004000)
#define RTC_CR_ALRBIE                        ((uint32_t)0x00002000)
#define RTC_CR_ALRAIE                        ((uint32_t)0x00001000)
#define RTC_CR_TSE                           ((uint32_t)0x00000800)
#define RTC_CR_WUTE                          ((uint32_t)0x00000400)
#define RTC_CR_ALRBE                         ((uint32_t)0x00000200)
#define RTC_CR_ALRAE                         ((uint32_t)0x00000100)
#define RTC_CR_FMT                           ((uint32_t)0x00000040)
#define RTC_CR_BYPSHAD                       ((uint32_t)0x00000020)
#define RTC_CR_REFCKON                       ((uint32_t)0x00000010)
#define RTC_CR_TSEDGE                        ((uint32_t)0x00000008)
#define RTC_CR_WUCKSEL                       ((uint32_t)0x00000007)
#define RTC_CR_WUCKSEL_0                     ((uint32_t)0x00000001)
#define RTC_CR_WUCKSEL_1                     ((uint32_t)0x00000002)
#define RTC_CR_WUCKSEL_2                     ((uint32_t)0x00000004)

/********************  Bits definition for RTC_ISR register  ******************/
#define RTC_ISR_ITSF                         ((uint32_t)0x00020000)
#define RTC_ISR_RECALPF                      ((uint32_t)0x00010000)
#define RTC_ISR_TAMP3F                       ((uint32_t)0x00008000)
#define RTC_ISR_TAMP2F                       ((uint32_t)0x00004000)
#define RTC_ISR_TAMP1F                       ((uint32_t)0x00002000)
#define RTC_ISR_TSOVF                        ((uint32_t)0x00001000)
#define RTC_ISR_TSF                          ((uint32_t)0x00000800)
#define RTC_ISR_WUTF                         ((uint32_t)0x00000400)
#define RTC_ISR_ALRBF                        ((uint32_t)0x00000200)
#define RTC_ISR_ALRAF                        ((uint32_t)0x00000100)
#define RTC_ISR_INIT                         ((uint32_t)0x00000080)
#define RTC_ISR_INITF                        ((uint32_t)0x00000040)
#define RTC_ISR_RSF                          ((uint32_t)0x00000020)
#define RTC_ISR_INITS                        ((uint32_t)0x00000010)
#define RTC_ISR_SHPF                         ((uint32_t)0x00000008)
#define RTC_ISR_WUTWF                        ((uint32_t)0x00000004)
#define RTC_ISR_ALRBWF                       ((uint32_t)0x00000002)
#define RTC_ISR_ALRAWF                       ((uint32_t)0x00000001)

/********************  Bits definition for RTC_PRER register  *****************/
#define RTC_PRER_PREDIV_A                    ((uint32_t)0x007F0000)
#define RTC_PRER_PREDIV_S                    ((uint32_t)0x00007FFF)

/********************  Bits definition for RTC_WUTR register  *****************/
#define RTC_WUTR_WUT                         ((uint32_t)0x0000FFFF)

/********************  Bits definition for RTC_ALRMAR register  ***************/
#define RTC_ALRMAR_MSK4                      ((uint32_t)0x80000000)
#define RTC_ALRMAR_WDSEL                     ((uint32_t)0x40000000)
#define RTC_ALRMAR_DT                        ((uint32_t)0x30000000)
#define RTC_ALRMAR_DT_0                      ((uint32_t)0x10000000)
#define RTC_ALRMAR_DT_1                      ((uint32_t)0x20000000)
#define RTC_ALRMAR_DU                        ((uint32_t)0x0F000000)
#define RTC_ALRMAR_DU_0                      ((uint32_t)0x01000000)
#define RTC_ALRMAR_DU_1                      ((uint32_t)0x02000000)
#define RTC_ALRMAR_DU_2                      ((uint32_t)0x04000000)
#define RTC_ALRMAR_DU_3                      ((uint32_t)0x08000000)
#define RTC_ALRMAR_MSK3                      ((uint32_t)0x00800000)
#define RTC_ALRMAR_PM                        ((uint32_t)0x00400000)
#define RTC_ALRMAR_HT                        ((uint32_t)0x00300000)
#define RTC_ALRMAR_HT_0                      ((uint32_t)0x00100000)
#define RTC_ALRMAR_HT_1                      ((uint32_t)0x00200000)
#define RTC_ALRMAR_HU                        ((uint32_t)0x000F0000)
#define RTC_ALRMAR_HU_0                      ((uint32_t)0x00010000)
#define RTC_ALRMAR_HU_1                      ((uint32_t)0x00020000)
#define RTC_ALRMAR_HU_2                      ((uint32_t)0x00040000)
#define RTC_ALRMAR_HU_3                      ((uint32_t)0x00080000)
#define RTC_ALRMAR_MSK2                      ((uint32_t)0x00008000)
#define RTC_ALRMAR_MNT                       ((uint32_t)0x00007000)
#define RTC_ALRMAR_MNT_0                     ((uint32_t)0x00001000)
#define RTC_ALRMAR_MNT_1                     ((uint32_t)0x00002000)
#define RTC_ALRMAR_MNT_2                     ((uint32_t)0x00004000)
#define RTC_ALRMAR_MNU                       ((uint32_t)0x00000F00)
#define RTC_ALRMAR_MNU_0                     ((uint32_t)0x00000100)
#define RTC_ALRMAR_MNU_1                     ((uint32_t)0x00000200)
#define RTC_ALRMAR_MNU_2                     ((uint32_t)0x00000400)
#define RTC_ALRMAR_MNU_3                     ((uint32_t)0x00000800)
#define RTC_ALRMAR_MSK1                      ((uint32_t)0x00000080)
#define RTC_ALRMAR_ST                        ((uint32_t)0x00000070)
#define RTC_ALRMAR_ST_0                      ((uint32_t)0x00000010)
#define RTC_ALRMAR_ST_1                      ((uint32_t)0x00000020)
#define RTC_ALRMAR_ST_2                      ((uint32_t)0x00000040)
#define RTC_ALRMAR_SU                        ((uint32_t)0x0000000F)
#define RTC_ALRMAR_SU_0                      ((uint32_t)0x00000001)
#define RTC_ALRMAR_SU_1                      ((uint32_t)0x00000002)
#define RTC_ALRMAR_SU_2                      ((uint32_t)0x00000004)
#define RTC_ALRMAR_SU_3                      ((uint32_t)0x00000008)

/********************  Bits definition for RTC_ALRMBR register  ***************/
#define RTC_ALRMBR_MSK4                      ((uint32_t)0x80000000)
#define RTC_ALRMBR_WDSEL                     ((uint32_t)0x40000000)
#define RTC_ALRMBR_DT                        ((uint32_t)0x30000000)
#define RTC_ALRMBR_DT_0                      ((uint32_t)0x10000000)
#define RTC_ALRMBR_DT_1                      ((uint32_t)0x20000000)
#define RTC_ALRMBR_DU                        ((uint32_t)0x0F000000)
#define RTC_ALRMBR_DU_0                      ((uint32_t)0x01000000)
#define RTC_ALRMBR_DU_1                      ((uint32_t)0x02000000)
#define RTC_ALRMBR_DU_2                      ((uint32_t)0x04000000)
#define RTC_ALRMBR_DU_3                      ((uint32_t)0x08000000)
#define RTC_ALRMBR_MSK3                      ((uint32_t)0x00800000)
#define RTC_ALRMBR_PM                        ((uint32_t)0x00400000)
#define RTC_ALRMBR_HT                        ((uint32_t)0x00300000)
#define RTC_ALRMBR_HT_0                      ((uint32_t)0x00100000)
#define RTC_ALRMBR_HT_1                      ((uint32_t)0x00200000)
#define RTC_ALRMBR_HU                        ((uint32_t)0x000F0000)
#define RTC_ALRMBR_HU_0                      ((uint32_t)0x00010000)
#define RTC_ALRMBR_HU_1                      ((uint32_t)0x00020000)
#define RTC_ALRMBR_HU_2                      ((uint32_t)0x00040000)
#define RTC_ALRMBR_HU_3                      ((uint32_t)0x00080000)
#define RTC_ALRMBR_MSK2                      ((uint32_t)0x00008000)
#define RTC_ALRMBR_MNT                       ((uint32_t)0x00007000)
#define RTC_ALRMBR_MNT_0                     ((uint32_t)0x00001000)
#define RTC_ALRMBR_MNT_1                     ((uint32_t)0x00002000)
#define RTC_ALRMBR_MNT_2                     ((uint32_t)0x00004000)
#define RTC_ALRMBR_MNU                       ((uint32_t)0x00000F00)
#define RTC_ALRMBR_MNU_0                     ((uint32_t)0x00000100)
#define RTC_ALRMBR_MNU_1                     ((uint32_t)0x00000200)
#define RTC_ALRMBR_MNU_2                     ((uint32_t)0x00000400)
#define RTC_ALRMBR_MNU_3                     ((uint32_t)0x00000800)
#define RTC_ALRMBR_MSK1                      ((uint32_t)0x00000080)
#define RTC_ALRMBR_ST                        ((uint32_t)0x00000070)
#define RTC_ALRMBR_ST_0                      ((uint32_t)0x00000010)
#define RTC_ALRMBR_ST_1                      ((uint32_t)0x00000020)
#define RTC_ALRMBR_ST_2                      ((uint32_t)0x00000040)
#define RTC_ALRMBR_SU                        ((uint32_t)0x0000000F)
#define RTC_ALRMBR_SU_0                      ((uint32_t)0x00000001)
#define RTC_ALRMBR_SU_1                      ((uint32_t)0x00000002)
#define RTC_ALRMBR_SU_2                      ((uint32_t)0x00000004)
#define RTC_ALRMBR_SU_3                      ((uint32_t)0x00000008)

/********************  Bits definition for RTC_WPR register  ******************/
#define RTC_WPR_KEY                          ((uint32_t)0x000000FF)

/********************  Bits definition for RTC_SSR register  ******************/
#define RTC_SSR_SS                           ((uint32_t)0x0000FFFF)

/********************  Bits definition for RTC_SHIFTR register  ***************/
#define RTC_SHIFTR_SUBFS                     ((uint32_t)0x00007FFF)
#define RTC_SHIFTR_ADD1S                     ((uint32_t)0x80000000)

/********************  Bits definition for RTC_TSTR register  *****************/
#define RTC_TSTR_PM                          ((uint32_t)0x00400000)
#define RTC_TSTR_HT                          ((uint32_t)0x00300000)
#define RTC_TSTR_HT_0                        ((uint32_t)0x00100000)
#define RTC_TSTR_HT_1                        ((uint32_t)0x00200000)
#define RTC_TSTR_HU                          ((uint32_t)0x000F0000)
#define RTC_TSTR_HU_0                        ((uint32_t)0x00010000)
#define RTC_TSTR_HU_1                        ((uint32_t)0x00020000)
#define RTC_TSTR_HU_2                        ((uint32_t)0x00040000)
#define RTC_TSTR_HU_3                        ((uint32_t)0x00080000)
#define RTC_TSTR_MNT                         ((uint32_t)0x00007000)
#define RTC_TSTR_MNT_0                       ((uint32_t)0x00001000)
#define RTC_TSTR_MNT_1                       ((uint32_t)0x00002000)
#define RTC_TSTR_MNT_2                       ((uint32_t)0x00004000)
#define RTC_TSTR_MNU                         ((uint32_t)0x00000F00)
#define RTC_TSTR_MNU_0                       ((uint32_t)0x00000100)
#define RTC_TSTR_MNU_1                       ((uint32_t)0x00000200)
#define RTC_TSTR_MNU_2                       ((uint32_t)0x00000400)
#define RTC_TSTR_MNU_3                       ((uint32_t)0x00000800)
#define RTC_TSTR_ST                          ((uint32_t)0x00000070)
#define RTC_TSTR_ST_0                        ((uint32_t)0x00000010)
#define RTC_TSTR_ST_1                        ((uint32_t)0x00000020)
#define RTC_TSTR_ST_2                        ((uint32_t)0x00000040)
#define RTC_TSTR_SU                          ((uint32_t)0x0000000F)
#define RTC_TSTR_SU_0                        ((uint32_t)0x00000001)
#define RTC_TSTR_SU_1                        ((uint32_t)0x00000002)
#define RTC_TSTR_SU_2                        ((uint32_t)0x00000004)
#define RTC_TSTR_SU_3                        ((uint32_t)0x00000008)

/********************  Bits definition for RTC_TSDR register  *****************/
#define RTC_TSDR_WDU                         ((uint32_t)0x0000E000)
#define RTC_TSDR_WDU_0                       ((uint32_t)0x00002000)
#define RTC_TSDR_WDU_1                       ((uint32_t)0x00004000)
#define RTC_TSDR_WDU_2                       ((uint32_t)0x00008000)
#define RTC_TSDR_MT                          ((uint32_t)0x00001000)
#define RTC_TSDR_MU                          ((uint32_t)0x00000F00)
#define RTC_TSDR_MU_0                        ((uint32_t)0x00000100)
#define RTC_TSDR_MU_1                        ((uint32_t)0x00000200)
#define RTC_TSDR_MU_2                        ((uint32_t)0x00000400)
#define RTC_TSDR_MU_3                        ((uint32_t)0x00000800)
#define RTC_TSDR_DT                          ((uint32_t)0x00000030)
#define RTC_TSDR_DT_0                        ((uint32_t)0x00000010)
#define RTC_TSDR_DT_1                        ((uint32_t)0x00000020)
#define RTC_TSDR_DU                          ((uint32_t)0x0000000F)
#define RTC_TSDR_DU_0                        ((uint32_t)0x00000001)
#define RTC_TSDR_DU_1                        ((uint32_t)0x00000002)
#define RTC_TSDR_DU_2                        ((uint32_t)0x00000004)
#define RTC_TSDR_DU_3                        ((uint32_t)0x00000008)

/********************  Bits definition for RTC_TSSSR register  ****************/
#define RTC_TSSSR_SS                         ((uint32_t)0x0000FFFF)

/********************  Bits definition for RTC_CAL register  *****************/
#define RTC_CALR_CALP                        ((uint32_t)0x00008000)
#define RTC_CALR_CALW8                       ((uint32_t)0x00004000)
#define RTC_CALR_CALW16                      ((uint32_t)0x00002000)
#define RTC_CALR_CALM                        ((uint32_t)0x000001FF)
#define RTC_CALR_CALM_0                      ((uint32_t)0x00000001)
#define RTC_CALR_CALM_1                      ((uint32_t)0x00000002)
#define RTC_CALR_CALM_2                      ((uint32_t)0x00000004)
#define RTC_CALR_CALM_3                      ((uint32_t)0x00000008)
#define RTC_CALR_CALM_4                      ((uint32_t)0x00000010)
#define RTC_CALR_CALM_5                      ((uint32_t)0x00000020)
#define RTC_CALR_CALM_6                      ((uint32_t)0x00000040)
#define RTC_CALR_CALM_7                      ((uint32_t)0x00000080)
#define RTC_CALR_CALM_8                      ((uint32_t)0x00000100)

/********************  Bits definition for RTC_TAMPCR register  ****************/
#define RTC_TAMPCR_TAMP3MF                    ((uint32_t)0x01000000)
#define RTC_TAMPCR_TAMP3NOERASE               ((uint32_t)0x00800000)
#define RTC_TAMPCR_TAMP3IE                    ((uint32_t)0x00400000)
#define RTC_TAMPCR_TAMP2MF                    ((uint32_t)0x00200000)
#define RTC_TAMPCR_TAMP2NOERASE               ((uint32_t)0x00100000)
#define RTC_TAMPCR_TAMP2IE                    ((uint32_t)0x00080000)
#define RTC_TAMPCR_TAMP1MF                    ((uint32_t)0x00040000)
#define RTC_TAMPCR_TAMP1NOERASE               ((uint32_t)0x00020000)
#define RTC_TAMPCR_TAMP1IE                    ((uint32_t)0x00010000)
#define RTC_TAMPCR_TAMPPUDIS                  ((uint32_t)0x00008000)
#define RTC_TAMPCR_TAMPPRCH                   ((uint32_t)0x00006000)
#define RTC_TAMPCR_TAMPPRCH_0                 ((uint32_t)0x00002000)
#define RTC_TAMPCR_TAMPPRCH_1                 ((uint32_t)0x00004000)
#define RTC_TAMPCR_TAMPFLT                    ((uint32_t)0x00001800)
#define RTC_TAMPCR_TAMPFLT_0                  ((uint32_t)0x00000800)
#define RTC_TAMPCR_TAMPFLT_1                  ((uint32_t)0x00001000)
#define RTC_TAMPCR_TAMPFREQ                   ((uint32_t)0x00000700)
#define RTC_TAMPCR_TAMPFREQ_0                 ((uint32_t)0x00000100)
#define RTC_TAMPCR_TAMPFREQ_1                 ((uint32_t)0x00000200)
#define RTC_TAMPCR_TAMPFREQ_2                 ((uint32_t)0x00000400)
#define RTC_TAMPCR_TAMPTS                     ((uint32_t)0x00000080)
#define RTC_TAMPCR_TAMP3_TRG                  ((uint32_t)0x00000040)
#define RTC_TAMPCR_TAMP3E                     ((uint32_t)0x00000020)
#define RTC_TAMPCR_TAMP2_TRG                   ((uint32_t)0x00000010)
#define RTC_TAMPCR_TAMP2E                     ((uint32_t)0x00000008)
#define RTC_TAMPCR_TAMPIE                     ((uint32_t)0x00000004)
#define RTC_TAMPCR_TAMP1_TRG                   ((uint32_t)0x00000002)
#define RTC_TAMPCR_TAMP1E                     ((uint32_t)0x00000001)

/********************  Bits definition for RTC_ALRMASSR register  *************/
#define RTC_ALRMASSR_MASKSS                  ((uint32_t)0x0F000000)
#define RTC_ALRMASSR_MASKSS_0                ((uint32_t)0x01000000)
#define RTC_ALRMASSR_MASKSS_1                ((uint32_t)0x02000000)
#define RTC_ALRMASSR_MASKSS_2                ((uint32_t)0x04000000)
#define RTC_ALRMASSR_MASKSS_3                ((uint32_t)0x08000000)
#define RTC_ALRMASSR_SS                      ((uint32_t)0x00007FFF)

/********************  Bits definition for RTC_ALRMBSSR register  *************/
#define RTC_ALRMBSSR_MASKSS                  ((uint32_t)0x0F000000)
#define RTC_ALRMBSSR_MASKSS_0                ((uint32_t)0x01000000)
#define RTC_ALRMBSSR_MASKSS_1                ((uint32_t)0x02000000)
#define RTC_ALRMBSSR_MASKSS_2                ((uint32_t)0x04000000)
#define RTC_ALRMBSSR_MASKSS_3                ((uint32_t)0x08000000)
#define RTC_ALRMBSSR_SS                      ((uint32_t)0x00007FFF)

/********************  Bits definition for RTC_OR register  ****************/
#define RTC_OR_TSINSEL                      ((uint32_t)0x00000006)
#define RTC_OR_TSINSEL_0                    ((uint32_t)0x00000002)
#define RTC_OR_TSINSEL_1                    ((uint32_t)0x00000004)
#define RTC_OR_ALARMTYPE                    ((uint32_t)0x00000008)


/********************  Bits definition for RTC_BKP0R register  ****************/
#define RTC_BKP0R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP1R register  ****************/
#define RTC_BKP1R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP2R register  ****************/
#define RTC_BKP2R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP3R register  ****************/
#define RTC_BKP3R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP4R register  ****************/
#define RTC_BKP4R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP5R register  ****************/
#define RTC_BKP5R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP6R register  ****************/
#define RTC_BKP6R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP7R register  ****************/
#define RTC_BKP7R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP8R register  ****************/
#define RTC_BKP8R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP9R register  ****************/
#define RTC_BKP9R                            ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP10R register  ***************/
#define RTC_BKP10R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP11R register  ***************/
#define RTC_BKP11R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP12R register  ***************/
#define RTC_BKP12R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP13R register  ***************/
#define RTC_BKP13R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP14R register  ***************/
#define RTC_BKP14R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP15R register  ***************/
#define RTC_BKP15R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP16R register  ***************/
#define RTC_BKP16R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP17R register  ***************/
#define RTC_BKP17R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP18R register  ***************/
#define RTC_BKP18R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP19R register  ***************/
#define RTC_BKP19R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP20R register  ***************/
#define RTC_BKP20R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP21R register  ***************/
#define RTC_BKP21R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP22R register  ***************/
#define RTC_BKP22R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP23R register  ***************/
#define RTC_BKP23R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP24R register  ***************/
#define RTC_BKP24R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP25R register  ***************/
#define RTC_BKP25R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP26R register  ***************/
#define RTC_BKP26R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP27R register  ***************/
#define RTC_BKP27R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP28R register  ***************/
#define RTC_BKP28R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP29R register  ***************/
#define RTC_BKP29R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP30R register  ***************/
#define RTC_BKP30R                           ((uint32_t)0xFFFFFFFF)

/********************  Bits definition for RTC_BKP31R register  ***************/
#define RTC_BKP31R                           ((uint32_t)0xFFFFFFFF)

/******************** Number of backup registers ******************************/
#define RTC_BKP_NUMBER                       ((uint32_t)0x00000020)


/******************************************************************************/
/*                                                                            */
/*                          Serial Audio Interface                            */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for SAI_GCR register  *******************/
#define  SAI_GCR_SYNCIN                  ((uint32_t)0x00000003)        /*!<SYNCIN[1:0] bits (Synchronization Inputs)   */
#define  SAI_GCR_SYNCIN_0                ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  SAI_GCR_SYNCIN_1                ((uint32_t)0x00000002)        /*!<Bit 1 */

#define  SAI_GCR_SYNCOUT                 ((uint32_t)0x00000030)        /*!<SYNCOUT[1:0] bits (Synchronization Outputs) */
#define  SAI_GCR_SYNCOUT_0               ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  SAI_GCR_SYNCOUT_1               ((uint32_t)0x00000020)        /*!<Bit 1 */

/*******************  Bit definition for SAI_xCR1 register  *******************/
#define  SAI_xCR1_MODE                    ((uint32_t)0x00000003)        /*!<MODE[1:0] bits (Audio Block Mode)           */
#define  SAI_xCR1_MODE_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  SAI_xCR1_MODE_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */

#define  SAI_xCR1_PRTCFG                  ((uint32_t)0x0000000C)        /*!<PRTCFG[1:0] bits (Protocol Configuration)   */
#define  SAI_xCR1_PRTCFG_0                ((uint32_t)0x00000004)        /*!<Bit 0 */
#define  SAI_xCR1_PRTCFG_1                ((uint32_t)0x00000008)        /*!<Bit 1 */

#define  SAI_xCR1_DS                      ((uint32_t)0x000000E0)        /*!<DS[1:0] bits (Data Size) */
#define  SAI_xCR1_DS_0                    ((uint32_t)0x00000020)        /*!<Bit 0 */
#define  SAI_xCR1_DS_1                    ((uint32_t)0x00000040)        /*!<Bit 1 */
#define  SAI_xCR1_DS_2                    ((uint32_t)0x00000080)        /*!<Bit 2 */

#define  SAI_xCR1_LSBFIRST                ((uint32_t)0x00000100)        /*!<LSB First Configuration  */
#define  SAI_xCR1_CKSTR                   ((uint32_t)0x00000200)        /*!<ClocK STRobing edge      */

#define  SAI_xCR1_SYNCEN                  ((uint32_t)0x00000C00)        /*!<SYNCEN[1:0](SYNChronization ENable) */
#define  SAI_xCR1_SYNCEN_0                ((uint32_t)0x00000400)        /*!<Bit 0 */
#define  SAI_xCR1_SYNCEN_1                ((uint32_t)0x00000800)        /*!<Bit 1 */

#define  SAI_xCR1_MONO                    ((uint32_t)0x00001000)        /*!<Mono mode                  */
#define  SAI_xCR1_OUTDRIV                 ((uint32_t)0x00002000)        /*!<Output Drive               */
#define  SAI_xCR1_SAIEN                   ((uint32_t)0x00010000)        /*!<Audio Block enable         */
#define  SAI_xCR1_DMAEN                   ((uint32_t)0x00020000)        /*!<DMA enable                 */
#define  SAI_xCR1_NODIV                   ((uint32_t)0x00080000)        /*!<No Divider Configuration   */

#define  SAI_xCR1_MCKDIV                  ((uint32_t)0x00F00000)        /*!<MCKDIV[3:0] (Master ClocK Divider)  */
#define  SAI_xCR1_MCKDIV_0                ((uint32_t)0x00100000)        /*!<Bit 0  */
#define  SAI_xCR1_MCKDIV_1                ((uint32_t)0x00200000)        /*!<Bit 1  */
#define  SAI_xCR1_MCKDIV_2                ((uint32_t)0x00400000)        /*!<Bit 2  */
#define  SAI_xCR1_MCKDIV_3                ((uint32_t)0x00800000)        /*!<Bit 3  */

/*******************  Bit definition for SAI_xCR2 register  *******************/
#define  SAI_xCR2_FTH                     ((uint32_t)0x00000007)        /*!<FTH[2:0](Fifo THreshold)  */
#define  SAI_xCR2_FTH_0                   ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  SAI_xCR2_FTH_1                   ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  SAI_xCR2_FTH_2                   ((uint32_t)0x00000004)        /*!<Bit 2 */

#define  SAI_xCR2_FFLUSH                  ((uint32_t)0x00000008)        /*!<Fifo FLUSH                       */
#define  SAI_xCR2_TRIS                    ((uint32_t)0x00000010)        /*!<TRIState Management on data line */
#define  SAI_xCR2_MUTE                    ((uint32_t)0x00000020)        /*!<Mute mode                        */
#define  SAI_xCR2_MUTEVAL                 ((uint32_t)0x00000040)        /*!<Muate value                      */

#define  SAI_xCR2_MUTECNT                  ((uint32_t)0x00001F80)       /*!<MUTECNT[5:0] (MUTE counter) */
#define  SAI_xCR2_MUTECNT_0               ((uint32_t)0x00000080)        /*!<Bit 0 */
#define  SAI_xCR2_MUTECNT_1               ((uint32_t)0x00000100)        /*!<Bit 1 */
#define  SAI_xCR2_MUTECNT_2               ((uint32_t)0x00000200)        /*!<Bit 2 */
#define  SAI_xCR2_MUTECNT_3               ((uint32_t)0x00000400)        /*!<Bit 3 */
#define  SAI_xCR2_MUTECNT_4               ((uint32_t)0x00000800)        /*!<Bit 4 */
#define  SAI_xCR2_MUTECNT_5               ((uint32_t)0x00001000)        /*!<Bit 5 */

#define  SAI_xCR2_CPL                     ((uint32_t)0x00080000)        /*!< Complement Bit             */

#define  SAI_xCR2_COMP                    ((uint32_t)0x0000C000)        /*!<COMP[1:0] (Companding mode) */
#define  SAI_xCR2_COMP_0                  ((uint32_t)0x00004000)        /*!<Bit 0 */
#define  SAI_xCR2_COMP_1                  ((uint32_t)0x00008000)        /*!<Bit 1 */

/******************  Bit definition for SAI_xFRCR register  *******************/
#define  SAI_xFRCR_FRL                    ((uint32_t)0x000000FF)        /*!<FRL[1:0](Frame length)  */
#define  SAI_xFRCR_FRL_0                  ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  SAI_xFRCR_FRL_1                  ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  SAI_xFRCR_FRL_2                  ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  SAI_xFRCR_FRL_3                  ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  SAI_xFRCR_FRL_4                  ((uint32_t)0x00000010)        /*!<Bit 4 */
#define  SAI_xFRCR_FRL_5                  ((uint32_t)0x00000020)        /*!<Bit 5 */
#define  SAI_xFRCR_FRL_6                  ((uint32_t)0x00000040)        /*!<Bit 6 */
#define  SAI_xFRCR_FRL_7                  ((uint32_t)0x00000080)        /*!<Bit 7 */

#define  SAI_xFRCR_FSALL                  ((uint32_t)0x00007F00)        /*!<FRL[1:0] (Frame synchronization active level length)  */
#define  SAI_xFRCR_FSALL_0                ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  SAI_xFRCR_FSALL_1                ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  SAI_xFRCR_FSALL_2                ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  SAI_xFRCR_FSALL_3                ((uint32_t)0x00000800)        /*!<Bit 3 */
#define  SAI_xFRCR_FSALL_4                ((uint32_t)0x00001000)        /*!<Bit 4 */
#define  SAI_xFRCR_FSALL_5                ((uint32_t)0x00002000)        /*!<Bit 5 */
#define  SAI_xFRCR_FSALL_6                ((uint32_t)0x00004000)        /*!<Bit 6 */

#define  SAI_xFRCR_FSDEF                  ((uint32_t)0x00010000)        /*!< Frame Synchronization Definition */
#define  SAI_xFRCR_FSPO                   ((uint32_t)0x00020000)        /*!<Frame Synchronization POLarity    */
#define  SAI_xFRCR_FSOFF                  ((uint32_t)0x00040000)        /*!<Frame Synchronization OFFset      */

/******************  Bit definition for SAI_xSLOTR register  *******************/
#define  SAI_xSLOTR_FBOFF                 ((uint32_t)0x0000001F)        /*!<FRL[4:0](First Bit Offset)  */
#define  SAI_xSLOTR_FBOFF_0               ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  SAI_xSLOTR_FBOFF_1               ((uint32_t)0x00000002)        /*!<Bit 1 */
#define  SAI_xSLOTR_FBOFF_2               ((uint32_t)0x00000004)        /*!<Bit 2 */
#define  SAI_xSLOTR_FBOFF_3               ((uint32_t)0x00000008)        /*!<Bit 3 */
#define  SAI_xSLOTR_FBOFF_4               ((uint32_t)0x00000010)        /*!<Bit 4 */

#define  SAI_xSLOTR_SLOTSZ                ((uint32_t)0x000000C0)        /*!<SLOTSZ[1:0] (Slot size)  */
#define  SAI_xSLOTR_SLOTSZ_0              ((uint32_t)0x00000040)        /*!<Bit 0 */
#define  SAI_xSLOTR_SLOTSZ_1              ((uint32_t)0x00000080)        /*!<Bit 1 */

#define  SAI_xSLOTR_NBSLOT                ((uint32_t)0x00000F00)        /*!<NBSLOT[3:0] (Number of Slot in audio Frame)  */
#define  SAI_xSLOTR_NBSLOT_0              ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  SAI_xSLOTR_NBSLOT_1              ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  SAI_xSLOTR_NBSLOT_2              ((uint32_t)0x00000400)        /*!<Bit 2 */
#define  SAI_xSLOTR_NBSLOT_3              ((uint32_t)0x00000800)        /*!<Bit 3 */

#define  SAI_xSLOTR_SLOTEN                ((uint32_t)0xFFFF0000)        /*!<SLOTEN[15:0] (Slot Enable)  */

/*******************  Bit definition for SAI_xIMR register  *******************/
#define  SAI_xIMR_OVRUDRIE                ((uint32_t)0x00000001)        /*!<Overrun underrun interrupt enable                              */
#define  SAI_xIMR_MUTEDETIE               ((uint32_t)0x00000002)        /*!<Mute detection interrupt enable                                */
#define  SAI_xIMR_WCKCFGIE                ((uint32_t)0x00000004)        /*!<Wrong Clock Configuration interrupt enable                     */
#define  SAI_xIMR_FREQIE                  ((uint32_t)0x00000008)        /*!<FIFO request interrupt enable                                  */
#define  SAI_xIMR_CNRDYIE                 ((uint32_t)0x00000010)        /*!<Codec not ready interrupt enable                               */
#define  SAI_xIMR_AFSDETIE                ((uint32_t)0x00000020)        /*!<Anticipated frame synchronization detection interrupt enable   */
#define  SAI_xIMR_LFSDETIE                ((uint32_t)0x00000040)        /*!<Late frame synchronization detection interrupt enable          */

/********************  Bit definition for SAI_xSR register  *******************/
#define  SAI_xSR_OVRUDR                   ((uint32_t)0x00000001)         /*!<Overrun underrun                               */
#define  SAI_xSR_MUTEDET                  ((uint32_t)0x00000002)         /*!<Mute detection                                 */
#define  SAI_xSR_WCKCFG                   ((uint32_t)0x00000004)         /*!<Wrong Clock Configuration                      */
#define  SAI_xSR_FREQ                     ((uint32_t)0x00000008)         /*!<FIFO request                                   */
#define  SAI_xSR_CNRDY                    ((uint32_t)0x00000010)         /*!<Codec not ready                                */
#define  SAI_xSR_AFSDET                   ((uint32_t)0x00000020)         /*!<Anticipated frame synchronization detection    */
#define  SAI_xSR_LFSDET                   ((uint32_t)0x00000040)         /*!<Late frame synchronization detection           */

#define  SAI_xSR_FLVL                     ((uint32_t)0x00070000)         /*!<FLVL[2:0] (FIFO Level Threshold)               */
#define  SAI_xSR_FLVL_0                   ((uint32_t)0x00010000)         /*!<Bit 0 */
#define  SAI_xSR_FLVL_1                   ((uint32_t)0x00020000)         /*!<Bit 1 */
#define  SAI_xSR_FLVL_2                   ((uint32_t)0x00040000)         /*!<Bit 2 */

/******************  Bit definition for SAI_xCLRFR register  ******************/
#define  SAI_xCLRFR_COVRUDR               ((uint32_t)0x00000001)        /*!<Clear Overrun underrun                               */
#define  SAI_xCLRFR_CMUTEDET              ((uint32_t)0x00000002)        /*!<Clear Mute detection                                 */
#define  SAI_xCLRFR_CWCKCFG               ((uint32_t)0x00000004)        /*!<Clear Wrong Clock Configuration                      */
#define  SAI_xCLRFR_CFREQ                 ((uint32_t)0x00000008)        /*!<Clear FIFO request                                   */
#define  SAI_xCLRFR_CCNRDY                ((uint32_t)0x00000010)        /*!<Clear Codec not ready                                */
#define  SAI_xCLRFR_CAFSDET               ((uint32_t)0x00000020)        /*!<Clear Anticipated frame synchronization detection    */
#define  SAI_xCLRFR_CLFSDET               ((uint32_t)0x00000040)        /*!<Clear Late frame synchronization detection           */

/******************  Bit definition for SAI_xDR register  *********************/
#define  SAI_xDR_DATA                     ((uint32_t)0xFFFFFFFF)

/******************************************************************************/
/*                                                                            */
/*                              SPDIF-RX Interface                            */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for SPDIF_CR register  *******************/
#define  SPDIFRX_CR_SPDIFEN                  ((uint32_t)0x00000003)        /*!<Peripheral Block Enable                      */
#define  SPDIFRX_CR_RXDMAEN                  ((uint32_t)0x00000004)        /*!<Receiver DMA Enable for data flow            */
#define  SPDIFRX_CR_RXSTEO                   ((uint32_t)0x00000008)        /*!<Stereo Mode                                  */
#define  SPDIFRX_CR_DRFMT                    ((uint32_t)0x00000030)        /*!<RX Data format                               */
#define  SPDIFRX_CR_PMSK                     ((uint32_t)0x00000040)        /*!<Mask Parity error bit                        */
#define  SPDIFRX_CR_VMSK                     ((uint32_t)0x00000080)        /*!<Mask of Validity bit                         */
#define  SPDIFRX_CR_CUMSK                    ((uint32_t)0x00000100)        /*!<Mask of channel status and user bits         */
#define  SPDIFRX_CR_PTMSK                    ((uint32_t)0x00000200)        /*!<Mask of Preamble Type bits                   */
#define  SPDIFRX_CR_CBDMAEN                  ((uint32_t)0x00000400)        /*!<Control Buffer DMA ENable for control flow   */
#define  SPDIFRX_CR_CHSEL                    ((uint32_t)0x00000800)        /*!<Channel Selection                            */
#define  SPDIFRX_CR_NBTR                     ((uint32_t)0x00003000)        /*!<Maximum allowed re-tries during synchronization phase */
#define  SPDIFRX_CR_WFA                      ((uint32_t)0x00004000)        /*!<Wait For Activity     */
#define  SPDIFRX_CR_INSEL                    ((uint32_t)0x00070000)        /*!<SPDIF input selection */

/*******************  Bit definition for SPDIFRX_IMR register  *******************/
#define  SPDIFRX_IMR_RXNEIE                   ((uint32_t)0x00000001)        /*!<RXNE interrupt enable                              */
#define  SPDIFRX_IMR_CSRNEIE                  ((uint32_t)0x00000002)        /*!<Control Buffer Ready Interrupt Enable              */
#define  SPDIFRX_IMR_PERRIE                   ((uint32_t)0x00000004)        /*!<Parity error interrupt enable                      */
#define  SPDIFRX_IMR_OVRIE                    ((uint32_t)0x00000008)        /*!<Overrun error Interrupt Enable                     */
#define  SPDIFRX_IMR_SBLKIE                   ((uint32_t)0x00000010)        /*!<Synchronization Block Detected Interrupt Enable    */
#define  SPDIFRX_IMR_SYNCDIE                  ((uint32_t)0x00000020)        /*!<Synchronization Done                               */
#define  SPDIFRX_IMR_IFEIE                    ((uint32_t)0x00000040)        /*!<Serial Interface Error Interrupt Enable            */

/*******************  Bit definition for SPDIFRX_SR register  *******************/
#define  SPDIFRX_SR_RXNE                   ((uint32_t)0x00000001)       /*!<Read data register not empty                          */
#define  SPDIFRX_SR_CSRNE                  ((uint32_t)0x00000002)       /*!<The Control Buffer register is not empty              */
#define  SPDIFRX_SR_PERR                   ((uint32_t)0x00000004)       /*!<Parity error                                          */
#define  SPDIFRX_SR_OVR                    ((uint32_t)0x00000008)       /*!<Overrun error                                         */
#define  SPDIFRX_SR_SBD                    ((uint32_t)0x00000010)       /*!<Synchronization Block Detected                        */
#define  SPDIFRX_SR_SYNCD                  ((uint32_t)0x00000020)       /*!<Synchronization Done                                  */
#define  SPDIFRX_SR_FERR                   ((uint32_t)0x00000040)       /*!<Framing error                                         */
#define  SPDIFRX_SR_SERR                   ((uint32_t)0x00000080)       /*!<Synchronization error                                 */
#define  SPDIFRX_SR_TERR                   ((uint32_t)0x00000100)       /*!<Time-out error                                        */
#define  SPDIFRX_SR_WIDTH5                 ((uint32_t)0x7FFF0000)       /*!<Duration of 5 symbols counted with spdif_clk          */

/*******************  Bit definition for SPDIFRX_IFCR register  *******************/
#define  SPDIFRX_IFCR_PERRCF               ((uint32_t)0x00000004)       /*!<Clears the Parity error flag                         */
#define  SPDIFRX_IFCR_OVRCF                ((uint32_t)0x00000008)       /*!<Clears the Overrun error flag                        */
#define  SPDIFRX_IFCR_SBDCF                ((uint32_t)0x00000010)       /*!<Clears the Synchronization Block Detected flag       */
#define  SPDIFRX_IFCR_SYNCDCF              ((uint32_t)0x00000020)       /*!<Clears the Synchronization Done flag                 */

/*******************  Bit definition for SPDIFRX_DR register  (DRFMT = 0b00 case) *******************/
#define  SPDIFRX_DR0_DR                    ((uint32_t)0x00FFFFFF)        /*!<Data value            */
#define  SPDIFRX_DR0_PE                    ((uint32_t)0x01000000)        /*!<Parity Error bit      */
#define  SPDIFRX_DR0_V                     ((uint32_t)0x02000000)        /*!<Validity bit          */
#define  SPDIFRX_DR0_U                     ((uint32_t)0x04000000)        /*!<User bit              */
#define  SPDIFRX_DR0_C                     ((uint32_t)0x08000000)        /*!<Channel Status bit    */
#define  SPDIFRX_DR0_PT                    ((uint32_t)0x30000000)        /*!<Preamble Type         */

/*******************  Bit definition for SPDIFRX_DR register  (DRFMT = 0b01 case) *******************/
#define  SPDIFRX_DR1_DR                    ((uint32_t)0xFFFFFF00)        /*!<Data value            */
#define  SPDIFRX_DR1_PT                    ((uint32_t)0x00000030)        /*!<Preamble Type         */
#define  SPDIFRX_DR1_C                     ((uint32_t)0x00000008)        /*!<Channel Status bit    */
#define  SPDIFRX_DR1_U                     ((uint32_t)0x00000004)        /*!<User bit              */
#define  SPDIFRX_DR1_V                     ((uint32_t)0x00000002)        /*!<Validity bit          */
#define  SPDIFRX_DR1_PE                    ((uint32_t)0x00000001)        /*!<Parity Error bit      */

/*******************  Bit definition for SPDIFRX_DR register  (DRFMT = 0b10 case) *******************/
#define  SPDIFRX_DR1_DRNL1                 ((uint32_t)0xFFFF0000)        /*!<Data value Channel B      */
#define  SPDIFRX_DR1_DRNL2                 ((uint32_t)0x0000FFFF)        /*!<Data value Channel A      */

/*******************  Bit definition for SPDIFRX_CSR register   *******************/
#define  SPDIFRX_CSR_USR                     ((uint32_t)0x0000FFFF)        /*!<User data information           */
#define  SPDIFRX_CSR_CS                      ((uint32_t)0x00FF0000)        /*!<Channel A status information    */
#define  SPDIFRX_CSR_SOB                     ((uint32_t)0x01000000)        /*!<Start Of Block                  */

/*******************  Bit definition for SPDIFRX_DIR register    *******************/
#define  SPDIFRX_DIR_THI                 ((uint32_t)0x000013FF)        /*!<Threshold LOW      */
#define  SPDIFRX_DIR_TLO                 ((uint32_t)0x1FFF0000)        /*!<Threshold HIGH     */


/******************************************************************************/
/*                                                                            */
/*                          SD host Interface                                 */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for SDMMC_POWER register  ******************/
#define  SDMMC_POWER_PWRCTRL                  ((uint32_t)0x03)               /*!<PWRCTRL[1:0] bits (Power supply control bits) */
#define  SDMMC_POWER_PWRCTRL_0                ((uint32_t)0x01)               /*!<Bit 0 */
#define  SDMMC_POWER_PWRCTRL_1                ((uint32_t)0x02)               /*!<Bit 1 */

/******************  Bit definition for SDMMC_CLKCR register  ******************/
#define  SDMMC_CLKCR_CLKDIV                   ((uint32_t)0x00FF)            /*!<Clock divide factor             */
#define  SDMMC_CLKCR_CLKEN                    ((uint32_t)0x0100)            /*!<Clock enable bit                */
#define  SDMMC_CLKCR_PWRSAV                   ((uint32_t)0x0200)            /*!<Power saving configuration bit  */
#define  SDMMC_CLKCR_BYPASS                   ((uint32_t)0x0400)            /*!<Clock divider bypass enable bit */

#define  SDMMC_CLKCR_WIDBUS                   ((uint32_t)0x1800)            /*!<WIDBUS[1:0] bits (Wide bus mode enable bit) */
#define  SDMMC_CLKCR_WIDBUS_0                 ((uint32_t)0x0800)            /*!<Bit 0 */
#define  SDMMC_CLKCR_WIDBUS_1                 ((uint32_t)0x1000)            /*!<Bit 1 */

#define  SDMMC_CLKCR_NEGEDGE                  ((uint32_t)0x2000)            /*!<SDMMC_CK dephasing selection bit */
#define  SDMMC_CLKCR_HWFC_EN                  ((uint32_t)0x4000)            /*!<HW Flow Control enable          */

/*******************  Bit definition for SDMMC_ARG register  *******************/
#define  SDMMC_ARG_CMDARG                     ((uint32_t)0xFFFFFFFF)            /*!<Command argument */

/*******************  Bit definition for SDMMC_CMD register  *******************/
#define  SDMMC_CMD_CMDINDEX                   ((uint32_t)0x003F)            /*!<Command Index                               */

#define  SDMMC_CMD_WAITRESP                   ((uint32_t)0x00C0)            /*!<WAITRESP[1:0] bits (Wait for response bits) */
#define  SDMMC_CMD_WAITRESP_0                 ((uint32_t)0x0040)            /*!< Bit 0 */
#define  SDMMC_CMD_WAITRESP_1                 ((uint32_t)0x0080)            /*!< Bit 1 */

#define  SDMMC_CMD_WAITINT                    ((uint32_t)0x0100)            /*!<CPSM Waits for Interrupt Request                               */
#define  SDMMC_CMD_WAITPEND                   ((uint32_t)0x0200)            /*!<CPSM Waits for ends of data transfer (CmdPend internal signal) */
#define  SDMMC_CMD_CPSMEN                     ((uint32_t)0x0400)            /*!<Command path state machine (CPSM) Enable bit                   */
#define  SDMMC_CMD_SDIOSUSPEND                ((uint32_t)0x0800)            /*!<SD I/O suspend command                                         */

/*****************  Bit definition for SDMMC_RESPCMD register  *****************/
#define  SDMMC_RESPCMD_RESPCMD                ((uint32_t)0x3F)               /*!<Response command index */

/******************  Bit definition for SDMMC_RESP0 register  ******************/
#define  SDMMC_RESP0_CARDSTATUS0              ((uint32_t)0xFFFFFFFF)        /*!<Card Status */

/******************  Bit definition for SDMMC_RESP1 register  ******************/
#define  SDMMC_RESP1_CARDSTATUS1              ((uint32_t)0xFFFFFFFF)        /*!<Card Status */

/******************  Bit definition for SDMMC_RESP2 register  ******************/
#define  SDMMC_RESP2_CARDSTATUS2              ((uint32_t)0xFFFFFFFF)        /*!<Card Status */

/******************  Bit definition for SDMMC_RESP3 register  ******************/
#define  SDMMC_RESP3_CARDSTATUS3              ((uint32_t)0xFFFFFFFF)        /*!<Card Status */

/******************  Bit definition for SDMMC_RESP4 register  ******************/
#define  SDMMC_RESP4_CARDSTATUS4              ((uint32_t)0xFFFFFFFF)        /*!<Card Status */

/******************  Bit definition for SDMMC_DTIMER register  *****************/
#define  SDMMC_DTIMER_DATATIME                ((uint32_t)0xFFFFFFFF)        /*!<Data timeout period. */

/******************  Bit definition for SDMMC_DLEN register  *******************/
#define  SDMMC_DLEN_DATALENGTH                ((uint32_t)0x01FFFFFF)        /*!<Data length value    */

/******************  Bit definition for SDMMC_DCTRL register  ******************/
#define  SDMMC_DCTRL_DTEN                     ((uint32_t)0x0001)            /*!<Data transfer enabled bit         */
#define  SDMMC_DCTRL_DTDIR                    ((uint32_t)0x0002)            /*!<Data transfer direction selection */
#define  SDMMC_DCTRL_DTMODE                   ((uint32_t)0x0004)            /*!<Data transfer mode selection      */
#define  SDMMC_DCTRL_DMAEN                    ((uint32_t)0x0008)            /*!<DMA enabled bit                   */

#define  SDMMC_DCTRL_DBLOCKSIZE               ((uint32_t)0x00F0)            /*!<DBLOCKSIZE[3:0] bits (Data block size) */
#define  SDMMC_DCTRL_DBLOCKSIZE_0             ((uint32_t)0x0010)            /*!<Bit 0 */
#define  SDMMC_DCTRL_DBLOCKSIZE_1             ((uint32_t)0x0020)            /*!<Bit 1 */
#define  SDMMC_DCTRL_DBLOCKSIZE_2             ((uint32_t)0x0040)            /*!<Bit 2 */
#define  SDMMC_DCTRL_DBLOCKSIZE_3             ((uint32_t)0x0080)            /*!<Bit 3 */

#define  SDMMC_DCTRL_RWSTART                  ((uint32_t)0x0100)            /*!<Read wait start         */
#define  SDMMC_DCTRL_RWSTOP                   ((uint32_t)0x0200)            /*!<Read wait stop          */
#define  SDMMC_DCTRL_RWMOD                    ((uint32_t)0x0400)            /*!<Read wait mode          */
#define  SDMMC_DCTRL_SDIOEN                   ((uint32_t)0x0800)            /*!<SD I/O enable functions */

/******************  Bit definition for SDMMC_DCOUNT register  *****************/
#define  SDMMC_DCOUNT_DATACOUNT               ((uint32_t)0x01FFFFFF)        /*!<Data count value */

/******************  Bit definition for SDMMC_STA register  ********************/
#define  SDMMC_STA_CCRCFAIL                   ((uint32_t)0x00000001)        /*!<Command response received (CRC check failed)  */
#define  SDMMC_STA_DCRCFAIL                   ((uint32_t)0x00000002)        /*!<Data block sent/received (CRC check failed)   */
#define  SDMMC_STA_CTIMEOUT                   ((uint32_t)0x00000004)        /*!<Command response timeout                      */
#define  SDMMC_STA_DTIMEOUT                   ((uint32_t)0x00000008)        /*!<Data timeout                                  */
#define  SDMMC_STA_TXUNDERR                   ((uint32_t)0x00000010)        /*!<Transmit FIFO underrun error                  */
#define  SDMMC_STA_RXOVERR                    ((uint32_t)0x00000020)        /*!<Received FIFO overrun error                   */
#define  SDMMC_STA_CMDREND                    ((uint32_t)0x00000040)        /*!<Command response received (CRC check passed)  */
#define  SDMMC_STA_CMDSENT                    ((uint32_t)0x00000080)        /*!<Command sent (no response required)           */
#define  SDMMC_STA_DATAEND                    ((uint32_t)0x00000100)        /*!<Data end (data counter, SDIDCOUNT, is zero)   */
#define  SDMMC_STA_DBCKEND                    ((uint32_t)0x00000400)        /*!<Data block sent/received (CRC check passed)   */
#define  SDMMC_STA_CMDACT                     ((uint32_t)0x00000800)        /*!<Command transfer in progress                  */
#define  SDMMC_STA_TXACT                      ((uint32_t)0x00001000)        /*!<Data transmit in progress                     */
#define  SDMMC_STA_RXACT                      ((uint32_t)0x00002000)        /*!<Data receive in progress                      */
#define  SDMMC_STA_TXFIFOHE                   ((uint32_t)0x00004000)        /*!<Transmit FIFO Half Empty: at least 8 words can be written into the FIFO */
#define  SDMMC_STA_RXFIFOHF                   ((uint32_t)0x00008000)        /*!<Receive FIFO Half Full: there are at least 8 words in the FIFO */
#define  SDMMC_STA_TXFIFOF                    ((uint32_t)0x00010000)        /*!<Transmit FIFO full                            */
#define  SDMMC_STA_RXFIFOF                    ((uint32_t)0x00020000)        /*!<Receive FIFO full                             */
#define  SDMMC_STA_TXFIFOE                    ((uint32_t)0x00040000)        /*!<Transmit FIFO empty                           */
#define  SDMMC_STA_RXFIFOE                    ((uint32_t)0x00080000)        /*!<Receive FIFO empty                            */
#define  SDMMC_STA_TXDAVL                     ((uint32_t)0x00100000)        /*!<Data available in transmit FIFO               */
#define  SDMMC_STA_RXDAVL                     ((uint32_t)0x00200000)        /*!<Data available in receive FIFO                */
#define  SDMMC_STA_SDIOIT                     ((uint32_t)0x00400000)        /*!<SDMMC interrupt received                       */

/*******************  Bit definition for SDMMC_ICR register  *******************/
#define  SDMMC_ICR_CCRCFAILC                  ((uint32_t)0x00000001)        /*!<CCRCFAIL flag clear bit */
#define  SDMMC_ICR_DCRCFAILC                  ((uint32_t)0x00000002)        /*!<DCRCFAIL flag clear bit */
#define  SDMMC_ICR_CTIMEOUTC                  ((uint32_t)0x00000004)        /*!<CTIMEOUT flag clear bit */
#define  SDMMC_ICR_DTIMEOUTC                  ((uint32_t)0x00000008)        /*!<DTIMEOUT flag clear bit */
#define  SDMMC_ICR_TXUNDERRC                  ((uint32_t)0x00000010)        /*!<TXUNDERR flag clear bit */
#define  SDMMC_ICR_RXOVERRC                   ((uint32_t)0x00000020)        /*!<RXOVERR flag clear bit  */
#define  SDMMC_ICR_CMDRENDC                   ((uint32_t)0x00000040)        /*!<CMDREND flag clear bit  */
#define  SDMMC_ICR_CMDSENTC                   ((uint32_t)0x00000080)        /*!<CMDSENT flag clear bit  */
#define  SDMMC_ICR_DATAENDC                   ((uint32_t)0x00000100)        /*!<DATAEND flag clear bit  */
#define  SDMMC_ICR_DBCKENDC                   ((uint32_t)0x00000400)        /*!<DBCKEND flag clear bit  */
#define  SDMMC_ICR_SDIOITC                    ((uint32_t)0x00400000)        /*!<SDMMCIT flag clear bit   */

/******************  Bit definition for SDMMC_MASK register  *******************/
#define  SDMMC_MASK_CCRCFAILIE                ((uint32_t)0x00000001)        /*!<Command CRC Fail Interrupt Enable          */
#define  SDMMC_MASK_DCRCFAILIE                ((uint32_t)0x00000002)        /*!<Data CRC Fail Interrupt Enable             */
#define  SDMMC_MASK_CTIMEOUTIE                ((uint32_t)0x00000004)        /*!<Command TimeOut Interrupt Enable           */
#define  SDMMC_MASK_DTIMEOUTIE                ((uint32_t)0x00000008)        /*!<Data TimeOut Interrupt Enable              */
#define  SDMMC_MASK_TXUNDERRIE                ((uint32_t)0x00000010)        /*!<Tx FIFO UnderRun Error Interrupt Enable    */
#define  SDMMC_MASK_RXOVERRIE                 ((uint32_t)0x00000020)        /*!<Rx FIFO OverRun Error Interrupt Enable     */
#define  SDMMC_MASK_CMDRENDIE                 ((uint32_t)0x00000040)        /*!<Command Response Received Interrupt Enable */
#define  SDMMC_MASK_CMDSENTIE                 ((uint32_t)0x00000080)        /*!<Command Sent Interrupt Enable              */
#define  SDMMC_MASK_DATAENDIE                 ((uint32_t)0x00000100)        /*!<Data End Interrupt Enable                  */
#define  SDMMC_MASK_DBCKENDIE                 ((uint32_t)0x00000400)        /*!<Data Block End Interrupt Enable            */
#define  SDMMC_MASK_CMDACTIE                  ((uint32_t)0x00000800)        /*!<CCommand Acting Interrupt Enable           */
#define  SDMMC_MASK_TXACTIE                   ((uint32_t)0x00001000)        /*!<Data Transmit Acting Interrupt Enable      */
#define  SDMMC_MASK_RXACTIE                   ((uint32_t)0x00002000)        /*!<Data receive acting interrupt enabled      */
#define  SDMMC_MASK_TXFIFOHEIE                ((uint32_t)0x00004000)        /*!<Tx FIFO Half Empty interrupt Enable        */
#define  SDMMC_MASK_RXFIFOHFIE                ((uint32_t)0x00008000)        /*!<Rx FIFO Half Full interrupt Enable         */
#define  SDMMC_MASK_TXFIFOFIE                 ((uint32_t)0x00010000)        /*!<Tx FIFO Full interrupt Enable              */
#define  SDMMC_MASK_RXFIFOFIE                 ((uint32_t)0x00020000)        /*!<Rx FIFO Full interrupt Enable              */
#define  SDMMC_MASK_TXFIFOEIE                 ((uint32_t)0x00040000)        /*!<Tx FIFO Empty interrupt Enable             */
#define  SDMMC_MASK_RXFIFOEIE                 ((uint32_t)0x00080000)        /*!<Rx FIFO Empty interrupt Enable             */
#define  SDMMC_MASK_TXDAVLIE                  ((uint32_t)0x00100000)        /*!<Data available in Tx FIFO interrupt Enable */
#define  SDMMC_MASK_RXDAVLIE                  ((uint32_t)0x00200000)        /*!<Data available in Rx FIFO interrupt Enable */
#define  SDMMC_MASK_SDIOITIE                  ((uint32_t)0x00400000)        /*!<SDMMC Mode Interrupt Received interrupt Enable */

/*****************  Bit definition for SDMMC_FIFOCNT register  *****************/
#define  SDMMC_FIFOCNT_FIFOCOUNT              ((uint32_t)0x00FFFFFF)        /*!<Remaining number of words to be written to or read from the FIFO */

/******************  Bit definition for SDMMC_FIFO register  *******************/
#define  SDMMC_FIFO_FIFODATA                  ((uint32_t)0xFFFFFFFF)        /*!<Receive and transmit FIFO data */

/******************************************************************************/
/*                                                                            */
/*                        Serial Peripheral Interface (SPI)                   */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for SPI_CR1 register  ********************/
#define  SPI_CR1_CPHA                        ((uint32_t)0x00000001)        /*!< Clock Phase                        */
#define  SPI_CR1_CPOL                        ((uint32_t)0x00000002)        /*!< Clock Polarity                     */
#define  SPI_CR1_MSTR                        ((uint32_t)0x00000004)        /*!< Master Selection                   */
#define  SPI_CR1_BR                          ((uint32_t)0x00000038)        /*!< BR[2:0] bits (Baud Rate Control)   */
#define  SPI_CR1_BR_0                        ((uint32_t)0x00000008)        /*!< Bit 0 */
#define  SPI_CR1_BR_1                        ((uint32_t)0x00000010)        /*!< Bit 1 */
#define  SPI_CR1_BR_2                        ((uint32_t)0x00000020)        /*!< Bit 2 */
#define  SPI_CR1_SPE                         ((uint32_t)0x00000040)        /*!< SPI Enable                          */
#define  SPI_CR1_LSBFIRST                    ((uint32_t)0x00000080)        /*!< Frame Format                        */
#define  SPI_CR1_SSI                         ((uint32_t)0x00000100)        /*!< Internal slave select               */
#define  SPI_CR1_SSM                         ((uint32_t)0x00000200)        /*!< Software slave management           */
#define  SPI_CR1_RXONLY                      ((uint32_t)0x00000400)        /*!< Receive only                        */
#define  SPI_CR1_CRCL                        ((uint32_t)0x00000800)        /*!< CRC Length                          */
#define  SPI_CR1_CRCNEXT                     ((uint32_t)0x00001000)        /*!< Transmit CRC next                   */
#define  SPI_CR1_CRCEN                       ((uint32_t)0x00002000)        /*!< Hardware CRC calculation enable     */
#define  SPI_CR1_BIDIOE                      ((uint32_t)0x00004000)        /*!< Output enable in bidirectional mode */
#define  SPI_CR1_BIDIMODE                    ((uint32_t)0x00008000)        /*!< Bidirectional data mode enable      */

/*******************  Bit definition for SPI_CR2 register  ********************/
#define  SPI_CR2_RXDMAEN                     ((uint32_t)0x00000001)        /*!< Rx Buffer DMA Enable                 */
#define  SPI_CR2_TXDMAEN                     ((uint32_t)0x00000002)        /*!< Tx Buffer DMA Enable                 */
#define  SPI_CR2_SSOE                        ((uint32_t)0x00000004)        /*!< SS Output Enable                     */
#define  SPI_CR2_NSSP                        ((uint32_t)0x00000008)        /*!< NSS pulse management Enable          */
#define  SPI_CR2_FRF                         ((uint32_t)0x00000010)        /*!< Frame Format Enable                  */
#define  SPI_CR2_ERRIE                       ((uint32_t)0x00000020)        /*!< Error Interrupt Enable               */
#define  SPI_CR2_RXNEIE                      ((uint32_t)0x00000040)        /*!< RX buffer Not Empty Interrupt Enable */
#define  SPI_CR2_TXEIE                       ((uint32_t)0x00000080)        /*!< Tx buffer Empty Interrupt Enable     */
#define  SPI_CR2_DS                          ((uint32_t)0x00000F00)        /*!< DS[3:0] Data Size                    */
#define  SPI_CR2_DS_0                        ((uint32_t)0x00000100)        /*!< Bit 0 */
#define  SPI_CR2_DS_1                        ((uint32_t)0x00000200)        /*!< Bit 1 */
#define  SPI_CR2_DS_2                        ((uint32_t)0x00000400)        /*!< Bit 2 */
#define  SPI_CR2_DS_3                        ((uint32_t)0x00000800)        /*!< Bit 3 */
#define  SPI_CR2_FRXTH                       ((uint32_t)0x00001000)        /*!< FIFO reception Threshold           */
#define  SPI_CR2_LDMARX                      ((uint32_t)0x00002000)        /*!< Last DMA transfer for reception    */
#define  SPI_CR2_LDMATX                      ((uint32_t)0x00004000)        /*!< Last DMA transfer for transmission */

/********************  Bit definition for SPI_SR register  ********************/
#define  SPI_SR_RXNE                         ((uint32_t)0x00000001)        /*!< Receive buffer Not Empty  */
#define  SPI_SR_TXE                          ((uint32_t)0x00000002)        /*!< Transmit buffer Empty     */
#define  SPI_SR_CHSIDE                       ((uint32_t)0x00000004)        /*!< Channel side              */
#define  SPI_SR_UDR                          ((uint32_t)0x00000008)        /*!< Underrun flag             */
#define  SPI_SR_CRCERR                       ((uint32_t)0x00000010)        /*!< CRC Error flag            */
#define  SPI_SR_MODF                         ((uint32_t)0x00000020)        /*!< Mode fault                */
#define  SPI_SR_OVR                          ((uint32_t)0x00000040)        /*!< Overrun flag              */
#define  SPI_SR_BSY                          ((uint32_t)0x00000080)        /*!< Busy flag                 */
#define  SPI_SR_FRE                          ((uint32_t)0x00000100)        /*!< TI frame format error     */
#define  SPI_SR_FRLVL                        ((uint32_t)0x00000600)        /*!< FIFO Reception Level      */
#define  SPI_SR_FRLVL_0                      ((uint32_t)0x00000200)        /*!< Bit 0 */
#define  SPI_SR_FRLVL_1                      ((uint32_t)0x00000400)        /*!< Bit 1 */
#define  SPI_SR_FTLVL                        ((uint32_t)0x00001800)        /*!< FIFO Transmission Level   */
#define  SPI_SR_FTLVL_0                      ((uint32_t)0x00000800)        /*!< Bit 0 */
#define  SPI_SR_FTLVL_1                      ((uint32_t)0x00001000)        /*!< Bit 1 */

/********************  Bit definition for SPI_DR register  ********************/
#define  SPI_DR_DR                           ((uint32_t)0xFFFF)            /*!< Data Register */

/*******************  Bit definition for SPI_CRCPR register  ******************/
#define  SPI_CRCPR_CRCPOLY                   ((uint32_t)0xFFFF)            /*!< CRC polynomial register */

/******************  Bit definition for SPI_RXCRCR register  ******************/
#define  SPI_RXCRCR_RXCRC                    ((uint32_t)0xFFFF)            /*!< Rx CRC Register */

/******************  Bit definition for SPI_TXCRCR register  ******************/
#define  SPI_TXCRCR_TXCRC                    ((uint32_t)0xFFFF)            /*!< Tx CRC Register */

/******************  Bit definition for SPI_I2SCFGR register  *****************/
#define  SPI_I2SCFGR_CHLEN                   ((uint32_t)0x00000001)        /*!<Channel length (number of bits per audio channel) */
#define  SPI_I2SCFGR_DATLEN                  ((uint32_t)0x00000006)        /*!<DATLEN[1:0] bits (Data length to be transferred)  */
#define  SPI_I2SCFGR_DATLEN_0                ((uint32_t)0x00000002)        /*!<Bit 0 */
#define  SPI_I2SCFGR_DATLEN_1                ((uint32_t)0x00000004)        /*!<Bit 1 */
#define  SPI_I2SCFGR_CKPOL                   ((uint32_t)0x00000008)        /*!<steady state clock polarity                       */
#define  SPI_I2SCFGR_I2SSTD                  ((uint32_t)0x00000030)        /*!<I2SSTD[1:0] bits (I2S standard selection)         */
#define  SPI_I2SCFGR_I2SSTD_0                ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  SPI_I2SCFGR_I2SSTD_1                ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  SPI_I2SCFGR_PCMSYNC                 ((uint32_t)0x00000080)        /*!<PCM frame synchronization                         */
#define  SPI_I2SCFGR_I2SCFG                  ((uint32_t)0x00000300)        /*!<I2SCFG[1:0] bits (I2S configuration mode)         */
#define  SPI_I2SCFGR_I2SCFG_0                ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  SPI_I2SCFGR_I2SCFG_1                ((uint32_t)0x00000200)        /*!<Bit 1 */
#define  SPI_I2SCFGR_I2SE                    ((uint32_t)0x00000400)        /*!<I2S Enable                                        */
#define  SPI_I2SCFGR_I2SMOD                  ((uint32_t)0x00000800)        /*!<I2S mode selection                                */
#define  SPI_I2SCFGR_ASTRTEN                 ((uint32_t)0x00001000)        /*!<Asynchronous start enable                        */

/******************  Bit definition for SPI_I2SPR register  *******************/
#define  SPI_I2SPR_I2SDIV                    ((uint32_t)0x00FF)            /*!<I2S Linear prescaler         */
#define  SPI_I2SPR_ODD                       ((uint32_t)0x0100)            /*!<Odd factor for the prescaler */
#define  SPI_I2SPR_MCKOE                     ((uint32_t)0x0200)            /*!<Master Clock Output Enable   */


/******************************************************************************/
/*                                                                            */
/*                                 SYSCFG                                     */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for SYSCFG_MEMRMP register  ***************/
#define SYSCFG_MEMRMP_MEM_BOOT          ((uint32_t)0x00000001) /*!< Boot information after Reset */

#define SYSCFG_MEMRMP_SWP_FMC          ((uint32_t)0x00000C00) /*!< FMC Memory Mapping swapping */
#define SYSCFG_MEMRMP_SWP_FMC_0        ((uint32_t)0x00000400)
#define SYSCFG_MEMRMP_SWP_FMC_1        ((uint32_t)0x00000800)

/******************  Bit definition for SYSCFG_PMC register  ******************/
#define SYSCFG_PMC_ADCxDC2              ((uint32_t)0x00070000) /*!< Refer to AN4073 on how to use this bit  */
#define SYSCFG_PMC_ADC1DC2              ((uint32_t)0x00010000) /*!< Refer to AN4073 on how to use this bit  */
#define SYSCFG_PMC_ADC2DC2              ((uint32_t)0x00020000) /*!< Refer to AN4073 on how to use this bit  */
#define SYSCFG_PMC_ADC3DC2              ((uint32_t)0x00040000) /*!< Refer to AN4073 on how to use this bit  */

#define SYSCFG_PMC_MII_RMII_SEL         ((uint32_t)0x00800000) /*!<Ethernet PHY interface selection */

/*****************  Bit definition for SYSCFG_EXTICR1 register  ***************/
#define SYSCFG_EXTICR1_EXTI0            ((uint32_t)0x000F) /*!<EXTI 0 configuration */
#define SYSCFG_EXTICR1_EXTI1            ((uint32_t)0x00F0) /*!<EXTI 1 configuration */
#define SYSCFG_EXTICR1_EXTI2            ((uint32_t)0x0F00) /*!<EXTI 2 configuration */
#define SYSCFG_EXTICR1_EXTI3            ((uint32_t)0xF000) /*!<EXTI 3 configuration */
/**
  * @brief   EXTI0 configuration
  */
#define SYSCFG_EXTICR1_EXTI0_PA         ((uint32_t)0x0000) /*!<PA[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PB         ((uint32_t)0x0001) /*!<PB[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PC         ((uint32_t)0x0002) /*!<PC[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PD         ((uint32_t)0x0003) /*!<PD[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PE         ((uint32_t)0x0004) /*!<PE[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PF         ((uint32_t)0x0005) /*!<PF[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PG         ((uint32_t)0x0006) /*!<PG[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PH         ((uint32_t)0x0007) /*!<PH[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PI         ((uint32_t)0x0008) /*!<PI[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PJ         ((uint32_t)0x0009) /*!<PJ[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PK         ((uint32_t)0x000A) /*!<PK[0] pin */

/**
  * @brief   EXTI1 configuration
  */
#define SYSCFG_EXTICR1_EXTI1_PA         ((uint32_t)0x0000) /*!<PA[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PB         ((uint32_t)0x0010) /*!<PB[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PC         ((uint32_t)0x0020) /*!<PC[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PD         ((uint32_t)0x0030) /*!<PD[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PE         ((uint32_t)0x0040) /*!<PE[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PF         ((uint32_t)0x0050) /*!<PF[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PG         ((uint32_t)0x0060) /*!<PG[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PH         ((uint32_t)0x0070) /*!<PH[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PI         ((uint32_t)0x0080) /*!<PI[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PJ         ((uint32_t)0x0090) /*!<PJ[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PK         ((uint32_t)0x00A0) /*!<PK[1] pin */

/**
  * @brief   EXTI2 configuration
  */
#define SYSCFG_EXTICR1_EXTI2_PA         ((uint32_t)0x0000) /*!<PA[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PB         ((uint32_t)0x0100) /*!<PB[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PC         ((uint32_t)0x0200) /*!<PC[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PD         ((uint32_t)0x0300) /*!<PD[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PE         ((uint32_t)0x0400) /*!<PE[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PF         ((uint32_t)0x0500) /*!<PF[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PG         ((uint32_t)0x0600) /*!<PG[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PH         ((uint32_t)0x0700) /*!<PH[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PI         ((uint32_t)0x0800) /*!<PI[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PJ         ((uint32_t)0x0900) /*!<PJ[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PK         ((uint32_t)0x0A00) /*!<PK[2] pin */

/**
  * @brief   EXTI3 configuration
  */
#define SYSCFG_EXTICR1_EXTI3_PA         ((uint32_t)0x0000) /*!<PA[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PB         ((uint32_t)0x1000) /*!<PB[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PC         ((uint32_t)0x2000) /*!<PC[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PD         ((uint32_t)0x3000) /*!<PD[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PE         ((uint32_t)0x4000) /*!<PE[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PF         ((uint32_t)0x5000) /*!<PF[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PG         ((uint32_t)0x6000) /*!<PG[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PH         ((uint32_t)0x7000) /*!<PH[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PI         ((uint32_t)0x8000) /*!<PI[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PJ         ((uint32_t)0x9000) /*!<PJ[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PK         ((uint32_t)0xA000) /*!<PK[3] pin */

/*****************  Bit definition for SYSCFG_EXTICR2 register  ***************/
#define SYSCFG_EXTICR2_EXTI4            ((uint32_t)0x000F) /*!<EXTI 4 configuration */
#define SYSCFG_EXTICR2_EXTI5            ((uint32_t)0x00F0) /*!<EXTI 5 configuration */
#define SYSCFG_EXTICR2_EXTI6            ((uint32_t)0x0F00) /*!<EXTI 6 configuration */
#define SYSCFG_EXTICR2_EXTI7            ((uint32_t)0xF000) /*!<EXTI 7 configuration */
/**
  * @brief   EXTI4 configuration
  */
#define SYSCFG_EXTICR2_EXTI4_PA         ((uint32_t)0x0000) /*!<PA[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PB         ((uint32_t)0x0001) /*!<PB[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PC         ((uint32_t)0x0002) /*!<PC[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PD         ((uint32_t)0x0003) /*!<PD[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PE         ((uint32_t)0x0004) /*!<PE[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PF         ((uint32_t)0x0005) /*!<PF[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PG         ((uint32_t)0x0006) /*!<PG[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PH         ((uint32_t)0x0007) /*!<PH[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PI         ((uint32_t)0x0008) /*!<PI[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PJ         ((uint32_t)0x0009) /*!<PJ[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PK         ((uint32_t)0x000A) /*!<PK[4] pin */

/**
  * @brief   EXTI5 configuration
  */
#define SYSCFG_EXTICR2_EXTI5_PA         ((uint32_t)0x0000) /*!<PA[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PB         ((uint32_t)0x0010) /*!<PB[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PC         ((uint32_t)0x0020) /*!<PC[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PD         ((uint32_t)0x0030) /*!<PD[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PE         ((uint32_t)0x0040) /*!<PE[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PF         ((uint32_t)0x0050) /*!<PF[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PG         ((uint32_t)0x0060) /*!<PG[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PH         ((uint32_t)0x0070) /*!<PH[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PI         ((uint32_t)0x0080) /*!<PI[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PJ         ((uint32_t)0x0090) /*!<PJ[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PK         ((uint32_t)0x00A0) /*!<PK[5] pin */

/**
  * @brief   EXTI6 configuration
  */
#define SYSCFG_EXTICR2_EXTI6_PA         ((uint32_t)0x0000) /*!<PA[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PB         ((uint32_t)0x0100) /*!<PB[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PC         ((uint32_t)0x0200) /*!<PC[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PD         ((uint32_t)0x0300) /*!<PD[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PE         ((uint32_t)0x0400) /*!<PE[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PF         ((uint32_t)0x0500) /*!<PF[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PG         ((uint32_t)0x0600) /*!<PG[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PH         ((uint32_t)0x0700) /*!<PH[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PI         ((uint32_t)0x0800) /*!<PI[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PJ         ((uint32_t)0x0900) /*!<PJ[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PK         ((uint32_t)0x0A00) /*!<PK[6] pin */

/**
  * @brief   EXTI7 configuration
  */
#define SYSCFG_EXTICR2_EXTI7_PA         ((uint32_t)0x0000) /*!<PA[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PB         ((uint32_t)0x1000) /*!<PB[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PC         ((uint32_t)0x2000) /*!<PC[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PD         ((uint32_t)0x3000) /*!<PD[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PE         ((uint32_t)0x4000) /*!<PE[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PF         ((uint32_t)0x5000) /*!<PF[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PG         ((uint32_t)0x6000) /*!<PG[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PH         ((uint32_t)0x7000) /*!<PH[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PI         ((uint32_t)0x8000) /*!<PI[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PJ         ((uint32_t)0x9000) /*!<PJ[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PK         ((uint32_t)0xA000) /*!<PK[7] pin */

/*****************  Bit definition for SYSCFG_EXTICR3 register  ***************/
#define SYSCFG_EXTICR3_EXTI8            ((uint32_t)0x000F) /*!<EXTI 8 configuration */
#define SYSCFG_EXTICR3_EXTI9            ((uint32_t)0x00F0) /*!<EXTI 9 configuration */
#define SYSCFG_EXTICR3_EXTI10           ((uint32_t)0x0F00) /*!<EXTI 10 configuration */
#define SYSCFG_EXTICR3_EXTI11           ((uint32_t)0xF000) /*!<EXTI 11 configuration */

/**
  * @brief   EXTI8 configuration
  */
#define SYSCFG_EXTICR3_EXTI8_PA         ((uint32_t)0x0000) /*!<PA[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PB         ((uint32_t)0x0001) /*!<PB[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PC         ((uint32_t)0x0002) /*!<PC[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PD         ((uint32_t)0x0003) /*!<PD[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PE         ((uint32_t)0x0004) /*!<PE[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PF         ((uint32_t)0x0005) /*!<PF[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PG         ((uint32_t)0x0006) /*!<PG[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PH         ((uint32_t)0x0007) /*!<PH[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PI         ((uint32_t)0x0008) /*!<PI[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PJ         ((uint32_t)0x0009) /*!<PJ[8] pin */

/**
  * @brief   EXTI9 configuration
  */
#define SYSCFG_EXTICR3_EXTI9_PA         ((uint32_t)0x0000) /*!<PA[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PB         ((uint32_t)0x0010) /*!<PB[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PC         ((uint32_t)0x0020) /*!<PC[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PD         ((uint32_t)0x0030) /*!<PD[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PE         ((uint32_t)0x0040) /*!<PE[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PF         ((uint32_t)0x0050) /*!<PF[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PG         ((uint32_t)0x0060) /*!<PG[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PH         ((uint32_t)0x0070) /*!<PH[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PI         ((uint32_t)0x0080) /*!<PI[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PJ         ((uint32_t)0x0090) /*!<PJ[9] pin */

/**
  * @brief   EXTI10 configuration
  */
#define SYSCFG_EXTICR3_EXTI10_PA        ((uint32_t)0x0000) /*!<PA[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PB        ((uint32_t)0x0100) /*!<PB[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PC        ((uint32_t)0x0200) /*!<PC[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PD        ((uint32_t)0x0300) /*!<PD[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PE        ((uint32_t)0x0400) /*!<PE[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PF        ((uint32_t)0x0500) /*!<PF[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PG        ((uint32_t)0x0600) /*!<PG[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PH        ((uint32_t)0x0700) /*!<PH[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PI        ((uint32_t)0x0800) /*!<PI[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PJ        ((uint32_t)0x0900) /*!<PJ[10] pin */

/**
  * @brief   EXTI11 configuration
  */
#define SYSCFG_EXTICR3_EXTI11_PA        ((uint32_t)0x0000) /*!<PA[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PB        ((uint32_t)0x1000) /*!<PB[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PC        ((uint32_t)0x2000) /*!<PC[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PD        ((uint32_t)0x3000) /*!<PD[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PE        ((uint32_t)0x4000) /*!<PE[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PF        ((uint32_t)0x5000) /*!<PF[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PG        ((uint32_t)0x6000) /*!<PG[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PH        ((uint32_t)0x7000) /*!<PH[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PI        ((uint32_t)0x8000) /*!<PI[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PJ        ((uint32_t)0x9000) /*!<PJ[11] pin */


/*****************  Bit definition for SYSCFG_EXTICR4 register  ***************/
#define SYSCFG_EXTICR4_EXTI12           ((uint32_t)0x000F) /*!<EXTI 12 configuration */
#define SYSCFG_EXTICR4_EXTI13           ((uint32_t)0x00F0) /*!<EXTI 13 configuration */
#define SYSCFG_EXTICR4_EXTI14           ((uint32_t)0x0F00) /*!<EXTI 14 configuration */
#define SYSCFG_EXTICR4_EXTI15           ((uint32_t)0xF000) /*!<EXTI 15 configuration */
/**
  * @brief   EXTI12 configuration
  */
#define SYSCFG_EXTICR4_EXTI12_PA        ((uint32_t)0x0000) /*!<PA[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PB        ((uint32_t)0x0001) /*!<PB[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PC        ((uint32_t)0x0002) /*!<PC[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PD        ((uint32_t)0x0003) /*!<PD[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PE        ((uint32_t)0x0004) /*!<PE[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PF        ((uint32_t)0x0005) /*!<PF[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PG        ((uint32_t)0x0006) /*!<PG[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PH        ((uint32_t)0x0007) /*!<PH[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PI        ((uint32_t)0x0008) /*!<PI[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PJ        ((uint32_t)0x0009) /*!<PJ[12] pin */

/**
  * @brief   EXTI13 configuration
  */
#define SYSCFG_EXTICR4_EXTI13_PA        ((uint32_t)0x0000) /*!<PA[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PB        ((uint32_t)0x0010) /*!<PB[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PC        ((uint32_t)0x0020) /*!<PC[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PD        ((uint32_t)0x0030) /*!<PD[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PE        ((uint32_t)0x0040) /*!<PE[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PF        ((uint32_t)0x0050) /*!<PF[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PG        ((uint32_t)0x0060) /*!<PG[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PH        ((uint32_t)0x0070) /*!<PH[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PI        ((uint32_t)0x0008) /*!<PI[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PJ        ((uint32_t)0x0009) /*!<PJ[13] pin */

/**
  * @brief   EXTI14 configuration
  */
#define SYSCFG_EXTICR4_EXTI14_PA        ((uint32_t)0x0000) /*!<PA[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PB        ((uint32_t)0x0100) /*!<PB[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PC        ((uint32_t)0x0200) /*!<PC[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PD        ((uint32_t)0x0300) /*!<PD[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PE        ((uint32_t)0x0400) /*!<PE[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PF        ((uint32_t)0x0500) /*!<PF[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PG        ((uint32_t)0x0600) /*!<PG[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PH        ((uint32_t)0x0700) /*!<PH[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PI        ((uint32_t)0x0800) /*!<PI[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PJ        ((uint32_t)0x0900) /*!<PJ[14] pin */

/**
  * @brief   EXTI15 configuration
  */
#define SYSCFG_EXTICR4_EXTI15_PA        ((uint32_t)0x0000) /*!<PA[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PB        ((uint32_t)0x1000) /*!<PB[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PC        ((uint32_t)0x2000) /*!<PC[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PD        ((uint32_t)0x3000) /*!<PD[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PE        ((uint32_t)0x4000) /*!<PE[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PF        ((uint32_t)0x5000) /*!<PF[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PG        ((uint32_t)0x6000) /*!<PG[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PH        ((uint32_t)0x7000) /*!<PH[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PI        ((uint32_t)0x8000) /*!<PI[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PJ        ((uint32_t)0x9000) /*!<PJ[15] pin */

/******************  Bit definition for SYSCFG_CMPCR register  ****************/
#define SYSCFG_CMPCR_CMP_PD             ((uint32_t)0x00000001) /*!<Compensation cell power-down */
#define SYSCFG_CMPCR_READY              ((uint32_t)0x00000100) /*!<Compensation cell ready flag*/

/******************************************************************************/
/*                                                                            */
/*                                    TIM                                     */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for TIM_CR1 register  ********************/
#define  TIM_CR1_CEN                         ((uint32_t)0x0001)            /*!<Counter enable        */
#define  TIM_CR1_UDIS                        ((uint32_t)0x0002)            /*!<Update disable        */
#define  TIM_CR1_URS                         ((uint32_t)0x0004)            /*!<Update request source */
#define  TIM_CR1_OPM                         ((uint32_t)0x0008)            /*!<One pulse mode        */
#define  TIM_CR1_DIR                         ((uint32_t)0x0010)            /*!<Direction             */

#define  TIM_CR1_CMS                         ((uint32_t)0x0060)            /*!<CMS[1:0] bits (Center-aligned mode selection) */
#define  TIM_CR1_CMS_0                       ((uint32_t)0x0020)            /*!<Bit 0 */
#define  TIM_CR1_CMS_1                       ((uint32_t)0x0040)            /*!<Bit 1 */

#define  TIM_CR1_ARPE                        ((uint32_t)0x0080)            /*!<Auto-reload preload enable     */

#define  TIM_CR1_CKD                         ((uint32_t)0x0300)            /*!<CKD[1:0] bits (clock division) */
#define  TIM_CR1_CKD_0                       ((uint32_t)0x0100)            /*!<Bit 0 */
#define  TIM_CR1_CKD_1                       ((uint32_t)0x0200)            /*!<Bit 1 */
#define  TIM_CR1_UIFREMAP                    ((uint32_t)0x0800)            /*!<UIF status bit */

/*******************  Bit definition for TIM_CR2 register  ********************/
#define  TIM_CR2_CCPC                        ((uint32_t)0x00000001)            /*!<Capture/Compare Preloaded Control        */
#define  TIM_CR2_CCUS                        ((uint32_t)0x00000004)            /*!<Capture/Compare Control Update Selection */
#define  TIM_CR2_CCDS                        ((uint32_t)0x00000008)            /*!<Capture/Compare DMA Selection            */

#define  TIM_CR2_OIS5                        ((uint32_t)0x00010000)            /*!<Output Idle state 4 (OC4 output) */
#define  TIM_CR2_OIS6                        ((uint32_t)0x00040000)            /*!<Output Idle state 4 (OC4 output) */

#define  TIM_CR2_MMS                         ((uint32_t)0x0070)            /*!<MMS[2:0] bits (Master Mode Selection) */
#define  TIM_CR2_MMS_0                       ((uint32_t)0x0010)            /*!<Bit 0 */
#define  TIM_CR2_MMS_1                       ((uint32_t)0x0020)            /*!<Bit 1 */
#define  TIM_CR2_MMS_2                       ((uint32_t)0x0040)            /*!<Bit 2 */

#define  TIM_CR2_MMS2                        ((uint32_t)0x00F00000)            /*!<MMS[2:0] bits (Master Mode Selection) */
#define  TIM_CR2_MMS2_0                      ((uint32_t)0x00100000)            /*!<Bit 0 */
#define  TIM_CR2_MMS2_1                      ((uint32_t)0x00200000)            /*!<Bit 1 */
#define  TIM_CR2_MMS2_2                      ((uint32_t)0x00400000)            /*!<Bit 2 */
#define  TIM_CR2_MMS2_3                      ((uint32_t)0x00800000)            /*!<Bit 2 */

#define  TIM_CR2_TI1S                        ((uint32_t)0x0080)            /*!<TI1 Selection */
#define  TIM_CR2_OIS1                        ((uint32_t)0x0100)            /*!<Output Idle state 1 (OC1 output)  */
#define  TIM_CR2_OIS1N                       ((uint32_t)0x0200)            /*!<Output Idle state 1 (OC1N output) */
#define  TIM_CR2_OIS2                        ((uint32_t)0x0400)            /*!<Output Idle state 2 (OC2 output)  */
#define  TIM_CR2_OIS2N                       ((uint32_t)0x0800)            /*!<Output Idle state 2 (OC2N output) */
#define  TIM_CR2_OIS3                        ((uint32_t)0x1000)            /*!<Output Idle state 3 (OC3 output)  */
#define  TIM_CR2_OIS3N                       ((uint32_t)0x2000)            /*!<Output Idle state 3 (OC3N output) */
#define  TIM_CR2_OIS4                        ((uint32_t)0x4000)            /*!<Output Idle state 4 (OC4 output)  */

/*******************  Bit definition for TIM_SMCR register  *******************/
#define  TIM_SMCR_SMS                        ((uint32_t)0x00010007)            /*!<SMS[2:0] bits (Slave mode selection)    */
#define  TIM_SMCR_SMS_0                      ((uint32_t)0x00000001)            /*!<Bit 0 */
#define  TIM_SMCR_SMS_1                      ((uint32_t)0x00000002)            /*!<Bit 1 */
#define  TIM_SMCR_SMS_2                      ((uint32_t)0x00000004)            /*!<Bit 2 */
#define  TIM_SMCR_SMS_3                      ((uint32_t)0x00010000)            /*!<Bit 3 */
#define  TIM_SMCR_OCCS                       ((uint32_t)0x00000008)            /*!< OCREF clear selection */

#define  TIM_SMCR_TS                         ((uint32_t)0x0070)            /*!<TS[2:0] bits (Trigger selection)        */
#define  TIM_SMCR_TS_0                       ((uint32_t)0x0010)            /*!<Bit 0 */
#define  TIM_SMCR_TS_1                       ((uint32_t)0x0020)            /*!<Bit 1 */
#define  TIM_SMCR_TS_2                       ((uint32_t)0x0040)            /*!<Bit 2 */

#define  TIM_SMCR_MSM                        ((uint32_t)0x0080)            /*!<Master/slave mode                       */

#define  TIM_SMCR_ETF                        ((uint32_t)0x0F00)            /*!<ETF[3:0] bits (External trigger filter) */
#define  TIM_SMCR_ETF_0                      ((uint32_t)0x0100)            /*!<Bit 0 */
#define  TIM_SMCR_ETF_1                      ((uint32_t)0x0200)            /*!<Bit 1 */
#define  TIM_SMCR_ETF_2                      ((uint32_t)0x0400)            /*!<Bit 2 */
#define  TIM_SMCR_ETF_3                      ((uint32_t)0x0800)            /*!<Bit 3 */

#define  TIM_SMCR_ETPS                       ((uint32_t)0x3000)            /*!<ETPS[1:0] bits (External trigger prescaler) */
#define  TIM_SMCR_ETPS_0                     ((uint32_t)0x1000)            /*!<Bit 0 */
#define  TIM_SMCR_ETPS_1                     ((uint32_t)0x2000)            /*!<Bit 1 */


#define  TIM_SMCR_ECE                        ((uint32_t)0x4000)            /*!<External clock enable     */
#define  TIM_SMCR_ETP                        ((uint32_t)0x8000)            /*!<External trigger polarity */

/*******************  Bit definition for TIM_DIER register  *******************/
#define  TIM_DIER_UIE                        ((uint32_t)0x0001)            /*!<Update interrupt enable */
#define  TIM_DIER_CC1IE                      ((uint32_t)0x0002)            /*!<Capture/Compare 1 interrupt enable   */
#define  TIM_DIER_CC2IE                      ((uint32_t)0x0004)            /*!<Capture/Compare 2 interrupt enable   */
#define  TIM_DIER_CC3IE                      ((uint32_t)0x0008)            /*!<Capture/Compare 3 interrupt enable   */
#define  TIM_DIER_CC4IE                      ((uint32_t)0x0010)            /*!<Capture/Compare 4 interrupt enable   */
#define  TIM_DIER_COMIE                      ((uint32_t)0x0020)            /*!<COM interrupt enable                 */
#define  TIM_DIER_TIE                        ((uint32_t)0x0040)            /*!<Trigger interrupt enable             */
#define  TIM_DIER_BIE                        ((uint32_t)0x0080)            /*!<Break interrupt enable               */
#define  TIM_DIER_UDE                        ((uint32_t)0x0100)            /*!<Update DMA request enable            */
#define  TIM_DIER_CC1DE                      ((uint32_t)0x0200)            /*!<Capture/Compare 1 DMA request enable */
#define  TIM_DIER_CC2DE                      ((uint32_t)0x0400)            /*!<Capture/Compare 2 DMA request enable */
#define  TIM_DIER_CC3DE                      ((uint32_t)0x0800)            /*!<Capture/Compare 3 DMA request enable */
#define  TIM_DIER_CC4DE                      ((uint32_t)0x1000)            /*!<Capture/Compare 4 DMA request enable */
#define  TIM_DIER_COMDE                      ((uint32_t)0x2000)            /*!<COM DMA request enable               */
#define  TIM_DIER_TDE                        ((uint32_t)0x4000)            /*!<Trigger DMA request enable           */

/********************  Bit definition for TIM_SR register  ********************/
#define  TIM_SR_UIF                          ((uint32_t)0x0001)            /*!<Update interrupt Flag              */
#define  TIM_SR_CC1IF                        ((uint32_t)0x0002)            /*!<Capture/Compare 1 interrupt Flag   */
#define  TIM_SR_CC2IF                        ((uint32_t)0x0004)            /*!<Capture/Compare 2 interrupt Flag   */
#define  TIM_SR_CC3IF                        ((uint32_t)0x0008)            /*!<Capture/Compare 3 interrupt Flag   */
#define  TIM_SR_CC4IF                        ((uint32_t)0x0010)            /*!<Capture/Compare 4 interrupt Flag   */
#define  TIM_SR_COMIF                        ((uint32_t)0x0020)            /*!<COM interrupt Flag                 */
#define  TIM_SR_TIF                          ((uint32_t)0x0040)            /*!<Trigger interrupt Flag             */
#define  TIM_SR_BIF                          ((uint32_t)0x0080)            /*!<Break interrupt Flag               */
#define  TIM_SR_B2IF                         ((uint32_t)0x0100)            /*!<Break2 interrupt Flag               */
#define  TIM_SR_CC1OF                        ((uint32_t)0x0200)            /*!<Capture/Compare 1 Overcapture Flag */
#define  TIM_SR_CC2OF                        ((uint32_t)0x0400)            /*!<Capture/Compare 2 Overcapture Flag */
#define  TIM_SR_CC3OF                        ((uint32_t)0x0800)            /*!<Capture/Compare 3 Overcapture Flag */
#define  TIM_SR_CC4OF                        ((uint32_t)0x1000)            /*!<Capture/Compare 4 Overcapture Flag */

/*******************  Bit definition for TIM_EGR register  ********************/
#define  TIM_EGR_UG                          ((uint32_t)0x00000001)               /*!<Update Generation                         */
#define  TIM_EGR_CC1G                        ((uint32_t)0x00000002)               /*!<Capture/Compare 1 Generation              */
#define  TIM_EGR_CC2G                        ((uint32_t)0x00000004)               /*!<Capture/Compare 2 Generation              */
#define  TIM_EGR_CC3G                        ((uint32_t)0x00000008)               /*!<Capture/Compare 3 Generation              */
#define  TIM_EGR_CC4G                        ((uint32_t)0x00000010)               /*!<Capture/Compare 4 Generation              */
#define  TIM_EGR_COMG                        ((uint32_t)0x00000020)               /*!<Capture/Compare Control Update Generation */
#define  TIM_EGR_TG                          ((uint32_t)0x00000040)               /*!<Trigger Generation                        */
#define  TIM_EGR_BG                          ((uint32_t)0x00000080)               /*!<Break Generation                          */
#define  TIM_EGR_B2G                         ((uint32_t)0x00000100)              /*!<Break2 Generation                          */

/******************  Bit definition for TIM_CCMR1 register  *******************/
#define  TIM_CCMR1_CC1S                      ((uint32_t)0x00000003)            /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
#define  TIM_CCMR1_CC1S_0                    ((uint32_t)0x00000001)            /*!<Bit 0 */
#define  TIM_CCMR1_CC1S_1                    ((uint32_t)0x00000002)            /*!<Bit 1 */

#define  TIM_CCMR1_OC1FE                     ((uint32_t)0x00000004)            /*!<Output Compare 1 Fast enable                 */
#define  TIM_CCMR1_OC1PE                     ((uint32_t)0x00000008)            /*!<Output Compare 1 Preload enable              */

#define  TIM_CCMR1_OC1M                      ((uint32_t)0x00010070)            /*!<OC1M[2:0] bits (Output Compare 1 Mode)       */
#define  TIM_CCMR1_OC1M_0                    ((uint32_t)0x00000010)            /*!<Bit 0 */
#define  TIM_CCMR1_OC1M_1                    ((uint32_t)0x00000020)            /*!<Bit 1 */
#define  TIM_CCMR1_OC1M_2                    ((uint32_t)0x00000040)            /*!<Bit 2 */
#define  TIM_CCMR1_OC1M_3                    ((uint32_t)0x00010000)            /*!<Bit 3 */

#define  TIM_CCMR1_OC1CE                     ((uint32_t)0x00000080)            /*!<Output Compare 1Clear Enable                 */

#define  TIM_CCMR1_CC2S                      ((uint32_t)0x00000300)            /*!<CC2S[1:0] bits (Capture/Compare 2 Selection) */
#define  TIM_CCMR1_CC2S_0                    ((uint32_t)0x00000100)            /*!<Bit 0 */
#define  TIM_CCMR1_CC2S_1                    ((uint32_t)0x00000200)            /*!<Bit 1 */

#define  TIM_CCMR1_OC2FE                     ((uint32_t)0x00000400)            /*!<Output Compare 2 Fast enable                 */
#define  TIM_CCMR1_OC2PE                     ((uint32_t)0x00000800)            /*!<Output Compare 2 Preload enable              */

#define  TIM_CCMR1_OC2M                      ((uint32_t)0x01007000)            /*!<OC2M[2:0] bits (Output Compare 2 Mode)       */
#define  TIM_CCMR1_OC2M_0                    ((uint32_t)0x00001000)            /*!<Bit 0 */
#define  TIM_CCMR1_OC2M_1                    ((uint32_t)0x00002000)            /*!<Bit 1 */
#define  TIM_CCMR1_OC2M_2                    ((uint32_t)0x00004000)            /*!<Bit 2 */
#define  TIM_CCMR1_OC2M_3                    ((uint32_t)0x01000000)            /*!<Bit 3 */

#define  TIM_CCMR1_OC2CE                     ((uint32_t)0x00008000)            /*!<Output Compare 2 Clear Enable */

/*----------------------------------------------------------------------------*/

#define  TIM_CCMR1_IC1PSC                    ((uint32_t)0x000C)            /*!<IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
#define  TIM_CCMR1_IC1PSC_0                  ((uint32_t)0x0004)            /*!<Bit 0 */
#define  TIM_CCMR1_IC1PSC_1                  ((uint32_t)0x0008)            /*!<Bit 1 */

#define  TIM_CCMR1_IC1F                      ((uint32_t)0x00F0)            /*!<IC1F[3:0] bits (Input Capture 1 Filter)      */
#define  TIM_CCMR1_IC1F_0                    ((uint32_t)0x0010)            /*!<Bit 0 */
#define  TIM_CCMR1_IC1F_1                    ((uint32_t)0x0020)            /*!<Bit 1 */
#define  TIM_CCMR1_IC1F_2                    ((uint32_t)0x0040)            /*!<Bit 2 */
#define  TIM_CCMR1_IC1F_3                    ((uint32_t)0x0080)            /*!<Bit 3 */

#define  TIM_CCMR1_IC2PSC                    ((uint32_t)0x0C00)            /*!<IC2PSC[1:0] bits (Input Capture 2 Prescaler)  */
#define  TIM_CCMR1_IC2PSC_0                  ((uint32_t)0x0400)            /*!<Bit 0 */
#define  TIM_CCMR1_IC2PSC_1                  ((uint32_t)0x0800)            /*!<Bit 1 */

#define  TIM_CCMR1_IC2F                      ((uint32_t)0xF000)            /*!<IC2F[3:0] bits (Input Capture 2 Filter)       */
#define  TIM_CCMR1_IC2F_0                    ((uint32_t)0x1000)            /*!<Bit 0 */
#define  TIM_CCMR1_IC2F_1                    ((uint32_t)0x2000)            /*!<Bit 1 */
#define  TIM_CCMR1_IC2F_2                    ((uint32_t)0x4000)            /*!<Bit 2 */
#define  TIM_CCMR1_IC2F_3                    ((uint32_t)0x8000)            /*!<Bit 3 */

/******************  Bit definition for TIM_CCMR2 register  *******************/
#define  TIM_CCMR2_CC3S                      ((uint32_t)0x00000003)        /*!<CC3S[1:0] bits (Capture/Compare 3 Selection)  */
#define  TIM_CCMR2_CC3S_0                    ((uint32_t)0x00000001)        /*!<Bit 0 */
#define  TIM_CCMR2_CC3S_1                    ((uint32_t)0x00000002)        /*!<Bit 1 */

#define  TIM_CCMR2_OC3FE                     ((uint32_t)0x00000004)        /*!<Output Compare 3 Fast enable           */
#define  TIM_CCMR2_OC3PE                     ((uint32_t)0x00000008)        /*!<Output Compare 3 Preload enable        */

#define  TIM_CCMR2_OC3M                      ((uint32_t)0x00010070)        /*!<OC3M[2:0] bits (Output Compare 3 Mode) */
#define  TIM_CCMR2_OC3M_0                    ((uint32_t)0x00000010)        /*!<Bit 0 */
#define  TIM_CCMR2_OC3M_1                    ((uint32_t)0x00000020)        /*!<Bit 1 */
#define  TIM_CCMR2_OC3M_2                    ((uint32_t)0x00000040)        /*!<Bit 2 */
#define  TIM_CCMR2_OC3M_3                    ((uint32_t)0x00010000)        /*!<Bit 3 */



#define  TIM_CCMR2_OC3CE                     ((uint32_t)0x00000080)        /*!<Output Compare 3 Clear Enable */

#define  TIM_CCMR2_CC4S                      ((uint32_t)0x00000300)        /*!<CC4S[1:0] bits (Capture/Compare 4 Selection) */
#define  TIM_CCMR2_CC4S_0                    ((uint32_t)0x00000100)        /*!<Bit 0 */
#define  TIM_CCMR2_CC4S_1                    ((uint32_t)0x00000200)        /*!<Bit 1 */

#define  TIM_CCMR2_OC4FE                     ((uint32_t)0x00000400)        /*!<Output Compare 4 Fast enable    */
#define  TIM_CCMR2_OC4PE                     ((uint32_t)0x00000800)        /*!<Output Compare 4 Preload enable */

#define  TIM_CCMR2_OC4M                      ((uint32_t)0x01007000)        /*!<OC4M[2:0] bits (Output Compare 4 Mode) */
#define  TIM_CCMR2_OC4M_0                    ((uint32_t)0x00001000)        /*!<Bit 0 */
#define  TIM_CCMR2_OC4M_1                    ((uint32_t)0x00002000)        /*!<Bit 1 */
#define  TIM_CCMR2_OC4M_2                    ((uint32_t)0x00004000)        /*!<Bit 2 */
#define  TIM_CCMR2_OC4M_3                    ((uint32_t)0x01000000)        /*!<Bit 3 */

#define  TIM_CCMR2_OC4CE                     ((uint32_t)0x8000)            /*!<Output Compare 4 Clear Enable */

/*----------------------------------------------------------------------------*/

#define  TIM_CCMR2_IC3PSC                    ((uint32_t)0x000C)            /*!<IC3PSC[1:0] bits (Input Capture 3 Prescaler) */
#define  TIM_CCMR2_IC3PSC_0                  ((uint32_t)0x0004)            /*!<Bit 0 */
#define  TIM_CCMR2_IC3PSC_1                  ((uint32_t)0x0008)            /*!<Bit 1 */

#define  TIM_CCMR2_IC3F                      ((uint32_t)0x00F0)            /*!<IC3F[3:0] bits (Input Capture 3 Filter) */
#define  TIM_CCMR2_IC3F_0                    ((uint32_t)0x0010)            /*!<Bit 0 */
#define  TIM_CCMR2_IC3F_1                    ((uint32_t)0x0020)            /*!<Bit 1 */
#define  TIM_CCMR2_IC3F_2                    ((uint32_t)0x0040)            /*!<Bit 2 */
#define  TIM_CCMR2_IC3F_3                    ((uint32_t)0x0080)            /*!<Bit 3 */

#define  TIM_CCMR2_IC4PSC                    ((uint32_t)0x0C00)            /*!<IC4PSC[1:0] bits (Input Capture 4 Prescaler) */
#define  TIM_CCMR2_IC4PSC_0                  ((uint32_t)0x0400)            /*!<Bit 0 */
#define  TIM_CCMR2_IC4PSC_1                  ((uint32_t)0x0800)            /*!<Bit 1 */

#define  TIM_CCMR2_IC4F                      ((uint32_t)0xF000)            /*!<IC4F[3:0] bits (Input Capture 4 Filter) */
#define  TIM_CCMR2_IC4F_0                    ((uint32_t)0x1000)            /*!<Bit 0 */
#define  TIM_CCMR2_IC4F_1                    ((uint32_t)0x2000)            /*!<Bit 1 */
#define  TIM_CCMR2_IC4F_2                    ((uint32_t)0x4000)            /*!<Bit 2 */
#define  TIM_CCMR2_IC4F_3                    ((uint32_t)0x8000)            /*!<Bit 3 */

/*******************  Bit definition for TIM_CCER register  *******************/
#define  TIM_CCER_CC1E                       ((uint32_t)0x00000001)            /*!<Capture/Compare 1 output enable */
#define  TIM_CCER_CC1P                       ((uint32_t)0x00000002)            /*!<Capture/Compare 1 output Polarity */
#define  TIM_CCER_CC1NE                      ((uint32_t)0x00000004)            /*!<Capture/Compare 1 Complementary output enable */
#define  TIM_CCER_CC1NP                      ((uint32_t)0x00000008)            /*!<Capture/Compare 1 Complementary output Polarity */
#define  TIM_CCER_CC2E                       ((uint32_t)0x00000010)            /*!<Capture/Compare 2 output enable */
#define  TIM_CCER_CC2P                       ((uint32_t)0x00000020)            /*!<Capture/Compare 2 output Polarity */
#define  TIM_CCER_CC2NE                      ((uint32_t)0x00000040)            /*!<Capture/Compare 2 Complementary output enable */
#define  TIM_CCER_CC2NP                      ((uint32_t)0x00000080)            /*!<Capture/Compare 2 Complementary output Polarity */
#define  TIM_CCER_CC3E                       ((uint32_t)0x00000100)            /*!<Capture/Compare 3 output enable */
#define  TIM_CCER_CC3P                       ((uint32_t)0x00000200)            /*!<Capture/Compare 3 output Polarity */
#define  TIM_CCER_CC3NE                      ((uint32_t)0x00000400)            /*!<Capture/Compare 3 Complementary output enable */
#define  TIM_CCER_CC3NP                      ((uint32_t)0x00000800)            /*!<Capture/Compare 3 Complementary output Polarity */
#define  TIM_CCER_CC4E                       ((uint32_t)0x00001000)            /*!<Capture/Compare 4 output enable */
#define  TIM_CCER_CC4P                       ((uint32_t)0x00002000)            /*!<Capture/Compare 4 output Polarity */
#define  TIM_CCER_CC4NP                      ((uint32_t)0x00008000)            /*!<Capture/Compare 4 Complementary output Polarity */
#define  TIM_CCER_CC5E                       ((uint32_t)0x00010000)            /*!<Capture/Compare 5 output enable */
#define  TIM_CCER_CC5P                       ((uint32_t)0x00020000)            /*!<Capture/Compare 5 output Polarity */
#define  TIM_CCER_CC6E                       ((uint32_t)0x00100000)            /*!<Capture/Compare 6 output enable */
#define  TIM_CCER_CC6P                       ((uint32_t)0x00200000)            /*!<Capture/Compare 6 output Polarity */


/*******************  Bit definition for TIM_CNT register  ********************/
#define  TIM_CNT_CNT                         ((uint32_t)0xFFFF)            /*!<Counter Value            */

/*******************  Bit definition for TIM_PSC register  ********************/
#define  TIM_PSC_PSC                         ((uint32_t)0xFFFF)            /*!<Prescaler Value          */

/*******************  Bit definition for TIM_ARR register  ********************/
#define  TIM_ARR_ARR                         ((uint32_t)0xFFFF)            /*!<actual auto-reload Value */

/*******************  Bit definition for TIM_RCR register  ********************/
#define  TIM_RCR_REP                         ((uint8_t)0xFF)               /*!<Repetition Counter Value */

/*******************  Bit definition for TIM_CCR1 register  *******************/
#define  TIM_CCR1_CCR1                       ((uint32_t)0xFFFF)            /*!<Capture/Compare 1 Value  */

/*******************  Bit definition for TIM_CCR2 register  *******************/
#define  TIM_CCR2_CCR2                       ((uint32_t)0xFFFF)            /*!<Capture/Compare 2 Value  */

/*******************  Bit definition for TIM_CCR3 register  *******************/
#define  TIM_CCR3_CCR3                       ((uint32_t)0xFFFF)            /*!<Capture/Compare 3 Value  */

/*******************  Bit definition for TIM_CCR4 register  *******************/
#define  TIM_CCR4_CCR4                       ((uint32_t)0xFFFF)            /*!<Capture/Compare 4 Value  */

/*******************  Bit definition for TIM_BDTR register  *******************/
#define  TIM_BDTR_DTG                        ((uint32_t)0x000000FF)            /*!<DTG[0:7] bits (Dead-Time Generator set-up) */
#define  TIM_BDTR_DTG_0                      ((uint32_t)0x00000001)            /*!<Bit 0 */
#define  TIM_BDTR_DTG_1                      ((uint32_t)0x00000002)            /*!<Bit 1 */
#define  TIM_BDTR_DTG_2                      ((uint32_t)0x00000004)            /*!<Bit 2 */
#define  TIM_BDTR_DTG_3                      ((uint32_t)0x00000008)            /*!<Bit 3 */
#define  TIM_BDTR_DTG_4                      ((uint32_t)0x00000010)            /*!<Bit 4 */
#define  TIM_BDTR_DTG_5                      ((uint32_t)0x00000020)            /*!<Bit 5 */
#define  TIM_BDTR_DTG_6                      ((uint32_t)0x00000040)            /*!<Bit 6 */
#define  TIM_BDTR_DTG_7                      ((uint32_t)0x00000080)            /*!<Bit 7 */

#define  TIM_BDTR_LOCK                       ((uint32_t)0x00000300)            /*!<LOCK[1:0] bits (Lock Configuration) */
#define  TIM_BDTR_LOCK_0                     ((uint32_t)0x00000100)            /*!<Bit 0 */
#define  TIM_BDTR_LOCK_1                     ((uint32_t)0x00000200)            /*!<Bit 1 */

#define  TIM_BDTR_OSSI                       ((uint32_t)0x00000400)            /*!<Off-State Selection for Idle mode */
#define  TIM_BDTR_OSSR                       ((uint32_t)0x00000800)            /*!<Off-State Selection for Run mode  */
#define  TIM_BDTR_BKE                        ((uint32_t)0x00001000)            /*!<Break enable                      */
#define  TIM_BDTR_BKP                        ((uint32_t)0x00002000)            /*!<Break Polarity                    */
#define  TIM_BDTR_AOE                        ((uint32_t)0x00004000)            /*!<Automatic Output enable           */
#define  TIM_BDTR_MOE                        ((uint32_t)0x00008000)            /*!<Main Output enable                */
#define  TIM_BDTR_BKF                        ((uint32_t)0x000F0000)            /*!<Break Filter for Break1 */
#define  TIM_BDTR_BK2F                       ((uint32_t)0x00F00000)            /*!<Break Filter for Break2 */
#define  TIM_BDTR_BK2E                       ((uint32_t)0x01000000)            /*!<Break enable for Break2 */
#define  TIM_BDTR_BK2P                       ((uint32_t)0x02000000)            /*!<Break Polarity for Break2 */

/*******************  Bit definition for TIM_DCR register  ********************/
#define  TIM_DCR_DBA                         ((uint32_t)0x001F)            /*!<DBA[4:0] bits (DMA Base Address) */
#define  TIM_DCR_DBA_0                       ((uint32_t)0x0001)            /*!<Bit 0 */
#define  TIM_DCR_DBA_1                       ((uint32_t)0x0002)            /*!<Bit 1 */
#define  TIM_DCR_DBA_2                       ((uint32_t)0x0004)            /*!<Bit 2 */
#define  TIM_DCR_DBA_3                       ((uint32_t)0x0008)            /*!<Bit 3 */
#define  TIM_DCR_DBA_4                       ((uint32_t)0x0010)            /*!<Bit 4 */

#define  TIM_DCR_DBL                         ((uint32_t)0x1F00)            /*!<DBL[4:0] bits (DMA Burst Length) */
#define  TIM_DCR_DBL_0                       ((uint32_t)0x0100)            /*!<Bit 0 */
#define  TIM_DCR_DBL_1                       ((uint32_t)0x0200)            /*!<Bit 1 */
#define  TIM_DCR_DBL_2                       ((uint32_t)0x0400)            /*!<Bit 2 */
#define  TIM_DCR_DBL_3                       ((uint32_t)0x0800)            /*!<Bit 3 */
#define  TIM_DCR_DBL_4                       ((uint32_t)0x1000)            /*!<Bit 4 */

/*******************  Bit definition for TIM_DMAR register  *******************/
#define  TIM_DMAR_DMAB                       ((uint32_t)0xFFFF)            /*!<DMA register for burst accesses                    */

/*******************  Bit definition for TIM_OR register  *********************/
#define TIM_OR_TI4_RMP                       ((uint32_t)0x00C0)            /*!<TI4_RMP[1:0] bits (TIM5 Input 4 remap)             */
#define TIM_OR_TI4_RMP_0                     ((uint32_t)0x0040)            /*!<Bit 0 */
#define TIM_OR_TI4_RMP_1                     ((uint32_t)0x0080)            /*!<Bit 1 */
#define TIM_OR_ITR1_RMP                      ((uint32_t)0x0C00)            /*!<ITR1_RMP[1:0] bits (TIM2 Internal trigger 1 remap) */
#define TIM_OR_ITR1_RMP_0                    ((uint32_t)0x0400)            /*!<Bit 0 */
#define TIM_OR_ITR1_RMP_1                    ((uint32_t)0x0800)            /*!<Bit 1 */

/******************  Bit definition for TIM_CCMR3 register  *******************/
#define  TIM_CCMR3_OC5FE                     ((uint32_t)0x00000004)            /*!<Output Compare 5 Fast enable */
#define  TIM_CCMR3_OC5PE                     ((uint32_t)0x00000008)            /*!<Output Compare 5 Preload enable */

#define  TIM_CCMR3_OC5M                      ((uint32_t)0x00010070)            /*!<OC5M[2:0] bits (Output Compare 5 Mode) */
#define  TIM_CCMR3_OC5M_0                    ((uint32_t)0x00000010)            /*!<Bit 0 */
#define  TIM_CCMR3_OC5M_1                    ((uint32_t)0x00000020)            /*!<Bit 1 */
#define  TIM_CCMR3_OC5M_2                    ((uint32_t)0x00000040)            /*!<Bit 2 */
#define  TIM_CCMR3_OC5M_3                    ((uint32_t)0x00010000)            /*!<Bit 3 */

#define  TIM_CCMR3_OC5CE                     ((uint32_t)0x00000080)            /*!<Output Compare 5 Clear Enable */

#define  TIM_CCMR3_OC6FE                     ((uint32_t)0x00000400)            /*!<Output Compare 4 Fast enable */
#define  TIM_CCMR3_OC6PE                     ((uint32_t)0x00000800)            /*!<Output Compare 4 Preload enable */

#define  TIM_CCMR3_OC6M                      ((uint32_t)0x01007000)            /*!<OC4M[2:0] bits (Output Compare 4 Mode) */
#define  TIM_CCMR3_OC6M_0                    ((uint32_t)0x00001000)            /*!<Bit 0 */
#define  TIM_CCMR3_OC6M_1                    ((uint32_t)0x00002000)            /*!<Bit 1 */
#define  TIM_CCMR3_OC6M_2                    ((uint32_t)0x00004000)            /*!<Bit 2 */
#define  TIM_CCMR3_OC6M_3                    ((uint32_t)0x01000000)            /*!<Bit 3 */

#define  TIM_CCMR3_OC6CE                     ((uint32_t)0x00008000)            /*!<Output Compare 4 Clear Enable */

/*******************  Bit definition for TIM_CCR5 register  *******************/
#define  TIM_CCR5_CCR5                       ((uint32_t)0xFFFFFFFF)        /*!<Capture/Compare 5 Value */
#define  TIM_CCR5_GC5C1                      ((uint32_t)0x20000000)        /*!<Group Channel 5 and Channel 1 */
#define  TIM_CCR5_GC5C2                      ((uint32_t)0x40000000)        /*!<Group Channel 5 and Channel 2 */
#define  TIM_CCR5_GC5C3                      ((uint32_t)0x80000000)        /*!<Group Channel 5 and Channel 3 */

/*******************  Bit definition for TIM_CCR6 register  *******************/
#define  TIM_CCR6_CCR6                       ((uint16_t)0xFFFF)            /*!<Capture/Compare 6 Value */

/******************************************************************************/
/*                                                                            */
/*                         Low Power Timer (LPTIM)                            */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for LPTIM_ISR register  *******************/
#define  LPTIM_ISR_CMPM                         ((uint32_t)0x00000001)            /*!< Compare match                       */
#define  LPTIM_ISR_ARRM                         ((uint32_t)0x00000002)            /*!< Autoreload match                    */
#define  LPTIM_ISR_EXTTRIG                      ((uint32_t)0x00000004)            /*!< External trigger edge event         */
#define  LPTIM_ISR_CMPOK                        ((uint32_t)0x00000008)            /*!< Compare register update OK          */
#define  LPTIM_ISR_ARROK                        ((uint32_t)0x00000010)            /*!< Autoreload register update OK       */
#define  LPTIM_ISR_UP                           ((uint32_t)0x00000020)            /*!< Counter direction change down to up */
#define  LPTIM_ISR_DOWN                         ((uint32_t)0x00000040)            /*!< Counter direction change up to down */

/******************  Bit definition for LPTIM_ICR register  *******************/
#define  LPTIM_ICR_CMPMCF                       ((uint32_t)0x00000001)            /*!< Compare match Clear Flag                       */
#define  LPTIM_ICR_ARRMCF                       ((uint32_t)0x00000002)            /*!< Autoreload match Clear Flag                    */
#define  LPTIM_ICR_EXTTRIGCF                    ((uint32_t)0x00000004)            /*!< External trigger edge event Clear Flag         */
#define  LPTIM_ICR_CMPOKCF                      ((uint32_t)0x00000008)            /*!< Compare register update OK Clear Flag          */
#define  LPTIM_ICR_ARROKCF                      ((uint32_t)0x00000010)            /*!< Autoreload register update OK Clear Flag       */
#define  LPTIM_ICR_UPCF                         ((uint32_t)0x00000020)            /*!< Counter direction change down to up Clear Flag */
#define  LPTIM_ICR_DOWNCF                       ((uint32_t)0x00000040)            /*!< Counter direction change up to down Clear Flag */

/******************  Bit definition for LPTIM_IER register ********************/
#define  LPTIM_IER_CMPMIE                       ((uint32_t)0x00000001)            /*!< Compare match Interrupt Enable                       */
#define  LPTIM_IER_ARRMIE                       ((uint32_t)0x00000002)            /*!< Autoreload match Interrupt Enable                    */
#define  LPTIM_IER_EXTTRIGIE                    ((uint32_t)0x00000004)            /*!< External trigger edge event Interrupt Enable         */
#define  LPTIM_IER_CMPOKIE                      ((uint32_t)0x00000008)            /*!< Compare register update OK Interrupt Enable          */
#define  LPTIM_IER_ARROKIE                      ((uint32_t)0x00000010)            /*!< Autoreload register update OK Interrupt Enable       */
#define  LPTIM_IER_UPIE                         ((uint32_t)0x00000020)            /*!< Counter direction change down to up Interrupt Enable */
#define  LPTIM_IER_DOWNIE                       ((uint32_t)0x00000040)            /*!< Counter direction change up to down Interrupt Enable */

/******************  Bit definition for LPTIM_CFGR register *******************/
#define  LPTIM_CFGR_CKSEL                       ((uint32_t)0x00000001)             /*!< Clock selector */

#define  LPTIM_CFGR_CKPOL                       ((uint32_t)0x00000006)             /*!< CKPOL[1:0] bits (Clock polarity) */
#define  LPTIM_CFGR_CKPOL_0                     ((uint32_t)0x00000002)             /*!< Bit 0 */
#define  LPTIM_CFGR_CKPOL_1                     ((uint32_t)0x00000004)             /*!< Bit 1 */

#define  LPTIM_CFGR_CKFLT                       ((uint32_t)0x00000018)             /*!< CKFLT[1:0] bits (Configurable digital filter for external clock) */
#define  LPTIM_CFGR_CKFLT_0                     ((uint32_t)0x00000008)             /*!< Bit 0 */
#define  LPTIM_CFGR_CKFLT_1                     ((uint32_t)0x00000010)             /*!< Bit 1 */

#define  LPTIM_CFGR_TRGFLT                      ((uint32_t)0x000000C0)             /*!< TRGFLT[1:0] bits (Configurable digital filter for trigger) */
#define  LPTIM_CFGR_TRGFLT_0                    ((uint32_t)0x00000040)             /*!< Bit 0 */
#define  LPTIM_CFGR_TRGFLT_1                    ((uint32_t)0x00000080)             /*!< Bit 1 */

#define  LPTIM_CFGR_PRESC                       ((uint32_t)0x00000E00)             /*!< PRESC[2:0] bits (Clock prescaler) */
#define  LPTIM_CFGR_PRESC_0                     ((uint32_t)0x00000200)             /*!< Bit 0 */
#define  LPTIM_CFGR_PRESC_1                     ((uint32_t)0x00000400)             /*!< Bit 1 */
#define  LPTIM_CFGR_PRESC_2                     ((uint32_t)0x00000800)             /*!< Bit 2 */

#define  LPTIM_CFGR_TRIGSEL                     ((uint32_t)0x0000E000)             /*!< TRIGSEL[2:0]] bits (Trigger selector) */
#define  LPTIM_CFGR_TRIGSEL_0                   ((uint32_t)0x00002000)             /*!< Bit 0 */
#define  LPTIM_CFGR_TRIGSEL_1                   ((uint32_t)0x00004000)             /*!< Bit 1 */
#define  LPTIM_CFGR_TRIGSEL_2                   ((uint32_t)0x00008000)             /*!< Bit 2 */

#define  LPTIM_CFGR_TRIGEN                      ((uint32_t)0x00060000)             /*!< TRIGEN[1:0] bits (Trigger enable and polarity) */
#define  LPTIM_CFGR_TRIGEN_0                    ((uint32_t)0x00020000)             /*!< Bit 0 */
#define  LPTIM_CFGR_TRIGEN_1                    ((uint32_t)0x00040000)             /*!< Bit 1 */

#define  LPTIM_CFGR_TIMOUT                      ((uint32_t)0x00080000)             /*!< Timout enable           */
#define  LPTIM_CFGR_WAVE                        ((uint32_t)0x00100000)             /*!< Waveform shape          */
#define  LPTIM_CFGR_WAVPOL                      ((uint32_t)0x00200000)             /*!< Waveform shape polarity */
#define  LPTIM_CFGR_PRELOAD                     ((uint32_t)0x00400000)             /*!< Reg update mode         */
#define  LPTIM_CFGR_COUNTMODE                   ((uint32_t)0x00800000)             /*!< Counter mode enable     */
#define  LPTIM_CFGR_ENC                         ((uint32_t)0x01000000)             /*!< Encoder mode enable     */

/******************  Bit definition for LPTIM_CR register  ********************/
#define  LPTIM_CR_ENABLE                        ((uint32_t)0x00000001)             /*!< LPTIMer enable                 */
#define  LPTIM_CR_SNGSTRT                       ((uint32_t)0x00080002)             /*!< Timer start in single mode     */
#define  LPTIM_CR_CNTSTRT                       ((uint32_t)0x00000004)             /*!< Timer start in continuous mode */

/******************  Bit definition for LPTIM_CMP register  *******************/
#define  LPTIM_CMP_CMP                          ((uint32_t)0x0000FFFF)             /*!< Compare register     */

/******************  Bit definition for LPTIM_ARR register  *******************/
#define  LPTIM_ARR_ARR                          ((uint32_t)0x0000FFFF)             /*!< Auto reload register */

/******************  Bit definition for LPTIM_CNT register  *******************/
#define  LPTIM_CNT_CNT                          ((uint32_t)0x0000FFFF)             /*!< Counter register     */
/******************************************************************************/
/*                                                                            */
/*      Universal Synchronous Asynchronous Receiver Transmitter (USART)       */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for USART_CR1 register  *******************/
#define  USART_CR1_UE                        ((uint32_t)0x00000001)            /*!< USART Enable                                    */
#define  USART_CR1_RE                        ((uint32_t)0x00000004)            /*!< Receiver Enable                                 */
#define  USART_CR1_TE                        ((uint32_t)0x00000008)            /*!< Transmitter Enable                              */
#define  USART_CR1_IDLEIE                    ((uint32_t)0x00000010)            /*!< IDLE Interrupt Enable                           */
#define  USART_CR1_RXNEIE                    ((uint32_t)0x00000020)            /*!< RXNE Interrupt Enable                           */
#define  USART_CR1_TCIE                      ((uint32_t)0x00000040)            /*!< Transmission Complete Interrupt Enable          */
#define  USART_CR1_TXEIE                     ((uint32_t)0x00000080)            /*!< TXE Interrupt Enable                            */
#define  USART_CR1_PEIE                      ((uint32_t)0x00000100)            /*!< PE Interrupt Enable                             */
#define  USART_CR1_PS                        ((uint32_t)0x00000200)            /*!< Parity Selection                                */
#define  USART_CR1_PCE                       ((uint32_t)0x00000400)            /*!< Parity Control Enable                           */
#define  USART_CR1_WAKE                      ((uint32_t)0x00000800)            /*!< Receiver Wakeup method                          */
#define  USART_CR1_M                         ((uint32_t)0x10001000)            /*!< Word length                                     */
#define  USART_CR1_M_0                       ((uint32_t)0x00001000)            /*!< Word length - Bit 0                             */
#define  USART_CR1_MME                       ((uint32_t)0x00002000)            /*!< Mute Mode Enable                                */
#define  USART_CR1_CMIE                      ((uint32_t)0x00004000)            /*!< Character match interrupt enable                */
#define  USART_CR1_OVER8                     ((uint32_t)0x00008000)            /*!< Oversampling by 8-bit or 16-bit mode            */
#define  USART_CR1_DEDT                      ((uint32_t)0x001F0000)            /*!< DEDT[4:0] bits (Driver Enable Deassertion Time) */
#define  USART_CR1_DEDT_0                    ((uint32_t)0x00010000)            /*!< Bit 0 */
#define  USART_CR1_DEDT_1                    ((uint32_t)0x00020000)            /*!< Bit 1 */
#define  USART_CR1_DEDT_2                    ((uint32_t)0x00040000)            /*!< Bit 2 */
#define  USART_CR1_DEDT_3                    ((uint32_t)0x00080000)            /*!< Bit 3 */
#define  USART_CR1_DEDT_4                    ((uint32_t)0x00100000)            /*!< Bit 4 */
#define  USART_CR1_DEAT                      ((uint32_t)0x03E00000)            /*!< DEAT[4:0] bits (Driver Enable Assertion Time)   */
#define  USART_CR1_DEAT_0                    ((uint32_t)0x00200000)            /*!< Bit 0 */
#define  USART_CR1_DEAT_1                    ((uint32_t)0x00400000)            /*!< Bit 1 */
#define  USART_CR1_DEAT_2                    ((uint32_t)0x00800000)            /*!< Bit 2 */
#define  USART_CR1_DEAT_3                    ((uint32_t)0x01000000)            /*!< Bit 3 */
#define  USART_CR1_DEAT_4                    ((uint32_t)0x02000000)            /*!< Bit 4 */
#define  USART_CR1_RTOIE                     ((uint32_t)0x04000000)            /*!< Receive Time Out interrupt enable */
#define  USART_CR1_EOBIE                     ((uint32_t)0x08000000)            /*!< End of Block interrupt enable     */
#define  USART_CR1_M_1                       ((uint32_t)0x10000000)            /*!< Word length - Bit 1               */

/******************  Bit definition for USART_CR2 register  *******************/
#define  USART_CR2_ADDM7                     ((uint32_t)0x00000010)            /*!< 7-bit or 4-bit Address Detection       */
#define  USART_CR2_LBDL                      ((uint32_t)0x00000020)            /*!< LIN Break Detection Length             */
#define  USART_CR2_LBDIE                     ((uint32_t)0x00000040)            /*!< LIN Break Detection Interrupt Enable   */
#define  USART_CR2_LBCL                      ((uint32_t)0x00000100)            /*!< Last Bit Clock pulse                   */
#define  USART_CR2_CPHA                      ((uint32_t)0x00000200)            /*!< Clock Phase                            */
#define  USART_CR2_CPOL                      ((uint32_t)0x00000400)            /*!< Clock Polarity                         */
#define  USART_CR2_CLKEN                     ((uint32_t)0x00000800)            /*!< Clock Enable                           */
#define  USART_CR2_STOP                      ((uint32_t)0x00003000)            /*!< STOP[1:0] bits (STOP bits)             */
#define  USART_CR2_STOP_0                    ((uint32_t)0x00001000)            /*!< Bit 0 */
#define  USART_CR2_STOP_1                    ((uint32_t)0x00002000)            /*!< Bit 1 */
#define  USART_CR2_LINEN                     ((uint32_t)0x00004000)            /*!< LIN mode enable                        */
#define  USART_CR2_SWAP                      ((uint32_t)0x00008000)            /*!< SWAP TX/RX pins                        */
#define  USART_CR2_RXINV                     ((uint32_t)0x00010000)            /*!< RX pin active level inversion          */
#define  USART_CR2_TXINV                     ((uint32_t)0x00020000)            /*!< TX pin active level inversion          */
#define  USART_CR2_DATAINV                   ((uint32_t)0x00040000)            /*!< Binary data inversion                  */
#define  USART_CR2_MSBFIRST                  ((uint32_t)0x00080000)            /*!< Most Significant Bit First             */
#define  USART_CR2_ABREN                     ((uint32_t)0x00100000)            /*!< Auto Baud-Rate Enable                  */
#define  USART_CR2_ABRMODE                   ((uint32_t)0x00600000)            /*!< ABRMOD[1:0] bits (Auto Baud-Rate Mode) */
#define  USART_CR2_ABRMODE_0                 ((uint32_t)0x00200000)            /*!< Bit 0 */
#define  USART_CR2_ABRMODE_1                 ((uint32_t)0x00400000)            /*!< Bit 1 */
#define  USART_CR2_RTOEN                     ((uint32_t)0x00800000)            /*!< Receiver Time-Out enable  */
#define  USART_CR2_ADD                       ((uint32_t)0xFF000000)            /*!< Address of the USART node */

/******************  Bit definition for USART_CR3 register  *******************/
#define  USART_CR3_EIE                       ((uint32_t)0x00000001)            /*!< Error Interrupt Enable                         */
#define  USART_CR3_IREN                      ((uint32_t)0x00000002)            /*!< IrDA mode Enable                               */
#define  USART_CR3_IRLP                      ((uint32_t)0x00000004)            /*!< IrDA Low-Power                                 */
#define  USART_CR3_HDSEL                     ((uint32_t)0x00000008)            /*!< Half-Duplex Selection                          */
#define  USART_CR3_NACK                      ((uint32_t)0x00000010)            /*!< SmartCard NACK enable                          */
#define  USART_CR3_SCEN                      ((uint32_t)0x00000020)            /*!< SmartCard mode enable                          */
#define  USART_CR3_DMAR                      ((uint32_t)0x00000040)            /*!< DMA Enable Receiver                            */
#define  USART_CR3_DMAT                      ((uint32_t)0x00000080)            /*!< DMA Enable Transmitter                         */
#define  USART_CR3_RTSE                      ((uint32_t)0x00000100)            /*!< RTS Enable                                     */
#define  USART_CR3_CTSE                      ((uint32_t)0x00000200)            /*!< CTS Enable                                     */
#define  USART_CR3_CTSIE                     ((uint32_t)0x00000400)            /*!< CTS Interrupt Enable                           */
#define  USART_CR3_ONEBIT                    ((uint32_t)0x00000800)            /*!< One sample bit method enable                   */
#define  USART_CR3_OVRDIS                    ((uint32_t)0x00001000)            /*!< Overrun Disable                                */
#define  USART_CR3_DDRE                      ((uint32_t)0x00002000)            /*!< DMA Disable on Reception Error                 */
#define  USART_CR3_DEM                       ((uint32_t)0x00004000)            /*!< Driver Enable Mode                             */
#define  USART_CR3_DEP                       ((uint32_t)0x00008000)            /*!< Driver Enable Polarity Selection               */
#define  USART_CR3_SCARCNT                   ((uint32_t)0x000E0000)            /*!< SCARCNT[2:0] bits (SmartCard Auto-Retry Count) */
#define  USART_CR3_SCARCNT_0                 ((uint32_t)0x00020000)            /*!< Bit 0 */
#define  USART_CR3_SCARCNT_1                 ((uint32_t)0x00040000)            /*!< Bit 1 */
#define  USART_CR3_SCARCNT_2                 ((uint32_t)0x00080000)            /*!< Bit 2 */

/******************  Bit definition for USART_BRR register  *******************/
#define  USART_BRR_DIV_FRACTION              ((uint32_t)0x000F)                /*!< Fraction of USARTDIV */
#define  USART_BRR_DIV_MANTISSA              ((uint32_t)0xFFF0)                /*!< Mantissa of USARTDIV */

/******************  Bit definition for USART_GTPR register  ******************/
#define  USART_GTPR_PSC                      ((uint32_t)0x00FF)                /*!< PSC[7:0] bits (Prescaler value) */
#define  USART_GTPR_GT                       ((uint32_t)0xFF00)                /*!< GT[7:0] bits (Guard time value) */


/*******************  Bit definition for USART_RTOR register  *****************/
#define  USART_RTOR_RTO                      ((uint32_t)0x00FFFFFF)            /*!< Receiver Time Out Value */
#define  USART_RTOR_BLEN                     ((uint32_t)0xFF000000)            /*!< Block Length */

/*******************  Bit definition for USART_RQR register  ******************/
#define  USART_RQR_ABRRQ                     ((uint32_t)0x0001)                /*!< Auto-Baud Rate Request      */
#define  USART_RQR_SBKRQ                     ((uint32_t)0x0002)                /*!< Send Break Request          */
#define  USART_RQR_MMRQ                      ((uint32_t)0x0004)                /*!< Mute Mode Request           */
#define  USART_RQR_RXFRQ                     ((uint32_t)0x0008)                /*!< Receive Data flush Request  */
#define  USART_RQR_TXFRQ                     ((uint32_t)0x0010)                /*!< Transmit data flush Request */

/*******************  Bit definition for USART_ISR register  ******************/
#define  USART_ISR_PE                        ((uint32_t)0x00000001)            /*!< Parity Error                        */
#define  USART_ISR_FE                        ((uint32_t)0x00000002)            /*!< Framing Error                       */
#define  USART_ISR_NE                        ((uint32_t)0x00000004)            /*!< Noise detected Flag                 */
#define  USART_ISR_ORE                       ((uint32_t)0x00000008)            /*!< OverRun Error                       */
#define  USART_ISR_IDLE                      ((uint32_t)0x00000010)            /*!< IDLE line detected                  */
#define  USART_ISR_RXNE                      ((uint32_t)0x00000020)            /*!< Read Data Register Not Empty        */
#define  USART_ISR_TC                        ((uint32_t)0x00000040)            /*!< Transmission Complete               */
#define  USART_ISR_TXE                       ((uint32_t)0x00000080)            /*!< Transmit Data Register Empty        */
#define  USART_ISR_LBD                       ((uint32_t)0x00000100)            /*!< LIN Break Detection Flag            */
#define  USART_ISR_CTSIF                     ((uint32_t)0x00000200)            /*!< CTS interrupt flag                  */
#define  USART_ISR_CTS                       ((uint32_t)0x00000400)            /*!< CTS flag                            */
#define  USART_ISR_RTOF                      ((uint32_t)0x00000800)            /*!< Receiver Time Out                   */
#define  USART_ISR_EOBF                      ((uint32_t)0x00001000)            /*!< End Of Block Flag                   */
#define  USART_ISR_ABRE                      ((uint32_t)0x00004000)            /*!< Auto-Baud Rate Error                */
#define  USART_ISR_ABRF                      ((uint32_t)0x00008000)            /*!< Auto-Baud Rate Flag                 */
#define  USART_ISR_BUSY                      ((uint32_t)0x00010000)            /*!< Busy Flag                           */
#define  USART_ISR_CMF                       ((uint32_t)0x00020000)            /*!< Character Match Flag                */
#define  USART_ISR_SBKF                      ((uint32_t)0x00040000)            /*!< Send Break Flag                     */
#define  USART_ISR_RWU                       ((uint32_t)0x00080000)            /*!< Receive Wake Up from mute mode Flag */
#define  USART_ISR_WUF                       ((uint32_t)0x00100000)            /*!< Wake Up from stop mode Flag         */
#define  USART_ISR_TEACK                     ((uint32_t)0x00200000)            /*!< Transmit Enable Acknowledge Flag    */
#define  USART_ISR_REACK                     ((uint32_t)0x00400000)            /*!< Receive Enable Acknowledge Flag     */

/*******************  Bit definition for USART_ICR register  ******************/
#define  USART_ICR_PECF                      ((uint32_t)0x00000001)            /*!< Parity Error Clear Flag             */
#define  USART_ICR_FECF                      ((uint32_t)0x00000002)            /*!< Framing Error Clear Flag            */
#define  USART_ICR_NCF                       ((uint32_t)0x00000004)            /*!< Noise detected Clear Flag           */
#define  USART_ICR_ORECF                     ((uint32_t)0x00000008)            /*!< OverRun Error Clear Flag            */
#define  USART_ICR_IDLECF                    ((uint32_t)0x00000010)            /*!< IDLE line detected Clear Flag       */
#define  USART_ICR_TCCF                      ((uint32_t)0x00000040)            /*!< Transmission Complete Clear Flag    */
#define  USART_ICR_LBDCF                     ((uint32_t)0x00000100)            /*!< LIN Break Detection Clear Flag      */
#define  USART_ICR_CTSCF                     ((uint32_t)0x00000200)            /*!< CTS Interrupt Clear Flag            */
#define  USART_ICR_RTOCF                     ((uint32_t)0x00000800)            /*!< Receiver Time Out Clear Flag        */
#define  USART_ICR_EOBCF                     ((uint32_t)0x00001000)            /*!< End Of Block Clear Flag             */
#define  USART_ICR_CMCF                      ((uint32_t)0x00020000)            /*!< Character Match Clear Flag          */
#define  USART_ICR_WUCF                      ((uint32_t)0x00100000)            /*!< Wake Up from stop mode Clear Flag   */

/*******************  Bit definition for USART_RDR register  ******************/
#define  USART_RDR_RDR                       ((uint32_t)0x01FF)                /*!< RDR[8:0] bits (Receive Data value) */

/*******************  Bit definition for USART_TDR register  ******************/
#define  USART_TDR_TDR                       ((uint32_t)0x01FF)                /*!< TDR[8:0] bits (Transmit Data value) */

/******************************************************************************/
/*                                                                            */
/*                            Window WATCHDOG                                 */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for WWDG_CR register  ********************/
#define  WWDG_CR_T                           ((uint32_t)0x7F)               /*!<T[6:0] bits (7-Bit counter (MSB to LSB)) */
#define  WWDG_CR_T0                          ((uint32_t)0x01)               /*!<Bit 0 */
#define  WWDG_CR_T1                          ((uint32_t)0x02)               /*!<Bit 1 */
#define  WWDG_CR_T2                          ((uint32_t)0x04)               /*!<Bit 2 */
#define  WWDG_CR_T3                          ((uint32_t)0x08)               /*!<Bit 3 */
#define  WWDG_CR_T4                          ((uint32_t)0x10)               /*!<Bit 4 */
#define  WWDG_CR_T5                          ((uint32_t)0x20)               /*!<Bit 5 */
#define  WWDG_CR_T6                          ((uint32_t)0x40)               /*!<Bit 6 */

#define  WWDG_CR_WDGA                        ((uint32_t)0x80)               /*!<Activation bit */

/*******************  Bit definition for WWDG_CFR register  *******************/
#define  WWDG_CFR_W                          ((uint32_t)0x007F)            /*!<W[6:0] bits (7-bit window value) */
#define  WWDG_CFR_W0                         ((uint32_t)0x0001)            /*!<Bit 0 */
#define  WWDG_CFR_W1                         ((uint32_t)0x0002)            /*!<Bit 1 */
#define  WWDG_CFR_W2                         ((uint32_t)0x0004)            /*!<Bit 2 */
#define  WWDG_CFR_W3                         ((uint32_t)0x0008)            /*!<Bit 3 */
#define  WWDG_CFR_W4                         ((uint32_t)0x0010)            /*!<Bit 4 */
#define  WWDG_CFR_W5                         ((uint32_t)0x0020)            /*!<Bit 5 */
#define  WWDG_CFR_W6                         ((uint32_t)0x0040)            /*!<Bit 6 */

#define  WWDG_CFR_WDGTB                      ((uint32_t)0x0180)            /*!<WDGTB[1:0] bits (Timer Base) */
#define  WWDG_CFR_WDGTB0                     ((uint32_t)0x0080)            /*!<Bit 0 */
#define  WWDG_CFR_WDGTB1                     ((uint32_t)0x0100)            /*!<Bit 1 */

#define  WWDG_CFR_EWI                        ((uint32_t)0x0200)            /*!<Early Wakeup Interrupt */

/*******************  Bit definition for WWDG_SR register  ********************/
#define  WWDG_SR_EWIF                        ((uint32_t)0x01)               /*!<Early Wakeup Interrupt Flag */

/******************************************************************************/
/*                                                                            */
/*                                DBG                                         */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for DBGMCU_IDCODE register  *************/
#define  DBGMCU_IDCODE_DEV_ID                ((uint32_t)0x00000FFF)
#define  DBGMCU_IDCODE_REV_ID                ((uint32_t)0xFFFF0000)

/********************  Bit definition for DBGMCU_CR register  *****************/
#define  DBGMCU_CR_DBG_SLEEP                 ((uint32_t)0x00000001)
#define  DBGMCU_CR_DBG_STOP                  ((uint32_t)0x00000002)
#define  DBGMCU_CR_DBG_STANDBY               ((uint32_t)0x00000004)
#define  DBGMCU_CR_TRACE_IOEN                ((uint32_t)0x00000020)

#define  DBGMCU_CR_TRACE_MODE                ((uint32_t)0x000000C0)
#define  DBGMCU_CR_TRACE_MODE_0              ((uint32_t)0x00000040) /*!<Bit 0 */
#define  DBGMCU_CR_TRACE_MODE_1              ((uint32_t)0x00000080) /*!<Bit 1 */

/********************  Bit definition for DBGMCU_APB1_FZ register  ************/
#define  DBGMCU_APB1_FZ_DBG_TIM2_STOP            ((uint32_t)0x00000001)
#define  DBGMCU_APB1_FZ_DBG_TIM3_STOP            ((uint32_t)0x00000002)
#define  DBGMCU_APB1_FZ_DBG_TIM4_STOP            ((uint32_t)0x00000004)
#define  DBGMCU_APB1_FZ_DBG_TIM5_STOP            ((uint32_t)0x00000008)
#define  DBGMCU_APB1_FZ_DBG_TIM6_STOP            ((uint32_t)0x00000010)
#define  DBGMCU_APB1_FZ_DBG_TIM7_STOP            ((uint32_t)0x00000020)
#define  DBGMCU_APB1_FZ_DBG_TIM12_STOP           ((uint32_t)0x00000040)
#define  DBGMCU_APB1_FZ_DBG_TIM13_STOP           ((uint32_t)0x00000080)
#define  DBGMCU_APB1_FZ_DBG_TIM14_STOP           ((uint32_t)0x00000100)
#define  DBGMCU_APB1_FZ_DBG_RTC_STOP             ((uint32_t)0x00000400)
#define  DBGMCU_APB1_FZ_DBG_WWDG_STOP            ((uint32_t)0x00000800)
#define  DBGMCU_APB1_FZ_DBG_IWDG_STOP            ((uint32_t)0x00001000)
#define  DBGMCU_APB1_FZ_DBG_I2C1_SMBUS_TIMEOUT   ((uint32_t)0x00200000)
#define  DBGMCU_APB1_FZ_DBG_I2C2_SMBUS_TIMEOUT   ((uint32_t)0x00400000)
#define  DBGMCU_APB1_FZ_DBG_I2C3_SMBUS_TIMEOUT   ((uint32_t)0x00800000)
#define  DBGMCU_APB1_FZ_DBG_CAN1_STOP            ((uint32_t)0x02000000)
#define  DBGMCU_APB1_FZ_DBG_CAN2_STOP            ((uint32_t)0x04000000)

/********************  Bit definition for DBGMCU_APB2_FZ register  ************/
#define  DBGMCU_APB1_FZ_DBG_TIM1_STOP        ((uint32_t)0x00000001)
#define  DBGMCU_APB1_FZ_DBG_TIM8_STOP        ((uint32_t)0x00000002)
#define  DBGMCU_APB1_FZ_DBG_TIM9_STOP        ((uint32_t)0x00010000)
#define  DBGMCU_APB1_FZ_DBG_TIM10_STOP       ((uint32_t)0x00020000)
#define  DBGMCU_APB1_FZ_DBG_TIM11_STOP       ((uint32_t)0x00040000)

/******************************************************************************/
/*                                                                            */
/*                Ethernet MAC Registers bits definitions                     */
/*                                                                            */
/******************************************************************************/
/* Bit definition for Ethernet MAC Control Register register */
#define ETH_MACCR_WD      ((uint32_t)0x00800000)  /* Watchdog disable */
#define ETH_MACCR_JD      ((uint32_t)0x00400000)  /* Jabber disable */
#define ETH_MACCR_IFG     ((uint32_t)0x000E0000)  /* Inter-frame gap */
#define ETH_MACCR_IFG_96Bit     ((uint32_t)0x00000000)  /* Minimum IFG between frames during transmission is 96Bit */
  #define ETH_MACCR_IFG_88Bit     ((uint32_t)0x00020000)  /* Minimum IFG between frames during transmission is 88Bit */
  #define ETH_MACCR_IFG_80Bit     ((uint32_t)0x00040000)  /* Minimum IFG between frames during transmission is 80Bit */
  #define ETH_MACCR_IFG_72Bit     ((uint32_t)0x00060000)  /* Minimum IFG between frames during transmission is 72Bit */
  #define ETH_MACCR_IFG_64Bit     ((uint32_t)0x00080000)  /* Minimum IFG between frames during transmission is 64Bit */
  #define ETH_MACCR_IFG_56Bit     ((uint32_t)0x000A0000)  /* Minimum IFG between frames during transmission is 56Bit */
  #define ETH_MACCR_IFG_48Bit     ((uint32_t)0x000C0000)  /* Minimum IFG between frames during transmission is 48Bit */
  #define ETH_MACCR_IFG_40Bit     ((uint32_t)0x000E0000)  /* Minimum IFG between frames during transmission is 40Bit */
#define ETH_MACCR_CSD     ((uint32_t)0x00010000)  /* Carrier sense disable (during transmission) */
#define ETH_MACCR_FES     ((uint32_t)0x00004000)  /* Fast ethernet speed */
#define ETH_MACCR_ROD     ((uint32_t)0x00002000)  /* Receive own disable */
#define ETH_MACCR_LM      ((uint32_t)0x00001000)  /* loopback mode */
#define ETH_MACCR_DM      ((uint32_t)0x00000800)  /* Duplex mode */
#define ETH_MACCR_IPCO    ((uint32_t)0x00000400)  /* IP Checksum offload */
#define ETH_MACCR_RD      ((uint32_t)0x00000200)  /* Retry disable */
#define ETH_MACCR_APCS    ((uint32_t)0x00000080)  /* Automatic Pad/CRC stripping */
#define ETH_MACCR_BL      ((uint32_t)0x00000060)  /* Back-off limit: random integer number (r) of slot time delays before rescheduling
                                                       a transmission attempt during retries after a collision: 0 =< r <2^k */
  #define ETH_MACCR_BL_10    ((uint32_t)0x00000000)  /* k = min (n, 10) */
  #define ETH_MACCR_BL_8     ((uint32_t)0x00000020)  /* k = min (n, 8) */
  #define ETH_MACCR_BL_4     ((uint32_t)0x00000040)  /* k = min (n, 4) */
  #define ETH_MACCR_BL_1     ((uint32_t)0x00000060)  /* k = min (n, 1) */
#define ETH_MACCR_DC      ((uint32_t)0x00000010)  /* Defferal check */
#define ETH_MACCR_TE      ((uint32_t)0x00000008)  /* Transmitter enable */
#define ETH_MACCR_RE      ((uint32_t)0x00000004)  /* Receiver enable */

/* Bit definition for Ethernet MAC Frame Filter Register */
#define ETH_MACFFR_RA     ((uint32_t)0x80000000)  /* Receive all */
#define ETH_MACFFR_HPF    ((uint32_t)0x00000400)  /* Hash or perfect filter */
#define ETH_MACFFR_SAF    ((uint32_t)0x00000200)  /* Source address filter enable */
#define ETH_MACFFR_SAIF   ((uint32_t)0x00000100)  /* SA inverse filtering */
#define ETH_MACFFR_PCF    ((uint32_t)0x000000C0)  /* Pass control frames: 3 cases */
  #define ETH_MACFFR_PCF_BlockAll                ((uint32_t)0x00000040)  /* MAC filters all control frames from reaching the application */
  #define ETH_MACFFR_PCF_ForwardAll              ((uint32_t)0x00000080)  /* MAC forwards all control frames to application even if they fail the Address Filter */
  #define ETH_MACFFR_PCF_ForwardPassedAddrFilter ((uint32_t)0x000000C0)  /* MAC forwards control frames that pass the Address Filter. */
#define ETH_MACFFR_BFD    ((uint32_t)0x00000020)  /* Broadcast frame disable */
#define ETH_MACFFR_PAM    ((uint32_t)0x00000010)  /* Pass all mutlicast */
#define ETH_MACFFR_DAIF   ((uint32_t)0x00000008)  /* DA Inverse filtering */
#define ETH_MACFFR_HM     ((uint32_t)0x00000004)  /* Hash multicast */
#define ETH_MACFFR_HU     ((uint32_t)0x00000002)  /* Hash unicast */
#define ETH_MACFFR_PM     ((uint32_t)0x00000001)  /* Promiscuous mode */

/* Bit definition for Ethernet MAC Hash Table High Register */
#define ETH_MACHTHR_HTH   ((uint32_t)0xFFFFFFFF)  /* Hash table high */

/* Bit definition for Ethernet MAC Hash Table Low Register */
#define ETH_MACHTLR_HTL   ((uint32_t)0xFFFFFFFF)  /* Hash table low */

/* Bit definition for Ethernet MAC MII Address Register */
#define ETH_MACMIIAR_PA   ((uint32_t)0x0000F800)  /* Physical layer address */
#define ETH_MACMIIAR_MR   ((uint32_t)0x000007C0)  /* MII register in the selected PHY */
#define ETH_MACMIIAR_CR   ((uint32_t)0x0000001C)  /* CR clock range: 6 cases */
  #define ETH_MACMIIAR_CR_Div42   ((uint32_t)0x00000000)  /* HCLK:60-100 MHz; MDC clock= HCLK/42 */
  #define ETH_MACMIIAR_CR_Div62   ((uint32_t)0x00000004)  /* HCLK:100-150 MHz; MDC clock= HCLK/62 */
  #define ETH_MACMIIAR_CR_Div16   ((uint32_t)0x00000008)  /* HCLK:20-35 MHz; MDC clock= HCLK/16 */
  #define ETH_MACMIIAR_CR_Div26   ((uint32_t)0x0000000C)  /* HCLK:35-60 MHz; MDC clock= HCLK/26 */
  #define ETH_MACMIIAR_CR_Div102  ((uint32_t)0x00000010)  /* HCLK:150-168 MHz; MDC clock= HCLK/102 */
#define ETH_MACMIIAR_MW   ((uint32_t)0x00000002)  /* MII write */
#define ETH_MACMIIAR_MB   ((uint32_t)0x00000001)  /* MII busy */

/* Bit definition for Ethernet MAC MII Data Register */
#define ETH_MACMIIDR_MD   ((uint32_t)0x0000FFFF)  /* MII data: read/write data from/to PHY */

/* Bit definition for Ethernet MAC Flow Control Register */
#define ETH_MACFCR_PT     ((uint32_t)0xFFFF0000)  /* Pause time */
#define ETH_MACFCR_ZQPD   ((uint32_t)0x00000080)  /* Zero-quanta pause disable */
#define ETH_MACFCR_PLT    ((uint32_t)0x00000030)  /* Pause low threshold: 4 cases */
  #define ETH_MACFCR_PLT_Minus4   ((uint32_t)0x00000000)  /* Pause time minus 4 slot times */
  #define ETH_MACFCR_PLT_Minus28  ((uint32_t)0x00000010)  /* Pause time minus 28 slot times */
  #define ETH_MACFCR_PLT_Minus144 ((uint32_t)0x00000020)  /* Pause time minus 144 slot times */
  #define ETH_MACFCR_PLT_Minus256 ((uint32_t)0x00000030)  /* Pause time minus 256 slot times */
#define ETH_MACFCR_UPFD   ((uint32_t)0x00000008)  /* Unicast pause frame detect */
#define ETH_MACFCR_RFCE   ((uint32_t)0x00000004)  /* Receive flow control enable */
#define ETH_MACFCR_TFCE   ((uint32_t)0x00000002)  /* Transmit flow control enable */
#define ETH_MACFCR_FCBBPA ((uint32_t)0x00000001)  /* Flow control busy/backpressure activate */

/* Bit definition for Ethernet MAC VLAN Tag Register */
#define ETH_MACVLANTR_VLANTC ((uint32_t)0x00010000)  /* 12-bit VLAN tag comparison */
#define ETH_MACVLANTR_VLANTI ((uint32_t)0x0000FFFF)  /* VLAN tag identifier (for receive frames) */

/* Bit definition for Ethernet MAC Remote Wake-UpFrame Filter Register */
#define ETH_MACRWUFFR_D   ((uint32_t)0xFFFFFFFF)  /* Wake-up frame filter register data */
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
#define ETH_MACPMTCSR_WFFRPR ((uint32_t)0x80000000)  /* Wake-Up Frame Filter Register Pointer Reset */
#define ETH_MACPMTCSR_GU     ((uint32_t)0x00000200)  /* Global Unicast */
#define ETH_MACPMTCSR_WFR    ((uint32_t)0x00000040)  /* Wake-Up Frame Received */
#define ETH_MACPMTCSR_MPR    ((uint32_t)0x00000020)  /* Magic Packet Received */
#define ETH_MACPMTCSR_WFE    ((uint32_t)0x00000004)  /* Wake-Up Frame Enable */
#define ETH_MACPMTCSR_MPE    ((uint32_t)0x00000002)  /* Magic Packet Enable */
#define ETH_MACPMTCSR_PD     ((uint32_t)0x00000001)  /* Power Down */

/* Bit definition for Ethernet MAC Status Register */
#define ETH_MACSR_TSTS      ((uint32_t)0x00000200)  /* Time stamp trigger status */
#define ETH_MACSR_MMCTS     ((uint32_t)0x00000040)  /* MMC transmit status */
#define ETH_MACSR_MMMCRS    ((uint32_t)0x00000020)  /* MMC receive status */
#define ETH_MACSR_MMCS      ((uint32_t)0x00000010)  /* MMC status */
#define ETH_MACSR_PMTS      ((uint32_t)0x00000008)  /* PMT status */

/* Bit definition for Ethernet MAC Interrupt Mask Register */
#define ETH_MACIMR_TSTIM     ((uint32_t)0x00000200)  /* Time stamp trigger interrupt mask */
#define ETH_MACIMR_PMTIM     ((uint32_t)0x00000008)  /* PMT interrupt mask */

/* Bit definition for Ethernet MAC Address0 High Register */
#define ETH_MACA0HR_MACA0H   ((uint32_t)0x0000FFFF)  /* MAC address0 high */

/* Bit definition for Ethernet MAC Address0 Low Register */
#define ETH_MACA0LR_MACA0L   ((uint32_t)0xFFFFFFFF)  /* MAC address0 low */

/* Bit definition for Ethernet MAC Address1 High Register */
#define ETH_MACA1HR_AE       ((uint32_t)0x80000000)  /* Address enable */
#define ETH_MACA1HR_SA       ((uint32_t)0x40000000)  /* Source address */
#define ETH_MACA1HR_MBC      ((uint32_t)0x3F000000)  /* Mask byte control: bits to mask for comparison of the MAC Address bytes */
  #define ETH_MACA1HR_MBC_HBits15_8    ((uint32_t)0x20000000)  /* Mask MAC Address high reg bits [15:8] */
  #define ETH_MACA1HR_MBC_HBits7_0     ((uint32_t)0x10000000)  /* Mask MAC Address high reg bits [7:0] */
  #define ETH_MACA1HR_MBC_LBits31_24   ((uint32_t)0x08000000)  /* Mask MAC Address low reg bits [31:24] */
  #define ETH_MACA1HR_MBC_LBits23_16   ((uint32_t)0x04000000)  /* Mask MAC Address low reg bits [23:16] */
  #define ETH_MACA1HR_MBC_LBits15_8    ((uint32_t)0x02000000)  /* Mask MAC Address low reg bits [15:8] */
  #define ETH_MACA1HR_MBC_LBits7_0     ((uint32_t)0x01000000)  /* Mask MAC Address low reg bits [7:0] */
#define ETH_MACA1HR_MACA1H   ((uint32_t)0x0000FFFF)  /* MAC address1 high */

/* Bit definition for Ethernet MAC Address1 Low Register */
#define ETH_MACA1LR_MACA1L   ((uint32_t)0xFFFFFFFF)  /* MAC address1 low */

/* Bit definition for Ethernet MAC Address2 High Register */
#define ETH_MACA2HR_AE       ((uint32_t)0x80000000)  /* Address enable */
#define ETH_MACA2HR_SA       ((uint32_t)0x40000000)  /* Source address */
#define ETH_MACA2HR_MBC      ((uint32_t)0x3F000000)  /* Mask byte control */
  #define ETH_MACA2HR_MBC_HBits15_8    ((uint32_t)0x20000000)  /* Mask MAC Address high reg bits [15:8] */
  #define ETH_MACA2HR_MBC_HBits7_0     ((uint32_t)0x10000000)  /* Mask MAC Address high reg bits [7:0] */
  #define ETH_MACA2HR_MBC_LBits31_24   ((uint32_t)0x08000000)  /* Mask MAC Address low reg bits [31:24] */
  #define ETH_MACA2HR_MBC_LBits23_16   ((uint32_t)0x04000000)  /* Mask MAC Address low reg bits [23:16] */
  #define ETH_MACA2HR_MBC_LBits15_8    ((uint32_t)0x02000000)  /* Mask MAC Address low reg bits [15:8] */
  #define ETH_MACA2HR_MBC_LBits7_0     ((uint32_t)0x01000000)  /* Mask MAC Address low reg bits [70] */
#define ETH_MACA2HR_MACA2H   ((uint32_t)0x0000FFFF)  /* MAC address1 high */

/* Bit definition for Ethernet MAC Address2 Low Register */
#define ETH_MACA2LR_MACA2L   ((uint32_t)0xFFFFFFFF)  /* MAC address2 low */

/* Bit definition for Ethernet MAC Address3 High Register */
#define ETH_MACA3HR_AE       ((uint32_t)0x80000000)  /* Address enable */
#define ETH_MACA3HR_SA       ((uint32_t)0x40000000)  /* Source address */
#define ETH_MACA3HR_MBC      ((uint32_t)0x3F000000)  /* Mask byte control */
  #define ETH_MACA3HR_MBC_HBits15_8    ((uint32_t)0x20000000)  /* Mask MAC Address high reg bits [15:8] */
  #define ETH_MACA3HR_MBC_HBits7_0     ((uint32_t)0x10000000)  /* Mask MAC Address high reg bits [7:0] */
  #define ETH_MACA3HR_MBC_LBits31_24   ((uint32_t)0x08000000)  /* Mask MAC Address low reg bits [31:24] */
  #define ETH_MACA3HR_MBC_LBits23_16   ((uint32_t)0x04000000)  /* Mask MAC Address low reg bits [23:16] */
  #define ETH_MACA3HR_MBC_LBits15_8    ((uint32_t)0x02000000)  /* Mask MAC Address low reg bits [15:8] */
  #define ETH_MACA3HR_MBC_LBits7_0     ((uint32_t)0x01000000)  /* Mask MAC Address low reg bits [70] */
#define ETH_MACA3HR_MACA3H   ((uint32_t)0x0000FFFF)  /* MAC address3 high */

/* Bit definition for Ethernet MAC Address3 Low Register */
#define ETH_MACA3LR_MACA3L   ((uint32_t)0xFFFFFFFF)  /* MAC address3 low */

/******************************************************************************/
/*                Ethernet MMC Registers bits definition                      */
/******************************************************************************/

/* Bit definition for Ethernet MMC Contol Register */
#define ETH_MMCCR_MCFHP      ((uint32_t)0x00000020)  /* MMC counter Full-Half preset */
#define ETH_MMCCR_MCP        ((uint32_t)0x00000010)  /* MMC counter preset */
#define ETH_MMCCR_MCF        ((uint32_t)0x00000008)  /* MMC Counter Freeze */
#define ETH_MMCCR_ROR        ((uint32_t)0x00000004)  /* Reset on Read */
#define ETH_MMCCR_CSR        ((uint32_t)0x00000002)  /* Counter Stop Rollover */
#define ETH_MMCCR_CR         ((uint32_t)0x00000001)  /* Counters Reset */

/* Bit definition for Ethernet MMC Receive Interrupt Register */
#define ETH_MMCRIR_RGUFS     ((uint32_t)0x00020000)  /* Set when Rx good unicast frames counter reaches half the maximum value */
#define ETH_MMCRIR_RFAES     ((uint32_t)0x00000040)  /* Set when Rx alignment error counter reaches half the maximum value */
#define ETH_MMCRIR_RFCES     ((uint32_t)0x00000020)  /* Set when Rx crc error counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Transmit Interrupt Register */
#define ETH_MMCTIR_TGFS      ((uint32_t)0x00200000)  /* Set when Tx good frame count counter reaches half the maximum value */
#define ETH_MMCTIR_TGFMSCS   ((uint32_t)0x00008000)  /* Set when Tx good multi col counter reaches half the maximum value */
#define ETH_MMCTIR_TGFSCS    ((uint32_t)0x00004000)  /* Set when Tx good single col counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Receive Interrupt Mask Register */
#define ETH_MMCRIMR_RGUFM    ((uint32_t)0x00020000)  /* Mask the interrupt when Rx good unicast frames counter reaches half the maximum value */
#define ETH_MMCRIMR_RFAEM    ((uint32_t)0x00000040)  /* Mask the interrupt when when Rx alignment error counter reaches half the maximum value */
#define ETH_MMCRIMR_RFCEM    ((uint32_t)0x00000020)  /* Mask the interrupt when Rx crc error counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Transmit Interrupt Mask Register */
#define ETH_MMCTIMR_TGFM     ((uint32_t)0x00200000)  /* Mask the interrupt when Tx good frame count counter reaches half the maximum value */
#define ETH_MMCTIMR_TGFMSCM  ((uint32_t)0x00008000)  /* Mask the interrupt when Tx good multi col counter reaches half the maximum value */
#define ETH_MMCTIMR_TGFSCM   ((uint32_t)0x00004000)  /* Mask the interrupt when Tx good single col counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Transmitted Good Frames after Single Collision Counter Register */
#define ETH_MMCTGFSCCR_TGFSCC     ((uint32_t)0xFFFFFFFF)  /* Number of successfully transmitted frames after a single collision in Half-duplex mode. */

/* Bit definition for Ethernet MMC Transmitted Good Frames after More than a Single Collision Counter Register */
#define ETH_MMCTGFMSCCR_TGFMSCC   ((uint32_t)0xFFFFFFFF)  /* Number of successfully transmitted frames after more than a single collision in Half-duplex mode. */

/* Bit definition for Ethernet MMC Transmitted Good Frames Counter Register */
#define ETH_MMCTGFCR_TGFC    ((uint32_t)0xFFFFFFFF)  /* Number of good frames transmitted. */

/* Bit definition for Ethernet MMC Received Frames with CRC Error Counter Register */
#define ETH_MMCRFCECR_RFCEC  ((uint32_t)0xFFFFFFFF)  /* Number of frames received with CRC error. */

/* Bit definition for Ethernet MMC Received Frames with Alignement Error Counter Register */
#define ETH_MMCRFAECR_RFAEC  ((uint32_t)0xFFFFFFFF)  /* Number of frames received with alignment (dribble) error */

/* Bit definition for Ethernet MMC Received Good Unicast Frames Counter Register */
#define ETH_MMCRGUFCR_RGUFC  ((uint32_t)0xFFFFFFFF)  /* Number of good unicast frames received. */

/******************************************************************************/
/*               Ethernet PTP Registers bits definition                       */
/******************************************************************************/

/* Bit definition for Ethernet PTP Time Stamp Contol Register */
#define ETH_PTPTSCR_TSCNT       ((uint32_t)0x00030000)  /* Time stamp clock node type */
#define ETH_PTPTSSR_TSSMRME     ((uint32_t)0x00008000)  /* Time stamp snapshot for message relevant to master enable */
#define ETH_PTPTSSR_TSSEME      ((uint32_t)0x00004000)  /* Time stamp snapshot for event message enable */
#define ETH_PTPTSSR_TSSIPV4FE   ((uint32_t)0x00002000)  /* Time stamp snapshot for IPv4 frames enable */
#define ETH_PTPTSSR_TSSIPV6FE   ((uint32_t)0x00001000)  /* Time stamp snapshot for IPv6 frames enable */
#define ETH_PTPTSSR_TSSPTPOEFE  ((uint32_t)0x00000800)  /* Time stamp snapshot for PTP over ethernet frames enable */
#define ETH_PTPTSSR_TSPTPPSV2E  ((uint32_t)0x00000400)  /* Time stamp PTP packet snooping for version2 format enable */
#define ETH_PTPTSSR_TSSSR       ((uint32_t)0x00000200)  /* Time stamp Sub-seconds rollover */
#define ETH_PTPTSSR_TSSARFE     ((uint32_t)0x00000100)  /* Time stamp snapshot for all received frames enable */

#define ETH_PTPTSCR_TSARU    ((uint32_t)0x00000020)  /* Addend register update */
#define ETH_PTPTSCR_TSITE    ((uint32_t)0x00000010)  /* Time stamp interrupt trigger enable */
#define ETH_PTPTSCR_TSSTU    ((uint32_t)0x00000008)  /* Time stamp update */
#define ETH_PTPTSCR_TSSTI    ((uint32_t)0x00000004)  /* Time stamp initialize */
#define ETH_PTPTSCR_TSFCU    ((uint32_t)0x00000002)  /* Time stamp fine or coarse update */
#define ETH_PTPTSCR_TSE      ((uint32_t)0x00000001)  /* Time stamp enable */

/* Bit definition for Ethernet PTP Sub-Second Increment Register */
#define ETH_PTPSSIR_STSSI    ((uint32_t)0x000000FF)  /* System time Sub-second increment value */

/* Bit definition for Ethernet PTP Time Stamp High Register */
#define ETH_PTPTSHR_STS      ((uint32_t)0xFFFFFFFF)  /* System Time second */

/* Bit definition for Ethernet PTP Time Stamp Low Register */
#define ETH_PTPTSLR_STPNS    ((uint32_t)0x80000000)  /* System Time Positive or negative time */
#define ETH_PTPTSLR_STSS     ((uint32_t)0x7FFFFFFF)  /* System Time sub-seconds */

/* Bit definition for Ethernet PTP Time Stamp High Update Register */
#define ETH_PTPTSHUR_TSUS    ((uint32_t)0xFFFFFFFF)  /* Time stamp update seconds */

/* Bit definition for Ethernet PTP Time Stamp Low Update Register */
#define ETH_PTPTSLUR_TSUPNS  ((uint32_t)0x80000000)  /* Time stamp update Positive or negative time */
#define ETH_PTPTSLUR_TSUSS   ((uint32_t)0x7FFFFFFF)  /* Time stamp update sub-seconds */

/* Bit definition for Ethernet PTP Time Stamp Addend Register */
#define ETH_PTPTSAR_TSA      ((uint32_t)0xFFFFFFFF)  /* Time stamp addend */

/* Bit definition for Ethernet PTP Target Time High Register */
#define ETH_PTPTTHR_TTSH     ((uint32_t)0xFFFFFFFF)  /* Target time stamp high */

/* Bit definition for Ethernet PTP Target Time Low Register */
#define ETH_PTPTTLR_TTSL     ((uint32_t)0xFFFFFFFF)  /* Target time stamp low */

/* Bit definition for Ethernet PTP Time Stamp Status Register */
#define ETH_PTPTSSR_TSTTR    ((uint32_t)0x00000020)  /* Time stamp target time reached */
#define ETH_PTPTSSR_TSSO     ((uint32_t)0x00000010)  /* Time stamp seconds overflow */

/******************************************************************************/
/*                 Ethernet DMA Registers bits definition                     */
/******************************************************************************/

/* Bit definition for Ethernet DMA Bus Mode Register */
#define ETH_DMABMR_AAB       ((uint32_t)0x02000000)  /* Address-Aligned beats */
#define ETH_DMABMR_FPM        ((uint32_t)0x01000000)  /* 4xPBL mode */
#define ETH_DMABMR_USP       ((uint32_t)0x00800000)  /* Use separate PBL */
#define ETH_DMABMR_RDP       ((uint32_t)0x007E0000)  /* RxDMA PBL */
  #define ETH_DMABMR_RDP_1Beat    ((uint32_t)0x00020000)  /* maximum number of beats to be transferred in one RxDMA transaction is 1 */
  #define ETH_DMABMR_RDP_2Beat    ((uint32_t)0x00040000)  /* maximum number of beats to be transferred in one RxDMA transaction is 2 */
  #define ETH_DMABMR_RDP_4Beat    ((uint32_t)0x00080000)  /* maximum number of beats to be transferred in one RxDMA transaction is 4 */
  #define ETH_DMABMR_RDP_8Beat    ((uint32_t)0x00100000)  /* maximum number of beats to be transferred in one RxDMA transaction is 8 */
  #define ETH_DMABMR_RDP_16Beat   ((uint32_t)0x00200000)  /* maximum number of beats to be transferred in one RxDMA transaction is 16 */
  #define ETH_DMABMR_RDP_32Beat   ((uint32_t)0x00400000)  /* maximum number of beats to be transferred in one RxDMA transaction is 32 */
  #define ETH_DMABMR_RDP_4xPBL_4Beat   ((uint32_t)0x01020000)  /* maximum number of beats to be transferred in one RxDMA transaction is 4 */
  #define ETH_DMABMR_RDP_4xPBL_8Beat   ((uint32_t)0x01040000)  /* maximum number of beats to be transferred in one RxDMA transaction is 8 */
  #define ETH_DMABMR_RDP_4xPBL_16Beat  ((uint32_t)0x01080000)  /* maximum number of beats to be transferred in one RxDMA transaction is 16 */
  #define ETH_DMABMR_RDP_4xPBL_32Beat  ((uint32_t)0x01100000)  /* maximum number of beats to be transferred in one RxDMA transaction is 32 */
  #define ETH_DMABMR_RDP_4xPBL_64Beat  ((uint32_t)0x01200000)  /* maximum number of beats to be transferred in one RxDMA transaction is 64 */
  #define ETH_DMABMR_RDP_4xPBL_128Beat ((uint32_t)0x01400000)  /* maximum number of beats to be transferred in one RxDMA transaction is 128 */
#define ETH_DMABMR_FB        ((uint32_t)0x00010000)  /* Fixed Burst */
#define ETH_DMABMR_RTPR      ((uint32_t)0x0000C000)  /* Rx Tx priority ratio */
  #define ETH_DMABMR_RTPR_1_1     ((uint32_t)0x00000000)  /* Rx Tx priority ratio */
  #define ETH_DMABMR_RTPR_2_1     ((uint32_t)0x00004000)  /* Rx Tx priority ratio */
  #define ETH_DMABMR_RTPR_3_1     ((uint32_t)0x00008000)  /* Rx Tx priority ratio */
  #define ETH_DMABMR_RTPR_4_1     ((uint32_t)0x0000C000)  /* Rx Tx priority ratio */
#define ETH_DMABMR_PBL    ((uint32_t)0x00003F00)  /* Programmable burst length */
  #define ETH_DMABMR_PBL_1Beat    ((uint32_t)0x00000100)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 1 */
  #define ETH_DMABMR_PBL_2Beat    ((uint32_t)0x00000200)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 2 */
  #define ETH_DMABMR_PBL_4Beat    ((uint32_t)0x00000400)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
  #define ETH_DMABMR_PBL_8Beat    ((uint32_t)0x00000800)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
  #define ETH_DMABMR_PBL_16Beat   ((uint32_t)0x00001000)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
  #define ETH_DMABMR_PBL_32Beat   ((uint32_t)0x00002000)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
  #define ETH_DMABMR_PBL_4xPBL_4Beat   ((uint32_t)0x01000100)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
  #define ETH_DMABMR_PBL_4xPBL_8Beat   ((uint32_t)0x01000200)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
  #define ETH_DMABMR_PBL_4xPBL_16Beat  ((uint32_t)0x01000400)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
  #define ETH_DMABMR_PBL_4xPBL_32Beat  ((uint32_t)0x01000800)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
  #define ETH_DMABMR_PBL_4xPBL_64Beat  ((uint32_t)0x01001000)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 64 */
  #define ETH_DMABMR_PBL_4xPBL_128Beat ((uint32_t)0x01002000)  /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 128 */
#define ETH_DMABMR_EDE       ((uint32_t)0x00000080)  /* Enhanced Descriptor Enable */
#define ETH_DMABMR_DSL       ((uint32_t)0x0000007C)  /* Descriptor Skip Length */
#define ETH_DMABMR_DA        ((uint32_t)0x00000002)  /* DMA arbitration scheme */
#define ETH_DMABMR_SR        ((uint32_t)0x00000001)  /* Software reset */

/* Bit definition for Ethernet DMA Transmit Poll Demand Register */
#define ETH_DMATPDR_TPD      ((uint32_t)0xFFFFFFFF)  /* Transmit poll demand */

/* Bit definition for Ethernet DMA Receive Poll Demand Register */
#define ETH_DMARPDR_RPD      ((uint32_t)0xFFFFFFFF)  /* Receive poll demand  */

/* Bit definition for Ethernet DMA Receive Descriptor List Address Register */
#define ETH_DMARDLAR_SRL     ((uint32_t)0xFFFFFFFF)  /* Start of receive list */

/* Bit definition for Ethernet DMA Transmit Descriptor List Address Register */
#define ETH_DMATDLAR_STL     ((uint32_t)0xFFFFFFFF)  /* Start of transmit list */

/* Bit definition for Ethernet DMA Status Register */
#define ETH_DMASR_TSTS       ((uint32_t)0x20000000)  /* Time-stamp trigger status */
#define ETH_DMASR_PMTS       ((uint32_t)0x10000000)  /* PMT status */
#define ETH_DMASR_MMCS       ((uint32_t)0x08000000)  /* MMC status */
#define ETH_DMASR_EBS        ((uint32_t)0x03800000)  /* Error bits status */
  /* combination with EBS[2:0] for GetFlagStatus function */
  #define ETH_DMASR_EBS_DescAccess      ((uint32_t)0x02000000)  /* Error bits 0-data buffer, 1-desc. access */
  #define ETH_DMASR_EBS_ReadTransf      ((uint32_t)0x01000000)  /* Error bits 0-write trnsf, 1-read transfr */
  #define ETH_DMASR_EBS_DataTransfTx    ((uint32_t)0x00800000)  /* Error bits 0-Rx DMA, 1-Tx DMA */
#define ETH_DMASR_TPS         ((uint32_t)0x00700000)  /* Transmit process state */
  #define ETH_DMASR_TPS_Stopped         ((uint32_t)0x00000000)  /* Stopped - Reset or Stop Tx Command issued  */
  #define ETH_DMASR_TPS_Fetching        ((uint32_t)0x00100000)  /* Running - fetching the Tx descriptor */
  #define ETH_DMASR_TPS_Waiting         ((uint32_t)0x00200000)  /* Running - waiting for status */
  #define ETH_DMASR_TPS_Reading         ((uint32_t)0x00300000)  /* Running - reading the data from host memory */
  #define ETH_DMASR_TPS_Suspended       ((uint32_t)0x00600000)  /* Suspended - Tx Descriptor unavailabe */
  #define ETH_DMASR_TPS_Closing         ((uint32_t)0x00700000)  /* Running - closing Rx descriptor */
#define ETH_DMASR_RPS         ((uint32_t)0x000E0000)  /* Receive process state */
  #define ETH_DMASR_RPS_Stopped         ((uint32_t)0x00000000)  /* Stopped - Reset or Stop Rx Command issued */
  #define ETH_DMASR_RPS_Fetching        ((uint32_t)0x00020000)  /* Running - fetching the Rx descriptor */
  #define ETH_DMASR_RPS_Waiting         ((uint32_t)0x00060000)  /* Running - waiting for packet */
  #define ETH_DMASR_RPS_Suspended       ((uint32_t)0x00080000)  /* Suspended - Rx Descriptor unavailable */
  #define ETH_DMASR_RPS_Closing         ((uint32_t)0x000A0000)  /* Running - closing descriptor */
  #define ETH_DMASR_RPS_Queuing         ((uint32_t)0x000E0000)  /* Running - queuing the recieve frame into host memory */
#define ETH_DMASR_NIS        ((uint32_t)0x00010000)  /* Normal interrupt summary */
#define ETH_DMASR_AIS        ((uint32_t)0x00008000)  /* Abnormal interrupt summary */
#define ETH_DMASR_ERS        ((uint32_t)0x00004000)  /* Early receive status */
#define ETH_DMASR_FBES       ((uint32_t)0x00002000)  /* Fatal bus error status */
#define ETH_DMASR_ETS        ((uint32_t)0x00000400)  /* Early transmit status */
#define ETH_DMASR_RWTS       ((uint32_t)0x00000200)  /* Receive watchdog timeout status */
#define ETH_DMASR_RPSS       ((uint32_t)0x00000100)  /* Receive process stopped status */
#define ETH_DMASR_RBUS       ((uint32_t)0x00000080)  /* Receive buffer unavailable status */
#define ETH_DMASR_RS         ((uint32_t)0x00000040)  /* Receive status */
#define ETH_DMASR_TUS        ((uint32_t)0x00000020)  /* Transmit underflow status */
#define ETH_DMASR_ROS        ((uint32_t)0x00000010)  /* Receive overflow status */
#define ETH_DMASR_TJTS       ((uint32_t)0x00000008)  /* Transmit jabber timeout status */
#define ETH_DMASR_TBUS       ((uint32_t)0x00000004)  /* Transmit buffer unavailable status */
#define ETH_DMASR_TPSS       ((uint32_t)0x00000002)  /* Transmit process stopped status */
#define ETH_DMASR_TS         ((uint32_t)0x00000001)  /* Transmit status */

/* Bit definition for Ethernet DMA Operation Mode Register */
#define ETH_DMAOMR_DTCEFD    ((uint32_t)0x04000000)  /* Disable Dropping of TCP/IP checksum error frames */
#define ETH_DMAOMR_RSF       ((uint32_t)0x02000000)  /* Receive store and forward */
#define ETH_DMAOMR_DFRF      ((uint32_t)0x01000000)  /* Disable flushing of received frames */
#define ETH_DMAOMR_TSF       ((uint32_t)0x00200000)  /* Transmit store and forward */
#define ETH_DMAOMR_FTF       ((uint32_t)0x00100000)  /* Flush transmit FIFO */
#define ETH_DMAOMR_TTC       ((uint32_t)0x0001C000)  /* Transmit threshold control */
  #define ETH_DMAOMR_TTC_64Bytes       ((uint32_t)0x00000000)  /* threshold level of the MTL Transmit FIFO is 64 Bytes */
  #define ETH_DMAOMR_TTC_128Bytes      ((uint32_t)0x00004000)  /* threshold level of the MTL Transmit FIFO is 128 Bytes */
  #define ETH_DMAOMR_TTC_192Bytes      ((uint32_t)0x00008000)  /* threshold level of the MTL Transmit FIFO is 192 Bytes */
  #define ETH_DMAOMR_TTC_256Bytes      ((uint32_t)0x0000C000)  /* threshold level of the MTL Transmit FIFO is 256 Bytes */
  #define ETH_DMAOMR_TTC_40Bytes       ((uint32_t)0x00010000)  /* threshold level of the MTL Transmit FIFO is 40 Bytes */
  #define ETH_DMAOMR_TTC_32Bytes       ((uint32_t)0x00014000)  /* threshold level of the MTL Transmit FIFO is 32 Bytes */
  #define ETH_DMAOMR_TTC_24Bytes       ((uint32_t)0x00018000)  /* threshold level of the MTL Transmit FIFO is 24 Bytes */
  #define ETH_DMAOMR_TTC_16Bytes       ((uint32_t)0x0001C000)  /* threshold level of the MTL Transmit FIFO is 16 Bytes */
#define ETH_DMAOMR_ST        ((uint32_t)0x00002000)  /* Start/stop transmission command */
#define ETH_DMAOMR_FEF       ((uint32_t)0x00000080)  /* Forward error frames */
#define ETH_DMAOMR_FUGF      ((uint32_t)0x00000040)  /* Forward undersized good frames */
#define ETH_DMAOMR_RTC       ((uint32_t)0x00000018)  /* receive threshold control */
  #define ETH_DMAOMR_RTC_64Bytes       ((uint32_t)0x00000000)  /* threshold level of the MTL Receive FIFO is 64 Bytes */
  #define ETH_DMAOMR_RTC_32Bytes       ((uint32_t)0x00000008)  /* threshold level of the MTL Receive FIFO is 32 Bytes */
  #define ETH_DMAOMR_RTC_96Bytes       ((uint32_t)0x00000010)  /* threshold level of the MTL Receive FIFO is 96 Bytes */
  #define ETH_DMAOMR_RTC_128Bytes      ((uint32_t)0x00000018)  /* threshold level of the MTL Receive FIFO is 128 Bytes */
#define ETH_DMAOMR_OSF       ((uint32_t)0x00000004)  /* operate on second frame */
#define ETH_DMAOMR_SR        ((uint32_t)0x00000002)  /* Start/stop receive */

/* Bit definition for Ethernet DMA Interrupt Enable Register */
#define ETH_DMAIER_NISE      ((uint32_t)0x00010000)  /* Normal interrupt summary enable */
#define ETH_DMAIER_AISE      ((uint32_t)0x00008000)  /* Abnormal interrupt summary enable */
#define ETH_DMAIER_ERIE      ((uint32_t)0x00004000)  /* Early receive interrupt enable */
#define ETH_DMAIER_FBEIE     ((uint32_t)0x00002000)  /* Fatal bus error interrupt enable */
#define ETH_DMAIER_ETIE      ((uint32_t)0x00000400)  /* Early transmit interrupt enable */
#define ETH_DMAIER_RWTIE     ((uint32_t)0x00000200)  /* Receive watchdog timeout interrupt enable */
#define ETH_DMAIER_RPSIE     ((uint32_t)0x00000100)  /* Receive process stopped interrupt enable */
#define ETH_DMAIER_RBUIE     ((uint32_t)0x00000080)  /* Receive buffer unavailable interrupt enable */
#define ETH_DMAIER_RIE       ((uint32_t)0x00000040)  /* Receive interrupt enable */
#define ETH_DMAIER_TUIE      ((uint32_t)0x00000020)  /* Transmit Underflow interrupt enable */
#define ETH_DMAIER_ROIE      ((uint32_t)0x00000010)  /* Receive Overflow interrupt enable */
#define ETH_DMAIER_TJTIE     ((uint32_t)0x00000008)  /* Transmit jabber timeout interrupt enable */
#define ETH_DMAIER_TBUIE     ((uint32_t)0x00000004)  /* Transmit buffer unavailable interrupt enable */
#define ETH_DMAIER_TPSIE     ((uint32_t)0x00000002)  /* Transmit process stopped interrupt enable */
#define ETH_DMAIER_TIE       ((uint32_t)0x00000001)  /* Transmit interrupt enable */

/* Bit definition for Ethernet DMA Missed Frame and Buffer Overflow Counter Register */
#define ETH_DMAMFBOCR_OFOC   ((uint32_t)0x10000000)  /* Overflow bit for FIFO overflow counter */
#define ETH_DMAMFBOCR_MFA    ((uint32_t)0x0FFE0000)  /* Number of frames missed by the application */
#define ETH_DMAMFBOCR_OMFC   ((uint32_t)0x00010000)  /* Overflow bit for missed frame counter */
#define ETH_DMAMFBOCR_MFC    ((uint32_t)0x0000FFFF)  /* Number of frames missed by the controller */

/* Bit definition for Ethernet DMA Current Host Transmit Descriptor Register */
#define ETH_DMACHTDR_HTDAP   ((uint32_t)0xFFFFFFFF)  /* Host transmit descriptor address pointer */

/* Bit definition for Ethernet DMA Current Host Receive Descriptor Register */
#define ETH_DMACHRDR_HRDAP   ((uint32_t)0xFFFFFFFF)  /* Host receive descriptor address pointer */

/* Bit definition for Ethernet DMA Current Host Transmit Buffer Address Register */
#define ETH_DMACHTBAR_HTBAP  ((uint32_t)0xFFFFFFFF)  /* Host transmit buffer address pointer */

/* Bit definition for Ethernet DMA Current Host Receive Buffer Address Register */
#define ETH_DMACHRBAR_HRBAP  ((uint32_t)0xFFFFFFFF)  /* Host receive buffer address pointer */

/******************************************************************************/
/*                                                                            */
/*                                       USB_OTG                              */
/*                                                                            */
/******************************************************************************/
/********************  Bit definition for USB_OTG_GOTGCTL register  ********************/
#define USB_OTG_GOTGCTL_SRQSCS                  ((uint32_t)0x00000001)         /*!< Session request success */
#define USB_OTG_GOTGCTL_SRQ                     ((uint32_t)0x00000002)         /*!< Session request */
#define USB_OTG_GOTGCTL_VBVALOEN                ((uint32_t)0x00000004)         /*!< VBUS valid override enable */
#define USB_OTG_GOTGCTL_VBVALOVAL               ((uint32_t)0x00000008)         /*!< VBUS valid override value */
#define USB_OTG_GOTGCTL_AVALOEN                 ((uint32_t)0x00000010)         /*!< A-peripheral session valid override enable */
#define USB_OTG_GOTGCTL_AVALOVAL                ((uint32_t)0x00000020)         /*!< A-peripheral session valid override value */
#define USB_OTG_GOTGCTL_BVALOEN                 ((uint32_t)0x00000040)         /*!< B-peripheral session valid override enable */
#define USB_OTG_GOTGCTL_BVALOVAL                ((uint32_t)0x00000080)         /*!< B-peripheral session valid override value  */
#define USB_OTG_GOTGCTL_HNGSCS                  ((uint32_t)0x00000100)         /*!< Host set HNP enable */
#define USB_OTG_GOTGCTL_HNPRQ                   ((uint32_t)0x00000200)         /*!< HNP request */
#define USB_OTG_GOTGCTL_HSHNPEN                 ((uint32_t)0x00000400)         /*!< Host set HNP enable */
#define USB_OTG_GOTGCTL_DHNPEN                  ((uint32_t)0x00000800)         /*!< Device HNP enabled */
#define USB_OTG_GOTGCTL_EHEN                    ((uint32_t)0x00001000)         /*!< Embedded host enable */
#define USB_OTG_GOTGCTL_CIDSTS                  ((uint32_t)0x00010000)         /*!< Connector ID status */
#define USB_OTG_GOTGCTL_DBCT                    ((uint32_t)0x00020000)         /*!< Long/short debounce time */
#define USB_OTG_GOTGCTL_ASVLD                   ((uint32_t)0x00040000)         /*!< A-session valid  */
#define USB_OTG_GOTGCTL_BSESVLD                 ((uint32_t)0x00080000)         /*!< B-session valid */
#define USB_OTG_GOTGCTL_OTGVER                  ((uint32_t)0x00100000)         /*!< OTG version  */

/********************  Bit definition for USB_OTG_HCFG register  ********************/
#define USB_OTG_HCFG_FSLSPCS                 ((uint32_t)0x00000003)            /*!< FS/LS PHY clock select  */
#define USB_OTG_HCFG_FSLSPCS_0               ((uint32_t)0x00000001)            /*!<Bit 0 */
#define USB_OTG_HCFG_FSLSPCS_1               ((uint32_t)0x00000002)            /*!<Bit 1 */
#define USB_OTG_HCFG_FSLSS                   ((uint32_t)0x00000004)            /*!< FS- and LS-only support */

/********************  Bit definition for USB_OTG_DCFG register  ********************/
#define USB_OTG_DCFG_DSPD                    ((uint32_t)0x00000003)            /*!< Device speed */
#define USB_OTG_DCFG_DSPD_0                  ((uint32_t)0x00000001)            /*!<Bit 0 */
#define USB_OTG_DCFG_DSPD_1                  ((uint32_t)0x00000002)            /*!<Bit 1 */
#define USB_OTG_DCFG_NZLSOHSK                ((uint32_t)0x00000004)            /*!< Nonzero-length status OUT handshake */

#define USB_OTG_DCFG_DAD                     ((uint32_t)0x000007F0)            /*!< Device address */
#define USB_OTG_DCFG_DAD_0                   ((uint32_t)0x00000010)            /*!<Bit 0 */
#define USB_OTG_DCFG_DAD_1                   ((uint32_t)0x00000020)            /*!<Bit 1 */
#define USB_OTG_DCFG_DAD_2                   ((uint32_t)0x00000040)            /*!<Bit 2 */
#define USB_OTG_DCFG_DAD_3                   ((uint32_t)0x00000080)            /*!<Bit 3 */
#define USB_OTG_DCFG_DAD_4                   ((uint32_t)0x00000100)            /*!<Bit 4 */
#define USB_OTG_DCFG_DAD_5                   ((uint32_t)0x00000200)            /*!<Bit 5 */
#define USB_OTG_DCFG_DAD_6                   ((uint32_t)0x00000400)            /*!<Bit 6 */

#define USB_OTG_DCFG_PFIVL                   ((uint32_t)0x00001800)            /*!< Periodic (micro)frame interval */
#define USB_OTG_DCFG_PFIVL_0                 ((uint32_t)0x00000800)            /*!<Bit 0 */
#define USB_OTG_DCFG_PFIVL_1                 ((uint32_t)0x00001000)            /*!<Bit 1 */

#define USB_OTG_DCFG_PERSCHIVL               ((uint32_t)0x03000000)            /*!< Periodic scheduling interval */
#define USB_OTG_DCFG_PERSCHIVL_0             ((uint32_t)0x01000000)            /*!<Bit 0 */
#define USB_OTG_DCFG_PERSCHIVL_1             ((uint32_t)0x02000000)            /*!<Bit 1 */

/********************  Bit definition for USB_OTG_PCGCR register  ********************/
#define USB_OTG_PCGCR_STPPCLK                 ((uint32_t)0x00000001)            /*!< Stop PHY clock */
#define USB_OTG_PCGCR_GATEHCLK                ((uint32_t)0x00000002)            /*!< Gate HCLK */
#define USB_OTG_PCGCR_PHYSUSP                 ((uint32_t)0x00000010)            /*!< PHY suspended */

/********************  Bit definition for USB_OTG_GOTGINT register  ********************/
#define USB_OTG_GOTGINT_SEDET                   ((uint32_t)0x00000004)            /*!< Session end detected                   */
#define USB_OTG_GOTGINT_SRSSCHG                 ((uint32_t)0x00000100)            /*!< Session request success status change  */
#define USB_OTG_GOTGINT_HNSSCHG                 ((uint32_t)0x00000200)            /*!< Host negotiation success status change */
#define USB_OTG_GOTGINT_HNGDET                  ((uint32_t)0x00020000)            /*!< Host negotiation detected              */
#define USB_OTG_GOTGINT_ADTOCHG                 ((uint32_t)0x00040000)            /*!< A-device timeout change                */
#define USB_OTG_GOTGINT_DBCDNE                  ((uint32_t)0x00080000)            /*!< Debounce done                          */
#define USB_OTG_GOTGINT_IDCHNG                  ((uint32_t)0x00100000)            /*!< Change in ID pin input value           */

/********************  Bit definition for USB_OTG_DCTL register  ********************/
#define USB_OTG_DCTL_RWUSIG                  ((uint32_t)0x00000001)            /*!< Remote wakeup signaling */
#define USB_OTG_DCTL_SDIS                    ((uint32_t)0x00000002)            /*!< Soft disconnect         */
#define USB_OTG_DCTL_GINSTS                  ((uint32_t)0x00000004)            /*!< Global IN NAK status    */
#define USB_OTG_DCTL_GONSTS                  ((uint32_t)0x00000008)            /*!< Global OUT NAK status   */

#define USB_OTG_DCTL_TCTL                    ((uint32_t)0x00000070)            /*!< Test control */
#define USB_OTG_DCTL_TCTL_0                  ((uint32_t)0x00000010)            /*!<Bit 0 */
#define USB_OTG_DCTL_TCTL_1                  ((uint32_t)0x00000020)            /*!<Bit 1 */
#define USB_OTG_DCTL_TCTL_2                  ((uint32_t)0x00000040)            /*!<Bit 2 */
#define USB_OTG_DCTL_SGINAK                  ((uint32_t)0x00000080)            /*!< Set global IN NAK         */
#define USB_OTG_DCTL_CGINAK                  ((uint32_t)0x00000100)            /*!< Clear global IN NAK       */
#define USB_OTG_DCTL_SGONAK                  ((uint32_t)0x00000200)            /*!< Set global OUT NAK        */
#define USB_OTG_DCTL_CGONAK                  ((uint32_t)0x00000400)            /*!< Clear global OUT NAK      */
#define USB_OTG_DCTL_POPRGDNE                ((uint32_t)0x00000800)            /*!< Power-on programming done */

/********************  Bit definition for USB_OTG_HFIR register  ********************/
#define USB_OTG_HFIR_FRIVL                   ((uint32_t)0x0000FFFF)            /*!< Frame interval */

/********************  Bit definition for USB_OTG_HFNUM register  ********************/
#define USB_OTG_HFNUM_FRNUM                   ((uint32_t)0x0000FFFF)            /*!< Frame number         */
#define USB_OTG_HFNUM_FTREM                   ((uint32_t)0xFFFF0000)            /*!< Frame time remaining */

/********************  Bit definition for USB_OTG_DSTS register  ********************/
#define USB_OTG_DSTS_SUSPSTS                 ((uint32_t)0x00000001)            /*!< Suspend status   */

#define USB_OTG_DSTS_ENUMSPD                 ((uint32_t)0x00000006)            /*!< Enumerated speed */
#define USB_OTG_DSTS_ENUMSPD_0               ((uint32_t)0x00000002)            /*!<Bit 0 */
#define USB_OTG_DSTS_ENUMSPD_1               ((uint32_t)0x00000004)            /*!<Bit 1 */
#define USB_OTG_DSTS_EERR                    ((uint32_t)0x00000008)            /*!< Erratic error     */
#define USB_OTG_DSTS_FNSOF                   ((uint32_t)0x003FFF00)            /*!< Frame number of the received SOF */

/********************  Bit definition for USB_OTG_GAHBCFG register  ********************/
#define USB_OTG_GAHBCFG_GINT                    ((uint32_t)0x00000001)            /*!< Global interrupt mask */
#define USB_OTG_GAHBCFG_HBSTLEN                 ((uint32_t)0x0000001E)            /*!< Burst length/type */
#define USB_OTG_GAHBCFG_HBSTLEN_0               ((uint32_t)0x00000002)            /*!<Bit 0 */
#define USB_OTG_GAHBCFG_HBSTLEN_1               ((uint32_t)0x00000004)            /*!<Bit 1 */
#define USB_OTG_GAHBCFG_HBSTLEN_2               ((uint32_t)0x00000008)            /*!<Bit 2 */
#define USB_OTG_GAHBCFG_HBSTLEN_3               ((uint32_t)0x00000010)            /*!<Bit 3 */
#define USB_OTG_GAHBCFG_DMAEN                   ((uint32_t)0x00000020)            /*!< DMA enable */
#define USB_OTG_GAHBCFG_TXFELVL                 ((uint32_t)0x00000080)            /*!< TxFIFO empty level */
#define USB_OTG_GAHBCFG_PTXFELVL                ((uint32_t)0x00000100)            /*!< Periodic TxFIFO empty level */

/********************  Bit definition for USB_OTG_GUSBCFG register  ********************/
#define USB_OTG_GUSBCFG_TOCAL                   ((uint32_t)0x00000007)            /*!< FS timeout calibration */
#define USB_OTG_GUSBCFG_TOCAL_0                 ((uint32_t)0x00000001)            /*!<Bit 0 */
#define USB_OTG_GUSBCFG_TOCAL_1                 ((uint32_t)0x00000002)            /*!<Bit 1 */
#define USB_OTG_GUSBCFG_TOCAL_2                 ((uint32_t)0x00000004)            /*!<Bit 2 */
#define USB_OTG_GUSBCFG_PHYSEL                  ((uint32_t)0x00000040)            /*!< USB 2.0 high-speed ULPI PHY or USB 1.1 full-speed serial transceiver select */
#define USB_OTG_GUSBCFG_SRPCAP                  ((uint32_t)0x00000100)            /*!< SRP-capable */
#define USB_OTG_GUSBCFG_HNPCAP                  ((uint32_t)0x00000200)            /*!< HNP-capable */
#define USB_OTG_GUSBCFG_TRDT                    ((uint32_t)0x00003C00)            /*!< USB turnaround time */
#define USB_OTG_GUSBCFG_TRDT_0                  ((uint32_t)0x00000400)            /*!<Bit 0 */
#define USB_OTG_GUSBCFG_TRDT_1                  ((uint32_t)0x00000800)            /*!<Bit 1 */
#define USB_OTG_GUSBCFG_TRDT_2                  ((uint32_t)0x00001000)            /*!<Bit 2 */
#define USB_OTG_GUSBCFG_TRDT_3                  ((uint32_t)0x00002000)            /*!<Bit 3 */
#define USB_OTG_GUSBCFG_PHYLPCS                 ((uint32_t)0x00008000)            /*!< PHY Low-power clock select */
#define USB_OTG_GUSBCFG_ULPIFSLS                ((uint32_t)0x00020000)            /*!< ULPI FS/LS select               */
#define USB_OTG_GUSBCFG_ULPIAR                  ((uint32_t)0x00040000)            /*!< ULPI Auto-resume                */
#define USB_OTG_GUSBCFG_ULPICSM                 ((uint32_t)0x00080000)            /*!< ULPI Clock SuspendM             */
#define USB_OTG_GUSBCFG_ULPIEVBUSD              ((uint32_t)0x00100000)            /*!< ULPI External VBUS Drive        */
#define USB_OTG_GUSBCFG_ULPIEVBUSI              ((uint32_t)0x00200000)            /*!< ULPI external VBUS indicator    */
#define USB_OTG_GUSBCFG_TSDPS                   ((uint32_t)0x00400000)            /*!< TermSel DLine pulsing selection */
#define USB_OTG_GUSBCFG_PCCI                    ((uint32_t)0x00800000)            /*!< Indicator complement            */
#define USB_OTG_GUSBCFG_PTCI                    ((uint32_t)0x01000000)            /*!< Indicator pass through          */
#define USB_OTG_GUSBCFG_ULPIIPD                 ((uint32_t)0x02000000)            /*!< ULPI interface protect disable  */
#define USB_OTG_GUSBCFG_FHMOD                   ((uint32_t)0x20000000)            /*!< Forced host mode                */
#define USB_OTG_GUSBCFG_FDMOD                   ((uint32_t)0x40000000)            /*!< Forced peripheral mode          */
#define USB_OTG_GUSBCFG_CTXPKT                  ((uint32_t)0x80000000)            /*!< Corrupt Tx packet               */

/********************  Bit definition for USB_OTG_GRSTCTL register  ********************/
#define USB_OTG_GRSTCTL_CSRST                   ((uint32_t)0x00000001)            /*!< Core soft reset          */
#define USB_OTG_GRSTCTL_HSRST                   ((uint32_t)0x00000002)            /*!< HCLK soft reset          */
#define USB_OTG_GRSTCTL_FCRST                   ((uint32_t)0x00000004)            /*!< Host frame counter reset */
#define USB_OTG_GRSTCTL_RXFFLSH                 ((uint32_t)0x00000010)            /*!< RxFIFO flush             */
#define USB_OTG_GRSTCTL_TXFFLSH                 ((uint32_t)0x00000020)            /*!< TxFIFO flush             */
#define USB_OTG_GRSTCTL_TXFNUM                  ((uint32_t)0x000007C0)            /*!< TxFIFO number */
#define USB_OTG_GRSTCTL_TXFNUM_0                ((uint32_t)0x00000040)            /*!<Bit 0 */
#define USB_OTG_GRSTCTL_TXFNUM_1                ((uint32_t)0x00000080)            /*!<Bit 1 */
#define USB_OTG_GRSTCTL_TXFNUM_2                ((uint32_t)0x00000100)            /*!<Bit 2 */
#define USB_OTG_GRSTCTL_TXFNUM_3                ((uint32_t)0x00000200)            /*!<Bit 3 */
#define USB_OTG_GRSTCTL_TXFNUM_4                ((uint32_t)0x00000400)            /*!<Bit 4 */
#define USB_OTG_GRSTCTL_DMAREQ                  ((uint32_t)0x40000000)            /*!< DMA request signal */
#define USB_OTG_GRSTCTL_AHBIDL                  ((uint32_t)0x80000000)            /*!< AHB master idle */

/********************  Bit definition for USB_OTG_DIEPMSK register  ********************/
#define USB_OTG_DIEPMSK_XFRCM                   ((uint32_t)0x00000001)            /*!< Transfer completed interrupt mask                 */
#define USB_OTG_DIEPMSK_EPDM                    ((uint32_t)0x00000002)            /*!< Endpoint disabled interrupt mask                  */
#define USB_OTG_DIEPMSK_TOM                     ((uint32_t)0x00000008)            /*!< Timeout condition mask (nonisochronous endpoints) */
#define USB_OTG_DIEPMSK_ITTXFEMSK               ((uint32_t)0x00000010)            /*!< IN token received when TxFIFO empty mask          */
#define USB_OTG_DIEPMSK_INEPNMM                 ((uint32_t)0x00000020)            /*!< IN token received with EP mismatch mask           */
#define USB_OTG_DIEPMSK_INEPNEM                 ((uint32_t)0x00000040)            /*!< IN endpoint NAK effective mask                    */
#define USB_OTG_DIEPMSK_TXFURM                  ((uint32_t)0x00000100)            /*!< FIFO underrun mask                                */
#define USB_OTG_DIEPMSK_BIM                     ((uint32_t)0x00000200)            /*!< BNA interrupt mask                                */

/********************  Bit definition for USB_OTG_HPTXSTS register  ********************/
#define USB_OTG_HPTXSTS_PTXFSAVL                ((uint32_t)0x0000FFFF)            /*!< Periodic transmit data FIFO space available     */
#define USB_OTG_HPTXSTS_PTXQSAV                 ((uint32_t)0x00FF0000)            /*!< Periodic transmit request queue space available */
#define USB_OTG_HPTXSTS_PTXQSAV_0               ((uint32_t)0x00010000)            /*!<Bit 0 */
#define USB_OTG_HPTXSTS_PTXQSAV_1               ((uint32_t)0x00020000)            /*!<Bit 1 */
#define USB_OTG_HPTXSTS_PTXQSAV_2               ((uint32_t)0x00040000)            /*!<Bit 2 */
#define USB_OTG_HPTXSTS_PTXQSAV_3               ((uint32_t)0x00080000)            /*!<Bit 3 */
#define USB_OTG_HPTXSTS_PTXQSAV_4               ((uint32_t)0x00100000)            /*!<Bit 4 */
#define USB_OTG_HPTXSTS_PTXQSAV_5               ((uint32_t)0x00200000)            /*!<Bit 5 */
#define USB_OTG_HPTXSTS_PTXQSAV_6               ((uint32_t)0x00400000)            /*!<Bit 6 */
#define USB_OTG_HPTXSTS_PTXQSAV_7               ((uint32_t)0x00800000)            /*!<Bit 7 */

#define USB_OTG_HPTXSTS_PTXQTOP                 ((uint32_t)0xFF000000)            /*!< Top of the periodic transmit request queue */
#define USB_OTG_HPTXSTS_PTXQTOP_0               ((uint32_t)0x01000000)            /*!<Bit 0 */
#define USB_OTG_HPTXSTS_PTXQTOP_1               ((uint32_t)0x02000000)            /*!<Bit 1 */
#define USB_OTG_HPTXSTS_PTXQTOP_2               ((uint32_t)0x04000000)            /*!<Bit 2 */
#define USB_OTG_HPTXSTS_PTXQTOP_3               ((uint32_t)0x08000000)            /*!<Bit 3 */
#define USB_OTG_HPTXSTS_PTXQTOP_4               ((uint32_t)0x10000000)            /*!<Bit 4 */
#define USB_OTG_HPTXSTS_PTXQTOP_5               ((uint32_t)0x20000000)            /*!<Bit 5 */
#define USB_OTG_HPTXSTS_PTXQTOP_6               ((uint32_t)0x40000000)            /*!<Bit 6 */
#define USB_OTG_HPTXSTS_PTXQTOP_7               ((uint32_t)0x80000000)            /*!<Bit 7 */

/********************  Bit definition for USB_OTG_HAINT register  ********************/
#define USB_OTG_HAINT_HAINT                   ((uint32_t)0x0000FFFF)            /*!< Channel interrupts */

/********************  Bit definition for USB_OTG_DOEPMSK register  ********************/
#define USB_OTG_DOEPMSK_XFRCM                   ((uint32_t)0x00000001)            /*!< Transfer completed interrupt mask */
#define USB_OTG_DOEPMSK_EPDM                    ((uint32_t)0x00000002)            /*!< Endpoint disabled interrupt mask               */
#define USB_OTG_DOEPMSK_STUPM                   ((uint32_t)0x00000008)            /*!< SETUP phase done mask                          */
#define USB_OTG_DOEPMSK_OTEPDM                  ((uint32_t)0x00000010)            /*!< OUT token received when endpoint disabled mask */
#define USB_OTG_DOEPMSK_B2BSTUP                 ((uint32_t)0x00000040)            /*!< Back-to-back SETUP packets received mask       */
#define USB_OTG_DOEPMSK_OPEM                    ((uint32_t)0x00000100)            /*!< OUT packet error mask                          */
#define USB_OTG_DOEPMSK_BOIM                    ((uint32_t)0x00000200)            /*!< BNA interrupt mask                             */

/********************  Bit definition for USB_OTG_GINTSTS register  ********************/
#define USB_OTG_GINTSTS_CMOD                    ((uint32_t)0x00000001)            /*!< Current mode of operation                      */
#define USB_OTG_GINTSTS_MMIS                    ((uint32_t)0x00000002)            /*!< Mode mismatch interrupt                        */
#define USB_OTG_GINTSTS_OTGINT                  ((uint32_t)0x00000004)            /*!< OTG interrupt                                  */
#define USB_OTG_GINTSTS_SOF                     ((uint32_t)0x00000008)            /*!< Start of frame                                 */
#define USB_OTG_GINTSTS_RXFLVL                  ((uint32_t)0x00000010)            /*!< RxFIFO nonempty                                */
#define USB_OTG_GINTSTS_NPTXFE                  ((uint32_t)0x00000020)            /*!< Nonperiodic TxFIFO empty                       */
#define USB_OTG_GINTSTS_GINAKEFF                ((uint32_t)0x00000040)            /*!< Global IN nonperiodic NAK effective            */
#define USB_OTG_GINTSTS_BOUTNAKEFF              ((uint32_t)0x00000080)            /*!< Global OUT NAK effective                       */
#define USB_OTG_GINTSTS_ESUSP                   ((uint32_t)0x00000400)            /*!< Early suspend                                  */
#define USB_OTG_GINTSTS_USBSUSP                 ((uint32_t)0x00000800)            /*!< USB suspend                                    */
#define USB_OTG_GINTSTS_USBRST                  ((uint32_t)0x00001000)            /*!< USB reset                                      */
#define USB_OTG_GINTSTS_ENUMDNE                 ((uint32_t)0x00002000)            /*!< Enumeration done                               */
#define USB_OTG_GINTSTS_ISOODRP                 ((uint32_t)0x00004000)            /*!< Isochronous OUT packet dropped interrupt       */
#define USB_OTG_GINTSTS_EOPF                    ((uint32_t)0x00008000)            /*!< End of periodic frame interrupt                */
#define USB_OTG_GINTSTS_IEPINT                  ((uint32_t)0x00040000)            /*!< IN endpoint interrupt                          */
#define USB_OTG_GINTSTS_OEPINT                  ((uint32_t)0x00080000)            /*!< OUT endpoint interrupt                         */
#define USB_OTG_GINTSTS_IISOIXFR                ((uint32_t)0x00100000)            /*!< Incomplete isochronous IN transfer             */
#define USB_OTG_GINTSTS_PXFR_INCOMPISOOUT       ((uint32_t)0x00200000)            /*!< Incomplete periodic transfer                   */
#define USB_OTG_GINTSTS_DATAFSUSP               ((uint32_t)0x00400000)            /*!< Data fetch suspended                           */
#define USB_OTG_GINTSTS_RSTDET                  ((uint32_t)0x00800000)            /*!< Reset detected interrupt                       */
#define USB_OTG_GINTSTS_HPRTINT                 ((uint32_t)0x01000000)            /*!< Host port interrupt                            */
#define USB_OTG_GINTSTS_HCINT                   ((uint32_t)0x02000000)            /*!< Host channels interrupt                        */
#define USB_OTG_GINTSTS_PTXFE                   ((uint32_t)0x04000000)            /*!< Periodic TxFIFO empty                          */
#define USB_OTG_GINTSTS_LPMINT                  ((uint32_t)0x08000000)            /*!< LPM interrupt                                  */
#define USB_OTG_GINTSTS_CIDSCHG                 ((uint32_t)0x10000000)            /*!< Connector ID status change                     */
#define USB_OTG_GINTSTS_DISCINT                 ((uint32_t)0x20000000)            /*!< Disconnect detected interrupt                  */
#define USB_OTG_GINTSTS_SRQINT                  ((uint32_t)0x40000000)            /*!< Session request/new session detected interrupt */
#define USB_OTG_GINTSTS_WKUINT                  ((uint32_t)0x80000000)            /*!< Resume/remote wakeup detected interrupt        */

/********************  Bit definition for USB_OTG_GINTMSK register  ********************/
#define USB_OTG_GINTMSK_MMISM                   ((uint32_t)0x00000002)            /*!< Mode mismatch interrupt mask                        */
#define USB_OTG_GINTMSK_OTGINT                  ((uint32_t)0x00000004)            /*!< OTG interrupt mask                                  */
#define USB_OTG_GINTMSK_SOFM                    ((uint32_t)0x00000008)            /*!< Start of frame mask                                 */
#define USB_OTG_GINTMSK_RXFLVLM                 ((uint32_t)0x00000010)            /*!< Receive FIFO nonempty mask                          */
#define USB_OTG_GINTMSK_NPTXFEM                 ((uint32_t)0x00000020)            /*!< Nonperiodic TxFIFO empty mask                       */
#define USB_OTG_GINTMSK_GINAKEFFM               ((uint32_t)0x00000040)            /*!< Global nonperiodic IN NAK effective mask            */
#define USB_OTG_GINTMSK_GONAKEFFM               ((uint32_t)0x00000080)            /*!< Global OUT NAK effective mask                       */
#define USB_OTG_GINTMSK_ESUSPM                  ((uint32_t)0x00000400)            /*!< Early suspend mask                                  */
#define USB_OTG_GINTMSK_USBSUSPM                ((uint32_t)0x00000800)            /*!< USB suspend mask                                    */
#define USB_OTG_GINTMSK_USBRST                  ((uint32_t)0x00001000)            /*!< USB reset mask                                      */
#define USB_OTG_GINTMSK_ENUMDNEM                ((uint32_t)0x00002000)            /*!< Enumeration done mask                               */
#define USB_OTG_GINTMSK_ISOODRPM                ((uint32_t)0x00004000)            /*!< Isochronous OUT packet dropped interrupt mask       */
#define USB_OTG_GINTMSK_EOPFM                   ((uint32_t)0x00008000)            /*!< End of periodic frame interrupt mask                */
#define USB_OTG_GINTMSK_EPMISM                  ((uint32_t)0x00020000)            /*!< Endpoint mismatch interrupt mask                    */
#define USB_OTG_GINTMSK_IEPINT                  ((uint32_t)0x00040000)            /*!< IN endpoints interrupt mask                         */
#define USB_OTG_GINTMSK_OEPINT                  ((uint32_t)0x00080000)            /*!< OUT endpoints interrupt mask                        */
#define USB_OTG_GINTMSK_IISOIXFRM               ((uint32_t)0x00100000)            /*!< Incomplete isochronous IN transfer mask             */
#define USB_OTG_GINTMSK_PXFRM_IISOOXFRM         ((uint32_t)0x00200000)            /*!< Incomplete periodic transfer mask                   */
#define USB_OTG_GINTMSK_FSUSPM                  ((uint32_t)0x00400000)            /*!< Data fetch suspended mask                           */
#define USB_OTG_GINTMSK_RSTDEM                  ((uint32_t)0x00800000)            /*!< Reset detected interrupt mask                      */
#define USB_OTG_GINTMSK_PRTIM                   ((uint32_t)0x01000000)            /*!< Host port interrupt mask                            */
#define USB_OTG_GINTMSK_HCIM                    ((uint32_t)0x02000000)            /*!< Host channels interrupt mask                        */
#define USB_OTG_GINTMSK_PTXFEM                  ((uint32_t)0x04000000)            /*!< Periodic TxFIFO empty mask                          */
#define USB_OTG_GINTMSK_LPMINTM                 ((uint32_t)0x08000000)            /*!< LPM interrupt Mask                                  */
#define USB_OTG_GINTMSK_CIDSCHGM                ((uint32_t)0x10000000)            /*!< Connector ID status change mask                     */
#define USB_OTG_GINTMSK_DISCINT                 ((uint32_t)0x20000000)            /*!< Disconnect detected interrupt mask                  */
#define USB_OTG_GINTMSK_SRQIM                   ((uint32_t)0x40000000)            /*!< Session request/new session detected interrupt mask */
#define USB_OTG_GINTMSK_WUIM                    ((uint32_t)0x80000000)            /*!< Resume/remote wakeup detected interrupt mask        */

/********************  Bit definition for USB_OTG_DAINT register  ********************/
#define USB_OTG_DAINT_IEPINT                  ((uint32_t)0x0000FFFF)            /*!< IN endpoint interrupt bits  */
#define USB_OTG_DAINT_OEPINT                  ((uint32_t)0xFFFF0000)            /*!< OUT endpoint interrupt bits */

/********************  Bit definition for USB_OTG_HAINTMSK register  ********************/
#define USB_OTG_HAINTMSK_HAINTM                  ((uint32_t)0x0000FFFF)            /*!< Channel interrupt mask */

/********************  Bit definition for USB_OTG_GRXSTSP register  ********************/
#define USB_OTG_GRXSTSP_EPNUM                    ((uint32_t)0x0000000F)            /*!< IN EP interrupt mask bits  */
#define USB_OTG_GRXSTSP_BCNT                     ((uint32_t)0x00007FF0)            /*!< OUT EP interrupt mask bits */
#define USB_OTG_GRXSTSP_DPID                     ((uint32_t)0x00018000)            /*!< OUT EP interrupt mask bits */
#define USB_OTG_GRXSTSP_PKTSTS                   ((uint32_t)0x001E0000)            /*!< OUT EP interrupt mask bits */

/********************  Bit definition for USB_OTG_DAINTMSK register  ********************/
#define USB_OTG_DAINTMSK_IEPM                    ((uint32_t)0x0000FFFF)            /*!< IN EP interrupt mask bits */
#define USB_OTG_DAINTMSK_OEPM                    ((uint32_t)0xFFFF0000)            /*!< OUT EP interrupt mask bits */

/********************  Bit definition for OTG register  ********************/

#define USB_OTG_CHNUM                   ((uint32_t)0x0000000F)            /*!< Channel number */
#define USB_OTG_CHNUM_0                 ((uint32_t)0x00000001)            /*!<Bit 0 */
#define USB_OTG_CHNUM_1                 ((uint32_t)0x00000002)            /*!<Bit 1 */
#define USB_OTG_CHNUM_2                 ((uint32_t)0x00000004)            /*!<Bit 2 */
#define USB_OTG_CHNUM_3                 ((uint32_t)0x00000008)            /*!<Bit 3 */
#define USB_OTG_BCNT                    ((uint32_t)0x00007FF0)            /*!< Byte count */

#define USB_OTG_DPID                    ((uint32_t)0x00018000)            /*!< Data PID */
#define USB_OTG_DPID_0                  ((uint32_t)0x00008000)            /*!<Bit 0 */
#define USB_OTG_DPID_1                  ((uint32_t)0x00010000)            /*!<Bit 1 */

#define USB_OTG_PKTSTS                  ((uint32_t)0x001E0000)            /*!< Packet status */
#define USB_OTG_PKTSTS_0                ((uint32_t)0x00020000)            /*!<Bit 0 */
#define USB_OTG_PKTSTS_1                ((uint32_t)0x00040000)            /*!<Bit 1 */
#define USB_OTG_PKTSTS_2                ((uint32_t)0x00080000)            /*!<Bit 2 */
#define USB_OTG_PKTSTS_3                ((uint32_t)0x00100000)            /*!<Bit 3 */

#define USB_OTG_EPNUM                   ((uint32_t)0x0000000F)            /*!< Endpoint number */
#define USB_OTG_EPNUM_0                 ((uint32_t)0x00000001)            /*!<Bit 0 */
#define USB_OTG_EPNUM_1                 ((uint32_t)0x00000002)            /*!<Bit 1 */
#define USB_OTG_EPNUM_2                 ((uint32_t)0x00000004)            /*!<Bit 2 */
#define USB_OTG_EPNUM_3                 ((uint32_t)0x00000008)            /*!<Bit 3 */

#define USB_OTG_FRMNUM                  ((uint32_t)0x01E00000)            /*!< Frame number */
#define USB_OTG_FRMNUM_0                ((uint32_t)0x00200000)            /*!<Bit 0 */
#define USB_OTG_FRMNUM_1                ((uint32_t)0x00400000)            /*!<Bit 1 */
#define USB_OTG_FRMNUM_2                ((uint32_t)0x00800000)            /*!<Bit 2 */
#define USB_OTG_FRMNUM_3                ((uint32_t)0x01000000)            /*!<Bit 3 */

/********************  Bit definition for OTG register  ********************/

#define USB_OTG_CHNUM                   ((uint32_t)0x0000000F)            /*!< Channel number */
#define USB_OTG_CHNUM_0                 ((uint32_t)0x00000001)            /*!<Bit 0 */
#define USB_OTG_CHNUM_1                 ((uint32_t)0x00000002)            /*!<Bit 1 */
#define USB_OTG_CHNUM_2                 ((uint32_t)0x00000004)            /*!<Bit 2 */
#define USB_OTG_CHNUM_3                 ((uint32_t)0x00000008)            /*!<Bit 3 */
#define USB_OTG_BCNT                    ((uint32_t)0x00007FF0)            /*!< Byte count */

#define USB_OTG_DPID                    ((uint32_t)0x00018000)            /*!< Data PID */
#define USB_OTG_DPID_0                  ((uint32_t)0x00008000)            /*!<Bit 0 */
#define USB_OTG_DPID_1                  ((uint32_t)0x00010000)            /*!<Bit 1 */

#define USB_OTG_PKTSTS                  ((uint32_t)0x001E0000)            /*!< Packet status */
#define USB_OTG_PKTSTS_0                ((uint32_t)0x00020000)            /*!<Bit 0 */
#define USB_OTG_PKTSTS_1                ((uint32_t)0x00040000)            /*!<Bit 1 */
#define USB_OTG_PKTSTS_2                ((uint32_t)0x00080000)            /*!<Bit 2 */
#define USB_OTG_PKTSTS_3                ((uint32_t)0x00100000)            /*!<Bit 3 */

#define USB_OTG_EPNUM                   ((uint32_t)0x0000000F)            /*!< Endpoint number */
#define USB_OTG_EPNUM_0                 ((uint32_t)0x00000001)            /*!<Bit 0 */
#define USB_OTG_EPNUM_1                 ((uint32_t)0x00000002)            /*!<Bit 1 */
#define USB_OTG_EPNUM_2                 ((uint32_t)0x00000004)            /*!<Bit 2 */
#define USB_OTG_EPNUM_3                 ((uint32_t)0x00000008)            /*!<Bit 3 */

#define USB_OTG_FRMNUM                  ((uint32_t)0x01E00000)            /*!< Frame number */
#define USB_OTG_FRMNUM_0                ((uint32_t)0x00200000)            /*!<Bit 0 */
#define USB_OTG_FRMNUM_1                ((uint32_t)0x00400000)            /*!<Bit 1 */
#define USB_OTG_FRMNUM_2                ((uint32_t)0x00800000)            /*!<Bit 2 */
#define USB_OTG_FRMNUM_3                ((uint32_t)0x01000000)            /*!<Bit 3 */

/********************  Bit definition for USB_OTG_GRXFSIZ register  ********************/
#define USB_OTG_GRXFSIZ_RXFD            ((uint32_t)0x0000FFFF)            /*!< RxFIFO depth */

/********************  Bit definition for USB_OTG_DVBUSDIS register  ********************/
#define USB_OTG_DVBUSDIS_VBUSDT         ((uint32_t)0x0000FFFF)            /*!< Device VBUS discharge time */

/********************  Bit definition for OTG register  ********************/
#define USB_OTG_NPTXFSA                 ((uint32_t)0x0000FFFF)            /*!< Nonperiodic transmit RAM start address */
#define USB_OTG_NPTXFD                  ((uint32_t)0xFFFF0000)            /*!< Nonperiodic TxFIFO depth               */
#define USB_OTG_TX0FSA                  ((uint32_t)0x0000FFFF)            /*!< Endpoint 0 transmit RAM start address  */
#define USB_OTG_TX0FD                   ((uint32_t)0xFFFF0000)            /*!< Endpoint 0 TxFIFO depth                */

/********************  Bit definition for USB_OTG_DVBUSPULSE register  ********************/
#define USB_OTG_DVBUSPULSE_DVBUSP                  ((uint32_t)0x00000FFF)            /*!< Device VBUS pulsing time */

/********************  Bit definition for USB_OTG_GNPTXSTS register  ********************/
#define USB_OTG_GNPTXSTS_NPTXFSAV                ((uint32_t)0x0000FFFF)            /*!< Nonperiodic TxFIFO space available */

#define USB_OTG_GNPTXSTS_NPTQXSAV                ((uint32_t)0x00FF0000)            /*!< Nonperiodic transmit request queue space available */
#define USB_OTG_GNPTXSTS_NPTQXSAV_0              ((uint32_t)0x00010000)            /*!<Bit 0 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_1              ((uint32_t)0x00020000)            /*!<Bit 1 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_2              ((uint32_t)0x00040000)            /*!<Bit 2 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_3              ((uint32_t)0x00080000)            /*!<Bit 3 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_4              ((uint32_t)0x00100000)            /*!<Bit 4 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_5              ((uint32_t)0x00200000)            /*!<Bit 5 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_6              ((uint32_t)0x00400000)            /*!<Bit 6 */
#define USB_OTG_GNPTXSTS_NPTQXSAV_7              ((uint32_t)0x00800000)            /*!<Bit 7 */

#define USB_OTG_GNPTXSTS_NPTXQTOP                ((uint32_t)0x7F000000)            /*!< Top of the nonperiodic transmit request queue */
#define USB_OTG_GNPTXSTS_NPTXQTOP_0              ((uint32_t)0x01000000)            /*!<Bit 0 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_1              ((uint32_t)0x02000000)            /*!<Bit 1 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_2              ((uint32_t)0x04000000)            /*!<Bit 2 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_3              ((uint32_t)0x08000000)            /*!<Bit 3 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_4              ((uint32_t)0x10000000)            /*!<Bit 4 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_5              ((uint32_t)0x20000000)            /*!<Bit 5 */
#define USB_OTG_GNPTXSTS_NPTXQTOP_6              ((uint32_t)0x40000000)            /*!<Bit 6 */

/********************  Bit definition for USB_OTG_DTHRCTL register  ********************/
#define USB_OTG_DTHRCTL_NONISOTHREN             ((uint32_t)0x00000001)            /*!< Nonisochronous IN endpoints threshold enable */
#define USB_OTG_DTHRCTL_ISOTHREN                ((uint32_t)0x00000002)            /*!< ISO IN endpoint threshold enable */

#define USB_OTG_DTHRCTL_TXTHRLEN                ((uint32_t)0x000007FC)            /*!< Transmit threshold length */
#define USB_OTG_DTHRCTL_TXTHRLEN_0              ((uint32_t)0x00000004)            /*!<Bit 0 */
#define USB_OTG_DTHRCTL_TXTHRLEN_1              ((uint32_t)0x00000008)            /*!<Bit 1 */
#define USB_OTG_DTHRCTL_TXTHRLEN_2              ((uint32_t)0x00000010)            /*!<Bit 2 */
#define USB_OTG_DTHRCTL_TXTHRLEN_3              ((uint32_t)0x00000020)            /*!<Bit 3 */
#define USB_OTG_DTHRCTL_TXTHRLEN_4              ((uint32_t)0x00000040)            /*!<Bit 4 */
#define USB_OTG_DTHRCTL_TXTHRLEN_5              ((uint32_t)0x00000080)            /*!<Bit 5 */
#define USB_OTG_DTHRCTL_TXTHRLEN_6              ((uint32_t)0x00000100)            /*!<Bit 6 */
#define USB_OTG_DTHRCTL_TXTHRLEN_7              ((uint32_t)0x00000200)            /*!<Bit 7 */
#define USB_OTG_DTHRCTL_TXTHRLEN_8              ((uint32_t)0x00000400)            /*!<Bit 8 */
#define USB_OTG_DTHRCTL_RXTHREN                 ((uint32_t)0x00010000)            /*!< Receive threshold enable */

#define USB_OTG_DTHRCTL_RXTHRLEN                ((uint32_t)0x03FE0000)            /*!< Receive threshold length */
#define USB_OTG_DTHRCTL_RXTHRLEN_0              ((uint32_t)0x00020000)            /*!<Bit 0 */
#define USB_OTG_DTHRCTL_RXTHRLEN_1              ((uint32_t)0x00040000)            /*!<Bit 1 */
#define USB_OTG_DTHRCTL_RXTHRLEN_2              ((uint32_t)0x00080000)            /*!<Bit 2 */
#define USB_OTG_DTHRCTL_RXTHRLEN_3              ((uint32_t)0x00100000)            /*!<Bit 3 */
#define USB_OTG_DTHRCTL_RXTHRLEN_4              ((uint32_t)0x00200000)            /*!<Bit 4 */
#define USB_OTG_DTHRCTL_RXTHRLEN_5              ((uint32_t)0x00400000)            /*!<Bit 5 */
#define USB_OTG_DTHRCTL_RXTHRLEN_6              ((uint32_t)0x00800000)            /*!<Bit 6 */
#define USB_OTG_DTHRCTL_RXTHRLEN_7              ((uint32_t)0x01000000)            /*!<Bit 7 */
#define USB_OTG_DTHRCTL_RXTHRLEN_8              ((uint32_t)0x02000000)            /*!<Bit 8 */
#define USB_OTG_DTHRCTL_ARPEN                   ((uint32_t)0x08000000)            /*!< Arbiter parking enable */

/********************  Bit definition for USB_OTG_DIEPEMPMSK register  ********************/
#define USB_OTG_DIEPEMPMSK_INEPTXFEM               ((uint32_t)0x0000FFFF)         /*!< IN EP Tx FIFO empty interrupt mask bits */

/********************  Bit definition for USB_OTG_DEACHINT register  ********************/
#define USB_OTG_DEACHINT_IEP1INT                 ((uint32_t)0x00000002)           /*!< IN endpoint 1interrupt bit   */
#define USB_OTG_DEACHINT_OEP1INT                 ((uint32_t)0x00020000)           /*!< OUT endpoint 1 interrupt bit */

/********************  Bit definition for USB_OTG_GCCFG register  ********************/
#define USB_OTG_GCCFG_PWRDWN                 ((uint32_t)0x00010000)              /*!< Power down */
#define USB_OTG_GCCFG_VBDEN                  ((uint32_t)0x00200000)              /*!< USB VBUS Detection Enable */

/********************  Bit definition for USB_OTG_GPWRDN) register  ********************/
#define USB_OTG_GPWRDN_ADPMEN                 ((uint32_t)0x00000001)             /*!< ADP module enable */
#define USB_OTG_GPWRDN_ADPIF                  ((uint32_t)0x00800000)             /*!< ADP Interrupt flag */

/********************  Bit definition for USB_OTG_DEACHINTMSK register  ********************/
#define USB_OTG_DEACHINTMSK_IEP1INTM          ((uint32_t)0x00000002)            /*!< IN Endpoint 1 interrupt mask bit  */
#define USB_OTG_DEACHINTMSK_OEP1INTM          ((uint32_t)0x00020000)            /*!< OUT Endpoint 1 interrupt mask bit */

/********************  Bit definition for USB_OTG_CID register  ********************/
#define USB_OTG_CID_PRODUCT_ID               ((uint32_t)0xFFFFFFFF)            /*!< Product ID field */

/********************  Bit definition for USB_OTG_GLPMCFG register  ********************/
#define  USB_OTG_GLPMCFG_LPMEN               ((uint32_t)0x00000001)            /*!< LPM support enable                                     */
#define  USB_OTG_GLPMCFG_LPMACK              ((uint32_t)0x00000002)            /*!< LPM Token acknowledge enable                           */
#define  USB_OTG_GLPMCFG_BESL                ((uint32_t)0x0000003C)            /*!< BESL value received with last ACKed LPM Token          */
#define  USB_OTG_GLPMCFG_REMWAKE             ((uint32_t)0x00000040)            /*!< bRemoteWake value received with last ACKed LPM Token   */
#define  USB_OTG_GLPMCFG_L1SSEN              ((uint32_t)0x00000080)            /*!< L1 shallow sleep enable                                */
#define  USB_OTG_GLPMCFG_BESLTHRS            ((uint32_t)0x00000F00)            /*!< BESL threshold                                         */
#define  USB_OTG_GLPMCFG_L1DSEN              ((uint32_t)0x00001000)            /*!< L1 deep sleep enable                                   */
#define  USB_OTG_GLPMCFG_LPMRSP              ((uint32_t)0x00006000)            /*!< LPM response                                           */
#define  USB_OTG_GLPMCFG_SLPSTS              ((uint32_t)0x00008000)            /*!< Port sleep status                                      */
#define  USB_OTG_GLPMCFG_L1RSMOK             ((uint32_t)0x00010000)            /*!< Sleep State Resume OK                                  */
#define  USB_OTG_GLPMCFG_LPMCHIDX            ((uint32_t)0x001E0000)            /*!< LPM Channel Index                                      */
#define  USB_OTG_GLPMCFG_LPMRCNT             ((uint32_t)0x00E00000)            /*!< LPM retry count                                        */
#define  USB_OTG_GLPMCFG_SNDLPM              ((uint32_t)0x01000000)            /*!< Send LPM transaction                                   */
#define  USB_OTG_GLPMCFG_LPMRCNTSTS          ((uint32_t)0x0E000000)            /*!< LPM retry count status                                 */
#define  USB_OTG_GLPMCFG_ENBESL              ((uint32_t)0x10000000)            /*!< Enable best effort service latency                     */

/********************  Bit definition for USB_OTG_DIEPEACHMSK1 register  ********************/
#define USB_OTG_DIEPEACHMSK1_XFRCM           ((uint32_t)0x00000001)            /*!< Transfer completed interrupt mask                 */
#define USB_OTG_DIEPEACHMSK1_EPDM            ((uint32_t)0x00000002)            /*!< Endpoint disabled interrupt mask                  */
#define USB_OTG_DIEPEACHMSK1_TOM             ((uint32_t)0x00000008)            /*!< Timeout condition mask (nonisochronous endpoints) */
#define USB_OTG_DIEPEACHMSK1_ITTXFEMSK       ((uint32_t)0x00000010)            /*!< IN token received when TxFIFO empty mask          */
#define USB_OTG_DIEPEACHMSK1_INEPNMM         ((uint32_t)0x00000020)            /*!< IN token received with EP mismatch mask           */
#define USB_OTG_DIEPEACHMSK1_INEPNEM         ((uint32_t)0x00000040)            /*!< IN endpoint NAK effective mask                    */
#define USB_OTG_DIEPEACHMSK1_TXFURM          ((uint32_t)0x00000100)            /*!< FIFO underrun mask                                */
#define USB_OTG_DIEPEACHMSK1_BIM             ((uint32_t)0x00000200)            /*!< BNA interrupt mask                                */
#define USB_OTG_DIEPEACHMSK1_NAKM            ((uint32_t)0x00002000)            /*!< NAK interrupt mask                                */

/********************  Bit definition for USB_OTG_HPRT register  ********************/
#define USB_OTG_HPRT_PCSTS                   ((uint32_t)0x00000001)            /*!< Port connect status        */
#define USB_OTG_HPRT_PCDET                   ((uint32_t)0x00000002)            /*!< Port connect detected      */
#define USB_OTG_HPRT_PENA                    ((uint32_t)0x00000004)            /*!< Port enable                */
#define USB_OTG_HPRT_PENCHNG                 ((uint32_t)0x00000008)            /*!< Port enable/disable change */
#define USB_OTG_HPRT_POCA                    ((uint32_t)0x00000010)            /*!< Port overcurrent active    */
#define USB_OTG_HPRT_POCCHNG                 ((uint32_t)0x00000020)            /*!< Port overcurrent change    */
#define USB_OTG_HPRT_PRES                    ((uint32_t)0x00000040)            /*!< Port resume                */
#define USB_OTG_HPRT_PSUSP                   ((uint32_t)0x00000080)            /*!< Port suspend               */
#define USB_OTG_HPRT_PRST                    ((uint32_t)0x00000100)            /*!< Port reset                 */

#define USB_OTG_HPRT_PLSTS                   ((uint32_t)0x00000C00)            /*!< Port line status           */
#define USB_OTG_HPRT_PLSTS_0                 ((uint32_t)0x00000400)            /*!<Bit 0 */
#define USB_OTG_HPRT_PLSTS_1                 ((uint32_t)0x00000800)            /*!<Bit 1 */
#define USB_OTG_HPRT_PPWR                    ((uint32_t)0x00001000)            /*!< Port power                 */

#define USB_OTG_HPRT_PTCTL                   ((uint32_t)0x0001E000)            /*!< Port test control          */
#define USB_OTG_HPRT_PTCTL_0                 ((uint32_t)0x00002000)            /*!<Bit 0 */
#define USB_OTG_HPRT_PTCTL_1                 ((uint32_t)0x00004000)            /*!<Bit 1 */
#define USB_OTG_HPRT_PTCTL_2                 ((uint32_t)0x00008000)            /*!<Bit 2 */
#define USB_OTG_HPRT_PTCTL_3                 ((uint32_t)0x00010000)            /*!<Bit 3 */

#define USB_OTG_HPRT_PSPD                    ((uint32_t)0x00060000)            /*!< Port speed                 */
#define USB_OTG_HPRT_PSPD_0                  ((uint32_t)0x00020000)            /*!<Bit 0 */
#define USB_OTG_HPRT_PSPD_1                  ((uint32_t)0x00040000)            /*!<Bit 1 */

/********************  Bit definition for USB_OTG_DOEPEACHMSK1 register  ********************/
#define USB_OTG_DOEPEACHMSK1_XFRCM                   ((uint32_t)0x00000001)            /*!< Transfer completed interrupt mask         */
#define USB_OTG_DOEPEACHMSK1_EPDM                    ((uint32_t)0x00000002)            /*!< Endpoint disabled interrupt mask          */
#define USB_OTG_DOEPEACHMSK1_TOM                     ((uint32_t)0x00000008)            /*!< Timeout condition mask                    */
#define USB_OTG_DOEPEACHMSK1_ITTXFEMSK               ((uint32_t)0x00000010)            /*!< IN token received when TxFIFO empty mask  */
#define USB_OTG_DOEPEACHMSK1_INEPNMM                 ((uint32_t)0x00000020)            /*!< IN token received with EP mismatch mask   */
#define USB_OTG_DOEPEACHMSK1_INEPNEM                 ((uint32_t)0x00000040)            /*!< IN endpoint NAK effective mask            */
#define USB_OTG_DOEPEACHMSK1_TXFURM                  ((uint32_t)0x00000100)            /*!< OUT packet error mask                     */
#define USB_OTG_DOEPEACHMSK1_BIM                     ((uint32_t)0x00000200)            /*!< BNA interrupt mask                        */
#define USB_OTG_DOEPEACHMSK1_BERRM                   ((uint32_t)0x00001000)            /*!< Bubble error interrupt mask               */
#define USB_OTG_DOEPEACHMSK1_NAKM                    ((uint32_t)0x00002000)            /*!< NAK interrupt mask                        */
#define USB_OTG_DOEPEACHMSK1_NYETM                   ((uint32_t)0x00004000)            /*!< NYET interrupt mask                       */

/********************  Bit definition for USB_OTG_HPTXFSIZ register  ********************/
#define USB_OTG_HPTXFSIZ_PTXSA                   ((uint32_t)0x0000FFFF)            /*!< Host periodic TxFIFO start address            */
#define USB_OTG_HPTXFSIZ_PTXFD                   ((uint32_t)0xFFFF0000)            /*!< Host periodic TxFIFO depth                    */

/********************  Bit definition for USB_OTG_DIEPCTL register  ********************/
#define USB_OTG_DIEPCTL_MPSIZ                   ((uint32_t)0x000007FF)            /*!< Maximum packet size              */
#define USB_OTG_DIEPCTL_USBAEP                  ((uint32_t)0x00008000)            /*!< USB active endpoint              */
#define USB_OTG_DIEPCTL_EONUM_DPID              ((uint32_t)0x00010000)            /*!< Even/odd frame                   */
#define USB_OTG_DIEPCTL_NAKSTS                  ((uint32_t)0x00020000)            /*!< NAK status                       */

#define USB_OTG_DIEPCTL_EPTYP                   ((uint32_t)0x000C0000)            /*!< Endpoint type                    */
#define USB_OTG_DIEPCTL_EPTYP_0                 ((uint32_t)0x00040000)            /*!<Bit 0 */
#define USB_OTG_DIEPCTL_EPTYP_1                 ((uint32_t)0x00080000)            /*!<Bit 1 */
#define USB_OTG_DIEPCTL_STALL                   ((uint32_t)0x00200000)            /*!< STALL handshake                  */

#define USB_OTG_DIEPCTL_TXFNUM                  ((uint32_t)0x03C00000)            /*!< TxFIFO number                    */
#define USB_OTG_DIEPCTL_TXFNUM_0                ((uint32_t)0x00400000)            /*!<Bit 0 */
#define USB_OTG_DIEPCTL_TXFNUM_1                ((uint32_t)0x00800000)            /*!<Bit 1 */
#define USB_OTG_DIEPCTL_TXFNUM_2                ((uint32_t)0x01000000)            /*!<Bit 2 */
#define USB_OTG_DIEPCTL_TXFNUM_3                ((uint32_t)0x02000000)            /*!<Bit 3 */
#define USB_OTG_DIEPCTL_CNAK                    ((uint32_t)0x04000000)            /*!< Clear NAK                        */
#define USB_OTG_DIEPCTL_SNAK                    ((uint32_t)0x08000000)            /*!< Set NAK */
#define USB_OTG_DIEPCTL_SD0PID_SEVNFRM          ((uint32_t)0x10000000)            /*!< Set DATA0 PID                    */
#define USB_OTG_DIEPCTL_SODDFRM                 ((uint32_t)0x20000000)            /*!< Set odd frame                    */
#define USB_OTG_DIEPCTL_EPDIS                   ((uint32_t)0x40000000)            /*!< Endpoint disable                 */
#define USB_OTG_DIEPCTL_EPENA                   ((uint32_t)0x80000000)            /*!< Endpoint enable                  */

/********************  Bit definition for USB_OTG_HCCHAR register  ********************/
#define USB_OTG_HCCHAR_MPSIZ                   ((uint32_t)0x000007FF)            /*!< Maximum packet size */

#define USB_OTG_HCCHAR_EPNUM                   ((uint32_t)0x00007800)            /*!< Endpoint number */
#define USB_OTG_HCCHAR_EPNUM_0                 ((uint32_t)0x00000800)            /*!<Bit 0 */
#define USB_OTG_HCCHAR_EPNUM_1                 ((uint32_t)0x00001000)            /*!<Bit 1 */
#define USB_OTG_HCCHAR_EPNUM_2                 ((uint32_t)0x00002000)            /*!<Bit 2 */
#define USB_OTG_HCCHAR_EPNUM_3                 ((uint32_t)0x00004000)            /*!<Bit 3 */
#define USB_OTG_HCCHAR_EPDIR                   ((uint32_t)0x00008000)            /*!< Endpoint direction */
#define USB_OTG_HCCHAR_LSDEV                   ((uint32_t)0x00020000)            /*!< Low-speed device */

#define USB_OTG_HCCHAR_EPTYP                   ((uint32_t)0x000C0000)            /*!< Endpoint type */
#define USB_OTG_HCCHAR_EPTYP_0                 ((uint32_t)0x00040000)            /*!<Bit 0 */
#define USB_OTG_HCCHAR_EPTYP_1                 ((uint32_t)0x00080000)            /*!<Bit 1 */

#define USB_OTG_HCCHAR_MC                      ((uint32_t)0x00300000)            /*!< Multi Count (MC) / Error Count (EC) */
#define USB_OTG_HCCHAR_MC_0                    ((uint32_t)0x00100000)            /*!<Bit 0 */
#define USB_OTG_HCCHAR_MC_1                    ((uint32_t)0x00200000)            /*!<Bit 1 */

#define USB_OTG_HCCHAR_DAD                     ((uint32_t)0x1FC00000)            /*!< Device address */
#define USB_OTG_HCCHAR_DAD_0                   ((uint32_t)0x00400000)            /*!<Bit 0 */
#define USB_OTG_HCCHAR_DAD_1                   ((uint32_t)0x00800000)            /*!<Bit 1 */
#define USB_OTG_HCCHAR_DAD_2                   ((uint32_t)0x01000000)            /*!<Bit 2 */
#define USB_OTG_HCCHAR_DAD_3                   ((uint32_t)0x02000000)            /*!<Bit 3 */
#define USB_OTG_HCCHAR_DAD_4                   ((uint32_t)0x04000000)            /*!<Bit 4 */
#define USB_OTG_HCCHAR_DAD_5                   ((uint32_t)0x08000000)            /*!<Bit 5 */
#define USB_OTG_HCCHAR_DAD_6                   ((uint32_t)0x10000000)            /*!<Bit 6 */
#define USB_OTG_HCCHAR_ODDFRM                  ((uint32_t)0x20000000)            /*!< Odd frame */
#define USB_OTG_HCCHAR_CHDIS                   ((uint32_t)0x40000000)            /*!< Channel disable */
#define USB_OTG_HCCHAR_CHENA                   ((uint32_t)0x80000000)            /*!< Channel enable */

/********************  Bit definition for USB_OTG_HCSPLT register  ********************/

#define USB_OTG_HCSPLT_PRTADDR                 ((uint32_t)0x0000007F)            /*!< Port address */
#define USB_OTG_HCSPLT_PRTADDR_0               ((uint32_t)0x00000001)            /*!<Bit 0 */
#define USB_OTG_HCSPLT_PRTADDR_1               ((uint32_t)0x00000002)            /*!<Bit 1 */
#define USB_OTG_HCSPLT_PRTADDR_2               ((uint32_t)0x00000004)            /*!<Bit 2 */
#define USB_OTG_HCSPLT_PRTADDR_3               ((uint32_t)0x00000008)            /*!<Bit 3 */
#define USB_OTG_HCSPLT_PRTADDR_4               ((uint32_t)0x00000010)            /*!<Bit 4 */
#define USB_OTG_HCSPLT_PRTADDR_5               ((uint32_t)0x00000020)            /*!<Bit 5 */
#define USB_OTG_HCSPLT_PRTADDR_6               ((uint32_t)0x00000040)            /*!<Bit 6 */

#define USB_OTG_HCSPLT_HUBADDR                 ((uint32_t)0x00003F80)            /*!< Hub address */
#define USB_OTG_HCSPLT_HUBADDR_0               ((uint32_t)0x00000080)            /*!<Bit 0 */
#define USB_OTG_HCSPLT_HUBADDR_1               ((uint32_t)0x00000100)            /*!<Bit 1 */
#define USB_OTG_HCSPLT_HUBADDR_2               ((uint32_t)0x00000200)            /*!<Bit 2 */
#define USB_OTG_HCSPLT_HUBADDR_3               ((uint32_t)0x00000400)            /*!<Bit 3 */
#define USB_OTG_HCSPLT_HUBADDR_4               ((uint32_t)0x00000800)            /*!<Bit 4 */
#define USB_OTG_HCSPLT_HUBADDR_5               ((uint32_t)0x00001000)            /*!<Bit 5 */
#define USB_OTG_HCSPLT_HUBADDR_6               ((uint32_t)0x00002000)            /*!<Bit 6 */

#define USB_OTG_HCSPLT_XACTPOS                 ((uint32_t)0x0000C000)            /*!< XACTPOS */
#define USB_OTG_HCSPLT_XACTPOS_0               ((uint32_t)0x00004000)            /*!<Bit 0 */
#define USB_OTG_HCSPLT_XACTPOS_1               ((uint32_t)0x00008000)            /*!<Bit 1 */
#define USB_OTG_HCSPLT_COMPLSPLT               ((uint32_t)0x00010000)            /*!< Do complete split */
#define USB_OTG_HCSPLT_SPLITEN                 ((uint32_t)0x80000000)            /*!< Split enable */

/********************  Bit definition for USB_OTG_HCINT register  ********************/
#define USB_OTG_HCINT_XFRC                    ((uint32_t)0x00000001)            /*!< Transfer completed */
#define USB_OTG_HCINT_CHH                     ((uint32_t)0x00000002)            /*!< Channel halted */
#define USB_OTG_HCINT_AHBERR                  ((uint32_t)0x00000004)            /*!< AHB error */
#define USB_OTG_HCINT_STALL                   ((uint32_t)0x00000008)            /*!< STALL response received interrupt */
#define USB_OTG_HCINT_NAK                     ((uint32_t)0x00000010)            /*!< NAK response received interrupt */
#define USB_OTG_HCINT_ACK                     ((uint32_t)0x00000020)            /*!< ACK response received/transmitted interrupt */
#define USB_OTG_HCINT_NYET                    ((uint32_t)0x00000040)            /*!< Response received interrupt */
#define USB_OTG_HCINT_TXERR                   ((uint32_t)0x00000080)            /*!< Transaction error */
#define USB_OTG_HCINT_BBERR                   ((uint32_t)0x00000100)            /*!< Babble error */
#define USB_OTG_HCINT_FRMOR                   ((uint32_t)0x00000200)            /*!< Frame overrun */
#define USB_OTG_HCINT_DTERR                   ((uint32_t)0x00000400)            /*!< Data toggle error */

/********************  Bit definition for USB_OTG_DIEPINT register  ********************/
#define USB_OTG_DIEPINT_XFRC                    ((uint32_t)0x00000001)            /*!< Transfer completed interrupt */
#define USB_OTG_DIEPINT_EPDISD                  ((uint32_t)0x00000002)            /*!< Endpoint disabled interrupt */
#define USB_OTG_DIEPINT_TOC                     ((uint32_t)0x00000008)            /*!< Timeout condition */
#define USB_OTG_DIEPINT_ITTXFE                  ((uint32_t)0x00000010)            /*!< IN token received when TxFIFO is empty */
#define USB_OTG_DIEPINT_INEPNE                  ((uint32_t)0x00000040)            /*!< IN endpoint NAK effective */
#define USB_OTG_DIEPINT_TXFE                    ((uint32_t)0x00000080)            /*!< Transmit FIFO empty */
#define USB_OTG_DIEPINT_TXFIFOUDRN              ((uint32_t)0x00000100)            /*!< Transmit Fifo Underrun */
#define USB_OTG_DIEPINT_BNA                     ((uint32_t)0x00000200)            /*!< Buffer not available interrupt */
#define USB_OTG_DIEPINT_PKTDRPSTS               ((uint32_t)0x00000800)            /*!< Packet dropped status */
#define USB_OTG_DIEPINT_BERR                    ((uint32_t)0x00001000)            /*!< Babble error interrupt */
#define USB_OTG_DIEPINT_NAK                     ((uint32_t)0x00002000)            /*!< NAK interrupt */

/********************  Bit definition for USB_OTG_HCINTMSK register  ********************/
#define USB_OTG_HCINTMSK_XFRCM                   ((uint32_t)0x00000001)            /*!< Transfer completed mask */
#define USB_OTG_HCINTMSK_CHHM                    ((uint32_t)0x00000002)            /*!< Channel halted mask */
#define USB_OTG_HCINTMSK_AHBERR                  ((uint32_t)0x00000004)            /*!< AHB error */
#define USB_OTG_HCINTMSK_STALLM                  ((uint32_t)0x00000008)            /*!< STALL response received interrupt mask */
#define USB_OTG_HCINTMSK_NAKM                    ((uint32_t)0x00000010)            /*!< NAK response received interrupt mask */
#define USB_OTG_HCINTMSK_ACKM                    ((uint32_t)0x00000020)            /*!< ACK response received/transmitted interrupt mask */
#define USB_OTG_HCINTMSK_NYET                    ((uint32_t)0x00000040)            /*!< response received interrupt mask */
#define USB_OTG_HCINTMSK_TXERRM                  ((uint32_t)0x00000080)            /*!< Transaction error mask */
#define USB_OTG_HCINTMSK_BBERRM                  ((uint32_t)0x00000100)            /*!< Babble error mask */
#define USB_OTG_HCINTMSK_FRMORM                  ((uint32_t)0x00000200)            /*!< Frame overrun mask */
#define USB_OTG_HCINTMSK_DTERRM                  ((uint32_t)0x00000400)            /*!< Data toggle error mask */

/********************  Bit definition for USB_OTG_DIEPTSIZ register  ********************/

#define USB_OTG_DIEPTSIZ_XFRSIZ                  ((uint32_t)0x0007FFFF)            /*!< Transfer size */
#define USB_OTG_DIEPTSIZ_PKTCNT                  ((uint32_t)0x1FF80000)            /*!< Packet count */
#define USB_OTG_DIEPTSIZ_MULCNT                  ((uint32_t)0x60000000)            /*!< Packet count */
/********************  Bit definition for USB_OTG_HCTSIZ register  ********************/
#define USB_OTG_HCTSIZ_XFRSIZ                    ((uint32_t)0x0007FFFF)            /*!< Transfer size */
#define USB_OTG_HCTSIZ_PKTCNT                    ((uint32_t)0x1FF80000)            /*!< Packet count */
#define USB_OTG_HCTSIZ_DOPING                    ((uint32_t)0x80000000)            /*!< Do PING */
#define USB_OTG_HCTSIZ_DPID                      ((uint32_t)0x60000000)            /*!< Data PID */
#define USB_OTG_HCTSIZ_DPID_0                    ((uint32_t)0x20000000)            /*!<Bit 0 */
#define USB_OTG_HCTSIZ_DPID_1                    ((uint32_t)0x40000000)            /*!<Bit 1 */

/********************  Bit definition for USB_OTG_DIEPDMA register  ********************/
#define USB_OTG_DIEPDMA_DMAADDR                  ((uint32_t)0xFFFFFFFF)            /*!< DMA address */

/********************  Bit definition for USB_OTG_HCDMA register  ********************/
#define USB_OTG_HCDMA_DMAADDR                    ((uint32_t)0xFFFFFFFF)            /*!< DMA address */

/********************  Bit definition for USB_OTG_DTXFSTS register  ********************/
#define USB_OTG_DTXFSTS_INEPTFSAV                ((uint32_t)0x0000FFFF)            /*!< IN endpoint TxFIFO space available */

/********************  Bit definition for USB_OTG_DIEPTXF register  ********************/
#define USB_OTG_DIEPTXF_INEPTXSA                 ((uint32_t)0x0000FFFF)            /*!< IN endpoint FIFOx transmit RAM start address */
#define USB_OTG_DIEPTXF_INEPTXFD                 ((uint32_t)0xFFFF0000)            /*!< IN endpoint TxFIFO depth */

/********************  Bit definition for USB_OTG_DOEPCTL register  ********************/
#define USB_OTG_DOEPCTL_MPSIZ                     ((uint32_t)0x000007FF)            /*!< Maximum packet size */          /*!<Bit 1 */
#define USB_OTG_DOEPCTL_USBAEP                    ((uint32_t)0x00008000)            /*!< USB active endpoint */
#define USB_OTG_DOEPCTL_NAKSTS                    ((uint32_t)0x00020000)            /*!< NAK status */
#define USB_OTG_DOEPCTL_SD0PID_SEVNFRM            ((uint32_t)0x10000000)            /*!< Set DATA0 PID */
#define USB_OTG_DOEPCTL_SODDFRM                   ((uint32_t)0x20000000)            /*!< Set odd frame */
#define USB_OTG_DOEPCTL_EPTYP                     ((uint32_t)0x000C0000)            /*!< Endpoint type */
#define USB_OTG_DOEPCTL_EPTYP_0                   ((uint32_t)0x00040000)            /*!<Bit 0 */
#define USB_OTG_DOEPCTL_EPTYP_1                   ((uint32_t)0x00080000)            /*!<Bit 1 */
#define USB_OTG_DOEPCTL_SNPM                      ((uint32_t)0x00100000)            /*!< Snoop mode */
#define USB_OTG_DOEPCTL_STALL                     ((uint32_t)0x00200000)            /*!< STALL handshake */
#define USB_OTG_DOEPCTL_CNAK                      ((uint32_t)0x04000000)            /*!< Clear NAK */
#define USB_OTG_DOEPCTL_SNAK                      ((uint32_t)0x08000000)            /*!< Set NAK */
#define USB_OTG_DOEPCTL_EPDIS                     ((uint32_t)0x40000000)            /*!< Endpoint disable */
#define USB_OTG_DOEPCTL_EPENA                     ((uint32_t)0x80000000)            /*!< Endpoint enable */

/********************  Bit definition for USB_OTG_DOEPINT register  ********************/
#define USB_OTG_DOEPINT_XFRC                    ((uint32_t)0x00000001)            /*!< Transfer completed interrupt */
#define USB_OTG_DOEPINT_EPDISD                  ((uint32_t)0x00000002)            /*!< Endpoint disabled interrupt */
#define USB_OTG_DOEPINT_STUP                    ((uint32_t)0x00000008)            /*!< SETUP phase done */
#define USB_OTG_DOEPINT_OTEPDIS                 ((uint32_t)0x00000010)            /*!< OUT token received when endpoint disabled */
#define USB_OTG_DOEPINT_B2BSTUP                 ((uint32_t)0x00000040)            /*!< Back-to-back SETUP packets received */
#define USB_OTG_DOEPINT_NYET                    ((uint32_t)0x00004000)            /*!< NYET interrupt */

/********************  Bit definition for USB_OTG_DOEPTSIZ register  ********************/
#define USB_OTG_DOEPTSIZ_XFRSIZ                  ((uint32_t)0x0007FFFF)            /*!< Transfer size */
#define USB_OTG_DOEPTSIZ_PKTCNT                  ((uint32_t)0x1FF80000)            /*!< Packet count */

#define USB_OTG_DOEPTSIZ_STUPCNT                 ((uint32_t)0x60000000)            /*!< SETUP packet count */
#define USB_OTG_DOEPTSIZ_STUPCNT_0               ((uint32_t)0x20000000)            /*!<Bit 0 */
#define USB_OTG_DOEPTSIZ_STUPCNT_1               ((uint32_t)0x40000000)            /*!<Bit 1 */

/********************  Bit definition for PCGCCTL register  ********************/
#define USB_OTG_PCGCCTL_STOPCLK                 ((uint32_t)0x00000001)            /*!< SETUP packet count */
#define USB_OTG_PCGCCTL_GATECLK                 ((uint32_t)0x00000002)            /*!<Bit 0 */
#define USB_OTG_PCGCCTL_PHYSUSP                 ((uint32_t)0x00000010)            /*!<Bit 1 */

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
#define IS_SAI_BLOCK_PERIPH(__PERIPH__) (((__PERIPH__) == SAI1_Block_A) || \
                                     ((__PERIPH__) == SAI1_Block_B) || \
                                     ((__PERIPH__) == SAI2_Block_A) || \
                                     ((__PERIPH__) == SAI2_Block_B))


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
