/**
 * \file
 *
 * \brief Component description for SYSCTRL
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

#ifndef _SAMD21_SYSCTRL_COMPONENT_
#define _SAMD21_SYSCTRL_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SYSCTRL */
/* ========================================================================== */
/** \addtogroup SAMD21_SYSCTRL System Control */
/*@{*/

#define SYSCTRL_U2100
#define REV_SYSCTRL                 0x201

/* -------- SYSCTRL_INTENCLR : (SYSCTRL Offset: 0x00) (R/W 32) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t XOSCRDY:1;        /*!< bit:      0  XOSC Ready Interrupt Enable        */
    uint32_t XOSC32KRDY:1;     /*!< bit:      1  XOSC32K Ready Interrupt Enable     */
    uint32_t OSC32KRDY:1;      /*!< bit:      2  OSC32K Ready Interrupt Enable      */
    uint32_t OSC8MRDY:1;       /*!< bit:      3  OSC8M Ready Interrupt Enable       */
    uint32_t DFLLRDY:1;        /*!< bit:      4  DFLL Ready Interrupt Enable        */
    uint32_t DFLLOOB:1;        /*!< bit:      5  DFLL Out Of Bounds Interrupt Enable */
    uint32_t DFLLLCKF:1;       /*!< bit:      6  DFLL Lock Fine Interrupt Enable    */
    uint32_t DFLLLCKC:1;       /*!< bit:      7  DFLL Lock Coarse Interrupt Enable  */
    uint32_t DFLLRCS:1;        /*!< bit:      8  DFLL Reference Clock Stopped Interrupt Enable */
    uint32_t BOD33RDY:1;       /*!< bit:      9  BOD33 Ready Interrupt Enable       */
    uint32_t BOD33DET:1;       /*!< bit:     10  BOD33 Detection Interrupt Enable   */
    uint32_t B33SRDY:1;        /*!< bit:     11  BOD33 Synchronization Ready Interrupt Enable */
    uint32_t :3;               /*!< bit: 12..14  Reserved                           */
    uint32_t DPLLLCKR:1;       /*!< bit:     15  DPLL Lock Rise Interrupt Enable    */
    uint32_t DPLLLCKF:1;       /*!< bit:     16  DPLL Lock Fall Interrupt Enable    */
    uint32_t DPLLLTO:1;        /*!< bit:     17  DPLL Lock Timeout Interrupt Enable */
    uint32_t :14;              /*!< bit: 18..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SYSCTRL_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_INTENCLR_OFFSET     0x00         /**< \brief (SYSCTRL_INTENCLR offset) Interrupt Enable Clear */
#define SYSCTRL_INTENCLR_RESETVALUE 0x00000000ul /**< \brief (SYSCTRL_INTENCLR reset_value) Interrupt Enable Clear */

#define SYSCTRL_INTENCLR_XOSCRDY_Pos 0            /**< \brief (SYSCTRL_INTENCLR) XOSC Ready Interrupt Enable */
#define SYSCTRL_INTENCLR_XOSCRDY    (0x1ul << SYSCTRL_INTENCLR_XOSCRDY_Pos)
#define SYSCTRL_INTENCLR_XOSC32KRDY_Pos 1            /**< \brief (SYSCTRL_INTENCLR) XOSC32K Ready Interrupt Enable */
#define SYSCTRL_INTENCLR_XOSC32KRDY (0x1ul << SYSCTRL_INTENCLR_XOSC32KRDY_Pos)
#define SYSCTRL_INTENCLR_OSC32KRDY_Pos 2            /**< \brief (SYSCTRL_INTENCLR) OSC32K Ready Interrupt Enable */
#define SYSCTRL_INTENCLR_OSC32KRDY  (0x1ul << SYSCTRL_INTENCLR_OSC32KRDY_Pos)
#define SYSCTRL_INTENCLR_OSC8MRDY_Pos 3            /**< \brief (SYSCTRL_INTENCLR) OSC8M Ready Interrupt Enable */
#define SYSCTRL_INTENCLR_OSC8MRDY   (0x1ul << SYSCTRL_INTENCLR_OSC8MRDY_Pos)
#define SYSCTRL_INTENCLR_DFLLRDY_Pos 4            /**< \brief (SYSCTRL_INTENCLR) DFLL Ready Interrupt Enable */
#define SYSCTRL_INTENCLR_DFLLRDY    (0x1ul << SYSCTRL_INTENCLR_DFLLRDY_Pos)
#define SYSCTRL_INTENCLR_DFLLOOB_Pos 5            /**< \brief (SYSCTRL_INTENCLR) DFLL Out Of Bounds Interrupt Enable */
#define SYSCTRL_INTENCLR_DFLLOOB    (0x1ul << SYSCTRL_INTENCLR_DFLLOOB_Pos)
#define SYSCTRL_INTENCLR_DFLLLCKF_Pos 6            /**< \brief (SYSCTRL_INTENCLR) DFLL Lock Fine Interrupt Enable */
#define SYSCTRL_INTENCLR_DFLLLCKF   (0x1ul << SYSCTRL_INTENCLR_DFLLLCKF_Pos)
#define SYSCTRL_INTENCLR_DFLLLCKC_Pos 7            /**< \brief (SYSCTRL_INTENCLR) DFLL Lock Coarse Interrupt Enable */
#define SYSCTRL_INTENCLR_DFLLLCKC   (0x1ul << SYSCTRL_INTENCLR_DFLLLCKC_Pos)
#define SYSCTRL_INTENCLR_DFLLRCS_Pos 8            /**< \brief (SYSCTRL_INTENCLR) DFLL Reference Clock Stopped Interrupt Enable */
#define SYSCTRL_INTENCLR_DFLLRCS    (0x1ul << SYSCTRL_INTENCLR_DFLLRCS_Pos)
#define SYSCTRL_INTENCLR_BOD33RDY_Pos 9            /**< \brief (SYSCTRL_INTENCLR) BOD33 Ready Interrupt Enable */
#define SYSCTRL_INTENCLR_BOD33RDY   (0x1ul << SYSCTRL_INTENCLR_BOD33RDY_Pos)
#define SYSCTRL_INTENCLR_BOD33DET_Pos 10           /**< \brief (SYSCTRL_INTENCLR) BOD33 Detection Interrupt Enable */
#define SYSCTRL_INTENCLR_BOD33DET   (0x1ul << SYSCTRL_INTENCLR_BOD33DET_Pos)
#define SYSCTRL_INTENCLR_B33SRDY_Pos 11           /**< \brief (SYSCTRL_INTENCLR) BOD33 Synchronization Ready Interrupt Enable */
#define SYSCTRL_INTENCLR_B33SRDY    (0x1ul << SYSCTRL_INTENCLR_B33SRDY_Pos)
#define SYSCTRL_INTENCLR_DPLLLCKR_Pos 15           /**< \brief (SYSCTRL_INTENCLR) DPLL Lock Rise Interrupt Enable */
#define SYSCTRL_INTENCLR_DPLLLCKR   (0x1ul << SYSCTRL_INTENCLR_DPLLLCKR_Pos)
#define SYSCTRL_INTENCLR_DPLLLCKF_Pos 16           /**< \brief (SYSCTRL_INTENCLR) DPLL Lock Fall Interrupt Enable */
#define SYSCTRL_INTENCLR_DPLLLCKF   (0x1ul << SYSCTRL_INTENCLR_DPLLLCKF_Pos)
#define SYSCTRL_INTENCLR_DPLLLTO_Pos 17           /**< \brief (SYSCTRL_INTENCLR) DPLL Lock Timeout Interrupt Enable */
#define SYSCTRL_INTENCLR_DPLLLTO    (0x1ul << SYSCTRL_INTENCLR_DPLLLTO_Pos)
#define SYSCTRL_INTENCLR_MASK       0x00038FFFul /**< \brief (SYSCTRL_INTENCLR) MASK Register */

/* -------- SYSCTRL_INTENSET : (SYSCTRL Offset: 0x04) (R/W 32) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t XOSCRDY:1;        /*!< bit:      0  XOSC Ready Interrupt Enable        */
    uint32_t XOSC32KRDY:1;     /*!< bit:      1  XOSC32K Ready Interrupt Enable     */
    uint32_t OSC32KRDY:1;      /*!< bit:      2  OSC32K Ready Interrupt Enable      */
    uint32_t OSC8MRDY:1;       /*!< bit:      3  OSC8M Ready Interrupt Enable       */
    uint32_t DFLLRDY:1;        /*!< bit:      4  DFLL Ready Interrupt Enable        */
    uint32_t DFLLOOB:1;        /*!< bit:      5  DFLL Out Of Bounds Interrupt Enable */
    uint32_t DFLLLCKF:1;       /*!< bit:      6  DFLL Lock Fine Interrupt Enable    */
    uint32_t DFLLLCKC:1;       /*!< bit:      7  DFLL Lock Coarse Interrupt Enable  */
    uint32_t DFLLRCS:1;        /*!< bit:      8  DFLL Reference Clock Stopped Interrupt Enable */
    uint32_t BOD33RDY:1;       /*!< bit:      9  BOD33 Ready Interrupt Enable       */
    uint32_t BOD33DET:1;       /*!< bit:     10  BOD33 Detection Interrupt Enable   */
    uint32_t B33SRDY:1;        /*!< bit:     11  BOD33 Synchronization Ready Interrupt Enable */
    uint32_t :3;               /*!< bit: 12..14  Reserved                           */
    uint32_t DPLLLCKR:1;       /*!< bit:     15  DPLL Lock Rise Interrupt Enable    */
    uint32_t DPLLLCKF:1;       /*!< bit:     16  DPLL Lock Fall Interrupt Enable    */
    uint32_t DPLLLTO:1;        /*!< bit:     17  DPLL Lock Timeout Interrupt Enable */
    uint32_t :14;              /*!< bit: 18..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SYSCTRL_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_INTENSET_OFFSET     0x04         /**< \brief (SYSCTRL_INTENSET offset) Interrupt Enable Set */
#define SYSCTRL_INTENSET_RESETVALUE 0x00000000ul /**< \brief (SYSCTRL_INTENSET reset_value) Interrupt Enable Set */

#define SYSCTRL_INTENSET_XOSCRDY_Pos 0            /**< \brief (SYSCTRL_INTENSET) XOSC Ready Interrupt Enable */
#define SYSCTRL_INTENSET_XOSCRDY    (0x1ul << SYSCTRL_INTENSET_XOSCRDY_Pos)
#define SYSCTRL_INTENSET_XOSC32KRDY_Pos 1            /**< \brief (SYSCTRL_INTENSET) XOSC32K Ready Interrupt Enable */
#define SYSCTRL_INTENSET_XOSC32KRDY (0x1ul << SYSCTRL_INTENSET_XOSC32KRDY_Pos)
#define SYSCTRL_INTENSET_OSC32KRDY_Pos 2            /**< \brief (SYSCTRL_INTENSET) OSC32K Ready Interrupt Enable */
#define SYSCTRL_INTENSET_OSC32KRDY  (0x1ul << SYSCTRL_INTENSET_OSC32KRDY_Pos)
#define SYSCTRL_INTENSET_OSC8MRDY_Pos 3            /**< \brief (SYSCTRL_INTENSET) OSC8M Ready Interrupt Enable */
#define SYSCTRL_INTENSET_OSC8MRDY   (0x1ul << SYSCTRL_INTENSET_OSC8MRDY_Pos)
#define SYSCTRL_INTENSET_DFLLRDY_Pos 4            /**< \brief (SYSCTRL_INTENSET) DFLL Ready Interrupt Enable */
#define SYSCTRL_INTENSET_DFLLRDY    (0x1ul << SYSCTRL_INTENSET_DFLLRDY_Pos)
#define SYSCTRL_INTENSET_DFLLOOB_Pos 5            /**< \brief (SYSCTRL_INTENSET) DFLL Out Of Bounds Interrupt Enable */
#define SYSCTRL_INTENSET_DFLLOOB    (0x1ul << SYSCTRL_INTENSET_DFLLOOB_Pos)
#define SYSCTRL_INTENSET_DFLLLCKF_Pos 6            /**< \brief (SYSCTRL_INTENSET) DFLL Lock Fine Interrupt Enable */
#define SYSCTRL_INTENSET_DFLLLCKF   (0x1ul << SYSCTRL_INTENSET_DFLLLCKF_Pos)
#define SYSCTRL_INTENSET_DFLLLCKC_Pos 7            /**< \brief (SYSCTRL_INTENSET) DFLL Lock Coarse Interrupt Enable */
#define SYSCTRL_INTENSET_DFLLLCKC   (0x1ul << SYSCTRL_INTENSET_DFLLLCKC_Pos)
#define SYSCTRL_INTENSET_DFLLRCS_Pos 8            /**< \brief (SYSCTRL_INTENSET) DFLL Reference Clock Stopped Interrupt Enable */
#define SYSCTRL_INTENSET_DFLLRCS    (0x1ul << SYSCTRL_INTENSET_DFLLRCS_Pos)
#define SYSCTRL_INTENSET_BOD33RDY_Pos 9            /**< \brief (SYSCTRL_INTENSET) BOD33 Ready Interrupt Enable */
#define SYSCTRL_INTENSET_BOD33RDY   (0x1ul << SYSCTRL_INTENSET_BOD33RDY_Pos)
#define SYSCTRL_INTENSET_BOD33DET_Pos 10           /**< \brief (SYSCTRL_INTENSET) BOD33 Detection Interrupt Enable */
#define SYSCTRL_INTENSET_BOD33DET   (0x1ul << SYSCTRL_INTENSET_BOD33DET_Pos)
#define SYSCTRL_INTENSET_B33SRDY_Pos 11           /**< \brief (SYSCTRL_INTENSET) BOD33 Synchronization Ready Interrupt Enable */
#define SYSCTRL_INTENSET_B33SRDY    (0x1ul << SYSCTRL_INTENSET_B33SRDY_Pos)
#define SYSCTRL_INTENSET_DPLLLCKR_Pos 15           /**< \brief (SYSCTRL_INTENSET) DPLL Lock Rise Interrupt Enable */
#define SYSCTRL_INTENSET_DPLLLCKR   (0x1ul << SYSCTRL_INTENSET_DPLLLCKR_Pos)
#define SYSCTRL_INTENSET_DPLLLCKF_Pos 16           /**< \brief (SYSCTRL_INTENSET) DPLL Lock Fall Interrupt Enable */
#define SYSCTRL_INTENSET_DPLLLCKF   (0x1ul << SYSCTRL_INTENSET_DPLLLCKF_Pos)
#define SYSCTRL_INTENSET_DPLLLTO_Pos 17           /**< \brief (SYSCTRL_INTENSET) DPLL Lock Timeout Interrupt Enable */
#define SYSCTRL_INTENSET_DPLLLTO    (0x1ul << SYSCTRL_INTENSET_DPLLLTO_Pos)
#define SYSCTRL_INTENSET_MASK       0x00038FFFul /**< \brief (SYSCTRL_INTENSET) MASK Register */

/* -------- SYSCTRL_INTFLAG : (SYSCTRL Offset: 0x08) (R/W 32) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t XOSCRDY:1;        /*!< bit:      0  XOSC Ready                         */
    __I uint32_t XOSC32KRDY:1;     /*!< bit:      1  XOSC32K Ready                      */
    __I uint32_t OSC32KRDY:1;      /*!< bit:      2  OSC32K Ready                       */
    __I uint32_t OSC8MRDY:1;       /*!< bit:      3  OSC8M Ready                        */
    __I uint32_t DFLLRDY:1;        /*!< bit:      4  DFLL Ready                         */
    __I uint32_t DFLLOOB:1;        /*!< bit:      5  DFLL Out Of Bounds                 */
    __I uint32_t DFLLLCKF:1;       /*!< bit:      6  DFLL Lock Fine                     */
    __I uint32_t DFLLLCKC:1;       /*!< bit:      7  DFLL Lock Coarse                   */
    __I uint32_t DFLLRCS:1;        /*!< bit:      8  DFLL Reference Clock Stopped       */
    __I uint32_t BOD33RDY:1;       /*!< bit:      9  BOD33 Ready                        */
    __I uint32_t BOD33DET:1;       /*!< bit:     10  BOD33 Detection                    */
    __I uint32_t B33SRDY:1;        /*!< bit:     11  BOD33 Synchronization Ready        */
    __I uint32_t :3;               /*!< bit: 12..14  Reserved                           */
    __I uint32_t DPLLLCKR:1;       /*!< bit:     15  DPLL Lock Rise                     */
    __I uint32_t DPLLLCKF:1;       /*!< bit:     16  DPLL Lock Fall                     */
    __I uint32_t DPLLLTO:1;        /*!< bit:     17  DPLL Lock Timeout                  */
    __I uint32_t :14;              /*!< bit: 18..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SYSCTRL_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_INTFLAG_OFFSET      0x08         /**< \brief (SYSCTRL_INTFLAG offset) Interrupt Flag Status and Clear */
#define SYSCTRL_INTFLAG_RESETVALUE  0x00000000ul /**< \brief (SYSCTRL_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define SYSCTRL_INTFLAG_XOSCRDY_Pos 0            /**< \brief (SYSCTRL_INTFLAG) XOSC Ready */
#define SYSCTRL_INTFLAG_XOSCRDY     (0x1ul << SYSCTRL_INTFLAG_XOSCRDY_Pos)
#define SYSCTRL_INTFLAG_XOSC32KRDY_Pos 1            /**< \brief (SYSCTRL_INTFLAG) XOSC32K Ready */
#define SYSCTRL_INTFLAG_XOSC32KRDY  (0x1ul << SYSCTRL_INTFLAG_XOSC32KRDY_Pos)
#define SYSCTRL_INTFLAG_OSC32KRDY_Pos 2            /**< \brief (SYSCTRL_INTFLAG) OSC32K Ready */
#define SYSCTRL_INTFLAG_OSC32KRDY   (0x1ul << SYSCTRL_INTFLAG_OSC32KRDY_Pos)
#define SYSCTRL_INTFLAG_OSC8MRDY_Pos 3            /**< \brief (SYSCTRL_INTFLAG) OSC8M Ready */
#define SYSCTRL_INTFLAG_OSC8MRDY    (0x1ul << SYSCTRL_INTFLAG_OSC8MRDY_Pos)
#define SYSCTRL_INTFLAG_DFLLRDY_Pos 4            /**< \brief (SYSCTRL_INTFLAG) DFLL Ready */
#define SYSCTRL_INTFLAG_DFLLRDY     (0x1ul << SYSCTRL_INTFLAG_DFLLRDY_Pos)
#define SYSCTRL_INTFLAG_DFLLOOB_Pos 5            /**< \brief (SYSCTRL_INTFLAG) DFLL Out Of Bounds */
#define SYSCTRL_INTFLAG_DFLLOOB     (0x1ul << SYSCTRL_INTFLAG_DFLLOOB_Pos)
#define SYSCTRL_INTFLAG_DFLLLCKF_Pos 6            /**< \brief (SYSCTRL_INTFLAG) DFLL Lock Fine */
#define SYSCTRL_INTFLAG_DFLLLCKF    (0x1ul << SYSCTRL_INTFLAG_DFLLLCKF_Pos)
#define SYSCTRL_INTFLAG_DFLLLCKC_Pos 7            /**< \brief (SYSCTRL_INTFLAG) DFLL Lock Coarse */
#define SYSCTRL_INTFLAG_DFLLLCKC    (0x1ul << SYSCTRL_INTFLAG_DFLLLCKC_Pos)
#define SYSCTRL_INTFLAG_DFLLRCS_Pos 8            /**< \brief (SYSCTRL_INTFLAG) DFLL Reference Clock Stopped */
#define SYSCTRL_INTFLAG_DFLLRCS     (0x1ul << SYSCTRL_INTFLAG_DFLLRCS_Pos)
#define SYSCTRL_INTFLAG_BOD33RDY_Pos 9            /**< \brief (SYSCTRL_INTFLAG) BOD33 Ready */
#define SYSCTRL_INTFLAG_BOD33RDY    (0x1ul << SYSCTRL_INTFLAG_BOD33RDY_Pos)
#define SYSCTRL_INTFLAG_BOD33DET_Pos 10           /**< \brief (SYSCTRL_INTFLAG) BOD33 Detection */
#define SYSCTRL_INTFLAG_BOD33DET    (0x1ul << SYSCTRL_INTFLAG_BOD33DET_Pos)
#define SYSCTRL_INTFLAG_B33SRDY_Pos 11           /**< \brief (SYSCTRL_INTFLAG) BOD33 Synchronization Ready */
#define SYSCTRL_INTFLAG_B33SRDY     (0x1ul << SYSCTRL_INTFLAG_B33SRDY_Pos)
#define SYSCTRL_INTFLAG_DPLLLCKR_Pos 15           /**< \brief (SYSCTRL_INTFLAG) DPLL Lock Rise */
#define SYSCTRL_INTFLAG_DPLLLCKR    (0x1ul << SYSCTRL_INTFLAG_DPLLLCKR_Pos)
#define SYSCTRL_INTFLAG_DPLLLCKF_Pos 16           /**< \brief (SYSCTRL_INTFLAG) DPLL Lock Fall */
#define SYSCTRL_INTFLAG_DPLLLCKF    (0x1ul << SYSCTRL_INTFLAG_DPLLLCKF_Pos)
#define SYSCTRL_INTFLAG_DPLLLTO_Pos 17           /**< \brief (SYSCTRL_INTFLAG) DPLL Lock Timeout */
#define SYSCTRL_INTFLAG_DPLLLTO     (0x1ul << SYSCTRL_INTFLAG_DPLLLTO_Pos)
#define SYSCTRL_INTFLAG_MASK        0x00038FFFul /**< \brief (SYSCTRL_INTFLAG) MASK Register */

/* -------- SYSCTRL_PCLKSR : (SYSCTRL Offset: 0x0C) (R/  32) Power and Clocks Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t XOSCRDY:1;        /*!< bit:      0  XOSC Ready                         */
    uint32_t XOSC32KRDY:1;     /*!< bit:      1  XOSC32K Ready                      */
    uint32_t OSC32KRDY:1;      /*!< bit:      2  OSC32K Ready                       */
    uint32_t OSC8MRDY:1;       /*!< bit:      3  OSC8M Ready                        */
    uint32_t DFLLRDY:1;        /*!< bit:      4  DFLL Ready                         */
    uint32_t DFLLOOB:1;        /*!< bit:      5  DFLL Out Of Bounds                 */
    uint32_t DFLLLCKF:1;       /*!< bit:      6  DFLL Lock Fine                     */
    uint32_t DFLLLCKC:1;       /*!< bit:      7  DFLL Lock Coarse                   */
    uint32_t DFLLRCS:1;        /*!< bit:      8  DFLL Reference Clock Stopped       */
    uint32_t BOD33RDY:1;       /*!< bit:      9  BOD33 Ready                        */
    uint32_t BOD33DET:1;       /*!< bit:     10  BOD33 Detection                    */
    uint32_t B33SRDY:1;        /*!< bit:     11  BOD33 Synchronization Ready        */
    uint32_t :3;               /*!< bit: 12..14  Reserved                           */
    uint32_t DPLLLCKR:1;       /*!< bit:     15  DPLL Lock Rise                     */
    uint32_t DPLLLCKF:1;       /*!< bit:     16  DPLL Lock Fall                     */
    uint32_t DPLLLTO:1;        /*!< bit:     17  DPLL Lock Timeout                  */
    uint32_t :14;              /*!< bit: 18..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SYSCTRL_PCLKSR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_PCLKSR_OFFSET       0x0C         /**< \brief (SYSCTRL_PCLKSR offset) Power and Clocks Status */
#define SYSCTRL_PCLKSR_RESETVALUE   0x00000000ul /**< \brief (SYSCTRL_PCLKSR reset_value) Power and Clocks Status */

#define SYSCTRL_PCLKSR_XOSCRDY_Pos  0            /**< \brief (SYSCTRL_PCLKSR) XOSC Ready */
#define SYSCTRL_PCLKSR_XOSCRDY      (0x1ul << SYSCTRL_PCLKSR_XOSCRDY_Pos)
#define SYSCTRL_PCLKSR_XOSC32KRDY_Pos 1            /**< \brief (SYSCTRL_PCLKSR) XOSC32K Ready */
#define SYSCTRL_PCLKSR_XOSC32KRDY   (0x1ul << SYSCTRL_PCLKSR_XOSC32KRDY_Pos)
#define SYSCTRL_PCLKSR_OSC32KRDY_Pos 2            /**< \brief (SYSCTRL_PCLKSR) OSC32K Ready */
#define SYSCTRL_PCLKSR_OSC32KRDY    (0x1ul << SYSCTRL_PCLKSR_OSC32KRDY_Pos)
#define SYSCTRL_PCLKSR_OSC8MRDY_Pos 3            /**< \brief (SYSCTRL_PCLKSR) OSC8M Ready */
#define SYSCTRL_PCLKSR_OSC8MRDY     (0x1ul << SYSCTRL_PCLKSR_OSC8MRDY_Pos)
#define SYSCTRL_PCLKSR_DFLLRDY_Pos  4            /**< \brief (SYSCTRL_PCLKSR) DFLL Ready */
#define SYSCTRL_PCLKSR_DFLLRDY      (0x1ul << SYSCTRL_PCLKSR_DFLLRDY_Pos)
#define SYSCTRL_PCLKSR_DFLLOOB_Pos  5            /**< \brief (SYSCTRL_PCLKSR) DFLL Out Of Bounds */
#define SYSCTRL_PCLKSR_DFLLOOB      (0x1ul << SYSCTRL_PCLKSR_DFLLOOB_Pos)
#define SYSCTRL_PCLKSR_DFLLLCKF_Pos 6            /**< \brief (SYSCTRL_PCLKSR) DFLL Lock Fine */
#define SYSCTRL_PCLKSR_DFLLLCKF     (0x1ul << SYSCTRL_PCLKSR_DFLLLCKF_Pos)
#define SYSCTRL_PCLKSR_DFLLLCKC_Pos 7            /**< \brief (SYSCTRL_PCLKSR) DFLL Lock Coarse */
#define SYSCTRL_PCLKSR_DFLLLCKC     (0x1ul << SYSCTRL_PCLKSR_DFLLLCKC_Pos)
#define SYSCTRL_PCLKSR_DFLLRCS_Pos  8            /**< \brief (SYSCTRL_PCLKSR) DFLL Reference Clock Stopped */
#define SYSCTRL_PCLKSR_DFLLRCS      (0x1ul << SYSCTRL_PCLKSR_DFLLRCS_Pos)
#define SYSCTRL_PCLKSR_BOD33RDY_Pos 9            /**< \brief (SYSCTRL_PCLKSR) BOD33 Ready */
#define SYSCTRL_PCLKSR_BOD33RDY     (0x1ul << SYSCTRL_PCLKSR_BOD33RDY_Pos)
#define SYSCTRL_PCLKSR_BOD33DET_Pos 10           /**< \brief (SYSCTRL_PCLKSR) BOD33 Detection */
#define SYSCTRL_PCLKSR_BOD33DET     (0x1ul << SYSCTRL_PCLKSR_BOD33DET_Pos)
#define SYSCTRL_PCLKSR_B33SRDY_Pos  11           /**< \brief (SYSCTRL_PCLKSR) BOD33 Synchronization Ready */
#define SYSCTRL_PCLKSR_B33SRDY      (0x1ul << SYSCTRL_PCLKSR_B33SRDY_Pos)
#define SYSCTRL_PCLKSR_DPLLLCKR_Pos 15           /**< \brief (SYSCTRL_PCLKSR) DPLL Lock Rise */
#define SYSCTRL_PCLKSR_DPLLLCKR     (0x1ul << SYSCTRL_PCLKSR_DPLLLCKR_Pos)
#define SYSCTRL_PCLKSR_DPLLLCKF_Pos 16           /**< \brief (SYSCTRL_PCLKSR) DPLL Lock Fall */
#define SYSCTRL_PCLKSR_DPLLLCKF     (0x1ul << SYSCTRL_PCLKSR_DPLLLCKF_Pos)
#define SYSCTRL_PCLKSR_DPLLLTO_Pos  17           /**< \brief (SYSCTRL_PCLKSR) DPLL Lock Timeout */
#define SYSCTRL_PCLKSR_DPLLLTO      (0x1ul << SYSCTRL_PCLKSR_DPLLLTO_Pos)
#define SYSCTRL_PCLKSR_MASK         0x00038FFFul /**< \brief (SYSCTRL_PCLKSR) MASK Register */

/* -------- SYSCTRL_XOSC : (SYSCTRL Offset: 0x10) (R/W 16) External Multipurpose Crystal Oscillator (XOSC) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t :1;               /*!< bit:      0  Reserved                           */
    uint16_t ENABLE:1;         /*!< bit:      1  Oscillator Enable                  */
    uint16_t XTALEN:1;         /*!< bit:      2  Crystal Oscillator Enable          */
    uint16_t :3;               /*!< bit:  3.. 5  Reserved                           */
    uint16_t RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint16_t ONDEMAND:1;       /*!< bit:      7  On Demand Control                  */
    uint16_t GAIN:3;           /*!< bit:  8..10  Oscillator Gain                    */
    uint16_t AMPGC:1;          /*!< bit:     11  Automatic Amplitude Gain Control   */
    uint16_t STARTUP:4;        /*!< bit: 12..15  Start-Up Time                      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} SYSCTRL_XOSC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_XOSC_OFFSET         0x10         /**< \brief (SYSCTRL_XOSC offset) External Multipurpose Crystal Oscillator (XOSC) Control */
#define SYSCTRL_XOSC_RESETVALUE     0x0080ul     /**< \brief (SYSCTRL_XOSC reset_value) External Multipurpose Crystal Oscillator (XOSC) Control */

#define SYSCTRL_XOSC_ENABLE_Pos     1            /**< \brief (SYSCTRL_XOSC) Oscillator Enable */
#define SYSCTRL_XOSC_ENABLE         (0x1ul << SYSCTRL_XOSC_ENABLE_Pos)
#define SYSCTRL_XOSC_XTALEN_Pos     2            /**< \brief (SYSCTRL_XOSC) Crystal Oscillator Enable */
#define SYSCTRL_XOSC_XTALEN         (0x1ul << SYSCTRL_XOSC_XTALEN_Pos)
#define SYSCTRL_XOSC_RUNSTDBY_Pos   6            /**< \brief (SYSCTRL_XOSC) Run in Standby */
#define SYSCTRL_XOSC_RUNSTDBY       (0x1ul << SYSCTRL_XOSC_RUNSTDBY_Pos)
#define SYSCTRL_XOSC_ONDEMAND_Pos   7            /**< \brief (SYSCTRL_XOSC) On Demand Control */
#define SYSCTRL_XOSC_ONDEMAND       (0x1ul << SYSCTRL_XOSC_ONDEMAND_Pos)
#define SYSCTRL_XOSC_GAIN_Pos       8            /**< \brief (SYSCTRL_XOSC) Oscillator Gain */
#define SYSCTRL_XOSC_GAIN_Msk       (0x7ul << SYSCTRL_XOSC_GAIN_Pos)
#define SYSCTRL_XOSC_GAIN(value)    (SYSCTRL_XOSC_GAIN_Msk & ((value) << SYSCTRL_XOSC_GAIN_Pos))
#define   SYSCTRL_XOSC_GAIN_0_Val         0x0ul  /**< \brief (SYSCTRL_XOSC) 2MHz */
#define   SYSCTRL_XOSC_GAIN_1_Val         0x1ul  /**< \brief (SYSCTRL_XOSC) 4MHz */
#define   SYSCTRL_XOSC_GAIN_2_Val         0x2ul  /**< \brief (SYSCTRL_XOSC) 8MHz */
#define   SYSCTRL_XOSC_GAIN_3_Val         0x3ul  /**< \brief (SYSCTRL_XOSC) 16MHz */
#define   SYSCTRL_XOSC_GAIN_4_Val         0x4ul  /**< \brief (SYSCTRL_XOSC) 30MHz */
#define SYSCTRL_XOSC_GAIN_0         (SYSCTRL_XOSC_GAIN_0_Val       << SYSCTRL_XOSC_GAIN_Pos)
#define SYSCTRL_XOSC_GAIN_1         (SYSCTRL_XOSC_GAIN_1_Val       << SYSCTRL_XOSC_GAIN_Pos)
#define SYSCTRL_XOSC_GAIN_2         (SYSCTRL_XOSC_GAIN_2_Val       << SYSCTRL_XOSC_GAIN_Pos)
#define SYSCTRL_XOSC_GAIN_3         (SYSCTRL_XOSC_GAIN_3_Val       << SYSCTRL_XOSC_GAIN_Pos)
#define SYSCTRL_XOSC_GAIN_4         (SYSCTRL_XOSC_GAIN_4_Val       << SYSCTRL_XOSC_GAIN_Pos)
#define SYSCTRL_XOSC_AMPGC_Pos      11           /**< \brief (SYSCTRL_XOSC) Automatic Amplitude Gain Control */
#define SYSCTRL_XOSC_AMPGC          (0x1ul << SYSCTRL_XOSC_AMPGC_Pos)
#define SYSCTRL_XOSC_STARTUP_Pos    12           /**< \brief (SYSCTRL_XOSC) Start-Up Time */
#define SYSCTRL_XOSC_STARTUP_Msk    (0xFul << SYSCTRL_XOSC_STARTUP_Pos)
#define SYSCTRL_XOSC_STARTUP(value) (SYSCTRL_XOSC_STARTUP_Msk & ((value) << SYSCTRL_XOSC_STARTUP_Pos))
#define SYSCTRL_XOSC_MASK           0xFFC6ul     /**< \brief (SYSCTRL_XOSC) MASK Register */

/* -------- SYSCTRL_XOSC32K : (SYSCTRL Offset: 0x14) (R/W 16) 32kHz External Crystal Oscillator (XOSC32K) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t :1;               /*!< bit:      0  Reserved                           */
    uint16_t ENABLE:1;         /*!< bit:      1  Oscillator Enable                  */
    uint16_t XTALEN:1;         /*!< bit:      2  Crystal Oscillator Enable          */
    uint16_t EN32K:1;          /*!< bit:      3  32kHz Output Enable                */
    uint16_t EN1K:1;           /*!< bit:      4  1kHz Output Enable                 */
    uint16_t AAMPEN:1;         /*!< bit:      5  Automatic Amplitude Control Enable */
    uint16_t RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint16_t ONDEMAND:1;       /*!< bit:      7  On Demand Control                  */
    uint16_t STARTUP:3;        /*!< bit:  8..10  Oscillator Start-Up Time           */
    uint16_t :1;               /*!< bit:     11  Reserved                           */
    uint16_t WRTLOCK:1;        /*!< bit:     12  Write Lock                         */
    uint16_t :3;               /*!< bit: 13..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} SYSCTRL_XOSC32K_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_XOSC32K_OFFSET      0x14         /**< \brief (SYSCTRL_XOSC32K offset) 32kHz External Crystal Oscillator (XOSC32K) Control */
#define SYSCTRL_XOSC32K_RESETVALUE  0x0080ul     /**< \brief (SYSCTRL_XOSC32K reset_value) 32kHz External Crystal Oscillator (XOSC32K) Control */

#define SYSCTRL_XOSC32K_ENABLE_Pos  1            /**< \brief (SYSCTRL_XOSC32K) Oscillator Enable */
#define SYSCTRL_XOSC32K_ENABLE      (0x1ul << SYSCTRL_XOSC32K_ENABLE_Pos)
#define SYSCTRL_XOSC32K_XTALEN_Pos  2            /**< \brief (SYSCTRL_XOSC32K) Crystal Oscillator Enable */
#define SYSCTRL_XOSC32K_XTALEN      (0x1ul << SYSCTRL_XOSC32K_XTALEN_Pos)
#define SYSCTRL_XOSC32K_EN32K_Pos   3            /**< \brief (SYSCTRL_XOSC32K) 32kHz Output Enable */
#define SYSCTRL_XOSC32K_EN32K       (0x1ul << SYSCTRL_XOSC32K_EN32K_Pos)
#define SYSCTRL_XOSC32K_EN1K_Pos    4            /**< \brief (SYSCTRL_XOSC32K) 1kHz Output Enable */
#define SYSCTRL_XOSC32K_EN1K        (0x1ul << SYSCTRL_XOSC32K_EN1K_Pos)
#define SYSCTRL_XOSC32K_AAMPEN_Pos  5            /**< \brief (SYSCTRL_XOSC32K) Automatic Amplitude Control Enable */
#define SYSCTRL_XOSC32K_AAMPEN      (0x1ul << SYSCTRL_XOSC32K_AAMPEN_Pos)
#define SYSCTRL_XOSC32K_RUNSTDBY_Pos 6            /**< \brief (SYSCTRL_XOSC32K) Run in Standby */
#define SYSCTRL_XOSC32K_RUNSTDBY    (0x1ul << SYSCTRL_XOSC32K_RUNSTDBY_Pos)
#define SYSCTRL_XOSC32K_ONDEMAND_Pos 7            /**< \brief (SYSCTRL_XOSC32K) On Demand Control */
#define SYSCTRL_XOSC32K_ONDEMAND    (0x1ul << SYSCTRL_XOSC32K_ONDEMAND_Pos)
#define SYSCTRL_XOSC32K_STARTUP_Pos 8            /**< \brief (SYSCTRL_XOSC32K) Oscillator Start-Up Time */
#define SYSCTRL_XOSC32K_STARTUP_Msk (0x7ul << SYSCTRL_XOSC32K_STARTUP_Pos)
#define SYSCTRL_XOSC32K_STARTUP(value) (SYSCTRL_XOSC32K_STARTUP_Msk & ((value) << SYSCTRL_XOSC32K_STARTUP_Pos))
#define SYSCTRL_XOSC32K_WRTLOCK_Pos 12           /**< \brief (SYSCTRL_XOSC32K) Write Lock */
#define SYSCTRL_XOSC32K_WRTLOCK     (0x1ul << SYSCTRL_XOSC32K_WRTLOCK_Pos)
#define SYSCTRL_XOSC32K_MASK        0x17FEul     /**< \brief (SYSCTRL_XOSC32K) MASK Register */

/* -------- SYSCTRL_OSC32K : (SYSCTRL Offset: 0x18) (R/W 32) 32kHz Internal Oscillator (OSC32K) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t ENABLE:1;         /*!< bit:      1  Oscillator Enable                  */
    uint32_t EN32K:1;          /*!< bit:      2  32kHz Output Enable                */
    uint32_t EN1K:1;           /*!< bit:      3  1kHz Output Enable                 */
    uint32_t :2;               /*!< bit:  4.. 5  Reserved                           */
    uint32_t RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint32_t ONDEMAND:1;       /*!< bit:      7  On Demand Control                  */
    uint32_t STARTUP:3;        /*!< bit:  8..10  Oscillator Start-Up Time           */
    uint32_t :1;               /*!< bit:     11  Reserved                           */
    uint32_t WRTLOCK:1;        /*!< bit:     12  Write Lock                         */
    uint32_t :3;               /*!< bit: 13..15  Reserved                           */
    uint32_t CALIB:7;          /*!< bit: 16..22  Oscillator Calibration             */
    uint32_t :9;               /*!< bit: 23..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SYSCTRL_OSC32K_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_OSC32K_OFFSET       0x18         /**< \brief (SYSCTRL_OSC32K offset) 32kHz Internal Oscillator (OSC32K) Control */
#define SYSCTRL_OSC32K_RESETVALUE   0x003F0080ul /**< \brief (SYSCTRL_OSC32K reset_value) 32kHz Internal Oscillator (OSC32K) Control */

#define SYSCTRL_OSC32K_ENABLE_Pos   1            /**< \brief (SYSCTRL_OSC32K) Oscillator Enable */
#define SYSCTRL_OSC32K_ENABLE       (0x1ul << SYSCTRL_OSC32K_ENABLE_Pos)
#define SYSCTRL_OSC32K_EN32K_Pos    2            /**< \brief (SYSCTRL_OSC32K) 32kHz Output Enable */
#define SYSCTRL_OSC32K_EN32K        (0x1ul << SYSCTRL_OSC32K_EN32K_Pos)
#define SYSCTRL_OSC32K_EN1K_Pos     3            /**< \brief (SYSCTRL_OSC32K) 1kHz Output Enable */
#define SYSCTRL_OSC32K_EN1K         (0x1ul << SYSCTRL_OSC32K_EN1K_Pos)
#define SYSCTRL_OSC32K_RUNSTDBY_Pos 6            /**< \brief (SYSCTRL_OSC32K) Run in Standby */
#define SYSCTRL_OSC32K_RUNSTDBY     (0x1ul << SYSCTRL_OSC32K_RUNSTDBY_Pos)
#define SYSCTRL_OSC32K_ONDEMAND_Pos 7            /**< \brief (SYSCTRL_OSC32K) On Demand Control */
#define SYSCTRL_OSC32K_ONDEMAND     (0x1ul << SYSCTRL_OSC32K_ONDEMAND_Pos)
#define SYSCTRL_OSC32K_STARTUP_Pos  8            /**< \brief (SYSCTRL_OSC32K) Oscillator Start-Up Time */
#define SYSCTRL_OSC32K_STARTUP_Msk  (0x7ul << SYSCTRL_OSC32K_STARTUP_Pos)
#define SYSCTRL_OSC32K_STARTUP(value) (SYSCTRL_OSC32K_STARTUP_Msk & ((value) << SYSCTRL_OSC32K_STARTUP_Pos))
#define SYSCTRL_OSC32K_WRTLOCK_Pos  12           /**< \brief (SYSCTRL_OSC32K) Write Lock */
#define SYSCTRL_OSC32K_WRTLOCK      (0x1ul << SYSCTRL_OSC32K_WRTLOCK_Pos)
#define SYSCTRL_OSC32K_CALIB_Pos    16           /**< \brief (SYSCTRL_OSC32K) Oscillator Calibration */
#define SYSCTRL_OSC32K_CALIB_Msk    (0x7Ful << SYSCTRL_OSC32K_CALIB_Pos)
#define SYSCTRL_OSC32K_CALIB(value) (SYSCTRL_OSC32K_CALIB_Msk & ((value) << SYSCTRL_OSC32K_CALIB_Pos))
#define SYSCTRL_OSC32K_MASK         0x007F17CEul /**< \brief (SYSCTRL_OSC32K) MASK Register */

/* -------- SYSCTRL_OSCULP32K : (SYSCTRL Offset: 0x1C) (R/W  8) 32kHz Ultra Low Power Internal Oscillator (OSCULP32K) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CALIB:5;          /*!< bit:  0.. 4  Oscillator Calibration             */
    uint8_t  :2;               /*!< bit:  5.. 6  Reserved                           */
    uint8_t  WRTLOCK:1;        /*!< bit:      7  Write Lock                         */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} SYSCTRL_OSCULP32K_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_OSCULP32K_OFFSET    0x1C         /**< \brief (SYSCTRL_OSCULP32K offset) 32kHz Ultra Low Power Internal Oscillator (OSCULP32K) Control */
#define SYSCTRL_OSCULP32K_RESETVALUE 0x1Ful       /**< \brief (SYSCTRL_OSCULP32K reset_value) 32kHz Ultra Low Power Internal Oscillator (OSCULP32K) Control */

#define SYSCTRL_OSCULP32K_CALIB_Pos 0            /**< \brief (SYSCTRL_OSCULP32K) Oscillator Calibration */
#define SYSCTRL_OSCULP32K_CALIB_Msk (0x1Ful << SYSCTRL_OSCULP32K_CALIB_Pos)
#define SYSCTRL_OSCULP32K_CALIB(value) (SYSCTRL_OSCULP32K_CALIB_Msk & ((value) << SYSCTRL_OSCULP32K_CALIB_Pos))
#define SYSCTRL_OSCULP32K_WRTLOCK_Pos 7            /**< \brief (SYSCTRL_OSCULP32K) Write Lock */
#define SYSCTRL_OSCULP32K_WRTLOCK   (0x1ul << SYSCTRL_OSCULP32K_WRTLOCK_Pos)
#define SYSCTRL_OSCULP32K_MASK      0x9Ful       /**< \brief (SYSCTRL_OSCULP32K) MASK Register */

/* -------- SYSCTRL_OSC8M : (SYSCTRL Offset: 0x20) (R/W 32) 8MHz Internal Oscillator (OSC8M) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t ENABLE:1;         /*!< bit:      1  Oscillator Enable                  */
    uint32_t :4;               /*!< bit:  2.. 5  Reserved                           */
    uint32_t RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint32_t ONDEMAND:1;       /*!< bit:      7  On Demand Control                  */
    uint32_t PRESC:2;          /*!< bit:  8.. 9  Oscillator Prescaler               */
    uint32_t :6;               /*!< bit: 10..15  Reserved                           */
    uint32_t CALIB:12;         /*!< bit: 16..27  Oscillator Calibration             */
    uint32_t :2;               /*!< bit: 28..29  Reserved                           */
    uint32_t FRANGE:2;         /*!< bit: 30..31  Oscillator Frequency Range         */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SYSCTRL_OSC8M_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_OSC8M_OFFSET        0x20         /**< \brief (SYSCTRL_OSC8M offset) 8MHz Internal Oscillator (OSC8M) Control */
#define SYSCTRL_OSC8M_RESETVALUE    0x87070382ul /**< \brief (SYSCTRL_OSC8M reset_value) 8MHz Internal Oscillator (OSC8M) Control */

#define SYSCTRL_OSC8M_ENABLE_Pos    1            /**< \brief (SYSCTRL_OSC8M) Oscillator Enable */
#define SYSCTRL_OSC8M_ENABLE        (0x1ul << SYSCTRL_OSC8M_ENABLE_Pos)
#define SYSCTRL_OSC8M_RUNSTDBY_Pos  6            /**< \brief (SYSCTRL_OSC8M) Run in Standby */
#define SYSCTRL_OSC8M_RUNSTDBY      (0x1ul << SYSCTRL_OSC8M_RUNSTDBY_Pos)
#define SYSCTRL_OSC8M_ONDEMAND_Pos  7            /**< \brief (SYSCTRL_OSC8M) On Demand Control */
#define SYSCTRL_OSC8M_ONDEMAND      (0x1ul << SYSCTRL_OSC8M_ONDEMAND_Pos)
#define SYSCTRL_OSC8M_PRESC_Pos     8            /**< \brief (SYSCTRL_OSC8M) Oscillator Prescaler */
#define SYSCTRL_OSC8M_PRESC_Msk     (0x3ul << SYSCTRL_OSC8M_PRESC_Pos)
#define SYSCTRL_OSC8M_PRESC(value)  (SYSCTRL_OSC8M_PRESC_Msk & ((value) << SYSCTRL_OSC8M_PRESC_Pos))
#define   SYSCTRL_OSC8M_PRESC_0_Val       0x0ul  /**< \brief (SYSCTRL_OSC8M) 1 */
#define   SYSCTRL_OSC8M_PRESC_1_Val       0x1ul  /**< \brief (SYSCTRL_OSC8M) 2 */
#define   SYSCTRL_OSC8M_PRESC_2_Val       0x2ul  /**< \brief (SYSCTRL_OSC8M) 4 */
#define   SYSCTRL_OSC8M_PRESC_3_Val       0x3ul  /**< \brief (SYSCTRL_OSC8M) 8 */
#define SYSCTRL_OSC8M_PRESC_0       (SYSCTRL_OSC8M_PRESC_0_Val     << SYSCTRL_OSC8M_PRESC_Pos)
#define SYSCTRL_OSC8M_PRESC_1       (SYSCTRL_OSC8M_PRESC_1_Val     << SYSCTRL_OSC8M_PRESC_Pos)
#define SYSCTRL_OSC8M_PRESC_2       (SYSCTRL_OSC8M_PRESC_2_Val     << SYSCTRL_OSC8M_PRESC_Pos)
#define SYSCTRL_OSC8M_PRESC_3       (SYSCTRL_OSC8M_PRESC_3_Val     << SYSCTRL_OSC8M_PRESC_Pos)
#define SYSCTRL_OSC8M_CALIB_Pos     16           /**< \brief (SYSCTRL_OSC8M) Oscillator Calibration */
#define SYSCTRL_OSC8M_CALIB_Msk     (0xFFFul << SYSCTRL_OSC8M_CALIB_Pos)
#define SYSCTRL_OSC8M_CALIB(value)  (SYSCTRL_OSC8M_CALIB_Msk & ((value) << SYSCTRL_OSC8M_CALIB_Pos))
#define SYSCTRL_OSC8M_FRANGE_Pos    30           /**< \brief (SYSCTRL_OSC8M) Oscillator Frequency Range */
#define SYSCTRL_OSC8M_FRANGE_Msk    (0x3ul << SYSCTRL_OSC8M_FRANGE_Pos)
#define SYSCTRL_OSC8M_FRANGE(value) (SYSCTRL_OSC8M_FRANGE_Msk & ((value) << SYSCTRL_OSC8M_FRANGE_Pos))
#define   SYSCTRL_OSC8M_FRANGE_0_Val      0x0ul  /**< \brief (SYSCTRL_OSC8M) 4 to 6MHz */
#define   SYSCTRL_OSC8M_FRANGE_1_Val      0x1ul  /**< \brief (SYSCTRL_OSC8M) 6 to 8MHz */
#define   SYSCTRL_OSC8M_FRANGE_2_Val      0x2ul  /**< \brief (SYSCTRL_OSC8M) 8 to 11MHz */
#define   SYSCTRL_OSC8M_FRANGE_3_Val      0x3ul  /**< \brief (SYSCTRL_OSC8M) 11 to 15MHz */
#define SYSCTRL_OSC8M_FRANGE_0      (SYSCTRL_OSC8M_FRANGE_0_Val    << SYSCTRL_OSC8M_FRANGE_Pos)
#define SYSCTRL_OSC8M_FRANGE_1      (SYSCTRL_OSC8M_FRANGE_1_Val    << SYSCTRL_OSC8M_FRANGE_Pos)
#define SYSCTRL_OSC8M_FRANGE_2      (SYSCTRL_OSC8M_FRANGE_2_Val    << SYSCTRL_OSC8M_FRANGE_Pos)
#define SYSCTRL_OSC8M_FRANGE_3      (SYSCTRL_OSC8M_FRANGE_3_Val    << SYSCTRL_OSC8M_FRANGE_Pos)
#define SYSCTRL_OSC8M_MASK          0xCFFF03C2ul /**< \brief (SYSCTRL_OSC8M) MASK Register */

/* -------- SYSCTRL_DFLLCTRL : (SYSCTRL Offset: 0x24) (R/W 16) DFLL48M Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t :1;               /*!< bit:      0  Reserved                           */
    uint16_t ENABLE:1;         /*!< bit:      1  DFLL Enable                        */
    uint16_t MODE:1;           /*!< bit:      2  Operating Mode Selection           */
    uint16_t STABLE:1;         /*!< bit:      3  Stable DFLL Frequency              */
    uint16_t LLAW:1;           /*!< bit:      4  Lose Lock After Wake               */
    uint16_t USBCRM:1;         /*!< bit:      5  USB Clock Recovery Mode            */
    uint16_t RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint16_t ONDEMAND:1;       /*!< bit:      7  On Demand Control                  */
    uint16_t CCDIS:1;          /*!< bit:      8  Chill Cycle Disable                */
    uint16_t QLDIS:1;          /*!< bit:      9  Quick Lock Disable                 */
    uint16_t BPLCKC:1;         /*!< bit:     10  Bypass Coarse Lock                 */
    uint16_t WAITLOCK:1;       /*!< bit:     11  Wait Lock                          */
    uint16_t :4;               /*!< bit: 12..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} SYSCTRL_DFLLCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_DFLLCTRL_OFFSET     0x24         /**< \brief (SYSCTRL_DFLLCTRL offset) DFLL48M Control */
#define SYSCTRL_DFLLCTRL_RESETVALUE 0x0080ul     /**< \brief (SYSCTRL_DFLLCTRL reset_value) DFLL48M Control */

#define SYSCTRL_DFLLCTRL_ENABLE_Pos 1            /**< \brief (SYSCTRL_DFLLCTRL) DFLL Enable */
#define SYSCTRL_DFLLCTRL_ENABLE     (0x1ul << SYSCTRL_DFLLCTRL_ENABLE_Pos)
#define SYSCTRL_DFLLCTRL_MODE_Pos   2            /**< \brief (SYSCTRL_DFLLCTRL) Operating Mode Selection */
#define SYSCTRL_DFLLCTRL_MODE       (0x1ul << SYSCTRL_DFLLCTRL_MODE_Pos)
#define SYSCTRL_DFLLCTRL_STABLE_Pos 3            /**< \brief (SYSCTRL_DFLLCTRL) Stable DFLL Frequency */
#define SYSCTRL_DFLLCTRL_STABLE     (0x1ul << SYSCTRL_DFLLCTRL_STABLE_Pos)
#define SYSCTRL_DFLLCTRL_LLAW_Pos   4            /**< \brief (SYSCTRL_DFLLCTRL) Lose Lock After Wake */
#define SYSCTRL_DFLLCTRL_LLAW       (0x1ul << SYSCTRL_DFLLCTRL_LLAW_Pos)
#define SYSCTRL_DFLLCTRL_USBCRM_Pos 5            /**< \brief (SYSCTRL_DFLLCTRL) USB Clock Recovery Mode */
#define SYSCTRL_DFLLCTRL_USBCRM     (0x1ul << SYSCTRL_DFLLCTRL_USBCRM_Pos)
#define SYSCTRL_DFLLCTRL_RUNSTDBY_Pos 6            /**< \brief (SYSCTRL_DFLLCTRL) Run in Standby */
#define SYSCTRL_DFLLCTRL_RUNSTDBY   (0x1ul << SYSCTRL_DFLLCTRL_RUNSTDBY_Pos)
#define SYSCTRL_DFLLCTRL_ONDEMAND_Pos 7            /**< \brief (SYSCTRL_DFLLCTRL) On Demand Control */
#define SYSCTRL_DFLLCTRL_ONDEMAND   (0x1ul << SYSCTRL_DFLLCTRL_ONDEMAND_Pos)
#define SYSCTRL_DFLLCTRL_CCDIS_Pos  8            /**< \brief (SYSCTRL_DFLLCTRL) Chill Cycle Disable */
#define SYSCTRL_DFLLCTRL_CCDIS      (0x1ul << SYSCTRL_DFLLCTRL_CCDIS_Pos)
#define SYSCTRL_DFLLCTRL_QLDIS_Pos  9            /**< \brief (SYSCTRL_DFLLCTRL) Quick Lock Disable */
#define SYSCTRL_DFLLCTRL_QLDIS      (0x1ul << SYSCTRL_DFLLCTRL_QLDIS_Pos)
#define SYSCTRL_DFLLCTRL_BPLCKC_Pos 10           /**< \brief (SYSCTRL_DFLLCTRL) Bypass Coarse Lock */
#define SYSCTRL_DFLLCTRL_BPLCKC     (0x1ul << SYSCTRL_DFLLCTRL_BPLCKC_Pos)
#define SYSCTRL_DFLLCTRL_WAITLOCK_Pos 11           /**< \brief (SYSCTRL_DFLLCTRL) Wait Lock */
#define SYSCTRL_DFLLCTRL_WAITLOCK   (0x1ul << SYSCTRL_DFLLCTRL_WAITLOCK_Pos)
#define SYSCTRL_DFLLCTRL_MASK       0x0FFEul     /**< \brief (SYSCTRL_DFLLCTRL) MASK Register */

/* -------- SYSCTRL_DFLLVAL : (SYSCTRL Offset: 0x28) (R/W 32) DFLL48M Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t FINE:10;          /*!< bit:  0.. 9  Fine Value                         */
    uint32_t COARSE:6;         /*!< bit: 10..15  Coarse Value                       */
    uint32_t DIFF:16;          /*!< bit: 16..31  Multiplication Ratio Difference    */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SYSCTRL_DFLLVAL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_DFLLVAL_OFFSET      0x28         /**< \brief (SYSCTRL_DFLLVAL offset) DFLL48M Value */
#define SYSCTRL_DFLLVAL_RESETVALUE  0x00000000ul /**< \brief (SYSCTRL_DFLLVAL reset_value) DFLL48M Value */

#define SYSCTRL_DFLLVAL_FINE_Pos    0            /**< \brief (SYSCTRL_DFLLVAL) Fine Value */
#define SYSCTRL_DFLLVAL_FINE_Msk    (0x3FFul << SYSCTRL_DFLLVAL_FINE_Pos)
#define SYSCTRL_DFLLVAL_FINE(value) (SYSCTRL_DFLLVAL_FINE_Msk & ((value) << SYSCTRL_DFLLVAL_FINE_Pos))
#define SYSCTRL_DFLLVAL_COARSE_Pos  10           /**< \brief (SYSCTRL_DFLLVAL) Coarse Value */
#define SYSCTRL_DFLLVAL_COARSE_Msk  (0x3Ful << SYSCTRL_DFLLVAL_COARSE_Pos)
#define SYSCTRL_DFLLVAL_COARSE(value) (SYSCTRL_DFLLVAL_COARSE_Msk & ((value) << SYSCTRL_DFLLVAL_COARSE_Pos))
#define SYSCTRL_DFLLVAL_DIFF_Pos    16           /**< \brief (SYSCTRL_DFLLVAL) Multiplication Ratio Difference */
#define SYSCTRL_DFLLVAL_DIFF_Msk    (0xFFFFul << SYSCTRL_DFLLVAL_DIFF_Pos)
#define SYSCTRL_DFLLVAL_DIFF(value) (SYSCTRL_DFLLVAL_DIFF_Msk & ((value) << SYSCTRL_DFLLVAL_DIFF_Pos))
#define SYSCTRL_DFLLVAL_MASK        0xFFFFFFFFul /**< \brief (SYSCTRL_DFLLVAL) MASK Register */

/* -------- SYSCTRL_DFLLMUL : (SYSCTRL Offset: 0x2C) (R/W 32) DFLL48M Multiplier -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t MUL:16;           /*!< bit:  0..15  DFLL Multiply Factor               */
    uint32_t FSTEP:10;         /*!< bit: 16..25  Fine Maximum Step                  */
    uint32_t CSTEP:6;          /*!< bit: 26..31  Coarse Maximum Step                */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SYSCTRL_DFLLMUL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_DFLLMUL_OFFSET      0x2C         /**< \brief (SYSCTRL_DFLLMUL offset) DFLL48M Multiplier */
#define SYSCTRL_DFLLMUL_RESETVALUE  0x00000000ul /**< \brief (SYSCTRL_DFLLMUL reset_value) DFLL48M Multiplier */

#define SYSCTRL_DFLLMUL_MUL_Pos     0            /**< \brief (SYSCTRL_DFLLMUL) DFLL Multiply Factor */
#define SYSCTRL_DFLLMUL_MUL_Msk     (0xFFFFul << SYSCTRL_DFLLMUL_MUL_Pos)
#define SYSCTRL_DFLLMUL_MUL(value)  (SYSCTRL_DFLLMUL_MUL_Msk & ((value) << SYSCTRL_DFLLMUL_MUL_Pos))
#define SYSCTRL_DFLLMUL_FSTEP_Pos   16           /**< \brief (SYSCTRL_DFLLMUL) Fine Maximum Step */
#define SYSCTRL_DFLLMUL_FSTEP_Msk   (0x3FFul << SYSCTRL_DFLLMUL_FSTEP_Pos)
#define SYSCTRL_DFLLMUL_FSTEP(value) (SYSCTRL_DFLLMUL_FSTEP_Msk & ((value) << SYSCTRL_DFLLMUL_FSTEP_Pos))
#define SYSCTRL_DFLLMUL_CSTEP_Pos   26           /**< \brief (SYSCTRL_DFLLMUL) Coarse Maximum Step */
#define SYSCTRL_DFLLMUL_CSTEP_Msk   (0x3Ful << SYSCTRL_DFLLMUL_CSTEP_Pos)
#define SYSCTRL_DFLLMUL_CSTEP(value) (SYSCTRL_DFLLMUL_CSTEP_Msk & ((value) << SYSCTRL_DFLLMUL_CSTEP_Pos))
#define SYSCTRL_DFLLMUL_MASK        0xFFFFFFFFul /**< \brief (SYSCTRL_DFLLMUL) MASK Register */

/* -------- SYSCTRL_DFLLSYNC : (SYSCTRL Offset: 0x30) (R/W  8) DFLL48M Synchronization -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :7;               /*!< bit:  0.. 6  Reserved                           */
    uint8_t  READREQ:1;        /*!< bit:      7  Read Request                       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} SYSCTRL_DFLLSYNC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_DFLLSYNC_OFFSET     0x30         /**< \brief (SYSCTRL_DFLLSYNC offset) DFLL48M Synchronization */
#define SYSCTRL_DFLLSYNC_RESETVALUE 0x00ul       /**< \brief (SYSCTRL_DFLLSYNC reset_value) DFLL48M Synchronization */

#define SYSCTRL_DFLLSYNC_READREQ_Pos 7            /**< \brief (SYSCTRL_DFLLSYNC) Read Request */
#define SYSCTRL_DFLLSYNC_READREQ    (0x1ul << SYSCTRL_DFLLSYNC_READREQ_Pos)
#define SYSCTRL_DFLLSYNC_MASK       0x80ul       /**< \brief (SYSCTRL_DFLLSYNC) MASK Register */

/* -------- SYSCTRL_BOD33 : (SYSCTRL Offset: 0x34) (R/W 32) 3.3V Brown-Out Detector (BOD33) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t ENABLE:1;         /*!< bit:      1  Enable                             */
    uint32_t HYST:1;           /*!< bit:      2  Hysteresis                         */
    uint32_t ACTION:2;         /*!< bit:  3.. 4  BOD33 Action                       */
    uint32_t :1;               /*!< bit:      5  Reserved                           */
    uint32_t RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint32_t :1;               /*!< bit:      7  Reserved                           */
    uint32_t MODE:1;           /*!< bit:      8  Operation Mode                     */
    uint32_t CEN:1;            /*!< bit:      9  Clock Enable                       */
    uint32_t :2;               /*!< bit: 10..11  Reserved                           */
    uint32_t PSEL:4;           /*!< bit: 12..15  Prescaler Select                   */
    uint32_t LEVEL:6;          /*!< bit: 16..21  BOD33 Threshold Level              */
    uint32_t :10;              /*!< bit: 22..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SYSCTRL_BOD33_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_BOD33_OFFSET        0x34         /**< \brief (SYSCTRL_BOD33 offset) 3.3V Brown-Out Detector (BOD33) Control */
#define SYSCTRL_BOD33_RESETVALUE    0x00000000ul /**< \brief (SYSCTRL_BOD33 reset_value) 3.3V Brown-Out Detector (BOD33) Control */

#define SYSCTRL_BOD33_ENABLE_Pos    1            /**< \brief (SYSCTRL_BOD33) Enable */
#define SYSCTRL_BOD33_ENABLE        (0x1ul << SYSCTRL_BOD33_ENABLE_Pos)
#define SYSCTRL_BOD33_HYST_Pos      2            /**< \brief (SYSCTRL_BOD33) Hysteresis */
#define SYSCTRL_BOD33_HYST          (0x1ul << SYSCTRL_BOD33_HYST_Pos)
#define SYSCTRL_BOD33_ACTION_Pos    3            /**< \brief (SYSCTRL_BOD33) BOD33 Action */
#define SYSCTRL_BOD33_ACTION_Msk    (0x3ul << SYSCTRL_BOD33_ACTION_Pos)
#define SYSCTRL_BOD33_ACTION(value) (SYSCTRL_BOD33_ACTION_Msk & ((value) << SYSCTRL_BOD33_ACTION_Pos))
#define   SYSCTRL_BOD33_ACTION_NONE_Val   0x0ul  /**< \brief (SYSCTRL_BOD33) No action */
#define   SYSCTRL_BOD33_ACTION_RESET_Val  0x1ul  /**< \brief (SYSCTRL_BOD33) The BOD33 generates a reset */
#define   SYSCTRL_BOD33_ACTION_INTERRUPT_Val 0x2ul  /**< \brief (SYSCTRL_BOD33) The BOD33 generates an interrupt */
#define SYSCTRL_BOD33_ACTION_NONE   (SYSCTRL_BOD33_ACTION_NONE_Val << SYSCTRL_BOD33_ACTION_Pos)
#define SYSCTRL_BOD33_ACTION_RESET  (SYSCTRL_BOD33_ACTION_RESET_Val << SYSCTRL_BOD33_ACTION_Pos)
#define SYSCTRL_BOD33_ACTION_INTERRUPT (SYSCTRL_BOD33_ACTION_INTERRUPT_Val << SYSCTRL_BOD33_ACTION_Pos)
#define SYSCTRL_BOD33_RUNSTDBY_Pos  6            /**< \brief (SYSCTRL_BOD33) Run in Standby */
#define SYSCTRL_BOD33_RUNSTDBY      (0x1ul << SYSCTRL_BOD33_RUNSTDBY_Pos)
#define SYSCTRL_BOD33_MODE_Pos      8            /**< \brief (SYSCTRL_BOD33) Operation Mode */
#define SYSCTRL_BOD33_MODE          (0x1ul << SYSCTRL_BOD33_MODE_Pos)
#define SYSCTRL_BOD33_CEN_Pos       9            /**< \brief (SYSCTRL_BOD33) Clock Enable */
#define SYSCTRL_BOD33_CEN           (0x1ul << SYSCTRL_BOD33_CEN_Pos)
#define SYSCTRL_BOD33_PSEL_Pos      12           /**< \brief (SYSCTRL_BOD33) Prescaler Select */
#define SYSCTRL_BOD33_PSEL_Msk      (0xFul << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_PSEL(value)   (SYSCTRL_BOD33_PSEL_Msk & ((value) << SYSCTRL_BOD33_PSEL_Pos))
#define   SYSCTRL_BOD33_PSEL_DIV2_Val     0x0ul  /**< \brief (SYSCTRL_BOD33) Divide clock by 2 */
#define   SYSCTRL_BOD33_PSEL_DIV4_Val     0x1ul  /**< \brief (SYSCTRL_BOD33) Divide clock by 4 */
#define   SYSCTRL_BOD33_PSEL_DIV8_Val     0x2ul  /**< \brief (SYSCTRL_BOD33) Divide clock by 8 */
#define   SYSCTRL_BOD33_PSEL_DIV16_Val    0x3ul  /**< \brief (SYSCTRL_BOD33) Divide clock by 16 */
#define   SYSCTRL_BOD33_PSEL_DIV32_Val    0x4ul  /**< \brief (SYSCTRL_BOD33) Divide clock by 32 */
#define   SYSCTRL_BOD33_PSEL_DIV64_Val    0x5ul  /**< \brief (SYSCTRL_BOD33) Divide clock by 64 */
#define   SYSCTRL_BOD33_PSEL_DIV128_Val   0x6ul  /**< \brief (SYSCTRL_BOD33) Divide clock by 128 */
#define   SYSCTRL_BOD33_PSEL_DIV256_Val   0x7ul  /**< \brief (SYSCTRL_BOD33) Divide clock by 256 */
#define   SYSCTRL_BOD33_PSEL_DIV512_Val   0x8ul  /**< \brief (SYSCTRL_BOD33) Divide clock by 512 */
#define   SYSCTRL_BOD33_PSEL_DIV1K_Val    0x9ul  /**< \brief (SYSCTRL_BOD33) Divide clock by 1024 */
#define   SYSCTRL_BOD33_PSEL_DIV2K_Val    0xAul  /**< \brief (SYSCTRL_BOD33) Divide clock by 2048 */
#define   SYSCTRL_BOD33_PSEL_DIV4K_Val    0xBul  /**< \brief (SYSCTRL_BOD33) Divide clock by 4096 */
#define   SYSCTRL_BOD33_PSEL_DIV8K_Val    0xCul  /**< \brief (SYSCTRL_BOD33) Divide clock by 8192 */
#define   SYSCTRL_BOD33_PSEL_DIV16K_Val   0xDul  /**< \brief (SYSCTRL_BOD33) Divide clock by 16384 */
#define   SYSCTRL_BOD33_PSEL_DIV32K_Val   0xEul  /**< \brief (SYSCTRL_BOD33) Divide clock by 32768 */
#define   SYSCTRL_BOD33_PSEL_DIV64K_Val   0xFul  /**< \brief (SYSCTRL_BOD33) Divide clock by 65536 */
#define SYSCTRL_BOD33_PSEL_DIV2     (SYSCTRL_BOD33_PSEL_DIV2_Val   << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_PSEL_DIV4     (SYSCTRL_BOD33_PSEL_DIV4_Val   << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_PSEL_DIV8     (SYSCTRL_BOD33_PSEL_DIV8_Val   << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_PSEL_DIV16    (SYSCTRL_BOD33_PSEL_DIV16_Val  << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_PSEL_DIV32    (SYSCTRL_BOD33_PSEL_DIV32_Val  << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_PSEL_DIV64    (SYSCTRL_BOD33_PSEL_DIV64_Val  << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_PSEL_DIV128   (SYSCTRL_BOD33_PSEL_DIV128_Val << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_PSEL_DIV256   (SYSCTRL_BOD33_PSEL_DIV256_Val << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_PSEL_DIV512   (SYSCTRL_BOD33_PSEL_DIV512_Val << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_PSEL_DIV1K    (SYSCTRL_BOD33_PSEL_DIV1K_Val  << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_PSEL_DIV2K    (SYSCTRL_BOD33_PSEL_DIV2K_Val  << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_PSEL_DIV4K    (SYSCTRL_BOD33_PSEL_DIV4K_Val  << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_PSEL_DIV8K    (SYSCTRL_BOD33_PSEL_DIV8K_Val  << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_PSEL_DIV16K   (SYSCTRL_BOD33_PSEL_DIV16K_Val << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_PSEL_DIV32K   (SYSCTRL_BOD33_PSEL_DIV32K_Val << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_PSEL_DIV64K   (SYSCTRL_BOD33_PSEL_DIV64K_Val << SYSCTRL_BOD33_PSEL_Pos)
#define SYSCTRL_BOD33_LEVEL_Pos     16           /**< \brief (SYSCTRL_BOD33) BOD33 Threshold Level */
#define SYSCTRL_BOD33_LEVEL_Msk     (0x3Ful << SYSCTRL_BOD33_LEVEL_Pos)
#define SYSCTRL_BOD33_LEVEL(value)  (SYSCTRL_BOD33_LEVEL_Msk & ((value) << SYSCTRL_BOD33_LEVEL_Pos))
#define SYSCTRL_BOD33_MASK          0x003FF35Eul /**< \brief (SYSCTRL_BOD33) MASK Register */

/* -------- SYSCTRL_VREG : (SYSCTRL Offset: 0x3C) (R/W 16) Voltage Regulator System (VREG) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t :6;               /*!< bit:  0.. 5  Reserved                           */
    uint16_t RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint16_t :6;               /*!< bit:  7..12  Reserved                           */
    uint16_t FORCELDO:1;       /*!< bit:     13  Force LDO Voltage Regulator        */
    uint16_t :2;               /*!< bit: 14..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} SYSCTRL_VREG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_VREG_OFFSET         0x3C         /**< \brief (SYSCTRL_VREG offset) Voltage Regulator System (VREG) Control */
#define SYSCTRL_VREG_RESETVALUE     0x0000ul     /**< \brief (SYSCTRL_VREG reset_value) Voltage Regulator System (VREG) Control */

#define SYSCTRL_VREG_RUNSTDBY_Pos   6            /**< \brief (SYSCTRL_VREG) Run in Standby */
#define SYSCTRL_VREG_RUNSTDBY       (0x1ul << SYSCTRL_VREG_RUNSTDBY_Pos)
#define SYSCTRL_VREG_FORCELDO_Pos   13           /**< \brief (SYSCTRL_VREG) Force LDO Voltage Regulator */
#define SYSCTRL_VREG_FORCELDO       (0x1ul << SYSCTRL_VREG_FORCELDO_Pos)
#define SYSCTRL_VREG_MASK           0x2040ul     /**< \brief (SYSCTRL_VREG) MASK Register */

/* -------- SYSCTRL_VREF : (SYSCTRL Offset: 0x40) (R/W 32) Voltage References System (VREF) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :1;               /*!< bit:      0  Reserved                           */
    uint32_t TSEN:1;           /*!< bit:      1  Temperature Sensor Enable          */
    uint32_t BGOUTEN:1;        /*!< bit:      2  Bandgap Output Enable              */
    uint32_t :13;              /*!< bit:  3..15  Reserved                           */
    uint32_t CALIB:11;         /*!< bit: 16..26  Bandgap Voltage Generator Calibration */
    uint32_t :5;               /*!< bit: 27..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SYSCTRL_VREF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_VREF_OFFSET         0x40         /**< \brief (SYSCTRL_VREF offset) Voltage References System (VREF) Control */
#define SYSCTRL_VREF_RESETVALUE     0x00000000ul /**< \brief (SYSCTRL_VREF reset_value) Voltage References System (VREF) Control */

#define SYSCTRL_VREF_TSEN_Pos       1            /**< \brief (SYSCTRL_VREF) Temperature Sensor Enable */
#define SYSCTRL_VREF_TSEN           (0x1ul << SYSCTRL_VREF_TSEN_Pos)
#define SYSCTRL_VREF_BGOUTEN_Pos    2            /**< \brief (SYSCTRL_VREF) Bandgap Output Enable */
#define SYSCTRL_VREF_BGOUTEN        (0x1ul << SYSCTRL_VREF_BGOUTEN_Pos)
#define SYSCTRL_VREF_CALIB_Pos      16           /**< \brief (SYSCTRL_VREF) Bandgap Voltage Generator Calibration */
#define SYSCTRL_VREF_CALIB_Msk      (0x7FFul << SYSCTRL_VREF_CALIB_Pos)
#define SYSCTRL_VREF_CALIB(value)   (SYSCTRL_VREF_CALIB_Msk & ((value) << SYSCTRL_VREF_CALIB_Pos))
#define SYSCTRL_VREF_MASK           0x07FF0006ul /**< \brief (SYSCTRL_VREF) MASK Register */

/* -------- SYSCTRL_DPLLCTRLA : (SYSCTRL Offset: 0x44) (R/W  8) DPLL Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :1;               /*!< bit:      0  Reserved                           */
    uint8_t  ENABLE:1;         /*!< bit:      1  DPLL Enable                        */
    uint8_t  :4;               /*!< bit:  2.. 5  Reserved                           */
    uint8_t  RUNSTDBY:1;       /*!< bit:      6  Run in Standby                     */
    uint8_t  ONDEMAND:1;       /*!< bit:      7  On Demand Clock Activation         */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} SYSCTRL_DPLLCTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_DPLLCTRLA_OFFSET    0x44         /**< \brief (SYSCTRL_DPLLCTRLA offset) DPLL Control A */
#define SYSCTRL_DPLLCTRLA_RESETVALUE 0x80ul       /**< \brief (SYSCTRL_DPLLCTRLA reset_value) DPLL Control A */

#define SYSCTRL_DPLLCTRLA_ENABLE_Pos 1            /**< \brief (SYSCTRL_DPLLCTRLA) DPLL Enable */
#define SYSCTRL_DPLLCTRLA_ENABLE    (0x1ul << SYSCTRL_DPLLCTRLA_ENABLE_Pos)
#define SYSCTRL_DPLLCTRLA_RUNSTDBY_Pos 6            /**< \brief (SYSCTRL_DPLLCTRLA) Run in Standby */
#define SYSCTRL_DPLLCTRLA_RUNSTDBY  (0x1ul << SYSCTRL_DPLLCTRLA_RUNSTDBY_Pos)
#define SYSCTRL_DPLLCTRLA_ONDEMAND_Pos 7            /**< \brief (SYSCTRL_DPLLCTRLA) On Demand Clock Activation */
#define SYSCTRL_DPLLCTRLA_ONDEMAND  (0x1ul << SYSCTRL_DPLLCTRLA_ONDEMAND_Pos)
#define SYSCTRL_DPLLCTRLA_MASK      0xC2ul       /**< \brief (SYSCTRL_DPLLCTRLA) MASK Register */

/* -------- SYSCTRL_DPLLRATIO : (SYSCTRL Offset: 0x48) (R/W 32) DPLL Ratio Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t LDR:12;           /*!< bit:  0..11  Loop Divider Ratio                 */
    uint32_t :4;               /*!< bit: 12..15  Reserved                           */
    uint32_t LDRFRAC:4;        /*!< bit: 16..19  Loop Divider Ratio Fractional Part */
    uint32_t :12;              /*!< bit: 20..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SYSCTRL_DPLLRATIO_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_DPLLRATIO_OFFSET    0x48         /**< \brief (SYSCTRL_DPLLRATIO offset) DPLL Ratio Control */
#define SYSCTRL_DPLLRATIO_RESETVALUE 0x00000000ul /**< \brief (SYSCTRL_DPLLRATIO reset_value) DPLL Ratio Control */

#define SYSCTRL_DPLLRATIO_LDR_Pos   0            /**< \brief (SYSCTRL_DPLLRATIO) Loop Divider Ratio */
#define SYSCTRL_DPLLRATIO_LDR_Msk   (0xFFFul << SYSCTRL_DPLLRATIO_LDR_Pos)
#define SYSCTRL_DPLLRATIO_LDR(value) (SYSCTRL_DPLLRATIO_LDR_Msk & ((value) << SYSCTRL_DPLLRATIO_LDR_Pos))
#define SYSCTRL_DPLLRATIO_LDRFRAC_Pos 16           /**< \brief (SYSCTRL_DPLLRATIO) Loop Divider Ratio Fractional Part */
#define SYSCTRL_DPLLRATIO_LDRFRAC_Msk (0xFul << SYSCTRL_DPLLRATIO_LDRFRAC_Pos)
#define SYSCTRL_DPLLRATIO_LDRFRAC(value) (SYSCTRL_DPLLRATIO_LDRFRAC_Msk & ((value) << SYSCTRL_DPLLRATIO_LDRFRAC_Pos))
#define SYSCTRL_DPLLRATIO_MASK      0x000F0FFFul /**< \brief (SYSCTRL_DPLLRATIO) MASK Register */

/* -------- SYSCTRL_DPLLCTRLB : (SYSCTRL Offset: 0x4C) (R/W 32) DPLL Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t FILTER:2;         /*!< bit:  0.. 1  Proportional Integral Filter Selection */
    uint32_t LPEN:1;           /*!< bit:      2  Low-Power Enable                   */
    uint32_t WUF:1;            /*!< bit:      3  Wake Up Fast                       */
    uint32_t REFCLK:2;         /*!< bit:  4.. 5  Reference Clock Selection          */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t LTIME:3;          /*!< bit:  8..10  Lock Time                          */
    uint32_t :1;               /*!< bit:     11  Reserved                           */
    uint32_t LBYPASS:1;        /*!< bit:     12  Lock Bypass                        */
    uint32_t :3;               /*!< bit: 13..15  Reserved                           */
    uint32_t DIV:11;           /*!< bit: 16..26  Clock Divider                      */
    uint32_t :5;               /*!< bit: 27..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} SYSCTRL_DPLLCTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_DPLLCTRLB_OFFSET    0x4C         /**< \brief (SYSCTRL_DPLLCTRLB offset) DPLL Control B */
#define SYSCTRL_DPLLCTRLB_RESETVALUE 0x00000000ul /**< \brief (SYSCTRL_DPLLCTRLB reset_value) DPLL Control B */

#define SYSCTRL_DPLLCTRLB_FILTER_Pos 0            /**< \brief (SYSCTRL_DPLLCTRLB) Proportional Integral Filter Selection */
#define SYSCTRL_DPLLCTRLB_FILTER_Msk (0x3ul << SYSCTRL_DPLLCTRLB_FILTER_Pos)
#define SYSCTRL_DPLLCTRLB_FILTER(value) (SYSCTRL_DPLLCTRLB_FILTER_Msk & ((value) << SYSCTRL_DPLLCTRLB_FILTER_Pos))
#define   SYSCTRL_DPLLCTRLB_FILTER_DEFAULT_Val 0x0ul  /**< \brief (SYSCTRL_DPLLCTRLB) Default filter mode */
#define   SYSCTRL_DPLLCTRLB_FILTER_LBFILT_Val 0x1ul  /**< \brief (SYSCTRL_DPLLCTRLB) Low bandwidth filter */
#define   SYSCTRL_DPLLCTRLB_FILTER_HBFILT_Val 0x2ul  /**< \brief (SYSCTRL_DPLLCTRLB) High bandwidth filter */
#define   SYSCTRL_DPLLCTRLB_FILTER_HDFILT_Val 0x3ul  /**< \brief (SYSCTRL_DPLLCTRLB) High damping filter */
#define SYSCTRL_DPLLCTRLB_FILTER_DEFAULT (SYSCTRL_DPLLCTRLB_FILTER_DEFAULT_Val << SYSCTRL_DPLLCTRLB_FILTER_Pos)
#define SYSCTRL_DPLLCTRLB_FILTER_LBFILT (SYSCTRL_DPLLCTRLB_FILTER_LBFILT_Val << SYSCTRL_DPLLCTRLB_FILTER_Pos)
#define SYSCTRL_DPLLCTRLB_FILTER_HBFILT (SYSCTRL_DPLLCTRLB_FILTER_HBFILT_Val << SYSCTRL_DPLLCTRLB_FILTER_Pos)
#define SYSCTRL_DPLLCTRLB_FILTER_HDFILT (SYSCTRL_DPLLCTRLB_FILTER_HDFILT_Val << SYSCTRL_DPLLCTRLB_FILTER_Pos)
#define SYSCTRL_DPLLCTRLB_LPEN_Pos  2            /**< \brief (SYSCTRL_DPLLCTRLB) Low-Power Enable */
#define SYSCTRL_DPLLCTRLB_LPEN      (0x1ul << SYSCTRL_DPLLCTRLB_LPEN_Pos)
#define SYSCTRL_DPLLCTRLB_WUF_Pos   3            /**< \brief (SYSCTRL_DPLLCTRLB) Wake Up Fast */
#define SYSCTRL_DPLLCTRLB_WUF       (0x1ul << SYSCTRL_DPLLCTRLB_WUF_Pos)
#define SYSCTRL_DPLLCTRLB_REFCLK_Pos 4            /**< \brief (SYSCTRL_DPLLCTRLB) Reference Clock Selection */
#define SYSCTRL_DPLLCTRLB_REFCLK_Msk (0x3ul << SYSCTRL_DPLLCTRLB_REFCLK_Pos)
#define SYSCTRL_DPLLCTRLB_REFCLK(value) (SYSCTRL_DPLLCTRLB_REFCLK_Msk & ((value) << SYSCTRL_DPLLCTRLB_REFCLK_Pos))
#define   SYSCTRL_DPLLCTRLB_REFCLK_REF0_Val 0x0ul  /**< \brief (SYSCTRL_DPLLCTRLB) CLK_DPLL_REF0 clock reference */
#define   SYSCTRL_DPLLCTRLB_REFCLK_REF1_Val 0x1ul  /**< \brief (SYSCTRL_DPLLCTRLB) CLK_DPLL_REF1 clock reference */
#define   SYSCTRL_DPLLCTRLB_REFCLK_GCLK_Val 0x2ul  /**< \brief (SYSCTRL_DPLLCTRLB) GCLK_DPLL clock reference */
#define SYSCTRL_DPLLCTRLB_REFCLK_REF0 (SYSCTRL_DPLLCTRLB_REFCLK_REF0_Val << SYSCTRL_DPLLCTRLB_REFCLK_Pos)
#define SYSCTRL_DPLLCTRLB_REFCLK_REF1 (SYSCTRL_DPLLCTRLB_REFCLK_REF1_Val << SYSCTRL_DPLLCTRLB_REFCLK_Pos)
#define SYSCTRL_DPLLCTRLB_REFCLK_GCLK (SYSCTRL_DPLLCTRLB_REFCLK_GCLK_Val << SYSCTRL_DPLLCTRLB_REFCLK_Pos)
#define SYSCTRL_DPLLCTRLB_LTIME_Pos 8            /**< \brief (SYSCTRL_DPLLCTRLB) Lock Time */
#define SYSCTRL_DPLLCTRLB_LTIME_Msk (0x7ul << SYSCTRL_DPLLCTRLB_LTIME_Pos)
#define SYSCTRL_DPLLCTRLB_LTIME(value) (SYSCTRL_DPLLCTRLB_LTIME_Msk & ((value) << SYSCTRL_DPLLCTRLB_LTIME_Pos))
#define   SYSCTRL_DPLLCTRLB_LTIME_DEFAULT_Val 0x0ul  /**< \brief (SYSCTRL_DPLLCTRLB) No time-out */
#define   SYSCTRL_DPLLCTRLB_LTIME_8MS_Val 0x4ul  /**< \brief (SYSCTRL_DPLLCTRLB) Time-out if no lock within 8 ms */
#define   SYSCTRL_DPLLCTRLB_LTIME_9MS_Val 0x5ul  /**< \brief (SYSCTRL_DPLLCTRLB) Time-out if no lock within 9 ms */
#define   SYSCTRL_DPLLCTRLB_LTIME_10MS_Val 0x6ul  /**< \brief (SYSCTRL_DPLLCTRLB) Time-out if no lock within 10 ms */
#define   SYSCTRL_DPLLCTRLB_LTIME_11MS_Val 0x7ul  /**< \brief (SYSCTRL_DPLLCTRLB) Time-out if no lock within 11 ms */
#define SYSCTRL_DPLLCTRLB_LTIME_DEFAULT (SYSCTRL_DPLLCTRLB_LTIME_DEFAULT_Val << SYSCTRL_DPLLCTRLB_LTIME_Pos)
#define SYSCTRL_DPLLCTRLB_LTIME_8MS (SYSCTRL_DPLLCTRLB_LTIME_8MS_Val << SYSCTRL_DPLLCTRLB_LTIME_Pos)
#define SYSCTRL_DPLLCTRLB_LTIME_9MS (SYSCTRL_DPLLCTRLB_LTIME_9MS_Val << SYSCTRL_DPLLCTRLB_LTIME_Pos)
#define SYSCTRL_DPLLCTRLB_LTIME_10MS (SYSCTRL_DPLLCTRLB_LTIME_10MS_Val << SYSCTRL_DPLLCTRLB_LTIME_Pos)
#define SYSCTRL_DPLLCTRLB_LTIME_11MS (SYSCTRL_DPLLCTRLB_LTIME_11MS_Val << SYSCTRL_DPLLCTRLB_LTIME_Pos)
#define SYSCTRL_DPLLCTRLB_LBYPASS_Pos 12           /**< \brief (SYSCTRL_DPLLCTRLB) Lock Bypass */
#define SYSCTRL_DPLLCTRLB_LBYPASS   (0x1ul << SYSCTRL_DPLLCTRLB_LBYPASS_Pos)
#define SYSCTRL_DPLLCTRLB_DIV_Pos   16           /**< \brief (SYSCTRL_DPLLCTRLB) Clock Divider */
#define SYSCTRL_DPLLCTRLB_DIV_Msk   (0x7FFul << SYSCTRL_DPLLCTRLB_DIV_Pos)
#define SYSCTRL_DPLLCTRLB_DIV(value) (SYSCTRL_DPLLCTRLB_DIV_Msk & ((value) << SYSCTRL_DPLLCTRLB_DIV_Pos))
#define SYSCTRL_DPLLCTRLB_MASK      0x07FF173Ful /**< \brief (SYSCTRL_DPLLCTRLB) MASK Register */

/* -------- SYSCTRL_DPLLSTATUS : (SYSCTRL Offset: 0x50) (R/   8) DPLL Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  LOCK:1;           /*!< bit:      0  DPLL Lock Status                   */
    uint8_t  CLKRDY:1;         /*!< bit:      1  Output Clock Ready                 */
    uint8_t  ENABLE:1;         /*!< bit:      2  DPLL Enable                        */
    uint8_t  DIV:1;            /*!< bit:      3  Divider Enable                     */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} SYSCTRL_DPLLSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define SYSCTRL_DPLLSTATUS_OFFSET   0x50         /**< \brief (SYSCTRL_DPLLSTATUS offset) DPLL Status */
#define SYSCTRL_DPLLSTATUS_RESETVALUE 0x00ul       /**< \brief (SYSCTRL_DPLLSTATUS reset_value) DPLL Status */

#define SYSCTRL_DPLLSTATUS_LOCK_Pos 0            /**< \brief (SYSCTRL_DPLLSTATUS) DPLL Lock Status */
#define SYSCTRL_DPLLSTATUS_LOCK     (0x1ul << SYSCTRL_DPLLSTATUS_LOCK_Pos)
#define SYSCTRL_DPLLSTATUS_CLKRDY_Pos 1            /**< \brief (SYSCTRL_DPLLSTATUS) Output Clock Ready */
#define SYSCTRL_DPLLSTATUS_CLKRDY   (0x1ul << SYSCTRL_DPLLSTATUS_CLKRDY_Pos)
#define SYSCTRL_DPLLSTATUS_ENABLE_Pos 2            /**< \brief (SYSCTRL_DPLLSTATUS) DPLL Enable */
#define SYSCTRL_DPLLSTATUS_ENABLE   (0x1ul << SYSCTRL_DPLLSTATUS_ENABLE_Pos)
#define SYSCTRL_DPLLSTATUS_DIV_Pos  3            /**< \brief (SYSCTRL_DPLLSTATUS) Divider Enable */
#define SYSCTRL_DPLLSTATUS_DIV      (0x1ul << SYSCTRL_DPLLSTATUS_DIV_Pos)
#define SYSCTRL_DPLLSTATUS_MASK     0x0Ful       /**< \brief (SYSCTRL_DPLLSTATUS) MASK Register */

/** \brief SYSCTRL hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO SYSCTRL_INTENCLR_Type     INTENCLR;    /**< \brief Offset: 0x00 (R/W 32) Interrupt Enable Clear */
  __IO SYSCTRL_INTENSET_Type     INTENSET;    /**< \brief Offset: 0x04 (R/W 32) Interrupt Enable Set */
  __IO SYSCTRL_INTFLAG_Type      INTFLAG;     /**< \brief Offset: 0x08 (R/W 32) Interrupt Flag Status and Clear */
  __I  SYSCTRL_PCLKSR_Type       PCLKSR;      /**< \brief Offset: 0x0C (R/  32) Power and Clocks Status */
  __IO SYSCTRL_XOSC_Type         XOSC;        /**< \brief Offset: 0x10 (R/W 16) External Multipurpose Crystal Oscillator (XOSC) Control */
       RoReg8                    Reserved1[0x2];
  __IO SYSCTRL_XOSC32K_Type      XOSC32K;     /**< \brief Offset: 0x14 (R/W 16) 32kHz External Crystal Oscillator (XOSC32K) Control */
       RoReg8                    Reserved2[0x2];
  __IO SYSCTRL_OSC32K_Type       OSC32K;      /**< \brief Offset: 0x18 (R/W 32) 32kHz Internal Oscillator (OSC32K) Control */
  __IO SYSCTRL_OSCULP32K_Type    OSCULP32K;   /**< \brief Offset: 0x1C (R/W  8) 32kHz Ultra Low Power Internal Oscillator (OSCULP32K) Control */
       RoReg8                    Reserved3[0x3];
  __IO SYSCTRL_OSC8M_Type        OSC8M;       /**< \brief Offset: 0x20 (R/W 32) 8MHz Internal Oscillator (OSC8M) Control */
  __IO SYSCTRL_DFLLCTRL_Type     DFLLCTRL;    /**< \brief Offset: 0x24 (R/W 16) DFLL48M Control */
       RoReg8                    Reserved4[0x2];
  __IO SYSCTRL_DFLLVAL_Type      DFLLVAL;     /**< \brief Offset: 0x28 (R/W 32) DFLL48M Value */
  __IO SYSCTRL_DFLLMUL_Type      DFLLMUL;     /**< \brief Offset: 0x2C (R/W 32) DFLL48M Multiplier */
  __IO SYSCTRL_DFLLSYNC_Type     DFLLSYNC;    /**< \brief Offset: 0x30 (R/W  8) DFLL48M Synchronization */
       RoReg8                    Reserved5[0x3];
  __IO SYSCTRL_BOD33_Type        BOD33;       /**< \brief Offset: 0x34 (R/W 32) 3.3V Brown-Out Detector (BOD33) Control */
       RoReg8                    Reserved6[0x4];
  __IO SYSCTRL_VREG_Type         VREG;        /**< \brief Offset: 0x3C (R/W 16) Voltage Regulator System (VREG) Control */
       RoReg8                    Reserved7[0x2];
  __IO SYSCTRL_VREF_Type         VREF;        /**< \brief Offset: 0x40 (R/W 32) Voltage References System (VREF) Control */
  __IO SYSCTRL_DPLLCTRLA_Type    DPLLCTRLA;   /**< \brief Offset: 0x44 (R/W  8) DPLL Control A */
       RoReg8                    Reserved8[0x3];
  __IO SYSCTRL_DPLLRATIO_Type    DPLLRATIO;   /**< \brief Offset: 0x48 (R/W 32) DPLL Ratio Control */
  __IO SYSCTRL_DPLLCTRLB_Type    DPLLCTRLB;   /**< \brief Offset: 0x4C (R/W 32) DPLL Control B */
  __I  SYSCTRL_DPLLSTATUS_Type   DPLLSTATUS;  /**< \brief Offset: 0x50 (R/   8) DPLL Status */
} Sysctrl;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD21_SYSCTRL_COMPONENT_ */
