/**
 * \file
 *
 * \brief Instance description for RSTC
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

#ifndef _SAMD51_RSTC_INSTANCE_
#define _SAMD51_RSTC_INSTANCE_

/* ========== Register definition for RSTC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_RSTC_RCAUSE            (0x40000C00) /**< \brief (RSTC) Reset Cause */
#define REG_RSTC_BKUPEXIT          (0x40000C02) /**< \brief (RSTC) Backup Exit Source */
#else
#define REG_RSTC_RCAUSE            (*(RoReg8 *)0x40000C00UL) /**< \brief (RSTC) Reset Cause */
#define REG_RSTC_BKUPEXIT          (*(RoReg8 *)0x40000C02UL) /**< \brief (RSTC) Backup Exit Source */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for RSTC peripheral ========== */
#define RSTC_BACKUP_IMPLEMENTED     1       
#define RSTC_HIB_IMPLEMENTED        1       
#define RSTC_NUMBER_OF_EXTWAKE      0        // number of external wakeup line
#define RSTC_NVMRST_IMPLEMENTED     1       

#endif /* _SAMD51_RSTC_INSTANCE_ */
