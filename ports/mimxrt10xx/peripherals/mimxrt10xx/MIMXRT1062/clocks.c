// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT
/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "mpconfigport.h"

#include "fsl_clock.h"
#include "fsl_iomuxc.h"
#include "fsl_device_registers.h"

#include "clocks.h"

#define BOARD_XTAL0_CLK_HZ                         24000000U  /*!< Board xtal0 frequency in Hz */
#define BOARD_XTAL32K_CLK_HZ                          32768U  /*!< Board xtal32k frequency in Hz */
#define BOARD_BOOTCLOCKRUN_CORE_CLOCK             600000000U  /*!< Core clock frequency: 600000000Hz */

const clock_arm_pll_config_t armPllConfig_BOARD_BootClockRUN =
{
    .loopDivider = 100,                           /* PLL loop divider, Fout = Fin * 50 */
    .src = 0,                                     /* Bypass clock source, 0 - OSC 24M, 1 - CLK1_P and CLK1_N */
};
const clock_sys_pll_config_t sysPllConfig_BOARD_BootClockRUN =
{
    .loopDivider = 1,                             /* PLL loop divider, Fout = Fin * ( 20 + loopDivider*2 + numerator / denominator ) */
    .numerator = 0,                               /* 30 bit numerator of fractional loop divider */
    .denominator = 1,                             /* 30 bit denominator of fractional loop divider */
    .src = 0,                                     /* Bypass clock source, 0 - OSC 24M, 1 - CLK1_P and CLK1_N */
};
const clock_usb_pll_config_t usb1PllConfig_BOARD_BootClockRUN =
{
    .loopDivider = 0,                             /* PLL loop divider, Fout = Fin * 20 */
    .src = 0,                                     /* Bypass clock source, 0 - OSC 24M, 1 - CLK1_P and CLK1_N */
};

// Based on the hello_world example in the SDK
void clocks_init(void) {
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
    /* Setting the VDD_SOC to 1.275V. It is necessary to config AHB to 600Mhz. */
    DCDC->REG3 = (DCDC->REG3 & (~DCDC_REG3_TRG_MASK)) | DCDC_REG3_TRG(0x13);
    /* Waiting for DCDC_STS_DC_OK bit is asserted */
    while (DCDC_REG0_STS_DC_OK_MASK != (DCDC_REG0_STS_DC_OK_MASK & DCDC->REG0)) {
    }
    /* Set AHB_PODF. */
    CLOCK_SetDiv(kCLOCK_AhbDiv, 0);
    /* Disable IPG clock gate. */
    CLOCK_DisableClock(kCLOCK_Adc1);
    CLOCK_DisableClock(kCLOCK_Adc2);
    CLOCK_DisableClock(kCLOCK_Xbar1);
    CLOCK_DisableClock(kCLOCK_Xbar2);
    /* Set IPG_PODF. */
    CLOCK_SetDiv(kCLOCK_IpgDiv, 3);
    /* Set ARM_PODF. */
    CLOCK_SetDiv(kCLOCK_ArmDiv, 1);
    /* Set PERIPH_CLK2_PODF. */
    CLOCK_SetDiv(kCLOCK_PeriphClk2Div, 0);
    /* Disable PERCLK clock gate. */
    CLOCK_DisableClock(kCLOCK_Gpt1);
    CLOCK_DisableClock(kCLOCK_Gpt1S);
    CLOCK_DisableClock(kCLOCK_Gpt2);
    CLOCK_DisableClock(kCLOCK_Gpt2S);
    CLOCK_DisableClock(kCLOCK_Pit);
    /* Set PERCLK_PODF. */
    CLOCK_SetDiv(kCLOCK_PerclkDiv, 1);
    /* Disable USDHC1 clock gate. */
    CLOCK_DisableClock(kCLOCK_Usdhc1);
    /* Set USDHC1_PODF. */
    CLOCK_SetDiv(kCLOCK_Usdhc1Div, 1);
    /* Set Usdhc1 clock source. */
    CLOCK_SetMux(kCLOCK_Usdhc1Mux, 0);
    /* Disable USDHC2 clock gate. */
    CLOCK_DisableClock(kCLOCK_Usdhc2);
    /* Set USDHC2_PODF. */
    CLOCK_SetDiv(kCLOCK_Usdhc2Div, 1);
    /* Set Usdhc2 clock source. */
    CLOCK_SetMux(kCLOCK_Usdhc2Mux, 0);
    /* In SDK projects, SDRAM (configured by SEMC) will be initialized in either debug script or dcd.
     * With this macro SKIP_SYSCLK_INIT, system pll (selected to be SEMC source clock in SDK projects) will be left unchanged.
     * Note: If another clock source is selected for SEMC, user may want to avoid changing that clock as well.*/
    #ifndef SKIP_SYSCLK_INIT
    /* Disable Semc clock gate. */
    CLOCK_DisableClock(kCLOCK_Semc);
    /* Set SEMC_PODF. */
    CLOCK_SetDiv(kCLOCK_SemcDiv, 7);
    /* Set Semc alt clock source. */
    CLOCK_SetMux(kCLOCK_SemcAltMux, 0);
    /* Set Semc clock source. */
    CLOCK_SetMux(kCLOCK_SemcMux, 0);
    #endif
    /* Disable LPSPI clock gate. */
    CLOCK_DisableClock(kCLOCK_Lpspi1);
    CLOCK_DisableClock(kCLOCK_Lpspi2);
    CLOCK_DisableClock(kCLOCK_Lpspi3);
    CLOCK_DisableClock(kCLOCK_Lpspi4);
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
    /* Disable SAI2 clock gate. */
    CLOCK_DisableClock(kCLOCK_Sai2);
    /* Set SAI2_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Sai2PreDiv, 3);
    /* Set SAI2_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Sai2Div, 1);
    /* Set Sai2 clock source. */
    CLOCK_SetMux(kCLOCK_Sai2Mux, 0);
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
    CLOCK_DisableClock(kCLOCK_Lpi2c3);
    /* Set LPI2C_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, 0);
    /* Set Lpi2c clock source. */
    CLOCK_SetMux(kCLOCK_Lpi2cMux, 0);
    /* Disable CAN clock gate. */
    CLOCK_DisableClock(kCLOCK_Can1);
    CLOCK_DisableClock(kCLOCK_Can2);
    CLOCK_DisableClock(kCLOCK_Can3);
    CLOCK_DisableClock(kCLOCK_Can1S);
    CLOCK_DisableClock(kCLOCK_Can2S);
    CLOCK_DisableClock(kCLOCK_Can3S);
    /* Set CAN_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_CanDiv, 1);
    /* Set Can clock source. */
    CLOCK_SetMux(kCLOCK_CanMux, 2);
    /* Disable UART clock gate. */
    CLOCK_DisableClock(kCLOCK_Lpuart1);
    CLOCK_DisableClock(kCLOCK_Lpuart2);
    CLOCK_DisableClock(kCLOCK_Lpuart3);
    CLOCK_DisableClock(kCLOCK_Lpuart4);
    CLOCK_DisableClock(kCLOCK_Lpuart5);
    CLOCK_DisableClock(kCLOCK_Lpuart6);
    CLOCK_DisableClock(kCLOCK_Lpuart7);
    CLOCK_DisableClock(kCLOCK_Lpuart8);
    /* Set UART_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_UartDiv, 0);
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
    /* Disable Flexio2 clock gate. */
    CLOCK_DisableClock(kCLOCK_Flexio2);
    /* Set FLEXIO2_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Flexio2PreDiv, 1);
    /* Set FLEXIO2_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Flexio2Div, 7);
    /* Set Flexio2 clock source. */
    CLOCK_SetMux(kCLOCK_Flexio2Mux, 3);
    /* Set Pll3 sw clock source. */
    CLOCK_SetMux(kCLOCK_Pll3SwMux, 0);
    /* Init ARM PLL. */
    CLOCK_InitArmPll(&armPllConfig_BOARD_BootClockRUN);
    /* In SDK projects, SDRAM (configured by SEMC) will be initialized in either debug script or dcd.
     * With this macro SKIP_SYSCLK_INIT, system pll (selected to be SEMC source clock in SDK projects) will be left unchanged.
     * Note: If another clock source is selected for SEMC, user may want to avoid changing that clock as well.*/
    #ifndef SKIP_SYSCLK_INIT
    /* Init System PLL. */
    CLOCK_InitSysPll(&sysPllConfig_BOARD_BootClockRUN);
    /* Init System pfd0. */
    CLOCK_InitSysPfd(kCLOCK_Pfd0, 27);
    /* Init System pfd1. */
    CLOCK_InitSysPfd(kCLOCK_Pfd1, 16);
    /* Init System pfd2. */
    CLOCK_InitSysPfd(kCLOCK_Pfd2, 24);
    /* Init System pfd3. */
    CLOCK_InitSysPfd(kCLOCK_Pfd3, 16);
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
    /* DeInit Video PLL. */
    CLOCK_DeinitVideoPll();
    /* Bypass Video PLL. */
    CCM_ANALOG->PLL_VIDEO |= CCM_ANALOG_PLL_VIDEO_BYPASS_MASK;
    /* Set divider for Video PLL. */
    CCM_ANALOG->MISC2 = (CCM_ANALOG->MISC2 & (~CCM_ANALOG_MISC2_VIDEO_DIV_MASK)) | CCM_ANALOG_MISC2_VIDEO_DIV(0);
    /* Enable Video PLL output. */
    CCM_ANALOG->PLL_VIDEO |= CCM_ANALOG_PLL_VIDEO_ENABLE_MASK;
    /* DeInit Enet PLL. */
    CLOCK_DeinitEnetPll();
    /* Bypass Enet PLL. */
    CLOCK_SetPllBypass(CCM_ANALOG, kCLOCK_PllEnet, 1);
    /* Set Enet output divider. */
    CCM_ANALOG->PLL_ENET = (CCM_ANALOG->PLL_ENET & (~CCM_ANALOG_PLL_ENET_DIV_SELECT_MASK)) | CCM_ANALOG_PLL_ENET_DIV_SELECT(1);
    /* Enable Enet output. */
    CCM_ANALOG->PLL_ENET |= CCM_ANALOG_PLL_ENET_ENABLE_MASK;
    /* Set Enet2 output divider. */
    CCM_ANALOG->PLL_ENET = (CCM_ANALOG->PLL_ENET & (~CCM_ANALOG_PLL_ENET_ENET2_DIV_SELECT_MASK)) | CCM_ANALOG_PLL_ENET_ENET2_DIV_SELECT(0);
    /* Enable Enet2 output. */
    CCM_ANALOG->PLL_ENET |= CCM_ANALOG_PLL_ENET_ENET2_REF_EN_MASK;
    /* Enable Enet25M output. */
    CCM_ANALOG->PLL_ENET |= CCM_ANALOG_PLL_ENET_ENET_25M_REF_EN_MASK;
    /* DeInit Usb2 PLL. */
    CLOCK_DeinitUsb2Pll();
    /* Bypass Usb2 PLL. */
    CLOCK_SetPllBypass(CCM_ANALOG, kCLOCK_PllUsb2, 1);
    /* Enable Usb2 PLL output. */
    CCM_ANALOG->PLL_USB2 |= CCM_ANALOG_PLL_USB2_ENABLE_MASK;
    /* Set preperiph clock source. */
    CLOCK_SetMux(kCLOCK_PrePeriphMux, 3);
    /* Set periph clock source. */
    CLOCK_SetMux(kCLOCK_PeriphMux, 0);
    /* Set periph clock2 clock source. */
    CLOCK_SetMux(kCLOCK_PeriphClk2Mux, 0);
    /* Set per clock source. */
    CLOCK_SetMux(kCLOCK_PerclkMux, 0);
    /* Set lvds1 clock source. */
    CCM_ANALOG->MISC1 = (CCM_ANALOG->MISC1 & (~CCM_ANALOG_MISC1_LVDS1_CLK_SEL_MASK)) | CCM_ANALOG_MISC1_LVDS1_CLK_SEL(0);
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
    /* Set SAI2 MCLK3 clock source. */
    IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR, kIOMUXC_GPR_SAI2MClk3Sel, 0);
    /* Set SAI3 MCLK3 clock source. */
    IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR, kIOMUXC_GPR_SAI3MClk3Sel, 0);
    /* Set MQS configuration. */
    IOMUXC_MQSConfig(IOMUXC_GPR, kIOMUXC_MqsPwmOverSampleRate32, 0);
    /* Set ENET1 Tx clock source. */
    IOMUXC_EnableMode(IOMUXC_GPR, kIOMUXC_GPR_ENET1RefClkMode, false);
    /* Set ENET2 Tx clock source. */
    #if defined(FSL_IOMUXC_DRIVER_VERSION) && (FSL_IOMUXC_DRIVER_VERSION != (MAKE_VERSION(2, 0, 0)))
    IOMUXC_EnableMode(IOMUXC_GPR, kIOMUXC_GPR_ENET2RefClkMode, false);
    #else
    IOMUXC_EnableMode(IOMUXC_GPR, IOMUXC_GPR_GPR1_ENET2_CLK_SEL_MASK, false);
    #endif
    /* Set GPT1 High frequency reference clock source. */
    IOMUXC_GPR->GPR5 &= ~IOMUXC_GPR_GPR5_VREF_1M_CLK_GPT1_MASK;
    /* Set GPT2 High frequency reference clock source. */
    IOMUXC_GPR->GPR5 &= ~IOMUXC_GPR_GPR5_VREF_1M_CLK_GPT2_MASK;
    /* Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKRUN_CORE_CLOCK;

    CLOCK_EnableClock(kCLOCK_Iomuxc);
}

/* clockspeed.c
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2017 PJRC.COM, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
// Note setarmclock is a port from Teensyduino for the Teensy 4.x written by Paul Stroffgren,
// A brief explanation of F_CPU_ACTUAL vs F_CPU
//  https://forum.pjrc.com/threads/57236?p=212642&viewfull=1#post212642
volatile uint32_t F_CPU_ACTUAL = 396000000;
volatile uint32_t F_BUS_ACTUAL = 132000000;

// Define these to increase the voltage when attempting overclocking
// The frequency step is how quickly to increase voltage per frequency
// The datasheet says 1600 is the absolute maximum voltage.  The hardware
// can actually create up to 1575.  But 1300 is the recommended limit.
//  (earlier versions of the datasheet said 1300 was the absolute max)
#define OVERCLOCK_STEPSIZE  28000000
#define OVERCLOCK_MAX_VOLT  1575

#define DCDC_REG3                                               0x40080012
#define DCDC_REG0                                               0x40080000
#define DCDC_REG0_STS_DC_OK_L                           ((uint32_t)(1 << 31))
#define CCM_ANALOG_PLL_USB1_ENABLE_L            ((uint32_t)(1 << 13))
#define CCM_ANALOG_PLL_USB1_POWER_L             ((uint32_t)(1 << 12))
#define CCM_ANALOG_PLL_USB1_EN_USB_CLKS_L               ((uint32_t)(1 << 6))
#define CCM_ANALOG_PLL_USB1_LOCK_L              ((uint32_t)(1 << 31))
#define CCM_CCGR6_DCDC(n)                       ((uint32_t)(((n) & 0x03) << 6))
#define CCM_ANALOG_PLL_ARM_LOCK_L                       ((uint32_t)(1 << 31))
#define CCM_ANALOG_PLL_ARM_BYPASS_L             ((uint32_t)(1 << 16))
#define CCM_ANALOG_PLL_ARM_ENABLE_L             ((uint32_t)(1 << 13))
#define CCM_ANALOG_PLL_ARM_POWERDOWN_L          ((uint32_t)(1 << 12))
#define CCM_CDHIPR_ARM_PODF_BUSY_L              ((uint32_t)(1 << 16))
#define CCM_CDHIPR_AHB_PODF_BUSY_L              ((uint32_t)(1 << 1))
#define CCM_CDHIPR_PERIPH_CLK_SEL_BUSY_L                ((uint32_t)(1 << 5))
#define CCM_CBCDR_PERIPH_CLK_SEL_L              ((uint32_t)(1 << 25))
#define CCM_CCGR_OFF                            0
#define CCM_CCGR_ON_RUNONLY                     1
#define CCM_CCGR_ON                             3

/* Teensyduino Core Library - clockspeed.c
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2017 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// uint32_t set_arm_clock(uint32_t frequency);

// stuff needing wait handshake:
//  CCM_CACRR  ARM_PODF
//  CCM_CBCDR  PERIPH_CLK_SEL
//  CCM_CBCMR  PERIPH2_CLK_SEL
//  CCM_CBCDR  AHB_PODF
//  CCM_CBCDR  SEMC_PODF

uint32_t setarmclock(uint32_t frequency) {
    uint32_t cbcdr = CCM->CBCDR;     // pg 1021
    uint32_t cbcmr = CCM->CBCMR;     // pg 1023
    uint32_t dcdc = DCDC->REG3;

    // compute required voltage
    uint32_t voltage = 1150;     // default = 1.15V
    if (frequency > 528000000) {
        voltage = 1250;         // 1.25V
        #if defined(OVERCLOCK_STEPSIZE) && defined(OVERCLOCK_MAX_VOLT)
        if (frequency > 600000000) {
            voltage += ((frequency - 600000000) / OVERCLOCK_STEPSIZE) * 25;
            if (voltage > OVERCLOCK_MAX_VOLT) {
                voltage = OVERCLOCK_MAX_VOLT;
            }
        }
        #endif
    } else if (frequency <= 24000000) {
        voltage = 950;         // 0.95
    }

    // if voltage needs to increase, do it before switch clock speed
    CCM->CCGR6 |= CCM_CCGR6_DCDC(CCM_CCGR_ON);
    if ((dcdc & ((uint32_t)(0x1F << 0))) < ((uint32_t)(((voltage - 800) / 25) & 0x1F) << 0)) {
        dcdc &= ~((uint32_t)(0x1F << 0));
        dcdc |= ((uint32_t)(((voltage - 800) / 25) & 0x1F) << 0);
        DCDC->REG3 = dcdc;
        while (!(DCDC->REG0 & DCDC_REG0_STS_DC_OK_L)) {
            ;                                                   // wait voltage settling
        }
    }

    if (!(cbcdr & CCM_CBCDR_PERIPH_CLK_SEL_L)) {
        const uint32_t need1s = CCM_ANALOG_PLL_USB1_ENABLE_L | CCM_ANALOG_PLL_USB1_POWER_L |
            CCM_ANALOG_PLL_USB1_LOCK_L | CCM_ANALOG_PLL_USB1_EN_USB_CLKS_L;
        uint32_t sel, div;
        if ((CCM_ANALOG->PLL_USB1 & need1s) == need1s) {
            sel = 0;
            div = 3;             // divide down to 120 MHz, so IPG is ok even if IPG_PODF=0
        } else {
            sel = 1;
            div = 0;
        }
        if ((cbcdr & ((uint32_t)(0x07 << 27))) != CCM_CBCDR_PERIPH_CLK2_PODF(div)) {
            // PERIPH_CLK2 divider needs to be changed
            cbcdr &= ~((uint32_t)(0x07 << 27));
            cbcdr |= CCM_CBCDR_PERIPH_CLK2_PODF(div);
            CCM->CBCDR = cbcdr;
        }
        if ((cbcmr & ((uint32_t)(0x03 << 12))) != CCM_CBCMR_PERIPH_CLK2_SEL(sel)) {
            // PERIPH_CLK2 source select needs to be changed
            cbcmr &= ~((uint32_t)(0x03 << 12));
            cbcmr |= CCM_CBCMR_PERIPH_CLK2_SEL(sel);
            CCM->CBCMR = cbcmr;
            while (CCM->CDHIPR & ((uint32_t)(1 << 3))) {
                ;                                                  // wait
            }
        }
        // switch over to PERIPH_CLK2
        cbcdr |= ((uint32_t)(1 << 25));
        CCM->CBCDR = cbcdr;
        while (CCM->CDHIPR & ((uint32_t)(1 << 5))) {
            ;                                              // wait
        }
    }

    // TODO: check if PLL2 running, can 352, 396 or 528 can work? (no need for ARM PLL)

    // DIV_SELECT: 54-108 = official range 648 to 1296 in 12 MHz steps
    uint32_t div_arm = 1;
    uint32_t div_ahb = 1;
    while (frequency * div_arm * div_ahb < 648000000) {
        if (div_arm < 8) {
            div_arm = div_arm + 1;
        } else {
            if (div_ahb < 5) {
                div_ahb = div_ahb + 1;
                div_arm = 1;
            } else {
                break;
            }
        }
    }
    uint32_t mult = (frequency * div_arm * div_ahb + 6000000) / 12000000;
    if (mult > 108) {
        mult = 108;
    }
    if (mult < 54) {
        mult = 54;
    }

    frequency = mult * 12000000 / div_arm / div_ahb;

    const uint32_t arm_pll_mask = CCM_ANALOG_PLL_ARM_LOCK_L | CCM_ANALOG_PLL_ARM_BYPASS_L |
        CCM_ANALOG_PLL_ARM_ENABLE_L | CCM_ANALOG_PLL_ARM_POWERDOWN_L |
        CCM_ANALOG_PLL_ARM_DIV_SELECT_MASK;
    if ((CCM_ANALOG->PLL_ARM & arm_pll_mask) != (CCM_ANALOG_PLL_ARM_LOCK_L
                                                 | CCM_ANALOG_PLL_ARM_ENABLE_L | CCM_ANALOG_PLL_ARM_DIV_SELECT(mult))) {
        // printf("ARM PLL needs reconfigure\n");
        CCM_ANALOG->PLL_ARM = CCM_ANALOG_PLL_ARM_POWERDOWN_L;
        // TODO: delay needed?
        CCM_ANALOG->PLL_ARM = CCM_ANALOG_PLL_ARM_ENABLE_L
            | CCM_ANALOG_PLL_ARM_DIV_SELECT(mult);
        while (!(CCM_ANALOG->PLL_ARM & CCM_ANALOG_PLL_ARM_LOCK_L)) {
            ;                                                                // wait for lock
        }
    }

    if ((CCM->CACRR & ((uint32_t)(0x07 << 0))) != (div_arm - 1)) {
        CCM->CACRR = CCM_CACRR_ARM_PODF(div_arm - 1);
        while (CCM->CDHIPR & CCM_CDHIPR_ARM_PODF_BUSY_L) {
            ;                                                      // wait
        }
    }

    if ((cbcdr & ((uint32_t)(0x07 << 10))) != CCM_CBCDR_AHB_PODF(div_ahb - 1)) {
        cbcdr &= ~((uint32_t)(0x07 << 10));
        cbcdr |= CCM_CBCDR_AHB_PODF(div_ahb - 1);
        CCM->CBCDR = cbcdr;
        while (CCM->CDHIPR & CCM_CDHIPR_AHB_PODF_BUSY_L) {
            ;                                                     // wait
        }
    }

    uint32_t div_ipg = (frequency + 149999999) / 150000000;
    if (div_ipg > 4) {
        div_ipg = 4;
    }
    if ((cbcdr & ((uint32_t)(0x03 << 8))) != (CCM_CBCDR_IPG_PODF(div_ipg - 1))) {
        cbcdr &= ~((uint32_t)(0x03 << 8));
        cbcdr |= CCM_CBCDR_IPG_PODF(div_ipg - 1);
        // TODO: how to safely change IPG_PODF ??
        CCM->CBCDR = cbcdr;
    }

    // cbcdr &= ~CCM_CBCDR_PERIPH_CLK_SEL;
    // CCM_CBCDR = cbcdr;  // why does this not work at 24 MHz?
    CCM->CBCDR &= ~((uint32_t)(1 << 25));
    while (CCM->CDHIPR & CCM_CDHIPR_PERIPH_CLK_SEL_BUSY_L) {
        ;                                                        // wait

    }
    F_CPU_ACTUAL = frequency;
    F_BUS_ACTUAL = frequency / div_ipg;
    // scale_cpu_cycles_to_microseconds = 0xFFFFFFFFu / (uint32_t)(frequency / 1000000u);

    // if voltage needs to decrease, do it after switch clock speed
    if ((dcdc & ((uint32_t)(0x1F << 0))) > ((uint32_t)(((voltage - 800) / 25) & 0x1F) << 0)) {
        dcdc &= ~((uint32_t)(0x1F << 0));
        dcdc |= ((uint32_t)(0x1F << 0));
        DCDC->REG3 = dcdc;
        while (!(DCDC->REG0 & DCDC_REG0_STS_DC_OK_L)) {
            ;                                                   // wait voltage settling
        }
    }

    return frequency;
}
