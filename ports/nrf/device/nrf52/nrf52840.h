
/****************************************************************************************************//**
 * @file     nrf52840.h
 *
 * @brief    CMSIS Cortex-M4 Peripheral Access Layer Header File for
 *           nrf52840 from Nordic Semiconductor.
 *
 * @version  V1
 * @date     18. November 2016
 *
 * @note     Generated with SVDConv V2.81d 
 *           from CMSIS SVD File 'nrf52840.svd' Version 1,
 *
 * @par      Copyright (c) 2016, Nordic Semiconductor ASA
 *           All rights reserved.
 *           
 *           Redistribution and use in source and binary forms, with or without
 *           modification, are permitted provided that the following conditions are met:
 *           
 *           * Redistributions of source code must retain the above copyright notice, this
 *           list of conditions and the following disclaimer.
 *           
 *           * Redistributions in binary form must reproduce the above copyright notice,
 *           this list of conditions and the following disclaimer in the documentation
 *           and/or other materials provided with the distribution.
 *           
 *           * Neither the name of Nordic Semiconductor ASA nor the names of its
 *           contributors may be used to endorse or promote products derived from
 *           this software without specific prior written permission.
 *           
 *           THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *           AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *           IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *           DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *           FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *           DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *           SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *           CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *           OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *           OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *           
 *
 *******************************************************************************************************/



/** @addtogroup Nordic Semiconductor
  * @{
  */

/** @addtogroup nrf52840
  * @{
  */

#ifndef NRF52840_H
#define NRF52840_H

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum {
/* -------------------  Cortex-M4 Processor Exceptions Numbers  ------------------- */
  Reset_IRQn                    = -15,              /*!<   1  Reset Vector, invoked on Power up and warm reset                 */
  NonMaskableInt_IRQn           = -14,              /*!<   2  Non maskable Interrupt, cannot be stopped or preempted           */
  HardFault_IRQn                = -13,              /*!<   3  Hard Fault, all classes of Fault                                 */
  MemoryManagement_IRQn         = -12,              /*!<   4  Memory Management, MPU mismatch, including Access Violation
                                                         and No Match                                                          */
  BusFault_IRQn                 = -11,              /*!<   5  Bus Fault, Pre-Fetch-, Memory Access Fault, other address/memory
                                                         related Fault                                                         */
  UsageFault_IRQn               = -10,              /*!<   6  Usage Fault, i.e. Undef Instruction, Illegal State Transition    */
  SVCall_IRQn                   =  -5,              /*!<  11  System Service Call via SVC instruction                          */
  DebugMonitor_IRQn             =  -4,              /*!<  12  Debug Monitor                                                    */
  PendSV_IRQn                   =  -2,              /*!<  14  Pendable request for system service                              */
  SysTick_IRQn                  =  -1,              /*!<  15  System Tick Timer                                                */
/* ---------------------  nrf52840 Specific Interrupt Numbers  -------------------- */
  POWER_CLOCK_IRQn              =   0,              /*!<   0  POWER_CLOCK                                                      */
  RADIO_IRQn                    =   1,              /*!<   1  RADIO                                                            */
  UARTE0_UART0_IRQn             =   2,              /*!<   2  UARTE0_UART0                                                     */
  SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn=   3,      /*!<   3  SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0                                */
  SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn=   4,      /*!<   4  SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1                                */
  NFCT_IRQn                     =   5,              /*!<   5  NFCT                                                             */
  GPIOTE_IRQn                   =   6,              /*!<   6  GPIOTE                                                           */
  SAADC_IRQn                    =   7,              /*!<   7  SAADC                                                            */
  TIMER0_IRQn                   =   8,              /*!<   8  TIMER0                                                           */
  TIMER1_IRQn                   =   9,              /*!<   9  TIMER1                                                           */
  TIMER2_IRQn                   =  10,              /*!<  10  TIMER2                                                           */
  RTC0_IRQn                     =  11,              /*!<  11  RTC0                                                             */
  TEMP_IRQn                     =  12,              /*!<  12  TEMP                                                             */
  RNG_IRQn                      =  13,              /*!<  13  RNG                                                              */
  ECB_IRQn                      =  14,              /*!<  14  ECB                                                              */
  CCM_AAR_IRQn                  =  15,              /*!<  15  CCM_AAR                                                          */
  WDT_IRQn                      =  16,              /*!<  16  WDT                                                              */
  RTC1_IRQn                     =  17,              /*!<  17  RTC1                                                             */
  QDEC_IRQn                     =  18,              /*!<  18  QDEC                                                             */
  COMP_LPCOMP_IRQn              =  19,              /*!<  19  COMP_LPCOMP                                                      */
  SWI0_EGU0_IRQn                =  20,              /*!<  20  SWI0_EGU0                                                        */
  SWI1_EGU1_IRQn                =  21,              /*!<  21  SWI1_EGU1                                                        */
  SWI2_EGU2_IRQn                =  22,              /*!<  22  SWI2_EGU2                                                        */
  SWI3_EGU3_IRQn                =  23,              /*!<  23  SWI3_EGU3                                                        */
  SWI4_EGU4_IRQn                =  24,              /*!<  24  SWI4_EGU4                                                        */
  SWI5_EGU5_IRQn                =  25,              /*!<  25  SWI5_EGU5                                                        */
  TIMER3_IRQn                   =  26,              /*!<  26  TIMER3                                                           */
  TIMER4_IRQn                   =  27,              /*!<  27  TIMER4                                                           */
  PWM0_IRQn                     =  28,              /*!<  28  PWM0                                                             */
  PDM_IRQn                      =  29,              /*!<  29  PDM                                                              */
  MWU_IRQn                      =  32,              /*!<  32  MWU                                                              */
  PWM1_IRQn                     =  33,              /*!<  33  PWM1                                                             */
  PWM2_IRQn                     =  34,              /*!<  34  PWM2                                                             */
  SPIM2_SPIS2_SPI2_IRQn         =  35,              /*!<  35  SPIM2_SPIS2_SPI2                                                 */
  RTC2_IRQn                     =  36,              /*!<  36  RTC2                                                             */
  I2S_IRQn                      =  37,              /*!<  37  I2S                                                              */
  FPU_IRQn                      =  38,              /*!<  38  FPU                                                              */
  USBD_IRQn                     =  39,              /*!<  39  USBD                                                             */
  UARTE1_IRQn                   =  40,              /*!<  40  UARTE1                                                           */
  QSPI_IRQn                     =  41,              /*!<  41  QSPI                                                             */
  CRYPTOCELL_IRQn               =  42,              /*!<  42  CRYPTOCELL                                                       */
  SPIM3_IRQn                    =  43,              /*!<  43  SPIM3                                                            */
  PWM3_IRQn                     =  45               /*!<  45  PWM3                                                             */
} IRQn_Type;


/** @addtogroup Configuration_of_CMSIS
  * @{
  */


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* ----------------Configuration of the Cortex-M4 Processor and Core Peripherals---------------- */
#define __CM4_REV                 0x0001            /*!< Cortex-M4 Core Revision                                               */
#define __MPU_PRESENT                  1            /*!< MPU present or not                                                    */
#define __NVIC_PRIO_BITS               3            /*!< Number of Bits used for Priority Levels                               */
#define __Vendor_SysTickConfig         0            /*!< Set to 1 if different SysTick Config is used                          */
#define __FPU_PRESENT                  1            /*!< FPU present or not                                                    */
/** @} */ /* End of group Configuration_of_CMSIS */

#include "core_cm4.h"                               /*!< Cortex-M4 processor and core peripherals                              */
#include "system_nrf52840.h"                        /*!< nrf52840 System                                                       */


/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */


/** @addtogroup Device_Peripheral_Registers
  * @{
  */


/* -------------------  Start of section using anonymous unions  ------------------ */
#if defined(__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler type
#endif


typedef struct {
  __I  uint32_t  PART;                              /*!< Part code                                                             */
  __I  uint32_t  VARIANT;                           /*!< Part variant (hardware version and production configuration).         */
  __I  uint32_t  PACKAGE;                           /*!< Package option                                                        */
  __I  uint32_t  RAM;                               /*!< RAM variant                                                           */
  __I  uint32_t  FLASH;                             /*!< Flash variant                                                         */
  __IO uint32_t  UNUSED0[3];                        /*!< Description collection[0]: Unspecified                                */
} FICR_INFO_Type;

typedef struct {
  __I  uint32_t  A0;                                /*!< Slope definition A0.                                                  */
  __I  uint32_t  A1;                                /*!< Slope definition A1.                                                  */
  __I  uint32_t  A2;                                /*!< Slope definition A2.                                                  */
  __I  uint32_t  A3;                                /*!< Slope definition A3.                                                  */
  __I  uint32_t  A4;                                /*!< Slope definition A4.                                                  */
  __I  uint32_t  A5;                                /*!< Slope definition A5.                                                  */
  __I  uint32_t  B0;                                /*!< y-intercept B0.                                                       */
  __I  uint32_t  B1;                                /*!< y-intercept B1.                                                       */
  __I  uint32_t  B2;                                /*!< y-intercept B2.                                                       */
  __I  uint32_t  B3;                                /*!< y-intercept B3.                                                       */
  __I  uint32_t  B4;                                /*!< y-intercept B4.                                                       */
  __I  uint32_t  B5;                                /*!< y-intercept B5.                                                       */
  __I  uint32_t  T0;                                /*!< Segment end T0.                                                       */
  __I  uint32_t  T1;                                /*!< Segment end T1.                                                       */
  __I  uint32_t  T2;                                /*!< Segment end T2.                                                       */
  __I  uint32_t  T3;                                /*!< Segment end T3.                                                       */
  __I  uint32_t  T4;                                /*!< Segment end T4.                                                       */
} FICR_TEMP_Type;

typedef struct {
  __I  uint32_t  TAGHEADER0;                        /*!< Default header for NFC Tag. Software can read these values to
                                                         populate NFCID1_3RD_LAST, NFCID1_2ND_LAST and NFCID1_LAST.            */
  __I  uint32_t  TAGHEADER1;                        /*!< Default header for NFC Tag. Software can read these values to
                                                         populate NFCID1_3RD_LAST, NFCID1_2ND_LAST and NFCID1_LAST.            */
  __I  uint32_t  TAGHEADER2;                        /*!< Default header for NFC Tag. Software can read these values to
                                                         populate NFCID1_3RD_LAST, NFCID1_2ND_LAST and NFCID1_LAST.            */
  __I  uint32_t  TAGHEADER3;                        /*!< Default header for NFC Tag. Software can read these values to
                                                         populate NFCID1_3RD_LAST, NFCID1_2ND_LAST and NFCID1_LAST.            */
} FICR_NFC_Type;

typedef struct {
  __IO uint32_t  POWER;                             /*!< Description cluster[0]: RAM0 power control register                   */
  __O  uint32_t  POWERSET;                          /*!< Description cluster[0]: RAM0 power control set register               */
  __O  uint32_t  POWERCLR;                          /*!< Description cluster[0]: RAM0 power control clear register             */
  __I  uint32_t  RESERVED0;
} POWER_RAM_Type;

typedef struct {
  __IO uint32_t  RTS;                               /*!< Pin select for RTS signal                                             */
  __IO uint32_t  TXD;                               /*!< Pin select for TXD signal                                             */
  __IO uint32_t  CTS;                               /*!< Pin select for CTS signal                                             */
  __IO uint32_t  RXD;                               /*!< Pin select for RXD signal                                             */
} UARTE_PSEL_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< Data pointer                                                          */
  __IO uint32_t  MAXCNT;                            /*!< Maximum number of bytes in receive buffer                             */
  __I  uint32_t  AMOUNT;                            /*!< Number of bytes transferred in the last transaction                   */
} UARTE_RXD_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< Data pointer                                                          */
  __IO uint32_t  MAXCNT;                            /*!< Maximum number of bytes in transmit buffer                            */
  __I  uint32_t  AMOUNT;                            /*!< Number of bytes transferred in the last transaction                   */
} UARTE_TXD_Type;

typedef struct {
  __IO uint32_t  RTS;                               /*!< Pin select for RTS                                                    */
  __IO uint32_t  TXD;                               /*!< Pin select for TXD                                                    */
  __IO uint32_t  CTS;                               /*!< Pin select for CTS                                                    */
  __IO uint32_t  RXD;                               /*!< Pin select for RXD                                                    */
} UART_PSEL_Type;

typedef struct {
  __IO uint32_t  SCK;                               /*!< Pin select for SCK                                                    */
  __IO uint32_t  MOSI;                              /*!< Pin select for MOSI signal                                            */
  __IO uint32_t  MISO;                              /*!< Pin select for MISO signal                                            */
  __IO uint32_t  CSN;                               /*!< Pin select for CSN                                                    */
} SPIM_PSEL_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< Data pointer                                                          */
  __IO uint32_t  MAXCNT;                            /*!< Maximum number of bytes in receive buffer                             */
  __I  uint32_t  AMOUNT;                            /*!< Number of bytes transferred in the last transaction                   */
  __IO uint32_t  LIST;                              /*!< EasyDMA list type                                                     */
} SPIM_RXD_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< Data pointer                                                          */
  __IO uint32_t  MAXCNT;                            /*!< Number of bytes in transmit buffer                                    */
  __I  uint32_t  AMOUNT;                            /*!< Number of bytes transferred in the last transaction                   */
  __IO uint32_t  LIST;                              /*!< EasyDMA list type                                                     */
} SPIM_TXD_Type;

typedef struct {
  __IO uint32_t  RXDELAY;                           /*!< Sample delay for input serial data on MISO                            */
  __IO uint32_t  CSNDUR;                            /*!< Minimum duration between edge of CSN and edge of SCK and minimum
                                                         duration CSN must stay high between transactions                      */
} SPIM_IFTIMING_Type;

typedef struct {
  __IO uint32_t  SCK;                               /*!< Pin select for SCK                                                    */
  __IO uint32_t  MISO;                              /*!< Pin select for MISO signal                                            */
  __IO uint32_t  MOSI;                              /*!< Pin select for MOSI signal                                            */
  __IO uint32_t  CSN;                               /*!< Pin select for CSN signal                                             */
} SPIS_PSEL_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< RXD data pointer                                                      */
  __IO uint32_t  MAXCNT;                            /*!< Maximum number of bytes in receive buffer                             */
  __I  uint32_t  AMOUNT;                            /*!< Number of bytes received in last granted transaction                  */
} SPIS_RXD_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< TXD data pointer                                                      */
  __IO uint32_t  MAXCNT;                            /*!< Maximum number of bytes in transmit buffer                            */
  __I  uint32_t  AMOUNT;                            /*!< Number of bytes transmitted in last granted transaction               */
} SPIS_TXD_Type;

typedef struct {
  __IO uint32_t  SCL;                               /*!< Pin select for SCL signal                                             */
  __IO uint32_t  SDA;                               /*!< Pin select for SDA signal                                             */
} TWIM_PSEL_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< Data pointer                                                          */
  __IO uint32_t  MAXCNT;                            /*!< Maximum number of bytes in receive buffer                             */
  __I  uint32_t  AMOUNT;                            /*!< Number of bytes transferred in the last transaction                   */
  __IO uint32_t  LIST;                              /*!< EasyDMA list type                                                     */
} TWIM_RXD_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< Data pointer                                                          */
  __IO uint32_t  MAXCNT;                            /*!< Maximum number of bytes in transmit buffer                            */
  __I  uint32_t  AMOUNT;                            /*!< Number of bytes transferred in the last transaction                   */
  __IO uint32_t  LIST;                              /*!< EasyDMA list type                                                     */
} TWIM_TXD_Type;

typedef struct {
  __IO uint32_t  SCL;                               /*!< Pin select for SCL signal                                             */
  __IO uint32_t  SDA;                               /*!< Pin select for SDA signal                                             */
} TWIS_PSEL_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< RXD Data pointer                                                      */
  __IO uint32_t  MAXCNT;                            /*!< Maximum number of bytes in RXD buffer                                 */
  __I  uint32_t  AMOUNT;                            /*!< Number of bytes transferred in the last RXD transaction               */
} TWIS_RXD_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< TXD Data pointer                                                      */
  __IO uint32_t  MAXCNT;                            /*!< Maximum number of bytes in TXD buffer                                 */
  __I  uint32_t  AMOUNT;                            /*!< Number of bytes transferred in the last TXD transaction               */
} TWIS_TXD_Type;

typedef struct {
  __IO uint32_t  SCK;                               /*!< Pin select for SCK                                                    */
  __IO uint32_t  MOSI;                              /*!< Pin select for MOSI signal                                            */
  __IO uint32_t  MISO;                              /*!< Pin select for MISO signal                                            */
} SPI_PSEL_Type;

typedef struct {
  __IO uint32_t  SCL;                               /*!< Pin select for SCL                                                    */
  __IO uint32_t  SDA;                               /*!< Pin select for SDA                                                    */
} TWI_PSEL_Type;

typedef struct {
  __IO uint32_t  RX;                                /*!< Result of last incoming frame                                         */
} NFCT_FRAMESTATUS_Type;

typedef struct {
  __IO uint32_t  FRAMECONFIG;                       /*!< Configuration of outgoing frames                                      */
  __IO uint32_t  AMOUNT;                            /*!< Size of outgoing frame                                                */
} NFCT_TXD_Type;

typedef struct {
  __IO uint32_t  FRAMECONFIG;                       /*!< Configuration of incoming frames                                      */
  __I  uint32_t  AMOUNT;                            /*!< Size of last incoming frame                                           */
} NFCT_RXD_Type;

typedef struct {
  __IO uint32_t  LIMITH;                            /*!< Description cluster[0]: Last results is equal or above CH[0].LIMIT.HIGH */
  __IO uint32_t  LIMITL;                            /*!< Description cluster[0]: Last results is equal or below CH[0].LIMIT.LOW */
} SAADC_EVENTS_CH_Type;

typedef struct {
  __IO uint32_t  PSELP;                             /*!< Description cluster[0]: Input positive pin selection for CH[0]        */
  __IO uint32_t  PSELN;                             /*!< Description cluster[0]: Input negative pin selection for CH[0]        */
  __IO uint32_t  CONFIG;                            /*!< Description cluster[0]: Input configuration for CH[0]                 */
  __IO uint32_t  LIMIT;                             /*!< Description cluster[0]: High/low limits for event monitoring
                                                         a channel                                                             */
} SAADC_CH_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< Data pointer                                                          */
  __IO uint32_t  MAXCNT;                            /*!< Maximum number of buffer words to transfer                            */
  __I  uint32_t  AMOUNT;                            /*!< Number of buffer words transferred since last START                   */
} SAADC_RESULT_Type;

typedef struct {
  __IO uint32_t  LED;                               /*!< Pin select for LED signal                                             */
  __IO uint32_t  A;                                 /*!< Pin select for A signal                                               */
  __IO uint32_t  B;                                 /*!< Pin select for B signal                                               */
} QDEC_PSEL_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< Description cluster[0]: Beginning address in Data RAM of sequence
                                                         A                                                                     */
  __IO uint32_t  CNT;                               /*!< Description cluster[0]: Amount of values (duty cycles) in sequence
                                                         A                                                                     */
  __IO uint32_t  REFRESH;                           /*!< Description cluster[0]: Amount of additional PWM periods between
                                                         samples loaded to compare register (load every CNT+1 PWM periods)     */
  __IO uint32_t  ENDDELAY;                          /*!< Description cluster[0]: Time added after the sequence                 */
  __I  uint32_t  RESERVED1[4];
} PWM_SEQ_Type;

typedef struct {
  __IO uint32_t  OUT[4];                            /*!< Description collection[0]: Output pin select for PWM channel
                                                         0                                                                     */
} PWM_PSEL_Type;

typedef struct {
  __IO uint32_t  CLK;                               /*!< Pin number configuration for PDM CLK signal                           */
  __IO uint32_t  DIN;                               /*!< Pin number configuration for PDM DIN signal                           */
} PDM_PSEL_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< RAM address pointer to write samples to with EasyDMA                  */
  __IO uint32_t  MAXCNT;                            /*!< Number of samples to allocate memory for in EasyDMA mode              */
} PDM_SAMPLE_Type;

typedef struct {
  __IO uint32_t  ADDR;                              /*!< Description cluster[0]: Configure the word-aligned start address
                                                         of region 0 to protect                                                */
  __IO uint32_t  SIZE;                              /*!< Description cluster[0]: Size of region to protect counting from
                                                         address ACL[0].ADDR. Write '0' as no effect.                          */
  __IO uint32_t  PERM;                              /*!< Description cluster[0]: Access permissions for region 0 as defined
                                                         by start address ACL[0].ADDR and size ACL[0].SIZE                     */
  __IO uint32_t  UNUSED0;                           /*!< Unspecified                                                           */
} ACL_ACL_Type;

typedef struct {
  __O  uint32_t  EN;                                /*!< Description cluster[0]: Enable channel group 0                        */
  __O  uint32_t  DIS;                               /*!< Description cluster[0]: Disable channel group 0                       */
} PPI_TASKS_CHG_Type;

typedef struct {
  __IO uint32_t  EEP;                               /*!< Description cluster[0]: Channel 0 event end-point                     */
  __IO uint32_t  TEP;                               /*!< Description cluster[0]: Channel 0 task end-point                      */
} PPI_CH_Type;

typedef struct {
  __IO uint32_t  TEP;                               /*!< Description cluster[0]: Channel 0 task end-point                      */
} PPI_FORK_Type;

typedef struct {
  __IO uint32_t  WA;                                /*!< Description cluster[0]: Write access to region 0 detected             */
  __IO uint32_t  RA;                                /*!< Description cluster[0]: Read access to region 0 detected              */
} MWU_EVENTS_REGION_Type;

typedef struct {
  __IO uint32_t  WA;                                /*!< Description cluster[0]: Write access to peripheral region 0
                                                         detected                                                              */
  __IO uint32_t  RA;                                /*!< Description cluster[0]: Read access to peripheral region 0 detected   */
} MWU_EVENTS_PREGION_Type;

typedef struct {
  __IO uint32_t  SUBSTATWA;                         /*!< Description cluster[0]: Source of event/interrupt in region
                                                         0, write access detected while corresponding subregion was enabled
                                                          for watching                                                         */
  __IO uint32_t  SUBSTATRA;                         /*!< Description cluster[0]: Source of event/interrupt in region
                                                         0, read access detected while corresponding subregion was enabled
                                                          for watching                                                         */
} MWU_PERREGION_Type;

typedef struct {
  __IO uint32_t  START;                             /*!< Description cluster[0]: Start address for region 0                    */
  __IO uint32_t  END;                               /*!< Description cluster[0]: End address of region 0                       */
  __I  uint32_t  RESERVED2[2];
} MWU_REGION_Type;

typedef struct {
  __I  uint32_t  START;                             /*!< Description cluster[0]: Reserved for future use                       */
  __I  uint32_t  END;                               /*!< Description cluster[0]: Reserved for future use                       */
  __IO uint32_t  SUBS;                              /*!< Description cluster[0]: Subregions of region 0                        */
  __I  uint32_t  RESERVED3;
} MWU_PREGION_Type;

typedef struct {
  __IO uint32_t  MODE;                              /*!< I2S mode.                                                             */
  __IO uint32_t  RXEN;                              /*!< Reception (RX) enable.                                                */
  __IO uint32_t  TXEN;                              /*!< Transmission (TX) enable.                                             */
  __IO uint32_t  MCKEN;                             /*!< Master clock generator enable.                                        */
  __IO uint32_t  MCKFREQ;                           /*!< Master clock generator frequency.                                     */
  __IO uint32_t  RATIO;                             /*!< MCK / LRCK ratio.                                                     */
  __IO uint32_t  SWIDTH;                            /*!< Sample width.                                                         */
  __IO uint32_t  ALIGN;                             /*!< Alignment of sample within a frame.                                   */
  __IO uint32_t  FORMAT;                            /*!< Frame format.                                                         */
  __IO uint32_t  CHANNELS;                          /*!< Enable channels.                                                      */
} I2S_CONFIG_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< Receive buffer RAM start address.                                     */
} I2S_RXD_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< Transmit buffer RAM start address.                                    */
} I2S_TXD_Type;

typedef struct {
  __IO uint32_t  MAXCNT;                            /*!< Size of RXD and TXD buffers.                                          */
} I2S_RXTXD_Type;

typedef struct {
  __IO uint32_t  MCK;                               /*!< Pin select for MCK signal.                                            */
  __IO uint32_t  SCK;                               /*!< Pin select for SCK signal.                                            */
  __IO uint32_t  LRCK;                              /*!< Pin select for LRCK signal.                                           */
  __IO uint32_t  SDIN;                              /*!< Pin select for SDIN signal.                                           */
  __IO uint32_t  SDOUT;                             /*!< Pin select for SDOUT signal.                                          */
} I2S_PSEL_Type;

typedef struct {
  __I  uint32_t  EPIN[8];                           /*!< Description collection[0]: IN endpoint halted status. Can be
                                                         used as is as response to a GetStatus() request to endpoint.          */
  __I  uint32_t  RESERVED4;
  __I  uint32_t  EPOUT[8];                          /*!< Description collection[0]: OUT endpoint halted status. Can be
                                                         used as is as response to a GetStatus() request to endpoint.          */
} USBD_HALTED_Type;

typedef struct {
  __IO uint32_t  EPOUT[8];                          /*!< Description collection[0]: Amount of bytes received last in
                                                         the data stage of this OUT endpoint                                   */
  __IO uint32_t  ISOOUT;                            /*!< Amount of bytes received last on this iso OUT data endpoint           */
} USBD_SIZE_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< Description cluster[0]: Data pointer                                  */
  __IO uint32_t  MAXCNT;                            /*!< Description cluster[0]: Maximum number of bytes to transfer           */
  __I  uint32_t  AMOUNT;                            /*!< Description cluster[0]: Number of bytes transferred in the last
                                                         transaction                                                           */
  __I  uint32_t  RESERVED5[2];
} USBD_EPIN_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< Data pointer                                                          */
  __IO uint32_t  MAXCNT;                            /*!< Maximum number of bytes to transfer                                   */
  __I  uint32_t  AMOUNT;                            /*!< Number of bytes transferred in the last transaction                   */
} USBD_ISOIN_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< Description cluster[0]: Data pointer                                  */
  __IO uint32_t  MAXCNT;                            /*!< Description cluster[0]: Maximum number of bytes to transfer           */
  __I  uint32_t  AMOUNT;                            /*!< Description cluster[0]: Number of bytes transferred in the last
                                                         transaction                                                           */
  __I  uint32_t  RESERVED6[2];
} USBD_EPOUT_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< Data pointer                                                          */
  __IO uint32_t  MAXCNT;                            /*!< Maximum number of bytes to transfer                                   */
  __I  uint32_t  AMOUNT;                            /*!< Number of bytes transferred in the last transaction                   */
} USBD_ISOOUT_Type;

typedef struct {
  __IO uint32_t  SRC;                               /*!< Flash memory source address                                           */
  __IO uint32_t  DST;                               /*!< RAM destination address                                               */
  __IO uint32_t  CNT;                               /*!< Read transfer length                                                  */
} QSPI_READ_Type;

typedef struct {
  __IO uint32_t  DST;                               /*!< Flash destination address                                             */
  __IO uint32_t  SRC;                               /*!< RAM source address                                                    */
  __IO uint32_t  CNT;                               /*!< Write transfer length                                                 */
} QSPI_WRITE_Type;

typedef struct {
  __IO uint32_t  PTR;                               /*!< Start address of flash block to be erased                             */
  __IO uint32_t  LEN;                               /*!< Size of block to be erased.                                           */
} QSPI_ERASE_Type;

typedef struct {
  __IO uint32_t  SCK;                               /*!< Pin select for serial clock SCK                                       */
  __IO uint32_t  CSN;                               /*!< Pin select for chip select signal CSN.                                */
  __I  uint32_t  RESERVED7;
  __IO uint32_t  IO0;                               /*!< Pin select for serial data MOSI/IO0.                                  */
  __IO uint32_t  IO1;                               /*!< Pin select for serial data MISO/IO1.                                  */
  __IO uint32_t  IO2;                               /*!< Pin select for serial data IO2.                                       */
  __IO uint32_t  IO3;                               /*!< Pin select for serial data IO3.                                       */
} QSPI_PSEL_Type;


/* ================================================================================ */
/* ================                      FICR                      ================ */
/* ================================================================================ */


/**
  * @brief Factory Information Configuration Registers (FICR)
  */

typedef struct {                                    /*!< FICR Structure                                                        */
  __I  uint32_t  RESERVED0[4];
  __I  uint32_t  CODEPAGESIZE;                      /*!< Code memory page size                                                 */
  __I  uint32_t  CODESIZE;                          /*!< Code memory size                                                      */
  __I  uint32_t  RESERVED1[18];
  __I  uint32_t  DEVICEID[2];                       /*!< Description collection[0]: Device identifier                          */
  __I  uint32_t  RESERVED2[6];
  __I  uint32_t  ER[4];                             /*!< Description collection[0]: Encryption root, word 0                    */
  __I  uint32_t  IR[4];                             /*!< Description collection[0]: Identity Root, word 0                      */
  __I  uint32_t  DEVICEADDRTYPE;                    /*!< Device address type                                                   */
  __I  uint32_t  DEVICEADDR[2];                     /*!< Description collection[0]: Device address 0                           */
  __I  uint32_t  RESERVED3[21];
  FICR_INFO_Type INFO;                              /*!< Device info                                                           */
  __I  uint32_t  RESERVED4[185];
  FICR_TEMP_Type TEMP;                              /*!< Registers storing factory TEMP module linearization coefficients      */
  __I  uint32_t  RESERVED5[2];
  FICR_NFC_Type NFC;                                /*!< Unspecified                                                           */
} NRF_FICR_Type;


/* ================================================================================ */
/* ================                      UICR                      ================ */
/* ================================================================================ */


/**
  * @brief User Information Configuration Registers (UICR)
  */

typedef struct {                                    /*!< UICR Structure                                                        */
  __IO uint32_t  UNUSED0;                           /*!< Unspecified                                                           */
  __IO uint32_t  UNUSED1;                           /*!< Unspecified                                                           */
  __IO uint32_t  UNUSED2;                           /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED0;
  __IO uint32_t  UNUSED3;                           /*!< Unspecified                                                           */
  __IO uint32_t  NRFFW[15];                         /*!< Description collection[0]: Reserved for Nordic firmware design        */
  __IO uint32_t  NRFHW[12];                         /*!< Description collection[0]: Reserved for Nordic hardware design        */
  __IO uint32_t  CUSTOMER[32];                      /*!< Description collection[0]: Reserved for customer                      */
  __I  uint32_t  RESERVED1[64];
  __IO uint32_t  PSELRESET[2];                      /*!< Description collection[0]: Mapping of the nRESET function             */
  __IO uint32_t  APPROTECT;                         /*!< Access port protection                                                */
  __IO uint32_t  NFCPINS;                           /*!< Setting of pins dedicated to NFC functionality: NFC antenna
                                                         or GPIO                                                               */
  __I  uint32_t  RESERVED2[60];
  __IO uint32_t  EXTSUPPLY;                         /*!< Enable external circuitry to be supplied from VDD pin. Applicable
                                                         in 'High voltage mode' only.                                          */
  __IO uint32_t  REGOUT0;                           /*!< GPIO reference voltage / external output supply voltage in 'High
                                                         voltage mode'.                                                        */
} NRF_UICR_Type;


/* ================================================================================ */
/* ================                      POWER                     ================ */
/* ================================================================================ */


/**
  * @brief Power control (POWER)
  */

typedef struct {                                    /*!< POWER Structure                                                       */
  __I  uint32_t  RESERVED0[30];
  __O  uint32_t  TASKS_CONSTLAT;                    /*!< Enable constant latency mode                                          */
  __O  uint32_t  TASKS_LOWPWR;                      /*!< Enable low power mode (variable latency)                              */
  __I  uint32_t  RESERVED1[34];
  __IO uint32_t  EVENTS_POFWARN;                    /*!< Power failure warning                                                 */
  __I  uint32_t  RESERVED2[2];
  __IO uint32_t  EVENTS_SLEEPENTER;                 /*!< CPU entered WFI/WFE sleep                                             */
  __IO uint32_t  EVENTS_SLEEPEXIT;                  /*!< CPU exited WFI/WFE sleep                                              */
  __IO uint32_t  EVENTS_USBDETECTED;                /*!< Voltage supply detected on VBUS                                       */
  __IO uint32_t  EVENTS_USBREMOVED;                 /*!< Voltage supply removed from VBUS                                      */
  __IO uint32_t  EVENTS_USBPWRRDY;                  /*!< USB 3.3 V supply ready                                                */
  __I  uint32_t  RESERVED3[119];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED4[61];
  __IO uint32_t  RESETREAS;                         /*!< Reset reason                                                          */
  __I  uint32_t  RESERVED5[9];
  __I  uint32_t  RAMSTATUS;                         /*!< Deprecated register - RAM status register                             */
  __I  uint32_t  RESERVED6[3];
  __I  uint32_t  USBREGSTATUS;                      /*!< USB supply status                                                     */
  __I  uint32_t  RESERVED7[49];
  __O  uint32_t  SYSTEMOFF;                         /*!< System OFF register                                                   */
  __I  uint32_t  RESERVED8[3];
  __IO uint32_t  POFCON;                            /*!< Power failure comparator configuration                                */
  __I  uint32_t  RESERVED9[2];
  __IO uint32_t  GPREGRET;                          /*!< General purpose retention register                                    */
  __IO uint32_t  GPREGRET2;                         /*!< General purpose retention register                                    */
  __I  uint32_t  RESERVED10[21];
  __IO uint32_t  DCDCEN;                            /*!< Enable DC/DC converter for REG1 stage.                                */
  __I  uint32_t  RESERVED11;
  __IO uint32_t  DCDCEN0;                           /*!< Enable DC/DC converter for REG0 stage.                                */
  __I  uint32_t  RESERVED12[47];
  __I  uint32_t  MAINREGSTATUS;                     /*!< Main supply status                                                    */
  __I  uint32_t  RESERVED13[175];
  POWER_RAM_Type RAM[9];                            /*!< Unspecified                                                           */
} NRF_POWER_Type;


/* ================================================================================ */
/* ================                      CLOCK                     ================ */
/* ================================================================================ */


/**
  * @brief Clock control (CLOCK)
  */

typedef struct {                                    /*!< CLOCK Structure                                                       */
  __O  uint32_t  TASKS_HFCLKSTART;                  /*!< Start HFCLK crystal oscillator                                        */
  __O  uint32_t  TASKS_HFCLKSTOP;                   /*!< Stop HFCLK crystal oscillator                                         */
  __O  uint32_t  TASKS_LFCLKSTART;                  /*!< Start LFCLK source                                                    */
  __O  uint32_t  TASKS_LFCLKSTOP;                   /*!< Stop LFCLK source                                                     */
  __O  uint32_t  TASKS_CAL;                         /*!< Start calibration of LFRC or LFULP oscillator                         */
  __O  uint32_t  TASKS_CTSTART;                     /*!< Start calibration timer                                               */
  __O  uint32_t  TASKS_CTSTOP;                      /*!< Stop calibration timer                                                */
  __I  uint32_t  RESERVED0[57];
  __IO uint32_t  EVENTS_HFCLKSTARTED;               /*!< HFCLK oscillator started                                              */
  __IO uint32_t  EVENTS_LFCLKSTARTED;               /*!< LFCLK started                                                         */
  __I  uint32_t  RESERVED1;
  __IO uint32_t  EVENTS_DONE;                       /*!< Calibration of LFCLK RC oscillator complete event                     */
  __IO uint32_t  EVENTS_CTTO;                       /*!< Calibration timer timeout                                             */
  __I  uint32_t  RESERVED2[124];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED3[63];
  __I  uint32_t  HFCLKRUN;                          /*!< Status indicating that HFCLKSTART task has been triggered             */
  __I  uint32_t  HFCLKSTAT;                         /*!< HFCLK status                                                          */
  __I  uint32_t  RESERVED4;
  __I  uint32_t  LFCLKRUN;                          /*!< Status indicating that LFCLKSTART task has been triggered             */
  __I  uint32_t  LFCLKSTAT;                         /*!< LFCLK status                                                          */
  __I  uint32_t  LFCLKSRCCOPY;                      /*!< Copy of LFCLKSRC register, set when LFCLKSTART task was triggered     */
  __I  uint32_t  RESERVED5[62];
  __IO uint32_t  LFCLKSRC;                          /*!< Clock source for the LFCLK                                            */
  __I  uint32_t  RESERVED6[7];
  __IO uint32_t  CTIV;                              /*!< Calibration timer interval                                            */
  __I  uint32_t  RESERVED7[8];
  __IO uint32_t  TRACECONFIG;                       /*!< Clocking options for the Trace Port debug interface                   */
} NRF_CLOCK_Type;


/* ================================================================================ */
/* ================                      RADIO                     ================ */
/* ================================================================================ */


/**
  * @brief 2.4 GHz Radio (RADIO)
  */

typedef struct {                                    /*!< RADIO Structure                                                       */
  __O  uint32_t  TASKS_TXEN;                        /*!< Enable RADIO in TX mode                                               */
  __O  uint32_t  TASKS_RXEN;                        /*!< Enable RADIO in RX mode                                               */
  __O  uint32_t  TASKS_START;                       /*!< Start RADIO                                                           */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop RADIO                                                            */
  __O  uint32_t  TASKS_DISABLE;                     /*!< Disable RADIO                                                         */
  __O  uint32_t  TASKS_RSSISTART;                   /*!< Start the RSSI and take one single sample of the receive signal
                                                         strength.                                                             */
  __O  uint32_t  TASKS_RSSISTOP;                    /*!< Stop the RSSI measurement                                             */
  __O  uint32_t  TASKS_BCSTART;                     /*!< Start the bit counter                                                 */
  __O  uint32_t  TASKS_BCSTOP;                      /*!< Stop the bit counter                                                  */
  __O  uint32_t  TASKS_EDSTART;                     /*!< Start the Energy Detect measurement used in IEEE 802.15.4 mode        */
  __O  uint32_t  TASKS_EDSTOP;                      /*!< Stop the Energy Detect measurement                                    */
  __O  uint32_t  TASKS_CCASTART;                    /*!< Start the Clear Channel Assessment used in IEEE 802.15.4 mode         */
  __O  uint32_t  TASKS_CCASTOP;                     /*!< Stop the Clear Channel Assessment                                     */
  __I  uint32_t  RESERVED0[51];
  __IO uint32_t  EVENTS_READY;                      /*!< RADIO has ramped up and is ready to be started                        */
  __IO uint32_t  EVENTS_ADDRESS;                    /*!< Address sent or received                                              */
  __IO uint32_t  EVENTS_PAYLOAD;                    /*!< Packet payload sent or received                                       */
  __IO uint32_t  EVENTS_END;                        /*!< Packet sent or received                                               */
  __IO uint32_t  EVENTS_DISABLED;                   /*!< RADIO has been disabled                                               */
  __IO uint32_t  EVENTS_DEVMATCH;                   /*!< A device address match occurred on the last received packet           */
  __IO uint32_t  EVENTS_DEVMISS;                    /*!< No device address match occurred on the last received packet          */
  __IO uint32_t  EVENTS_RSSIEND;                    /*!< Sampling of receive signal strength complete.                         */
  __I  uint32_t  RESERVED1[2];
  __IO uint32_t  EVENTS_BCMATCH;                    /*!< Bit counter reached bit count value.                                  */
  __I  uint32_t  RESERVED2;
  __IO uint32_t  EVENTS_CRCOK;                      /*!< Packet received with CRC ok                                           */
  __IO uint32_t  EVENTS_CRCERROR;                   /*!< Packet received with CRC error                                        */
  __IO uint32_t  EVENTS_FRAMESTART;                 /*!< IEEE 802.15.4 length field received                                   */
  __IO uint32_t  EVENTS_EDEND;                      /*!< Sampling of Energy Detection complete. A new ED sample is ready
                                                         for readout from the RADIO.EDSAMPLE register                          */
  __IO uint32_t  EVENTS_EDSTOPPED;                  /*!< The sampling of Energy Detection has stopped                          */
  __IO uint32_t  EVENTS_CCAIDLE;                    /*!< Wireless medium in idle - clear to send                               */
  __IO uint32_t  EVENTS_CCABUSY;                    /*!< Wireless medium busy - do not send                                    */
  __IO uint32_t  EVENTS_CCASTOPPED;                 /*!< The CCA has stopped                                                   */
  __IO uint32_t  EVENTS_RATEBOOST;                  /*!< Ble_LR CI field received, receive mode is changed from Ble_LR125Kbit
                                                         to Ble_LR500Kbit.                                                     */
  __IO uint32_t  EVENTS_TXREADY;                    /*!< RADIO has ramped up and is ready to be started TX path                */
  __IO uint32_t  EVENTS_RXREADY;                    /*!< RADIO has ramped up and is ready to be started RX path                */
  __IO uint32_t  EVENTS_MHRMATCH;                   /*!< MAC Header match found.                                               */
  __I  uint32_t  RESERVED3[40];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED4[64];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED5[61];
  __I  uint32_t  CRCSTATUS;                         /*!< CRC status                                                            */
  __I  uint32_t  RESERVED6;
  __I  uint32_t  RXMATCH;                           /*!< Received address                                                      */
  __I  uint32_t  RXCRC;                             /*!< CRC field of previously received packet                               */
  __I  uint32_t  DAI;                               /*!< Device address match index                                            */
  __I  uint32_t  RESERVED7[60];
  __IO uint32_t  PACKETPTR;                         /*!< Packet pointer                                                        */
  __IO uint32_t  FREQUENCY;                         /*!< Frequency                                                             */
  __IO uint32_t  TXPOWER;                           /*!< Output power                                                          */
  __IO uint32_t  MODE;                              /*!< Data rate and modulation                                              */
  __IO uint32_t  PCNF0;                             /*!< Packet configuration register 0                                       */
  __IO uint32_t  PCNF1;                             /*!< Packet configuration register 1                                       */
  __IO uint32_t  BASE0;                             /*!< Base address 0                                                        */
  __IO uint32_t  BASE1;                             /*!< Base address 1                                                        */
  __IO uint32_t  PREFIX0;                           /*!< Prefixes bytes for logical addresses 0-3                              */
  __IO uint32_t  PREFIX1;                           /*!< Prefixes bytes for logical addresses 4-7                              */
  __IO uint32_t  TXADDRESS;                         /*!< Transmit address select                                               */
  __IO uint32_t  RXADDRESSES;                       /*!< Receive address select                                                */
  __IO uint32_t  CRCCNF;                            /*!< CRC configuration                                                     */
  __IO uint32_t  CRCPOLY;                           /*!< CRC polynomial                                                        */
  __IO uint32_t  CRCINIT;                           /*!< CRC initial value                                                     */
  __I  uint32_t  RESERVED8;
  __IO uint32_t  TIFS;                              /*!< Inter Frame Spacing in us                                             */
  __I  uint32_t  RSSISAMPLE;                        /*!< RSSI sample                                                           */
  __I  uint32_t  RESERVED9;
  __I  uint32_t  STATE;                             /*!< Current radio state                                                   */
  __IO uint32_t  DATAWHITEIV;                       /*!< Data whitening initial value                                          */
  __I  uint32_t  RESERVED10[2];
  __IO uint32_t  BCC;                               /*!< Bit counter compare                                                   */
  __I  uint32_t  RESERVED11[39];
  __IO uint32_t  DAB[8];                            /*!< Description collection[0]: Device address base segment 0              */
  __IO uint32_t  DAP[8];                            /*!< Description collection[0]: Device address prefix 0                    */
  __IO uint32_t  DACNF;                             /*!< Device address match configuration                                    */
  __IO uint32_t  MHRMATCHCONF;                      /*!< Search Pattern Configuration                                          */
  __IO uint32_t  MHRMATCHMAS;                       /*!< Pattern mask                                                          */
  __I  uint32_t  RESERVED12;
  __IO uint32_t  MODECNF0;                          /*!< Radio mode configuration register 0                                   */
  __I  uint32_t  RESERVED13[3];
  __IO uint32_t  SFD;                               /*!< IEEE 802.15.4 Start of Frame Delimiter                                */
  __IO uint32_t  EDCNT;                             /*!< IEEE 802.15.4 Energy Detect Loop Count                                */
  __IO uint32_t  EDSAMPLE;                          /*!< IEEE 802.15.4 Energy Detect Level                                     */
  __IO uint32_t  CCACTRL;                           /*!< IEEE 802.15.4 Clear Channel Assessment Control                        */
  __I  uint32_t  RESERVED14[611];
  __IO uint32_t  POWER;                             /*!< Peripheral power control                                              */
} NRF_RADIO_Type;


/* ================================================================================ */
/* ================                      UARTE                     ================ */
/* ================================================================================ */


/**
  * @brief UART with EasyDMA 0 (UARTE)
  */

typedef struct {                                    /*!< UARTE Structure                                                       */
  __O  uint32_t  TASKS_STARTRX;                     /*!< Start UART receiver                                                   */
  __O  uint32_t  TASKS_STOPRX;                      /*!< Stop UART receiver                                                    */
  __O  uint32_t  TASKS_STARTTX;                     /*!< Start UART transmitter                                                */
  __O  uint32_t  TASKS_STOPTX;                      /*!< Stop UART transmitter                                                 */
  __I  uint32_t  RESERVED0[7];
  __O  uint32_t  TASKS_FLUSHRX;                     /*!< Flush RX FIFO into RX buffer                                          */
  __I  uint32_t  RESERVED1[52];
  __IO uint32_t  EVENTS_CTS;                        /*!< CTS is activated (set low). Clear To Send.                            */
  __IO uint32_t  EVENTS_NCTS;                       /*!< CTS is deactivated (set high). Not Clear To Send.                     */
  __IO uint32_t  EVENTS_RXDRDY;                     /*!< Data received in RXD (but potentially not yet transferred to
                                                         Data RAM)                                                             */
  __I  uint32_t  RESERVED2;
  __IO uint32_t  EVENTS_ENDRX;                      /*!< Receive buffer is filled up                                           */
  __I  uint32_t  RESERVED3[2];
  __IO uint32_t  EVENTS_TXDRDY;                     /*!< Data sent from TXD                                                    */
  __IO uint32_t  EVENTS_ENDTX;                      /*!< Last TX byte transmitted                                              */
  __IO uint32_t  EVENTS_ERROR;                      /*!< Error detected                                                        */
  __I  uint32_t  RESERVED4[7];
  __IO uint32_t  EVENTS_RXTO;                       /*!< Receiver timeout                                                      */
  __I  uint32_t  RESERVED5;
  __IO uint32_t  EVENTS_RXSTARTED;                  /*!< UART receiver has started                                             */
  __IO uint32_t  EVENTS_TXSTARTED;                  /*!< UART transmitter has started                                          */
  __I  uint32_t  RESERVED6;
  __IO uint32_t  EVENTS_TXSTOPPED;                  /*!< Transmitter stopped                                                   */
  __I  uint32_t  RESERVED7[41];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED8[63];
  __IO uint32_t  INTEN;                             /*!< Enable or disable interrupt                                           */
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED9[93];
  __IO uint32_t  ERRORSRC;                          /*!< Error source Note : this register is read / write one to clear.       */
  __I  uint32_t  RESERVED10[31];
  __IO uint32_t  ENABLE;                            /*!< Enable UART                                                           */
  __I  uint32_t  RESERVED11;
  UARTE_PSEL_Type PSEL;                             /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED12[3];
  __IO uint32_t  BAUDRATE;                          /*!< Baud rate. Accuracy depends on the HFCLK source selected.             */
  __I  uint32_t  RESERVED13[3];
  UARTE_RXD_Type RXD;                               /*!< RXD EasyDMA channel                                                   */
  __I  uint32_t  RESERVED14;
  UARTE_TXD_Type TXD;                               /*!< TXD EasyDMA channel                                                   */
  __I  uint32_t  RESERVED15[7];
  __IO uint32_t  CONFIG;                            /*!< Configuration of parity and hardware flow control                     */
} NRF_UARTE_Type;


/* ================================================================================ */
/* ================                      UART                      ================ */
/* ================================================================================ */


/**
  * @brief Universal Asynchronous Receiver/Transmitter (UART)
  */

typedef struct {                                    /*!< UART Structure                                                        */
  __O  uint32_t  TASKS_STARTRX;                     /*!< Start UART receiver                                                   */
  __O  uint32_t  TASKS_STOPRX;                      /*!< Stop UART receiver                                                    */
  __O  uint32_t  TASKS_STARTTX;                     /*!< Start UART transmitter                                                */
  __O  uint32_t  TASKS_STOPTX;                      /*!< Stop UART transmitter                                                 */
  __I  uint32_t  RESERVED0[3];
  __O  uint32_t  TASKS_SUSPEND;                     /*!< Suspend UART                                                          */
  __I  uint32_t  RESERVED1[56];
  __IO uint32_t  EVENTS_CTS;                        /*!< CTS is activated (set low). Clear To Send.                            */
  __IO uint32_t  EVENTS_NCTS;                       /*!< CTS is deactivated (set high). Not Clear To Send.                     */
  __IO uint32_t  EVENTS_RXDRDY;                     /*!< Data received in RXD                                                  */
  __I  uint32_t  RESERVED2[4];
  __IO uint32_t  EVENTS_TXDRDY;                     /*!< Data sent from TXD                                                    */
  __I  uint32_t  RESERVED3;
  __IO uint32_t  EVENTS_ERROR;                      /*!< Error detected                                                        */
  __I  uint32_t  RESERVED4[7];
  __IO uint32_t  EVENTS_RXTO;                       /*!< Receiver timeout                                                      */
  __I  uint32_t  RESERVED5[46];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED6[64];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED7[93];
  __IO uint32_t  ERRORSRC;                          /*!< Error source                                                          */
  __I  uint32_t  RESERVED8[31];
  __IO uint32_t  ENABLE;                            /*!< Enable UART                                                           */
  __I  uint32_t  RESERVED9;
  UART_PSEL_Type PSEL;                              /*!< Unspecified                                                           */
  __I  uint32_t  RXD;                               /*!< RXD register                                                          */
  __O  uint32_t  TXD;                               /*!< TXD register                                                          */
  __I  uint32_t  RESERVED10;
  __IO uint32_t  BAUDRATE;                          /*!< Baud rate. Accuracy depends on the HFCLK source selected.             */
  __I  uint32_t  RESERVED11[17];
  __IO uint32_t  CONFIG;                            /*!< Configuration of parity and hardware flow control                     */
} NRF_UART_Type;


/* ================================================================================ */
/* ================                      SPIM                      ================ */
/* ================================================================================ */


/**
  * @brief Serial Peripheral Interface Master with EasyDMA 0 (SPIM)
  */

typedef struct {                                    /*!< SPIM Structure                                                        */
  __I  uint32_t  RESERVED0[4];
  __O  uint32_t  TASKS_START;                       /*!< Start SPI transaction                                                 */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop SPI transaction                                                  */
  __I  uint32_t  RESERVED1;
  __O  uint32_t  TASKS_SUSPEND;                     /*!< Suspend SPI transaction                                               */
  __O  uint32_t  TASKS_RESUME;                      /*!< Resume SPI transaction                                                */
  __I  uint32_t  RESERVED2[56];
  __IO uint32_t  EVENTS_STOPPED;                    /*!< SPI transaction has stopped                                           */
  __I  uint32_t  RESERVED3[2];
  __IO uint32_t  EVENTS_ENDRX;                      /*!< End of RXD buffer reached                                             */
  __I  uint32_t  RESERVED4;
  __IO uint32_t  EVENTS_END;                        /*!< End of RXD buffer and TXD buffer reached                              */
  __I  uint32_t  RESERVED5;
  __IO uint32_t  EVENTS_ENDTX;                      /*!< End of TXD buffer reached                                             */
  __I  uint32_t  RESERVED6[10];
  __IO uint32_t  EVENTS_STARTED;                    /*!< Transaction started                                                   */
  __I  uint32_t  RESERVED7[44];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED8[64];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED9[61];
  __IO uint32_t  STALLSTAT;                         /*!< Stall status for EasyDMA RAM accesses. The fields in this register
                                                         is set to STALL by hardware whenever a stall occurres and can
                                                          be cleared (set to NOSTALL) by the CPU.                              */
  __I  uint32_t  RESERVED10[63];
  __IO uint32_t  ENABLE;                            /*!< Enable SPIM                                                           */
  __I  uint32_t  RESERVED11;
  SPIM_PSEL_Type PSEL;                              /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED12[3];
  __IO uint32_t  FREQUENCY;                         /*!< SPI frequency. Accuracy depends on the HFCLK source selected.         */
  __I  uint32_t  RESERVED13[3];
  SPIM_RXD_Type RXD;                                /*!< RXD EasyDMA channel                                                   */
  SPIM_TXD_Type TXD;                                /*!< TXD EasyDMA channel                                                   */
  __IO uint32_t  CONFIG;                            /*!< Configuration register                                                */
  __I  uint32_t  RESERVED14[2];
  SPIM_IFTIMING_Type IFTIMING;                      /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED15[22];
  __IO uint32_t  ORC;                               /*!< Byte transmitted after TXD.MAXCNT bytes have been transmitted
                                                         in the case when RXD.MAXCNT is greater than TXD.MAXCNT                */
} NRF_SPIM_Type;


/* ================================================================================ */
/* ================                      SPIS                      ================ */
/* ================================================================================ */


/**
  * @brief SPI Slave 0 (SPIS)
  */

typedef struct {                                    /*!< SPIS Structure                                                        */
  __I  uint32_t  RESERVED0[9];
  __O  uint32_t  TASKS_ACQUIRE;                     /*!< Acquire SPI semaphore                                                 */
  __O  uint32_t  TASKS_RELEASE;                     /*!< Release SPI semaphore, enabling the SPI slave to acquire it           */
  __I  uint32_t  RESERVED1[54];
  __IO uint32_t  EVENTS_END;                        /*!< Granted transaction completed                                         */
  __I  uint32_t  RESERVED2[2];
  __IO uint32_t  EVENTS_ENDRX;                      /*!< End of RXD buffer reached                                             */
  __I  uint32_t  RESERVED3[5];
  __IO uint32_t  EVENTS_ACQUIRED;                   /*!< Semaphore acquired                                                    */
  __I  uint32_t  RESERVED4[53];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED5[64];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED6[61];
  __I  uint32_t  SEMSTAT;                           /*!< Semaphore status register                                             */
  __I  uint32_t  RESERVED7[15];
  __IO uint32_t  STATUS;                            /*!< Status from last transaction                                          */
  __I  uint32_t  RESERVED8[47];
  __IO uint32_t  ENABLE;                            /*!< Enable SPI slave                                                      */
  __I  uint32_t  RESERVED9;
  SPIS_PSEL_Type PSEL;                              /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED10[7];
  SPIS_RXD_Type RXD;                                /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED11;
  SPIS_TXD_Type TXD;                                /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED12;
  __IO uint32_t  CONFIG;                            /*!< Configuration register                                                */
  __I  uint32_t  RESERVED13;
  __IO uint32_t  DEF;                               /*!< Default character. Character clocked out in case of an ignored
                                                         transaction.                                                          */
  __I  uint32_t  RESERVED14[24];
  __IO uint32_t  ORC;                               /*!< Over-read character                                                   */
} NRF_SPIS_Type;


/* ================================================================================ */
/* ================                      TWIM                      ================ */
/* ================================================================================ */


/**
  * @brief I2C compatible Two-Wire Master Interface with EasyDMA 0 (TWIM)
  */

typedef struct {                                    /*!< TWIM Structure                                                        */
  __O  uint32_t  TASKS_STARTRX;                     /*!< Start TWI receive sequence                                            */
  __I  uint32_t  RESERVED0;
  __O  uint32_t  TASKS_STARTTX;                     /*!< Start TWI transmit sequence                                           */
  __I  uint32_t  RESERVED1[2];
  __O  uint32_t  TASKS_STOP;                        /*!< Stop TWI transaction. Must be issued while the TWI master is
                                                         not suspended.                                                        */
  __I  uint32_t  RESERVED2;
  __O  uint32_t  TASKS_SUSPEND;                     /*!< Suspend TWI transaction                                               */
  __O  uint32_t  TASKS_RESUME;                      /*!< Resume TWI transaction                                                */
  __I  uint32_t  RESERVED3[56];
  __IO uint32_t  EVENTS_STOPPED;                    /*!< TWI stopped                                                           */
  __I  uint32_t  RESERVED4[7];
  __IO uint32_t  EVENTS_ERROR;                      /*!< TWI error                                                             */
  __I  uint32_t  RESERVED5[8];
  __IO uint32_t  EVENTS_SUSPENDED;                  /*!< Last byte has been sent out after the SUSPEND task has been
                                                         issued, TWI traffic is now suspended.                                 */
  __IO uint32_t  EVENTS_RXSTARTED;                  /*!< Receive sequence started                                              */
  __IO uint32_t  EVENTS_TXSTARTED;                  /*!< Transmit sequence started                                             */
  __I  uint32_t  RESERVED6[2];
  __IO uint32_t  EVENTS_LASTRX;                     /*!< Byte boundary, starting to receive the last byte                      */
  __IO uint32_t  EVENTS_LASTTX;                     /*!< Byte boundary, starting to transmit the last byte                     */
  __I  uint32_t  RESERVED7[39];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED8[63];
  __IO uint32_t  INTEN;                             /*!< Enable or disable interrupt                                           */
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED9[110];
  __IO uint32_t  ERRORSRC;                          /*!< Error source                                                          */
  __I  uint32_t  RESERVED10[14];
  __IO uint32_t  ENABLE;                            /*!< Enable TWIM                                                           */
  __I  uint32_t  RESERVED11;
  TWIM_PSEL_Type PSEL;                              /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED12[5];
  __IO uint32_t  FREQUENCY;                         /*!< TWI frequency. Accuracy depends on the HFCLK source selected.         */
  __I  uint32_t  RESERVED13[3];
  TWIM_RXD_Type RXD;                                /*!< RXD EasyDMA channel                                                   */
  TWIM_TXD_Type TXD;                                /*!< TXD EasyDMA channel                                                   */
  __I  uint32_t  RESERVED14[13];
  __IO uint32_t  ADDRESS;                           /*!< Address used in the TWI transfer                                      */
} NRF_TWIM_Type;


/* ================================================================================ */
/* ================                      TWIS                      ================ */
/* ================================================================================ */


/**
  * @brief I2C compatible Two-Wire Slave Interface with EasyDMA 0 (TWIS)
  */

typedef struct {                                    /*!< TWIS Structure                                                        */
  __I  uint32_t  RESERVED0[5];
  __O  uint32_t  TASKS_STOP;                        /*!< Stop TWI transaction                                                  */
  __I  uint32_t  RESERVED1;
  __O  uint32_t  TASKS_SUSPEND;                     /*!< Suspend TWI transaction                                               */
  __O  uint32_t  TASKS_RESUME;                      /*!< Resume TWI transaction                                                */
  __I  uint32_t  RESERVED2[3];
  __O  uint32_t  TASKS_PREPARERX;                   /*!< Prepare the TWI slave to respond to a write command                   */
  __O  uint32_t  TASKS_PREPARETX;                   /*!< Prepare the TWI slave to respond to a read command                    */
  __I  uint32_t  RESERVED3[51];
  __IO uint32_t  EVENTS_STOPPED;                    /*!< TWI stopped                                                           */
  __I  uint32_t  RESERVED4[7];
  __IO uint32_t  EVENTS_ERROR;                      /*!< TWI error                                                             */
  __I  uint32_t  RESERVED5[9];
  __IO uint32_t  EVENTS_RXSTARTED;                  /*!< Receive sequence started                                              */
  __IO uint32_t  EVENTS_TXSTARTED;                  /*!< Transmit sequence started                                             */
  __I  uint32_t  RESERVED6[4];
  __IO uint32_t  EVENTS_WRITE;                      /*!< Write command received                                                */
  __IO uint32_t  EVENTS_READ;                       /*!< Read command received                                                 */
  __I  uint32_t  RESERVED7[37];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED8[63];
  __IO uint32_t  INTEN;                             /*!< Enable or disable interrupt                                           */
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED9[113];
  __IO uint32_t  ERRORSRC;                          /*!< Error source                                                          */
  __I  uint32_t  MATCH;                             /*!< Status register indicating which address had a match                  */
  __I  uint32_t  RESERVED10[10];
  __IO uint32_t  ENABLE;                            /*!< Enable TWIS                                                           */
  __I  uint32_t  RESERVED11;
  TWIS_PSEL_Type PSEL;                              /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED12[9];
  TWIS_RXD_Type RXD;                                /*!< RXD EasyDMA channel                                                   */
  __I  uint32_t  RESERVED13;
  TWIS_TXD_Type TXD;                                /*!< TXD EasyDMA channel                                                   */
  __I  uint32_t  RESERVED14[14];
  __IO uint32_t  ADDRESS[2];                        /*!< Description collection[0]: TWI slave address 0                        */
  __I  uint32_t  RESERVED15;
  __IO uint32_t  CONFIG;                            /*!< Configuration register for the address match mechanism                */
  __I  uint32_t  RESERVED16[10];
  __IO uint32_t  ORC;                               /*!< Over-read character. Character sent out in case of an over-read
                                                         of the transmit buffer.                                               */
} NRF_TWIS_Type;


/* ================================================================================ */
/* ================                       SPI                      ================ */
/* ================================================================================ */


/**
  * @brief Serial Peripheral Interface 0 (SPI)
  */

typedef struct {                                    /*!< SPI Structure                                                         */
  __I  uint32_t  RESERVED0[66];
  __IO uint32_t  EVENTS_READY;                      /*!< TXD byte sent and RXD byte received                                   */
  __I  uint32_t  RESERVED1[126];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED2[125];
  __IO uint32_t  ENABLE;                            /*!< Enable SPI                                                            */
  __I  uint32_t  RESERVED3;
  SPI_PSEL_Type PSEL;                               /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED4;
  __I  uint32_t  RXD;                               /*!< RXD register                                                          */
  __IO uint32_t  TXD;                               /*!< TXD register                                                          */
  __I  uint32_t  RESERVED5;
  __IO uint32_t  FREQUENCY;                         /*!< SPI frequency. Accuracy depends on the HFCLK source selected.         */
  __I  uint32_t  RESERVED6[11];
  __IO uint32_t  CONFIG;                            /*!< Configuration register                                                */
} NRF_SPI_Type;


/* ================================================================================ */
/* ================                       TWI                      ================ */
/* ================================================================================ */


/**
  * @brief I2C compatible Two-Wire Interface 0 (TWI)
  */

typedef struct {                                    /*!< TWI Structure                                                         */
  __O  uint32_t  TASKS_STARTRX;                     /*!< Start TWI receive sequence                                            */
  __I  uint32_t  RESERVED0;
  __O  uint32_t  TASKS_STARTTX;                     /*!< Start TWI transmit sequence                                           */
  __I  uint32_t  RESERVED1[2];
  __O  uint32_t  TASKS_STOP;                        /*!< Stop TWI transaction                                                  */
  __I  uint32_t  RESERVED2;
  __O  uint32_t  TASKS_SUSPEND;                     /*!< Suspend TWI transaction                                               */
  __O  uint32_t  TASKS_RESUME;                      /*!< Resume TWI transaction                                                */
  __I  uint32_t  RESERVED3[56];
  __IO uint32_t  EVENTS_STOPPED;                    /*!< TWI stopped                                                           */
  __IO uint32_t  EVENTS_RXDREADY;                   /*!< TWI RXD byte received                                                 */
  __I  uint32_t  RESERVED4[4];
  __IO uint32_t  EVENTS_TXDSENT;                    /*!< TWI TXD byte sent                                                     */
  __I  uint32_t  RESERVED5;
  __IO uint32_t  EVENTS_ERROR;                      /*!< TWI error                                                             */
  __I  uint32_t  RESERVED6[4];
  __IO uint32_t  EVENTS_BB;                         /*!< TWI byte boundary, generated before each byte that is sent or
                                                         received                                                              */
  __I  uint32_t  RESERVED7[3];
  __IO uint32_t  EVENTS_SUSPENDED;                  /*!< TWI entered the suspended state                                       */
  __I  uint32_t  RESERVED8[45];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED9[64];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED10[110];
  __IO uint32_t  ERRORSRC;                          /*!< Error source                                                          */
  __I  uint32_t  RESERVED11[14];
  __IO uint32_t  ENABLE;                            /*!< Enable TWI                                                            */
  __I  uint32_t  RESERVED12;
  TWI_PSEL_Type PSEL;                               /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED13[2];
  __I  uint32_t  RXD;                               /*!< RXD register                                                          */
  __IO uint32_t  TXD;                               /*!< TXD register                                                          */
  __I  uint32_t  RESERVED14;
  __IO uint32_t  FREQUENCY;                         /*!< TWI frequency. Accuracy depends on the HFCLK source selected.         */
  __I  uint32_t  RESERVED15[24];
  __IO uint32_t  ADDRESS;                           /*!< Address used in the TWI transfer                                      */
} NRF_TWI_Type;


/* ================================================================================ */
/* ================                      NFCT                      ================ */
/* ================================================================================ */


/**
  * @brief NFC-A compatible radio (NFCT)
  */

typedef struct {                                    /*!< NFCT Structure                                                        */
  __O  uint32_t  TASKS_ACTIVATE;                    /*!< Activate NFCT peripheral for incoming and outgoing frames, change
                                                         state to activated                                                    */
  __O  uint32_t  TASKS_DISABLE;                     /*!< Disable NFCT peripheral                                               */
  __O  uint32_t  TASKS_SENSE;                       /*!< Enable NFC sense field mode, change state to sense mode               */
  __O  uint32_t  TASKS_STARTTX;                     /*!< Start transmission of an outgoing frame, change state to transmit     */
  __I  uint32_t  RESERVED0[3];
  __O  uint32_t  TASKS_ENABLERXDATA;                /*!< Initializes the EasyDMA for receive.                                  */
  __I  uint32_t  RESERVED1;
  __O  uint32_t  TASKS_GOIDLE;                      /*!< Force state machine to IDLE state                                     */
  __O  uint32_t  TASKS_GOSLEEP;                     /*!< Force state machine to SLEEP_A state                                  */
  __I  uint32_t  RESERVED2[53];
  __IO uint32_t  EVENTS_READY;                      /*!< The NFCT peripheral is ready to receive and send frames               */
  __IO uint32_t  EVENTS_FIELDDETECTED;              /*!< Remote NFC field detected                                             */
  __IO uint32_t  EVENTS_FIELDLOST;                  /*!< Remote NFC field lost                                                 */
  __IO uint32_t  EVENTS_TXFRAMESTART;               /*!< Marks the start of the first symbol of a transmitted frame            */
  __IO uint32_t  EVENTS_TXFRAMEEND;                 /*!< Marks the end of the last transmitted on-air symbol of a frame        */
  __IO uint32_t  EVENTS_RXFRAMESTART;               /*!< Marks the end of the first symbol of a received frame                 */
  __IO uint32_t  EVENTS_RXFRAMEEND;                 /*!< Received data has been checked (CRC, parity) and transferred
                                                         to RAM, and EasyDMA has ended accessing the RX buffer                 */
  __IO uint32_t  EVENTS_ERROR;                      /*!< NFC error reported. The ERRORSTATUS register contains details
                                                         on the source of the error.                                           */
  __I  uint32_t  RESERVED3[2];
  __IO uint32_t  EVENTS_RXERROR;                    /*!< NFC RX frame error reported. The FRAMESTATUS.RX register contains
                                                         details on the source of the error.                                   */
  __IO uint32_t  EVENTS_ENDRX;                      /*!< RX buffer (as defined by PACKETPTR and MAXLEN) in Data RAM full.      */
  __IO uint32_t  EVENTS_ENDTX;                      /*!< Transmission of data in RAM has ended, and EasyDMA has ended
                                                         accessing the TX buffer                                               */
  __I  uint32_t  RESERVED4;
  __IO uint32_t  EVENTS_AUTOCOLRESSTARTED;          /*!< Auto collision resolution process has started                         */
  __I  uint32_t  RESERVED5[3];
  __IO uint32_t  EVENTS_COLLISION;                  /*!< NFC auto collision resolution error reported.                         */
  __IO uint32_t  EVENTS_SELECTED;                   /*!< NFC auto collision resolution successfully completed                  */
  __IO uint32_t  EVENTS_STARTED;                    /*!< EasyDMA is ready to receive or send frames.                           */
  __I  uint32_t  RESERVED6[43];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED7[63];
  __IO uint32_t  INTEN;                             /*!< Enable or disable interrupt                                           */
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED8[62];
  __IO uint32_t  ERRORSTATUS;                       /*!< NFC Error Status register                                             */
  __I  uint32_t  RESERVED9;
  NFCT_FRAMESTATUS_Type FRAMESTATUS;                /*!< Unspecified                                                           */
  __I  uint32_t  NFCTAGSTATE;                       /*!< NfcTag state register                                                 */
  __I  uint32_t  RESERVED10[10];
  __I  uint32_t  FIELDPRESENT;                      /*!< Indicates the presence or not of a valid field                        */
  __I  uint32_t  RESERVED11[49];
  __IO uint32_t  FRAMEDELAYMIN;                     /*!< Minimum frame delay                                                   */
  __IO uint32_t  FRAMEDELAYMAX;                     /*!< Maximum frame delay                                                   */
  __IO uint32_t  FRAMEDELAYMODE;                    /*!< Configuration register for the Frame Delay Timer                      */
  __IO uint32_t  PACKETPTR;                         /*!< Packet pointer for TXD and RXD data storage in Data RAM               */
  __IO uint32_t  MAXLEN;                            /*!< Size of the RAM buffer allocated to TXD and RXD data storage
                                                         each                                                                  */
  NFCT_TXD_Type TXD;                                /*!< Unspecified                                                           */
  NFCT_RXD_Type RXD;                                /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED12[26];
  __IO uint32_t  NFCID1_LAST;                       /*!< Last NFCID1 part (4, 7 or 10 bytes ID)                                */
  __IO uint32_t  NFCID1_2ND_LAST;                   /*!< Second last NFCID1 part (7 or 10 bytes ID)                            */
  __IO uint32_t  NFCID1_3RD_LAST;                   /*!< Third last NFCID1 part (10 bytes ID)                                  */
  __IO uint32_t  AUTOCOLRESCONFIG;                  /*!< Controls the auto collision resolution function. This setting
                                                         must be done before the NFCT peripheral is enabled.                   */
  __IO uint32_t  SENSRES;                           /*!< NFC-A SENS_RES auto-response settings                                 */
  __IO uint32_t  SELRES;                            /*!< NFC-A SEL_RES auto-response settings                                  */
} NRF_NFCT_Type;


/* ================================================================================ */
/* ================                     GPIOTE                     ================ */
/* ================================================================================ */


/**
  * @brief GPIO Tasks and Events (GPIOTE)
  */

typedef struct {                                    /*!< GPIOTE Structure                                                      */
  __O  uint32_t  TASKS_OUT[8];                      /*!< Description collection[0]: Task for writing to pin specified
                                                         in CONFIG[0].PSEL. Action on pin is configured in CONFIG[0].POLARITY. */
  __I  uint32_t  RESERVED0[4];
  __O  uint32_t  TASKS_SET[8];                      /*!< Description collection[0]: Task for writing to pin specified
                                                         in CONFIG[0].PSEL. Action on pin is to set it high.                   */
  __I  uint32_t  RESERVED1[4];
  __O  uint32_t  TASKS_CLR[8];                      /*!< Description collection[0]: Task for writing to pin specified
                                                         in CONFIG[0].PSEL. Action on pin is to set it low.                    */
  __I  uint32_t  RESERVED2[32];
  __IO uint32_t  EVENTS_IN[8];                      /*!< Description collection[0]: Event generated from pin specified
                                                         in CONFIG[0].PSEL                                                     */
  __I  uint32_t  RESERVED3[23];
  __IO uint32_t  EVENTS_PORT;                       /*!< Event generated from multiple input GPIO pins with SENSE mechanism
                                                         enabled                                                               */
  __I  uint32_t  RESERVED4[97];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED5[129];
  __IO uint32_t  CONFIG[8];                         /*!< Description collection[0]: Configuration for OUT[n], SET[n]
                                                         and CLR[n] tasks and IN[n] event                                      */
} NRF_GPIOTE_Type;


/* ================================================================================ */
/* ================                      SAADC                     ================ */
/* ================================================================================ */


/**
  * @brief Analog to Digital Converter (SAADC)
  */

typedef struct {                                    /*!< SAADC Structure                                                       */
  __O  uint32_t  TASKS_START;                       /*!< Start the ADC and prepare the result buffer in RAM                    */
  __O  uint32_t  TASKS_SAMPLE;                      /*!< Take one ADC sample, if scan is enabled all channels are sampled      */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop the ADC and terminate any on-going conversion                    */
  __O  uint32_t  TASKS_CALIBRATEOFFSET;             /*!< Starts offset auto-calibration                                        */
  __I  uint32_t  RESERVED0[60];
  __IO uint32_t  EVENTS_STARTED;                    /*!< The ADC has started                                                   */
  __IO uint32_t  EVENTS_END;                        /*!< The ADC has filled up the Result buffer                               */
  __IO uint32_t  EVENTS_DONE;                       /*!< A conversion task has been completed. Depending on the mode,
                                                         multiple conversions might be needed for a result to be transferred
                                                          to RAM.                                                              */
  __IO uint32_t  EVENTS_RESULTDONE;                 /*!< A result is ready to get transferred to RAM.                          */
  __IO uint32_t  EVENTS_CALIBRATEDONE;              /*!< Calibration is complete                                               */
  __IO uint32_t  EVENTS_STOPPED;                    /*!< The ADC has stopped                                                   */
  SAADC_EVENTS_CH_Type EVENTS_CH[8];                /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED1[106];
  __IO uint32_t  INTEN;                             /*!< Enable or disable interrupt                                           */
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED2[61];
  __I  uint32_t  STATUS;                            /*!< Status                                                                */
  __I  uint32_t  RESERVED3[63];
  __IO uint32_t  ENABLE;                            /*!< Enable or disable ADC                                                 */
  __I  uint32_t  RESERVED4[3];
  SAADC_CH_Type CH[8];                              /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED5[24];
  __IO uint32_t  RESOLUTION;                        /*!< Resolution configuration                                              */
  __IO uint32_t  OVERSAMPLE;                        /*!< Oversampling configuration. OVERSAMPLE should not be combined
                                                         with SCAN. The RESOLUTION is applied before averaging, thus
                                                          for high OVERSAMPLE a higher RESOLUTION should be used.              */
  __IO uint32_t  SAMPLERATE;                        /*!< Controls normal or continuous sample rate                             */
  __I  uint32_t  RESERVED6[12];
  SAADC_RESULT_Type RESULT;                         /*!< RESULT EasyDMA channel                                                */
} NRF_SAADC_Type;


/* ================================================================================ */
/* ================                      TIMER                     ================ */
/* ================================================================================ */


/**
  * @brief Timer/Counter 0 (TIMER)
  */

typedef struct {                                    /*!< TIMER Structure                                                       */
  __O  uint32_t  TASKS_START;                       /*!< Start Timer                                                           */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop Timer                                                            */
  __O  uint32_t  TASKS_COUNT;                       /*!< Increment Timer (Counter mode only)                                   */
  __O  uint32_t  TASKS_CLEAR;                       /*!< Clear time                                                            */
  __O  uint32_t  TASKS_SHUTDOWN;                    /*!< Deprecated register - Shut down timer                                 */
  __I  uint32_t  RESERVED0[11];
  __O  uint32_t  TASKS_CAPTURE[6];                  /*!< Description collection[0]: Capture Timer value to CC[0] register      */
  __I  uint32_t  RESERVED1[58];
  __IO uint32_t  EVENTS_COMPARE[6];                 /*!< Description collection[0]: Compare event on CC[0] match               */
  __I  uint32_t  RESERVED2[42];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED3[64];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED4[61];
  __I  uint32_t  STATUS;                            /*!< Timer status                                                          */
  __I  uint32_t  RESERVED5[64];
  __IO uint32_t  MODE;                              /*!< Timer mode selection                                                  */
  __IO uint32_t  BITMODE;                           /*!< Configure the number of bits used by the TIMER                        */
  __I  uint32_t  RESERVED6;
  __IO uint32_t  PRESCALER;                         /*!< Timer prescaler register                                              */
  __I  uint32_t  RESERVED7[11];
  __IO uint32_t  CC[6];                             /*!< Description collection[0]: Capture/Compare register 0                 */
} NRF_TIMER_Type;


/* ================================================================================ */
/* ================                       RTC                      ================ */
/* ================================================================================ */


/**
  * @brief Real time counter 0 (RTC)
  */

typedef struct {                                    /*!< RTC Structure                                                         */
  __O  uint32_t  TASKS_START;                       /*!< Start RTC COUNTER                                                     */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop RTC COUNTER                                                      */
  __O  uint32_t  TASKS_CLEAR;                       /*!< Clear RTC COUNTER                                                     */
  __O  uint32_t  TASKS_TRIGOVRFLW;                  /*!< Set COUNTER to 0xFFFFF0                                               */
  __I  uint32_t  RESERVED0[60];
  __IO uint32_t  EVENTS_TICK;                       /*!< Event on COUNTER increment                                            */
  __IO uint32_t  EVENTS_OVRFLW;                     /*!< Event on COUNTER overflow                                             */
  __I  uint32_t  RESERVED1[14];
  __IO uint32_t  EVENTS_COMPARE[4];                 /*!< Description collection[0]: Compare event on CC[0] match               */
  __I  uint32_t  RESERVED2[109];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED3[13];
  __IO uint32_t  EVTEN;                             /*!< Enable or disable event routing                                       */
  __IO uint32_t  EVTENSET;                          /*!< Enable event routing                                                  */
  __IO uint32_t  EVTENCLR;                          /*!< Disable event routing                                                 */
  __I  uint32_t  RESERVED4[110];
  __I  uint32_t  COUNTER;                           /*!< Current COUNTER value                                                 */
  __IO uint32_t  PRESCALER;                         /*!< 12 bit prescaler for COUNTER frequency (32768/(PRESCALER+1)).Must
                                                         be written when RTC is stopped                                        */
  __I  uint32_t  RESERVED5[13];
  __IO uint32_t  CC[4];                             /*!< Description collection[0]: Compare register 0                         */
} NRF_RTC_Type;


/* ================================================================================ */
/* ================                      TEMP                      ================ */
/* ================================================================================ */


/**
  * @brief Temperature Sensor (TEMP)
  */

typedef struct {                                    /*!< TEMP Structure                                                        */
  __O  uint32_t  TASKS_START;                       /*!< Start temperature measurement                                         */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop temperature measurement                                          */
  __I  uint32_t  RESERVED0[62];
  __IO uint32_t  EVENTS_DATARDY;                    /*!< Temperature measurement complete, data ready                          */
  __I  uint32_t  RESERVED1[128];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED2[127];
  __I  int32_t   TEMP;                              /*!< Temperature in degC (0.25deg steps)                                   */
  __I  uint32_t  RESERVED3[5];
  __IO uint32_t  A0;                                /*!< Slope of 1st piece wise linear function                               */
  __IO uint32_t  A1;                                /*!< Slope of 2nd piece wise linear function                               */
  __IO uint32_t  A2;                                /*!< Slope of 3rd piece wise linear function                               */
  __IO uint32_t  A3;                                /*!< Slope of 4th piece wise linear function                               */
  __IO uint32_t  A4;                                /*!< Slope of 5th piece wise linear function                               */
  __IO uint32_t  A5;                                /*!< Slope of 6th piece wise linear function                               */
  __I  uint32_t  RESERVED4[2];
  __IO uint32_t  B0;                                /*!< y-intercept of 1st piece wise linear function                         */
  __IO uint32_t  B1;                                /*!< y-intercept of 2nd piece wise linear function                         */
  __IO uint32_t  B2;                                /*!< y-intercept of 3rd piece wise linear function                         */
  __IO uint32_t  B3;                                /*!< y-intercept of 4th piece wise linear function                         */
  __IO uint32_t  B4;                                /*!< y-intercept of 5th piece wise linear function                         */
  __IO uint32_t  B5;                                /*!< y-intercept of 6th piece wise linear function                         */
  __I  uint32_t  RESERVED5[2];
  __IO uint32_t  T0;                                /*!< End point of 1st piece wise linear function                           */
  __IO uint32_t  T1;                                /*!< End point of 2nd piece wise linear function                           */
  __IO uint32_t  T2;                                /*!< End point of 3rd piece wise linear function                           */
  __IO uint32_t  T3;                                /*!< End point of 4th piece wise linear function                           */
  __IO uint32_t  T4;                                /*!< End point of 5th piece wise linear function                           */
} NRF_TEMP_Type;


/* ================================================================================ */
/* ================                       RNG                      ================ */
/* ================================================================================ */


/**
  * @brief Random Number Generator (RNG)
  */

typedef struct {                                    /*!< RNG Structure                                                         */
  __O  uint32_t  TASKS_START;                       /*!< Task starting the random number generator                             */
  __O  uint32_t  TASKS_STOP;                        /*!< Task stopping the random number generator                             */
  __I  uint32_t  RESERVED0[62];
  __IO uint32_t  EVENTS_VALRDY;                     /*!< Event being generated for every new random number written to
                                                         the VALUE register                                                    */
  __I  uint32_t  RESERVED1[63];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED2[64];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED3[126];
  __IO uint32_t  CONFIG;                            /*!< Configuration register                                                */
  __I  uint32_t  VALUE;                             /*!< Output random number                                                  */
} NRF_RNG_Type;


/* ================================================================================ */
/* ================                       ECB                      ================ */
/* ================================================================================ */


/**
  * @brief AES ECB Mode Encryption (ECB)
  */

typedef struct {                                    /*!< ECB Structure                                                         */
  __O  uint32_t  TASKS_STARTECB;                    /*!< Start ECB block encrypt                                               */
  __O  uint32_t  TASKS_STOPECB;                     /*!< Abort a possible executing ECB operation                              */
  __I  uint32_t  RESERVED0[62];
  __IO uint32_t  EVENTS_ENDECB;                     /*!< ECB block encrypt complete                                            */
  __IO uint32_t  EVENTS_ERRORECB;                   /*!< ECB block encrypt aborted because of a STOPECB task or due to
                                                         an error                                                              */
  __I  uint32_t  RESERVED1[127];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED2[126];
  __IO uint32_t  ECBDATAPTR;                        /*!< ECB block encrypt memory pointers                                     */
} NRF_ECB_Type;


/* ================================================================================ */
/* ================                       CCM                      ================ */
/* ================================================================================ */


/**
  * @brief AES CCM Mode Encryption (CCM)
  */

typedef struct {                                    /*!< CCM Structure                                                         */
  __O  uint32_t  TASKS_KSGEN;                       /*!< Start generation of key-stream. This operation will stop by
                                                         itself when completed.                                                */
  __O  uint32_t  TASKS_CRYPT;                       /*!< Start encryption/decryption. This operation will stop by itself
                                                         when completed.                                                       */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop encryption/decryption                                            */
  __O  uint32_t  TASKS_RATEOVERRIDE;                /*!< Override DATARATE setting in MODE register with the contents
                                                         of the RATEOVERRIDE register for any ongoing encryption/decryption    */
  __I  uint32_t  RESERVED0[60];
  __IO uint32_t  EVENTS_ENDKSGEN;                   /*!< Key-stream generation complete                                        */
  __IO uint32_t  EVENTS_ENDCRYPT;                   /*!< Encrypt/decrypt complete                                              */
  __IO uint32_t  EVENTS_ERROR;                      /*!< Deprecated register - CCM error event                                 */
  __I  uint32_t  RESERVED1[61];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED2[64];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED3[61];
  __I  uint32_t  MICSTATUS;                         /*!< MIC check result                                                      */
  __I  uint32_t  RESERVED4[63];
  __IO uint32_t  ENABLE;                            /*!< Enable                                                                */
  __IO uint32_t  MODE;                              /*!< Operation mode                                                        */
  __IO uint32_t  CNFPTR;                            /*!< Pointer to data structure holding AES key and NONCE vector            */
  __IO uint32_t  INPTR;                             /*!< Input pointer                                                         */
  __IO uint32_t  OUTPTR;                            /*!< Output pointer                                                        */
  __IO uint32_t  SCRATCHPTR;                        /*!< Pointer to data area used for temporary storage                       */
  __IO uint32_t  MAXPACKETSIZE;                     /*!< Length of key-stream generated when MODE.LENGTH = Extended.           */
  __IO uint32_t  RATEOVERRIDE;                      /*!< Data rate override setting.                                           */
} NRF_CCM_Type;


/* ================================================================================ */
/* ================                       AAR                      ================ */
/* ================================================================================ */


/**
  * @brief Accelerated Address Resolver (AAR)
  */

typedef struct {                                    /*!< AAR Structure                                                         */
  __O  uint32_t  TASKS_START;                       /*!< Start resolving addresses based on IRKs specified in the IRK
                                                         data structure                                                        */
  __I  uint32_t  RESERVED0;
  __O  uint32_t  TASKS_STOP;                        /*!< Stop resolving addresses                                              */
  __I  uint32_t  RESERVED1[61];
  __IO uint32_t  EVENTS_END;                        /*!< Address resolution procedure complete                                 */
  __IO uint32_t  EVENTS_RESOLVED;                   /*!< Address resolved                                                      */
  __IO uint32_t  EVENTS_NOTRESOLVED;                /*!< Address not resolved                                                  */
  __I  uint32_t  RESERVED2[126];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED3[61];
  __I  uint32_t  STATUS;                            /*!< Resolution status                                                     */
  __I  uint32_t  RESERVED4[63];
  __IO uint32_t  ENABLE;                            /*!< Enable AAR                                                            */
  __IO uint32_t  NIRK;                              /*!< Number of IRKs                                                        */
  __IO uint32_t  IRKPTR;                            /*!< Pointer to IRK data structure                                         */
  __I  uint32_t  RESERVED5;
  __IO uint32_t  ADDRPTR;                           /*!< Pointer to the resolvable address                                     */
  __IO uint32_t  SCRATCHPTR;                        /*!< Pointer to data area used for temporary storage                       */
} NRF_AAR_Type;


/* ================================================================================ */
/* ================                       WDT                      ================ */
/* ================================================================================ */


/**
  * @brief Watchdog Timer (WDT)
  */

typedef struct {                                    /*!< WDT Structure                                                         */
  __O  uint32_t  TASKS_START;                       /*!< Start the watchdog                                                    */
  __I  uint32_t  RESERVED0[63];
  __IO uint32_t  EVENTS_TIMEOUT;                    /*!< Watchdog timeout                                                      */
  __I  uint32_t  RESERVED1[128];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED2[61];
  __I  uint32_t  RUNSTATUS;                         /*!< Run status                                                            */
  __I  uint32_t  REQSTATUS;                         /*!< Request status                                                        */
  __I  uint32_t  RESERVED3[63];
  __IO uint32_t  CRV;                               /*!< Counter reload value                                                  */
  __IO uint32_t  RREN;                              /*!< Enable register for reload request registers                          */
  __IO uint32_t  CONFIG;                            /*!< Configuration register                                                */
  __I  uint32_t  RESERVED4[60];
  __O  uint32_t  RR[8];                             /*!< Description collection[0]: Reload request 0                           */
} NRF_WDT_Type;


/* ================================================================================ */
/* ================                      QDEC                      ================ */
/* ================================================================================ */


/**
  * @brief Quadrature Decoder (QDEC)
  */

typedef struct {                                    /*!< QDEC Structure                                                        */
  __O  uint32_t  TASKS_START;                       /*!< Task starting the quadrature decoder                                  */
  __O  uint32_t  TASKS_STOP;                        /*!< Task stopping the quadrature decoder                                  */
  __O  uint32_t  TASKS_READCLRACC;                  /*!< Read and clear ACC and ACCDBL                                         */
  __O  uint32_t  TASKS_RDCLRACC;                    /*!< Read and clear ACC                                                    */
  __O  uint32_t  TASKS_RDCLRDBL;                    /*!< Read and clear ACCDBL                                                 */
  __I  uint32_t  RESERVED0[59];
  __IO uint32_t  EVENTS_SAMPLERDY;                  /*!< Event being generated for every new sample value written to
                                                         the SAMPLE register                                                   */
  __IO uint32_t  EVENTS_REPORTRDY;                  /*!< Non-null report ready                                                 */
  __IO uint32_t  EVENTS_ACCOF;                      /*!< ACC or ACCDBL register overflow                                       */
  __IO uint32_t  EVENTS_DBLRDY;                     /*!< Double displacement(s) detected                                       */
  __IO uint32_t  EVENTS_STOPPED;                    /*!< QDEC has been stopped                                                 */
  __I  uint32_t  RESERVED1[59];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED2[64];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED3[125];
  __IO uint32_t  ENABLE;                            /*!< Enable the quadrature decoder                                         */
  __IO uint32_t  LEDPOL;                            /*!< LED output pin polarity                                               */
  __IO uint32_t  SAMPLEPER;                         /*!< Sample period                                                         */
  __I  int32_t   SAMPLE;                            /*!< Motion sample value                                                   */
  __IO uint32_t  REPORTPER;                         /*!< Number of samples to be taken before REPORTRDY and DBLRDY events
                                                         can be generated                                                      */
  __I  int32_t   ACC;                               /*!< Register accumulating the valid transitions                           */
  __I  int32_t   ACCREAD;                           /*!< Snapshot of the ACC register, updated by the READCLRACC or RDCLRACC
                                                         task                                                                  */
  QDEC_PSEL_Type PSEL;                              /*!< Unspecified                                                           */
  __IO uint32_t  DBFEN;                             /*!< Enable input debounce filters                                         */
  __I  uint32_t  RESERVED4[5];
  __IO uint32_t  LEDPRE;                            /*!< Time period the LED is switched ON prior to sampling                  */
  __I  uint32_t  ACCDBL;                            /*!< Register accumulating the number of detected double transitions       */
  __I  uint32_t  ACCDBLREAD;                        /*!< Snapshot of the ACCDBL, updated by the READCLRACC or RDCLRDBL
                                                         task                                                                  */
} NRF_QDEC_Type;


/* ================================================================================ */
/* ================                      COMP                      ================ */
/* ================================================================================ */


/**
  * @brief Comparator (COMP)
  */

typedef struct {                                    /*!< COMP Structure                                                        */
  __O  uint32_t  TASKS_START;                       /*!< Start comparator                                                      */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop comparator                                                       */
  __O  uint32_t  TASKS_SAMPLE;                      /*!< Sample comparator value                                               */
  __I  uint32_t  RESERVED0[61];
  __IO uint32_t  EVENTS_READY;                      /*!< COMP is ready and output is valid                                     */
  __IO uint32_t  EVENTS_DOWN;                       /*!< Downward crossing                                                     */
  __IO uint32_t  EVENTS_UP;                         /*!< Upward crossing                                                       */
  __IO uint32_t  EVENTS_CROSS;                      /*!< Downward or upward crossing                                           */
  __I  uint32_t  RESERVED1[60];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED2[63];
  __IO uint32_t  INTEN;                             /*!< Enable or disable interrupt                                           */
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED3[61];
  __I  uint32_t  RESULT;                            /*!< Compare result                                                        */
  __I  uint32_t  RESERVED4[63];
  __IO uint32_t  ENABLE;                            /*!< COMP enable                                                           */
  __IO uint32_t  PSEL;                              /*!< Pin select                                                            */
  __IO uint32_t  REFSEL;                            /*!< Reference source select                                               */
  __IO uint32_t  EXTREFSEL;                         /*!< External reference select                                             */
  __I  uint32_t  RESERVED5[8];
  __IO uint32_t  TH;                                /*!< Threshold configuration for hysteresis unit                           */
  __IO uint32_t  MODE;                              /*!< Mode configuration                                                    */
  __IO uint32_t  HYST;                              /*!< Comparator hysteresis enable                                          */
  __IO uint32_t  ISOURCE;                           /*!< Current source select on analog input                                 */
} NRF_COMP_Type;


/* ================================================================================ */
/* ================                     LPCOMP                     ================ */
/* ================================================================================ */


/**
  * @brief Low Power Comparator (LPCOMP)
  */

typedef struct {                                    /*!< LPCOMP Structure                                                      */
  __O  uint32_t  TASKS_START;                       /*!< Start comparator                                                      */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop comparator                                                       */
  __O  uint32_t  TASKS_SAMPLE;                      /*!< Sample comparator value                                               */
  __I  uint32_t  RESERVED0[61];
  __IO uint32_t  EVENTS_READY;                      /*!< LPCOMP is ready and output is valid                                   */
  __IO uint32_t  EVENTS_DOWN;                       /*!< Downward crossing                                                     */
  __IO uint32_t  EVENTS_UP;                         /*!< Upward crossing                                                       */
  __IO uint32_t  EVENTS_CROSS;                      /*!< Downward or upward crossing                                           */
  __I  uint32_t  RESERVED1[60];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED2[64];
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED3[61];
  __I  uint32_t  RESULT;                            /*!< Compare result                                                        */
  __I  uint32_t  RESERVED4[63];
  __IO uint32_t  ENABLE;                            /*!< Enable LPCOMP                                                         */
  __IO uint32_t  PSEL;                              /*!< Input pin select                                                      */
  __IO uint32_t  REFSEL;                            /*!< Reference select                                                      */
  __IO uint32_t  EXTREFSEL;                         /*!< External reference select                                             */
  __I  uint32_t  RESERVED5[4];
  __IO uint32_t  ANADETECT;                         /*!< Analog detect configuration                                           */
  __I  uint32_t  RESERVED6[5];
  __IO uint32_t  HYST;                              /*!< Comparator hysteresis enable                                          */
} NRF_LPCOMP_Type;


/* ================================================================================ */
/* ================                       SWI                      ================ */
/* ================================================================================ */


/**
  * @brief Software interrupt 0 (SWI)
  */

typedef struct {                                    /*!< SWI Structure                                                         */
  __I  uint32_t  UNUSED;                            /*!< Unused.                                                               */
} NRF_SWI_Type;


/* ================================================================================ */
/* ================                       EGU                      ================ */
/* ================================================================================ */


/**
  * @brief Event Generator Unit 0 (EGU)
  */

typedef struct {                                    /*!< EGU Structure                                                         */
  __O  uint32_t  TASKS_TRIGGER[16];                 /*!< Description collection[0]: Trigger 0 for triggering the corresponding
                                                         TRIGGERED[0] event                                                    */
  __I  uint32_t  RESERVED0[48];
  __IO uint32_t  EVENTS_TRIGGERED[16];              /*!< Description collection[0]: Event number 0 generated by triggering
                                                         the corresponding TRIGGER[0] task                                     */
  __I  uint32_t  RESERVED1[112];
  __IO uint32_t  INTEN;                             /*!< Enable or disable interrupt                                           */
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
} NRF_EGU_Type;


/* ================================================================================ */
/* ================                       PWM                      ================ */
/* ================================================================================ */


/**
  * @brief Pulse Width Modulation Unit 0 (PWM)
  */

typedef struct {                                    /*!< PWM Structure                                                         */
  __I  uint32_t  RESERVED0;
  __O  uint32_t  TASKS_STOP;                        /*!< Stops PWM pulse generation on all channels at the end of current
                                                         PWM period, and stops sequence playback                               */
  __O  uint32_t  TASKS_SEQSTART[2];                 /*!< Description collection[0]: Loads the first PWM value on all
                                                         enabled channels from sequence 0, and starts playing that sequence
                                                          at the rate defined in SEQ[0]REFRESH and/or DECODER.MODE. Causes
                                                          PWM generation to start it was not running.                          */
  __O  uint32_t  TASKS_NEXTSTEP;                    /*!< Steps by one value in the current sequence on all enabled channels
                                                         if DECODER.MODE=NextStep. Does not cause PWM generation to start
                                                          it was not running.                                                  */
  __I  uint32_t  RESERVED1[60];
  __IO uint32_t  EVENTS_STOPPED;                    /*!< Response to STOP task, emitted when PWM pulses are no longer
                                                         generated                                                             */
  __IO uint32_t  EVENTS_SEQSTARTED[2];              /*!< Description collection[0]: First PWM period started on sequence
                                                         0                                                                     */
  __IO uint32_t  EVENTS_SEQEND[2];                  /*!< Description collection[0]: Emitted at end of every sequence
                                                         0, when last value from RAM has been applied to wave counter          */
  __IO uint32_t  EVENTS_PWMPERIODEND;               /*!< Emitted at the end of each PWM period                                 */
  __IO uint32_t  EVENTS_LOOPSDONE;                  /*!< Concatenated sequences have been played the amount of times
                                                         defined in LOOP.CNT                                                   */
  __I  uint32_t  RESERVED2[56];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED3[63];
  __IO uint32_t  INTEN;                             /*!< Enable or disable interrupt                                           */
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED4[125];
  __IO uint32_t  ENABLE;                            /*!< PWM module enable register                                            */
  __IO uint32_t  MODE;                              /*!< Selects operating mode of the wave counter                            */
  __IO uint32_t  COUNTERTOP;                        /*!< Value up to which the pulse generator counter counts                  */
  __IO uint32_t  PRESCALER;                         /*!< Configuration for PWM_CLK                                             */
  __IO uint32_t  DECODER;                           /*!< Configuration of the decoder                                          */
  __IO uint32_t  LOOP;                              /*!< Amount of playback of a loop                                          */
  __I  uint32_t  RESERVED5[2];
  PWM_SEQ_Type SEQ[2];                              /*!< Unspecified                                                           */
  PWM_PSEL_Type PSEL;                               /*!< Unspecified                                                           */
} NRF_PWM_Type;


/* ================================================================================ */
/* ================                       PDM                      ================ */
/* ================================================================================ */


/**
  * @brief Pulse Density Modulation (Digital Microphone) Interface (PDM)
  */

typedef struct {                                    /*!< PDM Structure                                                         */
  __O  uint32_t  TASKS_START;                       /*!< Starts continuous PDM transfer                                        */
  __O  uint32_t  TASKS_STOP;                        /*!< Stops PDM transfer                                                    */
  __I  uint32_t  RESERVED0[62];
  __IO uint32_t  EVENTS_STARTED;                    /*!< PDM transfer has started                                              */
  __IO uint32_t  EVENTS_STOPPED;                    /*!< PDM transfer has finished                                             */
  __IO uint32_t  EVENTS_END;                        /*!< The PDM has written the last sample specified by SAMPLE.MAXCNT
                                                         (or the last sample after a STOP task has been received) to
                                                          Data RAM                                                             */
  __I  uint32_t  RESERVED1[125];
  __IO uint32_t  INTEN;                             /*!< Enable or disable interrupt                                           */
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED2[125];
  __IO uint32_t  ENABLE;                            /*!< PDM module enable register                                            */
  __IO uint32_t  PDMCLKCTRL;                        /*!< PDM clock generator control                                           */
  __IO uint32_t  MODE;                              /*!< Defines the routing of the connected PDM microphones' signals         */
  __I  uint32_t  RESERVED3[3];
  __IO uint32_t  GAINL;                             /*!< Left output gain adjustment                                           */
  __IO uint32_t  GAINR;                             /*!< Right output gain adjustment                                          */
  __IO uint32_t  RATIO;                             /*!< Selects the ratio between PDM_CLK and output sample rate. Change
                                                         PDMCLKCTRL accordingly.                                               */
  __I  uint32_t  RESERVED4[7];
  PDM_PSEL_Type PSEL;                               /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED5[6];
  PDM_SAMPLE_Type SAMPLE;                           /*!< Unspecified                                                           */
} NRF_PDM_Type;


/* ================================================================================ */
/* ================                      NVMC                      ================ */
/* ================================================================================ */


/**
  * @brief Non Volatile Memory Controller (NVMC)
  */

typedef struct {                                    /*!< NVMC Structure                                                        */
  __I  uint32_t  RESERVED0[256];
  __I  uint32_t  READY;                             /*!< Ready flag                                                            */
  __I  uint32_t  RESERVED1[64];
  __IO uint32_t  CONFIG;                            /*!< Configuration register                                                */
  
  union {
    __IO uint32_t  ERASEPCR1;                       /*!< Deprecated register - Register for erasing a page in Code area.
                                                         Equivalent to ERASEPAGE.                                              */
    __IO uint32_t  ERASEPAGE;                       /*!< Register for erasing a page in Code area                              */
  };
  __IO uint32_t  ERASEALL;                          /*!< Register for erasing all non-volatile user memory                     */
  __IO uint32_t  ERASEPCR0;                         /*!< Deprecated register - Register for erasing a page in Code area.
                                                         Equivalent to ERASEPAGE.                                              */
  __IO uint32_t  ERASEUICR;                         /*!< Register for erasing User Information Configuration Registers         */
  __I  uint32_t  RESERVED2[10];
  __IO uint32_t  ICACHECNF;                         /*!< I-Code cache configuration register.                                  */
  __I  uint32_t  RESERVED3;
  __IO uint32_t  IHIT;                              /*!< I-Code cache hit counter.                                             */
  __IO uint32_t  IMISS;                             /*!< I-Code cache miss counter.                                            */
} NRF_NVMC_Type;


/* ================================================================================ */
/* ================                       ACL                      ================ */
/* ================================================================================ */


/**
  * @brief Access control lists (ACL)
  */

typedef struct {                                    /*!< ACL Structure                                                         */
  __I  uint32_t  RESERVED0[449];
  __IO uint32_t  DISABLEINDEBUG;                    /*!< Disable all ACL protection mechanisms for regions while in debug
                                                         mode                                                                  */
  __I  uint32_t  RESERVED1[62];
  ACL_ACL_Type ACL[8];                              /*!< Unspecified                                                           */
} NRF_ACL_Type;


/* ================================================================================ */
/* ================                       PPI                      ================ */
/* ================================================================================ */


/**
  * @brief Programmable Peripheral Interconnect (PPI)
  */

typedef struct {                                    /*!< PPI Structure                                                         */
  PPI_TASKS_CHG_Type TASKS_CHG[6];                  /*!< Channel group tasks                                                   */
  __I  uint32_t  RESERVED0[308];
  __IO uint32_t  CHEN;                              /*!< Channel enable register                                               */
  __IO uint32_t  CHENSET;                           /*!< Channel enable set register                                           */
  __IO uint32_t  CHENCLR;                           /*!< Channel enable clear register                                         */
  __I  uint32_t  RESERVED1;
  PPI_CH_Type CH[20];                               /*!< PPI Channel                                                           */
  __I  uint32_t  RESERVED2[148];
  __IO uint32_t  CHG[6];                            /*!< Description collection[0]: Channel group 0                            */
  __I  uint32_t  RESERVED3[62];
  PPI_FORK_Type FORK[32];                           /*!< Fork                                                                  */
} NRF_PPI_Type;


/* ================================================================================ */
/* ================                       MWU                      ================ */
/* ================================================================================ */


/**
  * @brief Memory Watch Unit (MWU)
  */

typedef struct {                                    /*!< MWU Structure                                                         */
  __I  uint32_t  RESERVED0[64];
  MWU_EVENTS_REGION_Type EVENTS_REGION[4];          /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED1[16];
  MWU_EVENTS_PREGION_Type EVENTS_PREGION[2];        /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED2[100];
  __IO uint32_t  INTEN;                             /*!< Enable or disable interrupt                                           */
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED3[5];
  __IO uint32_t  NMIEN;                             /*!< Enable or disable non-maskable interrupt                              */
  __IO uint32_t  NMIENSET;                          /*!< Enable non-maskable interrupt                                         */
  __IO uint32_t  NMIENCLR;                          /*!< Disable non-maskable interrupt                                        */
  __I  uint32_t  RESERVED4[53];
  MWU_PERREGION_Type PERREGION[2];                  /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED5[64];
  __IO uint32_t  REGIONEN;                          /*!< Enable/disable regions watch                                          */
  __IO uint32_t  REGIONENSET;                       /*!< Enable regions watch                                                  */
  __IO uint32_t  REGIONENCLR;                       /*!< Disable regions watch                                                 */
  __I  uint32_t  RESERVED6[57];
  MWU_REGION_Type REGION[4];                        /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED7[32];
  MWU_PREGION_Type PREGION[2];                      /*!< Unspecified                                                           */
} NRF_MWU_Type;


/* ================================================================================ */
/* ================                       I2S                      ================ */
/* ================================================================================ */


/**
  * @brief Inter-IC Sound (I2S)
  */

typedef struct {                                    /*!< I2S Structure                                                         */
  __O  uint32_t  TASKS_START;                       /*!< Starts continuous I2S transfer. Also starts MCK generator when
                                                         this is enabled.                                                      */
  __O  uint32_t  TASKS_STOP;                        /*!< Stops I2S transfer. Also stops MCK generator. Triggering this
                                                         task will cause the {event:STOPPED} event to be generated.            */
  __I  uint32_t  RESERVED0[63];
  __IO uint32_t  EVENTS_RXPTRUPD;                   /*!< The RXD.PTR register has been copied to internal double-buffers.
                                                         When the I2S module is started and RX is enabled, this event
                                                          will be generated for every RXTXD.MAXCNT words that are received
                                                          on the SDIN pin.                                                     */
  __IO uint32_t  EVENTS_STOPPED;                    /*!< I2S transfer stopped.                                                 */
  __I  uint32_t  RESERVED1[2];
  __IO uint32_t  EVENTS_TXPTRUPD;                   /*!< The TDX.PTR register has been copied to internal double-buffers.
                                                         When the I2S module is started and TX is enabled, this event
                                                          will be generated for every RXTXD.MAXCNT words that are sent
                                                          on the SDOUT pin.                                                    */
  __I  uint32_t  RESERVED2[122];
  __IO uint32_t  INTEN;                             /*!< Enable or disable interrupt                                           */
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED3[125];
  __IO uint32_t  ENABLE;                            /*!< Enable I2S module.                                                    */
  I2S_CONFIG_Type CONFIG;                           /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED4[3];
  I2S_RXD_Type RXD;                                 /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED5;
  I2S_TXD_Type TXD;                                 /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED6[3];
  I2S_RXTXD_Type RXTXD;                             /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED7[3];
  I2S_PSEL_Type PSEL;                               /*!< Unspecified                                                           */
} NRF_I2S_Type;


/* ================================================================================ */
/* ================                       FPU                      ================ */
/* ================================================================================ */


/**
  * @brief FPU (FPU)
  */

typedef struct {                                    /*!< FPU Structure                                                         */
  __I  uint32_t  UNUSED;                            /*!< Unused.                                                               */
} NRF_FPU_Type;


/* ================================================================================ */
/* ================                      USBD                      ================ */
/* ================================================================================ */


/**
  * @brief Universal Serial Bus device (USBD)
  */

typedef struct {                                    /*!< USBD Structure                                                        */
  __I  uint32_t  RESERVED0;
  __O  uint32_t  TASKS_STARTEPIN[8];                /*!< Description collection[0]: Captures the EPIN[0].PTR, EPIN[0].MAXCNT
                                                         and EPIN[0].CONFIG registers values, and enables endpoint IN
                                                          0 to respond to traffic from host                                    */
  __O  uint32_t  TASKS_STARTISOIN;                  /*!< Captures the ISOIN.PTR, ISOIN.MAXCNT and ISOIN.CONFIG registers
                                                         values, and enables sending data on iso endpoint                      */
  __O  uint32_t  TASKS_STARTEPOUT[8];               /*!< Description collection[0]: Captures the EPOUT[0].PTR, EPOUT[0].MAXCNT
                                                         and EPOUT[0].CONFIG registers values, and enables endpoint 0
                                                          to respond to traffic from host                                      */
  __O  uint32_t  TASKS_STARTISOOUT;                 /*!< Captures the ISOOUT.PTR, ISOOUT.MAXCNT and ISOOUT.CONFIG registers
                                                         values, and enables receiving of data on iso endpoint                 */
  __O  uint32_t  TASKS_EP0RCVOUT;                   /*!< Allows OUT data stage on control endpoint 0                           */
  __O  uint32_t  TASKS_EP0STATUS;                   /*!< Allows status stage on control endpoint 0                             */
  __O  uint32_t  TASKS_EP0STALL;                    /*!< STALLs data and status stage on control endpoint 0                    */
  __O  uint32_t  TASKS_DPDMDRIVE;                   /*!< Forces D+ and D-lines to the state defined in the DPDMVALUE
                                                         register                                                              */
  __O  uint32_t  TASKS_DPDMNODRIVE;                 /*!< Stops forcing D+ and D- lines to any state (USB engine takes
                                                         control)                                                              */
  __I  uint32_t  RESERVED1[40];
  __IO uint32_t  EVENTS_USBRESET;                   /*!< Signals that a USB reset condition has been detected on the
                                                         USB lines                                                             */
  __IO uint32_t  EVENTS_STARTED;                    /*!< Confirms that the EPIN[n].PTR, EPIN[n].MAXCNT, EPIN[n].CONFIG,
                                                         or EPOUT[n].PTR, EPOUT[n].MAXCNT and EPOUT[n].CONFIG registers
                                                          have been captured on all endpoints reported in the EPSTATUS
                                                          register                                                             */
  __IO uint32_t  EVENTS_ENDEPIN[8];                 /*!< Description collection[0]: The whole EPIN[0] buffer has been
                                                         consumed. The RAM buffer can be accessed safely by software.          */
  __IO uint32_t  EVENTS_EP0DATADONE;                /*!< An acknowledged data transfer has taken place on the control
                                                         endpoint                                                              */
  __IO uint32_t  EVENTS_ENDISOIN;                   /*!< The whole ISOIN buffer has been consumed. The RAM buffer can
                                                         be accessed safely by software.                                       */
  __IO uint32_t  EVENTS_ENDEPOUT[8];                /*!< Description collection[0]: The whole EPOUT[0] buffer has been
                                                         consumed. The RAM buffer can be accessed safely by software.          */
  __IO uint32_t  EVENTS_ENDISOOUT;                  /*!< The whole ISOOUT buffer has been consumed. The RAM buffer can
                                                         be accessed safely by software.                                       */
  __IO uint32_t  EVENTS_SOF;                        /*!< Signals that a SOF (start of frame) condition has been detected
                                                         on the USB lines                                                      */
  __IO uint32_t  EVENTS_USBEVENT;                   /*!< An event or an error not covered by specific events has occurred,
                                                         check EVENTCAUSE register to find the cause                           */
  __IO uint32_t  EVENTS_EP0SETUP;                   /*!< A valid SETUP token has been received (and acknowledged) on
                                                         the control endpoint                                                  */
  __IO uint32_t  EVENTS_EPDATA;                     /*!< A data transfer has occurred on a data endpoint, indicated by
                                                         the EPDATASTATUS register                                             */
  __IO uint32_t  EVENTS_ACCESSFAULT;                /*!< Access to an unavailable USB register has been attempted (software
                                                         or EasyDMA). This event can get fired even when USBD is not
                                                          ENABLEd.                                                             */
  __I  uint32_t  RESERVED2[38];
  __IO uint32_t  SHORTS;                            /*!< Shortcut register                                                     */
  __I  uint32_t  RESERVED3[63];
  __IO uint32_t  INTEN;                             /*!< Enable or disable interrupt                                           */
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED4[61];
  __IO uint32_t  EVENTCAUSE;                        /*!< Details on event that caused the USBEVENT event                       */
  __I  uint32_t  BUSSTATE;                          /*!< Provides the logic state of the D+ and D- lines                       */
  __I  uint32_t  RESERVED5[6];
  USBD_HALTED_Type HALTED;                          /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED6;
  __IO uint32_t  EPSTATUS;                          /*!< Provides information on which endpoint's EasyDMA registers have
                                                         been captured                                                         */
  __IO uint32_t  EPDATASTATUS;                      /*!< Provides information on which endpoint(s) an acknowledged data
                                                         transfer has occurred (EPDATA event)                                  */
  __I  uint32_t  USBADDR;                           /*!< Device USB address                                                    */
  __I  uint32_t  RESERVED7[3];
  __I  uint32_t  BMREQUESTTYPE;                     /*!< SETUP data, byte 0, bmRequestType                                     */
  __I  uint32_t  BREQUEST;                          /*!< SETUP data, byte 1, bRequest                                          */
  __I  uint32_t  WVALUEL;                           /*!< SETUP data, byte 2, LSB of wValue                                     */
  __I  uint32_t  WVALUEH;                           /*!< SETUP data, byte 3, MSB of wValue                                     */
  __I  uint32_t  WINDEXL;                           /*!< SETUP data, byte 4, LSB of wIndex                                     */
  __I  uint32_t  WINDEXH;                           /*!< SETUP data, byte 5, MSB of wIndex                                     */
  __I  uint32_t  WLENGTHL;                          /*!< SETUP data, byte 6, LSB of wLength                                    */
  __I  uint32_t  WLENGTHH;                          /*!< SETUP data, byte 7, MSB of wLength                                    */
  USBD_SIZE_Type SIZE;                              /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED8[15];
  __IO uint32_t  ENABLE;                            /*!< Enable USB                                                            */
  __IO uint32_t  USBPULLUP;                         /*!< Control of the USB pull-up                                            */
  __IO uint32_t  DPDMVALUE;                         /*!< State at which the DPDMDRIVE task will force D+ and D-. The
                                                         DPDMNODRIVE task reverts the control of the lines to MAC IP
                                                          (no forcing).                                                        */
  __IO uint32_t  DTOGGLE;                           /*!< Data toggle control and status.                                       */
  __IO uint32_t  EPINEN;                            /*!< Endpoint IN enable                                                    */
  __IO uint32_t  EPOUTEN;                           /*!< Endpoint OUT enable                                                   */
  __O  uint32_t  EPSTALL;                           /*!< STALL endpoints                                                       */
  __IO uint32_t  ISOSPLIT;                          /*!< Controls the split of ISO buffers                                     */
  __I  uint32_t  FRAMECNTR;                         /*!< Returns the current value of the start of frame counter               */
  __I  uint32_t  RESERVED9[3];
  __IO uint32_t  ISOINCONFIG;                       /*!< Controls the response of the ISO IN endpoint to an IN token
                                                         when no data is ready to be sent                                      */
  __I  uint32_t  RESERVED10[51];
  USBD_EPIN_Type EPIN[8];                           /*!< Unspecified                                                           */
  USBD_ISOIN_Type ISOIN;                            /*!< Unspecified                                                           */
  __I  uint32_t  RESERVED11[21];
  USBD_EPOUT_Type EPOUT[8];                         /*!< Unspecified                                                           */
  USBD_ISOOUT_Type ISOOUT;                          /*!< Unspecified                                                           */
} NRF_USBD_Type;


/* ================================================================================ */
/* ================                      QSPI                      ================ */
/* ================================================================================ */


/**
  * @brief External flash interface (QSPI)
  */

typedef struct {                                    /*!< QSPI Structure                                                        */
  __O  uint32_t  TASKS_ACTIVATE;                    /*!< Activate QSPI interface                                               */
  __O  uint32_t  TASKS_READSTART;                   /*!< Start transfer from external flash memory to internal RAM             */
  __O  uint32_t  TASKS_WRITESTART;                  /*!< Start transfer from internal RAM to external flash memory             */
  __O  uint32_t  TASKS_ERASESTART;                  /*!< Start external flash memory erase operation                           */
  __I  uint32_t  RESERVED0[60];
  __IO uint32_t  EVENTS_READY;                      /*!< QSPI peripheral is ready. This event will be generated as a
                                                         response to any QSPI task.                                            */
  __I  uint32_t  RESERVED1[127];
  __IO uint32_t  INTEN;                             /*!< Enable or disable interrupt                                           */
  __IO uint32_t  INTENSET;                          /*!< Enable interrupt                                                      */
  __IO uint32_t  INTENCLR;                          /*!< Disable interrupt                                                     */
  __I  uint32_t  RESERVED2[125];
  __IO uint32_t  ENABLE;                            /*!< Enable QSPI peripheral and acquire the pins selected in PSELn
                                                         registers                                                             */
  QSPI_READ_Type READ;                              /*!< Unspecified                                                           */
  QSPI_WRITE_Type WRITE;                            /*!< Unspecified                                                           */
  QSPI_ERASE_Type ERASE;                            /*!< Unspecified                                                           */
  QSPI_PSEL_Type PSEL;                              /*!< Unspecified                                                           */
  __IO uint32_t  XIPOFFSET;                         /*!< Address offset into the external memory for Execute in Place
                                                         operation.                                                            */
  __IO uint32_t  IFCONFIG0;                         /*!< Interface configuration.                                              */
  __I  uint32_t  RESERVED3[46];
  __IO uint32_t  IFCONFIG1;                         /*!< Interface configuration.                                              */
  __I  uint32_t  STATUS;                            /*!< Status register.                                                      */
  __I  uint32_t  RESERVED4[3];
  __IO uint32_t  DPMDUR;                            /*!< Set the duration required to enter/exit deep power-down mode
                                                         (DPM).                                                                */
  __I  uint32_t  RESERVED5[3];
  __IO uint32_t  ADDRCONF;                          /*!< Extended address configuration.                                       */
  __I  uint32_t  RESERVED6[3];
  __IO uint32_t  CINSTRCONF;                        /*!< Custom instruction configuration register.                            */
  __IO uint32_t  CINSTRDAT0;                        /*!< Custom instruction data register 0.                                   */
  __IO uint32_t  CINSTRDAT1;                        /*!< Custom instruction data register 1.                                   */
  __IO uint32_t  IFTIMING;                          /*!< SPI interface timing.                                                 */
} NRF_QSPI_Type;


/* ================================================================================ */
/* ================                      GPIO                      ================ */
/* ================================================================================ */


/**
  * @brief GPIO Port 1 (GPIO)
  */

typedef struct {                                    /*!< GPIO Structure                                                        */
  __I  uint32_t  RESERVED0[321];
  __IO uint32_t  OUT;                               /*!< Write GPIO port                                                       */
  __IO uint32_t  OUTSET;                            /*!< Set individual bits in GPIO port                                      */
  __IO uint32_t  OUTCLR;                            /*!< Clear individual bits in GPIO port                                    */
  __I  uint32_t  IN;                                /*!< Read GPIO port                                                        */
  __IO uint32_t  DIR;                               /*!< Direction of GPIO pins                                                */
  __IO uint32_t  DIRSET;                            /*!< DIR set register                                                      */
  __IO uint32_t  DIRCLR;                            /*!< DIR clear register                                                    */
  __IO uint32_t  LATCH;                             /*!< Latch register indicating what GPIO pins that have met the criteria
                                                         set in the PIN_CNF[n].SENSE registers                                 */
  __IO uint32_t  DETECTMODE;                        /*!< Select between default DETECT signal behaviour and LDETECT mode       */
  __I  uint32_t  RESERVED1[118];
  __IO uint32_t  PIN_CNF[32];                       /*!< Description collection[0]: Configuration of GPIO pins                 */
} NRF_GPIO_Type;


/* ================================================================================ */
/* ================                   CRYPTOCELL                   ================ */
/* ================================================================================ */


/**
  * @brief ARM CryptoCell register interface (CRYPTOCELL)
  */

typedef struct {                                    /*!< CRYPTOCELL Structure                                                  */
  __I  uint32_t  RESERVED0[320];
  __IO uint32_t  ENABLE;                            /*!< Control power and clock for ARM CryptoCell subsystem                  */
} NRF_CRYPTOCELL_Type;


/* --------------------  End of section using anonymous unions  ------------------- */
#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
  /* leave anonymous unions enabled */
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning restore
#else
  #warning Not supported compiler type
#endif




/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */

#define NRF_FICR_BASE                   0x10000000UL
#define NRF_UICR_BASE                   0x10001000UL
#define NRF_POWER_BASE                  0x40000000UL
#define NRF_CLOCK_BASE                  0x40000000UL
#define NRF_RADIO_BASE                  0x40001000UL
#define NRF_UARTE0_BASE                 0x40002000UL
#define NRF_UART0_BASE                  0x40002000UL
#define NRF_SPIM0_BASE                  0x40003000UL
#define NRF_SPIS0_BASE                  0x40003000UL
#define NRF_TWIM0_BASE                  0x40003000UL
#define NRF_TWIS0_BASE                  0x40003000UL
#define NRF_SPI0_BASE                   0x40003000UL
#define NRF_TWI0_BASE                   0x40003000UL
#define NRF_SPIM1_BASE                  0x40004000UL
#define NRF_SPIS1_BASE                  0x40004000UL
#define NRF_TWIM1_BASE                  0x40004000UL
#define NRF_TWIS1_BASE                  0x40004000UL
#define NRF_SPI1_BASE                   0x40004000UL
#define NRF_TWI1_BASE                   0x40004000UL
#define NRF_NFCT_BASE                   0x40005000UL
#define NRF_GPIOTE_BASE                 0x40006000UL
#define NRF_SAADC_BASE                  0x40007000UL
#define NRF_TIMER0_BASE                 0x40008000UL
#define NRF_TIMER1_BASE                 0x40009000UL
#define NRF_TIMER2_BASE                 0x4000A000UL
#define NRF_RTC0_BASE                   0x4000B000UL
#define NRF_TEMP_BASE                   0x4000C000UL
#define NRF_RNG_BASE                    0x4000D000UL
#define NRF_ECB_BASE                    0x4000E000UL
#define NRF_CCM_BASE                    0x4000F000UL
#define NRF_AAR_BASE                    0x4000F000UL
#define NRF_WDT_BASE                    0x40010000UL
#define NRF_RTC1_BASE                   0x40011000UL
#define NRF_QDEC_BASE                   0x40012000UL
#define NRF_COMP_BASE                   0x40013000UL
#define NRF_LPCOMP_BASE                 0x40013000UL
#define NRF_SWI0_BASE                   0x40014000UL
#define NRF_EGU0_BASE                   0x40014000UL
#define NRF_SWI1_BASE                   0x40015000UL
#define NRF_EGU1_BASE                   0x40015000UL
#define NRF_SWI2_BASE                   0x40016000UL
#define NRF_EGU2_BASE                   0x40016000UL
#define NRF_SWI3_BASE                   0x40017000UL
#define NRF_EGU3_BASE                   0x40017000UL
#define NRF_SWI4_BASE                   0x40018000UL
#define NRF_EGU4_BASE                   0x40018000UL
#define NRF_SWI5_BASE                   0x40019000UL
#define NRF_EGU5_BASE                   0x40019000UL
#define NRF_TIMER3_BASE                 0x4001A000UL
#define NRF_TIMER4_BASE                 0x4001B000UL
#define NRF_PWM0_BASE                   0x4001C000UL
#define NRF_PDM_BASE                    0x4001D000UL
#define NRF_NVMC_BASE                   0x4001E000UL
#define NRF_ACL_BASE                    0x4001E000UL
#define NRF_PPI_BASE                    0x4001F000UL
#define NRF_MWU_BASE                    0x40020000UL
#define NRF_PWM1_BASE                   0x40021000UL
#define NRF_PWM2_BASE                   0x40022000UL
#define NRF_SPIM2_BASE                  0x40023000UL
#define NRF_SPIS2_BASE                  0x40023000UL
#define NRF_SPI2_BASE                   0x40023000UL
#define NRF_RTC2_BASE                   0x40024000UL
#define NRF_I2S_BASE                    0x40025000UL
#define NRF_FPU_BASE                    0x40026000UL
#define NRF_USBD_BASE                   0x40027000UL
#define NRF_UARTE1_BASE                 0x40028000UL
#define NRF_QSPI_BASE                   0x40029000UL
#define NRF_SPIM3_BASE                  0x4002B000UL
#define NRF_PWM3_BASE                   0x4002D000UL
#define NRF_P0_BASE                     0x50000000UL
#define NRF_P1_BASE                     0x50000300UL
#define NRF_CRYPTOCELL_BASE             0x5002A000UL


/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */

#define NRF_FICR                        ((NRF_FICR_Type           *) NRF_FICR_BASE)
#define NRF_UICR                        ((NRF_UICR_Type           *) NRF_UICR_BASE)
#define NRF_POWER                       ((NRF_POWER_Type          *) NRF_POWER_BASE)
#define NRF_CLOCK                       ((NRF_CLOCK_Type          *) NRF_CLOCK_BASE)
#define NRF_RADIO                       ((NRF_RADIO_Type          *) NRF_RADIO_BASE)
#define NRF_UARTE0                      ((NRF_UARTE_Type          *) NRF_UARTE0_BASE)
#define NRF_UART0                       ((NRF_UART_Type           *) NRF_UART0_BASE)
#define NRF_SPIM0                       ((NRF_SPIM_Type           *) NRF_SPIM0_BASE)
#define NRF_SPIS0                       ((NRF_SPIS_Type           *) NRF_SPIS0_BASE)
#define NRF_TWIM0                       ((NRF_TWIM_Type           *) NRF_TWIM0_BASE)
#define NRF_TWIS0                       ((NRF_TWIS_Type           *) NRF_TWIS0_BASE)
#define NRF_SPI0                        ((NRF_SPI_Type            *) NRF_SPI0_BASE)
#define NRF_TWI0                        ((NRF_TWI_Type            *) NRF_TWI0_BASE)
#define NRF_SPIM1                       ((NRF_SPIM_Type           *) NRF_SPIM1_BASE)
#define NRF_SPIS1                       ((NRF_SPIS_Type           *) NRF_SPIS1_BASE)
#define NRF_TWIM1                       ((NRF_TWIM_Type           *) NRF_TWIM1_BASE)
#define NRF_TWIS1                       ((NRF_TWIS_Type           *) NRF_TWIS1_BASE)
#define NRF_SPI1                        ((NRF_SPI_Type            *) NRF_SPI1_BASE)
#define NRF_TWI1                        ((NRF_TWI_Type            *) NRF_TWI1_BASE)
#define NRF_NFCT                        ((NRF_NFCT_Type           *) NRF_NFCT_BASE)
#define NRF_GPIOTE                      ((NRF_GPIOTE_Type         *) NRF_GPIOTE_BASE)
#define NRF_SAADC                       ((NRF_SAADC_Type          *) NRF_SAADC_BASE)
#define NRF_TIMER0                      ((NRF_TIMER_Type          *) NRF_TIMER0_BASE)
#define NRF_TIMER1                      ((NRF_TIMER_Type          *) NRF_TIMER1_BASE)
#define NRF_TIMER2                      ((NRF_TIMER_Type          *) NRF_TIMER2_BASE)
#define NRF_RTC0                        ((NRF_RTC_Type            *) NRF_RTC0_BASE)
#define NRF_TEMP                        ((NRF_TEMP_Type           *) NRF_TEMP_BASE)
#define NRF_RNG                         ((NRF_RNG_Type            *) NRF_RNG_BASE)
#define NRF_ECB                         ((NRF_ECB_Type            *) NRF_ECB_BASE)
#define NRF_CCM                         ((NRF_CCM_Type            *) NRF_CCM_BASE)
#define NRF_AAR                         ((NRF_AAR_Type            *) NRF_AAR_BASE)
#define NRF_WDT                         ((NRF_WDT_Type            *) NRF_WDT_BASE)
#define NRF_RTC1                        ((NRF_RTC_Type            *) NRF_RTC1_BASE)
#define NRF_QDEC                        ((NRF_QDEC_Type           *) NRF_QDEC_BASE)
#define NRF_COMP                        ((NRF_COMP_Type           *) NRF_COMP_BASE)
#define NRF_LPCOMP                      ((NRF_LPCOMP_Type         *) NRF_LPCOMP_BASE)
#define NRF_SWI0                        ((NRF_SWI_Type            *) NRF_SWI0_BASE)
#define NRF_EGU0                        ((NRF_EGU_Type            *) NRF_EGU0_BASE)
#define NRF_SWI1                        ((NRF_SWI_Type            *) NRF_SWI1_BASE)
#define NRF_EGU1                        ((NRF_EGU_Type            *) NRF_EGU1_BASE)
#define NRF_SWI2                        ((NRF_SWI_Type            *) NRF_SWI2_BASE)
#define NRF_EGU2                        ((NRF_EGU_Type            *) NRF_EGU2_BASE)
#define NRF_SWI3                        ((NRF_SWI_Type            *) NRF_SWI3_BASE)
#define NRF_EGU3                        ((NRF_EGU_Type            *) NRF_EGU3_BASE)
#define NRF_SWI4                        ((NRF_SWI_Type            *) NRF_SWI4_BASE)
#define NRF_EGU4                        ((NRF_EGU_Type            *) NRF_EGU4_BASE)
#define NRF_SWI5                        ((NRF_SWI_Type            *) NRF_SWI5_BASE)
#define NRF_EGU5                        ((NRF_EGU_Type            *) NRF_EGU5_BASE)
#define NRF_TIMER3                      ((NRF_TIMER_Type          *) NRF_TIMER3_BASE)
#define NRF_TIMER4                      ((NRF_TIMER_Type          *) NRF_TIMER4_BASE)
#define NRF_PWM0                        ((NRF_PWM_Type            *) NRF_PWM0_BASE)
#define NRF_PDM                         ((NRF_PDM_Type            *) NRF_PDM_BASE)
#define NRF_NVMC                        ((NRF_NVMC_Type           *) NRF_NVMC_BASE)
#define NRF_ACL                         ((NRF_ACL_Type            *) NRF_ACL_BASE)
#define NRF_PPI                         ((NRF_PPI_Type            *) NRF_PPI_BASE)
#define NRF_MWU                         ((NRF_MWU_Type            *) NRF_MWU_BASE)
#define NRF_PWM1                        ((NRF_PWM_Type            *) NRF_PWM1_BASE)
#define NRF_PWM2                        ((NRF_PWM_Type            *) NRF_PWM2_BASE)
#define NRF_SPIM2                       ((NRF_SPIM_Type           *) NRF_SPIM2_BASE)
#define NRF_SPIS2                       ((NRF_SPIS_Type           *) NRF_SPIS2_BASE)
#define NRF_SPI2                        ((NRF_SPI_Type            *) NRF_SPI2_BASE)
#define NRF_RTC2                        ((NRF_RTC_Type            *) NRF_RTC2_BASE)
#define NRF_I2S                         ((NRF_I2S_Type            *) NRF_I2S_BASE)
#define NRF_FPU                         ((NRF_FPU_Type            *) NRF_FPU_BASE)
#define NRF_USBD                        ((NRF_USBD_Type           *) NRF_USBD_BASE)
#define NRF_UARTE1                      ((NRF_UARTE_Type          *) NRF_UARTE1_BASE)
#define NRF_QSPI                        ((NRF_QSPI_Type           *) NRF_QSPI_BASE)
#define NRF_SPIM3                       ((NRF_SPIM_Type           *) NRF_SPIM3_BASE)
#define NRF_PWM3                        ((NRF_PWM_Type            *) NRF_PWM3_BASE)
#define NRF_P0                          ((NRF_GPIO_Type           *) NRF_P0_BASE)
#define NRF_P1                          ((NRF_GPIO_Type           *) NRF_P1_BASE)
#define NRF_CRYPTOCELL                  ((NRF_CRYPTOCELL_Type     *) NRF_CRYPTOCELL_BASE)


/** @} */ /* End of group Device_Peripheral_Registers */
/** @} */ /* End of group nrf52840 */
/** @} */ /* End of group Nordic Semiconductor */

#ifdef __cplusplus
}
#endif


#endif  /* nrf52840_H */

