/**
 * \file
 *
 * \brief Instance description for EVSYS
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

#ifndef _SAMD21_EVSYS_INSTANCE_
#define _SAMD21_EVSYS_INSTANCE_

/* ========== Register definition for EVSYS peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_EVSYS_CTRL             (0x42000400) /**< \brief (EVSYS) Control */
#define REG_EVSYS_CHANNEL          (0x42000404) /**< \brief (EVSYS) Channel */
#define REG_EVSYS_USER             (0x42000408) /**< \brief (EVSYS) User Multiplexer */
#define REG_EVSYS_CHSTATUS         (0x4200040C) /**< \brief (EVSYS) Channel Status */
#define REG_EVSYS_INTENCLR         (0x42000410) /**< \brief (EVSYS) Interrupt Enable Clear */
#define REG_EVSYS_INTENSET         (0x42000414) /**< \brief (EVSYS) Interrupt Enable Set */
#define REG_EVSYS_INTFLAG          (0x42000418) /**< \brief (EVSYS) Interrupt Flag Status and Clear */
#else
#define REG_EVSYS_CTRL             (*(WoReg8 *)0x42000400UL) /**< \brief (EVSYS) Control */
#define REG_EVSYS_CHANNEL          (*(RwReg  *)0x42000404UL) /**< \brief (EVSYS) Channel */
#define REG_EVSYS_USER             (*(RwReg16*)0x42000408UL) /**< \brief (EVSYS) User Multiplexer */
#define REG_EVSYS_CHSTATUS         (*(RoReg  *)0x4200040CUL) /**< \brief (EVSYS) Channel Status */
#define REG_EVSYS_INTENCLR         (*(RwReg  *)0x42000410UL) /**< \brief (EVSYS) Interrupt Enable Clear */
#define REG_EVSYS_INTENSET         (*(RwReg  *)0x42000414UL) /**< \brief (EVSYS) Interrupt Enable Set */
#define REG_EVSYS_INTFLAG          (*(RwReg  *)0x42000418UL) /**< \brief (EVSYS) Interrupt Flag Status and Clear */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for EVSYS peripheral ========== */
#define EVSYS_CHANNELS              12       // Number of Channels
#define EVSYS_CHANNELS_BITS         4        // Number of bits to select Channel
#define EVSYS_CHANNELS_MSB          11       // Number of Channels - 1
#define EVSYS_EXTEVT_NUM            0        // Number of External Event Generators
#define EVSYS_GCLK_ID_0             7
#define EVSYS_GCLK_ID_1             8
#define EVSYS_GCLK_ID_2             9
#define EVSYS_GCLK_ID_3             10
#define EVSYS_GCLK_ID_4             11
#define EVSYS_GCLK_ID_5             12
#define EVSYS_GCLK_ID_6             13
#define EVSYS_GCLK_ID_7             14
#define EVSYS_GCLK_ID_8             15
#define EVSYS_GCLK_ID_9             16
#define EVSYS_GCLK_ID_10            17
#define EVSYS_GCLK_ID_11            18
#define EVSYS_GCLK_ID_LSB           7
#define EVSYS_GCLK_ID_MSB           18
#define EVSYS_GCLK_ID_SIZE          12
#define EVSYS_GENERATORS            73       // Total Number of Event Generators
#define EVSYS_GENERATORS_BITS       7        // Number of bits to select Event Generator
#define EVSYS_USERS                 29       // Total Number of Event Users
#define EVSYS_USERS_BITS            5        // Number of bits to select Event User

// GENERATORS
#define EVSYS_ID_GEN_RTC_CMP_0      1
#define EVSYS_ID_GEN_RTC_CMP_1      2
#define EVSYS_ID_GEN_RTC_OVF        3
#define EVSYS_ID_GEN_RTC_PER_0      4
#define EVSYS_ID_GEN_RTC_PER_1      5
#define EVSYS_ID_GEN_RTC_PER_2      6
#define EVSYS_ID_GEN_RTC_PER_3      7
#define EVSYS_ID_GEN_RTC_PER_4      8
#define EVSYS_ID_GEN_RTC_PER_5      9
#define EVSYS_ID_GEN_RTC_PER_6      10
#define EVSYS_ID_GEN_RTC_PER_7      11
#define EVSYS_ID_GEN_EIC_EXTINT_0   12
#define EVSYS_ID_GEN_EIC_EXTINT_1   13
#define EVSYS_ID_GEN_EIC_EXTINT_2   14
#define EVSYS_ID_GEN_EIC_EXTINT_3   15
#define EVSYS_ID_GEN_EIC_EXTINT_4   16
#define EVSYS_ID_GEN_EIC_EXTINT_5   17
#define EVSYS_ID_GEN_EIC_EXTINT_6   18
#define EVSYS_ID_GEN_EIC_EXTINT_7   19
#define EVSYS_ID_GEN_EIC_EXTINT_8   20
#define EVSYS_ID_GEN_EIC_EXTINT_9   21
#define EVSYS_ID_GEN_EIC_EXTINT_10  22
#define EVSYS_ID_GEN_EIC_EXTINT_11  23
#define EVSYS_ID_GEN_EIC_EXTINT_12  24
#define EVSYS_ID_GEN_EIC_EXTINT_13  25
#define EVSYS_ID_GEN_EIC_EXTINT_14  26
#define EVSYS_ID_GEN_EIC_EXTINT_15  27
#define EVSYS_ID_GEN_EIC_EXTINT_16  28
#define EVSYS_ID_GEN_EIC_EXTINT_17  29
#define EVSYS_ID_GEN_DMAC_CH_0      30
#define EVSYS_ID_GEN_DMAC_CH_1      31
#define EVSYS_ID_GEN_DMAC_CH_2      32
#define EVSYS_ID_GEN_DMAC_CH_3      33
#define EVSYS_ID_GEN_TCC0_OVF       34
#define EVSYS_ID_GEN_TCC0_TRG       35
#define EVSYS_ID_GEN_TCC0_CNT       36
#define EVSYS_ID_GEN_TCC0_MCX_0     37
#define EVSYS_ID_GEN_TCC0_MCX_1     38
#define EVSYS_ID_GEN_TCC0_MCX_2     39
#define EVSYS_ID_GEN_TCC0_MCX_3     40
#define EVSYS_ID_GEN_TCC1_OVF       41
#define EVSYS_ID_GEN_TCC1_TRG       42
#define EVSYS_ID_GEN_TCC1_CNT       43
#define EVSYS_ID_GEN_TCC1_MCX_0     44
#define EVSYS_ID_GEN_TCC1_MCX_1     45
#define EVSYS_ID_GEN_TCC2_OVF       46
#define EVSYS_ID_GEN_TCC2_TRG       47
#define EVSYS_ID_GEN_TCC2_CNT       48
#define EVSYS_ID_GEN_TCC2_MCX_0     49
#define EVSYS_ID_GEN_TCC2_MCX_1     50
#define EVSYS_ID_GEN_TC3_OVF        51
#define EVSYS_ID_GEN_TC3_MCX_0      52
#define EVSYS_ID_GEN_TC3_MCX_1      53
#define EVSYS_ID_GEN_TC4_OVF        54
#define EVSYS_ID_GEN_TC4_MCX_0      55
#define EVSYS_ID_GEN_TC4_MCX_1      56
#define EVSYS_ID_GEN_TC5_OVF        57
#define EVSYS_ID_GEN_TC5_MCX_0      58
#define EVSYS_ID_GEN_TC5_MCX_1      59
#define EVSYS_ID_GEN_TC6_OVF        60
#define EVSYS_ID_GEN_TC6_MCX_0      61
#define EVSYS_ID_GEN_TC6_MCX_1      62
#define EVSYS_ID_GEN_TC7_OVF        63
#define EVSYS_ID_GEN_TC7_MCX_0      64
#define EVSYS_ID_GEN_TC7_MCX_1      65
#define EVSYS_ID_GEN_ADC_RESRDY     66
#define EVSYS_ID_GEN_ADC_WINMON     67
#define EVSYS_ID_GEN_AC_COMP_0      68
#define EVSYS_ID_GEN_AC_COMP_1      69
#define EVSYS_ID_GEN_AC_WIN_0       70
#define EVSYS_ID_GEN_DAC_EMPTY      71

// USERS
#define EVSYS_ID_USER_DMAC_CH_0     0
#define EVSYS_ID_USER_DMAC_CH_1     1
#define EVSYS_ID_USER_DMAC_CH_2     2
#define EVSYS_ID_USER_DMAC_CH_3     3
#define EVSYS_ID_USER_TCC0_EV_0     4
#define EVSYS_ID_USER_TCC0_EV_1     5
#define EVSYS_ID_USER_TCC0_MC_0     6
#define EVSYS_ID_USER_TCC0_MC_1     7
#define EVSYS_ID_USER_TCC0_MC_2     8
#define EVSYS_ID_USER_TCC0_MC_3     9
#define EVSYS_ID_USER_TCC1_EV_0     10
#define EVSYS_ID_USER_TCC1_EV_1     11
#define EVSYS_ID_USER_TCC1_MC_0     12
#define EVSYS_ID_USER_TCC1_MC_1     13
#define EVSYS_ID_USER_TCC2_EV_0     14
#define EVSYS_ID_USER_TCC2_EV_1     15
#define EVSYS_ID_USER_TCC2_MC_0     16
#define EVSYS_ID_USER_TCC2_MC_1     17
#define EVSYS_ID_USER_TC3_EVU       18
#define EVSYS_ID_USER_TC4_EVU       19
#define EVSYS_ID_USER_TC5_EVU       20
#define EVSYS_ID_USER_TC6_EVU       21
#define EVSYS_ID_USER_TC7_EVU       22
#define EVSYS_ID_USER_ADC_START     23
#define EVSYS_ID_USER_ADC_SYNC      24
#define EVSYS_ID_USER_AC_SOC_0      25
#define EVSYS_ID_USER_AC_SOC_1      26
#define EVSYS_ID_USER_DAC_START     27

#endif /* _SAMD21_EVSYS_INSTANCE_ */
