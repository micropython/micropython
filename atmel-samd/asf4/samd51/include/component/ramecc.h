/**
 * \file
 *
 * \brief Component description for RAMECC
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

#ifndef _SAMD51_RAMECC_COMPONENT_
#define _SAMD51_RAMECC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR RAMECC */
/* ========================================================================== */
/** \addtogroup SAMD51_RAMECC RAM ECC */
/*@{*/

#define RAMECC_U2268
#define REV_RAMECC                  0x100

/* -------- RAMECC_INTENCLR : (RAMECC Offset: 0x0) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SINGLEE:1;        /*!< bit:      0  Single Bit ECC Error Interrupt Enable Clear */
    uint8_t  DUALE:1;          /*!< bit:      1  Dual Bit ECC Error Interrupt Enable Clear */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} RAMECC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RAMECC_INTENCLR_OFFSET      0x0          /**< \brief (RAMECC_INTENCLR offset) Interrupt Enable Clear */
#define RAMECC_INTENCLR_RESETVALUE  _U_(0x00)    /**< \brief (RAMECC_INTENCLR reset_value) Interrupt Enable Clear */

#define RAMECC_INTENCLR_SINGLEE_Pos 0            /**< \brief (RAMECC_INTENCLR) Single Bit ECC Error Interrupt Enable Clear */
#define RAMECC_INTENCLR_SINGLEE     (_U_(0x1) << RAMECC_INTENCLR_SINGLEE_Pos)
#define RAMECC_INTENCLR_DUALE_Pos   1            /**< \brief (RAMECC_INTENCLR) Dual Bit ECC Error Interrupt Enable Clear */
#define RAMECC_INTENCLR_DUALE       (_U_(0x1) << RAMECC_INTENCLR_DUALE_Pos)
#define RAMECC_INTENCLR_MASK        _U_(0x03)    /**< \brief (RAMECC_INTENCLR) MASK Register */

/* -------- RAMECC_INTENSET : (RAMECC Offset: 0x1) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SINGLEE:1;        /*!< bit:      0  Single Bit ECC Error Interrupt Enable Set */
    uint8_t  DUALE:1;          /*!< bit:      1  Dual Bit ECC Error Interrupt Enable Set */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} RAMECC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RAMECC_INTENSET_OFFSET      0x1          /**< \brief (RAMECC_INTENSET offset) Interrupt Enable Set */
#define RAMECC_INTENSET_RESETVALUE  _U_(0x00)    /**< \brief (RAMECC_INTENSET reset_value) Interrupt Enable Set */

#define RAMECC_INTENSET_SINGLEE_Pos 0            /**< \brief (RAMECC_INTENSET) Single Bit ECC Error Interrupt Enable Set */
#define RAMECC_INTENSET_SINGLEE     (_U_(0x1) << RAMECC_INTENSET_SINGLEE_Pos)
#define RAMECC_INTENSET_DUALE_Pos   1            /**< \brief (RAMECC_INTENSET) Dual Bit ECC Error Interrupt Enable Set */
#define RAMECC_INTENSET_DUALE       (_U_(0x1) << RAMECC_INTENSET_DUALE_Pos)
#define RAMECC_INTENSET_MASK        _U_(0x03)    /**< \brief (RAMECC_INTENSET) MASK Register */

/* -------- RAMECC_INTFLAG : (RAMECC Offset: 0x2) (R/W  8) Interrupt Flag -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  SINGLEE:1;        /*!< bit:      0  Single Bit ECC Error Interrupt     */
    __I uint8_t  DUALE:1;          /*!< bit:      1  Dual Bit ECC Error Interrupt       */
    __I uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} RAMECC_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RAMECC_INTFLAG_OFFSET       0x2          /**< \brief (RAMECC_INTFLAG offset) Interrupt Flag */
#define RAMECC_INTFLAG_RESETVALUE   _U_(0x00)    /**< \brief (RAMECC_INTFLAG reset_value) Interrupt Flag */

#define RAMECC_INTFLAG_SINGLEE_Pos  0            /**< \brief (RAMECC_INTFLAG) Single Bit ECC Error Interrupt */
#define RAMECC_INTFLAG_SINGLEE      (_U_(0x1) << RAMECC_INTFLAG_SINGLEE_Pos)
#define RAMECC_INTFLAG_DUALE_Pos    1            /**< \brief (RAMECC_INTFLAG) Dual Bit ECC Error Interrupt */
#define RAMECC_INTFLAG_DUALE        (_U_(0x1) << RAMECC_INTFLAG_DUALE_Pos)
#define RAMECC_INTFLAG_MASK         _U_(0x03)    /**< \brief (RAMECC_INTFLAG) MASK Register */

/* -------- RAMECC_STATUS : (RAMECC Offset: 0x3) (R/   8) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  ECCDIS:1;         /*!< bit:      0  ECC Disable                        */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} RAMECC_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RAMECC_STATUS_OFFSET        0x3          /**< \brief (RAMECC_STATUS offset) Status */
#define RAMECC_STATUS_RESETVALUE    _U_(0x00)    /**< \brief (RAMECC_STATUS reset_value) Status */

#define RAMECC_STATUS_ECCDIS_Pos    0            /**< \brief (RAMECC_STATUS) ECC Disable */
#define RAMECC_STATUS_ECCDIS        (_U_(0x1) << RAMECC_STATUS_ECCDIS_Pos)
#define RAMECC_STATUS_MASK          _U_(0x01)    /**< \brief (RAMECC_STATUS) MASK Register */

/* -------- RAMECC_ERRADDR : (RAMECC Offset: 0x4) (R/  32) Error Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ERRADDR:17;       /*!< bit:  0..16  Error Address                      */
    uint32_t :15;              /*!< bit: 17..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} RAMECC_ERRADDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RAMECC_ERRADDR_OFFSET       0x4          /**< \brief (RAMECC_ERRADDR offset) Error Address */
#define RAMECC_ERRADDR_RESETVALUE   _U_(0x00000000) /**< \brief (RAMECC_ERRADDR reset_value) Error Address */

#define RAMECC_ERRADDR_ERRADDR_Pos  0            /**< \brief (RAMECC_ERRADDR) Error Address */
#define RAMECC_ERRADDR_ERRADDR_Msk  (_U_(0x1FFFF) << RAMECC_ERRADDR_ERRADDR_Pos)
#define RAMECC_ERRADDR_ERRADDR(value) (RAMECC_ERRADDR_ERRADDR_Msk & ((value) << RAMECC_ERRADDR_ERRADDR_Pos))
#define RAMECC_ERRADDR_MASK         _U_(0x0001FFFF) /**< \brief (RAMECC_ERRADDR) MASK Register */

/* -------- RAMECC_DBGCTRL : (RAMECC Offset: 0xF) (R/W  8) Debug Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  ECCDIS:1;         /*!< bit:      0  ECC Disable                        */
    uint8_t  ECCELOG:1;        /*!< bit:      1  ECC Error Log                      */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} RAMECC_DBGCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define RAMECC_DBGCTRL_OFFSET       0xF          /**< \brief (RAMECC_DBGCTRL offset) Debug Control */
#define RAMECC_DBGCTRL_RESETVALUE   _U_(0x00)    /**< \brief (RAMECC_DBGCTRL reset_value) Debug Control */

#define RAMECC_DBGCTRL_ECCDIS_Pos   0            /**< \brief (RAMECC_DBGCTRL) ECC Disable */
#define RAMECC_DBGCTRL_ECCDIS       (_U_(0x1) << RAMECC_DBGCTRL_ECCDIS_Pos)
#define RAMECC_DBGCTRL_ECCELOG_Pos  1            /**< \brief (RAMECC_DBGCTRL) ECC Error Log */
#define RAMECC_DBGCTRL_ECCELOG      (_U_(0x1) << RAMECC_DBGCTRL_ECCELOG_Pos)
#define RAMECC_DBGCTRL_MASK         _U_(0x03)    /**< \brief (RAMECC_DBGCTRL) MASK Register */

/** \brief RAMECC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO RAMECC_INTENCLR_Type      INTENCLR;    /**< \brief Offset: 0x0 (R/W  8) Interrupt Enable Clear */
  __IO RAMECC_INTENSET_Type      INTENSET;    /**< \brief Offset: 0x1 (R/W  8) Interrupt Enable Set */
  __IO RAMECC_INTFLAG_Type       INTFLAG;     /**< \brief Offset: 0x2 (R/W  8) Interrupt Flag */
  __I  RAMECC_STATUS_Type        STATUS;      /**< \brief Offset: 0x3 (R/   8) Status */
  __I  RAMECC_ERRADDR_Type       ERRADDR;     /**< \brief Offset: 0x4 (R/  32) Error Address */
       RoReg8                    Reserved1[0x7];
  __IO RAMECC_DBGCTRL_Type       DBGCTRL;     /**< \brief Offset: 0xF (R/W  8) Debug Control */
} Ramecc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_RAMECC_COMPONENT_ */
