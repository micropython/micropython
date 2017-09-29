/**
 * \file
 *
 * \brief Instance description for TC1
 *
 * Copyright (c) 2017 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD51_TC1_INSTANCE_
#define _SAMD51_TC1_INSTANCE_

/* ========== Register definition for TC1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC1_CTRLA              (0x40003C00) /**< \brief (TC1) Control A */
#define REG_TC1_CTRLBCLR           (0x40003C04) /**< \brief (TC1) Control B Clear */
#define REG_TC1_CTRLBSET           (0x40003C05) /**< \brief (TC1) Control B Set */
#define REG_TC1_EVCTRL             (0x40003C06) /**< \brief (TC1) Event Control */
#define REG_TC1_INTENCLR           (0x40003C08) /**< \brief (TC1) Interrupt Enable Clear */
#define REG_TC1_INTENSET           (0x40003C09) /**< \brief (TC1) Interrupt Enable Set */
#define REG_TC1_INTFLAG            (0x40003C0A) /**< \brief (TC1) Interrupt Flag Status and Clear */
#define REG_TC1_STATUS             (0x40003C0B) /**< \brief (TC1) Status */
#define REG_TC1_WAVE               (0x40003C0C) /**< \brief (TC1) Waveform Generation Control */
#define REG_TC1_DRVCTRL            (0x40003C0D) /**< \brief (TC1) Control C */
#define REG_TC1_DBGCTRL            (0x40003C0F) /**< \brief (TC1) Debug Control */
#define REG_TC1_SYNCBUSY           (0x40003C10) /**< \brief (TC1) Synchronization Status */
#define REG_TC1_COUNT16_COUNT      (0x40003C14) /**< \brief (TC1) COUNT16 Count */
#define REG_TC1_COUNT16_CC0        (0x40003C1C) /**< \brief (TC1) COUNT16 Compare and Capture 0 */
#define REG_TC1_COUNT16_CC1        (0x40003C1E) /**< \brief (TC1) COUNT16 Compare and Capture 1 */
#define REG_TC1_COUNT16_CCBUF0     (0x40003C30) /**< \brief (TC1) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT16_CCBUF1     (0x40003C32) /**< \brief (TC1) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC1_COUNT32_COUNT      (0x40003C14) /**< \brief (TC1) COUNT32 Count */
#define REG_TC1_COUNT32_CC0        (0x40003C1C) /**< \brief (TC1) COUNT32 Compare and Capture 0 */
#define REG_TC1_COUNT32_CC1        (0x40003C20) /**< \brief (TC1) COUNT32 Compare and Capture 1 */
#define REG_TC1_COUNT32_CCBUF0     (0x40003C30) /**< \brief (TC1) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT32_CCBUF1     (0x40003C34) /**< \brief (TC1) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC1_COUNT8_COUNT       (0x40003C14) /**< \brief (TC1) COUNT8 Count */
#define REG_TC1_COUNT8_PER         (0x40003C1B) /**< \brief (TC1) COUNT8 Period */
#define REG_TC1_COUNT8_CC0         (0x40003C1C) /**< \brief (TC1) COUNT8 Compare and Capture 0 */
#define REG_TC1_COUNT8_CC1         (0x40003C1D) /**< \brief (TC1) COUNT8 Compare and Capture 1 */
#define REG_TC1_COUNT8_PERBUF      (0x40003C2F) /**< \brief (TC1) COUNT8 Period Buffer */
#define REG_TC1_COUNT8_CCBUF0      (0x40003C30) /**< \brief (TC1) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT8_CCBUF1      (0x40003C31) /**< \brief (TC1) COUNT8 Compare and Capture Buffer 1 */
#else
#define REG_TC1_CTRLA              (*(RwReg  *)0x40003C00UL) /**< \brief (TC1) Control A */
#define REG_TC1_CTRLBCLR           (*(RwReg8 *)0x40003C04UL) /**< \brief (TC1) Control B Clear */
#define REG_TC1_CTRLBSET           (*(RwReg8 *)0x40003C05UL) /**< \brief (TC1) Control B Set */
#define REG_TC1_EVCTRL             (*(RwReg16*)0x40003C06UL) /**< \brief (TC1) Event Control */
#define REG_TC1_INTENCLR           (*(RwReg8 *)0x40003C08UL) /**< \brief (TC1) Interrupt Enable Clear */
#define REG_TC1_INTENSET           (*(RwReg8 *)0x40003C09UL) /**< \brief (TC1) Interrupt Enable Set */
#define REG_TC1_INTFLAG            (*(RwReg8 *)0x40003C0AUL) /**< \brief (TC1) Interrupt Flag Status and Clear */
#define REG_TC1_STATUS             (*(RwReg8 *)0x40003C0BUL) /**< \brief (TC1) Status */
#define REG_TC1_WAVE               (*(RwReg8 *)0x40003C0CUL) /**< \brief (TC1) Waveform Generation Control */
#define REG_TC1_DRVCTRL            (*(RwReg8 *)0x40003C0DUL) /**< \brief (TC1) Control C */
#define REG_TC1_DBGCTRL            (*(RwReg8 *)0x40003C0FUL) /**< \brief (TC1) Debug Control */
#define REG_TC1_SYNCBUSY           (*(RoReg  *)0x40003C10UL) /**< \brief (TC1) Synchronization Status */
#define REG_TC1_COUNT16_COUNT      (*(RwReg16*)0x40003C14UL) /**< \brief (TC1) COUNT16 Count */
#define REG_TC1_COUNT16_CC0        (*(RwReg16*)0x40003C1CUL) /**< \brief (TC1) COUNT16 Compare and Capture 0 */
#define REG_TC1_COUNT16_CC1        (*(RwReg16*)0x40003C1EUL) /**< \brief (TC1) COUNT16 Compare and Capture 1 */
#define REG_TC1_COUNT16_CCBUF0     (*(RwReg16*)0x40003C30UL) /**< \brief (TC1) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT16_CCBUF1     (*(RwReg16*)0x40003C32UL) /**< \brief (TC1) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC1_COUNT32_COUNT      (*(RwReg  *)0x40003C14UL) /**< \brief (TC1) COUNT32 Count */
#define REG_TC1_COUNT32_CC0        (*(RwReg  *)0x40003C1CUL) /**< \brief (TC1) COUNT32 Compare and Capture 0 */
#define REG_TC1_COUNT32_CC1        (*(RwReg  *)0x40003C20UL) /**< \brief (TC1) COUNT32 Compare and Capture 1 */
#define REG_TC1_COUNT32_CCBUF0     (*(RwReg  *)0x40003C30UL) /**< \brief (TC1) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT32_CCBUF1     (*(RwReg  *)0x40003C34UL) /**< \brief (TC1) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC1_COUNT8_COUNT       (*(RwReg8 *)0x40003C14UL) /**< \brief (TC1) COUNT8 Count */
#define REG_TC1_COUNT8_PER         (*(RwReg8 *)0x40003C1BUL) /**< \brief (TC1) COUNT8 Period */
#define REG_TC1_COUNT8_CC0         (*(RwReg8 *)0x40003C1CUL) /**< \brief (TC1) COUNT8 Compare and Capture 0 */
#define REG_TC1_COUNT8_CC1         (*(RwReg8 *)0x40003C1DUL) /**< \brief (TC1) COUNT8 Compare and Capture 1 */
#define REG_TC1_COUNT8_PERBUF      (*(RwReg8 *)0x40003C2FUL) /**< \brief (TC1) COUNT8 Period Buffer */
#define REG_TC1_COUNT8_CCBUF0      (*(RwReg8 *)0x40003C30UL) /**< \brief (TC1) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC1_COUNT8_CCBUF1      (*(RwReg8 *)0x40003C31UL) /**< \brief (TC1) COUNT8 Compare and Capture Buffer 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC1 peripheral ========== */
#define TC1_CC_NUM                  2       
#define TC1_DMAC_ID_MC_0            48
#define TC1_DMAC_ID_MC_1            49
#define TC1_DMAC_ID_MC_LSB          48
#define TC1_DMAC_ID_MC_MSB          49
#define TC1_DMAC_ID_MC_SIZE         2
#define TC1_DMAC_ID_OVF             47       // Indexes of DMA Overflow trigger
#define TC1_EXT                     0        // Coding of implemented extended features (keep 0 value)
#define TC1_GCLK_ID                 9        // Index of Generic Clock
#define TC1_MASTER_SLAVE_MODE       2        // TC type 0 : NA, 1 : Master, 2 : Slave
#define TC1_OW_NUM                  2        // Number of Output Waveforms

#endif /* _SAMD51_TC1_INSTANCE_ */
