/**
 * \file
 *
 * \brief Instance description for TCC1
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

#ifndef _SAMD51_TCC1_INSTANCE_
#define _SAMD51_TCC1_INSTANCE_

/* ========== Register definition for TCC1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TCC1_CTRLA             (0x41018000) /**< \brief (TCC1) Control A */
#define REG_TCC1_CTRLBCLR          (0x41018004) /**< \brief (TCC1) Control B Clear */
#define REG_TCC1_CTRLBSET          (0x41018005) /**< \brief (TCC1) Control B Set */
#define REG_TCC1_SYNCBUSY          (0x41018008) /**< \brief (TCC1) Synchronization Busy */
#define REG_TCC1_FCTRLA            (0x4101800C) /**< \brief (TCC1) Recoverable Fault A Configuration */
#define REG_TCC1_FCTRLB            (0x41018010) /**< \brief (TCC1) Recoverable Fault B Configuration */
#define REG_TCC1_WEXCTRL           (0x41018014) /**< \brief (TCC1) Waveform Extension Configuration */
#define REG_TCC1_DRVCTRL           (0x41018018) /**< \brief (TCC1) Driver Control */
#define REG_TCC1_DBGCTRL           (0x4101801E) /**< \brief (TCC1) Debug Control */
#define REG_TCC1_EVCTRL            (0x41018020) /**< \brief (TCC1) Event Control */
#define REG_TCC1_INTENCLR          (0x41018024) /**< \brief (TCC1) Interrupt Enable Clear */
#define REG_TCC1_INTENSET          (0x41018028) /**< \brief (TCC1) Interrupt Enable Set */
#define REG_TCC1_INTFLAG           (0x4101802C) /**< \brief (TCC1) Interrupt Flag Status and Clear */
#define REG_TCC1_STATUS            (0x41018030) /**< \brief (TCC1) Status */
#define REG_TCC1_COUNT             (0x41018034) /**< \brief (TCC1) Count */
#define REG_TCC1_PATT              (0x41018038) /**< \brief (TCC1) Pattern */
#define REG_TCC1_WAVE              (0x4101803C) /**< \brief (TCC1) Waveform Control */
#define REG_TCC1_PER               (0x41018040) /**< \brief (TCC1) Period */
#define REG_TCC1_CC0               (0x41018044) /**< \brief (TCC1) Compare and Capture 0 */
#define REG_TCC1_CC1               (0x41018048) /**< \brief (TCC1) Compare and Capture 1 */
#define REG_TCC1_CC2               (0x4101804C) /**< \brief (TCC1) Compare and Capture 2 */
#define REG_TCC1_CC3               (0x41018050) /**< \brief (TCC1) Compare and Capture 3 */
#define REG_TCC1_PATTBUF           (0x41018064) /**< \brief (TCC1) Pattern Buffer */
#define REG_TCC1_PERBUF            (0x4101806C) /**< \brief (TCC1) Period Buffer */
#define REG_TCC1_CCBUF0            (0x41018070) /**< \brief (TCC1) Compare and Capture Buffer 0 */
#define REG_TCC1_CCBUF1            (0x41018074) /**< \brief (TCC1) Compare and Capture Buffer 1 */
#define REG_TCC1_CCBUF2            (0x41018078) /**< \brief (TCC1) Compare and Capture Buffer 2 */
#define REG_TCC1_CCBUF3            (0x4101807C) /**< \brief (TCC1) Compare and Capture Buffer 3 */
#else
#define REG_TCC1_CTRLA             (*(RwReg  *)0x41018000UL) /**< \brief (TCC1) Control A */
#define REG_TCC1_CTRLBCLR          (*(RwReg8 *)0x41018004UL) /**< \brief (TCC1) Control B Clear */
#define REG_TCC1_CTRLBSET          (*(RwReg8 *)0x41018005UL) /**< \brief (TCC1) Control B Set */
#define REG_TCC1_SYNCBUSY          (*(RoReg  *)0x41018008UL) /**< \brief (TCC1) Synchronization Busy */
#define REG_TCC1_FCTRLA            (*(RwReg  *)0x4101800CUL) /**< \brief (TCC1) Recoverable Fault A Configuration */
#define REG_TCC1_FCTRLB            (*(RwReg  *)0x41018010UL) /**< \brief (TCC1) Recoverable Fault B Configuration */
#define REG_TCC1_WEXCTRL           (*(RwReg  *)0x41018014UL) /**< \brief (TCC1) Waveform Extension Configuration */
#define REG_TCC1_DRVCTRL           (*(RwReg  *)0x41018018UL) /**< \brief (TCC1) Driver Control */
#define REG_TCC1_DBGCTRL           (*(RwReg8 *)0x4101801EUL) /**< \brief (TCC1) Debug Control */
#define REG_TCC1_EVCTRL            (*(RwReg  *)0x41018020UL) /**< \brief (TCC1) Event Control */
#define REG_TCC1_INTENCLR          (*(RwReg  *)0x41018024UL) /**< \brief (TCC1) Interrupt Enable Clear */
#define REG_TCC1_INTENSET          (*(RwReg  *)0x41018028UL) /**< \brief (TCC1) Interrupt Enable Set */
#define REG_TCC1_INTFLAG           (*(RwReg  *)0x4101802CUL) /**< \brief (TCC1) Interrupt Flag Status and Clear */
#define REG_TCC1_STATUS            (*(RwReg  *)0x41018030UL) /**< \brief (TCC1) Status */
#define REG_TCC1_COUNT             (*(RwReg  *)0x41018034UL) /**< \brief (TCC1) Count */
#define REG_TCC1_PATT              (*(RwReg16*)0x41018038UL) /**< \brief (TCC1) Pattern */
#define REG_TCC1_WAVE              (*(RwReg  *)0x4101803CUL) /**< \brief (TCC1) Waveform Control */
#define REG_TCC1_PER               (*(RwReg  *)0x41018040UL) /**< \brief (TCC1) Period */
#define REG_TCC1_CC0               (*(RwReg  *)0x41018044UL) /**< \brief (TCC1) Compare and Capture 0 */
#define REG_TCC1_CC1               (*(RwReg  *)0x41018048UL) /**< \brief (TCC1) Compare and Capture 1 */
#define REG_TCC1_CC2               (*(RwReg  *)0x4101804CUL) /**< \brief (TCC1) Compare and Capture 2 */
#define REG_TCC1_CC3               (*(RwReg  *)0x41018050UL) /**< \brief (TCC1) Compare and Capture 3 */
#define REG_TCC1_PATTBUF           (*(RwReg16*)0x41018064UL) /**< \brief (TCC1) Pattern Buffer */
#define REG_TCC1_PERBUF            (*(RwReg  *)0x4101806CUL) /**< \brief (TCC1) Period Buffer */
#define REG_TCC1_CCBUF0            (*(RwReg  *)0x41018070UL) /**< \brief (TCC1) Compare and Capture Buffer 0 */
#define REG_TCC1_CCBUF1            (*(RwReg  *)0x41018074UL) /**< \brief (TCC1) Compare and Capture Buffer 1 */
#define REG_TCC1_CCBUF2            (*(RwReg  *)0x41018078UL) /**< \brief (TCC1) Compare and Capture Buffer 2 */
#define REG_TCC1_CCBUF3            (*(RwReg  *)0x4101807CUL) /**< \brief (TCC1) Compare and Capture Buffer 3 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TCC1 peripheral ========== */
#define TCC1_CC_NUM                 4        // Number of Compare/Capture units
#define TCC1_DITHERING              1        // Dithering feature implemented
#define TCC1_DMAC_ID_MC_0           30
#define TCC1_DMAC_ID_MC_1           31
#define TCC1_DMAC_ID_MC_2           32
#define TCC1_DMAC_ID_MC_3           33
#define TCC1_DMAC_ID_MC_LSB         30
#define TCC1_DMAC_ID_MC_MSB         33
#define TCC1_DMAC_ID_MC_SIZE        4
#define TCC1_DMAC_ID_OVF            29       // DMA overflow/underflow/retrigger trigger
#define TCC1_DTI                    1        // Dead-Time-Insertion feature implemented
#define TCC1_EXT                    31       // Coding of implemented extended features
#define TCC1_GCLK_ID                25       // Index of Generic Clock
#define TCC1_MASTER_SLAVE_MODE      2        // TCC type 0 : NA, 1 : Master, 2 : Slave
#define TCC1_OTMX                   1        // Output Matrix feature implemented
#define TCC1_OW_NUM                 8        // Number of Output Waveforms
#define TCC1_PG                     1        // Pattern Generation feature implemented
#define TCC1_SIZE                   24      
#define TCC1_SWAP                   1        // DTI outputs swap feature implemented

#endif /* _SAMD51_TCC1_INSTANCE_ */
