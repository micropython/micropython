/**
 * \file
 *
 * \brief Component description for CCL
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

#ifndef _SAMD51_CCL_COMPONENT_
#define _SAMD51_CCL_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR CCL */
/* ========================================================================== */
/** \addtogroup SAMD51_CCL Configurable Custom Logic */
/*@{*/

#define CCL_U2225
#define REV_CCL                     0x110

/* -------- CCL_CTRL : (CCL Offset: 0x0) (R/W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  :4;               /*!< bit:  2.. 5  Reserved                           */
    uint8_t  RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint8_t  :1;               /*!< bit:      7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} CCL_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CCL_CTRL_OFFSET             0x0          /**< \brief (CCL_CTRL offset) Control */
#define CCL_CTRL_RESETVALUE         _U_(0x00)    /**< \brief (CCL_CTRL reset_value) Control */

#define CCL_CTRL_SWRST_Pos          0            /**< \brief (CCL_CTRL) Software Reset */
#define CCL_CTRL_SWRST              (_U_(0x1) << CCL_CTRL_SWRST_Pos)
#define CCL_CTRL_ENABLE_Pos         1            /**< \brief (CCL_CTRL) Enable */
#define CCL_CTRL_ENABLE             (_U_(0x1) << CCL_CTRL_ENABLE_Pos)
#define CCL_CTRL_RUNSTDBY_Pos       6            /**< \brief (CCL_CTRL) Run in Standby */
#define CCL_CTRL_RUNSTDBY           (_U_(0x1) << CCL_CTRL_RUNSTDBY_Pos)
#define CCL_CTRL_MASK               _U_(0x43)    /**< \brief (CCL_CTRL) MASK Register */

/* -------- CCL_SEQCTRL : (CCL Offset: 0x4) (R/W  8) SEQ Control x -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SEQSEL:4;         /*!< bit:  0.. 3  Sequential Selection               */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} CCL_SEQCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CCL_SEQCTRL_OFFSET          0x4          /**< \brief (CCL_SEQCTRL offset) SEQ Control x */
#define CCL_SEQCTRL_RESETVALUE      _U_(0x00)    /**< \brief (CCL_SEQCTRL reset_value) SEQ Control x */

#define CCL_SEQCTRL_SEQSEL_Pos      0            /**< \brief (CCL_SEQCTRL) Sequential Selection */
#define CCL_SEQCTRL_SEQSEL_Msk      (_U_(0xF) << CCL_SEQCTRL_SEQSEL_Pos)
#define CCL_SEQCTRL_SEQSEL(value)   (CCL_SEQCTRL_SEQSEL_Msk & ((value) << CCL_SEQCTRL_SEQSEL_Pos))
#define   CCL_SEQCTRL_SEQSEL_DISABLE_Val  _U_(0x0)   /**< \brief (CCL_SEQCTRL) Sequential logic is disabled */
#define   CCL_SEQCTRL_SEQSEL_DFF_Val      _U_(0x1)   /**< \brief (CCL_SEQCTRL) D flip flop */
#define   CCL_SEQCTRL_SEQSEL_JK_Val       _U_(0x2)   /**< \brief (CCL_SEQCTRL) JK flip flop */
#define   CCL_SEQCTRL_SEQSEL_LATCH_Val    _U_(0x3)   /**< \brief (CCL_SEQCTRL) D latch */
#define   CCL_SEQCTRL_SEQSEL_RS_Val       _U_(0x4)   /**< \brief (CCL_SEQCTRL) RS latch */
#define CCL_SEQCTRL_SEQSEL_DISABLE  (CCL_SEQCTRL_SEQSEL_DISABLE_Val << CCL_SEQCTRL_SEQSEL_Pos)
#define CCL_SEQCTRL_SEQSEL_DFF      (CCL_SEQCTRL_SEQSEL_DFF_Val    << CCL_SEQCTRL_SEQSEL_Pos)
#define CCL_SEQCTRL_SEQSEL_JK       (CCL_SEQCTRL_SEQSEL_JK_Val     << CCL_SEQCTRL_SEQSEL_Pos)
#define CCL_SEQCTRL_SEQSEL_LATCH    (CCL_SEQCTRL_SEQSEL_LATCH_Val  << CCL_SEQCTRL_SEQSEL_Pos)
#define CCL_SEQCTRL_SEQSEL_RS       (CCL_SEQCTRL_SEQSEL_RS_Val     << CCL_SEQCTRL_SEQSEL_Pos)
#define CCL_SEQCTRL_MASK            _U_(0x0F)    /**< \brief (CCL_SEQCTRL) MASK Register */

/* -------- CCL_LUTCTRL : (CCL Offset: 0x8) (R/W 32) LUT Control x -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t ENABLE:1;         /*!< bit:      1  LUT Enable                         */
    uint32_t :2;               /*!< bit:  2.. 3  Reserved                           */
    uint32_t FILTSEL:2;        /*!< bit:  4.. 5  Filter Selection                   */
    uint32_t :1;               /*!< bit:      6  Reserved                           */
    uint32_t EDGESEL:1;        /*!< bit:      7  Edge Selection                     */
    uint32_t INSEL0:4;         /*!< bit:  8..11  Input Selection 0                  */
    uint32_t INSEL1:4;         /*!< bit: 12..15  Input Selection 1                  */
    uint32_t INSEL2:4;         /*!< bit: 16..19  Input Selection 2                  */
    uint32_t INVEI:1;          /*!< bit:     20  Inverted Event Input Enable        */
    uint32_t LUTEI:1;          /*!< bit:     21  LUT Event Input Enable             */
    uint32_t LUTEO:1;          /*!< bit:     22  LUT Event Output Enable            */
    uint32_t :1;               /*!< bit:     23  Reserved                           */
    uint32_t TRUTH:8;          /*!< bit: 24..31  Truth Value                        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} CCL_LUTCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define CCL_LUTCTRL_OFFSET          0x8          /**< \brief (CCL_LUTCTRL offset) LUT Control x */
#define CCL_LUTCTRL_RESETVALUE      _U_(0x00000000) /**< \brief (CCL_LUTCTRL reset_value) LUT Control x */

#define CCL_LUTCTRL_ENABLE_Pos      1            /**< \brief (CCL_LUTCTRL) LUT Enable */
#define CCL_LUTCTRL_ENABLE          (_U_(0x1) << CCL_LUTCTRL_ENABLE_Pos)
#define CCL_LUTCTRL_FILTSEL_Pos     4            /**< \brief (CCL_LUTCTRL) Filter Selection */
#define CCL_LUTCTRL_FILTSEL_Msk     (_U_(0x3) << CCL_LUTCTRL_FILTSEL_Pos)
#define CCL_LUTCTRL_FILTSEL(value)  (CCL_LUTCTRL_FILTSEL_Msk & ((value) << CCL_LUTCTRL_FILTSEL_Pos))
#define   CCL_LUTCTRL_FILTSEL_DISABLE_Val _U_(0x0)   /**< \brief (CCL_LUTCTRL) Filter disabled */
#define   CCL_LUTCTRL_FILTSEL_SYNCH_Val   _U_(0x1)   /**< \brief (CCL_LUTCTRL) Synchronizer enabled */
#define   CCL_LUTCTRL_FILTSEL_FILTER_Val  _U_(0x2)   /**< \brief (CCL_LUTCTRL) Filter enabled */
#define CCL_LUTCTRL_FILTSEL_DISABLE (CCL_LUTCTRL_FILTSEL_DISABLE_Val << CCL_LUTCTRL_FILTSEL_Pos)
#define CCL_LUTCTRL_FILTSEL_SYNCH   (CCL_LUTCTRL_FILTSEL_SYNCH_Val << CCL_LUTCTRL_FILTSEL_Pos)
#define CCL_LUTCTRL_FILTSEL_FILTER  (CCL_LUTCTRL_FILTSEL_FILTER_Val << CCL_LUTCTRL_FILTSEL_Pos)
#define CCL_LUTCTRL_EDGESEL_Pos     7            /**< \brief (CCL_LUTCTRL) Edge Selection */
#define CCL_LUTCTRL_EDGESEL         (_U_(0x1) << CCL_LUTCTRL_EDGESEL_Pos)
#define CCL_LUTCTRL_INSEL0_Pos      8            /**< \brief (CCL_LUTCTRL) Input Selection 0 */
#define CCL_LUTCTRL_INSEL0_Msk      (_U_(0xF) << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0(value)   (CCL_LUTCTRL_INSEL0_Msk & ((value) << CCL_LUTCTRL_INSEL0_Pos))
#define   CCL_LUTCTRL_INSEL0_MASK_Val     _U_(0x0)   /**< \brief (CCL_LUTCTRL) Masked input */
#define   CCL_LUTCTRL_INSEL0_FEEDBACK_Val _U_(0x1)   /**< \brief (CCL_LUTCTRL) Feedback input source */
#define   CCL_LUTCTRL_INSEL0_LINK_Val     _U_(0x2)   /**< \brief (CCL_LUTCTRL) Linked LUT input source */
#define   CCL_LUTCTRL_INSEL0_EVENT_Val    _U_(0x3)   /**< \brief (CCL_LUTCTRL) Event input source */
#define   CCL_LUTCTRL_INSEL0_IO_Val       _U_(0x4)   /**< \brief (CCL_LUTCTRL) I/O pin input source */
#define   CCL_LUTCTRL_INSEL0_AC_Val       _U_(0x5)   /**< \brief (CCL_LUTCTRL) AC input source */
#define   CCL_LUTCTRL_INSEL0_TC_Val       _U_(0x6)   /**< \brief (CCL_LUTCTRL) TC input source */
#define   CCL_LUTCTRL_INSEL0_ALTTC_Val    _U_(0x7)   /**< \brief (CCL_LUTCTRL) Alternate TC input source */
#define   CCL_LUTCTRL_INSEL0_TCC_Val      _U_(0x8)   /**< \brief (CCL_LUTCTRL) TCC input source */
#define   CCL_LUTCTRL_INSEL0_SERCOM_Val   _U_(0x9)   /**< \brief (CCL_LUTCTRL) SERCOM input source */
#define CCL_LUTCTRL_INSEL0_MASK     (CCL_LUTCTRL_INSEL0_MASK_Val   << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_FEEDBACK (CCL_LUTCTRL_INSEL0_FEEDBACK_Val << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_LINK     (CCL_LUTCTRL_INSEL0_LINK_Val   << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_EVENT    (CCL_LUTCTRL_INSEL0_EVENT_Val  << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_IO       (CCL_LUTCTRL_INSEL0_IO_Val     << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_AC       (CCL_LUTCTRL_INSEL0_AC_Val     << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_TC       (CCL_LUTCTRL_INSEL0_TC_Val     << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_ALTTC    (CCL_LUTCTRL_INSEL0_ALTTC_Val  << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_TCC      (CCL_LUTCTRL_INSEL0_TCC_Val    << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL0_SERCOM   (CCL_LUTCTRL_INSEL0_SERCOM_Val << CCL_LUTCTRL_INSEL0_Pos)
#define CCL_LUTCTRL_INSEL1_Pos      12           /**< \brief (CCL_LUTCTRL) Input Selection 1 */
#define CCL_LUTCTRL_INSEL1_Msk      (_U_(0xF) << CCL_LUTCTRL_INSEL1_Pos)
#define CCL_LUTCTRL_INSEL1(value)   (CCL_LUTCTRL_INSEL1_Msk & ((value) << CCL_LUTCTRL_INSEL1_Pos))
#define   CCL_LUTCTRL_INSEL1_MASK_Val     _U_(0x0)   /**< \brief (CCL_LUTCTRL) Masked input */
#define   CCL_LUTCTRL_INSEL1_FEEDBACK_Val _U_(0x1)   /**< \brief (CCL_LUTCTRL) Feedback input source */
#define   CCL_LUTCTRL_INSEL1_LINK_Val     _U_(0x2)   /**< \brief (CCL_LUTCTRL) Linked LUT input source */
#define   CCL_LUTCTRL_INSEL1_EVENT_Val    _U_(0x3)   /**< \brief (CCL_LUTCTRL) Event input source */
#define   CCL_LUTCTRL_INSEL1_IO_Val       _U_(0x4)   /**< \brief (CCL_LUTCTRL) I/O pin input source */
#define   CCL_LUTCTRL_INSEL1_AC_Val       _U_(0x5)   /**< \brief (CCL_LUTCTRL) AC input source */
#define   CCL_LUTCTRL_INSEL1_TC_Val       _U_(0x6)   /**< \brief (CCL_LUTCTRL) TC input source */
#define   CCL_LUTCTRL_INSEL1_ALTTC_Val    _U_(0x7)   /**< \brief (CCL_LUTCTRL) Alternate TC input source */
#define   CCL_LUTCTRL_INSEL1_TCC_Val      _U_(0x8)   /**< \brief (CCL_LUTCTRL) TCC input source */
#define   CCL_LUTCTRL_INSEL1_SERCOM_Val   _U_(0x9)   /**< \brief (CCL_LUTCTRL) SERCOM input source */
#define CCL_LUTCTRL_INSEL1_MASK     (CCL_LUTCTRL_INSEL1_MASK_Val   << CCL_LUTCTRL_INSEL1_Pos)
#define CCL_LUTCTRL_INSEL1_FEEDBACK (CCL_LUTCTRL_INSEL1_FEEDBACK_Val << CCL_LUTCTRL_INSEL1_Pos)
#define CCL_LUTCTRL_INSEL1_LINK     (CCL_LUTCTRL_INSEL1_LINK_Val   << CCL_LUTCTRL_INSEL1_Pos)
#define CCL_LUTCTRL_INSEL1_EVENT    (CCL_LUTCTRL_INSEL1_EVENT_Val  << CCL_LUTCTRL_INSEL1_Pos)
#define CCL_LUTCTRL_INSEL1_IO       (CCL_LUTCTRL_INSEL1_IO_Val     << CCL_LUTCTRL_INSEL1_Pos)
#define CCL_LUTCTRL_INSEL1_AC       (CCL_LUTCTRL_INSEL1_AC_Val     << CCL_LUTCTRL_INSEL1_Pos)
#define CCL_LUTCTRL_INSEL1_TC       (CCL_LUTCTRL_INSEL1_TC_Val     << CCL_LUTCTRL_INSEL1_Pos)
#define CCL_LUTCTRL_INSEL1_ALTTC    (CCL_LUTCTRL_INSEL1_ALTTC_Val  << CCL_LUTCTRL_INSEL1_Pos)
#define CCL_LUTCTRL_INSEL1_TCC      (CCL_LUTCTRL_INSEL1_TCC_Val    << CCL_LUTCTRL_INSEL1_Pos)
#define CCL_LUTCTRL_INSEL1_SERCOM   (CCL_LUTCTRL_INSEL1_SERCOM_Val << CCL_LUTCTRL_INSEL1_Pos)
#define CCL_LUTCTRL_INSEL2_Pos      16           /**< \brief (CCL_LUTCTRL) Input Selection 2 */
#define CCL_LUTCTRL_INSEL2_Msk      (_U_(0xF) << CCL_LUTCTRL_INSEL2_Pos)
#define CCL_LUTCTRL_INSEL2(value)   (CCL_LUTCTRL_INSEL2_Msk & ((value) << CCL_LUTCTRL_INSEL2_Pos))
#define   CCL_LUTCTRL_INSEL2_MASK_Val     _U_(0x0)   /**< \brief (CCL_LUTCTRL) Masked input */
#define   CCL_LUTCTRL_INSEL2_FEEDBACK_Val _U_(0x1)   /**< \brief (CCL_LUTCTRL) Feedback input source */
#define   CCL_LUTCTRL_INSEL2_LINK_Val     _U_(0x2)   /**< \brief (CCL_LUTCTRL) Linked LUT input source */
#define   CCL_LUTCTRL_INSEL2_EVENT_Val    _U_(0x3)   /**< \brief (CCL_LUTCTRL) Event input source */
#define   CCL_LUTCTRL_INSEL2_IO_Val       _U_(0x4)   /**< \brief (CCL_LUTCTRL) I/O pin input source */
#define   CCL_LUTCTRL_INSEL2_AC_Val       _U_(0x5)   /**< \brief (CCL_LUTCTRL) AC input source */
#define   CCL_LUTCTRL_INSEL2_TC_Val       _U_(0x6)   /**< \brief (CCL_LUTCTRL) TC input source */
#define   CCL_LUTCTRL_INSEL2_ALTTC_Val    _U_(0x7)   /**< \brief (CCL_LUTCTRL) Alternate TC input source */
#define   CCL_LUTCTRL_INSEL2_TCC_Val      _U_(0x8)   /**< \brief (CCL_LUTCTRL) TCC input source */
#define   CCL_LUTCTRL_INSEL2_SERCOM_Val   _U_(0x9)   /**< \brief (CCL_LUTCTRL) SERCOM input source */
#define CCL_LUTCTRL_INSEL2_MASK     (CCL_LUTCTRL_INSEL2_MASK_Val   << CCL_LUTCTRL_INSEL2_Pos)
#define CCL_LUTCTRL_INSEL2_FEEDBACK (CCL_LUTCTRL_INSEL2_FEEDBACK_Val << CCL_LUTCTRL_INSEL2_Pos)
#define CCL_LUTCTRL_INSEL2_LINK     (CCL_LUTCTRL_INSEL2_LINK_Val   << CCL_LUTCTRL_INSEL2_Pos)
#define CCL_LUTCTRL_INSEL2_EVENT    (CCL_LUTCTRL_INSEL2_EVENT_Val  << CCL_LUTCTRL_INSEL2_Pos)
#define CCL_LUTCTRL_INSEL2_IO       (CCL_LUTCTRL_INSEL2_IO_Val     << CCL_LUTCTRL_INSEL2_Pos)
#define CCL_LUTCTRL_INSEL2_AC       (CCL_LUTCTRL_INSEL2_AC_Val     << CCL_LUTCTRL_INSEL2_Pos)
#define CCL_LUTCTRL_INSEL2_TC       (CCL_LUTCTRL_INSEL2_TC_Val     << CCL_LUTCTRL_INSEL2_Pos)
#define CCL_LUTCTRL_INSEL2_ALTTC    (CCL_LUTCTRL_INSEL2_ALTTC_Val  << CCL_LUTCTRL_INSEL2_Pos)
#define CCL_LUTCTRL_INSEL2_TCC      (CCL_LUTCTRL_INSEL2_TCC_Val    << CCL_LUTCTRL_INSEL2_Pos)
#define CCL_LUTCTRL_INSEL2_SERCOM   (CCL_LUTCTRL_INSEL2_SERCOM_Val << CCL_LUTCTRL_INSEL2_Pos)
#define CCL_LUTCTRL_INVEI_Pos       20           /**< \brief (CCL_LUTCTRL) Inverted Event Input Enable */
#define CCL_LUTCTRL_INVEI           (_U_(0x1) << CCL_LUTCTRL_INVEI_Pos)
#define CCL_LUTCTRL_LUTEI_Pos       21           /**< \brief (CCL_LUTCTRL) LUT Event Input Enable */
#define CCL_LUTCTRL_LUTEI           (_U_(0x1) << CCL_LUTCTRL_LUTEI_Pos)
#define CCL_LUTCTRL_LUTEO_Pos       22           /**< \brief (CCL_LUTCTRL) LUT Event Output Enable */
#define CCL_LUTCTRL_LUTEO           (_U_(0x1) << CCL_LUTCTRL_LUTEO_Pos)
#define CCL_LUTCTRL_TRUTH_Pos       24           /**< \brief (CCL_LUTCTRL) Truth Value */
#define CCL_LUTCTRL_TRUTH_Msk       (_U_(0xFF) << CCL_LUTCTRL_TRUTH_Pos)
#define CCL_LUTCTRL_TRUTH(value)    (CCL_LUTCTRL_TRUTH_Msk & ((value) << CCL_LUTCTRL_TRUTH_Pos))
#define CCL_LUTCTRL_MASK            _U_(0xFF7FFFB2) /**< \brief (CCL_LUTCTRL) MASK Register */

/** \brief CCL hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO CCL_CTRL_Type             CTRL;        /**< \brief Offset: 0x0 (R/W  8) Control */
       RoReg8                    Reserved1[0x3];
  __IO CCL_SEQCTRL_Type          SEQCTRL[2];  /**< \brief Offset: 0x4 (R/W  8) SEQ Control x */
       RoReg8                    Reserved2[0x2];
  __IO CCL_LUTCTRL_Type          LUTCTRL[4];  /**< \brief Offset: 0x8 (R/W 32) LUT Control x */
} Ccl;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_CCL_COMPONENT_ */
