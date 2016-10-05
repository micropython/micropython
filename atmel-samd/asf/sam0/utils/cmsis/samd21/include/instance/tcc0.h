/**
 * \file
 *
 * \brief Instance description for TCC0
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

#ifndef _SAMD21_TCC0_INSTANCE_
#define _SAMD21_TCC0_INSTANCE_

/* ========== Register definition for TCC0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TCC0_CTRLA             (0x42002000U) /**< \brief (TCC0) Control A */
#define REG_TCC0_CTRLBCLR          (0x42002004U) /**< \brief (TCC0) Control B Clear */
#define REG_TCC0_CTRLBSET          (0x42002005U) /**< \brief (TCC0) Control B Set */
#define REG_TCC0_SYNCBUSY          (0x42002008U) /**< \brief (TCC0) Synchronization Busy */
#define REG_TCC0_FCTRLA            (0x4200200CU) /**< \brief (TCC0) Recoverable Fault A Configuration */
#define REG_TCC0_FCTRLB            (0x42002010U) /**< \brief (TCC0) Recoverable Fault B Configuration */
#define REG_TCC0_WEXCTRL           (0x42002014U) /**< \brief (TCC0) Waveform Extension Configuration */
#define REG_TCC0_DRVCTRL           (0x42002018U) /**< \brief (TCC0) Driver Control */
#define REG_TCC0_DBGCTRL           (0x4200201EU) /**< \brief (TCC0) Debug Control */
#define REG_TCC0_EVCTRL            (0x42002020U) /**< \brief (TCC0) Event Control */
#define REG_TCC0_INTENCLR          (0x42002024U) /**< \brief (TCC0) Interrupt Enable Clear */
#define REG_TCC0_INTENSET          (0x42002028U) /**< \brief (TCC0) Interrupt Enable Set */
#define REG_TCC0_INTFLAG           (0x4200202CU) /**< \brief (TCC0) Interrupt Flag Status and Clear */
#define REG_TCC0_STATUS            (0x42002030U) /**< \brief (TCC0) Status */
#define REG_TCC0_COUNT             (0x42002034U) /**< \brief (TCC0) Count */
#define REG_TCC0_PATT              (0x42002038U) /**< \brief (TCC0) Pattern */
#define REG_TCC0_WAVE              (0x4200203CU) /**< \brief (TCC0) Waveform Control */
#define REG_TCC0_PER               (0x42002040U) /**< \brief (TCC0) Period */
#define REG_TCC0_CC0               (0x42002044U) /**< \brief (TCC0) Compare and Capture 0 */
#define REG_TCC0_CC1               (0x42002048U) /**< \brief (TCC0) Compare and Capture 1 */
#define REG_TCC0_CC2               (0x4200204CU) /**< \brief (TCC0) Compare and Capture 2 */
#define REG_TCC0_CC3               (0x42002050U) /**< \brief (TCC0) Compare and Capture 3 */
#define REG_TCC0_PATTB             (0x42002064U) /**< \brief (TCC0) Pattern Buffer */
#define REG_TCC0_WAVEB             (0x42002068U) /**< \brief (TCC0) Waveform Control Buffer */
#define REG_TCC0_PERB              (0x4200206CU) /**< \brief (TCC0) Period Buffer */
#define REG_TCC0_CCB0              (0x42002070U) /**< \brief (TCC0) Compare and Capture Buffer 0 */
#define REG_TCC0_CCB1              (0x42002074U) /**< \brief (TCC0) Compare and Capture Buffer 1 */
#define REG_TCC0_CCB2              (0x42002078U) /**< \brief (TCC0) Compare and Capture Buffer 2 */
#define REG_TCC0_CCB3              (0x4200207CU) /**< \brief (TCC0) Compare and Capture Buffer 3 */
#else
#define REG_TCC0_CTRLA             (*(RwReg  *)0x42002000U) /**< \brief (TCC0) Control A */
#define REG_TCC0_CTRLBCLR          (*(RwReg8 *)0x42002004U) /**< \brief (TCC0) Control B Clear */
#define REG_TCC0_CTRLBSET          (*(RwReg8 *)0x42002005U) /**< \brief (TCC0) Control B Set */
#define REG_TCC0_SYNCBUSY          (*(RoReg  *)0x42002008U) /**< \brief (TCC0) Synchronization Busy */
#define REG_TCC0_FCTRLA            (*(RwReg  *)0x4200200CU) /**< \brief (TCC0) Recoverable Fault A Configuration */
#define REG_TCC0_FCTRLB            (*(RwReg  *)0x42002010U) /**< \brief (TCC0) Recoverable Fault B Configuration */
#define REG_TCC0_WEXCTRL           (*(RwReg  *)0x42002014U) /**< \brief (TCC0) Waveform Extension Configuration */
#define REG_TCC0_DRVCTRL           (*(RwReg  *)0x42002018U) /**< \brief (TCC0) Driver Control */
#define REG_TCC0_DBGCTRL           (*(RwReg8 *)0x4200201EU) /**< \brief (TCC0) Debug Control */
#define REG_TCC0_EVCTRL            (*(RwReg  *)0x42002020U) /**< \brief (TCC0) Event Control */
#define REG_TCC0_INTENCLR          (*(RwReg  *)0x42002024U) /**< \brief (TCC0) Interrupt Enable Clear */
#define REG_TCC0_INTENSET          (*(RwReg  *)0x42002028U) /**< \brief (TCC0) Interrupt Enable Set */
#define REG_TCC0_INTFLAG           (*(RwReg  *)0x4200202CU) /**< \brief (TCC0) Interrupt Flag Status and Clear */
#define REG_TCC0_STATUS            (*(RwReg  *)0x42002030U) /**< \brief (TCC0) Status */
#define REG_TCC0_COUNT             (*(RwReg  *)0x42002034U) /**< \brief (TCC0) Count */
#define REG_TCC0_PATT              (*(RwReg16*)0x42002038U) /**< \brief (TCC0) Pattern */
#define REG_TCC0_WAVE              (*(RwReg  *)0x4200203CU) /**< \brief (TCC0) Waveform Control */
#define REG_TCC0_PER               (*(RwReg  *)0x42002040U) /**< \brief (TCC0) Period */
#define REG_TCC0_CC0               (*(RwReg  *)0x42002044U) /**< \brief (TCC0) Compare and Capture 0 */
#define REG_TCC0_CC1               (*(RwReg  *)0x42002048U) /**< \brief (TCC0) Compare and Capture 1 */
#define REG_TCC0_CC2               (*(RwReg  *)0x4200204CU) /**< \brief (TCC0) Compare and Capture 2 */
#define REG_TCC0_CC3               (*(RwReg  *)0x42002050U) /**< \brief (TCC0) Compare and Capture 3 */
#define REG_TCC0_PATTB             (*(RwReg16*)0x42002064U) /**< \brief (TCC0) Pattern Buffer */
#define REG_TCC0_WAVEB             (*(RwReg  *)0x42002068U) /**< \brief (TCC0) Waveform Control Buffer */
#define REG_TCC0_PERB              (*(RwReg  *)0x4200206CU) /**< \brief (TCC0) Period Buffer */
#define REG_TCC0_CCB0              (*(RwReg  *)0x42002070U) /**< \brief (TCC0) Compare and Capture Buffer 0 */
#define REG_TCC0_CCB1              (*(RwReg  *)0x42002074U) /**< \brief (TCC0) Compare and Capture Buffer 1 */
#define REG_TCC0_CCB2              (*(RwReg  *)0x42002078U) /**< \brief (TCC0) Compare and Capture Buffer 2 */
#define REG_TCC0_CCB3              (*(RwReg  *)0x4200207CU) /**< \brief (TCC0) Compare and Capture Buffer 3 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TCC0 peripheral ========== */
#define TCC0_CC_NUM                 4        // Number of Compare/Capture units
#define TCC0_DITHERING              1        // Dithering feature implemented
#define TCC0_DMAC_ID_MC_0           14
#define TCC0_DMAC_ID_MC_1           15
#define TCC0_DMAC_ID_MC_2           16
#define TCC0_DMAC_ID_MC_3           17
#define TCC0_DMAC_ID_MC_LSB         14
#define TCC0_DMAC_ID_MC_MSB         17
#define TCC0_DMAC_ID_MC_SIZE        4
#define TCC0_DMAC_ID_OVF            13       // DMA overflow/underflow/retrigger trigger
#define TCC0_DTI                    1        // Dead-Time-Insertion feature implemented
#define TCC0_EXT                    31       // (@_DITHERING*16+@_PG*8+@_SWAP*4+@_DTI*2+@_OTMX*1)
#define TCC0_GCLK_ID                26       // Index of Generic Clock
#define TCC0_MASTER                 0       
#define TCC0_OTMX                   1        // Output Matrix feature implemented
#define TCC0_OW_NUM                 8        // Number of Output Waveforms
#define TCC0_PG                     1        // Pattern Generation feature implemented
#define TCC0_SIZE                   24      
#define TCC0_SWAP                   1        // DTI outputs swap feature implemented
#define TCC0_TYPE                   1        // TCC type 0 : NA, 1 : Master, 2 : Slave

#endif /* _SAMD21_TCC0_INSTANCE_ */
