/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2023 Damien P. George
 * Copyright (c) 2023 Infineon Technologies AG
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

#include "py/runtime.h"
#include "py/objstr.h"
#include "extmod/modnetwork.h"
#include "network_ifx_wcm.h"

#include "cybsp.h"
#include "cy_wcm.h"
#include "mtb_hal_sdio.h"
#include "mtb_hal_gpio.h"
#include "cy_sd_host.h"

#include "FreeRTOS.h"
#include "task.h"

#if MICROPY_PY_NETWORK_IFX_WCM

// ---------------------------------------------------------------------------
// SDIO / GPIO hardware state
// ---------------------------------------------------------------------------

#define WIFI_SDIO_INTERRUPT_PRIORITY    (7U)
#define WIFI_HOST_WAKE_INTERRUPT_PRIORITY (2U)
#define WIFI_SDIO_FREQUENCY_HZ          (25000000U)
#define WIFI_SDIO_BLOCK_SIZE            (64U)

static mtb_hal_sdio_t sdio_obj;
static cy_stc_sd_host_context_t sdhc_ctx;
static cy_wcm_config_t wcm_config;
static bool wcm_initialized = false;

static void wifi_sdio_isr(void) {
    mtb_hal_sdio_process_interrupt(&sdio_obj);
}

static void wifi_host_wake_isr(void) {
    mtb_hal_gpio_process_interrupt(&wcm_config.wifi_host_wake_pin);
}

static void wifi_sdio_init(void) {
    cy_stc_sysint_t sdio_intr = {
        .intrSrc = CYBSP_WIFI_SDIO_IRQ,
        .intrPriority = WIFI_SDIO_INTERRUPT_PRIORITY
    };
    cy_stc_sysint_t wake_intr = {
        .intrSrc = CYBSP_WIFI_HOST_WAKE_IRQ,
        .intrPriority = WIFI_HOST_WAKE_INTERRUPT_PRIORITY
    };

    Cy_SysInt_Init(&sdio_intr, wifi_sdio_isr);
    NVIC_EnableIRQ(CYBSP_WIFI_SDIO_IRQ);

    mtb_hal_sdio_setup(&sdio_obj, &CYBSP_WIFI_SDIO_sdio_hal_config, NULL, &sdhc_ctx);

    Cy_SD_Host_Enable(CYBSP_WIFI_SDIO_HW);
    Cy_SD_Host_Init(CYBSP_WIFI_SDIO_HW, CYBSP_WIFI_SDIO_sdio_hal_config.host_config, &sdhc_ctx);
    Cy_SD_Host_SetHostBusWidth(CYBSP_WIFI_SDIO_HW, CY_SD_HOST_BUS_WIDTH_4_BIT);

    mtb_hal_sdio_cfg_t sdio_cfg = {
        .frequencyhal_hz = WIFI_SDIO_FREQUENCY_HZ,
        .block_size = WIFI_SDIO_BLOCK_SIZE
    };
    mtb_hal_sdio_configure(&sdio_obj, &sdio_cfg);

    mtb_hal_gpio_setup(&wcm_config.wifi_wl_pin,
        CYBSP_WIFI_WL_REG_ON_PORT_NUM, CYBSP_WIFI_WL_REG_ON_PIN);
    mtb_hal_gpio_setup(&wcm_config.wifi_host_wake_pin,
        CYBSP_WIFI_HOST_WAKE_PORT_NUM, CYBSP_WIFI_HOST_WAKE_PIN);

    Cy_SysInt_Init(&wake_intr, wifi_host_wake_isr);
    NVIC_EnableIRQ(CYBSP_WIFI_HOST_WAKE_IRQ);
}

// ---------------------------------------------------------------------------
// network_init / network_deinit called from main.c
// ---------------------------------------------------------------------------

void network_init(void) {
    // Full WiFi init: SDIO hardware setup then cy_wcm_init() (firmware download).
    // Called once before the soft_reset loop from main.c, after a short vTaskDelay
    // so FreeRTOS is stable enough to service SDIO bulk DMA interrupts.
    wifi_sdio_init();
    wcm_config.interface = CY_WCM_INTERFACE_TYPE_STA;
    wcm_config.wifi_interface_instance = &sdio_obj;

    cy_rslt_t ret = cy_wcm_init(&wcm_config);
    if (ret != CY_RSLT_SUCCESS) {
        mp_printf(&mp_plat_print, "network_init: cy_wcm_init failed (0x%08X)\r\n", (unsigned int)ret);
        return;
    }
    wcm_initialized = true;
}

void network_deinit(void) {
    if (wcm_initialized) {
        cy_wcm_deinit();
        wcm_initialized = false;
    }
}

// Safety guard: if network_init() failed, make_new retries WCM init.
static void wcm_ensure_init(void) {
    if (wcm_initialized) {
        return;
    }
    cy_rslt_t ret = cy_wcm_init(&wcm_config);
    if (ret != CY_RSLT_SUCCESS) {
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("WiFi init failed (0x%08X)"), (unsigned int)ret);
    }
    wcm_initialized = true;
}

// ---------------------------------------------------------------------------
// Minimal MicroPython type — make_new + print so the module registers cleanly
// ---------------------------------------------------------------------------

static mp_obj_t network_ifx_wcm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    wcm_ensure_init();
    mp_raise_NotImplementedError(MP_ERROR_TEXT("WiFi WLAN object not yet implemented"));
}

static void network_ifx_wcm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_printf(print, "<IFX_WCM>");
}

static const mp_rom_map_elem_t network_ifx_wcm_locals_dict_table[] = {
};
static MP_DEFINE_CONST_DICT(network_ifx_wcm_locals_dict, network_ifx_wcm_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_network_ifx_wcm_type,
    MP_QSTR_IFX_WCM,
    MP_TYPE_FLAG_NONE,
    make_new, network_ifx_wcm_make_new,
    print, network_ifx_wcm_print,
    locals_dict, &network_ifx_wcm_locals_dict
    );

#endif // MICROPY_PY_NETWORK_IFX_WCM
