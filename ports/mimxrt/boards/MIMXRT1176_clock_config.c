/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * How to setup clock using clock driver functions:
 *
 * 1. Call CLOCK_InitXXXPLL() to configure corresponding PLL clock.
 *
 * 2. Call CLOCK_InitXXXpfd() to configure corresponding PLL pfd clock.
 *
 * 3. Call CLOCK_SetRootClock() to configure corresponding module clock source and divider.
 *
 */

#include CLOCK_CONFIG_H
#include "fsl_iomuxc.h"
#include "fsl_dcdc.h"
#include "fsl_pmu.h"
#include "fsl_clock.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* System clock frequency. */
extern uint32_t SystemCoreClock;

/*******************************************************************************
 ************************ BOARD_InitBootClocks function ************************
 ******************************************************************************/
void BOARD_InitBootClocks(void) {
    BOARD_BootClockRUN();
}

#if defined(XIP_BOOT_HEADER_ENABLE) && (XIP_BOOT_HEADER_ENABLE == 1)
#if defined(XIP_BOOT_HEADER_DCD_ENABLE) && (XIP_BOOT_HEADER_DCD_ENABLE == 1)
/* This function should not run from SDRAM since it will change SEMC configuration. */
AT_QUICKACCESS_SECTION_CODE(void UpdateSemcClock(void));
void UpdateSemcClock(void) {
    /* Enable self-refresh mode and update semc clock root to 200MHz. */
    SEMC->IPCMD = 0xA55A000D;
    while ((SEMC->INTR & 0x3) == 0) {
        ;
    }
    SEMC->INTR = 0x3;
    SEMC->DCCR = 0x0B;
    /*
    * Currently we are using SEMC parameter which fit both 166MHz and 200MHz, only
    * need to change the SEMC clock root here. If customer is using their own DCD and
    * want to switch from 166MHz to 200MHz, extra SEMC configuration might need to be
    * adjusted here to fine tune the SDRAM performance
    */
    CCM->CLOCK_ROOT[kCLOCK_Root_Semc].CONTROL = 0x602;
}
#endif
#endif

/*******************************************************************************
 ********************** Configuration BOARD_BootClockRUN ***********************
 ******************************************************************************/

/*******************************************************************************
 * Variables for BOARD_BootClockRUN configuration
 ******************************************************************************/

#ifndef SKIP_POWER_ADJUSTMENT
#if __CORTEX_M == 7
#define BYPASS_LDO_LPSR 1
#define SKIP_LDO_ADJUSTMENT 1
#elif __CORTEX_M == 4
#define SKIP_DCDC_ADJUSTMENT 1
#define SKIP_FBB_ENABLE 1
#endif
#endif

const clock_arm_pll_config_t armPllConfig_BOARD_BootClockRUN = {
    .postDivider = kCLOCK_PllPostDiv2,            /* Post divider, 0 - DIV by 2, 1 - DIV by 4, 2 - DIV by 8, 3 - DIV by 1 */
    .loopDivider = 166,                           /* PLL Loop divider, Fout = Fin * ( loopDivider / ( 2 * postDivider ) ) */
};

const clock_sys_pll1_config_t sysPll1Config_BOARD_BootClockRUN = {
    .pllDiv2En = true,
};

const clock_sys_pll2_config_t sysPll2Config_BOARD_BootClockRUN = {
    .mfd = 268435455,                             /* Denominator of spread spectrum */
    .ss = NULL,                                   /* Spread spectrum parameter */
    .ssEnable = false,                            /* Enable spread spectrum or not */
};

const clock_video_pll_config_t videoPllConfig_BOARD_BootClockRUN = {
    .loopDivider = 41,                            /* PLL Loop divider, valid range for DIV_SELECT divider value: 27 ~ 54. */
    .postDivider = 0,                             /* Divider after PLL, should only be 1, 2, 4, 8, 16, 32 */
    .numerator = 1,                               /* 30 bit numerator of fractional loop divider, Fout = Fin * ( loopDivider + numerator / denominator ) */
    .denominator = 960000,                        /* 30 bit denominator of fractional loop divider, Fout = Fin * ( loopDivider + numerator / denominator ) */
    .ss = NULL,                                   /* Spread spectrum parameter */
    .ssEnable = false,                            /* Enable spread spectrum or not */
};

/*******************************************************************************
 * Code for BOARD_BootClockRUN configuration
 ******************************************************************************/
void BOARD_BootClockRUN(void) {
    clock_root_config_t rootCfg = {0};

    #if !defined(SKIP_DCDC_ADJUSTMENT) || (!SKIP_DCDC_ADJUSTMENT)
    if ((OCOTP->FUSEN[16].FUSE == 0x57AC5969U) && ((OCOTP->FUSEN[17].FUSE & 0xFFU) == 0x0BU)) {
        DCDC_SetVDD1P0BuckModeTargetVoltage(DCDC, kDCDC_1P0BuckTarget1P15V);
    } else {
        /* Set 1.125V for production samples to align with data sheet requirement */
        DCDC_SetVDD1P0BuckModeTargetVoltage(DCDC, kDCDC_1P0BuckTarget1P125V);
    }
    #endif

    #if !defined(SKIP_FBB_ENABLE) || (!SKIP_FBB_ENABLE)
    /* Check if FBB need to be enabled in OverDrive(OD) mode */
    if (((OCOTP->FUSEN[7].FUSE & 0x10U) >> 4U) != 1) {
        PMU_EnableBodyBias(ANADIG_PMU, kPMU_FBB_CM7, true);
    } else {
        PMU_EnableBodyBias(ANADIG_PMU, kPMU_FBB_CM7, false);
    }
    #endif

    #if defined(BYPASS_LDO_LPSR) && BYPASS_LDO_LPSR
    PMU_StaticEnableLpsrAnaLdoBypassMode(ANADIG_LDO_SNVS, true);
    PMU_StaticEnableLpsrDigLdoBypassMode(ANADIG_LDO_SNVS, true);
    #endif

    #if !defined(SKIP_LDO_ADJUSTMENT) || (!SKIP_LDO_ADJUSTMENT)
    pmu_static_lpsr_ana_ldo_config_t lpsrAnaConfig;
    pmu_static_lpsr_dig_config_t lpsrDigConfig;

    if ((ANADIG_LDO_SNVS->PMU_LDO_LPSR_ANA & ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_BYPASS_MODE_EN_MASK) == 0UL) {
        PMU_StaticGetLpsrAnaLdoDefaultConfig(&lpsrAnaConfig);
        PMU_StaticLpsrAnaLdoInit(ANADIG_LDO_SNVS, &lpsrAnaConfig);
    }

    if ((ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG & ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_BYPASS_MODE_MASK) == 0UL) {
        PMU_StaticGetLpsrDigLdoDefaultConfig(&lpsrDigConfig);
        lpsrDigConfig.targetVoltage = kPMU_LpsrDigTargetStableVoltage1P117V;
        PMU_StaticLpsrDigLdoInit(ANADIG_LDO_SNVS, &lpsrDigConfig);
    }
    #endif

    /* PLL LDO shall be enabled first before enable PLLs */

    /* Config CLK_1M */
    CLOCK_OSC_Set1MHzOutputBehavior(kCLOCK_1MHzOutEnableFreeRunning1Mhz);

    /* Init OSC RC 16M */
    ANADIG_OSC->OSC_16M_CTRL |= ANADIG_OSC_OSC_16M_CTRL_EN_IRC4M16M_MASK;

    /* Init OSC RC 400M */
    CLOCK_OSC_EnableOscRc400M();
    CLOCK_OSC_GateOscRc400M(true);

    /* Init OSC RC 48M */
    CLOCK_OSC_EnableOsc48M(true);
    CLOCK_OSC_EnableOsc48MDiv2(true);

    /* Config OSC 24M */
    ANADIG_OSC->OSC_24M_CTRL |= ANADIG_OSC_OSC_24M_CTRL_OSC_EN(1) | ANADIG_OSC_OSC_24M_CTRL_BYPASS_EN(0) | ANADIG_OSC_OSC_24M_CTRL_BYPASS_CLK(0) | ANADIG_OSC_OSC_24M_CTRL_LP_EN(1) | ANADIG_OSC_OSC_24M_CTRL_OSC_24M_GATE(0);
    /* Wait for 24M OSC to be stable. */
    while (ANADIG_OSC_OSC_24M_CTRL_OSC_24M_STABLE_MASK !=
           (ANADIG_OSC->OSC_24M_CTRL & ANADIG_OSC_OSC_24M_CTRL_OSC_24M_STABLE_MASK)) {
    }

    /* Switch both core, M7 Systick and Bus_Lpsr to OscRC48MDiv2 first */
    rootCfg.mux = kCLOCK_M7_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    #if __CORTEX_M == 7
    CLOCK_SetRootClock(kCLOCK_Root_M7, &rootCfg);
    CLOCK_SetRootClock(kCLOCK_Root_M7_Systick, &rootCfg);
    #endif
    #if __CORTEX_M == 4
    CLOCK_SetRootClock(kCLOCK_Root_M4, &rootCfg);
    CLOCK_SetRootClock(kCLOCK_Root_Bus_Lpsr, &rootCfg);
    #endif

    /*
    * if DCD is used, please make sure the clock source of SEMC is not changed in the following PLL/PFD configuration code.
    */
    /* Init Arm Pll. */
    CLOCK_InitArmPll(&armPllConfig_BOARD_BootClockRUN);

    /* Bypass Sys Pll1. */
    // CLOCK_SetPllBypass(kCLOCK_PllSys1, true);

    /* Init Sys Pll1 and enable PllSys1_Div2 output */
    CLOCK_InitSysPll1(&sysPll1Config_BOARD_BootClockRUN);


    /* Init Sys Pll2. */
    CLOCK_InitSysPll2(&sysPll2Config_BOARD_BootClockRUN);

    /* Init System Pll2 pfd0. */
    CLOCK_InitPfd(kCLOCK_PllSys2, kCLOCK_Pfd0, 27);

    /* Init System Pll2 pfd1. */
    CLOCK_InitPfd(kCLOCK_PllSys2, kCLOCK_Pfd1, 16);

    /* Init System Pll2 pfd2. */
    CLOCK_InitPfd(kCLOCK_PllSys2, kCLOCK_Pfd2, 24);

    /* Init System Pll2 pfd3. */
    CLOCK_InitPfd(kCLOCK_PllSys2, kCLOCK_Pfd3, 32);

    /* Init Sys Pll3. */
    CLOCK_InitSysPll3();

    /* Init System Pll3 pfd0. */
    CLOCK_InitPfd(kCLOCK_PllSys3, kCLOCK_Pfd0, 13);

    /* Init System Pll3 pfd1. */
    CLOCK_InitPfd(kCLOCK_PllSys3, kCLOCK_Pfd1, 17);

    /* Init System Pll3 pfd2. */
    CLOCK_InitPfd(kCLOCK_PllSys3, kCLOCK_Pfd2, 32);

    /* Init System Pll3 pfd3. */
    CLOCK_InitPfd(kCLOCK_PllSys3, kCLOCK_Pfd3, 22);

    /* Bypass Audio Pll. */
    CLOCK_SetPllBypass(kCLOCK_PllAudio, true);

    /* DeInit Audio Pll. */
    CLOCK_DeinitAudioPll();

    /* Init Video Pll. */
    CLOCK_InitVideoPll(&videoPllConfig_BOARD_BootClockRUN);

    /* Module clock root configurations. */
    /* Configure M7 using ARM_PLL_CLK */
    #if __CORTEX_M == 7
    rootCfg.mux = kCLOCK_M7_ClockRoot_MuxArmPllOut;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_M7, &rootCfg);
    #endif

    /* Configure M4 using SYS_PLL3_PFD3_CLK */
    #if __CORTEX_M == 4
    rootCfg.mux = kCLOCK_M4_ClockRoot_MuxSysPll3Pfd3;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_M4, &rootCfg);
    #endif

    /* Configure BUS using SYS_PLL3_CLK */
    rootCfg.mux = kCLOCK_BUS_ClockRoot_MuxSysPll3Out;
    rootCfg.div = 3;
    CLOCK_SetRootClock(kCLOCK_Root_Bus, &rootCfg);

    /* Configure BUS_LPSR using SYS_PLL3_CLK */
    /* BUS_LPSR must not be more than 120MHz */
    rootCfg.mux = kCLOCK_BUS_LPSR_ClockRoot_MuxSysPll3Out;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Bus_Lpsr, &rootCfg);

    /* Configure SEMC using SYS_PLL2_PFD1_CLK */
    #ifndef SKIP_SEMC_INIT
    rootCfg.mux = kCLOCK_SEMC_ClockRoot_MuxSysPll2Pfd1;
    rootCfg.div = 3;
    CLOCK_SetRootClock(kCLOCK_Root_Semc, &rootCfg);
    #endif

    #if defined(XIP_BOOT_HEADER_ENABLE) && (XIP_BOOT_HEADER_ENABLE == 1)
    #if defined(XIP_BOOT_HEADER_DCD_ENABLE) && (XIP_BOOT_HEADER_DCD_ENABLE == 1)
    UpdateSemcClock();
    #endif
    #endif

    /* Configure CSSYS using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_CSSYS_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Cssys, &rootCfg);

    /* Configure CSTRACE using SYS_PLL2_CLK */
    rootCfg.mux = kCLOCK_CSTRACE_ClockRoot_MuxSysPll2Out;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Cstrace, &rootCfg);

    // Strange settings for SYSTICK: 24MHz for M4, 100kHz for M7 ?
    /* Configure M4_SYSTICK using OSC_RC_48M_DIV2 */
    #if __CORTEX_M == 4
    rootCfg.mux = kCLOCK_M4_SYSTICK_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_M4_Systick, &rootCfg);
    #endif

    /* Configure M7_SYSTICK using OSC_RC_48M_DIV2 */
    #if __CORTEX_M == 7
    rootCfg.mux = kCLOCK_M7_SYSTICK_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 240;
    CLOCK_SetRootClock(kCLOCK_Root_M7_Systick, &rootCfg);
    #endif

    /* Configure ADC1 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_ADC1_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Adc1, &rootCfg);

    /* Configure ADC2 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_ADC2_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Adc2, &rootCfg);

    /* Configure ACMP using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_ACMP_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Acmp, &rootCfg);

    /* Configure FLEXIO1 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_FLEXIO1_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Flexio1, &rootCfg);

    /* Configure FLEXIO2 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_FLEXIO2_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Flexio2, &rootCfg);

    /* Configure GPT1 using OSC_24M*/
    rootCfg.mux = kCLOCK_GPT1_ClockRoot_MuxOsc24MOut;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Gpt1, &rootCfg);

    /* Configure GPT2 using OSC_24M */
    rootCfg.mux = kCLOCK_GPT2_ClockRoot_MuxOsc24MOut;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Gpt2, &rootCfg);

    /* Configure GPT3 using OSC_24M */
    rootCfg.mux = kCLOCK_GPT3_ClockRoot_MuxOsc24MOut;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Gpt3, &rootCfg);

    /* Configure GPT4 using OSC_24M */
    rootCfg.mux = kCLOCK_GPT4_ClockRoot_MuxOsc24MOut;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Gpt4, &rootCfg);

    /* Configure GPT5 using OSC_24M */
    rootCfg.mux = kCLOCK_GPT5_ClockRoot_MuxOsc24MOut;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Gpt5, &rootCfg);

    /* Configure GPT6 using OSC_24M */
    rootCfg.mux = kCLOCK_GPT6_ClockRoot_MuxOsc24MOut;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Gpt6, &rootCfg);

    /* Configure FLEXSPI1 using OSC_RC_48M_DIV2 */
    #if !(defined(XIP_EXTERNAL_FLASH) && (XIP_EXTERNAL_FLASH == 1))
    rootCfg.mux = kCLOCK_FLEXSPI1_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Flexspi1, &rootCfg);
    #endif

    /* Configure FLEXSPI2 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_FLEXSPI2_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Flexspi2, &rootCfg);

    /* Configure CAN1 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_CAN1_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Can1, &rootCfg);

    /* Configure CAN2 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_CAN2_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Can2, &rootCfg);

    /* Configure CAN3 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_CAN3_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Can3, &rootCfg);

    /* Configure LPUART1 using SYS_PLL2_PFD3_CLK */
    rootCfg.mux = kCLOCK_LPUART1_ClockRoot_MuxSysPll2Pfd3;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Lpuart1, &rootCfg);

    /* Configure LPUART2 using SYS_PLL2_PFD3_CLK */
    rootCfg.mux = kCLOCK_LPUART2_ClockRoot_MuxSysPll2Pfd3;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Lpuart2, &rootCfg);

    /* Configure LPUART3 using SYS_PLL2_PFD3_CLK */
    rootCfg.mux = kCLOCK_LPUART3_ClockRoot_MuxSysPll2Pfd3;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Lpuart3, &rootCfg);

    /* Configure LPUART4 using SYS_PLL2_PFD3_CLK */
    rootCfg.mux = kCLOCK_LPUART4_ClockRoot_MuxSysPll2Pfd3;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Lpuart4, &rootCfg);

    /* Configure LPUART5 using SYS_PLL2_PFD3_CLK */
    rootCfg.mux = kCLOCK_LPUART5_ClockRoot_MuxSysPll2Pfd3;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Lpuart5, &rootCfg);

    /* Configure LPUART6 using SYS_PLL2_PFD3_CLK */
    rootCfg.mux = kCLOCK_LPUART6_ClockRoot_MuxSysPll2Pfd3;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Lpuart6, &rootCfg);

    /* Configure LPUART7 using SYS_PLL2_PFD3_CLK */
    rootCfg.mux = kCLOCK_LPUART7_ClockRoot_MuxSysPll2Pfd3;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Lpuart7, &rootCfg);

    /* Configure LPUART8 using SYS_PLL2_PFD3_CLK */
    rootCfg.mux = kCLOCK_LPUART8_ClockRoot_MuxSysPll2Pfd3;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Lpuart8, &rootCfg);

    /* Configure LPUART9 using SYS_PLL2_PFD3_CLK */
    rootCfg.mux = kCLOCK_LPUART9_ClockRoot_MuxSysPll2Pfd3;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Lpuart9, &rootCfg);

    /* Configure LPUART10 using SYS_PLL2_PFD3_CLK */
    rootCfg.mux = kCLOCK_LPUART10_ClockRoot_MuxSysPll2Pfd3;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Lpuart10, &rootCfg);

    /* Configure LPUART11 using SYS_PLL2_PFD3_CLK */
    rootCfg.mux = kCLOCK_LPUART11_ClockRoot_MuxSysPll2Pfd3;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Lpuart11, &rootCfg);

    /* Configure LPUART12 using SYS_PLL2_PFD3_CLK */
    rootCfg.mux = kCLOCK_LPUART12_ClockRoot_MuxSysPll2Pfd3;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Lpuart12, &rootCfg);


    /* Configure LPI2C1 using SYS_PLL3_DIV2 (240MHz) */
    rootCfg.mux = kCLOCK_LPI2C1_ClockRoot_MuxSysPll3Div2;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Lpi2c1, &rootCfg);

    /* Configure LPI2C2 using SYS_PLL3_DIV2 (240MHz) */
    rootCfg.mux = kCLOCK_LPI2C2_ClockRoot_MuxSysPll3Div2;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Lpi2c2, &rootCfg);

    /* Configure LPI2C3 using SYS_PLL3_DIV2 (240MHz) */
    rootCfg.mux = kCLOCK_LPI2C3_ClockRoot_MuxSysPll3Div2;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Lpi2c3, &rootCfg);

    /* Configure LPI2C4 using SYS_PLL3_DIV2 (240MHz) */
    rootCfg.mux = kCLOCK_LPI2C4_ClockRoot_MuxSysPll3Div2;
    rootCfg.div = 4;
    CLOCK_SetRootClock(kCLOCK_Root_Lpi2c4, &rootCfg);

    /* Configure LPI2C5 using SYS_PLL3_OUT (480MHz) */
    rootCfg.mux = kCLOCK_LPI2C5_ClockRoot_MuxSysPll3Out;
    rootCfg.div = 8;
    CLOCK_SetRootClock(kCLOCK_Root_Lpi2c5, &rootCfg);

    /* Configure LPI2C6 using SYS_PLL3_OUT (480MHz) */
    rootCfg.mux = kCLOCK_LPI2C6_ClockRoot_MuxSysPll3Out;
    rootCfg.div = 8;
    CLOCK_SetRootClock(kCLOCK_Root_Lpi2c6, &rootCfg);


    /* Configure LPSPI1 using SYS_PLL_3_PDF2 (270MHz) */
    rootCfg.mux = kCLOCK_LPSPI1_ClockRoot_MuxSysPll3Pfd2;
    rootCfg.div = 2;
    CLOCK_SetRootClock(kCLOCK_Root_Lpspi1, &rootCfg);

    /* Configure LPSPI2 using SYS_PLL_3_PDF2 (270MHz) */
    rootCfg.mux = kCLOCK_LPSPI2_ClockRoot_MuxSysPll3Pfd2;
    rootCfg.div = 2;
    CLOCK_SetRootClock(kCLOCK_Root_Lpspi2, &rootCfg);

    /* Configure LPSPI3 using SYS_PLL_3_PDF2 (270MHz) */
    rootCfg.mux = kCLOCK_LPSPI3_ClockRoot_MuxSysPll3Pfd2;
    rootCfg.div = 2;
    CLOCK_SetRootClock(kCLOCK_Root_Lpspi3, &rootCfg);

    /* Configure LPSPI4 using SYS_PLL_3_PDF2 (270MHz) */
    rootCfg.mux = kCLOCK_LPSPI4_ClockRoot_MuxSysPll3Pfd2;
    rootCfg.div = 2;
    CLOCK_SetRootClock(kCLOCK_Root_Lpspi4, &rootCfg);

    /* Configure LPSPI5 using SYS_PLL_3_PDF2 (270MHz) */
    rootCfg.mux = kCLOCK_LPSPI5_ClockRoot_MuxSysPll3Pfd2;
    rootCfg.div = 2;
    CLOCK_SetRootClock(kCLOCK_Root_Lpspi5, &rootCfg);

    /* Configure LPSPI6 using SYS_PLL_3_PDF2 (270MHz) */
    rootCfg.mux = kCLOCK_LPSPI6_ClockRoot_MuxSysPll3Pfd2;
    rootCfg.div = 2;
    CLOCK_SetRootClock(kCLOCK_Root_Lpspi6, &rootCfg);


    /* Configure EMV1 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_EMV1_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Emv1, &rootCfg);

    /* Configure EMV2 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_EMV2_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Emv2, &rootCfg);

    /* Configure ENET1 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_ENET1_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Enet1, &rootCfg);

    /* Configure ENET2 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_ENET2_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Enet2, &rootCfg);

    /* Configure ENET_QOS using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_ENET_QOS_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Enet_Qos, &rootCfg);

    /* Configure ENET_25M using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_ENET_25M_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Enet_25m, &rootCfg);

    /* Configure ENET_TIMER1 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_ENET_TIMER1_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Enet_Timer1, &rootCfg);

    /* Configure ENET_TIMER2 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_ENET_TIMER2_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Enet_Timer2, &rootCfg);

    /* Configure ENET_TIMER3 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_ENET_TIMER3_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Enet_Timer3, &rootCfg);

    /* Configure USDHC1 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_USDHC1_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Usdhc1, &rootCfg);

    /* Configure USDHC2 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_USDHC2_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Usdhc2, &rootCfg);

    /* Configure ASRC using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_ASRC_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Asrc, &rootCfg);

    /* Configure MQS using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_MQS_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Mqs, &rootCfg);

    /* Configure MIC using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_MIC_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Mic, &rootCfg);

    /* Configure SPDIF using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_SPDIF_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Spdif, &rootCfg);

    /* Configure SAI1 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_SAI1_ClockRoot_MuxOsc24MOut;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Sai1, &rootCfg);

    /* Configure SAI2 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_SAI2_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Sai2, &rootCfg);

    /* Configure SAI3 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_SAI3_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Sai3, &rootCfg);

    /* Configure SAI4 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_SAI4_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Sai4, &rootCfg);

    /* Configure GC355 using PLL_VIDEO_CLK */
    rootCfg.mux = kCLOCK_GC355_ClockRoot_MuxVideoPllOut;
    rootCfg.div = 2;
    CLOCK_SetRootClock(kCLOCK_Root_Gc355, &rootCfg);

    /* Configure LCDIF using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_LCDIF_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Lcdif, &rootCfg);

    /* Configure LCDIFV2 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_LCDIFV2_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Lcdifv2, &rootCfg);

    /* Configure MIPI_REF using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_MIPI_REF_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Mipi_Ref, &rootCfg);

    /* Configure MIPI_ESC using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_MIPI_ESC_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Mipi_Esc, &rootCfg);

    /* Configure CSI2 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_CSI2_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Csi2, &rootCfg);

    /* Configure CSI2_ESC using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_CSI2_ESC_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Csi2_Esc, &rootCfg);

    /* Configure CSI2_UI using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_CSI2_UI_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Csi2_Ui, &rootCfg);

    /* Configure CSI using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_CSI_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Csi, &rootCfg);

    /* Configure CKO1 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_CKO1_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Cko1, &rootCfg);

    /* Configure CKO2 using OSC_RC_48M_DIV2 */
    rootCfg.mux = kCLOCK_CKO2_ClockRoot_MuxOscRc48MDiv2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Cko2, &rootCfg);

    /* Set SAI1 MCLK1 clock source. */
    IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR, kIOMUXC_GPR_SAI1MClk1Sel, 0);
    /* Set SAI1 MCLK2 clock source. */
    IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR, kIOMUXC_GPR_SAI1MClk2Sel, 3);
    /* Set SAI1 MCLK3 clock source. */
    IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR, kIOMUXC_GPR_SAI1MClk3Sel, 0);
    /* Set SAI2 MCLK3 clock source. */
    IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR, kIOMUXC_GPR_SAI2MClk3Sel, 0);
    /* Set SAI3 MCLK3 clock source. */
    IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR, kIOMUXC_GPR_SAI3MClk3Sel, 0);

    /* Set MQS configuration. */
    IOMUXC_MQSConfig(IOMUXC_GPR, kIOMUXC_MqsPwmOverSampleRate32, 0);
    /* Set ENET Tx clock source. */
    IOMUXC_GPR->GPR4 &= ~IOMUXC_GPR_GPR4_ENET_TX_CLK_SEL_MASK;
    /* Set ENET_1G Tx clock source. */
    IOMUXC_GPR->GPR5 = ((IOMUXC_GPR->GPR5 & ~IOMUXC_GPR_GPR5_ENET1G_TX_CLK_SEL_MASK) | IOMUXC_GPR_GPR5_ENET1G_RGMII_EN_MASK);
    /* Set GPT1 High frequency reference clock source. */
    IOMUXC_GPR->GPR22 &= ~IOMUXC_GPR_GPR22_REF_1M_CLK_GPT1_MASK;
    /* Set GPT2 High frequency reference clock source. */
    IOMUXC_GPR->GPR23 &= ~IOMUXC_GPR_GPR23_REF_1M_CLK_GPT2_MASK;
    /* Set GPT3 High frequency reference clock source. */
    IOMUXC_GPR->GPR24 &= ~IOMUXC_GPR_GPR24_REF_1M_CLK_GPT3_MASK;
    /* Set GPT4 High frequency reference clock source. */
    IOMUXC_GPR->GPR25 &= ~IOMUXC_GPR_GPR25_REF_1M_CLK_GPT4_MASK;
    /* Set GPT5 High frequency reference clock source. */
    IOMUXC_GPR->GPR26 &= ~IOMUXC_GPR_GPR26_REF_1M_CLK_GPT5_MASK;
    /* Set GPT6 High frequency reference clock source. */
    IOMUXC_GPR->GPR27 &= ~IOMUXC_GPR_GPR27_REF_1M_CLK_GPT6_MASK;

    #if __CORTEX_M == 7
    SystemCoreClock = CLOCK_GetRootClockFreq(kCLOCK_Root_M7);
    #else
    SystemCoreClock = CLOCK_GetRootClockFreq(kCLOCK_Root_M4);
    #endif
}
