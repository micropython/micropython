/**
 * \file
 *
 * \brief Component description for FREQM
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

#ifndef _SAMD51_FREQM_COMPONENT_
#define _SAMD51_FREQM_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR FREQM */
/* ========================================================================== */
/** \addtogroup SAMD51_FREQM Frequency Meter */
/*@{*/

#define FREQM_U2257
#define REV_FREQM                   0x110

/* -------- FREQM_CTRLA : (FREQM Offset: 0x00) (R/W  8) Control A Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} FREQM_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define FREQM_CTRLA_OFFSET          0x00         /**< \brief (FREQM_CTRLA offset) Control A Register */
#define FREQM_CTRLA_RESETVALUE      _U_(0x00)    /**< \brief (FREQM_CTRLA reset_value) Control A Register */

#define FREQM_CTRLA_SWRST_Pos       0            /**< \brief (FREQM_CTRLA) Software Reset */
#define FREQM_CTRLA_SWRST           (_U_(0x1) << FREQM_CTRLA_SWRST_Pos)
#define FREQM_CTRLA_ENABLE_Pos      1            /**< \brief (FREQM_CTRLA) Enable */
#define FREQM_CTRLA_ENABLE          (_U_(0x1) << FREQM_CTRLA_ENABLE_Pos)
#define FREQM_CTRLA_MASK            _U_(0x03)    /**< \brief (FREQM_CTRLA) MASK Register */

/* -------- FREQM_CTRLB : (FREQM Offset: 0x01) ( /W  8) Control B Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  START:1;          /*!< bit:      0  Start Measurement                  */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} FREQM_CTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define FREQM_CTRLB_OFFSET          0x01         /**< \brief (FREQM_CTRLB offset) Control B Register */
#define FREQM_CTRLB_RESETVALUE      _U_(0x00)    /**< \brief (FREQM_CTRLB reset_value) Control B Register */

#define FREQM_CTRLB_START_Pos       0            /**< \brief (FREQM_CTRLB) Start Measurement */
#define FREQM_CTRLB_START           (_U_(0x1) << FREQM_CTRLB_START_Pos)
#define FREQM_CTRLB_MASK            _U_(0x01)    /**< \brief (FREQM_CTRLB) MASK Register */

/* -------- FREQM_CFGA : (FREQM Offset: 0x02) (R/W 16) Config A register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t REFNUM:8;         /*!< bit:  0.. 7  Number of Reference Clock Cycles   */
    uint16_t :8;               /*!< bit:  8..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} FREQM_CFGA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define FREQM_CFGA_OFFSET           0x02         /**< \brief (FREQM_CFGA offset) Config A register */
#define FREQM_CFGA_RESETVALUE       _U_(0x0000)  /**< \brief (FREQM_CFGA reset_value) Config A register */

#define FREQM_CFGA_REFNUM_Pos       0            /**< \brief (FREQM_CFGA) Number of Reference Clock Cycles */
#define FREQM_CFGA_REFNUM_Msk       (_U_(0xFF) << FREQM_CFGA_REFNUM_Pos)
#define FREQM_CFGA_REFNUM(value)    (FREQM_CFGA_REFNUM_Msk & ((value) << FREQM_CFGA_REFNUM_Pos))
#define FREQM_CFGA_MASK             _U_(0x00FF)  /**< \brief (FREQM_CFGA) MASK Register */

/* -------- FREQM_INTENCLR : (FREQM Offset: 0x08) (R/W  8) Interrupt Enable Clear Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DONE:1;           /*!< bit:      0  Measurement Done Interrupt Enable  */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} FREQM_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define FREQM_INTENCLR_OFFSET       0x08         /**< \brief (FREQM_INTENCLR offset) Interrupt Enable Clear Register */
#define FREQM_INTENCLR_RESETVALUE   _U_(0x00)    /**< \brief (FREQM_INTENCLR reset_value) Interrupt Enable Clear Register */

#define FREQM_INTENCLR_DONE_Pos     0            /**< \brief (FREQM_INTENCLR) Measurement Done Interrupt Enable */
#define FREQM_INTENCLR_DONE         (_U_(0x1) << FREQM_INTENCLR_DONE_Pos)
#define FREQM_INTENCLR_MASK         _U_(0x01)    /**< \brief (FREQM_INTENCLR) MASK Register */

/* -------- FREQM_INTENSET : (FREQM Offset: 0x09) (R/W  8) Interrupt Enable Set Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DONE:1;           /*!< bit:      0  Measurement Done Interrupt Enable  */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} FREQM_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define FREQM_INTENSET_OFFSET       0x09         /**< \brief (FREQM_INTENSET offset) Interrupt Enable Set Register */
#define FREQM_INTENSET_RESETVALUE   _U_(0x00)    /**< \brief (FREQM_INTENSET reset_value) Interrupt Enable Set Register */

#define FREQM_INTENSET_DONE_Pos     0            /**< \brief (FREQM_INTENSET) Measurement Done Interrupt Enable */
#define FREQM_INTENSET_DONE         (_U_(0x1) << FREQM_INTENSET_DONE_Pos)
#define FREQM_INTENSET_MASK         _U_(0x01)    /**< \brief (FREQM_INTENSET) MASK Register */

/* -------- FREQM_INTFLAG : (FREQM Offset: 0x0A) (R/W  8) Interrupt Flag Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  DONE:1;           /*!< bit:      0  Measurement Done                   */
    __I uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} FREQM_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define FREQM_INTFLAG_OFFSET        0x0A         /**< \brief (FREQM_INTFLAG offset) Interrupt Flag Register */
#define FREQM_INTFLAG_RESETVALUE    _U_(0x00)    /**< \brief (FREQM_INTFLAG reset_value) Interrupt Flag Register */

#define FREQM_INTFLAG_DONE_Pos      0            /**< \brief (FREQM_INTFLAG) Measurement Done */
#define FREQM_INTFLAG_DONE          (_U_(0x1) << FREQM_INTFLAG_DONE_Pos)
#define FREQM_INTFLAG_MASK          _U_(0x01)    /**< \brief (FREQM_INTFLAG) MASK Register */

/* -------- FREQM_STATUS : (FREQM Offset: 0x0B) (R/W  8) Status Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  BUSY:1;           /*!< bit:      0  FREQM Status                       */
    uint8_t  OVF:1;            /*!< bit:      1  Sticky Count Value Overflow        */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} FREQM_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define FREQM_STATUS_OFFSET         0x0B         /**< \brief (FREQM_STATUS offset) Status Register */
#define FREQM_STATUS_RESETVALUE     _U_(0x00)    /**< \brief (FREQM_STATUS reset_value) Status Register */

#define FREQM_STATUS_BUSY_Pos       0            /**< \brief (FREQM_STATUS) FREQM Status */
#define FREQM_STATUS_BUSY           (_U_(0x1) << FREQM_STATUS_BUSY_Pos)
#define FREQM_STATUS_OVF_Pos        1            /**< \brief (FREQM_STATUS) Sticky Count Value Overflow */
#define FREQM_STATUS_OVF            (_U_(0x1) << FREQM_STATUS_OVF_Pos)
#define FREQM_STATUS_MASK           _U_(0x03)    /**< \brief (FREQM_STATUS) MASK Register */

/* -------- FREQM_SYNCBUSY : (FREQM Offset: 0x0C) (R/  32) Synchronization Busy Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint32_t :30;              /*!< bit:  2..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} FREQM_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define FREQM_SYNCBUSY_OFFSET       0x0C         /**< \brief (FREQM_SYNCBUSY offset) Synchronization Busy Register */
#define FREQM_SYNCBUSY_RESETVALUE   _U_(0x00000000) /**< \brief (FREQM_SYNCBUSY reset_value) Synchronization Busy Register */

#define FREQM_SYNCBUSY_SWRST_Pos    0            /**< \brief (FREQM_SYNCBUSY) Software Reset */
#define FREQM_SYNCBUSY_SWRST        (_U_(0x1) << FREQM_SYNCBUSY_SWRST_Pos)
#define FREQM_SYNCBUSY_ENABLE_Pos   1            /**< \brief (FREQM_SYNCBUSY) Enable */
#define FREQM_SYNCBUSY_ENABLE       (_U_(0x1) << FREQM_SYNCBUSY_ENABLE_Pos)
#define FREQM_SYNCBUSY_MASK         _U_(0x00000003) /**< \brief (FREQM_SYNCBUSY) MASK Register */

/* -------- FREQM_VALUE : (FREQM Offset: 0x10) (R/  32) Count Value Register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t VALUE:24;         /*!< bit:  0..23  Measurement Value                  */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} FREQM_VALUE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define FREQM_VALUE_OFFSET          0x10         /**< \brief (FREQM_VALUE offset) Count Value Register */
#define FREQM_VALUE_RESETVALUE      _U_(0x00000000) /**< \brief (FREQM_VALUE reset_value) Count Value Register */

#define FREQM_VALUE_VALUE_Pos       0            /**< \brief (FREQM_VALUE) Measurement Value */
#define FREQM_VALUE_VALUE_Msk       (_U_(0xFFFFFF) << FREQM_VALUE_VALUE_Pos)
#define FREQM_VALUE_VALUE(value)    (FREQM_VALUE_VALUE_Msk & ((value) << FREQM_VALUE_VALUE_Pos))
#define FREQM_VALUE_MASK            _U_(0x00FFFFFF) /**< \brief (FREQM_VALUE) MASK Register */

/** \brief FREQM hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO FREQM_CTRLA_Type          CTRLA;       /**< \brief Offset: 0x00 (R/W  8) Control A Register */
  __O  FREQM_CTRLB_Type          CTRLB;       /**< \brief Offset: 0x01 ( /W  8) Control B Register */
  __IO FREQM_CFGA_Type           CFGA;        /**< \brief Offset: 0x02 (R/W 16) Config A register */
       RoReg8                    Reserved1[0x4];
  __IO FREQM_INTENCLR_Type       INTENCLR;    /**< \brief Offset: 0x08 (R/W  8) Interrupt Enable Clear Register */
  __IO FREQM_INTENSET_Type       INTENSET;    /**< \brief Offset: 0x09 (R/W  8) Interrupt Enable Set Register */
  __IO FREQM_INTFLAG_Type        INTFLAG;     /**< \brief Offset: 0x0A (R/W  8) Interrupt Flag Register */
  __IO FREQM_STATUS_Type         STATUS;      /**< \brief Offset: 0x0B (R/W  8) Status Register */
  __I  FREQM_SYNCBUSY_Type       SYNCBUSY;    /**< \brief Offset: 0x0C (R/  32) Synchronization Busy Register */
  __I  FREQM_VALUE_Type          VALUE;       /**< \brief Offset: 0x10 (R/  32) Count Value Register */
} Freqm;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_FREQM_COMPONENT_ */
