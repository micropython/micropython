/**
 * \file
 *
 * \brief Component description for QSPI
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

#ifndef _SAMD51_QSPI_COMPONENT_
#define _SAMD51_QSPI_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR QSPI */
/* ========================================================================== */
/** \addtogroup SAMD51_QSPI Quad SPI interface */
/*@{*/

#define QSPI_U2008
#define REV_QSPI                    0x163

/* -------- QSPI_CTRLA : (QSPI Offset: 0x00) (R/W 32) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint32_t :22;              /*!< bit:  2..23  Reserved                           */
    uint32_t LASTXFER:1;       /*!< bit:     24  Last Transfer                      */
    uint32_t :7;               /*!< bit: 25..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} QSPI_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define QSPI_CTRLA_OFFSET           0x00         /**< \brief (QSPI_CTRLA offset) Control A */
#define QSPI_CTRLA_RESETVALUE       _U_(0x00000000) /**< \brief (QSPI_CTRLA reset_value) Control A */

#define QSPI_CTRLA_SWRST_Pos        0            /**< \brief (QSPI_CTRLA) Software Reset */
#define QSPI_CTRLA_SWRST            (_U_(0x1) << QSPI_CTRLA_SWRST_Pos)
#define QSPI_CTRLA_ENABLE_Pos       1            /**< \brief (QSPI_CTRLA) Enable */
#define QSPI_CTRLA_ENABLE           (_U_(0x1) << QSPI_CTRLA_ENABLE_Pos)
#define QSPI_CTRLA_LASTXFER_Pos     24           /**< \brief (QSPI_CTRLA) Last Transfer */
#define QSPI_CTRLA_LASTXFER         (_U_(0x1) << QSPI_CTRLA_LASTXFER_Pos)
#define QSPI_CTRLA_MASK             _U_(0x01000003) /**< \brief (QSPI_CTRLA) MASK Register */

/* -------- QSPI_CTRLB : (QSPI Offset: 0x04) (R/W 32) Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t MODE:1;           /*!< bit:      0  Serial Memory Mode                 */
    uint32_t LOOPEN:1;         /*!< bit:      1  Local Loopback Enable              */
    uint32_t WDRBT:1;          /*!< bit:      2  Wait Data Read Before Transfer     */
    uint32_t SMEMREG:1;        /*!< bit:      3  Serial Memory reg                  */
    uint32_t CSMODE:2;         /*!< bit:  4.. 5  Chip Select Mode                   */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t DATALEN:4;        /*!< bit:  8..11  Data Length                        */
    uint32_t :4;               /*!< bit: 12..15  Reserved                           */
    uint32_t DLYBCT:8;         /*!< bit: 16..23  Delay Between Consecutive Transfers */
    uint32_t DLYCS:8;          /*!< bit: 24..31  Minimum Inactive CS Delay          */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} QSPI_CTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define QSPI_CTRLB_OFFSET           0x04         /**< \brief (QSPI_CTRLB offset) Control B */
#define QSPI_CTRLB_RESETVALUE       _U_(0x00000000) /**< \brief (QSPI_CTRLB reset_value) Control B */

#define QSPI_CTRLB_MODE_Pos         0            /**< \brief (QSPI_CTRLB) Serial Memory Mode */
#define QSPI_CTRLB_MODE             (_U_(0x1) << QSPI_CTRLB_MODE_Pos)
#define   QSPI_CTRLB_MODE_SPI_Val         _U_(0x0)   /**< \brief (QSPI_CTRLB) SPI operating mode */
#define   QSPI_CTRLB_MODE_MEMORY_Val      _U_(0x1)   /**< \brief (QSPI_CTRLB) Serial Memory operating mode */
#define QSPI_CTRLB_MODE_SPI         (QSPI_CTRLB_MODE_SPI_Val       << QSPI_CTRLB_MODE_Pos)
#define QSPI_CTRLB_MODE_MEMORY      (QSPI_CTRLB_MODE_MEMORY_Val    << QSPI_CTRLB_MODE_Pos)
#define QSPI_CTRLB_LOOPEN_Pos       1            /**< \brief (QSPI_CTRLB) Local Loopback Enable */
#define QSPI_CTRLB_LOOPEN           (_U_(0x1) << QSPI_CTRLB_LOOPEN_Pos)
#define QSPI_CTRLB_WDRBT_Pos        2            /**< \brief (QSPI_CTRLB) Wait Data Read Before Transfer */
#define QSPI_CTRLB_WDRBT            (_U_(0x1) << QSPI_CTRLB_WDRBT_Pos)
#define QSPI_CTRLB_SMEMREG_Pos      3            /**< \brief (QSPI_CTRLB) Serial Memory reg */
#define QSPI_CTRLB_SMEMREG          (_U_(0x1) << QSPI_CTRLB_SMEMREG_Pos)
#define QSPI_CTRLB_CSMODE_Pos       4            /**< \brief (QSPI_CTRLB) Chip Select Mode */
#define QSPI_CTRLB_CSMODE_Msk       (_U_(0x3) << QSPI_CTRLB_CSMODE_Pos)
#define QSPI_CTRLB_CSMODE(value)    (QSPI_CTRLB_CSMODE_Msk & ((value) << QSPI_CTRLB_CSMODE_Pos))
#define   QSPI_CTRLB_CSMODE_NORELOAD_Val  _U_(0x0)   /**< \brief (QSPI_CTRLB) The chip select is deasserted if TD has not been reloaded before the end of the current transfer. */
#define   QSPI_CTRLB_CSMODE_LASTXFER_Val  _U_(0x1)   /**< \brief (QSPI_CTRLB) The chip select is deasserted when the bit LASTXFER is written at 1 and the character written in TD has been transferred. */
#define   QSPI_CTRLB_CSMODE_SYSTEMATICALLY_Val _U_(0x2)   /**< \brief (QSPI_CTRLB) The chip select is deasserted systematically after each transfer. */
#define QSPI_CTRLB_CSMODE_NORELOAD  (QSPI_CTRLB_CSMODE_NORELOAD_Val << QSPI_CTRLB_CSMODE_Pos)
#define QSPI_CTRLB_CSMODE_LASTXFER  (QSPI_CTRLB_CSMODE_LASTXFER_Val << QSPI_CTRLB_CSMODE_Pos)
#define QSPI_CTRLB_CSMODE_SYSTEMATICALLY (QSPI_CTRLB_CSMODE_SYSTEMATICALLY_Val << QSPI_CTRLB_CSMODE_Pos)
#define QSPI_CTRLB_DATALEN_Pos      8            /**< \brief (QSPI_CTRLB) Data Length */
#define QSPI_CTRLB_DATALEN_Msk      (_U_(0xF) << QSPI_CTRLB_DATALEN_Pos)
#define QSPI_CTRLB_DATALEN(value)   (QSPI_CTRLB_DATALEN_Msk & ((value) << QSPI_CTRLB_DATALEN_Pos))
#define   QSPI_CTRLB_DATALEN_8BITS_Val    _U_(0x0)   /**< \brief (QSPI_CTRLB) 8-bits transfer */
#define   QSPI_CTRLB_DATALEN_9BITS_Val    _U_(0x1)   /**< \brief (QSPI_CTRLB) 9 bits transfer */
#define   QSPI_CTRLB_DATALEN_10BITS_Val   _U_(0x2)   /**< \brief (QSPI_CTRLB) 10-bits transfer */
#define   QSPI_CTRLB_DATALEN_11BITS_Val   _U_(0x3)   /**< \brief (QSPI_CTRLB) 11-bits transfer */
#define   QSPI_CTRLB_DATALEN_12BITS_Val   _U_(0x4)   /**< \brief (QSPI_CTRLB) 12-bits transfer */
#define   QSPI_CTRLB_DATALEN_13BITS_Val   _U_(0x5)   /**< \brief (QSPI_CTRLB) 13-bits transfer */
#define   QSPI_CTRLB_DATALEN_14BITS_Val   _U_(0x6)   /**< \brief (QSPI_CTRLB) 14-bits transfer */
#define   QSPI_CTRLB_DATALEN_15BITS_Val   _U_(0x7)   /**< \brief (QSPI_CTRLB) 15-bits transfer */
#define   QSPI_CTRLB_DATALEN_16BITS_Val   _U_(0x8)   /**< \brief (QSPI_CTRLB) 16-bits transfer */
#define QSPI_CTRLB_DATALEN_8BITS    (QSPI_CTRLB_DATALEN_8BITS_Val  << QSPI_CTRLB_DATALEN_Pos)
#define QSPI_CTRLB_DATALEN_9BITS    (QSPI_CTRLB_DATALEN_9BITS_Val  << QSPI_CTRLB_DATALEN_Pos)
#define QSPI_CTRLB_DATALEN_10BITS   (QSPI_CTRLB_DATALEN_10BITS_Val << QSPI_CTRLB_DATALEN_Pos)
#define QSPI_CTRLB_DATALEN_11BITS   (QSPI_CTRLB_DATALEN_11BITS_Val << QSPI_CTRLB_DATALEN_Pos)
#define QSPI_CTRLB_DATALEN_12BITS   (QSPI_CTRLB_DATALEN_12BITS_Val << QSPI_CTRLB_DATALEN_Pos)
#define QSPI_CTRLB_DATALEN_13BITS   (QSPI_CTRLB_DATALEN_13BITS_Val << QSPI_CTRLB_DATALEN_Pos)
#define QSPI_CTRLB_DATALEN_14BITS   (QSPI_CTRLB_DATALEN_14BITS_Val << QSPI_CTRLB_DATALEN_Pos)
#define QSPI_CTRLB_DATALEN_15BITS   (QSPI_CTRLB_DATALEN_15BITS_Val << QSPI_CTRLB_DATALEN_Pos)
#define QSPI_CTRLB_DATALEN_16BITS   (QSPI_CTRLB_DATALEN_16BITS_Val << QSPI_CTRLB_DATALEN_Pos)
#define QSPI_CTRLB_DLYBCT_Pos       16           /**< \brief (QSPI_CTRLB) Delay Between Consecutive Transfers */
#define QSPI_CTRLB_DLYBCT_Msk       (_U_(0xFF) << QSPI_CTRLB_DLYBCT_Pos)
#define QSPI_CTRLB_DLYBCT(value)    (QSPI_CTRLB_DLYBCT_Msk & ((value) << QSPI_CTRLB_DLYBCT_Pos))
#define QSPI_CTRLB_DLYCS_Pos        24           /**< \brief (QSPI_CTRLB) Minimum Inactive CS Delay */
#define QSPI_CTRLB_DLYCS_Msk        (_U_(0xFF) << QSPI_CTRLB_DLYCS_Pos)
#define QSPI_CTRLB_DLYCS(value)     (QSPI_CTRLB_DLYCS_Msk & ((value) << QSPI_CTRLB_DLYCS_Pos))
#define QSPI_CTRLB_MASK             _U_(0xFFFF0F3F) /**< \brief (QSPI_CTRLB) MASK Register */

/* -------- QSPI_BAUD : (QSPI Offset: 0x08) (R/W 32) Baud Rate -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CPOL:1;           /*!< bit:      0  Clock Polarity                     */
    uint32_t CPHA:1;           /*!< bit:      1  Clock Phase                        */
    uint32_t :6;               /*!< bit:  2.. 7  Reserved                           */
    uint32_t BAUD:8;           /*!< bit:  8..15  Serial Clock Baud Rate             */
    uint32_t DLYBS:8;          /*!< bit: 16..23  Delay Before SCK                   */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} QSPI_BAUD_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define QSPI_BAUD_OFFSET            0x08         /**< \brief (QSPI_BAUD offset) Baud Rate */
#define QSPI_BAUD_RESETVALUE        _U_(0x00000000) /**< \brief (QSPI_BAUD reset_value) Baud Rate */

#define QSPI_BAUD_CPOL_Pos          0            /**< \brief (QSPI_BAUD) Clock Polarity */
#define QSPI_BAUD_CPOL              (_U_(0x1) << QSPI_BAUD_CPOL_Pos)
#define QSPI_BAUD_CPHA_Pos          1            /**< \brief (QSPI_BAUD) Clock Phase */
#define QSPI_BAUD_CPHA              (_U_(0x1) << QSPI_BAUD_CPHA_Pos)
#define QSPI_BAUD_BAUD_Pos          8            /**< \brief (QSPI_BAUD) Serial Clock Baud Rate */
#define QSPI_BAUD_BAUD_Msk          (_U_(0xFF) << QSPI_BAUD_BAUD_Pos)
#define QSPI_BAUD_BAUD(value)       (QSPI_BAUD_BAUD_Msk & ((value) << QSPI_BAUD_BAUD_Pos))
#define QSPI_BAUD_DLYBS_Pos         16           /**< \brief (QSPI_BAUD) Delay Before SCK */
#define QSPI_BAUD_DLYBS_Msk         (_U_(0xFF) << QSPI_BAUD_DLYBS_Pos)
#define QSPI_BAUD_DLYBS(value)      (QSPI_BAUD_DLYBS_Msk & ((value) << QSPI_BAUD_DLYBS_Pos))
#define QSPI_BAUD_MASK              _U_(0x00FFFF03) /**< \brief (QSPI_BAUD) MASK Register */

/* -------- QSPI_RXDATA : (QSPI Offset: 0x0C) (R/  32) Receive Data -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DATA:16;          /*!< bit:  0..15  Receive Data                       */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} QSPI_RXDATA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define QSPI_RXDATA_OFFSET          0x0C         /**< \brief (QSPI_RXDATA offset) Receive Data */
#define QSPI_RXDATA_RESETVALUE      _U_(0x00000000) /**< \brief (QSPI_RXDATA reset_value) Receive Data */

#define QSPI_RXDATA_DATA_Pos        0            /**< \brief (QSPI_RXDATA) Receive Data */
#define QSPI_RXDATA_DATA_Msk        (_U_(0xFFFF) << QSPI_RXDATA_DATA_Pos)
#define QSPI_RXDATA_DATA(value)     (QSPI_RXDATA_DATA_Msk & ((value) << QSPI_RXDATA_DATA_Pos))
#define QSPI_RXDATA_MASK            _U_(0x0000FFFF) /**< \brief (QSPI_RXDATA) MASK Register */

/* -------- QSPI_TXDATA : (QSPI Offset: 0x10) ( /W 32) Transmit Data -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DATA:16;          /*!< bit:  0..15  Transmit Data                      */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} QSPI_TXDATA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define QSPI_TXDATA_OFFSET          0x10         /**< \brief (QSPI_TXDATA offset) Transmit Data */
#define QSPI_TXDATA_RESETVALUE      _U_(0x00000000) /**< \brief (QSPI_TXDATA reset_value) Transmit Data */

#define QSPI_TXDATA_DATA_Pos        0            /**< \brief (QSPI_TXDATA) Transmit Data */
#define QSPI_TXDATA_DATA_Msk        (_U_(0xFFFF) << QSPI_TXDATA_DATA_Pos)
#define QSPI_TXDATA_DATA(value)     (QSPI_TXDATA_DATA_Msk & ((value) << QSPI_TXDATA_DATA_Pos))
#define QSPI_TXDATA_MASK            _U_(0x0000FFFF) /**< \brief (QSPI_TXDATA) MASK Register */

/* -------- QSPI_INTENCLR : (QSPI Offset: 0x14) (R/W 32) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RXC:1;            /*!< bit:      0  Receive Data Register Full Interrupt Disable */
    uint32_t DRE:1;            /*!< bit:      1  Transmit Data Register Empty Interrupt Disable */
    uint32_t TXC:1;            /*!< bit:      2  Transmission Complete Interrupt Disable */
    uint32_t ERROR:1;          /*!< bit:      3  Overrun Error Interrupt Disable    */
    uint32_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint32_t CSRISE:1;         /*!< bit:      8  Chip Select Rise Interrupt Disable */
    uint32_t :1;               /*!< bit:      9  Reserved                           */
    uint32_t INSTREND:1;       /*!< bit:     10  Instruction End Interrupt Disable  */
    uint32_t :21;              /*!< bit: 11..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} QSPI_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define QSPI_INTENCLR_OFFSET        0x14         /**< \brief (QSPI_INTENCLR offset) Interrupt Enable Clear */
#define QSPI_INTENCLR_RESETVALUE    _U_(0x00000000) /**< \brief (QSPI_INTENCLR reset_value) Interrupt Enable Clear */

#define QSPI_INTENCLR_RXC_Pos       0            /**< \brief (QSPI_INTENCLR) Receive Data Register Full Interrupt Disable */
#define QSPI_INTENCLR_RXC           (_U_(0x1) << QSPI_INTENCLR_RXC_Pos)
#define QSPI_INTENCLR_DRE_Pos       1            /**< \brief (QSPI_INTENCLR) Transmit Data Register Empty Interrupt Disable */
#define QSPI_INTENCLR_DRE           (_U_(0x1) << QSPI_INTENCLR_DRE_Pos)
#define QSPI_INTENCLR_TXC_Pos       2            /**< \brief (QSPI_INTENCLR) Transmission Complete Interrupt Disable */
#define QSPI_INTENCLR_TXC           (_U_(0x1) << QSPI_INTENCLR_TXC_Pos)
#define QSPI_INTENCLR_ERROR_Pos     3            /**< \brief (QSPI_INTENCLR) Overrun Error Interrupt Disable */
#define QSPI_INTENCLR_ERROR         (_U_(0x1) << QSPI_INTENCLR_ERROR_Pos)
#define QSPI_INTENCLR_CSRISE_Pos    8            /**< \brief (QSPI_INTENCLR) Chip Select Rise Interrupt Disable */
#define QSPI_INTENCLR_CSRISE        (_U_(0x1) << QSPI_INTENCLR_CSRISE_Pos)
#define QSPI_INTENCLR_INSTREND_Pos  10           /**< \brief (QSPI_INTENCLR) Instruction End Interrupt Disable */
#define QSPI_INTENCLR_INSTREND      (_U_(0x1) << QSPI_INTENCLR_INSTREND_Pos)
#define QSPI_INTENCLR_MASK          _U_(0x0000050F) /**< \brief (QSPI_INTENCLR) MASK Register */

/* -------- QSPI_INTENSET : (QSPI Offset: 0x18) (R/W 32) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RXC:1;            /*!< bit:      0  Receive Data Register Full Interrupt Enable */
    uint32_t DRE:1;            /*!< bit:      1  Transmit Data Register Empty Interrupt Enable */
    uint32_t TXC:1;            /*!< bit:      2  Transmission Complete Interrupt Enable */
    uint32_t ERROR:1;          /*!< bit:      3  Overrun Error Interrupt Enable     */
    uint32_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint32_t CSRISE:1;         /*!< bit:      8  Chip Select Rise Interrupt Enable  */
    uint32_t :1;               /*!< bit:      9  Reserved                           */
    uint32_t INSTREND:1;       /*!< bit:     10  Instruction End Interrupt Enable   */
    uint32_t :21;              /*!< bit: 11..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} QSPI_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define QSPI_INTENSET_OFFSET        0x18         /**< \brief (QSPI_INTENSET offset) Interrupt Enable Set */
#define QSPI_INTENSET_RESETVALUE    _U_(0x00000000) /**< \brief (QSPI_INTENSET reset_value) Interrupt Enable Set */

#define QSPI_INTENSET_RXC_Pos       0            /**< \brief (QSPI_INTENSET) Receive Data Register Full Interrupt Enable */
#define QSPI_INTENSET_RXC           (_U_(0x1) << QSPI_INTENSET_RXC_Pos)
#define QSPI_INTENSET_DRE_Pos       1            /**< \brief (QSPI_INTENSET) Transmit Data Register Empty Interrupt Enable */
#define QSPI_INTENSET_DRE           (_U_(0x1) << QSPI_INTENSET_DRE_Pos)
#define QSPI_INTENSET_TXC_Pos       2            /**< \brief (QSPI_INTENSET) Transmission Complete Interrupt Enable */
#define QSPI_INTENSET_TXC           (_U_(0x1) << QSPI_INTENSET_TXC_Pos)
#define QSPI_INTENSET_ERROR_Pos     3            /**< \brief (QSPI_INTENSET) Overrun Error Interrupt Enable */
#define QSPI_INTENSET_ERROR         (_U_(0x1) << QSPI_INTENSET_ERROR_Pos)
#define QSPI_INTENSET_CSRISE_Pos    8            /**< \brief (QSPI_INTENSET) Chip Select Rise Interrupt Enable */
#define QSPI_INTENSET_CSRISE        (_U_(0x1) << QSPI_INTENSET_CSRISE_Pos)
#define QSPI_INTENSET_INSTREND_Pos  10           /**< \brief (QSPI_INTENSET) Instruction End Interrupt Enable */
#define QSPI_INTENSET_INSTREND      (_U_(0x1) << QSPI_INTENSET_INSTREND_Pos)
#define QSPI_INTENSET_MASK          _U_(0x0000050F) /**< \brief (QSPI_INTENSET) MASK Register */

/* -------- QSPI_INTFLAG : (QSPI Offset: 0x1C) (R/W 32) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t RXC:1;            /*!< bit:      0  Receive Data Register Full         */
    __I uint32_t DRE:1;            /*!< bit:      1  Transmit Data Register Empty       */
    __I uint32_t TXC:1;            /*!< bit:      2  Transmission Complete              */
    __I uint32_t ERROR:1;          /*!< bit:      3  Overrun Error                      */
    __I uint32_t :4;               /*!< bit:  4.. 7  Reserved                           */
    __I uint32_t CSRISE:1;         /*!< bit:      8  Chip Select Rise                   */
    __I uint32_t :1;               /*!< bit:      9  Reserved                           */
    __I uint32_t INSTREND:1;       /*!< bit:     10  Instruction End                    */
    __I uint32_t :21;              /*!< bit: 11..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} QSPI_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define QSPI_INTFLAG_OFFSET         0x1C         /**< \brief (QSPI_INTFLAG offset) Interrupt Flag Status and Clear */
#define QSPI_INTFLAG_RESETVALUE     _U_(0x00000000) /**< \brief (QSPI_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define QSPI_INTFLAG_RXC_Pos        0            /**< \brief (QSPI_INTFLAG) Receive Data Register Full */
#define QSPI_INTFLAG_RXC            (_U_(0x1) << QSPI_INTFLAG_RXC_Pos)
#define QSPI_INTFLAG_DRE_Pos        1            /**< \brief (QSPI_INTFLAG) Transmit Data Register Empty */
#define QSPI_INTFLAG_DRE            (_U_(0x1) << QSPI_INTFLAG_DRE_Pos)
#define QSPI_INTFLAG_TXC_Pos        2            /**< \brief (QSPI_INTFLAG) Transmission Complete */
#define QSPI_INTFLAG_TXC            (_U_(0x1) << QSPI_INTFLAG_TXC_Pos)
#define QSPI_INTFLAG_ERROR_Pos      3            /**< \brief (QSPI_INTFLAG) Overrun Error */
#define QSPI_INTFLAG_ERROR          (_U_(0x1) << QSPI_INTFLAG_ERROR_Pos)
#define QSPI_INTFLAG_CSRISE_Pos     8            /**< \brief (QSPI_INTFLAG) Chip Select Rise */
#define QSPI_INTFLAG_CSRISE         (_U_(0x1) << QSPI_INTFLAG_CSRISE_Pos)
#define QSPI_INTFLAG_INSTREND_Pos   10           /**< \brief (QSPI_INTFLAG) Instruction End */
#define QSPI_INTFLAG_INSTREND       (_U_(0x1) << QSPI_INTFLAG_INSTREND_Pos)
#define QSPI_INTFLAG_MASK           _U_(0x0000050F) /**< \brief (QSPI_INTFLAG) MASK Register */

/* -------- QSPI_STATUS : (QSPI Offset: 0x20) (R/  32) Status Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint32_t :7;               /*!< bit:  2.. 8  Reserved                           */
    uint32_t CSSTATUS:1;       /*!< bit:      9  Chip Select                        */
    uint32_t :22;              /*!< bit: 10..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} QSPI_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define QSPI_STATUS_OFFSET          0x20         /**< \brief (QSPI_STATUS offset) Status Register */
#define QSPI_STATUS_RESETVALUE      _U_(0x00000200) /**< \brief (QSPI_STATUS reset_value) Status Register */

#define QSPI_STATUS_ENABLE_Pos      1            /**< \brief (QSPI_STATUS) Enable */
#define QSPI_STATUS_ENABLE          (_U_(0x1) << QSPI_STATUS_ENABLE_Pos)
#define QSPI_STATUS_CSSTATUS_Pos    9            /**< \brief (QSPI_STATUS) Chip Select */
#define QSPI_STATUS_CSSTATUS        (_U_(0x1) << QSPI_STATUS_CSSTATUS_Pos)
#define QSPI_STATUS_MASK            _U_(0x00000202) /**< \brief (QSPI_STATUS) MASK Register */

/* -------- QSPI_INSTRADDR : (QSPI Offset: 0x30) (R/W 32) Instruction Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ADDR:32;          /*!< bit:  0..31  Instruction Address                */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} QSPI_INSTRADDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define QSPI_INSTRADDR_OFFSET       0x30         /**< \brief (QSPI_INSTRADDR offset) Instruction Address */
#define QSPI_INSTRADDR_RESETVALUE   _U_(0x00000000) /**< \brief (QSPI_INSTRADDR reset_value) Instruction Address */

#define QSPI_INSTRADDR_ADDR_Pos     0            /**< \brief (QSPI_INSTRADDR) Instruction Address */
#define QSPI_INSTRADDR_ADDR_Msk     (_U_(0xFFFFFFFF) << QSPI_INSTRADDR_ADDR_Pos)
#define QSPI_INSTRADDR_ADDR(value)  (QSPI_INSTRADDR_ADDR_Msk & ((value) << QSPI_INSTRADDR_ADDR_Pos))
#define QSPI_INSTRADDR_MASK         _U_(0xFFFFFFFF) /**< \brief (QSPI_INSTRADDR) MASK Register */

/* -------- QSPI_INSTRCTRL : (QSPI Offset: 0x34) (R/W 32) Instruction Code -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t INSTR:8;          /*!< bit:  0.. 7  Instruction Code                   */
    uint32_t :8;               /*!< bit:  8..15  Reserved                           */
    uint32_t OPTCODE:8;        /*!< bit: 16..23  Option Code                        */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} QSPI_INSTRCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define QSPI_INSTRCTRL_OFFSET       0x34         /**< \brief (QSPI_INSTRCTRL offset) Instruction Code */
#define QSPI_INSTRCTRL_RESETVALUE   _U_(0x00000000) /**< \brief (QSPI_INSTRCTRL reset_value) Instruction Code */

#define QSPI_INSTRCTRL_INSTR_Pos    0            /**< \brief (QSPI_INSTRCTRL) Instruction Code */
#define QSPI_INSTRCTRL_INSTR_Msk    (_U_(0xFF) << QSPI_INSTRCTRL_INSTR_Pos)
#define QSPI_INSTRCTRL_INSTR(value) (QSPI_INSTRCTRL_INSTR_Msk & ((value) << QSPI_INSTRCTRL_INSTR_Pos))
#define QSPI_INSTRCTRL_OPTCODE_Pos  16           /**< \brief (QSPI_INSTRCTRL) Option Code */
#define QSPI_INSTRCTRL_OPTCODE_Msk  (_U_(0xFF) << QSPI_INSTRCTRL_OPTCODE_Pos)
#define QSPI_INSTRCTRL_OPTCODE(value) (QSPI_INSTRCTRL_OPTCODE_Msk & ((value) << QSPI_INSTRCTRL_OPTCODE_Pos))
#define QSPI_INSTRCTRL_MASK         _U_(0x00FF00FF) /**< \brief (QSPI_INSTRCTRL) MASK Register */

/* -------- QSPI_INSTRFRAME : (QSPI Offset: 0x38) (R/W 32) Instruction Frame -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t WIDTH:3;          /*!< bit:  0.. 2  Instruction Code, Address, Option Code and Data Width */
    uint32_t :1;               /*!< bit:      3  Reserved                           */
    uint32_t INSTREN:1;        /*!< bit:      4  Instruction Enable                 */
    uint32_t ADDREN:1;         /*!< bit:      5  Address Enable                     */
    uint32_t OPTCODEEN:1;      /*!< bit:      6  Option Enable                      */
    uint32_t DATAEN:1;         /*!< bit:      7  Data Enable                        */
    uint32_t OPTCODELEN:2;     /*!< bit:  8.. 9  Option Code Length                 */
    uint32_t ADDRLEN:1;        /*!< bit:     10  Address Length                     */
    uint32_t :1;               /*!< bit:     11  Reserved                           */
    uint32_t TFRTYPE:2;        /*!< bit: 12..13  Data Transfer Type                 */
    uint32_t CRMODE:1;         /*!< bit:     14  Continuous Read Mode               */
    uint32_t DDREN:1;          /*!< bit:     15  Double Data Rate Enable            */
    uint32_t DUMMYLEN:5;       /*!< bit: 16..20  Dummy Cycles Length                */
    uint32_t :11;              /*!< bit: 21..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} QSPI_INSTRFRAME_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define QSPI_INSTRFRAME_OFFSET      0x38         /**< \brief (QSPI_INSTRFRAME offset) Instruction Frame */
#define QSPI_INSTRFRAME_RESETVALUE  _U_(0x00000000) /**< \brief (QSPI_INSTRFRAME reset_value) Instruction Frame */

#define QSPI_INSTRFRAME_WIDTH_Pos   0            /**< \brief (QSPI_INSTRFRAME) Instruction Code, Address, Option Code and Data Width */
#define QSPI_INSTRFRAME_WIDTH_Msk   (_U_(0x7) << QSPI_INSTRFRAME_WIDTH_Pos)
#define QSPI_INSTRFRAME_WIDTH(value) (QSPI_INSTRFRAME_WIDTH_Msk & ((value) << QSPI_INSTRFRAME_WIDTH_Pos))
#define   QSPI_INSTRFRAME_WIDTH_SINGLE_BIT_SPI_Val _U_(0x0)   /**< \brief (QSPI_INSTRFRAME) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Single-bit SPI */
#define   QSPI_INSTRFRAME_WIDTH_DUAL_OUTPUT_Val _U_(0x1)   /**< \brief (QSPI_INSTRFRAME) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Dual SPI */
#define   QSPI_INSTRFRAME_WIDTH_QUAD_OUTPUT_Val _U_(0x2)   /**< \brief (QSPI_INSTRFRAME) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Quad SPI */
#define   QSPI_INSTRFRAME_WIDTH_DUAL_IO_Val _U_(0x3)   /**< \brief (QSPI_INSTRFRAME) Instruction: Single-bit SPI / Address-Option: Dual SPI / Data: Dual SPI */
#define   QSPI_INSTRFRAME_WIDTH_QUAD_IO_Val _U_(0x4)   /**< \brief (QSPI_INSTRFRAME) Instruction: Single-bit SPI / Address-Option: Quad SPI / Data: Quad SPI */
#define   QSPI_INSTRFRAME_WIDTH_DUAL_CMD_Val _U_(0x5)   /**< \brief (QSPI_INSTRFRAME) Instruction: Dual SPI / Address-Option: Dual SPI / Data: Dual SPI */
#define   QSPI_INSTRFRAME_WIDTH_QUAD_CMD_Val _U_(0x6)   /**< \brief (QSPI_INSTRFRAME) Instruction: Quad SPI / Address-Option: Quad SPI / Data: Quad SPI */
#define QSPI_INSTRFRAME_WIDTH_SINGLE_BIT_SPI (QSPI_INSTRFRAME_WIDTH_SINGLE_BIT_SPI_Val << QSPI_INSTRFRAME_WIDTH_Pos)
#define QSPI_INSTRFRAME_WIDTH_DUAL_OUTPUT (QSPI_INSTRFRAME_WIDTH_DUAL_OUTPUT_Val << QSPI_INSTRFRAME_WIDTH_Pos)
#define QSPI_INSTRFRAME_WIDTH_QUAD_OUTPUT (QSPI_INSTRFRAME_WIDTH_QUAD_OUTPUT_Val << QSPI_INSTRFRAME_WIDTH_Pos)
#define QSPI_INSTRFRAME_WIDTH_DUAL_IO (QSPI_INSTRFRAME_WIDTH_DUAL_IO_Val << QSPI_INSTRFRAME_WIDTH_Pos)
#define QSPI_INSTRFRAME_WIDTH_QUAD_IO (QSPI_INSTRFRAME_WIDTH_QUAD_IO_Val << QSPI_INSTRFRAME_WIDTH_Pos)
#define QSPI_INSTRFRAME_WIDTH_DUAL_CMD (QSPI_INSTRFRAME_WIDTH_DUAL_CMD_Val << QSPI_INSTRFRAME_WIDTH_Pos)
#define QSPI_INSTRFRAME_WIDTH_QUAD_CMD (QSPI_INSTRFRAME_WIDTH_QUAD_CMD_Val << QSPI_INSTRFRAME_WIDTH_Pos)
#define QSPI_INSTRFRAME_INSTREN_Pos 4            /**< \brief (QSPI_INSTRFRAME) Instruction Enable */
#define QSPI_INSTRFRAME_INSTREN     (_U_(0x1) << QSPI_INSTRFRAME_INSTREN_Pos)
#define QSPI_INSTRFRAME_ADDREN_Pos  5            /**< \brief (QSPI_INSTRFRAME) Address Enable */
#define QSPI_INSTRFRAME_ADDREN      (_U_(0x1) << QSPI_INSTRFRAME_ADDREN_Pos)
#define QSPI_INSTRFRAME_OPTCODEEN_Pos 6            /**< \brief (QSPI_INSTRFRAME) Option Enable */
#define QSPI_INSTRFRAME_OPTCODEEN   (_U_(0x1) << QSPI_INSTRFRAME_OPTCODEEN_Pos)
#define QSPI_INSTRFRAME_DATAEN_Pos  7            /**< \brief (QSPI_INSTRFRAME) Data Enable */
#define QSPI_INSTRFRAME_DATAEN      (_U_(0x1) << QSPI_INSTRFRAME_DATAEN_Pos)
#define QSPI_INSTRFRAME_OPTCODELEN_Pos 8            /**< \brief (QSPI_INSTRFRAME) Option Code Length */
#define QSPI_INSTRFRAME_OPTCODELEN_Msk (_U_(0x3) << QSPI_INSTRFRAME_OPTCODELEN_Pos)
#define QSPI_INSTRFRAME_OPTCODELEN(value) (QSPI_INSTRFRAME_OPTCODELEN_Msk & ((value) << QSPI_INSTRFRAME_OPTCODELEN_Pos))
#define   QSPI_INSTRFRAME_OPTCODELEN_1BIT_Val _U_(0x0)   /**< \brief (QSPI_INSTRFRAME) 1-bit length option code */
#define   QSPI_INSTRFRAME_OPTCODELEN_2BITS_Val _U_(0x1)   /**< \brief (QSPI_INSTRFRAME) 2-bits length option code */
#define   QSPI_INSTRFRAME_OPTCODELEN_4BITS_Val _U_(0x2)   /**< \brief (QSPI_INSTRFRAME) 4-bits length option code */
#define   QSPI_INSTRFRAME_OPTCODELEN_8BITS_Val _U_(0x3)   /**< \brief (QSPI_INSTRFRAME) 8-bits length option code */
#define QSPI_INSTRFRAME_OPTCODELEN_1BIT (QSPI_INSTRFRAME_OPTCODELEN_1BIT_Val << QSPI_INSTRFRAME_OPTCODELEN_Pos)
#define QSPI_INSTRFRAME_OPTCODELEN_2BITS (QSPI_INSTRFRAME_OPTCODELEN_2BITS_Val << QSPI_INSTRFRAME_OPTCODELEN_Pos)
#define QSPI_INSTRFRAME_OPTCODELEN_4BITS (QSPI_INSTRFRAME_OPTCODELEN_4BITS_Val << QSPI_INSTRFRAME_OPTCODELEN_Pos)
#define QSPI_INSTRFRAME_OPTCODELEN_8BITS (QSPI_INSTRFRAME_OPTCODELEN_8BITS_Val << QSPI_INSTRFRAME_OPTCODELEN_Pos)
#define QSPI_INSTRFRAME_ADDRLEN_Pos 10           /**< \brief (QSPI_INSTRFRAME) Address Length */
#define QSPI_INSTRFRAME_ADDRLEN     (_U_(0x1) << QSPI_INSTRFRAME_ADDRLEN_Pos)
#define   QSPI_INSTRFRAME_ADDRLEN_24BITS_Val _U_(0x0)   /**< \brief (QSPI_INSTRFRAME) 24-bits address length */
#define   QSPI_INSTRFRAME_ADDRLEN_32BITS_Val _U_(0x1)   /**< \brief (QSPI_INSTRFRAME) 32-bits address length */
#define QSPI_INSTRFRAME_ADDRLEN_24BITS (QSPI_INSTRFRAME_ADDRLEN_24BITS_Val << QSPI_INSTRFRAME_ADDRLEN_Pos)
#define QSPI_INSTRFRAME_ADDRLEN_32BITS (QSPI_INSTRFRAME_ADDRLEN_32BITS_Val << QSPI_INSTRFRAME_ADDRLEN_Pos)
#define QSPI_INSTRFRAME_TFRTYPE_Pos 12           /**< \brief (QSPI_INSTRFRAME) Data Transfer Type */
#define QSPI_INSTRFRAME_TFRTYPE_Msk (_U_(0x3) << QSPI_INSTRFRAME_TFRTYPE_Pos)
#define QSPI_INSTRFRAME_TFRTYPE(value) (QSPI_INSTRFRAME_TFRTYPE_Msk & ((value) << QSPI_INSTRFRAME_TFRTYPE_Pos))
#define   QSPI_INSTRFRAME_TFRTYPE_READ_Val _U_(0x0)   /**< \brief (QSPI_INSTRFRAME) Read transfer from the serial memory.Scrambling is not performed.Read at random location (fetch) in the serial flash memory is not possible. */
#define   QSPI_INSTRFRAME_TFRTYPE_READMEMORY_Val _U_(0x1)   /**< \brief (QSPI_INSTRFRAME) Read data transfer from the serial memory.If enabled, scrambling is performed.Read at random location (fetch) in the serial flash memory is possible. */
#define   QSPI_INSTRFRAME_TFRTYPE_WRITE_Val _U_(0x2)   /**< \brief (QSPI_INSTRFRAME) Write transfer into the serial memory.Scrambling is not performed. */
#define   QSPI_INSTRFRAME_TFRTYPE_WRITEMEMORY_Val _U_(0x3)   /**< \brief (QSPI_INSTRFRAME) Write data transfer into the serial memory.If enabled, scrambling is performed. */
#define QSPI_INSTRFRAME_TFRTYPE_READ (QSPI_INSTRFRAME_TFRTYPE_READ_Val << QSPI_INSTRFRAME_TFRTYPE_Pos)
#define QSPI_INSTRFRAME_TFRTYPE_READMEMORY (QSPI_INSTRFRAME_TFRTYPE_READMEMORY_Val << QSPI_INSTRFRAME_TFRTYPE_Pos)
#define QSPI_INSTRFRAME_TFRTYPE_WRITE (QSPI_INSTRFRAME_TFRTYPE_WRITE_Val << QSPI_INSTRFRAME_TFRTYPE_Pos)
#define QSPI_INSTRFRAME_TFRTYPE_WRITEMEMORY (QSPI_INSTRFRAME_TFRTYPE_WRITEMEMORY_Val << QSPI_INSTRFRAME_TFRTYPE_Pos)
#define QSPI_INSTRFRAME_CRMODE_Pos  14           /**< \brief (QSPI_INSTRFRAME) Continuous Read Mode */
#define QSPI_INSTRFRAME_CRMODE      (_U_(0x1) << QSPI_INSTRFRAME_CRMODE_Pos)
#define QSPI_INSTRFRAME_DDREN_Pos   15           /**< \brief (QSPI_INSTRFRAME) Double Data Rate Enable */
#define QSPI_INSTRFRAME_DDREN       (_U_(0x1) << QSPI_INSTRFRAME_DDREN_Pos)
#define QSPI_INSTRFRAME_DUMMYLEN_Pos 16           /**< \brief (QSPI_INSTRFRAME) Dummy Cycles Length */
#define QSPI_INSTRFRAME_DUMMYLEN_Msk (_U_(0x1F) << QSPI_INSTRFRAME_DUMMYLEN_Pos)
#define QSPI_INSTRFRAME_DUMMYLEN(value) (QSPI_INSTRFRAME_DUMMYLEN_Msk & ((value) << QSPI_INSTRFRAME_DUMMYLEN_Pos))
#define QSPI_INSTRFRAME_MASK        _U_(0x001FF7F7) /**< \brief (QSPI_INSTRFRAME) MASK Register */

/* -------- QSPI_SCRAMBCTRL : (QSPI Offset: 0x40) (R/W 32) Scrambling Mode -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ENABLE:1;         /*!< bit:      0  Scrambling/Unscrambling Enable     */
    uint32_t RANDOMDIS:1;      /*!< bit:      1  Scrambling/Unscrambling Random Value Disable */
    uint32_t :30;              /*!< bit:  2..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} QSPI_SCRAMBCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define QSPI_SCRAMBCTRL_OFFSET      0x40         /**< \brief (QSPI_SCRAMBCTRL offset) Scrambling Mode */
#define QSPI_SCRAMBCTRL_RESETVALUE  _U_(0x00000000) /**< \brief (QSPI_SCRAMBCTRL reset_value) Scrambling Mode */

#define QSPI_SCRAMBCTRL_ENABLE_Pos  0            /**< \brief (QSPI_SCRAMBCTRL) Scrambling/Unscrambling Enable */
#define QSPI_SCRAMBCTRL_ENABLE      (_U_(0x1) << QSPI_SCRAMBCTRL_ENABLE_Pos)
#define QSPI_SCRAMBCTRL_RANDOMDIS_Pos 1            /**< \brief (QSPI_SCRAMBCTRL) Scrambling/Unscrambling Random Value Disable */
#define QSPI_SCRAMBCTRL_RANDOMDIS   (_U_(0x1) << QSPI_SCRAMBCTRL_RANDOMDIS_Pos)
#define QSPI_SCRAMBCTRL_MASK        _U_(0x00000003) /**< \brief (QSPI_SCRAMBCTRL) MASK Register */

/* -------- QSPI_SCRAMBKEY : (QSPI Offset: 0x44) ( /W 32) Scrambling Key -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t KEY:32;           /*!< bit:  0..31  Scrambling User Key                */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} QSPI_SCRAMBKEY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define QSPI_SCRAMBKEY_OFFSET       0x44         /**< \brief (QSPI_SCRAMBKEY offset) Scrambling Key */
#define QSPI_SCRAMBKEY_RESETVALUE   _U_(0x00000000) /**< \brief (QSPI_SCRAMBKEY reset_value) Scrambling Key */

#define QSPI_SCRAMBKEY_KEY_Pos      0            /**< \brief (QSPI_SCRAMBKEY) Scrambling User Key */
#define QSPI_SCRAMBKEY_KEY_Msk      (_U_(0xFFFFFFFF) << QSPI_SCRAMBKEY_KEY_Pos)
#define QSPI_SCRAMBKEY_KEY(value)   (QSPI_SCRAMBKEY_KEY_Msk & ((value) << QSPI_SCRAMBKEY_KEY_Pos))
#define QSPI_SCRAMBKEY_MASK         _U_(0xFFFFFFFF) /**< \brief (QSPI_SCRAMBKEY) MASK Register */

/** \brief QSPI APB hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO QSPI_CTRLA_Type           CTRLA;       /**< \brief Offset: 0x00 (R/W 32) Control A */
  __IO QSPI_CTRLB_Type           CTRLB;       /**< \brief Offset: 0x04 (R/W 32) Control B */
  __IO QSPI_BAUD_Type            BAUD;        /**< \brief Offset: 0x08 (R/W 32) Baud Rate */
  __I  QSPI_RXDATA_Type          RXDATA;      /**< \brief Offset: 0x0C (R/  32) Receive Data */
  __O  QSPI_TXDATA_Type          TXDATA;      /**< \brief Offset: 0x10 ( /W 32) Transmit Data */
  __IO QSPI_INTENCLR_Type        INTENCLR;    /**< \brief Offset: 0x14 (R/W 32) Interrupt Enable Clear */
  __IO QSPI_INTENSET_Type        INTENSET;    /**< \brief Offset: 0x18 (R/W 32) Interrupt Enable Set */
  __IO QSPI_INTFLAG_Type         INTFLAG;     /**< \brief Offset: 0x1C (R/W 32) Interrupt Flag Status and Clear */
  __I  QSPI_STATUS_Type          STATUS;      /**< \brief Offset: 0x20 (R/  32) Status Register */
       RoReg8                    Reserved1[0xC];
  __IO QSPI_INSTRADDR_Type       INSTRADDR;   /**< \brief Offset: 0x30 (R/W 32) Instruction Address */
  __IO QSPI_INSTRCTRL_Type       INSTRCTRL;   /**< \brief Offset: 0x34 (R/W 32) Instruction Code */
  __IO QSPI_INSTRFRAME_Type      INSTRFRAME;  /**< \brief Offset: 0x38 (R/W 32) Instruction Frame */
       RoReg8                    Reserved2[0x4];
  __IO QSPI_SCRAMBCTRL_Type      SCRAMBCTRL;  /**< \brief Offset: 0x40 (R/W 32) Scrambling Mode */
  __O  QSPI_SCRAMBKEY_Type       SCRAMBKEY;   /**< \brief Offset: 0x44 ( /W 32) Scrambling Key */
} Qspi;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_QSPI_COMPONENT_ */
