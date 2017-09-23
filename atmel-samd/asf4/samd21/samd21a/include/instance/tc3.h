/**
 * \file
 *
 * \brief Instance description for TC3
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

#ifndef _SAMD21_TC3_INSTANCE_
#define _SAMD21_TC3_INSTANCE_

/* ========== Register definition for TC3 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC3_CTRLA              (0x42002C00) /**< \brief (TC3) Control A */
#define REG_TC3_READREQ            (0x42002C02) /**< \brief (TC3) Read Request */
#define REG_TC3_CTRLBCLR           (0x42002C04) /**< \brief (TC3) Control B Clear */
#define REG_TC3_CTRLBSET           (0x42002C05) /**< \brief (TC3) Control B Set */
#define REG_TC3_CTRLC              (0x42002C06) /**< \brief (TC3) Control C */
#define REG_TC3_DBGCTRL            (0x42002C08) /**< \brief (TC3) Debug Control */
#define REG_TC3_EVCTRL             (0x42002C0A) /**< \brief (TC3) Event Control */
#define REG_TC3_INTENCLR           (0x42002C0C) /**< \brief (TC3) Interrupt Enable Clear */
#define REG_TC3_INTENSET           (0x42002C0D) /**< \brief (TC3) Interrupt Enable Set */
#define REG_TC3_INTFLAG            (0x42002C0E) /**< \brief (TC3) Interrupt Flag Status and Clear */
#define REG_TC3_STATUS             (0x42002C0F) /**< \brief (TC3) Status */
#define REG_TC3_COUNT16_COUNT      (0x42002C10) /**< \brief (TC3) COUNT16 Counter Value */
#define REG_TC3_COUNT16_CC0        (0x42002C18) /**< \brief (TC3) COUNT16 Compare/Capture 0 */
#define REG_TC3_COUNT16_CC1        (0x42002C1A) /**< \brief (TC3) COUNT16 Compare/Capture 1 */
#define REG_TC3_COUNT32_COUNT      (0x42002C10) /**< \brief (TC3) COUNT32 Counter Value */
#define REG_TC3_COUNT32_CC0        (0x42002C18) /**< \brief (TC3) COUNT32 Compare/Capture 0 */
#define REG_TC3_COUNT32_CC1        (0x42002C1C) /**< \brief (TC3) COUNT32 Compare/Capture 1 */
#define REG_TC3_COUNT8_COUNT       (0x42002C10) /**< \brief (TC3) COUNT8 Counter Value */
#define REG_TC3_COUNT8_PER         (0x42002C14) /**< \brief (TC3) COUNT8 Period Value */
#define REG_TC3_COUNT8_CC0         (0x42002C18) /**< \brief (TC3) COUNT8 Compare/Capture 0 */
#define REG_TC3_COUNT8_CC1         (0x42002C19) /**< \brief (TC3) COUNT8 Compare/Capture 1 */
#else
#define REG_TC3_CTRLA              (*(RwReg16*)0x42002C00UL) /**< \brief (TC3) Control A */
#define REG_TC3_READREQ            (*(RwReg16*)0x42002C02UL) /**< \brief (TC3) Read Request */
#define REG_TC3_CTRLBCLR           (*(RwReg8 *)0x42002C04UL) /**< \brief (TC3) Control B Clear */
#define REG_TC3_CTRLBSET           (*(RwReg8 *)0x42002C05UL) /**< \brief (TC3) Control B Set */
#define REG_TC3_CTRLC              (*(RwReg8 *)0x42002C06UL) /**< \brief (TC3) Control C */
#define REG_TC3_DBGCTRL            (*(RwReg8 *)0x42002C08UL) /**< \brief (TC3) Debug Control */
#define REG_TC3_EVCTRL             (*(RwReg16*)0x42002C0AUL) /**< \brief (TC3) Event Control */
#define REG_TC3_INTENCLR           (*(RwReg8 *)0x42002C0CUL) /**< \brief (TC3) Interrupt Enable Clear */
#define REG_TC3_INTENSET           (*(RwReg8 *)0x42002C0DUL) /**< \brief (TC3) Interrupt Enable Set */
#define REG_TC3_INTFLAG            (*(RwReg8 *)0x42002C0EUL) /**< \brief (TC3) Interrupt Flag Status and Clear */
#define REG_TC3_STATUS             (*(RoReg8 *)0x42002C0FUL) /**< \brief (TC3) Status */
#define REG_TC3_COUNT16_COUNT      (*(RwReg16*)0x42002C10UL) /**< \brief (TC3) COUNT16 Counter Value */
#define REG_TC3_COUNT16_CC0        (*(RwReg16*)0x42002C18UL) /**< \brief (TC3) COUNT16 Compare/Capture 0 */
#define REG_TC3_COUNT16_CC1        (*(RwReg16*)0x42002C1AUL) /**< \brief (TC3) COUNT16 Compare/Capture 1 */
#define REG_TC3_COUNT32_COUNT      (*(RwReg  *)0x42002C10UL) /**< \brief (TC3) COUNT32 Counter Value */
#define REG_TC3_COUNT32_CC0        (*(RwReg  *)0x42002C18UL) /**< \brief (TC3) COUNT32 Compare/Capture 0 */
#define REG_TC3_COUNT32_CC1        (*(RwReg  *)0x42002C1CUL) /**< \brief (TC3) COUNT32 Compare/Capture 1 */
#define REG_TC3_COUNT8_COUNT       (*(RwReg8 *)0x42002C10UL) /**< \brief (TC3) COUNT8 Counter Value */
#define REG_TC3_COUNT8_PER         (*(RwReg8 *)0x42002C14UL) /**< \brief (TC3) COUNT8 Period Value */
#define REG_TC3_COUNT8_CC0         (*(RwReg8 *)0x42002C18UL) /**< \brief (TC3) COUNT8 Compare/Capture 0 */
#define REG_TC3_COUNT8_CC1         (*(RwReg8 *)0x42002C19UL) /**< \brief (TC3) COUNT8 Compare/Capture 1 */
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
