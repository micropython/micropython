/**
 * \file
 *
 * \brief Header file for SAMD21G15B
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD21G15B_
#define _SAMD21G15B_

/**
 * \ingroup SAMD21_definitions
 * \addtogroup SAMD21G15B_definitions SAMD21G15B definitions
 * This file defines all structures and symbols for SAMD21G15B:
 *   - registers and bitfields
 *   - peripheral base address
 *   - peripheral ID
 *   - PIO definitions
*/
/*@{*/

#ifdef __cplusplus
 extern "C" {
#endif

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#include <stdint.h>
#ifndef __cplusplus
typedef volatile const uint32_t RoReg;   /**< Read only 32-bit register (volatile const unsigned int) */
typedef volatile const uint16_t RoReg16; /**< Read only 16-bit register (volatile const unsigned int) */
typedef volatile const uint8_t  RoReg8;  /**< Read only  8-bit register (volatile const unsigned int) */
#else
typedef volatile       uint32_t RoReg;   /**< Read only 32-bit register (volatile const unsigned int) */
typedef volatile       uint16_t RoReg16; /**< Read only 16-bit register (volatile const unsigned int) */
typedef volatile       uint8_t  RoReg8;  /**< Read only  8-bit register (volatile const unsigned int) */
#endif
typedef volatile       uint32_t WoReg;   /**< Write only 32-bit register (volatile unsigned int) */
typedef volatile       uint16_t WoReg16; /**< Write only 16-bit register (volatile unsigned int) */
typedef volatile       uint32_t WoReg8;  /**< Write only  8-bit register (volatile unsigned int) */
typedef volatile       uint32_t RwReg;   /**< Read-Write 32-bit register (volatile unsigned int) */
typedef volatile       uint16_t RwReg16; /**< Read-Write 16-bit register (volatile unsigned int) */
typedef volatile       uint8_t  RwReg8;  /**< Read-Write  8-bit register (volatile unsigned int) */
#define CAST(type, value) ((type *)(value))
#define REG_ACCESS(type, address) (*(type*)(address)) /**< C code: Register value */
#else
#define CAST(type, value) (value)
#define REG_ACCESS(type, address) (address) /**< Assembly code: Register address */
#endif

/* ************************************************************************** */
/**  CMSIS DEFINITIONS FOR SAMD21G15B */
/* ************************************************************************** */
/** \defgroup SAMD21G15B_cmsis CMSIS Definitions */
/*@{*/

/** Interrupt Number Definition */
typedef enum IRQn
{
  /******  Cortex-M0+ Processor Exceptions Numbers ******************************/
  NonMaskableInt_IRQn      = -14,/**<  2 Non Maskable Interrupt                 */
  HardFault_IRQn           = -13,/**<  3 Cortex-M0+ Hard Fault Interrupt        */
  SVCall_IRQn              = -5, /**< 11 Cortex-M0+ SV Call Interrupt           */
  PendSV_IRQn              = -2, /**< 14 Cortex-M0+ Pend SV Interrupt           */
  SysTick_IRQn             = -1, /**< 15 Cortex-M0+ System Tick Interrupt       */
  /******  SAMD21G15B-specific Interrupt Numbers ***********************/
  PM_IRQn                  =  0, /**<  0 SAMD21G15B Power Manager (PM) */
  SYSCTRL_IRQn             =  1, /**<  1 SAMD21G15B System Control (SYSCTRL) */
  WDT_IRQn                 =  2, /**<  2 SAMD21G15B Watchdog Timer (WDT) */
  RTC_IRQn                 =  3, /**<  3 SAMD21G15B Real-Time Counter (RTC) */
  EIC_IRQn                 =  4, /**<  4 SAMD21G15B External Interrupt Controller (EIC) */
  NVMCTRL_IRQn             =  5, /**<  5 SAMD21G15B Non-Volatile Memory Controller (NVMCTRL) */
  DMAC_IRQn                =  6, /**<  6 SAMD21G15B Direct Memory Access Controller (DMAC) */
  USB_IRQn                 =  7, /**<  7 SAMD21G15B Universal Serial Bus (USB) */
  EVSYS_IRQn               =  8, /**<  8 SAMD21G15B Event System Interface (EVSYS) */
  SERCOM0_IRQn             =  9, /**<  9 SAMD21G15B Serial Communication Interface 0 (SERCOM0) */
  SERCOM1_IRQn             = 10, /**< 10 SAMD21G15B Serial Communication Interface 1 (SERCOM1) */
  SERCOM2_IRQn             = 11, /**< 11 SAMD21G15B Serial Communication Interface 2 (SERCOM2) */
  SERCOM3_IRQn             = 12, /**< 12 SAMD21G15B Serial Communication Interface 3 (SERCOM3) */
  SERCOM4_IRQn             = 13, /**< 13 SAMD21G15B Serial Communication Interface 4 (SERCOM4) */
  SERCOM5_IRQn             = 14, /**< 14 SAMD21G15B Serial Communication Interface 5 (SERCOM5) */
  TCC0_IRQn                = 15, /**< 15 SAMD21G15B Timer Counter Control 0 (TCC0) */
  TCC1_IRQn                = 16, /**< 16 SAMD21G15B Timer Counter Control 1 (TCC1) */
  TCC2_IRQn                = 17, /**< 17 SAMD21G15B Timer Counter Control 2 (TCC2) */
  TC3_IRQn                 = 18, /**< 18 SAMD21G15B Basic Timer Counter 3 (TC3) */
  TC4_IRQn                 = 19, /**< 19 SAMD21G15B Basic Timer Counter 4 (TC4) */
  TC5_IRQn                 = 20, /**< 20 SAMD21G15B Basic Timer Counter 5 (TC5) */
  ADC_IRQn                 = 23, /**< 23 SAMD21G15B Analog Digital Converter (ADC) */
  AC_IRQn                  = 24, /**< 24 SAMD21G15B Analog Comparators  (AC) */
  DAC_IRQn                 = 25, /**< 25 SAMD21G15B Digital Analog Converter (DAC) */
  PTC_IRQn                 = 26, /**< 26 SAMD21G15B Peripheral Touch Controller (PTC) */
  I2S_IRQn                 = 27, /**< 27 SAMD21G15B Inter-IC Sound Interface (I2S) */

  PERIPH_COUNT_IRQn        = 29  /**< Number of peripheral IDs */
} IRQn_Type;

typedef struct _DeviceVectors
{
  /* Stack pointer */
  void* pvStack;

  /* Cortex-M handlers */
  void* pfnReset_Handler;
  void* pfnNMI_Handler;
  void* pfnHardFault_Handler;
  void* pvReservedM12;
  void* pvReservedM11;
  void* pvReservedM10;
  void* pvReservedM9;
  void* pvReservedM8;
  void* pvReservedM7;
  void* pvReservedM6;
  void* pfnSVC_Handler;
  void* pvReservedM4;
  void* pvReservedM3;
  void* pfnPendSV_Handler;
  void* pfnSysTick_Handler;

  /* Peripheral handlers */
  void* pfnPM_Handler;                    /*  0 Power Manager */
  void* pfnSYSCTRL_Handler;               /*  1 System Control */
  void* pfnWDT_Handler;                   /*  2 Watchdog Timer */
  void* pfnRTC_Handler;                   /*  3 Real-Time Counter */
  void* pfnEIC_Handler;                   /*  4 External Interrupt Controller */
  void* pfnNVMCTRL_Handler;               /*  5 Non-Volatile Memory Controller */
  void* pfnDMAC_Handler;                  /*  6 Direct Memory Access Controller */
  void* pfnUSB_Handler;                   /*  7 Universal Serial Bus */
  void* pfnEVSYS_Handler;                 /*  8 Event System Interface */
  void* pfnSERCOM0_Handler;               /*  9 Serial Communication Interface 0 */
  void* pfnSERCOM1_Handler;               /* 10 Serial Communication Interface 1 */
  void* pfnSERCOM2_Handler;               /* 11 Serial Communication Interface 2 */
  void* pfnSERCOM3_Handler;               /* 12 Serial Communication Interface 3 */
  void* pfnSERCOM4_Handler;               /* 13 Serial Communication Interface 4 */
  void* pfnSERCOM5_Handler;               /* 14 Serial Communication Interface 5 */
  void* pfnTCC0_Handler;                  /* 15 Timer Counter Control 0 */
  void* pfnTCC1_Handler;                  /* 16 Timer Counter Control 1 */
  void* pfnTCC2_Handler;                  /* 17 Timer Counter Control 2 */
  void* pfnTC3_Handler;                   /* 18 Basic Timer Counter 3 */
  void* pfnTC4_Handler;                   /* 19 Basic Timer Counter 4 */
  void* pfnTC5_Handler;                   /* 20 Basic Timer Counter 5 */
  void* pvReserved21;
  void* pvReserved22;
  void* pfnADC_Handler;                   /* 23 Analog Digital Converter */
  void* pfnAC_Handler;                    /* 24 Analog Comparators  */
  void* pfnDAC_Handler;                   /* 25 Digital Analog Converter */
  void* pfnPTC_Handler;                   /* 26 Peripheral Touch Controller */
  void* pfnI2S_Handler;                   /* 27 Inter-IC Sound Interface */
  void* pvReserved28;
} DeviceVectors;

/* Cortex-M0+ processor handlers */
void Reset_Handler               ( void );
void NMI_Handler                 ( void );
void HardFault_Handler           ( void );
void SVC_Handler                 ( void );
void PendSV_Handler              ( void );
void SysTick_Handler             ( void );

/* Peripherals handlers */
void PM_Handler                  ( void );
void SYSCTRL_Handler             ( void );
void WDT_Handler                 ( void );
void RTC_Handler                 ( void );
void EIC_Handler                 ( void );
void NVMCTRL_Handler             ( void );
void DMAC_Handler                ( void );
void USB_Handler                 ( void );
void EVSYS_Handler               ( void );
void SERCOM0_Handler             ( void );
void SERCOM1_Handler             ( void );
void SERCOM2_Handler             ( void );
void SERCOM3_Handler             ( void );
void SERCOM4_Handler             ( void );
void SERCOM5_Handler             ( void );
void TCC0_Handler                ( void );
void TCC1_Handler                ( void );
void TCC2_Handler                ( void );
void TC3_Handler                 ( void );
void TC4_Handler                 ( void );
void TC5_Handler                 ( void );
void ADC_Handler                 ( void );
void AC_Handler                  ( void );
void DAC_Handler                 ( void );
void PTC_Handler                 ( void );
void I2S_Handler                 ( void );

/*
 * \brief Configuration of the Cortex-M0+ Processor and Core Peripherals
 */

#define LITTLE_ENDIAN          1        
#define __CM0PLUS_REV          1         /*!< Core revision r0p1 */
#define __MPU_PRESENT          0         /*!< MPU present or not */
#define __NVIC_PRIO_BITS       2         /*!< Number of bits used for Priority Levels */
#define __VTOR_PRESENT         1         /*!< VTOR present or not */
#define __Vendor_SysTickConfig 0         /*!< Set to 1 if different SysTick Config is used */

/**
 * \brief CMSIS includes
 */

#include <core_cm0plus.h>
#if !defined DONT_USE_CMSIS_INIT
#include "system_samd21.h"
#endif /* DONT_USE_CMSIS_INIT */

/*@}*/

/* ************************************************************************** */
/**  SOFTWARE PERIPHERAL API DEFINITION FOR SAMD21G15B */
/* ************************************************************************** */
/** \defgroup SAMD21G15B_api Peripheral Software API */
/*@{*/

#include "component/ac.h"
#include "component/adc.h"
#include "component/dac.h"
#include "component/dmac.h"
#include "component/dsu.h"
#include "component/eic.h"
#include "component/evsys.h"
#include "component/gclk.h"
#include "component/hmatrixb.h"
#include "component/i2s.h"
#include "component/mtb.h"
#include "component/nvmctrl.h"
#include "component/pac.h"
#include "component/pm.h"
#include "component/port.h"
#include "component/rtc.h"
#include "component/sercom.h"
#include "component/sysctrl.h"
#include "component/tc.h"
#include "component/tcc.h"
#include "component/usb.h"
#include "component/wdt.h"
/*@}*/

/* ************************************************************************** */
/**  REGISTERS ACCESS DEFINITIONS FOR SAMD21G15B */
/* ************************************************************************** */
/** \defgroup SAMD21G15B_reg Registers Access Definitions */
/*@{*/

#include "instance/ac.h"
#include "instance/adc.h"
#include "instance/dac.h"
#include "instance/dmac.h"
#include "instance/dsu.h"
#include "instance/eic.h"
#include "instance/evsys.h"
#include "instance/gclk.h"
#include "instance/sbmatrix.h"
#include "instance/i2s.h"
#include "instance/mtb.h"
#include "instance/nvmctrl.h"
#include "instance/pac0.h"
#include "instance/pac1.h"
#include "instance/pac2.h"
#include "instance/pm.h"
#include "instance/port.h"
#include "instance/rtc.h"
#include "instance/sercom0.h"
#include "instance/sercom1.h"
#include "instance/sercom2.h"
#include "instance/sercom3.h"
#include "instance/sercom4.h"
#include "instance/sercom5.h"
#include "instance/sysctrl.h"
#include "instance/tc3.h"
#include "instance/tc4.h"
#include "instance/tc5.h"
#include "instance/tcc0.h"
#include "instance/tcc1.h"
#include "instance/tcc2.h"
#include "instance/usb.h"
#include "instance/wdt.h"
/*@}*/

/* ************************************************************************** */
/**  PERIPHERAL ID DEFINITIONS FOR SAMD21G15B */
/* ************************************************************************** */
/** \defgroup SAMD21G15B_id Peripheral Ids Definitions */
/*@{*/

// Peripheral instances on HPB0 bridge
#define ID_PAC0           0 /**< \brief Peripheral Access Controller 0 (PAC0) */
#define ID_PM             1 /**< \brief Power Manager (PM) */
#define ID_SYSCTRL        2 /**< \brief System Control (SYSCTRL) */
#define ID_GCLK           3 /**< \brief Generic Clock Generator (GCLK) */
#define ID_WDT            4 /**< \brief Watchdog Timer (WDT) */
#define ID_RTC            5 /**< \brief Real-Time Counter (RTC) */
#define ID_EIC            6 /**< \brief External Interrupt Controller (EIC) */

// Peripheral instances on HPB1 bridge
#define ID_PAC1          32 /**< \brief Peripheral Access Controller 1 (PAC1) */
#define ID_DSU           33 /**< \brief Device Service Unit (DSU) */
#define ID_NVMCTRL       34 /**< \brief Non-Volatile Memory Controller (NVMCTRL) */
#define ID_PORT          35 /**< \brief Port Module (PORT) */
#define ID_DMAC          36 /**< \brief Direct Memory Access Controller (DMAC) */
#define ID_USB           37 /**< \brief Universal Serial Bus (USB) */
#define ID_MTB           38 /**< \brief Cortex-M0+ Micro-Trace Buffer (MTB) */
#define ID_SBMATRIX      39 /**< \brief HSB Matrix (SBMATRIX) */

// Peripheral instances on HPB2 bridge
#define ID_PAC2          64 /**< \brief Peripheral Access Controller 2 (PAC2) */
#define ID_EVSYS         65 /**< \brief Event System Interface (EVSYS) */
#define ID_SERCOM0       66 /**< \brief Serial Communication Interface 0 (SERCOM0) */
#define ID_SERCOM1       67 /**< \brief Serial Communication Interface 1 (SERCOM1) */
#define ID_SERCOM2       68 /**< \brief Serial Communication Interface 2 (SERCOM2) */
#define ID_SERCOM3       69 /**< \brief Serial Communication Interface 3 (SERCOM3) */
#define ID_SERCOM4       70 /**< \brief Serial Communication Interface 4 (SERCOM4) */
#define ID_SERCOM5       71 /**< \brief Serial Communication Interface 5 (SERCOM5) */
#define ID_TCC0          72 /**< \brief Timer Counter Control 0 (TCC0) */
#define ID_TCC1          73 /**< \brief Timer Counter Control 1 (TCC1) */
#define ID_TCC2          74 /**< \brief Timer Counter Control 2 (TCC2) */
#define ID_TC3           75 /**< \brief Basic Timer Counter 3 (TC3) */
#define ID_TC4           76 /**< \brief Basic Timer Counter 4 (TC4) */
#define ID_TC5           77 /**< \brief Basic Timer Counter 5 (TC5) */
#define ID_ADC           80 /**< \brief Analog Digital Converter (ADC) */
#define ID_AC            81 /**< \brief Analog Comparators  (AC) */
#define ID_DAC           82 /**< \brief Digital Analog Converter (DAC) */
#define ID_PTC           83 /**< \brief Peripheral Touch Controller (PTC) */
#define ID_I2S           84 /**< \brief Inter-IC Sound Interface (I2S) */

#define ID_PERIPH_COUNT  85 /**< \brief Max number of peripheral IDs */
/*@}*/

/* ************************************************************************** */
/**  BASE ADDRESS DEFINITIONS FOR SAMD21G15B */
/* ************************************************************************** */
/** \defgroup SAMD21G15B_base Peripheral Base Address Definitions */
/*@{*/

#if defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)
#define AC                            (0x42004400UL) /**< \brief (AC) APB Base Address */
#define ADC                           (0x42004000UL) /**< \brief (ADC) APB Base Address */
#define DAC                           (0x42004800UL) /**< \brief (DAC) APB Base Address */
#define DMAC                          (0x41004800UL) /**< \brief (DMAC) APB Base Address */
#define DSU                           (0x41002000UL) /**< \brief (DSU) APB Base Address */
#define EIC                           (0x40001800UL) /**< \brief (EIC) APB Base Address */
#define EVSYS                         (0x42000400UL) /**< \brief (EVSYS) APB Base Address */
#define GCLK                          (0x40000C00UL) /**< \brief (GCLK) APB Base Address */
#define SBMATRIX                      (0x41007000UL) /**< \brief (SBMATRIX) APB Base Address */
#define I2S                           (0x42005000UL) /**< \brief (I2S) APB Base Address */
#define MTB                           (0x41006000UL) /**< \brief (MTB) APB Base Address */
#define NVMCTRL_AUX3                  (0x0080A000UL) /**< \brief (NVMCTRL) AUX3 Base Address */
#define NVMCTRL                       (0x41004000UL) /**< \brief (NVMCTRL) APB Base Address */
#define NVMCTRL_CAL                   (0x00800000UL) /**< \brief (NVMCTRL) CAL Base Address */
#define NVMCTRL_LOCKBIT               (0x00802000UL) /**< \brief (NVMCTRL) LOCKBIT Base Address */
#define NVMCTRL_OTP1                  (0x00806000UL) /**< \brief (NVMCTRL) OTP1 Base Address */
#define NVMCTRL_OTP2                  (0x00806008UL) /**< \brief (NVMCTRL) OTP2 Base Address */
#define NVMCTRL_OTP4                  (0x00806020UL) /**< \brief (NVMCTRL) OTP4 Base Address */
#define NVMCTRL_TEMP_LOG              (0x00806030UL) /**< \brief (NVMCTRL) TEMP_LOG Base Address */
#define NVMCTRL_USER                  (0x00804000UL) /**< \brief (NVMCTRL) USER Base Address */
#define PAC0                          (0x40000000UL) /**< \brief (PAC0) APB Base Address */
#define PAC1                          (0x41000000UL) /**< \brief (PAC1) APB Base Address */
#define PAC2                          (0x42000000UL) /**< \brief (PAC2) APB Base Address */
#define PM                            (0x40000400UL) /**< \brief (PM) APB Base Address */
#define PORT                          (0x41004400UL) /**< \brief (PORT) APB Base Address */
#define PORT_IOBUS                    (0x60000000UL) /**< \brief (PORT) IOBUS Base Address */
#define RTC                           (0x40001400UL) /**< \brief (RTC) APB Base Address */
#define SERCOM0                       (0x42000800UL) /**< \brief (SERCOM0) APB Base Address */
#define SERCOM1                       (0x42000C00UL) /**< \brief (SERCOM1) APB Base Address */
#define SERCOM2                       (0x42001000UL) /**< \brief (SERCOM2) APB Base Address */
#define SERCOM3                       (0x42001400UL) /**< \brief (SERCOM3) APB Base Address */
#define SERCOM4                       (0x42001800UL) /**< \brief (SERCOM4) APB Base Address */
#define SERCOM5                       (0x42001C00UL) /**< \brief (SERCOM5) APB Base Address */
#define SYSCTRL                       (0x40000800UL) /**< \brief (SYSCTRL) APB Base Address */
#define TC3                           (0x42002C00UL) /**< \brief (TC3) APB Base Address */
#define TC4                           (0x42003000UL) /**< \brief (TC4) APB Base Address */
#define TC5                           (0x42003400UL) /**< \brief (TC5) APB Base Address */
#define TCC0                          (0x42002000UL) /**< \brief (TCC0) APB Base Address */
#define TCC1                          (0x42002400UL) /**< \brief (TCC1) APB Base Address */
#define TCC2                          (0x42002800UL) /**< \brief (TCC2) APB Base Address */
#define USB                           (0x41005000UL) /**< \brief (USB) APB Base Address */
#define WDT                           (0x40001000UL) /**< \brief (WDT) APB Base Address */
#else
#define AC                ((Ac       *)0x42004400UL) /**< \brief (AC) APB Base Address */
#define AC_INST_NUM       1                          /**< \brief (AC) Number of instances */
#define AC_INSTS          { AC }                     /**< \brief (AC) Instances List */

#define ADC               ((Adc      *)0x42004000UL) /**< \brief (ADC) APB Base Address */
#define ADC_INST_NUM      1                          /**< \brief (ADC) Number of instances */
#define ADC_INSTS         { ADC }                    /**< \brief (ADC) Instances List */

#define DAC               ((Dac      *)0x42004800UL) /**< \brief (DAC) APB Base Address */
#define DAC_INST_NUM      1                          /**< \brief (DAC) Number of instances */
#define DAC_INSTS         { DAC }                    /**< \brief (DAC) Instances List */

#define DMAC              ((Dmac     *)0x41004800UL) /**< \brief (DMAC) APB Base Address */
#define DMAC_INST_NUM     1                          /**< \brief (DMAC) Number of instances */
#define DMAC_INSTS        { DMAC }                   /**< \brief (DMAC) Instances List */

#define DSU               ((Dsu      *)0x41002000UL) /**< \brief (DSU) APB Base Address */
#define DSU_INST_NUM      1                          /**< \brief (DSU) Number of instances */
#define DSU_INSTS         { DSU }                    /**< \brief (DSU) Instances List */

#define EIC               ((Eic      *)0x40001800UL) /**< \brief (EIC) APB Base Address */
#define EIC_INST_NUM      1                          /**< \brief (EIC) Number of instances */
#define EIC_INSTS         { EIC }                    /**< \brief (EIC) Instances List */

#define EVSYS             ((Evsys    *)0x42000400UL) /**< \brief (EVSYS) APB Base Address */
#define EVSYS_INST_NUM    1                          /**< \brief (EVSYS) Number of instances */
#define EVSYS_INSTS       { EVSYS }                  /**< \brief (EVSYS) Instances List */

#define GCLK              ((Gclk     *)0x40000C00UL) /**< \brief (GCLK) APB Base Address */
#define GCLK_INST_NUM     1                          /**< \brief (GCLK) Number of instances */
#define GCLK_INSTS        { GCLK }                   /**< \brief (GCLK) Instances List */

#define SBMATRIX          ((Hmatrixb *)0x41007000UL) /**< \brief (SBMATRIX) APB Base Address */
#define HMATRIXB_INST_NUM 1                          /**< \brief (HMATRIXB) Number of instances */
#define HMATRIXB_INSTS    { SBMATRIX }               /**< \brief (HMATRIXB) Instances List */

#define I2S               ((I2s      *)0x42005000UL) /**< \brief (I2S) APB Base Address */
#define I2S_INST_NUM      1                          /**< \brief (I2S) Number of instances */
#define I2S_INSTS         { I2S }                    /**< \brief (I2S) Instances List */

#define MTB               ((Mtb      *)0x41006000UL) /**< \brief (MTB) APB Base Address */
#define MTB_INST_NUM      1                          /**< \brief (MTB) Number of instances */
#define MTB_INSTS         { MTB }                    /**< \brief (MTB) Instances List */

#define NVMCTRL_AUX3                  (0x0080A000UL) /**< \brief (NVMCTRL) AUX3 Base Address */
#define NVMCTRL           ((Nvmctrl  *)0x41004000UL) /**< \brief (NVMCTRL) APB Base Address */
#define NVMCTRL_CAL                   (0x00800000UL) /**< \brief (NVMCTRL) CAL Base Address */
#define NVMCTRL_LOCKBIT               (0x00802000UL) /**< \brief (NVMCTRL) LOCKBIT Base Address */
#define NVMCTRL_OTP1                  (0x00806000UL) /**< \brief (NVMCTRL) OTP1 Base Address */
#define NVMCTRL_OTP2                  (0x00806008UL) /**< \brief (NVMCTRL) OTP2 Base Address */
#define NVMCTRL_OTP4                  (0x00806020UL) /**< \brief (NVMCTRL) OTP4 Base Address */
#define NVMCTRL_TEMP_LOG              (0x00806030UL) /**< \brief (NVMCTRL) TEMP_LOG Base Address */
#define NVMCTRL_USER                  (0x00804000UL) /**< \brief (NVMCTRL) USER Base Address */
#define NVMCTRL_INST_NUM  1                          /**< \brief (NVMCTRL) Number of instances */
#define NVMCTRL_INSTS     { NVMCTRL }                /**< \brief (NVMCTRL) Instances List */

#define PAC0              ((Pac      *)0x40000000UL) /**< \brief (PAC0) APB Base Address */
#define PAC1              ((Pac      *)0x41000000UL) /**< \brief (PAC1) APB Base Address */
#define PAC2              ((Pac      *)0x42000000UL) /**< \brief (PAC2) APB Base Address */
#define PAC_INST_NUM      3                          /**< \brief (PAC) Number of instances */
#define PAC_INSTS         { PAC0, PAC1, PAC2 }       /**< \brief (PAC) Instances List */

#define PM                ((Pm       *)0x40000400UL) /**< \brief (PM) APB Base Address */
#define PM_INST_NUM       1                          /**< \brief (PM) Number of instances */
#define PM_INSTS          { PM }                     /**< \brief (PM) Instances List */

#define PORT              ((Port     *)0x41004400UL) /**< \brief (PORT) APB Base Address */
#define PORT_IOBUS        ((Port     *)0x60000000UL) /**< \brief (PORT) IOBUS Base Address */
#define PORT_INST_NUM     1                          /**< \brief (PORT) Number of instances */
#define PORT_INSTS        { PORT }                   /**< \brief (PORT) Instances List */

#define PTC_GCLK_ID       34
#define PTC_INST_NUM      1                          /**< \brief (PTC) Number of instances */
#define PTC_INSTS         { PTC }                    /**< \brief (PTC) Instances List */

#define RTC               ((Rtc      *)0x40001400UL) /**< \brief (RTC) APB Base Address */
#define RTC_INST_NUM      1                          /**< \brief (RTC) Number of instances */
#define RTC_INSTS         { RTC }                    /**< \brief (RTC) Instances List */

#define SERCOM0           ((Sercom   *)0x42000800UL) /**< \brief (SERCOM0) APB Base Address */
#define SERCOM1           ((Sercom   *)0x42000C00UL) /**< \brief (SERCOM1) APB Base Address */
#define SERCOM2           ((Sercom   *)0x42001000UL) /**< \brief (SERCOM2) APB Base Address */
#define SERCOM3           ((Sercom   *)0x42001400UL) /**< \brief (SERCOM3) APB Base Address */
#define SERCOM4           ((Sercom   *)0x42001800UL) /**< \brief (SERCOM4) APB Base Address */
#define SERCOM5           ((Sercom   *)0x42001C00UL) /**< \brief (SERCOM5) APB Base Address */
#define SERCOM_INST_NUM   6                          /**< \brief (SERCOM) Number of instances */
#define SERCOM_INSTS      { SERCOM0, SERCOM1, SERCOM2, SERCOM3, SERCOM4, SERCOM5 } /**< \brief (SERCOM) Instances List */

#define SYSCTRL           ((Sysctrl  *)0x40000800UL) /**< \brief (SYSCTRL) APB Base Address */
#define SYSCTRL_INST_NUM  1                          /**< \brief (SYSCTRL) Number of instances */
#define SYSCTRL_INSTS     { SYSCTRL }                /**< \brief (SYSCTRL) Instances List */

#define TC3               ((Tc       *)0x42002C00UL) /**< \brief (TC3) APB Base Address */
#define TC4               ((Tc       *)0x42003000UL) /**< \brief (TC4) APB Base Address */
#define TC5               ((Tc       *)0x42003400UL) /**< \brief (TC5) APB Base Address */
#define TC_INST_NUM       3                          /**< \brief (TC) Number of instances */
#define TC_INSTS          { TC3, TC4, TC5 }          /**< \brief (TC) Instances List */

#define TCC0              ((Tcc      *)0x42002000UL) /**< \brief (TCC0) APB Base Address */
#define TCC1              ((Tcc      *)0x42002400UL) /**< \brief (TCC1) APB Base Address */
#define TCC2              ((Tcc      *)0x42002800UL) /**< \brief (TCC2) APB Base Address */
#define TCC_INST_NUM      3                          /**< \brief (TCC) Number of instances */
#define TCC_INSTS         { TCC0, TCC1, TCC2 }       /**< \brief (TCC) Instances List */

#define USB               ((Usb      *)0x41005000UL) /**< \brief (USB) APB Base Address */
#define USB_INST_NUM      1                          /**< \brief (USB) Number of instances */
#define USB_INSTS         { USB }                    /**< \brief (USB) Instances List */

#define WDT               ((Wdt      *)0x40001000UL) /**< \brief (WDT) APB Base Address */
#define WDT_INST_NUM      1                          /**< \brief (WDT) Number of instances */
#define WDT_INSTS         { WDT }                    /**< \brief (WDT) Instances List */

#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/*@}*/

/* ************************************************************************** */
/**  PORT DEFINITIONS FOR SAMD21G15B */
/* ************************************************************************** */
/** \defgroup SAMD21G15B_port PORT Definitions */
/*@{*/

#include "pio/samd21g15b.h"
/*@}*/

/* ************************************************************************** */
/**  MEMORY MAPPING DEFINITIONS FOR SAMD21G15B */
/* ************************************************************************** */

#define FLASH_SIZE            0x8000UL /* 32 kB */
#define FLASH_PAGE_SIZE       64
#define FLASH_NB_OF_PAGES     512
#define FLASH_USER_PAGE_SIZE  64
#define HMCRAMC0_SIZE         0x1000UL /* 4 kB */

#define FLASH_ADDR            (0x00000000u) /**< FLASH base address */
#define FLASH_USER_PAGE_ADDR  (0x00800000u) /**< FLASH_USER_PAGE base address */
#define HMCRAMC0_ADDR         (0x20000000u) /**< HMCRAMC0 base address */
#define HPB0_ADDR             (0x40000000u) /**< HPB0 base address */
#define HPB1_ADDR             (0x41000000u) /**< HPB1 base address */
#define HPB2_ADDR             (0x42000000u) /**< HPB2 base address */
#define PPB_ADDR              (0xE0000000u) /**< PPB base address */

#define DSU_DID_RESETVALUE    0x10011424UL
#define EIC_EXTINT_NUM        16
#define NVMCTRL_RWW_EEPROM_SIZE 0x400UL /* 1 kB */
#define PORT_GROUPS           2
#define USB_HOST              1

/* ************************************************************************** */
/**  ELECTRICAL DEFINITIONS FOR SAMD21G15B */
/* ************************************************************************** */


#ifdef __cplusplus
}
#endif

/*@}*/

#endif /* SAMD21G15B_H */
