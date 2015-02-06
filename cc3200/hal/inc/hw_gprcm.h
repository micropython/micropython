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

#ifndef __HW_GPRCM_H__
#define __HW_GPRCM_H__

//*****************************************************************************
//
// The following are defines for the GPRCM register offsets.
//
//*****************************************************************************
#define GPRCM_O_APPS_SOFT_RESET 0x00000000
#define GPRCM_O_APPS_LPDS_WAKEUP_CFG \
                                0x00000004

#define GPRCM_O_APPS_LPDS_WAKEUP_SRC \
                                0x00000008

#define GPRCM_O_APPS_RESET_CAUSE \
                                0x0000000C

#define GPRCM_O_APPS_LPDS_WAKETIME_OPP_CFG \
                                0x00000010

#define GPRCM_O_APPS_SRAM_DSLP_CFG \
                                0x00000018

#define GPRCM_O_APPS_SRAM_LPDS_CFG \
                                0x0000001C

#define GPRCM_O_APPS_LPDS_WAKETIME_WAKE_CFG \
                                0x00000020

#define GPRCM_O_TOP_DIE_ENABLE  0x00000100
#define GPRCM_O_TOP_DIE_ENABLE_PARAMETERS \
                                0x00000104

#define GPRCM_O_MCU_GLOBAL_SOFT_RESET \
                                0x00000108

#define GPRCM_O_ADC_CLK_CONFIG  0x0000010C
#define GPRCM_O_APPS_GPIO_WAKE_CONF \
                                0x00000110

#define GPRCM_O_EN_NWP_BOOT_WO_DEVINIT \
                                0x00000114

#define GPRCM_O_MEM_HCLK_DIV_CFG \
                                0x00000118

#define GPRCM_O_MEM_SYSCLK_DIV_CFG \
                                0x0000011C

#define GPRCM_O_APLLMCS_LOCK_TIME_CONF \
                                0x00000120

#define GPRCM_O_NWP_SOFT_RESET  0x00000400
#define GPRCM_O_NWP_LPDS_WAKEUP_CFG \
                                0x00000404

#define GPRCM_O_NWP_LPDS_WAKEUP_SRC \
                                0x00000408

#define GPRCM_O_NWP_RESET_CAUSE 0x0000040C
#define GPRCM_O_NWP_LPDS_WAKETIME_OPP_CFG \
                                0x00000410

#define GPRCM_O_NWP_SRAM_DSLP_CFG \
                                0x00000418

#define GPRCM_O_NWP_SRAM_LPDS_CFG \
                                0x0000041C

#define GPRCM_O_NWP_LPDS_WAKETIME_WAKE_CFG \
                                0x00000420

#define GPRCM_O_NWP_AUTONMS_SPI_MASTER_SEL \
                                0x00000424

#define GPRCM_O_NWP_AUTONMS_SPI_IDLE_REQ \
                                0x00000428

#define GPRCM_O_WLAN_TO_NWP_WAKE_REQUEST \
                                0x0000042C

#define GPRCM_O_NWP_TO_WLAN_WAKE_REQUEST \
                                0x00000430

#define GPRCM_O_NWP_GPIO_WAKE_CONF \
                                0x00000434

#define GPRCM_O_GPRCM_EFUSE_READ_REG12 \
                                0x00000438

#define GPRCM_O_GPRCM_DIEID_READ_REG5 \
                                0x00000448

#define GPRCM_O_GPRCM_DIEID_READ_REG6 \
                                0x0000044C

#define GPRCM_O_REF_FSM_CFG0    0x00000800
#define GPRCM_O_REF_FSM_CFG1    0x00000804
#define GPRCM_O_APLLMCS_WLAN_CONFIG0_40 \
                                0x00000808

#define GPRCM_O_APLLMCS_WLAN_CONFIG1_40 \
                                0x0000080C

#define GPRCM_O_APLLMCS_WLAN_CONFIG0_26 \
                                0x00000810

#define GPRCM_O_APLLMCS_WLAN_CONFIG1_26 \
                                0x00000814

#define GPRCM_O_APLLMCS_WLAN_OVERRIDES \
                                0x00000818

#define GPRCM_O_APLLMCS_MCU_RUN_CONFIG0_38P4 \
                                0x0000081C

#define GPRCM_O_APLLMCS_MCU_RUN_CONFIG1_38P4 \
                                0x00000820

#define GPRCM_O_APLLMCS_MCU_RUN_CONFIG0_26 \
                                0x00000824

#define GPRCM_O_APLLMCS_MCU_RUN_CONFIG1_26 \
                                0x00000828

#define GPRCM_O_SPARE_RW0       0x0000082C
#define GPRCM_O_SPARE_RW1       0x00000830
#define GPRCM_O_APLLMCS_MCU_OVERRIDES \
                                0x00000834

#define GPRCM_O_SYSCLK_SWITCH_STATUS \
                                0x00000838

#define GPRCM_O_REF_LDO_CONTROLS \
                                0x0000083C

#define GPRCM_O_REF_RTRIM_CONTROL \
                                0x00000840

#define GPRCM_O_REF_SLICER_CONTROLS0 \
                                0x00000844

#define GPRCM_O_REF_SLICER_CONTROLS1 \
                                0x00000848

#define GPRCM_O_REF_ANA_BGAP_CONTROLS0 \
                                0x0000084C

#define GPRCM_O_REF_ANA_BGAP_CONTROLS1 \
                                0x00000850

#define GPRCM_O_REF_ANA_SPARE_CONTROLS0 \
                                0x00000854

#define GPRCM_O_REF_ANA_SPARE_CONTROLS1 \
                                0x00000858

#define GPRCM_O_MEMSS_PSCON_OVERRIDES0 \
                                0x0000085C

#define GPRCM_O_MEMSS_PSCON_OVERRIDES1 \
                                0x00000860

#define GPRCM_O_PLL_REF_LOCK_OVERRIDES \
                                0x00000864

#define GPRCM_O_MCU_PSCON_DEBUG 0x00000868
#define GPRCM_O_MEMSS_PWR_PS    0x0000086C
#define GPRCM_O_REF_FSM_DEBUG   0x00000870
#define GPRCM_O_MEM_SYS_OPP_REQ_OVERRIDE \
                                0x00000874

#define GPRCM_O_MEM_TESTCTRL_PD_OPP_CONFIG \
                                0x00000878

#define GPRCM_O_MEM_WL_FAST_CLK_REQ_OVERRIDES \
                                0x0000087C

#define GPRCM_O_MEM_MCU_PD_MODE_REQ_OVERRIDES \
                                0x00000880

#define GPRCM_O_MEM_MCSPI_SRAM_OFF_REQ_OVERRIDES \
                                0x00000884

#define GPRCM_O_MEM_WLAN_APLLMCS_OVERRIDES \
                                0x00000888

#define GPRCM_O_MEM_REF_FSM_CFG2 \
                                0x0000088C

#define GPRCM_O_TESTCTRL_POWER_CTRL \
                                0x00000C10

#define GPRCM_O_SSDIO_POWER_CTRL \
                                0x00000C14

#define GPRCM_O_MCSPI_N1_POWER_CTRL \
                                0x00000C18

#define GPRCM_O_WELP_POWER_CTRL 0x00000C1C
#define GPRCM_O_WL_SDIO_POWER_CTRL \
                                0x00000C20

#define GPRCM_O_WLAN_SRAM_ACTIVE_PWR_CFG \
                                0x00000C24

#define GPRCM_O_WLAN_SRAM_SLEEP_PWR_CFG \
                                0x00000C28

#define GPRCM_O_APPS_SECURE_INIT_DONE \
                                0x00000C30

#define GPRCM_O_APPS_DEV_MODE_INIT_DONE \
                                0x00000C34

#define GPRCM_O_EN_APPS_REBOOT  0x00000C38
#define GPRCM_O_MEM_APPS_PERIPH_PRESENT \
                                0x00000C3C

#define GPRCM_O_MEM_NWP_PERIPH_PRESENT \
                                0x00000C40

#define GPRCM_O_MEM_SHARED_PERIPH_PRESENT \
                                0x00000C44

#define GPRCM_O_NWP_PWR_STATE   0x00000C48
#define GPRCM_O_APPS_PWR_STATE  0x00000C4C
#define GPRCM_O_MCU_PWR_STATE   0x00000C50
#define GPRCM_O_WTOP_PM_PS      0x00000C54
#define GPRCM_O_WTOP_PD_RESETZ_OVERRIDE_REG \
                                0x00000C58

#define GPRCM_O_WELP_PD_RESETZ_OVERRIDE_REG \
                                0x00000C5C

#define GPRCM_O_WL_SDIO_PD_RESETZ_OVERRIDE_REG \
                                0x00000C60

#define GPRCM_O_SSDIO_PD_RESETZ_OVERRIDE_REG \
                                0x00000C64

#define GPRCM_O_MCSPI_N1_PD_RESETZ_OVERRIDE_REG \
                                0x00000C68

#define GPRCM_O_TESTCTRL_PD_RESETZ_OVERRIDE_REG \
                                0x00000C6C

#define GPRCM_O_MCU_PD_RESETZ_OVERRIDE_REG \
                                0x00000C70

#define GPRCM_O_GPRCM_EFUSE_READ_REG0 \
                                0x00000C78

#define GPRCM_O_GPRCM_EFUSE_READ_REG1 \
                                0x00000C7C

#define GPRCM_O_GPRCM_EFUSE_READ_REG2 \
                                0x00000C80

#define GPRCM_O_GPRCM_EFUSE_READ_REG3 \
                                0x00000C84

#define GPRCM_O_WTOP_MEM_RET_CFG \
                                0x00000C88

#define GPRCM_O_COEX_CLK_SWALLOW_CFG0 \
                                0x00000C8C

#define GPRCM_O_COEX_CLK_SWALLOW_CFG1 \
                                0x00000C90

#define GPRCM_O_COEX_CLK_SWALLOW_CFG2 \
                                0x00000C94

#define GPRCM_O_COEX_CLK_SWALLOW_ENABLE \
                                0x00000C98

#define GPRCM_O_DCDC_CLK_GEN_CONFIG \
                                0x00000C9C

#define GPRCM_O_GPRCM_EFUSE_READ_REG4 \
                                0x00000CA0

#define GPRCM_O_GPRCM_EFUSE_READ_REG5 \
                                0x00000CA4

#define GPRCM_O_GPRCM_EFUSE_READ_REG6 \
                                0x00000CA8

#define GPRCM_O_GPRCM_EFUSE_READ_REG7 \
                                0x00000CAC

#define GPRCM_O_GPRCM_EFUSE_READ_REG8 \
                                0x00000CB0

#define GPRCM_O_GPRCM_EFUSE_READ_REG9 \
                                0x00000CB4

#define GPRCM_O_GPRCM_EFUSE_READ_REG10 \
                                0x00000CB8

#define GPRCM_O_GPRCM_EFUSE_READ_REG11 \
                                0x00000CBC

#define GPRCM_O_GPRCM_DIEID_READ_REG0 \
                                0x00000CC0

#define GPRCM_O_GPRCM_DIEID_READ_REG1 \
                                0x00000CC4

#define GPRCM_O_GPRCM_DIEID_READ_REG2 \
                                0x00000CC8

#define GPRCM_O_GPRCM_DIEID_READ_REG3 \
                                0x00000CCC

#define GPRCM_O_GPRCM_DIEID_READ_REG4 \
                                0x00000CD0

#define GPRCM_O_APPS_SS_OVERRIDES \
                                0x00000CD4

#define GPRCM_O_NWP_SS_OVERRIDES \
                                0x00000CD8

#define GPRCM_O_SHARED_SS_OVERRIDES \
                                0x00000CDC

#define GPRCM_O_IDMEM_CORE_RST_OVERRIDES \
                                0x00000CE0

#define GPRCM_O_TOP_DIE_FSM_OVERRIDES \
                                0x00000CE4

#define GPRCM_O_MCU_PSCON_OVERRIDES \
                                0x00000CE8

#define GPRCM_O_WTOP_PSCON_OVERRIDES \
                                0x00000CEC

#define GPRCM_O_WELP_PSCON_OVERRIDES \
                                0x00000CF0

#define GPRCM_O_WL_SDIO_PSCON_OVERRIDES \
                                0x00000CF4

#define GPRCM_O_MCSPI_PSCON_OVERRIDES \
                                0x00000CF8

#define GPRCM_O_SSDIO_PSCON_OVERRIDES \
                                0x00000CFC




//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APPS_SOFT_RESET register.
//
//******************************************************************************
#define GPRCM_APPS_SOFT_RESET_APPS_SOFT_RESET1 \
                                0x00000002  // Soft-reset1 for APPS : Cortex
                                            // sysrstn is asserted and in
                                            // addition to that the associated
                                            // APPS Peripherals are also reset.
                                            // This is an auto-clear bit.

#define GPRCM_APPS_SOFT_RESET_APPS_SOFT_RESET0 \
                                0x00000001  // Soft-reset0 for APPS : Only
                                            // sys-resetn for Cortex will be
                                            // asserted. This is an auto-clear
                                            // bit.

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APPS_LPDS_WAKEUP_CFG register.
//
//******************************************************************************
#define GPRCM_APPS_LPDS_WAKEUP_CFG_APPS_LPDS_WAKEUP_CFG_M \
                                0x000000FF  // Mask for LPDS Wakeup interrupt :
                                            // [7] - Host IRQ from NWP [6] -
                                            // NWP_LPDS_Wake_irq (TRUE_LPDS) [5]
                                            // - NWP Wake-request to APPS [4] -
                                            // GPIO [3:1] - Reserved [0] - LPDS
                                            // Wakeup-timer

#define GPRCM_APPS_LPDS_WAKEUP_CFG_APPS_LPDS_WAKEUP_CFG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APPS_LPDS_WAKEUP_SRC register.
//
//******************************************************************************
#define GPRCM_APPS_LPDS_WAKEUP_SRC_APPS_LPDS_WAKEUP_SRC_M \
                                0x000000FF  // Indicates the cause for wakeup
                                            // from LPDS : [7] - Host IRQ from
                                            // NWP [6] - NWP_LPDS_Wake_irq
                                            // (TRUE_LPDS) [5] - NWP
                                            // Wake-request to APPS [4] - GPIO
                                            // [3:1] - Reserved [0] - LPDS
                                            // Wakeup-timer

#define GPRCM_APPS_LPDS_WAKEUP_SRC_APPS_LPDS_WAKEUP_SRC_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APPS_RESET_CAUSE register.
//
//******************************************************************************
#define GPRCM_APPS_RESET_CAUSE_APPS_RESET_CAUSE_M \
                                0x000000FF  // Indicates the reset cause for
                                            // APPS : "0000" - Wake from HIB/OFF
                                            // mode; "0001" - Wake from LPDS ;
                                            // "0010" - Reserved ; "0011" -
                                            // Soft-reset0 (Only APPS
                                            // Cortex-sysrstn is asserted);
                                            // "0100" - Soft-reset1 (APPS
                                            // Cortex-sysrstn and APPS
                                            // peripherals are reset); "0101" -
                                            // WDOG0 (APPS Cortex-sysrstn and
                                            // APPS peripherals are reset);
                                            // "0110" - MCU Soft-reset (APPS +
                                            // NWP Cortex-sysrstn + Peripherals
                                            // are reset); "0111" - Secure Init
                                            // done (Indication that reset has
                                            // happened after DevInit); "1000" -
                                            // Dev Mode Patch Init done (During
                                            // development mode, patch
                                            // downloading and Cortex
                                            // re-vectoring is completed)

#define GPRCM_APPS_RESET_CAUSE_APPS_RESET_CAUSE_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APPS_LPDS_WAKETIME_OPP_CFG register.
//
//******************************************************************************
#define GPRCM_APPS_LPDS_WAKETIME_OPP_CFG_APPS_LPDS_WAKETIME_OPP_CFG_M \
                                0xFFFFFFFF  // OPP Request Configuration
                                            // (Number of slow-clk cycles) for
                                            // LPDS Wake-timer : This
                                            // configuration implies the RTC
                                            // time-stamp, which must be few
                                            // slow-clks prior to
                                            // APPS_LPDS_WAKETIME_WAKE_CFG, such
                                            // that by the time actual wakeup is
                                            // given, OPP is already switched to
                                            // ACTIVE (RUN).

#define GPRCM_APPS_LPDS_WAKETIME_OPP_CFG_APPS_LPDS_WAKETIME_OPP_CFG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APPS_SRAM_DSLP_CFG register.
//
//******************************************************************************
#define GPRCM_APPS_SRAM_DSLP_CFG_APPS_SRAM_DSLP_CFG_M \
                                0x000FFFFF  // Configuration of APPS Memories
                                            // during Deep-sleep : 0 - SRAMs are
                                            // OFF ; 1 - SRAMs are Retained.
                                            // APPS SRAM Cluster information :
                                            // [0] - 1st column in MEMSS
                                            // (Applicable only when owned by
                                            // APPS); [1] - 2nd column in MEMSS
                                            // (Applicable only when owned by
                                            // APPS); [2] - 3rd column in MEMSS
                                            // (Applicable only when owned by
                                            // APPS) ; [3] - 4th column in MEMSS
                                            // (Applicable only when owned by
                                            // APPS) ; [16] - MCU-PD - Apps
                                            // cluster 0 (TBD); [19:18] -
                                            // Reserved.

#define GPRCM_APPS_SRAM_DSLP_CFG_APPS_SRAM_DSLP_CFG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APPS_SRAM_LPDS_CFG register.
//
//******************************************************************************
#define GPRCM_APPS_SRAM_LPDS_CFG_APPS_SRAM_LPDS_CFG_M \
                                0x000FFFFF  // Configuration of APPS Memories
                                            // during LPDS : 0 - SRAMs are OFF ;
                                            // 1 - SRAMs are Retained. APPS SRAM
                                            // Cluster information : [0] - 1st
                                            // column in MEMSS (Applicable only
                                            // when owned by APPS); [1] - 2nd
                                            // column in MEMSS (Applicable only
                                            // when owned by APPS); [2] - 3rd
                                            // column in MEMSS (Applicable only
                                            // when owned by APPS) ; [3] - 4th
                                            // column in MEMSS (Applicable only
                                            // when owned by APPS) ; [16] -
                                            // MCU-PD - Apps cluster 0 (TBD);
                                            // [19:18] - Reserved.

#define GPRCM_APPS_SRAM_LPDS_CFG_APPS_SRAM_LPDS_CFG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APPS_LPDS_WAKETIME_WAKE_CFG register.
//
//******************************************************************************
#define GPRCM_APPS_LPDS_WAKETIME_WAKE_CFG_APPS_LPDS_WAKETIME_WAKE_CFG_M \
                                0xFFFFFFFF  // Configuration (in no of
                                            // slow_clks) which says when the
                                            // actual wakeup request for
                                            // removing the PD-reset be given.

#define GPRCM_APPS_LPDS_WAKETIME_WAKE_CFG_APPS_LPDS_WAKETIME_WAKE_CFG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_TOP_DIE_ENABLE register.
//
//******************************************************************************
#define GPRCM_TOP_DIE_ENABLE_FLASH_BUSY \
                                0x00001000

#define GPRCM_TOP_DIE_ENABLE_TOP_DIE_PWR_PS_M \
                                0x00000F00

#define GPRCM_TOP_DIE_ENABLE_TOP_DIE_PWR_PS_S 8
#define GPRCM_TOP_DIE_ENABLE_TOP_DIE_ENABLE_STATUS \
                                0x00000002  // 1 - Top-die is enabled ;

#define GPRCM_TOP_DIE_ENABLE_TOP_DIE_ENABLE \
                                0x00000001  // 1 - Enable the top-die ; 0 -
                                            // Disable the top-die

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_TOP_DIE_ENABLE_PARAMETERS register.
//
//******************************************************************************
#define GPRCM_TOP_DIE_ENABLE_PARAMETERS_FLASH_3P3_RSTN2D2D_POR_RSTN_M \
                                0xF0000000  // Configuration (in slow_clks) for
                                            // number of clks between
                                            // Flash-3p3-rstn to D2D POR Resetn.

#define GPRCM_TOP_DIE_ENABLE_PARAMETERS_FLASH_3P3_RSTN2D2D_POR_RSTN_S 28
#define GPRCM_TOP_DIE_ENABLE_PARAMETERS_TOP_DIE_SW_EN2TOP_DIE_FLASH_3P3_RSTN_M \
                                0x00FF0000  // Configuration (in slow_clks) for
                                            // number of clks between Top-die
                                            // Switch-Enable and Top-die Flash
                                            // 3p3 Reset removal

#define GPRCM_TOP_DIE_ENABLE_PARAMETERS_TOP_DIE_SW_EN2TOP_DIE_FLASH_3P3_RSTN_S 16
#define GPRCM_TOP_DIE_ENABLE_PARAMETERS_TOP_DIE_POR_RSTN2BOTT_DIE_FMC_RSTN_M \
                                0x000000FF  // Configuration (in slow_clks) for
                                            // number of clks between D2D POR
                                            // Reset removal and bottom die FMC
                                            // reset removal

#define GPRCM_TOP_DIE_ENABLE_PARAMETERS_TOP_DIE_POR_RSTN2BOTT_DIE_FMC_RSTN_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MCU_GLOBAL_SOFT_RESET register.
//
//******************************************************************************
#define GPRCM_MCU_GLOBAL_SOFT_RESET_MCU_GLOBAL_SOFT_RESET \
                                0x00000001  // 1 - Assert the global reset for
                                            // MCU (APPS + NWP) ; Asserts both
                                            // Cortex sysrstn and its
                                            // peripherals 0 - Deassert the
                                            // global reset for MCU (APPS + NWP)
                                            // ; Asserts both Cortex sysrstn and
                                            // its peripherals Note : Reset for
                                            // shared peripherals is not
                                            // affected here.

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_ADC_CLK_CONFIG register.
//
//******************************************************************************
#define GPRCM_ADC_CLK_CONFIG_ADC_CLKGEN_OFF_TIME_M \
                                0x000007C0  // Configuration (in number of 38.4
                                            // MHz clks) for the OFF-Time in
                                            // generation of ADC_CLK

#define GPRCM_ADC_CLK_CONFIG_ADC_CLKGEN_OFF_TIME_S 6
#define GPRCM_ADC_CLK_CONFIG_ADC_CLKGEN_ON_TIME_M \
                                0x0000003E  // Configuration (in number of 38.4
                                            // MHz clks) for the ON-Time in
                                            // generation of ADC_CLK

#define GPRCM_ADC_CLK_CONFIG_ADC_CLKGEN_ON_TIME_S 1
#define GPRCM_ADC_CLK_CONFIG_ADC_CLK_ENABLE \
                                0x00000001  // 1 - Enable the ADC_CLK ; 0 -
                                            // Disable the ADC_CLK

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APPS_GPIO_WAKE_CONF register.
//
//******************************************************************************
#define GPRCM_APPS_GPIO_WAKE_CONF_APPS_GPIO_WAKE_CONF_M \
                                0x00000003  // "00" - Wake on Level0 on
                                            // selected GPIO pin (GPIO is
                                            // selected inside the HIB3p3
                                            // module); "01" - Wakeup on
                                            // fall-edge of GPIO pin.

#define GPRCM_APPS_GPIO_WAKE_CONF_APPS_GPIO_WAKE_CONF_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_EN_NWP_BOOT_WO_DEVINIT register.
//
//******************************************************************************
#define GPRCM_EN_NWP_BOOT_WO_DEVINIT_reserved_M \
                                0xFFFFFFFE

#define GPRCM_EN_NWP_BOOT_WO_DEVINIT_reserved_S 1
#define GPRCM_EN_NWP_BOOT_WO_DEVINIT_mem_en_nwp_boot_wo_devinit \
                                0x00000001  // 1 - Override the secure-mode
                                            // done for booting up NWP (Wakeup
                                            // NWP on its event independent of
                                            // CM4 state) ; 0 - Donot override
                                            // the secure-mode done for NWP boot
                                            // (NWP must be enabled by CM4 only)

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MEM_HCLK_DIV_CFG register.
//
//******************************************************************************
#define GPRCM_MEM_HCLK_DIV_CFG_mem_hclk_div_cfg_M \
                                0x00000007  // Division configuration for
                                            // HCLKDIVOUT : "000" - Divide by 1
                                            // ; "001" - Divide by 2 ; "010" -
                                            // Divide by 3 ; "011" - Divide by 4
                                            // ; "100" - Divide by 5 ; "101" -
                                            // Divide by 6 ; "110" - Divide by 7
                                            // ; "111" - Divide by 8

#define GPRCM_MEM_HCLK_DIV_CFG_mem_hclk_div_cfg_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MEM_SYSCLK_DIV_CFG register.
//
//******************************************************************************
#define GPRCM_MEM_SYSCLK_DIV_CFG_mem_sysclk_div_off_time_M \
                                0x00000038

#define GPRCM_MEM_SYSCLK_DIV_CFG_mem_sysclk_div_off_time_S 3
#define GPRCM_MEM_SYSCLK_DIV_CFG_mem_sysclk_div_on_time_M \
                                0x00000007

#define GPRCM_MEM_SYSCLK_DIV_CFG_mem_sysclk_div_on_time_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APLLMCS_LOCK_TIME_CONF register.
//
//******************************************************************************
#define GPRCM_APLLMCS_LOCK_TIME_CONF_mem_apllmcs_wlan_lock_time_M \
                                0x0000FF00

#define GPRCM_APLLMCS_LOCK_TIME_CONF_mem_apllmcs_wlan_lock_time_S 8
#define GPRCM_APLLMCS_LOCK_TIME_CONF_mem_apllmcs_mcu_lock_time_M \
                                0x000000FF

#define GPRCM_APLLMCS_LOCK_TIME_CONF_mem_apllmcs_mcu_lock_time_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_NWP_SOFT_RESET register.
//
//******************************************************************************
#define GPRCM_NWP_SOFT_RESET_NWP_SOFT_RESET1 \
                                0x00000002  // Soft-reset1 for NWP - Cortex
                                            // sysrstn and NWP associated
                                            // peripherals are - This is an
                                            // auto-clr bit.

#define GPRCM_NWP_SOFT_RESET_NWP_SOFT_RESET0 \
                                0x00000001  // Soft-reset0 for NWP - Only
                                            // Cortex-sysrstn is asserted - This
                                            // is an auto-clear bit.

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_NWP_LPDS_WAKEUP_CFG register.
//
//******************************************************************************
#define GPRCM_NWP_LPDS_WAKEUP_CFG_NWP_LPDS_WAKEUP_CFG_M \
                                0x000000FF  // Mask for LPDS Wakeup interrupt :
                                            // 7 - WLAN Host Interrupt ; 6 -
                                            // WLAN to NWP Wake request ; 5 -
                                            // APPS to NWP Wake request; 4 -
                                            // GPIO Wakeup ; 3 - Autonomous UART
                                            // Wakeup ; 2 - SSDIO Wakeup ; 1 -
                                            // Autonomous SPI Wakeup ; 0 - LPDS
                                            // Wakeup-timer

#define GPRCM_NWP_LPDS_WAKEUP_CFG_NWP_LPDS_WAKEUP_CFG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_NWP_LPDS_WAKEUP_SRC register.
//
//******************************************************************************
#define GPRCM_NWP_LPDS_WAKEUP_SRC_NWP_LPDS_WAKEUP_SRC_M \
                                0x000000FF  // Indicates the cause for NWP
                                            // LPDS-Wakeup : 7 - WLAN Host
                                            // Interrupt ; 6 - WLAN to NWP Wake
                                            // request ; 5 - APPS to NWP Wake
                                            // request; 4 - GPIO Wakeup ; 3 -
                                            // Autonomous UART Wakeup ; 2 -
                                            // SSDIO Wakeup ; 1 - Autonomous SPI
                                            // Wakeup ; 0 - LPDS Wakeup-timer

#define GPRCM_NWP_LPDS_WAKEUP_SRC_NWP_LPDS_WAKEUP_SRC_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_NWP_RESET_CAUSE register.
//
//******************************************************************************
#define GPRCM_NWP_RESET_CAUSE_NWP_RESET_CAUSE_M \
                                0x000000FF  // Indicates the reset cause for
                                            // NWP : "0000" - Wake from HIB/OFF
                                            // mode; "0001" - Wake from LPDS ;
                                            // "0010" - Reserved ; "0011" -
                                            // Soft-reset0 (Only NWP
                                            // Cortex-sysrstn is asserted);
                                            // "0100" - Soft-reset1 (NWP
                                            // Cortex-sysrstn and NWP
                                            // peripherals are reset); "0101" -
                                            // WDOG0 (NWP Cortex-sysrstn and NWP
                                            // peripherals are reset); "0110" -
                                            // MCU Soft-reset (APPS + NWP
                                            // Cortex-sysrstn + Peripherals are
                                            // reset); "0111" - SSDIO Function2
                                            // reset (Only Cortex-sysrstn is
                                            // asserted) ; "1000" - Reset due to
                                            // WDOG of APPS (NWP Cortex-sysrstn
                                            // and NWP peripherals are reset);

#define GPRCM_NWP_RESET_CAUSE_NWP_RESET_CAUSE_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_NWP_LPDS_WAKETIME_OPP_CFG register.
//
//******************************************************************************
#define GPRCM_NWP_LPDS_WAKETIME_OPP_CFG_NWP_LPDS_WAKETIME_OPP_CFG_M \
                                0xFFFFFFFF  // OPP Request Configuration
                                            // (Number of slow-clk cycles) for
                                            // LPDS Wake-timer

#define GPRCM_NWP_LPDS_WAKETIME_OPP_CFG_NWP_LPDS_WAKETIME_OPP_CFG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_NWP_SRAM_DSLP_CFG register.
//
//******************************************************************************
#define GPRCM_NWP_SRAM_DSLP_CFG_NWP_SRAM_DSLP_CFG_M \
                                0x000FFFFF  // Configuration of NWP Memories
                                            // during DSLP : 0 - SRAMs are OFF ;
                                            // 1 - SRAMs are Retained. NWP SRAM
                                            // Cluster information : [2] - 3rd
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [3] - 4th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [4] - 5th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [5] - 6th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [6] - 7th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [7] - 8th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [8] - 9th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [9] - 10th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [10] - 11th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [11] - 12th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [12] - 13th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [13] - 14th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [14] - 15th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [19:18] -
                                            // Reserved.

#define GPRCM_NWP_SRAM_DSLP_CFG_NWP_SRAM_DSLP_CFG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_NWP_SRAM_LPDS_CFG register.
//
//******************************************************************************
#define GPRCM_NWP_SRAM_LPDS_CFG_NWP_SRAM_LPDS_CFG_M \
                                0x000FFFFF  // Configuration of NWP Memories
                                            // during LPDS : 0 - SRAMs are OFF ;
                                            // 1 - SRAMs are Retained. NWP SRAM
                                            // Cluster information : [2] - 3rd
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [3] - 4th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [4] - 5th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [5] - 6th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [6] - 7th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [7] - 8th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [8] - 9th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [9] - 10th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [10] - 11th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [11] - 12th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [12] - 13th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [13] - 14th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [14] - 15th
                                            // column in MEMSS (Applicable only
                                            // when owned by NWP) ; [19:18] -
                                            // Reserved.

#define GPRCM_NWP_SRAM_LPDS_CFG_NWP_SRAM_LPDS_CFG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_NWP_LPDS_WAKETIME_WAKE_CFG register.
//
//******************************************************************************
#define GPRCM_NWP_LPDS_WAKETIME_WAKE_CFG_NWP_LPDS_WAKETIME_WAKE_CFG_M \
                                0xFFFFFFFF  // Wake time configuration (no of
                                            // slow clks) for NWP wake from
                                            // LPDS.

#define GPRCM_NWP_LPDS_WAKETIME_WAKE_CFG_NWP_LPDS_WAKETIME_WAKE_CFG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_NWP_AUTONMS_SPI_MASTER_SEL register.
//
//******************************************************************************
#define GPRCM_NWP_AUTONMS_SPI_MASTER_SEL_F_M \
                                0xFFFE0000

#define GPRCM_NWP_AUTONMS_SPI_MASTER_SEL_F_S 17
#define GPRCM_NWP_AUTONMS_SPI_MASTER_SEL_MEM_AUTONMS_SPI_MASTER_SEL \
                                0x00010000  // 0 - APPS is selected as host for
                                            // Autonms SPI ; 1 - External host
                                            // is selected as host for Autonms
                                            // SPI

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_NWP_AUTONMS_SPI_IDLE_REQ register.
//
//******************************************************************************
#define GPRCM_NWP_AUTONMS_SPI_IDLE_REQ_NWP_AUTONMS_SPI_IDLE_WAKEUP \
                                0x00010000

#define GPRCM_NWP_AUTONMS_SPI_IDLE_REQ_NWP_AUTONMS_SPI_IDLE_ACK \
                                0x00000002  // When 1 => IDLE-mode is
                                            // acknowledged by the SPI-IP. (This
                                            // is for MCSPI_N1)

#define GPRCM_NWP_AUTONMS_SPI_IDLE_REQ_NWP_AUTONMS_SPI_IDLE_REQ \
                                0x00000001  // When 1 => Request for IDLE-mode
                                            // for autonomous SPI. (This is for
                                            // MCSPI_N1)

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_WLAN_TO_NWP_WAKE_REQUEST register.
//
//******************************************************************************
#define GPRCM_WLAN_TO_NWP_WAKE_REQUEST_WLAN_TO_NWP_WAKE_REQUEST \
                                0x00000001  // 1 - Request for waking up NWP
                                            // from any of its low-power modes
                                            // (SLP/DSLP/LPDS)

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_NWP_TO_WLAN_WAKE_REQUEST register.
//
//******************************************************************************
#define GPRCM_NWP_TO_WLAN_WAKE_REQUEST_NWP_TO_WLAN_WAKE_REQUEST \
                                0x00000001  // 1 - Request for wakinp up WLAN
                                            // from its ELP Mode (This gets
                                            // triggered to ELP-logic of WLAN)

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_NWP_GPIO_WAKE_CONF register.
//
//******************************************************************************
#define GPRCM_NWP_GPIO_WAKE_CONF_NWP_GPIO_WAKE_CONF_M \
                                0x00000003  // "00" - Wakeup on level0 of the
                                            // selected GPIO (GPIO gets selected
                                            // inside HIB3P3-module); "01" -
                                            // Wakeup on fall-edge of selected
                                            // GPIO.

#define GPRCM_NWP_GPIO_WAKE_CONF_NWP_GPIO_WAKE_CONF_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_EFUSE_READ_REG12 register.
//
//******************************************************************************
#define GPRCM_GPRCM_EFUSE_READ_REG12_FUSEFARM_ROW_32_MSW_M \
                                0x0000FFFF  // This corrsponds to ROW_32
                                            // [31:16] of the FUSEFARM. SPARE

#define GPRCM_GPRCM_EFUSE_READ_REG12_FUSEFARM_ROW_32_MSW_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_DIEID_READ_REG5 register.
//
//******************************************************************************
#define GPRCM_GPRCM_DIEID_READ_REG5_FUSEFARM_ROW_10_M \
                                0xFFFFFFFF  // Corresponds to ROW10 of FUSEFARM
                                            // : [5:0] - ADC OFFSET ; [13:6] -
                                            // TEMP_SENSE ; [14:14] - DFT_GSG ;
                                            // [15:15] - FMC_DISABLE ; [31:16] -
                                            // WLAN_MAC ID

#define GPRCM_GPRCM_DIEID_READ_REG5_FUSEFARM_ROW_10_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_DIEID_READ_REG6 register.
//
//******************************************************************************
#define GPRCM_GPRCM_DIEID_READ_REG6_FUSEFARM_ROW_11_M \
                                0xFFFFFFFF  // Corresponds to ROW11 of FUSEFARM
                                            // : [31:0] : WLAN MAC ID

#define GPRCM_GPRCM_DIEID_READ_REG6_FUSEFARM_ROW_11_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_REF_FSM_CFG0 register.
//
//******************************************************************************
#define GPRCM_REF_FSM_CFG0_BGAP_SETTLING_TIME_M \
                                0x00FF0000  // ANA-BGAP Settling time (In
                                            // number of slow_clks)

#define GPRCM_REF_FSM_CFG0_BGAP_SETTLING_TIME_S 16
#define GPRCM_REF_FSM_CFG0_FREF_LDO_SETTLING_TIME_M \
                                0x0000FF00  // Slicer LDO settling time (In
                                            // number of slow clks)

#define GPRCM_REF_FSM_CFG0_FREF_LDO_SETTLING_TIME_S 8
#define GPRCM_REF_FSM_CFG0_DIG_BUF_SETTLING_TIME_M \
                                0x000000FF  // Dig-buffer settling time (In
                                            // number of slow clks)

#define GPRCM_REF_FSM_CFG0_DIG_BUF_SETTLING_TIME_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_REF_FSM_CFG1 register.
//
//******************************************************************************
#define GPRCM_REF_FSM_CFG1_XTAL_SETTLING_TIME_M \
                                0xFF000000  // XTAL settling time (In number of
                                            // slow clks)

#define GPRCM_REF_FSM_CFG1_XTAL_SETTLING_TIME_S 24
#define GPRCM_REF_FSM_CFG1_SLICER_LV_SETTLING_TIME_M \
                                0x00FF0000  // LV Slicer settling time

#define GPRCM_REF_FSM_CFG1_SLICER_LV_SETTLING_TIME_S 16
#define GPRCM_REF_FSM_CFG1_SLICER_HV_PD_SETTLING_TIME_M \
                                0x0000FF00  // HV Slicer Pull-down settling
                                            // time

#define GPRCM_REF_FSM_CFG1_SLICER_HV_PD_SETTLING_TIME_S 8
#define GPRCM_REF_FSM_CFG1_SLICER_HV_SETTLING_TIME_M \
                                0x000000FF  // HV Slicer settling time

#define GPRCM_REF_FSM_CFG1_SLICER_HV_SETTLING_TIME_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APLLMCS_WLAN_CONFIG0_40 register.
//
//******************************************************************************
#define GPRCM_APLLMCS_WLAN_CONFIG0_40_APLLMCS_WLAN_N_40_M \
                                0x00007F00  // Configuration for WLAN APLLMCS -
                                            // N[6:0], if the XTAL frequency is
                                            // 40 MHz (Selected by efuse)

#define GPRCM_APLLMCS_WLAN_CONFIG0_40_APLLMCS_WLAN_N_40_S 8
#define GPRCM_APLLMCS_WLAN_CONFIG0_40_APLLMCS_WLAN_M_40_M \
                                0x000000FF  // Configuration for WLAN APLLMCS -
                                            // M[7:0], if the XTAL frequency is
                                            // 40 MHz (Selected by efuse)

#define GPRCM_APLLMCS_WLAN_CONFIG0_40_APLLMCS_WLAN_M_40_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APLLMCS_WLAN_CONFIG1_40 register.
//
//******************************************************************************
#define GPRCM_APLLMCS_WLAN_CONFIG1_40_APLLMCS_HISPEED_40 \
                                0x00000010  // Configuration for WLAN APLLMCS -
                                            // if the XTAL frequency if 40 MHz
                                            // (Selected by Efuse)

#define GPRCM_APLLMCS_WLAN_CONFIG1_40_APLLMCS_SEL96_40 \
                                0x00000008  // Configuration for WLAN APLLMCS -
                                            // Sel96, if the XTAL frequency is
                                            // 40 MHz (Selected by Efuse)

#define GPRCM_APLLMCS_WLAN_CONFIG1_40_APLLMCS_SELINPFREQ_40_M \
                                0x00000007  // Configuration for WLAN APLLMCS -
                                            // Selinpfreq, if the XTAL frequency
                                            // is 40 MHz (Selected by Efuse)

#define GPRCM_APLLMCS_WLAN_CONFIG1_40_APLLMCS_SELINPFREQ_40_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APLLMCS_WLAN_CONFIG0_26 register.
//
//******************************************************************************
#define GPRCM_APLLMCS_WLAN_CONFIG0_26_APLLMCS_WLAN_N_26_M \
                                0x00007F00  // Configuration for WLAN APLLMCS -
                                            // N[6:0], if the XTAL frequency is
                                            // 26 MHz (Selected by efuse)

#define GPRCM_APLLMCS_WLAN_CONFIG0_26_APLLMCS_WLAN_N_26_S 8
#define GPRCM_APLLMCS_WLAN_CONFIG0_26_APLLMCS_WLAN_M_26_M \
                                0x000000FF  // Configuration for WLAN APLLMCS -
                                            // M[7:0], if the XTAL frequency is
                                            // 26 MHz (Selected by efuse)

#define GPRCM_APLLMCS_WLAN_CONFIG0_26_APLLMCS_WLAN_M_26_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APLLMCS_WLAN_CONFIG1_26 register.
//
//******************************************************************************
#define GPRCM_APLLMCS_WLAN_CONFIG1_26_APLLMCS_HISPEED_26 \
                                0x00000010  // Configuration for WLAN APLLMCS -
                                            // if the XTAL frequency if 26 MHz
                                            // (Selected by Efuse)

#define GPRCM_APLLMCS_WLAN_CONFIG1_26_APLLMCS_SEL96_26 \
                                0x00000008  // Configuration for WLAN APLLMCS -
                                            // Sel96, if the XTAL frequency is
                                            // 26 MHz (Selected by Efuse)

#define GPRCM_APLLMCS_WLAN_CONFIG1_26_APLLMCS_SELINPFREQ_26_M \
                                0x00000007  // Configuration for WLAN APLLMCS -
                                            // Selinpfreq, if the XTAL frequency
                                            // is 26 MHz (Selected by Efuse)

#define GPRCM_APLLMCS_WLAN_CONFIG1_26_APLLMCS_SELINPFREQ_26_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APLLMCS_WLAN_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_APLLMCS_WLAN_OVERRIDES_APLLMCS_WLAN_POSTDIV_OVERRIDE_CTRL \
                                0x00080000

#define GPRCM_APLLMCS_WLAN_OVERRIDES_APLLMCS_WLAN_POSTDIV_OVERRIDE_M \
                                0x00070000

#define GPRCM_APLLMCS_WLAN_OVERRIDES_APLLMCS_WLAN_POSTDIV_OVERRIDE_S 16
#define GPRCM_APLLMCS_WLAN_OVERRIDES_APLLMCS_WLAN_SPARE_M \
                                0x00000700

#define GPRCM_APLLMCS_WLAN_OVERRIDES_APLLMCS_WLAN_SPARE_S 8
#define GPRCM_APLLMCS_WLAN_OVERRIDES_APLLMCS_WLAN_M_8_OVERRIDE_CTRL \
                                0x00000020  // Override control for
                                            // WLAN_APLLMCS_M[8]. When set to1,
                                            // M[8] will be selected by bit [3].
                                            // (Else controlled from WTOP)

#define GPRCM_APLLMCS_WLAN_OVERRIDES_APLLMCS_WLAN_M_8_OVERRIDE \
                                0x00000010  // Override for WLAN_APLLMCS_M[8].
                                            // Applicable only when bit [4] is
                                            // set to 1. (Else controlled from
                                            // WTOP)

#define GPRCM_APLLMCS_WLAN_OVERRIDES_APLLMCS_WLAN_N_7_8_OVERRIDE_CTRL \
                                0x00000004  // Override control for
                                            // WLAN_APLLMCS_N[8:7]. When set
                                            // to1, N[8:7] will be selected by
                                            // bits [2:1]. (Else controlled from
                                            // WTOP)

#define GPRCM_APLLMCS_WLAN_OVERRIDES_APLLMCS_WLAN_N_7_8_OVERRIDE_M \
                                0x00000003  // Override value for
                                            // WLAN_APLLMCS_N[8:7] bits.
                                            // Applicable only when bit [1] is
                                            // set to 1. (Else controlled from
                                            // WTOP)

#define GPRCM_APLLMCS_WLAN_OVERRIDES_APLLMCS_WLAN_N_7_8_OVERRIDE_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APLLMCS_MCU_RUN_CONFIG0_38P4 register.
//
//******************************************************************************
#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_38P4_APLLMCS_MCU_POSTDIV_M \
                                0x38000000

#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_38P4_APLLMCS_MCU_POSTDIV_S 27
#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_38P4_APLLMCS_MCU_SPARE_M \
                                0x07000000

#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_38P4_APLLMCS_MCU_SPARE_S 24
#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_38P4_APLLMCS_MCU_RUN_N_38P4_M \
                                0x007F0000  // Configuration for MCU-APLLMCS :
                                            // N during RUN mode. Selected if
                                            // the XTAL frequency is 38.4 MHz
                                            // (from Efuse)

#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_38P4_APLLMCS_MCU_RUN_N_38P4_S 16
#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_38P4_APLLMCS_MCU_RUN_M_38P4_M \
                                0x0000FF00  // Configuration for MCU-APLLMCS :
                                            // M during RUN mode. Selected if
                                            // the XTAL frequency is 38.4 MHz
                                            // (from Efuse)

#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_38P4_APLLMCS_MCU_RUN_M_38P4_S 8
#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_38P4_APLLMCS_MCU_RUN_M_8_38P4 \
                                0x00000010  // Configuration for MCU-APLLMCS :
                                            // M[8] during RUN mode. Selected if
                                            // the XTAL frequency is 38.4 MHz
                                            // (From Efuse)

#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_38P4_APLLMCS_MCU_RUN_N_7_8_38P4_M \
                                0x00000003  // Configuration for MCU-APLLMCS :
                                            // N[8:7] during RUN mode. Selected
                                            // if the XTAL frequency is 38.4 MHz
                                            // (From Efuse)

#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_38P4_APLLMCS_MCU_RUN_N_7_8_38P4_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APLLMCS_MCU_RUN_CONFIG1_38P4 register.
//
//******************************************************************************
#define GPRCM_APLLMCS_MCU_RUN_CONFIG1_38P4_APLLMCS_MCU_RUN_HISPEED_38P4 \
                                0x00000010  // Configuration for MCU-APLLMCS :
                                            // HISPEED during RUN mode. Selected
                                            // if the XTAL frequency is 38.4 MHz
                                            // (from Efuse)

#define GPRCM_APLLMCS_MCU_RUN_CONFIG1_38P4_APLLMCS_MCU_RUN_SEL96_38P4 \
                                0x00000008  // Configuration for MCU-APLLMCS :
                                            // SEL96 during RUN mode. Selected
                                            // if the XTAL frequency is 38.4 MHz
                                            // (from Efuse)

#define GPRCM_APLLMCS_MCU_RUN_CONFIG1_38P4_APLLMCS_MCU_RUN_SELINPFREQ_38P4_M \
                                0x00000007  // Configuration for MCU-APLLMCS :
                                            // SELINPFREQ during RUN mode.
                                            // Selected if the XTAL frequency is
                                            // 38.4 MHz (from Efuse)

#define GPRCM_APLLMCS_MCU_RUN_CONFIG1_38P4_APLLMCS_MCU_RUN_SELINPFREQ_38P4_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APLLMCS_MCU_RUN_CONFIG0_26 register.
//
//******************************************************************************
#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_26_APLLMCS_MCU_RUN_N_26_M \
                                0x007F0000  // Configuration for MCU-APLLMCS :
                                            // N during RUN mode. Selected if
                                            // the XTAL frequency is 26 MHz
                                            // (from Efuse)

#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_26_APLLMCS_MCU_RUN_N_26_S 16
#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_26_APLLMCS_MCU_RUN_M_26_M \
                                0x0000FF00  // Configuration for MCU-APLLMCS :
                                            // M during RUN mode. Selected if
                                            // the XTAL frequency is 26 MHz
                                            // (from Efuse)

#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_26_APLLMCS_MCU_RUN_M_26_S 8
#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_26_APLLMCS_MCU_RUN_M_8_26 \
                                0x00000010  // Configuration for MCU-APLLMCS :
                                            // M[8] during RUN mode. Selected if
                                            // the XTAL frequency is 26 MHz
                                            // (From Efuse)

#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_26_APLLMCS_MCU_RUN_N_7_8_26_M \
                                0x00000003  // Configuration for MCU-APLLMCS :
                                            // N[8:7] during RUN mode. Selected
                                            // if the XTAL frequency is 26 MHz
                                            // (From Efuse)

#define GPRCM_APLLMCS_MCU_RUN_CONFIG0_26_APLLMCS_MCU_RUN_N_7_8_26_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APLLMCS_MCU_RUN_CONFIG1_26 register.
//
//******************************************************************************
#define GPRCM_APLLMCS_MCU_RUN_CONFIG1_26_APLLMCS_MCU_RUN_HISPEED_26 \
                                0x00000010  // Configuration for MCU-APLLMCS :
                                            // HISPEED during RUN mode. Selected
                                            // if the XTAL frequency is 26 MHz
                                            // (from Efuse)

#define GPRCM_APLLMCS_MCU_RUN_CONFIG1_26_APLLMCS_MCU_RUN_SEL96_26 \
                                0x00000008  // Configuration for MCU-APLLMCS :
                                            // SEL96 during RUN mode. Selected
                                            // if the XTAL frequency is 26 MHz
                                            // (from Efuse)

#define GPRCM_APLLMCS_MCU_RUN_CONFIG1_26_APLLMCS_MCU_RUN_SELINPFREQ_26_M \
                                0x00000007  // Configuration for MCU-APLLMCS :
                                            // SELINPFREQ during RUN mode.
                                            // Selected if the XTAL frequency is
                                            // 26 MHz (from Efuse)

#define GPRCM_APLLMCS_MCU_RUN_CONFIG1_26_APLLMCS_MCU_RUN_SELINPFREQ_26_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the GPRCM_O_SPARE_RW0 register.
//
//******************************************************************************
//******************************************************************************
//
// The following are defines for the bit fields in the GPRCM_O_SPARE_RW1 register.
//
//******************************************************************************
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APLLMCS_MCU_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_APLLMCS_MCU_OVERRIDES_APLLMCS_MCU_LOCK \
                                0x00000400  // 1 - APLLMCS_MCU is locked ; 0 -
                                            // APLLMCS_MCU is not locked

#define GPRCM_APLLMCS_MCU_OVERRIDES_APLLMCS_MCU_ENABLE_OVERRIDE \
                                0x00000200  // Override for APLLMCS_MCU Enable.
                                            // Applicable if bit [8] is set

#define GPRCM_APLLMCS_MCU_OVERRIDES_APLLMCS_MCU_ENABLE_OVERRIDE_CTRL \
                                0x00000100  // 1 - Enable for APLLMCS_MCU comes
                                            // from bit [9]. 0 - Enable for
                                            // APLLMCS_MCU comes from FSM.

#define GPRCM_APLLMCS_MCU_OVERRIDES_SYSCLK_SRC_OVERRIDE_M \
                                0x00000006  // Override for sysclk src
                                            // (applicable only if bit [0] is
                                            // set to 1. "00"- SLOW_CLK "01"-
                                            // XTAL_CLK "10"- PLL_CLK

#define GPRCM_APLLMCS_MCU_OVERRIDES_SYSCLK_SRC_OVERRIDE_S 1
#define GPRCM_APLLMCS_MCU_OVERRIDES_SYSCLK_SRC_OVERRIDE_CTRL \
                                0x00000001  // 1 - Sysclk src is selected from
                                            // bits [2:1] of this register. 0 -
                                            // Sysclk src is selected from FSM

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_SYSCLK_SWITCH_STATUS register.
//
//******************************************************************************
#define GPRCM_SYSCLK_SWITCH_STATUS_SYSCLK_SWITCH_STATUS \
                                0x00000001  // 1 - Sysclk switching is
                                            // complete. 0 - Sysclk switching is
                                            // in progress.

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_REF_LDO_CONTROLS register.
//
//******************************************************************************
#define GPRCM_REF_LDO_CONTROLS_REF_LDO_ENABLE_OVERRIDE_CTRL \
                                0x00010000  // 1 - Enable for REF_LDO comes
                                            // from bit [0] of this register ; 0
                                            // - Enable for REF_LDO comes from
                                            // the FSM. Note : Final REF_LDO_EN
                                            // reaches on the port
                                            // TOP_PM_REG2[0] of gprcm.

#define GPRCM_REF_LDO_CONTROLS_REF_SPARE_CONTROL_M \
                                0x0000C000  // Spare bits for REF_CTRL_FSM.
                                            // Reaches directly on port
                                            // TOP_PM_REG2[15:14] of gprcm.

#define GPRCM_REF_LDO_CONTROLS_REF_SPARE_CONTROL_S 14
#define GPRCM_REF_LDO_CONTROLS_REF_TLOAD_ENABLE_M \
                                0x00003800  // REF TLOAD Enable. Reaches
                                            // directly on port
                                            // TOP_PM_REG2[13:11] of gprcm.

#define GPRCM_REF_LDO_CONTROLS_REF_TLOAD_ENABLE_S 11
#define GPRCM_REF_LDO_CONTROLS_REF_LDO_TMUX_CONTROL_M \
                                0x00000700  // REF_LDO Test-mux control.
                                            // Reaches directly on port
                                            // TOP_PM_REG2[10:8] of gprcm.

#define GPRCM_REF_LDO_CONTROLS_REF_LDO_TMUX_CONTROL_S 8
#define GPRCM_REF_LDO_CONTROLS_REF_BW_CONTROL_M \
                                0x000000C0  // REF BW Control. Reaches directly
                                            // on port TOP_PM_REG2[7:6] of
                                            // gprcm.

#define GPRCM_REF_LDO_CONTROLS_REF_BW_CONTROL_S 6
#define GPRCM_REF_LDO_CONTROLS_REF_VTRIM_CONTROL_M \
                                0x0000003C  // REF VTRIM Control. Reaches
                                            // directly on port TOP_PM_REG2[5:2]
                                            // of gprcm.

#define GPRCM_REF_LDO_CONTROLS_REF_VTRIM_CONTROL_S 2
#define GPRCM_REF_LDO_CONTROLS_REF_LDO_BYPASS_ENABLE \
                                0x00000002  // REF LDO Bypass Enable. Reaches
                                            // directly on port TOP_PM_REG2[1]
                                            // of gprcm.

#define GPRCM_REF_LDO_CONTROLS_REF_LDO_ENABLE \
                                0x00000001  // Override for REF_LDO Enable.
                                            // Applicable only if bit [16] of
                                            // this register is set. Note :
                                            // Final REF_LDO_EN reaches on the
                                            // port TOP_PM_REG2[0] of gprcm.

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_REF_RTRIM_CONTROL register.
//
//******************************************************************************
#define GPRCM_REF_RTRIM_CONTROL_TOP_PM_REG0_5_4_M \
                                0x18000000  // This is [5:4] bits of
                                            // TOP_PM_REG0

#define GPRCM_REF_RTRIM_CONTROL_TOP_PM_REG0_5_4_S 27
#define GPRCM_REF_RTRIM_CONTROL_TOP_CLKM_REG0_15_5_M \
                                0x07FF0000  // This is [15:5] bits of
                                            // TOP_CLKM_REG0

#define GPRCM_REF_RTRIM_CONTROL_TOP_CLKM_REG0_15_5_S 16
#define GPRCM_REF_RTRIM_CONTROL_REF_CLKM_RTRIM_OVERRIDE_CTRL \
                                0x00000100  // 1 - CLKM_RTRIM comes for
                                            // bits[4:0] of this register. 0 -
                                            // CLKM_RTRIM comes from Efuse
                                            // (after efuse_done = 1).

#define GPRCM_REF_RTRIM_CONTROL_REF_CLKM_RTRIM_M \
                                0x0000001F  // CLKM_TRIM Override. Applicable
                                            // when efuse_done = 0 or bit[8] is
                                            // set to 1.

#define GPRCM_REF_RTRIM_CONTROL_REF_CLKM_RTRIM_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_REF_SLICER_CONTROLS0 register.
//
//******************************************************************************
#define GPRCM_REF_SLICER_CONTROLS0_CLK_EN_WLAN_LOWV_OVERRIDE_CTRL \
                                0x00200000  // 1 - EN_DIG_BUF_TOP comes from
                                            // bit [14] of this register. 0 -
                                            // EN_DIG_BUF_TOP comes from the
                                            // FSM. Note : Final EN_DIG_BUF_WLAN
                                            // reaches on TOP_CLKM_REG1_IN[14]
                                            // port of gprcm

#define GPRCM_REF_SLICER_CONTROLS0_CLK_EN_TOP_LOWV_OVERRIDE_CTRL \
                                0x00100000  // 1 - EN_DIG_BUF_TOP comes from
                                            // bit [15] of this register. 0 -
                                            // EN_DIG_BUF_TOP comes from the
                                            // FSM. Note : Final EN_DIG_BUF_TOP
                                            // reaches on TOP_CLKM_REG1_IN[15]
                                            // port of gprcm

#define GPRCM_REF_SLICER_CONTROLS0_EN_XTAL_OVERRIDE_CTRL \
                                0x00080000  // 1 - EN_XTAL comes from bit [3]
                                            // of this register. 0 - EN_XTAL
                                            // comes from FSM. Note : Final
                                            // XTAL_EN reaches on
                                            // TOP_CLKM_REG1_IN[3] of gprcm.

#define GPRCM_REF_SLICER_CONTROLS0_EN_SLI_HV_OVERRIDE_CTRL \
                                0x00040000  // 1 - Enable HV Slicer comes from
                                            // bit [2] of this register. 0 -
                                            // Enable HV Slicer comes from FSM.
                                            // Note : Final HV_SLICER_EN reaches
                                            // on port TOP_CLKM_REG1_IN[1] of
                                            // gprcm.

#define GPRCM_REF_SLICER_CONTROLS0_EN_SLI_LV_OVERRIDE_CTRL \
                                0x00020000  // 1 - Enable LV Slicer comes from
                                            // bit[1] of this register. 0 -
                                            // Enable LV Slicer comes from FSM.
                                            // Note : final LV_SLICER_EN reaches
                                            // on port TOP_CLKM_REG1_IN[2] of
                                            // gprcm.

#define GPRCM_REF_SLICER_CONTROLS0_EN_SLI_HV_PDN_OVERRIDE_CTRL \
                                0x00010000  // 1 - Enable HV Pull-down comes
                                            // from bit[0] of this register. 0 -
                                            // Enable HV Pull-down comes from
                                            // FSM. Note : Final HV_PULL_DOWN
                                            // reaches on port
                                            // TOP_CLKM_REG1_IN[0] of gprcm.

#define GPRCM_REF_SLICER_CONTROLS0_CLK_EN_TOP_LOWV \
                                0x00008000  // Override for EN_DIG_BUF_TOP.
                                            // Applicable if bit[20] is set to
                                            // 1. Note : Final EN_DIG_BUF_TOP
                                            // reaches on TOP_CLKM_REG1_IN[15]
                                            // port of gprcm

#define GPRCM_REF_SLICER_CONTROLS0_CLK_EN_WLAN_LOWV \
                                0x00004000  // Override for EN_DIG_BUF_WLAN.
                                            // Applicable if bit[19] is set to
                                            // 1. Note : Final EN_DIG_BUF_WLAN
                                            // reaches on TOP_CLKM_REG1_IN[14]
                                            // port of gprcm

#define GPRCM_REF_SLICER_CONTROLS0_CLKOUT_FLIP_EN \
                                0x00002000  // CLKOUT Flip Enable. Reaches on
                                            // bit[13] of TOP_CLKM_REG1_IN[13]
                                            // port of gprcm.

#define GPRCM_REF_SLICER_CONTROLS0_EN_DIV2_WLAN_CLK \
                                0x00001000  // Enable divide2 in WLAN Clk-path.
                                            // Reaches on TOP_CLKM_REG1_IN[12]
                                            // port of gprcm

#define GPRCM_REF_SLICER_CONTROLS0_EN_DIV3_WLAN_CLK \
                                0x00000800  // Enable divide3 in WLAN Clk-path.
                                            // Reaches on TOP_CLKM_REG1_IN[11]
                                            // port of gprcm

#define GPRCM_REF_SLICER_CONTROLS0_EN_DIV4_WLAN_CLK \
                                0x00000400  // Enable divide4 in WLAN Clk-path.
                                            // Reaches on TOP_CLKM_REG1_IN[10]
                                            // port of gprcm

#define GPRCM_REF_SLICER_CONTROLS0_CM_TMUX_SEL_LOWV_M \
                                0x000003C0  // CM Test-mux select. Reaches on
                                            // TOP_CLMM_REG1_IN[9:6] port of
                                            // gprcm

#define GPRCM_REF_SLICER_CONTROLS0_CM_TMUX_SEL_LOWV_S 6
#define GPRCM_REF_SLICER_CONTROLS0_SLICER_SPARE0_M \
                                0x00000030  // Slicer spare0 control. Reaches
                                            // on TOP_CLKM_REG1_IN[5:4] port of
                                            // gprcm

#define GPRCM_REF_SLICER_CONTROLS0_SLICER_SPARE0_S 4
#define GPRCM_REF_SLICER_CONTROLS0_EN_XTAL \
                                0x00000008  // Enable XTAL override. Reaches on
                                            // TOP_CLKM_REG1_IN[3] port of gprcm

#define GPRCM_REF_SLICER_CONTROLS0_EN_SLICER_HV \
                                0x00000004  // Enable HV Slicer override.
                                            // Reaches on TOP_CLKM_REG1_IN[1]
                                            // port of gprcm

#define GPRCM_REF_SLICER_CONTROLS0_EN_SLICER_LV \
                                0x00000002  // Enable LV Slicer override.
                                            // Reaches on TOP_CLKM_REG1_IN[2]
                                            // port of gprcm

#define GPRCM_REF_SLICER_CONTROLS0_EN_SLICER_HV_PDN \
                                0x00000001  // Enable HV Pull-down override.
                                            // Reaches on TOP_CLKM_REG1_IN[0]
                                            // port of gprcm

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_REF_SLICER_CONTROLS1 register.
//
//******************************************************************************
#define GPRCM_REF_SLICER_CONTROLS1_SLICER_SPARE1_M \
                                0x0000FC00  // Slicer spare1. Reaches on port
                                            // TOP_CLKM_REG2_IN[15:10] of gprcm.

#define GPRCM_REF_SLICER_CONTROLS1_SLICER_SPARE1_S 10
#define GPRCM_REF_SLICER_CONTROLS1_XOSC_TRIM_M \
                                0x000003F0  // XOSC Trim. Reaches on port
                                            // TOP_CLKM_REG2_IN[9:4] of gprcm

#define GPRCM_REF_SLICER_CONTROLS1_XOSC_TRIM_S 4
#define GPRCM_REF_SLICER_CONTROLS1_SLICER_ITRIM_CHANGE_TOGGLE \
                                0x00000008  // Slicer ITRIM Toggle. Reaches on
                                            // port TOP_CLKM_REG2_IN[3] of
                                            // gprcm.

#define GPRCM_REF_SLICER_CONTROLS1_SLICER_LV_TRIM_M \
                                0x00000007  // LV Slicer trim. Reaches on port
                                            // TOP_CLKM_REG2_IN[2:0] of gprcm.

#define GPRCM_REF_SLICER_CONTROLS1_SLICER_LV_TRIM_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_REF_ANA_BGAP_CONTROLS0 register.
//
//******************************************************************************
#define GPRCM_REF_ANA_BGAP_CONTROLS0_reserved_M \
                                0xFF800000

#define GPRCM_REF_ANA_BGAP_CONTROLS0_reserved_S 23
#define GPRCM_REF_ANA_BGAP_CONTROLS0_mem_ref_mag_trim_override_ctrl \
                                0x00400000  // 1 - REF_MAG_TRIM comes from
                                            // bit[4:0] of register
                                            // REF_ANA_BGAP_CONTROLS1 [Addr :
                                            // 0x0850]; 0 - REF_MAG_TRIM comes
                                            // from efuse (After efc_done = 1).
                                            // Note : Final REF_MAG_TRIM reaches
                                            // on port TOP_PM_REG1[4:0] of gprcm

#define GPRCM_REF_ANA_BGAP_CONTROLS0_mem_ref_v2i_trim_override_ctrl \
                                0x00200000  // 1 - REF_V2I_TRIM comes from
                                            // bit[9:6] of this register ; 0 -
                                            // REF_V2I_TRIM comes from efuse
                                            // (After efc_done = 1). Note :
                                            // Final REF_V2I_TRIM reaches on
                                            // port TOP_PM_REG0[9:6] of gprcm.

#define GPRCM_REF_ANA_BGAP_CONTROLS0_mem_ref_temp_trim_override_ctrl \
                                0x00100000  // 1 - REF_TEMP_TRIM comes from
                                            // bit[15:10] of this register ; 0 -
                                            // REF_TEMP_TRIM comes from efuse
                                            // (After efc_done = 1). Note :
                                            // Final REF_TEMP_TRIM reaches on
                                            // port TOP_PM_REG0[15:10] of gprcm.

#define GPRCM_REF_ANA_BGAP_CONTROLS0_mem_ref_startup_en_override_ctrl \
                                0x00080000  // 1 - REF_STARTUP_EN comes from
                                            // bit [3] of this register ; 0 -
                                            // REF_STARTUP_EN comes from FSM.
                                            // Note : Final REF_STARTUP_EN
                                            // reaches on port TOP_PM_REG0[3] of
                                            // gprcm

#define GPRCM_REF_ANA_BGAP_CONTROLS0_mem_ref_v2i_en_override_ctrl \
                                0x00040000  // 1 - REF_V2I_EN comes from bit
                                            // [2] of this register ; 0 -
                                            // REF_V2I_EN comes from FSM. Note :
                                            // Final REF_V2I_EN reaches on port
                                            // TOP_PM_REG0[2] of gprcm.

#define GPRCM_REF_ANA_BGAP_CONTROLS0_mem_ref_fc_en_override_ctrl \
                                0x00020000  // 1 - REF_FC_EN comes from bit [1]
                                            // of this register ; 0 - REF_FC_EN
                                            // comes from FSM. Note : Final
                                            // REF_FC_EN reaches on port
                                            // TOP_PM_REG0[1] of gprcm.

#define GPRCM_REF_ANA_BGAP_CONTROLS0_mem_ref_bgap_en_override_ctrl \
                                0x00010000  // 1 - REF_BGAP_EN comes from bit
                                            // [0] of this register ; 0 -
                                            // REF_BGAP_EN comes from FSM. Note
                                            // : Final REF_BGAP_EN reaches on
                                            // port TOP_PM_REG0[0] of gprcm.

#define GPRCM_REF_ANA_BGAP_CONTROLS0_mem_ref_temp_trim_M \
                                0x0000FC00  // REF_TEMP_TRIM override.
                                            // Applicable when bit [20] of this
                                            // register set to 1. (or efc_done =
                                            // 0) Note : Final REF_TEMP_TRIM
                                            // reaches on port
                                            // TOP_PM_REG0[15:10] of gprcm.

#define GPRCM_REF_ANA_BGAP_CONTROLS0_mem_ref_temp_trim_S 10
#define GPRCM_REF_ANA_BGAP_CONTROLS0_mem_ref_v2i_trim_M \
                                0x000003C0  // REF_V2I_TRIM Override.
                                            // Applicable when bit [21] of this
                                            // register set to 1 . (of efc_done
                                            // = 0) Note : Final REF_V2I_TRIM
                                            // reaches on port TOP_PM_REG0[9:6]
                                            // of gprcm.

#define GPRCM_REF_ANA_BGAP_CONTROLS0_mem_ref_v2i_trim_S 6
#define GPRCM_REF_ANA_BGAP_CONTROLS0_NU1_M \
                                0x00000030

#define GPRCM_REF_ANA_BGAP_CONTROLS0_NU1_S 4
#define GPRCM_REF_ANA_BGAP_CONTROLS0_mem_ref_startup_en \
                                0x00000008  // REF_STARTUP_EN override.
                                            // Applicable when bit [19] of this
                                            // register is set to 1. Note :
                                            // Final REF_STARTUP_EN reaches on
                                            // port TOP_PM_REG0[3] of gprcm

#define GPRCM_REF_ANA_BGAP_CONTROLS0_mem_ref_v2i_en \
                                0x00000004  // REF_V2I_EN override. Applicable
                                            // when bit [21] of this register is
                                            // set to 1. Note : Final REF_V2I_EN
                                            // reaches on port TOP_PM_REG0[2] of
                                            // gprcm.

#define GPRCM_REF_ANA_BGAP_CONTROLS0_mem_ref_fc_en \
                                0x00000002  // REF_FC_EN override. Applicable
                                            // when bit [17] of this register is
                                            // set to 1. Note : Final REF_FC_EN
                                            // reaches on port TOP_PM_REG0[1] of
                                            // gprcm.

#define GPRCM_REF_ANA_BGAP_CONTROLS0_mem_ref_bgap_en \
                                0x00000001  // REF_BGAP_EN override. Applicable
                                            // when bit [16] of this register
                                            // set to 1. Note : Final
                                            // REF_BGAP_EN reaches on port
                                            // TOP_PM_REG0[0] of gprcm.

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_REF_ANA_BGAP_CONTROLS1 register.
//
//******************************************************************************
#define GPRCM_REF_ANA_BGAP_CONTROLS1_reserved_M \
                                0xFFFF0000

#define GPRCM_REF_ANA_BGAP_CONTROLS1_reserved_S 16
#define GPRCM_REF_ANA_BGAP_CONTROLS1_mem_ref_bg_spare_M \
                                0x0000C000  // REF_BGAP_SPARE. Reaches on port
                                            // TOP_PM_REG1[15:14] of gprcm.

#define GPRCM_REF_ANA_BGAP_CONTROLS1_mem_ref_bg_spare_S 14
#define GPRCM_REF_ANA_BGAP_CONTROLS1_mem_ref_bgap_tmux_ctrl_M \
                                0x00003E00  // REF_BGAP_TMUX_CTRL. Reaches on
                                            // port TOP_PM_REG1[13:9] of gprcm.

#define GPRCM_REF_ANA_BGAP_CONTROLS1_mem_ref_bgap_tmux_ctrl_S 9
#define GPRCM_REF_ANA_BGAP_CONTROLS1_mem_ref_filt_trim_M \
                                0x000001E0  // REF_FILT_TRIM. Reaches on port
                                            // TOP_PM_REG1[8:5] of gprcm.

#define GPRCM_REF_ANA_BGAP_CONTROLS1_mem_ref_filt_trim_S 5
#define GPRCM_REF_ANA_BGAP_CONTROLS1_mem_ref_mag_trim_M \
                                0x0000001F  // REF_MAG_TRIM Override.
                                            // Applicable when bit[22] of
                                            // REF_ANA_BGAP_CONTROLS0 [0x084C]
                                            // set to 1 (of efc_done = 0). Note
                                            // : Final REF_MAG_TRIM reaches on
                                            // port TOP_PM_REG1[4:0] of gprcm

#define GPRCM_REF_ANA_BGAP_CONTROLS1_mem_ref_mag_trim_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_REF_ANA_SPARE_CONTROLS0 register.
//
//******************************************************************************
#define GPRCM_REF_ANA_SPARE_CONTROLS0_reserved_M \
                                0xFFFF0000

#define GPRCM_REF_ANA_SPARE_CONTROLS0_reserved_S 16
#define GPRCM_REF_ANA_SPARE_CONTROLS0_mem_top_pm_reg3_M \
                                0x0000FFFF  // Spare control. Reaches on
                                            // TOP_PM_REG3 [15:0] of gprcm.

#define GPRCM_REF_ANA_SPARE_CONTROLS0_mem_top_pm_reg3_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_REF_ANA_SPARE_CONTROLS1 register.
//
//******************************************************************************
#define GPRCM_REF_ANA_SPARE_CONTROLS1_mem_top_clkm_reg3_M \
                                0xFFFF0000  // Spare control. Reaches on
                                            // TOP_CLKM_REG3 [15:0] of gprcm.

#define GPRCM_REF_ANA_SPARE_CONTROLS1_mem_top_clkm_reg3_S 16
#define GPRCM_REF_ANA_SPARE_CONTROLS1_mem_top_clkm_reg4_M \
                                0x0000FFFF  // Spare control. Reaches on
                                            // TOP_CLKM_REG4 [15:0] of gprcm.

#define GPRCM_REF_ANA_SPARE_CONTROLS1_mem_top_clkm_reg4_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MEMSS_PSCON_OVERRIDES0 register.
//
//******************************************************************************
#define GPRCM_MEMSS_PSCON_OVERRIDES0_mem_memss_pscon_mem_off_override_M \
                                0xFFFF0000

#define GPRCM_MEMSS_PSCON_OVERRIDES0_mem_memss_pscon_mem_off_override_S 16
#define GPRCM_MEMSS_PSCON_OVERRIDES0_mem_memss_pscon_mem_retain_override_M \
                                0x0000FFFF

#define GPRCM_MEMSS_PSCON_OVERRIDES0_mem_memss_pscon_mem_retain_override_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MEMSS_PSCON_OVERRIDES1 register.
//
//******************************************************************************
#define GPRCM_MEMSS_PSCON_OVERRIDES1_reserved_M \
                                0xFFFFFFC0

#define GPRCM_MEMSS_PSCON_OVERRIDES1_reserved_S 6
#define GPRCM_MEMSS_PSCON_OVERRIDES1_mem_memss_pscon_mem_update_override_ctrl \
                                0x00000020

#define GPRCM_MEMSS_PSCON_OVERRIDES1_mem_memss_pscon_mem_update_override \
                                0x00000010

#define GPRCM_MEMSS_PSCON_OVERRIDES1_mem_memss_pscon_sleep_override_ctrl \
                                0x00000008

#define GPRCM_MEMSS_PSCON_OVERRIDES1_mem_memss_pscon_sleep_override \
                                0x00000004

#define GPRCM_MEMSS_PSCON_OVERRIDES1_mem_memss_pscon_mem_off_override_ctrl \
                                0x00000002

#define GPRCM_MEMSS_PSCON_OVERRIDES1_mem_memms_pscon_mem_retain_override_ctrl \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_PLL_REF_LOCK_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_PLL_REF_LOCK_OVERRIDES_reserved_M \
                                0xFFFFFFF8

#define GPRCM_PLL_REF_LOCK_OVERRIDES_reserved_S 3
#define GPRCM_PLL_REF_LOCK_OVERRIDES_mem_mcu_apllmcs_lock_override \
                                0x00000004

#define GPRCM_PLL_REF_LOCK_OVERRIDES_mem_wlan_apllmcs_lock_override \
                                0x00000002

#define GPRCM_PLL_REF_LOCK_OVERRIDES_mem_ref_clk_valid_override \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MCU_PSCON_DEBUG register.
//
//******************************************************************************
#define GPRCM_MCU_PSCON_DEBUG_reserved_M \
                                0xFFFFFFC0

#define GPRCM_MCU_PSCON_DEBUG_reserved_S 6
#define GPRCM_MCU_PSCON_DEBUG_mcu_pscon_rtc_ps_M \
                                0x00000038  // MCU_PSCON_RTC_ON = "0000";
                                            // MCU_PSCON_RTC_OFF = "0001";
                                            // MCU_PSCON_RTC_RET = "0010";
                                            // MCU_PSCON_RTC_OFF_TO_ON = "0011";
                                            // MCU_PSCON_RTC_RET_TO_ON = "0100";
                                            // MCU_PSCON_RTC_ON_TO_RET = "0101";
                                            // MCU_PSCON_RTC_ON_TO_OFF = "0110";
                                            // MCU_PSCON_RTC_RET_TO_ON_WAIT_OPP
                                            // = "0111";
                                            // MCU_PSCON_RTC_OFF_TO_ON_WAIT_OPP
                                            // = "1000";

#define GPRCM_MCU_PSCON_DEBUG_mcu_pscon_rtc_ps_S 3
#define GPRCM_MCU_PSCON_DEBUG_mcu_pscon_sys_ps_M \
                                0x00000007

#define GPRCM_MCU_PSCON_DEBUG_mcu_pscon_sys_ps_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MEMSS_PWR_PS register.
//
//******************************************************************************
#define GPRCM_MEMSS_PWR_PS_reserved_M \
                                0xFFFFFFF8

#define GPRCM_MEMSS_PWR_PS_reserved_S 3
#define GPRCM_MEMSS_PWR_PS_pwr_ps_memss_M \
                                0x00000007  // MEMSS_PM_SLEEP = "000";
                                            // MEMSS_PM_WAIT_OPP = "010";
                                            // MEMSS_PM_ACTIVE = "011";
                                            // MEMSS_PM_SLEEP_TO_ACTIVE = "100";
                                            // MEMSS_PM_ACTIVE_TO_SLEEP = "101";

#define GPRCM_MEMSS_PWR_PS_pwr_ps_memss_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_REF_FSM_DEBUG register.
//
//******************************************************************************
#define GPRCM_REF_FSM_DEBUG_reserved_M \
                                0xFFFFFFC0

#define GPRCM_REF_FSM_DEBUG_reserved_S 6
#define GPRCM_REF_FSM_DEBUG_fref_mode_M \
                                0x00000030  // 01 - HV Mode ; 10 - LV Mode ; 11
                                            // - XTAL Mode

#define GPRCM_REF_FSM_DEBUG_fref_mode_S 4
#define GPRCM_REF_FSM_DEBUG_ref_fsm_ps_M \
                                0x0000000F  // constant FREF_CLK_OFF = "00000";
                                            // constant FREF_EN_BGAP = "00001";
                                            // constant FREF_EN_LDO = "00010";
                                            // constant FREF_EN_SLI_HV =
                                            // "00011"; constant
                                            // FREF_EN_SLI_HV_PD = "00100";
                                            // constant FREF_EN_DIG_BUF =
                                            // "00101"; constant FREF_EN_OSC =
                                            // "00110"; constant FREF_EN_SLI_LV
                                            // = "00111"; constant
                                            // FREF_EN_CLK_REQ = "01000";
                                            // constant FREF_CLK_VALID =
                                            // "01001"; constant FREF_MODE_DET0
                                            // = "01010"; constant
                                            // FREF_MODE_DET1 = "01011";
                                            // constant FREF_MODE_DET2 =
                                            // "10010"; constant FREF_MODE_DET3
                                            // = "10011"; constant FREF_VALID =
                                            // "01100"; constant FREF_VALID0 =
                                            // "01101"; constant FREF_VALID1 =
                                            // "01110"; constant FREF_VALID2 =
                                            // "01111"; constant
                                            // FREF_WAIT_EXT_TCXO0 = "10000";
                                            // constant FREF_WAIT_EXT_TCXO1 =
                                            // "10001";

#define GPRCM_REF_FSM_DEBUG_ref_fsm_ps_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MEM_SYS_OPP_REQ_OVERRIDE register.
//
//******************************************************************************
#define GPRCM_MEM_SYS_OPP_REQ_OVERRIDE_reserved_M \
                                0xFFFFFFE0

#define GPRCM_MEM_SYS_OPP_REQ_OVERRIDE_reserved_S 5
#define GPRCM_MEM_SYS_OPP_REQ_OVERRIDE_mem_sys_opp_req_override_ctrl \
                                0x00000010  // 1 - Override the sytem-opp
                                            // request to ANATOP using bit0 of
                                            // this register

#define GPRCM_MEM_SYS_OPP_REQ_OVERRIDE_mem_sys_opp_req_override_M \
                                0x0000000F  // "0001" - RUN ; "0010" - DSLP ;
                                            // "0100" - LPDS ; Others - NA

#define GPRCM_MEM_SYS_OPP_REQ_OVERRIDE_mem_sys_opp_req_override_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MEM_TESTCTRL_PD_OPP_CONFIG register.
//
//******************************************************************************
#define GPRCM_MEM_TESTCTRL_PD_OPP_CONFIG_reserved_M \
                                0xFFFFFFFE

#define GPRCM_MEM_TESTCTRL_PD_OPP_CONFIG_reserved_S 1
#define GPRCM_MEM_TESTCTRL_PD_OPP_CONFIG_mem_sleep_opp_enter_with_testpd_on \
                                0x00000001  // 1 - Enable sleep-opp (DSLP/LPDS)
                                            // entry even if Test-Pd is kept ON
                                            // ; 0 - Donot enable sleep-opp
                                            // (DSLP/LPDS) entry with Test-Pd
                                            // ON.

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MEM_WL_FAST_CLK_REQ_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_MEM_WL_FAST_CLK_REQ_OVERRIDES_reserved_M \
                                0xFFFFFFF8

#define GPRCM_MEM_WL_FAST_CLK_REQ_OVERRIDES_reserved_S 3
#define GPRCM_MEM_WL_FAST_CLK_REQ_OVERRIDES_mem_wl_fast_clk_req_override_ctrl \
                                0x00000004  // NA

#define GPRCM_MEM_WL_FAST_CLK_REQ_OVERRIDES_mem_wl_fast_clk_req_override \
                                0x00000002  // NA

#define GPRCM_MEM_WL_FAST_CLK_REQ_OVERRIDES_mem_wl_sleep_with_clk_req_override \
                                0x00000001  // NA

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MEM_MCU_PD_MODE_REQ_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_MEM_MCU_PD_MODE_REQ_OVERRIDES_mem_mcu_pd_mode_req_override_ctrl \
                                0x00000004  // 1 - Override the MCU-PD power
                                            // modes using bits [1] & [0] ;

#define GPRCM_MEM_MCU_PD_MODE_REQ_OVERRIDES_mem_mcu_pd_pwrdn_req_override \
                                0x00000002  // 1 - Request for power-down of
                                            // MCU-PD ;

#define GPRCM_MEM_MCU_PD_MODE_REQ_OVERRIDES_mem_mcu_pd_ret_req_override \
                                0x00000001  // 1 - Request for retention mode
                                            // of MCU-PD.

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MEM_MCSPI_SRAM_OFF_REQ_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_MEM_MCSPI_SRAM_OFF_REQ_OVERRIDES_mem_mcspi_sram_off_req_override_ctrl \
                                0x00000002  // 1- Override the MCSPI
                                            // (Autonomous SPI) memory state
                                            // using bit [0]

#define GPRCM_MEM_MCSPI_SRAM_OFF_REQ_OVERRIDES_mem_mcspi_sram_off_req_override \
                                0x00000001  // 1 - Request for power-down of
                                            // Autonomous SPI 8k memory ; 0 -
                                            // Donot request power-down of
                                            // Autonomous SPI 8k Memory

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MEM_WLAN_APLLMCS_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_MEM_WLAN_APLLMCS_OVERRIDES_wlan_apllmcs_lock \
                                0x00000100

#define GPRCM_MEM_WLAN_APLLMCS_OVERRIDES_mem_wlan_apllmcs_enable_override \
                                0x00000002

#define GPRCM_MEM_WLAN_APLLMCS_OVERRIDES_mem_wlan_apllmcs_enable_override_ctrl \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MEM_REF_FSM_CFG2 register.
//
//******************************************************************************
#define GPRCM_MEM_REF_FSM_CFG2_MEM_FC_DEASSERT_DELAY_M \
                                0x00380000  // Number of RTC clocks for keeping
                                            // the FC_EN asserted high

#define GPRCM_MEM_REF_FSM_CFG2_MEM_FC_DEASSERT_DELAY_S 19
#define GPRCM_MEM_REF_FSM_CFG2_MEM_STARTUP_DEASSERT_DELAY_M \
                                0x00070000  // Number of RTC clocks for keeping
                                            // the STARTUP_EN asserted high

#define GPRCM_MEM_REF_FSM_CFG2_MEM_STARTUP_DEASSERT_DELAY_S 16
#define GPRCM_MEM_REF_FSM_CFG2_MEM_EXT_TCXO_SETTLING_TIME_M \
                                0x0000FFFF  // Number of RTC clocks for waiting
                                            // for clock to settle.

#define GPRCM_MEM_REF_FSM_CFG2_MEM_EXT_TCXO_SETTLING_TIME_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_TESTCTRL_POWER_CTRL register.
//
//******************************************************************************
#define GPRCM_TESTCTRL_POWER_CTRL_TESTCTRL_PD_STATUS_M \
                                0x00000006

#define GPRCM_TESTCTRL_POWER_CTRL_TESTCTRL_PD_STATUS_S 1
#define GPRCM_TESTCTRL_POWER_CTRL_TESTCTRL_PD_ENABLE \
                                0x00000001  // 0 - Disable the TestCtrl-pd ; 1
                                            // - Enable the TestCtrl-pd.

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_SSDIO_POWER_CTRL register.
//
//******************************************************************************
#define GPRCM_SSDIO_POWER_CTRL_SSDIO_PD_STATUS_M \
                                0x00000006  // 1 - SSDIO-PD is ON ; 0 -
                                            // SSDIO-PD is OFF

#define GPRCM_SSDIO_POWER_CTRL_SSDIO_PD_STATUS_S 1
#define GPRCM_SSDIO_POWER_CTRL_SSDIO_PD_ENABLE \
                                0x00000001  // 0 - Disable the SSDIO-pd ; 1 -
                                            // Enable the SSDIO-pd.

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MCSPI_N1_POWER_CTRL register.
//
//******************************************************************************
#define GPRCM_MCSPI_N1_POWER_CTRL_MCSPI_N1_PD_STATUS_M \
                                0x00000006  // 1 - MCSPI_N1-PD is ON ; 0 -
                                            // MCSPI_N1-PD if OFF

#define GPRCM_MCSPI_N1_POWER_CTRL_MCSPI_N1_PD_STATUS_S 1
#define GPRCM_MCSPI_N1_POWER_CTRL_MCSPI_N1_PD_ENABLE \
                                0x00000001  // 0 - Disable the MCSPI_N1-pd ; 1
                                            // - Enable the MCSPI_N1-pd.

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_WELP_POWER_CTRL register.
//
//******************************************************************************
#define GPRCM_WELP_POWER_CTRL_WTOP_PD_STATUS_M \
                                0x00001C00

#define GPRCM_WELP_POWER_CTRL_WTOP_PD_STATUS_S 10
#define GPRCM_WELP_POWER_CTRL_WTOP_PD_REQ_OVERRIDE \
                                0x00000200

#define GPRCM_WELP_POWER_CTRL_WTOP_PD_REQ_OVERRIDE_CTRL \
                                0x00000100

#define GPRCM_WELP_POWER_CTRL_WELP_PD_STATUS_M \
                                0x00000006

#define GPRCM_WELP_POWER_CTRL_WELP_PD_STATUS_S 1
#define GPRCM_WELP_POWER_CTRL_WELP_PD_ENABLE \
                                0x00000001  // 0 - Disable the WELP-pd ; 1 -
                                            // Enable the WELP-pd.

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_WL_SDIO_POWER_CTRL register.
//
//******************************************************************************
#define GPRCM_WL_SDIO_POWER_CTRL_WL_SDIO_PD_STATUS_M \
                                0x00000006

#define GPRCM_WL_SDIO_POWER_CTRL_WL_SDIO_PD_STATUS_S 1
#define GPRCM_WL_SDIO_POWER_CTRL_WL_SDIO_PD_ENABLE \
                                0x00000001  // 0 - Disable the WL_SDIO-pd ; 1 -
                                            // Enable the WL_SDIO-pd.

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_WLAN_SRAM_ACTIVE_PWR_CFG register.
//
//******************************************************************************
#define GPRCM_WLAN_SRAM_ACTIVE_PWR_CFG_WLAN_SRAM_ACTIVE_PWR_CFG_M \
                                0x00FFFFFF  // SRAM (WTOP+DRP) state during
                                            // Active-mode : 1 - SRAMs are ON ;
                                            // 0 - SRAMs are OFF. Cluster
                                            // information : [0] - 1st column of
                                            // MEMSS (Applicable only when owned
                                            // by WTOP/PHY) [1] - 2nd column of
                                            // MEMSS (Applicable only when owned
                                            // by WTOP/PHY) ; [2] - 3rd column
                                            // of MEMSS (Applicable only when
                                            // owned by WTOP/PHY) ; [3] - 4th
                                            // column of MEMSS (Applicable only
                                            // when owned by WTOP/PHY) ; [4] -
                                            // 5th column of MEMSS (Applicable
                                            // only when owned by WTOP/PHY) ;
                                            // [5] - 6th column of MEMSS
                                            // (Applicable only when owned by
                                            // WTOP/PHY) ; [6] - 7th column of
                                            // MEMSS (Applicable only when owned
                                            // by WTOP/PHY) ; [7] - 8th column
                                            // of MEMSS (Applicable only when
                                            // owned by WTOP/PHY) ; [8] - 9th
                                            // column of MEMSS (Applicable only
                                            // when owned by WTOP/PHY) ; [9] -
                                            // 10th column of MEMSS (Applicable
                                            // only when owned by WTOP/PHY) ;
                                            // [10] - 11th column of MEMSS
                                            // (Applicable only when owned by
                                            // WTOP/PHY) ; [11] - 12th column of
                                            // MEMSS (Applicable only when owned
                                            // by WTOP/PHY) ; [12] - 13th column
                                            // of MEMSS (Applicable only when
                                            // owned by WTOP/PHY) ; [13] - 14th
                                            // column of MEMSS (Applicable only
                                            // when owned by WTOP/PHY) ; [14] -
                                            // 15th column of MEMSS (Applicable
                                            // only when owned by WTOP/PHY) ;
                                            // [15] - 16th column of MEMSS
                                            // (Applicable only when owned by
                                            // WTOP/PHY) ; [23:16] - Internal to
                                            // WTOP Cluster

#define GPRCM_WLAN_SRAM_ACTIVE_PWR_CFG_WLAN_SRAM_ACTIVE_PWR_CFG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_WLAN_SRAM_SLEEP_PWR_CFG register.
//
//******************************************************************************
#define GPRCM_WLAN_SRAM_SLEEP_PWR_CFG_WLAN_SRAM_SLEEP_PWR_CFG_M \
                                0x00FFFFFF  // SRAM (WTOP+DRP) state during
                                            // Sleep-mode : 1 - SRAMs are RET ;
                                            // 0 - SRAMs are OFF. Cluster
                                            // information : [0] - 1st column of
                                            // MEMSS (Applicable only when owned
                                            // by WTOP/PHY) [1] - 2nd column of
                                            // MEMSS (Applicable only when owned
                                            // by WTOP/PHY) ; [2] - 3rd column
                                            // of MEMSS (Applicable only when
                                            // owned by WTOP/PHY) ; [3] - 4th
                                            // column of MEMSS (Applicable only
                                            // when owned by WTOP/PHY) ; [4] -
                                            // 5th column of MEMSS (Applicable
                                            // only when owned by WTOP/PHY) ;
                                            // [5] - 6th column of MEMSS
                                            // (Applicable only when owned by
                                            // WTOP/PHY) ; [6] - 7th column of
                                            // MEMSS (Applicable only when owned
                                            // by WTOP/PHY) ; [7] - 8th column
                                            // of MEMSS (Applicable only when
                                            // owned by WTOP/PHY) ; [8] - 9th
                                            // column of MEMSS (Applicable only
                                            // when owned by WTOP/PHY) ; [9] -
                                            // 10th column of MEMSS (Applicable
                                            // only when owned by WTOP/PHY) ;
                                            // [10] - 11th column of MEMSS
                                            // (Applicable only when owned by
                                            // WTOP/PHY) ; [11] - 12th column of
                                            // MEMSS (Applicable only when owned
                                            // by WTOP/PHY) ; [12] - 13th column
                                            // of MEMSS (Applicable only when
                                            // owned by WTOP/PHY) ; [13] - 14th
                                            // column of MEMSS (Applicable only
                                            // when owned by WTOP/PHY) ; [14] -
                                            // 15th column of MEMSS (Applicable
                                            // only when owned by WTOP/PHY) ;
                                            // [15] - 16th column of MEMSS
                                            // (Applicable only when owned by
                                            // WTOP/PHY) ; [23:16] - Internal to
                                            // WTOP Cluster

#define GPRCM_WLAN_SRAM_SLEEP_PWR_CFG_WLAN_SRAM_SLEEP_PWR_CFG_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APPS_SECURE_INIT_DONE register.
//
//******************************************************************************
#define GPRCM_APPS_SECURE_INIT_DONE_SECURE_INIT_DONE_STATUS \
                                0x00000002  // 1-Secure mode init is done ;
                                            // 0-Secure mode init is not done

#define GPRCM_APPS_SECURE_INIT_DONE_APPS_SECURE_INIT_DONE \
                                0x00000001  // Must be programmed 1 in order to
                                            // say that secure-mode device init
                                            // is done

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APPS_DEV_MODE_INIT_DONE register.
//
//******************************************************************************
#define GPRCM_APPS_DEV_MODE_INIT_DONE_APPS_DEV_MODE_INIT_DONE \
                                0x00000001  // 1 - Patch download and other
                                            // initializations are done (before
                                            // removing APPS resetn) for
                                            // development mode (#3) . 0 -
                                            // Development mode (#3) init is not
                                            // done yet

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_EN_APPS_REBOOT register.
//
//******************************************************************************
#define GPRCM_EN_APPS_REBOOT_EN_APPS_REBOOT \
                                0x00000001  // 1 - When 1, disable the reboot
                                            // of APPS after DevInit is
                                            // completed. In this case, APPS
                                            // will permanantly help in reset. 0
                                            // - When 0, enable the reboot of
                                            // APPS after DevInit is completed.

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MEM_APPS_PERIPH_PRESENT register.
//
//******************************************************************************
#define GPRCM_MEM_APPS_PERIPH_PRESENT_WLAN_GEM_PP \
                                0x00010000  // 1 - Enable ; 0 - Disable

#define GPRCM_MEM_APPS_PERIPH_PRESENT_APPS_AES_PP \
                                0x00008000

#define GPRCM_MEM_APPS_PERIPH_PRESENT_APPS_DES_PP \
                                0x00004000

#define GPRCM_MEM_APPS_PERIPH_PRESENT_APPS_SHA_PP \
                                0x00002000

#define GPRCM_MEM_APPS_PERIPH_PRESENT_APPS_CAMERA_PP \
                                0x00001000

#define GPRCM_MEM_APPS_PERIPH_PRESENT_APPS_MMCHS_PP \
                                0x00000800

#define GPRCM_MEM_APPS_PERIPH_PRESENT_APPS_MCASP_PP \
                                0x00000400

#define GPRCM_MEM_APPS_PERIPH_PRESENT_APPS_MCSPI_A1_PP \
                                0x00000200

#define GPRCM_MEM_APPS_PERIPH_PRESENT_APPS_MCSPI_A2_PP \
                                0x00000100

#define GPRCM_MEM_APPS_PERIPH_PRESENT_APPS_UDMA_PP \
                                0x00000080

#define GPRCM_MEM_APPS_PERIPH_PRESENT_APPS_WDOG_PP \
                                0x00000040

#define GPRCM_MEM_APPS_PERIPH_PRESENT_APPS_UART_A0_PP \
                                0x00000020

#define GPRCM_MEM_APPS_PERIPH_PRESENT_APPS_UART_A1_PP \
                                0x00000010

#define GPRCM_MEM_APPS_PERIPH_PRESENT_APPS_GPT_A0_PP \
                                0x00000008

#define GPRCM_MEM_APPS_PERIPH_PRESENT_APPS_GPT_A1_PP \
                                0x00000004

#define GPRCM_MEM_APPS_PERIPH_PRESENT_APPS_GPT_A2_PP \
                                0x00000002

#define GPRCM_MEM_APPS_PERIPH_PRESENT_APPS_GPT_A3_PP \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MEM_NWP_PERIPH_PRESENT register.
//
//******************************************************************************
#define GPRCM_MEM_NWP_PERIPH_PRESENT_NWP_ASYNC_BRIDGE_PP \
                                0x00000200

#define GPRCM_MEM_NWP_PERIPH_PRESENT_NWP_MCSPI_N2_PP \
                                0x00000100

#define GPRCM_MEM_NWP_PERIPH_PRESENT_NWP_GPT_N0_PP \
                                0x00000080

#define GPRCM_MEM_NWP_PERIPH_PRESENT_NWP_GPT_N1_PP \
                                0x00000040

#define GPRCM_MEM_NWP_PERIPH_PRESENT_NWP_WDOG_PP \
                                0x00000020

#define GPRCM_MEM_NWP_PERIPH_PRESENT_NWP_UDMA_PP \
                                0x00000010

#define GPRCM_MEM_NWP_PERIPH_PRESENT_NWP_UART_N0_PP \
                                0x00000008

#define GPRCM_MEM_NWP_PERIPH_PRESENT_NWP_UART_N1_PP \
                                0x00000004

#define GPRCM_MEM_NWP_PERIPH_PRESENT_NWP_SSDIO_PP \
                                0x00000002

#define GPRCM_MEM_NWP_PERIPH_PRESENT_NWP_MCSPI_N1_PP \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MEM_SHARED_PERIPH_PRESENT register.
//
//******************************************************************************

#define GPRCM_MEM_SHARED_PERIPH_PRESENT_SHARED_MCSPI_PP \
                                0x00000040

#define GPRCM_MEM_SHARED_PERIPH_PRESENT_SHARED_I2C_PP \
                                0x00000020

#define GPRCM_MEM_SHARED_PERIPH_PRESENT_SHARED_GPIO_A_PP \
                                0x00000010

#define GPRCM_MEM_SHARED_PERIPH_PRESENT_SHARED_GPIO_B_PP \
                                0x00000008

#define GPRCM_MEM_SHARED_PERIPH_PRESENT_SHARED_GPIO_C_PP \
                                0x00000004

#define GPRCM_MEM_SHARED_PERIPH_PRESENT_SHARED_GPIO_D_PP \
                                0x00000002

#define GPRCM_MEM_SHARED_PERIPH_PRESENT_SHARED_GPIO_E_PP \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_NWP_PWR_STATE register.
//
//******************************************************************************
#define GPRCM_NWP_PWR_STATE_NWP_PWR_STATE_PS_M \
                                0x00000F00  // "0000"- PORZ :- NWP is yet to be
                                            // enabled by APPS during powerup
                                            // (from HIB/OFF) ; "0011"- ACTIVE
                                            // :- NWP is enabled, clocks and
                                            // resets to NWP-SubSystem are
                                            // enabled ; "0010"- LPDS :- NWP is
                                            // in LPDS-mode ; Clocks and reset
                                            // to NWP-SubSystem are gated ;
                                            // "0101"- WAIT_FOR_OPP :- NWP is in
                                            // transition from LPDS to ACTIVE,
                                            // where it is waiting for OPP to be
                                            // stable ; "1000"-
                                            // WAKE_TIMER_OPP_REQ :- NWP is in
                                            // transition from LPDS, where the
                                            // wakeup cause is LPDS_Wake timer
                                            // OTHERS : NA

#define GPRCM_NWP_PWR_STATE_NWP_PWR_STATE_PS_S 8
#define GPRCM_NWP_PWR_STATE_NWP_RCM_PS_M \
                                0x00000007  // "000" - NWP_RUN : NWP is in RUN
                                            // state (default) - Applicable only
                                            // when NWP_PWR_STATE_PS = ACTIVE ;
                                            // "001" - NWP_SLP : NWP is in SLEEP
                                            // state (default) - Applicable only
                                            // when NWP_PWR_STATE_PS = ACTIVE ;
                                            // "010" - NWP_DSLP : NWP is in
                                            // Deep-Sleep state (default) -
                                            // Applicable only when
                                            // NWP_PWR_STATE_PS = ACTIVE ; "011"
                                            // - WAIT_FOR_ACTIVE : NWP is in
                                            // transition from Deep-sleep to
                                            // Run, where it is waiting for OPP
                                            // to be stable ; "100" -
                                            // WAIT_FOR_DSLP_TIMER_WAKE_REQ :
                                            // NWP is in transition from
                                            // Deep-sleep to Run, where the
                                            // wakeup cause is deep-sleep
                                            // wake-timer

#define GPRCM_NWP_PWR_STATE_NWP_RCM_PS_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APPS_PWR_STATE register.
//
//******************************************************************************
#define GPRCM_APPS_PWR_STATE_APPS_PWR_STATE_PS_M \
                                0x00000F00  // "0000"- PORZ :- APPS is waiting
                                            // for PLL_clock during powerup
                                            // (from HIB/OFF) ; "0011"- ACTIVE
                                            // :- APPS is enabled, clocks and
                                            // resets to APPS-SubSystem are
                                            // enabled ; APPS might be either in
                                            // Secure or Un-secure mode during
                                            // this state. "1001" -
                                            // SECURE_MODE_LPDS :- While in
                                            // ACTIVE (Secure-mode), APPS had to
                                            // program the DevInit_done bit at
                                            // the end, after which it enters
                                            // into this state, where the reset
                                            // to APPS will be asserted. From
                                            // this state APPS might either
                                            // re-boot itself or enter into LPDS
                                            // depending upon whether the device
                                            // is 3200 or 3100. "0010"- LPDS :-
                                            // APPS is in LPDS-mode ; Clocks and
                                            // reset to APPS-SubSystem are gated
                                            // ; "0101"- WAIT_FOR_OPP :- APPS is
                                            // in transition from LPDS to
                                            // ACTIVE, where it is waiting for
                                            // OPP to be stable ; "1000" -
                                            // WAKE_TIMER_OPP_REQ : APPS is in
                                            // transition from LPDS, where the
                                            // wakeup cause is LPDS_Wake timer ;
                                            // "1010" - WAIT_FOR_PATCH_INIT :
                                            // APPS enters into this state
                                            // during development-mode #3 (SOP =
                                            // 3), where it is waiting for patch
                                            // download to complete and 0x4 hack
                                            // is programmed. OTHERS : NA

#define GPRCM_APPS_PWR_STATE_APPS_PWR_STATE_PS_S 8
#define GPRCM_APPS_PWR_STATE_APPS_RCM_PS_M \
                                0x00000007  // "000" - APPS_RUN : APPS is in
                                            // RUN state (default) - Applicable
                                            // only when APPS_PWR_STATE_PS =
                                            // ACTIVE ; "001" - APPS_SLP : APPS
                                            // is in SLEEP state (default) -
                                            // Applicable only when
                                            // APPS_PWR_STATE_PS = ACTIVE ;
                                            // "010" - APPS_DSLP : APPS is in
                                            // Deep-Sleep state (default) -
                                            // Applicable only when
                                            // APPS_PWR_STATE_PS = ACTIVE ;
                                            // "011" - WAIT_FOR_ACTIVE : APPS is
                                            // in transition from Deep-sleep to
                                            // Run, where it is waiting for OPP
                                            // to be stable ; "100" -
                                            // WAIT_FOR_DSLP_TIMER_WAKE_REQ :
                                            // APPS is in transition from
                                            // Deep-sleep to Run, where the
                                            // wakeup cause is deep-sleep
                                            // wake-timer

#define GPRCM_APPS_PWR_STATE_APPS_RCM_PS_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MCU_PWR_STATE register.
//
//******************************************************************************
#define GPRCM_MCU_PWR_STATE_MCU_OPP_PS_M \
                                0x0000001F  // TBD

#define GPRCM_MCU_PWR_STATE_MCU_OPP_PS_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_WTOP_PM_PS register.
//
//******************************************************************************
#define GPRCM_WTOP_PM_PS_WTOP_PM_PS_M \
                                0x00000007  // "011" - WTOP_PM_ACTIVE (Default)
                                            // :- WTOP_Pd is in ACTIVE mode;
                                            // "100" - WTOP_PM_ACTIVE_TO_SLEEP
                                            // :- WTOP_Pd is in transition from
                                            // ACTIVE to SLEEP ; "000" -
                                            // WTOP_PM_SLEEP : WTOP-Pd is in
                                            // Sleep-state ; "100" -
                                            // WTOP_PM_SLEEP_TO_ACTIVE : WTOP_Pd
                                            // is in transition from SLEEP to
                                            // ACTIVE ; "000" -
                                            // WTOP_PM_WAIT_FOR_OPP : Wait for
                                            // OPP to be stable ;

#define GPRCM_WTOP_PM_PS_WTOP_PM_PS_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_WTOP_PD_RESETZ_OVERRIDE_REG register.
//
//******************************************************************************
#define GPRCM_WTOP_PD_RESETZ_OVERRIDE_REG_WTOP_PD_RESETZ_OVERRIDE_CTRL \
                                0x00000100  // Override control for WTOP PD
                                            // Resetz. When set to 1,
                                            // WTOP_Resetz will be controlled by
                                            // bit [0]

#define GPRCM_WTOP_PD_RESETZ_OVERRIDE_REG_WTOP_PD_RESETZ_OVERRIDE \
                                0x00000001  // Override for WTOP PD Resetz.
                                            // Applicable only when bit[8] is
                                            // set to 1

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_WELP_PD_RESETZ_OVERRIDE_REG register.
//
//******************************************************************************
#define GPRCM_WELP_PD_RESETZ_OVERRIDE_REG_WELP_PD_RESETZ_OVERRIDE_CTRL \
                                0x00000100  // Override control for WELP PD
                                            // Resetz. When set to 1,
                                            // WELP_Resetz will be controlled by
                                            // bit [0]

#define GPRCM_WELP_PD_RESETZ_OVERRIDE_REG_WELP_PD_RESETZ_OVERRIDE \
                                0x00000001  // Override for WELP PD Resetz.
                                            // Applicable only when bit[8] is
                                            // set to 1

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_WL_SDIO_PD_RESETZ_OVERRIDE_REG register.
//
//******************************************************************************
#define GPRCM_WL_SDIO_PD_RESETZ_OVERRIDE_REG_WL_SDIO_PD_RESETZ_OVERRIDE_CTRL \
                                0x00000100  // Override control for WL_SDIO
                                            // Resetz. When set to 1,
                                            // WL_SDIO_Resetz will be controlled
                                            // by bit [0]

#define GPRCM_WL_SDIO_PD_RESETZ_OVERRIDE_REG_WL_SDIO_PD_RESETZ_OVERRIDE \
                                0x00000001  // Override for WL_SDIO Resetz.
                                            // Applicable only when bit[8] is
                                            // set to 1

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_SSDIO_PD_RESETZ_OVERRIDE_REG register.
//
//******************************************************************************
#define GPRCM_SSDIO_PD_RESETZ_OVERRIDE_REG_SSDIO_PD_RESETZ_OVERRIDE_CTRL \
                                0x00000100  // Override control for SSDIO
                                            // Resetz. When set to 1,
                                            // SSDIO_Resetz will be controlled
                                            // by bit [0]

#define GPRCM_SSDIO_PD_RESETZ_OVERRIDE_REG_SSDIO_PD_RESETZ_OVERRIDE \
                                0x00000001  // Override for SSDIO Resetz.
                                            // Applicable only when bit[8] is
                                            // set to 1

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MCSPI_N1_PD_RESETZ_OVERRIDE_REG register.
//
//******************************************************************************
#define GPRCM_MCSPI_N1_PD_RESETZ_OVERRIDE_REG_MCSPI_N1_PD_RESETZ_OVERRIDE_CTRL \
                                0x00000100  // Override control for MCSPI_N1
                                            // Resetz. When set to 1,
                                            // MCSPI_N1_Resetz will be
                                            // controlled by bit [0]

#define GPRCM_MCSPI_N1_PD_RESETZ_OVERRIDE_REG_MCSPI_N1_PD_RESETZ_OVERRIDE \
                                0x00000001  // Override for MCSPI_N1 Resetz.
                                            // Applicable only when bit[8] is
                                            // set to 1

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_TESTCTRL_PD_RESETZ_OVERRIDE_REG register.
//
//******************************************************************************
#define GPRCM_TESTCTRL_PD_RESETZ_OVERRIDE_REG_TESTCTRL_PD_RESETZ_OVERRIDE_CTRL \
                                0x00000100  // Override control for TESTCTRL-PD
                                            // Resetz. When set to 1,
                                            // TESTCTRL_Resetz will be
                                            // controlled by bit [0]

#define GPRCM_TESTCTRL_PD_RESETZ_OVERRIDE_REG_TESTCTRL_PD_RESETZ_OVERRIDE \
                                0x00000001  // Override for TESTCTRL Resetz.
                                            // Applicable only when bit[8] is
                                            // set to 1

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MCU_PD_RESETZ_OVERRIDE_REG register.
//
//******************************************************************************
#define GPRCM_MCU_PD_RESETZ_OVERRIDE_REG_MCU_PD_RESETZ_OVERRIDE_CTRL \
                                0x00000100  // Override control for MCU-PD
                                            // Resetz. When set to 1, MCU_Resetz
                                            // will be controlled by bit [0]

#define GPRCM_MCU_PD_RESETZ_OVERRIDE_REG_MCU_PD_RESETZ_OVERRIDE \
                                0x00000001  // Override for MCU Resetz.
                                            // Applicable only when bit[8] is
                                            // set to 1

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_EFUSE_READ_REG0 register.
//
//******************************************************************************
#define GPRCM_GPRCM_EFUSE_READ_REG0_FUSEFARM_ROW_14_M \
                                0xFFFFFFFF  // This is ROW_14 [31:0] of
                                            // FUSEFARM. [0:0] : XTAL_IS_26MHZ
                                            // [5:1] : TOP_CLKM_RTRIM[4:0]
                                            // [10:6] : ANA_BGAP_MAG_TRIM[4:0]
                                            // [16:11] : ANA_BGAP_TEMP_TRIM[5:0]
                                            // [20:17] : ANA_BGAP_V2I_TRIM[3:0]
                                            // [25:22] : PROCESS INDICATOR
                                            // [26:26] : Reserved [31:27] :
                                            // FUSEROM Version

#define GPRCM_GPRCM_EFUSE_READ_REG0_FUSEFARM_ROW_14_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_EFUSE_READ_REG1 register.
//
//******************************************************************************
#define GPRCM_GPRCM_EFUSE_READ_REG1_FUSEFARM_ROW_15_LSW_M \
                                0x0000FFFF  // This is ROW_15[15:0] of FUSEFARM
                                            // 1. NWP Peripheral Present bits
                                            // [15:8] NWP_GPT_N0_PP [15:15]
                                            // NWP_GPT_N1_PP [14:14] NWP_WDOG_PP
                                            // [13:13] NWP_UDMA_PP [12:12]
                                            // NWP_UART_N0_PP [11:11]
                                            // NWP_UART_N1_PP [10:10]
                                            // NWP_SSDIO_PP [9:9]
                                            // NWP_MCSPI_N1_PP [8:8] 2. Shared
                                            // Peripheral Present bits [7:0]
                                            // SHARED SPI PP [6:6]
                                            // SHARED I2C PP [5:5] SHARED
                                            // GPIO-A PP [4:4] SHARED GPIO-B PP
                                            // [3:3] SHARED GPIO-C PP [2:2]
                                            // SHARED GPIO-D PP [1:1] SHARED
                                            // GPIO-E PP [0:0]

#define GPRCM_GPRCM_EFUSE_READ_REG1_FUSEFARM_ROW_15_LSW_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_EFUSE_READ_REG2 register.
//
//******************************************************************************
#define GPRCM_GPRCM_EFUSE_READ_REG2_FUSEFARM_ROW_16_LSW_ROW_15_MSW_M \
                                0xFFFFFFFF  // This is ROW_16[15:0] &
                                            // ROW_15[31:16] of FUSEFARM.
                                            // [31:21] - Reserved [20:16] -
                                            // CHIP_ID [15:15] - SSBD SOP
                                            // Control [14:14] - SSBD TAP
                                            // Control [13:2] - APPS Peripheral
                                            // Present bits : APPS_CAMERA_PP
                                            // [13:13] APPS_MMCHS_PP [12:12]
                                            // APPS_MCASP_PP [11:11]
                                            // APPS_MCSPI_A1_PP [10:10]
                                            // APPS_MCSPI_A2_PP [9:9]
                                            // APPS_UDMA_PP [8:8] APPS_WDOG_PP
                                            // [7:7] APPS_UART_A0_PP [6:6]
                                            // APPS_UART_A1_PP [5:5]
                                            // APPS_GPT_A0_PP [4:4]
                                            // APPS_GPT_A1_PP [3:3]
                                            // APPS_GPT_A2_PP [2:2]
                                            // APPS_GPT_A3_PP [1:1] [0:0] - NWP
                                            // Peripheral present bits
                                            // NWP_ACSPI_PP [0:0]

#define GPRCM_GPRCM_EFUSE_READ_REG2_FUSEFARM_ROW_16_LSW_ROW_15_MSW_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_EFUSE_READ_REG3 register.
//
//******************************************************************************
#define GPRCM_GPRCM_EFUSE_READ_REG3_FUSEFARM_ROW_17_LSW_ROW_16_MSW_M \
                                0xFFFFFFFF  // This is ROW_17[15:0] &
                                            // ROW_16[31:16] of FUSEFARM :
                                            // [31:16] - TEST_TAP_KEY(15:0)
                                            // [15:0] - Reserved

#define GPRCM_GPRCM_EFUSE_READ_REG3_FUSEFARM_ROW_17_LSW_ROW_16_MSW_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_WTOP_MEM_RET_CFG register.
//
//******************************************************************************
#define GPRCM_WTOP_MEM_RET_CFG_WTOP_MEM_RET_CFG \
                                0x00000001  // 1 - Soft-compile memories in
                                            // WTOP can be turned-off during
                                            // WTOP-sleep mode ; 0 -
                                            // Soft-compile memories in WTOP
                                            // must be kept on during WTOP-sleep
                                            // mode.

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_COEX_CLK_SWALLOW_CFG0 register.
//
//******************************************************************************
#define GPRCM_COEX_CLK_SWALLOW_CFG0_Q_FACTOR_M \
                                0x007FFFFF  // TBD

#define GPRCM_COEX_CLK_SWALLOW_CFG0_Q_FACTOR_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_COEX_CLK_SWALLOW_CFG1 register.
//
//******************************************************************************
#define GPRCM_COEX_CLK_SWALLOW_CFG1_P_FACTOR_M \
                                0x000FFFFF  // TBD

#define GPRCM_COEX_CLK_SWALLOW_CFG1_P_FACTOR_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_COEX_CLK_SWALLOW_CFG2 register.
//
//******************************************************************************
#define GPRCM_COEX_CLK_SWALLOW_CFG2_CONSECUTIVE_SWALLOW_M \
                                0x00000018

#define GPRCM_COEX_CLK_SWALLOW_CFG2_CONSECUTIVE_SWALLOW_S 3
#define GPRCM_COEX_CLK_SWALLOW_CFG2_PRBS_GAIN \
                                0x00000004

#define GPRCM_COEX_CLK_SWALLOW_CFG2_PRBS_ENABLE \
                                0x00000002

#define GPRCM_COEX_CLK_SWALLOW_CFG2_SWALLOW_ENABLE \
                                0x00000001  // TBD

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_COEX_CLK_SWALLOW_ENABLE register.
//
//******************************************************************************
#define GPRCM_COEX_CLK_SWALLOW_ENABLE_COEX_CLK_SWALLOW_ENABLE \
                                0x00000001  // 1 - Enable switching of sysclk
                                            // to Coex-clk path ; 0 - Disable
                                            // switching of sysclk to Coex-clk
                                            // path.

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_DCDC_CLK_GEN_CONFIG register.
//
//******************************************************************************
#define GPRCM_DCDC_CLK_GEN_CONFIG_DCDC_CLK_ENABLE \
                                0x00000001  // 1 - Enable the clock for DCDC
                                            // (PWM-mode) ; 0 - Disable the
                                            // clock for DCDC (PWM-mode)

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_EFUSE_READ_REG4 register.
//
//******************************************************************************
#define GPRCM_GPRCM_EFUSE_READ_REG4_FUSEFARM_ROW_17_MSW_M \
                                0x0000FFFF  // This corresponds to
                                            // ROW_17[31:16] of the FUSEFARM :
                                            // [15:0] : TEST_TAP_KEY(31:16)

#define GPRCM_GPRCM_EFUSE_READ_REG4_FUSEFARM_ROW_17_MSW_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_EFUSE_READ_REG5 register.
//
//******************************************************************************
#define GPRCM_GPRCM_EFUSE_READ_REG5_FUSEFARM_ROW_18_M \
                                0xFFFFFFFF  // Corresponds to ROW_18 of
                                            // FUSEFARM. [29:0] -
                                            // MEMSS_COLUMN_SEL_LSW ; [30:30] -
                                            // WLAN GEM DISABLE ; [31:31] -
                                            // SERIAL WIRE JTAG SELECT

#define GPRCM_GPRCM_EFUSE_READ_REG5_FUSEFARM_ROW_18_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_EFUSE_READ_REG6 register.
//
//******************************************************************************
#define GPRCM_GPRCM_EFUSE_READ_REG6_FUSEFARM_ROW_19_LSW_M \
                                0x0000FFFF  // Corresponds to ROW_19[15:0] of
                                            // FUSEFARM. [15:0] :
                                            // MEMSS_COLUMN_SEL_MSW

#define GPRCM_GPRCM_EFUSE_READ_REG6_FUSEFARM_ROW_19_LSW_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_EFUSE_READ_REG7 register.
//
//******************************************************************************
#define GPRCM_GPRCM_EFUSE_READ_REG7_FUSEFARM_ROW_20_LSW_ROW_19_MSW_M \
                                0xFFFFFFFF  // Corresponds to ROW_20[15:0] &
                                            // ROW_19[31:16] of FUSEFARM.
                                            // FLASH_REGION0

#define GPRCM_GPRCM_EFUSE_READ_REG7_FUSEFARM_ROW_20_LSW_ROW_19_MSW_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_EFUSE_READ_REG8 register.
//
//******************************************************************************
#define GPRCM_GPRCM_EFUSE_READ_REG8_FUSEFARM_ROW_21_LSW_ROW_20_MSW_M \
                                0xFFFFFFFF  // Corresponds to ROW_21[15:0] &
                                            // ROW_20[31:16] of FUSEFARM.
                                            // FLASH_REGION1

#define GPRCM_GPRCM_EFUSE_READ_REG8_FUSEFARM_ROW_21_LSW_ROW_20_MSW_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_EFUSE_READ_REG9 register.
//
//******************************************************************************
#define GPRCM_GPRCM_EFUSE_READ_REG9_FUSEFARM_ROW_22_LSW_ROW_21_MSW_M \
                                0xFFFFFFFF  // Corresponds to ROW_22[15:0] &
                                            // ROW_21[31:16] of FUSEFARM.
                                            // FLASH_REGION2

#define GPRCM_GPRCM_EFUSE_READ_REG9_FUSEFARM_ROW_22_LSW_ROW_21_MSW_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_EFUSE_READ_REG10 register.
//
//******************************************************************************
#define GPRCM_GPRCM_EFUSE_READ_REG10_FUSEFARM_ROW_23_LSW_ROW_22_MSW_M \
                                0xFFFFFFFF  // Corresponds to ROW_23[15:0] &
                                            // ROW_22[31:16] of FUSEFARM.
                                            // FLASH_REGION3

#define GPRCM_GPRCM_EFUSE_READ_REG10_FUSEFARM_ROW_23_LSW_ROW_22_MSW_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_EFUSE_READ_REG11 register.
//
//******************************************************************************
#define GPRCM_GPRCM_EFUSE_READ_REG11_FUSEFARM_ROW_24_LSW_ROW_23_MSW_M \
                                0xFFFFFFFF  // Corresponds to ROW_24[15:0] &
                                            // ROW_23[31:16] of FUSEFARM.
                                            // FLASH_DESCRIPTOR

#define GPRCM_GPRCM_EFUSE_READ_REG11_FUSEFARM_ROW_24_LSW_ROW_23_MSW_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_DIEID_READ_REG0 register.
//
//******************************************************************************
#define GPRCM_GPRCM_DIEID_READ_REG0_FUSEFARM_191_160_M \
                                0xFFFFFFFF  // Corresponds to bits [191:160] of
                                            // the FUSEFARM. This is ROW_5 of
                                            // FUSEFARM [191:160] : [31:0] :
                                            // DIE_ID0 [31:0] : DEVX [11:0] DEVY
                                            // [23:12] DEVWAF [29:24] DEV_SPARE
                                            // [31:30]

#define GPRCM_GPRCM_DIEID_READ_REG0_FUSEFARM_191_160_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_DIEID_READ_REG1 register.
//
//******************************************************************************
#define GPRCM_GPRCM_DIEID_READ_REG1_FUSEFARM_223_192_M \
                                0xFFFFFFFF  // Corresponds to bits [223:192] of
                                            // the FUSEFARM. This is ROW_6 of
                                            // FUSEFARM :- DEVLOT [23:0] DEVFAB
                                            // [28:24] DEVFABBE [31:29]

#define GPRCM_GPRCM_DIEID_READ_REG1_FUSEFARM_223_192_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_DIEID_READ_REG2 register.
//
//******************************************************************************
#define GPRCM_GPRCM_DIEID_READ_REG2_FUSEFARM_255_224_M \
                                0xFFFFFFFF  // Corresponds to bits [255:224] of
                                            // the FUSEFARM. This is ROW_7 of
                                            // FUSEFARM:- DEVDESREV[4:0]
                                            // Memrepair[5:5] MakeDefined[16:6]
                                            // CHECKSUM[30:17] Reserved :
                                            // [31:31]

#define GPRCM_GPRCM_DIEID_READ_REG2_FUSEFARM_255_224_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_DIEID_READ_REG3 register.
//
//******************************************************************************
#define GPRCM_GPRCM_DIEID_READ_REG3_FUSEFARM_287_256_M \
                                0xFFFFFFFF  // Corresponds to bits [287:256] of
                                            // the FUSEFARM. This is ROW_8 of
                                            // FUSEFARM :- DIEID0 - DEVREG
                                            // [31:0]

#define GPRCM_GPRCM_DIEID_READ_REG3_FUSEFARM_287_256_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_GPRCM_DIEID_READ_REG4 register.
//
//******************************************************************************
#define GPRCM_GPRCM_DIEID_READ_REG4_FUSEFARM_319_288_M \
                                0xFFFFFFFF  // Corresponds to bits [319:288] of
                                            // the FUSEFARM. This is ROW_9 of
                                            // FUSEFARM :- [7:0] - VBATMON ;
                                            // [13:8] - BUFF_OFFSET ; [15:15] -
                                            // DFT_GXG ; [14:14] - DFT_GLX ;
                                            // [19:16] - PHY ROM Version ;
                                            // [23:20] - MAC ROM Version ;
                                            // [27:24] - NWP ROM Version ;
                                            // [31:28] - APPS ROM Version

#define GPRCM_GPRCM_DIEID_READ_REG4_FUSEFARM_319_288_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_APPS_SS_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_APPS_SS_OVERRIDES_reserved_M \
                                0xFFFFFC00

#define GPRCM_APPS_SS_OVERRIDES_reserved_S 10
#define GPRCM_APPS_SS_OVERRIDES_mem_apps_refclk_gating_override \
                                0x00000200

#define GPRCM_APPS_SS_OVERRIDES_mem_apps_refclk_gating_override_ctrl \
                                0x00000100

#define GPRCM_APPS_SS_OVERRIDES_mem_apps_pllclk_gating_override \
                                0x00000080

#define GPRCM_APPS_SS_OVERRIDES_mem_apps_pllclk_gating_override_ctrl \
                                0x00000040

#define GPRCM_APPS_SS_OVERRIDES_mem_apps_por_rstn_override \
                                0x00000020

#define GPRCM_APPS_SS_OVERRIDES_mem_apps_sysrstn_override \
                                0x00000010

#define GPRCM_APPS_SS_OVERRIDES_mem_apps_sysclk_gating_override \
                                0x00000008

#define GPRCM_APPS_SS_OVERRIDES_mem_apps_por_rstn_override_ctrl \
                                0x00000004

#define GPRCM_APPS_SS_OVERRIDES_mem_apps_sysrstn_override_ctrl \
                                0x00000002

#define GPRCM_APPS_SS_OVERRIDES_mem_apps_sysclk_gating_override_ctrl \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_NWP_SS_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_NWP_SS_OVERRIDES_reserved_M \
                                0xFFFFFC00

#define GPRCM_NWP_SS_OVERRIDES_reserved_S 10
#define GPRCM_NWP_SS_OVERRIDES_mem_nwp_refclk_gating_override \
                                0x00000200

#define GPRCM_NWP_SS_OVERRIDES_mem_nwp_refclk_gating_override_ctrl \
                                0x00000100

#define GPRCM_NWP_SS_OVERRIDES_mem_nwp_pllclk_gating_override \
                                0x00000080

#define GPRCM_NWP_SS_OVERRIDES_mem_nwp_pllclk_gating_override_ctrl \
                                0x00000040

#define GPRCM_NWP_SS_OVERRIDES_mem_nwp_por_rstn_override \
                                0x00000020

#define GPRCM_NWP_SS_OVERRIDES_mem_nwp_sysrstn_override \
                                0x00000010

#define GPRCM_NWP_SS_OVERRIDES_mem_nwp_sysclk_gating_override \
                                0x00000008

#define GPRCM_NWP_SS_OVERRIDES_mem_nwp_por_rstn_override_ctrl \
                                0x00000004

#define GPRCM_NWP_SS_OVERRIDES_mem_nwp_sysrstn_override_ctrl \
                                0x00000002

#define GPRCM_NWP_SS_OVERRIDES_mem_nwp_sysclk_gating_override_ctrl \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_SHARED_SS_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_SHARED_SS_OVERRIDES_reserved_M \
                                0xFFFFFF00

#define GPRCM_SHARED_SS_OVERRIDES_reserved_S 8
#define GPRCM_SHARED_SS_OVERRIDES_mem_shared_pllclk_gating_override_ctrl \
                                0x00000080

#define GPRCM_SHARED_SS_OVERRIDES_mem_shared_pllclk_gating_override \
                                0x00000040

#define GPRCM_SHARED_SS_OVERRIDES_mem_shared_refclk_gating_override_ctrl \
                                0x00000020

#define GPRCM_SHARED_SS_OVERRIDES_mem_shared_refclk_gating_override \
                                0x00000010

#define GPRCM_SHARED_SS_OVERRIDES_mem_shared_rstn_override \
                                0x00000008

#define GPRCM_SHARED_SS_OVERRIDES_mem_shared_sysclk_gating_override \
                                0x00000004

#define GPRCM_SHARED_SS_OVERRIDES_mem_shared_rstn_override_ctrl \
                                0x00000002

#define GPRCM_SHARED_SS_OVERRIDES_mem_shared_sysclk_gating_override_ctrl \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_IDMEM_CORE_RST_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_IDMEM_CORE_RST_OVERRIDES_reserved_M \
                                0xFFFFFF00

#define GPRCM_IDMEM_CORE_RST_OVERRIDES_reserved_S 8
#define GPRCM_IDMEM_CORE_RST_OVERRIDES_mem_idmem_core_sysrstn_override \
                                0x00000080

#define GPRCM_IDMEM_CORE_RST_OVERRIDES_mem_idmem_core_fmc_rstn_override \
                                0x00000040

#define GPRCM_IDMEM_CORE_RST_OVERRIDES_SPARE_RW1 \
                                0x00000020

#define GPRCM_IDMEM_CORE_RST_OVERRIDES_mem_idmem_core_piosc_gating_override \
                                0x00000010

#define GPRCM_IDMEM_CORE_RST_OVERRIDES_mem_idmem_core_sysrstn_override_ctrl \
                                0x00000008

#define GPRCM_IDMEM_CORE_RST_OVERRIDES_mem_idmem_core_fmc_rstn_override_ctrl \
                                0x00000004

#define GPRCM_IDMEM_CORE_RST_OVERRIDES_SPARE_RW0 \
                                0x00000002

#define GPRCM_IDMEM_CORE_RST_OVERRIDES_mem_idmem_core_piosc_gating_override_ctrl \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_TOP_DIE_FSM_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_TOP_DIE_FSM_OVERRIDES_reserved_M \
                                0xFFFFF000

#define GPRCM_TOP_DIE_FSM_OVERRIDES_reserved_S 12
#define GPRCM_TOP_DIE_FSM_OVERRIDES_mem_d2d_pwr_switch_pgoodin_override_ctrl \
                                0x00000800

#define GPRCM_TOP_DIE_FSM_OVERRIDES_mem_d2d_pwr_switch_pgoodin_override \
                                0x00000400

#define GPRCM_TOP_DIE_FSM_OVERRIDES_mem_d2d_hclk_gating_override \
                                0x00000200

#define GPRCM_TOP_DIE_FSM_OVERRIDES_mem_d2d_piosc_gating_override \
                                0x00000100

#define GPRCM_TOP_DIE_FSM_OVERRIDES_mem_d2d_rstn_override \
                                0x00000080

#define GPRCM_TOP_DIE_FSM_OVERRIDES_mem_d2d_pwr_switch_ponin_override \
                                0x00000040

#define GPRCM_TOP_DIE_FSM_OVERRIDES_mem_flash_ready_override \
                                0x00000020

#define GPRCM_TOP_DIE_FSM_OVERRIDES_mem_d2d_hclk_gating_override_ctrl \
                                0x00000010

#define GPRCM_TOP_DIE_FSM_OVERRIDES_mem_d2d_piosc_gating_override_ctrl \
                                0x00000008

#define GPRCM_TOP_DIE_FSM_OVERRIDES_mem_d2d_rstn_override_ctrl \
                                0x00000004

#define GPRCM_TOP_DIE_FSM_OVERRIDES_mem_d2d_pwr_switch_ponin_override_ctrl \
                                0x00000002

#define GPRCM_TOP_DIE_FSM_OVERRIDES_mem_flash_ready_override_ctrl \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MCU_PSCON_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_MCU_PSCON_OVERRIDES_reserved_M \
                                0xFFF00000

#define GPRCM_MCU_PSCON_OVERRIDES_reserved_S 20
#define GPRCM_MCU_PSCON_OVERRIDES_mem_mcu_pscon_mem_sleep_override_ctrl \
                                0x00080000

#define GPRCM_MCU_PSCON_OVERRIDES_mem_mcu_pscon_mem_update_override_ctrl \
                                0x00040000

#define GPRCM_MCU_PSCON_OVERRIDES_mem_mcu_pscon_mem_off_override_ctrl \
                                0x00020000

#define GPRCM_MCU_PSCON_OVERRIDES_mem_mcu_pscon_mem_retain_override_ctrl \
                                0x00010000

#define GPRCM_MCU_PSCON_OVERRIDES_NU1_M \
                                0x0000FC00

#define GPRCM_MCU_PSCON_OVERRIDES_NU1_S 10
#define GPRCM_MCU_PSCON_OVERRIDES_mem_mcu_pscon_sleep_override \
                                0x00000200

#define GPRCM_MCU_PSCON_OVERRIDES_mem_mcu_pscon_mem_update_override \
                                0x00000100

#define GPRCM_MCU_PSCON_OVERRIDES_mem_mcu_pscon_mem_off_override_M \
                                0x000000F0

#define GPRCM_MCU_PSCON_OVERRIDES_mem_mcu_pscon_mem_off_override_S 4
#define GPRCM_MCU_PSCON_OVERRIDES_mem_mcu_pscon_mem_retain_override_M \
                                0x0000000F

#define GPRCM_MCU_PSCON_OVERRIDES_mem_mcu_pscon_mem_retain_override_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_WTOP_PSCON_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_WTOP_PSCON_OVERRIDES_reserved_M \
                                0xFFC00000

#define GPRCM_WTOP_PSCON_OVERRIDES_reserved_S 22
#define GPRCM_WTOP_PSCON_OVERRIDES_mem_wtop_pscon_sleep_override_ctrl \
                                0x00200000

#define GPRCM_WTOP_PSCON_OVERRIDES_mem_wtop_pscon_mem_update_override_ctrl \
                                0x00100000

#define GPRCM_WTOP_PSCON_OVERRIDES_mem_wtop_pscon_mem_off_override_ctrl \
                                0x00080000

#define GPRCM_WTOP_PSCON_OVERRIDES_mem_wtop_pscon_mem_retain_override_ctrl \
                                0x00040000

#define GPRCM_WTOP_PSCON_OVERRIDES_mem_wtop_pscon_sleep_override \
                                0x00020000

#define GPRCM_WTOP_PSCON_OVERRIDES_mem_wtop_pscon_mem_update_override \
                                0x00010000

#define GPRCM_WTOP_PSCON_OVERRIDES_mem_wtop_pscon_mem_off_override_M \
                                0x0000FF00

#define GPRCM_WTOP_PSCON_OVERRIDES_mem_wtop_pscon_mem_off_override_S 8
#define GPRCM_WTOP_PSCON_OVERRIDES_mem_wtop_pscon_mem_retain_override_M \
                                0x000000FF

#define GPRCM_WTOP_PSCON_OVERRIDES_mem_wtop_pscon_mem_retain_override_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_WELP_PSCON_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_WELP_PSCON_OVERRIDES_reserved_M \
                                0xFFFFFFFC

#define GPRCM_WELP_PSCON_OVERRIDES_reserved_S 2
#define GPRCM_WELP_PSCON_OVERRIDES_mem_welp_pscon_sleep_override_ctrl \
                                0x00000002

#define GPRCM_WELP_PSCON_OVERRIDES_mem_welp_pscon_sleep_override \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_WL_SDIO_PSCON_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_WL_SDIO_PSCON_OVERRIDES_reserved_M \
                                0xFFFFFFFC

#define GPRCM_WL_SDIO_PSCON_OVERRIDES_reserved_S 2
#define GPRCM_WL_SDIO_PSCON_OVERRIDES_mem_wl_sdio_pscon_sleep_override_ctrl \
                                0x00000002

#define GPRCM_WL_SDIO_PSCON_OVERRIDES_mem_wl_sdio_pscon_sleep_override \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_MCSPI_PSCON_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_MCSPI_PSCON_OVERRIDES_reserved_M \
                                0xFFFFFF00

#define GPRCM_MCSPI_PSCON_OVERRIDES_reserved_S 8
#define GPRCM_MCSPI_PSCON_OVERRIDES_mem_mcspi_pscon_mem_retain_override_ctrl \
                                0x00000080

#define GPRCM_MCSPI_PSCON_OVERRIDES_mem_mcspi_pscon_mem_off_override_ctrl \
                                0x00000040

#define GPRCM_MCSPI_PSCON_OVERRIDES_mem_mcspi_pscon_mem_retain_override \
                                0x00000020

#define GPRCM_MCSPI_PSCON_OVERRIDES_mem_mcspi_pscon_mem_off_override \
                                0x00000010

#define GPRCM_MCSPI_PSCON_OVERRIDES_mem_mcspi_pscon_mem_update_override_ctrl \
                                0x00000008

#define GPRCM_MCSPI_PSCON_OVERRIDES_mem_mcspi_pscon_mem_update_override \
                                0x00000004

#define GPRCM_MCSPI_PSCON_OVERRIDES_mem_mcspi_pscon_sleep_override_ctrl \
                                0x00000002

#define GPRCM_MCSPI_PSCON_OVERRIDES_mem_mcspi_pscon_sleep_override \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// GPRCM_O_SSDIO_PSCON_OVERRIDES register.
//
//******************************************************************************
#define GPRCM_SSDIO_PSCON_OVERRIDES_reserved_M \
                                0xFFFFFFFC

#define GPRCM_SSDIO_PSCON_OVERRIDES_reserved_S 2
#define GPRCM_SSDIO_PSCON_OVERRIDES_mem_ssdio_pscon_sleep_override_ctrl \
                                0x00000002

#define GPRCM_SSDIO_PSCON_OVERRIDES_mem_ssdio_pscon_sleep_override \
                                0x00000001




#endif // __HW_GPRCM_H__
