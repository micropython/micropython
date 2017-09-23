/**
 * \file
 *
 * \brief Component description for AC
 *
 * Copyright (c) 2016 Atmel Corporation,
 *                    a wholly owned subsidiary of Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD21_AC_COMPONENT_
#define _SAMD21_AC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR AC */
/* ========================================================================== */
/** \addtogroup SAMD21_AC Analog Comparators */
/*@{*/

#define AC_U2205
#define REV_AC                      0x111

/* -------- AC_CTRLA : (AC Offset: 0x00) (R/W  8) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  RUNSTDBY:1;       /*!< bit:      2  Run in Standby                     */
    uint8_t  :4;               /*!< bit:  3.. 6  Reserved                           */
    uint8_t  LPMUX:1;          /*!< bit:      7  Low-Power Mux                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} AC_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AC_CTRLA_OFFSET             0x00         /**< \brief (AC_CTRLA offset) Control A */
#define AC_CTRLA_RESETVALUE         _U(0x00)     /**< \brief (AC_CTRLA reset_value) Control A */

#define AC_CTRLA_SWRST_Pos          0            /**< \brief (AC_CTRLA) Software Reset */
#define AC_CTRLA_SWRST              (_U(0x1) << AC_CTRLA_SWRST_Pos)
#define AC_CTRLA_ENABLE_Pos         1            /**< \brief (AC_CTRLA) Enable */
#define AC_CTRLA_ENABLE             (_U(0x1) << AC_CTRLA_ENABLE_Pos)
#define AC_CTRLA_RUNSTDBY_Pos       2            /**< \brief (AC_CTRLA) Run in Standby */
#define AC_CTRLA_RUNSTDBY_Msk       (_U(0x1) << AC_CTRLA_RUNSTDBY_Pos)
#define AC_CTRLA_RUNSTDBY(value)    (AC_CTRLA_RUNSTDBY_Msk & ((value) << AC_CTRLA_RUNSTDBY_Pos))
#define AC_CTRLA_LPMUX_Pos          7            /**< \brief (AC_CTRLA) Low-Power Mux */
#define AC_CTRLA_LPMUX              (_U(0x1) << AC_CTRLA_LPMUX_Pos)
#define AC_CTRLA_MASK               _U(0x87)     /**< \brief (AC_CTRLA) MASK Register */

/* -------- AC_CTRLB : (AC Offset: 0x01) ( /W  8) Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  START0:1;         /*!< bit:      0  Comparator 0 Start Comparison      */
    uint8_t  START1:1;         /*!< bit:      1  Comparator 1 Start Comparison      */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  START:2;          /*!< bit:  0.. 1  Comparator x Start Comparison      */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} AC_CTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AC_CTRLB_OFFSET             0x01         /**< \brief (AC_CTRLB offset) Control B */
#define AC_CTRLB_RESETVALUE         _U(0x00)     /**< \brief (AC_CTRLB reset_value) Control B */

#define AC_CTRLB_START0_Pos         0            /**< \brief (AC_CTRLB) Comparator 0 Start Comparison */
#define AC_CTRLB_START0             (1 << AC_CTRLB_START0_Pos)
#define AC_CTRLB_START1_Pos         1            /**< \brief (AC_CTRLB) Comparator 1 Start Comparison */
#define AC_CTRLB_START1             (1 << AC_CTRLB_START1_Pos)
#define AC_CTRLB_START_Pos          0            /**< \brief (AC_CTRLB) Comparator x Start Comparison */
#define AC_CTRLB_START_Msk          (_U(0x3) << AC_CTRLB_START_Pos)
#define AC_CTRLB_START(value)       (AC_CTRLB_START_Msk & ((value) << AC_CTRLB_START_Pos))
#define AC_CTRLB_MASK               _U(0x03)     /**< \brief (AC_CTRLB) MASK Register */

/* -------- AC_EVCTRL : (AC Offset: 0x02) (R/W 16) Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t COMPEO0:1;        /*!< bit:      0  Comparator 0 Event Output Enable   */
    uint16_t COMPEO1:1;        /*!< bit:      1  Comparator 1 Event Output Enable   */
    uint16_t :2;               /*!< bit:  2.. 3  Reserved                           */
    uint16_t WINEO0:1;         /*!< bit:      4  Window 0 Event Output Enable       */
    uint16_t :3;               /*!< bit:  5.. 7  Reserved                           */
    uint16_t COMPEI0:1;        /*!< bit:      8  Comparator 0 Event Input           */
    uint16_t COMPEI1:1;        /*!< bit:      9  Comparator 1 Event Input           */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint16_t COMPEO:2;         /*!< bit:  0.. 1  Comparator x Event Output Enable   */
    uint16_t :2;               /*!< bit:  2.. 3  Reserved                           */
    uint16_t WINEO:1;          /*!< bit:      4  Window x Event Output Enable       */
    uint16_t :3;               /*!< bit:  5.. 7  Reserved                           */
    uint16_t COMPEI:2;         /*!< bit:  8.. 9  Comparator x Event Input           */
    uint16_t :6;               /*!< bit: 10..15  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} AC_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AC_EVCTRL_OFFSET            0x02         /**< \brief (AC_EVCTRL offset) Event Control */
#define AC_EVCTRL_RESETVALUE        _U(0x0000)   /**< \brief (AC_EVCTRL reset_value) Event Control */

#define AC_EVCTRL_COMPEO0_Pos       0            /**< \brief (AC_EVCTRL) Comparator 0 Event Output Enable */
#define AC_EVCTRL_COMPEO0           (1 << AC_EVCTRL_COMPEO0_Pos)
#define AC_EVCTRL_COMPEO1_Pos       1            /**< \brief (AC_EVCTRL) Comparator 1 Event Output Enable */
#define AC_EVCTRL_COMPEO1           (1 << AC_EVCTRL_COMPEO1_Pos)
#define AC_EVCTRL_COMPEO_Pos        0            /**< \brief (AC_EVCTRL) Comparator x Event Output Enable */
#define AC_EVCTRL_COMPEO_Msk        (_U(0x3) << AC_EVCTRL_COMPEO_Pos)
#define AC_EVCTRL_COMPEO(value)     (AC_EVCTRL_COMPEO_Msk & ((value) << AC_EVCTRL_COMPEO_Pos))
#define AC_EVCTRL_WINEO0_Pos        4            /**< \brief (AC_EVCTRL) Window 0 Event Output Enable */
#define AC_EVCTRL_WINEO0            (1 << AC_EVCTRL_WINEO0_Pos)
#define AC_EVCTRL_WINEO_Pos         4            /**< \brief (AC_EVCTRL) Window x Event Output Enable */
#define AC_EVCTRL_WINEO_Msk         (_U(0x1) << AC_EVCTRL_WINEO_Pos)
#define AC_EVCTRL_WINEO(value)      (AC_EVCTRL_WINEO_Msk & ((value) << AC_EVCTRL_WINEO_Pos))
#define AC_EVCTRL_COMPEI0_Pos       8            /**< \brief (AC_EVCTRL) Comparator 0 Event Input */
#define AC_EVCTRL_COMPEI0           (1 << AC_EVCTRL_COMPEI0_Pos)
#define AC_EVCTRL_COMPEI1_Pos       9            /**< \brief (AC_EVCTRL) Comparator 1 Event Input */
#define AC_EVCTRL_COMPEI1           (1 << AC_EVCTRL_COMPEI1_Pos)
#define AC_EVCTRL_COMPEI_Pos        8            /**< \brief (AC_EVCTRL) Comparator x Event Input */
#define AC_EVCTRL_COMPEI_Msk        (_U(0x3) << AC_EVCTRL_COMPEI_Pos)
#define AC_EVCTRL_COMPEI(value)     (AC_EVCTRL_COMPEI_Msk & ((value) << AC_EVCTRL_COMPEI_Pos))
#define AC_EVCTRL_MASK              _U(0x0313)   /**< \brief (AC_EVCTRL) MASK Register */

/* -------- AC_INTENCLR : (AC Offset: 0x04) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  COMP0:1;          /*!< bit:      0  Comparator 0 Interrupt Enable      */
    uint8_t  COMP1:1;          /*!< bit:      1  Comparator 1 Interrupt Enable      */
    uint8_t  :2;               /*!< bit:  2.. 3  Reserved                           */
    uint8_t  WIN0:1;           /*!< bit:      4  Window 0 Interrupt Enable          */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  COMP:2;           /*!< bit:  0.. 1  Comparator x Interrupt Enable      */
    uint8_t  :2;               /*!< bit:  2.. 3  Reserved                           */
    uint8_t  WIN:1;            /*!< bit:      4  Window x Interrupt Enable          */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} AC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AC_INTENCLR_OFFSET          0x04         /**< \brief (AC_INTENCLR offset) Interrupt Enable Clear */
#define AC_INTENCLR_RESETVALUE      _U(0x00)     /**< \brief (AC_INTENCLR reset_value) Interrupt Enable Clear */

#define AC_INTENCLR_COMP0_Pos       0            /**< \brief (AC_INTENCLR) Comparator 0 Interrupt Enable */
#define AC_INTENCLR_COMP0           (1 << AC_INTENCLR_COMP0_Pos)
#define AC_INTENCLR_COMP1_Pos       1            /**< \brief (AC_INTENCLR) Comparator 1 Interrupt Enable */
#define AC_INTENCLR_COMP1           (1 << AC_INTENCLR_COMP1_Pos)
#define AC_INTENCLR_COMP_Pos        0            /**< \brief (AC_INTENCLR) Comparator x Interrupt Enable */
#define AC_INTENCLR_COMP_Msk        (_U(0x3) << AC_INTENCLR_COMP_Pos)
#define AC_INTENCLR_COMP(value)     (AC_INTENCLR_COMP_Msk & ((value) << AC_INTENCLR_COMP_Pos))
#define AC_INTENCLR_WIN0_Pos        4            /**< \brief (AC_INTENCLR) Window 0 Interrupt Enable */
#define AC_INTENCLR_WIN0            (1 << AC_INTENCLR_WIN0_Pos)
#define AC_INTENCLR_WIN_Pos         4            /**< \brief (AC_INTENCLR) Window x Interrupt Enable */
#define AC_INTENCLR_WIN_Msk         (_U(0x1) << AC_INTENCLR_WIN_Pos)
#define AC_INTENCLR_WIN(value)      (AC_INTENCLR_WIN_Msk & ((value) << AC_INTENCLR_WIN_Pos))
#define AC_INTENCLR_MASK            _U(0x13)     /**< \brief (AC_INTENCLR) MASK Register */

/* -------- AC_INTENSET : (AC Offset: 0x05) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  COMP0:1;          /*!< bit:      0  Comparator 0 Interrupt Enable      */
    uint8_t  COMP1:1;          /*!< bit:      1  Comparator 1 Interrupt Enable      */
    uint8_t  :2;               /*!< bit:  2.. 3  Reserved                           */
    uint8_t  WIN0:1;           /*!< bit:      4  Window 0 Interrupt Enable          */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  COMP:2;           /*!< bit:  0.. 1  Comparator x Interrupt Enable      */
    uint8_t  :2;               /*!< bit:  2.. 3  Reserved                           */
    uint8_t  WIN:1;            /*!< bit:      4  Window x Interrupt Enable          */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} AC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AC_INTENSET_OFFSET          0x05         /**< \brief (AC_INTENSET offset) Interrupt Enable Set */
#define AC_INTENSET_RESETVALUE      _U(0x00)     /**< \brief (AC_INTENSET reset_value) Interrupt Enable Set */

#define AC_INTENSET_COMP0_Pos       0            /**< \brief (AC_INTENSET) Comparator 0 Interrupt Enable */
#define AC_INTENSET_COMP0           (1 << AC_INTENSET_COMP0_Pos)
#define AC_INTENSET_COMP1_Pos       1            /**< \brief (AC_INTENSET) Comparator 1 Interrupt Enable */
#define AC_INTENSET_COMP1           (1 << AC_INTENSET_COMP1_Pos)
#define AC_INTENSET_COMP_Pos        0            /**< \brief (AC_INTENSET) Comparator x Interrupt Enable */
#define AC_INTENSET_COMP_Msk        (_U(0x3) << AC_INTENSET_COMP_Pos)
#define AC_INTENSET_COMP(value)     (AC_INTENSET_COMP_Msk & ((value) << AC_INTENSET_COMP_Pos))
#define AC_INTENSET_WIN0_Pos        4            /**< \brief (AC_INTENSET) Window 0 Interrupt Enable */
#define AC_INTENSET_WIN0            (1 << AC_INTENSET_WIN0_Pos)
#define AC_INTENSET_WIN_Pos         4            /**< \brief (AC_INTENSET) Window x Interrupt Enable */
#define AC_INTENSET_WIN_Msk         (_U(0x1) << AC_INTENSET_WIN_Pos)
#define AC_INTENSET_WIN(value)      (AC_INTENSET_WIN_Msk & ((value) << AC_INTENSET_WIN_Pos))
#define AC_INTENSET_MASK            _U(0x13)     /**< \brief (AC_INTENSET) MASK Register */

/* -------- AC_INTFLAG : (AC Offset: 0x06) (R/W  8) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  COMP0:1;          /*!< bit:      0  Comparator 0                       */
    __I uint8_t  COMP1:1;          /*!< bit:      1  Comparator 1                       */
    __I uint8_t  :2;               /*!< bit:  2.. 3  Reserved                           */
    __I uint8_t  WIN0:1;           /*!< bit:      4  Window 0                           */
    __I uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint8_t  COMP:2;           /*!< bit:  0.. 1  Comparator x                       */
    __I uint8_t  :2;               /*!< bit:  2.. 3  Reserved                           */
    __I uint8_t  WIN:1;            /*!< bit:      4  Window x                           */
    __I uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} AC_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AC_INTFLAG_OFFSET           0x06         /**< \brief (AC_INTFLAG offset) Interrupt Flag Status and Clear */
#define AC_INTFLAG_RESETVALUE       _U(0x00)     /**< \brief (AC_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define AC_INTFLAG_COMP0_Pos        0            /**< \brief (AC_INTFLAG) Comparator 0 */
#define AC_INTFLAG_COMP0            (1 << AC_INTFLAG_COMP0_Pos)
#define AC_INTFLAG_COMP1_Pos        1            /**< \brief (AC_INTFLAG) Comparator 1 */
#define AC_INTFLAG_COMP1            (1 << AC_INTFLAG_COMP1_Pos)
#define AC_INTFLAG_COMP_Pos         0            /**< \brief (AC_INTFLAG) Comparator x */
#define AC_INTFLAG_COMP_Msk         (_U(0x3) << AC_INTFLAG_COMP_Pos)
#define AC_INTFLAG_COMP(value)      (AC_INTFLAG_COMP_Msk & ((value) << AC_INTFLAG_COMP_Pos))
#define AC_INTFLAG_WIN0_Pos         4            /**< \brief (AC_INTFLAG) Window 0 */
#define AC_INTFLAG_WIN0             (1 << AC_INTFLAG_WIN0_Pos)
#define AC_INTFLAG_WIN_Pos          4            /**< \brief (AC_INTFLAG) Window x */
#define AC_INTFLAG_WIN_Msk          (_U(0x1) << AC_INTFLAG_WIN_Pos)
#define AC_INTFLAG_WIN(value)       (AC_INTFLAG_WIN_Msk & ((value) << AC_INTFLAG_WIN_Pos))
#define AC_INTFLAG_MASK             _U(0x13)     /**< \brief (AC_INTFLAG) MASK Register */

/* -------- AC_STATUSA : (AC Offset: 0x08) (R/   8) Status A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  STATE0:1;         /*!< bit:      0  Comparator 0 Current State         */
    uint8_t  STATE1:1;         /*!< bit:      1  Comparator 1 Current State         */
    uint8_t  :2;               /*!< bit:  2.. 3  Reserved                           */
    uint8_t  WSTATE0:2;        /*!< bit:  4.. 5  Window 0 Current State             */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  STATE:2;          /*!< bit:  0.. 1  Comparator x Current State         */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} AC_STATUSA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AC_STATUSA_OFFSET           0x08         /**< \brief (AC_STATUSA offset) Status A */
#define AC_STATUSA_RESETVALUE       _U(0x00)     /**< \brief (AC_STATUSA reset_value) Status A */

#define AC_STATUSA_STATE0_Pos       0            /**< \brief (AC_STATUSA) Comparator 0 Current State */
#define AC_STATUSA_STATE0           (1 << AC_STATUSA_STATE0_Pos)
#define AC_STATUSA_STATE1_Pos       1            /**< \brief (AC_STATUSA) Comparator 1 Current State */
#define AC_STATUSA_STATE1           (1 << AC_STATUSA_STATE1_Pos)
#define AC_STATUSA_STATE_Pos        0            /**< \brief (AC_STATUSA) Comparator x Current State */
#define AC_STATUSA_STATE_Msk        (_U(0x3) << AC_STATUSA_STATE_Pos)
#define AC_STATUSA_STATE(value)     (AC_STATUSA_STATE_Msk & ((value) << AC_STATUSA_STATE_Pos))
#define AC_STATUSA_WSTATE0_Pos      4            /**< \brief (AC_STATUSA) Window 0 Current State */
#define AC_STATUSA_WSTATE0_Msk      (_U(0x3) << AC_STATUSA_WSTATE0_Pos)
#define AC_STATUSA_WSTATE0(value)   (AC_STATUSA_WSTATE0_Msk & ((value) << AC_STATUSA_WSTATE0_Pos))
#define   AC_STATUSA_WSTATE0_ABOVE_Val    _U(0x0)   /**< \brief (AC_STATUSA) Signal is above window */
#define   AC_STATUSA_WSTATE0_INSIDE_Val   _U(0x1)   /**< \brief (AC_STATUSA) Signal is inside window */
#define   AC_STATUSA_WSTATE0_BELOW_Val    _U(0x2)   /**< \brief (AC_STATUSA) Signal is below window */
#define AC_STATUSA_WSTATE0_ABOVE    (AC_STATUSA_WSTATE0_ABOVE_Val  << AC_STATUSA_WSTATE0_Pos)
#define AC_STATUSA_WSTATE0_INSIDE   (AC_STATUSA_WSTATE0_INSIDE_Val << AC_STATUSA_WSTATE0_Pos)
#define AC_STATUSA_WSTATE0_BELOW    (AC_STATUSA_WSTATE0_BELOW_Val  << AC_STATUSA_WSTATE0_Pos)
#define AC_STATUSA_MASK             _U(0x33)     /**< \brief (AC_STATUSA) MASK Register */

/* -------- AC_STATUSB : (AC Offset: 0x09) (R/   8) Status B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  READY0:1;         /*!< bit:      0  Comparator 0 Ready                 */
    uint8_t  READY1:1;         /*!< bit:      1  Comparator 1 Ready                 */
    uint8_t  :5;               /*!< bit:  2.. 6  Reserved                           */
    uint8_t  SYNCBUSY:1;       /*!< bit:      7  Synchronization Busy               */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  READY:2;          /*!< bit:  0.. 1  Comparator x Ready                 */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} AC_STATUSB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AC_STATUSB_OFFSET           0x09         /**< \brief (AC_STATUSB offset) Status B */
#define AC_STATUSB_RESETVALUE       _U(0x00)     /**< \brief (AC_STATUSB reset_value) Status B */

#define AC_STATUSB_READY0_Pos       0            /**< \brief (AC_STATUSB) Comparator 0 Ready */
#define AC_STATUSB_READY0           (1 << AC_STATUSB_READY0_Pos)
#define AC_STATUSB_READY1_Pos       1            /**< \brief (AC_STATUSB) Comparator 1 Ready */
#define AC_STATUSB_READY1           (1 << AC_STATUSB_READY1_Pos)
#define AC_STATUSB_READY_Pos        0            /**< \brief (AC_STATUSB) Comparator x Ready */
#define AC_STATUSB_READY_Msk        (_U(0x3) << AC_STATUSB_READY_Pos)
#define AC_STATUSB_READY(value)     (AC_STATUSB_READY_Msk & ((value) << AC_STATUSB_READY_Pos))
#define AC_STATUSB_SYNCBUSY_Pos     7            /**< \brief (AC_STATUSB) Synchronization Busy */
#define AC_STATUSB_SYNCBUSY         (_U(0x1) << AC_STATUSB_SYNCBUSY_Pos)
#define AC_STATUSB_MASK             _U(0x83)     /**< \brief (AC_STATUSB) MASK Register */

/* -------- AC_STATUSC : (AC Offset: 0x0A) (R/   8) Status C -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  STATE0:1;         /*!< bit:      0  Comparator 0 Current State         */
    uint8_t  STATE1:1;         /*!< bit:      1  Comparator 1 Current State         */
    uint8_t  :2;               /*!< bit:  2.. 3  Reserved                           */
    uint8_t  WSTATE0:2;        /*!< bit:  4.. 5  Window 0 Current State             */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  STATE:2;          /*!< bit:  0.. 1  Comparator x Current State         */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} AC_STATUSC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AC_STATUSC_OFFSET           0x0A         /**< \brief (AC_STATUSC offset) Status C */
#define AC_STATUSC_RESETVALUE       _U(0x00)     /**< \brief (AC_STATUSC reset_value) Status C */

#define AC_STATUSC_STATE0_Pos       0            /**< \brief (AC_STATUSC) Comparator 0 Current State */
#define AC_STATUSC_STATE0           (1 << AC_STATUSC_STATE0_Pos)
#define AC_STATUSC_STATE1_Pos       1            /**< \brief (AC_STATUSC) Comparator 1 Current State */
#define AC_STATUSC_STATE1           (1 << AC_STATUSC_STATE1_Pos)
#define AC_STATUSC_STATE_Pos        0            /**< \brief (AC_STATUSC) Comparator x Current State */
#define AC_STATUSC_STATE_Msk        (_U(0x3) << AC_STATUSC_STATE_Pos)
#define AC_STATUSC_STATE(value)     (AC_STATUSC_STATE_Msk & ((value) << AC_STATUSC_STATE_Pos))
#define AC_STATUSC_WSTATE0_Pos      4            /**< \brief (AC_STATUSC) Window 0 Current State */
#define AC_STATUSC_WSTATE0_Msk      (_U(0x3) << AC_STATUSC_WSTATE0_Pos)
#define AC_STATUSC_WSTATE0(value)   (AC_STATUSC_WSTATE0_Msk & ((value) << AC_STATUSC_WSTATE0_Pos))
#define   AC_STATUSC_WSTATE0_ABOVE_Val    _U(0x0)   /**< \brief (AC_STATUSC) Signal is above window */
#define   AC_STATUSC_WSTATE0_INSIDE_Val   _U(0x1)   /**< \brief (AC_STATUSC) Signal is inside window */
#define   AC_STATUSC_WSTATE0_BELOW_Val    _U(0x2)   /**< \brief (AC_STATUSC) Signal is below window */
#define AC_STATUSC_WSTATE0_ABOVE    (AC_STATUSC_WSTATE0_ABOVE_Val  << AC_STATUSC_WSTATE0_Pos)
#define AC_STATUSC_WSTATE0_INSIDE   (AC_STATUSC_WSTATE0_INSIDE_Val << AC_STATUSC_WSTATE0_Pos)
#define AC_STATUSC_WSTATE0_BELOW    (AC_STATUSC_WSTATE0_BELOW_Val  << AC_STATUSC_WSTATE0_Pos)
#define AC_STATUSC_MASK             _U(0x33)     /**< \brief (AC_STATUSC) MASK Register */

/* -------- AC_WINCTRL : (AC Offset: 0x0C) (R/W  8) Window Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  WEN0:1;           /*!< bit:      0  Window 0 Mode Enable               */
    uint8_t  WINTSEL0:2;       /*!< bit:  1.. 2  Window 0 Interrupt Selection       */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} AC_WINCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AC_WINCTRL_OFFSET           0x0C         /**< \brief (AC_WINCTRL offset) Window Control */
#define AC_WINCTRL_RESETVALUE       _U(0x00)     /**< \brief (AC_WINCTRL reset_value) Window Control */

#define AC_WINCTRL_WEN0_Pos         0            /**< \brief (AC_WINCTRL) Window 0 Mode Enable */
#define AC_WINCTRL_WEN0             (_U(0x1) << AC_WINCTRL_WEN0_Pos)
#define AC_WINCTRL_WINTSEL0_Pos     1            /**< \brief (AC_WINCTRL) Window 0 Interrupt Selection */
#define AC_WINCTRL_WINTSEL0_Msk     (_U(0x3) << AC_WINCTRL_WINTSEL0_Pos)
#define AC_WINCTRL_WINTSEL0(value)  (AC_WINCTRL_WINTSEL0_Msk & ((value) << AC_WINCTRL_WINTSEL0_Pos))
#define   AC_WINCTRL_WINTSEL0_ABOVE_Val   _U(0x0)   /**< \brief (AC_WINCTRL) Interrupt on signal above window */
#define   AC_WINCTRL_WINTSEL0_INSIDE_Val  _U(0x1)   /**< \brief (AC_WINCTRL) Interrupt on signal inside window */
#define   AC_WINCTRL_WINTSEL0_BELOW_Val   _U(0x2)   /**< \brief (AC_WINCTRL) Interrupt on signal below window */
#define   AC_WINCTRL_WINTSEL0_OUTSIDE_Val _U(0x3)   /**< \brief (AC_WINCTRL) Interrupt on signal outside window */
#define AC_WINCTRL_WINTSEL0_ABOVE   (AC_WINCTRL_WINTSEL0_ABOVE_Val << AC_WINCTRL_WINTSEL0_Pos)
#define AC_WINCTRL_WINTSEL0_INSIDE  (AC_WINCTRL_WINTSEL0_INSIDE_Val << AC_WINCTRL_WINTSEL0_Pos)
#define AC_WINCTRL_WINTSEL0_BELOW   (AC_WINCTRL_WINTSEL0_BELOW_Val << AC_WINCTRL_WINTSEL0_Pos)
#define AC_WINCTRL_WINTSEL0_OUTSIDE (AC_WINCTRL_WINTSEL0_OUTSIDE_Val << AC_WINCTRL_WINTSEL0_Pos)
#define AC_WINCTRL_MASK             _U(0x07)     /**< \brief (AC_WINCTRL) MASK Register */

/* -------- AC_COMPCTRL : (AC Offset: 0x10) (R/W 32) Comparator Control n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ENABLE:1;         /*!< bit:      0  Enable                             */
    uint32_t SINGLE:1;         /*!< bit:      1  Single-Shot Mode                   */
    uint32_t SPEED:2;          /*!< bit:  2.. 3  Speed Selection                    */
    uint32_t :1;               /*!< bit:      4  Reserved                           */
    uint32_t INTSEL:2;         /*!< bit:  5.. 6  Interrupt Selection                */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t MUXNEG:3;         /*!< bit:  8..10  Negative Input Mux Selection       */
    uint32_t :1;               /*!< bit:     11  Reserved                           */
    uint32_t MUXPOS:2;         /*!< bit: 12..13  Positive Input Mux Selection       */
    uint32_t :1;               /*!< bit:     14  Reserved                           */
    uint32_t SWAP:1;           /*!< bit:     15  Swap Inputs and Invert             */
    uint32_t OUT:2;            /*!< bit: 16..17  Output                             */
    uint32_t :1;               /*!< bit:     18  Reserved                           */
    uint32_t HYST:1;           /*!< bit:     19  Hysteresis Enable                  */
    uint32_t :4;               /*!< bit: 20..23  Reserved                           */
    uint32_t FLEN:3;           /*!< bit: 24..26  Filter Length                      */
    uint32_t :5;               /*!< bit: 27..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} AC_COMPCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AC_COMPCTRL_OFFSET          0x10         /**< \brief (AC_COMPCTRL offset) Comparator Control n */
#define AC_COMPCTRL_RESETVALUE      _U(0x00000000) /**< \brief (AC_COMPCTRL reset_value) Comparator Control n */

#define AC_COMPCTRL_ENABLE_Pos      0            /**< \brief (AC_COMPCTRL) Enable */
#define AC_COMPCTRL_ENABLE          (_U(0x1) << AC_COMPCTRL_ENABLE_Pos)
#define AC_COMPCTRL_SINGLE_Pos      1            /**< \brief (AC_COMPCTRL) Single-Shot Mode */
#define AC_COMPCTRL_SINGLE          (_U(0x1) << AC_COMPCTRL_SINGLE_Pos)
#define AC_COMPCTRL_SPEED_Pos       2            /**< \brief (AC_COMPCTRL) Speed Selection */
#define AC_COMPCTRL_SPEED_Msk       (_U(0x3) << AC_COMPCTRL_SPEED_Pos)
#define AC_COMPCTRL_SPEED(value)    (AC_COMPCTRL_SPEED_Msk & ((value) << AC_COMPCTRL_SPEED_Pos))
#define   AC_COMPCTRL_SPEED_LOW_Val       _U(0x0)   /**< \brief (AC_COMPCTRL) Low speed */
#define   AC_COMPCTRL_SPEED_HIGH_Val      _U(0x1)   /**< \brief (AC_COMPCTRL) High speed */
#define AC_COMPCTRL_SPEED_LOW       (AC_COMPCTRL_SPEED_LOW_Val     << AC_COMPCTRL_SPEED_Pos)
#define AC_COMPCTRL_SPEED_HIGH      (AC_COMPCTRL_SPEED_HIGH_Val    << AC_COMPCTRL_SPEED_Pos)
#define AC_COMPCTRL_INTSEL_Pos      5            /**< \brief (AC_COMPCTRL) Interrupt Selection */
#define AC_COMPCTRL_INTSEL_Msk      (_U(0x3) << AC_COMPCTRL_INTSEL_Pos)
#define AC_COMPCTRL_INTSEL(value)   (AC_COMPCTRL_INTSEL_Msk & ((value) << AC_COMPCTRL_INTSEL_Pos))
#define   AC_COMPCTRL_INTSEL_TOGGLE_Val   _U(0x0)   /**< \brief (AC_COMPCTRL) Interrupt on comparator output toggle */
#define   AC_COMPCTRL_INTSEL_RISING_Val   _U(0x1)   /**< \brief (AC_COMPCTRL) Interrupt on comparator output rising */
#define   AC_COMPCTRL_INTSEL_FALLING_Val  _U(0x2)   /**< \brief (AC_COMPCTRL) Interrupt on comparator output falling */
#define   AC_COMPCTRL_INTSEL_EOC_Val      _U(0x3)   /**< \brief (AC_COMPCTRL) Interrupt on end of comparison (single-shot mode only) */
#define AC_COMPCTRL_INTSEL_TOGGLE   (AC_COMPCTRL_INTSEL_TOGGLE_Val << AC_COMPCTRL_INTSEL_Pos)
#define AC_COMPCTRL_INTSEL_RISING   (AC_COMPCTRL_INTSEL_RISING_Val << AC_COMPCTRL_INTSEL_Pos)
#define AC_COMPCTRL_INTSEL_FALLING  (AC_COMPCTRL_INTSEL_FALLING_Val << AC_COMPCTRL_INTSEL_Pos)
#define AC_COMPCTRL_INTSEL_EOC      (AC_COMPCTRL_INTSEL_EOC_Val    << AC_COMPCTRL_INTSEL_Pos)
#define AC_COMPCTRL_MUXNEG_Pos      8            /**< \brief (AC_COMPCTRL) Negative Input Mux Selection */
#define AC_COMPCTRL_MUXNEG_Msk      (_U(0x7) << AC_COMPCTRL_MUXNEG_Pos)
#define AC_COMPCTRL_MUXNEG(value)   (AC_COMPCTRL_MUXNEG_Msk & ((value) << AC_COMPCTRL_MUXNEG_Pos))
#define   AC_COMPCTRL_MUXNEG_PIN0_Val     _U(0x0)   /**< \brief (AC_COMPCTRL) I/O pin 0 */
#define   AC_COMPCTRL_MUXNEG_PIN1_Val     _U(0x1)   /**< \brief (AC_COMPCTRL) I/O pin 1 */
#define   AC_COMPCTRL_MUXNEG_PIN2_Val     _U(0x2)   /**< \brief (AC_COMPCTRL) I/O pin 2 */
#define   AC_COMPCTRL_MUXNEG_PIN3_Val     _U(0x3)   /**< \brief (AC_COMPCTRL) I/O pin 3 */
#define   AC_COMPCTRL_MUXNEG_GND_Val      _U(0x4)   /**< \brief (AC_COMPCTRL) Ground */
#define   AC_COMPCTRL_MUXNEG_VSCALE_Val   _U(0x5)   /**< \brief (AC_COMPCTRL) VDD scaler */
#define   AC_COMPCTRL_MUXNEG_BANDGAP_Val  _U(0x6)   /**< \brief (AC_COMPCTRL) Internal bandgap voltage */
#define   AC_COMPCTRL_MUXNEG_DAC_Val      _U(0x7)   /**< \brief (AC_COMPCTRL) DAC output */
#define AC_COMPCTRL_MUXNEG_PIN0     (AC_COMPCTRL_MUXNEG_PIN0_Val   << AC_COMPCTRL_MUXNEG_Pos)
#define AC_COMPCTRL_MUXNEG_PIN1     (AC_COMPCTRL_MUXNEG_PIN1_Val   << AC_COMPCTRL_MUXNEG_Pos)
#define AC_COMPCTRL_MUXNEG_PIN2     (AC_COMPCTRL_MUXNEG_PIN2_Val   << AC_COMPCTRL_MUXNEG_Pos)
#define AC_COMPCTRL_MUXNEG_PIN3     (AC_COMPCTRL_MUXNEG_PIN3_Val   << AC_COMPCTRL_MUXNEG_Pos)
#define AC_COMPCTRL_MUXNEG_GND      (AC_COMPCTRL_MUXNEG_GND_Val    << AC_COMPCTRL_MUXNEG_Pos)
#define AC_COMPCTRL_MUXNEG_VSCALE   (AC_COMPCTRL_MUXNEG_VSCALE_Val << AC_COMPCTRL_MUXNEG_Pos)
#define AC_COMPCTRL_MUXNEG_BANDGAP  (AC_COMPCTRL_MUXNEG_BANDGAP_Val << AC_COMPCTRL_MUXNEG_Pos)
#define AC_COMPCTRL_MUXNEG_DAC      (AC_COMPCTRL_MUXNEG_DAC_Val    << AC_COMPCTRL_MUXNEG_Pos)
#define AC_COMPCTRL_MUXPOS_Pos      12           /**< \brief (AC_COMPCTRL) Positive Input Mux Selection */
#define AC_COMPCTRL_MUXPOS_Msk      (_U(0x3) << AC_COMPCTRL_MUXPOS_Pos)
#define AC_COMPCTRL_MUXPOS(value)   (AC_COMPCTRL_MUXPOS_Msk & ((value) << AC_COMPCTRL_MUXPOS_Pos))
#define   AC_COMPCTRL_MUXPOS_PIN0_Val     _U(0x0)   /**< \brief (AC_COMPCTRL) I/O pin 0 */
#define   AC_COMPCTRL_MUXPOS_PIN1_Val     _U(0x1)   /**< \brief (AC_COMPCTRL) I/O pin 1 */
#define   AC_COMPCTRL_MUXPOS_PIN2_Val     _U(0x2)   /**< \brief (AC_COMPCTRL) I/O pin 2 */
#define   AC_COMPCTRL_MUXPOS_PIN3_Val     _U(0x3)   /**< \brief (AC_COMPCTRL) I/O pin 3 */
#define AC_COMPCTRL_MUXPOS_PIN0     (AC_COMPCTRL_MUXPOS_PIN0_Val   << AC_COMPCTRL_MUXPOS_Pos)
#define AC_COMPCTRL_MUXPOS_PIN1     (AC_COMPCTRL_MUXPOS_PIN1_Val   << AC_COMPCTRL_MUXPOS_Pos)
#define AC_COMPCTRL_MUXPOS_PIN2     (AC_COMPCTRL_MUXPOS_PIN2_Val   << AC_COMPCTRL_MUXPOS_Pos)
#define AC_COMPCTRL_MUXPOS_PIN3     (AC_COMPCTRL_MUXPOS_PIN3_Val   << AC_COMPCTRL_MUXPOS_Pos)
#define AC_COMPCTRL_SWAP_Pos        15           /**< \brief (AC_COMPCTRL) Swap Inputs and Invert */
#define AC_COMPCTRL_SWAP            (_U(0x1) << AC_COMPCTRL_SWAP_Pos)
#define AC_COMPCTRL_OUT_Pos         16           /**< \brief (AC_COMPCTRL) Output */
#define AC_COMPCTRL_OUT_Msk         (_U(0x3) << AC_COMPCTRL_OUT_Pos)
#define AC_COMPCTRL_OUT(value)      (AC_COMPCTRL_OUT_Msk & ((value) << AC_COMPCTRL_OUT_Pos))
#define   AC_COMPCTRL_OUT_OFF_Val         _U(0x0)   /**< \brief (AC_COMPCTRL) The output of COMPn is not routed to the COMPn I/O port */
#define   AC_COMPCTRL_OUT_ASYNC_Val       _U(0x1)   /**< \brief (AC_COMPCTRL) The asynchronous output of COMPn is routed to the COMPn I/O port */
#define   AC_COMPCTRL_OUT_SYNC_Val        _U(0x2)   /**< \brief (AC_COMPCTRL) The synchronous output (including filtering) of COMPn is routed to the COMPn I/O port */
#define AC_COMPCTRL_OUT_OFF         (AC_COMPCTRL_OUT_OFF_Val       << AC_COMPCTRL_OUT_Pos)
#define AC_COMPCTRL_OUT_ASYNC       (AC_COMPCTRL_OUT_ASYNC_Val     << AC_COMPCTRL_OUT_Pos)
#define AC_COMPCTRL_OUT_SYNC        (AC_COMPCTRL_OUT_SYNC_Val      << AC_COMPCTRL_OUT_Pos)
#define AC_COMPCTRL_HYST_Pos        19           /**< \brief (AC_COMPCTRL) Hysteresis Enable */
#define AC_COMPCTRL_HYST            (_U(0x1) << AC_COMPCTRL_HYST_Pos)
#define AC_COMPCTRL_FLEN_Pos        24           /**< \brief (AC_COMPCTRL) Filter Length */
#define AC_COMPCTRL_FLEN_Msk        (_U(0x7) << AC_COMPCTRL_FLEN_Pos)
#define AC_COMPCTRL_FLEN(value)     (AC_COMPCTRL_FLEN_Msk & ((value) << AC_COMPCTRL_FLEN_Pos))
#define   AC_COMPCTRL_FLEN_OFF_Val        _U(0x0)   /**< \brief (AC_COMPCTRL) No filtering */
#define   AC_COMPCTRL_FLEN_MAJ3_Val       _U(0x1)   /**< \brief (AC_COMPCTRL) 3-bit majority function (2 of 3) */
#define   AC_COMPCTRL_FLEN_MAJ5_Val       _U(0x2)   /**< \brief (AC_COMPCTRL) 5-bit majority function (3 of 5) */
#define AC_COMPCTRL_FLEN_OFF        (AC_COMPCTRL_FLEN_OFF_Val      << AC_COMPCTRL_FLEN_Pos)
#define AC_COMPCTRL_FLEN_MAJ3       (AC_COMPCTRL_FLEN_MAJ3_Val     << AC_COMPCTRL_FLEN_Pos)
#define AC_COMPCTRL_FLEN_MAJ5       (AC_COMPCTRL_FLEN_MAJ5_Val     << AC_COMPCTRL_FLEN_Pos)
#define AC_COMPCTRL_MASK            _U(0x070BB76F) /**< \brief (AC_COMPCTRL) MASK Register */

/* -------- AC_SCALER : (AC Offset: 0x20) (R/W  8) Scaler n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  VALUE:6;          /*!< bit:  0.. 5  Scaler Value                       */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} AC_SCALER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define AC_SCALER_OFFSET            0x20         /**< \brief (AC_SCALER offset) Scaler n */
#define AC_SCALER_RESETVALUE        _U(0x00)     /**< \brief (AC_SCALER reset_value) Scaler n */

#define AC_SCALER_VALUE_Pos         0            /**< \brief (AC_SCALER) Scaler Value */
#define AC_SCALER_VALUE_Msk         (_U(0x3F) << AC_SCALER_VALUE_Pos)
#define AC_SCALER_VALUE(value)      (AC_SCALER_VALUE_Msk & ((value) << AC_SCALER_VALUE_Pos))
#define AC_SCALER_MASK              _U(0x3F)     /**< \brief (AC_SCALER) MASK Register */

/** \brief AC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO AC_CTRLA_Type             CTRLA;       /**< \brief Offset: 0x00 (R/W  8) Control A */
  __O  AC_CTRLB_Type             CTRLB;       /**< \brief Offset: 0x01 ( /W  8) Control B */
  __IO AC_EVCTRL_Type            EVCTRL;      /**< \brief Offset: 0x02 (R/W 16) Event Control */
  __IO AC_INTENCLR_Type          INTENCLR;    /**< \brief Offset: 0x04 (R/W  8) Interrupt Enable Clear */
  __IO AC_INTENSET_Type          INTENSET;    /**< \brief Offset: 0x05 (R/W  8) Interrupt Enable Set */
  __IO AC_INTFLAG_Type           INTFLAG;     /**< \brief Offset: 0x06 (R/W  8) Interrupt Flag Status and Clear */
       RoReg8                    Reserved1[0x1];
  __I  AC_STATUSA_Type           STATUSA;     /**< \brief Offset: 0x08 (R/   8) Status A */
  __I  AC_STATUSB_Type           STATUSB;     /**< \brief Offset: 0x09 (R/   8) Status B */
  __I  AC_STATUSC_Type           STATUSC;     /**< \brief Offset: 0x0A (R/   8) Status C */
       RoReg8                    Reserved2[0x1];
  __IO AC_WINCTRL_Type           WINCTRL;     /**< \brief Offset: 0x0C (R/W  8) Window Control */
       RoReg8                    Reserved3[0x3];
  __IO AC_COMPCTRL_Type          COMPCTRL[2]; /**< \brief Offset: 0x10 (R/W 32) Comparator Control n */
       RoReg8                    Reserved4[0x8];
  __IO AC_SCALER_Type            SCALER[2];   /**< \brief Offset: 0x20 (R/W  8) Scaler n */
} Ac;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD21_AC_COMPONENT_ */
