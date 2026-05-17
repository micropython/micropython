/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
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
 *
 * NINA-W10 Python module.
 */

#include "py/mphal.h"

#if MICROPY_PY_NETWORK && MICROPY_PY_NETWORK_NINAW10

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#include "py/objtuple.h"
#include "py/objlist.h"
#include "py/stream.h"
#include "py/runtime.h"
#include "py/misc.h"
#include "py/mperrno.h"
#include "py/parsenum.h"
#include "shared/netutils/netutils.h"
#include "shared/runtime/softtimer.h"
#include "extmod/modnetwork.h"
#include "modmachine.h"

#include "nina_wifi_drv.h"

typedef struct _nina_obj_t {
    mp_obj_base_t base;
    bool active;
    bool poll_enable;
    uint32_t itf;
    mp_uint_t security;
    char ssid[NINA_MAX_SSID_LEN + 1];
    char key[NINA_MAX_WPA_LEN + 1];
} nina_obj_t;

// For auto-binding UDP sockets
#define BIND_PORT_RANGE_MIN     (65000)
#define BIND_PORT_RANGE_MAX     (65535)

#define SOCKET_IOCTL_FIONREAD   (0x4004667F)
#define SOCKET_IOCTL_FIONBIO    (0x8004667E)

#define SOCKET_POLL_RD          (0x01)
#define SOCKET_POLL_WR          (0x02)
#define SOCKET_POLL_ERR         (0x04)

#define SO_ACCEPTCONN           (0x0002)
#define SO_ERROR                (0x1007)
#define SO_TYPE                 (0x1008)
#define SO_NO_CHECK             (0x100a)
#define NINAW10_POLL_INTERVAL   (100)

#define IPADDR_STRLEN_MAX       46

#define is_nonblocking_error(errno) ((errno) == MP_EAGAIN || (errno) == MP_EWOULDBLOCK || (errno) == MP_EINPROGRESS)

#define debug_printf(...) // mp_printf(&mp_plat_print, __VA_ARGS__)

static uint16_t bind_port = BIND_PORT_RANGE_MIN;
const mp_obj_type_t mod_network_nic_type_nina;
static nina_obj_t network_nina_wl_sta = {{(mp_obj_type_t *)&mod_network_nic_type_nina}, false, false, MOD_NETWORK_STA_IF};
static nina_obj_t network_nina_wl_ap = {{(mp_obj_type_t *)&mod_network_nic_type_nina}, false, false, MOD_NETWORK_AP_IF};
static mp_sched_node_t mp_wifi_poll_node;
static soft_timer_entry_t mp_wifi_poll_timer;
static void network_ninaw10_deinit(void);

static bool network_ninaw10_dhcp_active = false;

static bool network_ninaw10_poll_list_is_empty(void) {
    return MP_STATE_PORT(mp_wifi_poll_list) == NULL ||
           MP_STATE_PORT(mp_wifi_poll_list)->len == 0;
}

static void network_ninaw10_poll_list_insert(mp_obj_t socket) {
    if (MP_STATE_PORT(mp_wifi_poll_list) == NULL) {
        MP_STATE_PORT(mp_wifi_poll_list) = mp_obj_new_list(0, NULL);
    }
    mp_obj_list_append(MP_STATE_PORT(mp_wifi_poll_list), socket);
}

static void network_ninaw10_poll_list_remove(mp_obj_t socket) {
    if (MP_STATE_PORT(mp_wifi_poll_list) == NULL) {
        return;
    }
    mp_obj_list_remove(MP_STATE_PORT(mp_wifi_poll_list), socket);
    if (MP_STATE_PORT(mp_wifi_poll_list)->len == 0) {
        MP_STATE_PORT(mp_wifi_poll_list) = NULL;
    }
}

static void network_ninaw10_poll_sockets(mp_sched_node_t *node) {
    (void)node;
    for (mp_uint_t i = 0; MP_STATE_PORT(mp_wifi_poll_list) && i < MP_STATE_PORT(mp_wifi_poll_list)->len;) {
        mod_network_socket_obj_t *socket = MP_STATE_PORT(mp_wifi_poll_list)->items[i];
        uint8_t flags = 0;
        if (socket->callback == MP_OBJ_NULL || nina_socket_poll(socket->fileno, &flags) < 0) {
            // Remove socket from poll list on error.
            socket->callback = MP_OBJ_NULL;
            network_ninaw10_poll_list_remove(socket);
            // Check the same index on the next iteration.
            continue;
        }
        if (flags & SOCKET_POLL_RD) {
            mp_call_function_1(socket->callback, MP_OBJ_FROM_PTR(socket));
        }
        i++;
        debug_printf("poll_sockets(%d) -> flags %d\n", socket->fileno, flags);
    }

    if (!network_ninaw10_poll_list_is_empty()) {
        // Reschedule the sockets polling code.
        soft_timer_reinsert(&mp_wifi_poll_timer, NINAW10_POLL_INTERVAL);
    }
}

static void network_ninaw10_poll_connect(mp_sched_node_t *node) {
    nina_obj_t *self = &network_nina_wl_sta;

    int status = nina_connection_status();
    if (status == NINA_STATUS_CONNECTED) {
        // Connected to AP, nothing else to do.
        self->poll_enable = false;
    } else if (status != NINA_STATUS_NO_SSID_AVAIL) {
        // If not connected, and no connection in progress, the connection attempt has failed.
        // Read the ESP failure reason, reconnect and reschedule the connection polling code.
        int reason = nina_connection_reason();
        if (reason == NINA_ESP_REASON_AUTH_EXPIRE ||
            reason == NINA_ESP_REASON_ASSOC_EXPIRE ||
            reason == NINA_ESP_REASON_NOT_AUTHED ||
            reason == NINA_ESP_REASON_4WAY_HANDSHAKE_TIMEOUT ||
            reason >= NINA_ESP_REASON_BEACON_TIMEOUT) {
            debug_printf("poll_connect() status: %d reason %d\n", status, reason);
            if (nina_connect(self->ssid, self->security, self->key, 0) != 0) {
                mp_raise_msg_varg(&mp_type_OSError,
                    MP_ERROR_TEXT("could not connect to ssid=%s, sec=%d, key=%s"),
                    self->ssid, self->security, self->key);
            }
        } else {
            // Will not attempt to reconnect if there's another error code set.
            self->poll_enable = false;
        }
    }

    // Reinsert the timer to schedule the polling code.
    soft_timer_reinsert(&mp_wifi_poll_timer, NINAW10_POLL_INTERVAL);
}

static void network_ninaw10_timer_callback(soft_timer_entry_t *self) {
    debug_printf("timer_callback() poll status STA: %d AP: %d SOCKETS: %d\n",
        network_nina_wl_sta.poll_enable, network_nina_wl_ap.poll_enable, !network_ninaw10_poll_list_is_empty());
    if (network_nina_wl_sta.poll_enable) {
        mp_sched_schedule_node(&mp_wifi_poll_node, network_ninaw10_poll_connect);
    } else if (!network_ninaw10_poll_list_is_empty()) {
        mp_sched_schedule_node(&mp_wifi_poll_node, network_ninaw10_poll_sockets);
    }
}

static mp_obj_t network_ninaw10_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_t nina_obj;
    if (n_args == 0 || mp_obj_get_int(args[0]) == MOD_NETWORK_STA_IF) {
        nina_obj = MP_OBJ_FROM_PTR(&network_nina_wl_sta);
    } else {
        nina_obj = MP_OBJ_FROM_PTR(&network_nina_wl_ap);
    }
    // Register with network module
    mod_network_register_nic(nina_obj);
    return nina_obj;
}

static mp_obj_t network_ninaw10_active(size_t n_args, const mp_obj_t *args) {
    nina_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 2) {
        bool active = mp_obj_is_true(args[1]);
        if (active && !self->active) {
            int error = 0;
            if ((error = nina_init()) != 0) {
                mp_raise_msg_varg(&mp_type_OSError,
                    MP_ERROR_TEXT("failed to initialize Nina-W10 module, error: %d"), error);
            }
            network_ninaw10_dhcp_active = true;
            // check firmware version
            uint8_t semver[NINA_FW_VER_LEN];
            if (nina_fw_version(semver) != 0) {
                nina_deinit();
                mp_raise_msg_varg(&mp_type_OSError,
                    MP_ERROR_TEXT("failed to read firmware version, error: %d"), error);
            }
            // Check the minimum supported firmware version.
            uint32_t fwmin = (NINA_FW_VER_MIN_MAJOR * 100) +
                (NINA_FW_VER_MIN_MINOR * 10) +
                (NINA_FW_VER_MIN_PATCH * 1);

            uint32_t fwver = (semver[NINA_FW_VER_MAJOR_OFFS] - 48) * 100 +
                (semver[NINA_FW_VER_MINOR_OFFS] - 48) * 10 +
                (semver[NINA_FW_VER_PATCH_OFFS] - 48) * 1;

            if (fwver < fwmin) {
                mp_raise_msg_varg(&mp_type_OSError,
                    MP_ERROR_TEXT("firmware version mismatch, minimum supported firmware is v%d.%d.%d found v%d.%d.%d"),
                    NINA_FW_VER_MIN_MAJOR, NINA_FW_VER_MIN_MINOR, NINA_FW_VER_MIN_PATCH, semver[NINA_FW_VER_MAJOR_OFFS] - 48,
                    semver[NINA_FW_VER_MINOR_OFFS] - 48, semver[NINA_FW_VER_PATCH_OFFS] - 48);
            }
            soft_timer_static_init(&mp_wifi_poll_timer, SOFT_TIMER_MODE_ONE_SHOT, 0, network_ninaw10_timer_callback);
        } else if (!active && self->active) {
            network_ninaw10_deinit();
            nina_deinit();
        }
        self->active = active;
        return mp_const_none;
    }
    return mp_obj_new_bool(self->active);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_ninaw10_active_obj, 1, 2, network_ninaw10_active);

static int nina_scan_callback(nina_scan_result_t *scan_result, void *arg) {
    mp_obj_t scan_list = (mp_obj_t)arg;
    mp_obj_t ap[6] = {
        mp_obj_new_bytes((uint8_t *)scan_result->ssid, strlen(scan_result->ssid)),
        mp_obj_new_bytes(scan_result->bssid, sizeof(scan_result->bssid)),
        mp_obj_new_int(scan_result->channel),
        mp_obj_new_int(scan_result->rssi),
        mp_obj_new_int(scan_result->security),
        MP_OBJ_NEW_SMALL_INT(1), // N
    };
    mp_obj_list_append(scan_list, mp_obj_new_tuple(MP_ARRAY_SIZE(ap), ap));
    return 0;
}

static mp_obj_t network_ninaw10_scan(mp_obj_t self_in) {
    mp_obj_t scan_list;
    scan_list = mp_obj_new_list(0, NULL);
    nina_scan(nina_scan_callback, scan_list, 10000);
    return scan_list;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_ninaw10_scan_obj, network_ninaw10_scan);

static mp_obj_t network_ninaw10_connect(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_key, ARG_security, ARG_channel };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_key,      MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_security, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = NINA_SEC_WPA_PSK} },
        { MP_QSTR_channel,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
    };

    // parse args
    nina_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get ssid
    const char *ssid = mp_obj_str_get_str(args[ARG_ssid].u_obj);
    if (strlen(ssid) == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("SSID can't be empty"));
    }

    // get encryption key
    const char *key = NULL;
    if (args[ARG_key].u_obj != mp_const_none) {
        key = mp_obj_str_get_str(args[ARG_key].u_obj);
    }

    // get security mode
    mp_uint_t security = args[ARG_security].u_int;
    // Ensure that the key is not empty if a security mode is used.
    if (security != NINA_SEC_OPEN && strlen(key) == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("key can't be empty"));
    }

    // Activate the interface if not active.
    if (!self->active) {
        network_ninaw10_active(2, (mp_obj_t [2]) { pos_args[0], mp_const_true });
    }

    // Disconnect active connections first.
    if (nina_isconnected()) {
        nina_disconnect();
    }

    if (self->itf == MOD_NETWORK_STA_IF) {
        // Initialize WiFi in Station mode.
        if (nina_connect(ssid, security, key, 0) != 0) {
            mp_raise_msg_varg(&mp_type_OSError,
                MP_ERROR_TEXT("could not connect to ssid=%s, sec=%d, key=%s"), ssid, security, key);
        }

        // Save connection info to re-connect if needed.
        self->security = security;
        strncpy(self->key, key, NINA_MAX_WPA_LEN);
        strncpy(self->ssid, ssid, NINA_MAX_SSID_LEN);
        self->poll_enable = true;
        soft_timer_reinsert(&mp_wifi_poll_timer, NINAW10_POLL_INTERVAL);
    } else {
        mp_uint_t channel = args[ARG_channel].u_int;
        // Initialize WiFi in AP mode.
        if (nina_start_ap(ssid, security, key, channel) != 0) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("failed to start in AP mode"));
        }
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_ninaw10_connect_obj, 1, network_ninaw10_connect);

static mp_obj_t network_ninaw10_disconnect(mp_obj_t self_in) {
    nina_disconnect();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_ninaw10_disconnect_obj, network_ninaw10_disconnect);

static mp_obj_t network_ninaw10_isconnected(mp_obj_t self_in) {
    return mp_obj_new_bool(nina_isconnected());
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_ninaw10_isconnected_obj, network_ninaw10_isconnected);

static mp_obj_t network_ninaw10_ifconfig(size_t n_args, const mp_obj_t *args) {
    nina_ifconfig_t ifconfig;
    if (n_args == 1) {
        // get ifconfig info
        nina_ifconfig(&ifconfig, false);
        mp_obj_t tuple[4] = {
            netutils_format_ipv4_addr(ifconfig.ip_addr, NETUTILS_BIG),
            netutils_format_ipv4_addr(ifconfig.subnet_addr, NETUTILS_BIG),
            netutils_format_ipv4_addr(ifconfig.gateway_addr, NETUTILS_BIG),
            netutils_format_ipv4_addr(ifconfig.dns_addr, NETUTILS_BIG),
        };
        return mp_obj_new_tuple(4, tuple);
    } else {
        // set ifconfig info
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 4, &items);
        netutils_parse_ipv4_addr(items[0], ifconfig.ip_addr, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[1], ifconfig.subnet_addr, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[2], ifconfig.gateway_addr, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[3], ifconfig.dns_addr, NETUTILS_BIG);
        nina_ifconfig(&ifconfig, true);
        network_ninaw10_dhcp_active = false;
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_ninaw10_ifconfig_obj, 1, 2, network_ninaw10_ifconfig);

mp_obj_t network_ninaw10_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    nina_ifconfig_t ifconfig;
    // get ifconfig info
    nina_ifconfig(&ifconfig, false);

    if (kwargs->used == 0) {
        // Get config value
        if (n_args != 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }
        switch (mp_obj_str_get_qstr(args[0])) {
            case MP_QSTR_dns: {
                return netutils_format_ipv4_addr(ifconfig.dns_addr, NETUTILS_BIG);
            }
            default: {
                mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                break;
            }
        }
    } else {
        // Set config value(s)
        if (n_args != 0) {
            mp_raise_TypeError(MP_ERROR_TEXT("can't specify pos and kw args"));
        }

        for (size_t i = 0; i < kwargs->alloc; ++i) {
            if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
                mp_map_elem_t *e = &kwargs->table[i];
                switch (mp_obj_str_get_qstr(e->key)) {
                    case MP_QSTR_dns: {
                        netutils_parse_ipv4_addr(e->value, ifconfig.dns_addr, NETUTILS_BIG);
                        nina_ifconfig(&ifconfig, true);
                        break;
                    }
                    default: {
                        mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                        break;
                    }
                }
            }
        }
    }
    return mp_const_none;
}

static mp_obj_t network_ninaw10_nic_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    nina_ifconfig_t ifconfig;
    // get ifconfig info
    nina_ifconfig(&ifconfig, false);

    if (kwargs->used == 0) {
        // Get config value
        if (n_args != 2) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }

        switch (mp_obj_str_get_qstr(args[1])) {
            case MP_QSTR_dhcp4: {
                return mp_obj_new_bool(network_ninaw10_dhcp_active);
            }
            case MP_QSTR_has_dhcp4: {
                uint16_t ip_sum =
                    ifconfig.ip_addr[0] + ifconfig.ip_addr[1] + ifconfig.ip_addr[2] + ifconfig.ip_addr[3];
                if (network_ninaw10_dhcp_active) {
                    return mp_obj_new_bool(ip_sum != 0);
                } else {
                    return mp_const_false;
                }
            }
            case MP_QSTR_addr4: {
                mp_obj_t tuple[2] = {
                    netutils_format_ipv4_addr(ifconfig.ip_addr, NETUTILS_BIG),
                    netutils_format_ipv4_addr(ifconfig.subnet_addr, NETUTILS_BIG),
                };
                return mp_obj_new_tuple(2, tuple);
            }
            case MP_QSTR_gw4: {
                return netutils_format_ipv4_addr(ifconfig.gateway_addr, NETUTILS_BIG);
            }
            default: {
                mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                break;
            }
        }
        return mp_const_none;
    } else {
        // Set config value(s)
        if (n_args != 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("can't specify pos and kw args"));
        }

        for (size_t i = 0; i < kwargs->alloc; ++i) {
            if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
                mp_map_elem_t *e = &kwargs->table[i];
                switch (mp_obj_str_get_qstr(e->key)) {
                    case MP_QSTR_dhcp4: {
                        mp_raise_ValueError(MP_ERROR_TEXT("DHCP control unsupported"));
                        break;
                    }
                    case MP_QSTR_addr4: {
                        int prefix_bits = 32;
                        if (e->value != mp_const_none && mp_obj_is_str(e->value)) {
                            size_t addr_len;
                            const char *input_str = mp_obj_str_get_data(e->value, &addr_len);
                            char *split = strchr(input_str, '/');
                            if (split) {
                                mp_obj_t prefix_obj = mp_parse_num_integer(split + 1, strlen(split + 1), 10, NULL);
                                prefix_bits = mp_obj_get_int(prefix_obj);
                                uint32_t mask = -(1u << (32 - prefix_bits));
                                ifconfig.subnet_addr[0] = (mask >> 24) & 0xFF;
                                ifconfig.subnet_addr[1] = (mask >> 16) & 0xFF;
                                ifconfig.subnet_addr[2] = (mask >> 8) & 0xFF;
                                ifconfig.subnet_addr[3] = mask & 0xFF;
                            }
                            netutils_parse_ipv4_addr(e->value, ifconfig.ip_addr, NETUTILS_BIG);
                        } else if (e->value != mp_const_none) {
                            mp_obj_t *items;
                            mp_obj_get_array_fixed_n(e->value, 2, &items);
                            netutils_parse_ipv4_addr(items[0], ifconfig.ip_addr, NETUTILS_BIG);
                            netutils_parse_ipv4_addr(items[1], ifconfig.subnet_addr, NETUTILS_BIG);
                        }
                        nina_ifconfig(&ifconfig, true);
                        network_ninaw10_dhcp_active = false;
                        break;
                    }
                    case MP_QSTR_gw4: {
                        netutils_parse_ipv4_addr(e->value, ifconfig.gateway_addr, NETUTILS_BIG);
                        nina_ifconfig(&ifconfig, true);
                        network_ninaw10_dhcp_active = false;
                        break;
                    }
                    default: {
                        mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                        break;
                    }
                }
            }
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_ninaw10_nic_ipconfig_obj, 1, network_ninaw10_nic_ipconfig);

static mp_obj_t network_ninaw10_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    nina_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    (void)self;

    if (kwargs->used == 0) {
        // Get config value
        if (n_args != 2) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }

        switch (mp_obj_str_get_qstr(args[1])) {
            case MP_QSTR_ssid: {
                nina_netinfo_t netinfo;
                nina_netinfo(&netinfo);
                return mp_obj_new_str_from_cstr(netinfo.ssid);
            }
            case MP_QSTR_security: {
                nina_netinfo_t netinfo;
                nina_netinfo(&netinfo);
                return mp_obj_new_int(netinfo.security);
            }
            case MP_QSTR_mac:
            case MP_QSTR_bssid: {
                nina_netinfo_t netinfo;
                nina_netinfo(&netinfo);
                return mp_obj_new_bytes(netinfo.bssid, 6);
            }
            case MP_QSTR_fw_version: {
                uint8_t fwver[NINA_FW_VER_LEN];
                nina_fw_version(fwver);
                return mp_obj_new_tuple(3, (mp_obj_t []) {
                    mp_obj_new_int(fwver[NINA_FW_VER_MAJOR_OFFS] - 48),
                    mp_obj_new_int(fwver[NINA_FW_VER_MINOR_OFFS] - 48),
                    mp_obj_new_int(fwver[NINA_FW_VER_PATCH_OFFS] - 48)
                });
            }
            default:
                mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
        }
    } else {
        if (self->itf != MOD_NETWORK_AP_IF) {
            mp_raise_ValueError(MP_ERROR_TEXT("AP required"));
        }
        // Call connect to set WiFi access point.
        return network_ninaw10_connect(n_args, args, kwargs);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_ninaw10_config_obj, 1, network_ninaw10_config);

static mp_obj_t network_ninaw10_status(size_t n_args, const mp_obj_t *args) {
    nina_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    (void)self;

    if (n_args == 1) {
        // no arguments: return link status
        return mp_obj_new_bool(nina_isconnected());
    }

    // Query parameter.
    switch (mp_obj_str_get_qstr(args[1])) {
        case MP_QSTR_rssi: {
            nina_netinfo_t netinfo;
            nina_netinfo(&netinfo);
            return mp_obj_new_int(netinfo.rssi);
        }
        case MP_QSTR_stations: {
            if (self->itf != MOD_NETWORK_AP_IF) {
                mp_raise_ValueError(MP_ERROR_TEXT("AP required"));
            }
            uint32_t sta_ip = 0;
            mp_obj_t sta_list = mp_obj_new_list(0, NULL);
            if (nina_connected_sta(&sta_ip) == 0) {
                mp_obj_list_append(sta_list,
                    netutils_format_inet_addr((uint8_t *)&sta_ip, 0, NETUTILS_BIG));
            }
            return sta_list;
        }
    }

    mp_raise_ValueError(MP_ERROR_TEXT("unknown status param"));
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_ninaw10_status_obj, 1, 2, network_ninaw10_status);

static mp_obj_t network_ninaw10_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t buf_in) {
    debug_printf("ioctl(%d)\n", mp_obj_get_int(cmd_in));
    nina_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t buf;
    mp_get_buffer_raise(buf_in, &buf, MP_BUFFER_READ | MP_BUFFER_WRITE);
    int ret = nina_ioctl(mp_obj_get_int(cmd_in), buf.len, buf.buf, self->itf);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("ioctl %d failed %d"), mp_obj_get_int(cmd_in), ret);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(network_ninaw10_ioctl_obj, network_ninaw10_ioctl);

static int network_ninaw10_gethostbyname(mp_obj_t nic, const char *name, mp_uint_t len, uint8_t *out_ip) {
    debug_printf("gethostbyname(%s)\n", name);
    return nina_gethostbyname(name, out_ip);
}

static int network_ninaw10_socket_poll(mod_network_socket_obj_t *socket, uint32_t rwf, int *_errno) {
    uint8_t flags = 0;
    debug_printf("socket_polling_rw(%d, %d, %d)\n", socket->fileno, socket->timeout, rwf);
    if (socket->timeout == 0) {
        // Non-blocking socket, next socket function will return EAGAIN
        return 0;
    }
    mp_uint_t start = mp_hal_ticks_ms();
    for (; !(flags & rwf); mp_hal_delay_ms(5)) {
        if (nina_socket_poll(socket->fileno, &flags) < 0) {
            nina_socket_errno(_errno);
            debug_printf("socket_poll(%d) -> errno %d flags %d\n", socket->fileno, *_errno, flags);
            return -1;
        }
        if (!(flags & rwf) && socket->timeout != -1 &&
            mp_hal_ticks_ms() - start > socket->timeout) {
            *_errno = MP_ETIMEDOUT;
            return -1;
        }
    }
    return 0;
}

static int network_ninaw10_socket_setblocking(mod_network_socket_obj_t *socket, bool blocking, int *_errno) {
    uint32_t nonblocking = !blocking;
    // set socket in non-blocking mode
    if (nina_socket_ioctl(socket->fileno, SOCKET_IOCTL_FIONBIO, &nonblocking, sizeof(nonblocking)) < 0) {
        nina_socket_errno(_errno);
        nina_socket_close(socket->fileno);
        return -1;
    }
    return 0;
}

static int network_ninaw10_socket_listening(mod_network_socket_obj_t *socket, int *_errno) {
    int listening = 0;
    if (nina_socket_getsockopt(socket->fileno, MOD_NETWORK_SOL_SOCKET,
        SO_ACCEPTCONN, &listening, sizeof(listening)) < 0) {
        nina_socket_errno(_errno);
        debug_printf("socket_getsockopt() -> errno %d\n", *_errno);
        return -1;
    }
    return listening;
}

static int network_ninaw10_socket_socket(mod_network_socket_obj_t *socket, int *_errno) {
    debug_printf("socket_socket(%d %d %d)\n", socket->domain, socket->type, socket->proto);

    uint8_t socket_type;

    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM:
            socket_type = NINA_SOCKET_TYPE_TCP;
            break;

        case MOD_NETWORK_SOCK_DGRAM:
            socket_type = NINA_SOCKET_TYPE_UDP;
            break;

        case MOD_NETWORK_SOCK_RAW:
            socket_type = NINA_SOCKET_TYPE_RAW;
            break;

        default:
            *_errno = MP_EINVAL;
            return -1;
    }

    if (socket->domain != MOD_NETWORK_AF_INET) {
        *_errno = MP_EAFNOSUPPORT;
        return -1;
    }

    // open socket
    int fd = nina_socket_socket(socket_type, socket->proto);
    if (fd < 0) {
        nina_socket_errno(_errno);
        debug_printf("socket_socket() -> errno %d\n", *_errno);
        return -1;
    }

    // set socket state
    socket->fileno = fd;
    socket->bound = false;
    socket->callback = MP_OBJ_NULL;
    return network_ninaw10_socket_setblocking(socket, false, _errno);
}

static void network_ninaw10_socket_close(mod_network_socket_obj_t *socket) {
    debug_printf("socket_close(%d)\n", socket->fileno);
    if (socket->callback != MP_OBJ_NULL) {
        socket->callback = MP_OBJ_NULL;
        network_ninaw10_poll_list_remove(socket);
    }
    if (socket->fileno >= 0) {
        nina_socket_close(socket->fileno);
        socket->fileno = -1; // Mark socket FD as invalid
    }
}

static int network_ninaw10_socket_bind(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    debug_printf("socket_bind(%d, %d)\n", socket->fileno, port);

    int ret = nina_socket_bind(socket->fileno, ip, port);
    if (ret < 0) {
        nina_socket_errno(_errno);
        network_ninaw10_socket_close(socket);
        debug_printf("socket_bind(%d, %d) -> errno: %d\n", socket->fileno, port, *_errno);
        return -1;
    }

    // Mark socket as bound to avoid auto-binding.
    socket->bound = true;
    return 0;
}

static int network_ninaw10_socket_listen(mod_network_socket_obj_t *socket, mp_int_t backlog, int *_errno) {
    debug_printf("socket_listen(%d, %d)\n", socket->fileno, backlog);
    int ret = nina_socket_listen(socket->fileno, backlog);
    if (ret < 0) {
        nina_socket_errno(_errno);
        network_ninaw10_socket_close(socket);
        debug_printf("socket_listen() -> errno %d\n", *_errno);
        return -1;
    }
    return 0;
}

static int network_ninaw10_socket_accept(mod_network_socket_obj_t *socket,
    mod_network_socket_obj_t *socket2, byte *ip, mp_uint_t *port, int *_errno) {
    debug_printf("socket_accept(%d)\n", socket->fileno);

    if (network_ninaw10_socket_poll(socket, SOCKET_POLL_RD, _errno) != 0) {
        return -1;
    }

    *port = 0;
    int fd = 0;
    int ret = nina_socket_accept(socket->fileno, ip, (uint16_t *)port, &fd);
    if (ret < 0) {
        nina_socket_errno(_errno);
        // Close socket if not a nonblocking error.
        if (!is_nonblocking_error(*_errno)) {
            network_ninaw10_socket_close(socket);
        }
        debug_printf("socket_accept() -> errno %d\n", *_errno);
        return -1;
    }

    // set socket state
    socket2->fileno = fd;
    socket2->bound = false;
    socket2->timeout = -1;
    socket2->callback = MP_OBJ_NULL;
    return network_ninaw10_socket_setblocking(socket2, false, _errno);
}

static int network_ninaw10_socket_connect(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    debug_printf("socket_connect(%d)\n", socket->fileno);

    int ret = nina_socket_connect(socket->fileno, ip, port);
    if (ret < 0) {
        nina_socket_errno(_errno);
        debug_printf("socket_connect() -> errno %d\n", *_errno);

        // Close socket if not a nonblocking error.
        if (!is_nonblocking_error(*_errno)) {
            network_ninaw10_socket_close(socket);
            return -1;
        }

        // Poll for write.
        if (socket->timeout == 0 ||
            network_ninaw10_socket_poll(socket, SOCKET_POLL_WR, _errno) != 0) {
            return -1;
        }
    }
    return 0;
}

static mp_uint_t network_ninaw10_socket_send(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, int *_errno) {
    debug_printf("socket_send(%d, %d)\n", socket->fileno, len);

    if (network_ninaw10_socket_poll(socket, SOCKET_POLL_WR, _errno) != 0) {
        return -1;
    }

    int ret = nina_socket_send(socket->fileno, buf, len);
    if (ret < 0) {
        nina_socket_errno(_errno);
        // Close socket if not a nonblocking error.
        if (!is_nonblocking_error(*_errno)) {
            network_ninaw10_socket_close(socket);
        }
        debug_printf("socket_send() -> errno %d\n", *_errno);
        return -1;
    }
    return ret;
}

static mp_uint_t network_ninaw10_socket_recv(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, int *_errno) {
    debug_printf("socket_recv(%d)\n", socket->fileno);
    // check if socket in listening state.
    if (network_ninaw10_socket_listening(socket, _errno) == 1) {
        *_errno = MP_ENOTCONN;
        return -1;
    }

    if (network_ninaw10_socket_poll(socket, SOCKET_POLL_RD, _errno) != 0) {
        return -1;
    }

    int ret = nina_socket_recv(socket->fileno, buf, len);
    if (ret < 0) {
        nina_socket_errno(_errno);
        if (*_errno == MP_ENOTCONN) {
            *_errno = 0;
            return 0;
        }
        // Close socket if not a nonblocking error.
        if (!is_nonblocking_error(*_errno)) {
            network_ninaw10_socket_close(socket);
        }
        debug_printf("socket_recv() -> errno %d\n", *_errno);
        return -1;
    }
    return ret;
}

static mp_uint_t network_ninaw10_socket_auto_bind(mod_network_socket_obj_t *socket, int *_errno) {
    debug_printf("socket_autobind(%d)\n", socket->fileno);
    if (socket->bound == false && socket->type != MOD_NETWORK_SOCK_RAW) {
        if (network_ninaw10_socket_bind(socket, NULL, bind_port, _errno) != 0) {
            nina_socket_errno(_errno);
            debug_printf("socket_bind() -> errno %d\n", *_errno);
            return -1;
        }
        bind_port++;
        bind_port = MIN(MAX(bind_port, BIND_PORT_RANGE_MIN), BIND_PORT_RANGE_MAX);
    }
    return 0;
}

static mp_uint_t network_ninaw10_socket_sendto(mod_network_socket_obj_t *socket,
    const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno) {
    debug_printf("socket_sendto(%d)\n", socket->fileno);
    // Auto-bind the socket first if the socket is unbound.
    if (network_ninaw10_socket_auto_bind(socket, _errno) != 0) {
        return -1;
    }

    if (network_ninaw10_socket_poll(socket, SOCKET_POLL_WR, _errno) != 0) {
        return -1;
    }

    int ret = nina_socket_sendto(socket->fileno, buf, len, ip, port);
    if (ret < 0) {
        nina_socket_errno(_errno);
        // Close socket if not a nonblocking error.
        if (!is_nonblocking_error(*_errno)) {
            network_ninaw10_socket_close(socket);
        }
        return -1;
    }
    return ret;
}

static mp_uint_t network_ninaw10_socket_recvfrom(mod_network_socket_obj_t *socket,
    byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno) {
    debug_printf("socket_recvfrom(%d)\n", socket->fileno);
    // Auto-bind the socket first if the socket is unbound.
    if (network_ninaw10_socket_auto_bind(socket, _errno) != 0) {
        return -1;
    }

    if (network_ninaw10_socket_poll(socket, SOCKET_POLL_RD, _errno) != 0) {
        return -1;
    }

    *port = 0;
    int ret = nina_socket_recvfrom(socket->fileno, buf, len, ip, (uint16_t *)port);
    if (ret < 0) {
        nina_socket_errno(_errno);
        // Close socket if not a nonblocking error.
        if (!is_nonblocking_error(*_errno)) {
            network_ninaw10_socket_close(socket);
        }
        debug_printf("socket_recvfrom() -> errno %d\n", *_errno);
        return -1;
    }
    return ret;
}

static int network_ninaw10_socket_setsockopt(mod_network_socket_obj_t *socket, mp_uint_t
    level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno) {
    debug_printf("socket_setsockopt(%d, %d)\n", socket->fileno, opt);
    if (opt == 20) {
        mp_sched_lock();
        socket->callback = (void *)optval;
        if (socket->callback != MP_OBJ_NULL) {
            network_ninaw10_poll_list_insert(socket);
            soft_timer_reinsert(&mp_wifi_poll_timer, NINAW10_POLL_INTERVAL);
        }
        mp_sched_unlock();
        return 0;
    }
    int ret = nina_socket_setsockopt(socket->fileno, level, opt, optval, optlen);
    if (ret < 0) {
        nina_socket_errno(_errno);
        network_ninaw10_socket_close(socket);
        debug_printf("socket_setsockopt() -> errno %d\n", *_errno);
        return -1;
    }
    return 0;
}

static int network_ninaw10_socket_settimeout(mod_network_socket_obj_t *socket, mp_uint_t timeout_ms, int *_errno) {
    debug_printf("socket_settimeout(%d, %d)\n", socket->fileno, timeout_ms);
    #if 0
    if (timeout_ms == 0 || timeout_ms == UINT32_MAX) {
        // blocking/nonblocking mode
        uint32_t nonblocking = (timeout_ms == 0);
        ret |= nina_socket_ioctl(socket->fileno, SOCKET_IOCTL_FIONBIO, &nonblocking, sizeof(nonblocking));
    } else {
        // timeout provided
        uint32_t tv[2] = {
            (timeout_ms / 1000),
            (timeout_ms % 1000) * 1000,
        };
        ret |= nina_socket_setsockopt(socket->fileno, MOD_NETWORK_SOL_SOCKET, MOD_NETWORK_SO_SNDTIMEO, tv, sizeof(tv));
        ret |= nina_socket_setsockopt(socket->fileno, MOD_NETWORK_SOL_SOCKET, MOD_NETWORK_SO_RCVTIMEO, tv, sizeof(tv));
    }
    if (ret < 0) {
        nina_socket_errno(_errno);
        debug_printf("socket_settimeout() -> errno %d\n", *_errno);
    }
    #endif
    socket->timeout = timeout_ms;
    return 0;
}

static int network_ninaw10_socket_ioctl(mod_network_socket_obj_t *socket, mp_uint_t request, mp_uint_t arg, int *_errno) {
    mp_uint_t ret = 0;
    debug_printf("socket_ioctl(%d, %d)\n", socket->fileno, request);
    if (request == MP_STREAM_POLL) {
        uint8_t flags = 0;
        if (nina_socket_poll(socket->fileno, &flags) < 0) {
            nina_socket_errno(_errno);
            ret = MP_STREAM_ERROR;
            debug_printf("socket_ioctl() -> errno %d\n", *_errno);
        }
        if ((arg & MP_STREAM_POLL_RD) && (flags & SOCKET_POLL_RD)) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((arg & MP_STREAM_POLL_WR) && (flags & SOCKET_POLL_WR)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        // NOTE: FIONREAD and FIONBIO are supported as well.
        *_errno = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

static void network_ninaw10_deinit(void) {
    // On soft-reboot, gc_sweep_all is called and all open sockets are closed
    // and collected. Make sure that the driver is not keeping any references
    // to collected sockets in the poll list.
    soft_timer_remove(&mp_wifi_poll_timer);
    MP_STATE_PORT(mp_wifi_poll_list) = NULL;
}

static const mp_rom_map_elem_t nina_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active),              MP_ROM_PTR(&network_ninaw10_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan),                MP_ROM_PTR(&network_ninaw10_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect),             MP_ROM_PTR(&network_ninaw10_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect),          MP_ROM_PTR(&network_ninaw10_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected),         MP_ROM_PTR(&network_ninaw10_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig),            MP_ROM_PTR(&network_ninaw10_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipconfig),            MP_ROM_PTR(&network_ninaw10_nic_ipconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_config),              MP_ROM_PTR(&network_ninaw10_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_status),              MP_ROM_PTR(&network_ninaw10_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl),               MP_ROM_PTR(&network_ninaw10_ioctl_obj) },

    // Class constants.
    { MP_ROM_QSTR(MP_QSTR_IF_STA),              MP_ROM_INT(MOD_NETWORK_STA_IF) },
    { MP_ROM_QSTR(MP_QSTR_IF_AP),               MP_ROM_INT(MOD_NETWORK_AP_IF) },

    { MP_ROM_QSTR(MP_QSTR_SEC_OPEN),            MP_ROM_INT(NINA_SEC_OPEN) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WEP),             MP_ROM_INT(NINA_SEC_WEP) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA_WPA2),        MP_ROM_INT(NINA_SEC_WPA_PSK) },

    // For backwards compatibility.
    { MP_ROM_QSTR(MP_QSTR_OPEN),                MP_ROM_INT(NINA_SEC_OPEN) },
    { MP_ROM_QSTR(MP_QSTR_WEP),                 MP_ROM_INT(NINA_SEC_WEP) },
    { MP_ROM_QSTR(MP_QSTR_WPA_PSK),             MP_ROM_INT(NINA_SEC_WPA_PSK) },
};

static MP_DEFINE_CONST_DICT(nina_locals_dict, nina_locals_dict_table);

static const mod_network_nic_protocol_t mod_network_nic_protocol_nina = {
    .gethostbyname = network_ninaw10_gethostbyname,
    .deinit = network_ninaw10_deinit,
    .socket = network_ninaw10_socket_socket,
    .close = network_ninaw10_socket_close,
    .bind = network_ninaw10_socket_bind,
    .listen = network_ninaw10_socket_listen,
    .accept = network_ninaw10_socket_accept,
    .connect = network_ninaw10_socket_connect,
    .send = network_ninaw10_socket_send,
    .recv = network_ninaw10_socket_recv,
    .sendto = network_ninaw10_socket_sendto,
    .recvfrom = network_ninaw10_socket_recvfrom,
    .setsockopt = network_ninaw10_socket_setsockopt,
    .settimeout = network_ninaw10_socket_settimeout,
    .ioctl = network_ninaw10_socket_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mod_network_nic_type_nina,
    MP_QSTR_nina,
    MP_TYPE_FLAG_NONE,
    make_new, network_ninaw10_make_new,
    locals_dict, &nina_locals_dict,
    protocol, &mod_network_nic_protocol_nina
    );

MP_REGISTER_ROOT_POINTER(struct _machine_spi_obj_t *mp_wifi_spi);
MP_REGISTER_ROOT_POINTER(struct _mp_obj_list_t *mp_wifi_poll_list);

#endif // #if MICROPY_PY_BLUETOOTH && MICROPY_PY_NETWORK_NINAW10
