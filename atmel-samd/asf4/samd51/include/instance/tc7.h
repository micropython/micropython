/**
 * \file
 *
 * \brief Instance description for TC7
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

#ifndef _SAMD51_TC7_INSTANCE_
#define _SAMD51_TC7_INSTANCE_

/* ========== Register definition for TC7 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC7_CTRLA              (0x43001800) /**< \brief (TC7) Control A */
#define REG_TC7_CTRLBCLR           (0x43001804) /**< \brief (TC7) Control B Clear */
#define REG_TC7_CTRLBSET           (0x43001805) /**< \brief (TC7) Control B Set */
#define REG_TC7_EVCTRL             (0x43001806) /**< \brief (TC7) Event Control */
#define REG_TC7_INTENCLR           (0x43001808) /**< \brief (TC7) Interrupt Enable Clear */
#define REG_TC7_INTENSET           (0x43001809) /**< \brief (TC7) Interrupt Enable Set */
#define REG_TC7_INTFLAG            (0x4300180A) /**< \brief (TC7) Interrupt Flag Status and Clear */
#define REG_TC7_STATUS             (0x4300180B) /**< \brief (TC7) Status */
#define REG_TC7_WAVE               (0x4300180C) /**< \brief (TC7) Waveform Generation Control */
#define REG_TC7_DRVCTRL            (0x4300180D) /**< \brief (TC7) Control C */
#define REG_TC7_DBGCTRL            (0x4300180F) /**< \brief (TC7) Debug Control */
#define REG_TC7_SYNCBUSY           (0x43001810) /**< \brief (TC7) Synchronization Status */
#define REG_TC7_COUNT16_COUNT      (0x43001814) /**< \brief (TC7) COUNT16 Count */
#define REG_TC7_COUNT16_CC0        (0x4300181C) /**< \brief (TC7) COUNT16 Compare and Capture 0 */
#define REG_TC7_COUNT16_CC1        (0x4300181E) /**< \brief (TC7) COUNT16 Compare and Capture 1 */
#define REG_TC7_COUNT16_CCBUF0     (0x43001830) /**< \brief (TC7) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC7_COUNT16_CCBUF1     (0x43001832) /**< \brief (TC7) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC7_COUNT32_COUNT      (0x43001814) /**< \brief (TC7) COUNT32 Count */
#define REG_TC7_COUNT32_CC0        (0x4300181C) /**< \brief (TC7) COUNT32 Compare and Capture 0 */
#define REG_TC7_COUNT32_CC1        (0x43001820) /**< \brief (TC7) COUNT32 Compare and Capture 1 */
#define REG_TC7_COUNT32_CCBUF0     (0x43001830) /**< \brief (TC7) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC7_COUNT32_CCBUF1     (0x43001834) /**< \brief (TC7) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC7_COUNT8_COUNT       (0x43001814) /**< \brief (TC7) COUNT8 Count */
#define REG_TC7_COUNT8_PER         (0x4300181B) /**< \brief (TC7) COUNT8 Period */
#define REG_TC7_COUNT8_CC0         (0x4300181C) /**< \brief (TC7) COUNT8 Compare and Capture 0 */
#define REG_TC7_COUNT8_CC1         (0x4300181D) /**< \brief (TC7) COUNT8 Compare and Capture 1 */
#define REG_TC7_COUNT8_PERBUF      (0x4300182F) /**< \brief (TC7) COUNT8 Period Buffer */
#define REG_TC7_COUNT8_CCBUF0      (0x43001830) /**< \brief (TC7) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC7_COUNT8_CCBUF1      (0x43001831) /**< \brief (TC7) COUNT8 Compare and Capture Buffer 1 */
#else
#define REG_TC7_CTRLA              (*(RwReg  *)0x43001800UL) /**< \brief (TC7) Control A */
#define REG_TC7_CTRLBCLR           (*(RwReg8 *)0x43001804UL) /**< \brief (TC7) Control B Clear */
#define REG_TC7_CTRLBSET           (*(RwReg8 *)0x43001805UL) /**< \brief (TC7) Control B Set */
#define REG_TC7_EVCTRL             (*(RwReg16*)0x43001806UL) /**< \brief (TC7) Event Control */
#define REG_TC7_INTENCLR           (*(RwReg8 *)0x43001808UL) /**< \brief (TC7) Interrupt Enable Clear */
#define REG_TC7_INTENSET           (*(RwReg8 *)0x43001809UL) /**< \brief (TC7) Interrupt Enable Set */
#define REG_TC7_INTFLAG            (*(RwReg8 *)0x4300180AUL) /**< \brief (TC7) Interrupt Flag Status and Clear */
#define REG_TC7_STATUS             (*(RwReg8 *)0x4300180BUL) /**< \brief (TC7) Status */
#define REG_TC7_WAVE               (*(RwReg8 *)0x4300180CUL) /**< \brief (TC7) Waveform Generation Control */
#define REG_TC7_DRVCTRL            (*(RwReg8 *)0x4300180DUL) /**< \brief (TC7) Control C */
#define REG_TC7_DBGCTRL            (*(RwReg8 *)0x4300180FUL) /**< \brief (TC7) Debug Control */
#define REG_TC7_SYNCBUSY           (*(RoReg  *)0x43001810UL) /**< \brief (TC7) Synchronization Status */
#define REG_TC7_COUNT16_COUNT      (*(RwReg16*)0x43001814UL) /**< \brief (TC7) COUNT16 Count */
#define REG_TC7_COUNT16_CC0        (*(RwReg16*)0x4300181CUL) /**< \brief (TC7) COUNT16 Compare and Capture 0 */
#define REG_TC7_COUNT16_CC1        (*(RwReg16*)0x4300181EUL) /**< \brief (TC7) COUNT16 Compare and Capture 1 */
#define REG_TC7_COUNT16_CCBUF0     (*(RwReg16*)0x43001830UL) /**< \brief (TC7) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC7_COUNT16_CCBUF1     (*(RwReg16*)0x43001832UL) /**< \brief (TC7) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC7_COUNT32_COUNT      (*(RwReg  *)0x43001814UL) /**< \brief (TC7) COUNT32 Count */
#define REG_TC7_COUNT32_CC0        (*(RwReg  *)0x4300181CUL) /**< \brief (TC7) COUNT32 Compare and Capture 0 */
#define REG_TC7_COUNT32_CC1        (*(RwReg  *)0x43001820UL) /**< \brief (TC7) COUNT32 Compare and Capture 1 */
#define REG_TC7_COUNT32_CCBUF0     (*(RwReg  *)0x43001830UL) /**< \brief (TC7) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC7_COUNT32_CCBUF1     (*(RwReg  *)0x43001834UL) /**< \brief (TC7) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC7_COUNT8_COUNT       (*(RwReg8 *)0x43001814UL) /**< \brief (TC7) COUNT8 Count */
#define REG_TC7_COUNT8_PER         (*(RwReg8 *)0x4300181BUL) /**< \brief (TC7) COUNT8 Period */
#define REG_TC7_COUNT8_CC0         (*(RwReg8 *)0x4300181CUL) /**< \brief (TC7) COUNT8 Compare and Capture 0 */
#define REG_TC7_COUNT8_CC1         (*(RwReg8 *)0x4300181DUL) /**< \brief (TC7) COUNT8 Compare and Capture 1 */
#define REG_TC7_COUNT8_PERBUF      (*(RwReg8 *)0x4300182FUL) /**< \brief (TC7) COUNT8 Period Buffer */
#define REG_TC7_COUNT8_CCBUF0      (*(RwReg8 *)0x43001830UL) /**< \brief (TC7) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC7_COUNT8_CCBUF1      (*(RwReg8 *)0x43001831UL) /**< \brief (TC7) COUNT8 Compare and Capture Buffer 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC7 peripheral ========== */
#define TC7_CC_NUM                  2       
#define TC7_DMAC_ID_MC_0            66
#define TC7_DMAC_ID_MC_1            67
#define TC7_DMAC_ID_MC_LSB          66
#define TC7_DMAC_ID_MC_MSB          67
#define TC7_DMAC_ID_MC_SIZE         2
#define TC7_DMAC_ID_OVF             65       // Indexes of DMA Overflow trigger
#define TC7_EXT                     0        // Coding of implemented extended features (keep 0 value)
#define TC7_GCLK_ID                 39       // Index of Generic Clock
#define TC7_MASTER_SLAVE_MODE       2        // TC type 0 : NA, 1 : Master, 2 : Slave
#define TC7_OW_NUM                  2        // Number of Output Waveforms

#endif /* _SAMD51_TC7_INSTANCE_ */
