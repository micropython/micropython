/**
 * \file
 *
 * \brief Instance description for TCC4
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

#ifndef _SAMD51_TCC4_INSTANCE_
#define _SAMD51_TCC4_INSTANCE_

/* ========== Register definition for TCC4 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TCC4_CTRLA             (0x43001000) /**< \brief (TCC4) Control A */
#define REG_TCC4_CTRLBCLR          (0x43001004) /**< \brief (TCC4) Control B Clear */
#define REG_TCC4_CTRLBSET          (0x43001005) /**< \brief (TCC4) Control B Set */
#define REG_TCC4_SYNCBUSY          (0x43001008) /**< \brief (TCC4) Synchronization Busy */
#define REG_TCC4_FCTRLA            (0x4300100C) /**< \brief (TCC4) Recoverable Fault A Configuration */
#define REG_TCC4_FCTRLB            (0x43001010) /**< \brief (TCC4) Recoverable Fault B Configuration */
#define REG_TCC4_DRVCTRL           (0x43001018) /**< \brief (TCC4) Driver Control */
#define REG_TCC4_DBGCTRL           (0x4300101E) /**< \brief (TCC4) Debug Control */
#define REG_TCC4_EVCTRL            (0x43001020) /**< \brief (TCC4) Event Control */
#define REG_TCC4_INTENCLR          (0x43001024) /**< \brief (TCC4) Interrupt Enable Clear */
#define REG_TCC4_INTENSET          (0x43001028) /**< \brief (TCC4) Interrupt Enable Set */
#define REG_TCC4_INTFLAG           (0x4300102C) /**< \brief (TCC4) Interrupt Flag Status and Clear */
#define REG_TCC4_STATUS            (0x43001030) /**< \brief (TCC4) Status */
#define REG_TCC4_COUNT             (0x43001034) /**< \brief (TCC4) Count */
#define REG_TCC4_WAVE              (0x4300103C) /**< \brief (TCC4) Waveform Control */
#define REG_TCC4_PER               (0x43001040) /**< \brief (TCC4) Period */
#define REG_TCC4_CC0               (0x43001044) /**< \brief (TCC4) Compare and Capture 0 */
#define REG_TCC4_CC1               (0x43001048) /**< \brief (TCC4) Compare and Capture 1 */
#define REG_TCC4_PERBUF            (0x4300106C) /**< \brief (TCC4) Period Buffer */
#define REG_TCC4_CCBUF0            (0x43001070) /**< \brief (TCC4) Compare and Capture Buffer 0 */
#define REG_TCC4_CCBUF1            (0x43001074) /**< \brief (TCC4) Compare and Capture Buffer 1 */
#else
#define REG_TCC4_CTRLA             (*(RwReg  *)0x43001000UL) /**< \brief (TCC4) Control A */
#define REG_TCC4_CTRLBCLR          (*(RwReg8 *)0x43001004UL) /**< \brief (TCC4) Control B Clear */
#define REG_TCC4_CTRLBSET          (*(RwReg8 *)0x43001005UL) /**< \brief (TCC4) Control B Set */
#define REG_TCC4_SYNCBUSY          (*(RoReg  *)0x43001008UL) /**< \brief (TCC4) Synchronization Busy */
#define REG_TCC4_FCTRLA            (*(RwReg  *)0x4300100CUL) /**< \brief (TCC4) Recoverable Fault A Configuration */
#define REG_TCC4_FCTRLB            (*(RwReg  *)0x43001010UL) /**< \brief (TCC4) Recoverable Fault B Configuration */
#define REG_TCC4_DRVCTRL           (*(RwReg  *)0x43001018UL) /**< \brief (TCC4) Driver Control */
#define REG_TCC4_DBGCTRL           (*(RwReg8 *)0x4300101EUL) /**< \brief (TCC4) Debug Control */
#define REG_TCC4_EVCTRL            (*(RwReg  *)0x43001020UL) /**< \brief (TCC4) Event Control */
#define REG_TCC4_INTENCLR          (*(RwReg  *)0x43001024UL) /**< \brief (TCC4) Interrupt Enable Clear */
#define REG_TCC4_INTENSET          (*(RwReg  *)0x43001028UL) /**< \brief (TCC4) Interrupt Enable Set */
#define REG_TCC4_INTFLAG           (*(RwReg  *)0x4300102CUL) /**< \brief (TCC4) Interrupt Flag Status and Clear */
#define REG_TCC4_STATUS            (*(RwReg  *)0x43001030UL) /**< \brief (TCC4) Status */
#define REG_TCC4_COUNT             (*(RwReg  *)0x43001034UL) /**< \brief (TCC4) Count */
#define REG_TCC4_WAVE              (*(RwReg  *)0x4300103CUL) /**< \brief (TCC4) Waveform Control */
#define REG_TCC4_PER               (*(RwReg  *)0x43001040UL) /**< \brief (TCC4) Period */
#define REG_TCC4_CC0               (*(RwReg  *)0x43001044UL) /**< \brief (TCC4) Compare and Capture 0 */
#define REG_TCC4_CC1               (*(RwReg  *)0x43001048UL) /**< \brief (TCC4) Compare and Capture 1 */
#define REG_TCC4_PERBUF            (*(RwReg  *)0x4300106CUL) /**< \brief (TCC4) Period Buffer */
#define REG_TCC4_CCBUF0            (*(RwReg  *)0x43001070UL) /**< \brief (TCC4) Compare and Capture Buffer 0 */
#define REG_TCC4_CCBUF1            (*(RwReg  *)0x43001074UL) /**< \brief (TCC4) Compare and Capture Buffer 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TCC4 peripheral ========== */
#define TCC4_CC_NUM                 2        // Number of Compare/Capture units
#define TCC4_DITHERING              0        // Dithering feature implemented
#define TCC4_DMAC_ID_MC_0           42
#define TCC4_DMAC_ID_MC_1           43
#define TCC4_DMAC_ID_MC_LSB         42
#define TCC4_DMAC_ID_MC_MSB         43
#define TCC4_DMAC_ID_MC_SIZE        2
#define TCC4_DMAC_ID_OVF            41       // DMA overflow/underflow/retrigger trigger
#define TCC4_DTI                    0        // Dead-Time-Insertion feature implemented
#define TCC4_EXT                    0        // Coding of implemented extended features
#define TCC4_GCLK_ID                38       // Index of Generic Clock
#define TCC4_MASTER_SLAVE_MODE      0        // TCC type 0 : NA, 1 : Master, 2 : Slave
#define TCC4_OTMX                   0        // Output Matrix feature implemented
#define TCC4_OW_NUM                 2        // Number of Output Waveforms
#define TCC4_PG                     0        // Pattern Generation feature implemented
#define TCC4_SIZE                   16      
#define TCC4_SWAP                   0        // DTI outputs swap feature implemented

#endif /* _SAMD51_TCC4_INSTANCE_ */
