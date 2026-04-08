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
#include "cy_syspm.h"
#include "cycfg_system.h"

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

typedef struct _network_ifx_wcm_obj_t {
    mp_obj_base_t base;
    cy_wcm_interface_t itf;
} network_ifx_wcm_obj_t;

static network_ifx_wcm_obj_t network_ifx_wcm_wl_sta = { { &mp_network_ifx_wcm_type }, CY_WCM_INTERFACE_TYPE_STA };
static network_ifx_wcm_obj_t network_ifx_wcm_wl_ap = { { &mp_network_ifx_wcm_type }, CY_WCM_INTERFACE_TYPE_AP };

static mtb_hal_sdio_t sdio_obj;
static cy_stc_sd_host_context_t sdhc_ctx;
static cy_wcm_config_t wcm_config;

#if (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP)
static cy_stc_syspm_callback_params_t sdhc_ds_params = {
    .context = &sdhc_ctx,
    .base = CYBSP_WIFI_SDIO_HW
};
static cy_stc_syspm_callback_t sdhc_ds_cb = {
    .callback = Cy_SD_Host_DeepSleepCallback,
    .skipMode = 0U,
    .type = CY_SYSPM_DEEPSLEEP,
    .callbackParams = &sdhc_ds_params,
    .prevItm = NULL,
    .nextItm = NULL,
    .order = 1U
};
#endif

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
// network_hw_init  — one-time HW bring-up, called before the soft_reset loop
// network_init / network_deinit — called every soft reset from main.c
// ---------------------------------------------------------------------------
#define wcm_assert_raise(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

void network_hw_init(void) {
    wifi_sdio_init();
    #if (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP)
    Cy_SysPm_RegisterCallback(&sdhc_ds_cb);
    #endif
    wcm_config.interface = CY_WCM_INTERFACE_TYPE_STA;
    wcm_config.wifi_interface_instance = &sdio_obj;
}

void network_init(void) {
    cy_rslt_t ret = cy_wcm_init(&wcm_config);
    wcm_assert_raise("network init error (code: %d)", ret);
}

void network_deinit(void) {
    cy_rslt_t ret = cy_wcm_deinit();
    wcm_assert_raise("network deinit error (code: %d)", ret);
}

// ---------------------------------------------------------------------------
// Minimal MicroPython type — make_new + print so the module registers cleanly
// ---------------------------------------------------------------------------

static mp_obj_t network_ifx_wcm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    if (n_args == 0 || mp_obj_get_int(args[0]) == MOD_NETWORK_STA_IF) {
        return MP_OBJ_FROM_PTR(&network_ifx_wcm_wl_sta);
    } else {
        return MP_OBJ_FROM_PTR(&network_ifx_wcm_wl_ap);
    }
}

static void network_ifx_wcm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(self_in);

    const char *status_str;
    if (self->itf == CY_WCM_INTERFACE_TYPE_STA) {
        if (cy_wcm_is_connected_to_ap()) {
            status_str = "joined";
        } else {
            status_str = "down";
        }
    } else {
        if (cy_wcm_is_ap_up()) {
            status_str = "up";
        } else {
            status_str = "down";
        }
    }

    cy_wcm_ip_address_t ip_address;
    cy_rslt_t ret = cy_wcm_get_ip_addr(self->itf, &ip_address);
    if (ret != CY_RSLT_SUCCESS) {
        ip_address.ip.v4 = 0;
    }

    mp_printf(print, "<IFX WCM %s %s %u.%u.%u.%u>",
        self->itf == CY_WCM_INTERFACE_TYPE_STA ? "STA" : "AP",
        status_str,
        ip_address.ip.v4 & 0xff,
        ip_address.ip.v4 >> 8 & 0xff,
        ip_address.ip.v4 >> 16 & 0xff,
        ip_address.ip.v4 >> 24
        );
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
