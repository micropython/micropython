/**
 * \file
 *
 * \brief Instance description for TCC2
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

#ifndef _SAMD21_TCC2_INSTANCE_
#define _SAMD21_TCC2_INSTANCE_

/* ========== Register definition for TCC2 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TCC2_CTRLA             (0x42002800) /**< \brief (TCC2) Control A */
#define REG_TCC2_CTRLBCLR          (0x42002804) /**< \brief (TCC2) Control B Clear */
#define REG_TCC2_CTRLBSET          (0x42002805) /**< \brief (TCC2) Control B Set */
#define REG_TCC2_SYNCBUSY          (0x42002808) /**< \brief (TCC2) Synchronization Busy */
#define REG_TCC2_FCTRLA            (0x4200280C) /**< \brief (TCC2) Recoverable Fault A Configuration */
#define REG_TCC2_FCTRLB            (0x42002810) /**< \brief (TCC2) Recoverable Fault B Configuration */
#define REG_TCC2_DRVCTRL           (0x42002818) /**< \brief (TCC2) Driver Control */
#define REG_TCC2_DBGCTRL           (0x4200281E) /**< \brief (TCC2) Debug Control */
#define REG_TCC2_EVCTRL            (0x42002820) /**< \brief (TCC2) Event Control */
#define REG_TCC2_INTENCLR          (0x42002824) /**< \brief (TCC2) Interrupt Enable Clear */
#define REG_TCC2_INTENSET          (0x42002828) /**< \brief (TCC2) Interrupt Enable Set */
#define REG_TCC2_INTFLAG           (0x4200282C) /**< \brief (TCC2) Interrupt Flag Status and Clear */
#define REG_TCC2_STATUS            (0x42002830) /**< \brief (TCC2) Status */
#define REG_TCC2_COUNT             (0x42002834) /**< \brief (TCC2) Count */
#define REG_TCC2_WAVE              (0x4200283C) /**< \brief (TCC2) Waveform Control */
#define REG_TCC2_PER               (0x42002840) /**< \brief (TCC2) Period */
#define REG_TCC2_CC0               (0x42002844) /**< \brief (TCC2) Compare and Capture 0 */
#define REG_TCC2_CC1               (0x42002848) /**< \brief (TCC2) Compare and Capture 1 */
#define REG_TCC2_WAVEB             (0x42002868) /**< \brief (TCC2) Waveform Control Buffer */
#define REG_TCC2_PERB              (0x4200286C) /**< \brief (TCC2) Period Buffer */
#define REG_TCC2_CCB0              (0x42002870) /**< \brief (TCC2) Compare and Capture Buffer 0 */
#define REG_TCC2_CCB1              (0x42002874) /**< \brief (TCC2) Compare and Capture Buffer 1 */
#else
#define REG_TCC2_CTRLA             (*(RwReg  *)0x42002800UL) /**< \brief (TCC2) Control A */
#define REG_TCC2_CTRLBCLR          (*(RwReg8 *)0x42002804UL) /**< \brief (TCC2) Control B Clear */
#define REG_TCC2_CTRLBSET          (*(RwReg8 *)0x42002805UL) /**< \brief (TCC2) Control B Set */
#define REG_TCC2_SYNCBUSY          (*(RoReg  *)0x42002808UL) /**< \brief (TCC2) Synchronization Busy */
#define REG_TCC2_FCTRLA            (*(RwReg  *)0x4200280CUL) /**< \brief (TCC2) Recoverable Fault A Configuration */
#define REG_TCC2_FCTRLB            (*(RwReg  *)0x42002810UL) /**< \brief (TCC2) Recoverable Fault B Configuration */
#define REG_TCC2_DRVCTRL           (*(RwReg  *)0x42002818UL) /**< \brief (TCC2) Driver Control */
#define REG_TCC2_DBGCTRL           (*(RwReg8 *)0x4200281EUL) /**< \brief (TCC2) Debug Control */
#define REG_TCC2_EVCTRL            (*(RwReg  *)0x42002820UL) /**< \brief (TCC2) Event Control */
#define REG_TCC2_INTENCLR          (*(RwReg  *)0x42002824UL) /**< \brief (TCC2) Interrupt Enable Clear */
#define REG_TCC2_INTENSET          (*(RwReg  *)0x42002828UL) /**< \brief (TCC2) Interrupt Enable Set */
#define REG_TCC2_INTFLAG           (*(RwReg  *)0x4200282CUL) /**< \brief (TCC2) Interrupt Flag Status and Clear */
#define REG_TCC2_STATUS            (*(RwReg  *)0x42002830UL) /**< \brief (TCC2) Status */
#define REG_TCC2_COUNT             (*(RwReg  *)0x42002834UL) /**< \brief (TCC2) Count */
#define REG_TCC2_WAVE              (*(RwReg  *)0x4200283CUL) /**< \brief (TCC2) Waveform Control */
#define REG_TCC2_PER               (*(RwReg  *)0x42002840UL) /**< \brief (TCC2) Period */
#define REG_TCC2_CC0               (*(RwReg  *)0x42002844UL) /**< \brief (TCC2) Compare and Capture 0 */
#define REG_TCC2_CC1               (*(RwReg  *)0x42002848UL) /**< \brief (TCC2) Compare and Capture 1 */
#define REG_TCC2_WAVEB             (*(RwReg  *)0x42002868UL) /**< \brief (TCC2) Waveform Control Buffer */
#define REG_TCC2_PERB              (*(RwReg  *)0x4200286CUL) /**< \brief (TCC2) Period Buffer */
#define REG_TCC2_CCB0              (*(RwReg  *)0x42002870UL) /**< \brief (TCC2) Compare and Capture Buffer 0 */
#define REG_TCC2_CCB1              (*(RwReg  *)0x42002874UL) /**< \brief (TCC2) Compare and Capture Buffer 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TCC2 peripheral ========== */
#define TCC2_CC_NUM                 2        // Number of Compare/Capture units
#define TCC2_DITHERING              0        // Dithering feature implemented
#define TCC2_DMAC_ID_MC_0           22
#define TCC2_DMAC_ID_MC_1           23
#define TCC2_DMAC_ID_MC_LSB         22
#define TCC2_DMAC_ID_MC_MSB         23
#define TCC2_DMAC_ID_MC_SIZE        2
#define TCC2_DMAC_ID_OVF            21       // DMA overflow/underflow/retrigger trigger
#define TCC2_DTI                    0        // Dead-Time-Insertion feature implemented
#define TCC2_EXT                    0        // Coding of implemented extended features
#define TCC2_GCLK_ID                27       // Index of Generic Clock
#define TCC2_OTMX                   0        // Output Matrix feature implemented
#define TCC2_OW_NUM                 2        // Number of Output Waveforms
#define TCC2_PG                     0        // Pattern Generation feature implemented
#define TCC2_SIZE                   16      
#define TCC2_SWAP                   0        // DTI outputs swap feature implemented
#define TCC2_TYPE                   0        // TCC type 0 : NA, 1 : Master, 2 : Slave

#endif /* _SAMD21_TCC2_INSTANCE_ */
