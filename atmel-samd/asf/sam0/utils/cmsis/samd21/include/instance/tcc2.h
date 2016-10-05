/**
 * \file
 *
 * \brief Instance description for TCC2
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD21_TCC2_INSTANCE_
#define _SAMD21_TCC2_INSTANCE_

/* ========== Register definition for TCC2 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TCC2_CTRLA             (0x42002800U) /**< \brief (TCC2) Control A */
#define REG_TCC2_CTRLBCLR          (0x42002804U) /**< \brief (TCC2) Control B Clear */
#define REG_TCC2_CTRLBSET          (0x42002805U) /**< \brief (TCC2) Control B Set */
#define REG_TCC2_SYNCBUSY          (0x42002808U) /**< \brief (TCC2) Synchronization Busy */
#define REG_TCC2_FCTRLA            (0x4200280CU) /**< \brief (TCC2) Recoverable Fault A Configuration */
#define REG_TCC2_FCTRLB            (0x42002810U) /**< \brief (TCC2) Recoverable Fault B Configuration */
#define REG_TCC2_DRVCTRL           (0x42002818U) /**< \brief (TCC2) Driver Control */
#define REG_TCC2_DBGCTRL           (0x4200281EU) /**< \brief (TCC2) Debug Control */
#define REG_TCC2_EVCTRL            (0x42002820U) /**< \brief (TCC2) Event Control */
#define REG_TCC2_INTENCLR          (0x42002824U) /**< \brief (TCC2) Interrupt Enable Clear */
#define REG_TCC2_INTENSET          (0x42002828U) /**< \brief (TCC2) Interrupt Enable Set */
#define REG_TCC2_INTFLAG           (0x4200282CU) /**< \brief (TCC2) Interrupt Flag Status and Clear */
#define REG_TCC2_STATUS            (0x42002830U) /**< \brief (TCC2) Status */
#define REG_TCC2_COUNT             (0x42002834U) /**< \brief (TCC2) Count */
#define REG_TCC2_WAVE              (0x4200283CU) /**< \brief (TCC2) Waveform Control */
#define REG_TCC2_PER               (0x42002840U) /**< \brief (TCC2) Period */
#define REG_TCC2_CC0               (0x42002844U) /**< \brief (TCC2) Compare and Capture 0 */
#define REG_TCC2_CC1               (0x42002848U) /**< \brief (TCC2) Compare and Capture 1 */
#define REG_TCC2_WAVEB             (0x42002868U) /**< \brief (TCC2) Waveform Control Buffer */
#define REG_TCC2_PERB              (0x4200286CU) /**< \brief (TCC2) Period Buffer */
#define REG_TCC2_CCB0              (0x42002870U) /**< \brief (TCC2) Compare and Capture Buffer 0 */
#define REG_TCC2_CCB1              (0x42002874U) /**< \brief (TCC2) Compare and Capture Buffer 1 */
#else
#define REG_TCC2_CTRLA             (*(RwReg  *)0x42002800U) /**< \brief (TCC2) Control A */
#define REG_TCC2_CTRLBCLR          (*(RwReg8 *)0x42002804U) /**< \brief (TCC2) Control B Clear */
#define REG_TCC2_CTRLBSET          (*(RwReg8 *)0x42002805U) /**< \brief (TCC2) Control B Set */
#define REG_TCC2_SYNCBUSY          (*(RoReg  *)0x42002808U) /**< \brief (TCC2) Synchronization Busy */
#define REG_TCC2_FCTRLA            (*(RwReg  *)0x4200280CU) /**< \brief (TCC2) Recoverable Fault A Configuration */
#define REG_TCC2_FCTRLB            (*(RwReg  *)0x42002810U) /**< \brief (TCC2) Recoverable Fault B Configuration */
#define REG_TCC2_DRVCTRL           (*(RwReg  *)0x42002818U) /**< \brief (TCC2) Driver Control */
#define REG_TCC2_DBGCTRL           (*(RwReg8 *)0x4200281EU) /**< \brief (TCC2) Debug Control */
#define REG_TCC2_EVCTRL            (*(RwReg  *)0x42002820U) /**< \brief (TCC2) Event Control */
#define REG_TCC2_INTENCLR          (*(RwReg  *)0x42002824U) /**< \brief (TCC2) Interrupt Enable Clear */
#define REG_TCC2_INTENSET          (*(RwReg  *)0x42002828U) /**< \brief (TCC2) Interrupt Enable Set */
#define REG_TCC2_INTFLAG           (*(RwReg  *)0x4200282CU) /**< \brief (TCC2) Interrupt Flag Status and Clear */
#define REG_TCC2_STATUS            (*(RwReg  *)0x42002830U) /**< \brief (TCC2) Status */
#define REG_TCC2_COUNT             (*(RwReg  *)0x42002834U) /**< \brief (TCC2) Count */
#define REG_TCC2_WAVE              (*(RwReg  *)0x4200283CU) /**< \brief (TCC2) Waveform Control */
#define REG_TCC2_PER               (*(RwReg  *)0x42002840U) /**< \brief (TCC2) Period */
#define REG_TCC2_CC0               (*(RwReg  *)0x42002844U) /**< \brief (TCC2) Compare and Capture 0 */
#define REG_TCC2_CC1               (*(RwReg  *)0x42002848U) /**< \brief (TCC2) Compare and Capture 1 */
#define REG_TCC2_WAVEB             (*(RwReg  *)0x42002868U) /**< \brief (TCC2) Waveform Control Buffer */
#define REG_TCC2_PERB              (*(RwReg  *)0x4200286CU) /**< \brief (TCC2) Period Buffer */
#define REG_TCC2_CCB0              (*(RwReg  *)0x42002870U) /**< \brief (TCC2) Compare and Capture Buffer 0 */
#define REG_TCC2_CCB1              (*(RwReg  *)0x42002874U) /**< \brief (TCC2) Compare and Capture Buffer 1 */
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
#define TCC2_MASTER                 0       
#define TCC2_OTMX                   0        // Output Matrix feature implemented
#define TCC2_OW_NUM                 2        // Number of Output Waveforms
#define TCC2_PG                     0        // Pattern Generation feature implemented
#define TCC2_SIZE                   16      
#define TCC2_SWAP                   0        // DTI outputs swap feature implemented
#define TCC2_TYPE                   0        // TCC type 0 : NA, 1 : Master, 2 : Slave

#endif /* _SAMD21_TCC2_INSTANCE_ */
