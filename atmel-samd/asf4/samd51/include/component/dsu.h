/**
 * \file
 *
 * \brief Component description for DSU
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

#ifndef _SAMD51_DSU_COMPONENT_
#define _SAMD51_DSU_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR DSU */
/* ========================================================================== */
/** \addtogroup SAMD51_DSU Device Service Unit */
/*@{*/

#define DSU_U2410
#define REV_DSU                     0x100

/* -------- DSU_CTRL : (DSU Offset: 0x0000) ( /W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  :1;               /*!< bit:      1  Reserved                           */
    uint8_t  CRC:1;            /*!< bit:      2  32-bit Cyclic Redundancy Code      */
    uint8_t  MBIST:1;          /*!< bit:      3  Memory built-in self-test          */
    uint8_t  CE:1;             /*!< bit:      4  Chip-Erase                         */
    uint8_t  :1;               /*!< bit:      5  Reserved                           */
    uint8_t  ARR:1;            /*!< bit:      6  Auxiliary Row Read                 */
    uint8_t  SMSA:1;           /*!< bit:      7  Start Memory Stream Access         */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DSU_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_CTRL_OFFSET             0x0000       /**< \brief (DSU_CTRL offset) Control */
#define DSU_CTRL_RESETVALUE         _U_(0x00)    /**< \brief (DSU_CTRL reset_value) Control */

#define DSU_CTRL_SWRST_Pos          0            /**< \brief (DSU_CTRL) Software Reset */
#define DSU_CTRL_SWRST              (_U_(0x1) << DSU_CTRL_SWRST_Pos)
#define DSU_CTRL_CRC_Pos            2            /**< \brief (DSU_CTRL) 32-bit Cyclic Redundancy Code */
#define DSU_CTRL_CRC                (_U_(0x1) << DSU_CTRL_CRC_Pos)
#define DSU_CTRL_MBIST_Pos          3            /**< \brief (DSU_CTRL) Memory built-in self-test */
#define DSU_CTRL_MBIST              (_U_(0x1) << DSU_CTRL_MBIST_Pos)
#define DSU_CTRL_CE_Pos             4            /**< \brief (DSU_CTRL) Chip-Erase */
#define DSU_CTRL_CE                 (_U_(0x1) << DSU_CTRL_CE_Pos)
#define DSU_CTRL_ARR_Pos            6            /**< \brief (DSU_CTRL) Auxiliary Row Read */
#define DSU_CTRL_ARR                (_U_(0x1) << DSU_CTRL_ARR_Pos)
#define DSU_CTRL_SMSA_Pos           7            /**< \brief (DSU_CTRL) Start Memory Stream Access */
#define DSU_CTRL_SMSA               (_U_(0x1) << DSU_CTRL_SMSA_Pos)
#define DSU_CTRL_MASK               _U_(0xDD)    /**< \brief (DSU_CTRL) MASK Register */

/* -------- DSU_STATUSA : (DSU Offset: 0x0001) (R/W  8) Status A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DONE:1;           /*!< bit:      0  Done                               */
    uint8_t  CRSTEXT:1;        /*!< bit:      1  CPU Reset Phase Extension          */
    uint8_t  BERR:1;           /*!< bit:      2  Bus Error                          */
    uint8_t  FAIL:1;           /*!< bit:      3  Failure                            */
    uint8_t  PERR:1;           /*!< bit:      4  Protection Error                   */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DSU_STATUSA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_STATUSA_OFFSET          0x0001       /**< \brief (DSU_STATUSA offset) Status A */
#define DSU_STATUSA_RESETVALUE      _U_(0x00)    /**< \brief (DSU_STATUSA reset_value) Status A */

#define DSU_STATUSA_DONE_Pos        0            /**< \brief (DSU_STATUSA) Done */
#define DSU_STATUSA_DONE            (_U_(0x1) << DSU_STATUSA_DONE_Pos)
#define DSU_STATUSA_CRSTEXT_Pos     1            /**< \brief (DSU_STATUSA) CPU Reset Phase Extension */
#define DSU_STATUSA_CRSTEXT         (_U_(0x1) << DSU_STATUSA_CRSTEXT_Pos)
#define DSU_STATUSA_BERR_Pos        2            /**< \brief (DSU_STATUSA) Bus Error */
#define DSU_STATUSA_BERR            (_U_(0x1) << DSU_STATUSA_BERR_Pos)
#define DSU_STATUSA_FAIL_Pos        3            /**< \brief (DSU_STATUSA) Failure */
#define DSU_STATUSA_FAIL            (_U_(0x1) << DSU_STATUSA_FAIL_Pos)
#define DSU_STATUSA_PERR_Pos        4            /**< \brief (DSU_STATUSA) Protection Error */
#define DSU_STATUSA_PERR            (_U_(0x1) << DSU_STATUSA_PERR_Pos)
#define DSU_STATUSA_MASK            _U_(0x1F)    /**< \brief (DSU_STATUSA) MASK Register */

/* -------- DSU_STATUSB : (DSU Offset: 0x0002) (R/   8) Status B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  PROT:1;           /*!< bit:      0  Protected                          */
    uint8_t  DBGPRES:1;        /*!< bit:      1  Debugger Present                   */
    uint8_t  DCCD0:1;          /*!< bit:      2  Debug Communication Channel 0 Dirty */
    uint8_t  DCCD1:1;          /*!< bit:      3  Debug Communication Channel 1 Dirty */
    uint8_t  HPE:1;            /*!< bit:      4  Hot-Plugging Enable                */
    uint8_t  CELCK:1;          /*!< bit:      5  Chip Erase Locked                  */
    uint8_t  TDCCD0:1;         /*!< bit:      6  Test Debug Communication Channel 0 Dirty */
    uint8_t  TDCCD1:1;         /*!< bit:      7  Test Debug Communication Channel 1 Dirty */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  :2;               /*!< bit:  0.. 1  Reserved                           */
    uint8_t  DCCD:2;           /*!< bit:  2.. 3  Debug Communication Channel x Dirty */
    uint8_t  :2;               /*!< bit:  4.. 5  Reserved                           */
    uint8_t  TDCCD:2;          /*!< bit:  6.. 7  Test Debug Communication Channel x Dirty */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DSU_STATUSB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_STATUSB_OFFSET          0x0002       /**< \brief (DSU_STATUSB offset) Status B */
#define DSU_STATUSB_RESETVALUE      _U_(0x00)    /**< \brief (DSU_STATUSB reset_value) Status B */

#define DSU_STATUSB_PROT_Pos        0            /**< \brief (DSU_STATUSB) Protected */
#define DSU_STATUSB_PROT            (_U_(0x1) << DSU_STATUSB_PROT_Pos)
#define DSU_STATUSB_DBGPRES_Pos     1            /**< \brief (DSU_STATUSB) Debugger Present */
#define DSU_STATUSB_DBGPRES         (_U_(0x1) << DSU_STATUSB_DBGPRES_Pos)
#define DSU_STATUSB_DCCD0_Pos       2            /**< \brief (DSU_STATUSB) Debug Communication Channel 0 Dirty */
#define DSU_STATUSB_DCCD0           (_U_(1) << DSU_STATUSB_DCCD0_Pos)
#define DSU_STATUSB_DCCD1_Pos       3            /**< \brief (DSU_STATUSB) Debug Communication Channel 1 Dirty */
#define DSU_STATUSB_DCCD1           (_U_(1) << DSU_STATUSB_DCCD1_Pos)
#define DSU_STATUSB_DCCD_Pos        2            /**< \brief (DSU_STATUSB) Debug Communication Channel x Dirty */
#define DSU_STATUSB_DCCD_Msk        (_U_(0x3) << DSU_STATUSB_DCCD_Pos)
#define DSU_STATUSB_DCCD(value)     (DSU_STATUSB_DCCD_Msk & ((value) << DSU_STATUSB_DCCD_Pos))
#define DSU_STATUSB_HPE_Pos         4            /**< \brief (DSU_STATUSB) Hot-Plugging Enable */
#define DSU_STATUSB_HPE             (_U_(0x1) << DSU_STATUSB_HPE_Pos)
#define DSU_STATUSB_CELCK_Pos       5            /**< \brief (DSU_STATUSB) Chip Erase Locked */
#define DSU_STATUSB_CELCK           (_U_(0x1) << DSU_STATUSB_CELCK_Pos)
#define DSU_STATUSB_TDCCD0_Pos      6            /**< \brief (DSU_STATUSB) Test Debug Communication Channel 0 Dirty */
#define DSU_STATUSB_TDCCD0          (_U_(1) << DSU_STATUSB_TDCCD0_Pos)
#define DSU_STATUSB_TDCCD1_Pos      7            /**< \brief (DSU_STATUSB) Test Debug Communication Channel 1 Dirty */
#define DSU_STATUSB_TDCCD1          (_U_(1) << DSU_STATUSB_TDCCD1_Pos)
#define DSU_STATUSB_TDCCD_Pos       6            /**< \brief (DSU_STATUSB) Test Debug Communication Channel x Dirty */
#define DSU_STATUSB_TDCCD_Msk       (_U_(0x3) << DSU_STATUSB_TDCCD_Pos)
#define DSU_STATUSB_TDCCD(value)    (DSU_STATUSB_TDCCD_Msk & ((value) << DSU_STATUSB_TDCCD_Pos))
#define DSU_STATUSB_MASK            _U_(0xFF)    /**< \brief (DSU_STATUSB) MASK Register */

/* -------- DSU_ADDR : (DSU Offset: 0x0004) (R/W 32) Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t AMOD:2;           /*!< bit:  0.. 1  Access Mode                        */
    uint32_t ADDR:30;          /*!< bit:  2..31  Address                            */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_ADDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_ADDR_OFFSET             0x0004       /**< \brief (DSU_ADDR offset) Address */
#define DSU_ADDR_RESETVALUE         _U_(0x00000000) /**< \brief (DSU_ADDR reset_value) Address */

#define DSU_ADDR_AMOD_Pos           0            /**< \brief (DSU_ADDR) Access Mode */
#define DSU_ADDR_AMOD_Msk           (_U_(0x3) << DSU_ADDR_AMOD_Pos)
#define DSU_ADDR_AMOD(value)        (DSU_ADDR_AMOD_Msk & ((value) << DSU_ADDR_AMOD_Pos))
#define DSU_ADDR_ADDR_Pos           2            /**< \brief (DSU_ADDR) Address */
#define DSU_ADDR_ADDR_Msk           (_U_(0x3FFFFFFF) << DSU_ADDR_ADDR_Pos)
#define DSU_ADDR_ADDR(value)        (DSU_ADDR_ADDR_Msk & ((value) << DSU_ADDR_ADDR_Pos))
#define DSU_ADDR_MASK               _U_(0xFFFFFFFF) /**< \brief (DSU_ADDR) MASK Register */

/* -------- DSU_LENGTH : (DSU Offset: 0x0008) (R/W 32) Length -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :2;               /*!< bit:  0.. 1  Reserved                           */
    uint32_t LENGTH:30;        /*!< bit:  2..31  Length                             */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_LENGTH_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_LENGTH_OFFSET           0x0008       /**< \brief (DSU_LENGTH offset) Length */
#define DSU_LENGTH_RESETVALUE       _U_(0x00000000) /**< \brief (DSU_LENGTH reset_value) Length */

#define DSU_LENGTH_LENGTH_Pos       2            /**< \brief (DSU_LENGTH) Length */
#define DSU_LENGTH_LENGTH_Msk       (_U_(0x3FFFFFFF) << DSU_LENGTH_LENGTH_Pos)
#define DSU_LENGTH_LENGTH(value)    (DSU_LENGTH_LENGTH_Msk & ((value) << DSU_LENGTH_LENGTH_Pos))
#define DSU_LENGTH_MASK             _U_(0xFFFFFFFC) /**< \brief (DSU_LENGTH) MASK Register */

/* -------- DSU_DATA : (DSU Offset: 0x000C) (R/W 32) Data -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DATA:32;          /*!< bit:  0..31  Data                               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_DATA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_DATA_OFFSET             0x000C       /**< \brief (DSU_DATA offset) Data */
#define DSU_DATA_RESETVALUE         _U_(0x00000000) /**< \brief (DSU_DATA reset_value) Data */

#define DSU_DATA_DATA_Pos           0            /**< \brief (DSU_DATA) Data */
#define DSU_DATA_DATA_Msk           (_U_(0xFFFFFFFF) << DSU_DATA_DATA_Pos)
#define DSU_DATA_DATA(value)        (DSU_DATA_DATA_Msk & ((value) << DSU_DATA_DATA_Pos))
#define DSU_DATA_MASK               _U_(0xFFFFFFFF) /**< \brief (DSU_DATA) MASK Register */

/* -------- DSU_DCC : (DSU Offset: 0x0010) (R/W 32) Debug Communication Channel n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DATA:32;          /*!< bit:  0..31  Data                               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_DCC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_DCC_OFFSET              0x0010       /**< \brief (DSU_DCC offset) Debug Communication Channel n */
#define DSU_DCC_RESETVALUE          _U_(0x00000000) /**< \brief (DSU_DCC reset_value) Debug Communication Channel n */

#define DSU_DCC_DATA_Pos            0            /**< \brief (DSU_DCC) Data */
#define DSU_DCC_DATA_Msk            (_U_(0xFFFFFFFF) << DSU_DCC_DATA_Pos)
#define DSU_DCC_DATA(value)         (DSU_DCC_DATA_Msk & ((value) << DSU_DCC_DATA_Pos))
#define DSU_DCC_MASK                _U_(0xFFFFFFFF) /**< \brief (DSU_DCC) MASK Register */

/* -------- DSU_DID : (DSU Offset: 0x0018) (R/  32) Device Identification -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DEVSEL:8;         /*!< bit:  0.. 7  Device Select                      */
    uint32_t REVISION:4;       /*!< bit:  8..11  Revision Number                    */
    uint32_t DIE:4;            /*!< bit: 12..15  Die Number                         */
    uint32_t SERIES:6;         /*!< bit: 16..21  Series                             */
    uint32_t :1;               /*!< bit:     22  Reserved                           */
    uint32_t FAMILY:5;         /*!< bit: 23..27  Family                             */
    uint32_t PROCESSOR:4;      /*!< bit: 28..31  Processor                          */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_DID_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_DID_OFFSET              0x0018       /**< \brief (DSU_DID offset) Device Identification */

#define DSU_DID_DEVSEL_Pos          0            /**< \brief (DSU_DID) Device Select */
#define DSU_DID_DEVSEL_Msk          (_U_(0xFF) << DSU_DID_DEVSEL_Pos)
#define DSU_DID_DEVSEL(value)       (DSU_DID_DEVSEL_Msk & ((value) << DSU_DID_DEVSEL_Pos))
#define DSU_DID_REVISION_Pos        8            /**< \brief (DSU_DID) Revision Number */
#define DSU_DID_REVISION_Msk        (_U_(0xF) << DSU_DID_REVISION_Pos)
#define DSU_DID_REVISION(value)     (DSU_DID_REVISION_Msk & ((value) << DSU_DID_REVISION_Pos))
#define DSU_DID_DIE_Pos             12           /**< \brief (DSU_DID) Die Number */
#define DSU_DID_DIE_Msk             (_U_(0xF) << DSU_DID_DIE_Pos)
#define DSU_DID_DIE(value)          (DSU_DID_DIE_Msk & ((value) << DSU_DID_DIE_Pos))
#define DSU_DID_SERIES_Pos          16           /**< \brief (DSU_DID) Series */
#define DSU_DID_SERIES_Msk          (_U_(0x3F) << DSU_DID_SERIES_Pos)
#define DSU_DID_SERIES(value)       (DSU_DID_SERIES_Msk & ((value) << DSU_DID_SERIES_Pos))
#define   DSU_DID_SERIES_0_Val            _U_(0x0)   /**< \brief (DSU_DID) Cortex-M0+ processor, basic feature set */
#define   DSU_DID_SERIES_1_Val            _U_(0x1)   /**< \brief (DSU_DID) Cortex-M0+ processor, USB */
#define DSU_DID_SERIES_0            (DSU_DID_SERIES_0_Val          << DSU_DID_SERIES_Pos)
#define DSU_DID_SERIES_1            (DSU_DID_SERIES_1_Val          << DSU_DID_SERIES_Pos)
#define DSU_DID_FAMILY_Pos          23           /**< \brief (DSU_DID) Family */
#define DSU_DID_FAMILY_Msk          (_U_(0x1F) << DSU_DID_FAMILY_Pos)
#define DSU_DID_FAMILY(value)       (DSU_DID_FAMILY_Msk & ((value) << DSU_DID_FAMILY_Pos))
#define   DSU_DID_FAMILY_0_Val            _U_(0x0)   /**< \brief (DSU_DID) General purpose microcontroller */
#define   DSU_DID_FAMILY_1_Val            _U_(0x1)   /**< \brief (DSU_DID) PicoPower */
#define DSU_DID_FAMILY_0            (DSU_DID_FAMILY_0_Val          << DSU_DID_FAMILY_Pos)
#define DSU_DID_FAMILY_1            (DSU_DID_FAMILY_1_Val          << DSU_DID_FAMILY_Pos)
#define DSU_DID_PROCESSOR_Pos       28           /**< \brief (DSU_DID) Processor */
#define DSU_DID_PROCESSOR_Msk       (_U_(0xF) << DSU_DID_PROCESSOR_Pos)
#define DSU_DID_PROCESSOR(value)    (DSU_DID_PROCESSOR_Msk & ((value) << DSU_DID_PROCESSOR_Pos))
#define   DSU_DID_PROCESSOR_CM0P_Val      _U_(0x1)   /**< \brief (DSU_DID) Cortex-M0+ */
#define   DSU_DID_PROCESSOR_CM23_Val      _U_(0x2)   /**< \brief (DSU_DID) Cortex-M23 */
#define   DSU_DID_PROCESSOR_CM3_Val       _U_(0x3)   /**< \brief (DSU_DID) Cortex-M3 */
#define   DSU_DID_PROCESSOR_CM4_Val       _U_(0x5)   /**< \brief (DSU_DID) Cortex-M4 */
#define   DSU_DID_PROCESSOR_CM4F_Val      _U_(0x6)   /**< \brief (DSU_DID) Cortex-M4 with FPU */
#define   DSU_DID_PROCESSOR_CM33_Val      _U_(0x7)   /**< \brief (DSU_DID) Cortex-M33 */
#define DSU_DID_PROCESSOR_CM0P      (DSU_DID_PROCESSOR_CM0P_Val    << DSU_DID_PROCESSOR_Pos)
#define DSU_DID_PROCESSOR_CM23      (DSU_DID_PROCESSOR_CM23_Val    << DSU_DID_PROCESSOR_Pos)
#define DSU_DID_PROCESSOR_CM3       (DSU_DID_PROCESSOR_CM3_Val     << DSU_DID_PROCESSOR_Pos)
#define DSU_DID_PROCESSOR_CM4       (DSU_DID_PROCESSOR_CM4_Val     << DSU_DID_PROCESSOR_Pos)
#define DSU_DID_PROCESSOR_CM4F      (DSU_DID_PROCESSOR_CM4F_Val    << DSU_DID_PROCESSOR_Pos)
#define DSU_DID_PROCESSOR_CM33      (DSU_DID_PROCESSOR_CM33_Val    << DSU_DID_PROCESSOR_Pos)
#define DSU_DID_MASK                _U_(0xFFBFFFFF) /**< \brief (DSU_DID) MASK Register */

/* -------- DSU_CFG : (DSU Offset: 0x001C) (R/W 32) Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t LQOS:2;           /*!< bit:  0.. 1  Latency Quality Of Service         */
    uint32_t DCCDMALEVEL:2;    /*!< bit:  2.. 3  DMA Trigger Level                  */
    uint32_t ETBRAMEN:1;       /*!< bit:      4  Trace Control                      */
    uint32_t :27;              /*!< bit:  5..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_CFG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_CFG_OFFSET              0x001C       /**< \brief (DSU_CFG offset) Configuration */
#define DSU_CFG_RESETVALUE          _U_(0x00000002) /**< \brief (DSU_CFG reset_value) Configuration */

#define DSU_CFG_LQOS_Pos            0            /**< \brief (DSU_CFG) Latency Quality Of Service */
#define DSU_CFG_LQOS_Msk            (_U_(0x3) << DSU_CFG_LQOS_Pos)
#define DSU_CFG_LQOS(value)         (DSU_CFG_LQOS_Msk & ((value) << DSU_CFG_LQOS_Pos))
#define DSU_CFG_DCCDMALEVEL_Pos     2            /**< \brief (DSU_CFG) DMA Trigger Level */
#define DSU_CFG_DCCDMALEVEL_Msk     (_U_(0x3) << DSU_CFG_DCCDMALEVEL_Pos)
#define DSU_CFG_DCCDMALEVEL(value)  (DSU_CFG_DCCDMALEVEL_Msk & ((value) << DSU_CFG_DCCDMALEVEL_Pos))
#define   DSU_CFG_DCCDMALEVEL_EMPTY_Val   _U_(0x0)   /**< \brief (DSU_CFG) Trigger rises when DCC is empty */
#define   DSU_CFG_DCCDMALEVEL_FULL_Val    _U_(0x1)   /**< \brief (DSU_CFG) Trigger rises when DCC is full */
#define DSU_CFG_DCCDMALEVEL_EMPTY   (DSU_CFG_DCCDMALEVEL_EMPTY_Val << DSU_CFG_DCCDMALEVEL_Pos)
#define DSU_CFG_DCCDMALEVEL_FULL    (DSU_CFG_DCCDMALEVEL_FULL_Val  << DSU_CFG_DCCDMALEVEL_Pos)
#define DSU_CFG_ETBRAMEN_Pos        4            /**< \brief (DSU_CFG) Trace Control */
#define DSU_CFG_ETBRAMEN            (_U_(0x1) << DSU_CFG_ETBRAMEN_Pos)
#define DSU_CFG_MASK                _U_(0x0000001F) /**< \brief (DSU_CFG) MASK Register */

/* -------- DSU_MBCTRL : (DSU Offset: 0x0040) (R/W 32) MBIST Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  MBIST Software Reset               */
    uint32_t ENABLE:1;         /*!< bit:      1  MBIST Enable                       */
    uint32_t :30;              /*!< bit:  2..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_MBCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_MBCTRL_OFFSET           0x0040       /**< \brief (DSU_MBCTRL offset) MBIST Control */
#define DSU_MBCTRL_RESETVALUE       _U_(0x00000000) /**< \brief (DSU_MBCTRL reset_value) MBIST Control */

#define DSU_MBCTRL_SWRST_Pos        0            /**< \brief (DSU_MBCTRL) MBIST Software Reset */
#define DSU_MBCTRL_SWRST            (_U_(0x1) << DSU_MBCTRL_SWRST_Pos)
#define DSU_MBCTRL_ENABLE_Pos       1            /**< \brief (DSU_MBCTRL) MBIST Enable */
#define DSU_MBCTRL_ENABLE           (_U_(0x1) << DSU_MBCTRL_ENABLE_Pos)
#define DSU_MBCTRL_MASK             _U_(0x00000003) /**< \brief (DSU_MBCTRL) MASK Register */

/* -------- DSU_MBCONFIG : (DSU Offset: 0x0044) (R/W 32) MBIST Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ALGO:5;           /*!< bit:  0.. 4  MBIST Algorithm                    */
    uint32_t :1;               /*!< bit:      5  Reserved                           */
    uint32_t DEFRDMARGIN:1;    /*!< bit:      6  Force Default Read Margin          */
    uint32_t DBG:1;            /*!< bit:      7  Enable Debug Mode                  */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_MBCONFIG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_MBCONFIG_OFFSET         0x0044       /**< \brief (DSU_MBCONFIG offset) MBIST Configuration */
#define DSU_MBCONFIG_RESETVALUE     _U_(0x00000000) /**< \brief (DSU_MBCONFIG reset_value) MBIST Configuration */

#define DSU_MBCONFIG_ALGO_Pos       0            /**< \brief (DSU_MBCONFIG) MBIST Algorithm */
#define DSU_MBCONFIG_ALGO_Msk       (_U_(0x1F) << DSU_MBCONFIG_ALGO_Pos)
#define DSU_MBCONFIG_ALGO(value)    (DSU_MBCONFIG_ALGO_Msk & ((value) << DSU_MBCONFIG_ALGO_Pos))
#define   DSU_MBCONFIG_ALGO_MEMCLEAR_Val  _U_(0x0)   /**< \brief (DSU_MBCONFIG) Memory Clear (1n) */
#define   DSU_MBCONFIG_ALGO_VERIFY_Val    _U_(0x1)   /**< \brief (DSU_MBCONFIG) Memory Verify (1n) */
#define   DSU_MBCONFIG_ALGO_CLEARVER_Val  _U_(0x2)   /**< \brief (DSU_MBCONFIG) Memory Clear and Verify (2n) */
#define   DSU_MBCONFIG_ALGO_ADDR_DEC_Val  _U_(0x3)   /**< \brief (DSU_MBCONFIG) Address Decoder (2n) */
#define   DSU_MBCONFIG_ALGO_MARCH_LR_Val  _U_(0x4)   /**< \brief (DSU_MBCONFIG) March LR (14n) */
#define   DSU_MBCONFIG_ALGO_MARCH_SR_Val  _U_(0x5)   /**< \brief (DSU_MBCONFIG) March SR (14n) */
#define   DSU_MBCONFIG_ALGO_MARCH_SS_Val  _U_(0x6)   /**< \brief (DSU_MBCONFIG) March SS (22n) */
#define   DSU_MBCONFIG_ALGO_CRC_UP_Val    _U_(0x8)   /**< \brief (DSU_MBCONFIG) CRC increasing address (1n) */
#define   DSU_MBCONFIG_ALGO_CRC_DOWN_Val  _U_(0x9)   /**< \brief (DSU_MBCONFIG) CRC decreasing address (1n) */
#define DSU_MBCONFIG_ALGO_MEMCLEAR  (DSU_MBCONFIG_ALGO_MEMCLEAR_Val << DSU_MBCONFIG_ALGO_Pos)
#define DSU_MBCONFIG_ALGO_VERIFY    (DSU_MBCONFIG_ALGO_VERIFY_Val  << DSU_MBCONFIG_ALGO_Pos)
#define DSU_MBCONFIG_ALGO_CLEARVER  (DSU_MBCONFIG_ALGO_CLEARVER_Val << DSU_MBCONFIG_ALGO_Pos)
#define DSU_MBCONFIG_ALGO_ADDR_DEC  (DSU_MBCONFIG_ALGO_ADDR_DEC_Val << DSU_MBCONFIG_ALGO_Pos)
#define DSU_MBCONFIG_ALGO_MARCH_LR  (DSU_MBCONFIG_ALGO_MARCH_LR_Val << DSU_MBCONFIG_ALGO_Pos)
#define DSU_MBCONFIG_ALGO_MARCH_SR  (DSU_MBCONFIG_ALGO_MARCH_SR_Val << DSU_MBCONFIG_ALGO_Pos)
#define DSU_MBCONFIG_ALGO_MARCH_SS  (DSU_MBCONFIG_ALGO_MARCH_SS_Val << DSU_MBCONFIG_ALGO_Pos)
#define DSU_MBCONFIG_ALGO_CRC_UP    (DSU_MBCONFIG_ALGO_CRC_UP_Val  << DSU_MBCONFIG_ALGO_Pos)
#define DSU_MBCONFIG_ALGO_CRC_DOWN  (DSU_MBCONFIG_ALGO_CRC_DOWN_Val << DSU_MBCONFIG_ALGO_Pos)
#define DSU_MBCONFIG_DEFRDMARGIN_Pos 6            /**< \brief (DSU_MBCONFIG) Force Default Read Margin */
#define DSU_MBCONFIG_DEFRDMARGIN    (_U_(0x1) << DSU_MBCONFIG_DEFRDMARGIN_Pos)
#define DSU_MBCONFIG_DBG_Pos        7            /**< \brief (DSU_MBCONFIG) Enable Debug Mode */
#define DSU_MBCONFIG_DBG            (_U_(0x1) << DSU_MBCONFIG_DBG_Pos)
#define DSU_MBCONFIG_MASK           _U_(0x000000DF) /**< \brief (DSU_MBCONFIG) MASK Register */

/* -------- DSU_MBWORD : (DSU Offset: 0x0048) (R/W 32) MBIST Background Word -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DATA:32;          /*!< bit:  0..31  MBIST Background Word              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_MBWORD_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_MBWORD_OFFSET           0x0048       /**< \brief (DSU_MBWORD offset) MBIST Background Word */
#define DSU_MBWORD_RESETVALUE       _U_(0x00000000) /**< \brief (DSU_MBWORD reset_value) MBIST Background Word */

#define DSU_MBWORD_DATA_Pos         0            /**< \brief (DSU_MBWORD) MBIST Background Word */
#define DSU_MBWORD_DATA_Msk         (_U_(0xFFFFFFFF) << DSU_MBWORD_DATA_Pos)
#define DSU_MBWORD_DATA(value)      (DSU_MBWORD_DATA_Msk & ((value) << DSU_MBWORD_DATA_Pos))
#define DSU_MBWORD_MASK             _U_(0xFFFFFFFF) /**< \brief (DSU_MBWORD) MASK Register */

/* -------- DSU_MBGSTAT : (DSU Offset: 0x004C) (R/W 32) MBIST Global Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ALLDONE:1;        /*!< bit:      0  MBIST Completed                    */
    uint32_t FAILED:1;         /*!< bit:      1  MBIST Failed                       */
    uint32_t ERRINFO:1;        /*!< bit:      2  MBIST Error Info Present           */
    uint32_t CONFIGURED:1;     /*!< bit:      3  MBIST Configuration Sent           */
    uint32_t :28;              /*!< bit:  4..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_MBGSTAT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_MBGSTAT_OFFSET          0x004C       /**< \brief (DSU_MBGSTAT offset) MBIST Global Status */
#define DSU_MBGSTAT_RESETVALUE      _U_(0x00000000) /**< \brief (DSU_MBGSTAT reset_value) MBIST Global Status */

#define DSU_MBGSTAT_ALLDONE_Pos     0            /**< \brief (DSU_MBGSTAT) MBIST Completed */
#define DSU_MBGSTAT_ALLDONE         (_U_(0x1) << DSU_MBGSTAT_ALLDONE_Pos)
#define DSU_MBGSTAT_FAILED_Pos      1            /**< \brief (DSU_MBGSTAT) MBIST Failed */
#define DSU_MBGSTAT_FAILED          (_U_(0x1) << DSU_MBGSTAT_FAILED_Pos)
#define DSU_MBGSTAT_ERRINFO_Pos     2            /**< \brief (DSU_MBGSTAT) MBIST Error Info Present */
#define DSU_MBGSTAT_ERRINFO         (_U_(0x1) << DSU_MBGSTAT_ERRINFO_Pos)
#define DSU_MBGSTAT_CONFIGURED_Pos  3            /**< \brief (DSU_MBGSTAT) MBIST Configuration Sent */
#define DSU_MBGSTAT_CONFIGURED      (_U_(0x1) << DSU_MBGSTAT_CONFIGURED_Pos)
#define DSU_MBGSTAT_MASK            _U_(0x0000000F) /**< \brief (DSU_MBGSTAT) MASK Register */

/* -------- DSU_MBDFAIL : (DSU Offset: 0x0050) (R/  32) MBIST Fail Data -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DATA:32;          /*!< bit:  0..31  Error Data Read                    */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_MBDFAIL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_MBDFAIL_OFFSET          0x0050       /**< \brief (DSU_MBDFAIL offset) MBIST Fail Data */
#define DSU_MBDFAIL_RESETVALUE      _U_(0x00000000) /**< \brief (DSU_MBDFAIL reset_value) MBIST Fail Data */

#define DSU_MBDFAIL_DATA_Pos        0            /**< \brief (DSU_MBDFAIL) Error Data Read */
#define DSU_MBDFAIL_DATA_Msk        (_U_(0xFFFFFFFF) << DSU_MBDFAIL_DATA_Pos)
#define DSU_MBDFAIL_DATA(value)     (DSU_MBDFAIL_DATA_Msk & ((value) << DSU_MBDFAIL_DATA_Pos))
#define DSU_MBDFAIL_MASK            _U_(0xFFFFFFFF) /**< \brief (DSU_MBDFAIL) MASK Register */

/* -------- DSU_MBDEXP : (DSU Offset: 0x0054) (R/  32) MBIST Expected Data -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DATA:32;          /*!< bit:  0..31  Expected Data                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_MBDEXP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_MBDEXP_OFFSET           0x0054       /**< \brief (DSU_MBDEXP offset) MBIST Expected Data */
#define DSU_MBDEXP_RESETVALUE       _U_(0x00000000) /**< \brief (DSU_MBDEXP reset_value) MBIST Expected Data */

#define DSU_MBDEXP_DATA_Pos         0            /**< \brief (DSU_MBDEXP) Expected Data */
#define DSU_MBDEXP_DATA_Msk         (_U_(0xFFFFFFFF) << DSU_MBDEXP_DATA_Pos)
#define DSU_MBDEXP_DATA(value)      (DSU_MBDEXP_DATA_Msk & ((value) << DSU_MBDEXP_DATA_Pos))
#define DSU_MBDEXP_MASK             _U_(0xFFFFFFFF) /**< \brief (DSU_MBDEXP) MASK Register */

/* -------- DSU_MBAFAIL : (DSU Offset: 0x0058) (R/  32) MBIST Fail Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ADDR:14;          /*!< bit:  0..13  Error Address                      */
    uint32_t :18;              /*!< bit: 14..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_MBAFAIL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_MBAFAIL_OFFSET          0x0058       /**< \brief (DSU_MBAFAIL offset) MBIST Fail Address */
#define DSU_MBAFAIL_RESETVALUE      _U_(0x00000000) /**< \brief (DSU_MBAFAIL reset_value) MBIST Fail Address */

#define DSU_MBAFAIL_ADDR_Pos        0            /**< \brief (DSU_MBAFAIL) Error Address */
#define DSU_MBAFAIL_ADDR_Msk        (_U_(0x3FFF) << DSU_MBAFAIL_ADDR_Pos)
#define DSU_MBAFAIL_ADDR(value)     (DSU_MBAFAIL_ADDR_Msk & ((value) << DSU_MBAFAIL_ADDR_Pos))
#define DSU_MBAFAIL_MASK            _U_(0x00003FFF) /**< \brief (DSU_MBAFAIL) MASK Register */

/* -------- DSU_MBCONTEXT : (DSU Offset: 0x005C) (R/  32) MBIST Fail Context -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SUBSTEP:5;        /*!< bit:  0.. 4  Algorithm Sub-step                 */
    uint32_t STEP:5;           /*!< bit:  5.. 9  Algorithm Step                     */
    uint32_t PORT:1;           /*!< bit:     10  DPRAM Port Index                   */
    uint32_t :21;              /*!< bit: 11..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_MBCONTEXT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_MBCONTEXT_OFFSET        0x005C       /**< \brief (DSU_MBCONTEXT offset) MBIST Fail Context */
#define DSU_MBCONTEXT_RESETVALUE    _U_(0x00000000) /**< \brief (DSU_MBCONTEXT reset_value) MBIST Fail Context */

#define DSU_MBCONTEXT_SUBSTEP_Pos   0            /**< \brief (DSU_MBCONTEXT) Algorithm Sub-step */
#define DSU_MBCONTEXT_SUBSTEP_Msk   (_U_(0x1F) << DSU_MBCONTEXT_SUBSTEP_Pos)
#define DSU_MBCONTEXT_SUBSTEP(value) (DSU_MBCONTEXT_SUBSTEP_Msk & ((value) << DSU_MBCONTEXT_SUBSTEP_Pos))
#define   DSU_MBCONTEXT_SUBSTEP_R0_1_Val  _U_(0x1)   /**< \brief (DSU_MBCONTEXT)  */
#define   DSU_MBCONTEXT_SUBSTEP_R1_1_Val  _U_(0x3)   /**< \brief (DSU_MBCONTEXT)  */
#define   DSU_MBCONTEXT_SUBSTEP_R0_2_Val  _U_(0x5)   /**< \brief (DSU_MBCONTEXT)  */
#define   DSU_MBCONTEXT_SUBSTEP_R1_2_Val  _U_(0x7)   /**< \brief (DSU_MBCONTEXT)  */
#define   DSU_MBCONTEXT_SUBSTEP_R0_3_Val  _U_(0x9)   /**< \brief (DSU_MBCONTEXT)  */
#define   DSU_MBCONTEXT_SUBSTEP_R1_3_Val  _U_(0xB)   /**< \brief (DSU_MBCONTEXT)  */
#define DSU_MBCONTEXT_SUBSTEP_R0_1  (DSU_MBCONTEXT_SUBSTEP_R0_1_Val << DSU_MBCONTEXT_SUBSTEP_Pos)
#define DSU_MBCONTEXT_SUBSTEP_R1_1  (DSU_MBCONTEXT_SUBSTEP_R1_1_Val << DSU_MBCONTEXT_SUBSTEP_Pos)
#define DSU_MBCONTEXT_SUBSTEP_R0_2  (DSU_MBCONTEXT_SUBSTEP_R0_2_Val << DSU_MBCONTEXT_SUBSTEP_Pos)
#define DSU_MBCONTEXT_SUBSTEP_R1_2  (DSU_MBCONTEXT_SUBSTEP_R1_2_Val << DSU_MBCONTEXT_SUBSTEP_Pos)
#define DSU_MBCONTEXT_SUBSTEP_R0_3  (DSU_MBCONTEXT_SUBSTEP_R0_3_Val << DSU_MBCONTEXT_SUBSTEP_Pos)
#define DSU_MBCONTEXT_SUBSTEP_R1_3  (DSU_MBCONTEXT_SUBSTEP_R1_3_Val << DSU_MBCONTEXT_SUBSTEP_Pos)
#define DSU_MBCONTEXT_STEP_Pos      5            /**< \brief (DSU_MBCONTEXT) Algorithm Step */
#define DSU_MBCONTEXT_STEP_Msk      (_U_(0x1F) << DSU_MBCONTEXT_STEP_Pos)
#define DSU_MBCONTEXT_STEP(value)   (DSU_MBCONTEXT_STEP_Msk & ((value) << DSU_MBCONTEXT_STEP_Pos))
#define   DSU_MBCONTEXT_STEP_DOWN_R0W1_Val _U_(0x2)   /**< \brief (DSU_MBCONTEXT)  */
#define   DSU_MBCONTEXT_STEP_UP_R1W0R0W1_Val _U_(0x3)   /**< \brief (DSU_MBCONTEXT)  */
#define   DSU_MBCONTEXT_STEP_UP_R1W0_Val  _U_(0x4)   /**< \brief (DSU_MBCONTEXT)  */
#define   DSU_MBCONTEXT_STEP_UP_R0W1R1W0_Val _U_(0x5)   /**< \brief (DSU_MBCONTEXT)  */
#define   DSU_MBCONTEXT_STEP_UP_R0_Val    _U_(0x6)   /**< \brief (DSU_MBCONTEXT)  */
#define   DSU_MBCONTEXT_STEP_UP_R0R0W0R0W1_Val _U_(0x7)   /**< \brief (DSU_MBCONTEXT)  */
#define   DSU_MBCONTEXT_STEP_UP_R1R1W1R1W0_Val _U_(0x8)   /**< \brief (DSU_MBCONTEXT)  */
#define   DSU_MBCONTEXT_STEP_DOWN_R0R0W0R0W1_Val _U_(0x9)   /**< \brief (DSU_MBCONTEXT)  */
#define   DSU_MBCONTEXT_STEP_DOWN_R1R1W1R1W0_Val _U_(0xA)   /**< \brief (DSU_MBCONTEXT)  */
#define   DSU_MBCONTEXT_STEP_UP_R0R0_Val  _U_(0xC)   /**< \brief (DSU_MBCONTEXT)  */
#define   DSU_MBCONTEXT_STEP_DOWN_R1W0R0W1_Val _U_(0xE)   /**< \brief (DSU_MBCONTEXT)  */
#define   DSU_MBCONTEXT_STEP_DOWN_R1R1_Val _U_(0xF)   /**< \brief (DSU_MBCONTEXT)  */
#define DSU_MBCONTEXT_STEP_DOWN_R0W1 (DSU_MBCONTEXT_STEP_DOWN_R0W1_Val << DSU_MBCONTEXT_STEP_Pos)
#define DSU_MBCONTEXT_STEP_UP_R1W0R0W1 (DSU_MBCONTEXT_STEP_UP_R1W0R0W1_Val << DSU_MBCONTEXT_STEP_Pos)
#define DSU_MBCONTEXT_STEP_UP_R1W0  (DSU_MBCONTEXT_STEP_UP_R1W0_Val << DSU_MBCONTEXT_STEP_Pos)
#define DSU_MBCONTEXT_STEP_UP_R0W1R1W0 (DSU_MBCONTEXT_STEP_UP_R0W1R1W0_Val << DSU_MBCONTEXT_STEP_Pos)
#define DSU_MBCONTEXT_STEP_UP_R0    (DSU_MBCONTEXT_STEP_UP_R0_Val  << DSU_MBCONTEXT_STEP_Pos)
#define DSU_MBCONTEXT_STEP_UP_R0R0W0R0W1 (DSU_MBCONTEXT_STEP_UP_R0R0W0R0W1_Val << DSU_MBCONTEXT_STEP_Pos)
#define DSU_MBCONTEXT_STEP_UP_R1R1W1R1W0 (DSU_MBCONTEXT_STEP_UP_R1R1W1R1W0_Val << DSU_MBCONTEXT_STEP_Pos)
#define DSU_MBCONTEXT_STEP_DOWN_R0R0W0R0W1 (DSU_MBCONTEXT_STEP_DOWN_R0R0W0R0W1_Val << DSU_MBCONTEXT_STEP_Pos)
#define DSU_MBCONTEXT_STEP_DOWN_R1R1W1R1W0 (DSU_MBCONTEXT_STEP_DOWN_R1R1W1R1W0_Val << DSU_MBCONTEXT_STEP_Pos)
#define DSU_MBCONTEXT_STEP_UP_R0R0  (DSU_MBCONTEXT_STEP_UP_R0R0_Val << DSU_MBCONTEXT_STEP_Pos)
#define DSU_MBCONTEXT_STEP_DOWN_R1W0R0W1 (DSU_MBCONTEXT_STEP_DOWN_R1W0R0W1_Val << DSU_MBCONTEXT_STEP_Pos)
#define DSU_MBCONTEXT_STEP_DOWN_R1R1 (DSU_MBCONTEXT_STEP_DOWN_R1R1_Val << DSU_MBCONTEXT_STEP_Pos)
#define DSU_MBCONTEXT_PORT_Pos      10           /**< \brief (DSU_MBCONTEXT) DPRAM Port Index */
#define DSU_MBCONTEXT_PORT          (_U_(0x1) << DSU_MBCONTEXT_PORT_Pos)
#define DSU_MBCONTEXT_MASK          _U_(0x000007FF) /**< \brief (DSU_MBCONTEXT) MASK Register */

/* -------- DSU_MBENABLE0 : (DSU Offset: 0x0060) (R/W 32) MBIST Memory Enable 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ENABLE0:1;        /*!< bit:      0  Memory 0 MBIST Enable              */
    uint32_t ENABLE1:1;        /*!< bit:      1  Memory 1 MBIST Enable              */
    uint32_t ENABLE2:1;        /*!< bit:      2  Memory 2 MBIST Enable              */
    uint32_t ENABLE3:1;        /*!< bit:      3  Memory 3 MBIST Enable              */
    uint32_t ENABLE4:1;        /*!< bit:      4  Memory 4 MBIST Enable              */
    uint32_t ENABLE5:1;        /*!< bit:      5  Memory 5 MBIST Enable              */
    uint32_t ENABLE6:1;        /*!< bit:      6  Memory 6 MBIST Enable              */
    uint32_t ENABLE7:1;        /*!< bit:      7  Memory 7 MBIST Enable              */
    uint32_t ENABLE8:1;        /*!< bit:      8  Memory 8 MBIST Enable              */
    uint32_t ENABLE9:1;        /*!< bit:      9  Memory 9 MBIST Enable              */
    uint32_t ENABLE10:1;       /*!< bit:     10  Memory 10 MBIST Enable             */
    uint32_t ENABLE11:1;       /*!< bit:     11  Memory 11 MBIST Enable             */
    uint32_t ENABLE12:1;       /*!< bit:     12  Memory 12 MBIST Enable             */
    uint32_t ENABLE13:1;       /*!< bit:     13  Memory 13 MBIST Enable             */
    uint32_t ENABLE14:1;       /*!< bit:     14  Memory 14 MBIST Enable             */
    uint32_t ENABLE15:1;       /*!< bit:     15  Memory 15 MBIST Enable             */
    uint32_t ENABLE16:1;       /*!< bit:     16  Memory 16 MBIST Enable             */
    uint32_t ENABLE17:1;       /*!< bit:     17  Memory 17 MBIST Enable             */
    uint32_t ENABLE18:1;       /*!< bit:     18  Memory 18 MBIST Enable             */
    uint32_t ENABLE19:1;       /*!< bit:     19  Memory 19 MBIST Enable             */
    uint32_t ENABLE20:1;       /*!< bit:     20  Memory 20 MBIST Enable             */
    uint32_t ENABLE21:1;       /*!< bit:     21  Memory 21 MBIST Enable             */
    uint32_t ENABLE22:1;       /*!< bit:     22  Memory 22 MBIST Enable             */
    uint32_t ENABLE23:1;       /*!< bit:     23  Memory 23 MBIST Enable             */
    uint32_t ENABLE24:1;       /*!< bit:     24  Memory 24 MBIST Enable             */
    uint32_t ENABLE25:1;       /*!< bit:     25  Memory 25 MBIST Enable             */
    uint32_t ENABLE26:1;       /*!< bit:     26  Memory 26 MBIST Enable             */
    uint32_t ENABLE27:1;       /*!< bit:     27  Memory 27 MBIST Enable             */
    uint32_t ENABLE28:1;       /*!< bit:     28  Memory 28 MBIST Enable             */
    uint32_t :3;               /*!< bit: 29..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t ENABLE:29;        /*!< bit:  0..28  Memory x MBIST Enable              */
    uint32_t :3;               /*!< bit: 29..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_MBENABLE0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_MBENABLE0_OFFSET        0x0060       /**< \brief (DSU_MBENABLE0 offset) MBIST Memory Enable 0 */
#define DSU_MBENABLE0_RESETVALUE    _U_(0x00000000) /**< \brief (DSU_MBENABLE0 reset_value) MBIST Memory Enable 0 */

#define DSU_MBENABLE0_ENABLE0_Pos   0            /**< \brief (DSU_MBENABLE0) Memory 0 MBIST Enable */
#define DSU_MBENABLE0_ENABLE0       (_U_(1) << DSU_MBENABLE0_ENABLE0_Pos)
#define DSU_MBENABLE0_ENABLE1_Pos   1            /**< \brief (DSU_MBENABLE0) Memory 1 MBIST Enable */
#define DSU_MBENABLE0_ENABLE1       (_U_(1) << DSU_MBENABLE0_ENABLE1_Pos)
#define DSU_MBENABLE0_ENABLE2_Pos   2            /**< \brief (DSU_MBENABLE0) Memory 2 MBIST Enable */
#define DSU_MBENABLE0_ENABLE2       (_U_(1) << DSU_MBENABLE0_ENABLE2_Pos)
#define DSU_MBENABLE0_ENABLE3_Pos   3            /**< \brief (DSU_MBENABLE0) Memory 3 MBIST Enable */
#define DSU_MBENABLE0_ENABLE3       (_U_(1) << DSU_MBENABLE0_ENABLE3_Pos)
#define DSU_MBENABLE0_ENABLE4_Pos   4            /**< \brief (DSU_MBENABLE0) Memory 4 MBIST Enable */
#define DSU_MBENABLE0_ENABLE4       (_U_(1) << DSU_MBENABLE0_ENABLE4_Pos)
#define DSU_MBENABLE0_ENABLE5_Pos   5            /**< \brief (DSU_MBENABLE0) Memory 5 MBIST Enable */
#define DSU_MBENABLE0_ENABLE5       (_U_(1) << DSU_MBENABLE0_ENABLE5_Pos)
#define DSU_MBENABLE0_ENABLE6_Pos   6            /**< \brief (DSU_MBENABLE0) Memory 6 MBIST Enable */
#define DSU_MBENABLE0_ENABLE6       (_U_(1) << DSU_MBENABLE0_ENABLE6_Pos)
#define DSU_MBENABLE0_ENABLE7_Pos   7            /**< \brief (DSU_MBENABLE0) Memory 7 MBIST Enable */
#define DSU_MBENABLE0_ENABLE7       (_U_(1) << DSU_MBENABLE0_ENABLE7_Pos)
#define DSU_MBENABLE0_ENABLE8_Pos   8            /**< \brief (DSU_MBENABLE0) Memory 8 MBIST Enable */
#define DSU_MBENABLE0_ENABLE8       (_U_(1) << DSU_MBENABLE0_ENABLE8_Pos)
#define DSU_MBENABLE0_ENABLE9_Pos   9            /**< \brief (DSU_MBENABLE0) Memory 9 MBIST Enable */
#define DSU_MBENABLE0_ENABLE9       (_U_(1) << DSU_MBENABLE0_ENABLE9_Pos)
#define DSU_MBENABLE0_ENABLE10_Pos  10           /**< \brief (DSU_MBENABLE0) Memory 10 MBIST Enable */
#define DSU_MBENABLE0_ENABLE10      (_U_(1) << DSU_MBENABLE0_ENABLE10_Pos)
#define DSU_MBENABLE0_ENABLE11_Pos  11           /**< \brief (DSU_MBENABLE0) Memory 11 MBIST Enable */
#define DSU_MBENABLE0_ENABLE11      (_U_(1) << DSU_MBENABLE0_ENABLE11_Pos)
#define DSU_MBENABLE0_ENABLE12_Pos  12           /**< \brief (DSU_MBENABLE0) Memory 12 MBIST Enable */
#define DSU_MBENABLE0_ENABLE12      (_U_(1) << DSU_MBENABLE0_ENABLE12_Pos)
#define DSU_MBENABLE0_ENABLE13_Pos  13           /**< \brief (DSU_MBENABLE0) Memory 13 MBIST Enable */
#define DSU_MBENABLE0_ENABLE13      (_U_(1) << DSU_MBENABLE0_ENABLE13_Pos)
#define DSU_MBENABLE0_ENABLE14_Pos  14           /**< \brief (DSU_MBENABLE0) Memory 14 MBIST Enable */
#define DSU_MBENABLE0_ENABLE14      (_U_(1) << DSU_MBENABLE0_ENABLE14_Pos)
#define DSU_MBENABLE0_ENABLE15_Pos  15           /**< \brief (DSU_MBENABLE0) Memory 15 MBIST Enable */
#define DSU_MBENABLE0_ENABLE15      (_U_(1) << DSU_MBENABLE0_ENABLE15_Pos)
#define DSU_MBENABLE0_ENABLE16_Pos  16           /**< \brief (DSU_MBENABLE0) Memory 16 MBIST Enable */
#define DSU_MBENABLE0_ENABLE16      (_U_(1) << DSU_MBENABLE0_ENABLE16_Pos)
#define DSU_MBENABLE0_ENABLE17_Pos  17           /**< \brief (DSU_MBENABLE0) Memory 17 MBIST Enable */
#define DSU_MBENABLE0_ENABLE17      (_U_(1) << DSU_MBENABLE0_ENABLE17_Pos)
#define DSU_MBENABLE0_ENABLE18_Pos  18           /**< \brief (DSU_MBENABLE0) Memory 18 MBIST Enable */
#define DSU_MBENABLE0_ENABLE18      (_U_(1) << DSU_MBENABLE0_ENABLE18_Pos)
#define DSU_MBENABLE0_ENABLE19_Pos  19           /**< \brief (DSU_MBENABLE0) Memory 19 MBIST Enable */
#define DSU_MBENABLE0_ENABLE19      (_U_(1) << DSU_MBENABLE0_ENABLE19_Pos)
#define DSU_MBENABLE0_ENABLE20_Pos  20           /**< \brief (DSU_MBENABLE0) Memory 20 MBIST Enable */
#define DSU_MBENABLE0_ENABLE20      (_U_(1) << DSU_MBENABLE0_ENABLE20_Pos)
#define DSU_MBENABLE0_ENABLE21_Pos  21           /**< \brief (DSU_MBENABLE0) Memory 21 MBIST Enable */
#define DSU_MBENABLE0_ENABLE21      (_U_(1) << DSU_MBENABLE0_ENABLE21_Pos)
#define DSU_MBENABLE0_ENABLE22_Pos  22           /**< \brief (DSU_MBENABLE0) Memory 22 MBIST Enable */
#define DSU_MBENABLE0_ENABLE22      (_U_(1) << DSU_MBENABLE0_ENABLE22_Pos)
#define DSU_MBENABLE0_ENABLE23_Pos  23           /**< \brief (DSU_MBENABLE0) Memory 23 MBIST Enable */
#define DSU_MBENABLE0_ENABLE23      (_U_(1) << DSU_MBENABLE0_ENABLE23_Pos)
#define DSU_MBENABLE0_ENABLE24_Pos  24           /**< \brief (DSU_MBENABLE0) Memory 24 MBIST Enable */
#define DSU_MBENABLE0_ENABLE24      (_U_(1) << DSU_MBENABLE0_ENABLE24_Pos)
#define DSU_MBENABLE0_ENABLE25_Pos  25           /**< \brief (DSU_MBENABLE0) Memory 25 MBIST Enable */
#define DSU_MBENABLE0_ENABLE25      (_U_(1) << DSU_MBENABLE0_ENABLE25_Pos)
#define DSU_MBENABLE0_ENABLE26_Pos  26           /**< \brief (DSU_MBENABLE0) Memory 26 MBIST Enable */
#define DSU_MBENABLE0_ENABLE26      (_U_(1) << DSU_MBENABLE0_ENABLE26_Pos)
#define DSU_MBENABLE0_ENABLE27_Pos  27           /**< \brief (DSU_MBENABLE0) Memory 27 MBIST Enable */
#define DSU_MBENABLE0_ENABLE27      (_U_(1) << DSU_MBENABLE0_ENABLE27_Pos)
#define DSU_MBENABLE0_ENABLE28_Pos  28           /**< \brief (DSU_MBENABLE0) Memory 28 MBIST Enable */
#define DSU_MBENABLE0_ENABLE28      (_U_(1) << DSU_MBENABLE0_ENABLE28_Pos)
#define DSU_MBENABLE0_ENABLE_Pos    0            /**< \brief (DSU_MBENABLE0) Memory x MBIST Enable */
#define DSU_MBENABLE0_ENABLE_Msk    (_U_(0x1FFFFFFF) << DSU_MBENABLE0_ENABLE_Pos)
#define DSU_MBENABLE0_ENABLE(value) (DSU_MBENABLE0_ENABLE_Msk & ((value) << DSU_MBENABLE0_ENABLE_Pos))
#define DSU_MBENABLE0_MASK          _U_(0x1FFFFFFF) /**< \brief (DSU_MBENABLE0) MASK Register */

/* -------- DSU_MBBUSY0 : (DSU Offset: 0x0068) (R/  32) MBIST Memory Busy 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t BUSY0:1;          /*!< bit:      0  Memory 0 BIST Busy                 */
    uint32_t BUSY1:1;          /*!< bit:      1  Memory 1 BIST Busy                 */
    uint32_t BUSY2:1;          /*!< bit:      2  Memory 2 BIST Busy                 */
    uint32_t BUSY3:1;          /*!< bit:      3  Memory 3 BIST Busy                 */
    uint32_t BUSY4:1;          /*!< bit:      4  Memory 4 BIST Busy                 */
    uint32_t BUSY5:1;          /*!< bit:      5  Memory 5 BIST Busy                 */
    uint32_t BUSY6:1;          /*!< bit:      6  Memory 6 BIST Busy                 */
    uint32_t BUSY7:1;          /*!< bit:      7  Memory 7 BIST Busy                 */
    uint32_t BUSY8:1;          /*!< bit:      8  Memory 8 BIST Busy                 */
    uint32_t BUSY9:1;          /*!< bit:      9  Memory 9 BIST Busy                 */
    uint32_t BUSY10:1;         /*!< bit:     10  Memory 10 BIST Busy                */
    uint32_t BUSY11:1;         /*!< bit:     11  Memory 11 BIST Busy                */
    uint32_t BUSY12:1;         /*!< bit:     12  Memory 12 BIST Busy                */
    uint32_t BUSY13:1;         /*!< bit:     13  Memory 13 BIST Busy                */
    uint32_t BUSY14:1;         /*!< bit:     14  Memory 14 BIST Busy                */
    uint32_t BUSY15:1;         /*!< bit:     15  Memory 15 BIST Busy                */
    uint32_t BUSY16:1;         /*!< bit:     16  Memory 16 BIST Busy                */
    uint32_t BUSY17:1;         /*!< bit:     17  Memory 17 BIST Busy                */
    uint32_t BUSY18:1;         /*!< bit:     18  Memory 18 BIST Busy                */
    uint32_t BUSY19:1;         /*!< bit:     19  Memory 19 BIST Busy                */
    uint32_t BUSY20:1;         /*!< bit:     20  Memory 20 BIST Busy                */
    uint32_t BUSY21:1;         /*!< bit:     21  Memory 21 BIST Busy                */
    uint32_t BUSY22:1;         /*!< bit:     22  Memory 22 BIST Busy                */
    uint32_t BUSY23:1;         /*!< bit:     23  Memory 23 BIST Busy                */
    uint32_t BUSY24:1;         /*!< bit:     24  Memory 24 BIST Busy                */
    uint32_t BUSY25:1;         /*!< bit:     25  Memory 25 BIST Busy                */
    uint32_t BUSY26:1;         /*!< bit:     26  Memory 26 BIST Busy                */
    uint32_t BUSY27:1;         /*!< bit:     27  Memory 27 BIST Busy                */
    uint32_t BUSY28:1;         /*!< bit:     28  Memory 28 BIST Busy                */
    uint32_t :3;               /*!< bit: 29..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t BUSY:29;          /*!< bit:  0..28  Memory x BIST Busy                 */
    uint32_t :3;               /*!< bit: 29..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_MBBUSY0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_MBBUSY0_OFFSET          0x0068       /**< \brief (DSU_MBBUSY0 offset) MBIST Memory Busy 0 */
#define DSU_MBBUSY0_RESETVALUE      _U_(0x00000000) /**< \brief (DSU_MBBUSY0 reset_value) MBIST Memory Busy 0 */

#define DSU_MBBUSY0_BUSY0_Pos       0            /**< \brief (DSU_MBBUSY0) Memory 0 BIST Busy */
#define DSU_MBBUSY0_BUSY0           (_U_(1) << DSU_MBBUSY0_BUSY0_Pos)
#define DSU_MBBUSY0_BUSY1_Pos       1            /**< \brief (DSU_MBBUSY0) Memory 1 BIST Busy */
#define DSU_MBBUSY0_BUSY1           (_U_(1) << DSU_MBBUSY0_BUSY1_Pos)
#define DSU_MBBUSY0_BUSY2_Pos       2            /**< \brief (DSU_MBBUSY0) Memory 2 BIST Busy */
#define DSU_MBBUSY0_BUSY2           (_U_(1) << DSU_MBBUSY0_BUSY2_Pos)
#define DSU_MBBUSY0_BUSY3_Pos       3            /**< \brief (DSU_MBBUSY0) Memory 3 BIST Busy */
#define DSU_MBBUSY0_BUSY3           (_U_(1) << DSU_MBBUSY0_BUSY3_Pos)
#define DSU_MBBUSY0_BUSY4_Pos       4            /**< \brief (DSU_MBBUSY0) Memory 4 BIST Busy */
#define DSU_MBBUSY0_BUSY4           (_U_(1) << DSU_MBBUSY0_BUSY4_Pos)
#define DSU_MBBUSY0_BUSY5_Pos       5            /**< \brief (DSU_MBBUSY0) Memory 5 BIST Busy */
#define DSU_MBBUSY0_BUSY5           (_U_(1) << DSU_MBBUSY0_BUSY5_Pos)
#define DSU_MBBUSY0_BUSY6_Pos       6            /**< \brief (DSU_MBBUSY0) Memory 6 BIST Busy */
#define DSU_MBBUSY0_BUSY6           (_U_(1) << DSU_MBBUSY0_BUSY6_Pos)
#define DSU_MBBUSY0_BUSY7_Pos       7            /**< \brief (DSU_MBBUSY0) Memory 7 BIST Busy */
#define DSU_MBBUSY0_BUSY7           (_U_(1) << DSU_MBBUSY0_BUSY7_Pos)
#define DSU_MBBUSY0_BUSY8_Pos       8            /**< \brief (DSU_MBBUSY0) Memory 8 BIST Busy */
#define DSU_MBBUSY0_BUSY8           (_U_(1) << DSU_MBBUSY0_BUSY8_Pos)
#define DSU_MBBUSY0_BUSY9_Pos       9            /**< \brief (DSU_MBBUSY0) Memory 9 BIST Busy */
#define DSU_MBBUSY0_BUSY9           (_U_(1) << DSU_MBBUSY0_BUSY9_Pos)
#define DSU_MBBUSY0_BUSY10_Pos      10           /**< \brief (DSU_MBBUSY0) Memory 10 BIST Busy */
#define DSU_MBBUSY0_BUSY10          (_U_(1) << DSU_MBBUSY0_BUSY10_Pos)
#define DSU_MBBUSY0_BUSY11_Pos      11           /**< \brief (DSU_MBBUSY0) Memory 11 BIST Busy */
#define DSU_MBBUSY0_BUSY11          (_U_(1) << DSU_MBBUSY0_BUSY11_Pos)
#define DSU_MBBUSY0_BUSY12_Pos      12           /**< \brief (DSU_MBBUSY0) Memory 12 BIST Busy */
#define DSU_MBBUSY0_BUSY12          (_U_(1) << DSU_MBBUSY0_BUSY12_Pos)
#define DSU_MBBUSY0_BUSY13_Pos      13           /**< \brief (DSU_MBBUSY0) Memory 13 BIST Busy */
#define DSU_MBBUSY0_BUSY13          (_U_(1) << DSU_MBBUSY0_BUSY13_Pos)
#define DSU_MBBUSY0_BUSY14_Pos      14           /**< \brief (DSU_MBBUSY0) Memory 14 BIST Busy */
#define DSU_MBBUSY0_BUSY14          (_U_(1) << DSU_MBBUSY0_BUSY14_Pos)
#define DSU_MBBUSY0_BUSY15_Pos      15           /**< \brief (DSU_MBBUSY0) Memory 15 BIST Busy */
#define DSU_MBBUSY0_BUSY15          (_U_(1) << DSU_MBBUSY0_BUSY15_Pos)
#define DSU_MBBUSY0_BUSY16_Pos      16           /**< \brief (DSU_MBBUSY0) Memory 16 BIST Busy */
#define DSU_MBBUSY0_BUSY16          (_U_(1) << DSU_MBBUSY0_BUSY16_Pos)
#define DSU_MBBUSY0_BUSY17_Pos      17           /**< \brief (DSU_MBBUSY0) Memory 17 BIST Busy */
#define DSU_MBBUSY0_BUSY17          (_U_(1) << DSU_MBBUSY0_BUSY17_Pos)
#define DSU_MBBUSY0_BUSY18_Pos      18           /**< \brief (DSU_MBBUSY0) Memory 18 BIST Busy */
#define DSU_MBBUSY0_BUSY18          (_U_(1) << DSU_MBBUSY0_BUSY18_Pos)
#define DSU_MBBUSY0_BUSY19_Pos      19           /**< \brief (DSU_MBBUSY0) Memory 19 BIST Busy */
#define DSU_MBBUSY0_BUSY19          (_U_(1) << DSU_MBBUSY0_BUSY19_Pos)
#define DSU_MBBUSY0_BUSY20_Pos      20           /**< \brief (DSU_MBBUSY0) Memory 20 BIST Busy */
#define DSU_MBBUSY0_BUSY20          (_U_(1) << DSU_MBBUSY0_BUSY20_Pos)
#define DSU_MBBUSY0_BUSY21_Pos      21           /**< \brief (DSU_MBBUSY0) Memory 21 BIST Busy */
#define DSU_MBBUSY0_BUSY21          (_U_(1) << DSU_MBBUSY0_BUSY21_Pos)
#define DSU_MBBUSY0_BUSY22_Pos      22           /**< \brief (DSU_MBBUSY0) Memory 22 BIST Busy */
#define DSU_MBBUSY0_BUSY22          (_U_(1) << DSU_MBBUSY0_BUSY22_Pos)
#define DSU_MBBUSY0_BUSY23_Pos      23           /**< \brief (DSU_MBBUSY0) Memory 23 BIST Busy */
#define DSU_MBBUSY0_BUSY23          (_U_(1) << DSU_MBBUSY0_BUSY23_Pos)
#define DSU_MBBUSY0_BUSY24_Pos      24           /**< \brief (DSU_MBBUSY0) Memory 24 BIST Busy */
#define DSU_MBBUSY0_BUSY24          (_U_(1) << DSU_MBBUSY0_BUSY24_Pos)
#define DSU_MBBUSY0_BUSY25_Pos      25           /**< \brief (DSU_MBBUSY0) Memory 25 BIST Busy */
#define DSU_MBBUSY0_BUSY25          (_U_(1) << DSU_MBBUSY0_BUSY25_Pos)
#define DSU_MBBUSY0_BUSY26_Pos      26           /**< \brief (DSU_MBBUSY0) Memory 26 BIST Busy */
#define DSU_MBBUSY0_BUSY26          (_U_(1) << DSU_MBBUSY0_BUSY26_Pos)
#define DSU_MBBUSY0_BUSY27_Pos      27           /**< \brief (DSU_MBBUSY0) Memory 27 BIST Busy */
#define DSU_MBBUSY0_BUSY27          (_U_(1) << DSU_MBBUSY0_BUSY27_Pos)
#define DSU_MBBUSY0_BUSY28_Pos      28           /**< \brief (DSU_MBBUSY0) Memory 28 BIST Busy */
#define DSU_MBBUSY0_BUSY28          (_U_(1) << DSU_MBBUSY0_BUSY28_Pos)
#define DSU_MBBUSY0_BUSY_Pos        0            /**< \brief (DSU_MBBUSY0) Memory x BIST Busy */
#define DSU_MBBUSY0_BUSY_Msk        (_U_(0x1FFFFFFF) << DSU_MBBUSY0_BUSY_Pos)
#define DSU_MBBUSY0_BUSY(value)     (DSU_MBBUSY0_BUSY_Msk & ((value) << DSU_MBBUSY0_BUSY_Pos))
#define DSU_MBBUSY0_MASK            _U_(0x1FFFFFFF) /**< \brief (DSU_MBBUSY0) MASK Register */

/* -------- DSU_MBSTATUS0 : (DSU Offset: 0x0070) (R/W 32) MBIST Memory Status 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t STATUS0:1;        /*!< bit:      0  Memory 0 MBIST Status              */
    uint32_t STATUS1:1;        /*!< bit:      1  Memory 1 MBIST Status              */
    uint32_t STATUS2:1;        /*!< bit:      2  Memory 2 MBIST Status              */
    uint32_t STATUS3:1;        /*!< bit:      3  Memory 3 MBIST Status              */
    uint32_t STATUS4:1;        /*!< bit:      4  Memory 4 MBIST Status              */
    uint32_t STATUS5:1;        /*!< bit:      5  Memory 5 MBIST Status              */
    uint32_t STATUS6:1;        /*!< bit:      6  Memory 6 MBIST Status              */
    uint32_t STATUS7:1;        /*!< bit:      7  Memory 7 MBIST Status              */
    uint32_t STATUS8:1;        /*!< bit:      8  Memory 8 MBIST Status              */
    uint32_t STATUS9:1;        /*!< bit:      9  Memory 9 MBIST Status              */
    uint32_t STATUS10:1;       /*!< bit:     10  Memory 10 MBIST Status             */
    uint32_t STATUS11:1;       /*!< bit:     11  Memory 11 MBIST Status             */
    uint32_t STATUS12:1;       /*!< bit:     12  Memory 12 MBIST Status             */
    uint32_t STATUS13:1;       /*!< bit:     13  Memory 13 MBIST Status             */
    uint32_t STATUS14:1;       /*!< bit:     14  Memory 14 MBIST Status             */
    uint32_t STATUS15:1;       /*!< bit:     15  Memory 15 MBIST Status             */
    uint32_t STATUS16:1;       /*!< bit:     16  Memory 16 MBIST Status             */
    uint32_t STATUS17:1;       /*!< bit:     17  Memory 17 MBIST Status             */
    uint32_t STATUS18:1;       /*!< bit:     18  Memory 18 MBIST Status             */
    uint32_t STATUS19:1;       /*!< bit:     19  Memory 19 MBIST Status             */
    uint32_t STATUS20:1;       /*!< bit:     20  Memory 20 MBIST Status             */
    uint32_t STATUS21:1;       /*!< bit:     21  Memory 21 MBIST Status             */
    uint32_t STATUS22:1;       /*!< bit:     22  Memory 22 MBIST Status             */
    uint32_t STATUS23:1;       /*!< bit:     23  Memory 23 MBIST Status             */
    uint32_t STATUS24:1;       /*!< bit:     24  Memory 24 MBIST Status             */
    uint32_t STATUS25:1;       /*!< bit:     25  Memory 25 MBIST Status             */
    uint32_t STATUS26:1;       /*!< bit:     26  Memory 26 MBIST Status             */
    uint32_t STATUS27:1;       /*!< bit:     27  Memory 27 MBIST Status             */
    uint32_t STATUS28:1;       /*!< bit:     28  Memory 28 MBIST Status             */
    uint32_t :3;               /*!< bit: 29..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t STATUS:29;        /*!< bit:  0..28  Memory x MBIST Status              */
    uint32_t :3;               /*!< bit: 29..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_MBSTATUS0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_MBSTATUS0_OFFSET        0x0070       /**< \brief (DSU_MBSTATUS0 offset) MBIST Memory Status 0 */
#define DSU_MBSTATUS0_RESETVALUE    _U_(0x00000000) /**< \brief (DSU_MBSTATUS0 reset_value) MBIST Memory Status 0 */

#define DSU_MBSTATUS0_STATUS0_Pos   0            /**< \brief (DSU_MBSTATUS0) Memory 0 MBIST Status */
#define DSU_MBSTATUS0_STATUS0       (_U_(1) << DSU_MBSTATUS0_STATUS0_Pos)
#define DSU_MBSTATUS0_STATUS1_Pos   1            /**< \brief (DSU_MBSTATUS0) Memory 1 MBIST Status */
#define DSU_MBSTATUS0_STATUS1       (_U_(1) << DSU_MBSTATUS0_STATUS1_Pos)
#define DSU_MBSTATUS0_STATUS2_Pos   2            /**< \brief (DSU_MBSTATUS0) Memory 2 MBIST Status */
#define DSU_MBSTATUS0_STATUS2       (_U_(1) << DSU_MBSTATUS0_STATUS2_Pos)
#define DSU_MBSTATUS0_STATUS3_Pos   3            /**< \brief (DSU_MBSTATUS0) Memory 3 MBIST Status */
#define DSU_MBSTATUS0_STATUS3       (_U_(1) << DSU_MBSTATUS0_STATUS3_Pos)
#define DSU_MBSTATUS0_STATUS4_Pos   4            /**< \brief (DSU_MBSTATUS0) Memory 4 MBIST Status */
#define DSU_MBSTATUS0_STATUS4       (_U_(1) << DSU_MBSTATUS0_STATUS4_Pos)
#define DSU_MBSTATUS0_STATUS5_Pos   5            /**< \brief (DSU_MBSTATUS0) Memory 5 MBIST Status */
#define DSU_MBSTATUS0_STATUS5       (_U_(1) << DSU_MBSTATUS0_STATUS5_Pos)
#define DSU_MBSTATUS0_STATUS6_Pos   6            /**< \brief (DSU_MBSTATUS0) Memory 6 MBIST Status */
#define DSU_MBSTATUS0_STATUS6       (_U_(1) << DSU_MBSTATUS0_STATUS6_Pos)
#define DSU_MBSTATUS0_STATUS7_Pos   7            /**< \brief (DSU_MBSTATUS0) Memory 7 MBIST Status */
#define DSU_MBSTATUS0_STATUS7       (_U_(1) << DSU_MBSTATUS0_STATUS7_Pos)
#define DSU_MBSTATUS0_STATUS8_Pos   8            /**< \brief (DSU_MBSTATUS0) Memory 8 MBIST Status */
#define DSU_MBSTATUS0_STATUS8       (_U_(1) << DSU_MBSTATUS0_STATUS8_Pos)
#define DSU_MBSTATUS0_STATUS9_Pos   9            /**< \brief (DSU_MBSTATUS0) Memory 9 MBIST Status */
#define DSU_MBSTATUS0_STATUS9       (_U_(1) << DSU_MBSTATUS0_STATUS9_Pos)
#define DSU_MBSTATUS0_STATUS10_Pos  10           /**< \brief (DSU_MBSTATUS0) Memory 10 MBIST Status */
#define DSU_MBSTATUS0_STATUS10      (_U_(1) << DSU_MBSTATUS0_STATUS10_Pos)
#define DSU_MBSTATUS0_STATUS11_Pos  11           /**< \brief (DSU_MBSTATUS0) Memory 11 MBIST Status */
#define DSU_MBSTATUS0_STATUS11      (_U_(1) << DSU_MBSTATUS0_STATUS11_Pos)
#define DSU_MBSTATUS0_STATUS12_Pos  12           /**< \brief (DSU_MBSTATUS0) Memory 12 MBIST Status */
#define DSU_MBSTATUS0_STATUS12      (_U_(1) << DSU_MBSTATUS0_STATUS12_Pos)
#define DSU_MBSTATUS0_STATUS13_Pos  13           /**< \brief (DSU_MBSTATUS0) Memory 13 MBIST Status */
#define DSU_MBSTATUS0_STATUS13      (_U_(1) << DSU_MBSTATUS0_STATUS13_Pos)
#define DSU_MBSTATUS0_STATUS14_Pos  14           /**< \brief (DSU_MBSTATUS0) Memory 14 MBIST Status */
#define DSU_MBSTATUS0_STATUS14      (_U_(1) << DSU_MBSTATUS0_STATUS14_Pos)
#define DSU_MBSTATUS0_STATUS15_Pos  15           /**< \brief (DSU_MBSTATUS0) Memory 15 MBIST Status */
#define DSU_MBSTATUS0_STATUS15      (_U_(1) << DSU_MBSTATUS0_STATUS15_Pos)
#define DSU_MBSTATUS0_STATUS16_Pos  16           /**< \brief (DSU_MBSTATUS0) Memory 16 MBIST Status */
#define DSU_MBSTATUS0_STATUS16      (_U_(1) << DSU_MBSTATUS0_STATUS16_Pos)
#define DSU_MBSTATUS0_STATUS17_Pos  17           /**< \brief (DSU_MBSTATUS0) Memory 17 MBIST Status */
#define DSU_MBSTATUS0_STATUS17      (_U_(1) << DSU_MBSTATUS0_STATUS17_Pos)
#define DSU_MBSTATUS0_STATUS18_Pos  18           /**< \brief (DSU_MBSTATUS0) Memory 18 MBIST Status */
#define DSU_MBSTATUS0_STATUS18      (_U_(1) << DSU_MBSTATUS0_STATUS18_Pos)
#define DSU_MBSTATUS0_STATUS19_Pos  19           /**< \brief (DSU_MBSTATUS0) Memory 19 MBIST Status */
#define DSU_MBSTATUS0_STATUS19      (_U_(1) << DSU_MBSTATUS0_STATUS19_Pos)
#define DSU_MBSTATUS0_STATUS20_Pos  20           /**< \brief (DSU_MBSTATUS0) Memory 20 MBIST Status */
#define DSU_MBSTATUS0_STATUS20      (_U_(1) << DSU_MBSTATUS0_STATUS20_Pos)
#define DSU_MBSTATUS0_STATUS21_Pos  21           /**< \brief (DSU_MBSTATUS0) Memory 21 MBIST Status */
#define DSU_MBSTATUS0_STATUS21      (_U_(1) << DSU_MBSTATUS0_STATUS21_Pos)
#define DSU_MBSTATUS0_STATUS22_Pos  22           /**< \brief (DSU_MBSTATUS0) Memory 22 MBIST Status */
#define DSU_MBSTATUS0_STATUS22      (_U_(1) << DSU_MBSTATUS0_STATUS22_Pos)
#define DSU_MBSTATUS0_STATUS23_Pos  23           /**< \brief (DSU_MBSTATUS0) Memory 23 MBIST Status */
#define DSU_MBSTATUS0_STATUS23      (_U_(1) << DSU_MBSTATUS0_STATUS23_Pos)
#define DSU_MBSTATUS0_STATUS24_Pos  24           /**< \brief (DSU_MBSTATUS0) Memory 24 MBIST Status */
#define DSU_MBSTATUS0_STATUS24      (_U_(1) << DSU_MBSTATUS0_STATUS24_Pos)
#define DSU_MBSTATUS0_STATUS25_Pos  25           /**< \brief (DSU_MBSTATUS0) Memory 25 MBIST Status */
#define DSU_MBSTATUS0_STATUS25      (_U_(1) << DSU_MBSTATUS0_STATUS25_Pos)
#define DSU_MBSTATUS0_STATUS26_Pos  26           /**< \brief (DSU_MBSTATUS0) Memory 26 MBIST Status */
#define DSU_MBSTATUS0_STATUS26      (_U_(1) << DSU_MBSTATUS0_STATUS26_Pos)
#define DSU_MBSTATUS0_STATUS27_Pos  27           /**< \brief (DSU_MBSTATUS0) Memory 27 MBIST Status */
#define DSU_MBSTATUS0_STATUS27      (_U_(1) << DSU_MBSTATUS0_STATUS27_Pos)
#define DSU_MBSTATUS0_STATUS28_Pos  28           /**< \brief (DSU_MBSTATUS0) Memory 28 MBIST Status */
#define DSU_MBSTATUS0_STATUS28      (_U_(1) << DSU_MBSTATUS0_STATUS28_Pos)
#define DSU_MBSTATUS0_STATUS_Pos    0            /**< \brief (DSU_MBSTATUS0) Memory x MBIST Status */
#define DSU_MBSTATUS0_STATUS_Msk    (_U_(0x1FFFFFFF) << DSU_MBSTATUS0_STATUS_Pos)
#define DSU_MBSTATUS0_STATUS(value) (DSU_MBSTATUS0_STATUS_Msk & ((value) << DSU_MBSTATUS0_STATUS_Pos))
#define DSU_MBSTATUS0_MASK          _U_(0x1FFFFFFF) /**< \brief (DSU_MBSTATUS0) MASK Register */

/* -------- DSU_DCFG : (DSU Offset: 0x00F0) (R/W 32) Device Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DCFG:32;          /*!< bit:  0..31  Device Configuration               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_DCFG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_DCFG_OFFSET             0x00F0       /**< \brief (DSU_DCFG offset) Device Configuration */
#define DSU_DCFG_RESETVALUE         _U_(0x00000000) /**< \brief (DSU_DCFG reset_value) Device Configuration */

#define DSU_DCFG_DCFG_Pos           0            /**< \brief (DSU_DCFG) Device Configuration */
#define DSU_DCFG_DCFG_Msk           (_U_(0xFFFFFFFF) << DSU_DCFG_DCFG_Pos)
#define DSU_DCFG_DCFG(value)        (DSU_DCFG_DCFG_Msk & ((value) << DSU_DCFG_DCFG_Pos))
#define DSU_DCFG_MASK               _U_(0xFFFFFFFF) /**< \brief (DSU_DCFG) MASK Register */

/* -------- DSU_ENTRY0 : (DSU Offset: 0x1000) (R/  32) CoreSight ROM Table Entry 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EPRES:1;          /*!< bit:      0  Entry Present                      */
    uint32_t FMT:1;            /*!< bit:      1  Format                             */
    uint32_t :10;              /*!< bit:  2..11  Reserved                           */
    uint32_t ADDOFF:20;        /*!< bit: 12..31  Address Offset                     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_ENTRY0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_ENTRY0_OFFSET           0x1000       /**< \brief (DSU_ENTRY0 offset) CoreSight ROM Table Entry 0 */
#define DSU_ENTRY0_RESETVALUE       _U_(0x9F0FC002) /**< \brief (DSU_ENTRY0 reset_value) CoreSight ROM Table Entry 0 */

#define DSU_ENTRY0_EPRES_Pos        0            /**< \brief (DSU_ENTRY0) Entry Present */
#define DSU_ENTRY0_EPRES            (_U_(0x1) << DSU_ENTRY0_EPRES_Pos)
#define DSU_ENTRY0_FMT_Pos          1            /**< \brief (DSU_ENTRY0) Format */
#define DSU_ENTRY0_FMT              (_U_(0x1) << DSU_ENTRY0_FMT_Pos)
#define DSU_ENTRY0_ADDOFF_Pos       12           /**< \brief (DSU_ENTRY0) Address Offset */
#define DSU_ENTRY0_ADDOFF_Msk       (_U_(0xFFFFF) << DSU_ENTRY0_ADDOFF_Pos)
#define DSU_ENTRY0_ADDOFF(value)    (DSU_ENTRY0_ADDOFF_Msk & ((value) << DSU_ENTRY0_ADDOFF_Pos))
#define DSU_ENTRY0_MASK             _U_(0xFFFFF003) /**< \brief (DSU_ENTRY0) MASK Register */

/* -------- DSU_ENTRY1 : (DSU Offset: 0x1004) (R/  32) CoreSight ROM Table Entry 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_ENTRY1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_ENTRY1_OFFSET           0x1004       /**< \brief (DSU_ENTRY1 offset) CoreSight ROM Table Entry 1 */
#define DSU_ENTRY1_RESETVALUE       _U_(0x00000000) /**< \brief (DSU_ENTRY1 reset_value) CoreSight ROM Table Entry 1 */
#define DSU_ENTRY1_MASK             _U_(0xFFFFFFFF) /**< \brief (DSU_ENTRY1) MASK Register */

/* -------- DSU_END : (DSU Offset: 0x1008) (R/  32) CoreSight ROM Table End -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t END:32;           /*!< bit:  0..31  End Marker                         */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_END_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_END_OFFSET              0x1008       /**< \brief (DSU_END offset) CoreSight ROM Table End */
#define DSU_END_RESETVALUE          _U_(0x00000000) /**< \brief (DSU_END reset_value) CoreSight ROM Table End */

#define DSU_END_END_Pos             0            /**< \brief (DSU_END) End Marker */
#define DSU_END_END_Msk             (_U_(0xFFFFFFFF) << DSU_END_END_Pos)
#define DSU_END_END(value)          (DSU_END_END_Msk & ((value) << DSU_END_END_Pos))
#define DSU_END_MASK                _U_(0xFFFFFFFF) /**< \brief (DSU_END) MASK Register */

/* -------- DSU_MEMTYPE : (DSU Offset: 0x1FCC) (R/  32) CoreSight ROM Table Memory Type -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SMEMP:1;          /*!< bit:      0  System Memory Present              */
    uint32_t :31;              /*!< bit:  1..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_MEMTYPE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_MEMTYPE_OFFSET          0x1FCC       /**< \brief (DSU_MEMTYPE offset) CoreSight ROM Table Memory Type */
#define DSU_MEMTYPE_RESETVALUE      _U_(0x00000000) /**< \brief (DSU_MEMTYPE reset_value) CoreSight ROM Table Memory Type */

#define DSU_MEMTYPE_SMEMP_Pos       0            /**< \brief (DSU_MEMTYPE) System Memory Present */
#define DSU_MEMTYPE_SMEMP           (_U_(0x1) << DSU_MEMTYPE_SMEMP_Pos)
#define DSU_MEMTYPE_MASK            _U_(0x00000001) /**< \brief (DSU_MEMTYPE) MASK Register */

/* -------- DSU_PID4 : (DSU Offset: 0x1FD0) (R/  32) Peripheral Identification 4 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t JEPCC:4;          /*!< bit:  0.. 3  JEP-106 Continuation Code          */
    uint32_t FKBC:4;           /*!< bit:  4.. 7  4KB count                          */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_PID4_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_PID4_OFFSET             0x1FD0       /**< \brief (DSU_PID4 offset) Peripheral Identification 4 */
#define DSU_PID4_RESETVALUE         _U_(0x00000000) /**< \brief (DSU_PID4 reset_value) Peripheral Identification 4 */

#define DSU_PID4_JEPCC_Pos          0            /**< \brief (DSU_PID4) JEP-106 Continuation Code */
#define DSU_PID4_JEPCC_Msk          (_U_(0xF) << DSU_PID4_JEPCC_Pos)
#define DSU_PID4_JEPCC(value)       (DSU_PID4_JEPCC_Msk & ((value) << DSU_PID4_JEPCC_Pos))
#define DSU_PID4_FKBC_Pos           4            /**< \brief (DSU_PID4) 4KB count */
#define DSU_PID4_FKBC_Msk           (_U_(0xF) << DSU_PID4_FKBC_Pos)
#define DSU_PID4_FKBC(value)        (DSU_PID4_FKBC_Msk & ((value) << DSU_PID4_FKBC_Pos))
#define DSU_PID4_MASK               _U_(0x000000FF) /**< \brief (DSU_PID4) MASK Register */

/* -------- DSU_PID5 : (DSU Offset: 0x1FD4) (R/  32) Peripheral Identification 5 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_PID5_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_PID5_OFFSET             0x1FD4       /**< \brief (DSU_PID5 offset) Peripheral Identification 5 */
#define DSU_PID5_RESETVALUE         _U_(0x00000000) /**< \brief (DSU_PID5 reset_value) Peripheral Identification 5 */
#define DSU_PID5_MASK               _U_(0x00000000) /**< \brief (DSU_PID5) MASK Register */

/* -------- DSU_PID6 : (DSU Offset: 0x1FD8) (R/  32) Peripheral Identification 6 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_PID6_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_PID6_OFFSET             0x1FD8       /**< \brief (DSU_PID6 offset) Peripheral Identification 6 */
#define DSU_PID6_RESETVALUE         _U_(0x00000000) /**< \brief (DSU_PID6 reset_value) Peripheral Identification 6 */
#define DSU_PID6_MASK               _U_(0x00000000) /**< \brief (DSU_PID6) MASK Register */

/* -------- DSU_PID7 : (DSU Offset: 0x1FDC) (R/  32) Peripheral Identification 7 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_PID7_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_PID7_OFFSET             0x1FDC       /**< \brief (DSU_PID7 offset) Peripheral Identification 7 */
#define DSU_PID7_RESETVALUE         _U_(0x00000000) /**< \brief (DSU_PID7 reset_value) Peripheral Identification 7 */
#define DSU_PID7_MASK               _U_(0x00000000) /**< \brief (DSU_PID7) MASK Register */

/* -------- DSU_PID0 : (DSU Offset: 0x1FE0) (R/  32) Peripheral Identification 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PARTNBL:8;        /*!< bit:  0.. 7  Part Number Low                    */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_PID0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_PID0_OFFSET             0x1FE0       /**< \brief (DSU_PID0 offset) Peripheral Identification 0 */
#define DSU_PID0_RESETVALUE         _U_(0x000000D0) /**< \brief (DSU_PID0 reset_value) Peripheral Identification 0 */

#define DSU_PID0_PARTNBL_Pos        0            /**< \brief (DSU_PID0) Part Number Low */
#define DSU_PID0_PARTNBL_Msk        (_U_(0xFF) << DSU_PID0_PARTNBL_Pos)
#define DSU_PID0_PARTNBL(value)     (DSU_PID0_PARTNBL_Msk & ((value) << DSU_PID0_PARTNBL_Pos))
#define DSU_PID0_MASK               _U_(0x000000FF) /**< \brief (DSU_PID0) MASK Register */

/* -------- DSU_PID1 : (DSU Offset: 0x1FE4) (R/  32) Peripheral Identification 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PARTNBH:4;        /*!< bit:  0.. 3  Part Number High                   */
    uint32_t JEPIDCL:4;        /*!< bit:  4.. 7  Low part of the JEP-106 Identity Code */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_PID1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_PID1_OFFSET             0x1FE4       /**< \brief (DSU_PID1 offset) Peripheral Identification 1 */
#define DSU_PID1_RESETVALUE         _U_(0x000000FC) /**< \brief (DSU_PID1 reset_value) Peripheral Identification 1 */

#define DSU_PID1_PARTNBH_Pos        0            /**< \brief (DSU_PID1) Part Number High */
#define DSU_PID1_PARTNBH_Msk        (_U_(0xF) << DSU_PID1_PARTNBH_Pos)
#define DSU_PID1_PARTNBH(value)     (DSU_PID1_PARTNBH_Msk & ((value) << DSU_PID1_PARTNBH_Pos))
#define DSU_PID1_JEPIDCL_Pos        4            /**< \brief (DSU_PID1) Low part of the JEP-106 Identity Code */
#define DSU_PID1_JEPIDCL_Msk        (_U_(0xF) << DSU_PID1_JEPIDCL_Pos)
#define DSU_PID1_JEPIDCL(value)     (DSU_PID1_JEPIDCL_Msk & ((value) << DSU_PID1_JEPIDCL_Pos))
#define DSU_PID1_MASK               _U_(0x000000FF) /**< \brief (DSU_PID1) MASK Register */

/* -------- DSU_PID2 : (DSU Offset: 0x1FE8) (R/  32) Peripheral Identification 2 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t JEPIDCH:3;        /*!< bit:  0.. 2  JEP-106 Identity Code High         */
    uint32_t JEPU:1;           /*!< bit:      3  JEP-106 Identity Code is used      */
    uint32_t REVISION:4;       /*!< bit:  4.. 7  Revision Number                    */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_PID2_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_PID2_OFFSET             0x1FE8       /**< \brief (DSU_PID2 offset) Peripheral Identification 2 */
#define DSU_PID2_RESETVALUE         _U_(0x00000009) /**< \brief (DSU_PID2 reset_value) Peripheral Identification 2 */

#define DSU_PID2_JEPIDCH_Pos        0            /**< \brief (DSU_PID2) JEP-106 Identity Code High */
#define DSU_PID2_JEPIDCH_Msk        (_U_(0x7) << DSU_PID2_JEPIDCH_Pos)
#define DSU_PID2_JEPIDCH(value)     (DSU_PID2_JEPIDCH_Msk & ((value) << DSU_PID2_JEPIDCH_Pos))
#define DSU_PID2_JEPU_Pos           3            /**< \brief (DSU_PID2) JEP-106 Identity Code is used */
#define DSU_PID2_JEPU               (_U_(0x1) << DSU_PID2_JEPU_Pos)
#define DSU_PID2_REVISION_Pos       4            /**< \brief (DSU_PID2) Revision Number */
#define DSU_PID2_REVISION_Msk       (_U_(0xF) << DSU_PID2_REVISION_Pos)
#define DSU_PID2_REVISION(value)    (DSU_PID2_REVISION_Msk & ((value) << DSU_PID2_REVISION_Pos))
#define DSU_PID2_MASK               _U_(0x000000FF) /**< \brief (DSU_PID2) MASK Register */

/* -------- DSU_PID3 : (DSU Offset: 0x1FEC) (R/  32) Peripheral Identification 3 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CUSMOD:4;         /*!< bit:  0.. 3  ARM CUSMOD                         */
    uint32_t REVAND:4;         /*!< bit:  4.. 7  Revision Number                    */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_PID3_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_PID3_OFFSET             0x1FEC       /**< \brief (DSU_PID3 offset) Peripheral Identification 3 */
#define DSU_PID3_RESETVALUE         _U_(0x00000000) /**< \brief (DSU_PID3 reset_value) Peripheral Identification 3 */

#define DSU_PID3_CUSMOD_Pos         0            /**< \brief (DSU_PID3) ARM CUSMOD */
#define DSU_PID3_CUSMOD_Msk         (_U_(0xF) << DSU_PID3_CUSMOD_Pos)
#define DSU_PID3_CUSMOD(value)      (DSU_PID3_CUSMOD_Msk & ((value) << DSU_PID3_CUSMOD_Pos))
#define DSU_PID3_REVAND_Pos         4            /**< \brief (DSU_PID3) Revision Number */
#define DSU_PID3_REVAND_Msk         (_U_(0xF) << DSU_PID3_REVAND_Pos)
#define DSU_PID3_REVAND(value)      (DSU_PID3_REVAND_Msk & ((value) << DSU_PID3_REVAND_Pos))
#define DSU_PID3_MASK               _U_(0x000000FF) /**< \brief (DSU_PID3) MASK Register */

/* -------- DSU_CID0 : (DSU Offset: 0x1FF0) (R/  32) Component Identification 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PREAMBLEB0:8;     /*!< bit:  0.. 7  Preamble Byte 0                    */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_CID0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_CID0_OFFSET             0x1FF0       /**< \brief (DSU_CID0 offset) Component Identification 0 */
#define DSU_CID0_RESETVALUE         _U_(0x0000000D) /**< \brief (DSU_CID0 reset_value) Component Identification 0 */

#define DSU_CID0_PREAMBLEB0_Pos     0            /**< \brief (DSU_CID0) Preamble Byte 0 */
#define DSU_CID0_PREAMBLEB0_Msk     (_U_(0xFF) << DSU_CID0_PREAMBLEB0_Pos)
#define DSU_CID0_PREAMBLEB0(value)  (DSU_CID0_PREAMBLEB0_Msk & ((value) << DSU_CID0_PREAMBLEB0_Pos))
#define DSU_CID0_MASK               _U_(0x000000FF) /**< \brief (DSU_CID0) MASK Register */

/* -------- DSU_CID1 : (DSU Offset: 0x1FF4) (R/  32) Component Identification 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PREAMBLE:4;       /*!< bit:  0.. 3  Preamble                           */
    uint32_t CCLASS:4;         /*!< bit:  4.. 7  Component Class                    */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_CID1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_CID1_OFFSET             0x1FF4       /**< \brief (DSU_CID1 offset) Component Identification 1 */
#define DSU_CID1_RESETVALUE         _U_(0x00000010) /**< \brief (DSU_CID1 reset_value) Component Identification 1 */

#define DSU_CID1_PREAMBLE_Pos       0            /**< \brief (DSU_CID1) Preamble */
#define DSU_CID1_PREAMBLE_Msk       (_U_(0xF) << DSU_CID1_PREAMBLE_Pos)
#define DSU_CID1_PREAMBLE(value)    (DSU_CID1_PREAMBLE_Msk & ((value) << DSU_CID1_PREAMBLE_Pos))
#define DSU_CID1_CCLASS_Pos         4            /**< \brief (DSU_CID1) Component Class */
#define DSU_CID1_CCLASS_Msk         (_U_(0xF) << DSU_CID1_CCLASS_Pos)
#define DSU_CID1_CCLASS(value)      (DSU_CID1_CCLASS_Msk & ((value) << DSU_CID1_CCLASS_Pos))
#define DSU_CID1_MASK               _U_(0x000000FF) /**< \brief (DSU_CID1) MASK Register */

/* -------- DSU_CID2 : (DSU Offset: 0x1FF8) (R/  32) Component Identification 2 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PREAMBLEB2:8;     /*!< bit:  0.. 7  Preamble Byte 2                    */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_CID2_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_CID2_OFFSET             0x1FF8       /**< \brief (DSU_CID2 offset) Component Identification 2 */
#define DSU_CID2_RESETVALUE         _U_(0x00000005) /**< \brief (DSU_CID2 reset_value) Component Identification 2 */

#define DSU_CID2_PREAMBLEB2_Pos     0            /**< \brief (DSU_CID2) Preamble Byte 2 */
#define DSU_CID2_PREAMBLEB2_Msk     (_U_(0xFF) << DSU_CID2_PREAMBLEB2_Pos)
#define DSU_CID2_PREAMBLEB2(value)  (DSU_CID2_PREAMBLEB2_Msk & ((value) << DSU_CID2_PREAMBLEB2_Pos))
#define DSU_CID2_MASK               _U_(0x000000FF) /**< \brief (DSU_CID2) MASK Register */

/* -------- DSU_CID3 : (DSU Offset: 0x1FFC) (R/  32) Component Identification 3 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PREAMBLEB3:8;     /*!< bit:  0.. 7  Preamble Byte 3                    */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_CID3_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_CID3_OFFSET             0x1FFC       /**< \brief (DSU_CID3 offset) Component Identification 3 */
#define DSU_CID3_RESETVALUE         _U_(0x000000B1) /**< \brief (DSU_CID3 reset_value) Component Identification 3 */

#define DSU_CID3_PREAMBLEB3_Pos     0            /**< \brief (DSU_CID3) Preamble Byte 3 */
#define DSU_CID3_PREAMBLEB3_Msk     (_U_(0xFF) << DSU_CID3_PREAMBLEB3_Pos)
#define DSU_CID3_PREAMBLEB3(value)  (DSU_CID3_PREAMBLEB3_Msk & ((value) << DSU_CID3_PREAMBLEB3_Pos))
#define DSU_CID3_MASK               _U_(0x000000FF) /**< \brief (DSU_CID3) MASK Register */

/** \brief DSU hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __O  DSU_CTRL_Type             CTRL;        /**< \brief Offset: 0x0000 ( /W  8) Control */
  __IO DSU_STATUSA_Type          STATUSA;     /**< \brief Offset: 0x0001 (R/W  8) Status A */
  __I  DSU_STATUSB_Type          STATUSB;     /**< \brief Offset: 0x0002 (R/   8) Status B */
       RoReg8                    Reserved1[0x1];
  __IO DSU_ADDR_Type             ADDR;        /**< \brief Offset: 0x0004 (R/W 32) Address */
  __IO DSU_LENGTH_Type           LENGTH;      /**< \brief Offset: 0x0008 (R/W 32) Length */
  __IO DSU_DATA_Type             DATA;        /**< \brief Offset: 0x000C (R/W 32) Data */
  __IO DSU_DCC_Type              DCC[2];      /**< \brief Offset: 0x0010 (R/W 32) Debug Communication Channel n */
  __I  DSU_DID_Type              DID;         /**< \brief Offset: 0x0018 (R/  32) Device Identification */
  __IO DSU_CFG_Type              CFG;         /**< \brief Offset: 0x001C (R/W 32) Configuration */
       RoReg8                    Reserved2[0x20];
  __IO DSU_MBCTRL_Type           MBCTRL;      /**< \brief Offset: 0x0040 (R/W 32) MBIST Control */
  __IO DSU_MBCONFIG_Type         MBCONFIG;    /**< \brief Offset: 0x0044 (R/W 32) MBIST Configuration */
  __IO DSU_MBWORD_Type           MBWORD;      /**< \brief Offset: 0x0048 (R/W 32) MBIST Background Word */
  __IO DSU_MBGSTAT_Type          MBGSTAT;     /**< \brief Offset: 0x004C (R/W 32) MBIST Global Status */
  __I  DSU_MBDFAIL_Type          MBDFAIL;     /**< \brief Offset: 0x0050 (R/  32) MBIST Fail Data */
  __I  DSU_MBDEXP_Type           MBDEXP;      /**< \brief Offset: 0x0054 (R/  32) MBIST Expected Data */
  __I  DSU_MBAFAIL_Type          MBAFAIL;     /**< \brief Offset: 0x0058 (R/  32) MBIST Fail Address */
  __I  DSU_MBCONTEXT_Type        MBCONTEXT;   /**< \brief Offset: 0x005C (R/  32) MBIST Fail Context */
  __IO DSU_MBENABLE0_Type        MBENABLE0;   /**< \brief Offset: 0x0060 (R/W 32) MBIST Memory Enable 0 */
       RoReg8                    Reserved3[0x4];
  __I  DSU_MBBUSY0_Type          MBBUSY0;     /**< \brief Offset: 0x0068 (R/  32) MBIST Memory Busy 0 */
       RoReg8                    Reserved4[0x4];
  __IO DSU_MBSTATUS0_Type        MBSTATUS0;   /**< \brief Offset: 0x0070 (R/W 32) MBIST Memory Status 0 */
       RoReg8                    Reserved5[0x7C];
  __IO DSU_DCFG_Type             DCFG[2];     /**< \brief Offset: 0x00F0 (R/W 32) Device Configuration */
       RoReg8                    Reserved6[0xF08];
  __I  DSU_ENTRY0_Type           ENTRY0;      /**< \brief Offset: 0x1000 (R/  32) CoreSight ROM Table Entry 0 */
  __I  DSU_ENTRY1_Type           ENTRY1;      /**< \brief Offset: 0x1004 (R/  32) CoreSight ROM Table Entry 1 */
  __I  DSU_END_Type              END;         /**< \brief Offset: 0x1008 (R/  32) CoreSight ROM Table End */
       RoReg8                    Reserved7[0xFC0];
  __I  DSU_MEMTYPE_Type          MEMTYPE;     /**< \brief Offset: 0x1FCC (R/  32) CoreSight ROM Table Memory Type */
  __I  DSU_PID4_Type             PID4;        /**< \brief Offset: 0x1FD0 (R/  32) Peripheral Identification 4 */
  __I  DSU_PID5_Type             PID5;        /**< \brief Offset: 0x1FD4 (R/  32) Peripheral Identification 5 */
  __I  DSU_PID6_Type             PID6;        /**< \brief Offset: 0x1FD8 (R/  32) Peripheral Identification 6 */
  __I  DSU_PID7_Type             PID7;        /**< \brief Offset: 0x1FDC (R/  32) Peripheral Identification 7 */
  __I  DSU_PID0_Type             PID0;        /**< \brief Offset: 0x1FE0 (R/  32) Peripheral Identification 0 */
  __I  DSU_PID1_Type             PID1;        /**< \brief Offset: 0x1FE4 (R/  32) Peripheral Identification 1 */
  __I  DSU_PID2_Type             PID2;        /**< \brief Offset: 0x1FE8 (R/  32) Peripheral Identification 2 */
  __I  DSU_PID3_Type             PID3;        /**< \brief Offset: 0x1FEC (R/  32) Peripheral Identification 3 */
  __I  DSU_CID0_Type             CID0;        /**< \brief Offset: 0x1FF0 (R/  32) Component Identification 0 */
  __I  DSU_CID1_Type             CID1;        /**< \brief Offset: 0x1FF4 (R/  32) Component Identification 1 */
  __I  DSU_CID2_Type             CID2;        /**< \brief Offset: 0x1FF8 (R/  32) Component Identification 2 */
  __I  DSU_CID3_Type             CID3;        /**< \brief Offset: 0x1FFC (R/  32) Component Identification 3 */
} Dsu;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_DSU_COMPONENT_ */
