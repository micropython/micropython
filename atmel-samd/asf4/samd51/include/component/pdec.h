/**
 * \file
 *
 * \brief Component description for PDEC
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

#ifndef _SAMD51_PDEC_COMPONENT_
#define _SAMD51_PDEC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR PDEC */
/* ========================================================================== */
/** \addtogroup SAMD51_PDEC Quadrature Decodeur */
/*@{*/

#define PDEC_U2263
#define REV_PDEC                    0x100

/* -------- PDEC_CTRLA : (PDEC Offset: 0x00) (R/W 32) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint32_t MODE:2;           /*!< bit:  2.. 3  Operation Mode                     */
    uint32_t :2;               /*!< bit:  4.. 5  Reserved                           */
    uint32_t RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t CONF:3;           /*!< bit:  8..10  PDEC Configuration                 */
    uint32_t ALOCK:1;          /*!< bit:     11  Auto Lock                          */
    uint32_t :2;               /*!< bit: 12..13  Reserved                           */
    uint32_t SWAP:1;           /*!< bit:     14  PDEC Phase A and B Swap            */
    uint32_t PEREN:1;          /*!< bit:     15  Period Enable                      */
    uint32_t PINEN0:1;         /*!< bit:     16  PDEC Input From Pin 0 Enable       */
    uint32_t PINEN1:1;         /*!< bit:     17  PDEC Input From Pin 1 Enable       */
    uint32_t PINEN2:1;         /*!< bit:     18  PDEC Input From Pin 2 Enable       */
    uint32_t :1;               /*!< bit:     19  Reserved                           */
    uint32_t PINVEN0:1;        /*!< bit:     20  IO Pin 0 Invert Enable             */
    uint32_t PINVEN1:1;        /*!< bit:     21  IO Pin 1 Invert Enable             */
    uint32_t PINVEN2:1;        /*!< bit:     22  IO Pin 2 Invert Enable             */
    uint32_t :1;               /*!< bit:     23  Reserved                           */
    uint32_t ANGULAR:3;        /*!< bit: 24..26  Angular Counter Length             */
    uint32_t :1;               /*!< bit:     27  Reserved                           */
    uint32_t MAXCMP:4;         /*!< bit: 28..31  Maximum Consecutive Missing Pulses */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :16;              /*!< bit:  0..15  Reserved                           */
    uint32_t PINEN:3;          /*!< bit: 16..18  PDEC Input From Pin x Enable       */
    uint32_t :1;               /*!< bit:     19  Reserved                           */
    uint32_t PINVEN:3;         /*!< bit: 20..22  IO Pin x Invert Enable             */
    uint32_t :9;               /*!< bit: 23..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PDEC_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_CTRLA_OFFSET           0x00         /**< \brief (PDEC_CTRLA offset) Control A */
#define PDEC_CTRLA_RESETVALUE       _U_(0x00000000) /**< \brief (PDEC_CTRLA reset_value) Control A */

#define PDEC_CTRLA_SWRST_Pos        0            /**< \brief (PDEC_CTRLA) Software Reset */
#define PDEC_CTRLA_SWRST            (_U_(0x1) << PDEC_CTRLA_SWRST_Pos)
#define PDEC_CTRLA_ENABLE_Pos       1            /**< \brief (PDEC_CTRLA) Enable */
#define PDEC_CTRLA_ENABLE           (_U_(0x1) << PDEC_CTRLA_ENABLE_Pos)
#define PDEC_CTRLA_MODE_Pos         2            /**< \brief (PDEC_CTRLA) Operation Mode */
#define PDEC_CTRLA_MODE_Msk         (_U_(0x3) << PDEC_CTRLA_MODE_Pos)
#define PDEC_CTRLA_MODE(value)      (PDEC_CTRLA_MODE_Msk & ((value) << PDEC_CTRLA_MODE_Pos))
#define   PDEC_CTRLA_MODE_QDEC_Val        _U_(0x0)   /**< \brief (PDEC_CTRLA) QDEC operating mode */
#define   PDEC_CTRLA_MODE_HALL_Val        _U_(0x1)   /**< \brief (PDEC_CTRLA) HALL operating mode */
#define   PDEC_CTRLA_MODE_COUNTER_Val     _U_(0x2)   /**< \brief (PDEC_CTRLA) COUNTER operating mode */
#define PDEC_CTRLA_MODE_QDEC        (PDEC_CTRLA_MODE_QDEC_Val      << PDEC_CTRLA_MODE_Pos)
#define PDEC_CTRLA_MODE_HALL        (PDEC_CTRLA_MODE_HALL_Val      << PDEC_CTRLA_MODE_Pos)
#define PDEC_CTRLA_MODE_COUNTER     (PDEC_CTRLA_MODE_COUNTER_Val   << PDEC_CTRLA_MODE_Pos)
#define PDEC_CTRLA_RUNSTDBY_Pos     6            /**< \brief (PDEC_CTRLA) Run in Standby */
#define PDEC_CTRLA_RUNSTDBY         (_U_(0x1) << PDEC_CTRLA_RUNSTDBY_Pos)
#define PDEC_CTRLA_CONF_Pos         8            /**< \brief (PDEC_CTRLA) PDEC Configuration */
#define PDEC_CTRLA_CONF_Msk         (_U_(0x7) << PDEC_CTRLA_CONF_Pos)
#define PDEC_CTRLA_CONF(value)      (PDEC_CTRLA_CONF_Msk & ((value) << PDEC_CTRLA_CONF_Pos))
#define   PDEC_CTRLA_CONF_X4_Val          _U_(0x0)   /**< \brief (PDEC_CTRLA) Quadrature decoder direction */
#define   PDEC_CTRLA_CONF_X4S_Val         _U_(0x1)   /**< \brief (PDEC_CTRLA) Secure Quadrature decoder direction */
#define   PDEC_CTRLA_CONF_X2_Val          _U_(0x2)   /**< \brief (PDEC_CTRLA) Decoder direction */
#define   PDEC_CTRLA_CONF_X2S_Val         _U_(0x3)   /**< \brief (PDEC_CTRLA) Secure decoder direction */
#define   PDEC_CTRLA_CONF_AUTOC_Val       _U_(0x4)   /**< \brief (PDEC_CTRLA) Auto correction mode */
#define PDEC_CTRLA_CONF_X4          (PDEC_CTRLA_CONF_X4_Val        << PDEC_CTRLA_CONF_Pos)
#define PDEC_CTRLA_CONF_X4S         (PDEC_CTRLA_CONF_X4S_Val       << PDEC_CTRLA_CONF_Pos)
#define PDEC_CTRLA_CONF_X2          (PDEC_CTRLA_CONF_X2_Val        << PDEC_CTRLA_CONF_Pos)
#define PDEC_CTRLA_CONF_X2S         (PDEC_CTRLA_CONF_X2S_Val       << PDEC_CTRLA_CONF_Pos)
#define PDEC_CTRLA_CONF_AUTOC       (PDEC_CTRLA_CONF_AUTOC_Val     << PDEC_CTRLA_CONF_Pos)
#define PDEC_CTRLA_ALOCK_Pos        11           /**< \brief (PDEC_CTRLA) Auto Lock */
#define PDEC_CTRLA_ALOCK            (_U_(0x1) << PDEC_CTRLA_ALOCK_Pos)
#define PDEC_CTRLA_SWAP_Pos         14           /**< \brief (PDEC_CTRLA) PDEC Phase A and B Swap */
#define PDEC_CTRLA_SWAP             (_U_(0x1) << PDEC_CTRLA_SWAP_Pos)
#define PDEC_CTRLA_PEREN_Pos        15           /**< \brief (PDEC_CTRLA) Period Enable */
#define PDEC_CTRLA_PEREN            (_U_(0x1) << PDEC_CTRLA_PEREN_Pos)
#define PDEC_CTRLA_PINEN0_Pos       16           /**< \brief (PDEC_CTRLA) PDEC Input From Pin 0 Enable */
#define PDEC_CTRLA_PINEN0           (_U_(1) << PDEC_CTRLA_PINEN0_Pos)
#define PDEC_CTRLA_PINEN1_Pos       17           /**< \brief (PDEC_CTRLA) PDEC Input From Pin 1 Enable */
#define PDEC_CTRLA_PINEN1           (_U_(1) << PDEC_CTRLA_PINEN1_Pos)
#define PDEC_CTRLA_PINEN2_Pos       18           /**< \brief (PDEC_CTRLA) PDEC Input From Pin 2 Enable */
#define PDEC_CTRLA_PINEN2           (_U_(1) << PDEC_CTRLA_PINEN2_Pos)
#define PDEC_CTRLA_PINEN_Pos        16           /**< \brief (PDEC_CTRLA) PDEC Input From Pin x Enable */
#define PDEC_CTRLA_PINEN_Msk        (_U_(0x7) << PDEC_CTRLA_PINEN_Pos)
#define PDEC_CTRLA_PINEN(value)     (PDEC_CTRLA_PINEN_Msk & ((value) << PDEC_CTRLA_PINEN_Pos))
#define PDEC_CTRLA_PINVEN0_Pos      20           /**< \brief (PDEC_CTRLA) IO Pin 0 Invert Enable */
#define PDEC_CTRLA_PINVEN0          (_U_(1) << PDEC_CTRLA_PINVEN0_Pos)
#define PDEC_CTRLA_PINVEN1_Pos      21           /**< \brief (PDEC_CTRLA) IO Pin 1 Invert Enable */
#define PDEC_CTRLA_PINVEN1          (_U_(1) << PDEC_CTRLA_PINVEN1_Pos)
#define PDEC_CTRLA_PINVEN2_Pos      22           /**< \brief (PDEC_CTRLA) IO Pin 2 Invert Enable */
#define PDEC_CTRLA_PINVEN2          (_U_(1) << PDEC_CTRLA_PINVEN2_Pos)
#define PDEC_CTRLA_PINVEN_Pos       20           /**< \brief (PDEC_CTRLA) IO Pin x Invert Enable */
#define PDEC_CTRLA_PINVEN_Msk       (_U_(0x7) << PDEC_CTRLA_PINVEN_Pos)
#define PDEC_CTRLA_PINVEN(value)    (PDEC_CTRLA_PINVEN_Msk & ((value) << PDEC_CTRLA_PINVEN_Pos))
#define PDEC_CTRLA_ANGULAR_Pos      24           /**< \brief (PDEC_CTRLA) Angular Counter Length */
#define PDEC_CTRLA_ANGULAR_Msk      (_U_(0x7) << PDEC_CTRLA_ANGULAR_Pos)
#define PDEC_CTRLA_ANGULAR(value)   (PDEC_CTRLA_ANGULAR_Msk & ((value) << PDEC_CTRLA_ANGULAR_Pos))
#define PDEC_CTRLA_MAXCMP_Pos       28           /**< \brief (PDEC_CTRLA) Maximum Consecutive Missing Pulses */
#define PDEC_CTRLA_MAXCMP_Msk       (_U_(0xF) << PDEC_CTRLA_MAXCMP_Pos)
#define PDEC_CTRLA_MAXCMP(value)    (PDEC_CTRLA_MAXCMP_Msk & ((value) << PDEC_CTRLA_MAXCMP_Pos))
#define PDEC_CTRLA_MASK             _U_(0xF777CF4F) /**< \brief (PDEC_CTRLA) MASK Register */

/* -------- PDEC_CTRLBCLR : (PDEC Offset: 0x04) (R/W  8) Control B Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :1;               /*!< bit:      0  Reserved                           */
    uint8_t  LUPD:1;           /*!< bit:      1  Lock Update                        */
    uint8_t  :3;               /*!< bit:  2.. 4  Reserved                           */
    uint8_t  CMD:3;            /*!< bit:  5.. 7  Command                            */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PDEC_CTRLBCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_CTRLBCLR_OFFSET        0x04         /**< \brief (PDEC_CTRLBCLR offset) Control B Clear */
#define PDEC_CTRLBCLR_RESETVALUE    _U_(0x00)    /**< \brief (PDEC_CTRLBCLR reset_value) Control B Clear */

#define PDEC_CTRLBCLR_LUPD_Pos      1            /**< \brief (PDEC_CTRLBCLR) Lock Update */
#define PDEC_CTRLBCLR_LUPD          (_U_(0x1) << PDEC_CTRLBCLR_LUPD_Pos)
#define PDEC_CTRLBCLR_CMD_Pos       5            /**< \brief (PDEC_CTRLBCLR) Command */
#define PDEC_CTRLBCLR_CMD_Msk       (_U_(0x7) << PDEC_CTRLBCLR_CMD_Pos)
#define PDEC_CTRLBCLR_CMD(value)    (PDEC_CTRLBCLR_CMD_Msk & ((value) << PDEC_CTRLBCLR_CMD_Pos))
#define   PDEC_CTRLBCLR_CMD_NONE_Val      _U_(0x0)   /**< \brief (PDEC_CTRLBCLR) No action */
#define   PDEC_CTRLBCLR_CMD_RETRIGGER_Val _U_(0x1)   /**< \brief (PDEC_CTRLBCLR) Force a counter restart or retrigger */
#define   PDEC_CTRLBCLR_CMD_UPDATE_Val    _U_(0x2)   /**< \brief (PDEC_CTRLBCLR) Force update of double buffered registers */
#define   PDEC_CTRLBCLR_CMD_READSYNC_Val  _U_(0x3)   /**< \brief (PDEC_CTRLBCLR) Force a read synchronization of COUNT */
#define   PDEC_CTRLBCLR_CMD_START_Val     _U_(0x4)   /**< \brief (PDEC_CTRLBCLR) Start QDEC/HALL */
#define   PDEC_CTRLBCLR_CMD_STOP_Val      _U_(0x5)   /**< \brief (PDEC_CTRLBCLR) Stop QDEC/HALL */
#define PDEC_CTRLBCLR_CMD_NONE      (PDEC_CTRLBCLR_CMD_NONE_Val    << PDEC_CTRLBCLR_CMD_Pos)
#define PDEC_CTRLBCLR_CMD_RETRIGGER (PDEC_CTRLBCLR_CMD_RETRIGGER_Val << PDEC_CTRLBCLR_CMD_Pos)
#define PDEC_CTRLBCLR_CMD_UPDATE    (PDEC_CTRLBCLR_CMD_UPDATE_Val  << PDEC_CTRLBCLR_CMD_Pos)
#define PDEC_CTRLBCLR_CMD_READSYNC  (PDEC_CTRLBCLR_CMD_READSYNC_Val << PDEC_CTRLBCLR_CMD_Pos)
#define PDEC_CTRLBCLR_CMD_START     (PDEC_CTRLBCLR_CMD_START_Val   << PDEC_CTRLBCLR_CMD_Pos)
#define PDEC_CTRLBCLR_CMD_STOP      (PDEC_CTRLBCLR_CMD_STOP_Val    << PDEC_CTRLBCLR_CMD_Pos)
#define PDEC_CTRLBCLR_MASK          _U_(0xE2)    /**< \brief (PDEC_CTRLBCLR) MASK Register */

/* -------- PDEC_CTRLBSET : (PDEC Offset: 0x05) (R/W  8) Control B Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :1;               /*!< bit:      0  Reserved                           */
    uint8_t  LUPD:1;           /*!< bit:      1  Lock Update                        */
    uint8_t  :3;               /*!< bit:  2.. 4  Reserved                           */
    uint8_t  CMD:3;            /*!< bit:  5.. 7  Command                            */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PDEC_CTRLBSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_CTRLBSET_OFFSET        0x05         /**< \brief (PDEC_CTRLBSET offset) Control B Set */
#define PDEC_CTRLBSET_RESETVALUE    _U_(0x00)    /**< \brief (PDEC_CTRLBSET reset_value) Control B Set */

#define PDEC_CTRLBSET_LUPD_Pos      1            /**< \brief (PDEC_CTRLBSET) Lock Update */
#define PDEC_CTRLBSET_LUPD          (_U_(0x1) << PDEC_CTRLBSET_LUPD_Pos)
#define PDEC_CTRLBSET_CMD_Pos       5            /**< \brief (PDEC_CTRLBSET) Command */
#define PDEC_CTRLBSET_CMD_Msk       (_U_(0x7) << PDEC_CTRLBSET_CMD_Pos)
#define PDEC_CTRLBSET_CMD(value)    (PDEC_CTRLBSET_CMD_Msk & ((value) << PDEC_CTRLBSET_CMD_Pos))
#define   PDEC_CTRLBSET_CMD_NONE_Val      _U_(0x0)   /**< \brief (PDEC_CTRLBSET) No action */
#define   PDEC_CTRLBSET_CMD_RETRIGGER_Val _U_(0x1)   /**< \brief (PDEC_CTRLBSET) Force a counter restart or retrigger */
#define   PDEC_CTRLBSET_CMD_UPDATE_Val    _U_(0x2)   /**< \brief (PDEC_CTRLBSET) Force update of double buffered registers */
#define   PDEC_CTRLBSET_CMD_READSYNC_Val  _U_(0x3)   /**< \brief (PDEC_CTRLBSET) Force a read synchronization of COUNT */
#define   PDEC_CTRLBSET_CMD_START_Val     _U_(0x4)   /**< \brief (PDEC_CTRLBSET) Start QDEC/HALL */
#define   PDEC_CTRLBSET_CMD_STOP_Val      _U_(0x5)   /**< \brief (PDEC_CTRLBSET) Stop QDEC/HALL */
#define PDEC_CTRLBSET_CMD_NONE      (PDEC_CTRLBSET_CMD_NONE_Val    << PDEC_CTRLBSET_CMD_Pos)
#define PDEC_CTRLBSET_CMD_RETRIGGER (PDEC_CTRLBSET_CMD_RETRIGGER_Val << PDEC_CTRLBSET_CMD_Pos)
#define PDEC_CTRLBSET_CMD_UPDATE    (PDEC_CTRLBSET_CMD_UPDATE_Val  << PDEC_CTRLBSET_CMD_Pos)
#define PDEC_CTRLBSET_CMD_READSYNC  (PDEC_CTRLBSET_CMD_READSYNC_Val << PDEC_CTRLBSET_CMD_Pos)
#define PDEC_CTRLBSET_CMD_START     (PDEC_CTRLBSET_CMD_START_Val   << PDEC_CTRLBSET_CMD_Pos)
#define PDEC_CTRLBSET_CMD_STOP      (PDEC_CTRLBSET_CMD_STOP_Val    << PDEC_CTRLBSET_CMD_Pos)
#define PDEC_CTRLBSET_MASK          _U_(0xE2)    /**< \brief (PDEC_CTRLBSET) MASK Register */

/* -------- PDEC_EVCTRL : (PDEC Offset: 0x06) (R/W 16) Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t EVACT:2;          /*!< bit:  0.. 1  Event Action                       */
    uint16_t EVINV:3;          /*!< bit:  2.. 4  Inverted Event Input Enable        */
    uint16_t EVEI:3;           /*!< bit:  5.. 7  Event Input Enable                 */
    uint16_t OVFEO:1;          /*!< bit:      8  Overflow/Underflow Output Event Enable */
    uint16_t ERREO:1;          /*!< bit:      9  Error  Output Event Enable         */
    uint16_t DIREO:1;          /*!< bit:     10  Direction Output Event Enable      */
    uint16_t VLCEO:1;          /*!< bit:     11  Velocity Output Event Enable       */
    uint16_t MCEO0:1;          /*!< bit:     12  Match Channel 0 Event Output Enable */
    uint16_t MCEO1:1;          /*!< bit:     13  Match Channel 1 Event Output Enable */
    uint16_t :2;               /*!< bit: 14..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t :12;              /*!< bit:  0..11  Reserved                           */
    uint16_t MCEO:2;           /*!< bit: 12..13  Match Channel x Event Output Enable */
    uint16_t :2;               /*!< bit: 14..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} PDEC_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_EVCTRL_OFFSET          0x06         /**< \brief (PDEC_EVCTRL offset) Event Control */
#define PDEC_EVCTRL_RESETVALUE      _U_(0x0000)  /**< \brief (PDEC_EVCTRL reset_value) Event Control */

#define PDEC_EVCTRL_EVACT_Pos       0            /**< \brief (PDEC_EVCTRL) Event Action */
#define PDEC_EVCTRL_EVACT_Msk       (_U_(0x3) << PDEC_EVCTRL_EVACT_Pos)
#define PDEC_EVCTRL_EVACT(value)    (PDEC_EVCTRL_EVACT_Msk & ((value) << PDEC_EVCTRL_EVACT_Pos))
#define   PDEC_EVCTRL_EVACT_OFF_Val       _U_(0x0)   /**< \brief (PDEC_EVCTRL) Event action disabled */
#define   PDEC_EVCTRL_EVACT_RETRIGGER_Val _U_(0x1)   /**< \brief (PDEC_EVCTRL) Start, restart or retrigger on event */
#define   PDEC_EVCTRL_EVACT_COUNT_Val     _U_(0x2)   /**< \brief (PDEC_EVCTRL) Count on event */
#define PDEC_EVCTRL_EVACT_OFF       (PDEC_EVCTRL_EVACT_OFF_Val     << PDEC_EVCTRL_EVACT_Pos)
#define PDEC_EVCTRL_EVACT_RETRIGGER (PDEC_EVCTRL_EVACT_RETRIGGER_Val << PDEC_EVCTRL_EVACT_Pos)
#define PDEC_EVCTRL_EVACT_COUNT     (PDEC_EVCTRL_EVACT_COUNT_Val   << PDEC_EVCTRL_EVACT_Pos)
#define PDEC_EVCTRL_EVINV_Pos       2            /**< \brief (PDEC_EVCTRL) Inverted Event Input Enable */
#define PDEC_EVCTRL_EVINV_Msk       (_U_(0x7) << PDEC_EVCTRL_EVINV_Pos)
#define PDEC_EVCTRL_EVINV(value)    (PDEC_EVCTRL_EVINV_Msk & ((value) << PDEC_EVCTRL_EVINV_Pos))
#define PDEC_EVCTRL_EVEI_Pos        5            /**< \brief (PDEC_EVCTRL) Event Input Enable */
#define PDEC_EVCTRL_EVEI_Msk        (_U_(0x7) << PDEC_EVCTRL_EVEI_Pos)
#define PDEC_EVCTRL_EVEI(value)     (PDEC_EVCTRL_EVEI_Msk & ((value) << PDEC_EVCTRL_EVEI_Pos))
#define PDEC_EVCTRL_OVFEO_Pos       8            /**< \brief (PDEC_EVCTRL) Overflow/Underflow Output Event Enable */
#define PDEC_EVCTRL_OVFEO           (_U_(0x1) << PDEC_EVCTRL_OVFEO_Pos)
#define PDEC_EVCTRL_ERREO_Pos       9            /**< \brief (PDEC_EVCTRL) Error  Output Event Enable */
#define PDEC_EVCTRL_ERREO           (_U_(0x1) << PDEC_EVCTRL_ERREO_Pos)
#define PDEC_EVCTRL_DIREO_Pos       10           /**< \brief (PDEC_EVCTRL) Direction Output Event Enable */
#define PDEC_EVCTRL_DIREO           (_U_(0x1) << PDEC_EVCTRL_DIREO_Pos)
#define PDEC_EVCTRL_VLCEO_Pos       11           /**< \brief (PDEC_EVCTRL) Velocity Output Event Enable */
#define PDEC_EVCTRL_VLCEO           (_U_(0x1) << PDEC_EVCTRL_VLCEO_Pos)
#define PDEC_EVCTRL_MCEO0_Pos       12           /**< \brief (PDEC_EVCTRL) Match Channel 0 Event Output Enable */
#define PDEC_EVCTRL_MCEO0           (_U_(1) << PDEC_EVCTRL_MCEO0_Pos)
#define PDEC_EVCTRL_MCEO1_Pos       13           /**< \brief (PDEC_EVCTRL) Match Channel 1 Event Output Enable */
#define PDEC_EVCTRL_MCEO1           (_U_(1) << PDEC_EVCTRL_MCEO1_Pos)
#define PDEC_EVCTRL_MCEO_Pos        12           /**< \brief (PDEC_EVCTRL) Match Channel x Event Output Enable */
#define PDEC_EVCTRL_MCEO_Msk        (_U_(0x3) << PDEC_EVCTRL_MCEO_Pos)
#define PDEC_EVCTRL_MCEO(value)     (PDEC_EVCTRL_MCEO_Msk & ((value) << PDEC_EVCTRL_MCEO_Pos))
#define PDEC_EVCTRL_MASK            _U_(0x3FFF)  /**< \brief (PDEC_EVCTRL) MASK Register */

/* -------- PDEC_INTENCLR : (PDEC Offset: 0x08) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  OVF:1;            /*!< bit:      0  Overflow/Underflow Interrupt Disable */
    uint8_t  ERR:1;            /*!< bit:      1  Error Interrupt Disable            */
    uint8_t  DIR:1;            /*!< bit:      2  Direction Interrupt Disable        */
    uint8_t  VLC:1;            /*!< bit:      3  Velocity Interrupt Disable         */
    uint8_t  MC0:1;            /*!< bit:      4  Channel 0 Compare Match Disable    */
    uint8_t  MC1:1;            /*!< bit:      5  Channel 1 Compare Match Disable    */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  :4;               /*!< bit:  0.. 3  Reserved                           */
    uint8_t  MC:2;             /*!< bit:  4.. 5  Channel x Compare Match Disable    */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PDEC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_INTENCLR_OFFSET        0x08         /**< \brief (PDEC_INTENCLR offset) Interrupt Enable Clear */
#define PDEC_INTENCLR_RESETVALUE    _U_(0x00)    /**< \brief (PDEC_INTENCLR reset_value) Interrupt Enable Clear */

#define PDEC_INTENCLR_OVF_Pos       0            /**< \brief (PDEC_INTENCLR) Overflow/Underflow Interrupt Disable */
#define PDEC_INTENCLR_OVF           (_U_(0x1) << PDEC_INTENCLR_OVF_Pos)
#define PDEC_INTENCLR_ERR_Pos       1            /**< \brief (PDEC_INTENCLR) Error Interrupt Disable */
#define PDEC_INTENCLR_ERR           (_U_(0x1) << PDEC_INTENCLR_ERR_Pos)
#define PDEC_INTENCLR_DIR_Pos       2            /**< \brief (PDEC_INTENCLR) Direction Interrupt Disable */
#define PDEC_INTENCLR_DIR           (_U_(0x1) << PDEC_INTENCLR_DIR_Pos)
#define PDEC_INTENCLR_VLC_Pos       3            /**< \brief (PDEC_INTENCLR) Velocity Interrupt Disable */
#define PDEC_INTENCLR_VLC           (_U_(0x1) << PDEC_INTENCLR_VLC_Pos)
#define PDEC_INTENCLR_MC0_Pos       4            /**< \brief (PDEC_INTENCLR) Channel 0 Compare Match Disable */
#define PDEC_INTENCLR_MC0           (_U_(1) << PDEC_INTENCLR_MC0_Pos)
#define PDEC_INTENCLR_MC1_Pos       5            /**< \brief (PDEC_INTENCLR) Channel 1 Compare Match Disable */
#define PDEC_INTENCLR_MC1           (_U_(1) << PDEC_INTENCLR_MC1_Pos)
#define PDEC_INTENCLR_MC_Pos        4            /**< \brief (PDEC_INTENCLR) Channel x Compare Match Disable */
#define PDEC_INTENCLR_MC_Msk        (_U_(0x3) << PDEC_INTENCLR_MC_Pos)
#define PDEC_INTENCLR_MC(value)     (PDEC_INTENCLR_MC_Msk & ((value) << PDEC_INTENCLR_MC_Pos))
#define PDEC_INTENCLR_MASK          _U_(0x3F)    /**< \brief (PDEC_INTENCLR) MASK Register */

/* -------- PDEC_INTENSET : (PDEC Offset: 0x09) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  OVF:1;            /*!< bit:      0  Overflow/Underflow Interrupt Enable */
    uint8_t  ERR:1;            /*!< bit:      1  Error Interrupt Enable             */
    uint8_t  DIR:1;            /*!< bit:      2  Direction Interrupt Enable         */
    uint8_t  VLC:1;            /*!< bit:      3  Velocity Interrupt Enable          */
    uint8_t  MC0:1;            /*!< bit:      4  Channel 0 Compare Match Enable     */
    uint8_t  MC1:1;            /*!< bit:      5  Channel 1 Compare Match Enable     */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  :4;               /*!< bit:  0.. 3  Reserved                           */
    uint8_t  MC:2;             /*!< bit:  4.. 5  Channel x Compare Match Enable     */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PDEC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_INTENSET_OFFSET        0x09         /**< \brief (PDEC_INTENSET offset) Interrupt Enable Set */
#define PDEC_INTENSET_RESETVALUE    _U_(0x00)    /**< \brief (PDEC_INTENSET reset_value) Interrupt Enable Set */

#define PDEC_INTENSET_OVF_Pos       0            /**< \brief (PDEC_INTENSET) Overflow/Underflow Interrupt Enable */
#define PDEC_INTENSET_OVF           (_U_(0x1) << PDEC_INTENSET_OVF_Pos)
#define PDEC_INTENSET_ERR_Pos       1            /**< \brief (PDEC_INTENSET) Error Interrupt Enable */
#define PDEC_INTENSET_ERR           (_U_(0x1) << PDEC_INTENSET_ERR_Pos)
#define PDEC_INTENSET_DIR_Pos       2            /**< \brief (PDEC_INTENSET) Direction Interrupt Enable */
#define PDEC_INTENSET_DIR           (_U_(0x1) << PDEC_INTENSET_DIR_Pos)
#define PDEC_INTENSET_VLC_Pos       3            /**< \brief (PDEC_INTENSET) Velocity Interrupt Enable */
#define PDEC_INTENSET_VLC           (_U_(0x1) << PDEC_INTENSET_VLC_Pos)
#define PDEC_INTENSET_MC0_Pos       4            /**< \brief (PDEC_INTENSET) Channel 0 Compare Match Enable */
#define PDEC_INTENSET_MC0           (_U_(1) << PDEC_INTENSET_MC0_Pos)
#define PDEC_INTENSET_MC1_Pos       5            /**< \brief (PDEC_INTENSET) Channel 1 Compare Match Enable */
#define PDEC_INTENSET_MC1           (_U_(1) << PDEC_INTENSET_MC1_Pos)
#define PDEC_INTENSET_MC_Pos        4            /**< \brief (PDEC_INTENSET) Channel x Compare Match Enable */
#define PDEC_INTENSET_MC_Msk        (_U_(0x3) << PDEC_INTENSET_MC_Pos)
#define PDEC_INTENSET_MC(value)     (PDEC_INTENSET_MC_Msk & ((value) << PDEC_INTENSET_MC_Pos))
#define PDEC_INTENSET_MASK          _U_(0x3F)    /**< \brief (PDEC_INTENSET) MASK Register */

/* -------- PDEC_INTFLAG : (PDEC Offset: 0x0A) (R/W  8) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  OVF:1;            /*!< bit:      0  Overflow/Underflow                 */
    __I uint8_t  ERR:1;            /*!< bit:      1  Error                              */
    __I uint8_t  DIR:1;            /*!< bit:      2  Direction Change                   */
    __I uint8_t  VLC:1;            /*!< bit:      3  Velocity                           */
    __I uint8_t  MC0:1;            /*!< bit:      4  Channel 0 Compare Match            */
    __I uint8_t  MC1:1;            /*!< bit:      5  Channel 1 Compare Match            */
    __I uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint8_t  :4;               /*!< bit:  0.. 3  Reserved                           */
    __I uint8_t  MC:2;             /*!< bit:  4.. 5  Channel x Compare Match            */
    __I uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PDEC_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_INTFLAG_OFFSET         0x0A         /**< \brief (PDEC_INTFLAG offset) Interrupt Flag Status and Clear */
#define PDEC_INTFLAG_RESETVALUE     _U_(0x00)    /**< \brief (PDEC_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define PDEC_INTFLAG_OVF_Pos        0            /**< \brief (PDEC_INTFLAG) Overflow/Underflow */
#define PDEC_INTFLAG_OVF            (_U_(0x1) << PDEC_INTFLAG_OVF_Pos)
#define PDEC_INTFLAG_ERR_Pos        1            /**< \brief (PDEC_INTFLAG) Error */
#define PDEC_INTFLAG_ERR            (_U_(0x1) << PDEC_INTFLAG_ERR_Pos)
#define PDEC_INTFLAG_DIR_Pos        2            /**< \brief (PDEC_INTFLAG) Direction Change */
#define PDEC_INTFLAG_DIR            (_U_(0x1) << PDEC_INTFLAG_DIR_Pos)
#define PDEC_INTFLAG_VLC_Pos        3            /**< \brief (PDEC_INTFLAG) Velocity */
#define PDEC_INTFLAG_VLC            (_U_(0x1) << PDEC_INTFLAG_VLC_Pos)
#define PDEC_INTFLAG_MC0_Pos        4            /**< \brief (PDEC_INTFLAG) Channel 0 Compare Match */
#define PDEC_INTFLAG_MC0            (_U_(1) << PDEC_INTFLAG_MC0_Pos)
#define PDEC_INTFLAG_MC1_Pos        5            /**< \brief (PDEC_INTFLAG) Channel 1 Compare Match */
#define PDEC_INTFLAG_MC1            (_U_(1) << PDEC_INTFLAG_MC1_Pos)
#define PDEC_INTFLAG_MC_Pos         4            /**< \brief (PDEC_INTFLAG) Channel x Compare Match */
#define PDEC_INTFLAG_MC_Msk         (_U_(0x3) << PDEC_INTFLAG_MC_Pos)
#define PDEC_INTFLAG_MC(value)      (PDEC_INTFLAG_MC_Msk & ((value) << PDEC_INTFLAG_MC_Pos))
#define PDEC_INTFLAG_MASK           _U_(0x3F)    /**< \brief (PDEC_INTFLAG) MASK Register */

/* -------- PDEC_STATUS : (PDEC Offset: 0x0C) (R/W 16) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t QERR:1;           /*!< bit:      0  Quadrature Error Flag              */
    uint16_t IDXERR:1;         /*!< bit:      1  Index Error Flag                   */
    uint16_t MPERR:1;          /*!< bit:      2  Missing Pulse Error flag           */
    uint16_t :1;               /*!< bit:      3  Reserved                           */
    uint16_t WINERR:1;         /*!< bit:      4  Window Error Flag                  */
    uint16_t HERR:1;           /*!< bit:      5  Hall Error Flag                    */
    uint16_t STOP:1;           /*!< bit:      6  Stop                               */
    uint16_t DIR:1;            /*!< bit:      7  Direction Status Flag              */
    uint16_t PRESCBUFV:1;      /*!< bit:      8  Prescaler Buffer Valid             */
    uint16_t FILTERBUFV:1;     /*!< bit:      9  Filter Buffer Valid                */
    uint16_t :2;               /*!< bit: 10..11  Reserved                           */
    uint16_t CCBUFV0:1;        /*!< bit:     12  Compare Channel 0 Buffer Valid     */
    uint16_t CCBUFV1:1;        /*!< bit:     13  Compare Channel 1 Buffer Valid     */
    uint16_t :2;               /*!< bit: 14..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t :12;              /*!< bit:  0..11  Reserved                           */
    uint16_t CCBUFV:2;         /*!< bit: 12..13  Compare Channel x Buffer Valid     */
    uint16_t :2;               /*!< bit: 14..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} PDEC_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_STATUS_OFFSET          0x0C         /**< \brief (PDEC_STATUS offset) Status */
#define PDEC_STATUS_RESETVALUE      _U_(0x0040)  /**< \brief (PDEC_STATUS reset_value) Status */

#define PDEC_STATUS_QERR_Pos        0            /**< \brief (PDEC_STATUS) Quadrature Error Flag */
#define PDEC_STATUS_QERR            (_U_(0x1) << PDEC_STATUS_QERR_Pos)
#define PDEC_STATUS_IDXERR_Pos      1            /**< \brief (PDEC_STATUS) Index Error Flag */
#define PDEC_STATUS_IDXERR          (_U_(0x1) << PDEC_STATUS_IDXERR_Pos)
#define PDEC_STATUS_MPERR_Pos       2            /**< \brief (PDEC_STATUS) Missing Pulse Error flag */
#define PDEC_STATUS_MPERR           (_U_(0x1) << PDEC_STATUS_MPERR_Pos)
#define PDEC_STATUS_WINERR_Pos      4            /**< \brief (PDEC_STATUS) Window Error Flag */
#define PDEC_STATUS_WINERR          (_U_(0x1) << PDEC_STATUS_WINERR_Pos)
#define PDEC_STATUS_HERR_Pos        5            /**< \brief (PDEC_STATUS) Hall Error Flag */
#define PDEC_STATUS_HERR            (_U_(0x1) << PDEC_STATUS_HERR_Pos)
#define PDEC_STATUS_STOP_Pos        6            /**< \brief (PDEC_STATUS) Stop */
#define PDEC_STATUS_STOP            (_U_(0x1) << PDEC_STATUS_STOP_Pos)
#define PDEC_STATUS_DIR_Pos         7            /**< \brief (PDEC_STATUS) Direction Status Flag */
#define PDEC_STATUS_DIR             (_U_(0x1) << PDEC_STATUS_DIR_Pos)
#define PDEC_STATUS_PRESCBUFV_Pos   8            /**< \brief (PDEC_STATUS) Prescaler Buffer Valid */
#define PDEC_STATUS_PRESCBUFV       (_U_(0x1) << PDEC_STATUS_PRESCBUFV_Pos)
#define PDEC_STATUS_FILTERBUFV_Pos  9            /**< \brief (PDEC_STATUS) Filter Buffer Valid */
#define PDEC_STATUS_FILTERBUFV      (_U_(0x1) << PDEC_STATUS_FILTERBUFV_Pos)
#define PDEC_STATUS_CCBUFV0_Pos     12           /**< \brief (PDEC_STATUS) Compare Channel 0 Buffer Valid */
#define PDEC_STATUS_CCBUFV0         (_U_(1) << PDEC_STATUS_CCBUFV0_Pos)
#define PDEC_STATUS_CCBUFV1_Pos     13           /**< \brief (PDEC_STATUS) Compare Channel 1 Buffer Valid */
#define PDEC_STATUS_CCBUFV1         (_U_(1) << PDEC_STATUS_CCBUFV1_Pos)
#define PDEC_STATUS_CCBUFV_Pos      12           /**< \brief (PDEC_STATUS) Compare Channel x Buffer Valid */
#define PDEC_STATUS_CCBUFV_Msk      (_U_(0x3) << PDEC_STATUS_CCBUFV_Pos)
#define PDEC_STATUS_CCBUFV(value)   (PDEC_STATUS_CCBUFV_Msk & ((value) << PDEC_STATUS_CCBUFV_Pos))
#define PDEC_STATUS_MASK            _U_(0x33F7)  /**< \brief (PDEC_STATUS) MASK Register */

/* -------- PDEC_DBGCTRL : (PDEC Offset: 0x0F) (R/W  8) Debug Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DBGRUN:1;         /*!< bit:      0  Debug Run Mode                     */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PDEC_DBGCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_DBGCTRL_OFFSET         0x0F         /**< \brief (PDEC_DBGCTRL offset) Debug Control */
#define PDEC_DBGCTRL_RESETVALUE     _U_(0x00)    /**< \brief (PDEC_DBGCTRL reset_value) Debug Control */

#define PDEC_DBGCTRL_DBGRUN_Pos     0            /**< \brief (PDEC_DBGCTRL) Debug Run Mode */
#define PDEC_DBGCTRL_DBGRUN         (_U_(0x1) << PDEC_DBGCTRL_DBGRUN_Pos)
#define PDEC_DBGCTRL_MASK           _U_(0x01)    /**< \brief (PDEC_DBGCTRL) MASK Register */

/* -------- PDEC_SYNCBUSY : (PDEC Offset: 0x10) (R/  32) Synchronization Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset Synchronization Busy */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable Synchronization Busy        */
    uint32_t CTRLB:1;          /*!< bit:      2  Control B Synchronization Busy     */
    uint32_t STATUS:1;         /*!< bit:      3  Status Synchronization Busy        */
    uint32_t PRESC:1;          /*!< bit:      4  Prescaler Synchronization Busy     */
    uint32_t FILTER:1;         /*!< bit:      5  Filter Synchronization Busy        */
    uint32_t COUNT:1;          /*!< bit:      6  Count Synchronization Busy         */
    uint32_t CC0:1;            /*!< bit:      7  Compare Channel 0 Synchronization Busy */
    uint32_t CC1:1;            /*!< bit:      8  Compare Channel 1 Synchronization Busy */
    uint32_t :23;              /*!< bit:  9..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :7;               /*!< bit:  0.. 6  Reserved                           */
    uint32_t CC:2;             /*!< bit:  7.. 8  Compare Channel x Synchronization Busy */
    uint32_t :23;              /*!< bit:  9..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PDEC_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_SYNCBUSY_OFFSET        0x10         /**< \brief (PDEC_SYNCBUSY offset) Synchronization Status */
#define PDEC_SYNCBUSY_RESETVALUE    _U_(0x00000000) /**< \brief (PDEC_SYNCBUSY reset_value) Synchronization Status */

#define PDEC_SYNCBUSY_SWRST_Pos     0            /**< \brief (PDEC_SYNCBUSY) Software Reset Synchronization Busy */
#define PDEC_SYNCBUSY_SWRST         (_U_(0x1) << PDEC_SYNCBUSY_SWRST_Pos)
#define PDEC_SYNCBUSY_ENABLE_Pos    1            /**< \brief (PDEC_SYNCBUSY) Enable Synchronization Busy */
#define PDEC_SYNCBUSY_ENABLE        (_U_(0x1) << PDEC_SYNCBUSY_ENABLE_Pos)
#define PDEC_SYNCBUSY_CTRLB_Pos     2            /**< \brief (PDEC_SYNCBUSY) Control B Synchronization Busy */
#define PDEC_SYNCBUSY_CTRLB         (_U_(0x1) << PDEC_SYNCBUSY_CTRLB_Pos)
#define PDEC_SYNCBUSY_STATUS_Pos    3            /**< \brief (PDEC_SYNCBUSY) Status Synchronization Busy */
#define PDEC_SYNCBUSY_STATUS        (_U_(0x1) << PDEC_SYNCBUSY_STATUS_Pos)
#define PDEC_SYNCBUSY_PRESC_Pos     4            /**< \brief (PDEC_SYNCBUSY) Prescaler Synchronization Busy */
#define PDEC_SYNCBUSY_PRESC         (_U_(0x1) << PDEC_SYNCBUSY_PRESC_Pos)
#define PDEC_SYNCBUSY_FILTER_Pos    5            /**< \brief (PDEC_SYNCBUSY) Filter Synchronization Busy */
#define PDEC_SYNCBUSY_FILTER        (_U_(0x1) << PDEC_SYNCBUSY_FILTER_Pos)
#define PDEC_SYNCBUSY_COUNT_Pos     6            /**< \brief (PDEC_SYNCBUSY) Count Synchronization Busy */
#define PDEC_SYNCBUSY_COUNT         (_U_(0x1) << PDEC_SYNCBUSY_COUNT_Pos)
#define PDEC_SYNCBUSY_CC0_Pos       7            /**< \brief (PDEC_SYNCBUSY) Compare Channel 0 Synchronization Busy */
#define PDEC_SYNCBUSY_CC0           (_U_(1) << PDEC_SYNCBUSY_CC0_Pos)
#define PDEC_SYNCBUSY_CC1_Pos       8            /**< \brief (PDEC_SYNCBUSY) Compare Channel 1 Synchronization Busy */
#define PDEC_SYNCBUSY_CC1           (_U_(1) << PDEC_SYNCBUSY_CC1_Pos)
#define PDEC_SYNCBUSY_CC_Pos        7            /**< \brief (PDEC_SYNCBUSY) Compare Channel x Synchronization Busy */
#define PDEC_SYNCBUSY_CC_Msk        (_U_(0x3) << PDEC_SYNCBUSY_CC_Pos)
#define PDEC_SYNCBUSY_CC(value)     (PDEC_SYNCBUSY_CC_Msk & ((value) << PDEC_SYNCBUSY_CC_Pos))
#define PDEC_SYNCBUSY_MASK          _U_(0x000001FF) /**< \brief (PDEC_SYNCBUSY) MASK Register */

/* -------- PDEC_PRESC : (PDEC Offset: 0x14) (R/W  8) Prescaler Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  PRESC:4;          /*!< bit:  0.. 3  Prescaler Value                    */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PDEC_PRESC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_PRESC_OFFSET           0x14         /**< \brief (PDEC_PRESC offset) Prescaler Value */
#define PDEC_PRESC_RESETVALUE       _U_(0x00)    /**< \brief (PDEC_PRESC reset_value) Prescaler Value */

#define PDEC_PRESC_PRESC_Pos        0            /**< \brief (PDEC_PRESC) Prescaler Value */
#define PDEC_PRESC_PRESC_Msk        (_U_(0xF) << PDEC_PRESC_PRESC_Pos)
#define PDEC_PRESC_PRESC(value)     (PDEC_PRESC_PRESC_Msk & ((value) << PDEC_PRESC_PRESC_Pos))
#define   PDEC_PRESC_PRESC_DIV1_Val       _U_(0x0)   /**< \brief (PDEC_PRESC) No division */
#define   PDEC_PRESC_PRESC_DIV2_Val       _U_(0x1)   /**< \brief (PDEC_PRESC) Divide by 2 */
#define   PDEC_PRESC_PRESC_DIV4_Val       _U_(0x2)   /**< \brief (PDEC_PRESC) Divide by 4 */
#define   PDEC_PRESC_PRESC_DIV8_Val       _U_(0x3)   /**< \brief (PDEC_PRESC) Divide by 8 */
#define   PDEC_PRESC_PRESC_DIV16_Val      _U_(0x4)   /**< \brief (PDEC_PRESC) Divide by 16 */
#define   PDEC_PRESC_PRESC_DIV32_Val      _U_(0x5)   /**< \brief (PDEC_PRESC) Divide by 32 */
#define   PDEC_PRESC_PRESC_DIV64_Val      _U_(0x6)   /**< \brief (PDEC_PRESC) Divide by 64 */
#define   PDEC_PRESC_PRESC_DIV128_Val     _U_(0x7)   /**< \brief (PDEC_PRESC) Divide by 128 */
#define   PDEC_PRESC_PRESC_DIV256_Val     _U_(0x8)   /**< \brief (PDEC_PRESC) Divide by 256 */
#define   PDEC_PRESC_PRESC_DIV512_Val     _U_(0x9)   /**< \brief (PDEC_PRESC) Divide by 512 */
#define   PDEC_PRESC_PRESC_DIV1024_Val    _U_(0xA)   /**< \brief (PDEC_PRESC) Divide by 1024 */
#define PDEC_PRESC_PRESC_DIV1       (PDEC_PRESC_PRESC_DIV1_Val     << PDEC_PRESC_PRESC_Pos)
#define PDEC_PRESC_PRESC_DIV2       (PDEC_PRESC_PRESC_DIV2_Val     << PDEC_PRESC_PRESC_Pos)
#define PDEC_PRESC_PRESC_DIV4       (PDEC_PRESC_PRESC_DIV4_Val     << PDEC_PRESC_PRESC_Pos)
#define PDEC_PRESC_PRESC_DIV8       (PDEC_PRESC_PRESC_DIV8_Val     << PDEC_PRESC_PRESC_Pos)
#define PDEC_PRESC_PRESC_DIV16      (PDEC_PRESC_PRESC_DIV16_Val    << PDEC_PRESC_PRESC_Pos)
#define PDEC_PRESC_PRESC_DIV32      (PDEC_PRESC_PRESC_DIV32_Val    << PDEC_PRESC_PRESC_Pos)
#define PDEC_PRESC_PRESC_DIV64      (PDEC_PRESC_PRESC_DIV64_Val    << PDEC_PRESC_PRESC_Pos)
#define PDEC_PRESC_PRESC_DIV128     (PDEC_PRESC_PRESC_DIV128_Val   << PDEC_PRESC_PRESC_Pos)
#define PDEC_PRESC_PRESC_DIV256     (PDEC_PRESC_PRESC_DIV256_Val   << PDEC_PRESC_PRESC_Pos)
#define PDEC_PRESC_PRESC_DIV512     (PDEC_PRESC_PRESC_DIV512_Val   << PDEC_PRESC_PRESC_Pos)
#define PDEC_PRESC_PRESC_DIV1024    (PDEC_PRESC_PRESC_DIV1024_Val  << PDEC_PRESC_PRESC_Pos)
#define PDEC_PRESC_MASK             _U_(0x0F)    /**< \brief (PDEC_PRESC) MASK Register */

/* -------- PDEC_FILTER : (PDEC Offset: 0x15) (R/W  8) Filter Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  FILTER:8;         /*!< bit:  0.. 7  Filter Value                       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PDEC_FILTER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_FILTER_OFFSET          0x15         /**< \brief (PDEC_FILTER offset) Filter Value */
#define PDEC_FILTER_RESETVALUE      _U_(0x00)    /**< \brief (PDEC_FILTER reset_value) Filter Value */

#define PDEC_FILTER_FILTER_Pos      0            /**< \brief (PDEC_FILTER) Filter Value */
#define PDEC_FILTER_FILTER_Msk      (_U_(0xFF) << PDEC_FILTER_FILTER_Pos)
#define PDEC_FILTER_FILTER(value)   (PDEC_FILTER_FILTER_Msk & ((value) << PDEC_FILTER_FILTER_Pos))
#define PDEC_FILTER_MASK            _U_(0xFF)    /**< \brief (PDEC_FILTER) MASK Register */

/* -------- PDEC_PRESCBUF : (PDEC Offset: 0x18) (R/W  8) Prescaler Buffer Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  PRESCBUF:4;       /*!< bit:  0.. 3  Prescaler Buffer Value             */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PDEC_PRESCBUF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_PRESCBUF_OFFSET        0x18         /**< \brief (PDEC_PRESCBUF offset) Prescaler Buffer Value */
#define PDEC_PRESCBUF_RESETVALUE    _U_(0x00)    /**< \brief (PDEC_PRESCBUF reset_value) Prescaler Buffer Value */

#define PDEC_PRESCBUF_PRESCBUF_Pos  0            /**< \brief (PDEC_PRESCBUF) Prescaler Buffer Value */
#define PDEC_PRESCBUF_PRESCBUF_Msk  (_U_(0xF) << PDEC_PRESCBUF_PRESCBUF_Pos)
#define PDEC_PRESCBUF_PRESCBUF(value) (PDEC_PRESCBUF_PRESCBUF_Msk & ((value) << PDEC_PRESCBUF_PRESCBUF_Pos))
#define   PDEC_PRESCBUF_PRESCBUF_DIV1_Val _U_(0x0)   /**< \brief (PDEC_PRESCBUF) No division */
#define   PDEC_PRESCBUF_PRESCBUF_DIV2_Val _U_(0x1)   /**< \brief (PDEC_PRESCBUF) Divide by 2 */
#define   PDEC_PRESCBUF_PRESCBUF_DIV4_Val _U_(0x2)   /**< \brief (PDEC_PRESCBUF) Divide by 4 */
#define   PDEC_PRESCBUF_PRESCBUF_DIV8_Val _U_(0x3)   /**< \brief (PDEC_PRESCBUF) Divide by 8 */
#define   PDEC_PRESCBUF_PRESCBUF_DIV16_Val _U_(0x4)   /**< \brief (PDEC_PRESCBUF) Divide by 16 */
#define   PDEC_PRESCBUF_PRESCBUF_DIV32_Val _U_(0x5)   /**< \brief (PDEC_PRESCBUF) Divide by 32 */
#define   PDEC_PRESCBUF_PRESCBUF_DIV64_Val _U_(0x6)   /**< \brief (PDEC_PRESCBUF) Divide by 64 */
#define   PDEC_PRESCBUF_PRESCBUF_DIV128_Val _U_(0x7)   /**< \brief (PDEC_PRESCBUF) Divide by 128 */
#define   PDEC_PRESCBUF_PRESCBUF_DIV256_Val _U_(0x8)   /**< \brief (PDEC_PRESCBUF) Divide by 256 */
#define   PDEC_PRESCBUF_PRESCBUF_DIV512_Val _U_(0x9)   /**< \brief (PDEC_PRESCBUF) Divide by 512 */
#define   PDEC_PRESCBUF_PRESCBUF_DIV1024_Val _U_(0xA)   /**< \brief (PDEC_PRESCBUF) Divide by 1024 */
#define PDEC_PRESCBUF_PRESCBUF_DIV1 (PDEC_PRESCBUF_PRESCBUF_DIV1_Val << PDEC_PRESCBUF_PRESCBUF_Pos)
#define PDEC_PRESCBUF_PRESCBUF_DIV2 (PDEC_PRESCBUF_PRESCBUF_DIV2_Val << PDEC_PRESCBUF_PRESCBUF_Pos)
#define PDEC_PRESCBUF_PRESCBUF_DIV4 (PDEC_PRESCBUF_PRESCBUF_DIV4_Val << PDEC_PRESCBUF_PRESCBUF_Pos)
#define PDEC_PRESCBUF_PRESCBUF_DIV8 (PDEC_PRESCBUF_PRESCBUF_DIV8_Val << PDEC_PRESCBUF_PRESCBUF_Pos)
#define PDEC_PRESCBUF_PRESCBUF_DIV16 (PDEC_PRESCBUF_PRESCBUF_DIV16_Val << PDEC_PRESCBUF_PRESCBUF_Pos)
#define PDEC_PRESCBUF_PRESCBUF_DIV32 (PDEC_PRESCBUF_PRESCBUF_DIV32_Val << PDEC_PRESCBUF_PRESCBUF_Pos)
#define PDEC_PRESCBUF_PRESCBUF_DIV64 (PDEC_PRESCBUF_PRESCBUF_DIV64_Val << PDEC_PRESCBUF_PRESCBUF_Pos)
#define PDEC_PRESCBUF_PRESCBUF_DIV128 (PDEC_PRESCBUF_PRESCBUF_DIV128_Val << PDEC_PRESCBUF_PRESCBUF_Pos)
#define PDEC_PRESCBUF_PRESCBUF_DIV256 (PDEC_PRESCBUF_PRESCBUF_DIV256_Val << PDEC_PRESCBUF_PRESCBUF_Pos)
#define PDEC_PRESCBUF_PRESCBUF_DIV512 (PDEC_PRESCBUF_PRESCBUF_DIV512_Val << PDEC_PRESCBUF_PRESCBUF_Pos)
#define PDEC_PRESCBUF_PRESCBUF_DIV1024 (PDEC_PRESCBUF_PRESCBUF_DIV1024_Val << PDEC_PRESCBUF_PRESCBUF_Pos)
#define PDEC_PRESCBUF_MASK          _U_(0x0F)    /**< \brief (PDEC_PRESCBUF) MASK Register */

/* -------- PDEC_FILTERBUF : (PDEC Offset: 0x19) (R/W  8) Filter Buffer Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  FILTERBUF:8;      /*!< bit:  0.. 7  Filter Buffer Value                */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PDEC_FILTERBUF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_FILTERBUF_OFFSET       0x19         /**< \brief (PDEC_FILTERBUF offset) Filter Buffer Value */
#define PDEC_FILTERBUF_RESETVALUE   _U_(0x00)    /**< \brief (PDEC_FILTERBUF reset_value) Filter Buffer Value */

#define PDEC_FILTERBUF_FILTERBUF_Pos 0            /**< \brief (PDEC_FILTERBUF) Filter Buffer Value */
#define PDEC_FILTERBUF_FILTERBUF_Msk (_U_(0xFF) << PDEC_FILTERBUF_FILTERBUF_Pos)
#define PDEC_FILTERBUF_FILTERBUF(value) (PDEC_FILTERBUF_FILTERBUF_Msk & ((value) << PDEC_FILTERBUF_FILTERBUF_Pos))
#define PDEC_FILTERBUF_MASK         _U_(0xFF)    /**< \brief (PDEC_FILTERBUF) MASK Register */

/* -------- PDEC_COUNT : (PDEC Offset: 0x1C) (R/W 32) Counter Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t COUNT:16;         /*!< bit:  0..15  Counter Value                      */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PDEC_COUNT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_COUNT_OFFSET           0x1C         /**< \brief (PDEC_COUNT offset) Counter Value */
#define PDEC_COUNT_RESETVALUE       _U_(0x00000000) /**< \brief (PDEC_COUNT reset_value) Counter Value */

#define PDEC_COUNT_COUNT_Pos        0            /**< \brief (PDEC_COUNT) Counter Value */
#define PDEC_COUNT_COUNT_Msk        (_U_(0xFFFF) << PDEC_COUNT_COUNT_Pos)
#define PDEC_COUNT_COUNT(value)     (PDEC_COUNT_COUNT_Msk & ((value) << PDEC_COUNT_COUNT_Pos))
#define PDEC_COUNT_MASK             _U_(0x0000FFFF) /**< \brief (PDEC_COUNT) MASK Register */

/* -------- PDEC_CC : (PDEC Offset: 0x20) (R/W 32) Channel n Compare Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CC:16;            /*!< bit:  0..15  Channel Compare Value              */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PDEC_CC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_CC_OFFSET              0x20         /**< \brief (PDEC_CC offset) Channel n Compare Value */
#define PDEC_CC_RESETVALUE          _U_(0x00000000) /**< \brief (PDEC_CC reset_value) Channel n Compare Value */

#define PDEC_CC_CC_Pos              0            /**< \brief (PDEC_CC) Channel Compare Value */
#define PDEC_CC_CC_Msk              (_U_(0xFFFF) << PDEC_CC_CC_Pos)
#define PDEC_CC_CC(value)           (PDEC_CC_CC_Msk & ((value) << PDEC_CC_CC_Pos))
#define PDEC_CC_MASK                _U_(0x0000FFFF) /**< \brief (PDEC_CC) MASK Register */

/* -------- PDEC_CCBUF : (PDEC Offset: 0x30) (R/W 32) Channel Compare Buffer Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CCBUF:16;         /*!< bit:  0..15  Channel Compare Buffer Value       */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PDEC_CCBUF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PDEC_CCBUF_OFFSET           0x30         /**< \brief (PDEC_CCBUF offset) Channel Compare Buffer Value */
#define PDEC_CCBUF_RESETVALUE       _U_(0x00000000) /**< \brief (PDEC_CCBUF reset_value) Channel Compare Buffer Value */

#define PDEC_CCBUF_CCBUF_Pos        0            /**< \brief (PDEC_CCBUF) Channel Compare Buffer Value */
#define PDEC_CCBUF_CCBUF_Msk        (_U_(0xFFFF) << PDEC_CCBUF_CCBUF_Pos)
#define PDEC_CCBUF_CCBUF(value)     (PDEC_CCBUF_CCBUF_Msk & ((value) << PDEC_CCBUF_CCBUF_Pos))
#define PDEC_CCBUF_MASK             _U_(0x0000FFFF) /**< \brief (PDEC_CCBUF) MASK Register */

/** \brief PDEC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO PDEC_CTRLA_Type           CTRLA;       /**< \brief Offset: 0x00 (R/W 32) Control A */
  __IO PDEC_CTRLBCLR_Type        CTRLBCLR;    /**< \brief Offset: 0x04 (R/W  8) Control B Clear */
  __IO PDEC_CTRLBSET_Type        CTRLBSET;    /**< \brief Offset: 0x05 (R/W  8) Control B Set */
  __IO PDEC_EVCTRL_Type          EVCTRL;      /**< \brief Offset: 0x06 (R/W 16) Event Control */
  __IO PDEC_INTENCLR_Type        INTENCLR;    /**< \brief Offset: 0x08 (R/W  8) Interrupt Enable Clear */
  __IO PDEC_INTENSET_Type        INTENSET;    /**< \brief Offset: 0x09 (R/W  8) Interrupt Enable Set */
  __IO PDEC_INTFLAG_Type         INTFLAG;     /**< \brief Offset: 0x0A (R/W  8) Interrupt Flag Status and Clear */
       RoReg8                    Reserved1[0x1];
  __IO PDEC_STATUS_Type          STATUS;      /**< \brief Offset: 0x0C (R/W 16) Status */
       RoReg8                    Reserved2[0x1];
  __IO PDEC_DBGCTRL_Type         DBGCTRL;     /**< \brief Offset: 0x0F (R/W  8) Debug Control */
  __I  PDEC_SYNCBUSY_Type        SYNCBUSY;    /**< \brief Offset: 0x10 (R/  32) Synchronization Status */
  __IO PDEC_PRESC_Type           PRESC;       /**< \brief Offset: 0x14 (R/W  8) Prescaler Value */
  __IO PDEC_FILTER_Type          FILTER;      /**< \brief Offset: 0x15 (R/W  8) Filter Value */
       RoReg8                    Reserved3[0x2];
  __IO PDEC_PRESCBUF_Type        PRESCBUF;    /**< \brief Offset: 0x18 (R/W  8) Prescaler Buffer Value */
  __IO PDEC_FILTERBUF_Type       FILTERBUF;   /**< \brief Offset: 0x19 (R/W  8) Filter Buffer Value */
       RoReg8                    Reserved4[0x2];
  __IO PDEC_COUNT_Type           COUNT;       /**< \brief Offset: 0x1C (R/W 32) Counter Value */
  __IO PDEC_CC_Type              CC[2];       /**< \brief Offset: 0x20 (R/W 32) Channel n Compare Value */
       RoReg8                    Reserved5[0x8];
  __IO PDEC_CCBUF_Type           CCBUF[2];    /**< \brief Offset: 0x30 (R/W 32) Channel Compare Buffer Value */
} Pdec;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_PDEC_COMPONENT_ */
