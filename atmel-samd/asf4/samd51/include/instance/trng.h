/**
 * \file
 *
 * \brief Instance description for TRNG
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

#ifndef _SAMD51_TRNG_INSTANCE_
#define _SAMD51_TRNG_INSTANCE_

/* ========== Register definition for TRNG peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TRNG_CTRLA             (0x42002800) /**< \brief (TRNG) Control A */
#define REG_TRNG_EVCTRL            (0x42002804) /**< \brief (TRNG) Event Control */
#define REG_TRNG_INTENCLR          (0x42002808) /**< \brief (TRNG) Interrupt Enable Clear */
#define REG_TRNG_INTENSET          (0x42002809) /**< \brief (TRNG) Interrupt Enable Set */
#define REG_TRNG_INTFLAG           (0x4200280A) /**< \brief (TRNG) Interrupt Flag Status and Clear */
#define REG_TRNG_DATA              (0x42002820) /**< \brief (TRNG) Output Data */
#else
#define REG_TRNG_CTRLA             (*(RwReg8 *)0x42002800UL) /**< \brief (TRNG) Control A */
#define REG_TRNG_EVCTRL            (*(RwReg8 *)0x42002804UL) /**< \brief (TRNG) Event Control */
#define REG_TRNG_INTENCLR          (*(RwReg8 *)0x42002808UL) /**< \brief (TRNG) Interrupt Enable Clear */
#define REG_TRNG_INTENSET          (*(RwReg8 *)0x42002809UL) /**< \brief (TRNG) Interrupt Enable Set */
#define REG_TRNG_INTFLAG           (*(RwReg8 *)0x4200280AUL) /**< \brief (TRNG) Interrupt Flag Status and Clear */
#define REG_TRNG_DATA              (*(RoReg  *)0x42002820UL) /**< \brief (TRNG) Output Data */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */


#endif /* _SAMD51_TRNG_INSTANCE_ */
