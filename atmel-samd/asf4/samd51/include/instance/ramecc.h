/**
 * \file
 *
 * \brief Instance description for RAMECC
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

#ifndef _SAMD51_RAMECC_INSTANCE_
#define _SAMD51_RAMECC_INSTANCE_

/* ========== Register definition for RAMECC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_RAMECC_INTENCLR        (0x41020000) /**< \brief (RAMECC) Interrupt Enable Clear */
#define REG_RAMECC_INTENSET        (0x41020001) /**< \brief (RAMECC) Interrupt Enable Set */
#define REG_RAMECC_INTFLAG         (0x41020002) /**< \brief (RAMECC) Interrupt Flag */
#define REG_RAMECC_STATUS          (0x41020003) /**< \brief (RAMECC) Status */
#define REG_RAMECC_ERRADDR         (0x41020004) /**< \brief (RAMECC) Error Address */
#define REG_RAMECC_DBGCTRL         (0x4102000F) /**< \brief (RAMECC) Debug Control */
#else
#define REG_RAMECC_INTENCLR        (*(RwReg8 *)0x41020000UL) /**< \brief (RAMECC) Interrupt Enable Clear */
#define REG_RAMECC_INTENSET        (*(RwReg8 *)0x41020001UL) /**< \brief (RAMECC) Interrupt Enable Set */
#define REG_RAMECC_INTFLAG         (*(RwReg8 *)0x41020002UL) /**< \brief (RAMECC) Interrupt Flag */
#define REG_RAMECC_STATUS          (*(RoReg8 *)0x41020003UL) /**< \brief (RAMECC) Status */
#define REG_RAMECC_ERRADDR         (*(RoReg  *)0x41020004UL) /**< \brief (RAMECC) Error Address */
#define REG_RAMECC_DBGCTRL         (*(RwReg8 *)0x4102000FUL) /**< \brief (RAMECC) Debug Control */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for RAMECC peripheral ========== */
#define RAMECC_RAMADDR_BITS         13       // Number of RAM address bits
#define RAMECC_RAMBANK_NUM          4        // Number of RAM banks

#endif /* _SAMD51_RAMECC_INSTANCE_ */
