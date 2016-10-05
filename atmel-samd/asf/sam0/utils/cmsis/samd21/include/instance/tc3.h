/**
 * \file
 *
 * \brief Instance description for TC3
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

#ifndef _SAMD21_TC3_INSTANCE_
#define _SAMD21_TC3_INSTANCE_

/* ========== Register definition for TC3 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC3_CTRLA              (0x42002C00U) /**< \brief (TC3) Control A */
#define REG_TC3_READREQ            (0x42002C02U) /**< \brief (TC3) Read Request */
#define REG_TC3_CTRLBCLR           (0x42002C04U) /**< \brief (TC3) Control B Clear */
#define REG_TC3_CTRLBSET           (0x42002C05U) /**< \brief (TC3) Control B Set */
#define REG_TC3_CTRLC              (0x42002C06U) /**< \brief (TC3) Control C */
#define REG_TC3_DBGCTRL            (0x42002C08U) /**< \brief (TC3) Debug Control */
#define REG_TC3_EVCTRL             (0x42002C0AU) /**< \brief (TC3) Event Control */
#define REG_TC3_INTENCLR           (0x42002C0CU) /**< \brief (TC3) Interrupt Enable Clear */
#define REG_TC3_INTENSET           (0x42002C0DU) /**< \brief (TC3) Interrupt Enable Set */
#define REG_TC3_INTFLAG            (0x42002C0EU) /**< \brief (TC3) Interrupt Flag Status and Clear */
#define REG_TC3_STATUS             (0x42002C0FU) /**< \brief (TC3) Status */
#define REG_TC3_COUNT16_COUNT      (0x42002C10U) /**< \brief (TC3) COUNT16 Counter Value */
#define REG_TC3_COUNT16_CC0        (0x42002C18U) /**< \brief (TC3) COUNT16 Compare/Capture 0 */
#define REG_TC3_COUNT16_CC1        (0x42002C1AU) /**< \brief (TC3) COUNT16 Compare/Capture 1 */
#define REG_TC3_COUNT32_COUNT      (0x42002C10U) /**< \brief (TC3) COUNT32 Counter Value */
#define REG_TC3_COUNT32_CC0        (0x42002C18U) /**< \brief (TC3) COUNT32 Compare/Capture 0 */
#define REG_TC3_COUNT32_CC1        (0x42002C1CU) /**< \brief (TC3) COUNT32 Compare/Capture 1 */
#define REG_TC3_COUNT8_COUNT       (0x42002C10U) /**< \brief (TC3) COUNT8 Counter Value */
#define REG_TC3_COUNT8_PER         (0x42002C14U) /**< \brief (TC3) COUNT8 Period Value */
#define REG_TC3_COUNT8_CC0         (0x42002C18U) /**< \brief (TC3) COUNT8 Compare/Capture 0 */
#define REG_TC3_COUNT8_CC1         (0x42002C19U) /**< \brief (TC3) COUNT8 Compare/Capture 1 */
#else
#define REG_TC3_CTRLA              (*(RwReg16*)0x42002C00U) /**< \brief (TC3) Control A */
#define REG_TC3_READREQ            (*(RwReg16*)0x42002C02U) /**< \brief (TC3) Read Request */
#define REG_TC3_CTRLBCLR           (*(RwReg8 *)0x42002C04U) /**< \brief (TC3) Control B Clear */
#define REG_TC3_CTRLBSET           (*(RwReg8 *)0x42002C05U) /**< \brief (TC3) Control B Set */
#define REG_TC3_CTRLC              (*(RwReg8 *)0x42002C06U) /**< \brief (TC3) Control C */
#define REG_TC3_DBGCTRL            (*(RwReg8 *)0x42002C08U) /**< \brief (TC3) Debug Control */
#define REG_TC3_EVCTRL             (*(RwReg16*)0x42002C0AU) /**< \brief (TC3) Event Control */
#define REG_TC3_INTENCLR           (*(RwReg8 *)0x42002C0CU) /**< \brief (TC3) Interrupt Enable Clear */
#define REG_TC3_INTENSET           (*(RwReg8 *)0x42002C0DU) /**< \brief (TC3) Interrupt Enable Set */
#define REG_TC3_INTFLAG            (*(RwReg8 *)0x42002C0EU) /**< \brief (TC3) Interrupt Flag Status and Clear */
#define REG_TC3_STATUS             (*(RoReg8 *)0x42002C0FU) /**< \brief (TC3) Status */
#define REG_TC3_COUNT16_COUNT      (*(RwReg16*)0x42002C10U) /**< \brief (TC3) COUNT16 Counter Value */
#define REG_TC3_COUNT16_CC0        (*(RwReg16*)0x42002C18U) /**< \brief (TC3) COUNT16 Compare/Capture 0 */
#define REG_TC3_COUNT16_CC1        (*(RwReg16*)0x42002C1AU) /**< \brief (TC3) COUNT16 Compare/Capture 1 */
#define REG_TC3_COUNT32_COUNT      (*(RwReg  *)0x42002C10U) /**< \brief (TC3) COUNT32 Counter Value */
#define REG_TC3_COUNT32_CC0        (*(RwReg  *)0x42002C18U) /**< \brief (TC3) COUNT32 Compare/Capture 0 */
#define REG_TC3_COUNT32_CC1        (*(RwReg  *)0x42002C1CU) /**< \brief (TC3) COUNT32 Compare/Capture 1 */
#define REG_TC3_COUNT8_COUNT       (*(RwReg8 *)0x42002C10U) /**< \brief (TC3) COUNT8 Counter Value */
#define REG_TC3_COUNT8_PER         (*(RwReg8 *)0x42002C14U) /**< \brief (TC3) COUNT8 Period Value */
#define REG_TC3_COUNT8_CC0         (*(RwReg8 *)0x42002C18U) /**< \brief (TC3) COUNT8 Compare/Capture 0 */
#define REG_TC3_COUNT8_CC1         (*(RwReg8 *)0x42002C19U) /**< \brief (TC3) COUNT8 Compare/Capture 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC3 peripheral ========== */
#define TC3_CC8_NUM                 2        // Number of 8-bit Counters
#define TC3_CC16_NUM                2        // Number of 16-bit Counters
#define TC3_CC32_NUM                2        // Number of 32-bit Counters
#define TC3_DITHERING_EXT           0        // Dithering feature implemented
#define TC3_DMAC_ID_MC_0            25
#define TC3_DMAC_ID_MC_1            26
#define TC3_DMAC_ID_MC_LSB          25
#define TC3_DMAC_ID_MC_MSB          26
#define TC3_DMAC_ID_MC_SIZE         2
#define TC3_DMAC_ID_OVF             24       // Indexes of DMA Overflow trigger
#define TC3_GCLK_ID                 27       // Index of Generic Clock
#define TC3_MASTER                  0       
#define TC3_OW_NUM                  2        // Number of Output Waveforms
#define TC3_PERIOD_EXT              0        // Period feature implemented
#define TC3_SHADOW_EXT              0        // Shadow feature implemented

#endif /* _SAMD21_TC3_INSTANCE_ */
