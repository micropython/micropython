/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MASSDRIVER EI (massdriver.space)
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

#include "py/mpconfig.h"

#if MICROPY_PY_ZEPHYR_NETWORK_WLAN

BUILD_ASSERT(IS_ENABLED(CONFIG_NETWORKING) && IS_ENABLED(CONFIG_WIFI),
    "CONFIG_NETWORKING and CONFIG_WIFI must be enabled to use MICROPY_PY_ZEPHYR_NETWORK_WLAN");

BUILD_ASSERT(IS_ENABLED(CONFIG_WIFI_NM) && IS_ENABLED(CONFIG_NET_L2_WIFI_MGMT),
    "CONFIG_WIFI_NM and CONFIG_NET_L2_WIFI_MGMT must be enabled to use MICROPY_PY_ZEPHYR_NETWORK_WLAN");

#include <zephyr/drivers/hwinfo.h>
#include <zephyr/net/wifi.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/wifi_utils.h>
#include <zephyr/sys/atomic.h>

#include "py/runtime.h"
#include "py/objlist.h"
#include "py/objstr.h"

#include "modnetwork.h"

#define ZEPHYR_WLAN_STA_CONNECT_TIMEOUT_S   30
#define ZEPHYR_WLAN_STA_SCAN_TIMEOUT_MS     10000

#define ZEPHYR_WLAN_EVENT_MASK ( \
    NET_EVENT_WIFI_CONNECT_RESULT | \
    NET_EVENT_WIFI_DISCONNECT_RESULT | \
    NET_EVENT_WIFI_AP_ENABLE_RESULT | \
    NET_EVENT_WIFI_AP_DISABLE_RESULT | \
    NET_EVENT_WIFI_AP_STA_CONNECTED | \
    NET_EVENT_WIFI_AP_STA_DISCONNECTED | \
    NET_EVENT_WIFI_P2P_DEVICE_FOUND | \
    NET_EVENT_WIFI_SCAN_RESULT | \
    NET_EVENT_WIFI_RAW_SCAN_RESULT)

/* Atomic bits identifiers */
#define ZEPHYR_WLAN_STATE_AP            0 /* Unset = STA mode */
#define ZEPHYR_WLAN_STATE_ACTIVE        1
#define ZEPHYR_WLAN_STATE_STA_CONNECTED 2
#define ZEPHYR_WLAN_STATE_STA_FAIL      3
#define ZEPHYR_WLAN_STATE_STA_BADPW     4
#define ZEPHYR_WLAN_STATE_STA_BADAP     5
#define ZEPHYR_WLAN_STATE_AP_UP         6
#define ZEPHYR_WLAN_STATE_DELETED       7

#define WIFI_FREQ_BAND_ALL \
    (BIT(WIFI_FREQ_BAND_2_4_GHZ) | BIT(WIFI_FREQ_BAND_5_GHZ) | BIT(WIFI_FREQ_BAND_6_GHZ))

typedef struct _zephyr_wlan_nw_t {
    mp_obj_t ssid;
    mp_obj_t password;
    bool password_is_pbkdf2;
    bool hidden;
    enum wifi_frequency_bands band;
    uint8_t channel;
    uint8_t mac[WIFI_MAC_ADDR_LEN];
    enum wifi_security_type security;
    enum wifi_frequency_bandwidths bandwidth;
} zephyr_wlan_nw_t;

typedef struct _zephyr_wlan_obj_t {
    NETWORK_ZEPHYR_BASE_FRONT;
    struct net_mgmt_event_callback cb;
    atomic_t state;
    mp_obj_t remote_sta_list;
    mp_obj_t scan_list;
    zephyr_wlan_nw_t network;
    struct k_fifo event_fifo;
    mp_sched_node_t event_node;
} zephyr_wlan_obj_t;

NETWORK_ZEPHYR_BASE_FRONT_CHECK(zephyr_wlan_obj_t, cb);

typedef struct _zephyr_wlan_event_t {
    void *fifo_reserved;
    uint64_t event;
    void *info;
    size_t info_len;
} zephyr_wlan_event_t;

/* Helper Functions ----------------------------------------------------------------------------- */

static int zephyr_wlan_sec_to_mpy(enum wifi_security_type security) {
    switch (security) {
        case WIFI_SECURITY_TYPE_NONE:
            return 0; /* Open */
        case WIFI_SECURITY_TYPE_WEP:
            return 1; /* WEP */
        case WIFI_SECURITY_TYPE_WPA_PSK:
            return 2; /* WPA-PSK */
        case WIFI_SECURITY_TYPE_PSK:
            return 3; /* WPA2-PSK */
        case WIFI_SECURITY_TYPE_WPA_AUTO_PERSONAL:
            return 4; /* WPA123-PSK */
        default:
            return -1; /* Not covered in specs */
    }
}

static enum wifi_security_type zephyr_wlan_security(int in) {
    if (in < 0 || in >= WIFI_SECURITY_TYPE_MAX) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid security type"));
    } else {
        return in;
    }
}

static enum wifi_frequency_bands zephyr_wlan_band(int in) {
    if ((in < 0 || in >= WIFI_FREQ_BAND_MAX) && in != WIFI_FREQ_BAND_UNKNOWN) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid band"));
    } else {
        return in;
    }
}

static uint8_t zephyr_wlan_channel(int in) {
    if ((in < WIFI_CHANNEL_MIN || in > WIFI_CHANNEL_MAX) && in != WIFI_CHANNEL_ANY) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid channel"));
    } else {
        return in;
    }
}

static enum wifi_frequency_bandwidths zephyr_wlan_bandwidth(int in) {
    if ((in < WIFI_FREQ_BANDWIDTH_20MHZ || in >= WIFI_FREQ_BANDWIDTH_MAX) && in != WIFI_FREQ_BANDWIDTH_UNKNOWN) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid bandwidth"));
    } else {
        return in;
    }
}

static void zephyr_wlan_updown(zephyr_wlan_obj_t *self, bool up) {
    if (up) {
        if (!net_if_is_admin_up(self->nic.net_if)) {
            int ret = net_if_up(self->nic.net_if);
            if (ret != 0) {
                mp_raise_msg_varg(&mp_type_RuntimeError,
                    MP_ERROR_TEXT("failed to bring interface up: %d"), ret);
            }
        }
        struct wifi_ps_params ps = {0};
        ps.enabled = WIFI_PS_DISABLED;
        ps.type = WIFI_PS_PARAM_STATE;

        int ret = net_mgmt(NET_REQUEST_WIFI_PS, self->nic.net_if, &ps, sizeof(ps));
        if (ret != 0 && ret != -ENOTSUP) {
            mp_raise_msg_varg(&mp_type_RuntimeError,
                MP_ERROR_TEXT("failed to disable power save: %d"), ret);
        }
        atomic_set_bit(&self->state, ZEPHYR_WLAN_STATE_ACTIVE);
    } else {
        struct wifi_ps_params ps = {0};
        ps.enabled = WIFI_PS_ENABLED;
        ps.type = WIFI_PS_PARAM_STATE;

        int ret = net_mgmt(NET_REQUEST_WIFI_PS, self->nic.net_if, &ps, sizeof(ps));
        if (ret != 0 && ret != -ENOTSUP) {
            mp_raise_msg_varg(&mp_type_RuntimeError,
                MP_ERROR_TEXT("failed to enable power save: %d"), ret);
        }
        if (net_if_is_admin_up(self->nic.net_if)) {
            int ret = net_if_down(self->nic.net_if);
            if (ret != 0) {
                mp_raise_msg_varg(&mp_type_RuntimeError,
                    MP_ERROR_TEXT("failed to bring interface down: %d"), ret);
            }
        }
        atomic_clear_bit(&self->state, ZEPHYR_WLAN_STATE_ACTIVE);
    }
}

static inline void zephyr_wlan_ready(zephyr_wlan_obj_t *self) {
    if (!atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_ACTIVE)) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("interface is not active"));
    }
}

#ifdef CONFIG_WIFI_MGMT_RAW_SCAN_RESULTS
/* From WiFi shell sample */
static int wifi_freq_to_channel(int frequency) {
    int channel;

    if (frequency == 2484) { /* channel 14 */
        channel = 14;
    } else if ((frequency <= 2472) && (frequency >= 2412)) {
        channel = ((frequency - 2412) / 5) + 1;
    } else if ((frequency <= 5320) && (frequency >= 5180)) {
        channel = ((frequency - 5180) / 5) + 36;
    } else if ((frequency <= 5720) && (frequency >= 5500)) {
        channel = ((frequency - 5500) / 5) + 100;
    } else if ((frequency <= 5895) && (frequency >= 5745)) {
        channel = ((frequency - 5745) / 5) + 149;
    } else {
        channel = frequency;
    }

    return channel;
}
/* That too */
static enum wifi_frequency_bands wifi_freq_to_band(int frequency) {
    enum wifi_frequency_bands band = WIFI_FREQ_BAND_2_4_GHZ;

    if ((frequency >= 2401) && (frequency <= 2495)) {
        band = WIFI_FREQ_BAND_2_4_GHZ;
    } else if ((frequency >= 5170) && (frequency <= 5895)) {
        band = WIFI_FREQ_BAND_5_GHZ;
    } else {
        band = WIFI_FREQ_BAND_6_GHZ;
    }

    return band;
}
#endif

#ifdef CONFIG_HWINFO
static uint8_t *zephyr_wlan_mac(void) {
    static uint8_t buf[WIFI_MAC_ADDR_LEN];

    if (hwinfo_get_device_id(buf, WIFI_MAC_ADDR_LEN) != WIFI_MAC_ADDR_LEN) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to get default wifi MAC address"));
    }

    return buf;
}
#endif

/* Event Functions ------------------------------------------------------------------------------ */

/* Handle offloaded foreign thread events in mpy context */
static void zephyr_wlan_event_handler_offload_handler(mp_sched_node_t *node) {
    zephyr_wlan_obj_t *self = CONTAINER_OF(node, zephyr_wlan_obj_t, event_node);
    zephyr_wlan_event_t *ev;

    while (true) {
        ev = k_fifo_get(&self->event_fifo, K_NO_WAIT);
        if (ev == NULL) {
            return;
        }
        switch (ev->event) {
            case NET_EVENT_WIFI_AP_STA_CONNECTED: {
                struct wifi_ap_sta_info *sta_info = (struct wifi_ap_sta_info *)ev->info;
                mp_obj_list_append(self->remote_sta_list, mp_obj_new_bytes(sta_info->mac, WIFI_MAC_ADDR_LEN));
                break;
            }
            case NET_EVENT_WIFI_AP_STA_DISCONNECTED: {
                struct wifi_ap_sta_info *sta_info = (struct wifi_ap_sta_info *)ev->info;
                mp_obj_list_remove(self->remote_sta_list, mp_obj_new_bytes(sta_info->mac, WIFI_MAC_ADDR_LEN));
                break;
            }
            #ifdef CONFIG_WIFI_NM_WPA_SUPPLICANT_P2P
            case NET_EVENT_WIFI_P2P_DEVICE_FOUND: {
                const struct wifi_p2p_device_info *peer_info = (const struct wifi_p2p_device_info *)ev->info;
                if (peer_info == NULL) {
                    break;
                }
                mp_obj_t peer_data;
                if (peer_info->device_name[0] == '\0') {
                    peer_data = mp_obj_new_bytes(peer_info->mac, WIFI_MAC_ADDR_LEN);
                } else {
                    mp_obj_t tobj[2] = {
                        mp_obj_new_bytes(peer_info->mac, WIFI_MAC_ADDR_LEN),
                        mp_obj_new_str_from_cstr(peer_info->device_name)
                    };
                    peer_data = mp_obj_new_tuple(2, tobj);
                }
                mp_obj_list_append(self->remote_sta_list, peer_data);
                break;
            }
            #endif
            case NET_EVENT_WIFI_SCAN_RESULT: {
                const struct wifi_scan_result *entry = (const struct wifi_scan_result *)ev->info;
                mp_obj_t tobj[] = {
                    mp_obj_new_str_from_cstr(entry->ssid),
                    mp_obj_new_bytes(entry->mac, WIFI_MAC_ADDR_LEN),
                    mp_obj_new_int(entry->channel),
                    mp_obj_new_int(entry->rssi),
                    mp_obj_new_int(zephyr_wlan_sec_to_mpy(entry->security)),
                    (strlen(entry->ssid) > 0 ? mp_const_false : mp_const_true),
                    /* Add band and Zephyr security type because the specs are too old */
                    mp_obj_new_int(entry->security),
                    mp_obj_new_int(entry->band),
                    mp_const_false, /* Indicate scan result is not raw */
                };
                mp_obj_list_append(self->scan_list, mp_obj_new_tuple(ARRAY_SIZE(tobj), tobj));
                break;
            }
            #ifdef CONFIG_WIFI_MGMT_RAW_SCAN_RESULTS
            case NET_EVENT_WIFI_RAW_SCAN_RESULT: {
                const struct wifi_raw_scan_result *entry = (const struct wifi_raw_scan_result *)ev->info;
                mp_obj_t tobj[] = {
                    mp_const_empty_bytes, /* No SSID when raw */
                    (entry->frame_length > 17 ?
                        mp_obj_new_bytes(&entry->data[10], WIFI_MAC_ADDR_LEN) : mp_obj_new_bytes(NULL, 0)), /* From wifi shell sample */
                    mp_obj_new_int(wifi_freq_to_channel(entry->frequency)),
                    mp_obj_new_int(entry->rssi),
                    mp_obj_new_int(-1),  /* We don't get that */
                    mp_const_true, /* I guess? */
                    mp_obj_new_int(-1), /* We don't get that */
                    mp_obj_new_int(wifi_freq_to_band(entry->frequency)),
                    mp_const_true, /* Indicate scan result is raw */
                    mp_obj_new_int(entry->frequency), /* Probably useful */
                    mp_obj_new_bytes(entry->data, entry->frame_length), /* Probably useful */
                };
                mp_obj_list_append(self->scan_list, mp_obj_new_tuple(ARRAY_SIZE(tobj), tobj));
                break;
            }
            #endif
        }
        if (ev->info_len > 0) {
            k_free(ev->info);
        }
        k_free(ev);
    }
}

void zephyr_wlan_event_handler_offload(zephyr_wlan_obj_t *self, uint64_t event, const void *info, size_t info_len) {
    zephyr_wlan_event_t *ev = k_malloc(sizeof(zephyr_wlan_event_t));
    if (ev == NULL) {
        return;
    }
    ev->info_len = info_len;
    ev->event = event;
    if (info_len > 0) {
        ev->info = k_malloc(info_len);
        if (ev->info == NULL) {
            k_free(ev);
            return;
        }
        memcpy(ev->info, info, info_len);
    } else {
        ev->info = NULL;
    }
    k_fifo_put(&self->event_fifo, ev);
    mp_sched_schedule_node(&self->event_node, zephyr_wlan_event_handler_offload_handler);
}

/* This function will be called for all interfaces events from a foreign thread */
static void zephyr_wlan_event_handler(struct net_mgmt_event_callback *cb, uint64_t mgmt_event,
    struct net_if *iface) {
    zephyr_wlan_obj_t *self = CONTAINER_OF(cb, zephyr_wlan_obj_t, cb);

    if (iface->if_dev != self->nic.net_if->if_dev) {
        /* Not our netif */
        return;
    }

    switch (mgmt_event) {
        case NET_EVENT_WIFI_AP_DISABLE_RESULT:
            atomic_clear_bit(&self->state, ZEPHYR_WLAN_STATE_AP_UP);
            break;
        case NET_EVENT_WIFI_AP_ENABLE_RESULT:
            atomic_set_bit(&self->state, ZEPHYR_WLAN_STATE_AP_UP);
            break;
        case NET_EVENT_WIFI_AP_STA_CONNECTED: {
            zephyr_wlan_event_handler_offload(self, mgmt_event, cb->info, sizeof(struct wifi_ap_sta_info));
            break;
        }
        case NET_EVENT_WIFI_AP_STA_DISCONNECTED: {
            zephyr_wlan_event_handler_offload(self, mgmt_event, cb->info, sizeof(struct wifi_ap_sta_info));
            break;
        }
        case NET_EVENT_WIFI_CONNECT_RESULT:
            const struct wifi_status *status = (const struct wifi_status *)cb->info;
            if (status->status != 0) {
                if (status->status == WIFI_STATUS_CONN_WRONG_PASSWORD) {
                    atomic_set_bit(&self->state, ZEPHYR_WLAN_STATE_STA_BADPW);
                } else {
                    atomic_set_bit(&self->state, ZEPHYR_WLAN_STATE_STA_FAIL);
                }
            } else {
                atomic_set_bit(&self->state, ZEPHYR_WLAN_STATE_STA_CONNECTED);
            }
            break;
        case NET_EVENT_WIFI_DISCONNECT_RESULT:
            atomic_clear_bit(&self->state, ZEPHYR_WLAN_STATE_STA_CONNECTED);
            break;
        #ifdef CONFIG_WIFI_NM_WPA_SUPPLICANT_P2P
        case NET_EVENT_WIFI_P2P_DEVICE_FOUND: {
            zephyr_wlan_event_handler_offload(self, mgmt_event, cb->info, sizeof(struct wifi_p2p_device_info));
            break;
        }
        #endif
        case NET_EVENT_WIFI_SCAN_RESULT: {
            zephyr_wlan_event_handler_offload(self, mgmt_event, cb->info, sizeof(struct wifi_scan_result));
            break;
        }
        #ifdef CONFIG_WIFI_MGMT_RAW_SCAN_RESULTS
        case NET_EVENT_WIFI_RAW_SCAN_RESULT: {
            zephyr_wlan_event_handler_offload(self, mgmt_event, cb->info, sizeof(struct wifi_raw_scan_result));
            break;
        }
        #endif
        default:
            mp_raise_NotImplementedError(MP_ERROR_TEXT("Unhandled WiFi Event type"));
    }
}

/* Methods -------------------------------------------------------------------------------------- */

static mp_obj_t network_zephyr_wlan_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw,
    const mp_obj_t *all_args) {

    enum { ARG_type, ARG_id };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_type, MP_ARG_INT, {.u_int = MOD_NETWORK_STA_IF} },
        { MP_QSTR_id, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_type].u_int != MOD_NETWORK_AP_IF && args[ARG_type].u_int != MOD_NETWORK_STA_IF) {
        mp_raise_ValueError(MP_ERROR_TEXT("Not Station or Access Point type"));
    }

    /* Validate chosen net_if */
    struct net_if *net_if = net_if_get_wifi_sta();
    if (net_if == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("No WiFi device available"));
    }
    if (args[ARG_type].u_int == MOD_NETWORK_AP_IF) {
        net_if = net_if_get_wifi_sap();
        if (net_if == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("No WiFi AP device available"));
        }
    }

    if (mp_obj_is_str(args[ARG_id].u_obj)) {
        int id = net_if_get_by_name(mp_obj_str_get_str(args[ARG_id].u_obj));
        if (id < 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("No such network interface"));
        }
        net_if = net_if_get_by_index(id);
        if (!net_if_is_wifi(net_if)) {
            mp_raise_TypeError(MP_ERROR_TEXT("Network interface is not WiFi"));
        }
    }

    zephyr_wlan_obj_t *self = mp_obj_malloc_with_finaliser(zephyr_wlan_obj_t, type);
    mp_obj_t obj = MP_OBJ_FROM_PTR(self);

    self->nic.net_if = net_if;
    network_zephyr_make_new(obj);
    atomic_clear(&self->state);
    k_fifo_init(&self->event_fifo);
    self->remote_sta_list = mp_obj_new_list(0, NULL);
    self->scan_list = mp_obj_new_list(0, NULL);

    self->network.ssid = mp_obj_new_str_from_cstr(CONFIG_NET_HOSTNAME);
    self->network.password = mp_const_empty_bytes;
    self->network.band = WIFI_FREQ_BAND_2_4_GHZ;
    self->network.channel = WIFI_CHANNEL_ANY;
    self->network.security = WIFI_SECURITY_TYPE_NONE;
    self->network.bandwidth = WIFI_FREQ_BANDWIDTH_20MHZ;
    self->network.password_is_pbkdf2 = false;
    self->network.hidden = false;
    #ifdef CONFIG_HWINFO
    memcpy(self->network.mac, zephyr_wlan_mac(), WIFI_MAC_ADDR_LEN);
    #else
    self->network.mac[0] = 0x00;
    self->network.mac[1] = 0x11;
    self->network.mac[2] = 0x22;
    self->network.mac[3] = 0x33;
    self->network.mac[4] = 0x44;
    self->network.mac[5] = 0x55;
    #endif

    net_mgmt_init_event_callback(&self->cb, zephyr_wlan_event_handler, ZEPHYR_WLAN_EVENT_MASK);
    net_mgmt_add_event_callback(&self->cb);

    if (n_args == 0 || args[ARG_type].u_int == MOD_NETWORK_STA_IF) {
        atomic_clear_bit(&self->state, ZEPHYR_WLAN_STATE_AP);
        /* Ready immediately */
        zephyr_wlan_updown(self, true);
    } else {
        atomic_set_bit(&self->state, ZEPHYR_WLAN_STATE_AP);
    }

    return obj;
}

/* Destructor to remove callback */
static mp_obj_t network_zephyr_wlan_deinit(mp_obj_t self_in) {
    zephyr_wlan_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (!atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_DELETED)) {
        atomic_set_bit(&self->state, ZEPHYR_WLAN_STATE_DELETED);

        net_mgmt_del_event_callback(&self->cb);

        zephyr_wlan_updown(self, false);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_zephyr_wlan_deinit_obj, network_zephyr_wlan_deinit);

static int network_zephyr_wlan_configure(zephyr_wlan_obj_t *self) {
    int ret;
    struct wifi_connect_req_params req = {
        .ssid = mp_obj_str_get_str(self->network.ssid),
        .psk = mp_obj_str_get_str(self->network.password),
        .sae_password = mp_obj_str_get_str(self->network.password),
        .band = self->network.band,
        .channel = self->network.channel,
        .security = self->network.security,
        .mfp = WIFI_MFP_OPTIONAL,
        .bandwidth = self->network.bandwidth,
        .verify_peer_cert = false,
        .eap_ver = 1,
        .ignore_broadcast_ssid = self->network.hidden ? 1 : 0,
        .timeout = ZEPHYR_WLAN_STA_CONNECT_TIMEOUT_S,
        .key_passwd = mp_obj_str_get_str(self->network.password),
        .psk_is_pbkdf2 = self->network.password_is_pbkdf2,
    };

    GET_STR_LEN(self->network.ssid, ssid_len);
    GET_STR_LEN(self->network.password, password_len);

    req.ssid_length = ssid_len;
    req.psk_length = password_len;
    req.sae_password_length = password_len;
    req.key_passwd_length = password_len;

    memcpy(req.bssid, self->network.mac, WIFI_MAC_ADDR_LEN);

    /* Following copied from WiFi shell sample */
    if (req.ssid_length > WIFI_SSID_MAX_LEN) {
        mp_raise_ValueError(MP_ERROR_TEXT("SSID is too long"));
    }

    if (req.security == WIFI_SECURITY_TYPE_SAE_HNP ||
        req.security == WIFI_SECURITY_TYPE_SAE_H2E ||
        req.security == WIFI_SECURITY_TYPE_SAE_AUTO ||
        req.security == WIFI_SECURITY_TYPE_OWE) {
        req.mfp = WIFI_MFP_REQUIRED;
    }

    if (req.security == WIFI_SECURITY_TYPE_SAE_HNP ||
        req.security == WIFI_SECURITY_TYPE_SAE_H2E ||
        req.security == WIFI_SECURITY_TYPE_SAE_AUTO ||
        req.security == WIFI_SECURITY_TYPE_FT_SAE ||
        req.security == WIFI_SECURITY_TYPE_SAE_EXT_KEY ||
        req.security == WIFI_SECURITY_TYPE_OWE) {
        req.psk = NULL;
        req.psk_length = 0;
    }

    if (req.channel != WIFI_CHANNEL_ANY && req.band != WIFI_FREQ_BAND_UNKNOWN) {
        if (!wifi_utils_validate_chan(req.band, req.channel)) {
            mp_raise_ValueError(MP_ERROR_TEXT("Band and channel combination is invalid"));
        }
    }

    if (atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_AP)) {
        ret = net_mgmt(NET_REQUEST_WIFI_AP_ENABLE, self->nic.net_if, &req,
            sizeof(struct wifi_connect_req_params));
        if (ret != 0) {
            mp_raise_msg_varg(&mp_type_RuntimeError,
                MP_ERROR_TEXT("Failed to activate AP: %d"), ret);
        }
    } else {
        atomic_clear_bit(&self->state, ZEPHYR_WLAN_STATE_STA_FAIL);
        atomic_clear_bit(&self->state, ZEPHYR_WLAN_STATE_STA_BADPW);
        ret = net_mgmt(NET_REQUEST_WIFI_CONNECT, self->nic.net_if, &req,
            sizeof(struct wifi_connect_req_params));
        if (ret != 0) {
            mp_raise_msg_varg(&mp_type_RuntimeError,
                MP_ERROR_TEXT("Failed to start connection: %d"), ret);
        }
    }

    return ret;
}

static mp_obj_t network_zephyr_wlan_active(size_t n_args, const mp_obj_t *args) {
    zephyr_wlan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    int ret;

    if (n_args > 1) {
        if (mp_obj_is_true(args[1])) {

            zephyr_wlan_updown(self, true);

            /* Only trigger when activating in AP mode */
            if (atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_AP)) {
                ret = network_zephyr_wlan_configure(self);
                if (ret != 0) {
                    mp_raise_msg_varg(&mp_type_RuntimeError,
                        MP_ERROR_TEXT("Failed to configure network: %d"), ret);
                }
            }
        } else {
            if (atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_AP)) {
                ret = net_mgmt(NET_REQUEST_WIFI_AP_DISABLE, self->nic.net_if, NULL, 0);
                if (ret != 0) {
                    mp_raise_msg_varg(&mp_type_RuntimeError,
                        MP_ERROR_TEXT("Failed to disable AP: %d"), ret);
                }
            } else {
                ret = net_mgmt(NET_REQUEST_WIFI_DISCONNECT, self->nic.net_if, NULL, 0);
                if (ret != 0 && ret != -EALREADY) {
                    mp_raise_msg_varg(&mp_type_RuntimeError,
                        MP_ERROR_TEXT("Failed to disconnect: %d"), ret);
                }
            }
            zephyr_wlan_updown(self, false);
        }
    } else {
        if (atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_ACTIVE)) {
            return mp_const_true;
        }
        return mp_const_false;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_zephyr_wlan_active_obj, 1, 2, network_zephyr_wlan_active);

static mp_obj_t network_zephyr_wlan_scan(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    zephyr_wlan_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    int ret;
    enum { ARG_type, ARG_bands };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_type, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_bands, MP_ARG_INT, {.u_int = WIFI_FREQ_BAND_ALL} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, &pos_args[1], kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    struct wifi_scan_params params = { 0 };

    params.scan_type = WIFI_SCAN_TYPE_ACTIVE;
    params.bands = args[ARG_bands].u_int;

    if (args[ARG_type].u_obj != MP_OBJ_NULL) {
        if (strcmp(mp_obj_str_get_str(args[ARG_type].u_obj), "passive") == 0) {
            params.scan_type = WIFI_SCAN_TYPE_PASSIVE;
        } else if (strcmp(mp_obj_str_get_str(args[ARG_type].u_obj), "active") == 0) {
            params.scan_type = WIFI_SCAN_TYPE_ACTIVE;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("type must be \"passive\" or \"active\""));
        }
    }

    zephyr_wlan_ready(self);

    self->scan_list = mp_obj_new_list(0, NULL);

    ret = net_mgmt(NET_REQUEST_WIFI_SCAN, self->nic.net_if, &params, sizeof(params));
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_RuntimeError,
            MP_ERROR_TEXT("Failed to start scan: %d"), ret);
    }

    MP_THREAD_GIL_EXIT();
    ret = net_mgmt_event_wait_on_iface(self->nic.net_if, NET_EVENT_WIFI_SCAN_DONE, NULL, NULL, 0,
        K_MSEC(ZEPHYR_WLAN_STA_SCAN_TIMEOUT_MS));
    MP_THREAD_GIL_ENTER();
    if (ret == -ETIMEDOUT) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Scan timed out"));
    } else if (ret != 0) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to scan: %d"), ret);
    }

    mp_handle_pending(true);

    return self->scan_list;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_zephyr_wlan_scan_obj, 1, network_zephyr_wlan_scan);

static mp_obj_t network_zephyr_wlan_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_key, ARG_bssid, ARG_band, ARG_security, ARG_key_pbkdf2 };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_key, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_band, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = WIFI_FREQ_BAND_UNKNOWN} },
        { MP_QSTR_security, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_key_pbkdf2, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, &pos_args[1], kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_ssid].u_obj != MP_OBJ_NULL) {
        if (!mp_obj_is_str(args[ARG_ssid].u_obj)) {
            mp_raise_TypeError(MP_ERROR_TEXT("SSID must be str"));
        }
    }

    if (args[ARG_bssid].u_obj != MP_OBJ_NULL) {
        if (!mp_obj_is_str_or_bytes(args[ARG_bssid].u_obj)) {
            mp_raise_TypeError(MP_ERROR_TEXT("BSSID must be bytes"));
        }
    }

    if (args[ARG_ssid].u_obj == MP_OBJ_NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("SSID must be provided"));
    }

    if (args[ARG_key].u_obj != MP_OBJ_NULL) {
        if (!mp_obj_is_str_or_bytes(args[ARG_key].u_obj)) {
            mp_raise_TypeError(MP_ERROR_TEXT("Key must be bytes or str"));
        }
    }

    if (args[ARG_key_pbkdf2].u_obj != MP_OBJ_NULL) {
        if (!mp_obj_is_str_or_bytes(args[ARG_key_pbkdf2].u_obj)
            || mp_obj_get_int(mp_obj_len(args[ARG_key_pbkdf2].u_obj)) != WIFI_PSK_PBKDF2_KEY_LEN) {
            mp_raise_TypeError(MP_ERROR_TEXT("PBKDF2 key must be 32 bytes"));
        }
        if (args[ARG_key_pbkdf2].u_obj != MP_OBJ_NULL && args[ARG_key].u_obj != MP_OBJ_NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("Only one key must be provided"));
        }
    }

    zephyr_wlan_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    zephyr_wlan_ready(self);

    atomic_clear_bit(&self->state, ZEPHYR_WLAN_STATE_STA_BADAP);

    /* Try to figure out security */
    if (args[ARG_security].u_int == -1) {
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t iterable = mp_getiter(self->scan_list, &iter_buf);
        mp_obj_t item;
        bool scanned = false;

        while (true) {
            while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
                if (args[ARG_bssid].u_obj != MP_OBJ_NULL) {
                    mp_obj_t bssid = mp_obj_tuple_subscr(item, mp_obj_new_int(1), MP_OBJ_SENTINEL);
                    if (mp_obj_equal(bssid, args[ARG_bssid].u_obj)) {
                        args[ARG_security].u_int = mp_obj_get_int(mp_obj_tuple_subscr(item, mp_obj_new_int(6), MP_OBJ_SENTINEL));
                        break;
                    }
                } else if (args[ARG_ssid].u_obj != MP_OBJ_NULL) {
                    mp_obj_t ssid = mp_obj_tuple_subscr(item, mp_obj_new_int(0), MP_OBJ_SENTINEL);
                    if (mp_obj_equal(ssid, args[ARG_ssid].u_obj)) {
                        args[ARG_security].u_int = mp_obj_get_int(mp_obj_tuple_subscr(item, mp_obj_new_int(6), MP_OBJ_SENTINEL));
                        break;
                    }
                }
            }

            /* Not figured out, try scanning */
            if (args[ARG_security].u_int == -1 && !scanned) {
                mp_obj_t scan_args[2];
                mp_load_method(pos_args[0], MP_QSTR_scan, scan_args);
                mp_call_method_n_kw(0, 0, scan_args);
                scanned = true;
                iterable = mp_getiter(self->scan_list, &iter_buf);
            } else if (args[ARG_security].u_int == -1 && scanned) {
                atomic_set_bit(&self->state, ZEPHYR_WLAN_STATE_STA_BADAP);
                mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Unable to identify security type"));
            } else {
                break;
            }
        }
    } else {
        args[ARG_security].u_int = zephyr_wlan_security(args[ARG_security].u_int);

        /* Opportunistically attempt to prevent security type confusion */
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t iterable = mp_getiter(self->scan_list, &iter_buf);
        mp_obj_t item;
        int scan_sec = -1;

        while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            if (args[ARG_bssid].u_obj != MP_OBJ_NULL) {
                mp_obj_t bssid = mp_obj_tuple_subscr(item, mp_obj_new_int(1), MP_OBJ_SENTINEL);
                if (mp_obj_equal(bssid, args[ARG_bssid].u_obj)) {
                    scan_sec = mp_obj_get_int(mp_obj_tuple_subscr(item, mp_obj_new_int(6), MP_OBJ_SENTINEL));
                    break;
                }
            } else if (args[ARG_ssid].u_obj != MP_OBJ_NULL) {
                mp_obj_t ssid = mp_obj_tuple_subscr(item, mp_obj_new_int(0), MP_OBJ_SENTINEL);
                if (mp_obj_equal(ssid, args[ARG_ssid].u_obj)) {
                    scan_sec = mp_obj_get_int(mp_obj_tuple_subscr(item, mp_obj_new_int(6), MP_OBJ_SENTINEL));
                    break;
                }
            }
        }

        if (scan_sec != args[ARG_security].u_int && scan_sec != -1) {
            mp_warning(MP_WARN_CAT(RuntimeWarning),
                "known security type for network (%d) doesn't match provided type (%d), this function expects Zephyr WLAN security constants",
                scan_sec, args[ARG_security].u_int);
        }
    }

    if (args[ARG_bssid].u_obj != MP_OBJ_NULL) {
        size_t len = WIFI_MAC_ADDR_LEN;
        const char *data = mp_obj_str_get_data(args[ARG_bssid].u_obj, &len);
        if (len != 6) {
            mp_raise_ValueError(MP_ERROR_TEXT("BSSID is not 6 bytes long"));
        }
        memcpy(self->network.mac, data, WIFI_MAC_ADDR_LEN);
    } else {
        memset(self->network.mac, 0, WIFI_MAC_ADDR_LEN);
    }

    if (args[ARG_key].u_obj != MP_OBJ_NULL) {
        self->network.password = args[ARG_key].u_obj;
        self->network.password_is_pbkdf2 = false;
    } else if (args[ARG_key_pbkdf2].u_obj != MP_OBJ_NULL) {
        self->network.password = args[ARG_key_pbkdf2].u_obj;
        self->network.password_is_pbkdf2 = true;
    } else {
        self->network.password = mp_const_empty_bytes;
    }

    self->network.band = zephyr_wlan_band(args[ARG_band].u_int);
    self->network.security = args[ARG_security].u_int;
    self->network.ssid = args[ARG_ssid].u_obj;

    int ret = network_zephyr_wlan_configure(self);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_RuntimeError,
            MP_ERROR_TEXT("Failed to configure network: %d"), ret);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_zephyr_wlan_connect_obj, 1, network_zephyr_wlan_connect);

static mp_obj_t network_zephyr_wlan_disconnect(mp_obj_t self_in) {
    zephyr_wlan_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret;

    zephyr_wlan_ready(self);

    ret = net_mgmt(NET_REQUEST_WIFI_DISCONNECT, self->nic.net_if, NULL, 0);
    if (ret == -EALREADY || !atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_STA_CONNECTED)) {
        return mp_const_none;
    } else if (ret != 0) {
        mp_raise_msg_varg(&mp_type_RuntimeError,
            MP_ERROR_TEXT("Failed to disconnect: %d"), ret);
    }

    MP_THREAD_GIL_EXIT();
    ret = net_mgmt_event_wait_on_iface(self->nic.net_if, NET_EVENT_WIFI_DISCONNECT_RESULT,
        NULL, NULL, 0, K_SECONDS(ZEPHYR_WLAN_STA_CONNECT_TIMEOUT_S));
    MP_THREAD_GIL_ENTER();
    if (ret == -ETIMEDOUT) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Disconnection timed out"));
    } else if (ret != 0) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to disconnect: %d"), ret);
    }

    mp_handle_pending(true);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_zephyr_wlan_disconnect_obj, network_zephyr_wlan_disconnect);

static mp_obj_t network_zephyr_wlan_isconnected(mp_obj_t self_in) {
    zephyr_wlan_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_STA_CONNECTED)) {
        return mp_const_true;
    }

    if (mp_obj_get_int(mp_obj_len(self->remote_sta_list)) > 0) {
        return mp_const_true;
    }

    return mp_const_false;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_zephyr_wlan_isconnected_obj, network_zephyr_wlan_isconnected);

static mp_obj_t network_zephyr_wlan_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    if (n_args != 1 && kwargs->used != 0) {
        mp_raise_TypeError(MP_ERROR_TEXT("either pos or kw args are allowed"));
    }

    if (n_args == 1 && kwargs->used == 0) {
        /* List supported keys */
        mp_obj_t tobj[] = {
            MP_OBJ_NEW_QSTR(MP_QSTR_mac),
            MP_OBJ_NEW_QSTR(MP_QSTR_ssid),
            MP_OBJ_NEW_QSTR(MP_QSTR_security),
            MP_OBJ_NEW_QSTR(MP_QSTR_channel),
            MP_OBJ_NEW_QSTR(MP_QSTR_bandwidth),
            MP_OBJ_NEW_QSTR(MP_QSTR_pbkdf2),
            MP_OBJ_NEW_QSTR(MP_QSTR_bssid),
            MP_OBJ_NEW_QSTR(MP_QSTR_mode),
            MP_OBJ_NEW_QSTR(MP_QSTR_link_mode),
            MP_OBJ_NEW_QSTR(MP_QSTR_band),
            MP_OBJ_NEW_QSTR(MP_QSTR_hidden),
        };
        return mp_obj_new_tuple(ARRAY_SIZE(tobj), tobj);
    } else if (n_args != 2 && kwargs->used == 0) {
        mp_raise_TypeError(MP_ERROR_TEXT("can query only one param"));
    }

    zephyr_wlan_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (kwargs->used != 0) {
        for (size_t i = 0; i < kwargs->alloc; i++) {
            if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
                mp_map_elem_t *e = &kwargs->table[i];
                switch (mp_obj_str_get_qstr(kwargs->table[i].key)) {
                    case MP_QSTR_mac: {
                        mp_buffer_info_t bufinfo;
                        mp_get_buffer_raise(e->value, &bufinfo, MP_BUFFER_READ);
                        if (bufinfo.len != WIFI_MAC_ADDR_LEN) {
                            mp_raise_ValueError(MP_ERROR_TEXT("invalid buffer length"));
                        }
                        memcpy(self->network.mac, bufinfo.buf, WIFI_MAC_ADDR_LEN);
                        break;
                    }
                    case MP_QSTR_ssid:
                    case MP_QSTR_essid: {
                        if (!mp_obj_is_str(e->value)) {
                            mp_raise_TypeError(MP_ERROR_TEXT("SSID must be str"));
                        }
                        self->network.ssid = e->value;
                        break;
                    }
                    case MP_QSTR_security:
                    case MP_QSTR_authmode: {
                        self->network.security = zephyr_wlan_security(mp_obj_get_int(e->value));
                        break;
                    }
                    case MP_QSTR_key:
                    case MP_QSTR_password: {
                        if (!mp_obj_is_str_or_bytes(e->value)) {
                            mp_raise_TypeError(MP_ERROR_TEXT("Key must be bytes or str"));
                        }
                        self->network.password = e->value;
                        break;
                    }
                    case MP_QSTR_channel: {
                        self->network.channel = zephyr_wlan_channel(mp_obj_get_int(e->value));
                        break;
                    }
                    case MP_QSTR_bandwidth: {
                        self->network.bandwidth = zephyr_wlan_bandwidth(mp_obj_get_int(e->value));
                        break;
                    }
                    case MP_QSTR_pbkdf2: {
                        self->network.password_is_pbkdf2 = mp_obj_is_true(e->value);
                        break;
                    }
                    case MP_QSTR_hidden: {
                        self->network.hidden = mp_obj_is_true(e->value);
                        break;
                    }
                    case MP_QSTR_hostname:
                    case MP_QSTR_dhcp_hostname: {
                        mp_raise_NotImplementedError(MP_ERROR_TEXT("interface hostname config is deprecated, use network module's"));
                        break;
                    }
                    default:
                        mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                        break;
                }
            }
        }
        return mp_const_none;
    }

    struct wifi_iface_status status = { 0 };
    int ret;

    ret = net_mgmt(NET_REQUEST_WIFI_IFACE_STATUS, self->nic.net_if, &status, sizeof(struct wifi_iface_status));
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to fetch status: %d"), ret);
    }

    /* Live status */
    if (atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_STA_CONNECTED) || atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_AP_UP)) {
        switch (mp_obj_str_get_qstr(args[1])) {
            #ifdef CONFIG_HWINFO
            case MP_QSTR_mac:
                return mp_obj_new_bytes(zephyr_wlan_mac(), WIFI_MAC_ADDR_LEN);
            #else
            case MP_QSTR_mac:
                #endif
            case MP_QSTR_bssid:
                return mp_obj_new_bytes(status.bssid, WIFI_MAC_ADDR_LEN);
            case MP_QSTR_ssid:
            case MP_QSTR_essid:
                return mp_obj_new_str_from_cstr(status.ssid);
            case MP_QSTR_security:
            case MP_QSTR_authmode:
                return mp_obj_new_int(status.security);
            case MP_QSTR_key:
            case MP_QSTR_password:
                return self->network.password;
            case MP_QSTR_channel:
                return mp_obj_new_int(status.channel);
            case MP_QSTR_band:
                return mp_obj_new_int(status.band);
            case MP_QSTR_mode:
                return mp_obj_new_int(status.iface_mode);
            case MP_QSTR_link_mode:
            case MP_QSTR_protocol:
                return mp_obj_new_int(status.link_mode);
            case MP_QSTR_pbkdf2:
                if (self->network.password_is_pbkdf2) {
                    return mp_const_true;
                } else {
                    return mp_const_false;
                }
            case MP_QSTR_hostname:
            case MP_QSTR_dhcp_hostname: {
                mp_raise_NotImplementedError(MP_ERROR_TEXT("interface hostname config is deprecated, use network module's"));
                break;
            }
            default:
                mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                break;
        }
        /* Theoretical status */
    } else {
        switch (mp_obj_str_get_qstr(args[1])) {
            #ifdef CONFIG_HWINFO
            case MP_QSTR_mac:
                return mp_obj_new_bytes(zephyr_wlan_mac(), WIFI_MAC_ADDR_LEN);
            #else
            case MP_QSTR_mac:
                #endif
            case MP_QSTR_bssid:
                return mp_obj_new_bytes(self->network.mac, WIFI_MAC_ADDR_LEN);
            case MP_QSTR_ssid:
            case MP_QSTR_essid:
                return self->network.ssid;
            case MP_QSTR_security:
            case MP_QSTR_authmode:
                return mp_obj_new_int(self->network.security);
            case MP_QSTR_key:
            case MP_QSTR_password:
                return self->network.password;
            case MP_QSTR_channel:
                return mp_obj_new_int(self->network.channel);
            case MP_QSTR_band:
                return mp_obj_new_int(self->network.band);
            case MP_QSTR_mode:
                if (atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_AP)) {
                    return mp_obj_new_int(WIFI_MODE_AP);
                } else {
                    return mp_obj_new_int(WIFI_MODE_INFRA);
                }
            case MP_QSTR_link_mode:
            case MP_QSTR_protocol:
                return mp_obj_new_int(WIFI_LINK_MODE_UNKNOWN);
            case MP_QSTR_pbkdf2:
                if (self->network.password_is_pbkdf2) {
                    return mp_const_true;
                } else {
                    return mp_const_false;
                }
            case MP_QSTR_hostname:
            case MP_QSTR_dhcp_hostname: {
                mp_raise_NotImplementedError(MP_ERROR_TEXT("interface hostname config is deprecated, use network module's"));
                break;
            }
            default:
                mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                break;
        }
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_zephyr_wlan_config_obj, 1, network_zephyr_wlan_config);

static mp_obj_t network_zephyr_wlan_status(size_t n_args, const mp_obj_t *args) {
    zephyr_wlan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    struct wifi_iface_status status = { 0 };
    int ret;

    ret = net_mgmt(NET_REQUEST_WIFI_IFACE_STATUS, self->nic.net_if, &status, sizeof(struct wifi_iface_status));

    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to fetch status: %d"), ret);
    }

    if (n_args == 1) {
        if (!atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_AP)) {
            mp_obj_t generic = network_zephyr_status_handler(n_args, args);

            if (generic != mp_const_none) {
                return generic;
            }

            if (atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_STA_BADPW)) {
                return MP_OBJ_NEW_SMALL_INT(NETWORK_STAT_WRONG_PASSWORD);
            }
            if (atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_STA_BADAP)) {
                return MP_OBJ_NEW_SMALL_INT(NETWORK_STAT_NO_AP_FOUND);
            }
            if (atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_STA_FAIL)) {
                return MP_OBJ_NEW_SMALL_INT(NETWORK_STAT_CONNECT_FAIL);
            }
            if (atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_STA_CONNECTED)) {
                return MP_OBJ_NEW_SMALL_INT(NETWORK_STAT_CONNECTING);
            }
            return MP_OBJ_NEW_SMALL_INT(NETWORK_STAT_IDLE);
        } else {
            return self->remote_sta_list;
        }
    }

    switch (mp_obj_str_get_qstr(args[1])) {
        case MP_QSTR_iface_mode: {
            return MP_OBJ_NEW_SMALL_INT(status.iface_mode);
        }
        case MP_QSTR_current_phy_tx_rate: {
            return mp_obj_new_float(status.current_phy_tx_rate);
        }
    }

    if (!atomic_test_bit(&self->state, ZEPHYR_WLAN_STATE_AP)) {

        switch (mp_obj_str_get_qstr(args[1])) {
            case MP_QSTR_rssi: {
                return MP_OBJ_NEW_SMALL_INT(status.rssi);
            }
        }
    } else {
        switch (mp_obj_str_get_qstr(args[1])) {
            case MP_QSTR_stations: {
                return self->remote_sta_list;
            }
        }
    }

    mp_raise_ValueError(MP_ERROR_TEXT("unknown status param"));

    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_zephyr_wlan_status_obj, 1, 2, network_zephyr_wlan_status);

/* Country code is per interface, only for RF */
static mp_obj_t network_zephyr_wlan_country(size_t n_args, const mp_obj_t *args) {
    network_zephyr_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        struct wifi_reg_domain regd = { .oper = WIFI_MGMT_GET, .chan_info = NULL };
        int ret = net_mgmt(NET_REQUEST_WIFI_REG_DOMAIN, self->net_if, &regd, sizeof(regd));
        if (ret != 0) {
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to get country code: %d"), ret);
        }
        return mp_obj_new_str(regd.country_code, 2);
    } else {
        size_t len;
        const char *str = mp_obj_str_get_data(args[1], &len);
        struct wifi_reg_domain regd = {
            .oper = WIFI_MGMT_SET,
            .force = true,
        };
        if (len != 2) {
            mp_raise_ValueError(NULL);
        }
        regd.country_code[0] = str[0];
        regd.country_code[1] = str[1];
        int ret = net_mgmt(NET_REQUEST_WIFI_REG_DOMAIN, self->net_if, &regd, sizeof(regd));
        if (ret != 0) {
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to set country code: %d"), ret);
        }
    }

    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_zephyr_wlan_country_obj, 1, 2, network_zephyr_wlan_country);

static const mp_rom_map_elem_t network_zephyr_wlan_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active),              MP_ROM_PTR(&network_zephyr_wlan_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan),                MP_ROM_PTR(&network_zephyr_wlan_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect),             MP_ROM_PTR(&network_zephyr_wlan_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect),          MP_ROM_PTR(&network_zephyr_wlan_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected),         MP_ROM_PTR(&network_zephyr_wlan_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_config),              MP_ROM_PTR(&network_zephyr_wlan_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_status),              MP_ROM_PTR(&network_zephyr_wlan_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_country),             MP_ROM_PTR(&network_zephyr_wlan_country_obj) },
    /* Destructors */
    { MP_ROM_QSTR(MP_QSTR_deinit),              MP_ROM_PTR(&network_zephyr_wlan_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__),             MP_ROM_PTR(&network_zephyr_wlan_deinit_obj) },
    /* Commons */
    { MP_ROM_QSTR(MP_QSTR_ifconfig),            MP_ROM_PTR(&network_zephyr_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipconfig),            MP_ROM_PTR(&network_zephyr_ipconfig_obj) },
    #ifdef CONFIG_NET_DHCPV4_SERVER
    { MP_ROM_QSTR(MP_QSTR_dhcp4_server),        MP_ROM_PTR(&network_zephyr_dhcp4_server_obj) },
    #endif
    #ifdef CONFIG_NET_DHCPV6_SERVER
    { MP_ROM_QSTR(MP_QSTR_dhcp6_server),        MP_ROM_PTR(&network_zephyr_dhcp6_server_obj) },
    #endif

    /* Security type constants matching ESP32 for convenience */
    { MP_ROM_QSTR(MP_QSTR_SEC_OPEN),            MP_ROM_INT(WIFI_SECURITY_TYPE_NONE) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WEP),             MP_ROM_INT(WIFI_SECURITY_TYPE_WEP) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA),             MP_ROM_INT(WIFI_SECURITY_TYPE_WPA_PSK) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA2),            MP_ROM_INT(WIFI_SECURITY_TYPE_PSK) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA_WPA2),        MP_ROM_INT(WIFI_SECURITY_TYPE_WPA_AUTO_PERSONAL) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA3),            MP_ROM_INT(WIFI_SECURITY_TYPE_WPA_AUTO_PERSONAL) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA2_WPA3),       MP_ROM_INT(WIFI_SECURITY_TYPE_WPA_AUTO_PERSONAL) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WAPI),            MP_ROM_INT(WIFI_SECURITY_TYPE_WAPI) },
    { MP_ROM_QSTR(MP_QSTR_SEC_OWE),             MP_ROM_INT(WIFI_SECURITY_TYPE_OWE) },
    { MP_ROM_QSTR(MP_QSTR_SEC_DPP),             MP_ROM_INT(WIFI_SECURITY_TYPE_DPP) },

    /* Security type constants matching Zephyr */
    { MP_ROM_QSTR(MP_QSTR_SEC_NONE),            MP_ROM_INT(WIFI_SECURITY_TYPE_NONE) },
    { MP_ROM_QSTR(MP_QSTR_SEC_PSK),             MP_ROM_INT(WIFI_SECURITY_TYPE_WPA_PSK) },
    { MP_ROM_QSTR(MP_QSTR_SEC_PSK_SHA256),      MP_ROM_INT(WIFI_SECURITY_TYPE_PSK_SHA256) },
    { MP_ROM_QSTR(MP_QSTR_SEC_SAE),             MP_ROM_INT(WIFI_SECURITY_TYPE_SAE) },
    { MP_ROM_QSTR(MP_QSTR_SEC_SAE_HNP),         MP_ROM_INT(WIFI_SECURITY_TYPE_SAE_HNP) },
    { MP_ROM_QSTR(MP_QSTR_SEC_SAE_AUTO),        MP_ROM_INT(WIFI_SECURITY_TYPE_SAE_AUTO) },
    { MP_ROM_QSTR(MP_QSTR_SEC_EAP),             MP_ROM_INT(WIFI_SECURITY_TYPE_EAP) },
    { MP_ROM_QSTR(MP_QSTR_SEC_EAP_TLS),         MP_ROM_INT(WIFI_SECURITY_TYPE_EAP_TLS) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA_PSK),         MP_ROM_INT(WIFI_SECURITY_TYPE_WPA_PSK) },
    { MP_ROM_QSTR(MP_QSTR_SEC_AUTO_PERSONAL),   MP_ROM_INT(WIFI_SECURITY_TYPE_WPA_AUTO_PERSONAL) },
    { MP_ROM_QSTR(MP_QSTR_SEC_EAP_PEAP_MSCHAPV2),
      MP_ROM_INT(WIFI_SECURITY_TYPE_EAP_PEAP_MSCHAPV2) },
    { MP_ROM_QSTR(MP_QSTR_SEC_EAP_PEAP_GTC),    MP_ROM_INT(WIFI_SECURITY_TYPE_EAP_PEAP_GTC) },
    { MP_ROM_QSTR(MP_QSTR_SEC_EAP_TTLS_MSCHAPV2),
      MP_ROM_INT(WIFI_SECURITY_TYPE_EAP_TTLS_MSCHAPV2) },
    { MP_ROM_QSTR(MP_QSTR_SEC_EAP_PEAP_TLS),    MP_ROM_INT(WIFI_SECURITY_TYPE_EAP_PEAP_TLS) },
    { MP_ROM_QSTR(MP_QSTR_SEC_FT_PSK),          MP_ROM_INT(WIFI_SECURITY_TYPE_FT_PSK) },
    { MP_ROM_QSTR(MP_QSTR_SEC_FT_SAE),          MP_ROM_INT(WIFI_SECURITY_TYPE_FT_SAE) },
    { MP_ROM_QSTR(MP_QSTR_SEC_FT_EAP),          MP_ROM_INT(WIFI_SECURITY_TYPE_FT_EAP) },
    { MP_ROM_QSTR(MP_QSTR_SEC_FT_EAP_SHA384),   MP_ROM_INT(WIFI_SECURITY_TYPE_FT_EAP_SHA384) },
    { MP_ROM_QSTR(MP_QSTR_SEC_SAE_EXT_KEY),     MP_ROM_INT(WIFI_SECURITY_TYPE_SAE_EXT_KEY) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WEP_OPEN),        MP_ROM_INT(WIFI_SECURITY_TYPE_WEP_OPEN) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WEP_SHARED),      MP_ROM_INT(WIFI_SECURITY_TYPE_WEP_SHARED) },

    /* Bandwidth constants */
    { MP_ROM_QSTR(MP_QSTR_BANDWIDTH_20),        MP_ROM_INT(WIFI_FREQ_BANDWIDTH_20MHZ) },
    { MP_ROM_QSTR(MP_QSTR_BANDWIDTH_40),        MP_ROM_INT(WIFI_FREQ_BANDWIDTH_40MHZ) },
    { MP_ROM_QSTR(MP_QSTR_BANDWIDTH_80),        MP_ROM_INT(WIFI_FREQ_BANDWIDTH_80MHZ) },
    { MP_ROM_QSTR(MP_QSTR_BANDWIDTH_UNKNOWN),   MP_ROM_INT(WIFI_FREQ_BANDWIDTH_UNKNOWN) },

    /* Band constants */
    { MP_ROM_QSTR(MP_QSTR_BAND_2_4_GHZ),        MP_ROM_INT(WIFI_FREQ_BAND_2_4_GHZ) },
    { MP_ROM_QSTR(MP_QSTR_BAND_5_GHZ),          MP_ROM_INT(WIFI_FREQ_BAND_5_GHZ) },
    { MP_ROM_QSTR(MP_QSTR_BAND_6_GHZ),          MP_ROM_INT(WIFI_FREQ_BAND_6_GHZ) },
    { MP_ROM_QSTR(MP_QSTR_BAND_UNKNOWN),        MP_ROM_INT(WIFI_FREQ_BAND_UNKNOWN) },

    /* Band constants masks used when scanning */
    { MP_ROM_QSTR(MP_QSTR_SCAN_BAND_2_4_GHZ),   MP_ROM_INT(BIT(WIFI_FREQ_BAND_2_4_GHZ)) },
    { MP_ROM_QSTR(MP_QSTR_SCAN_BAND_5_GHZ),     MP_ROM_INT(BIT(WIFI_FREQ_BAND_5_GHZ)) },
    { MP_ROM_QSTR(MP_QSTR_SCAN_BAND_6_GHZ),     MP_ROM_INT(BIT(WIFI_FREQ_BAND_6_GHZ)) },
    { MP_ROM_QSTR(MP_QSTR_SCAN_BAND_ALL),       MP_ROM_INT(WIFI_FREQ_BAND_ALL) },

    { MP_ROM_QSTR(MP_QSTR_CHANNEL_ANY),         MP_ROM_INT(WIFI_CHANNEL_ANY) },
};
static MP_DEFINE_CONST_DICT(network_zephyr_wlan_locals_dict, network_zephyr_wlan_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mod_network_zephyr_wlan_type,
    MP_QSTR_WLAN,
    MP_TYPE_FLAG_NONE,
    make_new, network_zephyr_wlan_make_new,
    locals_dict, &network_zephyr_wlan_locals_dict
    );

#endif /* MICROPY_PY_ZEPHYR_NETWORK_WLAN */
