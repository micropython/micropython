/**
 * \file
 *
 * \brief Component description for EIC
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

#ifndef _SAMD51_EIC_COMPONENT_
#define _SAMD51_EIC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR EIC */
/* ========================================================================== */
/** \addtogroup SAMD51_EIC External Interrupt Controller */
/*@{*/

#define EIC_U2254
#define REV_EIC                     0x300

/* -------- EIC_CTRLA : (EIC Offset: 0x00) (R/W  8) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  :2;               /*!< bit:  2.. 3  Reserved                           */
    uint8_t  CKSEL:1;          /*!< bit:      4  Clock Selection                    */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EIC_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_CTRLA_OFFSET            0x00         /**< \brief (EIC_CTRLA offset) Control A */
#define EIC_CTRLA_RESETVALUE        _U_(0x00)    /**< \brief (EIC_CTRLA reset_value) Control A */

#define EIC_CTRLA_SWRST_Pos         0            /**< \brief (EIC_CTRLA) Software Reset */
#define EIC_CTRLA_SWRST             (_U_(0x1) << EIC_CTRLA_SWRST_Pos)
#define EIC_CTRLA_ENABLE_Pos        1            /**< \brief (EIC_CTRLA) Enable */
#define EIC_CTRLA_ENABLE            (_U_(0x1) << EIC_CTRLA_ENABLE_Pos)
#define EIC_CTRLA_CKSEL_Pos         4            /**< \brief (EIC_CTRLA) Clock Selection */
#define EIC_CTRLA_CKSEL             (_U_(0x1) << EIC_CTRLA_CKSEL_Pos)
#define EIC_CTRLA_MASK              _U_(0x13)    /**< \brief (EIC_CTRLA) MASK Register */

/* -------- EIC_NMICTRL : (EIC Offset: 0x01) (R/W  8) Non-Maskable Interrupt Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  NMISENSE:3;       /*!< bit:  0.. 2  Non-Maskable Interrupt Sense Configuration */
    uint8_t  NMIFILTEN:1;      /*!< bit:      3  Non-Maskable Interrupt Filter Enable */
    uint8_t  NMIASYNCH:1;      /*!< bit:      4  Asynchronous Edge Detection Mode   */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EIC_NMICTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_NMICTRL_OFFSET          0x01         /**< \brief (EIC_NMICTRL offset) Non-Maskable Interrupt Control */
#define EIC_NMICTRL_RESETVALUE      _U_(0x00)    /**< \brief (EIC_NMICTRL reset_value) Non-Maskable Interrupt Control */

#define EIC_NMICTRL_NMISENSE_Pos    0            /**< \brief (EIC_NMICTRL) Non-Maskable Interrupt Sense Configuration */
#define EIC_NMICTRL_NMISENSE_Msk    (_U_(0x7) << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE(value) (EIC_NMICTRL_NMISENSE_Msk & ((value) << EIC_NMICTRL_NMISENSE_Pos))
#define   EIC_NMICTRL_NMISENSE_NONE_Val   _U_(0x0)   /**< \brief (EIC_NMICTRL) No detection */
#define   EIC_NMICTRL_NMISENSE_RISE_Val   _U_(0x1)   /**< \brief (EIC_NMICTRL) Rising-edge detection */
#define   EIC_NMICTRL_NMISENSE_FALL_Val   _U_(0x2)   /**< \brief (EIC_NMICTRL) Falling-edge detection */
#define   EIC_NMICTRL_NMISENSE_BOTH_Val   _U_(0x3)   /**< \brief (EIC_NMICTRL) Both-edges detection */
#define   EIC_NMICTRL_NMISENSE_HIGH_Val   _U_(0x4)   /**< \brief (EIC_NMICTRL) High-level detection */
#define   EIC_NMICTRL_NMISENSE_LOW_Val    _U_(0x5)   /**< \brief (EIC_NMICTRL) Low-level detection */
#define EIC_NMICTRL_NMISENSE_NONE   (EIC_NMICTRL_NMISENSE_NONE_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_RISE   (EIC_NMICTRL_NMISENSE_RISE_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_FALL   (EIC_NMICTRL_NMISENSE_FALL_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_BOTH   (EIC_NMICTRL_NMISENSE_BOTH_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_HIGH   (EIC_NMICTRL_NMISENSE_HIGH_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_LOW    (EIC_NMICTRL_NMISENSE_LOW_Val  << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMIFILTEN_Pos   3            /**< \brief (EIC_NMICTRL) Non-Maskable Interrupt Filter Enable */
#define EIC_NMICTRL_NMIFILTEN       (_U_(0x1) << EIC_NMICTRL_NMIFILTEN_Pos)
#define EIC_NMICTRL_NMIASYNCH_Pos   4            /**< \brief (EIC_NMICTRL) Asynchronous Edge Detection Mode */
#define EIC_NMICTRL_NMIASYNCH       (_U_(0x1) << EIC_NMICTRL_NMIASYNCH_Pos)
#define EIC_NMICTRL_MASK            _U_(0x1F)    /**< \brief (EIC_NMICTRL) MASK Register */

/* -------- EIC_NMIFLAG : (EIC Offset: 0x02) (R/W 16) Non-Maskable Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t NMI:1;            /*!< bit:      0  Non-Maskable Interrupt             */
    uint16_t :15;              /*!< bit:  1..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} EIC_NMIFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_NMIFLAG_OFFSET          0x02         /**< \brief (EIC_NMIFLAG offset) Non-Maskable Interrupt Flag Status and Clear */
#define EIC_NMIFLAG_RESETVALUE      _U_(0x0000)  /**< \brief (EIC_NMIFLAG reset_value) Non-Maskable Interrupt Flag Status and Clear */

#define EIC_NMIFLAG_NMI_Pos         0            /**< \brief (EIC_NMIFLAG) Non-Maskable Interrupt */
#define EIC_NMIFLAG_NMI             (_U_(0x1) << EIC_NMIFLAG_NMI_Pos)
#define EIC_NMIFLAG_MASK            _U_(0x0001)  /**< \brief (EIC_NMIFLAG) MASK Register */

/* -------- EIC_SYNCBUSY : (EIC Offset: 0x04) (R/  32) Synchronization Busy -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset Synchronization Busy Status */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable Synchronization Busy Status */
    uint32_t :30;              /*!< bit:  2..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_SYNCBUSY_OFFSET         0x04         /**< \brief (EIC_SYNCBUSY offset) Synchronization Busy */
#define EIC_SYNCBUSY_RESETVALUE     _U_(0x00000000) /**< \brief (EIC_SYNCBUSY reset_value) Synchronization Busy */

#define EIC_SYNCBUSY_SWRST_Pos      0            /**< \brief (EIC_SYNCBUSY) Software Reset Synchronization Busy Status */
#define EIC_SYNCBUSY_SWRST          (_U_(0x1) << EIC_SYNCBUSY_SWRST_Pos)
#define EIC_SYNCBUSY_ENABLE_Pos     1            /**< \brief (EIC_SYNCBUSY) Enable Synchronization Busy Status */
#define EIC_SYNCBUSY_ENABLE         (_U_(0x1) << EIC_SYNCBUSY_ENABLE_Pos)
#define EIC_SYNCBUSY_MASK           _U_(0x00000003) /**< \brief (EIC_SYNCBUSY) MASK Register */

/* -------- EIC_EVCTRL : (EIC Offset: 0x08) (R/W 32) Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EXTINTEO:16;      /*!< bit:  0..15  External Interrupt Event Output Enable */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_EVCTRL_OFFSET           0x08         /**< \brief (EIC_EVCTRL offset) Event Control */
#define EIC_EVCTRL_RESETVALUE       _U_(0x00000000) /**< \brief (EIC_EVCTRL reset_value) Event Control */

#define EIC_EVCTRL_EXTINTEO_Pos     0            /**< \brief (EIC_EVCTRL) External Interrupt Event Output Enable */
#define EIC_EVCTRL_EXTINTEO_Msk     (_U_(0xFFFF) << EIC_EVCTRL_EXTINTEO_Pos)
#define EIC_EVCTRL_EXTINTEO(value)  (EIC_EVCTRL_EXTINTEO_Msk & ((value) << EIC_EVCTRL_EXTINTEO_Pos))
#define EIC_EVCTRL_MASK             _U_(0x0000FFFF) /**< \brief (EIC_EVCTRL) MASK Register */

/* -------- EIC_INTENCLR : (EIC Offset: 0x0C) (R/W 32) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EXTINT:16;        /*!< bit:  0..15  External Interrupt Enable          */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_INTENCLR_OFFSET         0x0C         /**< \brief (EIC_INTENCLR offset) Interrupt Enable Clear */
#define EIC_INTENCLR_RESETVALUE     _U_(0x00000000) /**< \brief (EIC_INTENCLR reset_value) Interrupt Enable Clear */

#define EIC_INTENCLR_EXTINT_Pos     0            /**< \brief (EIC_INTENCLR) External Interrupt Enable */
#define EIC_INTENCLR_EXTINT_Msk     (_U_(0xFFFF) << EIC_INTENCLR_EXTINT_Pos)
#define EIC_INTENCLR_EXTINT(value)  (EIC_INTENCLR_EXTINT_Msk & ((value) << EIC_INTENCLR_EXTINT_Pos))
#define EIC_INTENCLR_MASK           _U_(0x0000FFFF) /**< \brief (EIC_INTENCLR) MASK Register */

/* -------- EIC_INTENSET : (EIC Offset: 0x10) (R/W 32) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EXTINT:16;        /*!< bit:  0..15  External Interrupt Enable          */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_INTENSET_OFFSET         0x10         /**< \brief (EIC_INTENSET offset) Interrupt Enable Set */
#define EIC_INTENSET_RESETVALUE     _U_(0x00000000) /**< \brief (EIC_INTENSET reset_value) Interrupt Enable Set */

#define EIC_INTENSET_EXTINT_Pos     0            /**< \brief (EIC_INTENSET) External Interrupt Enable */
#define EIC_INTENSET_EXTINT_Msk     (_U_(0xFFFF) << EIC_INTENSET_EXTINT_Pos)
#define EIC_INTENSET_EXTINT(value)  (EIC_INTENSET_EXTINT_Msk & ((value) << EIC_INTENSET_EXTINT_Pos))
#define EIC_INTENSET_MASK           _U_(0x0000FFFF) /**< \brief (EIC_INTENSET) MASK Register */

/* -------- EIC_INTFLAG : (EIC Offset: 0x14) (R/W 32) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t EXTINT:16;        /*!< bit:  0..15  External Interrupt                 */
    __I uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_INTFLAG_OFFSET          0x14         /**< \brief (EIC_INTFLAG offset) Interrupt Flag Status and Clear */
#define EIC_INTFLAG_RESETVALUE      _U_(0x00000000) /**< \brief (EIC_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define EIC_INTFLAG_EXTINT_Pos      0            /**< \brief (EIC_INTFLAG) External Interrupt */
#define EIC_INTFLAG_EXTINT_Msk      (_U_(0xFFFF) << EIC_INTFLAG_EXTINT_Pos)
#define EIC_INTFLAG_EXTINT(value)   (EIC_INTFLAG_EXTINT_Msk & ((value) << EIC_INTFLAG_EXTINT_Pos))
#define EIC_INTFLAG_MASK            _U_(0x0000FFFF) /**< \brief (EIC_INTFLAG) MASK Register */

/* -------- EIC_ASYNCH : (EIC Offset: 0x18) (R/W 32) External Interrupt Asynchronous Mode -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ASYNCH:16;        /*!< bit:  0..15  Asynchronous Edge Detection Mode   */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_ASYNCH_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_ASYNCH_OFFSET           0x18         /**< \brief (EIC_ASYNCH offset) External Interrupt Asynchronous Mode */
#define EIC_ASYNCH_RESETVALUE       _U_(0x00000000) /**< \brief (EIC_ASYNCH reset_value) External Interrupt Asynchronous Mode */

#define EIC_ASYNCH_ASYNCH_Pos       0            /**< \brief (EIC_ASYNCH) Asynchronous Edge Detection Mode */
#define EIC_ASYNCH_ASYNCH_Msk       (_U_(0xFFFF) << EIC_ASYNCH_ASYNCH_Pos)
#define EIC_ASYNCH_ASYNCH(value)    (EIC_ASYNCH_ASYNCH_Msk & ((value) << EIC_ASYNCH_ASYNCH_Pos))
#define EIC_ASYNCH_MASK             _U_(0x0000FFFF) /**< \brief (EIC_ASYNCH) MASK Register */

/* -------- EIC_CONFIG : (EIC Offset: 0x1C) (R/W 32) External Interrupt Sense Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SENSE0:3;         /*!< bit:  0.. 2  Input Sense Configuration 0        */
    uint32_t FILTEN0:1;        /*!< bit:      3  Filter Enable 0                    */
    uint32_t SENSE1:3;         /*!< bit:  4.. 6  Input Sense Configuration 1        */
    uint32_t FILTEN1:1;        /*!< bit:      7  Filter Enable 1                    */
    uint32_t SENSE2:3;         /*!< bit:  8..10  Input Sense Configuration 2        */
    uint32_t FILTEN2:1;        /*!< bit:     11  Filter Enable 2                    */
    uint32_t SENSE3:3;         /*!< bit: 12..14  Input Sense Configuration 3        */
    uint32_t FILTEN3:1;        /*!< bit:     15  Filter Enable 3                    */
    uint32_t SENSE4:3;         /*!< bit: 16..18  Input Sense Configuration 4        */
    uint32_t FILTEN4:1;        /*!< bit:     19  Filter Enable 4                    */
    uint32_t SENSE5:3;         /*!< bit: 20..22  Input Sense Configuration 5        */
    uint32_t FILTEN5:1;        /*!< bit:     23  Filter Enable 5                    */
    uint32_t SENSE6:3;         /*!< bit: 24..26  Input Sense Configuration 6        */
    uint32_t FILTEN6:1;        /*!< bit:     27  Filter Enable 6                    */
    uint32_t SENSE7:3;         /*!< bit: 28..30  Input Sense Configuration 7        */
    uint32_t FILTEN7:1;        /*!< bit:     31  Filter Enable 7                    */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_CONFIG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_CONFIG_OFFSET           0x1C         /**< \brief (EIC_CONFIG offset) External Interrupt Sense Configuration */
#define EIC_CONFIG_RESETVALUE       _U_(0x00000000) /**< \brief (EIC_CONFIG reset_value) External Interrupt Sense Configuration */

#define EIC_CONFIG_SENSE0_Pos       0            /**< \brief (EIC_CONFIG) Input Sense Configuration 0 */
#define EIC_CONFIG_SENSE0_Msk       (_U_(0x7) << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0(value)    (EIC_CONFIG_SENSE0_Msk & ((value) << EIC_CONFIG_SENSE0_Pos))
#define   EIC_CONFIG_SENSE0_NONE_Val      _U_(0x0)   /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE0_RISE_Val      _U_(0x1)   /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE0_FALL_Val      _U_(0x2)   /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE0_BOTH_Val      _U_(0x3)   /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE0_HIGH_Val      _U_(0x4)   /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE0_LOW_Val       _U_(0x5)   /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE0_NONE      (EIC_CONFIG_SENSE0_NONE_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_RISE      (EIC_CONFIG_SENSE0_RISE_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_FALL      (EIC_CONFIG_SENSE0_FALL_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_BOTH      (EIC_CONFIG_SENSE0_BOTH_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_HIGH      (EIC_CONFIG_SENSE0_HIGH_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_LOW       (EIC_CONFIG_SENSE0_LOW_Val     << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_FILTEN0_Pos      3            /**< \brief (EIC_CONFIG) Filter Enable 0 */
#define EIC_CONFIG_FILTEN0          (_U_(0x1) << EIC_CONFIG_FILTEN0_Pos)
#define EIC_CONFIG_SENSE1_Pos       4            /**< \brief (EIC_CONFIG) Input Sense Configuration 1 */
#define EIC_CONFIG_SENSE1_Msk       (_U_(0x7) << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1(value)    (EIC_CONFIG_SENSE1_Msk & ((value) << EIC_CONFIG_SENSE1_Pos))
#define   EIC_CONFIG_SENSE1_NONE_Val      _U_(0x0)   /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE1_RISE_Val      _U_(0x1)   /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE1_FALL_Val      _U_(0x2)   /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE1_BOTH_Val      _U_(0x3)   /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE1_HIGH_Val      _U_(0x4)   /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE1_LOW_Val       _U_(0x5)   /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE1_NONE      (EIC_CONFIG_SENSE1_NONE_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_RISE      (EIC_CONFIG_SENSE1_RISE_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_FALL      (EIC_CONFIG_SENSE1_FALL_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_BOTH      (EIC_CONFIG_SENSE1_BOTH_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_HIGH      (EIC_CONFIG_SENSE1_HIGH_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_LOW       (EIC_CONFIG_SENSE1_LOW_Val     << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_FILTEN1_Pos      7            /**< \brief (EIC_CONFIG) Filter Enable 1 */
#define EIC_CONFIG_FILTEN1          (_U_(0x1) << EIC_CONFIG_FILTEN1_Pos)
#define EIC_CONFIG_SENSE2_Pos       8            /**< \brief (EIC_CONFIG) Input Sense Configuration 2 */
#define EIC_CONFIG_SENSE2_Msk       (_U_(0x7) << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2(value)    (EIC_CONFIG_SENSE2_Msk & ((value) << EIC_CONFIG_SENSE2_Pos))
#define   EIC_CONFIG_SENSE2_NONE_Val      _U_(0x0)   /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE2_RISE_Val      _U_(0x1)   /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE2_FALL_Val      _U_(0x2)   /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE2_BOTH_Val      _U_(0x3)   /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE2_HIGH_Val      _U_(0x4)   /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE2_LOW_Val       _U_(0x5)   /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE2_NONE      (EIC_CONFIG_SENSE2_NONE_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_RISE      (EIC_CONFIG_SENSE2_RISE_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_FALL      (EIC_CONFIG_SENSE2_FALL_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_BOTH      (EIC_CONFIG_SENSE2_BOTH_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_HIGH      (EIC_CONFIG_SENSE2_HIGH_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_LOW       (EIC_CONFIG_SENSE2_LOW_Val     << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_FILTEN2_Pos      11           /**< \brief (EIC_CONFIG) Filter Enable 2 */
#define EIC_CONFIG_FILTEN2          (_U_(0x1) << EIC_CONFIG_FILTEN2_Pos)
#define EIC_CONFIG_SENSE3_Pos       12           /**< \brief (EIC_CONFIG) Input Sense Configuration 3 */
#define EIC_CONFIG_SENSE3_Msk       (_U_(0x7) << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3(value)    (EIC_CONFIG_SENSE3_Msk & ((value) << EIC_CONFIG_SENSE3_Pos))
#define   EIC_CONFIG_SENSE3_NONE_Val      _U_(0x0)   /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE3_RISE_Val      _U_(0x1)   /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE3_FALL_Val      _U_(0x2)   /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE3_BOTH_Val      _U_(0x3)   /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE3_HIGH_Val      _U_(0x4)   /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE3_LOW_Val       _U_(0x5)   /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE3_NONE      (EIC_CONFIG_SENSE3_NONE_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_RISE      (EIC_CONFIG_SENSE3_RISE_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_FALL      (EIC_CONFIG_SENSE3_FALL_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_BOTH      (EIC_CONFIG_SENSE3_BOTH_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_HIGH      (EIC_CONFIG_SENSE3_HIGH_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_LOW       (EIC_CONFIG_SENSE3_LOW_Val     << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_FILTEN3_Pos      15           /**< \brief (EIC_CONFIG) Filter Enable 3 */
#define EIC_CONFIG_FILTEN3          (_U_(0x1) << EIC_CONFIG_FILTEN3_Pos)
#define EIC_CONFIG_SENSE4_Pos       16           /**< \brief (EIC_CONFIG) Input Sense Configuration 4 */
#define EIC_CONFIG_SENSE4_Msk       (_U_(0x7) << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4(value)    (EIC_CONFIG_SENSE4_Msk & ((value) << EIC_CONFIG_SENSE4_Pos))
#define   EIC_CONFIG_SENSE4_NONE_Val      _U_(0x0)   /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE4_RISE_Val      _U_(0x1)   /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE4_FALL_Val      _U_(0x2)   /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE4_BOTH_Val      _U_(0x3)   /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE4_HIGH_Val      _U_(0x4)   /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE4_LOW_Val       _U_(0x5)   /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE4_NONE      (EIC_CONFIG_SENSE4_NONE_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_RISE      (EIC_CONFIG_SENSE4_RISE_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_FALL      (EIC_CONFIG_SENSE4_FALL_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_BOTH      (EIC_CONFIG_SENSE4_BOTH_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_HIGH      (EIC_CONFIG_SENSE4_HIGH_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_LOW       (EIC_CONFIG_SENSE4_LOW_Val     << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_FILTEN4_Pos      19           /**< \brief (EIC_CONFIG) Filter Enable 4 */
#define EIC_CONFIG_FILTEN4          (_U_(0x1) << EIC_CONFIG_FILTEN4_Pos)
#define EIC_CONFIG_SENSE5_Pos       20           /**< \brief (EIC_CONFIG) Input Sense Configuration 5 */
#define EIC_CONFIG_SENSE5_Msk       (_U_(0x7) << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5(value)    (EIC_CONFIG_SENSE5_Msk & ((value) << EIC_CONFIG_SENSE5_Pos))
#define   EIC_CONFIG_SENSE5_NONE_Val      _U_(0x0)   /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE5_RISE_Val      _U_(0x1)   /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE5_FALL_Val      _U_(0x2)   /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE5_BOTH_Val      _U_(0x3)   /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE5_HIGH_Val      _U_(0x4)   /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE5_LOW_Val       _U_(0x5)   /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE5_NONE      (EIC_CONFIG_SENSE5_NONE_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_RISE      (EIC_CONFIG_SENSE5_RISE_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_FALL      (EIC_CONFIG_SENSE5_FALL_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_BOTH      (EIC_CONFIG_SENSE5_BOTH_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_HIGH      (EIC_CONFIG_SENSE5_HIGH_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_LOW       (EIC_CONFIG_SENSE5_LOW_Val     << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_FILTEN5_Pos      23           /**< \brief (EIC_CONFIG) Filter Enable 5 */
#define EIC_CONFIG_FILTEN5          (_U_(0x1) << EIC_CONFIG_FILTEN5_Pos)
#define EIC_CONFIG_SENSE6_Pos       24           /**< \brief (EIC_CONFIG) Input Sense Configuration 6 */
#define EIC_CONFIG_SENSE6_Msk       (_U_(0x7) << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6(value)    (EIC_CONFIG_SENSE6_Msk & ((value) << EIC_CONFIG_SENSE6_Pos))
#define   EIC_CONFIG_SENSE6_NONE_Val      _U_(0x0)   /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE6_RISE_Val      _U_(0x1)   /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE6_FALL_Val      _U_(0x2)   /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE6_BOTH_Val      _U_(0x3)   /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE6_HIGH_Val      _U_(0x4)   /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE6_LOW_Val       _U_(0x5)   /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE6_NONE      (EIC_CONFIG_SENSE6_NONE_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_RISE      (EIC_CONFIG_SENSE6_RISE_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_FALL      (EIC_CONFIG_SENSE6_FALL_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_BOTH      (EIC_CONFIG_SENSE6_BOTH_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_HIGH      (EIC_CONFIG_SENSE6_HIGH_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_LOW       (EIC_CONFIG_SENSE6_LOW_Val     << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_FILTEN6_Pos      27           /**< \brief (EIC_CONFIG) Filter Enable 6 */
#define EIC_CONFIG_FILTEN6          (_U_(0x1) << EIC_CONFIG_FILTEN6_Pos)
#define EIC_CONFIG_SENSE7_Pos       28           /**< \brief (EIC_CONFIG) Input Sense Configuration 7 */
#define EIC_CONFIG_SENSE7_Msk       (_U_(0x7) << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7(value)    (EIC_CONFIG_SENSE7_Msk & ((value) << EIC_CONFIG_SENSE7_Pos))
#define   EIC_CONFIG_SENSE7_NONE_Val      _U_(0x0)   /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE7_RISE_Val      _U_(0x1)   /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE7_FALL_Val      _U_(0x2)   /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE7_BOTH_Val      _U_(0x3)   /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE7_HIGH_Val      _U_(0x4)   /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE7_LOW_Val       _U_(0x5)   /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE7_NONE      (EIC_CONFIG_SENSE7_NONE_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_RISE      (EIC_CONFIG_SENSE7_RISE_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_FALL      (EIC_CONFIG_SENSE7_FALL_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_BOTH      (EIC_CONFIG_SENSE7_BOTH_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_HIGH      (EIC_CONFIG_SENSE7_HIGH_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_LOW       (EIC_CONFIG_SENSE7_LOW_Val     << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_FILTEN7_Pos      31           /**< \brief (EIC_CONFIG) Filter Enable 7 */
#define EIC_CONFIG_FILTEN7          (_U_(0x1) << EIC_CONFIG_FILTEN7_Pos)
#define EIC_CONFIG_MASK             _U_(0xFFFFFFFF) /**< \brief (EIC_CONFIG) MASK Register */

/* -------- EIC_DEBOUNCEN : (EIC Offset: 0x30) (R/W 32) Debouncer Enable -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DEBOUNCEN:16;     /*!< bit:  0..15  Debouncer Enable                   */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_DEBOUNCEN_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_DEBOUNCEN_OFFSET        0x30         /**< \brief (EIC_DEBOUNCEN offset) Debouncer Enable */
#define EIC_DEBOUNCEN_RESETVALUE    _U_(0x00000000) /**< \brief (EIC_DEBOUNCEN reset_value) Debouncer Enable */

#define EIC_DEBOUNCEN_DEBOUNCEN_Pos 0            /**< \brief (EIC_DEBOUNCEN) Debouncer Enable */
#define EIC_DEBOUNCEN_DEBOUNCEN_Msk (_U_(0xFFFF) << EIC_DEBOUNCEN_DEBOUNCEN_Pos)
#define EIC_DEBOUNCEN_DEBOUNCEN(value) (EIC_DEBOUNCEN_DEBOUNCEN_Msk & ((value) << EIC_DEBOUNCEN_DEBOUNCEN_Pos))
#define EIC_DEBOUNCEN_MASK          _U_(0x0000FFFF) /**< \brief (EIC_DEBOUNCEN) MASK Register */

/* -------- EIC_DPRESCALER : (EIC Offset: 0x34) (R/W 32) Debouncer Prescaler -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PRESCALER0:3;     /*!< bit:  0.. 2  Debouncer Prescaler                */
    uint32_t STATES0:1;        /*!< bit:      3  Debouncer number of states         */
    uint32_t PRESCALER1:3;     /*!< bit:  4.. 6  Debouncer Prescaler                */
    uint32_t STATES1:1;        /*!< bit:      7  Debouncer number of states         */
    uint32_t :8;               /*!< bit:  8..15  Reserved                           */
    uint32_t TICKON:1;         /*!< bit:     16  Pin Sampler frequency selection    */
    uint32_t :15;              /*!< bit: 17..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_DPRESCALER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_DPRESCALER_OFFSET       0x34         /**< \brief (EIC_DPRESCALER offset) Debouncer Prescaler */
#define EIC_DPRESCALER_RESETVALUE   _U_(0x00000000) /**< \brief (EIC_DPRESCALER reset_value) Debouncer Prescaler */

#define EIC_DPRESCALER_PRESCALER0_Pos 0            /**< \brief (EIC_DPRESCALER) Debouncer Prescaler */
#define EIC_DPRESCALER_PRESCALER0_Msk (_U_(0x7) << EIC_DPRESCALER_PRESCALER0_Pos)
#define EIC_DPRESCALER_PRESCALER0(value) (EIC_DPRESCALER_PRESCALER0_Msk & ((value) << EIC_DPRESCALER_PRESCALER0_Pos))
#define EIC_DPRESCALER_STATES0_Pos  3            /**< \brief (EIC_DPRESCALER) Debouncer number of states */
#define EIC_DPRESCALER_STATES0      (_U_(0x1) << EIC_DPRESCALER_STATES0_Pos)
#define EIC_DPRESCALER_PRESCALER1_Pos 4            /**< \brief (EIC_DPRESCALER) Debouncer Prescaler */
#define EIC_DPRESCALER_PRESCALER1_Msk (_U_(0x7) << EIC_DPRESCALER_PRESCALER1_Pos)
#define EIC_DPRESCALER_PRESCALER1(value) (EIC_DPRESCALER_PRESCALER1_Msk & ((value) << EIC_DPRESCALER_PRESCALER1_Pos))
#define EIC_DPRESCALER_STATES1_Pos  7            /**< \brief (EIC_DPRESCALER) Debouncer number of states */
#define EIC_DPRESCALER_STATES1      (_U_(0x1) << EIC_DPRESCALER_STATES1_Pos)
#define EIC_DPRESCALER_TICKON_Pos   16           /**< \brief (EIC_DPRESCALER) Pin Sampler frequency selection */
#define EIC_DPRESCALER_TICKON       (_U_(0x1) << EIC_DPRESCALER_TICKON_Pos)
#define EIC_DPRESCALER_MASK         _U_(0x000100FF) /**< \brief (EIC_DPRESCALER) MASK Register */

/* -------- EIC_PINSTATE : (EIC Offset: 0x38) (R/  32) Pin State -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PINSTATE:16;      /*!< bit:  0..15  Pin State                          */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_PINSTATE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_PINSTATE_OFFSET         0x38         /**< \brief (EIC_PINSTATE offset) Pin State */
#define EIC_PINSTATE_RESETVALUE     _U_(0x00000000) /**< \brief (EIC_PINSTATE reset_value) Pin State */

#define EIC_PINSTATE_PINSTATE_Pos   0            /**< \brief (EIC_PINSTATE) Pin State */
#define EIC_PINSTATE_PINSTATE_Msk   (_U_(0xFFFF) << EIC_PINSTATE_PINSTATE_Pos)
#define EIC_PINSTATE_PINSTATE(value) (EIC_PINSTATE_PINSTATE_Msk & ((value) << EIC_PINSTATE_PINSTATE_Pos))
#define EIC_PINSTATE_MASK           _U_(0x0000FFFF) /**< \brief (EIC_PINSTATE) MASK Register */

/** \brief EIC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO EIC_CTRLA_Type            CTRLA;       /**< \brief Offset: 0x00 (R/W  8) Control A */
  __IO EIC_NMICTRL_Type          NMICTRL;     /**< \brief Offset: 0x01 (R/W  8) Non-Maskable Interrupt Control */
  __IO EIC_NMIFLAG_Type          NMIFLAG;     /**< \brief Offset: 0x02 (R/W 16) Non-Maskable Interrupt Flag Status and Clear */
  __I  EIC_SYNCBUSY_Type         SYNCBUSY;    /**< \brief Offset: 0x04 (R/  32) Synchronization Busy */
  __IO EIC_EVCTRL_Type           EVCTRL;      /**< \brief Offset: 0x08 (R/W 32) Event Control */
  __IO EIC_INTENCLR_Type         INTENCLR;    /**< \brief Offset: 0x0C (R/W 32) Interrupt Enable Clear */
  __IO EIC_INTENSET_Type         INTENSET;    /**< \brief Offset: 0x10 (R/W 32) Interrupt Enable Set */
  __IO EIC_INTFLAG_Type          INTFLAG;     /**< \brief Offset: 0x14 (R/W 32) Interrupt Flag Status and Clear */
  __IO EIC_ASYNCH_Type           ASYNCH;      /**< \brief Offset: 0x18 (R/W 32) External Interrupt Asynchronous Mode */
  __IO EIC_CONFIG_Type           CONFIG[2];   /**< \brief Offset: 0x1C (R/W 32) External Interrupt Sense Configuration */
       RoReg8                    Reserved1[0xC];
  __IO EIC_DEBOUNCEN_Type        DEBOUNCEN;   /**< \brief Offset: 0x30 (R/W 32) Debouncer Enable */
  __IO EIC_DPRESCALER_Type       DPRESCALER;  /**< \brief Offset: 0x34 (R/W 32) Debouncer Prescaler */
  __I  EIC_PINSTATE_Type         PINSTATE;    /**< \brief Offset: 0x38 (R/  32) Pin State */
} Eic;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_EIC_COMPONENT_ */
