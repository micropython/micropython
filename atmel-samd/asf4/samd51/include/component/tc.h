/**
 * \file
 *
 * \brief Component description for TC
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

#ifndef _SAMD51_TC_COMPONENT_
#define _SAMD51_TC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR TC */
/* ========================================================================== */
/** \addtogroup SAMD51_TC Basic Timer Counter */
/*@{*/

#define TC_U2249
#define REV_TC                      0x300

/* -------- TC_CTRLA : (TC Offset: 0x00) (R/W 32) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint32_t MODE:2;           /*!< bit:  2.. 3  Timer Counter Mode                 */
    uint32_t PRESCSYNC:2;      /*!< bit:  4.. 5  Prescaler and Counter Synchronization */
    uint32_t RUNSTDBY:1;       /*!< bit:      6  Run during Standby                 */
    uint32_t ONDEMAND:1;       /*!< bit:      7  Clock On Demand                    */
    uint32_t PRESCALER:3;      /*!< bit:  8..10  Prescaler                          */
    uint32_t ALOCK:1;          /*!< bit:     11  Auto Lock                          */
    uint32_t :4;               /*!< bit: 12..15  Reserved                           */
    uint32_t CAPTEN0:1;        /*!< bit:     16  Capture Channel 0 Enable           */
    uint32_t CAPTEN1:1;        /*!< bit:     17  Capture Channel 1 Enable           */
    uint32_t :2;               /*!< bit: 18..19  Reserved                           */
    uint32_t COPEN0:1;         /*!< bit:     20  Capture On Pin 0 Enable            */
    uint32_t COPEN1:1;         /*!< bit:     21  Capture On Pin 1 Enable            */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t CAPTMODE0:2;      /*!< bit: 24..25  Capture Mode Channel 0             */
    uint32_t :1;               /*!< bit:     26  Reserved                           */
    uint32_t CAPTMODE1:2;      /*!< bit: 27..28  Capture mode Channel 1             */
    uint32_t :3;               /*!< bit: 29..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :16;              /*!< bit:  0..15  Reserved                           */
    uint32_t CAPTEN:2;         /*!< bit: 16..17  Capture Channel x Enable           */
    uint32_t :2;               /*!< bit: 18..19  Reserved                           */
    uint32_t COPEN:2;          /*!< bit: 20..21  Capture On Pin x Enable            */
    uint32_t :10;              /*!< bit: 22..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TC_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_CTRLA_OFFSET             0x00         /**< \brief (TC_CTRLA offset) Control A */
#define TC_CTRLA_RESETVALUE         _U_(0x00000000) /**< \brief (TC_CTRLA reset_value) Control A */

#define TC_CTRLA_SWRST_Pos          0            /**< \brief (TC_CTRLA) Software Reset */
#define TC_CTRLA_SWRST              (_U_(0x1) << TC_CTRLA_SWRST_Pos)
#define TC_CTRLA_ENABLE_Pos         1            /**< \brief (TC_CTRLA) Enable */
#define TC_CTRLA_ENABLE             (_U_(0x1) << TC_CTRLA_ENABLE_Pos)
#define TC_CTRLA_MODE_Pos           2            /**< \brief (TC_CTRLA) Timer Counter Mode */
#define TC_CTRLA_MODE_Msk           (_U_(0x3) << TC_CTRLA_MODE_Pos)
#define TC_CTRLA_MODE(value)        (TC_CTRLA_MODE_Msk & ((value) << TC_CTRLA_MODE_Pos))
#define   TC_CTRLA_MODE_COUNT16_Val       _U_(0x0)   /**< \brief (TC_CTRLA) Counter in 16-bit mode */
#define   TC_CTRLA_MODE_COUNT8_Val        _U_(0x1)   /**< \brief (TC_CTRLA) Counter in 8-bit mode */
#define   TC_CTRLA_MODE_COUNT32_Val       _U_(0x2)   /**< \brief (TC_CTRLA) Counter in 32-bit mode */
#define TC_CTRLA_MODE_COUNT16       (TC_CTRLA_MODE_COUNT16_Val     << TC_CTRLA_MODE_Pos)
#define TC_CTRLA_MODE_COUNT8        (TC_CTRLA_MODE_COUNT8_Val      << TC_CTRLA_MODE_Pos)
#define TC_CTRLA_MODE_COUNT32       (TC_CTRLA_MODE_COUNT32_Val     << TC_CTRLA_MODE_Pos)
#define TC_CTRLA_PRESCSYNC_Pos      4            /**< \brief (TC_CTRLA) Prescaler and Counter Synchronization */
#define TC_CTRLA_PRESCSYNC_Msk      (_U_(0x3) << TC_CTRLA_PRESCSYNC_Pos)
#define TC_CTRLA_PRESCSYNC(value)   (TC_CTRLA_PRESCSYNC_Msk & ((value) << TC_CTRLA_PRESCSYNC_Pos))
#define   TC_CTRLA_PRESCSYNC_GCLK_Val     _U_(0x0)   /**< \brief (TC_CTRLA) Reload or reset the counter on next generic clock */
#define   TC_CTRLA_PRESCSYNC_PRESC_Val    _U_(0x1)   /**< \brief (TC_CTRLA) Reload or reset the counter on next prescaler clock */
#define   TC_CTRLA_PRESCSYNC_RESYNC_Val   _U_(0x2)   /**< \brief (TC_CTRLA) Reload or reset the counter on next generic clock and reset the prescaler counter */
#define TC_CTRLA_PRESCSYNC_GCLK     (TC_CTRLA_PRESCSYNC_GCLK_Val   << TC_CTRLA_PRESCSYNC_Pos)
#define TC_CTRLA_PRESCSYNC_PRESC    (TC_CTRLA_PRESCSYNC_PRESC_Val  << TC_CTRLA_PRESCSYNC_Pos)
#define TC_CTRLA_PRESCSYNC_RESYNC   (TC_CTRLA_PRESCSYNC_RESYNC_Val << TC_CTRLA_PRESCSYNC_Pos)
#define TC_CTRLA_RUNSTDBY_Pos       6            /**< \brief (TC_CTRLA) Run during Standby */
#define TC_CTRLA_RUNSTDBY           (_U_(0x1) << TC_CTRLA_RUNSTDBY_Pos)
#define TC_CTRLA_ONDEMAND_Pos       7            /**< \brief (TC_CTRLA) Clock On Demand */
#define TC_CTRLA_ONDEMAND           (_U_(0x1) << TC_CTRLA_ONDEMAND_Pos)
#define TC_CTRLA_PRESCALER_Pos      8            /**< \brief (TC_CTRLA) Prescaler */
#define TC_CTRLA_PRESCALER_Msk      (_U_(0x7) << TC_CTRLA_PRESCALER_Pos)
#define TC_CTRLA_PRESCALER(value)   (TC_CTRLA_PRESCALER_Msk & ((value) << TC_CTRLA_PRESCALER_Pos))
#define   TC_CTRLA_PRESCALER_DIV1_Val     _U_(0x0)   /**< \brief (TC_CTRLA) Prescaler: GCLK_TC */
#define   TC_CTRLA_PRESCALER_DIV2_Val     _U_(0x1)   /**< \brief (TC_CTRLA) Prescaler: GCLK_TC/2 */
#define   TC_CTRLA_PRESCALER_DIV4_Val     _U_(0x2)   /**< \brief (TC_CTRLA) Prescaler: GCLK_TC/4 */
#define   TC_CTRLA_PRESCALER_DIV8_Val     _U_(0x3)   /**< \brief (TC_CTRLA) Prescaler: GCLK_TC/8 */
#define   TC_CTRLA_PRESCALER_DIV16_Val    _U_(0x4)   /**< \brief (TC_CTRLA) Prescaler: GCLK_TC/16 */
#define   TC_CTRLA_PRESCALER_DIV64_Val    _U_(0x5)   /**< \brief (TC_CTRLA) Prescaler: GCLK_TC/64 */
#define   TC_CTRLA_PRESCALER_DIV256_Val   _U_(0x6)   /**< \brief (TC_CTRLA) Prescaler: GCLK_TC/256 */
#define   TC_CTRLA_PRESCALER_DIV1024_Val  _U_(0x7)   /**< \brief (TC_CTRLA) Prescaler: GCLK_TC/1024 */
#define TC_CTRLA_PRESCALER_DIV1     (TC_CTRLA_PRESCALER_DIV1_Val   << TC_CTRLA_PRESCALER_Pos)
#define TC_CTRLA_PRESCALER_DIV2     (TC_CTRLA_PRESCALER_DIV2_Val   << TC_CTRLA_PRESCALER_Pos)
#define TC_CTRLA_PRESCALER_DIV4     (TC_CTRLA_PRESCALER_DIV4_Val   << TC_CTRLA_PRESCALER_Pos)
#define TC_CTRLA_PRESCALER_DIV8     (TC_CTRLA_PRESCALER_DIV8_Val   << TC_CTRLA_PRESCALER_Pos)
#define TC_CTRLA_PRESCALER_DIV16    (TC_CTRLA_PRESCALER_DIV16_Val  << TC_CTRLA_PRESCALER_Pos)
#define TC_CTRLA_PRESCALER_DIV64    (TC_CTRLA_PRESCALER_DIV64_Val  << TC_CTRLA_PRESCALER_Pos)
#define TC_CTRLA_PRESCALER_DIV256   (TC_CTRLA_PRESCALER_DIV256_Val << TC_CTRLA_PRESCALER_Pos)
#define TC_CTRLA_PRESCALER_DIV1024  (TC_CTRLA_PRESCALER_DIV1024_Val << TC_CTRLA_PRESCALER_Pos)
#define TC_CTRLA_ALOCK_Pos          11           /**< \brief (TC_CTRLA) Auto Lock */
#define TC_CTRLA_ALOCK              (_U_(0x1) << TC_CTRLA_ALOCK_Pos)
#define TC_CTRLA_CAPTEN0_Pos        16           /**< \brief (TC_CTRLA) Capture Channel 0 Enable */
#define TC_CTRLA_CAPTEN0            (_U_(1) << TC_CTRLA_CAPTEN0_Pos)
#define TC_CTRLA_CAPTEN1_Pos        17           /**< \brief (TC_CTRLA) Capture Channel 1 Enable */
#define TC_CTRLA_CAPTEN1            (_U_(1) << TC_CTRLA_CAPTEN1_Pos)
#define TC_CTRLA_CAPTEN_Pos         16           /**< \brief (TC_CTRLA) Capture Channel x Enable */
#define TC_CTRLA_CAPTEN_Msk         (_U_(0x3) << TC_CTRLA_CAPTEN_Pos)
#define TC_CTRLA_CAPTEN(value)      (TC_CTRLA_CAPTEN_Msk & ((value) << TC_CTRLA_CAPTEN_Pos))
#define TC_CTRLA_COPEN0_Pos         20           /**< \brief (TC_CTRLA) Capture On Pin 0 Enable */
#define TC_CTRLA_COPEN0             (_U_(1) << TC_CTRLA_COPEN0_Pos)
#define TC_CTRLA_COPEN1_Pos         21           /**< \brief (TC_CTRLA) Capture On Pin 1 Enable */
#define TC_CTRLA_COPEN1             (_U_(1) << TC_CTRLA_COPEN1_Pos)
#define TC_CTRLA_COPEN_Pos          20           /**< \brief (TC_CTRLA) Capture On Pin x Enable */
#define TC_CTRLA_COPEN_Msk          (_U_(0x3) << TC_CTRLA_COPEN_Pos)
#define TC_CTRLA_COPEN(value)       (TC_CTRLA_COPEN_Msk & ((value) << TC_CTRLA_COPEN_Pos))
#define TC_CTRLA_CAPTMODE0_Pos      24           /**< \brief (TC_CTRLA) Capture Mode Channel 0 */
#define TC_CTRLA_CAPTMODE0_Msk      (_U_(0x3) << TC_CTRLA_CAPTMODE0_Pos)
#define TC_CTRLA_CAPTMODE0(value)   (TC_CTRLA_CAPTMODE0_Msk & ((value) << TC_CTRLA_CAPTMODE0_Pos))
#define   TC_CTRLA_CAPTMODE0_DEFAULT_Val  _U_(0x0)   /**< \brief (TC_CTRLA) Default capture */
#define   TC_CTRLA_CAPTMODE0_CAPTMIN_Val  _U_(0x1)   /**< \brief (TC_CTRLA) Minimum capture */
#define   TC_CTRLA_CAPTMODE0_CAPTMAX_Val  _U_(0x2)   /**< \brief (TC_CTRLA) Maximum capture */
#define TC_CTRLA_CAPTMODE0_DEFAULT  (TC_CTRLA_CAPTMODE0_DEFAULT_Val << TC_CTRLA_CAPTMODE0_Pos)
#define TC_CTRLA_CAPTMODE0_CAPTMIN  (TC_CTRLA_CAPTMODE0_CAPTMIN_Val << TC_CTRLA_CAPTMODE0_Pos)
#define TC_CTRLA_CAPTMODE0_CAPTMAX  (TC_CTRLA_CAPTMODE0_CAPTMAX_Val << TC_CTRLA_CAPTMODE0_Pos)
#define TC_CTRLA_CAPTMODE1_Pos      27           /**< \brief (TC_CTRLA) Capture mode Channel 1 */
#define TC_CTRLA_CAPTMODE1_Msk      (_U_(0x3) << TC_CTRLA_CAPTMODE1_Pos)
#define TC_CTRLA_CAPTMODE1(value)   (TC_CTRLA_CAPTMODE1_Msk & ((value) << TC_CTRLA_CAPTMODE1_Pos))
#define   TC_CTRLA_CAPTMODE1_DEFAULT_Val  _U_(0x0)   /**< \brief (TC_CTRLA) Default capture */
#define   TC_CTRLA_CAPTMODE1_CAPTMIN_Val  _U_(0x1)   /**< \brief (TC_CTRLA) Minimum capture */
#define   TC_CTRLA_CAPTMODE1_CAPTMAX_Val  _U_(0x2)   /**< \brief (TC_CTRLA) Maximum capture */
#define TC_CTRLA_CAPTMODE1_DEFAULT  (TC_CTRLA_CAPTMODE1_DEFAULT_Val << TC_CTRLA_CAPTMODE1_Pos)
#define TC_CTRLA_CAPTMODE1_CAPTMIN  (TC_CTRLA_CAPTMODE1_CAPTMIN_Val << TC_CTRLA_CAPTMODE1_Pos)
#define TC_CTRLA_CAPTMODE1_CAPTMAX  (TC_CTRLA_CAPTMODE1_CAPTMAX_Val << TC_CTRLA_CAPTMODE1_Pos)
#define TC_CTRLA_MASK               _U_(0x1B330FFF) /**< \brief (TC_CTRLA) MASK Register */

/* -------- TC_CTRLBCLR : (TC Offset: 0x04) (R/W  8) Control B Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DIR:1;            /*!< bit:      0  Counter Direction                  */
    uint8_t  LUPD:1;           /*!< bit:      1  Lock Update                        */
    uint8_t  ONESHOT:1;        /*!< bit:      2  One-Shot on Counter                */
    uint8_t  :2;               /*!< bit:  3.. 4  Reserved                           */
    uint8_t  CMD:3;            /*!< bit:  5.. 7  Command                            */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TC_CTRLBCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_CTRLBCLR_OFFSET          0x04         /**< \brief (TC_CTRLBCLR offset) Control B Clear */
#define TC_CTRLBCLR_RESETVALUE      _U_(0x00)    /**< \brief (TC_CTRLBCLR reset_value) Control B Clear */

#define TC_CTRLBCLR_DIR_Pos         0            /**< \brief (TC_CTRLBCLR) Counter Direction */
#define TC_CTRLBCLR_DIR             (_U_(0x1) << TC_CTRLBCLR_DIR_Pos)
#define TC_CTRLBCLR_LUPD_Pos        1            /**< \brief (TC_CTRLBCLR) Lock Update */
#define TC_CTRLBCLR_LUPD            (_U_(0x1) << TC_CTRLBCLR_LUPD_Pos)
#define TC_CTRLBCLR_ONESHOT_Pos     2            /**< \brief (TC_CTRLBCLR) One-Shot on Counter */
#define TC_CTRLBCLR_ONESHOT         (_U_(0x1) << TC_CTRLBCLR_ONESHOT_Pos)
#define TC_CTRLBCLR_CMD_Pos         5            /**< \brief (TC_CTRLBCLR) Command */
#define TC_CTRLBCLR_CMD_Msk         (_U_(0x7) << TC_CTRLBCLR_CMD_Pos)
#define TC_CTRLBCLR_CMD(value)      (TC_CTRLBCLR_CMD_Msk & ((value) << TC_CTRLBCLR_CMD_Pos))
#define   TC_CTRLBCLR_CMD_NONE_Val        _U_(0x0)   /**< \brief (TC_CTRLBCLR) No action */
#define   TC_CTRLBCLR_CMD_RETRIGGER_Val   _U_(0x1)   /**< \brief (TC_CTRLBCLR) Force a start, restart or retrigger */
#define   TC_CTRLBCLR_CMD_STOP_Val        _U_(0x2)   /**< \brief (TC_CTRLBCLR) Force a stop */
#define   TC_CTRLBCLR_CMD_UPDATE_Val      _U_(0x3)   /**< \brief (TC_CTRLBCLR) Force update of double-buffered register */
#define   TC_CTRLBCLR_CMD_READSYNC_Val    _U_(0x4)   /**< \brief (TC_CTRLBCLR) Force a read synchronization of COUNT */
#define   TC_CTRLBCLR_CMD_DMAOS_Val       _U_(0x5)   /**< \brief (TC_CTRLBCLR) One-shot DMA trigger */
#define TC_CTRLBCLR_CMD_NONE        (TC_CTRLBCLR_CMD_NONE_Val      << TC_CTRLBCLR_CMD_Pos)
#define TC_CTRLBCLR_CMD_RETRIGGER   (TC_CTRLBCLR_CMD_RETRIGGER_Val << TC_CTRLBCLR_CMD_Pos)
#define TC_CTRLBCLR_CMD_STOP        (TC_CTRLBCLR_CMD_STOP_Val      << TC_CTRLBCLR_CMD_Pos)
#define TC_CTRLBCLR_CMD_UPDATE      (TC_CTRLBCLR_CMD_UPDATE_Val    << TC_CTRLBCLR_CMD_Pos)
#define TC_CTRLBCLR_CMD_READSYNC    (TC_CTRLBCLR_CMD_READSYNC_Val  << TC_CTRLBCLR_CMD_Pos)
#define TC_CTRLBCLR_CMD_DMAOS       (TC_CTRLBCLR_CMD_DMAOS_Val     << TC_CTRLBCLR_CMD_Pos)
#define TC_CTRLBCLR_MASK            _U_(0xE7)    /**< \brief (TC_CTRLBCLR) MASK Register */

/* -------- TC_CTRLBSET : (TC Offset: 0x05) (R/W  8) Control B Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DIR:1;            /*!< bit:      0  Counter Direction                  */
    uint8_t  LUPD:1;           /*!< bit:      1  Lock Update                        */
    uint8_t  ONESHOT:1;        /*!< bit:      2  One-Shot on Counter                */
    uint8_t  :2;               /*!< bit:  3.. 4  Reserved                           */
    uint8_t  CMD:3;            /*!< bit:  5.. 7  Command                            */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TC_CTRLBSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_CTRLBSET_OFFSET          0x05         /**< \brief (TC_CTRLBSET offset) Control B Set */
#define TC_CTRLBSET_RESETVALUE      _U_(0x00)    /**< \brief (TC_CTRLBSET reset_value) Control B Set */

#define TC_CTRLBSET_DIR_Pos         0            /**< \brief (TC_CTRLBSET) Counter Direction */
#define TC_CTRLBSET_DIR             (_U_(0x1) << TC_CTRLBSET_DIR_Pos)
#define TC_CTRLBSET_LUPD_Pos        1            /**< \brief (TC_CTRLBSET) Lock Update */
#define TC_CTRLBSET_LUPD            (_U_(0x1) << TC_CTRLBSET_LUPD_Pos)
#define TC_CTRLBSET_ONESHOT_Pos     2            /**< \brief (TC_CTRLBSET) One-Shot on Counter */
#define TC_CTRLBSET_ONESHOT         (_U_(0x1) << TC_CTRLBSET_ONESHOT_Pos)
#define TC_CTRLBSET_CMD_Pos         5            /**< \brief (TC_CTRLBSET) Command */
#define TC_CTRLBSET_CMD_Msk         (_U_(0x7) << TC_CTRLBSET_CMD_Pos)
#define TC_CTRLBSET_CMD(value)      (TC_CTRLBSET_CMD_Msk & ((value) << TC_CTRLBSET_CMD_Pos))
#define   TC_CTRLBSET_CMD_NONE_Val        _U_(0x0)   /**< \brief (TC_CTRLBSET) No action */
#define   TC_CTRLBSET_CMD_RETRIGGER_Val   _U_(0x1)   /**< \brief (TC_CTRLBSET) Force a start, restart or retrigger */
#define   TC_CTRLBSET_CMD_STOP_Val        _U_(0x2)   /**< \brief (TC_CTRLBSET) Force a stop */
#define   TC_CTRLBSET_CMD_UPDATE_Val      _U_(0x3)   /**< \brief (TC_CTRLBSET) Force update of double-buffered register */
#define   TC_CTRLBSET_CMD_READSYNC_Val    _U_(0x4)   /**< \brief (TC_CTRLBSET) Force a read synchronization of COUNT */
#define   TC_CTRLBSET_CMD_DMAOS_Val       _U_(0x5)   /**< \brief (TC_CTRLBSET) One-shot DMA trigger */
#define TC_CTRLBSET_CMD_NONE        (TC_CTRLBSET_CMD_NONE_Val      << TC_CTRLBSET_CMD_Pos)
#define TC_CTRLBSET_CMD_RETRIGGER   (TC_CTRLBSET_CMD_RETRIGGER_Val << TC_CTRLBSET_CMD_Pos)
#define TC_CTRLBSET_CMD_STOP        (TC_CTRLBSET_CMD_STOP_Val      << TC_CTRLBSET_CMD_Pos)
#define TC_CTRLBSET_CMD_UPDATE      (TC_CTRLBSET_CMD_UPDATE_Val    << TC_CTRLBSET_CMD_Pos)
#define TC_CTRLBSET_CMD_READSYNC    (TC_CTRLBSET_CMD_READSYNC_Val  << TC_CTRLBSET_CMD_Pos)
#define TC_CTRLBSET_CMD_DMAOS       (TC_CTRLBSET_CMD_DMAOS_Val     << TC_CTRLBSET_CMD_Pos)
#define TC_CTRLBSET_MASK            _U_(0xE7)    /**< \brief (TC_CTRLBSET) MASK Register */

/* -------- TC_EVCTRL : (TC Offset: 0x06) (R/W 16) Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t EVACT:3;          /*!< bit:  0.. 2  Event Action                       */
    uint16_t :1;               /*!< bit:      3  Reserved                           */
    uint16_t TCINV:1;          /*!< bit:      4  TC Event Input Polarity            */
    uint16_t TCEI:1;           /*!< bit:      5  TC Event Enable                    */
    uint16_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint16_t OVFEO:1;          /*!< bit:      8  Event Output Enable                */
    uint16_t :3;               /*!< bit:  9..11  Reserved                           */
    uint16_t MCEO0:1;          /*!< bit:     12  MC Event Output Enable 0           */
    uint16_t MCEO1:1;          /*!< bit:     13  MC Event Output Enable 1           */
    uint16_t :2;               /*!< bit: 14..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t :12;              /*!< bit:  0..11  Reserved                           */
    uint16_t MCEO:2;           /*!< bit: 12..13  MC Event Output Enable x           */
    uint16_t :2;               /*!< bit: 14..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} TC_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_EVCTRL_OFFSET            0x06         /**< \brief (TC_EVCTRL offset) Event Control */
#define TC_EVCTRL_RESETVALUE        _U_(0x0000)  /**< \brief (TC_EVCTRL reset_value) Event Control */

#define TC_EVCTRL_EVACT_Pos         0            /**< \brief (TC_EVCTRL) Event Action */
#define TC_EVCTRL_EVACT_Msk         (_U_(0x7) << TC_EVCTRL_EVACT_Pos)
#define TC_EVCTRL_EVACT(value)      (TC_EVCTRL_EVACT_Msk & ((value) << TC_EVCTRL_EVACT_Pos))
#define   TC_EVCTRL_EVACT_OFF_Val         _U_(0x0)   /**< \brief (TC_EVCTRL) Event action disabled */
#define   TC_EVCTRL_EVACT_RETRIGGER_Val   _U_(0x1)   /**< \brief (TC_EVCTRL) Start, restart or retrigger TC on event */
#define   TC_EVCTRL_EVACT_COUNT_Val       _U_(0x2)   /**< \brief (TC_EVCTRL) Count on event */
#define   TC_EVCTRL_EVACT_START_Val       _U_(0x3)   /**< \brief (TC_EVCTRL) Start TC on event */
#define   TC_EVCTRL_EVACT_STAMP_Val       _U_(0x4)   /**< \brief (TC_EVCTRL) Time stamp capture */
#define   TC_EVCTRL_EVACT_PPW_Val         _U_(0x5)   /**< \brief (TC_EVCTRL) Period catured in CC0, pulse width in CC1 */
#define   TC_EVCTRL_EVACT_PWP_Val         _U_(0x6)   /**< \brief (TC_EVCTRL) Period catured in CC1, pulse width in CC0 */
#define   TC_EVCTRL_EVACT_PW_Val          _U_(0x7)   /**< \brief (TC_EVCTRL) Pulse width capture */
#define TC_EVCTRL_EVACT_OFF         (TC_EVCTRL_EVACT_OFF_Val       << TC_EVCTRL_EVACT_Pos)
#define TC_EVCTRL_EVACT_RETRIGGER   (TC_EVCTRL_EVACT_RETRIGGER_Val << TC_EVCTRL_EVACT_Pos)
#define TC_EVCTRL_EVACT_COUNT       (TC_EVCTRL_EVACT_COUNT_Val     << TC_EVCTRL_EVACT_Pos)
#define TC_EVCTRL_EVACT_START       (TC_EVCTRL_EVACT_START_Val     << TC_EVCTRL_EVACT_Pos)
#define TC_EVCTRL_EVACT_STAMP       (TC_EVCTRL_EVACT_STAMP_Val     << TC_EVCTRL_EVACT_Pos)
#define TC_EVCTRL_EVACT_PPW         (TC_EVCTRL_EVACT_PPW_Val       << TC_EVCTRL_EVACT_Pos)
#define TC_EVCTRL_EVACT_PWP         (TC_EVCTRL_EVACT_PWP_Val       << TC_EVCTRL_EVACT_Pos)
#define TC_EVCTRL_EVACT_PW          (TC_EVCTRL_EVACT_PW_Val        << TC_EVCTRL_EVACT_Pos)
#define TC_EVCTRL_TCINV_Pos         4            /**< \brief (TC_EVCTRL) TC Event Input Polarity */
#define TC_EVCTRL_TCINV             (_U_(0x1) << TC_EVCTRL_TCINV_Pos)
#define TC_EVCTRL_TCEI_Pos          5            /**< \brief (TC_EVCTRL) TC Event Enable */
#define TC_EVCTRL_TCEI              (_U_(0x1) << TC_EVCTRL_TCEI_Pos)
#define TC_EVCTRL_OVFEO_Pos         8            /**< \brief (TC_EVCTRL) Event Output Enable */
#define TC_EVCTRL_OVFEO             (_U_(0x1) << TC_EVCTRL_OVFEO_Pos)
#define TC_EVCTRL_MCEO0_Pos         12           /**< \brief (TC_EVCTRL) MC Event Output Enable 0 */
#define TC_EVCTRL_MCEO0             (_U_(1) << TC_EVCTRL_MCEO0_Pos)
#define TC_EVCTRL_MCEO1_Pos         13           /**< \brief (TC_EVCTRL) MC Event Output Enable 1 */
#define TC_EVCTRL_MCEO1             (_U_(1) << TC_EVCTRL_MCEO1_Pos)
#define TC_EVCTRL_MCEO_Pos          12           /**< \brief (TC_EVCTRL) MC Event Output Enable x */
#define TC_EVCTRL_MCEO_Msk          (_U_(0x3) << TC_EVCTRL_MCEO_Pos)
#define TC_EVCTRL_MCEO(value)       (TC_EVCTRL_MCEO_Msk & ((value) << TC_EVCTRL_MCEO_Pos))
#define TC_EVCTRL_MASK              _U_(0x3137)  /**< \brief (TC_EVCTRL) MASK Register */

/* -------- TC_INTENCLR : (TC Offset: 0x08) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  OVF:1;            /*!< bit:      0  OVF Interrupt Disable              */
    uint8_t  ERR:1;            /*!< bit:      1  ERR Interrupt Disable              */
    uint8_t  :2;               /*!< bit:  2.. 3  Reserved                           */
    uint8_t  MC0:1;            /*!< bit:      4  MC Interrupt Disable 0             */
    uint8_t  MC1:1;            /*!< bit:      5  MC Interrupt Disable 1             */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  :4;               /*!< bit:  0.. 3  Reserved                           */
    uint8_t  MC:2;             /*!< bit:  4.. 5  MC Interrupt Disable x             */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_INTENCLR_OFFSET          0x08         /**< \brief (TC_INTENCLR offset) Interrupt Enable Clear */
#define TC_INTENCLR_RESETVALUE      _U_(0x00)    /**< \brief (TC_INTENCLR reset_value) Interrupt Enable Clear */

#define TC_INTENCLR_OVF_Pos         0            /**< \brief (TC_INTENCLR) OVF Interrupt Disable */
#define TC_INTENCLR_OVF             (_U_(0x1) << TC_INTENCLR_OVF_Pos)
#define TC_INTENCLR_ERR_Pos         1            /**< \brief (TC_INTENCLR) ERR Interrupt Disable */
#define TC_INTENCLR_ERR             (_U_(0x1) << TC_INTENCLR_ERR_Pos)
#define TC_INTENCLR_MC0_Pos         4            /**< \brief (TC_INTENCLR) MC Interrupt Disable 0 */
#define TC_INTENCLR_MC0             (_U_(1) << TC_INTENCLR_MC0_Pos)
#define TC_INTENCLR_MC1_Pos         5            /**< \brief (TC_INTENCLR) MC Interrupt Disable 1 */
#define TC_INTENCLR_MC1             (_U_(1) << TC_INTENCLR_MC1_Pos)
#define TC_INTENCLR_MC_Pos          4            /**< \brief (TC_INTENCLR) MC Interrupt Disable x */
#define TC_INTENCLR_MC_Msk          (_U_(0x3) << TC_INTENCLR_MC_Pos)
#define TC_INTENCLR_MC(value)       (TC_INTENCLR_MC_Msk & ((value) << TC_INTENCLR_MC_Pos))
#define TC_INTENCLR_MASK            _U_(0x33)    /**< \brief (TC_INTENCLR) MASK Register */

/* -------- TC_INTENSET : (TC Offset: 0x09) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  OVF:1;            /*!< bit:      0  OVF Interrupt Enable               */
    uint8_t  ERR:1;            /*!< bit:      1  ERR Interrupt Enable               */
    uint8_t  :2;               /*!< bit:  2.. 3  Reserved                           */
    uint8_t  MC0:1;            /*!< bit:      4  MC Interrupt Enable 0              */
    uint8_t  MC1:1;            /*!< bit:      5  MC Interrupt Enable 1              */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  :4;               /*!< bit:  0.. 3  Reserved                           */
    uint8_t  MC:2;             /*!< bit:  4.. 5  MC Interrupt Enable x              */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_INTENSET_OFFSET          0x09         /**< \brief (TC_INTENSET offset) Interrupt Enable Set */
#define TC_INTENSET_RESETVALUE      _U_(0x00)    /**< \brief (TC_INTENSET reset_value) Interrupt Enable Set */

#define TC_INTENSET_OVF_Pos         0            /**< \brief (TC_INTENSET) OVF Interrupt Enable */
#define TC_INTENSET_OVF             (_U_(0x1) << TC_INTENSET_OVF_Pos)
#define TC_INTENSET_ERR_Pos         1            /**< \brief (TC_INTENSET) ERR Interrupt Enable */
#define TC_INTENSET_ERR             (_U_(0x1) << TC_INTENSET_ERR_Pos)
#define TC_INTENSET_MC0_Pos         4            /**< \brief (TC_INTENSET) MC Interrupt Enable 0 */
#define TC_INTENSET_MC0             (_U_(1) << TC_INTENSET_MC0_Pos)
#define TC_INTENSET_MC1_Pos         5            /**< \brief (TC_INTENSET) MC Interrupt Enable 1 */
#define TC_INTENSET_MC1             (_U_(1) << TC_INTENSET_MC1_Pos)
#define TC_INTENSET_MC_Pos          4            /**< \brief (TC_INTENSET) MC Interrupt Enable x */
#define TC_INTENSET_MC_Msk          (_U_(0x3) << TC_INTENSET_MC_Pos)
#define TC_INTENSET_MC(value)       (TC_INTENSET_MC_Msk & ((value) << TC_INTENSET_MC_Pos))
#define TC_INTENSET_MASK            _U_(0x33)    /**< \brief (TC_INTENSET) MASK Register */

/* -------- TC_INTFLAG : (TC Offset: 0x0A) (R/W  8) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  OVF:1;            /*!< bit:      0  OVF Interrupt Flag                 */
    __I uint8_t  ERR:1;            /*!< bit:      1  ERR Interrupt Flag                 */
    __I uint8_t  :2;               /*!< bit:  2.. 3  Reserved                           */
    __I uint8_t  MC0:1;            /*!< bit:      4  MC Interrupt Flag 0                */
    __I uint8_t  MC1:1;            /*!< bit:      5  MC Interrupt Flag 1                */
    __I uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint8_t  :4;               /*!< bit:  0.. 3  Reserved                           */
    __I uint8_t  MC:2;             /*!< bit:  4.. 5  MC Interrupt Flag x                */
    __I uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TC_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_INTFLAG_OFFSET           0x0A         /**< \brief (TC_INTFLAG offset) Interrupt Flag Status and Clear */
#define TC_INTFLAG_RESETVALUE       _U_(0x00)    /**< \brief (TC_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define TC_INTFLAG_OVF_Pos          0            /**< \brief (TC_INTFLAG) OVF Interrupt Flag */
#define TC_INTFLAG_OVF              (_U_(0x1) << TC_INTFLAG_OVF_Pos)
#define TC_INTFLAG_ERR_Pos          1            /**< \brief (TC_INTFLAG) ERR Interrupt Flag */
#define TC_INTFLAG_ERR              (_U_(0x1) << TC_INTFLAG_ERR_Pos)
#define TC_INTFLAG_MC0_Pos          4            /**< \brief (TC_INTFLAG) MC Interrupt Flag 0 */
#define TC_INTFLAG_MC0              (_U_(1) << TC_INTFLAG_MC0_Pos)
#define TC_INTFLAG_MC1_Pos          5            /**< \brief (TC_INTFLAG) MC Interrupt Flag 1 */
#define TC_INTFLAG_MC1              (_U_(1) << TC_INTFLAG_MC1_Pos)
#define TC_INTFLAG_MC_Pos           4            /**< \brief (TC_INTFLAG) MC Interrupt Flag x */
#define TC_INTFLAG_MC_Msk           (_U_(0x3) << TC_INTFLAG_MC_Pos)
#define TC_INTFLAG_MC(value)        (TC_INTFLAG_MC_Msk & ((value) << TC_INTFLAG_MC_Pos))
#define TC_INTFLAG_MASK             _U_(0x33)    /**< \brief (TC_INTFLAG) MASK Register */

/* -------- TC_STATUS : (TC Offset: 0x0B) (R/W  8) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  STOP:1;           /*!< bit:      0  Stop Status Flag                   */
    uint8_t  SLAVE:1;          /*!< bit:      1  Slave Status Flag                  */
    uint8_t  :1;               /*!< bit:      2  Reserved                           */
    uint8_t  PERBUFV:1;        /*!< bit:      3  Synchronization Busy Status        */
    uint8_t  CCBUFV0:1;        /*!< bit:      4  Compare channel buffer 0 valid     */
    uint8_t  CCBUFV1:1;        /*!< bit:      5  Compare channel buffer 1 valid     */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  :4;               /*!< bit:  0.. 3  Reserved                           */
    uint8_t  CCBUFV:2;         /*!< bit:  4.. 5  Compare channel buffer x valid     */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TC_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_STATUS_OFFSET            0x0B         /**< \brief (TC_STATUS offset) Status */
#define TC_STATUS_RESETVALUE        _U_(0x01)    /**< \brief (TC_STATUS reset_value) Status */

#define TC_STATUS_STOP_Pos          0            /**< \brief (TC_STATUS) Stop Status Flag */
#define TC_STATUS_STOP              (_U_(0x1) << TC_STATUS_STOP_Pos)
#define TC_STATUS_SLAVE_Pos         1            /**< \brief (TC_STATUS) Slave Status Flag */
#define TC_STATUS_SLAVE             (_U_(0x1) << TC_STATUS_SLAVE_Pos)
#define TC_STATUS_PERBUFV_Pos       3            /**< \brief (TC_STATUS) Synchronization Busy Status */
#define TC_STATUS_PERBUFV           (_U_(0x1) << TC_STATUS_PERBUFV_Pos)
#define TC_STATUS_CCBUFV0_Pos       4            /**< \brief (TC_STATUS) Compare channel buffer 0 valid */
#define TC_STATUS_CCBUFV0           (_U_(1) << TC_STATUS_CCBUFV0_Pos)
#define TC_STATUS_CCBUFV1_Pos       5            /**< \brief (TC_STATUS) Compare channel buffer 1 valid */
#define TC_STATUS_CCBUFV1           (_U_(1) << TC_STATUS_CCBUFV1_Pos)
#define TC_STATUS_CCBUFV_Pos        4            /**< \brief (TC_STATUS) Compare channel buffer x valid */
#define TC_STATUS_CCBUFV_Msk        (_U_(0x3) << TC_STATUS_CCBUFV_Pos)
#define TC_STATUS_CCBUFV(value)     (TC_STATUS_CCBUFV_Msk & ((value) << TC_STATUS_CCBUFV_Pos))
#define TC_STATUS_MASK              _U_(0x3B)    /**< \brief (TC_STATUS) MASK Register */

/* -------- TC_WAVE : (TC Offset: 0x0C) (R/W  8) Waveform Generation Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  WAVEGEN:2;        /*!< bit:  0.. 1  Waveform Generation Mode           */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TC_WAVE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_WAVE_OFFSET              0x0C         /**< \brief (TC_WAVE offset) Waveform Generation Control */
#define TC_WAVE_RESETVALUE          _U_(0x00)    /**< \brief (TC_WAVE reset_value) Waveform Generation Control */

#define TC_WAVE_WAVEGEN_Pos         0            /**< \brief (TC_WAVE) Waveform Generation Mode */
#define TC_WAVE_WAVEGEN_Msk         (_U_(0x3) << TC_WAVE_WAVEGEN_Pos)
#define TC_WAVE_WAVEGEN(value)      (TC_WAVE_WAVEGEN_Msk & ((value) << TC_WAVE_WAVEGEN_Pos))
#define   TC_WAVE_WAVEGEN_NFRQ_Val        _U_(0x0)   /**< \brief (TC_WAVE) Normal frequency */
#define   TC_WAVE_WAVEGEN_MFRQ_Val        _U_(0x1)   /**< \brief (TC_WAVE) Match frequency */
#define   TC_WAVE_WAVEGEN_NPWM_Val        _U_(0x2)   /**< \brief (TC_WAVE) Normal PWM */
#define   TC_WAVE_WAVEGEN_MPWM_Val        _U_(0x3)   /**< \brief (TC_WAVE) Match PWM */
#define TC_WAVE_WAVEGEN_NFRQ        (TC_WAVE_WAVEGEN_NFRQ_Val      << TC_WAVE_WAVEGEN_Pos)
#define TC_WAVE_WAVEGEN_MFRQ        (TC_WAVE_WAVEGEN_MFRQ_Val      << TC_WAVE_WAVEGEN_Pos)
#define TC_WAVE_WAVEGEN_NPWM        (TC_WAVE_WAVEGEN_NPWM_Val      << TC_WAVE_WAVEGEN_Pos)
#define TC_WAVE_WAVEGEN_MPWM        (TC_WAVE_WAVEGEN_MPWM_Val      << TC_WAVE_WAVEGEN_Pos)
#define TC_WAVE_MASK                _U_(0x03)    /**< \brief (TC_WAVE) MASK Register */

/* -------- TC_DRVCTRL : (TC Offset: 0x0D) (R/W  8) Control C -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  INVEN0:1;         /*!< bit:      0  Output Waveform Invert Enable 0    */
    uint8_t  INVEN1:1;         /*!< bit:      1  Output Waveform Invert Enable 1    */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  INVEN:2;          /*!< bit:  0.. 1  Output Waveform Invert Enable x    */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TC_DRVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_DRVCTRL_OFFSET           0x0D         /**< \brief (TC_DRVCTRL offset) Control C */
#define TC_DRVCTRL_RESETVALUE       _U_(0x00)    /**< \brief (TC_DRVCTRL reset_value) Control C */

#define TC_DRVCTRL_INVEN0_Pos       0            /**< \brief (TC_DRVCTRL) Output Waveform Invert Enable 0 */
#define TC_DRVCTRL_INVEN0           (_U_(1) << TC_DRVCTRL_INVEN0_Pos)
#define TC_DRVCTRL_INVEN1_Pos       1            /**< \brief (TC_DRVCTRL) Output Waveform Invert Enable 1 */
#define TC_DRVCTRL_INVEN1           (_U_(1) << TC_DRVCTRL_INVEN1_Pos)
#define TC_DRVCTRL_INVEN_Pos        0            /**< \brief (TC_DRVCTRL) Output Waveform Invert Enable x */
#define TC_DRVCTRL_INVEN_Msk        (_U_(0x3) << TC_DRVCTRL_INVEN_Pos)
#define TC_DRVCTRL_INVEN(value)     (TC_DRVCTRL_INVEN_Msk & ((value) << TC_DRVCTRL_INVEN_Pos))
#define TC_DRVCTRL_MASK             _U_(0x03)    /**< \brief (TC_DRVCTRL) MASK Register */

/* -------- TC_DBGCTRL : (TC Offset: 0x0F) (R/W  8) Debug Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DBGRUN:1;         /*!< bit:      0  Run During Debug                   */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TC_DBGCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_DBGCTRL_OFFSET           0x0F         /**< \brief (TC_DBGCTRL offset) Debug Control */
#define TC_DBGCTRL_RESETVALUE       _U_(0x00)    /**< \brief (TC_DBGCTRL reset_value) Debug Control */

#define TC_DBGCTRL_DBGRUN_Pos       0            /**< \brief (TC_DBGCTRL) Run During Debug */
#define TC_DBGCTRL_DBGRUN           (_U_(0x1) << TC_DBGCTRL_DBGRUN_Pos)
#define TC_DBGCTRL_MASK             _U_(0x01)    /**< \brief (TC_DBGCTRL) MASK Register */

/* -------- TC_SYNCBUSY : (TC Offset: 0x10) (R/  32) Synchronization Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  swrst                              */
    uint32_t ENABLE:1;         /*!< bit:      1  enable                             */
    uint32_t CTRLB:1;          /*!< bit:      2  CTRLB                              */
    uint32_t STATUS:1;         /*!< bit:      3  STATUS                             */
    uint32_t COUNT:1;          /*!< bit:      4  Counter                            */
    uint32_t PER:1;            /*!< bit:      5  Period                             */
    uint32_t CC0:1;            /*!< bit:      6  Compare Channel 0                  */
    uint32_t CC1:1;            /*!< bit:      7  Compare Channel 1                  */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :6;               /*!< bit:  0.. 5  Reserved                           */
    uint32_t CC:2;             /*!< bit:  6.. 7  Compare Channel x                  */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TC_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_SYNCBUSY_OFFSET          0x10         /**< \brief (TC_SYNCBUSY offset) Synchronization Status */
#define TC_SYNCBUSY_RESETVALUE      _U_(0x00000000) /**< \brief (TC_SYNCBUSY reset_value) Synchronization Status */

#define TC_SYNCBUSY_SWRST_Pos       0            /**< \brief (TC_SYNCBUSY) swrst */
#define TC_SYNCBUSY_SWRST           (_U_(0x1) << TC_SYNCBUSY_SWRST_Pos)
#define TC_SYNCBUSY_ENABLE_Pos      1            /**< \brief (TC_SYNCBUSY) enable */
#define TC_SYNCBUSY_ENABLE          (_U_(0x1) << TC_SYNCBUSY_ENABLE_Pos)
#define TC_SYNCBUSY_CTRLB_Pos       2            /**< \brief (TC_SYNCBUSY) CTRLB */
#define TC_SYNCBUSY_CTRLB           (_U_(0x1) << TC_SYNCBUSY_CTRLB_Pos)
#define TC_SYNCBUSY_STATUS_Pos      3            /**< \brief (TC_SYNCBUSY) STATUS */
#define TC_SYNCBUSY_STATUS          (_U_(0x1) << TC_SYNCBUSY_STATUS_Pos)
#define TC_SYNCBUSY_COUNT_Pos       4            /**< \brief (TC_SYNCBUSY) Counter */
#define TC_SYNCBUSY_COUNT           (_U_(0x1) << TC_SYNCBUSY_COUNT_Pos)
#define TC_SYNCBUSY_PER_Pos         5            /**< \brief (TC_SYNCBUSY) Period */
#define TC_SYNCBUSY_PER             (_U_(0x1) << TC_SYNCBUSY_PER_Pos)
#define TC_SYNCBUSY_CC0_Pos         6            /**< \brief (TC_SYNCBUSY) Compare Channel 0 */
#define TC_SYNCBUSY_CC0             (_U_(1) << TC_SYNCBUSY_CC0_Pos)
#define TC_SYNCBUSY_CC1_Pos         7            /**< \brief (TC_SYNCBUSY) Compare Channel 1 */
#define TC_SYNCBUSY_CC1             (_U_(1) << TC_SYNCBUSY_CC1_Pos)
#define TC_SYNCBUSY_CC_Pos          6            /**< \brief (TC_SYNCBUSY) Compare Channel x */
#define TC_SYNCBUSY_CC_Msk          (_U_(0x3) << TC_SYNCBUSY_CC_Pos)
#define TC_SYNCBUSY_CC(value)       (TC_SYNCBUSY_CC_Msk & ((value) << TC_SYNCBUSY_CC_Pos))
#define TC_SYNCBUSY_MASK            _U_(0x000000FF) /**< \brief (TC_SYNCBUSY) MASK Register */

/* -------- TC_COUNT16_COUNT : (TC Offset: 0x14) (R/W 16) COUNT16 COUNT16 Count -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t COUNT:16;         /*!< bit:  0..15  Counter Value                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} TC_COUNT16_COUNT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_COUNT16_COUNT_OFFSET     0x14         /**< \brief (TC_COUNT16_COUNT offset) COUNT16 Count */
#define TC_COUNT16_COUNT_RESETVALUE _U_(0x0000)  /**< \brief (TC_COUNT16_COUNT reset_value) COUNT16 Count */

#define TC_COUNT16_COUNT_COUNT_Pos  0            /**< \brief (TC_COUNT16_COUNT) Counter Value */
#define TC_COUNT16_COUNT_COUNT_Msk  (_U_(0xFFFF) << TC_COUNT16_COUNT_COUNT_Pos)
#define TC_COUNT16_COUNT_COUNT(value) (TC_COUNT16_COUNT_COUNT_Msk & ((value) << TC_COUNT16_COUNT_COUNT_Pos))
#define TC_COUNT16_COUNT_MASK       _U_(0xFFFF)  /**< \brief (TC_COUNT16_COUNT) MASK Register */

/* -------- TC_COUNT32_COUNT : (TC Offset: 0x14) (R/W 32) COUNT32 COUNT32 Count -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t COUNT:32;         /*!< bit:  0..31  Counter Value                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TC_COUNT32_COUNT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_COUNT32_COUNT_OFFSET     0x14         /**< \brief (TC_COUNT32_COUNT offset) COUNT32 Count */
#define TC_COUNT32_COUNT_RESETVALUE _U_(0x00000000) /**< \brief (TC_COUNT32_COUNT reset_value) COUNT32 Count */

#define TC_COUNT32_COUNT_COUNT_Pos  0            /**< \brief (TC_COUNT32_COUNT) Counter Value */
#define TC_COUNT32_COUNT_COUNT_Msk  (_U_(0xFFFFFFFF) << TC_COUNT32_COUNT_COUNT_Pos)
#define TC_COUNT32_COUNT_COUNT(value) (TC_COUNT32_COUNT_COUNT_Msk & ((value) << TC_COUNT32_COUNT_COUNT_Pos))
#define TC_COUNT32_COUNT_MASK       _U_(0xFFFFFFFF) /**< \brief (TC_COUNT32_COUNT) MASK Register */

/* -------- TC_COUNT8_COUNT : (TC Offset: 0x14) (R/W  8) COUNT8 COUNT8 Count -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  COUNT:8;          /*!< bit:  0.. 7  Counter Value                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TC_COUNT8_COUNT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_COUNT8_COUNT_OFFSET      0x14         /**< \brief (TC_COUNT8_COUNT offset) COUNT8 Count */
#define TC_COUNT8_COUNT_RESETVALUE  _U_(0x00)    /**< \brief (TC_COUNT8_COUNT reset_value) COUNT8 Count */

#define TC_COUNT8_COUNT_COUNT_Pos   0            /**< \brief (TC_COUNT8_COUNT) Counter Value */
#define TC_COUNT8_COUNT_COUNT_Msk   (_U_(0xFF) << TC_COUNT8_COUNT_COUNT_Pos)
#define TC_COUNT8_COUNT_COUNT(value) (TC_COUNT8_COUNT_COUNT_Msk & ((value) << TC_COUNT8_COUNT_COUNT_Pos))
#define TC_COUNT8_COUNT_MASK        _U_(0xFF)    /**< \brief (TC_COUNT8_COUNT) MASK Register */

/* -------- TC_COUNT8_PER : (TC Offset: 0x1B) (R/W  8) COUNT8 COUNT8 Period -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  PER:8;            /*!< bit:  0.. 7  Period Value                       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TC_COUNT8_PER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_COUNT8_PER_OFFSET        0x1B         /**< \brief (TC_COUNT8_PER offset) COUNT8 Period */
#define TC_COUNT8_PER_RESETVALUE    _U_(0xFF)    /**< \brief (TC_COUNT8_PER reset_value) COUNT8 Period */

#define TC_COUNT8_PER_PER_Pos       0            /**< \brief (TC_COUNT8_PER) Period Value */
#define TC_COUNT8_PER_PER_Msk       (_U_(0xFF) << TC_COUNT8_PER_PER_Pos)
#define TC_COUNT8_PER_PER(value)    (TC_COUNT8_PER_PER_Msk & ((value) << TC_COUNT8_PER_PER_Pos))
#define TC_COUNT8_PER_MASK          _U_(0xFF)    /**< \brief (TC_COUNT8_PER) MASK Register */

/* -------- TC_COUNT16_CC : (TC Offset: 0x1C) (R/W 16) COUNT16 COUNT16 Compare and Capture -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t CC:16;            /*!< bit:  0..15  Counter/Compare Value              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} TC_COUNT16_CC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_COUNT16_CC_OFFSET        0x1C         /**< \brief (TC_COUNT16_CC offset) COUNT16 Compare and Capture */
#define TC_COUNT16_CC_RESETVALUE    _U_(0x0000)  /**< \brief (TC_COUNT16_CC reset_value) COUNT16 Compare and Capture */

#define TC_COUNT16_CC_CC_Pos        0            /**< \brief (TC_COUNT16_CC) Counter/Compare Value */
#define TC_COUNT16_CC_CC_Msk        (_U_(0xFFFF) << TC_COUNT16_CC_CC_Pos)
#define TC_COUNT16_CC_CC(value)     (TC_COUNT16_CC_CC_Msk & ((value) << TC_COUNT16_CC_CC_Pos))
#define TC_COUNT16_CC_MASK          _U_(0xFFFF)  /**< \brief (TC_COUNT16_CC) MASK Register */

/* -------- TC_COUNT32_CC : (TC Offset: 0x1C) (R/W 32) COUNT32 COUNT32 Compare and Capture -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CC:32;            /*!< bit:  0..31  Counter/Compare Value              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TC_COUNT32_CC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_COUNT32_CC_OFFSET        0x1C         /**< \brief (TC_COUNT32_CC offset) COUNT32 Compare and Capture */
#define TC_COUNT32_CC_RESETVALUE    _U_(0x00000000) /**< \brief (TC_COUNT32_CC reset_value) COUNT32 Compare and Capture */

#define TC_COUNT32_CC_CC_Pos        0            /**< \brief (TC_COUNT32_CC) Counter/Compare Value */
#define TC_COUNT32_CC_CC_Msk        (_U_(0xFFFFFFFF) << TC_COUNT32_CC_CC_Pos)
#define TC_COUNT32_CC_CC(value)     (TC_COUNT32_CC_CC_Msk & ((value) << TC_COUNT32_CC_CC_Pos))
#define TC_COUNT32_CC_MASK          _U_(0xFFFFFFFF) /**< \brief (TC_COUNT32_CC) MASK Register */

/* -------- TC_COUNT8_CC : (TC Offset: 0x1C) (R/W  8) COUNT8 COUNT8 Compare and Capture -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CC:8;             /*!< bit:  0.. 7  Counter/Compare Value              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TC_COUNT8_CC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_COUNT8_CC_OFFSET         0x1C         /**< \brief (TC_COUNT8_CC offset) COUNT8 Compare and Capture */
#define TC_COUNT8_CC_RESETVALUE     _U_(0x00)    /**< \brief (TC_COUNT8_CC reset_value) COUNT8 Compare and Capture */

#define TC_COUNT8_CC_CC_Pos         0            /**< \brief (TC_COUNT8_CC) Counter/Compare Value */
#define TC_COUNT8_CC_CC_Msk         (_U_(0xFF) << TC_COUNT8_CC_CC_Pos)
#define TC_COUNT8_CC_CC(value)      (TC_COUNT8_CC_CC_Msk & ((value) << TC_COUNT8_CC_CC_Pos))
#define TC_COUNT8_CC_MASK           _U_(0xFF)    /**< \brief (TC_COUNT8_CC) MASK Register */

/* -------- TC_COUNT8_PERBUF : (TC Offset: 0x2F) (R/W  8) COUNT8 COUNT8 Period Buffer -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  PERBUF:8;         /*!< bit:  0.. 7  Period Buffer Value                */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TC_COUNT8_PERBUF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_COUNT8_PERBUF_OFFSET     0x2F         /**< \brief (TC_COUNT8_PERBUF offset) COUNT8 Period Buffer */
#define TC_COUNT8_PERBUF_RESETVALUE _U_(0xFF)    /**< \brief (TC_COUNT8_PERBUF reset_value) COUNT8 Period Buffer */

#define TC_COUNT8_PERBUF_PERBUF_Pos 0            /**< \brief (TC_COUNT8_PERBUF) Period Buffer Value */
#define TC_COUNT8_PERBUF_PERBUF_Msk (_U_(0xFF) << TC_COUNT8_PERBUF_PERBUF_Pos)
#define TC_COUNT8_PERBUF_PERBUF(value) (TC_COUNT8_PERBUF_PERBUF_Msk & ((value) << TC_COUNT8_PERBUF_PERBUF_Pos))
#define TC_COUNT8_PERBUF_MASK       _U_(0xFF)    /**< \brief (TC_COUNT8_PERBUF) MASK Register */

/* -------- TC_COUNT16_CCBUF : (TC Offset: 0x30) (R/W 16) COUNT16 COUNT16 Compare and Capture Buffer -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t CCBUF:16;         /*!< bit:  0..15  Counter/Compare Buffer Value       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} TC_COUNT16_CCBUF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_COUNT16_CCBUF_OFFSET     0x30         /**< \brief (TC_COUNT16_CCBUF offset) COUNT16 Compare and Capture Buffer */
#define TC_COUNT16_CCBUF_RESETVALUE _U_(0x0000)  /**< \brief (TC_COUNT16_CCBUF reset_value) COUNT16 Compare and Capture Buffer */

#define TC_COUNT16_CCBUF_CCBUF_Pos  0            /**< \brief (TC_COUNT16_CCBUF) Counter/Compare Buffer Value */
#define TC_COUNT16_CCBUF_CCBUF_Msk  (_U_(0xFFFF) << TC_COUNT16_CCBUF_CCBUF_Pos)
#define TC_COUNT16_CCBUF_CCBUF(value) (TC_COUNT16_CCBUF_CCBUF_Msk & ((value) << TC_COUNT16_CCBUF_CCBUF_Pos))
#define TC_COUNT16_CCBUF_MASK       _U_(0xFFFF)  /**< \brief (TC_COUNT16_CCBUF) MASK Register */

/* -------- TC_COUNT32_CCBUF : (TC Offset: 0x30) (R/W 32) COUNT32 COUNT32 Compare and Capture Buffer -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CCBUF:32;         /*!< bit:  0..31  Counter/Compare Buffer Value       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TC_COUNT32_CCBUF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_COUNT32_CCBUF_OFFSET     0x30         /**< \brief (TC_COUNT32_CCBUF offset) COUNT32 Compare and Capture Buffer */
#define TC_COUNT32_CCBUF_RESETVALUE _U_(0x00000000) /**< \brief (TC_COUNT32_CCBUF reset_value) COUNT32 Compare and Capture Buffer */

#define TC_COUNT32_CCBUF_CCBUF_Pos  0            /**< \brief (TC_COUNT32_CCBUF) Counter/Compare Buffer Value */
#define TC_COUNT32_CCBUF_CCBUF_Msk  (_U_(0xFFFFFFFF) << TC_COUNT32_CCBUF_CCBUF_Pos)
#define TC_COUNT32_CCBUF_CCBUF(value) (TC_COUNT32_CCBUF_CCBUF_Msk & ((value) << TC_COUNT32_CCBUF_CCBUF_Pos))
#define TC_COUNT32_CCBUF_MASK       _U_(0xFFFFFFFF) /**< \brief (TC_COUNT32_CCBUF) MASK Register */

/* -------- TC_COUNT8_CCBUF : (TC Offset: 0x30) (R/W  8) COUNT8 COUNT8 Compare and Capture Buffer -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CCBUF:8;          /*!< bit:  0.. 7  Counter/Compare Buffer Value       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TC_COUNT8_CCBUF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TC_COUNT8_CCBUF_OFFSET      0x30         /**< \brief (TC_COUNT8_CCBUF offset) COUNT8 Compare and Capture Buffer */
#define TC_COUNT8_CCBUF_RESETVALUE  _U_(0x00)    /**< \brief (TC_COUNT8_CCBUF reset_value) COUNT8 Compare and Capture Buffer */

#define TC_COUNT8_CCBUF_CCBUF_Pos   0            /**< \brief (TC_COUNT8_CCBUF) Counter/Compare Buffer Value */
#define TC_COUNT8_CCBUF_CCBUF_Msk   (_U_(0xFF) << TC_COUNT8_CCBUF_CCBUF_Pos)
#define TC_COUNT8_CCBUF_CCBUF(value) (TC_COUNT8_CCBUF_CCBUF_Msk & ((value) << TC_COUNT8_CCBUF_CCBUF_Pos))
#define TC_COUNT8_CCBUF_MASK        _U_(0xFF)    /**< \brief (TC_COUNT8_CCBUF) MASK Register */

/** \brief TC_COUNT8 hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct { /* 8-bit Counter Mode */
  __IO TC_CTRLA_Type             CTRLA;       /**< \brief Offset: 0x00 (R/W 32) Control A */
  __IO TC_CTRLBCLR_Type          CTRLBCLR;    /**< \brief Offset: 0x04 (R/W  8) Control B Clear */
  __IO TC_CTRLBSET_Type          CTRLBSET;    /**< \brief Offset: 0x05 (R/W  8) Control B Set */
  __IO TC_EVCTRL_Type            EVCTRL;      /**< \brief Offset: 0x06 (R/W 16) Event Control */
  __IO TC_INTENCLR_Type          INTENCLR;    /**< \brief Offset: 0x08 (R/W  8) Interrupt Enable Clear */
  __IO TC_INTENSET_Type          INTENSET;    /**< \brief Offset: 0x09 (R/W  8) Interrupt Enable Set */
  __IO TC_INTFLAG_Type           INTFLAG;     /**< \brief Offset: 0x0A (R/W  8) Interrupt Flag Status and Clear */
  __IO TC_STATUS_Type            STATUS;      /**< \brief Offset: 0x0B (R/W  8) Status */
  __IO TC_WAVE_Type              WAVE;        /**< \brief Offset: 0x0C (R/W  8) Waveform Generation Control */
  __IO TC_DRVCTRL_Type           DRVCTRL;     /**< \brief Offset: 0x0D (R/W  8) Control C */
       RoReg8                    Reserved1[0x1];
  __IO TC_DBGCTRL_Type           DBGCTRL;     /**< \brief Offset: 0x0F (R/W  8) Debug Control */
  __I  TC_SYNCBUSY_Type          SYNCBUSY;    /**< \brief Offset: 0x10 (R/  32) Synchronization Status */
  __IO TC_COUNT8_COUNT_Type      COUNT;       /**< \brief Offset: 0x14 (R/W  8) COUNT8 Count */
       RoReg8                    Reserved2[0x6];
  __IO TC_COUNT8_PER_Type        PER;         /**< \brief Offset: 0x1B (R/W  8) COUNT8 Period */
  __IO TC_COUNT8_CC_Type         CC[2];       /**< \brief Offset: 0x1C (R/W  8) COUNT8 Compare and Capture */
       RoReg8                    Reserved3[0x11];
  __IO TC_COUNT8_PERBUF_Type     PERBUF;      /**< \brief Offset: 0x2F (R/W  8) COUNT8 Period Buffer */
  __IO TC_COUNT8_CCBUF_Type      CCBUF[2];    /**< \brief Offset: 0x30 (R/W  8) COUNT8 Compare and Capture Buffer */
} TcCount8;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief TC_COUNT16 hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct { /* 16-bit Counter Mode */
  __IO TC_CTRLA_Type             CTRLA;       /**< \brief Offset: 0x00 (R/W 32) Control A */
  __IO TC_CTRLBCLR_Type          CTRLBCLR;    /**< \brief Offset: 0x04 (R/W  8) Control B Clear */
  __IO TC_CTRLBSET_Type          CTRLBSET;    /**< \brief Offset: 0x05 (R/W  8) Control B Set */
  __IO TC_EVCTRL_Type            EVCTRL;      /**< \brief Offset: 0x06 (R/W 16) Event Control */
  __IO TC_INTENCLR_Type          INTENCLR;    /**< \brief Offset: 0x08 (R/W  8) Interrupt Enable Clear */
  __IO TC_INTENSET_Type          INTENSET;    /**< \brief Offset: 0x09 (R/W  8) Interrupt Enable Set */
  __IO TC_INTFLAG_Type           INTFLAG;     /**< \brief Offset: 0x0A (R/W  8) Interrupt Flag Status and Clear */
  __IO TC_STATUS_Type            STATUS;      /**< \brief Offset: 0x0B (R/W  8) Status */
  __IO TC_WAVE_Type              WAVE;        /**< \brief Offset: 0x0C (R/W  8) Waveform Generation Control */
  __IO TC_DRVCTRL_Type           DRVCTRL;     /**< \brief Offset: 0x0D (R/W  8) Control C */
       RoReg8                    Reserved1[0x1];
  __IO TC_DBGCTRL_Type           DBGCTRL;     /**< \brief Offset: 0x0F (R/W  8) Debug Control */
  __I  TC_SYNCBUSY_Type          SYNCBUSY;    /**< \brief Offset: 0x10 (R/  32) Synchronization Status */
  __IO TC_COUNT16_COUNT_Type     COUNT;       /**< \brief Offset: 0x14 (R/W 16) COUNT16 Count */
       RoReg8                    Reserved2[0x6];
  __IO TC_COUNT16_CC_Type        CC[2];       /**< \brief Offset: 0x1C (R/W 16) COUNT16 Compare and Capture */
       RoReg8                    Reserved3[0x10];
  __IO TC_COUNT16_CCBUF_Type     CCBUF[2];    /**< \brief Offset: 0x30 (R/W 16) COUNT16 Compare and Capture Buffer */
} TcCount16;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief TC_COUNT32 hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct { /* 32-bit Counter Mode */
  __IO TC_CTRLA_Type             CTRLA;       /**< \brief Offset: 0x00 (R/W 32) Control A */
  __IO TC_CTRLBCLR_Type          CTRLBCLR;    /**< \brief Offset: 0x04 (R/W  8) Control B Clear */
  __IO TC_CTRLBSET_Type          CTRLBSET;    /**< \brief Offset: 0x05 (R/W  8) Control B Set */
  __IO TC_EVCTRL_Type            EVCTRL;      /**< \brief Offset: 0x06 (R/W 16) Event Control */
  __IO TC_INTENCLR_Type          INTENCLR;    /**< \brief Offset: 0x08 (R/W  8) Interrupt Enable Clear */
  __IO TC_INTENSET_Type          INTENSET;    /**< \brief Offset: 0x09 (R/W  8) Interrupt Enable Set */
  __IO TC_INTFLAG_Type           INTFLAG;     /**< \brief Offset: 0x0A (R/W  8) Interrupt Flag Status and Clear */
  __IO TC_STATUS_Type            STATUS;      /**< \brief Offset: 0x0B (R/W  8) Status */
  __IO TC_WAVE_Type              WAVE;        /**< \brief Offset: 0x0C (R/W  8) Waveform Generation Control */
  __IO TC_DRVCTRL_Type           DRVCTRL;     /**< \brief Offset: 0x0D (R/W  8) Control C */
       RoReg8                    Reserved1[0x1];
  __IO TC_DBGCTRL_Type           DBGCTRL;     /**< \brief Offset: 0x0F (R/W  8) Debug Control */
  __I  TC_SYNCBUSY_Type          SYNCBUSY;    /**< \brief Offset: 0x10 (R/  32) Synchronization Status */
  __IO TC_COUNT32_COUNT_Type     COUNT;       /**< \brief Offset: 0x14 (R/W 32) COUNT32 Count */
       RoReg8                    Reserved2[0x4];
  __IO TC_COUNT32_CC_Type        CC[2];       /**< \brief Offset: 0x1C (R/W 32) COUNT32 Compare and Capture */
       RoReg8                    Reserved3[0xC];
  __IO TC_COUNT32_CCBUF_Type     CCBUF[2];    /**< \brief Offset: 0x30 (R/W 32) COUNT32 Compare and Capture Buffer */
} TcCount32;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
       TcCount8                  COUNT8;      /**< \brief Offset: 0x00 8-bit Counter Mode */
       TcCount16                 COUNT16;     /**< \brief Offset: 0x00 16-bit Counter Mode */
       TcCount32                 COUNT32;     /**< \brief Offset: 0x00 32-bit Counter Mode */
} Tc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_TC_COMPONENT_ */
