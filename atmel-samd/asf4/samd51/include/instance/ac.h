/**
 * \file
 *
 * \brief Instance description for AC
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

#ifndef _SAMD51_AC_INSTANCE_
#define _SAMD51_AC_INSTANCE_

/* ========== Register definition for AC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_AC_CTRLA               (0x42002000) /**< \brief (AC) Control A */
#define REG_AC_CTRLB               (0x42002001) /**< \brief (AC) Control B */
#define REG_AC_EVCTRL              (0x42002002) /**< \brief (AC) Event Control */
#define REG_AC_INTENCLR            (0x42002004) /**< \brief (AC) Interrupt Enable Clear */
#define REG_AC_INTENSET            (0x42002005) /**< \brief (AC) Interrupt Enable Set */
#define REG_AC_INTFLAG             (0x42002006) /**< \brief (AC) Interrupt Flag Status and Clear */
#define REG_AC_STATUSA             (0x42002007) /**< \brief (AC) Status A */
#define REG_AC_STATUSB             (0x42002008) /**< \brief (AC) Status B */
#define REG_AC_DBGCTRL             (0x42002009) /**< \brief (AC) Debug Control */
#define REG_AC_WINCTRL             (0x4200200A) /**< \brief (AC) Window Control */
#define REG_AC_SCALER0             (0x4200200C) /**< \brief (AC) Scaler 0 */
#define REG_AC_SCALER1             (0x4200200D) /**< \brief (AC) Scaler 1 */
#define REG_AC_COMPCTRL0           (0x42002010) /**< \brief (AC) Comparator Control 0 */
#define REG_AC_COMPCTRL1           (0x42002014) /**< \brief (AC) Comparator Control 1 */
#define REG_AC_SYNCBUSY            (0x42002020) /**< \brief (AC) Synchronization Busy */
#define REG_AC_CALIB               (0x42002024) /**< \brief (AC) Calibration */
#else
#define REG_AC_CTRLA               (*(RwReg8 *)0x42002000UL) /**< \brief (AC) Control A */
#define REG_AC_CTRLB               (*(WoReg8 *)0x42002001UL) /**< \brief (AC) Control B */
#define REG_AC_EVCTRL              (*(RwReg16*)0x42002002UL) /**< \brief (AC) Event Control */
#define REG_AC_INTENCLR            (*(RwReg8 *)0x42002004UL) /**< \brief (AC) Interrupt Enable Clear */
#define REG_AC_INTENSET            (*(RwReg8 *)0x42002005UL) /**< \brief (AC) Interrupt Enable Set */
#define REG_AC_INTFLAG             (*(RwReg8 *)0x42002006UL) /**< \brief (AC) Interrupt Flag Status and Clear */
#define REG_AC_STATUSA             (*(RoReg8 *)0x42002007UL) /**< \brief (AC) Status A */
#define REG_AC_STATUSB             (*(RoReg8 *)0x42002008UL) /**< \brief (AC) Status B */
#define REG_AC_DBGCTRL             (*(RwReg8 *)0x42002009UL) /**< \brief (AC) Debug Control */
#define REG_AC_WINCTRL             (*(RwReg8 *)0x4200200AUL) /**< \brief (AC) Window Control */
#define REG_AC_SCALER0             (*(RwReg8 *)0x4200200CUL) /**< \brief (AC) Scaler 0 */
#define REG_AC_SCALER1             (*(RwReg8 *)0x4200200DUL) /**< \brief (AC) Scaler 1 */
#define REG_AC_COMPCTRL0           (*(RwReg  *)0x42002010UL) /**< \brief (AC) Comparator Control 0 */
#define REG_AC_COMPCTRL1           (*(RwReg  *)0x42002014UL) /**< \brief (AC) Comparator Control 1 */
#define REG_AC_SYNCBUSY            (*(RoReg  *)0x42002020UL) /**< \brief (AC) Synchronization Busy */
#define REG_AC_CALIB               (*(RwReg16*)0x42002024UL) /**< \brief (AC) Calibration */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for AC peripheral ========== */
#define AC_COMPCTRL_MUXNEG_OPAMP    7        // OPAMP selection for MUXNEG
#define AC_FUSES_BIAS1                       // PAIR1 Bias Calibration
#define AC_GCLK_ID                  32       // Index of Generic Clock
#define AC_IMPLEMENTS_VDBLR         0        // VDoubler implemented ?
#define AC_NUM_CMP                  2        // Number of comparators
#define AC_PAIRS                    1        // Number of pairs of comparators
#define AC_SPEED_LEVELS             2        // Number of speed values

#endif /* _SAMD51_AC_INSTANCE_ */
