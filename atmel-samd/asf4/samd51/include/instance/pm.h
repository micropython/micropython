/**
 * \file
 *
 * \brief Instance description for PM
 *
 * Copyright (c) 2017 Atmel Corporation,
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

#ifndef _SAMD51_PM_INSTANCE_
#define _SAMD51_PM_INSTANCE_

/* ========== Register definition for PM peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PM_CTRLA               (0x40000400) /**< \brief (PM) Control A */
#define REG_PM_SLEEPCFG            (0x40000401) /**< \brief (PM) Sleep Configuration */
#define REG_PM_INTENCLR            (0x40000404) /**< \brief (PM) Interrupt Enable Clear */
#define REG_PM_INTENSET            (0x40000405) /**< \brief (PM) Interrupt Enable Set */
#define REG_PM_INTFLAG             (0x40000406) /**< \brief (PM) Interrupt Flag Status and Clear */
#define REG_PM_STDBYCFG            (0x40000408) /**< \brief (PM) Standby Configuration */
#define REG_PM_HIBCFG              (0x40000409) /**< \brief (PM) Hibernate Configuration */
#define REG_PM_BKUPCFG             (0x4000040A) /**< \brief (PM) Backup Configuration */
#define REG_PM_PWSAKDLY            (0x40000412) /**< \brief (PM) Power Switch Acknowledge Delay */
#else
#define REG_PM_CTRLA               (*(RwReg8 *)0x40000400UL) /**< \brief (PM) Control A */
#define REG_PM_SLEEPCFG            (*(RwReg8 *)0x40000401UL) /**< \brief (PM) Sleep Configuration */
#define REG_PM_INTENCLR            (*(RwReg8 *)0x40000404UL) /**< \brief (PM) Interrupt Enable Clear */
#define REG_PM_INTENSET            (*(RwReg8 *)0x40000405UL) /**< \brief (PM) Interrupt Enable Set */
#define REG_PM_INTFLAG             (*(RwReg8 *)0x40000406UL) /**< \brief (PM) Interrupt Flag Status and Clear */
#define REG_PM_STDBYCFG            (*(RwReg8 *)0x40000408UL) /**< \brief (PM) Standby Configuration */
#define REG_PM_HIBCFG              (*(RwReg8 *)0x40000409UL) /**< \brief (PM) Hibernate Configuration */
#define REG_PM_BKUPCFG             (*(RwReg8 *)0x4000040AUL) /**< \brief (PM) Backup Configuration */
#define REG_PM_PWSAKDLY            (*(RwReg8 *)0x40000412UL) /**< \brief (PM) Power Switch Acknowledge Delay */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for PM peripheral ========== */
#define PM_PD_NUM                   0        // Number of switchable Power Domains

#endif /* _SAMD51_PM_INSTANCE_ */
