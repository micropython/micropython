/**
 * \file
 *
 * \brief Instance description for TC6
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

#ifndef _SAMD51_TC6_INSTANCE_
#define _SAMD51_TC6_INSTANCE_

/* ========== Register definition for TC6 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC6_CTRLA              (0x43001400) /**< \brief (TC6) Control A */
#define REG_TC6_CTRLBCLR           (0x43001404) /**< \brief (TC6) Control B Clear */
#define REG_TC6_CTRLBSET           (0x43001405) /**< \brief (TC6) Control B Set */
#define REG_TC6_EVCTRL             (0x43001406) /**< \brief (TC6) Event Control */
#define REG_TC6_INTENCLR           (0x43001408) /**< \brief (TC6) Interrupt Enable Clear */
#define REG_TC6_INTENSET           (0x43001409) /**< \brief (TC6) Interrupt Enable Set */
#define REG_TC6_INTFLAG            (0x4300140A) /**< \brief (TC6) Interrupt Flag Status and Clear */
#define REG_TC6_STATUS             (0x4300140B) /**< \brief (TC6) Status */
#define REG_TC6_WAVE               (0x4300140C) /**< \brief (TC6) Waveform Generation Control */
#define REG_TC6_DRVCTRL            (0x4300140D) /**< \brief (TC6) Control C */
#define REG_TC6_DBGCTRL            (0x4300140F) /**< \brief (TC6) Debug Control */
#define REG_TC6_SYNCBUSY           (0x43001410) /**< \brief (TC6) Synchronization Status */
#define REG_TC6_COUNT16_COUNT      (0x43001414) /**< \brief (TC6) COUNT16 Count */
#define REG_TC6_COUNT16_CC0        (0x4300141C) /**< \brief (TC6) COUNT16 Compare and Capture 0 */
#define REG_TC6_COUNT16_CC1        (0x4300141E) /**< \brief (TC6) COUNT16 Compare and Capture 1 */
#define REG_TC6_COUNT16_CCBUF0     (0x43001430) /**< \brief (TC6) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC6_COUNT16_CCBUF1     (0x43001432) /**< \brief (TC6) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC6_COUNT32_COUNT      (0x43001414) /**< \brief (TC6) COUNT32 Count */
#define REG_TC6_COUNT32_CC0        (0x4300141C) /**< \brief (TC6) COUNT32 Compare and Capture 0 */
#define REG_TC6_COUNT32_CC1        (0x43001420) /**< \brief (TC6) COUNT32 Compare and Capture 1 */
#define REG_TC6_COUNT32_CCBUF0     (0x43001430) /**< \brief (TC6) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC6_COUNT32_CCBUF1     (0x43001434) /**< \brief (TC6) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC6_COUNT8_COUNT       (0x43001414) /**< \brief (TC6) COUNT8 Count */
#define REG_TC6_COUNT8_PER         (0x4300141B) /**< \brief (TC6) COUNT8 Period */
#define REG_TC6_COUNT8_CC0         (0x4300141C) /**< \brief (TC6) COUNT8 Compare and Capture 0 */
#define REG_TC6_COUNT8_CC1         (0x4300141D) /**< \brief (TC6) COUNT8 Compare and Capture 1 */
#define REG_TC6_COUNT8_PERBUF      (0x4300142F) /**< \brief (TC6) COUNT8 Period Buffer */
#define REG_TC6_COUNT8_CCBUF0      (0x43001430) /**< \brief (TC6) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC6_COUNT8_CCBUF1      (0x43001431) /**< \brief (TC6) COUNT8 Compare and Capture Buffer 1 */
#else
#define REG_TC6_CTRLA              (*(RwReg  *)0x43001400UL) /**< \brief (TC6) Control A */
#define REG_TC6_CTRLBCLR           (*(RwReg8 *)0x43001404UL) /**< \brief (TC6) Control B Clear */
#define REG_TC6_CTRLBSET           (*(RwReg8 *)0x43001405UL) /**< \brief (TC6) Control B Set */
#define REG_TC6_EVCTRL             (*(RwReg16*)0x43001406UL) /**< \brief (TC6) Event Control */
#define REG_TC6_INTENCLR           (*(RwReg8 *)0x43001408UL) /**< \brief (TC6) Interrupt Enable Clear */
#define REG_TC6_INTENSET           (*(RwReg8 *)0x43001409UL) /**< \brief (TC6) Interrupt Enable Set */
#define REG_TC6_INTFLAG            (*(RwReg8 *)0x4300140AUL) /**< \brief (TC6) Interrupt Flag Status and Clear */
#define REG_TC6_STATUS             (*(RwReg8 *)0x4300140BUL) /**< \brief (TC6) Status */
#define REG_TC6_WAVE               (*(RwReg8 *)0x4300140CUL) /**< \brief (TC6) Waveform Generation Control */
#define REG_TC6_DRVCTRL            (*(RwReg8 *)0x4300140DUL) /**< \brief (TC6) Control C */
#define REG_TC6_DBGCTRL            (*(RwReg8 *)0x4300140FUL) /**< \brief (TC6) Debug Control */
#define REG_TC6_SYNCBUSY           (*(RoReg  *)0x43001410UL) /**< \brief (TC6) Synchronization Status */
#define REG_TC6_COUNT16_COUNT      (*(RwReg16*)0x43001414UL) /**< \brief (TC6) COUNT16 Count */
#define REG_TC6_COUNT16_CC0        (*(RwReg16*)0x4300141CUL) /**< \brief (TC6) COUNT16 Compare and Capture 0 */
#define REG_TC6_COUNT16_CC1        (*(RwReg16*)0x4300141EUL) /**< \brief (TC6) COUNT16 Compare and Capture 1 */
#define REG_TC6_COUNT16_CCBUF0     (*(RwReg16*)0x43001430UL) /**< \brief (TC6) COUNT16 Compare and Capture Buffer 0 */
#define REG_TC6_COUNT16_CCBUF1     (*(RwReg16*)0x43001432UL) /**< \brief (TC6) COUNT16 Compare and Capture Buffer 1 */
#define REG_TC6_COUNT32_COUNT      (*(RwReg  *)0x43001414UL) /**< \brief (TC6) COUNT32 Count */
#define REG_TC6_COUNT32_CC0        (*(RwReg  *)0x4300141CUL) /**< \brief (TC6) COUNT32 Compare and Capture 0 */
#define REG_TC6_COUNT32_CC1        (*(RwReg  *)0x43001420UL) /**< \brief (TC6) COUNT32 Compare and Capture 1 */
#define REG_TC6_COUNT32_CCBUF0     (*(RwReg  *)0x43001430UL) /**< \brief (TC6) COUNT32 Compare and Capture Buffer 0 */
#define REG_TC6_COUNT32_CCBUF1     (*(RwReg  *)0x43001434UL) /**< \brief (TC6) COUNT32 Compare and Capture Buffer 1 */
#define REG_TC6_COUNT8_COUNT       (*(RwReg8 *)0x43001414UL) /**< \brief (TC6) COUNT8 Count */
#define REG_TC6_COUNT8_PER         (*(RwReg8 *)0x4300141BUL) /**< \brief (TC6) COUNT8 Period */
#define REG_TC6_COUNT8_CC0         (*(RwReg8 *)0x4300141CUL) /**< \brief (TC6) COUNT8 Compare and Capture 0 */
#define REG_TC6_COUNT8_CC1         (*(RwReg8 *)0x4300141DUL) /**< \brief (TC6) COUNT8 Compare and Capture 1 */
#define REG_TC6_COUNT8_PERBUF      (*(RwReg8 *)0x4300142FUL) /**< \brief (TC6) COUNT8 Period Buffer */
#define REG_TC6_COUNT8_CCBUF0      (*(RwReg8 *)0x43001430UL) /**< \brief (TC6) COUNT8 Compare and Capture Buffer 0 */
#define REG_TC6_COUNT8_CCBUF1      (*(RwReg8 *)0x43001431UL) /**< \brief (TC6) COUNT8 Compare and Capture Buffer 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC6 peripheral ========== */
#define TC6_CC_NUM                  2       
#define TC6_DMAC_ID_MC_0            63
#define TC6_DMAC_ID_MC_1            64
#define TC6_DMAC_ID_MC_LSB          63
#define TC6_DMAC_ID_MC_MSB          64
#define TC6_DMAC_ID_MC_SIZE         2
#define TC6_DMAC_ID_OVF             62       // Indexes of DMA Overflow trigger
#define TC6_EXT                     0        // Coding of implemented extended features (keep 0 value)
#define TC6_GCLK_ID                 39       // Index of Generic Clock
#define TC6_MASTER_SLAVE_MODE       1        // TC type 0 : NA, 1 : Master, 2 : Slave
#define TC6_OW_NUM                  2        // Number of Output Waveforms

#endif /* _SAMD51_TC6_INSTANCE_ */
