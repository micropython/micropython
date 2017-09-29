/**
 * \file
 *
 * \brief Component description for PCC
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

#ifndef _SAMD51_PCC_COMPONENT_
#define _SAMD51_PCC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR PCC */
/* ========================================================================== */
/** \addtogroup SAMD51_PCC Parallel Capture Controller */
/*@{*/

#define PCC_U2017
#define REV_PCC                     0x110

/* -------- PCC_MR : (PCC Offset: 0x00) (R/W 32) Mode Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PCEN:1;           /*!< bit:      0  Parallel Capture Enable            */
    uint32_t :3;               /*!< bit:  1.. 3  Reserved                           */
    uint32_t DSIZE:2;          /*!< bit:  4.. 5  Data size                          */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t SCALE:1;          /*!< bit:      8  Scale data                         */
    uint32_t ALWYS:1;          /*!< bit:      9  Always Sampling                    */
    uint32_t HALFS:1;          /*!< bit:     10  Half Sampling                      */
    uint32_t FRSTS:1;          /*!< bit:     11  First sample                       */
    uint32_t :4;               /*!< bit: 12..15  Reserved                           */
    uint32_t ISIZE:3;          /*!< bit: 16..18  Input Data Size                    */
    uint32_t :11;              /*!< bit: 19..29  Reserved                           */
    uint32_t CID:2;            /*!< bit: 30..31  Clear If Disabled                  */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PCC_MR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PCC_MR_OFFSET               0x00         /**< \brief (PCC_MR offset) Mode Register */
#define PCC_MR_RESETVALUE           _U_(0x00000000) /**< \brief (PCC_MR reset_value) Mode Register */

#define PCC_MR_PCEN_Pos             0            /**< \brief (PCC_MR) Parallel Capture Enable */
#define PCC_MR_PCEN                 (_U_(0x1) << PCC_MR_PCEN_Pos)
#define PCC_MR_DSIZE_Pos            4            /**< \brief (PCC_MR) Data size */
#define PCC_MR_DSIZE_Msk            (_U_(0x3) << PCC_MR_DSIZE_Pos)
#define PCC_MR_DSIZE(value)         (PCC_MR_DSIZE_Msk & ((value) << PCC_MR_DSIZE_Pos))
#define PCC_MR_SCALE_Pos            8            /**< \brief (PCC_MR) Scale data */
#define PCC_MR_SCALE                (_U_(0x1) << PCC_MR_SCALE_Pos)
#define PCC_MR_ALWYS_Pos            9            /**< \brief (PCC_MR) Always Sampling */
#define PCC_MR_ALWYS                (_U_(0x1) << PCC_MR_ALWYS_Pos)
#define PCC_MR_HALFS_Pos            10           /**< \brief (PCC_MR) Half Sampling */
#define PCC_MR_HALFS                (_U_(0x1) << PCC_MR_HALFS_Pos)
#define PCC_MR_FRSTS_Pos            11           /**< \brief (PCC_MR) First sample */
#define PCC_MR_FRSTS                (_U_(0x1) << PCC_MR_FRSTS_Pos)
#define PCC_MR_ISIZE_Pos            16           /**< \brief (PCC_MR) Input Data Size */
#define PCC_MR_ISIZE_Msk            (_U_(0x7) << PCC_MR_ISIZE_Pos)
#define PCC_MR_ISIZE(value)         (PCC_MR_ISIZE_Msk & ((value) << PCC_MR_ISIZE_Pos))
#define PCC_MR_CID_Pos              30           /**< \brief (PCC_MR) Clear If Disabled */
#define PCC_MR_CID_Msk              (_U_(0x3) << PCC_MR_CID_Pos)
#define PCC_MR_CID(value)           (PCC_MR_CID_Msk & ((value) << PCC_MR_CID_Pos))
#define PCC_MR_MASK                 _U_(0xC0070F31) /**< \brief (PCC_MR) MASK Register */

/* -------- PCC_IER : (PCC Offset: 0x04) ( /W 32) Interrupt Enable Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DRDY:1;           /*!< bit:      0  Data Ready Interrupt Enable        */
    uint32_t OVRE:1;           /*!< bit:      1  Overrun Error Interrupt Enable     */
    uint32_t :30;              /*!< bit:  2..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PCC_IER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PCC_IER_OFFSET              0x04         /**< \brief (PCC_IER offset) Interrupt Enable Register */
#define PCC_IER_RESETVALUE          _U_(0x00000000) /**< \brief (PCC_IER reset_value) Interrupt Enable Register */

#define PCC_IER_DRDY_Pos            0            /**< \brief (PCC_IER) Data Ready Interrupt Enable */
#define PCC_IER_DRDY                (_U_(0x1) << PCC_IER_DRDY_Pos)
#define PCC_IER_OVRE_Pos            1            /**< \brief (PCC_IER) Overrun Error Interrupt Enable */
#define PCC_IER_OVRE                (_U_(0x1) << PCC_IER_OVRE_Pos)
#define PCC_IER_MASK                _U_(0x00000003) /**< \brief (PCC_IER) MASK Register */

/* -------- PCC_IDR : (PCC Offset: 0x08) ( /W 32) Interrupt Disable Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DRDY:1;           /*!< bit:      0  Data Ready Interrupt Disable       */
    uint32_t OVRE:1;           /*!< bit:      1  Overrun Error Interrupt Disable    */
    uint32_t :30;              /*!< bit:  2..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PCC_IDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PCC_IDR_OFFSET              0x08         /**< \brief (PCC_IDR offset) Interrupt Disable Register */
#define PCC_IDR_RESETVALUE          _U_(0x00000000) /**< \brief (PCC_IDR reset_value) Interrupt Disable Register */

#define PCC_IDR_DRDY_Pos            0            /**< \brief (PCC_IDR) Data Ready Interrupt Disable */
#define PCC_IDR_DRDY                (_U_(0x1) << PCC_IDR_DRDY_Pos)
#define PCC_IDR_OVRE_Pos            1            /**< \brief (PCC_IDR) Overrun Error Interrupt Disable */
#define PCC_IDR_OVRE                (_U_(0x1) << PCC_IDR_OVRE_Pos)
#define PCC_IDR_MASK                _U_(0x00000003) /**< \brief (PCC_IDR) MASK Register */

/* -------- PCC_IMR : (PCC Offset: 0x0C) (R/  32) Interrupt Mask Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DRDY:1;           /*!< bit:      0  Data Ready Interrupt Mask          */
    uint32_t OVRE:1;           /*!< bit:      1  Overrun Error Interrupt Mask       */
    uint32_t :30;              /*!< bit:  2..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PCC_IMR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PCC_IMR_OFFSET              0x0C         /**< \brief (PCC_IMR offset) Interrupt Mask Register */
#define PCC_IMR_RESETVALUE          _U_(0x00000000) /**< \brief (PCC_IMR reset_value) Interrupt Mask Register */

#define PCC_IMR_DRDY_Pos            0            /**< \brief (PCC_IMR) Data Ready Interrupt Mask */
#define PCC_IMR_DRDY                (_U_(0x1) << PCC_IMR_DRDY_Pos)
#define PCC_IMR_OVRE_Pos            1            /**< \brief (PCC_IMR) Overrun Error Interrupt Mask */
#define PCC_IMR_OVRE                (_U_(0x1) << PCC_IMR_OVRE_Pos)
#define PCC_IMR_MASK                _U_(0x00000003) /**< \brief (PCC_IMR) MASK Register */

/* -------- PCC_ISR : (PCC Offset: 0x10) (R/  32) Interrupt Status Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DRDY:1;           /*!< bit:      0  Data Ready Interrupt Status        */
    uint32_t OVRE:1;           /*!< bit:      1  Overrun Error Interrupt Status     */
    uint32_t :30;              /*!< bit:  2..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PCC_ISR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PCC_ISR_OFFSET              0x10         /**< \brief (PCC_ISR offset) Interrupt Status Register */
#define PCC_ISR_RESETVALUE          _U_(0x00000000) /**< \brief (PCC_ISR reset_value) Interrupt Status Register */

#define PCC_ISR_DRDY_Pos            0            /**< \brief (PCC_ISR) Data Ready Interrupt Status */
#define PCC_ISR_DRDY                (_U_(0x1) << PCC_ISR_DRDY_Pos)
#define PCC_ISR_OVRE_Pos            1            /**< \brief (PCC_ISR) Overrun Error Interrupt Status */
#define PCC_ISR_OVRE                (_U_(0x1) << PCC_ISR_OVRE_Pos)
#define PCC_ISR_MASK                _U_(0x00000003) /**< \brief (PCC_ISR) MASK Register */

/* -------- PCC_RHR : (PCC Offset: 0x14) (R/  32) Reception Holding Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RDATA:32;         /*!< bit:  0..31  Reception Data                     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PCC_RHR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PCC_RHR_OFFSET              0x14         /**< \brief (PCC_RHR offset) Reception Holding Register */
#define PCC_RHR_RESETVALUE          _U_(0x00000000) /**< \brief (PCC_RHR reset_value) Reception Holding Register */

#define PCC_RHR_RDATA_Pos           0            /**< \brief (PCC_RHR) Reception Data */
#define PCC_RHR_RDATA_Msk           (_U_(0xFFFFFFFF) << PCC_RHR_RDATA_Pos)
#define PCC_RHR_RDATA(value)        (PCC_RHR_RDATA_Msk & ((value) << PCC_RHR_RDATA_Pos))
#define PCC_RHR_MASK                _U_(0xFFFFFFFF) /**< \brief (PCC_RHR) MASK Register */

/* -------- PCC_WPMR : (PCC Offset: 0xE0) (R/W 32) Write Protection Mode Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t WPEN:1;           /*!< bit:      0  Write Protection Enable            */
    uint32_t :7;               /*!< bit:  1.. 7  Reserved                           */
    uint32_t WPKEY:24;         /*!< bit:  8..31  Write Protection Key               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PCC_WPMR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PCC_WPMR_OFFSET             0xE0         /**< \brief (PCC_WPMR offset) Write Protection Mode Register */
#define PCC_WPMR_RESETVALUE         _U_(0x00000000) /**< \brief (PCC_WPMR reset_value) Write Protection Mode Register */

#define PCC_WPMR_WPEN_Pos           0            /**< \brief (PCC_WPMR) Write Protection Enable */
#define PCC_WPMR_WPEN               (_U_(0x1) << PCC_WPMR_WPEN_Pos)
#define PCC_WPMR_WPKEY_Pos          8            /**< \brief (PCC_WPMR) Write Protection Key */
#define PCC_WPMR_WPKEY_Msk          (_U_(0xFFFFFF) << PCC_WPMR_WPKEY_Pos)
#define PCC_WPMR_WPKEY(value)       (PCC_WPMR_WPKEY_Msk & ((value) << PCC_WPMR_WPKEY_Pos))
#define PCC_WPMR_MASK               _U_(0xFFFFFF01) /**< \brief (PCC_WPMR) MASK Register */

/* -------- PCC_WPSR : (PCC Offset: 0xE4) (R/  32) Write Protection Status Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t WPVS:1;           /*!< bit:      0  Write Protection Violation Source  */
    uint32_t :7;               /*!< bit:  1.. 7  Reserved                           */
    uint32_t WPVSRC:16;        /*!< bit:  8..23  Write Protection Violation Status  */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PCC_WPSR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PCC_WPSR_OFFSET             0xE4         /**< \brief (PCC_WPSR offset) Write Protection Status Register */
#define PCC_WPSR_RESETVALUE         _U_(0x00000000) /**< \brief (PCC_WPSR reset_value) Write Protection Status Register */

#define PCC_WPSR_WPVS_Pos           0            /**< \brief (PCC_WPSR) Write Protection Violation Source */
#define PCC_WPSR_WPVS               (_U_(0x1) << PCC_WPSR_WPVS_Pos)
#define PCC_WPSR_WPVSRC_Pos         8            /**< \brief (PCC_WPSR) Write Protection Violation Status */
#define PCC_WPSR_WPVSRC_Msk         (_U_(0xFFFF) << PCC_WPSR_WPVSRC_Pos)
#define PCC_WPSR_WPVSRC(value)      (PCC_WPSR_WPVSRC_Msk & ((value) << PCC_WPSR_WPVSRC_Pos))
#define PCC_WPSR_MASK               _U_(0x00FFFF01) /**< \brief (PCC_WPSR) MASK Register */

/** \brief PCC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO PCC_MR_Type               MR;          /**< \brief Offset: 0x00 (R/W 32) Mode Register */
  __O  PCC_IER_Type              IER;         /**< \brief Offset: 0x04 ( /W 32) Interrupt Enable Register */
  __O  PCC_IDR_Type              IDR;         /**< \brief Offset: 0x08 ( /W 32) Interrupt Disable Register */
  __I  PCC_IMR_Type              IMR;         /**< \brief Offset: 0x0C (R/  32) Interrupt Mask Register */
  __I  PCC_ISR_Type              ISR;         /**< \brief Offset: 0x10 (R/  32) Interrupt Status Register */
  __I  PCC_RHR_Type              RHR;         /**< \brief Offset: 0x14 (R/  32) Reception Holding Register */
       RoReg8                    Reserved1[0xC8];
  __IO PCC_WPMR_Type             WPMR;        /**< \brief Offset: 0xE0 (R/W 32) Write Protection Mode Register */
  __I  PCC_WPSR_Type             WPSR;        /**< \brief Offset: 0xE4 (R/  32) Write Protection Status Register */
} Pcc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_PCC_COMPONENT_ */
