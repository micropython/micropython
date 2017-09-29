/**
 * \file
 *
 * \brief Component description for TRNG
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

#ifndef _SAMD51_TRNG_COMPONENT_
#define _SAMD51_TRNG_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR TRNG */
/* ========================================================================== */
/** \addtogroup SAMD51_TRNG True Random Generator */
/*@{*/

#define TRNG_U2242
#define REV_TRNG                    0x110

/* -------- TRNG_CTRLA : (TRNG Offset: 0x00) (R/W  8) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :1;               /*!< bit:      0  Reserved                           */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  :4;               /*!< bit:  2.. 5  Reserved                           */
    uint8_t  RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint8_t  :1;               /*!< bit:      7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TRNG_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TRNG_CTRLA_OFFSET           0x00         /**< \brief (TRNG_CTRLA offset) Control A */
#define TRNG_CTRLA_RESETVALUE       _U_(0x00)    /**< \brief (TRNG_CTRLA reset_value) Control A */

#define TRNG_CTRLA_ENABLE_Pos       1            /**< \brief (TRNG_CTRLA) Enable */
#define TRNG_CTRLA_ENABLE           (_U_(0x1) << TRNG_CTRLA_ENABLE_Pos)
#define TRNG_CTRLA_RUNSTDBY_Pos     6            /**< \brief (TRNG_CTRLA) Run in Standby */
#define TRNG_CTRLA_RUNSTDBY         (_U_(0x1) << TRNG_CTRLA_RUNSTDBY_Pos)
#define TRNG_CTRLA_MASK             _U_(0x42)    /**< \brief (TRNG_CTRLA) MASK Register */

/* -------- TRNG_EVCTRL : (TRNG Offset: 0x04) (R/W  8) Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DATARDYEO:1;      /*!< bit:      0  Data Ready Event Output            */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TRNG_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TRNG_EVCTRL_OFFSET          0x04         /**< \brief (TRNG_EVCTRL offset) Event Control */
#define TRNG_EVCTRL_RESETVALUE      _U_(0x00)    /**< \brief (TRNG_EVCTRL reset_value) Event Control */

#define TRNG_EVCTRL_DATARDYEO_Pos   0            /**< \brief (TRNG_EVCTRL) Data Ready Event Output */
#define TRNG_EVCTRL_DATARDYEO       (_U_(0x1) << TRNG_EVCTRL_DATARDYEO_Pos)
#define TRNG_EVCTRL_MASK            _U_(0x01)    /**< \brief (TRNG_EVCTRL) MASK Register */

/* -------- TRNG_INTENCLR : (TRNG Offset: 0x08) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DATARDY:1;        /*!< bit:      0  Data Ready Interrupt Enable        */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TRNG_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TRNG_INTENCLR_OFFSET        0x08         /**< \brief (TRNG_INTENCLR offset) Interrupt Enable Clear */
#define TRNG_INTENCLR_RESETVALUE    _U_(0x00)    /**< \brief (TRNG_INTENCLR reset_value) Interrupt Enable Clear */

#define TRNG_INTENCLR_DATARDY_Pos   0            /**< \brief (TRNG_INTENCLR) Data Ready Interrupt Enable */
#define TRNG_INTENCLR_DATARDY       (_U_(0x1) << TRNG_INTENCLR_DATARDY_Pos)
#define TRNG_INTENCLR_MASK          _U_(0x01)    /**< \brief (TRNG_INTENCLR) MASK Register */

/* -------- TRNG_INTENSET : (TRNG Offset: 0x09) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DATARDY:1;        /*!< bit:      0  Data Ready Interrupt Enable        */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TRNG_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TRNG_INTENSET_OFFSET        0x09         /**< \brief (TRNG_INTENSET offset) Interrupt Enable Set */
#define TRNG_INTENSET_RESETVALUE    _U_(0x00)    /**< \brief (TRNG_INTENSET reset_value) Interrupt Enable Set */

#define TRNG_INTENSET_DATARDY_Pos   0            /**< \brief (TRNG_INTENSET) Data Ready Interrupt Enable */
#define TRNG_INTENSET_DATARDY       (_U_(0x1) << TRNG_INTENSET_DATARDY_Pos)
#define TRNG_INTENSET_MASK          _U_(0x01)    /**< \brief (TRNG_INTENSET) MASK Register */

/* -------- TRNG_INTFLAG : (TRNG Offset: 0x0A) (R/W  8) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  DATARDY:1;        /*!< bit:      0  Data Ready Interrupt Flag          */
    __I uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} TRNG_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TRNG_INTFLAG_OFFSET         0x0A         /**< \brief (TRNG_INTFLAG offset) Interrupt Flag Status and Clear */
#define TRNG_INTFLAG_RESETVALUE     _U_(0x00)    /**< \brief (TRNG_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define TRNG_INTFLAG_DATARDY_Pos    0            /**< \brief (TRNG_INTFLAG) Data Ready Interrupt Flag */
#define TRNG_INTFLAG_DATARDY        (_U_(0x1) << TRNG_INTFLAG_DATARDY_Pos)
#define TRNG_INTFLAG_MASK           _U_(0x01)    /**< \brief (TRNG_INTFLAG) MASK Register */

/* -------- TRNG_DATA : (TRNG Offset: 0x20) (R/  32) Output Data -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DATA:32;          /*!< bit:  0..31  Output Data                        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} TRNG_DATA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define TRNG_DATA_OFFSET            0x20         /**< \brief (TRNG_DATA offset) Output Data */
#define TRNG_DATA_RESETVALUE        _U_(0x00000000) /**< \brief (TRNG_DATA reset_value) Output Data */

#define TRNG_DATA_DATA_Pos          0            /**< \brief (TRNG_DATA) Output Data */
#define TRNG_DATA_DATA_Msk          (_U_(0xFFFFFFFF) << TRNG_DATA_DATA_Pos)
#define TRNG_DATA_DATA(value)       (TRNG_DATA_DATA_Msk & ((value) << TRNG_DATA_DATA_Pos))
#define TRNG_DATA_MASK              _U_(0xFFFFFFFF) /**< \brief (TRNG_DATA) MASK Register */

/** \brief TRNG hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO TRNG_CTRLA_Type           CTRLA;       /**< \brief Offset: 0x00 (R/W  8) Control A */
       RoReg8                    Reserved1[0x3];
  __IO TRNG_EVCTRL_Type          EVCTRL;      /**< \brief Offset: 0x04 (R/W  8) Event Control */
       RoReg8                    Reserved2[0x3];
  __IO TRNG_INTENCLR_Type        INTENCLR;    /**< \brief Offset: 0x08 (R/W  8) Interrupt Enable Clear */
  __IO TRNG_INTENSET_Type        INTENSET;    /**< \brief Offset: 0x09 (R/W  8) Interrupt Enable Set */
  __IO TRNG_INTFLAG_Type         INTFLAG;     /**< \brief Offset: 0x0A (R/W  8) Interrupt Flag Status and Clear */
       RoReg8                    Reserved3[0x15];
  __I  TRNG_DATA_Type            DATA;        /**< \brief Offset: 0x20 (R/  32) Output Data */
} Trng;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_TRNG_COMPONENT_ */
