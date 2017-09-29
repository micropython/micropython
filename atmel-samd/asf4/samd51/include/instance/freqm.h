/**
 * \file
 *
 * \brief Instance description for FREQM
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

#ifndef _SAMD51_FREQM_INSTANCE_
#define _SAMD51_FREQM_INSTANCE_

/* ========== Register definition for FREQM peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_FREQM_CTRLA            (0x40002C00) /**< \brief (FREQM) Control A Register */
#define REG_FREQM_CTRLB            (0x40002C01) /**< \brief (FREQM) Control B Register */
#define REG_FREQM_CFGA             (0x40002C02) /**< \brief (FREQM) Config A register */
#define REG_FREQM_INTENCLR         (0x40002C08) /**< \brief (FREQM) Interrupt Enable Clear Register */
#define REG_FREQM_INTENSET         (0x40002C09) /**< \brief (FREQM) Interrupt Enable Set Register */
#define REG_FREQM_INTFLAG          (0x40002C0A) /**< \brief (FREQM) Interrupt Flag Register */
#define REG_FREQM_STATUS           (0x40002C0B) /**< \brief (FREQM) Status Register */
#define REG_FREQM_SYNCBUSY         (0x40002C0C) /**< \brief (FREQM) Synchronization Busy Register */
#define REG_FREQM_VALUE            (0x40002C10) /**< \brief (FREQM) Count Value Register */
#else
#define REG_FREQM_CTRLA            (*(RwReg8 *)0x40002C00UL) /**< \brief (FREQM) Control A Register */
#define REG_FREQM_CTRLB            (*(WoReg8 *)0x40002C01UL) /**< \brief (FREQM) Control B Register */
#define REG_FREQM_CFGA             (*(RwReg16*)0x40002C02UL) /**< \brief (FREQM) Config A register */
#define REG_FREQM_INTENCLR         (*(RwReg8 *)0x40002C08UL) /**< \brief (FREQM) Interrupt Enable Clear Register */
#define REG_FREQM_INTENSET         (*(RwReg8 *)0x40002C09UL) /**< \brief (FREQM) Interrupt Enable Set Register */
#define REG_FREQM_INTFLAG          (*(RwReg8 *)0x40002C0AUL) /**< \brief (FREQM) Interrupt Flag Register */
#define REG_FREQM_STATUS           (*(RwReg8 *)0x40002C0BUL) /**< \brief (FREQM) Status Register */
#define REG_FREQM_SYNCBUSY         (*(RoReg  *)0x40002C0CUL) /**< \brief (FREQM) Synchronization Busy Register */
#define REG_FREQM_VALUE            (*(RoReg  *)0x40002C10UL) /**< \brief (FREQM) Count Value Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for FREQM peripheral ========== */
#define FREQM_GCLK_ID_MSR           5        // Index of measure generic clock

#endif /* _SAMD51_FREQM_INSTANCE_ */
