/**
 * \file
 *
 * \brief Component description for CMCC
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

#ifndef _SAMD51_CMCC_COMPONENT_
#define _SAMD51_CMCC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR CMCC */
/* ========================================================================== */
/** \addtogroup SAMD51_CMCC Cortex M Cache Controller */
/*@{*/

#define CMCC_U2015
#define REV_CMCC                    0x600

/* -------- CMCC_TYPE : (CMCC Offset: 0x00) (R/  32) Cache Type Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t GCLK:1;           /*!< bit:      1  dynamic Clock Gating supported     */
    uint32_t :2;               /*!< bit:  2.. 3  Reserved                           */
    uint32_t RRP:1;            /*!< bit:      4  Round Robin Policy supported       */
    uint32_t WAYNUM:2;         /*!< bit:  5.. 6  Number of Way                      */
    uint32_t LCKDOWN:1;        /*!< bit:      7  Lock Down supported                */
    uint32_t CSIZE:3;          /*!< bit:  8..10  Cache Size                         */
    uint32_t CLSIZE:3;         /*!< bit: 11..13  Cache Line Size                    */
    uint32_t :18;              /*!< bit: 14..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CMCC_TYPE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CMCC_TYPE_OFFSET            0x00         /**< \brief (CMCC_TYPE offset) Cache Type Register */
#define CMCC_TYPE_RESETVALUE        _U_(0x000012D2) /**< \brief (CMCC_TYPE reset_value) Cache Type Register */

#define CMCC_TYPE_GCLK_Pos          1            /**< \brief (CMCC_TYPE) dynamic Clock Gating supported */
#define CMCC_TYPE_GCLK              (_U_(0x1) << CMCC_TYPE_GCLK_Pos)
#define CMCC_TYPE_RRP_Pos           4            /**< \brief (CMCC_TYPE) Round Robin Policy supported */
#define CMCC_TYPE_RRP               (_U_(0x1) << CMCC_TYPE_RRP_Pos)
#define CMCC_TYPE_WAYNUM_Pos        5            /**< \brief (CMCC_TYPE) Number of Way */
#define CMCC_TYPE_WAYNUM_Msk        (_U_(0x3) << CMCC_TYPE_WAYNUM_Pos)
#define CMCC_TYPE_WAYNUM(value)     (CMCC_TYPE_WAYNUM_Msk & ((value) << CMCC_TYPE_WAYNUM_Pos))
#define   CMCC_TYPE_WAYNUM_DMAPPED_Val    _U_(0x0)   /**< \brief (CMCC_TYPE) Direct Mapped Cache */
#define   CMCC_TYPE_WAYNUM_ARCH2WAY_Val   _U_(0x1)   /**< \brief (CMCC_TYPE) 2-WAY set associative */
#define   CMCC_TYPE_WAYNUM_ARCH4WAY_Val   _U_(0x2)   /**< \brief (CMCC_TYPE) 4-WAY set associative */
#define CMCC_TYPE_WAYNUM_DMAPPED    (CMCC_TYPE_WAYNUM_DMAPPED_Val  << CMCC_TYPE_WAYNUM_Pos)
#define CMCC_TYPE_WAYNUM_ARCH2WAY   (CMCC_TYPE_WAYNUM_ARCH2WAY_Val << CMCC_TYPE_WAYNUM_Pos)
#define CMCC_TYPE_WAYNUM_ARCH4WAY   (CMCC_TYPE_WAYNUM_ARCH4WAY_Val << CMCC_TYPE_WAYNUM_Pos)
#define CMCC_TYPE_LCKDOWN_Pos       7            /**< \brief (CMCC_TYPE) Lock Down supported */
#define CMCC_TYPE_LCKDOWN           (_U_(0x1) << CMCC_TYPE_LCKDOWN_Pos)
#define CMCC_TYPE_CSIZE_Pos         8            /**< \brief (CMCC_TYPE) Cache Size */
#define CMCC_TYPE_CSIZE_Msk         (_U_(0x7) << CMCC_TYPE_CSIZE_Pos)
#define CMCC_TYPE_CSIZE(value)      (CMCC_TYPE_CSIZE_Msk & ((value) << CMCC_TYPE_CSIZE_Pos))
#define   CMCC_TYPE_CSIZE_CSIZE_1KB_Val   _U_(0x0)   /**< \brief (CMCC_TYPE) Cache Size is 1 KB */
#define   CMCC_TYPE_CSIZE_CSIZE_2KB_Val   _U_(0x1)   /**< \brief (CMCC_TYPE) Cache Size is 2 KB */
#define   CMCC_TYPE_CSIZE_CSIZE_4KB_Val   _U_(0x2)   /**< \brief (CMCC_TYPE) Cache Size is 4 KB */
#define   CMCC_TYPE_CSIZE_CSIZE_8KB_Val   _U_(0x3)   /**< \brief (CMCC_TYPE) Cache Size is 8 KB */
#define   CMCC_TYPE_CSIZE_CSIZE_16KB_Val  _U_(0x4)   /**< \brief (CMCC_TYPE) Cache Size is 16 KB */
#define   CMCC_TYPE_CSIZE_CSIZE_32KB_Val  _U_(0x5)   /**< \brief (CMCC_TYPE) Cache Size is 32 KB */
#define   CMCC_TYPE_CSIZE_CSIZE_64KB_Val  _U_(0x6)   /**< \brief (CMCC_TYPE) Cache Size is 64 KB */
#define CMCC_TYPE_CSIZE_CSIZE_1KB   (CMCC_TYPE_CSIZE_CSIZE_1KB_Val << CMCC_TYPE_CSIZE_Pos)
#define CMCC_TYPE_CSIZE_CSIZE_2KB   (CMCC_TYPE_CSIZE_CSIZE_2KB_Val << CMCC_TYPE_CSIZE_Pos)
#define CMCC_TYPE_CSIZE_CSIZE_4KB   (CMCC_TYPE_CSIZE_CSIZE_4KB_Val << CMCC_TYPE_CSIZE_Pos)
#define CMCC_TYPE_CSIZE_CSIZE_8KB   (CMCC_TYPE_CSIZE_CSIZE_8KB_Val << CMCC_TYPE_CSIZE_Pos)
#define CMCC_TYPE_CSIZE_CSIZE_16KB  (CMCC_TYPE_CSIZE_CSIZE_16KB_Val << CMCC_TYPE_CSIZE_Pos)
#define CMCC_TYPE_CSIZE_CSIZE_32KB  (CMCC_TYPE_CSIZE_CSIZE_32KB_Val << CMCC_TYPE_CSIZE_Pos)
#define CMCC_TYPE_CSIZE_CSIZE_64KB  (CMCC_TYPE_CSIZE_CSIZE_64KB_Val << CMCC_TYPE_CSIZE_Pos)
#define CMCC_TYPE_CLSIZE_Pos        11           /**< \brief (CMCC_TYPE) Cache Line Size */
#define CMCC_TYPE_CLSIZE_Msk        (_U_(0x7) << CMCC_TYPE_CLSIZE_Pos)
#define CMCC_TYPE_CLSIZE(value)     (CMCC_TYPE_CLSIZE_Msk & ((value) << CMCC_TYPE_CLSIZE_Pos))
#define   CMCC_TYPE_CLSIZE_CLSIZE_4B_Val  _U_(0x0)   /**< \brief (CMCC_TYPE) Cache Line Size is 4 bytes */
#define   CMCC_TYPE_CLSIZE_CLSIZE_8B_Val  _U_(0x1)   /**< \brief (CMCC_TYPE) Cache Line Size is 8 bytes */
#define   CMCC_TYPE_CLSIZE_CLSIZE_16B_Val _U_(0x2)   /**< \brief (CMCC_TYPE) Cache Line Size is 16 bytes */
#define   CMCC_TYPE_CLSIZE_CLSIZE_32B_Val _U_(0x3)   /**< \brief (CMCC_TYPE) Cache Line Size is 32 bytes */
#define   CMCC_TYPE_CLSIZE_CLSIZE_64B_Val _U_(0x4)   /**< \brief (CMCC_TYPE) Cache Line Size is 64 bytes */
#define   CMCC_TYPE_CLSIZE_CLSIZE_128B_Val _U_(0x5)   /**< \brief (CMCC_TYPE) Cache Line Size is 128 bytes */
#define CMCC_TYPE_CLSIZE_CLSIZE_4B  (CMCC_TYPE_CLSIZE_CLSIZE_4B_Val << CMCC_TYPE_CLSIZE_Pos)
#define CMCC_TYPE_CLSIZE_CLSIZE_8B  (CMCC_TYPE_CLSIZE_CLSIZE_8B_Val << CMCC_TYPE_CLSIZE_Pos)
#define CMCC_TYPE_CLSIZE_CLSIZE_16B (CMCC_TYPE_CLSIZE_CLSIZE_16B_Val << CMCC_TYPE_CLSIZE_Pos)
#define CMCC_TYPE_CLSIZE_CLSIZE_32B (CMCC_TYPE_CLSIZE_CLSIZE_32B_Val << CMCC_TYPE_CLSIZE_Pos)
#define CMCC_TYPE_CLSIZE_CLSIZE_64B (CMCC_TYPE_CLSIZE_CLSIZE_64B_Val << CMCC_TYPE_CLSIZE_Pos)
#define CMCC_TYPE_CLSIZE_CLSIZE_128B (CMCC_TYPE_CLSIZE_CLSIZE_128B_Val << CMCC_TYPE_CLSIZE_Pos)
#define CMCC_TYPE_MASK              _U_(0x00003FF2) /**< \brief (CMCC_TYPE) MASK Register */

/* -------- CMCC_CFG : (CMCC Offset: 0x04) (R/W 32) Cache Configuration Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t ICDIS:1;          /*!< bit:      1  Instruction Cache Disable          */
    uint32_t DCDIS:1;          /*!< bit:      2  Data Cache Disable                 */
    uint32_t :1;               /*!< bit:      3  Reserved                           */
    uint32_t CSIZESW:3;        /*!< bit:  4.. 6  Cache size configured by software  */
    uint32_t :25;              /*!< bit:  7..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CMCC_CFG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CMCC_CFG_OFFSET             0x04         /**< \brief (CMCC_CFG offset) Cache Configuration Register */
#define CMCC_CFG_RESETVALUE         _U_(0x00000020) /**< \brief (CMCC_CFG reset_value) Cache Configuration Register */

#define CMCC_CFG_ICDIS_Pos          1            /**< \brief (CMCC_CFG) Instruction Cache Disable */
#define CMCC_CFG_ICDIS              (_U_(0x1) << CMCC_CFG_ICDIS_Pos)
#define CMCC_CFG_DCDIS_Pos          2            /**< \brief (CMCC_CFG) Data Cache Disable */
#define CMCC_CFG_DCDIS              (_U_(0x1) << CMCC_CFG_DCDIS_Pos)
#define CMCC_CFG_CSIZESW_Pos        4            /**< \brief (CMCC_CFG) Cache size configured by software */
#define CMCC_CFG_CSIZESW_Msk        (_U_(0x7) << CMCC_CFG_CSIZESW_Pos)
#define CMCC_CFG_CSIZESW(value)     (CMCC_CFG_CSIZESW_Msk & ((value) << CMCC_CFG_CSIZESW_Pos))
#define   CMCC_CFG_CSIZESW_CONF_CSIZE_1KB_Val _U_(0x0)   /**< \brief (CMCC_CFG) the Cache Size is configured to 1KB */
#define   CMCC_CFG_CSIZESW_CONF_CSIZE_2KB_Val _U_(0x1)   /**< \brief (CMCC_CFG) the Cache Size is configured to 2KB */
#define   CMCC_CFG_CSIZESW_CONF_CSIZE_4KB_Val _U_(0x2)   /**< \brief (CMCC_CFG) the Cache Size is configured to 4KB */
#define   CMCC_CFG_CSIZESW_CONF_CSIZE_8KB_Val _U_(0x3)   /**< \brief (CMCC_CFG) the Cache Size is configured to 8KB */
#define   CMCC_CFG_CSIZESW_CONF_CSIZE_16KB_Val _U_(0x4)   /**< \brief (CMCC_CFG) the Cache Size is configured to 16KB */
#define   CMCC_CFG_CSIZESW_CONF_CSIZE_32KB_Val _U_(0x5)   /**< \brief (CMCC_CFG) the Cache Size is configured to 32KB */
#define   CMCC_CFG_CSIZESW_CONF_CSIZE_64KB_Val _U_(0x6)   /**< \brief (CMCC_CFG) the Cache Size is configured to 64KB */
#define CMCC_CFG_CSIZESW_CONF_CSIZE_1KB (CMCC_CFG_CSIZESW_CONF_CSIZE_1KB_Val << CMCC_CFG_CSIZESW_Pos)
#define CMCC_CFG_CSIZESW_CONF_CSIZE_2KB (CMCC_CFG_CSIZESW_CONF_CSIZE_2KB_Val << CMCC_CFG_CSIZESW_Pos)
#define CMCC_CFG_CSIZESW_CONF_CSIZE_4KB (CMCC_CFG_CSIZESW_CONF_CSIZE_4KB_Val << CMCC_CFG_CSIZESW_Pos)
#define CMCC_CFG_CSIZESW_CONF_CSIZE_8KB (CMCC_CFG_CSIZESW_CONF_CSIZE_8KB_Val << CMCC_CFG_CSIZESW_Pos)
#define CMCC_CFG_CSIZESW_CONF_CSIZE_16KB (CMCC_CFG_CSIZESW_CONF_CSIZE_16KB_Val << CMCC_CFG_CSIZESW_Pos)
#define CMCC_CFG_CSIZESW_CONF_CSIZE_32KB (CMCC_CFG_CSIZESW_CONF_CSIZE_32KB_Val << CMCC_CFG_CSIZESW_Pos)
#define CMCC_CFG_CSIZESW_CONF_CSIZE_64KB (CMCC_CFG_CSIZESW_CONF_CSIZE_64KB_Val << CMCC_CFG_CSIZESW_Pos)
#define CMCC_CFG_MASK               _U_(0x00000076) /**< \brief (CMCC_CFG) MASK Register */

/* -------- CMCC_CTRL : (CMCC Offset: 0x08) ( /W 32) Cache Control Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CEN:1;            /*!< bit:      0  Cache Controller Enable            */
    uint32_t :31;              /*!< bit:  1..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CMCC_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CMCC_CTRL_OFFSET            0x08         /**< \brief (CMCC_CTRL offset) Cache Control Register */
#define CMCC_CTRL_RESETVALUE        _U_(0x00000000) /**< \brief (CMCC_CTRL reset_value) Cache Control Register */

#define CMCC_CTRL_CEN_Pos           0            /**< \brief (CMCC_CTRL) Cache Controller Enable */
#define CMCC_CTRL_CEN               (_U_(0x1) << CMCC_CTRL_CEN_Pos)
#define CMCC_CTRL_MASK              _U_(0x00000001) /**< \brief (CMCC_CTRL) MASK Register */

/* -------- CMCC_SR : (CMCC Offset: 0x0C) (R/  32) Cache Status Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CSTS:1;           /*!< bit:      0  Cache Controller Status            */
    uint32_t :31;              /*!< bit:  1..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CMCC_SR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CMCC_SR_OFFSET              0x0C         /**< \brief (CMCC_SR offset) Cache Status Register */
#define CMCC_SR_RESETVALUE          _U_(0x00000000) /**< \brief (CMCC_SR reset_value) Cache Status Register */

#define CMCC_SR_CSTS_Pos            0            /**< \brief (CMCC_SR) Cache Controller Status */
#define CMCC_SR_CSTS                (_U_(0x1) << CMCC_SR_CSTS_Pos)
#define CMCC_SR_MASK                _U_(0x00000001) /**< \brief (CMCC_SR) MASK Register */

/* -------- CMCC_LCKWAY : (CMCC Offset: 0x10) (R/W 32) Cache Lock per Way Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t LCKWAY:4;         /*!< bit:  0.. 3  Lockdown way Register              */
    uint32_t :28;              /*!< bit:  4..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CMCC_LCKWAY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CMCC_LCKWAY_OFFSET          0x10         /**< \brief (CMCC_LCKWAY offset) Cache Lock per Way Register */
#define CMCC_LCKWAY_RESETVALUE      _U_(0x00000000) /**< \brief (CMCC_LCKWAY reset_value) Cache Lock per Way Register */

#define CMCC_LCKWAY_LCKWAY_Pos      0            /**< \brief (CMCC_LCKWAY) Lockdown way Register */
#define CMCC_LCKWAY_LCKWAY_Msk      (_U_(0xF) << CMCC_LCKWAY_LCKWAY_Pos)
#define CMCC_LCKWAY_LCKWAY(value)   (CMCC_LCKWAY_LCKWAY_Msk & ((value) << CMCC_LCKWAY_LCKWAY_Pos))
#define CMCC_LCKWAY_MASK            _U_(0x0000000F) /**< \brief (CMCC_LCKWAY) MASK Register */

/* -------- CMCC_MAINT0 : (CMCC Offset: 0x20) ( /W 32) Cache Maintenance Register 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t INVALL:1;         /*!< bit:      0  Cache Controller invalidate All    */
    uint32_t :31;              /*!< bit:  1..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CMCC_MAINT0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CMCC_MAINT0_OFFSET          0x20         /**< \brief (CMCC_MAINT0 offset) Cache Maintenance Register 0 */
#define CMCC_MAINT0_RESETVALUE      _U_(0x00000000) /**< \brief (CMCC_MAINT0 reset_value) Cache Maintenance Register 0 */

#define CMCC_MAINT0_INVALL_Pos      0            /**< \brief (CMCC_MAINT0) Cache Controller invalidate All */
#define CMCC_MAINT0_INVALL          (_U_(0x1) << CMCC_MAINT0_INVALL_Pos)
#define CMCC_MAINT0_MASK            _U_(0x00000001) /**< \brief (CMCC_MAINT0) MASK Register */

/* -------- CMCC_MAINT1 : (CMCC Offset: 0x24) ( /W 32) Cache Maintenance Register 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :4;               /*!< bit:  0.. 3  Reserved                           */
    uint32_t INDEX:8;          /*!< bit:  4..11  Invalidate Index                   */
    uint32_t :16;              /*!< bit: 12..27  Reserved                           */
    uint32_t WAY:4;            /*!< bit: 28..31  Invalidate Way                     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CMCC_MAINT1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CMCC_MAINT1_OFFSET          0x24         /**< \brief (CMCC_MAINT1 offset) Cache Maintenance Register 1 */
#define CMCC_MAINT1_RESETVALUE      _U_(0x00000000) /**< \brief (CMCC_MAINT1 reset_value) Cache Maintenance Register 1 */

#define CMCC_MAINT1_INDEX_Pos       4            /**< \brief (CMCC_MAINT1) Invalidate Index */
#define CMCC_MAINT1_INDEX_Msk       (_U_(0xFF) << CMCC_MAINT1_INDEX_Pos)
#define CMCC_MAINT1_INDEX(value)    (CMCC_MAINT1_INDEX_Msk & ((value) << CMCC_MAINT1_INDEX_Pos))
#define CMCC_MAINT1_WAY_Pos         28           /**< \brief (CMCC_MAINT1) Invalidate Way */
#define CMCC_MAINT1_WAY_Msk         (_U_(0xF) << CMCC_MAINT1_WAY_Pos)
#define CMCC_MAINT1_WAY(value)      (CMCC_MAINT1_WAY_Msk & ((value) << CMCC_MAINT1_WAY_Pos))
#define   CMCC_MAINT1_WAY_WAY0_Val        _U_(0x0)   /**< \brief (CMCC_MAINT1) Way 0 is selection for index invalidation */
#define   CMCC_MAINT1_WAY_WAY1_Val        _U_(0x1)   /**< \brief (CMCC_MAINT1) Way 1 is selection for index invalidation */
#define   CMCC_MAINT1_WAY_WAY2_Val        _U_(0x2)   /**< \brief (CMCC_MAINT1) Way 2 is selection for index invalidation */
#define   CMCC_MAINT1_WAY_WAY3_Val        _U_(0x3)   /**< \brief (CMCC_MAINT1) Way 3 is selection for index invalidation */
#define CMCC_MAINT1_WAY_WAY0        (CMCC_MAINT1_WAY_WAY0_Val      << CMCC_MAINT1_WAY_Pos)
#define CMCC_MAINT1_WAY_WAY1        (CMCC_MAINT1_WAY_WAY1_Val      << CMCC_MAINT1_WAY_Pos)
#define CMCC_MAINT1_WAY_WAY2        (CMCC_MAINT1_WAY_WAY2_Val      << CMCC_MAINT1_WAY_Pos)
#define CMCC_MAINT1_WAY_WAY3        (CMCC_MAINT1_WAY_WAY3_Val      << CMCC_MAINT1_WAY_Pos)
#define CMCC_MAINT1_MASK            _U_(0xF0000FF0) /**< \brief (CMCC_MAINT1) MASK Register */

/* -------- CMCC_MCFG : (CMCC Offset: 0x28) (R/W 32) Cache Monitor Configuration Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t MODE:2;           /*!< bit:  0.. 1  Cache Controller Monitor Counter Mode */
    uint32_t :30;              /*!< bit:  2..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CMCC_MCFG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CMCC_MCFG_OFFSET            0x28         /**< \brief (CMCC_MCFG offset) Cache Monitor Configuration Register */
#define CMCC_MCFG_RESETVALUE        _U_(0x00000000) /**< \brief (CMCC_MCFG reset_value) Cache Monitor Configuration Register */

#define CMCC_MCFG_MODE_Pos          0            /**< \brief (CMCC_MCFG) Cache Controller Monitor Counter Mode */
#define CMCC_MCFG_MODE_Msk          (_U_(0x3) << CMCC_MCFG_MODE_Pos)
#define CMCC_MCFG_MODE(value)       (CMCC_MCFG_MODE_Msk & ((value) << CMCC_MCFG_MODE_Pos))
#define   CMCC_MCFG_MODE_CYCLE_COUNT_Val  _U_(0x0)   /**< \brief (CMCC_MCFG) cycle counter */
#define   CMCC_MCFG_MODE_IHIT_COUNT_Val   _U_(0x1)   /**< \brief (CMCC_MCFG) instruction hit counter */
#define   CMCC_MCFG_MODE_DHIT_COUNT_Val   _U_(0x2)   /**< \brief (CMCC_MCFG) data hit counter */
#define CMCC_MCFG_MODE_CYCLE_COUNT  (CMCC_MCFG_MODE_CYCLE_COUNT_Val << CMCC_MCFG_MODE_Pos)
#define CMCC_MCFG_MODE_IHIT_COUNT   (CMCC_MCFG_MODE_IHIT_COUNT_Val << CMCC_MCFG_MODE_Pos)
#define CMCC_MCFG_MODE_DHIT_COUNT   (CMCC_MCFG_MODE_DHIT_COUNT_Val << CMCC_MCFG_MODE_Pos)
#define CMCC_MCFG_MASK              _U_(0x00000003) /**< \brief (CMCC_MCFG) MASK Register */

/* -------- CMCC_MEN : (CMCC Offset: 0x2C) (R/W 32) Cache Monitor Enable Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t MENABLE:1;        /*!< bit:      0  Cache Controller Monitor Enable    */
    uint32_t :31;              /*!< bit:  1..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CMCC_MEN_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CMCC_MEN_OFFSET             0x2C         /**< \brief (CMCC_MEN offset) Cache Monitor Enable Register */
#define CMCC_MEN_RESETVALUE         _U_(0x00000000) /**< \brief (CMCC_MEN reset_value) Cache Monitor Enable Register */

#define CMCC_MEN_MENABLE_Pos        0            /**< \brief (CMCC_MEN) Cache Controller Monitor Enable */
#define CMCC_MEN_MENABLE            (_U_(0x1) << CMCC_MEN_MENABLE_Pos)
#define CMCC_MEN_MASK               _U_(0x00000001) /**< \brief (CMCC_MEN) MASK Register */

/* -------- CMCC_MCTRL : (CMCC Offset: 0x30) ( /W 32) Cache Monitor Control Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Cache Controller Software Reset    */
    uint32_t :31;              /*!< bit:  1..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CMCC_MCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CMCC_MCTRL_OFFSET           0x30         /**< \brief (CMCC_MCTRL offset) Cache Monitor Control Register */
#define CMCC_MCTRL_RESETVALUE       _U_(0x00000000) /**< \brief (CMCC_MCTRL reset_value) Cache Monitor Control Register */

#define CMCC_MCTRL_SWRST_Pos        0            /**< \brief (CMCC_MCTRL) Cache Controller Software Reset */
#define CMCC_MCTRL_SWRST            (_U_(0x1) << CMCC_MCTRL_SWRST_Pos)
#define CMCC_MCTRL_MASK             _U_(0x00000001) /**< \brief (CMCC_MCTRL) MASK Register */

/* -------- CMCC_MSR : (CMCC Offset: 0x34) (R/  32) Cache Monitor Status Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EVENT_CNT:32;     /*!< bit:  0..31  Monitor Event Counter              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CMCC_MSR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CMCC_MSR_OFFSET             0x34         /**< \brief (CMCC_MSR offset) Cache Monitor Status Register */
#define CMCC_MSR_RESETVALUE         _U_(0x00000000) /**< \brief (CMCC_MSR reset_value) Cache Monitor Status Register */

#define CMCC_MSR_EVENT_CNT_Pos      0            /**< \brief (CMCC_MSR) Monitor Event Counter */
#define CMCC_MSR_EVENT_CNT_Msk      (_U_(0xFFFFFFFF) << CMCC_MSR_EVENT_CNT_Pos)
#define CMCC_MSR_EVENT_CNT(value)   (CMCC_MSR_EVENT_CNT_Msk & ((value) << CMCC_MSR_EVENT_CNT_Pos))
#define CMCC_MSR_MASK               _U_(0xFFFFFFFF) /**< \brief (CMCC_MSR) MASK Register */

/** \brief CMCC APB hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __I  CMCC_TYPE_Type            TYPE;        /**< \brief Offset: 0x00 (R/  32) Cache Type Register */
  __IO CMCC_CFG_Type             CFG;         /**< \brief Offset: 0x04 (R/W 32) Cache Configuration Register */
  __O  CMCC_CTRL_Type            CTRL;        /**< \brief Offset: 0x08 ( /W 32) Cache Control Register */
  __I  CMCC_SR_Type              SR;          /**< \brief Offset: 0x0C (R/  32) Cache Status Register */
  __IO CMCC_LCKWAY_Type          LCKWAY;      /**< \brief Offset: 0x10 (R/W 32) Cache Lock per Way Register */
       RoReg8                    Reserved1[0xC];
  __O  CMCC_MAINT0_Type          MAINT0;      /**< \brief Offset: 0x20 ( /W 32) Cache Maintenance Register 0 */
  __O  CMCC_MAINT1_Type          MAINT1;      /**< \brief Offset: 0x24 ( /W 32) Cache Maintenance Register 1 */
  __IO CMCC_MCFG_Type            MCFG;        /**< \brief Offset: 0x28 (R/W 32) Cache Monitor Configuration Register */
  __IO CMCC_MEN_Type             MEN;         /**< \brief Offset: 0x2C (R/W 32) Cache Monitor Enable Register */
  __O  CMCC_MCTRL_Type           MCTRL;       /**< \brief Offset: 0x30 ( /W 32) Cache Monitor Control Register */
  __I  CMCC_MSR_Type             MSR;         /**< \brief Offset: 0x34 (R/  32) Cache Monitor Status Register */
} Cmcc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_CMCC_COMPONENT_ */
