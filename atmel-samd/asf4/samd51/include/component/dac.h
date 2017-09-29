/**
 * \file
 *
 * \brief Component description for DAC
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

#ifndef _SAMD51_DAC_COMPONENT_
#define _SAMD51_DAC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR DAC */
/* ========================================================================== */
/** \addtogroup SAMD51_DAC Digital-to-Analog Converter */
/*@{*/

#define DAC_U2502
#define REV_DAC                     0x100

/* -------- DAC_CTRLA : (DAC Offset: 0x00) (R/W  8) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable DAC Controller              */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_CTRLA_OFFSET            0x00         /**< \brief (DAC_CTRLA offset) Control A */
#define DAC_CTRLA_RESETVALUE        _U_(0x00)    /**< \brief (DAC_CTRLA reset_value) Control A */

#define DAC_CTRLA_SWRST_Pos         0            /**< \brief (DAC_CTRLA) Software Reset */
#define DAC_CTRLA_SWRST             (_U_(0x1) << DAC_CTRLA_SWRST_Pos)
#define DAC_CTRLA_ENABLE_Pos        1            /**< \brief (DAC_CTRLA) Enable DAC Controller */
#define DAC_CTRLA_ENABLE            (_U_(0x1) << DAC_CTRLA_ENABLE_Pos)
#define DAC_CTRLA_MASK              _U_(0x03)    /**< \brief (DAC_CTRLA) MASK Register */

/* -------- DAC_CTRLB : (DAC Offset: 0x01) (R/W  8) Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DIFF:1;           /*!< bit:      0  Differential mode enable           */
    uint8_t  REFSEL:2;         /*!< bit:  1.. 2  Reference Selection for DAC0/1     */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_CTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_CTRLB_OFFSET            0x01         /**< \brief (DAC_CTRLB offset) Control B */
#define DAC_CTRLB_RESETVALUE        _U_(0x02)    /**< \brief (DAC_CTRLB reset_value) Control B */

#define DAC_CTRLB_DIFF_Pos          0            /**< \brief (DAC_CTRLB) Differential mode enable */
#define DAC_CTRLB_DIFF              (_U_(0x1) << DAC_CTRLB_DIFF_Pos)
#define DAC_CTRLB_REFSEL_Pos        1            /**< \brief (DAC_CTRLB) Reference Selection for DAC0/1 */
#define DAC_CTRLB_REFSEL_Msk        (_U_(0x3) << DAC_CTRLB_REFSEL_Pos)
#define DAC_CTRLB_REFSEL(value)     (DAC_CTRLB_REFSEL_Msk & ((value) << DAC_CTRLB_REFSEL_Pos))
#define   DAC_CTRLB_REFSEL_VREFPU_Val     _U_(0x0)   /**< \brief (DAC_CTRLB) External reference unbuffered */
#define   DAC_CTRLB_REFSEL_VDDANA_Val     _U_(0x1)   /**< \brief (DAC_CTRLB) Analog supply */
#define   DAC_CTRLB_REFSEL_VREFPB_Val     _U_(0x2)   /**< \brief (DAC_CTRLB) External reference buffered */
#define   DAC_CTRLB_REFSEL_INTREF_Val     _U_(0x3)   /**< \brief (DAC_CTRLB) Internal bandgap reference */
#define DAC_CTRLB_REFSEL_VREFPU     (DAC_CTRLB_REFSEL_VREFPU_Val   << DAC_CTRLB_REFSEL_Pos)
#define DAC_CTRLB_REFSEL_VDDANA     (DAC_CTRLB_REFSEL_VDDANA_Val   << DAC_CTRLB_REFSEL_Pos)
#define DAC_CTRLB_REFSEL_VREFPB     (DAC_CTRLB_REFSEL_VREFPB_Val   << DAC_CTRLB_REFSEL_Pos)
#define DAC_CTRLB_REFSEL_INTREF     (DAC_CTRLB_REFSEL_INTREF_Val   << DAC_CTRLB_REFSEL_Pos)
#define DAC_CTRLB_MASK              _U_(0x07)    /**< \brief (DAC_CTRLB) MASK Register */

/* -------- DAC_EVCTRL : (DAC Offset: 0x02) (R/W  8) Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  STARTEI0:1;       /*!< bit:      0  Start Conversion Event Input DAC 0 */
    uint8_t  STARTEI1:1;       /*!< bit:      1  Start Conversion Event Input DAC 1 */
    uint8_t  EMPTYEO0:1;       /*!< bit:      2  Data Buffer Empty Event Output DAC 0 */
    uint8_t  EMPTYEO1:1;       /*!< bit:      3  Data Buffer Empty Event Output DAC 1 */
    uint8_t  INVEI0:1;         /*!< bit:      4  Enable Invertion of DAC 0 input event */
    uint8_t  INVEI1:1;         /*!< bit:      5  Enable Invertion of DAC 1 input event */
    uint8_t  RESRDYEO0:1;      /*!< bit:      6  Result Ready Event Output 0        */
    uint8_t  RESRDYEO1:1;      /*!< bit:      7  Result Ready Event Output 1        */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  STARTEI:2;        /*!< bit:  0.. 1  Start Conversion Event Input DAC x */
    uint8_t  EMPTYEO:2;        /*!< bit:  2.. 3  Data Buffer Empty Event Output DAC x */
    uint8_t  INVEI:2;          /*!< bit:  4.. 5  Enable Invertion of DAC x input event */
    uint8_t  RESRDYEO:2;       /*!< bit:  6.. 7  Result Ready Event Output x        */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_EVCTRL_OFFSET           0x02         /**< \brief (DAC_EVCTRL offset) Event Control */
#define DAC_EVCTRL_RESETVALUE       _U_(0x00)    /**< \brief (DAC_EVCTRL reset_value) Event Control */

#define DAC_EVCTRL_STARTEI0_Pos     0            /**< \brief (DAC_EVCTRL) Start Conversion Event Input DAC 0 */
#define DAC_EVCTRL_STARTEI0         (_U_(1) << DAC_EVCTRL_STARTEI0_Pos)
#define DAC_EVCTRL_STARTEI1_Pos     1            /**< \brief (DAC_EVCTRL) Start Conversion Event Input DAC 1 */
#define DAC_EVCTRL_STARTEI1         (_U_(1) << DAC_EVCTRL_STARTEI1_Pos)
#define DAC_EVCTRL_STARTEI_Pos      0            /**< \brief (DAC_EVCTRL) Start Conversion Event Input DAC x */
#define DAC_EVCTRL_STARTEI_Msk      (_U_(0x3) << DAC_EVCTRL_STARTEI_Pos)
#define DAC_EVCTRL_STARTEI(value)   (DAC_EVCTRL_STARTEI_Msk & ((value) << DAC_EVCTRL_STARTEI_Pos))
#define DAC_EVCTRL_EMPTYEO0_Pos     2            /**< \brief (DAC_EVCTRL) Data Buffer Empty Event Output DAC 0 */
#define DAC_EVCTRL_EMPTYEO0         (_U_(1) << DAC_EVCTRL_EMPTYEO0_Pos)
#define DAC_EVCTRL_EMPTYEO1_Pos     3            /**< \brief (DAC_EVCTRL) Data Buffer Empty Event Output DAC 1 */
#define DAC_EVCTRL_EMPTYEO1         (_U_(1) << DAC_EVCTRL_EMPTYEO1_Pos)
#define DAC_EVCTRL_EMPTYEO_Pos      2            /**< \brief (DAC_EVCTRL) Data Buffer Empty Event Output DAC x */
#define DAC_EVCTRL_EMPTYEO_Msk      (_U_(0x3) << DAC_EVCTRL_EMPTYEO_Pos)
#define DAC_EVCTRL_EMPTYEO(value)   (DAC_EVCTRL_EMPTYEO_Msk & ((value) << DAC_EVCTRL_EMPTYEO_Pos))
#define DAC_EVCTRL_INVEI0_Pos       4            /**< \brief (DAC_EVCTRL) Enable Invertion of DAC 0 input event */
#define DAC_EVCTRL_INVEI0           (_U_(1) << DAC_EVCTRL_INVEI0_Pos)
#define DAC_EVCTRL_INVEI1_Pos       5            /**< \brief (DAC_EVCTRL) Enable Invertion of DAC 1 input event */
#define DAC_EVCTRL_INVEI1           (_U_(1) << DAC_EVCTRL_INVEI1_Pos)
#define DAC_EVCTRL_INVEI_Pos        4            /**< \brief (DAC_EVCTRL) Enable Invertion of DAC x input event */
#define DAC_EVCTRL_INVEI_Msk        (_U_(0x3) << DAC_EVCTRL_INVEI_Pos)
#define DAC_EVCTRL_INVEI(value)     (DAC_EVCTRL_INVEI_Msk & ((value) << DAC_EVCTRL_INVEI_Pos))
#define DAC_EVCTRL_RESRDYEO0_Pos    6            /**< \brief (DAC_EVCTRL) Result Ready Event Output 0 */
#define DAC_EVCTRL_RESRDYEO0        (_U_(1) << DAC_EVCTRL_RESRDYEO0_Pos)
#define DAC_EVCTRL_RESRDYEO1_Pos    7            /**< \brief (DAC_EVCTRL) Result Ready Event Output 1 */
#define DAC_EVCTRL_RESRDYEO1        (_U_(1) << DAC_EVCTRL_RESRDYEO1_Pos)
#define DAC_EVCTRL_RESRDYEO_Pos     6            /**< \brief (DAC_EVCTRL) Result Ready Event Output x */
#define DAC_EVCTRL_RESRDYEO_Msk     (_U_(0x3) << DAC_EVCTRL_RESRDYEO_Pos)
#define DAC_EVCTRL_RESRDYEO(value)  (DAC_EVCTRL_RESRDYEO_Msk & ((value) << DAC_EVCTRL_RESRDYEO_Pos))
#define DAC_EVCTRL_MASK             _U_(0xFF)    /**< \brief (DAC_EVCTRL) MASK Register */

/* -------- DAC_INTENCLR : (DAC Offset: 0x04) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  UNDERRUN0:1;      /*!< bit:      0  Underrun 0 Interrupt Enable        */
    uint8_t  UNDERRUN1:1;      /*!< bit:      1  Underrun 1 Interrupt Enable        */
    uint8_t  EMPTY0:1;         /*!< bit:      2  Data Buffer 0 Empty Interrupt Enable */
    uint8_t  EMPTY1:1;         /*!< bit:      3  Data Buffer 1 Empty Interrupt Enable */
    uint8_t  RESRDY0:1;        /*!< bit:      4  Result 0 Ready Interrupt Enable    */
    uint8_t  RESRDY1:1;        /*!< bit:      5  Result 1 Ready Interrupt Enable    */
    uint8_t  OVERRUN0:1;       /*!< bit:      6  Overrun 0 Interrupt Enable         */
    uint8_t  OVERRUN1:1;       /*!< bit:      7  Overrun 1 Interrupt Enable         */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  UNDERRUN:2;       /*!< bit:  0.. 1  Underrun x Interrupt Enable        */
    uint8_t  EMPTY:2;          /*!< bit:  2.. 3  Data Buffer x Empty Interrupt Enable */
    uint8_t  RESRDY:2;         /*!< bit:  4.. 5  Result x Ready Interrupt Enable    */
    uint8_t  OVERRUN:2;        /*!< bit:  6.. 7  Overrun x Interrupt Enable         */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_INTENCLR_OFFSET         0x04         /**< \brief (DAC_INTENCLR offset) Interrupt Enable Clear */
#define DAC_INTENCLR_RESETVALUE     _U_(0x00)    /**< \brief (DAC_INTENCLR reset_value) Interrupt Enable Clear */

#define DAC_INTENCLR_UNDERRUN0_Pos  0            /**< \brief (DAC_INTENCLR) Underrun 0 Interrupt Enable */
#define DAC_INTENCLR_UNDERRUN0      (_U_(1) << DAC_INTENCLR_UNDERRUN0_Pos)
#define DAC_INTENCLR_UNDERRUN1_Pos  1            /**< \brief (DAC_INTENCLR) Underrun 1 Interrupt Enable */
#define DAC_INTENCLR_UNDERRUN1      (_U_(1) << DAC_INTENCLR_UNDERRUN1_Pos)
#define DAC_INTENCLR_UNDERRUN_Pos   0            /**< \brief (DAC_INTENCLR) Underrun x Interrupt Enable */
#define DAC_INTENCLR_UNDERRUN_Msk   (_U_(0x3) << DAC_INTENCLR_UNDERRUN_Pos)
#define DAC_INTENCLR_UNDERRUN(value) (DAC_INTENCLR_UNDERRUN_Msk & ((value) << DAC_INTENCLR_UNDERRUN_Pos))
#define DAC_INTENCLR_EMPTY0_Pos     2            /**< \brief (DAC_INTENCLR) Data Buffer 0 Empty Interrupt Enable */
#define DAC_INTENCLR_EMPTY0         (_U_(1) << DAC_INTENCLR_EMPTY0_Pos)
#define DAC_INTENCLR_EMPTY1_Pos     3            /**< \brief (DAC_INTENCLR) Data Buffer 1 Empty Interrupt Enable */
#define DAC_INTENCLR_EMPTY1         (_U_(1) << DAC_INTENCLR_EMPTY1_Pos)
#define DAC_INTENCLR_EMPTY_Pos      2            /**< \brief (DAC_INTENCLR) Data Buffer x Empty Interrupt Enable */
#define DAC_INTENCLR_EMPTY_Msk      (_U_(0x3) << DAC_INTENCLR_EMPTY_Pos)
#define DAC_INTENCLR_EMPTY(value)   (DAC_INTENCLR_EMPTY_Msk & ((value) << DAC_INTENCLR_EMPTY_Pos))
#define DAC_INTENCLR_RESRDY0_Pos    4            /**< \brief (DAC_INTENCLR) Result 0 Ready Interrupt Enable */
#define DAC_INTENCLR_RESRDY0        (_U_(1) << DAC_INTENCLR_RESRDY0_Pos)
#define DAC_INTENCLR_RESRDY1_Pos    5            /**< \brief (DAC_INTENCLR) Result 1 Ready Interrupt Enable */
#define DAC_INTENCLR_RESRDY1        (_U_(1) << DAC_INTENCLR_RESRDY1_Pos)
#define DAC_INTENCLR_RESRDY_Pos     4            /**< \brief (DAC_INTENCLR) Result x Ready Interrupt Enable */
#define DAC_INTENCLR_RESRDY_Msk     (_U_(0x3) << DAC_INTENCLR_RESRDY_Pos)
#define DAC_INTENCLR_RESRDY(value)  (DAC_INTENCLR_RESRDY_Msk & ((value) << DAC_INTENCLR_RESRDY_Pos))
#define DAC_INTENCLR_OVERRUN0_Pos   6            /**< \brief (DAC_INTENCLR) Overrun 0 Interrupt Enable */
#define DAC_INTENCLR_OVERRUN0       (_U_(1) << DAC_INTENCLR_OVERRUN0_Pos)
#define DAC_INTENCLR_OVERRUN1_Pos   7            /**< \brief (DAC_INTENCLR) Overrun 1 Interrupt Enable */
#define DAC_INTENCLR_OVERRUN1       (_U_(1) << DAC_INTENCLR_OVERRUN1_Pos)
#define DAC_INTENCLR_OVERRUN_Pos    6            /**< \brief (DAC_INTENCLR) Overrun x Interrupt Enable */
#define DAC_INTENCLR_OVERRUN_Msk    (_U_(0x3) << DAC_INTENCLR_OVERRUN_Pos)
#define DAC_INTENCLR_OVERRUN(value) (DAC_INTENCLR_OVERRUN_Msk & ((value) << DAC_INTENCLR_OVERRUN_Pos))
#define DAC_INTENCLR_MASK           _U_(0xFF)    /**< \brief (DAC_INTENCLR) MASK Register */

/* -------- DAC_INTENSET : (DAC Offset: 0x05) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  UNDERRUN0:1;      /*!< bit:      0  Underrun 0 Interrupt Enable        */
    uint8_t  UNDERRUN1:1;      /*!< bit:      1  Underrun 1 Interrupt Enable        */
    uint8_t  EMPTY0:1;         /*!< bit:      2  Data Buffer 0 Empty Interrupt Enable */
    uint8_t  EMPTY1:1;         /*!< bit:      3  Data Buffer 1 Empty Interrupt Enable */
    uint8_t  RESRDY0:1;        /*!< bit:      4  Result 0 Ready Interrupt Enable    */
    uint8_t  RESRDY1:1;        /*!< bit:      5  Result 1 Ready Interrupt Enable    */
    uint8_t  OVERRUN0:1;       /*!< bit:      6  Overrun 0 Interrupt Enable         */
    uint8_t  OVERRUN1:1;       /*!< bit:      7  Overrun 1 Interrupt Enable         */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  UNDERRUN:2;       /*!< bit:  0.. 1  Underrun x Interrupt Enable        */
    uint8_t  EMPTY:2;          /*!< bit:  2.. 3  Data Buffer x Empty Interrupt Enable */
    uint8_t  RESRDY:2;         /*!< bit:  4.. 5  Result x Ready Interrupt Enable    */
    uint8_t  OVERRUN:2;        /*!< bit:  6.. 7  Overrun x Interrupt Enable         */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_INTENSET_OFFSET         0x05         /**< \brief (DAC_INTENSET offset) Interrupt Enable Set */
#define DAC_INTENSET_RESETVALUE     _U_(0x00)    /**< \brief (DAC_INTENSET reset_value) Interrupt Enable Set */

#define DAC_INTENSET_UNDERRUN0_Pos  0            /**< \brief (DAC_INTENSET) Underrun 0 Interrupt Enable */
#define DAC_INTENSET_UNDERRUN0      (_U_(1) << DAC_INTENSET_UNDERRUN0_Pos)
#define DAC_INTENSET_UNDERRUN1_Pos  1            /**< \brief (DAC_INTENSET) Underrun 1 Interrupt Enable */
#define DAC_INTENSET_UNDERRUN1      (_U_(1) << DAC_INTENSET_UNDERRUN1_Pos)
#define DAC_INTENSET_UNDERRUN_Pos   0            /**< \brief (DAC_INTENSET) Underrun x Interrupt Enable */
#define DAC_INTENSET_UNDERRUN_Msk   (_U_(0x3) << DAC_INTENSET_UNDERRUN_Pos)
#define DAC_INTENSET_UNDERRUN(value) (DAC_INTENSET_UNDERRUN_Msk & ((value) << DAC_INTENSET_UNDERRUN_Pos))
#define DAC_INTENSET_EMPTY0_Pos     2            /**< \brief (DAC_INTENSET) Data Buffer 0 Empty Interrupt Enable */
#define DAC_INTENSET_EMPTY0         (_U_(1) << DAC_INTENSET_EMPTY0_Pos)
#define DAC_INTENSET_EMPTY1_Pos     3            /**< \brief (DAC_INTENSET) Data Buffer 1 Empty Interrupt Enable */
#define DAC_INTENSET_EMPTY1         (_U_(1) << DAC_INTENSET_EMPTY1_Pos)
#define DAC_INTENSET_EMPTY_Pos      2            /**< \brief (DAC_INTENSET) Data Buffer x Empty Interrupt Enable */
#define DAC_INTENSET_EMPTY_Msk      (_U_(0x3) << DAC_INTENSET_EMPTY_Pos)
#define DAC_INTENSET_EMPTY(value)   (DAC_INTENSET_EMPTY_Msk & ((value) << DAC_INTENSET_EMPTY_Pos))
#define DAC_INTENSET_RESRDY0_Pos    4            /**< \brief (DAC_INTENSET) Result 0 Ready Interrupt Enable */
#define DAC_INTENSET_RESRDY0        (_U_(1) << DAC_INTENSET_RESRDY0_Pos)
#define DAC_INTENSET_RESRDY1_Pos    5            /**< \brief (DAC_INTENSET) Result 1 Ready Interrupt Enable */
#define DAC_INTENSET_RESRDY1        (_U_(1) << DAC_INTENSET_RESRDY1_Pos)
#define DAC_INTENSET_RESRDY_Pos     4            /**< \brief (DAC_INTENSET) Result x Ready Interrupt Enable */
#define DAC_INTENSET_RESRDY_Msk     (_U_(0x3) << DAC_INTENSET_RESRDY_Pos)
#define DAC_INTENSET_RESRDY(value)  (DAC_INTENSET_RESRDY_Msk & ((value) << DAC_INTENSET_RESRDY_Pos))
#define DAC_INTENSET_OVERRUN0_Pos   6            /**< \brief (DAC_INTENSET) Overrun 0 Interrupt Enable */
#define DAC_INTENSET_OVERRUN0       (_U_(1) << DAC_INTENSET_OVERRUN0_Pos)
#define DAC_INTENSET_OVERRUN1_Pos   7            /**< \brief (DAC_INTENSET) Overrun 1 Interrupt Enable */
#define DAC_INTENSET_OVERRUN1       (_U_(1) << DAC_INTENSET_OVERRUN1_Pos)
#define DAC_INTENSET_OVERRUN_Pos    6            /**< \brief (DAC_INTENSET) Overrun x Interrupt Enable */
#define DAC_INTENSET_OVERRUN_Msk    (_U_(0x3) << DAC_INTENSET_OVERRUN_Pos)
#define DAC_INTENSET_OVERRUN(value) (DAC_INTENSET_OVERRUN_Msk & ((value) << DAC_INTENSET_OVERRUN_Pos))
#define DAC_INTENSET_MASK           _U_(0xFF)    /**< \brief (DAC_INTENSET) MASK Register */

/* -------- DAC_INTFLAG : (DAC Offset: 0x06) (R/W  8) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  UNDERRUN0:1;      /*!< bit:      0  Result 0 Underrun                  */
    __I uint8_t  UNDERRUN1:1;      /*!< bit:      1  Result 1 Underrun                  */
    __I uint8_t  EMPTY0:1;         /*!< bit:      2  Data Buffer 0 Empty                */
    __I uint8_t  EMPTY1:1;         /*!< bit:      3  Data Buffer 1 Empty                */
    __I uint8_t  RESRDY0:1;        /*!< bit:      4  Result 0 Ready                     */
    __I uint8_t  RESRDY1:1;        /*!< bit:      5  Result 1 Ready                     */
    __I uint8_t  OVERRUN0:1;       /*!< bit:      6  Result 0 Overrun                   */
    __I uint8_t  OVERRUN1:1;       /*!< bit:      7  Result 1 Overrun                   */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint8_t  UNDERRUN:2;       /*!< bit:  0.. 1  Result x Underrun                  */
    __I uint8_t  EMPTY:2;          /*!< bit:  2.. 3  Data Buffer x Empty                */
    __I uint8_t  RESRDY:2;         /*!< bit:  4.. 5  Result x Ready                     */
    __I uint8_t  OVERRUN:2;        /*!< bit:  6.. 7  Result x Overrun                   */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_INTFLAG_OFFSET          0x06         /**< \brief (DAC_INTFLAG offset) Interrupt Flag Status and Clear */
#define DAC_INTFLAG_RESETVALUE      _U_(0x00)    /**< \brief (DAC_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define DAC_INTFLAG_UNDERRUN0_Pos   0            /**< \brief (DAC_INTFLAG) Result 0 Underrun */
#define DAC_INTFLAG_UNDERRUN0       (_U_(1) << DAC_INTFLAG_UNDERRUN0_Pos)
#define DAC_INTFLAG_UNDERRUN1_Pos   1            /**< \brief (DAC_INTFLAG) Result 1 Underrun */
#define DAC_INTFLAG_UNDERRUN1       (_U_(1) << DAC_INTFLAG_UNDERRUN1_Pos)
#define DAC_INTFLAG_UNDERRUN_Pos    0            /**< \brief (DAC_INTFLAG) Result x Underrun */
#define DAC_INTFLAG_UNDERRUN_Msk    (_U_(0x3) << DAC_INTFLAG_UNDERRUN_Pos)
#define DAC_INTFLAG_UNDERRUN(value) (DAC_INTFLAG_UNDERRUN_Msk & ((value) << DAC_INTFLAG_UNDERRUN_Pos))
#define DAC_INTFLAG_EMPTY0_Pos      2            /**< \brief (DAC_INTFLAG) Data Buffer 0 Empty */
#define DAC_INTFLAG_EMPTY0          (_U_(1) << DAC_INTFLAG_EMPTY0_Pos)
#define DAC_INTFLAG_EMPTY1_Pos      3            /**< \brief (DAC_INTFLAG) Data Buffer 1 Empty */
#define DAC_INTFLAG_EMPTY1          (_U_(1) << DAC_INTFLAG_EMPTY1_Pos)
#define DAC_INTFLAG_EMPTY_Pos       2            /**< \brief (DAC_INTFLAG) Data Buffer x Empty */
#define DAC_INTFLAG_EMPTY_Msk       (_U_(0x3) << DAC_INTFLAG_EMPTY_Pos)
#define DAC_INTFLAG_EMPTY(value)    (DAC_INTFLAG_EMPTY_Msk & ((value) << DAC_INTFLAG_EMPTY_Pos))
#define DAC_INTFLAG_RESRDY0_Pos     4            /**< \brief (DAC_INTFLAG) Result 0 Ready */
#define DAC_INTFLAG_RESRDY0         (_U_(1) << DAC_INTFLAG_RESRDY0_Pos)
#define DAC_INTFLAG_RESRDY1_Pos     5            /**< \brief (DAC_INTFLAG) Result 1 Ready */
#define DAC_INTFLAG_RESRDY1         (_U_(1) << DAC_INTFLAG_RESRDY1_Pos)
#define DAC_INTFLAG_RESRDY_Pos      4            /**< \brief (DAC_INTFLAG) Result x Ready */
#define DAC_INTFLAG_RESRDY_Msk      (_U_(0x3) << DAC_INTFLAG_RESRDY_Pos)
#define DAC_INTFLAG_RESRDY(value)   (DAC_INTFLAG_RESRDY_Msk & ((value) << DAC_INTFLAG_RESRDY_Pos))
#define DAC_INTFLAG_OVERRUN0_Pos    6            /**< \brief (DAC_INTFLAG) Result 0 Overrun */
#define DAC_INTFLAG_OVERRUN0        (_U_(1) << DAC_INTFLAG_OVERRUN0_Pos)
#define DAC_INTFLAG_OVERRUN1_Pos    7            /**< \brief (DAC_INTFLAG) Result 1 Overrun */
#define DAC_INTFLAG_OVERRUN1        (_U_(1) << DAC_INTFLAG_OVERRUN1_Pos)
#define DAC_INTFLAG_OVERRUN_Pos     6            /**< \brief (DAC_INTFLAG) Result x Overrun */
#define DAC_INTFLAG_OVERRUN_Msk     (_U_(0x3) << DAC_INTFLAG_OVERRUN_Pos)
#define DAC_INTFLAG_OVERRUN(value)  (DAC_INTFLAG_OVERRUN_Msk & ((value) << DAC_INTFLAG_OVERRUN_Pos))
#define DAC_INTFLAG_MASK            _U_(0xFF)    /**< \brief (DAC_INTFLAG) MASK Register */

/* -------- DAC_STATUS : (DAC Offset: 0x07) (R/   8) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  READY0:1;         /*!< bit:      0  DAC 0 Startup Ready                */
    uint8_t  READY1:1;         /*!< bit:      1  DAC 1 Startup Ready                */
    uint8_t  EOC0:1;           /*!< bit:      2  DAC 0 End of Conversion            */
    uint8_t  EOC1:1;           /*!< bit:      3  DAC 1 End of Conversion            */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  READY:2;          /*!< bit:  0.. 1  DAC x Startup Ready                */
    uint8_t  EOC:2;            /*!< bit:  2.. 3  DAC x End of Conversion            */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_STATUS_OFFSET           0x07         /**< \brief (DAC_STATUS offset) Status */
#define DAC_STATUS_RESETVALUE       _U_(0x00)    /**< \brief (DAC_STATUS reset_value) Status */

#define DAC_STATUS_READY0_Pos       0            /**< \brief (DAC_STATUS) DAC 0 Startup Ready */
#define DAC_STATUS_READY0           (_U_(1) << DAC_STATUS_READY0_Pos)
#define DAC_STATUS_READY1_Pos       1            /**< \brief (DAC_STATUS) DAC 1 Startup Ready */
#define DAC_STATUS_READY1           (_U_(1) << DAC_STATUS_READY1_Pos)
#define DAC_STATUS_READY_Pos        0            /**< \brief (DAC_STATUS) DAC x Startup Ready */
#define DAC_STATUS_READY_Msk        (_U_(0x3) << DAC_STATUS_READY_Pos)
#define DAC_STATUS_READY(value)     (DAC_STATUS_READY_Msk & ((value) << DAC_STATUS_READY_Pos))
#define DAC_STATUS_EOC0_Pos         2            /**< \brief (DAC_STATUS) DAC 0 End of Conversion */
#define DAC_STATUS_EOC0             (_U_(1) << DAC_STATUS_EOC0_Pos)
#define DAC_STATUS_EOC1_Pos         3            /**< \brief (DAC_STATUS) DAC 1 End of Conversion */
#define DAC_STATUS_EOC1             (_U_(1) << DAC_STATUS_EOC1_Pos)
#define DAC_STATUS_EOC_Pos          2            /**< \brief (DAC_STATUS) DAC x End of Conversion */
#define DAC_STATUS_EOC_Msk          (_U_(0x3) << DAC_STATUS_EOC_Pos)
#define DAC_STATUS_EOC(value)       (DAC_STATUS_EOC_Msk & ((value) << DAC_STATUS_EOC_Pos))
#define DAC_STATUS_MASK             _U_(0x0F)    /**< \brief (DAC_STATUS) MASK Register */

/* -------- DAC_SYNCBUSY : (DAC Offset: 0x08) (R/  32) Synchronization Busy -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint32_t ENABLE:1;         /*!< bit:      1  DAC Enable Status                  */
    uint32_t DATA0:1;          /*!< bit:      2  Data DAC 0                         */
    uint32_t DATA1:1;          /*!< bit:      3  Data DAC 1                         */
    uint32_t DATABUF0:1;       /*!< bit:      4  Data Buffer DAC 0                  */
    uint32_t DATABUF1:1;       /*!< bit:      5  Data Buffer DAC 1                  */
    uint32_t :26;              /*!< bit:  6..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :2;               /*!< bit:  0.. 1  Reserved                           */
    uint32_t DATA:2;           /*!< bit:  2.. 3  Data DAC x                         */
    uint32_t DATABUF:2;        /*!< bit:  4.. 5  Data Buffer DAC x                  */
    uint32_t :26;              /*!< bit:  6..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DAC_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_SYNCBUSY_OFFSET         0x08         /**< \brief (DAC_SYNCBUSY offset) Synchronization Busy */
#define DAC_SYNCBUSY_RESETVALUE     _U_(0x00000000) /**< \brief (DAC_SYNCBUSY reset_value) Synchronization Busy */

#define DAC_SYNCBUSY_SWRST_Pos      0            /**< \brief (DAC_SYNCBUSY) Software Reset */
#define DAC_SYNCBUSY_SWRST          (_U_(0x1) << DAC_SYNCBUSY_SWRST_Pos)
#define DAC_SYNCBUSY_ENABLE_Pos     1            /**< \brief (DAC_SYNCBUSY) DAC Enable Status */
#define DAC_SYNCBUSY_ENABLE         (_U_(0x1) << DAC_SYNCBUSY_ENABLE_Pos)
#define DAC_SYNCBUSY_DATA0_Pos      2            /**< \brief (DAC_SYNCBUSY) Data DAC 0 */
#define DAC_SYNCBUSY_DATA0          (_U_(1) << DAC_SYNCBUSY_DATA0_Pos)
#define DAC_SYNCBUSY_DATA1_Pos      3            /**< \brief (DAC_SYNCBUSY) Data DAC 1 */
#define DAC_SYNCBUSY_DATA1          (_U_(1) << DAC_SYNCBUSY_DATA1_Pos)
#define DAC_SYNCBUSY_DATA_Pos       2            /**< \brief (DAC_SYNCBUSY) Data DAC x */
#define DAC_SYNCBUSY_DATA_Msk       (_U_(0x3) << DAC_SYNCBUSY_DATA_Pos)
#define DAC_SYNCBUSY_DATA(value)    (DAC_SYNCBUSY_DATA_Msk & ((value) << DAC_SYNCBUSY_DATA_Pos))
#define DAC_SYNCBUSY_DATABUF0_Pos   4            /**< \brief (DAC_SYNCBUSY) Data Buffer DAC 0 */
#define DAC_SYNCBUSY_DATABUF0       (_U_(1) << DAC_SYNCBUSY_DATABUF0_Pos)
#define DAC_SYNCBUSY_DATABUF1_Pos   5            /**< \brief (DAC_SYNCBUSY) Data Buffer DAC 1 */
#define DAC_SYNCBUSY_DATABUF1       (_U_(1) << DAC_SYNCBUSY_DATABUF1_Pos)
#define DAC_SYNCBUSY_DATABUF_Pos    4            /**< \brief (DAC_SYNCBUSY) Data Buffer DAC x */
#define DAC_SYNCBUSY_DATABUF_Msk    (_U_(0x3) << DAC_SYNCBUSY_DATABUF_Pos)
#define DAC_SYNCBUSY_DATABUF(value) (DAC_SYNCBUSY_DATABUF_Msk & ((value) << DAC_SYNCBUSY_DATABUF_Pos))
#define DAC_SYNCBUSY_MASK           _U_(0x0000003F) /**< \brief (DAC_SYNCBUSY) MASK Register */

/* -------- DAC_DACCTRL : (DAC Offset: 0x0C) (R/W 16) DAC n Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t LEFTADJ:1;        /*!< bit:      0  Left Adjusted Data                 */
    uint16_t ENABLE:1;         /*!< bit:      1  Enable DAC0                        */
    uint16_t CCTRL:2;          /*!< bit:  2.. 3  Current Control                    */
    uint16_t :1;               /*!< bit:      4  Reserved                           */
    uint16_t FEXT:1;           /*!< bit:      5  Standalone Filter                  */
    uint16_t RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint16_t DITHER:1;         /*!< bit:      7  Dithering Mode                     */
    uint16_t REFRESH:4;        /*!< bit:  8..11  Refresh period                     */
    uint16_t :1;               /*!< bit:     12  Reserved                           */
    uint16_t OSR:3;            /*!< bit: 13..15  Sampling Rate                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} DAC_DACCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_DACCTRL_OFFSET          0x0C         /**< \brief (DAC_DACCTRL offset) DAC n Control */
#define DAC_DACCTRL_RESETVALUE      _U_(0x0000)  /**< \brief (DAC_DACCTRL reset_value) DAC n Control */

#define DAC_DACCTRL_LEFTADJ_Pos     0            /**< \brief (DAC_DACCTRL) Left Adjusted Data */
#define DAC_DACCTRL_LEFTADJ         (_U_(0x1) << DAC_DACCTRL_LEFTADJ_Pos)
#define DAC_DACCTRL_ENABLE_Pos      1            /**< \brief (DAC_DACCTRL) Enable DAC0 */
#define DAC_DACCTRL_ENABLE          (_U_(0x1) << DAC_DACCTRL_ENABLE_Pos)
#define DAC_DACCTRL_CCTRL_Pos       2            /**< \brief (DAC_DACCTRL) Current Control */
#define DAC_DACCTRL_CCTRL_Msk       (_U_(0x3) << DAC_DACCTRL_CCTRL_Pos)
#define DAC_DACCTRL_CCTRL(value)    (DAC_DACCTRL_CCTRL_Msk & ((value) << DAC_DACCTRL_CCTRL_Pos))
#define   DAC_DACCTRL_CCTRL_CC100K_Val    _U_(0x0)   /**< \brief (DAC_DACCTRL) GCLK_DAC ≤ 1.2MHz (100kSPS) */
#define   DAC_DACCTRL_CCTRL_CC1M_Val      _U_(0x1)   /**< \brief (DAC_DACCTRL) 1.2MHz < GCLK_DAC  ≤ 6MHz (500kSPS) */
#define   DAC_DACCTRL_CCTRL_CC12M_Val     _U_(0x2)   /**< \brief (DAC_DACCTRL) 6MHz < GCLK_DAC ≤ 12MHz (1MSPS) */
#define DAC_DACCTRL_CCTRL_CC100K    (DAC_DACCTRL_CCTRL_CC100K_Val  << DAC_DACCTRL_CCTRL_Pos)
#define DAC_DACCTRL_CCTRL_CC1M      (DAC_DACCTRL_CCTRL_CC1M_Val    << DAC_DACCTRL_CCTRL_Pos)
#define DAC_DACCTRL_CCTRL_CC12M     (DAC_DACCTRL_CCTRL_CC12M_Val   << DAC_DACCTRL_CCTRL_Pos)
#define DAC_DACCTRL_FEXT_Pos        5            /**< \brief (DAC_DACCTRL) Standalone Filter */
#define DAC_DACCTRL_FEXT            (_U_(0x1) << DAC_DACCTRL_FEXT_Pos)
#define DAC_DACCTRL_RUNSTDBY_Pos    6            /**< \brief (DAC_DACCTRL) Run in Standby */
#define DAC_DACCTRL_RUNSTDBY        (_U_(0x1) << DAC_DACCTRL_RUNSTDBY_Pos)
#define DAC_DACCTRL_DITHER_Pos      7            /**< \brief (DAC_DACCTRL) Dithering Mode */
#define DAC_DACCTRL_DITHER          (_U_(0x1) << DAC_DACCTRL_DITHER_Pos)
#define DAC_DACCTRL_REFRESH_Pos     8            /**< \brief (DAC_DACCTRL) Refresh period */
#define DAC_DACCTRL_REFRESH_Msk     (_U_(0xF) << DAC_DACCTRL_REFRESH_Pos)
#define DAC_DACCTRL_REFRESH(value)  (DAC_DACCTRL_REFRESH_Msk & ((value) << DAC_DACCTRL_REFRESH_Pos))
#define DAC_DACCTRL_OSR_Pos         13           /**< \brief (DAC_DACCTRL) Sampling Rate */
#define DAC_DACCTRL_OSR_Msk         (_U_(0x7) << DAC_DACCTRL_OSR_Pos)
#define DAC_DACCTRL_OSR(value)      (DAC_DACCTRL_OSR_Msk & ((value) << DAC_DACCTRL_OSR_Pos))
#define DAC_DACCTRL_MASK            _U_(0xEFEF)  /**< \brief (DAC_DACCTRL) MASK Register */

/* -------- DAC_DATA : (DAC Offset: 0x10) ( /W 16) DAC n Data -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t DATA:16;          /*!< bit:  0..15  DAC0 Data                          */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} DAC_DATA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_DATA_OFFSET             0x10         /**< \brief (DAC_DATA offset) DAC n Data */
#define DAC_DATA_RESETVALUE         _U_(0x0000)  /**< \brief (DAC_DATA reset_value) DAC n Data */

#define DAC_DATA_DATA_Pos           0            /**< \brief (DAC_DATA) DAC0 Data */
#define DAC_DATA_DATA_Msk           (_U_(0xFFFF) << DAC_DATA_DATA_Pos)
#define DAC_DATA_DATA(value)        (DAC_DATA_DATA_Msk & ((value) << DAC_DATA_DATA_Pos))
#define DAC_DATA_MASK               _U_(0xFFFF)  /**< \brief (DAC_DATA) MASK Register */

/* -------- DAC_DATABUF : (DAC Offset: 0x14) ( /W 16) DAC n Data Buffer -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t DATABUF:16;       /*!< bit:  0..15  DAC0 Data Buffer                   */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} DAC_DATABUF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_DATABUF_OFFSET          0x14         /**< \brief (DAC_DATABUF offset) DAC n Data Buffer */
#define DAC_DATABUF_RESETVALUE      _U_(0x0000)  /**< \brief (DAC_DATABUF reset_value) DAC n Data Buffer */

#define DAC_DATABUF_DATABUF_Pos     0            /**< \brief (DAC_DATABUF) DAC0 Data Buffer */
#define DAC_DATABUF_DATABUF_Msk     (_U_(0xFFFF) << DAC_DATABUF_DATABUF_Pos)
#define DAC_DATABUF_DATABUF(value)  (DAC_DATABUF_DATABUF_Msk & ((value) << DAC_DATABUF_DATABUF_Pos))
#define DAC_DATABUF_MASK            _U_(0xFFFF)  /**< \brief (DAC_DATABUF) MASK Register */

/* -------- DAC_DBGCTRL : (DAC Offset: 0x18) (R/W  8) Debug Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DBGRUN:1;         /*!< bit:      0  Debug Run                          */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_DBGCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_DBGCTRL_OFFSET          0x18         /**< \brief (DAC_DBGCTRL offset) Debug Control */
#define DAC_DBGCTRL_RESETVALUE      _U_(0x00)    /**< \brief (DAC_DBGCTRL reset_value) Debug Control */

#define DAC_DBGCTRL_DBGRUN_Pos      0            /**< \brief (DAC_DBGCTRL) Debug Run */
#define DAC_DBGCTRL_DBGRUN          (_U_(0x1) << DAC_DBGCTRL_DBGRUN_Pos)
#define DAC_DBGCTRL_MASK            _U_(0x01)    /**< \brief (DAC_DBGCTRL) MASK Register */

/* -------- DAC_RESULT : (DAC Offset: 0x1C) (R/  16) Filter Result -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t RESULT:16;        /*!< bit:  0..15  Filter Result                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} DAC_RESULT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_RESULT_OFFSET           0x1C         /**< \brief (DAC_RESULT offset) Filter Result */
#define DAC_RESULT_RESETVALUE       _U_(0x0000)  /**< \brief (DAC_RESULT reset_value) Filter Result */

#define DAC_RESULT_RESULT_Pos       0            /**< \brief (DAC_RESULT) Filter Result */
#define DAC_RESULT_RESULT_Msk       (_U_(0xFFFF) << DAC_RESULT_RESULT_Pos)
#define DAC_RESULT_RESULT(value)    (DAC_RESULT_RESULT_Msk & ((value) << DAC_RESULT_RESULT_Pos))
#define DAC_RESULT_MASK             _U_(0xFFFF)  /**< \brief (DAC_RESULT) MASK Register */

/** \brief DAC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO DAC_CTRLA_Type            CTRLA;       /**< \brief Offset: 0x00 (R/W  8) Control A */
  __IO DAC_CTRLB_Type            CTRLB;       /**< \brief Offset: 0x01 (R/W  8) Control B */
  __IO DAC_EVCTRL_Type           EVCTRL;      /**< \brief Offset: 0x02 (R/W  8) Event Control */
       RoReg8                    Reserved1[0x1];
  __IO DAC_INTENCLR_Type         INTENCLR;    /**< \brief Offset: 0x04 (R/W  8) Interrupt Enable Clear */
  __IO DAC_INTENSET_Type         INTENSET;    /**< \brief Offset: 0x05 (R/W  8) Interrupt Enable Set */
  __IO DAC_INTFLAG_Type          INTFLAG;     /**< \brief Offset: 0x06 (R/W  8) Interrupt Flag Status and Clear */
  __I  DAC_STATUS_Type           STATUS;      /**< \brief Offset: 0x07 (R/   8) Status */
  __I  DAC_SYNCBUSY_Type         SYNCBUSY;    /**< \brief Offset: 0x08 (R/  32) Synchronization Busy */
  __IO DAC_DACCTRL_Type          DACCTRL[2];  /**< \brief Offset: 0x0C (R/W 16) DAC n Control */
  __O  DAC_DATA_Type             DATA[2];     /**< \brief Offset: 0x10 ( /W 16) DAC n Data */
  __O  DAC_DATABUF_Type          DATABUF[2];  /**< \brief Offset: 0x14 ( /W 16) DAC n Data Buffer */
  __IO DAC_DBGCTRL_Type          DBGCTRL;     /**< \brief Offset: 0x18 (R/W  8) Debug Control */
       RoReg8                    Reserved2[0x3];
  __I  DAC_RESULT_Type           RESULT[2];   /**< \brief Offset: 0x1C (R/  16) Filter Result */
} Dac;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_DAC_COMPONENT_ */
