/**
 * \file
 *
 * \brief Instance description for PM
 *
 * Copyright (c) 2016 Atmel Corporation,
 *                    a wholly owned subsidiary of Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD21_PM_INSTANCE_
#define _SAMD21_PM_INSTANCE_

/* ========== Register definition for PM peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PM_CTRL                (0x40000400) /**< \brief (PM) Control */
#define REG_PM_SLEEP               (0x40000401) /**< \brief (PM) Sleep Mode */
#define REG_PM_CPUSEL              (0x40000408) /**< \brief (PM) CPU Clock Select */
#define REG_PM_APBASEL             (0x40000409) /**< \brief (PM) APBA Clock Select */
#define REG_PM_APBBSEL             (0x4000040A) /**< \brief (PM) APBB Clock Select */
#define REG_PM_APBCSEL             (0x4000040B) /**< \brief (PM) APBC Clock Select */
#define REG_PM_AHBMASK             (0x40000414) /**< \brief (PM) AHB Mask */
#define REG_PM_APBAMASK            (0x40000418) /**< \brief (PM) APBA Mask */
#define REG_PM_APBBMASK            (0x4000041C) /**< \brief (PM) APBB Mask */
#define REG_PM_APBCMASK            (0x40000420) /**< \brief (PM) APBC Mask */
#define REG_PM_INTENCLR            (0x40000434) /**< \brief (PM) Interrupt Enable Clear */
#define REG_PM_INTENSET            (0x40000435) /**< \brief (PM) Interrupt Enable Set */
#define REG_PM_INTFLAG             (0x40000436) /**< \brief (PM) Interrupt Flag Status and Clear */
#define REG_PM_RCAUSE              (0x40000438) /**< \brief (PM) Reset Cause */
#else
#define REG_PM_CTRL                (*(RwReg8 *)0x40000400UL) /**< \brief (PM) Control */
#define REG_PM_SLEEP               (*(RwReg8 *)0x40000401UL) /**< \brief (PM) Sleep Mode */
#define REG_PM_CPUSEL              (*(RwReg8 *)0x40000408UL) /**< \brief (PM) CPU Clock Select */
#define REG_PM_APBASEL             (*(RwReg8 *)0x40000409UL) /**< \brief (PM) APBA Clock Select */
#define REG_PM_APBBSEL             (*(RwReg8 *)0x4000040AUL) /**< \brief (PM) APBB Clock Select */
#define REG_PM_APBCSEL             (*(RwReg8 *)0x4000040BUL) /**< \brief (PM) APBC Clock Select */
#define REG_PM_AHBMASK             (*(RwReg  *)0x40000414UL) /**< \brief (PM) AHB Mask */
#define REG_PM_APBAMASK            (*(RwReg  *)0x40000418UL) /**< \brief (PM) APBA Mask */
#define REG_PM_APBBMASK            (*(RwReg  *)0x4000041CUL) /**< \brief (PM) APBB Mask */
#define REG_PM_APBCMASK            (*(RwReg  *)0x40000420UL) /**< \brief (PM) APBC Mask */
#define REG_PM_INTENCLR            (*(RwReg8 *)0x40000434UL) /**< \brief (PM) Interrupt Enable Clear */
#define REG_PM_INTENSET            (*(RwReg8 *)0x40000435UL) /**< \brief (PM) Interrupt Enable Set */
#define REG_PM_INTFLAG             (*(RwReg8 *)0x40000436UL) /**< \brief (PM) Interrupt Flag Status and Clear */
#define REG_PM_RCAUSE              (*(RoReg8 *)0x40000438UL) /**< \brief (PM) Reset Cause */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for PM peripheral ========== */
#define PM_CTRL_MCSEL_DFLL48M       3       
#define PM_CTRL_MCSEL_GCLK          0       
#define PM_CTRL_MCSEL_OSC8M         1       
#define PM_CTRL_MCSEL_XOSC          2       
#define PM_PM_CLK_APB_NUM           2       

#endif /* _SAMD21_PM_INSTANCE_ */
