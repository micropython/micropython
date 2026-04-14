/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * CMSIS SC300 Core Peripheral Access Layer Header File
 */

#if   defined ( __ICCARM__ )
  #pragma system_include                        /* treat file as system include file for MISRA check */
#elif defined (__clang__)
  #pragma clang system_header                   /* treat file as system include file */
#elif defined ( __GNUC__ )
  #pragma GCC diagnostic ignored "-Wpedantic"   /* disable pedantic warning due to unnamed structs/unions */
#endif

#ifndef __CORE_SC300_H_GENERIC
#define __CORE_SC300_H_GENERIC

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

/**
  \page CMSIS_MISRA_Exceptions  MISRA-C:2004 Compliance Exceptions
  CMSIS violates the following MISRA-C:2004 rules:

   \li Required Rule 8.5, object/function definition in header file.<br>
     Function definitions in header files are used to allow 'inlining'.

   \li Required Rule 18.4, declaration of union type or object of union type: '{...}'.<br>
     Unions are used for effective representation of core registers.

   \li Advisory Rule 19.7, Function-like macro defined.<br>
     Function-like macros are used to allow more efficient code.
 */


/*******************************************************************************
 *                 CMSIS definitions
 ******************************************************************************/
/**
  \ingroup SC3000
  @{
 */

#include "cmsis_version.h"

/* CMSIS SC300 definitions */

#define __CORTEX_SC               (300U)                              /*!< Cortex Secure Core */

/** __FPU_USED indicates whether an FPU is used or not.
    This core does not support an FPU at all
*/
#define __FPU_USED       0U

#if defined ( __CC_ARM )
  #if defined (__TARGET_FPU_VFP)
    #error "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
  #endif

#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #if defined (__ARM_FP)
    #error "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
  #endif

#elif defined (__ti__)
  #if defined (__ARM_FP)
    #error "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
  #endif

#elif defined ( __GNUC__ )
  #if defined (__VFP_FP__) && !defined(__SOFTFP__)
    #error "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
  #endif

#elif defined ( __ICCARM__ )
  #if defined (__ARMVFP__)
    #error "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
  #endif

#elif defined ( __TI_ARM__ )
  #if defined (__TI_VFP_SUPPORT__)
    #error "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
  #endif

#elif defined ( __TASKING__ )
  #if defined (__FPU_VFP__)
    #error "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
  #endif

#elif defined ( __CSMC__ )
  #if ( __CSMC__ & 0x400U)
    #error "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
  #endif

#endif

#include "cmsis_compiler.h"               /* CMSIS compiler specific defines */


#ifdef __cplusplus
}
#endif

#endif /* __CORE_SC300_H_GENERIC */

#ifndef __CMSIS_GENERIC

#ifndef __CORE_SC300_H_DEPENDANT
#define __CORE_SC300_H_DEPENDANT

#ifdef __cplusplus
 extern "C" {
#endif

/* check device defines and use defaults */
#if defined __CHECK_DEVICE_DEFINES
  #ifndef __SC300_REV
    #define __SC300_REV               0x0000U
    #warning "__SC300_REV not defined in device header file; using default!"
  #endif

  #ifndef __MPU_PRESENT
    #define __MPU_PRESENT             0U
    #warning "__MPU_PRESENT not defined in device header file; using default!"
  #endif

  #ifndef __VTOR_PRESENT
    #define __VTOR_PRESENT            1U
    #warning "__VTOR_PRESENT not defined in device header file; using default!"
  #endif

  #ifndef __NVIC_PRIO_BITS
    #define __NVIC_PRIO_BITS          3U
    #warning "__NVIC_PRIO_BITS not defined in device header file; using default!"
  #endif

  #ifndef __Vendor_SysTickConfig
    #define __Vendor_SysTickConfig    0U
    #warning "__Vendor_SysTickConfig not defined in device header file; using default!"
  #endif
#endif

/* IO definitions (access restrictions to peripheral registers) */
/**
    \defgroup CMSIS_glob_defs CMSIS Global Defines

    <strong>IO Type Qualifiers</strong> are used
    \li to specify the access to peripheral variables.
    \li for automatic generation of peripheral register debug information.
*/
#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions */
#define     __IO    volatile             /*!< Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
#define     __OM     volatile            /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */

/*@} end of group SC300 */



/*******************************************************************************
 *                 Register Abstraction
  Core Register contain:
  - Core Register
  - Core NVIC Register
  - Core SCB Register
  - Core SysTick Register
  - Core Debug Register
  - Core MPU Register
 ******************************************************************************/
/**
  \defgroup CMSIS_core_register Defines and Type Definitions
  \brief Type definitions and defines for Cortex-M processor based devices.
*/

/**
  \ingroup    CMSIS_core_register
  \defgroup   CMSIS_CORE  Status and Control Registers
  \brief      Core Register type definitions.
  @{
 */

/**
  \brief  Union type to access the Application Program Status Register (APSR).
 */
typedef union
{
  struct
  {
    uint32_t _reserved0:27;              /*!< bit:  0..26  Reserved */
    uint32_t Q:1;                        /*!< bit:     27  Saturation condition flag */
    uint32_t V:1;                        /*!< bit:     28  Overflow condition code flag */
    uint32_t C:1;                        /*!< bit:     29  Carry condition code flag */
    uint32_t Z:1;                        /*!< bit:     30  Zero condition code flag */
    uint32_t N:1;                        /*!< bit:     31  Negative condition code flag */
  } b;                                   /*!< Structure used for bit  access */
  uint32_t w;                            /*!< Type      used for word access */
} APSR_Type;

/** \brief APSR Register Definitions */
#define APSR_N_Pos                         31U                                            /*!< APSR: N Position */
#define APSR_N_Msk                         (1UL << APSR_N_Pos)                            /*!< APSR: N Mask */

#define APSR_Z_Pos                         30U                                            /*!< APSR: Z Position */
#define APSR_Z_Msk                         (1UL << APSR_Z_Pos)                            /*!< APSR: Z Mask */

#define APSR_C_Pos                         29U                                            /*!< APSR: C Position */
#define APSR_C_Msk                         (1UL << APSR_C_Pos)                            /*!< APSR: C Mask */

#define APSR_V_Pos                         28U                                            /*!< APSR: V Position */
#define APSR_V_Msk                         (1UL << APSR_V_Pos)                            /*!< APSR: V Mask */

#define APSR_Q_Pos                         27U                                            /*!< APSR: Q Position */
#define APSR_Q_Msk                         (1UL << APSR_Q_Pos)                            /*!< APSR: Q Mask */


/**
  \brief  Union type to access the Interrupt Program Status Register (IPSR).
 */
typedef union
{
  struct
  {
    uint32_t ISR:9;                      /*!< bit:  0.. 8  Exception number */
    uint32_t _reserved0:23;              /*!< bit:  9..31  Reserved */
  } b;                                   /*!< Structure used for bit  access */
  uint32_t w;                            /*!< Type      used for word access */
} IPSR_Type;

/** \brief IPSR Register Definitions */
#define IPSR_ISR_Pos                        0U                                            /*!< IPSR: ISR Position */
#define IPSR_ISR_Msk                       (0x1FFUL /*<< IPSR_ISR_Pos*/)                  /*!< IPSR: ISR Mask */


/**
  \brief  Union type to access the Special-Purpose Program Status Registers (xPSR).
 */
typedef union
{
  struct
  {
    uint32_t ISR:9;                      /*!< bit:  0.. 8  Exception number */
    uint32_t _reserved0:1;               /*!< bit:      9  Reserved */
    uint32_t ICI_IT_1:6;                 /*!< bit: 10..15  ICI/IT part 1 */
    uint32_t _reserved1:8;               /*!< bit: 16..23  Reserved */
    uint32_t T:1;                        /*!< bit:     24  Thumb bit */
    uint32_t ICI_IT_2:2;                 /*!< bit: 25..26  ICI/IT part 2 */
    uint32_t Q:1;                        /*!< bit:     27  Saturation condition flag */
    uint32_t V:1;                        /*!< bit:     28  Overflow condition code flag */
    uint32_t C:1;                        /*!< bit:     29  Carry condition code flag */
    uint32_t Z:1;                        /*!< bit:     30  Zero condition code flag */
    uint32_t N:1;                        /*!< bit:     31  Negative condition code flag */
  } b;                                   /*!< Structure used for bit  access */
  uint32_t w;                            /*!< Type      used for word access */
} xPSR_Type;

/** \brief xPSR Register Definitions */
#define xPSR_N_Pos                         31U                                            /*!< xPSR: N Position */
#define xPSR_N_Msk                         (1UL << xPSR_N_Pos)                            /*!< xPSR: N Mask */

#define xPSR_Z_Pos                         30U                                            /*!< xPSR: Z Position */
#define xPSR_Z_Msk                         (1UL << xPSR_Z_Pos)                            /*!< xPSR: Z Mask */

#define xPSR_C_Pos                         29U                                            /*!< xPSR: C Position */
#define xPSR_C_Msk                         (1UL << xPSR_C_Pos)                            /*!< xPSR: C Mask */

#define xPSR_V_Pos                         28U                                            /*!< xPSR: V Position */
#define xPSR_V_Msk                         (1UL << xPSR_V_Pos)                            /*!< xPSR: V Mask */

#define xPSR_Q_Pos                         27U                                            /*!< xPSR: Q Position */
#define xPSR_Q_Msk                         (1UL << xPSR_Q_Pos)                            /*!< xPSR: Q Mask */

#define xPSR_ICI_IT_2_Pos                  25U                                            /*!< xPSR: ICI/IT part 2 Position */
#define xPSR_ICI_IT_2_Msk                  (3UL << xPSR_ICI_IT_2_Pos)                     /*!< xPSR: ICI/IT part 2 Mask */

#define xPSR_T_Pos                         24U                                            /*!< xPSR: T Position */
#define xPSR_T_Msk                         (1UL << xPSR_T_Pos)                            /*!< xPSR: T Mask */

#define xPSR_ICI_IT_1_Pos                  10U                                            /*!< xPSR: ICI/IT part 1 Position */
#define xPSR_ICI_IT_1_Msk                  (0x3FUL << xPSR_ICI_IT_1_Pos)                  /*!< xPSR: ICI/IT part 1 Mask */

#define xPSR_ISR_Pos                        0U                                            /*!< xPSR: ISR Position */
#define xPSR_ISR_Msk                       (0x1FFUL /*<< xPSR_ISR_Pos*/)                  /*!< xPSR: ISR Mask */


/**
  \brief  Union type to access the Control Registers (CONTROL).
 */
typedef union
{
  struct
  {
    uint32_t nPRIV:1;                    /*!< bit:      0  Execution privilege in Thread mode */
    uint32_t SPSEL:1;                    /*!< bit:      1  Stack to be used */
    uint32_t _reserved1:30;              /*!< bit:  2..31  Reserved */
  } b;                                   /*!< Structure used for bit  access */
  uint32_t w;                            /*!< Type      used for word access */
} CONTROL_Type;

/** \brief CONTROL Register Definitions */
#define CONTROL_SPSEL_Pos                   1U                                            /*!< CONTROL: SPSEL Position */
#define CONTROL_SPSEL_Msk                  (1UL << CONTROL_SPSEL_Pos)                     /*!< CONTROL: SPSEL Mask */

#define CONTROL_nPRIV_Pos                   0U                                            /*!< CONTROL: nPRIV Position */
#define CONTROL_nPRIV_Msk                  (1UL /*<< CONTROL_nPRIV_Pos*/)                 /*!< CONTROL: nPRIV Mask */

/*@} end of group CMSIS_CORE */


/**
  \ingroup    CMSIS_core_register
  \defgroup   CMSIS_NVIC  Nested Vectored Interrupt Controller (NVIC)
  \brief      Type definitions for the NVIC Registers
  @{
 */

/**
  \brief  Structure type to access the Nested Vectored Interrupt Controller (NVIC).
 */
typedef struct
{
  __IOM uint32_t ISER[8U];               /*!< Offset: 0x000 (R/W)  Interrupt Set Enable Register */
        uint32_t RESERVED0[24U];
  __IOM uint32_t ICER[8U];               /*!< Offset: 0x080 (R/W)  Interrupt Clear Enable Register */
        uint32_t RESERVED1[24U];
  __IOM uint32_t ISPR[8U];               /*!< Offset: 0x100 (R/W)  Interrupt Set Pending Register */
        uint32_t RESERVED2[24U];
  __IOM uint32_t ICPR[8U];               /*!< Offset: 0x180 (R/W)  Interrupt Clear Pending Register */
        uint32_t RESERVED3[24U];
  __IOM uint32_t IABR[8U];               /*!< Offset: 0x200 (R/W)  Interrupt Active bit Register */
        uint32_t RESERVED4[56U];
  __IOM uint8_t  IPR[240U];              /*!< Offset: 0x300 (R/W)  Interrupt Priority Register (8Bit wide) */
        uint32_t RESERVED5[644U];
  __OM  uint32_t STIR;                   /*!< Offset: 0xE00 ( /W)  Software Trigger Interrupt Register */
}  NVIC_Type;

/** \brief NVIC Software Triggered Interrupt Register Definitions */
#define NVIC_STIR_INTID_Pos                 0U                                         /*!< STIR: INTLINESNUM Position */
#define NVIC_STIR_INTID_Msk                (0x1FFUL /*<< NVIC_STIR_INTID_Pos*/)        /*!< STIR: INTLINESNUM Mask */

/*@} end of group CMSIS_NVIC */


/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_SCB     System Control Block (SCB)
  \brief    Type definitions for the System Control Block Registers
  @{
 */

/**
  \brief  Structure type to access the System Control Block (SCB).
 */
typedef struct
{
  __IM  uint32_t CPUID;                  /*!< Offset: 0x000 (R/ )  CPUID Base Register */
  __IOM uint32_t ICSR;                   /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register */
  __IOM uint32_t VTOR;                   /*!< Offset: 0x008 (R/W)  Vector Table Offset Register */
  __IOM uint32_t AIRCR;                  /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register */
  __IOM uint32_t SCR;                    /*!< Offset: 0x010 (R/W)  System Control Register */
  __IOM uint32_t CCR;                    /*!< Offset: 0x014 (R/W)  Configuration Control Register */
  __IOM uint8_t  SHPR[12U];              /*!< Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
  __IOM uint32_t SHCSR;                  /*!< Offset: 0x024 (R/W)  System Handler Control and State Register */
  __IOM uint32_t CFSR;                   /*!< Offset: 0x028 (R/W)  Configurable Fault Status Register */
  __IOM uint32_t HFSR;                   /*!< Offset: 0x02C (R/W)  HardFault Status Register */
  __IOM uint32_t DFSR;                   /*!< Offset: 0x030 (R/W)  Debug Fault Status Register */
  __IOM uint32_t MMFAR;                  /*!< Offset: 0x034 (R/W)  MemManage Fault Address Register */
  __IOM uint32_t BFAR;                   /*!< Offset: 0x038 (R/W)  BusFault Address Register */
  __IOM uint32_t AFSR;                   /*!< Offset: 0x03C (R/W)  Auxiliary Fault Status Register */
  __IM  uint32_t ID_PFR[2U];             /*!< Offset: 0x040 (R/ )  Processor Feature Register */
  __IM  uint32_t ID_DFR;                 /*!< Offset: 0x048 (R/ )  Debug Feature Register */
  __IM  uint32_t ID_AFR;                 /*!< Offset: 0x04C (R/ )  Auxiliary Feature Register */
  __IM  uint32_t ID_MMFR[4U];            /*!< Offset: 0x050 (R/ )  Memory Model Feature Register */
  __IM  uint32_t ID_ISAR[5U];            /*!< Offset: 0x060 (R/ )  Instruction Set Attributes Register */
        uint32_t RESERVED0[5U];
  __IOM uint32_t CPACR;                  /*!< Offset: 0x088 (R/W)  Coprocessor Access Control Register */
        uint32_t RESERVED1[129U];
  __IOM uint32_t SFCR;                   /*!< Offset: 0x290 (R/W)  Security Features Control Register */
} SCB_Type;

/** \brief SCB CPUID Register Definitions */
#define SCB_CPUID_IMPLEMENTER_Pos          24U                                            /*!< SCB CPUID: IMPLEMENTER Position */
#define SCB_CPUID_IMPLEMENTER_Msk          (0xFFUL << SCB_CPUID_IMPLEMENTER_Pos)          /*!< SCB CPUID: IMPLEMENTER Mask */

#define SCB_CPUID_VARIANT_Pos              20U                                            /*!< SCB CPUID: VARIANT Position */
#define SCB_CPUID_VARIANT_Msk              (0xFUL << SCB_CPUID_VARIANT_Pos)               /*!< SCB CPUID: VARIANT Mask */

#define SCB_CPUID_ARCHITECTURE_Pos         16U                                            /*!< SCB CPUID: ARCHITECTURE Position */
#define SCB_CPUID_ARCHITECTURE_Msk         (0xFUL << SCB_CPUID_ARCHITECTURE_Pos)          /*!< SCB CPUID: ARCHITECTURE Mask */

#define SCB_CPUID_PARTNO_Pos                4U                                            /*!< SCB CPUID: PARTNO Position */
#define SCB_CPUID_PARTNO_Msk               (0xFFFUL << SCB_CPUID_PARTNO_Pos)              /*!< SCB CPUID: PARTNO Mask */

#define SCB_CPUID_REVISION_Pos              0U                                            /*!< SCB CPUID: REVISION Position */
#define SCB_CPUID_REVISION_Msk             (0xFUL /*<< SCB_CPUID_REVISION_Pos*/)          /*!< SCB CPUID: REVISION Mask */

/** \brief SCB Interrupt Control State Register Definitions */
#define SCB_ICSR_NMIPENDSET_Pos            31U                                            /*!< SCB ICSR: NMIPENDSET Position */
#define SCB_ICSR_NMIPENDSET_Msk            (1UL << SCB_ICSR_NMIPENDSET_Pos)               /*!< SCB ICSR: NMIPENDSET Mask */

#define SCB_ICSR_PENDSVSET_Pos             28U                                            /*!< SCB ICSR: PENDSVSET Position */
#define SCB_ICSR_PENDSVSET_Msk             (1UL << SCB_ICSR_PENDSVSET_Pos)                /*!< SCB ICSR: PENDSVSET Mask */

#define SCB_ICSR_PENDSVCLR_Pos             27U                                            /*!< SCB ICSR: PENDSVCLR Position */
#define SCB_ICSR_PENDSVCLR_Msk             (1UL << SCB_ICSR_PENDSVCLR_Pos)                /*!< SCB ICSR: PENDSVCLR Mask */

#define SCB_ICSR_PENDSTSET_Pos             26U                                            /*!< SCB ICSR: PENDSTSET Position */
#define SCB_ICSR_PENDSTSET_Msk             (1UL << SCB_ICSR_PENDSTSET_Pos)                /*!< SCB ICSR: PENDSTSET Mask */

#define SCB_ICSR_PENDSTCLR_Pos             25U                                            /*!< SCB ICSR: PENDSTCLR Position */
#define SCB_ICSR_PENDSTCLR_Msk             (1UL << SCB_ICSR_PENDSTCLR_Pos)                /*!< SCB ICSR: PENDSTCLR Mask */

#define SCB_ICSR_ISRPREEMPT_Pos            23U                                            /*!< SCB ICSR: ISRPREEMPT Position */
#define SCB_ICSR_ISRPREEMPT_Msk            (1UL << SCB_ICSR_ISRPREEMPT_Pos)               /*!< SCB ICSR: ISRPREEMPT Mask */

#define SCB_ICSR_ISRPENDING_Pos            22U                                            /*!< SCB ICSR: ISRPENDING Position */
#define SCB_ICSR_ISRPENDING_Msk            (1UL << SCB_ICSR_ISRPENDING_Pos)               /*!< SCB ICSR: ISRPENDING Mask */

#define SCB_ICSR_VECTPENDING_Pos           12U                                            /*!< SCB ICSR: VECTPENDING Position */
#define SCB_ICSR_VECTPENDING_Msk           (0x1FFUL << SCB_ICSR_VECTPENDING_Pos)          /*!< SCB ICSR: VECTPENDING Mask */

#define SCB_ICSR_RETTOBASE_Pos             11U                                            /*!< SCB ICSR: RETTOBASE Position */
#define SCB_ICSR_RETTOBASE_Msk             (1UL << SCB_ICSR_RETTOBASE_Pos)                /*!< SCB ICSR: RETTOBASE Mask */

#define SCB_ICSR_VECTACTIVE_Pos             0U                                            /*!< SCB ICSR: VECTACTIVE Position */
#define SCB_ICSR_VECTACTIVE_Msk            (0x1FFUL /*<< SCB_ICSR_VECTACTIVE_Pos*/)       /*!< SCB ICSR: VECTACTIVE Mask */

/** \brief SCB Vector Table Offset Register Definitions */
#define SCB_VTOR_TBLBASE_Pos               29U                                            /*!< SCB VTOR: TBLBASE Position */
#define SCB_VTOR_TBLBASE_Msk               (1UL << SCB_VTOR_TBLBASE_Pos)                  /*!< SCB VTOR: TBLBASE Mask */

#define SCB_VTOR_TBLOFF_Pos                 7U                                            /*!< SCB VTOR: TBLOFF Position */
#define SCB_VTOR_TBLOFF_Msk                (0x3FFFFFUL << SCB_VTOR_TBLOFF_Pos)            /*!< SCB VTOR: TBLOFF Mask */

/** \brief SCB Application Interrupt and Reset Control Register Definitions */
#define SCB_AIRCR_VECTKEY_Pos              16U                                            /*!< SCB AIRCR: VECTKEY Position */
#define SCB_AIRCR_VECTKEY_Msk              (0xFFFFUL << SCB_AIRCR_VECTKEY_Pos)            /*!< SCB AIRCR: VECTKEY Mask */

#define SCB_AIRCR_VECTKEYSTAT_Pos          16U                                            /*!< SCB AIRCR: VECTKEYSTAT Position */
#define SCB_AIRCR_VECTKEYSTAT_Msk          (0xFFFFUL << SCB_AIRCR_VECTKEYSTAT_Pos)        /*!< SCB AIRCR: VECTKEYSTAT Mask */

#define SCB_AIRCR_ENDIANNESS_Pos           15U                                            /*!< SCB AIRCR: ENDIANNESS Position */
#define SCB_AIRCR_ENDIANNESS_Msk           (1UL << SCB_AIRCR_ENDIANNESS_Pos)              /*!< SCB AIRCR: ENDIANNESS Mask */

#define SCB_AIRCR_PRIGROUP_Pos              8U                                            /*!< SCB AIRCR: PRIGROUP Position */
#define SCB_AIRCR_PRIGROUP_Msk             (7UL << SCB_AIRCR_PRIGROUP_Pos)                /*!< SCB AIRCR: PRIGROUP Mask */

#define SCB_AIRCR_SYSRESETREQ_Pos           2U                                            /*!< SCB AIRCR: SYSRESETREQ Position */
#define SCB_AIRCR_SYSRESETREQ_Msk          (1UL << SCB_AIRCR_SYSRESETREQ_Pos)             /*!< SCB AIRCR: SYSRESETREQ Mask */

#define SCB_AIRCR_VECTCLRACTIVE_Pos         1U                                            /*!< SCB AIRCR: VECTCLRACTIVE Position */
#define SCB_AIRCR_VECTCLRACTIVE_Msk        (1UL << SCB_AIRCR_VECTCLRACTIVE_Pos)           /*!< SCB AIRCR: VECTCLRACTIVE Mask */

#define SCB_AIRCR_VECTRESET_Pos             0U                                            /*!< SCB AIRCR: VECTRESET Position */
#define SCB_AIRCR_VECTRESET_Msk            (1UL /*<< SCB_AIRCR_VECTRESET_Pos*/)           /*!< SCB AIRCR: VECTRESET Mask */

/** \brief SCB System Control Register Definitions */
#define SCB_SCR_SEVONPEND_Pos               4U                                            /*!< SCB SCR: SEVONPEND Position */
#define SCB_SCR_SEVONPEND_Msk              (1UL << SCB_SCR_SEVONPEND_Pos)                 /*!< SCB SCR: SEVONPEND Mask */

#define SCB_SCR_SLEEPDEEP_Pos               2U                                            /*!< SCB SCR: SLEEPDEEP Position */
#define SCB_SCR_SLEEPDEEP_Msk              (1UL << SCB_SCR_SLEEPDEEP_Pos)                 /*!< SCB SCR: SLEEPDEEP Mask */

#define SCB_SCR_SLEEPONEXIT_Pos             1U                                            /*!< SCB SCR: SLEEPONEXIT Position */
#define SCB_SCR_SLEEPONEXIT_Msk            (1UL << SCB_SCR_SLEEPONEXIT_Pos)               /*!< SCB SCR: SLEEPONEXIT Mask */

/** \brief SCB Configuration Control Register Definitions */
#define SCB_CCR_STKALIGN_Pos                9U                                            /*!< SCB CCR: STKALIGN Position */
#define SCB_CCR_STKALIGN_Msk               (1UL << SCB_CCR_STKALIGN_Pos)                  /*!< SCB CCR: STKALIGN Mask */

#define SCB_CCR_BFHFNMIGN_Pos               8U                                            /*!< SCB CCR: BFHFNMIGN Position */
#define SCB_CCR_BFHFNMIGN_Msk              (1UL << SCB_CCR_BFHFNMIGN_Pos)                 /*!< SCB CCR: BFHFNMIGN Mask */

#define SCB_CCR_DIV_0_TRP_Pos               4U                                            /*!< SCB CCR: DIV_0_TRP Position */
#define SCB_CCR_DIV_0_TRP_Msk              (1UL << SCB_CCR_DIV_0_TRP_Pos)                 /*!< SCB CCR: DIV_0_TRP Mask */

#define SCB_CCR_UNALIGN_TRP_Pos             3U                                            /*!< SCB CCR: UNALIGN_TRP Position */
#define SCB_CCR_UNALIGN_TRP_Msk            (1UL << SCB_CCR_UNALIGN_TRP_Pos)               /*!< SCB CCR: UNALIGN_TRP Mask */

#define SCB_CCR_USERSETMPEND_Pos            1U                                            /*!< SCB CCR: USERSETMPEND Position */
#define SCB_CCR_USERSETMPEND_Msk           (1UL << SCB_CCR_USERSETMPEND_Pos)              /*!< SCB CCR: USERSETMPEND Mask */

#define SCB_CCR_NONBASETHRDENA_Pos          0U                                            /*!< SCB CCR: NONBASETHRDENA Position */
#define SCB_CCR_NONBASETHRDENA_Msk         (1UL /*<< SCB_CCR_NONBASETHRDENA_Pos*/)        /*!< SCB CCR: NONBASETHRDENA Mask */

/** \brief SCB System Handler Control and State Register Definitions */
#define SCB_SHCSR_USGFAULTENA_Pos          18U                                            /*!< SCB SHCSR: USGFAULTENA Position */
#define SCB_SHCSR_USGFAULTENA_Msk          (1UL << SCB_SHCSR_USGFAULTENA_Pos)             /*!< SCB SHCSR: USGFAULTENA Mask */

#define SCB_SHCSR_BUSFAULTENA_Pos          17U                                            /*!< SCB SHCSR: BUSFAULTENA Position */
#define SCB_SHCSR_BUSFAULTENA_Msk          (1UL << SCB_SHCSR_BUSFAULTENA_Pos)             /*!< SCB SHCSR: BUSFAULTENA Mask */

#define SCB_SHCSR_MEMFAULTENA_Pos          16U                                            /*!< SCB SHCSR: MEMFAULTENA Position */
#define SCB_SHCSR_MEMFAULTENA_Msk          (1UL << SCB_SHCSR_MEMFAULTENA_Pos)             /*!< SCB SHCSR: MEMFAULTENA Mask */

#define SCB_SHCSR_SVCALLPENDED_Pos         15U                                            /*!< SCB SHCSR: SVCALLPENDED Position */
#define SCB_SHCSR_SVCALLPENDED_Msk         (1UL << SCB_SHCSR_SVCALLPENDED_Pos)            /*!< SCB SHCSR: SVCALLPENDED Mask */

#define SCB_SHCSR_BUSFAULTPENDED_Pos       14U                                            /*!< SCB SHCSR: BUSFAULTPENDED Position */
#define SCB_SHCSR_BUSFAULTPENDED_Msk       (1UL << SCB_SHCSR_BUSFAULTPENDED_Pos)          /*!< SCB SHCSR: BUSFAULTPENDED Mask */

#define SCB_SHCSR_MEMFAULTPENDED_Pos       13U                                            /*!< SCB SHCSR: MEMFAULTPENDED Position */
#define SCB_SHCSR_MEMFAULTPENDED_Msk       (1UL << SCB_SHCSR_MEMFAULTPENDED_Pos)          /*!< SCB SHCSR: MEMFAULTPENDED Mask */

#define SCB_SHCSR_USGFAULTPENDED_Pos       12U                                            /*!< SCB SHCSR: USGFAULTPENDED Position */
#define SCB_SHCSR_USGFAULTPENDED_Msk       (1UL << SCB_SHCSR_USGFAULTPENDED_Pos)          /*!< SCB SHCSR: USGFAULTPENDED Mask */

#define SCB_SHCSR_SYSTICKACT_Pos           11U                                            /*!< SCB SHCSR: SYSTICKACT Position */
#define SCB_SHCSR_SYSTICKACT_Msk           (1UL << SCB_SHCSR_SYSTICKACT_Pos)              /*!< SCB SHCSR: SYSTICKACT Mask */

#define SCB_SHCSR_PENDSVACT_Pos            10U                                            /*!< SCB SHCSR: PENDSVACT Position */
#define SCB_SHCSR_PENDSVACT_Msk            (1UL << SCB_SHCSR_PENDSVACT_Pos)               /*!< SCB SHCSR: PENDSVACT Mask */

#define SCB_SHCSR_MONITORACT_Pos            8U                                            /*!< SCB SHCSR: MONITORACT Position */
#define SCB_SHCSR_MONITORACT_Msk           (1UL << SCB_SHCSR_MONITORACT_Pos)              /*!< SCB SHCSR: MONITORACT Mask */

#define SCB_SHCSR_SVCALLACT_Pos             7U                                            /*!< SCB SHCSR: SVCALLACT Position */
#define SCB_SHCSR_SVCALLACT_Msk            (1UL << SCB_SHCSR_SVCALLACT_Pos)               /*!< SCB SHCSR: SVCALLACT Mask */

#define SCB_SHCSR_USGFAULTACT_Pos           3U                                            /*!< SCB SHCSR: USGFAULTACT Position */
#define SCB_SHCSR_USGFAULTACT_Msk          (1UL << SCB_SHCSR_USGFAULTACT_Pos)             /*!< SCB SHCSR: USGFAULTACT Mask */

#define SCB_SHCSR_BUSFAULTACT_Pos           1U                                            /*!< SCB SHCSR: BUSFAULTACT Position */
#define SCB_SHCSR_BUSFAULTACT_Msk          (1UL << SCB_SHCSR_BUSFAULTACT_Pos)             /*!< SCB SHCSR: BUSFAULTACT Mask */

#define SCB_SHCSR_MEMFAULTACT_Pos           0U                                            /*!< SCB SHCSR: MEMFAULTACT Position */
#define SCB_SHCSR_MEMFAULTACT_Msk          (1UL /*<< SCB_SHCSR_MEMFAULTACT_Pos*/)         /*!< SCB SHCSR: MEMFAULTACT Mask */

/** \brief SCB Configurable Fault Status Register Definitions */
#define SCB_CFSR_USGFAULTSR_Pos            16U                                            /*!< SCB CFSR: Usage Fault Status Register Position */
#define SCB_CFSR_USGFAULTSR_Msk            (0xFFFFUL << SCB_CFSR_USGFAULTSR_Pos)          /*!< SCB CFSR: Usage Fault Status Register Mask */

#define SCB_CFSR_BUSFAULTSR_Pos             8U                                            /*!< SCB CFSR: Bus Fault Status Register Position */
#define SCB_CFSR_BUSFAULTSR_Msk            (0xFFUL << SCB_CFSR_BUSFAULTSR_Pos)            /*!< SCB CFSR: Bus Fault Status Register Mask */

#define SCB_CFSR_MEMFAULTSR_Pos             0U                                            /*!< SCB CFSR: Memory Manage Fault Status Register Position */
#define SCB_CFSR_MEMFAULTSR_Msk            (0xFFUL /*<< SCB_CFSR_MEMFAULTSR_Pos*/)        /*!< SCB CFSR: Memory Manage Fault Status Register Mask */

/** \brief SCB MemManage Fault Status Register Definitions (part of SCB Configurable Fault Status Register) */
#define SCB_CFSR_MMARVALID_Pos             (SCB_CFSR_MEMFAULTSR_Pos + 7U)                 /*!< SCB CFSR (MMFSR): MMARVALID Position */
#define SCB_CFSR_MMARVALID_Msk             (1UL << SCB_CFSR_MMARVALID_Pos)                /*!< SCB CFSR (MMFSR): MMARVALID Mask */

#define SCB_CFSR_MSTKERR_Pos               (SCB_CFSR_MEMFAULTSR_Pos + 4U)                 /*!< SCB CFSR (MMFSR): MSTKERR Position */
#define SCB_CFSR_MSTKERR_Msk               (1UL << SCB_CFSR_MSTKERR_Pos)                  /*!< SCB CFSR (MMFSR): MSTKERR Mask */

#define SCB_CFSR_MUNSTKERR_Pos             (SCB_CFSR_MEMFAULTSR_Pos + 3U)                 /*!< SCB CFSR (MMFSR): MUNSTKERR Position */
#define SCB_CFSR_MUNSTKERR_Msk             (1UL << SCB_CFSR_MUNSTKERR_Pos)                /*!< SCB CFSR (MMFSR): MUNSTKERR Mask */

#define SCB_CFSR_DACCVIOL_Pos              (SCB_CFSR_MEMFAULTSR_Pos + 1U)                 /*!< SCB CFSR (MMFSR): DACCVIOL Position */
#define SCB_CFSR_DACCVIOL_Msk              (1UL << SCB_CFSR_DACCVIOL_Pos)                 /*!< SCB CFSR (MMFSR): DACCVIOL Mask */

#define SCB_CFSR_IACCVIOL_Pos              (SCB_CFSR_MEMFAULTSR_Pos + 0U)                 /*!< SCB CFSR (MMFSR): IACCVIOL Position */
#define SCB_CFSR_IACCVIOL_Msk              (1UL /*<< SCB_CFSR_IACCVIOL_Pos*/)             /*!< SCB CFSR (MMFSR): IACCVIOL Mask */

/** \brief SCB BusFault Status Register Definitions (part of SCB Configurable Fault Status Register) */
#define SCB_CFSR_BFARVALID_Pos            (SCB_CFSR_BUSFAULTSR_Pos + 7U)                  /*!< SCB CFSR (BFSR): BFARVALID Position */
#define SCB_CFSR_BFARVALID_Msk            (1UL << SCB_CFSR_BFARVALID_Pos)                 /*!< SCB CFSR (BFSR): BFARVALID Mask */

#define SCB_CFSR_STKERR_Pos               (SCB_CFSR_BUSFAULTSR_Pos + 4U)                  /*!< SCB CFSR (BFSR): STKERR Position */
#define SCB_CFSR_STKERR_Msk               (1UL << SCB_CFSR_STKERR_Pos)                    /*!< SCB CFSR (BFSR): STKERR Mask */

#define SCB_CFSR_UNSTKERR_Pos             (SCB_CFSR_BUSFAULTSR_Pos + 3U)                  /*!< SCB CFSR (BFSR): UNSTKERR Position */
#define SCB_CFSR_UNSTKERR_Msk             (1UL << SCB_CFSR_UNSTKERR_Pos)                  /*!< SCB CFSR (BFSR): UNSTKERR Mask */

#define SCB_CFSR_IMPRECISERR_Pos          (SCB_CFSR_BUSFAULTSR_Pos + 2U)                  /*!< SCB CFSR (BFSR): IMPRECISERR Position */
#define SCB_CFSR_IMPRECISERR_Msk          (1UL << SCB_CFSR_IMPRECISERR_Pos)               /*!< SCB CFSR (BFSR): IMPRECISERR Mask */

#define SCB_CFSR_PRECISERR_Pos            (SCB_CFSR_BUSFAULTSR_Pos + 1U)                  /*!< SCB CFSR (BFSR): PRECISERR Position */
#define SCB_CFSR_PRECISERR_Msk            (1UL << SCB_CFSR_PRECISERR_Pos)                 /*!< SCB CFSR (BFSR): PRECISERR Mask */

#define SCB_CFSR_IBUSERR_Pos              (SCB_CFSR_BUSFAULTSR_Pos + 0U)                  /*!< SCB CFSR (BFSR): IBUSERR Position */
#define SCB_CFSR_IBUSERR_Msk              (1UL << SCB_CFSR_IBUSERR_Pos)                   /*!< SCB CFSR (BFSR): IBUSERR Mask */

/** \brief SCB UsageFault Status Register Definitions (part of SCB Configurable Fault Status Register) */
#define SCB_CFSR_DIVBYZERO_Pos            (SCB_CFSR_USGFAULTSR_Pos + 9U)                  /*!< SCB CFSR (UFSR): DIVBYZERO Position */
#define SCB_CFSR_DIVBYZERO_Msk            (1UL << SCB_CFSR_DIVBYZERO_Pos)                 /*!< SCB CFSR (UFSR): DIVBYZERO Mask */

#define SCB_CFSR_UNALIGNED_Pos            (SCB_CFSR_USGFAULTSR_Pos + 8U)                  /*!< SCB CFSR (UFSR): UNALIGNED Position */
#define SCB_CFSR_UNALIGNED_Msk            (1UL << SCB_CFSR_UNALIGNED_Pos)                 /*!< SCB CFSR (UFSR): UNALIGNED Mask */

#define SCB_CFSR_NOCP_Pos                 (SCB_CFSR_USGFAULTSR_Pos + 3U)                  /*!< SCB CFSR (UFSR): NOCP Position */
#define SCB_CFSR_NOCP_Msk                 (1UL << SCB_CFSR_NOCP_Pos)                      /*!< SCB CFSR (UFSR): NOCP Mask */

#define SCB_CFSR_INVPC_Pos                (SCB_CFSR_USGFAULTSR_Pos + 2U)                  /*!< SCB CFSR (UFSR): INVPC Position */
#define SCB_CFSR_INVPC_Msk                (1UL << SCB_CFSR_INVPC_Pos)                     /*!< SCB CFSR (UFSR): INVPC Mask */

#define SCB_CFSR_INVSTATE_Pos             (SCB_CFSR_USGFAULTSR_Pos + 1U)                  /*!< SCB CFSR (UFSR): INVSTATE Position */
#define SCB_CFSR_INVSTATE_Msk             (1UL << SCB_CFSR_INVSTATE_Pos)                  /*!< SCB CFSR (UFSR): INVSTATE Mask */

#define SCB_CFSR_UNDEFINSTR_Pos           (SCB_CFSR_USGFAULTSR_Pos + 0U)                  /*!< SCB CFSR (UFSR): UNDEFINSTR Position */
#define SCB_CFSR_UNDEFINSTR_Msk           (1UL << SCB_CFSR_UNDEFINSTR_Pos)                /*!< SCB CFSR (UFSR): UNDEFINSTR Mask */

/** \brief SCB Hard Fault Status Register Definitions */
#define SCB_HFSR_DEBUGEVT_Pos              31U                                            /*!< SCB HFSR: DEBUGEVT Position */
#define SCB_HFSR_DEBUGEVT_Msk              (1UL << SCB_HFSR_DEBUGEVT_Pos)                 /*!< SCB HFSR: DEBUGEVT Mask */

#define SCB_HFSR_FORCED_Pos                30U                                            /*!< SCB HFSR: FORCED Position */
#define SCB_HFSR_FORCED_Msk                (1UL << SCB_HFSR_FORCED_Pos)                   /*!< SCB HFSR: FORCED Mask */

#define SCB_HFSR_VECTTBL_Pos                1U                                            /*!< SCB HFSR: VECTTBL Position */
#define SCB_HFSR_VECTTBL_Msk               (1UL << SCB_HFSR_VECTTBL_Pos)                  /*!< SCB HFSR: VECTTBL Mask */

/** \brief SCB Debug Fault Status Register Definitions */
#define SCB_DFSR_EXTERNAL_Pos               4U                                            /*!< SCB DFSR: EXTERNAL Position */
#define SCB_DFSR_EXTERNAL_Msk              (1UL << SCB_DFSR_EXTERNAL_Pos)                 /*!< SCB DFSR: EXTERNAL Mask */

#define SCB_DFSR_VCATCH_Pos                 3U                                            /*!< SCB DFSR: VCATCH Position */
#define SCB_DFSR_VCATCH_Msk                (1UL << SCB_DFSR_VCATCH_Pos)                   /*!< SCB DFSR: VCATCH Mask */

#define SCB_DFSR_DWTTRAP_Pos                2U                                            /*!< SCB DFSR: DWTTRAP Position */
#define SCB_DFSR_DWTTRAP_Msk               (1UL << SCB_DFSR_DWTTRAP_Pos)                  /*!< SCB DFSR: DWTTRAP Mask */

#define SCB_DFSR_BKPT_Pos                   1U                                            /*!< SCB DFSR: BKPT Position */
#define SCB_DFSR_BKPT_Msk                  (1UL << SCB_DFSR_BKPT_Pos)                     /*!< SCB DFSR: BKPT Mask */

#define SCB_DFSR_HALTED_Pos                 0U                                            /*!< SCB DFSR: HALTED Position */
#define SCB_DFSR_HALTED_Msk                (1UL /*<< SCB_DFSR_HALTED_Pos*/)               /*!< SCB DFSR: HALTED Mask */

/*@} end of group CMSIS_SCB */


/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_SCnSCB System Controls not in SCB (SCnSCB)
  \brief    Type definitions for the System Control and ID Register not in the SCB
  @{
 */

/**
  \brief  Structure type to access the System Control and ID Register not in the SCB.
 */
typedef struct
{
        uint32_t RESERVED0[1U];
  __IM  uint32_t ICTR;                   /*!< Offset: 0x004 (R/ )  Interrupt Controller Type Register */
  __IOM uint32_t ACTLR;                  /*!< Offset: 0x008 (R/W)  Auxiliary Control Register */
} SCnSCB_Type;

/** \brief SCnSCB Interrupt Controller Type Register Definitions */
#define SCnSCB_ICTR_INTLINESNUM_Pos         0U                                         /*!< ICTR: INTLINESNUM Position */
#define SCnSCB_ICTR_INTLINESNUM_Msk        (0xFUL /*<< SCnSCB_ICTR_INTLINESNUM_Pos*/)  /*!< ICTR: INTLINESNUM Mask */

/** \brief SCnSCB Auxiliary Control Register Definitions */
#define SCnSCB_ACTLR_DISFOLD_Pos            2U                                         /*!< ACTLR: DISFOLD Position */
#define SCnSCB_ACTLR_DISFOLD_Msk           (1UL << SCnSCB_ACTLR_DISFOLD_Pos)           /*!< ACTLR: DISFOLD Mask */

#define SCnSCB_ACTLR_DISDEFWBUF_Pos         1U                                         /*!< ACTLR: DISDEFWBUF Position */
#define SCnSCB_ACTLR_DISDEFWBUF_Msk        (1UL << SCnSCB_ACTLR_DISDEFWBUF_Pos)        /*!< ACTLR: DISDEFWBUF Mask */

#define SCnSCB_ACTLR_DISMCYCINT_Pos         0U                                         /*!< ACTLR: DISMCYCINT Position */
#define SCnSCB_ACTLR_DISMCYCINT_Msk        (1UL /*<< SCnSCB_ACTLR_DISMCYCINT_Pos*/)    /*!< ACTLR: DISMCYCINT Mask */

/*@} end of group CMSIS_SCnotSCB */


/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_SysTick     System Tick Timer (SysTick)
  \brief    Type definitions for the System Timer Registers.
  @{
 */

/**
  \brief  Structure type to access the System Timer (SysTick).
 */
typedef struct
{
  __IOM uint32_t CTRL;                   /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
  __IOM uint32_t LOAD;                   /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register */
  __IOM uint32_t VAL;                    /*!< Offset: 0x008 (R/W)  SysTick Current Value Register */
  __IM  uint32_t CALIB;                  /*!< Offset: 0x00C (R/ )  SysTick Calibration Register */
} SysTick_Type;

/** \brief SysTick Control / Status Register Definitions */
#define SysTick_CTRL_COUNTFLAG_Pos         16U                                            /*!< SysTick CTRL: COUNTFLAG Position */
#define SysTick_CTRL_COUNTFLAG_Msk         (1UL << SysTick_CTRL_COUNTFLAG_Pos)            /*!< SysTick CTRL: COUNTFLAG Mask */

#define SysTick_CTRL_CLKSOURCE_Pos          2U                                            /*!< SysTick CTRL: CLKSOURCE Position */
#define SysTick_CTRL_CLKSOURCE_Msk         (1UL << SysTick_CTRL_CLKSOURCE_Pos)            /*!< SysTick CTRL: CLKSOURCE Mask */

#define SysTick_CTRL_TICKINT_Pos            1U                                            /*!< SysTick CTRL: TICKINT Position */
#define SysTick_CTRL_TICKINT_Msk           (1UL << SysTick_CTRL_TICKINT_Pos)              /*!< SysTick CTRL: TICKINT Mask */

#define SysTick_CTRL_ENABLE_Pos             0U                                            /*!< SysTick CTRL: ENABLE Position */
#define SysTick_CTRL_ENABLE_Msk            (1UL /*<< SysTick_CTRL_ENABLE_Pos*/)           /*!< SysTick CTRL: ENABLE Mask */

/** \brief SysTick Reload Register Definitions */
#define SysTick_LOAD_RELOAD_Pos             0U                                            /*!< SysTick LOAD: RELOAD Position */
#define SysTick_LOAD_RELOAD_Msk            (0xFFFFFFUL /*<< SysTick_LOAD_RELOAD_Pos*/)    /*!< SysTick LOAD: RELOAD Mask */

/** \brief SysTick Current Register Definitions */
#define SysTick_VAL_CURRENT_Pos             0U                                            /*!< SysTick VAL: CURRENT Position */
#define SysTick_VAL_CURRENT_Msk            (0xFFFFFFUL /*<< SysTick_VAL_CURRENT_Pos*/)    /*!< SysTick VAL: CURRENT Mask */

/** \brief SysTick Calibration Register Definitions */
#define SysTick_CALIB_NOREF_Pos            31U                                            /*!< SysTick CALIB: NOREF Position */
#define SysTick_CALIB_NOREF_Msk            (1UL << SysTick_CALIB_NOREF_Pos)               /*!< SysTick CALIB: NOREF Mask */

#define SysTick_CALIB_SKEW_Pos             30U                                            /*!< SysTick CALIB: SKEW Position */
#define SysTick_CALIB_SKEW_Msk             (1UL << SysTick_CALIB_SKEW_Pos)                /*!< SysTick CALIB: SKEW Mask */

#define SysTick_CALIB_TENMS_Pos             0U                                            /*!< SysTick CALIB: TENMS Position */
#define SysTick_CALIB_TENMS_Msk            (0xFFFFFFUL /*<< SysTick_CALIB_TENMS_Pos*/)    /*!< SysTick CALIB: TENMS Mask */

/*@} end of group CMSIS_SysTick */


/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_ITM     Instrumentation Trace Macrocell (ITM)
  \brief    Type definitions for the Instrumentation Trace Macrocell (ITM)
  @{
 */

/**
  \brief  Structure type to access the Instrumentation Trace Macrocell Register (ITM).
 */
typedef struct
{
  __OM  union
  {
    __OM  uint8_t    u8;                 /*!< Offset: 0x000 ( /W)  Stimulus Port 8-bit */
    __OM  uint16_t   u16;                /*!< Offset: 0x000 ( /W)  Stimulus Port 16-bit */
    __OM  uint32_t   u32;                /*!< Offset: 0x000 ( /W)  Stimulus Port 32-bit */
  }  PORT [32U];                         /*!< Offset: 0x000 ( /W)  Stimulus Port Registers */
        uint32_t RESERVED0[864U];
  __IOM uint32_t TER;                    /*!< Offset: 0xE00 (R/W)  Trace Enable Register */
        uint32_t RESERVED1[15U];
  __IOM uint32_t TPR;                    /*!< Offset: 0xE40 (R/W)  Trace Privilege Register */
        uint32_t RESERVED2[15U];
  __IOM uint32_t TCR;                    /*!< Offset: 0xE80 (R/W)  Trace Control Register */
        uint32_t RESERVED3[32U];
        uint32_t RESERVED4[43U];
  __OM  uint32_t LAR;                    /*!< Offset: 0xFB0 ( /W)  Lock Access Register */
  __IM  uint32_t LSR;                    /*!< Offset: 0xFB4 (R/ )  Lock Status Register */
} ITM_Type;

/** \brief ITM Trace Privilege Register Definitions */
#define ITM_TPR_PRIVMASK_Pos                0U                                            /*!< ITM TPR: PRIVMASK Position */
#define ITM_TPR_PRIVMASK_Msk               (0xFUL /*<< ITM_TPR_PRIVMASK_Pos*/)            /*!< ITM TPR: PRIVMASK Mask */

/** \brief ITM Trace Control Register Definitions */
#define ITM_TCR_BUSY_Pos                   23U                                            /*!< ITM TCR: BUSY Position */
#define ITM_TCR_BUSY_Msk                   (1UL << ITM_TCR_BUSY_Pos)                      /*!< ITM TCR: BUSY Mask */

#define ITM_TCR_TRACEBUSID_Pos             16U                                            /*!< ITM TCR: ATBID Position */
#define ITM_TCR_TRACEBUSID_Msk             (0x7FUL << ITM_TCR_TRACEBUSID_Pos)             /*!< ITM TCR: ATBID Mask */

#define ITM_TCR_GTSFREQ_Pos                10U                                            /*!< ITM TCR: Global timestamp frequency Position */
#define ITM_TCR_GTSFREQ_Msk                (3UL << ITM_TCR_GTSFREQ_Pos)                   /*!< ITM TCR: Global timestamp frequency Mask */

#define ITM_TCR_TSPRESCALE_Pos              8U                                            /*!< ITM TCR: TSPrescale Position */
#define ITM_TCR_TSPRESCALE_Msk             (3UL << ITM_TCR_TSPRESCALE_Pos)                /*!< ITM TCR: TSPrescale Mask */

#define ITM_TCR_SWOENA_Pos                  4U                                            /*!< ITM TCR: SWOENA Position */
#define ITM_TCR_SWOENA_Msk                 (1UL << ITM_TCR_SWOENA_Pos)                    /*!< ITM TCR: SWOENA Mask */

#define ITM_TCR_DWTENA_Pos                  3U                                            /*!< ITM TCR: DWTENA Position */
#define ITM_TCR_DWTENA_Msk                 (1UL << ITM_TCR_DWTENA_Pos)                    /*!< ITM TCR: DWTENA Mask */

#define ITM_TCR_SYNCENA_Pos                 2U                                            /*!< ITM TCR: SYNCENA Position */
#define ITM_TCR_SYNCENA_Msk                (1UL << ITM_TCR_SYNCENA_Pos)                   /*!< ITM TCR: SYNCENA Mask */

#define ITM_TCR_TSENA_Pos                   1U                                            /*!< ITM TCR: TSENA Position */
#define ITM_TCR_TSENA_Msk                  (1UL << ITM_TCR_TSENA_Pos)                     /*!< ITM TCR: TSENA Mask */

#define ITM_TCR_ITMENA_Pos                  0U                                            /*!< ITM TCR: ITM Enable bit Position */
#define ITM_TCR_ITMENA_Msk                 (1UL /*<< ITM_TCR_ITMENA_Pos*/)                /*!< ITM TCR: ITM Enable bit Mask */

/** \brief ITM Lock Status Register Definitions */
#define ITM_LSR_BYTEACC_Pos                 2U                                            /*!< ITM LSR: ByteAcc Position */
#define ITM_LSR_BYTEACC_Msk                (1UL << ITM_LSR_BYTEACC_Pos)                   /*!< ITM LSR: ByteAcc Mask */

#define ITM_LSR_ACCESS_Pos                  1U                                            /*!< ITM LSR: Access Position */
#define ITM_LSR_ACCESS_Msk                 (1UL << ITM_LSR_ACCESS_Pos)                    /*!< ITM LSR: Access Mask */

#define ITM_LSR_PRESENT_Pos                 0U                                            /*!< ITM LSR: Present Position */
#define ITM_LSR_PRESENT_Msk                (1UL /*<< ITM_LSR_PRESENT_Pos*/)               /*!< ITM LSR: Present Mask */

/*@}*/ /* end of group CMSIS_ITM */


/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_DWT     Data Watchpoint and Trace (DWT)
  \brief    Type definitions for the Data Watchpoint and Trace (DWT)
  @{
 */

/**
  \brief  Structure type to access the Data Watchpoint and Trace Register (DWT).
 */
typedef struct
{
  __IOM uint32_t CTRL;                   /*!< Offset: 0x000 (R/W)  Control Register */
  __IOM uint32_t CYCCNT;                 /*!< Offset: 0x004 (R/W)  Cycle Count Register */
  __IOM uint32_t CPICNT;                 /*!< Offset: 0x008 (R/W)  CPI Count Register */
  __IOM uint32_t EXCCNT;                 /*!< Offset: 0x00C (R/W)  Exception Overhead Count Register */
  __IOM uint32_t SLEEPCNT;               /*!< Offset: 0x010 (R/W)  Sleep Count Register */
  __IOM uint32_t LSUCNT;                 /*!< Offset: 0x014 (R/W)  LSU Count Register */
  __IOM uint32_t FOLDCNT;                /*!< Offset: 0x018 (R/W)  Folded-instruction Count Register */
  __IM  uint32_t PCSR;                   /*!< Offset: 0x01C (R/ )  Program Counter Sample Register */
  __IOM uint32_t COMP0;                  /*!< Offset: 0x020 (R/W)  Comparator Register 0 */
  __IOM uint32_t MASK0;                  /*!< Offset: 0x024 (R/W)  Mask Register 0 */
  __IOM uint32_t FUNCTION0;              /*!< Offset: 0x028 (R/W)  Function Register 0 */
        uint32_t RESERVED0[1U];
  __IOM uint32_t COMP1;                  /*!< Offset: 0x030 (R/W)  Comparator Register 1 */
  __IOM uint32_t MASK1;                  /*!< Offset: 0x034 (R/W)  Mask Register 1 */
  __IOM uint32_t FUNCTION1;              /*!< Offset: 0x038 (R/W)  Function Register 1 */
        uint32_t RESERVED1[1U];
  __IOM uint32_t COMP2;                  /*!< Offset: 0x040 (R/W)  Comparator Register 2 */
  __IOM uint32_t MASK2;                  /*!< Offset: 0x044 (R/W)  Mask Register 2 */
  __IOM uint32_t FUNCTION2;              /*!< Offset: 0x048 (R/W)  Function Register 2 */
        uint32_t RESERVED2[1U];
  __IOM uint32_t COMP3;                  /*!< Offset: 0x050 (R/W)  Comparator Register 3 */
  __IOM uint32_t MASK3;                  /*!< Offset: 0x054 (R/W)  Mask Register 3 */
  __IOM uint32_t FUNCTION3;              /*!< Offset: 0x058 (R/W)  Function Register 3 */
} DWT_Type;

/** \brief DWT Control Register Definitions */
#define DWT_CTRL_NUMCOMP_Pos               28U                                         /*!< DWT CTRL: NUMCOMP Position */
#define DWT_CTRL_NUMCOMP_Msk               (0xFUL << DWT_CTRL_NUMCOMP_Pos)             /*!< DWT CTRL: NUMCOMP Mask */

#define DWT_CTRL_NOTRCPKT_Pos              27U                                         /*!< DWT CTRL: NOTRCPKT Position */
#define DWT_CTRL_NOTRCPKT_Msk              (1UL << DWT_CTRL_NOTRCPKT_Pos)              /*!< DWT CTRL: NOTRCPKT Mask */

#define DWT_CTRL_NOEXTTRIG_Pos             26U                                         /*!< DWT CTRL: NOEXTTRIG Position */
#define DWT_CTRL_NOEXTTRIG_Msk             (1UL << DWT_CTRL_NOEXTTRIG_Pos)             /*!< DWT CTRL: NOEXTTRIG Mask */

#define DWT_CTRL_NOCYCCNT_Pos              25U                                         /*!< DWT CTRL: NOCYCCNT Position */
#define DWT_CTRL_NOCYCCNT_Msk              (1UL << DWT_CTRL_NOCYCCNT_Pos)              /*!< DWT CTRL: NOCYCCNT Mask */

#define DWT_CTRL_NOPRFCNT_Pos              24U                                         /*!< DWT CTRL: NOPRFCNT Position */
#define DWT_CTRL_NOPRFCNT_Msk              (1UL << DWT_CTRL_NOPRFCNT_Pos)              /*!< DWT CTRL: NOPRFCNT Mask */

#define DWT_CTRL_CYCEVTENA_Pos             22U                                         /*!< DWT CTRL: CYCEVTENA Position */
#define DWT_CTRL_CYCEVTENA_Msk             (1UL << DWT_CTRL_CYCEVTENA_Pos)             /*!< DWT CTRL: CYCEVTENA Mask */

#define DWT_CTRL_FOLDEVTENA_Pos            21U                                         /*!< DWT CTRL: FOLDEVTENA Position */
#define DWT_CTRL_FOLDEVTENA_Msk            (1UL << DWT_CTRL_FOLDEVTENA_Pos)            /*!< DWT CTRL: FOLDEVTENA Mask */

#define DWT_CTRL_LSUEVTENA_Pos             20U                                         /*!< DWT CTRL: LSUEVTENA Position */
#define DWT_CTRL_LSUEVTENA_Msk             (1UL << DWT_CTRL_LSUEVTENA_Pos)             /*!< DWT CTRL: LSUEVTENA Mask */

#define DWT_CTRL_SLEEPEVTENA_Pos           19U                                         /*!< DWT CTRL: SLEEPEVTENA Position */
#define DWT_CTRL_SLEEPEVTENA_Msk           (1UL << DWT_CTRL_SLEEPEVTENA_Pos)           /*!< DWT CTRL: SLEEPEVTENA Mask */

#define DWT_CTRL_EXCEVTENA_Pos             18U                                         /*!< DWT CTRL: EXCEVTENA Position */
#define DWT_CTRL_EXCEVTENA_Msk             (1UL << DWT_CTRL_EXCEVTENA_Pos)             /*!< DWT CTRL: EXCEVTENA Mask */

#define DWT_CTRL_CPIEVTENA_Pos             17U                                         /*!< DWT CTRL: CPIEVTENA Position */
#define DWT_CTRL_CPIEVTENA_Msk             (1UL << DWT_CTRL_CPIEVTENA_Pos)             /*!< DWT CTRL: CPIEVTENA Mask */

#define DWT_CTRL_EXCTRCENA_Pos             16U                                         /*!< DWT CTRL: EXCTRCENA Position */
#define DWT_CTRL_EXCTRCENA_Msk             (1UL << DWT_CTRL_EXCTRCENA_Pos)             /*!< DWT CTRL: EXCTRCENA Mask */

#define DWT_CTRL_PCSAMPLENA_Pos            12U                                         /*!< DWT CTRL: PCSAMPLENA Position */
#define DWT_CTRL_PCSAMPLENA_Msk            (1UL << DWT_CTRL_PCSAMPLENA_Pos)            /*!< DWT CTRL: PCSAMPLENA Mask */

#define DWT_CTRL_SYNCTAP_Pos               10U                                         /*!< DWT CTRL: SYNCTAP Position */
#define DWT_CTRL_SYNCTAP_Msk               (0x3UL << DWT_CTRL_SYNCTAP_Pos)             /*!< DWT CTRL: SYNCTAP Mask */

#define DWT_CTRL_CYCTAP_Pos                 9U                                         /*!< DWT CTRL: CYCTAP Position */
#define DWT_CTRL_CYCTAP_Msk                (1UL << DWT_CTRL_CYCTAP_Pos)                /*!< DWT CTRL: CYCTAP Mask */

#define DWT_CTRL_POSTINIT_Pos               5U                                         /*!< DWT CTRL: POSTINIT Position */
#define DWT_CTRL_POSTINIT_Msk              (0xFUL << DWT_CTRL_POSTINIT_Pos)            /*!< DWT CTRL: POSTINIT Mask */

#define DWT_CTRL_POSTPRESET_Pos             1U                                         /*!< DWT CTRL: POSTPRESET Position */
#define DWT_CTRL_POSTPRESET_Msk            (0xFUL << DWT_CTRL_POSTPRESET_Pos)          /*!< DWT CTRL: POSTPRESET Mask */

#define DWT_CTRL_CYCCNTENA_Pos              0U                                         /*!< DWT CTRL: CYCCNTENA Position */
#define DWT_CTRL_CYCCNTENA_Msk             (1UL /*<< DWT_CTRL_CYCCNTENA_Pos*/)         /*!< DWT CTRL: CYCCNTENA Mask */

/** \brief DWT CPI Count Register Definitions */
#define DWT_CPICNT_CPICNT_Pos               0U                                         /*!< DWT CPICNT: CPICNT Position */
#define DWT_CPICNT_CPICNT_Msk              (0xFFUL /*<< DWT_CPICNT_CPICNT_Pos*/)       /*!< DWT CPICNT: CPICNT Mask */

/** \brief DWT Exception Overhead Count Register Definitions */
#define DWT_EXCCNT_EXCCNT_Pos               0U                                         /*!< DWT EXCCNT: EXCCNT Position */
#define DWT_EXCCNT_EXCCNT_Msk              (0xFFUL /*<< DWT_EXCCNT_EXCCNT_Pos*/)       /*!< DWT EXCCNT: EXCCNT Mask */

/** \brief DWT Sleep Count Register Definitions */
#define DWT_SLEEPCNT_SLEEPCNT_Pos           0U                                         /*!< DWT SLEEPCNT: SLEEPCNT Position */
#define DWT_SLEEPCNT_SLEEPCNT_Msk          (0xFFUL /*<< DWT_SLEEPCNT_SLEEPCNT_Pos*/)   /*!< DWT SLEEPCNT: SLEEPCNT Mask */

/** \brief DWT LSU Count Register Definitions */
#define DWT_LSUCNT_LSUCNT_Pos               0U                                         /*!< DWT LSUCNT: LSUCNT Position */
#define DWT_LSUCNT_LSUCNT_Msk              (0xFFUL /*<< DWT_LSUCNT_LSUCNT_Pos*/)       /*!< DWT LSUCNT: LSUCNT Mask */

/** \brief DWT Folded-instruction Count Register Definitions */
#define DWT_FOLDCNT_FOLDCNT_Pos             0U                                         /*!< DWT FOLDCNT: FOLDCNT Position */
#define DWT_FOLDCNT_FOLDCNT_Msk            (0xFFUL /*<< DWT_FOLDCNT_FOLDCNT_Pos*/)     /*!< DWT FOLDCNT: FOLDCNT Mask */

/** \brief DWT Comparator Mask Register Definitions */
#define DWT_MASK_MASK_Pos                   0U                                         /*!< DWT MASK: MASK Position */
#define DWT_MASK_MASK_Msk                  (0x1FUL /*<< DWT_MASK_MASK_Pos*/)           /*!< DWT MASK: MASK Mask */

/** \brief DWT Comparator Function Register Definitions */
#define DWT_FUNCTION_MATCHED_Pos           24U                                         /*!< DWT FUNCTION: MATCHED Position */
#define DWT_FUNCTION_MATCHED_Msk           (1UL << DWT_FUNCTION_MATCHED_Pos)           /*!< DWT FUNCTION: MATCHED Mask */

#define DWT_FUNCTION_DATAVADDR1_Pos        16U                                         /*!< DWT FUNCTION: DATAVADDR1 Position */
#define DWT_FUNCTION_DATAVADDR1_Msk        (0xFUL << DWT_FUNCTION_DATAVADDR1_Pos)      /*!< DWT FUNCTION: DATAVADDR1 Mask */

#define DWT_FUNCTION_DATAVADDR0_Pos        12U                                         /*!< DWT FUNCTION: DATAVADDR0 Position */
#define DWT_FUNCTION_DATAVADDR0_Msk        (0xFUL << DWT_FUNCTION_DATAVADDR0_Pos)      /*!< DWT FUNCTION: DATAVADDR0 Mask */

#define DWT_FUNCTION_DATAVSIZE_Pos         10U                                         /*!< DWT FUNCTION: DATAVSIZE Position */
#define DWT_FUNCTION_DATAVSIZE_Msk         (0x3UL << DWT_FUNCTION_DATAVSIZE_Pos)       /*!< DWT FUNCTION: DATAVSIZE Mask */

#define DWT_FUNCTION_LNK1ENA_Pos            9U                                         /*!< DWT FUNCTION: LNK1ENA Position */
#define DWT_FUNCTION_LNK1ENA_Msk           (1UL << DWT_FUNCTION_LNK1ENA_Pos)           /*!< DWT FUNCTION: LNK1ENA Mask */

#define DWT_FUNCTION_DATAVMATCH_Pos         8U                                         /*!< DWT FUNCTION: DATAVMATCH Position */
#define DWT_FUNCTION_DATAVMATCH_Msk        (1UL << DWT_FUNCTION_DATAVMATCH_Pos)        /*!< DWT FUNCTION: DATAVMATCH Mask */

#define DWT_FUNCTION_CYCMATCH_Pos           7U                                         /*!< DWT FUNCTION: CYCMATCH Position */
#define DWT_FUNCTION_CYCMATCH_Msk          (1UL << DWT_FUNCTION_CYCMATCH_Pos)          /*!< DWT FUNCTION: CYCMATCH Mask */

#define DWT_FUNCTION_EMITRANGE_Pos          5U                                         /*!< DWT FUNCTION: EMITRANGE Position */
#define DWT_FUNCTION_EMITRANGE_Msk         (1UL << DWT_FUNCTION_EMITRANGE_Pos)         /*!< DWT FUNCTION: EMITRANGE Mask */

#define DWT_FUNCTION_FUNCTION_Pos           0U                                         /*!< DWT FUNCTION: FUNCTION Position */
#define DWT_FUNCTION_FUNCTION_Msk          (0xFUL /*<< DWT_FUNCTION_FUNCTION_Pos*/)    /*!< DWT FUNCTION: FUNCTION Mask */

/*@}*/ /* end of group CMSIS_DWT */


/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_TPIU    Trace Port Interface Unit (TPIU)
  \brief    Type definitions for the Trace Port Interface Unit (TPIU)
  @{
 */

/**
  \brief  Structure type to access the Trace Port Interface Unit Register (TPIU).
 */
typedef struct
{
  __IM  uint32_t SSPSR;                  /*!< Offset: 0x000 (R/ )  Supported Parallel Port Size Register */
  __IOM uint32_t CSPSR;                  /*!< Offset: 0x004 (R/W)  Current Parallel Port Size Register */
        uint32_t RESERVED0[2U];
  __IOM uint32_t ACPR;                   /*!< Offset: 0x010 (R/W)  Asynchronous Clock Prescaler Register */
        uint32_t RESERVED1[55U];
  __IOM uint32_t SPPR;                   /*!< Offset: 0x0F0 (R/W)  Selected Pin Protocol Register */
        uint32_t RESERVED2[131U];
  __IM  uint32_t FFSR;                   /*!< Offset: 0x300 (R/ )  Formatter and Flush Status Register */
  __IOM uint32_t FFCR;                   /*!< Offset: 0x304 (R/W)  Formatter and Flush Control Register */
  __IM  uint32_t FSCR;                   /*!< Offset: 0x308 (R/ )  Formatter Synchronization Counter Register */
        uint32_t RESERVED3[759U];
  __IM  uint32_t TRIGGER;                /*!< Offset: 0xEE8 (R/ )  TRIGGER Register */
  __IM  uint32_t FIFO0;                  /*!< Offset: 0xEEC (R/ )  Integration ETM Data */
  __IM  uint32_t ITATBCTR2;              /*!< Offset: 0xEF0 (R/ )  ITATBCTR2 */
        uint32_t RESERVED4[1U];
  __IM  uint32_t ITATBCTR0;              /*!< Offset: 0xEF8 (R/ )  ITATBCTR0 */
  __IM  uint32_t FIFO1;                  /*!< Offset: 0xEFC (R/ )  Integration ITM Data */
  __IOM uint32_t ITCTRL;                 /*!< Offset: 0xF00 (R/W)  Integration Mode Control */
        uint32_t RESERVED5[39U];
  __IOM uint32_t CLAIMSET;               /*!< Offset: 0xFA0 (R/W)  Claim tag set */
  __IOM uint32_t CLAIMCLR;               /*!< Offset: 0xFA4 (R/W)  Claim tag clear */
        uint32_t RESERVED7[8U];
  __IM  uint32_t DEVID;                  /*!< Offset: 0xFC8 (R/ )  Device Configuration Register */
  __IM  uint32_t DEVTYPE;                /*!< Offset: 0xFCC (R/ )  Device Type Identifier Register */
} TPIU_Type;

/** \brief TPIU Asynchronous Clock Prescaler Register Definitions */
#define TPIU_ACPR_PRESCALER_Pos             0U                                         /*!< TPIU ACPR: PRESCALER Position */
#define TPIU_ACPR_PRESCALER_Msk            (0x1FFFUL /*<< TPIU_ACPR_PRESCALER_Pos*/)   /*!< TPIU ACPR: PRESCALER Mask */

/** \brief TPIU Selected Pin Protocol Register Definitions */
#define TPIU_SPPR_TXMODE_Pos                0U                                         /*!< TPIU SPPR: TXMODE Position */
#define TPIU_SPPR_TXMODE_Msk               (0x3UL /*<< TPIU_SPPR_TXMODE_Pos*/)         /*!< TPIU SPPR: TXMODE Mask */

/** \brief TPIU Formatter and Flush Status Register Definitions */
#define TPIU_FFSR_FtNonStop_Pos             3U                                         /*!< TPIU FFSR: FtNonStop Position */
#define TPIU_FFSR_FtNonStop_Msk            (1UL << TPIU_FFSR_FtNonStop_Pos)            /*!< TPIU FFSR: FtNonStop Mask */

#define TPIU_FFSR_TCPresent_Pos             2U                                         /*!< TPIU FFSR: TCPresent Position */
#define TPIU_FFSR_TCPresent_Msk            (1UL << TPIU_FFSR_TCPresent_Pos)            /*!< TPIU FFSR: TCPresent Mask */

#define TPIU_FFSR_FtStopped_Pos             1U                                         /*!< TPIU FFSR: FtStopped Position */
#define TPIU_FFSR_FtStopped_Msk            (1UL << TPIU_FFSR_FtStopped_Pos)            /*!< TPIU FFSR: FtStopped Mask */

#define TPIU_FFSR_FlInProg_Pos              0U                                         /*!< TPIU FFSR: FlInProg Position */
#define TPIU_FFSR_FlInProg_Msk             (1UL /*<< TPIU_FFSR_FlInProg_Pos*/)         /*!< TPIU FFSR: FlInProg Mask */

/** \brief TPIU Formatter and Flush Control Register Definitions */
#define TPIU_FFCR_TrigIn_Pos                8U                                         /*!< TPIU FFCR: TrigIn Position */
#define TPIU_FFCR_TrigIn_Msk               (1UL << TPIU_FFCR_TrigIn_Pos)               /*!< TPIU FFCR: TrigIn Mask */

#define TPIU_FFCR_EnFCont_Pos               1U                                         /*!< TPIU FFCR: EnFCont Position */
#define TPIU_FFCR_EnFCont_Msk              (1UL << TPIU_FFCR_EnFCont_Pos)              /*!< TPIU FFCR: EnFCont Mask */

/** \brief TPIU TRIGGER Register Definitions */
#define TPIU_TRIGGER_TRIGGER_Pos            0U                                         /*!< TPIU TRIGGER: TRIGGER Position */
#define TPIU_TRIGGER_TRIGGER_Msk           (1UL /*<< TPIU_TRIGGER_TRIGGER_Pos*/)       /*!< TPIU TRIGGER: TRIGGER Mask */

/** \brief TPIU Integration ETM Data Register Definitions (FIFO0) */
#define TPIU_FIFO0_ITM_ATVALID_Pos         29U                                         /*!< TPIU FIFO0: ITM_ATVALID Position */
#define TPIU_FIFO0_ITM_ATVALID_Msk         (1UL << TPIU_FIFO0_ITM_ATVALID_Pos)         /*!< TPIU FIFO0: ITM_ATVALID Mask */

#define TPIU_FIFO0_ITM_bytecount_Pos       27U                                         /*!< TPIU FIFO0: ITM_bytecount Position */
#define TPIU_FIFO0_ITM_bytecount_Msk       (0x3UL << TPIU_FIFO0_ITM_bytecount_Pos)     /*!< TPIU FIFO0: ITM_bytecount Mask */

#define TPIU_FIFO0_ETM_ATVALID_Pos         26U                                         /*!< TPIU FIFO0: ETM_ATVALID Position */
#define TPIU_FIFO0_ETM_ATVALID_Msk         (1UL << TPIU_FIFO0_ETM_ATVALID_Pos)         /*!< TPIU FIFO0: ETM_ATVALID Mask */

#define TPIU_FIFO0_ETM_bytecount_Pos       24U                                         /*!< TPIU FIFO0: ETM_bytecount Position */
#define TPIU_FIFO0_ETM_bytecount_Msk       (0x3UL << TPIU_FIFO0_ETM_bytecount_Pos)     /*!< TPIU FIFO0: ETM_bytecount Mask */

#define TPIU_FIFO0_ETM2_Pos                16U                                         /*!< TPIU FIFO0: ETM2 Position */
#define TPIU_FIFO0_ETM2_Msk                (0xFFUL << TPIU_FIFO0_ETM2_Pos)             /*!< TPIU FIFO0: ETM2 Mask */

#define TPIU_FIFO0_ETM1_Pos                 8U                                         /*!< TPIU FIFO0: ETM1 Position */
#define TPIU_FIFO0_ETM1_Msk                (0xFFUL << TPIU_FIFO0_ETM1_Pos)             /*!< TPIU FIFO0: ETM1 Mask */

#define TPIU_FIFO0_ETM0_Pos                 0U                                         /*!< TPIU FIFO0: ETM0 Position */
#define TPIU_FIFO0_ETM0_Msk                (0xFFUL /*<< TPIU_FIFO0_ETM0_Pos*/)         /*!< TPIU FIFO0: ETM0 Mask */

/** \brief TPIU ITATBCTR2 Register Definitions */
#define TPIU_ITATBCTR2_ATREADY2_Pos         0U                                         /*!< TPIU ITATBCTR2: ATREADY2 Position */
#define TPIU_ITATBCTR2_ATREADY2_Msk        (1UL /*<< TPIU_ITATBCTR2_ATREADY2_Pos*/)    /*!< TPIU ITATBCTR2: ATREADY2 Mask */

#define TPIU_ITATBCTR2_ATREADY1_Pos         0U                                         /*!< TPIU ITATBCTR2: ATREADY1 Position */
#define TPIU_ITATBCTR2_ATREADY1_Msk        (1UL /*<< TPIU_ITATBCTR2_ATREADY1_Pos*/)    /*!< TPIU ITATBCTR2: ATREADY1 Mask */

/** \brief TPIU Integration ITM Data Register Definitions (FIFO1) */
#define TPIU_FIFO1_ITM_ATVALID_Pos         29U                                         /*!< TPIU FIFO1: ITM_ATVALID Position */
#define TPIU_FIFO1_ITM_ATVALID_Msk         (1UL << TPIU_FIFO1_ITM_ATVALID_Pos)         /*!< TPIU FIFO1: ITM_ATVALID Mask */

#define TPIU_FIFO1_ITM_bytecount_Pos       27U                                         /*!< TPIU FIFO1: ITM_bytecount Position */
#define TPIU_FIFO1_ITM_bytecount_Msk       (0x3UL << TPIU_FIFO1_ITM_bytecount_Pos)     /*!< TPIU FIFO1: ITM_bytecount Mask */

#define TPIU_FIFO1_ETM_ATVALID_Pos         26U                                         /*!< TPIU FIFO1: ETM_ATVALID Position */
#define TPIU_FIFO1_ETM_ATVALID_Msk         (1UL << TPIU_FIFO1_ETM_ATVALID_Pos)         /*!< TPIU FIFO1: ETM_ATVALID Mask */

#define TPIU_FIFO1_ETM_bytecount_Pos       24U                                         /*!< TPIU FIFO1: ETM_bytecount Position */
#define TPIU_FIFO1_ETM_bytecount_Msk       (0x3UL << TPIU_FIFO1_ETM_bytecount_Pos)     /*!< TPIU FIFO1: ETM_bytecount Mask */

#define TPIU_FIFO1_ITM2_Pos                16U                                         /*!< TPIU FIFO1: ITM2 Position */
#define TPIU_FIFO1_ITM2_Msk                (0xFFUL << TPIU_FIFO1_ITM2_Pos)             /*!< TPIU FIFO1: ITM2 Mask */

#define TPIU_FIFO1_ITM1_Pos                 8U                                         /*!< TPIU FIFO1: ITM1 Position */
#define TPIU_FIFO1_ITM1_Msk                (0xFFUL << TPIU_FIFO1_ITM1_Pos)             /*!< TPIU FIFO1: ITM1 Mask */

#define TPIU_FIFO1_ITM0_Pos                 0U                                         /*!< TPIU FIFO1: ITM0 Position */
#define TPIU_FIFO1_ITM0_Msk                (0xFFUL /*<< TPIU_FIFO1_ITM0_Pos*/)         /*!< TPIU FIFO1: ITM0 Mask */

/** \brief TPIU ITATBCTR0 Register Definitions */
#define TPIU_ITATBCTR0_ATREADY2_Pos         0U                                         /*!< TPIU ITATBCTR0: ATREADY2 Position */
#define TPIU_ITATBCTR0_ATREADY2_Msk        (1UL /*<< TPIU_ITATBCTR0_ATREADY2_Pos*/)    /*!< TPIU ITATBCTR0: ATREADY2 Mask */

#define TPIU_ITATBCTR0_ATREADY1_Pos         0U                                         /*!< TPIU ITATBCTR0: ATREADY1 Position */
#define TPIU_ITATBCTR0_ATREADY1_Msk        (1UL /*<< TPIU_ITATBCTR0_ATREADY1_Pos*/)    /*!< TPIU ITATBCTR0: ATREADY1 Mask */

/** \brief TPIU Integration Mode Control Register Definitions */
#define TPIU_ITCTRL_Mode_Pos                0U                                         /*!< TPIU ITCTRL: Mode Position */
#define TPIU_ITCTRL_Mode_Msk               (0x3UL /*<< TPIU_ITCTRL_Mode_Pos*/)         /*!< TPIU ITCTRL: Mode Mask */

/** \brief TPIU DEVID Register Definitions */
#define TPIU_DEVID_NRZVALID_Pos            11U                                         /*!< TPIU DEVID: NRZVALID Position */
#define TPIU_DEVID_NRZVALID_Msk            (1UL << TPIU_DEVID_NRZVALID_Pos)            /*!< TPIU DEVID: NRZVALID Mask */

#define TPIU_DEVID_MANCVALID_Pos           10U                                         /*!< TPIU DEVID: MANCVALID Position */
#define TPIU_DEVID_MANCVALID_Msk           (1UL << TPIU_DEVID_MANCVALID_Pos)           /*!< TPIU DEVID: MANCVALID Mask */

#define TPIU_DEVID_PTINVALID_Pos            9U                                         /*!< TPIU DEVID: PTINVALID Position */
#define TPIU_DEVID_PTINVALID_Msk           (1UL << TPIU_DEVID_PTINVALID_Pos)           /*!< TPIU DEVID: PTINVALID Mask */

#define TPIU_DEVID_MinBufSz_Pos             6U                                         /*!< TPIU DEVID: MinBufSz Position */
#define TPIU_DEVID_MinBufSz_Msk            (0x7UL << TPIU_DEVID_MinBufSz_Pos)          /*!< TPIU DEVID: MinBufSz Mask */

#define TPIU_DEVID_AsynClkIn_Pos            5U                                         /*!< TPIU DEVID: AsynClkIn Position */
#define TPIU_DEVID_AsynClkIn_Msk           (1UL << TPIU_DEVID_AsynClkIn_Pos)           /*!< TPIU DEVID: AsynClkIn Mask */

#define TPIU_DEVID_NrTraceInput_Pos         0U                                         /*!< TPIU DEVID: NrTraceInput Position */
#define TPIU_DEVID_NrTraceInput_Msk        (0x3FUL /*<< TPIU_DEVID_NrTraceInput_Pos*/) /*!< TPIU DEVID: NrTraceInput Mask */

/** \brief TPIU DEVTYPE Register Definitions */
#define TPIU_DEVTYPE_SubType_Pos            4U                                         /*!< TPIU DEVTYPE: SubType Position */
#define TPIU_DEVTYPE_SubType_Msk           (0xFUL /*<< TPIU_DEVTYPE_SubType_Pos*/)     /*!< TPIU DEVTYPE: SubType Mask */

#define TPIU_DEVTYPE_MajorType_Pos          0U                                         /*!< TPIU DEVTYPE: MajorType Position */
#define TPIU_DEVTYPE_MajorType_Msk         (0xFUL << TPIU_DEVTYPE_MajorType_Pos)       /*!< TPIU DEVTYPE: MajorType Mask */

/*@}*/ /* end of group CMSIS_TPIU */


#if defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U)
/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_MPU     Memory Protection Unit (MPU)
  \brief    Type definitions for the Memory Protection Unit (MPU)
  @{
 */

/**
  \brief  Structure type to access the Memory Protection Unit (MPU).
 */
typedef struct
{
  __IM  uint32_t TYPE;                   /*!< Offset: 0x000 (R/ )  MPU Type Register */
  __IOM uint32_t CTRL;                   /*!< Offset: 0x004 (R/W)  MPU Control Register */
  __IOM uint32_t RNR;                    /*!< Offset: 0x008 (R/W)  MPU Region Number Register */
  __IOM uint32_t RBAR;                   /*!< Offset: 0x00C (R/W)  MPU Region Base Address Register */
  __IOM uint32_t RASR;                   /*!< Offset: 0x010 (R/W)  MPU Region Attribute and Size Register */
  __IOM uint32_t RBAR_A1;                /*!< Offset: 0x014 (R/W)  MPU Alias 1 Region Base Address Register */
  __IOM uint32_t RASR_A1;                /*!< Offset: 0x018 (R/W)  MPU Alias 1 Region Attribute and Size Register */
  __IOM uint32_t RBAR_A2;                /*!< Offset: 0x01C (R/W)  MPU Alias 2 Region Base Address Register */
  __IOM uint32_t RASR_A2;                /*!< Offset: 0x020 (R/W)  MPU Alias 2 Region Attribute and Size Register */
  __IOM uint32_t RBAR_A3;                /*!< Offset: 0x024 (R/W)  MPU Alias 3 Region Base Address Register */
  __IOM uint32_t RASR_A3;                /*!< Offset: 0x028 (R/W)  MPU Alias 3 Region Attribute and Size Register */
} MPU_Type;

#define MPU_TYPE_RALIASES                  4U

/** \brief MPU Type Register Definitions */
#define MPU_TYPE_IREGION_Pos               16U                                            /*!< MPU TYPE: IREGION Position */
#define MPU_TYPE_IREGION_Msk               (0xFFUL << MPU_TYPE_IREGION_Pos)               /*!< MPU TYPE: IREGION Mask */

#define MPU_TYPE_DREGION_Pos                8U                                            /*!< MPU TYPE: DREGION Position */
#define MPU_TYPE_DREGION_Msk               (0xFFUL << MPU_TYPE_DREGION_Pos)               /*!< MPU TYPE: DREGION Mask */

#define MPU_TYPE_SEPARATE_Pos               0U                                            /*!< MPU TYPE: SEPARATE Position */
#define MPU_TYPE_SEPARATE_Msk              (1UL /*<< MPU_TYPE_SEPARATE_Pos*/)             /*!< MPU TYPE: SEPARATE Mask */

/** \brief MPU Control Register Definitions */
#define MPU_CTRL_PRIVDEFENA_Pos             2U                                            /*!< MPU CTRL: PRIVDEFENA Position */
#define MPU_CTRL_PRIVDEFENA_Msk            (1UL << MPU_CTRL_PRIVDEFENA_Pos)               /*!< MPU CTRL: PRIVDEFENA Mask */

#define MPU_CTRL_HFNMIENA_Pos               1U                                            /*!< MPU CTRL: HFNMIENA Position */
#define MPU_CTRL_HFNMIENA_Msk              (1UL << MPU_CTRL_HFNMIENA_Pos)                 /*!< MPU CTRL: HFNMIENA Mask */

#define MPU_CTRL_ENABLE_Pos                 0U                                            /*!< MPU CTRL: ENABLE Position */
#define MPU_CTRL_ENABLE_Msk                (1UL /*<< MPU_CTRL_ENABLE_Pos*/)               /*!< MPU CTRL: ENABLE Mask */

/** \brief MPU Region Number Register Definitions */
#define MPU_RNR_REGION_Pos                  0U                                            /*!< MPU RNR: REGION Position */
#define MPU_RNR_REGION_Msk                 (0xFFUL /*<< MPU_RNR_REGION_Pos*/)             /*!< MPU RNR: REGION Mask */

/** \brief MPU Region Base Address Register Definitions */
#define MPU_RBAR_ADDR_Pos                   5U                                            /*!< MPU RBAR: ADDR Position */
#define MPU_RBAR_ADDR_Msk                  (0x7FFFFFFUL << MPU_RBAR_ADDR_Pos)             /*!< MPU RBAR: ADDR Mask */

#define MPU_RBAR_VALID_Pos                  4U                                            /*!< MPU RBAR: VALID Position */
#define MPU_RBAR_VALID_Msk                 (1UL << MPU_RBAR_VALID_Pos)                    /*!< MPU RBAR: VALID Mask */

#define MPU_RBAR_REGION_Pos                 0U                                            /*!< MPU RBAR: REGION Position */
#define MPU_RBAR_REGION_Msk                (0xFUL /*<< MPU_RBAR_REGION_Pos*/)             /*!< MPU RBAR: REGION Mask */

/** \brief MPU Region Attribute and Size Register Definitions */
#define MPU_RASR_ATTRS_Pos                 16U                                            /*!< MPU RASR: MPU Region Attribute field Position */
#define MPU_RASR_ATTRS_Msk                 (0xFFFFUL << MPU_RASR_ATTRS_Pos)               /*!< MPU RASR: MPU Region Attribute field Mask */

#define MPU_RASR_XN_Pos                    28U                                            /*!< MPU RASR: ATTRS.XN Position */
#define MPU_RASR_XN_Msk                    (1UL << MPU_RASR_XN_Pos)                       /*!< MPU RASR: ATTRS.XN Mask */

#define MPU_RASR_AP_Pos                    24U                                            /*!< MPU RASR: ATTRS.AP Position */
#define MPU_RASR_AP_Msk                    (0x7UL << MPU_RASR_AP_Pos)                     /*!< MPU RASR: ATTRS.AP Mask */

#define MPU_RASR_TEX_Pos                   19U                                            /*!< MPU RASR: ATTRS.TEX Position */
#define MPU_RASR_TEX_Msk                   (0x7UL << MPU_RASR_TEX_Pos)                    /*!< MPU RASR: ATTRS.TEX Mask */

#define MPU_RASR_S_Pos                     18U                                            /*!< MPU RASR: ATTRS.S Position */
#define MPU_RASR_S_Msk                     (1UL << MPU_RASR_S_Pos)                        /*!< MPU RASR: ATTRS.S Mask */

#define MPU_RASR_C_Pos                     17U                                            /*!< MPU RASR: ATTRS.C Position */
#define MPU_RASR_C_Msk                     (1UL << MPU_RASR_C_Pos)                        /*!< MPU RASR: ATTRS.C Mask */

#define MPU_RASR_B_Pos                     16U                                            /*!< MPU RASR: ATTRS.B Position */
#define MPU_RASR_B_Msk                     (1UL << MPU_RASR_B_Pos)                        /*!< MPU RASR: ATTRS.B Mask */

#define MPU_RASR_SRD_Pos                    8U                                            /*!< MPU RASR: Sub-Region Disable Position */
#define MPU_RASR_SRD_Msk                   (0xFFUL << MPU_RASR_SRD_Pos)                   /*!< MPU RASR: Sub-Region Disable Mask */

#define MPU_RASR_SIZE_Pos                   1U                                            /*!< MPU RASR: Region Size Field Position */
#define MPU_RASR_SIZE_Msk                  (0x1FUL << MPU_RASR_SIZE_Pos)                  /*!< MPU RASR: Region Size Field Mask */

#define MPU_RASR_ENABLE_Pos                 0U                                            /*!< MPU RASR: Region enable bit Position */
#define MPU_RASR_ENABLE_Msk                (1UL /*<< MPU_RASR_ENABLE_Pos*/)               /*!< MPU RASR: Region enable bit Disable Mask */

/*@} end of group CMSIS_MPU */
#endif


/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_DCB       Debug Control Block
  \brief    Type definitions for the Debug Control Block Registers
  @{
 */

/**
  \brief  Structure type to access the Debug Control Block Registers (DCB).
 */
typedef struct
{
  __IOM uint32_t DHCSR;                  /*!< Offset: 0x000 (R/W)  Debug Halting Control and Status Register */
  __OM  uint32_t DCRSR;                  /*!< Offset: 0x004 ( /W)  Debug Core Register Selector Register */
  __IOM uint32_t DCRDR;                  /*!< Offset: 0x008 (R/W)  Debug Core Register Data Register */
  __IOM uint32_t DEMCR;                  /*!< Offset: 0x00C (R/W)  Debug Exception and Monitor Control Register */
} DCB_Type;

/** \brief DCB Debug Halting Control and Status Register Definitions */
#define DCB_DHCSR_DBGKEY_Pos               16U                                            /*!< DCB DHCSR: Debug key Position */
#define DCB_DHCSR_DBGKEY_Msk               (0xFFFFUL << DCB_DHCSR_DBGKEY_Pos)             /*!< DCB DHCSR: Debug key Mask */

#define DCB_DHCSR_S_RESET_ST_Pos           25U                                            /*!< DCB DHCSR: Reset sticky status Position */
#define DCB_DHCSR_S_RESET_ST_Msk           (1UL << DCB_DHCSR_S_RESET_ST_Pos)              /*!< DCB DHCSR: Reset sticky status Mask */

#define DCB_DHCSR_S_RETIRE_ST_Pos          24U                                            /*!< DCB DHCSR: Retire sticky status Position */
#define DCB_DHCSR_S_RETIRE_ST_Msk          (1UL << DCB_DHCSR_S_RETIRE_ST_Pos)             /*!< DCB DHCSR: Retire sticky status Mask */

#define DCB_DHCSR_S_LOCKUP_Pos             19U                                            /*!< DCB DHCSR: Lockup status Position */
#define DCB_DHCSR_S_LOCKUP_Msk             (1UL << DCB_DHCSR_S_LOCKUP_Pos)                /*!< DCB DHCSR: Lockup status Mask */

#define DCB_DHCSR_S_SLEEP_Pos              18U                                            /*!< DCB DHCSR: Sleeping status Position */
#define DCB_DHCSR_S_SLEEP_Msk              (1UL << DCB_DHCSR_S_SLEEP_Pos)                 /*!< DCB DHCSR: Sleeping status Mask */

#define DCB_DHCSR_S_HALT_Pos               17U                                            /*!< DCB DHCSR: Halted status Position */
#define DCB_DHCSR_S_HALT_Msk               (1UL << DCB_DHCSR_S_HALT_Pos)                  /*!< DCB DHCSR: Halted status Mask */

#define DCB_DHCSR_S_REGRDY_Pos             16U                                            /*!< DCB DHCSR: Register ready status Position */
#define DCB_DHCSR_S_REGRDY_Msk             (1UL << DCB_DHCSR_S_REGRDY_Pos)                /*!< DCB DHCSR: Register ready status Mask */

#define DCB_DHCSR_C_SNAPSTALL_Pos           5U                                            /*!< DCB DHCSR: Snap stall control Position */
#define DCB_DHCSR_C_SNAPSTALL_Msk          (1UL << DCB_DHCSR_C_SNAPSTALL_Pos)             /*!< DCB DHCSR: Snap stall control Mask */

#define DCB_DHCSR_C_MASKINTS_Pos            3U                                            /*!< DCB DHCSR: Mask interrupts control Position */
#define DCB_DHCSR_C_MASKINTS_Msk           (1UL << DCB_DHCSR_C_MASKINTS_Pos)              /*!< DCB DHCSR: Mask interrupts control Mask */

#define DCB_DHCSR_C_STEP_Pos                2U                                            /*!< DCB DHCSR: Step control Position */
#define DCB_DHCSR_C_STEP_Msk               (1UL << DCB_DHCSR_C_STEP_Pos)                  /*!< DCB DHCSR: Step control Mask */

#define DCB_DHCSR_C_HALT_Pos                1U                                            /*!< DCB DHCSR: Halt control Position */
#define DCB_DHCSR_C_HALT_Msk               (1UL << DCB_DHCSR_C_HALT_Pos)                  /*!< DCB DHCSR: Halt control Mask */

#define DCB_DHCSR_C_DEBUGEN_Pos             0U                                            /*!< DCB DHCSR: Debug enable control Position */
#define DCB_DHCSR_C_DEBUGEN_Msk            (1UL /*<< DCB_DHCSR_C_DEBUGEN_Pos*/)           /*!< DCB DHCSR: Debug enable control Mask */

/** \brief DCB Debug Core Register Selector Register Definitions */
#define DCB_DCRSR_REGWnR_Pos               16U                                            /*!< DCB DCRSR: Register write/not-read Position */
#define DCB_DCRSR_REGWnR_Msk               (1UL << DCB_DCRSR_REGWnR_Pos)                  /*!< DCB DCRSR: Register write/not-read Mask */

#define DCB_DCRSR_REGSEL_Pos                0U                                            /*!< DCB DCRSR: Register selector Position */
#define DCB_DCRSR_REGSEL_Msk               (0x7FUL /*<< DCB_DCRSR_REGSEL_Pos*/)           /*!< DCB DCRSR: Register selector Mask */

/** \brief DCB Debug Core Register Data Register Definitions */
#define DCB_DCRDR_DBGTMP_Pos                0U                                            /*!< DCB DCRDR: Data temporary buffer Position */
#define DCB_DCRDR_DBGTMP_Msk               (0xFFFFFFFFUL /*<< DCB_DCRDR_DBGTMP_Pos*/)     /*!< DCB DCRDR: Data temporary buffer Mask */

/** \brief DCB Debug Exception and Monitor Control Register Definitions */
#define DCB_DEMCR_TRCENA_Pos               24U                                            /*!< DCB DEMCR: Trace enable Position */
#define DCB_DEMCR_TRCENA_Msk               (1UL << DCB_DEMCR_TRCENA_Pos)                  /*!< DCB DEMCR: Trace enable Mask */

#define DCB_DEMCR_MON_REQ_Pos              19U                                            /*!< DCB DEMCR: Monitor request Position */
#define DCB_DEMCR_MON_REQ_Msk              (1UL << DCB_DEMCR_MON_REQ_Pos)                 /*!< DCB DEMCR: Monitor request Mask */

#define DCB_DEMCR_MON_STEP_Pos             18U                                            /*!< DCB DEMCR: Monitor step Position */
#define DCB_DEMCR_MON_STEP_Msk             (1UL << DCB_DEMCR_MON_STEP_Pos)                /*!< DCB DEMCR: Monitor step Mask */

#define DCB_DEMCR_MON_PEND_Pos             17U                                            /*!< DCB DEMCR: Monitor pend Position */
#define DCB_DEMCR_MON_PEND_Msk             (1UL << DCB_DEMCR_MON_PEND_Pos)                /*!< DCB DEMCR: Monitor pend Mask */

#define DCB_DEMCR_MON_EN_Pos               16U                                            /*!< DCB DEMCR: Monitor enable Position */
#define DCB_DEMCR_MON_EN_Msk               (1UL << DCB_DEMCR_MON_EN_Pos)                  /*!< DCB DEMCR: Monitor enable Mask */

#define DCB_DEMCR_VC_HARDERR_Pos           10U                                            /*!< DCB DEMCR: Vector Catch HardFault errors Position */
#define DCB_DEMCR_VC_HARDERR_Msk           (1UL << DCB_DEMCR_VC_HARDERR_Pos)              /*!< DCB DEMCR: Vector Catch HardFault errors Mask */

#define DCB_DEMCR_VC_INTERR_Pos             9U                                            /*!< DCB DEMCR: Vector Catch interrupt errors Position */
#define DCB_DEMCR_VC_INTERR_Msk            (1UL << DCB_DEMCR_VC_INTERR_Pos)               /*!< DCB DEMCR: Vector Catch interrupt errors Mask */

#define DCB_DEMCR_VC_BUSERR_Pos             8U                                            /*!< DCB DEMCR: Vector Catch BusFault errors Position */
#define DCB_DEMCR_VC_BUSERR_Msk            (1UL << DCB_DEMCR_VC_BUSERR_Pos)               /*!< DCB DEMCR: Vector Catch BusFault errors Mask */

#define DCB_DEMCR_VC_STATERR_Pos            7U                                            /*!< DCB DEMCR: Vector Catch state errors Position */
#define DCB_DEMCR_VC_STATERR_Msk           (1UL << DCB_DEMCR_VC_STATERR_Pos)              /*!< DCB DEMCR: Vector Catch state errors Mask */

#define DCB_DEMCR_VC_CHKERR_Pos             6U                                            /*!< DCB DEMCR: Vector Catch check errors Position */
#define DCB_DEMCR_VC_CHKERR_Msk            (1UL << DCB_DEMCR_VC_CHKERR_Pos)               /*!< DCB DEMCR: Vector Catch check errors Mask */

#define DCB_DEMCR_VC_NOCPERR_Pos            5U                                            /*!< DCB DEMCR: Vector Catch NOCP errors Position */
#define DCB_DEMCR_VC_NOCPERR_Msk           (1UL << DCB_DEMCR_VC_NOCPERR_Pos)              /*!< DCB DEMCR: Vector Catch NOCP errors Mask */

#define DCB_DEMCR_VC_MMERR_Pos              4U                                            /*!< DCB DEMCR: Vector Catch MemManage errors Position */
#define DCB_DEMCR_VC_MMERR_Msk             (1UL << DCB_DEMCR_VC_MMERR_Pos)                /*!< DCB DEMCR: Vector Catch MemManage errors Mask */

#define DCB_DEMCR_VC_CORERESET_Pos          0U                                            /*!< DCB DEMCR: Vector Catch Core reset Position */
#define DCB_DEMCR_VC_CORERESET_Msk         (1UL /*<< DCB_DEMCR_VC_CORERESET_Pos*/)        /*!< DCB DEMCR: Vector Catch Core reset Mask */

/*@} end of group CMSIS_DCB */


/**
  \ingroup    CMSIS_core_register
  \defgroup   CMSIS_core_bitfield     Core register bit field macros
  \brief      Macros for use with bit field definitions (xxx_Pos, xxx_Msk).
  @{
 */

/**
  \brief   Mask and shift a bit field value for use in a register bit range.
  \param[in] field  Name of the register bit field.
  \param[in] value  Value of the bit field. This parameter is interpreted as an uint32_t type.
  \return           Masked and shifted value.
*/
#define _VAL2FLD(field, value)    (((uint32_t)(value) << field ## _Pos) & field ## _Msk)

/**
  \brief     Mask and shift a register value to extract a bit field value.
  \param[in] field  Name of the register bit field.
  \param[in] value  Value of register. This parameter is interpreted as an uint32_t type.
  \return           Masked and shifted bit field value.
*/
#define _FLD2VAL(field, value)    (((uint32_t)(value) & field ## _Msk) >> field ## _Pos)

/*@} end of group CMSIS_core_bitfield */


/**
  \ingroup    CMSIS_core_register
  \defgroup   CMSIS_core_base     Core Definitions
  \brief      Definitions for base addresses, unions, and structures.
  @{
 */

/* Memory mapping of Core Hardware */
#define SCS_BASE            (0xE000E000UL)                            /*!< System Control Space Base Address */
#define ITM_BASE            (0xE0000000UL)                            /*!< ITM Base Address */
#define DWT_BASE            (0xE0001000UL)                            /*!< DWT Base Address */
#define TPIU_BASE           (0xE0040000UL)                            /*!< TPIU Base Address */
#define DCB_BASE            (0xE000EDF0UL)                            /*!< Core Debug Base Address */
#define SysTick_BASE        (SCS_BASE +  0x0010UL)                    /*!< SysTick Base Address */
#define NVIC_BASE           (SCS_BASE +  0x0100UL)                    /*!< NVIC Base Address */
#define SCB_BASE            (SCS_BASE +  0x0D00UL)                    /*!< System Control Block Base Address */

#define SCnSCB              ((SCnSCB_Type    *)     SCS_BASE      )   /*!< System control Register not in SCB */
#define SCB                 ((SCB_Type       *)     SCB_BASE      )   /*!< SCB configuration struct */
#define SysTick             ((SysTick_Type   *)     SysTick_BASE  )   /*!< SysTick configuration struct */
#define NVIC                ((NVIC_Type      *)     NVIC_BASE     )   /*!< NVIC configuration struct */
#define ITM                 ((ITM_Type       *)     ITM_BASE      )   /*!< ITM configuration struct */
#define DWT                 ((DWT_Type       *)     DWT_BASE      )   /*!< DWT configuration struct */
#define TPIU                ((TPIU_Type      *)     TPIU_BASE     )   /*!< TPIU configuration struct */
#define DCB                 ((DCB_Type       *)     DCB_BASE      )   /*!< DCB configuration struct */

#if defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U)
  #define MPU_BASE          (SCS_BASE +  0x0D90UL)                    /*!< Memory Protection Unit */
  #define MPU               ((MPU_Type       *)     MPU_BASE      )   /*!< Memory Protection Unit */
#endif

/*@} */


/**
  \defgroup   CMSIS_deprecated_aliases     Backwards Compatibility Aliases
  \brief      Alias definitions present for backwards compatibility for deprecated symbols.
  @{
 */
 
#ifndef CMSIS_DISABLE_DEPRECATED

#define SCB_AIRCR_ENDIANESS_Pos            SCB_AIRCR_ENDIANNESS_Pos
#define SCB_AIRCR_ENDIANESS_Msk            SCB_AIRCR_ENDIANNESS_Msk

/* deprecated, CMSIS_5 backward compatibility */
typedef struct
{
  __IOM uint32_t DHCSR;
  __OM  uint32_t DCRSR;
  __IOM uint32_t DCRDR;
  __IOM uint32_t DEMCR;
} CoreDebug_Type;

/* Debug Halting Control and Status Register Definitions */
#define CoreDebug_DHCSR_DBGKEY_Pos         DCB_DHCSR_DBGKEY_Pos
#define CoreDebug_DHCSR_DBGKEY_Msk         DCB_DHCSR_DBGKEY_Msk

#define CoreDebug_DHCSR_S_RESET_ST_Pos     DCB_DHCSR_S_RESET_ST_Pos
#define CoreDebug_DHCSR_S_RESET_ST_Msk     DCB_DHCSR_S_RESET_ST_Msk

#define CoreDebug_DHCSR_S_RETIRE_ST_Pos    DCB_DHCSR_S_RETIRE_ST_Pos
#define CoreDebug_DHCSR_S_RETIRE_ST_Msk    DCB_DHCSR_S_RETIRE_ST_Msk

#define CoreDebug_DHCSR_S_LOCKUP_Pos       DCB_DHCSR_S_LOCKUP_Pos
#define CoreDebug_DHCSR_S_LOCKUP_Msk       DCB_DHCSR_S_LOCKUP_Msk

#define CoreDebug_DHCSR_S_SLEEP_Pos        DCB_DHCSR_S_SLEEP_Pos
#define CoreDebug_DHCSR_S_SLEEP_Msk        DCB_DHCSR_S_SLEEP_Msk

#define CoreDebug_DHCSR_S_HALT_Pos         DCB_DHCSR_S_HALT_Pos
#define CoreDebug_DHCSR_S_HALT_Msk         DCB_DHCSR_S_HALT_Msk

#define CoreDebug_DHCSR_S_REGRDY_Pos       DCB_DHCSR_S_REGRDY_Pos
#define CoreDebug_DHCSR_S_REGRDY_Msk       DCB_DHCSR_S_REGRDY_Msk

#define CoreDebug_DHCSR_C_SNAPSTALL_Pos    DCB_DHCSR_C_SNAPSTALL_Pos
#define CoreDebug_DHCSR_C_SNAPSTALL_Msk    DCB_DHCSR_C_SNAPSTALL_Msk

#define CoreDebug_DHCSR_C_MASKINTS_Pos     DCB_DHCSR_C_MASKINTS_Pos
#define CoreDebug_DHCSR_C_MASKINTS_Msk     DCB_DHCSR_C_MASKINTS_Msk

#define CoreDebug_DHCSR_C_STEP_Pos         DCB_DHCSR_C_STEP_Pos
#define CoreDebug_DHCSR_C_STEP_Msk         DCB_DHCSR_C_STEP_Msk

#define CoreDebug_DHCSR_C_HALT_Pos         DCB_DHCSR_C_HALT_Pos
#define CoreDebug_DHCSR_C_HALT_Msk         DCB_DHCSR_C_HALT_Msk

#define CoreDebug_DHCSR_C_DEBUGEN_Pos      DCB_DHCSR_C_DEBUGEN_Pos
#define CoreDebug_DHCSR_C_DEBUGEN_Msk      DCB_DHCSR_C_DEBUGEN_Msk

/* Debug Core Register Selector Register Definitions */
#define CoreDebug_DCRSR_REGWnR_Pos         DCB_DCRSR_REGWnR_Pos
#define CoreDebug_DCRSR_REGWnR_Msk         DCB_DCRSR_REGWnR_Msk

#define CoreDebug_DCRSR_REGSEL_Pos         DCB_DCRSR_REGSEL_Pos
#define CoreDebug_DCRSR_REGSEL_Msk         DCB_DCRSR_REGSEL_Msk

/* Debug Exception and Monitor Control Register Definitions */
#define CoreDebug_DEMCR_TRCENA_Pos         DCB_DEMCR_TRCENA_Pos
#define CoreDebug_DEMCR_TRCENA_Msk         DCB_DEMCR_TRCENA_Msk

#define CoreDebug_DEMCR_MON_REQ_Pos        DCB_DEMCR_MON_REQ_Pos
#define CoreDebug_DEMCR_MON_REQ_Msk        DCB_DEMCR_MON_REQ_Msk

#define CoreDebug_DEMCR_MON_STEP_Pos       DCB_DEMCR_MON_STEP_Pos
#define CoreDebug_DEMCR_MON_STEP_Msk       DCB_DEMCR_MON_STEP_Msk

#define CoreDebug_DEMCR_MON_PEND_Pos       DCB_DEMCR_MON_PEND_Pos
#define CoreDebug_DEMCR_MON_PEND_Msk       DCB_DEMCR_MON_PEND_Msk

#define CoreDebug_DEMCR_MON_EN_Pos         DCB_DEMCR_MON_EN_Pos
#define CoreDebug_DEMCR_MON_EN_Msk         DCB_DEMCR_MON_EN_Msk

#define CoreDebug_DEMCR_VC_HARDERR_Pos     DCB_DEMCR_VC_HARDERR_Pos
#define CoreDebug_DEMCR_VC_HARDERR_Msk     DCB_DEMCR_VC_HARDERR_Msk

#define CoreDebug_DEMCR_VC_INTERR_Pos      DCB_DEMCR_VC_INTERR_Pos
#define CoreDebug_DEMCR_VC_INTERR_Msk      DCB_DEMCR_VC_INTERR_Msk

#define CoreDebug_DEMCR_VC_BUSERR_Pos      DCB_DEMCR_VC_BUSERR_Pos
#define CoreDebug_DEMCR_VC_BUSERR_Msk      DCB_DEMCR_VC_BUSERR_Msk

#define CoreDebug_DEMCR_VC_STATERR_Pos     DCB_DEMCR_VC_STATERR_Pos
#define CoreDebug_DEMCR_VC_STATERR_Msk     DCB_DEMCR_VC_STATERR_Msk

#define CoreDebug_DEMCR_VC_CHKERR_Pos      DCB_DEMCR_VC_CHKERR_Pos
#define CoreDebug_DEMCR_VC_CHKERR_Msk      DCB_DEMCR_VC_CHKERR_Msk

#define CoreDebug_DEMCR_VC_NOCPERR_Pos     DCB_DEMCR_VC_NOCPERR_Pos
#define CoreDebug_DEMCR_VC_NOCPERR_Msk     DCB_DEMCR_VC_NOCPERR_Msk

#define CoreDebug_DEMCR_VC_MMERR_Pos       DCB_DEMCR_VC_MMERR_Pos
#define CoreDebug_DEMCR_VC_MMERR_Msk       DCB_DEMCR_VC_MMERR_Msk

#define CoreDebug_DEMCR_VC_CORERESET_Pos   DCB_DEMCR_VC_CORERESET_Pos
#define CoreDebug_DEMCR_VC_CORERESET_Msk   DCB_DEMCR_VC_CORERESET_Msk

#define CoreDebug           ((CoreDebug_Type *)     DCB_BASE)

#endif // CMSIS_DISABLE_DEPRECATED

/*@} */


/*******************************************************************************
 *                Hardware Abstraction Layer
  Core Function Interface contains:
  - Core NVIC Functions
  - Core SysTick Functions
  - Core Debug Functions
  - Core Register Access Functions
 ******************************************************************************/
/**
  \defgroup CMSIS_Core_FunctionInterface Functions and Instructions Reference
*/



/* ##########################   NVIC functions  #################################### */
/**
  \ingroup  CMSIS_Core_FunctionInterface
  \defgroup CMSIS_Core_NVICFunctions NVIC Functions
  \brief    Functions that manage interrupts and exceptions via the NVIC.
  @{
 */

#ifdef CMSIS_NVIC_VIRTUAL
  #ifndef CMSIS_NVIC_VIRTUAL_HEADER_FILE
    #define CMSIS_NVIC_VIRTUAL_HEADER_FILE "cmsis_nvic_virtual.h"
  #endif
  #include CMSIS_NVIC_VIRTUAL_HEADER_FILE
#else
  #define NVIC_SetPriorityGrouping    __NVIC_SetPriorityGrouping
  #define NVIC_GetPriorityGrouping    __NVIC_GetPriorityGrouping
  #define NVIC_EnableIRQ              __NVIC_EnableIRQ
  #define NVIC_GetEnableIRQ           __NVIC_GetEnableIRQ
  #define NVIC_DisableIRQ             __NVIC_DisableIRQ
  #define NVIC_GetPendingIRQ          __NVIC_GetPendingIRQ
  #define NVIC_SetPendingIRQ          __NVIC_SetPendingIRQ
  #define NVIC_ClearPendingIRQ        __NVIC_ClearPendingIRQ
  #define NVIC_GetActive              __NVIC_GetActive
  #define NVIC_SetPriority            __NVIC_SetPriority
  #define NVIC_GetPriority            __NVIC_GetPriority
  #define NVIC_SystemReset            __NVIC_SystemReset
#endif /* CMSIS_NVIC_VIRTUAL */

#ifdef CMSIS_VECTAB_VIRTUAL
  #ifndef CMSIS_VECTAB_VIRTUAL_HEADER_FILE
    #define CMSIS_VECTAB_VIRTUAL_HEADER_FILE "cmsis_vectab_virtual.h"
  #endif
  #include CMSIS_VECTAB_VIRTUAL_HEADER_FILE
#else
  #define NVIC_SetVector              __NVIC_SetVector
  #define NVIC_GetVector              __NVIC_GetVector
#endif  /* (CMSIS_VECTAB_VIRTUAL) */

#define NVIC_USER_IRQ_OFFSET          16


/* The following EXC_RETURN values are saved the LR on exception entry */
#define EXC_RETURN_HANDLER         (0xFFFFFFF1UL)     /* return to Handler mode, uses MSP after return                               */
#define EXC_RETURN_THREAD_MSP      (0xFFFFFFF9UL)     /* return to Thread mode, uses MSP after return                                */
#define EXC_RETURN_THREAD_PSP      (0xFFFFFFFDUL)     /* return to Thread mode, uses PSP after return                                */


/**
  \brief   Set Priority Grouping
  \details Sets the priority grouping field using the required unlock sequence.
           The parameter PriorityGroup is assigned to the field SCB->AIRCR [10:8] PRIGROUP field.
           Only values from 0..7 are used.
           In case of a conflict between priority grouping and available
           priority bits (__NVIC_PRIO_BITS), the smallest possible priority group is set.
  \param [in]      PriorityGroup  Priority grouping field.
 */
__STATIC_INLINE void __NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
  uint32_t reg_value;
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);             /* only values 0..7 are used          */

  reg_value  =  SCB->AIRCR;                                                   /* read old register configuration    */
  reg_value &= ~((uint32_t)(SCB_AIRCR_VECTKEY_Msk | SCB_AIRCR_PRIGROUP_Msk)); /* clear bits to change               */
  reg_value  =  (reg_value                                   |
                ((uint32_t)0x5FAUL << SCB_AIRCR_VECTKEY_Pos) |
                (PriorityGroupTmp << SCB_AIRCR_PRIGROUP_Pos)  );              /* Insert write key and priority group */
  SCB->AIRCR =  reg_value;
}


/**
  \brief   Get Priority Grouping
  \details Reads the priority grouping field from the NVIC Interrupt Controller.
  \return                Priority grouping field (SCB->AIRCR [10:8] PRIGROUP field).
 */
__STATIC_INLINE uint32_t __NVIC_GetPriorityGrouping(void)
{
  return ((uint32_t)((SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) >> SCB_AIRCR_PRIGROUP_Pos));
}


/**
  \brief   Enable Interrupt
  \details Enables a device specific interrupt in the NVIC interrupt controller.
  \param [in]      IRQn  Device specific interrupt number.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE void __NVIC_EnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    __COMPILER_BARRIER();
    NVIC->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __COMPILER_BARRIER();
  }
}


/**
  \brief   Get Interrupt Enable status
  \details Returns a device specific interrupt enable status from the NVIC interrupt controller.
  \param [in]      IRQn  Device specific interrupt number.
  \return             0  Interrupt is not enabled.
  \return             1  Interrupt is enabled.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE uint32_t __NVIC_GetEnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((NVIC->ISER[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}


/**
  \brief   Disable Interrupt
  \details Disables a device specific interrupt in the NVIC interrupt controller.
  \param [in]      IRQn  Device specific interrupt number.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE void __NVIC_DisableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    NVIC->ICER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __DSB();
    __ISB();
  }
}


/**
  \brief   Get Pending Interrupt
  \details Reads the NVIC pending register and returns the pending bit for the specified device specific interrupt.
  \param [in]      IRQn  Device specific interrupt number.
  \return             0  Interrupt status is not pending.
  \return             1  Interrupt status is pending.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE uint32_t __NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((NVIC->ISPR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}


/**
  \brief   Set Pending Interrupt
  \details Sets the pending bit of a device specific interrupt in the NVIC pending register.
  \param [in]      IRQn  Device specific interrupt number.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE void __NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    NVIC->ISPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}


/**
  \brief   Clear Pending Interrupt
  \details Clears the pending bit of a device specific interrupt in the NVIC pending register.
  \param [in]      IRQn  Device specific interrupt number.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE void __NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    NVIC->ICPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}


/**
  \brief   Get Active Interrupt
  \details Reads the active register in the NVIC and returns the active bit for the device specific interrupt.
  \param [in]      IRQn  Device specific interrupt number.
  \return             0  Interrupt status is not active.
  \return             1  Interrupt status is active.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE uint32_t __NVIC_GetActive(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((NVIC->IABR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}


/**
  \brief   Set Interrupt Priority
  \details Sets the priority of a device specific interrupt or a processor exception.
           The interrupt number can be positive to specify a device specific interrupt,
           or negative to specify a processor exception.
  \param [in]      IRQn  Interrupt number.
  \param [in]  priority  Priority to set.
  \note    The priority cannot be set for every processor exception.
 */
__STATIC_INLINE void __NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  if ((int32_t)(IRQn) >= 0)
  {
    NVIC->IPR[((uint32_t)IRQn)]               = (uint8_t)((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL);
  }
  else
  {
    SCB->SHPR[(((uint32_t)IRQn) & 0xFUL)-4UL] = (uint8_t)((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL);
  }
}


/**
  \brief   Get Interrupt Priority
  \details Reads the priority of a device specific interrupt or a processor exception.
           The interrupt number can be positive to specify a device specific interrupt,
           or negative to specify a processor exception.
  \param [in]   IRQn  Interrupt number.
  \return             Interrupt Priority.
                      Value is aligned automatically to the implemented priority bits of the microcontroller.
 */
__STATIC_INLINE uint32_t __NVIC_GetPriority(IRQn_Type IRQn)
{

  if ((int32_t)(IRQn) >= 0)
  {
    return(((uint32_t)NVIC->IPR[((uint32_t)IRQn)]               >> (8U - __NVIC_PRIO_BITS)));
  }
  else
  {
    return(((uint32_t)SCB->SHPR[(((uint32_t)IRQn) & 0xFUL)-4UL] >> (8U - __NVIC_PRIO_BITS)));
  }
}


/**
  \brief   Encode Priority
  \details Encodes the priority for an interrupt with the given priority group,
           preemptive priority value, and subpriority value.
           In case of a conflict between priority grouping and available
           priority bits (__NVIC_PRIO_BITS), the smallest possible priority group is set.
  \param [in]     PriorityGroup  Used priority group.
  \param [in]   PreemptPriority  Preemptive priority value (starting from 0).
  \param [in]       SubPriority  Subpriority value (starting from 0).
  \return                        Encoded priority. Value can be used in the function \ref NVIC_SetPriority().
 */
__STATIC_INLINE uint32_t NVIC_EncodePriority (uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority)
{
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);   /* only values 0..7 are used          */
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(__NVIC_PRIO_BITS)) ? (uint32_t)(__NVIC_PRIO_BITS) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits     = ((PriorityGroupTmp + (uint32_t)(__NVIC_PRIO_BITS)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(__NVIC_PRIO_BITS));

  return (
           ((PreemptPriority & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL)) << SubPriorityBits) |
           ((SubPriority     & (uint32_t)((1UL << (SubPriorityBits    )) - 1UL)))
         );
}


/**
  \brief   Decode Priority
  \details Decodes an interrupt priority value with a given priority group to
           preemptive priority value and subpriority value.
           In case of a conflict between priority grouping and available
           priority bits (__NVIC_PRIO_BITS) the smallest possible priority group is set.
  \param [in]         Priority   Priority value, which can be retrieved with the function \ref NVIC_GetPriority().
  \param [in]     PriorityGroup  Used priority group.
  \param [out] pPreemptPriority  Preemptive priority value (starting from 0).
  \param [out]     pSubPriority  Subpriority value (starting from 0).
 */
__STATIC_INLINE void NVIC_DecodePriority (uint32_t Priority, uint32_t PriorityGroup, uint32_t* const pPreemptPriority, uint32_t* const pSubPriority)
{
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);   /* only values 0..7 are used          */
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(__NVIC_PRIO_BITS)) ? (uint32_t)(__NVIC_PRIO_BITS) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits     = ((PriorityGroupTmp + (uint32_t)(__NVIC_PRIO_BITS)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(__NVIC_PRIO_BITS));

  *pPreemptPriority = (Priority >> SubPriorityBits) & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL);
  *pSubPriority     = (Priority                   ) & (uint32_t)((1UL << (SubPriorityBits    )) - 1UL);
}


/**
  \brief   Set Interrupt Vector
  \details Sets an interrupt vector in SRAM based interrupt vector table.
           The interrupt number can be positive to specify a device specific interrupt,
           or negative to specify a processor exception.
           VTOR must been relocated to SRAM before.
  \param [in]   IRQn      Interrupt number
  \param [in]   vector    Address of interrupt handler function
 */
__STATIC_INLINE void __NVIC_SetVector(IRQn_Type IRQn, uint32_t vector)
{
  uint32_t *vectors = (uint32_t *) ((uintptr_t) SCB->VTOR);
  vectors[(int32_t)IRQn + NVIC_USER_IRQ_OFFSET] = vector;
  /* ARM Application Note 321 states that the M3 does not require the architectural barrier */
}


/**
  \brief   Get Interrupt Vector
  \details Reads an interrupt vector from interrupt vector table.
           The interrupt number can be positive to specify a device specific interrupt,
           or negative to specify a processor exception.
  \param [in]   IRQn      Interrupt number.
  \return                 Address of interrupt handler function
 */
__STATIC_INLINE uint32_t __NVIC_GetVector(IRQn_Type IRQn)
{
  uint32_t *vectors = (uint32_t *) ((uintptr_t) SCB->VTOR);
  return vectors[(int32_t)IRQn + NVIC_USER_IRQ_OFFSET];
}


/**
  \brief   System Reset
  \details Initiates a system reset request to reset the MCU.
 */
__NO_RETURN __STATIC_INLINE void __NVIC_SystemReset(void)
{
  __DSB();                                                          /* Ensure all outstanding memory accesses included
                                                                       buffered write are completed before reset */
  SCB->AIRCR  = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos)    |
                           (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                            SCB_AIRCR_SYSRESETREQ_Msk    );         /* Keep priority group unchanged */
  __DSB();                                                          /* Ensure completion of memory access */

  for(;;)                                                           /* wait until reset */
  {
    __NOP();
  }
}

/*@} end of CMSIS_Core_NVICFunctions */


/* ##########################  MPU functions  #################################### */

#if defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U)

#include "m-profile/armv7m_mpu.h"

#endif


/* ##########################  FPU functions  #################################### */
/**
  \ingroup  CMSIS_Core_FunctionInterface
  \defgroup CMSIS_Core_FpuFunctions FPU Functions
  \brief    Function that provides FPU type.
  @{
 */

/**
  \brief   get FPU type
  \details returns the FPU type
  \returns
   - \b  0: No FPU
   - \b  1: Single precision FPU
   - \b  2: Double + Single precision FPU
 */
__STATIC_INLINE uint32_t SCB_GetFPUType(void)
{
    return 0U;           /* No FPU */
}

/*@} end of CMSIS_Core_FpuFunctions */


/* ##################################    SysTick function  ############################################ */
/**
  \ingroup  CMSIS_Core_FunctionInterface
  \defgroup CMSIS_Core_SysTickFunctions SysTick Functions
  \brief    Functions that configure the System.
  @{
 */

#if defined (__Vendor_SysTickConfig) && (__Vendor_SysTickConfig == 0U)

/**
  \brief   System Tick Configuration
  \details Initializes the System Timer and its interrupt, and starts the System Tick Timer.
           Counter is in free running mode to generate periodic interrupts.
  \param [in]  ticks  Number of ticks between two interrupts.
  \return          0  Function succeeded.
  \return          1  Function failed.
  \note    When the variable <b>__Vendor_SysTickConfig</b> is set to 1, then the
           function <b>SysTick_Config</b> is not included. In this case, the file <b><i>device</i>.h</b>
           must contain a vendor-specific implementation of this function.
 */
__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks)
{
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
  {
    return (1UL);                                                   /* Reload value impossible */
  }

  SysTick->LOAD  = (uint32_t)(ticks - 1UL);                         /* set reload register */
  NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
  SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
  return (0UL);                                                     /* Function successful */
}

#endif

/*@} end of CMSIS_Core_SysTickFunctions */



/* ##################################### Debug In/Output function ########################################### */
/**
  \ingroup  CMSIS_Core_FunctionInterface
  \defgroup CMSIS_core_DebugFunctions ITM Functions
  \brief    Functions that access the ITM debug interface.
  @{
 */

extern volatile int32_t ITM_RxBuffer;                              /*!< External variable to receive characters. */
#define                 ITM_RXBUFFER_EMPTY  ((int32_t)0x5AA55AA5U) /*!< Value identifying \ref ITM_RxBuffer is ready for next character. */


/**
  \brief   ITM Send Character
  \details Transmits a character via the ITM channel 0, and
           \li Just returns when no debugger is connected that has booked the output.
           \li Is blocking when a debugger is connected, but the previous character sent has not been transmitted.
  \param [in]     ch  Character to transmit.
  \returns            Character to transmit.
 */
__STATIC_INLINE uint32_t ITM_SendChar (uint32_t ch)
{
  if (((ITM->TCR & ITM_TCR_ITMENA_Msk) != 0UL) &&      /* ITM enabled */
      ((ITM->TER & 1UL               ) != 0UL)   )     /* ITM Port #0 enabled */
  {
    while (ITM->PORT[0U].u32 == 0UL)
    {
      __NOP();
    }
    ITM->PORT[0U].u8 = (uint8_t)ch;
  }
  return (ch);
}


/**
  \brief   ITM Receive Character
  \details Inputs a character via the external variable \ref ITM_RxBuffer.
  \return             Received character.
  \return         -1  No character pending.
 */
__STATIC_INLINE int32_t ITM_ReceiveChar (void)
{
  int32_t ch = -1;                           /* no character available */

  if (ITM_RxBuffer != ITM_RXBUFFER_EMPTY)
  {
    ch = ITM_RxBuffer;
    ITM_RxBuffer = ITM_RXBUFFER_EMPTY;       /* ready for next character */
  }

  return (ch);
}


/**
  \brief   ITM Check Character
  \details Checks whether a character is pending for reading in the variable \ref ITM_RxBuffer.
  \return          0  No character available.
  \return          1  Character available.
 */
__STATIC_INLINE int32_t ITM_CheckChar (void)
{

  if (ITM_RxBuffer == ITM_RXBUFFER_EMPTY)
  {
    return (0);                              /* no character available */
  }
  else
  {
    return (1);                              /*    character available */
  }
}

/*@} end of CMSIS_core_DebugFunctions */




#ifdef __cplusplus
}
#endif

#endif /* __CORE_SC300_H_DEPENDANT */

#endif /* __CMSIS_GENERIC */
