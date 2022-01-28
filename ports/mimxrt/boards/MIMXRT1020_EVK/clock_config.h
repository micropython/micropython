/*
 * Copyright 2018-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CLOCK_CONFIG_H_
#define _CLOCK_CONFIG_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_XTAL0_CLK_HZ 24000000U /*!< Board xtal0 frequency in Hz */

#define BOARD_XTAL32K_CLK_HZ 32768U /*!< Board xtal32k frequency in Hz */
/*******************************************************************************
 ************************ BOARD_InitBootClocks function ************************
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*!
 * @brief This function executes default configuration of clocks.
 *
 */
void BOARD_InitBootClocks(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*******************************************************************************
 ********************** Configuration BOARD_BootClockRUN ***********************
 ******************************************************************************/
/*******************************************************************************
 * Definitions for BOARD_BootClockRUN configuration
 ******************************************************************************/
#define BOARD_BOOTCLOCKRUN_CORE_CLOCK 500000000U /*!< Core clock frequency: 500000000Hz */

/* Clock outputs (values are in Hz): */
#define BOARD_BOOTCLOCKRUN_AHB_CLK_ROOT 500000000UL
#define BOARD_BOOTCLOCKRUN_CAN_CLK_ROOT 40000000UL
#define BOARD_BOOTCLOCKRUN_CKIL_SYNC_CLK_ROOT 32768UL
#define BOARD_BOOTCLOCKRUN_CLKO1_CLK 0UL
#define BOARD_BOOTCLOCKRUN_CLKO2_CLK 0UL
#define BOARD_BOOTCLOCKRUN_CLK_1M 1000000UL
#define BOARD_BOOTCLOCKRUN_CLK_24M 24000000UL
#define BOARD_BOOTCLOCKRUN_ENET1_TX_CLK 0UL
#define BOARD_BOOTCLOCKRUN_ENET_125M_CLK 0UL
#define BOARD_BOOTCLOCKRUN_ENET_25M_REF_CLK 0UL
#define BOARD_BOOTCLOCKRUN_ENET_500M_REF_CLK 500000000UL
#define BOARD_BOOTCLOCKRUN_FLEXIO1_CLK_ROOT 30000000UL
#define BOARD_BOOTCLOCKRUN_FLEXSPI_CLK_ROOT 132000000UL
#define BOARD_BOOTCLOCKRUN_GPT1_IPG_CLK_HIGHFREQ 62500000UL
#define BOARD_BOOTCLOCKRUN_GPT2_IPG_CLK_HIGHFREQ 62500000UL
#define BOARD_BOOTCLOCKRUN_IPG_CLK_ROOT 125000000UL
#define BOARD_BOOTCLOCKRUN_LPI2C_CLK_ROOT 60000000UL
#define BOARD_BOOTCLOCKRUN_LPSPI_CLK_ROOT 105600000UL
#define BOARD_BOOTCLOCKRUN_MQS_MCLK 63529411UL
#define BOARD_BOOTCLOCKRUN_PERCLK_CLK_ROOT 62500000UL
#define BOARD_BOOTCLOCKRUN_SAI1_CLK_ROOT 63529411UL
#define BOARD_BOOTCLOCKRUN_SAI1_MCLK1 63529411UL
#define BOARD_BOOTCLOCKRUN_SAI1_MCLK2 63529411UL
#define BOARD_BOOTCLOCKRUN_SAI1_MCLK3 30000000UL
#define BOARD_BOOTCLOCKRUN_SAI2_CLK_ROOT 63529411UL
#define BOARD_BOOTCLOCKRUN_SAI2_MCLK1 63529411UL
#define BOARD_BOOTCLOCKRUN_SAI2_MCLK2 0UL
#define BOARD_BOOTCLOCKRUN_SAI2_MCLK3 30000000UL
#define BOARD_BOOTCLOCKRUN_SAI3_CLK_ROOT 63529411UL
#define BOARD_BOOTCLOCKRUN_SAI3_MCLK1 63529411UL
#define BOARD_BOOTCLOCKRUN_SAI3_MCLK2 0UL
#define BOARD_BOOTCLOCKRUN_SAI3_MCLK3 30000000UL
#define BOARD_BOOTCLOCKRUN_SEMC_CLK_ROOT 62500000UL
#define BOARD_BOOTCLOCKRUN_SPDIF0_CLK_ROOT 30000000UL
#define BOARD_BOOTCLOCKRUN_SPDIF0_EXTCLK_OUT 0UL
#define BOARD_BOOTCLOCKRUN_TRACE_CLK_ROOT 117333333UL
#define BOARD_BOOTCLOCKRUN_UART_CLK_ROOT 80000000UL
#define BOARD_BOOTCLOCKRUN_USBPHY1_CLK 0UL
#define BOARD_BOOTCLOCKRUN_USDHC1_CLK_ROOT 176000000UL
#define BOARD_BOOTCLOCKRUN_USDHC2_CLK_ROOT 176000000UL

/*! @brief Usb1 PLL set for BOARD_BootClockRUN configuration.
 */
extern const clock_usb_pll_config_t usb1PllConfig_BOARD_BootClockRUN;
/*! @brief Sys PLL for BOARD_BootClockRUN configuration.
 */
extern const clock_sys_pll_config_t sysPllConfig_BOARD_BootClockRUN;
/*! @brief Enet PLL set for BOARD_BootClockRUN configuration.
 */
extern const clock_enet_pll_config_t enetPllConfig_BOARD_BootClockRUN;

/*******************************************************************************
 * API for BOARD_BootClockRUN configuration
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*!
 * @brief This function executes configuration of clocks.
 *
 */
void BOARD_BootClockRUN(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#endif /* _CLOCK_CONFIG_H_ */
