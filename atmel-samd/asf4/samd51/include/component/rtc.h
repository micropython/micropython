/**
 * \file
 *
 * \brief Component description for RTC
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

#ifndef _SAMD51_RTC_COMPONENT_
#define _SAMD51_RTC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR RTC */
/* ========================================================================== */
/** \addtogroup SAMD51_RTC Real-Time Counter */
/*@{*/

#define RTC_U2250
#define REV_RTC                     0x210

/* -------- RTC_MODE0_CTRLA : (RTC Offset: 0x00) (R/W 16) MODE0 MODE0 Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint16_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint16_t MODE:2;           /*!< bit:  2.. 3  Operating Mode                     */
    uint16_t :3;               /*!< bit:  4.. 6  Reserved                           */
    uint16_t MATCHCLR:1;       /*!< bit:      7  Clear on Match                     */
    uint16_t PRESCALER:4;      /*!< bit:  8..11  Prescaler                          */
    uint16_t :1;               /*!< bit:     12  Reserved                           */
    uint16_t BKTRST:1;         /*!< bit:     13  BKUP Registers Reset On Tamper Enable */
    uint16_t GPTRST:1;         /*!< bit:     14  GP Registers Reset On Tamper Enable */
    uint16_t COUNTSYNC:1;      /*!< bit:     15  Count Read Synchronization Enable  */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_CTRLA_OFFSET      0x00         /**< \brief (RTC_MODE0_CTRLA offset) MODE0 Control A */
#define RTC_MODE0_CTRLA_RESETVALUE  _U_(0x0000)  /**< \brief (RTC_MODE0_CTRLA reset_value) MODE0 Control A */

#define RTC_MODE0_CTRLA_SWRST_Pos   0            /**< \brief (RTC_MODE0_CTRLA) Software Reset */
#define RTC_MODE0_CTRLA_SWRST       (_U_(0x1) << RTC_MODE0_CTRLA_SWRST_Pos)
#define RTC_MODE0_CTRLA_ENABLE_Pos  1            /**< \brief (RTC_MODE0_CTRLA) Enable */
#define RTC_MODE0_CTRLA_ENABLE      (_U_(0x1) << RTC_MODE0_CTRLA_ENABLE_Pos)
#define RTC_MODE0_CTRLA_MODE_Pos    2            /**< \brief (RTC_MODE0_CTRLA) Operating Mode */
#define RTC_MODE0_CTRLA_MODE_Msk    (_U_(0x3) << RTC_MODE0_CTRLA_MODE_Pos)
#define RTC_MODE0_CTRLA_MODE(value) (RTC_MODE0_CTRLA_MODE_Msk & ((value) << RTC_MODE0_CTRLA_MODE_Pos))
#define   RTC_MODE0_CTRLA_MODE_COUNT32_Val _U_(0x0)   /**< \brief (RTC_MODE0_CTRLA) Mode 0: 32-bit Counter */
#define   RTC_MODE0_CTRLA_MODE_COUNT16_Val _U_(0x1)   /**< \brief (RTC_MODE0_CTRLA) Mode 1: 16-bit Counter */
#define   RTC_MODE0_CTRLA_MODE_CLOCK_Val  _U_(0x2)   /**< \brief (RTC_MODE0_CTRLA) Mode 2: Clock/Calendar */
#define RTC_MODE0_CTRLA_MODE_COUNT32 (RTC_MODE0_CTRLA_MODE_COUNT32_Val << RTC_MODE0_CTRLA_MODE_Pos)
#define RTC_MODE0_CTRLA_MODE_COUNT16 (RTC_MODE0_CTRLA_MODE_COUNT16_Val << RTC_MODE0_CTRLA_MODE_Pos)
#define RTC_MODE0_CTRLA_MODE_CLOCK  (RTC_MODE0_CTRLA_MODE_CLOCK_Val << RTC_MODE0_CTRLA_MODE_Pos)
#define RTC_MODE0_CTRLA_MATCHCLR_Pos 7            /**< \brief (RTC_MODE0_CTRLA) Clear on Match */
#define RTC_MODE0_CTRLA_MATCHCLR    (_U_(0x1) << RTC_MODE0_CTRLA_MATCHCLR_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_Pos 8            /**< \brief (RTC_MODE0_CTRLA) Prescaler */
#define RTC_MODE0_CTRLA_PRESCALER_Msk (_U_(0xF) << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER(value) (RTC_MODE0_CTRLA_PRESCALER_Msk & ((value) << RTC_MODE0_CTRLA_PRESCALER_Pos))
#define   RTC_MODE0_CTRLA_PRESCALER_OFF_Val _U_(0x0)   /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/1 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV1_Val _U_(0x1)   /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/1 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV2_Val _U_(0x2)   /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/2 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV4_Val _U_(0x3)   /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/4 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV8_Val _U_(0x4)   /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/8 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV16_Val _U_(0x5)   /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/16 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV32_Val _U_(0x6)   /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/32 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV64_Val _U_(0x7)   /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/64 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV128_Val _U_(0x8)   /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/128 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV256_Val _U_(0x9)   /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/256 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV512_Val _U_(0xA)   /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/512 */
#define   RTC_MODE0_CTRLA_PRESCALER_DIV1024_Val _U_(0xB)   /**< \brief (RTC_MODE0_CTRLA) CLK_RTC_CNT = GCLK_RTC/1024 */
#define RTC_MODE0_CTRLA_PRESCALER_OFF (RTC_MODE0_CTRLA_PRESCALER_OFF_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV1 (RTC_MODE0_CTRLA_PRESCALER_DIV1_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV2 (RTC_MODE0_CTRLA_PRESCALER_DIV2_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV4 (RTC_MODE0_CTRLA_PRESCALER_DIV4_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV8 (RTC_MODE0_CTRLA_PRESCALER_DIV8_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV16 (RTC_MODE0_CTRLA_PRESCALER_DIV16_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV32 (RTC_MODE0_CTRLA_PRESCALER_DIV32_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV64 (RTC_MODE0_CTRLA_PRESCALER_DIV64_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV128 (RTC_MODE0_CTRLA_PRESCALER_DIV128_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV256 (RTC_MODE0_CTRLA_PRESCALER_DIV256_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV512 (RTC_MODE0_CTRLA_PRESCALER_DIV512_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_PRESCALER_DIV1024 (RTC_MODE0_CTRLA_PRESCALER_DIV1024_Val << RTC_MODE0_CTRLA_PRESCALER_Pos)
#define RTC_MODE0_CTRLA_BKTRST_Pos  13           /**< \brief (RTC_MODE0_CTRLA) BKUP Registers Reset On Tamper Enable */
#define RTC_MODE0_CTRLA_BKTRST      (_U_(0x1) << RTC_MODE0_CTRLA_BKTRST_Pos)
#define RTC_MODE0_CTRLA_GPTRST_Pos  14           /**< \brief (RTC_MODE0_CTRLA) GP Registers Reset On Tamper Enable */
#define RTC_MODE0_CTRLA_GPTRST      (_U_(0x1) << RTC_MODE0_CTRLA_GPTRST_Pos)
#define RTC_MODE0_CTRLA_COUNTSYNC_Pos 15           /**< \brief (RTC_MODE0_CTRLA) Count Read Synchronization Enable */
#define RTC_MODE0_CTRLA_COUNTSYNC   (_U_(0x1) << RTC_MODE0_CTRLA_COUNTSYNC_Pos)
#define RTC_MODE0_CTRLA_MASK        _U_(0xEF8F)  /**< \brief (RTC_MODE0_CTRLA) MASK Register */

/* -------- RTC_MODE1_CTRLA : (RTC Offset: 0x00) (R/W 16) MODE1 MODE1 Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint16_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint16_t MODE:2;           /*!< bit:  2.. 3  Operating Mode                     */
    uint16_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint16_t PRESCALER:4;      /*!< bit:  8..11  Prescaler                          */
    uint16_t :1;               /*!< bit:     12  Reserved                           */
    uint16_t BKTRST:1;         /*!< bit:     13  BKUP Registers Reset On Tamper Enable */
    uint16_t GPTRST:1;         /*!< bit:     14  GP Registers Reset On Tamper Enable */
    uint16_t COUNTSYNC:1;      /*!< bit:     15  Count Read Synchronization Enable  */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_CTRLA_OFFSET      0x00         /**< \brief (RTC_MODE1_CTRLA offset) MODE1 Control A */
#define RTC_MODE1_CTRLA_RESETVALUE  _U_(0x0000)  /**< \brief (RTC_MODE1_CTRLA reset_value) MODE1 Control A */

#define RTC_MODE1_CTRLA_SWRST_Pos   0            /**< \brief (RTC_MODE1_CTRLA) Software Reset */
#define RTC_MODE1_CTRLA_SWRST       (_U_(0x1) << RTC_MODE1_CTRLA_SWRST_Pos)
#define RTC_MODE1_CTRLA_ENABLE_Pos  1            /**< \brief (RTC_MODE1_CTRLA) Enable */
#define RTC_MODE1_CTRLA_ENABLE      (_U_(0x1) << RTC_MODE1_CTRLA_ENABLE_Pos)
#define RTC_MODE1_CTRLA_MODE_Pos    2            /**< \brief (RTC_MODE1_CTRLA) Operating Mode */
#define RTC_MODE1_CTRLA_MODE_Msk    (_U_(0x3) << RTC_MODE1_CTRLA_MODE_Pos)
#define RTC_MODE1_CTRLA_MODE(value) (RTC_MODE1_CTRLA_MODE_Msk & ((value) << RTC_MODE1_CTRLA_MODE_Pos))
#define   RTC_MODE1_CTRLA_MODE_COUNT32_Val _U_(0x0)   /**< \brief (RTC_MODE1_CTRLA) Mode 0: 32-bit Counter */
#define   RTC_MODE1_CTRLA_MODE_COUNT16_Val _U_(0x1)   /**< \brief (RTC_MODE1_CTRLA) Mode 1: 16-bit Counter */
#define   RTC_MODE1_CTRLA_MODE_CLOCK_Val  _U_(0x2)   /**< \brief (RTC_MODE1_CTRLA) Mode 2: Clock/Calendar */
#define RTC_MODE1_CTRLA_MODE_COUNT32 (RTC_MODE1_CTRLA_MODE_COUNT32_Val << RTC_MODE1_CTRLA_MODE_Pos)
#define RTC_MODE1_CTRLA_MODE_COUNT16 (RTC_MODE1_CTRLA_MODE_COUNT16_Val << RTC_MODE1_CTRLA_MODE_Pos)
#define RTC_MODE1_CTRLA_MODE_CLOCK  (RTC_MODE1_CTRLA_MODE_CLOCK_Val << RTC_MODE1_CTRLA_MODE_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_Pos 8            /**< \brief (RTC_MODE1_CTRLA) Prescaler */
#define RTC_MODE1_CTRLA_PRESCALER_Msk (_U_(0xF) << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER(value) (RTC_MODE1_CTRLA_PRESCALER_Msk & ((value) << RTC_MODE1_CTRLA_PRESCALER_Pos))
#define   RTC_MODE1_CTRLA_PRESCALER_OFF_Val _U_(0x0)   /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/1 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV1_Val _U_(0x1)   /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/1 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV2_Val _U_(0x2)   /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/2 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV4_Val _U_(0x3)   /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/4 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV8_Val _U_(0x4)   /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/8 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV16_Val _U_(0x5)   /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/16 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV32_Val _U_(0x6)   /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/32 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV64_Val _U_(0x7)   /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/64 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV128_Val _U_(0x8)   /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/128 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV256_Val _U_(0x9)   /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/256 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV512_Val _U_(0xA)   /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/512 */
#define   RTC_MODE1_CTRLA_PRESCALER_DIV1024_Val _U_(0xB)   /**< \brief (RTC_MODE1_CTRLA) CLK_RTC_CNT = GCLK_RTC/1024 */
#define RTC_MODE1_CTRLA_PRESCALER_OFF (RTC_MODE1_CTRLA_PRESCALER_OFF_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV1 (RTC_MODE1_CTRLA_PRESCALER_DIV1_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV2 (RTC_MODE1_CTRLA_PRESCALER_DIV2_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV4 (RTC_MODE1_CTRLA_PRESCALER_DIV4_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV8 (RTC_MODE1_CTRLA_PRESCALER_DIV8_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV16 (RTC_MODE1_CTRLA_PRESCALER_DIV16_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV32 (RTC_MODE1_CTRLA_PRESCALER_DIV32_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV64 (RTC_MODE1_CTRLA_PRESCALER_DIV64_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV128 (RTC_MODE1_CTRLA_PRESCALER_DIV128_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV256 (RTC_MODE1_CTRLA_PRESCALER_DIV256_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV512 (RTC_MODE1_CTRLA_PRESCALER_DIV512_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_PRESCALER_DIV1024 (RTC_MODE1_CTRLA_PRESCALER_DIV1024_Val << RTC_MODE1_CTRLA_PRESCALER_Pos)
#define RTC_MODE1_CTRLA_BKTRST_Pos  13           /**< \brief (RTC_MODE1_CTRLA) BKUP Registers Reset On Tamper Enable */
#define RTC_MODE1_CTRLA_BKTRST      (_U_(0x1) << RTC_MODE1_CTRLA_BKTRST_Pos)
#define RTC_MODE1_CTRLA_GPTRST_Pos  14           /**< \brief (RTC_MODE1_CTRLA) GP Registers Reset On Tamper Enable */
#define RTC_MODE1_CTRLA_GPTRST      (_U_(0x1) << RTC_MODE1_CTRLA_GPTRST_Pos)
#define RTC_MODE1_CTRLA_COUNTSYNC_Pos 15           /**< \brief (RTC_MODE1_CTRLA) Count Read Synchronization Enable */
#define RTC_MODE1_CTRLA_COUNTSYNC   (_U_(0x1) << RTC_MODE1_CTRLA_COUNTSYNC_Pos)
#define RTC_MODE1_CTRLA_MASK        _U_(0xEF0F)  /**< \brief (RTC_MODE1_CTRLA) MASK Register */

/* -------- RTC_MODE2_CTRLA : (RTC Offset: 0x00) (R/W 16) MODE2 MODE2 Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint16_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint16_t MODE:2;           /*!< bit:  2.. 3  Operating Mode                     */
    uint16_t :2;               /*!< bit:  4.. 5  Reserved                           */
    uint16_t CLKREP:1;         /*!< bit:      6  Clock Representation               */
    uint16_t MATCHCLR:1;       /*!< bit:      7  Clear on Match                     */
    uint16_t PRESCALER:4;      /*!< bit:  8..11  Prescaler                          */
    uint16_t :1;               /*!< bit:     12  Reserved                           */
    uint16_t BKTRST:1;         /*!< bit:     13  BKUP Registers Reset On Tamper Enable */
    uint16_t GPTRST:1;         /*!< bit:     14  GP Registers Reset On Tamper Enable */
    uint16_t CLOCKSYNC:1;      /*!< bit:     15  Clock Read Synchronization Enable  */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_CTRLA_OFFSET      0x00         /**< \brief (RTC_MODE2_CTRLA offset) MODE2 Control A */
#define RTC_MODE2_CTRLA_RESETVALUE  _U_(0x0000)  /**< \brief (RTC_MODE2_CTRLA reset_value) MODE2 Control A */

#define RTC_MODE2_CTRLA_SWRST_Pos   0            /**< \brief (RTC_MODE2_CTRLA) Software Reset */
#define RTC_MODE2_CTRLA_SWRST       (_U_(0x1) << RTC_MODE2_CTRLA_SWRST_Pos)
#define RTC_MODE2_CTRLA_ENABLE_Pos  1            /**< \brief (RTC_MODE2_CTRLA) Enable */
#define RTC_MODE2_CTRLA_ENABLE      (_U_(0x1) << RTC_MODE2_CTRLA_ENABLE_Pos)
#define RTC_MODE2_CTRLA_MODE_Pos    2            /**< \brief (RTC_MODE2_CTRLA) Operating Mode */
#define RTC_MODE2_CTRLA_MODE_Msk    (_U_(0x3) << RTC_MODE2_CTRLA_MODE_Pos)
#define RTC_MODE2_CTRLA_MODE(value) (RTC_MODE2_CTRLA_MODE_Msk & ((value) << RTC_MODE2_CTRLA_MODE_Pos))
#define   RTC_MODE2_CTRLA_MODE_COUNT32_Val _U_(0x0)   /**< \brief (RTC_MODE2_CTRLA) Mode 0: 32-bit Counter */
#define   RTC_MODE2_CTRLA_MODE_COUNT16_Val _U_(0x1)   /**< \brief (RTC_MODE2_CTRLA) Mode 1: 16-bit Counter */
#define   RTC_MODE2_CTRLA_MODE_CLOCK_Val  _U_(0x2)   /**< \brief (RTC_MODE2_CTRLA) Mode 2: Clock/Calendar */
#define RTC_MODE2_CTRLA_MODE_COUNT32 (RTC_MODE2_CTRLA_MODE_COUNT32_Val << RTC_MODE2_CTRLA_MODE_Pos)
#define RTC_MODE2_CTRLA_MODE_COUNT16 (RTC_MODE2_CTRLA_MODE_COUNT16_Val << RTC_MODE2_CTRLA_MODE_Pos)
#define RTC_MODE2_CTRLA_MODE_CLOCK  (RTC_MODE2_CTRLA_MODE_CLOCK_Val << RTC_MODE2_CTRLA_MODE_Pos)
#define RTC_MODE2_CTRLA_CLKREP_Pos  6            /**< \brief (RTC_MODE2_CTRLA) Clock Representation */
#define RTC_MODE2_CTRLA_CLKREP      (_U_(0x1) << RTC_MODE2_CTRLA_CLKREP_Pos)
#define RTC_MODE2_CTRLA_MATCHCLR_Pos 7            /**< \brief (RTC_MODE2_CTRLA) Clear on Match */
#define RTC_MODE2_CTRLA_MATCHCLR    (_U_(0x1) << RTC_MODE2_CTRLA_MATCHCLR_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_Pos 8            /**< \brief (RTC_MODE2_CTRLA) Prescaler */
#define RTC_MODE2_CTRLA_PRESCALER_Msk (_U_(0xF) << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER(value) (RTC_MODE2_CTRLA_PRESCALER_Msk & ((value) << RTC_MODE2_CTRLA_PRESCALER_Pos))
#define   RTC_MODE2_CTRLA_PRESCALER_OFF_Val _U_(0x0)   /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/1 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV1_Val _U_(0x1)   /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/1 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV2_Val _U_(0x2)   /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/2 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV4_Val _U_(0x3)   /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/4 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV8_Val _U_(0x4)   /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/8 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV16_Val _U_(0x5)   /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/16 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV32_Val _U_(0x6)   /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/32 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV64_Val _U_(0x7)   /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/64 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV128_Val _U_(0x8)   /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/128 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV256_Val _U_(0x9)   /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/256 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV512_Val _U_(0xA)   /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/512 */
#define   RTC_MODE2_CTRLA_PRESCALER_DIV1024_Val _U_(0xB)   /**< \brief (RTC_MODE2_CTRLA) CLK_RTC_CNT = GCLK_RTC/1024 */
#define RTC_MODE2_CTRLA_PRESCALER_OFF (RTC_MODE2_CTRLA_PRESCALER_OFF_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV1 (RTC_MODE2_CTRLA_PRESCALER_DIV1_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV2 (RTC_MODE2_CTRLA_PRESCALER_DIV2_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV4 (RTC_MODE2_CTRLA_PRESCALER_DIV4_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV8 (RTC_MODE2_CTRLA_PRESCALER_DIV8_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV16 (RTC_MODE2_CTRLA_PRESCALER_DIV16_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV32 (RTC_MODE2_CTRLA_PRESCALER_DIV32_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV64 (RTC_MODE2_CTRLA_PRESCALER_DIV64_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV128 (RTC_MODE2_CTRLA_PRESCALER_DIV128_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV256 (RTC_MODE2_CTRLA_PRESCALER_DIV256_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV512 (RTC_MODE2_CTRLA_PRESCALER_DIV512_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_PRESCALER_DIV1024 (RTC_MODE2_CTRLA_PRESCALER_DIV1024_Val << RTC_MODE2_CTRLA_PRESCALER_Pos)
#define RTC_MODE2_CTRLA_BKTRST_Pos  13           /**< \brief (RTC_MODE2_CTRLA) BKUP Registers Reset On Tamper Enable */
#define RTC_MODE2_CTRLA_BKTRST      (_U_(0x1) << RTC_MODE2_CTRLA_BKTRST_Pos)
#define RTC_MODE2_CTRLA_GPTRST_Pos  14           /**< \brief (RTC_MODE2_CTRLA) GP Registers Reset On Tamper Enable */
#define RTC_MODE2_CTRLA_GPTRST      (_U_(0x1) << RTC_MODE2_CTRLA_GPTRST_Pos)
#define RTC_MODE2_CTRLA_CLOCKSYNC_Pos 15           /**< \brief (RTC_MODE2_CTRLA) Clock Read Synchronization Enable */
#define RTC_MODE2_CTRLA_CLOCKSYNC   (_U_(0x1) << RTC_MODE2_CTRLA_CLOCKSYNC_Pos)
#define RTC_MODE2_CTRLA_MASK        _U_(0xEFCF)  /**< \brief (RTC_MODE2_CTRLA) MASK Register */

/* -------- RTC_MODE0_CTRLB : (RTC Offset: 0x02) (R/W 16) MODE0 MODE0 Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t GP0EN:1;          /*!< bit:      0  General Purpose 0 Enable           */
    uint16_t GP2EN:1;          /*!< bit:      1  General Purpose 2 Enable           */
    uint16_t :2;               /*!< bit:  2.. 3  Reserved                           */
    uint16_t DEBMAJ:1;         /*!< bit:      4  Debouncer Majority Enable          */
    uint16_t DEBASYNC:1;       /*!< bit:      5  Debouncer Asynchronous Enable      */
    uint16_t RTCOUT:1;         /*!< bit:      6  RTC Output Enable                  */
    uint16_t DMAEN:1;          /*!< bit:      7  DMA Enable                         */
    uint16_t DEBF:3;           /*!< bit:  8..10  Debounce Freqnuency                */
    uint16_t :1;               /*!< bit:     11  Reserved                           */
    uint16_t ACTF:3;           /*!< bit: 12..14  Active Layer Freqnuency            */
    uint16_t :1;               /*!< bit:     15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_CTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_CTRLB_OFFSET      0x02         /**< \brief (RTC_MODE0_CTRLB offset) MODE0 Control B */
#define RTC_MODE0_CTRLB_RESETVALUE  _U_(0x0000)  /**< \brief (RTC_MODE0_CTRLB reset_value) MODE0 Control B */

#define RTC_MODE0_CTRLB_GP0EN_Pos   0            /**< \brief (RTC_MODE0_CTRLB) General Purpose 0 Enable */
#define RTC_MODE0_CTRLB_GP0EN       (_U_(0x1) << RTC_MODE0_CTRLB_GP0EN_Pos)
#define RTC_MODE0_CTRLB_GP2EN_Pos   1            /**< \brief (RTC_MODE0_CTRLB) General Purpose 2 Enable */
#define RTC_MODE0_CTRLB_GP2EN       (_U_(0x1) << RTC_MODE0_CTRLB_GP2EN_Pos)
#define RTC_MODE0_CTRLB_DEBMAJ_Pos  4            /**< \brief (RTC_MODE0_CTRLB) Debouncer Majority Enable */
#define RTC_MODE0_CTRLB_DEBMAJ      (_U_(0x1) << RTC_MODE0_CTRLB_DEBMAJ_Pos)
#define RTC_MODE0_CTRLB_DEBASYNC_Pos 5            /**< \brief (RTC_MODE0_CTRLB) Debouncer Asynchronous Enable */
#define RTC_MODE0_CTRLB_DEBASYNC    (_U_(0x1) << RTC_MODE0_CTRLB_DEBASYNC_Pos)
#define RTC_MODE0_CTRLB_RTCOUT_Pos  6            /**< \brief (RTC_MODE0_CTRLB) RTC Output Enable */
#define RTC_MODE0_CTRLB_RTCOUT      (_U_(0x1) << RTC_MODE0_CTRLB_RTCOUT_Pos)
#define RTC_MODE0_CTRLB_DMAEN_Pos   7            /**< \brief (RTC_MODE0_CTRLB) DMA Enable */
#define RTC_MODE0_CTRLB_DMAEN       (_U_(0x1) << RTC_MODE0_CTRLB_DMAEN_Pos)
#define RTC_MODE0_CTRLB_DEBF_Pos    8            /**< \brief (RTC_MODE0_CTRLB) Debounce Freqnuency */
#define RTC_MODE0_CTRLB_DEBF_Msk    (_U_(0x7) << RTC_MODE0_CTRLB_DEBF_Pos)
#define RTC_MODE0_CTRLB_DEBF(value) (RTC_MODE0_CTRLB_DEBF_Msk & ((value) << RTC_MODE0_CTRLB_DEBF_Pos))
#define   RTC_MODE0_CTRLB_DEBF_DIV2_Val   _U_(0x0)   /**< \brief (RTC_MODE0_CTRLB) CLK_RTC_DEB = CLK_RTC/2 */
#define   RTC_MODE0_CTRLB_DEBF_DIV4_Val   _U_(0x1)   /**< \brief (RTC_MODE0_CTRLB) CLK_RTC_DEB = CLK_RTC/4 */
#define   RTC_MODE0_CTRLB_DEBF_DIV8_Val   _U_(0x2)   /**< \brief (RTC_MODE0_CTRLB) CLK_RTC_DEB = CLK_RTC/8 */
#define   RTC_MODE0_CTRLB_DEBF_DIV16_Val  _U_(0x3)   /**< \brief (RTC_MODE0_CTRLB) CLK_RTC_DEB = CLK_RTC/16 */
#define   RTC_MODE0_CTRLB_DEBF_DIV32_Val  _U_(0x4)   /**< \brief (RTC_MODE0_CTRLB) CLK_RTC_DEB = CLK_RTC/32 */
#define   RTC_MODE0_CTRLB_DEBF_DIV64_Val  _U_(0x5)   /**< \brief (RTC_MODE0_CTRLB) CLK_RTC_DEB = CLK_RTC/64 */
#define   RTC_MODE0_CTRLB_DEBF_DIV128_Val _U_(0x6)   /**< \brief (RTC_MODE0_CTRLB) CLK_RTC_DEB = CLK_RTC/128 */
#define   RTC_MODE0_CTRLB_DEBF_DIV256_Val _U_(0x7)   /**< \brief (RTC_MODE0_CTRLB) CLK_RTC_DEB = CLK_RTC/256 */
#define RTC_MODE0_CTRLB_DEBF_DIV2   (RTC_MODE0_CTRLB_DEBF_DIV2_Val << RTC_MODE0_CTRLB_DEBF_Pos)
#define RTC_MODE0_CTRLB_DEBF_DIV4   (RTC_MODE0_CTRLB_DEBF_DIV4_Val << RTC_MODE0_CTRLB_DEBF_Pos)
#define RTC_MODE0_CTRLB_DEBF_DIV8   (RTC_MODE0_CTRLB_DEBF_DIV8_Val << RTC_MODE0_CTRLB_DEBF_Pos)
#define RTC_MODE0_CTRLB_DEBF_DIV16  (RTC_MODE0_CTRLB_DEBF_DIV16_Val << RTC_MODE0_CTRLB_DEBF_Pos)
#define RTC_MODE0_CTRLB_DEBF_DIV32  (RTC_MODE0_CTRLB_DEBF_DIV32_Val << RTC_MODE0_CTRLB_DEBF_Pos)
#define RTC_MODE0_CTRLB_DEBF_DIV64  (RTC_MODE0_CTRLB_DEBF_DIV64_Val << RTC_MODE0_CTRLB_DEBF_Pos)
#define RTC_MODE0_CTRLB_DEBF_DIV128 (RTC_MODE0_CTRLB_DEBF_DIV128_Val << RTC_MODE0_CTRLB_DEBF_Pos)
#define RTC_MODE0_CTRLB_DEBF_DIV256 (RTC_MODE0_CTRLB_DEBF_DIV256_Val << RTC_MODE0_CTRLB_DEBF_Pos)
#define RTC_MODE0_CTRLB_ACTF_Pos    12           /**< \brief (RTC_MODE0_CTRLB) Active Layer Freqnuency */
#define RTC_MODE0_CTRLB_ACTF_Msk    (_U_(0x7) << RTC_MODE0_CTRLB_ACTF_Pos)
#define RTC_MODE0_CTRLB_ACTF(value) (RTC_MODE0_CTRLB_ACTF_Msk & ((value) << RTC_MODE0_CTRLB_ACTF_Pos))
#define   RTC_MODE0_CTRLB_ACTF_DIV2_Val   _U_(0x0)   /**< \brief (RTC_MODE0_CTRLB) CLK_RTC_OUT = CLK_RTC/2 */
#define   RTC_MODE0_CTRLB_ACTF_DIV4_Val   _U_(0x1)   /**< \brief (RTC_MODE0_CTRLB) CLK_RTC_OUT = CLK_RTC/4 */
#define   RTC_MODE0_CTRLB_ACTF_DIV8_Val   _U_(0x2)   /**< \brief (RTC_MODE0_CTRLB) CLK_RTC_OUT = CLK_RTC/8 */
#define   RTC_MODE0_CTRLB_ACTF_DIV16_Val  _U_(0x3)   /**< \brief (RTC_MODE0_CTRLB) CLK_RTC_OUT = CLK_RTC/16 */
#define   RTC_MODE0_CTRLB_ACTF_DIV32_Val  _U_(0x4)   /**< \brief (RTC_MODE0_CTRLB) CLK_RTC_OUT = CLK_RTC/32 */
#define   RTC_MODE0_CTRLB_ACTF_DIV64_Val  _U_(0x5)   /**< \brief (RTC_MODE0_CTRLB) CLK_RTC_OUT = CLK_RTC/64 */
#define   RTC_MODE0_CTRLB_ACTF_DIV128_Val _U_(0x6)   /**< \brief (RTC_MODE0_CTRLB) CLK_RTC_OUT = CLK_RTC/128 */
#define   RTC_MODE0_CTRLB_ACTF_DIV256_Val _U_(0x7)   /**< \brief (RTC_MODE0_CTRLB) CLK_RTC_OUT = CLK_RTC/256 */
#define RTC_MODE0_CTRLB_ACTF_DIV2   (RTC_MODE0_CTRLB_ACTF_DIV2_Val << RTC_MODE0_CTRLB_ACTF_Pos)
#define RTC_MODE0_CTRLB_ACTF_DIV4   (RTC_MODE0_CTRLB_ACTF_DIV4_Val << RTC_MODE0_CTRLB_ACTF_Pos)
#define RTC_MODE0_CTRLB_ACTF_DIV8   (RTC_MODE0_CTRLB_ACTF_DIV8_Val << RTC_MODE0_CTRLB_ACTF_Pos)
#define RTC_MODE0_CTRLB_ACTF_DIV16  (RTC_MODE0_CTRLB_ACTF_DIV16_Val << RTC_MODE0_CTRLB_ACTF_Pos)
#define RTC_MODE0_CTRLB_ACTF_DIV32  (RTC_MODE0_CTRLB_ACTF_DIV32_Val << RTC_MODE0_CTRLB_ACTF_Pos)
#define RTC_MODE0_CTRLB_ACTF_DIV64  (RTC_MODE0_CTRLB_ACTF_DIV64_Val << RTC_MODE0_CTRLB_ACTF_Pos)
#define RTC_MODE0_CTRLB_ACTF_DIV128 (RTC_MODE0_CTRLB_ACTF_DIV128_Val << RTC_MODE0_CTRLB_ACTF_Pos)
#define RTC_MODE0_CTRLB_ACTF_DIV256 (RTC_MODE0_CTRLB_ACTF_DIV256_Val << RTC_MODE0_CTRLB_ACTF_Pos)
#define RTC_MODE0_CTRLB_MASK        _U_(0x77F3)  /**< \brief (RTC_MODE0_CTRLB) MASK Register */

/* -------- RTC_MODE1_CTRLB : (RTC Offset: 0x02) (R/W 16) MODE1 MODE1 Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t GP0EN:1;          /*!< bit:      0  General Purpose 0 Enable           */
    uint16_t GP2EN:1;          /*!< bit:      1  General Purpose 2 Enable           */
    uint16_t :2;               /*!< bit:  2.. 3  Reserved                           */
    uint16_t DEBMAJ:1;         /*!< bit:      4  Debouncer Majority Enable          */
    uint16_t DEBASYNC:1;       /*!< bit:      5  Debouncer Asynchronous Enable      */
    uint16_t RTCOUT:1;         /*!< bit:      6  RTC Output Enable                  */
    uint16_t DMAEN:1;          /*!< bit:      7  DMA Enable                         */
    uint16_t DEBF:3;           /*!< bit:  8..10  Debounce Freqnuency                */
    uint16_t :1;               /*!< bit:     11  Reserved                           */
    uint16_t ACTF:3;           /*!< bit: 12..14  Active Layer Freqnuency            */
    uint16_t :1;               /*!< bit:     15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_CTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_CTRLB_OFFSET      0x02         /**< \brief (RTC_MODE1_CTRLB offset) MODE1 Control B */
#define RTC_MODE1_CTRLB_RESETVALUE  _U_(0x0000)  /**< \brief (RTC_MODE1_CTRLB reset_value) MODE1 Control B */

#define RTC_MODE1_CTRLB_GP0EN_Pos   0            /**< \brief (RTC_MODE1_CTRLB) General Purpose 0 Enable */
#define RTC_MODE1_CTRLB_GP0EN       (_U_(0x1) << RTC_MODE1_CTRLB_GP0EN_Pos)
#define RTC_MODE1_CTRLB_GP2EN_Pos   1            /**< \brief (RTC_MODE1_CTRLB) General Purpose 2 Enable */
#define RTC_MODE1_CTRLB_GP2EN       (_U_(0x1) << RTC_MODE1_CTRLB_GP2EN_Pos)
#define RTC_MODE1_CTRLB_DEBMAJ_Pos  4            /**< \brief (RTC_MODE1_CTRLB) Debouncer Majority Enable */
#define RTC_MODE1_CTRLB_DEBMAJ      (_U_(0x1) << RTC_MODE1_CTRLB_DEBMAJ_Pos)
#define RTC_MODE1_CTRLB_DEBASYNC_Pos 5            /**< \brief (RTC_MODE1_CTRLB) Debouncer Asynchronous Enable */
#define RTC_MODE1_CTRLB_DEBASYNC    (_U_(0x1) << RTC_MODE1_CTRLB_DEBASYNC_Pos)
#define RTC_MODE1_CTRLB_RTCOUT_Pos  6            /**< \brief (RTC_MODE1_CTRLB) RTC Output Enable */
#define RTC_MODE1_CTRLB_RTCOUT      (_U_(0x1) << RTC_MODE1_CTRLB_RTCOUT_Pos)
#define RTC_MODE1_CTRLB_DMAEN_Pos   7            /**< \brief (RTC_MODE1_CTRLB) DMA Enable */
#define RTC_MODE1_CTRLB_DMAEN       (_U_(0x1) << RTC_MODE1_CTRLB_DMAEN_Pos)
#define RTC_MODE1_CTRLB_DEBF_Pos    8            /**< \brief (RTC_MODE1_CTRLB) Debounce Freqnuency */
#define RTC_MODE1_CTRLB_DEBF_Msk    (_U_(0x7) << RTC_MODE1_CTRLB_DEBF_Pos)
#define RTC_MODE1_CTRLB_DEBF(value) (RTC_MODE1_CTRLB_DEBF_Msk & ((value) << RTC_MODE1_CTRLB_DEBF_Pos))
#define   RTC_MODE1_CTRLB_DEBF_DIV2_Val   _U_(0x0)   /**< \brief (RTC_MODE1_CTRLB) CLK_RTC_DEB = CLK_RTC/2 */
#define   RTC_MODE1_CTRLB_DEBF_DIV4_Val   _U_(0x1)   /**< \brief (RTC_MODE1_CTRLB) CLK_RTC_DEB = CLK_RTC/4 */
#define   RTC_MODE1_CTRLB_DEBF_DIV8_Val   _U_(0x2)   /**< \brief (RTC_MODE1_CTRLB) CLK_RTC_DEB = CLK_RTC/8 */
#define   RTC_MODE1_CTRLB_DEBF_DIV16_Val  _U_(0x3)   /**< \brief (RTC_MODE1_CTRLB) CLK_RTC_DEB = CLK_RTC/16 */
#define   RTC_MODE1_CTRLB_DEBF_DIV32_Val  _U_(0x4)   /**< \brief (RTC_MODE1_CTRLB) CLK_RTC_DEB = CLK_RTC/32 */
#define   RTC_MODE1_CTRLB_DEBF_DIV64_Val  _U_(0x5)   /**< \brief (RTC_MODE1_CTRLB) CLK_RTC_DEB = CLK_RTC/64 */
#define   RTC_MODE1_CTRLB_DEBF_DIV128_Val _U_(0x6)   /**< \brief (RTC_MODE1_CTRLB) CLK_RTC_DEB = CLK_RTC/128 */
#define   RTC_MODE1_CTRLB_DEBF_DIV256_Val _U_(0x7)   /**< \brief (RTC_MODE1_CTRLB) CLK_RTC_DEB = CLK_RTC/256 */
#define RTC_MODE1_CTRLB_DEBF_DIV2   (RTC_MODE1_CTRLB_DEBF_DIV2_Val << RTC_MODE1_CTRLB_DEBF_Pos)
#define RTC_MODE1_CTRLB_DEBF_DIV4   (RTC_MODE1_CTRLB_DEBF_DIV4_Val << RTC_MODE1_CTRLB_DEBF_Pos)
#define RTC_MODE1_CTRLB_DEBF_DIV8   (RTC_MODE1_CTRLB_DEBF_DIV8_Val << RTC_MODE1_CTRLB_DEBF_Pos)
#define RTC_MODE1_CTRLB_DEBF_DIV16  (RTC_MODE1_CTRLB_DEBF_DIV16_Val << RTC_MODE1_CTRLB_DEBF_Pos)
#define RTC_MODE1_CTRLB_DEBF_DIV32  (RTC_MODE1_CTRLB_DEBF_DIV32_Val << RTC_MODE1_CTRLB_DEBF_Pos)
#define RTC_MODE1_CTRLB_DEBF_DIV64  (RTC_MODE1_CTRLB_DEBF_DIV64_Val << RTC_MODE1_CTRLB_DEBF_Pos)
#define RTC_MODE1_CTRLB_DEBF_DIV128 (RTC_MODE1_CTRLB_DEBF_DIV128_Val << RTC_MODE1_CTRLB_DEBF_Pos)
#define RTC_MODE1_CTRLB_DEBF_DIV256 (RTC_MODE1_CTRLB_DEBF_DIV256_Val << RTC_MODE1_CTRLB_DEBF_Pos)
#define RTC_MODE1_CTRLB_ACTF_Pos    12           /**< \brief (RTC_MODE1_CTRLB) Active Layer Freqnuency */
#define RTC_MODE1_CTRLB_ACTF_Msk    (_U_(0x7) << RTC_MODE1_CTRLB_ACTF_Pos)
#define RTC_MODE1_CTRLB_ACTF(value) (RTC_MODE1_CTRLB_ACTF_Msk & ((value) << RTC_MODE1_CTRLB_ACTF_Pos))
#define   RTC_MODE1_CTRLB_ACTF_DIV2_Val   _U_(0x0)   /**< \brief (RTC_MODE1_CTRLB) CLK_RTC_OUT = CLK_RTC/2 */
#define   RTC_MODE1_CTRLB_ACTF_DIV4_Val   _U_(0x1)   /**< \brief (RTC_MODE1_CTRLB) CLK_RTC_OUT = CLK_RTC/4 */
#define   RTC_MODE1_CTRLB_ACTF_DIV8_Val   _U_(0x2)   /**< \brief (RTC_MODE1_CTRLB) CLK_RTC_OUT = CLK_RTC/8 */
#define   RTC_MODE1_CTRLB_ACTF_DIV16_Val  _U_(0x3)   /**< \brief (RTC_MODE1_CTRLB) CLK_RTC_OUT = CLK_RTC/16 */
#define   RTC_MODE1_CTRLB_ACTF_DIV32_Val  _U_(0x4)   /**< \brief (RTC_MODE1_CTRLB) CLK_RTC_OUT = CLK_RTC/32 */
#define   RTC_MODE1_CTRLB_ACTF_DIV64_Val  _U_(0x5)   /**< \brief (RTC_MODE1_CTRLB) CLK_RTC_OUT = CLK_RTC/64 */
#define   RTC_MODE1_CTRLB_ACTF_DIV128_Val _U_(0x6)   /**< \brief (RTC_MODE1_CTRLB) CLK_RTC_OUT = CLK_RTC/128 */
#define   RTC_MODE1_CTRLB_ACTF_DIV256_Val _U_(0x7)   /**< \brief (RTC_MODE1_CTRLB) CLK_RTC_OUT = CLK_RTC/256 */
#define RTC_MODE1_CTRLB_ACTF_DIV2   (RTC_MODE1_CTRLB_ACTF_DIV2_Val << RTC_MODE1_CTRLB_ACTF_Pos)
#define RTC_MODE1_CTRLB_ACTF_DIV4   (RTC_MODE1_CTRLB_ACTF_DIV4_Val << RTC_MODE1_CTRLB_ACTF_Pos)
#define RTC_MODE1_CTRLB_ACTF_DIV8   (RTC_MODE1_CTRLB_ACTF_DIV8_Val << RTC_MODE1_CTRLB_ACTF_Pos)
#define RTC_MODE1_CTRLB_ACTF_DIV16  (RTC_MODE1_CTRLB_ACTF_DIV16_Val << RTC_MODE1_CTRLB_ACTF_Pos)
#define RTC_MODE1_CTRLB_ACTF_DIV32  (RTC_MODE1_CTRLB_ACTF_DIV32_Val << RTC_MODE1_CTRLB_ACTF_Pos)
#define RTC_MODE1_CTRLB_ACTF_DIV64  (RTC_MODE1_CTRLB_ACTF_DIV64_Val << RTC_MODE1_CTRLB_ACTF_Pos)
#define RTC_MODE1_CTRLB_ACTF_DIV128 (RTC_MODE1_CTRLB_ACTF_DIV128_Val << RTC_MODE1_CTRLB_ACTF_Pos)
#define RTC_MODE1_CTRLB_ACTF_DIV256 (RTC_MODE1_CTRLB_ACTF_DIV256_Val << RTC_MODE1_CTRLB_ACTF_Pos)
#define RTC_MODE1_CTRLB_MASK        _U_(0x77F3)  /**< \brief (RTC_MODE1_CTRLB) MASK Register */

/* -------- RTC_MODE2_CTRLB : (RTC Offset: 0x02) (R/W 16) MODE2 MODE2 Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t GP0EN:1;          /*!< bit:      0  General Purpose 0 Enable           */
    uint16_t GP2EN:1;          /*!< bit:      1  General Purpose 2 Enable           */
    uint16_t :2;               /*!< bit:  2.. 3  Reserved                           */
    uint16_t DEBMAJ:1;         /*!< bit:      4  Debouncer Majority Enable          */
    uint16_t DEBASYNC:1;       /*!< bit:      5  Debouncer Asynchronous Enable      */
    uint16_t RTCOUT:1;         /*!< bit:      6  RTC Output Enable                  */
    uint16_t DMAEN:1;          /*!< bit:      7  DMA Enable                         */
    uint16_t DEBF:3;           /*!< bit:  8..10  Debounce Freqnuency                */
    uint16_t :1;               /*!< bit:     11  Reserved                           */
    uint16_t ACTF:3;           /*!< bit: 12..14  Active Layer Freqnuency            */
    uint16_t :1;               /*!< bit:     15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_CTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_CTRLB_OFFSET      0x02         /**< \brief (RTC_MODE2_CTRLB offset) MODE2 Control B */
#define RTC_MODE2_CTRLB_RESETVALUE  _U_(0x0000)  /**< \brief (RTC_MODE2_CTRLB reset_value) MODE2 Control B */

#define RTC_MODE2_CTRLB_GP0EN_Pos   0            /**< \brief (RTC_MODE2_CTRLB) General Purpose 0 Enable */
#define RTC_MODE2_CTRLB_GP0EN       (_U_(0x1) << RTC_MODE2_CTRLB_GP0EN_Pos)
#define RTC_MODE2_CTRLB_GP2EN_Pos   1            /**< \brief (RTC_MODE2_CTRLB) General Purpose 2 Enable */
#define RTC_MODE2_CTRLB_GP2EN       (_U_(0x1) << RTC_MODE2_CTRLB_GP2EN_Pos)
#define RTC_MODE2_CTRLB_DEBMAJ_Pos  4            /**< \brief (RTC_MODE2_CTRLB) Debouncer Majority Enable */
#define RTC_MODE2_CTRLB_DEBMAJ      (_U_(0x1) << RTC_MODE2_CTRLB_DEBMAJ_Pos)
#define RTC_MODE2_CTRLB_DEBASYNC_Pos 5            /**< \brief (RTC_MODE2_CTRLB) Debouncer Asynchronous Enable */
#define RTC_MODE2_CTRLB_DEBASYNC    (_U_(0x1) << RTC_MODE2_CTRLB_DEBASYNC_Pos)
#define RTC_MODE2_CTRLB_RTCOUT_Pos  6            /**< \brief (RTC_MODE2_CTRLB) RTC Output Enable */
#define RTC_MODE2_CTRLB_RTCOUT      (_U_(0x1) << RTC_MODE2_CTRLB_RTCOUT_Pos)
#define RTC_MODE2_CTRLB_DMAEN_Pos   7            /**< \brief (RTC_MODE2_CTRLB) DMA Enable */
#define RTC_MODE2_CTRLB_DMAEN       (_U_(0x1) << RTC_MODE2_CTRLB_DMAEN_Pos)
#define RTC_MODE2_CTRLB_DEBF_Pos    8            /**< \brief (RTC_MODE2_CTRLB) Debounce Freqnuency */
#define RTC_MODE2_CTRLB_DEBF_Msk    (_U_(0x7) << RTC_MODE2_CTRLB_DEBF_Pos)
#define RTC_MODE2_CTRLB_DEBF(value) (RTC_MODE2_CTRLB_DEBF_Msk & ((value) << RTC_MODE2_CTRLB_DEBF_Pos))
#define   RTC_MODE2_CTRLB_DEBF_DIV2_Val   _U_(0x0)   /**< \brief (RTC_MODE2_CTRLB) CLK_RTC_DEB = CLK_RTC/2 */
#define   RTC_MODE2_CTRLB_DEBF_DIV4_Val   _U_(0x1)   /**< \brief (RTC_MODE2_CTRLB) CLK_RTC_DEB = CLK_RTC/4 */
#define   RTC_MODE2_CTRLB_DEBF_DIV8_Val   _U_(0x2)   /**< \brief (RTC_MODE2_CTRLB) CLK_RTC_DEB = CLK_RTC/8 */
#define   RTC_MODE2_CTRLB_DEBF_DIV16_Val  _U_(0x3)   /**< \brief (RTC_MODE2_CTRLB) CLK_RTC_DEB = CLK_RTC/16 */
#define   RTC_MODE2_CTRLB_DEBF_DIV32_Val  _U_(0x4)   /**< \brief (RTC_MODE2_CTRLB) CLK_RTC_DEB = CLK_RTC/32 */
#define   RTC_MODE2_CTRLB_DEBF_DIV64_Val  _U_(0x5)   /**< \brief (RTC_MODE2_CTRLB) CLK_RTC_DEB = CLK_RTC/64 */
#define   RTC_MODE2_CTRLB_DEBF_DIV128_Val _U_(0x6)   /**< \brief (RTC_MODE2_CTRLB) CLK_RTC_DEB = CLK_RTC/128 */
#define   RTC_MODE2_CTRLB_DEBF_DIV256_Val _U_(0x7)   /**< \brief (RTC_MODE2_CTRLB) CLK_RTC_DEB = CLK_RTC/256 */
#define RTC_MODE2_CTRLB_DEBF_DIV2   (RTC_MODE2_CTRLB_DEBF_DIV2_Val << RTC_MODE2_CTRLB_DEBF_Pos)
#define RTC_MODE2_CTRLB_DEBF_DIV4   (RTC_MODE2_CTRLB_DEBF_DIV4_Val << RTC_MODE2_CTRLB_DEBF_Pos)
#define RTC_MODE2_CTRLB_DEBF_DIV8   (RTC_MODE2_CTRLB_DEBF_DIV8_Val << RTC_MODE2_CTRLB_DEBF_Pos)
#define RTC_MODE2_CTRLB_DEBF_DIV16  (RTC_MODE2_CTRLB_DEBF_DIV16_Val << RTC_MODE2_CTRLB_DEBF_Pos)
#define RTC_MODE2_CTRLB_DEBF_DIV32  (RTC_MODE2_CTRLB_DEBF_DIV32_Val << RTC_MODE2_CTRLB_DEBF_Pos)
#define RTC_MODE2_CTRLB_DEBF_DIV64  (RTC_MODE2_CTRLB_DEBF_DIV64_Val << RTC_MODE2_CTRLB_DEBF_Pos)
#define RTC_MODE2_CTRLB_DEBF_DIV128 (RTC_MODE2_CTRLB_DEBF_DIV128_Val << RTC_MODE2_CTRLB_DEBF_Pos)
#define RTC_MODE2_CTRLB_DEBF_DIV256 (RTC_MODE2_CTRLB_DEBF_DIV256_Val << RTC_MODE2_CTRLB_DEBF_Pos)
#define RTC_MODE2_CTRLB_ACTF_Pos    12           /**< \brief (RTC_MODE2_CTRLB) Active Layer Freqnuency */
#define RTC_MODE2_CTRLB_ACTF_Msk    (_U_(0x7) << RTC_MODE2_CTRLB_ACTF_Pos)
#define RTC_MODE2_CTRLB_ACTF(value) (RTC_MODE2_CTRLB_ACTF_Msk & ((value) << RTC_MODE2_CTRLB_ACTF_Pos))
#define   RTC_MODE2_CTRLB_ACTF_DIV2_Val   _U_(0x0)   /**< \brief (RTC_MODE2_CTRLB) CLK_RTC_OUT = CLK_RTC/2 */
#define   RTC_MODE2_CTRLB_ACTF_DIV4_Val   _U_(0x1)   /**< \brief (RTC_MODE2_CTRLB) CLK_RTC_OUT = CLK_RTC/4 */
#define   RTC_MODE2_CTRLB_ACTF_DIV8_Val   _U_(0x2)   /**< \brief (RTC_MODE2_CTRLB) CLK_RTC_OUT = CLK_RTC/8 */
#define   RTC_MODE2_CTRLB_ACTF_DIV16_Val  _U_(0x3)   /**< \brief (RTC_MODE2_CTRLB) CLK_RTC_OUT = CLK_RTC/16 */
#define   RTC_MODE2_CTRLB_ACTF_DIV32_Val  _U_(0x4)   /**< \brief (RTC_MODE2_CTRLB) CLK_RTC_OUT = CLK_RTC/32 */
#define   RTC_MODE2_CTRLB_ACTF_DIV64_Val  _U_(0x5)   /**< \brief (RTC_MODE2_CTRLB) CLK_RTC_OUT = CLK_RTC/64 */
#define   RTC_MODE2_CTRLB_ACTF_DIV128_Val _U_(0x6)   /**< \brief (RTC_MODE2_CTRLB) CLK_RTC_OUT = CLK_RTC/128 */
#define   RTC_MODE2_CTRLB_ACTF_DIV256_Val _U_(0x7)   /**< \brief (RTC_MODE2_CTRLB) CLK_RTC_OUT = CLK_RTC/256 */
#define RTC_MODE2_CTRLB_ACTF_DIV2   (RTC_MODE2_CTRLB_ACTF_DIV2_Val << RTC_MODE2_CTRLB_ACTF_Pos)
#define RTC_MODE2_CTRLB_ACTF_DIV4   (RTC_MODE2_CTRLB_ACTF_DIV4_Val << RTC_MODE2_CTRLB_ACTF_Pos)
#define RTC_MODE2_CTRLB_ACTF_DIV8   (RTC_MODE2_CTRLB_ACTF_DIV8_Val << RTC_MODE2_CTRLB_ACTF_Pos)
#define RTC_MODE2_CTRLB_ACTF_DIV16  (RTC_MODE2_CTRLB_ACTF_DIV16_Val << RTC_MODE2_CTRLB_ACTF_Pos)
#define RTC_MODE2_CTRLB_ACTF_DIV32  (RTC_MODE2_CTRLB_ACTF_DIV32_Val << RTC_MODE2_CTRLB_ACTF_Pos)
#define RTC_MODE2_CTRLB_ACTF_DIV64  (RTC_MODE2_CTRLB_ACTF_DIV64_Val << RTC_MODE2_CTRLB_ACTF_Pos)
#define RTC_MODE2_CTRLB_ACTF_DIV128 (RTC_MODE2_CTRLB_ACTF_DIV128_Val << RTC_MODE2_CTRLB_ACTF_Pos)
#define RTC_MODE2_CTRLB_ACTF_DIV256 (RTC_MODE2_CTRLB_ACTF_DIV256_Val << RTC_MODE2_CTRLB_ACTF_Pos)
#define RTC_MODE2_CTRLB_MASK        _U_(0x77F3)  /**< \brief (RTC_MODE2_CTRLB) MASK Register */

/* -------- RTC_MODE0_EVCTRL : (RTC Offset: 0x04) (R/W 32) MODE0 MODE0 Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PEREO0:1;         /*!< bit:      0  Periodic Interval 0 Event Output Enable */
    uint32_t PEREO1:1;         /*!< bit:      1  Periodic Interval 1 Event Output Enable */
    uint32_t PEREO2:1;         /*!< bit:      2  Periodic Interval 2 Event Output Enable */
    uint32_t PEREO3:1;         /*!< bit:      3  Periodic Interval 3 Event Output Enable */
    uint32_t PEREO4:1;         /*!< bit:      4  Periodic Interval 4 Event Output Enable */
    uint32_t PEREO5:1;         /*!< bit:      5  Periodic Interval 5 Event Output Enable */
    uint32_t PEREO6:1;         /*!< bit:      6  Periodic Interval 6 Event Output Enable */
    uint32_t PEREO7:1;         /*!< bit:      7  Periodic Interval 7 Event Output Enable */
    uint32_t CMPEO0:1;         /*!< bit:      8  Compare 0 Event Output Enable      */
    uint32_t CMPEO1:1;         /*!< bit:      9  Compare 1 Event Output Enable      */
    uint32_t :4;               /*!< bit: 10..13  Reserved                           */
    uint32_t TAMPEREO:1;       /*!< bit:     14  Tamper Event Output Enable         */
    uint32_t OVFEO:1;          /*!< bit:     15  Overflow Event Output Enable       */
    uint32_t TAMPEVEI:1;       /*!< bit:     16  Tamper Event Input Enable          */
    uint32_t :15;              /*!< bit: 17..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t PEREO:8;          /*!< bit:  0.. 7  Periodic Interval x Event Output Enable */
    uint32_t CMPEO:2;          /*!< bit:  8.. 9  Compare x Event Output Enable      */
    uint32_t :22;              /*!< bit: 10..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_EVCTRL_OFFSET     0x04         /**< \brief (RTC_MODE0_EVCTRL offset) MODE0 Event Control */
#define RTC_MODE0_EVCTRL_RESETVALUE _U_(0x00000000) /**< \brief (RTC_MODE0_EVCTRL reset_value) MODE0 Event Control */

#define RTC_MODE0_EVCTRL_PEREO0_Pos 0            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval 0 Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO0     (_U_(1) << RTC_MODE0_EVCTRL_PEREO0_Pos)
#define RTC_MODE0_EVCTRL_PEREO1_Pos 1            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval 1 Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO1     (_U_(1) << RTC_MODE0_EVCTRL_PEREO1_Pos)
#define RTC_MODE0_EVCTRL_PEREO2_Pos 2            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval 2 Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO2     (_U_(1) << RTC_MODE0_EVCTRL_PEREO2_Pos)
#define RTC_MODE0_EVCTRL_PEREO3_Pos 3            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval 3 Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO3     (_U_(1) << RTC_MODE0_EVCTRL_PEREO3_Pos)
#define RTC_MODE0_EVCTRL_PEREO4_Pos 4            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval 4 Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO4     (_U_(1) << RTC_MODE0_EVCTRL_PEREO4_Pos)
#define RTC_MODE0_EVCTRL_PEREO5_Pos 5            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval 5 Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO5     (_U_(1) << RTC_MODE0_EVCTRL_PEREO5_Pos)
#define RTC_MODE0_EVCTRL_PEREO6_Pos 6            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval 6 Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO6     (_U_(1) << RTC_MODE0_EVCTRL_PEREO6_Pos)
#define RTC_MODE0_EVCTRL_PEREO7_Pos 7            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval 7 Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO7     (_U_(1) << RTC_MODE0_EVCTRL_PEREO7_Pos)
#define RTC_MODE0_EVCTRL_PEREO_Pos  0            /**< \brief (RTC_MODE0_EVCTRL) Periodic Interval x Event Output Enable */
#define RTC_MODE0_EVCTRL_PEREO_Msk  (_U_(0xFF) << RTC_MODE0_EVCTRL_PEREO_Pos)
#define RTC_MODE0_EVCTRL_PEREO(value) (RTC_MODE0_EVCTRL_PEREO_Msk & ((value) << RTC_MODE0_EVCTRL_PEREO_Pos))
#define RTC_MODE0_EVCTRL_CMPEO0_Pos 8            /**< \brief (RTC_MODE0_EVCTRL) Compare 0 Event Output Enable */
#define RTC_MODE0_EVCTRL_CMPEO0     (_U_(1) << RTC_MODE0_EVCTRL_CMPEO0_Pos)
#define RTC_MODE0_EVCTRL_CMPEO1_Pos 9            /**< \brief (RTC_MODE0_EVCTRL) Compare 1 Event Output Enable */
#define RTC_MODE0_EVCTRL_CMPEO1     (_U_(1) << RTC_MODE0_EVCTRL_CMPEO1_Pos)
#define RTC_MODE0_EVCTRL_CMPEO_Pos  8            /**< \brief (RTC_MODE0_EVCTRL) Compare x Event Output Enable */
#define RTC_MODE0_EVCTRL_CMPEO_Msk  (_U_(0x3) << RTC_MODE0_EVCTRL_CMPEO_Pos)
#define RTC_MODE0_EVCTRL_CMPEO(value) (RTC_MODE0_EVCTRL_CMPEO_Msk & ((value) << RTC_MODE0_EVCTRL_CMPEO_Pos))
#define RTC_MODE0_EVCTRL_TAMPEREO_Pos 14           /**< \brief (RTC_MODE0_EVCTRL) Tamper Event Output Enable */
#define RTC_MODE0_EVCTRL_TAMPEREO   (_U_(0x1) << RTC_MODE0_EVCTRL_TAMPEREO_Pos)
#define RTC_MODE0_EVCTRL_OVFEO_Pos  15           /**< \brief (RTC_MODE0_EVCTRL) Overflow Event Output Enable */
#define RTC_MODE0_EVCTRL_OVFEO      (_U_(0x1) << RTC_MODE0_EVCTRL_OVFEO_Pos)
#define RTC_MODE0_EVCTRL_TAMPEVEI_Pos 16           /**< \brief (RTC_MODE0_EVCTRL) Tamper Event Input Enable */
#define RTC_MODE0_EVCTRL_TAMPEVEI   (_U_(0x1) << RTC_MODE0_EVCTRL_TAMPEVEI_Pos)
#define RTC_MODE0_EVCTRL_MASK       _U_(0x0001C3FF) /**< \brief (RTC_MODE0_EVCTRL) MASK Register */

/* -------- RTC_MODE1_EVCTRL : (RTC Offset: 0x04) (R/W 32) MODE1 MODE1 Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PEREO0:1;         /*!< bit:      0  Periodic Interval 0 Event Output Enable */
    uint32_t PEREO1:1;         /*!< bit:      1  Periodic Interval 1 Event Output Enable */
    uint32_t PEREO2:1;         /*!< bit:      2  Periodic Interval 2 Event Output Enable */
    uint32_t PEREO3:1;         /*!< bit:      3  Periodic Interval 3 Event Output Enable */
    uint32_t PEREO4:1;         /*!< bit:      4  Periodic Interval 4 Event Output Enable */
    uint32_t PEREO5:1;         /*!< bit:      5  Periodic Interval 5 Event Output Enable */
    uint32_t PEREO6:1;         /*!< bit:      6  Periodic Interval 6 Event Output Enable */
    uint32_t PEREO7:1;         /*!< bit:      7  Periodic Interval 7 Event Output Enable */
    uint32_t CMPEO0:1;         /*!< bit:      8  Compare 0 Event Output Enable      */
    uint32_t CMPEO1:1;         /*!< bit:      9  Compare 1 Event Output Enable      */
    uint32_t CMPEO2:1;         /*!< bit:     10  Compare 2 Event Output Enable      */
    uint32_t CMPEO3:1;         /*!< bit:     11  Compare 3 Event Output Enable      */
    uint32_t :2;               /*!< bit: 12..13  Reserved                           */
    uint32_t TAMPEREO:1;       /*!< bit:     14  Tamper Event Output Enable         */
    uint32_t OVFEO:1;          /*!< bit:     15  Overflow Event Output Enable       */
    uint32_t TAMPEVEI:1;       /*!< bit:     16  Tamper Event Input Enable          */
    uint32_t :15;              /*!< bit: 17..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t PEREO:8;          /*!< bit:  0.. 7  Periodic Interval x Event Output Enable */
    uint32_t CMPEO:4;          /*!< bit:  8..11  Compare x Event Output Enable      */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_EVCTRL_OFFSET     0x04         /**< \brief (RTC_MODE1_EVCTRL offset) MODE1 Event Control */
#define RTC_MODE1_EVCTRL_RESETVALUE _U_(0x00000000) /**< \brief (RTC_MODE1_EVCTRL reset_value) MODE1 Event Control */

#define RTC_MODE1_EVCTRL_PEREO0_Pos 0            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval 0 Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO0     (_U_(1) << RTC_MODE1_EVCTRL_PEREO0_Pos)
#define RTC_MODE1_EVCTRL_PEREO1_Pos 1            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval 1 Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO1     (_U_(1) << RTC_MODE1_EVCTRL_PEREO1_Pos)
#define RTC_MODE1_EVCTRL_PEREO2_Pos 2            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval 2 Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO2     (_U_(1) << RTC_MODE1_EVCTRL_PEREO2_Pos)
#define RTC_MODE1_EVCTRL_PEREO3_Pos 3            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval 3 Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO3     (_U_(1) << RTC_MODE1_EVCTRL_PEREO3_Pos)
#define RTC_MODE1_EVCTRL_PEREO4_Pos 4            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval 4 Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO4     (_U_(1) << RTC_MODE1_EVCTRL_PEREO4_Pos)
#define RTC_MODE1_EVCTRL_PEREO5_Pos 5            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval 5 Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO5     (_U_(1) << RTC_MODE1_EVCTRL_PEREO5_Pos)
#define RTC_MODE1_EVCTRL_PEREO6_Pos 6            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval 6 Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO6     (_U_(1) << RTC_MODE1_EVCTRL_PEREO6_Pos)
#define RTC_MODE1_EVCTRL_PEREO7_Pos 7            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval 7 Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO7     (_U_(1) << RTC_MODE1_EVCTRL_PEREO7_Pos)
#define RTC_MODE1_EVCTRL_PEREO_Pos  0            /**< \brief (RTC_MODE1_EVCTRL) Periodic Interval x Event Output Enable */
#define RTC_MODE1_EVCTRL_PEREO_Msk  (_U_(0xFF) << RTC_MODE1_EVCTRL_PEREO_Pos)
#define RTC_MODE1_EVCTRL_PEREO(value) (RTC_MODE1_EVCTRL_PEREO_Msk & ((value) << RTC_MODE1_EVCTRL_PEREO_Pos))
#define RTC_MODE1_EVCTRL_CMPEO0_Pos 8            /**< \brief (RTC_MODE1_EVCTRL) Compare 0 Event Output Enable */
#define RTC_MODE1_EVCTRL_CMPEO0     (_U_(1) << RTC_MODE1_EVCTRL_CMPEO0_Pos)
#define RTC_MODE1_EVCTRL_CMPEO1_Pos 9            /**< \brief (RTC_MODE1_EVCTRL) Compare 1 Event Output Enable */
#define RTC_MODE1_EVCTRL_CMPEO1     (_U_(1) << RTC_MODE1_EVCTRL_CMPEO1_Pos)
#define RTC_MODE1_EVCTRL_CMPEO2_Pos 10           /**< \brief (RTC_MODE1_EVCTRL) Compare 2 Event Output Enable */
#define RTC_MODE1_EVCTRL_CMPEO2     (_U_(1) << RTC_MODE1_EVCTRL_CMPEO2_Pos)
#define RTC_MODE1_EVCTRL_CMPEO3_Pos 11           /**< \brief (RTC_MODE1_EVCTRL) Compare 3 Event Output Enable */
#define RTC_MODE1_EVCTRL_CMPEO3     (_U_(1) << RTC_MODE1_EVCTRL_CMPEO3_Pos)
#define RTC_MODE1_EVCTRL_CMPEO_Pos  8            /**< \brief (RTC_MODE1_EVCTRL) Compare x Event Output Enable */
#define RTC_MODE1_EVCTRL_CMPEO_Msk  (_U_(0xF) << RTC_MODE1_EVCTRL_CMPEO_Pos)
#define RTC_MODE1_EVCTRL_CMPEO(value) (RTC_MODE1_EVCTRL_CMPEO_Msk & ((value) << RTC_MODE1_EVCTRL_CMPEO_Pos))
#define RTC_MODE1_EVCTRL_TAMPEREO_Pos 14           /**< \brief (RTC_MODE1_EVCTRL) Tamper Event Output Enable */
#define RTC_MODE1_EVCTRL_TAMPEREO   (_U_(0x1) << RTC_MODE1_EVCTRL_TAMPEREO_Pos)
#define RTC_MODE1_EVCTRL_OVFEO_Pos  15           /**< \brief (RTC_MODE1_EVCTRL) Overflow Event Output Enable */
#define RTC_MODE1_EVCTRL_OVFEO      (_U_(0x1) << RTC_MODE1_EVCTRL_OVFEO_Pos)
#define RTC_MODE1_EVCTRL_TAMPEVEI_Pos 16           /**< \brief (RTC_MODE1_EVCTRL) Tamper Event Input Enable */
#define RTC_MODE1_EVCTRL_TAMPEVEI   (_U_(0x1) << RTC_MODE1_EVCTRL_TAMPEVEI_Pos)
#define RTC_MODE1_EVCTRL_MASK       _U_(0x0001CFFF) /**< \brief (RTC_MODE1_EVCTRL) MASK Register */

/* -------- RTC_MODE2_EVCTRL : (RTC Offset: 0x04) (R/W 32) MODE2 MODE2 Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PEREO0:1;         /*!< bit:      0  Periodic Interval 0 Event Output Enable */
    uint32_t PEREO1:1;         /*!< bit:      1  Periodic Interval 1 Event Output Enable */
    uint32_t PEREO2:1;         /*!< bit:      2  Periodic Interval 2 Event Output Enable */
    uint32_t PEREO3:1;         /*!< bit:      3  Periodic Interval 3 Event Output Enable */
    uint32_t PEREO4:1;         /*!< bit:      4  Periodic Interval 4 Event Output Enable */
    uint32_t PEREO5:1;         /*!< bit:      5  Periodic Interval 5 Event Output Enable */
    uint32_t PEREO6:1;         /*!< bit:      6  Periodic Interval 6 Event Output Enable */
    uint32_t PEREO7:1;         /*!< bit:      7  Periodic Interval 7 Event Output Enable */
    uint32_t ALARMEO0:1;       /*!< bit:      8  Alarm 0 Event Output Enable        */
    uint32_t ALARMEO1:1;       /*!< bit:      9  Alarm 1 Event Output Enable        */
    uint32_t :4;               /*!< bit: 10..13  Reserved                           */
    uint32_t TAMPEREO:1;       /*!< bit:     14  Tamper Event Output Enable         */
    uint32_t OVFEO:1;          /*!< bit:     15  Overflow Event Output Enable       */
    uint32_t TAMPEVEI:1;       /*!< bit:     16  Tamper Event Input Enable          */
    uint32_t :15;              /*!< bit: 17..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t PEREO:8;          /*!< bit:  0.. 7  Periodic Interval x Event Output Enable */
    uint32_t ALARMEO:2;        /*!< bit:  8.. 9  Alarm x Event Output Enable        */
    uint32_t :22;              /*!< bit: 10..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_EVCTRL_OFFSET     0x04         /**< \brief (RTC_MODE2_EVCTRL offset) MODE2 Event Control */
#define RTC_MODE2_EVCTRL_RESETVALUE _U_(0x00000000) /**< \brief (RTC_MODE2_EVCTRL reset_value) MODE2 Event Control */

#define RTC_MODE2_EVCTRL_PEREO0_Pos 0            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval 0 Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO0     (_U_(1) << RTC_MODE2_EVCTRL_PEREO0_Pos)
#define RTC_MODE2_EVCTRL_PEREO1_Pos 1            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval 1 Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO1     (_U_(1) << RTC_MODE2_EVCTRL_PEREO1_Pos)
#define RTC_MODE2_EVCTRL_PEREO2_Pos 2            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval 2 Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO2     (_U_(1) << RTC_MODE2_EVCTRL_PEREO2_Pos)
#define RTC_MODE2_EVCTRL_PEREO3_Pos 3            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval 3 Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO3     (_U_(1) << RTC_MODE2_EVCTRL_PEREO3_Pos)
#define RTC_MODE2_EVCTRL_PEREO4_Pos 4            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval 4 Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO4     (_U_(1) << RTC_MODE2_EVCTRL_PEREO4_Pos)
#define RTC_MODE2_EVCTRL_PEREO5_Pos 5            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval 5 Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO5     (_U_(1) << RTC_MODE2_EVCTRL_PEREO5_Pos)
#define RTC_MODE2_EVCTRL_PEREO6_Pos 6            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval 6 Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO6     (_U_(1) << RTC_MODE2_EVCTRL_PEREO6_Pos)
#define RTC_MODE2_EVCTRL_PEREO7_Pos 7            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval 7 Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO7     (_U_(1) << RTC_MODE2_EVCTRL_PEREO7_Pos)
#define RTC_MODE2_EVCTRL_PEREO_Pos  0            /**< \brief (RTC_MODE2_EVCTRL) Periodic Interval x Event Output Enable */
#define RTC_MODE2_EVCTRL_PEREO_Msk  (_U_(0xFF) << RTC_MODE2_EVCTRL_PEREO_Pos)
#define RTC_MODE2_EVCTRL_PEREO(value) (RTC_MODE2_EVCTRL_PEREO_Msk & ((value) << RTC_MODE2_EVCTRL_PEREO_Pos))
#define RTC_MODE2_EVCTRL_ALARMEO0_Pos 8            /**< \brief (RTC_MODE2_EVCTRL) Alarm 0 Event Output Enable */
#define RTC_MODE2_EVCTRL_ALARMEO0   (_U_(1) << RTC_MODE2_EVCTRL_ALARMEO0_Pos)
#define RTC_MODE2_EVCTRL_ALARMEO1_Pos 9            /**< \brief (RTC_MODE2_EVCTRL) Alarm 1 Event Output Enable */
#define RTC_MODE2_EVCTRL_ALARMEO1   (_U_(1) << RTC_MODE2_EVCTRL_ALARMEO1_Pos)
#define RTC_MODE2_EVCTRL_ALARMEO_Pos 8            /**< \brief (RTC_MODE2_EVCTRL) Alarm x Event Output Enable */
#define RTC_MODE2_EVCTRL_ALARMEO_Msk (_U_(0x3) << RTC_MODE2_EVCTRL_ALARMEO_Pos)
#define RTC_MODE2_EVCTRL_ALARMEO(value) (RTC_MODE2_EVCTRL_ALARMEO_Msk & ((value) << RTC_MODE2_EVCTRL_ALARMEO_Pos))
#define RTC_MODE2_EVCTRL_TAMPEREO_Pos 14           /**< \brief (RTC_MODE2_EVCTRL) Tamper Event Output Enable */
#define RTC_MODE2_EVCTRL_TAMPEREO   (_U_(0x1) << RTC_MODE2_EVCTRL_TAMPEREO_Pos)
#define RTC_MODE2_EVCTRL_OVFEO_Pos  15           /**< \brief (RTC_MODE2_EVCTRL) Overflow Event Output Enable */
#define RTC_MODE2_EVCTRL_OVFEO      (_U_(0x1) << RTC_MODE2_EVCTRL_OVFEO_Pos)
#define RTC_MODE2_EVCTRL_TAMPEVEI_Pos 16           /**< \brief (RTC_MODE2_EVCTRL) Tamper Event Input Enable */
#define RTC_MODE2_EVCTRL_TAMPEVEI   (_U_(0x1) << RTC_MODE2_EVCTRL_TAMPEVEI_Pos)
#define RTC_MODE2_EVCTRL_MASK       _U_(0x0001C3FF) /**< \brief (RTC_MODE2_EVCTRL) MASK Register */

/* -------- RTC_MODE0_INTENCLR : (RTC Offset: 0x08) (R/W 16) MODE0 MODE0 Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0 Interrupt Enable */
    uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1 Interrupt Enable */
    uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2 Interrupt Enable */
    uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3 Interrupt Enable */
    uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4 Interrupt Enable */
    uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5 Interrupt Enable */
    uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6 Interrupt Enable */
    uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7 Interrupt Enable */
    uint16_t CMP0:1;           /*!< bit:      8  Compare 0 Interrupt Enable         */
    uint16_t CMP1:1;           /*!< bit:      9  Compare 1 Interrupt Enable         */
    uint16_t :4;               /*!< bit: 10..13  Reserved                           */
    uint16_t TAMPER:1;         /*!< bit:     14  Tamper Enable                      */
    uint16_t OVF:1;            /*!< bit:     15  Overflow Interrupt Enable          */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x Interrupt Enable */
    uint16_t CMP:2;            /*!< bit:  8.. 9  Compare x Interrupt Enable         */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_INTENCLR_OFFSET   0x08         /**< \brief (RTC_MODE0_INTENCLR offset) MODE0 Interrupt Enable Clear */
#define RTC_MODE0_INTENCLR_RESETVALUE _U_(0x0000)  /**< \brief (RTC_MODE0_INTENCLR reset_value) MODE0 Interrupt Enable Clear */

#define RTC_MODE0_INTENCLR_PER0_Pos 0            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval 0 Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER0     (_U_(1) << RTC_MODE0_INTENCLR_PER0_Pos)
#define RTC_MODE0_INTENCLR_PER1_Pos 1            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval 1 Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER1     (_U_(1) << RTC_MODE0_INTENCLR_PER1_Pos)
#define RTC_MODE0_INTENCLR_PER2_Pos 2            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval 2 Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER2     (_U_(1) << RTC_MODE0_INTENCLR_PER2_Pos)
#define RTC_MODE0_INTENCLR_PER3_Pos 3            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval 3 Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER3     (_U_(1) << RTC_MODE0_INTENCLR_PER3_Pos)
#define RTC_MODE0_INTENCLR_PER4_Pos 4            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval 4 Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER4     (_U_(1) << RTC_MODE0_INTENCLR_PER4_Pos)
#define RTC_MODE0_INTENCLR_PER5_Pos 5            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval 5 Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER5     (_U_(1) << RTC_MODE0_INTENCLR_PER5_Pos)
#define RTC_MODE0_INTENCLR_PER6_Pos 6            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval 6 Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER6     (_U_(1) << RTC_MODE0_INTENCLR_PER6_Pos)
#define RTC_MODE0_INTENCLR_PER7_Pos 7            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval 7 Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER7     (_U_(1) << RTC_MODE0_INTENCLR_PER7_Pos)
#define RTC_MODE0_INTENCLR_PER_Pos  0            /**< \brief (RTC_MODE0_INTENCLR) Periodic Interval x Interrupt Enable */
#define RTC_MODE0_INTENCLR_PER_Msk  (_U_(0xFF) << RTC_MODE0_INTENCLR_PER_Pos)
#define RTC_MODE0_INTENCLR_PER(value) (RTC_MODE0_INTENCLR_PER_Msk & ((value) << RTC_MODE0_INTENCLR_PER_Pos))
#define RTC_MODE0_INTENCLR_CMP0_Pos 8            /**< \brief (RTC_MODE0_INTENCLR) Compare 0 Interrupt Enable */
#define RTC_MODE0_INTENCLR_CMP0     (_U_(1) << RTC_MODE0_INTENCLR_CMP0_Pos)
#define RTC_MODE0_INTENCLR_CMP1_Pos 9            /**< \brief (RTC_MODE0_INTENCLR) Compare 1 Interrupt Enable */
#define RTC_MODE0_INTENCLR_CMP1     (_U_(1) << RTC_MODE0_INTENCLR_CMP1_Pos)
#define RTC_MODE0_INTENCLR_CMP_Pos  8            /**< \brief (RTC_MODE0_INTENCLR) Compare x Interrupt Enable */
#define RTC_MODE0_INTENCLR_CMP_Msk  (_U_(0x3) << RTC_MODE0_INTENCLR_CMP_Pos)
#define RTC_MODE0_INTENCLR_CMP(value) (RTC_MODE0_INTENCLR_CMP_Msk & ((value) << RTC_MODE0_INTENCLR_CMP_Pos))
#define RTC_MODE0_INTENCLR_TAMPER_Pos 14           /**< \brief (RTC_MODE0_INTENCLR) Tamper Enable */
#define RTC_MODE0_INTENCLR_TAMPER   (_U_(0x1) << RTC_MODE0_INTENCLR_TAMPER_Pos)
#define RTC_MODE0_INTENCLR_OVF_Pos  15           /**< \brief (RTC_MODE0_INTENCLR) Overflow Interrupt Enable */
#define RTC_MODE0_INTENCLR_OVF      (_U_(0x1) << RTC_MODE0_INTENCLR_OVF_Pos)
#define RTC_MODE0_INTENCLR_MASK     _U_(0xC3FF)  /**< \brief (RTC_MODE0_INTENCLR) MASK Register */

/* -------- RTC_MODE1_INTENCLR : (RTC Offset: 0x08) (R/W 16) MODE1 MODE1 Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0 Interrupt Enable */
    uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1 Interrupt Enable */
    uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2 Interrupt Enable */
    uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3 Interrupt Enable */
    uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4 Interrupt Enable */
    uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5 Interrupt Enable */
    uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6 Interrupt Enable */
    uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7 Interrupt Enable */
    uint16_t CMP0:1;           /*!< bit:      8  Compare 0 Interrupt Enable         */
    uint16_t CMP1:1;           /*!< bit:      9  Compare 1 Interrupt Enable         */
    uint16_t CMP2:1;           /*!< bit:     10  Compare 2 Interrupt Enable         */
    uint16_t CMP3:1;           /*!< bit:     11  Compare 3 Interrupt Enable         */
    uint16_t :2;               /*!< bit: 12..13  Reserved                           */
    uint16_t TAMPER:1;         /*!< bit:     14  Tamper Enable                      */
    uint16_t OVF:1;            /*!< bit:     15  Overflow Interrupt Enable          */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x Interrupt Enable */
    uint16_t CMP:4;            /*!< bit:  8..11  Compare x Interrupt Enable         */
    uint16_t :4;               /*!< bit: 12..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_INTENCLR_OFFSET   0x08         /**< \brief (RTC_MODE1_INTENCLR offset) MODE1 Interrupt Enable Clear */
#define RTC_MODE1_INTENCLR_RESETVALUE _U_(0x0000)  /**< \brief (RTC_MODE1_INTENCLR reset_value) MODE1 Interrupt Enable Clear */

#define RTC_MODE1_INTENCLR_PER0_Pos 0            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval 0 Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER0     (_U_(1) << RTC_MODE1_INTENCLR_PER0_Pos)
#define RTC_MODE1_INTENCLR_PER1_Pos 1            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval 1 Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER1     (_U_(1) << RTC_MODE1_INTENCLR_PER1_Pos)
#define RTC_MODE1_INTENCLR_PER2_Pos 2            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval 2 Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER2     (_U_(1) << RTC_MODE1_INTENCLR_PER2_Pos)
#define RTC_MODE1_INTENCLR_PER3_Pos 3            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval 3 Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER3     (_U_(1) << RTC_MODE1_INTENCLR_PER3_Pos)
#define RTC_MODE1_INTENCLR_PER4_Pos 4            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval 4 Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER4     (_U_(1) << RTC_MODE1_INTENCLR_PER4_Pos)
#define RTC_MODE1_INTENCLR_PER5_Pos 5            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval 5 Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER5     (_U_(1) << RTC_MODE1_INTENCLR_PER5_Pos)
#define RTC_MODE1_INTENCLR_PER6_Pos 6            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval 6 Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER6     (_U_(1) << RTC_MODE1_INTENCLR_PER6_Pos)
#define RTC_MODE1_INTENCLR_PER7_Pos 7            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval 7 Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER7     (_U_(1) << RTC_MODE1_INTENCLR_PER7_Pos)
#define RTC_MODE1_INTENCLR_PER_Pos  0            /**< \brief (RTC_MODE1_INTENCLR) Periodic Interval x Interrupt Enable */
#define RTC_MODE1_INTENCLR_PER_Msk  (_U_(0xFF) << RTC_MODE1_INTENCLR_PER_Pos)
#define RTC_MODE1_INTENCLR_PER(value) (RTC_MODE1_INTENCLR_PER_Msk & ((value) << RTC_MODE1_INTENCLR_PER_Pos))
#define RTC_MODE1_INTENCLR_CMP0_Pos 8            /**< \brief (RTC_MODE1_INTENCLR) Compare 0 Interrupt Enable */
#define RTC_MODE1_INTENCLR_CMP0     (_U_(1) << RTC_MODE1_INTENCLR_CMP0_Pos)
#define RTC_MODE1_INTENCLR_CMP1_Pos 9            /**< \brief (RTC_MODE1_INTENCLR) Compare 1 Interrupt Enable */
#define RTC_MODE1_INTENCLR_CMP1     (_U_(1) << RTC_MODE1_INTENCLR_CMP1_Pos)
#define RTC_MODE1_INTENCLR_CMP2_Pos 10           /**< \brief (RTC_MODE1_INTENCLR) Compare 2 Interrupt Enable */
#define RTC_MODE1_INTENCLR_CMP2     (_U_(1) << RTC_MODE1_INTENCLR_CMP2_Pos)
#define RTC_MODE1_INTENCLR_CMP3_Pos 11           /**< \brief (RTC_MODE1_INTENCLR) Compare 3 Interrupt Enable */
#define RTC_MODE1_INTENCLR_CMP3     (_U_(1) << RTC_MODE1_INTENCLR_CMP3_Pos)
#define RTC_MODE1_INTENCLR_CMP_Pos  8            /**< \brief (RTC_MODE1_INTENCLR) Compare x Interrupt Enable */
#define RTC_MODE1_INTENCLR_CMP_Msk  (_U_(0xF) << RTC_MODE1_INTENCLR_CMP_Pos)
#define RTC_MODE1_INTENCLR_CMP(value) (RTC_MODE1_INTENCLR_CMP_Msk & ((value) << RTC_MODE1_INTENCLR_CMP_Pos))
#define RTC_MODE1_INTENCLR_TAMPER_Pos 14           /**< \brief (RTC_MODE1_INTENCLR) Tamper Enable */
#define RTC_MODE1_INTENCLR_TAMPER   (_U_(0x1) << RTC_MODE1_INTENCLR_TAMPER_Pos)
#define RTC_MODE1_INTENCLR_OVF_Pos  15           /**< \brief (RTC_MODE1_INTENCLR) Overflow Interrupt Enable */
#define RTC_MODE1_INTENCLR_OVF      (_U_(0x1) << RTC_MODE1_INTENCLR_OVF_Pos)
#define RTC_MODE1_INTENCLR_MASK     _U_(0xCFFF)  /**< \brief (RTC_MODE1_INTENCLR) MASK Register */

/* -------- RTC_MODE2_INTENCLR : (RTC Offset: 0x08) (R/W 16) MODE2 MODE2 Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0 Interrupt Enable */
    uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1 Interrupt Enable */
    uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2 Interrupt Enable */
    uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3 Interrupt Enable */
    uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4 Interrupt Enable */
    uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5 Interrupt Enable */
    uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6 Interrupt Enable */
    uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7 Interrupt Enable */
    uint16_t ALARM0:1;         /*!< bit:      8  Alarm 0 Interrupt Enable           */
    uint16_t ALARM1:1;         /*!< bit:      9  Alarm 1 Interrupt Enable           */
    uint16_t :4;               /*!< bit: 10..13  Reserved                           */
    uint16_t TAMPER:1;         /*!< bit:     14  Tamper Enable                      */
    uint16_t OVF:1;            /*!< bit:     15  Overflow Interrupt Enable          */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x Interrupt Enable */
    uint16_t ALARM:2;          /*!< bit:  8.. 9  Alarm x Interrupt Enable           */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_INTENCLR_OFFSET   0x08         /**< \brief (RTC_MODE2_INTENCLR offset) MODE2 Interrupt Enable Clear */
#define RTC_MODE2_INTENCLR_RESETVALUE _U_(0x0000)  /**< \brief (RTC_MODE2_INTENCLR reset_value) MODE2 Interrupt Enable Clear */

#define RTC_MODE2_INTENCLR_PER0_Pos 0            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval 0 Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER0     (_U_(1) << RTC_MODE2_INTENCLR_PER0_Pos)
#define RTC_MODE2_INTENCLR_PER1_Pos 1            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval 1 Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER1     (_U_(1) << RTC_MODE2_INTENCLR_PER1_Pos)
#define RTC_MODE2_INTENCLR_PER2_Pos 2            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval 2 Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER2     (_U_(1) << RTC_MODE2_INTENCLR_PER2_Pos)
#define RTC_MODE2_INTENCLR_PER3_Pos 3            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval 3 Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER3     (_U_(1) << RTC_MODE2_INTENCLR_PER3_Pos)
#define RTC_MODE2_INTENCLR_PER4_Pos 4            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval 4 Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER4     (_U_(1) << RTC_MODE2_INTENCLR_PER4_Pos)
#define RTC_MODE2_INTENCLR_PER5_Pos 5            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval 5 Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER5     (_U_(1) << RTC_MODE2_INTENCLR_PER5_Pos)
#define RTC_MODE2_INTENCLR_PER6_Pos 6            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval 6 Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER6     (_U_(1) << RTC_MODE2_INTENCLR_PER6_Pos)
#define RTC_MODE2_INTENCLR_PER7_Pos 7            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval 7 Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER7     (_U_(1) << RTC_MODE2_INTENCLR_PER7_Pos)
#define RTC_MODE2_INTENCLR_PER_Pos  0            /**< \brief (RTC_MODE2_INTENCLR) Periodic Interval x Interrupt Enable */
#define RTC_MODE2_INTENCLR_PER_Msk  (_U_(0xFF) << RTC_MODE2_INTENCLR_PER_Pos)
#define RTC_MODE2_INTENCLR_PER(value) (RTC_MODE2_INTENCLR_PER_Msk & ((value) << RTC_MODE2_INTENCLR_PER_Pos))
#define RTC_MODE2_INTENCLR_ALARM0_Pos 8            /**< \brief (RTC_MODE2_INTENCLR) Alarm 0 Interrupt Enable */
#define RTC_MODE2_INTENCLR_ALARM0   (_U_(1) << RTC_MODE2_INTENCLR_ALARM0_Pos)
#define RTC_MODE2_INTENCLR_ALARM1_Pos 9            /**< \brief (RTC_MODE2_INTENCLR) Alarm 1 Interrupt Enable */
#define RTC_MODE2_INTENCLR_ALARM1   (_U_(1) << RTC_MODE2_INTENCLR_ALARM1_Pos)
#define RTC_MODE2_INTENCLR_ALARM_Pos 8            /**< \brief (RTC_MODE2_INTENCLR) Alarm x Interrupt Enable */
#define RTC_MODE2_INTENCLR_ALARM_Msk (_U_(0x3) << RTC_MODE2_INTENCLR_ALARM_Pos)
#define RTC_MODE2_INTENCLR_ALARM(value) (RTC_MODE2_INTENCLR_ALARM_Msk & ((value) << RTC_MODE2_INTENCLR_ALARM_Pos))
#define RTC_MODE2_INTENCLR_TAMPER_Pos 14           /**< \brief (RTC_MODE2_INTENCLR) Tamper Enable */
#define RTC_MODE2_INTENCLR_TAMPER   (_U_(0x1) << RTC_MODE2_INTENCLR_TAMPER_Pos)
#define RTC_MODE2_INTENCLR_OVF_Pos  15           /**< \brief (RTC_MODE2_INTENCLR) Overflow Interrupt Enable */
#define RTC_MODE2_INTENCLR_OVF      (_U_(0x1) << RTC_MODE2_INTENCLR_OVF_Pos)
#define RTC_MODE2_INTENCLR_MASK     _U_(0xC3FF)  /**< \brief (RTC_MODE2_INTENCLR) MASK Register */

/* -------- RTC_MODE0_INTENSET : (RTC Offset: 0x0A) (R/W 16) MODE0 MODE0 Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0 Interrupt Enable */
    uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1 Interrupt Enable */
    uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2 Interrupt Enable */
    uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3 Interrupt Enable */
    uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4 Interrupt Enable */
    uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5 Interrupt Enable */
    uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6 Interrupt Enable */
    uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7 Interrupt Enable */
    uint16_t CMP0:1;           /*!< bit:      8  Compare 0 Interrupt Enable         */
    uint16_t CMP1:1;           /*!< bit:      9  Compare 1 Interrupt Enable         */
    uint16_t :4;               /*!< bit: 10..13  Reserved                           */
    uint16_t TAMPER:1;         /*!< bit:     14  Tamper Enable                      */
    uint16_t OVF:1;            /*!< bit:     15  Overflow Interrupt Enable          */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x Interrupt Enable */
    uint16_t CMP:2;            /*!< bit:  8.. 9  Compare x Interrupt Enable         */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_INTENSET_OFFSET   0x0A         /**< \brief (RTC_MODE0_INTENSET offset) MODE0 Interrupt Enable Set */
#define RTC_MODE0_INTENSET_RESETVALUE _U_(0x0000)  /**< \brief (RTC_MODE0_INTENSET reset_value) MODE0 Interrupt Enable Set */

#define RTC_MODE0_INTENSET_PER0_Pos 0            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval 0 Interrupt Enable */
#define RTC_MODE0_INTENSET_PER0     (_U_(1) << RTC_MODE0_INTENSET_PER0_Pos)
#define RTC_MODE0_INTENSET_PER1_Pos 1            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval 1 Interrupt Enable */
#define RTC_MODE0_INTENSET_PER1     (_U_(1) << RTC_MODE0_INTENSET_PER1_Pos)
#define RTC_MODE0_INTENSET_PER2_Pos 2            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval 2 Interrupt Enable */
#define RTC_MODE0_INTENSET_PER2     (_U_(1) << RTC_MODE0_INTENSET_PER2_Pos)
#define RTC_MODE0_INTENSET_PER3_Pos 3            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval 3 Interrupt Enable */
#define RTC_MODE0_INTENSET_PER3     (_U_(1) << RTC_MODE0_INTENSET_PER3_Pos)
#define RTC_MODE0_INTENSET_PER4_Pos 4            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval 4 Interrupt Enable */
#define RTC_MODE0_INTENSET_PER4     (_U_(1) << RTC_MODE0_INTENSET_PER4_Pos)
#define RTC_MODE0_INTENSET_PER5_Pos 5            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval 5 Interrupt Enable */
#define RTC_MODE0_INTENSET_PER5     (_U_(1) << RTC_MODE0_INTENSET_PER5_Pos)
#define RTC_MODE0_INTENSET_PER6_Pos 6            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval 6 Interrupt Enable */
#define RTC_MODE0_INTENSET_PER6     (_U_(1) << RTC_MODE0_INTENSET_PER6_Pos)
#define RTC_MODE0_INTENSET_PER7_Pos 7            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval 7 Interrupt Enable */
#define RTC_MODE0_INTENSET_PER7     (_U_(1) << RTC_MODE0_INTENSET_PER7_Pos)
#define RTC_MODE0_INTENSET_PER_Pos  0            /**< \brief (RTC_MODE0_INTENSET) Periodic Interval x Interrupt Enable */
#define RTC_MODE0_INTENSET_PER_Msk  (_U_(0xFF) << RTC_MODE0_INTENSET_PER_Pos)
#define RTC_MODE0_INTENSET_PER(value) (RTC_MODE0_INTENSET_PER_Msk & ((value) << RTC_MODE0_INTENSET_PER_Pos))
#define RTC_MODE0_INTENSET_CMP0_Pos 8            /**< \brief (RTC_MODE0_INTENSET) Compare 0 Interrupt Enable */
#define RTC_MODE0_INTENSET_CMP0     (_U_(1) << RTC_MODE0_INTENSET_CMP0_Pos)
#define RTC_MODE0_INTENSET_CMP1_Pos 9            /**< \brief (RTC_MODE0_INTENSET) Compare 1 Interrupt Enable */
#define RTC_MODE0_INTENSET_CMP1     (_U_(1) << RTC_MODE0_INTENSET_CMP1_Pos)
#define RTC_MODE0_INTENSET_CMP_Pos  8            /**< \brief (RTC_MODE0_INTENSET) Compare x Interrupt Enable */
#define RTC_MODE0_INTENSET_CMP_Msk  (_U_(0x3) << RTC_MODE0_INTENSET_CMP_Pos)
#define RTC_MODE0_INTENSET_CMP(value) (RTC_MODE0_INTENSET_CMP_Msk & ((value) << RTC_MODE0_INTENSET_CMP_Pos))
#define RTC_MODE0_INTENSET_TAMPER_Pos 14           /**< \brief (RTC_MODE0_INTENSET) Tamper Enable */
#define RTC_MODE0_INTENSET_TAMPER   (_U_(0x1) << RTC_MODE0_INTENSET_TAMPER_Pos)
#define RTC_MODE0_INTENSET_OVF_Pos  15           /**< \brief (RTC_MODE0_INTENSET) Overflow Interrupt Enable */
#define RTC_MODE0_INTENSET_OVF      (_U_(0x1) << RTC_MODE0_INTENSET_OVF_Pos)
#define RTC_MODE0_INTENSET_MASK     _U_(0xC3FF)  /**< \brief (RTC_MODE0_INTENSET) MASK Register */

/* -------- RTC_MODE1_INTENSET : (RTC Offset: 0x0A) (R/W 16) MODE1 MODE1 Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0 Interrupt Enable */
    uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1 Interrupt Enable */
    uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2 Interrupt Enable */
    uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3 Interrupt Enable */
    uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4 Interrupt Enable */
    uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5 Interrupt Enable */
    uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6 Interrupt Enable */
    uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7 Interrupt Enable */
    uint16_t CMP0:1;           /*!< bit:      8  Compare 0 Interrupt Enable         */
    uint16_t CMP1:1;           /*!< bit:      9  Compare 1 Interrupt Enable         */
    uint16_t CMP2:1;           /*!< bit:     10  Compare 2 Interrupt Enable         */
    uint16_t CMP3:1;           /*!< bit:     11  Compare 3 Interrupt Enable         */
    uint16_t :2;               /*!< bit: 12..13  Reserved                           */
    uint16_t TAMPER:1;         /*!< bit:     14  Tamper Enable                      */
    uint16_t OVF:1;            /*!< bit:     15  Overflow Interrupt Enable          */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x Interrupt Enable */
    uint16_t CMP:4;            /*!< bit:  8..11  Compare x Interrupt Enable         */
    uint16_t :4;               /*!< bit: 12..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_INTENSET_OFFSET   0x0A         /**< \brief (RTC_MODE1_INTENSET offset) MODE1 Interrupt Enable Set */
#define RTC_MODE1_INTENSET_RESETVALUE _U_(0x0000)  /**< \brief (RTC_MODE1_INTENSET reset_value) MODE1 Interrupt Enable Set */

#define RTC_MODE1_INTENSET_PER0_Pos 0            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval 0 Interrupt Enable */
#define RTC_MODE1_INTENSET_PER0     (_U_(1) << RTC_MODE1_INTENSET_PER0_Pos)
#define RTC_MODE1_INTENSET_PER1_Pos 1            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval 1 Interrupt Enable */
#define RTC_MODE1_INTENSET_PER1     (_U_(1) << RTC_MODE1_INTENSET_PER1_Pos)
#define RTC_MODE1_INTENSET_PER2_Pos 2            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval 2 Interrupt Enable */
#define RTC_MODE1_INTENSET_PER2     (_U_(1) << RTC_MODE1_INTENSET_PER2_Pos)
#define RTC_MODE1_INTENSET_PER3_Pos 3            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval 3 Interrupt Enable */
#define RTC_MODE1_INTENSET_PER3     (_U_(1) << RTC_MODE1_INTENSET_PER3_Pos)
#define RTC_MODE1_INTENSET_PER4_Pos 4            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval 4 Interrupt Enable */
#define RTC_MODE1_INTENSET_PER4     (_U_(1) << RTC_MODE1_INTENSET_PER4_Pos)
#define RTC_MODE1_INTENSET_PER5_Pos 5            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval 5 Interrupt Enable */
#define RTC_MODE1_INTENSET_PER5     (_U_(1) << RTC_MODE1_INTENSET_PER5_Pos)
#define RTC_MODE1_INTENSET_PER6_Pos 6            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval 6 Interrupt Enable */
#define RTC_MODE1_INTENSET_PER6     (_U_(1) << RTC_MODE1_INTENSET_PER6_Pos)
#define RTC_MODE1_INTENSET_PER7_Pos 7            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval 7 Interrupt Enable */
#define RTC_MODE1_INTENSET_PER7     (_U_(1) << RTC_MODE1_INTENSET_PER7_Pos)
#define RTC_MODE1_INTENSET_PER_Pos  0            /**< \brief (RTC_MODE1_INTENSET) Periodic Interval x Interrupt Enable */
#define RTC_MODE1_INTENSET_PER_Msk  (_U_(0xFF) << RTC_MODE1_INTENSET_PER_Pos)
#define RTC_MODE1_INTENSET_PER(value) (RTC_MODE1_INTENSET_PER_Msk & ((value) << RTC_MODE1_INTENSET_PER_Pos))
#define RTC_MODE1_INTENSET_CMP0_Pos 8            /**< \brief (RTC_MODE1_INTENSET) Compare 0 Interrupt Enable */
#define RTC_MODE1_INTENSET_CMP0     (_U_(1) << RTC_MODE1_INTENSET_CMP0_Pos)
#define RTC_MODE1_INTENSET_CMP1_Pos 9            /**< \brief (RTC_MODE1_INTENSET) Compare 1 Interrupt Enable */
#define RTC_MODE1_INTENSET_CMP1     (_U_(1) << RTC_MODE1_INTENSET_CMP1_Pos)
#define RTC_MODE1_INTENSET_CMP2_Pos 10           /**< \brief (RTC_MODE1_INTENSET) Compare 2 Interrupt Enable */
#define RTC_MODE1_INTENSET_CMP2     (_U_(1) << RTC_MODE1_INTENSET_CMP2_Pos)
#define RTC_MODE1_INTENSET_CMP3_Pos 11           /**< \brief (RTC_MODE1_INTENSET) Compare 3 Interrupt Enable */
#define RTC_MODE1_INTENSET_CMP3     (_U_(1) << RTC_MODE1_INTENSET_CMP3_Pos)
#define RTC_MODE1_INTENSET_CMP_Pos  8            /**< \brief (RTC_MODE1_INTENSET) Compare x Interrupt Enable */
#define RTC_MODE1_INTENSET_CMP_Msk  (_U_(0xF) << RTC_MODE1_INTENSET_CMP_Pos)
#define RTC_MODE1_INTENSET_CMP(value) (RTC_MODE1_INTENSET_CMP_Msk & ((value) << RTC_MODE1_INTENSET_CMP_Pos))
#define RTC_MODE1_INTENSET_TAMPER_Pos 14           /**< \brief (RTC_MODE1_INTENSET) Tamper Enable */
#define RTC_MODE1_INTENSET_TAMPER   (_U_(0x1) << RTC_MODE1_INTENSET_TAMPER_Pos)
#define RTC_MODE1_INTENSET_OVF_Pos  15           /**< \brief (RTC_MODE1_INTENSET) Overflow Interrupt Enable */
#define RTC_MODE1_INTENSET_OVF      (_U_(0x1) << RTC_MODE1_INTENSET_OVF_Pos)
#define RTC_MODE1_INTENSET_MASK     _U_(0xCFFF)  /**< \brief (RTC_MODE1_INTENSET) MASK Register */

/* -------- RTC_MODE2_INTENSET : (RTC Offset: 0x0A) (R/W 16) MODE2 MODE2 Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0 Enable         */
    uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1 Enable         */
    uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2 Enable         */
    uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3 Enable         */
    uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4 Enable         */
    uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5 Enable         */
    uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6 Enable         */
    uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7 Enable         */
    uint16_t ALARM0:1;         /*!< bit:      8  Alarm 0 Interrupt Enable           */
    uint16_t ALARM1:1;         /*!< bit:      9  Alarm 1 Interrupt Enable           */
    uint16_t :4;               /*!< bit: 10..13  Reserved                           */
    uint16_t TAMPER:1;         /*!< bit:     14  Tamper Enable                      */
    uint16_t OVF:1;            /*!< bit:     15  Overflow Interrupt Enable          */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x Enable         */
    uint16_t ALARM:2;          /*!< bit:  8.. 9  Alarm x Interrupt Enable           */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_INTENSET_OFFSET   0x0A         /**< \brief (RTC_MODE2_INTENSET offset) MODE2 Interrupt Enable Set */
#define RTC_MODE2_INTENSET_RESETVALUE _U_(0x0000)  /**< \brief (RTC_MODE2_INTENSET reset_value) MODE2 Interrupt Enable Set */

#define RTC_MODE2_INTENSET_PER0_Pos 0            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval 0 Enable */
#define RTC_MODE2_INTENSET_PER0     (_U_(1) << RTC_MODE2_INTENSET_PER0_Pos)
#define RTC_MODE2_INTENSET_PER1_Pos 1            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval 1 Enable */
#define RTC_MODE2_INTENSET_PER1     (_U_(1) << RTC_MODE2_INTENSET_PER1_Pos)
#define RTC_MODE2_INTENSET_PER2_Pos 2            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval 2 Enable */
#define RTC_MODE2_INTENSET_PER2     (_U_(1) << RTC_MODE2_INTENSET_PER2_Pos)
#define RTC_MODE2_INTENSET_PER3_Pos 3            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval 3 Enable */
#define RTC_MODE2_INTENSET_PER3     (_U_(1) << RTC_MODE2_INTENSET_PER3_Pos)
#define RTC_MODE2_INTENSET_PER4_Pos 4            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval 4 Enable */
#define RTC_MODE2_INTENSET_PER4     (_U_(1) << RTC_MODE2_INTENSET_PER4_Pos)
#define RTC_MODE2_INTENSET_PER5_Pos 5            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval 5 Enable */
#define RTC_MODE2_INTENSET_PER5     (_U_(1) << RTC_MODE2_INTENSET_PER5_Pos)
#define RTC_MODE2_INTENSET_PER6_Pos 6            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval 6 Enable */
#define RTC_MODE2_INTENSET_PER6     (_U_(1) << RTC_MODE2_INTENSET_PER6_Pos)
#define RTC_MODE2_INTENSET_PER7_Pos 7            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval 7 Enable */
#define RTC_MODE2_INTENSET_PER7     (_U_(1) << RTC_MODE2_INTENSET_PER7_Pos)
#define RTC_MODE2_INTENSET_PER_Pos  0            /**< \brief (RTC_MODE2_INTENSET) Periodic Interval x Enable */
#define RTC_MODE2_INTENSET_PER_Msk  (_U_(0xFF) << RTC_MODE2_INTENSET_PER_Pos)
#define RTC_MODE2_INTENSET_PER(value) (RTC_MODE2_INTENSET_PER_Msk & ((value) << RTC_MODE2_INTENSET_PER_Pos))
#define RTC_MODE2_INTENSET_ALARM0_Pos 8            /**< \brief (RTC_MODE2_INTENSET) Alarm 0 Interrupt Enable */
#define RTC_MODE2_INTENSET_ALARM0   (_U_(1) << RTC_MODE2_INTENSET_ALARM0_Pos)
#define RTC_MODE2_INTENSET_ALARM1_Pos 9            /**< \brief (RTC_MODE2_INTENSET) Alarm 1 Interrupt Enable */
#define RTC_MODE2_INTENSET_ALARM1   (_U_(1) << RTC_MODE2_INTENSET_ALARM1_Pos)
#define RTC_MODE2_INTENSET_ALARM_Pos 8            /**< \brief (RTC_MODE2_INTENSET) Alarm x Interrupt Enable */
#define RTC_MODE2_INTENSET_ALARM_Msk (_U_(0x3) << RTC_MODE2_INTENSET_ALARM_Pos)
#define RTC_MODE2_INTENSET_ALARM(value) (RTC_MODE2_INTENSET_ALARM_Msk & ((value) << RTC_MODE2_INTENSET_ALARM_Pos))
#define RTC_MODE2_INTENSET_TAMPER_Pos 14           /**< \brief (RTC_MODE2_INTENSET) Tamper Enable */
#define RTC_MODE2_INTENSET_TAMPER   (_U_(0x1) << RTC_MODE2_INTENSET_TAMPER_Pos)
#define RTC_MODE2_INTENSET_OVF_Pos  15           /**< \brief (RTC_MODE2_INTENSET) Overflow Interrupt Enable */
#define RTC_MODE2_INTENSET_OVF      (_U_(0x1) << RTC_MODE2_INTENSET_OVF_Pos)
#define RTC_MODE2_INTENSET_MASK     _U_(0xC3FF)  /**< \brief (RTC_MODE2_INTENSET) MASK Register */

/* -------- RTC_MODE0_INTFLAG : (RTC Offset: 0x0C) (R/W 16) MODE0 MODE0 Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0                */
    __I uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1                */
    __I uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2                */
    __I uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3                */
    __I uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4                */
    __I uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5                */
    __I uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6                */
    __I uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7                */
    __I uint16_t CMP0:1;           /*!< bit:      8  Compare 0                          */
    __I uint16_t CMP1:1;           /*!< bit:      9  Compare 1                          */
    __I uint16_t :4;               /*!< bit: 10..13  Reserved                           */
    __I uint16_t TAMPER:1;         /*!< bit:     14  Tamper                             */
    __I uint16_t OVF:1;            /*!< bit:     15  Overflow                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x                */
    __I uint16_t CMP:2;            /*!< bit:  8.. 9  Compare x                          */
    __I uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_INTFLAG_OFFSET    0x0C         /**< \brief (RTC_MODE0_INTFLAG offset) MODE0 Interrupt Flag Status and Clear */
#define RTC_MODE0_INTFLAG_RESETVALUE _U_(0x0000)  /**< \brief (RTC_MODE0_INTFLAG reset_value) MODE0 Interrupt Flag Status and Clear */

#define RTC_MODE0_INTFLAG_PER0_Pos  0            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval 0 */
#define RTC_MODE0_INTFLAG_PER0      (_U_(1) << RTC_MODE0_INTFLAG_PER0_Pos)
#define RTC_MODE0_INTFLAG_PER1_Pos  1            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval 1 */
#define RTC_MODE0_INTFLAG_PER1      (_U_(1) << RTC_MODE0_INTFLAG_PER1_Pos)
#define RTC_MODE0_INTFLAG_PER2_Pos  2            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval 2 */
#define RTC_MODE0_INTFLAG_PER2      (_U_(1) << RTC_MODE0_INTFLAG_PER2_Pos)
#define RTC_MODE0_INTFLAG_PER3_Pos  3            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval 3 */
#define RTC_MODE0_INTFLAG_PER3      (_U_(1) << RTC_MODE0_INTFLAG_PER3_Pos)
#define RTC_MODE0_INTFLAG_PER4_Pos  4            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval 4 */
#define RTC_MODE0_INTFLAG_PER4      (_U_(1) << RTC_MODE0_INTFLAG_PER4_Pos)
#define RTC_MODE0_INTFLAG_PER5_Pos  5            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval 5 */
#define RTC_MODE0_INTFLAG_PER5      (_U_(1) << RTC_MODE0_INTFLAG_PER5_Pos)
#define RTC_MODE0_INTFLAG_PER6_Pos  6            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval 6 */
#define RTC_MODE0_INTFLAG_PER6      (_U_(1) << RTC_MODE0_INTFLAG_PER6_Pos)
#define RTC_MODE0_INTFLAG_PER7_Pos  7            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval 7 */
#define RTC_MODE0_INTFLAG_PER7      (_U_(1) << RTC_MODE0_INTFLAG_PER7_Pos)
#define RTC_MODE0_INTFLAG_PER_Pos   0            /**< \brief (RTC_MODE0_INTFLAG) Periodic Interval x */
#define RTC_MODE0_INTFLAG_PER_Msk   (_U_(0xFF) << RTC_MODE0_INTFLAG_PER_Pos)
#define RTC_MODE0_INTFLAG_PER(value) (RTC_MODE0_INTFLAG_PER_Msk & ((value) << RTC_MODE0_INTFLAG_PER_Pos))
#define RTC_MODE0_INTFLAG_CMP0_Pos  8            /**< \brief (RTC_MODE0_INTFLAG) Compare 0 */
#define RTC_MODE0_INTFLAG_CMP0      (_U_(1) << RTC_MODE0_INTFLAG_CMP0_Pos)
#define RTC_MODE0_INTFLAG_CMP1_Pos  9            /**< \brief (RTC_MODE0_INTFLAG) Compare 1 */
#define RTC_MODE0_INTFLAG_CMP1      (_U_(1) << RTC_MODE0_INTFLAG_CMP1_Pos)
#define RTC_MODE0_INTFLAG_CMP_Pos   8            /**< \brief (RTC_MODE0_INTFLAG) Compare x */
#define RTC_MODE0_INTFLAG_CMP_Msk   (_U_(0x3) << RTC_MODE0_INTFLAG_CMP_Pos)
#define RTC_MODE0_INTFLAG_CMP(value) (RTC_MODE0_INTFLAG_CMP_Msk & ((value) << RTC_MODE0_INTFLAG_CMP_Pos))
#define RTC_MODE0_INTFLAG_TAMPER_Pos 14           /**< \brief (RTC_MODE0_INTFLAG) Tamper */
#define RTC_MODE0_INTFLAG_TAMPER    (_U_(0x1) << RTC_MODE0_INTFLAG_TAMPER_Pos)
#define RTC_MODE0_INTFLAG_OVF_Pos   15           /**< \brief (RTC_MODE0_INTFLAG) Overflow */
#define RTC_MODE0_INTFLAG_OVF       (_U_(0x1) << RTC_MODE0_INTFLAG_OVF_Pos)
#define RTC_MODE0_INTFLAG_MASK      _U_(0xC3FF)  /**< \brief (RTC_MODE0_INTFLAG) MASK Register */

/* -------- RTC_MODE1_INTFLAG : (RTC Offset: 0x0C) (R/W 16) MODE1 MODE1 Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0                */
    __I uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1                */
    __I uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2                */
    __I uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3                */
    __I uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4                */
    __I uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5                */
    __I uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6                */
    __I uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7                */
    __I uint16_t CMP0:1;           /*!< bit:      8  Compare 0                          */
    __I uint16_t CMP1:1;           /*!< bit:      9  Compare 1                          */
    __I uint16_t CMP2:1;           /*!< bit:     10  Compare 2                          */
    __I uint16_t CMP3:1;           /*!< bit:     11  Compare 3                          */
    __I uint16_t :2;               /*!< bit: 12..13  Reserved                           */
    __I uint16_t TAMPER:1;         /*!< bit:     14  Tamper                             */
    __I uint16_t OVF:1;            /*!< bit:     15  Overflow                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x                */
    __I uint16_t CMP:4;            /*!< bit:  8..11  Compare x                          */
    __I uint16_t :4;               /*!< bit: 12..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_INTFLAG_OFFSET    0x0C         /**< \brief (RTC_MODE1_INTFLAG offset) MODE1 Interrupt Flag Status and Clear */
#define RTC_MODE1_INTFLAG_RESETVALUE _U_(0x0000)  /**< \brief (RTC_MODE1_INTFLAG reset_value) MODE1 Interrupt Flag Status and Clear */

#define RTC_MODE1_INTFLAG_PER0_Pos  0            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval 0 */
#define RTC_MODE1_INTFLAG_PER0      (_U_(1) << RTC_MODE1_INTFLAG_PER0_Pos)
#define RTC_MODE1_INTFLAG_PER1_Pos  1            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval 1 */
#define RTC_MODE1_INTFLAG_PER1      (_U_(1) << RTC_MODE1_INTFLAG_PER1_Pos)
#define RTC_MODE1_INTFLAG_PER2_Pos  2            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval 2 */
#define RTC_MODE1_INTFLAG_PER2      (_U_(1) << RTC_MODE1_INTFLAG_PER2_Pos)
#define RTC_MODE1_INTFLAG_PER3_Pos  3            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval 3 */
#define RTC_MODE1_INTFLAG_PER3      (_U_(1) << RTC_MODE1_INTFLAG_PER3_Pos)
#define RTC_MODE1_INTFLAG_PER4_Pos  4            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval 4 */
#define RTC_MODE1_INTFLAG_PER4      (_U_(1) << RTC_MODE1_INTFLAG_PER4_Pos)
#define RTC_MODE1_INTFLAG_PER5_Pos  5            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval 5 */
#define RTC_MODE1_INTFLAG_PER5      (_U_(1) << RTC_MODE1_INTFLAG_PER5_Pos)
#define RTC_MODE1_INTFLAG_PER6_Pos  6            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval 6 */
#define RTC_MODE1_INTFLAG_PER6      (_U_(1) << RTC_MODE1_INTFLAG_PER6_Pos)
#define RTC_MODE1_INTFLAG_PER7_Pos  7            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval 7 */
#define RTC_MODE1_INTFLAG_PER7      (_U_(1) << RTC_MODE1_INTFLAG_PER7_Pos)
#define RTC_MODE1_INTFLAG_PER_Pos   0            /**< \brief (RTC_MODE1_INTFLAG) Periodic Interval x */
#define RTC_MODE1_INTFLAG_PER_Msk   (_U_(0xFF) << RTC_MODE1_INTFLAG_PER_Pos)
#define RTC_MODE1_INTFLAG_PER(value) (RTC_MODE1_INTFLAG_PER_Msk & ((value) << RTC_MODE1_INTFLAG_PER_Pos))
#define RTC_MODE1_INTFLAG_CMP0_Pos  8            /**< \brief (RTC_MODE1_INTFLAG) Compare 0 */
#define RTC_MODE1_INTFLAG_CMP0      (_U_(1) << RTC_MODE1_INTFLAG_CMP0_Pos)
#define RTC_MODE1_INTFLAG_CMP1_Pos  9            /**< \brief (RTC_MODE1_INTFLAG) Compare 1 */
#define RTC_MODE1_INTFLAG_CMP1      (_U_(1) << RTC_MODE1_INTFLAG_CMP1_Pos)
#define RTC_MODE1_INTFLAG_CMP2_Pos  10           /**< \brief (RTC_MODE1_INTFLAG) Compare 2 */
#define RTC_MODE1_INTFLAG_CMP2      (_U_(1) << RTC_MODE1_INTFLAG_CMP2_Pos)
#define RTC_MODE1_INTFLAG_CMP3_Pos  11           /**< \brief (RTC_MODE1_INTFLAG) Compare 3 */
#define RTC_MODE1_INTFLAG_CMP3      (_U_(1) << RTC_MODE1_INTFLAG_CMP3_Pos)
#define RTC_MODE1_INTFLAG_CMP_Pos   8            /**< \brief (RTC_MODE1_INTFLAG) Compare x */
#define RTC_MODE1_INTFLAG_CMP_Msk   (_U_(0xF) << RTC_MODE1_INTFLAG_CMP_Pos)
#define RTC_MODE1_INTFLAG_CMP(value) (RTC_MODE1_INTFLAG_CMP_Msk & ((value) << RTC_MODE1_INTFLAG_CMP_Pos))
#define RTC_MODE1_INTFLAG_TAMPER_Pos 14           /**< \brief (RTC_MODE1_INTFLAG) Tamper */
#define RTC_MODE1_INTFLAG_TAMPER    (_U_(0x1) << RTC_MODE1_INTFLAG_TAMPER_Pos)
#define RTC_MODE1_INTFLAG_OVF_Pos   15           /**< \brief (RTC_MODE1_INTFLAG) Overflow */
#define RTC_MODE1_INTFLAG_OVF       (_U_(0x1) << RTC_MODE1_INTFLAG_OVF_Pos)
#define RTC_MODE1_INTFLAG_MASK      _U_(0xCFFF)  /**< \brief (RTC_MODE1_INTFLAG) MASK Register */

/* -------- RTC_MODE2_INTFLAG : (RTC Offset: 0x0C) (R/W 16) MODE2 MODE2 Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint16_t PER0:1;           /*!< bit:      0  Periodic Interval 0                */
    __I uint16_t PER1:1;           /*!< bit:      1  Periodic Interval 1                */
    __I uint16_t PER2:1;           /*!< bit:      2  Periodic Interval 2                */
    __I uint16_t PER3:1;           /*!< bit:      3  Periodic Interval 3                */
    __I uint16_t PER4:1;           /*!< bit:      4  Periodic Interval 4                */
    __I uint16_t PER5:1;           /*!< bit:      5  Periodic Interval 5                */
    __I uint16_t PER6:1;           /*!< bit:      6  Periodic Interval 6                */
    __I uint16_t PER7:1;           /*!< bit:      7  Periodic Interval 7                */
    __I uint16_t ALARM0:1;         /*!< bit:      8  Alarm 0                            */
    __I uint16_t ALARM1:1;         /*!< bit:      9  Alarm 1                            */
    __I uint16_t :4;               /*!< bit: 10..13  Reserved                           */
    __I uint16_t TAMPER:1;         /*!< bit:     14  Tamper                             */
    __I uint16_t OVF:1;            /*!< bit:     15  Overflow                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint16_t PER:8;            /*!< bit:  0.. 7  Periodic Interval x                */
    __I uint16_t ALARM:2;          /*!< bit:  8.. 9  Alarm x                            */
    __I uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_INTFLAG_OFFSET    0x0C         /**< \brief (RTC_MODE2_INTFLAG offset) MODE2 Interrupt Flag Status and Clear */
#define RTC_MODE2_INTFLAG_RESETVALUE _U_(0x0000)  /**< \brief (RTC_MODE2_INTFLAG reset_value) MODE2 Interrupt Flag Status and Clear */

#define RTC_MODE2_INTFLAG_PER0_Pos  0            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval 0 */
#define RTC_MODE2_INTFLAG_PER0      (_U_(1) << RTC_MODE2_INTFLAG_PER0_Pos)
#define RTC_MODE2_INTFLAG_PER1_Pos  1            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval 1 */
#define RTC_MODE2_INTFLAG_PER1      (_U_(1) << RTC_MODE2_INTFLAG_PER1_Pos)
#define RTC_MODE2_INTFLAG_PER2_Pos  2            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval 2 */
#define RTC_MODE2_INTFLAG_PER2      (_U_(1) << RTC_MODE2_INTFLAG_PER2_Pos)
#define RTC_MODE2_INTFLAG_PER3_Pos  3            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval 3 */
#define RTC_MODE2_INTFLAG_PER3      (_U_(1) << RTC_MODE2_INTFLAG_PER3_Pos)
#define RTC_MODE2_INTFLAG_PER4_Pos  4            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval 4 */
#define RTC_MODE2_INTFLAG_PER4      (_U_(1) << RTC_MODE2_INTFLAG_PER4_Pos)
#define RTC_MODE2_INTFLAG_PER5_Pos  5            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval 5 */
#define RTC_MODE2_INTFLAG_PER5      (_U_(1) << RTC_MODE2_INTFLAG_PER5_Pos)
#define RTC_MODE2_INTFLAG_PER6_Pos  6            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval 6 */
#define RTC_MODE2_INTFLAG_PER6      (_U_(1) << RTC_MODE2_INTFLAG_PER6_Pos)
#define RTC_MODE2_INTFLAG_PER7_Pos  7            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval 7 */
#define RTC_MODE2_INTFLAG_PER7      (_U_(1) << RTC_MODE2_INTFLAG_PER7_Pos)
#define RTC_MODE2_INTFLAG_PER_Pos   0            /**< \brief (RTC_MODE2_INTFLAG) Periodic Interval x */
#define RTC_MODE2_INTFLAG_PER_Msk   (_U_(0xFF) << RTC_MODE2_INTFLAG_PER_Pos)
#define RTC_MODE2_INTFLAG_PER(value) (RTC_MODE2_INTFLAG_PER_Msk & ((value) << RTC_MODE2_INTFLAG_PER_Pos))
#define RTC_MODE2_INTFLAG_ALARM0_Pos 8            /**< \brief (RTC_MODE2_INTFLAG) Alarm 0 */
#define RTC_MODE2_INTFLAG_ALARM0    (_U_(1) << RTC_MODE2_INTFLAG_ALARM0_Pos)
#define RTC_MODE2_INTFLAG_ALARM1_Pos 9            /**< \brief (RTC_MODE2_INTFLAG) Alarm 1 */
#define RTC_MODE2_INTFLAG_ALARM1    (_U_(1) << RTC_MODE2_INTFLAG_ALARM1_Pos)
#define RTC_MODE2_INTFLAG_ALARM_Pos 8            /**< \brief (RTC_MODE2_INTFLAG) Alarm x */
#define RTC_MODE2_INTFLAG_ALARM_Msk (_U_(0x3) << RTC_MODE2_INTFLAG_ALARM_Pos)
#define RTC_MODE2_INTFLAG_ALARM(value) (RTC_MODE2_INTFLAG_ALARM_Msk & ((value) << RTC_MODE2_INTFLAG_ALARM_Pos))
#define RTC_MODE2_INTFLAG_TAMPER_Pos 14           /**< \brief (RTC_MODE2_INTFLAG) Tamper */
#define RTC_MODE2_INTFLAG_TAMPER    (_U_(0x1) << RTC_MODE2_INTFLAG_TAMPER_Pos)
#define RTC_MODE2_INTFLAG_OVF_Pos   15           /**< \brief (RTC_MODE2_INTFLAG) Overflow */
#define RTC_MODE2_INTFLAG_OVF       (_U_(0x1) << RTC_MODE2_INTFLAG_OVF_Pos)
#define RTC_MODE2_INTFLAG_MASK      _U_(0xC3FF)  /**< \brief (RTC_MODE2_INTFLAG) MASK Register */

/* -------- RTC_DBGCTRL : (RTC Offset: 0x0E) (R/W  8) Debug Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DBGRUN:1;         /*!< bit:      0  Run During Debug                   */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} RTC_DBGCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_DBGCTRL_OFFSET          0x0E         /**< \brief (RTC_DBGCTRL offset) Debug Control */
#define RTC_DBGCTRL_RESETVALUE      _U_(0x00)    /**< \brief (RTC_DBGCTRL reset_value) Debug Control */

#define RTC_DBGCTRL_DBGRUN_Pos      0            /**< \brief (RTC_DBGCTRL) Run During Debug */
#define RTC_DBGCTRL_DBGRUN          (_U_(0x1) << RTC_DBGCTRL_DBGRUN_Pos)
#define RTC_DBGCTRL_MASK            _U_(0x01)    /**< \brief (RTC_DBGCTRL) MASK Register */

/* -------- RTC_MODE0_SYNCBUSY : (RTC Offset: 0x10) (R/  32) MODE0 MODE0 Synchronization Busy Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset Busy                */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable Bit Busy                    */
    uint32_t FREQCORR:1;       /*!< bit:      2  FREQCORR Register Busy             */
    uint32_t COUNT:1;          /*!< bit:      3  COUNT Register Busy                */
    uint32_t :1;               /*!< bit:      4  Reserved                           */
    uint32_t COMP0:1;          /*!< bit:      5  COMP 0 Register Busy               */
    uint32_t COMP1:1;          /*!< bit:      6  COMP 1 Register Busy               */
    uint32_t :8;               /*!< bit:  7..14  Reserved                           */
    uint32_t COUNTSYNC:1;      /*!< bit:     15  Count Synchronization Enable Bit Busy */
    uint32_t GP0:1;            /*!< bit:     16  General Purpose 0 Register Busy    */
    uint32_t GP1:1;            /*!< bit:     17  General Purpose 1 Register Busy    */
    uint32_t GP2:1;            /*!< bit:     18  General Purpose 2 Register Busy    */
    uint32_t GP3:1;            /*!< bit:     19  General Purpose 3 Register Busy    */
    uint32_t :12;              /*!< bit: 20..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :5;               /*!< bit:  0.. 4  Reserved                           */
    uint32_t COMP:2;           /*!< bit:  5.. 6  COMP x Register Busy               */
    uint32_t :9;               /*!< bit:  7..15  Reserved                           */
    uint32_t GP:4;             /*!< bit: 16..19  General Purpose x Register Busy    */
    uint32_t :12;              /*!< bit: 20..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_SYNCBUSY_OFFSET   0x10         /**< \brief (RTC_MODE0_SYNCBUSY offset) MODE0 Synchronization Busy Status */
#define RTC_MODE0_SYNCBUSY_RESETVALUE _U_(0x00000000) /**< \brief (RTC_MODE0_SYNCBUSY reset_value) MODE0 Synchronization Busy Status */

#define RTC_MODE0_SYNCBUSY_SWRST_Pos 0            /**< \brief (RTC_MODE0_SYNCBUSY) Software Reset Busy */
#define RTC_MODE0_SYNCBUSY_SWRST    (_U_(0x1) << RTC_MODE0_SYNCBUSY_SWRST_Pos)
#define RTC_MODE0_SYNCBUSY_ENABLE_Pos 1            /**< \brief (RTC_MODE0_SYNCBUSY) Enable Bit Busy */
#define RTC_MODE0_SYNCBUSY_ENABLE   (_U_(0x1) << RTC_MODE0_SYNCBUSY_ENABLE_Pos)
#define RTC_MODE0_SYNCBUSY_FREQCORR_Pos 2            /**< \brief (RTC_MODE0_SYNCBUSY) FREQCORR Register Busy */
#define RTC_MODE0_SYNCBUSY_FREQCORR (_U_(0x1) << RTC_MODE0_SYNCBUSY_FREQCORR_Pos)
#define RTC_MODE0_SYNCBUSY_COUNT_Pos 3            /**< \brief (RTC_MODE0_SYNCBUSY) COUNT Register Busy */
#define RTC_MODE0_SYNCBUSY_COUNT    (_U_(0x1) << RTC_MODE0_SYNCBUSY_COUNT_Pos)
#define RTC_MODE0_SYNCBUSY_COMP0_Pos 5            /**< \brief (RTC_MODE0_SYNCBUSY) COMP 0 Register Busy */
#define RTC_MODE0_SYNCBUSY_COMP0    (_U_(1) << RTC_MODE0_SYNCBUSY_COMP0_Pos)
#define RTC_MODE0_SYNCBUSY_COMP1_Pos 6            /**< \brief (RTC_MODE0_SYNCBUSY) COMP 1 Register Busy */
#define RTC_MODE0_SYNCBUSY_COMP1    (_U_(1) << RTC_MODE0_SYNCBUSY_COMP1_Pos)
#define RTC_MODE0_SYNCBUSY_COMP_Pos 5            /**< \brief (RTC_MODE0_SYNCBUSY) COMP x Register Busy */
#define RTC_MODE0_SYNCBUSY_COMP_Msk (_U_(0x3) << RTC_MODE0_SYNCBUSY_COMP_Pos)
#define RTC_MODE0_SYNCBUSY_COMP(value) (RTC_MODE0_SYNCBUSY_COMP_Msk & ((value) << RTC_MODE0_SYNCBUSY_COMP_Pos))
#define RTC_MODE0_SYNCBUSY_COUNTSYNC_Pos 15           /**< \brief (RTC_MODE0_SYNCBUSY) Count Synchronization Enable Bit Busy */
#define RTC_MODE0_SYNCBUSY_COUNTSYNC (_U_(0x1) << RTC_MODE0_SYNCBUSY_COUNTSYNC_Pos)
#define RTC_MODE0_SYNCBUSY_GP0_Pos  16           /**< \brief (RTC_MODE0_SYNCBUSY) General Purpose 0 Register Busy */
#define RTC_MODE0_SYNCBUSY_GP0      (_U_(1) << RTC_MODE0_SYNCBUSY_GP0_Pos)
#define RTC_MODE0_SYNCBUSY_GP1_Pos  17           /**< \brief (RTC_MODE0_SYNCBUSY) General Purpose 1 Register Busy */
#define RTC_MODE0_SYNCBUSY_GP1      (_U_(1) << RTC_MODE0_SYNCBUSY_GP1_Pos)
#define RTC_MODE0_SYNCBUSY_GP2_Pos  18           /**< \brief (RTC_MODE0_SYNCBUSY) General Purpose 2 Register Busy */
#define RTC_MODE0_SYNCBUSY_GP2      (_U_(1) << RTC_MODE0_SYNCBUSY_GP2_Pos)
#define RTC_MODE0_SYNCBUSY_GP3_Pos  19           /**< \brief (RTC_MODE0_SYNCBUSY) General Purpose 3 Register Busy */
#define RTC_MODE0_SYNCBUSY_GP3      (_U_(1) << RTC_MODE0_SYNCBUSY_GP3_Pos)
#define RTC_MODE0_SYNCBUSY_GP_Pos   16           /**< \brief (RTC_MODE0_SYNCBUSY) General Purpose x Register Busy */
#define RTC_MODE0_SYNCBUSY_GP_Msk   (_U_(0xF) << RTC_MODE0_SYNCBUSY_GP_Pos)
#define RTC_MODE0_SYNCBUSY_GP(value) (RTC_MODE0_SYNCBUSY_GP_Msk & ((value) << RTC_MODE0_SYNCBUSY_GP_Pos))
#define RTC_MODE0_SYNCBUSY_MASK     _U_(0x000F806F) /**< \brief (RTC_MODE0_SYNCBUSY) MASK Register */

/* -------- RTC_MODE1_SYNCBUSY : (RTC Offset: 0x10) (R/  32) MODE1 MODE1 Synchronization Busy Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset Bit Busy            */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable Bit Busy                    */
    uint32_t FREQCORR:1;       /*!< bit:      2  FREQCORR Register Busy             */
    uint32_t COUNT:1;          /*!< bit:      3  COUNT Register Busy                */
    uint32_t PER:1;            /*!< bit:      4  PER Register Busy                  */
    uint32_t COMP0:1;          /*!< bit:      5  COMP 0 Register Busy               */
    uint32_t COMP1:1;          /*!< bit:      6  COMP 1 Register Busy               */
    uint32_t COMP2:1;          /*!< bit:      7  COMP 2 Register Busy               */
    uint32_t COMP3:1;          /*!< bit:      8  COMP 3 Register Busy               */
    uint32_t :6;               /*!< bit:  9..14  Reserved                           */
    uint32_t COUNTSYNC:1;      /*!< bit:     15  Count Synchronization Enable Bit Busy */
    uint32_t GP0:1;            /*!< bit:     16  General Purpose 0 Register Busy    */
    uint32_t GP1:1;            /*!< bit:     17  General Purpose 1 Register Busy    */
    uint32_t GP2:1;            /*!< bit:     18  General Purpose 2 Register Busy    */
    uint32_t GP3:1;            /*!< bit:     19  General Purpose 3 Register Busy    */
    uint32_t :12;              /*!< bit: 20..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :5;               /*!< bit:  0.. 4  Reserved                           */
    uint32_t COMP:4;           /*!< bit:  5.. 8  COMP x Register Busy               */
    uint32_t :7;               /*!< bit:  9..15  Reserved                           */
    uint32_t GP:4;             /*!< bit: 16..19  General Purpose x Register Busy    */
    uint32_t :12;              /*!< bit: 20..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_SYNCBUSY_OFFSET   0x10         /**< \brief (RTC_MODE1_SYNCBUSY offset) MODE1 Synchronization Busy Status */
#define RTC_MODE1_SYNCBUSY_RESETVALUE _U_(0x00000000) /**< \brief (RTC_MODE1_SYNCBUSY reset_value) MODE1 Synchronization Busy Status */

#define RTC_MODE1_SYNCBUSY_SWRST_Pos 0            /**< \brief (RTC_MODE1_SYNCBUSY) Software Reset Bit Busy */
#define RTC_MODE1_SYNCBUSY_SWRST    (_U_(0x1) << RTC_MODE1_SYNCBUSY_SWRST_Pos)
#define RTC_MODE1_SYNCBUSY_ENABLE_Pos 1            /**< \brief (RTC_MODE1_SYNCBUSY) Enable Bit Busy */
#define RTC_MODE1_SYNCBUSY_ENABLE   (_U_(0x1) << RTC_MODE1_SYNCBUSY_ENABLE_Pos)
#define RTC_MODE1_SYNCBUSY_FREQCORR_Pos 2            /**< \brief (RTC_MODE1_SYNCBUSY) FREQCORR Register Busy */
#define RTC_MODE1_SYNCBUSY_FREQCORR (_U_(0x1) << RTC_MODE1_SYNCBUSY_FREQCORR_Pos)
#define RTC_MODE1_SYNCBUSY_COUNT_Pos 3            /**< \brief (RTC_MODE1_SYNCBUSY) COUNT Register Busy */
#define RTC_MODE1_SYNCBUSY_COUNT    (_U_(0x1) << RTC_MODE1_SYNCBUSY_COUNT_Pos)
#define RTC_MODE1_SYNCBUSY_PER_Pos  4            /**< \brief (RTC_MODE1_SYNCBUSY) PER Register Busy */
#define RTC_MODE1_SYNCBUSY_PER      (_U_(0x1) << RTC_MODE1_SYNCBUSY_PER_Pos)
#define RTC_MODE1_SYNCBUSY_COMP0_Pos 5            /**< \brief (RTC_MODE1_SYNCBUSY) COMP 0 Register Busy */
#define RTC_MODE1_SYNCBUSY_COMP0    (_U_(1) << RTC_MODE1_SYNCBUSY_COMP0_Pos)
#define RTC_MODE1_SYNCBUSY_COMP1_Pos 6            /**< \brief (RTC_MODE1_SYNCBUSY) COMP 1 Register Busy */
#define RTC_MODE1_SYNCBUSY_COMP1    (_U_(1) << RTC_MODE1_SYNCBUSY_COMP1_Pos)
#define RTC_MODE1_SYNCBUSY_COMP2_Pos 7            /**< \brief (RTC_MODE1_SYNCBUSY) COMP 2 Register Busy */
#define RTC_MODE1_SYNCBUSY_COMP2    (_U_(1) << RTC_MODE1_SYNCBUSY_COMP2_Pos)
#define RTC_MODE1_SYNCBUSY_COMP3_Pos 8            /**< \brief (RTC_MODE1_SYNCBUSY) COMP 3 Register Busy */
#define RTC_MODE1_SYNCBUSY_COMP3    (_U_(1) << RTC_MODE1_SYNCBUSY_COMP3_Pos)
#define RTC_MODE1_SYNCBUSY_COMP_Pos 5            /**< \brief (RTC_MODE1_SYNCBUSY) COMP x Register Busy */
#define RTC_MODE1_SYNCBUSY_COMP_Msk (_U_(0xF) << RTC_MODE1_SYNCBUSY_COMP_Pos)
#define RTC_MODE1_SYNCBUSY_COMP(value) (RTC_MODE1_SYNCBUSY_COMP_Msk & ((value) << RTC_MODE1_SYNCBUSY_COMP_Pos))
#define RTC_MODE1_SYNCBUSY_COUNTSYNC_Pos 15           /**< \brief (RTC_MODE1_SYNCBUSY) Count Synchronization Enable Bit Busy */
#define RTC_MODE1_SYNCBUSY_COUNTSYNC (_U_(0x1) << RTC_MODE1_SYNCBUSY_COUNTSYNC_Pos)
#define RTC_MODE1_SYNCBUSY_GP0_Pos  16           /**< \brief (RTC_MODE1_SYNCBUSY) General Purpose 0 Register Busy */
#define RTC_MODE1_SYNCBUSY_GP0      (_U_(1) << RTC_MODE1_SYNCBUSY_GP0_Pos)
#define RTC_MODE1_SYNCBUSY_GP1_Pos  17           /**< \brief (RTC_MODE1_SYNCBUSY) General Purpose 1 Register Busy */
#define RTC_MODE1_SYNCBUSY_GP1      (_U_(1) << RTC_MODE1_SYNCBUSY_GP1_Pos)
#define RTC_MODE1_SYNCBUSY_GP2_Pos  18           /**< \brief (RTC_MODE1_SYNCBUSY) General Purpose 2 Register Busy */
#define RTC_MODE1_SYNCBUSY_GP2      (_U_(1) << RTC_MODE1_SYNCBUSY_GP2_Pos)
#define RTC_MODE1_SYNCBUSY_GP3_Pos  19           /**< \brief (RTC_MODE1_SYNCBUSY) General Purpose 3 Register Busy */
#define RTC_MODE1_SYNCBUSY_GP3      (_U_(1) << RTC_MODE1_SYNCBUSY_GP3_Pos)
#define RTC_MODE1_SYNCBUSY_GP_Pos   16           /**< \brief (RTC_MODE1_SYNCBUSY) General Purpose x Register Busy */
#define RTC_MODE1_SYNCBUSY_GP_Msk   (_U_(0xF) << RTC_MODE1_SYNCBUSY_GP_Pos)
#define RTC_MODE1_SYNCBUSY_GP(value) (RTC_MODE1_SYNCBUSY_GP_Msk & ((value) << RTC_MODE1_SYNCBUSY_GP_Pos))
#define RTC_MODE1_SYNCBUSY_MASK     _U_(0x000F81FF) /**< \brief (RTC_MODE1_SYNCBUSY) MASK Register */

/* -------- RTC_MODE2_SYNCBUSY : (RTC Offset: 0x10) (R/  32) MODE2 MODE2 Synchronization Busy Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset Bit Busy            */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable Bit Busy                    */
    uint32_t FREQCORR:1;       /*!< bit:      2  FREQCORR Register Busy             */
    uint32_t CLOCK:1;          /*!< bit:      3  CLOCK Register Busy                */
    uint32_t :1;               /*!< bit:      4  Reserved                           */
    uint32_t ALARM0:1;         /*!< bit:      5  ALARM 0 Register Busy              */
    uint32_t ALARM1:1;         /*!< bit:      6  ALARM 1 Register Busy              */
    uint32_t :4;               /*!< bit:  7..10  Reserved                           */
    uint32_t MASK0:1;          /*!< bit:     11  MASK 0 Register Busy               */
    uint32_t MASK1:1;          /*!< bit:     12  MASK 1 Register Busy               */
    uint32_t :2;               /*!< bit: 13..14  Reserved                           */
    uint32_t CLOCKSYNC:1;      /*!< bit:     15  Clock Synchronization Enable Bit Busy */
    uint32_t GP0:1;            /*!< bit:     16  General Purpose 0 Register Busy    */
    uint32_t GP1:1;            /*!< bit:     17  General Purpose 1 Register Busy    */
    uint32_t GP2:1;            /*!< bit:     18  General Purpose 2 Register Busy    */
    uint32_t GP3:1;            /*!< bit:     19  General Purpose 3 Register Busy    */
    uint32_t :12;              /*!< bit: 20..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :5;               /*!< bit:  0.. 4  Reserved                           */
    uint32_t ALARM:2;          /*!< bit:  5.. 6  ALARM x Register Busy              */
    uint32_t :4;               /*!< bit:  7..10  Reserved                           */
    uint32_t MASK:2;           /*!< bit: 11..12  MASK x Register Busy               */
    uint32_t :3;               /*!< bit: 13..15  Reserved                           */
    uint32_t GP:4;             /*!< bit: 16..19  General Purpose x Register Busy    */
    uint32_t :12;              /*!< bit: 20..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_SYNCBUSY_OFFSET   0x10         /**< \brief (RTC_MODE2_SYNCBUSY offset) MODE2 Synchronization Busy Status */
#define RTC_MODE2_SYNCBUSY_RESETVALUE _U_(0x00000000) /**< \brief (RTC_MODE2_SYNCBUSY reset_value) MODE2 Synchronization Busy Status */

#define RTC_MODE2_SYNCBUSY_SWRST_Pos 0            /**< \brief (RTC_MODE2_SYNCBUSY) Software Reset Bit Busy */
#define RTC_MODE2_SYNCBUSY_SWRST    (_U_(0x1) << RTC_MODE2_SYNCBUSY_SWRST_Pos)
#define RTC_MODE2_SYNCBUSY_ENABLE_Pos 1            /**< \brief (RTC_MODE2_SYNCBUSY) Enable Bit Busy */
#define RTC_MODE2_SYNCBUSY_ENABLE   (_U_(0x1) << RTC_MODE2_SYNCBUSY_ENABLE_Pos)
#define RTC_MODE2_SYNCBUSY_FREQCORR_Pos 2            /**< \brief (RTC_MODE2_SYNCBUSY) FREQCORR Register Busy */
#define RTC_MODE2_SYNCBUSY_FREQCORR (_U_(0x1) << RTC_MODE2_SYNCBUSY_FREQCORR_Pos)
#define RTC_MODE2_SYNCBUSY_CLOCK_Pos 3            /**< \brief (RTC_MODE2_SYNCBUSY) CLOCK Register Busy */
#define RTC_MODE2_SYNCBUSY_CLOCK    (_U_(0x1) << RTC_MODE2_SYNCBUSY_CLOCK_Pos)
#define RTC_MODE2_SYNCBUSY_ALARM0_Pos 5            /**< \brief (RTC_MODE2_SYNCBUSY) ALARM 0 Register Busy */
#define RTC_MODE2_SYNCBUSY_ALARM0   (_U_(1) << RTC_MODE2_SYNCBUSY_ALARM0_Pos)
#define RTC_MODE2_SYNCBUSY_ALARM1_Pos 6            /**< \brief (RTC_MODE2_SYNCBUSY) ALARM 1 Register Busy */
#define RTC_MODE2_SYNCBUSY_ALARM1   (_U_(1) << RTC_MODE2_SYNCBUSY_ALARM1_Pos)
#define RTC_MODE2_SYNCBUSY_ALARM_Pos 5            /**< \brief (RTC_MODE2_SYNCBUSY) ALARM x Register Busy */
#define RTC_MODE2_SYNCBUSY_ALARM_Msk (_U_(0x3) << RTC_MODE2_SYNCBUSY_ALARM_Pos)
#define RTC_MODE2_SYNCBUSY_ALARM(value) (RTC_MODE2_SYNCBUSY_ALARM_Msk & ((value) << RTC_MODE2_SYNCBUSY_ALARM_Pos))
#define RTC_MODE2_SYNCBUSY_MASK0_Pos 11           /**< \brief (RTC_MODE2_SYNCBUSY) MASK 0 Register Busy */
#define RTC_MODE2_SYNCBUSY_MASK0    (_U_(1) << RTC_MODE2_SYNCBUSY_MASK0_Pos)
#define RTC_MODE2_SYNCBUSY_MASK1_Pos 12           /**< \brief (RTC_MODE2_SYNCBUSY) MASK 1 Register Busy */
#define RTC_MODE2_SYNCBUSY_MASK1    (_U_(1) << RTC_MODE2_SYNCBUSY_MASK1_Pos)
#define RTC_MODE2_SYNCBUSY_MASK_Pos 11           /**< \brief (RTC_MODE2_SYNCBUSY) MASK x Register Busy */
#define RTC_MODE2_SYNCBUSY_MASK_Msk (_U_(0x3) << RTC_MODE2_SYNCBUSY_MASK_Pos)
#define RTC_MODE2_SYNCBUSY_MASK(value) (RTC_MODE2_SYNCBUSY_MASK_Msk & ((value) << RTC_MODE2_SYNCBUSY_MASK_Pos))
#define RTC_MODE2_SYNCBUSY_CLOCKSYNC_Pos 15           /**< \brief (RTC_MODE2_SYNCBUSY) Clock Synchronization Enable Bit Busy */
#define RTC_MODE2_SYNCBUSY_CLOCKSYNC (_U_(0x1) << RTC_MODE2_SYNCBUSY_CLOCKSYNC_Pos)
#define RTC_MODE2_SYNCBUSY_GP0_Pos  16           /**< \brief (RTC_MODE2_SYNCBUSY) General Purpose 0 Register Busy */
#define RTC_MODE2_SYNCBUSY_GP0      (_U_(1) << RTC_MODE2_SYNCBUSY_GP0_Pos)
#define RTC_MODE2_SYNCBUSY_GP1_Pos  17           /**< \brief (RTC_MODE2_SYNCBUSY) General Purpose 1 Register Busy */
#define RTC_MODE2_SYNCBUSY_GP1      (_U_(1) << RTC_MODE2_SYNCBUSY_GP1_Pos)
#define RTC_MODE2_SYNCBUSY_GP2_Pos  18           /**< \brief (RTC_MODE2_SYNCBUSY) General Purpose 2 Register Busy */
#define RTC_MODE2_SYNCBUSY_GP2      (_U_(1) << RTC_MODE2_SYNCBUSY_GP2_Pos)
#define RTC_MODE2_SYNCBUSY_GP3_Pos  19           /**< \brief (RTC_MODE2_SYNCBUSY) General Purpose 3 Register Busy */
#define RTC_MODE2_SYNCBUSY_GP3      (_U_(1) << RTC_MODE2_SYNCBUSY_GP3_Pos)
#define RTC_MODE2_SYNCBUSY_GP_Pos   16           /**< \brief (RTC_MODE2_SYNCBUSY) General Purpose x Register Busy */
#define RTC_MODE2_SYNCBUSY_GP_Msk   (_U_(0xF) << RTC_MODE2_SYNCBUSY_GP_Pos)
#define RTC_MODE2_SYNCBUSY_GP(value) (RTC_MODE2_SYNCBUSY_GP_Msk & ((value) << RTC_MODE2_SYNCBUSY_GP_Pos))
#define RTC_MODE2_SYNCBUSY_MASK_    _U_(0x000F986F) /**< \brief (RTC_MODE2_SYNCBUSY) MASK Register */

/* -------- RTC_FREQCORR : (RTC Offset: 0x14) (R/W  8) Frequency Correction -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  VALUE:7;          /*!< bit:  0.. 6  Correction Value                   */
    uint8_t  SIGN:1;           /*!< bit:      7  Correction Sign                    */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} RTC_FREQCORR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_FREQCORR_OFFSET         0x14         /**< \brief (RTC_FREQCORR offset) Frequency Correction */
#define RTC_FREQCORR_RESETVALUE     _U_(0x00)    /**< \brief (RTC_FREQCORR reset_value) Frequency Correction */

#define RTC_FREQCORR_VALUE_Pos      0            /**< \brief (RTC_FREQCORR) Correction Value */
#define RTC_FREQCORR_VALUE_Msk      (_U_(0x7F) << RTC_FREQCORR_VALUE_Pos)
#define RTC_FREQCORR_VALUE(value)   (RTC_FREQCORR_VALUE_Msk & ((value) << RTC_FREQCORR_VALUE_Pos))
#define RTC_FREQCORR_SIGN_Pos       7            /**< \brief (RTC_FREQCORR) Correction Sign */
#define RTC_FREQCORR_SIGN           (_U_(0x1) << RTC_FREQCORR_SIGN_Pos)
#define RTC_FREQCORR_MASK           _U_(0xFF)    /**< \brief (RTC_FREQCORR) MASK Register */

/* -------- RTC_MODE0_COUNT : (RTC Offset: 0x18) (R/W 32) MODE0 MODE0 Counter Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t COUNT:32;         /*!< bit:  0..31  Counter Value                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_COUNT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_COUNT_OFFSET      0x18         /**< \brief (RTC_MODE0_COUNT offset) MODE0 Counter Value */
#define RTC_MODE0_COUNT_RESETVALUE  _U_(0x00000000) /**< \brief (RTC_MODE0_COUNT reset_value) MODE0 Counter Value */

#define RTC_MODE0_COUNT_COUNT_Pos   0            /**< \brief (RTC_MODE0_COUNT) Counter Value */
#define RTC_MODE0_COUNT_COUNT_Msk   (_U_(0xFFFFFFFF) << RTC_MODE0_COUNT_COUNT_Pos)
#define RTC_MODE0_COUNT_COUNT(value) (RTC_MODE0_COUNT_COUNT_Msk & ((value) << RTC_MODE0_COUNT_COUNT_Pos))
#define RTC_MODE0_COUNT_MASK        _U_(0xFFFFFFFF) /**< \brief (RTC_MODE0_COUNT) MASK Register */

/* -------- RTC_MODE1_COUNT : (RTC Offset: 0x18) (R/W 16) MODE1 MODE1 Counter Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t COUNT:16;         /*!< bit:  0..15  Counter Value                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_COUNT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_COUNT_OFFSET      0x18         /**< \brief (RTC_MODE1_COUNT offset) MODE1 Counter Value */
#define RTC_MODE1_COUNT_RESETVALUE  _U_(0x0000)  /**< \brief (RTC_MODE1_COUNT reset_value) MODE1 Counter Value */

#define RTC_MODE1_COUNT_COUNT_Pos   0            /**< \brief (RTC_MODE1_COUNT) Counter Value */
#define RTC_MODE1_COUNT_COUNT_Msk   (_U_(0xFFFF) << RTC_MODE1_COUNT_COUNT_Pos)
#define RTC_MODE1_COUNT_COUNT(value) (RTC_MODE1_COUNT_COUNT_Msk & ((value) << RTC_MODE1_COUNT_COUNT_Pos))
#define RTC_MODE1_COUNT_MASK        _U_(0xFFFF)  /**< \brief (RTC_MODE1_COUNT) MASK Register */

/* -------- RTC_MODE2_CLOCK : (RTC Offset: 0x18) (R/W 32) MODE2 MODE2 Clock Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SECOND:6;         /*!< bit:  0.. 5  Second                             */
    uint32_t MINUTE:6;         /*!< bit:  6..11  Minute                             */
    uint32_t HOUR:5;           /*!< bit: 12..16  Hour                               */
    uint32_t DAY:5;            /*!< bit: 17..21  Day                                */
    uint32_t MONTH:4;          /*!< bit: 22..25  Month                              */
    uint32_t YEAR:6;           /*!< bit: 26..31  Year                               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_CLOCK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_CLOCK_OFFSET      0x18         /**< \brief (RTC_MODE2_CLOCK offset) MODE2 Clock Value */
#define RTC_MODE2_CLOCK_RESETVALUE  _U_(0x00000000) /**< \brief (RTC_MODE2_CLOCK reset_value) MODE2 Clock Value */

#define RTC_MODE2_CLOCK_SECOND_Pos  0            /**< \brief (RTC_MODE2_CLOCK) Second */
#define RTC_MODE2_CLOCK_SECOND_Msk  (_U_(0x3F) << RTC_MODE2_CLOCK_SECOND_Pos)
#define RTC_MODE2_CLOCK_SECOND(value) (RTC_MODE2_CLOCK_SECOND_Msk & ((value) << RTC_MODE2_CLOCK_SECOND_Pos))
#define RTC_MODE2_CLOCK_MINUTE_Pos  6            /**< \brief (RTC_MODE2_CLOCK) Minute */
#define RTC_MODE2_CLOCK_MINUTE_Msk  (_U_(0x3F) << RTC_MODE2_CLOCK_MINUTE_Pos)
#define RTC_MODE2_CLOCK_MINUTE(value) (RTC_MODE2_CLOCK_MINUTE_Msk & ((value) << RTC_MODE2_CLOCK_MINUTE_Pos))
#define RTC_MODE2_CLOCK_HOUR_Pos    12           /**< \brief (RTC_MODE2_CLOCK) Hour */
#define RTC_MODE2_CLOCK_HOUR_Msk    (_U_(0x1F) << RTC_MODE2_CLOCK_HOUR_Pos)
#define RTC_MODE2_CLOCK_HOUR(value) (RTC_MODE2_CLOCK_HOUR_Msk & ((value) << RTC_MODE2_CLOCK_HOUR_Pos))
#define   RTC_MODE2_CLOCK_HOUR_AM_Val     _U_(0x0)   /**< \brief (RTC_MODE2_CLOCK) AM when CLKREP in 12-hour */
#define   RTC_MODE2_CLOCK_HOUR_PM_Val     _U_(0x10)   /**< \brief (RTC_MODE2_CLOCK) PM when CLKREP in 12-hour */
#define RTC_MODE2_CLOCK_HOUR_AM     (RTC_MODE2_CLOCK_HOUR_AM_Val   << RTC_MODE2_CLOCK_HOUR_Pos)
#define RTC_MODE2_CLOCK_HOUR_PM     (RTC_MODE2_CLOCK_HOUR_PM_Val   << RTC_MODE2_CLOCK_HOUR_Pos)
#define RTC_MODE2_CLOCK_DAY_Pos     17           /**< \brief (RTC_MODE2_CLOCK) Day */
#define RTC_MODE2_CLOCK_DAY_Msk     (_U_(0x1F) << RTC_MODE2_CLOCK_DAY_Pos)
#define RTC_MODE2_CLOCK_DAY(value)  (RTC_MODE2_CLOCK_DAY_Msk & ((value) << RTC_MODE2_CLOCK_DAY_Pos))
#define RTC_MODE2_CLOCK_MONTH_Pos   22           /**< \brief (RTC_MODE2_CLOCK) Month */
#define RTC_MODE2_CLOCK_MONTH_Msk   (_U_(0xF) << RTC_MODE2_CLOCK_MONTH_Pos)
#define RTC_MODE2_CLOCK_MONTH(value) (RTC_MODE2_CLOCK_MONTH_Msk & ((value) << RTC_MODE2_CLOCK_MONTH_Pos))
#define RTC_MODE2_CLOCK_YEAR_Pos    26           /**< \brief (RTC_MODE2_CLOCK) Year */
#define RTC_MODE2_CLOCK_YEAR_Msk    (_U_(0x3F) << RTC_MODE2_CLOCK_YEAR_Pos)
#define RTC_MODE2_CLOCK_YEAR(value) (RTC_MODE2_CLOCK_YEAR_Msk & ((value) << RTC_MODE2_CLOCK_YEAR_Pos))
#define RTC_MODE2_CLOCK_MASK        _U_(0xFFFFFFFF) /**< \brief (RTC_MODE2_CLOCK) MASK Register */

/* -------- RTC_MODE1_PER : (RTC Offset: 0x1C) (R/W 16) MODE1 MODE1 Counter Period -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t PER:16;           /*!< bit:  0..15  Counter Period                     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_PER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_PER_OFFSET        0x1C         /**< \brief (RTC_MODE1_PER offset) MODE1 Counter Period */
#define RTC_MODE1_PER_RESETVALUE    _U_(0x0000)  /**< \brief (RTC_MODE1_PER reset_value) MODE1 Counter Period */

#define RTC_MODE1_PER_PER_Pos       0            /**< \brief (RTC_MODE1_PER) Counter Period */
#define RTC_MODE1_PER_PER_Msk       (_U_(0xFFFF) << RTC_MODE1_PER_PER_Pos)
#define RTC_MODE1_PER_PER(value)    (RTC_MODE1_PER_PER_Msk & ((value) << RTC_MODE1_PER_PER_Pos))
#define RTC_MODE1_PER_MASK          _U_(0xFFFF)  /**< \brief (RTC_MODE1_PER) MASK Register */

/* -------- RTC_MODE0_COMP : (RTC Offset: 0x20) (R/W 32) MODE0 MODE0 Compare n Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t COMP:32;          /*!< bit:  0..31  Compare Value                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_COMP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_COMP_OFFSET       0x20         /**< \brief (RTC_MODE0_COMP offset) MODE0 Compare n Value */
#define RTC_MODE0_COMP_RESETVALUE   _U_(0x00000000) /**< \brief (RTC_MODE0_COMP reset_value) MODE0 Compare n Value */

#define RTC_MODE0_COMP_COMP_Pos     0            /**< \brief (RTC_MODE0_COMP) Compare Value */
#define RTC_MODE0_COMP_COMP_Msk     (_U_(0xFFFFFFFF) << RTC_MODE0_COMP_COMP_Pos)
#define RTC_MODE0_COMP_COMP(value)  (RTC_MODE0_COMP_COMP_Msk & ((value) << RTC_MODE0_COMP_COMP_Pos))
#define RTC_MODE0_COMP_MASK         _U_(0xFFFFFFFF) /**< \brief (RTC_MODE0_COMP) MASK Register */

/* -------- RTC_MODE1_COMP : (RTC Offset: 0x20) (R/W 16) MODE1 MODE1 Compare n Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t COMP:16;          /*!< bit:  0..15  Compare Value                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_COMP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_COMP_OFFSET       0x20         /**< \brief (RTC_MODE1_COMP offset) MODE1 Compare n Value */
#define RTC_MODE1_COMP_RESETVALUE   _U_(0x0000)  /**< \brief (RTC_MODE1_COMP reset_value) MODE1 Compare n Value */

#define RTC_MODE1_COMP_COMP_Pos     0            /**< \brief (RTC_MODE1_COMP) Compare Value */
#define RTC_MODE1_COMP_COMP_Msk     (_U_(0xFFFF) << RTC_MODE1_COMP_COMP_Pos)
#define RTC_MODE1_COMP_COMP(value)  (RTC_MODE1_COMP_COMP_Msk & ((value) << RTC_MODE1_COMP_COMP_Pos))
#define RTC_MODE1_COMP_MASK         _U_(0xFFFF)  /**< \brief (RTC_MODE1_COMP) MASK Register */

/* -------- RTC_MODE2_ALARM : (RTC Offset: 0x20) (R/W 32) MODE2 MODE2_ALARM Alarm n Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SECOND:6;         /*!< bit:  0.. 5  Second                             */
    uint32_t MINUTE:6;         /*!< bit:  6..11  Minute                             */
    uint32_t HOUR:5;           /*!< bit: 12..16  Hour                               */
    uint32_t DAY:5;            /*!< bit: 17..21  Day                                */
    uint32_t MONTH:4;          /*!< bit: 22..25  Month                              */
    uint32_t YEAR:6;           /*!< bit: 26..31  Year                               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_ALARM_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_ALARM_OFFSET      0x20         /**< \brief (RTC_MODE2_ALARM offset) MODE2_ALARM Alarm n Value */
#define RTC_MODE2_ALARM_RESETVALUE  _U_(0x00000000) /**< \brief (RTC_MODE2_ALARM reset_value) MODE2_ALARM Alarm n Value */

#define RTC_MODE2_ALARM_SECOND_Pos  0            /**< \brief (RTC_MODE2_ALARM) Second */
#define RTC_MODE2_ALARM_SECOND_Msk  (_U_(0x3F) << RTC_MODE2_ALARM_SECOND_Pos)
#define RTC_MODE2_ALARM_SECOND(value) (RTC_MODE2_ALARM_SECOND_Msk & ((value) << RTC_MODE2_ALARM_SECOND_Pos))
#define RTC_MODE2_ALARM_MINUTE_Pos  6            /**< \brief (RTC_MODE2_ALARM) Minute */
#define RTC_MODE2_ALARM_MINUTE_Msk  (_U_(0x3F) << RTC_MODE2_ALARM_MINUTE_Pos)
#define RTC_MODE2_ALARM_MINUTE(value) (RTC_MODE2_ALARM_MINUTE_Msk & ((value) << RTC_MODE2_ALARM_MINUTE_Pos))
#define RTC_MODE2_ALARM_HOUR_Pos    12           /**< \brief (RTC_MODE2_ALARM) Hour */
#define RTC_MODE2_ALARM_HOUR_Msk    (_U_(0x1F) << RTC_MODE2_ALARM_HOUR_Pos)
#define RTC_MODE2_ALARM_HOUR(value) (RTC_MODE2_ALARM_HOUR_Msk & ((value) << RTC_MODE2_ALARM_HOUR_Pos))
#define   RTC_MODE2_ALARM_HOUR_AM_Val     _U_(0x0)   /**< \brief (RTC_MODE2_ALARM) Morning hour */
#define   RTC_MODE2_ALARM_HOUR_PM_Val     _U_(0x10)   /**< \brief (RTC_MODE2_ALARM) Afternoon hour */
#define RTC_MODE2_ALARM_HOUR_AM     (RTC_MODE2_ALARM_HOUR_AM_Val   << RTC_MODE2_ALARM_HOUR_Pos)
#define RTC_MODE2_ALARM_HOUR_PM     (RTC_MODE2_ALARM_HOUR_PM_Val   << RTC_MODE2_ALARM_HOUR_Pos)
#define RTC_MODE2_ALARM_DAY_Pos     17           /**< \brief (RTC_MODE2_ALARM) Day */
#define RTC_MODE2_ALARM_DAY_Msk     (_U_(0x1F) << RTC_MODE2_ALARM_DAY_Pos)
#define RTC_MODE2_ALARM_DAY(value)  (RTC_MODE2_ALARM_DAY_Msk & ((value) << RTC_MODE2_ALARM_DAY_Pos))
#define RTC_MODE2_ALARM_MONTH_Pos   22           /**< \brief (RTC_MODE2_ALARM) Month */
#define RTC_MODE2_ALARM_MONTH_Msk   (_U_(0xF) << RTC_MODE2_ALARM_MONTH_Pos)
#define RTC_MODE2_ALARM_MONTH(value) (RTC_MODE2_ALARM_MONTH_Msk & ((value) << RTC_MODE2_ALARM_MONTH_Pos))
#define RTC_MODE2_ALARM_YEAR_Pos    26           /**< \brief (RTC_MODE2_ALARM) Year */
#define RTC_MODE2_ALARM_YEAR_Msk    (_U_(0x3F) << RTC_MODE2_ALARM_YEAR_Pos)
#define RTC_MODE2_ALARM_YEAR(value) (RTC_MODE2_ALARM_YEAR_Msk & ((value) << RTC_MODE2_ALARM_YEAR_Pos))
#define RTC_MODE2_ALARM_MASK        _U_(0xFFFFFFFF) /**< \brief (RTC_MODE2_ALARM) MASK Register */

/* -------- RTC_MODE2_MASK : (RTC Offset: 0x24) (R/W  8) MODE2 MODE2_ALARM Alarm n Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SEL:3;            /*!< bit:  0.. 2  Alarm Mask Selection               */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} RTC_MODE2_MASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_MASK_OFFSET       0x24         /**< \brief (RTC_MODE2_MASK offset) MODE2_ALARM Alarm n Mask */
#define RTC_MODE2_MASK_RESETVALUE   _U_(0x00)    /**< \brief (RTC_MODE2_MASK reset_value) MODE2_ALARM Alarm n Mask */

#define RTC_MODE2_MASK_SEL_Pos      0            /**< \brief (RTC_MODE2_MASK) Alarm Mask Selection */
#define RTC_MODE2_MASK_SEL_Msk      (_U_(0x7) << RTC_MODE2_MASK_SEL_Pos)
#define RTC_MODE2_MASK_SEL(value)   (RTC_MODE2_MASK_SEL_Msk & ((value) << RTC_MODE2_MASK_SEL_Pos))
#define   RTC_MODE2_MASK_SEL_OFF_Val      _U_(0x0)   /**< \brief (RTC_MODE2_MASK) Alarm Disabled */
#define   RTC_MODE2_MASK_SEL_SS_Val       _U_(0x1)   /**< \brief (RTC_MODE2_MASK) Match seconds only */
#define   RTC_MODE2_MASK_SEL_MMSS_Val     _U_(0x2)   /**< \brief (RTC_MODE2_MASK) Match seconds and minutes only */
#define   RTC_MODE2_MASK_SEL_HHMMSS_Val   _U_(0x3)   /**< \brief (RTC_MODE2_MASK) Match seconds, minutes, and hours only */
#define   RTC_MODE2_MASK_SEL_DDHHMMSS_Val _U_(0x4)   /**< \brief (RTC_MODE2_MASK) Match seconds, minutes, hours, and days only */
#define   RTC_MODE2_MASK_SEL_MMDDHHMMSS_Val _U_(0x5)   /**< \brief (RTC_MODE2_MASK) Match seconds, minutes, hours, days, and months only */
#define   RTC_MODE2_MASK_SEL_YYMMDDHHMMSS_Val _U_(0x6)   /**< \brief (RTC_MODE2_MASK) Match seconds, minutes, hours, days, months, and years */
#define RTC_MODE2_MASK_SEL_OFF      (RTC_MODE2_MASK_SEL_OFF_Val    << RTC_MODE2_MASK_SEL_Pos)
#define RTC_MODE2_MASK_SEL_SS       (RTC_MODE2_MASK_SEL_SS_Val     << RTC_MODE2_MASK_SEL_Pos)
#define RTC_MODE2_MASK_SEL_MMSS     (RTC_MODE2_MASK_SEL_MMSS_Val   << RTC_MODE2_MASK_SEL_Pos)
#define RTC_MODE2_MASK_SEL_HHMMSS   (RTC_MODE2_MASK_SEL_HHMMSS_Val << RTC_MODE2_MASK_SEL_Pos)
#define RTC_MODE2_MASK_SEL_DDHHMMSS (RTC_MODE2_MASK_SEL_DDHHMMSS_Val << RTC_MODE2_MASK_SEL_Pos)
#define RTC_MODE2_MASK_SEL_MMDDHHMMSS (RTC_MODE2_MASK_SEL_MMDDHHMMSS_Val << RTC_MODE2_MASK_SEL_Pos)
#define RTC_MODE2_MASK_SEL_YYMMDDHHMMSS (RTC_MODE2_MASK_SEL_YYMMDDHHMMSS_Val << RTC_MODE2_MASK_SEL_Pos)
#define RTC_MODE2_MASK_MASK         _U_(0x07)    /**< \brief (RTC_MODE2_MASK) MASK Register */

/* -------- RTC_GP : (RTC Offset: 0x40) (R/W 32) General Purpose -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t GP:32;            /*!< bit:  0..31  General Purpose                    */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_GP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_GP_OFFSET               0x40         /**< \brief (RTC_GP offset) General Purpose */
#define RTC_GP_RESETVALUE           _U_(0x00000000) /**< \brief (RTC_GP reset_value) General Purpose */

#define RTC_GP_GP_Pos               0            /**< \brief (RTC_GP) General Purpose */
#define RTC_GP_GP_Msk               (_U_(0xFFFFFFFF) << RTC_GP_GP_Pos)
#define RTC_GP_GP(value)            (RTC_GP_GP_Msk & ((value) << RTC_GP_GP_Pos))
#define RTC_GP_MASK                 _U_(0xFFFFFFFF) /**< \brief (RTC_GP) MASK Register */

/* -------- RTC_TAMPCTRL : (RTC Offset: 0x60) (R/W 32) Tamper Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t IN0ACT:2;         /*!< bit:  0.. 1  Tamper Input 0 Action              */
    uint32_t IN1ACT:2;         /*!< bit:  2.. 3  Tamper Input 1 Action              */
    uint32_t IN2ACT:2;         /*!< bit:  4.. 5  Tamper Input 2 Action              */
    uint32_t IN3ACT:2;         /*!< bit:  6.. 7  Tamper Input 3 Action              */
    uint32_t IN4ACT:2;         /*!< bit:  8.. 9  Tamper Input 4 Action              */
    uint32_t :6;               /*!< bit: 10..15  Reserved                           */
    uint32_t TAMLVL0:1;        /*!< bit:     16  Tamper Level Select 0              */
    uint32_t TAMLVL1:1;        /*!< bit:     17  Tamper Level Select 1              */
    uint32_t TAMLVL2:1;        /*!< bit:     18  Tamper Level Select 2              */
    uint32_t TAMLVL3:1;        /*!< bit:     19  Tamper Level Select 3              */
    uint32_t TAMLVL4:1;        /*!< bit:     20  Tamper Level Select 4              */
    uint32_t :3;               /*!< bit: 21..23  Reserved                           */
    uint32_t DEBNC0:1;         /*!< bit:     24  Debouncer Enable 0                 */
    uint32_t DEBNC1:1;         /*!< bit:     25  Debouncer Enable 1                 */
    uint32_t DEBNC2:1;         /*!< bit:     26  Debouncer Enable 2                 */
    uint32_t DEBNC3:1;         /*!< bit:     27  Debouncer Enable 3                 */
    uint32_t DEBNC4:1;         /*!< bit:     28  Debouncer Enable 4                 */
    uint32_t :3;               /*!< bit: 29..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :16;              /*!< bit:  0..15  Reserved                           */
    uint32_t TAMLVL:5;         /*!< bit: 16..20  Tamper Level Select x              */
    uint32_t :3;               /*!< bit: 21..23  Reserved                           */
    uint32_t DEBNC:5;          /*!< bit: 24..28  Debouncer Enable x                 */
    uint32_t :3;               /*!< bit: 29..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_TAMPCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_TAMPCTRL_OFFSET         0x60         /**< \brief (RTC_TAMPCTRL offset) Tamper Control */
#define RTC_TAMPCTRL_RESETVALUE     _U_(0x00000000) /**< \brief (RTC_TAMPCTRL reset_value) Tamper Control */

#define RTC_TAMPCTRL_IN0ACT_Pos     0            /**< \brief (RTC_TAMPCTRL) Tamper Input 0 Action */
#define RTC_TAMPCTRL_IN0ACT_Msk     (_U_(0x3) << RTC_TAMPCTRL_IN0ACT_Pos)
#define RTC_TAMPCTRL_IN0ACT(value)  (RTC_TAMPCTRL_IN0ACT_Msk & ((value) << RTC_TAMPCTRL_IN0ACT_Pos))
#define   RTC_TAMPCTRL_IN0ACT_OFF_Val     _U_(0x0)   /**< \brief (RTC_TAMPCTRL) Off (Disabled) */
#define   RTC_TAMPCTRL_IN0ACT_WAKE_Val    _U_(0x1)   /**< \brief (RTC_TAMPCTRL) Wake without timestamp */
#define   RTC_TAMPCTRL_IN0ACT_CAPTURE_Val _U_(0x2)   /**< \brief (RTC_TAMPCTRL) Capture timestamp */
#define   RTC_TAMPCTRL_IN0ACT_ACTL_Val    _U_(0x3)   /**< \brief (RTC_TAMPCTRL) Compare IN0 to OUT */
#define RTC_TAMPCTRL_IN0ACT_OFF     (RTC_TAMPCTRL_IN0ACT_OFF_Val   << RTC_TAMPCTRL_IN0ACT_Pos)
#define RTC_TAMPCTRL_IN0ACT_WAKE    (RTC_TAMPCTRL_IN0ACT_WAKE_Val  << RTC_TAMPCTRL_IN0ACT_Pos)
#define RTC_TAMPCTRL_IN0ACT_CAPTURE (RTC_TAMPCTRL_IN0ACT_CAPTURE_Val << RTC_TAMPCTRL_IN0ACT_Pos)
#define RTC_TAMPCTRL_IN0ACT_ACTL    (RTC_TAMPCTRL_IN0ACT_ACTL_Val  << RTC_TAMPCTRL_IN0ACT_Pos)
#define RTC_TAMPCTRL_IN1ACT_Pos     2            /**< \brief (RTC_TAMPCTRL) Tamper Input 1 Action */
#define RTC_TAMPCTRL_IN1ACT_Msk     (_U_(0x3) << RTC_TAMPCTRL_IN1ACT_Pos)
#define RTC_TAMPCTRL_IN1ACT(value)  (RTC_TAMPCTRL_IN1ACT_Msk & ((value) << RTC_TAMPCTRL_IN1ACT_Pos))
#define   RTC_TAMPCTRL_IN1ACT_OFF_Val     _U_(0x0)   /**< \brief (RTC_TAMPCTRL) Off (Disabled) */
#define   RTC_TAMPCTRL_IN1ACT_WAKE_Val    _U_(0x1)   /**< \brief (RTC_TAMPCTRL) Wake without timestamp */
#define   RTC_TAMPCTRL_IN1ACT_CAPTURE_Val _U_(0x2)   /**< \brief (RTC_TAMPCTRL) Capture timestamp */
#define   RTC_TAMPCTRL_IN1ACT_ACTL_Val    _U_(0x3)   /**< \brief (RTC_TAMPCTRL) Compare IN1 to OUT */
#define RTC_TAMPCTRL_IN1ACT_OFF     (RTC_TAMPCTRL_IN1ACT_OFF_Val   << RTC_TAMPCTRL_IN1ACT_Pos)
#define RTC_TAMPCTRL_IN1ACT_WAKE    (RTC_TAMPCTRL_IN1ACT_WAKE_Val  << RTC_TAMPCTRL_IN1ACT_Pos)
#define RTC_TAMPCTRL_IN1ACT_CAPTURE (RTC_TAMPCTRL_IN1ACT_CAPTURE_Val << RTC_TAMPCTRL_IN1ACT_Pos)
#define RTC_TAMPCTRL_IN1ACT_ACTL    (RTC_TAMPCTRL_IN1ACT_ACTL_Val  << RTC_TAMPCTRL_IN1ACT_Pos)
#define RTC_TAMPCTRL_IN2ACT_Pos     4            /**< \brief (RTC_TAMPCTRL) Tamper Input 2 Action */
#define RTC_TAMPCTRL_IN2ACT_Msk     (_U_(0x3) << RTC_TAMPCTRL_IN2ACT_Pos)
#define RTC_TAMPCTRL_IN2ACT(value)  (RTC_TAMPCTRL_IN2ACT_Msk & ((value) << RTC_TAMPCTRL_IN2ACT_Pos))
#define   RTC_TAMPCTRL_IN2ACT_OFF_Val     _U_(0x0)   /**< \brief (RTC_TAMPCTRL) Off (Disabled) */
#define   RTC_TAMPCTRL_IN2ACT_WAKE_Val    _U_(0x1)   /**< \brief (RTC_TAMPCTRL) Wake without timestamp */
#define   RTC_TAMPCTRL_IN2ACT_CAPTURE_Val _U_(0x2)   /**< \brief (RTC_TAMPCTRL) Capture timestamp */
#define   RTC_TAMPCTRL_IN2ACT_ACTL_Val    _U_(0x3)   /**< \brief (RTC_TAMPCTRL) Compare IN2 to OUT */
#define RTC_TAMPCTRL_IN2ACT_OFF     (RTC_TAMPCTRL_IN2ACT_OFF_Val   << RTC_TAMPCTRL_IN2ACT_Pos)
#define RTC_TAMPCTRL_IN2ACT_WAKE    (RTC_TAMPCTRL_IN2ACT_WAKE_Val  << RTC_TAMPCTRL_IN2ACT_Pos)
#define RTC_TAMPCTRL_IN2ACT_CAPTURE (RTC_TAMPCTRL_IN2ACT_CAPTURE_Val << RTC_TAMPCTRL_IN2ACT_Pos)
#define RTC_TAMPCTRL_IN2ACT_ACTL    (RTC_TAMPCTRL_IN2ACT_ACTL_Val  << RTC_TAMPCTRL_IN2ACT_Pos)
#define RTC_TAMPCTRL_IN3ACT_Pos     6            /**< \brief (RTC_TAMPCTRL) Tamper Input 3 Action */
#define RTC_TAMPCTRL_IN3ACT_Msk     (_U_(0x3) << RTC_TAMPCTRL_IN3ACT_Pos)
#define RTC_TAMPCTRL_IN3ACT(value)  (RTC_TAMPCTRL_IN3ACT_Msk & ((value) << RTC_TAMPCTRL_IN3ACT_Pos))
#define   RTC_TAMPCTRL_IN3ACT_OFF_Val     _U_(0x0)   /**< \brief (RTC_TAMPCTRL) Off (Disabled) */
#define   RTC_TAMPCTRL_IN3ACT_WAKE_Val    _U_(0x1)   /**< \brief (RTC_TAMPCTRL) Wake without timestamp */
#define   RTC_TAMPCTRL_IN3ACT_CAPTURE_Val _U_(0x2)   /**< \brief (RTC_TAMPCTRL) Capture timestamp */
#define   RTC_TAMPCTRL_IN3ACT_ACTL_Val    _U_(0x3)   /**< \brief (RTC_TAMPCTRL) Compare IN3 to OUT */
#define RTC_TAMPCTRL_IN3ACT_OFF     (RTC_TAMPCTRL_IN3ACT_OFF_Val   << RTC_TAMPCTRL_IN3ACT_Pos)
#define RTC_TAMPCTRL_IN3ACT_WAKE    (RTC_TAMPCTRL_IN3ACT_WAKE_Val  << RTC_TAMPCTRL_IN3ACT_Pos)
#define RTC_TAMPCTRL_IN3ACT_CAPTURE (RTC_TAMPCTRL_IN3ACT_CAPTURE_Val << RTC_TAMPCTRL_IN3ACT_Pos)
#define RTC_TAMPCTRL_IN3ACT_ACTL    (RTC_TAMPCTRL_IN3ACT_ACTL_Val  << RTC_TAMPCTRL_IN3ACT_Pos)
#define RTC_TAMPCTRL_IN4ACT_Pos     8            /**< \brief (RTC_TAMPCTRL) Tamper Input 4 Action */
#define RTC_TAMPCTRL_IN4ACT_Msk     (_U_(0x3) << RTC_TAMPCTRL_IN4ACT_Pos)
#define RTC_TAMPCTRL_IN4ACT(value)  (RTC_TAMPCTRL_IN4ACT_Msk & ((value) << RTC_TAMPCTRL_IN4ACT_Pos))
#define   RTC_TAMPCTRL_IN4ACT_OFF_Val     _U_(0x0)   /**< \brief (RTC_TAMPCTRL) Off (Disabled) */
#define   RTC_TAMPCTRL_IN4ACT_WAKE_Val    _U_(0x1)   /**< \brief (RTC_TAMPCTRL) Wake without timestamp */
#define   RTC_TAMPCTRL_IN4ACT_CAPTURE_Val _U_(0x2)   /**< \brief (RTC_TAMPCTRL) Capture timestamp */
#define   RTC_TAMPCTRL_IN4ACT_ACTL_Val    _U_(0x3)   /**< \brief (RTC_TAMPCTRL) Compare IN4 to OUT */
#define RTC_TAMPCTRL_IN4ACT_OFF     (RTC_TAMPCTRL_IN4ACT_OFF_Val   << RTC_TAMPCTRL_IN4ACT_Pos)
#define RTC_TAMPCTRL_IN4ACT_WAKE    (RTC_TAMPCTRL_IN4ACT_WAKE_Val  << RTC_TAMPCTRL_IN4ACT_Pos)
#define RTC_TAMPCTRL_IN4ACT_CAPTURE (RTC_TAMPCTRL_IN4ACT_CAPTURE_Val << RTC_TAMPCTRL_IN4ACT_Pos)
#define RTC_TAMPCTRL_IN4ACT_ACTL    (RTC_TAMPCTRL_IN4ACT_ACTL_Val  << RTC_TAMPCTRL_IN4ACT_Pos)
#define RTC_TAMPCTRL_TAMLVL0_Pos    16           /**< \brief (RTC_TAMPCTRL) Tamper Level Select 0 */
#define RTC_TAMPCTRL_TAMLVL0        (_U_(1) << RTC_TAMPCTRL_TAMLVL0_Pos)
#define RTC_TAMPCTRL_TAMLVL1_Pos    17           /**< \brief (RTC_TAMPCTRL) Tamper Level Select 1 */
#define RTC_TAMPCTRL_TAMLVL1        (_U_(1) << RTC_TAMPCTRL_TAMLVL1_Pos)
#define RTC_TAMPCTRL_TAMLVL2_Pos    18           /**< \brief (RTC_TAMPCTRL) Tamper Level Select 2 */
#define RTC_TAMPCTRL_TAMLVL2        (_U_(1) << RTC_TAMPCTRL_TAMLVL2_Pos)
#define RTC_TAMPCTRL_TAMLVL3_Pos    19           /**< \brief (RTC_TAMPCTRL) Tamper Level Select 3 */
#define RTC_TAMPCTRL_TAMLVL3        (_U_(1) << RTC_TAMPCTRL_TAMLVL3_Pos)
#define RTC_TAMPCTRL_TAMLVL4_Pos    20           /**< \brief (RTC_TAMPCTRL) Tamper Level Select 4 */
#define RTC_TAMPCTRL_TAMLVL4        (_U_(1) << RTC_TAMPCTRL_TAMLVL4_Pos)
#define RTC_TAMPCTRL_TAMLVL_Pos     16           /**< \brief (RTC_TAMPCTRL) Tamper Level Select x */
#define RTC_TAMPCTRL_TAMLVL_Msk     (_U_(0x1F) << RTC_TAMPCTRL_TAMLVL_Pos)
#define RTC_TAMPCTRL_TAMLVL(value)  (RTC_TAMPCTRL_TAMLVL_Msk & ((value) << RTC_TAMPCTRL_TAMLVL_Pos))
#define RTC_TAMPCTRL_DEBNC0_Pos     24           /**< \brief (RTC_TAMPCTRL) Debouncer Enable 0 */
#define RTC_TAMPCTRL_DEBNC0         (_U_(1) << RTC_TAMPCTRL_DEBNC0_Pos)
#define RTC_TAMPCTRL_DEBNC1_Pos     25           /**< \brief (RTC_TAMPCTRL) Debouncer Enable 1 */
#define RTC_TAMPCTRL_DEBNC1         (_U_(1) << RTC_TAMPCTRL_DEBNC1_Pos)
#define RTC_TAMPCTRL_DEBNC2_Pos     26           /**< \brief (RTC_TAMPCTRL) Debouncer Enable 2 */
#define RTC_TAMPCTRL_DEBNC2         (_U_(1) << RTC_TAMPCTRL_DEBNC2_Pos)
#define RTC_TAMPCTRL_DEBNC3_Pos     27           /**< \brief (RTC_TAMPCTRL) Debouncer Enable 3 */
#define RTC_TAMPCTRL_DEBNC3         (_U_(1) << RTC_TAMPCTRL_DEBNC3_Pos)
#define RTC_TAMPCTRL_DEBNC4_Pos     28           /**< \brief (RTC_TAMPCTRL) Debouncer Enable 4 */
#define RTC_TAMPCTRL_DEBNC4         (_U_(1) << RTC_TAMPCTRL_DEBNC4_Pos)
#define RTC_TAMPCTRL_DEBNC_Pos      24           /**< \brief (RTC_TAMPCTRL) Debouncer Enable x */
#define RTC_TAMPCTRL_DEBNC_Msk      (_U_(0x1F) << RTC_TAMPCTRL_DEBNC_Pos)
#define RTC_TAMPCTRL_DEBNC(value)   (RTC_TAMPCTRL_DEBNC_Msk & ((value) << RTC_TAMPCTRL_DEBNC_Pos))
#define RTC_TAMPCTRL_MASK           _U_(0x1F1F03FF) /**< \brief (RTC_TAMPCTRL) MASK Register */

/* -------- RTC_MODE0_TIMESTAMP : (RTC Offset: 0x64) (R/  32) MODE0 MODE0 Timestamp -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t COUNT:32;         /*!< bit:  0..31  Count Timestamp Value              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE0_TIMESTAMP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE0_TIMESTAMP_OFFSET  0x64         /**< \brief (RTC_MODE0_TIMESTAMP offset) MODE0 Timestamp */
#define RTC_MODE0_TIMESTAMP_RESETVALUE _U_(0x00000000) /**< \brief (RTC_MODE0_TIMESTAMP reset_value) MODE0 Timestamp */

#define RTC_MODE0_TIMESTAMP_COUNT_Pos 0            /**< \brief (RTC_MODE0_TIMESTAMP) Count Timestamp Value */
#define RTC_MODE0_TIMESTAMP_COUNT_Msk (_U_(0xFFFFFFFF) << RTC_MODE0_TIMESTAMP_COUNT_Pos)
#define RTC_MODE0_TIMESTAMP_COUNT(value) (RTC_MODE0_TIMESTAMP_COUNT_Msk & ((value) << RTC_MODE0_TIMESTAMP_COUNT_Pos))
#define RTC_MODE0_TIMESTAMP_MASK    _U_(0xFFFFFFFF) /**< \brief (RTC_MODE0_TIMESTAMP) MASK Register */

/* -------- RTC_MODE1_TIMESTAMP : (RTC Offset: 0x64) (R/  32) MODE1 MODE1 Timestamp -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t COUNT:16;         /*!< bit:  0..15  Count Timestamp Value              */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE1_TIMESTAMP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE1_TIMESTAMP_OFFSET  0x64         /**< \brief (RTC_MODE1_TIMESTAMP offset) MODE1 Timestamp */
#define RTC_MODE1_TIMESTAMP_RESETVALUE _U_(0x00000000) /**< \brief (RTC_MODE1_TIMESTAMP reset_value) MODE1 Timestamp */

#define RTC_MODE1_TIMESTAMP_COUNT_Pos 0            /**< \brief (RTC_MODE1_TIMESTAMP) Count Timestamp Value */
#define RTC_MODE1_TIMESTAMP_COUNT_Msk (_U_(0xFFFF) << RTC_MODE1_TIMESTAMP_COUNT_Pos)
#define RTC_MODE1_TIMESTAMP_COUNT(value) (RTC_MODE1_TIMESTAMP_COUNT_Msk & ((value) << RTC_MODE1_TIMESTAMP_COUNT_Pos))
#define RTC_MODE1_TIMESTAMP_MASK    _U_(0x0000FFFF) /**< \brief (RTC_MODE1_TIMESTAMP) MASK Register */

/* -------- RTC_MODE2_TIMESTAMP : (RTC Offset: 0x64) (R/  32) MODE2 MODE2 Timestamp -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SECOND:6;         /*!< bit:  0.. 5  Second Timestamp Value             */
    uint32_t MINUTE:6;         /*!< bit:  6..11  Minute Timestamp Value             */
    uint32_t HOUR:5;           /*!< bit: 12..16  Hour Timestamp Value               */
    uint32_t DAY:5;            /*!< bit: 17..21  Day Timestamp Value                */
    uint32_t MONTH:4;          /*!< bit: 22..25  Month Timestamp Value              */
    uint32_t YEAR:6;           /*!< bit: 26..31  Year Timestamp Value               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_MODE2_TIMESTAMP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_MODE2_TIMESTAMP_OFFSET  0x64         /**< \brief (RTC_MODE2_TIMESTAMP offset) MODE2 Timestamp */
#define RTC_MODE2_TIMESTAMP_RESETVALUE _U_(0x00000000) /**< \brief (RTC_MODE2_TIMESTAMP reset_value) MODE2 Timestamp */

#define RTC_MODE2_TIMESTAMP_SECOND_Pos 0            /**< \brief (RTC_MODE2_TIMESTAMP) Second Timestamp Value */
#define RTC_MODE2_TIMESTAMP_SECOND_Msk (_U_(0x3F) << RTC_MODE2_TIMESTAMP_SECOND_Pos)
#define RTC_MODE2_TIMESTAMP_SECOND(value) (RTC_MODE2_TIMESTAMP_SECOND_Msk & ((value) << RTC_MODE2_TIMESTAMP_SECOND_Pos))
#define RTC_MODE2_TIMESTAMP_MINUTE_Pos 6            /**< \brief (RTC_MODE2_TIMESTAMP) Minute Timestamp Value */
#define RTC_MODE2_TIMESTAMP_MINUTE_Msk (_U_(0x3F) << RTC_MODE2_TIMESTAMP_MINUTE_Pos)
#define RTC_MODE2_TIMESTAMP_MINUTE(value) (RTC_MODE2_TIMESTAMP_MINUTE_Msk & ((value) << RTC_MODE2_TIMESTAMP_MINUTE_Pos))
#define RTC_MODE2_TIMESTAMP_HOUR_Pos 12           /**< \brief (RTC_MODE2_TIMESTAMP) Hour Timestamp Value */
#define RTC_MODE2_TIMESTAMP_HOUR_Msk (_U_(0x1F) << RTC_MODE2_TIMESTAMP_HOUR_Pos)
#define RTC_MODE2_TIMESTAMP_HOUR(value) (RTC_MODE2_TIMESTAMP_HOUR_Msk & ((value) << RTC_MODE2_TIMESTAMP_HOUR_Pos))
#define   RTC_MODE2_TIMESTAMP_HOUR_AM_Val _U_(0x0)   /**< \brief (RTC_MODE2_TIMESTAMP) AM when CLKREP in 12-hour */
#define   RTC_MODE2_TIMESTAMP_HOUR_PM_Val _U_(0x10)   /**< \brief (RTC_MODE2_TIMESTAMP) PM when CLKREP in 12-hour */
#define RTC_MODE2_TIMESTAMP_HOUR_AM (RTC_MODE2_TIMESTAMP_HOUR_AM_Val << RTC_MODE2_TIMESTAMP_HOUR_Pos)
#define RTC_MODE2_TIMESTAMP_HOUR_PM (RTC_MODE2_TIMESTAMP_HOUR_PM_Val << RTC_MODE2_TIMESTAMP_HOUR_Pos)
#define RTC_MODE2_TIMESTAMP_DAY_Pos 17           /**< \brief (RTC_MODE2_TIMESTAMP) Day Timestamp Value */
#define RTC_MODE2_TIMESTAMP_DAY_Msk (_U_(0x1F) << RTC_MODE2_TIMESTAMP_DAY_Pos)
#define RTC_MODE2_TIMESTAMP_DAY(value) (RTC_MODE2_TIMESTAMP_DAY_Msk & ((value) << RTC_MODE2_TIMESTAMP_DAY_Pos))
#define RTC_MODE2_TIMESTAMP_MONTH_Pos 22           /**< \brief (RTC_MODE2_TIMESTAMP) Month Timestamp Value */
#define RTC_MODE2_TIMESTAMP_MONTH_Msk (_U_(0xF) << RTC_MODE2_TIMESTAMP_MONTH_Pos)
#define RTC_MODE2_TIMESTAMP_MONTH(value) (RTC_MODE2_TIMESTAMP_MONTH_Msk & ((value) << RTC_MODE2_TIMESTAMP_MONTH_Pos))
#define RTC_MODE2_TIMESTAMP_YEAR_Pos 26           /**< \brief (RTC_MODE2_TIMESTAMP) Year Timestamp Value */
#define RTC_MODE2_TIMESTAMP_YEAR_Msk (_U_(0x3F) << RTC_MODE2_TIMESTAMP_YEAR_Pos)
#define RTC_MODE2_TIMESTAMP_YEAR(value) (RTC_MODE2_TIMESTAMP_YEAR_Msk & ((value) << RTC_MODE2_TIMESTAMP_YEAR_Pos))
#define RTC_MODE2_TIMESTAMP_MASK    _U_(0xFFFFFFFF) /**< \brief (RTC_MODE2_TIMESTAMP) MASK Register */

/* -------- RTC_TAMPID : (RTC Offset: 0x68) (R/W 32) Tamper ID -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t TAMPID0:1;        /*!< bit:      0  Tamper Input 0 Detected            */
    uint32_t TAMPID1:1;        /*!< bit:      1  Tamper Input 1 Detected            */
    uint32_t TAMPID2:1;        /*!< bit:      2  Tamper Input 2 Detected            */
    uint32_t TAMPID3:1;        /*!< bit:      3  Tamper Input 3 Detected            */
    uint32_t TAMPID4:1;        /*!< bit:      4  Tamper Input 4 Detected            */
    uint32_t :26;              /*!< bit:  5..30  Reserved                           */
    uint32_t TAMPEVT:1;        /*!< bit:     31  Tamper Event Detected              */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t TAMPID:5;         /*!< bit:  0.. 4  Tamper Input x Detected            */
    uint32_t :27;              /*!< bit:  5..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_TAMPID_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_TAMPID_OFFSET           0x68         /**< \brief (RTC_TAMPID offset) Tamper ID */
#define RTC_TAMPID_RESETVALUE       _U_(0x00000000) /**< \brief (RTC_TAMPID reset_value) Tamper ID */

#define RTC_TAMPID_TAMPID0_Pos      0            /**< \brief (RTC_TAMPID) Tamper Input 0 Detected */
#define RTC_TAMPID_TAMPID0          (_U_(1) << RTC_TAMPID_TAMPID0_Pos)
#define RTC_TAMPID_TAMPID1_Pos      1            /**< \brief (RTC_TAMPID) Tamper Input 1 Detected */
#define RTC_TAMPID_TAMPID1          (_U_(1) << RTC_TAMPID_TAMPID1_Pos)
#define RTC_TAMPID_TAMPID2_Pos      2            /**< \brief (RTC_TAMPID) Tamper Input 2 Detected */
#define RTC_TAMPID_TAMPID2          (_U_(1) << RTC_TAMPID_TAMPID2_Pos)
#define RTC_TAMPID_TAMPID3_Pos      3            /**< \brief (RTC_TAMPID) Tamper Input 3 Detected */
#define RTC_TAMPID_TAMPID3          (_U_(1) << RTC_TAMPID_TAMPID3_Pos)
#define RTC_TAMPID_TAMPID4_Pos      4            /**< \brief (RTC_TAMPID) Tamper Input 4 Detected */
#define RTC_TAMPID_TAMPID4          (_U_(1) << RTC_TAMPID_TAMPID4_Pos)
#define RTC_TAMPID_TAMPID_Pos       0            /**< \brief (RTC_TAMPID) Tamper Input x Detected */
#define RTC_TAMPID_TAMPID_Msk       (_U_(0x1F) << RTC_TAMPID_TAMPID_Pos)
#define RTC_TAMPID_TAMPID(value)    (RTC_TAMPID_TAMPID_Msk & ((value) << RTC_TAMPID_TAMPID_Pos))
#define RTC_TAMPID_TAMPEVT_Pos      31           /**< \brief (RTC_TAMPID) Tamper Event Detected */
#define RTC_TAMPID_TAMPEVT          (_U_(0x1) << RTC_TAMPID_TAMPEVT_Pos)
#define RTC_TAMPID_MASK             _U_(0x8000001F) /**< \brief (RTC_TAMPID) MASK Register */

/* -------- RTC_BKUP : (RTC Offset: 0x80) (R/W 32) Backup -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BKUP:32;          /*!< bit:  0..31  Backup                             */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RTC_BKUP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RTC_BKUP_OFFSET             0x80         /**< \brief (RTC_BKUP offset) Backup */
#define RTC_BKUP_RESETVALUE         _U_(0x00000000) /**< \brief (RTC_BKUP reset_value) Backup */

#define RTC_BKUP_BKUP_Pos           0            /**< \brief (RTC_BKUP) Backup */
#define RTC_BKUP_BKUP_Msk           (_U_(0xFFFFFFFF) << RTC_BKUP_BKUP_Pos)
#define RTC_BKUP_BKUP(value)        (RTC_BKUP_BKUP_Msk & ((value) << RTC_BKUP_BKUP_Pos))
#define RTC_BKUP_MASK               _U_(0xFFFFFFFF) /**< \brief (RTC_BKUP) MASK Register */

/** \brief RtcMode2Alarm hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO RTC_MODE2_ALARM_Type      ALARM;       /**< \brief Offset: 0x00 (R/W 32) MODE2_ALARM Alarm n Value */
  __IO RTC_MODE2_MASK_Type       MASK;        /**< \brief Offset: 0x04 (R/W  8) MODE2_ALARM Alarm n Mask */
       RoReg8                    Reserved1[0x3];
} RtcMode2Alarm;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief RTC_MODE0 hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct { /* 32-bit Counter with Single 32-bit Compare */
  __IO RTC_MODE0_CTRLA_Type      CTRLA;       /**< \brief Offset: 0x00 (R/W 16) MODE0 Control A */
  __IO RTC_MODE0_CTRLB_Type      CTRLB;       /**< \brief Offset: 0x02 (R/W 16) MODE0 Control B */
  __IO RTC_MODE0_EVCTRL_Type     EVCTRL;      /**< \brief Offset: 0x04 (R/W 32) MODE0 Event Control */
  __IO RTC_MODE0_INTENCLR_Type   INTENCLR;    /**< \brief Offset: 0x08 (R/W 16) MODE0 Interrupt Enable Clear */
  __IO RTC_MODE0_INTENSET_Type   INTENSET;    /**< \brief Offset: 0x0A (R/W 16) MODE0 Interrupt Enable Set */
  __IO RTC_MODE0_INTFLAG_Type    INTFLAG;     /**< \brief Offset: 0x0C (R/W 16) MODE0 Interrupt Flag Status and Clear */
  __IO RTC_DBGCTRL_Type          DBGCTRL;     /**< \brief Offset: 0x0E (R/W  8) Debug Control */
       RoReg8                    Reserved1[0x1];
  __I  RTC_MODE0_SYNCBUSY_Type   SYNCBUSY;    /**< \brief Offset: 0x10 (R/  32) MODE0 Synchronization Busy Status */
  __IO RTC_FREQCORR_Type         FREQCORR;    /**< \brief Offset: 0x14 (R/W  8) Frequency Correction */
       RoReg8                    Reserved2[0x3];
  __IO RTC_MODE0_COUNT_Type      COUNT;       /**< \brief Offset: 0x18 (R/W 32) MODE0 Counter Value */
       RoReg8                    Reserved3[0x4];
  __IO RTC_MODE0_COMP_Type       COMP[2];     /**< \brief Offset: 0x20 (R/W 32) MODE0 Compare n Value */
       RoReg8                    Reserved4[0x18];
  __IO RTC_GP_Type               GP[4];       /**< \brief Offset: 0x40 (R/W 32) General Purpose */
       RoReg8                    Reserved5[0x10];
  __IO RTC_TAMPCTRL_Type         TAMPCTRL;    /**< \brief Offset: 0x60 (R/W 32) Tamper Control */
  __I  RTC_MODE0_TIMESTAMP_Type  TIMESTAMP;   /**< \brief Offset: 0x64 (R/  32) MODE0 Timestamp */
  __IO RTC_TAMPID_Type           TAMPID;      /**< \brief Offset: 0x68 (R/W 32) Tamper ID */
       RoReg8                    Reserved6[0x14];
  __IO RTC_BKUP_Type             BKUP[8];     /**< \brief Offset: 0x80 (R/W 32) Backup */
} RtcMode0;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief RTC_MODE1 hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct { /* 16-bit Counter with Two 16-bit Compares */
  __IO RTC_MODE1_CTRLA_Type      CTRLA;       /**< \brief Offset: 0x00 (R/W 16) MODE1 Control A */
  __IO RTC_MODE1_CTRLB_Type      CTRLB;       /**< \brief Offset: 0x02 (R/W 16) MODE1 Control B */
  __IO RTC_MODE1_EVCTRL_Type     EVCTRL;      /**< \brief Offset: 0x04 (R/W 32) MODE1 Event Control */
  __IO RTC_MODE1_INTENCLR_Type   INTENCLR;    /**< \brief Offset: 0x08 (R/W 16) MODE1 Interrupt Enable Clear */
  __IO RTC_MODE1_INTENSET_Type   INTENSET;    /**< \brief Offset: 0x0A (R/W 16) MODE1 Interrupt Enable Set */
  __IO RTC_MODE1_INTFLAG_Type    INTFLAG;     /**< \brief Offset: 0x0C (R/W 16) MODE1 Interrupt Flag Status and Clear */
  __IO RTC_DBGCTRL_Type          DBGCTRL;     /**< \brief Offset: 0x0E (R/W  8) Debug Control */
       RoReg8                    Reserved1[0x1];
  __I  RTC_MODE1_SYNCBUSY_Type   SYNCBUSY;    /**< \brief Offset: 0x10 (R/  32) MODE1 Synchronization Busy Status */
  __IO RTC_FREQCORR_Type         FREQCORR;    /**< \brief Offset: 0x14 (R/W  8) Frequency Correction */
       RoReg8                    Reserved2[0x3];
  __IO RTC_MODE1_COUNT_Type      COUNT;       /**< \brief Offset: 0x18 (R/W 16) MODE1 Counter Value */
       RoReg8                    Reserved3[0x2];
  __IO RTC_MODE1_PER_Type        PER;         /**< \brief Offset: 0x1C (R/W 16) MODE1 Counter Period */
       RoReg8                    Reserved4[0x2];
  __IO RTC_MODE1_COMP_Type       COMP[4];     /**< \brief Offset: 0x20 (R/W 16) MODE1 Compare n Value */
       RoReg8                    Reserved5[0x18];
  __IO RTC_GP_Type               GP[4];       /**< \brief Offset: 0x40 (R/W 32) General Purpose */
       RoReg8                    Reserved6[0x10];
  __IO RTC_TAMPCTRL_Type         TAMPCTRL;    /**< \brief Offset: 0x60 (R/W 32) Tamper Control */
  __I  RTC_MODE1_TIMESTAMP_Type  TIMESTAMP;   /**< \brief Offset: 0x64 (R/  32) MODE1 Timestamp */
  __IO RTC_TAMPID_Type           TAMPID;      /**< \brief Offset: 0x68 (R/W 32) Tamper ID */
       RoReg8                    Reserved7[0x14];
  __IO RTC_BKUP_Type             BKUP[8];     /**< \brief Offset: 0x80 (R/W 32) Backup */
} RtcMode1;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief RTC_MODE2 hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct { /* Clock/Calendar with Alarm */
  __IO RTC_MODE2_CTRLA_Type      CTRLA;       /**< \brief Offset: 0x00 (R/W 16) MODE2 Control A */
  __IO RTC_MODE2_CTRLB_Type      CTRLB;       /**< \brief Offset: 0x02 (R/W 16) MODE2 Control B */
  __IO RTC_MODE2_EVCTRL_Type     EVCTRL;      /**< \brief Offset: 0x04 (R/W 32) MODE2 Event Control */
  __IO RTC_MODE2_INTENCLR_Type   INTENCLR;    /**< \brief Offset: 0x08 (R/W 16) MODE2 Interrupt Enable Clear */
  __IO RTC_MODE2_INTENSET_Type   INTENSET;    /**< \brief Offset: 0x0A (R/W 16) MODE2 Interrupt Enable Set */
  __IO RTC_MODE2_INTFLAG_Type    INTFLAG;     /**< \brief Offset: 0x0C (R/W 16) MODE2 Interrupt Flag Status and Clear */
  __IO RTC_DBGCTRL_Type          DBGCTRL;     /**< \brief Offset: 0x0E (R/W  8) Debug Control */
       RoReg8                    Reserved1[0x1];
  __I  RTC_MODE2_SYNCBUSY_Type   SYNCBUSY;    /**< \brief Offset: 0x10 (R/  32) MODE2 Synchronization Busy Status */
  __IO RTC_FREQCORR_Type         FREQCORR;    /**< \brief Offset: 0x14 (R/W  8) Frequency Correction */
       RoReg8                    Reserved2[0x3];
  __IO RTC_MODE2_CLOCK_Type      CLOCK;       /**< \brief Offset: 0x18 (R/W 32) MODE2 Clock Value */
       RoReg8                    Reserved3[0x4];
       RtcMode2Alarm             Mode2Alarm[2]; /**< \brief Offset: 0x20 RtcMode2Alarm groups [NUM_OF_ALARMS] */
       RoReg8                    Reserved4[0x10];
  __IO RTC_GP_Type               GP[4];       /**< \brief Offset: 0x40 (R/W 32) General Purpose */
       RoReg8                    Reserved5[0x10];
  __IO RTC_TAMPCTRL_Type         TAMPCTRL;    /**< \brief Offset: 0x60 (R/W 32) Tamper Control */
  __I  RTC_MODE2_TIMESTAMP_Type  TIMESTAMP;   /**< \brief Offset: 0x64 (R/  32) MODE2 Timestamp */
  __IO RTC_TAMPID_Type           TAMPID;      /**< \brief Offset: 0x68 (R/W 32) Tamper ID */
       RoReg8                    Reserved6[0x14];
  __IO RTC_BKUP_Type             BKUP[8];     /**< \brief Offset: 0x80 (R/W 32) Backup */
} RtcMode2;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
       RtcMode0                  MODE0;       /**< \brief Offset: 0x00 32-bit Counter with Single 32-bit Compare */
       RtcMode1                  MODE1;       /**< \brief Offset: 0x00 16-bit Counter with Two 16-bit Compares */
       RtcMode2                  MODE2;       /**< \brief Offset: 0x00 Clock/Calendar with Alarm */
} Rtc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_RTC_COMPONENT_ */
