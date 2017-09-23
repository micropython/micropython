/**
 * \file
 *
 * \brief Component description for PAC
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

#ifndef _SAMD21_PAC_COMPONENT_
#define _SAMD21_PAC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR PAC */
/* ========================================================================== */
/** \addtogroup SAMD21_PAC Peripheral Access Controller */
/*@{*/

#define PAC_U2211
#define REV_PAC                     0x101

/* -------- PAC_WPCLR : (PAC Offset: 0x0) (R/W 32) Write Protection Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t WP:31;            /*!< bit:  1..31  Write Protection Clear             */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_WPCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_WPCLR_OFFSET            0x0          /**< \brief (PAC_WPCLR offset) Write Protection Clear */
#define PAC_WPCLR_RESETVALUE        _U(0x00000000) /**< \brief (PAC_WPCLR reset_value) Write Protection Clear */

#define PAC_WPCLR_WP_Pos            1            /**< \brief (PAC_WPCLR) Write Protection Clear */
#define PAC_WPCLR_WP_Msk            (_U(0x7FFFFFFF) << PAC_WPCLR_WP_Pos)
#define PAC_WPCLR_WP(value)         (PAC_WPCLR_WP_Msk & ((value) << PAC_WPCLR_WP_Pos))
#define PAC_WPCLR_MASK              _U(0xFFFFFFFE) /**< \brief (PAC_WPCLR) MASK Register */

/* -------- PAC_WPSET : (PAC Offset: 0x4) (R/W 32) Write Protection Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t WP:31;            /*!< bit:  1..31  Write Protection Set               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PAC_WPSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PAC_WPSET_OFFSET            0x4          /**< \brief (PAC_WPSET offset) Write Protection Set */
#define PAC_WPSET_RESETVALUE        _U(0x00000000) /**< \brief (PAC_WPSET reset_value) Write Protection Set */

#define PAC_WPSET_WP_Pos            1            /**< \brief (PAC_WPSET) Write Protection Set */
#define PAC_WPSET_WP_Msk            (_U(0x7FFFFFFF) << PAC_WPSET_WP_Pos)
#define PAC_WPSET_WP(value)         (PAC_WPSET_WP_Msk & ((value) << PAC_WPSET_WP_Pos))
#define PAC_WPSET_MASK              _U(0xFFFFFFFE) /**< \brief (PAC_WPSET) MASK Register */

/** \brief PAC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO PAC_WPCLR_Type            WPCLR;       /**< \brief Offset: 0x0 (R/W 32) Write Protection Clear */
  __IO PAC_WPSET_Type            WPSET;       /**< \brief Offset: 0x4 (R/W 32) Write Protection Set */
} Pac;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD21_PAC_COMPONENT_ */
