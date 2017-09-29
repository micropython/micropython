/**
 * \file
 *
 * \brief Instance description for TCC3
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

#ifndef _SAMD51_TCC3_INSTANCE_
#define _SAMD51_TCC3_INSTANCE_

/* ========== Register definition for TCC3 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TCC3_CTRLA             (0x42001000) /**< \brief (TCC3) Control A */
#define REG_TCC3_CTRLBCLR          (0x42001004) /**< \brief (TCC3) Control B Clear */
#define REG_TCC3_CTRLBSET          (0x42001005) /**< \brief (TCC3) Control B Set */
#define REG_TCC3_SYNCBUSY          (0x42001008) /**< \brief (TCC3) Synchronization Busy */
#define REG_TCC3_FCTRLA            (0x4200100C) /**< \brief (TCC3) Recoverable Fault A Configuration */
#define REG_TCC3_FCTRLB            (0x42001010) /**< \brief (TCC3) Recoverable Fault B Configuration */
#define REG_TCC3_DRVCTRL           (0x42001018) /**< \brief (TCC3) Driver Control */
#define REG_TCC3_DBGCTRL           (0x4200101E) /**< \brief (TCC3) Debug Control */
#define REG_TCC3_EVCTRL            (0x42001020) /**< \brief (TCC3) Event Control */
#define REG_TCC3_INTENCLR          (0x42001024) /**< \brief (TCC3) Interrupt Enable Clear */
#define REG_TCC3_INTENSET          (0x42001028) /**< \brief (TCC3) Interrupt Enable Set */
#define REG_TCC3_INTFLAG           (0x4200102C) /**< \brief (TCC3) Interrupt Flag Status and Clear */
#define REG_TCC3_STATUS            (0x42001030) /**< \brief (TCC3) Status */
#define REG_TCC3_COUNT             (0x42001034) /**< \brief (TCC3) Count */
#define REG_TCC3_WAVE              (0x4200103C) /**< \brief (TCC3) Waveform Control */
#define REG_TCC3_PER               (0x42001040) /**< \brief (TCC3) Period */
#define REG_TCC3_CC0               (0x42001044) /**< \brief (TCC3) Compare and Capture 0 */
#define REG_TCC3_CC1               (0x42001048) /**< \brief (TCC3) Compare and Capture 1 */
#define REG_TCC3_PERBUF            (0x4200106C) /**< \brief (TCC3) Period Buffer */
#define REG_TCC3_CCBUF0            (0x42001070) /**< \brief (TCC3) Compare and Capture Buffer 0 */
#define REG_TCC3_CCBUF1            (0x42001074) /**< \brief (TCC3) Compare and Capture Buffer 1 */
#else
#define REG_TCC3_CTRLA             (*(RwReg  *)0x42001000UL) /**< \brief (TCC3) Control A */
#define REG_TCC3_CTRLBCLR          (*(RwReg8 *)0x42001004UL) /**< \brief (TCC3) Control B Clear */
#define REG_TCC3_CTRLBSET          (*(RwReg8 *)0x42001005UL) /**< \brief (TCC3) Control B Set */
#define REG_TCC3_SYNCBUSY          (*(RoReg  *)0x42001008UL) /**< \brief (TCC3) Synchronization Busy */
#define REG_TCC3_FCTRLA            (*(RwReg  *)0x4200100CUL) /**< \brief (TCC3) Recoverable Fault A Configuration */
#define REG_TCC3_FCTRLB            (*(RwReg  *)0x42001010UL) /**< \brief (TCC3) Recoverable Fault B Configuration */
#define REG_TCC3_DRVCTRL           (*(RwReg  *)0x42001018UL) /**< \brief (TCC3) Driver Control */
#define REG_TCC3_DBGCTRL           (*(RwReg8 *)0x4200101EUL) /**< \brief (TCC3) Debug Control */
#define REG_TCC3_EVCTRL            (*(RwReg  *)0x42001020UL) /**< \brief (TCC3) Event Control */
#define REG_TCC3_INTENCLR          (*(RwReg  *)0x42001024UL) /**< \brief (TCC3) Interrupt Enable Clear */
#define REG_TCC3_INTENSET          (*(RwReg  *)0x42001028UL) /**< \brief (TCC3) Interrupt Enable Set */
#define REG_TCC3_INTFLAG           (*(RwReg  *)0x4200102CUL) /**< \brief (TCC3) Interrupt Flag Status and Clear */
#define REG_TCC3_STATUS            (*(RwReg  *)0x42001030UL) /**< \brief (TCC3) Status */
#define REG_TCC3_COUNT             (*(RwReg  *)0x42001034UL) /**< \brief (TCC3) Count */
#define REG_TCC3_WAVE              (*(RwReg  *)0x4200103CUL) /**< \brief (TCC3) Waveform Control */
#define REG_TCC3_PER               (*(RwReg  *)0x42001040UL) /**< \brief (TCC3) Period */
#define REG_TCC3_CC0               (*(RwReg  *)0x42001044UL) /**< \brief (TCC3) Compare and Capture 0 */
#define REG_TCC3_CC1               (*(RwReg  *)0x42001048UL) /**< \brief (TCC3) Compare and Capture 1 */
#define REG_TCC3_PERBUF            (*(RwReg  *)0x4200106CUL) /**< \brief (TCC3) Period Buffer */
#define REG_TCC3_CCBUF0            (*(RwReg  *)0x42001070UL) /**< \brief (TCC3) Compare and Capture Buffer 0 */
#define REG_TCC3_CCBUF1            (*(RwReg  *)0x42001074UL) /**< \brief (TCC3) Compare and Capture Buffer 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TCC3 peripheral ========== */
#define TCC3_CC_NUM                 2        // Number of Compare/Capture units
#define TCC3_DITHERING              0        // Dithering feature implemented
#define TCC3_DMAC_ID_MC_0           39
#define TCC3_DMAC_ID_MC_1           40
#define TCC3_DMAC_ID_MC_LSB         39
#define TCC3_DMAC_ID_MC_MSB         40
#define TCC3_DMAC_ID_MC_SIZE        2
#define TCC3_DMAC_ID_OVF            38       // DMA overflow/underflow/retrigger trigger
#define TCC3_DTI                    0        // Dead-Time-Insertion feature implemented
#define TCC3_EXT                    0        // Coding of implemented extended features
#define TCC3_GCLK_ID                29       // Index of Generic Clock
#define TCC3_MASTER_SLAVE_MODE      0        // TCC type 0 : NA, 1 : Master, 2 : Slave
#define TCC3_OTMX                   0        // Output Matrix feature implemented
#define TCC3_OW_NUM                 2        // Number of Output Waveforms
#define TCC3_PG                     0        // Pattern Generation feature implemented
#define TCC3_SIZE                   16      
#define TCC3_SWAP                   0        // DTI outputs swap feature implemented

#endif /* _SAMD51_TCC3_INSTANCE_ */
