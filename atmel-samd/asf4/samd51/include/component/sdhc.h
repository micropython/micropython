/**
 * \file
 *
 * \brief Component description for SDHC
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

#ifndef _SAMD51_SDHC_COMPONENT_
#define _SAMD51_SDHC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SDHC */
/* ========================================================================== */
/** \addtogroup SAMD51_SDHC SD/MMC Host Controller */
/*@{*/

#define SDHC_U2011
#define REV_SDHC                    0x183

/* -------- SDHC_SSAR : (SDHC Offset: 0x000) (R/W 32) SDMA System Address / Argument 2 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct { // CMD23 mode
    uint32_t ARG2:32;          /*!< bit:  0..31  Argument 2                         */
  } CMD23;                     /*!< Structure used for CMD23                        */
  struct {
    uint32_t ADDR:32;          /*!< bit:  0..31  SDMA System Address                */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SDHC_SSAR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_SSAR_OFFSET            0x000        /**< \brief (SDHC_SSAR offset) SDMA System Address / Argument 2 */
#define SDHC_SSAR_RESETVALUE        _U_(0x00000000) /**< \brief (SDHC_SSAR reset_value) SDMA System Address / Argument 2 */

// CMD23 mode
#define SDHC_SSAR_CMD23_ARG2_Pos    0            /**< \brief (SDHC_SSAR_CMD23) Argument 2 */
#define SDHC_SSAR_CMD23_ARG2_Msk    (_U_(0xFFFFFFFF) << SDHC_SSAR_CMD23_ARG2_Pos)
#define SDHC_SSAR_CMD23_ARG2(value) (SDHC_SSAR_CMD23_ARG2_Msk & ((value) << SDHC_SSAR_CMD23_ARG2_Pos))
#define SDHC_SSAR_CMD23_MASK        _U_(0xFFFFFFFF) /**< \brief (SDHC_SSAR_CMD23) MASK Register */

#define SDHC_SSAR_ADDR_Pos          0            /**< \brief (SDHC_SSAR) SDMA System Address */
#define SDHC_SSAR_ADDR_Msk          (_U_(0xFFFFFFFF) << SDHC_SSAR_ADDR_Pos)
#define SDHC_SSAR_ADDR(value)       (SDHC_SSAR_ADDR_Msk & ((value) << SDHC_SSAR_ADDR_Pos))
#define SDHC_SSAR_MASK              _U_(0xFFFFFFFF) /**< \brief (SDHC_SSAR) MASK Register */

/* -------- SDHC_BSR : (SDHC Offset: 0x004) (R/W 16) Block Size -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t BLOCKSIZE:10;     /*!< bit:  0.. 9  Transfer Block Size                */
    uint16_t :2;               /*!< bit: 10..11  Reserved                           */
    uint16_t BOUNDARY:3;       /*!< bit: 12..14  SDMA Buffer Boundary               */
    uint16_t :1;               /*!< bit:     15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_BSR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_BSR_OFFSET             0x004        /**< \brief (SDHC_BSR offset) Block Size */
#define SDHC_BSR_RESETVALUE         _U_(0x0000)  /**< \brief (SDHC_BSR reset_value) Block Size */

#define SDHC_BSR_BLOCKSIZE_Pos      0            /**< \brief (SDHC_BSR) Transfer Block Size */
#define SDHC_BSR_BLOCKSIZE_Msk      (_U_(0x3FF) << SDHC_BSR_BLOCKSIZE_Pos)
#define SDHC_BSR_BLOCKSIZE(value)   (SDHC_BSR_BLOCKSIZE_Msk & ((value) << SDHC_BSR_BLOCKSIZE_Pos))
#define SDHC_BSR_BOUNDARY_Pos       12           /**< \brief (SDHC_BSR) SDMA Buffer Boundary */
#define SDHC_BSR_BOUNDARY_Msk       (_U_(0x7) << SDHC_BSR_BOUNDARY_Pos)
#define SDHC_BSR_BOUNDARY(value)    (SDHC_BSR_BOUNDARY_Msk & ((value) << SDHC_BSR_BOUNDARY_Pos))
#define   SDHC_BSR_BOUNDARY_4K_Val        _U_(0x0)   /**< \brief (SDHC_BSR) 4k bytes */
#define   SDHC_BSR_BOUNDARY_8K_Val        _U_(0x1)   /**< \brief (SDHC_BSR) 8k bytes */
#define   SDHC_BSR_BOUNDARY_16K_Val       _U_(0x2)   /**< \brief (SDHC_BSR) 16k bytes */
#define   SDHC_BSR_BOUNDARY_32K_Val       _U_(0x3)   /**< \brief (SDHC_BSR) 32k bytes */
#define   SDHC_BSR_BOUNDARY_64K_Val       _U_(0x4)   /**< \brief (SDHC_BSR) 64k bytes */
#define   SDHC_BSR_BOUNDARY_128K_Val      _U_(0x5)   /**< \brief (SDHC_BSR) 128k bytes */
#define   SDHC_BSR_BOUNDARY_256K_Val      _U_(0x6)   /**< \brief (SDHC_BSR) 256k bytes */
#define   SDHC_BSR_BOUNDARY_512K_Val      _U_(0x7)   /**< \brief (SDHC_BSR) 512k bytes */
#define SDHC_BSR_BOUNDARY_4K        (SDHC_BSR_BOUNDARY_4K_Val      << SDHC_BSR_BOUNDARY_Pos)
#define SDHC_BSR_BOUNDARY_8K        (SDHC_BSR_BOUNDARY_8K_Val      << SDHC_BSR_BOUNDARY_Pos)
#define SDHC_BSR_BOUNDARY_16K       (SDHC_BSR_BOUNDARY_16K_Val     << SDHC_BSR_BOUNDARY_Pos)
#define SDHC_BSR_BOUNDARY_32K       (SDHC_BSR_BOUNDARY_32K_Val     << SDHC_BSR_BOUNDARY_Pos)
#define SDHC_BSR_BOUNDARY_64K       (SDHC_BSR_BOUNDARY_64K_Val     << SDHC_BSR_BOUNDARY_Pos)
#define SDHC_BSR_BOUNDARY_128K      (SDHC_BSR_BOUNDARY_128K_Val    << SDHC_BSR_BOUNDARY_Pos)
#define SDHC_BSR_BOUNDARY_256K      (SDHC_BSR_BOUNDARY_256K_Val    << SDHC_BSR_BOUNDARY_Pos)
#define SDHC_BSR_BOUNDARY_512K      (SDHC_BSR_BOUNDARY_512K_Val    << SDHC_BSR_BOUNDARY_Pos)
#define SDHC_BSR_MASK               _U_(0x73FF)  /**< \brief (SDHC_BSR) MASK Register */

/* -------- SDHC_BCR : (SDHC Offset: 0x006) (R/W 16) Block Count -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t BCNT:16;          /*!< bit:  0..15  Blocks Count for Current Transfer  */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_BCR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_BCR_OFFSET             0x006        /**< \brief (SDHC_BCR offset) Block Count */
#define SDHC_BCR_RESETVALUE         _U_(0x0000)  /**< \brief (SDHC_BCR reset_value) Block Count */

#define SDHC_BCR_BCNT_Pos           0            /**< \brief (SDHC_BCR) Blocks Count for Current Transfer */
#define SDHC_BCR_BCNT_Msk           (_U_(0xFFFF) << SDHC_BCR_BCNT_Pos)
#define SDHC_BCR_BCNT(value)        (SDHC_BCR_BCNT_Msk & ((value) << SDHC_BCR_BCNT_Pos))
#define SDHC_BCR_MASK               _U_(0xFFFF)  /**< \brief (SDHC_BCR) MASK Register */

/* -------- SDHC_ARG1R : (SDHC Offset: 0x008) (R/W 32) Argument 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ARG:32;           /*!< bit:  0..31  Argument 1                         */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SDHC_ARG1R_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_ARG1R_OFFSET           0x008        /**< \brief (SDHC_ARG1R offset) Argument 1 */
#define SDHC_ARG1R_RESETVALUE       _U_(0x00000000) /**< \brief (SDHC_ARG1R reset_value) Argument 1 */

#define SDHC_ARG1R_ARG_Pos          0            /**< \brief (SDHC_ARG1R) Argument 1 */
#define SDHC_ARG1R_ARG_Msk          (_U_(0xFFFFFFFF) << SDHC_ARG1R_ARG_Pos)
#define SDHC_ARG1R_ARG(value)       (SDHC_ARG1R_ARG_Msk & ((value) << SDHC_ARG1R_ARG_Pos))
#define SDHC_ARG1R_MASK             _U_(0xFFFFFFFF) /**< \brief (SDHC_ARG1R) MASK Register */

/* -------- SDHC_TMR : (SDHC Offset: 0x00C) (R/W 16) Transfer Mode -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t DMAEN:1;          /*!< bit:      0  DMA Enable                         */
    uint16_t BCEN:1;           /*!< bit:      1  Block Count Enable                 */
    uint16_t ACMDEN:2;         /*!< bit:  2.. 3  Auto Command Enable                */
    uint16_t DTDSEL:1;         /*!< bit:      4  Data Transfer Direction Selection  */
    uint16_t MSBSEL:1;         /*!< bit:      5  Multi/Single Block Selection       */
    uint16_t :10;              /*!< bit:  6..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_TMR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_TMR_OFFSET             0x00C        /**< \brief (SDHC_TMR offset) Transfer Mode */
#define SDHC_TMR_RESETVALUE         _U_(0x0000)  /**< \brief (SDHC_TMR reset_value) Transfer Mode */

#define SDHC_TMR_DMAEN_Pos          0            /**< \brief (SDHC_TMR) DMA Enable */
#define SDHC_TMR_DMAEN              (_U_(0x1) << SDHC_TMR_DMAEN_Pos)
#define   SDHC_TMR_DMAEN_DISABLE_Val      _U_(0x0)   /**< \brief (SDHC_TMR) No data transfer or Non DMA data transfer */
#define   SDHC_TMR_DMAEN_ENABLE_Val       _U_(0x1)   /**< \brief (SDHC_TMR) DMA data transfer */
#define SDHC_TMR_DMAEN_DISABLE      (SDHC_TMR_DMAEN_DISABLE_Val    << SDHC_TMR_DMAEN_Pos)
#define SDHC_TMR_DMAEN_ENABLE       (SDHC_TMR_DMAEN_ENABLE_Val     << SDHC_TMR_DMAEN_Pos)
#define SDHC_TMR_BCEN_Pos           1            /**< \brief (SDHC_TMR) Block Count Enable */
#define SDHC_TMR_BCEN               (_U_(0x1) << SDHC_TMR_BCEN_Pos)
#define   SDHC_TMR_BCEN_DISABLE_Val       _U_(0x0)   /**< \brief (SDHC_TMR) Disable */
#define   SDHC_TMR_BCEN_ENABLE_Val        _U_(0x1)   /**< \brief (SDHC_TMR) Enable */
#define SDHC_TMR_BCEN_DISABLE       (SDHC_TMR_BCEN_DISABLE_Val     << SDHC_TMR_BCEN_Pos)
#define SDHC_TMR_BCEN_ENABLE        (SDHC_TMR_BCEN_ENABLE_Val      << SDHC_TMR_BCEN_Pos)
#define SDHC_TMR_ACMDEN_Pos         2            /**< \brief (SDHC_TMR) Auto Command Enable */
#define SDHC_TMR_ACMDEN_Msk         (_U_(0x3) << SDHC_TMR_ACMDEN_Pos)
#define SDHC_TMR_ACMDEN(value)      (SDHC_TMR_ACMDEN_Msk & ((value) << SDHC_TMR_ACMDEN_Pos))
#define   SDHC_TMR_ACMDEN_DISABLED_Val    _U_(0x0)   /**< \brief (SDHC_TMR) Auto Command Disabled */
#define   SDHC_TMR_ACMDEN_CMD12_Val       _U_(0x1)   /**< \brief (SDHC_TMR) Auto CMD12 Enable */
#define   SDHC_TMR_ACMDEN_CMD23_Val       _U_(0x2)   /**< \brief (SDHC_TMR) Auto CMD23 Enable */
#define   SDHC_TMR_ACMDEN_3_Val           _U_(0x3)   /**< \brief (SDHC_TMR) Reserved */
#define SDHC_TMR_ACMDEN_DISABLED    (SDHC_TMR_ACMDEN_DISABLED_Val  << SDHC_TMR_ACMDEN_Pos)
#define SDHC_TMR_ACMDEN_CMD12       (SDHC_TMR_ACMDEN_CMD12_Val     << SDHC_TMR_ACMDEN_Pos)
#define SDHC_TMR_ACMDEN_CMD23       (SDHC_TMR_ACMDEN_CMD23_Val     << SDHC_TMR_ACMDEN_Pos)
#define SDHC_TMR_ACMDEN_3           (SDHC_TMR_ACMDEN_3_Val         << SDHC_TMR_ACMDEN_Pos)
#define SDHC_TMR_DTDSEL_Pos         4            /**< \brief (SDHC_TMR) Data Transfer Direction Selection */
#define SDHC_TMR_DTDSEL             (_U_(0x1) << SDHC_TMR_DTDSEL_Pos)
#define   SDHC_TMR_DTDSEL_WRITE_Val       _U_(0x0)   /**< \brief (SDHC_TMR) Write (Host to Card) */
#define   SDHC_TMR_DTDSEL_READ_Val        _U_(0x1)   /**< \brief (SDHC_TMR) Read (Card to Host) */
#define SDHC_TMR_DTDSEL_WRITE       (SDHC_TMR_DTDSEL_WRITE_Val     << SDHC_TMR_DTDSEL_Pos)
#define SDHC_TMR_DTDSEL_READ        (SDHC_TMR_DTDSEL_READ_Val      << SDHC_TMR_DTDSEL_Pos)
#define SDHC_TMR_MSBSEL_Pos         5            /**< \brief (SDHC_TMR) Multi/Single Block Selection */
#define SDHC_TMR_MSBSEL             (_U_(0x1) << SDHC_TMR_MSBSEL_Pos)
#define   SDHC_TMR_MSBSEL_SINGLE_Val      _U_(0x0)   /**< \brief (SDHC_TMR) Single Block */
#define   SDHC_TMR_MSBSEL_MULTIPLE_Val    _U_(0x1)   /**< \brief (SDHC_TMR) Multiple Block */
#define SDHC_TMR_MSBSEL_SINGLE      (SDHC_TMR_MSBSEL_SINGLE_Val    << SDHC_TMR_MSBSEL_Pos)
#define SDHC_TMR_MSBSEL_MULTIPLE    (SDHC_TMR_MSBSEL_MULTIPLE_Val  << SDHC_TMR_MSBSEL_Pos)
#define SDHC_TMR_MASK               _U_(0x003F)  /**< \brief (SDHC_TMR) MASK Register */

/* -------- SDHC_CR : (SDHC Offset: 0x00E) (R/W 16) Command -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t RESPTYP:2;        /*!< bit:  0.. 1  Response Type                      */
    uint16_t :1;               /*!< bit:      2  Reserved                           */
    uint16_t CMDCCEN:1;        /*!< bit:      3  Command CRC Check Enable           */
    uint16_t CMDICEN:1;        /*!< bit:      4  Command Index Check Enable         */
    uint16_t DPSEL:1;          /*!< bit:      5  Data Present Select                */
    uint16_t CMDTYP:2;         /*!< bit:  6.. 7  Command Type                       */
    uint16_t CMDIDX:6;         /*!< bit:  8..13  Command Index                      */
    uint16_t :2;               /*!< bit: 14..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_CR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_CR_OFFSET              0x00E        /**< \brief (SDHC_CR offset) Command */
#define SDHC_CR_RESETVALUE          _U_(0x0000)  /**< \brief (SDHC_CR reset_value) Command */

#define SDHC_CR_RESPTYP_Pos         0            /**< \brief (SDHC_CR) Response Type */
#define SDHC_CR_RESPTYP_Msk         (_U_(0x3) << SDHC_CR_RESPTYP_Pos)
#define SDHC_CR_RESPTYP(value)      (SDHC_CR_RESPTYP_Msk & ((value) << SDHC_CR_RESPTYP_Pos))
#define   SDHC_CR_RESPTYP_NONE_Val        _U_(0x0)   /**< \brief (SDHC_CR) No response */
#define   SDHC_CR_RESPTYP_136_BIT_Val     _U_(0x1)   /**< \brief (SDHC_CR) 136-bit response */
#define   SDHC_CR_RESPTYP_48_BIT_Val      _U_(0x2)   /**< \brief (SDHC_CR) 48-bit response */
#define   SDHC_CR_RESPTYP_48_BIT_BUSY_Val _U_(0x3)   /**< \brief (SDHC_CR) 48-bit response check busy after response */
#define SDHC_CR_RESPTYP_NONE        (SDHC_CR_RESPTYP_NONE_Val      << SDHC_CR_RESPTYP_Pos)
#define SDHC_CR_RESPTYP_136_BIT     (SDHC_CR_RESPTYP_136_BIT_Val   << SDHC_CR_RESPTYP_Pos)
#define SDHC_CR_RESPTYP_48_BIT      (SDHC_CR_RESPTYP_48_BIT_Val    << SDHC_CR_RESPTYP_Pos)
#define SDHC_CR_RESPTYP_48_BIT_BUSY (SDHC_CR_RESPTYP_48_BIT_BUSY_Val << SDHC_CR_RESPTYP_Pos)
#define SDHC_CR_CMDCCEN_Pos         3            /**< \brief (SDHC_CR) Command CRC Check Enable */
#define SDHC_CR_CMDCCEN             (_U_(0x1) << SDHC_CR_CMDCCEN_Pos)
#define   SDHC_CR_CMDCCEN_DISABLE_Val     _U_(0x0)   /**< \brief (SDHC_CR) Disable */
#define   SDHC_CR_CMDCCEN_ENABLE_Val      _U_(0x1)   /**< \brief (SDHC_CR) Enable */
#define SDHC_CR_CMDCCEN_DISABLE     (SDHC_CR_CMDCCEN_DISABLE_Val   << SDHC_CR_CMDCCEN_Pos)
#define SDHC_CR_CMDCCEN_ENABLE      (SDHC_CR_CMDCCEN_ENABLE_Val    << SDHC_CR_CMDCCEN_Pos)
#define SDHC_CR_CMDICEN_Pos         4            /**< \brief (SDHC_CR) Command Index Check Enable */
#define SDHC_CR_CMDICEN             (_U_(0x1) << SDHC_CR_CMDICEN_Pos)
#define   SDHC_CR_CMDICEN_DISABLE_Val     _U_(0x0)   /**< \brief (SDHC_CR) Disable */
#define   SDHC_CR_CMDICEN_ENABLE_Val      _U_(0x1)   /**< \brief (SDHC_CR) Enable */
#define SDHC_CR_CMDICEN_DISABLE     (SDHC_CR_CMDICEN_DISABLE_Val   << SDHC_CR_CMDICEN_Pos)
#define SDHC_CR_CMDICEN_ENABLE      (SDHC_CR_CMDICEN_ENABLE_Val    << SDHC_CR_CMDICEN_Pos)
#define SDHC_CR_DPSEL_Pos           5            /**< \brief (SDHC_CR) Data Present Select */
#define SDHC_CR_DPSEL               (_U_(0x1) << SDHC_CR_DPSEL_Pos)
#define   SDHC_CR_DPSEL_NO_DATA_Val       _U_(0x0)   /**< \brief (SDHC_CR) No Data Present */
#define   SDHC_CR_DPSEL_DATA_Val          _U_(0x1)   /**< \brief (SDHC_CR) Data Present */
#define SDHC_CR_DPSEL_NO_DATA       (SDHC_CR_DPSEL_NO_DATA_Val     << SDHC_CR_DPSEL_Pos)
#define SDHC_CR_DPSEL_DATA          (SDHC_CR_DPSEL_DATA_Val        << SDHC_CR_DPSEL_Pos)
#define SDHC_CR_CMDTYP_Pos          6            /**< \brief (SDHC_CR) Command Type */
#define SDHC_CR_CMDTYP_Msk          (_U_(0x3) << SDHC_CR_CMDTYP_Pos)
#define SDHC_CR_CMDTYP(value)       (SDHC_CR_CMDTYP_Msk & ((value) << SDHC_CR_CMDTYP_Pos))
#define   SDHC_CR_CMDTYP_NORMAL_Val       _U_(0x0)   /**< \brief (SDHC_CR) Other commands */
#define   SDHC_CR_CMDTYP_SUSPEND_Val      _U_(0x1)   /**< \brief (SDHC_CR) CMD52 for writing Bus Suspend in CCCR */
#define   SDHC_CR_CMDTYP_RESUME_Val       _U_(0x2)   /**< \brief (SDHC_CR) CMD52 for writing Function Select in CCCR */
#define   SDHC_CR_CMDTYP_ABORT_Val        _U_(0x3)   /**< \brief (SDHC_CR) CMD12, CMD52 for writing I/O Abort in CCCR */
#define SDHC_CR_CMDTYP_NORMAL       (SDHC_CR_CMDTYP_NORMAL_Val     << SDHC_CR_CMDTYP_Pos)
#define SDHC_CR_CMDTYP_SUSPEND      (SDHC_CR_CMDTYP_SUSPEND_Val    << SDHC_CR_CMDTYP_Pos)
#define SDHC_CR_CMDTYP_RESUME       (SDHC_CR_CMDTYP_RESUME_Val     << SDHC_CR_CMDTYP_Pos)
#define SDHC_CR_CMDTYP_ABORT        (SDHC_CR_CMDTYP_ABORT_Val      << SDHC_CR_CMDTYP_Pos)
#define SDHC_CR_CMDIDX_Pos          8            /**< \brief (SDHC_CR) Command Index */
#define SDHC_CR_CMDIDX_Msk          (_U_(0x3F) << SDHC_CR_CMDIDX_Pos)
#define SDHC_CR_CMDIDX(value)       (SDHC_CR_CMDIDX_Msk & ((value) << SDHC_CR_CMDIDX_Pos))
#define SDHC_CR_MASK                _U_(0x3FFB)  /**< \brief (SDHC_CR) MASK Register */

/* -------- SDHC_RR : (SDHC Offset: 0x010) (R/  32) Response -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CMDRESP:32;       /*!< bit:  0..31  Command Response                   */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SDHC_RR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_RR_OFFSET              0x010        /**< \brief (SDHC_RR offset) Response */
#define SDHC_RR_RESETVALUE          _U_(0x00000000) /**< \brief (SDHC_RR reset_value) Response */

#define SDHC_RR_CMDRESP_Pos         0            /**< \brief (SDHC_RR) Command Response */
#define SDHC_RR_CMDRESP_Msk         (_U_(0xFFFFFFFF) << SDHC_RR_CMDRESP_Pos)
#define SDHC_RR_CMDRESP(value)      (SDHC_RR_CMDRESP_Msk & ((value) << SDHC_RR_CMDRESP_Pos))
#define SDHC_RR_MASK                _U_(0xFFFFFFFF) /**< \brief (SDHC_RR) MASK Register */

/* -------- SDHC_BDPR : (SDHC Offset: 0x020) (R/W 32) Buffer Data Port -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BUFDATA:32;       /*!< bit:  0..31  Buffer Data                        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SDHC_BDPR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_BDPR_OFFSET            0x020        /**< \brief (SDHC_BDPR offset) Buffer Data Port */
#define SDHC_BDPR_RESETVALUE        _U_(0x00000000) /**< \brief (SDHC_BDPR reset_value) Buffer Data Port */

#define SDHC_BDPR_BUFDATA_Pos       0            /**< \brief (SDHC_BDPR) Buffer Data */
#define SDHC_BDPR_BUFDATA_Msk       (_U_(0xFFFFFFFF) << SDHC_BDPR_BUFDATA_Pos)
#define SDHC_BDPR_BUFDATA(value)    (SDHC_BDPR_BUFDATA_Msk & ((value) << SDHC_BDPR_BUFDATA_Pos))
#define SDHC_BDPR_MASK              _U_(0xFFFFFFFF) /**< \brief (SDHC_BDPR) MASK Register */

/* -------- SDHC_PSR : (SDHC Offset: 0x024) (R/  32) Present State -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CMDINHC:1;        /*!< bit:      0  Command Inhibit (CMD)              */
    uint32_t CMDINHD:1;        /*!< bit:      1  Command Inhibit (DAT)              */
    uint32_t DLACT:1;          /*!< bit:      2  DAT Line Active                    */
    uint32_t RTREQ:1;          /*!< bit:      3  Re-Tuning Request                  */
    uint32_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint32_t WTACT:1;          /*!< bit:      8  Write Transfer Active              */
    uint32_t RTACT:1;          /*!< bit:      9  Read Transfer Active               */
    uint32_t BUFWREN:1;        /*!< bit:     10  Buffer Write Enable                */
    uint32_t BUFRDEN:1;        /*!< bit:     11  Buffer Read Enable                 */
    uint32_t :4;               /*!< bit: 12..15  Reserved                           */
    uint32_t CARDINS:1;        /*!< bit:     16  Card Inserted                      */
    uint32_t CARDSS:1;         /*!< bit:     17  Card State Stable                  */
    uint32_t CARDDPL:1;        /*!< bit:     18  Card Detect Pin Level              */
    uint32_t WRPPL:1;          /*!< bit:     19  Write Protect Pin Level            */
    uint32_t DATLL:4;          /*!< bit: 20..23  DAT[3:0] Line Level                */
    uint32_t CMDLL:1;          /*!< bit:     24  CMD Line Level                     */
    uint32_t :7;               /*!< bit: 25..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SDHC_PSR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_PSR_OFFSET             0x024        /**< \brief (SDHC_PSR offset) Present State */
#define SDHC_PSR_RESETVALUE         _U_(0x00F80000) /**< \brief (SDHC_PSR reset_value) Present State */

#define SDHC_PSR_CMDINHC_Pos        0            /**< \brief (SDHC_PSR) Command Inhibit (CMD) */
#define SDHC_PSR_CMDINHC            (_U_(0x1) << SDHC_PSR_CMDINHC_Pos)
#define   SDHC_PSR_CMDINHC_CAN_Val        _U_(0x0)   /**< \brief (SDHC_PSR) Can issue command using only CMD line */
#define   SDHC_PSR_CMDINHC_CANNOT_Val     _U_(0x1)   /**< \brief (SDHC_PSR) Cannot issue command */
#define SDHC_PSR_CMDINHC_CAN        (SDHC_PSR_CMDINHC_CAN_Val      << SDHC_PSR_CMDINHC_Pos)
#define SDHC_PSR_CMDINHC_CANNOT     (SDHC_PSR_CMDINHC_CANNOT_Val   << SDHC_PSR_CMDINHC_Pos)
#define SDHC_PSR_CMDINHD_Pos        1            /**< \brief (SDHC_PSR) Command Inhibit (DAT) */
#define SDHC_PSR_CMDINHD            (_U_(0x1) << SDHC_PSR_CMDINHD_Pos)
#define   SDHC_PSR_CMDINHD_CAN_Val        _U_(0x0)   /**< \brief (SDHC_PSR) Can issue command which uses the DAT line */
#define   SDHC_PSR_CMDINHD_CANNOT_Val     _U_(0x1)   /**< \brief (SDHC_PSR) Cannot issue command which uses the DAT line */
#define SDHC_PSR_CMDINHD_CAN        (SDHC_PSR_CMDINHD_CAN_Val      << SDHC_PSR_CMDINHD_Pos)
#define SDHC_PSR_CMDINHD_CANNOT     (SDHC_PSR_CMDINHD_CANNOT_Val   << SDHC_PSR_CMDINHD_Pos)
#define SDHC_PSR_DLACT_Pos          2            /**< \brief (SDHC_PSR) DAT Line Active */
#define SDHC_PSR_DLACT              (_U_(0x1) << SDHC_PSR_DLACT_Pos)
#define   SDHC_PSR_DLACT_INACTIVE_Val     _U_(0x0)   /**< \brief (SDHC_PSR) DAT Line Inactive */
#define   SDHC_PSR_DLACT_ACTIVE_Val       _U_(0x1)   /**< \brief (SDHC_PSR) DAT Line Active */
#define SDHC_PSR_DLACT_INACTIVE     (SDHC_PSR_DLACT_INACTIVE_Val   << SDHC_PSR_DLACT_Pos)
#define SDHC_PSR_DLACT_ACTIVE       (SDHC_PSR_DLACT_ACTIVE_Val     << SDHC_PSR_DLACT_Pos)
#define SDHC_PSR_RTREQ_Pos          3            /**< \brief (SDHC_PSR) Re-Tuning Request */
#define SDHC_PSR_RTREQ              (_U_(0x1) << SDHC_PSR_RTREQ_Pos)
#define   SDHC_PSR_RTREQ_OK_Val           _U_(0x0)   /**< \brief (SDHC_PSR) Fixed or well-tuned sampling clock */
#define   SDHC_PSR_RTREQ_REQUIRED_Val     _U_(0x1)   /**< \brief (SDHC_PSR) Sampling clock needs re-tuning */
#define SDHC_PSR_RTREQ_OK           (SDHC_PSR_RTREQ_OK_Val         << SDHC_PSR_RTREQ_Pos)
#define SDHC_PSR_RTREQ_REQUIRED     (SDHC_PSR_RTREQ_REQUIRED_Val   << SDHC_PSR_RTREQ_Pos)
#define SDHC_PSR_WTACT_Pos          8            /**< \brief (SDHC_PSR) Write Transfer Active */
#define SDHC_PSR_WTACT              (_U_(0x1) << SDHC_PSR_WTACT_Pos)
#define   SDHC_PSR_WTACT_NO_Val           _U_(0x0)   /**< \brief (SDHC_PSR) No valid data */
#define   SDHC_PSR_WTACT_YES_Val          _U_(0x1)   /**< \brief (SDHC_PSR) Transferring data */
#define SDHC_PSR_WTACT_NO           (SDHC_PSR_WTACT_NO_Val         << SDHC_PSR_WTACT_Pos)
#define SDHC_PSR_WTACT_YES          (SDHC_PSR_WTACT_YES_Val        << SDHC_PSR_WTACT_Pos)
#define SDHC_PSR_RTACT_Pos          9            /**< \brief (SDHC_PSR) Read Transfer Active */
#define SDHC_PSR_RTACT              (_U_(0x1) << SDHC_PSR_RTACT_Pos)
#define   SDHC_PSR_RTACT_NO_Val           _U_(0x0)   /**< \brief (SDHC_PSR) No valid data */
#define   SDHC_PSR_RTACT_YES_Val          _U_(0x1)   /**< \brief (SDHC_PSR) Transferring data */
#define SDHC_PSR_RTACT_NO           (SDHC_PSR_RTACT_NO_Val         << SDHC_PSR_RTACT_Pos)
#define SDHC_PSR_RTACT_YES          (SDHC_PSR_RTACT_YES_Val        << SDHC_PSR_RTACT_Pos)
#define SDHC_PSR_BUFWREN_Pos        10           /**< \brief (SDHC_PSR) Buffer Write Enable */
#define SDHC_PSR_BUFWREN            (_U_(0x1) << SDHC_PSR_BUFWREN_Pos)
#define   SDHC_PSR_BUFWREN_DISABLE_Val    _U_(0x0)   /**< \brief (SDHC_PSR) Write disable */
#define   SDHC_PSR_BUFWREN_ENABLE_Val     _U_(0x1)   /**< \brief (SDHC_PSR) Write enable */
#define SDHC_PSR_BUFWREN_DISABLE    (SDHC_PSR_BUFWREN_DISABLE_Val  << SDHC_PSR_BUFWREN_Pos)
#define SDHC_PSR_BUFWREN_ENABLE     (SDHC_PSR_BUFWREN_ENABLE_Val   << SDHC_PSR_BUFWREN_Pos)
#define SDHC_PSR_BUFRDEN_Pos        11           /**< \brief (SDHC_PSR) Buffer Read Enable */
#define SDHC_PSR_BUFRDEN            (_U_(0x1) << SDHC_PSR_BUFRDEN_Pos)
#define   SDHC_PSR_BUFRDEN_DISABLE_Val    _U_(0x0)   /**< \brief (SDHC_PSR) Read disable */
#define   SDHC_PSR_BUFRDEN_ENABLE_Val     _U_(0x1)   /**< \brief (SDHC_PSR) Read enable */
#define SDHC_PSR_BUFRDEN_DISABLE    (SDHC_PSR_BUFRDEN_DISABLE_Val  << SDHC_PSR_BUFRDEN_Pos)
#define SDHC_PSR_BUFRDEN_ENABLE     (SDHC_PSR_BUFRDEN_ENABLE_Val   << SDHC_PSR_BUFRDEN_Pos)
#define SDHC_PSR_CARDINS_Pos        16           /**< \brief (SDHC_PSR) Card Inserted */
#define SDHC_PSR_CARDINS            (_U_(0x1) << SDHC_PSR_CARDINS_Pos)
#define   SDHC_PSR_CARDINS_NO_Val         _U_(0x0)   /**< \brief (SDHC_PSR) Reset or Debouncing or No Card */
#define   SDHC_PSR_CARDINS_YES_Val        _U_(0x1)   /**< \brief (SDHC_PSR) Card inserted */
#define SDHC_PSR_CARDINS_NO         (SDHC_PSR_CARDINS_NO_Val       << SDHC_PSR_CARDINS_Pos)
#define SDHC_PSR_CARDINS_YES        (SDHC_PSR_CARDINS_YES_Val      << SDHC_PSR_CARDINS_Pos)
#define SDHC_PSR_CARDSS_Pos         17           /**< \brief (SDHC_PSR) Card State Stable */
#define SDHC_PSR_CARDSS             (_U_(0x1) << SDHC_PSR_CARDSS_Pos)
#define   SDHC_PSR_CARDSS_NO_Val          _U_(0x0)   /**< \brief (SDHC_PSR) Reset or Debouncing */
#define   SDHC_PSR_CARDSS_YES_Val         _U_(0x1)   /**< \brief (SDHC_PSR) No Card or Insered */
#define SDHC_PSR_CARDSS_NO          (SDHC_PSR_CARDSS_NO_Val        << SDHC_PSR_CARDSS_Pos)
#define SDHC_PSR_CARDSS_YES         (SDHC_PSR_CARDSS_YES_Val       << SDHC_PSR_CARDSS_Pos)
#define SDHC_PSR_CARDDPL_Pos        18           /**< \brief (SDHC_PSR) Card Detect Pin Level */
#define SDHC_PSR_CARDDPL            (_U_(0x1) << SDHC_PSR_CARDDPL_Pos)
#define   SDHC_PSR_CARDDPL_NO_Val         _U_(0x0)   /**< \brief (SDHC_PSR) No card present (SDCD#=1) */
#define   SDHC_PSR_CARDDPL_YES_Val        _U_(0x1)   /**< \brief (SDHC_PSR) Card present (SDCD#=0) */
#define SDHC_PSR_CARDDPL_NO         (SDHC_PSR_CARDDPL_NO_Val       << SDHC_PSR_CARDDPL_Pos)
#define SDHC_PSR_CARDDPL_YES        (SDHC_PSR_CARDDPL_YES_Val      << SDHC_PSR_CARDDPL_Pos)
#define SDHC_PSR_WRPPL_Pos          19           /**< \brief (SDHC_PSR) Write Protect Pin Level */
#define SDHC_PSR_WRPPL              (_U_(0x1) << SDHC_PSR_WRPPL_Pos)
#define   SDHC_PSR_WRPPL_PROTECTED_Val    _U_(0x0)   /**< \brief (SDHC_PSR) Write protected (SDWP#=0) */
#define   SDHC_PSR_WRPPL_ENABLED_Val      _U_(0x1)   /**< \brief (SDHC_PSR) Write enabled (SDWP#=1) */
#define SDHC_PSR_WRPPL_PROTECTED    (SDHC_PSR_WRPPL_PROTECTED_Val  << SDHC_PSR_WRPPL_Pos)
#define SDHC_PSR_WRPPL_ENABLED      (SDHC_PSR_WRPPL_ENABLED_Val    << SDHC_PSR_WRPPL_Pos)
#define SDHC_PSR_DATLL_Pos          20           /**< \brief (SDHC_PSR) DAT[3:0] Line Level */
#define SDHC_PSR_DATLL_Msk          (_U_(0xF) << SDHC_PSR_DATLL_Pos)
#define SDHC_PSR_DATLL(value)       (SDHC_PSR_DATLL_Msk & ((value) << SDHC_PSR_DATLL_Pos))
#define SDHC_PSR_CMDLL_Pos          24           /**< \brief (SDHC_PSR) CMD Line Level */
#define SDHC_PSR_CMDLL              (_U_(0x1) << SDHC_PSR_CMDLL_Pos)
#define SDHC_PSR_MASK               _U_(0x01FF0F0F) /**< \brief (SDHC_PSR) MASK Register */

/* -------- SDHC_HC1R : (SDHC Offset: 0x028) (R/W  8) Host Control 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  LEDCTRL:1;        /*!< bit:      0  LED Control                        */
    uint8_t  DW:1;             /*!< bit:      1  Data Width                         */
    uint8_t  HSEN:1;           /*!< bit:      2  High Speed Enable                  */
    uint8_t  DMASEL:2;         /*!< bit:  3.. 4  DMA Select                         */
    uint8_t  :1;               /*!< bit:      5  Reserved                           */
    uint8_t  CARDDTL:1;        /*!< bit:      6  Card Detect Test Level             */
    uint8_t  CARDDSEL:1;       /*!< bit:      7  Card Detect Signal Selection       */
  } bit;                       /*!< Structure used for bit  access                  */
  struct { // EMMC mode
    uint8_t  :1;               /*!< bit:      0  Reserved                           */
    uint8_t  DW:1;             /*!< bit:      1  Data Width                         */
    uint8_t  HSEN:1;           /*!< bit:      2  High Speed Enable                  */
    uint8_t  DMASEL:2;         /*!< bit:  3.. 4  DMA Select                         */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } EMMC;                      /*!< Structure used for EMMC                         */
  uint8_t reg;                 /*!< Type      used for register access              */
} SDHC_HC1R_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_HC1R_OFFSET            0x028        /**< \brief (SDHC_HC1R offset) Host Control 1 */
#define SDHC_HC1R_RESETVALUE        _U_(0xE00)   /**< \brief (SDHC_HC1R reset_value) Host Control 1 */

#define SDHC_HC1R_LEDCTRL_Pos       0            /**< \brief (SDHC_HC1R) LED Control */
#define SDHC_HC1R_LEDCTRL           (_U_(0x1) << SDHC_HC1R_LEDCTRL_Pos)
#define   SDHC_HC1R_LEDCTRL_OFF_Val       _U_(0x0)   /**< \brief (SDHC_HC1R) LED off */
#define   SDHC_HC1R_LEDCTRL_ON_Val        _U_(0x1)   /**< \brief (SDHC_HC1R) LED on */
#define SDHC_HC1R_LEDCTRL_OFF       (SDHC_HC1R_LEDCTRL_OFF_Val     << SDHC_HC1R_LEDCTRL_Pos)
#define SDHC_HC1R_LEDCTRL_ON        (SDHC_HC1R_LEDCTRL_ON_Val      << SDHC_HC1R_LEDCTRL_Pos)
#define SDHC_HC1R_DW_Pos            1            /**< \brief (SDHC_HC1R) Data Width */
#define SDHC_HC1R_DW                (_U_(0x1) << SDHC_HC1R_DW_Pos)
#define   SDHC_HC1R_DW_1BIT_Val           _U_(0x0)   /**< \brief (SDHC_HC1R) 1-bit mode */
#define   SDHC_HC1R_DW_4BIT_Val           _U_(0x1)   /**< \brief (SDHC_HC1R) 4-bit mode */
#define SDHC_HC1R_DW_1BIT           (SDHC_HC1R_DW_1BIT_Val         << SDHC_HC1R_DW_Pos)
#define SDHC_HC1R_DW_4BIT           (SDHC_HC1R_DW_4BIT_Val         << SDHC_HC1R_DW_Pos)
#define SDHC_HC1R_HSEN_Pos          2            /**< \brief (SDHC_HC1R) High Speed Enable */
#define SDHC_HC1R_HSEN              (_U_(0x1) << SDHC_HC1R_HSEN_Pos)
#define   SDHC_HC1R_HSEN_NORMAL_Val       _U_(0x0)   /**< \brief (SDHC_HC1R) Normal Speed mode */
#define   SDHC_HC1R_HSEN_HIGH_Val         _U_(0x1)   /**< \brief (SDHC_HC1R) High Speed mode */
#define SDHC_HC1R_HSEN_NORMAL       (SDHC_HC1R_HSEN_NORMAL_Val     << SDHC_HC1R_HSEN_Pos)
#define SDHC_HC1R_HSEN_HIGH         (SDHC_HC1R_HSEN_HIGH_Val       << SDHC_HC1R_HSEN_Pos)
#define SDHC_HC1R_DMASEL_Pos        3            /**< \brief (SDHC_HC1R) DMA Select */
#define SDHC_HC1R_DMASEL_Msk        (_U_(0x3) << SDHC_HC1R_DMASEL_Pos)
#define SDHC_HC1R_DMASEL(value)     (SDHC_HC1R_DMASEL_Msk & ((value) << SDHC_HC1R_DMASEL_Pos))
#define   SDHC_HC1R_DMASEL_SDMA_Val       _U_(0x0)   /**< \brief (SDHC_HC1R) SDMA is selected */
#define   SDHC_HC1R_DMASEL_1_Val          _U_(0x1)   /**< \brief (SDHC_HC1R) Reserved */
#define   SDHC_HC1R_DMASEL_32BIT_Val      _U_(0x2)   /**< \brief (SDHC_HC1R) 32-bit Address ADMA2 is selected */
#define SDHC_HC1R_DMASEL_SDMA       (SDHC_HC1R_DMASEL_SDMA_Val     << SDHC_HC1R_DMASEL_Pos)
#define SDHC_HC1R_DMASEL_1          (SDHC_HC1R_DMASEL_1_Val        << SDHC_HC1R_DMASEL_Pos)
#define SDHC_HC1R_DMASEL_32BIT      (SDHC_HC1R_DMASEL_32BIT_Val    << SDHC_HC1R_DMASEL_Pos)
#define SDHC_HC1R_CARDDTL_Pos       6            /**< \brief (SDHC_HC1R) Card Detect Test Level */
#define SDHC_HC1R_CARDDTL           (_U_(0x1) << SDHC_HC1R_CARDDTL_Pos)
#define   SDHC_HC1R_CARDDTL_NO_Val        _U_(0x0)   /**< \brief (SDHC_HC1R) No Card */
#define   SDHC_HC1R_CARDDTL_YES_Val       _U_(0x1)   /**< \brief (SDHC_HC1R) Card Inserted */
#define SDHC_HC1R_CARDDTL_NO        (SDHC_HC1R_CARDDTL_NO_Val      << SDHC_HC1R_CARDDTL_Pos)
#define SDHC_HC1R_CARDDTL_YES       (SDHC_HC1R_CARDDTL_YES_Val     << SDHC_HC1R_CARDDTL_Pos)
#define SDHC_HC1R_CARDDSEL_Pos      7            /**< \brief (SDHC_HC1R) Card Detect Signal Selection */
#define SDHC_HC1R_CARDDSEL          (_U_(0x1) << SDHC_HC1R_CARDDSEL_Pos)
#define   SDHC_HC1R_CARDDSEL_NORMAL_Val   _U_(0x0)   /**< \brief (SDHC_HC1R) SDCD# is selected (for normal use) */
#define   SDHC_HC1R_CARDDSEL_TEST_Val     _U_(0x1)   /**< \brief (SDHC_HC1R) The Card Select Test Level is selected (for test purpose) */
#define SDHC_HC1R_CARDDSEL_NORMAL   (SDHC_HC1R_CARDDSEL_NORMAL_Val << SDHC_HC1R_CARDDSEL_Pos)
#define SDHC_HC1R_CARDDSEL_TEST     (SDHC_HC1R_CARDDSEL_TEST_Val   << SDHC_HC1R_CARDDSEL_Pos)
#define SDHC_HC1R_MASK              _U_(0xDF)    /**< \brief (SDHC_HC1R) MASK Register */

// EMMC mode
#define SDHC_HC1R_EMMC_DW_Pos       1            /**< \brief (SDHC_HC1R_EMMC) Data Width */
#define SDHC_HC1R_EMMC_DW           (_U_(0x1) << SDHC_HC1R_EMMC_DW_Pos)
#define   SDHC_HC1R_EMMC_DW_1BIT_Val      _U_(0x0)   /**< \brief (SDHC_HC1R_EMMC) 1-bit mode */
#define   SDHC_HC1R_EMMC_DW_4BIT_Val      _U_(0x1)   /**< \brief (SDHC_HC1R_EMMC) 4-bit mode */
#define SDHC_HC1R_EMMC_DW_1BIT      (SDHC_HC1R_EMMC_DW_1BIT_Val    << SDHC_HC1R_EMMC_DW_Pos)
#define SDHC_HC1R_EMMC_DW_4BIT      (SDHC_HC1R_EMMC_DW_4BIT_Val    << SDHC_HC1R_EMMC_DW_Pos)
#define SDHC_HC1R_EMMC_HSEN_Pos     2            /**< \brief (SDHC_HC1R_EMMC) High Speed Enable */
#define SDHC_HC1R_EMMC_HSEN         (_U_(0x1) << SDHC_HC1R_EMMC_HSEN_Pos)
#define   SDHC_HC1R_EMMC_HSEN_NORMAL_Val  _U_(0x0)   /**< \brief (SDHC_HC1R_EMMC) Normal Speed mode */
#define   SDHC_HC1R_EMMC_HSEN_HIGH_Val    _U_(0x1)   /**< \brief (SDHC_HC1R_EMMC) High Speed mode */
#define SDHC_HC1R_EMMC_HSEN_NORMAL  (SDHC_HC1R_EMMC_HSEN_NORMAL_Val << SDHC_HC1R_EMMC_HSEN_Pos)
#define SDHC_HC1R_EMMC_HSEN_HIGH    (SDHC_HC1R_EMMC_HSEN_HIGH_Val  << SDHC_HC1R_EMMC_HSEN_Pos)
#define SDHC_HC1R_EMMC_DMASEL_Pos   3            /**< \brief (SDHC_HC1R_EMMC) DMA Select */
#define SDHC_HC1R_EMMC_DMASEL_Msk   (_U_(0x3) << SDHC_HC1R_EMMC_DMASEL_Pos)
#define SDHC_HC1R_EMMC_DMASEL(value) (SDHC_HC1R_EMMC_DMASEL_Msk & ((value) << SDHC_HC1R_EMMC_DMASEL_Pos))
#define   SDHC_HC1R_EMMC_DMASEL_SDMA_Val  _U_(0x0)   /**< \brief (SDHC_HC1R_EMMC) SDMA is selected */
#define   SDHC_HC1R_EMMC_DMASEL_1_Val     _U_(0x1)   /**< \brief (SDHC_HC1R_EMMC) Reserved */
#define   SDHC_HC1R_EMMC_DMASEL_32BIT_Val _U_(0x2)   /**< \brief (SDHC_HC1R_EMMC) 32-bit Address ADMA2 is selected */
#define SDHC_HC1R_EMMC_DMASEL_SDMA  (SDHC_HC1R_EMMC_DMASEL_SDMA_Val << SDHC_HC1R_EMMC_DMASEL_Pos)
#define SDHC_HC1R_EMMC_DMASEL_1     (SDHC_HC1R_EMMC_DMASEL_1_Val   << SDHC_HC1R_EMMC_DMASEL_Pos)
#define SDHC_HC1R_EMMC_DMASEL_32BIT (SDHC_HC1R_EMMC_DMASEL_32BIT_Val << SDHC_HC1R_EMMC_DMASEL_Pos)
#define SDHC_HC1R_EMMC_MASK         _U_(0x1E)    /**< \brief (SDHC_HC1R_EMMC) MASK Register */

/* -------- SDHC_PCR : (SDHC Offset: 0x029) (R/W  8) Power Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SDBPWR:1;         /*!< bit:      0  SD Bus Power                       */
    uint8_t  SDBVSEL:3;        /*!< bit:  1.. 3  SD Bus Voltage Select              */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} SDHC_PCR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_PCR_OFFSET             0x029        /**< \brief (SDHC_PCR offset) Power Control */
#define SDHC_PCR_RESETVALUE         _U_(0x0E)    /**< \brief (SDHC_PCR reset_value) Power Control */

#define SDHC_PCR_SDBPWR_Pos         0            /**< \brief (SDHC_PCR) SD Bus Power */
#define SDHC_PCR_SDBPWR             (_U_(0x1) << SDHC_PCR_SDBPWR_Pos)
#define   SDHC_PCR_SDBPWR_OFF_Val         _U_(0x0)   /**< \brief (SDHC_PCR) Power off */
#define   SDHC_PCR_SDBPWR_ON_Val          _U_(0x1)   /**< \brief (SDHC_PCR) Power on */
#define SDHC_PCR_SDBPWR_OFF         (SDHC_PCR_SDBPWR_OFF_Val       << SDHC_PCR_SDBPWR_Pos)
#define SDHC_PCR_SDBPWR_ON          (SDHC_PCR_SDBPWR_ON_Val        << SDHC_PCR_SDBPWR_Pos)
#define SDHC_PCR_SDBVSEL_Pos        1            /**< \brief (SDHC_PCR) SD Bus Voltage Select */
#define SDHC_PCR_SDBVSEL_Msk        (_U_(0x7) << SDHC_PCR_SDBVSEL_Pos)
#define SDHC_PCR_SDBVSEL(value)     (SDHC_PCR_SDBVSEL_Msk & ((value) << SDHC_PCR_SDBVSEL_Pos))
#define   SDHC_PCR_SDBVSEL_1V8_Val        _U_(0x5)   /**< \brief (SDHC_PCR) 1.8V (Typ.) */
#define   SDHC_PCR_SDBVSEL_3V0_Val        _U_(0x6)   /**< \brief (SDHC_PCR) 3.0V (Typ.) */
#define   SDHC_PCR_SDBVSEL_3V3_Val        _U_(0x7)   /**< \brief (SDHC_PCR) 3.3V (Typ.) */
#define SDHC_PCR_SDBVSEL_1V8        (SDHC_PCR_SDBVSEL_1V8_Val      << SDHC_PCR_SDBVSEL_Pos)
#define SDHC_PCR_SDBVSEL_3V0        (SDHC_PCR_SDBVSEL_3V0_Val      << SDHC_PCR_SDBVSEL_Pos)
#define SDHC_PCR_SDBVSEL_3V3        (SDHC_PCR_SDBVSEL_3V3_Val      << SDHC_PCR_SDBVSEL_Pos)
#define SDHC_PCR_MASK               _U_(0x0F)    /**< \brief (SDHC_PCR) MASK Register */

/* -------- SDHC_BGCR : (SDHC Offset: 0x02A) (R/W  8) Block Gap Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  STPBGR:1;         /*!< bit:      0  Stop at Block Gap Request          */
    uint8_t  CONTR:1;          /*!< bit:      1  Continue Request                   */
    uint8_t  RWCTRL:1;         /*!< bit:      2  Read Wait Control                  */
    uint8_t  INTBG:1;          /*!< bit:      3  Interrupt at Block Gap             */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct { // EMMC mode
    uint8_t  STPBGR:1;         /*!< bit:      0  Stop at Block Gap Request          */
    uint8_t  CONTR:1;          /*!< bit:      1  Continue Request                   */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } EMMC;                      /*!< Structure used for EMMC                         */
  uint8_t reg;                 /*!< Type      used for register access              */
} SDHC_BGCR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_BGCR_OFFSET            0x02A        /**< \brief (SDHC_BGCR offset) Block Gap Control */
#define SDHC_BGCR_RESETVALUE        _U_(0x00)    /**< \brief (SDHC_BGCR reset_value) Block Gap Control */

#define SDHC_BGCR_STPBGR_Pos        0            /**< \brief (SDHC_BGCR) Stop at Block Gap Request */
#define SDHC_BGCR_STPBGR            (_U_(0x1) << SDHC_BGCR_STPBGR_Pos)
#define   SDHC_BGCR_STPBGR_TRANSFER_Val   _U_(0x0)   /**< \brief (SDHC_BGCR) Transfer */
#define   SDHC_BGCR_STPBGR_STOP_Val       _U_(0x1)   /**< \brief (SDHC_BGCR) Stop */
#define SDHC_BGCR_STPBGR_TRANSFER   (SDHC_BGCR_STPBGR_TRANSFER_Val << SDHC_BGCR_STPBGR_Pos)
#define SDHC_BGCR_STPBGR_STOP       (SDHC_BGCR_STPBGR_STOP_Val     << SDHC_BGCR_STPBGR_Pos)
#define SDHC_BGCR_CONTR_Pos         1            /**< \brief (SDHC_BGCR) Continue Request */
#define SDHC_BGCR_CONTR             (_U_(0x1) << SDHC_BGCR_CONTR_Pos)
#define   SDHC_BGCR_CONTR_GO_ON_Val       _U_(0x0)   /**< \brief (SDHC_BGCR) Not affected */
#define   SDHC_BGCR_CONTR_RESTART_Val     _U_(0x1)   /**< \brief (SDHC_BGCR) Restart */
#define SDHC_BGCR_CONTR_GO_ON       (SDHC_BGCR_CONTR_GO_ON_Val     << SDHC_BGCR_CONTR_Pos)
#define SDHC_BGCR_CONTR_RESTART     (SDHC_BGCR_CONTR_RESTART_Val   << SDHC_BGCR_CONTR_Pos)
#define SDHC_BGCR_RWCTRL_Pos        2            /**< \brief (SDHC_BGCR) Read Wait Control */
#define SDHC_BGCR_RWCTRL            (_U_(0x1) << SDHC_BGCR_RWCTRL_Pos)
#define   SDHC_BGCR_RWCTRL_DISABLE_Val    _U_(0x0)   /**< \brief (SDHC_BGCR) Disable Read Wait Control */
#define   SDHC_BGCR_RWCTRL_ENABLE_Val     _U_(0x1)   /**< \brief (SDHC_BGCR) Enable Read Wait Control */
#define SDHC_BGCR_RWCTRL_DISABLE    (SDHC_BGCR_RWCTRL_DISABLE_Val  << SDHC_BGCR_RWCTRL_Pos)
#define SDHC_BGCR_RWCTRL_ENABLE     (SDHC_BGCR_RWCTRL_ENABLE_Val   << SDHC_BGCR_RWCTRL_Pos)
#define SDHC_BGCR_INTBG_Pos         3            /**< \brief (SDHC_BGCR) Interrupt at Block Gap */
#define SDHC_BGCR_INTBG             (_U_(0x1) << SDHC_BGCR_INTBG_Pos)
#define   SDHC_BGCR_INTBG_DISABLED_Val    _U_(0x0)   /**< \brief (SDHC_BGCR) Disabled */
#define   SDHC_BGCR_INTBG_ENABLED_Val     _U_(0x1)   /**< \brief (SDHC_BGCR) Enabled */
#define SDHC_BGCR_INTBG_DISABLED    (SDHC_BGCR_INTBG_DISABLED_Val  << SDHC_BGCR_INTBG_Pos)
#define SDHC_BGCR_INTBG_ENABLED     (SDHC_BGCR_INTBG_ENABLED_Val   << SDHC_BGCR_INTBG_Pos)
#define SDHC_BGCR_MASK              _U_(0x0F)    /**< \brief (SDHC_BGCR) MASK Register */

// EMMC mode
#define SDHC_BGCR_EMMC_STPBGR_Pos   0            /**< \brief (SDHC_BGCR_EMMC) Stop at Block Gap Request */
#define SDHC_BGCR_EMMC_STPBGR       (_U_(0x1) << SDHC_BGCR_EMMC_STPBGR_Pos)
#define   SDHC_BGCR_EMMC_STPBGR_TRANSFER_Val _U_(0x0)   /**< \brief (SDHC_BGCR_EMMC) Transfer */
#define   SDHC_BGCR_EMMC_STPBGR_STOP_Val  _U_(0x1)   /**< \brief (SDHC_BGCR_EMMC) Stop */
#define SDHC_BGCR_EMMC_STPBGR_TRANSFER (SDHC_BGCR_EMMC_STPBGR_TRANSFER_Val << SDHC_BGCR_EMMC_STPBGR_Pos)
#define SDHC_BGCR_EMMC_STPBGR_STOP  (SDHC_BGCR_EMMC_STPBGR_STOP_Val << SDHC_BGCR_EMMC_STPBGR_Pos)
#define SDHC_BGCR_EMMC_CONTR_Pos    1            /**< \brief (SDHC_BGCR_EMMC) Continue Request */
#define SDHC_BGCR_EMMC_CONTR        (_U_(0x1) << SDHC_BGCR_EMMC_CONTR_Pos)
#define   SDHC_BGCR_EMMC_CONTR_GO_ON_Val  _U_(0x0)   /**< \brief (SDHC_BGCR_EMMC) Not affected */
#define   SDHC_BGCR_EMMC_CONTR_RESTART_Val _U_(0x1)   /**< \brief (SDHC_BGCR_EMMC) Restart */
#define SDHC_BGCR_EMMC_CONTR_GO_ON  (SDHC_BGCR_EMMC_CONTR_GO_ON_Val << SDHC_BGCR_EMMC_CONTR_Pos)
#define SDHC_BGCR_EMMC_CONTR_RESTART (SDHC_BGCR_EMMC_CONTR_RESTART_Val << SDHC_BGCR_EMMC_CONTR_Pos)
#define SDHC_BGCR_EMMC_MASK         _U_(0x03)    /**< \brief (SDHC_BGCR_EMMC) MASK Register */

/* -------- SDHC_WCR : (SDHC Offset: 0x02B) (R/W  8) Wakeup Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  WKENCINT:1;       /*!< bit:      0  Wakeup Event Enable on Card Interrupt */
    uint8_t  WKENCINS:1;       /*!< bit:      1  Wakeup Event Enable on Card Insertion */
    uint8_t  WKENCREM:1;       /*!< bit:      2  Wakeup Event Enable on Card Removal */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} SDHC_WCR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_WCR_OFFSET             0x02B        /**< \brief (SDHC_WCR offset) Wakeup Control */
#define SDHC_WCR_RESETVALUE         _U_(0x00)    /**< \brief (SDHC_WCR reset_value) Wakeup Control */

#define SDHC_WCR_WKENCINT_Pos       0            /**< \brief (SDHC_WCR) Wakeup Event Enable on Card Interrupt */
#define SDHC_WCR_WKENCINT           (_U_(0x1) << SDHC_WCR_WKENCINT_Pos)
#define   SDHC_WCR_WKENCINT_DISABLE_Val   _U_(0x0)   /**< \brief (SDHC_WCR) Disable */
#define   SDHC_WCR_WKENCINT_ENABLE_Val    _U_(0x1)   /**< \brief (SDHC_WCR) Enable */
#define SDHC_WCR_WKENCINT_DISABLE   (SDHC_WCR_WKENCINT_DISABLE_Val << SDHC_WCR_WKENCINT_Pos)
#define SDHC_WCR_WKENCINT_ENABLE    (SDHC_WCR_WKENCINT_ENABLE_Val  << SDHC_WCR_WKENCINT_Pos)
#define SDHC_WCR_WKENCINS_Pos       1            /**< \brief (SDHC_WCR) Wakeup Event Enable on Card Insertion */
#define SDHC_WCR_WKENCINS           (_U_(0x1) << SDHC_WCR_WKENCINS_Pos)
#define   SDHC_WCR_WKENCINS_DISABLE_Val   _U_(0x0)   /**< \brief (SDHC_WCR) Disable */
#define   SDHC_WCR_WKENCINS_ENABLE_Val    _U_(0x1)   /**< \brief (SDHC_WCR) Enable */
#define SDHC_WCR_WKENCINS_DISABLE   (SDHC_WCR_WKENCINS_DISABLE_Val << SDHC_WCR_WKENCINS_Pos)
#define SDHC_WCR_WKENCINS_ENABLE    (SDHC_WCR_WKENCINS_ENABLE_Val  << SDHC_WCR_WKENCINS_Pos)
#define SDHC_WCR_WKENCREM_Pos       2            /**< \brief (SDHC_WCR) Wakeup Event Enable on Card Removal */
#define SDHC_WCR_WKENCREM           (_U_(0x1) << SDHC_WCR_WKENCREM_Pos)
#define   SDHC_WCR_WKENCREM_DISABLE_Val   _U_(0x0)   /**< \brief (SDHC_WCR) Disable */
#define   SDHC_WCR_WKENCREM_ENABLE_Val    _U_(0x1)   /**< \brief (SDHC_WCR) Enable */
#define SDHC_WCR_WKENCREM_DISABLE   (SDHC_WCR_WKENCREM_DISABLE_Val << SDHC_WCR_WKENCREM_Pos)
#define SDHC_WCR_WKENCREM_ENABLE    (SDHC_WCR_WKENCREM_ENABLE_Val  << SDHC_WCR_WKENCREM_Pos)
#define SDHC_WCR_MASK               _U_(0x07)    /**< \brief (SDHC_WCR) MASK Register */

/* -------- SDHC_CCR : (SDHC Offset: 0x02C) (R/W 16) Clock Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t INTCLKEN:1;       /*!< bit:      0  Internal Clock Enable              */
    uint16_t INTCLKS:1;        /*!< bit:      1  Internal Clock Stable              */
    uint16_t SDCLKEN:1;        /*!< bit:      2  SD Clock Enable                    */
    uint16_t :2;               /*!< bit:  3.. 4  Reserved                           */
    uint16_t CLKGSEL:1;        /*!< bit:      5  Clock Generator Select             */
    uint16_t USDCLKFSEL:2;     /*!< bit:  6.. 7  Upper Bits of SDCLK Frequency Select */
    uint16_t SDCLKFSEL:8;      /*!< bit:  8..15  SDCLK Frequency Select             */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_CCR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_CCR_OFFSET             0x02C        /**< \brief (SDHC_CCR offset) Clock Control */
#define SDHC_CCR_RESETVALUE         _U_(0x0000)  /**< \brief (SDHC_CCR reset_value) Clock Control */

#define SDHC_CCR_INTCLKEN_Pos       0            /**< \brief (SDHC_CCR) Internal Clock Enable */
#define SDHC_CCR_INTCLKEN           (_U_(0x1) << SDHC_CCR_INTCLKEN_Pos)
#define   SDHC_CCR_INTCLKEN_OFF_Val       _U_(0x0)   /**< \brief (SDHC_CCR) Stop */
#define   SDHC_CCR_INTCLKEN_ON_Val        _U_(0x1)   /**< \brief (SDHC_CCR) Oscillate */
#define SDHC_CCR_INTCLKEN_OFF       (SDHC_CCR_INTCLKEN_OFF_Val     << SDHC_CCR_INTCLKEN_Pos)
#define SDHC_CCR_INTCLKEN_ON        (SDHC_CCR_INTCLKEN_ON_Val      << SDHC_CCR_INTCLKEN_Pos)
#define SDHC_CCR_INTCLKS_Pos        1            /**< \brief (SDHC_CCR) Internal Clock Stable */
#define SDHC_CCR_INTCLKS            (_U_(0x1) << SDHC_CCR_INTCLKS_Pos)
#define   SDHC_CCR_INTCLKS_NOT_READY_Val  _U_(0x0)   /**< \brief (SDHC_CCR) Not Ready */
#define   SDHC_CCR_INTCLKS_READY_Val      _U_(0x1)   /**< \brief (SDHC_CCR) Ready */
#define SDHC_CCR_INTCLKS_NOT_READY  (SDHC_CCR_INTCLKS_NOT_READY_Val << SDHC_CCR_INTCLKS_Pos)
#define SDHC_CCR_INTCLKS_READY      (SDHC_CCR_INTCLKS_READY_Val    << SDHC_CCR_INTCLKS_Pos)
#define SDHC_CCR_SDCLKEN_Pos        2            /**< \brief (SDHC_CCR) SD Clock Enable */
#define SDHC_CCR_SDCLKEN            (_U_(0x1) << SDHC_CCR_SDCLKEN_Pos)
#define   SDHC_CCR_SDCLKEN_DISABLE_Val    _U_(0x0)   /**< \brief (SDHC_CCR) Disable */
#define   SDHC_CCR_SDCLKEN_ENABLE_Val     _U_(0x1)   /**< \brief (SDHC_CCR) Enable */
#define SDHC_CCR_SDCLKEN_DISABLE    (SDHC_CCR_SDCLKEN_DISABLE_Val  << SDHC_CCR_SDCLKEN_Pos)
#define SDHC_CCR_SDCLKEN_ENABLE     (SDHC_CCR_SDCLKEN_ENABLE_Val   << SDHC_CCR_SDCLKEN_Pos)
#define SDHC_CCR_CLKGSEL_Pos        5            /**< \brief (SDHC_CCR) Clock Generator Select */
#define SDHC_CCR_CLKGSEL            (_U_(0x1) << SDHC_CCR_CLKGSEL_Pos)
#define   SDHC_CCR_CLKGSEL_DIV_Val        _U_(0x0)   /**< \brief (SDHC_CCR) Divided Clock Mode */
#define   SDHC_CCR_CLKGSEL_PROG_Val       _U_(0x1)   /**< \brief (SDHC_CCR) Programmable Clock Mode */
#define SDHC_CCR_CLKGSEL_DIV        (SDHC_CCR_CLKGSEL_DIV_Val      << SDHC_CCR_CLKGSEL_Pos)
#define SDHC_CCR_CLKGSEL_PROG       (SDHC_CCR_CLKGSEL_PROG_Val     << SDHC_CCR_CLKGSEL_Pos)
#define SDHC_CCR_USDCLKFSEL_Pos     6            /**< \brief (SDHC_CCR) Upper Bits of SDCLK Frequency Select */
#define SDHC_CCR_USDCLKFSEL_Msk     (_U_(0x3) << SDHC_CCR_USDCLKFSEL_Pos)
#define SDHC_CCR_USDCLKFSEL(value)  (SDHC_CCR_USDCLKFSEL_Msk & ((value) << SDHC_CCR_USDCLKFSEL_Pos))
#define SDHC_CCR_SDCLKFSEL_Pos      8            /**< \brief (SDHC_CCR) SDCLK Frequency Select */
#define SDHC_CCR_SDCLKFSEL_Msk      (_U_(0xFF) << SDHC_CCR_SDCLKFSEL_Pos)
#define SDHC_CCR_SDCLKFSEL(value)   (SDHC_CCR_SDCLKFSEL_Msk & ((value) << SDHC_CCR_SDCLKFSEL_Pos))
#define SDHC_CCR_MASK               _U_(0xFFE7)  /**< \brief (SDHC_CCR) MASK Register */

/* -------- SDHC_TCR : (SDHC Offset: 0x02E) (R/W  8) Timeout Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DTCVAL:4;         /*!< bit:  0.. 3  Data Timeout Counter Value         */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} SDHC_TCR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_TCR_OFFSET             0x02E        /**< \brief (SDHC_TCR offset) Timeout Control */
#define SDHC_TCR_RESETVALUE         _U_(0x00)    /**< \brief (SDHC_TCR reset_value) Timeout Control */

#define SDHC_TCR_DTCVAL_Pos         0            /**< \brief (SDHC_TCR) Data Timeout Counter Value */
#define SDHC_TCR_DTCVAL_Msk         (_U_(0xF) << SDHC_TCR_DTCVAL_Pos)
#define SDHC_TCR_DTCVAL(value)      (SDHC_TCR_DTCVAL_Msk & ((value) << SDHC_TCR_DTCVAL_Pos))
#define SDHC_TCR_MASK               _U_(0x0F)    /**< \brief (SDHC_TCR) MASK Register */

/* -------- SDHC_SRR : (SDHC Offset: 0x02F) (R/W  8) Software Reset -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRSTALL:1;       /*!< bit:      0  Software Reset For All             */
    uint8_t  SWRSTCMD:1;       /*!< bit:      1  Software Reset For CMD Line        */
    uint8_t  SWRSTDAT:1;       /*!< bit:      2  Software Reset For DAT Line        */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} SDHC_SRR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_SRR_OFFSET             0x02F        /**< \brief (SDHC_SRR offset) Software Reset */
#define SDHC_SRR_RESETVALUE         _U_(0x00)    /**< \brief (SDHC_SRR reset_value) Software Reset */

#define SDHC_SRR_SWRSTALL_Pos       0            /**< \brief (SDHC_SRR) Software Reset For All */
#define SDHC_SRR_SWRSTALL           (_U_(0x1) << SDHC_SRR_SWRSTALL_Pos)
#define   SDHC_SRR_SWRSTALL_WORK_Val      _U_(0x0)   /**< \brief (SDHC_SRR) Work */
#define   SDHC_SRR_SWRSTALL_RESET_Val     _U_(0x1)   /**< \brief (SDHC_SRR) Reset */
#define SDHC_SRR_SWRSTALL_WORK      (SDHC_SRR_SWRSTALL_WORK_Val    << SDHC_SRR_SWRSTALL_Pos)
#define SDHC_SRR_SWRSTALL_RESET     (SDHC_SRR_SWRSTALL_RESET_Val   << SDHC_SRR_SWRSTALL_Pos)
#define SDHC_SRR_SWRSTCMD_Pos       1            /**< \brief (SDHC_SRR) Software Reset For CMD Line */
#define SDHC_SRR_SWRSTCMD           (_U_(0x1) << SDHC_SRR_SWRSTCMD_Pos)
#define   SDHC_SRR_SWRSTCMD_WORK_Val      _U_(0x0)   /**< \brief (SDHC_SRR) Work */
#define   SDHC_SRR_SWRSTCMD_RESET_Val     _U_(0x1)   /**< \brief (SDHC_SRR) Reset */
#define SDHC_SRR_SWRSTCMD_WORK      (SDHC_SRR_SWRSTCMD_WORK_Val    << SDHC_SRR_SWRSTCMD_Pos)
#define SDHC_SRR_SWRSTCMD_RESET     (SDHC_SRR_SWRSTCMD_RESET_Val   << SDHC_SRR_SWRSTCMD_Pos)
#define SDHC_SRR_SWRSTDAT_Pos       2            /**< \brief (SDHC_SRR) Software Reset For DAT Line */
#define SDHC_SRR_SWRSTDAT           (_U_(0x1) << SDHC_SRR_SWRSTDAT_Pos)
#define   SDHC_SRR_SWRSTDAT_WORK_Val      _U_(0x0)   /**< \brief (SDHC_SRR) Work */
#define   SDHC_SRR_SWRSTDAT_RESET_Val     _U_(0x1)   /**< \brief (SDHC_SRR) Reset */
#define SDHC_SRR_SWRSTDAT_WORK      (SDHC_SRR_SWRSTDAT_WORK_Val    << SDHC_SRR_SWRSTDAT_Pos)
#define SDHC_SRR_SWRSTDAT_RESET     (SDHC_SRR_SWRSTDAT_RESET_Val   << SDHC_SRR_SWRSTDAT_Pos)
#define SDHC_SRR_MASK               _U_(0x07)    /**< \brief (SDHC_SRR) MASK Register */

/* -------- SDHC_NISTR : (SDHC Offset: 0x030) (R/W 16) Normal Interrupt Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t CMDC:1;           /*!< bit:      0  Command Complete                   */
    uint16_t TRFC:1;           /*!< bit:      1  Transfer Complete                  */
    uint16_t BLKGE:1;          /*!< bit:      2  Block Gap Event                    */
    uint16_t DMAINT:1;         /*!< bit:      3  DMA Interrupt                      */
    uint16_t BWRRDY:1;         /*!< bit:      4  Buffer Write Ready                 */
    uint16_t BRDRDY:1;         /*!< bit:      5  Buffer Read Ready                  */
    uint16_t CINS:1;           /*!< bit:      6  Card Insertion                     */
    uint16_t CREM:1;           /*!< bit:      7  Card Removal                       */
    uint16_t CINT:1;           /*!< bit:      8  Card Interrupt                     */
    uint16_t :6;               /*!< bit:  9..14  Reserved                           */
    uint16_t ERRINT:1;         /*!< bit:     15  Error Interrupt                    */
  } bit;                       /*!< Structure used for bit  access                  */
  struct { // EMMC mode
    uint16_t CMDC:1;           /*!< bit:      0  Command Complete                   */
    uint16_t TRFC:1;           /*!< bit:      1  Transfer Complete                  */
    uint16_t BLKGE:1;          /*!< bit:      2  Block Gap Event                    */
    uint16_t DMAINT:1;         /*!< bit:      3  DMA Interrupt                      */
    uint16_t BWRRDY:1;         /*!< bit:      4  Buffer Write Ready                 */
    uint16_t BRDRDY:1;         /*!< bit:      5  Buffer Read Ready                  */
    uint16_t :8;               /*!< bit:  6..13  Reserved                           */
    uint16_t BOOTAR:1;         /*!< bit:     14  Boot Acknowledge Received          */
    uint16_t ERRINT:1;         /*!< bit:     15  Error Interrupt                    */
  } EMMC;                      /*!< Structure used for EMMC                         */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_NISTR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_NISTR_OFFSET           0x030        /**< \brief (SDHC_NISTR offset) Normal Interrupt Status */
#define SDHC_NISTR_RESETVALUE       _U_(0x0000)  /**< \brief (SDHC_NISTR reset_value) Normal Interrupt Status */

#define SDHC_NISTR_CMDC_Pos         0            /**< \brief (SDHC_NISTR) Command Complete */
#define SDHC_NISTR_CMDC             (_U_(0x1) << SDHC_NISTR_CMDC_Pos)
#define   SDHC_NISTR_CMDC_NO_Val          _U_(0x0)   /**< \brief (SDHC_NISTR) No command complete */
#define   SDHC_NISTR_CMDC_YES_Val         _U_(0x1)   /**< \brief (SDHC_NISTR) Command complete */
#define SDHC_NISTR_CMDC_NO          (SDHC_NISTR_CMDC_NO_Val        << SDHC_NISTR_CMDC_Pos)
#define SDHC_NISTR_CMDC_YES         (SDHC_NISTR_CMDC_YES_Val       << SDHC_NISTR_CMDC_Pos)
#define SDHC_NISTR_TRFC_Pos         1            /**< \brief (SDHC_NISTR) Transfer Complete */
#define SDHC_NISTR_TRFC             (_U_(0x1) << SDHC_NISTR_TRFC_Pos)
#define   SDHC_NISTR_TRFC_NO_Val          _U_(0x0)   /**< \brief (SDHC_NISTR) Not complete */
#define   SDHC_NISTR_TRFC_YES_Val         _U_(0x1)   /**< \brief (SDHC_NISTR) Command execution is completed */
#define SDHC_NISTR_TRFC_NO          (SDHC_NISTR_TRFC_NO_Val        << SDHC_NISTR_TRFC_Pos)
#define SDHC_NISTR_TRFC_YES         (SDHC_NISTR_TRFC_YES_Val       << SDHC_NISTR_TRFC_Pos)
#define SDHC_NISTR_BLKGE_Pos        2            /**< \brief (SDHC_NISTR) Block Gap Event */
#define SDHC_NISTR_BLKGE            (_U_(0x1) << SDHC_NISTR_BLKGE_Pos)
#define   SDHC_NISTR_BLKGE_NO_Val         _U_(0x0)   /**< \brief (SDHC_NISTR) No Block Gap Event */
#define   SDHC_NISTR_BLKGE_STOP_Val       _U_(0x1)   /**< \brief (SDHC_NISTR) Transaction stopped at block gap */
#define SDHC_NISTR_BLKGE_NO         (SDHC_NISTR_BLKGE_NO_Val       << SDHC_NISTR_BLKGE_Pos)
#define SDHC_NISTR_BLKGE_STOP       (SDHC_NISTR_BLKGE_STOP_Val     << SDHC_NISTR_BLKGE_Pos)
#define SDHC_NISTR_DMAINT_Pos       3            /**< \brief (SDHC_NISTR) DMA Interrupt */
#define SDHC_NISTR_DMAINT           (_U_(0x1) << SDHC_NISTR_DMAINT_Pos)
#define   SDHC_NISTR_DMAINT_NO_Val        _U_(0x0)   /**< \brief (SDHC_NISTR) No DMA Interrupt */
#define   SDHC_NISTR_DMAINT_YES_Val       _U_(0x1)   /**< \brief (SDHC_NISTR) DMA Interrupt is generated */
#define SDHC_NISTR_DMAINT_NO        (SDHC_NISTR_DMAINT_NO_Val      << SDHC_NISTR_DMAINT_Pos)
#define SDHC_NISTR_DMAINT_YES       (SDHC_NISTR_DMAINT_YES_Val     << SDHC_NISTR_DMAINT_Pos)
#define SDHC_NISTR_BWRRDY_Pos       4            /**< \brief (SDHC_NISTR) Buffer Write Ready */
#define SDHC_NISTR_BWRRDY           (_U_(0x1) << SDHC_NISTR_BWRRDY_Pos)
#define   SDHC_NISTR_BWRRDY_NO_Val        _U_(0x0)   /**< \brief (SDHC_NISTR) Not ready to write buffer */
#define   SDHC_NISTR_BWRRDY_YES_Val       _U_(0x1)   /**< \brief (SDHC_NISTR) Ready to write buffer */
#define SDHC_NISTR_BWRRDY_NO        (SDHC_NISTR_BWRRDY_NO_Val      << SDHC_NISTR_BWRRDY_Pos)
#define SDHC_NISTR_BWRRDY_YES       (SDHC_NISTR_BWRRDY_YES_Val     << SDHC_NISTR_BWRRDY_Pos)
#define SDHC_NISTR_BRDRDY_Pos       5            /**< \brief (SDHC_NISTR) Buffer Read Ready */
#define SDHC_NISTR_BRDRDY           (_U_(0x1) << SDHC_NISTR_BRDRDY_Pos)
#define   SDHC_NISTR_BRDRDY_NO_Val        _U_(0x0)   /**< \brief (SDHC_NISTR) Not ready to read buffer */
#define   SDHC_NISTR_BRDRDY_YES_Val       _U_(0x1)   /**< \brief (SDHC_NISTR) Ready to read buffer */
#define SDHC_NISTR_BRDRDY_NO        (SDHC_NISTR_BRDRDY_NO_Val      << SDHC_NISTR_BRDRDY_Pos)
#define SDHC_NISTR_BRDRDY_YES       (SDHC_NISTR_BRDRDY_YES_Val     << SDHC_NISTR_BRDRDY_Pos)
#define SDHC_NISTR_CINS_Pos         6            /**< \brief (SDHC_NISTR) Card Insertion */
#define SDHC_NISTR_CINS             (_U_(0x1) << SDHC_NISTR_CINS_Pos)
#define   SDHC_NISTR_CINS_NO_Val          _U_(0x0)   /**< \brief (SDHC_NISTR) Card state stable or Debouncing */
#define   SDHC_NISTR_CINS_YES_Val         _U_(0x1)   /**< \brief (SDHC_NISTR) Card inserted */
#define SDHC_NISTR_CINS_NO          (SDHC_NISTR_CINS_NO_Val        << SDHC_NISTR_CINS_Pos)
#define SDHC_NISTR_CINS_YES         (SDHC_NISTR_CINS_YES_Val       << SDHC_NISTR_CINS_Pos)
#define SDHC_NISTR_CREM_Pos         7            /**< \brief (SDHC_NISTR) Card Removal */
#define SDHC_NISTR_CREM             (_U_(0x1) << SDHC_NISTR_CREM_Pos)
#define   SDHC_NISTR_CREM_NO_Val          _U_(0x0)   /**< \brief (SDHC_NISTR) Card state stable or Debouncing */
#define   SDHC_NISTR_CREM_YES_Val         _U_(0x1)   /**< \brief (SDHC_NISTR) Card Removed */
#define SDHC_NISTR_CREM_NO          (SDHC_NISTR_CREM_NO_Val        << SDHC_NISTR_CREM_Pos)
#define SDHC_NISTR_CREM_YES         (SDHC_NISTR_CREM_YES_Val       << SDHC_NISTR_CREM_Pos)
#define SDHC_NISTR_CINT_Pos         8            /**< \brief (SDHC_NISTR) Card Interrupt */
#define SDHC_NISTR_CINT             (_U_(0x1) << SDHC_NISTR_CINT_Pos)
#define   SDHC_NISTR_CINT_NO_Val          _U_(0x0)   /**< \brief (SDHC_NISTR) No Card Interrupt */
#define   SDHC_NISTR_CINT_YES_Val         _U_(0x1)   /**< \brief (SDHC_NISTR) Generate Card Interrupt */
#define SDHC_NISTR_CINT_NO          (SDHC_NISTR_CINT_NO_Val        << SDHC_NISTR_CINT_Pos)
#define SDHC_NISTR_CINT_YES         (SDHC_NISTR_CINT_YES_Val       << SDHC_NISTR_CINT_Pos)
#define SDHC_NISTR_ERRINT_Pos       15           /**< \brief (SDHC_NISTR) Error Interrupt */
#define SDHC_NISTR_ERRINT           (_U_(0x1) << SDHC_NISTR_ERRINT_Pos)
#define   SDHC_NISTR_ERRINT_NO_Val        _U_(0x0)   /**< \brief (SDHC_NISTR) No Error */
#define   SDHC_NISTR_ERRINT_YES_Val       _U_(0x1)   /**< \brief (SDHC_NISTR) Error */
#define SDHC_NISTR_ERRINT_NO        (SDHC_NISTR_ERRINT_NO_Val      << SDHC_NISTR_ERRINT_Pos)
#define SDHC_NISTR_ERRINT_YES       (SDHC_NISTR_ERRINT_YES_Val     << SDHC_NISTR_ERRINT_Pos)
#define SDHC_NISTR_MASK             _U_(0x81FF)  /**< \brief (SDHC_NISTR) MASK Register */

// EMMC mode
#define SDHC_NISTR_EMMC_CMDC_Pos    0            /**< \brief (SDHC_NISTR_EMMC) Command Complete */
#define SDHC_NISTR_EMMC_CMDC        (_U_(0x1) << SDHC_NISTR_EMMC_CMDC_Pos)
#define   SDHC_NISTR_EMMC_CMDC_NO_Val     _U_(0x0)   /**< \brief (SDHC_NISTR_EMMC) No command complete */
#define   SDHC_NISTR_EMMC_CMDC_YES_Val    _U_(0x1)   /**< \brief (SDHC_NISTR_EMMC) Command complete */
#define SDHC_NISTR_EMMC_CMDC_NO     (SDHC_NISTR_EMMC_CMDC_NO_Val   << SDHC_NISTR_EMMC_CMDC_Pos)
#define SDHC_NISTR_EMMC_CMDC_YES    (SDHC_NISTR_EMMC_CMDC_YES_Val  << SDHC_NISTR_EMMC_CMDC_Pos)
#define SDHC_NISTR_EMMC_TRFC_Pos    1            /**< \brief (SDHC_NISTR_EMMC) Transfer Complete */
#define SDHC_NISTR_EMMC_TRFC        (_U_(0x1) << SDHC_NISTR_EMMC_TRFC_Pos)
#define   SDHC_NISTR_EMMC_TRFC_NO_Val     _U_(0x0)   /**< \brief (SDHC_NISTR_EMMC) Not complete */
#define   SDHC_NISTR_EMMC_TRFC_YES_Val    _U_(0x1)   /**< \brief (SDHC_NISTR_EMMC) Command execution is completed */
#define SDHC_NISTR_EMMC_TRFC_NO     (SDHC_NISTR_EMMC_TRFC_NO_Val   << SDHC_NISTR_EMMC_TRFC_Pos)
#define SDHC_NISTR_EMMC_TRFC_YES    (SDHC_NISTR_EMMC_TRFC_YES_Val  << SDHC_NISTR_EMMC_TRFC_Pos)
#define SDHC_NISTR_EMMC_BLKGE_Pos   2            /**< \brief (SDHC_NISTR_EMMC) Block Gap Event */
#define SDHC_NISTR_EMMC_BLKGE       (_U_(0x1) << SDHC_NISTR_EMMC_BLKGE_Pos)
#define   SDHC_NISTR_EMMC_BLKGE_NO_Val    _U_(0x0)   /**< \brief (SDHC_NISTR_EMMC) No Block Gap Event */
#define   SDHC_NISTR_EMMC_BLKGE_STOP_Val  _U_(0x1)   /**< \brief (SDHC_NISTR_EMMC) Transaction stopped at block gap */
#define SDHC_NISTR_EMMC_BLKGE_NO    (SDHC_NISTR_EMMC_BLKGE_NO_Val  << SDHC_NISTR_EMMC_BLKGE_Pos)
#define SDHC_NISTR_EMMC_BLKGE_STOP  (SDHC_NISTR_EMMC_BLKGE_STOP_Val << SDHC_NISTR_EMMC_BLKGE_Pos)
#define SDHC_NISTR_EMMC_DMAINT_Pos  3            /**< \brief (SDHC_NISTR_EMMC) DMA Interrupt */
#define SDHC_NISTR_EMMC_DMAINT      (_U_(0x1) << SDHC_NISTR_EMMC_DMAINT_Pos)
#define   SDHC_NISTR_EMMC_DMAINT_NO_Val   _U_(0x0)   /**< \brief (SDHC_NISTR_EMMC) No DMA Interrupt */
#define   SDHC_NISTR_EMMC_DMAINT_YES_Val  _U_(0x1)   /**< \brief (SDHC_NISTR_EMMC) DMA Interrupt is generated */
#define SDHC_NISTR_EMMC_DMAINT_NO   (SDHC_NISTR_EMMC_DMAINT_NO_Val << SDHC_NISTR_EMMC_DMAINT_Pos)
#define SDHC_NISTR_EMMC_DMAINT_YES  (SDHC_NISTR_EMMC_DMAINT_YES_Val << SDHC_NISTR_EMMC_DMAINT_Pos)
#define SDHC_NISTR_EMMC_BWRRDY_Pos  4            /**< \brief (SDHC_NISTR_EMMC) Buffer Write Ready */
#define SDHC_NISTR_EMMC_BWRRDY      (_U_(0x1) << SDHC_NISTR_EMMC_BWRRDY_Pos)
#define   SDHC_NISTR_EMMC_BWRRDY_NO_Val   _U_(0x0)   /**< \brief (SDHC_NISTR_EMMC) Not ready to write buffer */
#define   SDHC_NISTR_EMMC_BWRRDY_YES_Val  _U_(0x1)   /**< \brief (SDHC_NISTR_EMMC) Ready to write buffer */
#define SDHC_NISTR_EMMC_BWRRDY_NO   (SDHC_NISTR_EMMC_BWRRDY_NO_Val << SDHC_NISTR_EMMC_BWRRDY_Pos)
#define SDHC_NISTR_EMMC_BWRRDY_YES  (SDHC_NISTR_EMMC_BWRRDY_YES_Val << SDHC_NISTR_EMMC_BWRRDY_Pos)
#define SDHC_NISTR_EMMC_BRDRDY_Pos  5            /**< \brief (SDHC_NISTR_EMMC) Buffer Read Ready */
#define SDHC_NISTR_EMMC_BRDRDY      (_U_(0x1) << SDHC_NISTR_EMMC_BRDRDY_Pos)
#define   SDHC_NISTR_EMMC_BRDRDY_NO_Val   _U_(0x0)   /**< \brief (SDHC_NISTR_EMMC) Not ready to read buffer */
#define   SDHC_NISTR_EMMC_BRDRDY_YES_Val  _U_(0x1)   /**< \brief (SDHC_NISTR_EMMC) Ready to read buffer */
#define SDHC_NISTR_EMMC_BRDRDY_NO   (SDHC_NISTR_EMMC_BRDRDY_NO_Val << SDHC_NISTR_EMMC_BRDRDY_Pos)
#define SDHC_NISTR_EMMC_BRDRDY_YES  (SDHC_NISTR_EMMC_BRDRDY_YES_Val << SDHC_NISTR_EMMC_BRDRDY_Pos)
#define SDHC_NISTR_EMMC_BOOTAR_Pos  14           /**< \brief (SDHC_NISTR_EMMC) Boot Acknowledge Received */
#define SDHC_NISTR_EMMC_BOOTAR      (_U_(0x1) << SDHC_NISTR_EMMC_BOOTAR_Pos)
#define SDHC_NISTR_EMMC_ERRINT_Pos  15           /**< \brief (SDHC_NISTR_EMMC) Error Interrupt */
#define SDHC_NISTR_EMMC_ERRINT      (_U_(0x1) << SDHC_NISTR_EMMC_ERRINT_Pos)
#define   SDHC_NISTR_EMMC_ERRINT_NO_Val   _U_(0x0)   /**< \brief (SDHC_NISTR_EMMC) No Error */
#define   SDHC_NISTR_EMMC_ERRINT_YES_Val  _U_(0x1)   /**< \brief (SDHC_NISTR_EMMC) Error */
#define SDHC_NISTR_EMMC_ERRINT_NO   (SDHC_NISTR_EMMC_ERRINT_NO_Val << SDHC_NISTR_EMMC_ERRINT_Pos)
#define SDHC_NISTR_EMMC_ERRINT_YES  (SDHC_NISTR_EMMC_ERRINT_YES_Val << SDHC_NISTR_EMMC_ERRINT_Pos)
#define SDHC_NISTR_EMMC_MASK        _U_(0xC03F)  /**< \brief (SDHC_NISTR_EMMC) MASK Register */

/* -------- SDHC_EISTR : (SDHC Offset: 0x032) (R/W 16) Error Interrupt Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t CMDTEO:1;         /*!< bit:      0  Command Timeout Error              */
    uint16_t CMDCRC:1;         /*!< bit:      1  Command CRC Error                  */
    uint16_t CMDEND:1;         /*!< bit:      2  Command End Bit Error              */
    uint16_t CMDIDX:1;         /*!< bit:      3  Command Index Error                */
    uint16_t DATTEO:1;         /*!< bit:      4  Data Timeout Error                 */
    uint16_t DATCRC:1;         /*!< bit:      5  Data CRC Error                     */
    uint16_t DATEND:1;         /*!< bit:      6  Data End Bit Error                 */
    uint16_t CURLIM:1;         /*!< bit:      7  Current Limit Error                */
    uint16_t ACMD:1;           /*!< bit:      8  Auto CMD Error                     */
    uint16_t ADMA:1;           /*!< bit:      9  ADMA Error                         */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct { // EMMC mode
    uint16_t CMDTEO:1;         /*!< bit:      0  Command Timeout Error              */
    uint16_t CMDCRC:1;         /*!< bit:      1  Command CRC Error                  */
    uint16_t CMDEND:1;         /*!< bit:      2  Command End Bit Error              */
    uint16_t CMDIDX:1;         /*!< bit:      3  Command Index Error                */
    uint16_t DATTEO:1;         /*!< bit:      4  Data Timeout Error                 */
    uint16_t DATCRC:1;         /*!< bit:      5  Data CRC Error                     */
    uint16_t DATEND:1;         /*!< bit:      6  Data End Bit Error                 */
    uint16_t CURLIM:1;         /*!< bit:      7  Current Limit Error                */
    uint16_t ACMD:1;           /*!< bit:      8  Auto CMD Error                     */
    uint16_t ADMA:1;           /*!< bit:      9  ADMA Error                         */
    uint16_t :2;               /*!< bit: 10..11  Reserved                           */
    uint16_t BOOTAE:1;         /*!< bit:     12  Boot Acknowledge Error             */
    uint16_t :3;               /*!< bit: 13..15  Reserved                           */
  } EMMC;                      /*!< Structure used for EMMC                         */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_EISTR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_EISTR_OFFSET           0x032        /**< \brief (SDHC_EISTR offset) Error Interrupt Status */
#define SDHC_EISTR_RESETVALUE       _U_(0x0000)  /**< \brief (SDHC_EISTR reset_value) Error Interrupt Status */

#define SDHC_EISTR_CMDTEO_Pos       0            /**< \brief (SDHC_EISTR) Command Timeout Error */
#define SDHC_EISTR_CMDTEO           (_U_(0x1) << SDHC_EISTR_CMDTEO_Pos)
#define   SDHC_EISTR_CMDTEO_NO_Val        _U_(0x0)   /**< \brief (SDHC_EISTR) No Error */
#define   SDHC_EISTR_CMDTEO_YES_Val       _U_(0x1)   /**< \brief (SDHC_EISTR) Timeout */
#define SDHC_EISTR_CMDTEO_NO        (SDHC_EISTR_CMDTEO_NO_Val      << SDHC_EISTR_CMDTEO_Pos)
#define SDHC_EISTR_CMDTEO_YES       (SDHC_EISTR_CMDTEO_YES_Val     << SDHC_EISTR_CMDTEO_Pos)
#define SDHC_EISTR_CMDCRC_Pos       1            /**< \brief (SDHC_EISTR) Command CRC Error */
#define SDHC_EISTR_CMDCRC           (_U_(0x1) << SDHC_EISTR_CMDCRC_Pos)
#define   SDHC_EISTR_CMDCRC_NO_Val        _U_(0x0)   /**< \brief (SDHC_EISTR) No Error */
#define   SDHC_EISTR_CMDCRC_YES_Val       _U_(0x1)   /**< \brief (SDHC_EISTR) CRC Error Generated */
#define SDHC_EISTR_CMDCRC_NO        (SDHC_EISTR_CMDCRC_NO_Val      << SDHC_EISTR_CMDCRC_Pos)
#define SDHC_EISTR_CMDCRC_YES       (SDHC_EISTR_CMDCRC_YES_Val     << SDHC_EISTR_CMDCRC_Pos)
#define SDHC_EISTR_CMDEND_Pos       2            /**< \brief (SDHC_EISTR) Command End Bit Error */
#define SDHC_EISTR_CMDEND           (_U_(0x1) << SDHC_EISTR_CMDEND_Pos)
#define   SDHC_EISTR_CMDEND_NO_Val        _U_(0x0)   /**< \brief (SDHC_EISTR) No error */
#define   SDHC_EISTR_CMDEND_YES_Val       _U_(0x1)   /**< \brief (SDHC_EISTR) End Bit Error Generated */
#define SDHC_EISTR_CMDEND_NO        (SDHC_EISTR_CMDEND_NO_Val      << SDHC_EISTR_CMDEND_Pos)
#define SDHC_EISTR_CMDEND_YES       (SDHC_EISTR_CMDEND_YES_Val     << SDHC_EISTR_CMDEND_Pos)
#define SDHC_EISTR_CMDIDX_Pos       3            /**< \brief (SDHC_EISTR) Command Index Error */
#define SDHC_EISTR_CMDIDX           (_U_(0x1) << SDHC_EISTR_CMDIDX_Pos)
#define   SDHC_EISTR_CMDIDX_NO_Val        _U_(0x0)   /**< \brief (SDHC_EISTR) No Error */
#define   SDHC_EISTR_CMDIDX_YES_Val       _U_(0x1)   /**< \brief (SDHC_EISTR) Error */
#define SDHC_EISTR_CMDIDX_NO        (SDHC_EISTR_CMDIDX_NO_Val      << SDHC_EISTR_CMDIDX_Pos)
#define SDHC_EISTR_CMDIDX_YES       (SDHC_EISTR_CMDIDX_YES_Val     << SDHC_EISTR_CMDIDX_Pos)
#define SDHC_EISTR_DATTEO_Pos       4            /**< \brief (SDHC_EISTR) Data Timeout Error */
#define SDHC_EISTR_DATTEO           (_U_(0x1) << SDHC_EISTR_DATTEO_Pos)
#define   SDHC_EISTR_DATTEO_NO_Val        _U_(0x0)   /**< \brief (SDHC_EISTR) No Error */
#define   SDHC_EISTR_DATTEO_YES_Val       _U_(0x1)   /**< \brief (SDHC_EISTR) Timeout */
#define SDHC_EISTR_DATTEO_NO        (SDHC_EISTR_DATTEO_NO_Val      << SDHC_EISTR_DATTEO_Pos)
#define SDHC_EISTR_DATTEO_YES       (SDHC_EISTR_DATTEO_YES_Val     << SDHC_EISTR_DATTEO_Pos)
#define SDHC_EISTR_DATCRC_Pos       5            /**< \brief (SDHC_EISTR) Data CRC Error */
#define SDHC_EISTR_DATCRC           (_U_(0x1) << SDHC_EISTR_DATCRC_Pos)
#define   SDHC_EISTR_DATCRC_NO_Val        _U_(0x0)   /**< \brief (SDHC_EISTR) No Error */
#define   SDHC_EISTR_DATCRC_YES_Val       _U_(0x1)   /**< \brief (SDHC_EISTR) Error */
#define SDHC_EISTR_DATCRC_NO        (SDHC_EISTR_DATCRC_NO_Val      << SDHC_EISTR_DATCRC_Pos)
#define SDHC_EISTR_DATCRC_YES       (SDHC_EISTR_DATCRC_YES_Val     << SDHC_EISTR_DATCRC_Pos)
#define SDHC_EISTR_DATEND_Pos       6            /**< \brief (SDHC_EISTR) Data End Bit Error */
#define SDHC_EISTR_DATEND           (_U_(0x1) << SDHC_EISTR_DATEND_Pos)
#define   SDHC_EISTR_DATEND_NO_Val        _U_(0x0)   /**< \brief (SDHC_EISTR) No Error */
#define   SDHC_EISTR_DATEND_YES_Val       _U_(0x1)   /**< \brief (SDHC_EISTR) Error */
#define SDHC_EISTR_DATEND_NO        (SDHC_EISTR_DATEND_NO_Val      << SDHC_EISTR_DATEND_Pos)
#define SDHC_EISTR_DATEND_YES       (SDHC_EISTR_DATEND_YES_Val     << SDHC_EISTR_DATEND_Pos)
#define SDHC_EISTR_CURLIM_Pos       7            /**< \brief (SDHC_EISTR) Current Limit Error */
#define SDHC_EISTR_CURLIM           (_U_(0x1) << SDHC_EISTR_CURLIM_Pos)
#define   SDHC_EISTR_CURLIM_NO_Val        _U_(0x0)   /**< \brief (SDHC_EISTR) No Error */
#define   SDHC_EISTR_CURLIM_YES_Val       _U_(0x1)   /**< \brief (SDHC_EISTR) Power Fail */
#define SDHC_EISTR_CURLIM_NO        (SDHC_EISTR_CURLIM_NO_Val      << SDHC_EISTR_CURLIM_Pos)
#define SDHC_EISTR_CURLIM_YES       (SDHC_EISTR_CURLIM_YES_Val     << SDHC_EISTR_CURLIM_Pos)
#define SDHC_EISTR_ACMD_Pos         8            /**< \brief (SDHC_EISTR) Auto CMD Error */
#define SDHC_EISTR_ACMD             (_U_(0x1) << SDHC_EISTR_ACMD_Pos)
#define   SDHC_EISTR_ACMD_NO_Val          _U_(0x0)   /**< \brief (SDHC_EISTR) No Error */
#define   SDHC_EISTR_ACMD_YES_Val         _U_(0x1)   /**< \brief (SDHC_EISTR) Error */
#define SDHC_EISTR_ACMD_NO          (SDHC_EISTR_ACMD_NO_Val        << SDHC_EISTR_ACMD_Pos)
#define SDHC_EISTR_ACMD_YES         (SDHC_EISTR_ACMD_YES_Val       << SDHC_EISTR_ACMD_Pos)
#define SDHC_EISTR_ADMA_Pos         9            /**< \brief (SDHC_EISTR) ADMA Error */
#define SDHC_EISTR_ADMA             (_U_(0x1) << SDHC_EISTR_ADMA_Pos)
#define   SDHC_EISTR_ADMA_NO_Val          _U_(0x0)   /**< \brief (SDHC_EISTR) No Error */
#define   SDHC_EISTR_ADMA_YES_Val         _U_(0x1)   /**< \brief (SDHC_EISTR) Error */
#define SDHC_EISTR_ADMA_NO          (SDHC_EISTR_ADMA_NO_Val        << SDHC_EISTR_ADMA_Pos)
#define SDHC_EISTR_ADMA_YES         (SDHC_EISTR_ADMA_YES_Val       << SDHC_EISTR_ADMA_Pos)
#define SDHC_EISTR_MASK             _U_(0x03FF)  /**< \brief (SDHC_EISTR) MASK Register */

// EMMC mode
#define SDHC_EISTR_EMMC_CMDTEO_Pos  0            /**< \brief (SDHC_EISTR_EMMC) Command Timeout Error */
#define SDHC_EISTR_EMMC_CMDTEO      (_U_(0x1) << SDHC_EISTR_EMMC_CMDTEO_Pos)
#define   SDHC_EISTR_EMMC_CMDTEO_NO_Val   _U_(0x0)   /**< \brief (SDHC_EISTR_EMMC) No Error */
#define   SDHC_EISTR_EMMC_CMDTEO_YES_Val  _U_(0x1)   /**< \brief (SDHC_EISTR_EMMC) Timeout */
#define SDHC_EISTR_EMMC_CMDTEO_NO   (SDHC_EISTR_EMMC_CMDTEO_NO_Val << SDHC_EISTR_EMMC_CMDTEO_Pos)
#define SDHC_EISTR_EMMC_CMDTEO_YES  (SDHC_EISTR_EMMC_CMDTEO_YES_Val << SDHC_EISTR_EMMC_CMDTEO_Pos)
#define SDHC_EISTR_EMMC_CMDCRC_Pos  1            /**< \brief (SDHC_EISTR_EMMC) Command CRC Error */
#define SDHC_EISTR_EMMC_CMDCRC      (_U_(0x1) << SDHC_EISTR_EMMC_CMDCRC_Pos)
#define   SDHC_EISTR_EMMC_CMDCRC_NO_Val   _U_(0x0)   /**< \brief (SDHC_EISTR_EMMC) No Error */
#define   SDHC_EISTR_EMMC_CMDCRC_YES_Val  _U_(0x1)   /**< \brief (SDHC_EISTR_EMMC) CRC Error Generated */
#define SDHC_EISTR_EMMC_CMDCRC_NO   (SDHC_EISTR_EMMC_CMDCRC_NO_Val << SDHC_EISTR_EMMC_CMDCRC_Pos)
#define SDHC_EISTR_EMMC_CMDCRC_YES  (SDHC_EISTR_EMMC_CMDCRC_YES_Val << SDHC_EISTR_EMMC_CMDCRC_Pos)
#define SDHC_EISTR_EMMC_CMDEND_Pos  2            /**< \brief (SDHC_EISTR_EMMC) Command End Bit Error */
#define SDHC_EISTR_EMMC_CMDEND      (_U_(0x1) << SDHC_EISTR_EMMC_CMDEND_Pos)
#define   SDHC_EISTR_EMMC_CMDEND_NO_Val   _U_(0x0)   /**< \brief (SDHC_EISTR_EMMC) No error */
#define   SDHC_EISTR_EMMC_CMDEND_YES_Val  _U_(0x1)   /**< \brief (SDHC_EISTR_EMMC) End Bit Error Generated */
#define SDHC_EISTR_EMMC_CMDEND_NO   (SDHC_EISTR_EMMC_CMDEND_NO_Val << SDHC_EISTR_EMMC_CMDEND_Pos)
#define SDHC_EISTR_EMMC_CMDEND_YES  (SDHC_EISTR_EMMC_CMDEND_YES_Val << SDHC_EISTR_EMMC_CMDEND_Pos)
#define SDHC_EISTR_EMMC_CMDIDX_Pos  3            /**< \brief (SDHC_EISTR_EMMC) Command Index Error */
#define SDHC_EISTR_EMMC_CMDIDX      (_U_(0x1) << SDHC_EISTR_EMMC_CMDIDX_Pos)
#define   SDHC_EISTR_EMMC_CMDIDX_NO_Val   _U_(0x0)   /**< \brief (SDHC_EISTR_EMMC) No Error */
#define   SDHC_EISTR_EMMC_CMDIDX_YES_Val  _U_(0x1)   /**< \brief (SDHC_EISTR_EMMC) Error */
#define SDHC_EISTR_EMMC_CMDIDX_NO   (SDHC_EISTR_EMMC_CMDIDX_NO_Val << SDHC_EISTR_EMMC_CMDIDX_Pos)
#define SDHC_EISTR_EMMC_CMDIDX_YES  (SDHC_EISTR_EMMC_CMDIDX_YES_Val << SDHC_EISTR_EMMC_CMDIDX_Pos)
#define SDHC_EISTR_EMMC_DATTEO_Pos  4            /**< \brief (SDHC_EISTR_EMMC) Data Timeout Error */
#define SDHC_EISTR_EMMC_DATTEO      (_U_(0x1) << SDHC_EISTR_EMMC_DATTEO_Pos)
#define   SDHC_EISTR_EMMC_DATTEO_NO_Val   _U_(0x0)   /**< \brief (SDHC_EISTR_EMMC) No Error */
#define   SDHC_EISTR_EMMC_DATTEO_YES_Val  _U_(0x1)   /**< \brief (SDHC_EISTR_EMMC) Timeout */
#define SDHC_EISTR_EMMC_DATTEO_NO   (SDHC_EISTR_EMMC_DATTEO_NO_Val << SDHC_EISTR_EMMC_DATTEO_Pos)
#define SDHC_EISTR_EMMC_DATTEO_YES  (SDHC_EISTR_EMMC_DATTEO_YES_Val << SDHC_EISTR_EMMC_DATTEO_Pos)
#define SDHC_EISTR_EMMC_DATCRC_Pos  5            /**< \brief (SDHC_EISTR_EMMC) Data CRC Error */
#define SDHC_EISTR_EMMC_DATCRC      (_U_(0x1) << SDHC_EISTR_EMMC_DATCRC_Pos)
#define   SDHC_EISTR_EMMC_DATCRC_NO_Val   _U_(0x0)   /**< \brief (SDHC_EISTR_EMMC) No Error */
#define   SDHC_EISTR_EMMC_DATCRC_YES_Val  _U_(0x1)   /**< \brief (SDHC_EISTR_EMMC) Error */
#define SDHC_EISTR_EMMC_DATCRC_NO   (SDHC_EISTR_EMMC_DATCRC_NO_Val << SDHC_EISTR_EMMC_DATCRC_Pos)
#define SDHC_EISTR_EMMC_DATCRC_YES  (SDHC_EISTR_EMMC_DATCRC_YES_Val << SDHC_EISTR_EMMC_DATCRC_Pos)
#define SDHC_EISTR_EMMC_DATEND_Pos  6            /**< \brief (SDHC_EISTR_EMMC) Data End Bit Error */
#define SDHC_EISTR_EMMC_DATEND      (_U_(0x1) << SDHC_EISTR_EMMC_DATEND_Pos)
#define   SDHC_EISTR_EMMC_DATEND_NO_Val   _U_(0x0)   /**< \brief (SDHC_EISTR_EMMC) No Error */
#define   SDHC_EISTR_EMMC_DATEND_YES_Val  _U_(0x1)   /**< \brief (SDHC_EISTR_EMMC) Error */
#define SDHC_EISTR_EMMC_DATEND_NO   (SDHC_EISTR_EMMC_DATEND_NO_Val << SDHC_EISTR_EMMC_DATEND_Pos)
#define SDHC_EISTR_EMMC_DATEND_YES  (SDHC_EISTR_EMMC_DATEND_YES_Val << SDHC_EISTR_EMMC_DATEND_Pos)
#define SDHC_EISTR_EMMC_CURLIM_Pos  7            /**< \brief (SDHC_EISTR_EMMC) Current Limit Error */
#define SDHC_EISTR_EMMC_CURLIM      (_U_(0x1) << SDHC_EISTR_EMMC_CURLIM_Pos)
#define   SDHC_EISTR_EMMC_CURLIM_NO_Val   _U_(0x0)   /**< \brief (SDHC_EISTR_EMMC) No Error */
#define   SDHC_EISTR_EMMC_CURLIM_YES_Val  _U_(0x1)   /**< \brief (SDHC_EISTR_EMMC) Power Fail */
#define SDHC_EISTR_EMMC_CURLIM_NO   (SDHC_EISTR_EMMC_CURLIM_NO_Val << SDHC_EISTR_EMMC_CURLIM_Pos)
#define SDHC_EISTR_EMMC_CURLIM_YES  (SDHC_EISTR_EMMC_CURLIM_YES_Val << SDHC_EISTR_EMMC_CURLIM_Pos)
#define SDHC_EISTR_EMMC_ACMD_Pos    8            /**< \brief (SDHC_EISTR_EMMC) Auto CMD Error */
#define SDHC_EISTR_EMMC_ACMD        (_U_(0x1) << SDHC_EISTR_EMMC_ACMD_Pos)
#define   SDHC_EISTR_EMMC_ACMD_NO_Val     _U_(0x0)   /**< \brief (SDHC_EISTR_EMMC) No Error */
#define   SDHC_EISTR_EMMC_ACMD_YES_Val    _U_(0x1)   /**< \brief (SDHC_EISTR_EMMC) Error */
#define SDHC_EISTR_EMMC_ACMD_NO     (SDHC_EISTR_EMMC_ACMD_NO_Val   << SDHC_EISTR_EMMC_ACMD_Pos)
#define SDHC_EISTR_EMMC_ACMD_YES    (SDHC_EISTR_EMMC_ACMD_YES_Val  << SDHC_EISTR_EMMC_ACMD_Pos)
#define SDHC_EISTR_EMMC_ADMA_Pos    9            /**< \brief (SDHC_EISTR_EMMC) ADMA Error */
#define SDHC_EISTR_EMMC_ADMA        (_U_(0x1) << SDHC_EISTR_EMMC_ADMA_Pos)
#define   SDHC_EISTR_EMMC_ADMA_NO_Val     _U_(0x0)   /**< \brief (SDHC_EISTR_EMMC) No Error */
#define   SDHC_EISTR_EMMC_ADMA_YES_Val    _U_(0x1)   /**< \brief (SDHC_EISTR_EMMC) Error */
#define SDHC_EISTR_EMMC_ADMA_NO     (SDHC_EISTR_EMMC_ADMA_NO_Val   << SDHC_EISTR_EMMC_ADMA_Pos)
#define SDHC_EISTR_EMMC_ADMA_YES    (SDHC_EISTR_EMMC_ADMA_YES_Val  << SDHC_EISTR_EMMC_ADMA_Pos)
#define SDHC_EISTR_EMMC_BOOTAE_Pos  12           /**< \brief (SDHC_EISTR_EMMC) Boot Acknowledge Error */
#define SDHC_EISTR_EMMC_BOOTAE      (_U_(0x1) << SDHC_EISTR_EMMC_BOOTAE_Pos)
#define   SDHC_EISTR_EMMC_BOOTAE_0_Val    _U_(0x0)   /**< \brief (SDHC_EISTR_EMMC) FIFO contains at least one byte */
#define   SDHC_EISTR_EMMC_BOOTAE_1_Val    _U_(0x1)   /**< \brief (SDHC_EISTR_EMMC) FIFO is empty */
#define SDHC_EISTR_EMMC_BOOTAE_0    (SDHC_EISTR_EMMC_BOOTAE_0_Val  << SDHC_EISTR_EMMC_BOOTAE_Pos)
#define SDHC_EISTR_EMMC_BOOTAE_1    (SDHC_EISTR_EMMC_BOOTAE_1_Val  << SDHC_EISTR_EMMC_BOOTAE_Pos)
#define SDHC_EISTR_EMMC_MASK        _U_(0x13FF)  /**< \brief (SDHC_EISTR_EMMC) MASK Register */

/* -------- SDHC_NISTER : (SDHC Offset: 0x034) (R/W 16) Normal Interrupt Status Enable -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t CMDC:1;           /*!< bit:      0  Command Complete Status Enable     */
    uint16_t TRFC:1;           /*!< bit:      1  Transfer Complete Status Enable    */
    uint16_t BLKGE:1;          /*!< bit:      2  Block Gap Event Status Enable      */
    uint16_t DMAINT:1;         /*!< bit:      3  DMA Interrupt Status Enable        */
    uint16_t BWRRDY:1;         /*!< bit:      4  Buffer Write Ready Status Enable   */
    uint16_t BRDRDY:1;         /*!< bit:      5  Buffer Read Ready Status Enable    */
    uint16_t CINS:1;           /*!< bit:      6  Card Insertion Status Enable       */
    uint16_t CREM:1;           /*!< bit:      7  Card Removal Status Enable         */
    uint16_t CINT:1;           /*!< bit:      8  Card Interrupt Status Enable       */
    uint16_t :7;               /*!< bit:  9..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct { // EMMC mode
    uint16_t CMDC:1;           /*!< bit:      0  Command Complete Status Enable     */
    uint16_t TRFC:1;           /*!< bit:      1  Transfer Complete Status Enable    */
    uint16_t BLKGE:1;          /*!< bit:      2  Block Gap Event Status Enable      */
    uint16_t DMAINT:1;         /*!< bit:      3  DMA Interrupt Status Enable        */
    uint16_t BWRRDY:1;         /*!< bit:      4  Buffer Write Ready Status Enable   */
    uint16_t BRDRDY:1;         /*!< bit:      5  Buffer Read Ready Status Enable    */
    uint16_t :8;               /*!< bit:  6..13  Reserved                           */
    uint16_t BOOTAR:1;         /*!< bit:     14  Boot Acknowledge Received Status Enable */
    uint16_t :1;               /*!< bit:     15  Reserved                           */
  } EMMC;                      /*!< Structure used for EMMC                         */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_NISTER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_NISTER_OFFSET          0x034        /**< \brief (SDHC_NISTER offset) Normal Interrupt Status Enable */
#define SDHC_NISTER_RESETVALUE      _U_(0x0000)  /**< \brief (SDHC_NISTER reset_value) Normal Interrupt Status Enable */

#define SDHC_NISTER_CMDC_Pos        0            /**< \brief (SDHC_NISTER) Command Complete Status Enable */
#define SDHC_NISTER_CMDC            (_U_(0x1) << SDHC_NISTER_CMDC_Pos)
#define   SDHC_NISTER_CMDC_MASKED_Val     _U_(0x0)   /**< \brief (SDHC_NISTER) Masked */
#define   SDHC_NISTER_CMDC_ENABLED_Val    _U_(0x1)   /**< \brief (SDHC_NISTER) Enabled */
#define SDHC_NISTER_CMDC_MASKED     (SDHC_NISTER_CMDC_MASKED_Val   << SDHC_NISTER_CMDC_Pos)
#define SDHC_NISTER_CMDC_ENABLED    (SDHC_NISTER_CMDC_ENABLED_Val  << SDHC_NISTER_CMDC_Pos)
#define SDHC_NISTER_TRFC_Pos        1            /**< \brief (SDHC_NISTER) Transfer Complete Status Enable */
#define SDHC_NISTER_TRFC            (_U_(0x1) << SDHC_NISTER_TRFC_Pos)
#define   SDHC_NISTER_TRFC_MASKED_Val     _U_(0x0)   /**< \brief (SDHC_NISTER) Masked */
#define   SDHC_NISTER_TRFC_ENABLED_Val    _U_(0x1)   /**< \brief (SDHC_NISTER) Enabled */
#define SDHC_NISTER_TRFC_MASKED     (SDHC_NISTER_TRFC_MASKED_Val   << SDHC_NISTER_TRFC_Pos)
#define SDHC_NISTER_TRFC_ENABLED    (SDHC_NISTER_TRFC_ENABLED_Val  << SDHC_NISTER_TRFC_Pos)
#define SDHC_NISTER_BLKGE_Pos       2            /**< \brief (SDHC_NISTER) Block Gap Event Status Enable */
#define SDHC_NISTER_BLKGE           (_U_(0x1) << SDHC_NISTER_BLKGE_Pos)
#define   SDHC_NISTER_BLKGE_MASKED_Val    _U_(0x0)   /**< \brief (SDHC_NISTER) Masked */
#define   SDHC_NISTER_BLKGE_ENABLED_Val   _U_(0x1)   /**< \brief (SDHC_NISTER) Enabled */
#define SDHC_NISTER_BLKGE_MASKED    (SDHC_NISTER_BLKGE_MASKED_Val  << SDHC_NISTER_BLKGE_Pos)
#define SDHC_NISTER_BLKGE_ENABLED   (SDHC_NISTER_BLKGE_ENABLED_Val << SDHC_NISTER_BLKGE_Pos)
#define SDHC_NISTER_DMAINT_Pos      3            /**< \brief (SDHC_NISTER) DMA Interrupt Status Enable */
#define SDHC_NISTER_DMAINT          (_U_(0x1) << SDHC_NISTER_DMAINT_Pos)
#define   SDHC_NISTER_DMAINT_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_NISTER) Masked */
#define   SDHC_NISTER_DMAINT_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_NISTER) Enabled */
#define SDHC_NISTER_DMAINT_MASKED   (SDHC_NISTER_DMAINT_MASKED_Val << SDHC_NISTER_DMAINT_Pos)
#define SDHC_NISTER_DMAINT_ENABLED  (SDHC_NISTER_DMAINT_ENABLED_Val << SDHC_NISTER_DMAINT_Pos)
#define SDHC_NISTER_BWRRDY_Pos      4            /**< \brief (SDHC_NISTER) Buffer Write Ready Status Enable */
#define SDHC_NISTER_BWRRDY          (_U_(0x1) << SDHC_NISTER_BWRRDY_Pos)
#define   SDHC_NISTER_BWRRDY_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_NISTER) Masked */
#define   SDHC_NISTER_BWRRDY_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_NISTER) Enabled */
#define SDHC_NISTER_BWRRDY_MASKED   (SDHC_NISTER_BWRRDY_MASKED_Val << SDHC_NISTER_BWRRDY_Pos)
#define SDHC_NISTER_BWRRDY_ENABLED  (SDHC_NISTER_BWRRDY_ENABLED_Val << SDHC_NISTER_BWRRDY_Pos)
#define SDHC_NISTER_BRDRDY_Pos      5            /**< \brief (SDHC_NISTER) Buffer Read Ready Status Enable */
#define SDHC_NISTER_BRDRDY          (_U_(0x1) << SDHC_NISTER_BRDRDY_Pos)
#define   SDHC_NISTER_BRDRDY_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_NISTER) Masked */
#define   SDHC_NISTER_BRDRDY_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_NISTER) Enabled */
#define SDHC_NISTER_BRDRDY_MASKED   (SDHC_NISTER_BRDRDY_MASKED_Val << SDHC_NISTER_BRDRDY_Pos)
#define SDHC_NISTER_BRDRDY_ENABLED  (SDHC_NISTER_BRDRDY_ENABLED_Val << SDHC_NISTER_BRDRDY_Pos)
#define SDHC_NISTER_CINS_Pos        6            /**< \brief (SDHC_NISTER) Card Insertion Status Enable */
#define SDHC_NISTER_CINS            (_U_(0x1) << SDHC_NISTER_CINS_Pos)
#define   SDHC_NISTER_CINS_MASKED_Val     _U_(0x0)   /**< \brief (SDHC_NISTER) Masked */
#define   SDHC_NISTER_CINS_ENABLED_Val    _U_(0x1)   /**< \brief (SDHC_NISTER) Enabled */
#define SDHC_NISTER_CINS_MASKED     (SDHC_NISTER_CINS_MASKED_Val   << SDHC_NISTER_CINS_Pos)
#define SDHC_NISTER_CINS_ENABLED    (SDHC_NISTER_CINS_ENABLED_Val  << SDHC_NISTER_CINS_Pos)
#define SDHC_NISTER_CREM_Pos        7            /**< \brief (SDHC_NISTER) Card Removal Status Enable */
#define SDHC_NISTER_CREM            (_U_(0x1) << SDHC_NISTER_CREM_Pos)
#define   SDHC_NISTER_CREM_MASKED_Val     _U_(0x0)   /**< \brief (SDHC_NISTER) Masked */
#define   SDHC_NISTER_CREM_ENABLED_Val    _U_(0x1)   /**< \brief (SDHC_NISTER) Enabled */
#define SDHC_NISTER_CREM_MASKED     (SDHC_NISTER_CREM_MASKED_Val   << SDHC_NISTER_CREM_Pos)
#define SDHC_NISTER_CREM_ENABLED    (SDHC_NISTER_CREM_ENABLED_Val  << SDHC_NISTER_CREM_Pos)
#define SDHC_NISTER_CINT_Pos        8            /**< \brief (SDHC_NISTER) Card Interrupt Status Enable */
#define SDHC_NISTER_CINT            (_U_(0x1) << SDHC_NISTER_CINT_Pos)
#define   SDHC_NISTER_CINT_MASKED_Val     _U_(0x0)   /**< \brief (SDHC_NISTER) Masked */
#define   SDHC_NISTER_CINT_ENABLED_Val    _U_(0x1)   /**< \brief (SDHC_NISTER) Enabled */
#define SDHC_NISTER_CINT_MASKED     (SDHC_NISTER_CINT_MASKED_Val   << SDHC_NISTER_CINT_Pos)
#define SDHC_NISTER_CINT_ENABLED    (SDHC_NISTER_CINT_ENABLED_Val  << SDHC_NISTER_CINT_Pos)
#define SDHC_NISTER_MASK            _U_(0x01FF)  /**< \brief (SDHC_NISTER) MASK Register */

// EMMC mode
#define SDHC_NISTER_EMMC_CMDC_Pos   0            /**< \brief (SDHC_NISTER_EMMC) Command Complete Status Enable */
#define SDHC_NISTER_EMMC_CMDC       (_U_(0x1) << SDHC_NISTER_EMMC_CMDC_Pos)
#define   SDHC_NISTER_EMMC_CMDC_MASKED_Val _U_(0x0)   /**< \brief (SDHC_NISTER_EMMC) Masked */
#define   SDHC_NISTER_EMMC_CMDC_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_NISTER_EMMC) Enabled */
#define SDHC_NISTER_EMMC_CMDC_MASKED (SDHC_NISTER_EMMC_CMDC_MASKED_Val << SDHC_NISTER_EMMC_CMDC_Pos)
#define SDHC_NISTER_EMMC_CMDC_ENABLED (SDHC_NISTER_EMMC_CMDC_ENABLED_Val << SDHC_NISTER_EMMC_CMDC_Pos)
#define SDHC_NISTER_EMMC_TRFC_Pos   1            /**< \brief (SDHC_NISTER_EMMC) Transfer Complete Status Enable */
#define SDHC_NISTER_EMMC_TRFC       (_U_(0x1) << SDHC_NISTER_EMMC_TRFC_Pos)
#define   SDHC_NISTER_EMMC_TRFC_MASKED_Val _U_(0x0)   /**< \brief (SDHC_NISTER_EMMC) Masked */
#define   SDHC_NISTER_EMMC_TRFC_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_NISTER_EMMC) Enabled */
#define SDHC_NISTER_EMMC_TRFC_MASKED (SDHC_NISTER_EMMC_TRFC_MASKED_Val << SDHC_NISTER_EMMC_TRFC_Pos)
#define SDHC_NISTER_EMMC_TRFC_ENABLED (SDHC_NISTER_EMMC_TRFC_ENABLED_Val << SDHC_NISTER_EMMC_TRFC_Pos)
#define SDHC_NISTER_EMMC_BLKGE_Pos  2            /**< \brief (SDHC_NISTER_EMMC) Block Gap Event Status Enable */
#define SDHC_NISTER_EMMC_BLKGE      (_U_(0x1) << SDHC_NISTER_EMMC_BLKGE_Pos)
#define   SDHC_NISTER_EMMC_BLKGE_MASKED_Val _U_(0x0)   /**< \brief (SDHC_NISTER_EMMC) Masked */
#define   SDHC_NISTER_EMMC_BLKGE_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_NISTER_EMMC) Enabled */
#define SDHC_NISTER_EMMC_BLKGE_MASKED (SDHC_NISTER_EMMC_BLKGE_MASKED_Val << SDHC_NISTER_EMMC_BLKGE_Pos)
#define SDHC_NISTER_EMMC_BLKGE_ENABLED (SDHC_NISTER_EMMC_BLKGE_ENABLED_Val << SDHC_NISTER_EMMC_BLKGE_Pos)
#define SDHC_NISTER_EMMC_DMAINT_Pos 3            /**< \brief (SDHC_NISTER_EMMC) DMA Interrupt Status Enable */
#define SDHC_NISTER_EMMC_DMAINT     (_U_(0x1) << SDHC_NISTER_EMMC_DMAINT_Pos)
#define   SDHC_NISTER_EMMC_DMAINT_MASKED_Val _U_(0x0)   /**< \brief (SDHC_NISTER_EMMC) Masked */
#define   SDHC_NISTER_EMMC_DMAINT_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_NISTER_EMMC) Enabled */
#define SDHC_NISTER_EMMC_DMAINT_MASKED (SDHC_NISTER_EMMC_DMAINT_MASKED_Val << SDHC_NISTER_EMMC_DMAINT_Pos)
#define SDHC_NISTER_EMMC_DMAINT_ENABLED (SDHC_NISTER_EMMC_DMAINT_ENABLED_Val << SDHC_NISTER_EMMC_DMAINT_Pos)
#define SDHC_NISTER_EMMC_BWRRDY_Pos 4            /**< \brief (SDHC_NISTER_EMMC) Buffer Write Ready Status Enable */
#define SDHC_NISTER_EMMC_BWRRDY     (_U_(0x1) << SDHC_NISTER_EMMC_BWRRDY_Pos)
#define   SDHC_NISTER_EMMC_BWRRDY_MASKED_Val _U_(0x0)   /**< \brief (SDHC_NISTER_EMMC) Masked */
#define   SDHC_NISTER_EMMC_BWRRDY_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_NISTER_EMMC) Enabled */
#define SDHC_NISTER_EMMC_BWRRDY_MASKED (SDHC_NISTER_EMMC_BWRRDY_MASKED_Val << SDHC_NISTER_EMMC_BWRRDY_Pos)
#define SDHC_NISTER_EMMC_BWRRDY_ENABLED (SDHC_NISTER_EMMC_BWRRDY_ENABLED_Val << SDHC_NISTER_EMMC_BWRRDY_Pos)
#define SDHC_NISTER_EMMC_BRDRDY_Pos 5            /**< \brief (SDHC_NISTER_EMMC) Buffer Read Ready Status Enable */
#define SDHC_NISTER_EMMC_BRDRDY     (_U_(0x1) << SDHC_NISTER_EMMC_BRDRDY_Pos)
#define   SDHC_NISTER_EMMC_BRDRDY_MASKED_Val _U_(0x0)   /**< \brief (SDHC_NISTER_EMMC) Masked */
#define   SDHC_NISTER_EMMC_BRDRDY_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_NISTER_EMMC) Enabled */
#define SDHC_NISTER_EMMC_BRDRDY_MASKED (SDHC_NISTER_EMMC_BRDRDY_MASKED_Val << SDHC_NISTER_EMMC_BRDRDY_Pos)
#define SDHC_NISTER_EMMC_BRDRDY_ENABLED (SDHC_NISTER_EMMC_BRDRDY_ENABLED_Val << SDHC_NISTER_EMMC_BRDRDY_Pos)
#define SDHC_NISTER_EMMC_BOOTAR_Pos 14           /**< \brief (SDHC_NISTER_EMMC) Boot Acknowledge Received Status Enable */
#define SDHC_NISTER_EMMC_BOOTAR     (_U_(0x1) << SDHC_NISTER_EMMC_BOOTAR_Pos)
#define SDHC_NISTER_EMMC_MASK       _U_(0x403F)  /**< \brief (SDHC_NISTER_EMMC) MASK Register */

/* -------- SDHC_EISTER : (SDHC Offset: 0x036) (R/W 16) Error Interrupt Status Enable -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t CMDTEO:1;         /*!< bit:      0  Command Timeout Error Status Enable */
    uint16_t CMDCRC:1;         /*!< bit:      1  Command CRC Error Status Enable    */
    uint16_t CMDEND:1;         /*!< bit:      2  Command End Bit Error Status Enable */
    uint16_t CMDIDX:1;         /*!< bit:      3  Command Index Error Status Enable  */
    uint16_t DATTEO:1;         /*!< bit:      4  Data Timeout Error Status Enable   */
    uint16_t DATCRC:1;         /*!< bit:      5  Data CRC Error Status Enable       */
    uint16_t DATEND:1;         /*!< bit:      6  Data End Bit Error Status Enable   */
    uint16_t CURLIM:1;         /*!< bit:      7  Current Limit Error Status Enable  */
    uint16_t ACMD:1;           /*!< bit:      8  Auto CMD Error Status Enable       */
    uint16_t ADMA:1;           /*!< bit:      9  ADMA Error Status Enable           */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct { // EMMC mode
    uint16_t CMDTEO:1;         /*!< bit:      0  Command Timeout Error Status Enable */
    uint16_t CMDCRC:1;         /*!< bit:      1  Command CRC Error Status Enable    */
    uint16_t CMDEND:1;         /*!< bit:      2  Command End Bit Error Status Enable */
    uint16_t CMDIDX:1;         /*!< bit:      3  Command Index Error Status Enable  */
    uint16_t DATTEO:1;         /*!< bit:      4  Data Timeout Error Status Enable   */
    uint16_t DATCRC:1;         /*!< bit:      5  Data CRC Error Status Enable       */
    uint16_t DATEND:1;         /*!< bit:      6  Data End Bit Error Status Enable   */
    uint16_t CURLIM:1;         /*!< bit:      7  Current Limit Error Status Enable  */
    uint16_t ACMD:1;           /*!< bit:      8  Auto CMD Error Status Enable       */
    uint16_t ADMA:1;           /*!< bit:      9  ADMA Error Status Enable           */
    uint16_t :2;               /*!< bit: 10..11  Reserved                           */
    uint16_t BOOTAE:1;         /*!< bit:     12  Boot Acknowledge Error Status Enable */
    uint16_t :3;               /*!< bit: 13..15  Reserved                           */
  } EMMC;                      /*!< Structure used for EMMC                         */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_EISTER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_EISTER_OFFSET          0x036        /**< \brief (SDHC_EISTER offset) Error Interrupt Status Enable */
#define SDHC_EISTER_RESETVALUE      _U_(0x0000)  /**< \brief (SDHC_EISTER reset_value) Error Interrupt Status Enable */

#define SDHC_EISTER_CMDTEO_Pos      0            /**< \brief (SDHC_EISTER) Command Timeout Error Status Enable */
#define SDHC_EISTER_CMDTEO          (_U_(0x1) << SDHC_EISTER_CMDTEO_Pos)
#define   SDHC_EISTER_CMDTEO_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_EISTER) Masked */
#define   SDHC_EISTER_CMDTEO_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_EISTER) Enabled */
#define SDHC_EISTER_CMDTEO_MASKED   (SDHC_EISTER_CMDTEO_MASKED_Val << SDHC_EISTER_CMDTEO_Pos)
#define SDHC_EISTER_CMDTEO_ENABLED  (SDHC_EISTER_CMDTEO_ENABLED_Val << SDHC_EISTER_CMDTEO_Pos)
#define SDHC_EISTER_CMDCRC_Pos      1            /**< \brief (SDHC_EISTER) Command CRC Error Status Enable */
#define SDHC_EISTER_CMDCRC          (_U_(0x1) << SDHC_EISTER_CMDCRC_Pos)
#define   SDHC_EISTER_CMDCRC_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_EISTER) Masked */
#define   SDHC_EISTER_CMDCRC_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_EISTER) Enabled */
#define SDHC_EISTER_CMDCRC_MASKED   (SDHC_EISTER_CMDCRC_MASKED_Val << SDHC_EISTER_CMDCRC_Pos)
#define SDHC_EISTER_CMDCRC_ENABLED  (SDHC_EISTER_CMDCRC_ENABLED_Val << SDHC_EISTER_CMDCRC_Pos)
#define SDHC_EISTER_CMDEND_Pos      2            /**< \brief (SDHC_EISTER) Command End Bit Error Status Enable */
#define SDHC_EISTER_CMDEND          (_U_(0x1) << SDHC_EISTER_CMDEND_Pos)
#define   SDHC_EISTER_CMDEND_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_EISTER) Masked */
#define   SDHC_EISTER_CMDEND_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_EISTER) Enabled */
#define SDHC_EISTER_CMDEND_MASKED   (SDHC_EISTER_CMDEND_MASKED_Val << SDHC_EISTER_CMDEND_Pos)
#define SDHC_EISTER_CMDEND_ENABLED  (SDHC_EISTER_CMDEND_ENABLED_Val << SDHC_EISTER_CMDEND_Pos)
#define SDHC_EISTER_CMDIDX_Pos      3            /**< \brief (SDHC_EISTER) Command Index Error Status Enable */
#define SDHC_EISTER_CMDIDX          (_U_(0x1) << SDHC_EISTER_CMDIDX_Pos)
#define   SDHC_EISTER_CMDIDX_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_EISTER) Masked */
#define   SDHC_EISTER_CMDIDX_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_EISTER) Enabled */
#define SDHC_EISTER_CMDIDX_MASKED   (SDHC_EISTER_CMDIDX_MASKED_Val << SDHC_EISTER_CMDIDX_Pos)
#define SDHC_EISTER_CMDIDX_ENABLED  (SDHC_EISTER_CMDIDX_ENABLED_Val << SDHC_EISTER_CMDIDX_Pos)
#define SDHC_EISTER_DATTEO_Pos      4            /**< \brief (SDHC_EISTER) Data Timeout Error Status Enable */
#define SDHC_EISTER_DATTEO          (_U_(0x1) << SDHC_EISTER_DATTEO_Pos)
#define   SDHC_EISTER_DATTEO_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_EISTER) Masked */
#define   SDHC_EISTER_DATTEO_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_EISTER) Enabled */
#define SDHC_EISTER_DATTEO_MASKED   (SDHC_EISTER_DATTEO_MASKED_Val << SDHC_EISTER_DATTEO_Pos)
#define SDHC_EISTER_DATTEO_ENABLED  (SDHC_EISTER_DATTEO_ENABLED_Val << SDHC_EISTER_DATTEO_Pos)
#define SDHC_EISTER_DATCRC_Pos      5            /**< \brief (SDHC_EISTER) Data CRC Error Status Enable */
#define SDHC_EISTER_DATCRC          (_U_(0x1) << SDHC_EISTER_DATCRC_Pos)
#define   SDHC_EISTER_DATCRC_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_EISTER) Masked */
#define   SDHC_EISTER_DATCRC_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_EISTER) Enabled */
#define SDHC_EISTER_DATCRC_MASKED   (SDHC_EISTER_DATCRC_MASKED_Val << SDHC_EISTER_DATCRC_Pos)
#define SDHC_EISTER_DATCRC_ENABLED  (SDHC_EISTER_DATCRC_ENABLED_Val << SDHC_EISTER_DATCRC_Pos)
#define SDHC_EISTER_DATEND_Pos      6            /**< \brief (SDHC_EISTER) Data End Bit Error Status Enable */
#define SDHC_EISTER_DATEND          (_U_(0x1) << SDHC_EISTER_DATEND_Pos)
#define   SDHC_EISTER_DATEND_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_EISTER) Masked */
#define   SDHC_EISTER_DATEND_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_EISTER) Enabled */
#define SDHC_EISTER_DATEND_MASKED   (SDHC_EISTER_DATEND_MASKED_Val << SDHC_EISTER_DATEND_Pos)
#define SDHC_EISTER_DATEND_ENABLED  (SDHC_EISTER_DATEND_ENABLED_Val << SDHC_EISTER_DATEND_Pos)
#define SDHC_EISTER_CURLIM_Pos      7            /**< \brief (SDHC_EISTER) Current Limit Error Status Enable */
#define SDHC_EISTER_CURLIM          (_U_(0x1) << SDHC_EISTER_CURLIM_Pos)
#define   SDHC_EISTER_CURLIM_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_EISTER) Masked */
#define   SDHC_EISTER_CURLIM_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_EISTER) Enabled */
#define SDHC_EISTER_CURLIM_MASKED   (SDHC_EISTER_CURLIM_MASKED_Val << SDHC_EISTER_CURLIM_Pos)
#define SDHC_EISTER_CURLIM_ENABLED  (SDHC_EISTER_CURLIM_ENABLED_Val << SDHC_EISTER_CURLIM_Pos)
#define SDHC_EISTER_ACMD_Pos        8            /**< \brief (SDHC_EISTER) Auto CMD Error Status Enable */
#define SDHC_EISTER_ACMD            (_U_(0x1) << SDHC_EISTER_ACMD_Pos)
#define   SDHC_EISTER_ACMD_MASKED_Val     _U_(0x0)   /**< \brief (SDHC_EISTER) Masked */
#define   SDHC_EISTER_ACMD_ENABLED_Val    _U_(0x1)   /**< \brief (SDHC_EISTER) Enabled */
#define SDHC_EISTER_ACMD_MASKED     (SDHC_EISTER_ACMD_MASKED_Val   << SDHC_EISTER_ACMD_Pos)
#define SDHC_EISTER_ACMD_ENABLED    (SDHC_EISTER_ACMD_ENABLED_Val  << SDHC_EISTER_ACMD_Pos)
#define SDHC_EISTER_ADMA_Pos        9            /**< \brief (SDHC_EISTER) ADMA Error Status Enable */
#define SDHC_EISTER_ADMA            (_U_(0x1) << SDHC_EISTER_ADMA_Pos)
#define   SDHC_EISTER_ADMA_MASKED_Val     _U_(0x0)   /**< \brief (SDHC_EISTER) Masked */
#define   SDHC_EISTER_ADMA_ENABLED_Val    _U_(0x1)   /**< \brief (SDHC_EISTER) Enabled */
#define SDHC_EISTER_ADMA_MASKED     (SDHC_EISTER_ADMA_MASKED_Val   << SDHC_EISTER_ADMA_Pos)
#define SDHC_EISTER_ADMA_ENABLED    (SDHC_EISTER_ADMA_ENABLED_Val  << SDHC_EISTER_ADMA_Pos)
#define SDHC_EISTER_MASK            _U_(0x03FF)  /**< \brief (SDHC_EISTER) MASK Register */

// EMMC mode
#define SDHC_EISTER_EMMC_CMDTEO_Pos 0            /**< \brief (SDHC_EISTER_EMMC) Command Timeout Error Status Enable */
#define SDHC_EISTER_EMMC_CMDTEO     (_U_(0x1) << SDHC_EISTER_EMMC_CMDTEO_Pos)
#define   SDHC_EISTER_EMMC_CMDTEO_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISTER_EMMC) Masked */
#define   SDHC_EISTER_EMMC_CMDTEO_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISTER_EMMC) Enabled */
#define SDHC_EISTER_EMMC_CMDTEO_MASKED (SDHC_EISTER_EMMC_CMDTEO_MASKED_Val << SDHC_EISTER_EMMC_CMDTEO_Pos)
#define SDHC_EISTER_EMMC_CMDTEO_ENABLED (SDHC_EISTER_EMMC_CMDTEO_ENABLED_Val << SDHC_EISTER_EMMC_CMDTEO_Pos)
#define SDHC_EISTER_EMMC_CMDCRC_Pos 1            /**< \brief (SDHC_EISTER_EMMC) Command CRC Error Status Enable */
#define SDHC_EISTER_EMMC_CMDCRC     (_U_(0x1) << SDHC_EISTER_EMMC_CMDCRC_Pos)
#define   SDHC_EISTER_EMMC_CMDCRC_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISTER_EMMC) Masked */
#define   SDHC_EISTER_EMMC_CMDCRC_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISTER_EMMC) Enabled */
#define SDHC_EISTER_EMMC_CMDCRC_MASKED (SDHC_EISTER_EMMC_CMDCRC_MASKED_Val << SDHC_EISTER_EMMC_CMDCRC_Pos)
#define SDHC_EISTER_EMMC_CMDCRC_ENABLED (SDHC_EISTER_EMMC_CMDCRC_ENABLED_Val << SDHC_EISTER_EMMC_CMDCRC_Pos)
#define SDHC_EISTER_EMMC_CMDEND_Pos 2            /**< \brief (SDHC_EISTER_EMMC) Command End Bit Error Status Enable */
#define SDHC_EISTER_EMMC_CMDEND     (_U_(0x1) << SDHC_EISTER_EMMC_CMDEND_Pos)
#define   SDHC_EISTER_EMMC_CMDEND_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISTER_EMMC) Masked */
#define   SDHC_EISTER_EMMC_CMDEND_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISTER_EMMC) Enabled */
#define SDHC_EISTER_EMMC_CMDEND_MASKED (SDHC_EISTER_EMMC_CMDEND_MASKED_Val << SDHC_EISTER_EMMC_CMDEND_Pos)
#define SDHC_EISTER_EMMC_CMDEND_ENABLED (SDHC_EISTER_EMMC_CMDEND_ENABLED_Val << SDHC_EISTER_EMMC_CMDEND_Pos)
#define SDHC_EISTER_EMMC_CMDIDX_Pos 3            /**< \brief (SDHC_EISTER_EMMC) Command Index Error Status Enable */
#define SDHC_EISTER_EMMC_CMDIDX     (_U_(0x1) << SDHC_EISTER_EMMC_CMDIDX_Pos)
#define   SDHC_EISTER_EMMC_CMDIDX_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISTER_EMMC) Masked */
#define   SDHC_EISTER_EMMC_CMDIDX_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISTER_EMMC) Enabled */
#define SDHC_EISTER_EMMC_CMDIDX_MASKED (SDHC_EISTER_EMMC_CMDIDX_MASKED_Val << SDHC_EISTER_EMMC_CMDIDX_Pos)
#define SDHC_EISTER_EMMC_CMDIDX_ENABLED (SDHC_EISTER_EMMC_CMDIDX_ENABLED_Val << SDHC_EISTER_EMMC_CMDIDX_Pos)
#define SDHC_EISTER_EMMC_DATTEO_Pos 4            /**< \brief (SDHC_EISTER_EMMC) Data Timeout Error Status Enable */
#define SDHC_EISTER_EMMC_DATTEO     (_U_(0x1) << SDHC_EISTER_EMMC_DATTEO_Pos)
#define   SDHC_EISTER_EMMC_DATTEO_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISTER_EMMC) Masked */
#define   SDHC_EISTER_EMMC_DATTEO_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISTER_EMMC) Enabled */
#define SDHC_EISTER_EMMC_DATTEO_MASKED (SDHC_EISTER_EMMC_DATTEO_MASKED_Val << SDHC_EISTER_EMMC_DATTEO_Pos)
#define SDHC_EISTER_EMMC_DATTEO_ENABLED (SDHC_EISTER_EMMC_DATTEO_ENABLED_Val << SDHC_EISTER_EMMC_DATTEO_Pos)
#define SDHC_EISTER_EMMC_DATCRC_Pos 5            /**< \brief (SDHC_EISTER_EMMC) Data CRC Error Status Enable */
#define SDHC_EISTER_EMMC_DATCRC     (_U_(0x1) << SDHC_EISTER_EMMC_DATCRC_Pos)
#define   SDHC_EISTER_EMMC_DATCRC_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISTER_EMMC) Masked */
#define   SDHC_EISTER_EMMC_DATCRC_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISTER_EMMC) Enabled */
#define SDHC_EISTER_EMMC_DATCRC_MASKED (SDHC_EISTER_EMMC_DATCRC_MASKED_Val << SDHC_EISTER_EMMC_DATCRC_Pos)
#define SDHC_EISTER_EMMC_DATCRC_ENABLED (SDHC_EISTER_EMMC_DATCRC_ENABLED_Val << SDHC_EISTER_EMMC_DATCRC_Pos)
#define SDHC_EISTER_EMMC_DATEND_Pos 6            /**< \brief (SDHC_EISTER_EMMC) Data End Bit Error Status Enable */
#define SDHC_EISTER_EMMC_DATEND     (_U_(0x1) << SDHC_EISTER_EMMC_DATEND_Pos)
#define   SDHC_EISTER_EMMC_DATEND_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISTER_EMMC) Masked */
#define   SDHC_EISTER_EMMC_DATEND_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISTER_EMMC) Enabled */
#define SDHC_EISTER_EMMC_DATEND_MASKED (SDHC_EISTER_EMMC_DATEND_MASKED_Val << SDHC_EISTER_EMMC_DATEND_Pos)
#define SDHC_EISTER_EMMC_DATEND_ENABLED (SDHC_EISTER_EMMC_DATEND_ENABLED_Val << SDHC_EISTER_EMMC_DATEND_Pos)
#define SDHC_EISTER_EMMC_CURLIM_Pos 7            /**< \brief (SDHC_EISTER_EMMC) Current Limit Error Status Enable */
#define SDHC_EISTER_EMMC_CURLIM     (_U_(0x1) << SDHC_EISTER_EMMC_CURLIM_Pos)
#define   SDHC_EISTER_EMMC_CURLIM_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISTER_EMMC) Masked */
#define   SDHC_EISTER_EMMC_CURLIM_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISTER_EMMC) Enabled */
#define SDHC_EISTER_EMMC_CURLIM_MASKED (SDHC_EISTER_EMMC_CURLIM_MASKED_Val << SDHC_EISTER_EMMC_CURLIM_Pos)
#define SDHC_EISTER_EMMC_CURLIM_ENABLED (SDHC_EISTER_EMMC_CURLIM_ENABLED_Val << SDHC_EISTER_EMMC_CURLIM_Pos)
#define SDHC_EISTER_EMMC_ACMD_Pos   8            /**< \brief (SDHC_EISTER_EMMC) Auto CMD Error Status Enable */
#define SDHC_EISTER_EMMC_ACMD       (_U_(0x1) << SDHC_EISTER_EMMC_ACMD_Pos)
#define   SDHC_EISTER_EMMC_ACMD_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISTER_EMMC) Masked */
#define   SDHC_EISTER_EMMC_ACMD_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISTER_EMMC) Enabled */
#define SDHC_EISTER_EMMC_ACMD_MASKED (SDHC_EISTER_EMMC_ACMD_MASKED_Val << SDHC_EISTER_EMMC_ACMD_Pos)
#define SDHC_EISTER_EMMC_ACMD_ENABLED (SDHC_EISTER_EMMC_ACMD_ENABLED_Val << SDHC_EISTER_EMMC_ACMD_Pos)
#define SDHC_EISTER_EMMC_ADMA_Pos   9            /**< \brief (SDHC_EISTER_EMMC) ADMA Error Status Enable */
#define SDHC_EISTER_EMMC_ADMA       (_U_(0x1) << SDHC_EISTER_EMMC_ADMA_Pos)
#define   SDHC_EISTER_EMMC_ADMA_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISTER_EMMC) Masked */
#define   SDHC_EISTER_EMMC_ADMA_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISTER_EMMC) Enabled */
#define SDHC_EISTER_EMMC_ADMA_MASKED (SDHC_EISTER_EMMC_ADMA_MASKED_Val << SDHC_EISTER_EMMC_ADMA_Pos)
#define SDHC_EISTER_EMMC_ADMA_ENABLED (SDHC_EISTER_EMMC_ADMA_ENABLED_Val << SDHC_EISTER_EMMC_ADMA_Pos)
#define SDHC_EISTER_EMMC_BOOTAE_Pos 12           /**< \brief (SDHC_EISTER_EMMC) Boot Acknowledge Error Status Enable */
#define SDHC_EISTER_EMMC_BOOTAE     (_U_(0x1) << SDHC_EISTER_EMMC_BOOTAE_Pos)
#define SDHC_EISTER_EMMC_MASK       _U_(0x13FF)  /**< \brief (SDHC_EISTER_EMMC) MASK Register */

/* -------- SDHC_NISIER : (SDHC Offset: 0x038) (R/W 16) Normal Interrupt Signal Enable -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t CMDC:1;           /*!< bit:      0  Command Complete Signal Enable     */
    uint16_t TRFC:1;           /*!< bit:      1  Transfer Complete Signal Enable    */
    uint16_t BLKGE:1;          /*!< bit:      2  Block Gap Event Signal Enable      */
    uint16_t DMAINT:1;         /*!< bit:      3  DMA Interrupt Signal Enable        */
    uint16_t BWRRDY:1;         /*!< bit:      4  Buffer Write Ready Signal Enable   */
    uint16_t BRDRDY:1;         /*!< bit:      5  Buffer Read Ready Signal Enable    */
    uint16_t CINS:1;           /*!< bit:      6  Card Insertion Signal Enable       */
    uint16_t CREM:1;           /*!< bit:      7  Card Removal Signal Enable         */
    uint16_t CINT:1;           /*!< bit:      8  Card Interrupt Signal Enable       */
    uint16_t :7;               /*!< bit:  9..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct { // EMMC mode
    uint16_t CMDC:1;           /*!< bit:      0  Command Complete Signal Enable     */
    uint16_t TRFC:1;           /*!< bit:      1  Transfer Complete Signal Enable    */
    uint16_t BLKGE:1;          /*!< bit:      2  Block Gap Event Signal Enable      */
    uint16_t DMAINT:1;         /*!< bit:      3  DMA Interrupt Signal Enable        */
    uint16_t BWRRDY:1;         /*!< bit:      4  Buffer Write Ready Signal Enable   */
    uint16_t BRDRDY:1;         /*!< bit:      5  Buffer Read Ready Signal Enable    */
    uint16_t :8;               /*!< bit:  6..13  Reserved                           */
    uint16_t BOOTAR:1;         /*!< bit:     14  Boot Acknowledge Received Signal Enable */
    uint16_t :1;               /*!< bit:     15  Reserved                           */
  } EMMC;                      /*!< Structure used for EMMC                         */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_NISIER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_NISIER_OFFSET          0x038        /**< \brief (SDHC_NISIER offset) Normal Interrupt Signal Enable */
#define SDHC_NISIER_RESETVALUE      _U_(0x0000)  /**< \brief (SDHC_NISIER reset_value) Normal Interrupt Signal Enable */

#define SDHC_NISIER_CMDC_Pos        0            /**< \brief (SDHC_NISIER) Command Complete Signal Enable */
#define SDHC_NISIER_CMDC            (_U_(0x1) << SDHC_NISIER_CMDC_Pos)
#define   SDHC_NISIER_CMDC_MASKED_Val     _U_(0x0)   /**< \brief (SDHC_NISIER) Masked */
#define   SDHC_NISIER_CMDC_ENABLED_Val    _U_(0x1)   /**< \brief (SDHC_NISIER) Enabled */
#define SDHC_NISIER_CMDC_MASKED     (SDHC_NISIER_CMDC_MASKED_Val   << SDHC_NISIER_CMDC_Pos)
#define SDHC_NISIER_CMDC_ENABLED    (SDHC_NISIER_CMDC_ENABLED_Val  << SDHC_NISIER_CMDC_Pos)
#define SDHC_NISIER_TRFC_Pos        1            /**< \brief (SDHC_NISIER) Transfer Complete Signal Enable */
#define SDHC_NISIER_TRFC            (_U_(0x1) << SDHC_NISIER_TRFC_Pos)
#define   SDHC_NISIER_TRFC_MASKED_Val     _U_(0x0)   /**< \brief (SDHC_NISIER) Masked */
#define   SDHC_NISIER_TRFC_ENABLED_Val    _U_(0x1)   /**< \brief (SDHC_NISIER) Enabled */
#define SDHC_NISIER_TRFC_MASKED     (SDHC_NISIER_TRFC_MASKED_Val   << SDHC_NISIER_TRFC_Pos)
#define SDHC_NISIER_TRFC_ENABLED    (SDHC_NISIER_TRFC_ENABLED_Val  << SDHC_NISIER_TRFC_Pos)
#define SDHC_NISIER_BLKGE_Pos       2            /**< \brief (SDHC_NISIER) Block Gap Event Signal Enable */
#define SDHC_NISIER_BLKGE           (_U_(0x1) << SDHC_NISIER_BLKGE_Pos)
#define   SDHC_NISIER_BLKGE_MASKED_Val    _U_(0x0)   /**< \brief (SDHC_NISIER) Masked */
#define   SDHC_NISIER_BLKGE_ENABLED_Val   _U_(0x1)   /**< \brief (SDHC_NISIER) Enabled */
#define SDHC_NISIER_BLKGE_MASKED    (SDHC_NISIER_BLKGE_MASKED_Val  << SDHC_NISIER_BLKGE_Pos)
#define SDHC_NISIER_BLKGE_ENABLED   (SDHC_NISIER_BLKGE_ENABLED_Val << SDHC_NISIER_BLKGE_Pos)
#define SDHC_NISIER_DMAINT_Pos      3            /**< \brief (SDHC_NISIER) DMA Interrupt Signal Enable */
#define SDHC_NISIER_DMAINT          (_U_(0x1) << SDHC_NISIER_DMAINT_Pos)
#define   SDHC_NISIER_DMAINT_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_NISIER) Masked */
#define   SDHC_NISIER_DMAINT_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_NISIER) Enabled */
#define SDHC_NISIER_DMAINT_MASKED   (SDHC_NISIER_DMAINT_MASKED_Val << SDHC_NISIER_DMAINT_Pos)
#define SDHC_NISIER_DMAINT_ENABLED  (SDHC_NISIER_DMAINT_ENABLED_Val << SDHC_NISIER_DMAINT_Pos)
#define SDHC_NISIER_BWRRDY_Pos      4            /**< \brief (SDHC_NISIER) Buffer Write Ready Signal Enable */
#define SDHC_NISIER_BWRRDY          (_U_(0x1) << SDHC_NISIER_BWRRDY_Pos)
#define   SDHC_NISIER_BWRRDY_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_NISIER) Masked */
#define   SDHC_NISIER_BWRRDY_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_NISIER) Enabled */
#define SDHC_NISIER_BWRRDY_MASKED   (SDHC_NISIER_BWRRDY_MASKED_Val << SDHC_NISIER_BWRRDY_Pos)
#define SDHC_NISIER_BWRRDY_ENABLED  (SDHC_NISIER_BWRRDY_ENABLED_Val << SDHC_NISIER_BWRRDY_Pos)
#define SDHC_NISIER_BRDRDY_Pos      5            /**< \brief (SDHC_NISIER) Buffer Read Ready Signal Enable */
#define SDHC_NISIER_BRDRDY          (_U_(0x1) << SDHC_NISIER_BRDRDY_Pos)
#define   SDHC_NISIER_BRDRDY_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_NISIER) Masked */
#define   SDHC_NISIER_BRDRDY_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_NISIER) Enabled */
#define SDHC_NISIER_BRDRDY_MASKED   (SDHC_NISIER_BRDRDY_MASKED_Val << SDHC_NISIER_BRDRDY_Pos)
#define SDHC_NISIER_BRDRDY_ENABLED  (SDHC_NISIER_BRDRDY_ENABLED_Val << SDHC_NISIER_BRDRDY_Pos)
#define SDHC_NISIER_CINS_Pos        6            /**< \brief (SDHC_NISIER) Card Insertion Signal Enable */
#define SDHC_NISIER_CINS            (_U_(0x1) << SDHC_NISIER_CINS_Pos)
#define   SDHC_NISIER_CINS_MASKED_Val     _U_(0x0)   /**< \brief (SDHC_NISIER) Masked */
#define   SDHC_NISIER_CINS_ENABLED_Val    _U_(0x1)   /**< \brief (SDHC_NISIER) Enabled */
#define SDHC_NISIER_CINS_MASKED     (SDHC_NISIER_CINS_MASKED_Val   << SDHC_NISIER_CINS_Pos)
#define SDHC_NISIER_CINS_ENABLED    (SDHC_NISIER_CINS_ENABLED_Val  << SDHC_NISIER_CINS_Pos)
#define SDHC_NISIER_CREM_Pos        7            /**< \brief (SDHC_NISIER) Card Removal Signal Enable */
#define SDHC_NISIER_CREM            (_U_(0x1) << SDHC_NISIER_CREM_Pos)
#define   SDHC_NISIER_CREM_MASKED_Val     _U_(0x0)   /**< \brief (SDHC_NISIER) Masked */
#define   SDHC_NISIER_CREM_ENABLED_Val    _U_(0x1)   /**< \brief (SDHC_NISIER) Enabled */
#define SDHC_NISIER_CREM_MASKED     (SDHC_NISIER_CREM_MASKED_Val   << SDHC_NISIER_CREM_Pos)
#define SDHC_NISIER_CREM_ENABLED    (SDHC_NISIER_CREM_ENABLED_Val  << SDHC_NISIER_CREM_Pos)
#define SDHC_NISIER_CINT_Pos        8            /**< \brief (SDHC_NISIER) Card Interrupt Signal Enable */
#define SDHC_NISIER_CINT            (_U_(0x1) << SDHC_NISIER_CINT_Pos)
#define   SDHC_NISIER_CINT_MASKED_Val     _U_(0x0)   /**< \brief (SDHC_NISIER) Masked */
#define   SDHC_NISIER_CINT_ENABLED_Val    _U_(0x1)   /**< \brief (SDHC_NISIER) Enabled */
#define SDHC_NISIER_CINT_MASKED     (SDHC_NISIER_CINT_MASKED_Val   << SDHC_NISIER_CINT_Pos)
#define SDHC_NISIER_CINT_ENABLED    (SDHC_NISIER_CINT_ENABLED_Val  << SDHC_NISIER_CINT_Pos)
#define SDHC_NISIER_MASK            _U_(0x01FF)  /**< \brief (SDHC_NISIER) MASK Register */

// EMMC mode
#define SDHC_NISIER_EMMC_CMDC_Pos   0            /**< \brief (SDHC_NISIER_EMMC) Command Complete Signal Enable */
#define SDHC_NISIER_EMMC_CMDC       (_U_(0x1) << SDHC_NISIER_EMMC_CMDC_Pos)
#define   SDHC_NISIER_EMMC_CMDC_MASKED_Val _U_(0x0)   /**< \brief (SDHC_NISIER_EMMC) Masked */
#define   SDHC_NISIER_EMMC_CMDC_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_NISIER_EMMC) Enabled */
#define SDHC_NISIER_EMMC_CMDC_MASKED (SDHC_NISIER_EMMC_CMDC_MASKED_Val << SDHC_NISIER_EMMC_CMDC_Pos)
#define SDHC_NISIER_EMMC_CMDC_ENABLED (SDHC_NISIER_EMMC_CMDC_ENABLED_Val << SDHC_NISIER_EMMC_CMDC_Pos)
#define SDHC_NISIER_EMMC_TRFC_Pos   1            /**< \brief (SDHC_NISIER_EMMC) Transfer Complete Signal Enable */
#define SDHC_NISIER_EMMC_TRFC       (_U_(0x1) << SDHC_NISIER_EMMC_TRFC_Pos)
#define   SDHC_NISIER_EMMC_TRFC_MASKED_Val _U_(0x0)   /**< \brief (SDHC_NISIER_EMMC) Masked */
#define   SDHC_NISIER_EMMC_TRFC_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_NISIER_EMMC) Enabled */
#define SDHC_NISIER_EMMC_TRFC_MASKED (SDHC_NISIER_EMMC_TRFC_MASKED_Val << SDHC_NISIER_EMMC_TRFC_Pos)
#define SDHC_NISIER_EMMC_TRFC_ENABLED (SDHC_NISIER_EMMC_TRFC_ENABLED_Val << SDHC_NISIER_EMMC_TRFC_Pos)
#define SDHC_NISIER_EMMC_BLKGE_Pos  2            /**< \brief (SDHC_NISIER_EMMC) Block Gap Event Signal Enable */
#define SDHC_NISIER_EMMC_BLKGE      (_U_(0x1) << SDHC_NISIER_EMMC_BLKGE_Pos)
#define   SDHC_NISIER_EMMC_BLKGE_MASKED_Val _U_(0x0)   /**< \brief (SDHC_NISIER_EMMC) Masked */
#define   SDHC_NISIER_EMMC_BLKGE_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_NISIER_EMMC) Enabled */
#define SDHC_NISIER_EMMC_BLKGE_MASKED (SDHC_NISIER_EMMC_BLKGE_MASKED_Val << SDHC_NISIER_EMMC_BLKGE_Pos)
#define SDHC_NISIER_EMMC_BLKGE_ENABLED (SDHC_NISIER_EMMC_BLKGE_ENABLED_Val << SDHC_NISIER_EMMC_BLKGE_Pos)
#define SDHC_NISIER_EMMC_DMAINT_Pos 3            /**< \brief (SDHC_NISIER_EMMC) DMA Interrupt Signal Enable */
#define SDHC_NISIER_EMMC_DMAINT     (_U_(0x1) << SDHC_NISIER_EMMC_DMAINT_Pos)
#define   SDHC_NISIER_EMMC_DMAINT_MASKED_Val _U_(0x0)   /**< \brief (SDHC_NISIER_EMMC) Masked */
#define   SDHC_NISIER_EMMC_DMAINT_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_NISIER_EMMC) Enabled */
#define SDHC_NISIER_EMMC_DMAINT_MASKED (SDHC_NISIER_EMMC_DMAINT_MASKED_Val << SDHC_NISIER_EMMC_DMAINT_Pos)
#define SDHC_NISIER_EMMC_DMAINT_ENABLED (SDHC_NISIER_EMMC_DMAINT_ENABLED_Val << SDHC_NISIER_EMMC_DMAINT_Pos)
#define SDHC_NISIER_EMMC_BWRRDY_Pos 4            /**< \brief (SDHC_NISIER_EMMC) Buffer Write Ready Signal Enable */
#define SDHC_NISIER_EMMC_BWRRDY     (_U_(0x1) << SDHC_NISIER_EMMC_BWRRDY_Pos)
#define   SDHC_NISIER_EMMC_BWRRDY_MASKED_Val _U_(0x0)   /**< \brief (SDHC_NISIER_EMMC) Masked */
#define   SDHC_NISIER_EMMC_BWRRDY_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_NISIER_EMMC) Enabled */
#define SDHC_NISIER_EMMC_BWRRDY_MASKED (SDHC_NISIER_EMMC_BWRRDY_MASKED_Val << SDHC_NISIER_EMMC_BWRRDY_Pos)
#define SDHC_NISIER_EMMC_BWRRDY_ENABLED (SDHC_NISIER_EMMC_BWRRDY_ENABLED_Val << SDHC_NISIER_EMMC_BWRRDY_Pos)
#define SDHC_NISIER_EMMC_BRDRDY_Pos 5            /**< \brief (SDHC_NISIER_EMMC) Buffer Read Ready Signal Enable */
#define SDHC_NISIER_EMMC_BRDRDY     (_U_(0x1) << SDHC_NISIER_EMMC_BRDRDY_Pos)
#define   SDHC_NISIER_EMMC_BRDRDY_MASKED_Val _U_(0x0)   /**< \brief (SDHC_NISIER_EMMC) Masked */
#define   SDHC_NISIER_EMMC_BRDRDY_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_NISIER_EMMC) Enabled */
#define SDHC_NISIER_EMMC_BRDRDY_MASKED (SDHC_NISIER_EMMC_BRDRDY_MASKED_Val << SDHC_NISIER_EMMC_BRDRDY_Pos)
#define SDHC_NISIER_EMMC_BRDRDY_ENABLED (SDHC_NISIER_EMMC_BRDRDY_ENABLED_Val << SDHC_NISIER_EMMC_BRDRDY_Pos)
#define SDHC_NISIER_EMMC_BOOTAR_Pos 14           /**< \brief (SDHC_NISIER_EMMC) Boot Acknowledge Received Signal Enable */
#define SDHC_NISIER_EMMC_BOOTAR     (_U_(0x1) << SDHC_NISIER_EMMC_BOOTAR_Pos)
#define SDHC_NISIER_EMMC_MASK       _U_(0x403F)  /**< \brief (SDHC_NISIER_EMMC) MASK Register */

/* -------- SDHC_EISIER : (SDHC Offset: 0x03A) (R/W 16) Error Interrupt Signal Enable -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t CMDTEO:1;         /*!< bit:      0  Command Timeout Error Signal Enable */
    uint16_t CMDCRC:1;         /*!< bit:      1  Command CRC Error Signal Enable    */
    uint16_t CMDEND:1;         /*!< bit:      2  Command End Bit Error Signal Enable */
    uint16_t CMDIDX:1;         /*!< bit:      3  Command Index Error Signal Enable  */
    uint16_t DATTEO:1;         /*!< bit:      4  Data Timeout Error Signal Enable   */
    uint16_t DATCRC:1;         /*!< bit:      5  Data CRC Error Signal Enable       */
    uint16_t DATEND:1;         /*!< bit:      6  Data End Bit Error Signal Enable   */
    uint16_t CURLIM:1;         /*!< bit:      7  Current Limit Error Signal Enable  */
    uint16_t ACMD:1;           /*!< bit:      8  Auto CMD Error Signal Enable       */
    uint16_t ADMA:1;           /*!< bit:      9  ADMA Error Signal Enable           */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct { // EMMC mode
    uint16_t CMDTEO:1;         /*!< bit:      0  Command Timeout Error Signal Enable */
    uint16_t CMDCRC:1;         /*!< bit:      1  Command CRC Error Signal Enable    */
    uint16_t CMDEND:1;         /*!< bit:      2  Command End Bit Error Signal Enable */
    uint16_t CMDIDX:1;         /*!< bit:      3  Command Index Error Signal Enable  */
    uint16_t DATTEO:1;         /*!< bit:      4  Data Timeout Error Signal Enable   */
    uint16_t DATCRC:1;         /*!< bit:      5  Data CRC Error Signal Enable       */
    uint16_t DATEND:1;         /*!< bit:      6  Data End Bit Error Signal Enable   */
    uint16_t CURLIM:1;         /*!< bit:      7  Current Limit Error Signal Enable  */
    uint16_t ACMD:1;           /*!< bit:      8  Auto CMD Error Signal Enable       */
    uint16_t ADMA:1;           /*!< bit:      9  ADMA Error Signal Enable           */
    uint16_t :2;               /*!< bit: 10..11  Reserved                           */
    uint16_t BOOTAE:1;         /*!< bit:     12  Boot Acknowledge Error Signal Enable */
    uint16_t :3;               /*!< bit: 13..15  Reserved                           */
  } EMMC;                      /*!< Structure used for EMMC                         */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_EISIER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_EISIER_OFFSET          0x03A        /**< \brief (SDHC_EISIER offset) Error Interrupt Signal Enable */
#define SDHC_EISIER_RESETVALUE      _U_(0x0000)  /**< \brief (SDHC_EISIER reset_value) Error Interrupt Signal Enable */

#define SDHC_EISIER_CMDTEO_Pos      0            /**< \brief (SDHC_EISIER) Command Timeout Error Signal Enable */
#define SDHC_EISIER_CMDTEO          (_U_(0x1) << SDHC_EISIER_CMDTEO_Pos)
#define   SDHC_EISIER_CMDTEO_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_EISIER) Masked */
#define   SDHC_EISIER_CMDTEO_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_EISIER) Enabled */
#define SDHC_EISIER_CMDTEO_MASKED   (SDHC_EISIER_CMDTEO_MASKED_Val << SDHC_EISIER_CMDTEO_Pos)
#define SDHC_EISIER_CMDTEO_ENABLED  (SDHC_EISIER_CMDTEO_ENABLED_Val << SDHC_EISIER_CMDTEO_Pos)
#define SDHC_EISIER_CMDCRC_Pos      1            /**< \brief (SDHC_EISIER) Command CRC Error Signal Enable */
#define SDHC_EISIER_CMDCRC          (_U_(0x1) << SDHC_EISIER_CMDCRC_Pos)
#define   SDHC_EISIER_CMDCRC_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_EISIER) Masked */
#define   SDHC_EISIER_CMDCRC_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_EISIER) Enabled */
#define SDHC_EISIER_CMDCRC_MASKED   (SDHC_EISIER_CMDCRC_MASKED_Val << SDHC_EISIER_CMDCRC_Pos)
#define SDHC_EISIER_CMDCRC_ENABLED  (SDHC_EISIER_CMDCRC_ENABLED_Val << SDHC_EISIER_CMDCRC_Pos)
#define SDHC_EISIER_CMDEND_Pos      2            /**< \brief (SDHC_EISIER) Command End Bit Error Signal Enable */
#define SDHC_EISIER_CMDEND          (_U_(0x1) << SDHC_EISIER_CMDEND_Pos)
#define   SDHC_EISIER_CMDEND_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_EISIER) Masked */
#define   SDHC_EISIER_CMDEND_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_EISIER) Enabled */
#define SDHC_EISIER_CMDEND_MASKED   (SDHC_EISIER_CMDEND_MASKED_Val << SDHC_EISIER_CMDEND_Pos)
#define SDHC_EISIER_CMDEND_ENABLED  (SDHC_EISIER_CMDEND_ENABLED_Val << SDHC_EISIER_CMDEND_Pos)
#define SDHC_EISIER_CMDIDX_Pos      3            /**< \brief (SDHC_EISIER) Command Index Error Signal Enable */
#define SDHC_EISIER_CMDIDX          (_U_(0x1) << SDHC_EISIER_CMDIDX_Pos)
#define   SDHC_EISIER_CMDIDX_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_EISIER) Masked */
#define   SDHC_EISIER_CMDIDX_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_EISIER) Enabled */
#define SDHC_EISIER_CMDIDX_MASKED   (SDHC_EISIER_CMDIDX_MASKED_Val << SDHC_EISIER_CMDIDX_Pos)
#define SDHC_EISIER_CMDIDX_ENABLED  (SDHC_EISIER_CMDIDX_ENABLED_Val << SDHC_EISIER_CMDIDX_Pos)
#define SDHC_EISIER_DATTEO_Pos      4            /**< \brief (SDHC_EISIER) Data Timeout Error Signal Enable */
#define SDHC_EISIER_DATTEO          (_U_(0x1) << SDHC_EISIER_DATTEO_Pos)
#define   SDHC_EISIER_DATTEO_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_EISIER) Masked */
#define   SDHC_EISIER_DATTEO_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_EISIER) Enabled */
#define SDHC_EISIER_DATTEO_MASKED   (SDHC_EISIER_DATTEO_MASKED_Val << SDHC_EISIER_DATTEO_Pos)
#define SDHC_EISIER_DATTEO_ENABLED  (SDHC_EISIER_DATTEO_ENABLED_Val << SDHC_EISIER_DATTEO_Pos)
#define SDHC_EISIER_DATCRC_Pos      5            /**< \brief (SDHC_EISIER) Data CRC Error Signal Enable */
#define SDHC_EISIER_DATCRC          (_U_(0x1) << SDHC_EISIER_DATCRC_Pos)
#define   SDHC_EISIER_DATCRC_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_EISIER) Masked */
#define   SDHC_EISIER_DATCRC_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_EISIER) Enabled */
#define SDHC_EISIER_DATCRC_MASKED   (SDHC_EISIER_DATCRC_MASKED_Val << SDHC_EISIER_DATCRC_Pos)
#define SDHC_EISIER_DATCRC_ENABLED  (SDHC_EISIER_DATCRC_ENABLED_Val << SDHC_EISIER_DATCRC_Pos)
#define SDHC_EISIER_DATEND_Pos      6            /**< \brief (SDHC_EISIER) Data End Bit Error Signal Enable */
#define SDHC_EISIER_DATEND          (_U_(0x1) << SDHC_EISIER_DATEND_Pos)
#define   SDHC_EISIER_DATEND_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_EISIER) Masked */
#define   SDHC_EISIER_DATEND_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_EISIER) Enabled */
#define SDHC_EISIER_DATEND_MASKED   (SDHC_EISIER_DATEND_MASKED_Val << SDHC_EISIER_DATEND_Pos)
#define SDHC_EISIER_DATEND_ENABLED  (SDHC_EISIER_DATEND_ENABLED_Val << SDHC_EISIER_DATEND_Pos)
#define SDHC_EISIER_CURLIM_Pos      7            /**< \brief (SDHC_EISIER) Current Limit Error Signal Enable */
#define SDHC_EISIER_CURLIM          (_U_(0x1) << SDHC_EISIER_CURLIM_Pos)
#define   SDHC_EISIER_CURLIM_MASKED_Val   _U_(0x0)   /**< \brief (SDHC_EISIER) Masked */
#define   SDHC_EISIER_CURLIM_ENABLED_Val  _U_(0x1)   /**< \brief (SDHC_EISIER) Enabled */
#define SDHC_EISIER_CURLIM_MASKED   (SDHC_EISIER_CURLIM_MASKED_Val << SDHC_EISIER_CURLIM_Pos)
#define SDHC_EISIER_CURLIM_ENABLED  (SDHC_EISIER_CURLIM_ENABLED_Val << SDHC_EISIER_CURLIM_Pos)
#define SDHC_EISIER_ACMD_Pos        8            /**< \brief (SDHC_EISIER) Auto CMD Error Signal Enable */
#define SDHC_EISIER_ACMD            (_U_(0x1) << SDHC_EISIER_ACMD_Pos)
#define   SDHC_EISIER_ACMD_MASKED_Val     _U_(0x0)   /**< \brief (SDHC_EISIER) Masked */
#define   SDHC_EISIER_ACMD_ENABLED_Val    _U_(0x1)   /**< \brief (SDHC_EISIER) Enabled */
#define SDHC_EISIER_ACMD_MASKED     (SDHC_EISIER_ACMD_MASKED_Val   << SDHC_EISIER_ACMD_Pos)
#define SDHC_EISIER_ACMD_ENABLED    (SDHC_EISIER_ACMD_ENABLED_Val  << SDHC_EISIER_ACMD_Pos)
#define SDHC_EISIER_ADMA_Pos        9            /**< \brief (SDHC_EISIER) ADMA Error Signal Enable */
#define SDHC_EISIER_ADMA            (_U_(0x1) << SDHC_EISIER_ADMA_Pos)
#define   SDHC_EISIER_ADMA_MASKED_Val     _U_(0x0)   /**< \brief (SDHC_EISIER) Masked */
#define   SDHC_EISIER_ADMA_ENABLED_Val    _U_(0x1)   /**< \brief (SDHC_EISIER) Enabled */
#define SDHC_EISIER_ADMA_MASKED     (SDHC_EISIER_ADMA_MASKED_Val   << SDHC_EISIER_ADMA_Pos)
#define SDHC_EISIER_ADMA_ENABLED    (SDHC_EISIER_ADMA_ENABLED_Val  << SDHC_EISIER_ADMA_Pos)
#define SDHC_EISIER_MASK            _U_(0x03FF)  /**< \brief (SDHC_EISIER) MASK Register */

// EMMC mode
#define SDHC_EISIER_EMMC_CMDTEO_Pos 0            /**< \brief (SDHC_EISIER_EMMC) Command Timeout Error Signal Enable */
#define SDHC_EISIER_EMMC_CMDTEO     (_U_(0x1) << SDHC_EISIER_EMMC_CMDTEO_Pos)
#define   SDHC_EISIER_EMMC_CMDTEO_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISIER_EMMC) Masked */
#define   SDHC_EISIER_EMMC_CMDTEO_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISIER_EMMC) Enabled */
#define SDHC_EISIER_EMMC_CMDTEO_MASKED (SDHC_EISIER_EMMC_CMDTEO_MASKED_Val << SDHC_EISIER_EMMC_CMDTEO_Pos)
#define SDHC_EISIER_EMMC_CMDTEO_ENABLED (SDHC_EISIER_EMMC_CMDTEO_ENABLED_Val << SDHC_EISIER_EMMC_CMDTEO_Pos)
#define SDHC_EISIER_EMMC_CMDCRC_Pos 1            /**< \brief (SDHC_EISIER_EMMC) Command CRC Error Signal Enable */
#define SDHC_EISIER_EMMC_CMDCRC     (_U_(0x1) << SDHC_EISIER_EMMC_CMDCRC_Pos)
#define   SDHC_EISIER_EMMC_CMDCRC_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISIER_EMMC) Masked */
#define   SDHC_EISIER_EMMC_CMDCRC_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISIER_EMMC) Enabled */
#define SDHC_EISIER_EMMC_CMDCRC_MASKED (SDHC_EISIER_EMMC_CMDCRC_MASKED_Val << SDHC_EISIER_EMMC_CMDCRC_Pos)
#define SDHC_EISIER_EMMC_CMDCRC_ENABLED (SDHC_EISIER_EMMC_CMDCRC_ENABLED_Val << SDHC_EISIER_EMMC_CMDCRC_Pos)
#define SDHC_EISIER_EMMC_CMDEND_Pos 2            /**< \brief (SDHC_EISIER_EMMC) Command End Bit Error Signal Enable */
#define SDHC_EISIER_EMMC_CMDEND     (_U_(0x1) << SDHC_EISIER_EMMC_CMDEND_Pos)
#define   SDHC_EISIER_EMMC_CMDEND_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISIER_EMMC) Masked */
#define   SDHC_EISIER_EMMC_CMDEND_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISIER_EMMC) Enabled */
#define SDHC_EISIER_EMMC_CMDEND_MASKED (SDHC_EISIER_EMMC_CMDEND_MASKED_Val << SDHC_EISIER_EMMC_CMDEND_Pos)
#define SDHC_EISIER_EMMC_CMDEND_ENABLED (SDHC_EISIER_EMMC_CMDEND_ENABLED_Val << SDHC_EISIER_EMMC_CMDEND_Pos)
#define SDHC_EISIER_EMMC_CMDIDX_Pos 3            /**< \brief (SDHC_EISIER_EMMC) Command Index Error Signal Enable */
#define SDHC_EISIER_EMMC_CMDIDX     (_U_(0x1) << SDHC_EISIER_EMMC_CMDIDX_Pos)
#define   SDHC_EISIER_EMMC_CMDIDX_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISIER_EMMC) Masked */
#define   SDHC_EISIER_EMMC_CMDIDX_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISIER_EMMC) Enabled */
#define SDHC_EISIER_EMMC_CMDIDX_MASKED (SDHC_EISIER_EMMC_CMDIDX_MASKED_Val << SDHC_EISIER_EMMC_CMDIDX_Pos)
#define SDHC_EISIER_EMMC_CMDIDX_ENABLED (SDHC_EISIER_EMMC_CMDIDX_ENABLED_Val << SDHC_EISIER_EMMC_CMDIDX_Pos)
#define SDHC_EISIER_EMMC_DATTEO_Pos 4            /**< \brief (SDHC_EISIER_EMMC) Data Timeout Error Signal Enable */
#define SDHC_EISIER_EMMC_DATTEO     (_U_(0x1) << SDHC_EISIER_EMMC_DATTEO_Pos)
#define   SDHC_EISIER_EMMC_DATTEO_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISIER_EMMC) Masked */
#define   SDHC_EISIER_EMMC_DATTEO_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISIER_EMMC) Enabled */
#define SDHC_EISIER_EMMC_DATTEO_MASKED (SDHC_EISIER_EMMC_DATTEO_MASKED_Val << SDHC_EISIER_EMMC_DATTEO_Pos)
#define SDHC_EISIER_EMMC_DATTEO_ENABLED (SDHC_EISIER_EMMC_DATTEO_ENABLED_Val << SDHC_EISIER_EMMC_DATTEO_Pos)
#define SDHC_EISIER_EMMC_DATCRC_Pos 5            /**< \brief (SDHC_EISIER_EMMC) Data CRC Error Signal Enable */
#define SDHC_EISIER_EMMC_DATCRC     (_U_(0x1) << SDHC_EISIER_EMMC_DATCRC_Pos)
#define   SDHC_EISIER_EMMC_DATCRC_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISIER_EMMC) Masked */
#define   SDHC_EISIER_EMMC_DATCRC_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISIER_EMMC) Enabled */
#define SDHC_EISIER_EMMC_DATCRC_MASKED (SDHC_EISIER_EMMC_DATCRC_MASKED_Val << SDHC_EISIER_EMMC_DATCRC_Pos)
#define SDHC_EISIER_EMMC_DATCRC_ENABLED (SDHC_EISIER_EMMC_DATCRC_ENABLED_Val << SDHC_EISIER_EMMC_DATCRC_Pos)
#define SDHC_EISIER_EMMC_DATEND_Pos 6            /**< \brief (SDHC_EISIER_EMMC) Data End Bit Error Signal Enable */
#define SDHC_EISIER_EMMC_DATEND     (_U_(0x1) << SDHC_EISIER_EMMC_DATEND_Pos)
#define   SDHC_EISIER_EMMC_DATEND_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISIER_EMMC) Masked */
#define   SDHC_EISIER_EMMC_DATEND_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISIER_EMMC) Enabled */
#define SDHC_EISIER_EMMC_DATEND_MASKED (SDHC_EISIER_EMMC_DATEND_MASKED_Val << SDHC_EISIER_EMMC_DATEND_Pos)
#define SDHC_EISIER_EMMC_DATEND_ENABLED (SDHC_EISIER_EMMC_DATEND_ENABLED_Val << SDHC_EISIER_EMMC_DATEND_Pos)
#define SDHC_EISIER_EMMC_CURLIM_Pos 7            /**< \brief (SDHC_EISIER_EMMC) Current Limit Error Signal Enable */
#define SDHC_EISIER_EMMC_CURLIM     (_U_(0x1) << SDHC_EISIER_EMMC_CURLIM_Pos)
#define   SDHC_EISIER_EMMC_CURLIM_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISIER_EMMC) Masked */
#define   SDHC_EISIER_EMMC_CURLIM_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISIER_EMMC) Enabled */
#define SDHC_EISIER_EMMC_CURLIM_MASKED (SDHC_EISIER_EMMC_CURLIM_MASKED_Val << SDHC_EISIER_EMMC_CURLIM_Pos)
#define SDHC_EISIER_EMMC_CURLIM_ENABLED (SDHC_EISIER_EMMC_CURLIM_ENABLED_Val << SDHC_EISIER_EMMC_CURLIM_Pos)
#define SDHC_EISIER_EMMC_ACMD_Pos   8            /**< \brief (SDHC_EISIER_EMMC) Auto CMD Error Signal Enable */
#define SDHC_EISIER_EMMC_ACMD       (_U_(0x1) << SDHC_EISIER_EMMC_ACMD_Pos)
#define   SDHC_EISIER_EMMC_ACMD_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISIER_EMMC) Masked */
#define   SDHC_EISIER_EMMC_ACMD_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISIER_EMMC) Enabled */
#define SDHC_EISIER_EMMC_ACMD_MASKED (SDHC_EISIER_EMMC_ACMD_MASKED_Val << SDHC_EISIER_EMMC_ACMD_Pos)
#define SDHC_EISIER_EMMC_ACMD_ENABLED (SDHC_EISIER_EMMC_ACMD_ENABLED_Val << SDHC_EISIER_EMMC_ACMD_Pos)
#define SDHC_EISIER_EMMC_ADMA_Pos   9            /**< \brief (SDHC_EISIER_EMMC) ADMA Error Signal Enable */
#define SDHC_EISIER_EMMC_ADMA       (_U_(0x1) << SDHC_EISIER_EMMC_ADMA_Pos)
#define   SDHC_EISIER_EMMC_ADMA_MASKED_Val _U_(0x0)   /**< \brief (SDHC_EISIER_EMMC) Masked */
#define   SDHC_EISIER_EMMC_ADMA_ENABLED_Val _U_(0x1)   /**< \brief (SDHC_EISIER_EMMC) Enabled */
#define SDHC_EISIER_EMMC_ADMA_MASKED (SDHC_EISIER_EMMC_ADMA_MASKED_Val << SDHC_EISIER_EMMC_ADMA_Pos)
#define SDHC_EISIER_EMMC_ADMA_ENABLED (SDHC_EISIER_EMMC_ADMA_ENABLED_Val << SDHC_EISIER_EMMC_ADMA_Pos)
#define SDHC_EISIER_EMMC_BOOTAE_Pos 12           /**< \brief (SDHC_EISIER_EMMC) Boot Acknowledge Error Signal Enable */
#define SDHC_EISIER_EMMC_BOOTAE     (_U_(0x1) << SDHC_EISIER_EMMC_BOOTAE_Pos)
#define SDHC_EISIER_EMMC_MASK       _U_(0x13FF)  /**< \brief (SDHC_EISIER_EMMC) MASK Register */

/* -------- SDHC_ACESR : (SDHC Offset: 0x03C) (R/  16) Auto CMD Error Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t ACMD12NE:1;       /*!< bit:      0  Auto CMD12 Not Executed            */
    uint16_t ACMDTEO:1;        /*!< bit:      1  Auto CMD Timeout Error             */
    uint16_t ACMDCRC:1;        /*!< bit:      2  Auto CMD CRC Error                 */
    uint16_t ACMDEND:1;        /*!< bit:      3  Auto CMD End Bit Error             */
    uint16_t ACMDIDX:1;        /*!< bit:      4  Auto CMD Index Error               */
    uint16_t :2;               /*!< bit:  5.. 6  Reserved                           */
    uint16_t CMDNI:1;          /*!< bit:      7  Command not Issued By Auto CMD12 Error */
    uint16_t :8;               /*!< bit:  8..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_ACESR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_ACESR_OFFSET           0x03C        /**< \brief (SDHC_ACESR offset) Auto CMD Error Status */
#define SDHC_ACESR_RESETVALUE       _U_(0x0000)  /**< \brief (SDHC_ACESR reset_value) Auto CMD Error Status */

#define SDHC_ACESR_ACMD12NE_Pos     0            /**< \brief (SDHC_ACESR) Auto CMD12 Not Executed */
#define SDHC_ACESR_ACMD12NE         (_U_(0x1) << SDHC_ACESR_ACMD12NE_Pos)
#define   SDHC_ACESR_ACMD12NE_EXEC_Val    _U_(0x0)   /**< \brief (SDHC_ACESR) Executed */
#define   SDHC_ACESR_ACMD12NE_NOT_EXEC_Val _U_(0x1)   /**< \brief (SDHC_ACESR) Not executed */
#define SDHC_ACESR_ACMD12NE_EXEC    (SDHC_ACESR_ACMD12NE_EXEC_Val  << SDHC_ACESR_ACMD12NE_Pos)
#define SDHC_ACESR_ACMD12NE_NOT_EXEC (SDHC_ACESR_ACMD12NE_NOT_EXEC_Val << SDHC_ACESR_ACMD12NE_Pos)
#define SDHC_ACESR_ACMDTEO_Pos      1            /**< \brief (SDHC_ACESR) Auto CMD Timeout Error */
#define SDHC_ACESR_ACMDTEO          (_U_(0x1) << SDHC_ACESR_ACMDTEO_Pos)
#define   SDHC_ACESR_ACMDTEO_NO_Val       _U_(0x0)   /**< \brief (SDHC_ACESR) No error */
#define   SDHC_ACESR_ACMDTEO_YES_Val      _U_(0x1)   /**< \brief (SDHC_ACESR) Timeout */
#define SDHC_ACESR_ACMDTEO_NO       (SDHC_ACESR_ACMDTEO_NO_Val     << SDHC_ACESR_ACMDTEO_Pos)
#define SDHC_ACESR_ACMDTEO_YES      (SDHC_ACESR_ACMDTEO_YES_Val    << SDHC_ACESR_ACMDTEO_Pos)
#define SDHC_ACESR_ACMDCRC_Pos      2            /**< \brief (SDHC_ACESR) Auto CMD CRC Error */
#define SDHC_ACESR_ACMDCRC          (_U_(0x1) << SDHC_ACESR_ACMDCRC_Pos)
#define   SDHC_ACESR_ACMDCRC_NO_Val       _U_(0x0)   /**< \brief (SDHC_ACESR) No error */
#define   SDHC_ACESR_ACMDCRC_YES_Val      _U_(0x1)   /**< \brief (SDHC_ACESR) CRC Error Generated */
#define SDHC_ACESR_ACMDCRC_NO       (SDHC_ACESR_ACMDCRC_NO_Val     << SDHC_ACESR_ACMDCRC_Pos)
#define SDHC_ACESR_ACMDCRC_YES      (SDHC_ACESR_ACMDCRC_YES_Val    << SDHC_ACESR_ACMDCRC_Pos)
#define SDHC_ACESR_ACMDEND_Pos      3            /**< \brief (SDHC_ACESR) Auto CMD End Bit Error */
#define SDHC_ACESR_ACMDEND          (_U_(0x1) << SDHC_ACESR_ACMDEND_Pos)
#define   SDHC_ACESR_ACMDEND_NO_Val       _U_(0x0)   /**< \brief (SDHC_ACESR) No error */
#define   SDHC_ACESR_ACMDEND_YES_Val      _U_(0x1)   /**< \brief (SDHC_ACESR) End Bit Error Generated */
#define SDHC_ACESR_ACMDEND_NO       (SDHC_ACESR_ACMDEND_NO_Val     << SDHC_ACESR_ACMDEND_Pos)
#define SDHC_ACESR_ACMDEND_YES      (SDHC_ACESR_ACMDEND_YES_Val    << SDHC_ACESR_ACMDEND_Pos)
#define SDHC_ACESR_ACMDIDX_Pos      4            /**< \brief (SDHC_ACESR) Auto CMD Index Error */
#define SDHC_ACESR_ACMDIDX          (_U_(0x1) << SDHC_ACESR_ACMDIDX_Pos)
#define   SDHC_ACESR_ACMDIDX_NO_Val       _U_(0x0)   /**< \brief (SDHC_ACESR) No error */
#define   SDHC_ACESR_ACMDIDX_YES_Val      _U_(0x1)   /**< \brief (SDHC_ACESR) Error */
#define SDHC_ACESR_ACMDIDX_NO       (SDHC_ACESR_ACMDIDX_NO_Val     << SDHC_ACESR_ACMDIDX_Pos)
#define SDHC_ACESR_ACMDIDX_YES      (SDHC_ACESR_ACMDIDX_YES_Val    << SDHC_ACESR_ACMDIDX_Pos)
#define SDHC_ACESR_CMDNI_Pos        7            /**< \brief (SDHC_ACESR) Command not Issued By Auto CMD12 Error */
#define SDHC_ACESR_CMDNI            (_U_(0x1) << SDHC_ACESR_CMDNI_Pos)
#define   SDHC_ACESR_CMDNI_OK_Val         _U_(0x0)   /**< \brief (SDHC_ACESR) No error */
#define   SDHC_ACESR_CMDNI_NOT_ISSUED_Val _U_(0x1)   /**< \brief (SDHC_ACESR) Not Issued */
#define SDHC_ACESR_CMDNI_OK         (SDHC_ACESR_CMDNI_OK_Val       << SDHC_ACESR_CMDNI_Pos)
#define SDHC_ACESR_CMDNI_NOT_ISSUED (SDHC_ACESR_CMDNI_NOT_ISSUED_Val << SDHC_ACESR_CMDNI_Pos)
#define SDHC_ACESR_MASK             _U_(0x009F)  /**< \brief (SDHC_ACESR) MASK Register */

/* -------- SDHC_HC2R : (SDHC Offset: 0x03E) (R/W 16) Host Control 2 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t UHSMS:3;          /*!< bit:  0.. 2  UHS Mode Select                    */
    uint16_t VS18EN:1;         /*!< bit:      3  1.8V Signaling Enable              */
    uint16_t DRVSEL:2;         /*!< bit:  4.. 5  Driver Strength Select             */
    uint16_t EXTUN:1;          /*!< bit:      6  Execute Tuning                     */
    uint16_t SLCKSEL:1;        /*!< bit:      7  Sampling Clock Select              */
    uint16_t :6;               /*!< bit:  8..13  Reserved                           */
    uint16_t ASINTEN:1;        /*!< bit:     14  Asynchronous Interrupt Enable      */
    uint16_t PVALEN:1;         /*!< bit:     15  Preset Value Enable                */
  } bit;                       /*!< Structure used for bit  access                  */
  struct { // EMMC mode
    uint16_t HS200EN:4;        /*!< bit:  0.. 3  HS200 Mode Enable                  */
    uint16_t DRVSEL:2;         /*!< bit:  4.. 5  Driver Strength Select             */
    uint16_t EXTUN:1;          /*!< bit:      6  Execute Tuning                     */
    uint16_t SLCKSEL:1;        /*!< bit:      7  Sampling Clock Select              */
    uint16_t :7;               /*!< bit:  8..14  Reserved                           */
    uint16_t PVALEN:1;         /*!< bit:     15  Preset Value Enable                */
  } EMMC;                      /*!< Structure used for EMMC                         */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_HC2R_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_HC2R_OFFSET            0x03E        /**< \brief (SDHC_HC2R offset) Host Control 2 */
#define SDHC_HC2R_RESETVALUE        _U_(0x0000)  /**< \brief (SDHC_HC2R reset_value) Host Control 2 */

#define SDHC_HC2R_UHSMS_Pos         0            /**< \brief (SDHC_HC2R) UHS Mode Select */
#define SDHC_HC2R_UHSMS_Msk         (_U_(0x7) << SDHC_HC2R_UHSMS_Pos)
#define SDHC_HC2R_UHSMS(value)      (SDHC_HC2R_UHSMS_Msk & ((value) << SDHC_HC2R_UHSMS_Pos))
#define   SDHC_HC2R_UHSMS_SDR12_Val       _U_(0x0)   /**< \brief (SDHC_HC2R) SDR12 */
#define   SDHC_HC2R_UHSMS_SDR25_Val       _U_(0x1)   /**< \brief (SDHC_HC2R) SDR25 */
#define   SDHC_HC2R_UHSMS_SDR50_Val       _U_(0x2)   /**< \brief (SDHC_HC2R) SDR50 */
#define   SDHC_HC2R_UHSMS_SDR104_Val      _U_(0x3)   /**< \brief (SDHC_HC2R) SDR104 */
#define   SDHC_HC2R_UHSMS_DDR50_Val       _U_(0x4)   /**< \brief (SDHC_HC2R) DDR50 */
#define SDHC_HC2R_UHSMS_SDR12       (SDHC_HC2R_UHSMS_SDR12_Val     << SDHC_HC2R_UHSMS_Pos)
#define SDHC_HC2R_UHSMS_SDR25       (SDHC_HC2R_UHSMS_SDR25_Val     << SDHC_HC2R_UHSMS_Pos)
#define SDHC_HC2R_UHSMS_SDR50       (SDHC_HC2R_UHSMS_SDR50_Val     << SDHC_HC2R_UHSMS_Pos)
#define SDHC_HC2R_UHSMS_SDR104      (SDHC_HC2R_UHSMS_SDR104_Val    << SDHC_HC2R_UHSMS_Pos)
#define SDHC_HC2R_UHSMS_DDR50       (SDHC_HC2R_UHSMS_DDR50_Val     << SDHC_HC2R_UHSMS_Pos)
#define SDHC_HC2R_VS18EN_Pos        3            /**< \brief (SDHC_HC2R) 1.8V Signaling Enable */
#define SDHC_HC2R_VS18EN            (_U_(0x1) << SDHC_HC2R_VS18EN_Pos)
#define   SDHC_HC2R_VS18EN_S33V_Val       _U_(0x0)   /**< \brief (SDHC_HC2R) 3.3V Signaling */
#define   SDHC_HC2R_VS18EN_S18V_Val       _U_(0x1)   /**< \brief (SDHC_HC2R) 1.8V Signaling */
#define SDHC_HC2R_VS18EN_S33V       (SDHC_HC2R_VS18EN_S33V_Val     << SDHC_HC2R_VS18EN_Pos)
#define SDHC_HC2R_VS18EN_S18V       (SDHC_HC2R_VS18EN_S18V_Val     << SDHC_HC2R_VS18EN_Pos)
#define SDHC_HC2R_DRVSEL_Pos        4            /**< \brief (SDHC_HC2R) Driver Strength Select */
#define SDHC_HC2R_DRVSEL_Msk        (_U_(0x3) << SDHC_HC2R_DRVSEL_Pos)
#define SDHC_HC2R_DRVSEL(value)     (SDHC_HC2R_DRVSEL_Msk & ((value) << SDHC_HC2R_DRVSEL_Pos))
#define   SDHC_HC2R_DRVSEL_B_Val          _U_(0x0)   /**< \brief (SDHC_HC2R) Driver Type B is Selected (Default) */
#define   SDHC_HC2R_DRVSEL_A_Val          _U_(0x1)   /**< \brief (SDHC_HC2R) Driver Type A is Selected */
#define   SDHC_HC2R_DRVSEL_C_Val          _U_(0x2)   /**< \brief (SDHC_HC2R) Driver Type C is Selected */
#define   SDHC_HC2R_DRVSEL_D_Val          _U_(0x3)   /**< \brief (SDHC_HC2R) Driver Type D is Selected */
#define SDHC_HC2R_DRVSEL_B          (SDHC_HC2R_DRVSEL_B_Val        << SDHC_HC2R_DRVSEL_Pos)
#define SDHC_HC2R_DRVSEL_A          (SDHC_HC2R_DRVSEL_A_Val        << SDHC_HC2R_DRVSEL_Pos)
#define SDHC_HC2R_DRVSEL_C          (SDHC_HC2R_DRVSEL_C_Val        << SDHC_HC2R_DRVSEL_Pos)
#define SDHC_HC2R_DRVSEL_D          (SDHC_HC2R_DRVSEL_D_Val        << SDHC_HC2R_DRVSEL_Pos)
#define SDHC_HC2R_EXTUN_Pos         6            /**< \brief (SDHC_HC2R) Execute Tuning */
#define SDHC_HC2R_EXTUN             (_U_(0x1) << SDHC_HC2R_EXTUN_Pos)
#define   SDHC_HC2R_EXTUN_NO_Val          _U_(0x0)   /**< \brief (SDHC_HC2R) Not Tuned or Tuning Completed */
#define   SDHC_HC2R_EXTUN_REQUESTED_Val   _U_(0x1)   /**< \brief (SDHC_HC2R) Execute Tuning */
#define SDHC_HC2R_EXTUN_NO          (SDHC_HC2R_EXTUN_NO_Val        << SDHC_HC2R_EXTUN_Pos)
#define SDHC_HC2R_EXTUN_REQUESTED   (SDHC_HC2R_EXTUN_REQUESTED_Val << SDHC_HC2R_EXTUN_Pos)
#define SDHC_HC2R_SLCKSEL_Pos       7            /**< \brief (SDHC_HC2R) Sampling Clock Select */
#define SDHC_HC2R_SLCKSEL           (_U_(0x1) << SDHC_HC2R_SLCKSEL_Pos)
#define   SDHC_HC2R_SLCKSEL_FIXED_Val     _U_(0x0)   /**< \brief (SDHC_HC2R) Fixed clock is used to sample data */
#define   SDHC_HC2R_SLCKSEL_TUNED_Val     _U_(0x1)   /**< \brief (SDHC_HC2R) Tuned clock is used to sample data */
#define SDHC_HC2R_SLCKSEL_FIXED     (SDHC_HC2R_SLCKSEL_FIXED_Val   << SDHC_HC2R_SLCKSEL_Pos)
#define SDHC_HC2R_SLCKSEL_TUNED     (SDHC_HC2R_SLCKSEL_TUNED_Val   << SDHC_HC2R_SLCKSEL_Pos)
#define SDHC_HC2R_ASINTEN_Pos       14           /**< \brief (SDHC_HC2R) Asynchronous Interrupt Enable */
#define SDHC_HC2R_ASINTEN           (_U_(0x1) << SDHC_HC2R_ASINTEN_Pos)
#define   SDHC_HC2R_ASINTEN_DISABLED_Val  _U_(0x0)   /**< \brief (SDHC_HC2R) Disabled */
#define   SDHC_HC2R_ASINTEN_ENABLED_Val   _U_(0x1)   /**< \brief (SDHC_HC2R) Enabled */
#define SDHC_HC2R_ASINTEN_DISABLED  (SDHC_HC2R_ASINTEN_DISABLED_Val << SDHC_HC2R_ASINTEN_Pos)
#define SDHC_HC2R_ASINTEN_ENABLED   (SDHC_HC2R_ASINTEN_ENABLED_Val << SDHC_HC2R_ASINTEN_Pos)
#define SDHC_HC2R_PVALEN_Pos        15           /**< \brief (SDHC_HC2R) Preset Value Enable */
#define SDHC_HC2R_PVALEN            (_U_(0x1) << SDHC_HC2R_PVALEN_Pos)
#define   SDHC_HC2R_PVALEN_HOST_Val       _U_(0x0)   /**< \brief (SDHC_HC2R) SDCLK and Driver Strength are controlled by Host Controller */
#define   SDHC_HC2R_PVALEN_AUTO_Val       _U_(0x1)   /**< \brief (SDHC_HC2R) Automatic Selection by Preset Value is Enabled */
#define SDHC_HC2R_PVALEN_HOST       (SDHC_HC2R_PVALEN_HOST_Val     << SDHC_HC2R_PVALEN_Pos)
#define SDHC_HC2R_PVALEN_AUTO       (SDHC_HC2R_PVALEN_AUTO_Val     << SDHC_HC2R_PVALEN_Pos)
#define SDHC_HC2R_MASK              _U_(0xC0FF)  /**< \brief (SDHC_HC2R) MASK Register */

// EMMC mode
#define SDHC_HC2R_EMMC_HS200EN_Pos  0            /**< \brief (SDHC_HC2R_EMMC) HS200 Mode Enable */
#define SDHC_HC2R_EMMC_HS200EN_Msk  (_U_(0xF) << SDHC_HC2R_EMMC_HS200EN_Pos)
#define SDHC_HC2R_EMMC_HS200EN(value) (SDHC_HC2R_EMMC_HS200EN_Msk & ((value) << SDHC_HC2R_EMMC_HS200EN_Pos))
#define   SDHC_HC2R_EMMC_HS200EN_SDR12_Val _U_(0x0)   /**< \brief (SDHC_HC2R_EMMC) SDR12 */
#define   SDHC_HC2R_EMMC_HS200EN_SDR25_Val _U_(0x1)   /**< \brief (SDHC_HC2R_EMMC) SDR25 */
#define   SDHC_HC2R_EMMC_HS200EN_SDR50_Val _U_(0x2)   /**< \brief (SDHC_HC2R_EMMC) SDR50 */
#define   SDHC_HC2R_EMMC_HS200EN_SDR104_Val _U_(0x3)   /**< \brief (SDHC_HC2R_EMMC) SDR104 */
#define   SDHC_HC2R_EMMC_HS200EN_DDR50_Val _U_(0x4)   /**< \brief (SDHC_HC2R_EMMC) DDR50 */
#define SDHC_HC2R_EMMC_HS200EN_SDR12 (SDHC_HC2R_EMMC_HS200EN_SDR12_Val << SDHC_HC2R_EMMC_HS200EN_Pos)
#define SDHC_HC2R_EMMC_HS200EN_SDR25 (SDHC_HC2R_EMMC_HS200EN_SDR25_Val << SDHC_HC2R_EMMC_HS200EN_Pos)
#define SDHC_HC2R_EMMC_HS200EN_SDR50 (SDHC_HC2R_EMMC_HS200EN_SDR50_Val << SDHC_HC2R_EMMC_HS200EN_Pos)
#define SDHC_HC2R_EMMC_HS200EN_SDR104 (SDHC_HC2R_EMMC_HS200EN_SDR104_Val << SDHC_HC2R_EMMC_HS200EN_Pos)
#define SDHC_HC2R_EMMC_HS200EN_DDR50 (SDHC_HC2R_EMMC_HS200EN_DDR50_Val << SDHC_HC2R_EMMC_HS200EN_Pos)
#define SDHC_HC2R_EMMC_DRVSEL_Pos   4            /**< \brief (SDHC_HC2R_EMMC) Driver Strength Select */
#define SDHC_HC2R_EMMC_DRVSEL_Msk   (_U_(0x3) << SDHC_HC2R_EMMC_DRVSEL_Pos)
#define SDHC_HC2R_EMMC_DRVSEL(value) (SDHC_HC2R_EMMC_DRVSEL_Msk & ((value) << SDHC_HC2R_EMMC_DRVSEL_Pos))
#define   SDHC_HC2R_EMMC_DRVSEL_B_Val     _U_(0x0)   /**< \brief (SDHC_HC2R_EMMC) Driver Type B is Selected (Default) */
#define   SDHC_HC2R_EMMC_DRVSEL_A_Val     _U_(0x1)   /**< \brief (SDHC_HC2R_EMMC) Driver Type A is Selected */
#define   SDHC_HC2R_EMMC_DRVSEL_C_Val     _U_(0x2)   /**< \brief (SDHC_HC2R_EMMC) Driver Type C is Selected */
#define   SDHC_HC2R_EMMC_DRVSEL_D_Val     _U_(0x3)   /**< \brief (SDHC_HC2R_EMMC) Driver Type D is Selected */
#define SDHC_HC2R_EMMC_DRVSEL_B     (SDHC_HC2R_EMMC_DRVSEL_B_Val   << SDHC_HC2R_EMMC_DRVSEL_Pos)
#define SDHC_HC2R_EMMC_DRVSEL_A     (SDHC_HC2R_EMMC_DRVSEL_A_Val   << SDHC_HC2R_EMMC_DRVSEL_Pos)
#define SDHC_HC2R_EMMC_DRVSEL_C     (SDHC_HC2R_EMMC_DRVSEL_C_Val   << SDHC_HC2R_EMMC_DRVSEL_Pos)
#define SDHC_HC2R_EMMC_DRVSEL_D     (SDHC_HC2R_EMMC_DRVSEL_D_Val   << SDHC_HC2R_EMMC_DRVSEL_Pos)
#define SDHC_HC2R_EMMC_EXTUN_Pos    6            /**< \brief (SDHC_HC2R_EMMC) Execute Tuning */
#define SDHC_HC2R_EMMC_EXTUN        (_U_(0x1) << SDHC_HC2R_EMMC_EXTUN_Pos)
#define   SDHC_HC2R_EMMC_EXTUN_NO_Val     _U_(0x0)   /**< \brief (SDHC_HC2R_EMMC) Not Tuned or Tuning Completed */
#define   SDHC_HC2R_EMMC_EXTUN_REQUESTED_Val _U_(0x1)   /**< \brief (SDHC_HC2R_EMMC) Execute Tuning */
#define SDHC_HC2R_EMMC_EXTUN_NO     (SDHC_HC2R_EMMC_EXTUN_NO_Val   << SDHC_HC2R_EMMC_EXTUN_Pos)
#define SDHC_HC2R_EMMC_EXTUN_REQUESTED (SDHC_HC2R_EMMC_EXTUN_REQUESTED_Val << SDHC_HC2R_EMMC_EXTUN_Pos)
#define SDHC_HC2R_EMMC_SLCKSEL_Pos  7            /**< \brief (SDHC_HC2R_EMMC) Sampling Clock Select */
#define SDHC_HC2R_EMMC_SLCKSEL      (_U_(0x1) << SDHC_HC2R_EMMC_SLCKSEL_Pos)
#define   SDHC_HC2R_EMMC_SLCKSEL_FIXED_Val _U_(0x0)   /**< \brief (SDHC_HC2R_EMMC) Fixed clock is used to sample data */
#define   SDHC_HC2R_EMMC_SLCKSEL_TUNED_Val _U_(0x1)   /**< \brief (SDHC_HC2R_EMMC) Tuned clock is used to sample data */
#define SDHC_HC2R_EMMC_SLCKSEL_FIXED (SDHC_HC2R_EMMC_SLCKSEL_FIXED_Val << SDHC_HC2R_EMMC_SLCKSEL_Pos)
#define SDHC_HC2R_EMMC_SLCKSEL_TUNED (SDHC_HC2R_EMMC_SLCKSEL_TUNED_Val << SDHC_HC2R_EMMC_SLCKSEL_Pos)
#define SDHC_HC2R_EMMC_PVALEN_Pos   15           /**< \brief (SDHC_HC2R_EMMC) Preset Value Enable */
#define SDHC_HC2R_EMMC_PVALEN       (_U_(0x1) << SDHC_HC2R_EMMC_PVALEN_Pos)
#define   SDHC_HC2R_EMMC_PVALEN_HOST_Val  _U_(0x0)   /**< \brief (SDHC_HC2R_EMMC) SDCLK and Driver Strength are controlled by Host Controller */
#define   SDHC_HC2R_EMMC_PVALEN_AUTO_Val  _U_(0x1)   /**< \brief (SDHC_HC2R_EMMC) Automatic Selection by Preset Value is Enabled */
#define SDHC_HC2R_EMMC_PVALEN_HOST  (SDHC_HC2R_EMMC_PVALEN_HOST_Val << SDHC_HC2R_EMMC_PVALEN_Pos)
#define SDHC_HC2R_EMMC_PVALEN_AUTO  (SDHC_HC2R_EMMC_PVALEN_AUTO_Val << SDHC_HC2R_EMMC_PVALEN_Pos)
#define SDHC_HC2R_EMMC_MASK         _U_(0x80FF)  /**< \brief (SDHC_HC2R_EMMC) MASK Register */

/* -------- SDHC_CA0R : (SDHC Offset: 0x040) (R/  32) Capabilities 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t TEOCLKF:6;        /*!< bit:  0.. 5  Timeout Clock Frequency            */
    uint32_t :1;               /*!< bit:      6  Reserved                           */
    uint32_t TEOCLKU:1;        /*!< bit:      7  Timeout Clock Unit                 */
    uint32_t BASECLKF:8;       /*!< bit:  8..15  Base Clock Frequency               */
    uint32_t MAXBLKL:2;        /*!< bit: 16..17  Max Block Length                   */
    uint32_t ED8SUP:1;         /*!< bit:     18  8-bit Support for Embedded Device  */
    uint32_t ADMA2SUP:1;       /*!< bit:     19  ADMA2 Support                      */
    uint32_t :1;               /*!< bit:     20  Reserved                           */
    uint32_t HSSUP:1;          /*!< bit:     21  High Speed Support                 */
    uint32_t SDMASUP:1;        /*!< bit:     22  SDMA Support                       */
    uint32_t SRSUP:1;          /*!< bit:     23  Suspend/Resume Support             */
    uint32_t V33VSUP:1;        /*!< bit:     24  Voltage Support 3.3V               */
    uint32_t V30VSUP:1;        /*!< bit:     25  Voltage Support 3.0V               */
    uint32_t V18VSUP:1;        /*!< bit:     26  Voltage Support 1.8V               */
    uint32_t :1;               /*!< bit:     27  Reserved                           */
    uint32_t SB64SUP:1;        /*!< bit:     28  64-Bit System Bus Support          */
    uint32_t ASINTSUP:1;       /*!< bit:     29  Asynchronous Interrupt Support     */
    uint32_t SLTYPE:2;         /*!< bit: 30..31  Slot Type                          */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SDHC_CA0R_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_CA0R_OFFSET            0x040        /**< \brief (SDHC_CA0R offset) Capabilities 0 */
#define SDHC_CA0R_RESETVALUE        _U_(0x27E80080) /**< \brief (SDHC_CA0R reset_value) Capabilities 0 */

#define SDHC_CA0R_TEOCLKF_Pos       0            /**< \brief (SDHC_CA0R) Timeout Clock Frequency */
#define SDHC_CA0R_TEOCLKF_Msk       (_U_(0x3F) << SDHC_CA0R_TEOCLKF_Pos)
#define SDHC_CA0R_TEOCLKF(value)    (SDHC_CA0R_TEOCLKF_Msk & ((value) << SDHC_CA0R_TEOCLKF_Pos))
#define   SDHC_CA0R_TEOCLKF_OTHER_Val     _U_(0x0)   /**< \brief (SDHC_CA0R) Get information via another method */
#define SDHC_CA0R_TEOCLKF_OTHER     (SDHC_CA0R_TEOCLKF_OTHER_Val   << SDHC_CA0R_TEOCLKF_Pos)
#define SDHC_CA0R_TEOCLKU_Pos       7            /**< \brief (SDHC_CA0R) Timeout Clock Unit */
#define SDHC_CA0R_TEOCLKU           (_U_(0x1) << SDHC_CA0R_TEOCLKU_Pos)
#define   SDHC_CA0R_TEOCLKU_KHZ_Val       _U_(0x0)   /**< \brief (SDHC_CA0R) kHz */
#define   SDHC_CA0R_TEOCLKU_MHZ_Val       _U_(0x1)   /**< \brief (SDHC_CA0R) MHz */
#define SDHC_CA0R_TEOCLKU_KHZ       (SDHC_CA0R_TEOCLKU_KHZ_Val     << SDHC_CA0R_TEOCLKU_Pos)
#define SDHC_CA0R_TEOCLKU_MHZ       (SDHC_CA0R_TEOCLKU_MHZ_Val     << SDHC_CA0R_TEOCLKU_Pos)
#define SDHC_CA0R_BASECLKF_Pos      8            /**< \brief (SDHC_CA0R) Base Clock Frequency */
#define SDHC_CA0R_BASECLKF_Msk      (_U_(0xFF) << SDHC_CA0R_BASECLKF_Pos)
#define SDHC_CA0R_BASECLKF(value)   (SDHC_CA0R_BASECLKF_Msk & ((value) << SDHC_CA0R_BASECLKF_Pos))
#define   SDHC_CA0R_BASECLKF_OTHER_Val    _U_(0x0)   /**< \brief (SDHC_CA0R) Get information via another method */
#define SDHC_CA0R_BASECLKF_OTHER    (SDHC_CA0R_BASECLKF_OTHER_Val  << SDHC_CA0R_BASECLKF_Pos)
#define SDHC_CA0R_MAXBLKL_Pos       16           /**< \brief (SDHC_CA0R) Max Block Length */
#define SDHC_CA0R_MAXBLKL_Msk       (_U_(0x3) << SDHC_CA0R_MAXBLKL_Pos)
#define SDHC_CA0R_MAXBLKL(value)    (SDHC_CA0R_MAXBLKL_Msk & ((value) << SDHC_CA0R_MAXBLKL_Pos))
#define   SDHC_CA0R_MAXBLKL_512_Val       _U_(0x0)   /**< \brief (SDHC_CA0R) 512 bytes */
#define   SDHC_CA0R_MAXBLKL_1024_Val      _U_(0x1)   /**< \brief (SDHC_CA0R) 1024 bytes */
#define   SDHC_CA0R_MAXBLKL_2048_Val      _U_(0x2)   /**< \brief (SDHC_CA0R) 2048 bytes */
#define SDHC_CA0R_MAXBLKL_512       (SDHC_CA0R_MAXBLKL_512_Val     << SDHC_CA0R_MAXBLKL_Pos)
#define SDHC_CA0R_MAXBLKL_1024      (SDHC_CA0R_MAXBLKL_1024_Val    << SDHC_CA0R_MAXBLKL_Pos)
#define SDHC_CA0R_MAXBLKL_2048      (SDHC_CA0R_MAXBLKL_2048_Val    << SDHC_CA0R_MAXBLKL_Pos)
#define SDHC_CA0R_ED8SUP_Pos        18           /**< \brief (SDHC_CA0R) 8-bit Support for Embedded Device */
#define SDHC_CA0R_ED8SUP            (_U_(0x1) << SDHC_CA0R_ED8SUP_Pos)
#define   SDHC_CA0R_ED8SUP_NO_Val         _U_(0x0)   /**< \brief (SDHC_CA0R) 8-bit Bus Width not Supported */
#define   SDHC_CA0R_ED8SUP_YES_Val        _U_(0x1)   /**< \brief (SDHC_CA0R) 8-bit Bus Width Supported */
#define SDHC_CA0R_ED8SUP_NO         (SDHC_CA0R_ED8SUP_NO_Val       << SDHC_CA0R_ED8SUP_Pos)
#define SDHC_CA0R_ED8SUP_YES        (SDHC_CA0R_ED8SUP_YES_Val      << SDHC_CA0R_ED8SUP_Pos)
#define SDHC_CA0R_ADMA2SUP_Pos      19           /**< \brief (SDHC_CA0R) ADMA2 Support */
#define SDHC_CA0R_ADMA2SUP          (_U_(0x1) << SDHC_CA0R_ADMA2SUP_Pos)
#define   SDHC_CA0R_ADMA2SUP_NO_Val       _U_(0x0)   /**< \brief (SDHC_CA0R) ADMA2 not Supported */
#define   SDHC_CA0R_ADMA2SUP_YES_Val      _U_(0x1)   /**< \brief (SDHC_CA0R) ADMA2 Supported */
#define SDHC_CA0R_ADMA2SUP_NO       (SDHC_CA0R_ADMA2SUP_NO_Val     << SDHC_CA0R_ADMA2SUP_Pos)
#define SDHC_CA0R_ADMA2SUP_YES      (SDHC_CA0R_ADMA2SUP_YES_Val    << SDHC_CA0R_ADMA2SUP_Pos)
#define SDHC_CA0R_HSSUP_Pos         21           /**< \brief (SDHC_CA0R) High Speed Support */
#define SDHC_CA0R_HSSUP             (_U_(0x1) << SDHC_CA0R_HSSUP_Pos)
#define   SDHC_CA0R_HSSUP_NO_Val          _U_(0x0)   /**< \brief (SDHC_CA0R) High Speed not Supported */
#define   SDHC_CA0R_HSSUP_YES_Val         _U_(0x1)   /**< \brief (SDHC_CA0R) High Speed Supported */
#define SDHC_CA0R_HSSUP_NO          (SDHC_CA0R_HSSUP_NO_Val        << SDHC_CA0R_HSSUP_Pos)
#define SDHC_CA0R_HSSUP_YES         (SDHC_CA0R_HSSUP_YES_Val       << SDHC_CA0R_HSSUP_Pos)
#define SDHC_CA0R_SDMASUP_Pos       22           /**< \brief (SDHC_CA0R) SDMA Support */
#define SDHC_CA0R_SDMASUP           (_U_(0x1) << SDHC_CA0R_SDMASUP_Pos)
#define   SDHC_CA0R_SDMASUP_NO_Val        _U_(0x0)   /**< \brief (SDHC_CA0R) SDMA not Supported */
#define   SDHC_CA0R_SDMASUP_YES_Val       _U_(0x1)   /**< \brief (SDHC_CA0R) SDMA Supported */
#define SDHC_CA0R_SDMASUP_NO        (SDHC_CA0R_SDMASUP_NO_Val      << SDHC_CA0R_SDMASUP_Pos)
#define SDHC_CA0R_SDMASUP_YES       (SDHC_CA0R_SDMASUP_YES_Val     << SDHC_CA0R_SDMASUP_Pos)
#define SDHC_CA0R_SRSUP_Pos         23           /**< \brief (SDHC_CA0R) Suspend/Resume Support */
#define SDHC_CA0R_SRSUP             (_U_(0x1) << SDHC_CA0R_SRSUP_Pos)
#define   SDHC_CA0R_SRSUP_NO_Val          _U_(0x0)   /**< \brief (SDHC_CA0R) Suspend/Resume not Supported */
#define   SDHC_CA0R_SRSUP_YES_Val         _U_(0x1)   /**< \brief (SDHC_CA0R) Suspend/Resume Supported */
#define SDHC_CA0R_SRSUP_NO          (SDHC_CA0R_SRSUP_NO_Val        << SDHC_CA0R_SRSUP_Pos)
#define SDHC_CA0R_SRSUP_YES         (SDHC_CA0R_SRSUP_YES_Val       << SDHC_CA0R_SRSUP_Pos)
#define SDHC_CA0R_V33VSUP_Pos       24           /**< \brief (SDHC_CA0R) Voltage Support 3.3V */
#define SDHC_CA0R_V33VSUP           (_U_(0x1) << SDHC_CA0R_V33VSUP_Pos)
#define   SDHC_CA0R_V33VSUP_NO_Val        _U_(0x0)   /**< \brief (SDHC_CA0R) 3.3V Not Supported */
#define   SDHC_CA0R_V33VSUP_YES_Val       _U_(0x1)   /**< \brief (SDHC_CA0R) 3.3V Supported */
#define SDHC_CA0R_V33VSUP_NO        (SDHC_CA0R_V33VSUP_NO_Val      << SDHC_CA0R_V33VSUP_Pos)
#define SDHC_CA0R_V33VSUP_YES       (SDHC_CA0R_V33VSUP_YES_Val     << SDHC_CA0R_V33VSUP_Pos)
#define SDHC_CA0R_V30VSUP_Pos       25           /**< \brief (SDHC_CA0R) Voltage Support 3.0V */
#define SDHC_CA0R_V30VSUP           (_U_(0x1) << SDHC_CA0R_V30VSUP_Pos)
#define   SDHC_CA0R_V30VSUP_NO_Val        _U_(0x0)   /**< \brief (SDHC_CA0R) 3.0V Not Supported */
#define   SDHC_CA0R_V30VSUP_YES_Val       _U_(0x1)   /**< \brief (SDHC_CA0R) 3.0V Supported */
#define SDHC_CA0R_V30VSUP_NO        (SDHC_CA0R_V30VSUP_NO_Val      << SDHC_CA0R_V30VSUP_Pos)
#define SDHC_CA0R_V30VSUP_YES       (SDHC_CA0R_V30VSUP_YES_Val     << SDHC_CA0R_V30VSUP_Pos)
#define SDHC_CA0R_V18VSUP_Pos       26           /**< \brief (SDHC_CA0R) Voltage Support 1.8V */
#define SDHC_CA0R_V18VSUP           (_U_(0x1) << SDHC_CA0R_V18VSUP_Pos)
#define   SDHC_CA0R_V18VSUP_NO_Val        _U_(0x0)   /**< \brief (SDHC_CA0R) 1.8V Not Supported */
#define   SDHC_CA0R_V18VSUP_YES_Val       _U_(0x1)   /**< \brief (SDHC_CA0R) 1.8V Supported */
#define SDHC_CA0R_V18VSUP_NO        (SDHC_CA0R_V18VSUP_NO_Val      << SDHC_CA0R_V18VSUP_Pos)
#define SDHC_CA0R_V18VSUP_YES       (SDHC_CA0R_V18VSUP_YES_Val     << SDHC_CA0R_V18VSUP_Pos)
#define SDHC_CA0R_SB64SUP_Pos       28           /**< \brief (SDHC_CA0R) 64-Bit System Bus Support */
#define SDHC_CA0R_SB64SUP           (_U_(0x1) << SDHC_CA0R_SB64SUP_Pos)
#define   SDHC_CA0R_SB64SUP_NO_Val        _U_(0x0)   /**< \brief (SDHC_CA0R) 32-bit Address Descriptors and System Bus */
#define   SDHC_CA0R_SB64SUP_YES_Val       _U_(0x1)   /**< \brief (SDHC_CA0R) 64-bit Address Descriptors and System Bus */
#define SDHC_CA0R_SB64SUP_NO        (SDHC_CA0R_SB64SUP_NO_Val      << SDHC_CA0R_SB64SUP_Pos)
#define SDHC_CA0R_SB64SUP_YES       (SDHC_CA0R_SB64SUP_YES_Val     << SDHC_CA0R_SB64SUP_Pos)
#define SDHC_CA0R_ASINTSUP_Pos      29           /**< \brief (SDHC_CA0R) Asynchronous Interrupt Support */
#define SDHC_CA0R_ASINTSUP          (_U_(0x1) << SDHC_CA0R_ASINTSUP_Pos)
#define   SDHC_CA0R_ASINTSUP_NO_Val       _U_(0x0)   /**< \brief (SDHC_CA0R) Asynchronous Interrupt not Supported */
#define   SDHC_CA0R_ASINTSUP_YES_Val      _U_(0x1)   /**< \brief (SDHC_CA0R) Asynchronous Interrupt supported */
#define SDHC_CA0R_ASINTSUP_NO       (SDHC_CA0R_ASINTSUP_NO_Val     << SDHC_CA0R_ASINTSUP_Pos)
#define SDHC_CA0R_ASINTSUP_YES      (SDHC_CA0R_ASINTSUP_YES_Val    << SDHC_CA0R_ASINTSUP_Pos)
#define SDHC_CA0R_SLTYPE_Pos        30           /**< \brief (SDHC_CA0R) Slot Type */
#define SDHC_CA0R_SLTYPE_Msk        (_U_(0x3) << SDHC_CA0R_SLTYPE_Pos)
#define SDHC_CA0R_SLTYPE(value)     (SDHC_CA0R_SLTYPE_Msk & ((value) << SDHC_CA0R_SLTYPE_Pos))
#define   SDHC_CA0R_SLTYPE_REMOVABLE_Val  _U_(0x0)   /**< \brief (SDHC_CA0R) Removable Card Slot */
#define   SDHC_CA0R_SLTYPE_EMBEDDED_Val   _U_(0x1)   /**< \brief (SDHC_CA0R) Embedded Slot for One Device */
#define SDHC_CA0R_SLTYPE_REMOVABLE  (SDHC_CA0R_SLTYPE_REMOVABLE_Val << SDHC_CA0R_SLTYPE_Pos)
#define SDHC_CA0R_SLTYPE_EMBEDDED   (SDHC_CA0R_SLTYPE_EMBEDDED_Val << SDHC_CA0R_SLTYPE_Pos)
#define SDHC_CA0R_MASK              _U_(0xF7EFFFBF) /**< \brief (SDHC_CA0R) MASK Register */

/* -------- SDHC_CA1R : (SDHC Offset: 0x044) (R/  32) Capabilities 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SDR50SUP:1;       /*!< bit:      0  SDR50 Support                      */
    uint32_t SDR104SUP:1;      /*!< bit:      1  SDR104 Support                     */
    uint32_t DDR50SUP:1;       /*!< bit:      2  DDR50 Support                      */
    uint32_t :1;               /*!< bit:      3  Reserved                           */
    uint32_t DRVASUP:1;        /*!< bit:      4  Driver Type A Support              */
    uint32_t DRVCSUP:1;        /*!< bit:      5  Driver Type C Support              */
    uint32_t DRVDSUP:1;        /*!< bit:      6  Driver Type D Support              */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t TCNTRT:4;         /*!< bit:  8..11  Timer Count for Re-Tuning          */
    uint32_t :1;               /*!< bit:     12  Reserved                           */
    uint32_t TSDR50:1;         /*!< bit:     13  Use Tuning for SDR50               */
    uint32_t :2;               /*!< bit: 14..15  Reserved                           */
    uint32_t CLKMULT:8;        /*!< bit: 16..23  Clock Multiplier                   */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SDHC_CA1R_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_CA1R_OFFSET            0x044        /**< \brief (SDHC_CA1R offset) Capabilities 1 */
#define SDHC_CA1R_RESETVALUE        _U_(0x00000070) /**< \brief (SDHC_CA1R reset_value) Capabilities 1 */

#define SDHC_CA1R_SDR50SUP_Pos      0            /**< \brief (SDHC_CA1R) SDR50 Support */
#define SDHC_CA1R_SDR50SUP          (_U_(0x1) << SDHC_CA1R_SDR50SUP_Pos)
#define   SDHC_CA1R_SDR50SUP_NO_Val       _U_(0x0)   /**< \brief (SDHC_CA1R) SDR50 is Not Supported */
#define   SDHC_CA1R_SDR50SUP_YES_Val      _U_(0x1)   /**< \brief (SDHC_CA1R) SDR50 is Supported */
#define SDHC_CA1R_SDR50SUP_NO       (SDHC_CA1R_SDR50SUP_NO_Val     << SDHC_CA1R_SDR50SUP_Pos)
#define SDHC_CA1R_SDR50SUP_YES      (SDHC_CA1R_SDR50SUP_YES_Val    << SDHC_CA1R_SDR50SUP_Pos)
#define SDHC_CA1R_SDR104SUP_Pos     1            /**< \brief (SDHC_CA1R) SDR104 Support */
#define SDHC_CA1R_SDR104SUP         (_U_(0x1) << SDHC_CA1R_SDR104SUP_Pos)
#define   SDHC_CA1R_SDR104SUP_NO_Val      _U_(0x0)   /**< \brief (SDHC_CA1R) SDR104 is Not Supported */
#define   SDHC_CA1R_SDR104SUP_YES_Val     _U_(0x1)   /**< \brief (SDHC_CA1R) SDR104 is Supported */
#define SDHC_CA1R_SDR104SUP_NO      (SDHC_CA1R_SDR104SUP_NO_Val    << SDHC_CA1R_SDR104SUP_Pos)
#define SDHC_CA1R_SDR104SUP_YES     (SDHC_CA1R_SDR104SUP_YES_Val   << SDHC_CA1R_SDR104SUP_Pos)
#define SDHC_CA1R_DDR50SUP_Pos      2            /**< \brief (SDHC_CA1R) DDR50 Support */
#define SDHC_CA1R_DDR50SUP          (_U_(0x1) << SDHC_CA1R_DDR50SUP_Pos)
#define   SDHC_CA1R_DDR50SUP_NO_Val       _U_(0x0)   /**< \brief (SDHC_CA1R) DDR50 is Not Supported */
#define   SDHC_CA1R_DDR50SUP_YES_Val      _U_(0x1)   /**< \brief (SDHC_CA1R) DDR50 is Supported */
#define SDHC_CA1R_DDR50SUP_NO       (SDHC_CA1R_DDR50SUP_NO_Val     << SDHC_CA1R_DDR50SUP_Pos)
#define SDHC_CA1R_DDR50SUP_YES      (SDHC_CA1R_DDR50SUP_YES_Val    << SDHC_CA1R_DDR50SUP_Pos)
#define SDHC_CA1R_DRVASUP_Pos       4            /**< \brief (SDHC_CA1R) Driver Type A Support */
#define SDHC_CA1R_DRVASUP           (_U_(0x1) << SDHC_CA1R_DRVASUP_Pos)
#define   SDHC_CA1R_DRVASUP_NO_Val        _U_(0x0)   /**< \brief (SDHC_CA1R) Driver Type A is Not Supported */
#define   SDHC_CA1R_DRVASUP_YES_Val       _U_(0x1)   /**< \brief (SDHC_CA1R) Driver Type A is Supported */
#define SDHC_CA1R_DRVASUP_NO        (SDHC_CA1R_DRVASUP_NO_Val      << SDHC_CA1R_DRVASUP_Pos)
#define SDHC_CA1R_DRVASUP_YES       (SDHC_CA1R_DRVASUP_YES_Val     << SDHC_CA1R_DRVASUP_Pos)
#define SDHC_CA1R_DRVCSUP_Pos       5            /**< \brief (SDHC_CA1R) Driver Type C Support */
#define SDHC_CA1R_DRVCSUP           (_U_(0x1) << SDHC_CA1R_DRVCSUP_Pos)
#define   SDHC_CA1R_DRVCSUP_NO_Val        _U_(0x0)   /**< \brief (SDHC_CA1R) Driver Type C is Not Supported */
#define   SDHC_CA1R_DRVCSUP_YES_Val       _U_(0x1)   /**< \brief (SDHC_CA1R) Driver Type C is Supported */
#define SDHC_CA1R_DRVCSUP_NO        (SDHC_CA1R_DRVCSUP_NO_Val      << SDHC_CA1R_DRVCSUP_Pos)
#define SDHC_CA1R_DRVCSUP_YES       (SDHC_CA1R_DRVCSUP_YES_Val     << SDHC_CA1R_DRVCSUP_Pos)
#define SDHC_CA1R_DRVDSUP_Pos       6            /**< \brief (SDHC_CA1R) Driver Type D Support */
#define SDHC_CA1R_DRVDSUP           (_U_(0x1) << SDHC_CA1R_DRVDSUP_Pos)
#define   SDHC_CA1R_DRVDSUP_NO_Val        _U_(0x0)   /**< \brief (SDHC_CA1R) Driver Type D is Not Supported */
#define   SDHC_CA1R_DRVDSUP_YES_Val       _U_(0x1)   /**< \brief (SDHC_CA1R) Driver Type D is Supported */
#define SDHC_CA1R_DRVDSUP_NO        (SDHC_CA1R_DRVDSUP_NO_Val      << SDHC_CA1R_DRVDSUP_Pos)
#define SDHC_CA1R_DRVDSUP_YES       (SDHC_CA1R_DRVDSUP_YES_Val     << SDHC_CA1R_DRVDSUP_Pos)
#define SDHC_CA1R_TCNTRT_Pos        8            /**< \brief (SDHC_CA1R) Timer Count for Re-Tuning */
#define SDHC_CA1R_TCNTRT_Msk        (_U_(0xF) << SDHC_CA1R_TCNTRT_Pos)
#define SDHC_CA1R_TCNTRT(value)     (SDHC_CA1R_TCNTRT_Msk & ((value) << SDHC_CA1R_TCNTRT_Pos))
#define   SDHC_CA1R_TCNTRT_DISABLED_Val   _U_(0x0)   /**< \brief (SDHC_CA1R) Re-Tuning Timer disabled */
#define   SDHC_CA1R_TCNTRT_1S_Val         _U_(0x1)   /**< \brief (SDHC_CA1R) 1 second */
#define   SDHC_CA1R_TCNTRT_2S_Val         _U_(0x2)   /**< \brief (SDHC_CA1R) 2 seconds */
#define   SDHC_CA1R_TCNTRT_4S_Val         _U_(0x3)   /**< \brief (SDHC_CA1R) 4 seconds */
#define   SDHC_CA1R_TCNTRT_8S_Val         _U_(0x4)   /**< \brief (SDHC_CA1R) 8 seconds */
#define   SDHC_CA1R_TCNTRT_16S_Val        _U_(0x5)   /**< \brief (SDHC_CA1R) 16 seconds */
#define   SDHC_CA1R_TCNTRT_32S_Val        _U_(0x6)   /**< \brief (SDHC_CA1R) 32 seconds */
#define   SDHC_CA1R_TCNTRT_64S_Val        _U_(0x7)   /**< \brief (SDHC_CA1R) 64 seconds */
#define   SDHC_CA1R_TCNTRT_128S_Val       _U_(0x8)   /**< \brief (SDHC_CA1R) 128 seconds */
#define   SDHC_CA1R_TCNTRT_256S_Val       _U_(0x9)   /**< \brief (SDHC_CA1R) 256 seconds */
#define   SDHC_CA1R_TCNTRT_512S_Val       _U_(0xA)   /**< \brief (SDHC_CA1R) 512 seconds */
#define   SDHC_CA1R_TCNTRT_1024S_Val      _U_(0xB)   /**< \brief (SDHC_CA1R) 1024 seconds */
#define   SDHC_CA1R_TCNTRT_OTHER_Val      _U_(0xF)   /**< \brief (SDHC_CA1R) Get information from other source */
#define SDHC_CA1R_TCNTRT_DISABLED   (SDHC_CA1R_TCNTRT_DISABLED_Val << SDHC_CA1R_TCNTRT_Pos)
#define SDHC_CA1R_TCNTRT_1S         (SDHC_CA1R_TCNTRT_1S_Val       << SDHC_CA1R_TCNTRT_Pos)
#define SDHC_CA1R_TCNTRT_2S         (SDHC_CA1R_TCNTRT_2S_Val       << SDHC_CA1R_TCNTRT_Pos)
#define SDHC_CA1R_TCNTRT_4S         (SDHC_CA1R_TCNTRT_4S_Val       << SDHC_CA1R_TCNTRT_Pos)
#define SDHC_CA1R_TCNTRT_8S         (SDHC_CA1R_TCNTRT_8S_Val       << SDHC_CA1R_TCNTRT_Pos)
#define SDHC_CA1R_TCNTRT_16S        (SDHC_CA1R_TCNTRT_16S_Val      << SDHC_CA1R_TCNTRT_Pos)
#define SDHC_CA1R_TCNTRT_32S        (SDHC_CA1R_TCNTRT_32S_Val      << SDHC_CA1R_TCNTRT_Pos)
#define SDHC_CA1R_TCNTRT_64S        (SDHC_CA1R_TCNTRT_64S_Val      << SDHC_CA1R_TCNTRT_Pos)
#define SDHC_CA1R_TCNTRT_128S       (SDHC_CA1R_TCNTRT_128S_Val     << SDHC_CA1R_TCNTRT_Pos)
#define SDHC_CA1R_TCNTRT_256S       (SDHC_CA1R_TCNTRT_256S_Val     << SDHC_CA1R_TCNTRT_Pos)
#define SDHC_CA1R_TCNTRT_512S       (SDHC_CA1R_TCNTRT_512S_Val     << SDHC_CA1R_TCNTRT_Pos)
#define SDHC_CA1R_TCNTRT_1024S      (SDHC_CA1R_TCNTRT_1024S_Val    << SDHC_CA1R_TCNTRT_Pos)
#define SDHC_CA1R_TCNTRT_OTHER      (SDHC_CA1R_TCNTRT_OTHER_Val    << SDHC_CA1R_TCNTRT_Pos)
#define SDHC_CA1R_TSDR50_Pos        13           /**< \brief (SDHC_CA1R) Use Tuning for SDR50 */
#define SDHC_CA1R_TSDR50            (_U_(0x1) << SDHC_CA1R_TSDR50_Pos)
#define   SDHC_CA1R_TSDR50_NO_Val         _U_(0x0)   /**< \brief (SDHC_CA1R) SDR50 does not require tuning */
#define   SDHC_CA1R_TSDR50_YES_Val        _U_(0x1)   /**< \brief (SDHC_CA1R) SDR50 requires tuning */
#define SDHC_CA1R_TSDR50_NO         (SDHC_CA1R_TSDR50_NO_Val       << SDHC_CA1R_TSDR50_Pos)
#define SDHC_CA1R_TSDR50_YES        (SDHC_CA1R_TSDR50_YES_Val      << SDHC_CA1R_TSDR50_Pos)
#define SDHC_CA1R_CLKMULT_Pos       16           /**< \brief (SDHC_CA1R) Clock Multiplier */
#define SDHC_CA1R_CLKMULT_Msk       (_U_(0xFF) << SDHC_CA1R_CLKMULT_Pos)
#define SDHC_CA1R_CLKMULT(value)    (SDHC_CA1R_CLKMULT_Msk & ((value) << SDHC_CA1R_CLKMULT_Pos))
#define   SDHC_CA1R_CLKMULT_NO_Val        _U_(0x0)   /**< \brief (SDHC_CA1R) Clock Multiplier is Not Supported */
#define SDHC_CA1R_CLKMULT_NO        (SDHC_CA1R_CLKMULT_NO_Val      << SDHC_CA1R_CLKMULT_Pos)
#define SDHC_CA1R_MASK              _U_(0x00FF2F77) /**< \brief (SDHC_CA1R) MASK Register */

/* -------- SDHC_MCCAR : (SDHC Offset: 0x048) (R/  32) Maximum Current Capabilities -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t MAXCUR33V:8;      /*!< bit:  0.. 7  Maximum Current for 3.3V           */
    uint32_t MAXCUR30V:8;      /*!< bit:  8..15  Maximum Current for 3.0V           */
    uint32_t MAXCUR18V:8;      /*!< bit: 16..23  Maximum Current for 1.8V           */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SDHC_MCCAR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_MCCAR_OFFSET           0x048        /**< \brief (SDHC_MCCAR offset) Maximum Current Capabilities */
#define SDHC_MCCAR_RESETVALUE       _U_(0x00000000) /**< \brief (SDHC_MCCAR reset_value) Maximum Current Capabilities */

#define SDHC_MCCAR_MAXCUR33V_Pos    0            /**< \brief (SDHC_MCCAR) Maximum Current for 3.3V */
#define SDHC_MCCAR_MAXCUR33V_Msk    (_U_(0xFF) << SDHC_MCCAR_MAXCUR33V_Pos)
#define SDHC_MCCAR_MAXCUR33V(value) (SDHC_MCCAR_MAXCUR33V_Msk & ((value) << SDHC_MCCAR_MAXCUR33V_Pos))
#define   SDHC_MCCAR_MAXCUR33V_OTHER_Val  _U_(0x0)   /**< \brief (SDHC_MCCAR) Get information via another method */
#define   SDHC_MCCAR_MAXCUR33V_4MA_Val    _U_(0x1)   /**< \brief (SDHC_MCCAR) 4mA */
#define   SDHC_MCCAR_MAXCUR33V_8MA_Val    _U_(0x2)   /**< \brief (SDHC_MCCAR) 8mA */
#define   SDHC_MCCAR_MAXCUR33V_12MA_Val   _U_(0x3)   /**< \brief (SDHC_MCCAR) 12mA */
#define SDHC_MCCAR_MAXCUR33V_OTHER  (SDHC_MCCAR_MAXCUR33V_OTHER_Val << SDHC_MCCAR_MAXCUR33V_Pos)
#define SDHC_MCCAR_MAXCUR33V_4MA    (SDHC_MCCAR_MAXCUR33V_4MA_Val  << SDHC_MCCAR_MAXCUR33V_Pos)
#define SDHC_MCCAR_MAXCUR33V_8MA    (SDHC_MCCAR_MAXCUR33V_8MA_Val  << SDHC_MCCAR_MAXCUR33V_Pos)
#define SDHC_MCCAR_MAXCUR33V_12MA   (SDHC_MCCAR_MAXCUR33V_12MA_Val << SDHC_MCCAR_MAXCUR33V_Pos)
#define SDHC_MCCAR_MAXCUR30V_Pos    8            /**< \brief (SDHC_MCCAR) Maximum Current for 3.0V */
#define SDHC_MCCAR_MAXCUR30V_Msk    (_U_(0xFF) << SDHC_MCCAR_MAXCUR30V_Pos)
#define SDHC_MCCAR_MAXCUR30V(value) (SDHC_MCCAR_MAXCUR30V_Msk & ((value) << SDHC_MCCAR_MAXCUR30V_Pos))
#define   SDHC_MCCAR_MAXCUR30V_OTHER_Val  _U_(0x0)   /**< \brief (SDHC_MCCAR) Get information via another method */
#define   SDHC_MCCAR_MAXCUR30V_4MA_Val    _U_(0x1)   /**< \brief (SDHC_MCCAR) 4mA */
#define   SDHC_MCCAR_MAXCUR30V_8MA_Val    _U_(0x2)   /**< \brief (SDHC_MCCAR) 8mA */
#define   SDHC_MCCAR_MAXCUR30V_12MA_Val   _U_(0x3)   /**< \brief (SDHC_MCCAR) 12mA */
#define SDHC_MCCAR_MAXCUR30V_OTHER  (SDHC_MCCAR_MAXCUR30V_OTHER_Val << SDHC_MCCAR_MAXCUR30V_Pos)
#define SDHC_MCCAR_MAXCUR30V_4MA    (SDHC_MCCAR_MAXCUR30V_4MA_Val  << SDHC_MCCAR_MAXCUR30V_Pos)
#define SDHC_MCCAR_MAXCUR30V_8MA    (SDHC_MCCAR_MAXCUR30V_8MA_Val  << SDHC_MCCAR_MAXCUR30V_Pos)
#define SDHC_MCCAR_MAXCUR30V_12MA   (SDHC_MCCAR_MAXCUR30V_12MA_Val << SDHC_MCCAR_MAXCUR30V_Pos)
#define SDHC_MCCAR_MAXCUR18V_Pos    16           /**< \brief (SDHC_MCCAR) Maximum Current for 1.8V */
#define SDHC_MCCAR_MAXCUR18V_Msk    (_U_(0xFF) << SDHC_MCCAR_MAXCUR18V_Pos)
#define SDHC_MCCAR_MAXCUR18V(value) (SDHC_MCCAR_MAXCUR18V_Msk & ((value) << SDHC_MCCAR_MAXCUR18V_Pos))
#define   SDHC_MCCAR_MAXCUR18V_OTHER_Val  _U_(0x0)   /**< \brief (SDHC_MCCAR) Get information via another method */
#define   SDHC_MCCAR_MAXCUR18V_4MA_Val    _U_(0x1)   /**< \brief (SDHC_MCCAR) 4mA */
#define   SDHC_MCCAR_MAXCUR18V_8MA_Val    _U_(0x2)   /**< \brief (SDHC_MCCAR) 8mA */
#define   SDHC_MCCAR_MAXCUR18V_12MA_Val   _U_(0x3)   /**< \brief (SDHC_MCCAR) 12mA */
#define SDHC_MCCAR_MAXCUR18V_OTHER  (SDHC_MCCAR_MAXCUR18V_OTHER_Val << SDHC_MCCAR_MAXCUR18V_Pos)
#define SDHC_MCCAR_MAXCUR18V_4MA    (SDHC_MCCAR_MAXCUR18V_4MA_Val  << SDHC_MCCAR_MAXCUR18V_Pos)
#define SDHC_MCCAR_MAXCUR18V_8MA    (SDHC_MCCAR_MAXCUR18V_8MA_Val  << SDHC_MCCAR_MAXCUR18V_Pos)
#define SDHC_MCCAR_MAXCUR18V_12MA   (SDHC_MCCAR_MAXCUR18V_12MA_Val << SDHC_MCCAR_MAXCUR18V_Pos)
#define SDHC_MCCAR_MASK             _U_(0x00FFFFFF) /**< \brief (SDHC_MCCAR) MASK Register */

/* -------- SDHC_FERACES : (SDHC Offset: 0x050) ( /W 16) Force Event for Auto CMD Error Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t ACMD12NE:1;       /*!< bit:      0  Force Event for Auto CMD12 Not Executed */
    uint16_t ACMDTEO:1;        /*!< bit:      1  Force Event for Auto CMD Timeout Error */
    uint16_t ACMDCRC:1;        /*!< bit:      2  Force Event for Auto CMD CRC Error */
    uint16_t ACMDEND:1;        /*!< bit:      3  Force Event for Auto CMD End Bit Error */
    uint16_t ACMDIDX:1;        /*!< bit:      4  Force Event for Auto CMD Index Error */
    uint16_t :2;               /*!< bit:  5.. 6  Reserved                           */
    uint16_t CMDNI:1;          /*!< bit:      7  Force Event for Command Not Issued By Auto CMD12 Error */
    uint16_t :8;               /*!< bit:  8..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_FERACES_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_FERACES_OFFSET         0x050        /**< \brief (SDHC_FERACES offset) Force Event for Auto CMD Error Status */
#define SDHC_FERACES_RESETVALUE     _U_(0x0000)  /**< \brief (SDHC_FERACES reset_value) Force Event for Auto CMD Error Status */

#define SDHC_FERACES_ACMD12NE_Pos   0            /**< \brief (SDHC_FERACES) Force Event for Auto CMD12 Not Executed */
#define SDHC_FERACES_ACMD12NE       (_U_(0x1) << SDHC_FERACES_ACMD12NE_Pos)
#define   SDHC_FERACES_ACMD12NE_NO_Val    _U_(0x0)   /**< \brief (SDHC_FERACES) No Interrupt */
#define   SDHC_FERACES_ACMD12NE_YES_Val   _U_(0x1)   /**< \brief (SDHC_FERACES) Interrupt is generated */
#define SDHC_FERACES_ACMD12NE_NO    (SDHC_FERACES_ACMD12NE_NO_Val  << SDHC_FERACES_ACMD12NE_Pos)
#define SDHC_FERACES_ACMD12NE_YES   (SDHC_FERACES_ACMD12NE_YES_Val << SDHC_FERACES_ACMD12NE_Pos)
#define SDHC_FERACES_ACMDTEO_Pos    1            /**< \brief (SDHC_FERACES) Force Event for Auto CMD Timeout Error */
#define SDHC_FERACES_ACMDTEO        (_U_(0x1) << SDHC_FERACES_ACMDTEO_Pos)
#define   SDHC_FERACES_ACMDTEO_NO_Val     _U_(0x0)   /**< \brief (SDHC_FERACES) No Interrupt */
#define   SDHC_FERACES_ACMDTEO_YES_Val    _U_(0x1)   /**< \brief (SDHC_FERACES) Interrupt is generated */
#define SDHC_FERACES_ACMDTEO_NO     (SDHC_FERACES_ACMDTEO_NO_Val   << SDHC_FERACES_ACMDTEO_Pos)
#define SDHC_FERACES_ACMDTEO_YES    (SDHC_FERACES_ACMDTEO_YES_Val  << SDHC_FERACES_ACMDTEO_Pos)
#define SDHC_FERACES_ACMDCRC_Pos    2            /**< \brief (SDHC_FERACES) Force Event for Auto CMD CRC Error */
#define SDHC_FERACES_ACMDCRC        (_U_(0x1) << SDHC_FERACES_ACMDCRC_Pos)
#define   SDHC_FERACES_ACMDCRC_NO_Val     _U_(0x0)   /**< \brief (SDHC_FERACES) No Interrupt */
#define   SDHC_FERACES_ACMDCRC_YES_Val    _U_(0x1)   /**< \brief (SDHC_FERACES) Interrupt is generated */
#define SDHC_FERACES_ACMDCRC_NO     (SDHC_FERACES_ACMDCRC_NO_Val   << SDHC_FERACES_ACMDCRC_Pos)
#define SDHC_FERACES_ACMDCRC_YES    (SDHC_FERACES_ACMDCRC_YES_Val  << SDHC_FERACES_ACMDCRC_Pos)
#define SDHC_FERACES_ACMDEND_Pos    3            /**< \brief (SDHC_FERACES) Force Event for Auto CMD End Bit Error */
#define SDHC_FERACES_ACMDEND        (_U_(0x1) << SDHC_FERACES_ACMDEND_Pos)
#define   SDHC_FERACES_ACMDEND_NO_Val     _U_(0x0)   /**< \brief (SDHC_FERACES) No Interrupt */
#define   SDHC_FERACES_ACMDEND_YES_Val    _U_(0x1)   /**< \brief (SDHC_FERACES) Interrupt is generated */
#define SDHC_FERACES_ACMDEND_NO     (SDHC_FERACES_ACMDEND_NO_Val   << SDHC_FERACES_ACMDEND_Pos)
#define SDHC_FERACES_ACMDEND_YES    (SDHC_FERACES_ACMDEND_YES_Val  << SDHC_FERACES_ACMDEND_Pos)
#define SDHC_FERACES_ACMDIDX_Pos    4            /**< \brief (SDHC_FERACES) Force Event for Auto CMD Index Error */
#define SDHC_FERACES_ACMDIDX        (_U_(0x1) << SDHC_FERACES_ACMDIDX_Pos)
#define   SDHC_FERACES_ACMDIDX_NO_Val     _U_(0x0)   /**< \brief (SDHC_FERACES) No Interrupt */
#define   SDHC_FERACES_ACMDIDX_YES_Val    _U_(0x1)   /**< \brief (SDHC_FERACES) Interrupt is generated */
#define SDHC_FERACES_ACMDIDX_NO     (SDHC_FERACES_ACMDIDX_NO_Val   << SDHC_FERACES_ACMDIDX_Pos)
#define SDHC_FERACES_ACMDIDX_YES    (SDHC_FERACES_ACMDIDX_YES_Val  << SDHC_FERACES_ACMDIDX_Pos)
#define SDHC_FERACES_CMDNI_Pos      7            /**< \brief (SDHC_FERACES) Force Event for Command Not Issued By Auto CMD12 Error */
#define SDHC_FERACES_CMDNI          (_U_(0x1) << SDHC_FERACES_CMDNI_Pos)
#define   SDHC_FERACES_CMDNI_NO_Val       _U_(0x0)   /**< \brief (SDHC_FERACES) No Interrupt */
#define   SDHC_FERACES_CMDNI_YES_Val      _U_(0x1)   /**< \brief (SDHC_FERACES) Interrupt is generated */
#define SDHC_FERACES_CMDNI_NO       (SDHC_FERACES_CMDNI_NO_Val     << SDHC_FERACES_CMDNI_Pos)
#define SDHC_FERACES_CMDNI_YES      (SDHC_FERACES_CMDNI_YES_Val    << SDHC_FERACES_CMDNI_Pos)
#define SDHC_FERACES_MASK           _U_(0x009F)  /**< \brief (SDHC_FERACES) MASK Register */

/* -------- SDHC_FEREIS : (SDHC Offset: 0x052) ( /W 16) Force Event for Error Interrupt Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t CMDTEO:1;         /*!< bit:      0  Force Event for Command Timeout Error */
    uint16_t CMDCRC:1;         /*!< bit:      1  Force Event for Command CRC Error  */
    uint16_t CMDEND:1;         /*!< bit:      2  Force Event for Command End Bit Error */
    uint16_t CMDIDX:1;         /*!< bit:      3  Force Event for Command Index Error */
    uint16_t DATTEO:1;         /*!< bit:      4  Force Event for Data Timeout Error */
    uint16_t DATCRC:1;         /*!< bit:      5  Force Event for Data CRC Error     */
    uint16_t DATEND:1;         /*!< bit:      6  Force Event for Data End Bit Error */
    uint16_t CURLIM:1;         /*!< bit:      7  Force Event for Current Limit Error */
    uint16_t ACMD:1;           /*!< bit:      8  Force Event for Auto CMD Error     */
    uint16_t ADMA:1;           /*!< bit:      9  Force Event for ADMA Error         */
    uint16_t :2;               /*!< bit: 10..11  Reserved                           */
    uint16_t BOOTAE:1;         /*!< bit:     12  Force Event for Boot Acknowledge Error */
    uint16_t :3;               /*!< bit: 13..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_FEREIS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_FEREIS_OFFSET          0x052        /**< \brief (SDHC_FEREIS offset) Force Event for Error Interrupt Status */
#define SDHC_FEREIS_RESETVALUE      _U_(0x0000)  /**< \brief (SDHC_FEREIS reset_value) Force Event for Error Interrupt Status */

#define SDHC_FEREIS_CMDTEO_Pos      0            /**< \brief (SDHC_FEREIS) Force Event for Command Timeout Error */
#define SDHC_FEREIS_CMDTEO          (_U_(0x1) << SDHC_FEREIS_CMDTEO_Pos)
#define   SDHC_FEREIS_CMDTEO_NO_Val       _U_(0x0)   /**< \brief (SDHC_FEREIS) No Interrupt */
#define   SDHC_FEREIS_CMDTEO_YES_Val      _U_(0x1)   /**< \brief (SDHC_FEREIS) Interrupt is generated */
#define SDHC_FEREIS_CMDTEO_NO       (SDHC_FEREIS_CMDTEO_NO_Val     << SDHC_FEREIS_CMDTEO_Pos)
#define SDHC_FEREIS_CMDTEO_YES      (SDHC_FEREIS_CMDTEO_YES_Val    << SDHC_FEREIS_CMDTEO_Pos)
#define SDHC_FEREIS_CMDCRC_Pos      1            /**< \brief (SDHC_FEREIS) Force Event for Command CRC Error */
#define SDHC_FEREIS_CMDCRC          (_U_(0x1) << SDHC_FEREIS_CMDCRC_Pos)
#define   SDHC_FEREIS_CMDCRC_NO_Val       _U_(0x0)   /**< \brief (SDHC_FEREIS) No Interrupt */
#define   SDHC_FEREIS_CMDCRC_YES_Val      _U_(0x1)   /**< \brief (SDHC_FEREIS) Interrupt is generated */
#define SDHC_FEREIS_CMDCRC_NO       (SDHC_FEREIS_CMDCRC_NO_Val     << SDHC_FEREIS_CMDCRC_Pos)
#define SDHC_FEREIS_CMDCRC_YES      (SDHC_FEREIS_CMDCRC_YES_Val    << SDHC_FEREIS_CMDCRC_Pos)
#define SDHC_FEREIS_CMDEND_Pos      2            /**< \brief (SDHC_FEREIS) Force Event for Command End Bit Error */
#define SDHC_FEREIS_CMDEND          (_U_(0x1) << SDHC_FEREIS_CMDEND_Pos)
#define   SDHC_FEREIS_CMDEND_NO_Val       _U_(0x0)   /**< \brief (SDHC_FEREIS) No Interrupt */
#define   SDHC_FEREIS_CMDEND_YES_Val      _U_(0x1)   /**< \brief (SDHC_FEREIS) Interrupt is generated */
#define SDHC_FEREIS_CMDEND_NO       (SDHC_FEREIS_CMDEND_NO_Val     << SDHC_FEREIS_CMDEND_Pos)
#define SDHC_FEREIS_CMDEND_YES      (SDHC_FEREIS_CMDEND_YES_Val    << SDHC_FEREIS_CMDEND_Pos)
#define SDHC_FEREIS_CMDIDX_Pos      3            /**< \brief (SDHC_FEREIS) Force Event for Command Index Error */
#define SDHC_FEREIS_CMDIDX          (_U_(0x1) << SDHC_FEREIS_CMDIDX_Pos)
#define   SDHC_FEREIS_CMDIDX_NO_Val       _U_(0x0)   /**< \brief (SDHC_FEREIS) No Interrupt */
#define   SDHC_FEREIS_CMDIDX_YES_Val      _U_(0x1)   /**< \brief (SDHC_FEREIS) Interrupt is generated */
#define SDHC_FEREIS_CMDIDX_NO       (SDHC_FEREIS_CMDIDX_NO_Val     << SDHC_FEREIS_CMDIDX_Pos)
#define SDHC_FEREIS_CMDIDX_YES      (SDHC_FEREIS_CMDIDX_YES_Val    << SDHC_FEREIS_CMDIDX_Pos)
#define SDHC_FEREIS_DATTEO_Pos      4            /**< \brief (SDHC_FEREIS) Force Event for Data Timeout Error */
#define SDHC_FEREIS_DATTEO          (_U_(0x1) << SDHC_FEREIS_DATTEO_Pos)
#define   SDHC_FEREIS_DATTEO_NO_Val       _U_(0x0)   /**< \brief (SDHC_FEREIS) No Interrupt */
#define   SDHC_FEREIS_DATTEO_YES_Val      _U_(0x1)   /**< \brief (SDHC_FEREIS) Interrupt is generated */
#define SDHC_FEREIS_DATTEO_NO       (SDHC_FEREIS_DATTEO_NO_Val     << SDHC_FEREIS_DATTEO_Pos)
#define SDHC_FEREIS_DATTEO_YES      (SDHC_FEREIS_DATTEO_YES_Val    << SDHC_FEREIS_DATTEO_Pos)
#define SDHC_FEREIS_DATCRC_Pos      5            /**< \brief (SDHC_FEREIS) Force Event for Data CRC Error */
#define SDHC_FEREIS_DATCRC          (_U_(0x1) << SDHC_FEREIS_DATCRC_Pos)
#define   SDHC_FEREIS_DATCRC_NO_Val       _U_(0x0)   /**< \brief (SDHC_FEREIS) No Interrupt */
#define   SDHC_FEREIS_DATCRC_YES_Val      _U_(0x1)   /**< \brief (SDHC_FEREIS) Interrupt is generated */
#define SDHC_FEREIS_DATCRC_NO       (SDHC_FEREIS_DATCRC_NO_Val     << SDHC_FEREIS_DATCRC_Pos)
#define SDHC_FEREIS_DATCRC_YES      (SDHC_FEREIS_DATCRC_YES_Val    << SDHC_FEREIS_DATCRC_Pos)
#define SDHC_FEREIS_DATEND_Pos      6            /**< \brief (SDHC_FEREIS) Force Event for Data End Bit Error */
#define SDHC_FEREIS_DATEND          (_U_(0x1) << SDHC_FEREIS_DATEND_Pos)
#define   SDHC_FEREIS_DATEND_NO_Val       _U_(0x0)   /**< \brief (SDHC_FEREIS) No Interrupt */
#define   SDHC_FEREIS_DATEND_YES_Val      _U_(0x1)   /**< \brief (SDHC_FEREIS) Interrupt is generated */
#define SDHC_FEREIS_DATEND_NO       (SDHC_FEREIS_DATEND_NO_Val     << SDHC_FEREIS_DATEND_Pos)
#define SDHC_FEREIS_DATEND_YES      (SDHC_FEREIS_DATEND_YES_Val    << SDHC_FEREIS_DATEND_Pos)
#define SDHC_FEREIS_CURLIM_Pos      7            /**< \brief (SDHC_FEREIS) Force Event for Current Limit Error */
#define SDHC_FEREIS_CURLIM          (_U_(0x1) << SDHC_FEREIS_CURLIM_Pos)
#define   SDHC_FEREIS_CURLIM_NO_Val       _U_(0x0)   /**< \brief (SDHC_FEREIS) No Interrupt */
#define   SDHC_FEREIS_CURLIM_YES_Val      _U_(0x1)   /**< \brief (SDHC_FEREIS) Interrupt is generated */
#define SDHC_FEREIS_CURLIM_NO       (SDHC_FEREIS_CURLIM_NO_Val     << SDHC_FEREIS_CURLIM_Pos)
#define SDHC_FEREIS_CURLIM_YES      (SDHC_FEREIS_CURLIM_YES_Val    << SDHC_FEREIS_CURLIM_Pos)
#define SDHC_FEREIS_ACMD_Pos        8            /**< \brief (SDHC_FEREIS) Force Event for Auto CMD Error */
#define SDHC_FEREIS_ACMD            (_U_(0x1) << SDHC_FEREIS_ACMD_Pos)
#define   SDHC_FEREIS_ACMD_NO_Val         _U_(0x0)   /**< \brief (SDHC_FEREIS) No Interrupt */
#define   SDHC_FEREIS_ACMD_YES_Val        _U_(0x1)   /**< \brief (SDHC_FEREIS) Interrupt is generated */
#define SDHC_FEREIS_ACMD_NO         (SDHC_FEREIS_ACMD_NO_Val       << SDHC_FEREIS_ACMD_Pos)
#define SDHC_FEREIS_ACMD_YES        (SDHC_FEREIS_ACMD_YES_Val      << SDHC_FEREIS_ACMD_Pos)
#define SDHC_FEREIS_ADMA_Pos        9            /**< \brief (SDHC_FEREIS) Force Event for ADMA Error */
#define SDHC_FEREIS_ADMA            (_U_(0x1) << SDHC_FEREIS_ADMA_Pos)
#define   SDHC_FEREIS_ADMA_NO_Val         _U_(0x0)   /**< \brief (SDHC_FEREIS) No Interrupt */
#define   SDHC_FEREIS_ADMA_YES_Val        _U_(0x1)   /**< \brief (SDHC_FEREIS) Interrupt is generated */
#define SDHC_FEREIS_ADMA_NO         (SDHC_FEREIS_ADMA_NO_Val       << SDHC_FEREIS_ADMA_Pos)
#define SDHC_FEREIS_ADMA_YES        (SDHC_FEREIS_ADMA_YES_Val      << SDHC_FEREIS_ADMA_Pos)
#define SDHC_FEREIS_BOOTAE_Pos      12           /**< \brief (SDHC_FEREIS) Force Event for Boot Acknowledge Error */
#define SDHC_FEREIS_BOOTAE          (_U_(0x1) << SDHC_FEREIS_BOOTAE_Pos)
#define   SDHC_FEREIS_BOOTAE_NO_Val       _U_(0x0)   /**< \brief (SDHC_FEREIS) No Interrupt */
#define   SDHC_FEREIS_BOOTAE_YES_Val      _U_(0x1)   /**< \brief (SDHC_FEREIS) Interrupt is generated */
#define SDHC_FEREIS_BOOTAE_NO       (SDHC_FEREIS_BOOTAE_NO_Val     << SDHC_FEREIS_BOOTAE_Pos)
#define SDHC_FEREIS_BOOTAE_YES      (SDHC_FEREIS_BOOTAE_YES_Val    << SDHC_FEREIS_BOOTAE_Pos)
#define SDHC_FEREIS_MASK            _U_(0x13FF)  /**< \brief (SDHC_FEREIS) MASK Register */

/* -------- SDHC_AESR : (SDHC Offset: 0x054) (R/   8) ADMA Error Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  ERRST:2;          /*!< bit:  0.. 1  ADMA Error State                   */
    uint8_t  LMIS:1;           /*!< bit:      2  ADMA Length Mismatch Error         */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} SDHC_AESR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_AESR_OFFSET            0x054        /**< \brief (SDHC_AESR offset) ADMA Error Status */
#define SDHC_AESR_RESETVALUE        _U_(0x00)    /**< \brief (SDHC_AESR reset_value) ADMA Error Status */

#define SDHC_AESR_ERRST_Pos         0            /**< \brief (SDHC_AESR) ADMA Error State */
#define SDHC_AESR_ERRST_Msk         (_U_(0x3) << SDHC_AESR_ERRST_Pos)
#define SDHC_AESR_ERRST(value)      (SDHC_AESR_ERRST_Msk & ((value) << SDHC_AESR_ERRST_Pos))
#define   SDHC_AESR_ERRST_STOP_Val        _U_(0x0)   /**< \brief (SDHC_AESR) ST_STOP (Stop DMA) */
#define   SDHC_AESR_ERRST_FDS_Val         _U_(0x1)   /**< \brief (SDHC_AESR) ST_FDS (Fetch Descriptor) */
#define   SDHC_AESR_ERRST_2_Val           _U_(0x2)   /**< \brief (SDHC_AESR) Reserved */
#define   SDHC_AESR_ERRST_TFR_Val         _U_(0x3)   /**< \brief (SDHC_AESR) ST_TFR (Transfer Data) */
#define SDHC_AESR_ERRST_STOP        (SDHC_AESR_ERRST_STOP_Val      << SDHC_AESR_ERRST_Pos)
#define SDHC_AESR_ERRST_FDS         (SDHC_AESR_ERRST_FDS_Val       << SDHC_AESR_ERRST_Pos)
#define SDHC_AESR_ERRST_2           (SDHC_AESR_ERRST_2_Val         << SDHC_AESR_ERRST_Pos)
#define SDHC_AESR_ERRST_TFR         (SDHC_AESR_ERRST_TFR_Val       << SDHC_AESR_ERRST_Pos)
#define SDHC_AESR_LMIS_Pos          2            /**< \brief (SDHC_AESR) ADMA Length Mismatch Error */
#define SDHC_AESR_LMIS              (_U_(0x1) << SDHC_AESR_LMIS_Pos)
#define   SDHC_AESR_LMIS_NO_Val           _U_(0x0)   /**< \brief (SDHC_AESR) No Error */
#define   SDHC_AESR_LMIS_YES_Val          _U_(0x1)   /**< \brief (SDHC_AESR) Error */
#define SDHC_AESR_LMIS_NO           (SDHC_AESR_LMIS_NO_Val         << SDHC_AESR_LMIS_Pos)
#define SDHC_AESR_LMIS_YES          (SDHC_AESR_LMIS_YES_Val        << SDHC_AESR_LMIS_Pos)
#define SDHC_AESR_MASK              _U_(0x07)    /**< \brief (SDHC_AESR) MASK Register */

/* -------- SDHC_ASAR : (SDHC Offset: 0x058) (R/W 32) ADMA System Address n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ADMASA:32;        /*!< bit:  0..31  ADMA System Address                */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SDHC_ASAR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_ASAR_OFFSET            0x058        /**< \brief (SDHC_ASAR offset) ADMA System Address n */
#define SDHC_ASAR_RESETVALUE        _U_(0x00000000) /**< \brief (SDHC_ASAR reset_value) ADMA System Address n */

#define SDHC_ASAR_ADMASA_Pos        0            /**< \brief (SDHC_ASAR) ADMA System Address */
#define SDHC_ASAR_ADMASA_Msk        (_U_(0xFFFFFFFF) << SDHC_ASAR_ADMASA_Pos)
#define SDHC_ASAR_ADMASA(value)     (SDHC_ASAR_ADMASA_Msk & ((value) << SDHC_ASAR_ADMASA_Pos))
#define SDHC_ASAR_MASK              _U_(0xFFFFFFFF) /**< \brief (SDHC_ASAR) MASK Register */

/* -------- SDHC_PVR : (SDHC Offset: 0x060) (R/W 16) Preset Value n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t SDCLKFSEL:10;     /*!< bit:  0.. 9  SDCLK Frequency Select Value for Initialization */
    uint16_t CLKGSEL:1;        /*!< bit:     10  Clock Generator Select Value for Initialization */
    uint16_t :3;               /*!< bit: 11..13  Reserved                           */
    uint16_t DRVSEL:2;         /*!< bit: 14..15  Driver Strength Select Value for Initialization */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_PVR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_PVR_OFFSET             0x060        /**< \brief (SDHC_PVR offset) Preset Value n */
#define SDHC_PVR_RESETVALUE         _U_(0x0000)  /**< \brief (SDHC_PVR reset_value) Preset Value n */

#define SDHC_PVR_SDCLKFSEL_Pos      0            /**< \brief (SDHC_PVR) SDCLK Frequency Select Value for Initialization */
#define SDHC_PVR_SDCLKFSEL_Msk      (_U_(0x3FF) << SDHC_PVR_SDCLKFSEL_Pos)
#define SDHC_PVR_SDCLKFSEL(value)   (SDHC_PVR_SDCLKFSEL_Msk & ((value) << SDHC_PVR_SDCLKFSEL_Pos))
#define SDHC_PVR_CLKGSEL_Pos        10           /**< \brief (SDHC_PVR) Clock Generator Select Value for Initialization */
#define SDHC_PVR_CLKGSEL            (_U_(0x1) << SDHC_PVR_CLKGSEL_Pos)
#define   SDHC_PVR_CLKGSEL_DIV_Val        _U_(0x0)   /**< \brief (SDHC_PVR) Host Controller Ver2.00 Compatible Clock Generator (Divider) */
#define   SDHC_PVR_CLKGSEL_PROG_Val       _U_(0x1)   /**< \brief (SDHC_PVR) Programmable Clock Generator */
#define SDHC_PVR_CLKGSEL_DIV        (SDHC_PVR_CLKGSEL_DIV_Val      << SDHC_PVR_CLKGSEL_Pos)
#define SDHC_PVR_CLKGSEL_PROG       (SDHC_PVR_CLKGSEL_PROG_Val     << SDHC_PVR_CLKGSEL_Pos)
#define SDHC_PVR_DRVSEL_Pos         14           /**< \brief (SDHC_PVR) Driver Strength Select Value for Initialization */
#define SDHC_PVR_DRVSEL_Msk         (_U_(0x3) << SDHC_PVR_DRVSEL_Pos)
#define SDHC_PVR_DRVSEL(value)      (SDHC_PVR_DRVSEL_Msk & ((value) << SDHC_PVR_DRVSEL_Pos))
#define   SDHC_PVR_DRVSEL_B_Val           _U_(0x0)   /**< \brief (SDHC_PVR) Driver Type B is Selected */
#define   SDHC_PVR_DRVSEL_A_Val           _U_(0x1)   /**< \brief (SDHC_PVR) Driver Type A is Selected */
#define   SDHC_PVR_DRVSEL_C_Val           _U_(0x2)   /**< \brief (SDHC_PVR) Driver Type C is Selected */
#define   SDHC_PVR_DRVSEL_D_Val           _U_(0x3)   /**< \brief (SDHC_PVR) Driver Type D is Selected */
#define SDHC_PVR_DRVSEL_B           (SDHC_PVR_DRVSEL_B_Val         << SDHC_PVR_DRVSEL_Pos)
#define SDHC_PVR_DRVSEL_A           (SDHC_PVR_DRVSEL_A_Val         << SDHC_PVR_DRVSEL_Pos)
#define SDHC_PVR_DRVSEL_C           (SDHC_PVR_DRVSEL_C_Val         << SDHC_PVR_DRVSEL_Pos)
#define SDHC_PVR_DRVSEL_D           (SDHC_PVR_DRVSEL_D_Val         << SDHC_PVR_DRVSEL_Pos)
#define SDHC_PVR_MASK               _U_(0xC7FF)  /**< \brief (SDHC_PVR) MASK Register */

/* -------- SDHC_SISR : (SDHC Offset: 0x0FC) (R/  16) Slot Interrupt Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t INTSSL:1;         /*!< bit:      0  Interrupt Signal for Each Slot     */
    uint16_t :15;              /*!< bit:  1..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_SISR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_SISR_OFFSET            0x0FC        /**< \brief (SDHC_SISR offset) Slot Interrupt Status */
#define SDHC_SISR_RESETVALUE        _U_(0x20000) /**< \brief (SDHC_SISR reset_value) Slot Interrupt Status */

#define SDHC_SISR_INTSSL_Pos        0            /**< \brief (SDHC_SISR) Interrupt Signal for Each Slot */
#define SDHC_SISR_INTSSL_Msk        (_U_(0x1) << SDHC_SISR_INTSSL_Pos)
#define SDHC_SISR_INTSSL(value)     (SDHC_SISR_INTSSL_Msk & ((value) << SDHC_SISR_INTSSL_Pos))
#define SDHC_SISR_MASK              _U_(0x0001)  /**< \brief (SDHC_SISR) MASK Register */

/* -------- SDHC_HCVR : (SDHC Offset: 0x0FE) (R/  16) Host Controller Version -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t SVER:8;           /*!< bit:  0.. 7  Spec Version                       */
    uint16_t VVER:8;           /*!< bit:  8..15  Vendor Version                     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} SDHC_HCVR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_HCVR_OFFSET            0x0FE        /**< \brief (SDHC_HCVR offset) Host Controller Version */
#define SDHC_HCVR_RESETVALUE        _U_(0x1802)  /**< \brief (SDHC_HCVR reset_value) Host Controller Version */

#define SDHC_HCVR_SVER_Pos          0            /**< \brief (SDHC_HCVR) Spec Version */
#define SDHC_HCVR_SVER_Msk          (_U_(0xFF) << SDHC_HCVR_SVER_Pos)
#define SDHC_HCVR_SVER(value)       (SDHC_HCVR_SVER_Msk & ((value) << SDHC_HCVR_SVER_Pos))
#define SDHC_HCVR_VVER_Pos          8            /**< \brief (SDHC_HCVR) Vendor Version */
#define SDHC_HCVR_VVER_Msk          (_U_(0xFF) << SDHC_HCVR_VVER_Pos)
#define SDHC_HCVR_VVER(value)       (SDHC_HCVR_VVER_Msk & ((value) << SDHC_HCVR_VVER_Pos))
#define SDHC_HCVR_MASK              _U_(0xFFFF)  /**< \brief (SDHC_HCVR) MASK Register */

/* -------- SDHC_MC1R : (SDHC Offset: 0x204) (R/W  8) MMC Control 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CMDTYP:2;         /*!< bit:  0.. 1  e.MMC Command Type                 */
    uint8_t  :1;               /*!< bit:      2  Reserved                           */
    uint8_t  DDR:1;            /*!< bit:      3  e.MMC HSDDR Mode                   */
    uint8_t  OPD:1;            /*!< bit:      4  e.MMC Open Drain Mode              */
    uint8_t  BOOTA:1;          /*!< bit:      5  e.MMC Boot Acknowledge Enable      */
    uint8_t  RSTN:1;           /*!< bit:      6  e.MMC Reset Signal                 */
    uint8_t  FCD:1;            /*!< bit:      7  e.MMC Force Card Detect            */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} SDHC_MC1R_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_MC1R_OFFSET            0x204        /**< \brief (SDHC_MC1R offset) MMC Control 1 */
#define SDHC_MC1R_RESETVALUE        _U_(0x00)    /**< \brief (SDHC_MC1R reset_value) MMC Control 1 */

#define SDHC_MC1R_CMDTYP_Pos        0            /**< \brief (SDHC_MC1R) e.MMC Command Type */
#define SDHC_MC1R_CMDTYP_Msk        (_U_(0x3) << SDHC_MC1R_CMDTYP_Pos)
#define SDHC_MC1R_CMDTYP(value)     (SDHC_MC1R_CMDTYP_Msk & ((value) << SDHC_MC1R_CMDTYP_Pos))
#define   SDHC_MC1R_CMDTYP_NORMAL_Val     _U_(0x0)   /**< \brief (SDHC_MC1R) Not a MMC specific command */
#define   SDHC_MC1R_CMDTYP_WAITIRQ_Val    _U_(0x1)   /**< \brief (SDHC_MC1R) Wait IRQ Command */
#define   SDHC_MC1R_CMDTYP_STREAM_Val     _U_(0x2)   /**< \brief (SDHC_MC1R) Stream Command */
#define   SDHC_MC1R_CMDTYP_BOOT_Val       _U_(0x3)   /**< \brief (SDHC_MC1R) Boot Command */
#define SDHC_MC1R_CMDTYP_NORMAL     (SDHC_MC1R_CMDTYP_NORMAL_Val   << SDHC_MC1R_CMDTYP_Pos)
#define SDHC_MC1R_CMDTYP_WAITIRQ    (SDHC_MC1R_CMDTYP_WAITIRQ_Val  << SDHC_MC1R_CMDTYP_Pos)
#define SDHC_MC1R_CMDTYP_STREAM     (SDHC_MC1R_CMDTYP_STREAM_Val   << SDHC_MC1R_CMDTYP_Pos)
#define SDHC_MC1R_CMDTYP_BOOT       (SDHC_MC1R_CMDTYP_BOOT_Val     << SDHC_MC1R_CMDTYP_Pos)
#define SDHC_MC1R_DDR_Pos           3            /**< \brief (SDHC_MC1R) e.MMC HSDDR Mode */
#define SDHC_MC1R_DDR               (_U_(0x1) << SDHC_MC1R_DDR_Pos)
#define SDHC_MC1R_OPD_Pos           4            /**< \brief (SDHC_MC1R) e.MMC Open Drain Mode */
#define SDHC_MC1R_OPD               (_U_(0x1) << SDHC_MC1R_OPD_Pos)
#define SDHC_MC1R_BOOTA_Pos         5            /**< \brief (SDHC_MC1R) e.MMC Boot Acknowledge Enable */
#define SDHC_MC1R_BOOTA             (_U_(0x1) << SDHC_MC1R_BOOTA_Pos)
#define SDHC_MC1R_RSTN_Pos          6            /**< \brief (SDHC_MC1R) e.MMC Reset Signal */
#define SDHC_MC1R_RSTN              (_U_(0x1) << SDHC_MC1R_RSTN_Pos)
#define SDHC_MC1R_FCD_Pos           7            /**< \brief (SDHC_MC1R) e.MMC Force Card Detect */
#define SDHC_MC1R_FCD               (_U_(0x1) << SDHC_MC1R_FCD_Pos)
#define SDHC_MC1R_MASK              _U_(0xFB)    /**< \brief (SDHC_MC1R) MASK Register */

/* -------- SDHC_MC2R : (SDHC Offset: 0x205) ( /W  8) MMC Control 2 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SRESP:1;          /*!< bit:      0  e.MMC Abort Wait IRQ               */
    uint8_t  ABOOT:1;          /*!< bit:      1  e.MMC Abort Boot                   */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} SDHC_MC2R_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_MC2R_OFFSET            0x205        /**< \brief (SDHC_MC2R offset) MMC Control 2 */
#define SDHC_MC2R_RESETVALUE        _U_(0x00)    /**< \brief (SDHC_MC2R reset_value) MMC Control 2 */

#define SDHC_MC2R_SRESP_Pos         0            /**< \brief (SDHC_MC2R) e.MMC Abort Wait IRQ */
#define SDHC_MC2R_SRESP             (_U_(0x1) << SDHC_MC2R_SRESP_Pos)
#define SDHC_MC2R_ABOOT_Pos         1            /**< \brief (SDHC_MC2R) e.MMC Abort Boot */
#define SDHC_MC2R_ABOOT             (_U_(0x1) << SDHC_MC2R_ABOOT_Pos)
#define SDHC_MC2R_MASK              _U_(0x03)    /**< \brief (SDHC_MC2R) MASK Register */

/* -------- SDHC_ACR : (SDHC Offset: 0x208) (R/W 32) AHB Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BMAX:2;           /*!< bit:  0.. 1  AHB Maximum Burst                  */
    uint32_t :30;              /*!< bit:  2..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SDHC_ACR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_ACR_OFFSET             0x208        /**< \brief (SDHC_ACR offset) AHB Control */
#define SDHC_ACR_RESETVALUE         _U_(0x00000000) /**< \brief (SDHC_ACR reset_value) AHB Control */

#define SDHC_ACR_BMAX_Pos           0            /**< \brief (SDHC_ACR) AHB Maximum Burst */
#define SDHC_ACR_BMAX_Msk           (_U_(0x3) << SDHC_ACR_BMAX_Pos)
#define SDHC_ACR_BMAX(value)        (SDHC_ACR_BMAX_Msk & ((value) << SDHC_ACR_BMAX_Pos))
#define   SDHC_ACR_BMAX_INCR16_Val        _U_(0x0)   /**< \brief (SDHC_ACR)  */
#define   SDHC_ACR_BMAX_INCR8_Val         _U_(0x1)   /**< \brief (SDHC_ACR)  */
#define   SDHC_ACR_BMAX_INCR4_Val         _U_(0x2)   /**< \brief (SDHC_ACR)  */
#define   SDHC_ACR_BMAX_SINGLE_Val        _U_(0x3)   /**< \brief (SDHC_ACR)  */
#define SDHC_ACR_BMAX_INCR16        (SDHC_ACR_BMAX_INCR16_Val      << SDHC_ACR_BMAX_Pos)
#define SDHC_ACR_BMAX_INCR8         (SDHC_ACR_BMAX_INCR8_Val       << SDHC_ACR_BMAX_Pos)
#define SDHC_ACR_BMAX_INCR4         (SDHC_ACR_BMAX_INCR4_Val       << SDHC_ACR_BMAX_Pos)
#define SDHC_ACR_BMAX_SINGLE        (SDHC_ACR_BMAX_SINGLE_Val      << SDHC_ACR_BMAX_Pos)
#define SDHC_ACR_MASK               _U_(0x00000003) /**< \brief (SDHC_ACR) MASK Register */

/* -------- SDHC_CC2R : (SDHC Offset: 0x20C) (R/W 32) Clock Control 2 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t FSDCLKD:1;        /*!< bit:      0  Force SDCK Disabled                */
    uint32_t :31;              /*!< bit:  1..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SDHC_CC2R_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_CC2R_OFFSET            0x20C        /**< \brief (SDHC_CC2R offset) Clock Control 2 */
#define SDHC_CC2R_RESETVALUE        _U_(0x00000000) /**< \brief (SDHC_CC2R reset_value) Clock Control 2 */

#define SDHC_CC2R_FSDCLKD_Pos       0            /**< \brief (SDHC_CC2R) Force SDCK Disabled */
#define SDHC_CC2R_FSDCLKD           (_U_(0x1) << SDHC_CC2R_FSDCLKD_Pos)
#define   SDHC_CC2R_FSDCLKD_NOEFFECT_Val  _U_(0x0)   /**< \brief (SDHC_CC2R) No effect */
#define   SDHC_CC2R_FSDCLKD_DISABLE_Val   _U_(0x1)   /**< \brief (SDHC_CC2R) SDCLK can be stopped at any time after DATA transfer.SDCLK enable forcing for 8 SDCLK cycles is disabled */
#define SDHC_CC2R_FSDCLKD_NOEFFECT  (SDHC_CC2R_FSDCLKD_NOEFFECT_Val << SDHC_CC2R_FSDCLKD_Pos)
#define SDHC_CC2R_FSDCLKD_DISABLE   (SDHC_CC2R_FSDCLKD_DISABLE_Val << SDHC_CC2R_FSDCLKD_Pos)
#define SDHC_CC2R_MASK              _U_(0x00000001) /**< \brief (SDHC_CC2R) MASK Register */

/* -------- SDHC_CACR : (SDHC Offset: 0x230) (R/W 32) Capabilities Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CAPWREN:1;        /*!< bit:      0  Capabilities Registers Write Enable (Required to write the correct frequencies in the Capabilities Registers) */
    uint32_t :7;               /*!< bit:  1.. 7  Reserved                           */
    uint32_t KEY:8;            /*!< bit:  8..15  Key (0x46)                         */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SDHC_CACR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_CACR_OFFSET            0x230        /**< \brief (SDHC_CACR offset) Capabilities Control */
#define SDHC_CACR_RESETVALUE        _U_(0x00000000) /**< \brief (SDHC_CACR reset_value) Capabilities Control */

#define SDHC_CACR_CAPWREN_Pos       0            /**< \brief (SDHC_CACR) Capabilities Registers Write Enable (Required to write the correct frequencies in the Capabilities Registers) */
#define SDHC_CACR_CAPWREN           (_U_(0x1) << SDHC_CACR_CAPWREN_Pos)
#define SDHC_CACR_KEY_Pos           8            /**< \brief (SDHC_CACR) Key (0x46) */
#define SDHC_CACR_KEY_Msk           (_U_(0xFF) << SDHC_CACR_KEY_Pos)
#define SDHC_CACR_KEY(value)        (SDHC_CACR_KEY_Msk & ((value) << SDHC_CACR_KEY_Pos))
#define SDHC_CACR_MASK              _U_(0x0000FF01) /**< \brief (SDHC_CACR) MASK Register */

/* -------- SDHC_DBGR : (SDHC Offset: 0x234) (R/W  8) Debug -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  NIDBG:1;          /*!< bit:      0  Non-intrusive debug enable         */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} SDHC_DBGR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SDHC_DBGR_OFFSET            0x234        /**< \brief (SDHC_DBGR offset) Debug */
#define SDHC_DBGR_RESETVALUE        _U_(0x00)    /**< \brief (SDHC_DBGR reset_value) Debug */

#define SDHC_DBGR_NIDBG_Pos         0            /**< \brief (SDHC_DBGR) Non-intrusive debug enable */
#define SDHC_DBGR_NIDBG             (_U_(0x1) << SDHC_DBGR_NIDBG_Pos)
#define   SDHC_DBGR_NIDBG_IDBG_Val        _U_(0x0)   /**< \brief (SDHC_DBGR) Debugging is intrusive (reads of BDPR from debugger are considered and increment the internal buffer pointer) */
#define   SDHC_DBGR_NIDBG_NIDBG_Val       _U_(0x1)   /**< \brief (SDHC_DBGR) Debugging is not intrusive (reads of BDPR from debugger are discarded and do not increment the internal buffer pointer) */
#define SDHC_DBGR_NIDBG_IDBG        (SDHC_DBGR_NIDBG_IDBG_Val      << SDHC_DBGR_NIDBG_Pos)
#define SDHC_DBGR_NIDBG_NIDBG       (SDHC_DBGR_NIDBG_NIDBG_Val     << SDHC_DBGR_NIDBG_Pos)
#define SDHC_DBGR_MASK              _U_(0x01)    /**< \brief (SDHC_DBGR) MASK Register */

/** \brief SDHC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO SDHC_SSAR_Type            SSAR;        /**< \brief Offset: 0x000 (R/W 32) SDMA System Address / Argument 2 */
  __IO SDHC_BSR_Type             BSR;         /**< \brief Offset: 0x004 (R/W 16) Block Size */
  __IO SDHC_BCR_Type             BCR;         /**< \brief Offset: 0x006 (R/W 16) Block Count */
  __IO SDHC_ARG1R_Type           ARG1R;       /**< \brief Offset: 0x008 (R/W 32) Argument 1 */
  __IO SDHC_TMR_Type             TMR;         /**< \brief Offset: 0x00C (R/W 16) Transfer Mode */
  __IO SDHC_CR_Type              CR;          /**< \brief Offset: 0x00E (R/W 16) Command */
  __I  SDHC_RR_Type              RR[4];       /**< \brief Offset: 0x010 (R/  32) Response */
  __IO SDHC_BDPR_Type            BDPR;        /**< \brief Offset: 0x020 (R/W 32) Buffer Data Port */
  __I  SDHC_PSR_Type             PSR;         /**< \brief Offset: 0x024 (R/  32) Present State */
  __IO SDHC_HC1R_Type            HC1R;        /**< \brief Offset: 0x028 (R/W  8) Host Control 1 */
  __IO SDHC_PCR_Type             PCR;         /**< \brief Offset: 0x029 (R/W  8) Power Control */
  __IO SDHC_BGCR_Type            BGCR;        /**< \brief Offset: 0x02A (R/W  8) Block Gap Control */
  __IO SDHC_WCR_Type             WCR;         /**< \brief Offset: 0x02B (R/W  8) Wakeup Control */
  __IO SDHC_CCR_Type             CCR;         /**< \brief Offset: 0x02C (R/W 16) Clock Control */
  __IO SDHC_TCR_Type             TCR;         /**< \brief Offset: 0x02E (R/W  8) Timeout Control */
  __IO SDHC_SRR_Type             SRR;         /**< \brief Offset: 0x02F (R/W  8) Software Reset */
  __IO SDHC_NISTR_Type           NISTR;       /**< \brief Offset: 0x030 (R/W 16) Normal Interrupt Status */
  __IO SDHC_EISTR_Type           EISTR;       /**< \brief Offset: 0x032 (R/W 16) Error Interrupt Status */
  __IO SDHC_NISTER_Type          NISTER;      /**< \brief Offset: 0x034 (R/W 16) Normal Interrupt Status Enable */
  __IO SDHC_EISTER_Type          EISTER;      /**< \brief Offset: 0x036 (R/W 16) Error Interrupt Status Enable */
  __IO SDHC_NISIER_Type          NISIER;      /**< \brief Offset: 0x038 (R/W 16) Normal Interrupt Signal Enable */
  __IO SDHC_EISIER_Type          EISIER;      /**< \brief Offset: 0x03A (R/W 16) Error Interrupt Signal Enable */
  __I  SDHC_ACESR_Type           ACESR;       /**< \brief Offset: 0x03C (R/  16) Auto CMD Error Status */
  __IO SDHC_HC2R_Type            HC2R;        /**< \brief Offset: 0x03E (R/W 16) Host Control 2 */
  __I  SDHC_CA0R_Type            CA0R;        /**< \brief Offset: 0x040 (R/  32) Capabilities 0 */
  __I  SDHC_CA1R_Type            CA1R;        /**< \brief Offset: 0x044 (R/  32) Capabilities 1 */
  __I  SDHC_MCCAR_Type           MCCAR;       /**< \brief Offset: 0x048 (R/  32) Maximum Current Capabilities */
       RoReg8                    Reserved1[0x4];
  __O  SDHC_FERACES_Type         FERACES;     /**< \brief Offset: 0x050 ( /W 16) Force Event for Auto CMD Error Status */
  __O  SDHC_FEREIS_Type          FEREIS;      /**< \brief Offset: 0x052 ( /W 16) Force Event for Error Interrupt Status */
  __I  SDHC_AESR_Type            AESR;        /**< \brief Offset: 0x054 (R/   8) ADMA Error Status */
       RoReg8                    Reserved2[0x3];
  __IO SDHC_ASAR_Type            ASAR[1];     /**< \brief Offset: 0x058 (R/W 32) ADMA System Address n */
       RoReg8                    Reserved3[0x4];
  __IO SDHC_PVR_Type             PVR[8];      /**< \brief Offset: 0x060 (R/W 16) Preset Value n */
       RoReg8                    Reserved4[0x8C];
  __I  SDHC_SISR_Type            SISR;        /**< \brief Offset: 0x0FC (R/  16) Slot Interrupt Status */
  __I  SDHC_HCVR_Type            HCVR;        /**< \brief Offset: 0x0FE (R/  16) Host Controller Version */
       RoReg8                    Reserved5[0x104];
  __IO SDHC_MC1R_Type            MC1R;        /**< \brief Offset: 0x204 (R/W  8) MMC Control 1 */
  __O  SDHC_MC2R_Type            MC2R;        /**< \brief Offset: 0x205 ( /W  8) MMC Control 2 */
       RoReg8                    Reserved6[0x2];
  __IO SDHC_ACR_Type             ACR;         /**< \brief Offset: 0x208 (R/W 32) AHB Control */
  __IO SDHC_CC2R_Type            CC2R;        /**< \brief Offset: 0x20C (R/W 32) Clock Control 2 */
       RoReg8                    Reserved7[0x20];
  __IO SDHC_CACR_Type            CACR;        /**< \brief Offset: 0x230 (R/W 32) Capabilities Control */
  __IO SDHC_DBGR_Type            DBGR;        /**< \brief Offset: 0x234 (R/W  8) Debug */
} Sdhc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_SDHC_COMPONENT_ */
