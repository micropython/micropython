/**
 * \file
 *
 * \brief Instance description for PM
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD21_PM_INSTANCE_
#define _SAMD21_PM_INSTANCE_

/* ========== Register definition for PM peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PM_CTRL                (0x40000400U) /**< \brief (PM) Control */
#define REG_PM_SLEEP               (0x40000401U) /**< \brief (PM) Sleep Mode */
#define REG_PM_EXTCTRL             (0x40000402U) /**< \brief (PM) External Reset Controller */
#define REG_PM_CPUSEL              (0x40000408U) /**< \brief (PM) CPU Clock Select */
#define REG_PM_APBASEL             (0x40000409U) /**< \brief (PM) APBA Clock Select */
#define REG_PM_APBBSEL             (0x4000040AU) /**< \brief (PM) APBB Clock Select */
#define REG_PM_APBCSEL             (0x4000040BU) /**< \brief (PM) APBC Clock Select */
#define REG_PM_AHBMASK             (0x40000414U) /**< \brief (PM) AHB Mask */
#define REG_PM_APBAMASK            (0x40000418U) /**< \brief (PM) APBA Mask */
#define REG_PM_APBBMASK            (0x4000041CU) /**< \brief (PM) APBB Mask */
#define REG_PM_APBCMASK            (0x40000420U) /**< \brief (PM) APBC Mask */
#define REG_PM_INTENCLR            (0x40000434U) /**< \brief (PM) Interrupt Enable Clear */
#define REG_PM_INTENSET            (0x40000435U) /**< \brief (PM) Interrupt Enable Set */
#define REG_PM_INTFLAG             (0x40000436U) /**< \brief (PM) Interrupt Flag Status and Clear */
#define REG_PM_RCAUSE              (0x40000438U) /**< \brief (PM) Reset Cause */
#else
#define REG_PM_CTRL                (*(RwReg8 *)0x40000400U) /**< \brief (PM) Control */
#define REG_PM_SLEEP               (*(RwReg8 *)0x40000401U) /**< \brief (PM) Sleep Mode */
#define REG_PM_EXTCTRL             (*(RwReg8 *)0x40000402U) /**< \brief (PM) External Reset Controller */
#define REG_PM_CPUSEL              (*(RwReg8 *)0x40000408U) /**< \brief (PM) CPU Clock Select */
#define REG_PM_APBASEL             (*(RwReg8 *)0x40000409U) /**< \brief (PM) APBA Clock Select */
#define REG_PM_APBBSEL             (*(RwReg8 *)0x4000040AU) /**< \brief (PM) APBB Clock Select */
#define REG_PM_APBCSEL             (*(RwReg8 *)0x4000040BU) /**< \brief (PM) APBC Clock Select */
#define REG_PM_AHBMASK             (*(RwReg  *)0x40000414U) /**< \brief (PM) AHB Mask */
#define REG_PM_APBAMASK            (*(RwReg  *)0x40000418U) /**< \brief (PM) APBA Mask */
#define REG_PM_APBBMASK            (*(RwReg  *)0x4000041CU) /**< \brief (PM) APBB Mask */
#define REG_PM_APBCMASK            (*(RwReg  *)0x40000420U) /**< \brief (PM) APBC Mask */
#define REG_PM_INTENCLR            (*(RwReg8 *)0x40000434U) /**< \brief (PM) Interrupt Enable Clear */
#define REG_PM_INTENSET            (*(RwReg8 *)0x40000435U) /**< \brief (PM) Interrupt Enable Set */
#define REG_PM_INTFLAG             (*(RwReg8 *)0x40000436U) /**< \brief (PM) Interrupt Flag Status and Clear */
#define REG_PM_RCAUSE              (*(RoReg8 *)0x40000438U) /**< \brief (PM) Reset Cause */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for PM peripheral ========== */
#define PM_CTRL_MCSEL_DFLL48M       3       
#define PM_CTRL_MCSEL_GCLK          0       
#define PM_CTRL_MCSEL_OSC8M         1       
#define PM_CTRL_MCSEL_XOSC          2       
#define PM_PM_CLK_APB_NUM           2       

#endif /* _SAMD21_PM_INSTANCE_ */
