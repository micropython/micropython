/**
 * \file
 *
 * \brief Instance description for TCC0
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

#ifndef _SAMD51_TCC0_INSTANCE_
#define _SAMD51_TCC0_INSTANCE_

/* ========== Register definition for TCC0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TCC0_CTRLA             (0x41016000) /**< \brief (TCC0) Control A */
#define REG_TCC0_CTRLBCLR          (0x41016004) /**< \brief (TCC0) Control B Clear */
#define REG_TCC0_CTRLBSET          (0x41016005) /**< \brief (TCC0) Control B Set */
#define REG_TCC0_SYNCBUSY          (0x41016008) /**< \brief (TCC0) Synchronization Busy */
#define REG_TCC0_FCTRLA            (0x4101600C) /**< \brief (TCC0) Recoverable Fault A Configuration */
#define REG_TCC0_FCTRLB            (0x41016010) /**< \brief (TCC0) Recoverable Fault B Configuration */
#define REG_TCC0_WEXCTRL           (0x41016014) /**< \brief (TCC0) Waveform Extension Configuration */
#define REG_TCC0_DRVCTRL           (0x41016018) /**< \brief (TCC0) Driver Control */
#define REG_TCC0_DBGCTRL           (0x4101601E) /**< \brief (TCC0) Debug Control */
#define REG_TCC0_EVCTRL            (0x41016020) /**< \brief (TCC0) Event Control */
#define REG_TCC0_INTENCLR          (0x41016024) /**< \brief (TCC0) Interrupt Enable Clear */
#define REG_TCC0_INTENSET          (0x41016028) /**< \brief (TCC0) Interrupt Enable Set */
#define REG_TCC0_INTFLAG           (0x4101602C) /**< \brief (TCC0) Interrupt Flag Status and Clear */
#define REG_TCC0_STATUS            (0x41016030) /**< \brief (TCC0) Status */
#define REG_TCC0_COUNT             (0x41016034) /**< \brief (TCC0) Count */
#define REG_TCC0_PATT              (0x41016038) /**< \brief (TCC0) Pattern */
#define REG_TCC0_WAVE              (0x4101603C) /**< \brief (TCC0) Waveform Control */
#define REG_TCC0_PER               (0x41016040) /**< \brief (TCC0) Period */
#define REG_TCC0_CC0               (0x41016044) /**< \brief (TCC0) Compare and Capture 0 */
#define REG_TCC0_CC1               (0x41016048) /**< \brief (TCC0) Compare and Capture 1 */
#define REG_TCC0_CC2               (0x4101604C) /**< \brief (TCC0) Compare and Capture 2 */
#define REG_TCC0_CC3               (0x41016050) /**< \brief (TCC0) Compare and Capture 3 */
#define REG_TCC0_CC4               (0x41016054) /**< \brief (TCC0) Compare and Capture 4 */
#define REG_TCC0_CC5               (0x41016058) /**< \brief (TCC0) Compare and Capture 5 */
#define REG_TCC0_PATTBUF           (0x41016064) /**< \brief (TCC0) Pattern Buffer */
#define REG_TCC0_PERBUF            (0x4101606C) /**< \brief (TCC0) Period Buffer */
#define REG_TCC0_CCBUF0            (0x41016070) /**< \brief (TCC0) Compare and Capture Buffer 0 */
#define REG_TCC0_CCBUF1            (0x41016074) /**< \brief (TCC0) Compare and Capture Buffer 1 */
#define REG_TCC0_CCBUF2            (0x41016078) /**< \brief (TCC0) Compare and Capture Buffer 2 */
#define REG_TCC0_CCBUF3            (0x4101607C) /**< \brief (TCC0) Compare and Capture Buffer 3 */
#define REG_TCC0_CCBUF4            (0x41016080) /**< \brief (TCC0) Compare and Capture Buffer 4 */
#define REG_TCC0_CCBUF5            (0x41016084) /**< \brief (TCC0) Compare and Capture Buffer 5 */
#else
#define REG_TCC0_CTRLA             (*(RwReg  *)0x41016000UL) /**< \brief (TCC0) Control A */
#define REG_TCC0_CTRLBCLR          (*(RwReg8 *)0x41016004UL) /**< \brief (TCC0) Control B Clear */
#define REG_TCC0_CTRLBSET          (*(RwReg8 *)0x41016005UL) /**< \brief (TCC0) Control B Set */
#define REG_TCC0_SYNCBUSY          (*(RoReg  *)0x41016008UL) /**< \brief (TCC0) Synchronization Busy */
#define REG_TCC0_FCTRLA            (*(RwReg  *)0x4101600CUL) /**< \brief (TCC0) Recoverable Fault A Configuration */
#define REG_TCC0_FCTRLB            (*(RwReg  *)0x41016010UL) /**< \brief (TCC0) Recoverable Fault B Configuration */
#define REG_TCC0_WEXCTRL           (*(RwReg  *)0x41016014UL) /**< \brief (TCC0) Waveform Extension Configuration */
#define REG_TCC0_DRVCTRL           (*(RwReg  *)0x41016018UL) /**< \brief (TCC0) Driver Control */
#define REG_TCC0_DBGCTRL           (*(RwReg8 *)0x4101601EUL) /**< \brief (TCC0) Debug Control */
#define REG_TCC0_EVCTRL            (*(RwReg  *)0x41016020UL) /**< \brief (TCC0) Event Control */
#define REG_TCC0_INTENCLR          (*(RwReg  *)0x41016024UL) /**< \brief (TCC0) Interrupt Enable Clear */
#define REG_TCC0_INTENSET          (*(RwReg  *)0x41016028UL) /**< \brief (TCC0) Interrupt Enable Set */
#define REG_TCC0_INTFLAG           (*(RwReg  *)0x4101602CUL) /**< \brief (TCC0) Interrupt Flag Status and Clear */
#define REG_TCC0_STATUS            (*(RwReg  *)0x41016030UL) /**< \brief (TCC0) Status */
#define REG_TCC0_COUNT             (*(RwReg  *)0x41016034UL) /**< \brief (TCC0) Count */
#define REG_TCC0_PATT              (*(RwReg16*)0x41016038UL) /**< \brief (TCC0) Pattern */
#define REG_TCC0_WAVE              (*(RwReg  *)0x4101603CUL) /**< \brief (TCC0) Waveform Control */
#define REG_TCC0_PER               (*(RwReg  *)0x41016040UL) /**< \brief (TCC0) Period */
#define REG_TCC0_CC0               (*(RwReg  *)0x41016044UL) /**< \brief (TCC0) Compare and Capture 0 */
#define REG_TCC0_CC1               (*(RwReg  *)0x41016048UL) /**< \brief (TCC0) Compare and Capture 1 */
#define REG_TCC0_CC2               (*(RwReg  *)0x4101604CUL) /**< \brief (TCC0) Compare and Capture 2 */
#define REG_TCC0_CC3               (*(RwReg  *)0x41016050UL) /**< \brief (TCC0) Compare and Capture 3 */
#define REG_TCC0_CC4               (*(RwReg  *)0x41016054UL) /**< \brief (TCC0) Compare and Capture 4 */
#define REG_TCC0_CC5               (*(RwReg  *)0x41016058UL) /**< \brief (TCC0) Compare and Capture 5 */
#define REG_TCC0_PATTBUF           (*(RwReg16*)0x41016064UL) /**< \brief (TCC0) Pattern Buffer */
#define REG_TCC0_PERBUF            (*(RwReg  *)0x4101606CUL) /**< \brief (TCC0) Period Buffer */
#define REG_TCC0_CCBUF0            (*(RwReg  *)0x41016070UL) /**< \brief (TCC0) Compare and Capture Buffer 0 */
#define REG_TCC0_CCBUF1            (*(RwReg  *)0x41016074UL) /**< \brief (TCC0) Compare and Capture Buffer 1 */
#define REG_TCC0_CCBUF2            (*(RwReg  *)0x41016078UL) /**< \brief (TCC0) Compare and Capture Buffer 2 */
#define REG_TCC0_CCBUF3            (*(RwReg  *)0x4101607CUL) /**< \brief (TCC0) Compare and Capture Buffer 3 */
#define REG_TCC0_CCBUF4            (*(RwReg  *)0x41016080UL) /**< \brief (TCC0) Compare and Capture Buffer 4 */
#define REG_TCC0_CCBUF5            (*(RwReg  *)0x41016084UL) /**< \brief (TCC0) Compare and Capture Buffer 5 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TCC0 peripheral ========== */
#define TCC0_CC_NUM                 6        // Number of Compare/Capture units
#define TCC0_DITHERING              1        // Dithering feature implemented
#define TCC0_DMAC_ID_MC_0           23
#define TCC0_DMAC_ID_MC_1           24
#define TCC0_DMAC_ID_MC_2           25
#define TCC0_DMAC_ID_MC_3           26
#define TCC0_DMAC_ID_MC_4           27
#define TCC0_DMAC_ID_MC_5           28
#define TCC0_DMAC_ID_MC_LSB         23
#define TCC0_DMAC_ID_MC_MSB         28
#define TCC0_DMAC_ID_MC_SIZE        6
#define TCC0_DMAC_ID_OVF            22       // DMA overflow/underflow/retrigger trigger
#define TCC0_DTI                    1        // Dead-Time-Insertion feature implemented
#define TCC0_EXT                    31       // Coding of implemented extended features
#define TCC0_GCLK_ID                25       // Index of Generic Clock
#define TCC0_MASTER_SLAVE_MODE      1        // TCC type 0 : NA, 1 : Master, 2 : Slave
#define TCC0_OTMX                   1        // Output Matrix feature implemented
#define TCC0_OW_NUM                 8        // Number of Output Waveforms
#define TCC0_PG                     1        // Pattern Generation feature implemented
#define TCC0_SIZE                   24      
#define TCC0_SWAP                   1        // DTI outputs swap feature implemented

#endif /* _SAMD51_TCC0_INSTANCE_ */
