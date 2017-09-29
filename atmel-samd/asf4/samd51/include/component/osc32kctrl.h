/**
 * \file
 *
 * \brief Component description for OSC32KCTRL
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

#ifndef _SAMD51_OSC32KCTRL_COMPONENT_
#define _SAMD51_OSC32KCTRL_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR OSC32KCTRL */
/* ========================================================================== */
/** \addtogroup SAMD51_OSC32KCTRL 32kHz Oscillators Control */
/*@{*/

#define OSC32KCTRL_U2400
#define REV_OSC32KCTRL              0x100

/* -------- OSC32KCTRL_INTENCLR : (OSC32KCTRL Offset: 0x00) (R/W 32) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t XOSC32KRDY:1;     /*!< bit:      0  XOSC32K Ready Interrupt Enable     */
    uint32_t :1;               /*!< bit:      1  Reserved                           */
    uint32_t XOSC32KFAIL:1;    /*!< bit:      2  XOSC32K Clock Failure Detector Interrupt Enable */
    uint32_t :29;              /*!< bit:  3..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OSC32KCTRL_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSC32KCTRL_INTENCLR_OFFSET  0x00         /**< \brief (OSC32KCTRL_INTENCLR offset) Interrupt Enable Clear */
#define OSC32KCTRL_INTENCLR_RESETVALUE _U_(0x00000000) /**< \brief (OSC32KCTRL_INTENCLR reset_value) Interrupt Enable Clear */

#define OSC32KCTRL_INTENCLR_XOSC32KRDY_Pos 0            /**< \brief (OSC32KCTRL_INTENCLR) XOSC32K Ready Interrupt Enable */
#define OSC32KCTRL_INTENCLR_XOSC32KRDY (_U_(0x1) << OSC32KCTRL_INTENCLR_XOSC32KRDY_Pos)
#define OSC32KCTRL_INTENCLR_XOSC32KFAIL_Pos 2            /**< \brief (OSC32KCTRL_INTENCLR) XOSC32K Clock Failure Detector Interrupt Enable */
#define OSC32KCTRL_INTENCLR_XOSC32KFAIL (_U_(0x1) << OSC32KCTRL_INTENCLR_XOSC32KFAIL_Pos)
#define OSC32KCTRL_INTENCLR_MASK    _U_(0x00000005) /**< \brief (OSC32KCTRL_INTENCLR) MASK Register */

/* -------- OSC32KCTRL_INTENSET : (OSC32KCTRL Offset: 0x04) (R/W 32) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t XOSC32KRDY:1;     /*!< bit:      0  XOSC32K Ready Interrupt Enable     */
    uint32_t :1;               /*!< bit:      1  Reserved                           */
    uint32_t XOSC32KFAIL:1;    /*!< bit:      2  XOSC32K Clock Failure Detector Interrupt Enable */
    uint32_t :29;              /*!< bit:  3..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OSC32KCTRL_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSC32KCTRL_INTENSET_OFFSET  0x04         /**< \brief (OSC32KCTRL_INTENSET offset) Interrupt Enable Set */
#define OSC32KCTRL_INTENSET_RESETVALUE _U_(0x00000000) /**< \brief (OSC32KCTRL_INTENSET reset_value) Interrupt Enable Set */

#define OSC32KCTRL_INTENSET_XOSC32KRDY_Pos 0            /**< \brief (OSC32KCTRL_INTENSET) XOSC32K Ready Interrupt Enable */
#define OSC32KCTRL_INTENSET_XOSC32KRDY (_U_(0x1) << OSC32KCTRL_INTENSET_XOSC32KRDY_Pos)
#define OSC32KCTRL_INTENSET_XOSC32KFAIL_Pos 2            /**< \brief (OSC32KCTRL_INTENSET) XOSC32K Clock Failure Detector Interrupt Enable */
#define OSC32KCTRL_INTENSET_XOSC32KFAIL (_U_(0x1) << OSC32KCTRL_INTENSET_XOSC32KFAIL_Pos)
#define OSC32KCTRL_INTENSET_MASK    _U_(0x00000005) /**< \brief (OSC32KCTRL_INTENSET) MASK Register */

/* -------- OSC32KCTRL_INTFLAG : (OSC32KCTRL Offset: 0x08) (R/W 32) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t XOSC32KRDY:1;     /*!< bit:      0  XOSC32K Ready                      */
    __I uint32_t :1;               /*!< bit:      1  Reserved                           */
    __I uint32_t XOSC32KFAIL:1;    /*!< bit:      2  XOSC32K Clock Failure Detector     */
    __I uint32_t :29;              /*!< bit:  3..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OSC32KCTRL_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSC32KCTRL_INTFLAG_OFFSET   0x08         /**< \brief (OSC32KCTRL_INTFLAG offset) Interrupt Flag Status and Clear */
#define OSC32KCTRL_INTFLAG_RESETVALUE _U_(0x00000000) /**< \brief (OSC32KCTRL_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define OSC32KCTRL_INTFLAG_XOSC32KRDY_Pos 0            /**< \brief (OSC32KCTRL_INTFLAG) XOSC32K Ready */
#define OSC32KCTRL_INTFLAG_XOSC32KRDY (_U_(0x1) << OSC32KCTRL_INTFLAG_XOSC32KRDY_Pos)
#define OSC32KCTRL_INTFLAG_XOSC32KFAIL_Pos 2            /**< \brief (OSC32KCTRL_INTFLAG) XOSC32K Clock Failure Detector */
#define OSC32KCTRL_INTFLAG_XOSC32KFAIL (_U_(0x1) << OSC32KCTRL_INTFLAG_XOSC32KFAIL_Pos)
#define OSC32KCTRL_INTFLAG_MASK     _U_(0x00000005) /**< \brief (OSC32KCTRL_INTFLAG) MASK Register */

/* -------- OSC32KCTRL_STATUS : (OSC32KCTRL Offset: 0x0C) (R/  32) Power and Clocks Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t XOSC32KRDY:1;     /*!< bit:      0  XOSC32K Ready                      */
    uint32_t :1;               /*!< bit:      1  Reserved                           */
    uint32_t XOSC32KFAIL:1;    /*!< bit:      2  XOSC32K Clock Failure Detector     */
    uint32_t XOSC32KSW:1;      /*!< bit:      3  XOSC32K Clock switch               */
    uint32_t :28;              /*!< bit:  4..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OSC32KCTRL_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSC32KCTRL_STATUS_OFFSET    0x0C         /**< \brief (OSC32KCTRL_STATUS offset) Power and Clocks Status */
#define OSC32KCTRL_STATUS_RESETVALUE _U_(0x00000000) /**< \brief (OSC32KCTRL_STATUS reset_value) Power and Clocks Status */

#define OSC32KCTRL_STATUS_XOSC32KRDY_Pos 0            /**< \brief (OSC32KCTRL_STATUS) XOSC32K Ready */
#define OSC32KCTRL_STATUS_XOSC32KRDY (_U_(0x1) << OSC32KCTRL_STATUS_XOSC32KRDY_Pos)
#define OSC32KCTRL_STATUS_XOSC32KFAIL_Pos 2            /**< \brief (OSC32KCTRL_STATUS) XOSC32K Clock Failure Detector */
#define OSC32KCTRL_STATUS_XOSC32KFAIL (_U_(0x1) << OSC32KCTRL_STATUS_XOSC32KFAIL_Pos)
#define OSC32KCTRL_STATUS_XOSC32KSW_Pos 3            /**< \brief (OSC32KCTRL_STATUS) XOSC32K Clock switch */
#define OSC32KCTRL_STATUS_XOSC32KSW (_U_(0x1) << OSC32KCTRL_STATUS_XOSC32KSW_Pos)
#define OSC32KCTRL_STATUS_MASK      _U_(0x0000000D) /**< \brief (OSC32KCTRL_STATUS) MASK Register */

/* -------- OSC32KCTRL_RTCCTRL : (OSC32KCTRL Offset: 0x10) (R/W  8) RTC Clock Selection -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  RTCSEL:3;         /*!< bit:  0.. 2  RTC Clock Selection                */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} OSC32KCTRL_RTCCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSC32KCTRL_RTCCTRL_OFFSET   0x10         /**< \brief (OSC32KCTRL_RTCCTRL offset) RTC Clock Selection */
#define OSC32KCTRL_RTCCTRL_RESETVALUE _U_(0x00)    /**< \brief (OSC32KCTRL_RTCCTRL reset_value) RTC Clock Selection */

#define OSC32KCTRL_RTCCTRL_RTCSEL_Pos 0            /**< \brief (OSC32KCTRL_RTCCTRL) RTC Clock Selection */
#define OSC32KCTRL_RTCCTRL_RTCSEL_Msk (_U_(0x7) << OSC32KCTRL_RTCCTRL_RTCSEL_Pos)
#define OSC32KCTRL_RTCCTRL_RTCSEL(value) (OSC32KCTRL_RTCCTRL_RTCSEL_Msk & ((value) << OSC32KCTRL_RTCCTRL_RTCSEL_Pos))
#define   OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K_Val _U_(0x0)   /**< \brief (OSC32KCTRL_RTCCTRL) 1.024kHz from 32kHz internal ULP oscillator */
#define   OSC32KCTRL_RTCCTRL_RTCSEL_ULP32K_Val _U_(0x1)   /**< \brief (OSC32KCTRL_RTCCTRL) 32.768kHz from 32kHz internal ULP oscillator */
#define   OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K_Val _U_(0x4)   /**< \brief (OSC32KCTRL_RTCCTRL) 1.024kHz from 32.768kHz internal oscillator */
#define   OSC32KCTRL_RTCCTRL_RTCSEL_XOSC32K_Val _U_(0x5)   /**< \brief (OSC32KCTRL_RTCCTRL) 32.768kHz from 32.768kHz external crystal oscillator */
#define OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K (OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K_Val << OSC32KCTRL_RTCCTRL_RTCSEL_Pos)
#define OSC32KCTRL_RTCCTRL_RTCSEL_ULP32K (OSC32KCTRL_RTCCTRL_RTCSEL_ULP32K_Val << OSC32KCTRL_RTCCTRL_RTCSEL_Pos)
#define OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K (OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K_Val << OSC32KCTRL_RTCCTRL_RTCSEL_Pos)
#define OSC32KCTRL_RTCCTRL_RTCSEL_XOSC32K (OSC32KCTRL_RTCCTRL_RTCSEL_XOSC32K_Val << OSC32KCTRL_RTCCTRL_RTCSEL_Pos)
#define OSC32KCTRL_RTCCTRL_MASK     _U_(0x07)    /**< \brief (OSC32KCTRL_RTCCTRL) MASK Register */

/* -------- OSC32KCTRL_XOSC32K : (OSC32KCTRL Offset: 0x14) (R/W 16) 32kHz External Crystal Oscillator (XOSC32K) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t :1;               /*!< bit:      0  Reserved                           */
    uint16_t ENABLE:1;         /*!< bit:      1  Oscillator Enable                  */
    uint16_t XTALEN:1;         /*!< bit:      2  Crystal Oscillator Enable          */
    uint16_t EN32K:1;          /*!< bit:      3  32kHz Output Enable                */
    uint16_t EN1K:1;           /*!< bit:      4  1kHz Output Enable                 */
    uint16_t :1;               /*!< bit:      5  Reserved                           */
    uint16_t RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint16_t ONDEMAND:1;       /*!< bit:      7  On Demand Control                  */
    uint16_t STARTUP:3;        /*!< bit:  8..10  Oscillator Start-Up Time           */
    uint16_t :1;               /*!< bit:     11  Reserved                           */
    uint16_t WRTLOCK:1;        /*!< bit:     12  Write Lock                         */
    uint16_t CGM:2;            /*!< bit: 13..14  Control Gain Mode                  */
    uint16_t :1;               /*!< bit:     15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} OSC32KCTRL_XOSC32K_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSC32KCTRL_XOSC32K_OFFSET   0x14         /**< \brief (OSC32KCTRL_XOSC32K offset) 32kHz External Crystal Oscillator (XOSC32K) Control */
#define OSC32KCTRL_XOSC32K_RESETVALUE _U_(0x2080)  /**< \brief (OSC32KCTRL_XOSC32K reset_value) 32kHz External Crystal Oscillator (XOSC32K) Control */

#define OSC32KCTRL_XOSC32K_ENABLE_Pos 1            /**< \brief (OSC32KCTRL_XOSC32K) Oscillator Enable */
#define OSC32KCTRL_XOSC32K_ENABLE   (_U_(0x1) << OSC32KCTRL_XOSC32K_ENABLE_Pos)
#define OSC32KCTRL_XOSC32K_XTALEN_Pos 2            /**< \brief (OSC32KCTRL_XOSC32K) Crystal Oscillator Enable */
#define OSC32KCTRL_XOSC32K_XTALEN   (_U_(0x1) << OSC32KCTRL_XOSC32K_XTALEN_Pos)
#define OSC32KCTRL_XOSC32K_EN32K_Pos 3            /**< \brief (OSC32KCTRL_XOSC32K) 32kHz Output Enable */
#define OSC32KCTRL_XOSC32K_EN32K    (_U_(0x1) << OSC32KCTRL_XOSC32K_EN32K_Pos)
#define OSC32KCTRL_XOSC32K_EN1K_Pos 4            /**< \brief (OSC32KCTRL_XOSC32K) 1kHz Output Enable */
#define OSC32KCTRL_XOSC32K_EN1K     (_U_(0x1) << OSC32KCTRL_XOSC32K_EN1K_Pos)
#define OSC32KCTRL_XOSC32K_RUNSTDBY_Pos 6            /**< \brief (OSC32KCTRL_XOSC32K) Run in Standby */
#define OSC32KCTRL_XOSC32K_RUNSTDBY (_U_(0x1) << OSC32KCTRL_XOSC32K_RUNSTDBY_Pos)
#define OSC32KCTRL_XOSC32K_ONDEMAND_Pos 7            /**< \brief (OSC32KCTRL_XOSC32K) On Demand Control */
#define OSC32KCTRL_XOSC32K_ONDEMAND (_U_(0x1) << OSC32KCTRL_XOSC32K_ONDEMAND_Pos)
#define OSC32KCTRL_XOSC32K_STARTUP_Pos 8            /**< \brief (OSC32KCTRL_XOSC32K) Oscillator Start-Up Time */
#define OSC32KCTRL_XOSC32K_STARTUP_Msk (_U_(0x7) << OSC32KCTRL_XOSC32K_STARTUP_Pos)
#define OSC32KCTRL_XOSC32K_STARTUP(value) (OSC32KCTRL_XOSC32K_STARTUP_Msk & ((value) << OSC32KCTRL_XOSC32K_STARTUP_Pos))
#define OSC32KCTRL_XOSC32K_WRTLOCK_Pos 12           /**< \brief (OSC32KCTRL_XOSC32K) Write Lock */
#define OSC32KCTRL_XOSC32K_WRTLOCK  (_U_(0x1) << OSC32KCTRL_XOSC32K_WRTLOCK_Pos)
#define OSC32KCTRL_XOSC32K_CGM_Pos  13           /**< \brief (OSC32KCTRL_XOSC32K) Control Gain Mode */
#define OSC32KCTRL_XOSC32K_CGM_Msk  (_U_(0x3) << OSC32KCTRL_XOSC32K_CGM_Pos)
#define OSC32KCTRL_XOSC32K_CGM(value) (OSC32KCTRL_XOSC32K_CGM_Msk & ((value) << OSC32KCTRL_XOSC32K_CGM_Pos))
#define   OSC32KCTRL_XOSC32K_CGM_XT_Val   _U_(0x1)   /**< \brief (OSC32KCTRL_XOSC32K) Standard mode */
#define   OSC32KCTRL_XOSC32K_CGM_HS_Val   _U_(0x2)   /**< \brief (OSC32KCTRL_XOSC32K) High Speed mode */
#define OSC32KCTRL_XOSC32K_CGM_XT   (OSC32KCTRL_XOSC32K_CGM_XT_Val << OSC32KCTRL_XOSC32K_CGM_Pos)
#define OSC32KCTRL_XOSC32K_CGM_HS   (OSC32KCTRL_XOSC32K_CGM_HS_Val << OSC32KCTRL_XOSC32K_CGM_Pos)
#define OSC32KCTRL_XOSC32K_MASK     _U_(0x77DE)  /**< \brief (OSC32KCTRL_XOSC32K) MASK Register */

/* -------- OSC32KCTRL_CFDCTRL : (OSC32KCTRL Offset: 0x16) (R/W  8) Clock Failure Detector Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CFDEN:1;          /*!< bit:      0  Clock Failure Detector Enable      */
    uint8_t  SWBACK:1;         /*!< bit:      1  Clock Switch Back                  */
    uint8_t  CFDPRESC:1;       /*!< bit:      2  Clock Failure Detector Prescaler   */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} OSC32KCTRL_CFDCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSC32KCTRL_CFDCTRL_OFFSET   0x16         /**< \brief (OSC32KCTRL_CFDCTRL offset) Clock Failure Detector Control */
#define OSC32KCTRL_CFDCTRL_RESETVALUE _U_(0x00)    /**< \brief (OSC32KCTRL_CFDCTRL reset_value) Clock Failure Detector Control */

#define OSC32KCTRL_CFDCTRL_CFDEN_Pos 0            /**< \brief (OSC32KCTRL_CFDCTRL) Clock Failure Detector Enable */
#define OSC32KCTRL_CFDCTRL_CFDEN    (_U_(0x1) << OSC32KCTRL_CFDCTRL_CFDEN_Pos)
#define OSC32KCTRL_CFDCTRL_SWBACK_Pos 1            /**< \brief (OSC32KCTRL_CFDCTRL) Clock Switch Back */
#define OSC32KCTRL_CFDCTRL_SWBACK   (_U_(0x1) << OSC32KCTRL_CFDCTRL_SWBACK_Pos)
#define OSC32KCTRL_CFDCTRL_CFDPRESC_Pos 2            /**< \brief (OSC32KCTRL_CFDCTRL) Clock Failure Detector Prescaler */
#define OSC32KCTRL_CFDCTRL_CFDPRESC (_U_(0x1) << OSC32KCTRL_CFDCTRL_CFDPRESC_Pos)
#define OSC32KCTRL_CFDCTRL_MASK     _U_(0x07)    /**< \brief (OSC32KCTRL_CFDCTRL) MASK Register */

/* -------- OSC32KCTRL_EVCTRL : (OSC32KCTRL Offset: 0x17) (R/W  8) Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CFDEO:1;          /*!< bit:      0  Clock Failure Detector Event Output Enable */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} OSC32KCTRL_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSC32KCTRL_EVCTRL_OFFSET    0x17         /**< \brief (OSC32KCTRL_EVCTRL offset) Event Control */
#define OSC32KCTRL_EVCTRL_RESETVALUE _U_(0x00)    /**< \brief (OSC32KCTRL_EVCTRL reset_value) Event Control */

#define OSC32KCTRL_EVCTRL_CFDEO_Pos 0            /**< \brief (OSC32KCTRL_EVCTRL) Clock Failure Detector Event Output Enable */
#define OSC32KCTRL_EVCTRL_CFDEO     (_U_(0x1) << OSC32KCTRL_EVCTRL_CFDEO_Pos)
#define OSC32KCTRL_EVCTRL_MASK      _U_(0x01)    /**< \brief (OSC32KCTRL_EVCTRL) MASK Register */

/* -------- OSC32KCTRL_OSCULP32K : (OSC32KCTRL Offset: 0x1C) (R/W 32) 32kHz Ultra Low Power Internal Oscillator (OSCULP32K) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t EN32K:1;          /*!< bit:      1  Enable Out 32k                     */
    uint32_t EN1K:1;           /*!< bit:      2  Enable Out 1k                      */
    uint32_t :5;               /*!< bit:  3.. 7  Reserved                           */
    uint32_t CALIB:6;          /*!< bit:  8..13  Oscillator Calibration             */
    uint32_t :1;               /*!< bit:     14  Reserved                           */
    uint32_t WRTLOCK:1;        /*!< bit:     15  Write Lock                         */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} OSC32KCTRL_OSCULP32K_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define OSC32KCTRL_OSCULP32K_OFFSET 0x1C         /**< \brief (OSC32KCTRL_OSCULP32K offset) 32kHz Ultra Low Power Internal Oscillator (OSCULP32K) Control */
#define OSC32KCTRL_OSCULP32K_RESETVALUE _U_(0x00000000) /**< \brief (OSC32KCTRL_OSCULP32K reset_value) 32kHz Ultra Low Power Internal Oscillator (OSCULP32K) Control */

#define OSC32KCTRL_OSCULP32K_EN32K_Pos 1            /**< \brief (OSC32KCTRL_OSCULP32K) Enable Out 32k */
#define OSC32KCTRL_OSCULP32K_EN32K  (_U_(0x1) << OSC32KCTRL_OSCULP32K_EN32K_Pos)
#define OSC32KCTRL_OSCULP32K_EN1K_Pos 2            /**< \brief (OSC32KCTRL_OSCULP32K) Enable Out 1k */
#define OSC32KCTRL_OSCULP32K_EN1K   (_U_(0x1) << OSC32KCTRL_OSCULP32K_EN1K_Pos)
#define OSC32KCTRL_OSCULP32K_CALIB_Pos 8            /**< \brief (OSC32KCTRL_OSCULP32K) Oscillator Calibration */
#define OSC32KCTRL_OSCULP32K_CALIB_Msk (_U_(0x3F) << OSC32KCTRL_OSCULP32K_CALIB_Pos)
#define OSC32KCTRL_OSCULP32K_CALIB(value) (OSC32KCTRL_OSCULP32K_CALIB_Msk & ((value) << OSC32KCTRL_OSCULP32K_CALIB_Pos))
#define OSC32KCTRL_OSCULP32K_WRTLOCK_Pos 15           /**< \brief (OSC32KCTRL_OSCULP32K) Write Lock */
#define OSC32KCTRL_OSCULP32K_WRTLOCK (_U_(0x1) << OSC32KCTRL_OSCULP32K_WRTLOCK_Pos)
#define OSC32KCTRL_OSCULP32K_MASK   _U_(0x0000BF06) /**< \brief (OSC32KCTRL_OSCULP32K) MASK Register */

/** \brief OSC32KCTRL hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO OSC32KCTRL_INTENCLR_Type  INTENCLR;    /**< \brief Offset: 0x00 (R/W 32) Interrupt Enable Clear */
  __IO OSC32KCTRL_INTENSET_Type  INTENSET;    /**< \brief Offset: 0x04 (R/W 32) Interrupt Enable Set */
  __IO OSC32KCTRL_INTFLAG_Type   INTFLAG;     /**< \brief Offset: 0x08 (R/W 32) Interrupt Flag Status and Clear */
  __I  OSC32KCTRL_STATUS_Type    STATUS;      /**< \brief Offset: 0x0C (R/  32) Power and Clocks Status */
  __IO OSC32KCTRL_RTCCTRL_Type   RTCCTRL;     /**< \brief Offset: 0x10 (R/W  8) RTC Clock Selection */
       RoReg8                    Reserved1[0x3];
  __IO OSC32KCTRL_XOSC32K_Type   XOSC32K;     /**< \brief Offset: 0x14 (R/W 16) 32kHz External Crystal Oscillator (XOSC32K) Control */
  __IO OSC32KCTRL_CFDCTRL_Type   CFDCTRL;     /**< \brief Offset: 0x16 (R/W  8) Clock Failure Detector Control */
  __IO OSC32KCTRL_EVCTRL_Type    EVCTRL;      /**< \brief Offset: 0x17 (R/W  8) Event Control */
       RoReg8                    Reserved2[0x4];
  __IO OSC32KCTRL_OSCULP32K_Type OSCULP32K;   /**< \brief Offset: 0x1C (R/W 32) 32kHz Ultra Low Power Internal Oscillator (OSCULP32K) Control */
} Osc32kctrl;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_OSC32KCTRL_COMPONENT_ */
