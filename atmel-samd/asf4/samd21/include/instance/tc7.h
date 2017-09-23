/**
 * \file
 *
 * \brief Instance description for TC7
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

#ifndef _SAMD21_TC7_INSTANCE_
#define _SAMD21_TC7_INSTANCE_

/* ========== Register definition for TC7 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC7_CTRLA              (0x42003C00) /**< \brief (TC7) Control A */
#define REG_TC7_READREQ            (0x42003C02) /**< \brief (TC7) Read Request */
#define REG_TC7_CTRLBCLR           (0x42003C04) /**< \brief (TC7) Control B Clear */
#define REG_TC7_CTRLBSET           (0x42003C05) /**< \brief (TC7) Control B Set */
#define REG_TC7_CTRLC              (0x42003C06) /**< \brief (TC7) Control C */
#define REG_TC7_DBGCTRL            (0x42003C08) /**< \brief (TC7) Debug Control */
#define REG_TC7_EVCTRL             (0x42003C0A) /**< \brief (TC7) Event Control */
#define REG_TC7_INTENCLR           (0x42003C0C) /**< \brief (TC7) Interrupt Enable Clear */
#define REG_TC7_INTENSET           (0x42003C0D) /**< \brief (TC7) Interrupt Enable Set */
#define REG_TC7_INTFLAG            (0x42003C0E) /**< \brief (TC7) Interrupt Flag Status and Clear */
#define REG_TC7_STATUS             (0x42003C0F) /**< \brief (TC7) Status */
#define REG_TC7_COUNT16_COUNT      (0x42003C10) /**< \brief (TC7) COUNT16 Counter Value */
#define REG_TC7_COUNT16_CC0        (0x42003C18) /**< \brief (TC7) COUNT16 Compare/Capture 0 */
#define REG_TC7_COUNT16_CC1        (0x42003C1A) /**< \brief (TC7) COUNT16 Compare/Capture 1 */
#define REG_TC7_COUNT32_COUNT      (0x42003C10) /**< \brief (TC7) COUNT32 Counter Value */
#define REG_TC7_COUNT32_CC0        (0x42003C18) /**< \brief (TC7) COUNT32 Compare/Capture 0 */
#define REG_TC7_COUNT32_CC1        (0x42003C1C) /**< \brief (TC7) COUNT32 Compare/Capture 1 */
#define REG_TC7_COUNT8_COUNT       (0x42003C10) /**< \brief (TC7) COUNT8 Counter Value */
#define REG_TC7_COUNT8_PER         (0x42003C14) /**< \brief (TC7) COUNT8 Period Value */
#define REG_TC7_COUNT8_CC0         (0x42003C18) /**< \brief (TC7) COUNT8 Compare/Capture 0 */
#define REG_TC7_COUNT8_CC1         (0x42003C19) /**< \brief (TC7) COUNT8 Compare/Capture 1 */
#else
#define REG_TC7_CTRLA              (*(RwReg16*)0x42003C00UL) /**< \brief (TC7) Control A */
#define REG_TC7_READREQ            (*(RwReg16*)0x42003C02UL) /**< \brief (TC7) Read Request */
#define REG_TC7_CTRLBCLR           (*(RwReg8 *)0x42003C04UL) /**< \brief (TC7) Control B Clear */
#define REG_TC7_CTRLBSET           (*(RwReg8 *)0x42003C05UL) /**< \brief (TC7) Control B Set */
#define REG_TC7_CTRLC              (*(RwReg8 *)0x42003C06UL) /**< \brief (TC7) Control C */
#define REG_TC7_DBGCTRL            (*(RwReg8 *)0x42003C08UL) /**< \brief (TC7) Debug Control */
#define REG_TC7_EVCTRL             (*(RwReg16*)0x42003C0AUL) /**< \brief (TC7) Event Control */
#define REG_TC7_INTENCLR           (*(RwReg8 *)0x42003C0CUL) /**< \brief (TC7) Interrupt Enable Clear */
#define REG_TC7_INTENSET           (*(RwReg8 *)0x42003C0DUL) /**< \brief (TC7) Interrupt Enable Set */
#define REG_TC7_INTFLAG            (*(RwReg8 *)0x42003C0EUL) /**< \brief (TC7) Interrupt Flag Status and Clear */
#define REG_TC7_STATUS             (*(RoReg8 *)0x42003C0FUL) /**< \brief (TC7) Status */
#define REG_TC7_COUNT16_COUNT      (*(RwReg16*)0x42003C10UL) /**< \brief (TC7) COUNT16 Counter Value */
#define REG_TC7_COUNT16_CC0        (*(RwReg16*)0x42003C18UL) /**< \brief (TC7) COUNT16 Compare/Capture 0 */
#define REG_TC7_COUNT16_CC1        (*(RwReg16*)0x42003C1AUL) /**< \brief (TC7) COUNT16 Compare/Capture 1 */
#define REG_TC7_COUNT32_COUNT      (*(RwReg  *)0x42003C10UL) /**< \brief (TC7) COUNT32 Counter Value */
#define REG_TC7_COUNT32_CC0        (*(RwReg  *)0x42003C18UL) /**< \brief (TC7) COUNT32 Compare/Capture 0 */
#define REG_TC7_COUNT32_CC1        (*(RwReg  *)0x42003C1CUL) /**< \brief (TC7) COUNT32 Compare/Capture 1 */
#define REG_TC7_COUNT8_COUNT       (*(RwReg8 *)0x42003C10UL) /**< \brief (TC7) COUNT8 Counter Value */
#define REG_TC7_COUNT8_PER         (*(RwReg8 *)0x42003C14UL) /**< \brief (TC7) COUNT8 Period Value */
#define REG_TC7_COUNT8_CC0         (*(RwReg8 *)0x42003C18UL) /**< \brief (TC7) COUNT8 Compare/Capture 0 */
#define REG_TC7_COUNT8_CC1         (*(RwReg8 *)0x42003C19UL) /**< \brief (TC7) COUNT8 Compare/Capture 1 */
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
