/**
 * \file
 *
 * \brief Instance description for PORT
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

#ifndef _SAMD51_PORT_INSTANCE_
#define _SAMD51_PORT_INSTANCE_

/* ========== Register definition for PORT peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PORT_DIR0              (0x41008000) /**< \brief (PORT) Data Direction 0 */
#define REG_PORT_DIRCLR0           (0x41008004) /**< \brief (PORT) Data Direction Clear 0 */
#define REG_PORT_DIRSET0           (0x41008008) /**< \brief (PORT) Data Direction Set 0 */
#define REG_PORT_DIRTGL0           (0x4100800C) /**< \brief (PORT) Data Direction Toggle 0 */
#define REG_PORT_OUT0              (0x41008010) /**< \brief (PORT) Data Output Value 0 */
#define REG_PORT_OUTCLR0           (0x41008014) /**< \brief (PORT) Data Output Value Clear 0 */
#define REG_PORT_OUTSET0           (0x41008018) /**< \brief (PORT) Data Output Value Set 0 */
#define REG_PORT_OUTTGL0           (0x4100801C) /**< \brief (PORT) Data Output Value Toggle 0 */
#define REG_PORT_IN0               (0x41008020) /**< \brief (PORT) Data Input Value 0 */
#define REG_PORT_CTRL0             (0x41008024) /**< \brief (PORT) Control 0 */
#define REG_PORT_WRCONFIG0         (0x41008028) /**< \brief (PORT) Write Configuration 0 */
#define REG_PORT_EVCTRL0           (0x4100802C) /**< \brief (PORT) Event Input Control 0 */
#define REG_PORT_PMUX0             (0x41008030) /**< \brief (PORT) Peripheral Multiplexing 0 */
#define REG_PORT_PINCFG0           (0x41008040) /**< \brief (PORT) Pin Configuration 0 */
#define REG_PORT_DIR1              (0x41008080) /**< \brief (PORT) Data Direction 1 */
#define REG_PORT_DIRCLR1           (0x41008084) /**< \brief (PORT) Data Direction Clear 1 */
#define REG_PORT_DIRSET1           (0x41008088) /**< \brief (PORT) Data Direction Set 1 */
#define REG_PORT_DIRTGL1           (0x4100808C) /**< \brief (PORT) Data Direction Toggle 1 */
#define REG_PORT_OUT1              (0x41008090) /**< \brief (PORT) Data Output Value 1 */
#define REG_PORT_OUTCLR1           (0x41008094) /**< \brief (PORT) Data Output Value Clear 1 */
#define REG_PORT_OUTSET1           (0x41008098) /**< \brief (PORT) Data Output Value Set 1 */
#define REG_PORT_OUTTGL1           (0x4100809C) /**< \brief (PORT) Data Output Value Toggle 1 */
#define REG_PORT_IN1               (0x410080A0) /**< \brief (PORT) Data Input Value 1 */
#define REG_PORT_CTRL1             (0x410080A4) /**< \brief (PORT) Control 1 */
#define REG_PORT_WRCONFIG1         (0x410080A8) /**< \brief (PORT) Write Configuration 1 */
#define REG_PORT_EVCTRL1           (0x410080AC) /**< \brief (PORT) Event Input Control 1 */
#define REG_PORT_PMUX1             (0x410080B0) /**< \brief (PORT) Peripheral Multiplexing 1 */
#define REG_PORT_PINCFG1           (0x410080C0) /**< \brief (PORT) Pin Configuration 1 */
#define REG_PORT_DIR2              (0x41008100) /**< \brief (PORT) Data Direction 2 */
#define REG_PORT_DIRCLR2           (0x41008104) /**< \brief (PORT) Data Direction Clear 2 */
#define REG_PORT_DIRSET2           (0x41008108) /**< \brief (PORT) Data Direction Set 2 */
#define REG_PORT_DIRTGL2           (0x4100810C) /**< \brief (PORT) Data Direction Toggle 2 */
#define REG_PORT_OUT2              (0x41008110) /**< \brief (PORT) Data Output Value 2 */
#define REG_PORT_OUTCLR2           (0x41008114) /**< \brief (PORT) Data Output Value Clear 2 */
#define REG_PORT_OUTSET2           (0x41008118) /**< \brief (PORT) Data Output Value Set 2 */
#define REG_PORT_OUTTGL2           (0x4100811C) /**< \brief (PORT) Data Output Value Toggle 2 */
#define REG_PORT_IN2               (0x41008120) /**< \brief (PORT) Data Input Value 2 */
#define REG_PORT_CTRL2             (0x41008124) /**< \brief (PORT) Control 2 */
#define REG_PORT_WRCONFIG2         (0x41008128) /**< \brief (PORT) Write Configuration 2 */
#define REG_PORT_EVCTRL2           (0x4100812C) /**< \brief (PORT) Event Input Control 2 */
#define REG_PORT_PMUX2             (0x41008130) /**< \brief (PORT) Peripheral Multiplexing 2 */
#define REG_PORT_PINCFG2           (0x41008140) /**< \brief (PORT) Pin Configuration 2 */
#define REG_PORT_DIR3              (0x41008180) /**< \brief (PORT) Data Direction 3 */
#define REG_PORT_DIRCLR3           (0x41008184) /**< \brief (PORT) Data Direction Clear 3 */
#define REG_PORT_DIRSET3           (0x41008188) /**< \brief (PORT) Data Direction Set 3 */
#define REG_PORT_DIRTGL3           (0x4100818C) /**< \brief (PORT) Data Direction Toggle 3 */
#define REG_PORT_OUT3              (0x41008190) /**< \brief (PORT) Data Output Value 3 */
#define REG_PORT_OUTCLR3           (0x41008194) /**< \brief (PORT) Data Output Value Clear 3 */
#define REG_PORT_OUTSET3           (0x41008198) /**< \brief (PORT) Data Output Value Set 3 */
#define REG_PORT_OUTTGL3           (0x4100819C) /**< \brief (PORT) Data Output Value Toggle 3 */
#define REG_PORT_IN3               (0x410081A0) /**< \brief (PORT) Data Input Value 3 */
#define REG_PORT_CTRL3             (0x410081A4) /**< \brief (PORT) Control 3 */
#define REG_PORT_WRCONFIG3         (0x410081A8) /**< \brief (PORT) Write Configuration 3 */
#define REG_PORT_EVCTRL3           (0x410081AC) /**< \brief (PORT) Event Input Control 3 */
#define REG_PORT_PMUX3             (0x410081B0) /**< \brief (PORT) Peripheral Multiplexing 3 */
#define REG_PORT_PINCFG3           (0x410081C0) /**< \brief (PORT) Pin Configuration 3 */
#else
#define REG_PORT_DIR0              (*(RwReg  *)0x41008000UL) /**< \brief (PORT) Data Direction 0 */
#define REG_PORT_DIRCLR0           (*(RwReg  *)0x41008004UL) /**< \brief (PORT) Data Direction Clear 0 */
#define REG_PORT_DIRSET0           (*(RwReg  *)0x41008008UL) /**< \brief (PORT) Data Direction Set 0 */
#define REG_PORT_DIRTGL0           (*(RwReg  *)0x4100800CUL) /**< \brief (PORT) Data Direction Toggle 0 */
#define REG_PORT_OUT0              (*(RwReg  *)0x41008010UL) /**< \brief (PORT) Data Output Value 0 */
#define REG_PORT_OUTCLR0           (*(RwReg  *)0x41008014UL) /**< \brief (PORT) Data Output Value Clear 0 */
#define REG_PORT_OUTSET0           (*(RwReg  *)0x41008018UL) /**< \brief (PORT) Data Output Value Set 0 */
#define REG_PORT_OUTTGL0           (*(RwReg  *)0x4100801CUL) /**< \brief (PORT) Data Output Value Toggle 0 */
#define REG_PORT_IN0               (*(RoReg  *)0x41008020UL) /**< \brief (PORT) Data Input Value 0 */
#define REG_PORT_CTRL0             (*(RwReg  *)0x41008024UL) /**< \brief (PORT) Control 0 */
#define REG_PORT_WRCONFIG0         (*(WoReg  *)0x41008028UL) /**< \brief (PORT) Write Configuration 0 */
#define REG_PORT_EVCTRL0           (*(RwReg  *)0x4100802CUL) /**< \brief (PORT) Event Input Control 0 */
#define REG_PORT_PMUX0             (*(RwReg  *)0x41008030UL) /**< \brief (PORT) Peripheral Multiplexing 0 */
#define REG_PORT_PINCFG0           (*(RwReg  *)0x41008040UL) /**< \brief (PORT) Pin Configuration 0 */
#define REG_PORT_DIR1              (*(RwReg  *)0x41008080UL) /**< \brief (PORT) Data Direction 1 */
#define REG_PORT_DIRCLR1           (*(RwReg  *)0x41008084UL) /**< \brief (PORT) Data Direction Clear 1 */
#define REG_PORT_DIRSET1           (*(RwReg  *)0x41008088UL) /**< \brief (PORT) Data Direction Set 1 */
#define REG_PORT_DIRTGL1           (*(RwReg  *)0x4100808CUL) /**< \brief (PORT) Data Direction Toggle 1 */
#define REG_PORT_OUT1              (*(RwReg  *)0x41008090UL) /**< \brief (PORT) Data Output Value 1 */
#define REG_PORT_OUTCLR1           (*(RwReg  *)0x41008094UL) /**< \brief (PORT) Data Output Value Clear 1 */
#define REG_PORT_OUTSET1           (*(RwReg  *)0x41008098UL) /**< \brief (PORT) Data Output Value Set 1 */
#define REG_PORT_OUTTGL1           (*(RwReg  *)0x4100809CUL) /**< \brief (PORT) Data Output Value Toggle 1 */
#define REG_PORT_IN1               (*(RoReg  *)0x410080A0UL) /**< \brief (PORT) Data Input Value 1 */
#define REG_PORT_CTRL1             (*(RwReg  *)0x410080A4UL) /**< \brief (PORT) Control 1 */
#define REG_PORT_WRCONFIG1         (*(WoReg  *)0x410080A8UL) /**< \brief (PORT) Write Configuration 1 */
#define REG_PORT_EVCTRL1           (*(RwReg  *)0x410080ACUL) /**< \brief (PORT) Event Input Control 1 */
#define REG_PORT_PMUX1             (*(RwReg  *)0x410080B0UL) /**< \brief (PORT) Peripheral Multiplexing 1 */
#define REG_PORT_PINCFG1           (*(RwReg  *)0x410080C0UL) /**< \brief (PORT) Pin Configuration 1 */
#define REG_PORT_DIR2              (*(RwReg  *)0x41008100UL) /**< \brief (PORT) Data Direction 2 */
#define REG_PORT_DIRCLR2           (*(RwReg  *)0x41008104UL) /**< \brief (PORT) Data Direction Clear 2 */
#define REG_PORT_DIRSET2           (*(RwReg  *)0x41008108UL) /**< \brief (PORT) Data Direction Set 2 */
#define REG_PORT_DIRTGL2           (*(RwReg  *)0x4100810CUL) /**< \brief (PORT) Data Direction Toggle 2 */
#define REG_PORT_OUT2              (*(RwReg  *)0x41008110UL) /**< \brief (PORT) Data Output Value 2 */
#define REG_PORT_OUTCLR2           (*(RwReg  *)0x41008114UL) /**< \brief (PORT) Data Output Value Clear 2 */
#define REG_PORT_OUTSET2           (*(RwReg  *)0x41008118UL) /**< \brief (PORT) Data Output Value Set 2 */
#define REG_PORT_OUTTGL2           (*(RwReg  *)0x4100811CUL) /**< \brief (PORT) Data Output Value Toggle 2 */
#define REG_PORT_IN2               (*(RoReg  *)0x41008120UL) /**< \brief (PORT) Data Input Value 2 */
#define REG_PORT_CTRL2             (*(RwReg  *)0x41008124UL) /**< \brief (PORT) Control 2 */
#define REG_PORT_WRCONFIG2         (*(WoReg  *)0x41008128UL) /**< \brief (PORT) Write Configuration 2 */
#define REG_PORT_EVCTRL2           (*(RwReg  *)0x4100812CUL) /**< \brief (PORT) Event Input Control 2 */
#define REG_PORT_PMUX2             (*(RwReg  *)0x41008130UL) /**< \brief (PORT) Peripheral Multiplexing 2 */
#define REG_PORT_PINCFG2           (*(RwReg  *)0x41008140UL) /**< \brief (PORT) Pin Configuration 2 */
#define REG_PORT_DIR3              (*(RwReg  *)0x41008180UL) /**< \brief (PORT) Data Direction 3 */
#define REG_PORT_DIRCLR3           (*(RwReg  *)0x41008184UL) /**< \brief (PORT) Data Direction Clear 3 */
#define REG_PORT_DIRSET3           (*(RwReg  *)0x41008188UL) /**< \brief (PORT) Data Direction Set 3 */
#define REG_PORT_DIRTGL3           (*(RwReg  *)0x4100818CUL) /**< \brief (PORT) Data Direction Toggle 3 */
#define REG_PORT_OUT3              (*(RwReg  *)0x41008190UL) /**< \brief (PORT) Data Output Value 3 */
#define REG_PORT_OUTCLR3           (*(RwReg  *)0x41008194UL) /**< \brief (PORT) Data Output Value Clear 3 */
#define REG_PORT_OUTSET3           (*(RwReg  *)0x41008198UL) /**< \brief (PORT) Data Output Value Set 3 */
#define REG_PORT_OUTTGL3           (*(RwReg  *)0x4100819CUL) /**< \brief (PORT) Data Output Value Toggle 3 */
#define REG_PORT_IN3               (*(RoReg  *)0x410081A0UL) /**< \brief (PORT) Data Input Value 3 */
#define REG_PORT_CTRL3             (*(RwReg  *)0x410081A4UL) /**< \brief (PORT) Control 3 */
#define REG_PORT_WRCONFIG3         (*(WoReg  *)0x410081A8UL) /**< \brief (PORT) Write Configuration 3 */
#define REG_PORT_EVCTRL3           (*(RwReg  *)0x410081ACUL) /**< \brief (PORT) Event Input Control 3 */
#define REG_PORT_PMUX3             (*(RwReg  *)0x410081B0UL) /**< \brief (PORT) Peripheral Multiplexing 3 */
#define REG_PORT_PINCFG3           (*(RwReg  *)0x410081C0UL) /**< \brief (PORT) Pin Configuration 3 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for PORT peripheral ========== */
#define PORT_BITS                   118     
#define PORT_DIR_DEFAULT_VAL        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }
#define PORT_DIR_IMPLEMENTED        { 0xCBFFFFFF, 0xFFFFFFFF, 0xDFFFFCFF, 0x00301F03 }
#define PORT_DRVSTR                 1        // DRVSTR supported
#define PORT_DRVSTR_DEFAULT_VAL     { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }
#define PORT_DRVSTR_IMPLEMENTED     { 0xC8FFFFFF, 0xFFFFFFFF, 0xDFFFFCFF, 0x00301F03 }
#define PORT_EVENT_IMPLEMENTED      { 0xCBFFFFFF, 0xFFFFFFFF, 0xDFFFFCFF, 0x00301F03 }
#define PORT_EV_NUM                 4       
#define PORT_INEN_DEFAULT_VAL       { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }
#define PORT_INEN_IMPLEMENTED       { 0xCBFFFFFF, 0xFFFFFFFF, 0xDFFFFCFF, 0x00301F03 }
#define PORT_ODRAIN                 0        // ODRAIN supported
#define PORT_ODRAIN_DEFAULT_VAL     { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }
#define PORT_ODRAIN_IMPLEMENTED     { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }
#define PORT_OUT_DEFAULT_VAL        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }
#define PORT_OUT_IMPLEMENTED        { 0xCBFFFFFF, 0xFFFFFFFF, 0xDFFFFCFF, 0x00301F03 }
#define PORT_PIN_IMPLEMENTED        { 0xCBFFFFFF, 0xFFFFFFFF, 0xDFFFFCFF, 0x00301F03 }
#define PORT_PMUXBIT0_DEFAULT_VAL   { 0x40000000, 0x00000000, 0x00000000, 0x00000000 }
#define PORT_PMUXBIT0_IMPLEMENTED   { 0xCBFFFFFF, 0xFFFFFFFF, 0xDFFFFC1F, 0x00301F03 }
#define PORT_PMUXBIT1_DEFAULT_VAL   { 0x40000000, 0x00000000, 0x00000000, 0x00000000 }
#define PORT_PMUXBIT1_IMPLEMENTED   { 0xCBFFFFFB, 0xFFFFFFFF, 0x1FFFFCF0, 0x00300F00 }
#define PORT_PMUXBIT2_DEFAULT_VAL   { 0x40000000, 0x00000000, 0x00000000, 0x00000000 }
#define PORT_PMUXBIT2_IMPLEMENTED   { 0xCBFFFFFB, 0xFFFFFFFF, 0x1FFFFC10, 0x00301F00 }
#define PORT_PMUXBIT3_DEFAULT_VAL   { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }
#define PORT_PMUXBIT3_IMPLEMENTED   { 0xCBFFFFF8, 0x33FFFFFF, 0x18FFF8C0, 0x00300000 }
#define PORT_PMUXEN_DEFAULT_VAL     { 0x40000000, 0x00000000, 0x00000000, 0x00000000 }
#define PORT_PMUXEN_IMPLEMENTED     { 0xCBFFFFFF, 0xFFFFFFFF, 0xDFFFFCFF, 0x00301F03 }
#define PORT_PPP_IMPLEMENTED        { 0x00000001 } // IOBUS2 implemented?
#define PORT_PULLEN_DEFAULT_VAL     { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }
#define PORT_PULLEN_IMPLEMENTED     { 0xCBFFFFFF, 0xFFFFFFFF, 0xDFFFFCFF, 0x00301F03 }
#define PORT_SLEWLIM                0        // SLEWLIM supported
#define PORT_SLEWLIM_DEFAULT_VAL    { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }
#define PORT_SLEWLIM_IMPLEMENTED    { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }

#endif /* _SAMD51_PORT_INSTANCE_ */
