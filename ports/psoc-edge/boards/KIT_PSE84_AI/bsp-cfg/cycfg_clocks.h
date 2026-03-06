/*******************************************************************************
 * File Name: cycfg_clocks.h
 *
 * Description:
 * Clock configuration
 * This file was automatically generated and should not be modified.
 * Configurator Backend 3.60.0
 * device-db 4.34.0.9502
 * mtb-dsl-pse8xxgp 1.2.0.895
 *
 *******************************************************************************
 * Copyright 2026 Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

#if !defined(CYCFG_CLOCKS_H)
#define CYCFG_CLOCKS_H

#include "cycfg_notices.h"
#include "cy_sysclk.h"
#include "cy_wdt.h"
#include "cycfg_clock_types.h"
#include "cy_rtc.h"
#include "cy_gpio.h"

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#define CYBSP_RTC_ENABLED 1U
#define CYBSP_RTC_10_MONTH_OFFSET (28U)
#define CYBSP_RTC_MONTH_OFFSET (24U)
#define CYBSP_RTC_10_DAY_OFFSET (20U)
#define CYBSP_RTC_DAY_OFFSET (16U)
#define CYBSP_RTC_1000_YEAR_OFFSET (12U)
#define CYBSP_RTC_100_YEAR_OFFSET (8U)
#define CYBSP_RTC_10_YEAR_OFFSET (4U)
#define CYBSP_RTC_YEAR_OFFSET (0U)
#define srss_0_clock_0_eco_0_ENABLED 1U
#define srss_0_clock_0_ext_0_ENABLED 1U
#define srss_0_clock_0_iho_0_ENABLED 1U
#define srss_0_clock_0_wco_0_ENABLED 1U
#define srss_0_clock_0_timerclk_0_ENABLED 1U
#define srss_0_clock_0_pilo_0_ENABLED 1U
#define srss_0_clock_0_lfclk_0_ENABLED 1U
#define CY_CFG_SYSCLK_CLKLF_FREQ_HZ 32768
#define CY_CFG_SYSCLK_CLKLF_SOURCE CY_SYSCLK_CLKLF_IN_PILO
#define srss_0_clock_0_pathmux_0_ENABLED 1U
#define srss_0_clock_0_pathmux_1_ENABLED 1U
#define srss_0_clock_0_pathmux_2_ENABLED 1U
#define srss_0_clock_0_pathmux_3_ENABLED 1U
#define srss_0_clock_0_pathmux_4_ENABLED 1U
#define srss_0_clock_0_pathmux_5_ENABLED 1U
#define srss_0_clock_0_bakclk_0_ENABLED 1U
#define srss_0_clock_0_pll250m_0_ENABLED 1U
#define srss_0_clock_0_pll250m_1_ENABLED 1U
#define srss_0_clock_0_pll500m_0_ENABLED 1U
#define srss_0_clock_0_hfclk_0_ENABLED 1U
#define CY_CFG_SYSCLK_CLKHF0 0UL
#define srss_0_clock_0_hfclk_1_ENABLED 1U
#define CY_CFG_SYSCLK_CLKHF1 1UL
#define srss_0_clock_0_hfclk_2_ENABLED 1U
#define CY_CFG_SYSCLK_CLKHF2 2UL
#define srss_0_clock_0_hfclk_3_ENABLED 1U
#define CY_CFG_SYSCLK_CLKHF3 3UL
#define srss_0_clock_0_hfclk_4_ENABLED 1U
#define CY_CFG_SYSCLK_CLKHF4 4UL
#define srss_0_clock_0_hfclk_5_ENABLED 1U
#define CY_CFG_SYSCLK_CLKHF5 5UL
#define srss_0_clock_0_hfclk_6_ENABLED 1U
#define CY_CFG_SYSCLK_CLKHF6 6UL
#define srss_0_clock_0_hfclk_7_ENABLED 1U
#define CY_CFG_SYSCLK_CLKHF7 7UL
#define srss_0_clock_0_hfclk_8_ENABLED 1U
#define CY_CFG_SYSCLK_CLKHF8 8UL
#define srss_0_clock_0_hfclk_9_ENABLED 1U
#define CY_CFG_SYSCLK_CLKHF9 9UL
#define srss_0_clock_0_hfclk_10_ENABLED 1U
#define CY_CFG_SYSCLK_CLKHF10 10UL
#define srss_0_clock_0_hfclk_11_ENABLED 1U
#define CY_CFG_SYSCLK_CLKHF11 11UL
#define srss_0_clock_0_hfclk_12_ENABLED 1U
#define CY_CFG_SYSCLK_CLKHF12 12UL
#define srss_0_clock_0_hfclk_13_ENABLED 1U
#define CY_CFG_SYSCLK_CLKHF13 13UL

extern const cy_stc_rtc_config_t CYBSP_RTC_config;
extern const cycfg_clkhf_config_t cycfg_hf0Config;
extern const cycfg_clkhf_config_t cycfg_hf1Config;
extern const cycfg_clkhf_config_t cycfg_hf2Config;
extern const cycfg_clkhf_config_t cycfg_hf3Config;
extern const cycfg_clkhf_config_t cycfg_hf4Config;
extern const cycfg_clkhf_config_t cycfg_hf5Config;
extern const cycfg_clkhf_config_t cycfg_hf6Config;
extern const cycfg_clkhf_config_t cycfg_hf7Config;
extern const cycfg_clkhf_config_t cycfg_hf8Config;
extern const cycfg_clkhf_config_t cycfg_hf9Config;
extern const cycfg_clkhf_config_t cycfg_hf10Config;
extern const cycfg_clkhf_config_t cycfg_hf11Config;
extern const cycfg_clkhf_config_t cycfg_hf12Config;
extern const cycfg_clkhf_config_t cycfg_hf13Config;

void init_cycfg_ns_clocks(void);
void init_cycfg_clocks(void);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* CYCFG_CLOCKS_H */
