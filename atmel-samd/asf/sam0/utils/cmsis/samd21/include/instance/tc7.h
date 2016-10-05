/**
 * \file
 *
 * \brief Instance description for TC7
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

#ifndef _SAMD21_TC7_INSTANCE_
#define _SAMD21_TC7_INSTANCE_

/* ========== Register definition for TC7 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC7_CTRLA              (0x42003C00U) /**< \brief (TC7) Control A */
#define REG_TC7_READREQ            (0x42003C02U) /**< \brief (TC7) Read Request */
#define REG_TC7_CTRLBCLR           (0x42003C04U) /**< \brief (TC7) Control B Clear */
#define REG_TC7_CTRLBSET           (0x42003C05U) /**< \brief (TC7) Control B Set */
#define REG_TC7_CTRLC              (0x42003C06U) /**< \brief (TC7) Control C */
#define REG_TC7_DBGCTRL            (0x42003C08U) /**< \brief (TC7) Debug Control */
#define REG_TC7_EVCTRL             (0x42003C0AU) /**< \brief (TC7) Event Control */
#define REG_TC7_INTENCLR           (0x42003C0CU) /**< \brief (TC7) Interrupt Enable Clear */
#define REG_TC7_INTENSET           (0x42003C0DU) /**< \brief (TC7) Interrupt Enable Set */
#define REG_TC7_INTFLAG            (0x42003C0EU) /**< \brief (TC7) Interrupt Flag Status and Clear */
#define REG_TC7_STATUS             (0x42003C0FU) /**< \brief (TC7) Status */
#define REG_TC7_COUNT16_COUNT      (0x42003C10U) /**< \brief (TC7) COUNT16 Counter Value */
#define REG_TC7_COUNT16_CC0        (0x42003C18U) /**< \brief (TC7) COUNT16 Compare/Capture 0 */
#define REG_TC7_COUNT16_CC1        (0x42003C1AU) /**< \brief (TC7) COUNT16 Compare/Capture 1 */
#define REG_TC7_COUNT32_COUNT      (0x42003C10U) /**< \brief (TC7) COUNT32 Counter Value */
#define REG_TC7_COUNT32_CC0        (0x42003C18U) /**< \brief (TC7) COUNT32 Compare/Capture 0 */
#define REG_TC7_COUNT32_CC1        (0x42003C1CU) /**< \brief (TC7) COUNT32 Compare/Capture 1 */
#define REG_TC7_COUNT8_COUNT       (0x42003C10U) /**< \brief (TC7) COUNT8 Counter Value */
#define REG_TC7_COUNT8_PER         (0x42003C14U) /**< \brief (TC7) COUNT8 Period Value */
#define REG_TC7_COUNT8_CC0         (0x42003C18U) /**< \brief (TC7) COUNT8 Compare/Capture 0 */
#define REG_TC7_COUNT8_CC1         (0x42003C19U) /**< \brief (TC7) COUNT8 Compare/Capture 1 */
#else
#define REG_TC7_CTRLA              (*(RwReg16*)0x42003C00U) /**< \brief (TC7) Control A */
#define REG_TC7_READREQ            (*(RwReg16*)0x42003C02U) /**< \brief (TC7) Read Request */
#define REG_TC7_CTRLBCLR           (*(RwReg8 *)0x42003C04U) /**< \brief (TC7) Control B Clear */
#define REG_TC7_CTRLBSET           (*(RwReg8 *)0x42003C05U) /**< \brief (TC7) Control B Set */
#define REG_TC7_CTRLC              (*(RwReg8 *)0x42003C06U) /**< \brief (TC7) Control C */
#define REG_TC7_DBGCTRL            (*(RwReg8 *)0x42003C08U) /**< \brief (TC7) Debug Control */
#define REG_TC7_EVCTRL             (*(RwReg16*)0x42003C0AU) /**< \brief (TC7) Event Control */
#define REG_TC7_INTENCLR           (*(RwReg8 *)0x42003C0CU) /**< \brief (TC7) Interrupt Enable Clear */
#define REG_TC7_INTENSET           (*(RwReg8 *)0x42003C0DU) /**< \brief (TC7) Interrupt Enable Set */
#define REG_TC7_INTFLAG            (*(RwReg8 *)0x42003C0EU) /**< \brief (TC7) Interrupt Flag Status and Clear */
#define REG_TC7_STATUS             (*(RoReg8 *)0x42003C0FU) /**< \brief (TC7) Status */
#define REG_TC7_COUNT16_COUNT      (*(RwReg16*)0x42003C10U) /**< \brief (TC7) COUNT16 Counter Value */
#define REG_TC7_COUNT16_CC0        (*(RwReg16*)0x42003C18U) /**< \brief (TC7) COUNT16 Compare/Capture 0 */
#define REG_TC7_COUNT16_CC1        (*(RwReg16*)0x42003C1AU) /**< \brief (TC7) COUNT16 Compare/Capture 1 */
#define REG_TC7_COUNT32_COUNT      (*(RwReg  *)0x42003C10U) /**< \brief (TC7) COUNT32 Counter Value */
#define REG_TC7_COUNT32_CC0        (*(RwReg  *)0x42003C18U) /**< \brief (TC7) COUNT32 Compare/Capture 0 */
#define REG_TC7_COUNT32_CC1        (*(RwReg  *)0x42003C1CU) /**< \brief (TC7) COUNT32 Compare/Capture 1 */
#define REG_TC7_COUNT8_COUNT       (*(RwReg8 *)0x42003C10U) /**< \brief (TC7) COUNT8 Counter Value */
#define REG_TC7_COUNT8_PER         (*(RwReg8 *)0x42003C14U) /**< \brief (TC7) COUNT8 Period Value */
#define REG_TC7_COUNT8_CC0         (*(RwReg8 *)0x42003C18U) /**< \brief (TC7) COUNT8 Compare/Capture 0 */
#define REG_TC7_COUNT8_CC1         (*(RwReg8 *)0x42003C19U) /**< \brief (TC7) COUNT8 Compare/Capture 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC7 peripheral ========== */
#define TC7_CC8_NUM                 2        // Number of 8-bit Counters
#define TC7_CC16_NUM                2        // Number of 16-bit Counters
#define TC7_CC32_NUM                2        // Number of 32-bit Counters
#define TC7_DITHERING_EXT           0        // Dithering feature implemented
#define TC7_DMAC_ID_MC_0            37
#define TC7_DMAC_ID_MC_1            38
#define TC7_DMAC_ID_MC_LSB          37
#define TC7_DMAC_ID_MC_MSB          38
#define TC7_DMAC_ID_MC_SIZE         2
#define TC7_DMAC_ID_OVF             36       // Indexes of DMA Overflow trigger
#define TC7_GCLK_ID                 29       // Index of Generic Clock
#define TC7_MASTER                  0       
#define TC7_OW_NUM                  2        // Number of Output Waveforms
#define TC7_PERIOD_EXT              0        // Period feature implemented
#define TC7_SHADOW_EXT              0        // Shadow feature implemented

#endif /* _SAMD21_TC7_INSTANCE_ */
