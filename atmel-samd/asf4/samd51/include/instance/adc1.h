/**
 * \file
 *
 * \brief Instance description for ADC1
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

#ifndef _SAMD51_ADC1_INSTANCE_
#define _SAMD51_ADC1_INSTANCE_

/* ========== Register definition for ADC1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_ADC1_CTRLA             (0x43002000) /**< \brief (ADC1) Control A */
#define REG_ADC1_EVCTRL            (0x43002002) /**< \brief (ADC1) Event Control */
#define REG_ADC1_DBGCTRL           (0x43002003) /**< \brief (ADC1) Debug Control */
#define REG_ADC1_INPUTCTRL         (0x43002004) /**< \brief (ADC1) Input Control */
#define REG_ADC1_CTRLB             (0x43002006) /**< \brief (ADC1) Control B */
#define REG_ADC1_REFCTRL           (0x43002008) /**< \brief (ADC1) Reference Control */
#define REG_ADC1_AVGCTRL           (0x4300200A) /**< \brief (ADC1) Average Control */
#define REG_ADC1_SAMPCTRL          (0x4300200B) /**< \brief (ADC1) Sample Time Control */
#define REG_ADC1_WINLT             (0x4300200C) /**< \brief (ADC1) Window Monitor Lower Threshold */
#define REG_ADC1_WINUT             (0x4300200E) /**< \brief (ADC1) Window Monitor Upper Threshold */
#define REG_ADC1_GAINCORR          (0x43002010) /**< \brief (ADC1) Gain Correction */
#define REG_ADC1_OFFSETCORR        (0x43002012) /**< \brief (ADC1) Offset Correction */
#define REG_ADC1_SWTRIG            (0x43002014) /**< \brief (ADC1) Software Trigger */
#define REG_ADC1_INTENCLR          (0x4300202C) /**< \brief (ADC1) Interrupt Enable Clear */
#define REG_ADC1_INTENSET          (0x4300202D) /**< \brief (ADC1) Interrupt Enable Set */
#define REG_ADC1_INTFLAG           (0x4300202E) /**< \brief (ADC1) Interrupt Flag Status and Clear */
#define REG_ADC1_STATUS            (0x4300202F) /**< \brief (ADC1) Status */
#define REG_ADC1_SYNCBUSY          (0x43002030) /**< \brief (ADC1) Synchronization Busy */
#define REG_ADC1_DSEQDATA          (0x43002034) /**< \brief (ADC1) DMA Sequencial Data */
#define REG_ADC1_DSEQCTRL          (0x43002038) /**< \brief (ADC1) DMA Sequential Control */
#define REG_ADC1_DSEQSTAT          (0x4300203C) /**< \brief (ADC1) DMA Sequencial Status */
#define REG_ADC1_RESULT            (0x43002040) /**< \brief (ADC1) Result Conversion Value */
#define REG_ADC1_RESS              (0x43002044) /**< \brief (ADC1) Last Sample Result */
#define REG_ADC1_CALIB             (0x43002048) /**< \brief (ADC1) Calibration */
#else
#define REG_ADC1_CTRLA             (*(RwReg16*)0x43002000UL) /**< \brief (ADC1) Control A */
#define REG_ADC1_EVCTRL            (*(RwReg8 *)0x43002002UL) /**< \brief (ADC1) Event Control */
#define REG_ADC1_DBGCTRL           (*(RwReg8 *)0x43002003UL) /**< \brief (ADC1) Debug Control */
#define REG_ADC1_INPUTCTRL         (*(RwReg16*)0x43002004UL) /**< \brief (ADC1) Input Control */
#define REG_ADC1_CTRLB             (*(RwReg16*)0x43002006UL) /**< \brief (ADC1) Control B */
#define REG_ADC1_REFCTRL           (*(RwReg8 *)0x43002008UL) /**< \brief (ADC1) Reference Control */
#define REG_ADC1_AVGCTRL           (*(RwReg8 *)0x4300200AUL) /**< \brief (ADC1) Average Control */
#define REG_ADC1_SAMPCTRL          (*(RwReg8 *)0x4300200BUL) /**< \brief (ADC1) Sample Time Control */
#define REG_ADC1_WINLT             (*(RwReg16*)0x4300200CUL) /**< \brief (ADC1) Window Monitor Lower Threshold */
#define REG_ADC1_WINUT             (*(RwReg16*)0x4300200EUL) /**< \brief (ADC1) Window Monitor Upper Threshold */
#define REG_ADC1_GAINCORR          (*(RwReg16*)0x43002010UL) /**< \brief (ADC1) Gain Correction */
#define REG_ADC1_OFFSETCORR        (*(RwReg16*)0x43002012UL) /**< \brief (ADC1) Offset Correction */
#define REG_ADC1_SWTRIG            (*(RwReg8 *)0x43002014UL) /**< \brief (ADC1) Software Trigger */
#define REG_ADC1_INTENCLR          (*(RwReg8 *)0x4300202CUL) /**< \brief (ADC1) Interrupt Enable Clear */
#define REG_ADC1_INTENSET          (*(RwReg8 *)0x4300202DUL) /**< \brief (ADC1) Interrupt Enable Set */
#define REG_ADC1_INTFLAG           (*(RwReg8 *)0x4300202EUL) /**< \brief (ADC1) Interrupt Flag Status and Clear */
#define REG_ADC1_STATUS            (*(RoReg8 *)0x4300202FUL) /**< \brief (ADC1) Status */
#define REG_ADC1_SYNCBUSY          (*(RoReg  *)0x43002030UL) /**< \brief (ADC1) Synchronization Busy */
#define REG_ADC1_DSEQDATA          (*(WoReg  *)0x43002034UL) /**< \brief (ADC1) DMA Sequencial Data */
#define REG_ADC1_DSEQCTRL          (*(RwReg  *)0x43002038UL) /**< \brief (ADC1) DMA Sequential Control */
#define REG_ADC1_DSEQSTAT          (*(RoReg  *)0x4300203CUL) /**< \brief (ADC1) DMA Sequencial Status */
#define REG_ADC1_RESULT            (*(RoReg16*)0x43002040UL) /**< \brief (ADC1) Result Conversion Value */
#define REG_ADC1_RESS              (*(RoReg16*)0x43002044UL) /**< \brief (ADC1) Last Sample Result */
#define REG_ADC1_CALIB             (*(RwReg16*)0x43002048UL) /**< \brief (ADC1) Calibration */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for ADC1 peripheral ========== */
#define ADC1_BANDGAP                27       // MUXPOS value to select BANDGAP
#define ADC1_CTAT                   29       // MUXPOS value to select CTAT
#define ADC1_DMAC_ID_RESRDY         70       // Index of DMA RESRDY trigger
#define ADC1_DMAC_ID_SEQ            71       // Index of DMA SEQ trigger
#define ADC1_EXTCHANNEL_MSB         15       // Number of external channels
#define ADC1_GCLK_ID                41       // Index of Generic Clock
#define ADC1_MASTER_SLAVE_MODE      2        // ADC Master/Slave Mode
#define ADC1_OPAMP2                 0        // MUXPOS value to select OPAMP2
#define ADC1_OPAMP01                0        // MUXPOS value to select OPAMP01
#define ADC1_PTAT                   28       // MUXPOS value to select PTAT
#define ADC1_TOUCH_IMPLEMENTED      0        // TOUCH implemented or not
#define ADC1_TOUCH_LINES_NUM        1        // Number of touch lines

#endif /* _SAMD51_ADC1_INSTANCE_ */
