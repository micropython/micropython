/**
 * \file
 *
 * \brief Instance description for OSCCTRL
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

#ifndef _SAMD51_OSCCTRL_INSTANCE_
#define _SAMD51_OSCCTRL_INSTANCE_

/* ========== Register definition for OSCCTRL peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_OSCCTRL_EVCTRL         (0x40001000) /**< \brief (OSCCTRL) Event Control */
#define REG_OSCCTRL_INTENCLR       (0x40001004) /**< \brief (OSCCTRL) Interrupt Enable Clear */
#define REG_OSCCTRL_INTENSET       (0x40001008) /**< \brief (OSCCTRL) Interrupt Enable Set */
#define REG_OSCCTRL_INTFLAG        (0x4000100C) /**< \brief (OSCCTRL) Interrupt Flag Status and Clear */
#define REG_OSCCTRL_STATUS         (0x40001010) /**< \brief (OSCCTRL) Status */
#define REG_OSCCTRL_XOSCCTRL0      (0x40001014) /**< \brief (OSCCTRL) External Multipurpose Crystal Oscillator Control 0 */
#define REG_OSCCTRL_XOSCCTRL1      (0x40001018) /**< \brief (OSCCTRL) External Multipurpose Crystal Oscillator Control 1 */
#define REG_OSCCTRL_DFLLCTRLA      (0x4000101C) /**< \brief (OSCCTRL) DFLL48M Control A */
#define REG_OSCCTRL_DFLLCTRLB      (0x40001020) /**< \brief (OSCCTRL) DFLL48M Control B */
#define REG_OSCCTRL_DFLLVAL        (0x40001024) /**< \brief (OSCCTRL) DFLL48M Value */
#define REG_OSCCTRL_DFLLMUL        (0x40001028) /**< \brief (OSCCTRL) DFLL48M Multiplier */
#define REG_OSCCTRL_DFLLSYNC       (0x4000102C) /**< \brief (OSCCTRL) DFLL48M Synchronization */
#define REG_OSCCTRL_DPLLCTRLA0     (0x40001030) /**< \brief (OSCCTRL) DPLL Control A 0 */
#define REG_OSCCTRL_DPLLRATIO0     (0x40001034) /**< \brief (OSCCTRL) DPLL Ratio Control 0 */
#define REG_OSCCTRL_DPLLCTRLB0     (0x40001038) /**< \brief (OSCCTRL) DPLL Control B 0 */
#define REG_OSCCTRL_DPLLSYNCBUSY0  (0x4000103C) /**< \brief (OSCCTRL) DPLL Synchronization Busy 0 */
#define REG_OSCCTRL_DPLLSTATUS0    (0x40001040) /**< \brief (OSCCTRL) DPLL Status 0 */
#define REG_OSCCTRL_DPLLCTRLA1     (0x40001044) /**< \brief (OSCCTRL) DPLL Control A 1 */
#define REG_OSCCTRL_DPLLRATIO1     (0x40001048) /**< \brief (OSCCTRL) DPLL Ratio Control 1 */
#define REG_OSCCTRL_DPLLCTRLB1     (0x4000104C) /**< \brief (OSCCTRL) DPLL Control B 1 */
#define REG_OSCCTRL_DPLLSYNCBUSY1  (0x40001050) /**< \brief (OSCCTRL) DPLL Synchronization Busy 1 */
#define REG_OSCCTRL_DPLLSTATUS1    (0x40001054) /**< \brief (OSCCTRL) DPLL Status 1 */
#else
#define REG_OSCCTRL_EVCTRL         (*(RwReg8 *)0x40001000UL) /**< \brief (OSCCTRL) Event Control */
#define REG_OSCCTRL_INTENCLR       (*(RwReg  *)0x40001004UL) /**< \brief (OSCCTRL) Interrupt Enable Clear */
#define REG_OSCCTRL_INTENSET       (*(RwReg  *)0x40001008UL) /**< \brief (OSCCTRL) Interrupt Enable Set */
#define REG_OSCCTRL_INTFLAG        (*(RwReg  *)0x4000100CUL) /**< \brief (OSCCTRL) Interrupt Flag Status and Clear */
#define REG_OSCCTRL_STATUS         (*(RoReg  *)0x40001010UL) /**< \brief (OSCCTRL) Status */
#define REG_OSCCTRL_XOSCCTRL0      (*(RwReg  *)0x40001014UL) /**< \brief (OSCCTRL) External Multipurpose Crystal Oscillator Control 0 */
#define REG_OSCCTRL_XOSCCTRL1      (*(RwReg  *)0x40001018UL) /**< \brief (OSCCTRL) External Multipurpose Crystal Oscillator Control 1 */
#define REG_OSCCTRL_DFLLCTRLA      (*(RwReg8 *)0x4000101CUL) /**< \brief (OSCCTRL) DFLL48M Control A */
#define REG_OSCCTRL_DFLLCTRLB      (*(RwReg8 *)0x40001020UL) /**< \brief (OSCCTRL) DFLL48M Control B */
#define REG_OSCCTRL_DFLLVAL        (*(RwReg  *)0x40001024UL) /**< \brief (OSCCTRL) DFLL48M Value */
#define REG_OSCCTRL_DFLLMUL        (*(RwReg  *)0x40001028UL) /**< \brief (OSCCTRL) DFLL48M Multiplier */
#define REG_OSCCTRL_DFLLSYNC       (*(RwReg8 *)0x4000102CUL) /**< \brief (OSCCTRL) DFLL48M Synchronization */
#define REG_OSCCTRL_DPLLCTRLA0     (*(RwReg8 *)0x40001030UL) /**< \brief (OSCCTRL) DPLL Control A 0 */
#define REG_OSCCTRL_DPLLRATIO0     (*(RwReg8 *)0x40001034UL) /**< \brief (OSCCTRL) DPLL Ratio Control 0 */
#define REG_OSCCTRL_DPLLCTRLB0     (*(RwReg8 *)0x40001038UL) /**< \brief (OSCCTRL) DPLL Control B 0 */
#define REG_OSCCTRL_DPLLSYNCBUSY0  (*(RoReg8 *)0x4000103CUL) /**< \brief (OSCCTRL) DPLL Synchronization Busy 0 */
#define REG_OSCCTRL_DPLLSTATUS0    (*(RoReg8 *)0x40001040UL) /**< \brief (OSCCTRL) DPLL Status 0 */
#define REG_OSCCTRL_DPLLCTRLA1     (*(RwReg8 *)0x40001044UL) /**< \brief (OSCCTRL) DPLL Control A 1 */
#define REG_OSCCTRL_DPLLRATIO1     (*(RwReg8 *)0x40001048UL) /**< \brief (OSCCTRL) DPLL Ratio Control 1 */
#define REG_OSCCTRL_DPLLCTRLB1     (*(RwReg8 *)0x4000104CUL) /**< \brief (OSCCTRL) DPLL Control B 1 */
#define REG_OSCCTRL_DPLLSYNCBUSY1  (*(RoReg8 *)0x40001050UL) /**< \brief (OSCCTRL) DPLL Synchronization Busy 1 */
#define REG_OSCCTRL_DPLLSTATUS1    (*(RoReg8 *)0x40001054UL) /**< \brief (OSCCTRL) DPLL Status 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for OSCCTRL peripheral ========== */
#define OSCCTRL_DFLLS_NUM           1        // Number of DFLLs
#define OSCCTRL_DFLL_IMPLEMENTED    1        // DFLL implemented
#define OSCCTRL_DFLL48M_BIASTESTPT_IMPLEMENTED 0        // DFLL48M bias test mode implemented
#define OSCCTRL_DFLL48M_CDACSTEPSIZE_SIZE 2        // Size COARSE DAC STEP
#define OSCCTRL_DFLL48M_COARSE_RESET_VALUE 32       // DFLL48M Frequency Coarse Reset Value (Before Calibration)
#define OSCCTRL_DFLL48M_COARSE_SIZE 6        // Size COARSE CALIBRATION
#define OSCCTRL_DFLL48M_ENABLE_RESET_VALUE 1        // Run oscillator at reset
#define OSCCTRL_DFLL48M_FDACSTEPSIZE_SIZE 2        // Size FINE DAC STEP
#define OSCCTRL_DFLL48M_FINE_RESET_VALUE 128      // DFLL48M Frequency Fine Reset Value (Before Calibration)
#define OSCCTRL_DFLL48M_FINE_SIZE   8        // Size FINE CALIBRATION
#define OSCCTRL_DFLL48M_ONDEMAND_RESET_VALUE 1        // Run oscillator always or only when requested
#define OSCCTRL_DFLL48M_RUNSTDBY_RESET_VALUE 0        // Run oscillator even if standby mode
#define OSCCTRL_DFLL48M_TCAL_SIZE   4        // Size TEMP CALIBRATION
#define OSCCTRL_DFLL48M_TCBIAS_SIZE 2        // Size TC BIAS CALIBRATION
#define OSCCTRL_DFLL48M_TESTPTSEL_SIZE 3        // Size TEST POINT SELECTOR
#define OSCCTRL_DFLL48M_WAITLOCK_ACTIVE 1        // Enable Wait Lock Feature
#define OSCCTRL_DPLLS_NUM           2        // Number of DPLLs
#define OSCCTRL_DPLL0_IMPLEMENTED   1        // DPLL0 implemented
#define OSCCTRL_DPLL0_I12ND_I12NDFRAC_PAD_CONTROL 0        // NOT_IMPLEMENTED: The ND and NDFRAC pad tests are not used, use registers instead
#define OSCCTRL_DPLL0_OCC_IMPLEMENTED 1        // DPLL0 OCC Implemented
#define OSCCTRL_DPLL1_IMPLEMENTED   1        // DPLL1 implemented
#define OSCCTRL_DPLL1_I12ND_I12NDFRAC_PAD_CONTROL 0        // NOT_IMPLEMENTED: The ND and NDFRAC pad tests are not used, use registers instead
#define OSCCTRL_DPLL1_OCC_IMPLEMENTED 0        // DPLL1 OCC Implemented
#define OSCCTRL_GCLK_ID_DFLL48      0        // Index of Generic Clock for DFLL48
#define OSCCTRL_GCLK_ID_FDPLL0      1        // Index of Generic Clock for DPLL0
#define OSCCTRL_GCLK_ID_FDPLL1      2        // Index of Generic Clock for DPLL1
#define OSCCTRL_GCLK_ID_FDPLL032K   3        // Index of Generic Clock for DPLL0 32K
#define OSCCTRL_GCLK_ID_FDPLL132K   3        // Index of Generic Clock for DPLL1 32K
#define OSCCTRL_OSC16M_IMPLEMENTED  0        // OSC16M implemented
#define OSCCTRL_OSC48M_IMPLEMENTED  0        // OSC48M implemented
#define OSCCTRL_OSC48M_NUM          1       
#define OSCCTRL_RCOSCS_NUM          1        // Number of RCOSCs (min 1)
#define OSCCTRL_XOSCS_NUM           2        // Number of XOSCs
#define OSCCTRL_XOSC0_CFD_CLK_SELECT_SIZE 4        // Clock fail prescaler size
#define OSCCTRL_XOSC0_CFD_IMPLEMENTED 1        // Clock fail detected for xosc implemented
#define OSCCTRL_XOSC0_IMPLEMENTED   1        // XOSC0 implemented
#define OSCCTRL_XOSC0_ONDEMAND_RESET_VALUE 1        // Run oscillator always or only when requested
#define OSCCTRL_XOSC0_RUNSTDBY_RESET_VALUE 0        // Run oscillator even if standby mode
#define OSCCTRL_XOSC1_CFD_CLK_SELECT_SIZE 4        // Clock fail prescaler size
#define OSCCTRL_XOSC1_CFD_IMPLEMENTED 1        // Clock fail detected for xosc implemented
#define OSCCTRL_XOSC1_IMPLEMENTED   1        // XOSC1 implemented
#define OSCCTRL_XOSC1_ONDEMAND_RESET_VALUE 1        // Run oscillator always or only when requested
#define OSCCTRL_XOSC1_RUNSTDBY_RESET_VALUE 0        // Run oscillator even if standby mode
#define OSCCTRL_DFLL48M_VERSION     0x100   
#define OSCCTRL_FDPLL_VERSION       0x100   
#define OSCCTRL_XOSC_VERSION        0x100   

#endif /* _SAMD51_OSCCTRL_INSTANCE_ */
