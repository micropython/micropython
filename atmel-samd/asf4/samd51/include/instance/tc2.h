/**
 * \file
 *
 * \brief Instance description for TC2
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

#ifndef _SAMD51_TC2_INSTANCE_
#define _SAMD51_TC2_INSTANCE_

/* ========== Register definition for TC2 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC2_CTRLA              (0x4101A000) /**< \brief (TC2) Control A */
#define REG_TC2_CTRLBCLR           (0x4101A004) /**< \brief (TC2) Control B Clear */
#define REG_TC2_CTRLBSET           (0x4101A005) /**< \brief (TC2) Control B Set */
#define REG_TC2_EVCTRL             (0x4101A006) /**< \brief (TC2) Event Control */
#define REG_TC2_INTENCLR           (0x4101A008) /**< \brief (TC2) Interrupt Enable Clear */
#define REG_TC2_INTENSET           (0x4101A009) /**< \brief (TC2) Interrupt Enable Set */
#define REG_TC2_INTFLAG            (0x4101A00A) /**< \brief (TC2) Interrupt Flag Status and Clear */
#define REG_TC2_STATUS             (0x4101A00B) /**< \brief (TC2) Status */
#define REG_TC2_WAVE               (0x4101A00C) /**< \brief (TC2) Waveform Generation Control */
#define REG_TC2_DRVCTRL            (0x4101A00D) /**< \brief (TC2) Control C */
#define REG_TC2_DBGCTRL            (0x4101A00F) /**< \brief (TC2) Debug Control */
#define REG_TC2_SYNCBUSY           (0x4101A010) /**< \brief (TC2) Synchronization Status */
#define REG_TC2_COUNT16_COUNT      (0x4101A014) /**< \brief (TC2) COUNT16 Count */
#define REG_TC2_COUNT16_CC0        (0x4101A01C) /**< \brief (TC2) COUNT16 Compare and Capture 0 */
#define REG_TC2_COUNT16_CC1        (0x4101A01E) /**< \brief (TC2) COUNT16 Compare and Capture 1 */
#define REG_TC2_COUNT16_CCBUF0     (0x4101A030) /**< \brief (TC2) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC2_COUNT16_CCBUF1     (0x4101A032) /**< \brief (TC2) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC2_COUNT32_COUNT      (0x4101A014) /**< \brief (TC2) COUNT32 Count */
#define REG_TC2_COUNT32_CC0        (0x4101A01C) /**< \brief (TC2) COUNT32 Compare and Capture 0 */
#define REG_TC2_COUNT32_CC1        (0x4101A020) /**< \brief (TC2) COUNT32 Compare and Capture 1 */
#define REG_TC2_COUNT32_CCBUF0     (0x4101A030) /**< \brief (TC2) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC2_COUNT32_CCBUF1     (0x4101A034) /**< \brief (TC2) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC2_COUNT8_COUNT       (0x4101A014) /**< \brief (TC2) COUNT8 Count */
#define REG_TC2_COUNT8_PER         (0x4101A01B) /**< \brief (TC2) COUNT8 Period */
#define REG_TC2_COUNT8_CC0         (0x4101A01C) /**< \brief (TC2) COUNT8 Compare and Capture 0 */
#define REG_TC2_COUNT8_CC1         (0x4101A01D) /**< \brief (TC2) COUNT8 Compare and Capture 1 */
#define REG_TC2_COUNT8_PERBUF      (0x4101A02F) /**< \brief (TC2) COUNT8 Period Buffer */
#define REG_TC2_COUNT8_CCBUF0      (0x4101A030) /**< \brief (TC2) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC2_COUNT8_CCBUF1      (0x4101A031) /**< \brief (TC2) COUNT8 Compare and Capture Buffer 1 */
#else
#define REG_TC2_CTRLA              (*(RwReg  *)0x4101A000UL) /**< \brief (TC2) Control A */
#define REG_TC2_CTRLBCLR           (*(RwReg8 *)0x4101A004UL) /**< \brief (TC2) Control B Clear */
#define REG_TC2_CTRLBSET           (*(RwReg8 *)0x4101A005UL) /**< \brief (TC2) Control B Set */
#define REG_TC2_EVCTRL             (*(RwReg16*)0x4101A006UL) /**< \brief (TC2) Event Control */
#define REG_TC2_INTENCLR           (*(RwReg8 *)0x4101A008UL) /**< \brief (TC2) Interrupt Enable Clear */
#define REG_TC2_INTENSET           (*(RwReg8 *)0x4101A009UL) /**< \brief (TC2) Interrupt Enable Set */
#define REG_TC2_INTFLAG            (*(RwReg8 *)0x4101A00AUL) /**< \brief (TC2) Interrupt Flag Status and Clear */
#define REG_TC2_STATUS             (*(RwReg8 *)0x4101A00BUL) /**< \brief (TC2) Status */
#define REG_TC2_WAVE               (*(RwReg8 *)0x4101A00CUL) /**< \brief (TC2) Waveform Generation Control */
#define REG_TC2_DRVCTRL            (*(RwReg8 *)0x4101A00DUL) /**< \brief (TC2) Control C */
#define REG_TC2_DBGCTRL            (*(RwReg8 *)0x4101A00FUL) /**< \brief (TC2) Debug Control */
#define REG_TC2_SYNCBUSY           (*(RoReg  *)0x4101A010UL) /**< \brief (TC2) Synchronization Status */
#define REG_TC2_COUNT16_COUNT      (*(RwReg16*)0x4101A014UL) /**< \brief (TC2) COUNT16 Count */
#define REG_TC2_COUNT16_CC0        (*(RwReg16*)0x4101A01CUL) /**< \brief (TC2) COUNT16 Compare and Capture 0 */
#define REG_TC2_COUNT16_CC1        (*(RwReg16*)0x4101A01EUL) /**< \brief (TC2) COUNT16 Compare and Capture 1 */
#define REG_TC2_COUNT16_CCBUF0     (*(RwReg16*)0x4101A030UL) /**< \brief (TC2) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC2_COUNT16_CCBUF1     (*(RwReg16*)0x4101A032UL) /**< \brief (TC2) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC2_COUNT32_COUNT      (*(RwReg  *)0x4101A014UL) /**< \brief (TC2) COUNT32 Count */
#define REG_TC2_COUNT32_CC0        (*(RwReg  *)0x4101A01CUL) /**< \brief (TC2) COUNT32 Compare and Capture 0 */
#define REG_TC2_COUNT32_CC1        (*(RwReg  *)0x4101A020UL) /**< \brief (TC2) COUNT32 Compare and Capture 1 */
#define REG_TC2_COUNT32_CCBUF0     (*(RwReg  *)0x4101A030UL) /**< \brief (TC2) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC2_COUNT32_CCBUF1     (*(RwReg  *)0x4101A034UL) /**< \brief (TC2) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC2_COUNT8_COUNT       (*(RwReg8 *)0x4101A014UL) /**< \brief (TC2) COUNT8 Count */
#define REG_TC2_COUNT8_PER         (*(RwReg8 *)0x4101A01BUL) /**< \brief (TC2) COUNT8 Period */
#define REG_TC2_COUNT8_CC0         (*(RwReg8 *)0x4101A01CUL) /**< \brief (TC2) COUNT8 Compare and Capture 0 */
#define REG_TC2_COUNT8_CC1         (*(RwReg8 *)0x4101A01DUL) /**< \brief (TC2) COUNT8 Compare and Capture 1 */
#define REG_TC2_COUNT8_PERBUF      (*(RwReg8 *)0x4101A02FUL) /**< \brief (TC2) COUNT8 Period Buffer */
#define REG_TC2_COUNT8_CCBUF0      (*(RwReg8 *)0x4101A030UL) /**< \brief (TC2) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC2_COUNT8_CCBUF1      (*(RwReg8 *)0x4101A031UL) /**< \brief (TC2) COUNT8 Compare and Capture Buffer 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC2 peripheral ========== */
#define TC2_CC_NUM                  2       
#define TC2_DMAC_ID_MC_0            51
#define TC2_DMAC_ID_MC_1            52
#define TC2_DMAC_ID_MC_LSB          51
#define TC2_DMAC_ID_MC_MSB          52
#define TC2_DMAC_ID_MC_SIZE         2
#define TC2_DMAC_ID_OVF             50       // Indexes of DMA Overflow trigger
#define TC2_EXT                     0        // Coding of implemented extended features (keep 0 value)
#define TC2_GCLK_ID                 26       // Index of Generic Clock
#define TC2_MASTER_SLAVE_MODE       1        // TC type 0 : NA, 1 : Master, 2 : Slave
#define TC2_OW_NUM                  2        // Number of Output Waveforms

#endif /* _SAMD51_TC2_INSTANCE_ */
