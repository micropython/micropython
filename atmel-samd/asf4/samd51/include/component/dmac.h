/**
 * \file
 *
 * \brief Component description for DMAC
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

#ifndef _SAMD51_DMAC_COMPONENT_
#define _SAMD51_DMAC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR DMAC */
/* ========================================================================== */
/** \addtogroup SAMD51_DMAC Direct Memory Access Controller */
/*@{*/

#define DMAC_U2503
#define REV_DMAC                    0x100

/* -------- DMAC_CTRL : (DMAC Offset: 0x00) (R/W 16) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint16_t DMAENABLE:1;      /*!< bit:      1  DMA Enable                         */
    uint16_t :6;               /*!< bit:  2.. 7  Reserved                           */
    uint16_t LVLEN0:1;         /*!< bit:      8  Priority Level 0 Enable            */
    uint16_t LVLEN1:1;         /*!< bit:      9  Priority Level 1 Enable            */
    uint16_t LVLEN2:1;         /*!< bit:     10  Priority Level 2 Enable            */
    uint16_t LVLEN3:1;         /*!< bit:     11  Priority Level 3 Enable            */
    uint16_t :4;               /*!< bit: 12..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t :8;               /*!< bit:  0.. 7  Reserved                           */
    uint16_t LVLEN:4;          /*!< bit:  8..11  Priority Level x Enable            */
    uint16_t :4;               /*!< bit: 12..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} DMAC_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CTRL_OFFSET            0x00         /**< \brief (DMAC_CTRL offset) Control */
#define DMAC_CTRL_RESETVALUE        _U_(0x0000)  /**< \brief (DMAC_CTRL reset_value) Control */

#define DMAC_CTRL_SWRST_Pos         0            /**< \brief (DMAC_CTRL) Software Reset */
#define DMAC_CTRL_SWRST             (_U_(0x1) << DMAC_CTRL_SWRST_Pos)
#define DMAC_CTRL_DMAENABLE_Pos     1            /**< \brief (DMAC_CTRL) DMA Enable */
#define DMAC_CTRL_DMAENABLE         (_U_(0x1) << DMAC_CTRL_DMAENABLE_Pos)
#define DMAC_CTRL_LVLEN0_Pos        8            /**< \brief (DMAC_CTRL) Priority Level 0 Enable */
#define DMAC_CTRL_LVLEN0            (_U_(1) << DMAC_CTRL_LVLEN0_Pos)
#define DMAC_CTRL_LVLEN1_Pos        9            /**< \brief (DMAC_CTRL) Priority Level 1 Enable */
#define DMAC_CTRL_LVLEN1            (_U_(1) << DMAC_CTRL_LVLEN1_Pos)
#define DMAC_CTRL_LVLEN2_Pos        10           /**< \brief (DMAC_CTRL) Priority Level 2 Enable */
#define DMAC_CTRL_LVLEN2            (_U_(1) << DMAC_CTRL_LVLEN2_Pos)
#define DMAC_CTRL_LVLEN3_Pos        11           /**< \brief (DMAC_CTRL) Priority Level 3 Enable */
#define DMAC_CTRL_LVLEN3            (_U_(1) << DMAC_CTRL_LVLEN3_Pos)
#define DMAC_CTRL_LVLEN_Pos         8            /**< \brief (DMAC_CTRL) Priority Level x Enable */
#define DMAC_CTRL_LVLEN_Msk         (_U_(0xF) << DMAC_CTRL_LVLEN_Pos)
#define DMAC_CTRL_LVLEN(value)      (DMAC_CTRL_LVLEN_Msk & ((value) << DMAC_CTRL_LVLEN_Pos))
#define DMAC_CTRL_MASK              _U_(0x0F03)  /**< \brief (DMAC_CTRL) MASK Register */

/* -------- DMAC_CRCCTRL : (DMAC Offset: 0x02) (R/W 16) CRC Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t CRCBEATSIZE:2;    /*!< bit:  0.. 1  CRC Beat Size                      */
    uint16_t CRCPOLY:2;        /*!< bit:  2.. 3  CRC Polynomial Type                */
    uint16_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint16_t CRCSRC:6;         /*!< bit:  8..13  CRC Input Source                   */
    uint16_t CRCMODE:2;        /*!< bit: 14..15  CRC Operating Mode                 */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} DMAC_CRCCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CRCCTRL_OFFSET         0x02         /**< \brief (DMAC_CRCCTRL offset) CRC Control */
#define DMAC_CRCCTRL_RESETVALUE     _U_(0x0000)  /**< \brief (DMAC_CRCCTRL reset_value) CRC Control */

#define DMAC_CRCCTRL_CRCBEATSIZE_Pos 0            /**< \brief (DMAC_CRCCTRL) CRC Beat Size */
#define DMAC_CRCCTRL_CRCBEATSIZE_Msk (_U_(0x3) << DMAC_CRCCTRL_CRCBEATSIZE_Pos)
#define DMAC_CRCCTRL_CRCBEATSIZE(value) (DMAC_CRCCTRL_CRCBEATSIZE_Msk & ((value) << DMAC_CRCCTRL_CRCBEATSIZE_Pos))
#define   DMAC_CRCCTRL_CRCBEATSIZE_BYTE_Val _U_(0x0)   /**< \brief (DMAC_CRCCTRL) 8-bit bus transfer */
#define   DMAC_CRCCTRL_CRCBEATSIZE_HWORD_Val _U_(0x1)   /**< \brief (DMAC_CRCCTRL) 16-bit bus transfer */
#define   DMAC_CRCCTRL_CRCBEATSIZE_WORD_Val _U_(0x2)   /**< \brief (DMAC_CRCCTRL) 32-bit bus transfer */
#define DMAC_CRCCTRL_CRCBEATSIZE_BYTE (DMAC_CRCCTRL_CRCBEATSIZE_BYTE_Val << DMAC_CRCCTRL_CRCBEATSIZE_Pos)
#define DMAC_CRCCTRL_CRCBEATSIZE_HWORD (DMAC_CRCCTRL_CRCBEATSIZE_HWORD_Val << DMAC_CRCCTRL_CRCBEATSIZE_Pos)
#define DMAC_CRCCTRL_CRCBEATSIZE_WORD (DMAC_CRCCTRL_CRCBEATSIZE_WORD_Val << DMAC_CRCCTRL_CRCBEATSIZE_Pos)
#define DMAC_CRCCTRL_CRCPOLY_Pos    2            /**< \brief (DMAC_CRCCTRL) CRC Polynomial Type */
#define DMAC_CRCCTRL_CRCPOLY_Msk    (_U_(0x3) << DMAC_CRCCTRL_CRCPOLY_Pos)
#define DMAC_CRCCTRL_CRCPOLY(value) (DMAC_CRCCTRL_CRCPOLY_Msk & ((value) << DMAC_CRCCTRL_CRCPOLY_Pos))
#define   DMAC_CRCCTRL_CRCPOLY_CRC16_Val  _U_(0x0)   /**< \brief (DMAC_CRCCTRL) CRC-16 (CRC-CCITT) */
#define   DMAC_CRCCTRL_CRCPOLY_CRC32_Val  _U_(0x1)   /**< \brief (DMAC_CRCCTRL) CRC32 (IEEE 802.3) */
#define DMAC_CRCCTRL_CRCPOLY_CRC16  (DMAC_CRCCTRL_CRCPOLY_CRC16_Val << DMAC_CRCCTRL_CRCPOLY_Pos)
#define DMAC_CRCCTRL_CRCPOLY_CRC32  (DMAC_CRCCTRL_CRCPOLY_CRC32_Val << DMAC_CRCCTRL_CRCPOLY_Pos)
#define DMAC_CRCCTRL_CRCSRC_Pos     8            /**< \brief (DMAC_CRCCTRL) CRC Input Source */
#define DMAC_CRCCTRL_CRCSRC_Msk     (_U_(0x3F) << DMAC_CRCCTRL_CRCSRC_Pos)
#define DMAC_CRCCTRL_CRCSRC(value)  (DMAC_CRCCTRL_CRCSRC_Msk & ((value) << DMAC_CRCCTRL_CRCSRC_Pos))
#define   DMAC_CRCCTRL_CRCSRC_DISABLE_Val _U_(0x0)   /**< \brief (DMAC_CRCCTRL) CRC Disabled */
#define   DMAC_CRCCTRL_CRCSRC_IO_Val      _U_(0x1)   /**< \brief (DMAC_CRCCTRL) I/O interface */
#define DMAC_CRCCTRL_CRCSRC_DISABLE (DMAC_CRCCTRL_CRCSRC_DISABLE_Val << DMAC_CRCCTRL_CRCSRC_Pos)
#define DMAC_CRCCTRL_CRCSRC_IO      (DMAC_CRCCTRL_CRCSRC_IO_Val    << DMAC_CRCCTRL_CRCSRC_Pos)
#define DMAC_CRCCTRL_CRCMODE_Pos    14           /**< \brief (DMAC_CRCCTRL) CRC Operating Mode */
#define DMAC_CRCCTRL_CRCMODE_Msk    (_U_(0x3) << DMAC_CRCCTRL_CRCMODE_Pos)
#define DMAC_CRCCTRL_CRCMODE(value) (DMAC_CRCCTRL_CRCMODE_Msk & ((value) << DMAC_CRCCTRL_CRCMODE_Pos))
#define   DMAC_CRCCTRL_CRCMODE_DEFAULT_Val _U_(0x0)   /**< \brief (DMAC_CRCCTRL) Default operating mode */
#define   DMAC_CRCCTRL_CRCMODE_CRCMON_Val _U_(0x2)   /**< \brief (DMAC_CRCCTRL) Memory CRC monitor operating mode */
#define   DMAC_CRCCTRL_CRCMODE_CRCGEN_Val _U_(0x3)   /**< \brief (DMAC_CRCCTRL) Memory CRC generation operating mode */
#define DMAC_CRCCTRL_CRCMODE_DEFAULT (DMAC_CRCCTRL_CRCMODE_DEFAULT_Val << DMAC_CRCCTRL_CRCMODE_Pos)
#define DMAC_CRCCTRL_CRCMODE_CRCMON (DMAC_CRCCTRL_CRCMODE_CRCMON_Val << DMAC_CRCCTRL_CRCMODE_Pos)
#define DMAC_CRCCTRL_CRCMODE_CRCGEN (DMAC_CRCCTRL_CRCMODE_CRCGEN_Val << DMAC_CRCCTRL_CRCMODE_Pos)
#define DMAC_CRCCTRL_MASK           _U_(0xFF0F)  /**< \brief (DMAC_CRCCTRL) MASK Register */

/* -------- DMAC_CRCDATAIN : (DMAC Offset: 0x04) (R/W 32) CRC Data Input -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CRCDATAIN:32;     /*!< bit:  0..31  CRC Data Input                     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_CRCDATAIN_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CRCDATAIN_OFFSET       0x04         /**< \brief (DMAC_CRCDATAIN offset) CRC Data Input */
#define DMAC_CRCDATAIN_RESETVALUE   _U_(0x00000000) /**< \brief (DMAC_CRCDATAIN reset_value) CRC Data Input */

#define DMAC_CRCDATAIN_CRCDATAIN_Pos 0            /**< \brief (DMAC_CRCDATAIN) CRC Data Input */
#define DMAC_CRCDATAIN_CRCDATAIN_Msk (_U_(0xFFFFFFFF) << DMAC_CRCDATAIN_CRCDATAIN_Pos)
#define DMAC_CRCDATAIN_CRCDATAIN(value) (DMAC_CRCDATAIN_CRCDATAIN_Msk & ((value) << DMAC_CRCDATAIN_CRCDATAIN_Pos))
#define DMAC_CRCDATAIN_MASK         _U_(0xFFFFFFFF) /**< \brief (DMAC_CRCDATAIN) MASK Register */

/* -------- DMAC_CRCCHKSUM : (DMAC Offset: 0x08) (R/W 32) CRC Checksum -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CRCCHKSUM:32;     /*!< bit:  0..31  CRC Checksum                       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_CRCCHKSUM_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CRCCHKSUM_OFFSET       0x08         /**< \brief (DMAC_CRCCHKSUM offset) CRC Checksum */
#define DMAC_CRCCHKSUM_RESETVALUE   _U_(0x00000000) /**< \brief (DMAC_CRCCHKSUM reset_value) CRC Checksum */

#define DMAC_CRCCHKSUM_CRCCHKSUM_Pos 0            /**< \brief (DMAC_CRCCHKSUM) CRC Checksum */
#define DMAC_CRCCHKSUM_CRCCHKSUM_Msk (_U_(0xFFFFFFFF) << DMAC_CRCCHKSUM_CRCCHKSUM_Pos)
#define DMAC_CRCCHKSUM_CRCCHKSUM(value) (DMAC_CRCCHKSUM_CRCCHKSUM_Msk & ((value) << DMAC_CRCCHKSUM_CRCCHKSUM_Pos))
#define DMAC_CRCCHKSUM_MASK         _U_(0xFFFFFFFF) /**< \brief (DMAC_CRCCHKSUM) MASK Register */

/* -------- DMAC_CRCSTATUS : (DMAC Offset: 0x0C) (R/W  8) CRC Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CRCBUSY:1;        /*!< bit:      0  CRC Module Busy                    */
    uint8_t  CRCZERO:1;        /*!< bit:      1  CRC Zero                           */
    uint8_t  CRCERR:1;         /*!< bit:      2  CRC Error                          */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_CRCSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CRCSTATUS_OFFSET       0x0C         /**< \brief (DMAC_CRCSTATUS offset) CRC Status */
#define DMAC_CRCSTATUS_RESETVALUE   _U_(0x00)    /**< \brief (DMAC_CRCSTATUS reset_value) CRC Status */

#define DMAC_CRCSTATUS_CRCBUSY_Pos  0            /**< \brief (DMAC_CRCSTATUS) CRC Module Busy */
#define DMAC_CRCSTATUS_CRCBUSY      (_U_(0x1) << DMAC_CRCSTATUS_CRCBUSY_Pos)
#define DMAC_CRCSTATUS_CRCZERO_Pos  1            /**< \brief (DMAC_CRCSTATUS) CRC Zero */
#define DMAC_CRCSTATUS_CRCZERO      (_U_(0x1) << DMAC_CRCSTATUS_CRCZERO_Pos)
#define DMAC_CRCSTATUS_CRCERR_Pos   2            /**< \brief (DMAC_CRCSTATUS) CRC Error */
#define DMAC_CRCSTATUS_CRCERR       (_U_(0x1) << DMAC_CRCSTATUS_CRCERR_Pos)
#define DMAC_CRCSTATUS_MASK         _U_(0x07)    /**< \brief (DMAC_CRCSTATUS) MASK Register */

/* -------- DMAC_DBGCTRL : (DMAC Offset: 0x0D) (R/W  8) Debug Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DBGRUN:1;         /*!< bit:      0  Debug Run                          */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_DBGCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_DBGCTRL_OFFSET         0x0D         /**< \brief (DMAC_DBGCTRL offset) Debug Control */
#define DMAC_DBGCTRL_RESETVALUE     _U_(0x00)    /**< \brief (DMAC_DBGCTRL reset_value) Debug Control */

#define DMAC_DBGCTRL_DBGRUN_Pos     0            /**< \brief (DMAC_DBGCTRL) Debug Run */
#define DMAC_DBGCTRL_DBGRUN         (_U_(0x1) << DMAC_DBGCTRL_DBGRUN_Pos)
#define DMAC_DBGCTRL_MASK           _U_(0x01)    /**< \brief (DMAC_DBGCTRL) MASK Register */

/* -------- DMAC_SWTRIGCTRL : (DMAC Offset: 0x10) (R/W 32) Software Trigger Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWTRIG0:1;        /*!< bit:      0  Channel 0 Software Trigger         */
    uint32_t SWTRIG1:1;        /*!< bit:      1  Channel 1 Software Trigger         */
    uint32_t SWTRIG2:1;        /*!< bit:      2  Channel 2 Software Trigger         */
    uint32_t SWTRIG3:1;        /*!< bit:      3  Channel 3 Software Trigger         */
    uint32_t SWTRIG4:1;        /*!< bit:      4  Channel 4 Software Trigger         */
    uint32_t SWTRIG5:1;        /*!< bit:      5  Channel 5 Software Trigger         */
    uint32_t SWTRIG6:1;        /*!< bit:      6  Channel 6 Software Trigger         */
    uint32_t SWTRIG7:1;        /*!< bit:      7  Channel 7 Software Trigger         */
    uint32_t SWTRIG8:1;        /*!< bit:      8  Channel 8 Software Trigger         */
    uint32_t SWTRIG9:1;        /*!< bit:      9  Channel 9 Software Trigger         */
    uint32_t SWTRIG10:1;       /*!< bit:     10  Channel 10 Software Trigger        */
    uint32_t SWTRIG11:1;       /*!< bit:     11  Channel 11 Software Trigger        */
    uint32_t SWTRIG12:1;       /*!< bit:     12  Channel 12 Software Trigger        */
    uint32_t SWTRIG13:1;       /*!< bit:     13  Channel 13 Software Trigger        */
    uint32_t SWTRIG14:1;       /*!< bit:     14  Channel 14 Software Trigger        */
    uint32_t SWTRIG15:1;       /*!< bit:     15  Channel 15 Software Trigger        */
    uint32_t SWTRIG16:1;       /*!< bit:     16  Channel 16 Software Trigger        */
    uint32_t SWTRIG17:1;       /*!< bit:     17  Channel 17 Software Trigger        */
    uint32_t SWTRIG18:1;       /*!< bit:     18  Channel 18 Software Trigger        */
    uint32_t SWTRIG19:1;       /*!< bit:     19  Channel 19 Software Trigger        */
    uint32_t SWTRIG20:1;       /*!< bit:     20  Channel 20 Software Trigger        */
    uint32_t SWTRIG21:1;       /*!< bit:     21  Channel 21 Software Trigger        */
    uint32_t SWTRIG22:1;       /*!< bit:     22  Channel 22 Software Trigger        */
    uint32_t SWTRIG23:1;       /*!< bit:     23  Channel 23 Software Trigger        */
    uint32_t SWTRIG24:1;       /*!< bit:     24  Channel 24 Software Trigger        */
    uint32_t SWTRIG25:1;       /*!< bit:     25  Channel 25 Software Trigger        */
    uint32_t SWTRIG26:1;       /*!< bit:     26  Channel 26 Software Trigger        */
    uint32_t SWTRIG27:1;       /*!< bit:     27  Channel 27 Software Trigger        */
    uint32_t SWTRIG28:1;       /*!< bit:     28  Channel 28 Software Trigger        */
    uint32_t SWTRIG29:1;       /*!< bit:     29  Channel 29 Software Trigger        */
    uint32_t SWTRIG30:1;       /*!< bit:     30  Channel 30 Software Trigger        */
    uint32_t SWTRIG31:1;       /*!< bit:     31  Channel 31 Software Trigger        */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t SWTRIG:32;        /*!< bit:  0..31  Channel x Software Trigger         */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_SWTRIGCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_SWTRIGCTRL_OFFSET      0x10         /**< \brief (DMAC_SWTRIGCTRL offset) Software Trigger Control */
#define DMAC_SWTRIGCTRL_RESETVALUE  _U_(0x00000000) /**< \brief (DMAC_SWTRIGCTRL reset_value) Software Trigger Control */

#define DMAC_SWTRIGCTRL_SWTRIG0_Pos 0            /**< \brief (DMAC_SWTRIGCTRL) Channel 0 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG0     (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG0_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG1_Pos 1            /**< \brief (DMAC_SWTRIGCTRL) Channel 1 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG1     (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG1_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG2_Pos 2            /**< \brief (DMAC_SWTRIGCTRL) Channel 2 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG2     (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG2_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG3_Pos 3            /**< \brief (DMAC_SWTRIGCTRL) Channel 3 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG3     (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG3_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG4_Pos 4            /**< \brief (DMAC_SWTRIGCTRL) Channel 4 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG4     (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG4_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG5_Pos 5            /**< \brief (DMAC_SWTRIGCTRL) Channel 5 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG5     (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG5_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG6_Pos 6            /**< \brief (DMAC_SWTRIGCTRL) Channel 6 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG6     (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG6_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG7_Pos 7            /**< \brief (DMAC_SWTRIGCTRL) Channel 7 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG7     (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG7_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG8_Pos 8            /**< \brief (DMAC_SWTRIGCTRL) Channel 8 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG8     (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG8_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG9_Pos 9            /**< \brief (DMAC_SWTRIGCTRL) Channel 9 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG9     (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG9_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG10_Pos 10           /**< \brief (DMAC_SWTRIGCTRL) Channel 10 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG10    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG10_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG11_Pos 11           /**< \brief (DMAC_SWTRIGCTRL) Channel 11 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG11    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG11_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG12_Pos 12           /**< \brief (DMAC_SWTRIGCTRL) Channel 12 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG12    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG12_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG13_Pos 13           /**< \brief (DMAC_SWTRIGCTRL) Channel 13 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG13    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG13_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG14_Pos 14           /**< \brief (DMAC_SWTRIGCTRL) Channel 14 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG14    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG14_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG15_Pos 15           /**< \brief (DMAC_SWTRIGCTRL) Channel 15 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG15    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG15_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG16_Pos 16           /**< \brief (DMAC_SWTRIGCTRL) Channel 16 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG16    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG16_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG17_Pos 17           /**< \brief (DMAC_SWTRIGCTRL) Channel 17 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG17    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG17_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG18_Pos 18           /**< \brief (DMAC_SWTRIGCTRL) Channel 18 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG18    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG18_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG19_Pos 19           /**< \brief (DMAC_SWTRIGCTRL) Channel 19 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG19    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG19_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG20_Pos 20           /**< \brief (DMAC_SWTRIGCTRL) Channel 20 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG20    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG20_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG21_Pos 21           /**< \brief (DMAC_SWTRIGCTRL) Channel 21 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG21    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG21_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG22_Pos 22           /**< \brief (DMAC_SWTRIGCTRL) Channel 22 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG22    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG22_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG23_Pos 23           /**< \brief (DMAC_SWTRIGCTRL) Channel 23 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG23    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG23_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG24_Pos 24           /**< \brief (DMAC_SWTRIGCTRL) Channel 24 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG24    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG24_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG25_Pos 25           /**< \brief (DMAC_SWTRIGCTRL) Channel 25 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG25    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG25_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG26_Pos 26           /**< \brief (DMAC_SWTRIGCTRL) Channel 26 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG26    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG26_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG27_Pos 27           /**< \brief (DMAC_SWTRIGCTRL) Channel 27 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG27    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG27_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG28_Pos 28           /**< \brief (DMAC_SWTRIGCTRL) Channel 28 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG28    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG28_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG29_Pos 29           /**< \brief (DMAC_SWTRIGCTRL) Channel 29 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG29    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG29_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG30_Pos 30           /**< \brief (DMAC_SWTRIGCTRL) Channel 30 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG30    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG30_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG31_Pos 31           /**< \brief (DMAC_SWTRIGCTRL) Channel 31 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG31    (_U_(1) << DMAC_SWTRIGCTRL_SWTRIG31_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG_Pos  0            /**< \brief (DMAC_SWTRIGCTRL) Channel x Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG_Msk  (_U_(0xFFFFFFFF) << DMAC_SWTRIGCTRL_SWTRIG_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG(value) (DMAC_SWTRIGCTRL_SWTRIG_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG_Pos))
#define DMAC_SWTRIGCTRL_MASK        _U_(0xFFFFFFFF) /**< \brief (DMAC_SWTRIGCTRL) MASK Register */

/* -------- DMAC_PRICTRL0 : (DMAC Offset: 0x14) (R/W 32) Priority Control 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t LVLPRI0:5;        /*!< bit:  0.. 4  Level 0 Channel Priority Number    */
    uint32_t QOS0:2;           /*!< bit:  5.. 6  Level 0 Quality of Service         */
    uint32_t RRLVLEN0:1;       /*!< bit:      7  Level 0 Round-Robin Scheduling Enable */
    uint32_t LVLPRI1:5;        /*!< bit:  8..12  Level 1 Channel Priority Number    */
    uint32_t QOS1:2;           /*!< bit: 13..14  Level 1 Quality of Service         */
    uint32_t RRLVLEN1:1;       /*!< bit:     15  Level 1 Round-Robin Scheduling Enable */
    uint32_t LVLPRI2:5;        /*!< bit: 16..20  Level 2 Channel Priority Number    */
    uint32_t QOS2:2;           /*!< bit: 21..22  Level 2 Quality of Service         */
    uint32_t RRLVLEN2:1;       /*!< bit:     23  Level 2 Round-Robin Scheduling Enable */
    uint32_t LVLPRI3:5;        /*!< bit: 24..28  Level 3 Channel Priority Number    */
    uint32_t QOS3:2;           /*!< bit: 29..30  Level 3 Quality of Service         */
    uint32_t RRLVLEN3:1;       /*!< bit:     31  Level 3 Round-Robin Scheduling Enable */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_PRICTRL0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_PRICTRL0_OFFSET        0x14         /**< \brief (DMAC_PRICTRL0 offset) Priority Control 0 */
#define DMAC_PRICTRL0_RESETVALUE    _U_(0x40404040) /**< \brief (DMAC_PRICTRL0 reset_value) Priority Control 0 */

#define DMAC_PRICTRL0_LVLPRI0_Pos   0            /**< \brief (DMAC_PRICTRL0) Level 0 Channel Priority Number */
#define DMAC_PRICTRL0_LVLPRI0_Msk   (_U_(0x1F) << DMAC_PRICTRL0_LVLPRI0_Pos)
#define DMAC_PRICTRL0_LVLPRI0(value) (DMAC_PRICTRL0_LVLPRI0_Msk & ((value) << DMAC_PRICTRL0_LVLPRI0_Pos))
#define DMAC_PRICTRL0_QOS0_Pos      5            /**< \brief (DMAC_PRICTRL0) Level 0 Quality of Service */
#define DMAC_PRICTRL0_QOS0_Msk      (_U_(0x3) << DMAC_PRICTRL0_QOS0_Pos)
#define DMAC_PRICTRL0_QOS0(value)   (DMAC_PRICTRL0_QOS0_Msk & ((value) << DMAC_PRICTRL0_QOS0_Pos))
#define   DMAC_PRICTRL0_QOS0_REGULAR_Val  _U_(0x0)   /**< \brief (DMAC_PRICTRL0) Regular delivery */
#define   DMAC_PRICTRL0_QOS0_SHORTAGE_Val _U_(0x1)   /**< \brief (DMAC_PRICTRL0) Bandwidth shortage */
#define   DMAC_PRICTRL0_QOS0_SENSITIVE_Val _U_(0x2)   /**< \brief (DMAC_PRICTRL0) Latency sensitive */
#define   DMAC_PRICTRL0_QOS0_CRITICAL_Val _U_(0x3)   /**< \brief (DMAC_PRICTRL0) Latency critical */
#define DMAC_PRICTRL0_QOS0_REGULAR  (DMAC_PRICTRL0_QOS0_REGULAR_Val << DMAC_PRICTRL0_QOS0_Pos)
#define DMAC_PRICTRL0_QOS0_SHORTAGE (DMAC_PRICTRL0_QOS0_SHORTAGE_Val << DMAC_PRICTRL0_QOS0_Pos)
#define DMAC_PRICTRL0_QOS0_SENSITIVE (DMAC_PRICTRL0_QOS0_SENSITIVE_Val << DMAC_PRICTRL0_QOS0_Pos)
#define DMAC_PRICTRL0_QOS0_CRITICAL (DMAC_PRICTRL0_QOS0_CRITICAL_Val << DMAC_PRICTRL0_QOS0_Pos)
#define DMAC_PRICTRL0_RRLVLEN0_Pos  7            /**< \brief (DMAC_PRICTRL0) Level 0 Round-Robin Scheduling Enable */
#define DMAC_PRICTRL0_RRLVLEN0      (_U_(0x1) << DMAC_PRICTRL0_RRLVLEN0_Pos)
#define DMAC_PRICTRL0_LVLPRI1_Pos   8            /**< \brief (DMAC_PRICTRL0) Level 1 Channel Priority Number */
#define DMAC_PRICTRL0_LVLPRI1_Msk   (_U_(0x1F) << DMAC_PRICTRL0_LVLPRI1_Pos)
#define DMAC_PRICTRL0_LVLPRI1(value) (DMAC_PRICTRL0_LVLPRI1_Msk & ((value) << DMAC_PRICTRL0_LVLPRI1_Pos))
#define DMAC_PRICTRL0_QOS1_Pos      13           /**< \brief (DMAC_PRICTRL0) Level 1 Quality of Service */
#define DMAC_PRICTRL0_QOS1_Msk      (_U_(0x3) << DMAC_PRICTRL0_QOS1_Pos)
#define DMAC_PRICTRL0_QOS1(value)   (DMAC_PRICTRL0_QOS1_Msk & ((value) << DMAC_PRICTRL0_QOS1_Pos))
#define   DMAC_PRICTRL0_QOS1_REGULAR_Val  _U_(0x0)   /**< \brief (DMAC_PRICTRL0) Regular delivery */
#define   DMAC_PRICTRL0_QOS1_SHORTAGE_Val _U_(0x1)   /**< \brief (DMAC_PRICTRL0) Bandwidth shortage */
#define   DMAC_PRICTRL0_QOS1_SENSITIVE_Val _U_(0x2)   /**< \brief (DMAC_PRICTRL0) Latency sensitive */
#define   DMAC_PRICTRL0_QOS1_CRITICAL_Val _U_(0x3)   /**< \brief (DMAC_PRICTRL0) Latency critical */
#define DMAC_PRICTRL0_QOS1_REGULAR  (DMAC_PRICTRL0_QOS1_REGULAR_Val << DMAC_PRICTRL0_QOS1_Pos)
#define DMAC_PRICTRL0_QOS1_SHORTAGE (DMAC_PRICTRL0_QOS1_SHORTAGE_Val << DMAC_PRICTRL0_QOS1_Pos)
#define DMAC_PRICTRL0_QOS1_SENSITIVE (DMAC_PRICTRL0_QOS1_SENSITIVE_Val << DMAC_PRICTRL0_QOS1_Pos)
#define DMAC_PRICTRL0_QOS1_CRITICAL (DMAC_PRICTRL0_QOS1_CRITICAL_Val << DMAC_PRICTRL0_QOS1_Pos)
#define DMAC_PRICTRL0_RRLVLEN1_Pos  15           /**< \brief (DMAC_PRICTRL0) Level 1 Round-Robin Scheduling Enable */
#define DMAC_PRICTRL0_RRLVLEN1      (_U_(0x1) << DMAC_PRICTRL0_RRLVLEN1_Pos)
#define DMAC_PRICTRL0_LVLPRI2_Pos   16           /**< \brief (DMAC_PRICTRL0) Level 2 Channel Priority Number */
#define DMAC_PRICTRL0_LVLPRI2_Msk   (_U_(0x1F) << DMAC_PRICTRL0_LVLPRI2_Pos)
#define DMAC_PRICTRL0_LVLPRI2(value) (DMAC_PRICTRL0_LVLPRI2_Msk & ((value) << DMAC_PRICTRL0_LVLPRI2_Pos))
#define DMAC_PRICTRL0_QOS2_Pos      21           /**< \brief (DMAC_PRICTRL0) Level 2 Quality of Service */
#define DMAC_PRICTRL0_QOS2_Msk      (_U_(0x3) << DMAC_PRICTRL0_QOS2_Pos)
#define DMAC_PRICTRL0_QOS2(value)   (DMAC_PRICTRL0_QOS2_Msk & ((value) << DMAC_PRICTRL0_QOS2_Pos))
#define   DMAC_PRICTRL0_QOS2_REGULAR_Val  _U_(0x0)   /**< \brief (DMAC_PRICTRL0) Regular delivery */
#define   DMAC_PRICTRL0_QOS2_SHORTAGE_Val _U_(0x1)   /**< \brief (DMAC_PRICTRL0) Bandwidth shortage */
#define   DMAC_PRICTRL0_QOS2_SENSITIVE_Val _U_(0x2)   /**< \brief (DMAC_PRICTRL0) Latency sensitive */
#define   DMAC_PRICTRL0_QOS2_CRITICAL_Val _U_(0x3)   /**< \brief (DMAC_PRICTRL0) Latency critical */
#define DMAC_PRICTRL0_QOS2_REGULAR  (DMAC_PRICTRL0_QOS2_REGULAR_Val << DMAC_PRICTRL0_QOS2_Pos)
#define DMAC_PRICTRL0_QOS2_SHORTAGE (DMAC_PRICTRL0_QOS2_SHORTAGE_Val << DMAC_PRICTRL0_QOS2_Pos)
#define DMAC_PRICTRL0_QOS2_SENSITIVE (DMAC_PRICTRL0_QOS2_SENSITIVE_Val << DMAC_PRICTRL0_QOS2_Pos)
#define DMAC_PRICTRL0_QOS2_CRITICAL (DMAC_PRICTRL0_QOS2_CRITICAL_Val << DMAC_PRICTRL0_QOS2_Pos)
#define DMAC_PRICTRL0_RRLVLEN2_Pos  23           /**< \brief (DMAC_PRICTRL0) Level 2 Round-Robin Scheduling Enable */
#define DMAC_PRICTRL0_RRLVLEN2      (_U_(0x1) << DMAC_PRICTRL0_RRLVLEN2_Pos)
#define DMAC_PRICTRL0_LVLPRI3_Pos   24           /**< \brief (DMAC_PRICTRL0) Level 3 Channel Priority Number */
#define DMAC_PRICTRL0_LVLPRI3_Msk   (_U_(0x1F) << DMAC_PRICTRL0_LVLPRI3_Pos)
#define DMAC_PRICTRL0_LVLPRI3(value) (DMAC_PRICTRL0_LVLPRI3_Msk & ((value) << DMAC_PRICTRL0_LVLPRI3_Pos))
#define DMAC_PRICTRL0_QOS3_Pos      29           /**< \brief (DMAC_PRICTRL0) Level 3 Quality of Service */
#define DMAC_PRICTRL0_QOS3_Msk      (_U_(0x3) << DMAC_PRICTRL0_QOS3_Pos)
#define DMAC_PRICTRL0_QOS3(value)   (DMAC_PRICTRL0_QOS3_Msk & ((value) << DMAC_PRICTRL0_QOS3_Pos))
#define   DMAC_PRICTRL0_QOS3_REGULAR_Val  _U_(0x0)   /**< \brief (DMAC_PRICTRL0) Regular delivery */
#define   DMAC_PRICTRL0_QOS3_SHORTAGE_Val _U_(0x1)   /**< \brief (DMAC_PRICTRL0) Bandwidth shortage */
#define   DMAC_PRICTRL0_QOS3_SENSITIVE_Val _U_(0x2)   /**< \brief (DMAC_PRICTRL0) Latency sensitive */
#define   DMAC_PRICTRL0_QOS3_CRITICAL_Val _U_(0x3)   /**< \brief (DMAC_PRICTRL0) Latency critical */
#define DMAC_PRICTRL0_QOS3_REGULAR  (DMAC_PRICTRL0_QOS3_REGULAR_Val << DMAC_PRICTRL0_QOS3_Pos)
#define DMAC_PRICTRL0_QOS3_SHORTAGE (DMAC_PRICTRL0_QOS3_SHORTAGE_Val << DMAC_PRICTRL0_QOS3_Pos)
#define DMAC_PRICTRL0_QOS3_SENSITIVE (DMAC_PRICTRL0_QOS3_SENSITIVE_Val << DMAC_PRICTRL0_QOS3_Pos)
#define DMAC_PRICTRL0_QOS3_CRITICAL (DMAC_PRICTRL0_QOS3_CRITICAL_Val << DMAC_PRICTRL0_QOS3_Pos)
#define DMAC_PRICTRL0_RRLVLEN3_Pos  31           /**< \brief (DMAC_PRICTRL0) Level 3 Round-Robin Scheduling Enable */
#define DMAC_PRICTRL0_RRLVLEN3      (_U_(0x1) << DMAC_PRICTRL0_RRLVLEN3_Pos)
#define DMAC_PRICTRL0_MASK          _U_(0xFFFFFFFF) /**< \brief (DMAC_PRICTRL0) MASK Register */

/* -------- DMAC_INTPEND : (DMAC Offset: 0x20) (R/W 16) Interrupt Pending -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t ID:5;             /*!< bit:  0.. 4  Channel ID                         */
    uint16_t :3;               /*!< bit:  5.. 7  Reserved                           */
    uint16_t TERR:1;           /*!< bit:      8  Transfer Error                     */
    uint16_t TCMPL:1;          /*!< bit:      9  Transfer Complete                  */
    uint16_t SUSP:1;           /*!< bit:     10  Channel Suspend                    */
    uint16_t :1;               /*!< bit:     11  Reserved                           */
    uint16_t CRCERR:1;         /*!< bit:     12  CRC Error                          */
    uint16_t FERR:1;           /*!< bit:     13  Fetch Error                        */
    uint16_t BUSY:1;           /*!< bit:     14  Busy                               */
    uint16_t PEND:1;           /*!< bit:     15  Pending                            */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} DMAC_INTPEND_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_INTPEND_OFFSET         0x20         /**< \brief (DMAC_INTPEND offset) Interrupt Pending */
#define DMAC_INTPEND_RESETVALUE     _U_(0x0000)  /**< \brief (DMAC_INTPEND reset_value) Interrupt Pending */

#define DMAC_INTPEND_ID_Pos         0            /**< \brief (DMAC_INTPEND) Channel ID */
#define DMAC_INTPEND_ID_Msk         (_U_(0x1F) << DMAC_INTPEND_ID_Pos)
#define DMAC_INTPEND_ID(value)      (DMAC_INTPEND_ID_Msk & ((value) << DMAC_INTPEND_ID_Pos))
#define DMAC_INTPEND_TERR_Pos       8            /**< \brief (DMAC_INTPEND) Transfer Error */
#define DMAC_INTPEND_TERR           (_U_(0x1) << DMAC_INTPEND_TERR_Pos)
#define DMAC_INTPEND_TCMPL_Pos      9            /**< \brief (DMAC_INTPEND) Transfer Complete */
#define DMAC_INTPEND_TCMPL          (_U_(0x1) << DMAC_INTPEND_TCMPL_Pos)
#define DMAC_INTPEND_SUSP_Pos       10           /**< \brief (DMAC_INTPEND) Channel Suspend */
#define DMAC_INTPEND_SUSP           (_U_(0x1) << DMAC_INTPEND_SUSP_Pos)
#define DMAC_INTPEND_CRCERR_Pos     12           /**< \brief (DMAC_INTPEND) CRC Error */
#define DMAC_INTPEND_CRCERR         (_U_(0x1) << DMAC_INTPEND_CRCERR_Pos)
#define DMAC_INTPEND_FERR_Pos       13           /**< \brief (DMAC_INTPEND) Fetch Error */
#define DMAC_INTPEND_FERR           (_U_(0x1) << DMAC_INTPEND_FERR_Pos)
#define DMAC_INTPEND_BUSY_Pos       14           /**< \brief (DMAC_INTPEND) Busy */
#define DMAC_INTPEND_BUSY           (_U_(0x1) << DMAC_INTPEND_BUSY_Pos)
#define DMAC_INTPEND_PEND_Pos       15           /**< \brief (DMAC_INTPEND) Pending */
#define DMAC_INTPEND_PEND           (_U_(0x1) << DMAC_INTPEND_PEND_Pos)
#define DMAC_INTPEND_MASK           _U_(0xF71F)  /**< \brief (DMAC_INTPEND) MASK Register */

/* -------- DMAC_INTSTATUS : (DMAC Offset: 0x24) (R/  32) Interrupt Status -------- */
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
    uint32_t CHINT12:1;        /*!< bit:     12  Channel 12 Pending Interrupt       */
    uint32_t CHINT13:1;        /*!< bit:     13  Channel 13 Pending Interrupt       */
    uint32_t CHINT14:1;        /*!< bit:     14  Channel 14 Pending Interrupt       */
    uint32_t CHINT15:1;        /*!< bit:     15  Channel 15 Pending Interrupt       */
    uint32_t CHINT16:1;        /*!< bit:     16  Channel 16 Pending Interrupt       */
    uint32_t CHINT17:1;        /*!< bit:     17  Channel 17 Pending Interrupt       */
    uint32_t CHINT18:1;        /*!< bit:     18  Channel 18 Pending Interrupt       */
    uint32_t CHINT19:1;        /*!< bit:     19  Channel 19 Pending Interrupt       */
    uint32_t CHINT20:1;        /*!< bit:     20  Channel 20 Pending Interrupt       */
    uint32_t CHINT21:1;        /*!< bit:     21  Channel 21 Pending Interrupt       */
    uint32_t CHINT22:1;        /*!< bit:     22  Channel 22 Pending Interrupt       */
    uint32_t CHINT23:1;        /*!< bit:     23  Channel 23 Pending Interrupt       */
    uint32_t CHINT24:1;        /*!< bit:     24  Channel 24 Pending Interrupt       */
    uint32_t CHINT25:1;        /*!< bit:     25  Channel 25 Pending Interrupt       */
    uint32_t CHINT26:1;        /*!< bit:     26  Channel 26 Pending Interrupt       */
    uint32_t CHINT27:1;        /*!< bit:     27  Channel 27 Pending Interrupt       */
    uint32_t CHINT28:1;        /*!< bit:     28  Channel 28 Pending Interrupt       */
    uint32_t CHINT29:1;        /*!< bit:     29  Channel 29 Pending Interrupt       */
    uint32_t CHINT30:1;        /*!< bit:     30  Channel 30 Pending Interrupt       */
    uint32_t CHINT31:1;        /*!< bit:     31  Channel 31 Pending Interrupt       */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t CHINT:32;         /*!< bit:  0..31  Channel x Pending Interrupt        */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_INTSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_INTSTATUS_OFFSET       0x24         /**< \brief (DMAC_INTSTATUS offset) Interrupt Status */
#define DMAC_INTSTATUS_RESETVALUE   _U_(0x00000000) /**< \brief (DMAC_INTSTATUS reset_value) Interrupt Status */

#define DMAC_INTSTATUS_CHINT0_Pos   0            /**< \brief (DMAC_INTSTATUS) Channel 0 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT0       (_U_(1) << DMAC_INTSTATUS_CHINT0_Pos)
#define DMAC_INTSTATUS_CHINT1_Pos   1            /**< \brief (DMAC_INTSTATUS) Channel 1 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT1       (_U_(1) << DMAC_INTSTATUS_CHINT1_Pos)
#define DMAC_INTSTATUS_CHINT2_Pos   2            /**< \brief (DMAC_INTSTATUS) Channel 2 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT2       (_U_(1) << DMAC_INTSTATUS_CHINT2_Pos)
#define DMAC_INTSTATUS_CHINT3_Pos   3            /**< \brief (DMAC_INTSTATUS) Channel 3 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT3       (_U_(1) << DMAC_INTSTATUS_CHINT3_Pos)
#define DMAC_INTSTATUS_CHINT4_Pos   4            /**< \brief (DMAC_INTSTATUS) Channel 4 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT4       (_U_(1) << DMAC_INTSTATUS_CHINT4_Pos)
#define DMAC_INTSTATUS_CHINT5_Pos   5            /**< \brief (DMAC_INTSTATUS) Channel 5 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT5       (_U_(1) << DMAC_INTSTATUS_CHINT5_Pos)
#define DMAC_INTSTATUS_CHINT6_Pos   6            /**< \brief (DMAC_INTSTATUS) Channel 6 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT6       (_U_(1) << DMAC_INTSTATUS_CHINT6_Pos)
#define DMAC_INTSTATUS_CHINT7_Pos   7            /**< \brief (DMAC_INTSTATUS) Channel 7 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT7       (_U_(1) << DMAC_INTSTATUS_CHINT7_Pos)
#define DMAC_INTSTATUS_CHINT8_Pos   8            /**< \brief (DMAC_INTSTATUS) Channel 8 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT8       (_U_(1) << DMAC_INTSTATUS_CHINT8_Pos)
#define DMAC_INTSTATUS_CHINT9_Pos   9            /**< \brief (DMAC_INTSTATUS) Channel 9 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT9       (_U_(1) << DMAC_INTSTATUS_CHINT9_Pos)
#define DMAC_INTSTATUS_CHINT10_Pos  10           /**< \brief (DMAC_INTSTATUS) Channel 10 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT10      (_U_(1) << DMAC_INTSTATUS_CHINT10_Pos)
#define DMAC_INTSTATUS_CHINT11_Pos  11           /**< \brief (DMAC_INTSTATUS) Channel 11 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT11      (_U_(1) << DMAC_INTSTATUS_CHINT11_Pos)
#define DMAC_INTSTATUS_CHINT12_Pos  12           /**< \brief (DMAC_INTSTATUS) Channel 12 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT12      (_U_(1) << DMAC_INTSTATUS_CHINT12_Pos)
#define DMAC_INTSTATUS_CHINT13_Pos  13           /**< \brief (DMAC_INTSTATUS) Channel 13 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT13      (_U_(1) << DMAC_INTSTATUS_CHINT13_Pos)
#define DMAC_INTSTATUS_CHINT14_Pos  14           /**< \brief (DMAC_INTSTATUS) Channel 14 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT14      (_U_(1) << DMAC_INTSTATUS_CHINT14_Pos)
#define DMAC_INTSTATUS_CHINT15_Pos  15           /**< \brief (DMAC_INTSTATUS) Channel 15 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT15      (_U_(1) << DMAC_INTSTATUS_CHINT15_Pos)
#define DMAC_INTSTATUS_CHINT16_Pos  16           /**< \brief (DMAC_INTSTATUS) Channel 16 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT16      (_U_(1) << DMAC_INTSTATUS_CHINT16_Pos)
#define DMAC_INTSTATUS_CHINT17_Pos  17           /**< \brief (DMAC_INTSTATUS) Channel 17 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT17      (_U_(1) << DMAC_INTSTATUS_CHINT17_Pos)
#define DMAC_INTSTATUS_CHINT18_Pos  18           /**< \brief (DMAC_INTSTATUS) Channel 18 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT18      (_U_(1) << DMAC_INTSTATUS_CHINT18_Pos)
#define DMAC_INTSTATUS_CHINT19_Pos  19           /**< \brief (DMAC_INTSTATUS) Channel 19 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT19      (_U_(1) << DMAC_INTSTATUS_CHINT19_Pos)
#define DMAC_INTSTATUS_CHINT20_Pos  20           /**< \brief (DMAC_INTSTATUS) Channel 20 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT20      (_U_(1) << DMAC_INTSTATUS_CHINT20_Pos)
#define DMAC_INTSTATUS_CHINT21_Pos  21           /**< \brief (DMAC_INTSTATUS) Channel 21 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT21      (_U_(1) << DMAC_INTSTATUS_CHINT21_Pos)
#define DMAC_INTSTATUS_CHINT22_Pos  22           /**< \brief (DMAC_INTSTATUS) Channel 22 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT22      (_U_(1) << DMAC_INTSTATUS_CHINT22_Pos)
#define DMAC_INTSTATUS_CHINT23_Pos  23           /**< \brief (DMAC_INTSTATUS) Channel 23 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT23      (_U_(1) << DMAC_INTSTATUS_CHINT23_Pos)
#define DMAC_INTSTATUS_CHINT24_Pos  24           /**< \brief (DMAC_INTSTATUS) Channel 24 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT24      (_U_(1) << DMAC_INTSTATUS_CHINT24_Pos)
#define DMAC_INTSTATUS_CHINT25_Pos  25           /**< \brief (DMAC_INTSTATUS) Channel 25 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT25      (_U_(1) << DMAC_INTSTATUS_CHINT25_Pos)
#define DMAC_INTSTATUS_CHINT26_Pos  26           /**< \brief (DMAC_INTSTATUS) Channel 26 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT26      (_U_(1) << DMAC_INTSTATUS_CHINT26_Pos)
#define DMAC_INTSTATUS_CHINT27_Pos  27           /**< \brief (DMAC_INTSTATUS) Channel 27 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT27      (_U_(1) << DMAC_INTSTATUS_CHINT27_Pos)
#define DMAC_INTSTATUS_CHINT28_Pos  28           /**< \brief (DMAC_INTSTATUS) Channel 28 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT28      (_U_(1) << DMAC_INTSTATUS_CHINT28_Pos)
#define DMAC_INTSTATUS_CHINT29_Pos  29           /**< \brief (DMAC_INTSTATUS) Channel 29 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT29      (_U_(1) << DMAC_INTSTATUS_CHINT29_Pos)
#define DMAC_INTSTATUS_CHINT30_Pos  30           /**< \brief (DMAC_INTSTATUS) Channel 30 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT30      (_U_(1) << DMAC_INTSTATUS_CHINT30_Pos)
#define DMAC_INTSTATUS_CHINT31_Pos  31           /**< \brief (DMAC_INTSTATUS) Channel 31 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT31      (_U_(1) << DMAC_INTSTATUS_CHINT31_Pos)
#define DMAC_INTSTATUS_CHINT_Pos    0            /**< \brief (DMAC_INTSTATUS) Channel x Pending Interrupt */
#define DMAC_INTSTATUS_CHINT_Msk    (_U_(0xFFFFFFFF) << DMAC_INTSTATUS_CHINT_Pos)
#define DMAC_INTSTATUS_CHINT(value) (DMAC_INTSTATUS_CHINT_Msk & ((value) << DMAC_INTSTATUS_CHINT_Pos))
#define DMAC_INTSTATUS_MASK         _U_(0xFFFFFFFF) /**< \brief (DMAC_INTSTATUS) MASK Register */

/* -------- DMAC_BUSYCH : (DMAC Offset: 0x28) (R/  32) Busy Channels -------- */
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
    uint32_t BUSYCH12:1;       /*!< bit:     12  Busy Channel 12                    */
    uint32_t BUSYCH13:1;       /*!< bit:     13  Busy Channel 13                    */
    uint32_t BUSYCH14:1;       /*!< bit:     14  Busy Channel 14                    */
    uint32_t BUSYCH15:1;       /*!< bit:     15  Busy Channel 15                    */
    uint32_t BUSYCH16:1;       /*!< bit:     16  Busy Channel 16                    */
    uint32_t BUSYCH17:1;       /*!< bit:     17  Busy Channel 17                    */
    uint32_t BUSYCH18:1;       /*!< bit:     18  Busy Channel 18                    */
    uint32_t BUSYCH19:1;       /*!< bit:     19  Busy Channel 19                    */
    uint32_t BUSYCH20:1;       /*!< bit:     20  Busy Channel 20                    */
    uint32_t BUSYCH21:1;       /*!< bit:     21  Busy Channel 21                    */
    uint32_t BUSYCH22:1;       /*!< bit:     22  Busy Channel 22                    */
    uint32_t BUSYCH23:1;       /*!< bit:     23  Busy Channel 23                    */
    uint32_t BUSYCH24:1;       /*!< bit:     24  Busy Channel 24                    */
    uint32_t BUSYCH25:1;       /*!< bit:     25  Busy Channel 25                    */
    uint32_t BUSYCH26:1;       /*!< bit:     26  Busy Channel 26                    */
    uint32_t BUSYCH27:1;       /*!< bit:     27  Busy Channel 27                    */
    uint32_t BUSYCH28:1;       /*!< bit:     28  Busy Channel 28                    */
    uint32_t BUSYCH29:1;       /*!< bit:     29  Busy Channel 29                    */
    uint32_t BUSYCH30:1;       /*!< bit:     30  Busy Channel 30                    */
    uint32_t BUSYCH31:1;       /*!< bit:     31  Busy Channel 31                    */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t BUSYCH:32;        /*!< bit:  0..31  Busy Channel x                     */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_BUSYCH_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_BUSYCH_OFFSET          0x28         /**< \brief (DMAC_BUSYCH offset) Busy Channels */
#define DMAC_BUSYCH_RESETVALUE      _U_(0x00000000) /**< \brief (DMAC_BUSYCH reset_value) Busy Channels */

#define DMAC_BUSYCH_BUSYCH0_Pos     0            /**< \brief (DMAC_BUSYCH) Busy Channel 0 */
#define DMAC_BUSYCH_BUSYCH0         (_U_(1) << DMAC_BUSYCH_BUSYCH0_Pos)
#define DMAC_BUSYCH_BUSYCH1_Pos     1            /**< \brief (DMAC_BUSYCH) Busy Channel 1 */
#define DMAC_BUSYCH_BUSYCH1         (_U_(1) << DMAC_BUSYCH_BUSYCH1_Pos)
#define DMAC_BUSYCH_BUSYCH2_Pos     2            /**< \brief (DMAC_BUSYCH) Busy Channel 2 */
#define DMAC_BUSYCH_BUSYCH2         (_U_(1) << DMAC_BUSYCH_BUSYCH2_Pos)
#define DMAC_BUSYCH_BUSYCH3_Pos     3            /**< \brief (DMAC_BUSYCH) Busy Channel 3 */
#define DMAC_BUSYCH_BUSYCH3         (_U_(1) << DMAC_BUSYCH_BUSYCH3_Pos)
#define DMAC_BUSYCH_BUSYCH4_Pos     4            /**< \brief (DMAC_BUSYCH) Busy Channel 4 */
#define DMAC_BUSYCH_BUSYCH4         (_U_(1) << DMAC_BUSYCH_BUSYCH4_Pos)
#define DMAC_BUSYCH_BUSYCH5_Pos     5            /**< \brief (DMAC_BUSYCH) Busy Channel 5 */
#define DMAC_BUSYCH_BUSYCH5         (_U_(1) << DMAC_BUSYCH_BUSYCH5_Pos)
#define DMAC_BUSYCH_BUSYCH6_Pos     6            /**< \brief (DMAC_BUSYCH) Busy Channel 6 */
#define DMAC_BUSYCH_BUSYCH6         (_U_(1) << DMAC_BUSYCH_BUSYCH6_Pos)
#define DMAC_BUSYCH_BUSYCH7_Pos     7            /**< \brief (DMAC_BUSYCH) Busy Channel 7 */
#define DMAC_BUSYCH_BUSYCH7         (_U_(1) << DMAC_BUSYCH_BUSYCH7_Pos)
#define DMAC_BUSYCH_BUSYCH8_Pos     8            /**< \brief (DMAC_BUSYCH) Busy Channel 8 */
#define DMAC_BUSYCH_BUSYCH8         (_U_(1) << DMAC_BUSYCH_BUSYCH8_Pos)
#define DMAC_BUSYCH_BUSYCH9_Pos     9            /**< \brief (DMAC_BUSYCH) Busy Channel 9 */
#define DMAC_BUSYCH_BUSYCH9         (_U_(1) << DMAC_BUSYCH_BUSYCH9_Pos)
#define DMAC_BUSYCH_BUSYCH10_Pos    10           /**< \brief (DMAC_BUSYCH) Busy Channel 10 */
#define DMAC_BUSYCH_BUSYCH10        (_U_(1) << DMAC_BUSYCH_BUSYCH10_Pos)
#define DMAC_BUSYCH_BUSYCH11_Pos    11           /**< \brief (DMAC_BUSYCH) Busy Channel 11 */
#define DMAC_BUSYCH_BUSYCH11        (_U_(1) << DMAC_BUSYCH_BUSYCH11_Pos)
#define DMAC_BUSYCH_BUSYCH12_Pos    12           /**< \brief (DMAC_BUSYCH) Busy Channel 12 */
#define DMAC_BUSYCH_BUSYCH12        (_U_(1) << DMAC_BUSYCH_BUSYCH12_Pos)
#define DMAC_BUSYCH_BUSYCH13_Pos    13           /**< \brief (DMAC_BUSYCH) Busy Channel 13 */
#define DMAC_BUSYCH_BUSYCH13        (_U_(1) << DMAC_BUSYCH_BUSYCH13_Pos)
#define DMAC_BUSYCH_BUSYCH14_Pos    14           /**< \brief (DMAC_BUSYCH) Busy Channel 14 */
#define DMAC_BUSYCH_BUSYCH14        (_U_(1) << DMAC_BUSYCH_BUSYCH14_Pos)
#define DMAC_BUSYCH_BUSYCH15_Pos    15           /**< \brief (DMAC_BUSYCH) Busy Channel 15 */
#define DMAC_BUSYCH_BUSYCH15        (_U_(1) << DMAC_BUSYCH_BUSYCH15_Pos)
#define DMAC_BUSYCH_BUSYCH16_Pos    16           /**< \brief (DMAC_BUSYCH) Busy Channel 16 */
#define DMAC_BUSYCH_BUSYCH16        (_U_(1) << DMAC_BUSYCH_BUSYCH16_Pos)
#define DMAC_BUSYCH_BUSYCH17_Pos    17           /**< \brief (DMAC_BUSYCH) Busy Channel 17 */
#define DMAC_BUSYCH_BUSYCH17        (_U_(1) << DMAC_BUSYCH_BUSYCH17_Pos)
#define DMAC_BUSYCH_BUSYCH18_Pos    18           /**< \brief (DMAC_BUSYCH) Busy Channel 18 */
#define DMAC_BUSYCH_BUSYCH18        (_U_(1) << DMAC_BUSYCH_BUSYCH18_Pos)
#define DMAC_BUSYCH_BUSYCH19_Pos    19           /**< \brief (DMAC_BUSYCH) Busy Channel 19 */
#define DMAC_BUSYCH_BUSYCH19        (_U_(1) << DMAC_BUSYCH_BUSYCH19_Pos)
#define DMAC_BUSYCH_BUSYCH20_Pos    20           /**< \brief (DMAC_BUSYCH) Busy Channel 20 */
#define DMAC_BUSYCH_BUSYCH20        (_U_(1) << DMAC_BUSYCH_BUSYCH20_Pos)
#define DMAC_BUSYCH_BUSYCH21_Pos    21           /**< \brief (DMAC_BUSYCH) Busy Channel 21 */
#define DMAC_BUSYCH_BUSYCH21        (_U_(1) << DMAC_BUSYCH_BUSYCH21_Pos)
#define DMAC_BUSYCH_BUSYCH22_Pos    22           /**< \brief (DMAC_BUSYCH) Busy Channel 22 */
#define DMAC_BUSYCH_BUSYCH22        (_U_(1) << DMAC_BUSYCH_BUSYCH22_Pos)
#define DMAC_BUSYCH_BUSYCH23_Pos    23           /**< \brief (DMAC_BUSYCH) Busy Channel 23 */
#define DMAC_BUSYCH_BUSYCH23        (_U_(1) << DMAC_BUSYCH_BUSYCH23_Pos)
#define DMAC_BUSYCH_BUSYCH24_Pos    24           /**< \brief (DMAC_BUSYCH) Busy Channel 24 */
#define DMAC_BUSYCH_BUSYCH24        (_U_(1) << DMAC_BUSYCH_BUSYCH24_Pos)
#define DMAC_BUSYCH_BUSYCH25_Pos    25           /**< \brief (DMAC_BUSYCH) Busy Channel 25 */
#define DMAC_BUSYCH_BUSYCH25        (_U_(1) << DMAC_BUSYCH_BUSYCH25_Pos)
#define DMAC_BUSYCH_BUSYCH26_Pos    26           /**< \brief (DMAC_BUSYCH) Busy Channel 26 */
#define DMAC_BUSYCH_BUSYCH26        (_U_(1) << DMAC_BUSYCH_BUSYCH26_Pos)
#define DMAC_BUSYCH_BUSYCH27_Pos    27           /**< \brief (DMAC_BUSYCH) Busy Channel 27 */
#define DMAC_BUSYCH_BUSYCH27        (_U_(1) << DMAC_BUSYCH_BUSYCH27_Pos)
#define DMAC_BUSYCH_BUSYCH28_Pos    28           /**< \brief (DMAC_BUSYCH) Busy Channel 28 */
#define DMAC_BUSYCH_BUSYCH28        (_U_(1) << DMAC_BUSYCH_BUSYCH28_Pos)
#define DMAC_BUSYCH_BUSYCH29_Pos    29           /**< \brief (DMAC_BUSYCH) Busy Channel 29 */
#define DMAC_BUSYCH_BUSYCH29        (_U_(1) << DMAC_BUSYCH_BUSYCH29_Pos)
#define DMAC_BUSYCH_BUSYCH30_Pos    30           /**< \brief (DMAC_BUSYCH) Busy Channel 30 */
#define DMAC_BUSYCH_BUSYCH30        (_U_(1) << DMAC_BUSYCH_BUSYCH30_Pos)
#define DMAC_BUSYCH_BUSYCH31_Pos    31           /**< \brief (DMAC_BUSYCH) Busy Channel 31 */
#define DMAC_BUSYCH_BUSYCH31        (_U_(1) << DMAC_BUSYCH_BUSYCH31_Pos)
#define DMAC_BUSYCH_BUSYCH_Pos      0            /**< \brief (DMAC_BUSYCH) Busy Channel x */
#define DMAC_BUSYCH_BUSYCH_Msk      (_U_(0xFFFFFFFF) << DMAC_BUSYCH_BUSYCH_Pos)
#define DMAC_BUSYCH_BUSYCH(value)   (DMAC_BUSYCH_BUSYCH_Msk & ((value) << DMAC_BUSYCH_BUSYCH_Pos))
#define DMAC_BUSYCH_MASK            _U_(0xFFFFFFFF) /**< \brief (DMAC_BUSYCH) MASK Register */

/* -------- DMAC_PENDCH : (DMAC Offset: 0x2C) (R/  32) Pending Channels -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PENDCH0:1;        /*!< bit:      0  Pending Channel 0                  */
    uint32_t PENDCH1:1;        /*!< bit:      1  Pending Channel 1                  */
    uint32_t PENDCH2:1;        /*!< bit:      2  Pending Channel 2                  */
    uint32_t PENDCH3:1;        /*!< bit:      3  Pending Channel 3                  */
    uint32_t PENDCH4:1;        /*!< bit:      4  Pending Channel 4                  */
    uint32_t PENDCH5:1;        /*!< bit:      5  Pending Channel 5                  */
    uint32_t PENDCH6:1;        /*!< bit:      6  Pending Channel 6                  */
    uint32_t PENDCH7:1;        /*!< bit:      7  Pending Channel 7                  */
    uint32_t PENDCH8:1;        /*!< bit:      8  Pending Channel 8                  */
    uint32_t PENDCH9:1;        /*!< bit:      9  Pending Channel 9                  */
    uint32_t PENDCH10:1;       /*!< bit:     10  Pending Channel 10                 */
    uint32_t PENDCH11:1;       /*!< bit:     11  Pending Channel 11                 */
    uint32_t PENDCH12:1;       /*!< bit:     12  Pending Channel 12                 */
    uint32_t PENDCH13:1;       /*!< bit:     13  Pending Channel 13                 */
    uint32_t PENDCH14:1;       /*!< bit:     14  Pending Channel 14                 */
    uint32_t PENDCH15:1;       /*!< bit:     15  Pending Channel 15                 */
    uint32_t PENDCH16:1;       /*!< bit:     16  Pending Channel 16                 */
    uint32_t PENDCH17:1;       /*!< bit:     17  Pending Channel 17                 */
    uint32_t PENDCH18:1;       /*!< bit:     18  Pending Channel 18                 */
    uint32_t PENDCH19:1;       /*!< bit:     19  Pending Channel 19                 */
    uint32_t PENDCH20:1;       /*!< bit:     20  Pending Channel 20                 */
    uint32_t PENDCH21:1;       /*!< bit:     21  Pending Channel 21                 */
    uint32_t PENDCH22:1;       /*!< bit:     22  Pending Channel 22                 */
    uint32_t PENDCH23:1;       /*!< bit:     23  Pending Channel 23                 */
    uint32_t PENDCH24:1;       /*!< bit:     24  Pending Channel 24                 */
    uint32_t PENDCH25:1;       /*!< bit:     25  Pending Channel 25                 */
    uint32_t PENDCH26:1;       /*!< bit:     26  Pending Channel 26                 */
    uint32_t PENDCH27:1;       /*!< bit:     27  Pending Channel 27                 */
    uint32_t PENDCH28:1;       /*!< bit:     28  Pending Channel 28                 */
    uint32_t PENDCH29:1;       /*!< bit:     29  Pending Channel 29                 */
    uint32_t PENDCH30:1;       /*!< bit:     30  Pending Channel 30                 */
    uint32_t PENDCH31:1;       /*!< bit:     31  Pending Channel 31                 */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t PENDCH:32;        /*!< bit:  0..31  Pending Channel x                  */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_PENDCH_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_PENDCH_OFFSET          0x2C         /**< \brief (DMAC_PENDCH offset) Pending Channels */
#define DMAC_PENDCH_RESETVALUE      _U_(0x00000000) /**< \brief (DMAC_PENDCH reset_value) Pending Channels */

#define DMAC_PENDCH_PENDCH0_Pos     0            /**< \brief (DMAC_PENDCH) Pending Channel 0 */
#define DMAC_PENDCH_PENDCH0         (_U_(1) << DMAC_PENDCH_PENDCH0_Pos)
#define DMAC_PENDCH_PENDCH1_Pos     1            /**< \brief (DMAC_PENDCH) Pending Channel 1 */
#define DMAC_PENDCH_PENDCH1         (_U_(1) << DMAC_PENDCH_PENDCH1_Pos)
#define DMAC_PENDCH_PENDCH2_Pos     2            /**< \brief (DMAC_PENDCH) Pending Channel 2 */
#define DMAC_PENDCH_PENDCH2         (_U_(1) << DMAC_PENDCH_PENDCH2_Pos)
#define DMAC_PENDCH_PENDCH3_Pos     3            /**< \brief (DMAC_PENDCH) Pending Channel 3 */
#define DMAC_PENDCH_PENDCH3         (_U_(1) << DMAC_PENDCH_PENDCH3_Pos)
#define DMAC_PENDCH_PENDCH4_Pos     4            /**< \brief (DMAC_PENDCH) Pending Channel 4 */
#define DMAC_PENDCH_PENDCH4         (_U_(1) << DMAC_PENDCH_PENDCH4_Pos)
#define DMAC_PENDCH_PENDCH5_Pos     5            /**< \brief (DMAC_PENDCH) Pending Channel 5 */
#define DMAC_PENDCH_PENDCH5         (_U_(1) << DMAC_PENDCH_PENDCH5_Pos)
#define DMAC_PENDCH_PENDCH6_Pos     6            /**< \brief (DMAC_PENDCH) Pending Channel 6 */
#define DMAC_PENDCH_PENDCH6         (_U_(1) << DMAC_PENDCH_PENDCH6_Pos)
#define DMAC_PENDCH_PENDCH7_Pos     7            /**< \brief (DMAC_PENDCH) Pending Channel 7 */
#define DMAC_PENDCH_PENDCH7         (_U_(1) << DMAC_PENDCH_PENDCH7_Pos)
#define DMAC_PENDCH_PENDCH8_Pos     8            /**< \brief (DMAC_PENDCH) Pending Channel 8 */
#define DMAC_PENDCH_PENDCH8         (_U_(1) << DMAC_PENDCH_PENDCH8_Pos)
#define DMAC_PENDCH_PENDCH9_Pos     9            /**< \brief (DMAC_PENDCH) Pending Channel 9 */
#define DMAC_PENDCH_PENDCH9         (_U_(1) << DMAC_PENDCH_PENDCH9_Pos)
#define DMAC_PENDCH_PENDCH10_Pos    10           /**< \brief (DMAC_PENDCH) Pending Channel 10 */
#define DMAC_PENDCH_PENDCH10        (_U_(1) << DMAC_PENDCH_PENDCH10_Pos)
#define DMAC_PENDCH_PENDCH11_Pos    11           /**< \brief (DMAC_PENDCH) Pending Channel 11 */
#define DMAC_PENDCH_PENDCH11        (_U_(1) << DMAC_PENDCH_PENDCH11_Pos)
#define DMAC_PENDCH_PENDCH12_Pos    12           /**< \brief (DMAC_PENDCH) Pending Channel 12 */
#define DMAC_PENDCH_PENDCH12        (_U_(1) << DMAC_PENDCH_PENDCH12_Pos)
#define DMAC_PENDCH_PENDCH13_Pos    13           /**< \brief (DMAC_PENDCH) Pending Channel 13 */
#define DMAC_PENDCH_PENDCH13        (_U_(1) << DMAC_PENDCH_PENDCH13_Pos)
#define DMAC_PENDCH_PENDCH14_Pos    14           /**< \brief (DMAC_PENDCH) Pending Channel 14 */
#define DMAC_PENDCH_PENDCH14        (_U_(1) << DMAC_PENDCH_PENDCH14_Pos)
#define DMAC_PENDCH_PENDCH15_Pos    15           /**< \brief (DMAC_PENDCH) Pending Channel 15 */
#define DMAC_PENDCH_PENDCH15        (_U_(1) << DMAC_PENDCH_PENDCH15_Pos)
#define DMAC_PENDCH_PENDCH16_Pos    16           /**< \brief (DMAC_PENDCH) Pending Channel 16 */
#define DMAC_PENDCH_PENDCH16        (_U_(1) << DMAC_PENDCH_PENDCH16_Pos)
#define DMAC_PENDCH_PENDCH17_Pos    17           /**< \brief (DMAC_PENDCH) Pending Channel 17 */
#define DMAC_PENDCH_PENDCH17        (_U_(1) << DMAC_PENDCH_PENDCH17_Pos)
#define DMAC_PENDCH_PENDCH18_Pos    18           /**< \brief (DMAC_PENDCH) Pending Channel 18 */
#define DMAC_PENDCH_PENDCH18        (_U_(1) << DMAC_PENDCH_PENDCH18_Pos)
#define DMAC_PENDCH_PENDCH19_Pos    19           /**< \brief (DMAC_PENDCH) Pending Channel 19 */
#define DMAC_PENDCH_PENDCH19        (_U_(1) << DMAC_PENDCH_PENDCH19_Pos)
#define DMAC_PENDCH_PENDCH20_Pos    20           /**< \brief (DMAC_PENDCH) Pending Channel 20 */
#define DMAC_PENDCH_PENDCH20        (_U_(1) << DMAC_PENDCH_PENDCH20_Pos)
#define DMAC_PENDCH_PENDCH21_Pos    21           /**< \brief (DMAC_PENDCH) Pending Channel 21 */
#define DMAC_PENDCH_PENDCH21        (_U_(1) << DMAC_PENDCH_PENDCH21_Pos)
#define DMAC_PENDCH_PENDCH22_Pos    22           /**< \brief (DMAC_PENDCH) Pending Channel 22 */
#define DMAC_PENDCH_PENDCH22        (_U_(1) << DMAC_PENDCH_PENDCH22_Pos)
#define DMAC_PENDCH_PENDCH23_Pos    23           /**< \brief (DMAC_PENDCH) Pending Channel 23 */
#define DMAC_PENDCH_PENDCH23        (_U_(1) << DMAC_PENDCH_PENDCH23_Pos)
#define DMAC_PENDCH_PENDCH24_Pos    24           /**< \brief (DMAC_PENDCH) Pending Channel 24 */
#define DMAC_PENDCH_PENDCH24        (_U_(1) << DMAC_PENDCH_PENDCH24_Pos)
#define DMAC_PENDCH_PENDCH25_Pos    25           /**< \brief (DMAC_PENDCH) Pending Channel 25 */
#define DMAC_PENDCH_PENDCH25        (_U_(1) << DMAC_PENDCH_PENDCH25_Pos)
#define DMAC_PENDCH_PENDCH26_Pos    26           /**< \brief (DMAC_PENDCH) Pending Channel 26 */
#define DMAC_PENDCH_PENDCH26        (_U_(1) << DMAC_PENDCH_PENDCH26_Pos)
#define DMAC_PENDCH_PENDCH27_Pos    27           /**< \brief (DMAC_PENDCH) Pending Channel 27 */
#define DMAC_PENDCH_PENDCH27        (_U_(1) << DMAC_PENDCH_PENDCH27_Pos)
#define DMAC_PENDCH_PENDCH28_Pos    28           /**< \brief (DMAC_PENDCH) Pending Channel 28 */
#define DMAC_PENDCH_PENDCH28        (_U_(1) << DMAC_PENDCH_PENDCH28_Pos)
#define DMAC_PENDCH_PENDCH29_Pos    29           /**< \brief (DMAC_PENDCH) Pending Channel 29 */
#define DMAC_PENDCH_PENDCH29        (_U_(1) << DMAC_PENDCH_PENDCH29_Pos)
#define DMAC_PENDCH_PENDCH30_Pos    30           /**< \brief (DMAC_PENDCH) Pending Channel 30 */
#define DMAC_PENDCH_PENDCH30        (_U_(1) << DMAC_PENDCH_PENDCH30_Pos)
#define DMAC_PENDCH_PENDCH31_Pos    31           /**< \brief (DMAC_PENDCH) Pending Channel 31 */
#define DMAC_PENDCH_PENDCH31        (_U_(1) << DMAC_PENDCH_PENDCH31_Pos)
#define DMAC_PENDCH_PENDCH_Pos      0            /**< \brief (DMAC_PENDCH) Pending Channel x */
#define DMAC_PENDCH_PENDCH_Msk      (_U_(0xFFFFFFFF) << DMAC_PENDCH_PENDCH_Pos)
#define DMAC_PENDCH_PENDCH(value)   (DMAC_PENDCH_PENDCH_Msk & ((value) << DMAC_PENDCH_PENDCH_Pos))
#define DMAC_PENDCH_MASK            _U_(0xFFFFFFFF) /**< \brief (DMAC_PENDCH) MASK Register */

/* -------- DMAC_ACTIVE : (DMAC Offset: 0x30) (R/  32) Active Channel and Levels -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t LVLEX0:1;         /*!< bit:      0  Level 0 Channel Trigger Request Executing */
    uint32_t LVLEX1:1;         /*!< bit:      1  Level 1 Channel Trigger Request Executing */
    uint32_t LVLEX2:1;         /*!< bit:      2  Level 2 Channel Trigger Request Executing */
    uint32_t LVLEX3:1;         /*!< bit:      3  Level 3 Channel Trigger Request Executing */
    uint32_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint32_t ID:5;             /*!< bit:  8..12  Active Channel ID                  */
    uint32_t :2;               /*!< bit: 13..14  Reserved                           */
    uint32_t ABUSY:1;          /*!< bit:     15  Active Channel Busy                */
    uint32_t BTCNT:16;         /*!< bit: 16..31  Active Channel Block Transfer Count */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t LVLEX:4;          /*!< bit:  0.. 3  Level x Channel Trigger Request Executing */
    uint32_t :28;              /*!< bit:  4..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_ACTIVE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_ACTIVE_OFFSET          0x30         /**< \brief (DMAC_ACTIVE offset) Active Channel and Levels */
#define DMAC_ACTIVE_RESETVALUE      _U_(0x00000000) /**< \brief (DMAC_ACTIVE reset_value) Active Channel and Levels */

#define DMAC_ACTIVE_LVLEX0_Pos      0            /**< \brief (DMAC_ACTIVE) Level 0 Channel Trigger Request Executing */
#define DMAC_ACTIVE_LVLEX0          (_U_(1) << DMAC_ACTIVE_LVLEX0_Pos)
#define DMAC_ACTIVE_LVLEX1_Pos      1            /**< \brief (DMAC_ACTIVE) Level 1 Channel Trigger Request Executing */
#define DMAC_ACTIVE_LVLEX1          (_U_(1) << DMAC_ACTIVE_LVLEX1_Pos)
#define DMAC_ACTIVE_LVLEX2_Pos      2            /**< \brief (DMAC_ACTIVE) Level 2 Channel Trigger Request Executing */
#define DMAC_ACTIVE_LVLEX2          (_U_(1) << DMAC_ACTIVE_LVLEX2_Pos)
#define DMAC_ACTIVE_LVLEX3_Pos      3            /**< \brief (DMAC_ACTIVE) Level 3 Channel Trigger Request Executing */
#define DMAC_ACTIVE_LVLEX3          (_U_(1) << DMAC_ACTIVE_LVLEX3_Pos)
#define DMAC_ACTIVE_LVLEX_Pos       0            /**< \brief (DMAC_ACTIVE) Level x Channel Trigger Request Executing */
#define DMAC_ACTIVE_LVLEX_Msk       (_U_(0xF) << DMAC_ACTIVE_LVLEX_Pos)
#define DMAC_ACTIVE_LVLEX(value)    (DMAC_ACTIVE_LVLEX_Msk & ((value) << DMAC_ACTIVE_LVLEX_Pos))
#define DMAC_ACTIVE_ID_Pos          8            /**< \brief (DMAC_ACTIVE) Active Channel ID */
#define DMAC_ACTIVE_ID_Msk          (_U_(0x1F) << DMAC_ACTIVE_ID_Pos)
#define DMAC_ACTIVE_ID(value)       (DMAC_ACTIVE_ID_Msk & ((value) << DMAC_ACTIVE_ID_Pos))
#define DMAC_ACTIVE_ABUSY_Pos       15           /**< \brief (DMAC_ACTIVE) Active Channel Busy */
#define DMAC_ACTIVE_ABUSY           (_U_(0x1) << DMAC_ACTIVE_ABUSY_Pos)
#define DMAC_ACTIVE_BTCNT_Pos       16           /**< \brief (DMAC_ACTIVE) Active Channel Block Transfer Count */
#define DMAC_ACTIVE_BTCNT_Msk       (_U_(0xFFFF) << DMAC_ACTIVE_BTCNT_Pos)
#define DMAC_ACTIVE_BTCNT(value)    (DMAC_ACTIVE_BTCNT_Msk & ((value) << DMAC_ACTIVE_BTCNT_Pos))
#define DMAC_ACTIVE_MASK            _U_(0xFFFF9F0F) /**< \brief (DMAC_ACTIVE) MASK Register */

/* -------- DMAC_BASEADDR : (DMAC Offset: 0x34) (R/W 32) Descriptor Memory Section Base Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BASEADDR:32;      /*!< bit:  0..31  Descriptor Memory Base Address     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_BASEADDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_BASEADDR_OFFSET        0x34         /**< \brief (DMAC_BASEADDR offset) Descriptor Memory Section Base Address */
#define DMAC_BASEADDR_RESETVALUE    _U_(0x00000000) /**< \brief (DMAC_BASEADDR reset_value) Descriptor Memory Section Base Address */

#define DMAC_BASEADDR_BASEADDR_Pos  0            /**< \brief (DMAC_BASEADDR) Descriptor Memory Base Address */
#define DMAC_BASEADDR_BASEADDR_Msk  (_U_(0xFFFFFFFF) << DMAC_BASEADDR_BASEADDR_Pos)
#define DMAC_BASEADDR_BASEADDR(value) (DMAC_BASEADDR_BASEADDR_Msk & ((value) << DMAC_BASEADDR_BASEADDR_Pos))
#define DMAC_BASEADDR_MASK          _U_(0xFFFFFFFF) /**< \brief (DMAC_BASEADDR) MASK Register */

/* -------- DMAC_WRBADDR : (DMAC Offset: 0x38) (R/W 32) Write-Back Memory Section Base Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t WRBADDR:32;       /*!< bit:  0..31  Write-Back Memory Base Address     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_WRBADDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_WRBADDR_OFFSET         0x38         /**< \brief (DMAC_WRBADDR offset) Write-Back Memory Section Base Address */
#define DMAC_WRBADDR_RESETVALUE     _U_(0x00000000) /**< \brief (DMAC_WRBADDR reset_value) Write-Back Memory Section Base Address */

#define DMAC_WRBADDR_WRBADDR_Pos    0            /**< \brief (DMAC_WRBADDR) Write-Back Memory Base Address */
#define DMAC_WRBADDR_WRBADDR_Msk    (_U_(0xFFFFFFFF) << DMAC_WRBADDR_WRBADDR_Pos)
#define DMAC_WRBADDR_WRBADDR(value) (DMAC_WRBADDR_WRBADDR_Msk & ((value) << DMAC_WRBADDR_WRBADDR_Pos))
#define DMAC_WRBADDR_MASK           _U_(0xFFFFFFFF) /**< \brief (DMAC_WRBADDR) MASK Register */

/* -------- DMAC_CHCTRLA : (DMAC Offset: 0x40) (R/W 32) CHANNEL Channel n Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Channel Software Reset             */
    uint32_t ENABLE:1;         /*!< bit:      1  Channel Enable                     */
    uint32_t :4;               /*!< bit:  2.. 5  Reserved                           */
    uint32_t RUNSTDBY:1;       /*!< bit:      6  Channel Run in Standby             */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t TRIGSRC:7;        /*!< bit:  8..14  Trigger Source                     */
    uint32_t :5;               /*!< bit: 15..19  Reserved                           */
    uint32_t TRIGACT:2;        /*!< bit: 20..21  Trigger Action                     */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t BURSTLEN:4;       /*!< bit: 24..27  Burst Length                       */
    uint32_t THRESHOLD:2;      /*!< bit: 28..29  FIFO Threshold                     */
    uint32_t :2;               /*!< bit: 30..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_CHCTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CHCTRLA_OFFSET         0x40         /**< \brief (DMAC_CHCTRLA offset) Channel n Control A */
#define DMAC_CHCTRLA_RESETVALUE     _U_(0x00000000) /**< \brief (DMAC_CHCTRLA reset_value) Channel n Control A */

#define DMAC_CHCTRLA_SWRST_Pos      0            /**< \brief (DMAC_CHCTRLA) Channel Software Reset */
#define DMAC_CHCTRLA_SWRST          (_U_(0x1) << DMAC_CHCTRLA_SWRST_Pos)
#define DMAC_CHCTRLA_ENABLE_Pos     1            /**< \brief (DMAC_CHCTRLA) Channel Enable */
#define DMAC_CHCTRLA_ENABLE         (_U_(0x1) << DMAC_CHCTRLA_ENABLE_Pos)
#define DMAC_CHCTRLA_RUNSTDBY_Pos   6            /**< \brief (DMAC_CHCTRLA) Channel Run in Standby */
#define DMAC_CHCTRLA_RUNSTDBY       (_U_(0x1) << DMAC_CHCTRLA_RUNSTDBY_Pos)
#define DMAC_CHCTRLA_TRIGSRC_Pos    8            /**< \brief (DMAC_CHCTRLA) Trigger Source */
#define DMAC_CHCTRLA_TRIGSRC_Msk    (_U_(0x7F) << DMAC_CHCTRLA_TRIGSRC_Pos)
#define DMAC_CHCTRLA_TRIGSRC(value) (DMAC_CHCTRLA_TRIGSRC_Msk & ((value) << DMAC_CHCTRLA_TRIGSRC_Pos))
#define   DMAC_CHCTRLA_TRIGSRC_DISABLE_Val _U_(0x0)   /**< \brief (DMAC_CHCTRLA) Only software/event triggers */
#define DMAC_CHCTRLA_TRIGSRC_DISABLE (DMAC_CHCTRLA_TRIGSRC_DISABLE_Val << DMAC_CHCTRLA_TRIGSRC_Pos)
#define DMAC_CHCTRLA_TRIGACT_Pos    20           /**< \brief (DMAC_CHCTRLA) Trigger Action */
#define DMAC_CHCTRLA_TRIGACT_Msk    (_U_(0x3) << DMAC_CHCTRLA_TRIGACT_Pos)
#define DMAC_CHCTRLA_TRIGACT(value) (DMAC_CHCTRLA_TRIGACT_Msk & ((value) << DMAC_CHCTRLA_TRIGACT_Pos))
#define   DMAC_CHCTRLA_TRIGACT_BLOCK_Val  _U_(0x0)   /**< \brief (DMAC_CHCTRLA) One trigger required for each block transfer */
#define   DMAC_CHCTRLA_TRIGACT_BURST_Val  _U_(0x2)   /**< \brief (DMAC_CHCTRLA) One trigger required for each burst transfer */
#define   DMAC_CHCTRLA_TRIGACT_TRANSACTION_Val _U_(0x3)   /**< \brief (DMAC_CHCTRLA) One trigger required for each transaction */
#define DMAC_CHCTRLA_TRIGACT_BLOCK  (DMAC_CHCTRLA_TRIGACT_BLOCK_Val << DMAC_CHCTRLA_TRIGACT_Pos)
#define DMAC_CHCTRLA_TRIGACT_BURST  (DMAC_CHCTRLA_TRIGACT_BURST_Val << DMAC_CHCTRLA_TRIGACT_Pos)
#define DMAC_CHCTRLA_TRIGACT_TRANSACTION (DMAC_CHCTRLA_TRIGACT_TRANSACTION_Val << DMAC_CHCTRLA_TRIGACT_Pos)
#define DMAC_CHCTRLA_BURSTLEN_Pos   24           /**< \brief (DMAC_CHCTRLA) Burst Length */
#define DMAC_CHCTRLA_BURSTLEN_Msk   (_U_(0xF) << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_BURSTLEN(value) (DMAC_CHCTRLA_BURSTLEN_Msk & ((value) << DMAC_CHCTRLA_BURSTLEN_Pos))
#define   DMAC_CHCTRLA_BURSTLEN_SINGLE_Val _U_(0x0)   /**< \brief (DMAC_CHCTRLA) Single-beat burst length */
#define   DMAC_CHCTRLA_BURSTLEN_2BEAT_Val _U_(0x1)   /**< \brief (DMAC_CHCTRLA) 2-beats burst length */
#define   DMAC_CHCTRLA_BURSTLEN_3BEAT_Val _U_(0x2)   /**< \brief (DMAC_CHCTRLA) 3-beats burst length */
#define   DMAC_CHCTRLA_BURSTLEN_4BEAT_Val _U_(0x3)   /**< \brief (DMAC_CHCTRLA) 4-beats burst length */
#define   DMAC_CHCTRLA_BURSTLEN_5BEAT_Val _U_(0x4)   /**< \brief (DMAC_CHCTRLA) 5-beats burst length */
#define   DMAC_CHCTRLA_BURSTLEN_6BEAT_Val _U_(0x5)   /**< \brief (DMAC_CHCTRLA) 6-beats burst length */
#define   DMAC_CHCTRLA_BURSTLEN_7BEAT_Val _U_(0x6)   /**< \brief (DMAC_CHCTRLA) 7-beats burst length */
#define   DMAC_CHCTRLA_BURSTLEN_8BEAT_Val _U_(0x7)   /**< \brief (DMAC_CHCTRLA) 8-beats burst length */
#define   DMAC_CHCTRLA_BURSTLEN_9BEAT_Val _U_(0x8)   /**< \brief (DMAC_CHCTRLA) 9-beats burst length */
#define   DMAC_CHCTRLA_BURSTLEN_10BEAT_Val _U_(0x9)   /**< \brief (DMAC_CHCTRLA) 10-beats burst length */
#define   DMAC_CHCTRLA_BURSTLEN_11BEAT_Val _U_(0xA)   /**< \brief (DMAC_CHCTRLA) 11-beats burst length */
#define   DMAC_CHCTRLA_BURSTLEN_12BEAT_Val _U_(0xB)   /**< \brief (DMAC_CHCTRLA) 12-beats burst length */
#define   DMAC_CHCTRLA_BURSTLEN_13BEAT_Val _U_(0xC)   /**< \brief (DMAC_CHCTRLA) 13-beats burst length */
#define   DMAC_CHCTRLA_BURSTLEN_14BEAT_Val _U_(0xD)   /**< \brief (DMAC_CHCTRLA) 14-beats burst length */
#define   DMAC_CHCTRLA_BURSTLEN_15BEAT_Val _U_(0xE)   /**< \brief (DMAC_CHCTRLA) 15-beats burst length */
#define   DMAC_CHCTRLA_BURSTLEN_16BEAT_Val _U_(0xF)   /**< \brief (DMAC_CHCTRLA) 16-beats burst length */
#define DMAC_CHCTRLA_BURSTLEN_SINGLE (DMAC_CHCTRLA_BURSTLEN_SINGLE_Val << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_BURSTLEN_2BEAT (DMAC_CHCTRLA_BURSTLEN_2BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_BURSTLEN_3BEAT (DMAC_CHCTRLA_BURSTLEN_3BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_BURSTLEN_4BEAT (DMAC_CHCTRLA_BURSTLEN_4BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_BURSTLEN_5BEAT (DMAC_CHCTRLA_BURSTLEN_5BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_BURSTLEN_6BEAT (DMAC_CHCTRLA_BURSTLEN_6BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_BURSTLEN_7BEAT (DMAC_CHCTRLA_BURSTLEN_7BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_BURSTLEN_8BEAT (DMAC_CHCTRLA_BURSTLEN_8BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_BURSTLEN_9BEAT (DMAC_CHCTRLA_BURSTLEN_9BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_BURSTLEN_10BEAT (DMAC_CHCTRLA_BURSTLEN_10BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_BURSTLEN_11BEAT (DMAC_CHCTRLA_BURSTLEN_11BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_BURSTLEN_12BEAT (DMAC_CHCTRLA_BURSTLEN_12BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_BURSTLEN_13BEAT (DMAC_CHCTRLA_BURSTLEN_13BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_BURSTLEN_14BEAT (DMAC_CHCTRLA_BURSTLEN_14BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_BURSTLEN_15BEAT (DMAC_CHCTRLA_BURSTLEN_15BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_BURSTLEN_16BEAT (DMAC_CHCTRLA_BURSTLEN_16BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos)
#define DMAC_CHCTRLA_THRESHOLD_Pos  28           /**< \brief (DMAC_CHCTRLA) FIFO Threshold */
#define DMAC_CHCTRLA_THRESHOLD_Msk  (_U_(0x3) << DMAC_CHCTRLA_THRESHOLD_Pos)
#define DMAC_CHCTRLA_THRESHOLD(value) (DMAC_CHCTRLA_THRESHOLD_Msk & ((value) << DMAC_CHCTRLA_THRESHOLD_Pos))
#define   DMAC_CHCTRLA_THRESHOLD_1BEAT_Val _U_(0x0)   /**< \brief (DMAC_CHCTRLA) Destination write starts after each beat source address read */
#define   DMAC_CHCTRLA_THRESHOLD_2BEATS_Val _U_(0x1)   /**< \brief (DMAC_CHCTRLA) Destination write starts after 2-beats source address read */
#define   DMAC_CHCTRLA_THRESHOLD_4BEATS_Val _U_(0x2)   /**< \brief (DMAC_CHCTRLA) Destination write starts after 4-beats source address read */
#define   DMAC_CHCTRLA_THRESHOLD_8BEATS_Val _U_(0x3)   /**< \brief (DMAC_CHCTRLA) Destination write starts after 8-beats source address read */
#define DMAC_CHCTRLA_THRESHOLD_1BEAT (DMAC_CHCTRLA_THRESHOLD_1BEAT_Val << DMAC_CHCTRLA_THRESHOLD_Pos)
#define DMAC_CHCTRLA_THRESHOLD_2BEATS (DMAC_CHCTRLA_THRESHOLD_2BEATS_Val << DMAC_CHCTRLA_THRESHOLD_Pos)
#define DMAC_CHCTRLA_THRESHOLD_4BEATS (DMAC_CHCTRLA_THRESHOLD_4BEATS_Val << DMAC_CHCTRLA_THRESHOLD_Pos)
#define DMAC_CHCTRLA_THRESHOLD_8BEATS (DMAC_CHCTRLA_THRESHOLD_8BEATS_Val << DMAC_CHCTRLA_THRESHOLD_Pos)
#define DMAC_CHCTRLA_MASK           _U_(0x3F307F43) /**< \brief (DMAC_CHCTRLA) MASK Register */

/* -------- DMAC_CHCTRLB : (DMAC Offset: 0x44) (R/W  8) CHANNEL Channel n Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CMD:2;            /*!< bit:  0.. 1  Software Command                   */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_CHCTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CHCTRLB_OFFSET         0x44         /**< \brief (DMAC_CHCTRLB offset) Channel n Control B */
#define DMAC_CHCTRLB_RESETVALUE     _U_(0x00)    /**< \brief (DMAC_CHCTRLB reset_value) Channel n Control B */

#define DMAC_CHCTRLB_CMD_Pos        0            /**< \brief (DMAC_CHCTRLB) Software Command */
#define DMAC_CHCTRLB_CMD_Msk        (_U_(0x3) << DMAC_CHCTRLB_CMD_Pos)
#define DMAC_CHCTRLB_CMD(value)     (DMAC_CHCTRLB_CMD_Msk & ((value) << DMAC_CHCTRLB_CMD_Pos))
#define   DMAC_CHCTRLB_CMD_NOACT_Val      _U_(0x0)   /**< \brief (DMAC_CHCTRLB) No action */
#define   DMAC_CHCTRLB_CMD_SUSPEND_Val    _U_(0x1)   /**< \brief (DMAC_CHCTRLB) Channel suspend operation */
#define   DMAC_CHCTRLB_CMD_RESUME_Val     _U_(0x2)   /**< \brief (DMAC_CHCTRLB) Channel resume operation */
#define DMAC_CHCTRLB_CMD_NOACT      (DMAC_CHCTRLB_CMD_NOACT_Val    << DMAC_CHCTRLB_CMD_Pos)
#define DMAC_CHCTRLB_CMD_SUSPEND    (DMAC_CHCTRLB_CMD_SUSPEND_Val  << DMAC_CHCTRLB_CMD_Pos)
#define DMAC_CHCTRLB_CMD_RESUME     (DMAC_CHCTRLB_CMD_RESUME_Val   << DMAC_CHCTRLB_CMD_Pos)
#define DMAC_CHCTRLB_MASK           _U_(0x03)    /**< \brief (DMAC_CHCTRLB) MASK Register */

/* -------- DMAC_CHPRILVL : (DMAC Offset: 0x45) (R/W  8) CHANNEL Channel n Priority Level -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  PRILVL:2;         /*!< bit:  0.. 1  Channel Priority Level             */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_CHPRILVL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CHPRILVL_OFFSET        0x45         /**< \brief (DMAC_CHPRILVL offset) Channel n Priority Level */
#define DMAC_CHPRILVL_RESETVALUE    _U_(0x00)    /**< \brief (DMAC_CHPRILVL reset_value) Channel n Priority Level */

#define DMAC_CHPRILVL_PRILVL_Pos    0            /**< \brief (DMAC_CHPRILVL) Channel Priority Level */
#define DMAC_CHPRILVL_PRILVL_Msk    (_U_(0x3) << DMAC_CHPRILVL_PRILVL_Pos)
#define DMAC_CHPRILVL_PRILVL(value) (DMAC_CHPRILVL_PRILVL_Msk & ((value) << DMAC_CHPRILVL_PRILVL_Pos))
#define   DMAC_CHPRILVL_PRILVL_LVL0_Val   _U_(0x0)   /**< \brief (DMAC_CHPRILVL) Channel Priority Level 0 (Lowest Level) */
#define   DMAC_CHPRILVL_PRILVL_LVL1_Val   _U_(0x1)   /**< \brief (DMAC_CHPRILVL) Channel Priority Level 1 */
#define   DMAC_CHPRILVL_PRILVL_LVL2_Val   _U_(0x2)   /**< \brief (DMAC_CHPRILVL) Channel Priority Level 2 */
#define   DMAC_CHPRILVL_PRILVL_LVL3_Val   _U_(0x3)   /**< \brief (DMAC_CHPRILVL) Channel Priority Level 3 */
#define   DMAC_CHPRILVL_PRILVL_LVL4_Val   _U_(0x4)   /**< \brief (DMAC_CHPRILVL) Channel Priority Level 4 */
#define   DMAC_CHPRILVL_PRILVL_LVL5_Val   _U_(0x5)   /**< \brief (DMAC_CHPRILVL) Channel Priority Level 5 */
#define   DMAC_CHPRILVL_PRILVL_LVL6_Val   _U_(0x6)   /**< \brief (DMAC_CHPRILVL) Channel Priority Level 6 */
#define   DMAC_CHPRILVL_PRILVL_LVL7_Val   _U_(0x7)   /**< \brief (DMAC_CHPRILVL) Channel Priority Level 7 (Highest Level) */
#define DMAC_CHPRILVL_PRILVL_LVL0   (DMAC_CHPRILVL_PRILVL_LVL0_Val << DMAC_CHPRILVL_PRILVL_Pos)
#define DMAC_CHPRILVL_PRILVL_LVL1   (DMAC_CHPRILVL_PRILVL_LVL1_Val << DMAC_CHPRILVL_PRILVL_Pos)
#define DMAC_CHPRILVL_PRILVL_LVL2   (DMAC_CHPRILVL_PRILVL_LVL2_Val << DMAC_CHPRILVL_PRILVL_Pos)
#define DMAC_CHPRILVL_PRILVL_LVL3   (DMAC_CHPRILVL_PRILVL_LVL3_Val << DMAC_CHPRILVL_PRILVL_Pos)
#define DMAC_CHPRILVL_PRILVL_LVL4   (DMAC_CHPRILVL_PRILVL_LVL4_Val << DMAC_CHPRILVL_PRILVL_Pos)
#define DMAC_CHPRILVL_PRILVL_LVL5   (DMAC_CHPRILVL_PRILVL_LVL5_Val << DMAC_CHPRILVL_PRILVL_Pos)
#define DMAC_CHPRILVL_PRILVL_LVL6   (DMAC_CHPRILVL_PRILVL_LVL6_Val << DMAC_CHPRILVL_PRILVL_Pos)
#define DMAC_CHPRILVL_PRILVL_LVL7   (DMAC_CHPRILVL_PRILVL_LVL7_Val << DMAC_CHPRILVL_PRILVL_Pos)
#define DMAC_CHPRILVL_MASK          _U_(0x03)    /**< \brief (DMAC_CHPRILVL) MASK Register */

/* -------- DMAC_CHEVCTRL : (DMAC Offset: 0x46) (R/W  8) CHANNEL Channel n Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  EVACT:3;          /*!< bit:  0.. 2  Channel Event Input Action         */
    uint8_t  :1;               /*!< bit:      3  Reserved                           */
    uint8_t  EVOMODE:2;        /*!< bit:  4.. 5  Channel Event Output Mode          */
    uint8_t  EVIE:1;           /*!< bit:      6  Channel Event Input Enable         */
    uint8_t  EVOE:1;           /*!< bit:      7  Channel Event Output Enable        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_CHEVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CHEVCTRL_OFFSET        0x46         /**< \brief (DMAC_CHEVCTRL offset) Channel n Event Control */
#define DMAC_CHEVCTRL_RESETVALUE    _U_(0x00)    /**< \brief (DMAC_CHEVCTRL reset_value) Channel n Event Control */

#define DMAC_CHEVCTRL_EVACT_Pos     0            /**< \brief (DMAC_CHEVCTRL) Channel Event Input Action */
#define DMAC_CHEVCTRL_EVACT_Msk     (_U_(0x7) << DMAC_CHEVCTRL_EVACT_Pos)
#define DMAC_CHEVCTRL_EVACT(value)  (DMAC_CHEVCTRL_EVACT_Msk & ((value) << DMAC_CHEVCTRL_EVACT_Pos))
#define   DMAC_CHEVCTRL_EVACT_NOACT_Val   _U_(0x0)   /**< \brief (DMAC_CHEVCTRL) No action */
#define   DMAC_CHEVCTRL_EVACT_TRIG_Val    _U_(0x1)   /**< \brief (DMAC_CHEVCTRL) Transfer and periodic transfer trigger */
#define   DMAC_CHEVCTRL_EVACT_CTRIG_Val   _U_(0x2)   /**< \brief (DMAC_CHEVCTRL) Conditional transfer trigger */
#define   DMAC_CHEVCTRL_EVACT_CBLOCK_Val  _U_(0x3)   /**< \brief (DMAC_CHEVCTRL) Conditional block transfer */
#define   DMAC_CHEVCTRL_EVACT_SUSPEND_Val _U_(0x4)   /**< \brief (DMAC_CHEVCTRL) Channel suspend operation */
#define   DMAC_CHEVCTRL_EVACT_RESUME_Val  _U_(0x5)   /**< \brief (DMAC_CHEVCTRL) Channel resume operation */
#define   DMAC_CHEVCTRL_EVACT_SSKIP_Val   _U_(0x6)   /**< \brief (DMAC_CHEVCTRL) Skip next block suspend action */
#define   DMAC_CHEVCTRL_EVACT_INCPRI_Val  _U_(0x7)   /**< \brief (DMAC_CHEVCTRL) Increase priority */
#define DMAC_CHEVCTRL_EVACT_NOACT   (DMAC_CHEVCTRL_EVACT_NOACT_Val << DMAC_CHEVCTRL_EVACT_Pos)
#define DMAC_CHEVCTRL_EVACT_TRIG    (DMAC_CHEVCTRL_EVACT_TRIG_Val  << DMAC_CHEVCTRL_EVACT_Pos)
#define DMAC_CHEVCTRL_EVACT_CTRIG   (DMAC_CHEVCTRL_EVACT_CTRIG_Val << DMAC_CHEVCTRL_EVACT_Pos)
#define DMAC_CHEVCTRL_EVACT_CBLOCK  (DMAC_CHEVCTRL_EVACT_CBLOCK_Val << DMAC_CHEVCTRL_EVACT_Pos)
#define DMAC_CHEVCTRL_EVACT_SUSPEND (DMAC_CHEVCTRL_EVACT_SUSPEND_Val << DMAC_CHEVCTRL_EVACT_Pos)
#define DMAC_CHEVCTRL_EVACT_RESUME  (DMAC_CHEVCTRL_EVACT_RESUME_Val << DMAC_CHEVCTRL_EVACT_Pos)
#define DMAC_CHEVCTRL_EVACT_SSKIP   (DMAC_CHEVCTRL_EVACT_SSKIP_Val << DMAC_CHEVCTRL_EVACT_Pos)
#define DMAC_CHEVCTRL_EVACT_INCPRI  (DMAC_CHEVCTRL_EVACT_INCPRI_Val << DMAC_CHEVCTRL_EVACT_Pos)
#define DMAC_CHEVCTRL_EVOMODE_Pos   4            /**< \brief (DMAC_CHEVCTRL) Channel Event Output Mode */
#define DMAC_CHEVCTRL_EVOMODE_Msk   (_U_(0x3) << DMAC_CHEVCTRL_EVOMODE_Pos)
#define DMAC_CHEVCTRL_EVOMODE(value) (DMAC_CHEVCTRL_EVOMODE_Msk & ((value) << DMAC_CHEVCTRL_EVOMODE_Pos))
#define   DMAC_CHEVCTRL_EVOMODE_DEFAULT_Val _U_(0x0)   /**< \brief (DMAC_CHEVCTRL) Block event output selection. Refer to BTCTRL.EVOSEL for available selections. */
#define   DMAC_CHEVCTRL_EVOMODE_TRIGACT_Val _U_(0x1)   /**< \brief (DMAC_CHEVCTRL) Ongoing trigger action */
#define DMAC_CHEVCTRL_EVOMODE_DEFAULT (DMAC_CHEVCTRL_EVOMODE_DEFAULT_Val << DMAC_CHEVCTRL_EVOMODE_Pos)
#define DMAC_CHEVCTRL_EVOMODE_TRIGACT (DMAC_CHEVCTRL_EVOMODE_TRIGACT_Val << DMAC_CHEVCTRL_EVOMODE_Pos)
#define DMAC_CHEVCTRL_EVIE_Pos      6            /**< \brief (DMAC_CHEVCTRL) Channel Event Input Enable */
#define DMAC_CHEVCTRL_EVIE          (_U_(0x1) << DMAC_CHEVCTRL_EVIE_Pos)
#define DMAC_CHEVCTRL_EVOE_Pos      7            /**< \brief (DMAC_CHEVCTRL) Channel Event Output Enable */
#define DMAC_CHEVCTRL_EVOE          (_U_(0x1) << DMAC_CHEVCTRL_EVOE_Pos)
#define DMAC_CHEVCTRL_MASK          _U_(0xF7)    /**< \brief (DMAC_CHEVCTRL) MASK Register */

/* -------- DMAC_CHINTENCLR : (DMAC Offset: 0x4C) (R/W  8) CHANNEL Channel n Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  TERR:1;           /*!< bit:      0  Channel Transfer Error Interrupt Enable */
    uint8_t  TCMPL:1;          /*!< bit:      1  Channel Transfer Complete Interrupt Enable */
    uint8_t  SUSP:1;           /*!< bit:      2  Channel Suspend Interrupt Enable   */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_CHINTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CHINTENCLR_OFFSET      0x4C         /**< \brief (DMAC_CHINTENCLR offset) Channel n Interrupt Enable Clear */
#define DMAC_CHINTENCLR_RESETVALUE  _U_(0x00)    /**< \brief (DMAC_CHINTENCLR reset_value) Channel n Interrupt Enable Clear */

#define DMAC_CHINTENCLR_TERR_Pos    0            /**< \brief (DMAC_CHINTENCLR) Channel Transfer Error Interrupt Enable */
#define DMAC_CHINTENCLR_TERR        (_U_(0x1) << DMAC_CHINTENCLR_TERR_Pos)
#define DMAC_CHINTENCLR_TCMPL_Pos   1            /**< \brief (DMAC_CHINTENCLR) Channel Transfer Complete Interrupt Enable */
#define DMAC_CHINTENCLR_TCMPL       (_U_(0x1) << DMAC_CHINTENCLR_TCMPL_Pos)
#define DMAC_CHINTENCLR_SUSP_Pos    2            /**< \brief (DMAC_CHINTENCLR) Channel Suspend Interrupt Enable */
#define DMAC_CHINTENCLR_SUSP        (_U_(0x1) << DMAC_CHINTENCLR_SUSP_Pos)
#define DMAC_CHINTENCLR_MASK        _U_(0x07)    /**< \brief (DMAC_CHINTENCLR) MASK Register */

/* -------- DMAC_CHINTENSET : (DMAC Offset: 0x4D) (R/W  8) CHANNEL Channel n Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  TERR:1;           /*!< bit:      0  Channel Transfer Error Interrupt Enable */
    uint8_t  TCMPL:1;          /*!< bit:      1  Channel Transfer Complete Interrupt Enable */
    uint8_t  SUSP:1;           /*!< bit:      2  Channel Suspend Interrupt Enable   */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_CHINTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CHINTENSET_OFFSET      0x4D         /**< \brief (DMAC_CHINTENSET offset) Channel n Interrupt Enable Set */
#define DMAC_CHINTENSET_RESETVALUE  _U_(0x00)    /**< \brief (DMAC_CHINTENSET reset_value) Channel n Interrupt Enable Set */

#define DMAC_CHINTENSET_TERR_Pos    0            /**< \brief (DMAC_CHINTENSET) Channel Transfer Error Interrupt Enable */
#define DMAC_CHINTENSET_TERR        (_U_(0x1) << DMAC_CHINTENSET_TERR_Pos)
#define DMAC_CHINTENSET_TCMPL_Pos   1            /**< \brief (DMAC_CHINTENSET) Channel Transfer Complete Interrupt Enable */
#define DMAC_CHINTENSET_TCMPL       (_U_(0x1) << DMAC_CHINTENSET_TCMPL_Pos)
#define DMAC_CHINTENSET_SUSP_Pos    2            /**< \brief (DMAC_CHINTENSET) Channel Suspend Interrupt Enable */
#define DMAC_CHINTENSET_SUSP        (_U_(0x1) << DMAC_CHINTENSET_SUSP_Pos)
#define DMAC_CHINTENSET_MASK        _U_(0x07)    /**< \brief (DMAC_CHINTENSET) MASK Register */

/* -------- DMAC_CHINTFLAG : (DMAC Offset: 0x4E) (R/W  8) CHANNEL Channel n Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  TERR:1;           /*!< bit:      0  Channel Transfer Error             */
    __I uint8_t  TCMPL:1;          /*!< bit:      1  Channel Transfer Complete          */
    __I uint8_t  SUSP:1;           /*!< bit:      2  Channel Suspend                    */
    __I uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_CHINTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CHINTFLAG_OFFSET       0x4E         /**< \brief (DMAC_CHINTFLAG offset) Channel n Interrupt Flag Status and Clear */
#define DMAC_CHINTFLAG_RESETVALUE   _U_(0x00)    /**< \brief (DMAC_CHINTFLAG reset_value) Channel n Interrupt Flag Status and Clear */

#define DMAC_CHINTFLAG_TERR_Pos     0            /**< \brief (DMAC_CHINTFLAG) Channel Transfer Error */
#define DMAC_CHINTFLAG_TERR         (_U_(0x1) << DMAC_CHINTFLAG_TERR_Pos)
#define DMAC_CHINTFLAG_TCMPL_Pos    1            /**< \brief (DMAC_CHINTFLAG) Channel Transfer Complete */
#define DMAC_CHINTFLAG_TCMPL        (_U_(0x1) << DMAC_CHINTFLAG_TCMPL_Pos)
#define DMAC_CHINTFLAG_SUSP_Pos     2            /**< \brief (DMAC_CHINTFLAG) Channel Suspend */
#define DMAC_CHINTFLAG_SUSP         (_U_(0x1) << DMAC_CHINTFLAG_SUSP_Pos)
#define DMAC_CHINTFLAG_MASK         _U_(0x07)    /**< \brief (DMAC_CHINTFLAG) MASK Register */

/* -------- DMAC_CHSTATUS : (DMAC Offset: 0x4F) (R/W  8) CHANNEL Channel n Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  PEND:1;           /*!< bit:      0  Channel Pending                    */
    uint8_t  BUSY:1;           /*!< bit:      1  Channel Busy                       */
    uint8_t  FERR:1;           /*!< bit:      2  Channel Fetch Error                */
    uint8_t  CRCERR:1;         /*!< bit:      3  Channel CRC Error                  */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_CHSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CHSTATUS_OFFSET        0x4F         /**< \brief (DMAC_CHSTATUS offset) Channel n Status */
#define DMAC_CHSTATUS_RESETVALUE    _U_(0x00)    /**< \brief (DMAC_CHSTATUS reset_value) Channel n Status */

#define DMAC_CHSTATUS_PEND_Pos      0            /**< \brief (DMAC_CHSTATUS) Channel Pending */
#define DMAC_CHSTATUS_PEND          (_U_(0x1) << DMAC_CHSTATUS_PEND_Pos)
#define DMAC_CHSTATUS_BUSY_Pos      1            /**< \brief (DMAC_CHSTATUS) Channel Busy */
#define DMAC_CHSTATUS_BUSY          (_U_(0x1) << DMAC_CHSTATUS_BUSY_Pos)
#define DMAC_CHSTATUS_FERR_Pos      2            /**< \brief (DMAC_CHSTATUS) Channel Fetch Error */
#define DMAC_CHSTATUS_FERR          (_U_(0x1) << DMAC_CHSTATUS_FERR_Pos)
#define DMAC_CHSTATUS_CRCERR_Pos    3            /**< \brief (DMAC_CHSTATUS) Channel CRC Error */
#define DMAC_CHSTATUS_CRCERR        (_U_(0x1) << DMAC_CHSTATUS_CRCERR_Pos)
#define DMAC_CHSTATUS_MASK          _U_(0x0F)    /**< \brief (DMAC_CHSTATUS) MASK Register */

/* -------- DMAC_BTCTRL : (DMAC Offset: 0x00) (R/W 16) Block Transfer Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t VALID:1;          /*!< bit:      0  Descriptor Valid                   */
    uint16_t EVOSEL:2;         /*!< bit:  1.. 2  Block Event Output Selection       */
    uint16_t BLOCKACT:2;       /*!< bit:  3.. 4  Block Action                       */
    uint16_t :3;               /*!< bit:  5.. 7  Reserved                           */
    uint16_t BEATSIZE:2;       /*!< bit:  8.. 9  Beat Size                          */
    uint16_t SRCINC:1;         /*!< bit:     10  Source Address Increment Enable    */
    uint16_t DSTINC:1;         /*!< bit:     11  Destination Address Increment Enable */
    uint16_t STEPSEL:1;        /*!< bit:     12  Step Selection                     */
    uint16_t STEPSIZE:3;       /*!< bit: 13..15  Address Increment Step Size        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} DMAC_BTCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_BTCTRL_OFFSET          0x00         /**< \brief (DMAC_BTCTRL offset) Block Transfer Control */
#define DMAC_BTCTRL_RESETVALUE      _U_(0x0000)  /**< \brief (DMAC_BTCTRL reset_value) Block Transfer Control */

#define DMAC_BTCTRL_VALID_Pos       0            /**< \brief (DMAC_BTCTRL) Descriptor Valid */
#define DMAC_BTCTRL_VALID           (_U_(0x1) << DMAC_BTCTRL_VALID_Pos)
#define DMAC_BTCTRL_EVOSEL_Pos      1            /**< \brief (DMAC_BTCTRL) Block Event Output Selection */
#define DMAC_BTCTRL_EVOSEL_Msk      (_U_(0x3) << DMAC_BTCTRL_EVOSEL_Pos)
#define DMAC_BTCTRL_EVOSEL(value)   (DMAC_BTCTRL_EVOSEL_Msk & ((value) << DMAC_BTCTRL_EVOSEL_Pos))
#define   DMAC_BTCTRL_EVOSEL_DISABLE_Val  _U_(0x0)   /**< \brief (DMAC_BTCTRL) Event generation disabled */
#define   DMAC_BTCTRL_EVOSEL_BLOCK_Val    _U_(0x1)   /**< \brief (DMAC_BTCTRL) Block event strobe */
#define   DMAC_BTCTRL_EVOSEL_BURST_Val    _U_(0x3)   /**< \brief (DMAC_BTCTRL) Burst event strobe */
#define DMAC_BTCTRL_EVOSEL_DISABLE  (DMAC_BTCTRL_EVOSEL_DISABLE_Val << DMAC_BTCTRL_EVOSEL_Pos)
#define DMAC_BTCTRL_EVOSEL_BLOCK    (DMAC_BTCTRL_EVOSEL_BLOCK_Val  << DMAC_BTCTRL_EVOSEL_Pos)
#define DMAC_BTCTRL_EVOSEL_BURST    (DMAC_BTCTRL_EVOSEL_BURST_Val  << DMAC_BTCTRL_EVOSEL_Pos)
#define DMAC_BTCTRL_BLOCKACT_Pos    3            /**< \brief (DMAC_BTCTRL) Block Action */
#define DMAC_BTCTRL_BLOCKACT_Msk    (_U_(0x3) << DMAC_BTCTRL_BLOCKACT_Pos)
#define DMAC_BTCTRL_BLOCKACT(value) (DMAC_BTCTRL_BLOCKACT_Msk & ((value) << DMAC_BTCTRL_BLOCKACT_Pos))
#define   DMAC_BTCTRL_BLOCKACT_NOACT_Val  _U_(0x0)   /**< \brief (DMAC_BTCTRL) Channel will be disabled if it is the last block transfer in the transaction */
#define   DMAC_BTCTRL_BLOCKACT_INT_Val    _U_(0x1)   /**< \brief (DMAC_BTCTRL) Channel will be disabled if it is the last block transfer in the transaction and block interrupt */
#define   DMAC_BTCTRL_BLOCKACT_SUSPEND_Val _U_(0x2)   /**< \brief (DMAC_BTCTRL) Channel suspend operation is completed */
#define   DMAC_BTCTRL_BLOCKACT_BOTH_Val   _U_(0x3)   /**< \brief (DMAC_BTCTRL) Both channel suspend operation and block interrupt */
#define DMAC_BTCTRL_BLOCKACT_NOACT  (DMAC_BTCTRL_BLOCKACT_NOACT_Val << DMAC_BTCTRL_BLOCKACT_Pos)
#define DMAC_BTCTRL_BLOCKACT_INT    (DMAC_BTCTRL_BLOCKACT_INT_Val  << DMAC_BTCTRL_BLOCKACT_Pos)
#define DMAC_BTCTRL_BLOCKACT_SUSPEND (DMAC_BTCTRL_BLOCKACT_SUSPEND_Val << DMAC_BTCTRL_BLOCKACT_Pos)
#define DMAC_BTCTRL_BLOCKACT_BOTH   (DMAC_BTCTRL_BLOCKACT_BOTH_Val << DMAC_BTCTRL_BLOCKACT_Pos)
#define DMAC_BTCTRL_BEATSIZE_Pos    8            /**< \brief (DMAC_BTCTRL) Beat Size */
#define DMAC_BTCTRL_BEATSIZE_Msk    (_U_(0x3) << DMAC_BTCTRL_BEATSIZE_Pos)
#define DMAC_BTCTRL_BEATSIZE(value) (DMAC_BTCTRL_BEATSIZE_Msk & ((value) << DMAC_BTCTRL_BEATSIZE_Pos))
#define   DMAC_BTCTRL_BEATSIZE_BYTE_Val   _U_(0x0)   /**< \brief (DMAC_BTCTRL) 8-bit bus transfer */
#define   DMAC_BTCTRL_BEATSIZE_HWORD_Val  _U_(0x1)   /**< \brief (DMAC_BTCTRL) 16-bit bus transfer */
#define   DMAC_BTCTRL_BEATSIZE_WORD_Val   _U_(0x2)   /**< \brief (DMAC_BTCTRL) 32-bit bus transfer */
#define DMAC_BTCTRL_BEATSIZE_BYTE   (DMAC_BTCTRL_BEATSIZE_BYTE_Val << DMAC_BTCTRL_BEATSIZE_Pos)
#define DMAC_BTCTRL_BEATSIZE_HWORD  (DMAC_BTCTRL_BEATSIZE_HWORD_Val << DMAC_BTCTRL_BEATSIZE_Pos)
#define DMAC_BTCTRL_BEATSIZE_WORD   (DMAC_BTCTRL_BEATSIZE_WORD_Val << DMAC_BTCTRL_BEATSIZE_Pos)
#define DMAC_BTCTRL_SRCINC_Pos      10           /**< \brief (DMAC_BTCTRL) Source Address Increment Enable */
#define DMAC_BTCTRL_SRCINC          (_U_(0x1) << DMAC_BTCTRL_SRCINC_Pos)
#define DMAC_BTCTRL_DSTINC_Pos      11           /**< \brief (DMAC_BTCTRL) Destination Address Increment Enable */
#define DMAC_BTCTRL_DSTINC          (_U_(0x1) << DMAC_BTCTRL_DSTINC_Pos)
#define DMAC_BTCTRL_STEPSEL_Pos     12           /**< \brief (DMAC_BTCTRL) Step Selection */
#define DMAC_BTCTRL_STEPSEL         (_U_(0x1) << DMAC_BTCTRL_STEPSEL_Pos)
#define   DMAC_BTCTRL_STEPSEL_DST_Val     _U_(0x0)   /**< \brief (DMAC_BTCTRL) Step size settings apply to the destination address */
#define   DMAC_BTCTRL_STEPSEL_SRC_Val     _U_(0x1)   /**< \brief (DMAC_BTCTRL) Step size settings apply to the source address */
#define DMAC_BTCTRL_STEPSEL_DST     (DMAC_BTCTRL_STEPSEL_DST_Val   << DMAC_BTCTRL_STEPSEL_Pos)
#define DMAC_BTCTRL_STEPSEL_SRC     (DMAC_BTCTRL_STEPSEL_SRC_Val   << DMAC_BTCTRL_STEPSEL_Pos)
#define DMAC_BTCTRL_STEPSIZE_Pos    13           /**< \brief (DMAC_BTCTRL) Address Increment Step Size */
#define DMAC_BTCTRL_STEPSIZE_Msk    (_U_(0x7) << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_STEPSIZE(value) (DMAC_BTCTRL_STEPSIZE_Msk & ((value) << DMAC_BTCTRL_STEPSIZE_Pos))
#define   DMAC_BTCTRL_STEPSIZE_X1_Val     _U_(0x0)   /**< \brief (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 1 */
#define   DMAC_BTCTRL_STEPSIZE_X2_Val     _U_(0x1)   /**< \brief (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 2 */
#define   DMAC_BTCTRL_STEPSIZE_X4_Val     _U_(0x2)   /**< \brief (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 4 */
#define   DMAC_BTCTRL_STEPSIZE_X8_Val     _U_(0x3)   /**< \brief (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 8 */
#define   DMAC_BTCTRL_STEPSIZE_X16_Val    _U_(0x4)   /**< \brief (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 16 */
#define   DMAC_BTCTRL_STEPSIZE_X32_Val    _U_(0x5)   /**< \brief (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 32 */
#define   DMAC_BTCTRL_STEPSIZE_X64_Val    _U_(0x6)   /**< \brief (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 64 */
#define   DMAC_BTCTRL_STEPSIZE_X128_Val   _U_(0x7)   /**< \brief (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 128 */
#define DMAC_BTCTRL_STEPSIZE_X1     (DMAC_BTCTRL_STEPSIZE_X1_Val   << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_STEPSIZE_X2     (DMAC_BTCTRL_STEPSIZE_X2_Val   << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_STEPSIZE_X4     (DMAC_BTCTRL_STEPSIZE_X4_Val   << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_STEPSIZE_X8     (DMAC_BTCTRL_STEPSIZE_X8_Val   << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_STEPSIZE_X16    (DMAC_BTCTRL_STEPSIZE_X16_Val  << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_STEPSIZE_X32    (DMAC_BTCTRL_STEPSIZE_X32_Val  << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_STEPSIZE_X64    (DMAC_BTCTRL_STEPSIZE_X64_Val  << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_STEPSIZE_X128   (DMAC_BTCTRL_STEPSIZE_X128_Val << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_MASK            _U_(0xFF1F)  /**< \brief (DMAC_BTCTRL) MASK Register */

/* -------- DMAC_BTCNT : (DMAC Offset: 0x02) (R/W 16) Block Transfer Count -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t BTCNT:16;         /*!< bit:  0..15  Block Transfer Count               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} DMAC_BTCNT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_BTCNT_OFFSET           0x02         /**< \brief (DMAC_BTCNT offset) Block Transfer Count */
#define DMAC_BTCNT_RESETVALUE       _U_(0x0000)  /**< \brief (DMAC_BTCNT reset_value) Block Transfer Count */

#define DMAC_BTCNT_BTCNT_Pos        0            /**< \brief (DMAC_BTCNT) Block Transfer Count */
#define DMAC_BTCNT_BTCNT_Msk        (_U_(0xFFFF) << DMAC_BTCNT_BTCNT_Pos)
#define DMAC_BTCNT_BTCNT(value)     (DMAC_BTCNT_BTCNT_Msk & ((value) << DMAC_BTCNT_BTCNT_Pos))
#define DMAC_BTCNT_MASK             _U_(0xFFFF)  /**< \brief (DMAC_BTCNT) MASK Register */

/* -------- DMAC_SRCADDR : (DMAC Offset: 0x04) (R/W 32) Block Transfer Source Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SRCADDR:32;       /*!< bit:  0..31  Transfer Source Address            */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_SRCADDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_SRCADDR_OFFSET         0x04         /**< \brief (DMAC_SRCADDR offset) Block Transfer Source Address */
#define DMAC_SRCADDR_RESETVALUE     _U_(0x00000000) /**< \brief (DMAC_SRCADDR reset_value) Block Transfer Source Address */

#define DMAC_SRCADDR_SRCADDR_Pos    0            /**< \brief (DMAC_SRCADDR) Transfer Source Address */
#define DMAC_SRCADDR_SRCADDR_Msk    (_U_(0xFFFFFFFF) << DMAC_SRCADDR_SRCADDR_Pos)
#define DMAC_SRCADDR_SRCADDR(value) (DMAC_SRCADDR_SRCADDR_Msk & ((value) << DMAC_SRCADDR_SRCADDR_Pos))
#define DMAC_SRCADDR_MASK           _U_(0xFFFFFFFF) /**< \brief (DMAC_SRCADDR) MASK Register */

/* -------- DMAC_DSTADDR : (DMAC Offset: 0x08) (R/W 32) Block Transfer Destination Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct { // CRC mode
    uint32_t CHKINIT:32;       /*!< bit:  0..31  CRC Checksum Initial Value         */
  } CRC;                       /*!< Structure used for CRC                          */
  struct {
    uint32_t DSTADDR:32;       /*!< bit:  0..31  Transfer Destination Address       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_DSTADDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_DSTADDR_OFFSET         0x08         /**< \brief (DMAC_DSTADDR offset) Block Transfer Destination Address */

// CRC mode
#define DMAC_DSTADDR_CRC_CHKINIT_Pos 0            /**< \brief (DMAC_DSTADDR_CRC) CRC Checksum Initial Value */
#define DMAC_DSTADDR_CRC_CHKINIT_Msk (_U_(0xFFFFFFFF) << DMAC_DSTADDR_CRC_CHKINIT_Pos)
#define DMAC_DSTADDR_CRC_CHKINIT(value) (DMAC_DSTADDR_CRC_CHKINIT_Msk & ((value) << DMAC_DSTADDR_CRC_CHKINIT_Pos))
#define DMAC_DSTADDR_CRC_MASK       _U_(0xFFFFFFFF) /**< \brief (DMAC_DSTADDR_CRC) MASK Register */

#define DMAC_DSTADDR_DSTADDR_Pos    0            /**< \brief (DMAC_DSTADDR) Transfer Destination Address */
#define DMAC_DSTADDR_DSTADDR_Msk    (_U_(0xFFFFFFFF) << DMAC_DSTADDR_DSTADDR_Pos)
#define DMAC_DSTADDR_DSTADDR(value) (DMAC_DSTADDR_DSTADDR_Msk & ((value) << DMAC_DSTADDR_DSTADDR_Pos))
#define DMAC_DSTADDR_MASK           _U_(0xFFFFFFFF) /**< \brief (DMAC_DSTADDR) MASK Register */

/* -------- DMAC_DESCADDR : (DMAC Offset: 0x0C) (R/W 32) Next Descriptor Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DESCADDR:32;      /*!< bit:  0..31  Next Descriptor Address            */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_DESCADDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_DESCADDR_OFFSET        0x0C         /**< \brief (DMAC_DESCADDR offset) Next Descriptor Address */

#define DMAC_DESCADDR_DESCADDR_Pos  0            /**< \brief (DMAC_DESCADDR) Next Descriptor Address */
#define DMAC_DESCADDR_DESCADDR_Msk  (_U_(0xFFFFFFFF) << DMAC_DESCADDR_DESCADDR_Pos)
#define DMAC_DESCADDR_DESCADDR(value) (DMAC_DESCADDR_DESCADDR_Msk & ((value) << DMAC_DESCADDR_DESCADDR_Pos))
#define DMAC_DESCADDR_MASK          _U_(0xFFFFFFFF) /**< \brief (DMAC_DESCADDR) MASK Register */

/** \brief DmacChannel hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO DMAC_CHCTRLA_Type         CHCTRLA;     /**< \brief Offset: 0x00 (R/W 32) Channel n Control A */
  __IO DMAC_CHCTRLB_Type         CHCTRLB;     /**< \brief Offset: 0x04 (R/W  8) Channel n Control B */
  __IO DMAC_CHPRILVL_Type        CHPRILVL;    /**< \brief Offset: 0x05 (R/W  8) Channel n Priority Level */
  __IO DMAC_CHEVCTRL_Type        CHEVCTRL;    /**< \brief Offset: 0x06 (R/W  8) Channel n Event Control */
       RoReg8                    Reserved1[0x5];
  __IO DMAC_CHINTENCLR_Type      CHINTENCLR;  /**< \brief Offset: 0x0C (R/W  8) Channel n Interrupt Enable Clear */
  __IO DMAC_CHINTENSET_Type      CHINTENSET;  /**< \brief Offset: 0x0D (R/W  8) Channel n Interrupt Enable Set */
  __IO DMAC_CHINTFLAG_Type       CHINTFLAG;   /**< \brief Offset: 0x0E (R/W  8) Channel n Interrupt Flag Status and Clear */
  __IO DMAC_CHSTATUS_Type        CHSTATUS;    /**< \brief Offset: 0x0F (R/W  8) Channel n Status */
} DmacChannel;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief DMAC APB hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO DMAC_CTRL_Type            CTRL;        /**< \brief Offset: 0x00 (R/W 16) Control */
  __IO DMAC_CRCCTRL_Type         CRCCTRL;     /**< \brief Offset: 0x02 (R/W 16) CRC Control */
  __IO DMAC_CRCDATAIN_Type       CRCDATAIN;   /**< \brief Offset: 0x04 (R/W 32) CRC Data Input */
  __IO DMAC_CRCCHKSUM_Type       CRCCHKSUM;   /**< \brief Offset: 0x08 (R/W 32) CRC Checksum */
  __IO DMAC_CRCSTATUS_Type       CRCSTATUS;   /**< \brief Offset: 0x0C (R/W  8) CRC Status */
  __IO DMAC_DBGCTRL_Type         DBGCTRL;     /**< \brief Offset: 0x0D (R/W  8) Debug Control */
       RoReg8                    Reserved1[0x2];
  __IO DMAC_SWTRIGCTRL_Type      SWTRIGCTRL;  /**< \brief Offset: 0x10 (R/W 32) Software Trigger Control */
  __IO DMAC_PRICTRL0_Type        PRICTRL0;    /**< \brief Offset: 0x14 (R/W 32) Priority Control 0 */
       RoReg8                    Reserved2[0x8];
  __IO DMAC_INTPEND_Type         INTPEND;     /**< \brief Offset: 0x20 (R/W 16) Interrupt Pending */
       RoReg8                    Reserved3[0x2];
  __I  DMAC_INTSTATUS_Type       INTSTATUS;   /**< \brief Offset: 0x24 (R/  32) Interrupt Status */
  __I  DMAC_BUSYCH_Type          BUSYCH;      /**< \brief Offset: 0x28 (R/  32) Busy Channels */
  __I  DMAC_PENDCH_Type          PENDCH;      /**< \brief Offset: 0x2C (R/  32) Pending Channels */
  __I  DMAC_ACTIVE_Type          ACTIVE;      /**< \brief Offset: 0x30 (R/  32) Active Channel and Levels */
  __IO DMAC_BASEADDR_Type        BASEADDR;    /**< \brief Offset: 0x34 (R/W 32) Descriptor Memory Section Base Address */
  __IO DMAC_WRBADDR_Type         WRBADDR;     /**< \brief Offset: 0x38 (R/W 32) Write-Back Memory Section Base Address */
       RoReg8                    Reserved4[0x4];
       DmacChannel               Channel[32]; /**< \brief Offset: 0x40 DmacChannel groups [CH_NUM] */
} Dmac;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief DMAC Descriptor SRAM registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO DMAC_BTCTRL_Type          BTCTRL;      /**< \brief Offset: 0x00 (R/W 16) Block Transfer Control */
  __IO DMAC_BTCNT_Type           BTCNT;       /**< \brief Offset: 0x02 (R/W 16) Block Transfer Count */
  __IO DMAC_SRCADDR_Type         SRCADDR;     /**< \brief Offset: 0x04 (R/W 32) Block Transfer Source Address */
  __IO DMAC_DSTADDR_Type         DSTADDR;     /**< \brief Offset: 0x08 (R/W 32) Block Transfer Destination Address */
  __IO DMAC_DESCADDR_Type        DESCADDR;    /**< \brief Offset: 0x0C (R/W 32) Next Descriptor Address */
} DmacDescriptor
#ifdef __GNUC__
  __attribute__ ((aligned (8)))
#endif
;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#ifdef __GNUC__
 #define SECTION_DMAC_DESCRIPTOR      __attribute__ ((section(".hsram")))
#elif defined(__ICCARM__)
 #define SECTION_DMAC_DESCRIPTOR      @".hsram"
#endif

/*@}*/

#endif /* _SAMD51_DMAC_COMPONENT_ */
