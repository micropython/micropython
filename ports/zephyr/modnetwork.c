/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Contributors
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
#include "py/mphal.h"

#if MICROPY_PY_ZEPHYR_NETWORK

#include "shared/netutils/netutils.h"

#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/net_event.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/dhcpv4.h>
#include <zephyr/net/dns_resolve.h>

#ifdef CONFIG_WIFI
#include <zephyr/net/wifi_mgmt.h>
#endif

#ifdef CONFIG_NET_L2_ETHERNET
#include <zephyr/net/ethernet.h>
#endif

// Network interface types
#define NET_IF_TYPE_WLAN  0
#define NET_IF_TYPE_LAN   1

// Connection status
#define NET_STATUS_IDLE          0
#define NET_STATUS_CONNECTING    1
#define NET_STATUS_CONNECTED     2
#define NET_STATUS_DISCONNECTED  3
#define NET_STATUS_GOT_IP        4

typedef struct _network_if_obj_t {
    mp_obj_base_t base;
    struct net_if *iface;
    uint8_t if_type;
    bool active;
    uint8_t status;
} network_if_obj_t;

static network_if_obj_t network_wlan_obj = {
    .base = {NULL},  // Will be set during type initialization
    .iface = NULL,
    .if_type = NET_IF_TYPE_WLAN,
    .active = false,
    .status = NET_STATUS_IDLE
};

static network_if_obj_t network_lan_obj = {
    .base = {NULL},  // Will be set during type initialization
    .iface = NULL,
    .if_type = NET_IF_TYPE_LAN,
    .active = false,
    .status = NET_STATUS_IDLE
};

// Forward declarations
static const mp_obj_type_t network_wlan_type;
static const mp_obj_type_t network_lan_type;

// Event handler for network events
static struct net_mgmt_event_callback net_mgmt_cb;

static void network_event_handler(struct net_mgmt_event_callback *cb,
    uint64_t mgmt_event, struct net_if *iface) {
    #ifdef CONFIG_WIFI
    if (mgmt_event == NET_EVENT_WIFI_CONNECT_RESULT) {
        network_wlan_obj.status = NET_STATUS_CONNECTED;
    } else if (mgmt_event == NET_EVENT_WIFI_DISCONNECT_RESULT) {
        network_wlan_obj.status = NET_STATUS_DISCONNECTED;
    }
    #endif

    if (mgmt_event == NET_EVENT_IPV4_ADDR_ADD) {
        if (network_wlan_obj.iface == iface) {
            network_wlan_obj.status = NET_STATUS_GOT_IP;
        } else if (network_lan_obj.iface == iface) {
            network_lan_obj.status = NET_STATUS_GOT_IP;
        }
    }
}

void mod_network_init(void) {
    // Initialize event callback for network events
    net_mgmt_init_event_callback(&net_mgmt_cb, network_event_handler,
        NET_EVENT_IPV4_ADDR_ADD | NET_EVENT_IPV4_ADDR_DEL
        #ifdef CONFIG_WIFI
        | NET_EVENT_WIFI_CONNECT_RESULT | NET_EVENT_WIFI_DISCONNECT_RESULT
        #endif
        );
    net_mgmt_add_event_callback(&net_mgmt_cb);
}

// Helper to find network interface by type
static struct net_if *find_iface_by_type(int if_type) {
    struct net_if *iface = NULL;

    #ifdef CONFIG_WIFI
    if (if_type == NET_IF_TYPE_WLAN) {
        // Find WiFi interface
        iface = net_if_get_first_wifi();
    }
    #endif

    #ifdef CONFIG_NET_L2_ETHERNET
    if (if_type == NET_IF_TYPE_LAN) {
        // Find Ethernet interface
        for (int i = 1; iface == NULL && i <= 10; i++) {
            struct net_if *test_iface = net_if_get_by_index(i);
            if (test_iface && net_if_l2(test_iface) == &NET_L2_GET_NAME(ETHERNET)) {
                iface = test_iface;
                break;
            }
        }
    }
    #endif

    return iface;
}

// WLAN class implementation
#ifdef CONFIG_WIFI

static mp_obj_t network_wlan_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    network_if_obj_t *self = &network_wlan_obj;
    self->base.type = &network_wlan_type;

    // Find WiFi interface if not already set
    if (self->iface == NULL) {
        self->iface = find_iface_by_type(NET_IF_TYPE_WLAN);
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t network_wlan_active(size_t n_args, const mp_obj_t *args) {
    network_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // Get active status
        return mp_obj_new_bool(self->active && self->iface != NULL);
    } else {
        // Set active status
        bool active = mp_obj_is_true(args[1]);

        if (active && !self->active) {
            // Activate interface
            if (self->iface == NULL) {
                self->iface = find_iface_by_type(NET_IF_TYPE_WLAN);
            }
            if (self->iface == NULL) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("WiFi interface not found"));
            }
            net_if_up(self->iface);
            self->active = true;
        } else if (!active && self->active) {
            // Deactivate interface
            if (self->iface) {
                net_if_down(self->iface);
            }
            self->active = false;
            self->status = NET_STATUS_IDLE;
        }

        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_wlan_active_obj, 1, 2, network_wlan_active);

static mp_obj_t network_wlan_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_password };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_password, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    network_if_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (!self->active || self->iface == NULL) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("WiFi not active"));
    }

    const char *ssid = mp_obj_str_get_str(args[ARG_ssid].u_obj);
    const char *password = args[ARG_password].u_obj != mp_const_none ?
        mp_obj_str_get_str(args[ARG_password].u_obj) : NULL;

    struct wifi_connect_req_params params = {0};
    params.ssid = (uint8_t *)ssid;
    params.ssid_length = strlen(ssid);
    params.channel = WIFI_CHANNEL_ANY;
    params.security = password ? WIFI_SECURITY_TYPE_PSK : WIFI_SECURITY_TYPE_NONE;

    if (password) {
        params.psk = (uint8_t *)password;
        params.psk_length = strlen(password);
    }

    self->status = NET_STATUS_CONNECTING;

    int ret = net_mgmt(NET_REQUEST_WIFI_CONNECT, self->iface, &params, sizeof(params));
    if (ret < 0) {
        self->status = NET_STATUS_IDLE;
        mp_raise_OSError(-ret);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_wlan_connect_obj, 1, network_wlan_connect);

static mp_obj_t network_wlan_disconnect(mp_obj_t self_in) {
    network_if_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->iface) {
        int ret = net_mgmt(NET_REQUEST_WIFI_DISCONNECT, self->iface, NULL, 0);
        if (ret < 0) {
            mp_raise_OSError(-ret);
        }
        self->status = NET_STATUS_DISCONNECTED;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_disconnect_obj, network_wlan_disconnect);

static mp_obj_t network_wlan_isconnected(mp_obj_t self_in) {
    network_if_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(self->status == NET_STATUS_CONNECTED || self->status == NET_STATUS_GOT_IP);
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_isconnected_obj, network_wlan_isconnected);

static mp_obj_t network_wlan_status(size_t n_args, const mp_obj_t *args) {
    network_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // Return connection status
        return MP_OBJ_NEW_SMALL_INT(self->status);
    }

    // Query specific parameter - to be implemented
    mp_raise_ValueError(MP_ERROR_TEXT("unknown status param"));
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_wlan_status_obj, 1, 2, network_wlan_status);

static mp_obj_t network_wlan_ifconfig(size_t n_args, const mp_obj_t *args) {
    network_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (!self->iface) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("interface not available"));
    }

    if (n_args == 1) {
        // Get IP configuration
        struct net_if_config *cfg = net_if_get_config(self->iface);
        if (!cfg || !cfg->ip.ipv4) {
            return mp_const_none;
        }

        // Get IP address
        struct in_addr *addr = &cfg->ip.ipv4->unicast[0].ipv4.address.in_addr;

        // Get netmask
        struct in_addr netmask = net_if_ipv4_get_netmask_by_addr(self->iface, addr);

        // Get gateway
        struct in_addr gw = net_if_ipv4_get_gw(self->iface);

        // Get DNS server
        // Note: If you see a static DNS (like 192.0.2.2) instead of your DHCP DNS,
        // comment out CONFIG_DNS_SERVER1 in prj.conf or board.conf to use DHCP-provided DNS
        const struct dns_resolve_context *dns_ctx = dns_resolve_get_default();
        struct in_addr dns = {0};
        bool found_dhcp_dns = false;

        if (dns_ctx) {
            // First pass: look for DHCP-provided DNS servers only
            for (int i = 0; i < DNS_RESOLVER_MAX_SERVERS; i++) {
                if (dns_ctx->servers[i].dns_server.sa_family == AF_INET &&
                    dns_ctx->servers[i].source == DNS_SOURCE_DHCPV4) {
                    struct sockaddr_in *dns_addr = (struct sockaddr_in *)&dns_ctx->servers[i].dns_server;
                    dns = dns_addr->sin_addr;
                    found_dhcp_dns = true;
                    break;
                }
            }

            // Second pass: if no DHCP DNS, fall back to any configured DNS
            if (!found_dhcp_dns) {
                for (int i = 0; i < DNS_RESOLVER_MAX_SERVERS; i++) {
                    if (dns_ctx->servers[i].dns_server.sa_family == AF_INET) {
                        struct sockaddr_in *dns_addr = (struct sockaddr_in *)&dns_ctx->servers[i].dns_server;
                        dns = dns_addr->sin_addr;
                        break;
                    }
                }
            }
        }

        mp_obj_t tuple[4] = {
            netutils_format_ipv4_addr((uint8_t *)addr, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&netmask, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&gw, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&dns, NETUTILS_BIG),
        };

        return mp_obj_new_tuple(4, tuple);
    } else {
        // Set IP configuration - to be implemented
        mp_raise_NotImplementedError(MP_ERROR_TEXT("ifconfig set not implemented"));
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_wlan_ifconfig_obj, 1, 2, network_wlan_ifconfig);

static const mp_rom_map_elem_t network_wlan_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&network_wlan_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&network_wlan_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&network_wlan_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&network_wlan_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&network_wlan_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&network_wlan_ifconfig_obj) },
};
static MP_DEFINE_CONST_DICT(network_wlan_locals_dict, network_wlan_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    network_wlan_type,
    MP_QSTR_WLAN,
    MP_TYPE_FLAG_NONE,
    make_new, network_wlan_make_new,
    locals_dict, &network_wlan_locals_dict
    );

#endif // CONFIG_WIFI

// LAN class implementation
#ifdef CONFIG_NET_L2_ETHERNET

static mp_obj_t network_lan_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    network_if_obj_t *self = &network_lan_obj;
    self->base.type = &network_lan_type;

    // Find Ethernet interface if not already set
    if (self->iface == NULL) {
        self->iface = find_iface_by_type(NET_IF_TYPE_LAN);
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t network_lan_active(size_t n_args, const mp_obj_t *args) {
    network_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // Get active status
        return mp_obj_new_bool(self->active && self->iface != NULL);
    } else {
        // Set active status
        bool active = mp_obj_is_true(args[1]);

        if (active && !self->active) {
            // Activate interface
            if (self->iface == NULL) {
                self->iface = find_iface_by_type(NET_IF_TYPE_LAN);
            }
            if (self->iface == NULL) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Ethernet interface not found"));
            }
            net_if_up(self->iface);
            self->active = true;
        } else if (!active && self->active) {
            // Deactivate interface
            if (self->iface) {
                net_if_down(self->iface);
            }
            self->active = false;
            self->status = NET_STATUS_IDLE;
        }

        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_lan_active_obj, 1, 2, network_lan_active);

static mp_obj_t network_lan_isconnected(mp_obj_t self_in) {
    network_if_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (!self->iface) {
        return mp_const_false;
    }

    // Check if interface has link
    return mp_obj_new_bool(net_if_is_up(self->iface));
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_lan_isconnected_obj, network_lan_isconnected);

static mp_obj_t network_lan_ifconfig(size_t n_args, const mp_obj_t *args) {
    network_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (!self->iface) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("interface not available"));
    }

    if (n_args == 1) {
        // Get IP configuration
        struct net_if_config *cfg = net_if_get_config(self->iface);
        if (!cfg || !cfg->ip.ipv4) {
            return mp_const_none;
        }

        // Get IP address
        struct in_addr *addr = &cfg->ip.ipv4->unicast[0].ipv4.address.in_addr;

        // Get netmask
        struct in_addr netmask = net_if_ipv4_get_netmask_by_addr(self->iface, addr);

        // Get gateway
        struct in_addr gw = net_if_ipv4_get_gw(self->iface);

        // Get DNS server
        // Note: If you see a static DNS (like 192.0.2.2) instead of your DHCP DNS,
        // comment out CONFIG_DNS_SERVER1 in prj.conf or board.conf to use DHCP-provided DNS
        const struct dns_resolve_context *dns_ctx = dns_resolve_get_default();
        struct in_addr dns = {0};
        bool found_dhcp_dns = false;

        if (dns_ctx) {
            // First pass: look for DHCP-provided DNS servers only
            for (int i = 0; i < DNS_RESOLVER_MAX_SERVERS; i++) {
                if (dns_ctx->servers[i].dns_server.sa_family == AF_INET &&
                    dns_ctx->servers[i].source == DNS_SOURCE_DHCPV4) {
                    struct sockaddr_in *dns_addr = (struct sockaddr_in *)&dns_ctx->servers[i].dns_server;
                    dns = dns_addr->sin_addr;
                    found_dhcp_dns = true;
                    break;
                }
            }

            // Second pass: if no DHCP DNS, fall back to any configured DNS
            if (!found_dhcp_dns) {
                for (int i = 0; i < DNS_RESOLVER_MAX_SERVERS; i++) {
                    if (dns_ctx->servers[i].dns_server.sa_family == AF_INET) {
                        struct sockaddr_in *dns_addr = (struct sockaddr_in *)&dns_ctx->servers[i].dns_server;
                        dns = dns_addr->sin_addr;
                        break;
                    }
                }
            }
        }

        mp_obj_t tuple[4] = {
            netutils_format_ipv4_addr((uint8_t *)addr, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&netmask, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&gw, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&dns, NETUTILS_BIG),
        };

        return mp_obj_new_tuple(4, tuple);
    } else {
        // Set IP configuration - to be implemented
        mp_raise_NotImplementedError(MP_ERROR_TEXT("ifconfig set not implemented"));
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_lan_ifconfig_obj, 1, 2, network_lan_ifconfig);

static const mp_rom_map_elem_t network_lan_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&network_lan_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&network_lan_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&network_lan_ifconfig_obj) },
};
static MP_DEFINE_CONST_DICT(network_lan_locals_dict, network_lan_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    network_lan_type,
    MP_QSTR_LAN,
    MP_TYPE_FLAG_NONE,
    make_new, network_lan_make_new,
    locals_dict, &network_lan_locals_dict
    );

#endif // CONFIG_NET_L2_ETHERNET

// Module globals
static const mp_rom_map_elem_t mp_module_network_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_network) },

    #ifdef CONFIG_WIFI
    { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&network_wlan_type) },
    #endif

    #ifdef CONFIG_NET_L2_ETHERNET
    { MP_ROM_QSTR(MP_QSTR_LAN), MP_ROM_PTR(&network_lan_type) },
    #endif

    // Interface type constants
    { MP_ROM_QSTR(MP_QSTR_STA_IF), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_AP_IF), MP_ROM_INT(1) },

    // Status constants
    { MP_ROM_QSTR(MP_QSTR_STAT_IDLE), MP_ROM_INT(NET_STATUS_IDLE) },
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECTING), MP_ROM_INT(NET_STATUS_CONNECTING) },
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECTED), MP_ROM_INT(NET_STATUS_CONNECTED) },
    { MP_ROM_QSTR(MP_QSTR_STAT_DISCONNECTED), MP_ROM_INT(NET_STATUS_DISCONNECTED) },
    { MP_ROM_QSTR(MP_QSTR_STAT_GOT_IP), MP_ROM_INT(NET_STATUS_GOT_IP) },
};
static MP_DEFINE_CONST_DICT(mp_module_network_globals, mp_module_network_globals_table);

const mp_obj_module_t mp_module_network = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_network_globals,
};

MP_REGISTER_MODULE(MP_QSTR_network, mp_module_network);

#endif // MICROPY_PY_ZEPHYR_NETWORK
