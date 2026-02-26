/*******************************************************************************
 * File Name: cycfg_system.h
 *
 * Description:
 * System configuration
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

#if !defined(CYCFG_SYSTEM_H)
#define CYCFG_SYSTEM_H

#include "cycfg_notices.h"
#include "cy_syspm.h"
#include "system_edge.h"
#include "cy_sysclk.h"
#include "cy_syspm_pdcm.h"
#include "cy_device.h"
#include "cy_mpc.h"
#include "cy_ppc.h"
#include "cy_cmsis_utils.h"

#if defined(CY_PDL_TZ_ENABLED)
#include "cy_cmsis_utils.h"
#endif /* defined(CY_PDL_TZ_ENABLED) */

#if defined (COMPONENT_SECURE_DEVICE) && defined(COMPONENT_MW_MTB_SRF)
#include "mtb_srf.h"
#endif /* defined (COMPONENT_SECURE_DEVICE) && defined(COMPONENT_MW_MTB_SRF) */

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#define srss_0_power_0_ENABLED 1U
#define CY_CFG_PWR_MODE_LP 0x01UL
#define CY_CFG_PWR_MODE_ULP 0x02UL
#define CY_CFG_PWR_MODE_HP 0x03UL
#define CY_CFG_PWR_MODE_ACTIVE 0x04UL
#define CY_CFG_PWR_MODE_SLEEP 0x08UL
#define CY_CFG_PWR_MODE_DEEPSLEEP 0x10UL
#define CY_CFG_PWR_MODE_DEEPSLEEP_RAM 0x11UL
#define CY_CFG_PWR_MODE_DEEPSLEEP_OFF 0x12UL
#define CY_CFG_PWR_SYS_IDLE_MODE CY_CFG_PWR_MODE_DEEPSLEEP
#define CY_CFG_PWR_DEEPSLEEP_LATENCY 20UL
#define CY_CFG_PWR_SYS_ACTIVE_MODE CY_CFG_PWR_MODE_HP
#define CY_CFG_PWR_SYS_ACTIVE_PROFILE CY_CFG_PWR_MODE_HP
#define CY_CFG_PWR_VDDA_MV 1800
#define CY_CFG_PWR_VDDD_MV 1800
#define CY_CFG_PWR_VDDIO0_MV 1800
#define CY_CFG_PWR_VDDIO1_MV 1800
#define CY_CFG_PWR_CBUCK_VOLT CY_SYSPM_CORE_BUCK_VOLTAGE_0_90V
#define CY_CFG_PWR_CBUCK_MODE CY_SYSPM_CORE_BUCK_MODE_HP
#define CY_CFG_PWR_SRAMLDO_VOLT CY_SYSPM_SRAMLDO_VOLTAGE_0_80V
#define CY_CFG_PWR_PD1_DOMAIN 1
#define CY_CFG_PWR_PPU_MAIN PPU_V1_MODE_FULL_RET
#define CY_CFG_PWR_PPU_PD1 PPU_V1_MODE_FULL_RET
#define CY_CFG_PWR_PPU_SRAM0 PPU_V1_MODE_MEM_RET
#define CY_CFG_PWR_PPU_SRAM1 PPU_V1_MODE_MEM_RET
#define CY_CFG_PWR_PPU_SYSCPU PPU_V1_MODE_FULL_RET
#define CY_CFG_PWR_PPU_APPCPUSS PPU_V1_MODE_FULL_RET
#define CY_CFG_PWR_PPU_APPCPU PPU_V1_MODE_FULL_RET
#define CY_CFG_PWR_PPU_SOCMEM PPU_V1_MODE_MEM_RET
#define CY_CFG_PWR_PPU_U55 PPU_V1_MODE_ON
#define M33S_ENABLED 1U
#define M33_ENABLED 1U
#define M55_ENABLED 1U
#define M33NSC_ENABLED 1U
#define M33_M55_ENABLED 1U
#define reserved_ENABLED 1U
#define vres_0_protection_0_mpc_0_ENABLED 1U
#define M33S_UNIFIED_MPC_DOMAIN_IDX 0U
#define M33_UNIFIED_MPC_DOMAIN_IDX 1U
#define M55_UNIFIED_MPC_DOMAIN_IDX 2U
#define M33NSC_UNIFIED_MPC_DOMAIN_IDX 3U
#define M33_M55_UNIFIED_MPC_DOMAIN_IDX 4U
#define Cy_MPC_Init init_cycfg_mpc
#define PPC_PC_MASK_ALL_ACCESS 0xFFU
#define Cy_PPC0_Init init_cycfg_ppc0
#define Cy_PPC1_Init init_cycfg_ppc1
#define m33syscpuss_0_cm33_0_sau_0_ENABLED 1U
#define m55appcpuss_0_cm55_0_mpu_ns_0_ENABLED 1U
#define mxrramc_0_mpc_0_RESPONSE CY_MPC_BUS_ERR
#define mxrramc_0_mpc_0_REGION_COUNT 4U
#define mxsramc_0_mpc_0_RESPONSE CY_MPC_BUS_ERR
#define mxsramc_0_mpc_0_REGION_COUNT 3U
#define mxsramc_1_mpc_0_RESPONSE CY_MPC_BUS_ERR
#define mxsramc_1_mpc_0_REGION_COUNT 2U
#define smif_0_mpc_0_RESPONSE CY_MPC_BUS_ERR
#define smif_0_mpc_0_REGION_COUNT 6U
#define smif_1_mpc_0_RESPONSE CY_MPC_RZWI
#define smif_1_mpc_0_REGION_COUNT 0U
#define socmem_0_mpc_0_RESPONSE CY_MPC_BUS_ERR
#define socmem_0_mpc_0_REGION_COUNT 2U

#if defined (CY_PDL_TZ_ENABLED)
extern const cy_stc_mpc_rot_cfg_t M33S_mpc_cfg[];
extern const cy_stc_mpc_rot_cfg_t M33_mpc_cfg[];
extern const cy_stc_mpc_rot_cfg_t M55_mpc_cfg[];
extern const cy_stc_mpc_rot_cfg_t M33NSC_mpc_cfg[];
extern const cy_stc_mpc_rot_cfg_t M33_M55_mpc_cfg[];
extern const cy_stc_mpc_regions_t M33S_mpc_regions[];
extern const cy_stc_mpc_regions_t M33_mpc_regions[];
extern const cy_stc_mpc_regions_t M55_mpc_regions[];
extern const cy_stc_mpc_regions_t M33NSC_mpc_regions[];
extern const cy_stc_mpc_regions_t M33_M55_mpc_regions[];
extern const cy_stc_mpc_resp_cfg_t cy_response_mpcs[];
extern const size_t cy_response_mpcs_count;
extern const cy_stc_mpc_unified_t unified_mpc_domains[];
extern const size_t unified_mpc_domains_count;
#endif /* defined (CY_PDL_TZ_ENABLED) */

#if defined (COMPONENT_SECURE_DEVICE) && defined(COMPONENT_MW_MTB_SRF)
extern const mtb_srf_memory_protection_s_t mtb_srf_memory_protection_s[];
extern const uint8_t mtb_srf_protection_range_s_count;
#endif /* defined (COMPONENT_SECURE_DEVICE) && defined(COMPONENT_MW_MTB_SRF) */

extern const cy_stc_ppc_attribute_t cycfg_unused_ppc_cfg;
extern const cy_en_prot_region_t cycfg_unused_ppc_0_regions[];
extern const cy_en_prot_region_t cycfg_unused_ppc_1_regions[];
extern const size_t cycfg_unused_ppc_0_regions_count;
extern const size_t cycfg_unused_ppc_1_regions_count;

#if defined(CY_PDL_TZ_ENABLED)
extern const cy_stc_sau_config_t SAU_config[4];
#endif /* defined(CY_PDL_TZ_ENABLED) */

extern const cy_stc_mpu_config_t cycfg_mpu_cm55_ns_0_config[3];

#if defined(COMPONENT_SECURE_DEVICE) && defined(COMPONENT_MW_MTB_SRF)
extern const mtb_srf_protection_range_s_t mxrramc_0_mpc_0_srf_protection_range_s[mxrramc_0_mpc_0_REGION_COUNT];
extern const mtb_srf_protection_range_s_t mxsramc_0_mpc_0_srf_protection_range_s[mxsramc_0_mpc_0_REGION_COUNT];
extern const mtb_srf_protection_range_s_t mxsramc_1_mpc_0_srf_protection_range_s[mxsramc_1_mpc_0_REGION_COUNT];
extern const mtb_srf_protection_range_s_t smif_0_mpc_0_srf_protection_range_s[smif_0_mpc_0_REGION_COUNT];
extern const mtb_srf_protection_range_s_t socmem_0_mpc_0_srf_protection_range_s[socmem_0_mpc_0_REGION_COUNT];
#endif /* defined(COMPONENT_SECURE_DEVICE) && defined(COMPONENT_MW_MTB_SRF) */

void init_cycfg_ns_power(void);
void init_cycfg_power(void);

#if defined(CY_PDL_TZ_ENABLED)
cy_rslt_t init_cycfg_mpc(void);
#endif /* defined(CY_PDL_TZ_ENABLED) */

cy_rslt_t init_cycfg_ppc0(void);
cy_rslt_t init_cycfg_ppc1(void);
void init_cycfg_system(void);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* CYCFG_SYSTEM_H */
