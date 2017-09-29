/**
 * \file
 *
 * \brief Component description for TCC
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

#ifndef _SAMD51_TCC_COMPONENT_
#define _SAMD51_TCC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR TCC */
/* ========================================================================== */
/** \addtogroup SAMD51_TCC Timer Counter Control */
/*@{*/

#define TCC_U2213
#define REV_TCC                     0x310

/* -------- TCC_CTRLA : (TCC Offset: 0x00) (R/W 32) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint32_t :3;               /*!< bit:  2.. 4  Reserved                           */
    uint32_t RESOLUTION:2;     /*!< bit:  5.. 6  Enhanced Resolution                */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t PRESCALER:3;      /*!< bit:  8..10  Prescaler                          */
    uint32_t RUNSTDBY:1;       /*!< bit:     11  Run in Standby                     */
    uint32_t PRESCSYNC:2;      /*!< bit: 12..13  Prescaler and Counter Synchronization Selection */
    uint32_t ALOCK:1;          /*!< bit:     14  Auto Lock                          */
    uint32_t MSYNC:1;          /*!< bit:     15  Master Synchronization (only for TCC Slave Instance) */
    uint32_t :7;               /*!< bit: 16..22  Reserved                           */
    uint32_t DMAOS:1;          /*!< bit:     23  DMA One-shot Trigger Mode          */
    uint32_t CPTEN0:1;         /*!< bit:     24  Capture Channel 0 Enable           */
    uint32_t CPTEN1:1;         /*!< bit:     25  Capture Channel 1 Enable           */
    uint32_t CPTEN2:1;         /*!< bit:     26  Capture Channel 2 Enable           */
    uint32_t CPTEN3:1;         /*!< bit:     27  Capture Channel 3 Enable           */
    uint32_t CPTEN4:1;         /*!< bit:     28  Capture Channel 4 Enable           */
    uint32_t CPTEN5:1;         /*!< bit:     29  Capture Channel 5 Enable           */
    uint32_t :2;               /*!< bit: 30..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :24;              /*!< bit:  0..23  Reserved                           */
    uint32_t CPTEN:6;          /*!< bit: 24..29  Capture Channel x Enable           */
    uint32_t :2;               /*!< bit: 30..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_CTRLA_OFFSET            0x00         /**< \brief (TCC_CTRLA offset) Control A */
#define TCC_CTRLA_RESETVALUE        _U_(0x00000000) /**< \brief (TCC_CTRLA reset_value) Control A */

#define TCC_CTRLA_SWRST_Pos         0            /**< \brief (TCC_CTRLA) Software Reset */
#define TCC_CTRLA_SWRST             (_U_(0x1) << TCC_CTRLA_SWRST_Pos)
#define TCC_CTRLA_ENABLE_Pos        1            /**< \brief (TCC_CTRLA) Enable */
#define TCC_CTRLA_ENABLE            (_U_(0x1) << TCC_CTRLA_ENABLE_Pos)
#define TCC_CTRLA_RESOLUTION_Pos    5            /**< \brief (TCC_CTRLA) Enhanced Resolution */
#define TCC_CTRLA_RESOLUTION_Msk    (_U_(0x3) << TCC_CTRLA_RESOLUTION_Pos)
#define TCC_CTRLA_RESOLUTION(value) (TCC_CTRLA_RESOLUTION_Msk & ((value) << TCC_CTRLA_RESOLUTION_Pos))
#define   TCC_CTRLA_RESOLUTION_NONE_Val   _U_(0x0)   /**< \brief (TCC_CTRLA) Dithering is disabled */
#define   TCC_CTRLA_RESOLUTION_DITH4_Val  _U_(0x1)   /**< \brief (TCC_CTRLA) Dithering is done every 16 PWM frames */
#define   TCC_CTRLA_RESOLUTION_DITH5_Val  _U_(0x2)   /**< \brief (TCC_CTRLA) Dithering is done every 32 PWM frames */
#define   TCC_CTRLA_RESOLUTION_DITH6_Val  _U_(0x3)   /**< \brief (TCC_CTRLA) Dithering is done every 64 PWM frames */
#define TCC_CTRLA_RESOLUTION_NONE   (TCC_CTRLA_RESOLUTION_NONE_Val << TCC_CTRLA_RESOLUTION_Pos)
#define TCC_CTRLA_RESOLUTION_DITH4  (TCC_CTRLA_RESOLUTION_DITH4_Val << TCC_CTRLA_RESOLUTION_Pos)
#define TCC_CTRLA_RESOLUTION_DITH5  (TCC_CTRLA_RESOLUTION_DITH5_Val << TCC_CTRLA_RESOLUTION_Pos)
#define TCC_CTRLA_RESOLUTION_DITH6  (TCC_CTRLA_RESOLUTION_DITH6_Val << TCC_CTRLA_RESOLUTION_Pos)
#define TCC_CTRLA_PRESCALER_Pos     8            /**< \brief (TCC_CTRLA) Prescaler */
#define TCC_CTRLA_PRESCALER_Msk     (_U_(0x7) << TCC_CTRLA_PRESCALER_Pos)
#define TCC_CTRLA_PRESCALER(value)  (TCC_CTRLA_PRESCALER_Msk & ((value) << TCC_CTRLA_PRESCALER_Pos))
#define   TCC_CTRLA_PRESCALER_DIV1_Val    _U_(0x0)   /**< \brief (TCC_CTRLA) No division */
#define   TCC_CTRLA_PRESCALER_DIV2_Val    _U_(0x1)   /**< \brief (TCC_CTRLA) Divide by 2 */
#define   TCC_CTRLA_PRESCALER_DIV4_Val    _U_(0x2)   /**< \brief (TCC_CTRLA) Divide by 4 */
#define   TCC_CTRLA_PRESCALER_DIV8_Val    _U_(0x3)   /**< \brief (TCC_CTRLA) Divide by 8 */
#define   TCC_CTRLA_PRESCALER_DIV16_Val   _U_(0x4)   /**< \brief (TCC_CTRLA) Divide by 16 */
#define   TCC_CTRLA_PRESCALER_DIV64_Val   _U_(0x5)   /**< \brief (TCC_CTRLA) Divide by 64 */
#define   TCC_CTRLA_PRESCALER_DIV256_Val  _U_(0x6)   /**< \brief (TCC_CTRLA) Divide by 256 */
#define   TCC_CTRLA_PRESCALER_DIV1024_Val _U_(0x7)   /**< \brief (TCC_CTRLA) Divide by 1024 */
#define TCC_CTRLA_PRESCALER_DIV1    (TCC_CTRLA_PRESCALER_DIV1_Val  << TCC_CTRLA_PRESCALER_Pos)
#define TCC_CTRLA_PRESCALER_DIV2    (TCC_CTRLA_PRESCALER_DIV2_Val  << TCC_CTRLA_PRESCALER_Pos)
#define TCC_CTRLA_PRESCALER_DIV4    (TCC_CTRLA_PRESCALER_DIV4_Val  << TCC_CTRLA_PRESCALER_Pos)
#define TCC_CTRLA_PRESCALER_DIV8    (TCC_CTRLA_PRESCALER_DIV8_Val  << TCC_CTRLA_PRESCALER_Pos)
#define TCC_CTRLA_PRESCALER_DIV16   (TCC_CTRLA_PRESCALER_DIV16_Val << TCC_CTRLA_PRESCALER_Pos)
#define TCC_CTRLA_PRESCALER_DIV64   (TCC_CTRLA_PRESCALER_DIV64_Val << TCC_CTRLA_PRESCALER_Pos)
#define TCC_CTRLA_PRESCALER_DIV256  (TCC_CTRLA_PRESCALER_DIV256_Val << TCC_CTRLA_PRESCALER_Pos)
#define TCC_CTRLA_PRESCALER_DIV1024 (TCC_CTRLA_PRESCALER_DIV1024_Val << TCC_CTRLA_PRESCALER_Pos)
#define TCC_CTRLA_RUNSTDBY_Pos      11           /**< \brief (TCC_CTRLA) Run in Standby */
#define TCC_CTRLA_RUNSTDBY          (_U_(0x1) << TCC_CTRLA_RUNSTDBY_Pos)
#define TCC_CTRLA_PRESCSYNC_Pos     12           /**< \brief (TCC_CTRLA) Prescaler and Counter Synchronization Selection */
#define TCC_CTRLA_PRESCSYNC_Msk     (_U_(0x3) << TCC_CTRLA_PRESCSYNC_Pos)
#define TCC_CTRLA_PRESCSYNC(value)  (TCC_CTRLA_PRESCSYNC_Msk & ((value) << TCC_CTRLA_PRESCSYNC_Pos))
#define   TCC_CTRLA_PRESCSYNC_GCLK_Val    _U_(0x0)   /**< \brief (TCC_CTRLA) Reload or reset counter on next GCLK */
#define   TCC_CTRLA_PRESCSYNC_PRESC_Val   _U_(0x1)   /**< \brief (TCC_CTRLA) Reload or reset counter on next prescaler clock */
#define   TCC_CTRLA_PRESCSYNC_RESYNC_Val  _U_(0x2)   /**< \brief (TCC_CTRLA) Reload or reset counter on next GCLK and reset prescaler counter */
#define TCC_CTRLA_PRESCSYNC_GCLK    (TCC_CTRLA_PRESCSYNC_GCLK_Val  << TCC_CTRLA_PRESCSYNC_Pos)
#define TCC_CTRLA_PRESCSYNC_PRESC   (TCC_CTRLA_PRESCSYNC_PRESC_Val << TCC_CTRLA_PRESCSYNC_Pos)
#define TCC_CTRLA_PRESCSYNC_RESYNC  (TCC_CTRLA_PRESCSYNC_RESYNC_Val << TCC_CTRLA_PRESCSYNC_Pos)
#define TCC_CTRLA_ALOCK_Pos         14           /**< \brief (TCC_CTRLA) Auto Lock */
#define TCC_CTRLA_ALOCK             (_U_(0x1) << TCC_CTRLA_ALOCK_Pos)
#define TCC_CTRLA_MSYNC_Pos         15           /**< \brief (TCC_CTRLA) Master Synchronization (only for TCC Slave Instance) */
#define TCC_CTRLA_MSYNC             (_U_(0x1) << TCC_CTRLA_MSYNC_Pos)
#define TCC_CTRLA_DMAOS_Pos         23           /**< \brief (TCC_CTRLA) DMA One-shot Trigger Mode */
#define TCC_CTRLA_DMAOS             (_U_(0x1) << TCC_CTRLA_DMAOS_Pos)
#define TCC_CTRLA_CPTEN0_Pos        24           /**< \brief (TCC_CTRLA) Capture Channel 0 Enable */
#define TCC_CTRLA_CPTEN0            (_U_(1) << TCC_CTRLA_CPTEN0_Pos)
#define TCC_CTRLA_CPTEN1_Pos        25           /**< \brief (TCC_CTRLA) Capture Channel 1 Enable */
#define TCC_CTRLA_CPTEN1            (_U_(1) << TCC_CTRLA_CPTEN1_Pos)
#define TCC_CTRLA_CPTEN2_Pos        26           /**< \brief (TCC_CTRLA) Capture Channel 2 Enable */
#define TCC_CTRLA_CPTEN2            (_U_(1) << TCC_CTRLA_CPTEN2_Pos)
#define TCC_CTRLA_CPTEN3_Pos        27           /**< \brief (TCC_CTRLA) Capture Channel 3 Enable */
#define TCC_CTRLA_CPTEN3            (_U_(1) << TCC_CTRLA_CPTEN3_Pos)
#define TCC_CTRLA_CPTEN4_Pos        28           /**< \brief (TCC_CTRLA) Capture Channel 4 Enable */
#define TCC_CTRLA_CPTEN4            (_U_(1) << TCC_CTRLA_CPTEN4_Pos)
#define TCC_CTRLA_CPTEN5_Pos        29           /**< \brief (TCC_CTRLA) Capture Channel 5 Enable */
#define TCC_CTRLA_CPTEN5            (_U_(1) << TCC_CTRLA_CPTEN5_Pos)
#define TCC_CTRLA_CPTEN_Pos         24           /**< \brief (TCC_CTRLA) Capture Channel x Enable */
#define TCC_CTRLA_CPTEN_Msk         (_U_(0x3F) << TCC_CTRLA_CPTEN_Pos)
#define TCC_CTRLA_CPTEN(value)      (TCC_CTRLA_CPTEN_Msk & ((value) << TCC_CTRLA_CPTEN_Pos))
#define TCC_CTRLA_MASK              _U_(0x3F80FF63) /**< \brief (TCC_CTRLA) MASK Register */

/* -------- TCC_CTRLBCLR : (TCC Offset: 0x04) (R/W  8) Control B Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DIR:1;            /*!< bit:      0  Counter Direction                  */
    uint8_t  LUPD:1;           /*!< bit:      1  Lock Update                        */
    uint8_t  ONESHOT:1;        /*!< bit:      2  One-Shot                           */
    uint8_t  IDXCMD:2;         /*!< bit:  3.. 4  Ramp Index Command                 */
    uint8_t  CMD:3;            /*!< bit:  5.. 7  TCC Command                        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TCC_CTRLBCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_CTRLBCLR_OFFSET         0x04         /**< \brief (TCC_CTRLBCLR offset) Control B Clear */
#define TCC_CTRLBCLR_RESETVALUE     _U_(0x00)    /**< \brief (TCC_CTRLBCLR reset_value) Control B Clear */

#define TCC_CTRLBCLR_DIR_Pos        0            /**< \brief (TCC_CTRLBCLR) Counter Direction */
#define TCC_CTRLBCLR_DIR            (_U_(0x1) << TCC_CTRLBCLR_DIR_Pos)
#define TCC_CTRLBCLR_LUPD_Pos       1            /**< \brief (TCC_CTRLBCLR) Lock Update */
#define TCC_CTRLBCLR_LUPD           (_U_(0x1) << TCC_CTRLBCLR_LUPD_Pos)
#define TCC_CTRLBCLR_ONESHOT_Pos    2            /**< \brief (TCC_CTRLBCLR) One-Shot */
#define TCC_CTRLBCLR_ONESHOT        (_U_(0x1) << TCC_CTRLBCLR_ONESHOT_Pos)
#define TCC_CTRLBCLR_IDXCMD_Pos     3            /**< \brief (TCC_CTRLBCLR) Ramp Index Command */
#define TCC_CTRLBCLR_IDXCMD_Msk     (_U_(0x3) << TCC_CTRLBCLR_IDXCMD_Pos)
#define TCC_CTRLBCLR_IDXCMD(value)  (TCC_CTRLBCLR_IDXCMD_Msk & ((value) << TCC_CTRLBCLR_IDXCMD_Pos))
#define   TCC_CTRLBCLR_IDXCMD_DISABLE_Val _U_(0x0)   /**< \brief (TCC_CTRLBCLR) Command disabled: Index toggles between cycles A and B */
#define   TCC_CTRLBCLR_IDXCMD_SET_Val     _U_(0x1)   /**< \brief (TCC_CTRLBCLR) Set index: cycle B will be forced in the next cycle */
#define   TCC_CTRLBCLR_IDXCMD_CLEAR_Val   _U_(0x2)   /**< \brief (TCC_CTRLBCLR) Clear index: cycle A will be forced in the next cycle */
#define   TCC_CTRLBCLR_IDXCMD_HOLD_Val    _U_(0x3)   /**< \brief (TCC_CTRLBCLR) Hold index: the next cycle will be the same as the current cycle */
#define TCC_CTRLBCLR_IDXCMD_DISABLE (TCC_CTRLBCLR_IDXCMD_DISABLE_Val << TCC_CTRLBCLR_IDXCMD_Pos)
#define TCC_CTRLBCLR_IDXCMD_SET     (TCC_CTRLBCLR_IDXCMD_SET_Val   << TCC_CTRLBCLR_IDXCMD_Pos)
#define TCC_CTRLBCLR_IDXCMD_CLEAR   (TCC_CTRLBCLR_IDXCMD_CLEAR_Val << TCC_CTRLBCLR_IDXCMD_Pos)
#define TCC_CTRLBCLR_IDXCMD_HOLD    (TCC_CTRLBCLR_IDXCMD_HOLD_Val  << TCC_CTRLBCLR_IDXCMD_Pos)
#define TCC_CTRLBCLR_CMD_Pos        5            /**< \brief (TCC_CTRLBCLR) TCC Command */
#define TCC_CTRLBCLR_CMD_Msk        (_U_(0x7) << TCC_CTRLBCLR_CMD_Pos)
#define TCC_CTRLBCLR_CMD(value)     (TCC_CTRLBCLR_CMD_Msk & ((value) << TCC_CTRLBCLR_CMD_Pos))
#define   TCC_CTRLBCLR_CMD_NONE_Val       _U_(0x0)   /**< \brief (TCC_CTRLBCLR) No action */
#define   TCC_CTRLBCLR_CMD_RETRIGGER_Val  _U_(0x1)   /**< \brief (TCC_CTRLBCLR) Clear start, restart or retrigger */
#define   TCC_CTRLBCLR_CMD_STOP_Val       _U_(0x2)   /**< \brief (TCC_CTRLBCLR) Force stop */
#define   TCC_CTRLBCLR_CMD_UPDATE_Val     _U_(0x3)   /**< \brief (TCC_CTRLBCLR) Force update or double buffered registers */
#define   TCC_CTRLBCLR_CMD_READSYNC_Val   _U_(0x4)   /**< \brief (TCC_CTRLBCLR) Force COUNT read synchronization */
#define   TCC_CTRLBCLR_CMD_DMAOS_Val      _U_(0x5)   /**< \brief (TCC_CTRLBCLR) One-shot DMA trigger */
#define TCC_CTRLBCLR_CMD_NONE       (TCC_CTRLBCLR_CMD_NONE_Val     << TCC_CTRLBCLR_CMD_Pos)
#define TCC_CTRLBCLR_CMD_RETRIGGER  (TCC_CTRLBCLR_CMD_RETRIGGER_Val << TCC_CTRLBCLR_CMD_Pos)
#define TCC_CTRLBCLR_CMD_STOP       (TCC_CTRLBCLR_CMD_STOP_Val     << TCC_CTRLBCLR_CMD_Pos)
#define TCC_CTRLBCLR_CMD_UPDATE     (TCC_CTRLBCLR_CMD_UPDATE_Val   << TCC_CTRLBCLR_CMD_Pos)
#define TCC_CTRLBCLR_CMD_READSYNC   (TCC_CTRLBCLR_CMD_READSYNC_Val << TCC_CTRLBCLR_CMD_Pos)
#define TCC_CTRLBCLR_CMD_DMAOS      (TCC_CTRLBCLR_CMD_DMAOS_Val    << TCC_CTRLBCLR_CMD_Pos)
#define TCC_CTRLBCLR_MASK           _U_(0xFF)    /**< \brief (TCC_CTRLBCLR) MASK Register */

/* -------- TCC_CTRLBSET : (TCC Offset: 0x05) (R/W  8) Control B Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DIR:1;            /*!< bit:      0  Counter Direction                  */
    uint8_t  LUPD:1;           /*!< bit:      1  Lock Update                        */
    uint8_t  ONESHOT:1;        /*!< bit:      2  One-Shot                           */
    uint8_t  IDXCMD:2;         /*!< bit:  3.. 4  Ramp Index Command                 */
    uint8_t  CMD:3;            /*!< bit:  5.. 7  TCC Command                        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TCC_CTRLBSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_CTRLBSET_OFFSET         0x05         /**< \brief (TCC_CTRLBSET offset) Control B Set */
#define TCC_CTRLBSET_RESETVALUE     _U_(0x00)    /**< \brief (TCC_CTRLBSET reset_value) Control B Set */

#define TCC_CTRLBSET_DIR_Pos        0            /**< \brief (TCC_CTRLBSET) Counter Direction */
#define TCC_CTRLBSET_DIR            (_U_(0x1) << TCC_CTRLBSET_DIR_Pos)
#define TCC_CTRLBSET_LUPD_Pos       1            /**< \brief (TCC_CTRLBSET) Lock Update */
#define TCC_CTRLBSET_LUPD           (_U_(0x1) << TCC_CTRLBSET_LUPD_Pos)
#define TCC_CTRLBSET_ONESHOT_Pos    2            /**< \brief (TCC_CTRLBSET) One-Shot */
#define TCC_CTRLBSET_ONESHOT        (_U_(0x1) << TCC_CTRLBSET_ONESHOT_Pos)
#define TCC_CTRLBSET_IDXCMD_Pos     3            /**< \brief (TCC_CTRLBSET) Ramp Index Command */
#define TCC_CTRLBSET_IDXCMD_Msk     (_U_(0x3) << TCC_CTRLBSET_IDXCMD_Pos)
#define TCC_CTRLBSET_IDXCMD(value)  (TCC_CTRLBSET_IDXCMD_Msk & ((value) << TCC_CTRLBSET_IDXCMD_Pos))
#define   TCC_CTRLBSET_IDXCMD_DISABLE_Val _U_(0x0)   /**< \brief (TCC_CTRLBSET) Command disabled: Index toggles between cycles A and B */
#define   TCC_CTRLBSET_IDXCMD_SET_Val     _U_(0x1)   /**< \brief (TCC_CTRLBSET) Set index: cycle B will be forced in the next cycle */
#define   TCC_CTRLBSET_IDXCMD_CLEAR_Val   _U_(0x2)   /**< \brief (TCC_CTRLBSET) Clear index: cycle A will be forced in the next cycle */
#define   TCC_CTRLBSET_IDXCMD_HOLD_Val    _U_(0x3)   /**< \brief (TCC_CTRLBSET) Hold index: the next cycle will be the same as the current cycle */
#define TCC_CTRLBSET_IDXCMD_DISABLE (TCC_CTRLBSET_IDXCMD_DISABLE_Val << TCC_CTRLBSET_IDXCMD_Pos)
#define TCC_CTRLBSET_IDXCMD_SET     (TCC_CTRLBSET_IDXCMD_SET_Val   << TCC_CTRLBSET_IDXCMD_Pos)
#define TCC_CTRLBSET_IDXCMD_CLEAR   (TCC_CTRLBSET_IDXCMD_CLEAR_Val << TCC_CTRLBSET_IDXCMD_Pos)
#define TCC_CTRLBSET_IDXCMD_HOLD    (TCC_CTRLBSET_IDXCMD_HOLD_Val  << TCC_CTRLBSET_IDXCMD_Pos)
#define TCC_CTRLBSET_CMD_Pos        5            /**< \brief (TCC_CTRLBSET) TCC Command */
#define TCC_CTRLBSET_CMD_Msk        (_U_(0x7) << TCC_CTRLBSET_CMD_Pos)
#define TCC_CTRLBSET_CMD(value)     (TCC_CTRLBSET_CMD_Msk & ((value) << TCC_CTRLBSET_CMD_Pos))
#define   TCC_CTRLBSET_CMD_NONE_Val       _U_(0x0)   /**< \brief (TCC_CTRLBSET) No action */
#define   TCC_CTRLBSET_CMD_RETRIGGER_Val  _U_(0x1)   /**< \brief (TCC_CTRLBSET) Clear start, restart or retrigger */
#define   TCC_CTRLBSET_CMD_STOP_Val       _U_(0x2)   /**< \brief (TCC_CTRLBSET) Force stop */
#define   TCC_CTRLBSET_CMD_UPDATE_Val     _U_(0x3)   /**< \brief (TCC_CTRLBSET) Force update or double buffered registers */
#define   TCC_CTRLBSET_CMD_READSYNC_Val   _U_(0x4)   /**< \brief (TCC_CTRLBSET) Force COUNT read synchronization */
#define   TCC_CTRLBSET_CMD_DMAOS_Val      _U_(0x5)   /**< \brief (TCC_CTRLBSET) One-shot DMA trigger */
#define TCC_CTRLBSET_CMD_NONE       (TCC_CTRLBSET_CMD_NONE_Val     << TCC_CTRLBSET_CMD_Pos)
#define TCC_CTRLBSET_CMD_RETRIGGER  (TCC_CTRLBSET_CMD_RETRIGGER_Val << TCC_CTRLBSET_CMD_Pos)
#define TCC_CTRLBSET_CMD_STOP       (TCC_CTRLBSET_CMD_STOP_Val     << TCC_CTRLBSET_CMD_Pos)
#define TCC_CTRLBSET_CMD_UPDATE     (TCC_CTRLBSET_CMD_UPDATE_Val   << TCC_CTRLBSET_CMD_Pos)
#define TCC_CTRLBSET_CMD_READSYNC   (TCC_CTRLBSET_CMD_READSYNC_Val << TCC_CTRLBSET_CMD_Pos)
#define TCC_CTRLBSET_CMD_DMAOS      (TCC_CTRLBSET_CMD_DMAOS_Val    << TCC_CTRLBSET_CMD_Pos)
#define TCC_CTRLBSET_MASK           _U_(0xFF)    /**< \brief (TCC_CTRLBSET) MASK Register */

/* -------- TCC_SYNCBUSY : (TCC Offset: 0x08) (R/  32) Synchronization Busy -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Swrst Busy                         */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable Busy                        */
    uint32_t CTRLB:1;          /*!< bit:      2  Ctrlb Busy                         */
    uint32_t STATUS:1;         /*!< bit:      3  Status Busy                        */
    uint32_t COUNT:1;          /*!< bit:      4  Count Busy                         */
    uint32_t PATT:1;           /*!< bit:      5  Pattern Busy                       */
    uint32_t WAVE:1;           /*!< bit:      6  Wave Busy                          */
    uint32_t PER:1;            /*!< bit:      7  Period Busy                        */
    uint32_t CC0:1;            /*!< bit:      8  Compare Channel 0 Busy             */
    uint32_t CC1:1;            /*!< bit:      9  Compare Channel 1 Busy             */
    uint32_t CC2:1;            /*!< bit:     10  Compare Channel 2 Busy             */
    uint32_t CC3:1;            /*!< bit:     11  Compare Channel 3 Busy             */
    uint32_t CC4:1;            /*!< bit:     12  Compare Channel 4 Busy             */
    uint32_t CC5:1;            /*!< bit:     13  Compare Channel 5 Busy             */
    uint32_t :18;              /*!< bit: 14..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :8;               /*!< bit:  0.. 7  Reserved                           */
    uint32_t CC:6;             /*!< bit:  8..13  Compare Channel x Busy             */
    uint32_t :18;              /*!< bit: 14..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_SYNCBUSY_OFFSET         0x08         /**< \brief (TCC_SYNCBUSY offset) Synchronization Busy */
#define TCC_SYNCBUSY_RESETVALUE     _U_(0x00000000) /**< \brief (TCC_SYNCBUSY reset_value) Synchronization Busy */

#define TCC_SYNCBUSY_SWRST_Pos      0            /**< \brief (TCC_SYNCBUSY) Swrst Busy */
#define TCC_SYNCBUSY_SWRST          (_U_(0x1) << TCC_SYNCBUSY_SWRST_Pos)
#define TCC_SYNCBUSY_ENABLE_Pos     1            /**< \brief (TCC_SYNCBUSY) Enable Busy */
#define TCC_SYNCBUSY_ENABLE         (_U_(0x1) << TCC_SYNCBUSY_ENABLE_Pos)
#define TCC_SYNCBUSY_CTRLB_Pos      2            /**< \brief (TCC_SYNCBUSY) Ctrlb Busy */
#define TCC_SYNCBUSY_CTRLB          (_U_(0x1) << TCC_SYNCBUSY_CTRLB_Pos)
#define TCC_SYNCBUSY_STATUS_Pos     3            /**< \brief (TCC_SYNCBUSY) Status Busy */
#define TCC_SYNCBUSY_STATUS         (_U_(0x1) << TCC_SYNCBUSY_STATUS_Pos)
#define TCC_SYNCBUSY_COUNT_Pos      4            /**< \brief (TCC_SYNCBUSY) Count Busy */
#define TCC_SYNCBUSY_COUNT          (_U_(0x1) << TCC_SYNCBUSY_COUNT_Pos)
#define TCC_SYNCBUSY_PATT_Pos       5            /**< \brief (TCC_SYNCBUSY) Pattern Busy */
#define TCC_SYNCBUSY_PATT           (_U_(0x1) << TCC_SYNCBUSY_PATT_Pos)
#define TCC_SYNCBUSY_WAVE_Pos       6            /**< \brief (TCC_SYNCBUSY) Wave Busy */
#define TCC_SYNCBUSY_WAVE           (_U_(0x1) << TCC_SYNCBUSY_WAVE_Pos)
#define TCC_SYNCBUSY_PER_Pos        7            /**< \brief (TCC_SYNCBUSY) Period Busy */
#define TCC_SYNCBUSY_PER            (_U_(0x1) << TCC_SYNCBUSY_PER_Pos)
#define TCC_SYNCBUSY_CC0_Pos        8            /**< \brief (TCC_SYNCBUSY) Compare Channel 0 Busy */
#define TCC_SYNCBUSY_CC0            (_U_(1) << TCC_SYNCBUSY_CC0_Pos)
#define TCC_SYNCBUSY_CC1_Pos        9            /**< \brief (TCC_SYNCBUSY) Compare Channel 1 Busy */
#define TCC_SYNCBUSY_CC1            (_U_(1) << TCC_SYNCBUSY_CC1_Pos)
#define TCC_SYNCBUSY_CC2_Pos        10           /**< \brief (TCC_SYNCBUSY) Compare Channel 2 Busy */
#define TCC_SYNCBUSY_CC2            (_U_(1) << TCC_SYNCBUSY_CC2_Pos)
#define TCC_SYNCBUSY_CC3_Pos        11           /**< \brief (TCC_SYNCBUSY) Compare Channel 3 Busy */
#define TCC_SYNCBUSY_CC3            (_U_(1) << TCC_SYNCBUSY_CC3_Pos)
#define TCC_SYNCBUSY_CC4_Pos        12           /**< \brief (TCC_SYNCBUSY) Compare Channel 4 Busy */
#define TCC_SYNCBUSY_CC4            (_U_(1) << TCC_SYNCBUSY_CC4_Pos)
#define TCC_SYNCBUSY_CC5_Pos        13           /**< \brief (TCC_SYNCBUSY) Compare Channel 5 Busy */
#define TCC_SYNCBUSY_CC5            (_U_(1) << TCC_SYNCBUSY_CC5_Pos)
#define TCC_SYNCBUSY_CC_Pos         8            /**< \brief (TCC_SYNCBUSY) Compare Channel x Busy */
#define TCC_SYNCBUSY_CC_Msk         (_U_(0x3F) << TCC_SYNCBUSY_CC_Pos)
#define TCC_SYNCBUSY_CC(value)      (TCC_SYNCBUSY_CC_Msk & ((value) << TCC_SYNCBUSY_CC_Pos))
#define TCC_SYNCBUSY_MASK           _U_(0x00003FFF) /**< \brief (TCC_SYNCBUSY) MASK Register */

/* -------- TCC_FCTRLA : (TCC Offset: 0x0C) (R/W 32) Recoverable Fault A Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SRC:2;            /*!< bit:  0.. 1  Fault A Source                     */
    uint32_t :1;               /*!< bit:      2  Reserved                           */
    uint32_t KEEP:1;           /*!< bit:      3  Fault A Keeper                     */
    uint32_t QUAL:1;           /*!< bit:      4  Fault A Qualification              */
    uint32_t BLANK:2;          /*!< bit:  5.. 6  Fault A Blanking Mode              */
    uint32_t RESTART:1;        /*!< bit:      7  Fault A Restart                    */
    uint32_t HALT:2;           /*!< bit:  8.. 9  Fault A Halt Mode                  */
    uint32_t CHSEL:2;          /*!< bit: 10..11  Fault A Capture Channel            */
    uint32_t CAPTURE:3;        /*!< bit: 12..14  Fault A Capture Action             */
    uint32_t BLANKPRESC:1;     /*!< bit:     15  Fault A Blanking Prescaler         */
    uint32_t BLANKVAL:8;       /*!< bit: 16..23  Fault A Blanking Time              */
    uint32_t FILTERVAL:4;      /*!< bit: 24..27  Fault A Filter Value               */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_FCTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_FCTRLA_OFFSET           0x0C         /**< \brief (TCC_FCTRLA offset) Recoverable Fault A Configuration */
#define TCC_FCTRLA_RESETVALUE       _U_(0x00000000) /**< \brief (TCC_FCTRLA reset_value) Recoverable Fault A Configuration */

#define TCC_FCTRLA_SRC_Pos          0            /**< \brief (TCC_FCTRLA) Fault A Source */
#define TCC_FCTRLA_SRC_Msk          (_U_(0x3) << TCC_FCTRLA_SRC_Pos)
#define TCC_FCTRLA_SRC(value)       (TCC_FCTRLA_SRC_Msk & ((value) << TCC_FCTRLA_SRC_Pos))
#define   TCC_FCTRLA_SRC_DISABLE_Val      _U_(0x0)   /**< \brief (TCC_FCTRLA) Fault input disabled */
#define   TCC_FCTRLA_SRC_ENABLE_Val       _U_(0x1)   /**< \brief (TCC_FCTRLA) MCEx (x=0,1) event input */
#define   TCC_FCTRLA_SRC_INVERT_Val       _U_(0x2)   /**< \brief (TCC_FCTRLA) Inverted MCEx (x=0,1) event input */
#define   TCC_FCTRLA_SRC_ALTFAULT_Val     _U_(0x3)   /**< \brief (TCC_FCTRLA) Alternate fault (A or B) state at the end of the previous period */
#define TCC_FCTRLA_SRC_DISABLE      (TCC_FCTRLA_SRC_DISABLE_Val    << TCC_FCTRLA_SRC_Pos)
#define TCC_FCTRLA_SRC_ENABLE       (TCC_FCTRLA_SRC_ENABLE_Val     << TCC_FCTRLA_SRC_Pos)
#define TCC_FCTRLA_SRC_INVERT       (TCC_FCTRLA_SRC_INVERT_Val     << TCC_FCTRLA_SRC_Pos)
#define TCC_FCTRLA_SRC_ALTFAULT     (TCC_FCTRLA_SRC_ALTFAULT_Val   << TCC_FCTRLA_SRC_Pos)
#define TCC_FCTRLA_KEEP_Pos         3            /**< \brief (TCC_FCTRLA) Fault A Keeper */
#define TCC_FCTRLA_KEEP             (_U_(0x1) << TCC_FCTRLA_KEEP_Pos)
#define TCC_FCTRLA_QUAL_Pos         4            /**< \brief (TCC_FCTRLA) Fault A Qualification */
#define TCC_FCTRLA_QUAL             (_U_(0x1) << TCC_FCTRLA_QUAL_Pos)
#define TCC_FCTRLA_BLANK_Pos        5            /**< \brief (TCC_FCTRLA) Fault A Blanking Mode */
#define TCC_FCTRLA_BLANK_Msk        (_U_(0x3) << TCC_FCTRLA_BLANK_Pos)
#define TCC_FCTRLA_BLANK(value)     (TCC_FCTRLA_BLANK_Msk & ((value) << TCC_FCTRLA_BLANK_Pos))
#define   TCC_FCTRLA_BLANK_START_Val      _U_(0x0)   /**< \brief (TCC_FCTRLA) Blanking applied from start of the ramp */
#define   TCC_FCTRLA_BLANK_RISE_Val       _U_(0x1)   /**< \brief (TCC_FCTRLA) Blanking applied from rising edge of the output waveform */
#define   TCC_FCTRLA_BLANK_FALL_Val       _U_(0x2)   /**< \brief (TCC_FCTRLA) Blanking applied from falling edge of the output waveform */
#define   TCC_FCTRLA_BLANK_BOTH_Val       _U_(0x3)   /**< \brief (TCC_FCTRLA) Blanking applied from each toggle of the output waveform */
#define TCC_FCTRLA_BLANK_START      (TCC_FCTRLA_BLANK_START_Val    << TCC_FCTRLA_BLANK_Pos)
#define TCC_FCTRLA_BLANK_RISE       (TCC_FCTRLA_BLANK_RISE_Val     << TCC_FCTRLA_BLANK_Pos)
#define TCC_FCTRLA_BLANK_FALL       (TCC_FCTRLA_BLANK_FALL_Val     << TCC_FCTRLA_BLANK_Pos)
#define TCC_FCTRLA_BLANK_BOTH       (TCC_FCTRLA_BLANK_BOTH_Val     << TCC_FCTRLA_BLANK_Pos)
#define TCC_FCTRLA_RESTART_Pos      7            /**< \brief (TCC_FCTRLA) Fault A Restart */
#define TCC_FCTRLA_RESTART          (_U_(0x1) << TCC_FCTRLA_RESTART_Pos)
#define TCC_FCTRLA_HALT_Pos         8            /**< \brief (TCC_FCTRLA) Fault A Halt Mode */
#define TCC_FCTRLA_HALT_Msk         (_U_(0x3) << TCC_FCTRLA_HALT_Pos)
#define TCC_FCTRLA_HALT(value)      (TCC_FCTRLA_HALT_Msk & ((value) << TCC_FCTRLA_HALT_Pos))
#define   TCC_FCTRLA_HALT_DISABLE_Val     _U_(0x0)   /**< \brief (TCC_FCTRLA) Halt action disabled */
#define   TCC_FCTRLA_HALT_HW_Val          _U_(0x1)   /**< \brief (TCC_FCTRLA) Hardware halt action */
#define   TCC_FCTRLA_HALT_SW_Val          _U_(0x2)   /**< \brief (TCC_FCTRLA) Software halt action */
#define   TCC_FCTRLA_HALT_NR_Val          _U_(0x3)   /**< \brief (TCC_FCTRLA) Non-recoverable fault */
#define TCC_FCTRLA_HALT_DISABLE     (TCC_FCTRLA_HALT_DISABLE_Val   << TCC_FCTRLA_HALT_Pos)
#define TCC_FCTRLA_HALT_HW          (TCC_FCTRLA_HALT_HW_Val        << TCC_FCTRLA_HALT_Pos)
#define TCC_FCTRLA_HALT_SW          (TCC_FCTRLA_HALT_SW_Val        << TCC_FCTRLA_HALT_Pos)
#define TCC_FCTRLA_HALT_NR          (TCC_FCTRLA_HALT_NR_Val        << TCC_FCTRLA_HALT_Pos)
#define TCC_FCTRLA_CHSEL_Pos        10           /**< \brief (TCC_FCTRLA) Fault A Capture Channel */
#define TCC_FCTRLA_CHSEL_Msk        (_U_(0x3) << TCC_FCTRLA_CHSEL_Pos)
#define TCC_FCTRLA_CHSEL(value)     (TCC_FCTRLA_CHSEL_Msk & ((value) << TCC_FCTRLA_CHSEL_Pos))
#define   TCC_FCTRLA_CHSEL_CC0_Val        _U_(0x0)   /**< \brief (TCC_FCTRLA) Capture value stored in channel 0 */
#define   TCC_FCTRLA_CHSEL_CC1_Val        _U_(0x1)   /**< \brief (TCC_FCTRLA) Capture value stored in channel 1 */
#define   TCC_FCTRLA_CHSEL_CC2_Val        _U_(0x2)   /**< \brief (TCC_FCTRLA) Capture value stored in channel 2 */
#define   TCC_FCTRLA_CHSEL_CC3_Val        _U_(0x3)   /**< \brief (TCC_FCTRLA) Capture value stored in channel 3 */
#define TCC_FCTRLA_CHSEL_CC0        (TCC_FCTRLA_CHSEL_CC0_Val      << TCC_FCTRLA_CHSEL_Pos)
#define TCC_FCTRLA_CHSEL_CC1        (TCC_FCTRLA_CHSEL_CC1_Val      << TCC_FCTRLA_CHSEL_Pos)
#define TCC_FCTRLA_CHSEL_CC2        (TCC_FCTRLA_CHSEL_CC2_Val      << TCC_FCTRLA_CHSEL_Pos)
#define TCC_FCTRLA_CHSEL_CC3        (TCC_FCTRLA_CHSEL_CC3_Val      << TCC_FCTRLA_CHSEL_Pos)
#define TCC_FCTRLA_CAPTURE_Pos      12           /**< \brief (TCC_FCTRLA) Fault A Capture Action */
#define TCC_FCTRLA_CAPTURE_Msk      (_U_(0x7) << TCC_FCTRLA_CAPTURE_Pos)
#define TCC_FCTRLA_CAPTURE(value)   (TCC_FCTRLA_CAPTURE_Msk & ((value) << TCC_FCTRLA_CAPTURE_Pos))
#define   TCC_FCTRLA_CAPTURE_DISABLE_Val  _U_(0x0)   /**< \brief (TCC_FCTRLA) No capture */
#define   TCC_FCTRLA_CAPTURE_CAPT_Val     _U_(0x1)   /**< \brief (TCC_FCTRLA) Capture on fault */
#define   TCC_FCTRLA_CAPTURE_CAPTMIN_Val  _U_(0x2)   /**< \brief (TCC_FCTRLA) Minimum capture */
#define   TCC_FCTRLA_CAPTURE_CAPTMAX_Val  _U_(0x3)   /**< \brief (TCC_FCTRLA) Maximum capture */
#define   TCC_FCTRLA_CAPTURE_LOCMIN_Val   _U_(0x4)   /**< \brief (TCC_FCTRLA) Minimum local detection */
#define   TCC_FCTRLA_CAPTURE_LOCMAX_Val   _U_(0x5)   /**< \brief (TCC_FCTRLA) Maximum local detection */
#define   TCC_FCTRLA_CAPTURE_DERIV0_Val   _U_(0x6)   /**< \brief (TCC_FCTRLA) Minimum and maximum local detection */
#define   TCC_FCTRLA_CAPTURE_CAPTMARK_Val _U_(0x7)   /**< \brief (TCC_FCTRLA) Capture with ramp index as MSB value */
#define TCC_FCTRLA_CAPTURE_DISABLE  (TCC_FCTRLA_CAPTURE_DISABLE_Val << TCC_FCTRLA_CAPTURE_Pos)
#define TCC_FCTRLA_CAPTURE_CAPT     (TCC_FCTRLA_CAPTURE_CAPT_Val   << TCC_FCTRLA_CAPTURE_Pos)
#define TCC_FCTRLA_CAPTURE_CAPTMIN  (TCC_FCTRLA_CAPTURE_CAPTMIN_Val << TCC_FCTRLA_CAPTURE_Pos)
#define TCC_FCTRLA_CAPTURE_CAPTMAX  (TCC_FCTRLA_CAPTURE_CAPTMAX_Val << TCC_FCTRLA_CAPTURE_Pos)
#define TCC_FCTRLA_CAPTURE_LOCMIN   (TCC_FCTRLA_CAPTURE_LOCMIN_Val << TCC_FCTRLA_CAPTURE_Pos)
#define TCC_FCTRLA_CAPTURE_LOCMAX   (TCC_FCTRLA_CAPTURE_LOCMAX_Val << TCC_FCTRLA_CAPTURE_Pos)
#define TCC_FCTRLA_CAPTURE_DERIV0   (TCC_FCTRLA_CAPTURE_DERIV0_Val << TCC_FCTRLA_CAPTURE_Pos)
#define TCC_FCTRLA_CAPTURE_CAPTMARK (TCC_FCTRLA_CAPTURE_CAPTMARK_Val << TCC_FCTRLA_CAPTURE_Pos)
#define TCC_FCTRLA_BLANKPRESC_Pos   15           /**< \brief (TCC_FCTRLA) Fault A Blanking Prescaler */
#define TCC_FCTRLA_BLANKPRESC       (_U_(0x1) << TCC_FCTRLA_BLANKPRESC_Pos)
#define TCC_FCTRLA_BLANKVAL_Pos     16           /**< \brief (TCC_FCTRLA) Fault A Blanking Time */
#define TCC_FCTRLA_BLANKVAL_Msk     (_U_(0xFF) << TCC_FCTRLA_BLANKVAL_Pos)
#define TCC_FCTRLA_BLANKVAL(value)  (TCC_FCTRLA_BLANKVAL_Msk & ((value) << TCC_FCTRLA_BLANKVAL_Pos))
#define TCC_FCTRLA_FILTERVAL_Pos    24           /**< \brief (TCC_FCTRLA) Fault A Filter Value */
#define TCC_FCTRLA_FILTERVAL_Msk    (_U_(0xF) << TCC_FCTRLA_FILTERVAL_Pos)
#define TCC_FCTRLA_FILTERVAL(value) (TCC_FCTRLA_FILTERVAL_Msk & ((value) << TCC_FCTRLA_FILTERVAL_Pos))
#define TCC_FCTRLA_MASK             _U_(0x0FFFFFFB) /**< \brief (TCC_FCTRLA) MASK Register */

/* -------- TCC_FCTRLB : (TCC Offset: 0x10) (R/W 32) Recoverable Fault B Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SRC:2;            /*!< bit:  0.. 1  Fault B Source                     */
    uint32_t :1;               /*!< bit:      2  Reserved                           */
    uint32_t KEEP:1;           /*!< bit:      3  Fault B Keeper                     */
    uint32_t QUAL:1;           /*!< bit:      4  Fault B Qualification              */
    uint32_t BLANK:2;          /*!< bit:  5.. 6  Fault B Blanking Mode              */
    uint32_t RESTART:1;        /*!< bit:      7  Fault B Restart                    */
    uint32_t HALT:2;           /*!< bit:  8.. 9  Fault B Halt Mode                  */
    uint32_t CHSEL:2;          /*!< bit: 10..11  Fault B Capture Channel            */
    uint32_t CAPTURE:3;        /*!< bit: 12..14  Fault B Capture Action             */
    uint32_t BLANKPRESC:1;     /*!< bit:     15  Fault B Blanking Prescaler         */
    uint32_t BLANKVAL:8;       /*!< bit: 16..23  Fault B Blanking Time              */
    uint32_t FILTERVAL:4;      /*!< bit: 24..27  Fault B Filter Value               */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_FCTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_FCTRLB_OFFSET           0x10         /**< \brief (TCC_FCTRLB offset) Recoverable Fault B Configuration */
#define TCC_FCTRLB_RESETVALUE       _U_(0x00000000) /**< \brief (TCC_FCTRLB reset_value) Recoverable Fault B Configuration */

#define TCC_FCTRLB_SRC_Pos          0            /**< \brief (TCC_FCTRLB) Fault B Source */
#define TCC_FCTRLB_SRC_Msk          (_U_(0x3) << TCC_FCTRLB_SRC_Pos)
#define TCC_FCTRLB_SRC(value)       (TCC_FCTRLB_SRC_Msk & ((value) << TCC_FCTRLB_SRC_Pos))
#define   TCC_FCTRLB_SRC_DISABLE_Val      _U_(0x0)   /**< \brief (TCC_FCTRLB) Fault input disabled */
#define   TCC_FCTRLB_SRC_ENABLE_Val       _U_(0x1)   /**< \brief (TCC_FCTRLB) MCEx (x=0,1) event input */
#define   TCC_FCTRLB_SRC_INVERT_Val       _U_(0x2)   /**< \brief (TCC_FCTRLB) Inverted MCEx (x=0,1) event input */
#define   TCC_FCTRLB_SRC_ALTFAULT_Val     _U_(0x3)   /**< \brief (TCC_FCTRLB) Alternate fault (A or B) state at the end of the previous period */
#define TCC_FCTRLB_SRC_DISABLE      (TCC_FCTRLB_SRC_DISABLE_Val    << TCC_FCTRLB_SRC_Pos)
#define TCC_FCTRLB_SRC_ENABLE       (TCC_FCTRLB_SRC_ENABLE_Val     << TCC_FCTRLB_SRC_Pos)
#define TCC_FCTRLB_SRC_INVERT       (TCC_FCTRLB_SRC_INVERT_Val     << TCC_FCTRLB_SRC_Pos)
#define TCC_FCTRLB_SRC_ALTFAULT     (TCC_FCTRLB_SRC_ALTFAULT_Val   << TCC_FCTRLB_SRC_Pos)
#define TCC_FCTRLB_KEEP_Pos         3            /**< \brief (TCC_FCTRLB) Fault B Keeper */
#define TCC_FCTRLB_KEEP             (_U_(0x1) << TCC_FCTRLB_KEEP_Pos)
#define TCC_FCTRLB_QUAL_Pos         4            /**< \brief (TCC_FCTRLB) Fault B Qualification */
#define TCC_FCTRLB_QUAL             (_U_(0x1) << TCC_FCTRLB_QUAL_Pos)
#define TCC_FCTRLB_BLANK_Pos        5            /**< \brief (TCC_FCTRLB) Fault B Blanking Mode */
#define TCC_FCTRLB_BLANK_Msk        (_U_(0x3) << TCC_FCTRLB_BLANK_Pos)
#define TCC_FCTRLB_BLANK(value)     (TCC_FCTRLB_BLANK_Msk & ((value) << TCC_FCTRLB_BLANK_Pos))
#define   TCC_FCTRLB_BLANK_START_Val      _U_(0x0)   /**< \brief (TCC_FCTRLB) Blanking applied from start of the ramp */
#define   TCC_FCTRLB_BLANK_RISE_Val       _U_(0x1)   /**< \brief (TCC_FCTRLB) Blanking applied from rising edge of the output waveform */
#define   TCC_FCTRLB_BLANK_FALL_Val       _U_(0x2)   /**< \brief (TCC_FCTRLB) Blanking applied from falling edge of the output waveform */
#define   TCC_FCTRLB_BLANK_BOTH_Val       _U_(0x3)   /**< \brief (TCC_FCTRLB) Blanking applied from each toggle of the output waveform */
#define TCC_FCTRLB_BLANK_START      (TCC_FCTRLB_BLANK_START_Val    << TCC_FCTRLB_BLANK_Pos)
#define TCC_FCTRLB_BLANK_RISE       (TCC_FCTRLB_BLANK_RISE_Val     << TCC_FCTRLB_BLANK_Pos)
#define TCC_FCTRLB_BLANK_FALL       (TCC_FCTRLB_BLANK_FALL_Val     << TCC_FCTRLB_BLANK_Pos)
#define TCC_FCTRLB_BLANK_BOTH       (TCC_FCTRLB_BLANK_BOTH_Val     << TCC_FCTRLB_BLANK_Pos)
#define TCC_FCTRLB_RESTART_Pos      7            /**< \brief (TCC_FCTRLB) Fault B Restart */
#define TCC_FCTRLB_RESTART          (_U_(0x1) << TCC_FCTRLB_RESTART_Pos)
#define TCC_FCTRLB_HALT_Pos         8            /**< \brief (TCC_FCTRLB) Fault B Halt Mode */
#define TCC_FCTRLB_HALT_Msk         (_U_(0x3) << TCC_FCTRLB_HALT_Pos)
#define TCC_FCTRLB_HALT(value)      (TCC_FCTRLB_HALT_Msk & ((value) << TCC_FCTRLB_HALT_Pos))
#define   TCC_FCTRLB_HALT_DISABLE_Val     _U_(0x0)   /**< \brief (TCC_FCTRLB) Halt action disabled */
#define   TCC_FCTRLB_HALT_HW_Val          _U_(0x1)   /**< \brief (TCC_FCTRLB) Hardware halt action */
#define   TCC_FCTRLB_HALT_SW_Val          _U_(0x2)   /**< \brief (TCC_FCTRLB) Software halt action */
#define   TCC_FCTRLB_HALT_NR_Val          _U_(0x3)   /**< \brief (TCC_FCTRLB) Non-recoverable fault */
#define TCC_FCTRLB_HALT_DISABLE     (TCC_FCTRLB_HALT_DISABLE_Val   << TCC_FCTRLB_HALT_Pos)
#define TCC_FCTRLB_HALT_HW          (TCC_FCTRLB_HALT_HW_Val        << TCC_FCTRLB_HALT_Pos)
#define TCC_FCTRLB_HALT_SW          (TCC_FCTRLB_HALT_SW_Val        << TCC_FCTRLB_HALT_Pos)
#define TCC_FCTRLB_HALT_NR          (TCC_FCTRLB_HALT_NR_Val        << TCC_FCTRLB_HALT_Pos)
#define TCC_FCTRLB_CHSEL_Pos        10           /**< \brief (TCC_FCTRLB) Fault B Capture Channel */
#define TCC_FCTRLB_CHSEL_Msk        (_U_(0x3) << TCC_FCTRLB_CHSEL_Pos)
#define TCC_FCTRLB_CHSEL(value)     (TCC_FCTRLB_CHSEL_Msk & ((value) << TCC_FCTRLB_CHSEL_Pos))
#define   TCC_FCTRLB_CHSEL_CC0_Val        _U_(0x0)   /**< \brief (TCC_FCTRLB) Capture value stored in channel 0 */
#define   TCC_FCTRLB_CHSEL_CC1_Val        _U_(0x1)   /**< \brief (TCC_FCTRLB) Capture value stored in channel 1 */
#define   TCC_FCTRLB_CHSEL_CC2_Val        _U_(0x2)   /**< \brief (TCC_FCTRLB) Capture value stored in channel 2 */
#define   TCC_FCTRLB_CHSEL_CC3_Val        _U_(0x3)   /**< \brief (TCC_FCTRLB) Capture value stored in channel 3 */
#define TCC_FCTRLB_CHSEL_CC0        (TCC_FCTRLB_CHSEL_CC0_Val      << TCC_FCTRLB_CHSEL_Pos)
#define TCC_FCTRLB_CHSEL_CC1        (TCC_FCTRLB_CHSEL_CC1_Val      << TCC_FCTRLB_CHSEL_Pos)
#define TCC_FCTRLB_CHSEL_CC2        (TCC_FCTRLB_CHSEL_CC2_Val      << TCC_FCTRLB_CHSEL_Pos)
#define TCC_FCTRLB_CHSEL_CC3        (TCC_FCTRLB_CHSEL_CC3_Val      << TCC_FCTRLB_CHSEL_Pos)
#define TCC_FCTRLB_CAPTURE_Pos      12           /**< \brief (TCC_FCTRLB) Fault B Capture Action */
#define TCC_FCTRLB_CAPTURE_Msk      (_U_(0x7) << TCC_FCTRLB_CAPTURE_Pos)
#define TCC_FCTRLB_CAPTURE(value)   (TCC_FCTRLB_CAPTURE_Msk & ((value) << TCC_FCTRLB_CAPTURE_Pos))
#define   TCC_FCTRLB_CAPTURE_DISABLE_Val  _U_(0x0)   /**< \brief (TCC_FCTRLB) No capture */
#define   TCC_FCTRLB_CAPTURE_CAPT_Val     _U_(0x1)   /**< \brief (TCC_FCTRLB) Capture on fault */
#define   TCC_FCTRLB_CAPTURE_CAPTMIN_Val  _U_(0x2)   /**< \brief (TCC_FCTRLB) Minimum capture */
#define   TCC_FCTRLB_CAPTURE_CAPTMAX_Val  _U_(0x3)   /**< \brief (TCC_FCTRLB) Maximum capture */
#define   TCC_FCTRLB_CAPTURE_LOCMIN_Val   _U_(0x4)   /**< \brief (TCC_FCTRLB) Minimum local detection */
#define   TCC_FCTRLB_CAPTURE_LOCMAX_Val   _U_(0x5)   /**< \brief (TCC_FCTRLB) Maximum local detection */
#define   TCC_FCTRLB_CAPTURE_DERIV0_Val   _U_(0x6)   /**< \brief (TCC_FCTRLB) Minimum and maximum local detection */
#define   TCC_FCTRLB_CAPTURE_CAPTMARK_Val _U_(0x7)   /**< \brief (TCC_FCTRLB) Capture with ramp index as MSB value */
#define TCC_FCTRLB_CAPTURE_DISABLE  (TCC_FCTRLB_CAPTURE_DISABLE_Val << TCC_FCTRLB_CAPTURE_Pos)
#define TCC_FCTRLB_CAPTURE_CAPT     (TCC_FCTRLB_CAPTURE_CAPT_Val   << TCC_FCTRLB_CAPTURE_Pos)
#define TCC_FCTRLB_CAPTURE_CAPTMIN  (TCC_FCTRLB_CAPTURE_CAPTMIN_Val << TCC_FCTRLB_CAPTURE_Pos)
#define TCC_FCTRLB_CAPTURE_CAPTMAX  (TCC_FCTRLB_CAPTURE_CAPTMAX_Val << TCC_FCTRLB_CAPTURE_Pos)
#define TCC_FCTRLB_CAPTURE_LOCMIN   (TCC_FCTRLB_CAPTURE_LOCMIN_Val << TCC_FCTRLB_CAPTURE_Pos)
#define TCC_FCTRLB_CAPTURE_LOCMAX   (TCC_FCTRLB_CAPTURE_LOCMAX_Val << TCC_FCTRLB_CAPTURE_Pos)
#define TCC_FCTRLB_CAPTURE_DERIV0   (TCC_FCTRLB_CAPTURE_DERIV0_Val << TCC_FCTRLB_CAPTURE_Pos)
#define TCC_FCTRLB_CAPTURE_CAPTMARK (TCC_FCTRLB_CAPTURE_CAPTMARK_Val << TCC_FCTRLB_CAPTURE_Pos)
#define TCC_FCTRLB_BLANKPRESC_Pos   15           /**< \brief (TCC_FCTRLB) Fault B Blanking Prescaler */
#define TCC_FCTRLB_BLANKPRESC       (_U_(0x1) << TCC_FCTRLB_BLANKPRESC_Pos)
#define TCC_FCTRLB_BLANKVAL_Pos     16           /**< \brief (TCC_FCTRLB) Fault B Blanking Time */
#define TCC_FCTRLB_BLANKVAL_Msk     (_U_(0xFF) << TCC_FCTRLB_BLANKVAL_Pos)
#define TCC_FCTRLB_BLANKVAL(value)  (TCC_FCTRLB_BLANKVAL_Msk & ((value) << TCC_FCTRLB_BLANKVAL_Pos))
#define TCC_FCTRLB_FILTERVAL_Pos    24           /**< \brief (TCC_FCTRLB) Fault B Filter Value */
#define TCC_FCTRLB_FILTERVAL_Msk    (_U_(0xF) << TCC_FCTRLB_FILTERVAL_Pos)
#define TCC_FCTRLB_FILTERVAL(value) (TCC_FCTRLB_FILTERVAL_Msk & ((value) << TCC_FCTRLB_FILTERVAL_Pos))
#define TCC_FCTRLB_MASK             _U_(0x0FFFFFFB) /**< \brief (TCC_FCTRLB) MASK Register */

/* -------- TCC_WEXCTRL : (TCC Offset: 0x14) (R/W 32) Waveform Extension Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t OTMX:2;           /*!< bit:  0.. 1  Output Matrix                      */
    uint32_t :6;               /*!< bit:  2.. 7  Reserved                           */
    uint32_t DTIEN0:1;         /*!< bit:      8  Dead-time Insertion Generator 0 Enable */
    uint32_t DTIEN1:1;         /*!< bit:      9  Dead-time Insertion Generator 1 Enable */
    uint32_t DTIEN2:1;         /*!< bit:     10  Dead-time Insertion Generator 2 Enable */
    uint32_t DTIEN3:1;         /*!< bit:     11  Dead-time Insertion Generator 3 Enable */
    uint32_t :4;               /*!< bit: 12..15  Reserved                           */
    uint32_t DTLS:8;           /*!< bit: 16..23  Dead-time Low Side Outputs Value   */
    uint32_t DTHS:8;           /*!< bit: 24..31  Dead-time High Side Outputs Value  */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :8;               /*!< bit:  0.. 7  Reserved                           */
    uint32_t DTIEN:4;          /*!< bit:  8..11  Dead-time Insertion Generator x Enable */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_WEXCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_WEXCTRL_OFFSET          0x14         /**< \brief (TCC_WEXCTRL offset) Waveform Extension Configuration */
#define TCC_WEXCTRL_RESETVALUE      _U_(0x00000000) /**< \brief (TCC_WEXCTRL reset_value) Waveform Extension Configuration */

#define TCC_WEXCTRL_OTMX_Pos        0            /**< \brief (TCC_WEXCTRL) Output Matrix */
#define TCC_WEXCTRL_OTMX_Msk        (_U_(0x3) << TCC_WEXCTRL_OTMX_Pos)
#define TCC_WEXCTRL_OTMX(value)     (TCC_WEXCTRL_OTMX_Msk & ((value) << TCC_WEXCTRL_OTMX_Pos))
#define TCC_WEXCTRL_DTIEN0_Pos      8            /**< \brief (TCC_WEXCTRL) Dead-time Insertion Generator 0 Enable */
#define TCC_WEXCTRL_DTIEN0          (_U_(1) << TCC_WEXCTRL_DTIEN0_Pos)
#define TCC_WEXCTRL_DTIEN1_Pos      9            /**< \brief (TCC_WEXCTRL) Dead-time Insertion Generator 1 Enable */
#define TCC_WEXCTRL_DTIEN1          (_U_(1) << TCC_WEXCTRL_DTIEN1_Pos)
#define TCC_WEXCTRL_DTIEN2_Pos      10           /**< \brief (TCC_WEXCTRL) Dead-time Insertion Generator 2 Enable */
#define TCC_WEXCTRL_DTIEN2          (_U_(1) << TCC_WEXCTRL_DTIEN2_Pos)
#define TCC_WEXCTRL_DTIEN3_Pos      11           /**< \brief (TCC_WEXCTRL) Dead-time Insertion Generator 3 Enable */
#define TCC_WEXCTRL_DTIEN3          (_U_(1) << TCC_WEXCTRL_DTIEN3_Pos)
#define TCC_WEXCTRL_DTIEN_Pos       8            /**< \brief (TCC_WEXCTRL) Dead-time Insertion Generator x Enable */
#define TCC_WEXCTRL_DTIEN_Msk       (_U_(0xF) << TCC_WEXCTRL_DTIEN_Pos)
#define TCC_WEXCTRL_DTIEN(value)    (TCC_WEXCTRL_DTIEN_Msk & ((value) << TCC_WEXCTRL_DTIEN_Pos))
#define TCC_WEXCTRL_DTLS_Pos        16           /**< \brief (TCC_WEXCTRL) Dead-time Low Side Outputs Value */
#define TCC_WEXCTRL_DTLS_Msk        (_U_(0xFF) << TCC_WEXCTRL_DTLS_Pos)
#define TCC_WEXCTRL_DTLS(value)     (TCC_WEXCTRL_DTLS_Msk & ((value) << TCC_WEXCTRL_DTLS_Pos))
#define TCC_WEXCTRL_DTHS_Pos        24           /**< \brief (TCC_WEXCTRL) Dead-time High Side Outputs Value */
#define TCC_WEXCTRL_DTHS_Msk        (_U_(0xFF) << TCC_WEXCTRL_DTHS_Pos)
#define TCC_WEXCTRL_DTHS(value)     (TCC_WEXCTRL_DTHS_Msk & ((value) << TCC_WEXCTRL_DTHS_Pos))
#define TCC_WEXCTRL_MASK            _U_(0xFFFF0F03) /**< \brief (TCC_WEXCTRL) MASK Register */

/* -------- TCC_DRVCTRL : (TCC Offset: 0x18) (R/W 32) Driver Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t NRE0:1;           /*!< bit:      0  Non-Recoverable State 0 Output Enable */
    uint32_t NRE1:1;           /*!< bit:      1  Non-Recoverable State 1 Output Enable */
    uint32_t NRE2:1;           /*!< bit:      2  Non-Recoverable State 2 Output Enable */
    uint32_t NRE3:1;           /*!< bit:      3  Non-Recoverable State 3 Output Enable */
    uint32_t NRE4:1;           /*!< bit:      4  Non-Recoverable State 4 Output Enable */
    uint32_t NRE5:1;           /*!< bit:      5  Non-Recoverable State 5 Output Enable */
    uint32_t NRE6:1;           /*!< bit:      6  Non-Recoverable State 6 Output Enable */
    uint32_t NRE7:1;           /*!< bit:      7  Non-Recoverable State 7 Output Enable */
    uint32_t NRV0:1;           /*!< bit:      8  Non-Recoverable State 0 Output Value */
    uint32_t NRV1:1;           /*!< bit:      9  Non-Recoverable State 1 Output Value */
    uint32_t NRV2:1;           /*!< bit:     10  Non-Recoverable State 2 Output Value */
    uint32_t NRV3:1;           /*!< bit:     11  Non-Recoverable State 3 Output Value */
    uint32_t NRV4:1;           /*!< bit:     12  Non-Recoverable State 4 Output Value */
    uint32_t NRV5:1;           /*!< bit:     13  Non-Recoverable State 5 Output Value */
    uint32_t NRV6:1;           /*!< bit:     14  Non-Recoverable State 6 Output Value */
    uint32_t NRV7:1;           /*!< bit:     15  Non-Recoverable State 7 Output Value */
    uint32_t INVEN0:1;         /*!< bit:     16  Output Waveform 0 Inversion        */
    uint32_t INVEN1:1;         /*!< bit:     17  Output Waveform 1 Inversion        */
    uint32_t INVEN2:1;         /*!< bit:     18  Output Waveform 2 Inversion        */
    uint32_t INVEN3:1;         /*!< bit:     19  Output Waveform 3 Inversion        */
    uint32_t INVEN4:1;         /*!< bit:     20  Output Waveform 4 Inversion        */
    uint32_t INVEN5:1;         /*!< bit:     21  Output Waveform 5 Inversion        */
    uint32_t INVEN6:1;         /*!< bit:     22  Output Waveform 6 Inversion        */
    uint32_t INVEN7:1;         /*!< bit:     23  Output Waveform 7 Inversion        */
    uint32_t FILTERVAL0:4;     /*!< bit: 24..27  Non-Recoverable Fault Input 0 Filter Value */
    uint32_t FILTERVAL1:4;     /*!< bit: 28..31  Non-Recoverable Fault Input 1 Filter Value */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t NRE:8;            /*!< bit:  0.. 7  Non-Recoverable State x Output Enable */
    uint32_t NRV:8;            /*!< bit:  8..15  Non-Recoverable State x Output Value */
    uint32_t INVEN:8;          /*!< bit: 16..23  Output Waveform x Inversion        */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_DRVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_DRVCTRL_OFFSET          0x18         /**< \brief (TCC_DRVCTRL offset) Driver Control */
#define TCC_DRVCTRL_RESETVALUE      _U_(0x00000000) /**< \brief (TCC_DRVCTRL reset_value) Driver Control */

#define TCC_DRVCTRL_NRE0_Pos        0            /**< \brief (TCC_DRVCTRL) Non-Recoverable State 0 Output Enable */
#define TCC_DRVCTRL_NRE0            (_U_(1) << TCC_DRVCTRL_NRE0_Pos)
#define TCC_DRVCTRL_NRE1_Pos        1            /**< \brief (TCC_DRVCTRL) Non-Recoverable State 1 Output Enable */
#define TCC_DRVCTRL_NRE1            (_U_(1) << TCC_DRVCTRL_NRE1_Pos)
#define TCC_DRVCTRL_NRE2_Pos        2            /**< \brief (TCC_DRVCTRL) Non-Recoverable State 2 Output Enable */
#define TCC_DRVCTRL_NRE2            (_U_(1) << TCC_DRVCTRL_NRE2_Pos)
#define TCC_DRVCTRL_NRE3_Pos        3            /**< \brief (TCC_DRVCTRL) Non-Recoverable State 3 Output Enable */
#define TCC_DRVCTRL_NRE3            (_U_(1) << TCC_DRVCTRL_NRE3_Pos)
#define TCC_DRVCTRL_NRE4_Pos        4            /**< \brief (TCC_DRVCTRL) Non-Recoverable State 4 Output Enable */
#define TCC_DRVCTRL_NRE4            (_U_(1) << TCC_DRVCTRL_NRE4_Pos)
#define TCC_DRVCTRL_NRE5_Pos        5            /**< \brief (TCC_DRVCTRL) Non-Recoverable State 5 Output Enable */
#define TCC_DRVCTRL_NRE5            (_U_(1) << TCC_DRVCTRL_NRE5_Pos)
#define TCC_DRVCTRL_NRE6_Pos        6            /**< \brief (TCC_DRVCTRL) Non-Recoverable State 6 Output Enable */
#define TCC_DRVCTRL_NRE6            (_U_(1) << TCC_DRVCTRL_NRE6_Pos)
#define TCC_DRVCTRL_NRE7_Pos        7            /**< \brief (TCC_DRVCTRL) Non-Recoverable State 7 Output Enable */
#define TCC_DRVCTRL_NRE7            (_U_(1) << TCC_DRVCTRL_NRE7_Pos)
#define TCC_DRVCTRL_NRE_Pos         0            /**< \brief (TCC_DRVCTRL) Non-Recoverable State x Output Enable */
#define TCC_DRVCTRL_NRE_Msk         (_U_(0xFF) << TCC_DRVCTRL_NRE_Pos)
#define TCC_DRVCTRL_NRE(value)      (TCC_DRVCTRL_NRE_Msk & ((value) << TCC_DRVCTRL_NRE_Pos))
#define TCC_DRVCTRL_NRV0_Pos        8            /**< \brief (TCC_DRVCTRL) Non-Recoverable State 0 Output Value */
#define TCC_DRVCTRL_NRV0            (_U_(1) << TCC_DRVCTRL_NRV0_Pos)
#define TCC_DRVCTRL_NRV1_Pos        9            /**< \brief (TCC_DRVCTRL) Non-Recoverable State 1 Output Value */
#define TCC_DRVCTRL_NRV1            (_U_(1) << TCC_DRVCTRL_NRV1_Pos)
#define TCC_DRVCTRL_NRV2_Pos        10           /**< \brief (TCC_DRVCTRL) Non-Recoverable State 2 Output Value */
#define TCC_DRVCTRL_NRV2            (_U_(1) << TCC_DRVCTRL_NRV2_Pos)
#define TCC_DRVCTRL_NRV3_Pos        11           /**< \brief (TCC_DRVCTRL) Non-Recoverable State 3 Output Value */
#define TCC_DRVCTRL_NRV3            (_U_(1) << TCC_DRVCTRL_NRV3_Pos)
#define TCC_DRVCTRL_NRV4_Pos        12           /**< \brief (TCC_DRVCTRL) Non-Recoverable State 4 Output Value */
#define TCC_DRVCTRL_NRV4            (_U_(1) << TCC_DRVCTRL_NRV4_Pos)
#define TCC_DRVCTRL_NRV5_Pos        13           /**< \brief (TCC_DRVCTRL) Non-Recoverable State 5 Output Value */
#define TCC_DRVCTRL_NRV5            (_U_(1) << TCC_DRVCTRL_NRV5_Pos)
#define TCC_DRVCTRL_NRV6_Pos        14           /**< \brief (TCC_DRVCTRL) Non-Recoverable State 6 Output Value */
#define TCC_DRVCTRL_NRV6            (_U_(1) << TCC_DRVCTRL_NRV6_Pos)
#define TCC_DRVCTRL_NRV7_Pos        15           /**< \brief (TCC_DRVCTRL) Non-Recoverable State 7 Output Value */
#define TCC_DRVCTRL_NRV7            (_U_(1) << TCC_DRVCTRL_NRV7_Pos)
#define TCC_DRVCTRL_NRV_Pos         8            /**< \brief (TCC_DRVCTRL) Non-Recoverable State x Output Value */
#define TCC_DRVCTRL_NRV_Msk         (_U_(0xFF) << TCC_DRVCTRL_NRV_Pos)
#define TCC_DRVCTRL_NRV(value)      (TCC_DRVCTRL_NRV_Msk & ((value) << TCC_DRVCTRL_NRV_Pos))
#define TCC_DRVCTRL_INVEN0_Pos      16           /**< \brief (TCC_DRVCTRL) Output Waveform 0 Inversion */
#define TCC_DRVCTRL_INVEN0          (_U_(1) << TCC_DRVCTRL_INVEN0_Pos)
#define TCC_DRVCTRL_INVEN1_Pos      17           /**< \brief (TCC_DRVCTRL) Output Waveform 1 Inversion */
#define TCC_DRVCTRL_INVEN1          (_U_(1) << TCC_DRVCTRL_INVEN1_Pos)
#define TCC_DRVCTRL_INVEN2_Pos      18           /**< \brief (TCC_DRVCTRL) Output Waveform 2 Inversion */
#define TCC_DRVCTRL_INVEN2          (_U_(1) << TCC_DRVCTRL_INVEN2_Pos)
#define TCC_DRVCTRL_INVEN3_Pos      19           /**< \brief (TCC_DRVCTRL) Output Waveform 3 Inversion */
#define TCC_DRVCTRL_INVEN3          (_U_(1) << TCC_DRVCTRL_INVEN3_Pos)
#define TCC_DRVCTRL_INVEN4_Pos      20           /**< \brief (TCC_DRVCTRL) Output Waveform 4 Inversion */
#define TCC_DRVCTRL_INVEN4          (_U_(1) << TCC_DRVCTRL_INVEN4_Pos)
#define TCC_DRVCTRL_INVEN5_Pos      21           /**< \brief (TCC_DRVCTRL) Output Waveform 5 Inversion */
#define TCC_DRVCTRL_INVEN5          (_U_(1) << TCC_DRVCTRL_INVEN5_Pos)
#define TCC_DRVCTRL_INVEN6_Pos      22           /**< \brief (TCC_DRVCTRL) Output Waveform 6 Inversion */
#define TCC_DRVCTRL_INVEN6          (_U_(1) << TCC_DRVCTRL_INVEN6_Pos)
#define TCC_DRVCTRL_INVEN7_Pos      23           /**< \brief (TCC_DRVCTRL) Output Waveform 7 Inversion */
#define TCC_DRVCTRL_INVEN7          (_U_(1) << TCC_DRVCTRL_INVEN7_Pos)
#define TCC_DRVCTRL_INVEN_Pos       16           /**< \brief (TCC_DRVCTRL) Output Waveform x Inversion */
#define TCC_DRVCTRL_INVEN_Msk       (_U_(0xFF) << TCC_DRVCTRL_INVEN_Pos)
#define TCC_DRVCTRL_INVEN(value)    (TCC_DRVCTRL_INVEN_Msk & ((value) << TCC_DRVCTRL_INVEN_Pos))
#define TCC_DRVCTRL_FILTERVAL0_Pos  24           /**< \brief (TCC_DRVCTRL) Non-Recoverable Fault Input 0 Filter Value */
#define TCC_DRVCTRL_FILTERVAL0_Msk  (_U_(0xF) << TCC_DRVCTRL_FILTERVAL0_Pos)
#define TCC_DRVCTRL_FILTERVAL0(value) (TCC_DRVCTRL_FILTERVAL0_Msk & ((value) << TCC_DRVCTRL_FILTERVAL0_Pos))
#define TCC_DRVCTRL_FILTERVAL1_Pos  28           /**< \brief (TCC_DRVCTRL) Non-Recoverable Fault Input 1 Filter Value */
#define TCC_DRVCTRL_FILTERVAL1_Msk  (_U_(0xF) << TCC_DRVCTRL_FILTERVAL1_Pos)
#define TCC_DRVCTRL_FILTERVAL1(value) (TCC_DRVCTRL_FILTERVAL1_Msk & ((value) << TCC_DRVCTRL_FILTERVAL1_Pos))
#define TCC_DRVCTRL_MASK            _U_(0xFFFFFFFF) /**< \brief (TCC_DRVCTRL) MASK Register */

/* -------- TCC_DBGCTRL : (TCC Offset: 0x1E) (R/W  8) Debug Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DBGRUN:1;         /*!< bit:      0  Debug Running Mode                 */
    uint8_t  :1;               /*!< bit:      1  Reserved                           */
    uint8_t  FDDBD:1;          /*!< bit:      2  Fault Detection on Debug Break Detection */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TCC_DBGCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_DBGCTRL_OFFSET          0x1E         /**< \brief (TCC_DBGCTRL offset) Debug Control */
#define TCC_DBGCTRL_RESETVALUE      _U_(0x00)    /**< \brief (TCC_DBGCTRL reset_value) Debug Control */

#define TCC_DBGCTRL_DBGRUN_Pos      0            /**< \brief (TCC_DBGCTRL) Debug Running Mode */
#define TCC_DBGCTRL_DBGRUN          (_U_(0x1) << TCC_DBGCTRL_DBGRUN_Pos)
#define TCC_DBGCTRL_FDDBD_Pos       2            /**< \brief (TCC_DBGCTRL) Fault Detection on Debug Break Detection */
#define TCC_DBGCTRL_FDDBD           (_U_(0x1) << TCC_DBGCTRL_FDDBD_Pos)
#define TCC_DBGCTRL_MASK            _U_(0x05)    /**< \brief (TCC_DBGCTRL) MASK Register */

/* -------- TCC_EVCTRL : (TCC Offset: 0x20) (R/W 32) Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EVACT0:3;         /*!< bit:  0.. 2  Timer/counter Input Event0 Action  */
    uint32_t EVACT1:3;         /*!< bit:  3.. 5  Timer/counter Input Event1 Action  */
    uint32_t CNTSEL:2;         /*!< bit:  6.. 7  Timer/counter Output Event Mode    */
    uint32_t OVFEO:1;          /*!< bit:      8  Overflow/Underflow Output Event Enable */
    uint32_t TRGEO:1;          /*!< bit:      9  Retrigger Output Event Enable      */
    uint32_t CNTEO:1;          /*!< bit:     10  Timer/counter Output Event Enable  */
    uint32_t :1;               /*!< bit:     11  Reserved                           */
    uint32_t TCINV0:1;         /*!< bit:     12  Inverted Event 0 Input Enable      */
    uint32_t TCINV1:1;         /*!< bit:     13  Inverted Event 1 Input Enable      */
    uint32_t TCEI0:1;          /*!< bit:     14  Timer/counter Event 0 Input Enable */
    uint32_t TCEI1:1;          /*!< bit:     15  Timer/counter Event 1 Input Enable */
    uint32_t MCEI0:1;          /*!< bit:     16  Match or Capture Channel 0 Event Input Enable */
    uint32_t MCEI1:1;          /*!< bit:     17  Match or Capture Channel 1 Event Input Enable */
    uint32_t MCEI2:1;          /*!< bit:     18  Match or Capture Channel 2 Event Input Enable */
    uint32_t MCEI3:1;          /*!< bit:     19  Match or Capture Channel 3 Event Input Enable */
    uint32_t MCEI4:1;          /*!< bit:     20  Match or Capture Channel 4 Event Input Enable */
    uint32_t MCEI5:1;          /*!< bit:     21  Match or Capture Channel 5 Event Input Enable */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t MCEO0:1;          /*!< bit:     24  Match or Capture Channel 0 Event Output Enable */
    uint32_t MCEO1:1;          /*!< bit:     25  Match or Capture Channel 1 Event Output Enable */
    uint32_t MCEO2:1;          /*!< bit:     26  Match or Capture Channel 2 Event Output Enable */
    uint32_t MCEO3:1;          /*!< bit:     27  Match or Capture Channel 3 Event Output Enable */
    uint32_t MCEO4:1;          /*!< bit:     28  Match or Capture Channel 4 Event Output Enable */
    uint32_t MCEO5:1;          /*!< bit:     29  Match or Capture Channel 5 Event Output Enable */
    uint32_t :2;               /*!< bit: 30..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :12;              /*!< bit:  0..11  Reserved                           */
    uint32_t TCINV:2;          /*!< bit: 12..13  Inverted Event x Input Enable      */
    uint32_t TCEI:2;           /*!< bit: 14..15  Timer/counter Event x Input Enable */
    uint32_t MCEI:6;           /*!< bit: 16..21  Match or Capture Channel x Event Input Enable */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t MCEO:6;           /*!< bit: 24..29  Match or Capture Channel x Event Output Enable */
    uint32_t :2;               /*!< bit: 30..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_EVCTRL_OFFSET           0x20         /**< \brief (TCC_EVCTRL offset) Event Control */
#define TCC_EVCTRL_RESETVALUE       _U_(0x00000000) /**< \brief (TCC_EVCTRL reset_value) Event Control */

#define TCC_EVCTRL_EVACT0_Pos       0            /**< \brief (TCC_EVCTRL) Timer/counter Input Event0 Action */
#define TCC_EVCTRL_EVACT0_Msk       (_U_(0x7) << TCC_EVCTRL_EVACT0_Pos)
#define TCC_EVCTRL_EVACT0(value)    (TCC_EVCTRL_EVACT0_Msk & ((value) << TCC_EVCTRL_EVACT0_Pos))
#define   TCC_EVCTRL_EVACT0_OFF_Val       _U_(0x0)   /**< \brief (TCC_EVCTRL) Event action disabled */
#define   TCC_EVCTRL_EVACT0_RETRIGGER_Val _U_(0x1)   /**< \brief (TCC_EVCTRL) Start, restart or re-trigger counter on event */
#define   TCC_EVCTRL_EVACT0_COUNTEV_Val   _U_(0x2)   /**< \brief (TCC_EVCTRL) Count on event */
#define   TCC_EVCTRL_EVACT0_START_Val     _U_(0x3)   /**< \brief (TCC_EVCTRL) Start counter on event */
#define   TCC_EVCTRL_EVACT0_INC_Val       _U_(0x4)   /**< \brief (TCC_EVCTRL) Increment counter on event */
#define   TCC_EVCTRL_EVACT0_COUNT_Val     _U_(0x5)   /**< \brief (TCC_EVCTRL) Count on active state of asynchronous event */
#define   TCC_EVCTRL_EVACT0_STAMP_Val     _U_(0x6)   /**< \brief (TCC_EVCTRL) Stamp capture */
#define   TCC_EVCTRL_EVACT0_FAULT_Val     _U_(0x7)   /**< \brief (TCC_EVCTRL) Non-recoverable fault */
#define TCC_EVCTRL_EVACT0_OFF       (TCC_EVCTRL_EVACT0_OFF_Val     << TCC_EVCTRL_EVACT0_Pos)
#define TCC_EVCTRL_EVACT0_RETRIGGER (TCC_EVCTRL_EVACT0_RETRIGGER_Val << TCC_EVCTRL_EVACT0_Pos)
#define TCC_EVCTRL_EVACT0_COUNTEV   (TCC_EVCTRL_EVACT0_COUNTEV_Val << TCC_EVCTRL_EVACT0_Pos)
#define TCC_EVCTRL_EVACT0_START     (TCC_EVCTRL_EVACT0_START_Val   << TCC_EVCTRL_EVACT0_Pos)
#define TCC_EVCTRL_EVACT0_INC       (TCC_EVCTRL_EVACT0_INC_Val     << TCC_EVCTRL_EVACT0_Pos)
#define TCC_EVCTRL_EVACT0_COUNT     (TCC_EVCTRL_EVACT0_COUNT_Val   << TCC_EVCTRL_EVACT0_Pos)
#define TCC_EVCTRL_EVACT0_STAMP     (TCC_EVCTRL_EVACT0_STAMP_Val   << TCC_EVCTRL_EVACT0_Pos)
#define TCC_EVCTRL_EVACT0_FAULT     (TCC_EVCTRL_EVACT0_FAULT_Val   << TCC_EVCTRL_EVACT0_Pos)
#define TCC_EVCTRL_EVACT1_Pos       3            /**< \brief (TCC_EVCTRL) Timer/counter Input Event1 Action */
#define TCC_EVCTRL_EVACT1_Msk       (_U_(0x7) << TCC_EVCTRL_EVACT1_Pos)
#define TCC_EVCTRL_EVACT1(value)    (TCC_EVCTRL_EVACT1_Msk & ((value) << TCC_EVCTRL_EVACT1_Pos))
#define   TCC_EVCTRL_EVACT1_OFF_Val       _U_(0x0)   /**< \brief (TCC_EVCTRL) Event action disabled */
#define   TCC_EVCTRL_EVACT1_RETRIGGER_Val _U_(0x1)   /**< \brief (TCC_EVCTRL) Re-trigger counter on event */
#define   TCC_EVCTRL_EVACT1_DIR_Val       _U_(0x2)   /**< \brief (TCC_EVCTRL) Direction control */
#define   TCC_EVCTRL_EVACT1_STOP_Val      _U_(0x3)   /**< \brief (TCC_EVCTRL) Stop counter on event */
#define   TCC_EVCTRL_EVACT1_DEC_Val       _U_(0x4)   /**< \brief (TCC_EVCTRL) Decrement counter on event */
#define   TCC_EVCTRL_EVACT1_PPW_Val       _U_(0x5)   /**< \brief (TCC_EVCTRL) Period capture value in CC0 register, pulse width capture value in CC1 register */
#define   TCC_EVCTRL_EVACT1_PWP_Val       _U_(0x6)   /**< \brief (TCC_EVCTRL) Period capture value in CC1 register, pulse width capture value in CC0 register */
#define   TCC_EVCTRL_EVACT1_FAULT_Val     _U_(0x7)   /**< \brief (TCC_EVCTRL) Non-recoverable fault */
#define TCC_EVCTRL_EVACT1_OFF       (TCC_EVCTRL_EVACT1_OFF_Val     << TCC_EVCTRL_EVACT1_Pos)
#define TCC_EVCTRL_EVACT1_RETRIGGER (TCC_EVCTRL_EVACT1_RETRIGGER_Val << TCC_EVCTRL_EVACT1_Pos)
#define TCC_EVCTRL_EVACT1_DIR       (TCC_EVCTRL_EVACT1_DIR_Val     << TCC_EVCTRL_EVACT1_Pos)
#define TCC_EVCTRL_EVACT1_STOP      (TCC_EVCTRL_EVACT1_STOP_Val    << TCC_EVCTRL_EVACT1_Pos)
#define TCC_EVCTRL_EVACT1_DEC       (TCC_EVCTRL_EVACT1_DEC_Val     << TCC_EVCTRL_EVACT1_Pos)
#define TCC_EVCTRL_EVACT1_PPW       (TCC_EVCTRL_EVACT1_PPW_Val     << TCC_EVCTRL_EVACT1_Pos)
#define TCC_EVCTRL_EVACT1_PWP       (TCC_EVCTRL_EVACT1_PWP_Val     << TCC_EVCTRL_EVACT1_Pos)
#define TCC_EVCTRL_EVACT1_FAULT     (TCC_EVCTRL_EVACT1_FAULT_Val   << TCC_EVCTRL_EVACT1_Pos)
#define TCC_EVCTRL_CNTSEL_Pos       6            /**< \brief (TCC_EVCTRL) Timer/counter Output Event Mode */
#define TCC_EVCTRL_CNTSEL_Msk       (_U_(0x3) << TCC_EVCTRL_CNTSEL_Pos)
#define TCC_EVCTRL_CNTSEL(value)    (TCC_EVCTRL_CNTSEL_Msk & ((value) << TCC_EVCTRL_CNTSEL_Pos))
#define   TCC_EVCTRL_CNTSEL_START_Val     _U_(0x0)   /**< \brief (TCC_EVCTRL) An interrupt/event is generated when a new counter cycle starts */
#define   TCC_EVCTRL_CNTSEL_END_Val       _U_(0x1)   /**< \brief (TCC_EVCTRL) An interrupt/event is generated when a counter cycle ends */
#define   TCC_EVCTRL_CNTSEL_BETWEEN_Val   _U_(0x2)   /**< \brief (TCC_EVCTRL) An interrupt/event is generated when a counter cycle ends, except for the first and last cycles */
#define   TCC_EVCTRL_CNTSEL_BOUNDARY_Val  _U_(0x3)   /**< \brief (TCC_EVCTRL) An interrupt/event is generated when a new counter cycle starts or a counter cycle ends */
#define TCC_EVCTRL_CNTSEL_START     (TCC_EVCTRL_CNTSEL_START_Val   << TCC_EVCTRL_CNTSEL_Pos)
#define TCC_EVCTRL_CNTSEL_END       (TCC_EVCTRL_CNTSEL_END_Val     << TCC_EVCTRL_CNTSEL_Pos)
#define TCC_EVCTRL_CNTSEL_BETWEEN   (TCC_EVCTRL_CNTSEL_BETWEEN_Val << TCC_EVCTRL_CNTSEL_Pos)
#define TCC_EVCTRL_CNTSEL_BOUNDARY  (TCC_EVCTRL_CNTSEL_BOUNDARY_Val << TCC_EVCTRL_CNTSEL_Pos)
#define TCC_EVCTRL_OVFEO_Pos        8            /**< \brief (TCC_EVCTRL) Overflow/Underflow Output Event Enable */
#define TCC_EVCTRL_OVFEO            (_U_(0x1) << TCC_EVCTRL_OVFEO_Pos)
#define TCC_EVCTRL_TRGEO_Pos        9            /**< \brief (TCC_EVCTRL) Retrigger Output Event Enable */
#define TCC_EVCTRL_TRGEO            (_U_(0x1) << TCC_EVCTRL_TRGEO_Pos)
#define TCC_EVCTRL_CNTEO_Pos        10           /**< \brief (TCC_EVCTRL) Timer/counter Output Event Enable */
#define TCC_EVCTRL_CNTEO            (_U_(0x1) << TCC_EVCTRL_CNTEO_Pos)
#define TCC_EVCTRL_TCINV0_Pos       12           /**< \brief (TCC_EVCTRL) Inverted Event 0 Input Enable */
#define TCC_EVCTRL_TCINV0           (_U_(1) << TCC_EVCTRL_TCINV0_Pos)
#define TCC_EVCTRL_TCINV1_Pos       13           /**< \brief (TCC_EVCTRL) Inverted Event 1 Input Enable */
#define TCC_EVCTRL_TCINV1           (_U_(1) << TCC_EVCTRL_TCINV1_Pos)
#define TCC_EVCTRL_TCINV_Pos        12           /**< \brief (TCC_EVCTRL) Inverted Event x Input Enable */
#define TCC_EVCTRL_TCINV_Msk        (_U_(0x3) << TCC_EVCTRL_TCINV_Pos)
#define TCC_EVCTRL_TCINV(value)     (TCC_EVCTRL_TCINV_Msk & ((value) << TCC_EVCTRL_TCINV_Pos))
#define TCC_EVCTRL_TCEI0_Pos        14           /**< \brief (TCC_EVCTRL) Timer/counter Event 0 Input Enable */
#define TCC_EVCTRL_TCEI0            (_U_(1) << TCC_EVCTRL_TCEI0_Pos)
#define TCC_EVCTRL_TCEI1_Pos        15           /**< \brief (TCC_EVCTRL) Timer/counter Event 1 Input Enable */
#define TCC_EVCTRL_TCEI1            (_U_(1) << TCC_EVCTRL_TCEI1_Pos)
#define TCC_EVCTRL_TCEI_Pos         14           /**< \brief (TCC_EVCTRL) Timer/counter Event x Input Enable */
#define TCC_EVCTRL_TCEI_Msk         (_U_(0x3) << TCC_EVCTRL_TCEI_Pos)
#define TCC_EVCTRL_TCEI(value)      (TCC_EVCTRL_TCEI_Msk & ((value) << TCC_EVCTRL_TCEI_Pos))
#define TCC_EVCTRL_MCEI0_Pos        16           /**< \brief (TCC_EVCTRL) Match or Capture Channel 0 Event Input Enable */
#define TCC_EVCTRL_MCEI0            (_U_(1) << TCC_EVCTRL_MCEI0_Pos)
#define TCC_EVCTRL_MCEI1_Pos        17           /**< \brief (TCC_EVCTRL) Match or Capture Channel 1 Event Input Enable */
#define TCC_EVCTRL_MCEI1            (_U_(1) << TCC_EVCTRL_MCEI1_Pos)
#define TCC_EVCTRL_MCEI2_Pos        18           /**< \brief (TCC_EVCTRL) Match or Capture Channel 2 Event Input Enable */
#define TCC_EVCTRL_MCEI2            (_U_(1) << TCC_EVCTRL_MCEI2_Pos)
#define TCC_EVCTRL_MCEI3_Pos        19           /**< \brief (TCC_EVCTRL) Match or Capture Channel 3 Event Input Enable */
#define TCC_EVCTRL_MCEI3            (_U_(1) << TCC_EVCTRL_MCEI3_Pos)
#define TCC_EVCTRL_MCEI4_Pos        20           /**< \brief (TCC_EVCTRL) Match or Capture Channel 4 Event Input Enable */
#define TCC_EVCTRL_MCEI4            (_U_(1) << TCC_EVCTRL_MCEI4_Pos)
#define TCC_EVCTRL_MCEI5_Pos        21           /**< \brief (TCC_EVCTRL) Match or Capture Channel 5 Event Input Enable */
#define TCC_EVCTRL_MCEI5            (_U_(1) << TCC_EVCTRL_MCEI5_Pos)
#define TCC_EVCTRL_MCEI_Pos         16           /**< \brief (TCC_EVCTRL) Match or Capture Channel x Event Input Enable */
#define TCC_EVCTRL_MCEI_Msk         (_U_(0x3F) << TCC_EVCTRL_MCEI_Pos)
#define TCC_EVCTRL_MCEI(value)      (TCC_EVCTRL_MCEI_Msk & ((value) << TCC_EVCTRL_MCEI_Pos))
#define TCC_EVCTRL_MCEO0_Pos        24           /**< \brief (TCC_EVCTRL) Match or Capture Channel 0 Event Output Enable */
#define TCC_EVCTRL_MCEO0            (_U_(1) << TCC_EVCTRL_MCEO0_Pos)
#define TCC_EVCTRL_MCEO1_Pos        25           /**< \brief (TCC_EVCTRL) Match or Capture Channel 1 Event Output Enable */
#define TCC_EVCTRL_MCEO1            (_U_(1) << TCC_EVCTRL_MCEO1_Pos)
#define TCC_EVCTRL_MCEO2_Pos        26           /**< \brief (TCC_EVCTRL) Match or Capture Channel 2 Event Output Enable */
#define TCC_EVCTRL_MCEO2            (_U_(1) << TCC_EVCTRL_MCEO2_Pos)
#define TCC_EVCTRL_MCEO3_Pos        27           /**< \brief (TCC_EVCTRL) Match or Capture Channel 3 Event Output Enable */
#define TCC_EVCTRL_MCEO3            (_U_(1) << TCC_EVCTRL_MCEO3_Pos)
#define TCC_EVCTRL_MCEO4_Pos        28           /**< \brief (TCC_EVCTRL) Match or Capture Channel 4 Event Output Enable */
#define TCC_EVCTRL_MCEO4            (_U_(1) << TCC_EVCTRL_MCEO4_Pos)
#define TCC_EVCTRL_MCEO5_Pos        29           /**< \brief (TCC_EVCTRL) Match or Capture Channel 5 Event Output Enable */
#define TCC_EVCTRL_MCEO5            (_U_(1) << TCC_EVCTRL_MCEO5_Pos)
#define TCC_EVCTRL_MCEO_Pos         24           /**< \brief (TCC_EVCTRL) Match or Capture Channel x Event Output Enable */
#define TCC_EVCTRL_MCEO_Msk         (_U_(0x3F) << TCC_EVCTRL_MCEO_Pos)
#define TCC_EVCTRL_MCEO(value)      (TCC_EVCTRL_MCEO_Msk & ((value) << TCC_EVCTRL_MCEO_Pos))
#define TCC_EVCTRL_MASK             _U_(0x3F3FF7FF) /**< \brief (TCC_EVCTRL) MASK Register */

/* -------- TCC_INTENCLR : (TCC Offset: 0x24) (R/W 32) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t OVF:1;            /*!< bit:      0  Overflow Interrupt Enable          */
    uint32_t TRG:1;            /*!< bit:      1  Retrigger Interrupt Enable         */
    uint32_t CNT:1;            /*!< bit:      2  Counter Interrupt Enable           */
    uint32_t ERR:1;            /*!< bit:      3  Error Interrupt Enable             */
    uint32_t :6;               /*!< bit:  4.. 9  Reserved                           */
    uint32_t UFS:1;            /*!< bit:     10  Non-Recoverable Update Fault Interrupt Enable */
    uint32_t DFS:1;            /*!< bit:     11  Non-Recoverable Debug Fault Interrupt Enable */
    uint32_t FAULTA:1;         /*!< bit:     12  Recoverable Fault A Interrupt Enable */
    uint32_t FAULTB:1;         /*!< bit:     13  Recoverable Fault B Interrupt Enable */
    uint32_t FAULT0:1;         /*!< bit:     14  Non-Recoverable Fault 0 Interrupt Enable */
    uint32_t FAULT1:1;         /*!< bit:     15  Non-Recoverable Fault 1 Interrupt Enable */
    uint32_t MC0:1;            /*!< bit:     16  Match or Capture Channel 0 Interrupt Enable */
    uint32_t MC1:1;            /*!< bit:     17  Match or Capture Channel 1 Interrupt Enable */
    uint32_t MC2:1;            /*!< bit:     18  Match or Capture Channel 2 Interrupt Enable */
    uint32_t MC3:1;            /*!< bit:     19  Match or Capture Channel 3 Interrupt Enable */
    uint32_t MC4:1;            /*!< bit:     20  Match or Capture Channel 4 Interrupt Enable */
    uint32_t MC5:1;            /*!< bit:     21  Match or Capture Channel 5 Interrupt Enable */
    uint32_t :10;              /*!< bit: 22..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :16;              /*!< bit:  0..15  Reserved                           */
    uint32_t MC:6;             /*!< bit: 16..21  Match or Capture Channel x Interrupt Enable */
    uint32_t :10;              /*!< bit: 22..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_INTENCLR_OFFSET         0x24         /**< \brief (TCC_INTENCLR offset) Interrupt Enable Clear */
#define TCC_INTENCLR_RESETVALUE     _U_(0x00000000) /**< \brief (TCC_INTENCLR reset_value) Interrupt Enable Clear */

#define TCC_INTENCLR_OVF_Pos        0            /**< \brief (TCC_INTENCLR) Overflow Interrupt Enable */
#define TCC_INTENCLR_OVF            (_U_(0x1) << TCC_INTENCLR_OVF_Pos)
#define TCC_INTENCLR_TRG_Pos        1            /**< \brief (TCC_INTENCLR) Retrigger Interrupt Enable */
#define TCC_INTENCLR_TRG            (_U_(0x1) << TCC_INTENCLR_TRG_Pos)
#define TCC_INTENCLR_CNT_Pos        2            /**< \brief (TCC_INTENCLR) Counter Interrupt Enable */
#define TCC_INTENCLR_CNT            (_U_(0x1) << TCC_INTENCLR_CNT_Pos)
#define TCC_INTENCLR_ERR_Pos        3            /**< \brief (TCC_INTENCLR) Error Interrupt Enable */
#define TCC_INTENCLR_ERR            (_U_(0x1) << TCC_INTENCLR_ERR_Pos)
#define TCC_INTENCLR_UFS_Pos        10           /**< \brief (TCC_INTENCLR) Non-Recoverable Update Fault Interrupt Enable */
#define TCC_INTENCLR_UFS            (_U_(0x1) << TCC_INTENCLR_UFS_Pos)
#define TCC_INTENCLR_DFS_Pos        11           /**< \brief (TCC_INTENCLR) Non-Recoverable Debug Fault Interrupt Enable */
#define TCC_INTENCLR_DFS            (_U_(0x1) << TCC_INTENCLR_DFS_Pos)
#define TCC_INTENCLR_FAULTA_Pos     12           /**< \brief (TCC_INTENCLR) Recoverable Fault A Interrupt Enable */
#define TCC_INTENCLR_FAULTA         (_U_(0x1) << TCC_INTENCLR_FAULTA_Pos)
#define TCC_INTENCLR_FAULTB_Pos     13           /**< \brief (TCC_INTENCLR) Recoverable Fault B Interrupt Enable */
#define TCC_INTENCLR_FAULTB         (_U_(0x1) << TCC_INTENCLR_FAULTB_Pos)
#define TCC_INTENCLR_FAULT0_Pos     14           /**< \brief (TCC_INTENCLR) Non-Recoverable Fault 0 Interrupt Enable */
#define TCC_INTENCLR_FAULT0         (_U_(0x1) << TCC_INTENCLR_FAULT0_Pos)
#define TCC_INTENCLR_FAULT1_Pos     15           /**< \brief (TCC_INTENCLR) Non-Recoverable Fault 1 Interrupt Enable */
#define TCC_INTENCLR_FAULT1         (_U_(0x1) << TCC_INTENCLR_FAULT1_Pos)
#define TCC_INTENCLR_MC0_Pos        16           /**< \brief (TCC_INTENCLR) Match or Capture Channel 0 Interrupt Enable */
#define TCC_INTENCLR_MC0            (_U_(1) << TCC_INTENCLR_MC0_Pos)
#define TCC_INTENCLR_MC1_Pos        17           /**< \brief (TCC_INTENCLR) Match or Capture Channel 1 Interrupt Enable */
#define TCC_INTENCLR_MC1            (_U_(1) << TCC_INTENCLR_MC1_Pos)
#define TCC_INTENCLR_MC2_Pos        18           /**< \brief (TCC_INTENCLR) Match or Capture Channel 2 Interrupt Enable */
#define TCC_INTENCLR_MC2            (_U_(1) << TCC_INTENCLR_MC2_Pos)
#define TCC_INTENCLR_MC3_Pos        19           /**< \brief (TCC_INTENCLR) Match or Capture Channel 3 Interrupt Enable */
#define TCC_INTENCLR_MC3            (_U_(1) << TCC_INTENCLR_MC3_Pos)
#define TCC_INTENCLR_MC4_Pos        20           /**< \brief (TCC_INTENCLR) Match or Capture Channel 4 Interrupt Enable */
#define TCC_INTENCLR_MC4            (_U_(1) << TCC_INTENCLR_MC4_Pos)
#define TCC_INTENCLR_MC5_Pos        21           /**< \brief (TCC_INTENCLR) Match or Capture Channel 5 Interrupt Enable */
#define TCC_INTENCLR_MC5            (_U_(1) << TCC_INTENCLR_MC5_Pos)
#define TCC_INTENCLR_MC_Pos         16           /**< \brief (TCC_INTENCLR) Match or Capture Channel x Interrupt Enable */
#define TCC_INTENCLR_MC_Msk         (_U_(0x3F) << TCC_INTENCLR_MC_Pos)
#define TCC_INTENCLR_MC(value)      (TCC_INTENCLR_MC_Msk & ((value) << TCC_INTENCLR_MC_Pos))
#define TCC_INTENCLR_MASK           _U_(0x003FFC0F) /**< \brief (TCC_INTENCLR) MASK Register */

/* -------- TCC_INTENSET : (TCC Offset: 0x28) (R/W 32) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t OVF:1;            /*!< bit:      0  Overflow Interrupt Enable          */
    uint32_t TRG:1;            /*!< bit:      1  Retrigger Interrupt Enable         */
    uint32_t CNT:1;            /*!< bit:      2  Counter Interrupt Enable           */
    uint32_t ERR:1;            /*!< bit:      3  Error Interrupt Enable             */
    uint32_t :6;               /*!< bit:  4.. 9  Reserved                           */
    uint32_t UFS:1;            /*!< bit:     10  Non-Recoverable Update Fault Interrupt Enable */
    uint32_t DFS:1;            /*!< bit:     11  Non-Recoverable Debug Fault Interrupt Enable */
    uint32_t FAULTA:1;         /*!< bit:     12  Recoverable Fault A Interrupt Enable */
    uint32_t FAULTB:1;         /*!< bit:     13  Recoverable Fault B Interrupt Enable */
    uint32_t FAULT0:1;         /*!< bit:     14  Non-Recoverable Fault 0 Interrupt Enable */
    uint32_t FAULT1:1;         /*!< bit:     15  Non-Recoverable Fault 1 Interrupt Enable */
    uint32_t MC0:1;            /*!< bit:     16  Match or Capture Channel 0 Interrupt Enable */
    uint32_t MC1:1;            /*!< bit:     17  Match or Capture Channel 1 Interrupt Enable */
    uint32_t MC2:1;            /*!< bit:     18  Match or Capture Channel 2 Interrupt Enable */
    uint32_t MC3:1;            /*!< bit:     19  Match or Capture Channel 3 Interrupt Enable */
    uint32_t MC4:1;            /*!< bit:     20  Match or Capture Channel 4 Interrupt Enable */
    uint32_t MC5:1;            /*!< bit:     21  Match or Capture Channel 5 Interrupt Enable */
    uint32_t :10;              /*!< bit: 22..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :16;              /*!< bit:  0..15  Reserved                           */
    uint32_t MC:6;             /*!< bit: 16..21  Match or Capture Channel x Interrupt Enable */
    uint32_t :10;              /*!< bit: 22..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_INTENSET_OFFSET         0x28         /**< \brief (TCC_INTENSET offset) Interrupt Enable Set */
#define TCC_INTENSET_RESETVALUE     _U_(0x00000000) /**< \brief (TCC_INTENSET reset_value) Interrupt Enable Set */

#define TCC_INTENSET_OVF_Pos        0            /**< \brief (TCC_INTENSET) Overflow Interrupt Enable */
#define TCC_INTENSET_OVF            (_U_(0x1) << TCC_INTENSET_OVF_Pos)
#define TCC_INTENSET_TRG_Pos        1            /**< \brief (TCC_INTENSET) Retrigger Interrupt Enable */
#define TCC_INTENSET_TRG            (_U_(0x1) << TCC_INTENSET_TRG_Pos)
#define TCC_INTENSET_CNT_Pos        2            /**< \brief (TCC_INTENSET) Counter Interrupt Enable */
#define TCC_INTENSET_CNT            (_U_(0x1) << TCC_INTENSET_CNT_Pos)
#define TCC_INTENSET_ERR_Pos        3            /**< \brief (TCC_INTENSET) Error Interrupt Enable */
#define TCC_INTENSET_ERR            (_U_(0x1) << TCC_INTENSET_ERR_Pos)
#define TCC_INTENSET_UFS_Pos        10           /**< \brief (TCC_INTENSET) Non-Recoverable Update Fault Interrupt Enable */
#define TCC_INTENSET_UFS            (_U_(0x1) << TCC_INTENSET_UFS_Pos)
#define TCC_INTENSET_DFS_Pos        11           /**< \brief (TCC_INTENSET) Non-Recoverable Debug Fault Interrupt Enable */
#define TCC_INTENSET_DFS            (_U_(0x1) << TCC_INTENSET_DFS_Pos)
#define TCC_INTENSET_FAULTA_Pos     12           /**< \brief (TCC_INTENSET) Recoverable Fault A Interrupt Enable */
#define TCC_INTENSET_FAULTA         (_U_(0x1) << TCC_INTENSET_FAULTA_Pos)
#define TCC_INTENSET_FAULTB_Pos     13           /**< \brief (TCC_INTENSET) Recoverable Fault B Interrupt Enable */
#define TCC_INTENSET_FAULTB         (_U_(0x1) << TCC_INTENSET_FAULTB_Pos)
#define TCC_INTENSET_FAULT0_Pos     14           /**< \brief (TCC_INTENSET) Non-Recoverable Fault 0 Interrupt Enable */
#define TCC_INTENSET_FAULT0         (_U_(0x1) << TCC_INTENSET_FAULT0_Pos)
#define TCC_INTENSET_FAULT1_Pos     15           /**< \brief (TCC_INTENSET) Non-Recoverable Fault 1 Interrupt Enable */
#define TCC_INTENSET_FAULT1         (_U_(0x1) << TCC_INTENSET_FAULT1_Pos)
#define TCC_INTENSET_MC0_Pos        16           /**< \brief (TCC_INTENSET) Match or Capture Channel 0 Interrupt Enable */
#define TCC_INTENSET_MC0            (_U_(1) << TCC_INTENSET_MC0_Pos)
#define TCC_INTENSET_MC1_Pos        17           /**< \brief (TCC_INTENSET) Match or Capture Channel 1 Interrupt Enable */
#define TCC_INTENSET_MC1            (_U_(1) << TCC_INTENSET_MC1_Pos)
#define TCC_INTENSET_MC2_Pos        18           /**< \brief (TCC_INTENSET) Match or Capture Channel 2 Interrupt Enable */
#define TCC_INTENSET_MC2            (_U_(1) << TCC_INTENSET_MC2_Pos)
#define TCC_INTENSET_MC3_Pos        19           /**< \brief (TCC_INTENSET) Match or Capture Channel 3 Interrupt Enable */
#define TCC_INTENSET_MC3            (_U_(1) << TCC_INTENSET_MC3_Pos)
#define TCC_INTENSET_MC4_Pos        20           /**< \brief (TCC_INTENSET) Match or Capture Channel 4 Interrupt Enable */
#define TCC_INTENSET_MC4            (_U_(1) << TCC_INTENSET_MC4_Pos)
#define TCC_INTENSET_MC5_Pos        21           /**< \brief (TCC_INTENSET) Match or Capture Channel 5 Interrupt Enable */
#define TCC_INTENSET_MC5            (_U_(1) << TCC_INTENSET_MC5_Pos)
#define TCC_INTENSET_MC_Pos         16           /**< \brief (TCC_INTENSET) Match or Capture Channel x Interrupt Enable */
#define TCC_INTENSET_MC_Msk         (_U_(0x3F) << TCC_INTENSET_MC_Pos)
#define TCC_INTENSET_MC(value)      (TCC_INTENSET_MC_Msk & ((value) << TCC_INTENSET_MC_Pos))
#define TCC_INTENSET_MASK           _U_(0x003FFC0F) /**< \brief (TCC_INTENSET) MASK Register */

/* -------- TCC_INTFLAG : (TCC Offset: 0x2C) (R/W 32) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t OVF:1;            /*!< bit:      0  Overflow                           */
    __I uint32_t TRG:1;            /*!< bit:      1  Retrigger                          */
    __I uint32_t CNT:1;            /*!< bit:      2  Counter                            */
    __I uint32_t ERR:1;            /*!< bit:      3  Error                              */
    __I uint32_t :6;               /*!< bit:  4.. 9  Reserved                           */
    __I uint32_t UFS:1;            /*!< bit:     10  Non-Recoverable Update Fault       */
    __I uint32_t DFS:1;            /*!< bit:     11  Non-Recoverable Debug Fault        */
    __I uint32_t FAULTA:1;         /*!< bit:     12  Recoverable Fault A                */
    __I uint32_t FAULTB:1;         /*!< bit:     13  Recoverable Fault B                */
    __I uint32_t FAULT0:1;         /*!< bit:     14  Non-Recoverable Fault 0            */
    __I uint32_t FAULT1:1;         /*!< bit:     15  Non-Recoverable Fault 1            */
    __I uint32_t MC0:1;            /*!< bit:     16  Match or Capture 0                 */
    __I uint32_t MC1:1;            /*!< bit:     17  Match or Capture 1                 */
    __I uint32_t MC2:1;            /*!< bit:     18  Match or Capture 2                 */
    __I uint32_t MC3:1;            /*!< bit:     19  Match or Capture 3                 */
    __I uint32_t MC4:1;            /*!< bit:     20  Match or Capture 4                 */
    __I uint32_t MC5:1;            /*!< bit:     21  Match or Capture 5                 */
    __I uint32_t :10;              /*!< bit: 22..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint32_t :16;              /*!< bit:  0..15  Reserved                           */
    __I uint32_t MC:6;             /*!< bit: 16..21  Match or Capture x                 */
    __I uint32_t :10;              /*!< bit: 22..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_INTFLAG_OFFSET          0x2C         /**< \brief (TCC_INTFLAG offset) Interrupt Flag Status and Clear */
#define TCC_INTFLAG_RESETVALUE      _U_(0x00000000) /**< \brief (TCC_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define TCC_INTFLAG_OVF_Pos         0            /**< \brief (TCC_INTFLAG) Overflow */
#define TCC_INTFLAG_OVF             (_U_(0x1) << TCC_INTFLAG_OVF_Pos)
#define TCC_INTFLAG_TRG_Pos         1            /**< \brief (TCC_INTFLAG) Retrigger */
#define TCC_INTFLAG_TRG             (_U_(0x1) << TCC_INTFLAG_TRG_Pos)
#define TCC_INTFLAG_CNT_Pos         2            /**< \brief (TCC_INTFLAG) Counter */
#define TCC_INTFLAG_CNT             (_U_(0x1) << TCC_INTFLAG_CNT_Pos)
#define TCC_INTFLAG_ERR_Pos         3            /**< \brief (TCC_INTFLAG) Error */
#define TCC_INTFLAG_ERR             (_U_(0x1) << TCC_INTFLAG_ERR_Pos)
#define TCC_INTFLAG_UFS_Pos         10           /**< \brief (TCC_INTFLAG) Non-Recoverable Update Fault */
#define TCC_INTFLAG_UFS             (_U_(0x1) << TCC_INTFLAG_UFS_Pos)
#define TCC_INTFLAG_DFS_Pos         11           /**< \brief (TCC_INTFLAG) Non-Recoverable Debug Fault */
#define TCC_INTFLAG_DFS             (_U_(0x1) << TCC_INTFLAG_DFS_Pos)
#define TCC_INTFLAG_FAULTA_Pos      12           /**< \brief (TCC_INTFLAG) Recoverable Fault A */
#define TCC_INTFLAG_FAULTA          (_U_(0x1) << TCC_INTFLAG_FAULTA_Pos)
#define TCC_INTFLAG_FAULTB_Pos      13           /**< \brief (TCC_INTFLAG) Recoverable Fault B */
#define TCC_INTFLAG_FAULTB          (_U_(0x1) << TCC_INTFLAG_FAULTB_Pos)
#define TCC_INTFLAG_FAULT0_Pos      14           /**< \brief (TCC_INTFLAG) Non-Recoverable Fault 0 */
#define TCC_INTFLAG_FAULT0          (_U_(0x1) << TCC_INTFLAG_FAULT0_Pos)
#define TCC_INTFLAG_FAULT1_Pos      15           /**< \brief (TCC_INTFLAG) Non-Recoverable Fault 1 */
#define TCC_INTFLAG_FAULT1          (_U_(0x1) << TCC_INTFLAG_FAULT1_Pos)
#define TCC_INTFLAG_MC0_Pos         16           /**< \brief (TCC_INTFLAG) Match or Capture 0 */
#define TCC_INTFLAG_MC0             (_U_(1) << TCC_INTFLAG_MC0_Pos)
#define TCC_INTFLAG_MC1_Pos         17           /**< \brief (TCC_INTFLAG) Match or Capture 1 */
#define TCC_INTFLAG_MC1             (_U_(1) << TCC_INTFLAG_MC1_Pos)
#define TCC_INTFLAG_MC2_Pos         18           /**< \brief (TCC_INTFLAG) Match or Capture 2 */
#define TCC_INTFLAG_MC2             (_U_(1) << TCC_INTFLAG_MC2_Pos)
#define TCC_INTFLAG_MC3_Pos         19           /**< \brief (TCC_INTFLAG) Match or Capture 3 */
#define TCC_INTFLAG_MC3             (_U_(1) << TCC_INTFLAG_MC3_Pos)
#define TCC_INTFLAG_MC4_Pos         20           /**< \brief (TCC_INTFLAG) Match or Capture 4 */
#define TCC_INTFLAG_MC4             (_U_(1) << TCC_INTFLAG_MC4_Pos)
#define TCC_INTFLAG_MC5_Pos         21           /**< \brief (TCC_INTFLAG) Match or Capture 5 */
#define TCC_INTFLAG_MC5             (_U_(1) << TCC_INTFLAG_MC5_Pos)
#define TCC_INTFLAG_MC_Pos          16           /**< \brief (TCC_INTFLAG) Match or Capture x */
#define TCC_INTFLAG_MC_Msk          (_U_(0x3F) << TCC_INTFLAG_MC_Pos)
#define TCC_INTFLAG_MC(value)       (TCC_INTFLAG_MC_Msk & ((value) << TCC_INTFLAG_MC_Pos))
#define TCC_INTFLAG_MASK            _U_(0x003FFC0F) /**< \brief (TCC_INTFLAG) MASK Register */

/* -------- TCC_STATUS : (TCC Offset: 0x30) (R/W 32) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t STOP:1;           /*!< bit:      0  Stop                               */
    uint32_t IDX:1;            /*!< bit:      1  Ramp                               */
    uint32_t UFS:1;            /*!< bit:      2  Non-recoverable Update Fault State */
    uint32_t DFS:1;            /*!< bit:      3  Non-Recoverable Debug Fault State  */
    uint32_t SLAVE:1;          /*!< bit:      4  Slave                              */
    uint32_t PATTBUFV:1;       /*!< bit:      5  Pattern Buffer Valid               */
    uint32_t :1;               /*!< bit:      6  Reserved                           */
    uint32_t PERBUFV:1;        /*!< bit:      7  Period Buffer Valid                */
    uint32_t FAULTAIN:1;       /*!< bit:      8  Recoverable Fault A Input          */
    uint32_t FAULTBIN:1;       /*!< bit:      9  Recoverable Fault B Input          */
    uint32_t FAULT0IN:1;       /*!< bit:     10  Non-Recoverable Fault0 Input       */
    uint32_t FAULT1IN:1;       /*!< bit:     11  Non-Recoverable Fault1 Input       */
    uint32_t FAULTA:1;         /*!< bit:     12  Recoverable Fault A State          */
    uint32_t FAULTB:1;         /*!< bit:     13  Recoverable Fault B State          */
    uint32_t FAULT0:1;         /*!< bit:     14  Non-Recoverable Fault 0 State      */
    uint32_t FAULT1:1;         /*!< bit:     15  Non-Recoverable Fault 1 State      */
    uint32_t CCBUFV0:1;        /*!< bit:     16  Compare Channel 0 Buffer Valid     */
    uint32_t CCBUFV1:1;        /*!< bit:     17  Compare Channel 1 Buffer Valid     */
    uint32_t CCBUFV2:1;        /*!< bit:     18  Compare Channel 2 Buffer Valid     */
    uint32_t CCBUFV3:1;        /*!< bit:     19  Compare Channel 3 Buffer Valid     */
    uint32_t CCBUFV4:1;        /*!< bit:     20  Compare Channel 4 Buffer Valid     */
    uint32_t CCBUFV5:1;        /*!< bit:     21  Compare Channel 5 Buffer Valid     */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t CMP0:1;           /*!< bit:     24  Compare Channel 0 Value            */
    uint32_t CMP1:1;           /*!< bit:     25  Compare Channel 1 Value            */
    uint32_t CMP2:1;           /*!< bit:     26  Compare Channel 2 Value            */
    uint32_t CMP3:1;           /*!< bit:     27  Compare Channel 3 Value            */
    uint32_t CMP4:1;           /*!< bit:     28  Compare Channel 4 Value            */
    uint32_t CMP5:1;           /*!< bit:     29  Compare Channel 5 Value            */
    uint32_t :2;               /*!< bit: 30..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :16;              /*!< bit:  0..15  Reserved                           */
    uint32_t CCBUFV:6;         /*!< bit: 16..21  Compare Channel x Buffer Valid     */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t CMP:6;            /*!< bit: 24..29  Compare Channel x Value            */
    uint32_t :2;               /*!< bit: 30..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_STATUS_OFFSET           0x30         /**< \brief (TCC_STATUS offset) Status */
#define TCC_STATUS_RESETVALUE       _U_(0x00000001) /**< \brief (TCC_STATUS reset_value) Status */

#define TCC_STATUS_STOP_Pos         0            /**< \brief (TCC_STATUS) Stop */
#define TCC_STATUS_STOP             (_U_(0x1) << TCC_STATUS_STOP_Pos)
#define TCC_STATUS_IDX_Pos          1            /**< \brief (TCC_STATUS) Ramp */
#define TCC_STATUS_IDX              (_U_(0x1) << TCC_STATUS_IDX_Pos)
#define TCC_STATUS_UFS_Pos          2            /**< \brief (TCC_STATUS) Non-recoverable Update Fault State */
#define TCC_STATUS_UFS              (_U_(0x1) << TCC_STATUS_UFS_Pos)
#define TCC_STATUS_DFS_Pos          3            /**< \brief (TCC_STATUS) Non-Recoverable Debug Fault State */
#define TCC_STATUS_DFS              (_U_(0x1) << TCC_STATUS_DFS_Pos)
#define TCC_STATUS_SLAVE_Pos        4            /**< \brief (TCC_STATUS) Slave */
#define TCC_STATUS_SLAVE            (_U_(0x1) << TCC_STATUS_SLAVE_Pos)
#define TCC_STATUS_PATTBUFV_Pos     5            /**< \brief (TCC_STATUS) Pattern Buffer Valid */
#define TCC_STATUS_PATTBUFV         (_U_(0x1) << TCC_STATUS_PATTBUFV_Pos)
#define TCC_STATUS_PERBUFV_Pos      7            /**< \brief (TCC_STATUS) Period Buffer Valid */
#define TCC_STATUS_PERBUFV          (_U_(0x1) << TCC_STATUS_PERBUFV_Pos)
#define TCC_STATUS_FAULTAIN_Pos     8            /**< \brief (TCC_STATUS) Recoverable Fault A Input */
#define TCC_STATUS_FAULTAIN         (_U_(0x1) << TCC_STATUS_FAULTAIN_Pos)
#define TCC_STATUS_FAULTBIN_Pos     9            /**< \brief (TCC_STATUS) Recoverable Fault B Input */
#define TCC_STATUS_FAULTBIN         (_U_(0x1) << TCC_STATUS_FAULTBIN_Pos)
#define TCC_STATUS_FAULT0IN_Pos     10           /**< \brief (TCC_STATUS) Non-Recoverable Fault0 Input */
#define TCC_STATUS_FAULT0IN         (_U_(0x1) << TCC_STATUS_FAULT0IN_Pos)
#define TCC_STATUS_FAULT1IN_Pos     11           /**< \brief (TCC_STATUS) Non-Recoverable Fault1 Input */
#define TCC_STATUS_FAULT1IN         (_U_(0x1) << TCC_STATUS_FAULT1IN_Pos)
#define TCC_STATUS_FAULTA_Pos       12           /**< \brief (TCC_STATUS) Recoverable Fault A State */
#define TCC_STATUS_FAULTA           (_U_(0x1) << TCC_STATUS_FAULTA_Pos)
#define TCC_STATUS_FAULTB_Pos       13           /**< \brief (TCC_STATUS) Recoverable Fault B State */
#define TCC_STATUS_FAULTB           (_U_(0x1) << TCC_STATUS_FAULTB_Pos)
#define TCC_STATUS_FAULT0_Pos       14           /**< \brief (TCC_STATUS) Non-Recoverable Fault 0 State */
#define TCC_STATUS_FAULT0           (_U_(0x1) << TCC_STATUS_FAULT0_Pos)
#define TCC_STATUS_FAULT1_Pos       15           /**< \brief (TCC_STATUS) Non-Recoverable Fault 1 State */
#define TCC_STATUS_FAULT1           (_U_(0x1) << TCC_STATUS_FAULT1_Pos)
#define TCC_STATUS_CCBUFV0_Pos      16           /**< \brief (TCC_STATUS) Compare Channel 0 Buffer Valid */
#define TCC_STATUS_CCBUFV0          (_U_(1) << TCC_STATUS_CCBUFV0_Pos)
#define TCC_STATUS_CCBUFV1_Pos      17           /**< \brief (TCC_STATUS) Compare Channel 1 Buffer Valid */
#define TCC_STATUS_CCBUFV1          (_U_(1) << TCC_STATUS_CCBUFV1_Pos)
#define TCC_STATUS_CCBUFV2_Pos      18           /**< \brief (TCC_STATUS) Compare Channel 2 Buffer Valid */
#define TCC_STATUS_CCBUFV2          (_U_(1) << TCC_STATUS_CCBUFV2_Pos)
#define TCC_STATUS_CCBUFV3_Pos      19           /**< \brief (TCC_STATUS) Compare Channel 3 Buffer Valid */
#define TCC_STATUS_CCBUFV3          (_U_(1) << TCC_STATUS_CCBUFV3_Pos)
#define TCC_STATUS_CCBUFV4_Pos      20           /**< \brief (TCC_STATUS) Compare Channel 4 Buffer Valid */
#define TCC_STATUS_CCBUFV4          (_U_(1) << TCC_STATUS_CCBUFV4_Pos)
#define TCC_STATUS_CCBUFV5_Pos      21           /**< \brief (TCC_STATUS) Compare Channel 5 Buffer Valid */
#define TCC_STATUS_CCBUFV5          (_U_(1) << TCC_STATUS_CCBUFV5_Pos)
#define TCC_STATUS_CCBUFV_Pos       16           /**< \brief (TCC_STATUS) Compare Channel x Buffer Valid */
#define TCC_STATUS_CCBUFV_Msk       (_U_(0x3F) << TCC_STATUS_CCBUFV_Pos)
#define TCC_STATUS_CCBUFV(value)    (TCC_STATUS_CCBUFV_Msk & ((value) << TCC_STATUS_CCBUFV_Pos))
#define TCC_STATUS_CMP0_Pos         24           /**< \brief (TCC_STATUS) Compare Channel 0 Value */
#define TCC_STATUS_CMP0             (_U_(1) << TCC_STATUS_CMP0_Pos)
#define TCC_STATUS_CMP1_Pos         25           /**< \brief (TCC_STATUS) Compare Channel 1 Value */
#define TCC_STATUS_CMP1             (_U_(1) << TCC_STATUS_CMP1_Pos)
#define TCC_STATUS_CMP2_Pos         26           /**< \brief (TCC_STATUS) Compare Channel 2 Value */
#define TCC_STATUS_CMP2             (_U_(1) << TCC_STATUS_CMP2_Pos)
#define TCC_STATUS_CMP3_Pos         27           /**< \brief (TCC_STATUS) Compare Channel 3 Value */
#define TCC_STATUS_CMP3             (_U_(1) << TCC_STATUS_CMP3_Pos)
#define TCC_STATUS_CMP4_Pos         28           /**< \brief (TCC_STATUS) Compare Channel 4 Value */
#define TCC_STATUS_CMP4             (_U_(1) << TCC_STATUS_CMP4_Pos)
#define TCC_STATUS_CMP5_Pos         29           /**< \brief (TCC_STATUS) Compare Channel 5 Value */
#define TCC_STATUS_CMP5             (_U_(1) << TCC_STATUS_CMP5_Pos)
#define TCC_STATUS_CMP_Pos          24           /**< \brief (TCC_STATUS) Compare Channel x Value */
#define TCC_STATUS_CMP_Msk          (_U_(0x3F) << TCC_STATUS_CMP_Pos)
#define TCC_STATUS_CMP(value)       (TCC_STATUS_CMP_Msk & ((value) << TCC_STATUS_CMP_Pos))
#define TCC_STATUS_MASK             _U_(0x3F3FFFBF) /**< \brief (TCC_STATUS) MASK Register */

/* -------- TCC_COUNT : (TCC Offset: 0x34) (R/W 32) Count -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct { // DITH4 mode
    uint32_t :4;               /*!< bit:  0.. 3  Reserved                           */
    uint32_t COUNT:20;         /*!< bit:  4..23  Counter Value                      */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } DITH4;                     /*!< Structure used for DITH4                        */
  struct { // DITH5 mode
    uint32_t :5;               /*!< bit:  0.. 4  Reserved                           */
    uint32_t COUNT:19;         /*!< bit:  5..23  Counter Value                      */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } DITH5;                     /*!< Structure used for DITH5                        */
  struct { // DITH6 mode
    uint32_t :6;               /*!< bit:  0.. 5  Reserved                           */
    uint32_t COUNT:18;         /*!< bit:  6..23  Counter Value                      */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } DITH6;                     /*!< Structure used for DITH6                        */
  struct {
    uint32_t COUNT:24;         /*!< bit:  0..23  Counter Value                      */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_COUNT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_COUNT_OFFSET            0x34         /**< \brief (TCC_COUNT offset) Count */
#define TCC_COUNT_RESETVALUE        _U_(0x00000000) /**< \brief (TCC_COUNT reset_value) Count */

// DITH4 mode
#define TCC_COUNT_DITH4_COUNT_Pos   4            /**< \brief (TCC_COUNT_DITH4) Counter Value */
#define TCC_COUNT_DITH4_COUNT_Msk   (_U_(0xFFFFF) << TCC_COUNT_DITH4_COUNT_Pos)
#define TCC_COUNT_DITH4_COUNT(value) (TCC_COUNT_DITH4_COUNT_Msk & ((value) << TCC_COUNT_DITH4_COUNT_Pos))
#define TCC_COUNT_DITH4_MASK        _U_(0x00FFFFF0) /**< \brief (TCC_COUNT_DITH4) MASK Register */

// DITH5 mode
#define TCC_COUNT_DITH5_COUNT_Pos   5            /**< \brief (TCC_COUNT_DITH5) Counter Value */
#define TCC_COUNT_DITH5_COUNT_Msk   (_U_(0x7FFFF) << TCC_COUNT_DITH5_COUNT_Pos)
#define TCC_COUNT_DITH5_COUNT(value) (TCC_COUNT_DITH5_COUNT_Msk & ((value) << TCC_COUNT_DITH5_COUNT_Pos))
#define TCC_COUNT_DITH5_MASK        _U_(0x00FFFFE0) /**< \brief (TCC_COUNT_DITH5) MASK Register */

// DITH6 mode
#define TCC_COUNT_DITH6_COUNT_Pos   6            /**< \brief (TCC_COUNT_DITH6) Counter Value */
#define TCC_COUNT_DITH6_COUNT_Msk   (_U_(0x3FFFF) << TCC_COUNT_DITH6_COUNT_Pos)
#define TCC_COUNT_DITH6_COUNT(value) (TCC_COUNT_DITH6_COUNT_Msk & ((value) << TCC_COUNT_DITH6_COUNT_Pos))
#define TCC_COUNT_DITH6_MASK        _U_(0x00FFFFC0) /**< \brief (TCC_COUNT_DITH6) MASK Register */

#define TCC_COUNT_COUNT_Pos         0            /**< \brief (TCC_COUNT) Counter Value */
#define TCC_COUNT_COUNT_Msk         (_U_(0xFFFFFF) << TCC_COUNT_COUNT_Pos)
#define TCC_COUNT_COUNT(value)      (TCC_COUNT_COUNT_Msk & ((value) << TCC_COUNT_COUNT_Pos))
#define TCC_COUNT_MASK              _U_(0x00FFFFFF) /**< \brief (TCC_COUNT) MASK Register */

/* -------- TCC_PATT : (TCC Offset: 0x38) (R/W 16) Pattern -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PGE0:1;           /*!< bit:      0  Pattern Generator 0 Output Enable  */
    uint16_t PGE1:1;           /*!< bit:      1  Pattern Generator 1 Output Enable  */
    uint16_t PGE2:1;           /*!< bit:      2  Pattern Generator 2 Output Enable  */
    uint16_t PGE3:1;           /*!< bit:      3  Pattern Generator 3 Output Enable  */
    uint16_t PGE4:1;           /*!< bit:      4  Pattern Generator 4 Output Enable  */
    uint16_t PGE5:1;           /*!< bit:      5  Pattern Generator 5 Output Enable  */
    uint16_t PGE6:1;           /*!< bit:      6  Pattern Generator 6 Output Enable  */
    uint16_t PGE7:1;           /*!< bit:      7  Pattern Generator 7 Output Enable  */
    uint16_t PGV0:1;           /*!< bit:      8  Pattern Generator 0 Output Value   */
    uint16_t PGV1:1;           /*!< bit:      9  Pattern Generator 1 Output Value   */
    uint16_t PGV2:1;           /*!< bit:     10  Pattern Generator 2 Output Value   */
    uint16_t PGV3:1;           /*!< bit:     11  Pattern Generator 3 Output Value   */
    uint16_t PGV4:1;           /*!< bit:     12  Pattern Generator 4 Output Value   */
    uint16_t PGV5:1;           /*!< bit:     13  Pattern Generator 5 Output Value   */
    uint16_t PGV6:1;           /*!< bit:     14  Pattern Generator 6 Output Value   */
    uint16_t PGV7:1;           /*!< bit:     15  Pattern Generator 7 Output Value   */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PGE:8;            /*!< bit:  0.. 7  Pattern Generator x Output Enable  */
    uint16_t PGV:8;            /*!< bit:  8..15  Pattern Generator x Output Value   */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} TCC_PATT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_PATT_OFFSET             0x38         /**< \brief (TCC_PATT offset) Pattern */
#define TCC_PATT_RESETVALUE         _U_(0x0000)  /**< \brief (TCC_PATT reset_value) Pattern */

#define TCC_PATT_PGE0_Pos           0            /**< \brief (TCC_PATT) Pattern Generator 0 Output Enable */
#define TCC_PATT_PGE0               (_U_(1) << TCC_PATT_PGE0_Pos)
#define TCC_PATT_PGE1_Pos           1            /**< \brief (TCC_PATT) Pattern Generator 1 Output Enable */
#define TCC_PATT_PGE1               (_U_(1) << TCC_PATT_PGE1_Pos)
#define TCC_PATT_PGE2_Pos           2            /**< \brief (TCC_PATT) Pattern Generator 2 Output Enable */
#define TCC_PATT_PGE2               (_U_(1) << TCC_PATT_PGE2_Pos)
#define TCC_PATT_PGE3_Pos           3            /**< \brief (TCC_PATT) Pattern Generator 3 Output Enable */
#define TCC_PATT_PGE3               (_U_(1) << TCC_PATT_PGE3_Pos)
#define TCC_PATT_PGE4_Pos           4            /**< \brief (TCC_PATT) Pattern Generator 4 Output Enable */
#define TCC_PATT_PGE4               (_U_(1) << TCC_PATT_PGE4_Pos)
#define TCC_PATT_PGE5_Pos           5            /**< \brief (TCC_PATT) Pattern Generator 5 Output Enable */
#define TCC_PATT_PGE5               (_U_(1) << TCC_PATT_PGE5_Pos)
#define TCC_PATT_PGE6_Pos           6            /**< \brief (TCC_PATT) Pattern Generator 6 Output Enable */
#define TCC_PATT_PGE6               (_U_(1) << TCC_PATT_PGE6_Pos)
#define TCC_PATT_PGE7_Pos           7            /**< \brief (TCC_PATT) Pattern Generator 7 Output Enable */
#define TCC_PATT_PGE7               (_U_(1) << TCC_PATT_PGE7_Pos)
#define TCC_PATT_PGE_Pos            0            /**< \brief (TCC_PATT) Pattern Generator x Output Enable */
#define TCC_PATT_PGE_Msk            (_U_(0xFF) << TCC_PATT_PGE_Pos)
#define TCC_PATT_PGE(value)         (TCC_PATT_PGE_Msk & ((value) << TCC_PATT_PGE_Pos))
#define TCC_PATT_PGV0_Pos           8            /**< \brief (TCC_PATT) Pattern Generator 0 Output Value */
#define TCC_PATT_PGV0               (_U_(1) << TCC_PATT_PGV0_Pos)
#define TCC_PATT_PGV1_Pos           9            /**< \brief (TCC_PATT) Pattern Generator 1 Output Value */
#define TCC_PATT_PGV1               (_U_(1) << TCC_PATT_PGV1_Pos)
#define TCC_PATT_PGV2_Pos           10           /**< \brief (TCC_PATT) Pattern Generator 2 Output Value */
#define TCC_PATT_PGV2               (_U_(1) << TCC_PATT_PGV2_Pos)
#define TCC_PATT_PGV3_Pos           11           /**< \brief (TCC_PATT) Pattern Generator 3 Output Value */
#define TCC_PATT_PGV3               (_U_(1) << TCC_PATT_PGV3_Pos)
#define TCC_PATT_PGV4_Pos           12           /**< \brief (TCC_PATT) Pattern Generator 4 Output Value */
#define TCC_PATT_PGV4               (_U_(1) << TCC_PATT_PGV4_Pos)
#define TCC_PATT_PGV5_Pos           13           /**< \brief (TCC_PATT) Pattern Generator 5 Output Value */
#define TCC_PATT_PGV5               (_U_(1) << TCC_PATT_PGV5_Pos)
#define TCC_PATT_PGV6_Pos           14           /**< \brief (TCC_PATT) Pattern Generator 6 Output Value */
#define TCC_PATT_PGV6               (_U_(1) << TCC_PATT_PGV6_Pos)
#define TCC_PATT_PGV7_Pos           15           /**< \brief (TCC_PATT) Pattern Generator 7 Output Value */
#define TCC_PATT_PGV7               (_U_(1) << TCC_PATT_PGV7_Pos)
#define TCC_PATT_PGV_Pos            8            /**< \brief (TCC_PATT) Pattern Generator x Output Value */
#define TCC_PATT_PGV_Msk            (_U_(0xFF) << TCC_PATT_PGV_Pos)
#define TCC_PATT_PGV(value)         (TCC_PATT_PGV_Msk & ((value) << TCC_PATT_PGV_Pos))
#define TCC_PATT_MASK               _U_(0xFFFF)  /**< \brief (TCC_PATT) MASK Register */

/* -------- TCC_WAVE : (TCC Offset: 0x3C) (R/W 32) Waveform Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t WAVEGEN:3;        /*!< bit:  0.. 2  Waveform Generation                */
    uint32_t :1;               /*!< bit:      3  Reserved                           */
    uint32_t RAMP:2;           /*!< bit:  4.. 5  Ramp Mode                          */
    uint32_t :1;               /*!< bit:      6  Reserved                           */
    uint32_t CIPEREN:1;        /*!< bit:      7  Circular period Enable             */
    uint32_t CICCEN0:1;        /*!< bit:      8  Circular Channel 0 Enable          */
    uint32_t CICCEN1:1;        /*!< bit:      9  Circular Channel 1 Enable          */
    uint32_t CICCEN2:1;        /*!< bit:     10  Circular Channel 2 Enable          */
    uint32_t CICCEN3:1;        /*!< bit:     11  Circular Channel 3 Enable          */
    uint32_t :4;               /*!< bit: 12..15  Reserved                           */
    uint32_t POL0:1;           /*!< bit:     16  Channel 0 Polarity                 */
    uint32_t POL1:1;           /*!< bit:     17  Channel 1 Polarity                 */
    uint32_t POL2:1;           /*!< bit:     18  Channel 2 Polarity                 */
    uint32_t POL3:1;           /*!< bit:     19  Channel 3 Polarity                 */
    uint32_t POL4:1;           /*!< bit:     20  Channel 4 Polarity                 */
    uint32_t POL5:1;           /*!< bit:     21  Channel 5 Polarity                 */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t SWAP0:1;          /*!< bit:     24  Swap DTI Output Pair 0             */
    uint32_t SWAP1:1;          /*!< bit:     25  Swap DTI Output Pair 1             */
    uint32_t SWAP2:1;          /*!< bit:     26  Swap DTI Output Pair 2             */
    uint32_t SWAP3:1;          /*!< bit:     27  Swap DTI Output Pair 3             */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :8;               /*!< bit:  0.. 7  Reserved                           */
    uint32_t CICCEN:4;         /*!< bit:  8..11  Circular Channel x Enable          */
    uint32_t :4;               /*!< bit: 12..15  Reserved                           */
    uint32_t POL:6;            /*!< bit: 16..21  Channel x Polarity                 */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t SWAP:4;           /*!< bit: 24..27  Swap DTI Output Pair x             */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_WAVE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_WAVE_OFFSET             0x3C         /**< \brief (TCC_WAVE offset) Waveform Control */
#define TCC_WAVE_RESETVALUE         _U_(0x00000000) /**< \brief (TCC_WAVE reset_value) Waveform Control */

#define TCC_WAVE_WAVEGEN_Pos        0            /**< \brief (TCC_WAVE) Waveform Generation */
#define TCC_WAVE_WAVEGEN_Msk        (_U_(0x7) << TCC_WAVE_WAVEGEN_Pos)
#define TCC_WAVE_WAVEGEN(value)     (TCC_WAVE_WAVEGEN_Msk & ((value) << TCC_WAVE_WAVEGEN_Pos))
#define   TCC_WAVE_WAVEGEN_NFRQ_Val       _U_(0x0)   /**< \brief (TCC_WAVE) Normal frequency */
#define   TCC_WAVE_WAVEGEN_MFRQ_Val       _U_(0x1)   /**< \brief (TCC_WAVE) Match frequency */
#define   TCC_WAVE_WAVEGEN_NPWM_Val       _U_(0x2)   /**< \brief (TCC_WAVE) Normal PWM */
#define   TCC_WAVE_WAVEGEN_DSCRITICAL_Val _U_(0x4)   /**< \brief (TCC_WAVE) Dual-slope critical */
#define   TCC_WAVE_WAVEGEN_DSBOTTOM_Val   _U_(0x5)   /**< \brief (TCC_WAVE) Dual-slope with interrupt/event condition when COUNT reaches ZERO */
#define   TCC_WAVE_WAVEGEN_DSBOTH_Val     _U_(0x6)   /**< \brief (TCC_WAVE) Dual-slope with interrupt/event condition when COUNT reaches ZERO or TOP */
#define   TCC_WAVE_WAVEGEN_DSTOP_Val      _U_(0x7)   /**< \brief (TCC_WAVE) Dual-slope with interrupt/event condition when COUNT reaches TOP */
#define TCC_WAVE_WAVEGEN_NFRQ       (TCC_WAVE_WAVEGEN_NFRQ_Val     << TCC_WAVE_WAVEGEN_Pos)
#define TCC_WAVE_WAVEGEN_MFRQ       (TCC_WAVE_WAVEGEN_MFRQ_Val     << TCC_WAVE_WAVEGEN_Pos)
#define TCC_WAVE_WAVEGEN_NPWM       (TCC_WAVE_WAVEGEN_NPWM_Val     << TCC_WAVE_WAVEGEN_Pos)
#define TCC_WAVE_WAVEGEN_DSCRITICAL (TCC_WAVE_WAVEGEN_DSCRITICAL_Val << TCC_WAVE_WAVEGEN_Pos)
#define TCC_WAVE_WAVEGEN_DSBOTTOM   (TCC_WAVE_WAVEGEN_DSBOTTOM_Val << TCC_WAVE_WAVEGEN_Pos)
#define TCC_WAVE_WAVEGEN_DSBOTH     (TCC_WAVE_WAVEGEN_DSBOTH_Val   << TCC_WAVE_WAVEGEN_Pos)
#define TCC_WAVE_WAVEGEN_DSTOP      (TCC_WAVE_WAVEGEN_DSTOP_Val    << TCC_WAVE_WAVEGEN_Pos)
#define TCC_WAVE_RAMP_Pos           4            /**< \brief (TCC_WAVE) Ramp Mode */
#define TCC_WAVE_RAMP_Msk           (_U_(0x3) << TCC_WAVE_RAMP_Pos)
#define TCC_WAVE_RAMP(value)        (TCC_WAVE_RAMP_Msk & ((value) << TCC_WAVE_RAMP_Pos))
#define   TCC_WAVE_RAMP_RAMP1_Val         _U_(0x0)   /**< \brief (TCC_WAVE) RAMP1 operation */
#define   TCC_WAVE_RAMP_RAMP2A_Val        _U_(0x1)   /**< \brief (TCC_WAVE) Alternative RAMP2 operation */
#define   TCC_WAVE_RAMP_RAMP2_Val         _U_(0x2)   /**< \brief (TCC_WAVE) RAMP2 operation */
#define   TCC_WAVE_RAMP_RAMP2C_Val        _U_(0x3)   /**< \brief (TCC_WAVE) Critical RAMP2 operation */
#define TCC_WAVE_RAMP_RAMP1         (TCC_WAVE_RAMP_RAMP1_Val       << TCC_WAVE_RAMP_Pos)
#define TCC_WAVE_RAMP_RAMP2A        (TCC_WAVE_RAMP_RAMP2A_Val      << TCC_WAVE_RAMP_Pos)
#define TCC_WAVE_RAMP_RAMP2         (TCC_WAVE_RAMP_RAMP2_Val       << TCC_WAVE_RAMP_Pos)
#define TCC_WAVE_RAMP_RAMP2C        (TCC_WAVE_RAMP_RAMP2C_Val      << TCC_WAVE_RAMP_Pos)
#define TCC_WAVE_CIPEREN_Pos        7            /**< \brief (TCC_WAVE) Circular period Enable */
#define TCC_WAVE_CIPEREN            (_U_(0x1) << TCC_WAVE_CIPEREN_Pos)
#define TCC_WAVE_CICCEN0_Pos        8            /**< \brief (TCC_WAVE) Circular Channel 0 Enable */
#define TCC_WAVE_CICCEN0            (_U_(1) << TCC_WAVE_CICCEN0_Pos)
#define TCC_WAVE_CICCEN1_Pos        9            /**< \brief (TCC_WAVE) Circular Channel 1 Enable */
#define TCC_WAVE_CICCEN1            (_U_(1) << TCC_WAVE_CICCEN1_Pos)
#define TCC_WAVE_CICCEN2_Pos        10           /**< \brief (TCC_WAVE) Circular Channel 2 Enable */
#define TCC_WAVE_CICCEN2            (_U_(1) << TCC_WAVE_CICCEN2_Pos)
#define TCC_WAVE_CICCEN3_Pos        11           /**< \brief (TCC_WAVE) Circular Channel 3 Enable */
#define TCC_WAVE_CICCEN3            (_U_(1) << TCC_WAVE_CICCEN3_Pos)
#define TCC_WAVE_CICCEN_Pos         8            /**< \brief (TCC_WAVE) Circular Channel x Enable */
#define TCC_WAVE_CICCEN_Msk         (_U_(0xF) << TCC_WAVE_CICCEN_Pos)
#define TCC_WAVE_CICCEN(value)      (TCC_WAVE_CICCEN_Msk & ((value) << TCC_WAVE_CICCEN_Pos))
#define TCC_WAVE_POL0_Pos           16           /**< \brief (TCC_WAVE) Channel 0 Polarity */
#define TCC_WAVE_POL0               (_U_(1) << TCC_WAVE_POL0_Pos)
#define TCC_WAVE_POL1_Pos           17           /**< \brief (TCC_WAVE) Channel 1 Polarity */
#define TCC_WAVE_POL1               (_U_(1) << TCC_WAVE_POL1_Pos)
#define TCC_WAVE_POL2_Pos           18           /**< \brief (TCC_WAVE) Channel 2 Polarity */
#define TCC_WAVE_POL2               (_U_(1) << TCC_WAVE_POL2_Pos)
#define TCC_WAVE_POL3_Pos           19           /**< \brief (TCC_WAVE) Channel 3 Polarity */
#define TCC_WAVE_POL3               (_U_(1) << TCC_WAVE_POL3_Pos)
#define TCC_WAVE_POL4_Pos           20           /**< \brief (TCC_WAVE) Channel 4 Polarity */
#define TCC_WAVE_POL4               (_U_(1) << TCC_WAVE_POL4_Pos)
#define TCC_WAVE_POL5_Pos           21           /**< \brief (TCC_WAVE) Channel 5 Polarity */
#define TCC_WAVE_POL5               (_U_(1) << TCC_WAVE_POL5_Pos)
#define TCC_WAVE_POL_Pos            16           /**< \brief (TCC_WAVE) Channel x Polarity */
#define TCC_WAVE_POL_Msk            (_U_(0x3F) << TCC_WAVE_POL_Pos)
#define TCC_WAVE_POL(value)         (TCC_WAVE_POL_Msk & ((value) << TCC_WAVE_POL_Pos))
#define TCC_WAVE_SWAP0_Pos          24           /**< \brief (TCC_WAVE) Swap DTI Output Pair 0 */
#define TCC_WAVE_SWAP0              (_U_(1) << TCC_WAVE_SWAP0_Pos)
#define TCC_WAVE_SWAP1_Pos          25           /**< \brief (TCC_WAVE) Swap DTI Output Pair 1 */
#define TCC_WAVE_SWAP1              (_U_(1) << TCC_WAVE_SWAP1_Pos)
#define TCC_WAVE_SWAP2_Pos          26           /**< \brief (TCC_WAVE) Swap DTI Output Pair 2 */
#define TCC_WAVE_SWAP2              (_U_(1) << TCC_WAVE_SWAP2_Pos)
#define TCC_WAVE_SWAP3_Pos          27           /**< \brief (TCC_WAVE) Swap DTI Output Pair 3 */
#define TCC_WAVE_SWAP3              (_U_(1) << TCC_WAVE_SWAP3_Pos)
#define TCC_WAVE_SWAP_Pos           24           /**< \brief (TCC_WAVE) Swap DTI Output Pair x */
#define TCC_WAVE_SWAP_Msk           (_U_(0xF) << TCC_WAVE_SWAP_Pos)
#define TCC_WAVE_SWAP(value)        (TCC_WAVE_SWAP_Msk & ((value) << TCC_WAVE_SWAP_Pos))
#define TCC_WAVE_MASK               _U_(0x0F3F0FB7) /**< \brief (TCC_WAVE) MASK Register */

/* -------- TCC_PER : (TCC Offset: 0x40) (R/W 32) Period -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct { // DITH4 mode
    uint32_t DITHER:4;         /*!< bit:  0.. 3  Dithering Cycle Number             */
    uint32_t PER:20;           /*!< bit:  4..23  Period Value                       */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } DITH4;                     /*!< Structure used for DITH4                        */
  struct { // DITH5 mode
    uint32_t DITHER:5;         /*!< bit:  0.. 4  Dithering Cycle Number             */
    uint32_t PER:19;           /*!< bit:  5..23  Period Value                       */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } DITH5;                     /*!< Structure used for DITH5                        */
  struct { // DITH6 mode
    uint32_t DITHER:6;         /*!< bit:  0.. 5  Dithering Cycle Number             */
    uint32_t PER:18;           /*!< bit:  6..23  Period Value                       */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } DITH6;                     /*!< Structure used for DITH6                        */
  struct {
    uint32_t PER:24;           /*!< bit:  0..23  Period Value                       */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_PER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_PER_OFFSET              0x40         /**< \brief (TCC_PER offset) Period */
#define TCC_PER_RESETVALUE          _U_(0xFFFFFFFF) /**< \brief (TCC_PER reset_value) Period */

// DITH4 mode
#define TCC_PER_DITH4_DITHER_Pos    0            /**< \brief (TCC_PER_DITH4) Dithering Cycle Number */
#define TCC_PER_DITH4_DITHER_Msk    (_U_(0xF) << TCC_PER_DITH4_DITHER_Pos)
#define TCC_PER_DITH4_DITHER(value) (TCC_PER_DITH4_DITHER_Msk & ((value) << TCC_PER_DITH4_DITHER_Pos))
#define TCC_PER_DITH4_PER_Pos       4            /**< \brief (TCC_PER_DITH4) Period Value */
#define TCC_PER_DITH4_PER_Msk       (_U_(0xFFFFF) << TCC_PER_DITH4_PER_Pos)
#define TCC_PER_DITH4_PER(value)    (TCC_PER_DITH4_PER_Msk & ((value) << TCC_PER_DITH4_PER_Pos))
#define TCC_PER_DITH4_MASK          _U_(0x00FFFFFF) /**< \brief (TCC_PER_DITH4) MASK Register */

// DITH5 mode
#define TCC_PER_DITH5_DITHER_Pos    0            /**< \brief (TCC_PER_DITH5) Dithering Cycle Number */
#define TCC_PER_DITH5_DITHER_Msk    (_U_(0x1F) << TCC_PER_DITH5_DITHER_Pos)
#define TCC_PER_DITH5_DITHER(value) (TCC_PER_DITH5_DITHER_Msk & ((value) << TCC_PER_DITH5_DITHER_Pos))
#define TCC_PER_DITH5_PER_Pos       5            /**< \brief (TCC_PER_DITH5) Period Value */
#define TCC_PER_DITH5_PER_Msk       (_U_(0x7FFFF) << TCC_PER_DITH5_PER_Pos)
#define TCC_PER_DITH5_PER(value)    (TCC_PER_DITH5_PER_Msk & ((value) << TCC_PER_DITH5_PER_Pos))
#define TCC_PER_DITH5_MASK          _U_(0x00FFFFFF) /**< \brief (TCC_PER_DITH5) MASK Register */

// DITH6 mode
#define TCC_PER_DITH6_DITHER_Pos    0            /**< \brief (TCC_PER_DITH6) Dithering Cycle Number */
#define TCC_PER_DITH6_DITHER_Msk    (_U_(0x3F) << TCC_PER_DITH6_DITHER_Pos)
#define TCC_PER_DITH6_DITHER(value) (TCC_PER_DITH6_DITHER_Msk & ((value) << TCC_PER_DITH6_DITHER_Pos))
#define TCC_PER_DITH6_PER_Pos       6            /**< \brief (TCC_PER_DITH6) Period Value */
#define TCC_PER_DITH6_PER_Msk       (_U_(0x3FFFF) << TCC_PER_DITH6_PER_Pos)
#define TCC_PER_DITH6_PER(value)    (TCC_PER_DITH6_PER_Msk & ((value) << TCC_PER_DITH6_PER_Pos))
#define TCC_PER_DITH6_MASK          _U_(0x00FFFFFF) /**< \brief (TCC_PER_DITH6) MASK Register */

#define TCC_PER_PER_Pos             0            /**< \brief (TCC_PER) Period Value */
#define TCC_PER_PER_Msk             (_U_(0xFFFFFF) << TCC_PER_PER_Pos)
#define TCC_PER_PER(value)          (TCC_PER_PER_Msk & ((value) << TCC_PER_PER_Pos))
#define TCC_PER_MASK                _U_(0x00FFFFFF) /**< \brief (TCC_PER) MASK Register */

/* -------- TCC_CC : (TCC Offset: 0x44) (R/W 32) Compare and Capture -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct { // DITH4 mode
    uint32_t DITHER:4;         /*!< bit:  0.. 3  Dithering Cycle Number             */
    uint32_t CC:20;            /*!< bit:  4..23  Channel Compare/Capture Value      */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } DITH4;                     /*!< Structure used for DITH4                        */
  struct { // DITH5 mode
    uint32_t DITHER:5;         /*!< bit:  0.. 4  Dithering Cycle Number             */
    uint32_t CC:19;            /*!< bit:  5..23  Channel Compare/Capture Value      */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } DITH5;                     /*!< Structure used for DITH5                        */
  struct { // DITH6 mode
    uint32_t DITHER:6;         /*!< bit:  0.. 5  Dithering Cycle Number             */
    uint32_t CC:18;            /*!< bit:  6..23  Channel Compare/Capture Value      */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } DITH6;                     /*!< Structure used for DITH6                        */
  struct {
    uint32_t CC:24;            /*!< bit:  0..23  Channel Compare/Capture Value      */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_CC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_CC_OFFSET               0x44         /**< \brief (TCC_CC offset) Compare and Capture */
#define TCC_CC_RESETVALUE           _U_(0x00000000) /**< \brief (TCC_CC reset_value) Compare and Capture */

// DITH4 mode
#define TCC_CC_DITH4_DITHER_Pos     0            /**< \brief (TCC_CC_DITH4) Dithering Cycle Number */
#define TCC_CC_DITH4_DITHER_Msk     (_U_(0xF) << TCC_CC_DITH4_DITHER_Pos)
#define TCC_CC_DITH4_DITHER(value)  (TCC_CC_DITH4_DITHER_Msk & ((value) << TCC_CC_DITH4_DITHER_Pos))
#define TCC_CC_DITH4_CC_Pos         4            /**< \brief (TCC_CC_DITH4) Channel Compare/Capture Value */
#define TCC_CC_DITH4_CC_Msk         (_U_(0xFFFFF) << TCC_CC_DITH4_CC_Pos)
#define TCC_CC_DITH4_CC(value)      (TCC_CC_DITH4_CC_Msk & ((value) << TCC_CC_DITH4_CC_Pos))
#define TCC_CC_DITH4_MASK           _U_(0x00FFFFFF) /**< \brief (TCC_CC_DITH4) MASK Register */

// DITH5 mode
#define TCC_CC_DITH5_DITHER_Pos     0            /**< \brief (TCC_CC_DITH5) Dithering Cycle Number */
#define TCC_CC_DITH5_DITHER_Msk     (_U_(0x1F) << TCC_CC_DITH5_DITHER_Pos)
#define TCC_CC_DITH5_DITHER(value)  (TCC_CC_DITH5_DITHER_Msk & ((value) << TCC_CC_DITH5_DITHER_Pos))
#define TCC_CC_DITH5_CC_Pos         5            /**< \brief (TCC_CC_DITH5) Channel Compare/Capture Value */
#define TCC_CC_DITH5_CC_Msk         (_U_(0x7FFFF) << TCC_CC_DITH5_CC_Pos)
#define TCC_CC_DITH5_CC(value)      (TCC_CC_DITH5_CC_Msk & ((value) << TCC_CC_DITH5_CC_Pos))
#define TCC_CC_DITH5_MASK           _U_(0x00FFFFFF) /**< \brief (TCC_CC_DITH5) MASK Register */

// DITH6 mode
#define TCC_CC_DITH6_DITHER_Pos     0            /**< \brief (TCC_CC_DITH6) Dithering Cycle Number */
#define TCC_CC_DITH6_DITHER_Msk     (_U_(0x3F) << TCC_CC_DITH6_DITHER_Pos)
#define TCC_CC_DITH6_DITHER(value)  (TCC_CC_DITH6_DITHER_Msk & ((value) << TCC_CC_DITH6_DITHER_Pos))
#define TCC_CC_DITH6_CC_Pos         6            /**< \brief (TCC_CC_DITH6) Channel Compare/Capture Value */
#define TCC_CC_DITH6_CC_Msk         (_U_(0x3FFFF) << TCC_CC_DITH6_CC_Pos)
#define TCC_CC_DITH6_CC(value)      (TCC_CC_DITH6_CC_Msk & ((value) << TCC_CC_DITH6_CC_Pos))
#define TCC_CC_DITH6_MASK           _U_(0x00FFFFFF) /**< \brief (TCC_CC_DITH6) MASK Register */

#define TCC_CC_CC_Pos               0            /**< \brief (TCC_CC) Channel Compare/Capture Value */
#define TCC_CC_CC_Msk               (_U_(0xFFFFFF) << TCC_CC_CC_Pos)
#define TCC_CC_CC(value)            (TCC_CC_CC_Msk & ((value) << TCC_CC_CC_Pos))
#define TCC_CC_MASK                 _U_(0x00FFFFFF) /**< \brief (TCC_CC) MASK Register */

/* -------- TCC_PATTBUF : (TCC Offset: 0x64) (R/W 16) Pattern Buffer -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PGEB0:1;          /*!< bit:      0  Pattern Generator 0 Output Enable Buffer */
    uint16_t PGEB1:1;          /*!< bit:      1  Pattern Generator 1 Output Enable Buffer */
    uint16_t PGEB2:1;          /*!< bit:      2  Pattern Generator 2 Output Enable Buffer */
    uint16_t PGEB3:1;          /*!< bit:      3  Pattern Generator 3 Output Enable Buffer */
    uint16_t PGEB4:1;          /*!< bit:      4  Pattern Generator 4 Output Enable Buffer */
    uint16_t PGEB5:1;          /*!< bit:      5  Pattern Generator 5 Output Enable Buffer */
    uint16_t PGEB6:1;          /*!< bit:      6  Pattern Generator 6 Output Enable Buffer */
    uint16_t PGEB7:1;          /*!< bit:      7  Pattern Generator 7 Output Enable Buffer */
    uint16_t PGVB0:1;          /*!< bit:      8  Pattern Generator 0 Output Enable  */
    uint16_t PGVB1:1;          /*!< bit:      9  Pattern Generator 1 Output Enable  */
    uint16_t PGVB2:1;          /*!< bit:     10  Pattern Generator 2 Output Enable  */
    uint16_t PGVB3:1;          /*!< bit:     11  Pattern Generator 3 Output Enable  */
    uint16_t PGVB4:1;          /*!< bit:     12  Pattern Generator 4 Output Enable  */
    uint16_t PGVB5:1;          /*!< bit:     13  Pattern Generator 5 Output Enable  */
    uint16_t PGVB6:1;          /*!< bit:     14  Pattern Generator 6 Output Enable  */
    uint16_t PGVB7:1;          /*!< bit:     15  Pattern Generator 7 Output Enable  */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PGEB:8;           /*!< bit:  0.. 7  Pattern Generator x Output Enable Buffer */
    uint16_t PGVB:8;           /*!< bit:  8..15  Pattern Generator x Output Enable  */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} TCC_PATTBUF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_PATTBUF_OFFSET          0x64         /**< \brief (TCC_PATTBUF offset) Pattern Buffer */
#define TCC_PATTBUF_RESETVALUE      _U_(0x0000)  /**< \brief (TCC_PATTBUF reset_value) Pattern Buffer */

#define TCC_PATTBUF_PGEB0_Pos       0            /**< \brief (TCC_PATTBUF) Pattern Generator 0 Output Enable Buffer */
#define TCC_PATTBUF_PGEB0           (_U_(1) << TCC_PATTBUF_PGEB0_Pos)
#define TCC_PATTBUF_PGEB1_Pos       1            /**< \brief (TCC_PATTBUF) Pattern Generator 1 Output Enable Buffer */
#define TCC_PATTBUF_PGEB1           (_U_(1) << TCC_PATTBUF_PGEB1_Pos)
#define TCC_PATTBUF_PGEB2_Pos       2            /**< \brief (TCC_PATTBUF) Pattern Generator 2 Output Enable Buffer */
#define TCC_PATTBUF_PGEB2           (_U_(1) << TCC_PATTBUF_PGEB2_Pos)
#define TCC_PATTBUF_PGEB3_Pos       3            /**< \brief (TCC_PATTBUF) Pattern Generator 3 Output Enable Buffer */
#define TCC_PATTBUF_PGEB3           (_U_(1) << TCC_PATTBUF_PGEB3_Pos)
#define TCC_PATTBUF_PGEB4_Pos       4            /**< \brief (TCC_PATTBUF) Pattern Generator 4 Output Enable Buffer */
#define TCC_PATTBUF_PGEB4           (_U_(1) << TCC_PATTBUF_PGEB4_Pos)
#define TCC_PATTBUF_PGEB5_Pos       5            /**< \brief (TCC_PATTBUF) Pattern Generator 5 Output Enable Buffer */
#define TCC_PATTBUF_PGEB5           (_U_(1) << TCC_PATTBUF_PGEB5_Pos)
#define TCC_PATTBUF_PGEB6_Pos       6            /**< \brief (TCC_PATTBUF) Pattern Generator 6 Output Enable Buffer */
#define TCC_PATTBUF_PGEB6           (_U_(1) << TCC_PATTBUF_PGEB6_Pos)
#define TCC_PATTBUF_PGEB7_Pos       7            /**< \brief (TCC_PATTBUF) Pattern Generator 7 Output Enable Buffer */
#define TCC_PATTBUF_PGEB7           (_U_(1) << TCC_PATTBUF_PGEB7_Pos)
#define TCC_PATTBUF_PGEB_Pos        0            /**< \brief (TCC_PATTBUF) Pattern Generator x Output Enable Buffer */
#define TCC_PATTBUF_PGEB_Msk        (_U_(0xFF) << TCC_PATTBUF_PGEB_Pos)
#define TCC_PATTBUF_PGEB(value)     (TCC_PATTBUF_PGEB_Msk & ((value) << TCC_PATTBUF_PGEB_Pos))
#define TCC_PATTBUF_PGVB0_Pos       8            /**< \brief (TCC_PATTBUF) Pattern Generator 0 Output Enable */
#define TCC_PATTBUF_PGVB0           (_U_(1) << TCC_PATTBUF_PGVB0_Pos)
#define TCC_PATTBUF_PGVB1_Pos       9            /**< \brief (TCC_PATTBUF) Pattern Generator 1 Output Enable */
#define TCC_PATTBUF_PGVB1           (_U_(1) << TCC_PATTBUF_PGVB1_Pos)
#define TCC_PATTBUF_PGVB2_Pos       10           /**< \brief (TCC_PATTBUF) Pattern Generator 2 Output Enable */
#define TCC_PATTBUF_PGVB2           (_U_(1) << TCC_PATTBUF_PGVB2_Pos)
#define TCC_PATTBUF_PGVB3_Pos       11           /**< \brief (TCC_PATTBUF) Pattern Generator 3 Output Enable */
#define TCC_PATTBUF_PGVB3           (_U_(1) << TCC_PATTBUF_PGVB3_Pos)
#define TCC_PATTBUF_PGVB4_Pos       12           /**< \brief (TCC_PATTBUF) Pattern Generator 4 Output Enable */
#define TCC_PATTBUF_PGVB4           (_U_(1) << TCC_PATTBUF_PGVB4_Pos)
#define TCC_PATTBUF_PGVB5_Pos       13           /**< \brief (TCC_PATTBUF) Pattern Generator 5 Output Enable */
#define TCC_PATTBUF_PGVB5           (_U_(1) << TCC_PATTBUF_PGVB5_Pos)
#define TCC_PATTBUF_PGVB6_Pos       14           /**< \brief (TCC_PATTBUF) Pattern Generator 6 Output Enable */
#define TCC_PATTBUF_PGVB6           (_U_(1) << TCC_PATTBUF_PGVB6_Pos)
#define TCC_PATTBUF_PGVB7_Pos       15           /**< \brief (TCC_PATTBUF) Pattern Generator 7 Output Enable */
#define TCC_PATTBUF_PGVB7           (_U_(1) << TCC_PATTBUF_PGVB7_Pos)
#define TCC_PATTBUF_PGVB_Pos        8            /**< \brief (TCC_PATTBUF) Pattern Generator x Output Enable */
#define TCC_PATTBUF_PGVB_Msk        (_U_(0xFF) << TCC_PATTBUF_PGVB_Pos)
#define TCC_PATTBUF_PGVB(value)     (TCC_PATTBUF_PGVB_Msk & ((value) << TCC_PATTBUF_PGVB_Pos))
#define TCC_PATTBUF_MASK            _U_(0xFFFF)  /**< \brief (TCC_PATTBUF) MASK Register */

/* -------- TCC_PERBUF : (TCC Offset: 0x6C) (R/W 32) Period Buffer -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct { // DITH4 mode
    uint32_t DITHERBUF:4;      /*!< bit:  0.. 3  Dithering Buffer Cycle Number      */
    uint32_t PERBUF:20;        /*!< bit:  4..23  Period Buffer Value                */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } DITH4;                     /*!< Structure used for DITH4                        */
  struct { // DITH5 mode
    uint32_t DITHERBUF:5;      /*!< bit:  0.. 4  Dithering Buffer Cycle Number      */
    uint32_t PERBUF:19;        /*!< bit:  5..23  Period Buffer Value                */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } DITH5;                     /*!< Structure used for DITH5                        */
  struct { // DITH6 mode
    uint32_t DITHERBUF:6;      /*!< bit:  0.. 5  Dithering Buffer Cycle Number      */
    uint32_t PERBUF:18;        /*!< bit:  6..23  Period Buffer Value                */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } DITH6;                     /*!< Structure used for DITH6                        */
  struct {
    uint32_t PERBUF:24;        /*!< bit:  0..23  Period Buffer Value                */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_PERBUF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_PERBUF_OFFSET           0x6C         /**< \brief (TCC_PERBUF offset) Period Buffer */
#define TCC_PERBUF_RESETVALUE       _U_(0xFFFFFFFF) /**< \brief (TCC_PERBUF reset_value) Period Buffer */

// DITH4 mode
#define TCC_PERBUF_DITH4_DITHERBUF_Pos 0            /**< \brief (TCC_PERBUF_DITH4) Dithering Buffer Cycle Number */
#define TCC_PERBUF_DITH4_DITHERBUF_Msk (_U_(0xF) << TCC_PERBUF_DITH4_DITHERBUF_Pos)
#define TCC_PERBUF_DITH4_DITHERBUF(value) (TCC_PERBUF_DITH4_DITHERBUF_Msk & ((value) << TCC_PERBUF_DITH4_DITHERBUF_Pos))
#define TCC_PERBUF_DITH4_PERBUF_Pos 4            /**< \brief (TCC_PERBUF_DITH4) Period Buffer Value */
#define TCC_PERBUF_DITH4_PERBUF_Msk (_U_(0xFFFFF) << TCC_PERBUF_DITH4_PERBUF_Pos)
#define TCC_PERBUF_DITH4_PERBUF(value) (TCC_PERBUF_DITH4_PERBUF_Msk & ((value) << TCC_PERBUF_DITH4_PERBUF_Pos))
#define TCC_PERBUF_DITH4_MASK       _U_(0x00FFFFFF) /**< \brief (TCC_PERBUF_DITH4) MASK Register */

// DITH5 mode
#define TCC_PERBUF_DITH5_DITHERBUF_Pos 0            /**< \brief (TCC_PERBUF_DITH5) Dithering Buffer Cycle Number */
#define TCC_PERBUF_DITH5_DITHERBUF_Msk (_U_(0x1F) << TCC_PERBUF_DITH5_DITHERBUF_Pos)
#define TCC_PERBUF_DITH5_DITHERBUF(value) (TCC_PERBUF_DITH5_DITHERBUF_Msk & ((value) << TCC_PERBUF_DITH5_DITHERBUF_Pos))
#define TCC_PERBUF_DITH5_PERBUF_Pos 5            /**< \brief (TCC_PERBUF_DITH5) Period Buffer Value */
#define TCC_PERBUF_DITH5_PERBUF_Msk (_U_(0x7FFFF) << TCC_PERBUF_DITH5_PERBUF_Pos)
#define TCC_PERBUF_DITH5_PERBUF(value) (TCC_PERBUF_DITH5_PERBUF_Msk & ((value) << TCC_PERBUF_DITH5_PERBUF_Pos))
#define TCC_PERBUF_DITH5_MASK       _U_(0x00FFFFFF) /**< \brief (TCC_PERBUF_DITH5) MASK Register */

// DITH6 mode
#define TCC_PERBUF_DITH6_DITHERBUF_Pos 0            /**< \brief (TCC_PERBUF_DITH6) Dithering Buffer Cycle Number */
#define TCC_PERBUF_DITH6_DITHERBUF_Msk (_U_(0x3F) << TCC_PERBUF_DITH6_DITHERBUF_Pos)
#define TCC_PERBUF_DITH6_DITHERBUF(value) (TCC_PERBUF_DITH6_DITHERBUF_Msk & ((value) << TCC_PERBUF_DITH6_DITHERBUF_Pos))
#define TCC_PERBUF_DITH6_PERBUF_Pos 6            /**< \brief (TCC_PERBUF_DITH6) Period Buffer Value */
#define TCC_PERBUF_DITH6_PERBUF_Msk (_U_(0x3FFFF) << TCC_PERBUF_DITH6_PERBUF_Pos)
#define TCC_PERBUF_DITH6_PERBUF(value) (TCC_PERBUF_DITH6_PERBUF_Msk & ((value) << TCC_PERBUF_DITH6_PERBUF_Pos))
#define TCC_PERBUF_DITH6_MASK       _U_(0x00FFFFFF) /**< \brief (TCC_PERBUF_DITH6) MASK Register */

#define TCC_PERBUF_PERBUF_Pos       0            /**< \brief (TCC_PERBUF) Period Buffer Value */
#define TCC_PERBUF_PERBUF_Msk       (_U_(0xFFFFFF) << TCC_PERBUF_PERBUF_Pos)
#define TCC_PERBUF_PERBUF(value)    (TCC_PERBUF_PERBUF_Msk & ((value) << TCC_PERBUF_PERBUF_Pos))
#define TCC_PERBUF_MASK             _U_(0x00FFFFFF) /**< \brief (TCC_PERBUF) MASK Register */

/* -------- TCC_CCBUF : (TCC Offset: 0x70) (R/W 32) Compare and Capture Buffer -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct { // DITH4 mode
    uint32_t CCBUF:4;          /*!< bit:  0.. 3  Channel Compare/Capture Buffer Value */
    uint32_t DITHERBUF:20;     /*!< bit:  4..23  Dithering Buffer Cycle Number      */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } DITH4;                     /*!< Structure used for DITH4                        */
  struct { // DITH5 mode
    uint32_t DITHERBUF:5;      /*!< bit:  0.. 4  Dithering Buffer Cycle Number      */
    uint32_t CCBUF:19;         /*!< bit:  5..23  Channel Compare/Capture Buffer Value */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } DITH5;                     /*!< Structure used for DITH5                        */
  struct { // DITH6 mode
    uint32_t DITHERBUF:6;      /*!< bit:  0.. 5  Dithering Buffer Cycle Number      */
    uint32_t CCBUF:18;         /*!< bit:  6..23  Channel Compare/Capture Buffer Value */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } DITH6;                     /*!< Structure used for DITH6                        */
  struct {
    uint32_t CCBUF:24;         /*!< bit:  0..23  Channel Compare/Capture Buffer Value */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TCC_CCBUF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TCC_CCBUF_OFFSET            0x70         /**< \brief (TCC_CCBUF offset) Compare and Capture Buffer */
#define TCC_CCBUF_RESETVALUE        _U_(0x00000000) /**< \brief (TCC_CCBUF reset_value) Compare and Capture Buffer */

// DITH4 mode
#define TCC_CCBUF_DITH4_CCBUF_Pos   0            /**< \brief (TCC_CCBUF_DITH4) Channel Compare/Capture Buffer Value */
#define TCC_CCBUF_DITH4_CCBUF_Msk   (_U_(0xF) << TCC_CCBUF_DITH4_CCBUF_Pos)
#define TCC_CCBUF_DITH4_CCBUF(value) (TCC_CCBUF_DITH4_CCBUF_Msk & ((value) << TCC_CCBUF_DITH4_CCBUF_Pos))
#define TCC_CCBUF_DITH4_DITHERBUF_Pos 4            /**< \brief (TCC_CCBUF_DITH4) Dithering Buffer Cycle Number */
#define TCC_CCBUF_DITH4_DITHERBUF_Msk (_U_(0xFFFFF) << TCC_CCBUF_DITH4_DITHERBUF_Pos)
#define TCC_CCBUF_DITH4_DITHERBUF(value) (TCC_CCBUF_DITH4_DITHERBUF_Msk & ((value) << TCC_CCBUF_DITH4_DITHERBUF_Pos))
#define TCC_CCBUF_DITH4_MASK        _U_(0x00FFFFFF) /**< \brief (TCC_CCBUF_DITH4) MASK Register */

// DITH5 mode
#define TCC_CCBUF_DITH5_DITHERBUF_Pos 0            /**< \brief (TCC_CCBUF_DITH5) Dithering Buffer Cycle Number */
#define TCC_CCBUF_DITH5_DITHERBUF_Msk (_U_(0x1F) << TCC_CCBUF_DITH5_DITHERBUF_Pos)
#define TCC_CCBUF_DITH5_DITHERBUF(value) (TCC_CCBUF_DITH5_DITHERBUF_Msk & ((value) << TCC_CCBUF_DITH5_DITHERBUF_Pos))
#define TCC_CCBUF_DITH5_CCBUF_Pos   5            /**< \brief (TCC_CCBUF_DITH5) Channel Compare/Capture Buffer Value */
#define TCC_CCBUF_DITH5_CCBUF_Msk   (_U_(0x7FFFF) << TCC_CCBUF_DITH5_CCBUF_Pos)
#define TCC_CCBUF_DITH5_CCBUF(value) (TCC_CCBUF_DITH5_CCBUF_Msk & ((value) << TCC_CCBUF_DITH5_CCBUF_Pos))
#define TCC_CCBUF_DITH5_MASK        _U_(0x00FFFFFF) /**< \brief (TCC_CCBUF_DITH5) MASK Register */

// DITH6 mode
#define TCC_CCBUF_DITH6_DITHERBUF_Pos 0            /**< \brief (TCC_CCBUF_DITH6) Dithering Buffer Cycle Number */
#define TCC_CCBUF_DITH6_DITHERBUF_Msk (_U_(0x3F) << TCC_CCBUF_DITH6_DITHERBUF_Pos)
#define TCC_CCBUF_DITH6_DITHERBUF(value) (TCC_CCBUF_DITH6_DITHERBUF_Msk & ((value) << TCC_CCBUF_DITH6_DITHERBUF_Pos))
#define TCC_CCBUF_DITH6_CCBUF_Pos   6            /**< \brief (TCC_CCBUF_DITH6) Channel Compare/Capture Buffer Value */
#define TCC_CCBUF_DITH6_CCBUF_Msk   (_U_(0x3FFFF) << TCC_CCBUF_DITH6_CCBUF_Pos)
#define TCC_CCBUF_DITH6_CCBUF(value) (TCC_CCBUF_DITH6_CCBUF_Msk & ((value) << TCC_CCBUF_DITH6_CCBUF_Pos))
#define TCC_CCBUF_DITH6_MASK        _U_(0x00FFFFFF) /**< \brief (TCC_CCBUF_DITH6) MASK Register */

#define TCC_CCBUF_CCBUF_Pos         0            /**< \brief (TCC_CCBUF) Channel Compare/Capture Buffer Value */
#define TCC_CCBUF_CCBUF_Msk         (_U_(0xFFFFFF) << TCC_CCBUF_CCBUF_Pos)
#define TCC_CCBUF_CCBUF(value)      (TCC_CCBUF_CCBUF_Msk & ((value) << TCC_CCBUF_CCBUF_Pos))
#define TCC_CCBUF_MASK              _U_(0x00FFFFFF) /**< \brief (TCC_CCBUF) MASK Register */

/** \brief TCC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO TCC_CTRLA_Type            CTRLA;       /**< \brief Offset: 0x00 (R/W 32) Control A */
  __IO TCC_CTRLBCLR_Type         CTRLBCLR;    /**< \brief Offset: 0x04 (R/W  8) Control B Clear */
  __IO TCC_CTRLBSET_Type         CTRLBSET;    /**< \brief Offset: 0x05 (R/W  8) Control B Set */
       RoReg8                    Reserved1[0x2];
  __I  TCC_SYNCBUSY_Type         SYNCBUSY;    /**< \brief Offset: 0x08 (R/  32) Synchronization Busy */
  __IO TCC_FCTRLA_Type           FCTRLA;      /**< \brief Offset: 0x0C (R/W 32) Recoverable Fault A Configuration */
  __IO TCC_FCTRLB_Type           FCTRLB;      /**< \brief Offset: 0x10 (R/W 32) Recoverable Fault B Configuration */
  __IO TCC_WEXCTRL_Type          WEXCTRL;     /**< \brief Offset: 0x14 (R/W 32) Waveform Extension Configuration */
  __IO TCC_DRVCTRL_Type          DRVCTRL;     /**< \brief Offset: 0x18 (R/W 32) Driver Control */
       RoReg8                    Reserved2[0x2];
  __IO TCC_DBGCTRL_Type          DBGCTRL;     /**< \brief Offset: 0x1E (R/W  8) Debug Control */
       RoReg8                    Reserved3[0x1];
  __IO TCC_EVCTRL_Type           EVCTRL;      /**< \brief Offset: 0x20 (R/W 32) Event Control */
  __IO TCC_INTENCLR_Type         INTENCLR;    /**< \brief Offset: 0x24 (R/W 32) Interrupt Enable Clear */
  __IO TCC_INTENSET_Type         INTENSET;    /**< \brief Offset: 0x28 (R/W 32) Interrupt Enable Set */
  __IO TCC_INTFLAG_Type          INTFLAG;     /**< \brief Offset: 0x2C (R/W 32) Interrupt Flag Status and Clear */
  __IO TCC_STATUS_Type           STATUS;      /**< \brief Offset: 0x30 (R/W 32) Status */
  __IO TCC_COUNT_Type            COUNT;       /**< \brief Offset: 0x34 (R/W 32) Count */
  __IO TCC_PATT_Type             PATT;        /**< \brief Offset: 0x38 (R/W 16) Pattern */
       RoReg8                    Reserved4[0x2];
  __IO TCC_WAVE_Type             WAVE;        /**< \brief Offset: 0x3C (R/W 32) Waveform Control */
  __IO TCC_PER_Type              PER;         /**< \brief Offset: 0x40 (R/W 32) Period */
  __IO TCC_CC_Type               CC[6];       /**< \brief Offset: 0x44 (R/W 32) Compare and Capture */
       RoReg8                    Reserved5[0x8];
  __IO TCC_PATTBUF_Type          PATTBUF;     /**< \brief Offset: 0x64 (R/W 16) Pattern Buffer */
       RoReg8                    Reserved6[0x6];
  __IO TCC_PERBUF_Type           PERBUF;      /**< \brief Offset: 0x6C (R/W 32) Period Buffer */
  __IO TCC_CCBUF_Type            CCBUF[6];    /**< \brief Offset: 0x70 (R/W 32) Compare and Capture Buffer */
} Tcc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_TCC_COMPONENT_ */
