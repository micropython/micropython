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

#include <string.h>

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

#define NETWORK_WLAN_DEFAULT_SSID       "mpy-psoc-edge-wlan"
#define NETWORK_WLAN_DEFAULT_PASSWORD   "mpy_PSoC_w3lc0me!"
#define NETWORK_WLAN_DEFAULT_SECURITY   CY_WCM_SECURITY_WPA2_AES_PSK
#define NETWORK_WLAN_DEFAULT_CHANNEL    9

#define NETWORK_WLAN_AP_IP              "192.168.0.1"
#define NETWORK_WLAN_AP_GATEWAY_IP      "192.168.0.1"
#define NETWORK_WLAN_AP_NETMASK_IP      "255.255.255.0"

// ---------------------------------------------------------------------------
// SDIO / GPIO hardware state
// ---------------------------------------------------------------------------

#define WIFI_SDIO_INTERRUPT_PRIORITY    (7U)
#define WIFI_HOST_WAKE_INTERRUPT_PRIORITY (2U)
#define WIFI_SDIO_FREQUENCY_HZ          (25000000U)
#define WIFI_SDIO_BLOCK_SIZE            (64U)

typedef struct {
    cy_wcm_ap_config_t ap_config;
} network_ifx_wcm_ap_obj_t;

typedef struct {
    cy_wcm_associated_ap_info_t ap_info;
    uint8_t connect_retries;
} network_ifx_wcm_sta_obj_t;

typedef union {
    network_ifx_wcm_ap_obj_t ap_obj;
    network_ifx_wcm_sta_obj_t sta_obj;
} itf_obj_t;

typedef struct _network_ifx_wcm_obj_t {
    mp_obj_base_t base;
    cy_wcm_interface_t itf;
    itf_obj_t itf_obj;
} network_ifx_wcm_obj_t;

static network_ifx_wcm_obj_t network_ifx_wcm_wl_sta = { { &mp_network_ifx_wcm_type }, CY_WCM_INTERFACE_TYPE_STA };
static network_ifx_wcm_obj_t network_ifx_wcm_wl_ap = { { &mp_network_ifx_wcm_type }, CY_WCM_INTERFACE_TYPE_AP };

#define wcm_get_ap_conf_ptr(net_obj) & (net_obj.itf_obj.ap_obj.ap_config)
#define wcm_get_sta_conf_ptr(net_obj) & (net_obj.itf_obj.sta_obj)

extern uint8_t cy_wcm_is_ap_up(void);

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

// Network Access Point initialization with default network parameters
void network_ap_init() {
    cy_rslt_t ret = CY_RSLT_SUCCESS;
    cy_wcm_ap_config_t *ap_conf = wcm_get_ap_conf_ptr(network_ifx_wcm_wl_ap);
    cy_wcm_ip_setting_t *ap_ip_settings = &(ap_conf->ip_settings);

    ap_conf->channel = NETWORK_WLAN_DEFAULT_CHANNEL;
    memcpy(ap_conf->ap_credentials.SSID, NETWORK_WLAN_DEFAULT_SSID, strlen(NETWORK_WLAN_DEFAULT_SSID) + 1);
    memcpy(ap_conf->ap_credentials.password, NETWORK_WLAN_DEFAULT_PASSWORD, strlen(NETWORK_WLAN_DEFAULT_PASSWORD) + 1);
    ap_conf->ap_credentials.security = NETWORK_WLAN_DEFAULT_SECURITY;

    cy_wcm_set_ap_ip_setting(ap_ip_settings, NETWORK_WLAN_AP_IP, NETWORK_WLAN_AP_NETMASK_IP, NETWORK_WLAN_AP_GATEWAY_IP, CY_WCM_IP_VER_V4);
    wcm_assert_raise("network ap ip setting error (code: %d)", ret);
}

static void restart_ap(cy_wcm_ap_config_t *ap_conf) {
    if (cy_wcm_is_ap_up()) {
        uint32_t ret = cy_wcm_stop_ap();
        wcm_assert_raise("network ap deactivate error (with code: %d)", ret);
        ret = cy_wcm_start_ap(ap_conf);
        wcm_assert_raise("network ap active error (with code: %d)", ret);
    }
}

void network_sta_init() {
    network_ifx_wcm_sta_obj_t *sta_conf = wcm_get_sta_conf_ptr(network_ifx_wcm_wl_sta);
    sta_conf->connect_retries = 3; // Default connect retries
}

void network_hw_init(void) {
    wifi_sdio_init();
    #if (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP)
    Cy_SysPm_RegisterCallback(&sdhc_ds_cb);
    #endif
    wcm_config.interface = CY_WCM_INTERFACE_TYPE_AP_STA;
    wcm_config.wifi_interface_instance = &sdio_obj;
}

void network_init(void) {
    cy_rslt_t ret = cy_wcm_init(&wcm_config);
    wcm_assert_raise("network init error (code: %d)", ret);

    network_ap_init();
    network_sta_init();
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

/*******************************************************************************/
// network API

static mp_obj_t network_ifx_wcm_active(size_t n_args, const mp_obj_t *args) {
    cy_rslt_t ret = CY_RSLT_SUCCESS;
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (self->itf == CY_WCM_INTERFACE_TYPE_STA) {
        if (n_args == 1) {
            return mp_obj_new_bool(cy_wcm_is_connected_to_ap());
        }
    } else if (self->itf == CY_WCM_INTERFACE_TYPE_AP) {
        if (n_args == 1) {
            return mp_obj_new_bool(cy_wcm_is_ap_up());
        } else {
            if (mp_obj_is_true(args[1])) {
                cy_wcm_ap_config_t *ap_conf = wcm_get_ap_conf_ptr(network_ifx_wcm_wl_ap);
                ret = cy_wcm_start_ap(ap_conf);
                wcm_assert_raise("network ap active error (with code: %d)", ret);
            } else {
                ret = cy_wcm_stop_ap();
                wcm_assert_raise("network ap deactivate error (with code: %d)", ret);
            }
        }
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_ifx_wcm_active_obj, 1, 2, network_ifx_wcm_active);

static mp_obj_t network_ifx_wcm_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    if (self->itf != CY_WCM_INTERFACE_TYPE_STA) {
        mp_raise_ValueError(MP_ERROR_TEXT("network STA required"));
    }

    enum { ARG_ssid, ARG_key, ARG_bssid };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid,  MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_key,   MP_ARG_OBJ,                   {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ,  {.u_rom_obj = MP_ROM_NONE} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    cy_wcm_connect_params_t connect_param;
    cy_wcm_ip_address_t ip_address;
    memset(&connect_param, 0, sizeof(cy_wcm_connect_params_t));

    // SSID
    mp_buffer_info_t ssid;
    mp_get_buffer_raise(args[ARG_ssid].u_obj, &ssid, MP_BUFFER_READ);
    if (ssid.len > CY_WCM_MAX_SSID_LEN) {
        mp_raise_TypeError(MP_ERROR_TEXT("network connect SSID too long"));
    }
    memcpy(connect_param.ap_credentials.SSID, ssid.buf, ssid.len);

    // Key (optional)
    if (args[ARG_key].u_obj != mp_const_none) {
        mp_buffer_info_t key;
        mp_get_buffer_raise(args[ARG_key].u_obj, &key, MP_BUFFER_READ);
        if (key.len > CY_WCM_MAX_PASSPHRASE_LEN) {
            mp_raise_TypeError(MP_ERROR_TEXT("network connect key too long"));
        }
        memcpy(connect_param.ap_credentials.password, key.buf, key.len);
    }

    // BSSID (optional)
    if (args[ARG_bssid].u_obj != mp_const_none) {
        mp_buffer_info_t bssid;
        mp_get_buffer_raise(args[ARG_bssid].u_obj, &bssid, MP_BUFFER_READ);
        if (bssid.len != CY_WCM_MAC_ADDR_LEN) {
            mp_raise_ValueError(MP_ERROR_TEXT("bssid address invalid length"));
        }
        memcpy(connect_param.BSSID, bssid.buf, bssid.len);
    }

    // Let WCM discover security type
    connect_param.ap_credentials.security = CY_WCM_SECURITY_UNKNOWN;

    network_ifx_wcm_sta_obj_t *sta_conf = wcm_get_sta_conf_ptr(network_ifx_wcm_wl_sta);
    uint8_t retries = sta_conf->connect_retries;
    cy_rslt_t ret;
    do {
        ret = cy_wcm_connect_ap(&connect_param, &ip_address);
    } while (ret != CY_RSLT_SUCCESS && --retries > 0);
    wcm_assert_raise("network sta connect error (with code: %d)", ret);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_ifx_wcm_connect_obj, 1, network_ifx_wcm_connect);

static mp_obj_t network_ifx_wcm_disconnect(mp_obj_t self_in) {
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->itf != CY_WCM_INTERFACE_TYPE_STA) {
        mp_raise_ValueError(MP_ERROR_TEXT("network STA required"));
    }
    cy_rslt_t ret = cy_wcm_disconnect_ap();
    wcm_assert_raise("network sta disconnect error (with code: %d)", ret);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_ifx_wcm_disconnect_obj, network_ifx_wcm_disconnect);

static mp_obj_t network_ifx_wcm_isconnected(mp_obj_t self_in) {
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->itf == CY_WCM_INTERFACE_TYPE_STA) {
        return mp_obj_new_bool(cy_wcm_is_connected_to_ap());
    } else if (self->itf == CY_WCM_INTERFACE_TYPE_AP) {
        // Return False immediately if AP is not running
        if (!cy_wcm_is_ap_up()) {
            return mp_obj_new_bool(false);
        }
        // True if at least one client is associated
        bool is_a_sta_connected = false;
        cy_wcm_mac_t sta[1] = {0};
        cy_wcm_mac_t not_conn_sta = {0, 0, 0, 0, 0, 0};
        cy_rslt_t ret = cy_wcm_get_associated_client_list(sta, 1);
        wcm_assert_raise("network ap isconnected error (with code: %d)", ret);
        if (memcmp(&sta[0], &not_conn_sta, CY_WCM_MAC_ADDR_LEN) != 0) {
            is_a_sta_connected = true;
        }
        return mp_obj_new_bool(is_a_sta_connected);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_ifx_wcm_isconnected_obj, network_ifx_wcm_isconnected);

static const mp_rom_map_elem_t network_ifx_wcm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active),      MP_ROM_PTR(&network_ifx_wcm_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect),     MP_ROM_PTR(&network_ifx_wcm_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect),  MP_ROM_PTR(&network_ifx_wcm_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&network_ifx_wcm_isconnected_obj) },
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
