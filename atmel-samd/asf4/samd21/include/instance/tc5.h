/**
 * \file
 *
 * \brief Instance description for TC5
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

#ifndef _SAMD21_TC5_INSTANCE_
#define _SAMD21_TC5_INSTANCE_

/* ========== Register definition for TC5 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC5_CTRLA              (0x42003400) /**< \brief (TC5) Control A */
#define REG_TC5_READREQ            (0x42003402) /**< \brief (TC5) Read Request */
#define REG_TC5_CTRLBCLR           (0x42003404) /**< \brief (TC5) Control B Clear */
#define REG_TC5_CTRLBSET           (0x42003405) /**< \brief (TC5) Control B Set */
#define REG_TC5_CTRLC              (0x42003406) /**< \brief (TC5) Control C */
#define REG_TC5_DBGCTRL            (0x42003408) /**< \brief (TC5) Debug Control */
#define REG_TC5_EVCTRL             (0x4200340A) /**< \brief (TC5) Event Control */
#define REG_TC5_INTENCLR           (0x4200340C) /**< \brief (TC5) Interrupt Enable Clear */
#define REG_TC5_INTENSET           (0x4200340D) /**< \brief (TC5) Interrupt Enable Set */
#define REG_TC5_INTFLAG            (0x4200340E) /**< \brief (TC5) Interrupt Flag Status and Clear */
#define REG_TC5_STATUS             (0x4200340F) /**< \brief (TC5) Status */
#define REG_TC5_COUNT16_COUNT      (0x42003410) /**< \brief (TC5) COUNT16 Counter Value */
#define REG_TC5_COUNT16_CC0        (0x42003418) /**< \brief (TC5) COUNT16 Compare/Capture 0 */
#define REG_TC5_COUNT16_CC1        (0x4200341A) /**< \brief (TC5) COUNT16 Compare/Capture 1 */
#define REG_TC5_COUNT32_COUNT      (0x42003410) /**< \brief (TC5) COUNT32 Counter Value */
#define REG_TC5_COUNT32_CC0        (0x42003418) /**< \brief (TC5) COUNT32 Compare/Capture 0 */
#define REG_TC5_COUNT32_CC1        (0x4200341C) /**< \brief (TC5) COUNT32 Compare/Capture 1 */
#define REG_TC5_COUNT8_COUNT       (0x42003410) /**< \brief (TC5) COUNT8 Counter Value */
#define REG_TC5_COUNT8_PER         (0x42003414) /**< \brief (TC5) COUNT8 Period Value */
#define REG_TC5_COUNT8_CC0         (0x42003418) /**< \brief (TC5) COUNT8 Compare/Capture 0 */
#define REG_TC5_COUNT8_CC1         (0x42003419) /**< \brief (TC5) COUNT8 Compare/Capture 1 */
#else
#define REG_TC5_CTRLA              (*(RwReg16*)0x42003400UL) /**< \brief (TC5) Control A */
#define REG_TC5_READREQ            (*(RwReg16*)0x42003402UL) /**< \brief (TC5) Read Request */
#define REG_TC5_CTRLBCLR           (*(RwReg8 *)0x42003404UL) /**< \brief (TC5) Control B Clear */
#define REG_TC5_CTRLBSET           (*(RwReg8 *)0x42003405UL) /**< \brief (TC5) Control B Set */
#define REG_TC5_CTRLC              (*(RwReg8 *)0x42003406UL) /**< \brief (TC5) Control C */
#define REG_TC5_DBGCTRL            (*(RwReg8 *)0x42003408UL) /**< \brief (TC5) Debug Control */
#define REG_TC5_EVCTRL             (*(RwReg16*)0x4200340AUL) /**< \brief (TC5) Event Control */
#define REG_TC5_INTENCLR           (*(RwReg8 *)0x4200340CUL) /**< \brief (TC5) Interrupt Enable Clear */
#define REG_TC5_INTENSET           (*(RwReg8 *)0x4200340DUL) /**< \brief (TC5) Interrupt Enable Set */
#define REG_TC5_INTFLAG            (*(RwReg8 *)0x4200340EUL) /**< \brief (TC5) Interrupt Flag Status and Clear */
#define REG_TC5_STATUS             (*(RoReg8 *)0x4200340FUL) /**< \brief (TC5) Status */
#define REG_TC5_COUNT16_COUNT      (*(RwReg16*)0x42003410UL) /**< \brief (TC5) COUNT16 Counter Value */
#define REG_TC5_COUNT16_CC0        (*(RwReg16*)0x42003418UL) /**< \brief (TC5) COUNT16 Compare/Capture 0 */
#define REG_TC5_COUNT16_CC1        (*(RwReg16*)0x4200341AUL) /**< \brief (TC5) COUNT16 Compare/Capture 1 */
#define REG_TC5_COUNT32_COUNT      (*(RwReg  *)0x42003410UL) /**< \brief (TC5) COUNT32 Counter Value */
#define REG_TC5_COUNT32_CC0        (*(RwReg  *)0x42003418UL) /**< \brief (TC5) COUNT32 Compare/Capture 0 */
#define REG_TC5_COUNT32_CC1        (*(RwReg  *)0x4200341CUL) /**< \brief (TC5) COUNT32 Compare/Capture 1 */
#define REG_TC5_COUNT8_COUNT       (*(RwReg8 *)0x42003410UL) /**< \brief (TC5) COUNT8 Counter Value */
#define REG_TC5_COUNT8_PER         (*(RwReg8 *)0x42003414UL) /**< \brief (TC5) COUNT8 Period Value */
#define REG_TC5_COUNT8_CC0         (*(RwReg8 *)0x42003418UL) /**< \brief (TC5) COUNT8 Compare/Capture 0 */
#define REG_TC5_COUNT8_CC1         (*(RwReg8 *)0x42003419UL) /**< \brief (TC5) COUNT8 Compare/Capture 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC5 peripheral ========== */
#define TC5_CC8_NUM                 2        // Number of 8-bit Counters
#define TC5_CC16_NUM                2        // Number of 16-bit Counters
#define TC5_CC32_NUM                2        // Number of 32-bit Counters
#define TC5_DITHERING_EXT           0        // Dithering feature implemented
#define TC5_DMAC_ID_MC_0            31
#define TC5_DMAC_ID_MC_1            32
#define TC5_DMAC_ID_MC_LSB          31
#define TC5_DMAC_ID_MC_MSB          32
#define TC5_DMAC_ID_MC_SIZE         2
#define TC5_DMAC_ID_OVF             30       // Indexes of DMA Overflow trigger
#define TC5_GCLK_ID                 28       // Index of Generic Clock
#define TC5_MASTER                  0       
#define TC5_OW_NUM                  2        // Number of Output Waveforms
#define TC5_PERIOD_EXT              0        // Period feature implemented
#define TC5_SHADOW_EXT              0        // Shadow feature implemented

#endif /* _SAMD21_TC5_INSTANCE_ */
