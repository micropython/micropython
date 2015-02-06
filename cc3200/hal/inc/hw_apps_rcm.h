//*****************************************************************************
//
//  Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#ifndef __HW_APPS_RCM_H__
#define __HW_APPS_RCM_H__

//*****************************************************************************
//
// The following are defines for the APPS_RCM register offsets.
//
//*****************************************************************************
#define APPS_RCM_O_CAMERA_CLK_GEN \
                                0x00000000

#define APPS_RCM_O_CAMERA_CLK_GATING \
                                0x00000004

#define APPS_RCM_O_CAMERA_SOFT_RESET \
                                0x00000008

#define APPS_RCM_O_MCASP_CLK_GATING \
                                0x00000014

#define APPS_RCM_O_MCASP_SOFT_RESET \
                                0x00000018

#define APPS_RCM_O_MMCHS_CLK_GEN \
                                0x00000020

#define APPS_RCM_O_MMCHS_CLK_GATING \
                                0x00000024

#define APPS_RCM_O_MMCHS_SOFT_RESET \
                                0x00000028

#define APPS_RCM_O_MCSPI_A1_CLK_GEN \
                                0x0000002C

#define APPS_RCM_O_MCSPI_A1_CLK_GATING \
                                0x00000030

#define APPS_RCM_O_MCSPI_A1_SOFT_RESET \
                                0x00000034

#define APPS_RCM_O_MCSPI_A2_CLK_GEN \
                                0x00000038

#define APPS_RCM_O_MCSPI_A2_CLK_GATING \
                                0x00000040

#define APPS_RCM_O_MCSPI_A2_SOFT_RESET \
                                0x00000044

#define APPS_RCM_O_UDMA_A_CLK_GATING \
                                0x00000048

#define APPS_RCM_O_UDMA_A_SOFT_RESET \
                                0x0000004C

#define APPS_RCM_O_GPIO_A_CLK_GATING \
                                0x00000050

#define APPS_RCM_O_GPIO_A_SOFT_RESET \
                                0x00000054

#define APPS_RCM_O_GPIO_B_CLK_GATING \
                                0x00000058

#define APPS_RCM_O_GPIO_B_SOFT_RESET \
                                0x0000005C

#define APPS_RCM_O_GPIO_C_CLK_GATING \
                                0x00000060

#define APPS_RCM_O_GPIO_C_SOFT_RESET \
                                0x00000064

#define APPS_RCM_O_GPIO_D_CLK_GATING \
                                0x00000068

#define APPS_RCM_O_GPIO_D_SOFT_RESET \
                                0x0000006C

#define APPS_RCM_O_GPIO_E_CLK_GATING \
                                0x00000070

#define APPS_RCM_O_GPIO_E_SOFT_RESET \
                                0x00000074

#define APPS_RCM_O_WDOG_A_CLK_GATING \
                                0x00000078

#define APPS_RCM_O_WDOG_A_SOFT_RESET \
                                0x0000007C

#define APPS_RCM_O_UART_A0_CLK_GATING \
                                0x00000080

#define APPS_RCM_O_UART_A0_SOFT_RESET \
                                0x00000084

#define APPS_RCM_O_UART_A1_CLK_GATING \
                                0x00000088

#define APPS_RCM_O_UART_A1_SOFT_RESET \
                                0x0000008C

#define APPS_RCM_O_GPT_A0_CLK_GATING \
                                0x00000090

#define APPS_RCM_O_GPT_A0_SOFT_RESET \
                                0x00000094

#define APPS_RCM_O_GPT_A1_CLK_GATING \
                                0x00000098

#define APPS_RCM_O_GPT_A1_SOFT_RESET \
                                0x0000009C

#define APPS_RCM_O_GPT_A2_CLK_GATING \
                                0x000000A0

#define APPS_RCM_O_GPT_A2_SOFT_RESET \
                                0x000000A4

#define APPS_RCM_O_GPT_A3_CLK_GATING \
                                0x000000A8

#define APPS_RCM_O_GPT_A3_SOFT_RESET \
                                0x000000AC

#define APPS_RCM_O_MCASP_FRAC_CLK_CONFIG0 \
                                0x000000B0

#define APPS_RCM_O_MCASP_FRAC_CLK_CONFIG1 \
                                0x000000B4

#define APPS_RCM_O_CRYPTO_CLK_GATING \
                                0x000000B8

#define APPS_RCM_O_CRYPTO_SOFT_RESET \
                                0x000000BC

#define APPS_RCM_O_MCSPI_S0_CLK_GATING \
                                0x000000C8

#define APPS_RCM_O_MCSPI_S0_SOFT_RESET \
                                0x000000CC

#define APPS_RCM_O_MCSPI_S0_CLKDIV_CFG \
                                0x000000D0

#define APPS_RCM_O_I2C_CLK_GATING \
                                0x000000D8

#define APPS_RCM_O_I2C_SOFT_RESET \
                                0x000000DC

#define APPS_RCM_O_APPS_LPDS_REQ \
                                0x000000E4

#define APPS_RCM_O_APPS_TURBO_REQ \
                                0x000000EC

#define APPS_RCM_O_APPS_DSLP_WAKE_CONFIG \
                                0x00000108

#define APPS_RCM_O_APPS_DSLP_WAKE_TIMER_CFG \
                                0x0000010C

#define APPS_RCM_O_APPS_RCM_SLP_WAKE_ENABLE \
                                0x00000110

#define APPS_RCM_O_APPS_SLP_WAKETIMER_CFG \
                                0x00000114

#define APPS_RCM_O_APPS_TO_NWP_WAKE_REQUEST \
                                0x00000118

#define APPS_RCM_O_APPS_RCM_INTERRUPT_STATUS \
                                0x00000120

#define APPS_RCM_O_APPS_RCM_INTERRUPT_ENABLE \
                                0x00000124





//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_CAMERA_CLK_GEN register.
//
//******************************************************************************
#define APPS_RCM_CAMERA_CLK_GEN_CAMERA_PLLCKDIV_OFF_TIME_M \
                                0x00000700  // Configuration of OFF-TIME for
                                            // dividing PLL clk (240 MHz) in
                                            // generation of Camera func-clk :
                                            // "000" - 1 "001" - 2 "010" - 3
                                            // "011" - 4 "100" - 5 "101" - 6
                                            // "110" - 7 "111" - 8

#define APPS_RCM_CAMERA_CLK_GEN_CAMERA_PLLCKDIV_OFF_TIME_S 8
#define APPS_RCM_CAMERA_CLK_GEN_NU1_M \
                                0x000000F8

#define APPS_RCM_CAMERA_CLK_GEN_NU1_S 3
#define APPS_RCM_CAMERA_CLK_GEN_CAMERA_PLLCKDIV_ON_TIME_M \
                                0x00000007  // Configuration of ON-TIME for
                                            // dividing PLL clk (240 MHz) in
                                            // generation of Camera func-clk :
                                            // "000" - 1 "001" - 2 "010" - 3
                                            // "011" - 4 "100" - 5 "101" - 6
                                            // "110" - 7 "111" - 8

#define APPS_RCM_CAMERA_CLK_GEN_CAMERA_PLLCKDIV_ON_TIME_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_CAMERA_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_CAMERA_CLK_GATING_NU1_M \
                                0x00FE0000

#define APPS_RCM_CAMERA_CLK_GATING_NU1_S 17
#define APPS_RCM_CAMERA_CLK_GATING_CAMERA_DSLP_CLK_ENABLE \
                                0x00010000  // 0 - Disable camera clk during
                                            // deep-sleep mode

#define APPS_RCM_CAMERA_CLK_GATING_NU2_M \
                                0x0000FE00

#define APPS_RCM_CAMERA_CLK_GATING_NU2_S 9
#define APPS_RCM_CAMERA_CLK_GATING_CAMERA_SLP_CLK_ENABLE \
                                0x00000100  // 1- Enable camera clk during
                                            // sleep mode ; 0- Disable camera
                                            // clk during sleep mode

#define APPS_RCM_CAMERA_CLK_GATING_NU3_M \
                                0x000000FE

#define APPS_RCM_CAMERA_CLK_GATING_NU3_S 1
#define APPS_RCM_CAMERA_CLK_GATING_CAMERA_RUN_CLK_ENABLE \
                                0x00000001  // 1- Enable camera clk during run
                                            // mode ; 0- Disable camera clk
                                            // during run mode

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_CAMERA_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_CAMERA_SOFT_RESET_CAMERA_ENABLED_STATUS \
                                0x00000002  // 1 - Camera clocks/resets are
                                            // enabled ; 0 - Camera
                                            // clocks/resets are disabled

#define APPS_RCM_CAMERA_SOFT_RESET_CAMERA_SOFT_RESET \
                                0x00000001  // 1 - Assert reset for Camera-core
                                            // ; 0 - De-assert reset for
                                            // Camera-core

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_MCASP_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_MCASP_CLK_GATING_NU1_M \
                                0x00FE0000

#define APPS_RCM_MCASP_CLK_GATING_NU1_S 17
#define APPS_RCM_MCASP_CLK_GATING_MCASP_DSLP_CLK_ENABLE \
                                0x00010000  // 0 - Disable MCASP clk during
                                            // deep-sleep mode

#define APPS_RCM_MCASP_CLK_GATING_NU2_M \
                                0x0000FE00

#define APPS_RCM_MCASP_CLK_GATING_NU2_S 9
#define APPS_RCM_MCASP_CLK_GATING_MCASP_SLP_CLK_ENABLE \
                                0x00000100  // 1- Enable MCASP clk during sleep
                                            // mode ; 0- Disable MCASP clk
                                            // during sleep mode

#define APPS_RCM_MCASP_CLK_GATING_NU3_M \
                                0x000000FE

#define APPS_RCM_MCASP_CLK_GATING_NU3_S 1
#define APPS_RCM_MCASP_CLK_GATING_MCASP_RUN_CLK_ENABLE \
                                0x00000001  // 1- Enable MCASP clk during run
                                            // mode ; 0- Disable MCASP clk
                                            // during run mode

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_MCASP_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_MCASP_SOFT_RESET_MCASP_ENABLED_STATUS \
                                0x00000002  // 1 - MCASP Clocks/resets are
                                            // enabled ; 0 - MCASP Clocks/resets
                                            // are disabled

#define APPS_RCM_MCASP_SOFT_RESET_MCASP_SOFT_RESET \
                                0x00000001  // 1 - Assert reset for MCASP-core
                                            // ; 0 - De-assert reset for
                                            // MCASP-core

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_MMCHS_CLK_GEN register.
//
//******************************************************************************
#define APPS_RCM_MMCHS_CLK_GEN_MMCHS_PLLCKDIV_OFF_TIME_M \
                                0x00000700  // Configuration of OFF-TIME for
                                            // dividing PLL clk (240 MHz) in
                                            // generation of MMCHS func-clk :
                                            // "000" - 1 "001" - 2 "010" - 3
                                            // "011" - 4 "100" - 5 "101" - 6
                                            // "110" - 7 "111" - 8

#define APPS_RCM_MMCHS_CLK_GEN_MMCHS_PLLCKDIV_OFF_TIME_S 8
#define APPS_RCM_MMCHS_CLK_GEN_NU1_M \
                                0x000000F8

#define APPS_RCM_MMCHS_CLK_GEN_NU1_S 3
#define APPS_RCM_MMCHS_CLK_GEN_MMCHS_PLLCKDIV_ON_TIME_M \
                                0x00000007  // Configuration of ON-TIME for
                                            // dividing PLL clk (240 MHz) in
                                            // generation of MMCHS func-clk :
                                            // "000" - 1 "001" - 2 "010" - 3
                                            // "011" - 4 "100" - 5 "101" - 6
                                            // "110" - 7 "111" - 8

#define APPS_RCM_MMCHS_CLK_GEN_MMCHS_PLLCKDIV_ON_TIME_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_MMCHS_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_MMCHS_CLK_GATING_NU1_M \
                                0x00FE0000

#define APPS_RCM_MMCHS_CLK_GATING_NU1_S 17
#define APPS_RCM_MMCHS_CLK_GATING_MMCHS_DSLP_CLK_ENABLE \
                                0x00010000  // 0 - Disable MMCHS clk during
                                            // deep-sleep mode

#define APPS_RCM_MMCHS_CLK_GATING_NU2_M \
                                0x0000FE00

#define APPS_RCM_MMCHS_CLK_GATING_NU2_S 9
#define APPS_RCM_MMCHS_CLK_GATING_MMCHS_SLP_CLK_ENABLE \
                                0x00000100  // 1- Enable MMCHS clk during sleep
                                            // mode ; 0- Disable MMCHS clk
                                            // during sleep mode

#define APPS_RCM_MMCHS_CLK_GATING_NU3_M \
                                0x000000FE

#define APPS_RCM_MMCHS_CLK_GATING_NU3_S 1
#define APPS_RCM_MMCHS_CLK_GATING_MMCHS_RUN_CLK_ENABLE \
                                0x00000001  // 1- Enable MMCHS clk during run
                                            // mode ; 0- Disable MMCHS clk
                                            // during run mode

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_MMCHS_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_MMCHS_SOFT_RESET_MMCHS_ENABLED_STATUS \
                                0x00000002  // 1 - MMCHS Clocks/resets are
                                            // enabled ; 0 - MMCHS Clocks/resets
                                            // are disabled

#define APPS_RCM_MMCHS_SOFT_RESET_MMCHS_SOFT_RESET \
                                0x00000001  // 1 - Assert reset for MMCHS-core
                                            // ; 0 - De-assert reset for
                                            // MMCHS-core

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_MCSPI_A1_CLK_GEN register.
//
//******************************************************************************
#define APPS_RCM_MCSPI_A1_CLK_GEN_MCSPI_A1_BAUD_CLK_SEL \
                                0x00010000  // 0 - XTAL clk is used as baud clk
                                            // for MCSPI_A1 ; 1 - PLL divclk is
                                            // used as baud clk for MCSPI_A1.

#define APPS_RCM_MCSPI_A1_CLK_GEN_NU1_M \
                                0x0000F800

#define APPS_RCM_MCSPI_A1_CLK_GEN_NU1_S 11
#define APPS_RCM_MCSPI_A1_CLK_GEN_MCSPI_A1_PLLCLKDIV_OFF_TIME_M \
                                0x00000700  // Configuration of OFF-TIME for
                                            // dividing PLL clk (240 MHz) in
                                            // generation of MCSPI_A1 func-clk :
                                            // "000" - 1 "001" - 2 "010" - 3
                                            // "011" - 4 "100" - 5 "101" - 6
                                            // "110" - 7 "111" - 8

#define APPS_RCM_MCSPI_A1_CLK_GEN_MCSPI_A1_PLLCLKDIV_OFF_TIME_S 8
#define APPS_RCM_MCSPI_A1_CLK_GEN_NU2_M \
                                0x000000F8

#define APPS_RCM_MCSPI_A1_CLK_GEN_NU2_S 3
#define APPS_RCM_MCSPI_A1_CLK_GEN_MCSPI_A1_PLLCLKDIV_ON_TIME_M \
                                0x00000007  // Configuration of ON-TIME for
                                            // dividing PLL clk (240 MHz) in
                                            // generation of MCSPI_A1 func-clk :
                                            // "000" - 1 "001" - 2 "010" - 3
                                            // "011" - 4 "100" - 5 "101" - 6
                                            // "110" - 7 "111" - 8

#define APPS_RCM_MCSPI_A1_CLK_GEN_MCSPI_A1_PLLCLKDIV_ON_TIME_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_MCSPI_A1_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_MCSPI_A1_CLK_GATING_NU1_M \
                                0x00FE0000

#define APPS_RCM_MCSPI_A1_CLK_GATING_NU1_S 17
#define APPS_RCM_MCSPI_A1_CLK_GATING_MCSPI_A1_DSLP_CLK_ENABLE \
                                0x00010000  // 0 - Disable MCSPI_A1 clk during
                                            // deep-sleep mode

#define APPS_RCM_MCSPI_A1_CLK_GATING_NU2_M \
                                0x0000FE00

#define APPS_RCM_MCSPI_A1_CLK_GATING_NU2_S 9
#define APPS_RCM_MCSPI_A1_CLK_GATING_MCSPI_A1_SLP_CLK_ENABLE \
                                0x00000100  // 1- Enable MCSPI_A1 clk during
                                            // sleep mode ; 0- Disable MCSPI_A1
                                            // clk during sleep mode

#define APPS_RCM_MCSPI_A1_CLK_GATING_NU3_M \
                                0x000000FE

#define APPS_RCM_MCSPI_A1_CLK_GATING_NU3_S 1
#define APPS_RCM_MCSPI_A1_CLK_GATING_MCSPI_A1_RUN_CLK_ENABLE \
                                0x00000001  // 1- Enable MCSPI_A1 clk during
                                            // run mode ; 0- Disable MCSPI_A1
                                            // clk during run mode

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_MCSPI_A1_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_MCSPI_A1_SOFT_RESET_MCSPI_A1_ENABLED_STATUS \
                                0x00000002  // 1 - MCSPI_A1 Clocks/Resets are
                                            // enabled ; 0 - MCSPI_A1
                                            // Clocks/Resets are disabled

#define APPS_RCM_MCSPI_A1_SOFT_RESET_MCSPI_A1_SOFT_RESET \
                                0x00000001  // 1 - Assert reset for
                                            // MCSPI_A1-core ; 0 - De-assert
                                            // reset for MCSPI_A1-core

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_MCSPI_A2_CLK_GEN register.
//
//******************************************************************************
#define APPS_RCM_MCSPI_A2_CLK_GEN_MCSPI_A2_BAUD_CLK_SEL \
                                0x00010000  // 0 - XTAL clk is used as baud-clk
                                            // for MCSPI_A2 ; 1 - PLL divclk is
                                            // used as baud-clk for MCSPI_A2

#define APPS_RCM_MCSPI_A2_CLK_GEN_NU1_M \
                                0x0000F800

#define APPS_RCM_MCSPI_A2_CLK_GEN_NU1_S 11
#define APPS_RCM_MCSPI_A2_CLK_GEN_MCSPI_A2_PLLCKDIV_OFF_TIME_M \
                                0x00000700  // Configuration of OFF-TIME for
                                            // dividing PLL clk (240 MHz) in
                                            // generation of MCSPI_A2 func-clk :
                                            // "000" - 1 "001" - 2 "010" - 3
                                            // "011" - 4 "100" - 5 "101" - 6
                                            // "110" - 7 "111" - 8

#define APPS_RCM_MCSPI_A2_CLK_GEN_MCSPI_A2_PLLCKDIV_OFF_TIME_S 8
#define APPS_RCM_MCSPI_A2_CLK_GEN_NU2_M \
                                0x000000F8

#define APPS_RCM_MCSPI_A2_CLK_GEN_NU2_S 3
#define APPS_RCM_MCSPI_A2_CLK_GEN_MCSPI_A2_PLLCKDIV_ON_TIME_M \
                                0x00000007  // Configuration of OFF-TIME for
                                            // dividing PLL clk (240 MHz) in
                                            // generation of MCSPI_A2 func-clk :
                                            // "000" - 1 "001" - 2 "010" - 3
                                            // "011" - 4 "100" - 5 "101" - 6
                                            // "110" - 7 "111" - 8

#define APPS_RCM_MCSPI_A2_CLK_GEN_MCSPI_A2_PLLCKDIV_ON_TIME_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_MCSPI_A2_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_MCSPI_A2_CLK_GATING_NU1_M \
                                0x00FE0000

#define APPS_RCM_MCSPI_A2_CLK_GATING_NU1_S 17
#define APPS_RCM_MCSPI_A2_CLK_GATING_MCSPI_A2_DSLP_CLK_ENABLE \
                                0x00010000  // 0 - Disable MCSPI_A2 clk during
                                            // deep-sleep mode

#define APPS_RCM_MCSPI_A2_CLK_GATING_NU2_M \
                                0x0000FE00

#define APPS_RCM_MCSPI_A2_CLK_GATING_NU2_S 9
#define APPS_RCM_MCSPI_A2_CLK_GATING_MCSPI_A2_SLP_CLK_ENABLE \
                                0x00000100  // 1- Enable MCSPI_A2 clk during
                                            // sleep mode ; 0- Disable MCSPI_A2
                                            // clk during sleep mode

#define APPS_RCM_MCSPI_A2_CLK_GATING_NU3_M \
                                0x000000FE

#define APPS_RCM_MCSPI_A2_CLK_GATING_NU3_S 1
#define APPS_RCM_MCSPI_A2_CLK_GATING_MCSPI_A2_RUN_CLK_ENABLE \
                                0x00000001  // 1- Enable MCSPI_A2 clk during
                                            // run mode ; 0- Disable MCSPI_A2
                                            // clk during run mode

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_MCSPI_A2_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_MCSPI_A2_SOFT_RESET_MCSPI_A2_ENABLED_STATUS \
                                0x00000002  // 1 - MCSPI_A2 Clocks/Resets are
                                            // enabled ; 0 - MCSPI_A2
                                            // Clocks/Resets are disabled

#define APPS_RCM_MCSPI_A2_SOFT_RESET_MCSPI_A2_SOFT_RESET \
                                0x00000001  // 1 - Assert reset for
                                            // MCSPI_A2-core ; 0 - De-assert
                                            // reset for MCSPI_A2-core

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_UDMA_A_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_UDMA_A_CLK_GATING_UDMA_A_DSLP_CLK_ENABLE \
                                0x00010000  // 1 - Enable UDMA_A clk during
                                            // deep-sleep mode 0 - Disable
                                            // UDMA_A clk during deep-sleep mode
                                            // ;

#define APPS_RCM_UDMA_A_CLK_GATING_NU1_M \
                                0x0000FE00

#define APPS_RCM_UDMA_A_CLK_GATING_NU1_S 9
#define APPS_RCM_UDMA_A_CLK_GATING_UDMA_A_SLP_CLK_ENABLE \
                                0x00000100  // 1 - Enable UDMA_A clk during
                                            // sleep mode 0 - Disable UDMA_A clk
                                            // during sleep mode ;

#define APPS_RCM_UDMA_A_CLK_GATING_NU2_M \
                                0x000000FE

#define APPS_RCM_UDMA_A_CLK_GATING_NU2_S 1
#define APPS_RCM_UDMA_A_CLK_GATING_UDMA_A_RUN_CLK_ENABLE \
                                0x00000001  // 1 - Enable UDMA_A clk during run
                                            // mode 0 - Disable UDMA_A clk
                                            // during run mode ;

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_UDMA_A_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_UDMA_A_SOFT_RESET_UDMA_A_ENABLED_STATUS \
                                0x00000002  // 1 - UDMA_A Clocks/Resets are
                                            // enabled ; 0 - UDMA_A
                                            // Clocks/Resets are disabled

#define APPS_RCM_UDMA_A_SOFT_RESET_UDMA_A_SOFT_RESET \
                                0x00000001  // 1 - Assert reset for DMA_A ; 0 -
                                            // De-assert reset for DMA_A

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPIO_A_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_GPIO_A_CLK_GATING_GPIO_A_DSLP_CLK_ENABLE \
                                0x00010000  // 1 - Enable GPIO_A clk during
                                            // deep-sleep mode 0 - Disable
                                            // GPIO_A clk during deep-sleep mode
                                            // ;

#define APPS_RCM_GPIO_A_CLK_GATING_NU1_M \
                                0x0000FE00

#define APPS_RCM_GPIO_A_CLK_GATING_NU1_S 9
#define APPS_RCM_GPIO_A_CLK_GATING_GPIO_A_SLP_CLK_ENABLE \
                                0x00000100  // 1 - Enable GPIO_A clk during
                                            // sleep mode 0 - Disable GPIO_A clk
                                            // during sleep mode ;

#define APPS_RCM_GPIO_A_CLK_GATING_NU2_M \
                                0x000000FE

#define APPS_RCM_GPIO_A_CLK_GATING_NU2_S 1
#define APPS_RCM_GPIO_A_CLK_GATING_GPIO_A_RUN_CLK_ENABLE \
                                0x00000001  // 1 - Enable GPIO_A clk during run
                                            // mode 0 - Disable GPIO_A clk
                                            // during run mode ;

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPIO_A_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_GPIO_A_SOFT_RESET_GPIO_A_ENABLED_STATUS \
                                0x00000002  // 1 - GPIO_A Clocks/Resets are
                                            // enabled ; 0 - GPIO_A
                                            // Clocks/Resets are disabled

#define APPS_RCM_GPIO_A_SOFT_RESET_GPIO_A_SOFT_RESET \
                                0x00000001  // 1 - Assert reset for GPIO_A ; 0
                                            // - De-assert reset for GPIO_A

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPIO_B_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_GPIO_B_CLK_GATING_GPIO_B_DSLP_CLK_ENABLE \
                                0x00010000  // 1 - Enable GPIO_B clk during
                                            // deep-sleep mode 0 - Disable
                                            // GPIO_B clk during deep-sleep mode
                                            // ;

#define APPS_RCM_GPIO_B_CLK_GATING_NU1_M \
                                0x0000FE00

#define APPS_RCM_GPIO_B_CLK_GATING_NU1_S 9
#define APPS_RCM_GPIO_B_CLK_GATING_GPIO_B_SLP_CLK_ENABLE \
                                0x00000100  // 1 - Enable GPIO_B clk during
                                            // sleep mode 0 - Disable GPIO_B clk
                                            // during sleep mode ;

#define APPS_RCM_GPIO_B_CLK_GATING_NU2_M \
                                0x000000FE

#define APPS_RCM_GPIO_B_CLK_GATING_NU2_S 1
#define APPS_RCM_GPIO_B_CLK_GATING_GPIO_B_RUN_CLK_ENABLE \
                                0x00000001  // 1 - Enable GPIO_B clk during run
                                            // mode 0 - Disable GPIO_B clk
                                            // during run mode ;

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPIO_B_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_GPIO_B_SOFT_RESET_GPIO_B_ENABLED_STATUS \
                                0x00000002  // 1 - GPIO_B Clocks/Resets are
                                            // enabled ; 0 - GPIO_B
                                            // Clocks/Resets are disabled

#define APPS_RCM_GPIO_B_SOFT_RESET_GPIO_B_SOFT_RESET \
                                0x00000001  // 1 - Assert reset for GPIO_B ; 0
                                            // - De-assert reset for GPIO_B

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPIO_C_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_GPIO_C_CLK_GATING_GPIO_C_DSLP_CLK_ENABLE \
                                0x00010000  // 1 - Enable GPIO_C clk during
                                            // deep-sleep mode 0 - Disable
                                            // GPIO_C clk during deep-sleep mode
                                            // ;

#define APPS_RCM_GPIO_C_CLK_GATING_NU1_M \
                                0x0000FE00

#define APPS_RCM_GPIO_C_CLK_GATING_NU1_S 9
#define APPS_RCM_GPIO_C_CLK_GATING_GPIO_C_SLP_CLK_ENABLE \
                                0x00000100  // 1 - Enable GPIO_C clk during
                                            // sleep mode 0 - Disable GPIO_C clk
                                            // during sleep mode ;

#define APPS_RCM_GPIO_C_CLK_GATING_NU2_M \
                                0x000000FE

#define APPS_RCM_GPIO_C_CLK_GATING_NU2_S 1
#define APPS_RCM_GPIO_C_CLK_GATING_GPIO_C_RUN_CLK_ENABLE \
                                0x00000001  // 1 - Enable GPIO_C clk during run
                                            // mode 0 - Disable GPIO_C clk
                                            // during run mode ;

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPIO_C_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_GPIO_C_SOFT_RESET_GPIO_C_ENABLED_STATUS \
                                0x00000002  // 1 - GPIO_C Clocks/Resets are
                                            // enabled ; 0 - GPIO_C
                                            // Clocks/Resets are disabled

#define APPS_RCM_GPIO_C_SOFT_RESET_GPIO_C_SOFT_RESET \
                                0x00000001  // 1 - Assert reset for GPIO_C ; 0
                                            // - De-assert reset for GPIO_C

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPIO_D_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_GPIO_D_CLK_GATING_GPIO_D_DSLP_CLK_ENABLE \
                                0x00010000  // 1 - Enable GPIO_D clk during
                                            // deep-sleep mode 0 - Disable
                                            // GPIO_D clk during deep-sleep mode
                                            // ;

#define APPS_RCM_GPIO_D_CLK_GATING_NU1_M \
                                0x0000FE00

#define APPS_RCM_GPIO_D_CLK_GATING_NU1_S 9
#define APPS_RCM_GPIO_D_CLK_GATING_GPIO_D_SLP_CLK_ENABLE \
                                0x00000100  // 1 - Enable GPIO_D clk during
                                            // sleep mode 0 - Disable GPIO_D clk
                                            // during sleep mode ;

#define APPS_RCM_GPIO_D_CLK_GATING_NU2_M \
                                0x000000FE

#define APPS_RCM_GPIO_D_CLK_GATING_NU2_S 1
#define APPS_RCM_GPIO_D_CLK_GATING_GPIO_D_RUN_CLK_ENABLE \
                                0x00000001  // 1 - Enable GPIO_D clk during run
                                            // mode 0 - Disable GPIO_D clk
                                            // during run mode ;

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPIO_D_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_GPIO_D_SOFT_RESET_GPIO_D_ENABLED_STATUS \
                                0x00000002  // 1 - GPIO_D Clocks/Resets are
                                            // enabled ; 0 - GPIO_D
                                            // Clocks/Resets are disabled

#define APPS_RCM_GPIO_D_SOFT_RESET_GPIO_D_SOFT_RESET \
                                0x00000001  // 1 - Assert reset for GPIO_D ; 0
                                            // - De-assert reset for GPIO_D

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPIO_E_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_GPIO_E_CLK_GATING_GPIO_E_DSLP_CLK_ENABLE \
                                0x00010000  // 1 - Enable GPIO_E clk during
                                            // deep-sleep mode 0 - Disable
                                            // GPIO_E clk during deep-sleep mode
                                            // ;

#define APPS_RCM_GPIO_E_CLK_GATING_NU1_M \
                                0x0000FE00

#define APPS_RCM_GPIO_E_CLK_GATING_NU1_S 9
#define APPS_RCM_GPIO_E_CLK_GATING_GPIO_E_SLP_CLK_ENABLE \
                                0x00000100  // 1 - Enable GPIO_E clk during
                                            // sleep mode 0 - Disable GPIO_E clk
                                            // during sleep mode ;

#define APPS_RCM_GPIO_E_CLK_GATING_NU2_M \
                                0x000000FE

#define APPS_RCM_GPIO_E_CLK_GATING_NU2_S 1
#define APPS_RCM_GPIO_E_CLK_GATING_GPIO_E_RUN_CLK_ENABLE \
                                0x00000001  // 1 - Enable GPIO_E clk during run
                                            // mode 0 - Disable GPIO_E clk
                                            // during run mode ;

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPIO_E_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_GPIO_E_SOFT_RESET_GPIO_E_ENABLED_STATUS \
                                0x00000002  // 1 - GPIO_E Clocks/Resets are
                                            // enabled ; 0 - GPIO_E
                                            // Clocks/Resets are disabled

#define APPS_RCM_GPIO_E_SOFT_RESET_GPIO_E_SOFT_RESET \
                                0x00000001  // 1 - Assert reset for GPIO_E ; 0
                                            // - De-assert reset for GPIO_E

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_WDOG_A_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_WDOG_A_CLK_GATING_WDOG_A_BAUD_CLK_SEL_M \
                                0x03000000  // "00" - Sysclk ; "01" - REF_CLK
                                            // (38.4 MHz) ; "10/11" - Slow_clk

#define APPS_RCM_WDOG_A_CLK_GATING_WDOG_A_BAUD_CLK_SEL_S 24
#define APPS_RCM_WDOG_A_CLK_GATING_WDOG_A_DSLP_CLK_ENABLE \
                                0x00010000  // 1 - Enable WDOG_A clk during
                                            // deep-sleep mode 0 - Disable
                                            // WDOG_A clk during deep-sleep mode
                                            // ;

#define APPS_RCM_WDOG_A_CLK_GATING_NU1_M \
                                0x0000FE00

#define APPS_RCM_WDOG_A_CLK_GATING_NU1_S 9
#define APPS_RCM_WDOG_A_CLK_GATING_WDOG_A_SLP_CLK_ENABLE \
                                0x00000100  // 1 - Enable WDOG_A clk during
                                            // sleep mode 0 - Disable WDOG_A clk
                                            // during sleep mode ;

#define APPS_RCM_WDOG_A_CLK_GATING_NU2_M \
                                0x000000FE

#define APPS_RCM_WDOG_A_CLK_GATING_NU2_S 1
#define APPS_RCM_WDOG_A_CLK_GATING_WDOG_A_RUN_CLK_ENABLE \
                                0x00000001  // 1 - Enable WDOG_A clk during run
                                            // mode 0 - Disable WDOG_A clk
                                            // during run mode ;

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_WDOG_A_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_WDOG_A_SOFT_RESET_WDOG_A_ENABLED_STATUS \
                                0x00000002  // 1 - WDOG_A Clocks/Resets are
                                            // enabled ; 0 - WDOG_A
                                            // Clocks/Resets are disabled

#define APPS_RCM_WDOG_A_SOFT_RESET_WDOG_A_SOFT_RESET \
                                0x00000001  // 1 - Assert reset for WDOG_A ; 0
                                            // - De-assert reset for WDOG_A

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_UART_A0_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_UART_A0_CLK_GATING_UART_A0_DSLP_CLK_ENABLE \
                                0x00010000  // 1 - Enable UART_A0 clk during
                                            // deep-sleep mode 0 - Disable
                                            // UART_A0 clk during deep-sleep
                                            // mode ;

#define APPS_RCM_UART_A0_CLK_GATING_NU1_M \
                                0x0000FE00

#define APPS_RCM_UART_A0_CLK_GATING_NU1_S 9
#define APPS_RCM_UART_A0_CLK_GATING_UART_A0_SLP_CLK_ENABLE \
                                0x00000100  // 1 - Enable UART_A0 clk during
                                            // sleep mode 0 - Disable UART_A0
                                            // clk during sleep mode ;

#define APPS_RCM_UART_A0_CLK_GATING_NU2_M \
                                0x000000FE

#define APPS_RCM_UART_A0_CLK_GATING_NU2_S 1
#define APPS_RCM_UART_A0_CLK_GATING_UART_A0_RUN_CLK_ENABLE \
                                0x00000001  // 1 - Enable UART_A0 clk during
                                            // run mode 0 - Disable UART_A0 clk
                                            // during run mode ;

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_UART_A0_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_UART_A0_SOFT_RESET_UART_A0_ENABLED_STATUS \
                                0x00000002  // 1 - UART_A0 Clocks/Resets are
                                            // enabled ; 0 - UART_A0
                                            // Clocks/Resets are disabled

#define APPS_RCM_UART_A0_SOFT_RESET_UART_A0_SOFT_RESET \
                                0x00000001  // 1 - Assert reset for UART_A0 ; 0
                                            // - De-assert reset for UART_A0

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_UART_A1_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_UART_A1_CLK_GATING_UART_A1_DSLP_CLK_ENABLE \
                                0x00010000  // 1 - Enable UART_A1 clk during
                                            // deep-sleep mode 0 - Disable
                                            // UART_A1 clk during deep-sleep
                                            // mode ;

#define APPS_RCM_UART_A1_CLK_GATING_NU1_M \
                                0x0000FE00

#define APPS_RCM_UART_A1_CLK_GATING_NU1_S 9
#define APPS_RCM_UART_A1_CLK_GATING_UART_A1_SLP_CLK_ENABLE \
                                0x00000100  // 1 - Enable UART_A1 clk during
                                            // sleep mode 0 - Disable UART_A1
                                            // clk during sleep mode ;

#define APPS_RCM_UART_A1_CLK_GATING_NU2_M \
                                0x000000FE

#define APPS_RCM_UART_A1_CLK_GATING_NU2_S 1
#define APPS_RCM_UART_A1_CLK_GATING_UART_A1_RUN_CLK_ENABLE \
                                0x00000001  // 1 - Enable UART_A1 clk during
                                            // run mode 0 - Disable UART_A1 clk
                                            // during run mode ;

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_UART_A1_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_UART_A1_SOFT_RESET_UART_A1_ENABLED_STATUS \
                                0x00000002  // 1 - UART_A1 Clocks/Resets are
                                            // enabled ; 0 - UART_A1
                                            // Clocks/Resets are disabled

#define APPS_RCM_UART_A1_SOFT_RESET_UART_A1_SOFT_RESET \
                                0x00000001  // 1 - Assert the soft reset for
                                            // UART_A1 ; 0 - De-assert the soft
                                            // reset for UART_A1

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPT_A0_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_GPT_A0_CLK_GATING_GPT_A0_DSLP_CLK_ENABLE \
                                0x00010000  // 1 - Enable the GPT_A0 clock
                                            // during deep-sleep ; 0 - Disable
                                            // the GPT_A0 clock during
                                            // deep-sleep

#define APPS_RCM_GPT_A0_CLK_GATING_NU1_M \
                                0x0000FE00

#define APPS_RCM_GPT_A0_CLK_GATING_NU1_S 9
#define APPS_RCM_GPT_A0_CLK_GATING_GPT_A0_SLP_CLK_ENABLE \
                                0x00000100  // 1 - Enable the GPT_A0 clock
                                            // during sleep ; 0 - Disable the
                                            // GPT_A0 clock during sleep

#define APPS_RCM_GPT_A0_CLK_GATING_NU2_M \
                                0x000000FE

#define APPS_RCM_GPT_A0_CLK_GATING_NU2_S 1
#define APPS_RCM_GPT_A0_CLK_GATING_GPT_A0_RUN_CLK_ENABLE \
                                0x00000001  // 1 - Enable the GPT_A0 clock
                                            // during run ; 0 - Disable the
                                            // GPT_A0 clock during run

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPT_A0_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_GPT_A0_SOFT_RESET_GPT_A0_ENABLED_STATUS \
                                0x00000002  // 1 - GPT_A0 clocks/resets are
                                            // enabled ; 0 - GPT_A0
                                            // clocks/resets are disabled

#define APPS_RCM_GPT_A0_SOFT_RESET_GPT_A0_SOFT_RESET \
                                0x00000001  // 1 - Assert the soft reset for
                                            // GPT_A0 ; 0 - De-assert the soft
                                            // reset for GPT_A0

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPT_A1_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_GPT_A1_CLK_GATING_GPT_A1_DSLP_CLK_ENABLE \
                                0x00010000  // 1 - Enable the GPT_A1 clock
                                            // during deep-sleep ; 0 - Disable
                                            // the GPT_A1 clock during
                                            // deep-sleep

#define APPS_RCM_GPT_A1_CLK_GATING_NU1_M \
                                0x0000FE00

#define APPS_RCM_GPT_A1_CLK_GATING_NU1_S 9
#define APPS_RCM_GPT_A1_CLK_GATING_GPT_A1_SLP_CLK_ENABLE \
                                0x00000100  // 1 - Enable the GPT_A1 clock
                                            // during sleep ; 0 - Disable the
                                            // GPT_A1 clock during sleep

#define APPS_RCM_GPT_A1_CLK_GATING_NU2_M \
                                0x000000FE

#define APPS_RCM_GPT_A1_CLK_GATING_NU2_S 1
#define APPS_RCM_GPT_A1_CLK_GATING_GPT_A1_RUN_CLK_ENABLE \
                                0x00000001  // 1 - Enable the GPT_A1 clock
                                            // during run ; 0 - Disable the
                                            // GPT_A1 clock during run

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPT_A1_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_GPT_A1_SOFT_RESET_GPT_A1_ENABLED_STATUS \
                                0x00000002  // 1 - GPT_A1 clocks/resets are
                                            // enabled ; 0 - GPT_A1
                                            // clocks/resets are disabled

#define APPS_RCM_GPT_A1_SOFT_RESET_GPT_A1_SOFT_RESET \
                                0x00000001  // 1 - Assert the soft reset for
                                            // GPT_A1 ; 0 - De-assert the soft
                                            // reset for GPT_A1

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPT_A2_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_GPT_A2_CLK_GATING_GPT_A2_DSLP_CLK_ENABLE \
                                0x00010000  // 1 - Enable the GPT_A2 clock
                                            // during deep-sleep ; 0 - Disable
                                            // the GPT_A2 clock during
                                            // deep-sleep

#define APPS_RCM_GPT_A2_CLK_GATING_NU1_M \
                                0x0000FE00

#define APPS_RCM_GPT_A2_CLK_GATING_NU1_S 9
#define APPS_RCM_GPT_A2_CLK_GATING_GPT_A2_SLP_CLK_ENABLE \
                                0x00000100  // 1 - Enable the GPT_A2 clock
                                            // during sleep ; 0 - Disable the
                                            // GPT_A2 clock during sleep

#define APPS_RCM_GPT_A2_CLK_GATING_NU2_M \
                                0x000000FE

#define APPS_RCM_GPT_A2_CLK_GATING_NU2_S 1
#define APPS_RCM_GPT_A2_CLK_GATING_GPT_A2_RUN_CLK_ENABLE \
                                0x00000001  // 1 - Enable the GPT_A2 clock
                                            // during run ; 0 - Disable the
                                            // GPT_A2 clock during run

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPT_A2_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_GPT_A2_SOFT_RESET_GPT_A2_ENABLED_STATUS \
                                0x00000002  // 1 - GPT_A2 clocks/resets are
                                            // enabled ; 0 - GPT_A2
                                            // clocks/resets are disabled

#define APPS_RCM_GPT_A2_SOFT_RESET_GPT_A2_SOFT_RESET \
                                0x00000001  // 1 - Assert the soft reset for
                                            // GPT_A2 ; 0 - De-assert the soft
                                            // reset for GPT_A2

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPT_A3_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_GPT_A3_CLK_GATING_GPT_A3_DSLP_CLK_ENABLE \
                                0x00010000  // 1 - Enable the GPT_A3 clock
                                            // during deep-sleep ; 0 - Disable
                                            // the GPT_A3 clock during
                                            // deep-sleep

#define APPS_RCM_GPT_A3_CLK_GATING_NU1_M \
                                0x0000FE00

#define APPS_RCM_GPT_A3_CLK_GATING_NU1_S 9
#define APPS_RCM_GPT_A3_CLK_GATING_GPT_A3_SLP_CLK_ENABLE \
                                0x00000100  // 1 - Enable the GPT_A3 clock
                                            // during sleep ; 0 - Disable the
                                            // GPT_A3 clock during sleep

#define APPS_RCM_GPT_A3_CLK_GATING_NU2_M \
                                0x000000FE

#define APPS_RCM_GPT_A3_CLK_GATING_NU2_S 1
#define APPS_RCM_GPT_A3_CLK_GATING_GPT_A3_RUN_CLK_ENABLE \
                                0x00000001  // 1 - Enable the GPT_A3 clock
                                            // during run ; 0 - Disable the
                                            // GPT_A3 clock during run

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_GPT_A3_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_GPT_A3_SOFT_RESET_GPT_A3_ENABLED_STATUS \
                                0x00000002  // 1 - GPT_A3 Clocks/resets are
                                            // enabled ; 0 - GPT_A3
                                            // Clocks/resets are disabled

#define APPS_RCM_GPT_A3_SOFT_RESET_GPT_A3_SOFT_RESET \
                                0x00000001  // 1 - Assert the soft reset for
                                            // GPT_A3 ; 0 - De-assert the soft
                                            // reset for GPT_A3

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_MCASP_FRAC_CLK_CONFIG0 register.
//
//******************************************************************************
#define APPS_RCM_MCASP_FRAC_CLK_CONFIG0_MCASP_FRAC_DIV_DIVISOR_M \
                                0x03FF0000

#define APPS_RCM_MCASP_FRAC_CLK_CONFIG0_MCASP_FRAC_DIV_DIVISOR_S 16
#define APPS_RCM_MCASP_FRAC_CLK_CONFIG0_MCASP_FRAC_DIV_FRACTION_M \
                                0x0000FFFF

#define APPS_RCM_MCASP_FRAC_CLK_CONFIG0_MCASP_FRAC_DIV_FRACTION_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_MCASP_FRAC_CLK_CONFIG1 register.
//
//******************************************************************************
#define APPS_RCM_MCASP_FRAC_CLK_CONFIG1_MCASP_FRAC_DIV_SOFT_RESET \
                                0x00010000  // 1 - Assert the reset for MCASP
                                            // Frac-clk div; 0 - Donot assert
                                            // the reset for MCASP frac clk-div

#define APPS_RCM_MCASP_FRAC_CLK_CONFIG1_MCASP_FRAC_DIV_PERIOD_M \
                                0x000003FF

#define APPS_RCM_MCASP_FRAC_CLK_CONFIG1_MCASP_FRAC_DIV_PERIOD_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_CRYPTO_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_CRYPTO_CLK_GATING_CRYPTO_DSLP_CLK_ENABLE \
                                0x00010000  // 0 - Disable the Crypto clock
                                            // during deep-sleep

#define APPS_RCM_CRYPTO_CLK_GATING_NU1_M \
                                0x0000FE00

#define APPS_RCM_CRYPTO_CLK_GATING_NU1_S 9
#define APPS_RCM_CRYPTO_CLK_GATING_CRYPTO_SLP_CLK_ENABLE \
                                0x00000100  // 1 - Enable the Crypto clock
                                            // during sleep ; 0 - Disable the
                                            // Crypto clock during sleep

#define APPS_RCM_CRYPTO_CLK_GATING_NU2_M \
                                0x000000FE

#define APPS_RCM_CRYPTO_CLK_GATING_NU2_S 1
#define APPS_RCM_CRYPTO_CLK_GATING_CRYPTO_RUN_CLK_ENABLE \
                                0x00000001  // 1 - Enable the Crypto clock
                                            // during run ; 0 - Disable the
                                            // Crypto clock during run

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_CRYPTO_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_CRYPTO_SOFT_RESET_CRYPTO_ENABLED_STATUS \
                                0x00000002  // 1 - Crypto clocks/resets are
                                            // enabled ; 0 - Crypto
                                            // clocks/resets are disabled

#define APPS_RCM_CRYPTO_SOFT_RESET_CRYPTO_SOFT_RESET \
                                0x00000001  // 1 - Assert the soft reset for
                                            // Crypto ; 0 - De-assert the soft
                                            // reset for Crypto

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_MCSPI_S0_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_MCSPI_S0_CLK_GATING_MCSPI_S0_DSLP_CLK_ENABLE \
                                0x00010000  // 0 - Disable the MCSPI_S0 clock
                                            // during deep-sleep

#define APPS_RCM_MCSPI_S0_CLK_GATING_NU1_M \
                                0x0000FE00

#define APPS_RCM_MCSPI_S0_CLK_GATING_NU1_S 9
#define APPS_RCM_MCSPI_S0_CLK_GATING_MCSPI_S0_SLP_CLK_ENABLE \
                                0x00000100  // 1 - Enable the MCSPI_S0 clock
                                            // during sleep ; 0 - Disable the
                                            // MCSPI_S0 clock during sleep

#define APPS_RCM_MCSPI_S0_CLK_GATING_NU2_M \
                                0x000000FE

#define APPS_RCM_MCSPI_S0_CLK_GATING_NU2_S 1
#define APPS_RCM_MCSPI_S0_CLK_GATING_MCSPI_S0_RUN_CLK_ENABLE \
                                0x00000001  // 1 - Enable the MCSPI_S0 clock
                                            // during run ; 0 - Disable the
                                            // MCSPI_S0 clock during run

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_MCSPI_S0_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_MCSPI_S0_SOFT_RESET_MCSPI_S0_ENABLED_STATUS \
                                0x00000002  // 1 - MCSPI_S0 Clocks/Resets are
                                            // enabled ; 0 - MCSPI_S0
                                            // Clocks/resets are disabled

#define APPS_RCM_MCSPI_S0_SOFT_RESET_MCSPI_S0_SOFT_RESET \
                                0x00000001  // 1 - Assert the soft reset for
                                            // MCSPI_S0 ; 0 - De-assert the soft
                                            // reset for MCSPI_S0

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_MCSPI_S0_CLKDIV_CFG register.
//
//******************************************************************************
#define APPS_RCM_MCSPI_S0_CLKDIV_CFG_MCSPI_S0_BAUD_CLK_SEL \
                                0x00010000  // 0 - XTAL clk is used as baud-clk
                                            // for MCSPI_S0 ; 1 - PLL divclk is
                                            // used as buad-clk for MCSPI_S0

#define APPS_RCM_MCSPI_S0_CLKDIV_CFG_NU1_M \
                                0x0000F800

#define APPS_RCM_MCSPI_S0_CLKDIV_CFG_NU1_S 11
#define APPS_RCM_MCSPI_S0_CLKDIV_CFG_MCSPI_S0_PLLCLKDIV_OFF_TIME_M \
                                0x00000700  // Configuration of OFF-TIME for
                                            // dividing PLL clk (240 MHz) in
                                            // generation of MCSPI_S0 func-clk :
                                            // "000" - 1 "001" - 2 "010" - 3
                                            // "011" - 4 "100" - 5 "101" - 6
                                            // "110" - 7 "111" - 8

#define APPS_RCM_MCSPI_S0_CLKDIV_CFG_MCSPI_S0_PLLCLKDIV_OFF_TIME_S 8
#define APPS_RCM_MCSPI_S0_CLKDIV_CFG_NU2_M \
                                0x000000F8

#define APPS_RCM_MCSPI_S0_CLKDIV_CFG_NU2_S 3
#define APPS_RCM_MCSPI_S0_CLKDIV_CFG_MCSPI_S0_PLLCLKDIV_ON_TIME_M \
                                0x00000007  // Configuration of ON-TIME for
                                            // dividing PLL clk (240 MHz) in
                                            // generation of MCSPI_S0 func-clk :
                                            // "000" - 1 "001" - 2 "010" - 3
                                            // "011" - 4 "100" - 5 "101" - 6
                                            // "110" - 7 "111" - 8

#define APPS_RCM_MCSPI_S0_CLKDIV_CFG_MCSPI_S0_PLLCLKDIV_ON_TIME_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_I2C_CLK_GATING register.
//
//******************************************************************************
#define APPS_RCM_I2C_CLK_GATING_I2C_DSLP_CLK_ENABLE \
                                0x00010000  // 1 - Enable the I2C Clock during
                                            // deep-sleep 0 - Disable the I2C
                                            // clock during deep-sleep

#define APPS_RCM_I2C_CLK_GATING_NU1_M \
                                0x0000FE00

#define APPS_RCM_I2C_CLK_GATING_NU1_S 9
#define APPS_RCM_I2C_CLK_GATING_I2C_SLP_CLK_ENABLE \
                                0x00000100  // 1 - Enable the I2C clock during
                                            // sleep ; 0 - Disable the I2C clock
                                            // during sleep

#define APPS_RCM_I2C_CLK_GATING_NU2_M \
                                0x000000FE

#define APPS_RCM_I2C_CLK_GATING_NU2_S 1
#define APPS_RCM_I2C_CLK_GATING_I2C_RUN_CLK_ENABLE \
                                0x00000001  // 1 - Enable the I2C clock during
                                            // run ; 0 - Disable the I2C clock
                                            // during run

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_I2C_SOFT_RESET register.
//
//******************************************************************************
#define APPS_RCM_I2C_SOFT_RESET_I2C_ENABLED_STATUS \
                                0x00000002  // 1 - I2C Clocks/Resets are
                                            // enabled ; 0 - I2C clocks/resets
                                            // are disabled

#define APPS_RCM_I2C_SOFT_RESET_I2C_SOFT_RESET \
                                0x00000001  // 1 - Assert the soft reset for
                                            // Shared-I2C ; 0 - De-assert the
                                            // soft reset for Shared-I2C

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_APPS_LPDS_REQ register.
//
//******************************************************************************
#define APPS_RCM_APPS_LPDS_REQ_APPS_LPDS_REQ \
                                0x00000001  // 1 - Request for LPDS

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_APPS_TURBO_REQ register.
//
//******************************************************************************
#define APPS_RCM_APPS_TURBO_REQ_APPS_TURBO_REQ \
                                0x00000001  // 1 - Request for TURBO

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_APPS_DSLP_WAKE_CONFIG register.
//
//******************************************************************************
#define APPS_RCM_APPS_DSLP_WAKE_CONFIG_DSLP_WAKE_FROM_NWP_ENABLE \
                                0x00000002  // 1 - Enable the NWP to wake APPS
                                            // from deep-sleep ; 0 - Disable NWP
                                            // to wake APPS from deep-sleep

#define APPS_RCM_APPS_DSLP_WAKE_CONFIG_DSLP_WAKE_TIMER_ENABLE \
                                0x00000001  // 1 - Enable deep-sleep wake timer
                                            // in APPS RCM for deep-sleep; 0 -
                                            // Disable deep-sleep wake timer in
                                            // APPS RCM

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_APPS_DSLP_WAKE_TIMER_CFG register.
//
//******************************************************************************
#define APPS_RCM_APPS_DSLP_WAKE_TIMER_CFG_DSLP_WAKE_TIMER_OPP_CFG_M \
                                0xFFFF0000  // Configuration (in slow_clks)
                                            // which says when to request for
                                            // OPP during deep-sleep exit

#define APPS_RCM_APPS_DSLP_WAKE_TIMER_CFG_DSLP_WAKE_TIMER_OPP_CFG_S 16
#define APPS_RCM_APPS_DSLP_WAKE_TIMER_CFG_DSLP_WAKE_TIMER_WAKE_CFG_M \
                                0x0000FFFF  // Configuration (in slow_clks)
                                            // which says when to request for
                                            // WAKE during deep-sleep exit

#define APPS_RCM_APPS_DSLP_WAKE_TIMER_CFG_DSLP_WAKE_TIMER_WAKE_CFG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_APPS_RCM_SLP_WAKE_ENABLE register.
//
//******************************************************************************
#define APPS_RCM_APPS_RCM_SLP_WAKE_ENABLE_SLP_WAKE_FROM_NWP_ENABLE \
                                0x00000002  // 1- Enable the sleep wakeup due
                                            // to NWP request. 0- Disable the
                                            // sleep wakeup due to NWP request

#define APPS_RCM_APPS_RCM_SLP_WAKE_ENABLE_SLP_WAKE_TIMER_ENABLE \
                                0x00000001  // 1- Enable the sleep wakeup due
                                            // to sleep-timer; 0-Disable the
                                            // sleep wakeup due to sleep-timer

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_APPS_SLP_WAKETIMER_CFG register.
//
//******************************************************************************
#define APPS_RCM_APPS_SLP_WAKETIMER_CFG_SLP_WAKE_TIMER_CFG_M \
                                0xFFFFFFFF  // Configuration (number of
                                            // sysclks-80MHz) for the Sleep
                                            // wakeup timer

#define APPS_RCM_APPS_SLP_WAKETIMER_CFG_SLP_WAKE_TIMER_CFG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_APPS_TO_NWP_WAKE_REQUEST register.
//
//******************************************************************************
#define APPS_RCM_APPS_TO_NWP_WAKE_REQUEST_APPS_TO_NWP_WAKEUP_REQUEST \
                                0x00000001  // When 1 => APPS generated a wake
                                            // request to NWP (When NWP is in
                                            // any of its low-power modes :
                                            // SLP/DSLP/LPDS)

//******************************************************************************
//
// The following are defines for the bit fields in the
// APPS_RCM_O_APPS_RCM_INTERRUPT_STATUS register.
//
//******************************************************************************
#define APPS_RCM_APPS_RCM_INTERRUPT_STATUS_apps_deep_sleep_timer_wake \
                                0x00000008  // 1 - Indicates that deep-sleep
                                            // timer expiry had caused the
                                            // wakeup from deep-sleep

#define APPS_RCM_APPS_RCM_INTERRUPT_STATUS_apps_sleep_timer_wake \
                                0x00000004  // 1 - Indicates that sleep timer
                                            // expiry had caused the wakeup from
                                            // sleep

#define APPS_RCM_APPS_RCM_INTERRUPT_STATUS_apps_deep_sleep_wake_from_nwp \
                                0x00000002  // 1 - Indicates that NWP had
                                            // caused the wakeup from deep-sleep

#define APPS_RCM_APPS_RCM_INTERRUPT_STATUS_apps_sleep_wake_from_nwp \
                                0x00000001  // 1 - Indicates that NWP had
                                            // caused the wakeup from Sleep




#endif // __HW_APPS_RCM_H__
