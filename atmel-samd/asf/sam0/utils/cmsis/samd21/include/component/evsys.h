/**
 * \file
 *
 * \brief Component description for EVSYS
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

#ifndef _SAMD21_EVSYS_COMPONENT_
#define _SAMD21_EVSYS_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR EVSYS */
/* ========================================================================== */
/** \addtogroup SAMD21_EVSYS Event System Interface */
/*@{*/

#define EVSYS_U2208
#define REV_EVSYS                   0x101

/* -------- EVSYS_CTRL : (EVSYS Offset: 0x00) ( /W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  :3;               /*!< bit:  1.. 3  Reserved                           */
    uint8_t  GCLKREQ:1;        /*!< bit:      4  Generic Clock Requests             */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EVSYS_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_CTRL_OFFSET           0x00         /**< \brief (EVSYS_CTRL offset) Control */
#define EVSYS_CTRL_RESETVALUE       0x00ul       /**< \brief (EVSYS_CTRL reset_value) Control */

#define EVSYS_CTRL_SWRST_Pos        0            /**< \brief (EVSYS_CTRL) Software Reset */
#define EVSYS_CTRL_SWRST            (0x1ul << EVSYS_CTRL_SWRST_Pos)
#define EVSYS_CTRL_GCLKREQ_Pos      4            /**< \brief (EVSYS_CTRL) Generic Clock Requests */
#define EVSYS_CTRL_GCLKREQ          (0x1ul << EVSYS_CTRL_GCLKREQ_Pos)
#define EVSYS_CTRL_MASK             0x11ul       /**< \brief (EVSYS_CTRL) MASK Register */

/* -------- EVSYS_CHANNEL : (EVSYS Offset: 0x04) (R/W 32) Channel -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CHANNEL:4;        /*!< bit:  0.. 3  Channel Selection                  */
    uint32_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint32_t SWEVT:1;          /*!< bit:      8  Software Event                     */
    uint32_t :7;               /*!< bit:  9..15  Reserved                           */
    uint32_t EVGEN:7;          /*!< bit: 16..22  Event Generator Selection          */
    uint32_t :1;               /*!< bit:     23  Reserved                           */
    uint32_t PATH:2;           /*!< bit: 24..25  Path Selection                     */
    uint32_t EDGSEL:2;         /*!< bit: 26..27  Edge Detection Selection           */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EVSYS_CHANNEL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_CHANNEL_OFFSET        0x04         /**< \brief (EVSYS_CHANNEL offset) Channel */
#define EVSYS_CHANNEL_RESETVALUE    0x00000000ul /**< \brief (EVSYS_CHANNEL reset_value) Channel */

#define EVSYS_CHANNEL_CHANNEL_Pos   0            /**< \brief (EVSYS_CHANNEL) Channel Selection */
#define EVSYS_CHANNEL_CHANNEL_Msk   (0xFul << EVSYS_CHANNEL_CHANNEL_Pos)
#define EVSYS_CHANNEL_CHANNEL(value) (EVSYS_CHANNEL_CHANNEL_Msk & ((value) << EVSYS_CHANNEL_CHANNEL_Pos))
#define EVSYS_CHANNEL_SWEVT_Pos     8            /**< \brief (EVSYS_CHANNEL) Software Event */
#define EVSYS_CHANNEL_SWEVT         (0x1ul << EVSYS_CHANNEL_SWEVT_Pos)
#define EVSYS_CHANNEL_EVGEN_Pos     16           /**< \brief (EVSYS_CHANNEL) Event Generator Selection */
#define EVSYS_CHANNEL_EVGEN_Msk     (0x7Ful << EVSYS_CHANNEL_EVGEN_Pos)
#define EVSYS_CHANNEL_EVGEN(value)  (EVSYS_CHANNEL_EVGEN_Msk & ((value) << EVSYS_CHANNEL_EVGEN_Pos))
#define EVSYS_CHANNEL_PATH_Pos      24           /**< \brief (EVSYS_CHANNEL) Path Selection */
#define EVSYS_CHANNEL_PATH_Msk      (0x3ul << EVSYS_CHANNEL_PATH_Pos)
#define EVSYS_CHANNEL_PATH(value)   (EVSYS_CHANNEL_PATH_Msk & ((value) << EVSYS_CHANNEL_PATH_Pos))
#define   EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val 0x0ul  /**< \brief (EVSYS_CHANNEL) Synchronous path */
#define   EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val 0x1ul  /**< \brief (EVSYS_CHANNEL) Resynchronized path */
#define   EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val 0x2ul  /**< \brief (EVSYS_CHANNEL) Asynchronous path */
#define EVSYS_CHANNEL_PATH_SYNCHRONOUS (EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val << EVSYS_CHANNEL_PATH_Pos)
#define EVSYS_CHANNEL_PATH_RESYNCHRONIZED (EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val << EVSYS_CHANNEL_PATH_Pos)
#define EVSYS_CHANNEL_PATH_ASYNCHRONOUS (EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val << EVSYS_CHANNEL_PATH_Pos)
#define EVSYS_CHANNEL_EDGSEL_Pos    26           /**< \brief (EVSYS_CHANNEL) Edge Detection Selection */
#define EVSYS_CHANNEL_EDGSEL_Msk    (0x3ul << EVSYS_CHANNEL_EDGSEL_Pos)
#define EVSYS_CHANNEL_EDGSEL(value) (EVSYS_CHANNEL_EDGSEL_Msk & ((value) << EVSYS_CHANNEL_EDGSEL_Pos))
#define   EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val 0x0ul  /**< \brief (EVSYS_CHANNEL) No event output when using the resynchronized or synchronous path */
#define   EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val 0x1ul  /**< \brief (EVSYS_CHANNEL) Event detection only on the rising edge of the signal from the event generator when using the resynchronized or synchronous path */
#define   EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val 0x2ul  /**< \brief (EVSYS_CHANNEL) Event detection only on the falling edge of the signal from the event generator when using the resynchronized or synchronous path */
#define   EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val 0x3ul  /**< \brief (EVSYS_CHANNEL) Event detection on rising and falling edges of the signal from the event generator when using the resynchronized or synchronous path */
#define EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT (EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val << EVSYS_CHANNEL_EDGSEL_Pos)
#define EVSYS_CHANNEL_EDGSEL_RISING_EDGE (EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val << EVSYS_CHANNEL_EDGSEL_Pos)
#define EVSYS_CHANNEL_EDGSEL_FALLING_EDGE (EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val << EVSYS_CHANNEL_EDGSEL_Pos)
#define EVSYS_CHANNEL_EDGSEL_BOTH_EDGES (EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val << EVSYS_CHANNEL_EDGSEL_Pos)
#define EVSYS_CHANNEL_MASK          0x0F7F010Ful /**< \brief (EVSYS_CHANNEL) MASK Register */

/* -------- EVSYS_USER : (EVSYS Offset: 0x08) (R/W 16) User Multiplexer -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t USER:5;           /*!< bit:  0.. 4  User Multiplexer Selection         */
    uint16_t :3;               /*!< bit:  5.. 7  Reserved                           */
    uint16_t CHANNEL:5;        /*!< bit:  8..12  Channel Event Selection            */
    uint16_t :3;               /*!< bit: 13..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} EVSYS_USER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_USER_OFFSET           0x08         /**< \brief (EVSYS_USER offset) User Multiplexer */
#define EVSYS_USER_RESETVALUE       0x0000ul     /**< \brief (EVSYS_USER reset_value) User Multiplexer */

#define EVSYS_USER_USER_Pos         0            /**< \brief (EVSYS_USER) User Multiplexer Selection */
#define EVSYS_USER_USER_Msk         (0x1Ful << EVSYS_USER_USER_Pos)
#define EVSYS_USER_USER(value)      (EVSYS_USER_USER_Msk & ((value) << EVSYS_USER_USER_Pos))
#define EVSYS_USER_CHANNEL_Pos      8            /**< \brief (EVSYS_USER) Channel Event Selection */
#define EVSYS_USER_CHANNEL_Msk      (0x1Ful << EVSYS_USER_CHANNEL_Pos)
#define EVSYS_USER_CHANNEL(value)   (EVSYS_USER_CHANNEL_Msk & ((value) << EVSYS_USER_CHANNEL_Pos))
#define   EVSYS_USER_CHANNEL_0_Val        0x0ul  /**< \brief (EVSYS_USER) No Channel Output Selected */
#define EVSYS_USER_CHANNEL_0        (EVSYS_USER_CHANNEL_0_Val      << EVSYS_USER_CHANNEL_Pos)
#define EVSYS_USER_MASK             0x1F1Ful     /**< \brief (EVSYS_USER) MASK Register */

/* -------- EVSYS_CHSTATUS : (EVSYS Offset: 0x0C) (R/  32) Channel Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t USRRDY0:1;        /*!< bit:      0  Channel 0 User Ready               */
    uint32_t USRRDY1:1;        /*!< bit:      1  Channel 1 User Ready               */
    uint32_t USRRDY2:1;        /*!< bit:      2  Channel 2 User Ready               */
    uint32_t USRRDY3:1;        /*!< bit:      3  Channel 3 User Ready               */
    uint32_t USRRDY4:1;        /*!< bit:      4  Channel 4 User Ready               */
    uint32_t USRRDY5:1;        /*!< bit:      5  Channel 5 User Ready               */
    uint32_t USRRDY6:1;        /*!< bit:      6  Channel 6 User Ready               */
    uint32_t USRRDY7:1;        /*!< bit:      7  Channel 7 User Ready               */
    uint32_t CHBUSY0:1;        /*!< bit:      8  Channel 0 Busy                     */
    uint32_t CHBUSY1:1;        /*!< bit:      9  Channel 1 Busy                     */
    uint32_t CHBUSY2:1;        /*!< bit:     10  Channel 2 Busy                     */
    uint32_t CHBUSY3:1;        /*!< bit:     11  Channel 3 Busy                     */
    uint32_t CHBUSY4:1;        /*!< bit:     12  Channel 4 Busy                     */
    uint32_t CHBUSY5:1;        /*!< bit:     13  Channel 5 Busy                     */
    uint32_t CHBUSY6:1;        /*!< bit:     14  Channel 6 Busy                     */
    uint32_t CHBUSY7:1;        /*!< bit:     15  Channel 7 Busy                     */
    uint32_t USRRDY8:1;        /*!< bit:     16  Channel 8 User Ready               */
    uint32_t USRRDY9:1;        /*!< bit:     17  Channel 9 User Ready               */
    uint32_t USRRDY10:1;       /*!< bit:     18  Channel 10 User Ready              */
    uint32_t USRRDY11:1;       /*!< bit:     19  Channel 11 User Ready              */
    uint32_t :4;               /*!< bit: 20..23  Reserved                           */
    uint32_t CHBUSY8:1;        /*!< bit:     24  Channel 8 Busy                     */
    uint32_t CHBUSY9:1;        /*!< bit:     25  Channel 9 Busy                     */
    uint32_t CHBUSY10:1;       /*!< bit:     26  Channel 10 Busy                    */
    uint32_t CHBUSY11:1;       /*!< bit:     27  Channel 11 Busy                    */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t USRRDY:8;         /*!< bit:  0.. 7  Channel x User Ready               */
    uint32_t CHBUSY:8;         /*!< bit:  8..15  Channel x Busy                     */
    uint32_t USRRDYp8:4;       /*!< bit: 16..19  Channel x+8 User Ready             */
    uint32_t :4;               /*!< bit: 20..23  Reserved                           */
    uint32_t CHBUSYp8:4;       /*!< bit: 24..27  Channel x+8 Busy                   */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EVSYS_CHSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_CHSTATUS_OFFSET       0x0C         /**< \brief (EVSYS_CHSTATUS offset) Channel Status */
#define EVSYS_CHSTATUS_RESETVALUE   0x000F00FFul /**< \brief (EVSYS_CHSTATUS reset_value) Channel Status */

#define EVSYS_CHSTATUS_USRRDY0_Pos  0            /**< \brief (EVSYS_CHSTATUS) Channel 0 User Ready */
#define EVSYS_CHSTATUS_USRRDY0      (1 << EVSYS_CHSTATUS_USRRDY0_Pos)
#define EVSYS_CHSTATUS_USRRDY1_Pos  1            /**< \brief (EVSYS_CHSTATUS) Channel 1 User Ready */
#define EVSYS_CHSTATUS_USRRDY1      (1 << EVSYS_CHSTATUS_USRRDY1_Pos)
#define EVSYS_CHSTATUS_USRRDY2_Pos  2            /**< \brief (EVSYS_CHSTATUS) Channel 2 User Ready */
#define EVSYS_CHSTATUS_USRRDY2      (1 << EVSYS_CHSTATUS_USRRDY2_Pos)
#define EVSYS_CHSTATUS_USRRDY3_Pos  3            /**< \brief (EVSYS_CHSTATUS) Channel 3 User Ready */
#define EVSYS_CHSTATUS_USRRDY3      (1 << EVSYS_CHSTATUS_USRRDY3_Pos)
#define EVSYS_CHSTATUS_USRRDY4_Pos  4            /**< \brief (EVSYS_CHSTATUS) Channel 4 User Ready */
#define EVSYS_CHSTATUS_USRRDY4      (1 << EVSYS_CHSTATUS_USRRDY4_Pos)
#define EVSYS_CHSTATUS_USRRDY5_Pos  5            /**< \brief (EVSYS_CHSTATUS) Channel 5 User Ready */
#define EVSYS_CHSTATUS_USRRDY5      (1 << EVSYS_CHSTATUS_USRRDY5_Pos)
#define EVSYS_CHSTATUS_USRRDY6_Pos  6            /**< \brief (EVSYS_CHSTATUS) Channel 6 User Ready */
#define EVSYS_CHSTATUS_USRRDY6      (1 << EVSYS_CHSTATUS_USRRDY6_Pos)
#define EVSYS_CHSTATUS_USRRDY7_Pos  7            /**< \brief (EVSYS_CHSTATUS) Channel 7 User Ready */
#define EVSYS_CHSTATUS_USRRDY7      (1 << EVSYS_CHSTATUS_USRRDY7_Pos)
#define EVSYS_CHSTATUS_USRRDY_Pos   0            /**< \brief (EVSYS_CHSTATUS) Channel x User Ready */
#define EVSYS_CHSTATUS_USRRDY_Msk   (0xFFul << EVSYS_CHSTATUS_USRRDY_Pos)
#define EVSYS_CHSTATUS_USRRDY(value) (EVSYS_CHSTATUS_USRRDY_Msk & ((value) << EVSYS_CHSTATUS_USRRDY_Pos))
#define EVSYS_CHSTATUS_CHBUSY0_Pos  8            /**< \brief (EVSYS_CHSTATUS) Channel 0 Busy */
#define EVSYS_CHSTATUS_CHBUSY0      (1 << EVSYS_CHSTATUS_CHBUSY0_Pos)
#define EVSYS_CHSTATUS_CHBUSY1_Pos  9            /**< \brief (EVSYS_CHSTATUS) Channel 1 Busy */
#define EVSYS_CHSTATUS_CHBUSY1      (1 << EVSYS_CHSTATUS_CHBUSY1_Pos)
#define EVSYS_CHSTATUS_CHBUSY2_Pos  10           /**< \brief (EVSYS_CHSTATUS) Channel 2 Busy */
#define EVSYS_CHSTATUS_CHBUSY2      (1 << EVSYS_CHSTATUS_CHBUSY2_Pos)
#define EVSYS_CHSTATUS_CHBUSY3_Pos  11           /**< \brief (EVSYS_CHSTATUS) Channel 3 Busy */
#define EVSYS_CHSTATUS_CHBUSY3      (1 << EVSYS_CHSTATUS_CHBUSY3_Pos)
#define EVSYS_CHSTATUS_CHBUSY4_Pos  12           /**< \brief (EVSYS_CHSTATUS) Channel 4 Busy */
#define EVSYS_CHSTATUS_CHBUSY4      (1 << EVSYS_CHSTATUS_CHBUSY4_Pos)
#define EVSYS_CHSTATUS_CHBUSY5_Pos  13           /**< \brief (EVSYS_CHSTATUS) Channel 5 Busy */
#define EVSYS_CHSTATUS_CHBUSY5      (1 << EVSYS_CHSTATUS_CHBUSY5_Pos)
#define EVSYS_CHSTATUS_CHBUSY6_Pos  14           /**< \brief (EVSYS_CHSTATUS) Channel 6 Busy */
#define EVSYS_CHSTATUS_CHBUSY6      (1 << EVSYS_CHSTATUS_CHBUSY6_Pos)
#define EVSYS_CHSTATUS_CHBUSY7_Pos  15           /**< \brief (EVSYS_CHSTATUS) Channel 7 Busy */
#define EVSYS_CHSTATUS_CHBUSY7      (1 << EVSYS_CHSTATUS_CHBUSY7_Pos)
#define EVSYS_CHSTATUS_CHBUSY_Pos   8            /**< \brief (EVSYS_CHSTATUS) Channel x Busy */
#define EVSYS_CHSTATUS_CHBUSY_Msk   (0xFFul << EVSYS_CHSTATUS_CHBUSY_Pos)
#define EVSYS_CHSTATUS_CHBUSY(value) (EVSYS_CHSTATUS_CHBUSY_Msk & ((value) << EVSYS_CHSTATUS_CHBUSY_Pos))
#define EVSYS_CHSTATUS_USRRDY8_Pos  16           /**< \brief (EVSYS_CHSTATUS) Channel 8 User Ready */
#define EVSYS_CHSTATUS_USRRDY8      (1 << EVSYS_CHSTATUS_USRRDY8_Pos)
#define EVSYS_CHSTATUS_USRRDY9_Pos  17           /**< \brief (EVSYS_CHSTATUS) Channel 9 User Ready */
#define EVSYS_CHSTATUS_USRRDY9      (1 << EVSYS_CHSTATUS_USRRDY9_Pos)
#define EVSYS_CHSTATUS_USRRDY10_Pos 18           /**< \brief (EVSYS_CHSTATUS) Channel 10 User Ready */
#define EVSYS_CHSTATUS_USRRDY10     (1 << EVSYS_CHSTATUS_USRRDY10_Pos)
#define EVSYS_CHSTATUS_USRRDY11_Pos 19           /**< \brief (EVSYS_CHSTATUS) Channel 11 User Ready */
#define EVSYS_CHSTATUS_USRRDY11     (1 << EVSYS_CHSTATUS_USRRDY11_Pos)
#define EVSYS_CHSTATUS_USRRDYp8_Pos 16           /**< \brief (EVSYS_CHSTATUS) Channel x+8 User Ready */
#define EVSYS_CHSTATUS_USRRDYp8_Msk (0xFul << EVSYS_CHSTATUS_USRRDYp8_Pos)
#define EVSYS_CHSTATUS_USRRDYp8(value) (EVSYS_CHSTATUS_USRRDYp8_Msk & ((value) << EVSYS_CHSTATUS_USRRDYp8_Pos))
#define EVSYS_CHSTATUS_CHBUSY8_Pos  24           /**< \brief (EVSYS_CHSTATUS) Channel 8 Busy */
#define EVSYS_CHSTATUS_CHBUSY8      (1 << EVSYS_CHSTATUS_CHBUSY8_Pos)
#define EVSYS_CHSTATUS_CHBUSY9_Pos  25           /**< \brief (EVSYS_CHSTATUS) Channel 9 Busy */
#define EVSYS_CHSTATUS_CHBUSY9      (1 << EVSYS_CHSTATUS_CHBUSY9_Pos)
#define EVSYS_CHSTATUS_CHBUSY10_Pos 26           /**< \brief (EVSYS_CHSTATUS) Channel 10 Busy */
#define EVSYS_CHSTATUS_CHBUSY10     (1 << EVSYS_CHSTATUS_CHBUSY10_Pos)
#define EVSYS_CHSTATUS_CHBUSY11_Pos 27           /**< \brief (EVSYS_CHSTATUS) Channel 11 Busy */
#define EVSYS_CHSTATUS_CHBUSY11     (1 << EVSYS_CHSTATUS_CHBUSY11_Pos)
#define EVSYS_CHSTATUS_CHBUSYp8_Pos 24           /**< \brief (EVSYS_CHSTATUS) Channel x+8 Busy */
#define EVSYS_CHSTATUS_CHBUSYp8_Msk (0xFul << EVSYS_CHSTATUS_CHBUSYp8_Pos)
#define EVSYS_CHSTATUS_CHBUSYp8(value) (EVSYS_CHSTATUS_CHBUSYp8_Msk & ((value) << EVSYS_CHSTATUS_CHBUSYp8_Pos))
#define EVSYS_CHSTATUS_MASK         0x0F0FFFFFul /**< \brief (EVSYS_CHSTATUS) MASK Register */

/* -------- EVSYS_INTENCLR : (EVSYS Offset: 0x10) (R/W 32) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t OVR0:1;           /*!< bit:      0  Channel 0 Overrun Interrupt Enable */
    uint32_t OVR1:1;           /*!< bit:      1  Channel 1 Overrun Interrupt Enable */
    uint32_t OVR2:1;           /*!< bit:      2  Channel 2 Overrun Interrupt Enable */
    uint32_t OVR3:1;           /*!< bit:      3  Channel 3 Overrun Interrupt Enable */
    uint32_t OVR4:1;           /*!< bit:      4  Channel 4 Overrun Interrupt Enable */
    uint32_t OVR5:1;           /*!< bit:      5  Channel 5 Overrun Interrupt Enable */
    uint32_t OVR6:1;           /*!< bit:      6  Channel 6 Overrun Interrupt Enable */
    uint32_t OVR7:1;           /*!< bit:      7  Channel 7 Overrun Interrupt Enable */
    uint32_t EVD0:1;           /*!< bit:      8  Channel 0 Event Detection Interrupt Enable */
    uint32_t EVD1:1;           /*!< bit:      9  Channel 1 Event Detection Interrupt Enable */
    uint32_t EVD2:1;           /*!< bit:     10  Channel 2 Event Detection Interrupt Enable */
    uint32_t EVD3:1;           /*!< bit:     11  Channel 3 Event Detection Interrupt Enable */
    uint32_t EVD4:1;           /*!< bit:     12  Channel 4 Event Detection Interrupt Enable */
    uint32_t EVD5:1;           /*!< bit:     13  Channel 5 Event Detection Interrupt Enable */
    uint32_t EVD6:1;           /*!< bit:     14  Channel 6 Event Detection Interrupt Enable */
    uint32_t EVD7:1;           /*!< bit:     15  Channel 7 Event Detection Interrupt Enable */
    uint32_t OVR8:1;           /*!< bit:     16  Channel 8 Overrun Interrupt Enable */
    uint32_t OVR9:1;           /*!< bit:     17  Channel 9 Overrun Interrupt Enable */
    uint32_t OVR10:1;          /*!< bit:     18  Channel 10 Overrun Interrupt Enable */
    uint32_t OVR11:1;          /*!< bit:     19  Channel 11 Overrun Interrupt Enable */
    uint32_t :4;               /*!< bit: 20..23  Reserved                           */
    uint32_t EVD8:1;           /*!< bit:     24  Channel 8 Event Detection Interrupt Enable */
    uint32_t EVD9:1;           /*!< bit:     25  Channel 9 Event Detection Interrupt Enable */
    uint32_t EVD10:1;          /*!< bit:     26  Channel 10 Event Detection Interrupt Enable */
    uint32_t EVD11:1;          /*!< bit:     27  Channel 11 Event Detection Interrupt Enable */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t OVR:8;            /*!< bit:  0.. 7  Channel x Overrun Interrupt Enable */
    uint32_t EVD:8;            /*!< bit:  8..15  Channel x Event Detection Interrupt Enable */
    uint32_t OVRp8:4;          /*!< bit: 16..19  Channel x+8 Overrun Interrupt Enable */
    uint32_t :4;               /*!< bit: 20..23  Reserved                           */
    uint32_t EVDp8:4;          /*!< bit: 24..27  Channel x+8 Event Detection Interrupt Enable */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EVSYS_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_INTENCLR_OFFSET       0x10         /**< \brief (EVSYS_INTENCLR offset) Interrupt Enable Clear */
#define EVSYS_INTENCLR_RESETVALUE   0x00000000ul /**< \brief (EVSYS_INTENCLR reset_value) Interrupt Enable Clear */

#define EVSYS_INTENCLR_OVR0_Pos     0            /**< \brief (EVSYS_INTENCLR) Channel 0 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR0         (1 << EVSYS_INTENCLR_OVR0_Pos)
#define EVSYS_INTENCLR_OVR1_Pos     1            /**< \brief (EVSYS_INTENCLR) Channel 1 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR1         (1 << EVSYS_INTENCLR_OVR1_Pos)
#define EVSYS_INTENCLR_OVR2_Pos     2            /**< \brief (EVSYS_INTENCLR) Channel 2 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR2         (1 << EVSYS_INTENCLR_OVR2_Pos)
#define EVSYS_INTENCLR_OVR3_Pos     3            /**< \brief (EVSYS_INTENCLR) Channel 3 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR3         (1 << EVSYS_INTENCLR_OVR3_Pos)
#define EVSYS_INTENCLR_OVR4_Pos     4            /**< \brief (EVSYS_INTENCLR) Channel 4 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR4         (1 << EVSYS_INTENCLR_OVR4_Pos)
#define EVSYS_INTENCLR_OVR5_Pos     5            /**< \brief (EVSYS_INTENCLR) Channel 5 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR5         (1 << EVSYS_INTENCLR_OVR5_Pos)
#define EVSYS_INTENCLR_OVR6_Pos     6            /**< \brief (EVSYS_INTENCLR) Channel 6 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR6         (1 << EVSYS_INTENCLR_OVR6_Pos)
#define EVSYS_INTENCLR_OVR7_Pos     7            /**< \brief (EVSYS_INTENCLR) Channel 7 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR7         (1 << EVSYS_INTENCLR_OVR7_Pos)
#define EVSYS_INTENCLR_OVR_Pos      0            /**< \brief (EVSYS_INTENCLR) Channel x Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR_Msk      (0xFFul << EVSYS_INTENCLR_OVR_Pos)
#define EVSYS_INTENCLR_OVR(value)   (EVSYS_INTENCLR_OVR_Msk & ((value) << EVSYS_INTENCLR_OVR_Pos))
#define EVSYS_INTENCLR_EVD0_Pos     8            /**< \brief (EVSYS_INTENCLR) Channel 0 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD0         (1 << EVSYS_INTENCLR_EVD0_Pos)
#define EVSYS_INTENCLR_EVD1_Pos     9            /**< \brief (EVSYS_INTENCLR) Channel 1 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD1         (1 << EVSYS_INTENCLR_EVD1_Pos)
#define EVSYS_INTENCLR_EVD2_Pos     10           /**< \brief (EVSYS_INTENCLR) Channel 2 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD2         (1 << EVSYS_INTENCLR_EVD2_Pos)
#define EVSYS_INTENCLR_EVD3_Pos     11           /**< \brief (EVSYS_INTENCLR) Channel 3 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD3         (1 << EVSYS_INTENCLR_EVD3_Pos)
#define EVSYS_INTENCLR_EVD4_Pos     12           /**< \brief (EVSYS_INTENCLR) Channel 4 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD4         (1 << EVSYS_INTENCLR_EVD4_Pos)
#define EVSYS_INTENCLR_EVD5_Pos     13           /**< \brief (EVSYS_INTENCLR) Channel 5 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD5         (1 << EVSYS_INTENCLR_EVD5_Pos)
#define EVSYS_INTENCLR_EVD6_Pos     14           /**< \brief (EVSYS_INTENCLR) Channel 6 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD6         (1 << EVSYS_INTENCLR_EVD6_Pos)
#define EVSYS_INTENCLR_EVD7_Pos     15           /**< \brief (EVSYS_INTENCLR) Channel 7 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD7         (1 << EVSYS_INTENCLR_EVD7_Pos)
#define EVSYS_INTENCLR_EVD_Pos      8            /**< \brief (EVSYS_INTENCLR) Channel x Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD_Msk      (0xFFul << EVSYS_INTENCLR_EVD_Pos)
#define EVSYS_INTENCLR_EVD(value)   (EVSYS_INTENCLR_EVD_Msk & ((value) << EVSYS_INTENCLR_EVD_Pos))
#define EVSYS_INTENCLR_OVR8_Pos     16           /**< \brief (EVSYS_INTENCLR) Channel 8 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR8         (1 << EVSYS_INTENCLR_OVR8_Pos)
#define EVSYS_INTENCLR_OVR9_Pos     17           /**< \brief (EVSYS_INTENCLR) Channel 9 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR9         (1 << EVSYS_INTENCLR_OVR9_Pos)
#define EVSYS_INTENCLR_OVR10_Pos    18           /**< \brief (EVSYS_INTENCLR) Channel 10 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR10        (1 << EVSYS_INTENCLR_OVR10_Pos)
#define EVSYS_INTENCLR_OVR11_Pos    19           /**< \brief (EVSYS_INTENCLR) Channel 11 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR11        (1 << EVSYS_INTENCLR_OVR11_Pos)
#define EVSYS_INTENCLR_OVRp8_Pos    16           /**< \brief (EVSYS_INTENCLR) Channel x+8 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVRp8_Msk    (0xFul << EVSYS_INTENCLR_OVRp8_Pos)
#define EVSYS_INTENCLR_OVRp8(value) (EVSYS_INTENCLR_OVRp8_Msk & ((value) << EVSYS_INTENCLR_OVRp8_Pos))
#define EVSYS_INTENCLR_EVD8_Pos     24           /**< \brief (EVSYS_INTENCLR) Channel 8 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD8         (1 << EVSYS_INTENCLR_EVD8_Pos)
#define EVSYS_INTENCLR_EVD9_Pos     25           /**< \brief (EVSYS_INTENCLR) Channel 9 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD9         (1 << EVSYS_INTENCLR_EVD9_Pos)
#define EVSYS_INTENCLR_EVD10_Pos    26           /**< \brief (EVSYS_INTENCLR) Channel 10 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD10        (1 << EVSYS_INTENCLR_EVD10_Pos)
#define EVSYS_INTENCLR_EVD11_Pos    27           /**< \brief (EVSYS_INTENCLR) Channel 11 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD11        (1 << EVSYS_INTENCLR_EVD11_Pos)
#define EVSYS_INTENCLR_EVDp8_Pos    24           /**< \brief (EVSYS_INTENCLR) Channel x+8 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVDp8_Msk    (0xFul << EVSYS_INTENCLR_EVDp8_Pos)
#define EVSYS_INTENCLR_EVDp8(value) (EVSYS_INTENCLR_EVDp8_Msk & ((value) << EVSYS_INTENCLR_EVDp8_Pos))
#define EVSYS_INTENCLR_MASK         0x0F0FFFFFul /**< \brief (EVSYS_INTENCLR) MASK Register */

/* -------- EVSYS_INTENSET : (EVSYS Offset: 0x14) (R/W 32) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t OVR0:1;           /*!< bit:      0  Channel 0 Overrun Interrupt Enable */
    uint32_t OVR1:1;           /*!< bit:      1  Channel 1 Overrun Interrupt Enable */
    uint32_t OVR2:1;           /*!< bit:      2  Channel 2 Overrun Interrupt Enable */
    uint32_t OVR3:1;           /*!< bit:      3  Channel 3 Overrun Interrupt Enable */
    uint32_t OVR4:1;           /*!< bit:      4  Channel 4 Overrun Interrupt Enable */
    uint32_t OVR5:1;           /*!< bit:      5  Channel 5 Overrun Interrupt Enable */
    uint32_t OVR6:1;           /*!< bit:      6  Channel 6 Overrun Interrupt Enable */
    uint32_t OVR7:1;           /*!< bit:      7  Channel 7 Overrun Interrupt Enable */
    uint32_t EVD0:1;           /*!< bit:      8  Channel 0 Event Detection Interrupt Enable */
    uint32_t EVD1:1;           /*!< bit:      9  Channel 1 Event Detection Interrupt Enable */
    uint32_t EVD2:1;           /*!< bit:     10  Channel 2 Event Detection Interrupt Enable */
    uint32_t EVD3:1;           /*!< bit:     11  Channel 3 Event Detection Interrupt Enable */
    uint32_t EVD4:1;           /*!< bit:     12  Channel 4 Event Detection Interrupt Enable */
    uint32_t EVD5:1;           /*!< bit:     13  Channel 5 Event Detection Interrupt Enable */
    uint32_t EVD6:1;           /*!< bit:     14  Channel 6 Event Detection Interrupt Enable */
    uint32_t EVD7:1;           /*!< bit:     15  Channel 7 Event Detection Interrupt Enable */
    uint32_t OVR8:1;           /*!< bit:     16  Channel 8 Overrun Interrupt Enable */
    uint32_t OVR9:1;           /*!< bit:     17  Channel 9 Overrun Interrupt Enable */
    uint32_t OVR10:1;          /*!< bit:     18  Channel 10 Overrun Interrupt Enable */
    uint32_t OVR11:1;          /*!< bit:     19  Channel 11 Overrun Interrupt Enable */
    uint32_t :4;               /*!< bit: 20..23  Reserved                           */
    uint32_t EVD8:1;           /*!< bit:     24  Channel 8 Event Detection Interrupt Enable */
    uint32_t EVD9:1;           /*!< bit:     25  Channel 9 Event Detection Interrupt Enable */
    uint32_t EVD10:1;          /*!< bit:     26  Channel 10 Event Detection Interrupt Enable */
    uint32_t EVD11:1;          /*!< bit:     27  Channel 11 Event Detection Interrupt Enable */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t OVR:8;            /*!< bit:  0.. 7  Channel x Overrun Interrupt Enable */
    uint32_t EVD:8;            /*!< bit:  8..15  Channel x Event Detection Interrupt Enable */
    uint32_t OVRp8:4;          /*!< bit: 16..19  Channel x+8 Overrun Interrupt Enable */
    uint32_t :4;               /*!< bit: 20..23  Reserved                           */
    uint32_t EVDp8:4;          /*!< bit: 24..27  Channel x+8 Event Detection Interrupt Enable */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EVSYS_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_INTENSET_OFFSET       0x14         /**< \brief (EVSYS_INTENSET offset) Interrupt Enable Set */
#define EVSYS_INTENSET_RESETVALUE   0x00000000ul /**< \brief (EVSYS_INTENSET reset_value) Interrupt Enable Set */

#define EVSYS_INTENSET_OVR0_Pos     0            /**< \brief (EVSYS_INTENSET) Channel 0 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR0         (1 << EVSYS_INTENSET_OVR0_Pos)
#define EVSYS_INTENSET_OVR1_Pos     1            /**< \brief (EVSYS_INTENSET) Channel 1 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR1         (1 << EVSYS_INTENSET_OVR1_Pos)
#define EVSYS_INTENSET_OVR2_Pos     2            /**< \brief (EVSYS_INTENSET) Channel 2 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR2         (1 << EVSYS_INTENSET_OVR2_Pos)
#define EVSYS_INTENSET_OVR3_Pos     3            /**< \brief (EVSYS_INTENSET) Channel 3 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR3         (1 << EVSYS_INTENSET_OVR3_Pos)
#define EVSYS_INTENSET_OVR4_Pos     4            /**< \brief (EVSYS_INTENSET) Channel 4 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR4         (1 << EVSYS_INTENSET_OVR4_Pos)
#define EVSYS_INTENSET_OVR5_Pos     5            /**< \brief (EVSYS_INTENSET) Channel 5 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR5         (1 << EVSYS_INTENSET_OVR5_Pos)
#define EVSYS_INTENSET_OVR6_Pos     6            /**< \brief (EVSYS_INTENSET) Channel 6 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR6         (1 << EVSYS_INTENSET_OVR6_Pos)
#define EVSYS_INTENSET_OVR7_Pos     7            /**< \brief (EVSYS_INTENSET) Channel 7 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR7         (1 << EVSYS_INTENSET_OVR7_Pos)
#define EVSYS_INTENSET_OVR_Pos      0            /**< \brief (EVSYS_INTENSET) Channel x Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR_Msk      (0xFFul << EVSYS_INTENSET_OVR_Pos)
#define EVSYS_INTENSET_OVR(value)   (EVSYS_INTENSET_OVR_Msk & ((value) << EVSYS_INTENSET_OVR_Pos))
#define EVSYS_INTENSET_EVD0_Pos     8            /**< \brief (EVSYS_INTENSET) Channel 0 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD0         (1 << EVSYS_INTENSET_EVD0_Pos)
#define EVSYS_INTENSET_EVD1_Pos     9            /**< \brief (EVSYS_INTENSET) Channel 1 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD1         (1 << EVSYS_INTENSET_EVD1_Pos)
#define EVSYS_INTENSET_EVD2_Pos     10           /**< \brief (EVSYS_INTENSET) Channel 2 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD2         (1 << EVSYS_INTENSET_EVD2_Pos)
#define EVSYS_INTENSET_EVD3_Pos     11           /**< \brief (EVSYS_INTENSET) Channel 3 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD3         (1 << EVSYS_INTENSET_EVD3_Pos)
#define EVSYS_INTENSET_EVD4_Pos     12           /**< \brief (EVSYS_INTENSET) Channel 4 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD4         (1 << EVSYS_INTENSET_EVD4_Pos)
#define EVSYS_INTENSET_EVD5_Pos     13           /**< \brief (EVSYS_INTENSET) Channel 5 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD5         (1 << EVSYS_INTENSET_EVD5_Pos)
#define EVSYS_INTENSET_EVD6_Pos     14           /**< \brief (EVSYS_INTENSET) Channel 6 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD6         (1 << EVSYS_INTENSET_EVD6_Pos)
#define EVSYS_INTENSET_EVD7_Pos     15           /**< \brief (EVSYS_INTENSET) Channel 7 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD7         (1 << EVSYS_INTENSET_EVD7_Pos)
#define EVSYS_INTENSET_EVD_Pos      8            /**< \brief (EVSYS_INTENSET) Channel x Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD_Msk      (0xFFul << EVSYS_INTENSET_EVD_Pos)
#define EVSYS_INTENSET_EVD(value)   (EVSYS_INTENSET_EVD_Msk & ((value) << EVSYS_INTENSET_EVD_Pos))
#define EVSYS_INTENSET_OVR8_Pos     16           /**< \brief (EVSYS_INTENSET) Channel 8 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR8         (1 << EVSYS_INTENSET_OVR8_Pos)
#define EVSYS_INTENSET_OVR9_Pos     17           /**< \brief (EVSYS_INTENSET) Channel 9 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR9         (1 << EVSYS_INTENSET_OVR9_Pos)
#define EVSYS_INTENSET_OVR10_Pos    18           /**< \brief (EVSYS_INTENSET) Channel 10 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR10        (1 << EVSYS_INTENSET_OVR10_Pos)
#define EVSYS_INTENSET_OVR11_Pos    19           /**< \brief (EVSYS_INTENSET) Channel 11 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR11        (1 << EVSYS_INTENSET_OVR11_Pos)
#define EVSYS_INTENSET_OVRp8_Pos    16           /**< \brief (EVSYS_INTENSET) Channel x+8 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVRp8_Msk    (0xFul << EVSYS_INTENSET_OVRp8_Pos)
#define EVSYS_INTENSET_OVRp8(value) (EVSYS_INTENSET_OVRp8_Msk & ((value) << EVSYS_INTENSET_OVRp8_Pos))
#define EVSYS_INTENSET_EVD8_Pos     24           /**< \brief (EVSYS_INTENSET) Channel 8 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD8         (1 << EVSYS_INTENSET_EVD8_Pos)
#define EVSYS_INTENSET_EVD9_Pos     25           /**< \brief (EVSYS_INTENSET) Channel 9 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD9         (1 << EVSYS_INTENSET_EVD9_Pos)
#define EVSYS_INTENSET_EVD10_Pos    26           /**< \brief (EVSYS_INTENSET) Channel 10 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD10        (1 << EVSYS_INTENSET_EVD10_Pos)
#define EVSYS_INTENSET_EVD11_Pos    27           /**< \brief (EVSYS_INTENSET) Channel 11 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD11        (1 << EVSYS_INTENSET_EVD11_Pos)
#define EVSYS_INTENSET_EVDp8_Pos    24           /**< \brief (EVSYS_INTENSET) Channel x+8 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVDp8_Msk    (0xFul << EVSYS_INTENSET_EVDp8_Pos)
#define EVSYS_INTENSET_EVDp8(value) (EVSYS_INTENSET_EVDp8_Msk & ((value) << EVSYS_INTENSET_EVDp8_Pos))
#define EVSYS_INTENSET_MASK         0x0F0FFFFFul /**< \brief (EVSYS_INTENSET) MASK Register */

/* -------- EVSYS_INTFLAG : (EVSYS Offset: 0x18) (R/W 32) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t OVR0:1;           /*!< bit:      0  Channel 0 Overrun                  */
    __I uint32_t OVR1:1;           /*!< bit:      1  Channel 1 Overrun                  */
    __I uint32_t OVR2:1;           /*!< bit:      2  Channel 2 Overrun                  */
    __I uint32_t OVR3:1;           /*!< bit:      3  Channel 3 Overrun                  */
    __I uint32_t OVR4:1;           /*!< bit:      4  Channel 4 Overrun                  */
    __I uint32_t OVR5:1;           /*!< bit:      5  Channel 5 Overrun                  */
    __I uint32_t OVR6:1;           /*!< bit:      6  Channel 6 Overrun                  */
    __I uint32_t OVR7:1;           /*!< bit:      7  Channel 7 Overrun                  */
    __I uint32_t EVD0:1;           /*!< bit:      8  Channel 0 Event Detection          */
    __I uint32_t EVD1:1;           /*!< bit:      9  Channel 1 Event Detection          */
    __I uint32_t EVD2:1;           /*!< bit:     10  Channel 2 Event Detection          */
    __I uint32_t EVD3:1;           /*!< bit:     11  Channel 3 Event Detection          */
    __I uint32_t EVD4:1;           /*!< bit:     12  Channel 4 Event Detection          */
    __I uint32_t EVD5:1;           /*!< bit:     13  Channel 5 Event Detection          */
    __I uint32_t EVD6:1;           /*!< bit:     14  Channel 6 Event Detection          */
    __I uint32_t EVD7:1;           /*!< bit:     15  Channel 7 Event Detection          */
    __I uint32_t OVR8:1;           /*!< bit:     16  Channel 8 Overrun                  */
    __I uint32_t OVR9:1;           /*!< bit:     17  Channel 9 Overrun                  */
    __I uint32_t OVR10:1;          /*!< bit:     18  Channel 10 Overrun                 */
    __I uint32_t OVR11:1;          /*!< bit:     19  Channel 11 Overrun                 */
    __I uint32_t :4;               /*!< bit: 20..23  Reserved                           */
    __I uint32_t EVD8:1;           /*!< bit:     24  Channel 8 Event Detection          */
    __I uint32_t EVD9:1;           /*!< bit:     25  Channel 9 Event Detection          */
    __I uint32_t EVD10:1;          /*!< bit:     26  Channel 10 Event Detection         */
    __I uint32_t EVD11:1;          /*!< bit:     27  Channel 11 Event Detection         */
    __I uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint32_t OVR:8;            /*!< bit:  0.. 7  Channel x Overrun                  */
    __I uint32_t EVD:8;            /*!< bit:  8..15  Channel x Event Detection          */
    __I uint32_t OVRp8:4;          /*!< bit: 16..19  Channel x+8 Overrun                */
    __I uint32_t :4;               /*!< bit: 20..23  Reserved                           */
    __I uint32_t EVDp8:4;          /*!< bit: 24..27  Channel x+8 Event Detection        */
    __I uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EVSYS_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_INTFLAG_OFFSET        0x18         /**< \brief (EVSYS_INTFLAG offset) Interrupt Flag Status and Clear */
#define EVSYS_INTFLAG_RESETVALUE    0x00000000ul /**< \brief (EVSYS_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define EVSYS_INTFLAG_OVR0_Pos      0            /**< \brief (EVSYS_INTFLAG) Channel 0 Overrun */
#define EVSYS_INTFLAG_OVR0          (1 << EVSYS_INTFLAG_OVR0_Pos)
#define EVSYS_INTFLAG_OVR1_Pos      1            /**< \brief (EVSYS_INTFLAG) Channel 1 Overrun */
#define EVSYS_INTFLAG_OVR1          (1 << EVSYS_INTFLAG_OVR1_Pos)
#define EVSYS_INTFLAG_OVR2_Pos      2            /**< \brief (EVSYS_INTFLAG) Channel 2 Overrun */
#define EVSYS_INTFLAG_OVR2          (1 << EVSYS_INTFLAG_OVR2_Pos)
#define EVSYS_INTFLAG_OVR3_Pos      3            /**< \brief (EVSYS_INTFLAG) Channel 3 Overrun */
#define EVSYS_INTFLAG_OVR3          (1 << EVSYS_INTFLAG_OVR3_Pos)
#define EVSYS_INTFLAG_OVR4_Pos      4            /**< \brief (EVSYS_INTFLAG) Channel 4 Overrun */
#define EVSYS_INTFLAG_OVR4          (1 << EVSYS_INTFLAG_OVR4_Pos)
#define EVSYS_INTFLAG_OVR5_Pos      5            /**< \brief (EVSYS_INTFLAG) Channel 5 Overrun */
#define EVSYS_INTFLAG_OVR5          (1 << EVSYS_INTFLAG_OVR5_Pos)
#define EVSYS_INTFLAG_OVR6_Pos      6            /**< \brief (EVSYS_INTFLAG) Channel 6 Overrun */
#define EVSYS_INTFLAG_OVR6          (1 << EVSYS_INTFLAG_OVR6_Pos)
#define EVSYS_INTFLAG_OVR7_Pos      7            /**< \brief (EVSYS_INTFLAG) Channel 7 Overrun */
#define EVSYS_INTFLAG_OVR7          (1 << EVSYS_INTFLAG_OVR7_Pos)
#define EVSYS_INTFLAG_OVR_Pos       0            /**< \brief (EVSYS_INTFLAG) Channel x Overrun */
#define EVSYS_INTFLAG_OVR_Msk       (0xFFul << EVSYS_INTFLAG_OVR_Pos)
#define EVSYS_INTFLAG_OVR(value)    (EVSYS_INTFLAG_OVR_Msk & ((value) << EVSYS_INTFLAG_OVR_Pos))
#define EVSYS_INTFLAG_EVD0_Pos      8            /**< \brief (EVSYS_INTFLAG) Channel 0 Event Detection */
#define EVSYS_INTFLAG_EVD0          (1 << EVSYS_INTFLAG_EVD0_Pos)
#define EVSYS_INTFLAG_EVD1_Pos      9            /**< \brief (EVSYS_INTFLAG) Channel 1 Event Detection */
#define EVSYS_INTFLAG_EVD1          (1 << EVSYS_INTFLAG_EVD1_Pos)
#define EVSYS_INTFLAG_EVD2_Pos      10           /**< \brief (EVSYS_INTFLAG) Channel 2 Event Detection */
#define EVSYS_INTFLAG_EVD2          (1 << EVSYS_INTFLAG_EVD2_Pos)
#define EVSYS_INTFLAG_EVD3_Pos      11           /**< \brief (EVSYS_INTFLAG) Channel 3 Event Detection */
#define EVSYS_INTFLAG_EVD3          (1 << EVSYS_INTFLAG_EVD3_Pos)
#define EVSYS_INTFLAG_EVD4_Pos      12           /**< \brief (EVSYS_INTFLAG) Channel 4 Event Detection */
#define EVSYS_INTFLAG_EVD4          (1 << EVSYS_INTFLAG_EVD4_Pos)
#define EVSYS_INTFLAG_EVD5_Pos      13           /**< \brief (EVSYS_INTFLAG) Channel 5 Event Detection */
#define EVSYS_INTFLAG_EVD5          (1 << EVSYS_INTFLAG_EVD5_Pos)
#define EVSYS_INTFLAG_EVD6_Pos      14           /**< \brief (EVSYS_INTFLAG) Channel 6 Event Detection */
#define EVSYS_INTFLAG_EVD6          (1 << EVSYS_INTFLAG_EVD6_Pos)
#define EVSYS_INTFLAG_EVD7_Pos      15           /**< \brief (EVSYS_INTFLAG) Channel 7 Event Detection */
#define EVSYS_INTFLAG_EVD7          (1 << EVSYS_INTFLAG_EVD7_Pos)
#define EVSYS_INTFLAG_EVD_Pos       8            /**< \brief (EVSYS_INTFLAG) Channel x Event Detection */
#define EVSYS_INTFLAG_EVD_Msk       (0xFFul << EVSYS_INTFLAG_EVD_Pos)
#define EVSYS_INTFLAG_EVD(value)    (EVSYS_INTFLAG_EVD_Msk & ((value) << EVSYS_INTFLAG_EVD_Pos))
#define EVSYS_INTFLAG_OVR8_Pos      16           /**< \brief (EVSYS_INTFLAG) Channel 8 Overrun */
#define EVSYS_INTFLAG_OVR8          (1 << EVSYS_INTFLAG_OVR8_Pos)
#define EVSYS_INTFLAG_OVR9_Pos      17           /**< \brief (EVSYS_INTFLAG) Channel 9 Overrun */
#define EVSYS_INTFLAG_OVR9          (1 << EVSYS_INTFLAG_OVR9_Pos)
#define EVSYS_INTFLAG_OVR10_Pos     18           /**< \brief (EVSYS_INTFLAG) Channel 10 Overrun */
#define EVSYS_INTFLAG_OVR10         (1 << EVSYS_INTFLAG_OVR10_Pos)
#define EVSYS_INTFLAG_OVR11_Pos     19           /**< \brief (EVSYS_INTFLAG) Channel 11 Overrun */
#define EVSYS_INTFLAG_OVR11         (1 << EVSYS_INTFLAG_OVR11_Pos)
#define EVSYS_INTFLAG_OVRp8_Pos     16           /**< \brief (EVSYS_INTFLAG) Channel x+8 Overrun */
#define EVSYS_INTFLAG_OVRp8_Msk     (0xFul << EVSYS_INTFLAG_OVRp8_Pos)
#define EVSYS_INTFLAG_OVRp8(value)  (EVSYS_INTFLAG_OVRp8_Msk & ((value) << EVSYS_INTFLAG_OVRp8_Pos))
#define EVSYS_INTFLAG_EVD8_Pos      24           /**< \brief (EVSYS_INTFLAG) Channel 8 Event Detection */
#define EVSYS_INTFLAG_EVD8          (1 << EVSYS_INTFLAG_EVD8_Pos)
#define EVSYS_INTFLAG_EVD9_Pos      25           /**< \brief (EVSYS_INTFLAG) Channel 9 Event Detection */
#define EVSYS_INTFLAG_EVD9          (1 << EVSYS_INTFLAG_EVD9_Pos)
#define EVSYS_INTFLAG_EVD10_Pos     26           /**< \brief (EVSYS_INTFLAG) Channel 10 Event Detection */
#define EVSYS_INTFLAG_EVD10         (1 << EVSYS_INTFLAG_EVD10_Pos)
#define EVSYS_INTFLAG_EVD11_Pos     27           /**< \brief (EVSYS_INTFLAG) Channel 11 Event Detection */
#define EVSYS_INTFLAG_EVD11         (1 << EVSYS_INTFLAG_EVD11_Pos)
#define EVSYS_INTFLAG_EVDp8_Pos     24           /**< \brief (EVSYS_INTFLAG) Channel x+8 Event Detection */
#define EVSYS_INTFLAG_EVDp8_Msk     (0xFul << EVSYS_INTFLAG_EVDp8_Pos)
#define EVSYS_INTFLAG_EVDp8(value)  (EVSYS_INTFLAG_EVDp8_Msk & ((value) << EVSYS_INTFLAG_EVDp8_Pos))
#define EVSYS_INTFLAG_MASK          0x0F0FFFFFul /**< \brief (EVSYS_INTFLAG) MASK Register */

/** \brief EVSYS hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __O  EVSYS_CTRL_Type           CTRL;        /**< \brief Offset: 0x00 ( /W  8) Control */
       RoReg8                    Reserved1[0x3];
  __IO EVSYS_CHANNEL_Type        CHANNEL;     /**< \brief Offset: 0x04 (R/W 32) Channel */
  __IO EVSYS_USER_Type           USER;        /**< \brief Offset: 0x08 (R/W 16) User Multiplexer */
       RoReg8                    Reserved2[0x2];
  __I  EVSYS_CHSTATUS_Type       CHSTATUS;    /**< \brief Offset: 0x0C (R/  32) Channel Status */
  __IO EVSYS_INTENCLR_Type       INTENCLR;    /**< \brief Offset: 0x10 (R/W 32) Interrupt Enable Clear */
  __IO EVSYS_INTENSET_Type       INTENSET;    /**< \brief Offset: 0x14 (R/W 32) Interrupt Enable Set */
  __IO EVSYS_INTFLAG_Type        INTFLAG;     /**< \brief Offset: 0x18 (R/W 32) Interrupt Flag Status and Clear */
} Evsys;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD21_EVSYS_COMPONENT_ */
