/**
 * \file
 *
 * \brief Instance description for TC4
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

#ifndef _SAMD21_TC4_INSTANCE_
#define _SAMD21_TC4_INSTANCE_

/* ========== Register definition for TC4 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC4_CTRLA              (0x42003000U) /**< \brief (TC4) Control A */
#define REG_TC4_READREQ            (0x42003002U) /**< \brief (TC4) Read Request */
#define REG_TC4_CTRLBCLR           (0x42003004U) /**< \brief (TC4) Control B Clear */
#define REG_TC4_CTRLBSET           (0x42003005U) /**< \brief (TC4) Control B Set */
#define REG_TC4_CTRLC              (0x42003006U) /**< \brief (TC4) Control C */
#define REG_TC4_DBGCTRL            (0x42003008U) /**< \brief (TC4) Debug Control */
#define REG_TC4_EVCTRL             (0x4200300AU) /**< \brief (TC4) Event Control */
#define REG_TC4_INTENCLR           (0x4200300CU) /**< \brief (TC4) Interrupt Enable Clear */
#define REG_TC4_INTENSET           (0x4200300DU) /**< \brief (TC4) Interrupt Enable Set */
#define REG_TC4_INTFLAG            (0x4200300EU) /**< \brief (TC4) Interrupt Flag Status and Clear */
#define REG_TC4_STATUS             (0x4200300FU) /**< \brief (TC4) Status */
#define REG_TC4_COUNT16_COUNT      (0x42003010U) /**< \brief (TC4) COUNT16 Counter Value */
#define REG_TC4_COUNT16_CC0        (0x42003018U) /**< \brief (TC4) COUNT16 Compare/Capture 0 */
#define REG_TC4_COUNT16_CC1        (0x4200301AU) /**< \brief (TC4) COUNT16 Compare/Capture 1 */
#define REG_TC4_COUNT32_COUNT      (0x42003010U) /**< \brief (TC4) COUNT32 Counter Value */
#define REG_TC4_COUNT32_CC0        (0x42003018U) /**< \brief (TC4) COUNT32 Compare/Capture 0 */
#define REG_TC4_COUNT32_CC1        (0x4200301CU) /**< \brief (TC4) COUNT32 Compare/Capture 1 */
#define REG_TC4_COUNT8_COUNT       (0x42003010U) /**< \brief (TC4) COUNT8 Counter Value */
#define REG_TC4_COUNT8_PER         (0x42003014U) /**< \brief (TC4) COUNT8 Period Value */
#define REG_TC4_COUNT8_CC0         (0x42003018U) /**< \brief (TC4) COUNT8 Compare/Capture 0 */
#define REG_TC4_COUNT8_CC1         (0x42003019U) /**< \brief (TC4) COUNT8 Compare/Capture 1 */
#else
#define REG_TC4_CTRLA              (*(RwReg16*)0x42003000U) /**< \brief (TC4) Control A */
#define REG_TC4_READREQ            (*(RwReg16*)0x42003002U) /**< \brief (TC4) Read Request */
#define REG_TC4_CTRLBCLR           (*(RwReg8 *)0x42003004U) /**< \brief (TC4) Control B Clear */
#define REG_TC4_CTRLBSET           (*(RwReg8 *)0x42003005U) /**< \brief (TC4) Control B Set */
#define REG_TC4_CTRLC              (*(RwReg8 *)0x42003006U) /**< \brief (TC4) Control C */
#define REG_TC4_DBGCTRL            (*(RwReg8 *)0x42003008U) /**< \brief (TC4) Debug Control */
#define REG_TC4_EVCTRL             (*(RwReg16*)0x4200300AU) /**< \brief (TC4) Event Control */
#define REG_TC4_INTENCLR           (*(RwReg8 *)0x4200300CU) /**< \brief (TC4) Interrupt Enable Clear */
#define REG_TC4_INTENSET           (*(RwReg8 *)0x4200300DU) /**< \brief (TC4) Interrupt Enable Set */
#define REG_TC4_INTFLAG            (*(RwReg8 *)0x4200300EU) /**< \brief (TC4) Interrupt Flag Status and Clear */
#define REG_TC4_STATUS             (*(RoReg8 *)0x4200300FU) /**< \brief (TC4) Status */
#define REG_TC4_COUNT16_COUNT      (*(RwReg16*)0x42003010U) /**< \brief (TC4) COUNT16 Counter Value */
#define REG_TC4_COUNT16_CC0        (*(RwReg16*)0x42003018U) /**< \brief (TC4) COUNT16 Compare/Capture 0 */
#define REG_TC4_COUNT16_CC1        (*(RwReg16*)0x4200301AU) /**< \brief (TC4) COUNT16 Compare/Capture 1 */
#define REG_TC4_COUNT32_COUNT      (*(RwReg  *)0x42003010U) /**< \brief (TC4) COUNT32 Counter Value */
#define REG_TC4_COUNT32_CC0        (*(RwReg  *)0x42003018U) /**< \brief (TC4) COUNT32 Compare/Capture 0 */
#define REG_TC4_COUNT32_CC1        (*(RwReg  *)0x4200301CU) /**< \brief (TC4) COUNT32 Compare/Capture 1 */
#define REG_TC4_COUNT8_COUNT       (*(RwReg8 *)0x42003010U) /**< \brief (TC4) COUNT8 Counter Value */
#define REG_TC4_COUNT8_PER         (*(RwReg8 *)0x42003014U) /**< \brief (TC4) COUNT8 Period Value */
#define REG_TC4_COUNT8_CC0         (*(RwReg8 *)0x42003018U) /**< \brief (TC4) COUNT8 Compare/Capture 0 */
#define REG_TC4_COUNT8_CC1         (*(RwReg8 *)0x42003019U) /**< \brief (TC4) COUNT8 Compare/Capture 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC4 peripheral ========== */
#define TC4_CC8_NUM                 2        // Number of 8-bit Counters
#define TC4_CC16_NUM                2        // Number of 16-bit Counters
#define TC4_CC32_NUM                2        // Number of 32-bit Counters
#define TC4_DITHERING_EXT           0        // Dithering feature implemented
#define TC4_DMAC_ID_MC_0            28
#define TC4_DMAC_ID_MC_1            29
#define TC4_DMAC_ID_MC_LSB          28
#define TC4_DMAC_ID_MC_MSB          29
#define TC4_DMAC_ID_MC_SIZE         2
#define TC4_DMAC_ID_OVF             27       // Indexes of DMA Overflow trigger
#define TC4_GCLK_ID                 28       // Index of Generic Clock
#define TC4_MASTER                  1       
#define TC4_OW_NUM                  2        // Number of Output Waveforms
#define TC4_PERIOD_EXT              0        // Period feature implemented
#define TC4_SHADOW_EXT              0        // Shadow feature implemented

#endif /* _SAMD21_TC4_INSTANCE_ */
