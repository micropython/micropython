/**
 * \file
 *
 * \brief Component description for EVSYS
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

#ifndef _SAMD51_EVSYS_COMPONENT_
#define _SAMD51_EVSYS_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR EVSYS */
/* ========================================================================== */
/** \addtogroup SAMD51_EVSYS Event System Interface */
/*@{*/

#define EVSYS_U2504
#define REV_EVSYS                   0x100

/* -------- EVSYS_CTRLA : (EVSYS Offset: 0x000) (R/W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EVSYS_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_CTRLA_OFFSET          0x000        /**< \brief (EVSYS_CTRLA offset) Control */
#define EVSYS_CTRLA_RESETVALUE      _U_(0x00)    /**< \brief (EVSYS_CTRLA reset_value) Control */

#define EVSYS_CTRLA_SWRST_Pos       0            /**< \brief (EVSYS_CTRLA) Software Reset */
#define EVSYS_CTRLA_SWRST           (_U_(0x1) << EVSYS_CTRLA_SWRST_Pos)
#define EVSYS_CTRLA_MASK            _U_(0x01)    /**< \brief (EVSYS_CTRLA) MASK Register */

/* -------- EVSYS_SWEVT : (EVSYS Offset: 0x004) ( /W 32) Software Event -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CHANNEL0:1;       /*!< bit:      0  Channel 0 Software Selection       */
    uint32_t CHANNEL1:1;       /*!< bit:      1  Channel 1 Software Selection       */
    uint32_t CHANNEL2:1;       /*!< bit:      2  Channel 2 Software Selection       */
    uint32_t CHANNEL3:1;       /*!< bit:      3  Channel 3 Software Selection       */
    uint32_t CHANNEL4:1;       /*!< bit:      4  Channel 4 Software Selection       */
    uint32_t CHANNEL5:1;       /*!< bit:      5  Channel 5 Software Selection       */
    uint32_t CHANNEL6:1;       /*!< bit:      6  Channel 6 Software Selection       */
    uint32_t CHANNEL7:1;       /*!< bit:      7  Channel 7 Software Selection       */
    uint32_t CHANNEL8:1;       /*!< bit:      8  Channel 8 Software Selection       */
    uint32_t CHANNEL9:1;       /*!< bit:      9  Channel 9 Software Selection       */
    uint32_t CHANNEL10:1;      /*!< bit:     10  Channel 10 Software Selection      */
    uint32_t CHANNEL11:1;      /*!< bit:     11  Channel 11 Software Selection      */
    uint32_t CHANNEL12:1;      /*!< bit:     12  Channel 12 Software Selection      */
    uint32_t CHANNEL13:1;      /*!< bit:     13  Channel 13 Software Selection      */
    uint32_t CHANNEL14:1;      /*!< bit:     14  Channel 14 Software Selection      */
    uint32_t CHANNEL15:1;      /*!< bit:     15  Channel 15 Software Selection      */
    uint32_t CHANNEL16:1;      /*!< bit:     16  Channel 16 Software Selection      */
    uint32_t CHANNEL17:1;      /*!< bit:     17  Channel 17 Software Selection      */
    uint32_t CHANNEL18:1;      /*!< bit:     18  Channel 18 Software Selection      */
    uint32_t CHANNEL19:1;      /*!< bit:     19  Channel 19 Software Selection      */
    uint32_t CHANNEL20:1;      /*!< bit:     20  Channel 20 Software Selection      */
    uint32_t CHANNEL21:1;      /*!< bit:     21  Channel 21 Software Selection      */
    uint32_t CHANNEL22:1;      /*!< bit:     22  Channel 22 Software Selection      */
    uint32_t CHANNEL23:1;      /*!< bit:     23  Channel 23 Software Selection      */
    uint32_t CHANNEL24:1;      /*!< bit:     24  Channel 24 Software Selection      */
    uint32_t CHANNEL25:1;      /*!< bit:     25  Channel 25 Software Selection      */
    uint32_t CHANNEL26:1;      /*!< bit:     26  Channel 26 Software Selection      */
    uint32_t CHANNEL27:1;      /*!< bit:     27  Channel 27 Software Selection      */
    uint32_t CHANNEL28:1;      /*!< bit:     28  Channel 28 Software Selection      */
    uint32_t CHANNEL29:1;      /*!< bit:     29  Channel 29 Software Selection      */
    uint32_t CHANNEL30:1;      /*!< bit:     30  Channel 30 Software Selection      */
    uint32_t CHANNEL31:1;      /*!< bit:     31  Channel 31 Software Selection      */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t CHANNEL:32;       /*!< bit:  0..31  Channel x Software Selection       */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EVSYS_SWEVT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_SWEVT_OFFSET          0x004        /**< \brief (EVSYS_SWEVT offset) Software Event */
#define EVSYS_SWEVT_RESETVALUE      _U_(0x00000000) /**< \brief (EVSYS_SWEVT reset_value) Software Event */

#define EVSYS_SWEVT_CHANNEL0_Pos    0            /**< \brief (EVSYS_SWEVT) Channel 0 Software Selection */
#define EVSYS_SWEVT_CHANNEL0        (_U_(1) << EVSYS_SWEVT_CHANNEL0_Pos)
#define EVSYS_SWEVT_CHANNEL1_Pos    1            /**< \brief (EVSYS_SWEVT) Channel 1 Software Selection */
#define EVSYS_SWEVT_CHANNEL1        (_U_(1) << EVSYS_SWEVT_CHANNEL1_Pos)
#define EVSYS_SWEVT_CHANNEL2_Pos    2            /**< \brief (EVSYS_SWEVT) Channel 2 Software Selection */
#define EVSYS_SWEVT_CHANNEL2        (_U_(1) << EVSYS_SWEVT_CHANNEL2_Pos)
#define EVSYS_SWEVT_CHANNEL3_Pos    3            /**< \brief (EVSYS_SWEVT) Channel 3 Software Selection */
#define EVSYS_SWEVT_CHANNEL3        (_U_(1) << EVSYS_SWEVT_CHANNEL3_Pos)
#define EVSYS_SWEVT_CHANNEL4_Pos    4            /**< \brief (EVSYS_SWEVT) Channel 4 Software Selection */
#define EVSYS_SWEVT_CHANNEL4        (_U_(1) << EVSYS_SWEVT_CHANNEL4_Pos)
#define EVSYS_SWEVT_CHANNEL5_Pos    5            /**< \brief (EVSYS_SWEVT) Channel 5 Software Selection */
#define EVSYS_SWEVT_CHANNEL5        (_U_(1) << EVSYS_SWEVT_CHANNEL5_Pos)
#define EVSYS_SWEVT_CHANNEL6_Pos    6            /**< \brief (EVSYS_SWEVT) Channel 6 Software Selection */
#define EVSYS_SWEVT_CHANNEL6        (_U_(1) << EVSYS_SWEVT_CHANNEL6_Pos)
#define EVSYS_SWEVT_CHANNEL7_Pos    7            /**< \brief (EVSYS_SWEVT) Channel 7 Software Selection */
#define EVSYS_SWEVT_CHANNEL7        (_U_(1) << EVSYS_SWEVT_CHANNEL7_Pos)
#define EVSYS_SWEVT_CHANNEL8_Pos    8            /**< \brief (EVSYS_SWEVT) Channel 8 Software Selection */
#define EVSYS_SWEVT_CHANNEL8        (_U_(1) << EVSYS_SWEVT_CHANNEL8_Pos)
#define EVSYS_SWEVT_CHANNEL9_Pos    9            /**< \brief (EVSYS_SWEVT) Channel 9 Software Selection */
#define EVSYS_SWEVT_CHANNEL9        (_U_(1) << EVSYS_SWEVT_CHANNEL9_Pos)
#define EVSYS_SWEVT_CHANNEL10_Pos   10           /**< \brief (EVSYS_SWEVT) Channel 10 Software Selection */
#define EVSYS_SWEVT_CHANNEL10       (_U_(1) << EVSYS_SWEVT_CHANNEL10_Pos)
#define EVSYS_SWEVT_CHANNEL11_Pos   11           /**< \brief (EVSYS_SWEVT) Channel 11 Software Selection */
#define EVSYS_SWEVT_CHANNEL11       (_U_(1) << EVSYS_SWEVT_CHANNEL11_Pos)
#define EVSYS_SWEVT_CHANNEL12_Pos   12           /**< \brief (EVSYS_SWEVT) Channel 12 Software Selection */
#define EVSYS_SWEVT_CHANNEL12       (_U_(1) << EVSYS_SWEVT_CHANNEL12_Pos)
#define EVSYS_SWEVT_CHANNEL13_Pos   13           /**< \brief (EVSYS_SWEVT) Channel 13 Software Selection */
#define EVSYS_SWEVT_CHANNEL13       (_U_(1) << EVSYS_SWEVT_CHANNEL13_Pos)
#define EVSYS_SWEVT_CHANNEL14_Pos   14           /**< \brief (EVSYS_SWEVT) Channel 14 Software Selection */
#define EVSYS_SWEVT_CHANNEL14       (_U_(1) << EVSYS_SWEVT_CHANNEL14_Pos)
#define EVSYS_SWEVT_CHANNEL15_Pos   15           /**< \brief (EVSYS_SWEVT) Channel 15 Software Selection */
#define EVSYS_SWEVT_CHANNEL15       (_U_(1) << EVSYS_SWEVT_CHANNEL15_Pos)
#define EVSYS_SWEVT_CHANNEL16_Pos   16           /**< \brief (EVSYS_SWEVT) Channel 16 Software Selection */
#define EVSYS_SWEVT_CHANNEL16       (_U_(1) << EVSYS_SWEVT_CHANNEL16_Pos)
#define EVSYS_SWEVT_CHANNEL17_Pos   17           /**< \brief (EVSYS_SWEVT) Channel 17 Software Selection */
#define EVSYS_SWEVT_CHANNEL17       (_U_(1) << EVSYS_SWEVT_CHANNEL17_Pos)
#define EVSYS_SWEVT_CHANNEL18_Pos   18           /**< \brief (EVSYS_SWEVT) Channel 18 Software Selection */
#define EVSYS_SWEVT_CHANNEL18       (_U_(1) << EVSYS_SWEVT_CHANNEL18_Pos)
#define EVSYS_SWEVT_CHANNEL19_Pos   19           /**< \brief (EVSYS_SWEVT) Channel 19 Software Selection */
#define EVSYS_SWEVT_CHANNEL19       (_U_(1) << EVSYS_SWEVT_CHANNEL19_Pos)
#define EVSYS_SWEVT_CHANNEL20_Pos   20           /**< \brief (EVSYS_SWEVT) Channel 20 Software Selection */
#define EVSYS_SWEVT_CHANNEL20       (_U_(1) << EVSYS_SWEVT_CHANNEL20_Pos)
#define EVSYS_SWEVT_CHANNEL21_Pos   21           /**< \brief (EVSYS_SWEVT) Channel 21 Software Selection */
#define EVSYS_SWEVT_CHANNEL21       (_U_(1) << EVSYS_SWEVT_CHANNEL21_Pos)
#define EVSYS_SWEVT_CHANNEL22_Pos   22           /**< \brief (EVSYS_SWEVT) Channel 22 Software Selection */
#define EVSYS_SWEVT_CHANNEL22       (_U_(1) << EVSYS_SWEVT_CHANNEL22_Pos)
#define EVSYS_SWEVT_CHANNEL23_Pos   23           /**< \brief (EVSYS_SWEVT) Channel 23 Software Selection */
#define EVSYS_SWEVT_CHANNEL23       (_U_(1) << EVSYS_SWEVT_CHANNEL23_Pos)
#define EVSYS_SWEVT_CHANNEL24_Pos   24           /**< \brief (EVSYS_SWEVT) Channel 24 Software Selection */
#define EVSYS_SWEVT_CHANNEL24       (_U_(1) << EVSYS_SWEVT_CHANNEL24_Pos)
#define EVSYS_SWEVT_CHANNEL25_Pos   25           /**< \brief (EVSYS_SWEVT) Channel 25 Software Selection */
#define EVSYS_SWEVT_CHANNEL25       (_U_(1) << EVSYS_SWEVT_CHANNEL25_Pos)
#define EVSYS_SWEVT_CHANNEL26_Pos   26           /**< \brief (EVSYS_SWEVT) Channel 26 Software Selection */
#define EVSYS_SWEVT_CHANNEL26       (_U_(1) << EVSYS_SWEVT_CHANNEL26_Pos)
#define EVSYS_SWEVT_CHANNEL27_Pos   27           /**< \brief (EVSYS_SWEVT) Channel 27 Software Selection */
#define EVSYS_SWEVT_CHANNEL27       (_U_(1) << EVSYS_SWEVT_CHANNEL27_Pos)
#define EVSYS_SWEVT_CHANNEL28_Pos   28           /**< \brief (EVSYS_SWEVT) Channel 28 Software Selection */
#define EVSYS_SWEVT_CHANNEL28       (_U_(1) << EVSYS_SWEVT_CHANNEL28_Pos)
#define EVSYS_SWEVT_CHANNEL29_Pos   29           /**< \brief (EVSYS_SWEVT) Channel 29 Software Selection */
#define EVSYS_SWEVT_CHANNEL29       (_U_(1) << EVSYS_SWEVT_CHANNEL29_Pos)
#define EVSYS_SWEVT_CHANNEL30_Pos   30           /**< \brief (EVSYS_SWEVT) Channel 30 Software Selection */
#define EVSYS_SWEVT_CHANNEL30       (_U_(1) << EVSYS_SWEVT_CHANNEL30_Pos)
#define EVSYS_SWEVT_CHANNEL31_Pos   31           /**< \brief (EVSYS_SWEVT) Channel 31 Software Selection */
#define EVSYS_SWEVT_CHANNEL31       (_U_(1) << EVSYS_SWEVT_CHANNEL31_Pos)
#define EVSYS_SWEVT_CHANNEL_Pos     0            /**< \brief (EVSYS_SWEVT) Channel x Software Selection */
#define EVSYS_SWEVT_CHANNEL_Msk     (_U_(0xFFFFFFFF) << EVSYS_SWEVT_CHANNEL_Pos)
#define EVSYS_SWEVT_CHANNEL(value)  (EVSYS_SWEVT_CHANNEL_Msk & ((value) << EVSYS_SWEVT_CHANNEL_Pos))
#define EVSYS_SWEVT_MASK            _U_(0xFFFFFFFF) /**< \brief (EVSYS_SWEVT) MASK Register */

/* -------- EVSYS_PRICTRL : (EVSYS Offset: 0x008) (R/W  8) Priority Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  PRI:4;            /*!< bit:  0.. 3  Channel Priority Number            */
    uint8_t  :3;               /*!< bit:  4.. 6  Reserved                           */
    uint8_t  RREN:1;           /*!< bit:      7  Round-Robin Scheduling Enable      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EVSYS_PRICTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_PRICTRL_OFFSET        0x008        /**< \brief (EVSYS_PRICTRL offset) Priority Control */
#define EVSYS_PRICTRL_RESETVALUE    _U_(0x00)    /**< \brief (EVSYS_PRICTRL reset_value) Priority Control */

#define EVSYS_PRICTRL_PRI_Pos       0            /**< \brief (EVSYS_PRICTRL) Channel Priority Number */
#define EVSYS_PRICTRL_PRI_Msk       (_U_(0xF) << EVSYS_PRICTRL_PRI_Pos)
#define EVSYS_PRICTRL_PRI(value)    (EVSYS_PRICTRL_PRI_Msk & ((value) << EVSYS_PRICTRL_PRI_Pos))
#define EVSYS_PRICTRL_RREN_Pos      7            /**< \brief (EVSYS_PRICTRL) Round-Robin Scheduling Enable */
#define EVSYS_PRICTRL_RREN          (_U_(0x1) << EVSYS_PRICTRL_RREN_Pos)
#define EVSYS_PRICTRL_MASK          _U_(0x8F)    /**< \brief (EVSYS_PRICTRL) MASK Register */

/* -------- EVSYS_INTPEND : (EVSYS Offset: 0x010) (R/W 16) Channel Pending Interrupt -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t ID:4;             /*!< bit:  0.. 3  Channel ID                         */
    uint16_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint16_t OVR:1;            /*!< bit:      8  Channel Overrun                    */
    uint16_t EVD:1;            /*!< bit:      9  Channel Event Detected             */
    uint16_t :4;               /*!< bit: 10..13  Reserved                           */
    uint16_t READY:1;          /*!< bit:     14  Ready                              */
    uint16_t BUSY:1;           /*!< bit:     15  Busy                               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} EVSYS_INTPEND_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_INTPEND_OFFSET        0x010        /**< \brief (EVSYS_INTPEND offset) Channel Pending Interrupt */
#define EVSYS_INTPEND_RESETVALUE    _U_(0x4000)  /**< \brief (EVSYS_INTPEND reset_value) Channel Pending Interrupt */

#define EVSYS_INTPEND_ID_Pos        0            /**< \brief (EVSYS_INTPEND) Channel ID */
#define EVSYS_INTPEND_ID_Msk        (_U_(0xF) << EVSYS_INTPEND_ID_Pos)
#define EVSYS_INTPEND_ID(value)     (EVSYS_INTPEND_ID_Msk & ((value) << EVSYS_INTPEND_ID_Pos))
#define EVSYS_INTPEND_OVR_Pos       8            /**< \brief (EVSYS_INTPEND) Channel Overrun */
#define EVSYS_INTPEND_OVR           (_U_(0x1) << EVSYS_INTPEND_OVR_Pos)
#define EVSYS_INTPEND_EVD_Pos       9            /**< \brief (EVSYS_INTPEND) Channel Event Detected */
#define EVSYS_INTPEND_EVD           (_U_(0x1) << EVSYS_INTPEND_EVD_Pos)
#define EVSYS_INTPEND_READY_Pos     14           /**< \brief (EVSYS_INTPEND) Ready */
#define EVSYS_INTPEND_READY         (_U_(0x1) << EVSYS_INTPEND_READY_Pos)
#define EVSYS_INTPEND_BUSY_Pos      15           /**< \brief (EVSYS_INTPEND) Busy */
#define EVSYS_INTPEND_BUSY          (_U_(0x1) << EVSYS_INTPEND_BUSY_Pos)
#define EVSYS_INTPEND_MASK          _U_(0xC30F)  /**< \brief (EVSYS_INTPEND) MASK Register */

/* -------- EVSYS_INTSTATUS : (EVSYS Offset: 0x014) (R/  32) Interrupt Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CHINT0:1;         /*!< bit:      0  Channel 0 Pending Interrupt        */
    uint32_t CHINT1:1;         /*!< bit:      1  Channel 1 Pending Interrupt        */
    uint32_t CHINT2:1;         /*!< bit:      2  Channel 2 Pending Interrupt        */
    uint32_t CHINT3:1;         /*!< bit:      3  Channel 3 Pending Interrupt        */
    uint32_t CHINT4:1;         /*!< bit:      4  Channel 4 Pending Interrupt        */
    uint32_t CHINT5:1;         /*!< bit:      5  Channel 5 Pending Interrupt        */
    uint32_t CHINT6:1;         /*!< bit:      6  Channel 6 Pending Interrupt        */
    uint32_t CHINT7:1;         /*!< bit:      7  Channel 7 Pending Interrupt        */
    uint32_t CHINT8:1;         /*!< bit:      8  Channel 8 Pending Interrupt        */
    uint32_t CHINT9:1;         /*!< bit:      9  Channel 9 Pending Interrupt        */
    uint32_t CHINT10:1;        /*!< bit:     10  Channel 10 Pending Interrupt       */
    uint32_t CHINT11:1;        /*!< bit:     11  Channel 11 Pending Interrupt       */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t CHINT:12;         /*!< bit:  0..11  Channel x Pending Interrupt        */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EVSYS_INTSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_INTSTATUS_OFFSET      0x014        /**< \brief (EVSYS_INTSTATUS offset) Interrupt Status */
#define EVSYS_INTSTATUS_RESETVALUE  _U_(0x00000000) /**< \brief (EVSYS_INTSTATUS reset_value) Interrupt Status */

#define EVSYS_INTSTATUS_CHINT0_Pos  0            /**< \brief (EVSYS_INTSTATUS) Channel 0 Pending Interrupt */
#define EVSYS_INTSTATUS_CHINT0      (_U_(1) << EVSYS_INTSTATUS_CHINT0_Pos)
#define EVSYS_INTSTATUS_CHINT1_Pos  1            /**< \brief (EVSYS_INTSTATUS) Channel 1 Pending Interrupt */
#define EVSYS_INTSTATUS_CHINT1      (_U_(1) << EVSYS_INTSTATUS_CHINT1_Pos)
#define EVSYS_INTSTATUS_CHINT2_Pos  2            /**< \brief (EVSYS_INTSTATUS) Channel 2 Pending Interrupt */
#define EVSYS_INTSTATUS_CHINT2      (_U_(1) << EVSYS_INTSTATUS_CHINT2_Pos)
#define EVSYS_INTSTATUS_CHINT3_Pos  3            /**< \brief (EVSYS_INTSTATUS) Channel 3 Pending Interrupt */
#define EVSYS_INTSTATUS_CHINT3      (_U_(1) << EVSYS_INTSTATUS_CHINT3_Pos)
#define EVSYS_INTSTATUS_CHINT4_Pos  4            /**< \brief (EVSYS_INTSTATUS) Channel 4 Pending Interrupt */
#define EVSYS_INTSTATUS_CHINT4      (_U_(1) << EVSYS_INTSTATUS_CHINT4_Pos)
#define EVSYS_INTSTATUS_CHINT5_Pos  5            /**< \brief (EVSYS_INTSTATUS) Channel 5 Pending Interrupt */
#define EVSYS_INTSTATUS_CHINT5      (_U_(1) << EVSYS_INTSTATUS_CHINT5_Pos)
#define EVSYS_INTSTATUS_CHINT6_Pos  6            /**< \brief (EVSYS_INTSTATUS) Channel 6 Pending Interrupt */
#define EVSYS_INTSTATUS_CHINT6      (_U_(1) << EVSYS_INTSTATUS_CHINT6_Pos)
#define EVSYS_INTSTATUS_CHINT7_Pos  7            /**< \brief (EVSYS_INTSTATUS) Channel 7 Pending Interrupt */
#define EVSYS_INTSTATUS_CHINT7      (_U_(1) << EVSYS_INTSTATUS_CHINT7_Pos)
#define EVSYS_INTSTATUS_CHINT8_Pos  8            /**< \brief (EVSYS_INTSTATUS) Channel 8 Pending Interrupt */
#define EVSYS_INTSTATUS_CHINT8      (_U_(1) << EVSYS_INTSTATUS_CHINT8_Pos)
#define EVSYS_INTSTATUS_CHINT9_Pos  9            /**< \brief (EVSYS_INTSTATUS) Channel 9 Pending Interrupt */
#define EVSYS_INTSTATUS_CHINT9      (_U_(1) << EVSYS_INTSTATUS_CHINT9_Pos)
#define EVSYS_INTSTATUS_CHINT10_Pos 10           /**< \brief (EVSYS_INTSTATUS) Channel 10 Pending Interrupt */
#define EVSYS_INTSTATUS_CHINT10     (_U_(1) << EVSYS_INTSTATUS_CHINT10_Pos)
#define EVSYS_INTSTATUS_CHINT11_Pos 11           /**< \brief (EVSYS_INTSTATUS) Channel 11 Pending Interrupt */
#define EVSYS_INTSTATUS_CHINT11     (_U_(1) << EVSYS_INTSTATUS_CHINT11_Pos)
#define EVSYS_INTSTATUS_CHINT_Pos   0            /**< \brief (EVSYS_INTSTATUS) Channel x Pending Interrupt */
#define EVSYS_INTSTATUS_CHINT_Msk   (_U_(0xFFF) << EVSYS_INTSTATUS_CHINT_Pos)
#define EVSYS_INTSTATUS_CHINT(value) (EVSYS_INTSTATUS_CHINT_Msk & ((value) << EVSYS_INTSTATUS_CHINT_Pos))
#define EVSYS_INTSTATUS_MASK        _U_(0x00000FFF) /**< \brief (EVSYS_INTSTATUS) MASK Register */

/* -------- EVSYS_BUSYCH : (EVSYS Offset: 0x018) (R/  32) Busy Channels -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BUSYCH0:1;        /*!< bit:      0  Busy Channel 0                     */
    uint32_t BUSYCH1:1;        /*!< bit:      1  Busy Channel 1                     */
    uint32_t BUSYCH2:1;        /*!< bit:      2  Busy Channel 2                     */
    uint32_t BUSYCH3:1;        /*!< bit:      3  Busy Channel 3                     */
    uint32_t BUSYCH4:1;        /*!< bit:      4  Busy Channel 4                     */
    uint32_t BUSYCH5:1;        /*!< bit:      5  Busy Channel 5                     */
    uint32_t BUSYCH6:1;        /*!< bit:      6  Busy Channel 6                     */
    uint32_t BUSYCH7:1;        /*!< bit:      7  Busy Channel 7                     */
    uint32_t BUSYCH8:1;        /*!< bit:      8  Busy Channel 8                     */
    uint32_t BUSYCH9:1;        /*!< bit:      9  Busy Channel 9                     */
    uint32_t BUSYCH10:1;       /*!< bit:     10  Busy Channel 10                    */
    uint32_t BUSYCH11:1;       /*!< bit:     11  Busy Channel 11                    */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t BUSYCH:12;        /*!< bit:  0..11  Busy Channel x                     */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EVSYS_BUSYCH_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_BUSYCH_OFFSET         0x018        /**< \brief (EVSYS_BUSYCH offset) Busy Channels */
#define EVSYS_BUSYCH_RESETVALUE     _U_(0x00000000) /**< \brief (EVSYS_BUSYCH reset_value) Busy Channels */

#define EVSYS_BUSYCH_BUSYCH0_Pos    0            /**< \brief (EVSYS_BUSYCH) Busy Channel 0 */
#define EVSYS_BUSYCH_BUSYCH0        (_U_(1) << EVSYS_BUSYCH_BUSYCH0_Pos)
#define EVSYS_BUSYCH_BUSYCH1_Pos    1            /**< \brief (EVSYS_BUSYCH) Busy Channel 1 */
#define EVSYS_BUSYCH_BUSYCH1        (_U_(1) << EVSYS_BUSYCH_BUSYCH1_Pos)
#define EVSYS_BUSYCH_BUSYCH2_Pos    2            /**< \brief (EVSYS_BUSYCH) Busy Channel 2 */
#define EVSYS_BUSYCH_BUSYCH2        (_U_(1) << EVSYS_BUSYCH_BUSYCH2_Pos)
#define EVSYS_BUSYCH_BUSYCH3_Pos    3            /**< \brief (EVSYS_BUSYCH) Busy Channel 3 */
#define EVSYS_BUSYCH_BUSYCH3        (_U_(1) << EVSYS_BUSYCH_BUSYCH3_Pos)
#define EVSYS_BUSYCH_BUSYCH4_Pos    4            /**< \brief (EVSYS_BUSYCH) Busy Channel 4 */
#define EVSYS_BUSYCH_BUSYCH4        (_U_(1) << EVSYS_BUSYCH_BUSYCH4_Pos)
#define EVSYS_BUSYCH_BUSYCH5_Pos    5            /**< \brief (EVSYS_BUSYCH) Busy Channel 5 */
#define EVSYS_BUSYCH_BUSYCH5        (_U_(1) << EVSYS_BUSYCH_BUSYCH5_Pos)
#define EVSYS_BUSYCH_BUSYCH6_Pos    6            /**< \brief (EVSYS_BUSYCH) Busy Channel 6 */
#define EVSYS_BUSYCH_BUSYCH6        (_U_(1) << EVSYS_BUSYCH_BUSYCH6_Pos)
#define EVSYS_BUSYCH_BUSYCH7_Pos    7            /**< \brief (EVSYS_BUSYCH) Busy Channel 7 */
#define EVSYS_BUSYCH_BUSYCH7        (_U_(1) << EVSYS_BUSYCH_BUSYCH7_Pos)
#define EVSYS_BUSYCH_BUSYCH8_Pos    8            /**< \brief (EVSYS_BUSYCH) Busy Channel 8 */
#define EVSYS_BUSYCH_BUSYCH8        (_U_(1) << EVSYS_BUSYCH_BUSYCH8_Pos)
#define EVSYS_BUSYCH_BUSYCH9_Pos    9            /**< \brief (EVSYS_BUSYCH) Busy Channel 9 */
#define EVSYS_BUSYCH_BUSYCH9        (_U_(1) << EVSYS_BUSYCH_BUSYCH9_Pos)
#define EVSYS_BUSYCH_BUSYCH10_Pos   10           /**< \brief (EVSYS_BUSYCH) Busy Channel 10 */
#define EVSYS_BUSYCH_BUSYCH10       (_U_(1) << EVSYS_BUSYCH_BUSYCH10_Pos)
#define EVSYS_BUSYCH_BUSYCH11_Pos   11           /**< \brief (EVSYS_BUSYCH) Busy Channel 11 */
#define EVSYS_BUSYCH_BUSYCH11       (_U_(1) << EVSYS_BUSYCH_BUSYCH11_Pos)
#define EVSYS_BUSYCH_BUSYCH_Pos     0            /**< \brief (EVSYS_BUSYCH) Busy Channel x */
#define EVSYS_BUSYCH_BUSYCH_Msk     (_U_(0xFFF) << EVSYS_BUSYCH_BUSYCH_Pos)
#define EVSYS_BUSYCH_BUSYCH(value)  (EVSYS_BUSYCH_BUSYCH_Msk & ((value) << EVSYS_BUSYCH_BUSYCH_Pos))
#define EVSYS_BUSYCH_MASK           _U_(0x00000FFF) /**< \brief (EVSYS_BUSYCH) MASK Register */

/* -------- EVSYS_READYUSR : (EVSYS Offset: 0x01C) (R/  32) Ready Users -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t READYUSR0:1;      /*!< bit:      0  Ready User for Channel 0           */
    uint32_t READYUSR1:1;      /*!< bit:      1  Ready User for Channel 1           */
    uint32_t READYUSR2:1;      /*!< bit:      2  Ready User for Channel 2           */
    uint32_t READYUSR3:1;      /*!< bit:      3  Ready User for Channel 3           */
    uint32_t READYUSR4:1;      /*!< bit:      4  Ready User for Channel 4           */
    uint32_t READYUSR5:1;      /*!< bit:      5  Ready User for Channel 5           */
    uint32_t READYUSR6:1;      /*!< bit:      6  Ready User for Channel 6           */
    uint32_t READYUSR7:1;      /*!< bit:      7  Ready User for Channel 7           */
    uint32_t READYUSR8:1;      /*!< bit:      8  Ready User for Channel 8           */
    uint32_t READYUSR9:1;      /*!< bit:      9  Ready User for Channel 9           */
    uint32_t READYUSR10:1;     /*!< bit:     10  Ready User for Channel 10          */
    uint32_t READYUSR11:1;     /*!< bit:     11  Ready User for Channel 11          */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t READYUSR:12;      /*!< bit:  0..11  Ready User for Channel x           */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EVSYS_READYUSR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_READYUSR_OFFSET       0x01C        /**< \brief (EVSYS_READYUSR offset) Ready Users */
#define EVSYS_READYUSR_RESETVALUE   _U_(0xFFFFFFFF) /**< \brief (EVSYS_READYUSR reset_value) Ready Users */

#define EVSYS_READYUSR_READYUSR0_Pos 0            /**< \brief (EVSYS_READYUSR) Ready User for Channel 0 */
#define EVSYS_READYUSR_READYUSR0    (_U_(1) << EVSYS_READYUSR_READYUSR0_Pos)
#define EVSYS_READYUSR_READYUSR1_Pos 1            /**< \brief (EVSYS_READYUSR) Ready User for Channel 1 */
#define EVSYS_READYUSR_READYUSR1    (_U_(1) << EVSYS_READYUSR_READYUSR1_Pos)
#define EVSYS_READYUSR_READYUSR2_Pos 2            /**< \brief (EVSYS_READYUSR) Ready User for Channel 2 */
#define EVSYS_READYUSR_READYUSR2    (_U_(1) << EVSYS_READYUSR_READYUSR2_Pos)
#define EVSYS_READYUSR_READYUSR3_Pos 3            /**< \brief (EVSYS_READYUSR) Ready User for Channel 3 */
#define EVSYS_READYUSR_READYUSR3    (_U_(1) << EVSYS_READYUSR_READYUSR3_Pos)
#define EVSYS_READYUSR_READYUSR4_Pos 4            /**< \brief (EVSYS_READYUSR) Ready User for Channel 4 */
#define EVSYS_READYUSR_READYUSR4    (_U_(1) << EVSYS_READYUSR_READYUSR4_Pos)
#define EVSYS_READYUSR_READYUSR5_Pos 5            /**< \brief (EVSYS_READYUSR) Ready User for Channel 5 */
#define EVSYS_READYUSR_READYUSR5    (_U_(1) << EVSYS_READYUSR_READYUSR5_Pos)
#define EVSYS_READYUSR_READYUSR6_Pos 6            /**< \brief (EVSYS_READYUSR) Ready User for Channel 6 */
#define EVSYS_READYUSR_READYUSR6    (_U_(1) << EVSYS_READYUSR_READYUSR6_Pos)
#define EVSYS_READYUSR_READYUSR7_Pos 7            /**< \brief (EVSYS_READYUSR) Ready User for Channel 7 */
#define EVSYS_READYUSR_READYUSR7    (_U_(1) << EVSYS_READYUSR_READYUSR7_Pos)
#define EVSYS_READYUSR_READYUSR8_Pos 8            /**< \brief (EVSYS_READYUSR) Ready User for Channel 8 */
#define EVSYS_READYUSR_READYUSR8    (_U_(1) << EVSYS_READYUSR_READYUSR8_Pos)
#define EVSYS_READYUSR_READYUSR9_Pos 9            /**< \brief (EVSYS_READYUSR) Ready User for Channel 9 */
#define EVSYS_READYUSR_READYUSR9    (_U_(1) << EVSYS_READYUSR_READYUSR9_Pos)
#define EVSYS_READYUSR_READYUSR10_Pos 10           /**< \brief (EVSYS_READYUSR) Ready User for Channel 10 */
#define EVSYS_READYUSR_READYUSR10   (_U_(1) << EVSYS_READYUSR_READYUSR10_Pos)
#define EVSYS_READYUSR_READYUSR11_Pos 11           /**< \brief (EVSYS_READYUSR) Ready User for Channel 11 */
#define EVSYS_READYUSR_READYUSR11   (_U_(1) << EVSYS_READYUSR_READYUSR11_Pos)
#define EVSYS_READYUSR_READYUSR_Pos 0            /**< \brief (EVSYS_READYUSR) Ready User for Channel x */
#define EVSYS_READYUSR_READYUSR_Msk (_U_(0xFFF) << EVSYS_READYUSR_READYUSR_Pos)
#define EVSYS_READYUSR_READYUSR(value) (EVSYS_READYUSR_READYUSR_Msk & ((value) << EVSYS_READYUSR_READYUSR_Pos))
#define EVSYS_READYUSR_MASK         _U_(0x00000FFF) /**< \brief (EVSYS_READYUSR) MASK Register */

/* -------- EVSYS_CHANNEL : (EVSYS Offset: 0x020) (R/W 32) CHANNEL Channel n Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EVGEN:7;          /*!< bit:  0.. 6  Event Generator Selection          */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t PATH:2;           /*!< bit:  8.. 9  Path Selection                     */
    uint32_t EDGSEL:2;         /*!< bit: 10..11  Edge Detection Selection           */
    uint32_t :2;               /*!< bit: 12..13  Reserved                           */
    uint32_t RUNSTDBY:1;       /*!< bit:     14  Run in standby                     */
    uint32_t ONDEMAND:1;       /*!< bit:     15  Generic Clock On Demand            */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EVSYS_CHANNEL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_CHANNEL_OFFSET        0x020        /**< \brief (EVSYS_CHANNEL offset) Channel n Control */
#define EVSYS_CHANNEL_RESETVALUE    _U_(0x00008000) /**< \brief (EVSYS_CHANNEL reset_value) Channel n Control */

#define EVSYS_CHANNEL_EVGEN_Pos     0            /**< \brief (EVSYS_CHANNEL) Event Generator Selection */
#define EVSYS_CHANNEL_EVGEN_Msk     (_U_(0x7F) << EVSYS_CHANNEL_EVGEN_Pos)
#define EVSYS_CHANNEL_EVGEN(value)  (EVSYS_CHANNEL_EVGEN_Msk & ((value) << EVSYS_CHANNEL_EVGEN_Pos))
#define EVSYS_CHANNEL_PATH_Pos      8            /**< \brief (EVSYS_CHANNEL) Path Selection */
#define EVSYS_CHANNEL_PATH_Msk      (_U_(0x3) << EVSYS_CHANNEL_PATH_Pos)
#define EVSYS_CHANNEL_PATH(value)   (EVSYS_CHANNEL_PATH_Msk & ((value) << EVSYS_CHANNEL_PATH_Pos))
#define   EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val _U_(0x0)   /**< \brief (EVSYS_CHANNEL) Synchronous path */
#define   EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val _U_(0x1)   /**< \brief (EVSYS_CHANNEL) Resynchronized path */
#define   EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val _U_(0x2)   /**< \brief (EVSYS_CHANNEL) Asynchronous path */
#define EVSYS_CHANNEL_PATH_SYNCHRONOUS (EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val << EVSYS_CHANNEL_PATH_Pos)
#define EVSYS_CHANNEL_PATH_RESYNCHRONIZED (EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val << EVSYS_CHANNEL_PATH_Pos)
#define EVSYS_CHANNEL_PATH_ASYNCHRONOUS (EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val << EVSYS_CHANNEL_PATH_Pos)
#define EVSYS_CHANNEL_EDGSEL_Pos    10           /**< \brief (EVSYS_CHANNEL) Edge Detection Selection */
#define EVSYS_CHANNEL_EDGSEL_Msk    (_U_(0x3) << EVSYS_CHANNEL_EDGSEL_Pos)
#define EVSYS_CHANNEL_EDGSEL(value) (EVSYS_CHANNEL_EDGSEL_Msk & ((value) << EVSYS_CHANNEL_EDGSEL_Pos))
#define   EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val _U_(0x0)   /**< \brief (EVSYS_CHANNEL) No event output when using the resynchronized or synchronous path */
#define   EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val _U_(0x1)   /**< \brief (EVSYS_CHANNEL) Event detection only on the rising edge of the signal from the event generator when using the resynchronized or synchronous path */
#define   EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val _U_(0x2)   /**< \brief (EVSYS_CHANNEL) Event detection only on the falling edge of the signal from the event generator when using the resynchronized or synchronous path */
#define   EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val _U_(0x3)   /**< \brief (EVSYS_CHANNEL) Event detection on rising and falling edges of the signal from the event generator when using the resynchronized or synchronous path */
#define EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT (EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val << EVSYS_CHANNEL_EDGSEL_Pos)
#define EVSYS_CHANNEL_EDGSEL_RISING_EDGE (EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val << EVSYS_CHANNEL_EDGSEL_Pos)
#define EVSYS_CHANNEL_EDGSEL_FALLING_EDGE (EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val << EVSYS_CHANNEL_EDGSEL_Pos)
#define EVSYS_CHANNEL_EDGSEL_BOTH_EDGES (EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val << EVSYS_CHANNEL_EDGSEL_Pos)
#define EVSYS_CHANNEL_RUNSTDBY_Pos  14           /**< \brief (EVSYS_CHANNEL) Run in standby */
#define EVSYS_CHANNEL_RUNSTDBY      (_U_(0x1) << EVSYS_CHANNEL_RUNSTDBY_Pos)
#define EVSYS_CHANNEL_ONDEMAND_Pos  15           /**< \brief (EVSYS_CHANNEL) Generic Clock On Demand */
#define EVSYS_CHANNEL_ONDEMAND      (_U_(0x1) << EVSYS_CHANNEL_ONDEMAND_Pos)
#define EVSYS_CHANNEL_MASK          _U_(0x0000CF7F) /**< \brief (EVSYS_CHANNEL) MASK Register */

/* -------- EVSYS_CHINTENCLR : (EVSYS Offset: 0x024) (R/W  8) CHANNEL Channel n Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  OVR:1;            /*!< bit:      0  Channel Overrun Interrupt Disable  */
    uint8_t  EVD:1;            /*!< bit:      1  Channel Event Detected Interrupt Disable */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EVSYS_CHINTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_CHINTENCLR_OFFSET     0x024        /**< \brief (EVSYS_CHINTENCLR offset) Channel n Interrupt Enable Clear */
#define EVSYS_CHINTENCLR_RESETVALUE _U_(0x00)    /**< \brief (EVSYS_CHINTENCLR reset_value) Channel n Interrupt Enable Clear */

#define EVSYS_CHINTENCLR_OVR_Pos    0            /**< \brief (EVSYS_CHINTENCLR) Channel Overrun Interrupt Disable */
#define EVSYS_CHINTENCLR_OVR        (_U_(0x1) << EVSYS_CHINTENCLR_OVR_Pos)
#define EVSYS_CHINTENCLR_EVD_Pos    1            /**< \brief (EVSYS_CHINTENCLR) Channel Event Detected Interrupt Disable */
#define EVSYS_CHINTENCLR_EVD        (_U_(0x1) << EVSYS_CHINTENCLR_EVD_Pos)
#define EVSYS_CHINTENCLR_MASK       _U_(0x03)    /**< \brief (EVSYS_CHINTENCLR) MASK Register */

/* -------- EVSYS_CHINTENSET : (EVSYS Offset: 0x025) (R/W  8) CHANNEL Channel n Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  OVR:1;            /*!< bit:      0  Channel Overrun Interrupt Enable   */
    uint8_t  EVD:1;            /*!< bit:      1  Channel Event Detected Interrupt Enable */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EVSYS_CHINTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_CHINTENSET_OFFSET     0x025        /**< \brief (EVSYS_CHINTENSET offset) Channel n Interrupt Enable Set */
#define EVSYS_CHINTENSET_RESETVALUE _U_(0x00)    /**< \brief (EVSYS_CHINTENSET reset_value) Channel n Interrupt Enable Set */

#define EVSYS_CHINTENSET_OVR_Pos    0            /**< \brief (EVSYS_CHINTENSET) Channel Overrun Interrupt Enable */
#define EVSYS_CHINTENSET_OVR        (_U_(0x1) << EVSYS_CHINTENSET_OVR_Pos)
#define EVSYS_CHINTENSET_EVD_Pos    1            /**< \brief (EVSYS_CHINTENSET) Channel Event Detected Interrupt Enable */
#define EVSYS_CHINTENSET_EVD        (_U_(0x1) << EVSYS_CHINTENSET_EVD_Pos)
#define EVSYS_CHINTENSET_MASK       _U_(0x03)    /**< \brief (EVSYS_CHINTENSET) MASK Register */

/* -------- EVSYS_CHINTFLAG : (EVSYS Offset: 0x026) (R/W  8) CHANNEL Channel n Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  OVR:1;            /*!< bit:      0  Channel Overrun                    */
    __I uint8_t  EVD:1;            /*!< bit:      1  Channel Event Detected             */
    __I uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EVSYS_CHINTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_CHINTFLAG_OFFSET      0x026        /**< \brief (EVSYS_CHINTFLAG offset) Channel n Interrupt Flag Status and Clear */
#define EVSYS_CHINTFLAG_RESETVALUE  _U_(0x00)    /**< \brief (EVSYS_CHINTFLAG reset_value) Channel n Interrupt Flag Status and Clear */

#define EVSYS_CHINTFLAG_OVR_Pos     0            /**< \brief (EVSYS_CHINTFLAG) Channel Overrun */
#define EVSYS_CHINTFLAG_OVR         (_U_(0x1) << EVSYS_CHINTFLAG_OVR_Pos)
#define EVSYS_CHINTFLAG_EVD_Pos     1            /**< \brief (EVSYS_CHINTFLAG) Channel Event Detected */
#define EVSYS_CHINTFLAG_EVD         (_U_(0x1) << EVSYS_CHINTFLAG_EVD_Pos)
#define EVSYS_CHINTFLAG_MASK        _U_(0x03)    /**< \brief (EVSYS_CHINTFLAG) MASK Register */

/* -------- EVSYS_CHSTATUS : (EVSYS Offset: 0x027) (R/   8) CHANNEL Channel n Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  RDYUSR:1;         /*!< bit:      0  Ready User                         */
    uint8_t  BUSYCH:1;         /*!< bit:      1  Busy Channel                       */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EVSYS_CHSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_CHSTATUS_OFFSET       0x027        /**< \brief (EVSYS_CHSTATUS offset) Channel n Status */
#define EVSYS_CHSTATUS_RESETVALUE   _U_(0x01)    /**< \brief (EVSYS_CHSTATUS reset_value) Channel n Status */

#define EVSYS_CHSTATUS_RDYUSR_Pos   0            /**< \brief (EVSYS_CHSTATUS) Ready User */
#define EVSYS_CHSTATUS_RDYUSR       (_U_(0x1) << EVSYS_CHSTATUS_RDYUSR_Pos)
#define EVSYS_CHSTATUS_BUSYCH_Pos   1            /**< \brief (EVSYS_CHSTATUS) Busy Channel */
#define EVSYS_CHSTATUS_BUSYCH       (_U_(0x1) << EVSYS_CHSTATUS_BUSYCH_Pos)
#define EVSYS_CHSTATUS_MASK         _U_(0x03)    /**< \brief (EVSYS_CHSTATUS) MASK Register */

/* -------- EVSYS_USER : (EVSYS Offset: 0x120) (R/W 32) User Multiplexer n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CHANNEL:6;        /*!< bit:  0.. 5  Channel Event Selection            */
    uint32_t :26;              /*!< bit:  6..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EVSYS_USER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_USER_OFFSET           0x120        /**< \brief (EVSYS_USER offset) User Multiplexer n */
#define EVSYS_USER_RESETVALUE       _U_(0x00000000) /**< \brief (EVSYS_USER reset_value) User Multiplexer n */

#define EVSYS_USER_CHANNEL_Pos      0            /**< \brief (EVSYS_USER) Channel Event Selection */
#define EVSYS_USER_CHANNEL_Msk      (_U_(0x3F) << EVSYS_USER_CHANNEL_Pos)
#define EVSYS_USER_CHANNEL(value)   (EVSYS_USER_CHANNEL_Msk & ((value) << EVSYS_USER_CHANNEL_Pos))
#define EVSYS_USER_MASK             _U_(0x0000003F) /**< \brief (EVSYS_USER) MASK Register */

/** \brief EvsysChannel hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO EVSYS_CHANNEL_Type        CHANNEL;     /**< \brief Offset: 0x000 (R/W 32) Channel n Control */
  __IO EVSYS_CHINTENCLR_Type     CHINTENCLR;  /**< \brief Offset: 0x004 (R/W  8) Channel n Interrupt Enable Clear */
  __IO EVSYS_CHINTENSET_Type     CHINTENSET;  /**< \brief Offset: 0x005 (R/W  8) Channel n Interrupt Enable Set */
  __IO EVSYS_CHINTFLAG_Type      CHINTFLAG;   /**< \brief Offset: 0x006 (R/W  8) Channel n Interrupt Flag Status and Clear */
  __I  EVSYS_CHSTATUS_Type       CHSTATUS;    /**< \brief Offset: 0x007 (R/   8) Channel n Status */
} EvsysChannel;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief EVSYS hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO EVSYS_CTRLA_Type          CTRLA;       /**< \brief Offset: 0x000 (R/W  8) Control */
       RoReg8                    Reserved1[0x3];
  __O  EVSYS_SWEVT_Type          SWEVT;       /**< \brief Offset: 0x004 ( /W 32) Software Event */
  __IO EVSYS_PRICTRL_Type        PRICTRL;     /**< \brief Offset: 0x008 (R/W  8) Priority Control */
       RoReg8                    Reserved2[0x7];
  __IO EVSYS_INTPEND_Type        INTPEND;     /**< \brief Offset: 0x010 (R/W 16) Channel Pending Interrupt */
       RoReg8                    Reserved3[0x2];
  __I  EVSYS_INTSTATUS_Type      INTSTATUS;   /**< \brief Offset: 0x014 (R/  32) Interrupt Status */
  __I  EVSYS_BUSYCH_Type         BUSYCH;      /**< \brief Offset: 0x018 (R/  32) Busy Channels */
  __I  EVSYS_READYUSR_Type       READYUSR;    /**< \brief Offset: 0x01C (R/  32) Ready Users */
       EvsysChannel              Channel[32]; /**< \brief Offset: 0x020 EvsysChannel groups [CHANNELS] */
  __IO EVSYS_USER_Type           USER[67];    /**< \brief Offset: 0x120 (R/W 32) User Multiplexer n */
} Evsys;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_EVSYS_COMPONENT_ */
