/**
 * \file
 *
 * \brief Component description for SUPC
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

#ifndef _SAMD51_SUPC_COMPONENT_
#define _SAMD51_SUPC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SUPC */
/* ========================================================================== */
/** \addtogroup SAMD51_SUPC Supply Controller */
/*@{*/

#define SUPC_U2407
#define REV_SUPC                    0x100

/* -------- SUPC_INTENCLR : (SUPC Offset: 0x00) (R/W 32) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BOD33RDY:1;       /*!< bit:      0  BOD33 Ready                        */
    uint32_t BOD33DET:1;       /*!< bit:      1  BOD33 Detection                    */
    uint32_t B33SRDY:1;        /*!< bit:      2  BOD33 Synchronization Ready        */
    uint32_t BOD12RDY:1;       /*!< bit:      3  BOD12 Ready                        */
    uint32_t BOD12DET:1;       /*!< bit:      4  BOD12 Detection                    */
    uint32_t B12SRDY:1;        /*!< bit:      5  BOD12 Synchronization Ready        */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t VREGRDY:1;        /*!< bit:      8  Voltage Regulator Ready            */
    uint32_t :1;               /*!< bit:      9  Reserved                           */
    uint32_t VCORERDY:1;       /*!< bit:     10  VDDCORE Ready                      */
    uint32_t :21;              /*!< bit: 11..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_INTENCLR_OFFSET        0x00         /**< \brief (SUPC_INTENCLR offset) Interrupt Enable Clear */
#define SUPC_INTENCLR_RESETVALUE    _U_(0x00000000) /**< \brief (SUPC_INTENCLR reset_value) Interrupt Enable Clear */

#define SUPC_INTENCLR_BOD33RDY_Pos  0            /**< \brief (SUPC_INTENCLR) BOD33 Ready */
#define SUPC_INTENCLR_BOD33RDY      (_U_(0x1) << SUPC_INTENCLR_BOD33RDY_Pos)
#define SUPC_INTENCLR_BOD33DET_Pos  1            /**< \brief (SUPC_INTENCLR) BOD33 Detection */
#define SUPC_INTENCLR_BOD33DET      (_U_(0x1) << SUPC_INTENCLR_BOD33DET_Pos)
#define SUPC_INTENCLR_B33SRDY_Pos   2            /**< \brief (SUPC_INTENCLR) BOD33 Synchronization Ready */
#define SUPC_INTENCLR_B33SRDY       (_U_(0x1) << SUPC_INTENCLR_B33SRDY_Pos)
#define SUPC_INTENCLR_BOD12RDY_Pos  3            /**< \brief (SUPC_INTENCLR) BOD12 Ready */
#define SUPC_INTENCLR_BOD12RDY      (_U_(0x1) << SUPC_INTENCLR_BOD12RDY_Pos)
#define SUPC_INTENCLR_BOD12DET_Pos  4            /**< \brief (SUPC_INTENCLR) BOD12 Detection */
#define SUPC_INTENCLR_BOD12DET      (_U_(0x1) << SUPC_INTENCLR_BOD12DET_Pos)
#define SUPC_INTENCLR_B12SRDY_Pos   5            /**< \brief (SUPC_INTENCLR) BOD12 Synchronization Ready */
#define SUPC_INTENCLR_B12SRDY       (_U_(0x1) << SUPC_INTENCLR_B12SRDY_Pos)
#define SUPC_INTENCLR_VREGRDY_Pos   8            /**< \brief (SUPC_INTENCLR) Voltage Regulator Ready */
#define SUPC_INTENCLR_VREGRDY       (_U_(0x1) << SUPC_INTENCLR_VREGRDY_Pos)
#define SUPC_INTENCLR_VCORERDY_Pos  10           /**< \brief (SUPC_INTENCLR) VDDCORE Ready */
#define SUPC_INTENCLR_VCORERDY      (_U_(0x1) << SUPC_INTENCLR_VCORERDY_Pos)
#define SUPC_INTENCLR_MASK          _U_(0x0000053F) /**< \brief (SUPC_INTENCLR) MASK Register */

/* -------- SUPC_INTENSET : (SUPC Offset: 0x04) (R/W 32) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BOD33RDY:1;       /*!< bit:      0  BOD33 Ready                        */
    uint32_t BOD33DET:1;       /*!< bit:      1  BOD33 Detection                    */
    uint32_t B33SRDY:1;        /*!< bit:      2  BOD33 Synchronization Ready        */
    uint32_t BOD12RDY:1;       /*!< bit:      3  BOD12 Ready                        */
    uint32_t BOD12DET:1;       /*!< bit:      4  BOD12 Detection                    */
    uint32_t B12SRDY:1;        /*!< bit:      5  BOD12 Synchronization Ready        */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t VREGRDY:1;        /*!< bit:      8  Voltage Regulator Ready            */
    uint32_t :1;               /*!< bit:      9  Reserved                           */
    uint32_t VCORERDY:1;       /*!< bit:     10  VDDCORE Ready                      */
    uint32_t :21;              /*!< bit: 11..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_INTENSET_OFFSET        0x04         /**< \brief (SUPC_INTENSET offset) Interrupt Enable Set */
#define SUPC_INTENSET_RESETVALUE    _U_(0x00000000) /**< \brief (SUPC_INTENSET reset_value) Interrupt Enable Set */

#define SUPC_INTENSET_BOD33RDY_Pos  0            /**< \brief (SUPC_INTENSET) BOD33 Ready */
#define SUPC_INTENSET_BOD33RDY      (_U_(0x1) << SUPC_INTENSET_BOD33RDY_Pos)
#define SUPC_INTENSET_BOD33DET_Pos  1            /**< \brief (SUPC_INTENSET) BOD33 Detection */
#define SUPC_INTENSET_BOD33DET      (_U_(0x1) << SUPC_INTENSET_BOD33DET_Pos)
#define SUPC_INTENSET_B33SRDY_Pos   2            /**< \brief (SUPC_INTENSET) BOD33 Synchronization Ready */
#define SUPC_INTENSET_B33SRDY       (_U_(0x1) << SUPC_INTENSET_B33SRDY_Pos)
#define SUPC_INTENSET_BOD12RDY_Pos  3            /**< \brief (SUPC_INTENSET) BOD12 Ready */
#define SUPC_INTENSET_BOD12RDY      (_U_(0x1) << SUPC_INTENSET_BOD12RDY_Pos)
#define SUPC_INTENSET_BOD12DET_Pos  4            /**< \brief (SUPC_INTENSET) BOD12 Detection */
#define SUPC_INTENSET_BOD12DET      (_U_(0x1) << SUPC_INTENSET_BOD12DET_Pos)
#define SUPC_INTENSET_B12SRDY_Pos   5            /**< \brief (SUPC_INTENSET) BOD12 Synchronization Ready */
#define SUPC_INTENSET_B12SRDY       (_U_(0x1) << SUPC_INTENSET_B12SRDY_Pos)
#define SUPC_INTENSET_VREGRDY_Pos   8            /**< \brief (SUPC_INTENSET) Voltage Regulator Ready */
#define SUPC_INTENSET_VREGRDY       (_U_(0x1) << SUPC_INTENSET_VREGRDY_Pos)
#define SUPC_INTENSET_VCORERDY_Pos  10           /**< \brief (SUPC_INTENSET) VDDCORE Ready */
#define SUPC_INTENSET_VCORERDY      (_U_(0x1) << SUPC_INTENSET_VCORERDY_Pos)
#define SUPC_INTENSET_MASK          _U_(0x0000053F) /**< \brief (SUPC_INTENSET) MASK Register */

/* -------- SUPC_INTFLAG : (SUPC Offset: 0x08) (R/W 32) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t BOD33RDY:1;       /*!< bit:      0  BOD33 Ready                        */
    __I uint32_t BOD33DET:1;       /*!< bit:      1  BOD33 Detection                    */
    __I uint32_t B33SRDY:1;        /*!< bit:      2  BOD33 Synchronization Ready        */
    __I uint32_t BOD12RDY:1;       /*!< bit:      3  BOD12 Ready                        */
    __I uint32_t BOD12DET:1;       /*!< bit:      4  BOD12 Detection                    */
    __I uint32_t B12SRDY:1;        /*!< bit:      5  BOD12 Synchronization Ready        */
    __I uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    __I uint32_t VREGRDY:1;        /*!< bit:      8  Voltage Regulator Ready            */
    __I uint32_t :1;               /*!< bit:      9  Reserved                           */
    __I uint32_t VCORERDY:1;       /*!< bit:     10  VDDCORE Ready                      */
    __I uint32_t :21;              /*!< bit: 11..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_INTFLAG_OFFSET         0x08         /**< \brief (SUPC_INTFLAG offset) Interrupt Flag Status and Clear */
#define SUPC_INTFLAG_RESETVALUE     _U_(0x00000000) /**< \brief (SUPC_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define SUPC_INTFLAG_BOD33RDY_Pos   0            /**< \brief (SUPC_INTFLAG) BOD33 Ready */
#define SUPC_INTFLAG_BOD33RDY       (_U_(0x1) << SUPC_INTFLAG_BOD33RDY_Pos)
#define SUPC_INTFLAG_BOD33DET_Pos   1            /**< \brief (SUPC_INTFLAG) BOD33 Detection */
#define SUPC_INTFLAG_BOD33DET       (_U_(0x1) << SUPC_INTFLAG_BOD33DET_Pos)
#define SUPC_INTFLAG_B33SRDY_Pos    2            /**< \brief (SUPC_INTFLAG) BOD33 Synchronization Ready */
#define SUPC_INTFLAG_B33SRDY        (_U_(0x1) << SUPC_INTFLAG_B33SRDY_Pos)
#define SUPC_INTFLAG_BOD12RDY_Pos   3            /**< \brief (SUPC_INTFLAG) BOD12 Ready */
#define SUPC_INTFLAG_BOD12RDY       (_U_(0x1) << SUPC_INTFLAG_BOD12RDY_Pos)
#define SUPC_INTFLAG_BOD12DET_Pos   4            /**< \brief (SUPC_INTFLAG) BOD12 Detection */
#define SUPC_INTFLAG_BOD12DET       (_U_(0x1) << SUPC_INTFLAG_BOD12DET_Pos)
#define SUPC_INTFLAG_B12SRDY_Pos    5            /**< \brief (SUPC_INTFLAG) BOD12 Synchronization Ready */
#define SUPC_INTFLAG_B12SRDY        (_U_(0x1) << SUPC_INTFLAG_B12SRDY_Pos)
#define SUPC_INTFLAG_VREGRDY_Pos    8            /**< \brief (SUPC_INTFLAG) Voltage Regulator Ready */
#define SUPC_INTFLAG_VREGRDY        (_U_(0x1) << SUPC_INTFLAG_VREGRDY_Pos)
#define SUPC_INTFLAG_VCORERDY_Pos   10           /**< \brief (SUPC_INTFLAG) VDDCORE Ready */
#define SUPC_INTFLAG_VCORERDY       (_U_(0x1) << SUPC_INTFLAG_VCORERDY_Pos)
#define SUPC_INTFLAG_MASK           _U_(0x0000053F) /**< \brief (SUPC_INTFLAG) MASK Register */

/* -------- SUPC_STATUS : (SUPC Offset: 0x0C) (R/  32) Power and Clocks Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BOD33RDY:1;       /*!< bit:      0  BOD33 Ready                        */
    uint32_t BOD33DET:1;       /*!< bit:      1  BOD33 Detection                    */
    uint32_t B33SRDY:1;        /*!< bit:      2  BOD33 Synchronization Ready        */
    uint32_t BOD12RDY:1;       /*!< bit:      3  BOD12 Ready                        */
    uint32_t BOD12DET:1;       /*!< bit:      4  BOD12 Detection                    */
    uint32_t B12SRDY:1;        /*!< bit:      5  BOD12 Synchronization Ready        */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t VREGRDY:1;        /*!< bit:      8  Voltage Regulator Ready            */
    uint32_t :1;               /*!< bit:      9  Reserved                           */
    uint32_t VCORERDY:1;       /*!< bit:     10  VDDCORE Ready                      */
    uint32_t :21;              /*!< bit: 11..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_STATUS_OFFSET          0x0C         /**< \brief (SUPC_STATUS offset) Power and Clocks Status */
#define SUPC_STATUS_RESETVALUE      _U_(0x00000000) /**< \brief (SUPC_STATUS reset_value) Power and Clocks Status */

#define SUPC_STATUS_BOD33RDY_Pos    0            /**< \brief (SUPC_STATUS) BOD33 Ready */
#define SUPC_STATUS_BOD33RDY        (_U_(0x1) << SUPC_STATUS_BOD33RDY_Pos)
#define SUPC_STATUS_BOD33DET_Pos    1            /**< \brief (SUPC_STATUS) BOD33 Detection */
#define SUPC_STATUS_BOD33DET        (_U_(0x1) << SUPC_STATUS_BOD33DET_Pos)
#define SUPC_STATUS_B33SRDY_Pos     2            /**< \brief (SUPC_STATUS) BOD33 Synchronization Ready */
#define SUPC_STATUS_B33SRDY         (_U_(0x1) << SUPC_STATUS_B33SRDY_Pos)
#define SUPC_STATUS_BOD12RDY_Pos    3            /**< \brief (SUPC_STATUS) BOD12 Ready */
#define SUPC_STATUS_BOD12RDY        (_U_(0x1) << SUPC_STATUS_BOD12RDY_Pos)
#define SUPC_STATUS_BOD12DET_Pos    4            /**< \brief (SUPC_STATUS) BOD12 Detection */
#define SUPC_STATUS_BOD12DET        (_U_(0x1) << SUPC_STATUS_BOD12DET_Pos)
#define SUPC_STATUS_B12SRDY_Pos     5            /**< \brief (SUPC_STATUS) BOD12 Synchronization Ready */
#define SUPC_STATUS_B12SRDY         (_U_(0x1) << SUPC_STATUS_B12SRDY_Pos)
#define SUPC_STATUS_VREGRDY_Pos     8            /**< \brief (SUPC_STATUS) Voltage Regulator Ready */
#define SUPC_STATUS_VREGRDY         (_U_(0x1) << SUPC_STATUS_VREGRDY_Pos)
#define SUPC_STATUS_VCORERDY_Pos    10           /**< \brief (SUPC_STATUS) VDDCORE Ready */
#define SUPC_STATUS_VCORERDY        (_U_(0x1) << SUPC_STATUS_VCORERDY_Pos)
#define SUPC_STATUS_MASK            _U_(0x0000053F) /**< \brief (SUPC_STATUS) MASK Register */

/* -------- SUPC_BOD33 : (SUPC Offset: 0x10) (R/W 32) BOD33 Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint32_t ACTION:2;         /*!< bit:  2.. 3  Action when Threshold Crossed      */
    uint32_t STDBYCFG:1;       /*!< bit:      4  Configuration in Standby mode      */
    uint32_t RUNSTDBY:1;       /*!< bit:      5  Run in Standby mode                */
    uint32_t RUNHIB:1;         /*!< bit:      6  Run in Hibernate mode              */
    uint32_t RUNBKUP:1;        /*!< bit:      7  Run in Backup mode                 */
    uint32_t HYST:4;           /*!< bit:  8..11  Hysteresis value                   */
    uint32_t PSEL:3;           /*!< bit: 12..14  Prescaler Select                   */
    uint32_t :1;               /*!< bit:     15  Reserved                           */
    uint32_t LEVEL:8;          /*!< bit: 16..23  Threshold Level for VDD            */
    uint32_t VBATLEVEL:8;      /*!< bit: 24..31  Threshold Level in battery backup sleep mode for VBAT */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_BOD33_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_BOD33_OFFSET           0x10         /**< \brief (SUPC_BOD33 offset) BOD33 Control */
#define SUPC_BOD33_RESETVALUE       _U_(0x00000000) /**< \brief (SUPC_BOD33 reset_value) BOD33 Control */

#define SUPC_BOD33_ENABLE_Pos       1            /**< \brief (SUPC_BOD33) Enable */
#define SUPC_BOD33_ENABLE           (_U_(0x1) << SUPC_BOD33_ENABLE_Pos)
#define SUPC_BOD33_ACTION_Pos       2            /**< \brief (SUPC_BOD33) Action when Threshold Crossed */
#define SUPC_BOD33_ACTION_Msk       (_U_(0x3) << SUPC_BOD33_ACTION_Pos)
#define SUPC_BOD33_ACTION(value)    (SUPC_BOD33_ACTION_Msk & ((value) << SUPC_BOD33_ACTION_Pos))
#define   SUPC_BOD33_ACTION_NONE_Val      _U_(0x0)   /**< \brief (SUPC_BOD33) No action */
#define   SUPC_BOD33_ACTION_RESET_Val     _U_(0x1)   /**< \brief (SUPC_BOD33) The BOD33 generates a reset */
#define   SUPC_BOD33_ACTION_INT_Val       _U_(0x2)   /**< \brief (SUPC_BOD33) The BOD33 generates an interrupt */
#define   SUPC_BOD33_ACTION_BKUP_Val      _U_(0x3)   /**< \brief (SUPC_BOD33) The BOD33 puts the device in backup sleep mode */
#define SUPC_BOD33_ACTION_NONE      (SUPC_BOD33_ACTION_NONE_Val    << SUPC_BOD33_ACTION_Pos)
#define SUPC_BOD33_ACTION_RESET     (SUPC_BOD33_ACTION_RESET_Val   << SUPC_BOD33_ACTION_Pos)
#define SUPC_BOD33_ACTION_INT       (SUPC_BOD33_ACTION_INT_Val     << SUPC_BOD33_ACTION_Pos)
#define SUPC_BOD33_ACTION_BKUP      (SUPC_BOD33_ACTION_BKUP_Val    << SUPC_BOD33_ACTION_Pos)
#define SUPC_BOD33_STDBYCFG_Pos     4            /**< \brief (SUPC_BOD33) Configuration in Standby mode */
#define SUPC_BOD33_STDBYCFG         (_U_(0x1) << SUPC_BOD33_STDBYCFG_Pos)
#define SUPC_BOD33_RUNSTDBY_Pos     5            /**< \brief (SUPC_BOD33) Run in Standby mode */
#define SUPC_BOD33_RUNSTDBY         (_U_(0x1) << SUPC_BOD33_RUNSTDBY_Pos)
#define SUPC_BOD33_RUNHIB_Pos       6            /**< \brief (SUPC_BOD33) Run in Hibernate mode */
#define SUPC_BOD33_RUNHIB           (_U_(0x1) << SUPC_BOD33_RUNHIB_Pos)
#define SUPC_BOD33_RUNBKUP_Pos      7            /**< \brief (SUPC_BOD33) Run in Backup mode */
#define SUPC_BOD33_RUNBKUP          (_U_(0x1) << SUPC_BOD33_RUNBKUP_Pos)
#define SUPC_BOD33_HYST_Pos         8            /**< \brief (SUPC_BOD33) Hysteresis value */
#define SUPC_BOD33_HYST_Msk         (_U_(0xF) << SUPC_BOD33_HYST_Pos)
#define SUPC_BOD33_HYST(value)      (SUPC_BOD33_HYST_Msk & ((value) << SUPC_BOD33_HYST_Pos))
#define SUPC_BOD33_PSEL_Pos         12           /**< \brief (SUPC_BOD33) Prescaler Select */
#define SUPC_BOD33_PSEL_Msk         (_U_(0x7) << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL(value)      (SUPC_BOD33_PSEL_Msk & ((value) << SUPC_BOD33_PSEL_Pos))
#define   SUPC_BOD33_PSEL_NODIV_Val       _U_(0x0)   /**< \brief (SUPC_BOD33) Not divided */
#define   SUPC_BOD33_PSEL_DIV4_Val        _U_(0x1)   /**< \brief (SUPC_BOD33) Divide clock by 4 */
#define   SUPC_BOD33_PSEL_DIV8_Val        _U_(0x2)   /**< \brief (SUPC_BOD33) Divide clock by 8 */
#define   SUPC_BOD33_PSEL_DIV16_Val       _U_(0x3)   /**< \brief (SUPC_BOD33) Divide clock by 16 */
#define   SUPC_BOD33_PSEL_DIV32_Val       _U_(0x4)   /**< \brief (SUPC_BOD33) Divide clock by 32 */
#define   SUPC_BOD33_PSEL_DIV64_Val       _U_(0x5)   /**< \brief (SUPC_BOD33) Divide clock by 64 */
#define   SUPC_BOD33_PSEL_DIV128_Val      _U_(0x6)   /**< \brief (SUPC_BOD33) Divide clock by 128 */
#define   SUPC_BOD33_PSEL_DIV256_Val      _U_(0x7)   /**< \brief (SUPC_BOD33) Divide clock by 256 */
#define SUPC_BOD33_PSEL_NODIV       (SUPC_BOD33_PSEL_NODIV_Val     << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV4        (SUPC_BOD33_PSEL_DIV4_Val      << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV8        (SUPC_BOD33_PSEL_DIV8_Val      << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV16       (SUPC_BOD33_PSEL_DIV16_Val     << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV32       (SUPC_BOD33_PSEL_DIV32_Val     << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV64       (SUPC_BOD33_PSEL_DIV64_Val     << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV128      (SUPC_BOD33_PSEL_DIV128_Val    << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_PSEL_DIV256      (SUPC_BOD33_PSEL_DIV256_Val    << SUPC_BOD33_PSEL_Pos)
#define SUPC_BOD33_LEVEL_Pos        16           /**< \brief (SUPC_BOD33) Threshold Level for VDD */
#define SUPC_BOD33_LEVEL_Msk        (_U_(0xFF) << SUPC_BOD33_LEVEL_Pos)
#define SUPC_BOD33_LEVEL(value)     (SUPC_BOD33_LEVEL_Msk & ((value) << SUPC_BOD33_LEVEL_Pos))
#define SUPC_BOD33_VBATLEVEL_Pos    24           /**< \brief (SUPC_BOD33) Threshold Level in battery backup sleep mode for VBAT */
#define SUPC_BOD33_VBATLEVEL_Msk    (_U_(0xFF) << SUPC_BOD33_VBATLEVEL_Pos)
#define SUPC_BOD33_VBATLEVEL(value) (SUPC_BOD33_VBATLEVEL_Msk & ((value) << SUPC_BOD33_VBATLEVEL_Pos))
#define SUPC_BOD33_MASK             _U_(0xFFFF7FFE) /**< \brief (SUPC_BOD33) MASK Register */

/* -------- SUPC_BOD12 : (SUPC Offset: 0x14) (R/W 32) BOD12 Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint32_t HYST:1;           /*!< bit:      2  Hysteresis Enable                  */
    uint32_t ACTION:2;         /*!< bit:  3.. 4  Action when Threshold Crossed      */
    uint32_t STDBYCFG:1;       /*!< bit:      5  Configuration in Standby mode      */
    uint32_t RUNSTDBY:1;       /*!< bit:      6  Run during Standby                 */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t ACTCFG:1;         /*!< bit:      8  Configuration in Active mode       */
    uint32_t :3;               /*!< bit:  9..11  Reserved                           */
    uint32_t PSEL:4;           /*!< bit: 12..15  Prescaler Select                   */
    uint32_t LEVEL:6;          /*!< bit: 16..21  Threshold Level                    */
    uint32_t :10;              /*!< bit: 22..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_BOD12_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_BOD12_OFFSET           0x14         /**< \brief (SUPC_BOD12 offset) BOD12 Control */
#define SUPC_BOD12_RESETVALUE       _U_(0x00000000) /**< \brief (SUPC_BOD12 reset_value) BOD12 Control */

#define SUPC_BOD12_ENABLE_Pos       1            /**< \brief (SUPC_BOD12) Enable */
#define SUPC_BOD12_ENABLE           (_U_(0x1) << SUPC_BOD12_ENABLE_Pos)
#define SUPC_BOD12_HYST_Pos         2            /**< \brief (SUPC_BOD12) Hysteresis Enable */
#define SUPC_BOD12_HYST             (_U_(0x1) << SUPC_BOD12_HYST_Pos)
#define SUPC_BOD12_ACTION_Pos       3            /**< \brief (SUPC_BOD12) Action when Threshold Crossed */
#define SUPC_BOD12_ACTION_Msk       (_U_(0x3) << SUPC_BOD12_ACTION_Pos)
#define SUPC_BOD12_ACTION(value)    (SUPC_BOD12_ACTION_Msk & ((value) << SUPC_BOD12_ACTION_Pos))
#define   SUPC_BOD12_ACTION_NONE_Val      _U_(0x0)   /**< \brief (SUPC_BOD12) No action */
#define   SUPC_BOD12_ACTION_RESET_Val     _U_(0x1)   /**< \brief (SUPC_BOD12) The BOD12 generates a reset */
#define   SUPC_BOD12_ACTION_INT_Val       _U_(0x2)   /**< \brief (SUPC_BOD12) The BOD12 generates an interrupt */
#define SUPC_BOD12_ACTION_NONE      (SUPC_BOD12_ACTION_NONE_Val    << SUPC_BOD12_ACTION_Pos)
#define SUPC_BOD12_ACTION_RESET     (SUPC_BOD12_ACTION_RESET_Val   << SUPC_BOD12_ACTION_Pos)
#define SUPC_BOD12_ACTION_INT       (SUPC_BOD12_ACTION_INT_Val     << SUPC_BOD12_ACTION_Pos)
#define SUPC_BOD12_STDBYCFG_Pos     5            /**< \brief (SUPC_BOD12) Configuration in Standby mode */
#define SUPC_BOD12_STDBYCFG         (_U_(0x1) << SUPC_BOD12_STDBYCFG_Pos)
#define SUPC_BOD12_RUNSTDBY_Pos     6            /**< \brief (SUPC_BOD12) Run during Standby */
#define SUPC_BOD12_RUNSTDBY         (_U_(0x1) << SUPC_BOD12_RUNSTDBY_Pos)
#define SUPC_BOD12_ACTCFG_Pos       8            /**< \brief (SUPC_BOD12) Configuration in Active mode */
#define SUPC_BOD12_ACTCFG           (_U_(0x1) << SUPC_BOD12_ACTCFG_Pos)
#define SUPC_BOD12_PSEL_Pos         12           /**< \brief (SUPC_BOD12) Prescaler Select */
#define SUPC_BOD12_PSEL_Msk         (_U_(0xF) << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL(value)      (SUPC_BOD12_PSEL_Msk & ((value) << SUPC_BOD12_PSEL_Pos))
#define   SUPC_BOD12_PSEL_DIV2_Val        _U_(0x0)   /**< \brief (SUPC_BOD12) Divide clock by 2 */
#define   SUPC_BOD12_PSEL_DIV4_Val        _U_(0x1)   /**< \brief (SUPC_BOD12) Divide clock by 4 */
#define   SUPC_BOD12_PSEL_DIV8_Val        _U_(0x2)   /**< \brief (SUPC_BOD12) Divide clock by 8 */
#define   SUPC_BOD12_PSEL_DIV16_Val       _U_(0x3)   /**< \brief (SUPC_BOD12) Divide clock by 16 */
#define   SUPC_BOD12_PSEL_DIV32_Val       _U_(0x4)   /**< \brief (SUPC_BOD12) Divide clock by 32 */
#define   SUPC_BOD12_PSEL_DIV64_Val       _U_(0x5)   /**< \brief (SUPC_BOD12) Divide clock by 64 */
#define   SUPC_BOD12_PSEL_DIV128_Val      _U_(0x6)   /**< \brief (SUPC_BOD12) Divide clock by 128 */
#define   SUPC_BOD12_PSEL_DIV256_Val      _U_(0x7)   /**< \brief (SUPC_BOD12) Divide clock by 256 */
#define   SUPC_BOD12_PSEL_DIV512_Val      _U_(0x8)   /**< \brief (SUPC_BOD12) Divide clock by 512 */
#define   SUPC_BOD12_PSEL_DIV1024_Val     _U_(0x9)   /**< \brief (SUPC_BOD12) Divide clock by 1024 */
#define   SUPC_BOD12_PSEL_DIV2048_Val     _U_(0xA)   /**< \brief (SUPC_BOD12) Divide clock by 2048 */
#define   SUPC_BOD12_PSEL_DIV4096_Val     _U_(0xB)   /**< \brief (SUPC_BOD12) Divide clock by 4096 */
#define   SUPC_BOD12_PSEL_DIV8192_Val     _U_(0xC)   /**< \brief (SUPC_BOD12) Divide clock by 8192 */
#define   SUPC_BOD12_PSEL_DIV16384_Val    _U_(0xD)   /**< \brief (SUPC_BOD12) Divide clock by 16384 */
#define   SUPC_BOD12_PSEL_DIV32768_Val    _U_(0xE)   /**< \brief (SUPC_BOD12) Divide clock by 32768 */
#define   SUPC_BOD12_PSEL_DIV65536_Val    _U_(0xF)   /**< \brief (SUPC_BOD12) Divide clock by 65536 */
#define SUPC_BOD12_PSEL_DIV2        (SUPC_BOD12_PSEL_DIV2_Val      << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV4        (SUPC_BOD12_PSEL_DIV4_Val      << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV8        (SUPC_BOD12_PSEL_DIV8_Val      << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV16       (SUPC_BOD12_PSEL_DIV16_Val     << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV32       (SUPC_BOD12_PSEL_DIV32_Val     << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV64       (SUPC_BOD12_PSEL_DIV64_Val     << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV128      (SUPC_BOD12_PSEL_DIV128_Val    << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV256      (SUPC_BOD12_PSEL_DIV256_Val    << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV512      (SUPC_BOD12_PSEL_DIV512_Val    << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV1024     (SUPC_BOD12_PSEL_DIV1024_Val   << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV2048     (SUPC_BOD12_PSEL_DIV2048_Val   << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV4096     (SUPC_BOD12_PSEL_DIV4096_Val   << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV8192     (SUPC_BOD12_PSEL_DIV8192_Val   << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV16384    (SUPC_BOD12_PSEL_DIV16384_Val  << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV32768    (SUPC_BOD12_PSEL_DIV32768_Val  << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_PSEL_DIV65536    (SUPC_BOD12_PSEL_DIV65536_Val  << SUPC_BOD12_PSEL_Pos)
#define SUPC_BOD12_LEVEL_Pos        16           /**< \brief (SUPC_BOD12) Threshold Level */
#define SUPC_BOD12_LEVEL_Msk        (_U_(0x3F) << SUPC_BOD12_LEVEL_Pos)
#define SUPC_BOD12_LEVEL(value)     (SUPC_BOD12_LEVEL_Msk & ((value) << SUPC_BOD12_LEVEL_Pos))
#define SUPC_BOD12_MASK             _U_(0x003FF17E) /**< \brief (SUPC_BOD12) MASK Register */

/* -------- SUPC_VREG : (SUPC Offset: 0x18) (R/W 32) VREG Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint32_t SEL:1;            /*!< bit:      2  Voltage Regulator Selection        */
    uint32_t :4;               /*!< bit:  3.. 6  Reserved                           */
    uint32_t RUNBKUP:1;        /*!< bit:      7  Run in Backup mode                 */
    uint32_t :8;               /*!< bit:  8..15  Reserved                           */
    uint32_t VSEN:1;           /*!< bit:     16  Voltage Scaling Enable             */
    uint32_t :7;               /*!< bit: 17..23  Reserved                           */
    uint32_t VSPER:3;          /*!< bit: 24..26  Voltage Scaling Period             */
    uint32_t :5;               /*!< bit: 27..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_VREG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_VREG_OFFSET            0x18         /**< \brief (SUPC_VREG offset) VREG Control */
#define SUPC_VREG_RESETVALUE        _U_(0x00000002) /**< \brief (SUPC_VREG reset_value) VREG Control */

#define SUPC_VREG_ENABLE_Pos        1            /**< \brief (SUPC_VREG) Enable */
#define SUPC_VREG_ENABLE            (_U_(0x1) << SUPC_VREG_ENABLE_Pos)
#define SUPC_VREG_SEL_Pos           2            /**< \brief (SUPC_VREG) Voltage Regulator Selection */
#define SUPC_VREG_SEL               (_U_(0x1) << SUPC_VREG_SEL_Pos)
#define   SUPC_VREG_SEL_LDO_Val           _U_(0x0)   /**< \brief (SUPC_VREG) LDO selection */
#define   SUPC_VREG_SEL_BUCK_Val          _U_(0x1)   /**< \brief (SUPC_VREG) Buck selection */
#define SUPC_VREG_SEL_LDO           (SUPC_VREG_SEL_LDO_Val         << SUPC_VREG_SEL_Pos)
#define SUPC_VREG_SEL_BUCK          (SUPC_VREG_SEL_BUCK_Val        << SUPC_VREG_SEL_Pos)
#define SUPC_VREG_RUNBKUP_Pos       7            /**< \brief (SUPC_VREG) Run in Backup mode */
#define SUPC_VREG_RUNBKUP           (_U_(0x1) << SUPC_VREG_RUNBKUP_Pos)
#define SUPC_VREG_VSEN_Pos          16           /**< \brief (SUPC_VREG) Voltage Scaling Enable */
#define SUPC_VREG_VSEN              (_U_(0x1) << SUPC_VREG_VSEN_Pos)
#define SUPC_VREG_VSPER_Pos         24           /**< \brief (SUPC_VREG) Voltage Scaling Period */
#define SUPC_VREG_VSPER_Msk         (_U_(0x7) << SUPC_VREG_VSPER_Pos)
#define SUPC_VREG_VSPER(value)      (SUPC_VREG_VSPER_Msk & ((value) << SUPC_VREG_VSPER_Pos))
#define SUPC_VREG_MASK              _U_(0x07010086) /**< \brief (SUPC_VREG) MASK Register */

/* -------- SUPC_VREF : (SUPC Offset: 0x1C) (R/W 32) VREF Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t TSEN:1;           /*!< bit:      1  Temperature Sensor Output Enable   */
    uint32_t VREFOE:1;         /*!< bit:      2  Voltage Reference Output Enable    */
    uint32_t TSSEL:1;          /*!< bit:      3  Temperature Sensor Selection       */
    uint32_t :2;               /*!< bit:  4.. 5  Reserved                           */
    uint32_t RUNSTDBY:1;       /*!< bit:      6  Run during Standby                 */
    uint32_t ONDEMAND:1;       /*!< bit:      7  On Demand Contrl                   */
    uint32_t :8;               /*!< bit:  8..15  Reserved                           */
    uint32_t SEL:4;            /*!< bit: 16..19  Voltage Reference Selection        */
    uint32_t :12;              /*!< bit: 20..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_VREF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_VREF_OFFSET            0x1C         /**< \brief (SUPC_VREF offset) VREF Control */
#define SUPC_VREF_RESETVALUE        _U_(0x00000000) /**< \brief (SUPC_VREF reset_value) VREF Control */

#define SUPC_VREF_TSEN_Pos          1            /**< \brief (SUPC_VREF) Temperature Sensor Output Enable */
#define SUPC_VREF_TSEN              (_U_(0x1) << SUPC_VREF_TSEN_Pos)
#define SUPC_VREF_VREFOE_Pos        2            /**< \brief (SUPC_VREF) Voltage Reference Output Enable */
#define SUPC_VREF_VREFOE            (_U_(0x1) << SUPC_VREF_VREFOE_Pos)
#define SUPC_VREF_TSSEL_Pos         3            /**< \brief (SUPC_VREF) Temperature Sensor Selection */
#define SUPC_VREF_TSSEL             (_U_(0x1) << SUPC_VREF_TSSEL_Pos)
#define SUPC_VREF_RUNSTDBY_Pos      6            /**< \brief (SUPC_VREF) Run during Standby */
#define SUPC_VREF_RUNSTDBY          (_U_(0x1) << SUPC_VREF_RUNSTDBY_Pos)
#define SUPC_VREF_ONDEMAND_Pos      7            /**< \brief (SUPC_VREF) On Demand Contrl */
#define SUPC_VREF_ONDEMAND          (_U_(0x1) << SUPC_VREF_ONDEMAND_Pos)
#define SUPC_VREF_SEL_Pos           16           /**< \brief (SUPC_VREF) Voltage Reference Selection */
#define SUPC_VREF_SEL_Msk           (_U_(0xF) << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_SEL(value)        (SUPC_VREF_SEL_Msk & ((value) << SUPC_VREF_SEL_Pos))
#define   SUPC_VREF_SEL_1V0_Val           _U_(0x0)   /**< \brief (SUPC_VREF) 1.0V voltage reference typical value */
#define   SUPC_VREF_SEL_1V1_Val           _U_(0x1)   /**< \brief (SUPC_VREF) 1.1V voltage reference typical value */
#define   SUPC_VREF_SEL_1V2_Val           _U_(0x2)   /**< \brief (SUPC_VREF) 1.2V voltage reference typical value */
#define   SUPC_VREF_SEL_1V25_Val          _U_(0x3)   /**< \brief (SUPC_VREF) 1.25V voltage reference typical value */
#define   SUPC_VREF_SEL_2V0_Val           _U_(0x4)   /**< \brief (SUPC_VREF) 2.0V voltage reference typical value */
#define   SUPC_VREF_SEL_2V2_Val           _U_(0x5)   /**< \brief (SUPC_VREF) 2.2V voltage reference typical value */
#define   SUPC_VREF_SEL_2V4_Val           _U_(0x6)   /**< \brief (SUPC_VREF) 2.4V voltage reference typical value */
#define   SUPC_VREF_SEL_2V5_Val           _U_(0x7)   /**< \brief (SUPC_VREF) 2.5V voltage reference typical value */
#define SUPC_VREF_SEL_1V0           (SUPC_VREF_SEL_1V0_Val         << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_SEL_1V1           (SUPC_VREF_SEL_1V1_Val         << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_SEL_1V2           (SUPC_VREF_SEL_1V2_Val         << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_SEL_1V25          (SUPC_VREF_SEL_1V25_Val        << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_SEL_2V0           (SUPC_VREF_SEL_2V0_Val         << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_SEL_2V2           (SUPC_VREF_SEL_2V2_Val         << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_SEL_2V4           (SUPC_VREF_SEL_2V4_Val         << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_SEL_2V5           (SUPC_VREF_SEL_2V5_Val         << SUPC_VREF_SEL_Pos)
#define SUPC_VREF_MASK              _U_(0x000F00CE) /**< \brief (SUPC_VREF) MASK Register */

/* -------- SUPC_BBPS : (SUPC Offset: 0x20) (R/W 32) Battery Backup Power Switch -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CONF:1;           /*!< bit:      0  Battery Backup Configuration       */
    uint32_t :1;               /*!< bit:      1  Reserved                           */
    uint32_t WAKEEN:1;         /*!< bit:      2  Wake Enable                        */
    uint32_t :29;              /*!< bit:  3..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_BBPS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_BBPS_OFFSET            0x20         /**< \brief (SUPC_BBPS offset) Battery Backup Power Switch */
#define SUPC_BBPS_RESETVALUE        _U_(0x00000000) /**< \brief (SUPC_BBPS reset_value) Battery Backup Power Switch */

#define SUPC_BBPS_CONF_Pos          0            /**< \brief (SUPC_BBPS) Battery Backup Configuration */
#define SUPC_BBPS_CONF              (_U_(0x1) << SUPC_BBPS_CONF_Pos)
#define   SUPC_BBPS_CONF_BOD33_Val        _U_(0x0)   /**< \brief (SUPC_BBPS) The power switch is handled by the BOD33 */
#define   SUPC_BBPS_CONF_FORCED_Val       _U_(0x1)   /**< \brief (SUPC_BBPS) In Backup Domain, the backup domain is always supplied by battery backup power */
#define SUPC_BBPS_CONF_BOD33        (SUPC_BBPS_CONF_BOD33_Val      << SUPC_BBPS_CONF_Pos)
#define SUPC_BBPS_CONF_FORCED       (SUPC_BBPS_CONF_FORCED_Val     << SUPC_BBPS_CONF_Pos)
#define SUPC_BBPS_WAKEEN_Pos        2            /**< \brief (SUPC_BBPS) Wake Enable */
#define SUPC_BBPS_WAKEEN            (_U_(0x1) << SUPC_BBPS_WAKEEN_Pos)
#define SUPC_BBPS_MASK              _U_(0x00000005) /**< \brief (SUPC_BBPS) MASK Register */

/* -------- SUPC_BKOUT : (SUPC Offset: 0x24) (R/W 32) Backup Output Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EN:2;             /*!< bit:  0.. 1  Enable Output                      */
    uint32_t :6;               /*!< bit:  2.. 7  Reserved                           */
    uint32_t CLR:2;            /*!< bit:  8.. 9  Clear Output                       */
    uint32_t :6;               /*!< bit: 10..15  Reserved                           */
    uint32_t SET:2;            /*!< bit: 16..17  Set Output                         */
    uint32_t :6;               /*!< bit: 18..23  Reserved                           */
    uint32_t RTCTGL:2;         /*!< bit: 24..25  RTC Toggle Output                  */
    uint32_t :6;               /*!< bit: 26..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_BKOUT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_BKOUT_OFFSET           0x24         /**< \brief (SUPC_BKOUT offset) Backup Output Control */
#define SUPC_BKOUT_RESETVALUE       _U_(0x00000000) /**< \brief (SUPC_BKOUT reset_value) Backup Output Control */

#define SUPC_BKOUT_EN_Pos           0            /**< \brief (SUPC_BKOUT) Enable Output */
#define SUPC_BKOUT_EN_Msk           (_U_(0x3) << SUPC_BKOUT_EN_Pos)
#define SUPC_BKOUT_EN(value)        (SUPC_BKOUT_EN_Msk & ((value) << SUPC_BKOUT_EN_Pos))
#define SUPC_BKOUT_CLR_Pos          8            /**< \brief (SUPC_BKOUT) Clear Output */
#define SUPC_BKOUT_CLR_Msk          (_U_(0x3) << SUPC_BKOUT_CLR_Pos)
#define SUPC_BKOUT_CLR(value)       (SUPC_BKOUT_CLR_Msk & ((value) << SUPC_BKOUT_CLR_Pos))
#define SUPC_BKOUT_SET_Pos          16           /**< \brief (SUPC_BKOUT) Set Output */
#define SUPC_BKOUT_SET_Msk          (_U_(0x3) << SUPC_BKOUT_SET_Pos)
#define SUPC_BKOUT_SET(value)       (SUPC_BKOUT_SET_Msk & ((value) << SUPC_BKOUT_SET_Pos))
#define SUPC_BKOUT_RTCTGL_Pos       24           /**< \brief (SUPC_BKOUT) RTC Toggle Output */
#define SUPC_BKOUT_RTCTGL_Msk       (_U_(0x3) << SUPC_BKOUT_RTCTGL_Pos)
#define SUPC_BKOUT_RTCTGL(value)    (SUPC_BKOUT_RTCTGL_Msk & ((value) << SUPC_BKOUT_RTCTGL_Pos))
#define SUPC_BKOUT_MASK             _U_(0x03030303) /**< \brief (SUPC_BKOUT) MASK Register */

/* -------- SUPC_BKIN : (SUPC Offset: 0x28) (R/  32) Backup Input Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BKIN:8;           /*!< bit:  0.. 7  Backup Input Value                 */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SUPC_BKIN_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SUPC_BKIN_OFFSET            0x28         /**< \brief (SUPC_BKIN offset) Backup Input Control */
#define SUPC_BKIN_RESETVALUE        _U_(0x00000000) /**< \brief (SUPC_BKIN reset_value) Backup Input Control */

#define SUPC_BKIN_BKIN_Pos          0            /**< \brief (SUPC_BKIN) Backup Input Value */
#define SUPC_BKIN_BKIN_Msk          (_U_(0xFF) << SUPC_BKIN_BKIN_Pos)
#define SUPC_BKIN_BKIN(value)       (SUPC_BKIN_BKIN_Msk & ((value) << SUPC_BKIN_BKIN_Pos))
#define SUPC_BKIN_MASK              _U_(0x000000FF) /**< \brief (SUPC_BKIN) MASK Register */

/** \brief SUPC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO SUPC_INTENCLR_Type        INTENCLR;    /**< \brief Offset: 0x00 (R/W 32) Interrupt Enable Clear */
  __IO SUPC_INTENSET_Type        INTENSET;    /**< \brief Offset: 0x04 (R/W 32) Interrupt Enable Set */
  __IO SUPC_INTFLAG_Type         INTFLAG;     /**< \brief Offset: 0x08 (R/W 32) Interrupt Flag Status and Clear */
  __I  SUPC_STATUS_Type          STATUS;      /**< \brief Offset: 0x0C (R/  32) Power and Clocks Status */
  __IO SUPC_BOD33_Type           BOD33;       /**< \brief Offset: 0x10 (R/W 32) BOD33 Control */
  __IO SUPC_BOD12_Type           BOD12;       /**< \brief Offset: 0x14 (R/W 32) BOD12 Control */
  __IO SUPC_VREG_Type            VREG;        /**< \brief Offset: 0x18 (R/W 32) VREG Control */
  __IO SUPC_VREF_Type            VREF;        /**< \brief Offset: 0x1C (R/W 32) VREF Control */
  __IO SUPC_BBPS_Type            BBPS;        /**< \brief Offset: 0x20 (R/W 32) Battery Backup Power Switch */
  __IO SUPC_BKOUT_Type           BKOUT;       /**< \brief Offset: 0x24 (R/W 32) Backup Output Control */
  __I  SUPC_BKIN_Type            BKIN;        /**< \brief Offset: 0x28 (R/  32) Backup Input Control */
} Supc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_SUPC_COMPONENT_ */
