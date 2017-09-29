/**
 * \file
 *
 * \brief Instance description for SUPC
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

#ifndef _SAMD51_SUPC_INSTANCE_
#define _SAMD51_SUPC_INSTANCE_

/* ========== Register definition for SUPC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SUPC_INTENCLR          (0x40001800) /**< \brief (SUPC) Interrupt Enable Clear */
#define REG_SUPC_INTENSET          (0x40001804) /**< \brief (SUPC) Interrupt Enable Set */
#define REG_SUPC_INTFLAG           (0x40001808) /**< \brief (SUPC) Interrupt Flag Status and Clear */
#define REG_SUPC_STATUS            (0x4000180C) /**< \brief (SUPC) Power and Clocks Status */
#define REG_SUPC_BOD33             (0x40001810) /**< \brief (SUPC) BOD33 Control */
#define REG_SUPC_BOD12             (0x40001814) /**< \brief (SUPC) BOD12 Control */
#define REG_SUPC_VREG              (0x40001818) /**< \brief (SUPC) VREG Control */
#define REG_SUPC_VREF              (0x4000181C) /**< \brief (SUPC) VREF Control */
#define REG_SUPC_BBPS              (0x40001820) /**< \brief (SUPC) Battery Backup Power Switch */
#define REG_SUPC_BKOUT             (0x40001824) /**< \brief (SUPC) Backup Output Control */
#define REG_SUPC_BKIN              (0x40001828) /**< \brief (SUPC) Backup Input Control */
#else
#define REG_SUPC_INTENCLR          (*(RwReg  *)0x40001800UL) /**< \brief (SUPC) Interrupt Enable Clear */
#define REG_SUPC_INTENSET          (*(RwReg  *)0x40001804UL) /**< \brief (SUPC) Interrupt Enable Set */
#define REG_SUPC_INTFLAG           (*(RwReg  *)0x40001808UL) /**< \brief (SUPC) Interrupt Flag Status and Clear */
#define REG_SUPC_STATUS            (*(RoReg  *)0x4000180CUL) /**< \brief (SUPC) Power and Clocks Status */
#define REG_SUPC_BOD33             (*(RwReg  *)0x40001810UL) /**< \brief (SUPC) BOD33 Control */
#define REG_SUPC_BOD12             (*(RwReg  *)0x40001814UL) /**< \brief (SUPC) BOD12 Control */
#define REG_SUPC_VREG              (*(RwReg  *)0x40001818UL) /**< \brief (SUPC) VREG Control */
#define REG_SUPC_VREF              (*(RwReg  *)0x4000181CUL) /**< \brief (SUPC) VREF Control */
#define REG_SUPC_BBPS              (*(RwReg  *)0x40001820UL) /**< \brief (SUPC) Battery Backup Power Switch */
#define REG_SUPC_BKOUT             (*(RwReg  *)0x40001824UL) /**< \brief (SUPC) Backup Output Control */
#define REG_SUPC_BKIN              (*(RoReg  *)0x40001828UL) /**< \brief (SUPC) Backup Input Control */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SUPC peripheral ========== */
#define SUPC_BOD12_CALIB_MSB        5       
#define SUPC_BOD33_CALIB_MSB        5       

#endif /* _SAMD51_SUPC_INSTANCE_ */
