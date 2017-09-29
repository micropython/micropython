/**
 * \file
 *
 * \brief Component description for ICM
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

#ifndef _SAMD51_ICM_COMPONENT_
#define _SAMD51_ICM_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR ICM */
/* ========================================================================== */
/** \addtogroup SAMD51_ICM Integrity Check Monitor */
/*@{*/

#define ICM_U2010
#define REV_ICM                     0x120

/* -------- ICM_CFG : (ICM Offset: 0x00) (R/W 32) Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t WBDIS:1;          /*!< bit:      0  Write Back Disable                 */
    uint32_t EOMDIS:1;         /*!< bit:      1  End of Monitoring Disable          */
    uint32_t SLBDIS:1;         /*!< bit:      2  Secondary List Branching Disable   */
    uint32_t :1;               /*!< bit:      3  Reserved                           */
    uint32_t BBC:4;            /*!< bit:  4.. 7  Bus Burden Control                 */
    uint32_t ASCD:1;           /*!< bit:      8  Automatic Switch To Compare Digest */
    uint32_t DUALBUFF:1;       /*!< bit:      9  Dual Input Buffer                  */
    uint32_t :2;               /*!< bit: 10..11  Reserved                           */
    uint32_t UIHASH:1;         /*!< bit:     12  User Initial Hash Value            */
    uint32_t UALGO:3;          /*!< bit: 13..15  User SHA Algorithm                 */
    uint32_t HAPROT:6;         /*!< bit: 16..21  Region Hash Area Protection        */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t DAPROT:6;         /*!< bit: 24..29  Region Descriptor Area Protection  */
    uint32_t :2;               /*!< bit: 30..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} ICM_CFG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ICM_CFG_OFFSET              0x00         /**< \brief (ICM_CFG offset) Configuration */
#define ICM_CFG_RESETVALUE          _U_(0x00000000) /**< \brief (ICM_CFG reset_value) Configuration */

#define ICM_CFG_WBDIS_Pos           0            /**< \brief (ICM_CFG) Write Back Disable */
#define ICM_CFG_WBDIS               (_U_(0x1) << ICM_CFG_WBDIS_Pos)
#define ICM_CFG_EOMDIS_Pos          1            /**< \brief (ICM_CFG) End of Monitoring Disable */
#define ICM_CFG_EOMDIS              (_U_(0x1) << ICM_CFG_EOMDIS_Pos)
#define ICM_CFG_SLBDIS_Pos          2            /**< \brief (ICM_CFG) Secondary List Branching Disable */
#define ICM_CFG_SLBDIS              (_U_(0x1) << ICM_CFG_SLBDIS_Pos)
#define ICM_CFG_BBC_Pos             4            /**< \brief (ICM_CFG) Bus Burden Control */
#define ICM_CFG_BBC_Msk             (_U_(0xF) << ICM_CFG_BBC_Pos)
#define ICM_CFG_BBC(value)          (ICM_CFG_BBC_Msk & ((value) << ICM_CFG_BBC_Pos))
#define ICM_CFG_ASCD_Pos            8            /**< \brief (ICM_CFG) Automatic Switch To Compare Digest */
#define ICM_CFG_ASCD                (_U_(0x1) << ICM_CFG_ASCD_Pos)
#define ICM_CFG_DUALBUFF_Pos        9            /**< \brief (ICM_CFG) Dual Input Buffer */
#define ICM_CFG_DUALBUFF            (_U_(0x1) << ICM_CFG_DUALBUFF_Pos)
#define ICM_CFG_UIHASH_Pos          12           /**< \brief (ICM_CFG) User Initial Hash Value */
#define ICM_CFG_UIHASH              (_U_(0x1) << ICM_CFG_UIHASH_Pos)
#define ICM_CFG_UALGO_Pos           13           /**< \brief (ICM_CFG) User SHA Algorithm */
#define ICM_CFG_UALGO_Msk           (_U_(0x7) << ICM_CFG_UALGO_Pos)
#define ICM_CFG_UALGO(value)        (ICM_CFG_UALGO_Msk & ((value) << ICM_CFG_UALGO_Pos))
#define   ICM_CFG_UALGO_SHA1_Val          _U_(0x0)   /**< \brief (ICM_CFG) SHA1 Algorithm */
#define   ICM_CFG_UALGO_SHA256_Val        _U_(0x1)   /**< \brief (ICM_CFG) SHA256 Algorithm */
#define   ICM_CFG_UALGO_SHA224_Val        _U_(0x4)   /**< \brief (ICM_CFG) SHA224 Algorithm */
#define ICM_CFG_UALGO_SHA1          (ICM_CFG_UALGO_SHA1_Val        << ICM_CFG_UALGO_Pos)
#define ICM_CFG_UALGO_SHA256        (ICM_CFG_UALGO_SHA256_Val      << ICM_CFG_UALGO_Pos)
#define ICM_CFG_UALGO_SHA224        (ICM_CFG_UALGO_SHA224_Val      << ICM_CFG_UALGO_Pos)
#define ICM_CFG_HAPROT_Pos          16           /**< \brief (ICM_CFG) Region Hash Area Protection */
#define ICM_CFG_HAPROT_Msk          (_U_(0x3F) << ICM_CFG_HAPROT_Pos)
#define ICM_CFG_HAPROT(value)       (ICM_CFG_HAPROT_Msk & ((value) << ICM_CFG_HAPROT_Pos))
#define ICM_CFG_DAPROT_Pos          24           /**< \brief (ICM_CFG) Region Descriptor Area Protection */
#define ICM_CFG_DAPROT_Msk          (_U_(0x3F) << ICM_CFG_DAPROT_Pos)
#define ICM_CFG_DAPROT(value)       (ICM_CFG_DAPROT_Msk & ((value) << ICM_CFG_DAPROT_Pos))
#define ICM_CFG_MASK                _U_(0x3F3FF3F7) /**< \brief (ICM_CFG) MASK Register */

/* -------- ICM_CTRL : (ICM Offset: 0x04) ( /W 32) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ENABLE:1;         /*!< bit:      0  ICM Enable                         */
    uint32_t DISABLE:1;        /*!< bit:      1  ICM Disable Register               */
    uint32_t SWRST:1;          /*!< bit:      2  Software Reset                     */
    uint32_t :1;               /*!< bit:      3  Reserved                           */
    uint32_t REHASH:4;         /*!< bit:  4.. 7  Recompute Internal Hash            */
    uint32_t RMDIS:4;          /*!< bit:  8..11  Region Monitoring Disable          */
    uint32_t RMEN:4;           /*!< bit: 12..15  Region Monitoring Enable           */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} ICM_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ICM_CTRL_OFFSET             0x04         /**< \brief (ICM_CTRL offset) Control */

#define ICM_CTRL_ENABLE_Pos         0            /**< \brief (ICM_CTRL) ICM Enable */
#define ICM_CTRL_ENABLE             (_U_(0x1) << ICM_CTRL_ENABLE_Pos)
#define ICM_CTRL_DISABLE_Pos        1            /**< \brief (ICM_CTRL) ICM Disable Register */
#define ICM_CTRL_DISABLE            (_U_(0x1) << ICM_CTRL_DISABLE_Pos)
#define ICM_CTRL_SWRST_Pos          2            /**< \brief (ICM_CTRL) Software Reset */
#define ICM_CTRL_SWRST              (_U_(0x1) << ICM_CTRL_SWRST_Pos)
#define ICM_CTRL_REHASH_Pos         4            /**< \brief (ICM_CTRL) Recompute Internal Hash */
#define ICM_CTRL_REHASH_Msk         (_U_(0xF) << ICM_CTRL_REHASH_Pos)
#define ICM_CTRL_REHASH(value)      (ICM_CTRL_REHASH_Msk & ((value) << ICM_CTRL_REHASH_Pos))
#define ICM_CTRL_RMDIS_Pos          8            /**< \brief (ICM_CTRL) Region Monitoring Disable */
#define ICM_CTRL_RMDIS_Msk          (_U_(0xF) << ICM_CTRL_RMDIS_Pos)
#define ICM_CTRL_RMDIS(value)       (ICM_CTRL_RMDIS_Msk & ((value) << ICM_CTRL_RMDIS_Pos))
#define ICM_CTRL_RMEN_Pos           12           /**< \brief (ICM_CTRL) Region Monitoring Enable */
#define ICM_CTRL_RMEN_Msk           (_U_(0xF) << ICM_CTRL_RMEN_Pos)
#define ICM_CTRL_RMEN(value)        (ICM_CTRL_RMEN_Msk & ((value) << ICM_CTRL_RMEN_Pos))
#define ICM_CTRL_MASK               _U_(0x0000FFF7) /**< \brief (ICM_CTRL) MASK Register */

/* -------- ICM_SR : (ICM Offset: 0x08) (R/  32) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ENABLE:1;         /*!< bit:      0  ICM Controller Enable Register     */
    uint32_t :7;               /*!< bit:  1.. 7  Reserved                           */
    uint32_t RAWRMDIS:4;       /*!< bit:  8..11  RAW Region Monitoring Disabled Status */
    uint32_t RMDIS:4;          /*!< bit: 12..15  Region Monitoring Disabled Status  */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} ICM_SR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ICM_SR_OFFSET               0x08         /**< \brief (ICM_SR offset) Status */
#define ICM_SR_RESETVALUE           _U_(0x00000000) /**< \brief (ICM_SR reset_value) Status */

#define ICM_SR_ENABLE_Pos           0            /**< \brief (ICM_SR) ICM Controller Enable Register */
#define ICM_SR_ENABLE               (_U_(0x1) << ICM_SR_ENABLE_Pos)
#define ICM_SR_RAWRMDIS_Pos         8            /**< \brief (ICM_SR) RAW Region Monitoring Disabled Status */
#define ICM_SR_RAWRMDIS_Msk         (_U_(0xF) << ICM_SR_RAWRMDIS_Pos)
#define ICM_SR_RAWRMDIS(value)      (ICM_SR_RAWRMDIS_Msk & ((value) << ICM_SR_RAWRMDIS_Pos))
#define ICM_SR_RMDIS_Pos            12           /**< \brief (ICM_SR) Region Monitoring Disabled Status */
#define ICM_SR_RMDIS_Msk            (_U_(0xF) << ICM_SR_RMDIS_Pos)
#define ICM_SR_RMDIS(value)         (ICM_SR_RMDIS_Msk & ((value) << ICM_SR_RMDIS_Pos))
#define ICM_SR_MASK                 _U_(0x0000FF01) /**< \brief (ICM_SR) MASK Register */

/* -------- ICM_IER : (ICM Offset: 0x10) ( /W 32) Interrupt Enable -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RHC:4;            /*!< bit:  0.. 3  Region Hash Completed Interrupt Enable */
    uint32_t RDM:4;            /*!< bit:  4.. 7  Region Digest Mismatch Interrupt Enable */
    uint32_t RBE:4;            /*!< bit:  8..11  Region Bus Error Interrupt Enable  */
    uint32_t RWC:4;            /*!< bit: 12..15  Region Wrap Condition detected Interrupt Enable */
    uint32_t REC:4;            /*!< bit: 16..19  Region End bit Condition Detected Interrupt Enable */
    uint32_t RSU:4;            /*!< bit: 20..23  Region Status Updated Interrupt Disable */
    uint32_t URAD:1;           /*!< bit:     24  Undefined Register Access Detection Interrupt Enable */
    uint32_t :7;               /*!< bit: 25..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} ICM_IER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ICM_IER_OFFSET              0x10         /**< \brief (ICM_IER offset) Interrupt Enable */

#define ICM_IER_RHC_Pos             0            /**< \brief (ICM_IER) Region Hash Completed Interrupt Enable */
#define ICM_IER_RHC_Msk             (_U_(0xF) << ICM_IER_RHC_Pos)
#define ICM_IER_RHC(value)          (ICM_IER_RHC_Msk & ((value) << ICM_IER_RHC_Pos))
#define ICM_IER_RDM_Pos             4            /**< \brief (ICM_IER) Region Digest Mismatch Interrupt Enable */
#define ICM_IER_RDM_Msk             (_U_(0xF) << ICM_IER_RDM_Pos)
#define ICM_IER_RDM(value)          (ICM_IER_RDM_Msk & ((value) << ICM_IER_RDM_Pos))
#define ICM_IER_RBE_Pos             8            /**< \brief (ICM_IER) Region Bus Error Interrupt Enable */
#define ICM_IER_RBE_Msk             (_U_(0xF) << ICM_IER_RBE_Pos)
#define ICM_IER_RBE(value)          (ICM_IER_RBE_Msk & ((value) << ICM_IER_RBE_Pos))
#define ICM_IER_RWC_Pos             12           /**< \brief (ICM_IER) Region Wrap Condition detected Interrupt Enable */
#define ICM_IER_RWC_Msk             (_U_(0xF) << ICM_IER_RWC_Pos)
#define ICM_IER_RWC(value)          (ICM_IER_RWC_Msk & ((value) << ICM_IER_RWC_Pos))
#define ICM_IER_REC_Pos             16           /**< \brief (ICM_IER) Region End bit Condition Detected Interrupt Enable */
#define ICM_IER_REC_Msk             (_U_(0xF) << ICM_IER_REC_Pos)
#define ICM_IER_REC(value)          (ICM_IER_REC_Msk & ((value) << ICM_IER_REC_Pos))
#define ICM_IER_RSU_Pos             20           /**< \brief (ICM_IER) Region Status Updated Interrupt Disable */
#define ICM_IER_RSU_Msk             (_U_(0xF) << ICM_IER_RSU_Pos)
#define ICM_IER_RSU(value)          (ICM_IER_RSU_Msk & ((value) << ICM_IER_RSU_Pos))
#define ICM_IER_URAD_Pos            24           /**< \brief (ICM_IER) Undefined Register Access Detection Interrupt Enable */
#define ICM_IER_URAD                (_U_(0x1) << ICM_IER_URAD_Pos)
#define ICM_IER_MASK                _U_(0x01FFFFFF) /**< \brief (ICM_IER) MASK Register */

/* -------- ICM_IDR : (ICM Offset: 0x14) ( /W 32) Interrupt Disable -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RHC:4;            /*!< bit:  0.. 3  Region Hash Completed Interrupt Disable */
    uint32_t RDM:4;            /*!< bit:  4.. 7  Region Digest Mismatch Interrupt Disable */
    uint32_t RBE:4;            /*!< bit:  8..11  Region Bus Error Interrupt Disable */
    uint32_t RWC:4;            /*!< bit: 12..15  Region Wrap Condition Detected Interrupt Disable */
    uint32_t REC:4;            /*!< bit: 16..19  Region End bit Condition detected Interrupt Disable */
    uint32_t RSU:4;            /*!< bit: 20..23  Region Status Updated Interrupt Disable */
    uint32_t URAD:1;           /*!< bit:     24  Undefined Register Access Detection Interrupt Disable */
    uint32_t :7;               /*!< bit: 25..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} ICM_IDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ICM_IDR_OFFSET              0x14         /**< \brief (ICM_IDR offset) Interrupt Disable */
#define ICM_IDR_RESETVALUE          _U_(0x00000000) /**< \brief (ICM_IDR reset_value) Interrupt Disable */

#define ICM_IDR_RHC_Pos             0            /**< \brief (ICM_IDR) Region Hash Completed Interrupt Disable */
#define ICM_IDR_RHC_Msk             (_U_(0xF) << ICM_IDR_RHC_Pos)
#define ICM_IDR_RHC(value)          (ICM_IDR_RHC_Msk & ((value) << ICM_IDR_RHC_Pos))
#define ICM_IDR_RDM_Pos             4            /**< \brief (ICM_IDR) Region Digest Mismatch Interrupt Disable */
#define ICM_IDR_RDM_Msk             (_U_(0xF) << ICM_IDR_RDM_Pos)
#define ICM_IDR_RDM(value)          (ICM_IDR_RDM_Msk & ((value) << ICM_IDR_RDM_Pos))
#define ICM_IDR_RBE_Pos             8            /**< \brief (ICM_IDR) Region Bus Error Interrupt Disable */
#define ICM_IDR_RBE_Msk             (_U_(0xF) << ICM_IDR_RBE_Pos)
#define ICM_IDR_RBE(value)          (ICM_IDR_RBE_Msk & ((value) << ICM_IDR_RBE_Pos))
#define ICM_IDR_RWC_Pos             12           /**< \brief (ICM_IDR) Region Wrap Condition Detected Interrupt Disable */
#define ICM_IDR_RWC_Msk             (_U_(0xF) << ICM_IDR_RWC_Pos)
#define ICM_IDR_RWC(value)          (ICM_IDR_RWC_Msk & ((value) << ICM_IDR_RWC_Pos))
#define ICM_IDR_REC_Pos             16           /**< \brief (ICM_IDR) Region End bit Condition detected Interrupt Disable */
#define ICM_IDR_REC_Msk             (_U_(0xF) << ICM_IDR_REC_Pos)
#define ICM_IDR_REC(value)          (ICM_IDR_REC_Msk & ((value) << ICM_IDR_REC_Pos))
#define ICM_IDR_RSU_Pos             20           /**< \brief (ICM_IDR) Region Status Updated Interrupt Disable */
#define ICM_IDR_RSU_Msk             (_U_(0xF) << ICM_IDR_RSU_Pos)
#define ICM_IDR_RSU(value)          (ICM_IDR_RSU_Msk & ((value) << ICM_IDR_RSU_Pos))
#define ICM_IDR_URAD_Pos            24           /**< \brief (ICM_IDR) Undefined Register Access Detection Interrupt Disable */
#define ICM_IDR_URAD                (_U_(0x1) << ICM_IDR_URAD_Pos)
#define ICM_IDR_MASK                _U_(0x01FFFFFF) /**< \brief (ICM_IDR) MASK Register */

/* -------- ICM_IMR : (ICM Offset: 0x18) (R/  32) Interrupt Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RHC:4;            /*!< bit:  0.. 3  Region Hash Completed Interrupt Mask */
    uint32_t RDM:4;            /*!< bit:  4.. 7  Region Digest Mismatch Interrupt Mask */
    uint32_t RBE:4;            /*!< bit:  8..11  Region Bus Error Interrupt Mask    */
    uint32_t RWC:4;            /*!< bit: 12..15  Region Wrap Condition Detected Interrupt Mask */
    uint32_t REC:4;            /*!< bit: 16..19  Region End bit Condition Detected Interrupt Mask */
    uint32_t RSU:4;            /*!< bit: 20..23  Region Status Updated Interrupt Mask */
    uint32_t URAD:1;           /*!< bit:     24  Undefined Register Access Detection Interrupt Mask */
    uint32_t :7;               /*!< bit: 25..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} ICM_IMR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ICM_IMR_OFFSET              0x18         /**< \brief (ICM_IMR offset) Interrupt Mask */
#define ICM_IMR_RESETVALUE          _U_(0x00000000) /**< \brief (ICM_IMR reset_value) Interrupt Mask */

#define ICM_IMR_RHC_Pos             0            /**< \brief (ICM_IMR) Region Hash Completed Interrupt Mask */
#define ICM_IMR_RHC_Msk             (_U_(0xF) << ICM_IMR_RHC_Pos)
#define ICM_IMR_RHC(value)          (ICM_IMR_RHC_Msk & ((value) << ICM_IMR_RHC_Pos))
#define ICM_IMR_RDM_Pos             4            /**< \brief (ICM_IMR) Region Digest Mismatch Interrupt Mask */
#define ICM_IMR_RDM_Msk             (_U_(0xF) << ICM_IMR_RDM_Pos)
#define ICM_IMR_RDM(value)          (ICM_IMR_RDM_Msk & ((value) << ICM_IMR_RDM_Pos))
#define ICM_IMR_RBE_Pos             8            /**< \brief (ICM_IMR) Region Bus Error Interrupt Mask */
#define ICM_IMR_RBE_Msk             (_U_(0xF) << ICM_IMR_RBE_Pos)
#define ICM_IMR_RBE(value)          (ICM_IMR_RBE_Msk & ((value) << ICM_IMR_RBE_Pos))
#define ICM_IMR_RWC_Pos             12           /**< \brief (ICM_IMR) Region Wrap Condition Detected Interrupt Mask */
#define ICM_IMR_RWC_Msk             (_U_(0xF) << ICM_IMR_RWC_Pos)
#define ICM_IMR_RWC(value)          (ICM_IMR_RWC_Msk & ((value) << ICM_IMR_RWC_Pos))
#define ICM_IMR_REC_Pos             16           /**< \brief (ICM_IMR) Region End bit Condition Detected Interrupt Mask */
#define ICM_IMR_REC_Msk             (_U_(0xF) << ICM_IMR_REC_Pos)
#define ICM_IMR_REC(value)          (ICM_IMR_REC_Msk & ((value) << ICM_IMR_REC_Pos))
#define ICM_IMR_RSU_Pos             20           /**< \brief (ICM_IMR) Region Status Updated Interrupt Mask */
#define ICM_IMR_RSU_Msk             (_U_(0xF) << ICM_IMR_RSU_Pos)
#define ICM_IMR_RSU(value)          (ICM_IMR_RSU_Msk & ((value) << ICM_IMR_RSU_Pos))
#define ICM_IMR_URAD_Pos            24           /**< \brief (ICM_IMR) Undefined Register Access Detection Interrupt Mask */
#define ICM_IMR_URAD                (_U_(0x1) << ICM_IMR_URAD_Pos)
#define ICM_IMR_MASK                _U_(0x01FFFFFF) /**< \brief (ICM_IMR) MASK Register */

/* -------- ICM_ISR : (ICM Offset: 0x1C) (R/  32) Interrupt Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t RHC:4;            /*!< bit:  0.. 3  Region Hash Completed              */
    uint32_t RDM:4;            /*!< bit:  4.. 7  Region Digest Mismatch             */
    uint32_t RBE:4;            /*!< bit:  8..11  Region Bus Error                   */
    uint32_t RWC:4;            /*!< bit: 12..15  Region Wrap Condition Detected     */
    uint32_t REC:4;            /*!< bit: 16..19  Region End bit Condition Detected  */
    uint32_t RSU:4;            /*!< bit: 20..23  Region Status Updated Detected     */
    uint32_t URAD:1;           /*!< bit:     24  Undefined Register Access Detection Status */
    uint32_t :7;               /*!< bit: 25..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} ICM_ISR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ICM_ISR_OFFSET              0x1C         /**< \brief (ICM_ISR offset) Interrupt Status */
#define ICM_ISR_RESETVALUE          _U_(0x00000000) /**< \brief (ICM_ISR reset_value) Interrupt Status */

#define ICM_ISR_RHC_Pos             0            /**< \brief (ICM_ISR) Region Hash Completed */
#define ICM_ISR_RHC_Msk             (_U_(0xF) << ICM_ISR_RHC_Pos)
#define ICM_ISR_RHC(value)          (ICM_ISR_RHC_Msk & ((value) << ICM_ISR_RHC_Pos))
#define ICM_ISR_RDM_Pos             4            /**< \brief (ICM_ISR) Region Digest Mismatch */
#define ICM_ISR_RDM_Msk             (_U_(0xF) << ICM_ISR_RDM_Pos)
#define ICM_ISR_RDM(value)          (ICM_ISR_RDM_Msk & ((value) << ICM_ISR_RDM_Pos))
#define ICM_ISR_RBE_Pos             8            /**< \brief (ICM_ISR) Region Bus Error */
#define ICM_ISR_RBE_Msk             (_U_(0xF) << ICM_ISR_RBE_Pos)
#define ICM_ISR_RBE(value)          (ICM_ISR_RBE_Msk & ((value) << ICM_ISR_RBE_Pos))
#define ICM_ISR_RWC_Pos             12           /**< \brief (ICM_ISR) Region Wrap Condition Detected */
#define ICM_ISR_RWC_Msk             (_U_(0xF) << ICM_ISR_RWC_Pos)
#define ICM_ISR_RWC(value)          (ICM_ISR_RWC_Msk & ((value) << ICM_ISR_RWC_Pos))
#define ICM_ISR_REC_Pos             16           /**< \brief (ICM_ISR) Region End bit Condition Detected */
#define ICM_ISR_REC_Msk             (_U_(0xF) << ICM_ISR_REC_Pos)
#define ICM_ISR_REC(value)          (ICM_ISR_REC_Msk & ((value) << ICM_ISR_REC_Pos))
#define ICM_ISR_RSU_Pos             20           /**< \brief (ICM_ISR) Region Status Updated Detected */
#define ICM_ISR_RSU_Msk             (_U_(0xF) << ICM_ISR_RSU_Pos)
#define ICM_ISR_RSU(value)          (ICM_ISR_RSU_Msk & ((value) << ICM_ISR_RSU_Pos))
#define ICM_ISR_URAD_Pos            24           /**< \brief (ICM_ISR) Undefined Register Access Detection Status */
#define ICM_ISR_URAD                (_U_(0x1) << ICM_ISR_URAD_Pos)
#define ICM_ISR_MASK                _U_(0x01FFFFFF) /**< \brief (ICM_ISR) MASK Register */

/* -------- ICM_UASR : (ICM Offset: 0x20) (R/  32) Undefined Access Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t URAT:3;           /*!< bit:  0.. 2  Undefined Register Access Trace    */
    uint32_t :29;              /*!< bit:  3..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} ICM_UASR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ICM_UASR_OFFSET             0x20         /**< \brief (ICM_UASR offset) Undefined Access Status */
#define ICM_UASR_RESETVALUE         _U_(0x00000000) /**< \brief (ICM_UASR reset_value) Undefined Access Status */

#define ICM_UASR_URAT_Pos           0            /**< \brief (ICM_UASR) Undefined Register Access Trace */
#define ICM_UASR_URAT_Msk           (_U_(0x7) << ICM_UASR_URAT_Pos)
#define ICM_UASR_URAT(value)        (ICM_UASR_URAT_Msk & ((value) << ICM_UASR_URAT_Pos))
#define   ICM_UASR_URAT_UNSPEC_STRUCT_MEMBER_Val _U_(0x0)   /**< \brief (ICM_UASR) Unspecified structure member set to one detected when the descriptor is loaded */
#define   ICM_UASR_URAT_CFG_MODIFIED_Val  _U_(0x1)   /**< \brief (ICM_UASR) CFG modified during active monitoring */
#define   ICM_UASR_URAT_DSCR_MODIFIED_Val _U_(0x2)   /**< \brief (ICM_UASR) DSCR modified during active monitoring */
#define   ICM_UASR_URAT_HASH_MODIFIED_Val _U_(0x3)   /**< \brief (ICM_UASR) HASH modified during active monitoring */
#define   ICM_UASR_URAT_READ_ACCESS_Val   _U_(0x4)   /**< \brief (ICM_UASR) Write-only register read access */
#define ICM_UASR_URAT_UNSPEC_STRUCT_MEMBER (ICM_UASR_URAT_UNSPEC_STRUCT_MEMBER_Val << ICM_UASR_URAT_Pos)
#define ICM_UASR_URAT_CFG_MODIFIED  (ICM_UASR_URAT_CFG_MODIFIED_Val << ICM_UASR_URAT_Pos)
#define ICM_UASR_URAT_DSCR_MODIFIED (ICM_UASR_URAT_DSCR_MODIFIED_Val << ICM_UASR_URAT_Pos)
#define ICM_UASR_URAT_HASH_MODIFIED (ICM_UASR_URAT_HASH_MODIFIED_Val << ICM_UASR_URAT_Pos)
#define ICM_UASR_URAT_READ_ACCESS   (ICM_UASR_URAT_READ_ACCESS_Val << ICM_UASR_URAT_Pos)
#define ICM_UASR_MASK               _U_(0x00000007) /**< \brief (ICM_UASR) MASK Register */

/* -------- ICM_DSCR : (ICM Offset: 0x30) (R/W 32) Region Descriptor Area Start Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :6;               /*!< bit:  0.. 5  Reserved                           */
    uint32_t DASA:26;          /*!< bit:  6..31  Descriptor Area Start Address      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} ICM_DSCR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ICM_DSCR_OFFSET             0x30         /**< \brief (ICM_DSCR offset) Region Descriptor Area Start Address */
#define ICM_DSCR_RESETVALUE         _U_(0x00000000) /**< \brief (ICM_DSCR reset_value) Region Descriptor Area Start Address */

#define ICM_DSCR_DASA_Pos           6            /**< \brief (ICM_DSCR) Descriptor Area Start Address */
#define ICM_DSCR_DASA_Msk           (_U_(0x3FFFFFF) << ICM_DSCR_DASA_Pos)
#define ICM_DSCR_DASA(value)        (ICM_DSCR_DASA_Msk & ((value) << ICM_DSCR_DASA_Pos))
#define ICM_DSCR_MASK               _U_(0xFFFFFFC0) /**< \brief (ICM_DSCR) MASK Register */

/* -------- ICM_HASH : (ICM Offset: 0x34) (R/W 32) Region Hash Area Start Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :7;               /*!< bit:  0.. 6  Reserved                           */
    uint32_t HASA:25;          /*!< bit:  7..31  Hash Area Start Address            */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} ICM_HASH_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ICM_HASH_OFFSET             0x34         /**< \brief (ICM_HASH offset) Region Hash Area Start Address */
#define ICM_HASH_RESETVALUE         _U_(0x00000000) /**< \brief (ICM_HASH reset_value) Region Hash Area Start Address */

#define ICM_HASH_HASA_Pos           7            /**< \brief (ICM_HASH) Hash Area Start Address */
#define ICM_HASH_HASA_Msk           (_U_(0x1FFFFFF) << ICM_HASH_HASA_Pos)
#define ICM_HASH_HASA(value)        (ICM_HASH_HASA_Msk & ((value) << ICM_HASH_HASA_Pos))
#define ICM_HASH_MASK               _U_(0xFFFFFF80) /**< \brief (ICM_HASH) MASK Register */

/* -------- ICM_UIHVAL : (ICM Offset: 0x38) ( /W 32) User Initial Hash Value n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t VAL:32;           /*!< bit:  0..31  Initial Hash Value                 */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} ICM_UIHVAL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ICM_UIHVAL_OFFSET           0x38         /**< \brief (ICM_UIHVAL offset) User Initial Hash Value n */
#define ICM_UIHVAL_RESETVALUE       _U_(0x00000000) /**< \brief (ICM_UIHVAL reset_value) User Initial Hash Value n */

#define ICM_UIHVAL_VAL_Pos          0            /**< \brief (ICM_UIHVAL) Initial Hash Value */
#define ICM_UIHVAL_VAL_Msk          (_U_(0xFFFFFFFF) << ICM_UIHVAL_VAL_Pos)
#define ICM_UIHVAL_VAL(value)       (ICM_UIHVAL_VAL_Msk & ((value) << ICM_UIHVAL_VAL_Pos))
#define ICM_UIHVAL_MASK             _U_(0xFFFFFFFF) /**< \brief (ICM_UIHVAL) MASK Register */

/* -------- ICM_RADDR : (ICM Offset: 0x00) (R/W 32) Region Start Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} ICM_RADDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ICM_RADDR_OFFSET            0x00         /**< \brief (ICM_RADDR offset) Region Start Address */
#define ICM_RADDR_MASK              _U_(0xFFFFFFFF) /**< \brief (ICM_RADDR) MASK Register */

/* -------- ICM_RCFG : (ICM Offset: 0x04) (R/W 32) Region Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CDWBN:1;          /*!< bit:      0  Compare Digest Write Back          */
    uint32_t WRAP:1;           /*!< bit:      1  Region Wrap                        */
    uint32_t EOM:1;            /*!< bit:      2  End of Monitoring                  */
    uint32_t :1;               /*!< bit:      3  Reserved                           */
    uint32_t RHIEN:1;          /*!< bit:      4  Region Hash Interrupt Enable       */
    uint32_t DMIEN:1;          /*!< bit:      5  Region Digest Mismatch Interrupt Enable */
    uint32_t BEIEN:1;          /*!< bit:      6  Region Bus Error Interrupt Enable  */
    uint32_t WCIEN:1;          /*!< bit:      7  Region Wrap Condition Detected Interrupt Enable */
    uint32_t ECIEN:1;          /*!< bit:      8  Region End bit Condition detected Interrupt Enable */
    uint32_t SUIEN:1;          /*!< bit:      9  Region Status Updated Interrupt Enable */
    uint32_t PROCDLY:1;        /*!< bit:     10  SHA Processing Delay               */
    uint32_t :1;               /*!< bit:     11  Reserved                           */
    uint32_t ALGO:3;           /*!< bit: 12..14  SHA Algorithm                      */
    uint32_t :9;               /*!< bit: 15..23  Reserved                           */
    uint32_t MRPROT:6;         /*!< bit: 24..29  Memory Region AHB Protection       */
    uint32_t :2;               /*!< bit: 30..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} ICM_RCFG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ICM_RCFG_OFFSET             0x04         /**< \brief (ICM_RCFG offset) Region Configuration */
#define ICM_RCFG_RESETVALUE         _U_(0x00000000) /**< \brief (ICM_RCFG reset_value) Region Configuration */

#define ICM_RCFG_CDWBN_Pos          0            /**< \brief (ICM_RCFG) Compare Digest Write Back */
#define ICM_RCFG_CDWBN              (_U_(0x1) << ICM_RCFG_CDWBN_Pos)
#define   ICM_RCFG_CDWBN_WRBA_Val         _U_(0x0)   /**< \brief (ICM_RCFG)  */
#define   ICM_RCFG_CDWBN_COMP_Val         _U_(0x1)   /**< \brief (ICM_RCFG)  */
#define ICM_RCFG_CDWBN_WRBA         (ICM_RCFG_CDWBN_WRBA_Val       << ICM_RCFG_CDWBN_Pos)
#define ICM_RCFG_CDWBN_COMP         (ICM_RCFG_CDWBN_COMP_Val       << ICM_RCFG_CDWBN_Pos)
#define ICM_RCFG_WRAP_Pos           1            /**< \brief (ICM_RCFG) Region Wrap */
#define ICM_RCFG_WRAP               (_U_(0x1) << ICM_RCFG_WRAP_Pos)
#define   ICM_RCFG_WRAP_NO_Val            _U_(0x0)   /**< \brief (ICM_RCFG)  */
#define   ICM_RCFG_WRAP_YES_Val           _U_(0x1)   /**< \brief (ICM_RCFG)  */
#define ICM_RCFG_WRAP_NO            (ICM_RCFG_WRAP_NO_Val          << ICM_RCFG_WRAP_Pos)
#define ICM_RCFG_WRAP_YES           (ICM_RCFG_WRAP_YES_Val         << ICM_RCFG_WRAP_Pos)
#define ICM_RCFG_EOM_Pos            2            /**< \brief (ICM_RCFG) End of Monitoring */
#define ICM_RCFG_EOM                (_U_(0x1) << ICM_RCFG_EOM_Pos)
#define   ICM_RCFG_EOM_NO_Val             _U_(0x0)   /**< \brief (ICM_RCFG)  */
#define   ICM_RCFG_EOM_YES_Val            _U_(0x1)   /**< \brief (ICM_RCFG)  */
#define ICM_RCFG_EOM_NO             (ICM_RCFG_EOM_NO_Val           << ICM_RCFG_EOM_Pos)
#define ICM_RCFG_EOM_YES            (ICM_RCFG_EOM_YES_Val          << ICM_RCFG_EOM_Pos)
#define ICM_RCFG_RHIEN_Pos          4            /**< \brief (ICM_RCFG) Region Hash Interrupt Enable */
#define ICM_RCFG_RHIEN              (_U_(0x1) << ICM_RCFG_RHIEN_Pos)
#define   ICM_RCFG_RHIEN_EN_Val           _U_(0x0)   /**< \brief (ICM_RCFG)  */
#define   ICM_RCFG_RHIEN_DIS_Val          _U_(0x1)   /**< \brief (ICM_RCFG)  */
#define ICM_RCFG_RHIEN_EN           (ICM_RCFG_RHIEN_EN_Val         << ICM_RCFG_RHIEN_Pos)
#define ICM_RCFG_RHIEN_DIS          (ICM_RCFG_RHIEN_DIS_Val        << ICM_RCFG_RHIEN_Pos)
#define ICM_RCFG_DMIEN_Pos          5            /**< \brief (ICM_RCFG) Region Digest Mismatch Interrupt Enable */
#define ICM_RCFG_DMIEN              (_U_(0x1) << ICM_RCFG_DMIEN_Pos)
#define   ICM_RCFG_DMIEN_EN_Val           _U_(0x0)   /**< \brief (ICM_RCFG)  */
#define   ICM_RCFG_DMIEN_DIS_Val          _U_(0x1)   /**< \brief (ICM_RCFG)  */
#define ICM_RCFG_DMIEN_EN           (ICM_RCFG_DMIEN_EN_Val         << ICM_RCFG_DMIEN_Pos)
#define ICM_RCFG_DMIEN_DIS          (ICM_RCFG_DMIEN_DIS_Val        << ICM_RCFG_DMIEN_Pos)
#define ICM_RCFG_BEIEN_Pos          6            /**< \brief (ICM_RCFG) Region Bus Error Interrupt Enable */
#define ICM_RCFG_BEIEN              (_U_(0x1) << ICM_RCFG_BEIEN_Pos)
#define   ICM_RCFG_BEIEN_EN_Val           _U_(0x0)   /**< \brief (ICM_RCFG)  */
#define   ICM_RCFG_BEIEN_DIS_Val          _U_(0x1)   /**< \brief (ICM_RCFG)  */
#define ICM_RCFG_BEIEN_EN           (ICM_RCFG_BEIEN_EN_Val         << ICM_RCFG_BEIEN_Pos)
#define ICM_RCFG_BEIEN_DIS          (ICM_RCFG_BEIEN_DIS_Val        << ICM_RCFG_BEIEN_Pos)
#define ICM_RCFG_WCIEN_Pos          7            /**< \brief (ICM_RCFG) Region Wrap Condition Detected Interrupt Enable */
#define ICM_RCFG_WCIEN              (_U_(0x1) << ICM_RCFG_WCIEN_Pos)
#define   ICM_RCFG_WCIEN_EN_Val           _U_(0x0)   /**< \brief (ICM_RCFG)  */
#define   ICM_RCFG_WCIEN_DIS_Val          _U_(0x1)   /**< \brief (ICM_RCFG)  */
#define ICM_RCFG_WCIEN_EN           (ICM_RCFG_WCIEN_EN_Val         << ICM_RCFG_WCIEN_Pos)
#define ICM_RCFG_WCIEN_DIS          (ICM_RCFG_WCIEN_DIS_Val        << ICM_RCFG_WCIEN_Pos)
#define ICM_RCFG_ECIEN_Pos          8            /**< \brief (ICM_RCFG) Region End bit Condition detected Interrupt Enable */
#define ICM_RCFG_ECIEN              (_U_(0x1) << ICM_RCFG_ECIEN_Pos)
#define   ICM_RCFG_ECIEN_EN_Val           _U_(0x0)   /**< \brief (ICM_RCFG)  */
#define   ICM_RCFG_ECIEN_DIS_Val          _U_(0x1)   /**< \brief (ICM_RCFG)  */
#define ICM_RCFG_ECIEN_EN           (ICM_RCFG_ECIEN_EN_Val         << ICM_RCFG_ECIEN_Pos)
#define ICM_RCFG_ECIEN_DIS          (ICM_RCFG_ECIEN_DIS_Val        << ICM_RCFG_ECIEN_Pos)
#define ICM_RCFG_SUIEN_Pos          9            /**< \brief (ICM_RCFG) Region Status Updated Interrupt Enable */
#define ICM_RCFG_SUIEN              (_U_(0x1) << ICM_RCFG_SUIEN_Pos)
#define   ICM_RCFG_SUIEN_EN_Val           _U_(0x0)   /**< \brief (ICM_RCFG)  */
#define   ICM_RCFG_SUIEN_DIS_Val          _U_(0x1)   /**< \brief (ICM_RCFG)  */
#define ICM_RCFG_SUIEN_EN           (ICM_RCFG_SUIEN_EN_Val         << ICM_RCFG_SUIEN_Pos)
#define ICM_RCFG_SUIEN_DIS          (ICM_RCFG_SUIEN_DIS_Val        << ICM_RCFG_SUIEN_Pos)
#define ICM_RCFG_PROCDLY_Pos        10           /**< \brief (ICM_RCFG) SHA Processing Delay */
#define ICM_RCFG_PROCDLY            (_U_(0x1) << ICM_RCFG_PROCDLY_Pos)
#define   ICM_RCFG_PROCDLY_SHORT_Val      _U_(0x0)   /**< \brief (ICM_RCFG)  */
#define   ICM_RCFG_PROCDLY_LONG_Val       _U_(0x1)   /**< \brief (ICM_RCFG)  */
#define ICM_RCFG_PROCDLY_SHORT      (ICM_RCFG_PROCDLY_SHORT_Val    << ICM_RCFG_PROCDLY_Pos)
#define ICM_RCFG_PROCDLY_LONG       (ICM_RCFG_PROCDLY_LONG_Val     << ICM_RCFG_PROCDLY_Pos)
#define ICM_RCFG_ALGO_Pos           12           /**< \brief (ICM_RCFG) SHA Algorithm */
#define ICM_RCFG_ALGO_Msk           (_U_(0x7) << ICM_RCFG_ALGO_Pos)
#define ICM_RCFG_ALGO(value)        (ICM_RCFG_ALGO_Msk & ((value) << ICM_RCFG_ALGO_Pos))
#define ICM_RCFG_MRPROT_Pos         24           /**< \brief (ICM_RCFG) Memory Region AHB Protection */
#define ICM_RCFG_MRPROT_Msk         (_U_(0x3F) << ICM_RCFG_MRPROT_Pos)
#define ICM_RCFG_MRPROT(value)      (ICM_RCFG_MRPROT_Msk & ((value) << ICM_RCFG_MRPROT_Pos))
#define ICM_RCFG_MASK               _U_(0x3F0077F7) /**< \brief (ICM_RCFG) MASK Register */

/* -------- ICM_RCTRL : (ICM Offset: 0x08) (R/W 32) Region Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t TRSIZE:16;        /*!< bit:  0..15  Transfer Size                      */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} ICM_RCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ICM_RCTRL_OFFSET            0x08         /**< \brief (ICM_RCTRL offset) Region Control */

#define ICM_RCTRL_TRSIZE_Pos        0            /**< \brief (ICM_RCTRL) Transfer Size */
#define ICM_RCTRL_TRSIZE_Msk        (_U_(0xFFFF) << ICM_RCTRL_TRSIZE_Pos)
#define ICM_RCTRL_TRSIZE(value)     (ICM_RCTRL_TRSIZE_Msk & ((value) << ICM_RCTRL_TRSIZE_Pos))
#define ICM_RCTRL_MASK              _U_(0x0000FFFF) /**< \brief (ICM_RCTRL) MASK Register */

/* -------- ICM_RNEXT : (ICM Offset: 0x0C) (R/W 32) Region Next Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} ICM_RNEXT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ICM_RNEXT_OFFSET            0x0C         /**< \brief (ICM_RNEXT offset) Region Next Address */
#define ICM_RNEXT_MASK              _U_(0xFFFFFFFF) /**< \brief (ICM_RNEXT) MASK Register */

/** \brief ICM APB hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO ICM_CFG_Type              CFG;         /**< \brief Offset: 0x00 (R/W 32) Configuration */
  __O  ICM_CTRL_Type             CTRL;        /**< \brief Offset: 0x04 ( /W 32) Control */
  __I  ICM_SR_Type               SR;          /**< \brief Offset: 0x08 (R/  32) Status */
       RoReg8                    Reserved1[0x4];
  __O  ICM_IER_Type              IER;         /**< \brief Offset: 0x10 ( /W 32) Interrupt Enable */
  __O  ICM_IDR_Type              IDR;         /**< \brief Offset: 0x14 ( /W 32) Interrupt Disable */
  __I  ICM_IMR_Type              IMR;         /**< \brief Offset: 0x18 (R/  32) Interrupt Mask */
  __I  ICM_ISR_Type              ISR;         /**< \brief Offset: 0x1C (R/  32) Interrupt Status */
  __I  ICM_UASR_Type             UASR;        /**< \brief Offset: 0x20 (R/  32) Undefined Access Status */
       RoReg8                    Reserved2[0xC];
  __IO ICM_DSCR_Type             DSCR;        /**< \brief Offset: 0x30 (R/W 32) Region Descriptor Area Start Address */
  __IO ICM_HASH_Type             HASH;        /**< \brief Offset: 0x34 (R/W 32) Region Hash Area Start Address */
  __O  ICM_UIHVAL_Type           UIHVAL[8];   /**< \brief Offset: 0x38 ( /W 32) User Initial Hash Value n */
} Icm;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief ICM Descriptor SRAM registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO ICM_RADDR_Type            RADDR;       /**< \brief Offset: 0x00 (R/W 32) Region Start Address */
  __IO ICM_RCFG_Type             RCFG;        /**< \brief Offset: 0x04 (R/W 32) Region Configuration */
  __IO ICM_RCTRL_Type            RCTRL;       /**< \brief Offset: 0x08 (R/W 32) Region Control */
  __IO ICM_RNEXT_Type            RNEXT;       /**< \brief Offset: 0x0C (R/W 32) Region Next Address */
} IcmDescriptor;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SECTION_ICM_DESCRIPTOR

/*@}*/

#endif /* _SAMD51_ICM_COMPONENT_ */
