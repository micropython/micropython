/***************************************************************************//**
* \file cybsp.c
*
* Description:
* Provides initialization code for starting up the hardware contained on the
* Infineon board.
*
********************************************************************************
* \copyright
* Copyright 2018-2022 Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation
*
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
*******************************************************************************/

#include <stdlib.h>
#include "cy_syspm.h"
#include "cy_sysclk.h"
#include "cybsp.h"
#if defined(CY_USING_HAL)
#include "cyhal_hwmgr.h"
#include "cyhal_syspm.h"

#if defined(CYBSP_WIFI_CAPABLE) && defined(CYHAL_UDB_SIO)
#include "SDIO_HOST.h"
#endif
#endif // defined(CY_USING_HAL)

#if defined(COMPONENT_MW_CAT1CM0P)
    #include "mtb_cat1cm0p.h"
#endif

#if defined(__cplusplus)
extern "C" {
#endif

// The sysclk deep sleep callback is recommended to be the last callback that is executed before
// entry into deep sleep mode and the first one upon exit the deep sleep mode.
// Doing so minimizes the time spent on low power mode entry and exit.
#ifndef CYBSP_SYSCLK_PM_CALLBACK_ORDER
    #define CYBSP_SYSCLK_PM_CALLBACK_ORDER  (255u)
#endif

#if !defined(CYBSP_CUSTOM_SYSCLK_PM_CALLBACK)
// --------------------------------------------------------------------------------------------------
// cybsp_register_sysclk_pm_callback
//
// Registers a power management callback that prepares the clock system for entering deep sleep mode
// and restore the clocks upon wakeup from deep sleep.
// NOTE: This is called automatically as part of \ref cybsp_init
// --------------------------------------------------------------------------------------------------
static cy_rslt_t cybsp_register_sysclk_pm_callback(void) {
    cy_rslt_t result = CY_RSLT_SUCCESS;
    static cy_stc_syspm_callback_params_t cybsp_sysclk_pm_callback_param = { NULL, NULL };
    static cy_stc_syspm_callback_t cybsp_sysclk_pm_callback =
    {
        .callback = &Cy_SysClk_DeepSleepCallback,
        .type = CY_SYSPM_DEEPSLEEP,
        .callbackParams = &cybsp_sysclk_pm_callback_param,
        .order = CYBSP_SYSCLK_PM_CALLBACK_ORDER
    };

    if (!Cy_SysPm_RegisterCallback(&cybsp_sysclk_pm_callback)) {
        result = CYBSP_RSLT_ERR_SYSCLK_PM_CALLBACK;
    }
    return result;
}


#endif // if !defined(CYBSP_CUSTOM_SYSCLK_PM_CALLBACK)


// --------------------------------------------------------------------------------------------------
// cybsp_init
// --------------------------------------------------------------------------------------------------
cy_rslt_t cybsp_init(void) {
    // Setup hardware manager to track resource usage then initialize all system (clock/power) board
    // configuration
    #if defined(CY_USING_HAL)
    cy_rslt_t result = cyhal_hwmgr_init();

    if (CY_RSLT_SUCCESS == result) {
        result = cyhal_syspm_init();
    }

    #ifdef CY_CFG_PWR_VDDA_MV
    if (CY_RSLT_SUCCESS == result) {
        cyhal_syspm_set_supply_voltage(CYHAL_VOLTAGE_SUPPLY_VDDA, CY_CFG_PWR_VDDA_MV);
    }
    #endif

    #else // if defined(CY_USING_HAL)
    cy_rslt_t result = CY_RSLT_SUCCESS;
    #endif // if defined(CY_USING_HAL)

    // By default, the peripheral configuration will be done on the first core running user code.
    // This is the CM0+ if it is available and not running a pre-built image, and the CM4 otherwise.
    // This is done to ensure configuration is available for all cores that might need to use it.
    // In the case of a dual core project, this can be changed below to perform initialization on
    // the CM4 if necessary.
    #if defined(CORE_NAME_CM0_0) || !(__CM0P_PRESENT) || (defined(CORE_NAME_CM4_0) && \
    defined(CY_USING_PREBUILT_CM0P_IMAGE))
    cycfg_config_init();
    #endif

    // Do any additional configuration reservations that are needed on all cores.
    cycfg_config_reservations();

    if (CY_RSLT_SUCCESS == result) {
        #if defined(CYBSP_CUSTOM_SYSCLK_PM_CALLBACK)
        result = cybsp_register_custom_sysclk_pm_callback();
        #else
        result = cybsp_register_sysclk_pm_callback();
        #endif
    }

    #if defined(CYBSP_WIFI_CAPABLE) && defined(CYHAL_UDB_SIO)

    // Reserve resources for the UDB SDIO interface that might want to be used by others. This
    // includes specific clock and DMA instances. This must be done before other HAL API calls as
    // specific peripheral instances are needed
    // NOTE: The full SDIO/WiFi interface still needs to be initialized via
    // cybsp_wifi_init_primary(). This is typically done when starting up WiFi.
    if (CY_RSLT_SUCCESS == result) {
        result = SDIO_ReserveResources();
    }
    #endif // defined(CYBSP_WIFI_CAPABLE) && defined(CYHAL_UDB_SIO)

    // CYHAL_HWMGR_RSLT_ERR_INUSE error code could be returned if any needed for BSP resource was
    // reserved by user previously. Please review the Device Configurator (design.modus) and the BSP
    // reservation list (cyreservedresources.list) to make sure no resources are reserved by both.
    return result;
}


#if defined(__cplusplus)
}
#endif
