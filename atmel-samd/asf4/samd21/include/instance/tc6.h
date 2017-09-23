/**
 * \file
 *
 * \brief Instance description for TC6
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

#ifndef _SAMD21_TC6_INSTANCE_
#define _SAMD21_TC6_INSTANCE_

/* ========== Register definition for TC6 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC6_CTRLA              (0x42003800) /**< \brief (TC6) Control A */
#define REG_TC6_READREQ            (0x42003802) /**< \brief (TC6) Read Request */
#define REG_TC6_CTRLBCLR           (0x42003804) /**< \brief (TC6) Control B Clear */
#define REG_TC6_CTRLBSET           (0x42003805) /**< \brief (TC6) Control B Set */
#define REG_TC6_CTRLC              (0x42003806) /**< \brief (TC6) Control C */
#define REG_TC6_DBGCTRL            (0x42003808) /**< \brief (TC6) Debug Control */
#define REG_TC6_EVCTRL             (0x4200380A) /**< \brief (TC6) Event Control */
#define REG_TC6_INTENCLR           (0x4200380C) /**< \brief (TC6) Interrupt Enable Clear */
#define REG_TC6_INTENSET           (0x4200380D) /**< \brief (TC6) Interrupt Enable Set */
#define REG_TC6_INTFLAG            (0x4200380E) /**< \brief (TC6) Interrupt Flag Status and Clear */
#define REG_TC6_STATUS             (0x4200380F) /**< \brief (TC6) Status */
#define REG_TC6_COUNT16_COUNT      (0x42003810) /**< \brief (TC6) COUNT16 Counter Value */
#define REG_TC6_COUNT16_CC0        (0x42003818) /**< \brief (TC6) COUNT16 Compare/Capture 0 */
#define REG_TC6_COUNT16_CC1        (0x4200381A) /**< \brief (TC6) COUNT16 Compare/Capture 1 */
#define REG_TC6_COUNT32_COUNT      (0x42003810) /**< \brief (TC6) COUNT32 Counter Value */
#define REG_TC6_COUNT32_CC0        (0x42003818) /**< \brief (TC6) COUNT32 Compare/Capture 0 */
#define REG_TC6_COUNT32_CC1        (0x4200381C) /**< \brief (TC6) COUNT32 Compare/Capture 1 */
#define REG_TC6_COUNT8_COUNT       (0x42003810) /**< \brief (TC6) COUNT8 Counter Value */
#define REG_TC6_COUNT8_PER         (0x42003814) /**< \brief (TC6) COUNT8 Period Value */
#define REG_TC6_COUNT8_CC0         (0x42003818) /**< \brief (TC6) COUNT8 Compare/Capture 0 */
#define REG_TC6_COUNT8_CC1         (0x42003819) /**< \brief (TC6) COUNT8 Compare/Capture 1 */
#else
#define REG_TC6_CTRLA              (*(RwReg16*)0x42003800UL) /**< \brief (TC6) Control A */
#define REG_TC6_READREQ            (*(RwReg16*)0x42003802UL) /**< \brief (TC6) Read Request */
#define REG_TC6_CTRLBCLR           (*(RwReg8 *)0x42003804UL) /**< \brief (TC6) Control B Clear */
#define REG_TC6_CTRLBSET           (*(RwReg8 *)0x42003805UL) /**< \brief (TC6) Control B Set */
#define REG_TC6_CTRLC              (*(RwReg8 *)0x42003806UL) /**< \brief (TC6) Control C */
#define REG_TC6_DBGCTRL            (*(RwReg8 *)0x42003808UL) /**< \brief (TC6) Debug Control */
#define REG_TC6_EVCTRL             (*(RwReg16*)0x4200380AUL) /**< \brief (TC6) Event Control */
#define REG_TC6_INTENCLR           (*(RwReg8 *)0x4200380CUL) /**< \brief (TC6) Interrupt Enable Clear */
#define REG_TC6_INTENSET           (*(RwReg8 *)0x4200380DUL) /**< \brief (TC6) Interrupt Enable Set */
#define REG_TC6_INTFLAG            (*(RwReg8 *)0x4200380EUL) /**< \brief (TC6) Interrupt Flag Status and Clear */
#define REG_TC6_STATUS             (*(RoReg8 *)0x4200380FUL) /**< \brief (TC6) Status */
#define REG_TC6_COUNT16_COUNT      (*(RwReg16*)0x42003810UL) /**< \brief (TC6) COUNT16 Counter Value */
#define REG_TC6_COUNT16_CC0        (*(RwReg16*)0x42003818UL) /**< \brief (TC6) COUNT16 Compare/Capture 0 */
#define REG_TC6_COUNT16_CC1        (*(RwReg16*)0x4200381AUL) /**< \brief (TC6) COUNT16 Compare/Capture 1 */
#define REG_TC6_COUNT32_COUNT      (*(RwReg  *)0x42003810UL) /**< \brief (TC6) COUNT32 Counter Value */
#define REG_TC6_COUNT32_CC0        (*(RwReg  *)0x42003818UL) /**< \brief (TC6) COUNT32 Compare/Capture 0 */
#define REG_TC6_COUNT32_CC1        (*(RwReg  *)0x4200381CUL) /**< \brief (TC6) COUNT32 Compare/Capture 1 */
#define REG_TC6_COUNT8_COUNT       (*(RwReg8 *)0x42003810UL) /**< \brief (TC6) COUNT8 Counter Value */
#define REG_TC6_COUNT8_PER         (*(RwReg8 *)0x42003814UL) /**< \brief (TC6) COUNT8 Period Value */
#define REG_TC6_COUNT8_CC0         (*(RwReg8 *)0x42003818UL) /**< \brief (TC6) COUNT8 Compare/Capture 0 */
#define REG_TC6_COUNT8_CC1         (*(RwReg8 *)0x42003819UL) /**< \brief (TC6) COUNT8 Compare/Capture 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC6 peripheral ========== */
#define TC6_CC8_NUM                 2        // Number of 8-bit Counters
#define TC6_CC16_NUM                2        // Number of 16-bit Counters
#define TC6_CC32_NUM                2        // Number of 32-bit Counters
#define TC6_DITHERING_EXT           0        // Dithering feature implemented
#define TC6_DMAC_ID_MC_0            34
#define TC6_DMAC_ID_MC_1            35
#define TC6_DMAC_ID_MC_LSB          34
#define TC6_DMAC_ID_MC_MSB          35
#define TC6_DMAC_ID_MC_SIZE         2
#define TC6_DMAC_ID_OVF             33       // Indexes of DMA Overflow trigger
#define TC6_GCLK_ID                 29       // Index of Generic Clock
#define TC6_MASTER                  1       
#define TC6_OW_NUM                  2        // Number of Output Waveforms
#define TC6_PERIOD_EXT              0        // Period feature implemented
#define TC6_SHADOW_EXT              0        // Shadow feature implemented

#endif /* _SAMD21_TC6_INSTANCE_ */
