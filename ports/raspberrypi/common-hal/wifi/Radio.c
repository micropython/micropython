// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/port.h"
#include "shared-bindings/wifi/Radio.h"
#include "shared-bindings/wifi/Network.h"

#include <math.h>
#include <string.h>

#include "common-hal/wifi/__init__.h"
#include "shared/runtime/interrupt_char.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "bindings/cyw43/__init__.h"
#include "shared-bindings/ipaddress/IPv4Address.h"
#include "shared-bindings/wifi/ScannedNetworks.h"
#include "shared-bindings/wifi/AuthMode.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/ipaddress/__init__.h"
#include "common-hal/socketpool/__init__.h"

#include "lwip/sys.h"
#include "lwip/dns.h"
#include "lwip/icmp.h"
#include "lwip/raw.h"
#include "lwip_src/ping.h"

#include "shared/netutils/dhcpserver.h"

#define MAC_ADDRESS_LENGTH 6

#define NETIF_STA (&cyw43_state.netif[CYW43_ITF_STA])
#define NETIF_AP (&cyw43_state.netif[CYW43_ITF_AP])

static inline uint32_t nw_get_le32(const uint8_t *buf) {
    return buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24;
}

static inline void nw_put_le32(uint8_t *buf, uint32_t x) {
    buf[0] = x;
    buf[1] = x >> 8;
    buf[2] = x >> 16;
    buf[3] = x >> 24;
}

NORETURN static void ro_attribute(qstr attr) {
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q is read-only for this board"), attr);
}

bool common_hal_wifi_radio_get_enabled(wifi_radio_obj_t *self) {
    return self->enabled;
}

void common_hal_wifi_radio_set_enabled(wifi_radio_obj_t *self, bool enabled) {
    // TODO: Actually enable and disable the WiFi module at this point.
    if (self->enabled && !enabled) {
        common_hal_wifi_radio_stop_station(self);
        common_hal_wifi_radio_stop_ap(self);
    }
    self->enabled = enabled;

}

mp_obj_t common_hal_wifi_radio_get_hostname(wifi_radio_obj_t *self) {
    if (!NETIF_STA->hostname) {
        return mp_const_none;
    }
    return mp_obj_new_str(NETIF_STA->hostname, strlen(NETIF_STA->hostname));
}

void common_hal_wifi_radio_set_hostname(wifi_radio_obj_t *self, const char *hostname) {
    assert(strlen(hostname) < MP_ARRAY_SIZE(self->hostname));
    strncpy(self->hostname, hostname, MP_ARRAY_SIZE(self->hostname) - 1);
    netif_set_hostname(NETIF_STA, self->hostname);
    netif_set_hostname(NETIF_AP, self->hostname);
}

void wifi_radio_get_mac_address(wifi_radio_obj_t *self, uint8_t *mac) {
    memcpy(mac, cyw43_state.mac, MAC_ADDRESS_LENGTH);
}

mp_obj_t common_hal_wifi_radio_get_mac_address(wifi_radio_obj_t *self) {
    return mp_obj_new_bytes(cyw43_state.mac, MAC_ADDRESS_LENGTH);
}

void common_hal_wifi_radio_set_mac_address(wifi_radio_obj_t *self, const uint8_t *mac) {
    ro_attribute(MP_QSTR_mac_address);
}

mp_float_t common_hal_wifi_radio_get_tx_power(wifi_radio_obj_t *self) {
    uint8_t buf[13];
    memcpy(buf, "qtxpower\x00\x00\x00\x00\x00", 13);
    cyw43_ioctl(&cyw43_state, CYW43_IOCTL_GET_VAR, 13, buf, CYW43_ITF_STA);
    return nw_get_le32(buf) * MICROPY_FLOAT_CONST(0.25);
}

void common_hal_wifi_radio_set_tx_power(wifi_radio_obj_t *self, const mp_float_t tx_power) {
    mp_int_t dbm_times_four = (int)(4 * tx_power);
    uint8_t buf[9 + 4];
    memcpy(buf, "qtxpower\x00", 9);
    nw_put_le32(buf + 9, dbm_times_four);
    cyw43_ioctl(&cyw43_state, CYW43_IOCTL_SET_VAR, 9 + 4, buf, CYW43_ITF_STA);
    cyw43_ioctl(&cyw43_state, CYW43_IOCTL_SET_VAR, 9 + 4, buf, CYW43_ITF_AP);
}

mp_obj_t common_hal_wifi_radio_get_mac_address_ap(wifi_radio_obj_t *self) {
    return common_hal_wifi_radio_get_mac_address(self);
}

void common_hal_wifi_radio_set_mac_address_ap(wifi_radio_obj_t *self, const uint8_t *mac) {
    ro_attribute(MP_QSTR_mac_address_ap);
}

mp_obj_t common_hal_wifi_radio_start_scanning_networks(wifi_radio_obj_t *self, uint8_t start_channel, uint8_t stop_channel) {
    // channel bounds are ignored; not implemented in driver
    if (self->current_scan) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Already scanning for wifi networks"));
    }
    if (!common_hal_wifi_radio_get_enabled(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Wifi is not enabled"));
    }
    wifi_scannednetworks_obj_t *scan = mp_obj_malloc(wifi_scannednetworks_obj_t, &wifi_scannednetworks_type);
    mp_obj_t args[] = { mp_const_empty_tuple, MP_OBJ_NEW_SMALL_INT(16) };
    scan->results = MP_OBJ_TYPE_GET_SLOT(&mp_type_deque, make_new)(&mp_type_deque, 2, 0, args);
    self->current_scan = scan;
    wifi_scannednetworks_start_scan(scan);
    return scan;
}

void common_hal_wifi_radio_stop_scanning_networks(wifi_radio_obj_t *self) {
    self->current_scan = NULL;
}

void common_hal_wifi_radio_start_station(wifi_radio_obj_t *self) {
    cyw43_arch_enable_sta_mode();
    bindings_cyw43_wifi_enforce_pm();
}

void common_hal_wifi_radio_stop_station(wifi_radio_obj_t *self) {

    cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA);
    const size_t timeout_ms = 500;
    uint64_t start = port_get_raw_ticks(NULL);
    uint64_t deadline = start + timeout_ms;
    while (port_get_raw_ticks(NULL) < deadline && (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_DOWN)) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            break;
        }
    }
    bindings_cyw43_wifi_enforce_pm();
}

void common_hal_wifi_radio_start_ap(wifi_radio_obj_t *self, uint8_t *ssid, size_t ssid_len, uint8_t *password, size_t password_len, uint8_t channel, uint32_t authmode, uint8_t max_connections) {
    if (!common_hal_wifi_radio_get_enabled(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Wifi is not enabled"));
    }

    /* TODO: If the AP is stopped once it cannot be restarted.
     * This means that if if the user does:
     *
     * wifi.radio.start_ap(...)
     * wifi.radio.stop_ap()
     * wifi.radio.start_ap(...)
     *
     * The second start_ap will fail.
     */

    common_hal_wifi_radio_stop_ap(self);

    // Channel can only be changed after initial powerup and config of ap.
    // Defaults to 1 if not set or invalid (i.e. 13)
    cyw43_wifi_ap_set_channel(&cyw43_state, (const uint32_t)channel);

    if (password_len) {
        cyw43_arch_enable_ap_mode((const char *)ssid, (const char *)password, CYW43_AUTH_WPA2_AES_PSK);
    } else {
        cyw43_arch_enable_ap_mode((const char *)ssid, NULL, CYW43_AUTH_OPEN);
    }

    // TODO: Implement authmode check like in espressif
    bindings_cyw43_wifi_enforce_pm();

    const size_t timeout_ms = 500;
    uint64_t start = port_get_raw_ticks(NULL);
    uint64_t deadline = start + timeout_ms;
    while (port_get_raw_ticks(NULL) < deadline && (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_AP) != CYW43_LINK_UP)) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            break;
        }
    }
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_AP) != CYW43_LINK_UP) {
        common_hal_wifi_radio_stop_ap(self);
        // This is needed since it leaves a broken AP up.
        mp_raise_RuntimeError(MP_ERROR_TEXT("AP could not be started"));
    }
}

bool common_hal_wifi_radio_get_ap_active(wifi_radio_obj_t *self) {
    return cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_AP) == CYW43_LINK_UP;
}

void common_hal_wifi_radio_stop_ap(wifi_radio_obj_t *self) {
    if (!common_hal_wifi_radio_get_enabled(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("wifi is not enabled"));
    }

    cyw43_arch_disable_ap_mode();

    const size_t timeout_ms = 500;
    uint64_t start = port_get_raw_ticks(NULL);
    uint64_t deadline = start + timeout_ms;
    while (port_get_raw_ticks(NULL) < deadline && (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_AP) != CYW43_LINK_DOWN)) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            break;
        }
    }

    bindings_cyw43_wifi_enforce_pm();
}

// There's no published API for the DHCP server to retrieve lease information
// This code depends on undocumented internal structures and is likely to break in the future
static uint32_t cyw43_dhcps_get_ip_addr(dhcp_server_t *dhcp_server, uint8_t *mac_address) {
    for (int i = 0; i < DHCPS_MAX_IP; i++) {
        if (memcmp(dhcp_server->lease[i].mac, mac_address, MAC_ADDRESS_LENGTH) == 0) {
            return (dhcp_server->ip.addr & 0x00FFFFFF) + ((DHCPS_BASE_IP + i) << 24);
        }
    }

    return 0;
}

mp_obj_t common_hal_wifi_radio_get_stations_ap(wifi_radio_obj_t *self) {
    int max_stas;
    int num_stas;

    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_AP) != CYW43_LINK_UP) {
        return mp_const_none;
    }

    cyw43_wifi_ap_get_max_stas(&cyw43_state, &max_stas);

    uint8_t macs[max_stas * MAC_ADDRESS_LENGTH];

    cyw43_wifi_ap_get_stas(&cyw43_state, &num_stas, macs);

    mp_obj_t mp_sta_list = mp_obj_new_list(0, NULL);
    for (int i = 0; i < num_stas; i++) {
        mp_obj_t elems[3] = {
            mp_obj_new_bytes(&macs[i * MAC_ADDRESS_LENGTH], MAC_ADDRESS_LENGTH),
            mp_const_none,
            mp_const_none
        };

        uint32_t ipv4_addr = cyw43_dhcps_get_ip_addr(&cyw43_state.dhcp_server, &macs[i * MAC_ADDRESS_LENGTH]);
        if (ipv4_addr) {
            elems[2] = common_hal_ipaddress_new_ipv4address(ipv4_addr);
        }

        mp_obj_list_append(mp_sta_list, namedtuple_make_new((const mp_obj_type_t *)&wifi_radio_station_type, 3, 0, elems));
    }

    return mp_sta_list;
}

static bool connection_unchanged(wifi_radio_obj_t *self, const uint8_t *ssid, size_t ssid_len) {
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP) {
        return false;
    }
    if (ssid_len != self->connected_ssid_len) {
        return false;
    }
    if (memcmp(ssid, self->connected_ssid, self->connected_ssid_len)) {
        return false;
    }
    return true;
}

wifi_radio_error_t common_hal_wifi_radio_connect(wifi_radio_obj_t *self, uint8_t *ssid, size_t ssid_len, uint8_t *password, size_t password_len, uint8_t channel, mp_float_t timeout, uint8_t *bssid, size_t bssid_len) {
    if (!common_hal_wifi_radio_get_enabled(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Wifi is not enabled"));
    }

    if (ssid_len > 32) {
        return WIFI_RADIO_ERROR_CONNECTION_FAIL;
    }

    size_t timeout_ms = timeout <= 0 ? 8000 : (size_t)MICROPY_FLOAT_C_FUN(ceil)(timeout * 1000);
    uint64_t start = port_get_raw_ticks(NULL);
    uint64_t deadline = start + timeout_ms;

    if (connection_unchanged(self, ssid, ssid_len)) {
        return WIFI_RADIO_ERROR_NONE;
    }

    // disconnect
    common_hal_wifi_radio_stop_station(self);

    // connect
    int auth_mode = password_len ? CYW43_AUTH_WPA2_AES_PSK : CYW43_AUTH_OPEN;
    cyw43_arch_wifi_connect_async((const char *)ssid, (const char *)password, auth_mode);
    // TODO: Implement authmode check like in espressif

    while (port_get_raw_ticks(NULL) < deadline) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            break;
        }

        int result = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);

        switch (result) {
            case CYW43_LINK_UP:
                memcpy(self->connected_ssid, ssid, ssid_len);
                self->connected_ssid_len = ssid_len;
                bindings_cyw43_wifi_enforce_pm();
                return WIFI_RADIO_ERROR_NONE;
            case CYW43_LINK_FAIL:
                return WIFI_RADIO_ERROR_CONNECTION_FAIL;
            case CYW43_LINK_NONET:
                return WIFI_RADIO_ERROR_NO_AP_FOUND;
            case CYW43_LINK_BADAUTH:
                return WIFI_RADIO_ERROR_AUTH_FAIL;
        }
    }

    // Being here means we either timed out or got interrupted.
    return WIFI_RADIO_ERROR_UNSPECIFIED;
}

bool common_hal_wifi_radio_get_connected(wifi_radio_obj_t *self) {
    return cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP;
}

mp_obj_t common_hal_wifi_radio_get_ap_info(wifi_radio_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
}

mp_obj_t common_hal_wifi_radio_get_ipv4_gateway(wifi_radio_obj_t *self) {
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP) {
        return mp_const_none;
    }
    return common_hal_ipaddress_new_ipv4address(NETIF_STA->gw.addr);
}

mp_obj_t common_hal_wifi_radio_get_ipv4_gateway_ap(wifi_radio_obj_t *self) {
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_AP) != CYW43_LINK_UP) {
        return mp_const_none;
    }
    return common_hal_ipaddress_new_ipv4address(NETIF_AP->gw.addr);
}

mp_obj_t common_hal_wifi_radio_get_ipv4_subnet(wifi_radio_obj_t *self) {
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP) {
        return mp_const_none;
    }
    return common_hal_ipaddress_new_ipv4address(NETIF_STA->netmask.addr);
}

mp_obj_t common_hal_wifi_radio_get_ipv4_subnet_ap(wifi_radio_obj_t *self) {
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_AP) != CYW43_LINK_UP) {
        return mp_const_none;
    }
    return common_hal_ipaddress_new_ipv4address(NETIF_AP->netmask.addr);
}

uint32_t wifi_radio_get_ipv4_address(wifi_radio_obj_t *self) {
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP) {
        return 0;
    }
    return NETIF_STA->ip_addr.addr;
}

mp_obj_t common_hal_wifi_radio_get_ipv4_address(wifi_radio_obj_t *self) {
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP) {
        return mp_const_none;
    }
    return common_hal_ipaddress_new_ipv4address(NETIF_STA->ip_addr.addr);
}

mp_obj_t common_hal_wifi_radio_get_ipv4_address_ap(wifi_radio_obj_t *self) {
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_AP) != CYW43_LINK_UP) {
        return mp_const_none;
    }
    return common_hal_ipaddress_new_ipv4address(NETIF_AP->ip_addr.addr);
}

mp_obj_t common_hal_wifi_radio_get_ipv4_dns(wifi_radio_obj_t *self) {
    uint32_t addr = dns_getserver(0)->addr;
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP || addr == 0) {
        return mp_const_none;
    }
    return common_hal_ipaddress_new_ipv4address(addr);
}

void common_hal_wifi_radio_set_ipv4_dns(wifi_radio_obj_t *self, mp_obj_t ipv4_dns_addr) {
    ip4_addr_t addr;
    ipaddress_ipaddress_to_lwip(ipv4_dns_addr, &addr);
    dns_setserver(0, &addr);
}

void common_hal_wifi_radio_start_dhcp_client(wifi_radio_obj_t *self, bool ipv4, bool ipv6) {
    if (ipv4) {
        dhcp_start(NETIF_STA);
    } else {
        dhcp_stop(NETIF_STA);
    }
    if (ipv6) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_ipv6);
    }
}

void common_hal_wifi_radio_stop_dhcp_client(wifi_radio_obj_t *self) {
    dhcp_stop(NETIF_STA);
}

void common_hal_wifi_radio_start_dhcp_server(wifi_radio_obj_t *self) {
    ip4_addr_t ipv4_addr, netmask_addr;
    ipaddress_ipaddress_to_lwip(common_hal_wifi_radio_get_ipv4_address_ap(self), &ipv4_addr);
    ipaddress_ipaddress_to_lwip(common_hal_wifi_radio_get_ipv4_subnet_ap(self), &netmask_addr);
    dhcp_server_init(&cyw43_state.dhcp_server, &ipv4_addr, &netmask_addr);
}

void common_hal_wifi_radio_stop_dhcp_server(wifi_radio_obj_t *self) {
    dhcp_server_deinit(&cyw43_state.dhcp_server);
}

void common_hal_wifi_radio_set_ipv4_address(wifi_radio_obj_t *self, mp_obj_t ipv4, mp_obj_t netmask, mp_obj_t gateway, mp_obj_t ipv4_dns) {
    common_hal_wifi_radio_stop_dhcp_client(self);

    ip4_addr_t ipv4_addr, netmask_addr, gateway_addr;
    ipaddress_ipaddress_to_lwip(ipv4, &ipv4_addr);
    ipaddress_ipaddress_to_lwip(netmask, &netmask_addr);
    ipaddress_ipaddress_to_lwip(gateway, &gateway_addr);
    netif_set_addr(NETIF_STA, &ipv4_addr, &netmask_addr, &gateway_addr);
    if (ipv4_dns != MP_OBJ_NULL) {
        common_hal_wifi_radio_set_ipv4_dns(self, ipv4_dns);
    }
}

void common_hal_wifi_radio_set_ipv4_address_ap(wifi_radio_obj_t *self, mp_obj_t ipv4, mp_obj_t netmask, mp_obj_t gateway) {
    common_hal_wifi_radio_stop_dhcp_server(self);

    ip4_addr_t ipv4_addr, netmask_addr, gateway_addr;
    ipaddress_ipaddress_to_lwip(ipv4, &ipv4_addr);
    ipaddress_ipaddress_to_lwip(netmask, &netmask_addr);
    ipaddress_ipaddress_to_lwip(gateway, &gateway_addr);
    netif_set_addr(NETIF_AP, &ipv4_addr, &netmask_addr, &gateway_addr);

    common_hal_wifi_radio_start_dhcp_server(self);
}

volatile bool ping_received;
uint32_t ping_time;

static u8_t
ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr) {
    struct icmp_echo_hdr *iecho;
    LWIP_ASSERT("p != NULL", p != NULL);

    if ((p->tot_len >= (PBUF_IP_HLEN + sizeof(struct icmp_echo_hdr))) &&
        pbuf_remove_header(p, PBUF_IP_HLEN) == 0) {

        iecho = (struct icmp_echo_hdr *)p->payload;

        if ((iecho->id == PING_ID) && (iecho->seqno == lwip_htons(ping_seq_num))) {
            LWIP_DEBUGF(PING_DEBUG, ("ping: recv "));
            ip_addr_debug_print(PING_DEBUG, addr);
            LWIP_DEBUGF(PING_DEBUG, (" %"U32_F " ms\n", (sys_now() - ping_time)));

            /* do some ping result processing */
            ping_received = true;
            pbuf_free(p);
            return 1; /* eat the packet */
        }
        /* not eaten, restore original packet */
        pbuf_add_header(p, PBUF_IP_HLEN);
    }
    return 0; /* don't eat the packet */
}

mp_int_t common_hal_wifi_radio_ping(wifi_radio_obj_t *self, mp_obj_t ip_address, mp_float_t timeout) {
    ping_time = sys_now();
    ip_addr_t ping_addr;
    if (mp_obj_is_str(ip_address)) {
        socketpool_resolve_host_raise(mp_obj_str_get_str(ip_address), &ping_addr);
    } else {
        ipaddress_ipaddress_to_lwip(ip_address, &ping_addr);
    }

    struct raw_pcb *ping_pcb;
    MICROPY_PY_LWIP_ENTER
        ping_pcb = raw_new(IP_PROTO_ICMP);
    if (!ping_pcb) {
        MICROPY_PY_LWIP_EXIT
        return -1;
    }
    raw_recv(ping_pcb, ping_recv, NULL);
    raw_bind(ping_pcb, IP_ADDR_ANY);
    MICROPY_PY_LWIP_EXIT

        ping_received = false;
    ping_send(ping_pcb, &ping_addr);
    size_t timeout_ms = (size_t)MICROPY_FLOAT_C_FUN(ceil)(timeout * 1000);
    uint64_t start = port_get_raw_ticks(NULL);
    uint64_t deadline = start + timeout_ms;
    while (port_get_raw_ticks(NULL) < deadline && !ping_received) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            break;
        }
    }
    mp_int_t result = -1;
    if (ping_received) {
        uint64_t now = port_get_raw_ticks(NULL);
        result = now - start;
    }

    MICROPY_PY_LWIP_ENTER;
    raw_remove(ping_pcb);
    MICROPY_PY_LWIP_EXIT;

    return result;
}

void common_hal_wifi_radio_gc_collect(wifi_radio_obj_t *self) {
    // Only bother to scan the actual object references.
    gc_collect_ptr(self->current_scan);
}

mp_obj_t common_hal_wifi_radio_get_addresses(wifi_radio_obj_t *self) {
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP) {
        return mp_const_empty_tuple;
    }
    mp_obj_t args[] = {
        socketpool_ip_addr_to_str(&NETIF_STA->ip_addr),
    };
    return mp_obj_new_tuple(1, args);
}

mp_obj_t common_hal_wifi_radio_get_addresses_ap(wifi_radio_obj_t *self) {
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_AP) != CYW43_LINK_UP) {
        return mp_const_empty_tuple;
    }
    mp_obj_t args[] = {
        socketpool_ip_addr_to_str(&NETIF_AP->ip_addr),
    };
    return mp_obj_new_tuple(MP_ARRAY_SIZE(args), args);
}

mp_obj_t common_hal_wifi_radio_get_dns(wifi_radio_obj_t *self) {
    const ip_addr_t *dns_addr = dns_getserver(0);
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP || dns_addr->addr == 0) {
        return mp_const_empty_tuple;
    }
    mp_obj_t args[] = {
        socketpool_ip_addr_to_str(dns_addr),
    };
    return mp_obj_new_tuple(MP_ARRAY_SIZE(args), args);
}

void common_hal_wifi_radio_set_dns(wifi_radio_obj_t *self, mp_obj_t dns_addrs_obj) {
    mp_int_t len = mp_obj_get_int(mp_obj_len(dns_addrs_obj));
    mp_arg_validate_length_max(len, 1, MP_QSTR_dns);
    ip_addr_t addr;
    if (len == 0) {
        addr.addr = IPADDR_NONE;
    } else {
        mp_obj_t dns_addr_obj = mp_obj_subscr(dns_addrs_obj, MP_OBJ_NEW_SMALL_INT(0), MP_OBJ_SENTINEL);
        socketpool_resolve_host_raise(dns_addr_obj, &addr);
    }
    dns_setserver(0, &addr);
}
