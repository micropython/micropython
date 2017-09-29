/**
 * \file
 *
 * \brief Component description for NVMCTRL
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

#ifndef _SAMD51_NVMCTRL_COMPONENT_
#define _SAMD51_NVMCTRL_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR NVMCTRL */
/* ========================================================================== */
/** \addtogroup SAMD51_NVMCTRL Non-Volatile Memory Controller */
/*@{*/

#define NVMCTRL_U2409
#define REV_NVMCTRL                 0x100

/* -------- NVMCTRL_CTRLA : (NVMCTRL Offset: 0x00) (R/W 16) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t :2;               /*!< bit:  0.. 1  Reserved                           */
    uint16_t AUTOWS:1;         /*!< bit:      2  Auto Wait State Enable             */
    uint16_t SUSPEN:1;         /*!< bit:      3  Suspend Enable                     */
    uint16_t WMODE:2;          /*!< bit:  4.. 5  Write Mode                         */
    uint16_t PRM:2;            /*!< bit:  6.. 7  Power Reduction Mode during Sleep  */
    uint16_t RWS:4;            /*!< bit:  8..11  NVM Read Wait States               */
    uint16_t AHBNS0:1;         /*!< bit:     12  Force AHB0 access to NONSEQ, burst transfers are continuously rearbitrated */
    uint16_t AHBNS1:1;         /*!< bit:     13  Force AHB1 access to NONSEQ, burst transfers are continuously rearbitrated */
    uint16_t CACHEDIS0:1;      /*!< bit:     14  AHB0 Cache Disable                 */
    uint16_t CACHEDIS1:1;      /*!< bit:     15  AHB1 Cache Disable                 */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} NVMCTRL_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define NVMCTRL_CTRLA_OFFSET        0x00         /**< \brief (NVMCTRL_CTRLA offset) Control A */
#define NVMCTRL_CTRLA_RESETVALUE    _U_(0x0004)  /**< \brief (NVMCTRL_CTRLA reset_value) Control A */

#define NVMCTRL_CTRLA_AUTOWS_Pos    2            /**< \brief (NVMCTRL_CTRLA) Auto Wait State Enable */
#define NVMCTRL_CTRLA_AUTOWS        (_U_(0x1) << NVMCTRL_CTRLA_AUTOWS_Pos)
#define NVMCTRL_CTRLA_SUSPEN_Pos    3            /**< \brief (NVMCTRL_CTRLA) Suspend Enable */
#define NVMCTRL_CTRLA_SUSPEN        (_U_(0x1) << NVMCTRL_CTRLA_SUSPEN_Pos)
#define NVMCTRL_CTRLA_WMODE_Pos     4            /**< \brief (NVMCTRL_CTRLA) Write Mode */
#define NVMCTRL_CTRLA_WMODE_Msk     (_U_(0x3) << NVMCTRL_CTRLA_WMODE_Pos)
#define NVMCTRL_CTRLA_WMODE(value)  (NVMCTRL_CTRLA_WMODE_Msk & ((value) << NVMCTRL_CTRLA_WMODE_Pos))
#define   NVMCTRL_CTRLA_WMODE_MAN_Val     _U_(0x0)   /**< \brief (NVMCTRL_CTRLA) Manual Write */
#define   NVMCTRL_CTRLA_WMODE_ADW_Val     _U_(0x1)   /**< \brief (NVMCTRL_CTRLA) Automatic Double Word Write */
#define   NVMCTRL_CTRLA_WMODE_AQW_Val     _U_(0x2)   /**< \brief (NVMCTRL_CTRLA) Automatic Quad Word */
#define   NVMCTRL_CTRLA_WMODE_AP_Val      _U_(0x3)   /**< \brief (NVMCTRL_CTRLA) Automatic Page Write */
#define NVMCTRL_CTRLA_WMODE_MAN     (NVMCTRL_CTRLA_WMODE_MAN_Val   << NVMCTRL_CTRLA_WMODE_Pos)
#define NVMCTRL_CTRLA_WMODE_ADW     (NVMCTRL_CTRLA_WMODE_ADW_Val   << NVMCTRL_CTRLA_WMODE_Pos)
#define NVMCTRL_CTRLA_WMODE_AQW     (NVMCTRL_CTRLA_WMODE_AQW_Val   << NVMCTRL_CTRLA_WMODE_Pos)
#define NVMCTRL_CTRLA_WMODE_AP      (NVMCTRL_CTRLA_WMODE_AP_Val    << NVMCTRL_CTRLA_WMODE_Pos)
#define NVMCTRL_CTRLA_PRM_Pos       6            /**< \brief (NVMCTRL_CTRLA) Power Reduction Mode during Sleep */
#define NVMCTRL_CTRLA_PRM_Msk       (_U_(0x3) << NVMCTRL_CTRLA_PRM_Pos)
#define NVMCTRL_CTRLA_PRM(value)    (NVMCTRL_CTRLA_PRM_Msk & ((value) << NVMCTRL_CTRLA_PRM_Pos))
#define   NVMCTRL_CTRLA_PRM_SEMIAUTO_Val  _U_(0x0)   /**< \brief (NVMCTRL_CTRLA) NVM block enters low-power mode when entering standby mode. NVM block enters low-power mode when SPRM command is issued. NVM block exits low-power mode upon first access. */
#define   NVMCTRL_CTRLA_PRM_FULLAUTO_Val  _U_(0x1)   /**< \brief (NVMCTRL_CTRLA) NVM block enters low-power mode when entering standby mode. NVM block enters low-power mode when SPRM command is issued. NVM block exits low-power mode when system is not in standby mode. */
#define   NVMCTRL_CTRLA_PRM_MANUAL_Val    _U_(0x3)   /**< \brief (NVMCTRL_CTRLA) NVM block does not enter low-power mode when entering standby mode. NVM block enters low-power mode when SPRM command is issued. NVM block exits low-power mode upon first access. */
#define NVMCTRL_CTRLA_PRM_SEMIAUTO  (NVMCTRL_CTRLA_PRM_SEMIAUTO_Val << NVMCTRL_CTRLA_PRM_Pos)
#define NVMCTRL_CTRLA_PRM_FULLAUTO  (NVMCTRL_CTRLA_PRM_FULLAUTO_Val << NVMCTRL_CTRLA_PRM_Pos)
#define NVMCTRL_CTRLA_PRM_MANUAL    (NVMCTRL_CTRLA_PRM_MANUAL_Val  << NVMCTRL_CTRLA_PRM_Pos)
#define NVMCTRL_CTRLA_RWS_Pos       8            /**< \brief (NVMCTRL_CTRLA) NVM Read Wait States */
#define NVMCTRL_CTRLA_RWS_Msk       (_U_(0xF) << NVMCTRL_CTRLA_RWS_Pos)
#define NVMCTRL_CTRLA_RWS(value)    (NVMCTRL_CTRLA_RWS_Msk & ((value) << NVMCTRL_CTRLA_RWS_Pos))
#define NVMCTRL_CTRLA_AHBNS0_Pos    12           /**< \brief (NVMCTRL_CTRLA) Force AHB0 access to NONSEQ, burst transfers are continuously rearbitrated */
#define NVMCTRL_CTRLA_AHBNS0        (_U_(0x1) << NVMCTRL_CTRLA_AHBNS0_Pos)
#define NVMCTRL_CTRLA_AHBNS1_Pos    13           /**< \brief (NVMCTRL_CTRLA) Force AHB1 access to NONSEQ, burst transfers are continuously rearbitrated */
#define NVMCTRL_CTRLA_AHBNS1        (_U_(0x1) << NVMCTRL_CTRLA_AHBNS1_Pos)
#define NVMCTRL_CTRLA_CACHEDIS0_Pos 14           /**< \brief (NVMCTRL_CTRLA) AHB0 Cache Disable */
#define NVMCTRL_CTRLA_CACHEDIS0     (_U_(0x1) << NVMCTRL_CTRLA_CACHEDIS0_Pos)
#define NVMCTRL_CTRLA_CACHEDIS1_Pos 15           /**< \brief (NVMCTRL_CTRLA) AHB1 Cache Disable */
#define NVMCTRL_CTRLA_CACHEDIS1     (_U_(0x1) << NVMCTRL_CTRLA_CACHEDIS1_Pos)
#define NVMCTRL_CTRLA_MASK          _U_(0xFFFC)  /**< \brief (NVMCTRL_CTRLA) MASK Register */

/* -------- NVMCTRL_CTRLB : (NVMCTRL Offset: 0x04) ( /W 16) Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t CMD:7;            /*!< bit:  0.. 6  Command                            */
    uint16_t :1;               /*!< bit:      7  Reserved                           */
    uint16_t CMDEX:8;          /*!< bit:  8..15  Command Execution                  */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} NVMCTRL_CTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define NVMCTRL_CTRLB_OFFSET        0x04         /**< \brief (NVMCTRL_CTRLB offset) Control B */
#define NVMCTRL_CTRLB_RESETVALUE    _U_(0x0000)  /**< \brief (NVMCTRL_CTRLB reset_value) Control B */

#define NVMCTRL_CTRLB_CMD_Pos       0            /**< \brief (NVMCTRL_CTRLB) Command */
#define NVMCTRL_CTRLB_CMD_Msk       (_U_(0x7F) << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD(value)    (NVMCTRL_CTRLB_CMD_Msk & ((value) << NVMCTRL_CTRLB_CMD_Pos))
#define   NVMCTRL_CTRLB_CMD_EP_Val        _U_(0x0)   /**< \brief (NVMCTRL_CTRLB) Erase Page - Only supported in the USER and AUX pages. */
#define   NVMCTRL_CTRLB_CMD_EB_Val        _U_(0x1)   /**< \brief (NVMCTRL_CTRLB) Erase Block - Erases the block addressed by the ADDR register, not supported in the user page */
#define   NVMCTRL_CTRLB_CMD_WP_Val        _U_(0x3)   /**< \brief (NVMCTRL_CTRLB) Write Page - Writes the contents of the page buffer to the page addressed by the ADDR register, not supported in the user page */
#define   NVMCTRL_CTRLB_CMD_WQW_Val       _U_(0x4)   /**< \brief (NVMCTRL_CTRLB) Write Quad Word - Writes a 128-bit word at the location addressed by the ADDR register. */
#define   NVMCTRL_CTRLB_CMD_SWRST_Val     _U_(0x10)   /**< \brief (NVMCTRL_CTRLB) Software Reset - Power-Cycle the NVM memory and replay the device automatic calibration procedure and resets the module configuration registers */
#define   NVMCTRL_CTRLB_CMD_LR_Val        _U_(0x11)   /**< \brief (NVMCTRL_CTRLB) Lock Region - Locks the region containing the address location in the ADDR register. */
#define   NVMCTRL_CTRLB_CMD_UR_Val        _U_(0x12)   /**< \brief (NVMCTRL_CTRLB) Unlock Region - Unlocks the region containing the address location in the ADDR register. */
#define   NVMCTRL_CTRLB_CMD_SPRM_Val      _U_(0x13)   /**< \brief (NVMCTRL_CTRLB) Sets the power reduction mode. */
#define   NVMCTRL_CTRLB_CMD_CPRM_Val      _U_(0x14)   /**< \brief (NVMCTRL_CTRLB) Clears the power reduction mode. */
#define   NVMCTRL_CTRLB_CMD_PBC_Val       _U_(0x15)   /**< \brief (NVMCTRL_CTRLB) Page Buffer Clear - Clears the page buffer. */
#define   NVMCTRL_CTRLB_CMD_SSB_Val       _U_(0x16)   /**< \brief (NVMCTRL_CTRLB) Set Security Bit */
#define   NVMCTRL_CTRLB_CMD_BKSWRST_Val   _U_(0x17)   /**< \brief (NVMCTRL_CTRLB) Bank swap and system reset, if SMEE is used also reallocate SMEE data into the opposite BANK */
#define   NVMCTRL_CTRLB_CMD_CELCK_Val     _U_(0x18)   /**< \brief (NVMCTRL_CTRLB) Chip Erase Lock - DSU.CE command is not available */
#define   NVMCTRL_CTRLB_CMD_CEULCK_Val    _U_(0x19)   /**< \brief (NVMCTRL_CTRLB) Chip Erase Unlock - DSU.CE command is available */
#define   NVMCTRL_CTRLB_CMD_SBPDIS_Val    _U_(0x1A)   /**< \brief (NVMCTRL_CTRLB) Sets STATUS.BPDIS, Boot loader protection is discarded until CBPDIS is issued or next start-up sequence */
#define   NVMCTRL_CTRLB_CMD_CBPDIS_Val    _U_(0x1B)   /**< \brief (NVMCTRL_CTRLB) Clears STATUS.BPDIS, Boot loader protection is not discarded */
#define   NVMCTRL_CTRLB_CMD_ASEES0_Val    _U_(0x30)   /**< \brief (NVMCTRL_CTRLB) Activate SmartEEPROM Sector 0, deactivate Sector 1 */
#define   NVMCTRL_CTRLB_CMD_ASEES1_Val    _U_(0x31)   /**< \brief (NVMCTRL_CTRLB) Activate SmartEEPROM Sector 1, deactivate Sector 0 */
#define   NVMCTRL_CTRLB_CMD_SEERALOC_Val  _U_(0x32)   /**< \brief (NVMCTRL_CTRLB) Starts SmartEEPROM sector reallocation algorithm */
#define   NVMCTRL_CTRLB_CMD_SEEFLUSH_Val  _U_(0x33)   /**< \brief (NVMCTRL_CTRLB) Flush SMEE data when in buffered mode */
#define   NVMCTRL_CTRLB_CMD_LSEE_Val      _U_(0x34)   /**< \brief (NVMCTRL_CTRLB) Lock access to SmartEEPROM data from any mean */
#define   NVMCTRL_CTRLB_CMD_USEE_Val      _U_(0x35)   /**< \brief (NVMCTRL_CTRLB) Unlock access to SmartEEPROM data */
#define   NVMCTRL_CTRLB_CMD_LSEER_Val     _U_(0x36)   /**< \brief (NVMCTRL_CTRLB) Lock access to the SmartEEPROM Register Address Space (above 64KB) */
#define   NVMCTRL_CTRLB_CMD_USEER_Val     _U_(0x37)   /**< \brief (NVMCTRL_CTRLB) Unlock access to the SmartEEPROM Register Address Space (above 64KB) */
#define NVMCTRL_CTRLB_CMD_EP        (NVMCTRL_CTRLB_CMD_EP_Val      << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_EB        (NVMCTRL_CTRLB_CMD_EB_Val      << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_WP        (NVMCTRL_CTRLB_CMD_WP_Val      << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_WQW       (NVMCTRL_CTRLB_CMD_WQW_Val     << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_SWRST     (NVMCTRL_CTRLB_CMD_SWRST_Val   << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_LR        (NVMCTRL_CTRLB_CMD_LR_Val      << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_UR        (NVMCTRL_CTRLB_CMD_UR_Val      << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_SPRM      (NVMCTRL_CTRLB_CMD_SPRM_Val    << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_CPRM      (NVMCTRL_CTRLB_CMD_CPRM_Val    << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_PBC       (NVMCTRL_CTRLB_CMD_PBC_Val     << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_SSB       (NVMCTRL_CTRLB_CMD_SSB_Val     << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_BKSWRST   (NVMCTRL_CTRLB_CMD_BKSWRST_Val << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_CELCK     (NVMCTRL_CTRLB_CMD_CELCK_Val   << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_CEULCK    (NVMCTRL_CTRLB_CMD_CEULCK_Val  << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_SBPDIS    (NVMCTRL_CTRLB_CMD_SBPDIS_Val  << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_CBPDIS    (NVMCTRL_CTRLB_CMD_CBPDIS_Val  << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_ASEES0    (NVMCTRL_CTRLB_CMD_ASEES0_Val  << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_ASEES1    (NVMCTRL_CTRLB_CMD_ASEES1_Val  << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_SEERALOC  (NVMCTRL_CTRLB_CMD_SEERALOC_Val << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_SEEFLUSH  (NVMCTRL_CTRLB_CMD_SEEFLUSH_Val << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_LSEE      (NVMCTRL_CTRLB_CMD_LSEE_Val    << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_USEE      (NVMCTRL_CTRLB_CMD_USEE_Val    << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_LSEER     (NVMCTRL_CTRLB_CMD_LSEER_Val   << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMD_USEER     (NVMCTRL_CTRLB_CMD_USEER_Val   << NVMCTRL_CTRLB_CMD_Pos)
#define NVMCTRL_CTRLB_CMDEX_Pos     8            /**< \brief (NVMCTRL_CTRLB) Command Execution */
#define NVMCTRL_CTRLB_CMDEX_Msk     (_U_(0xFF) << NVMCTRL_CTRLB_CMDEX_Pos)
#define NVMCTRL_CTRLB_CMDEX(value)  (NVMCTRL_CTRLB_CMDEX_Msk & ((value) << NVMCTRL_CTRLB_CMDEX_Pos))
#define   NVMCTRL_CTRLB_CMDEX_KEY_Val     _U_(0xA5)   /**< \brief (NVMCTRL_CTRLB) Execution Key */
#define NVMCTRL_CTRLB_CMDEX_KEY     (NVMCTRL_CTRLB_CMDEX_KEY_Val   << NVMCTRL_CTRLB_CMDEX_Pos)
#define NVMCTRL_CTRLB_MASK          _U_(0xFF7F)  /**< \brief (NVMCTRL_CTRLB) MASK Register */

/* -------- NVMCTRL_PARAM : (NVMCTRL Offset: 0x08) (R/  32) NVM Parameter -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t NVMP:16;          /*!< bit:  0..15  NVM Pages                          */
    uint32_t PSZ:3;            /*!< bit: 16..18  Page Size                          */
    uint32_t :12;              /*!< bit: 19..30  Reserved                           */
    uint32_t SEE:1;            /*!< bit:     31  SmartEEPROM Supported              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} NVMCTRL_PARAM_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define NVMCTRL_PARAM_OFFSET        0x08         /**< \brief (NVMCTRL_PARAM offset) NVM Parameter */
#define NVMCTRL_PARAM_RESETVALUE    _U_(0x00060000) /**< \brief (NVMCTRL_PARAM reset_value) NVM Parameter */

#define NVMCTRL_PARAM_NVMP_Pos      0            /**< \brief (NVMCTRL_PARAM) NVM Pages */
#define NVMCTRL_PARAM_NVMP_Msk      (_U_(0xFFFF) << NVMCTRL_PARAM_NVMP_Pos)
#define NVMCTRL_PARAM_NVMP(value)   (NVMCTRL_PARAM_NVMP_Msk & ((value) << NVMCTRL_PARAM_NVMP_Pos))
#define NVMCTRL_PARAM_PSZ_Pos       16           /**< \brief (NVMCTRL_PARAM) Page Size */
#define NVMCTRL_PARAM_PSZ_Msk       (_U_(0x7) << NVMCTRL_PARAM_PSZ_Pos)
#define NVMCTRL_PARAM_PSZ(value)    (NVMCTRL_PARAM_PSZ_Msk & ((value) << NVMCTRL_PARAM_PSZ_Pos))
#define   NVMCTRL_PARAM_PSZ_8_Val         _U_(0x0)   /**< \brief (NVMCTRL_PARAM) 8 bytes */
#define   NVMCTRL_PARAM_PSZ_16_Val        _U_(0x1)   /**< \brief (NVMCTRL_PARAM) 16 bytes */
#define   NVMCTRL_PARAM_PSZ_32_Val        _U_(0x2)   /**< \brief (NVMCTRL_PARAM) 32 bytes */
#define   NVMCTRL_PARAM_PSZ_64_Val        _U_(0x3)   /**< \brief (NVMCTRL_PARAM) 64 bytes */
#define   NVMCTRL_PARAM_PSZ_128_Val       _U_(0x4)   /**< \brief (NVMCTRL_PARAM) 128 bytes */
#define   NVMCTRL_PARAM_PSZ_256_Val       _U_(0x5)   /**< \brief (NVMCTRL_PARAM) 256 bytes */
#define   NVMCTRL_PARAM_PSZ_512_Val       _U_(0x6)   /**< \brief (NVMCTRL_PARAM) 512 bytes */
#define   NVMCTRL_PARAM_PSZ_1024_Val      _U_(0x7)   /**< \brief (NVMCTRL_PARAM) 1024 bytes */
#define NVMCTRL_PARAM_PSZ_8         (NVMCTRL_PARAM_PSZ_8_Val       << NVMCTRL_PARAM_PSZ_Pos)
#define NVMCTRL_PARAM_PSZ_16        (NVMCTRL_PARAM_PSZ_16_Val      << NVMCTRL_PARAM_PSZ_Pos)
#define NVMCTRL_PARAM_PSZ_32        (NVMCTRL_PARAM_PSZ_32_Val      << NVMCTRL_PARAM_PSZ_Pos)
#define NVMCTRL_PARAM_PSZ_64        (NVMCTRL_PARAM_PSZ_64_Val      << NVMCTRL_PARAM_PSZ_Pos)
#define NVMCTRL_PARAM_PSZ_128       (NVMCTRL_PARAM_PSZ_128_Val     << NVMCTRL_PARAM_PSZ_Pos)
#define NVMCTRL_PARAM_PSZ_256       (NVMCTRL_PARAM_PSZ_256_Val     << NVMCTRL_PARAM_PSZ_Pos)
#define NVMCTRL_PARAM_PSZ_512       (NVMCTRL_PARAM_PSZ_512_Val     << NVMCTRL_PARAM_PSZ_Pos)
#define NVMCTRL_PARAM_PSZ_1024      (NVMCTRL_PARAM_PSZ_1024_Val    << NVMCTRL_PARAM_PSZ_Pos)
#define NVMCTRL_PARAM_SEE_Pos       31           /**< \brief (NVMCTRL_PARAM) SmartEEPROM Supported */
#define NVMCTRL_PARAM_SEE           (_U_(0x1) << NVMCTRL_PARAM_SEE_Pos)
#define NVMCTRL_PARAM_MASK          _U_(0x8007FFFF) /**< \brief (NVMCTRL_PARAM) MASK Register */

/* -------- NVMCTRL_INTENCLR : (NVMCTRL Offset: 0x0C) (R/W 16) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t DONE:1;           /*!< bit:      0  Command Done Interrupt Clear       */
    uint16_t ADDRE:1;          /*!< bit:      1  Address Error                      */
    uint16_t PROGE:1;          /*!< bit:      2  Programming Error Interrupt Clear  */
    uint16_t LOCKE:1;          /*!< bit:      3  Lock Error Interrupt Clear         */
    uint16_t ECCSE:1;          /*!< bit:      4  ECC Single Error Interrupt Clear   */
    uint16_t ECCDE:1;          /*!< bit:      5  ECC Dual Error Interrupt Clear     */
    uint16_t NVME:1;           /*!< bit:      6  NVM Error Interrupt Clear          */
    uint16_t SUSP:1;           /*!< bit:      7  Suspended Write Or Erase Interrupt Clear */
    uint16_t SEESFULL:1;       /*!< bit:      8  Active SEES Full Interrupt Clear   */
    uint16_t SEESOVF:1;        /*!< bit:      9  Active SEES Overflow Interrupt Clear */
    uint16_t SEEWRC:1;         /*!< bit:     10  SEE Write Completed Interrupt Clear */
    uint16_t :5;               /*!< bit: 11..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} NVMCTRL_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define NVMCTRL_INTENCLR_OFFSET     0x0C         /**< \brief (NVMCTRL_INTENCLR offset) Interrupt Enable Clear */
#define NVMCTRL_INTENCLR_RESETVALUE _U_(0x0000)  /**< \brief (NVMCTRL_INTENCLR reset_value) Interrupt Enable Clear */

#define NVMCTRL_INTENCLR_DONE_Pos   0            /**< \brief (NVMCTRL_INTENCLR) Command Done Interrupt Clear */
#define NVMCTRL_INTENCLR_DONE       (_U_(0x1) << NVMCTRL_INTENCLR_DONE_Pos)
#define NVMCTRL_INTENCLR_ADDRE_Pos  1            /**< \brief (NVMCTRL_INTENCLR) Address Error */
#define NVMCTRL_INTENCLR_ADDRE      (_U_(0x1) << NVMCTRL_INTENCLR_ADDRE_Pos)
#define NVMCTRL_INTENCLR_PROGE_Pos  2            /**< \brief (NVMCTRL_INTENCLR) Programming Error Interrupt Clear */
#define NVMCTRL_INTENCLR_PROGE      (_U_(0x1) << NVMCTRL_INTENCLR_PROGE_Pos)
#define NVMCTRL_INTENCLR_LOCKE_Pos  3            /**< \brief (NVMCTRL_INTENCLR) Lock Error Interrupt Clear */
#define NVMCTRL_INTENCLR_LOCKE      (_U_(0x1) << NVMCTRL_INTENCLR_LOCKE_Pos)
#define NVMCTRL_INTENCLR_ECCSE_Pos  4            /**< \brief (NVMCTRL_INTENCLR) ECC Single Error Interrupt Clear */
#define NVMCTRL_INTENCLR_ECCSE      (_U_(0x1) << NVMCTRL_INTENCLR_ECCSE_Pos)
#define NVMCTRL_INTENCLR_ECCDE_Pos  5            /**< \brief (NVMCTRL_INTENCLR) ECC Dual Error Interrupt Clear */
#define NVMCTRL_INTENCLR_ECCDE      (_U_(0x1) << NVMCTRL_INTENCLR_ECCDE_Pos)
#define NVMCTRL_INTENCLR_NVME_Pos   6            /**< \brief (NVMCTRL_INTENCLR) NVM Error Interrupt Clear */
#define NVMCTRL_INTENCLR_NVME       (_U_(0x1) << NVMCTRL_INTENCLR_NVME_Pos)
#define NVMCTRL_INTENCLR_SUSP_Pos   7            /**< \brief (NVMCTRL_INTENCLR) Suspended Write Or Erase Interrupt Clear */
#define NVMCTRL_INTENCLR_SUSP       (_U_(0x1) << NVMCTRL_INTENCLR_SUSP_Pos)
#define NVMCTRL_INTENCLR_SEESFULL_Pos 8            /**< \brief (NVMCTRL_INTENCLR) Active SEES Full Interrupt Clear */
#define NVMCTRL_INTENCLR_SEESFULL   (_U_(0x1) << NVMCTRL_INTENCLR_SEESFULL_Pos)
#define NVMCTRL_INTENCLR_SEESOVF_Pos 9            /**< \brief (NVMCTRL_INTENCLR) Active SEES Overflow Interrupt Clear */
#define NVMCTRL_INTENCLR_SEESOVF    (_U_(0x1) << NVMCTRL_INTENCLR_SEESOVF_Pos)
#define NVMCTRL_INTENCLR_SEEWRC_Pos 10           /**< \brief (NVMCTRL_INTENCLR) SEE Write Completed Interrupt Clear */
#define NVMCTRL_INTENCLR_SEEWRC     (_U_(0x1) << NVMCTRL_INTENCLR_SEEWRC_Pos)
#define NVMCTRL_INTENCLR_MASK       _U_(0x07FF)  /**< \brief (NVMCTRL_INTENCLR) MASK Register */

/* -------- NVMCTRL_INTENSET : (NVMCTRL Offset: 0x0E) (R/W 16) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t DONE:1;           /*!< bit:      0  Command Done Interrupt Enable      */
    uint16_t ADDRE:1;          /*!< bit:      1  Address Error Interrupt Enable     */
    uint16_t PROGE:1;          /*!< bit:      2  Programming Error Interrupt Enable */
    uint16_t LOCKE:1;          /*!< bit:      3  Lock Error Interrupt Enable        */
    uint16_t ECCSE:1;          /*!< bit:      4  ECC Single Error Interrupt Enable  */
    uint16_t ECCDE:1;          /*!< bit:      5  ECC Dual Error Interrupt Enable    */
    uint16_t NVME:1;           /*!< bit:      6  NVM Error Interrupt Enable         */
    uint16_t SUSP:1;           /*!< bit:      7  Suspended Write Or Erase  Interrupt Enable */
    uint16_t SEESFULL:1;       /*!< bit:      8  Active SEES Full Interrupt Enable  */
    uint16_t SEESOVF:1;        /*!< bit:      9  Active SEES Overflow Interrupt Enable */
    uint16_t SEEWRC:1;         /*!< bit:     10  SEE Write Completed Interrupt Enable */
    uint16_t :5;               /*!< bit: 11..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} NVMCTRL_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define NVMCTRL_INTENSET_OFFSET     0x0E         /**< \brief (NVMCTRL_INTENSET offset) Interrupt Enable Set */
#define NVMCTRL_INTENSET_RESETVALUE _U_(0x0000)  /**< \brief (NVMCTRL_INTENSET reset_value) Interrupt Enable Set */

#define NVMCTRL_INTENSET_DONE_Pos   0            /**< \brief (NVMCTRL_INTENSET) Command Done Interrupt Enable */
#define NVMCTRL_INTENSET_DONE       (_U_(0x1) << NVMCTRL_INTENSET_DONE_Pos)
#define NVMCTRL_INTENSET_ADDRE_Pos  1            /**< \brief (NVMCTRL_INTENSET) Address Error Interrupt Enable */
#define NVMCTRL_INTENSET_ADDRE      (_U_(0x1) << NVMCTRL_INTENSET_ADDRE_Pos)
#define NVMCTRL_INTENSET_PROGE_Pos  2            /**< \brief (NVMCTRL_INTENSET) Programming Error Interrupt Enable */
#define NVMCTRL_INTENSET_PROGE      (_U_(0x1) << NVMCTRL_INTENSET_PROGE_Pos)
#define NVMCTRL_INTENSET_LOCKE_Pos  3            /**< \brief (NVMCTRL_INTENSET) Lock Error Interrupt Enable */
#define NVMCTRL_INTENSET_LOCKE      (_U_(0x1) << NVMCTRL_INTENSET_LOCKE_Pos)
#define NVMCTRL_INTENSET_ECCSE_Pos  4            /**< \brief (NVMCTRL_INTENSET) ECC Single Error Interrupt Enable */
#define NVMCTRL_INTENSET_ECCSE      (_U_(0x1) << NVMCTRL_INTENSET_ECCSE_Pos)
#define NVMCTRL_INTENSET_ECCDE_Pos  5            /**< \brief (NVMCTRL_INTENSET) ECC Dual Error Interrupt Enable */
#define NVMCTRL_INTENSET_ECCDE      (_U_(0x1) << NVMCTRL_INTENSET_ECCDE_Pos)
#define NVMCTRL_INTENSET_NVME_Pos   6            /**< \brief (NVMCTRL_INTENSET) NVM Error Interrupt Enable */
#define NVMCTRL_INTENSET_NVME       (_U_(0x1) << NVMCTRL_INTENSET_NVME_Pos)
#define NVMCTRL_INTENSET_SUSP_Pos   7            /**< \brief (NVMCTRL_INTENSET) Suspended Write Or Erase  Interrupt Enable */
#define NVMCTRL_INTENSET_SUSP       (_U_(0x1) << NVMCTRL_INTENSET_SUSP_Pos)
#define NVMCTRL_INTENSET_SEESFULL_Pos 8            /**< \brief (NVMCTRL_INTENSET) Active SEES Full Interrupt Enable */
#define NVMCTRL_INTENSET_SEESFULL   (_U_(0x1) << NVMCTRL_INTENSET_SEESFULL_Pos)
#define NVMCTRL_INTENSET_SEESOVF_Pos 9            /**< \brief (NVMCTRL_INTENSET) Active SEES Overflow Interrupt Enable */
#define NVMCTRL_INTENSET_SEESOVF    (_U_(0x1) << NVMCTRL_INTENSET_SEESOVF_Pos)
#define NVMCTRL_INTENSET_SEEWRC_Pos 10           /**< \brief (NVMCTRL_INTENSET) SEE Write Completed Interrupt Enable */
#define NVMCTRL_INTENSET_SEEWRC     (_U_(0x1) << NVMCTRL_INTENSET_SEEWRC_Pos)
#define NVMCTRL_INTENSET_MASK       _U_(0x07FF)  /**< \brief (NVMCTRL_INTENSET) MASK Register */

/* -------- NVMCTRL_INTFLAG : (NVMCTRL Offset: 0x10) (R/W 16) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint16_t DONE:1;           /*!< bit:      0  Command Done                       */
    __I uint16_t ADDRE:1;          /*!< bit:      1  Address Error                      */
    __I uint16_t PROGE:1;          /*!< bit:      2  Programming Error                  */
    __I uint16_t LOCKE:1;          /*!< bit:      3  Lock Error                         */
    __I uint16_t ECCSE:1;          /*!< bit:      4  ECC Single Error                   */
    __I uint16_t ECCDE:1;          /*!< bit:      5  ECC Dual Error                     */
    __I uint16_t NVME:1;           /*!< bit:      6  NVM Error                          */
    __I uint16_t SUSP:1;           /*!< bit:      7  Suspended Write Or Erase Operation */
    __I uint16_t SEESFULL:1;       /*!< bit:      8  Active SEES Full                   */
    __I uint16_t SEESOVF:1;        /*!< bit:      9  Active SEES Overflow               */
    __I uint16_t SEEWRC:1;         /*!< bit:     10  SEE Write Completed                */
    __I uint16_t :5;               /*!< bit: 11..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} NVMCTRL_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define NVMCTRL_INTFLAG_OFFSET      0x10         /**< \brief (NVMCTRL_INTFLAG offset) Interrupt Flag Status and Clear */
#define NVMCTRL_INTFLAG_RESETVALUE  _U_(0x0000)  /**< \brief (NVMCTRL_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define NVMCTRL_INTFLAG_DONE_Pos    0            /**< \brief (NVMCTRL_INTFLAG) Command Done */
#define NVMCTRL_INTFLAG_DONE        (_U_(0x1) << NVMCTRL_INTFLAG_DONE_Pos)
#define NVMCTRL_INTFLAG_ADDRE_Pos   1            /**< \brief (NVMCTRL_INTFLAG) Address Error */
#define NVMCTRL_INTFLAG_ADDRE       (_U_(0x1) << NVMCTRL_INTFLAG_ADDRE_Pos)
#define NVMCTRL_INTFLAG_PROGE_Pos   2            /**< \brief (NVMCTRL_INTFLAG) Programming Error */
#define NVMCTRL_INTFLAG_PROGE       (_U_(0x1) << NVMCTRL_INTFLAG_PROGE_Pos)
#define NVMCTRL_INTFLAG_LOCKE_Pos   3            /**< \brief (NVMCTRL_INTFLAG) Lock Error */
#define NVMCTRL_INTFLAG_LOCKE       (_U_(0x1) << NVMCTRL_INTFLAG_LOCKE_Pos)
#define NVMCTRL_INTFLAG_ECCSE_Pos   4            /**< \brief (NVMCTRL_INTFLAG) ECC Single Error */
#define NVMCTRL_INTFLAG_ECCSE       (_U_(0x1) << NVMCTRL_INTFLAG_ECCSE_Pos)
#define NVMCTRL_INTFLAG_ECCDE_Pos   5            /**< \brief (NVMCTRL_INTFLAG) ECC Dual Error */
#define NVMCTRL_INTFLAG_ECCDE       (_U_(0x1) << NVMCTRL_INTFLAG_ECCDE_Pos)
#define NVMCTRL_INTFLAG_NVME_Pos    6            /**< \brief (NVMCTRL_INTFLAG) NVM Error */
#define NVMCTRL_INTFLAG_NVME        (_U_(0x1) << NVMCTRL_INTFLAG_NVME_Pos)
#define NVMCTRL_INTFLAG_SUSP_Pos    7            /**< \brief (NVMCTRL_INTFLAG) Suspended Write Or Erase Operation */
#define NVMCTRL_INTFLAG_SUSP        (_U_(0x1) << NVMCTRL_INTFLAG_SUSP_Pos)
#define NVMCTRL_INTFLAG_SEESFULL_Pos 8            /**< \brief (NVMCTRL_INTFLAG) Active SEES Full */
#define NVMCTRL_INTFLAG_SEESFULL    (_U_(0x1) << NVMCTRL_INTFLAG_SEESFULL_Pos)
#define NVMCTRL_INTFLAG_SEESOVF_Pos 9            /**< \brief (NVMCTRL_INTFLAG) Active SEES Overflow */
#define NVMCTRL_INTFLAG_SEESOVF     (_U_(0x1) << NVMCTRL_INTFLAG_SEESOVF_Pos)
#define NVMCTRL_INTFLAG_SEEWRC_Pos  10           /**< \brief (NVMCTRL_INTFLAG) SEE Write Completed */
#define NVMCTRL_INTFLAG_SEEWRC      (_U_(0x1) << NVMCTRL_INTFLAG_SEEWRC_Pos)
#define NVMCTRL_INTFLAG_MASK        _U_(0x07FF)  /**< \brief (NVMCTRL_INTFLAG) MASK Register */

/* -------- NVMCTRL_STATUS : (NVMCTRL Offset: 0x12) (R/  16) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t READY:1;          /*!< bit:      0  Ready to accept a command          */
    uint16_t PRM:1;            /*!< bit:      1  Power Reduction Mode               */
    uint16_t LOAD:1;           /*!< bit:      2  NVM Page Buffer Active Loading     */
    uint16_t SUSP:1;           /*!< bit:      3  NVM Write Or Erase Operation Is Suspended */
    uint16_t AFIRST:1;         /*!< bit:      4  BANKA First                        */
    uint16_t BPDIS:1;          /*!< bit:      5  Boot Loader Protection Disable     */
    uint16_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint16_t BOOTPROT:4;       /*!< bit:  8..11  Boot Loader Protection Size        */
    uint16_t :4;               /*!< bit: 12..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} NVMCTRL_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define NVMCTRL_STATUS_OFFSET       0x12         /**< \brief (NVMCTRL_STATUS offset) Status */
#define NVMCTRL_STATUS_RESETVALUE   _U_(0x0000)  /**< \brief (NVMCTRL_STATUS reset_value) Status */

#define NVMCTRL_STATUS_READY_Pos    0            /**< \brief (NVMCTRL_STATUS) Ready to accept a command */
#define NVMCTRL_STATUS_READY        (_U_(0x1) << NVMCTRL_STATUS_READY_Pos)
#define NVMCTRL_STATUS_PRM_Pos      1            /**< \brief (NVMCTRL_STATUS) Power Reduction Mode */
#define NVMCTRL_STATUS_PRM          (_U_(0x1) << NVMCTRL_STATUS_PRM_Pos)
#define NVMCTRL_STATUS_LOAD_Pos     2            /**< \brief (NVMCTRL_STATUS) NVM Page Buffer Active Loading */
#define NVMCTRL_STATUS_LOAD         (_U_(0x1) << NVMCTRL_STATUS_LOAD_Pos)
#define NVMCTRL_STATUS_SUSP_Pos     3            /**< \brief (NVMCTRL_STATUS) NVM Write Or Erase Operation Is Suspended */
#define NVMCTRL_STATUS_SUSP         (_U_(0x1) << NVMCTRL_STATUS_SUSP_Pos)
#define NVMCTRL_STATUS_AFIRST_Pos   4            /**< \brief (NVMCTRL_STATUS) BANKA First */
#define NVMCTRL_STATUS_AFIRST       (_U_(0x1) << NVMCTRL_STATUS_AFIRST_Pos)
#define NVMCTRL_STATUS_BPDIS_Pos    5            /**< \brief (NVMCTRL_STATUS) Boot Loader Protection Disable */
#define NVMCTRL_STATUS_BPDIS        (_U_(0x1) << NVMCTRL_STATUS_BPDIS_Pos)
#define NVMCTRL_STATUS_BOOTPROT_Pos 8            /**< \brief (NVMCTRL_STATUS) Boot Loader Protection Size */
#define NVMCTRL_STATUS_BOOTPROT_Msk (_U_(0xF) << NVMCTRL_STATUS_BOOTPROT_Pos)
#define NVMCTRL_STATUS_BOOTPROT(value) (NVMCTRL_STATUS_BOOTPROT_Msk & ((value) << NVMCTRL_STATUS_BOOTPROT_Pos))
#define NVMCTRL_STATUS_MASK         _U_(0x0F3F)  /**< \brief (NVMCTRL_STATUS) MASK Register */

/* -------- NVMCTRL_ADDR : (NVMCTRL Offset: 0x14) (R/W 32) Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ADDR:24;          /*!< bit:  0..23  NVM Address                        */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} NVMCTRL_ADDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define NVMCTRL_ADDR_OFFSET         0x14         /**< \brief (NVMCTRL_ADDR offset) Address */
#define NVMCTRL_ADDR_RESETVALUE     _U_(0x00000000) /**< \brief (NVMCTRL_ADDR reset_value) Address */

#define NVMCTRL_ADDR_ADDR_Pos       0            /**< \brief (NVMCTRL_ADDR) NVM Address */
#define NVMCTRL_ADDR_ADDR_Msk       (_U_(0xFFFFFF) << NVMCTRL_ADDR_ADDR_Pos)
#define NVMCTRL_ADDR_ADDR(value)    (NVMCTRL_ADDR_ADDR_Msk & ((value) << NVMCTRL_ADDR_ADDR_Pos))
#define NVMCTRL_ADDR_MASK           _U_(0x00FFFFFF) /**< \brief (NVMCTRL_ADDR) MASK Register */

/* -------- NVMCTRL_RUNLOCK : (NVMCTRL Offset: 0x18) (R/  32) Lock Section -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RUNLOCK:32;       /*!< bit:  0..31  Region Un-Lock Bits                */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} NVMCTRL_RUNLOCK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define NVMCTRL_RUNLOCK_OFFSET      0x18         /**< \brief (NVMCTRL_RUNLOCK offset) Lock Section */
#define NVMCTRL_RUNLOCK_RESETVALUE  _U_(0x00000000) /**< \brief (NVMCTRL_RUNLOCK reset_value) Lock Section */

#define NVMCTRL_RUNLOCK_RUNLOCK_Pos 0            /**< \brief (NVMCTRL_RUNLOCK) Region Un-Lock Bits */
#define NVMCTRL_RUNLOCK_RUNLOCK_Msk (_U_(0xFFFFFFFF) << NVMCTRL_RUNLOCK_RUNLOCK_Pos)
#define NVMCTRL_RUNLOCK_RUNLOCK(value) (NVMCTRL_RUNLOCK_RUNLOCK_Msk & ((value) << NVMCTRL_RUNLOCK_RUNLOCK_Pos))
#define NVMCTRL_RUNLOCK_MASK        _U_(0xFFFFFFFF) /**< \brief (NVMCTRL_RUNLOCK) MASK Register */

/* -------- NVMCTRL_PBLDATA : (NVMCTRL Offset: 0x1C) (R/  32) Page Buffer Load Data x -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DATA:32;          /*!< bit:  0..31  Page Buffer Data                   */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} NVMCTRL_PBLDATA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define NVMCTRL_PBLDATA_OFFSET      0x1C         /**< \brief (NVMCTRL_PBLDATA offset) Page Buffer Load Data x */
#define NVMCTRL_PBLDATA_RESETVALUE  _U_(0xFFFFFFFF) /**< \brief (NVMCTRL_PBLDATA reset_value) Page Buffer Load Data x */

#define NVMCTRL_PBLDATA_DATA_Pos    0            /**< \brief (NVMCTRL_PBLDATA) Page Buffer Data */
#define NVMCTRL_PBLDATA_DATA_Msk    (_U_(0xFFFFFFFF) << NVMCTRL_PBLDATA_DATA_Pos)
#define NVMCTRL_PBLDATA_DATA(value) (NVMCTRL_PBLDATA_DATA_Msk & ((value) << NVMCTRL_PBLDATA_DATA_Pos))
#define NVMCTRL_PBLDATA_MASK        _U_(0xFFFFFFFF) /**< \brief (NVMCTRL_PBLDATA) MASK Register */

/* -------- NVMCTRL_ECCERR : (NVMCTRL Offset: 0x24) (R/  32) ECC Error Status Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ADDR:24;          /*!< bit:  0..23  Error Address                      */
    uint32_t :4;               /*!< bit: 24..27  Reserved                           */
    uint32_t TYPEL:2;          /*!< bit: 28..29  Low Double-Word Error Type         */
    uint32_t TYPEH:2;          /*!< bit: 30..31  High Double-Word Error Type        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} NVMCTRL_ECCERR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define NVMCTRL_ECCERR_OFFSET       0x24         /**< \brief (NVMCTRL_ECCERR offset) ECC Error Status Register */
#define NVMCTRL_ECCERR_RESETVALUE   _U_(0x00000000) /**< \brief (NVMCTRL_ECCERR reset_value) ECC Error Status Register */

#define NVMCTRL_ECCERR_ADDR_Pos     0            /**< \brief (NVMCTRL_ECCERR) Error Address */
#define NVMCTRL_ECCERR_ADDR_Msk     (_U_(0xFFFFFF) << NVMCTRL_ECCERR_ADDR_Pos)
#define NVMCTRL_ECCERR_ADDR(value)  (NVMCTRL_ECCERR_ADDR_Msk & ((value) << NVMCTRL_ECCERR_ADDR_Pos))
#define NVMCTRL_ECCERR_TYPEL_Pos    28           /**< \brief (NVMCTRL_ECCERR) Low Double-Word Error Type */
#define NVMCTRL_ECCERR_TYPEL_Msk    (_U_(0x3) << NVMCTRL_ECCERR_TYPEL_Pos)
#define NVMCTRL_ECCERR_TYPEL(value) (NVMCTRL_ECCERR_TYPEL_Msk & ((value) << NVMCTRL_ECCERR_TYPEL_Pos))
#define   NVMCTRL_ECCERR_TYPEL_NONE_Val   _U_(0x0)   /**< \brief (NVMCTRL_ECCERR) No Error Detected Since Last Read */
#define   NVMCTRL_ECCERR_TYPEL_SINGLE_Val _U_(0x1)   /**< \brief (NVMCTRL_ECCERR) At Least One Single Error Detected Since last Read */
#define   NVMCTRL_ECCERR_TYPEL_DUAL_Val   _U_(0x2)   /**< \brief (NVMCTRL_ECCERR) At Least One Dual Error Detected Since Last Read */
#define NVMCTRL_ECCERR_TYPEL_NONE   (NVMCTRL_ECCERR_TYPEL_NONE_Val << NVMCTRL_ECCERR_TYPEL_Pos)
#define NVMCTRL_ECCERR_TYPEL_SINGLE (NVMCTRL_ECCERR_TYPEL_SINGLE_Val << NVMCTRL_ECCERR_TYPEL_Pos)
#define NVMCTRL_ECCERR_TYPEL_DUAL   (NVMCTRL_ECCERR_TYPEL_DUAL_Val << NVMCTRL_ECCERR_TYPEL_Pos)
#define NVMCTRL_ECCERR_TYPEH_Pos    30           /**< \brief (NVMCTRL_ECCERR) High Double-Word Error Type */
#define NVMCTRL_ECCERR_TYPEH_Msk    (_U_(0x3) << NVMCTRL_ECCERR_TYPEH_Pos)
#define NVMCTRL_ECCERR_TYPEH(value) (NVMCTRL_ECCERR_TYPEH_Msk & ((value) << NVMCTRL_ECCERR_TYPEH_Pos))
#define   NVMCTRL_ECCERR_TYPEH_NONE_Val   _U_(0x0)   /**< \brief (NVMCTRL_ECCERR) No Error Detected Since Last Read */
#define   NVMCTRL_ECCERR_TYPEH_SINGLE_Val _U_(0x1)   /**< \brief (NVMCTRL_ECCERR) At Least One Single Error Detected Since last Read */
#define   NVMCTRL_ECCERR_TYPEH_DUAL_Val   _U_(0x2)   /**< \brief (NVMCTRL_ECCERR) At Least One Dual Error Detected Since Last Read */
#define NVMCTRL_ECCERR_TYPEH_NONE   (NVMCTRL_ECCERR_TYPEH_NONE_Val << NVMCTRL_ECCERR_TYPEH_Pos)
#define NVMCTRL_ECCERR_TYPEH_SINGLE (NVMCTRL_ECCERR_TYPEH_SINGLE_Val << NVMCTRL_ECCERR_TYPEH_Pos)
#define NVMCTRL_ECCERR_TYPEH_DUAL   (NVMCTRL_ECCERR_TYPEH_DUAL_Val << NVMCTRL_ECCERR_TYPEH_Pos)
#define NVMCTRL_ECCERR_MASK         _U_(0xF0FFFFFF) /**< \brief (NVMCTRL_ECCERR) MASK Register */

/* -------- NVMCTRL_DBGCTRL : (NVMCTRL Offset: 0x28) (R/W  8) Debug Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  ECCDIS:1;         /*!< bit:      0  Debugger ECC Read Disable          */
    uint8_t  ECCELOG:1;        /*!< bit:      1  Debugger ECC Error Tracking Mode   */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} NVMCTRL_DBGCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define NVMCTRL_DBGCTRL_OFFSET      0x28         /**< \brief (NVMCTRL_DBGCTRL offset) Debug Control */
#define NVMCTRL_DBGCTRL_RESETVALUE  _U_(0x00)    /**< \brief (NVMCTRL_DBGCTRL reset_value) Debug Control */

#define NVMCTRL_DBGCTRL_ECCDIS_Pos  0            /**< \brief (NVMCTRL_DBGCTRL) Debugger ECC Read Disable */
#define NVMCTRL_DBGCTRL_ECCDIS      (_U_(0x1) << NVMCTRL_DBGCTRL_ECCDIS_Pos)
#define NVMCTRL_DBGCTRL_ECCELOG_Pos 1            /**< \brief (NVMCTRL_DBGCTRL) Debugger ECC Error Tracking Mode */
#define NVMCTRL_DBGCTRL_ECCELOG     (_U_(0x1) << NVMCTRL_DBGCTRL_ECCELOG_Pos)
#define NVMCTRL_DBGCTRL_MASK        _U_(0x03)    /**< \brief (NVMCTRL_DBGCTRL) MASK Register */

/* -------- NVMCTRL_SEECFG : (NVMCTRL Offset: 0x2A) (R/W  8) SmartEEPROM Configuration Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  WMODE:1;          /*!< bit:      0  Write Mode                         */
    uint8_t  APRDIS:1;         /*!< bit:      1  Automatic Page Reallocation Disable */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} NVMCTRL_SEECFG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define NVMCTRL_SEECFG_OFFSET       0x2A         /**< \brief (NVMCTRL_SEECFG offset) SmartEEPROM Configuration Register */
#define NVMCTRL_SEECFG_RESETVALUE   _U_(0x00)    /**< \brief (NVMCTRL_SEECFG reset_value) SmartEEPROM Configuration Register */

#define NVMCTRL_SEECFG_WMODE_Pos    0            /**< \brief (NVMCTRL_SEECFG) Write Mode */
#define NVMCTRL_SEECFG_WMODE        (_U_(0x1) << NVMCTRL_SEECFG_WMODE_Pos)
#define   NVMCTRL_SEECFG_WMODE_UNBUFFERED_Val _U_(0x0)   /**< \brief (NVMCTRL_SEECFG) A NVM write command is issued after each write in the pagebuffer */
#define   NVMCTRL_SEECFG_WMODE_BUFFERED_Val _U_(0x1)   /**< \brief (NVMCTRL_SEECFG) A NVM write command is issued when a write to a new page is requested */
#define NVMCTRL_SEECFG_WMODE_UNBUFFERED (NVMCTRL_SEECFG_WMODE_UNBUFFERED_Val << NVMCTRL_SEECFG_WMODE_Pos)
#define NVMCTRL_SEECFG_WMODE_BUFFERED (NVMCTRL_SEECFG_WMODE_BUFFERED_Val << NVMCTRL_SEECFG_WMODE_Pos)
#define NVMCTRL_SEECFG_APRDIS_Pos   1            /**< \brief (NVMCTRL_SEECFG) Automatic Page Reallocation Disable */
#define NVMCTRL_SEECFG_APRDIS       (_U_(0x1) << NVMCTRL_SEECFG_APRDIS_Pos)
#define NVMCTRL_SEECFG_MASK         _U_(0x03)    /**< \brief (NVMCTRL_SEECFG) MASK Register */

/* -------- NVMCTRL_SEESTAT : (NVMCTRL Offset: 0x2C) (R/  32) SmartEEPROM Status Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ASEES:1;          /*!< bit:      0  Active SmartEEPROM Sector          */
    uint32_t LOAD:1;           /*!< bit:      1  Page Buffer Loaded                 */
    uint32_t BUSY:1;           /*!< bit:      2  Busy                               */
    uint32_t LOCK:1;           /*!< bit:      3  SmartEEPROM Write Access Is Locked */
    uint32_t RLOCK:1;          /*!< bit:      4  SmartEEPROM Write Access To Register Address Space Is Locked */
    uint32_t :3;               /*!< bit:  5.. 7  Reserved                           */
    uint32_t SBLK:4;           /*!< bit:  8..11  Blocks Number In a Sector          */
    uint32_t :4;               /*!< bit: 12..15  Reserved                           */
    uint32_t PSZ:3;            /*!< bit: 16..18  SmartEEPROM Page Size              */
    uint32_t :13;              /*!< bit: 19..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} NVMCTRL_SEESTAT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define NVMCTRL_SEESTAT_OFFSET      0x2C         /**< \brief (NVMCTRL_SEESTAT offset) SmartEEPROM Status Register */
#define NVMCTRL_SEESTAT_RESETVALUE  _U_(0x00000000) /**< \brief (NVMCTRL_SEESTAT reset_value) SmartEEPROM Status Register */

#define NVMCTRL_SEESTAT_ASEES_Pos   0            /**< \brief (NVMCTRL_SEESTAT) Active SmartEEPROM Sector */
#define NVMCTRL_SEESTAT_ASEES       (_U_(0x1) << NVMCTRL_SEESTAT_ASEES_Pos)
#define NVMCTRL_SEESTAT_LOAD_Pos    1            /**< \brief (NVMCTRL_SEESTAT) Page Buffer Loaded */
#define NVMCTRL_SEESTAT_LOAD        (_U_(0x1) << NVMCTRL_SEESTAT_LOAD_Pos)
#define NVMCTRL_SEESTAT_BUSY_Pos    2            /**< \brief (NVMCTRL_SEESTAT) Busy */
#define NVMCTRL_SEESTAT_BUSY        (_U_(0x1) << NVMCTRL_SEESTAT_BUSY_Pos)
#define NVMCTRL_SEESTAT_LOCK_Pos    3            /**< \brief (NVMCTRL_SEESTAT) SmartEEPROM Write Access Is Locked */
#define NVMCTRL_SEESTAT_LOCK        (_U_(0x1) << NVMCTRL_SEESTAT_LOCK_Pos)
#define NVMCTRL_SEESTAT_RLOCK_Pos   4            /**< \brief (NVMCTRL_SEESTAT) SmartEEPROM Write Access To Register Address Space Is Locked */
#define NVMCTRL_SEESTAT_RLOCK       (_U_(0x1) << NVMCTRL_SEESTAT_RLOCK_Pos)
#define NVMCTRL_SEESTAT_SBLK_Pos    8            /**< \brief (NVMCTRL_SEESTAT) Blocks Number In a Sector */
#define NVMCTRL_SEESTAT_SBLK_Msk    (_U_(0xF) << NVMCTRL_SEESTAT_SBLK_Pos)
#define NVMCTRL_SEESTAT_SBLK(value) (NVMCTRL_SEESTAT_SBLK_Msk & ((value) << NVMCTRL_SEESTAT_SBLK_Pos))
#define NVMCTRL_SEESTAT_PSZ_Pos     16           /**< \brief (NVMCTRL_SEESTAT) SmartEEPROM Page Size */
#define NVMCTRL_SEESTAT_PSZ_Msk     (_U_(0x7) << NVMCTRL_SEESTAT_PSZ_Pos)
#define NVMCTRL_SEESTAT_PSZ(value)  (NVMCTRL_SEESTAT_PSZ_Msk & ((value) << NVMCTRL_SEESTAT_PSZ_Pos))
#define NVMCTRL_SEESTAT_MASK        _U_(0x00070F1F) /**< \brief (NVMCTRL_SEESTAT) MASK Register */

/** \brief NVMCTRL APB hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO NVMCTRL_CTRLA_Type        CTRLA;       /**< \brief Offset: 0x00 (R/W 16) Control A */
       RoReg8                    Reserved1[0x2];
  __O  NVMCTRL_CTRLB_Type        CTRLB;       /**< \brief Offset: 0x04 ( /W 16) Control B */
       RoReg8                    Reserved2[0x2];
  __I  NVMCTRL_PARAM_Type        PARAM;       /**< \brief Offset: 0x08 (R/  32) NVM Parameter */
  __IO NVMCTRL_INTENCLR_Type     INTENCLR;    /**< \brief Offset: 0x0C (R/W 16) Interrupt Enable Clear */
  __IO NVMCTRL_INTENSET_Type     INTENSET;    /**< \brief Offset: 0x0E (R/W 16) Interrupt Enable Set */
  __IO NVMCTRL_INTFLAG_Type      INTFLAG;     /**< \brief Offset: 0x10 (R/W 16) Interrupt Flag Status and Clear */
  __I  NVMCTRL_STATUS_Type       STATUS;      /**< \brief Offset: 0x12 (R/  16) Status */
  __IO NVMCTRL_ADDR_Type         ADDR;        /**< \brief Offset: 0x14 (R/W 32) Address */
  __I  NVMCTRL_RUNLOCK_Type      RUNLOCK;     /**< \brief Offset: 0x18 (R/  32) Lock Section */
  __I  NVMCTRL_PBLDATA_Type      PBLDATA[2];  /**< \brief Offset: 0x1C (R/  32) Page Buffer Load Data x */
  __I  NVMCTRL_ECCERR_Type       ECCERR;      /**< \brief Offset: 0x24 (R/  32) ECC Error Status Register */
  __IO NVMCTRL_DBGCTRL_Type      DBGCTRL;     /**< \brief Offset: 0x28 (R/W  8) Debug Control */
       RoReg8                    Reserved3[0x1];
  __IO NVMCTRL_SEECFG_Type       SEECFG;      /**< \brief Offset: 0x2A (R/W  8) SmartEEPROM Configuration Register */
       RoReg8                    Reserved4[0x1];
  __I  NVMCTRL_SEESTAT_Type      SEESTAT;     /**< \brief Offset: 0x2C (R/  32) SmartEEPROM Status Register */
} Nvmctrl;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SECTION_NVMCTRL_CB

#define SECTION_NVMCTRL_CBW0

#define SECTION_NVMCTRL_CBW1

#define SECTION_NVMCTRL_CBW2

#define SECTION_NVMCTRL_CBW3

#define SECTION_NVMCTRL_CBW4

#define SECTION_NVMCTRL_CBW5

#define SECTION_NVMCTRL_CBW6

#define SECTION_NVMCTRL_CBW7

#define SECTION_NVMCTRL_FS

#define SECTION_NVMCTRL_GB

#define SECTION_NVMCTRL_SW0

#define SECTION_NVMCTRL_SW1

#define SECTION_NVMCTRL_SW2

#define SECTION_NVMCTRL_SW3

#define SECTION_NVMCTRL_SW4

#define SECTION_NVMCTRL_SW5

#define SECTION_NVMCTRL_SW6

#define SECTION_NVMCTRL_SW7

#define SECTION_NVMCTRL_TEMP_LOG

#define SECTION_NVMCTRL_TEMP_LOG_W0

#define SECTION_NVMCTRL_TEMP_LOG_W1

#define SECTION_NVMCTRL_TEMP_LOG_W2

#define SECTION_NVMCTRL_TEMP_LOG_W3

#define SECTION_NVMCTRL_TEMP_LOG_W4

#define SECTION_NVMCTRL_TEMP_LOG_W5

#define SECTION_NVMCTRL_TEMP_LOG_W6

#define SECTION_NVMCTRL_TEMP_LOG_W7

#define SECTION_NVMCTRL_TLATCH

#define SECTION_NVMCTRL_USER

/*@}*/

/* ************************************************************************** */
/**  SOFTWARE PERIPHERAL API DEFINITION FOR NON-VOLATILE FUSES */
/* ************************************************************************** */
/** \addtogroup fuses_api Peripheral Software API */
/*@{*/


#define AC_FUSES_BIAS0_ADDR         NVMCTRL_SW0
#define AC_FUSES_BIAS0_Pos          0            /**< \brief (NVMCTRL_SW0) PAIR0 Bias Calibration */
#define AC_FUSES_BIAS0_Msk          (_U_(0x3) << AC_FUSES_BIAS0_Pos)
#define AC_FUSES_BIAS0(value)       (AC_FUSES_BIAS0_Msk & ((value) << AC_FUSES_BIAS0_Pos))

#define ADC0_FUSES_BIASCOMP_ADDR    NVMCTRL_SW0
#define ADC0_FUSES_BIASCOMP_Pos     2            /**< \brief (NVMCTRL_SW0) ADC Comparator Scaling */
#define ADC0_FUSES_BIASCOMP_Msk     (_U_(0x7) << ADC0_FUSES_BIASCOMP_Pos)
#define ADC0_FUSES_BIASCOMP(value)  (ADC0_FUSES_BIASCOMP_Msk & ((value) << ADC0_FUSES_BIASCOMP_Pos))

#define ADC0_FUSES_BIASR2R_ADDR     NVMCTRL_SW0
#define ADC0_FUSES_BIASR2R_Pos      8            /**< \brief (NVMCTRL_SW0) ADC Bias R2R ampli scaling */
#define ADC0_FUSES_BIASR2R_Msk      (_U_(0x7) << ADC0_FUSES_BIASR2R_Pos)
#define ADC0_FUSES_BIASR2R(value)   (ADC0_FUSES_BIASR2R_Msk & ((value) << ADC0_FUSES_BIASR2R_Pos))

#define ADC0_FUSES_BIASREFBUF_ADDR  NVMCTRL_SW0
#define ADC0_FUSES_BIASREFBUF_Pos   5            /**< \brief (NVMCTRL_SW0) ADC Bias Reference Buffer Scaling */
#define ADC0_FUSES_BIASREFBUF_Msk   (_U_(0x7) << ADC0_FUSES_BIASREFBUF_Pos)
#define ADC0_FUSES_BIASREFBUF(value) (ADC0_FUSES_BIASREFBUF_Msk & ((value) << ADC0_FUSES_BIASREFBUF_Pos))

#define ADC1_FUSES_BIASCOMP_ADDR    NVMCTRL_SW0
#define ADC1_FUSES_BIASCOMP_Pos     16           /**< \brief (NVMCTRL_SW0) ADC Comparator Scaling */
#define ADC1_FUSES_BIASCOMP_Msk     (_U_(0x7) << ADC1_FUSES_BIASCOMP_Pos)
#define ADC1_FUSES_BIASCOMP(value)  (ADC1_FUSES_BIASCOMP_Msk & ((value) << ADC1_FUSES_BIASCOMP_Pos))

#define ADC1_FUSES_BIASR2R_ADDR     NVMCTRL_SW0
#define ADC1_FUSES_BIASR2R_Pos      22           /**< \brief (NVMCTRL_SW0) ADC Bias R2R ampli scaling */
#define ADC1_FUSES_BIASR2R_Msk      (_U_(0x7) << ADC1_FUSES_BIASR2R_Pos)
#define ADC1_FUSES_BIASR2R(value)   (ADC1_FUSES_BIASR2R_Msk & ((value) << ADC1_FUSES_BIASR2R_Pos))

#define ADC1_FUSES_BIASREFBUF_ADDR  NVMCTRL_SW0
#define ADC1_FUSES_BIASREFBUF_Pos   19           /**< \brief (NVMCTRL_SW0) ADC Bias Reference Buffer Scaling */
#define ADC1_FUSES_BIASREFBUF_Msk   (_U_(0x7) << ADC1_FUSES_BIASREFBUF_Pos)
#define ADC1_FUSES_BIASREFBUF(value) (ADC1_FUSES_BIASREFBUF_Msk & ((value) << ADC1_FUSES_BIASREFBUF_Pos))

#define FUSES_BOD12USERLEVEL_ADDR   NVMCTRL_USER
#define FUSES_BOD12USERLEVEL_Pos    17           /**< \brief (NVMCTRL_USER) BOD12 User Level */
#define FUSES_BOD12USERLEVEL_Msk    (_U_(0x3F) << FUSES_BOD12USERLEVEL_Pos)
#define FUSES_BOD12USERLEVEL(value) (FUSES_BOD12USERLEVEL_Msk & ((value) << FUSES_BOD12USERLEVEL_Pos))

#define FUSES_BOD12_ACTION_ADDR     NVMCTRL_USER
#define FUSES_BOD12_ACTION_Pos      23           /**< \brief (NVMCTRL_USER) BOD12 Action */
#define FUSES_BOD12_ACTION_Msk      (_U_(0x3) << FUSES_BOD12_ACTION_Pos)
#define FUSES_BOD12_ACTION(value)   (FUSES_BOD12_ACTION_Msk & ((value) << FUSES_BOD12_ACTION_Pos))

#define FUSES_BOD12_DIS_ADDR        NVMCTRL_USER
#define FUSES_BOD12_DIS_Pos         16           /**< \brief (NVMCTRL_USER) BOD12 Disable */
#define FUSES_BOD12_DIS_Msk         (_U_(0x1) << FUSES_BOD12_DIS_Pos)

#define FUSES_BOD12_HYST_ADDR       NVMCTRL_USER
#define FUSES_BOD12_HYST_Pos        25           /**< \brief (NVMCTRL_USER) BOD12 Hysteresis */
#define FUSES_BOD12_HYST_Msk        (_U_(0x1) << FUSES_BOD12_HYST_Pos)

#define FUSES_BOD33USERLEVEL_ADDR   NVMCTRL_USER
#define FUSES_BOD33USERLEVEL_Pos    1            /**< \brief (NVMCTRL_USER) BOD33 User Level */
#define FUSES_BOD33USERLEVEL_Msk    (_U_(0xFF) << FUSES_BOD33USERLEVEL_Pos)
#define FUSES_BOD33USERLEVEL(value) (FUSES_BOD33USERLEVEL_Msk & ((value) << FUSES_BOD33USERLEVEL_Pos))

#define FUSES_BOD33_ACTION_ADDR     NVMCTRL_USER
#define FUSES_BOD33_ACTION_Pos      9            /**< \brief (NVMCTRL_USER) BOD33 Action */
#define FUSES_BOD33_ACTION_Msk      (_U_(0x3) << FUSES_BOD33_ACTION_Pos)
#define FUSES_BOD33_ACTION(value)   (FUSES_BOD33_ACTION_Msk & ((value) << FUSES_BOD33_ACTION_Pos))

#define FUSES_BOD33_DIS_ADDR        NVMCTRL_USER
#define FUSES_BOD33_DIS_Pos         0            /**< \brief (NVMCTRL_USER) BOD33 Disable */
#define FUSES_BOD33_DIS_Msk         (_U_(0x1) << FUSES_BOD33_DIS_Pos)

#define FUSES_BOD33_HYST_ADDR       NVMCTRL_USER
#define FUSES_BOD33_HYST_Pos        11           /**< \brief (NVMCTRL_USER) BOD33 Hysteresis */
#define FUSES_BOD33_HYST_Msk        (_U_(0xF) << FUSES_BOD33_HYST_Pos)
#define FUSES_BOD33_HYST(value)     (FUSES_BOD33_HYST_Msk & ((value) << FUSES_BOD33_HYST_Pos))

#define FUSES_HOT_ADC_VAL_CTAT_ADDR (NVMCTRL_TEMP_LOG + 8)
#define FUSES_HOT_ADC_VAL_CTAT_Pos  12           /**< \brief (NVMCTRL_TEMP_LOG) 12-bit ADC conversion at hot temperature CTAT */
#define FUSES_HOT_ADC_VAL_CTAT_Msk  (_U_(0xFFF) << FUSES_HOT_ADC_VAL_CTAT_Pos)
#define FUSES_HOT_ADC_VAL_CTAT(value) (FUSES_HOT_ADC_VAL_CTAT_Msk & ((value) << FUSES_HOT_ADC_VAL_CTAT_Pos))

#define FUSES_HOT_ADC_VAL_PTAT_ADDR (NVMCTRL_TEMP_LOG + 4)
#define FUSES_HOT_ADC_VAL_PTAT_Pos  20           /**< \brief (NVMCTRL_TEMP_LOG) 12-bit ADC conversion at hot temperature PTAT */
#define FUSES_HOT_ADC_VAL_PTAT_Msk  (_U_(0xFFF) << FUSES_HOT_ADC_VAL_PTAT_Pos)
#define FUSES_HOT_ADC_VAL_PTAT(value) (FUSES_HOT_ADC_VAL_PTAT_Msk & ((value) << FUSES_HOT_ADC_VAL_PTAT_Pos))

#define FUSES_HOT_INT1V_VAL_ADDR    (NVMCTRL_TEMP_LOG + 4)
#define FUSES_HOT_INT1V_VAL_Pos     0            /**< \brief (NVMCTRL_TEMP_LOG) 2's complement of the internal 1V reference drift at hot temperature (versus a 1.0 centered value) */
#define FUSES_HOT_INT1V_VAL_Msk     (_U_(0xFF) << FUSES_HOT_INT1V_VAL_Pos)
#define FUSES_HOT_INT1V_VAL(value)  (FUSES_HOT_INT1V_VAL_Msk & ((value) << FUSES_HOT_INT1V_VAL_Pos))

#define FUSES_HOT_TEMP_VAL_DEC_ADDR NVMCTRL_TEMP_LOG
#define FUSES_HOT_TEMP_VAL_DEC_Pos  20           /**< \brief (NVMCTRL_TEMP_LOG) Decimal part of hot temperature */
#define FUSES_HOT_TEMP_VAL_DEC_Msk  (_U_(0xF) << FUSES_HOT_TEMP_VAL_DEC_Pos)
#define FUSES_HOT_TEMP_VAL_DEC(value) (FUSES_HOT_TEMP_VAL_DEC_Msk & ((value) << FUSES_HOT_TEMP_VAL_DEC_Pos))

#define FUSES_HOT_TEMP_VAL_INT_ADDR NVMCTRL_TEMP_LOG
#define FUSES_HOT_TEMP_VAL_INT_Pos  12           /**< \brief (NVMCTRL_TEMP_LOG) Integer part of hot temperature in oC */
#define FUSES_HOT_TEMP_VAL_INT_Msk  (_U_(0xFF) << FUSES_HOT_TEMP_VAL_INT_Pos)
#define FUSES_HOT_TEMP_VAL_INT(value) (FUSES_HOT_TEMP_VAL_INT_Msk & ((value) << FUSES_HOT_TEMP_VAL_INT_Pos))

#define FUSES_ROOM_ADC_VAL_CTAT_ADDR (NVMCTRL_TEMP_LOG + 8)
#define FUSES_ROOM_ADC_VAL_CTAT_Pos 0            /**< \brief (NVMCTRL_TEMP_LOG) 12-bit ADC conversion at room temperature CTAT */
#define FUSES_ROOM_ADC_VAL_CTAT_Msk (_U_(0xFFF) << FUSES_ROOM_ADC_VAL_CTAT_Pos)
#define FUSES_ROOM_ADC_VAL_CTAT(value) (FUSES_ROOM_ADC_VAL_CTAT_Msk & ((value) << FUSES_ROOM_ADC_VAL_CTAT_Pos))

#define FUSES_ROOM_ADC_VAL_PTAT_ADDR (NVMCTRL_TEMP_LOG + 4)
#define FUSES_ROOM_ADC_VAL_PTAT_Pos 8            /**< \brief (NVMCTRL_TEMP_LOG) 12-bit ADC conversion at room temperature PTAT */
#define FUSES_ROOM_ADC_VAL_PTAT_Msk (_U_(0xFFF) << FUSES_ROOM_ADC_VAL_PTAT_Pos)
#define FUSES_ROOM_ADC_VAL_PTAT(value) (FUSES_ROOM_ADC_VAL_PTAT_Msk & ((value) << FUSES_ROOM_ADC_VAL_PTAT_Pos))

#define FUSES_ROOM_INT1V_VAL_ADDR   NVMCTRL_TEMP_LOG
#define FUSES_ROOM_INT1V_VAL_Pos    24           /**< \brief (NVMCTRL_TEMP_LOG) 2's complement of the internal 1V reference drift at room temperature (versus a 1.0 centered value) */
#define FUSES_ROOM_INT1V_VAL_Msk    (_U_(0xFF) << FUSES_ROOM_INT1V_VAL_Pos)
#define FUSES_ROOM_INT1V_VAL(value) (FUSES_ROOM_INT1V_VAL_Msk & ((value) << FUSES_ROOM_INT1V_VAL_Pos))

#define FUSES_ROOM_TEMP_VAL_DEC_ADDR NVMCTRL_TEMP_LOG
#define FUSES_ROOM_TEMP_VAL_DEC_Pos 8            /**< \brief (NVMCTRL_TEMP_LOG) Decimal part of room temperature */
#define FUSES_ROOM_TEMP_VAL_DEC_Msk (_U_(0xF) << FUSES_ROOM_TEMP_VAL_DEC_Pos)
#define FUSES_ROOM_TEMP_VAL_DEC(value) (FUSES_ROOM_TEMP_VAL_DEC_Msk & ((value) << FUSES_ROOM_TEMP_VAL_DEC_Pos))

#define FUSES_ROOM_TEMP_VAL_INT_ADDR NVMCTRL_TEMP_LOG
#define FUSES_ROOM_TEMP_VAL_INT_Pos 0            /**< \brief (NVMCTRL_TEMP_LOG) Integer part of room temperature in oC */
#define FUSES_ROOM_TEMP_VAL_INT_Msk (_U_(0xFF) << FUSES_ROOM_TEMP_VAL_INT_Pos)
#define FUSES_ROOM_TEMP_VAL_INT(value) (FUSES_ROOM_TEMP_VAL_INT_Msk & ((value) << FUSES_ROOM_TEMP_VAL_INT_Pos))

#define NVMCTRL_FUSES_BOOTPROT_ADDR NVMCTRL_USER
#define NVMCTRL_FUSES_BOOTPROT_Pos  26           /**< \brief (NVMCTRL_USER) Bootloader Size */
#define NVMCTRL_FUSES_BOOTPROT_Msk  (_U_(0xF) << NVMCTRL_FUSES_BOOTPROT_Pos)
#define NVMCTRL_FUSES_BOOTPROT(value) (NVMCTRL_FUSES_BOOTPROT_Msk & ((value) << NVMCTRL_FUSES_BOOTPROT_Pos))

#define NVMCTRL_FUSES_REGION_LOCKS_ADDR (NVMCTRL_USER + 8)
#define NVMCTRL_FUSES_REGION_LOCKS_Pos 0            /**< \brief (NVMCTRL_USER) NVM Region Locks */
#define NVMCTRL_FUSES_REGION_LOCKS_Msk (_U_(0xFFFFFFFF) << NVMCTRL_FUSES_REGION_LOCKS_Pos)
#define NVMCTRL_FUSES_REGION_LOCKS(value) (NVMCTRL_FUSES_REGION_LOCKS_Msk & ((value) << NVMCTRL_FUSES_REGION_LOCKS_Pos))

#define NVMCTRL_FUSES_SEEPSZ_ADDR   (NVMCTRL_USER + 4)
#define NVMCTRL_FUSES_SEEPSZ_Pos    4            /**< \brief (NVMCTRL_USER) Size Of SmartEEPROM Page */
#define NVMCTRL_FUSES_SEEPSZ_Msk    (_U_(0x7) << NVMCTRL_FUSES_SEEPSZ_Pos)
#define NVMCTRL_FUSES_SEEPSZ(value) (NVMCTRL_FUSES_SEEPSZ_Msk & ((value) << NVMCTRL_FUSES_SEEPSZ_Pos))

#define NVMCTRL_FUSES_SEESBLK_ADDR  (NVMCTRL_USER + 4)
#define NVMCTRL_FUSES_SEESBLK_Pos   0            /**< \brief (NVMCTRL_USER) Number Of Physical NVM Blocks Composing a SmartEEPROM Sector */
#define NVMCTRL_FUSES_SEESBLK_Msk   (_U_(0xF) << NVMCTRL_FUSES_SEESBLK_Pos)
#define NVMCTRL_FUSES_SEESBLK(value) (NVMCTRL_FUSES_SEESBLK_Msk & ((value) << NVMCTRL_FUSES_SEESBLK_Pos))

#define RAMECC_FUSES_ECCDIS_ADDR    (NVMCTRL_USER + 4)
#define RAMECC_FUSES_ECCDIS_Pos     7            /**< \brief (NVMCTRL_USER) RAM ECC Disable fuse */
#define RAMECC_FUSES_ECCDIS_Msk     (_U_(0x1) << RAMECC_FUSES_ECCDIS_Pos)

#define USB_FUSES_TRANSN_ADDR       (NVMCTRL_SW0 + 4)
#define USB_FUSES_TRANSN_Pos        0            /**< \brief (NVMCTRL_SW0) USB pad Transn calibration */
#define USB_FUSES_TRANSN_Msk        (_U_(0x1F) << USB_FUSES_TRANSN_Pos)
#define USB_FUSES_TRANSN(value)     (USB_FUSES_TRANSN_Msk & ((value) << USB_FUSES_TRANSN_Pos))

#define USB_FUSES_TRANSP_ADDR       (NVMCTRL_SW0 + 4)
#define USB_FUSES_TRANSP_Pos        5            /**< \brief (NVMCTRL_SW0) USB pad Transp calibration */
#define USB_FUSES_TRANSP_Msk        (_U_(0x1F) << USB_FUSES_TRANSP_Pos)
#define USB_FUSES_TRANSP(value)     (USB_FUSES_TRANSP_Msk & ((value) << USB_FUSES_TRANSP_Pos))

#define USB_FUSES_TRIM_ADDR         (NVMCTRL_SW0 + 4)
#define USB_FUSES_TRIM_Pos          10           /**< \brief (NVMCTRL_SW0) USB pad Trim calibration */
#define USB_FUSES_TRIM_Msk          (_U_(0x7) << USB_FUSES_TRIM_Pos)
#define USB_FUSES_TRIM(value)       (USB_FUSES_TRIM_Msk & ((value) << USB_FUSES_TRIM_Pos))

#define WDT_FUSES_ALWAYSON_ADDR     (NVMCTRL_USER + 4)
#define WDT_FUSES_ALWAYSON_Pos      17           /**< \brief (NVMCTRL_USER) WDT Always On */
#define WDT_FUSES_ALWAYSON_Msk      (_U_(0x1) << WDT_FUSES_ALWAYSON_Pos)

#define WDT_FUSES_ENABLE_ADDR       (NVMCTRL_USER + 4)
#define WDT_FUSES_ENABLE_Pos        16           /**< \brief (NVMCTRL_USER) WDT Enable */
#define WDT_FUSES_ENABLE_Msk        (_U_(0x1) << WDT_FUSES_ENABLE_Pos)

#define WDT_FUSES_EWOFFSET_ADDR     (NVMCTRL_USER + 4)
#define WDT_FUSES_EWOFFSET_Pos      26           /**< \brief (NVMCTRL_USER) WDT Early Warning Offset */
#define WDT_FUSES_EWOFFSET_Msk      (_U_(0xF) << WDT_FUSES_EWOFFSET_Pos)
#define WDT_FUSES_EWOFFSET(value)   (WDT_FUSES_EWOFFSET_Msk & ((value) << WDT_FUSES_EWOFFSET_Pos))

#define WDT_FUSES_PER_ADDR          (NVMCTRL_USER + 4)
#define WDT_FUSES_PER_Pos           18           /**< \brief (NVMCTRL_USER) WDT Period */
#define WDT_FUSES_PER_Msk           (_U_(0xF) << WDT_FUSES_PER_Pos)
#define WDT_FUSES_PER(value)        (WDT_FUSES_PER_Msk & ((value) << WDT_FUSES_PER_Pos))

#define WDT_FUSES_WEN_ADDR          (NVMCTRL_USER + 4)
#define WDT_FUSES_WEN_Pos           30           /**< \brief (NVMCTRL_USER) WDT Window Mode Enable */
#define WDT_FUSES_WEN_Msk           (_U_(0x1) << WDT_FUSES_WEN_Pos)

#define WDT_FUSES_WINDOW_ADDR       (NVMCTRL_USER + 4)
#define WDT_FUSES_WINDOW_Pos        22           /**< \brief (NVMCTRL_USER) WDT Window */
#define WDT_FUSES_WINDOW_Msk        (_U_(0xF) << WDT_FUSES_WINDOW_Pos)
#define WDT_FUSES_WINDOW(value)     (WDT_FUSES_WINDOW_Msk & ((value) << WDT_FUSES_WINDOW_Pos))

/*@}*/

#endif /* _SAMD51_NVMCTRL_COMPONENT_ */
