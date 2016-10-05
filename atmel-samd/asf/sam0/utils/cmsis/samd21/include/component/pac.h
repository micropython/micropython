/**
 * \file
 *
 * \brief Component description for PAC
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
#define PAC_WPCLR_RESETVALUE        0x00000000ul /**< \brief (PAC_WPCLR reset_value) Write Protection Clear */

#define PAC_WPCLR_WP_Pos            1            /**< \brief (PAC_WPCLR) Write Protection Clear */
#define PAC_WPCLR_WP_Msk            (0x7FFFFFFFul << PAC_WPCLR_WP_Pos)
#define PAC_WPCLR_WP(value)         (PAC_WPCLR_WP_Msk & ((value) << PAC_WPCLR_WP_Pos))
#define PAC_WPCLR_MASK              0xFFFFFFFEul /**< \brief (PAC_WPCLR) MASK Register */

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
#define PAC_WPSET_RESETVALUE        0x00000000ul /**< \brief (PAC_WPSET reset_value) Write Protection Set */

#define PAC_WPSET_WP_Pos            1            /**< \brief (PAC_WPSET) Write Protection Set */
#define PAC_WPSET_WP_Msk            (0x7FFFFFFFul << PAC_WPSET_WP_Pos)
#define PAC_WPSET_WP(value)         (PAC_WPSET_WP_Msk & ((value) << PAC_WPSET_WP_Pos))
#define PAC_WPSET_MASK              0xFFFFFFFEul /**< \brief (PAC_WPSET) MASK Register */

/** \brief PAC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO PAC_WPCLR_Type            WPCLR;       /**< \brief Offset: 0x0 (R/W 32) Write Protection Clear */
  __IO PAC_WPSET_Type            WPSET;       /**< \brief Offset: 0x4 (R/W 32) Write Protection Set */
} Pac;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD21_PAC_COMPONENT_ */
