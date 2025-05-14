/*
 * Copyright 2019 NXP
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
 * 3. Call CLOCK_SetMux() to configure corresponding clock source for target clock out.
 *
 * 4. Call CLOCK_SetDiv() to configure corresponding clock divider for target clock out.
 *
 * 5. Call CLOCK_SetXtalFreq() to set XTAL frequency based on board settings.
 *
 */

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Clocks v6.0
processor: MIMXRT1011xxxxx
package_id: MIMXRT1011DAE5A
mcu_data: ksdk2_0
processor_version: 0.0.1
board: MIMXRT1010-EVK
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

#include CLOCK_CONFIG_H
#include "fsl_iomuxc.h"

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

/*******************************************************************************
 ********************** Configuration BOARD_BootClockRUN ***********************
 ******************************************************************************/
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockRUN
called_from_default_init: true
outputs:
- {id: ADC_ALT_CLK.outFreq, value: 40 MHz}
- {id: CKIL_SYNC_CLK_ROOT.outFreq, value: 32.768 kHz}
- {id: CLK_1M.outFreq, value: 1 MHz}
- {id: CLK_24M.outFreq, value: 24 MHz}
- {id: CORE_CLK_ROOT.outFreq, value: 500 MHz}
- {id: ENET_500M_REF_CLK.outFreq, value: 500 MHz}
- {id: FLEXIO1_CLK_ROOT.outFreq, value: 30 MHz}
- {id: FLEXSPI_CLK_ROOT.outFreq, value: 132 MHz}
- {id: GPT1_ipg_clk_highfreq.outFreq, value: 62.5 MHz}
- {id: GPT2_ipg_clk_highfreq.outFreq, value: 62.5 MHz}
- {id: IPG_CLK_ROOT.outFreq, value: 125 MHz}
- {id: LPI2C_CLK_ROOT.outFreq, value: 60 MHz}
- {id: LPSPI_CLK_ROOT.outFreq, value: 105.6 MHz}
- {id: MQS_MCLK.outFreq, value: 1080/17 MHz}
- {id: PERCLK_CLK_ROOT.outFreq, value: 62.5 MHz}
- {id: SAI1_CLK_ROOT.outFreq, value: 1080/17 MHz}
- {id: SAI1_MCLK1.outFreq, value: 1080/17 MHz}
- {id: SAI1_MCLK2.outFreq, value: 1080/17 MHz}
- {id: SAI1_MCLK3.outFreq, value: 30 MHz}
- {id: SAI3_CLK_ROOT.outFreq, value: 1080/17 MHz}
- {id: SAI3_MCLK1.outFreq, value: 1080/17 MHz}
- {id: SAI3_MCLK3.outFreq, value: 30 MHz}
- {id: SPDIF0_CLK_ROOT.outFreq, value: 30 MHz}
- {id: TRACE_CLK_ROOT.outFreq, value: 352/3 MHz}
- {id: UART_CLK_ROOT.outFreq, value: 80 MHz}
settings:
- {id: CCM.ADC_ACLK_PODF.scale, value: '12', locked: true}
- {id: CCM.AHB_PODF.scale, value: '1', locked: true}
- {id: CCM.FLEXSPI_PODF.scale, value: '4', locked: true}
- {id: CCM.IPG_PODF.scale, value: '4'}
- {id: CCM.LPSPI_PODF.scale, value: '5', locked: true}
- {id: CCM.PERCLK_PODF.scale, value: '2', locked: true}
- {id: CCM.PRE_PERIPH_CLK_SEL.sel, value: CCM_ANALOG.ENET_500M_REF_CLK}
- {id: CCM.SAI1_CLK_SEL.sel, value: CCM_ANALOG.PLL3_PFD2_CLK}
- {id: CCM.SAI3_CLK_SEL.sel, value: CCM_ANALOG.PLL3_PFD2_CLK}
- {id: CCM.TRACE_PODF.scale, value: '3', locked: true}
- {id: CCM_ANALOG.PLL2.denom, value: '1'}
- {id: CCM_ANALOG.PLL2.div, value: '22'}
- {id: CCM_ANALOG.PLL2.num, value: '0'}
- {id: CCM_ANALOG.PLL2_BYPASS.sel, value: CCM_ANALOG.PLL2_OUT_CLK}
- {id: CCM_ANALOG.PLL2_PFD0_BYPASS.sel, value: CCM_ANALOG.PLL2_PFD0}
- {id: CCM_ANALOG.PLL2_PFD1_BYPASS.sel, value: CCM_ANALOG.PLL2_PFD1}
- {id: CCM_ANALOG.PLL2_PFD2_BYPASS.sel, value: CCM_ANALOG.PLL2_PFD2}
- {id: CCM_ANALOG.PLL2_PFD2_DIV.scale, value: '18', locked: true}
- {id: CCM_ANALOG.PLL2_PFD2_MUL.scale, value: '18', locked: true}
- {id: CCM_ANALOG.PLL2_PFD3_BYPASS.sel, value: CCM_ANALOG.PLL2_PFD3}
- {id: CCM_ANALOG.PLL2_PFD3_DIV.scale, value: '18', locked: true}
- {id: CCM_ANALOG.PLL2_PFD3_MUL.scale, value: '18', locked: true}
- {id: CCM_ANALOG.PLL3_BYPASS.sel, value: CCM_ANALOG.PLL3}
- {id: CCM_ANALOG.PLL3_PFD0_BYPASS.sel, value: CCM_ANALOG.PLL3_PFD0}
- {id: CCM_ANALOG.PLL3_PFD0_DIV.scale, value: '22', locked: true}
- {id: CCM_ANALOG.PLL3_PFD0_MUL.scale, value: '18', locked: true}
- {id: CCM_ANALOG.PLL3_PFD1_BYPASS.sel, value: CCM_ANALOG.PLL3_PFD1}
- {id: CCM_ANALOG.PLL3_PFD2_BYPASS.sel, value: CCM_ANALOG.PLL3_PFD2}
- {id: CCM_ANALOG.PLL3_PFD3_BYPASS.sel, value: CCM_ANALOG.PLL3_PFD3}
- {id: CCM_ANALOG.PLL3_PFD3_DIV.scale, value: '18', locked: true}
- {id: CCM_ANALOG.PLL3_PFD3_MUL.scale, value: '18', locked: true}
- {id: CCM_ANALOG.PLL6_BYPASS.sel, value: CCM_ANALOG.PLL6}
- {id: CCM_ANALOG_PLL_USB1_POWER_CFG, value: 'Yes'}
sources:
- {id: XTALOSC24M.OSC.outFreq, value: 24 MHz, enabled: true}
- {id: XTALOSC24M.RTC_OSC.outFreq, value: 32.768 kHz, enabled: true}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/*******************************************************************************
 * Variables for BOARD_BootClockRUN configuration
 ******************************************************************************/
const clock_sys_pll_config_t sysPllConfig_BOARD_BootClockRUN = {
    .loopDivider = 1, /* PLL loop divider, Fout = Fin * ( 20 + loopDivider*2 + numerator / denominator ) */
    .numerator = 0,   /* 30 bit numerator of fractional loop divider */
    .denominator = 1, /* 30 bit denominator of fractional loop divider */
    .src = 0,         /* Bypass clock source, 0 - OSC 24M, 1 - CLK1_P and CLK1_N */
};
const clock_usb_pll_config_t usb1PllConfig_BOARD_BootClockRUN = {
    .loopDivider = 0, /* PLL loop divider, Fout = Fin * 20 */
    .src = 0,         /* Bypass clock source, 0 - OSC 24M, 1 - CLK1_P and CLK1_N */
};
const clock_enet_pll_config_t enetPllConfig_BOARD_BootClockRUN = {
    .enableClkOutput500M = true, /* Enable the PLL providing the ENET 500MHz reference clock */
    .src = 0,                    /* Bypass clock source, 0 - OSC 24M, 1 - CLK1_P and CLK1_N */
};
/*******************************************************************************
 * Code for BOARD_BootClockRUN configuration
 ******************************************************************************/
void BOARD_BootClockRUN(void) {
    /* Init RTC OSC clock frequency. */
    CLOCK_SetRtcXtalFreq(32768U);
    /* Enable 1MHz clock output. */
    XTALOSC24M->OSC_CONFIG2 |= XTALOSC24M_OSC_CONFIG2_ENABLE_1M_MASK;
    /* Use free 1MHz clock output. */
    XTALOSC24M->OSC_CONFIG2 &= ~XTALOSC24M_OSC_CONFIG2_MUX_1M_MASK;
    /* Set XTAL 24MHz clock frequency. */
    CLOCK_SetXtalFreq(24000000U);
    /* Enable XTAL 24MHz clock source. */
    CLOCK_InitExternalClk(0);
    /* Enable internal RC. */
    CLOCK_InitRcOsc24M();
    /* Switch clock source to external OSC. */
    CLOCK_SwitchOsc(kCLOCK_XtalOsc);
    /* Set Oscillator ready counter value. */
    CCM->CCR = (CCM->CCR & (~CCM_CCR_OSCNT_MASK)) | CCM_CCR_OSCNT(127);
    /* Setting PeriphClk2Mux and PeriphMux to provide stable clock before PLLs are initialed */
    CLOCK_SetMux(kCLOCK_PeriphClk2Mux, 1); /* Set PERIPH_CLK2 MUX to OSC */
    CLOCK_SetMux(kCLOCK_PeriphMux, 1);     /* Set PERIPH_CLK MUX to PERIPH_CLK2 */
    /* Setting the VDD_SOC to 1.5V. It is necessary to config CORE to 500Mhz. */
    DCDC->REG3 = (DCDC->REG3 & (~DCDC_REG3_TRG_MASK)) | DCDC_REG3_TRG(0x12);
    /* Waiting for DCDC_STS_DC_OK bit is asserted */
    while (DCDC_REG0_STS_DC_OK_MASK != (DCDC_REG0_STS_DC_OK_MASK & DCDC->REG0)) {
    }
    /* Set AHB_PODF. */
    CLOCK_SetDiv(kCLOCK_AhbDiv, 0);
    /* Disable IPG clock gate. */
    CLOCK_DisableClock(kCLOCK_Adc1);
    CLOCK_DisableClock(kCLOCK_Xbar1);
    /* Set IPG_PODF. */
    CLOCK_SetDiv(kCLOCK_IpgDiv, 3);
    /* Disable PERCLK clock gate. */
    CLOCK_DisableClock(kCLOCK_Gpt1);
    CLOCK_DisableClock(kCLOCK_Gpt1S);
    CLOCK_DisableClock(kCLOCK_Gpt2);
    CLOCK_DisableClock(kCLOCK_Gpt2S);
    CLOCK_DisableClock(kCLOCK_Pit);
    /* Set PERCLK_PODF. */
    CLOCK_SetDiv(kCLOCK_PerclkDiv, 1);
    /* In SDK projects, external flash (configured by FLEXSPI) will be initialized by dcd.
     * With this macro XIP_EXTERNAL_FLASH, usb1 pll (selected to be FLEXSPI clock source in SDK projects) will be left
     * unchanged. Note: If another clock source is selected for FLEXSPI, user may want to avoid changing that clock as
     * well.*/
    #if !(defined(XIP_EXTERNAL_FLASH) && (XIP_EXTERNAL_FLASH == 1))
    /* Disable Flexspi clock gate. */
    CLOCK_DisableClock(kCLOCK_FlexSpi);
    /* Set FLEXSPI_PODF. */
    CLOCK_SetDiv(kCLOCK_FlexspiDiv, 3);
    /* Set Flexspi clock source. */
    CLOCK_SetMux(kCLOCK_FlexspiMux, 0);
    CLOCK_SetMux(kCLOCK_FlexspiSrcMux, 0);
    #endif
    /* Disable ADC_ACLK_EN clock gate. */
    CCM->CSCMR2 &= ~CCM_CSCMR2_ADC_ACLK_EN_MASK;
    /* Set ADC_ACLK_PODF. */
    CLOCK_SetDiv(kCLOCK_AdcDiv, 11);
    /* Disable LPSPI clock gate. */
    CLOCK_DisableClock(kCLOCK_Lpspi1);
    CLOCK_DisableClock(kCLOCK_Lpspi2);
    /* Set LPSPI_PODF. */
    CLOCK_SetDiv(kCLOCK_LpspiDiv, 4);
    /* Set Lpspi clock source. */
    CLOCK_SetMux(kCLOCK_LpspiMux, 2);
    /* Disable TRACE clock gate. */
    CLOCK_DisableClock(kCLOCK_Trace);
    /* Set TRACE_PODF. */
    CLOCK_SetDiv(kCLOCK_TraceDiv, 2);
    /* Set Trace clock source. */
    CLOCK_SetMux(kCLOCK_TraceMux, 2);
    /* Disable SAI1 clock gate. */
    CLOCK_DisableClock(kCLOCK_Sai1);
    /* Set SAI1_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Sai1PreDiv, 3);
    /* Set SAI1_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Sai1Div, 1);
    /* Set Sai1 clock source. */
    CLOCK_SetMux(kCLOCK_Sai1Mux, 0);
    /* Disable SAI3 clock gate. */
    CLOCK_DisableClock(kCLOCK_Sai3);
    /* Set SAI3_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Sai3PreDiv, 3);
    /* Set SAI3_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Sai3Div, 1);
    /* Set Sai3 clock source. */
    CLOCK_SetMux(kCLOCK_Sai3Mux, 0);
    /* Disable Lpi2c clock gate. */
    CLOCK_DisableClock(kCLOCK_Lpi2c1);
    CLOCK_DisableClock(kCLOCK_Lpi2c2);
    /* Set LPI2C_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, 0);
    /* Set Lpi2c clock source. */
    CLOCK_SetMux(kCLOCK_Lpi2cMux, 0);
    /* Disable UART clock gate. */
    CLOCK_DisableClock(kCLOCK_Lpuart1);
    CLOCK_DisableClock(kCLOCK_Lpuart2);
    CLOCK_DisableClock(kCLOCK_Lpuart3);
    CLOCK_DisableClock(kCLOCK_Lpuart4);
    /* Set UART_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_UartDiv, 1);
    /* Set Uart clock source. */
    CLOCK_SetMux(kCLOCK_UartMux, 0);
    /* Disable SPDIF clock gate. */
    CLOCK_DisableClock(kCLOCK_Spdif);
    /* Set SPDIF0_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Spdif0PreDiv, 1);
    /* Set SPDIF0_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Spdif0Div, 7);
    /* Set Spdif clock source. */
    CLOCK_SetMux(kCLOCK_SpdifMux, 3);
    /* Disable Flexio1 clock gate. */
    CLOCK_DisableClock(kCLOCK_Flexio1);
    /* Set FLEXIO1_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Flexio1PreDiv, 1);
    /* Set FLEXIO1_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Flexio1Div, 7);
    /* Set Flexio1 clock source. */
    CLOCK_SetMux(kCLOCK_Flexio1Mux, 3);
    /* Set Pll3 sw clock source. */
    CLOCK_SetMux(kCLOCK_Pll3SwMux, 0);
    /* Init System PLL. */
    CLOCK_InitSysPll(&sysPllConfig_BOARD_BootClockRUN);
    /* Init System pfd0. */
    CLOCK_InitSysPfd(kCLOCK_Pfd0, 27);
    /* Init System pfd1. */
    CLOCK_InitSysPfd(kCLOCK_Pfd1, 16);
    /* Init System pfd2. */
    CLOCK_InitSysPfd(kCLOCK_Pfd2, 18);
    /* Init System pfd3. */
    CLOCK_InitSysPfd(kCLOCK_Pfd3, 18);
    /* In SDK projects, external flash (configured by FLEXSPI) will be initialized by dcd.
     * With this macro XIP_EXTERNAL_FLASH, usb1 pll (selected to be FLEXSPI clock source in SDK projects) will be left
     * unchanged. Note: If another clock source is selected for FLEXSPI, user may want to avoid changing that clock as
     * well.*/
    #if !(defined(XIP_EXTERNAL_FLASH) && (XIP_EXTERNAL_FLASH == 1))
    /* Init Usb1 PLL. */
    CLOCK_InitUsb1Pll(&usb1PllConfig_BOARD_BootClockRUN);
    /* Init Usb1 pfd0. */
    CLOCK_InitUsb1Pfd(kCLOCK_Pfd0, 22);
    /* Init Usb1 pfd1. */
    CLOCK_InitUsb1Pfd(kCLOCK_Pfd1, 16);
    /* Init Usb1 pfd2. */
    CLOCK_InitUsb1Pfd(kCLOCK_Pfd2, 17);
    /* Init Usb1 pfd3. */
    CLOCK_InitUsb1Pfd(kCLOCK_Pfd3, 18);
    /* Disable Usb1 PLL output for USBPHY1. */
    CCM_ANALOG->PLL_USB1 &= ~CCM_ANALOG_PLL_USB1_EN_USB_CLKS_MASK;
    #endif
    /* DeInit Audio PLL. */
    CLOCK_DeinitAudioPll();
    /* Bypass Audio PLL. */
    CLOCK_SetPllBypass(CCM_ANALOG, kCLOCK_PllAudio, 1);
    /* Set divider for Audio PLL. */
    CCM_ANALOG->MISC2 &= ~CCM_ANALOG_MISC2_AUDIO_DIV_LSB_MASK;
    CCM_ANALOG->MISC2 &= ~CCM_ANALOG_MISC2_AUDIO_DIV_MSB_MASK;
    /* Enable Audio PLL output. */
    CCM_ANALOG->PLL_AUDIO |= CCM_ANALOG_PLL_AUDIO_ENABLE_MASK;
    /* Init Enet PLL. */
    CLOCK_InitEnetPll(&enetPllConfig_BOARD_BootClockRUN);
    /* Set preperiph clock source. */
    CLOCK_SetMux(kCLOCK_PrePeriphMux, 3);
    /* Set periph clock source. */
    CLOCK_SetMux(kCLOCK_PeriphMux, 0);
    /* Set periph clock2 clock source. */
    CLOCK_SetMux(kCLOCK_PeriphClk2Mux, 0);
    /* Set per clock source. */
    CLOCK_SetMux(kCLOCK_PerclkMux, 0);
    /* Set clock out1 divider. */
    CCM->CCOSR = (CCM->CCOSR & (~CCM_CCOSR_CLKO1_DIV_MASK)) | CCM_CCOSR_CLKO1_DIV(0);
    /* Set clock out1 source. */
    CCM->CCOSR = (CCM->CCOSR & (~CCM_CCOSR_CLKO1_SEL_MASK)) | CCM_CCOSR_CLKO1_SEL(1);
    /* Set clock out2 divider. */
    CCM->CCOSR = (CCM->CCOSR & (~CCM_CCOSR_CLKO2_DIV_MASK)) | CCM_CCOSR_CLKO2_DIV(0);
    /* Set clock out2 source. */
    CCM->CCOSR = (CCM->CCOSR & (~CCM_CCOSR_CLKO2_SEL_MASK)) | CCM_CCOSR_CLKO2_SEL(18);
    /* Set clock out1 drives clock out1. */
    CCM->CCOSR &= ~CCM_CCOSR_CLK_OUT_SEL_MASK;
    /* Disable clock out1. */
    CCM->CCOSR &= ~CCM_CCOSR_CLKO1_EN_MASK;
    /* Disable clock out2. */
    CCM->CCOSR &= ~CCM_CCOSR_CLKO2_EN_MASK;
    /* Set SAI1 MCLK1 clock source. */
    IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR, kIOMUXC_GPR_SAI1MClk1Sel, 0);
    /* Set SAI1 MCLK2 clock source. */
    IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR, kIOMUXC_GPR_SAI1MClk2Sel, 0);
    /* Set SAI1 MCLK3 clock source. */
    IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR, kIOMUXC_GPR_SAI1MClk3Sel, 0);
    /* Set SAI3 MCLK3 clock source. */
    IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR, kIOMUXC_GPR_SAI3MClk3Sel, 0);
    /* Set MQS configuration. */
    IOMUXC_MQSConfig(IOMUXC_GPR, kIOMUXC_MqsPwmOverSampleRate32, 0);
    /* Set GPT1 High frequency reference clock source. */
    IOMUXC_GPR->GPR5 &= ~IOMUXC_GPR_GPR5_VREF_1M_CLK_GPT1_MASK;
    /* Set GPT2 High frequency reference clock source. */
    IOMUXC_GPR->GPR5 &= ~IOMUXC_GPR_GPR5_VREF_1M_CLK_GPT2_MASK;
    /* Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKRUN_CORE_CLOCK;
}
