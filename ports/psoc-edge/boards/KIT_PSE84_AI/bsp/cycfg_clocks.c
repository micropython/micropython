/*******************************************************************************
 * File Name: cycfg_clocks.c
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

#include "cycfg_clocks.h"

#define CY_CFG_SYSCLK_ECO_ERROR 1
#define CY_CFG_SYSCLK_ALTHF_ERROR 2
#define CY_CFG_SYSCLK_PLL_ERROR 3
#define CY_CFG_SYSCLK_FLL_ERROR 4
#define CY_CFG_SYSCLK_WCO_ERROR 5
#define CY_CFG_SYSCLK_LPECO_ERROR 6

#if !defined(COMPONENT_CM7) && !defined(COMPONENT_CM55)
#define CY_CFG_SYSCLK_ACTIVE_CORE_HF CY_CFG_SYSCLK_CLKHF0
#endif /* !defined(COMPONENT_CM7) && !defined(COMPONENT_CM55) */

#if defined(COMPONENT_CM7) || defined(COMPONENT_CM55)
#define CY_CFG_SYSCLK_ACTIVE_CORE_HF CY_CFG_SYSCLK_CLKHF1
#endif /* defined(COMPONENT_CM7) || defined(COMPONENT_CM55) */

#define CY_CFG_SYSCLK_FIRST_CLKPATH_DIRECT ((cy_en_clkhf_in_sources_t)CY_SRSS_NUM_PLL)
#define CY_CFG_SYSCLK_ECO_ENABLED 1
#define CY_CFG_SYSCLK_ECO_FREQ 17203200UL
#define CY_CFG_SYSCLK_ECO_GPIO_IN_PRT GPIO_PRT19
#define CY_CFG_SYSCLK_ECO_GPIO_IN_PIN 0
#define CY_CFG_SYSCLK_ECO_GPIO_OUT_PRT GPIO_PRT19
#define CY_CFG_SYSCLK_ECO_GPIO_OUT_PIN 1
#define CY_CFG_SYSCLK_EXTCLK_ENABLED 1
#define CY_CFG_SYSCLK_EXTCLK_FREQ 24000000UL
#define CY_CFG_SYSCLK_EXTCLK_GPIO_PRT GPIO_PRT7
#define CY_CFG_SYSCLK_EXTCLK_GPIO_PIN 4
#define CY_CFG_SYSCLK_EXTCLK_GPIO_HSIOM P7_4_SRSS_EXT_CLK
#define CY_CFG_SYSCLK_IHO_ENABLED 1
#define CY_CFG_SYSCLK_WCO_ENABLED 1
#define CY_CFG_SYSCLK_WCO_IN_PRT GPIO_PRT18
#define CY_CFG_SYSCLK_WCO_IN_PIN 1U
#define CY_CFG_SYSCLK_WCO_OUT_PRT GPIO_PRT18
#define CY_CFG_SYSCLK_WCO_OUT_PIN 0U
#define CY_CFG_SYSCLK_WCO_BYPASS CY_SYSCLK_WCO_NOT_BYPASSED
#define CY_CFG_SYSCLK_PILO_ENABLED 1
#define CY_CFG_SYSCLK_CLKLF_ENABLED 1
#define CY_CFG_SYSCLK_CLKPATH0_SOURCE CY_SYSCLK_CLKPATH_IN_EXT
#define CY_CFG_SYSCLK_CLKPATH1_SOURCE CY_SYSCLK_CLKPATH_IN_ECO
#define CY_CFG_SYSCLK_CLKPATH2_SOURCE CY_SYSCLK_CLKPATH_IN_IHO
#define CY_CFG_SYSCLK_CLKPATH3_SOURCE CY_SYSCLK_CLKPATH_IN_IHO
#define CY_CFG_SYSCLK_CLKPATH4_SOURCE CY_SYSCLK_CLKPATH_IN_EXT
#define CY_CFG_SYSCLK_CLKPATH5_SOURCE CY_SYSCLK_CLKPATH_IN_IHO
#define CY_CFG_SYSCLK_CLKBAK_ENABLED 1
#define CY_CFG_SYSCLK_CLKBAK_SOURCE CY_SYSCLK_BAK_IN_WCO
#define CY_CFG_SYSCLK_DPLL_LP0_ENABLED 1
#define CY_CFG_SYSCLK_DPLL_LP0_FEEDBACK_DIV 50
#define CY_CFG_SYSCLK_DPLL_LP0_REFERENCE_DIV 3
#define CY_CFG_SYSCLK_DPLL_LP0_OUTPUT_DIV 1
#define CY_CFG_SYSCLK_DPLL_LP0_FRAC_DIV 0
#define CY_CFG_SYSCLK_DPLL_LP0_FRAC_DITHER_EN false
#define CY_CFG_SYSCLK_DPLL_LP0_FRAC_EN true
#define CY_CFG_SYSCLK_DPLL_LP0_LF_MODE false
#define CY_CFG_SYSCLK_DPLL_LP0_OUTPUT_MODE CY_SYSCLK_FLLPLL_OUTPUT_AUTO
#define CY_CFG_SYSCLK_DPLL_LP0_OUTPUT_FREQ 400000000
#define CY_CFG_SYSCLK_DPLL_LP1_ENABLED 1
#define CY_CFG_SYSCLK_DPLL_LP1_FEEDBACK_DIV 34
#define CY_CFG_SYSCLK_DPLL_LP1_REFERENCE_DIV 3
#define CY_CFG_SYSCLK_DPLL_LP1_OUTPUT_DIV 4
#define CY_CFG_SYSCLK_DPLL_LP1_FRAC_DIV 4793490
#define CY_CFG_SYSCLK_DPLL_LP1_FRAC_DITHER_EN false
#define CY_CFG_SYSCLK_DPLL_LP1_FRAC_EN true
#define CY_CFG_SYSCLK_DPLL_LP1_LF_MODE false
#define CY_CFG_SYSCLK_DPLL_LP1_OUTPUT_MODE CY_SYSCLK_FLLPLL_OUTPUT_AUTO
#define CY_CFG_SYSCLK_DPLL_LP1_OUTPUT_FREQ 49151999
#define CY_CFG_SYSCLK_DPLL_HP0_ENABLED 1
#define CY_CFG_SYSCLK_DPLL_HP0_P_DIV 0
#define CY_CFG_SYSCLK_DPLL_HP0_N_DIV 11
#define CY_CFG_SYSCLK_DPLL_HP0_K_DIV 1
#define CY_CFG_SYSCLK_DPLL_HP0_FRAC_DIV 0
#define CY_CFG_SYSCLK_DPLL_HP0_OUTPUT_MODE CY_SYSCLK_FLLPLL_OUTPUT_AUTO
#define CY_CFG_SYSCLK_DPLL_HP0_OUTPUT_FREQ 300000000
#define CY_CFG_SYSCLK_CLKHF0_ENABLED 1
#define CY_CFG_SYSCLK_CLKHF0_DIVIDER CY_SYSCLK_CLKHF_DIVIDE_BY_2
#define CY_CFG_SYSCLK_CLKHF0_FREQ_MHZ 200UL
#define CY_CFG_SYSCLK_CLKHF0_CLKPATH CY_SYSCLK_CLKHF_IN_CLKPATH0
#define CY_CFG_SYSCLK_CLKHF0_CLKPATH_NUM 0UL
#define CY_CFG_SYSCLK_CLKHF1_ENABLED 1
#define CY_CFG_SYSCLK_CLKHF1_DIVIDER CY_SYSCLK_CLKHF_NO_DIVIDE
#define CY_CFG_SYSCLK_CLKHF1_FREQ_MHZ 400UL
#define CY_CFG_SYSCLK_CLKHF1_CLKPATH CY_SYSCLK_CLKHF_IN_CLKPATH0
#define CY_CFG_SYSCLK_CLKHF1_CLKPATH_NUM 0UL
#define CY_CFG_SYSCLK_CLKHF2_ENABLED 1
#define CY_CFG_SYSCLK_CLKHF2_DIVIDER CY_SYSCLK_CLKHF_NO_DIVIDE
#define CY_CFG_SYSCLK_CLKHF2_FREQ_MHZ 300UL
#define CY_CFG_SYSCLK_CLKHF2_CLKPATH CY_SYSCLK_CLKHF_IN_CLKPATH2
#define CY_CFG_SYSCLK_CLKHF2_CLKPATH_NUM 2UL
#define CY_CFG_SYSCLK_CLKHF3_ENABLED 1
#define CY_CFG_SYSCLK_CLKHF3_DIVIDER CY_SYSCLK_CLKHF_DIVIDE_BY_2
#define CY_CFG_SYSCLK_CLKHF3_FREQ_MHZ 200UL
#define CY_CFG_SYSCLK_CLKHF3_CLKPATH CY_SYSCLK_CLKHF_IN_CLKPATH0
#define CY_CFG_SYSCLK_CLKHF3_CLKPATH_NUM 0UL
#define CY_CFG_SYSCLK_CLKHF4_ENABLED 1
#define CY_CFG_SYSCLK_CLKHF4_DIVIDER CY_SYSCLK_CLKHF_NO_DIVIDE
#define CY_CFG_SYSCLK_CLKHF4_FREQ_MHZ 400UL
#define CY_CFG_SYSCLK_CLKHF4_CLKPATH CY_SYSCLK_CLKHF_IN_CLKPATH0
#define CY_CFG_SYSCLK_CLKHF4_CLKPATH_NUM 0UL
#define CY_CFG_SYSCLK_CLKHF5_ENABLED 1
#define CY_CFG_SYSCLK_CLKHF5_DIVIDER CY_SYSCLK_CLKHF_DIVIDE_BY_2
#define CY_CFG_SYSCLK_CLKHF5_FREQ_MHZ 200UL
#define CY_CFG_SYSCLK_CLKHF5_CLKPATH CY_SYSCLK_CLKHF_IN_CLKPATH0
#define CY_CFG_SYSCLK_CLKHF5_CLKPATH_NUM 0UL
#define CY_CFG_SYSCLK_CLKHF6_ENABLED 1
#define CY_CFG_SYSCLK_CLKHF6_DIVIDER CY_SYSCLK_CLKHF_DIVIDE_BY_2
#define CY_CFG_SYSCLK_CLKHF6_FREQ_MHZ 200UL
#define CY_CFG_SYSCLK_CLKHF6_CLKPATH CY_SYSCLK_CLKHF_IN_CLKPATH0
#define CY_CFG_SYSCLK_CLKHF6_CLKPATH_NUM 0UL
#define CY_CFG_SYSCLK_CLKHF7_ENABLED 1
#define CY_CFG_SYSCLK_CLKHF7_DIVIDER CY_SYSCLK_CLKHF_NO_DIVIDE
#define CY_CFG_SYSCLK_CLKHF7_FREQ_MHZ 49UL
#define CY_CFG_SYSCLK_CLKHF7_CLKPATH CY_SYSCLK_CLKHF_IN_CLKPATH1
#define CY_CFG_SYSCLK_CLKHF7_CLKPATH_NUM 1UL
#define CY_CFG_SYSCLK_CLKHF8_ENABLED 1
#define CY_CFG_SYSCLK_CLKHF8_DIVIDER CY_SYSCLK_CLKHF_DIVIDE_BY_8
#define CY_CFG_SYSCLK_CLKHF8_FREQ_MHZ 50UL
#define CY_CFG_SYSCLK_CLKHF8_CLKPATH CY_SYSCLK_CLKHF_IN_CLKPATH0
#define CY_CFG_SYSCLK_CLKHF8_CLKPATH_NUM 0UL
#define CY_CFG_SYSCLK_CLKHF9_ENABLED 1
#define CY_CFG_SYSCLK_CLKHF9_DIVIDER CY_SYSCLK_CLKHF_DIVIDE_BY_5
#define CY_CFG_SYSCLK_CLKHF9_FREQ_MHZ 80UL
#define CY_CFG_SYSCLK_CLKHF9_CLKPATH CY_SYSCLK_CLKHF_IN_CLKPATH0
#define CY_CFG_SYSCLK_CLKHF9_CLKPATH_NUM 0UL
#define CY_CFG_SYSCLK_CLKHF10_ENABLED 1
#define CY_CFG_SYSCLK_CLKHF10_DIVIDER CY_SYSCLK_CLKHF_DIVIDE_BY_4
#define CY_CFG_SYSCLK_CLKHF10_FREQ_MHZ 100UL
#define CY_CFG_SYSCLK_CLKHF10_CLKPATH CY_SYSCLK_CLKHF_IN_CLKPATH0
#define CY_CFG_SYSCLK_CLKHF10_CLKPATH_NUM 0UL
#define CY_CFG_SYSCLK_CLKHF11_ENABLED 1
#define CY_CFG_SYSCLK_CLKHF11_DIVIDER CY_SYSCLK_CLKHF_DIVIDE_BY_2
#define CY_CFG_SYSCLK_CLKHF11_FREQ_MHZ 200UL
#define CY_CFG_SYSCLK_CLKHF11_CLKPATH CY_SYSCLK_CLKHF_IN_CLKPATH0
#define CY_CFG_SYSCLK_CLKHF11_CLKPATH_NUM 0UL
#define CY_CFG_SYSCLK_CLKHF12_ENABLED 1
#define CY_CFG_SYSCLK_CLKHF12_DIVIDER CY_SYSCLK_CLKHF_NO_DIVIDE
#define CY_CFG_SYSCLK_CLKHF12_FREQ_MHZ 24UL
#define CY_CFG_SYSCLK_CLKHF12_CLKPATH CY_SYSCLK_CLKHF_IN_CLKPATH4
#define CY_CFG_SYSCLK_CLKHF12_CLKPATH_NUM 4UL
#define CY_CFG_SYSCLK_CLKHF13_ENABLED 1
#define CY_CFG_SYSCLK_CLKHF13_DIVIDER CY_SYSCLK_CLKHF_DIVIDE_BY_4
#define CY_CFG_SYSCLK_CLKHF13_FREQ_MHZ 100UL
#define CY_CFG_SYSCLK_CLKHF13_CLKPATH CY_SYSCLK_CLKHF_IN_CLKPATH0
#define CY_CFG_SYSCLK_CLKHF13_CLKPATH_NUM 0UL

static const cycfg_clkhf_config_t * cycfg_clkhf_configs[] =
{
    &cycfg_hf0Config, 
    &cycfg_hf1Config, 
    &cycfg_hf2Config, 
    &cycfg_hf3Config, 
    &cycfg_hf4Config, 
    &cycfg_hf5Config, 
    &cycfg_hf6Config, 
    &cycfg_hf7Config, 
    &cycfg_hf8Config, 
    &cycfg_hf9Config, 
    &cycfg_hf10Config, 
    &cycfg_hf11Config, 
    &cycfg_hf12Config, 
    &cycfg_hf13Config, 
};
static const cycfg_clkhf_csv_config_t * cycfg_clkhf_csv_configs[] =
{
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
};
static const cy_en_clkpath_in_sources_t cycfg_clkpath_configs[] =
{
    CY_CFG_SYSCLK_CLKPATH0_SOURCE, 
    CY_CFG_SYSCLK_CLKPATH1_SOURCE, 
    CY_CFG_SYSCLK_CLKPATH2_SOURCE, 
    CY_CFG_SYSCLK_CLKPATH3_SOURCE, 
    CY_CFG_SYSCLK_CLKPATH4_SOURCE, 
    CY_CFG_SYSCLK_CLKPATH5_SOURCE, 
};
const cy_stc_rtc_config_t CYBSP_RTC_config =
{
    .sec = 0U,
    .min = 0U,
    .hour = 12U,
    .amPm = CY_RTC_AM,
    .hrFormat = CY_RTC_24_HOURS,
    .dayOfWeek = CY_RTC_MONDAY,
    .date = 1U,
    .month = CY_RTC_JANUARY,
    .year = 24U,
};
static const cy_stc_clk_eco_config_t srss_0_clock_0_eco_0_config =
{
    .ecoClkfreq = 17203200U,
    .ecoCtrim = 6U,
    .ecoGtrim = 2U,
    .ecoIboost = 1U,
};
static cy_stc_dpll_lp_config_t srss_0_clock_0_pll250m_0_lp_pllConfig =
{
    .feedbackDiv = 50,
    .referenceDiv = 3,
    .outputDiv = 1,
    .pllDcoMode = true,
    .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
    .fracDiv = 0,
    .fracDitherEn = false,
    .fracEn = true,
    .dcoCode = 0xFU,
    .kiInt = 0xAU,
    .kiFrac = 0xBU,
    .kiSscg = 0x7U,
    .kpInt = 0x8U,
    .kpFrac = 0x9U,
    .kpSscg = 0x7U,
};
static cy_stc_pll_manual_config_t srss_0_clock_0_pll250m_0_pllConfig =
{
    .lpPllCfg = &srss_0_clock_0_pll250m_0_lp_pllConfig,
};
static cy_stc_dpll_lp_config_t srss_0_clock_0_pll250m_1_lp_pllConfig =
{
    .feedbackDiv = 34,
    .referenceDiv = 3,
    .outputDiv = 4,
    .pllDcoMode = false,
    .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
    .fracDiv = 4793490,
    .fracDitherEn = false,
    .fracEn = true,
    .dcoCode = 0xFU,
    .kiInt = 0xAU,
    .kiFrac = 0xBU,
    .kiSscg = 0x7U,
    .kpInt = 0x8U,
    .kpFrac = 0x9U,
    .kpSscg = 0x7U,
};
static cy_stc_pll_manual_config_t srss_0_clock_0_pll250m_1_pllConfig =
{
    .lpPllCfg = &srss_0_clock_0_pll250m_1_lp_pllConfig,
};
static cy_stc_dpll_hp_config_t srss_0_clock_0_pll500m_0_hp_pllConfig =
{
    .pDiv = 0,
    .nDiv = 11,
    .kDiv = 1,
    .nDivFract = 0,
    .freqModeSel = (cy_en_wait_mode_select_t)7,
    .ivrTrim = 0x8U,
    .clkrSel = 0x1U,
    .alphaCoarse = 0xCU,
    .betaCoarse = 0x5U,
    .flockThresh = 3,
    .flockWait = 0x6U,
    .flockLkThres = 0x7U,
    .flockLkWait = 0x4U,
    .alphaExt = 0x14U,
    .betaExt = 20,
    .lfEn = 0x1U,
    .dcEn = 0x1U,
    .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
};
static cy_stc_pll_manual_config_t srss_0_clock_0_pll500m_0_pllConfig =
{
    .hpPllCfg = &srss_0_clock_0_pll500m_0_hp_pllConfig,
};
const cycfg_clkhf_config_t cycfg_hf0Config =
{
    .source = CY_CFG_SYSCLK_CLKHF0_CLKPATH,
    .divider = CY_CFG_SYSCLK_CLKHF0_DIVIDER,
};
const cycfg_clkhf_config_t cycfg_hf1Config =
{
    .source = CY_CFG_SYSCLK_CLKHF1_CLKPATH,
    .divider = CY_CFG_SYSCLK_CLKHF1_DIVIDER,
};
const cycfg_clkhf_config_t cycfg_hf2Config =
{
    .source = CY_CFG_SYSCLK_CLKHF2_CLKPATH,
    .divider = CY_CFG_SYSCLK_CLKHF2_DIVIDER,
};
const cycfg_clkhf_config_t cycfg_hf3Config =
{
    .source = CY_CFG_SYSCLK_CLKHF3_CLKPATH,
    .divider = CY_CFG_SYSCLK_CLKHF3_DIVIDER,
};
const cycfg_clkhf_config_t cycfg_hf4Config =
{
    .source = CY_CFG_SYSCLK_CLKHF4_CLKPATH,
    .divider = CY_CFG_SYSCLK_CLKHF4_DIVIDER,
};
const cycfg_clkhf_config_t cycfg_hf5Config =
{
    .source = CY_CFG_SYSCLK_CLKHF5_CLKPATH,
    .divider = CY_CFG_SYSCLK_CLKHF5_DIVIDER,
};
const cycfg_clkhf_config_t cycfg_hf6Config =
{
    .source = CY_CFG_SYSCLK_CLKHF6_CLKPATH,
    .divider = CY_CFG_SYSCLK_CLKHF6_DIVIDER,
};
const cycfg_clkhf_config_t cycfg_hf7Config =
{
    .source = CY_CFG_SYSCLK_CLKHF7_CLKPATH,
    .divider = CY_CFG_SYSCLK_CLKHF7_DIVIDER,
};
const cycfg_clkhf_config_t cycfg_hf8Config =
{
    .source = CY_CFG_SYSCLK_CLKHF8_CLKPATH,
    .divider = CY_CFG_SYSCLK_CLKHF8_DIVIDER,
};
const cycfg_clkhf_config_t cycfg_hf9Config =
{
    .source = CY_CFG_SYSCLK_CLKHF9_CLKPATH,
    .divider = CY_CFG_SYSCLK_CLKHF9_DIVIDER,
};
const cycfg_clkhf_config_t cycfg_hf10Config =
{
    .source = CY_CFG_SYSCLK_CLKHF10_CLKPATH,
    .divider = CY_CFG_SYSCLK_CLKHF10_DIVIDER,
};
const cycfg_clkhf_config_t cycfg_hf11Config =
{
    .source = CY_CFG_SYSCLK_CLKHF11_CLKPATH,
    .divider = CY_CFG_SYSCLK_CLKHF11_DIVIDER,
};
const cycfg_clkhf_config_t cycfg_hf12Config =
{
    .source = CY_CFG_SYSCLK_CLKHF12_CLKPATH,
    .divider = CY_CFG_SYSCLK_CLKHF12_DIVIDER,
};
const cycfg_clkhf_config_t cycfg_hf13Config =
{
    .source = CY_CFG_SYSCLK_CLKHF13_CLKPATH,
    .divider = CY_CFG_SYSCLK_CLKHF13_DIVIDER,
};

static void cycfg_SysClk_ClkHfInit(uint32_t clkHf, cy_en_clkhf_in_sources_t source, cy_en_clkhf_dividers_t divider);
static void cycfg_SysClk_ClkHfCsvInit(uint32_t clkHf, cy_en_clkhf_csv_ref_clk_t refClk, uint32_t accuracy, cy_en_clkhf_csv_action_t action);
__WEAK void __NO_RETURN cycfg_ClockStartupError(uint32_t error);
__STATIC_INLINE void Cy_SysClk_EcoInit(void);
__STATIC_INLINE void Cy_SysClk_ExtClkInit(void);
__STATIC_INLINE void Cy_SysClk_IhoInit(void);
__STATIC_INLINE void Cy_SysClk_WcoInit(void);
__STATIC_INLINE void Cy_SysClk_PiloInit(void);
__STATIC_INLINE void Cy_SysClk_ClkLfInit(void);
__STATIC_INLINE void Cy_SysClk_ClkBakInit(void);
__STATIC_INLINE void Cy_SysClk_Dpll_Lp0_Init(void);
__STATIC_INLINE void Cy_SysClk_Dpll_Lp1_Init(void);
__STATIC_INLINE void Cy_SysClk_Dpll_Hp0_Init(void);

static void cycfg_SysClk_ClkHfInit(uint32_t clkHf, cy_en_clkhf_in_sources_t source, cy_en_clkhf_dividers_t divider)
{
    Cy_SysClk_ClkHfSetDivider(clkHf, divider);
    Cy_SysClk_ClkHfSetSource(clkHf, source);
    if (clkHf != 0U)
    {
        Cy_SysClk_ClkHfEnable(clkHf);
    }
}
static void cycfg_SysClk_ClkHfCsvInit(uint32_t clkHf, cy_en_clkhf_csv_ref_clk_t refClk, uint32_t accuracy, cy_en_clkhf_csv_action_t action)
{
    const cy_stc_clkhf_csv_hf_config_t csvConfig = { clkHf, refClk };
    Cy_SysClk_ClkHfCsvConfigure(&csvConfig, accuracy);
    Cy_SysClk_ClkHfCsvAction(clkHf, action);
    Cy_SysClk_ClkHfCsvEnable(clkHf);
}
__WEAK void __NO_RETURN cycfg_ClockStartupError(uint32_t error)
{
    (void)error; /* Suppress the compiler warning */
    while(1);
}
void init_cycfg_ns_clocks(void)
{
    #if (!defined(COMPONENT_SECURE_DEVICE))
    /* Update global variables used in sysclock driver */
    #ifdef CY_CFG_SYSCLK_ECO_ENABLED
        Cy_SysClk_EcoSetFrequency(CY_CFG_SYSCLK_ECO_FREQ);
    #endif
    #ifdef CY_CFG_SYSCLK_EXTCLK_ENABLED
        Cy_SysClk_ExtClkSetFrequency(CY_CFG_SYSCLK_EXTCLK_FREQ);
    #endif
    
    /* Update global variables used by Cy_SysLib_Delay functions */
    uint32_t clk_hf0_freq_hz = 0;
    #if (CY_CFG_SYSCLK_CLKHF0_FREQ_MHZ)
    clk_hf0_freq_hz = CY_CFG_SYSCLK_CLKHF0_FREQ_MHZ * 1000000;
    #endif /* CY_CFG_SYSCLK_CLKHF0_FREQ_MHZ */
    uint32_t clk_hf1_freq_hz = 0;
    #if (CY_CFG_SYSCLK_CLKHF1_FREQ_MHZ)
    clk_hf1_freq_hz = CY_CFG_SYSCLK_CLKHF1_FREQ_MHZ * 1000000;
    #endif /* CY_CFG_SYSCLK_CLKHF1_FREQ_MHZ */
    SystemCoreClockSetup((CY_SYSCLK_CLK_CORE_HF_PATH_NUM == 0 ? clk_hf0_freq_hz : clk_hf1_freq_hz), clk_hf0_freq_hz);
    
    #endif /* (!defined(COMPONENT_SECURE_DEVICE)) */
}
__STATIC_INLINE void Cy_SysClk_EcoInit(void)
{
    (void)Cy_GPIO_Pin_FastInit(GPIO_PRT19, 0, CY_GPIO_DM_ANALOG, 0UL, HSIOM_SEL_GPIO);
    (void)Cy_GPIO_Pin_FastInit(GPIO_PRT19, 1, CY_GPIO_DM_ANALOG, 0UL, HSIOM_SEL_GPIO);
    if (CY_SYSCLK_BAD_PARAM == Cy_SysClk_EcoManualConfigure(&srss_0_clock_0_eco_0_config))
    {
        cycfg_ClockStartupError(CY_CFG_SYSCLK_ECO_ERROR);
    }
    if (CY_SYSCLK_TIMEOUT == Cy_SysClk_EcoEnable(3000UL))
    {
        cycfg_ClockStartupError(CY_CFG_SYSCLK_ECO_ERROR);
    }
    Cy_SysClk_EcoSetFrequency(CY_CFG_SYSCLK_ECO_FREQ);
}
__STATIC_INLINE void Cy_SysClk_ExtClkInit(void)
{
    (void)Cy_GPIO_Pin_SecFastInit(GPIO_PRT7, 4, CY_GPIO_DM_HIGHZ, 0UL, P7_4_SRSS_EXT_CLK);
    Cy_SysClk_ExtClkSetFrequency(CY_CFG_SYSCLK_EXTCLK_FREQ);
}
__STATIC_INLINE void Cy_SysClk_IhoInit(void)
{
    Cy_SysClk_IhoEnable();
}
__STATIC_INLINE void Cy_SysClk_WcoInit(void)
{
    (void)Cy_GPIO_Pin_FastInit(GPIO_PRT18, 1U, 0x00U, 0x00U, HSIOM_SEL_GPIO);
    (void)Cy_GPIO_Pin_FastInit(GPIO_PRT18, 0U, 0x00U, 0x00U, HSIOM_SEL_GPIO);
    if (CY_SYSCLK_SUCCESS != Cy_SysClk_WcoEnable(1000000UL))
    {
        cycfg_ClockStartupError(CY_CFG_SYSCLK_WCO_ERROR);
    }
}
__STATIC_INLINE void Cy_SysClk_PiloInit(void)
{
    Cy_SysClk_PiloEnable();
}
__STATIC_INLINE void Cy_SysClk_ClkLfInit(void)
{
    /* The WDT is unlocked in the default startup code */
    Cy_SysClk_ClkLfSetSource(CY_SYSCLK_CLKLF_IN_PILO);
}
__STATIC_INLINE void Cy_SysClk_ClkBakInit(void)
{
    Cy_SysClk_ClkBakSetSource(CY_SYSCLK_BAK_IN_WCO);
}
__STATIC_INLINE void Cy_SysClk_Dpll_Lp0_Init(void)
{
    #if !defined (CY_PDL_TZ_ENABLED)
    if (Cy_SysClk_PllIsEnabled(SRSS_DPLL_LP_0_PATH_NUM))
        return;
    #endif
    
    Cy_SysClk_PllDisable(SRSS_DPLL_LP_0_PATH_NUM);
    if (CY_SYSCLK_SUCCESS != Cy_SysClk_PllManualConfigure(SRSS_DPLL_LP_0_PATH_NUM, &srss_0_clock_0_pll250m_0_pllConfig))
    {
        cycfg_ClockStartupError(CY_CFG_SYSCLK_PLL_ERROR);
    }
    
    #if (defined (CY_IP_MXS22SRSS_VERSION) && defined (CY_IP_MXS22SRSS_VERSION)) && \
       ((CY_IP_MXS22SRSS_VERSION == 1) && (CY_IP_MXS22SRSS_VERSION_MINOR == 0))
        /* The workaround for devices with MXS22SRSS block 1.0 */
        uint32_t clkHfMask = Cy_SysClk_ClkHfGetMaskOnPath((cy_en_clkhf_in_sources_t)SRSS_DPLL_LP_0_PATH_NUM);
        if(clkHfMask)
        {
            Cy_SysClk_ClkHfEnableDirectMuxWithMask(clkHfMask, true);
        }
    #endif /* ((CY_IP_MXS22SRSS_VERSION == 1) && (CY_IP_MXS22SRSS_VERSION_MINOR == 0)) */
    
    if (CY_SYSCLK_SUCCESS != Cy_SysClk_PllEnable(SRSS_DPLL_LP_0_PATH_NUM, 10000u))
    {
        cycfg_ClockStartupError(CY_CFG_SYSCLK_PLL_ERROR);
    }
    else
    {
        /* The workaround for devices with MXS22SRSS block 1.0 */
    #if (defined (CY_IP_MXS22SRSS_VERSION) && defined (CY_IP_MXS22SRSS_VERSION)) && \
       ((CY_IP_MXS22SRSS_VERSION == 1) && (CY_IP_MXS22SRSS_VERSION_MINOR == 0))
        Cy_SysLib_DelayUs(SRSS_DPLL_LP_INIT_DELAY_USEC);
        Cy_SysClk_ClkHfEnableDirectMuxWithMask(clkHfMask, false);
    #endif /* ((CY_IP_MXS22SRSS_VERSION == 1) && (CY_IP_MXS22SRSS_VERSION_MINOR == 0)) */
    }
}
__STATIC_INLINE void Cy_SysClk_Dpll_Lp1_Init(void)
{
    #if !defined (CY_PDL_TZ_ENABLED)
    if (Cy_SysClk_PllIsEnabled(SRSS_DPLL_LP_1_PATH_NUM))
        return;
    #endif
    
    Cy_SysClk_PllDisable(SRSS_DPLL_LP_1_PATH_NUM);
    if (CY_SYSCLK_SUCCESS != Cy_SysClk_PllManualConfigure(SRSS_DPLL_LP_1_PATH_NUM, &srss_0_clock_0_pll250m_1_pllConfig))
    {
        cycfg_ClockStartupError(CY_CFG_SYSCLK_PLL_ERROR);
    }
    
    #if (defined (CY_IP_MXS22SRSS_VERSION) && defined (CY_IP_MXS22SRSS_VERSION)) && \
       ((CY_IP_MXS22SRSS_VERSION == 1) && (CY_IP_MXS22SRSS_VERSION_MINOR == 0))
        /* The workaround for devices with MXS22SRSS block 1.0 */
        uint32_t clkHfMask = Cy_SysClk_ClkHfGetMaskOnPath((cy_en_clkhf_in_sources_t)SRSS_DPLL_LP_1_PATH_NUM);
        if(clkHfMask)
        {
            Cy_SysClk_ClkHfEnableDirectMuxWithMask(clkHfMask, true);
        }
    #endif /* ((CY_IP_MXS22SRSS_VERSION == 1) && (CY_IP_MXS22SRSS_VERSION_MINOR == 0)) */
    
    if (CY_SYSCLK_SUCCESS != Cy_SysClk_PllEnable(SRSS_DPLL_LP_1_PATH_NUM, 10000u))
    {
        cycfg_ClockStartupError(CY_CFG_SYSCLK_PLL_ERROR);
    }
    else
    {
        /* The workaround for devices with MXS22SRSS block 1.0 */
    #if (defined (CY_IP_MXS22SRSS_VERSION) && defined (CY_IP_MXS22SRSS_VERSION)) && \
       ((CY_IP_MXS22SRSS_VERSION == 1) && (CY_IP_MXS22SRSS_VERSION_MINOR == 0))
        Cy_SysLib_DelayUs(SRSS_DPLL_LP_INIT_DELAY_USEC);
        Cy_SysClk_ClkHfEnableDirectMuxWithMask(clkHfMask, false);
    #endif /* ((CY_IP_MXS22SRSS_VERSION == 1) && (CY_IP_MXS22SRSS_VERSION_MINOR == 0)) */
    }
}
__STATIC_INLINE void Cy_SysClk_Dpll_Hp0_Init(void)
{
    #if !defined (CY_PDL_TZ_ENABLED)
    if (Cy_SysClk_PllIsEnabled(SRSS_DPLL_HP_0_PATH_NUM))
        return;
    #endif
    Cy_SysClk_PllDisable(SRSS_DPLL_HP_0_PATH_NUM);
    if (CY_SYSCLK_SUCCESS != Cy_SysClk_PllManualConfigure(SRSS_DPLL_HP_0_PATH_NUM, &srss_0_clock_0_pll500m_0_pllConfig))
    {
        cycfg_ClockStartupError(CY_CFG_SYSCLK_PLL_ERROR);
    }
    if (CY_SYSCLK_SUCCESS != Cy_SysClk_PllEnable(SRSS_DPLL_HP_0_PATH_NUM, 10000u))
    {
        cycfg_ClockStartupError(CY_CFG_SYSCLK_PLL_ERROR);
    }
}
void init_cycfg_clocks(void)
{
    #if (CY_CFG_PWR_VBACKUP_USING_VDDD)
        if (0u == Cy_SysLib_GetResetReason() /* POR, XRES, or BOD */)
        {
            Cy_SysLib_ResetBackupDomain();
        }
    #endif /* CY_CFG_PWR_VBACKUP_USING_VDDD */
    
    /* Set up a temporary fail-safe bypass for the IHO/IMO to the active core */
    Cy_SysClk_IhoInit();
    Cy_SysClk_ClkPathSetSource(CY_CFG_SYSCLK_FIRST_CLKPATH_DIRECT, CY_SYSCLK_CLKPATH_IN_IHO);
    Cy_SysClk_ClkHfSetSource(CY_CFG_SYSCLK_ACTIVE_CORE_HF, CY_CFG_SYSCLK_FIRST_CLKPATH_DIRECT);
    Cy_SysClk_ClkHfSetDivider(CY_CFG_SYSCLK_ACTIVE_CORE_HF, CY_SYSCLK_CLKHF_NO_DIVIDE);
    /* Reset and configure platform clocks */
    Cy_WDT_Unlock(); /* Unlock WDT to be able to modify LFCLK registers */
    for (uint32_t pll = (CY_SRSS_NUM_PLL - 1); pll > 0UL; --pll) /* PLL 1 is the first PLL. 0 is invalid. */
    {
        (void)Cy_SysClk_PllDisable(pll);
    }
    
    /* Enable all source clocks */
    Cy_SysClk_PiloInit();
    Cy_SysClk_WcoInit();
    Cy_SysClk_EcoInit();
    Cy_SysClk_ClkLfInit();
    Cy_SysClk_ExtClkInit();
    
    /* Configure the path clocks, except for the fail-safe bypass for the IHO/IMO */
    for (uint32_t i = 0; i < CY_SRSS_NUM_CLKPATH; i++)
    {
        if (i != CY_CFG_SYSCLK_FIRST_CLKPATH_DIRECT)
        {
            Cy_SysClk_ClkPathSetSource(i, cycfg_clkpath_configs[i]);
        }
    }
    
    /* Configure PLL clocks */
    
    /* Configure the HF clocks, excluding the HF clock of the active core */
    for (uint32_t i = 0U; i < CY_SRSS_NUM_HFROOT; i++)
    {
        if (i != CY_CFG_SYSCLK_ACTIVE_CORE_HF)
        {
            const cycfg_clkhf_config_t *hfConfig = cycfg_clkhf_configs[i];
            if (hfConfig != NULL)
            {
                cycfg_SysClk_ClkHfInit(i, hfConfig->source, hfConfig->divider);
            }
        }
    }
    /* Configure miscellaneous clocks */
    Cy_SysClk_ClkBakInit();
    Cy_SysClk_Dpll_Hp0_Init();
    #ifdef UPDATE_DPLL_LP_TRIM_VALUES
        /* Workaround: update DPLL_LP trim values */
        CY_SET_REG32(0x52403218,0x921F190A); /* DPLL_LP0_TEST3 */
        CY_SET_REG32(0x5240321C,0x08100000); /* DPLL_LP0_TEST4 */
    #endif
    Cy_SysClk_Dpll_Lp0_Init();
    #ifdef UPDATE_DPLL_LP_TRIM_VALUES
        /* Workaround: update DPLL_LP trim values */
        CY_SET_REG32(0x52403238,0x921F190A); /* DPLL_LP1_TEST3 */
        CY_SET_REG32(0x5240323C,0x08100000); /* DPLL_LP1_TEST4 */
    #endif
    Cy_SysClk_Dpll_Lp1_Init();
    /* Configure HF0 clock and disable any unused HF clocks */
    for (uint32_t i = 0U; i < CY_SRSS_NUM_HFROOT; i++)
    {
        const cycfg_clkhf_config_t *hfConfig = cycfg_clkhf_configs[i];
        if (hfConfig != NULL)
        {
            if (i == CY_CFG_SYSCLK_ACTIVE_CORE_HF)
            {
                cycfg_SysClk_ClkHfInit(i, hfConfig->source, hfConfig->divider);
            }
        }
        else
        {
            (void)Cy_SysClk_ClkHfDisable(i);
        }
    }
    /* Configure the path clocks used for the fail-safe bypass for the IHO/IMO */
    Cy_SysClk_ClkPathSetSource(CY_CFG_SYSCLK_FIRST_CLKPATH_DIRECT, cycfg_clkpath_configs[CY_CFG_SYSCLK_FIRST_CLKPATH_DIRECT]);
    #ifdef CY_CFG_SYSCLK_CLKALTSYSTICK_ENABLED
        Cy_SysClk_ClkAltSysTickInit();
    #endif
    
    /* Configure default enabled clocks */
    /* Disable unused clock sources */
    /* Wait 4 ILO cycles in case of unfinished CLKLF clock source transition */
    Cy_SysLib_DelayUs(200U);
    
    /* Update System configuration based on clock configuration */
    SystemCoreClockUpdate();
    
    /* Configure clock supervisors */
    for (uint32_t i = 0U; i < CY_SRSS_NUM_HFROOT; i++)
    {
        const cycfg_clkhf_csv_config_t *hfCsvConfig = cycfg_clkhf_csv_configs[i];
        if (hfCsvConfig != NULL)
        {
            cycfg_SysClk_ClkHfCsvInit(i, hfCsvConfig->refClk, hfCsvConfig->accuracy, hfCsvConfig->action);
        }
        else
        {
            Cy_SysClk_ClkHfCsvDisable(i);
        }
    }
    
    
}
