/**
 * \file
 *
 * \brief Component description for RSTC
 *
 * Copyright (c) 2017 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD51_RSTC_COMPONENT_
#define _SAMD51_RSTC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR RSTC */
/* ========================================================================== */
/** \addtogroup SAMD51_RSTC Reset Controller */
/*@{*/

#define RSTC_U2239
#define REV_RSTC                    0x400

/* -------- RSTC_RCAUSE : (RSTC Offset: 0x00) (R/   8) Reset Cause -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  POR:1;            /*!< bit:      0  Power On Reset                     */
    uint8_t  BODCORE:1;        /*!< bit:      1  Brown Out CORE Detector Reset      */
    uint8_t  BODVDD:1;         /*!< bit:      2  Brown Out VDD Detector Reset       */
    uint8_t  NVM:1;            /*!< bit:      3  NVM Reset                          */
    uint8_t  EXT:1;            /*!< bit:      4  External Reset                     */
    uint8_t  WDT:1;            /*!< bit:      5  Watchdog Reset                     */
    uint8_t  SYST:1;           /*!< bit:      6  System Reset Request               */
    uint8_t  BACKUP:1;         /*!< bit:      7  Backup Reset                       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} RSTC_RCAUSE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RSTC_RCAUSE_OFFSET          0x00         /**< \brief (RSTC_RCAUSE offset) Reset Cause */

#define RSTC_RCAUSE_POR_Pos         0            /**< \brief (RSTC_RCAUSE) Power On Reset */
#define RSTC_RCAUSE_POR             (_U_(0x1) << RSTC_RCAUSE_POR_Pos)
#define RSTC_RCAUSE_BODCORE_Pos     1            /**< \brief (RSTC_RCAUSE) Brown Out CORE Detector Reset */
#define RSTC_RCAUSE_BODCORE         (_U_(0x1) << RSTC_RCAUSE_BODCORE_Pos)
#define RSTC_RCAUSE_BODVDD_Pos      2            /**< \brief (RSTC_RCAUSE) Brown Out VDD Detector Reset */
#define RSTC_RCAUSE_BODVDD          (_U_(0x1) << RSTC_RCAUSE_BODVDD_Pos)
#define RSTC_RCAUSE_NVM_Pos         3            /**< \brief (RSTC_RCAUSE) NVM Reset */
#define RSTC_RCAUSE_NVM             (_U_(0x1) << RSTC_RCAUSE_NVM_Pos)
#define RSTC_RCAUSE_EXT_Pos         4            /**< \brief (RSTC_RCAUSE) External Reset */
#define RSTC_RCAUSE_EXT             (_U_(0x1) << RSTC_RCAUSE_EXT_Pos)
#define RSTC_RCAUSE_WDT_Pos         5            /**< \brief (RSTC_RCAUSE) Watchdog Reset */
#define RSTC_RCAUSE_WDT             (_U_(0x1) << RSTC_RCAUSE_WDT_Pos)
#define RSTC_RCAUSE_SYST_Pos        6            /**< \brief (RSTC_RCAUSE) System Reset Request */
#define RSTC_RCAUSE_SYST            (_U_(0x1) << RSTC_RCAUSE_SYST_Pos)
#define RSTC_RCAUSE_BACKUP_Pos      7            /**< \brief (RSTC_RCAUSE) Backup Reset */
#define RSTC_RCAUSE_BACKUP          (_U_(0x1) << RSTC_RCAUSE_BACKUP_Pos)
#define RSTC_RCAUSE_MASK            _U_(0xFF)    /**< \brief (RSTC_RCAUSE) MASK Register */

/* -------- RSTC_BKUPEXIT : (RSTC Offset: 0x02) (R/   8) Backup Exit Source -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :1;               /*!< bit:      0  Reserved                           */
    uint8_t  RTC:1;            /*!< bit:      1  Real Timer Counter Interrupt       */
    uint8_t  BBPS:1;           /*!< bit:      2  Battery Backup Power Switch        */
    uint8_t  :4;               /*!< bit:  3.. 6  Reserved                           */
    uint8_t  HIB:1;            /*!< bit:      7  Hibernate                          */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} RSTC_BKUPEXIT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RSTC_BKUPEXIT_OFFSET        0x02         /**< \brief (RSTC_BKUPEXIT offset) Backup Exit Source */
#define RSTC_BKUPEXIT_RESETVALUE    _U_(0x00)    /**< \brief (RSTC_BKUPEXIT reset_value) Backup Exit Source */

#define RSTC_BKUPEXIT_RTC_Pos       1            /**< \brief (RSTC_BKUPEXIT) Real Timer Counter Interrupt */
#define RSTC_BKUPEXIT_RTC           (_U_(0x1) << RSTC_BKUPEXIT_RTC_Pos)
#define RSTC_BKUPEXIT_BBPS_Pos      2            /**< \brief (RSTC_BKUPEXIT) Battery Backup Power Switch */
#define RSTC_BKUPEXIT_BBPS          (_U_(0x1) << RSTC_BKUPEXIT_BBPS_Pos)
#define RSTC_BKUPEXIT_HIB_Pos       7            /**< \brief (RSTC_BKUPEXIT) Hibernate */
#define RSTC_BKUPEXIT_HIB           (_U_(0x1) << RSTC_BKUPEXIT_HIB_Pos)
#define RSTC_BKUPEXIT_MASK          _U_(0x86)    /**< \brief (RSTC_BKUPEXIT) MASK Register */

/** \brief RSTC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __I  RSTC_RCAUSE_Type          RCAUSE;      /**< \brief Offset: 0x00 (R/   8) Reset Cause */
       RoReg8                    Reserved1[0x1];
  __I  RSTC_BKUPEXIT_Type        BKUPEXIT;    /**< \brief Offset: 0x02 (R/   8) Backup Exit Source */
} Rstc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_RSTC_COMPONENT_ */
