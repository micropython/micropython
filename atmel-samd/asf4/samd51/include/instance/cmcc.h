/**
 * \file
 *
 * \brief Instance description for CMCC
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

#ifndef _SAMD51_CMCC_INSTANCE_
#define _SAMD51_CMCC_INSTANCE_

/* ========== Register definition for CMCC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_CMCC_TYPE              (0x41006000) /**< \brief (CMCC) Cache Type Register */
#define REG_CMCC_CFG               (0x41006004) /**< \brief (CMCC) Cache Configuration Register */
#define REG_CMCC_CTRL              (0x41006008) /**< \brief (CMCC) Cache Control Register */
#define REG_CMCC_SR                (0x4100600C) /**< \brief (CMCC) Cache Status Register */
#define REG_CMCC_LCKWAY            (0x41006010) /**< \brief (CMCC) Cache Lock per Way Register */
#define REG_CMCC_MAINT0            (0x41006020) /**< \brief (CMCC) Cache Maintenance Register 0 */
#define REG_CMCC_MAINT1            (0x41006024) /**< \brief (CMCC) Cache Maintenance Register 1 */
#define REG_CMCC_MCFG              (0x41006028) /**< \brief (CMCC) Cache Monitor Configuration Register */
#define REG_CMCC_MEN               (0x4100602C) /**< \brief (CMCC) Cache Monitor Enable Register */
#define REG_CMCC_MCTRL             (0x41006030) /**< \brief (CMCC) Cache Monitor Control Register */
#define REG_CMCC_MSR               (0x41006034) /**< \brief (CMCC) Cache Monitor Status Register */
#else
#define REG_CMCC_TYPE              (*(RoReg  *)0x41006000UL) /**< \brief (CMCC) Cache Type Register */
#define REG_CMCC_CFG               (*(RwReg  *)0x41006004UL) /**< \brief (CMCC) Cache Configuration Register */
#define REG_CMCC_CTRL              (*(WoReg  *)0x41006008UL) /**< \brief (CMCC) Cache Control Register */
#define REG_CMCC_SR                (*(RoReg  *)0x4100600CUL) /**< \brief (CMCC) Cache Status Register */
#define REG_CMCC_LCKWAY            (*(RwReg  *)0x41006010UL) /**< \brief (CMCC) Cache Lock per Way Register */
#define REG_CMCC_MAINT0            (*(WoReg  *)0x41006020UL) /**< \brief (CMCC) Cache Maintenance Register 0 */
#define REG_CMCC_MAINT1            (*(WoReg  *)0x41006024UL) /**< \brief (CMCC) Cache Maintenance Register 1 */
#define REG_CMCC_MCFG              (*(RwReg  *)0x41006028UL) /**< \brief (CMCC) Cache Monitor Configuration Register */
#define REG_CMCC_MEN               (*(RwReg  *)0x4100602CUL) /**< \brief (CMCC) Cache Monitor Enable Register */
#define REG_CMCC_MCTRL             (*(WoReg  *)0x41006030UL) /**< \brief (CMCC) Cache Monitor Control Register */
#define REG_CMCC_MSR               (*(RoReg  *)0x41006034UL) /**< \brief (CMCC) Cache Monitor Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */


#endif /* _SAMD51_CMCC_INSTANCE_ */
