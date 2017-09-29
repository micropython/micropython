/**
 * \file
 *
 * \brief Instance description for NVMCTRL
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

#ifndef _SAMD51_NVMCTRL_INSTANCE_
#define _SAMD51_NVMCTRL_INSTANCE_

/* ========== Register definition for NVMCTRL peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_NVMCTRL_CTRLA          (0x41004000) /**< \brief (NVMCTRL) Control A */
#define REG_NVMCTRL_CTRLB          (0x41004004) /**< \brief (NVMCTRL) Control B */
#define REG_NVMCTRL_PARAM          (0x41004008) /**< \brief (NVMCTRL) NVM Parameter */
#define REG_NVMCTRL_INTENCLR       (0x4100400C) /**< \brief (NVMCTRL) Interrupt Enable Clear */
#define REG_NVMCTRL_INTENSET       (0x4100400E) /**< \brief (NVMCTRL) Interrupt Enable Set */
#define REG_NVMCTRL_INTFLAG        (0x41004010) /**< \brief (NVMCTRL) Interrupt Flag Status and Clear */
#define REG_NVMCTRL_STATUS         (0x41004012) /**< \brief (NVMCTRL) Status */
#define REG_NVMCTRL_ADDR           (0x41004014) /**< \brief (NVMCTRL) Address */
#define REG_NVMCTRL_RUNLOCK        (0x41004018) /**< \brief (NVMCTRL) Lock Section */
#define REG_NVMCTRL_PBLDATA0       (0x4100401C) /**< \brief (NVMCTRL) Page Buffer Load Data x 0 */
#define REG_NVMCTRL_PBLDATA1       (0x41004020) /**< \brief (NVMCTRL) Page Buffer Load Data x 1 */
#define REG_NVMCTRL_ECCERR         (0x41004024) /**< \brief (NVMCTRL) ECC Error Status Register */
#define REG_NVMCTRL_DBGCTRL        (0x41004028) /**< \brief (NVMCTRL) Debug Control */
#define REG_NVMCTRL_SEECFG         (0x4100402A) /**< \brief (NVMCTRL) SmartEEPROM Configuration Register */
#define REG_NVMCTRL_SEESTAT        (0x4100402C) /**< \brief (NVMCTRL) SmartEEPROM Status Register */
#else
#define REG_NVMCTRL_CTRLA          (*(RwReg16*)0x41004000UL) /**< \brief (NVMCTRL) Control A */
#define REG_NVMCTRL_CTRLB          (*(WoReg16*)0x41004004UL) /**< \brief (NVMCTRL) Control B */
#define REG_NVMCTRL_PARAM          (*(RoReg  *)0x41004008UL) /**< \brief (NVMCTRL) NVM Parameter */
#define REG_NVMCTRL_INTENCLR       (*(RwReg16*)0x4100400CUL) /**< \brief (NVMCTRL) Interrupt Enable Clear */
#define REG_NVMCTRL_INTENSET       (*(RwReg16*)0x4100400EUL) /**< \brief (NVMCTRL) Interrupt Enable Set */
#define REG_NVMCTRL_INTFLAG        (*(RwReg16*)0x41004010UL) /**< \brief (NVMCTRL) Interrupt Flag Status and Clear */
#define REG_NVMCTRL_STATUS         (*(RoReg16*)0x41004012UL) /**< \brief (NVMCTRL) Status */
#define REG_NVMCTRL_ADDR           (*(RwReg  *)0x41004014UL) /**< \brief (NVMCTRL) Address */
#define REG_NVMCTRL_RUNLOCK        (*(RoReg  *)0x41004018UL) /**< \brief (NVMCTRL) Lock Section */
#define REG_NVMCTRL_PBLDATA0       (*(RoReg  *)0x4100401CUL) /**< \brief (NVMCTRL) Page Buffer Load Data x 0 */
#define REG_NVMCTRL_PBLDATA1       (*(RoReg  *)0x41004020UL) /**< \brief (NVMCTRL) Page Buffer Load Data x 1 */
#define REG_NVMCTRL_ECCERR         (*(RoReg  *)0x41004024UL) /**< \brief (NVMCTRL) ECC Error Status Register */
#define REG_NVMCTRL_DBGCTRL        (*(RwReg8 *)0x41004028UL) /**< \brief (NVMCTRL) Debug Control */
#define REG_NVMCTRL_SEECFG         (*(RwReg8 *)0x4100402AUL) /**< \brief (NVMCTRL) SmartEEPROM Configuration Register */
#define REG_NVMCTRL_SEESTAT        (*(RoReg  *)0x4100402CUL) /**< \brief (NVMCTRL) SmartEEPROM Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for NVMCTRL peripheral ========== */
#define NVMCTRL_BLOCK_SIZE          8192     // Size Of Block (Bytes, Smallest Granularity for Erase Operation)
#define NVMCTRL_CLK_AHB_ID          6        // Index of AHB Clock in PM.AHBMASK register
#define NVMCTRL_CLK_AHB_ID_CACHE    23       // Index of AHB Clock in PM.AHBMASK register for NVMCTRL CACHE lines
#define NVMCTRL_CLK_AHB_ID_SMEEPROM 22       // Index of AHB Clock in PM.AHBMASK register for SMEE submodule
#define NVMCTRL_PAGE_SIZE           512      // Size Of Page (Bytes, Smallest Granularity for Write Operation In Main Array)

#endif /* _SAMD51_NVMCTRL_INSTANCE_ */
