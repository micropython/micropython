/**
 * \file
 *
 * \brief Component description for DMAC
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD21_DMAC_COMPONENT_
#define _SAMD21_DMAC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR DMAC */
/* ========================================================================== */
/** \addtogroup SAMD21_DMAC Direct Memory Access Controller */
/*@{*/

#define DMAC_U2223
#define REV_DMAC                    0x110

/* -------- DMAC_CTRL : (DMAC Offset: 0x00) (R/W 16) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint16_t DMAENABLE:1;      /*!< bit:      1  DMA Enable                         */
    uint16_t CRCENABLE:1;      /*!< bit:      2  CRC Enable                         */
    uint16_t :5;               /*!< bit:  3.. 7  Reserved                           */
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
#define DMAC_CTRL_RESETVALUE        0x0000ul     /**< \brief (DMAC_CTRL reset_value) Control */

#define DMAC_CTRL_SWRST_Pos         0            /**< \brief (DMAC_CTRL) Software Reset */
#define DMAC_CTRL_SWRST             (0x1ul << DMAC_CTRL_SWRST_Pos)
#define DMAC_CTRL_DMAENABLE_Pos     1            /**< \brief (DMAC_CTRL) DMA Enable */
#define DMAC_CTRL_DMAENABLE         (0x1ul << DMAC_CTRL_DMAENABLE_Pos)
#define DMAC_CTRL_CRCENABLE_Pos     2            /**< \brief (DMAC_CTRL) CRC Enable */
#define DMAC_CTRL_CRCENABLE         (0x1ul << DMAC_CTRL_CRCENABLE_Pos)
#define DMAC_CTRL_LVLEN0_Pos        8            /**< \brief (DMAC_CTRL) Priority Level 0 Enable */
#define DMAC_CTRL_LVLEN0            (1 << DMAC_CTRL_LVLEN0_Pos)
#define DMAC_CTRL_LVLEN1_Pos        9            /**< \brief (DMAC_CTRL) Priority Level 1 Enable */
#define DMAC_CTRL_LVLEN1            (1 << DMAC_CTRL_LVLEN1_Pos)
#define DMAC_CTRL_LVLEN2_Pos        10           /**< \brief (DMAC_CTRL) Priority Level 2 Enable */
#define DMAC_CTRL_LVLEN2            (1 << DMAC_CTRL_LVLEN2_Pos)
#define DMAC_CTRL_LVLEN3_Pos        11           /**< \brief (DMAC_CTRL) Priority Level 3 Enable */
#define DMAC_CTRL_LVLEN3            (1 << DMAC_CTRL_LVLEN3_Pos)
#define DMAC_CTRL_LVLEN_Pos         8            /**< \brief (DMAC_CTRL) Priority Level x Enable */
#define DMAC_CTRL_LVLEN_Msk         (0xFul << DMAC_CTRL_LVLEN_Pos)
#define DMAC_CTRL_LVLEN(value)      (DMAC_CTRL_LVLEN_Msk & ((value) << DMAC_CTRL_LVLEN_Pos))
#define DMAC_CTRL_MASK              0x0F07ul     /**< \brief (DMAC_CTRL) MASK Register */

/* -------- DMAC_CRCCTRL : (DMAC Offset: 0x02) (R/W 16) CRC Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t CRCBEATSIZE:2;    /*!< bit:  0.. 1  CRC Beat Size                      */
    uint16_t CRCPOLY:2;        /*!< bit:  2.. 3  CRC Polynomial Type                */
    uint16_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint16_t CRCSRC:6;         /*!< bit:  8..13  CRC Input Source                   */
    uint16_t :2;               /*!< bit: 14..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} DMAC_CRCCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CRCCTRL_OFFSET         0x02         /**< \brief (DMAC_CRCCTRL offset) CRC Control */
#define DMAC_CRCCTRL_RESETVALUE     0x0000ul     /**< \brief (DMAC_CRCCTRL reset_value) CRC Control */

#define DMAC_CRCCTRL_CRCBEATSIZE_Pos 0            /**< \brief (DMAC_CRCCTRL) CRC Beat Size */
#define DMAC_CRCCTRL_CRCBEATSIZE_Msk (0x3ul << DMAC_CRCCTRL_CRCBEATSIZE_Pos)
#define DMAC_CRCCTRL_CRCBEATSIZE(value) (DMAC_CRCCTRL_CRCBEATSIZE_Msk & ((value) << DMAC_CRCCTRL_CRCBEATSIZE_Pos))
#define   DMAC_CRCCTRL_CRCBEATSIZE_BYTE_Val 0x0ul  /**< \brief (DMAC_CRCCTRL) Byte bus access */
#define   DMAC_CRCCTRL_CRCBEATSIZE_HWORD_Val 0x1ul  /**< \brief (DMAC_CRCCTRL) Half-word bus access */
#define   DMAC_CRCCTRL_CRCBEATSIZE_WORD_Val 0x2ul  /**< \brief (DMAC_CRCCTRL) Word bus access */
#define DMAC_CRCCTRL_CRCBEATSIZE_BYTE (DMAC_CRCCTRL_CRCBEATSIZE_BYTE_Val << DMAC_CRCCTRL_CRCBEATSIZE_Pos)
#define DMAC_CRCCTRL_CRCBEATSIZE_HWORD (DMAC_CRCCTRL_CRCBEATSIZE_HWORD_Val << DMAC_CRCCTRL_CRCBEATSIZE_Pos)
#define DMAC_CRCCTRL_CRCBEATSIZE_WORD (DMAC_CRCCTRL_CRCBEATSIZE_WORD_Val << DMAC_CRCCTRL_CRCBEATSIZE_Pos)
#define DMAC_CRCCTRL_CRCPOLY_Pos    2            /**< \brief (DMAC_CRCCTRL) CRC Polynomial Type */
#define DMAC_CRCCTRL_CRCPOLY_Msk    (0x3ul << DMAC_CRCCTRL_CRCPOLY_Pos)
#define DMAC_CRCCTRL_CRCPOLY(value) (DMAC_CRCCTRL_CRCPOLY_Msk & ((value) << DMAC_CRCCTRL_CRCPOLY_Pos))
#define   DMAC_CRCCTRL_CRCPOLY_CRC16_Val  0x0ul  /**< \brief (DMAC_CRCCTRL) CRC-16 (CRC-CCITT) */
#define   DMAC_CRCCTRL_CRCPOLY_CRC32_Val  0x1ul  /**< \brief (DMAC_CRCCTRL) CRC32 (IEEE 802.3) */
#define DMAC_CRCCTRL_CRCPOLY_CRC16  (DMAC_CRCCTRL_CRCPOLY_CRC16_Val << DMAC_CRCCTRL_CRCPOLY_Pos)
#define DMAC_CRCCTRL_CRCPOLY_CRC32  (DMAC_CRCCTRL_CRCPOLY_CRC32_Val << DMAC_CRCCTRL_CRCPOLY_Pos)
#define DMAC_CRCCTRL_CRCSRC_Pos     8            /**< \brief (DMAC_CRCCTRL) CRC Input Source */
#define DMAC_CRCCTRL_CRCSRC_Msk     (0x3Ful << DMAC_CRCCTRL_CRCSRC_Pos)
#define DMAC_CRCCTRL_CRCSRC(value)  (DMAC_CRCCTRL_CRCSRC_Msk & ((value) << DMAC_CRCCTRL_CRCSRC_Pos))
#define   DMAC_CRCCTRL_CRCSRC_NOACT_Val   0x0ul  /**< \brief (DMAC_CRCCTRL) No action */
#define   DMAC_CRCCTRL_CRCSRC_IO_Val      0x1ul  /**< \brief (DMAC_CRCCTRL) I/O interface */
#define DMAC_CRCCTRL_CRCSRC_NOACT   (DMAC_CRCCTRL_CRCSRC_NOACT_Val << DMAC_CRCCTRL_CRCSRC_Pos)
#define DMAC_CRCCTRL_CRCSRC_IO      (DMAC_CRCCTRL_CRCSRC_IO_Val    << DMAC_CRCCTRL_CRCSRC_Pos)
#define DMAC_CRCCTRL_MASK           0x3F0Ful     /**< \brief (DMAC_CRCCTRL) MASK Register */

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
#define DMAC_CRCDATAIN_RESETVALUE   0x00000000ul /**< \brief (DMAC_CRCDATAIN reset_value) CRC Data Input */

#define DMAC_CRCDATAIN_CRCDATAIN_Pos 0            /**< \brief (DMAC_CRCDATAIN) CRC Data Input */
#define DMAC_CRCDATAIN_CRCDATAIN_Msk (0xFFFFFFFFul << DMAC_CRCDATAIN_CRCDATAIN_Pos)
#define DMAC_CRCDATAIN_CRCDATAIN(value) (DMAC_CRCDATAIN_CRCDATAIN_Msk & ((value) << DMAC_CRCDATAIN_CRCDATAIN_Pos))
#define DMAC_CRCDATAIN_MASK         0xFFFFFFFFul /**< \brief (DMAC_CRCDATAIN) MASK Register */

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
#define DMAC_CRCCHKSUM_RESETVALUE   0x00000000ul /**< \brief (DMAC_CRCCHKSUM reset_value) CRC Checksum */

#define DMAC_CRCCHKSUM_CRCCHKSUM_Pos 0            /**< \brief (DMAC_CRCCHKSUM) CRC Checksum */
#define DMAC_CRCCHKSUM_CRCCHKSUM_Msk (0xFFFFFFFFul << DMAC_CRCCHKSUM_CRCCHKSUM_Pos)
#define DMAC_CRCCHKSUM_CRCCHKSUM(value) (DMAC_CRCCHKSUM_CRCCHKSUM_Msk & ((value) << DMAC_CRCCHKSUM_CRCCHKSUM_Pos))
#define DMAC_CRCCHKSUM_MASK         0xFFFFFFFFul /**< \brief (DMAC_CRCCHKSUM) MASK Register */

/* -------- DMAC_CRCSTATUS : (DMAC Offset: 0x0C) (R/W  8) CRC Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CRCBUSY:1;        /*!< bit:      0  CRC Module Busy                    */
    uint8_t  CRCZERO:1;        /*!< bit:      1  CRC Zero                           */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_CRCSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CRCSTATUS_OFFSET       0x0C         /**< \brief (DMAC_CRCSTATUS offset) CRC Status */
#define DMAC_CRCSTATUS_RESETVALUE   0x00ul       /**< \brief (DMAC_CRCSTATUS reset_value) CRC Status */

#define DMAC_CRCSTATUS_CRCBUSY_Pos  0            /**< \brief (DMAC_CRCSTATUS) CRC Module Busy */
#define DMAC_CRCSTATUS_CRCBUSY      (0x1ul << DMAC_CRCSTATUS_CRCBUSY_Pos)
#define DMAC_CRCSTATUS_CRCZERO_Pos  1            /**< \brief (DMAC_CRCSTATUS) CRC Zero */
#define DMAC_CRCSTATUS_CRCZERO      (0x1ul << DMAC_CRCSTATUS_CRCZERO_Pos)
#define DMAC_CRCSTATUS_MASK         0x03ul       /**< \brief (DMAC_CRCSTATUS) MASK Register */

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
#define DMAC_DBGCTRL_RESETVALUE     0x00ul       /**< \brief (DMAC_DBGCTRL reset_value) Debug Control */

#define DMAC_DBGCTRL_DBGRUN_Pos     0            /**< \brief (DMAC_DBGCTRL) Debug Run */
#define DMAC_DBGCTRL_DBGRUN         (0x1ul << DMAC_DBGCTRL_DBGRUN_Pos)
#define DMAC_DBGCTRL_MASK           0x01ul       /**< \brief (DMAC_DBGCTRL) MASK Register */

/* -------- DMAC_QOSCTRL : (DMAC Offset: 0x0E) (R/W  8) QOS Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  WRBQOS:2;         /*!< bit:  0.. 1  Write-Back Quality of Service      */
    uint8_t  FQOS:2;           /*!< bit:  2.. 3  Fetch Quality of Service           */
    uint8_t  DQOS:2;           /*!< bit:  4.. 5  Data Transfer Quality of Service   */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_QOSCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_QOSCTRL_OFFSET         0x0E         /**< \brief (DMAC_QOSCTRL offset) QOS Control */
#define DMAC_QOSCTRL_RESETVALUE     0x15ul       /**< \brief (DMAC_QOSCTRL reset_value) QOS Control */

#define DMAC_QOSCTRL_WRBQOS_Pos     0            /**< \brief (DMAC_QOSCTRL) Write-Back Quality of Service */
#define DMAC_QOSCTRL_WRBQOS_Msk     (0x3ul << DMAC_QOSCTRL_WRBQOS_Pos)
#define DMAC_QOSCTRL_WRBQOS(value)  (DMAC_QOSCTRL_WRBQOS_Msk & ((value) << DMAC_QOSCTRL_WRBQOS_Pos))
#define   DMAC_QOSCTRL_WRBQOS_DISABLE_Val 0x0ul  /**< \brief (DMAC_QOSCTRL) Background (no sensitive operation) */
#define   DMAC_QOSCTRL_WRBQOS_LOW_Val     0x1ul  /**< \brief (DMAC_QOSCTRL) Sensitive Bandwidth */
#define   DMAC_QOSCTRL_WRBQOS_MEDIUM_Val  0x2ul  /**< \brief (DMAC_QOSCTRL) Sensitive Latency */
#define   DMAC_QOSCTRL_WRBQOS_HIGH_Val    0x3ul  /**< \brief (DMAC_QOSCTRL) Critical Latency */
#define DMAC_QOSCTRL_WRBQOS_DISABLE (DMAC_QOSCTRL_WRBQOS_DISABLE_Val << DMAC_QOSCTRL_WRBQOS_Pos)
#define DMAC_QOSCTRL_WRBQOS_LOW     (DMAC_QOSCTRL_WRBQOS_LOW_Val   << DMAC_QOSCTRL_WRBQOS_Pos)
#define DMAC_QOSCTRL_WRBQOS_MEDIUM  (DMAC_QOSCTRL_WRBQOS_MEDIUM_Val << DMAC_QOSCTRL_WRBQOS_Pos)
#define DMAC_QOSCTRL_WRBQOS_HIGH    (DMAC_QOSCTRL_WRBQOS_HIGH_Val  << DMAC_QOSCTRL_WRBQOS_Pos)
#define DMAC_QOSCTRL_FQOS_Pos       2            /**< \brief (DMAC_QOSCTRL) Fetch Quality of Service */
#define DMAC_QOSCTRL_FQOS_Msk       (0x3ul << DMAC_QOSCTRL_FQOS_Pos)
#define DMAC_QOSCTRL_FQOS(value)    (DMAC_QOSCTRL_FQOS_Msk & ((value) << DMAC_QOSCTRL_FQOS_Pos))
#define   DMAC_QOSCTRL_FQOS_DISABLE_Val   0x0ul  /**< \brief (DMAC_QOSCTRL) Background (no sensitive operation) */
#define   DMAC_QOSCTRL_FQOS_LOW_Val       0x1ul  /**< \brief (DMAC_QOSCTRL) Sensitive Bandwidth */
#define   DMAC_QOSCTRL_FQOS_MEDIUM_Val    0x2ul  /**< \brief (DMAC_QOSCTRL) Sensitive Latency */
#define   DMAC_QOSCTRL_FQOS_HIGH_Val      0x3ul  /**< \brief (DMAC_QOSCTRL) Critical Latency */
#define DMAC_QOSCTRL_FQOS_DISABLE   (DMAC_QOSCTRL_FQOS_DISABLE_Val << DMAC_QOSCTRL_FQOS_Pos)
#define DMAC_QOSCTRL_FQOS_LOW       (DMAC_QOSCTRL_FQOS_LOW_Val     << DMAC_QOSCTRL_FQOS_Pos)
#define DMAC_QOSCTRL_FQOS_MEDIUM    (DMAC_QOSCTRL_FQOS_MEDIUM_Val  << DMAC_QOSCTRL_FQOS_Pos)
#define DMAC_QOSCTRL_FQOS_HIGH      (DMAC_QOSCTRL_FQOS_HIGH_Val    << DMAC_QOSCTRL_FQOS_Pos)
#define DMAC_QOSCTRL_DQOS_Pos       4            /**< \brief (DMAC_QOSCTRL) Data Transfer Quality of Service */
#define DMAC_QOSCTRL_DQOS_Msk       (0x3ul << DMAC_QOSCTRL_DQOS_Pos)
#define DMAC_QOSCTRL_DQOS(value)    (DMAC_QOSCTRL_DQOS_Msk & ((value) << DMAC_QOSCTRL_DQOS_Pos))
#define   DMAC_QOSCTRL_DQOS_DISABLE_Val   0x0ul  /**< \brief (DMAC_QOSCTRL) Background (no sensitive operation) */
#define   DMAC_QOSCTRL_DQOS_LOW_Val       0x1ul  /**< \brief (DMAC_QOSCTRL) Sensitive Bandwidth */
#define   DMAC_QOSCTRL_DQOS_MEDIUM_Val    0x2ul  /**< \brief (DMAC_QOSCTRL) Sensitive Latency */
#define   DMAC_QOSCTRL_DQOS_HIGH_Val      0x3ul  /**< \brief (DMAC_QOSCTRL) Critical Latency */
#define DMAC_QOSCTRL_DQOS_DISABLE   (DMAC_QOSCTRL_DQOS_DISABLE_Val << DMAC_QOSCTRL_DQOS_Pos)
#define DMAC_QOSCTRL_DQOS_LOW       (DMAC_QOSCTRL_DQOS_LOW_Val     << DMAC_QOSCTRL_DQOS_Pos)
#define DMAC_QOSCTRL_DQOS_MEDIUM    (DMAC_QOSCTRL_DQOS_MEDIUM_Val  << DMAC_QOSCTRL_DQOS_Pos)
#define DMAC_QOSCTRL_DQOS_HIGH      (DMAC_QOSCTRL_DQOS_HIGH_Val    << DMAC_QOSCTRL_DQOS_Pos)
#define DMAC_QOSCTRL_MASK           0x3Ful       /**< \brief (DMAC_QOSCTRL) MASK Register */

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
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t SWTRIG:12;        /*!< bit:  0..11  Channel x Software Trigger         */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_SWTRIGCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_SWTRIGCTRL_OFFSET      0x10         /**< \brief (DMAC_SWTRIGCTRL offset) Software Trigger Control */
#define DMAC_SWTRIGCTRL_RESETVALUE  0x00000000ul /**< \brief (DMAC_SWTRIGCTRL reset_value) Software Trigger Control */

#define DMAC_SWTRIGCTRL_SWTRIG0_Pos 0            /**< \brief (DMAC_SWTRIGCTRL) Channel 0 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG0     (1 << DMAC_SWTRIGCTRL_SWTRIG0_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG1_Pos 1            /**< \brief (DMAC_SWTRIGCTRL) Channel 1 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG1     (1 << DMAC_SWTRIGCTRL_SWTRIG1_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG2_Pos 2            /**< \brief (DMAC_SWTRIGCTRL) Channel 2 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG2     (1 << DMAC_SWTRIGCTRL_SWTRIG2_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG3_Pos 3            /**< \brief (DMAC_SWTRIGCTRL) Channel 3 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG3     (1 << DMAC_SWTRIGCTRL_SWTRIG3_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG4_Pos 4            /**< \brief (DMAC_SWTRIGCTRL) Channel 4 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG4     (1 << DMAC_SWTRIGCTRL_SWTRIG4_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG5_Pos 5            /**< \brief (DMAC_SWTRIGCTRL) Channel 5 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG5     (1 << DMAC_SWTRIGCTRL_SWTRIG5_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG6_Pos 6            /**< \brief (DMAC_SWTRIGCTRL) Channel 6 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG6     (1 << DMAC_SWTRIGCTRL_SWTRIG6_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG7_Pos 7            /**< \brief (DMAC_SWTRIGCTRL) Channel 7 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG7     (1 << DMAC_SWTRIGCTRL_SWTRIG7_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG8_Pos 8            /**< \brief (DMAC_SWTRIGCTRL) Channel 8 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG8     (1 << DMAC_SWTRIGCTRL_SWTRIG8_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG9_Pos 9            /**< \brief (DMAC_SWTRIGCTRL) Channel 9 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG9     (1 << DMAC_SWTRIGCTRL_SWTRIG9_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG10_Pos 10           /**< \brief (DMAC_SWTRIGCTRL) Channel 10 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG10    (1 << DMAC_SWTRIGCTRL_SWTRIG10_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG11_Pos 11           /**< \brief (DMAC_SWTRIGCTRL) Channel 11 Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG11    (1 << DMAC_SWTRIGCTRL_SWTRIG11_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG_Pos  0            /**< \brief (DMAC_SWTRIGCTRL) Channel x Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG_Msk  (0xFFFul << DMAC_SWTRIGCTRL_SWTRIG_Pos)
#define DMAC_SWTRIGCTRL_SWTRIG(value) (DMAC_SWTRIGCTRL_SWTRIG_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG_Pos))
#define DMAC_SWTRIGCTRL_MASK        0x00000FFFul /**< \brief (DMAC_SWTRIGCTRL) MASK Register */

/* -------- DMAC_PRICTRL0 : (DMAC Offset: 0x14) (R/W 32) Priority Control 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t LVLPRI0:4;        /*!< bit:  0.. 3  Level 0 Channel Priority Number    */
    uint32_t :3;               /*!< bit:  4.. 6  Reserved                           */
    uint32_t RRLVLEN0:1;       /*!< bit:      7  Level 0 Round-Robin Scheduling Enable */
    uint32_t LVLPRI1:4;        /*!< bit:  8..11  Level 1 Channel Priority Number    */
    uint32_t :3;               /*!< bit: 12..14  Reserved                           */
    uint32_t RRLVLEN1:1;       /*!< bit:     15  Level 1 Round-Robin Scheduling Enable */
    uint32_t LVLPRI2:4;        /*!< bit: 16..19  Level 2 Channel Priority Number    */
    uint32_t :3;               /*!< bit: 20..22  Reserved                           */
    uint32_t RRLVLEN2:1;       /*!< bit:     23  Level 2 Round-Robin Scheduling Enable */
    uint32_t LVLPRI3:4;        /*!< bit: 24..27  Level 3 Channel Priority Number    */
    uint32_t :3;               /*!< bit: 28..30  Reserved                           */
    uint32_t RRLVLEN3:1;       /*!< bit:     31  Level 3 Round-Robin Scheduling Enable */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_PRICTRL0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_PRICTRL0_OFFSET        0x14         /**< \brief (DMAC_PRICTRL0 offset) Priority Control 0 */
#define DMAC_PRICTRL0_RESETVALUE    0x00000000ul /**< \brief (DMAC_PRICTRL0 reset_value) Priority Control 0 */

#define DMAC_PRICTRL0_LVLPRI0_Pos   0            /**< \brief (DMAC_PRICTRL0) Level 0 Channel Priority Number */
#define DMAC_PRICTRL0_LVLPRI0_Msk   (0xFul << DMAC_PRICTRL0_LVLPRI0_Pos)
#define DMAC_PRICTRL0_LVLPRI0(value) (DMAC_PRICTRL0_LVLPRI0_Msk & ((value) << DMAC_PRICTRL0_LVLPRI0_Pos))
#define DMAC_PRICTRL0_RRLVLEN0_Pos  7            /**< \brief (DMAC_PRICTRL0) Level 0 Round-Robin Scheduling Enable */
#define DMAC_PRICTRL0_RRLVLEN0      (0x1ul << DMAC_PRICTRL0_RRLVLEN0_Pos)
#define DMAC_PRICTRL0_LVLPRI1_Pos   8            /**< \brief (DMAC_PRICTRL0) Level 1 Channel Priority Number */
#define DMAC_PRICTRL0_LVLPRI1_Msk   (0xFul << DMAC_PRICTRL0_LVLPRI1_Pos)
#define DMAC_PRICTRL0_LVLPRI1(value) (DMAC_PRICTRL0_LVLPRI1_Msk & ((value) << DMAC_PRICTRL0_LVLPRI1_Pos))
#define DMAC_PRICTRL0_RRLVLEN1_Pos  15           /**< \brief (DMAC_PRICTRL0) Level 1 Round-Robin Scheduling Enable */
#define DMAC_PRICTRL0_RRLVLEN1      (0x1ul << DMAC_PRICTRL0_RRLVLEN1_Pos)
#define DMAC_PRICTRL0_LVLPRI2_Pos   16           /**< \brief (DMAC_PRICTRL0) Level 2 Channel Priority Number */
#define DMAC_PRICTRL0_LVLPRI2_Msk   (0xFul << DMAC_PRICTRL0_LVLPRI2_Pos)
#define DMAC_PRICTRL0_LVLPRI2(value) (DMAC_PRICTRL0_LVLPRI2_Msk & ((value) << DMAC_PRICTRL0_LVLPRI2_Pos))
#define DMAC_PRICTRL0_RRLVLEN2_Pos  23           /**< \brief (DMAC_PRICTRL0) Level 2 Round-Robin Scheduling Enable */
#define DMAC_PRICTRL0_RRLVLEN2      (0x1ul << DMAC_PRICTRL0_RRLVLEN2_Pos)
#define DMAC_PRICTRL0_LVLPRI3_Pos   24           /**< \brief (DMAC_PRICTRL0) Level 3 Channel Priority Number */
#define DMAC_PRICTRL0_LVLPRI3_Msk   (0xFul << DMAC_PRICTRL0_LVLPRI3_Pos)
#define DMAC_PRICTRL0_LVLPRI3(value) (DMAC_PRICTRL0_LVLPRI3_Msk & ((value) << DMAC_PRICTRL0_LVLPRI3_Pos))
#define DMAC_PRICTRL0_RRLVLEN3_Pos  31           /**< \brief (DMAC_PRICTRL0) Level 3 Round-Robin Scheduling Enable */
#define DMAC_PRICTRL0_RRLVLEN3      (0x1ul << DMAC_PRICTRL0_RRLVLEN3_Pos)
#define DMAC_PRICTRL0_MASK          0x8F8F8F8Ful /**< \brief (DMAC_PRICTRL0) MASK Register */

/* -------- DMAC_INTPEND : (DMAC Offset: 0x20) (R/W 16) Interrupt Pending -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t ID:4;             /*!< bit:  0.. 3  Channel ID                         */
    uint16_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint16_t TERR:1;           /*!< bit:      8  Transfer Error                     */
    uint16_t TCMPL:1;          /*!< bit:      9  Transfer Complete                  */
    uint16_t SUSP:1;           /*!< bit:     10  Channel Suspend                    */
    uint16_t :2;               /*!< bit: 11..12  Reserved                           */
    uint16_t FERR:1;           /*!< bit:     13  Fetch Error                        */
    uint16_t BUSY:1;           /*!< bit:     14  Busy                               */
    uint16_t PEND:1;           /*!< bit:     15  Pending                            */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} DMAC_INTPEND_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_INTPEND_OFFSET         0x20         /**< \brief (DMAC_INTPEND offset) Interrupt Pending */
#define DMAC_INTPEND_RESETVALUE     0x0000ul     /**< \brief (DMAC_INTPEND reset_value) Interrupt Pending */

#define DMAC_INTPEND_ID_Pos         0            /**< \brief (DMAC_INTPEND) Channel ID */
#define DMAC_INTPEND_ID_Msk         (0xFul << DMAC_INTPEND_ID_Pos)
#define DMAC_INTPEND_ID(value)      (DMAC_INTPEND_ID_Msk & ((value) << DMAC_INTPEND_ID_Pos))
#define DMAC_INTPEND_TERR_Pos       8            /**< \brief (DMAC_INTPEND) Transfer Error */
#define DMAC_INTPEND_TERR           (0x1ul << DMAC_INTPEND_TERR_Pos)
#define DMAC_INTPEND_TCMPL_Pos      9            /**< \brief (DMAC_INTPEND) Transfer Complete */
#define DMAC_INTPEND_TCMPL          (0x1ul << DMAC_INTPEND_TCMPL_Pos)
#define DMAC_INTPEND_SUSP_Pos       10           /**< \brief (DMAC_INTPEND) Channel Suspend */
#define DMAC_INTPEND_SUSP           (0x1ul << DMAC_INTPEND_SUSP_Pos)
#define DMAC_INTPEND_FERR_Pos       13           /**< \brief (DMAC_INTPEND) Fetch Error */
#define DMAC_INTPEND_FERR           (0x1ul << DMAC_INTPEND_FERR_Pos)
#define DMAC_INTPEND_BUSY_Pos       14           /**< \brief (DMAC_INTPEND) Busy */
#define DMAC_INTPEND_BUSY           (0x1ul << DMAC_INTPEND_BUSY_Pos)
#define DMAC_INTPEND_PEND_Pos       15           /**< \brief (DMAC_INTPEND) Pending */
#define DMAC_INTPEND_PEND           (0x1ul << DMAC_INTPEND_PEND_Pos)
#define DMAC_INTPEND_MASK           0xE70Ful     /**< \brief (DMAC_INTPEND) MASK Register */

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
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t CHINT:12;         /*!< bit:  0..11  Channel x Pending Interrupt        */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_INTSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_INTSTATUS_OFFSET       0x24         /**< \brief (DMAC_INTSTATUS offset) Interrupt Status */
#define DMAC_INTSTATUS_RESETVALUE   0x00000000ul /**< \brief (DMAC_INTSTATUS reset_value) Interrupt Status */

#define DMAC_INTSTATUS_CHINT0_Pos   0            /**< \brief (DMAC_INTSTATUS) Channel 0 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT0       (1 << DMAC_INTSTATUS_CHINT0_Pos)
#define DMAC_INTSTATUS_CHINT1_Pos   1            /**< \brief (DMAC_INTSTATUS) Channel 1 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT1       (1 << DMAC_INTSTATUS_CHINT1_Pos)
#define DMAC_INTSTATUS_CHINT2_Pos   2            /**< \brief (DMAC_INTSTATUS) Channel 2 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT2       (1 << DMAC_INTSTATUS_CHINT2_Pos)
#define DMAC_INTSTATUS_CHINT3_Pos   3            /**< \brief (DMAC_INTSTATUS) Channel 3 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT3       (1 << DMAC_INTSTATUS_CHINT3_Pos)
#define DMAC_INTSTATUS_CHINT4_Pos   4            /**< \brief (DMAC_INTSTATUS) Channel 4 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT4       (1 << DMAC_INTSTATUS_CHINT4_Pos)
#define DMAC_INTSTATUS_CHINT5_Pos   5            /**< \brief (DMAC_INTSTATUS) Channel 5 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT5       (1 << DMAC_INTSTATUS_CHINT5_Pos)
#define DMAC_INTSTATUS_CHINT6_Pos   6            /**< \brief (DMAC_INTSTATUS) Channel 6 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT6       (1 << DMAC_INTSTATUS_CHINT6_Pos)
#define DMAC_INTSTATUS_CHINT7_Pos   7            /**< \brief (DMAC_INTSTATUS) Channel 7 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT7       (1 << DMAC_INTSTATUS_CHINT7_Pos)
#define DMAC_INTSTATUS_CHINT8_Pos   8            /**< \brief (DMAC_INTSTATUS) Channel 8 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT8       (1 << DMAC_INTSTATUS_CHINT8_Pos)
#define DMAC_INTSTATUS_CHINT9_Pos   9            /**< \brief (DMAC_INTSTATUS) Channel 9 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT9       (1 << DMAC_INTSTATUS_CHINT9_Pos)
#define DMAC_INTSTATUS_CHINT10_Pos  10           /**< \brief (DMAC_INTSTATUS) Channel 10 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT10      (1 << DMAC_INTSTATUS_CHINT10_Pos)
#define DMAC_INTSTATUS_CHINT11_Pos  11           /**< \brief (DMAC_INTSTATUS) Channel 11 Pending Interrupt */
#define DMAC_INTSTATUS_CHINT11      (1 << DMAC_INTSTATUS_CHINT11_Pos)
#define DMAC_INTSTATUS_CHINT_Pos    0            /**< \brief (DMAC_INTSTATUS) Channel x Pending Interrupt */
#define DMAC_INTSTATUS_CHINT_Msk    (0xFFFul << DMAC_INTSTATUS_CHINT_Pos)
#define DMAC_INTSTATUS_CHINT(value) (DMAC_INTSTATUS_CHINT_Msk & ((value) << DMAC_INTSTATUS_CHINT_Pos))
#define DMAC_INTSTATUS_MASK         0x00000FFFul /**< \brief (DMAC_INTSTATUS) MASK Register */

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
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t BUSYCH:12;        /*!< bit:  0..11  Busy Channel x                     */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_BUSYCH_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_BUSYCH_OFFSET          0x28         /**< \brief (DMAC_BUSYCH offset) Busy Channels */
#define DMAC_BUSYCH_RESETVALUE      0x00000000ul /**< \brief (DMAC_BUSYCH reset_value) Busy Channels */

#define DMAC_BUSYCH_BUSYCH0_Pos     0            /**< \brief (DMAC_BUSYCH) Busy Channel 0 */
#define DMAC_BUSYCH_BUSYCH0         (1 << DMAC_BUSYCH_BUSYCH0_Pos)
#define DMAC_BUSYCH_BUSYCH1_Pos     1            /**< \brief (DMAC_BUSYCH) Busy Channel 1 */
#define DMAC_BUSYCH_BUSYCH1         (1 << DMAC_BUSYCH_BUSYCH1_Pos)
#define DMAC_BUSYCH_BUSYCH2_Pos     2            /**< \brief (DMAC_BUSYCH) Busy Channel 2 */
#define DMAC_BUSYCH_BUSYCH2         (1 << DMAC_BUSYCH_BUSYCH2_Pos)
#define DMAC_BUSYCH_BUSYCH3_Pos     3            /**< \brief (DMAC_BUSYCH) Busy Channel 3 */
#define DMAC_BUSYCH_BUSYCH3         (1 << DMAC_BUSYCH_BUSYCH3_Pos)
#define DMAC_BUSYCH_BUSYCH4_Pos     4            /**< \brief (DMAC_BUSYCH) Busy Channel 4 */
#define DMAC_BUSYCH_BUSYCH4         (1 << DMAC_BUSYCH_BUSYCH4_Pos)
#define DMAC_BUSYCH_BUSYCH5_Pos     5            /**< \brief (DMAC_BUSYCH) Busy Channel 5 */
#define DMAC_BUSYCH_BUSYCH5         (1 << DMAC_BUSYCH_BUSYCH5_Pos)
#define DMAC_BUSYCH_BUSYCH6_Pos     6            /**< \brief (DMAC_BUSYCH) Busy Channel 6 */
#define DMAC_BUSYCH_BUSYCH6         (1 << DMAC_BUSYCH_BUSYCH6_Pos)
#define DMAC_BUSYCH_BUSYCH7_Pos     7            /**< \brief (DMAC_BUSYCH) Busy Channel 7 */
#define DMAC_BUSYCH_BUSYCH7         (1 << DMAC_BUSYCH_BUSYCH7_Pos)
#define DMAC_BUSYCH_BUSYCH8_Pos     8            /**< \brief (DMAC_BUSYCH) Busy Channel 8 */
#define DMAC_BUSYCH_BUSYCH8         (1 << DMAC_BUSYCH_BUSYCH8_Pos)
#define DMAC_BUSYCH_BUSYCH9_Pos     9            /**< \brief (DMAC_BUSYCH) Busy Channel 9 */
#define DMAC_BUSYCH_BUSYCH9         (1 << DMAC_BUSYCH_BUSYCH9_Pos)
#define DMAC_BUSYCH_BUSYCH10_Pos    10           /**< \brief (DMAC_BUSYCH) Busy Channel 10 */
#define DMAC_BUSYCH_BUSYCH10        (1 << DMAC_BUSYCH_BUSYCH10_Pos)
#define DMAC_BUSYCH_BUSYCH11_Pos    11           /**< \brief (DMAC_BUSYCH) Busy Channel 11 */
#define DMAC_BUSYCH_BUSYCH11        (1 << DMAC_BUSYCH_BUSYCH11_Pos)
#define DMAC_BUSYCH_BUSYCH_Pos      0            /**< \brief (DMAC_BUSYCH) Busy Channel x */
#define DMAC_BUSYCH_BUSYCH_Msk      (0xFFFul << DMAC_BUSYCH_BUSYCH_Pos)
#define DMAC_BUSYCH_BUSYCH(value)   (DMAC_BUSYCH_BUSYCH_Msk & ((value) << DMAC_BUSYCH_BUSYCH_Pos))
#define DMAC_BUSYCH_MASK            0x00000FFFul /**< \brief (DMAC_BUSYCH) MASK Register */

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
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t PENDCH:12;        /*!< bit:  0..11  Pending Channel x                  */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_PENDCH_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_PENDCH_OFFSET          0x2C         /**< \brief (DMAC_PENDCH offset) Pending Channels */
#define DMAC_PENDCH_RESETVALUE      0x00000000ul /**< \brief (DMAC_PENDCH reset_value) Pending Channels */

#define DMAC_PENDCH_PENDCH0_Pos     0            /**< \brief (DMAC_PENDCH) Pending Channel 0 */
#define DMAC_PENDCH_PENDCH0         (1 << DMAC_PENDCH_PENDCH0_Pos)
#define DMAC_PENDCH_PENDCH1_Pos     1            /**< \brief (DMAC_PENDCH) Pending Channel 1 */
#define DMAC_PENDCH_PENDCH1         (1 << DMAC_PENDCH_PENDCH1_Pos)
#define DMAC_PENDCH_PENDCH2_Pos     2            /**< \brief (DMAC_PENDCH) Pending Channel 2 */
#define DMAC_PENDCH_PENDCH2         (1 << DMAC_PENDCH_PENDCH2_Pos)
#define DMAC_PENDCH_PENDCH3_Pos     3            /**< \brief (DMAC_PENDCH) Pending Channel 3 */
#define DMAC_PENDCH_PENDCH3         (1 << DMAC_PENDCH_PENDCH3_Pos)
#define DMAC_PENDCH_PENDCH4_Pos     4            /**< \brief (DMAC_PENDCH) Pending Channel 4 */
#define DMAC_PENDCH_PENDCH4         (1 << DMAC_PENDCH_PENDCH4_Pos)
#define DMAC_PENDCH_PENDCH5_Pos     5            /**< \brief (DMAC_PENDCH) Pending Channel 5 */
#define DMAC_PENDCH_PENDCH5         (1 << DMAC_PENDCH_PENDCH5_Pos)
#define DMAC_PENDCH_PENDCH6_Pos     6            /**< \brief (DMAC_PENDCH) Pending Channel 6 */
#define DMAC_PENDCH_PENDCH6         (1 << DMAC_PENDCH_PENDCH6_Pos)
#define DMAC_PENDCH_PENDCH7_Pos     7            /**< \brief (DMAC_PENDCH) Pending Channel 7 */
#define DMAC_PENDCH_PENDCH7         (1 << DMAC_PENDCH_PENDCH7_Pos)
#define DMAC_PENDCH_PENDCH8_Pos     8            /**< \brief (DMAC_PENDCH) Pending Channel 8 */
#define DMAC_PENDCH_PENDCH8         (1 << DMAC_PENDCH_PENDCH8_Pos)
#define DMAC_PENDCH_PENDCH9_Pos     9            /**< \brief (DMAC_PENDCH) Pending Channel 9 */
#define DMAC_PENDCH_PENDCH9         (1 << DMAC_PENDCH_PENDCH9_Pos)
#define DMAC_PENDCH_PENDCH10_Pos    10           /**< \brief (DMAC_PENDCH) Pending Channel 10 */
#define DMAC_PENDCH_PENDCH10        (1 << DMAC_PENDCH_PENDCH10_Pos)
#define DMAC_PENDCH_PENDCH11_Pos    11           /**< \brief (DMAC_PENDCH) Pending Channel 11 */
#define DMAC_PENDCH_PENDCH11        (1 << DMAC_PENDCH_PENDCH11_Pos)
#define DMAC_PENDCH_PENDCH_Pos      0            /**< \brief (DMAC_PENDCH) Pending Channel x */
#define DMAC_PENDCH_PENDCH_Msk      (0xFFFul << DMAC_PENDCH_PENDCH_Pos)
#define DMAC_PENDCH_PENDCH(value)   (DMAC_PENDCH_PENDCH_Msk & ((value) << DMAC_PENDCH_PENDCH_Pos))
#define DMAC_PENDCH_MASK            0x00000FFFul /**< \brief (DMAC_PENDCH) MASK Register */

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
#define DMAC_ACTIVE_RESETVALUE      0x00000000ul /**< \brief (DMAC_ACTIVE reset_value) Active Channel and Levels */

#define DMAC_ACTIVE_LVLEX0_Pos      0            /**< \brief (DMAC_ACTIVE) Level 0 Channel Trigger Request Executing */
#define DMAC_ACTIVE_LVLEX0          (1 << DMAC_ACTIVE_LVLEX0_Pos)
#define DMAC_ACTIVE_LVLEX1_Pos      1            /**< \brief (DMAC_ACTIVE) Level 1 Channel Trigger Request Executing */
#define DMAC_ACTIVE_LVLEX1          (1 << DMAC_ACTIVE_LVLEX1_Pos)
#define DMAC_ACTIVE_LVLEX2_Pos      2            /**< \brief (DMAC_ACTIVE) Level 2 Channel Trigger Request Executing */
#define DMAC_ACTIVE_LVLEX2          (1 << DMAC_ACTIVE_LVLEX2_Pos)
#define DMAC_ACTIVE_LVLEX3_Pos      3            /**< \brief (DMAC_ACTIVE) Level 3 Channel Trigger Request Executing */
#define DMAC_ACTIVE_LVLEX3          (1 << DMAC_ACTIVE_LVLEX3_Pos)
#define DMAC_ACTIVE_LVLEX_Pos       0            /**< \brief (DMAC_ACTIVE) Level x Channel Trigger Request Executing */
#define DMAC_ACTIVE_LVLEX_Msk       (0xFul << DMAC_ACTIVE_LVLEX_Pos)
#define DMAC_ACTIVE_LVLEX(value)    (DMAC_ACTIVE_LVLEX_Msk & ((value) << DMAC_ACTIVE_LVLEX_Pos))
#define DMAC_ACTIVE_ID_Pos          8            /**< \brief (DMAC_ACTIVE) Active Channel ID */
#define DMAC_ACTIVE_ID_Msk          (0x1Ful << DMAC_ACTIVE_ID_Pos)
#define DMAC_ACTIVE_ID(value)       (DMAC_ACTIVE_ID_Msk & ((value) << DMAC_ACTIVE_ID_Pos))
#define DMAC_ACTIVE_ABUSY_Pos       15           /**< \brief (DMAC_ACTIVE) Active Channel Busy */
#define DMAC_ACTIVE_ABUSY           (0x1ul << DMAC_ACTIVE_ABUSY_Pos)
#define DMAC_ACTIVE_BTCNT_Pos       16           /**< \brief (DMAC_ACTIVE) Active Channel Block Transfer Count */
#define DMAC_ACTIVE_BTCNT_Msk       (0xFFFFul << DMAC_ACTIVE_BTCNT_Pos)
#define DMAC_ACTIVE_BTCNT(value)    (DMAC_ACTIVE_BTCNT_Msk & ((value) << DMAC_ACTIVE_BTCNT_Pos))
#define DMAC_ACTIVE_MASK            0xFFFF9F0Ful /**< \brief (DMAC_ACTIVE) MASK Register */

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
#define DMAC_BASEADDR_RESETVALUE    0x00000000ul /**< \brief (DMAC_BASEADDR reset_value) Descriptor Memory Section Base Address */

#define DMAC_BASEADDR_BASEADDR_Pos  0            /**< \brief (DMAC_BASEADDR) Descriptor Memory Base Address */
#define DMAC_BASEADDR_BASEADDR_Msk  (0xFFFFFFFFul << DMAC_BASEADDR_BASEADDR_Pos)
#define DMAC_BASEADDR_BASEADDR(value) (DMAC_BASEADDR_BASEADDR_Msk & ((value) << DMAC_BASEADDR_BASEADDR_Pos))
#define DMAC_BASEADDR_MASK          0xFFFFFFFFul /**< \brief (DMAC_BASEADDR) MASK Register */

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
#define DMAC_WRBADDR_RESETVALUE     0x00000000ul /**< \brief (DMAC_WRBADDR reset_value) Write-Back Memory Section Base Address */

#define DMAC_WRBADDR_WRBADDR_Pos    0            /**< \brief (DMAC_WRBADDR) Write-Back Memory Base Address */
#define DMAC_WRBADDR_WRBADDR_Msk    (0xFFFFFFFFul << DMAC_WRBADDR_WRBADDR_Pos)
#define DMAC_WRBADDR_WRBADDR(value) (DMAC_WRBADDR_WRBADDR_Msk & ((value) << DMAC_WRBADDR_WRBADDR_Pos))
#define DMAC_WRBADDR_MASK           0xFFFFFFFFul /**< \brief (DMAC_WRBADDR) MASK Register */

/* -------- DMAC_CHID : (DMAC Offset: 0x3F) (R/W  8) Channel ID -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  ID:4;             /*!< bit:  0.. 3  Channel ID                         */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_CHID_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CHID_OFFSET            0x3F         /**< \brief (DMAC_CHID offset) Channel ID */
#define DMAC_CHID_RESETVALUE        0x00ul       /**< \brief (DMAC_CHID reset_value) Channel ID */

#define DMAC_CHID_ID_Pos            0            /**< \brief (DMAC_CHID) Channel ID */
#define DMAC_CHID_ID_Msk            (0xFul << DMAC_CHID_ID_Pos)
#define DMAC_CHID_ID(value)         (DMAC_CHID_ID_Msk & ((value) << DMAC_CHID_ID_Pos))
#define DMAC_CHID_MASK              0x0Ful       /**< \brief (DMAC_CHID) MASK Register */

/* -------- DMAC_CHCTRLA : (DMAC Offset: 0x40) (R/W  8) Channel Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Channel Software Reset             */
    uint8_t  ENABLE:1;         /*!< bit:      1  Channel Enable                     */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_CHCTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CHCTRLA_OFFSET         0x40         /**< \brief (DMAC_CHCTRLA offset) Channel Control A */
#define DMAC_CHCTRLA_RESETVALUE     0x00ul       /**< \brief (DMAC_CHCTRLA reset_value) Channel Control A */

#define DMAC_CHCTRLA_SWRST_Pos      0            /**< \brief (DMAC_CHCTRLA) Channel Software Reset */
#define DMAC_CHCTRLA_SWRST          (0x1ul << DMAC_CHCTRLA_SWRST_Pos)
#define DMAC_CHCTRLA_ENABLE_Pos     1            /**< \brief (DMAC_CHCTRLA) Channel Enable */
#define DMAC_CHCTRLA_ENABLE         (0x1ul << DMAC_CHCTRLA_ENABLE_Pos)
#define DMAC_CHCTRLA_MASK           0x03ul       /**< \brief (DMAC_CHCTRLA) MASK Register */

/* -------- DMAC_CHCTRLB : (DMAC Offset: 0x44) (R/W 32) Channel Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EVACT:3;          /*!< bit:  0.. 2  Event Input Action                 */
    uint32_t EVIE:1;           /*!< bit:      3  Channel Event Input Enable         */
    uint32_t EVOE:1;           /*!< bit:      4  Channel Event Output Enable        */
    uint32_t LVL:2;            /*!< bit:  5.. 6  Channel Arbitration Level          */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t TRIGSRC:6;        /*!< bit:  8..13  Peripheral Trigger Source          */
    uint32_t :8;               /*!< bit: 14..21  Reserved                           */
    uint32_t TRIGACT:2;        /*!< bit: 22..23  Trigger Action                     */
    uint32_t CMD:2;            /*!< bit: 24..25  Software Command                   */
    uint32_t :6;               /*!< bit: 26..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_CHCTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CHCTRLB_OFFSET         0x44         /**< \brief (DMAC_CHCTRLB offset) Channel Control B */
#define DMAC_CHCTRLB_RESETVALUE     0x00000000ul /**< \brief (DMAC_CHCTRLB reset_value) Channel Control B */

#define DMAC_CHCTRLB_EVACT_Pos      0            /**< \brief (DMAC_CHCTRLB) Event Input Action */
#define DMAC_CHCTRLB_EVACT_Msk      (0x7ul << DMAC_CHCTRLB_EVACT_Pos)
#define DMAC_CHCTRLB_EVACT(value)   (DMAC_CHCTRLB_EVACT_Msk & ((value) << DMAC_CHCTRLB_EVACT_Pos))
#define   DMAC_CHCTRLB_EVACT_NOACT_Val    0x0ul  /**< \brief (DMAC_CHCTRLB) No action */
#define   DMAC_CHCTRLB_EVACT_TRIG_Val     0x1ul  /**< \brief (DMAC_CHCTRLB) Transfer and periodic transfer trigger */
#define   DMAC_CHCTRLB_EVACT_CTRIG_Val    0x2ul  /**< \brief (DMAC_CHCTRLB) Conditional transfer trigger */
#define   DMAC_CHCTRLB_EVACT_CBLOCK_Val   0x3ul  /**< \brief (DMAC_CHCTRLB) Conditional block transfer */
#define   DMAC_CHCTRLB_EVACT_SUSPEND_Val  0x4ul  /**< \brief (DMAC_CHCTRLB) Channel suspend operation */
#define   DMAC_CHCTRLB_EVACT_RESUME_Val   0x5ul  /**< \brief (DMAC_CHCTRLB) Channel resume operation */
#define   DMAC_CHCTRLB_EVACT_SSKIP_Val    0x6ul  /**< \brief (DMAC_CHCTRLB) Skip next block suspend action */
#define DMAC_CHCTRLB_EVACT_NOACT    (DMAC_CHCTRLB_EVACT_NOACT_Val  << DMAC_CHCTRLB_EVACT_Pos)
#define DMAC_CHCTRLB_EVACT_TRIG     (DMAC_CHCTRLB_EVACT_TRIG_Val   << DMAC_CHCTRLB_EVACT_Pos)
#define DMAC_CHCTRLB_EVACT_CTRIG    (DMAC_CHCTRLB_EVACT_CTRIG_Val  << DMAC_CHCTRLB_EVACT_Pos)
#define DMAC_CHCTRLB_EVACT_CBLOCK   (DMAC_CHCTRLB_EVACT_CBLOCK_Val << DMAC_CHCTRLB_EVACT_Pos)
#define DMAC_CHCTRLB_EVACT_SUSPEND  (DMAC_CHCTRLB_EVACT_SUSPEND_Val << DMAC_CHCTRLB_EVACT_Pos)
#define DMAC_CHCTRLB_EVACT_RESUME   (DMAC_CHCTRLB_EVACT_RESUME_Val << DMAC_CHCTRLB_EVACT_Pos)
#define DMAC_CHCTRLB_EVACT_SSKIP    (DMAC_CHCTRLB_EVACT_SSKIP_Val  << DMAC_CHCTRLB_EVACT_Pos)
#define DMAC_CHCTRLB_EVIE_Pos       3            /**< \brief (DMAC_CHCTRLB) Channel Event Input Enable */
#define DMAC_CHCTRLB_EVIE           (0x1ul << DMAC_CHCTRLB_EVIE_Pos)
#define DMAC_CHCTRLB_EVOE_Pos       4            /**< \brief (DMAC_CHCTRLB) Channel Event Output Enable */
#define DMAC_CHCTRLB_EVOE           (0x1ul << DMAC_CHCTRLB_EVOE_Pos)
#define DMAC_CHCTRLB_LVL_Pos        5            /**< \brief (DMAC_CHCTRLB) Channel Arbitration Level */
#define DMAC_CHCTRLB_LVL_Msk        (0x3ul << DMAC_CHCTRLB_LVL_Pos)
#define DMAC_CHCTRLB_LVL(value)     (DMAC_CHCTRLB_LVL_Msk & ((value) << DMAC_CHCTRLB_LVL_Pos))
#define   DMAC_CHCTRLB_LVL_LVL0_Val       0x0ul  /**< \brief (DMAC_CHCTRLB) Channel Priority Level 0 */
#define   DMAC_CHCTRLB_LVL_LVL1_Val       0x1ul  /**< \brief (DMAC_CHCTRLB) Channel Priority Level 1 */
#define   DMAC_CHCTRLB_LVL_LVL2_Val       0x2ul  /**< \brief (DMAC_CHCTRLB) Channel Priority Level 2 */
#define   DMAC_CHCTRLB_LVL_LVL3_Val       0x3ul  /**< \brief (DMAC_CHCTRLB) Channel Priority Level 3 */
#define DMAC_CHCTRLB_LVL_LVL0       (DMAC_CHCTRLB_LVL_LVL0_Val     << DMAC_CHCTRLB_LVL_Pos)
#define DMAC_CHCTRLB_LVL_LVL1       (DMAC_CHCTRLB_LVL_LVL1_Val     << DMAC_CHCTRLB_LVL_Pos)
#define DMAC_CHCTRLB_LVL_LVL2       (DMAC_CHCTRLB_LVL_LVL2_Val     << DMAC_CHCTRLB_LVL_Pos)
#define DMAC_CHCTRLB_LVL_LVL3       (DMAC_CHCTRLB_LVL_LVL3_Val     << DMAC_CHCTRLB_LVL_Pos)
#define DMAC_CHCTRLB_TRIGSRC_Pos    8            /**< \brief (DMAC_CHCTRLB) Peripheral Trigger Source */
#define DMAC_CHCTRLB_TRIGSRC_Msk    (0x3Ful << DMAC_CHCTRLB_TRIGSRC_Pos)
#define DMAC_CHCTRLB_TRIGSRC(value) (DMAC_CHCTRLB_TRIGSRC_Msk & ((value) << DMAC_CHCTRLB_TRIGSRC_Pos))
#define   DMAC_CHCTRLB_TRIGSRC_DISABLE_Val 0x0ul  /**< \brief (DMAC_CHCTRLB) Only software/event triggers */
#define DMAC_CHCTRLB_TRIGSRC_DISABLE (DMAC_CHCTRLB_TRIGSRC_DISABLE_Val << DMAC_CHCTRLB_TRIGSRC_Pos)
#define DMAC_CHCTRLB_TRIGACT_Pos    22           /**< \brief (DMAC_CHCTRLB) Trigger Action */
#define DMAC_CHCTRLB_TRIGACT_Msk    (0x3ul << DMAC_CHCTRLB_TRIGACT_Pos)
#define DMAC_CHCTRLB_TRIGACT(value) (DMAC_CHCTRLB_TRIGACT_Msk & ((value) << DMAC_CHCTRLB_TRIGACT_Pos))
#define   DMAC_CHCTRLB_TRIGACT_BLOCK_Val  0x0ul  /**< \brief (DMAC_CHCTRLB) One trigger required for each block transfer */
#define   DMAC_CHCTRLB_TRIGACT_BEAT_Val   0x2ul  /**< \brief (DMAC_CHCTRLB) One trigger required for each beat transfer */
#define   DMAC_CHCTRLB_TRIGACT_TRANSACTION_Val 0x3ul  /**< \brief (DMAC_CHCTRLB) One trigger required for each transaction */
#define DMAC_CHCTRLB_TRIGACT_BLOCK  (DMAC_CHCTRLB_TRIGACT_BLOCK_Val << DMAC_CHCTRLB_TRIGACT_Pos)
#define DMAC_CHCTRLB_TRIGACT_BEAT   (DMAC_CHCTRLB_TRIGACT_BEAT_Val << DMAC_CHCTRLB_TRIGACT_Pos)
#define DMAC_CHCTRLB_TRIGACT_TRANSACTION (DMAC_CHCTRLB_TRIGACT_TRANSACTION_Val << DMAC_CHCTRLB_TRIGACT_Pos)
#define DMAC_CHCTRLB_CMD_Pos        24           /**< \brief (DMAC_CHCTRLB) Software Command */
#define DMAC_CHCTRLB_CMD_Msk        (0x3ul << DMAC_CHCTRLB_CMD_Pos)
#define DMAC_CHCTRLB_CMD(value)     (DMAC_CHCTRLB_CMD_Msk & ((value) << DMAC_CHCTRLB_CMD_Pos))
#define   DMAC_CHCTRLB_CMD_NOACT_Val      0x0ul  /**< \brief (DMAC_CHCTRLB) No action */
#define   DMAC_CHCTRLB_CMD_SUSPEND_Val    0x1ul  /**< \brief (DMAC_CHCTRLB) Channel suspend operation */
#define   DMAC_CHCTRLB_CMD_RESUME_Val     0x2ul  /**< \brief (DMAC_CHCTRLB) Channel resume operation */
#define DMAC_CHCTRLB_CMD_NOACT      (DMAC_CHCTRLB_CMD_NOACT_Val    << DMAC_CHCTRLB_CMD_Pos)
#define DMAC_CHCTRLB_CMD_SUSPEND    (DMAC_CHCTRLB_CMD_SUSPEND_Val  << DMAC_CHCTRLB_CMD_Pos)
#define DMAC_CHCTRLB_CMD_RESUME     (DMAC_CHCTRLB_CMD_RESUME_Val   << DMAC_CHCTRLB_CMD_Pos)
#define DMAC_CHCTRLB_MASK           0x03C03F7Ful /**< \brief (DMAC_CHCTRLB) MASK Register */

/* -------- DMAC_CHINTENCLR : (DMAC Offset: 0x4C) (R/W  8) Channel Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  TERR:1;           /*!< bit:      0  Transfer Error Interrupt Enable    */
    uint8_t  TCMPL:1;          /*!< bit:      1  Transfer Complete Interrupt Enable */
    uint8_t  SUSP:1;           /*!< bit:      2  Channel Suspend Interrupt Enable   */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_CHINTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CHINTENCLR_OFFSET      0x4C         /**< \brief (DMAC_CHINTENCLR offset) Channel Interrupt Enable Clear */
#define DMAC_CHINTENCLR_RESETVALUE  0x00ul       /**< \brief (DMAC_CHINTENCLR reset_value) Channel Interrupt Enable Clear */

#define DMAC_CHINTENCLR_TERR_Pos    0            /**< \brief (DMAC_CHINTENCLR) Transfer Error Interrupt Enable */
#define DMAC_CHINTENCLR_TERR        (0x1ul << DMAC_CHINTENCLR_TERR_Pos)
#define DMAC_CHINTENCLR_TCMPL_Pos   1            /**< \brief (DMAC_CHINTENCLR) Transfer Complete Interrupt Enable */
#define DMAC_CHINTENCLR_TCMPL       (0x1ul << DMAC_CHINTENCLR_TCMPL_Pos)
#define DMAC_CHINTENCLR_SUSP_Pos    2            /**< \brief (DMAC_CHINTENCLR) Channel Suspend Interrupt Enable */
#define DMAC_CHINTENCLR_SUSP        (0x1ul << DMAC_CHINTENCLR_SUSP_Pos)
#define DMAC_CHINTENCLR_MASK        0x07ul       /**< \brief (DMAC_CHINTENCLR) MASK Register */

/* -------- DMAC_CHINTENSET : (DMAC Offset: 0x4D) (R/W  8) Channel Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  TERR:1;           /*!< bit:      0  Transfer Error Interrupt Enable    */
    uint8_t  TCMPL:1;          /*!< bit:      1  Transfer Complete Interrupt Enable */
    uint8_t  SUSP:1;           /*!< bit:      2  Channel Suspend Interrupt Enable   */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_CHINTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CHINTENSET_OFFSET      0x4D         /**< \brief (DMAC_CHINTENSET offset) Channel Interrupt Enable Set */
#define DMAC_CHINTENSET_RESETVALUE  0x00ul       /**< \brief (DMAC_CHINTENSET reset_value) Channel Interrupt Enable Set */

#define DMAC_CHINTENSET_TERR_Pos    0            /**< \brief (DMAC_CHINTENSET) Transfer Error Interrupt Enable */
#define DMAC_CHINTENSET_TERR        (0x1ul << DMAC_CHINTENSET_TERR_Pos)
#define DMAC_CHINTENSET_TCMPL_Pos   1            /**< \brief (DMAC_CHINTENSET) Transfer Complete Interrupt Enable */
#define DMAC_CHINTENSET_TCMPL       (0x1ul << DMAC_CHINTENSET_TCMPL_Pos)
#define DMAC_CHINTENSET_SUSP_Pos    2            /**< \brief (DMAC_CHINTENSET) Channel Suspend Interrupt Enable */
#define DMAC_CHINTENSET_SUSP        (0x1ul << DMAC_CHINTENSET_SUSP_Pos)
#define DMAC_CHINTENSET_MASK        0x07ul       /**< \brief (DMAC_CHINTENSET) MASK Register */

/* -------- DMAC_CHINTFLAG : (DMAC Offset: 0x4E) (R/W  8) Channel Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  TERR:1;           /*!< bit:      0  Transfer Error                     */
    __I uint8_t  TCMPL:1;          /*!< bit:      1  Transfer Complete                  */
    __I uint8_t  SUSP:1;           /*!< bit:      2  Channel Suspend                    */
    __I uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_CHINTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CHINTFLAG_OFFSET       0x4E         /**< \brief (DMAC_CHINTFLAG offset) Channel Interrupt Flag Status and Clear */
#define DMAC_CHINTFLAG_RESETVALUE   0x00ul       /**< \brief (DMAC_CHINTFLAG reset_value) Channel Interrupt Flag Status and Clear */

#define DMAC_CHINTFLAG_TERR_Pos     0            /**< \brief (DMAC_CHINTFLAG) Transfer Error */
#define DMAC_CHINTFLAG_TERR         (0x1ul << DMAC_CHINTFLAG_TERR_Pos)
#define DMAC_CHINTFLAG_TCMPL_Pos    1            /**< \brief (DMAC_CHINTFLAG) Transfer Complete */
#define DMAC_CHINTFLAG_TCMPL        (0x1ul << DMAC_CHINTFLAG_TCMPL_Pos)
#define DMAC_CHINTFLAG_SUSP_Pos     2            /**< \brief (DMAC_CHINTFLAG) Channel Suspend */
#define DMAC_CHINTFLAG_SUSP         (0x1ul << DMAC_CHINTFLAG_SUSP_Pos)
#define DMAC_CHINTFLAG_MASK         0x07ul       /**< \brief (DMAC_CHINTFLAG) MASK Register */

/* -------- DMAC_CHSTATUS : (DMAC Offset: 0x4F) (R/   8) Channel Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  PEND:1;           /*!< bit:      0  Channel Pending                    */
    uint8_t  BUSY:1;           /*!< bit:      1  Channel Busy                       */
    uint8_t  FERR:1;           /*!< bit:      2  Fetch Error                        */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DMAC_CHSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_CHSTATUS_OFFSET        0x4F         /**< \brief (DMAC_CHSTATUS offset) Channel Status */
#define DMAC_CHSTATUS_RESETVALUE    0x00ul       /**< \brief (DMAC_CHSTATUS reset_value) Channel Status */

#define DMAC_CHSTATUS_PEND_Pos      0            /**< \brief (DMAC_CHSTATUS) Channel Pending */
#define DMAC_CHSTATUS_PEND          (0x1ul << DMAC_CHSTATUS_PEND_Pos)
#define DMAC_CHSTATUS_BUSY_Pos      1            /**< \brief (DMAC_CHSTATUS) Channel Busy */
#define DMAC_CHSTATUS_BUSY          (0x1ul << DMAC_CHSTATUS_BUSY_Pos)
#define DMAC_CHSTATUS_FERR_Pos      2            /**< \brief (DMAC_CHSTATUS) Fetch Error */
#define DMAC_CHSTATUS_FERR          (0x1ul << DMAC_CHSTATUS_FERR_Pos)
#define DMAC_CHSTATUS_MASK          0x07ul       /**< \brief (DMAC_CHSTATUS) MASK Register */

/* -------- DMAC_BTCTRL : (DMAC Offset: 0x00) (R/W 16) Block Transfer Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t VALID:1;          /*!< bit:      0  Descriptor Valid                   */
    uint16_t EVOSEL:2;         /*!< bit:  1.. 2  Event Output Selection             */
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
#define DMAC_BTCTRL_RESETVALUE      0x0000ul     /**< \brief (DMAC_BTCTRL reset_value) Block Transfer Control */

#define DMAC_BTCTRL_VALID_Pos       0            /**< \brief (DMAC_BTCTRL) Descriptor Valid */
#define DMAC_BTCTRL_VALID           (0x1ul << DMAC_BTCTRL_VALID_Pos)
#define DMAC_BTCTRL_EVOSEL_Pos      1            /**< \brief (DMAC_BTCTRL) Event Output Selection */
#define DMAC_BTCTRL_EVOSEL_Msk      (0x3ul << DMAC_BTCTRL_EVOSEL_Pos)
#define DMAC_BTCTRL_EVOSEL(value)   (DMAC_BTCTRL_EVOSEL_Msk & ((value) << DMAC_BTCTRL_EVOSEL_Pos))
#define   DMAC_BTCTRL_EVOSEL_DISABLE_Val  0x0ul  /**< \brief (DMAC_BTCTRL) Event generation disabled */
#define   DMAC_BTCTRL_EVOSEL_BLOCK_Val    0x1ul  /**< \brief (DMAC_BTCTRL) Event strobe when block transfer complete */
#define   DMAC_BTCTRL_EVOSEL_BEAT_Val     0x3ul  /**< \brief (DMAC_BTCTRL) Event strobe when beat transfer complete */
#define DMAC_BTCTRL_EVOSEL_DISABLE  (DMAC_BTCTRL_EVOSEL_DISABLE_Val << DMAC_BTCTRL_EVOSEL_Pos)
#define DMAC_BTCTRL_EVOSEL_BLOCK    (DMAC_BTCTRL_EVOSEL_BLOCK_Val  << DMAC_BTCTRL_EVOSEL_Pos)
#define DMAC_BTCTRL_EVOSEL_BEAT     (DMAC_BTCTRL_EVOSEL_BEAT_Val   << DMAC_BTCTRL_EVOSEL_Pos)
#define DMAC_BTCTRL_BLOCKACT_Pos    3            /**< \brief (DMAC_BTCTRL) Block Action */
#define DMAC_BTCTRL_BLOCKACT_Msk    (0x3ul << DMAC_BTCTRL_BLOCKACT_Pos)
#define DMAC_BTCTRL_BLOCKACT(value) (DMAC_BTCTRL_BLOCKACT_Msk & ((value) << DMAC_BTCTRL_BLOCKACT_Pos))
#define   DMAC_BTCTRL_BLOCKACT_NOACT_Val  0x0ul  /**< \brief (DMAC_BTCTRL) No action */
#define   DMAC_BTCTRL_BLOCKACT_INT_Val    0x1ul  /**< \brief (DMAC_BTCTRL) Channel in normal operation and block interrupt */
#define   DMAC_BTCTRL_BLOCKACT_SUSPEND_Val 0x2ul  /**< \brief (DMAC_BTCTRL) Channel suspend operation is completed */
#define   DMAC_BTCTRL_BLOCKACT_BOTH_Val   0x3ul  /**< \brief (DMAC_BTCTRL) Both channel suspend operation and block interrupt */
#define DMAC_BTCTRL_BLOCKACT_NOACT  (DMAC_BTCTRL_BLOCKACT_NOACT_Val << DMAC_BTCTRL_BLOCKACT_Pos)
#define DMAC_BTCTRL_BLOCKACT_INT    (DMAC_BTCTRL_BLOCKACT_INT_Val  << DMAC_BTCTRL_BLOCKACT_Pos)
#define DMAC_BTCTRL_BLOCKACT_SUSPEND (DMAC_BTCTRL_BLOCKACT_SUSPEND_Val << DMAC_BTCTRL_BLOCKACT_Pos)
#define DMAC_BTCTRL_BLOCKACT_BOTH   (DMAC_BTCTRL_BLOCKACT_BOTH_Val << DMAC_BTCTRL_BLOCKACT_Pos)
#define DMAC_BTCTRL_BEATSIZE_Pos    8            /**< \brief (DMAC_BTCTRL) Beat Size */
#define DMAC_BTCTRL_BEATSIZE_Msk    (0x3ul << DMAC_BTCTRL_BEATSIZE_Pos)
#define DMAC_BTCTRL_BEATSIZE(value) (DMAC_BTCTRL_BEATSIZE_Msk & ((value) << DMAC_BTCTRL_BEATSIZE_Pos))
#define   DMAC_BTCTRL_BEATSIZE_BYTE_Val   0x0ul  /**< \brief (DMAC_BTCTRL) 8-bit access */
#define   DMAC_BTCTRL_BEATSIZE_HWORD_Val  0x1ul  /**< \brief (DMAC_BTCTRL) 16-bit access */
#define   DMAC_BTCTRL_BEATSIZE_WORD_Val   0x2ul  /**< \brief (DMAC_BTCTRL) 32-bit access */
#define DMAC_BTCTRL_BEATSIZE_BYTE   (DMAC_BTCTRL_BEATSIZE_BYTE_Val << DMAC_BTCTRL_BEATSIZE_Pos)
#define DMAC_BTCTRL_BEATSIZE_HWORD  (DMAC_BTCTRL_BEATSIZE_HWORD_Val << DMAC_BTCTRL_BEATSIZE_Pos)
#define DMAC_BTCTRL_BEATSIZE_WORD   (DMAC_BTCTRL_BEATSIZE_WORD_Val << DMAC_BTCTRL_BEATSIZE_Pos)
#define DMAC_BTCTRL_SRCINC_Pos      10           /**< \brief (DMAC_BTCTRL) Source Address Increment Enable */
#define DMAC_BTCTRL_SRCINC          (0x1ul << DMAC_BTCTRL_SRCINC_Pos)
#define DMAC_BTCTRL_DSTINC_Pos      11           /**< \brief (DMAC_BTCTRL) Destination Address Increment Enable */
#define DMAC_BTCTRL_DSTINC          (0x1ul << DMAC_BTCTRL_DSTINC_Pos)
#define DMAC_BTCTRL_STEPSEL_Pos     12           /**< \brief (DMAC_BTCTRL) Step Selection */
#define DMAC_BTCTRL_STEPSEL         (0x1ul << DMAC_BTCTRL_STEPSEL_Pos)
#define   DMAC_BTCTRL_STEPSEL_DST_Val     0x0ul  /**< \brief (DMAC_BTCTRL) Step size settings apply to the destination address */
#define   DMAC_BTCTRL_STEPSEL_SRC_Val     0x1ul  /**< \brief (DMAC_BTCTRL) Step size settings apply to the source address */
#define DMAC_BTCTRL_STEPSEL_DST     (DMAC_BTCTRL_STEPSEL_DST_Val   << DMAC_BTCTRL_STEPSEL_Pos)
#define DMAC_BTCTRL_STEPSEL_SRC     (DMAC_BTCTRL_STEPSEL_SRC_Val   << DMAC_BTCTRL_STEPSEL_Pos)
#define DMAC_BTCTRL_STEPSIZE_Pos    13           /**< \brief (DMAC_BTCTRL) Address Increment Step Size */
#define DMAC_BTCTRL_STEPSIZE_Msk    (0x7ul << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_STEPSIZE(value) (DMAC_BTCTRL_STEPSIZE_Msk & ((value) << DMAC_BTCTRL_STEPSIZE_Pos))
#define   DMAC_BTCTRL_STEPSIZE_X1_Val     0x0ul  /**< \brief (DMAC_BTCTRL) Next ADDR <- ADDR + (1<<BEATSIZE) * 1 */
#define   DMAC_BTCTRL_STEPSIZE_X2_Val     0x1ul  /**< \brief (DMAC_BTCTRL) Next ADDR <- ADDR + (1<<BEATSIZE) * 2 */
#define   DMAC_BTCTRL_STEPSIZE_X4_Val     0x2ul  /**< \brief (DMAC_BTCTRL) Next ADDR <- ADDR + (1<<BEATSIZE) * 4 */
#define   DMAC_BTCTRL_STEPSIZE_X8_Val     0x3ul  /**< \brief (DMAC_BTCTRL) Next ADDR <- ADDR + (1<<BEATSIZE) * 8 */
#define   DMAC_BTCTRL_STEPSIZE_X16_Val    0x4ul  /**< \brief (DMAC_BTCTRL) Next ADDR <- ADDR + (1<<BEATSIZE) * 16 */
#define   DMAC_BTCTRL_STEPSIZE_X32_Val    0x5ul  /**< \brief (DMAC_BTCTRL) Next ADDR <- ADDR + (1<<BEATSIZE) * 32 */
#define   DMAC_BTCTRL_STEPSIZE_X64_Val    0x6ul  /**< \brief (DMAC_BTCTRL) Next ADDR <- ADDR + (1<<BEATSIZE) * 64 */
#define   DMAC_BTCTRL_STEPSIZE_X128_Val   0x7ul  /**< \brief (DMAC_BTCTRL) Next ADDR <- ADDR + (1<<BEATSIZE) * 128 */
#define DMAC_BTCTRL_STEPSIZE_X1     (DMAC_BTCTRL_STEPSIZE_X1_Val   << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_STEPSIZE_X2     (DMAC_BTCTRL_STEPSIZE_X2_Val   << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_STEPSIZE_X4     (DMAC_BTCTRL_STEPSIZE_X4_Val   << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_STEPSIZE_X8     (DMAC_BTCTRL_STEPSIZE_X8_Val   << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_STEPSIZE_X16    (DMAC_BTCTRL_STEPSIZE_X16_Val  << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_STEPSIZE_X32    (DMAC_BTCTRL_STEPSIZE_X32_Val  << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_STEPSIZE_X64    (DMAC_BTCTRL_STEPSIZE_X64_Val  << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_STEPSIZE_X128   (DMAC_BTCTRL_STEPSIZE_X128_Val << DMAC_BTCTRL_STEPSIZE_Pos)
#define DMAC_BTCTRL_MASK            0xFF1Ful     /**< \brief (DMAC_BTCTRL) MASK Register */

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

#define DMAC_BTCNT_BTCNT_Pos        0            /**< \brief (DMAC_BTCNT) Block Transfer Count */
#define DMAC_BTCNT_BTCNT_Msk        (0xFFFFul << DMAC_BTCNT_BTCNT_Pos)
#define DMAC_BTCNT_BTCNT(value)     (DMAC_BTCNT_BTCNT_Msk & ((value) << DMAC_BTCNT_BTCNT_Pos))
#define DMAC_BTCNT_MASK             0xFFFFul     /**< \brief (DMAC_BTCNT) MASK Register */

/* -------- DMAC_SRCADDR : (DMAC Offset: 0x04) (R/W 32) Transfer Source Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SRCADDR:32;       /*!< bit:  0..31  Transfer Source Address            */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_SRCADDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_SRCADDR_OFFSET         0x04         /**< \brief (DMAC_SRCADDR offset) Transfer Source Address */

#define DMAC_SRCADDR_SRCADDR_Pos    0            /**< \brief (DMAC_SRCADDR) Transfer Source Address */
#define DMAC_SRCADDR_SRCADDR_Msk    (0xFFFFFFFFul << DMAC_SRCADDR_SRCADDR_Pos)
#define DMAC_SRCADDR_SRCADDR(value) (DMAC_SRCADDR_SRCADDR_Msk & ((value) << DMAC_SRCADDR_SRCADDR_Pos))
#define DMAC_SRCADDR_MASK           0xFFFFFFFFul /**< \brief (DMAC_SRCADDR) MASK Register */

/* -------- DMAC_DSTADDR : (DMAC Offset: 0x08) (R/W 32) Transfer Destination Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DSTADDR:32;       /*!< bit:  0..31  Transfer Destination Address       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DMAC_DSTADDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DMAC_DSTADDR_OFFSET         0x08         /**< \brief (DMAC_DSTADDR offset) Transfer Destination Address */

#define DMAC_DSTADDR_DSTADDR_Pos    0            /**< \brief (DMAC_DSTADDR) Transfer Destination Address */
#define DMAC_DSTADDR_DSTADDR_Msk    (0xFFFFFFFFul << DMAC_DSTADDR_DSTADDR_Pos)
#define DMAC_DSTADDR_DSTADDR(value) (DMAC_DSTADDR_DSTADDR_Msk & ((value) << DMAC_DSTADDR_DSTADDR_Pos))
#define DMAC_DSTADDR_MASK           0xFFFFFFFFul /**< \brief (DMAC_DSTADDR) MASK Register */

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
#define DMAC_DESCADDR_DESCADDR_Msk  (0xFFFFFFFFul << DMAC_DESCADDR_DESCADDR_Pos)
#define DMAC_DESCADDR_DESCADDR(value) (DMAC_DESCADDR_DESCADDR_Msk & ((value) << DMAC_DESCADDR_DESCADDR_Pos))
#define DMAC_DESCADDR_MASK          0xFFFFFFFFul /**< \brief (DMAC_DESCADDR) MASK Register */

/** \brief DMAC APB hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO DMAC_CTRL_Type            CTRL;        /**< \brief Offset: 0x00 (R/W 16) Control */
  __IO DMAC_CRCCTRL_Type         CRCCTRL;     /**< \brief Offset: 0x02 (R/W 16) CRC Control */
  __IO DMAC_CRCDATAIN_Type       CRCDATAIN;   /**< \brief Offset: 0x04 (R/W 32) CRC Data Input */
  __IO DMAC_CRCCHKSUM_Type       CRCCHKSUM;   /**< \brief Offset: 0x08 (R/W 32) CRC Checksum */
  __IO DMAC_CRCSTATUS_Type       CRCSTATUS;   /**< \brief Offset: 0x0C (R/W  8) CRC Status */
  __IO DMAC_DBGCTRL_Type         DBGCTRL;     /**< \brief Offset: 0x0D (R/W  8) Debug Control */
  __IO DMAC_QOSCTRL_Type         QOSCTRL;     /**< \brief Offset: 0x0E (R/W  8) QOS Control */
       RoReg8                    Reserved1[0x1];
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
       RoReg8                    Reserved4[0x3];
  __IO DMAC_CHID_Type            CHID;        /**< \brief Offset: 0x3F (R/W  8) Channel ID */
  __IO DMAC_CHCTRLA_Type         CHCTRLA;     /**< \brief Offset: 0x40 (R/W  8) Channel Control A */
       RoReg8                    Reserved5[0x3];
  __IO DMAC_CHCTRLB_Type         CHCTRLB;     /**< \brief Offset: 0x44 (R/W 32) Channel Control B */
       RoReg8                    Reserved6[0x4];
  __IO DMAC_CHINTENCLR_Type      CHINTENCLR;  /**< \brief Offset: 0x4C (R/W  8) Channel Interrupt Enable Clear */
  __IO DMAC_CHINTENSET_Type      CHINTENSET;  /**< \brief Offset: 0x4D (R/W  8) Channel Interrupt Enable Set */
  __IO DMAC_CHINTFLAG_Type       CHINTFLAG;   /**< \brief Offset: 0x4E (R/W  8) Channel Interrupt Flag Status and Clear */
  __I  DMAC_CHSTATUS_Type        CHSTATUS;    /**< \brief Offset: 0x4F (R/   8) Channel Status */
} Dmac;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief DMAC Descriptor SRAM registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO DMAC_BTCTRL_Type          BTCTRL;      /**< \brief Offset: 0x00 (R/W 16) Block Transfer Control */
  __IO DMAC_BTCNT_Type           BTCNT;       /**< \brief Offset: 0x02 (R/W 16) Block Transfer Count */
  __IO DMAC_SRCADDR_Type         SRCADDR;     /**< \brief Offset: 0x04 (R/W 32) Transfer Source Address */
  __IO DMAC_DSTADDR_Type         DSTADDR;     /**< \brief Offset: 0x08 (R/W 32) Transfer Destination Address */
  __IO DMAC_DESCADDR_Type        DESCADDR;    /**< \brief Offset: 0x0C (R/W 32) Next Descriptor Address */
} DmacDescriptor
#ifdef __GNUC__
  __attribute__ ((aligned (8)))
#endif
;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SECTION_DMAC_DESCRIPTOR

/*@}*/

#endif /* _SAMD21_DMAC_COMPONENT_ */
