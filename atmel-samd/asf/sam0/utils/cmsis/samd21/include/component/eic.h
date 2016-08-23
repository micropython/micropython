/**
 * \file
 *
 * \brief Component description for EIC
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

#ifndef _SAMD21_EIC_COMPONENT_
#define _SAMD21_EIC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR EIC */
/* ========================================================================== */
/** \addtogroup SAMD21_EIC External Interrupt Controller */
/*@{*/

#define EIC_U2217
#define REV_EIC                     0x101

/* -------- EIC_CTRL : (EIC Offset: 0x00) (R/W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EIC_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_CTRL_OFFSET             0x00         /**< \brief (EIC_CTRL offset) Control */
#define EIC_CTRL_RESETVALUE         0x00ul       /**< \brief (EIC_CTRL reset_value) Control */

#define EIC_CTRL_SWRST_Pos          0            /**< \brief (EIC_CTRL) Software Reset */
#define EIC_CTRL_SWRST              (0x1ul << EIC_CTRL_SWRST_Pos)
#define EIC_CTRL_ENABLE_Pos         1            /**< \brief (EIC_CTRL) Enable */
#define EIC_CTRL_ENABLE             (0x1ul << EIC_CTRL_ENABLE_Pos)
#define EIC_CTRL_MASK               0x03ul       /**< \brief (EIC_CTRL) MASK Register */

/* -------- EIC_STATUS : (EIC Offset: 0x01) (R/   8) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :7;               /*!< bit:  0.. 6  Reserved                           */
    uint8_t  SYNCBUSY:1;       /*!< bit:      7  Synchronization Busy               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EIC_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_STATUS_OFFSET           0x01         /**< \brief (EIC_STATUS offset) Status */
#define EIC_STATUS_RESETVALUE       0x00ul       /**< \brief (EIC_STATUS reset_value) Status */

#define EIC_STATUS_SYNCBUSY_Pos     7            /**< \brief (EIC_STATUS) Synchronization Busy */
#define EIC_STATUS_SYNCBUSY         (0x1ul << EIC_STATUS_SYNCBUSY_Pos)
#define EIC_STATUS_MASK             0x80ul       /**< \brief (EIC_STATUS) MASK Register */

/* -------- EIC_NMICTRL : (EIC Offset: 0x02) (R/W  8) Non-Maskable Interrupt Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  NMISENSE:3;       /*!< bit:  0.. 2  Non-Maskable Interrupt Sense       */
    uint8_t  NMIFILTEN:1;      /*!< bit:      3  Non-Maskable Interrupt Filter Enable */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EIC_NMICTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_NMICTRL_OFFSET          0x02         /**< \brief (EIC_NMICTRL offset) Non-Maskable Interrupt Control */
#define EIC_NMICTRL_RESETVALUE      0x00ul       /**< \brief (EIC_NMICTRL reset_value) Non-Maskable Interrupt Control */

#define EIC_NMICTRL_NMISENSE_Pos    0            /**< \brief (EIC_NMICTRL) Non-Maskable Interrupt Sense */
#define EIC_NMICTRL_NMISENSE_Msk    (0x7ul << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE(value) (EIC_NMICTRL_NMISENSE_Msk & ((value) << EIC_NMICTRL_NMISENSE_Pos))
#define   EIC_NMICTRL_NMISENSE_NONE_Val   0x0ul  /**< \brief (EIC_NMICTRL) No detection */
#define   EIC_NMICTRL_NMISENSE_RISE_Val   0x1ul  /**< \brief (EIC_NMICTRL) Rising-edge detection */
#define   EIC_NMICTRL_NMISENSE_FALL_Val   0x2ul  /**< \brief (EIC_NMICTRL) Falling-edge detection */
#define   EIC_NMICTRL_NMISENSE_BOTH_Val   0x3ul  /**< \brief (EIC_NMICTRL) Both-edges detection */
#define   EIC_NMICTRL_NMISENSE_HIGH_Val   0x4ul  /**< \brief (EIC_NMICTRL) High-level detection */
#define   EIC_NMICTRL_NMISENSE_LOW_Val    0x5ul  /**< \brief (EIC_NMICTRL) Low-level detection */
#define EIC_NMICTRL_NMISENSE_NONE   (EIC_NMICTRL_NMISENSE_NONE_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_RISE   (EIC_NMICTRL_NMISENSE_RISE_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_FALL   (EIC_NMICTRL_NMISENSE_FALL_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_BOTH   (EIC_NMICTRL_NMISENSE_BOTH_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_HIGH   (EIC_NMICTRL_NMISENSE_HIGH_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_LOW    (EIC_NMICTRL_NMISENSE_LOW_Val  << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMIFILTEN_Pos   3            /**< \brief (EIC_NMICTRL) Non-Maskable Interrupt Filter Enable */
#define EIC_NMICTRL_NMIFILTEN       (0x1ul << EIC_NMICTRL_NMIFILTEN_Pos)
#define EIC_NMICTRL_MASK            0x0Ful       /**< \brief (EIC_NMICTRL) MASK Register */

/* -------- EIC_NMIFLAG : (EIC Offset: 0x03) (R/W  8) Non-Maskable Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  NMI:1;            /*!< bit:      0  Non-Maskable Interrupt             */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EIC_NMIFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_NMIFLAG_OFFSET          0x03         /**< \brief (EIC_NMIFLAG offset) Non-Maskable Interrupt Flag Status and Clear */
#define EIC_NMIFLAG_RESETVALUE      0x00ul       /**< \brief (EIC_NMIFLAG reset_value) Non-Maskable Interrupt Flag Status and Clear */

#define EIC_NMIFLAG_NMI_Pos         0            /**< \brief (EIC_NMIFLAG) Non-Maskable Interrupt */
#define EIC_NMIFLAG_NMI             (0x1ul << EIC_NMIFLAG_NMI_Pos)
#define EIC_NMIFLAG_MASK            0x01ul       /**< \brief (EIC_NMIFLAG) MASK Register */

/* -------- EIC_EVCTRL : (EIC Offset: 0x04) (R/W 32) Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EXTINTEO0:1;      /*!< bit:      0  External Interrupt 0 Event Output Enable */
    uint32_t EXTINTEO1:1;      /*!< bit:      1  External Interrupt 1 Event Output Enable */
    uint32_t EXTINTEO2:1;      /*!< bit:      2  External Interrupt 2 Event Output Enable */
    uint32_t EXTINTEO3:1;      /*!< bit:      3  External Interrupt 3 Event Output Enable */
    uint32_t EXTINTEO4:1;      /*!< bit:      4  External Interrupt 4 Event Output Enable */
    uint32_t EXTINTEO5:1;      /*!< bit:      5  External Interrupt 5 Event Output Enable */
    uint32_t EXTINTEO6:1;      /*!< bit:      6  External Interrupt 6 Event Output Enable */
    uint32_t EXTINTEO7:1;      /*!< bit:      7  External Interrupt 7 Event Output Enable */
    uint32_t EXTINTEO8:1;      /*!< bit:      8  External Interrupt 8 Event Output Enable */
    uint32_t EXTINTEO9:1;      /*!< bit:      9  External Interrupt 9 Event Output Enable */
    uint32_t EXTINTEO10:1;     /*!< bit:     10  External Interrupt 10 Event Output Enable */
    uint32_t EXTINTEO11:1;     /*!< bit:     11  External Interrupt 11 Event Output Enable */
    uint32_t EXTINTEO12:1;     /*!< bit:     12  External Interrupt 12 Event Output Enable */
    uint32_t EXTINTEO13:1;     /*!< bit:     13  External Interrupt 13 Event Output Enable */
    uint32_t EXTINTEO14:1;     /*!< bit:     14  External Interrupt 14 Event Output Enable */
    uint32_t EXTINTEO15:1;     /*!< bit:     15  External Interrupt 15 Event Output Enable */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t EXTINTEO:16;      /*!< bit:  0..15  External Interrupt x Event Output Enable */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_EVCTRL_OFFSET           0x04         /**< \brief (EIC_EVCTRL offset) Event Control */
#define EIC_EVCTRL_RESETVALUE       0x00000000ul /**< \brief (EIC_EVCTRL reset_value) Event Control */

#define EIC_EVCTRL_EXTINTEO0_Pos    0            /**< \brief (EIC_EVCTRL) External Interrupt 0 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO0        (1 << EIC_EVCTRL_EXTINTEO0_Pos)
#define EIC_EVCTRL_EXTINTEO1_Pos    1            /**< \brief (EIC_EVCTRL) External Interrupt 1 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO1        (1 << EIC_EVCTRL_EXTINTEO1_Pos)
#define EIC_EVCTRL_EXTINTEO2_Pos    2            /**< \brief (EIC_EVCTRL) External Interrupt 2 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO2        (1 << EIC_EVCTRL_EXTINTEO2_Pos)
#define EIC_EVCTRL_EXTINTEO3_Pos    3            /**< \brief (EIC_EVCTRL) External Interrupt 3 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO3        (1 << EIC_EVCTRL_EXTINTEO3_Pos)
#define EIC_EVCTRL_EXTINTEO4_Pos    4            /**< \brief (EIC_EVCTRL) External Interrupt 4 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO4        (1 << EIC_EVCTRL_EXTINTEO4_Pos)
#define EIC_EVCTRL_EXTINTEO5_Pos    5            /**< \brief (EIC_EVCTRL) External Interrupt 5 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO5        (1 << EIC_EVCTRL_EXTINTEO5_Pos)
#define EIC_EVCTRL_EXTINTEO6_Pos    6            /**< \brief (EIC_EVCTRL) External Interrupt 6 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO6        (1 << EIC_EVCTRL_EXTINTEO6_Pos)
#define EIC_EVCTRL_EXTINTEO7_Pos    7            /**< \brief (EIC_EVCTRL) External Interrupt 7 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO7        (1 << EIC_EVCTRL_EXTINTEO7_Pos)
#define EIC_EVCTRL_EXTINTEO8_Pos    8            /**< \brief (EIC_EVCTRL) External Interrupt 8 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO8        (1 << EIC_EVCTRL_EXTINTEO8_Pos)
#define EIC_EVCTRL_EXTINTEO9_Pos    9            /**< \brief (EIC_EVCTRL) External Interrupt 9 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO9        (1 << EIC_EVCTRL_EXTINTEO9_Pos)
#define EIC_EVCTRL_EXTINTEO10_Pos   10           /**< \brief (EIC_EVCTRL) External Interrupt 10 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO10       (1 << EIC_EVCTRL_EXTINTEO10_Pos)
#define EIC_EVCTRL_EXTINTEO11_Pos   11           /**< \brief (EIC_EVCTRL) External Interrupt 11 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO11       (1 << EIC_EVCTRL_EXTINTEO11_Pos)
#define EIC_EVCTRL_EXTINTEO12_Pos   12           /**< \brief (EIC_EVCTRL) External Interrupt 12 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO12       (1 << EIC_EVCTRL_EXTINTEO12_Pos)
#define EIC_EVCTRL_EXTINTEO13_Pos   13           /**< \brief (EIC_EVCTRL) External Interrupt 13 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO13       (1 << EIC_EVCTRL_EXTINTEO13_Pos)
#define EIC_EVCTRL_EXTINTEO14_Pos   14           /**< \brief (EIC_EVCTRL) External Interrupt 14 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO14       (1 << EIC_EVCTRL_EXTINTEO14_Pos)
#define EIC_EVCTRL_EXTINTEO15_Pos   15           /**< \brief (EIC_EVCTRL) External Interrupt 15 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO15       (1 << EIC_EVCTRL_EXTINTEO15_Pos)
#define EIC_EVCTRL_EXTINTEO_Pos     0            /**< \brief (EIC_EVCTRL) External Interrupt x Event Output Enable */
#define EIC_EVCTRL_EXTINTEO_Msk     (0xFFFFul << EIC_EVCTRL_EXTINTEO_Pos)
#define EIC_EVCTRL_EXTINTEO(value)  (EIC_EVCTRL_EXTINTEO_Msk & ((value) << EIC_EVCTRL_EXTINTEO_Pos))
#define EIC_EVCTRL_MASK             0x0000FFFFul /**< \brief (EIC_EVCTRL) MASK Register */

/* -------- EIC_INTENCLR : (EIC Offset: 0x08) (R/W 32) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EXTINT0:1;        /*!< bit:      0  External Interrupt 0 Enable        */
    uint32_t EXTINT1:1;        /*!< bit:      1  External Interrupt 1 Enable        */
    uint32_t EXTINT2:1;        /*!< bit:      2  External Interrupt 2 Enable        */
    uint32_t EXTINT3:1;        /*!< bit:      3  External Interrupt 3 Enable        */
    uint32_t EXTINT4:1;        /*!< bit:      4  External Interrupt 4 Enable        */
    uint32_t EXTINT5:1;        /*!< bit:      5  External Interrupt 5 Enable        */
    uint32_t EXTINT6:1;        /*!< bit:      6  External Interrupt 6 Enable        */
    uint32_t EXTINT7:1;        /*!< bit:      7  External Interrupt 7 Enable        */
    uint32_t EXTINT8:1;        /*!< bit:      8  External Interrupt 8 Enable        */
    uint32_t EXTINT9:1;        /*!< bit:      9  External Interrupt 9 Enable        */
    uint32_t EXTINT10:1;       /*!< bit:     10  External Interrupt 10 Enable       */
    uint32_t EXTINT11:1;       /*!< bit:     11  External Interrupt 11 Enable       */
    uint32_t EXTINT12:1;       /*!< bit:     12  External Interrupt 12 Enable       */
    uint32_t EXTINT13:1;       /*!< bit:     13  External Interrupt 13 Enable       */
    uint32_t EXTINT14:1;       /*!< bit:     14  External Interrupt 14 Enable       */
    uint32_t EXTINT15:1;       /*!< bit:     15  External Interrupt 15 Enable       */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t EXTINT:16;        /*!< bit:  0..15  External Interrupt x Enable        */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_INTENCLR_OFFSET         0x08         /**< \brief (EIC_INTENCLR offset) Interrupt Enable Clear */
#define EIC_INTENCLR_RESETVALUE     0x00000000ul /**< \brief (EIC_INTENCLR reset_value) Interrupt Enable Clear */

#define EIC_INTENCLR_EXTINT0_Pos    0            /**< \brief (EIC_INTENCLR) External Interrupt 0 Enable */
#define EIC_INTENCLR_EXTINT0        (1 << EIC_INTENCLR_EXTINT0_Pos)
#define EIC_INTENCLR_EXTINT1_Pos    1            /**< \brief (EIC_INTENCLR) External Interrupt 1 Enable */
#define EIC_INTENCLR_EXTINT1        (1 << EIC_INTENCLR_EXTINT1_Pos)
#define EIC_INTENCLR_EXTINT2_Pos    2            /**< \brief (EIC_INTENCLR) External Interrupt 2 Enable */
#define EIC_INTENCLR_EXTINT2        (1 << EIC_INTENCLR_EXTINT2_Pos)
#define EIC_INTENCLR_EXTINT3_Pos    3            /**< \brief (EIC_INTENCLR) External Interrupt 3 Enable */
#define EIC_INTENCLR_EXTINT3        (1 << EIC_INTENCLR_EXTINT3_Pos)
#define EIC_INTENCLR_EXTINT4_Pos    4            /**< \brief (EIC_INTENCLR) External Interrupt 4 Enable */
#define EIC_INTENCLR_EXTINT4        (1 << EIC_INTENCLR_EXTINT4_Pos)
#define EIC_INTENCLR_EXTINT5_Pos    5            /**< \brief (EIC_INTENCLR) External Interrupt 5 Enable */
#define EIC_INTENCLR_EXTINT5        (1 << EIC_INTENCLR_EXTINT5_Pos)
#define EIC_INTENCLR_EXTINT6_Pos    6            /**< \brief (EIC_INTENCLR) External Interrupt 6 Enable */
#define EIC_INTENCLR_EXTINT6        (1 << EIC_INTENCLR_EXTINT6_Pos)
#define EIC_INTENCLR_EXTINT7_Pos    7            /**< \brief (EIC_INTENCLR) External Interrupt 7 Enable */
#define EIC_INTENCLR_EXTINT7        (1 << EIC_INTENCLR_EXTINT7_Pos)
#define EIC_INTENCLR_EXTINT8_Pos    8            /**< \brief (EIC_INTENCLR) External Interrupt 8 Enable */
#define EIC_INTENCLR_EXTINT8        (1 << EIC_INTENCLR_EXTINT8_Pos)
#define EIC_INTENCLR_EXTINT9_Pos    9            /**< \brief (EIC_INTENCLR) External Interrupt 9 Enable */
#define EIC_INTENCLR_EXTINT9        (1 << EIC_INTENCLR_EXTINT9_Pos)
#define EIC_INTENCLR_EXTINT10_Pos   10           /**< \brief (EIC_INTENCLR) External Interrupt 10 Enable */
#define EIC_INTENCLR_EXTINT10       (1 << EIC_INTENCLR_EXTINT10_Pos)
#define EIC_INTENCLR_EXTINT11_Pos   11           /**< \brief (EIC_INTENCLR) External Interrupt 11 Enable */
#define EIC_INTENCLR_EXTINT11       (1 << EIC_INTENCLR_EXTINT11_Pos)
#define EIC_INTENCLR_EXTINT12_Pos   12           /**< \brief (EIC_INTENCLR) External Interrupt 12 Enable */
#define EIC_INTENCLR_EXTINT12       (1 << EIC_INTENCLR_EXTINT12_Pos)
#define EIC_INTENCLR_EXTINT13_Pos   13           /**< \brief (EIC_INTENCLR) External Interrupt 13 Enable */
#define EIC_INTENCLR_EXTINT13       (1 << EIC_INTENCLR_EXTINT13_Pos)
#define EIC_INTENCLR_EXTINT14_Pos   14           /**< \brief (EIC_INTENCLR) External Interrupt 14 Enable */
#define EIC_INTENCLR_EXTINT14       (1 << EIC_INTENCLR_EXTINT14_Pos)
#define EIC_INTENCLR_EXTINT15_Pos   15           /**< \brief (EIC_INTENCLR) External Interrupt 15 Enable */
#define EIC_INTENCLR_EXTINT15       (1 << EIC_INTENCLR_EXTINT15_Pos)
#define EIC_INTENCLR_EXTINT_Pos     0            /**< \brief (EIC_INTENCLR) External Interrupt x Enable */
#define EIC_INTENCLR_EXTINT_Msk     (0xFFFFul << EIC_INTENCLR_EXTINT_Pos)
#define EIC_INTENCLR_EXTINT(value)  (EIC_INTENCLR_EXTINT_Msk & ((value) << EIC_INTENCLR_EXTINT_Pos))
#define EIC_INTENCLR_MASK           0x0000FFFFul /**< \brief (EIC_INTENCLR) MASK Register */

/* -------- EIC_INTENSET : (EIC Offset: 0x0C) (R/W 32) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EXTINT0:1;        /*!< bit:      0  External Interrupt 0 Enable        */
    uint32_t EXTINT1:1;        /*!< bit:      1  External Interrupt 1 Enable        */
    uint32_t EXTINT2:1;        /*!< bit:      2  External Interrupt 2 Enable        */
    uint32_t EXTINT3:1;        /*!< bit:      3  External Interrupt 3 Enable        */
    uint32_t EXTINT4:1;        /*!< bit:      4  External Interrupt 4 Enable        */
    uint32_t EXTINT5:1;        /*!< bit:      5  External Interrupt 5 Enable        */
    uint32_t EXTINT6:1;        /*!< bit:      6  External Interrupt 6 Enable        */
    uint32_t EXTINT7:1;        /*!< bit:      7  External Interrupt 7 Enable        */
    uint32_t EXTINT8:1;        /*!< bit:      8  External Interrupt 8 Enable        */
    uint32_t EXTINT9:1;        /*!< bit:      9  External Interrupt 9 Enable        */
    uint32_t EXTINT10:1;       /*!< bit:     10  External Interrupt 10 Enable       */
    uint32_t EXTINT11:1;       /*!< bit:     11  External Interrupt 11 Enable       */
    uint32_t EXTINT12:1;       /*!< bit:     12  External Interrupt 12 Enable       */
    uint32_t EXTINT13:1;       /*!< bit:     13  External Interrupt 13 Enable       */
    uint32_t EXTINT14:1;       /*!< bit:     14  External Interrupt 14 Enable       */
    uint32_t EXTINT15:1;       /*!< bit:     15  External Interrupt 15 Enable       */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t EXTINT:16;        /*!< bit:  0..15  External Interrupt x Enable        */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_INTENSET_OFFSET         0x0C         /**< \brief (EIC_INTENSET offset) Interrupt Enable Set */
#define EIC_INTENSET_RESETVALUE     0x00000000ul /**< \brief (EIC_INTENSET reset_value) Interrupt Enable Set */

#define EIC_INTENSET_EXTINT0_Pos    0            /**< \brief (EIC_INTENSET) External Interrupt 0 Enable */
#define EIC_INTENSET_EXTINT0        (1 << EIC_INTENSET_EXTINT0_Pos)
#define EIC_INTENSET_EXTINT1_Pos    1            /**< \brief (EIC_INTENSET) External Interrupt 1 Enable */
#define EIC_INTENSET_EXTINT1        (1 << EIC_INTENSET_EXTINT1_Pos)
#define EIC_INTENSET_EXTINT2_Pos    2            /**< \brief (EIC_INTENSET) External Interrupt 2 Enable */
#define EIC_INTENSET_EXTINT2        (1 << EIC_INTENSET_EXTINT2_Pos)
#define EIC_INTENSET_EXTINT3_Pos    3            /**< \brief (EIC_INTENSET) External Interrupt 3 Enable */
#define EIC_INTENSET_EXTINT3        (1 << EIC_INTENSET_EXTINT3_Pos)
#define EIC_INTENSET_EXTINT4_Pos    4            /**< \brief (EIC_INTENSET) External Interrupt 4 Enable */
#define EIC_INTENSET_EXTINT4        (1 << EIC_INTENSET_EXTINT4_Pos)
#define EIC_INTENSET_EXTINT5_Pos    5            /**< \brief (EIC_INTENSET) External Interrupt 5 Enable */
#define EIC_INTENSET_EXTINT5        (1 << EIC_INTENSET_EXTINT5_Pos)
#define EIC_INTENSET_EXTINT6_Pos    6            /**< \brief (EIC_INTENSET) External Interrupt 6 Enable */
#define EIC_INTENSET_EXTINT6        (1 << EIC_INTENSET_EXTINT6_Pos)
#define EIC_INTENSET_EXTINT7_Pos    7            /**< \brief (EIC_INTENSET) External Interrupt 7 Enable */
#define EIC_INTENSET_EXTINT7        (1 << EIC_INTENSET_EXTINT7_Pos)
#define EIC_INTENSET_EXTINT8_Pos    8            /**< \brief (EIC_INTENSET) External Interrupt 8 Enable */
#define EIC_INTENSET_EXTINT8        (1 << EIC_INTENSET_EXTINT8_Pos)
#define EIC_INTENSET_EXTINT9_Pos    9            /**< \brief (EIC_INTENSET) External Interrupt 9 Enable */
#define EIC_INTENSET_EXTINT9        (1 << EIC_INTENSET_EXTINT9_Pos)
#define EIC_INTENSET_EXTINT10_Pos   10           /**< \brief (EIC_INTENSET) External Interrupt 10 Enable */
#define EIC_INTENSET_EXTINT10       (1 << EIC_INTENSET_EXTINT10_Pos)
#define EIC_INTENSET_EXTINT11_Pos   11           /**< \brief (EIC_INTENSET) External Interrupt 11 Enable */
#define EIC_INTENSET_EXTINT11       (1 << EIC_INTENSET_EXTINT11_Pos)
#define EIC_INTENSET_EXTINT12_Pos   12           /**< \brief (EIC_INTENSET) External Interrupt 12 Enable */
#define EIC_INTENSET_EXTINT12       (1 << EIC_INTENSET_EXTINT12_Pos)
#define EIC_INTENSET_EXTINT13_Pos   13           /**< \brief (EIC_INTENSET) External Interrupt 13 Enable */
#define EIC_INTENSET_EXTINT13       (1 << EIC_INTENSET_EXTINT13_Pos)
#define EIC_INTENSET_EXTINT14_Pos   14           /**< \brief (EIC_INTENSET) External Interrupt 14 Enable */
#define EIC_INTENSET_EXTINT14       (1 << EIC_INTENSET_EXTINT14_Pos)
#define EIC_INTENSET_EXTINT15_Pos   15           /**< \brief (EIC_INTENSET) External Interrupt 15 Enable */
#define EIC_INTENSET_EXTINT15       (1 << EIC_INTENSET_EXTINT15_Pos)
#define EIC_INTENSET_EXTINT_Pos     0            /**< \brief (EIC_INTENSET) External Interrupt x Enable */
#define EIC_INTENSET_EXTINT_Msk     (0xFFFFul << EIC_INTENSET_EXTINT_Pos)
#define EIC_INTENSET_EXTINT(value)  (EIC_INTENSET_EXTINT_Msk & ((value) << EIC_INTENSET_EXTINT_Pos))
#define EIC_INTENSET_MASK           0x0000FFFFul /**< \brief (EIC_INTENSET) MASK Register */

/* -------- EIC_INTFLAG : (EIC Offset: 0x10) (R/W 32) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t EXTINT0:1;        /*!< bit:      0  External Interrupt 0               */
    __I uint32_t EXTINT1:1;        /*!< bit:      1  External Interrupt 1               */
    __I uint32_t EXTINT2:1;        /*!< bit:      2  External Interrupt 2               */
    __I uint32_t EXTINT3:1;        /*!< bit:      3  External Interrupt 3               */
    __I uint32_t EXTINT4:1;        /*!< bit:      4  External Interrupt 4               */
    __I uint32_t EXTINT5:1;        /*!< bit:      5  External Interrupt 5               */
    __I uint32_t EXTINT6:1;        /*!< bit:      6  External Interrupt 6               */
    __I uint32_t EXTINT7:1;        /*!< bit:      7  External Interrupt 7               */
    __I uint32_t EXTINT8:1;        /*!< bit:      8  External Interrupt 8               */
    __I uint32_t EXTINT9:1;        /*!< bit:      9  External Interrupt 9               */
    __I uint32_t EXTINT10:1;       /*!< bit:     10  External Interrupt 10              */
    __I uint32_t EXTINT11:1;       /*!< bit:     11  External Interrupt 11              */
    __I uint32_t EXTINT12:1;       /*!< bit:     12  External Interrupt 12              */
    __I uint32_t EXTINT13:1;       /*!< bit:     13  External Interrupt 13              */
    __I uint32_t EXTINT14:1;       /*!< bit:     14  External Interrupt 14              */
    __I uint32_t EXTINT15:1;       /*!< bit:     15  External Interrupt 15              */
    __I uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint32_t EXTINT:16;        /*!< bit:  0..15  External Interrupt x               */
    __I uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_INTFLAG_OFFSET          0x10         /**< \brief (EIC_INTFLAG offset) Interrupt Flag Status and Clear */
#define EIC_INTFLAG_RESETVALUE      0x00000000ul /**< \brief (EIC_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define EIC_INTFLAG_EXTINT0_Pos     0            /**< \brief (EIC_INTFLAG) External Interrupt 0 */
#define EIC_INTFLAG_EXTINT0         (1 << EIC_INTFLAG_EXTINT0_Pos)
#define EIC_INTFLAG_EXTINT1_Pos     1            /**< \brief (EIC_INTFLAG) External Interrupt 1 */
#define EIC_INTFLAG_EXTINT1         (1 << EIC_INTFLAG_EXTINT1_Pos)
#define EIC_INTFLAG_EXTINT2_Pos     2            /**< \brief (EIC_INTFLAG) External Interrupt 2 */
#define EIC_INTFLAG_EXTINT2         (1 << EIC_INTFLAG_EXTINT2_Pos)
#define EIC_INTFLAG_EXTINT3_Pos     3            /**< \brief (EIC_INTFLAG) External Interrupt 3 */
#define EIC_INTFLAG_EXTINT3         (1 << EIC_INTFLAG_EXTINT3_Pos)
#define EIC_INTFLAG_EXTINT4_Pos     4            /**< \brief (EIC_INTFLAG) External Interrupt 4 */
#define EIC_INTFLAG_EXTINT4         (1 << EIC_INTFLAG_EXTINT4_Pos)
#define EIC_INTFLAG_EXTINT5_Pos     5            /**< \brief (EIC_INTFLAG) External Interrupt 5 */
#define EIC_INTFLAG_EXTINT5         (1 << EIC_INTFLAG_EXTINT5_Pos)
#define EIC_INTFLAG_EXTINT6_Pos     6            /**< \brief (EIC_INTFLAG) External Interrupt 6 */
#define EIC_INTFLAG_EXTINT6         (1 << EIC_INTFLAG_EXTINT6_Pos)
#define EIC_INTFLAG_EXTINT7_Pos     7            /**< \brief (EIC_INTFLAG) External Interrupt 7 */
#define EIC_INTFLAG_EXTINT7         (1 << EIC_INTFLAG_EXTINT7_Pos)
#define EIC_INTFLAG_EXTINT8_Pos     8            /**< \brief (EIC_INTFLAG) External Interrupt 8 */
#define EIC_INTFLAG_EXTINT8         (1 << EIC_INTFLAG_EXTINT8_Pos)
#define EIC_INTFLAG_EXTINT9_Pos     9            /**< \brief (EIC_INTFLAG) External Interrupt 9 */
#define EIC_INTFLAG_EXTINT9         (1 << EIC_INTFLAG_EXTINT9_Pos)
#define EIC_INTFLAG_EXTINT10_Pos    10           /**< \brief (EIC_INTFLAG) External Interrupt 10 */
#define EIC_INTFLAG_EXTINT10        (1 << EIC_INTFLAG_EXTINT10_Pos)
#define EIC_INTFLAG_EXTINT11_Pos    11           /**< \brief (EIC_INTFLAG) External Interrupt 11 */
#define EIC_INTFLAG_EXTINT11        (1 << EIC_INTFLAG_EXTINT11_Pos)
#define EIC_INTFLAG_EXTINT12_Pos    12           /**< \brief (EIC_INTFLAG) External Interrupt 12 */
#define EIC_INTFLAG_EXTINT12        (1 << EIC_INTFLAG_EXTINT12_Pos)
#define EIC_INTFLAG_EXTINT13_Pos    13           /**< \brief (EIC_INTFLAG) External Interrupt 13 */
#define EIC_INTFLAG_EXTINT13        (1 << EIC_INTFLAG_EXTINT13_Pos)
#define EIC_INTFLAG_EXTINT14_Pos    14           /**< \brief (EIC_INTFLAG) External Interrupt 14 */
#define EIC_INTFLAG_EXTINT14        (1 << EIC_INTFLAG_EXTINT14_Pos)
#define EIC_INTFLAG_EXTINT15_Pos    15           /**< \brief (EIC_INTFLAG) External Interrupt 15 */
#define EIC_INTFLAG_EXTINT15        (1 << EIC_INTFLAG_EXTINT15_Pos)
#define EIC_INTFLAG_EXTINT_Pos      0            /**< \brief (EIC_INTFLAG) External Interrupt x */
#define EIC_INTFLAG_EXTINT_Msk      (0xFFFFul << EIC_INTFLAG_EXTINT_Pos)
#define EIC_INTFLAG_EXTINT(value)   (EIC_INTFLAG_EXTINT_Msk & ((value) << EIC_INTFLAG_EXTINT_Pos))
#define EIC_INTFLAG_MASK            0x0000FFFFul /**< \brief (EIC_INTFLAG) MASK Register */

/* -------- EIC_WAKEUP : (EIC Offset: 0x14) (R/W 32) Wake-Up Enable -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t WAKEUPEN0:1;      /*!< bit:      0  External Interrupt 0 Wake-up Enable */
    uint32_t WAKEUPEN1:1;      /*!< bit:      1  External Interrupt 1 Wake-up Enable */
    uint32_t WAKEUPEN2:1;      /*!< bit:      2  External Interrupt 2 Wake-up Enable */
    uint32_t WAKEUPEN3:1;      /*!< bit:      3  External Interrupt 3 Wake-up Enable */
    uint32_t WAKEUPEN4:1;      /*!< bit:      4  External Interrupt 4 Wake-up Enable */
    uint32_t WAKEUPEN5:1;      /*!< bit:      5  External Interrupt 5 Wake-up Enable */
    uint32_t WAKEUPEN6:1;      /*!< bit:      6  External Interrupt 6 Wake-up Enable */
    uint32_t WAKEUPEN7:1;      /*!< bit:      7  External Interrupt 7 Wake-up Enable */
    uint32_t WAKEUPEN8:1;      /*!< bit:      8  External Interrupt 8 Wake-up Enable */
    uint32_t WAKEUPEN9:1;      /*!< bit:      9  External Interrupt 9 Wake-up Enable */
    uint32_t WAKEUPEN10:1;     /*!< bit:     10  External Interrupt 10 Wake-up Enable */
    uint32_t WAKEUPEN11:1;     /*!< bit:     11  External Interrupt 11 Wake-up Enable */
    uint32_t WAKEUPEN12:1;     /*!< bit:     12  External Interrupt 12 Wake-up Enable */
    uint32_t WAKEUPEN13:1;     /*!< bit:     13  External Interrupt 13 Wake-up Enable */
    uint32_t WAKEUPEN14:1;     /*!< bit:     14  External Interrupt 14 Wake-up Enable */
    uint32_t WAKEUPEN15:1;     /*!< bit:     15  External Interrupt 15 Wake-up Enable */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t WAKEUPEN:16;      /*!< bit:  0..15  External Interrupt x Wake-up Enable */
    uint32_t :16;              /*!< bit: 16..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_WAKEUP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_WAKEUP_OFFSET           0x14         /**< \brief (EIC_WAKEUP offset) Wake-Up Enable */
#define EIC_WAKEUP_RESETVALUE       0x00000000ul /**< \brief (EIC_WAKEUP reset_value) Wake-Up Enable */

#define EIC_WAKEUP_WAKEUPEN0_Pos    0            /**< \brief (EIC_WAKEUP) External Interrupt 0 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN0        (1 << EIC_WAKEUP_WAKEUPEN0_Pos)
#define EIC_WAKEUP_WAKEUPEN1_Pos    1            /**< \brief (EIC_WAKEUP) External Interrupt 1 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN1        (1 << EIC_WAKEUP_WAKEUPEN1_Pos)
#define EIC_WAKEUP_WAKEUPEN2_Pos    2            /**< \brief (EIC_WAKEUP) External Interrupt 2 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN2        (1 << EIC_WAKEUP_WAKEUPEN2_Pos)
#define EIC_WAKEUP_WAKEUPEN3_Pos    3            /**< \brief (EIC_WAKEUP) External Interrupt 3 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN3        (1 << EIC_WAKEUP_WAKEUPEN3_Pos)
#define EIC_WAKEUP_WAKEUPEN4_Pos    4            /**< \brief (EIC_WAKEUP) External Interrupt 4 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN4        (1 << EIC_WAKEUP_WAKEUPEN4_Pos)
#define EIC_WAKEUP_WAKEUPEN5_Pos    5            /**< \brief (EIC_WAKEUP) External Interrupt 5 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN5        (1 << EIC_WAKEUP_WAKEUPEN5_Pos)
#define EIC_WAKEUP_WAKEUPEN6_Pos    6            /**< \brief (EIC_WAKEUP) External Interrupt 6 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN6        (1 << EIC_WAKEUP_WAKEUPEN6_Pos)
#define EIC_WAKEUP_WAKEUPEN7_Pos    7            /**< \brief (EIC_WAKEUP) External Interrupt 7 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN7        (1 << EIC_WAKEUP_WAKEUPEN7_Pos)
#define EIC_WAKEUP_WAKEUPEN8_Pos    8            /**< \brief (EIC_WAKEUP) External Interrupt 8 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN8        (1 << EIC_WAKEUP_WAKEUPEN8_Pos)
#define EIC_WAKEUP_WAKEUPEN9_Pos    9            /**< \brief (EIC_WAKEUP) External Interrupt 9 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN9        (1 << EIC_WAKEUP_WAKEUPEN9_Pos)
#define EIC_WAKEUP_WAKEUPEN10_Pos   10           /**< \brief (EIC_WAKEUP) External Interrupt 10 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN10       (1 << EIC_WAKEUP_WAKEUPEN10_Pos)
#define EIC_WAKEUP_WAKEUPEN11_Pos   11           /**< \brief (EIC_WAKEUP) External Interrupt 11 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN11       (1 << EIC_WAKEUP_WAKEUPEN11_Pos)
#define EIC_WAKEUP_WAKEUPEN12_Pos   12           /**< \brief (EIC_WAKEUP) External Interrupt 12 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN12       (1 << EIC_WAKEUP_WAKEUPEN12_Pos)
#define EIC_WAKEUP_WAKEUPEN13_Pos   13           /**< \brief (EIC_WAKEUP) External Interrupt 13 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN13       (1 << EIC_WAKEUP_WAKEUPEN13_Pos)
#define EIC_WAKEUP_WAKEUPEN14_Pos   14           /**< \brief (EIC_WAKEUP) External Interrupt 14 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN14       (1 << EIC_WAKEUP_WAKEUPEN14_Pos)
#define EIC_WAKEUP_WAKEUPEN15_Pos   15           /**< \brief (EIC_WAKEUP) External Interrupt 15 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN15       (1 << EIC_WAKEUP_WAKEUPEN15_Pos)
#define EIC_WAKEUP_WAKEUPEN_Pos     0            /**< \brief (EIC_WAKEUP) External Interrupt x Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN_Msk     (0xFFFFul << EIC_WAKEUP_WAKEUPEN_Pos)
#define EIC_WAKEUP_WAKEUPEN(value)  (EIC_WAKEUP_WAKEUPEN_Msk & ((value) << EIC_WAKEUP_WAKEUPEN_Pos))
#define EIC_WAKEUP_MASK             0x0000FFFFul /**< \brief (EIC_WAKEUP) MASK Register */

/* -------- EIC_CONFIG : (EIC Offset: 0x18) (R/W 32) Configuration n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SENSE0:3;         /*!< bit:  0.. 2  Input Sense 0 Configuration        */
    uint32_t FILTEN0:1;        /*!< bit:      3  Filter 0 Enable                    */
    uint32_t SENSE1:3;         /*!< bit:  4.. 6  Input Sense 1 Configuration        */
    uint32_t FILTEN1:1;        /*!< bit:      7  Filter 1 Enable                    */
    uint32_t SENSE2:3;         /*!< bit:  8..10  Input Sense 2 Configuration        */
    uint32_t FILTEN2:1;        /*!< bit:     11  Filter 2 Enable                    */
    uint32_t SENSE3:3;         /*!< bit: 12..14  Input Sense 3 Configuration        */
    uint32_t FILTEN3:1;        /*!< bit:     15  Filter 3 Enable                    */
    uint32_t SENSE4:3;         /*!< bit: 16..18  Input Sense 4 Configuration        */
    uint32_t FILTEN4:1;        /*!< bit:     19  Filter 4 Enable                    */
    uint32_t SENSE5:3;         /*!< bit: 20..22  Input Sense 5 Configuration        */
    uint32_t FILTEN5:1;        /*!< bit:     23  Filter 5 Enable                    */
    uint32_t SENSE6:3;         /*!< bit: 24..26  Input Sense 6 Configuration        */
    uint32_t FILTEN6:1;        /*!< bit:     27  Filter 6 Enable                    */
    uint32_t SENSE7:3;         /*!< bit: 28..30  Input Sense 7 Configuration        */
    uint32_t FILTEN7:1;        /*!< bit:     31  Filter 7 Enable                    */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_CONFIG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_CONFIG_OFFSET           0x18         /**< \brief (EIC_CONFIG offset) Configuration n */
#define EIC_CONFIG_RESETVALUE       0x00000000ul /**< \brief (EIC_CONFIG reset_value) Configuration n */

#define EIC_CONFIG_SENSE0_Pos       0            /**< \brief (EIC_CONFIG) Input Sense 0 Configuration */
#define EIC_CONFIG_SENSE0_Msk       (0x7ul << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0(value)    (EIC_CONFIG_SENSE0_Msk & ((value) << EIC_CONFIG_SENSE0_Pos))
#define   EIC_CONFIG_SENSE0_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE0_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising-edge detection */
#define   EIC_CONFIG_SENSE0_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling-edge detection */
#define   EIC_CONFIG_SENSE0_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both-edges detection */
#define   EIC_CONFIG_SENSE0_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High-level detection */
#define   EIC_CONFIG_SENSE0_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low-level detection */
#define EIC_CONFIG_SENSE0_NONE      (EIC_CONFIG_SENSE0_NONE_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_RISE      (EIC_CONFIG_SENSE0_RISE_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_FALL      (EIC_CONFIG_SENSE0_FALL_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_BOTH      (EIC_CONFIG_SENSE0_BOTH_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_HIGH      (EIC_CONFIG_SENSE0_HIGH_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_LOW       (EIC_CONFIG_SENSE0_LOW_Val     << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_FILTEN0_Pos      3            /**< \brief (EIC_CONFIG) Filter 0 Enable */
#define EIC_CONFIG_FILTEN0          (0x1ul << EIC_CONFIG_FILTEN0_Pos)
#define EIC_CONFIG_SENSE1_Pos       4            /**< \brief (EIC_CONFIG) Input Sense 1 Configuration */
#define EIC_CONFIG_SENSE1_Msk       (0x7ul << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1(value)    (EIC_CONFIG_SENSE1_Msk & ((value) << EIC_CONFIG_SENSE1_Pos))
#define   EIC_CONFIG_SENSE1_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE1_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE1_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE1_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE1_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE1_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE1_NONE      (EIC_CONFIG_SENSE1_NONE_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_RISE      (EIC_CONFIG_SENSE1_RISE_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_FALL      (EIC_CONFIG_SENSE1_FALL_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_BOTH      (EIC_CONFIG_SENSE1_BOTH_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_HIGH      (EIC_CONFIG_SENSE1_HIGH_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_LOW       (EIC_CONFIG_SENSE1_LOW_Val     << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_FILTEN1_Pos      7            /**< \brief (EIC_CONFIG) Filter 1 Enable */
#define EIC_CONFIG_FILTEN1          (0x1ul << EIC_CONFIG_FILTEN1_Pos)
#define EIC_CONFIG_SENSE2_Pos       8            /**< \brief (EIC_CONFIG) Input Sense 2 Configuration */
#define EIC_CONFIG_SENSE2_Msk       (0x7ul << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2(value)    (EIC_CONFIG_SENSE2_Msk & ((value) << EIC_CONFIG_SENSE2_Pos))
#define   EIC_CONFIG_SENSE2_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE2_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE2_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE2_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE2_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE2_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE2_NONE      (EIC_CONFIG_SENSE2_NONE_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_RISE      (EIC_CONFIG_SENSE2_RISE_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_FALL      (EIC_CONFIG_SENSE2_FALL_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_BOTH      (EIC_CONFIG_SENSE2_BOTH_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_HIGH      (EIC_CONFIG_SENSE2_HIGH_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_LOW       (EIC_CONFIG_SENSE2_LOW_Val     << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_FILTEN2_Pos      11           /**< \brief (EIC_CONFIG) Filter 2 Enable */
#define EIC_CONFIG_FILTEN2          (0x1ul << EIC_CONFIG_FILTEN2_Pos)
#define EIC_CONFIG_SENSE3_Pos       12           /**< \brief (EIC_CONFIG) Input Sense 3 Configuration */
#define EIC_CONFIG_SENSE3_Msk       (0x7ul << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3(value)    (EIC_CONFIG_SENSE3_Msk & ((value) << EIC_CONFIG_SENSE3_Pos))
#define   EIC_CONFIG_SENSE3_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE3_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE3_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE3_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE3_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE3_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE3_NONE      (EIC_CONFIG_SENSE3_NONE_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_RISE      (EIC_CONFIG_SENSE3_RISE_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_FALL      (EIC_CONFIG_SENSE3_FALL_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_BOTH      (EIC_CONFIG_SENSE3_BOTH_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_HIGH      (EIC_CONFIG_SENSE3_HIGH_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_LOW       (EIC_CONFIG_SENSE3_LOW_Val     << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_FILTEN3_Pos      15           /**< \brief (EIC_CONFIG) Filter 3 Enable */
#define EIC_CONFIG_FILTEN3          (0x1ul << EIC_CONFIG_FILTEN3_Pos)
#define EIC_CONFIG_SENSE4_Pos       16           /**< \brief (EIC_CONFIG) Input Sense 4 Configuration */
#define EIC_CONFIG_SENSE4_Msk       (0x7ul << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4(value)    (EIC_CONFIG_SENSE4_Msk & ((value) << EIC_CONFIG_SENSE4_Pos))
#define   EIC_CONFIG_SENSE4_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE4_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE4_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE4_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE4_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE4_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE4_NONE      (EIC_CONFIG_SENSE4_NONE_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_RISE      (EIC_CONFIG_SENSE4_RISE_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_FALL      (EIC_CONFIG_SENSE4_FALL_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_BOTH      (EIC_CONFIG_SENSE4_BOTH_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_HIGH      (EIC_CONFIG_SENSE4_HIGH_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_LOW       (EIC_CONFIG_SENSE4_LOW_Val     << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_FILTEN4_Pos      19           /**< \brief (EIC_CONFIG) Filter 4 Enable */
#define EIC_CONFIG_FILTEN4          (0x1ul << EIC_CONFIG_FILTEN4_Pos)
#define EIC_CONFIG_SENSE5_Pos       20           /**< \brief (EIC_CONFIG) Input Sense 5 Configuration */
#define EIC_CONFIG_SENSE5_Msk       (0x7ul << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5(value)    (EIC_CONFIG_SENSE5_Msk & ((value) << EIC_CONFIG_SENSE5_Pos))
#define   EIC_CONFIG_SENSE5_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE5_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE5_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE5_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE5_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE5_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE5_NONE      (EIC_CONFIG_SENSE5_NONE_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_RISE      (EIC_CONFIG_SENSE5_RISE_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_FALL      (EIC_CONFIG_SENSE5_FALL_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_BOTH      (EIC_CONFIG_SENSE5_BOTH_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_HIGH      (EIC_CONFIG_SENSE5_HIGH_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_LOW       (EIC_CONFIG_SENSE5_LOW_Val     << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_FILTEN5_Pos      23           /**< \brief (EIC_CONFIG) Filter 5 Enable */
#define EIC_CONFIG_FILTEN5          (0x1ul << EIC_CONFIG_FILTEN5_Pos)
#define EIC_CONFIG_SENSE6_Pos       24           /**< \brief (EIC_CONFIG) Input Sense 6 Configuration */
#define EIC_CONFIG_SENSE6_Msk       (0x7ul << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6(value)    (EIC_CONFIG_SENSE6_Msk & ((value) << EIC_CONFIG_SENSE6_Pos))
#define   EIC_CONFIG_SENSE6_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE6_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE6_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE6_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE6_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE6_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE6_NONE      (EIC_CONFIG_SENSE6_NONE_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_RISE      (EIC_CONFIG_SENSE6_RISE_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_FALL      (EIC_CONFIG_SENSE6_FALL_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_BOTH      (EIC_CONFIG_SENSE6_BOTH_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_HIGH      (EIC_CONFIG_SENSE6_HIGH_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_LOW       (EIC_CONFIG_SENSE6_LOW_Val     << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_FILTEN6_Pos      27           /**< \brief (EIC_CONFIG) Filter 6 Enable */
#define EIC_CONFIG_FILTEN6          (0x1ul << EIC_CONFIG_FILTEN6_Pos)
#define EIC_CONFIG_SENSE7_Pos       28           /**< \brief (EIC_CONFIG) Input Sense 7 Configuration */
#define EIC_CONFIG_SENSE7_Msk       (0x7ul << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7(value)    (EIC_CONFIG_SENSE7_Msk & ((value) << EIC_CONFIG_SENSE7_Pos))
#define   EIC_CONFIG_SENSE7_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE7_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE7_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE7_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE7_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE7_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE7_NONE      (EIC_CONFIG_SENSE7_NONE_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_RISE      (EIC_CONFIG_SENSE7_RISE_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_FALL      (EIC_CONFIG_SENSE7_FALL_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_BOTH      (EIC_CONFIG_SENSE7_BOTH_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_HIGH      (EIC_CONFIG_SENSE7_HIGH_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_LOW       (EIC_CONFIG_SENSE7_LOW_Val     << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_FILTEN7_Pos      31           /**< \brief (EIC_CONFIG) Filter 7 Enable */
#define EIC_CONFIG_FILTEN7          (0x1ul << EIC_CONFIG_FILTEN7_Pos)
#define EIC_CONFIG_MASK             0xFFFFFFFFul /**< \brief (EIC_CONFIG) MASK Register */

/** \brief EIC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO EIC_CTRL_Type             CTRL;        /**< \brief Offset: 0x00 (R/W  8) Control */
  __I  EIC_STATUS_Type           STATUS;      /**< \brief Offset: 0x01 (R/   8) Status */
  __IO EIC_NMICTRL_Type          NMICTRL;     /**< \brief Offset: 0x02 (R/W  8) Non-Maskable Interrupt Control */
  __IO EIC_NMIFLAG_Type          NMIFLAG;     /**< \brief Offset: 0x03 (R/W  8) Non-Maskable Interrupt Flag Status and Clear */
  __IO EIC_EVCTRL_Type           EVCTRL;      /**< \brief Offset: 0x04 (R/W 32) Event Control */
  __IO EIC_INTENCLR_Type         INTENCLR;    /**< \brief Offset: 0x08 (R/W 32) Interrupt Enable Clear */
  __IO EIC_INTENSET_Type         INTENSET;    /**< \brief Offset: 0x0C (R/W 32) Interrupt Enable Set */
  __IO EIC_INTFLAG_Type          INTFLAG;     /**< \brief Offset: 0x10 (R/W 32) Interrupt Flag Status and Clear */
  __IO EIC_WAKEUP_Type           WAKEUP;      /**< \brief Offset: 0x14 (R/W 32) Wake-Up Enable */
  __IO EIC_CONFIG_Type           CONFIG[2];   /**< \brief Offset: 0x18 (R/W 32) Configuration n */
} Eic;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD21_EIC_COMPONENT_ */
