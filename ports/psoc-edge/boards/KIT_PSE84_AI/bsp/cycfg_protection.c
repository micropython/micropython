/*******************************************************************************
 * File Name: cycfg_protection.c
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

#include "cycfg_protection.h"
#include "cycfg_system.h"

void init_cycfg_protection(void)
{
#if (CY_SYSTEM_CPU_M33) && defined(COMPONENT_SECURE_DEVICE) && !defined(CYBSP_SKIP_SAU_INIT)
    Cy_SAU_Init(SAU, SAU_config, sizeof(SAU_config) / sizeof(SAU_config[0]));
#endif /* (CY_SYSTEM_CPU_M33) && defined(COMPONENT_SECURE_DEVICE) && !defined(CYBSP_SKIP_SAU_INIT) */
#if  (CY_SYSTEM_CPU_M33) && defined(COMPONENT_SECURE_DEVICE) && defined(CY_PDL_TZ_ENABLED) && !defined(CYBSP_SKIP_MPC_INIT)
    init_cycfg_mpc();
#endif /*  (CY_SYSTEM_CPU_M33) && defined(COMPONENT_SECURE_DEVICE) && defined(CY_PDL_TZ_ENABLED) && !defined(CYBSP_SKIP_MPC_INIT) */
#if  (CY_SYSTEM_CPU_M33) && defined(COMPONENT_SECURE_DEVICE) && defined(CY_PDL_TZ_ENABLED) && !defined(CYBSP_SKIP_PPC_INIT)
    init_cycfg_ppc0();
    init_cycfg_ppc1();
#endif /*  (CY_SYSTEM_CPU_M33) && defined(COMPONENT_SECURE_DEVICE) && defined(CY_PDL_TZ_ENABLED) && !defined(CYBSP_SKIP_PPC_INIT) */
#if (CY_SYSTEM_CPU_M55) && !defined(CYBSP_SKIP_MPU_INIT)
    Cy_MPU_Init(cycfg_mpu_cm55_ns_0_config, sizeof(cycfg_mpu_cm55_ns_0_config) / sizeof(cycfg_mpu_cm55_ns_0_config[0]));
#endif /* (CY_SYSTEM_CPU_M55) && !defined(CYBSP_SKIP_MPU_INIT) */
}
