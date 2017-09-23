/**
 * \file
 *
 * \brief Instance description for TC4
 *
 * Copyright (c) 2017 Atmel Corporation,
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

#ifndef _SAMD51_TC4_INSTANCE_
#define _SAMD51_TC4_INSTANCE_

/* ========== Register definition for TC4 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC4_CTRLA              (0x42001400) /**< \brief (TC4) Control A */
#define REG_TC4_CTRLBCLR           (0x42001404) /**< \brief (TC4) Control B Clear */
#define REG_TC4_CTRLBSET           (0x42001405) /**< \brief (TC4) Control B Set */
#define REG_TC4_EVCTRL             (0x42001406) /**< \brief (TC4) Event Control */
#define REG_TC4_INTENCLR           (0x42001408) /**< \brief (TC4) Interrupt Enable Clear */
#define REG_TC4_INTENSET           (0x42001409) /**< \brief (TC4) Interrupt Enable Set */
#define REG_TC4_INTFLAG            (0x4200140A) /**< \brief (TC4) Interrupt Flag Status and Clear */
#define REG_TC4_STATUS             (0x4200140B) /**< \brief (TC4) Status */
#define REG_TC4_WAVE               (0x4200140C) /**< \brief (TC4) Waveform Generation Control */
#define REG_TC4_DRVCTRL            (0x4200140D) /**< \brief (TC4) Control C */
#define REG_TC4_DBGCTRL            (0x4200140F) /**< \brief (TC4) Debug Control */
#define REG_TC4_SYNCBUSY           (0x42001410) /**< \brief (TC4) Synchronization Status */
#define REG_TC4_COUNT16_COUNT      (0x42001414) /**< \brief (TC4) COUNT16 Count */
#define REG_TC4_COUNT16_CC0        (0x4200141C) /**< \brief (TC4) COUNT16 Compare and Capture 0 */
#define REG_TC4_COUNT16_CC1        (0x4200141E) /**< \brief (TC4) COUNT16 Compare and Capture 1 */
#define REG_TC4_COUNT16_CCBUF0     (0x42001430) /**< \brief (TC4) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC4_COUNT16_CCBUF1     (0x42001432) /**< \brief (TC4) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC4_COUNT32_COUNT      (0x42001414) /**< \brief (TC4) COUNT32 Count */
#define REG_TC4_COUNT32_CC0        (0x4200141C) /**< \brief (TC4) COUNT32 Compare and Capture 0 */
#define REG_TC4_COUNT32_CC1        (0x42001420) /**< \brief (TC4) COUNT32 Compare and Capture 1 */
#define REG_TC4_COUNT32_CCBUF0     (0x42001430) /**< \brief (TC4) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC4_COUNT32_CCBUF1     (0x42001434) /**< \brief (TC4) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC4_COUNT8_COUNT       (0x42001414) /**< \brief (TC4) COUNT8 Count */
#define REG_TC4_COUNT8_PER         (0x4200141B) /**< \brief (TC4) COUNT8 Period */
#define REG_TC4_COUNT8_CC0         (0x4200141C) /**< \brief (TC4) COUNT8 Compare and Capture 0 */
#define REG_TC4_COUNT8_CC1         (0x4200141D) /**< \brief (TC4) COUNT8 Compare and Capture 1 */
#define REG_TC4_COUNT8_PERBUF      (0x4200142F) /**< \brief (TC4) COUNT8 Period Buffer */
#define REG_TC4_COUNT8_CCBUF0      (0x42001430) /**< \brief (TC4) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC4_COUNT8_CCBUF1      (0x42001431) /**< \brief (TC4) COUNT8 Compare and Capture Buffer 1 */
#else
#define REG_TC4_CTRLA              (*(RwReg  *)0x42001400UL) /**< \brief (TC4) Control A */
#define REG_TC4_CTRLBCLR           (*(RwReg8 *)0x42001404UL) /**< \brief (TC4) Control B Clear */
#define REG_TC4_CTRLBSET           (*(RwReg8 *)0x42001405UL) /**< \brief (TC4) Control B Set */
#define REG_TC4_EVCTRL             (*(RwReg16*)0x42001406UL) /**< \brief (TC4) Event Control */
#define REG_TC4_INTENCLR           (*(RwReg8 *)0x42001408UL) /**< \brief (TC4) Interrupt Enable Clear */
#define REG_TC4_INTENSET           (*(RwReg8 *)0x42001409UL) /**< \brief (TC4) Interrupt Enable Set */
#define REG_TC4_INTFLAG            (*(RwReg8 *)0x4200140AUL) /**< \brief (TC4) Interrupt Flag Status and Clear */
#define REG_TC4_STATUS             (*(RwReg8 *)0x4200140BUL) /**< \brief (TC4) Status */
#define REG_TC4_WAVE               (*(RwReg8 *)0x4200140CUL) /**< \brief (TC4) Waveform Generation Control */
#define REG_TC4_DRVCTRL            (*(RwReg8 *)0x4200140DUL) /**< \brief (TC4) Control C */
#define REG_TC4_DBGCTRL            (*(RwReg8 *)0x4200140FUL) /**< \brief (TC4) Debug Control */
#define REG_TC4_SYNCBUSY           (*(RoReg  *)0x42001410UL) /**< \brief (TC4) Synchronization Status */
#define REG_TC4_COUNT16_COUNT      (*(RwReg16*)0x42001414UL) /**< \brief (TC4) COUNT16 Count */
#define REG_TC4_COUNT16_CC0        (*(RwReg16*)0x4200141CUL) /**< \brief (TC4) COUNT16 Compare and Capture 0 */
#define REG_TC4_COUNT16_CC1        (*(RwReg16*)0x4200141EUL) /**< \brief (TC4) COUNT16 Compare and Capture 1 */
#define REG_TC4_COUNT16_CCBUF0     (*(RwReg16*)0x42001430UL) /**< \brief (TC4) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC4_COUNT16_CCBUF1     (*(RwReg16*)0x42001432UL) /**< \brief (TC4) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC4_COUNT32_COUNT      (*(RwReg  *)0x42001414UL) /**< \brief (TC4) COUNT32 Count */
#define REG_TC4_COUNT32_CC0        (*(RwReg  *)0x4200141CUL) /**< \brief (TC4) COUNT32 Compare and Capture 0 */
#define REG_TC4_COUNT32_CC1        (*(RwReg  *)0x42001420UL) /**< \brief (TC4) COUNT32 Compare and Capture 1 */
#define REG_TC4_COUNT32_CCBUF0     (*(RwReg  *)0x42001430UL) /**< \brief (TC4) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC4_COUNT32_CCBUF1     (*(RwReg  *)0x42001434UL) /**< \brief (TC4) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC4_COUNT8_COUNT       (*(RwReg8 *)0x42001414UL) /**< \brief (TC4) COUNT8 Count */
#define REG_TC4_COUNT8_PER         (*(RwReg8 *)0x4200141BUL) /**< \brief (TC4) COUNT8 Period */
#define REG_TC4_COUNT8_CC0         (*(RwReg8 *)0x4200141CUL) /**< \brief (TC4) COUNT8 Compare and Capture 0 */
#define REG_TC4_COUNT8_CC1         (*(RwReg8 *)0x4200141DUL) /**< \brief (TC4) COUNT8 Compare and Capture 1 */
#define REG_TC4_COUNT8_PERBUF      (*(RwReg8 *)0x4200142FUL) /**< \brief (TC4) COUNT8 Period Buffer */
#define REG_TC4_COUNT8_CCBUF0      (*(RwReg8 *)0x42001430UL) /**< \brief (TC4) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC4_COUNT8_CCBUF1      (*(RwReg8 *)0x42001431UL) /**< \brief (TC4) COUNT8 Compare and Capture Buffer 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC4 peripheral ========== */
#define TC4_CC_NUM                  2       
#define TC4_DMAC_ID_MC_0            57
#define TC4_DMAC_ID_MC_1            58
#define TC4_DMAC_ID_MC_LSB          57
#define TC4_DMAC_ID_MC_MSB          58
#define TC4_DMAC_ID_MC_SIZE         2
#define TC4_DMAC_ID_OVF             56       // Indexes of DMA Overflow trigger
#define TC4_EXT                     0        // Coding of implemented extended features (keep 0 value)
#define TC4_GCLK_ID                 30       // Index of Generic Clock
#define TC4_MASTER_SLAVE_MODE       1        // TC type 0 : NA, 1 : Master, 2 : Slave
#define TC4_OW_NUM                  2        // Number of Output Waveforms

#endif /* _SAMD51_TC4_INSTANCE_ */
