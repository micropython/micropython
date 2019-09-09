/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * How to setup clock using clock driver functions:
 *
 * 1. CLOCK_SetSimSafeDivs, to make sure core clock, bus clock, flexbus clock
 *    and flash clock are in allowed range during clock mode switch.
 *
 * 2. Call CLOCK_Osc0Init to setup OSC clock, if it is used in target mode.
 *
 * 3. Set MCG configuration, MCG includes three parts: FLL clock, PLL clock and
 *    internal reference clock(MCGIRCLK). Follow the steps to setup:
 *
 *    1). Call CLOCK_BootToXxxMode to set MCG to target mode.
 *
 *    2). If target mode is FBI/BLPI/PBI mode, the MCGIRCLK has been configured
 *        correctly. For other modes, need to call CLOCK_SetInternalRefClkConfig
 *        explicitly to setup MCGIRCLK.
 *
 *    3). Don't need to configure FLL explicitly, because if target mode is FLL
 *        mode, then FLL has been configured by the function CLOCK_BootToXxxMode,
 *        if the target mode is not FLL mode, the FLL is disabled.
 *
 *    4). If target mode is PEE/PBE/PEI/PBI mode, then the related PLL has been
 *        setup by CLOCK_BootToXxxMode. In FBE/FBI/FEE/FBE mode, the PLL could
 *        be enabled independently, call CLOCK_EnablePll0 explicitly in this case.
 *
 * 4. Call CLOCK_SetSimConfig to set the clock configuration in SIM.
 */

/* TEXT BELOW IS USED AS SETTING FOR THE CLOCKS TOOL *****************************
!!ClocksProfile
product: Clocks v1.0
processor: MK64FN1M0xxx12
package_id: MK64FN1M0VLL12
mcu_data: ksdk2_0
processor_version: 1.0.1
board: FRDM-K64F
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE CLOCKS TOOL **/

#include "fsl_smc.h"
#include "clock_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MCG_PLL_DISABLE 0U             /*!< MCGPLLCLK disabled */
#define OSC_CAP0P 0U                   /*!< Oscillator 0pF capacitor load */
#define OSC_ER_CLK_DISABLE 0U          /*!< Disable external reference clock */
#define SIM_OSC32KSEL_RTC32KCLK_CLK 2U /*!< OSC32KSEL select: RTC32KCLK clock (32.768kHz) */
#define SIM_PLLFLLSEL_IRC48MCLK_CLK 3U /*!< PLLFLL select: IRC48MCLK clock */
#define SIM_PLLFLLSEL_MCGPLLCLK_CLK 1U /*!< PLLFLL select: MCGPLLCLK clock */

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* System clock frequency. */
extern uint32_t SystemCoreClock;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*FUNCTION**********************************************************************
 *
 * Function Name : CLOCK_CONFIG_SetFllExtRefDiv
 * Description   : Configure FLL external reference divider (FRDIV).
 * Param frdiv   : The value to set FRDIV.
 *
 *END**************************************************************************/
static void CLOCK_CONFIG_SetFllExtRefDiv(uint8_t frdiv)
{
    MCG->C1 = ((MCG->C1 & ~MCG_C1_FRDIV_MASK) | MCG_C1_FRDIV(frdiv));
}

/*******************************************************************************
 ********************** Configuration BOARD_BootClockRUN ***********************
 ******************************************************************************/
/* TEXT BELOW IS USED AS SETTING FOR THE CLOCKS TOOL *****************************
!!Configuration
name: BOARD_BootClockRUN
outputs:
- {id: Bus_clock.outFreq, value: 60 MHz}
- {id: Core_clock.outFreq, value: 120 MHz, locked: true, accuracy: '0.001'}
- {id: Flash_clock.outFreq, value: 24 MHz}
- {id: FlexBus_clock.outFreq, value: 40 MHz}
- {id: LPO_clock.outFreq, value: 1 kHz}
- {id: MCGFFCLK.outFreq, value: 1.5625 MHz}
- {id: MCGIRCLK.outFreq, value: 32.768 kHz}
- {id: OSCERCLK.outFreq, value: 50 MHz}
- {id: PLLFLLCLK.outFreq, value: 120 MHz}
- {id: System_clock.outFreq, value: 120 MHz}
settings:
- {id: MCGMode, value: PEE}
- {id: MCG.FCRDIV.scale, value: '1', locked: true}
- {id: MCG.FRDIV.scale, value: '32'}
- {id: MCG.IREFS.sel, value: MCG.FRDIV}
- {id: MCG.PLLS.sel, value: MCG.PLL}
- {id: MCG.PRDIV.scale, value: '20', locked: true}
- {id: MCG.VDIV.scale, value: '48', locked: true}
- {id: MCG_C1_IRCLKEN_CFG, value: Enabled}
- {id: MCG_C2_RANGE0_CFG, value: Very_high}
- {id: MCG_C2_RANGE0_FRDIV_CFG, value: Very_high}
- {id: OSC_CR_ERCLKEN_CFG, value: Enabled}
- {id: RTCCLKOUTConfig, value: 'yes'}
- {id: RTC_CR_OSCE_CFG, value: Enabled}
- {id: RTC_CR_OSC_CAP_LOAD_CFG, value: SC10PF}
- {id: SIM.OSC32KSEL.sel, value: RTC.RTC32KCLK}
- {id: SIM.OUTDIV2.scale, value: '2'}
- {id: SIM.OUTDIV3.scale, value: '3'}
- {id: SIM.OUTDIV4.scale, value: '5'}
- {id: SIM.PLLFLLSEL.sel, value: MCG.MCGPLLCLK}
- {id: SIM.RTCCLKOUTSEL.sel, value: RTC.RTC32KCLK}
- {id: SIM.SDHCSRCSEL.sel, value: OSC.OSCERCLK}
- {id: SIM.TIMESRCSEL.sel, value: OSC.OSCERCLK}
- {id: SIM.USBDIV.scale, value: '5'}
- {id: SIM.USBFRAC.scale, value: '2'}
- {id: SIM.USBSRCSEL.sel, value: SIM.USBDIV}
sources:
- {id: OSC.OSC.outFreq, value: 50 MHz, enabled: true}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE CLOCKS TOOL **/

/*******************************************************************************
 * Variables for BOARD_BootClockRUN configuration
 ******************************************************************************/
const mcg_config_t mcgConfig_BOARD_BootClockRUN = {
    .mcgMode         = kMCG_ModePEE,      /* PEE - PLL Engaged External */
    .irclkEnableMode = kMCG_IrclkEnable,  /* MCGIRCLK enabled, MCGIRCLK disabled in STOP mode */
    .ircs            = kMCG_IrcSlow,      /* Slow internal reference clock selected */
    .fcrdiv          = 0x0U,              /* Fast IRC divider: divided by 1 */
    .frdiv           = 0x0U,              /* FLL reference clock divider: divided by 32 */
    .drs             = kMCG_DrsLow,       /* Low frequency range */
    .dmx32           = kMCG_Dmx32Default, /* DCO has a default range of 25% */
    .oscsel          = kMCG_OscselOsc,    /* Selects System Oscillator (OSCCLK) */
    .pll0Config =
        {
            .enableMode = MCG_PLL_DISABLE, /* MCGPLLCLK disabled */
            .prdiv      = 0x13U,           /* PLL Reference divider: divided by 20 */
            .vdiv       = 0x18U,           /* VCO divider: multiplied by 48 */
        },
};
const sim_clock_config_t simConfig_BOARD_BootClockRUN = {
    .pllFllSel = SIM_PLLFLLSEL_MCGPLLCLK_CLK, /* PLLFLL select: MCGPLLCLK clock */
    .er32kSrc  = SIM_OSC32KSEL_RTC32KCLK_CLK, /* OSC32KSEL select: RTC32KCLK clock (32.768kHz) */
    .clkdiv1   = 0x1240000U,                  /* SIM_CLKDIV1 - OUTDIV1: /1, OUTDIV2: /2, OUTDIV3: /3, OUTDIV4: /5 */
};
const osc_config_t oscConfig_BOARD_BootClockRUN = {
    .freq        = 50000000U,    /* Oscillator frequency: 50000000Hz */
    .capLoad     = (OSC_CAP0P),  /* Oscillator capacity load: 0pF */
    .workMode    = kOSC_ModeExt, /* Use external clock */
    .oscerConfig = {
        .enableMode =
            kOSC_ErClkEnable, /* Enable external reference clock, disable external reference clock in STOP mode */
    }};

/*******************************************************************************
 * Code for BOARD_BootClockRUN configuration
 ******************************************************************************/
void BOARD_BootClockRUN(void)
{
    /* Set the system clock dividers in SIM to safe value. */
    CLOCK_SetSimSafeDivs();
    /* Initializes OSC0 according to board configuration. */
    CLOCK_InitOsc0(&oscConfig_BOARD_BootClockRUN);
    CLOCK_SetXtal0Freq(oscConfig_BOARD_BootClockRUN.freq);
    /* Configure the Internal Reference clock (MCGIRCLK). */
    CLOCK_SetInternalRefClkConfig(mcgConfig_BOARD_BootClockRUN.irclkEnableMode, mcgConfig_BOARD_BootClockRUN.ircs,
                                  mcgConfig_BOARD_BootClockRUN.fcrdiv);
    /* Configure FLL external reference divider (FRDIV). */
    CLOCK_CONFIG_SetFllExtRefDiv(mcgConfig_BOARD_BootClockRUN.frdiv);
    /* Set MCG to PEE mode. */
    CLOCK_BootToPeeMode(mcgConfig_BOARD_BootClockRUN.oscsel, kMCG_PllClkSelPll0,
                        &mcgConfig_BOARD_BootClockRUN.pll0Config);
    /* Set the clock configuration in SIM module. */
    CLOCK_SetSimConfig(&simConfig_BOARD_BootClockRUN);
    /* Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKRUN_CORE_CLOCK;
}

/*******************************************************************************
 ********************* Configuration BOARD_BootClockVLPR ***********************
 ******************************************************************************/
/* TEXT BELOW IS USED AS SETTING FOR THE CLOCKS TOOL *****************************
!!Configuration
name: BOARD_BootClockVLPR
outputs:
- {id: Bus_clock.outFreq, value: 4 MHz}
- {id: Core_clock.outFreq, value: 4 MHz, locked: true, accuracy: '0.001'}
- {id: Flash_clock.outFreq, value: 800 kHz}
- {id: FlexBus_clock.outFreq, value: 4 MHz}
- {id: LPO_clock.outFreq, value: 1 kHz}
- {id: MCGIRCLK.outFreq, value: 4 MHz}
- {id: System_clock.outFreq, value: 4 MHz}
settings:
- {id: MCGMode, value: BLPI}
- {id: powerMode, value: VLPR}
- {id: MCG.CLKS.sel, value: MCG.IRCS}
- {id: MCG.FCRDIV.scale, value: '1'}
- {id: MCG.FRDIV.scale, value: '32'}
- {id: MCG.IRCS.sel, value: MCG.FCRDIV}
- {id: MCG_C1_IRCLKEN_CFG, value: Enabled}
- {id: MCG_C2_RANGE0_CFG, value: Very_high}
- {id: MCG_C2_RANGE0_FRDIV_CFG, value: Very_high}
- {id: RTC_CR_OSCE_CFG, value: Enabled}
- {id: RTC_CR_OSC_CAP_LOAD_CFG, value: SC10PF}
- {id: SIM.OSC32KSEL.sel, value: RTC.RTC32KCLK}
- {id: SIM.OUTDIV3.scale, value: '1'}
- {id: SIM.OUTDIV4.scale, value: '5'}
- {id: SIM.PLLFLLSEL.sel, value: IRC48M.IRC48MCLK}
- {id: SIM.RTCCLKOUTSEL.sel, value: RTC.RTC32KCLK}
sources:
- {id: OSC.OSC.outFreq, value: 50 MHz}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE CLOCKS TOOL **/

/*******************************************************************************
 * Variables for BOARD_BootClockVLPR configuration
 ******************************************************************************/
const mcg_config_t mcgConfig_BOARD_BootClockVLPR = {
    .mcgMode         = kMCG_ModeBLPI,     /* BLPI - Bypassed Low Power Internal */
    .irclkEnableMode = kMCG_IrclkEnable,  /* MCGIRCLK enabled, MCGIRCLK disabled in STOP mode */
    .ircs            = kMCG_IrcFast,      /* Fast internal reference clock selected */
    .fcrdiv          = 0x0U,              /* Fast IRC divider: divided by 1 */
    .frdiv           = 0x0U,              /* FLL reference clock divider: divided by 32 */
    .drs             = kMCG_DrsLow,       /* Low frequency range */
    .dmx32           = kMCG_Dmx32Default, /* DCO has a default range of 25% */
    .oscsel          = kMCG_OscselOsc,    /* Selects System Oscillator (OSCCLK) */
    .pll0Config =
        {
            .enableMode = MCG_PLL_DISABLE, /* MCGPLLCLK disabled */
            .prdiv      = 0x0U,            /* PLL Reference divider: divided by 1 */
            .vdiv       = 0x0U,            /* VCO divider: multiplied by 24 */
        },
};
const sim_clock_config_t simConfig_BOARD_BootClockVLPR = {
    .pllFllSel = SIM_PLLFLLSEL_IRC48MCLK_CLK, /* PLLFLL select: IRC48MCLK clock */
    .er32kSrc  = SIM_OSC32KSEL_RTC32KCLK_CLK, /* OSC32KSEL select: RTC32KCLK clock (32.768kHz) */
    .clkdiv1   = 0x40000U,                    /* SIM_CLKDIV1 - OUTDIV1: /1, OUTDIV2: /1, OUTDIV3: /1, OUTDIV4: /5 */
};
const osc_config_t oscConfig_BOARD_BootClockVLPR = {
    .freq        = 0U,           /* Oscillator frequency: 0Hz */
    .capLoad     = (OSC_CAP0P),  /* Oscillator capacity load: 0pF */
    .workMode    = kOSC_ModeExt, /* Use external clock */
    .oscerConfig = {
        .enableMode = OSC_ER_CLK_DISABLE, /* Disable external reference clock */
    }};

/*******************************************************************************
 * Code for BOARD_BootClockVLPR configuration
 ******************************************************************************/
void BOARD_BootClockVLPR(void)
{
    /* Set the system clock dividers in SIM to safe value. */
    CLOCK_SetSimSafeDivs();
    /* Set MCG to BLPI mode. */
    CLOCK_BootToBlpiMode(mcgConfig_BOARD_BootClockVLPR.fcrdiv, mcgConfig_BOARD_BootClockVLPR.ircs,
                         mcgConfig_BOARD_BootClockVLPR.irclkEnableMode);
    /* Set the clock configuration in SIM module. */
    CLOCK_SetSimConfig(&simConfig_BOARD_BootClockVLPR);
    /* Set VLPR power mode. */
    SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
#if (defined(FSL_FEATURE_SMC_HAS_LPWUI) && FSL_FEATURE_SMC_HAS_LPWUI)
    SMC_SetPowerModeVlpr(SMC, false);
#else
    SMC_SetPowerModeVlpr(SMC);
#endif
    while (SMC_GetPowerModeState(SMC) != kSMC_PowerStateVlpr)
    {
    }
    /* Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKVLPR_CORE_CLOCK;
}
