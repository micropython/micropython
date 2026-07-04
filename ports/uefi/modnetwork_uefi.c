/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
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

// The `network` module for the UEFI port: bring interfaces up/down and configure
// IP (static / DHCP) over the native EFI networking protocols. Lazy NIC discovery
// (never cached — a NIC connected at runtime shows up on the next query)
// + the LAN interface over EFI_SIMPLE_NETWORK_PROTOCOL (link/MAC) and
// EFI_IP4_CONFIG2_PROTOCOL (address/policy).

#include <string.h>

#include "py/runtime.h"
#include "py/objstr.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "uefi_port.h"
#include "efi.h"
#include "efi_net.h"

#define BS (mp_uefi_st->BootServices)

#ifndef EFI_ALREADY_STARTED
#define EFI_ALREADY_STARTED EFIERR(20)
#endif

static const EFI_GUID g_snp = EFI_SIMPLE_NETWORK_PROTOCOL_GUID;
static const EFI_GUID g_ip4cfg2 = EFI_IP4_CONFIG2_PROTOCOL_GUID;
static const EFI_GUID g_ip4_sb = EFI_IP4_SERVICE_BINDING_PROTOCOL_GUID;
static const EFI_GUID g_ip4 = EFI_IP4_PROTOCOL_GUID;

// Module-wide state (plain C strings; not GC-managed). Hostname is stored only (no
// DHCP option-12 injection); country is advisory (no UEFI analogue).
static char net_hostname[33] = "";
static char net_country[3] = "XX";

// ── NIC discovery (lazy — re-located on every call, never cached) ────────────
static UINTN net_nic_count(void) {
    UINTN count = 0;
    EFI_HANDLE *hb = NULL;
    if (EFI_ERROR(BS->LocateHandleBuffer(ByProtocol, (EFI_GUID *)&g_snp, NULL, &count, &hb)) || hb == NULL) {
        return 0;
    }
    BS->FreePool(hb);
    return count;
}

static EFI_HANDLE net_nic_at(UINTN idx) {
    UINTN count = 0;
    EFI_HANDLE *hb = NULL;
    EFI_HANDLE h = NULL;
    if (EFI_ERROR(BS->LocateHandleBuffer(ByProtocol, (EFI_GUID *)&g_snp, NULL, &count, &hb)) || hb == NULL) {
        return NULL;
    }
    if (idx < count) {
        h = hb[idx];
    }
    BS->FreePool(hb);
    return h;
}

static EFI_SIMPLE_NETWORK_PROTOCOL *open_snp(EFI_HANDLE h) {
    EFI_SIMPLE_NETWORK_PROTOCOL *snp = NULL;
    if (EFI_ERROR(BS->HandleProtocol(h, (EFI_GUID *)&g_snp, (void **)&snp))) {
        return NULL;
    }
    return snp;
}

static EFI_IP4_CONFIG2_PROTOCOL *open_ip4cfg2(EFI_HANDLE h) {
    EFI_IP4_CONFIG2_PROTOCOL *p = NULL;
    if (EFI_ERROR(BS->HandleProtocol(h, (EFI_GUID *)&g_ip4cfg2, (void **)&p))) {
        return NULL;
    }
    return p;
}

// ── IPv4 address <-> text ────────────────────────────────────────────────────
static mp_obj_t ipv4_to_str(const UINT8 a[4]) {
    char b[16];
    size_t p = 0;
    for (int i = 0; i < 4; i++) {
        if (i) {
            b[p++] = '.';
        }
        unsigned v = a[i];
        char t[3];
        int k = 0;
        do {
            t[k++] = '0' + v % 10;
            v /= 10;
        } while (v);
        while (k) {
            b[p++] = t[--k];
        }
    }
    return mp_obj_new_str(b, p);
}

// Parse "a.b.c.d" or "a.b.c.d/prefix"; fill out[4]; return the prefix (0..32) or -1
// if none was given. Raises ValueError on malformed input.
static int str_to_ipv4(const char *s, UINT8 out[4]) {
    unsigned oct = 0, ndig = 0, idx = 0;
    int prefix = -1;
    const char *p = s;
    for (;; p++) {
        char c = *p;
        if (c >= '0' && c <= '9') {
            oct = oct * 10 + (unsigned)(c - '0');
            ndig++;
            if (oct > 255) {
                goto bad;
            }
        } else if (c == '.') {
            if (ndig == 0 || idx >= 3) {
                goto bad;
            }
            out[idx++] = (UINT8)oct;
            oct = 0;
            ndig = 0;
        } else if (c == '/' || c == '\0') {
            if (ndig == 0 || idx != 3) {
                goto bad;
            }
            out[idx++] = (UINT8)oct;
            if (c == '/') {
                prefix = 0;
                ndig = 0;
                for (p++; *p; p++) {
                    if (*p < '0' || *p > '9') {
                        goto bad;
                    }
                    prefix = prefix * 10 + (*p - '0');
                    ndig++;
                }
                if (ndig == 0 || prefix > 32) {
                    goto bad;
                }
            }
            return prefix;
        } else {
            goto bad;
        }
    }
bad:
    mp_raise_ValueError(MP_ERROR_TEXT("bad IPv4 address"));
}

static void prefix_to_mask(int prefix, UINT8 m[4]) {
    uint32_t v = prefix > 0 ? (uint32_t)(0xFFFFFFFFu << (32 - prefix)) : 0u;
    m[0] = v >> 24;
    m[1] = v >> 16;
    m[2] = v >> 8;
    m[3] = v;
}

// ── LAN interface type ───────────────────────────────────────────────────────
typedef struct _network_lan_obj_t {
    mp_obj_base_t base;
    EFI_HANDLE handle;
} network_lan_obj_t;

static const mp_obj_type_t network_lan_type;

static mp_obj_t lan_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_int_t id = n_args > 0 ? mp_obj_get_int(args[0]) : 0;
    EFI_HANDLE h = id < 0 ? NULL : net_nic_at((UINTN)id);
    if (h == NULL) {
        mp_raise_OSError(MP_ENODEV);
    }
    network_lan_obj_t *o = mp_obj_malloc(network_lan_obj_t, type);
    o->handle = h;
    return MP_OBJ_FROM_PTR(o);
}

// Read the interface info (station addr, mask, route table) into `buf`; return the
// struct pointer, or NULL if unavailable.
static EFI_IP4_CONFIG2_INTERFACE_INFO *lan_ifinfo(network_lan_obj_t *self, uint8_t *buf, UINTN buflen) {
    EFI_IP4_CONFIG2_PROTOCOL *cfg = open_ip4cfg2(self->handle);
    if (cfg == NULL) {
        return NULL;
    }
    UINTN sz = buflen;
    if (EFI_ERROR(cfg->GetData(cfg, Ip4Config2DataTypeInterfaceInfo, &sz, buf))) {
        return NULL;
    }
    return (EFI_IP4_CONFIG2_INTERFACE_INFO *)buf;
}

static mp_obj_t lan_active(size_t n_args, const mp_obj_t *args) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    EFI_SIMPLE_NETWORK_PROTOCOL *snp = open_snp(self->handle);
    if (snp == NULL) {
        mp_raise_OSError(MP_ENODEV);
    }
    if (n_args > 1) {
        if (mp_obj_is_true(args[1])) {
            // Bring the interface up. BDS does not auto-connect the network stack
            // (nothing requested it at boot), so bind it first: ConnectController
            // installs MNP/ARP/IP4/IP4Config2/DHCP on this handle and the stack
            // drivers start the NIC. Then make sure the SNP itself is running, in
            // case nothing above it did (best-effort — ignore transient errors).
            BS->ConnectController(self->handle, NULL, NULL, 1 /* recursive */);
            if (snp->Mode != NULL && snp->Mode->State == EFI_SIMPLE_NETWORK_STOPPED) {
                snp->Start(snp);
            }
            if (snp->Mode != NULL && snp->Mode->State == EFI_SIMPLE_NETWORK_STARTED) {
                snp->Initialize(snp, 0, 0);
            }
        } else {
            BS->DisconnectController(self->handle, NULL, NULL);
            snp->Shutdown(snp);
            snp->Stop(snp);
        }
    }
    return mp_obj_new_bool(snp->Mode != NULL && snp->Mode->State == EFI_SIMPLE_NETWORK_INITIALIZED);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lan_active_obj, 1, 2, lan_active);

static mp_obj_t lan_isconnected(mp_obj_t self_in) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(self_in);
    EFI_SIMPLE_NETWORK_PROTOCOL *snp = open_snp(self->handle);
    if (snp == NULL || snp->Mode == NULL) {
        return mp_const_false;
    }
    // Link must be up (when the NIC reports it) AND an IPv4 address must be bound.
    if (snp->Mode->MediaPresentSupported && !snp->Mode->MediaPresent) {
        return mp_const_false;
    }
    uint8_t buf[512];
    EFI_IP4_CONFIG2_INTERFACE_INFO *info = lan_ifinfo(self, buf, sizeof(buf));
    if (info == NULL) {
        return mp_const_false;
    }
    const UINT8 *ip = info->StationAddress.Addr;
    return mp_obj_new_bool(ip[0] | ip[1] | ip[2] | ip[3]);
}
static MP_DEFINE_CONST_FUN_OBJ_1(lan_isconnected_obj, lan_isconnected);

static mp_obj_t lan_status(size_t n_args, const mp_obj_t *args) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args > 1) {
        // No per-parameter status for wired links yet (e.g. 'rssi' is WiFi-only).
        mp_raise_ValueError(MP_ERROR_TEXT("unknown status param"));
    }
    EFI_SIMPLE_NETWORK_PROTOCOL *snp = open_snp(self->handle);
    int up = snp != NULL && snp->Mode != NULL &&
        (!snp->Mode->MediaPresentSupported || snp->Mode->MediaPresent);
    return MP_OBJ_NEW_SMALL_INT(up);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lan_status_obj, 1, 2, lan_status);

// config('mac') / config('hostname') query; config(hostname=...) set.
static mp_obj_t lan_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (kwargs->used == 0) {
        if (n_args != 2) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }
        const char *p = mp_obj_str_get_str(args[1]);
        if (strcmp(p, "mac") == 0) {
            EFI_SIMPLE_NETWORK_PROTOCOL *snp = open_snp(self->handle);
            if (snp == NULL || snp->Mode == NULL) {
                mp_raise_OSError(MP_ENODEV);
            }
            return mp_obj_new_bytes(snp->Mode->CurrentAddress.Addr, 6);
        }
        if (strcmp(p, "hostname") == 0) {
            return mp_obj_new_str(net_hostname, strlen(net_hostname));
        }
        mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
    }
    for (size_t i = 0; i < kwargs->alloc; i++) {
        if (!mp_map_slot_is_filled(kwargs, i)) {
            continue;
        }
        const char *k = mp_obj_str_get_str(kwargs->table[i].key);
        if (strcmp(k, "hostname") == 0) {
            size_t len;
            const char *v = mp_obj_str_get_data(kwargs->table[i].value, &len);
            if (len >= sizeof(net_hostname)) {
                mp_raise_ValueError(MP_ERROR_TEXT("hostname too long"));
            }
            memcpy(net_hostname, v, len);
            net_hostname[len] = 0;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(lan_config_obj, 1, lan_config);

// Set the IPv4 policy + (optionally) a manual address on the interface.
static void lan_set_static(EFI_IP4_CONFIG2_PROTOCOL *cfg, const UINT8 ip[4], const UINT8 mask[4]) {
    EFI_IP4_CONFIG2_POLICY policy = Ip4Config2PolicyStatic;
    if (EFI_ERROR(cfg->SetData(cfg, Ip4Config2DataTypePolicy, sizeof(policy), &policy))) {
        mp_raise_OSError(MP_EIO);
    }
    EFI_IP4_CONFIG2_MANUAL_ADDRESS addr;
    memcpy(addr.Address.Addr, ip, 4);
    memcpy(addr.SubnetMask.Addr, mask, 4);
    if (EFI_ERROR(cfg->SetData(cfg, Ip4Config2DataTypeManualAddress, sizeof(addr), &addr))) {
        mp_raise_OSError(MP_EIO);
    }
}

// ipconfig('param') query; ipconfig(param=value, ...) set. Supported: dhcp4, addr4,
// gw4, dns.
static mp_obj_t lan_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    EFI_IP4_CONFIG2_PROTOCOL *cfg = open_ip4cfg2(self->handle);
    if (cfg == NULL) {
        mp_raise_OSError(MP_ENODEV);
    }
    if (kwargs->used == 0) {
        if (n_args != 2) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }
        const char *p = mp_obj_str_get_str(args[1]);
        uint8_t buf[512];
        EFI_IP4_CONFIG2_INTERFACE_INFO *info = lan_ifinfo(self, buf, sizeof(buf));
        if (strcmp(p, "has_dhcp4") == 0) {
            // True once DHCP has actually bound a non-zero address (the read-only
            // property the MicroPython docs describe — dhcp4 setup is non-blocking).
            const UINT8 *ip = info != NULL ? info->StationAddress.Addr : NULL;
            return mp_obj_new_bool(ip != NULL && (ip[0] | ip[1] | ip[2] | ip[3]));
        }
        if (strcmp(p, "addr4") == 0) {
            if (info == NULL) {
                mp_raise_OSError(MP_EIO);
            }
            mp_obj_t t[2] = { ipv4_to_str(info->StationAddress.Addr), ipv4_to_str(info->SubnetMask.Addr) };
            return mp_obj_new_tuple(2, t);
        }
        if (strcmp(p, "gw4") == 0) {
            // The default gateway is the route whose subnet is 0.0.0.0.
            if (info != NULL && info->RouteTable != NULL) {
                for (UINT32 i = 0; i < info->RouteTableSize; i++) {
                    EFI_IP4_ROUTE_TABLE *rt = &info->RouteTable[i];
                    const UINT8 *sn = rt->SubnetAddress.Addr;
                    if ((sn[0] | sn[1] | sn[2] | sn[3]) == 0) {
                        return ipv4_to_str(rt->GatewayAddress.Addr);
                    }
                }
            }
            return mp_const_none;
        }
        mp_raise_ValueError(MP_ERROR_TEXT("unknown ipconfig param"));
    }
    for (size_t i = 0; i < kwargs->alloc; i++) {
        if (!mp_map_slot_is_filled(kwargs, i)) {
            continue;
        }
        const char *k = mp_obj_str_get_str(kwargs->table[i].key);
        mp_obj_t v = kwargs->table[i].value;
        if (strcmp(k, "dhcp4") == 0) {
            EFI_IP4_CONFIG2_POLICY policy = mp_obj_is_true(v) ? Ip4Config2PolicyDhcp : Ip4Config2PolicyStatic;
            if (EFI_ERROR(cfg->SetData(cfg, Ip4Config2DataTypePolicy, sizeof(policy), &policy))) {
                mp_raise_OSError(MP_EIO);
            }
        } else if (strcmp(k, "addr4") == 0) {
            UINT8 ip[4], mask[4];
            int prefix = str_to_ipv4(mp_obj_str_get_str(v), ip);
            prefix_to_mask(prefix < 0 ? 24 : prefix, mask);
            lan_set_static(cfg, ip, mask);
        } else if (strcmp(k, "gw4") == 0) {
            UINT8 gw[4];
            str_to_ipv4(mp_obj_str_get_str(v), gw);
            EFI_IPv4_ADDRESS a;
            memcpy(a.Addr, gw, 4);
            if (EFI_ERROR(cfg->SetData(cfg, Ip4Config2DataTypeGateway, sizeof(a), &a))) {
                mp_raise_OSError(MP_EIO);
            }
        } else if (strcmp(k, "dns") == 0) {
            UINT8 dns[4];
            str_to_ipv4(mp_obj_str_get_str(v), dns);
            EFI_IPv4_ADDRESS a;
            memcpy(a.Addr, dns, 4);
            if (EFI_ERROR(cfg->SetData(cfg, Ip4Config2DataTypeDnsServer, sizeof(a), &a))) {
                mp_raise_OSError(MP_EIO);
            }
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("unknown ipconfig param"));
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(lan_ipconfig_obj, 1, lan_ipconfig);

// Legacy ifconfig: get/set (ip, mask, gateway, dns).
static mp_obj_t lan_ifconfig(size_t n_args, const mp_obj_t *args) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    EFI_IP4_CONFIG2_PROTOCOL *cfg = open_ip4cfg2(self->handle);
    if (cfg == NULL) {
        mp_raise_OSError(MP_ENODEV);
    }
    if (n_args == 1) {
        uint8_t buf[512];
        EFI_IP4_CONFIG2_INTERFACE_INFO *info = lan_ifinfo(self, buf, sizeof(buf));
        if (info == NULL) {
            mp_raise_OSError(MP_EIO);
        }
        mp_obj_t gw = mp_const_none;
        if (info->RouteTable != NULL) {
            for (UINT32 i = 0; i < info->RouteTableSize; i++) {
                const UINT8 *sn = info->RouteTable[i].SubnetAddress.Addr;
                if ((sn[0] | sn[1] | sn[2] | sn[3]) == 0) {
                    gw = ipv4_to_str(info->RouteTable[i].GatewayAddress.Addr);
                    break;
                }
            }
        }
        mp_obj_t t[4] = {
            ipv4_to_str(info->StationAddress.Addr), ipv4_to_str(info->SubnetMask.Addr),
            gw, mp_const_none,
        };
        return mp_obj_new_tuple(4, t);
    }
    // set: a 4-tuple/list (ip, mask, gw, dns)
    mp_obj_t *cfgv;
    size_t config_len;
    mp_obj_get_array(args[1], &config_len, &cfgv);
    if (config_len != 4) {
        mp_raise_ValueError(MP_ERROR_TEXT("ifconfig needs a 4-tuple"));
    }
    UINT8 ip[4], mask[4], gw[4], dns[4];
    str_to_ipv4(mp_obj_str_get_str(cfgv[0]), ip);
    str_to_ipv4(mp_obj_str_get_str(cfgv[1]), mask);
    lan_set_static(cfg, ip, mask);
    if (cfgv[2] != mp_const_none) {
        str_to_ipv4(mp_obj_str_get_str(cfgv[2]), gw);
        EFI_IPv4_ADDRESS a;
        memcpy(a.Addr, gw, 4);
        cfg->SetData(cfg, Ip4Config2DataTypeGateway, sizeof(a), &a);
    }
    if (cfgv[3] != mp_const_none) {
        str_to_ipv4(mp_obj_str_get_str(cfgv[3]), dns);
        EFI_IPv4_ADDRESS a;
        memcpy(a.Addr, dns, 4);
        cfg->SetData(cfg, Ip4Config2DataTypeDnsServer, sizeof(a), &a);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lan_ifconfig_obj, 1, 2, lan_ifconfig);

static const mp_rom_map_elem_t lan_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&lan_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&lan_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&lan_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&lan_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipconfig), MP_ROM_PTR(&lan_ipconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&lan_ifconfig_obj) },
};
static MP_DEFINE_CONST_DICT(lan_locals_dict, lan_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    network_lan_type,
    MP_QSTR_LAN,
    MP_TYPE_FLAG_NONE,
    make_new, lan_make_new,
    locals_dict, &lan_locals_dict
    );

// ── Module-level functions ───────────────────────────────────────────────────
static mp_obj_t network_interfaces(void) {
    UINTN n = net_nic_count();
    mp_obj_t list = mp_obj_new_list(0, NULL);
    for (UINTN i = 0; i < n; i++) {
        EFI_HANDLE h = net_nic_at(i);
        if (h == NULL) {
            continue;
        }
        network_lan_obj_t *o = mp_obj_malloc(network_lan_obj_t, &network_lan_type);
        o->handle = h;
        mp_obj_list_append(list, MP_OBJ_FROM_PTR(o));
    }
    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_0(network_interfaces_obj, network_interfaces);

static mp_obj_t network_hostname(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_obj_new_str(net_hostname, strlen(net_hostname));
    }
    size_t len;
    const char *s = mp_obj_str_get_data(args[0], &len);
    if (len >= sizeof(net_hostname)) {
        mp_raise_ValueError(MP_ERROR_TEXT("hostname too long"));
    }
    memcpy(net_hostname, s, len);
    net_hostname[len] = 0;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_hostname_obj, 0, 1, network_hostname);

static mp_obj_t network_country(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_obj_new_str(net_country, strlen(net_country));
    }
    size_t len;
    const char *s = mp_obj_str_get_data(args[0], &len);
    if (len != 2) {
        mp_raise_ValueError(MP_ERROR_TEXT("country code must be 2 letters"));
    }
    net_country[0] = s[0];
    net_country[1] = s[1];
    net_country[2] = 0;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_country_obj, 0, 1, network_country);

// Internet checksum (16-bit one's-complement) over an even/odd-length buffer.
static uint16_t inet_checksum(const uint8_t *data, size_t len) {
    uint32_t sum = 0;
    for (size_t i = 0; i + 1 < len; i += 2) {
        sum += (uint32_t)((data[i] << 8) | data[i + 1]);
    }
    if (len & 1) {
        sum += (uint32_t)(data[len - 1] << 8);
    }
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return (uint16_t) ~sum;
}

// network.ping(ip, timeout_ms=1000) -> round-trip ms (int), or None on timeout.
// Sends one ICMP echo request over EFI_IP4 and waits for the matching reply. `ip`
// must be a numeric IPv4 literal (resolve names with socket.getaddrinfo first).
static mp_obj_t network_ping(size_t n_args, const mp_obj_t *args) {
    UINT8 ip[4];
    str_to_ipv4(mp_obj_str_get_str(args[0]), ip);
    int timeout_ms = n_args > 1 ? mp_obj_get_int(args[1]) : 1000;

    UINTN count = 0;
    EFI_HANDLE *hb = NULL;
    if (EFI_ERROR(BS->LocateHandleBuffer(ByProtocol, (EFI_GUID *)&g_ip4_sb, NULL, &count, &hb)) ||
        hb == NULL || count == 0) {
        mp_raise_OSError(MP_ENODEV);
    }
    EFI_HANDLE nic = hb[0];
    BS->FreePool(hb);
    EFI_HANDLE child = NULL;
    EFI_IP4_PROTOCOL *ip4 = NULL;
    if (EFI_ERROR(net_sb_create_child(nic, &g_ip4_sb, &child)) ||
        EFI_ERROR(BS->HandleProtocol(child, (EFI_GUID *)&g_ip4, (void **)&ip4))) {
        mp_raise_OSError(MP_EIO);
    }
    EFI_IP4_CONFIG_DATA cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.DefaultProtocol = 1;         // ICMP
    cfg.AcceptIcmpErrors = 1;
    cfg.UseDefaultAddress = 1;
    cfg.TimeToLive = 64;
    if (EFI_ERROR(ip4->Configure(ip4, &cfg))) {
        net_sb_destroy_child(nic, &g_ip4_sb, child);
        mp_raise_OSError(MP_ENODEV);
    }

    // ICMP echo request: type 8, code 0, checksum, id, seq, + 8 bytes payload.
    uint16_t id = 0xBEEF, seq = 1;
    uint8_t pkt[16];
    memset(pkt, 0, sizeof(pkt));
    pkt[0] = 8;
    pkt[4] = id >> 8;
    pkt[5] = id & 0xFF;
    pkt[6] = seq >> 8;
    pkt[7] = seq & 0xFF;
    for (int i = 8; i < 16; i++) {
        pkt[i] = (uint8_t)i;
    }
    uint16_t ck = inet_checksum(pkt, sizeof(pkt));
    pkt[2] = ck >> 8;
    pkt[3] = ck & 0xFF;

    mp_obj_t result = mp_const_none;
    uint32_t t0 = mp_hal_ticks_ms();
    EFI_IP4_TRANSMIT_DATA tx;
    memset(&tx, 0, sizeof(tx));
    memcpy(tx.DestinationAddress.Addr, ip, 4);
    tx.TotalDataLength = sizeof(pkt);
    tx.FragmentCount = 1;
    tx.FragmentTable[0].FragmentLength = sizeof(pkt);
    tx.FragmentTable[0].FragmentBuffer = pkt;
    net_op_t txop;
    txop.event = NULL;
    if (!EFI_ERROR(net_op_init(&txop))) {
        EFI_IP4_COMPLETION_TOKEN txtok;
        memset(&txtok, 0, sizeof(txtok));
        txtok.Event = txop.event;
        txtok.Packet.TxData = &tx;
        if (!EFI_ERROR(ip4->Transmit(ip4, &txtok)) &&
            net_op_wait(&txop, timeout_ms) == 0 && !EFI_ERROR(txtok.Status)) {
            // Read replies until the matching echo reply arrives or time runs out.
            while (result == mp_const_none && (int)(mp_hal_ticks_ms() - t0) < timeout_ms) {
                net_op_t rxop;
                rxop.event = NULL;
                if (EFI_ERROR(net_op_init(&rxop))) {
                    break;
                }
                EFI_IP4_COMPLETION_TOKEN rxtok;
                memset(&rxtok, 0, sizeof(rxtok));
                rxtok.Event = rxop.event;
                int remain = timeout_ms - (int)(mp_hal_ticks_ms() - t0);
                if (remain < 0) {
                    remain = 0;
                }
                if (EFI_ERROR(ip4->Receive(ip4, &rxtok))) {
                    net_op_close(&rxop);
                    break;
                }
                if (net_op_wait(&rxop, remain) != 0) {
                    ip4->Cancel(ip4, &rxtok);
                    net_op_close(&rxop);
                    break;
                }
                if (!EFI_ERROR(rxtok.Status) && rxtok.Packet.RxData != NULL) {
                    EFI_IP4_RECEIVE_DATA *rx = rxtok.Packet.RxData;
                    if (rx->DataLength >= 8 && rx->FragmentCount >= 1) {
                        const uint8_t *d = rx->FragmentTable[0].FragmentBuffer;
                        if (d[0] == 0 && d[4] == (id >> 8) && d[5] == (id & 0xFF)) {
                            result = mp_obj_new_int(mp_hal_ticks_ms() - t0);   // echo reply matched
                        }
                    }
                    if (rx->RecycleSignal != NULL) {
                        BS->SignalEvent(rx->RecycleSignal);
                    }
                }
                net_op_close(&rxop);
            }
        }
        net_op_close(&txop);
    }
    ip4->Configure(ip4, NULL);
    net_sb_destroy_child(nic, &g_ip4_sb, child);
    return result;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_ping_obj, 1, 2, network_ping);

// Global IP config: ipconfig('dns') query / ipconfig(dns='8.8.8.8') set. The DNS
// override wins over the DHCP-provided resolver for socket.getaddrinfo.
static mp_obj_t network_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    if (kwargs->used == 0) {
        if (n_args != 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }
        const char *p = mp_obj_str_get_str(args[0]);
        if (strcmp(p, "dns") == 0) {
            UINT8 ip[4];
            return net_get_dns_override(ip) ? ipv4_to_str(ip) : mp_const_none;
        }
        mp_raise_ValueError(MP_ERROR_TEXT("unknown ipconfig param"));
    }
    for (size_t i = 0; i < kwargs->alloc; i++) {
        if (!mp_map_slot_is_filled(kwargs, i)) {
            continue;
        }
        const char *k = mp_obj_str_get_str(kwargs->table[i].key);
        if (strcmp(k, "dns") == 0) {
            UINT8 ip[4];
            str_to_ipv4(mp_obj_str_get_str(kwargs->table[i].value), ip);
            net_set_dns_override(ip);
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("unknown ipconfig param"));
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_ipconfig_obj, 0, network_ipconfig);

static const mp_rom_map_elem_t network_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_network) },
    { MP_ROM_QSTR(MP_QSTR_LAN), MP_ROM_PTR(&network_lan_type) },
    { MP_ROM_QSTR(MP_QSTR_interfaces), MP_ROM_PTR(&network_interfaces_obj) },
    { MP_ROM_QSTR(MP_QSTR_hostname), MP_ROM_PTR(&network_hostname_obj) },
    { MP_ROM_QSTR(MP_QSTR_country), MP_ROM_PTR(&network_country_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipconfig), MP_ROM_PTR(&network_ipconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_ping), MP_ROM_PTR(&network_ping_obj) },
};
static MP_DEFINE_CONST_DICT(network_globals, network_globals_table);

const mp_obj_module_t mp_module_network = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&network_globals,
};
MP_REGISTER_MODULE(MP_QSTR_network, mp_module_network);
