/**
 * \file
 *
 * \brief Instance description for GCLK
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

#ifndef _SAMD21_GCLK_INSTANCE_
#define _SAMD21_GCLK_INSTANCE_

/* ========== Register definition for GCLK peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_GCLK_CTRL              (0x40000C00) /**< \brief (GCLK) Control */
#define REG_GCLK_STATUS            (0x40000C01) /**< \brief (GCLK) Status */
#define REG_GCLK_CLKCTRL           (0x40000C02) /**< \brief (GCLK) Generic Clock Control */
#define REG_GCLK_GENCTRL           (0x40000C04) /**< \brief (GCLK) Generic Clock Generator Control */
#define REG_GCLK_GENDIV            (0x40000C08) /**< \brief (GCLK) Generic Clock Generator Division */
#else
#define REG_GCLK_CTRL              (*(RwReg8 *)0x40000C00UL) /**< \brief (GCLK) Control */
#define REG_GCLK_STATUS            (*(RoReg8 *)0x40000C01UL) /**< \brief (GCLK) Status */
#define REG_GCLK_CLKCTRL           (*(RwReg16*)0x40000C02UL) /**< \brief (GCLK) Generic Clock Control */
#define REG_GCLK_GENCTRL           (*(RwReg  *)0x40000C04UL) /**< \brief (GCLK) Generic Clock Generator Control */
#define REG_GCLK_GENDIV            (*(RwReg  *)0x40000C08UL) /**< \brief (GCLK) Generic Clock Generator Division */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for GCLK peripheral ========== */
#define GCLK_GENDIV_BITS            16      
#define GCLK_GEN_NUM                9        // Number of Generic Clock Generators
#define GCLK_GEN_NUM_MSB            8        // Number of Generic Clock Generators - 1
#define GCLK_GEN_SOURCE_NUM_MSB     8        // Number of Generic Clock Sources - 1
#define GCLK_NUM                    37       // Number of Generic Clock Users
#define GCLK_SOURCE_DFLL48M         7       
#define GCLK_SOURCE_FDPLL           8       
#define GCLK_SOURCE_GCLKGEN1        2       
#define GCLK_SOURCE_GCLKIN          1       
#define GCLK_SOURCE_NUM             9        // Number of Generic Clock Sources
#define GCLK_SOURCE_OSCULP32K       3       
#define GCLK_SOURCE_OSC8M           6       
#define GCLK_SOURCE_OSC32K          4       
#define GCLK_SOURCE_XOSC            0       
#define GCLK_SOURCE_XOSC32K         5       

#endif /* _SAMD21_GCLK_INSTANCE_ */
