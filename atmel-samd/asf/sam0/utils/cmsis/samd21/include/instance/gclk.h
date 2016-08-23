/**
 * \file
 *
 * \brief Instance description for GCLK
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

#ifndef _SAMD21_GCLK_INSTANCE_
#define _SAMD21_GCLK_INSTANCE_

/* ========== Register definition for GCLK peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_GCLK_CTRL              (0x40000C00U) /**< \brief (GCLK) Control */
#define REG_GCLK_STATUS            (0x40000C01U) /**< \brief (GCLK) Status */
#define REG_GCLK_CLKCTRL           (0x40000C02U) /**< \brief (GCLK) Generic Clock Control */
#define REG_GCLK_GENCTRL           (0x40000C04U) /**< \brief (GCLK) Generic Clock Generator Control */
#define REG_GCLK_GENDIV            (0x40000C08U) /**< \brief (GCLK) Generic Clock Generator Division */
#else
#define REG_GCLK_CTRL              (*(RwReg8 *)0x40000C00U) /**< \brief (GCLK) Control */
#define REG_GCLK_STATUS            (*(RoReg8 *)0x40000C01U) /**< \brief (GCLK) Status */
#define REG_GCLK_CLKCTRL           (*(RwReg16*)0x40000C02U) /**< \brief (GCLK) Generic Clock Control */
#define REG_GCLK_GENCTRL           (*(RwReg  *)0x40000C04U) /**< \brief (GCLK) Generic Clock Generator Control */
#define REG_GCLK_GENDIV            (*(RwReg  *)0x40000C08U) /**< \brief (GCLK) Generic Clock Generator Division */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for GCLK peripheral ========== */
#define GCLK_GENDIV_BITS            16      
#define GCLK_GEN_NUM                9        // Number of Generic Clock Generators
#define GCLK_GEN_NUM_MSB            8        // Number of Generic Clock Generators - 1
#define GCLK_GEN_SOURCE_NUM_MSB     8        // Number of Generic Clock Sources - 1
#define GCLK_NUM                    37       // Number of Generic Clock Users
#define GCLK_SOURCE_DFLL48M         7        // DFLL48M output
#define GCLK_SOURCE_FDPLL           8        // FDPLL output
#define GCLK_SOURCE_GCLKGEN1        2        // Generic clock generator 1 output
#define GCLK_SOURCE_GCLKIN          1        // Generator input pad
#define GCLK_SOURCE_NUM             9        // Number of Generic Clock Sources
#define GCLK_SOURCE_OSCULP32K       3        // OSCULP32K oscillator output
#define GCLK_SOURCE_OSC8M           6        // OSC8M oscillator output
#define GCLK_SOURCE_OSC32K          4        // OSC32K oscillator outpur
#define GCLK_SOURCE_XOSC            0        // XOSC oscillator output
#define GCLK_SOURCE_XOSC32K         5        // XOSC32K oscillator output

#endif /* _SAMD21_GCLK_INSTANCE_ */
