/**
 * \file
 *
 * \brief Component description for I2S
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

#ifndef _SAMD51_I2S_COMPONENT_
#define _SAMD51_I2S_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR I2S */
/* ========================================================================== */
/** \addtogroup SAMD51_I2S Inter-IC Sound Interface */
/*@{*/

#define I2S_U2224
#define REV_I2S                     0x200

/* -------- I2S_CTRLA : (I2S Offset: 0x00) (R/W  8) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  CKEN0:1;          /*!< bit:      2  Clock Unit 0 Enable                */
    uint8_t  CKEN1:1;          /*!< bit:      3  Clock Unit 1 Enable                */
    uint8_t  TXEN:1;           /*!< bit:      4  Tx Serializer Enable               */
    uint8_t  RXEN:1;           /*!< bit:      5  Rx Serializer Enable               */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  :2;               /*!< bit:  0.. 1  Reserved                           */
    uint8_t  CKEN:2;           /*!< bit:  2.. 3  Clock Unit x Enable                */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} I2S_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define I2S_CTRLA_OFFSET            0x00         /**< \brief (I2S_CTRLA offset) Control A */
#define I2S_CTRLA_RESETVALUE        _U_(0x00)    /**< \brief (I2S_CTRLA reset_value) Control A */

#define I2S_CTRLA_SWRST_Pos         0            /**< \brief (I2S_CTRLA) Software Reset */
#define I2S_CTRLA_SWRST             (_U_(0x1) << I2S_CTRLA_SWRST_Pos)
#define I2S_CTRLA_ENABLE_Pos        1            /**< \brief (I2S_CTRLA) Enable */
#define I2S_CTRLA_ENABLE            (_U_(0x1) << I2S_CTRLA_ENABLE_Pos)
#define I2S_CTRLA_CKEN0_Pos         2            /**< \brief (I2S_CTRLA) Clock Unit 0 Enable */
#define I2S_CTRLA_CKEN0             (_U_(1) << I2S_CTRLA_CKEN0_Pos)
#define I2S_CTRLA_CKEN1_Pos         3            /**< \brief (I2S_CTRLA) Clock Unit 1 Enable */
#define I2S_CTRLA_CKEN1             (_U_(1) << I2S_CTRLA_CKEN1_Pos)
#define I2S_CTRLA_CKEN_Pos          2            /**< \brief (I2S_CTRLA) Clock Unit x Enable */
#define I2S_CTRLA_CKEN_Msk          (_U_(0x3) << I2S_CTRLA_CKEN_Pos)
#define I2S_CTRLA_CKEN(value)       (I2S_CTRLA_CKEN_Msk & ((value) << I2S_CTRLA_CKEN_Pos))
#define I2S_CTRLA_TXEN_Pos          4            /**< \brief (I2S_CTRLA) Tx Serializer Enable */
#define I2S_CTRLA_TXEN              (_U_(0x1) << I2S_CTRLA_TXEN_Pos)
#define I2S_CTRLA_RXEN_Pos          5            /**< \brief (I2S_CTRLA) Rx Serializer Enable */
#define I2S_CTRLA_RXEN              (_U_(0x1) << I2S_CTRLA_RXEN_Pos)
#define I2S_CTRLA_MASK              _U_(0x3F)    /**< \brief (I2S_CTRLA) MASK Register */

/* -------- I2S_CLKCTRL : (I2S Offset: 0x04) (R/W 32) Clock Unit n Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SLOTSIZE:2;       /*!< bit:  0.. 1  Slot Size                          */
    uint32_t NBSLOTS:3;        /*!< bit:  2.. 4  Number of Slots in Frame           */
    uint32_t FSWIDTH:2;        /*!< bit:  5.. 6  Frame Sync Width                   */
    uint32_t BITDELAY:1;       /*!< bit:      7  Data Delay from Frame Sync         */
    uint32_t FSSEL:1;          /*!< bit:      8  Frame Sync Select                  */
    uint32_t FSINV:1;          /*!< bit:      9  Frame Sync Invert                  */
    uint32_t FSOUTINV:1;       /*!< bit:     10  Frame Sync Output Invert           */
    uint32_t SCKSEL:1;         /*!< bit:     11  Serial Clock Select                */
    uint32_t SCKOUTINV:1;      /*!< bit:     12  Serial Clock Output Invert         */
    uint32_t MCKSEL:1;         /*!< bit:     13  Master Clock Select                */
    uint32_t MCKEN:1;          /*!< bit:     14  Master Clock Enable                */
    uint32_t MCKOUTINV:1;      /*!< bit:     15  Master Clock Output Invert         */
    uint32_t MCKDIV:6;         /*!< bit: 16..21  Master Clock Division Factor       */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t MCKOUTDIV:6;      /*!< bit: 24..29  Master Clock Output Division Factor */
    uint32_t :2;               /*!< bit: 30..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} I2S_CLKCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define I2S_CLKCTRL_OFFSET          0x04         /**< \brief (I2S_CLKCTRL offset) Clock Unit n Control */
#define I2S_CLKCTRL_RESETVALUE      _U_(0x00000000) /**< \brief (I2S_CLKCTRL reset_value) Clock Unit n Control */

#define I2S_CLKCTRL_SLOTSIZE_Pos    0            /**< \brief (I2S_CLKCTRL) Slot Size */
#define I2S_CLKCTRL_SLOTSIZE_Msk    (_U_(0x3) << I2S_CLKCTRL_SLOTSIZE_Pos)
#define I2S_CLKCTRL_SLOTSIZE(value) (I2S_CLKCTRL_SLOTSIZE_Msk & ((value) << I2S_CLKCTRL_SLOTSIZE_Pos))
#define   I2S_CLKCTRL_SLOTSIZE_8_Val      _U_(0x0)   /**< \brief (I2S_CLKCTRL) 8-bit Slot for Clock Unit n */
#define   I2S_CLKCTRL_SLOTSIZE_16_Val     _U_(0x1)   /**< \brief (I2S_CLKCTRL) 16-bit Slot for Clock Unit n */
#define   I2S_CLKCTRL_SLOTSIZE_24_Val     _U_(0x2)   /**< \brief (I2S_CLKCTRL) 24-bit Slot for Clock Unit n */
#define   I2S_CLKCTRL_SLOTSIZE_32_Val     _U_(0x3)   /**< \brief (I2S_CLKCTRL) 32-bit Slot for Clock Unit n */
#define I2S_CLKCTRL_SLOTSIZE_8      (I2S_CLKCTRL_SLOTSIZE_8_Val    << I2S_CLKCTRL_SLOTSIZE_Pos)
#define I2S_CLKCTRL_SLOTSIZE_16     (I2S_CLKCTRL_SLOTSIZE_16_Val   << I2S_CLKCTRL_SLOTSIZE_Pos)
#define I2S_CLKCTRL_SLOTSIZE_24     (I2S_CLKCTRL_SLOTSIZE_24_Val   << I2S_CLKCTRL_SLOTSIZE_Pos)
#define I2S_CLKCTRL_SLOTSIZE_32     (I2S_CLKCTRL_SLOTSIZE_32_Val   << I2S_CLKCTRL_SLOTSIZE_Pos)
#define I2S_CLKCTRL_NBSLOTS_Pos     2            /**< \brief (I2S_CLKCTRL) Number of Slots in Frame */
#define I2S_CLKCTRL_NBSLOTS_Msk     (_U_(0x7) << I2S_CLKCTRL_NBSLOTS_Pos)
#define I2S_CLKCTRL_NBSLOTS(value)  (I2S_CLKCTRL_NBSLOTS_Msk & ((value) << I2S_CLKCTRL_NBSLOTS_Pos))
#define I2S_CLKCTRL_FSWIDTH_Pos     5            /**< \brief (I2S_CLKCTRL) Frame Sync Width */
#define I2S_CLKCTRL_FSWIDTH_Msk     (_U_(0x3) << I2S_CLKCTRL_FSWIDTH_Pos)
#define I2S_CLKCTRL_FSWIDTH(value)  (I2S_CLKCTRL_FSWIDTH_Msk & ((value) << I2S_CLKCTRL_FSWIDTH_Pos))
#define   I2S_CLKCTRL_FSWIDTH_SLOT_Val    _U_(0x0)   /**< \brief (I2S_CLKCTRL) Frame Sync Pulse is 1 Slot wide (default for I2S protocol) */
#define   I2S_CLKCTRL_FSWIDTH_HALF_Val    _U_(0x1)   /**< \brief (I2S_CLKCTRL) Frame Sync Pulse is half a Frame wide */
#define   I2S_CLKCTRL_FSWIDTH_BIT_Val     _U_(0x2)   /**< \brief (I2S_CLKCTRL) Frame Sync Pulse is 1 Bit wide */
#define   I2S_CLKCTRL_FSWIDTH_BURST_Val   _U_(0x3)   /**< \brief (I2S_CLKCTRL) Clock Unit n operates in Burst mode, with a 1-bit wide Frame Sync pulse per Data sample, only when Data transfer is requested */
#define I2S_CLKCTRL_FSWIDTH_SLOT    (I2S_CLKCTRL_FSWIDTH_SLOT_Val  << I2S_CLKCTRL_FSWIDTH_Pos)
#define I2S_CLKCTRL_FSWIDTH_HALF    (I2S_CLKCTRL_FSWIDTH_HALF_Val  << I2S_CLKCTRL_FSWIDTH_Pos)
#define I2S_CLKCTRL_FSWIDTH_BIT     (I2S_CLKCTRL_FSWIDTH_BIT_Val   << I2S_CLKCTRL_FSWIDTH_Pos)
#define I2S_CLKCTRL_FSWIDTH_BURST   (I2S_CLKCTRL_FSWIDTH_BURST_Val << I2S_CLKCTRL_FSWIDTH_Pos)
#define I2S_CLKCTRL_BITDELAY_Pos    7            /**< \brief (I2S_CLKCTRL) Data Delay from Frame Sync */
#define I2S_CLKCTRL_BITDELAY        (_U_(0x1) << I2S_CLKCTRL_BITDELAY_Pos)
#define   I2S_CLKCTRL_BITDELAY_LJ_Val     _U_(0x0)   /**< \brief (I2S_CLKCTRL) Left Justified (0 Bit Delay) */
#define   I2S_CLKCTRL_BITDELAY_I2S_Val    _U_(0x1)   /**< \brief (I2S_CLKCTRL) I2S (1 Bit Delay) */
#define I2S_CLKCTRL_BITDELAY_LJ     (I2S_CLKCTRL_BITDELAY_LJ_Val   << I2S_CLKCTRL_BITDELAY_Pos)
#define I2S_CLKCTRL_BITDELAY_I2S    (I2S_CLKCTRL_BITDELAY_I2S_Val  << I2S_CLKCTRL_BITDELAY_Pos)
#define I2S_CLKCTRL_FSSEL_Pos       8            /**< \brief (I2S_CLKCTRL) Frame Sync Select */
#define I2S_CLKCTRL_FSSEL           (_U_(0x1) << I2S_CLKCTRL_FSSEL_Pos)
#define   I2S_CLKCTRL_FSSEL_SCKDIV_Val    _U_(0x0)   /**< \brief (I2S_CLKCTRL) Divided Serial Clock n is used as Frame Sync n source */
#define   I2S_CLKCTRL_FSSEL_FSPIN_Val     _U_(0x1)   /**< \brief (I2S_CLKCTRL) FSn input pin is used as Frame Sync n source */
#define I2S_CLKCTRL_FSSEL_SCKDIV    (I2S_CLKCTRL_FSSEL_SCKDIV_Val  << I2S_CLKCTRL_FSSEL_Pos)
#define I2S_CLKCTRL_FSSEL_FSPIN     (I2S_CLKCTRL_FSSEL_FSPIN_Val   << I2S_CLKCTRL_FSSEL_Pos)
#define I2S_CLKCTRL_FSINV_Pos       9            /**< \brief (I2S_CLKCTRL) Frame Sync Invert */
#define I2S_CLKCTRL_FSINV           (_U_(0x1) << I2S_CLKCTRL_FSINV_Pos)
#define I2S_CLKCTRL_FSOUTINV_Pos    10           /**< \brief (I2S_CLKCTRL) Frame Sync Output Invert */
#define I2S_CLKCTRL_FSOUTINV        (_U_(0x1) << I2S_CLKCTRL_FSOUTINV_Pos)
#define I2S_CLKCTRL_SCKSEL_Pos      11           /**< \brief (I2S_CLKCTRL) Serial Clock Select */
#define I2S_CLKCTRL_SCKSEL          (_U_(0x1) << I2S_CLKCTRL_SCKSEL_Pos)
#define   I2S_CLKCTRL_SCKSEL_MCKDIV_Val   _U_(0x0)   /**< \brief (I2S_CLKCTRL) Divided Master Clock n is used as Serial Clock n source */
#define   I2S_CLKCTRL_SCKSEL_SCKPIN_Val   _U_(0x1)   /**< \brief (I2S_CLKCTRL) SCKn input pin is used as Serial Clock n source */
#define I2S_CLKCTRL_SCKSEL_MCKDIV   (I2S_CLKCTRL_SCKSEL_MCKDIV_Val << I2S_CLKCTRL_SCKSEL_Pos)
#define I2S_CLKCTRL_SCKSEL_SCKPIN   (I2S_CLKCTRL_SCKSEL_SCKPIN_Val << I2S_CLKCTRL_SCKSEL_Pos)
#define I2S_CLKCTRL_SCKOUTINV_Pos   12           /**< \brief (I2S_CLKCTRL) Serial Clock Output Invert */
#define I2S_CLKCTRL_SCKOUTINV       (_U_(0x1) << I2S_CLKCTRL_SCKOUTINV_Pos)
#define I2S_CLKCTRL_MCKSEL_Pos      13           /**< \brief (I2S_CLKCTRL) Master Clock Select */
#define I2S_CLKCTRL_MCKSEL          (_U_(0x1) << I2S_CLKCTRL_MCKSEL_Pos)
#define   I2S_CLKCTRL_MCKSEL_GCLK_Val     _U_(0x0)   /**< \brief (I2S_CLKCTRL) GCLK_I2S_n is used as Master Clock n source */
#define   I2S_CLKCTRL_MCKSEL_MCKPIN_Val   _U_(0x1)   /**< \brief (I2S_CLKCTRL) MCKn input pin is used as Master Clock n source */
#define I2S_CLKCTRL_MCKSEL_GCLK     (I2S_CLKCTRL_MCKSEL_GCLK_Val   << I2S_CLKCTRL_MCKSEL_Pos)
#define I2S_CLKCTRL_MCKSEL_MCKPIN   (I2S_CLKCTRL_MCKSEL_MCKPIN_Val << I2S_CLKCTRL_MCKSEL_Pos)
#define I2S_CLKCTRL_MCKEN_Pos       14           /**< \brief (I2S_CLKCTRL) Master Clock Enable */
#define I2S_CLKCTRL_MCKEN           (_U_(0x1) << I2S_CLKCTRL_MCKEN_Pos)
#define I2S_CLKCTRL_MCKOUTINV_Pos   15           /**< \brief (I2S_CLKCTRL) Master Clock Output Invert */
#define I2S_CLKCTRL_MCKOUTINV       (_U_(0x1) << I2S_CLKCTRL_MCKOUTINV_Pos)
#define I2S_CLKCTRL_MCKDIV_Pos      16           /**< \brief (I2S_CLKCTRL) Master Clock Division Factor */
#define I2S_CLKCTRL_MCKDIV_Msk      (_U_(0x3F) << I2S_CLKCTRL_MCKDIV_Pos)
#define I2S_CLKCTRL_MCKDIV(value)   (I2S_CLKCTRL_MCKDIV_Msk & ((value) << I2S_CLKCTRL_MCKDIV_Pos))
#define I2S_CLKCTRL_MCKOUTDIV_Pos   24           /**< \brief (I2S_CLKCTRL) Master Clock Output Division Factor */
#define I2S_CLKCTRL_MCKOUTDIV_Msk   (_U_(0x3F) << I2S_CLKCTRL_MCKOUTDIV_Pos)
#define I2S_CLKCTRL_MCKOUTDIV(value) (I2S_CLKCTRL_MCKOUTDIV_Msk & ((value) << I2S_CLKCTRL_MCKOUTDIV_Pos))
#define I2S_CLKCTRL_MASK            _U_(0x3F3FFFFF) /**< \brief (I2S_CLKCTRL) MASK Register */

/* -------- I2S_INTENCLR : (I2S Offset: 0x0C) (R/W 16) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t RXRDY0:1;         /*!< bit:      0  Receive Ready 0 Interrupt Enable   */
    uint16_t RXRDY1:1;         /*!< bit:      1  Receive Ready 1 Interrupt Enable   */
    uint16_t :2;               /*!< bit:  2.. 3  Reserved                           */
    uint16_t RXOR0:1;          /*!< bit:      4  Receive Overrun 0 Interrupt Enable */
    uint16_t RXOR1:1;          /*!< bit:      5  Receive Overrun 1 Interrupt Enable */
    uint16_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint16_t TXRDY0:1;         /*!< bit:      8  Transmit Ready 0 Interrupt Enable  */
    uint16_t TXRDY1:1;         /*!< bit:      9  Transmit Ready 1 Interrupt Enable  */
    uint16_t :2;               /*!< bit: 10..11  Reserved                           */
    uint16_t TXUR0:1;          /*!< bit:     12  Transmit Underrun 0 Interrupt Enable */
    uint16_t TXUR1:1;          /*!< bit:     13  Transmit Underrun 1 Interrupt Enable */
    uint16_t :2;               /*!< bit: 14..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t RXRDY:2;          /*!< bit:  0.. 1  Receive Ready x Interrupt Enable   */
    uint16_t :2;               /*!< bit:  2.. 3  Reserved                           */
    uint16_t RXOR:2;           /*!< bit:  4.. 5  Receive Overrun x Interrupt Enable */
    uint16_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint16_t TXRDY:2;          /*!< bit:  8.. 9  Transmit Ready x Interrupt Enable  */
    uint16_t :2;               /*!< bit: 10..11  Reserved                           */
    uint16_t TXUR:2;           /*!< bit: 12..13  Transmit Underrun x Interrupt Enable */
    uint16_t :2;               /*!< bit: 14..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} I2S_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define I2S_INTENCLR_OFFSET         0x0C         /**< \brief (I2S_INTENCLR offset) Interrupt Enable Clear */
#define I2S_INTENCLR_RESETVALUE     _U_(0x0000)  /**< \brief (I2S_INTENCLR reset_value) Interrupt Enable Clear */

#define I2S_INTENCLR_RXRDY0_Pos     0            /**< \brief (I2S_INTENCLR) Receive Ready 0 Interrupt Enable */
#define I2S_INTENCLR_RXRDY0         (_U_(1) << I2S_INTENCLR_RXRDY0_Pos)
#define I2S_INTENCLR_RXRDY1_Pos     1            /**< \brief (I2S_INTENCLR) Receive Ready 1 Interrupt Enable */
#define I2S_INTENCLR_RXRDY1         (_U_(1) << I2S_INTENCLR_RXRDY1_Pos)
#define I2S_INTENCLR_RXRDY_Pos      0            /**< \brief (I2S_INTENCLR) Receive Ready x Interrupt Enable */
#define I2S_INTENCLR_RXRDY_Msk      (_U_(0x3) << I2S_INTENCLR_RXRDY_Pos)
#define I2S_INTENCLR_RXRDY(value)   (I2S_INTENCLR_RXRDY_Msk & ((value) << I2S_INTENCLR_RXRDY_Pos))
#define I2S_INTENCLR_RXOR0_Pos      4            /**< \brief (I2S_INTENCLR) Receive Overrun 0 Interrupt Enable */
#define I2S_INTENCLR_RXOR0          (_U_(1) << I2S_INTENCLR_RXOR0_Pos)
#define I2S_INTENCLR_RXOR1_Pos      5            /**< \brief (I2S_INTENCLR) Receive Overrun 1 Interrupt Enable */
#define I2S_INTENCLR_RXOR1          (_U_(1) << I2S_INTENCLR_RXOR1_Pos)
#define I2S_INTENCLR_RXOR_Pos       4            /**< \brief (I2S_INTENCLR) Receive Overrun x Interrupt Enable */
#define I2S_INTENCLR_RXOR_Msk       (_U_(0x3) << I2S_INTENCLR_RXOR_Pos)
#define I2S_INTENCLR_RXOR(value)    (I2S_INTENCLR_RXOR_Msk & ((value) << I2S_INTENCLR_RXOR_Pos))
#define I2S_INTENCLR_TXRDY0_Pos     8            /**< \brief (I2S_INTENCLR) Transmit Ready 0 Interrupt Enable */
#define I2S_INTENCLR_TXRDY0         (_U_(1) << I2S_INTENCLR_TXRDY0_Pos)
#define I2S_INTENCLR_TXRDY1_Pos     9            /**< \brief (I2S_INTENCLR) Transmit Ready 1 Interrupt Enable */
#define I2S_INTENCLR_TXRDY1         (_U_(1) << I2S_INTENCLR_TXRDY1_Pos)
#define I2S_INTENCLR_TXRDY_Pos      8            /**< \brief (I2S_INTENCLR) Transmit Ready x Interrupt Enable */
#define I2S_INTENCLR_TXRDY_Msk      (_U_(0x3) << I2S_INTENCLR_TXRDY_Pos)
#define I2S_INTENCLR_TXRDY(value)   (I2S_INTENCLR_TXRDY_Msk & ((value) << I2S_INTENCLR_TXRDY_Pos))
#define I2S_INTENCLR_TXUR0_Pos      12           /**< \brief (I2S_INTENCLR) Transmit Underrun 0 Interrupt Enable */
#define I2S_INTENCLR_TXUR0          (_U_(1) << I2S_INTENCLR_TXUR0_Pos)
#define I2S_INTENCLR_TXUR1_Pos      13           /**< \brief (I2S_INTENCLR) Transmit Underrun 1 Interrupt Enable */
#define I2S_INTENCLR_TXUR1          (_U_(1) << I2S_INTENCLR_TXUR1_Pos)
#define I2S_INTENCLR_TXUR_Pos       12           /**< \brief (I2S_INTENCLR) Transmit Underrun x Interrupt Enable */
#define I2S_INTENCLR_TXUR_Msk       (_U_(0x3) << I2S_INTENCLR_TXUR_Pos)
#define I2S_INTENCLR_TXUR(value)    (I2S_INTENCLR_TXUR_Msk & ((value) << I2S_INTENCLR_TXUR_Pos))
#define I2S_INTENCLR_MASK           _U_(0x3333)  /**< \brief (I2S_INTENCLR) MASK Register */

/* -------- I2S_INTENSET : (I2S Offset: 0x10) (R/W 16) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t RXRDY0:1;         /*!< bit:      0  Receive Ready 0 Interrupt Enable   */
    uint16_t RXRDY1:1;         /*!< bit:      1  Receive Ready 1 Interrupt Enable   */
    uint16_t :2;               /*!< bit:  2.. 3  Reserved                           */
    uint16_t RXOR0:1;          /*!< bit:      4  Receive Overrun 0 Interrupt Enable */
    uint16_t RXOR1:1;          /*!< bit:      5  Receive Overrun 1 Interrupt Enable */
    uint16_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint16_t TXRDY0:1;         /*!< bit:      8  Transmit Ready 0 Interrupt Enable  */
    uint16_t TXRDY1:1;         /*!< bit:      9  Transmit Ready 1 Interrupt Enable  */
    uint16_t :2;               /*!< bit: 10..11  Reserved                           */
    uint16_t TXUR0:1;          /*!< bit:     12  Transmit Underrun 0 Interrupt Enable */
    uint16_t TXUR1:1;          /*!< bit:     13  Transmit Underrun 1 Interrupt Enable */
    uint16_t :2;               /*!< bit: 14..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t RXRDY:2;          /*!< bit:  0.. 1  Receive Ready x Interrupt Enable   */
    uint16_t :2;               /*!< bit:  2.. 3  Reserved                           */
    uint16_t RXOR:2;           /*!< bit:  4.. 5  Receive Overrun x Interrupt Enable */
    uint16_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint16_t TXRDY:2;          /*!< bit:  8.. 9  Transmit Ready x Interrupt Enable  */
    uint16_t :2;               /*!< bit: 10..11  Reserved                           */
    uint16_t TXUR:2;           /*!< bit: 12..13  Transmit Underrun x Interrupt Enable */
    uint16_t :2;               /*!< bit: 14..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} I2S_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define I2S_INTENSET_OFFSET         0x10         /**< \brief (I2S_INTENSET offset) Interrupt Enable Set */
#define I2S_INTENSET_RESETVALUE     _U_(0x0000)  /**< \brief (I2S_INTENSET reset_value) Interrupt Enable Set */

#define I2S_INTENSET_RXRDY0_Pos     0            /**< \brief (I2S_INTENSET) Receive Ready 0 Interrupt Enable */
#define I2S_INTENSET_RXRDY0         (_U_(1) << I2S_INTENSET_RXRDY0_Pos)
#define I2S_INTENSET_RXRDY1_Pos     1            /**< \brief (I2S_INTENSET) Receive Ready 1 Interrupt Enable */
#define I2S_INTENSET_RXRDY1         (_U_(1) << I2S_INTENSET_RXRDY1_Pos)
#define I2S_INTENSET_RXRDY_Pos      0            /**< \brief (I2S_INTENSET) Receive Ready x Interrupt Enable */
#define I2S_INTENSET_RXRDY_Msk      (_U_(0x3) << I2S_INTENSET_RXRDY_Pos)
#define I2S_INTENSET_RXRDY(value)   (I2S_INTENSET_RXRDY_Msk & ((value) << I2S_INTENSET_RXRDY_Pos))
#define I2S_INTENSET_RXOR0_Pos      4            /**< \brief (I2S_INTENSET) Receive Overrun 0 Interrupt Enable */
#define I2S_INTENSET_RXOR0          (_U_(1) << I2S_INTENSET_RXOR0_Pos)
#define I2S_INTENSET_RXOR1_Pos      5            /**< \brief (I2S_INTENSET) Receive Overrun 1 Interrupt Enable */
#define I2S_INTENSET_RXOR1          (_U_(1) << I2S_INTENSET_RXOR1_Pos)
#define I2S_INTENSET_RXOR_Pos       4            /**< \brief (I2S_INTENSET) Receive Overrun x Interrupt Enable */
#define I2S_INTENSET_RXOR_Msk       (_U_(0x3) << I2S_INTENSET_RXOR_Pos)
#define I2S_INTENSET_RXOR(value)    (I2S_INTENSET_RXOR_Msk & ((value) << I2S_INTENSET_RXOR_Pos))
#define I2S_INTENSET_TXRDY0_Pos     8            /**< \brief (I2S_INTENSET) Transmit Ready 0 Interrupt Enable */
#define I2S_INTENSET_TXRDY0         (_U_(1) << I2S_INTENSET_TXRDY0_Pos)
#define I2S_INTENSET_TXRDY1_Pos     9            /**< \brief (I2S_INTENSET) Transmit Ready 1 Interrupt Enable */
#define I2S_INTENSET_TXRDY1         (_U_(1) << I2S_INTENSET_TXRDY1_Pos)
#define I2S_INTENSET_TXRDY_Pos      8            /**< \brief (I2S_INTENSET) Transmit Ready x Interrupt Enable */
#define I2S_INTENSET_TXRDY_Msk      (_U_(0x3) << I2S_INTENSET_TXRDY_Pos)
#define I2S_INTENSET_TXRDY(value)   (I2S_INTENSET_TXRDY_Msk & ((value) << I2S_INTENSET_TXRDY_Pos))
#define I2S_INTENSET_TXUR0_Pos      12           /**< \brief (I2S_INTENSET) Transmit Underrun 0 Interrupt Enable */
#define I2S_INTENSET_TXUR0          (_U_(1) << I2S_INTENSET_TXUR0_Pos)
#define I2S_INTENSET_TXUR1_Pos      13           /**< \brief (I2S_INTENSET) Transmit Underrun 1 Interrupt Enable */
#define I2S_INTENSET_TXUR1          (_U_(1) << I2S_INTENSET_TXUR1_Pos)
#define I2S_INTENSET_TXUR_Pos       12           /**< \brief (I2S_INTENSET) Transmit Underrun x Interrupt Enable */
#define I2S_INTENSET_TXUR_Msk       (_U_(0x3) << I2S_INTENSET_TXUR_Pos)
#define I2S_INTENSET_TXUR(value)    (I2S_INTENSET_TXUR_Msk & ((value) << I2S_INTENSET_TXUR_Pos))
#define I2S_INTENSET_MASK           _U_(0x3333)  /**< \brief (I2S_INTENSET) MASK Register */

/* -------- I2S_INTFLAG : (I2S Offset: 0x14) (R/W 16) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint16_t RXRDY0:1;         /*!< bit:      0  Receive Ready 0                    */
    __I uint16_t RXRDY1:1;         /*!< bit:      1  Receive Ready 1                    */
    __I uint16_t :2;               /*!< bit:  2.. 3  Reserved                           */
    __I uint16_t RXOR0:1;          /*!< bit:      4  Receive Overrun 0                  */
    __I uint16_t RXOR1:1;          /*!< bit:      5  Receive Overrun 1                  */
    __I uint16_t :2;               /*!< bit:  6.. 7  Reserved                           */
    __I uint16_t TXRDY0:1;         /*!< bit:      8  Transmit Ready 0                   */
    __I uint16_t TXRDY1:1;         /*!< bit:      9  Transmit Ready 1                   */
    __I uint16_t :2;               /*!< bit: 10..11  Reserved                           */
    __I uint16_t TXUR0:1;          /*!< bit:     12  Transmit Underrun 0                */
    __I uint16_t TXUR1:1;          /*!< bit:     13  Transmit Underrun 1                */
    __I uint16_t :2;               /*!< bit: 14..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint16_t RXRDY:2;          /*!< bit:  0.. 1  Receive Ready x                    */
    __I uint16_t :2;               /*!< bit:  2.. 3  Reserved                           */
    __I uint16_t RXOR:2;           /*!< bit:  4.. 5  Receive Overrun x                  */
    __I uint16_t :2;               /*!< bit:  6.. 7  Reserved                           */
    __I uint16_t TXRDY:2;          /*!< bit:  8.. 9  Transmit Ready x                   */
    __I uint16_t :2;               /*!< bit: 10..11  Reserved                           */
    __I uint16_t TXUR:2;           /*!< bit: 12..13  Transmit Underrun x                */
    __I uint16_t :2;               /*!< bit: 14..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} I2S_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define I2S_INTFLAG_OFFSET          0x14         /**< \brief (I2S_INTFLAG offset) Interrupt Flag Status and Clear */
#define I2S_INTFLAG_RESETVALUE      _U_(0x0000)  /**< \brief (I2S_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define I2S_INTFLAG_RXRDY0_Pos      0            /**< \brief (I2S_INTFLAG) Receive Ready 0 */
#define I2S_INTFLAG_RXRDY0          (_U_(1) << I2S_INTFLAG_RXRDY0_Pos)
#define I2S_INTFLAG_RXRDY1_Pos      1            /**< \brief (I2S_INTFLAG) Receive Ready 1 */
#define I2S_INTFLAG_RXRDY1          (_U_(1) << I2S_INTFLAG_RXRDY1_Pos)
#define I2S_INTFLAG_RXRDY_Pos       0            /**< \brief (I2S_INTFLAG) Receive Ready x */
#define I2S_INTFLAG_RXRDY_Msk       (_U_(0x3) << I2S_INTFLAG_RXRDY_Pos)
#define I2S_INTFLAG_RXRDY(value)    (I2S_INTFLAG_RXRDY_Msk & ((value) << I2S_INTFLAG_RXRDY_Pos))
#define I2S_INTFLAG_RXOR0_Pos       4            /**< \brief (I2S_INTFLAG) Receive Overrun 0 */
#define I2S_INTFLAG_RXOR0           (_U_(1) << I2S_INTFLAG_RXOR0_Pos)
#define I2S_INTFLAG_RXOR1_Pos       5            /**< \brief (I2S_INTFLAG) Receive Overrun 1 */
#define I2S_INTFLAG_RXOR1           (_U_(1) << I2S_INTFLAG_RXOR1_Pos)
#define I2S_INTFLAG_RXOR_Pos        4            /**< \brief (I2S_INTFLAG) Receive Overrun x */
#define I2S_INTFLAG_RXOR_Msk        (_U_(0x3) << I2S_INTFLAG_RXOR_Pos)
#define I2S_INTFLAG_RXOR(value)     (I2S_INTFLAG_RXOR_Msk & ((value) << I2S_INTFLAG_RXOR_Pos))
#define I2S_INTFLAG_TXRDY0_Pos      8            /**< \brief (I2S_INTFLAG) Transmit Ready 0 */
#define I2S_INTFLAG_TXRDY0          (_U_(1) << I2S_INTFLAG_TXRDY0_Pos)
#define I2S_INTFLAG_TXRDY1_Pos      9            /**< \brief (I2S_INTFLAG) Transmit Ready 1 */
#define I2S_INTFLAG_TXRDY1          (_U_(1) << I2S_INTFLAG_TXRDY1_Pos)
#define I2S_INTFLAG_TXRDY_Pos       8            /**< \brief (I2S_INTFLAG) Transmit Ready x */
#define I2S_INTFLAG_TXRDY_Msk       (_U_(0x3) << I2S_INTFLAG_TXRDY_Pos)
#define I2S_INTFLAG_TXRDY(value)    (I2S_INTFLAG_TXRDY_Msk & ((value) << I2S_INTFLAG_TXRDY_Pos))
#define I2S_INTFLAG_TXUR0_Pos       12           /**< \brief (I2S_INTFLAG) Transmit Underrun 0 */
#define I2S_INTFLAG_TXUR0           (_U_(1) << I2S_INTFLAG_TXUR0_Pos)
#define I2S_INTFLAG_TXUR1_Pos       13           /**< \brief (I2S_INTFLAG) Transmit Underrun 1 */
#define I2S_INTFLAG_TXUR1           (_U_(1) << I2S_INTFLAG_TXUR1_Pos)
#define I2S_INTFLAG_TXUR_Pos        12           /**< \brief (I2S_INTFLAG) Transmit Underrun x */
#define I2S_INTFLAG_TXUR_Msk        (_U_(0x3) << I2S_INTFLAG_TXUR_Pos)
#define I2S_INTFLAG_TXUR(value)     (I2S_INTFLAG_TXUR_Msk & ((value) << I2S_INTFLAG_TXUR_Pos))
#define I2S_INTFLAG_MASK            _U_(0x3333)  /**< \brief (I2S_INTFLAG) MASK Register */

/* -------- I2S_SYNCBUSY : (I2S Offset: 0x18) (R/  16) Synchronization Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t SWRST:1;          /*!< bit:      0  Software Reset Synchronization Status */
    uint16_t ENABLE:1;         /*!< bit:      1  Enable Synchronization Status      */
    uint16_t CKEN0:1;          /*!< bit:      2  Clock Unit 0 Enable Synchronization Status */
    uint16_t CKEN1:1;          /*!< bit:      3  Clock Unit 1 Enable Synchronization Status */
    uint16_t TXEN:1;           /*!< bit:      4  Tx Serializer Enable Synchronization Status */
    uint16_t RXEN:1;           /*!< bit:      5  Rx Serializer Enable Synchronization Status */
    uint16_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint16_t TXDATA:1;         /*!< bit:      8  Tx Data Synchronization Status     */
    uint16_t RXDATA:1;         /*!< bit:      9  Rx Data Synchronization Status     */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t :2;               /*!< bit:  0.. 1  Reserved                           */
    uint16_t CKEN:2;           /*!< bit:  2.. 3  Clock Unit x Enable Synchronization Status */
    uint16_t :12;              /*!< bit:  4..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} I2S_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define I2S_SYNCBUSY_OFFSET         0x18         /**< \brief (I2S_SYNCBUSY offset) Synchronization Status */
#define I2S_SYNCBUSY_RESETVALUE     _U_(0x0000)  /**< \brief (I2S_SYNCBUSY reset_value) Synchronization Status */

#define I2S_SYNCBUSY_SWRST_Pos      0            /**< \brief (I2S_SYNCBUSY) Software Reset Synchronization Status */
#define I2S_SYNCBUSY_SWRST          (_U_(0x1) << I2S_SYNCBUSY_SWRST_Pos)
#define I2S_SYNCBUSY_ENABLE_Pos     1            /**< \brief (I2S_SYNCBUSY) Enable Synchronization Status */
#define I2S_SYNCBUSY_ENABLE         (_U_(0x1) << I2S_SYNCBUSY_ENABLE_Pos)
#define I2S_SYNCBUSY_CKEN0_Pos      2            /**< \brief (I2S_SYNCBUSY) Clock Unit 0 Enable Synchronization Status */
#define I2S_SYNCBUSY_CKEN0          (_U_(1) << I2S_SYNCBUSY_CKEN0_Pos)
#define I2S_SYNCBUSY_CKEN1_Pos      3            /**< \brief (I2S_SYNCBUSY) Clock Unit 1 Enable Synchronization Status */
#define I2S_SYNCBUSY_CKEN1          (_U_(1) << I2S_SYNCBUSY_CKEN1_Pos)
#define I2S_SYNCBUSY_CKEN_Pos       2            /**< \brief (I2S_SYNCBUSY) Clock Unit x Enable Synchronization Status */
#define I2S_SYNCBUSY_CKEN_Msk       (_U_(0x3) << I2S_SYNCBUSY_CKEN_Pos)
#define I2S_SYNCBUSY_CKEN(value)    (I2S_SYNCBUSY_CKEN_Msk & ((value) << I2S_SYNCBUSY_CKEN_Pos))
#define I2S_SYNCBUSY_TXEN_Pos       4            /**< \brief (I2S_SYNCBUSY) Tx Serializer Enable Synchronization Status */
#define I2S_SYNCBUSY_TXEN           (_U_(0x1) << I2S_SYNCBUSY_TXEN_Pos)
#define I2S_SYNCBUSY_RXEN_Pos       5            /**< \brief (I2S_SYNCBUSY) Rx Serializer Enable Synchronization Status */
#define I2S_SYNCBUSY_RXEN           (_U_(0x1) << I2S_SYNCBUSY_RXEN_Pos)
#define I2S_SYNCBUSY_TXDATA_Pos     8            /**< \brief (I2S_SYNCBUSY) Tx Data Synchronization Status */
#define I2S_SYNCBUSY_TXDATA         (_U_(0x1) << I2S_SYNCBUSY_TXDATA_Pos)
#define I2S_SYNCBUSY_RXDATA_Pos     9            /**< \brief (I2S_SYNCBUSY) Rx Data Synchronization Status */
#define I2S_SYNCBUSY_RXDATA         (_U_(0x1) << I2S_SYNCBUSY_RXDATA_Pos)
#define I2S_SYNCBUSY_MASK           _U_(0x033F)  /**< \brief (I2S_SYNCBUSY) MASK Register */

/* -------- I2S_TXCTRL : (I2S Offset: 0x20) (R/W 32) Tx Serializer Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :2;               /*!< bit:  0.. 1  Reserved                           */
    uint32_t TXDEFAULT:2;      /*!< bit:  2.. 3  Line Default Line when Slot Disabled */
    uint32_t TXSAME:1;         /*!< bit:      4  Transmit Data when Underrun        */
    uint32_t :2;               /*!< bit:  5.. 6  Reserved                           */
    uint32_t SLOTADJ:1;        /*!< bit:      7  Data Slot Formatting Adjust        */
    uint32_t DATASIZE:3;       /*!< bit:  8..10  Data Word Size                     */
    uint32_t :1;               /*!< bit:     11  Reserved                           */
    uint32_t WORDADJ:1;        /*!< bit:     12  Data Word Formatting Adjust        */
    uint32_t EXTEND:2;         /*!< bit: 13..14  Data Formatting Bit Extension      */
    uint32_t BITREV:1;         /*!< bit:     15  Data Formatting Bit Reverse        */
    uint32_t SLOTDIS0:1;       /*!< bit:     16  Slot 0 Disabled for this Serializer */
    uint32_t SLOTDIS1:1;       /*!< bit:     17  Slot 1 Disabled for this Serializer */
    uint32_t SLOTDIS2:1;       /*!< bit:     18  Slot 2 Disabled for this Serializer */
    uint32_t SLOTDIS3:1;       /*!< bit:     19  Slot 3 Disabled for this Serializer */
    uint32_t SLOTDIS4:1;       /*!< bit:     20  Slot 4 Disabled for this Serializer */
    uint32_t SLOTDIS5:1;       /*!< bit:     21  Slot 5 Disabled for this Serializer */
    uint32_t SLOTDIS6:1;       /*!< bit:     22  Slot 6 Disabled for this Serializer */
    uint32_t SLOTDIS7:1;       /*!< bit:     23  Slot 7 Disabled for this Serializer */
    uint32_t MONO:1;           /*!< bit:     24  Mono Mode                          */
    uint32_t DMA:1;            /*!< bit:     25  Single or Multiple DMA Channels    */
    uint32_t :6;               /*!< bit: 26..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :16;              /*!< bit:  0..15  Reserved                           */
    uint32_t SLOTDIS:8;        /*!< bit: 16..23  Slot x Disabled for this Serializer */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} I2S_TXCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define I2S_TXCTRL_OFFSET           0x20         /**< \brief (I2S_TXCTRL offset) Tx Serializer Control */
#define I2S_TXCTRL_RESETVALUE       _U_(0x00000000) /**< \brief (I2S_TXCTRL reset_value) Tx Serializer Control */

#define I2S_TXCTRL_TXDEFAULT_Pos    2            /**< \brief (I2S_TXCTRL) Line Default Line when Slot Disabled */
#define I2S_TXCTRL_TXDEFAULT_Msk    (_U_(0x3) << I2S_TXCTRL_TXDEFAULT_Pos)
#define I2S_TXCTRL_TXDEFAULT(value) (I2S_TXCTRL_TXDEFAULT_Msk & ((value) << I2S_TXCTRL_TXDEFAULT_Pos))
#define   I2S_TXCTRL_TXDEFAULT_ZERO_Val   _U_(0x0)   /**< \brief (I2S_TXCTRL) Output Default Value is 0 */
#define   I2S_TXCTRL_TXDEFAULT_ONE_Val    _U_(0x1)   /**< \brief (I2S_TXCTRL) Output Default Value is 1 */
#define   I2S_TXCTRL_TXDEFAULT_HIZ_Val    _U_(0x3)   /**< \brief (I2S_TXCTRL) Output Default Value is high impedance */
#define I2S_TXCTRL_TXDEFAULT_ZERO   (I2S_TXCTRL_TXDEFAULT_ZERO_Val << I2S_TXCTRL_TXDEFAULT_Pos)
#define I2S_TXCTRL_TXDEFAULT_ONE    (I2S_TXCTRL_TXDEFAULT_ONE_Val  << I2S_TXCTRL_TXDEFAULT_Pos)
#define I2S_TXCTRL_TXDEFAULT_HIZ    (I2S_TXCTRL_TXDEFAULT_HIZ_Val  << I2S_TXCTRL_TXDEFAULT_Pos)
#define I2S_TXCTRL_TXSAME_Pos       4            /**< \brief (I2S_TXCTRL) Transmit Data when Underrun */
#define I2S_TXCTRL_TXSAME           (_U_(0x1) << I2S_TXCTRL_TXSAME_Pos)
#define   I2S_TXCTRL_TXSAME_ZERO_Val      _U_(0x0)   /**< \brief (I2S_TXCTRL) Zero data transmitted in case of underrun */
#define   I2S_TXCTRL_TXSAME_SAME_Val      _U_(0x1)   /**< \brief (I2S_TXCTRL) Last data transmitted in case of underrun */
#define I2S_TXCTRL_TXSAME_ZERO      (I2S_TXCTRL_TXSAME_ZERO_Val    << I2S_TXCTRL_TXSAME_Pos)
#define I2S_TXCTRL_TXSAME_SAME      (I2S_TXCTRL_TXSAME_SAME_Val    << I2S_TXCTRL_TXSAME_Pos)
#define I2S_TXCTRL_SLOTADJ_Pos      7            /**< \brief (I2S_TXCTRL) Data Slot Formatting Adjust */
#define I2S_TXCTRL_SLOTADJ          (_U_(0x1) << I2S_TXCTRL_SLOTADJ_Pos)
#define   I2S_TXCTRL_SLOTADJ_RIGHT_Val    _U_(0x0)   /**< \brief (I2S_TXCTRL) Data is right adjusted in slot */
#define   I2S_TXCTRL_SLOTADJ_LEFT_Val     _U_(0x1)   /**< \brief (I2S_TXCTRL) Data is left adjusted in slot */
#define I2S_TXCTRL_SLOTADJ_RIGHT    (I2S_TXCTRL_SLOTADJ_RIGHT_Val  << I2S_TXCTRL_SLOTADJ_Pos)
#define I2S_TXCTRL_SLOTADJ_LEFT     (I2S_TXCTRL_SLOTADJ_LEFT_Val   << I2S_TXCTRL_SLOTADJ_Pos)
#define I2S_TXCTRL_DATASIZE_Pos     8            /**< \brief (I2S_TXCTRL) Data Word Size */
#define I2S_TXCTRL_DATASIZE_Msk     (_U_(0x7) << I2S_TXCTRL_DATASIZE_Pos)
#define I2S_TXCTRL_DATASIZE(value)  (I2S_TXCTRL_DATASIZE_Msk & ((value) << I2S_TXCTRL_DATASIZE_Pos))
#define   I2S_TXCTRL_DATASIZE_32_Val      _U_(0x0)   /**< \brief (I2S_TXCTRL) 32 bits */
#define   I2S_TXCTRL_DATASIZE_24_Val      _U_(0x1)   /**< \brief (I2S_TXCTRL) 24 bits */
#define   I2S_TXCTRL_DATASIZE_20_Val      _U_(0x2)   /**< \brief (I2S_TXCTRL) 20 bits */
#define   I2S_TXCTRL_DATASIZE_18_Val      _U_(0x3)   /**< \brief (I2S_TXCTRL) 18 bits */
#define   I2S_TXCTRL_DATASIZE_16_Val      _U_(0x4)   /**< \brief (I2S_TXCTRL) 16 bits */
#define   I2S_TXCTRL_DATASIZE_16C_Val     _U_(0x5)   /**< \brief (I2S_TXCTRL) 16 bits compact stereo */
#define   I2S_TXCTRL_DATASIZE_8_Val       _U_(0x6)   /**< \brief (I2S_TXCTRL) 8 bits */
#define   I2S_TXCTRL_DATASIZE_8C_Val      _U_(0x7)   /**< \brief (I2S_TXCTRL) 8 bits compact stereo */
#define I2S_TXCTRL_DATASIZE_32      (I2S_TXCTRL_DATASIZE_32_Val    << I2S_TXCTRL_DATASIZE_Pos)
#define I2S_TXCTRL_DATASIZE_24      (I2S_TXCTRL_DATASIZE_24_Val    << I2S_TXCTRL_DATASIZE_Pos)
#define I2S_TXCTRL_DATASIZE_20      (I2S_TXCTRL_DATASIZE_20_Val    << I2S_TXCTRL_DATASIZE_Pos)
#define I2S_TXCTRL_DATASIZE_18      (I2S_TXCTRL_DATASIZE_18_Val    << I2S_TXCTRL_DATASIZE_Pos)
#define I2S_TXCTRL_DATASIZE_16      (I2S_TXCTRL_DATASIZE_16_Val    << I2S_TXCTRL_DATASIZE_Pos)
#define I2S_TXCTRL_DATASIZE_16C     (I2S_TXCTRL_DATASIZE_16C_Val   << I2S_TXCTRL_DATASIZE_Pos)
#define I2S_TXCTRL_DATASIZE_8       (I2S_TXCTRL_DATASIZE_8_Val     << I2S_TXCTRL_DATASIZE_Pos)
#define I2S_TXCTRL_DATASIZE_8C      (I2S_TXCTRL_DATASIZE_8C_Val    << I2S_TXCTRL_DATASIZE_Pos)
#define I2S_TXCTRL_WORDADJ_Pos      12           /**< \brief (I2S_TXCTRL) Data Word Formatting Adjust */
#define I2S_TXCTRL_WORDADJ          (_U_(0x1) << I2S_TXCTRL_WORDADJ_Pos)
#define   I2S_TXCTRL_WORDADJ_RIGHT_Val    _U_(0x0)   /**< \brief (I2S_TXCTRL) Data is right adjusted in word */
#define   I2S_TXCTRL_WORDADJ_LEFT_Val     _U_(0x1)   /**< \brief (I2S_TXCTRL) Data is left adjusted in word */
#define I2S_TXCTRL_WORDADJ_RIGHT    (I2S_TXCTRL_WORDADJ_RIGHT_Val  << I2S_TXCTRL_WORDADJ_Pos)
#define I2S_TXCTRL_WORDADJ_LEFT     (I2S_TXCTRL_WORDADJ_LEFT_Val   << I2S_TXCTRL_WORDADJ_Pos)
#define I2S_TXCTRL_EXTEND_Pos       13           /**< \brief (I2S_TXCTRL) Data Formatting Bit Extension */
#define I2S_TXCTRL_EXTEND_Msk       (_U_(0x3) << I2S_TXCTRL_EXTEND_Pos)
#define I2S_TXCTRL_EXTEND(value)    (I2S_TXCTRL_EXTEND_Msk & ((value) << I2S_TXCTRL_EXTEND_Pos))
#define   I2S_TXCTRL_EXTEND_ZERO_Val      _U_(0x0)   /**< \brief (I2S_TXCTRL) Extend with zeroes */
#define   I2S_TXCTRL_EXTEND_ONE_Val       _U_(0x1)   /**< \brief (I2S_TXCTRL) Extend with ones */
#define   I2S_TXCTRL_EXTEND_MSBIT_Val     _U_(0x2)   /**< \brief (I2S_TXCTRL) Extend with Most Significant Bit */
#define   I2S_TXCTRL_EXTEND_LSBIT_Val     _U_(0x3)   /**< \brief (I2S_TXCTRL) Extend with Least Significant Bit */
#define I2S_TXCTRL_EXTEND_ZERO      (I2S_TXCTRL_EXTEND_ZERO_Val    << I2S_TXCTRL_EXTEND_Pos)
#define I2S_TXCTRL_EXTEND_ONE       (I2S_TXCTRL_EXTEND_ONE_Val     << I2S_TXCTRL_EXTEND_Pos)
#define I2S_TXCTRL_EXTEND_MSBIT     (I2S_TXCTRL_EXTEND_MSBIT_Val   << I2S_TXCTRL_EXTEND_Pos)
#define I2S_TXCTRL_EXTEND_LSBIT     (I2S_TXCTRL_EXTEND_LSBIT_Val   << I2S_TXCTRL_EXTEND_Pos)
#define I2S_TXCTRL_BITREV_Pos       15           /**< \brief (I2S_TXCTRL) Data Formatting Bit Reverse */
#define I2S_TXCTRL_BITREV           (_U_(0x1) << I2S_TXCTRL_BITREV_Pos)
#define   I2S_TXCTRL_BITREV_MSBIT_Val     _U_(0x0)   /**< \brief (I2S_TXCTRL) Transfer Data Most Significant Bit (MSB) first (default for I2S protocol) */
#define   I2S_TXCTRL_BITREV_LSBIT_Val     _U_(0x1)   /**< \brief (I2S_TXCTRL) Transfer Data Least Significant Bit (LSB) first */
#define I2S_TXCTRL_BITREV_MSBIT     (I2S_TXCTRL_BITREV_MSBIT_Val   << I2S_TXCTRL_BITREV_Pos)
#define I2S_TXCTRL_BITREV_LSBIT     (I2S_TXCTRL_BITREV_LSBIT_Val   << I2S_TXCTRL_BITREV_Pos)
#define I2S_TXCTRL_SLOTDIS0_Pos     16           /**< \brief (I2S_TXCTRL) Slot 0 Disabled for this Serializer */
#define I2S_TXCTRL_SLOTDIS0         (_U_(1) << I2S_TXCTRL_SLOTDIS0_Pos)
#define I2S_TXCTRL_SLOTDIS1_Pos     17           /**< \brief (I2S_TXCTRL) Slot 1 Disabled for this Serializer */
#define I2S_TXCTRL_SLOTDIS1         (_U_(1) << I2S_TXCTRL_SLOTDIS1_Pos)
#define I2S_TXCTRL_SLOTDIS2_Pos     18           /**< \brief (I2S_TXCTRL) Slot 2 Disabled for this Serializer */
#define I2S_TXCTRL_SLOTDIS2         (_U_(1) << I2S_TXCTRL_SLOTDIS2_Pos)
#define I2S_TXCTRL_SLOTDIS3_Pos     19           /**< \brief (I2S_TXCTRL) Slot 3 Disabled for this Serializer */
#define I2S_TXCTRL_SLOTDIS3         (_U_(1) << I2S_TXCTRL_SLOTDIS3_Pos)
#define I2S_TXCTRL_SLOTDIS4_Pos     20           /**< \brief (I2S_TXCTRL) Slot 4 Disabled for this Serializer */
#define I2S_TXCTRL_SLOTDIS4         (_U_(1) << I2S_TXCTRL_SLOTDIS4_Pos)
#define I2S_TXCTRL_SLOTDIS5_Pos     21           /**< \brief (I2S_TXCTRL) Slot 5 Disabled for this Serializer */
#define I2S_TXCTRL_SLOTDIS5         (_U_(1) << I2S_TXCTRL_SLOTDIS5_Pos)
#define I2S_TXCTRL_SLOTDIS6_Pos     22           /**< \brief (I2S_TXCTRL) Slot 6 Disabled for this Serializer */
#define I2S_TXCTRL_SLOTDIS6         (_U_(1) << I2S_TXCTRL_SLOTDIS6_Pos)
#define I2S_TXCTRL_SLOTDIS7_Pos     23           /**< \brief (I2S_TXCTRL) Slot 7 Disabled for this Serializer */
#define I2S_TXCTRL_SLOTDIS7         (_U_(1) << I2S_TXCTRL_SLOTDIS7_Pos)
#define I2S_TXCTRL_SLOTDIS_Pos      16           /**< \brief (I2S_TXCTRL) Slot x Disabled for this Serializer */
#define I2S_TXCTRL_SLOTDIS_Msk      (_U_(0xFF) << I2S_TXCTRL_SLOTDIS_Pos)
#define I2S_TXCTRL_SLOTDIS(value)   (I2S_TXCTRL_SLOTDIS_Msk & ((value) << I2S_TXCTRL_SLOTDIS_Pos))
#define I2S_TXCTRL_MONO_Pos         24           /**< \brief (I2S_TXCTRL) Mono Mode */
#define I2S_TXCTRL_MONO             (_U_(0x1) << I2S_TXCTRL_MONO_Pos)
#define   I2S_TXCTRL_MONO_STEREO_Val      _U_(0x0)   /**< \brief (I2S_TXCTRL) Normal mode */
#define   I2S_TXCTRL_MONO_MONO_Val        _U_(0x1)   /**< \brief (I2S_TXCTRL) Left channel data is duplicated to right channel */
#define I2S_TXCTRL_MONO_STEREO      (I2S_TXCTRL_MONO_STEREO_Val    << I2S_TXCTRL_MONO_Pos)
#define I2S_TXCTRL_MONO_MONO        (I2S_TXCTRL_MONO_MONO_Val      << I2S_TXCTRL_MONO_Pos)
#define I2S_TXCTRL_DMA_Pos          25           /**< \brief (I2S_TXCTRL) Single or Multiple DMA Channels */
#define I2S_TXCTRL_DMA              (_U_(0x1) << I2S_TXCTRL_DMA_Pos)
#define   I2S_TXCTRL_DMA_SINGLE_Val       _U_(0x0)   /**< \brief (I2S_TXCTRL) Single DMA channel */
#define   I2S_TXCTRL_DMA_MULTIPLE_Val     _U_(0x1)   /**< \brief (I2S_TXCTRL) One DMA channel per data channel */
#define I2S_TXCTRL_DMA_SINGLE       (I2S_TXCTRL_DMA_SINGLE_Val     << I2S_TXCTRL_DMA_Pos)
#define I2S_TXCTRL_DMA_MULTIPLE     (I2S_TXCTRL_DMA_MULTIPLE_Val   << I2S_TXCTRL_DMA_Pos)
#define I2S_TXCTRL_MASK             _U_(0x03FFF79C) /**< \brief (I2S_TXCTRL) MASK Register */

/* -------- I2S_RXCTRL : (I2S Offset: 0x24) (R/W 32) Rx Serializer Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SERMODE:2;        /*!< bit:  0.. 1  Serializer Mode                    */
    uint32_t :3;               /*!< bit:  2.. 4  Reserved                           */
    uint32_t CLKSEL:1;         /*!< bit:      5  Clock Unit Selection               */
    uint32_t :1;               /*!< bit:      6  Reserved                           */
    uint32_t SLOTADJ:1;        /*!< bit:      7  Data Slot Formatting Adjust        */
    uint32_t DATASIZE:3;       /*!< bit:  8..10  Data Word Size                     */
    uint32_t :1;               /*!< bit:     11  Reserved                           */
    uint32_t WORDADJ:1;        /*!< bit:     12  Data Word Formatting Adjust        */
    uint32_t EXTEND:2;         /*!< bit: 13..14  Data Formatting Bit Extension      */
    uint32_t BITREV:1;         /*!< bit:     15  Data Formatting Bit Reverse        */
    uint32_t SLOTDIS0:1;       /*!< bit:     16  Slot 0 Disabled for this Serializer */
    uint32_t SLOTDIS1:1;       /*!< bit:     17  Slot 1 Disabled for this Serializer */
    uint32_t SLOTDIS2:1;       /*!< bit:     18  Slot 2 Disabled for this Serializer */
    uint32_t SLOTDIS3:1;       /*!< bit:     19  Slot 3 Disabled for this Serializer */
    uint32_t SLOTDIS4:1;       /*!< bit:     20  Slot 4 Disabled for this Serializer */
    uint32_t SLOTDIS5:1;       /*!< bit:     21  Slot 5 Disabled for this Serializer */
    uint32_t SLOTDIS6:1;       /*!< bit:     22  Slot 6 Disabled for this Serializer */
    uint32_t SLOTDIS7:1;       /*!< bit:     23  Slot 7 Disabled for this Serializer */
    uint32_t MONO:1;           /*!< bit:     24  Mono Mode                          */
    uint32_t DMA:1;            /*!< bit:     25  Single or Multiple DMA Channels    */
    uint32_t RXLOOP:1;         /*!< bit:     26  Loop-back Test Mode                */
    uint32_t :5;               /*!< bit: 27..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :16;              /*!< bit:  0..15  Reserved                           */
    uint32_t SLOTDIS:8;        /*!< bit: 16..23  Slot x Disabled for this Serializer */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} I2S_RXCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define I2S_RXCTRL_OFFSET           0x24         /**< \brief (I2S_RXCTRL offset) Rx Serializer Control */
#define I2S_RXCTRL_RESETVALUE       _U_(0x00000000) /**< \brief (I2S_RXCTRL reset_value) Rx Serializer Control */

#define I2S_RXCTRL_SERMODE_Pos      0            /**< \brief (I2S_RXCTRL) Serializer Mode */
#define I2S_RXCTRL_SERMODE_Msk      (_U_(0x3) << I2S_RXCTRL_SERMODE_Pos)
#define I2S_RXCTRL_SERMODE(value)   (I2S_RXCTRL_SERMODE_Msk & ((value) << I2S_RXCTRL_SERMODE_Pos))
#define   I2S_RXCTRL_SERMODE_RX_Val       _U_(0x0)   /**< \brief (I2S_RXCTRL) Receive */
#define   I2S_RXCTRL_SERMODE_PDM2_Val     _U_(0x2)   /**< \brief (I2S_RXCTRL) Receive one PDM data on each serial clock edge */
#define I2S_RXCTRL_SERMODE_RX       (I2S_RXCTRL_SERMODE_RX_Val     << I2S_RXCTRL_SERMODE_Pos)
#define I2S_RXCTRL_SERMODE_PDM2     (I2S_RXCTRL_SERMODE_PDM2_Val   << I2S_RXCTRL_SERMODE_Pos)
#define I2S_RXCTRL_CLKSEL_Pos       5            /**< \brief (I2S_RXCTRL) Clock Unit Selection */
#define I2S_RXCTRL_CLKSEL           (_U_(0x1) << I2S_RXCTRL_CLKSEL_Pos)
#define   I2S_RXCTRL_CLKSEL_CLK0_Val      _U_(0x0)   /**< \brief (I2S_RXCTRL) Use Clock Unit 0 */
#define   I2S_RXCTRL_CLKSEL_CLK1_Val      _U_(0x1)   /**< \brief (I2S_RXCTRL) Use Clock Unit 1 */
#define I2S_RXCTRL_CLKSEL_CLK0      (I2S_RXCTRL_CLKSEL_CLK0_Val    << I2S_RXCTRL_CLKSEL_Pos)
#define I2S_RXCTRL_CLKSEL_CLK1      (I2S_RXCTRL_CLKSEL_CLK1_Val    << I2S_RXCTRL_CLKSEL_Pos)
#define I2S_RXCTRL_SLOTADJ_Pos      7            /**< \brief (I2S_RXCTRL) Data Slot Formatting Adjust */
#define I2S_RXCTRL_SLOTADJ          (_U_(0x1) << I2S_RXCTRL_SLOTADJ_Pos)
#define   I2S_RXCTRL_SLOTADJ_RIGHT_Val    _U_(0x0)   /**< \brief (I2S_RXCTRL) Data is right adjusted in slot */
#define   I2S_RXCTRL_SLOTADJ_LEFT_Val     _U_(0x1)   /**< \brief (I2S_RXCTRL) Data is left adjusted in slot */
#define I2S_RXCTRL_SLOTADJ_RIGHT    (I2S_RXCTRL_SLOTADJ_RIGHT_Val  << I2S_RXCTRL_SLOTADJ_Pos)
#define I2S_RXCTRL_SLOTADJ_LEFT     (I2S_RXCTRL_SLOTADJ_LEFT_Val   << I2S_RXCTRL_SLOTADJ_Pos)
#define I2S_RXCTRL_DATASIZE_Pos     8            /**< \brief (I2S_RXCTRL) Data Word Size */
#define I2S_RXCTRL_DATASIZE_Msk     (_U_(0x7) << I2S_RXCTRL_DATASIZE_Pos)
#define I2S_RXCTRL_DATASIZE(value)  (I2S_RXCTRL_DATASIZE_Msk & ((value) << I2S_RXCTRL_DATASIZE_Pos))
#define   I2S_RXCTRL_DATASIZE_32_Val      _U_(0x0)   /**< \brief (I2S_RXCTRL) 32 bits */
#define   I2S_RXCTRL_DATASIZE_24_Val      _U_(0x1)   /**< \brief (I2S_RXCTRL) 24 bits */
#define   I2S_RXCTRL_DATASIZE_20_Val      _U_(0x2)   /**< \brief (I2S_RXCTRL) 20 bits */
#define   I2S_RXCTRL_DATASIZE_18_Val      _U_(0x3)   /**< \brief (I2S_RXCTRL) 18 bits */
#define   I2S_RXCTRL_DATASIZE_16_Val      _U_(0x4)   /**< \brief (I2S_RXCTRL) 16 bits */
#define   I2S_RXCTRL_DATASIZE_16C_Val     _U_(0x5)   /**< \brief (I2S_RXCTRL) 16 bits compact stereo */
#define   I2S_RXCTRL_DATASIZE_8_Val       _U_(0x6)   /**< \brief (I2S_RXCTRL) 8 bits */
#define   I2S_RXCTRL_DATASIZE_8C_Val      _U_(0x7)   /**< \brief (I2S_RXCTRL) 8 bits compact stereo */
#define I2S_RXCTRL_DATASIZE_32      (I2S_RXCTRL_DATASIZE_32_Val    << I2S_RXCTRL_DATASIZE_Pos)
#define I2S_RXCTRL_DATASIZE_24      (I2S_RXCTRL_DATASIZE_24_Val    << I2S_RXCTRL_DATASIZE_Pos)
#define I2S_RXCTRL_DATASIZE_20      (I2S_RXCTRL_DATASIZE_20_Val    << I2S_RXCTRL_DATASIZE_Pos)
#define I2S_RXCTRL_DATASIZE_18      (I2S_RXCTRL_DATASIZE_18_Val    << I2S_RXCTRL_DATASIZE_Pos)
#define I2S_RXCTRL_DATASIZE_16      (I2S_RXCTRL_DATASIZE_16_Val    << I2S_RXCTRL_DATASIZE_Pos)
#define I2S_RXCTRL_DATASIZE_16C     (I2S_RXCTRL_DATASIZE_16C_Val   << I2S_RXCTRL_DATASIZE_Pos)
#define I2S_RXCTRL_DATASIZE_8       (I2S_RXCTRL_DATASIZE_8_Val     << I2S_RXCTRL_DATASIZE_Pos)
#define I2S_RXCTRL_DATASIZE_8C      (I2S_RXCTRL_DATASIZE_8C_Val    << I2S_RXCTRL_DATASIZE_Pos)
#define I2S_RXCTRL_WORDADJ_Pos      12           /**< \brief (I2S_RXCTRL) Data Word Formatting Adjust */
#define I2S_RXCTRL_WORDADJ          (_U_(0x1) << I2S_RXCTRL_WORDADJ_Pos)
#define   I2S_RXCTRL_WORDADJ_RIGHT_Val    _U_(0x0)   /**< \brief (I2S_RXCTRL) Data is right adjusted in word */
#define   I2S_RXCTRL_WORDADJ_LEFT_Val     _U_(0x1)   /**< \brief (I2S_RXCTRL) Data is left adjusted in word */
#define I2S_RXCTRL_WORDADJ_RIGHT    (I2S_RXCTRL_WORDADJ_RIGHT_Val  << I2S_RXCTRL_WORDADJ_Pos)
#define I2S_RXCTRL_WORDADJ_LEFT     (I2S_RXCTRL_WORDADJ_LEFT_Val   << I2S_RXCTRL_WORDADJ_Pos)
#define I2S_RXCTRL_EXTEND_Pos       13           /**< \brief (I2S_RXCTRL) Data Formatting Bit Extension */
#define I2S_RXCTRL_EXTEND_Msk       (_U_(0x3) << I2S_RXCTRL_EXTEND_Pos)
#define I2S_RXCTRL_EXTEND(value)    (I2S_RXCTRL_EXTEND_Msk & ((value) << I2S_RXCTRL_EXTEND_Pos))
#define   I2S_RXCTRL_EXTEND_ZERO_Val      _U_(0x0)   /**< \brief (I2S_RXCTRL) Extend with zeroes */
#define   I2S_RXCTRL_EXTEND_ONE_Val       _U_(0x1)   /**< \brief (I2S_RXCTRL) Extend with ones */
#define   I2S_RXCTRL_EXTEND_MSBIT_Val     _U_(0x2)   /**< \brief (I2S_RXCTRL) Extend with Most Significant Bit */
#define   I2S_RXCTRL_EXTEND_LSBIT_Val     _U_(0x3)   /**< \brief (I2S_RXCTRL) Extend with Least Significant Bit */
#define I2S_RXCTRL_EXTEND_ZERO      (I2S_RXCTRL_EXTEND_ZERO_Val    << I2S_RXCTRL_EXTEND_Pos)
#define I2S_RXCTRL_EXTEND_ONE       (I2S_RXCTRL_EXTEND_ONE_Val     << I2S_RXCTRL_EXTEND_Pos)
#define I2S_RXCTRL_EXTEND_MSBIT     (I2S_RXCTRL_EXTEND_MSBIT_Val   << I2S_RXCTRL_EXTEND_Pos)
#define I2S_RXCTRL_EXTEND_LSBIT     (I2S_RXCTRL_EXTEND_LSBIT_Val   << I2S_RXCTRL_EXTEND_Pos)
#define I2S_RXCTRL_BITREV_Pos       15           /**< \brief (I2S_RXCTRL) Data Formatting Bit Reverse */
#define I2S_RXCTRL_BITREV           (_U_(0x1) << I2S_RXCTRL_BITREV_Pos)
#define   I2S_RXCTRL_BITREV_MSBIT_Val     _U_(0x0)   /**< \brief (I2S_RXCTRL) Transfer Data Most Significant Bit (MSB) first (default for I2S protocol) */
#define   I2S_RXCTRL_BITREV_LSBIT_Val     _U_(0x1)   /**< \brief (I2S_RXCTRL) Transfer Data Least Significant Bit (LSB) first */
#define I2S_RXCTRL_BITREV_MSBIT     (I2S_RXCTRL_BITREV_MSBIT_Val   << I2S_RXCTRL_BITREV_Pos)
#define I2S_RXCTRL_BITREV_LSBIT     (I2S_RXCTRL_BITREV_LSBIT_Val   << I2S_RXCTRL_BITREV_Pos)
#define I2S_RXCTRL_SLOTDIS0_Pos     16           /**< \brief (I2S_RXCTRL) Slot 0 Disabled for this Serializer */
#define I2S_RXCTRL_SLOTDIS0         (_U_(1) << I2S_RXCTRL_SLOTDIS0_Pos)
#define I2S_RXCTRL_SLOTDIS1_Pos     17           /**< \brief (I2S_RXCTRL) Slot 1 Disabled for this Serializer */
#define I2S_RXCTRL_SLOTDIS1         (_U_(1) << I2S_RXCTRL_SLOTDIS1_Pos)
#define I2S_RXCTRL_SLOTDIS2_Pos     18           /**< \brief (I2S_RXCTRL) Slot 2 Disabled for this Serializer */
#define I2S_RXCTRL_SLOTDIS2         (_U_(1) << I2S_RXCTRL_SLOTDIS2_Pos)
#define I2S_RXCTRL_SLOTDIS3_Pos     19           /**< \brief (I2S_RXCTRL) Slot 3 Disabled for this Serializer */
#define I2S_RXCTRL_SLOTDIS3         (_U_(1) << I2S_RXCTRL_SLOTDIS3_Pos)
#define I2S_RXCTRL_SLOTDIS4_Pos     20           /**< \brief (I2S_RXCTRL) Slot 4 Disabled for this Serializer */
#define I2S_RXCTRL_SLOTDIS4         (_U_(1) << I2S_RXCTRL_SLOTDIS4_Pos)
#define I2S_RXCTRL_SLOTDIS5_Pos     21           /**< \brief (I2S_RXCTRL) Slot 5 Disabled for this Serializer */
#define I2S_RXCTRL_SLOTDIS5         (_U_(1) << I2S_RXCTRL_SLOTDIS5_Pos)
#define I2S_RXCTRL_SLOTDIS6_Pos     22           /**< \brief (I2S_RXCTRL) Slot 6 Disabled for this Serializer */
#define I2S_RXCTRL_SLOTDIS6         (_U_(1) << I2S_RXCTRL_SLOTDIS6_Pos)
#define I2S_RXCTRL_SLOTDIS7_Pos     23           /**< \brief (I2S_RXCTRL) Slot 7 Disabled for this Serializer */
#define I2S_RXCTRL_SLOTDIS7         (_U_(1) << I2S_RXCTRL_SLOTDIS7_Pos)
#define I2S_RXCTRL_SLOTDIS_Pos      16           /**< \brief (I2S_RXCTRL) Slot x Disabled for this Serializer */
#define I2S_RXCTRL_SLOTDIS_Msk      (_U_(0xFF) << I2S_RXCTRL_SLOTDIS_Pos)
#define I2S_RXCTRL_SLOTDIS(value)   (I2S_RXCTRL_SLOTDIS_Msk & ((value) << I2S_RXCTRL_SLOTDIS_Pos))
#define I2S_RXCTRL_MONO_Pos         24           /**< \brief (I2S_RXCTRL) Mono Mode */
#define I2S_RXCTRL_MONO             (_U_(0x1) << I2S_RXCTRL_MONO_Pos)
#define   I2S_RXCTRL_MONO_STEREO_Val      _U_(0x0)   /**< \brief (I2S_RXCTRL) Normal mode */
#define   I2S_RXCTRL_MONO_MONO_Val        _U_(0x1)   /**< \brief (I2S_RXCTRL) Left channel data is duplicated to right channel */
#define I2S_RXCTRL_MONO_STEREO      (I2S_RXCTRL_MONO_STEREO_Val    << I2S_RXCTRL_MONO_Pos)
#define I2S_RXCTRL_MONO_MONO        (I2S_RXCTRL_MONO_MONO_Val      << I2S_RXCTRL_MONO_Pos)
#define I2S_RXCTRL_DMA_Pos          25           /**< \brief (I2S_RXCTRL) Single or Multiple DMA Channels */
#define I2S_RXCTRL_DMA              (_U_(0x1) << I2S_RXCTRL_DMA_Pos)
#define   I2S_RXCTRL_DMA_SINGLE_Val       _U_(0x0)   /**< \brief (I2S_RXCTRL) Single DMA channel */
#define   I2S_RXCTRL_DMA_MULTIPLE_Val     _U_(0x1)   /**< \brief (I2S_RXCTRL) One DMA channel per data channel */
#define I2S_RXCTRL_DMA_SINGLE       (I2S_RXCTRL_DMA_SINGLE_Val     << I2S_RXCTRL_DMA_Pos)
#define I2S_RXCTRL_DMA_MULTIPLE     (I2S_RXCTRL_DMA_MULTIPLE_Val   << I2S_RXCTRL_DMA_Pos)
#define I2S_RXCTRL_RXLOOP_Pos       26           /**< \brief (I2S_RXCTRL) Loop-back Test Mode */
#define I2S_RXCTRL_RXLOOP           (_U_(0x1) << I2S_RXCTRL_RXLOOP_Pos)
#define I2S_RXCTRL_MASK             _U_(0x07FFF7A3) /**< \brief (I2S_RXCTRL) MASK Register */

/* -------- I2S_TXDATA : (I2S Offset: 0x30) ( /W 32) Tx Data -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DATA:32;          /*!< bit:  0..31  Sample Data                        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} I2S_TXDATA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define I2S_TXDATA_OFFSET           0x30         /**< \brief (I2S_TXDATA offset) Tx Data */
#define I2S_TXDATA_RESETVALUE       _U_(0x00000000) /**< \brief (I2S_TXDATA reset_value) Tx Data */

#define I2S_TXDATA_DATA_Pos         0            /**< \brief (I2S_TXDATA) Sample Data */
#define I2S_TXDATA_DATA_Msk         (_U_(0xFFFFFFFF) << I2S_TXDATA_DATA_Pos)
#define I2S_TXDATA_DATA(value)      (I2S_TXDATA_DATA_Msk & ((value) << I2S_TXDATA_DATA_Pos))
#define I2S_TXDATA_MASK             _U_(0xFFFFFFFF) /**< \brief (I2S_TXDATA) MASK Register */

/* -------- I2S_RXDATA : (I2S Offset: 0x34) (R/  32) Rx Data -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DATA:32;          /*!< bit:  0..31  Sample Data                        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} I2S_RXDATA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define I2S_RXDATA_OFFSET           0x34         /**< \brief (I2S_RXDATA offset) Rx Data */
#define I2S_RXDATA_RESETVALUE       _U_(0x00000000) /**< \brief (I2S_RXDATA reset_value) Rx Data */

#define I2S_RXDATA_DATA_Pos         0            /**< \brief (I2S_RXDATA) Sample Data */
#define I2S_RXDATA_DATA_Msk         (_U_(0xFFFFFFFF) << I2S_RXDATA_DATA_Pos)
#define I2S_RXDATA_DATA(value)      (I2S_RXDATA_DATA_Msk & ((value) << I2S_RXDATA_DATA_Pos))
#define I2S_RXDATA_MASK             _U_(0xFFFFFFFF) /**< \brief (I2S_RXDATA) MASK Register */

/** \brief I2S hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO I2S_CTRLA_Type            CTRLA;       /**< \brief Offset: 0x00 (R/W  8) Control A */
       RoReg8                    Reserved1[0x3];
  __IO I2S_CLKCTRL_Type          CLKCTRL[2];  /**< \brief Offset: 0x04 (R/W 32) Clock Unit n Control */
  __IO I2S_INTENCLR_Type         INTENCLR;    /**< \brief Offset: 0x0C (R/W 16) Interrupt Enable Clear */
       RoReg8                    Reserved2[0x2];
  __IO I2S_INTENSET_Type         INTENSET;    /**< \brief Offset: 0x10 (R/W 16) Interrupt Enable Set */
       RoReg8                    Reserved3[0x2];
  __IO I2S_INTFLAG_Type          INTFLAG;     /**< \brief Offset: 0x14 (R/W 16) Interrupt Flag Status and Clear */
       RoReg8                    Reserved4[0x2];
  __I  I2S_SYNCBUSY_Type         SYNCBUSY;    /**< \brief Offset: 0x18 (R/  16) Synchronization Status */
       RoReg8                    Reserved5[0x6];
  __IO I2S_TXCTRL_Type           TXCTRL;      /**< \brief Offset: 0x20 (R/W 32) Tx Serializer Control */
  __IO I2S_RXCTRL_Type           RXCTRL;      /**< \brief Offset: 0x24 (R/W 32) Rx Serializer Control */
       RoReg8                    Reserved6[0x8];
  __O  I2S_TXDATA_Type           TXDATA;      /**< \brief Offset: 0x30 ( /W 32) Tx Data */
  __I  I2S_RXDATA_Type           RXDATA;      /**< \brief Offset: 0x34 (R/  32) Rx Data */
} I2s;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_I2S_COMPONENT_ */
