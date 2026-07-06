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

// ── WLAN (WiFi station) over EFI WiFi2 ───────────────────────────────────────
// L2 association via EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL (+ EFI_SUPPLICANT_
// PROTOCOL for the WPA2-PSK credentials); everything above L2 (DHCP, addressing,
// sockets, DNS, TLS) reuses the LAN path on the same NIC handle. STA only — there
// is no UEFI SoftAP protocol, so IF_AP is unsupported. connect() is non-blocking
// (MCU idiom): it posts the ConnectNetwork token and returns; status()/
// isconnected() observe progress. See README.md / TODO.md.

#define WLAN_IF_STA 0
#define WLAN_IF_AP  1

// Public status codes (network.WLAN.STAT_* and module-level network.STAT_*).
#define WLAN_STAT_IDLE           0
#define WLAN_STAT_CONNECTING     1
#define WLAN_STAT_GOT_IP         2
#define WLAN_STAT_NO_AP_FOUND    3
#define WLAN_STAT_WRONG_PASSWORD 4
#define WLAN_STAT_CONNECT_FAIL   5

// Internal association state machine. connect() is fully non-blocking: it kicks off
// the scan and returns; wlan_poll() (driven by status()/isconnected()) advances
// SCANNING -> set credentials -> CONNECTING -> ASSOCIATED/FAILED. Each poll returns
// immediately and the caller sleeps between them, so the CPU stays idle (letting the
// driver's timer callbacks run) instead of spinning on a busy wait.
enum { CONN_IDLE, CONN_SCANNING, CONN_CONNECTING, CONN_ASSOCIATED, CONN_FAILED };

// How long to wait, in ms, for the scan and for the association to complete.
#define WLAN_SCAN_TIMEOUT_MS    15000
#define WLAN_CONNECT_TIMEOUT_MS 45000

static const EFI_GUID g_wmc2 = EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL_GUID;
static const EFI_GUID g_supplicant = EFI_SUPPLICANT_PROTOCOL_GUID;
static const EFI_GUID g_supplicant_sb = EFI_SUPPLICANT_SERVICE_BINDING_PROTOCOL_GUID;

// Single-radio pending-connect state. File-static (not GC-managed) so the tokens/
// buffers keep stable addresses for the firmware and the completion notify (which
// runs at TPL_CALLBACK) never touches the GC heap. See uefi_net.c.
static struct {
    // Scan phase.
    net_op_t scan_op;
    EFI_80211_GET_NETWORKS_TOKEN scan_tok;
    EFI_80211_GET_NETWORKS_DATA scan_data;
    // Connect phase — data.Network points into the retained scan_result, so it stays
    // valid (and no other WiFi2 call happens) between scan completion and ConnectNetwork.
    net_op_t op;
    EFI_80211_CONNECT_NETWORK_TOKEN token;
    EFI_80211_CONNECT_NETWORK_DATA data;
    EFI_80211_GET_NETWORKS_RESULT *scan_result;   // retained; freed on teardown
    EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL *wmc;  // cached so the poll can issue connect
    EFI_HANDLE nic;
    EFI_HANDLE sup_child;    // supplicant child we created (holds the PSK); freed on teardown
    EFI_HANDLE sup_nic;      // handle the supplicant SB child was created on (may != nic)
    uint32_t phase_start;    // mp_hal_ticks_ms() at the start of the current phase
    uint8_t key[EFI_MAX_SSID_LEN * 2 + 1];   // credentials, applied at scan completion
    uint8_t key_len;
    uint8_t has_key;
    int state;
    int fail_stat;           // WLAN_STAT_* when state == CONN_FAILED
    uint8_t ssid[EFI_MAX_SSID_LEN];
    uint8_t ssid_len;
    uint8_t quality;         // last connect/scan NetworkQuality, reported as status('rssi')
} g_conn;

// WLAN object — MUST share the {base, handle} prefix with network_lan_obj_t so the
// shared L3 method objects (lan_active/lan_ipconfig/lan_ifconfig) work on it.
typedef struct _network_wlan_obj_t {
    mp_obj_base_t base;
    EFI_HANDLE handle;
    uint8_t is_sta;
} network_wlan_obj_t;

static const mp_obj_type_t network_wlan_type;

static EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL *open_wmc2(EFI_HANDLE h) {
    EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL *w = NULL;
    if (EFI_ERROR(BS->HandleProtocol(h, (EFI_GUID *)&g_wmc2, (void **)&w))) {
        return NULL;
    }
    return w;
}

static EFI_HANDLE net_wmc2_first(void) {
    UINTN count = 0;
    EFI_HANDLE *hb = NULL;
    EFI_HANDLE h = NULL;
    if (EFI_ERROR(BS->LocateHandleBuffer(ByProtocol, (EFI_GUID *)&g_wmc2, NULL, &count, &hb)) || hb == NULL) {
        return NULL;
    }
    if (count > 0) {
        h = hb[0];
    }
    BS->FreePool(hb);
    return h;
}

// True once the interface holds a non-zero IPv4 station address (implies associated
// + a DHCP/static lease). Same test lan_isconnected() uses, keyed on a bare handle.
static int iface_has_ip(EFI_HANDLE h) {
    EFI_IP4_CONFIG2_PROTOCOL *cfg = open_ip4cfg2(h);
    if (cfg == NULL) {
        return 0;
    }
    uint8_t buf[512];
    UINTN sz = sizeof(buf);
    if (EFI_ERROR(cfg->GetData(cfg, Ip4Config2DataTypeInterfaceInfo, &sz, buf))) {
        return 0;
    }
    const UINT8 *ip = ((EFI_IP4_CONFIG2_INTERFACE_INFO *)buf)->StationAddress.Addr;
    return ip[0] | ip[1] | ip[2] | ip[3];
}

static void wlan_destroy_sup_child(void) {
    if (g_conn.sup_child != NULL) {
        net_sb_destroy_child(g_conn.sup_nic, &g_supplicant_sb, g_conn.sup_child);
        g_conn.sup_child = NULL;
    }
}

static void wlan_free_scan(void) {
    if (g_conn.scan_result != NULL) {
        BS->FreePool(g_conn.scan_result);
        g_conn.scan_result = NULL;
    }
}

// Close any pending completion events, release the supplicant child, free the
// retained scan result.
static void wlan_cleanup_pending(void) {
    if (g_conn.scan_op.event != NULL) {
        net_op_close(&g_conn.scan_op);
    }
    if (g_conn.op.event != NULL) {
        net_op_close(&g_conn.op);
    }
    wlan_destroy_sup_child();
    wlan_free_scan();
}

// Run one WMC2 GetNetworks (broadcast scan-all) and return the driver-allocated
// Result (caller frees via BS->FreePool), or NULL. Blocking, bounded by timeout_ms.
// Used by scan(); connect() drives its own scan non-blocking via wlan_poll().
static EFI_80211_GET_NETWORKS_RESULT *wlan_get_networks(EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL *wmc,
    int timeout_ms) {
    EFI_80211_GET_NETWORKS_DATA data;
    memset(&data, 0, sizeof(data));
    data.NumOfSSID = 0;    // broadcast probe: scan all
    net_op_t op;
    op.event = NULL;
    if (EFI_ERROR(net_op_init(&op))) {
        return NULL;
    }
    EFI_80211_GET_NETWORKS_TOKEN tok;
    memset(&tok, 0, sizeof(tok));
    tok.Event = op.event;
    tok.Data = &data;
    EFI_80211_GET_NETWORKS_RESULT *res = NULL;
    EFI_STATUS st = wmc->GetNetworks(wmc, &tok);
    if ((!EFI_ERROR(st) || st == EFI_NOT_READY) && net_op_wait(&op, timeout_ms) == 0 &&
        !EFI_ERROR(tok.Status)) {
        res = tok.Result;
    }
    net_op_close(&op);
    return res;
}

static int wlan_set_psk(EFI_HANDLE nic, const uint8_t *ssid, size_t ssid_len,
    const uint8_t *key, size_t key_len);

// Advance the non-blocking connect state machine (from status()/isconnected()). Each
// call is cheap and returns immediately; the caller sleeps between polls, so the CPU
// idles and the driver's timer callbacks (which drive scan + association) get to run.
static void wlan_poll(void) {
    // SCANNING: once GetNetworks finishes, find our SSID, push credentials and issue
    // ConnectNetwork — with no intervening WiFi2 call, so the descriptor pointer into
    // the retained scan Result stays valid.
    if (g_conn.state == CONN_SCANNING) {
        if (g_conn.scan_op.done) {
            g_conn.scan_result = EFI_ERROR(g_conn.scan_tok.Status) ? NULL : g_conn.scan_tok.Result;
            net_op_close(&g_conn.scan_op);
            EFI_80211_NETWORK *match = NULL;
            if (g_conn.scan_result != NULL) {
                for (UINT8 i = 0; i < g_conn.scan_result->NumOfNetworkDesc; i++) {
                    EFI_80211_NETWORK_DESCRIPTION *d = &g_conn.scan_result->NetworkDesc[i];
                    if (d->Network.SSId.SSIdLen == g_conn.ssid_len &&
                        memcmp(d->Network.SSId.SSId, g_conn.ssid, g_conn.ssid_len) == 0) {
                        match = &d->Network;
                        g_conn.quality = d->NetworkQuality;
                        break;
                    }
                }
            }
            if (match == NULL) {
                g_conn.state = CONN_FAILED;
                g_conn.fail_stat = WLAN_STAT_NO_AP_FOUND;
                wlan_free_scan();
                return;
            }
            if (g_conn.has_key) {
                wlan_set_psk(g_conn.nic, g_conn.ssid, g_conn.ssid_len, g_conn.key, g_conn.key_len);
            }
            g_conn.data.Network = match;
            g_conn.data.FailureTimeout = 30;
            EFI_STATUS st = net_op_init(&g_conn.op);
            if (!EFI_ERROR(st)) {
                g_conn.token.Event = g_conn.op.event;
                g_conn.token.Data = &g_conn.data;
                st = g_conn.wmc->ConnectNetwork(g_conn.wmc, &g_conn.token);
            }
            if (EFI_ERROR(st) && st != EFI_NOT_READY) {
                g_conn.state = CONN_FAILED;
                g_conn.fail_stat = WLAN_STAT_CONNECT_FAIL;
                wlan_cleanup_pending();
                return;
            }
            g_conn.state = CONN_CONNECTING;
            g_conn.phase_start = mp_hal_ticks_ms();
        } else if (mp_hal_ticks_ms() - g_conn.phase_start > WLAN_SCAN_TIMEOUT_MS) {
            g_conn.state = CONN_FAILED;
            g_conn.fail_stat = WLAN_STAT_NO_AP_FOUND;
            wlan_cleanup_pending();
        }
        return;
    }
    // CONNECTING: wait for association + the 4-way handshake to complete.
    if (g_conn.state == CONN_CONNECTING) {
        if (g_conn.op.done) {
            if (!EFI_ERROR(g_conn.token.Status) && g_conn.token.ResultCode == ConnectSuccess) {
                g_conn.state = CONN_ASSOCIATED;
            } else {
                g_conn.state = CONN_FAILED;
                switch (g_conn.token.ResultCode) {
                    case ConnectRefused:         // auth/assoc rejected — most often a bad key
                        g_conn.fail_stat = WLAN_STAT_WRONG_PASSWORD;
                        break;
                    case ConnectFailureTimeout:  // no AP answered in FailureTimeout
                        g_conn.fail_stat = WLAN_STAT_NO_AP_FOUND;
                        break;
                    default:
                        g_conn.fail_stat = WLAN_STAT_CONNECT_FAIL;
                        break;
                }
            }
            net_op_close(&g_conn.op);
        } else if (mp_hal_ticks_ms() - g_conn.phase_start > WLAN_CONNECT_TIMEOUT_MS) {
            g_conn.state = CONN_FAILED;
            g_conn.fail_stat = WLAN_STAT_CONNECT_FAIL;
            wlan_cleanup_pending();
        }
    }
}

// Best-effort security class from a scan result's RSN suites: 0 open, 2 WPA (TKIP),
// 3 WPA2 (CCMP). WiFi2 exposes no finer detail; default secured networks to WPA2.
static int security_from_network(const EFI_80211_NETWORK *n) {
    if (n->AKMSuite == NULL || n->AKMSuite->AKMSuiteCount == 0) {
        return 0;
    }
    if (n->CipherSuite != NULL && n->CipherSuite->CipherSuiteCount > 0) {
        UINT8 c = n->CipherSuite->CipherSuiteList[0].SuiteType;
        if (c == EFI_80211_CIPHER_SUITE_CCMP) {
            return 3;
        }
        if (c == 2 /* TKIP */) {
            return 2;
        }
    }
    return 3;
}

static mp_obj_t wlan_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_int_t iface = n_args > 0 ? mp_obj_get_int(args[0]) : WLAN_IF_STA;
    if (iface == WLAN_IF_AP) {
        mp_raise_OSError(MP_EOPNOTSUPP);    // no UEFI SoftAP protocol
    }
    if (iface != WLAN_IF_STA) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid WLAN interface"));
    }
    EFI_HANDLE h = net_wmc2_first();
    if (h == NULL) {
        mp_raise_OSError(MP_ENODEV);        // no EFI WiFi2 NIC present
    }
    network_wlan_obj_t *o = mp_obj_malloc(network_wlan_obj_t, type);
    o->handle = h;
    o->is_sta = 1;
    return MP_OBJ_FROM_PTR(o);
}

// scan() -> [(ssid, bssid, channel, RSSI, security, hidden), ...]. Blocking (rides
// WFE, Ctrl-C-able). NOTE: WiFi2's network description carries no BSSID and no
// channel, so bssid is b'' and channel 0; RSSI is NetworkQuality (0..100), not dBm.
static mp_obj_t wlan_scan(mp_obj_t self_in) {
    network_wlan_obj_t *self = MP_OBJ_TO_PTR(self_in);
    EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL *wmc = open_wmc2(self->handle);
    if (wmc == NULL) {
        mp_raise_OSError(MP_ENODEV);
    }
    mp_obj_t list = mp_obj_new_list(0, NULL);
    EFI_80211_GET_NETWORKS_RESULT *res = wlan_get_networks(wmc, 10000);
    if (res != NULL) {
        for (UINT8 i = 0; i < res->NumOfNetworkDesc; i++) {
            EFI_80211_NETWORK_DESCRIPTION *d = &res->NetworkDesc[i];
            int sslen = d->Network.SSId.SSIdLen;
            if (sslen > EFI_MAX_SSID_LEN) {
                sslen = EFI_MAX_SSID_LEN;
            }
            mp_obj_t t[6] = {
                mp_obj_new_bytes(d->Network.SSId.SSId, sslen),
                mp_const_empty_bytes,                          // bssid: not exposed by WiFi2
                MP_OBJ_NEW_SMALL_INT(0),                       // channel: not exposed by WiFi2
                MP_OBJ_NEW_SMALL_INT(d->NetworkQuality),       // RSSI proxy (link quality 0..100)
                MP_OBJ_NEW_SMALL_INT(security_from_network(&d->Network)),
                MP_OBJ_NEW_SMALL_INT(sslen == 0 ? 1 : 0),      // hidden
            };
            mp_obj_list_append(list, mp_obj_new_tuple(6, t));
        }
        BS->FreePool(res);
    }
    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_1(wlan_scan_obj, wlan_scan);

// Set the WPA2-PSK credentials on a supplicant. Required data formats: the SSID is a
// full EFI_80211_SSID struct (len + 32 bytes); the PSK is a NUL-terminated ASCII
// string whose DataSize includes the NUL. TargetSSIDName is optional (the connect
// descriptor also carries the SSID), so its result is ignored. The driver runs the
// 4-way handshake through the supplicant. Returns 1 if the PSK was accepted.
static int wlan_sup_setdata(EFI_SUPPLICANT_PROTOCOL *sup, const uint8_t *ssid, size_t ssid_len,
    const uint8_t *key, size_t key_len) {
    EFI_80211_SSID ss;
    memset(&ss, 0, sizeof(ss));
    ss.SSIdLen = (uint8_t)ssid_len;
    memcpy(ss.SSId, ssid, ssid_len);
    sup->SetData(sup, EfiSupplicant80211TargetSSIDName, &ss, sizeof(ss));   // optional; ignore result
    uint8_t psk[EFI_MAX_SSID_LEN * 2 + 1];   // WPA PSK is <= 63 chars + NUL
    memcpy(psk, key, key_len);
    psk[key_len] = 0;
    EFI_STATUS s2 = sup->SetData(sup, EfiSupplicant80211PskPassword, psk, key_len + 1);
    return !EFI_ERROR(s2);
}

// Locate the supplicant and set the credentials on it. Prefer the supplicant on the
// WMC2 NIC handle — the instance the driver uses during ConnectNetwork; fall back to a
// supplicant service binding (create a child) or a supplicant on another handle.
// Returns 1 if credentials were set.
static int wlan_set_psk(EFI_HANDLE nic, const uint8_t *ssid, size_t ssid_len,
    const uint8_t *key, size_t key_len) {
    UINTN n = 0;
    EFI_HANDLE *hb = NULL;

    // Preferred: a supplicant installed directly on the WMC2 NIC handle. A standalone
    // supplicant service binding, if present, is a separate, unrelated instance.
    EFI_SUPPLICANT_PROTOCOL *nsup = NULL;
    if (!EFI_ERROR(BS->HandleProtocol(nic, (EFI_GUID *)&g_supplicant, (void **)&nsup)) && nsup != NULL) {
        return wlan_sup_setdata(nsup, ssid, ssid_len, key, key_len);
    }

    // Strategy 1: a supplicant service binding on any handle — create a child.
    if (!EFI_ERROR(BS->LocateHandleBuffer(ByProtocol, (EFI_GUID *)&g_supplicant_sb, NULL, &n, &hb)) && hb) {
        for (UINTN i = 0; i < n; i++) {
            EFI_HANDLE child = NULL;
            if (EFI_ERROR(net_sb_create_child(hb[i], &g_supplicant_sb, &child))) {
                continue;
            }
            EFI_SUPPLICANT_PROTOCOL *sup = NULL;
            if (EFI_ERROR(BS->HandleProtocol(child, (EFI_GUID *)&g_supplicant, (void **)&sup)) || sup == NULL) {
                net_sb_destroy_child(hb[i], &g_supplicant_sb, child);
                continue;
            }
            g_conn.sup_nic = hb[i];
            g_conn.sup_child = child;
            BS->FreePool(hb);
            return wlan_sup_setdata(sup, ssid, ssid_len, key, key_len);
        }
        BS->FreePool(hb);
        hb = NULL;
    }

    // Strategy 2: a supplicant protocol installed directly on some handle.
    n = 0;
    if (!EFI_ERROR(BS->LocateHandleBuffer(ByProtocol, (EFI_GUID *)&g_supplicant, NULL, &n, &hb)) && hb) {
        for (UINTN i = 0; i < n; i++) {
            EFI_SUPPLICANT_PROTOCOL *sup = NULL;
            if (EFI_ERROR(BS->HandleProtocol(hb[i], (EFI_GUID *)&g_supplicant, (void **)&sup)) || sup == NULL) {
                continue;
            }
            BS->FreePool(hb);
            return wlan_sup_setdata(sup, ssid, ssid_len, key, key_len);
        }
        BS->FreePool(hb);
    }
    return 0;
}

// connect(ssid, key=None, *, bssid=None). Non-blocking: kicks off the scan and
// returns immediately; the scan → set-credentials → ConnectNetwork sequence is driven
// by wlan_poll() from status()/isconnected(). An open network omits `key`.
static mp_obj_t wlan_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    network_wlan_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    enum { ARG_ssid, ARG_key, ARG_bssid };
    static const mp_arg_t allowed[] = {
        { MP_QSTR_ssid, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_key, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };
    mp_arg_val_t vals[MP_ARRAY_SIZE(allowed)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed), allowed, vals);

    if (vals[ARG_ssid].u_obj == mp_const_none) {
        mp_raise_ValueError(MP_ERROR_TEXT("ssid required"));
    }
    size_t ssid_len;
    const char *ssid = mp_obj_str_get_data(vals[ARG_ssid].u_obj, &ssid_len);
    if (ssid_len > EFI_MAX_SSID_LEN) {
        mp_raise_ValueError(MP_ERROR_TEXT("ssid too long"));
    }
    const char *key = NULL;
    size_t key_len = 0;
    if (vals[ARG_key].u_obj != mp_const_none) {
        key = mp_obj_str_get_data(vals[ARG_key].u_obj, &key_len);
        if (key_len > 63) {
            mp_raise_ValueError(MP_ERROR_TEXT("key too long"));
        }
    }
    if (vals[ARG_bssid].u_obj != mp_const_none) {
        // WiFi2's connect data has no BSSID field, so we can't pin a specific AP.
        mp_raise_NotImplementedError(MP_ERROR_TEXT("bssid not supported by EFI WiFi2"));
    }

    EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL *wmc = open_wmc2(self->handle);
    if (wmc == NULL) {
        mp_raise_OSError(MP_ENODEV);
    }

    wlan_cleanup_pending();          // drop any prior child/event/scan before we reset
    memset(&g_conn, 0, sizeof(g_conn));
    g_conn.nic = self->handle;
    g_conn.wmc = wmc;
    memcpy(g_conn.ssid, ssid, ssid_len);
    g_conn.ssid_len = (uint8_t)ssid_len;
    if (key != NULL && key_len > 0) {
        memcpy(g_conn.key, key, key_len);
        g_conn.key_len = (uint8_t)key_len;
        g_conn.has_key = 1;
    }

    // Kick off the scan (full/broadcast) and return; wlan_poll() finds our SSID once it
    // completes, sets credentials, and issues ConnectNetwork with the descriptor still
    // inside the driver's own scan Result (no intervening WiFi2 call).
    if (EFI_ERROR(net_op_init(&g_conn.scan_op))) {
        g_conn.state = CONN_IDLE;
        mp_raise_OSError(MP_EIO);
    }
    memset(&g_conn.scan_data, 0, sizeof(g_conn.scan_data));
    g_conn.scan_data.NumOfSSID = 0;
    memset(&g_conn.scan_tok, 0, sizeof(g_conn.scan_tok));
    g_conn.scan_tok.Event = g_conn.scan_op.event;
    g_conn.scan_tok.Data = &g_conn.scan_data;
    EFI_STATUS st = wmc->GetNetworks(wmc, &g_conn.scan_tok);
    if (EFI_ERROR(st) && st != EFI_NOT_READY) {
        net_op_close(&g_conn.scan_op);
        g_conn.state = CONN_IDLE;
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("WMC2 GetNetworks: 0x%x"), (unsigned)st);
    }
    g_conn.state = CONN_SCANNING;
    g_conn.phase_start = mp_hal_ticks_ms();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(wlan_connect_obj, 1, wlan_connect);

static mp_obj_t wlan_disconnect(mp_obj_t self_in) {
    network_wlan_obj_t *self = MP_OBJ_TO_PTR(self_in);
    EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL *wmc = open_wmc2(self->handle);
    if (wmc == NULL) {
        mp_raise_OSError(MP_ENODEV);
    }
    net_op_t op;
    op.event = NULL;
    if (!EFI_ERROR(net_op_init(&op))) {
        EFI_80211_DISCONNECT_NETWORK_TOKEN tok;
        memset(&tok, 0, sizeof(tok));
        tok.Event = op.event;
        EFI_STATUS st = wmc->DisconnectNetwork(wmc, &tok);
        if (!EFI_ERROR(st) || st == EFI_NOT_READY) {
            net_op_wait(&op, 5000);
        }
        net_op_close(&op);
    }
    wlan_cleanup_pending();
    g_conn.state = CONN_IDLE;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(wlan_disconnect_obj, wlan_disconnect);

static mp_obj_t wlan_isconnected(mp_obj_t self_in) {
    network_wlan_obj_t *self = MP_OBJ_TO_PTR(self_in);
    wlan_poll();
    return mp_obj_new_bool(iface_has_ip(self->handle));
}
static MP_DEFINE_CONST_FUN_OBJ_1(wlan_isconnected_obj, wlan_isconnected);

// status() -> STAT_*; status('rssi') -> link quality (0..100, not dBm — WiFi2 gives
// no dBm RSSI).
static mp_obj_t wlan_status(size_t n_args, const mp_obj_t *args) {
    network_wlan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    wlan_poll();
    if (n_args > 1) {
        const char *p = mp_obj_str_get_str(args[1]);
        if (strcmp(p, "rssi") == 0) {
            return MP_OBJ_NEW_SMALL_INT(g_conn.quality);
        }
        mp_raise_ValueError(MP_ERROR_TEXT("unknown status param"));
    }
    if (iface_has_ip(self->handle)) {
        return MP_OBJ_NEW_SMALL_INT(WLAN_STAT_GOT_IP);
    }
    switch (g_conn.state) {
        case CONN_SCANNING:
        case CONN_CONNECTING:
        case CONN_ASSOCIATED:   // associated, still waiting on a DHCP lease
            return MP_OBJ_NEW_SMALL_INT(WLAN_STAT_CONNECTING);
        case CONN_FAILED:
            return MP_OBJ_NEW_SMALL_INT(g_conn.fail_stat);
        default:
            return MP_OBJ_NEW_SMALL_INT(WLAN_STAT_IDLE);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wlan_status_obj, 1, 2, wlan_status);

// active([up]) — for WiFi, bind the IP stack (ConnectController) so Ip4Config2/DHCP
// exist, but DO NOT touch the SNP (no Start/Initialize): the WiFi2 driver owns the
// radio, and initializing the SNP underneath it breaks association. Query returns
// whether the L3 stack is bound.
static mp_obj_t wlan_active(size_t n_args, const mp_obj_t *args) {
    network_wlan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args > 1) {
        if (mp_obj_is_true(args[1])) {
            BS->ConnectController(self->handle, NULL, NULL, 1 /* recursive */);
        } else {
            BS->DisconnectController(self->handle, NULL, NULL);
        }
    }
    return mp_obj_new_bool(open_ip4cfg2(self->handle) != NULL);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wlan_active_obj, 1, 2, wlan_active);

// config('mac'|'ssid'|'channel'|'hostname') query; config(hostname=...) set.
static mp_obj_t wlan_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_wlan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
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
        if (strcmp(p, "ssid") == 0) {
            return mp_obj_new_bytes(g_conn.ssid, g_conn.ssid_len);
        }
        if (strcmp(p, "channel") == 0) {
            return MP_OBJ_NEW_SMALL_INT(0);   // not exposed by WiFi2
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
static MP_DEFINE_CONST_FUN_OBJ_KW(wlan_config_obj, 1, wlan_config);

static const mp_rom_map_elem_t wlan_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&wlan_active_obj) },         // binds stack, not SNP
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&wlan_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&wlan_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&wlan_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&wlan_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&wlan_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&wlan_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipconfig), MP_ROM_PTR(&lan_ipconfig_obj) },      // shared L3
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&lan_ifconfig_obj) },      // shared L3
    { MP_ROM_QSTR(MP_QSTR_IF_STA), MP_ROM_INT(WLAN_IF_STA) },
    { MP_ROM_QSTR(MP_QSTR_IF_AP), MP_ROM_INT(WLAN_IF_AP) },
    { MP_ROM_QSTR(MP_QSTR_STAT_IDLE), MP_ROM_INT(WLAN_STAT_IDLE) },
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECTING), MP_ROM_INT(WLAN_STAT_CONNECTING) },
    { MP_ROM_QSTR(MP_QSTR_STAT_GOT_IP), MP_ROM_INT(WLAN_STAT_GOT_IP) },
    { MP_ROM_QSTR(MP_QSTR_STAT_NO_AP_FOUND), MP_ROM_INT(WLAN_STAT_NO_AP_FOUND) },
    { MP_ROM_QSTR(MP_QSTR_STAT_WRONG_PASSWORD), MP_ROM_INT(WLAN_STAT_WRONG_PASSWORD) },
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECT_FAIL), MP_ROM_INT(WLAN_STAT_CONNECT_FAIL) },
};
static MP_DEFINE_CONST_DICT(wlan_locals_dict, wlan_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    network_wlan_type,
    MP_QSTR_WLAN,
    MP_TYPE_FLAG_NONE,
    make_new, wlan_make_new,
    locals_dict, &wlan_locals_dict
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
    { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&network_wlan_type) },
    // Connection-status codes (the network docs describe these as module constants).
    { MP_ROM_QSTR(MP_QSTR_STAT_IDLE), MP_ROM_INT(WLAN_STAT_IDLE) },
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECTING), MP_ROM_INT(WLAN_STAT_CONNECTING) },
    { MP_ROM_QSTR(MP_QSTR_STAT_GOT_IP), MP_ROM_INT(WLAN_STAT_GOT_IP) },
    { MP_ROM_QSTR(MP_QSTR_STAT_NO_AP_FOUND), MP_ROM_INT(WLAN_STAT_NO_AP_FOUND) },
    { MP_ROM_QSTR(MP_QSTR_STAT_WRONG_PASSWORD), MP_ROM_INT(WLAN_STAT_WRONG_PASSWORD) },
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECT_FAIL), MP_ROM_INT(WLAN_STAT_CONNECT_FAIL) },
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
