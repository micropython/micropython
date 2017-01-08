
/****************************************************************************************************//**
 * @file     nrf51.h
 *
 * @brief    CMSIS Cortex-M0 Peripheral Access Layer Header File for
 *           nrf51 from Nordic Semiconductor.
 *
 * @version  V522
 * @date     18. November 2016
 *
 * @note     Generated with SVDConv V2.81d 
 *           from CMSIS SVD File 'nrf51.svd' Version 522,
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

/** @addtogroup nrf51
  * @{
  */

#ifndef NRF51_H
#define NRF51_H

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum {
/* -------------------  Cortex-M0 Processor Exceptions Numbers  ------------------- */
  Reset_IRQn                    = -15,              /*!<   1  Reset Vector, invoked on Power up and warm reset                 */
  NonMaskableInt_IRQn           = -14,              /*!<   2  Non maskable Interrupt, cannot be stopped or preempted           */
  HardFault_IRQn                = -13,              /*!<   3  Hard Fault, all classes of Fault                                 */
  SVCall_IRQn                   =  -5,              /*!<  11  System Service Call via SVC instruction                          */
  DebugMonitor_IRQn             =  -4,              /*!<  12  Debug Monitor                                                    */
  PendSV_IRQn                   =  -2,              /*!<  14  Pendable request for system service                              */
  SysTick_IRQn                  =  -1,              /*!<  15  System Tick Timer                                                */
/* ----------------------  nrf51 Specific Interrupt Numbers  ---------------------- */
  POWER_CLOCK_IRQn              =   0,              /*!<   0  POWER_CLOCK                                                      */
  RADIO_IRQn                    =   1,              /*!<   1  RADIO                                                            */
  UART0_IRQn                    =   2,              /*!<   2  UART0                                                            */
  SPI0_TWI0_IRQn                =   3,              /*!<   3  SPI0_TWI0                                                        */
  SPI1_TWI1_IRQn                =   4,              /*!<   4  SPI1_TWI1                                                        */
  GPIOTE_IRQn                   =   6,              /*!<   6  GPIOTE                                                           */
  ADC_IRQn                      =   7,              /*!<   7  ADC                                                              */
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
  LPCOMP_IRQn                   =  19,              /*!<  19  LPCOMP                                                           */
  SWI0_IRQn                     =  20,              /*!<  20  SWI0                                                             */
  SWI1_IRQn                     =  21,              /*!<  21  SWI1                                                             */
  SWI2_IRQn                     =  22,              /*!<  22  SWI2                                                             */
  SWI3_IRQn                     =  23,              /*!<  23  SWI3                                                             */
  SWI4_IRQn                     =  24,              /*!<  24  SWI4                                                             */
  SWI5_IRQn                     =  25               /*!<  25  SWI5                                                             */
} IRQn_Type;


/** @addtogroup Configuration_of_CMSIS
  * @{
  */


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* ----------------Configuration of the Cortex-M0 Processor and Core Peripherals---------------- */
#define __CM0_REV                 0x0301            /*!< Cortex-M0 Core Revision                                               */
#define __MPU_PRESENT                  0            /*!< MPU present or not                                                    */
#define __NVIC_PRIO_BITS               2            /*!< Number of Bits used for Priority Levels                               */
#define __Vendor_SysTickConfig         0            /*!< Set to 1 if different SysTick Config is used                          */
/** @} */ /* End of group Configuration_of_CMSIS */

#include "core_cm0.h"                               /*!< Cortex-M0 processor and core peripherals                              */
#include "system_nrf51.h"                           /*!< nrf51 System                                                          */


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
  __O  uint32_t  EN;                                /*!< Enable channel group.                                                 */
  __O  uint32_t  DIS;                               /*!< Disable channel group.                                                */
} PPI_TASKS_CHG_Type;

typedef struct {
  __IO uint32_t  EEP;                               /*!< Channel event end-point.                                              */
  __IO uint32_t  TEP;                               /*!< Channel task end-point.                                               */
} PPI_CH_Type;


/* ================================================================================ */
/* ================                      POWER                     ================ */
/* ================================================================================ */


/**
  * @brief Power Control. (POWER)
  */

typedef struct {                                    /*!< POWER Structure                                                       */
  __I  uint32_t  RESERVED0[30];
  __O  uint32_t  TASKS_CONSTLAT;                    /*!< Enable constant latency mode.                                         */
  __O  uint32_t  TASKS_LOWPWR;                      /*!< Enable low power mode (variable latency).                             */
  __I  uint32_t  RESERVED1[34];
  __IO uint32_t  EVENTS_POFWARN;                    /*!< Power failure warning.                                                */
  __I  uint32_t  RESERVED2[126];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED3[61];
  __IO uint32_t  RESETREAS;                         /*!< Reset reason.                                                         */
  __I  uint32_t  RESERVED4[9];
  __I  uint32_t  RAMSTATUS;                         /*!< Ram status register.                                                  */
  __I  uint32_t  RESERVED5[53];
  __O  uint32_t  SYSTEMOFF;                         /*!< System off register.                                                  */
  __I  uint32_t  RESERVED6[3];
  __IO uint32_t  POFCON;                            /*!< Power failure configuration.                                          */
  __I  uint32_t  RESERVED7[2];
  __IO uint32_t  GPREGRET;                          /*!< General purpose retention register. This register is a retained
                                                         register.                                                             */
  __I  uint32_t  RESERVED8;
  __IO uint32_t  RAMON;                             /*!< Ram on/off.                                                           */
  __I  uint32_t  RESERVED9[7];
  __IO uint32_t  RESET;                             /*!< Pin reset functionality configuration register. This register
                                                         is a retained register.                                               */
  __I  uint32_t  RESERVED10[3];
  __IO uint32_t  RAMONB;                            /*!< Ram on/off.                                                           */
  __I  uint32_t  RESERVED11[8];
  __IO uint32_t  DCDCEN;                            /*!< DCDC converter enable configuration register.                         */
  __I  uint32_t  RESERVED12[291];
  __IO uint32_t  DCDCFORCE;                         /*!< DCDC power-up force register.                                         */
} NRF_POWER_Type;


/* ================================================================================ */
/* ================                      CLOCK                     ================ */
/* ================================================================================ */


/**
  * @brief Clock control. (CLOCK)
  */

typedef struct {                                    /*!< CLOCK Structure                                                       */
  __O  uint32_t  TASKS_HFCLKSTART;                  /*!< Start HFCLK clock source.                                             */
  __O  uint32_t  TASKS_HFCLKSTOP;                   /*!< Stop HFCLK clock source.                                              */
  __O  uint32_t  TASKS_LFCLKSTART;                  /*!< Start LFCLK clock source.                                             */
  __O  uint32_t  TASKS_LFCLKSTOP;                   /*!< Stop LFCLK clock source.                                              */
  __O  uint32_t  TASKS_CAL;                         /*!< Start calibration of LFCLK RC oscillator.                             */
  __O  uint32_t  TASKS_CTSTART;                     /*!< Start calibration timer.                                              */
  __O  uint32_t  TASKS_CTSTOP;                      /*!< Stop calibration timer.                                               */
  __I  uint32_t  RESERVED0[57];
  __IO uint32_t  EVENTS_HFCLKSTARTED;               /*!< HFCLK oscillator started.                                             */
  __IO uint32_t  EVENTS_LFCLKSTARTED;               /*!< LFCLK oscillator started.                                             */
  __I  uint32_t  RESERVED1;
  __IO uint32_t  EVENTS_DONE;                       /*!< Calibration of LFCLK RC oscillator completed.                         */
  __IO uint32_t  EVENTS_CTTO;                       /*!< Calibration timer timeout.                                            */
  __I  uint32_t  RESERVED2[124];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED3[63];
  __I  uint32_t  HFCLKRUN;                          /*!< Task HFCLKSTART trigger status.                                       */
  __I  uint32_t  HFCLKSTAT;                         /*!< High frequency clock status.                                          */
  __I  uint32_t  RESERVED4;
  __I  uint32_t  LFCLKRUN;                          /*!< Task LFCLKSTART triggered status.                                     */
  __I  uint32_t  LFCLKSTAT;                         /*!< Low frequency clock status.                                           */
  __I  uint32_t  LFCLKSRCCOPY;                      /*!< Clock source for the LFCLK clock, set when task LKCLKSTART is
                                                         triggered.                                                            */
  __I  uint32_t  RESERVED5[62];
  __IO uint32_t  LFCLKSRC;                          /*!< Clock source for the LFCLK clock.                                     */
  __I  uint32_t  RESERVED6[7];
  __IO uint32_t  CTIV;                              /*!< Calibration timer interval.                                           */
  __I  uint32_t  RESERVED7[5];
  __IO uint32_t  XTALFREQ;                          /*!< Crystal frequency.                                                    */
} NRF_CLOCK_Type;


/* ================================================================================ */
/* ================                       MPU                      ================ */
/* ================================================================================ */


/**
  * @brief Memory Protection Unit. (MPU)
  */

typedef struct {                                    /*!< MPU Structure                                                         */
  __I  uint32_t  RESERVED0[330];
  __IO uint32_t  PERR0;                             /*!< Configuration of peripherals in mpu regions.                          */
  __IO uint32_t  RLENR0;                            /*!< Length of RAM region 0.                                               */
  __I  uint32_t  RESERVED1[52];
  __IO uint32_t  PROTENSET0;                        /*!< Erase and write protection bit enable set register.                   */
  __IO uint32_t  PROTENSET1;                        /*!< Erase and write protection bit enable set register.                   */
  __IO uint32_t  DISABLEINDEBUG;                    /*!< Disable erase and write protection mechanism in debug mode.           */
  __IO uint32_t  PROTBLOCKSIZE;                     /*!< Erase and write protection block size.                                */
} NRF_MPU_Type;


/* ================================================================================ */
/* ================                      RADIO                     ================ */
/* ================================================================================ */


/**
  * @brief The radio. (RADIO)
  */

typedef struct {                                    /*!< RADIO Structure                                                       */
  __O  uint32_t  TASKS_TXEN;                        /*!< Enable radio in TX mode.                                              */
  __O  uint32_t  TASKS_RXEN;                        /*!< Enable radio in RX mode.                                              */
  __O  uint32_t  TASKS_START;                       /*!< Start radio.                                                          */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop radio.                                                           */
  __O  uint32_t  TASKS_DISABLE;                     /*!< Disable radio.                                                        */
  __O  uint32_t  TASKS_RSSISTART;                   /*!< Start the RSSI and take one sample of the receive signal strength.    */
  __O  uint32_t  TASKS_RSSISTOP;                    /*!< Stop the RSSI measurement.                                            */
  __O  uint32_t  TASKS_BCSTART;                     /*!< Start the bit counter.                                                */
  __O  uint32_t  TASKS_BCSTOP;                      /*!< Stop the bit counter.                                                 */
  __I  uint32_t  RESERVED0[55];
  __IO uint32_t  EVENTS_READY;                      /*!< Ready event.                                                          */
  __IO uint32_t  EVENTS_ADDRESS;                    /*!< Address event.                                                        */
  __IO uint32_t  EVENTS_PAYLOAD;                    /*!< Payload event.                                                        */
  __IO uint32_t  EVENTS_END;                        /*!< End event.                                                            */
  __IO uint32_t  EVENTS_DISABLED;                   /*!< Disable event.                                                        */
  __IO uint32_t  EVENTS_DEVMATCH;                   /*!< A device address match occurred on the last received packet.          */
  __IO uint32_t  EVENTS_DEVMISS;                    /*!< No device address match occurred on the last received packet.         */
  __IO uint32_t  EVENTS_RSSIEND;                    /*!< Sampling of the receive signal strength complete. A new RSSI
                                                         sample is ready for readout at the RSSISAMPLE register.               */
  __I  uint32_t  RESERVED1[2];
  __IO uint32_t  EVENTS_BCMATCH;                    /*!< Bit counter reached bit count value specified in BCC register.        */
  __I  uint32_t  RESERVED2[53];
  __IO uint32_t  SHORTS;                            /*!< Shortcuts for the radio.                                              */
  __I  uint32_t  RESERVED3[64];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED4[61];
  __I  uint32_t  CRCSTATUS;                         /*!< CRC status of received packet.                                        */
  __I  uint32_t  RESERVED5;
  __I  uint32_t  RXMATCH;                           /*!< Received address.                                                     */
  __I  uint32_t  RXCRC;                             /*!< Received CRC.                                                         */
  __I  uint32_t  DAI;                               /*!< Device address match index.                                           */
  __I  uint32_t  RESERVED6[60];
  __IO uint32_t  PACKETPTR;                         /*!< Packet pointer. Decision point: START task.                           */
  __IO uint32_t  FREQUENCY;                         /*!< Frequency.                                                            */
  __IO uint32_t  TXPOWER;                           /*!< Output power.                                                         */
  __IO uint32_t  MODE;                              /*!< Data rate and modulation.                                             */
  __IO uint32_t  PCNF0;                             /*!< Packet configuration 0.                                               */
  __IO uint32_t  PCNF1;                             /*!< Packet configuration 1.                                               */
  __IO uint32_t  BASE0;                             /*!< Radio base address 0. Decision point: START task.                     */
  __IO uint32_t  BASE1;                             /*!< Radio base address 1. Decision point: START task.                     */
  __IO uint32_t  PREFIX0;                           /*!< Prefixes bytes for logical addresses 0 to 3.                          */
  __IO uint32_t  PREFIX1;                           /*!< Prefixes bytes for logical addresses 4 to 7.                          */
  __IO uint32_t  TXADDRESS;                         /*!< Transmit address select.                                              */
  __IO uint32_t  RXADDRESSES;                       /*!< Receive address select.                                               */
  __IO uint32_t  CRCCNF;                            /*!< CRC configuration.                                                    */
  __IO uint32_t  CRCPOLY;                           /*!< CRC polynomial.                                                       */
  __IO uint32_t  CRCINIT;                           /*!< CRC initial value.                                                    */
  __IO uint32_t  TEST;                              /*!< Test features enable register.                                        */
  __IO uint32_t  TIFS;                              /*!< Inter Frame Spacing in microseconds.                                  */
  __I  uint32_t  RSSISAMPLE;                        /*!< RSSI sample.                                                          */
  __I  uint32_t  RESERVED7;
  __I  uint32_t  STATE;                             /*!< Current radio state.                                                  */
  __IO uint32_t  DATAWHITEIV;                       /*!< Data whitening initial value.                                         */
  __I  uint32_t  RESERVED8[2];
  __IO uint32_t  BCC;                               /*!< Bit counter compare.                                                  */
  __I  uint32_t  RESERVED9[39];
  __IO uint32_t  DAB[8];                            /*!< Device address base segment.                                          */
  __IO uint32_t  DAP[8];                            /*!< Device address prefix.                                                */
  __IO uint32_t  DACNF;                             /*!< Device address match configuration.                                   */
  __I  uint32_t  RESERVED10[56];
  __IO uint32_t  OVERRIDE0;                         /*!< Trim value override register 0.                                       */
  __IO uint32_t  OVERRIDE1;                         /*!< Trim value override register 1.                                       */
  __IO uint32_t  OVERRIDE2;                         /*!< Trim value override register 2.                                       */
  __IO uint32_t  OVERRIDE3;                         /*!< Trim value override register 3.                                       */
  __IO uint32_t  OVERRIDE4;                         /*!< Trim value override register 4.                                       */
  __I  uint32_t  RESERVED11[561];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_RADIO_Type;


/* ================================================================================ */
/* ================                      UART                      ================ */
/* ================================================================================ */


/**
  * @brief Universal Asynchronous Receiver/Transmitter. (UART)
  */

typedef struct {                                    /*!< UART Structure                                                        */
  __O  uint32_t  TASKS_STARTRX;                     /*!< Start UART receiver.                                                  */
  __O  uint32_t  TASKS_STOPRX;                      /*!< Stop UART receiver.                                                   */
  __O  uint32_t  TASKS_STARTTX;                     /*!< Start UART transmitter.                                               */
  __O  uint32_t  TASKS_STOPTX;                      /*!< Stop UART transmitter.                                                */
  __I  uint32_t  RESERVED0[3];
  __O  uint32_t  TASKS_SUSPEND;                     /*!< Suspend UART.                                                         */
  __I  uint32_t  RESERVED1[56];
  __IO uint32_t  EVENTS_CTS;                        /*!< CTS activated.                                                        */
  __IO uint32_t  EVENTS_NCTS;                       /*!< CTS deactivated.                                                      */
  __IO uint32_t  EVENTS_RXDRDY;                     /*!< Data received in RXD.                                                 */
  __I  uint32_t  RESERVED2[4];
  __IO uint32_t  EVENTS_TXDRDY;                     /*!< Data sent from TXD.                                                   */
  __I  uint32_t  RESERVED3;
  __IO uint32_t  EVENTS_ERROR;                      /*!< Error detected.                                                       */
  __I  uint32_t  RESERVED4[7];
  __IO uint32_t  EVENTS_RXTO;                       /*!< Receiver timeout.                                                     */
  __I  uint32_t  RESERVED5[46];
  __IO uint32_t  SHORTS;                            /*!< Shortcuts for UART.                                                   */
  __I  uint32_t  RESERVED6[64];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED7[93];
  __IO uint32_t  ERRORSRC;                          /*!< Error source. Write error field to 1 to clear error.                  */
  __I  uint32_t  RESERVED8[31];
  __IO uint32_t  ENABLE;                            /*!< Enable UART and acquire IOs.                                          */
  __I  uint32_t  RESERVED9;
  __IO uint32_t  PSELRTS;                           /*!< Pin select for RTS.                                                   */
  __IO uint32_t  PSELTXD;                           /*!< Pin select for TXD.                                                   */
  __IO uint32_t  PSELCTS;                           /*!< Pin select for CTS.                                                   */
  __IO uint32_t  PSELRXD;                           /*!< Pin select for RXD.                                                   */
  __I  uint32_t  RXD;                               /*!< RXD register. On read action the buffer pointer is displaced.
                                                         Once read the character is consumed. If read when no character
                                                          available, the UART will stop working.                               */
  __O  uint32_t  TXD;                               /*!< TXD register.                                                         */
  __I  uint32_t  RESERVED10;
  __IO uint32_t  BAUDRATE;                          /*!< UART Baudrate.                                                        */
  __I  uint32_t  RESERVED11[17];
  __IO uint32_t  CONFIG;                            /*!< Configuration of parity and hardware flow control register.           */
  __I  uint32_t  RESERVED12[675];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_UART_Type;


/* ================================================================================ */
/* ================                       SPI                      ================ */
/* ================================================================================ */


/**
  * @brief SPI master 0. (SPI)
  */

typedef struct {                                    /*!< SPI Structure                                                         */
  __I  uint32_t  RESERVED0[66];
  __IO uint32_t  EVENTS_READY;                      /*!< TXD byte sent and RXD byte received.                                  */
  __I  uint32_t  RESERVED1[126];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED2[125];
  __IO uint32_t  ENABLE;                            /*!< Enable SPI.                                                           */
  __I  uint32_t  RESERVED3;
  __IO uint32_t  PSELSCK;                           /*!< Pin select for SCK.                                                   */
  __IO uint32_t  PSELMOSI;                          /*!< Pin select for MOSI.                                                  */
  __IO uint32_t  PSELMISO;                          /*!< Pin select for MISO.                                                  */
  __I  uint32_t  RESERVED4;
  __I  uint32_t  RXD;                               /*!< RX data.                                                              */
  __IO uint32_t  TXD;                               /*!< TX data.                                                              */
  __I  uint32_t  RESERVED5;
  __IO uint32_t  FREQUENCY;                         /*!< SPI frequency                                                         */
  __I  uint32_t  RESERVED6[11];
  __IO uint32_t  CONFIG;                            /*!< Configuration register.                                               */
  __I  uint32_t  RESERVED7[681];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_SPI_Type;


/* ================================================================================ */
/* ================                       TWI                      ================ */
/* ================================================================================ */


/**
  * @brief Two-wire interface master 0. (TWI)
  */

typedef struct {                                    /*!< TWI Structure                                                         */
  __O  uint32_t  TASKS_STARTRX;                     /*!< Start 2-Wire master receive sequence.                                 */
  __I  uint32_t  RESERVED0;
  __O  uint32_t  TASKS_STARTTX;                     /*!< Start 2-Wire master transmit sequence.                                */
  __I  uint32_t  RESERVED1[2];
  __O  uint32_t  TASKS_STOP;                        /*!< Stop 2-Wire transaction.                                              */
  __I  uint32_t  RESERVED2;
  __O  uint32_t  TASKS_SUSPEND;                     /*!< Suspend 2-Wire transaction.                                           */
  __O  uint32_t  TASKS_RESUME;                      /*!< Resume 2-Wire transaction.                                            */
  __I  uint32_t  RESERVED3[56];
  __IO uint32_t  EVENTS_STOPPED;                    /*!< Two-wire stopped.                                                     */
  __IO uint32_t  EVENTS_RXDREADY;                   /*!< Two-wire ready to deliver new RXD byte received.                      */
  __I  uint32_t  RESERVED4[4];
  __IO uint32_t  EVENTS_TXDSENT;                    /*!< Two-wire finished sending last TXD byte.                              */
  __I  uint32_t  RESERVED5;
  __IO uint32_t  EVENTS_ERROR;                      /*!< Two-wire error detected.                                              */
  __I  uint32_t  RESERVED6[4];
  __IO uint32_t  EVENTS_BB;                         /*!< Two-wire byte boundary.                                               */
  __I  uint32_t  RESERVED7[3];
  __IO uint32_t  EVENTS_SUSPENDED;                  /*!< Two-wire suspended.                                                   */
  __I  uint32_t  RESERVED8[45];
  __IO uint32_t  SHORTS;                            /*!< Shortcuts for TWI.                                                    */
  __I  uint32_t  RESERVED9[64];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED10[110];
  __IO uint32_t  ERRORSRC;                          /*!< Two-wire error source. Write error field to 1 to clear error.         */
  __I  uint32_t  RESERVED11[14];
  __IO uint32_t  ENABLE;                            /*!< Enable two-wire master.                                               */
  __I  uint32_t  RESERVED12;
  __IO uint32_t  PSELSCL;                           /*!< Pin select for SCL.                                                   */
  __IO uint32_t  PSELSDA;                           /*!< Pin select for SDA.                                                   */
  __I  uint32_t  RESERVED13[2];
  __I  uint32_t  RXD;                               /*!< RX data register.                                                     */
  __IO uint32_t  TXD;                               /*!< TX data register.                                                     */
  __I  uint32_t  RESERVED14;
  __IO uint32_t  FREQUENCY;                         /*!< Two-wire frequency.                                                   */
  __I  uint32_t  RESERVED15[24];
  __IO uint32_t  ADDRESS;                           /*!< Address used in the two-wire transfer.                                */
  __I  uint32_t  RESERVED16[668];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_TWI_Type;


/* ================================================================================ */
/* ================                      SPIS                      ================ */
/* ================================================================================ */


/**
  * @brief SPI slave 1. (SPIS)
  */

typedef struct {                                    /*!< SPIS Structure                                                        */
  __I  uint32_t  RESERVED0[9];
  __O  uint32_t  TASKS_ACQUIRE;                     /*!< Acquire SPI semaphore.                                                */
  __O  uint32_t  TASKS_RELEASE;                     /*!< Release SPI semaphore.                                                */
  __I  uint32_t  RESERVED1[54];
  __IO uint32_t  EVENTS_END;                        /*!< Granted transaction completed.                                        */
  __I  uint32_t  RESERVED2[2];
  __IO uint32_t  EVENTS_ENDRX;                      /*!< End of RXD buffer reached                                             */
  __I  uint32_t  RESERVED3[5];
  __IO uint32_t  EVENTS_ACQUIRED;                   /*!< Semaphore acquired.                                                   */
  __I  uint32_t  RESERVED4[53];
  __IO uint32_t  SHORTS;                            /*!< Shortcuts for SPIS.                                                   */
  __I  uint32_t  RESERVED5[64];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED6[61];
  __I  uint32_t  SEMSTAT;                           /*!< Semaphore status.                                                     */
  __I  uint32_t  RESERVED7[15];
  __IO uint32_t  STATUS;                            /*!< Status from last transaction.                                         */
  __I  uint32_t  RESERVED8[47];
  __IO uint32_t  ENABLE;                            /*!< Enable SPIS.                                                          */
  __I  uint32_t  RESERVED9;
  __IO uint32_t  PSELSCK;                           /*!< Pin select for SCK.                                                   */
  __IO uint32_t  PSELMISO;                          /*!< Pin select for MISO.                                                  */
  __IO uint32_t  PSELMOSI;                          /*!< Pin select for MOSI.                                                  */
  __IO uint32_t  PSELCSN;                           /*!< Pin select for CSN.                                                   */
  __I  uint32_t  RESERVED10[7];
  __IO uint32_t  RXDPTR;                            /*!< RX data pointer.                                                      */
  __IO uint32_t  MAXRX;                             /*!< Maximum number of bytes in the receive buffer.                        */
  __I  uint32_t  AMOUNTRX;                          /*!< Number of bytes received in last granted transaction.                 */
  __I  uint32_t  RESERVED11;
  __IO uint32_t  TXDPTR;                            /*!< TX data pointer.                                                      */
  __IO uint32_t  MAXTX;                             /*!< Maximum number of bytes in the transmit buffer.                       */
  __I  uint32_t  AMOUNTTX;                          /*!< Number of bytes transmitted in last granted transaction.              */
  __I  uint32_t  RESERVED12;
  __IO uint32_t  CONFIG;                            /*!< Configuration register.                                               */
  __I  uint32_t  RESERVED13;
  __IO uint32_t  DEF;                               /*!< Default character.                                                    */
  __I  uint32_t  RESERVED14[24];
  __IO uint32_t  ORC;                               /*!< Over-read character.                                                  */
  __I  uint32_t  RESERVED15[654];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_SPIS_Type;


/* ================================================================================ */
/* ================                     GPIOTE                     ================ */
/* ================================================================================ */


/**
  * @brief GPIO tasks and events. (GPIOTE)
  */

typedef struct {                                    /*!< GPIOTE Structure                                                      */
  __O  uint32_t  TASKS_OUT[4];                      /*!< Tasks asssociated with GPIOTE channels.                               */
  __I  uint32_t  RESERVED0[60];
  __IO uint32_t  EVENTS_IN[4];                      /*!< Tasks asssociated with GPIOTE channels.                               */
  __I  uint32_t  RESERVED1[27];
  __IO uint32_t  EVENTS_PORT;                       /*!< Event generated from multiple pins.                                   */
  __I  uint32_t  RESERVED2[97];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED3[129];
  __IO uint32_t  CONFIG[4];                         /*!< Channel configuration registers.                                      */
  __I  uint32_t  RESERVED4[695];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_GPIOTE_Type;


/* ================================================================================ */
/* ================                       ADC                      ================ */
/* ================================================================================ */


/**
  * @brief Analog to digital converter. (ADC)
  */

typedef struct {                                    /*!< ADC Structure                                                         */
  __O  uint32_t  TASKS_START;                       /*!< Start an ADC conversion.                                              */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop ADC.                                                             */
  __I  uint32_t  RESERVED0[62];
  __IO uint32_t  EVENTS_END;                        /*!< ADC conversion complete.                                              */
  __I  uint32_t  RESERVED1[128];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED2[61];
  __I  uint32_t  BUSY;                              /*!< ADC busy register.                                                    */
  __I  uint32_t  RESERVED3[63];
  __IO uint32_t  ENABLE;                            /*!< ADC enable.                                                           */
  __IO uint32_t  CONFIG;                            /*!< ADC configuration register.                                           */
  __I  uint32_t  RESULT;                            /*!< Result of ADC conversion.                                             */
  __I  uint32_t  RESERVED4[700];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_ADC_Type;


/* ================================================================================ */
/* ================                      TIMER                     ================ */
/* ================================================================================ */


/**
  * @brief Timer 0. (TIMER)
  */

typedef struct {                                    /*!< TIMER Structure                                                       */
  __O  uint32_t  TASKS_START;                       /*!< Start Timer.                                                          */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop Timer.                                                           */
  __O  uint32_t  TASKS_COUNT;                       /*!< Increment Timer (In counter mode).                                    */
  __O  uint32_t  TASKS_CLEAR;                       /*!< Clear timer.                                                          */
  __O  uint32_t  TASKS_SHUTDOWN;                    /*!< Shutdown timer.                                                       */
  __I  uint32_t  RESERVED0[11];
  __O  uint32_t  TASKS_CAPTURE[4];                  /*!< Capture Timer value to CC[n] registers.                               */
  __I  uint32_t  RESERVED1[60];
  __IO uint32_t  EVENTS_COMPARE[4];                 /*!< Compare event on CC[n] match.                                         */
  __I  uint32_t  RESERVED2[44];
  __IO uint32_t  SHORTS;                            /*!< Shortcuts for Timer.                                                  */
  __I  uint32_t  RESERVED3[64];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED4[126];
  __IO uint32_t  MODE;                              /*!< Timer Mode selection.                                                 */
  __IO uint32_t  BITMODE;                           /*!< Sets timer behaviour.                                                 */
  __I  uint32_t  RESERVED5;
  __IO uint32_t  PRESCALER;                         /*!< 4-bit prescaler to source clock frequency (max value 9). Source
                                                         clock frequency is divided by 2^SCALE.                                */
  __I  uint32_t  RESERVED6[11];
  __IO uint32_t  CC[4];                             /*!< Capture/compare registers.                                            */
  __I  uint32_t  RESERVED7[683];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_TIMER_Type;


/* ================================================================================ */
/* ================                       RTC                      ================ */
/* ================================================================================ */


/**
  * @brief Real time counter 0. (RTC)
  */

typedef struct {                                    /*!< RTC Structure                                                         */
  __O  uint32_t  TASKS_START;                       /*!< Start RTC Counter.                                                    */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop RTC Counter.                                                     */
  __O  uint32_t  TASKS_CLEAR;                       /*!< Clear RTC Counter.                                                    */
  __O  uint32_t  TASKS_TRIGOVRFLW;                  /*!< Set COUNTER to 0xFFFFFFF0.                                            */
  __I  uint32_t  RESERVED0[60];
  __IO uint32_t  EVENTS_TICK;                       /*!< Event on COUNTER increment.                                           */
  __IO uint32_t  EVENTS_OVRFLW;                     /*!< Event on COUNTER overflow.                                            */
  __I  uint32_t  RESERVED1[14];
  __IO uint32_t  EVENTS_COMPARE[4];                 /*!< Compare event on CC[n] match.                                         */
  __I  uint32_t  RESERVED2[109];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED3[13];
  __IO uint32_t  EVTEN;                             /*!< Configures event enable routing to PPI for each RTC event.            */
  __IO uint32_t  EVTENSET;                          /*!< Enable events routing to PPI. The reading of this register gives
                                                         the value of EVTEN.                                                   */
  __IO uint32_t  EVTENCLR;                          /*!< Disable events routing to PPI. The reading of this register
                                                         gives the value of EVTEN.                                             */
  __I  uint32_t  RESERVED4[110];
  __I  uint32_t  COUNTER;                           /*!< Current COUNTER value.                                                */
  __IO uint32_t  PRESCALER;                         /*!< 12-bit prescaler for COUNTER frequency (32768/(PRESCALER+1)).
                                                         Must be written when RTC is STOPed.                                   */
  __I  uint32_t  RESERVED5[13];
  __IO uint32_t  CC[4];                             /*!< Capture/compare registers.                                            */
  __I  uint32_t  RESERVED6[683];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_RTC_Type;


/* ================================================================================ */
/* ================                      TEMP                      ================ */
/* ================================================================================ */


/**
  * @brief Temperature Sensor. (TEMP)
  */

typedef struct {                                    /*!< TEMP Structure                                                        */
  __O  uint32_t  TASKS_START;                       /*!< Start temperature measurement.                                        */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop temperature measurement.                                         */
  __I  uint32_t  RESERVED0[62];
  __IO uint32_t  EVENTS_DATARDY;                    /*!< Temperature measurement complete, data ready event.                   */
  __I  uint32_t  RESERVED1[128];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED2[127];
  __I  int32_t   TEMP;                              /*!< Die temperature in degC, 2's complement format, 0.25 degC pecision.   */
  __I  uint32_t  RESERVED3[700];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_TEMP_Type;


/* ================================================================================ */
/* ================                       RNG                      ================ */
/* ================================================================================ */


/**
  * @brief Random Number Generator. (RNG)
  */

typedef struct {                                    /*!< RNG Structure                                                         */
  __O  uint32_t  TASKS_START;                       /*!< Start the random number generator.                                    */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop the random number generator.                                     */
  __I  uint32_t  RESERVED0[62];
  __IO uint32_t  EVENTS_VALRDY;                     /*!< New random number generated and written to VALUE register.            */
  __I  uint32_t  RESERVED1[63];
  __IO uint32_t  SHORTS;                            /*!< Shortcuts for the RNG.                                                */
  __I  uint32_t  RESERVED2[64];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register                                         */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register                                       */
  __I  uint32_t  RESERVED3[126];
  __IO uint32_t  CONFIG;                            /*!< Configuration register.                                               */
  __I  uint32_t  VALUE;                             /*!< RNG random number.                                                    */
  __I  uint32_t  RESERVED4[700];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_RNG_Type;


/* ================================================================================ */
/* ================                       ECB                      ================ */
/* ================================================================================ */


/**
  * @brief AES ECB Mode Encryption. (ECB)
  */

typedef struct {                                    /*!< ECB Structure                                                         */
  __O  uint32_t  TASKS_STARTECB;                    /*!< Start ECB block encrypt. If a crypto operation is running, this
                                                         will not initiate a new encryption and the ERRORECB event will
                                                          be triggered.                                                        */
  __O  uint32_t  TASKS_STOPECB;                     /*!< Stop current ECB encryption. If a crypto operation is running,
                                                         this will will trigger the ERRORECB event.                            */
  __I  uint32_t  RESERVED0[62];
  __IO uint32_t  EVENTS_ENDECB;                     /*!< ECB block encrypt complete.                                           */
  __IO uint32_t  EVENTS_ERRORECB;                   /*!< ECB block encrypt aborted due to a STOPECB task or due to an
                                                         error.                                                                */
  __I  uint32_t  RESERVED1[127];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED2[126];
  __IO uint32_t  ECBDATAPTR;                        /*!< ECB block encrypt memory pointer.                                     */
  __I  uint32_t  RESERVED3[701];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_ECB_Type;


/* ================================================================================ */
/* ================                       AAR                      ================ */
/* ================================================================================ */


/**
  * @brief Accelerated Address Resolver. (AAR)
  */

typedef struct {                                    /*!< AAR Structure                                                         */
  __O  uint32_t  TASKS_START;                       /*!< Start resolving addresses based on IRKs specified in the IRK
                                                         data structure.                                                       */
  __I  uint32_t  RESERVED0;
  __O  uint32_t  TASKS_STOP;                        /*!< Stop resolving addresses.                                             */
  __I  uint32_t  RESERVED1[61];
  __IO uint32_t  EVENTS_END;                        /*!< Address resolution procedure completed.                               */
  __IO uint32_t  EVENTS_RESOLVED;                   /*!< Address resolved.                                                     */
  __IO uint32_t  EVENTS_NOTRESOLVED;                /*!< Address not resolved.                                                 */
  __I  uint32_t  RESERVED2[126];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED3[61];
  __I  uint32_t  STATUS;                            /*!< Resolution status.                                                    */
  __I  uint32_t  RESERVED4[63];
  __IO uint32_t  ENABLE;                            /*!< Enable AAR.                                                           */
  __IO uint32_t  NIRK;                              /*!< Number of Identity root Keys in the IRK data structure.               */
  __IO uint32_t  IRKPTR;                            /*!< Pointer to the IRK data structure.                                    */
  __I  uint32_t  RESERVED5;
  __IO uint32_t  ADDRPTR;                           /*!< Pointer to the resolvable address (6 bytes).                          */
  __IO uint32_t  SCRATCHPTR;                        /*!< Pointer to a scratch data area used for temporary storage during
                                                         resolution. A minimum of 3 bytes must be reserved.                    */
  __I  uint32_t  RESERVED6[697];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_AAR_Type;


/* ================================================================================ */
/* ================                       CCM                      ================ */
/* ================================================================================ */


/**
  * @brief AES CCM Mode Encryption. (CCM)
  */

typedef struct {                                    /*!< CCM Structure                                                         */
  __O  uint32_t  TASKS_KSGEN;                       /*!< Start generation of key-stream. This operation will stop by
                                                         itself when completed.                                                */
  __O  uint32_t  TASKS_CRYPT;                       /*!< Start encrypt/decrypt. This operation will stop by itself when
                                                         completed.                                                            */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop encrypt/decrypt.                                                 */
  __I  uint32_t  RESERVED0[61];
  __IO uint32_t  EVENTS_ENDKSGEN;                   /*!< Keystream generation completed.                                       */
  __IO uint32_t  EVENTS_ENDCRYPT;                   /*!< Encrypt/decrypt completed.                                            */
  __IO uint32_t  EVENTS_ERROR;                      /*!< Error happened.                                                       */
  __I  uint32_t  RESERVED1[61];
  __IO uint32_t  SHORTS;                            /*!< Shortcuts for the CCM.                                                */
  __I  uint32_t  RESERVED2[64];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED3[61];
  __I  uint32_t  MICSTATUS;                         /*!< CCM RX MIC check result.                                              */
  __I  uint32_t  RESERVED4[63];
  __IO uint32_t  ENABLE;                            /*!< CCM enable.                                                           */
  __IO uint32_t  MODE;                              /*!< Operation mode.                                                       */
  __IO uint32_t  CNFPTR;                            /*!< Pointer to a data structure holding AES key and NONCE vector.         */
  __IO uint32_t  INPTR;                             /*!< Pointer to the input packet.                                          */
  __IO uint32_t  OUTPTR;                            /*!< Pointer to the output packet.                                         */
  __IO uint32_t  SCRATCHPTR;                        /*!< Pointer to a scratch data area used for temporary storage during
                                                         resolution. A minimum of 43 bytes must be reserved.                   */
  __I  uint32_t  RESERVED5[697];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_CCM_Type;


/* ================================================================================ */
/* ================                       WDT                      ================ */
/* ================================================================================ */


/**
  * @brief Watchdog Timer. (WDT)
  */

typedef struct {                                    /*!< WDT Structure                                                         */
  __O  uint32_t  TASKS_START;                       /*!< Start the watchdog.                                                   */
  __I  uint32_t  RESERVED0[63];
  __IO uint32_t  EVENTS_TIMEOUT;                    /*!< Watchdog timeout.                                                     */
  __I  uint32_t  RESERVED1[128];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED2[61];
  __I  uint32_t  RUNSTATUS;                         /*!< Watchdog running status.                                              */
  __I  uint32_t  REQSTATUS;                         /*!< Request status.                                                       */
  __I  uint32_t  RESERVED3[63];
  __IO uint32_t  CRV;                               /*!< Counter reload value in number of 32kiHz clock cycles.                */
  __IO uint32_t  RREN;                              /*!< Reload request enable.                                                */
  __IO uint32_t  CONFIG;                            /*!< Configuration register.                                               */
  __I  uint32_t  RESERVED4[60];
  __O  uint32_t  RR[8];                             /*!< Reload requests registers.                                            */
  __I  uint32_t  RESERVED5[631];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_WDT_Type;


/* ================================================================================ */
/* ================                      QDEC                      ================ */
/* ================================================================================ */


/**
  * @brief Rotary decoder. (QDEC)
  */

typedef struct {                                    /*!< QDEC Structure                                                        */
  __O  uint32_t  TASKS_START;                       /*!< Start the quadrature decoder.                                         */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop the quadrature decoder.                                          */
  __O  uint32_t  TASKS_READCLRACC;                  /*!< Transfers the content from ACC registers to ACCREAD registers,
                                                         and clears the ACC registers.                                         */
  __I  uint32_t  RESERVED0[61];
  __IO uint32_t  EVENTS_SAMPLERDY;                  /*!< A new sample is written to the sample register.                       */
  __IO uint32_t  EVENTS_REPORTRDY;                  /*!< REPORTPER number of samples accumulated in ACC register, and
                                                         ACC register different than zero.                                     */
  __IO uint32_t  EVENTS_ACCOF;                      /*!< ACC or ACCDBL register overflow.                                      */
  __I  uint32_t  RESERVED1[61];
  __IO uint32_t  SHORTS;                            /*!< Shortcuts for the QDEC.                                               */
  __I  uint32_t  RESERVED2[64];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED3[125];
  __IO uint32_t  ENABLE;                            /*!< Enable the QDEC.                                                      */
  __IO uint32_t  LEDPOL;                            /*!< LED output pin polarity.                                              */
  __IO uint32_t  SAMPLEPER;                         /*!< Sample period.                                                        */
  __I  int32_t   SAMPLE;                            /*!< Motion sample value.                                                  */
  __IO uint32_t  REPORTPER;                         /*!< Number of samples to generate an EVENT_REPORTRDY.                     */
  __I  int32_t   ACC;                               /*!< Accumulated valid transitions register.                               */
  __I  int32_t   ACCREAD;                           /*!< Snapshot of ACC register. Value generated by the TASKS_READCLEACC
                                                         task.                                                                 */
  __IO uint32_t  PSELLED;                           /*!< Pin select for LED output.                                            */
  __IO uint32_t  PSELA;                             /*!< Pin select for phase A input.                                         */
  __IO uint32_t  PSELB;                             /*!< Pin select for phase B input.                                         */
  __IO uint32_t  DBFEN;                             /*!< Enable debouncer input filters.                                       */
  __I  uint32_t  RESERVED4[5];
  __IO uint32_t  LEDPRE;                            /*!< Time LED is switched ON before the sample.                            */
  __I  uint32_t  ACCDBL;                            /*!< Accumulated double (error) transitions register.                      */
  __I  uint32_t  ACCDBLREAD;                        /*!< Snapshot of ACCDBL register. Value generated by the TASKS_READCLEACC
                                                         task.                                                                 */
  __I  uint32_t  RESERVED5[684];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_QDEC_Type;


/* ================================================================================ */
/* ================                     LPCOMP                     ================ */
/* ================================================================================ */


/**
  * @brief Low power comparator. (LPCOMP)
  */

typedef struct {                                    /*!< LPCOMP Structure                                                      */
  __O  uint32_t  TASKS_START;                       /*!< Start the comparator.                                                 */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop the comparator.                                                  */
  __O  uint32_t  TASKS_SAMPLE;                      /*!< Sample comparator value.                                              */
  __I  uint32_t  RESERVED0[61];
  __IO uint32_t  EVENTS_READY;                      /*!< LPCOMP is ready and output is valid.                                  */
  __IO uint32_t  EVENTS_DOWN;                       /*!< Input voltage crossed the threshold going down.                       */
  __IO uint32_t  EVENTS_UP;                         /*!< Input voltage crossed the threshold going up.                         */
  __IO uint32_t  EVENTS_CROSS;                      /*!< Input voltage crossed the threshold in any direction.                 */
  __I  uint32_t  RESERVED1[60];
  __IO uint32_t  SHORTS;                            /*!< Shortcuts for the LPCOMP.                                             */
  __I  uint32_t  RESERVED2[64];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED3[61];
  __I  uint32_t  RESULT;                            /*!< Result of last compare.                                               */
  __I  uint32_t  RESERVED4[63];
  __IO uint32_t  ENABLE;                            /*!< Enable the LPCOMP.                                                    */
  __IO uint32_t  PSEL;                              /*!< Input pin select.                                                     */
  __IO uint32_t  REFSEL;                            /*!< Reference select.                                                     */
  __IO uint32_t  EXTREFSEL;                         /*!< External reference select.                                            */
  __I  uint32_t  RESERVED5[4];
  __IO uint32_t  ANADETECT;                         /*!< Analog detect configuration.                                          */
  __I  uint32_t  RESERVED6[694];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_LPCOMP_Type;


/* ================================================================================ */
/* ================                       SWI                      ================ */
/* ================================================================================ */


/**
  * @brief SW Interrupts. (SWI)
  */

typedef struct {                                    /*!< SWI Structure                                                         */
  __I  uint32_t  UNUSED;                            /*!< Unused.                                                               */
} NRF_SWI_Type;


/* ================================================================================ */
/* ================                      NVMC                      ================ */
/* ================================================================================ */


/**
  * @brief Non Volatile Memory Controller. (NVMC)
  */

typedef struct {                                    /*!< NVMC Structure                                                        */
  __I  uint32_t  RESERVED0[256];
  __I  uint32_t  READY;                             /*!< Ready flag.                                                           */
  __I  uint32_t  RESERVED1[64];
  __IO uint32_t  CONFIG;                            /*!< Configuration register.                                               */
  
  union {
    __IO uint32_t  ERASEPCR1;                       /*!< Register for erasing a non-protected non-volatile memory page.        */
    __IO uint32_t  ERASEPAGE;                       /*!< Register for erasing a non-protected non-volatile memory page.        */
  };
  __IO uint32_t  ERASEALL;                          /*!< Register for erasing all non-volatile user memory.                    */
  __IO uint32_t  ERASEPCR0;                         /*!< Register for erasing a protected non-volatile memory page.            */
  __IO uint32_t  ERASEUICR;                         /*!< Register for start erasing User Information Congfiguration Registers. */
} NRF_NVMC_Type;


/* ================================================================================ */
/* ================                       PPI                      ================ */
/* ================================================================================ */


/**
  * @brief PPI controller. (PPI)
  */

typedef struct {                                    /*!< PPI Structure                                                         */
  PPI_TASKS_CHG_Type TASKS_CHG[4];                  /*!< Channel group tasks.                                                  */
  __I  uint32_t  RESERVED0[312];
  __IO uint32_t  CHEN;                              /*!< Channel enable.                                                       */
  __IO uint32_t  CHENSET;                           /*!< Channel enable set.                                                   */
  __IO uint32_t  CHENCLR;                           /*!< Channel enable clear.                                                 */
  __I  uint32_t  RESERVED1;
  PPI_CH_Type CH[16];                               /*!< PPI Channel.                                                          */
  __I  uint32_t  RESERVED2[156];
  __IO uint32_t  CHG[4];                            /*!< Channel group configuration.                                          */
} NRF_PPI_Type;


/* ================================================================================ */
/* ================                      FICR                      ================ */
/* ================================================================================ */


/**
  * @brief Factory Information Configuration. (FICR)
  */

typedef struct {                                    /*!< FICR Structure                                                        */
  __I  uint32_t  RESERVED0[4];
  __I  uint32_t  CODEPAGESIZE;                      /*!< Code memory page size in bytes.                                       */
  __I  uint32_t  CODESIZE;                          /*!< Code memory size in pages.                                            */
  __I  uint32_t  RESERVED1[4];
  __I  uint32_t  CLENR0;                            /*!< Length of code region 0 in bytes.                                     */
  __I  uint32_t  PPFC;                              /*!< Pre-programmed factory code present.                                  */
  __I  uint32_t  RESERVED2;
  __I  uint32_t  NUMRAMBLOCK;                       /*!< Number of individualy controllable RAM blocks.                        */
  
  union {
    __I  uint32_t  SIZERAMBLOCK[4];                 /*!< Deprecated array of size of RAM block in bytes. This name is
                                                         kept for backward compatinility purposes. Use SIZERAMBLOCKS
                                                          instead.                                                             */
    __I  uint32_t  SIZERAMBLOCKS;                   /*!< Size of RAM blocks in bytes.                                          */
  };
  __I  uint32_t  RESERVED3[5];
  __I  uint32_t  CONFIGID;                          /*!< Configuration identifier.                                             */
  __I  uint32_t  DEVICEID[2];                       /*!< Device identifier.                                                    */
  __I  uint32_t  RESERVED4[6];
  __I  uint32_t  ER[4];                             /*!< Encryption root.                                                      */
  __I  uint32_t  IR[4];                             /*!< Identity root.                                                        */
  __I  uint32_t  DEVICEADDRTYPE;                    /*!< Device address type.                                                  */
  __I  uint32_t  DEVICEADDR[2];                     /*!< Device address.                                                       */
  __I  uint32_t  OVERRIDEEN;                        /*!< Radio calibration override enable.                                    */
  __I  uint32_t  NRF_1MBIT[5];                      /*!< Override values for the OVERRIDEn registers in RADIO for NRF_1Mbit
                                                         mode.                                                                 */
  __I  uint32_t  RESERVED5[10];
  __I  uint32_t  BLE_1MBIT[5];                      /*!< Override values for the OVERRIDEn registers in RADIO for BLE_1Mbit
                                                         mode.                                                                 */
} NRF_FICR_Type;


/* ================================================================================ */
/* ================                      UICR                      ================ */
/* ================================================================================ */


/**
  * @brief User Information Configuration. (UICR)
  */

typedef struct {                                    /*!< UICR Structure                                                        */
  __IO uint32_t  CLENR0;                            /*!< Length of code region 0.                                              */
  __IO uint32_t  RBPCONF;                           /*!< Readback protection configuration.                                    */
  __IO uint32_t  XTALFREQ;                          /*!< Reset value for CLOCK XTALFREQ register.                              */
  __I  uint32_t  RESERVED0;
  __I  uint32_t  FWID;                              /*!< Firmware ID.                                                          */
  
  union {
    __IO uint32_t  NRFFW[15];                       /*!< Reserved for Nordic firmware design.                                  */
    __IO uint32_t  BOOTLOADERADDR;                  /*!< Bootloader start address.                                             */
  };
  __IO uint32_t  NRFHW[12];                         /*!< Reserved for Nordic hardware design.                                  */
  __IO uint32_t  CUSTOMER[32];                      /*!< Reserved for customer.                                                */
} NRF_UICR_Type;


/* ================================================================================ */
/* ================                      GPIO                      ================ */
/* ================================================================================ */


/**
  * @brief General purpose input and output. (GPIO)
  */

typedef struct {                                    /*!< GPIO Structure                                                        */
  __I  uint32_t  RESERVED0[321];
  __IO uint32_t  OUT;                               /*!< Write GPIO port.                                                      */
  __IO uint32_t  OUTSET;                            /*!< Set individual bits in GPIO port.                                     */
  __IO uint32_t  OUTCLR;                            /*!< Clear individual bits in GPIO port.                                   */
  __I  uint32_t  IN;                                /*!< Read GPIO port.                                                       */
  __IO uint32_t  DIR;                               /*!< Direction of GPIO pins.                                               */
  __IO uint32_t  DIRSET;                            /*!< DIR set register.                                                     */
  __IO uint32_t  DIRCLR;                            /*!< DIR clear register.                                                   */
  __I  uint32_t  RESERVED1[120];
  __IO uint32_t  PIN_CNF[32];                       /*!< Configuration of GPIO pins.                                           */
} NRF_GPIO_Type;


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

#define NRF_POWER_BASE                  0x40000000UL
#define NRF_CLOCK_BASE                  0x40000000UL
#define NRF_MPU_BASE                    0x40000000UL
#define NRF_RADIO_BASE                  0x40001000UL
#define NRF_UART0_BASE                  0x40002000UL
#define NRF_SPI0_BASE                   0x40003000UL
#define NRF_TWI0_BASE                   0x40003000UL
#define NRF_SPI1_BASE                   0x40004000UL
#define NRF_TWI1_BASE                   0x40004000UL
#define NRF_SPIS1_BASE                  0x40004000UL
#define NRF_GPIOTE_BASE                 0x40006000UL
#define NRF_ADC_BASE                    0x40007000UL
#define NRF_TIMER0_BASE                 0x40008000UL
#define NRF_TIMER1_BASE                 0x40009000UL
#define NRF_TIMER2_BASE                 0x4000A000UL
#define NRF_RTC0_BASE                   0x4000B000UL
#define NRF_TEMP_BASE                   0x4000C000UL
#define NRF_RNG_BASE                    0x4000D000UL
#define NRF_ECB_BASE                    0x4000E000UL
#define NRF_AAR_BASE                    0x4000F000UL
#define NRF_CCM_BASE                    0x4000F000UL
#define NRF_WDT_BASE                    0x40010000UL
#define NRF_RTC1_BASE                   0x40011000UL
#define NRF_QDEC_BASE                   0x40012000UL
#define NRF_LPCOMP_BASE                 0x40013000UL
#define NRF_SWI_BASE                    0x40014000UL
#define NRF_NVMC_BASE                   0x4001E000UL
#define NRF_PPI_BASE                    0x4001F000UL
#define NRF_FICR_BASE                   0x10000000UL
#define NRF_UICR_BASE                   0x10001000UL
#define NRF_GPIO_BASE                   0x50000000UL


/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */

#define NRF_POWER                       ((NRF_POWER_Type          *) NRF_POWER_BASE)
#define NRF_CLOCK                       ((NRF_CLOCK_Type          *) NRF_CLOCK_BASE)
#define NRF_MPU                         ((NRF_MPU_Type            *) NRF_MPU_BASE)
#define NRF_RADIO                       ((NRF_RADIO_Type          *) NRF_RADIO_BASE)
#define NRF_UART0                       ((NRF_UART_Type           *) NRF_UART0_BASE)
#define NRF_SPI0                        ((NRF_SPI_Type            *) NRF_SPI0_BASE)
#define NRF_TWI0                        ((NRF_TWI_Type            *) NRF_TWI0_BASE)
#define NRF_SPI1                        ((NRF_SPI_Type            *) NRF_SPI1_BASE)
#define NRF_TWI1                        ((NRF_TWI_Type            *) NRF_TWI1_BASE)
#define NRF_SPIS1                       ((NRF_SPIS_Type           *) NRF_SPIS1_BASE)
#define NRF_GPIOTE                      ((NRF_GPIOTE_Type         *) NRF_GPIOTE_BASE)
#define NRF_ADC                         ((NRF_ADC_Type            *) NRF_ADC_BASE)
#define NRF_TIMER0                      ((NRF_TIMER_Type          *) NRF_TIMER0_BASE)
#define NRF_TIMER1                      ((NRF_TIMER_Type          *) NRF_TIMER1_BASE)
#define NRF_TIMER2                      ((NRF_TIMER_Type          *) NRF_TIMER2_BASE)
#define NRF_RTC0                        ((NRF_RTC_Type            *) NRF_RTC0_BASE)
#define NRF_TEMP                        ((NRF_TEMP_Type           *) NRF_TEMP_BASE)
#define NRF_RNG                         ((NRF_RNG_Type            *) NRF_RNG_BASE)
#define NRF_ECB                         ((NRF_ECB_Type            *) NRF_ECB_BASE)
#define NRF_AAR                         ((NRF_AAR_Type            *) NRF_AAR_BASE)
#define NRF_CCM                         ((NRF_CCM_Type            *) NRF_CCM_BASE)
#define NRF_WDT                         ((NRF_WDT_Type            *) NRF_WDT_BASE)
#define NRF_RTC1                        ((NRF_RTC_Type            *) NRF_RTC1_BASE)
#define NRF_QDEC                        ((NRF_QDEC_Type           *) NRF_QDEC_BASE)
#define NRF_LPCOMP                      ((NRF_LPCOMP_Type         *) NRF_LPCOMP_BASE)
#define NRF_SWI                         ((NRF_SWI_Type            *) NRF_SWI_BASE)
#define NRF_NVMC                        ((NRF_NVMC_Type           *) NRF_NVMC_BASE)
#define NRF_PPI                         ((NRF_PPI_Type            *) NRF_PPI_BASE)
#define NRF_FICR                        ((NRF_FICR_Type           *) NRF_FICR_BASE)
#define NRF_UICR                        ((NRF_UICR_Type           *) NRF_UICR_BASE)
#define NRF_GPIO                        ((NRF_GPIO_Type           *) NRF_GPIO_BASE)


/** @} */ /* End of group Device_Peripheral_Registers */
/** @} */ /* End of group nrf51 */
/** @} */ /* End of group Nordic Semiconductor */

#ifdef __cplusplus
}
#endif


#endif  /* nrf51_H */

