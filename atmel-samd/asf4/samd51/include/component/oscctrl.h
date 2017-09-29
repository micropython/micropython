/**
 * \file
 *
 * \brief Component description for OSCCTRL
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

#ifndef _SAMD51_OSCCTRL_COMPONENT_
#define _SAMD51_OSCCTRL_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR OSCCTRL */
/* ========================================================================== */
/** \addtogroup SAMD51_OSCCTRL Oscillators Control */
/*@{*/

#define OSCCTRL_U2401
#define REV_OSCCTRL                 0x100

/* -------- OSCCTRL_EVCTRL : (OSCCTRL Offset: 0x00) (R/W  8) Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CFDEO0:1;         /*!< bit:      0  Clock 0 Failure Detector Event Output Enable */
    uint8_t  CFDEO1:1;         /*!< bit:      1  Clock 1 Failure Detector Event Output Enable */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  CFDEO:2;          /*!< bit:  0.. 1  Clock x Failure Detector Event Output Enable */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} OSCCTRL_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSCCTRL_EVCTRL_OFFSET       0x00         /**< \brief (OSCCTRL_EVCTRL offset) Event Control */
#define OSCCTRL_EVCTRL_RESETVALUE   _U_(0x00)    /**< \brief (OSCCTRL_EVCTRL reset_value) Event Control */

#define OSCCTRL_EVCTRL_CFDEO0_Pos   0            /**< \brief (OSCCTRL_EVCTRL) Clock 0 Failure Detector Event Output Enable */
#define OSCCTRL_EVCTRL_CFDEO0       (_U_(1) << OSCCTRL_EVCTRL_CFDEO0_Pos)
#define OSCCTRL_EVCTRL_CFDEO1_Pos   1            /**< \brief (OSCCTRL_EVCTRL) Clock 1 Failure Detector Event Output Enable */
#define OSCCTRL_EVCTRL_CFDEO1       (_U_(1) << OSCCTRL_EVCTRL_CFDEO1_Pos)
#define OSCCTRL_EVCTRL_CFDEO_Pos    0            /**< \brief (OSCCTRL_EVCTRL) Clock x Failure Detector Event Output Enable */
#define OSCCTRL_EVCTRL_CFDEO_Msk    (_U_(0x3) << OSCCTRL_EVCTRL_CFDEO_Pos)
#define OSCCTRL_EVCTRL_CFDEO(value) (OSCCTRL_EVCTRL_CFDEO_Msk & ((value) << OSCCTRL_EVCTRL_CFDEO_Pos))
#define OSCCTRL_EVCTRL_MASK         _U_(0x03)    /**< \brief (OSCCTRL_EVCTRL) MASK Register */

/* -------- OSCCTRL_INTENCLR : (OSCCTRL Offset: 0x04) (R/W 32) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t XOSCRDY0:1;       /*!< bit:      0  XOSC 0 Ready Interrupt Enable      */
    uint32_t XOSCRDY1:1;       /*!< bit:      1  XOSC 1 Ready Interrupt Enable      */
    uint32_t XOSCFAIL0:1;      /*!< bit:      2  XOSC 0 Clock Failure Detector Interrupt Enable */
    uint32_t XOSCFAIL1:1;      /*!< bit:      3  XOSC 1 Clock Failure Detector Interrupt Enable */
    uint32_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint32_t DFLLRDY:1;        /*!< bit:      8  DFLL Ready Interrupt Enable        */
    uint32_t DFLLOOB:1;        /*!< bit:      9  DFLL Out Of Bounds Interrupt Enable */
    uint32_t DFLLLCKF:1;       /*!< bit:     10  DFLL Lock Fine Interrupt Enable    */
    uint32_t DFLLLCKC:1;       /*!< bit:     11  DFLL Lock Coarse Interrupt Enable  */
    uint32_t DFLLRCS:1;        /*!< bit:     12  DFLL Reference Clock Stopped Interrupt Enable */
    uint32_t :3;               /*!< bit: 13..15  Reserved                           */
    uint32_t DPLL0LCKR:1;      /*!< bit:     16  DPLL0 Lock Rise Interrupt Enable   */
    uint32_t DPLL0LCKF:1;      /*!< bit:     17  DPLL0 Lock Fall Interrupt Enable   */
    uint32_t DPLL0LTO:1;       /*!< bit:     18  DPLL0 Lock Timeout Interrupt Enable */
    uint32_t DPLL0LDRTO:1;     /*!< bit:     19  DPLL0 Loop Divider Ratio Update Complete Interrupt Enable */
    uint32_t :4;               /*!< bit: 20..23  Reserved                           */
    uint32_t DPLL1LCKR:1;      /*!< bit:     24  DPLL1 Lock Rise Interrupt Enable   */
    uint32_t DPLL1LCKF:1;      /*!< bit:     25  DPLL1 Lock Fall Interrupt Enable   */
    uint32_t DPLL1LTO:1;       /*!< bit:     26  DPLL1 Lock Timeout Interrupt Enable */
    uint32_t DPLL1LDRTO:1;     /*!< bit:     27  DPLL1 Loop Divider Ratio Update Complete Interrupt Enable */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t XOSCRDY:2;        /*!< bit:  0.. 1  XOSC x Ready Interrupt Enable      */
    uint32_t XOSCFAIL:2;       /*!< bit:  2.. 3  XOSC x Clock Failure Detector Interrupt Enable */
    uint32_t :28;              /*!< bit:  4..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OSCCTRL_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSCCTRL_INTENCLR_OFFSET     0x04         /**< \brief (OSCCTRL_INTENCLR offset) Interrupt Enable Clear */
#define OSCCTRL_INTENCLR_RESETVALUE _U_(0x00000000) /**< \brief (OSCCTRL_INTENCLR reset_value) Interrupt Enable Clear */

#define OSCCTRL_INTENCLR_XOSCRDY0_Pos 0            /**< \brief (OSCCTRL_INTENCLR) XOSC 0 Ready Interrupt Enable */
#define OSCCTRL_INTENCLR_XOSCRDY0   (_U_(1) << OSCCTRL_INTENCLR_XOSCRDY0_Pos)
#define OSCCTRL_INTENCLR_XOSCRDY1_Pos 1            /**< \brief (OSCCTRL_INTENCLR) XOSC 1 Ready Interrupt Enable */
#define OSCCTRL_INTENCLR_XOSCRDY1   (_U_(1) << OSCCTRL_INTENCLR_XOSCRDY1_Pos)
#define OSCCTRL_INTENCLR_XOSCRDY_Pos 0            /**< \brief (OSCCTRL_INTENCLR) XOSC x Ready Interrupt Enable */
#define OSCCTRL_INTENCLR_XOSCRDY_Msk (_U_(0x3) << OSCCTRL_INTENCLR_XOSCRDY_Pos)
#define OSCCTRL_INTENCLR_XOSCRDY(value) (OSCCTRL_INTENCLR_XOSCRDY_Msk & ((value) << OSCCTRL_INTENCLR_XOSCRDY_Pos))
#define OSCCTRL_INTENCLR_XOSCFAIL0_Pos 2            /**< \brief (OSCCTRL_INTENCLR) XOSC 0 Clock Failure Detector Interrupt Enable */
#define OSCCTRL_INTENCLR_XOSCFAIL0  (_U_(1) << OSCCTRL_INTENCLR_XOSCFAIL0_Pos)
#define OSCCTRL_INTENCLR_XOSCFAIL1_Pos 3            /**< \brief (OSCCTRL_INTENCLR) XOSC 1 Clock Failure Detector Interrupt Enable */
#define OSCCTRL_INTENCLR_XOSCFAIL1  (_U_(1) << OSCCTRL_INTENCLR_XOSCFAIL1_Pos)
#define OSCCTRL_INTENCLR_XOSCFAIL_Pos 2            /**< \brief (OSCCTRL_INTENCLR) XOSC x Clock Failure Detector Interrupt Enable */
#define OSCCTRL_INTENCLR_XOSCFAIL_Msk (_U_(0x3) << OSCCTRL_INTENCLR_XOSCFAIL_Pos)
#define OSCCTRL_INTENCLR_XOSCFAIL(value) (OSCCTRL_INTENCLR_XOSCFAIL_Msk & ((value) << OSCCTRL_INTENCLR_XOSCFAIL_Pos))
#define OSCCTRL_INTENCLR_DFLLRDY_Pos 8            /**< \brief (OSCCTRL_INTENCLR) DFLL Ready Interrupt Enable */
#define OSCCTRL_INTENCLR_DFLLRDY    (_U_(0x1) << OSCCTRL_INTENCLR_DFLLRDY_Pos)
#define OSCCTRL_INTENCLR_DFLLOOB_Pos 9            /**< \brief (OSCCTRL_INTENCLR) DFLL Out Of Bounds Interrupt Enable */
#define OSCCTRL_INTENCLR_DFLLOOB    (_U_(0x1) << OSCCTRL_INTENCLR_DFLLOOB_Pos)
#define OSCCTRL_INTENCLR_DFLLLCKF_Pos 10           /**< \brief (OSCCTRL_INTENCLR) DFLL Lock Fine Interrupt Enable */
#define OSCCTRL_INTENCLR_DFLLLCKF   (_U_(0x1) << OSCCTRL_INTENCLR_DFLLLCKF_Pos)
#define OSCCTRL_INTENCLR_DFLLLCKC_Pos 11           /**< \brief (OSCCTRL_INTENCLR) DFLL Lock Coarse Interrupt Enable */
#define OSCCTRL_INTENCLR_DFLLLCKC   (_U_(0x1) << OSCCTRL_INTENCLR_DFLLLCKC_Pos)
#define OSCCTRL_INTENCLR_DFLLRCS_Pos 12           /**< \brief (OSCCTRL_INTENCLR) DFLL Reference Clock Stopped Interrupt Enable */
#define OSCCTRL_INTENCLR_DFLLRCS    (_U_(0x1) << OSCCTRL_INTENCLR_DFLLRCS_Pos)
#define OSCCTRL_INTENCLR_DPLL0LCKR_Pos 16           /**< \brief (OSCCTRL_INTENCLR) DPLL0 Lock Rise Interrupt Enable */
#define OSCCTRL_INTENCLR_DPLL0LCKR  (_U_(0x1) << OSCCTRL_INTENCLR_DPLL0LCKR_Pos)
#define OSCCTRL_INTENCLR_DPLL0LCKF_Pos 17           /**< \brief (OSCCTRL_INTENCLR) DPLL0 Lock Fall Interrupt Enable */
#define OSCCTRL_INTENCLR_DPLL0LCKF  (_U_(0x1) << OSCCTRL_INTENCLR_DPLL0LCKF_Pos)
#define OSCCTRL_INTENCLR_DPLL0LTO_Pos 18           /**< \brief (OSCCTRL_INTENCLR) DPLL0 Lock Timeout Interrupt Enable */
#define OSCCTRL_INTENCLR_DPLL0LTO   (_U_(0x1) << OSCCTRL_INTENCLR_DPLL0LTO_Pos)
#define OSCCTRL_INTENCLR_DPLL0LDRTO_Pos 19           /**< \brief (OSCCTRL_INTENCLR) DPLL0 Loop Divider Ratio Update Complete Interrupt Enable */
#define OSCCTRL_INTENCLR_DPLL0LDRTO (_U_(0x1) << OSCCTRL_INTENCLR_DPLL0LDRTO_Pos)
#define OSCCTRL_INTENCLR_DPLL1LCKR_Pos 24           /**< \brief (OSCCTRL_INTENCLR) DPLL1 Lock Rise Interrupt Enable */
#define OSCCTRL_INTENCLR_DPLL1LCKR  (_U_(0x1) << OSCCTRL_INTENCLR_DPLL1LCKR_Pos)
#define OSCCTRL_INTENCLR_DPLL1LCKF_Pos 25           /**< \brief (OSCCTRL_INTENCLR) DPLL1 Lock Fall Interrupt Enable */
#define OSCCTRL_INTENCLR_DPLL1LCKF  (_U_(0x1) << OSCCTRL_INTENCLR_DPLL1LCKF_Pos)
#define OSCCTRL_INTENCLR_DPLL1LTO_Pos 26           /**< \brief (OSCCTRL_INTENCLR) DPLL1 Lock Timeout Interrupt Enable */
#define OSCCTRL_INTENCLR_DPLL1LTO   (_U_(0x1) << OSCCTRL_INTENCLR_DPLL1LTO_Pos)
#define OSCCTRL_INTENCLR_DPLL1LDRTO_Pos 27           /**< \brief (OSCCTRL_INTENCLR) DPLL1 Loop Divider Ratio Update Complete Interrupt Enable */
#define OSCCTRL_INTENCLR_DPLL1LDRTO (_U_(0x1) << OSCCTRL_INTENCLR_DPLL1LDRTO_Pos)
#define OSCCTRL_INTENCLR_MASK       _U_(0x0F0F1F0F) /**< \brief (OSCCTRL_INTENCLR) MASK Register */

/* -------- OSCCTRL_INTENSET : (OSCCTRL Offset: 0x08) (R/W 32) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t XOSCRDY0:1;       /*!< bit:      0  XOSC 0 Ready Interrupt Enable      */
    uint32_t XOSCRDY1:1;       /*!< bit:      1  XOSC 1 Ready Interrupt Enable      */
    uint32_t XOSCFAIL0:1;      /*!< bit:      2  XOSC 0 Clock Failure Detector Interrupt Enable */
    uint32_t XOSCFAIL1:1;      /*!< bit:      3  XOSC 1 Clock Failure Detector Interrupt Enable */
    uint32_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint32_t DFLLRDY:1;        /*!< bit:      8  DFLL Ready Interrupt Enable        */
    uint32_t DFLLOOB:1;        /*!< bit:      9  DFLL Out Of Bounds Interrupt Enable */
    uint32_t DFLLLCKF:1;       /*!< bit:     10  DFLL Lock Fine Interrupt Enable    */
    uint32_t DFLLLCKC:1;       /*!< bit:     11  DFLL Lock Coarse Interrupt Enable  */
    uint32_t DFLLRCS:1;        /*!< bit:     12  DFLL Reference Clock Stopped Interrupt Enable */
    uint32_t :3;               /*!< bit: 13..15  Reserved                           */
    uint32_t DPLL0LCKR:1;      /*!< bit:     16  DPLL0 Lock Rise Interrupt Enable   */
    uint32_t DPLL0LCKF:1;      /*!< bit:     17  DPLL0 Lock Fall Interrupt Enable   */
    uint32_t DPLL0LTO:1;       /*!< bit:     18  DPLL0 Lock Timeout Interrupt Enable */
    uint32_t DPLL0LDRTO:1;     /*!< bit:     19  DPLL0 Loop Divider Ratio Update Complete Interrupt Enable */
    uint32_t :4;               /*!< bit: 20..23  Reserved                           */
    uint32_t DPLL1LCKR:1;      /*!< bit:     24  DPLL1 Lock Rise Interrupt Enable   */
    uint32_t DPLL1LCKF:1;      /*!< bit:     25  DPLL1 Lock Fall Interrupt Enable   */
    uint32_t DPLL1LTO:1;       /*!< bit:     26  DPLL1 Lock Timeout Interrupt Enable */
    uint32_t DPLL1LDRTO:1;     /*!< bit:     27  DPLL1 Loop Divider Ratio Update Complete Interrupt Enable */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t XOSCRDY:2;        /*!< bit:  0.. 1  XOSC x Ready Interrupt Enable      */
    uint32_t XOSCFAIL:2;       /*!< bit:  2.. 3  XOSC x Clock Failure Detector Interrupt Enable */
    uint32_t :28;              /*!< bit:  4..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OSCCTRL_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSCCTRL_INTENSET_OFFSET     0x08         /**< \brief (OSCCTRL_INTENSET offset) Interrupt Enable Set */
#define OSCCTRL_INTENSET_RESETVALUE _U_(0x00000000) /**< \brief (OSCCTRL_INTENSET reset_value) Interrupt Enable Set */

#define OSCCTRL_INTENSET_XOSCRDY0_Pos 0            /**< \brief (OSCCTRL_INTENSET) XOSC 0 Ready Interrupt Enable */
#define OSCCTRL_INTENSET_XOSCRDY0   (_U_(1) << OSCCTRL_INTENSET_XOSCRDY0_Pos)
#define OSCCTRL_INTENSET_XOSCRDY1_Pos 1            /**< \brief (OSCCTRL_INTENSET) XOSC 1 Ready Interrupt Enable */
#define OSCCTRL_INTENSET_XOSCRDY1   (_U_(1) << OSCCTRL_INTENSET_XOSCRDY1_Pos)
#define OSCCTRL_INTENSET_XOSCRDY_Pos 0            /**< \brief (OSCCTRL_INTENSET) XOSC x Ready Interrupt Enable */
#define OSCCTRL_INTENSET_XOSCRDY_Msk (_U_(0x3) << OSCCTRL_INTENSET_XOSCRDY_Pos)
#define OSCCTRL_INTENSET_XOSCRDY(value) (OSCCTRL_INTENSET_XOSCRDY_Msk & ((value) << OSCCTRL_INTENSET_XOSCRDY_Pos))
#define OSCCTRL_INTENSET_XOSCFAIL0_Pos 2            /**< \brief (OSCCTRL_INTENSET) XOSC 0 Clock Failure Detector Interrupt Enable */
#define OSCCTRL_INTENSET_XOSCFAIL0  (_U_(1) << OSCCTRL_INTENSET_XOSCFAIL0_Pos)
#define OSCCTRL_INTENSET_XOSCFAIL1_Pos 3            /**< \brief (OSCCTRL_INTENSET) XOSC 1 Clock Failure Detector Interrupt Enable */
#define OSCCTRL_INTENSET_XOSCFAIL1  (_U_(1) << OSCCTRL_INTENSET_XOSCFAIL1_Pos)
#define OSCCTRL_INTENSET_XOSCFAIL_Pos 2            /**< \brief (OSCCTRL_INTENSET) XOSC x Clock Failure Detector Interrupt Enable */
#define OSCCTRL_INTENSET_XOSCFAIL_Msk (_U_(0x3) << OSCCTRL_INTENSET_XOSCFAIL_Pos)
#define OSCCTRL_INTENSET_XOSCFAIL(value) (OSCCTRL_INTENSET_XOSCFAIL_Msk & ((value) << OSCCTRL_INTENSET_XOSCFAIL_Pos))
#define OSCCTRL_INTENSET_DFLLRDY_Pos 8            /**< \brief (OSCCTRL_INTENSET) DFLL Ready Interrupt Enable */
#define OSCCTRL_INTENSET_DFLLRDY    (_U_(0x1) << OSCCTRL_INTENSET_DFLLRDY_Pos)
#define OSCCTRL_INTENSET_DFLLOOB_Pos 9            /**< \brief (OSCCTRL_INTENSET) DFLL Out Of Bounds Interrupt Enable */
#define OSCCTRL_INTENSET_DFLLOOB    (_U_(0x1) << OSCCTRL_INTENSET_DFLLOOB_Pos)
#define OSCCTRL_INTENSET_DFLLLCKF_Pos 10           /**< \brief (OSCCTRL_INTENSET) DFLL Lock Fine Interrupt Enable */
#define OSCCTRL_INTENSET_DFLLLCKF   (_U_(0x1) << OSCCTRL_INTENSET_DFLLLCKF_Pos)
#define OSCCTRL_INTENSET_DFLLLCKC_Pos 11           /**< \brief (OSCCTRL_INTENSET) DFLL Lock Coarse Interrupt Enable */
#define OSCCTRL_INTENSET_DFLLLCKC   (_U_(0x1) << OSCCTRL_INTENSET_DFLLLCKC_Pos)
#define OSCCTRL_INTENSET_DFLLRCS_Pos 12           /**< \brief (OSCCTRL_INTENSET) DFLL Reference Clock Stopped Interrupt Enable */
#define OSCCTRL_INTENSET_DFLLRCS    (_U_(0x1) << OSCCTRL_INTENSET_DFLLRCS_Pos)
#define OSCCTRL_INTENSET_DPLL0LCKR_Pos 16           /**< \brief (OSCCTRL_INTENSET) DPLL0 Lock Rise Interrupt Enable */
#define OSCCTRL_INTENSET_DPLL0LCKR  (_U_(0x1) << OSCCTRL_INTENSET_DPLL0LCKR_Pos)
#define OSCCTRL_INTENSET_DPLL0LCKF_Pos 17           /**< \brief (OSCCTRL_INTENSET) DPLL0 Lock Fall Interrupt Enable */
#define OSCCTRL_INTENSET_DPLL0LCKF  (_U_(0x1) << OSCCTRL_INTENSET_DPLL0LCKF_Pos)
#define OSCCTRL_INTENSET_DPLL0LTO_Pos 18           /**< \brief (OSCCTRL_INTENSET) DPLL0 Lock Timeout Interrupt Enable */
#define OSCCTRL_INTENSET_DPLL0LTO   (_U_(0x1) << OSCCTRL_INTENSET_DPLL0LTO_Pos)
#define OSCCTRL_INTENSET_DPLL0LDRTO_Pos 19           /**< \brief (OSCCTRL_INTENSET) DPLL0 Loop Divider Ratio Update Complete Interrupt Enable */
#define OSCCTRL_INTENSET_DPLL0LDRTO (_U_(0x1) << OSCCTRL_INTENSET_DPLL0LDRTO_Pos)
#define OSCCTRL_INTENSET_DPLL1LCKR_Pos 24           /**< \brief (OSCCTRL_INTENSET) DPLL1 Lock Rise Interrupt Enable */
#define OSCCTRL_INTENSET_DPLL1LCKR  (_U_(0x1) << OSCCTRL_INTENSET_DPLL1LCKR_Pos)
#define OSCCTRL_INTENSET_DPLL1LCKF_Pos 25           /**< \brief (OSCCTRL_INTENSET) DPLL1 Lock Fall Interrupt Enable */
#define OSCCTRL_INTENSET_DPLL1LCKF  (_U_(0x1) << OSCCTRL_INTENSET_DPLL1LCKF_Pos)
#define OSCCTRL_INTENSET_DPLL1LTO_Pos 26           /**< \brief (OSCCTRL_INTENSET) DPLL1 Lock Timeout Interrupt Enable */
#define OSCCTRL_INTENSET_DPLL1LTO   (_U_(0x1) << OSCCTRL_INTENSET_DPLL1LTO_Pos)
#define OSCCTRL_INTENSET_DPLL1LDRTO_Pos 27           /**< \brief (OSCCTRL_INTENSET) DPLL1 Loop Divider Ratio Update Complete Interrupt Enable */
#define OSCCTRL_INTENSET_DPLL1LDRTO (_U_(0x1) << OSCCTRL_INTENSET_DPLL1LDRTO_Pos)
#define OSCCTRL_INTENSET_MASK       _U_(0x0F0F1F0F) /**< \brief (OSCCTRL_INTENSET) MASK Register */

/* -------- OSCCTRL_INTFLAG : (OSCCTRL Offset: 0x0C) (R/W 32) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t XOSCRDY0:1;       /*!< bit:      0  XOSC 0 Ready                       */
    __I uint32_t XOSCRDY1:1;       /*!< bit:      1  XOSC 1 Ready                       */
    __I uint32_t XOSCFAIL0:1;      /*!< bit:      2  XOSC 0 Clock Failure Detector      */
    __I uint32_t XOSCFAIL1:1;      /*!< bit:      3  XOSC 1 Clock Failure Detector      */
    __I uint32_t :4;               /*!< bit:  4.. 7  Reserved                           */
    __I uint32_t DFLLRDY:1;        /*!< bit:      8  DFLL Ready                         */
    __I uint32_t DFLLOOB:1;        /*!< bit:      9  DFLL Out Of Bounds                 */
    __I uint32_t DFLLLCKF:1;       /*!< bit:     10  DFLL Lock Fine                     */
    __I uint32_t DFLLLCKC:1;       /*!< bit:     11  DFLL Lock Coarse                   */
    __I uint32_t DFLLRCS:1;        /*!< bit:     12  DFLL Reference Clock Stopped       */
    __I uint32_t :3;               /*!< bit: 13..15  Reserved                           */
    __I uint32_t DPLL0LCKR:1;      /*!< bit:     16  DPLL0 Lock Rise                    */
    __I uint32_t DPLL0LCKF:1;      /*!< bit:     17  DPLL0 Lock Fall                    */
    __I uint32_t DPLL0LTO:1;       /*!< bit:     18  DPLL0 Lock Timeout                 */
    __I uint32_t DPLL0LDRTO:1;     /*!< bit:     19  DPLL0 Loop Divider Ratio Update Complete */
    __I uint32_t :4;               /*!< bit: 20..23  Reserved                           */
    __I uint32_t DPLL1LCKR:1;      /*!< bit:     24  DPLL1 Lock Rise                    */
    __I uint32_t DPLL1LCKF:1;      /*!< bit:     25  DPLL1 Lock Fall                    */
    __I uint32_t DPLL1LTO:1;       /*!< bit:     26  DPLL1 Lock Timeout                 */
    __I uint32_t DPLL1LDRTO:1;     /*!< bit:     27  DPLL1 Loop Divider Ratio Update Complete */
    __I uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint32_t XOSCRDY:2;        /*!< bit:  0.. 1  XOSC x Ready                       */
    __I uint32_t XOSCFAIL:2;       /*!< bit:  2.. 3  XOSC x Clock Failure Detector      */
    __I uint32_t :28;              /*!< bit:  4..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OSCCTRL_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSCCTRL_INTFLAG_OFFSET      0x0C         /**< \brief (OSCCTRL_INTFLAG offset) Interrupt Flag Status and Clear */
#define OSCCTRL_INTFLAG_RESETVALUE  _U_(0x00000000) /**< \brief (OSCCTRL_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define OSCCTRL_INTFLAG_XOSCRDY0_Pos 0            /**< \brief (OSCCTRL_INTFLAG) XOSC 0 Ready */
#define OSCCTRL_INTFLAG_XOSCRDY0    (_U_(1) << OSCCTRL_INTFLAG_XOSCRDY0_Pos)
#define OSCCTRL_INTFLAG_XOSCRDY1_Pos 1            /**< \brief (OSCCTRL_INTFLAG) XOSC 1 Ready */
#define OSCCTRL_INTFLAG_XOSCRDY1    (_U_(1) << OSCCTRL_INTFLAG_XOSCRDY1_Pos)
#define OSCCTRL_INTFLAG_XOSCRDY_Pos 0            /**< \brief (OSCCTRL_INTFLAG) XOSC x Ready */
#define OSCCTRL_INTFLAG_XOSCRDY_Msk (_U_(0x3) << OSCCTRL_INTFLAG_XOSCRDY_Pos)
#define OSCCTRL_INTFLAG_XOSCRDY(value) (OSCCTRL_INTFLAG_XOSCRDY_Msk & ((value) << OSCCTRL_INTFLAG_XOSCRDY_Pos))
#define OSCCTRL_INTFLAG_XOSCFAIL0_Pos 2            /**< \brief (OSCCTRL_INTFLAG) XOSC 0 Clock Failure Detector */
#define OSCCTRL_INTFLAG_XOSCFAIL0   (_U_(1) << OSCCTRL_INTFLAG_XOSCFAIL0_Pos)
#define OSCCTRL_INTFLAG_XOSCFAIL1_Pos 3            /**< \brief (OSCCTRL_INTFLAG) XOSC 1 Clock Failure Detector */
#define OSCCTRL_INTFLAG_XOSCFAIL1   (_U_(1) << OSCCTRL_INTFLAG_XOSCFAIL1_Pos)
#define OSCCTRL_INTFLAG_XOSCFAIL_Pos 2            /**< \brief (OSCCTRL_INTFLAG) XOSC x Clock Failure Detector */
#define OSCCTRL_INTFLAG_XOSCFAIL_Msk (_U_(0x3) << OSCCTRL_INTFLAG_XOSCFAIL_Pos)
#define OSCCTRL_INTFLAG_XOSCFAIL(value) (OSCCTRL_INTFLAG_XOSCFAIL_Msk & ((value) << OSCCTRL_INTFLAG_XOSCFAIL_Pos))
#define OSCCTRL_INTFLAG_DFLLRDY_Pos 8            /**< \brief (OSCCTRL_INTFLAG) DFLL Ready */
#define OSCCTRL_INTFLAG_DFLLRDY     (_U_(0x1) << OSCCTRL_INTFLAG_DFLLRDY_Pos)
#define OSCCTRL_INTFLAG_DFLLOOB_Pos 9            /**< \brief (OSCCTRL_INTFLAG) DFLL Out Of Bounds */
#define OSCCTRL_INTFLAG_DFLLOOB     (_U_(0x1) << OSCCTRL_INTFLAG_DFLLOOB_Pos)
#define OSCCTRL_INTFLAG_DFLLLCKF_Pos 10           /**< \brief (OSCCTRL_INTFLAG) DFLL Lock Fine */
#define OSCCTRL_INTFLAG_DFLLLCKF    (_U_(0x1) << OSCCTRL_INTFLAG_DFLLLCKF_Pos)
#define OSCCTRL_INTFLAG_DFLLLCKC_Pos 11           /**< \brief (OSCCTRL_INTFLAG) DFLL Lock Coarse */
#define OSCCTRL_INTFLAG_DFLLLCKC    (_U_(0x1) << OSCCTRL_INTFLAG_DFLLLCKC_Pos)
#define OSCCTRL_INTFLAG_DFLLRCS_Pos 12           /**< \brief (OSCCTRL_INTFLAG) DFLL Reference Clock Stopped */
#define OSCCTRL_INTFLAG_DFLLRCS     (_U_(0x1) << OSCCTRL_INTFLAG_DFLLRCS_Pos)
#define OSCCTRL_INTFLAG_DPLL0LCKR_Pos 16           /**< \brief (OSCCTRL_INTFLAG) DPLL0 Lock Rise */
#define OSCCTRL_INTFLAG_DPLL0LCKR   (_U_(0x1) << OSCCTRL_INTFLAG_DPLL0LCKR_Pos)
#define OSCCTRL_INTFLAG_DPLL0LCKF_Pos 17           /**< \brief (OSCCTRL_INTFLAG) DPLL0 Lock Fall */
#define OSCCTRL_INTFLAG_DPLL0LCKF   (_U_(0x1) << OSCCTRL_INTFLAG_DPLL0LCKF_Pos)
#define OSCCTRL_INTFLAG_DPLL0LTO_Pos 18           /**< \brief (OSCCTRL_INTFLAG) DPLL0 Lock Timeout */
#define OSCCTRL_INTFLAG_DPLL0LTO    (_U_(0x1) << OSCCTRL_INTFLAG_DPLL0LTO_Pos)
#define OSCCTRL_INTFLAG_DPLL0LDRTO_Pos 19           /**< \brief (OSCCTRL_INTFLAG) DPLL0 Loop Divider Ratio Update Complete */
#define OSCCTRL_INTFLAG_DPLL0LDRTO  (_U_(0x1) << OSCCTRL_INTFLAG_DPLL0LDRTO_Pos)
#define OSCCTRL_INTFLAG_DPLL1LCKR_Pos 24           /**< \brief (OSCCTRL_INTFLAG) DPLL1 Lock Rise */
#define OSCCTRL_INTFLAG_DPLL1LCKR   (_U_(0x1) << OSCCTRL_INTFLAG_DPLL1LCKR_Pos)
#define OSCCTRL_INTFLAG_DPLL1LCKF_Pos 25           /**< \brief (OSCCTRL_INTFLAG) DPLL1 Lock Fall */
#define OSCCTRL_INTFLAG_DPLL1LCKF   (_U_(0x1) << OSCCTRL_INTFLAG_DPLL1LCKF_Pos)
#define OSCCTRL_INTFLAG_DPLL1LTO_Pos 26           /**< \brief (OSCCTRL_INTFLAG) DPLL1 Lock Timeout */
#define OSCCTRL_INTFLAG_DPLL1LTO    (_U_(0x1) << OSCCTRL_INTFLAG_DPLL1LTO_Pos)
#define OSCCTRL_INTFLAG_DPLL1LDRTO_Pos 27           /**< \brief (OSCCTRL_INTFLAG) DPLL1 Loop Divider Ratio Update Complete */
#define OSCCTRL_INTFLAG_DPLL1LDRTO  (_U_(0x1) << OSCCTRL_INTFLAG_DPLL1LDRTO_Pos)
#define OSCCTRL_INTFLAG_MASK        _U_(0x0F0F1F0F) /**< \brief (OSCCTRL_INTFLAG) MASK Register */

/* -------- OSCCTRL_STATUS : (OSCCTRL Offset: 0x10) (R/  32) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t XOSCRDY0:1;       /*!< bit:      0  XOSC 0 Ready                       */
    uint32_t XOSCRDY1:1;       /*!< bit:      1  XOSC 1 Ready                       */
    uint32_t XOSCFAIL0:1;      /*!< bit:      2  XOSC 0 Clock Failure Detector      */
    uint32_t XOSCFAIL1:1;      /*!< bit:      3  XOSC 1 Clock Failure Detector      */
    uint32_t XOSCCKSW0:1;      /*!< bit:      4  XOSC 0 Clock Switch                */
    uint32_t XOSCCKSW1:1;      /*!< bit:      5  XOSC 1 Clock Switch                */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t DFLLRDY:1;        /*!< bit:      8  DFLL Ready                         */
    uint32_t DFLLOOB:1;        /*!< bit:      9  DFLL Out Of Bounds                 */
    uint32_t DFLLLCKF:1;       /*!< bit:     10  DFLL Lock Fine                     */
    uint32_t DFLLLCKC:1;       /*!< bit:     11  DFLL Lock Coarse                   */
    uint32_t DFLLRCS:1;        /*!< bit:     12  DFLL Reference Clock Stopped       */
    uint32_t :3;               /*!< bit: 13..15  Reserved                           */
    uint32_t DPLL0LCKR:1;      /*!< bit:     16  DPLL0 Lock Rise                    */
    uint32_t DPLL0LCKF:1;      /*!< bit:     17  DPLL0 Lock Fall                    */
    uint32_t DPLL0TO:1;        /*!< bit:     18  DPLL0 Timeout                      */
    uint32_t DPLL0LDRTO:1;     /*!< bit:     19  DPLL0 Loop Divider Ratio Update Complete */
    uint32_t :4;               /*!< bit: 20..23  Reserved                           */
    uint32_t DPLL1LCKR:1;      /*!< bit:     24  DPLL1 Lock Rise                    */
    uint32_t DPLL1LCKF:1;      /*!< bit:     25  DPLL1 Lock Fall                    */
    uint32_t DPLL1TO:1;        /*!< bit:     26  DPLL1 Timeout                      */
    uint32_t DPLL1LDRTO:1;     /*!< bit:     27  DPLL1 Loop Divider Ratio Update Complete */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t XOSCRDY:2;        /*!< bit:  0.. 1  XOSC x Ready                       */
    uint32_t XOSCFAIL:2;       /*!< bit:  2.. 3  XOSC x Clock Failure Detector      */
    uint32_t XOSCCKSW:2;       /*!< bit:  4.. 5  XOSC x Clock Switch                */
    uint32_t :26;              /*!< bit:  6..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OSCCTRL_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSCCTRL_STATUS_OFFSET       0x10         /**< \brief (OSCCTRL_STATUS offset) Status */
#define OSCCTRL_STATUS_RESETVALUE   _U_(0x00000000) /**< \brief (OSCCTRL_STATUS reset_value) Status */

#define OSCCTRL_STATUS_XOSCRDY0_Pos 0            /**< \brief (OSCCTRL_STATUS) XOSC 0 Ready */
#define OSCCTRL_STATUS_XOSCRDY0     (_U_(1) << OSCCTRL_STATUS_XOSCRDY0_Pos)
#define OSCCTRL_STATUS_XOSCRDY1_Pos 1            /**< \brief (OSCCTRL_STATUS) XOSC 1 Ready */
#define OSCCTRL_STATUS_XOSCRDY1     (_U_(1) << OSCCTRL_STATUS_XOSCRDY1_Pos)
#define OSCCTRL_STATUS_XOSCRDY_Pos  0            /**< \brief (OSCCTRL_STATUS) XOSC x Ready */
#define OSCCTRL_STATUS_XOSCRDY_Msk  (_U_(0x3) << OSCCTRL_STATUS_XOSCRDY_Pos)
#define OSCCTRL_STATUS_XOSCRDY(value) (OSCCTRL_STATUS_XOSCRDY_Msk & ((value) << OSCCTRL_STATUS_XOSCRDY_Pos))
#define OSCCTRL_STATUS_XOSCFAIL0_Pos 2            /**< \brief (OSCCTRL_STATUS) XOSC 0 Clock Failure Detector */
#define OSCCTRL_STATUS_XOSCFAIL0    (_U_(1) << OSCCTRL_STATUS_XOSCFAIL0_Pos)
#define OSCCTRL_STATUS_XOSCFAIL1_Pos 3            /**< \brief (OSCCTRL_STATUS) XOSC 1 Clock Failure Detector */
#define OSCCTRL_STATUS_XOSCFAIL1    (_U_(1) << OSCCTRL_STATUS_XOSCFAIL1_Pos)
#define OSCCTRL_STATUS_XOSCFAIL_Pos 2            /**< \brief (OSCCTRL_STATUS) XOSC x Clock Failure Detector */
#define OSCCTRL_STATUS_XOSCFAIL_Msk (_U_(0x3) << OSCCTRL_STATUS_XOSCFAIL_Pos)
#define OSCCTRL_STATUS_XOSCFAIL(value) (OSCCTRL_STATUS_XOSCFAIL_Msk & ((value) << OSCCTRL_STATUS_XOSCFAIL_Pos))
#define OSCCTRL_STATUS_XOSCCKSW0_Pos 4            /**< \brief (OSCCTRL_STATUS) XOSC 0 Clock Switch */
#define OSCCTRL_STATUS_XOSCCKSW0    (_U_(1) << OSCCTRL_STATUS_XOSCCKSW0_Pos)
#define OSCCTRL_STATUS_XOSCCKSW1_Pos 5            /**< \brief (OSCCTRL_STATUS) XOSC 1 Clock Switch */
#define OSCCTRL_STATUS_XOSCCKSW1    (_U_(1) << OSCCTRL_STATUS_XOSCCKSW1_Pos)
#define OSCCTRL_STATUS_XOSCCKSW_Pos 4            /**< \brief (OSCCTRL_STATUS) XOSC x Clock Switch */
#define OSCCTRL_STATUS_XOSCCKSW_Msk (_U_(0x3) << OSCCTRL_STATUS_XOSCCKSW_Pos)
#define OSCCTRL_STATUS_XOSCCKSW(value) (OSCCTRL_STATUS_XOSCCKSW_Msk & ((value) << OSCCTRL_STATUS_XOSCCKSW_Pos))
#define OSCCTRL_STATUS_DFLLRDY_Pos  8            /**< \brief (OSCCTRL_STATUS) DFLL Ready */
#define OSCCTRL_STATUS_DFLLRDY      (_U_(0x1) << OSCCTRL_STATUS_DFLLRDY_Pos)
#define OSCCTRL_STATUS_DFLLOOB_Pos  9            /**< \brief (OSCCTRL_STATUS) DFLL Out Of Bounds */
#define OSCCTRL_STATUS_DFLLOOB      (_U_(0x1) << OSCCTRL_STATUS_DFLLOOB_Pos)
#define OSCCTRL_STATUS_DFLLLCKF_Pos 10           /**< \brief (OSCCTRL_STATUS) DFLL Lock Fine */
#define OSCCTRL_STATUS_DFLLLCKF     (_U_(0x1) << OSCCTRL_STATUS_DFLLLCKF_Pos)
#define OSCCTRL_STATUS_DFLLLCKC_Pos 11           /**< \brief (OSCCTRL_STATUS) DFLL Lock Coarse */
#define OSCCTRL_STATUS_DFLLLCKC     (_U_(0x1) << OSCCTRL_STATUS_DFLLLCKC_Pos)
#define OSCCTRL_STATUS_DFLLRCS_Pos  12           /**< \brief (OSCCTRL_STATUS) DFLL Reference Clock Stopped */
#define OSCCTRL_STATUS_DFLLRCS      (_U_(0x1) << OSCCTRL_STATUS_DFLLRCS_Pos)
#define OSCCTRL_STATUS_DPLL0LCKR_Pos 16           /**< \brief (OSCCTRL_STATUS) DPLL0 Lock Rise */
#define OSCCTRL_STATUS_DPLL0LCKR    (_U_(0x1) << OSCCTRL_STATUS_DPLL0LCKR_Pos)
#define OSCCTRL_STATUS_DPLL0LCKF_Pos 17           /**< \brief (OSCCTRL_STATUS) DPLL0 Lock Fall */
#define OSCCTRL_STATUS_DPLL0LCKF    (_U_(0x1) << OSCCTRL_STATUS_DPLL0LCKF_Pos)
#define OSCCTRL_STATUS_DPLL0TO_Pos  18           /**< \brief (OSCCTRL_STATUS) DPLL0 Timeout */
#define OSCCTRL_STATUS_DPLL0TO      (_U_(0x1) << OSCCTRL_STATUS_DPLL0TO_Pos)
#define OSCCTRL_STATUS_DPLL0LDRTO_Pos 19           /**< \brief (OSCCTRL_STATUS) DPLL0 Loop Divider Ratio Update Complete */
#define OSCCTRL_STATUS_DPLL0LDRTO   (_U_(0x1) << OSCCTRL_STATUS_DPLL0LDRTO_Pos)
#define OSCCTRL_STATUS_DPLL1LCKR_Pos 24           /**< \brief (OSCCTRL_STATUS) DPLL1 Lock Rise */
#define OSCCTRL_STATUS_DPLL1LCKR    (_U_(0x1) << OSCCTRL_STATUS_DPLL1LCKR_Pos)
#define OSCCTRL_STATUS_DPLL1LCKF_Pos 25           /**< \brief (OSCCTRL_STATUS) DPLL1 Lock Fall */
#define OSCCTRL_STATUS_DPLL1LCKF    (_U_(0x1) << OSCCTRL_STATUS_DPLL1LCKF_Pos)
#define OSCCTRL_STATUS_DPLL1TO_Pos  26           /**< \brief (OSCCTRL_STATUS) DPLL1 Timeout */
#define OSCCTRL_STATUS_DPLL1TO      (_U_(0x1) << OSCCTRL_STATUS_DPLL1TO_Pos)
#define OSCCTRL_STATUS_DPLL1LDRTO_Pos 27           /**< \brief (OSCCTRL_STATUS) DPLL1 Loop Divider Ratio Update Complete */
#define OSCCTRL_STATUS_DPLL1LDRTO   (_U_(0x1) << OSCCTRL_STATUS_DPLL1LDRTO_Pos)
#define OSCCTRL_STATUS_MASK         _U_(0x0F0F1F3F) /**< \brief (OSCCTRL_STATUS) MASK Register */

/* -------- OSCCTRL_XOSCCTRL : (OSCCTRL Offset: 0x14) (R/W 32) External Multipurpose Crystal Oscillator Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t ENABLE:1;         /*!< bit:      1  Oscillator Enable                  */
    uint32_t XTALEN:1;         /*!< bit:      2  Crystal Oscillator Enable          */
    uint32_t :3;               /*!< bit:  3.. 5  Reserved                           */
    uint32_t RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint32_t ONDEMAND:1;       /*!< bit:      7  On Demand Control                  */
    uint32_t LOWBUFGAIN:1;     /*!< bit:      8  Low Buffer Gain Enable             */
    uint32_t IPTAT:2;          /*!< bit:  9..10  Oscillator Current Reference       */
    uint32_t IMULT:4;          /*!< bit: 11..14  Oscillator Current Multiplier      */
    uint32_t ENALC:1;          /*!< bit:     15  Automatic Loop Control Enable      */
    uint32_t CFDEN:1;          /*!< bit:     16  Clock Failure Detector Enable      */
    uint32_t SWBEN:1;          /*!< bit:     17  Xosc Clock Switch Enable           */
    uint32_t :2;               /*!< bit: 18..19  Reserved                           */
    uint32_t STARTUP:4;        /*!< bit: 20..23  Start-Up Time                      */
    uint32_t CFDPRESC:4;       /*!< bit: 24..27  Clock Failure Detector Prescaler   */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OSCCTRL_XOSCCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSCCTRL_XOSCCTRL_OFFSET     0x14         /**< \brief (OSCCTRL_XOSCCTRL offset) External Multipurpose Crystal Oscillator Control */
#define OSCCTRL_XOSCCTRL_RESETVALUE _U_(0x00000080) /**< \brief (OSCCTRL_XOSCCTRL reset_value) External Multipurpose Crystal Oscillator Control */

#define OSCCTRL_XOSCCTRL_ENABLE_Pos 1            /**< \brief (OSCCTRL_XOSCCTRL) Oscillator Enable */
#define OSCCTRL_XOSCCTRL_ENABLE     (_U_(0x1) << OSCCTRL_XOSCCTRL_ENABLE_Pos)
#define OSCCTRL_XOSCCTRL_XTALEN_Pos 2            /**< \brief (OSCCTRL_XOSCCTRL) Crystal Oscillator Enable */
#define OSCCTRL_XOSCCTRL_XTALEN     (_U_(0x1) << OSCCTRL_XOSCCTRL_XTALEN_Pos)
#define OSCCTRL_XOSCCTRL_RUNSTDBY_Pos 6            /**< \brief (OSCCTRL_XOSCCTRL) Run in Standby */
#define OSCCTRL_XOSCCTRL_RUNSTDBY   (_U_(0x1) << OSCCTRL_XOSCCTRL_RUNSTDBY_Pos)
#define OSCCTRL_XOSCCTRL_ONDEMAND_Pos 7            /**< \brief (OSCCTRL_XOSCCTRL) On Demand Control */
#define OSCCTRL_XOSCCTRL_ONDEMAND   (_U_(0x1) << OSCCTRL_XOSCCTRL_ONDEMAND_Pos)
#define OSCCTRL_XOSCCTRL_LOWBUFGAIN_Pos 8            /**< \brief (OSCCTRL_XOSCCTRL) Low Buffer Gain Enable */
#define OSCCTRL_XOSCCTRL_LOWBUFGAIN (_U_(0x1) << OSCCTRL_XOSCCTRL_LOWBUFGAIN_Pos)
#define OSCCTRL_XOSCCTRL_IPTAT_Pos  9            /**< \brief (OSCCTRL_XOSCCTRL) Oscillator Current Reference */
#define OSCCTRL_XOSCCTRL_IPTAT_Msk  (_U_(0x3) << OSCCTRL_XOSCCTRL_IPTAT_Pos)
#define OSCCTRL_XOSCCTRL_IPTAT(value) (OSCCTRL_XOSCCTRL_IPTAT_Msk & ((value) << OSCCTRL_XOSCCTRL_IPTAT_Pos))
#define OSCCTRL_XOSCCTRL_IMULT_Pos  11           /**< \brief (OSCCTRL_XOSCCTRL) Oscillator Current Multiplier */
#define OSCCTRL_XOSCCTRL_IMULT_Msk  (_U_(0xF) << OSCCTRL_XOSCCTRL_IMULT_Pos)
#define OSCCTRL_XOSCCTRL_IMULT(value) (OSCCTRL_XOSCCTRL_IMULT_Msk & ((value) << OSCCTRL_XOSCCTRL_IMULT_Pos))
#define OSCCTRL_XOSCCTRL_ENALC_Pos  15           /**< \brief (OSCCTRL_XOSCCTRL) Automatic Loop Control Enable */
#define OSCCTRL_XOSCCTRL_ENALC      (_U_(0x1) << OSCCTRL_XOSCCTRL_ENALC_Pos)
#define OSCCTRL_XOSCCTRL_CFDEN_Pos  16           /**< \brief (OSCCTRL_XOSCCTRL) Clock Failure Detector Enable */
#define OSCCTRL_XOSCCTRL_CFDEN      (_U_(0x1) << OSCCTRL_XOSCCTRL_CFDEN_Pos)
#define OSCCTRL_XOSCCTRL_SWBEN_Pos  17           /**< \brief (OSCCTRL_XOSCCTRL) Xosc Clock Switch Enable */
#define OSCCTRL_XOSCCTRL_SWBEN      (_U_(0x1) << OSCCTRL_XOSCCTRL_SWBEN_Pos)
#define OSCCTRL_XOSCCTRL_STARTUP_Pos 20           /**< \brief (OSCCTRL_XOSCCTRL) Start-Up Time */
#define OSCCTRL_XOSCCTRL_STARTUP_Msk (_U_(0xF) << OSCCTRL_XOSCCTRL_STARTUP_Pos)
#define OSCCTRL_XOSCCTRL_STARTUP(value) (OSCCTRL_XOSCCTRL_STARTUP_Msk & ((value) << OSCCTRL_XOSCCTRL_STARTUP_Pos))
#define OSCCTRL_XOSCCTRL_CFDPRESC_Pos 24           /**< \brief (OSCCTRL_XOSCCTRL) Clock Failure Detector Prescaler */
#define OSCCTRL_XOSCCTRL_CFDPRESC_Msk (_U_(0xF) << OSCCTRL_XOSCCTRL_CFDPRESC_Pos)
#define OSCCTRL_XOSCCTRL_CFDPRESC(value) (OSCCTRL_XOSCCTRL_CFDPRESC_Msk & ((value) << OSCCTRL_XOSCCTRL_CFDPRESC_Pos))
#define OSCCTRL_XOSCCTRL_MASK       _U_(0x0FF3FFC6) /**< \brief (OSCCTRL_XOSCCTRL) MASK Register */

/* -------- OSCCTRL_DFLLCTRLA : (OSCCTRL Offset: 0x1C) (R/W  8) DFLL48M Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :1;               /*!< bit:      0  Reserved                           */
    uint8_t  ENABLE:1;         /*!< bit:      1  DFLL Enable                        */
    uint8_t  :4;               /*!< bit:  2.. 5  Reserved                           */
    uint8_t  RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint8_t  ONDEMAND:1;       /*!< bit:      7  On Demand Control                  */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} OSCCTRL_DFLLCTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSCCTRL_DFLLCTRLA_OFFSET    0x1C         /**< \brief (OSCCTRL_DFLLCTRLA offset) DFLL48M Control A */
#define OSCCTRL_DFLLCTRLA_RESETVALUE _U_(0x82)    /**< \brief (OSCCTRL_DFLLCTRLA reset_value) DFLL48M Control A */

#define OSCCTRL_DFLLCTRLA_ENABLE_Pos 1            /**< \brief (OSCCTRL_DFLLCTRLA) DFLL Enable */
#define OSCCTRL_DFLLCTRLA_ENABLE    (_U_(0x1) << OSCCTRL_DFLLCTRLA_ENABLE_Pos)
#define OSCCTRL_DFLLCTRLA_RUNSTDBY_Pos 6            /**< \brief (OSCCTRL_DFLLCTRLA) Run in Standby */
#define OSCCTRL_DFLLCTRLA_RUNSTDBY  (_U_(0x1) << OSCCTRL_DFLLCTRLA_RUNSTDBY_Pos)
#define OSCCTRL_DFLLCTRLA_ONDEMAND_Pos 7            /**< \brief (OSCCTRL_DFLLCTRLA) On Demand Control */
#define OSCCTRL_DFLLCTRLA_ONDEMAND  (_U_(0x1) << OSCCTRL_DFLLCTRLA_ONDEMAND_Pos)
#define OSCCTRL_DFLLCTRLA_MASK      _U_(0xC2)    /**< \brief (OSCCTRL_DFLLCTRLA) MASK Register */

/* -------- OSCCTRL_DFLLCTRLB : (OSCCTRL Offset: 0x20) (R/W  8) DFLL48M Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  MODE:1;           /*!< bit:      0  Operating Mode Selection           */
    uint8_t  STABLE:1;         /*!< bit:      1  Stable DFLL Frequency              */
    uint8_t  LLAW:1;           /*!< bit:      2  Lose Lock After Wake               */
    uint8_t  USBCRM:1;         /*!< bit:      3  USB Clock Recovery Mode            */
    uint8_t  CCDIS:1;          /*!< bit:      4  Chill Cycle Disable                */
    uint8_t  QLDIS:1;          /*!< bit:      5  Quick Lock Disable                 */
    uint8_t  BPLCKC:1;         /*!< bit:      6  Bypass Coarse Lock                 */
    uint8_t  WAITLOCK:1;       /*!< bit:      7  Wait Lock                          */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} OSCCTRL_DFLLCTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSCCTRL_DFLLCTRLB_OFFSET    0x20         /**< \brief (OSCCTRL_DFLLCTRLB offset) DFLL48M Control B */
#define OSCCTRL_DFLLCTRLB_RESETVALUE _U_(0x00)    /**< \brief (OSCCTRL_DFLLCTRLB reset_value) DFLL48M Control B */

#define OSCCTRL_DFLLCTRLB_MODE_Pos  0            /**< \brief (OSCCTRL_DFLLCTRLB) Operating Mode Selection */
#define OSCCTRL_DFLLCTRLB_MODE      (_U_(0x1) << OSCCTRL_DFLLCTRLB_MODE_Pos)
#define OSCCTRL_DFLLCTRLB_STABLE_Pos 1            /**< \brief (OSCCTRL_DFLLCTRLB) Stable DFLL Frequency */
#define OSCCTRL_DFLLCTRLB_STABLE    (_U_(0x1) << OSCCTRL_DFLLCTRLB_STABLE_Pos)
#define OSCCTRL_DFLLCTRLB_LLAW_Pos  2            /**< \brief (OSCCTRL_DFLLCTRLB) Lose Lock After Wake */
#define OSCCTRL_DFLLCTRLB_LLAW      (_U_(0x1) << OSCCTRL_DFLLCTRLB_LLAW_Pos)
#define OSCCTRL_DFLLCTRLB_USBCRM_Pos 3            /**< \brief (OSCCTRL_DFLLCTRLB) USB Clock Recovery Mode */
#define OSCCTRL_DFLLCTRLB_USBCRM    (_U_(0x1) << OSCCTRL_DFLLCTRLB_USBCRM_Pos)
#define OSCCTRL_DFLLCTRLB_CCDIS_Pos 4            /**< \brief (OSCCTRL_DFLLCTRLB) Chill Cycle Disable */
#define OSCCTRL_DFLLCTRLB_CCDIS     (_U_(0x1) << OSCCTRL_DFLLCTRLB_CCDIS_Pos)
#define OSCCTRL_DFLLCTRLB_QLDIS_Pos 5            /**< \brief (OSCCTRL_DFLLCTRLB) Quick Lock Disable */
#define OSCCTRL_DFLLCTRLB_QLDIS     (_U_(0x1) << OSCCTRL_DFLLCTRLB_QLDIS_Pos)
#define OSCCTRL_DFLLCTRLB_BPLCKC_Pos 6            /**< \brief (OSCCTRL_DFLLCTRLB) Bypass Coarse Lock */
#define OSCCTRL_DFLLCTRLB_BPLCKC    (_U_(0x1) << OSCCTRL_DFLLCTRLB_BPLCKC_Pos)
#define OSCCTRL_DFLLCTRLB_WAITLOCK_Pos 7            /**< \brief (OSCCTRL_DFLLCTRLB) Wait Lock */
#define OSCCTRL_DFLLCTRLB_WAITLOCK  (_U_(0x1) << OSCCTRL_DFLLCTRLB_WAITLOCK_Pos)
#define OSCCTRL_DFLLCTRLB_MASK      _U_(0xFF)    /**< \brief (OSCCTRL_DFLLCTRLB) MASK Register */

/* -------- OSCCTRL_DFLLVAL : (OSCCTRL Offset: 0x24) (R/W 32) DFLL48M Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t FINE:8;           /*!< bit:  0.. 7  Fine Value                         */
    uint32_t :2;               /*!< bit:  8.. 9  Reserved                           */
    uint32_t COARSE:6;         /*!< bit: 10..15  Coarse Value                       */
    uint32_t DIFF:16;          /*!< bit: 16..31  Multiplication Ratio Difference    */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OSCCTRL_DFLLVAL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSCCTRL_DFLLVAL_OFFSET      0x24         /**< \brief (OSCCTRL_DFLLVAL offset) DFLL48M Value */
#define OSCCTRL_DFLLVAL_RESETVALUE  _U_(0x00000000) /**< \brief (OSCCTRL_DFLLVAL reset_value) DFLL48M Value */

#define OSCCTRL_DFLLVAL_FINE_Pos    0            /**< \brief (OSCCTRL_DFLLVAL) Fine Value */
#define OSCCTRL_DFLLVAL_FINE_Msk    (_U_(0xFF) << OSCCTRL_DFLLVAL_FINE_Pos)
#define OSCCTRL_DFLLVAL_FINE(value) (OSCCTRL_DFLLVAL_FINE_Msk & ((value) << OSCCTRL_DFLLVAL_FINE_Pos))
#define OSCCTRL_DFLLVAL_COARSE_Pos  10           /**< \brief (OSCCTRL_DFLLVAL) Coarse Value */
#define OSCCTRL_DFLLVAL_COARSE_Msk  (_U_(0x3F) << OSCCTRL_DFLLVAL_COARSE_Pos)
#define OSCCTRL_DFLLVAL_COARSE(value) (OSCCTRL_DFLLVAL_COARSE_Msk & ((value) << OSCCTRL_DFLLVAL_COARSE_Pos))
#define OSCCTRL_DFLLVAL_DIFF_Pos    16           /**< \brief (OSCCTRL_DFLLVAL) Multiplication Ratio Difference */
#define OSCCTRL_DFLLVAL_DIFF_Msk    (_U_(0xFFFF) << OSCCTRL_DFLLVAL_DIFF_Pos)
#define OSCCTRL_DFLLVAL_DIFF(value) (OSCCTRL_DFLLVAL_DIFF_Msk & ((value) << OSCCTRL_DFLLVAL_DIFF_Pos))
#define OSCCTRL_DFLLVAL_MASK        _U_(0xFFFFFCFF) /**< \brief (OSCCTRL_DFLLVAL) MASK Register */

/* -------- OSCCTRL_DFLLMUL : (OSCCTRL Offset: 0x28) (R/W 32) DFLL48M Multiplier -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t MUL:16;           /*!< bit:  0..15  DFLL Multiply Factor               */
    uint32_t FSTEP:8;          /*!< bit: 16..23  Fine Maximum Step                  */
    uint32_t :2;               /*!< bit: 24..25  Reserved                           */
    uint32_t CSTEP:6;          /*!< bit: 26..31  Coarse Maximum Step                */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OSCCTRL_DFLLMUL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSCCTRL_DFLLMUL_OFFSET      0x28         /**< \brief (OSCCTRL_DFLLMUL offset) DFLL48M Multiplier */
#define OSCCTRL_DFLLMUL_RESETVALUE  _U_(0x00000000) /**< \brief (OSCCTRL_DFLLMUL reset_value) DFLL48M Multiplier */

#define OSCCTRL_DFLLMUL_MUL_Pos     0            /**< \brief (OSCCTRL_DFLLMUL) DFLL Multiply Factor */
#define OSCCTRL_DFLLMUL_MUL_Msk     (_U_(0xFFFF) << OSCCTRL_DFLLMUL_MUL_Pos)
#define OSCCTRL_DFLLMUL_MUL(value)  (OSCCTRL_DFLLMUL_MUL_Msk & ((value) << OSCCTRL_DFLLMUL_MUL_Pos))
#define OSCCTRL_DFLLMUL_FSTEP_Pos   16           /**< \brief (OSCCTRL_DFLLMUL) Fine Maximum Step */
#define OSCCTRL_DFLLMUL_FSTEP_Msk   (_U_(0xFF) << OSCCTRL_DFLLMUL_FSTEP_Pos)
#define OSCCTRL_DFLLMUL_FSTEP(value) (OSCCTRL_DFLLMUL_FSTEP_Msk & ((value) << OSCCTRL_DFLLMUL_FSTEP_Pos))
#define OSCCTRL_DFLLMUL_CSTEP_Pos   26           /**< \brief (OSCCTRL_DFLLMUL) Coarse Maximum Step */
#define OSCCTRL_DFLLMUL_CSTEP_Msk   (_U_(0x3F) << OSCCTRL_DFLLMUL_CSTEP_Pos)
#define OSCCTRL_DFLLMUL_CSTEP(value) (OSCCTRL_DFLLMUL_CSTEP_Msk & ((value) << OSCCTRL_DFLLMUL_CSTEP_Pos))
#define OSCCTRL_DFLLMUL_MASK        _U_(0xFCFFFFFF) /**< \brief (OSCCTRL_DFLLMUL) MASK Register */

/* -------- OSCCTRL_DFLLSYNC : (OSCCTRL Offset: 0x2C) (R/W  8) DFLL48M Synchronization -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :1;               /*!< bit:      0  Reserved                           */
    uint8_t  ENABLE:1;         /*!< bit:      1  ENABLE Synchronization Busy        */
    uint8_t  DFLLCTRLB:1;      /*!< bit:      2  DFLLCTRLB Synchronization Busy     */
    uint8_t  DFLLVAL:1;        /*!< bit:      3  DFLLVAL Synchronization Busy       */
    uint8_t  DFLLMUL:1;        /*!< bit:      4  DFLLMUL Synchronization Busy       */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} OSCCTRL_DFLLSYNC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSCCTRL_DFLLSYNC_OFFSET     0x2C         /**< \brief (OSCCTRL_DFLLSYNC offset) DFLL48M Synchronization */
#define OSCCTRL_DFLLSYNC_RESETVALUE _U_(0x00)    /**< \brief (OSCCTRL_DFLLSYNC reset_value) DFLL48M Synchronization */

#define OSCCTRL_DFLLSYNC_ENABLE_Pos 1            /**< \brief (OSCCTRL_DFLLSYNC) ENABLE Synchronization Busy */
#define OSCCTRL_DFLLSYNC_ENABLE     (_U_(0x1) << OSCCTRL_DFLLSYNC_ENABLE_Pos)
#define OSCCTRL_DFLLSYNC_DFLLCTRLB_Pos 2            /**< \brief (OSCCTRL_DFLLSYNC) DFLLCTRLB Synchronization Busy */
#define OSCCTRL_DFLLSYNC_DFLLCTRLB  (_U_(0x1) << OSCCTRL_DFLLSYNC_DFLLCTRLB_Pos)
#define OSCCTRL_DFLLSYNC_DFLLVAL_Pos 3            /**< \brief (OSCCTRL_DFLLSYNC) DFLLVAL Synchronization Busy */
#define OSCCTRL_DFLLSYNC_DFLLVAL    (_U_(0x1) << OSCCTRL_DFLLSYNC_DFLLVAL_Pos)
#define OSCCTRL_DFLLSYNC_DFLLMUL_Pos 4            /**< \brief (OSCCTRL_DFLLSYNC) DFLLMUL Synchronization Busy */
#define OSCCTRL_DFLLSYNC_DFLLMUL    (_U_(0x1) << OSCCTRL_DFLLSYNC_DFLLMUL_Pos)
#define OSCCTRL_DFLLSYNC_MASK       _U_(0x1E)    /**< \brief (OSCCTRL_DFLLSYNC) MASK Register */

/* -------- OSCCTRL_DPLLCTRLA : (OSCCTRL Offset: 0x30) (R/W  8) DPLL DPLL Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :1;               /*!< bit:      0  Reserved                           */
    uint8_t  ENABLE:1;         /*!< bit:      1  DPLL Enable                        */
    uint8_t  :4;               /*!< bit:  2.. 5  Reserved                           */
    uint8_t  RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint8_t  ONDEMAND:1;       /*!< bit:      7  On Demand Control                  */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} OSCCTRL_DPLLCTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSCCTRL_DPLLCTRLA_OFFSET    0x30         /**< \brief (OSCCTRL_DPLLCTRLA offset) DPLL Control A */
#define OSCCTRL_DPLLCTRLA_RESETVALUE _U_(0x80)    /**< \brief (OSCCTRL_DPLLCTRLA reset_value) DPLL Control A */

#define OSCCTRL_DPLLCTRLA_ENABLE_Pos 1            /**< \brief (OSCCTRL_DPLLCTRLA) DPLL Enable */
#define OSCCTRL_DPLLCTRLA_ENABLE    (_U_(0x1) << OSCCTRL_DPLLCTRLA_ENABLE_Pos)
#define OSCCTRL_DPLLCTRLA_RUNSTDBY_Pos 6            /**< \brief (OSCCTRL_DPLLCTRLA) Run in Standby */
#define OSCCTRL_DPLLCTRLA_RUNSTDBY  (_U_(0x1) << OSCCTRL_DPLLCTRLA_RUNSTDBY_Pos)
#define OSCCTRL_DPLLCTRLA_ONDEMAND_Pos 7            /**< \brief (OSCCTRL_DPLLCTRLA) On Demand Control */
#define OSCCTRL_DPLLCTRLA_ONDEMAND  (_U_(0x1) << OSCCTRL_DPLLCTRLA_ONDEMAND_Pos)
#define OSCCTRL_DPLLCTRLA_MASK      _U_(0xC2)    /**< \brief (OSCCTRL_DPLLCTRLA) MASK Register */

/* -------- OSCCTRL_DPLLRATIO : (OSCCTRL Offset: 0x34) (R/W 32) DPLL DPLL Ratio Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t LDR:13;           /*!< bit:  0..12  Loop Divider Ratio                 */
    uint32_t :3;               /*!< bit: 13..15  Reserved                           */
    uint32_t LDRFRAC:5;        /*!< bit: 16..20  Loop Divider Ratio Fractional Part */
    uint32_t :11;              /*!< bit: 21..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OSCCTRL_DPLLRATIO_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSCCTRL_DPLLRATIO_OFFSET    0x34         /**< \brief (OSCCTRL_DPLLRATIO offset) DPLL Ratio Control */
#define OSCCTRL_DPLLRATIO_RESETVALUE _U_(0x00000000) /**< \brief (OSCCTRL_DPLLRATIO reset_value) DPLL Ratio Control */

#define OSCCTRL_DPLLRATIO_LDR_Pos   0            /**< \brief (OSCCTRL_DPLLRATIO) Loop Divider Ratio */
#define OSCCTRL_DPLLRATIO_LDR_Msk   (_U_(0x1FFF) << OSCCTRL_DPLLRATIO_LDR_Pos)
#define OSCCTRL_DPLLRATIO_LDR(value) (OSCCTRL_DPLLRATIO_LDR_Msk & ((value) << OSCCTRL_DPLLRATIO_LDR_Pos))
#define OSCCTRL_DPLLRATIO_LDRFRAC_Pos 16           /**< \brief (OSCCTRL_DPLLRATIO) Loop Divider Ratio Fractional Part */
#define OSCCTRL_DPLLRATIO_LDRFRAC_Msk (_U_(0x1F) << OSCCTRL_DPLLRATIO_LDRFRAC_Pos)
#define OSCCTRL_DPLLRATIO_LDRFRAC(value) (OSCCTRL_DPLLRATIO_LDRFRAC_Msk & ((value) << OSCCTRL_DPLLRATIO_LDRFRAC_Pos))
#define OSCCTRL_DPLLRATIO_MASK      _U_(0x001F1FFF) /**< \brief (OSCCTRL_DPLLRATIO) MASK Register */

/* -------- OSCCTRL_DPLLCTRLB : (OSCCTRL Offset: 0x38) (R/W 32) DPLL DPLL Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t FILTER:4;         /*!< bit:  0.. 3  Proportional Integral Filter Selection */
    uint32_t WUF:1;            /*!< bit:      4  Wake Up Fast                       */
    uint32_t REFCLK:3;         /*!< bit:  5.. 7  Reference Clock Selection          */
    uint32_t LTIME:3;          /*!< bit:  8..10  Lock Time                          */
    uint32_t LBYPASS:1;        /*!< bit:     11  Lock Bypass                        */
    uint32_t DCOFILTER:3;      /*!< bit: 12..14  Sigma-Delta DCO Filter Selection   */
    uint32_t DCOEN:1;          /*!< bit:     15  DCO Filter Enable                  */
    uint32_t DIV:11;           /*!< bit: 16..26  Clock Divider                      */
    uint32_t :5;               /*!< bit: 27..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OSCCTRL_DPLLCTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSCCTRL_DPLLCTRLB_OFFSET    0x38         /**< \brief (OSCCTRL_DPLLCTRLB offset) DPLL Control B */
#define OSCCTRL_DPLLCTRLB_RESETVALUE _U_(0x00000020) /**< \brief (OSCCTRL_DPLLCTRLB reset_value) DPLL Control B */

#define OSCCTRL_DPLLCTRLB_FILTER_Pos 0            /**< \brief (OSCCTRL_DPLLCTRLB) Proportional Integral Filter Selection */
#define OSCCTRL_DPLLCTRLB_FILTER_Msk (_U_(0xF) << OSCCTRL_DPLLCTRLB_FILTER_Pos)
#define OSCCTRL_DPLLCTRLB_FILTER(value) (OSCCTRL_DPLLCTRLB_FILTER_Msk & ((value) << OSCCTRL_DPLLCTRLB_FILTER_Pos))
#define OSCCTRL_DPLLCTRLB_WUF_Pos   4            /**< \brief (OSCCTRL_DPLLCTRLB) Wake Up Fast */
#define OSCCTRL_DPLLCTRLB_WUF       (_U_(0x1) << OSCCTRL_DPLLCTRLB_WUF_Pos)
#define OSCCTRL_DPLLCTRLB_REFCLK_Pos 5            /**< \brief (OSCCTRL_DPLLCTRLB) Reference Clock Selection */
#define OSCCTRL_DPLLCTRLB_REFCLK_Msk (_U_(0x7) << OSCCTRL_DPLLCTRLB_REFCLK_Pos)
#define OSCCTRL_DPLLCTRLB_REFCLK(value) (OSCCTRL_DPLLCTRLB_REFCLK_Msk & ((value) << OSCCTRL_DPLLCTRLB_REFCLK_Pos))
#define   OSCCTRL_DPLLCTRLB_REFCLK_GCLK_Val _U_(0x0)   /**< \brief (OSCCTRL_DPLLCTRLB) Dedicated GCLK clock reference */
#define   OSCCTRL_DPLLCTRLB_REFCLK_XOSC32_Val _U_(0x1)   /**< \brief (OSCCTRL_DPLLCTRLB) XOSC32K clock reference */
#define   OSCCTRL_DPLLCTRLB_REFCLK_XOSC0_Val _U_(0x2)   /**< \brief (OSCCTRL_DPLLCTRLB) XOSC0 clock reference */
#define   OSCCTRL_DPLLCTRLB_REFCLK_XOSC1_Val _U_(0x3)   /**< \brief (OSCCTRL_DPLLCTRLB) XOSC1 clock reference */
#define OSCCTRL_DPLLCTRLB_REFCLK_GCLK (OSCCTRL_DPLLCTRLB_REFCLK_GCLK_Val << OSCCTRL_DPLLCTRLB_REFCLK_Pos)
#define OSCCTRL_DPLLCTRLB_REFCLK_XOSC32 (OSCCTRL_DPLLCTRLB_REFCLK_XOSC32_Val << OSCCTRL_DPLLCTRLB_REFCLK_Pos)
#define OSCCTRL_DPLLCTRLB_REFCLK_XOSC0 (OSCCTRL_DPLLCTRLB_REFCLK_XOSC0_Val << OSCCTRL_DPLLCTRLB_REFCLK_Pos)
#define OSCCTRL_DPLLCTRLB_REFCLK_XOSC1 (OSCCTRL_DPLLCTRLB_REFCLK_XOSC1_Val << OSCCTRL_DPLLCTRLB_REFCLK_Pos)
#define OSCCTRL_DPLLCTRLB_LTIME_Pos 8            /**< \brief (OSCCTRL_DPLLCTRLB) Lock Time */
#define OSCCTRL_DPLLCTRLB_LTIME_Msk (_U_(0x7) << OSCCTRL_DPLLCTRLB_LTIME_Pos)
#define OSCCTRL_DPLLCTRLB_LTIME(value) (OSCCTRL_DPLLCTRLB_LTIME_Msk & ((value) << OSCCTRL_DPLLCTRLB_LTIME_Pos))
#define   OSCCTRL_DPLLCTRLB_LTIME_DEFAULT_Val _U_(0x0)   /**< \brief (OSCCTRL_DPLLCTRLB) No time-out. Automatic lock */
#define   OSCCTRL_DPLLCTRLB_LTIME_800US_Val _U_(0x4)   /**< \brief (OSCCTRL_DPLLCTRLB) Time-out if no lock within 800us */
#define   OSCCTRL_DPLLCTRLB_LTIME_900US_Val _U_(0x5)   /**< \brief (OSCCTRL_DPLLCTRLB) Time-out if no lock within 900us */
#define   OSCCTRL_DPLLCTRLB_LTIME_1MS_Val _U_(0x6)   /**< \brief (OSCCTRL_DPLLCTRLB) Time-out if no lock within 1ms */
#define   OSCCTRL_DPLLCTRLB_LTIME_1P1MS_Val _U_(0x7)   /**< \brief (OSCCTRL_DPLLCTRLB) Time-out if no lock within 1.1ms */
#define OSCCTRL_DPLLCTRLB_LTIME_DEFAULT (OSCCTRL_DPLLCTRLB_LTIME_DEFAULT_Val << OSCCTRL_DPLLCTRLB_LTIME_Pos)
#define OSCCTRL_DPLLCTRLB_LTIME_800US (OSCCTRL_DPLLCTRLB_LTIME_800US_Val << OSCCTRL_DPLLCTRLB_LTIME_Pos)
#define OSCCTRL_DPLLCTRLB_LTIME_900US (OSCCTRL_DPLLCTRLB_LTIME_900US_Val << OSCCTRL_DPLLCTRLB_LTIME_Pos)
#define OSCCTRL_DPLLCTRLB_LTIME_1MS (OSCCTRL_DPLLCTRLB_LTIME_1MS_Val << OSCCTRL_DPLLCTRLB_LTIME_Pos)
#define OSCCTRL_DPLLCTRLB_LTIME_1P1MS (OSCCTRL_DPLLCTRLB_LTIME_1P1MS_Val << OSCCTRL_DPLLCTRLB_LTIME_Pos)
#define OSCCTRL_DPLLCTRLB_LBYPASS_Pos 11           /**< \brief (OSCCTRL_DPLLCTRLB) Lock Bypass */
#define OSCCTRL_DPLLCTRLB_LBYPASS   (_U_(0x1) << OSCCTRL_DPLLCTRLB_LBYPASS_Pos)
#define OSCCTRL_DPLLCTRLB_DCOFILTER_Pos 12           /**< \brief (OSCCTRL_DPLLCTRLB) Sigma-Delta DCO Filter Selection */
#define OSCCTRL_DPLLCTRLB_DCOFILTER_Msk (_U_(0x7) << OSCCTRL_DPLLCTRLB_DCOFILTER_Pos)
#define OSCCTRL_DPLLCTRLB_DCOFILTER(value) (OSCCTRL_DPLLCTRLB_DCOFILTER_Msk & ((value) << OSCCTRL_DPLLCTRLB_DCOFILTER_Pos))
#define OSCCTRL_DPLLCTRLB_DCOEN_Pos 15           /**< \brief (OSCCTRL_DPLLCTRLB) DCO Filter Enable */
#define OSCCTRL_DPLLCTRLB_DCOEN     (_U_(0x1) << OSCCTRL_DPLLCTRLB_DCOEN_Pos)
#define OSCCTRL_DPLLCTRLB_DIV_Pos   16           /**< \brief (OSCCTRL_DPLLCTRLB) Clock Divider */
#define OSCCTRL_DPLLCTRLB_DIV_Msk   (_U_(0x7FF) << OSCCTRL_DPLLCTRLB_DIV_Pos)
#define OSCCTRL_DPLLCTRLB_DIV(value) (OSCCTRL_DPLLCTRLB_DIV_Msk & ((value) << OSCCTRL_DPLLCTRLB_DIV_Pos))
#define OSCCTRL_DPLLCTRLB_MASK      _U_(0x07FFFFFF) /**< \brief (OSCCTRL_DPLLCTRLB) MASK Register */

/* -------- OSCCTRL_DPLLSYNCBUSY : (OSCCTRL Offset: 0x3C) (R/  32) DPLL DPLL Synchronization Busy -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t ENABLE:1;         /*!< bit:      1  DPLL Enable Synchronization Status */
    uint32_t DPLLRATIO:1;      /*!< bit:      2  DPLL Loop Divider Ratio Synchronization Status */
    uint32_t :29;              /*!< bit:  3..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OSCCTRL_DPLLSYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSCCTRL_DPLLSYNCBUSY_OFFSET 0x3C         /**< \brief (OSCCTRL_DPLLSYNCBUSY offset) DPLL Synchronization Busy */
#define OSCCTRL_DPLLSYNCBUSY_RESETVALUE _U_(0x00000000) /**< \brief (OSCCTRL_DPLLSYNCBUSY reset_value) DPLL Synchronization Busy */

#define OSCCTRL_DPLLSYNCBUSY_ENABLE_Pos 1            /**< \brief (OSCCTRL_DPLLSYNCBUSY) DPLL Enable Synchronization Status */
#define OSCCTRL_DPLLSYNCBUSY_ENABLE (_U_(0x1) << OSCCTRL_DPLLSYNCBUSY_ENABLE_Pos)
#define OSCCTRL_DPLLSYNCBUSY_DPLLRATIO_Pos 2            /**< \brief (OSCCTRL_DPLLSYNCBUSY) DPLL Loop Divider Ratio Synchronization Status */
#define OSCCTRL_DPLLSYNCBUSY_DPLLRATIO (_U_(0x1) << OSCCTRL_DPLLSYNCBUSY_DPLLRATIO_Pos)
#define OSCCTRL_DPLLSYNCBUSY_MASK   _U_(0x00000006) /**< \brief (OSCCTRL_DPLLSYNCBUSY) MASK Register */

/* -------- OSCCTRL_DPLLSTATUS : (OSCCTRL Offset: 0x40) (R/  32) DPLL DPLL Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t LOCK:1;           /*!< bit:      0  DPLL Lock Status                   */
    uint32_t CLKRDY:1;         /*!< bit:      1  DPLL Clock Ready                   */
    uint32_t :30;              /*!< bit:  2..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OSCCTRL_DPLLSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSCCTRL_DPLLSTATUS_OFFSET   0x40         /**< \brief (OSCCTRL_DPLLSTATUS offset) DPLL Status */
#define OSCCTRL_DPLLSTATUS_RESETVALUE _U_(0x00000000) /**< \brief (OSCCTRL_DPLLSTATUS reset_value) DPLL Status */

#define OSCCTRL_DPLLSTATUS_LOCK_Pos 0            /**< \brief (OSCCTRL_DPLLSTATUS) DPLL Lock Status */
#define OSCCTRL_DPLLSTATUS_LOCK     (_U_(0x1) << OSCCTRL_DPLLSTATUS_LOCK_Pos)
#define OSCCTRL_DPLLSTATUS_CLKRDY_Pos 1            /**< \brief (OSCCTRL_DPLLSTATUS) DPLL Clock Ready */
#define OSCCTRL_DPLLSTATUS_CLKRDY   (_U_(0x1) << OSCCTRL_DPLLSTATUS_CLKRDY_Pos)
#define OSCCTRL_DPLLSTATUS_MASK     _U_(0x00000003) /**< \brief (OSCCTRL_DPLLSTATUS) MASK Register */

/** \brief OscctrlDpll hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO OSCCTRL_DPLLCTRLA_Type    DPLLCTRLA;   /**< \brief Offset: 0x00 (R/W  8) DPLL Control A */
       RoReg8                    Reserved1[0x3];
  __IO OSCCTRL_DPLLRATIO_Type    DPLLRATIO;   /**< \brief Offset: 0x04 (R/W 32) DPLL Ratio Control */
  __IO OSCCTRL_DPLLCTRLB_Type    DPLLCTRLB;   /**< \brief Offset: 0x08 (R/W 32) DPLL Control B */
  __I  OSCCTRL_DPLLSYNCBUSY_Type DPLLSYNCBUSY; /**< \brief Offset: 0x0C (R/  32) DPLL Synchronization Busy */
  __I  OSCCTRL_DPLLSTATUS_Type   DPLLSTATUS;  /**< \brief Offset: 0x10 (R/  32) DPLL Status */
} OscctrlDpll;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief OSCCTRL hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO OSCCTRL_EVCTRL_Type       EVCTRL;      /**< \brief Offset: 0x00 (R/W  8) Event Control */
       RoReg8                    Reserved1[0x3];
  __IO OSCCTRL_INTENCLR_Type     INTENCLR;    /**< \brief Offset: 0x04 (R/W 32) Interrupt Enable Clear */
  __IO OSCCTRL_INTENSET_Type     INTENSET;    /**< \brief Offset: 0x08 (R/W 32) Interrupt Enable Set */
  __IO OSCCTRL_INTFLAG_Type      INTFLAG;     /**< \brief Offset: 0x0C (R/W 32) Interrupt Flag Status and Clear */
  __I  OSCCTRL_STATUS_Type       STATUS;      /**< \brief Offset: 0x10 (R/  32) Status */
  __IO OSCCTRL_XOSCCTRL_Type     XOSCCTRL[2]; /**< \brief Offset: 0x14 (R/W 32) External Multipurpose Crystal Oscillator Control */
  __IO OSCCTRL_DFLLCTRLA_Type    DFLLCTRLA;   /**< \brief Offset: 0x1C (R/W  8) DFLL48M Control A */
       RoReg8                    Reserved2[0x3];
  __IO OSCCTRL_DFLLCTRLB_Type    DFLLCTRLB;   /**< \brief Offset: 0x20 (R/W  8) DFLL48M Control B */
       RoReg8                    Reserved3[0x3];
  __IO OSCCTRL_DFLLVAL_Type      DFLLVAL;     /**< \brief Offset: 0x24 (R/W 32) DFLL48M Value */
  __IO OSCCTRL_DFLLMUL_Type      DFLLMUL;     /**< \brief Offset: 0x28 (R/W 32) DFLL48M Multiplier */
  __IO OSCCTRL_DFLLSYNC_Type     DFLLSYNC;    /**< \brief Offset: 0x2C (R/W  8) DFLL48M Synchronization */
       RoReg8                    Reserved4[0x3];
       OscctrlDpll               Dpll[2];     /**< \brief Offset: 0x30 OscctrlDpll groups [DPLLS_NUM] */
} Oscctrl;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_OSCCTRL_COMPONENT_ */
