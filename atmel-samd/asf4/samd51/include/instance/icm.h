/**
 * \file
 *
 * \brief Instance description for ICM
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

#ifndef _SAMD51_ICM_INSTANCE_
#define _SAMD51_ICM_INSTANCE_

/* ========== Register definition for ICM peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_ICM_CFG                (0x42002C00) /**< \brief (ICM) Configuration */
#define REG_ICM_CTRL               (0x42002C04) /**< \brief (ICM) Control */
#define REG_ICM_SR                 (0x42002C08) /**< \brief (ICM) Status */
#define REG_ICM_IER                (0x42002C10) /**< \brief (ICM) Interrupt Enable */
#define REG_ICM_IDR                (0x42002C14) /**< \brief (ICM) Interrupt Disable */
#define REG_ICM_IMR                (0x42002C18) /**< \brief (ICM) Interrupt Mask */
#define REG_ICM_ISR                (0x42002C1C) /**< \brief (ICM) Interrupt Status */
#define REG_ICM_UASR               (0x42002C20) /**< \brief (ICM) Undefined Access Status */
#define REG_ICM_DSCR               (0x42002C30) /**< \brief (ICM) Region Descriptor Area Start Address */
#define REG_ICM_HASH               (0x42002C34) /**< \brief (ICM) Region Hash Area Start Address */
#define REG_ICM_UIHVAL0            (0x42002C38) /**< \brief (ICM) User Initial Hash Value 0 */
#define REG_ICM_UIHVAL1            (0x42002C3C) /**< \brief (ICM) User Initial Hash Value 1 */
#define REG_ICM_UIHVAL2            (0x42002C40) /**< \brief (ICM) User Initial Hash Value 2 */
#define REG_ICM_UIHVAL3            (0x42002C44) /**< \brief (ICM) User Initial Hash Value 3 */
#define REG_ICM_UIHVAL4            (0x42002C48) /**< \brief (ICM) User Initial Hash Value 4 */
#define REG_ICM_UIHVAL5            (0x42002C4C) /**< \brief (ICM) User Initial Hash Value 5 */
#define REG_ICM_UIHVAL6            (0x42002C50) /**< \brief (ICM) User Initial Hash Value 6 */
#define REG_ICM_UIHVAL7            (0x42002C54) /**< \brief (ICM) User Initial Hash Value 7 */
#else
#define REG_ICM_CFG                (*(RwReg  *)0x42002C00UL) /**< \brief (ICM) Configuration */
#define REG_ICM_CTRL               (*(WoReg  *)0x42002C04UL) /**< \brief (ICM) Control */
#define REG_ICM_SR                 (*(RoReg  *)0x42002C08UL) /**< \brief (ICM) Status */
#define REG_ICM_IER                (*(WoReg  *)0x42002C10UL) /**< \brief (ICM) Interrupt Enable */
#define REG_ICM_IDR                (*(WoReg  *)0x42002C14UL) /**< \brief (ICM) Interrupt Disable */
#define REG_ICM_IMR                (*(RoReg  *)0x42002C18UL) /**< \brief (ICM) Interrupt Mask */
#define REG_ICM_ISR                (*(RoReg  *)0x42002C1CUL) /**< \brief (ICM) Interrupt Status */
#define REG_ICM_UASR               (*(RoReg  *)0x42002C20UL) /**< \brief (ICM) Undefined Access Status */
#define REG_ICM_DSCR               (*(RwReg  *)0x42002C30UL) /**< \brief (ICM) Region Descriptor Area Start Address */
#define REG_ICM_HASH               (*(RwReg  *)0x42002C34UL) /**< \brief (ICM) Region Hash Area Start Address */
#define REG_ICM_UIHVAL0            (*(WoReg  *)0x42002C38UL) /**< \brief (ICM) User Initial Hash Value 0 */
#define REG_ICM_UIHVAL1            (*(WoReg  *)0x42002C3CUL) /**< \brief (ICM) User Initial Hash Value 1 */
#define REG_ICM_UIHVAL2            (*(WoReg  *)0x42002C40UL) /**< \brief (ICM) User Initial Hash Value 2 */
#define REG_ICM_UIHVAL3            (*(WoReg  *)0x42002C44UL) /**< \brief (ICM) User Initial Hash Value 3 */
#define REG_ICM_UIHVAL4            (*(WoReg  *)0x42002C48UL) /**< \brief (ICM) User Initial Hash Value 4 */
#define REG_ICM_UIHVAL5            (*(WoReg  *)0x42002C4CUL) /**< \brief (ICM) User Initial Hash Value 5 */
#define REG_ICM_UIHVAL6            (*(WoReg  *)0x42002C50UL) /**< \brief (ICM) User Initial Hash Value 6 */
#define REG_ICM_UIHVAL7            (*(WoReg  *)0x42002C54UL) /**< \brief (ICM) User Initial Hash Value 7 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for ICM peripheral ========== */
#define ICM_CLK_AHB_ID              19      

#endif /* _SAMD51_ICM_INSTANCE_ */
