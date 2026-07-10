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

// The `socket` module for the UEFI port: address constants, getaddrinfo (numeric
// passthrough + DNS via EFI_DNS4), and the socket stream type over EFI_UDP4/EFI_TCP4.

#include <string.h>

#include "py/runtime.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "uefi_port.h"
#include "efi.h"
#include "efi_net.h"

#define BS (mp_uefi_st->BootServices)

// Address family / socket type / protocol constants (BSD values, as MicroPython uses).
#define MOD_AF_INET 2
#define MOD_AF_INET6 10
#define MOD_SOCK_STREAM 1
#define MOD_SOCK_DGRAM 2
#define MOD_SOCK_RAW 3
#define MOD_IPPROTO_IP 0
#define MOD_IPPROTO_ICMP 1
#define MOD_IPPROTO_TCP 6
#define MOD_IPPROTO_UDP 17
#define MOD_SOL_SOCKET 1
#define MOD_SO_REUSEADDR 4

static const EFI_GUID g_dns4_sb = EFI_DNS4_SERVICE_BINDING_PROTOCOL_GUID;
static const EFI_GUID g_dns4 = EFI_DNS4_PROTOCOL_GUID;
static const EFI_GUID g_ip4cfg2 = EFI_IP4_CONFIG2_PROTOCOL_GUID;
static const EFI_GUID g_udp4_sb = EFI_UDP4_SERVICE_BINDING_PROTOCOL_GUID;
static const EFI_GUID g_udp4 = EFI_UDP4_PROTOCOL_GUID;
static const EFI_GUID g_tcp4_sb = EFI_TCP4_SERVICE_BINDING_PROTOCOL_GUID;
static const EFI_GUID g_tcp4 = EFI_TCP4_PROTOCOL_GUID;
static const EFI_GUID g_ip4_sb = EFI_IP4_SERVICE_BINDING_PROTOCOL_GUID;
static const EFI_GUID g_ip4 = EFI_IP4_PROTOCOL_GUID;

// ── IPv4 text helpers ────────────────────────────────────────────────────────
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

// Parse a dotted-quad "a.b.c.d" (no prefix). Returns true and fills out[4] if the
// whole string is a numeric IPv4 literal; false if it looks like a hostname.
static bool parse_numeric_ipv4(const char *s, size_t len, UINT8 out[4]) {
    unsigned oct = 0, ndig = 0, idx = 0;
    for (size_t i = 0; i < len; i++) {
        char c = s[i];
        if (c >= '0' && c <= '9') {
            oct = oct * 10 + (unsigned)(c - '0');
            ndig++;
            if (oct > 255 || ndig > 3) {
                return false;
            }
        } else if (c == '.') {
            if (ndig == 0 || idx >= 3) {
                return false;
            }
            out[idx++] = (UINT8)oct;
            oct = 0;
            ndig = 0;
        } else {
            return false;   // a non-numeric character -> a hostname
        }
    }
    if (ndig == 0 || idx != 3) {
        return false;
    }
    out[idx] = (UINT8)oct;
    return true;
}

static void utf8_to_utf16(const char *s, size_t len, CHAR16 *out, size_t cap) {
    size_t n = 0;
    for (size_t i = 0; i < len && n + 1 < cap; i++) {
        out[n++] = (CHAR16)(uint8_t)s[i];
    }
    out[n] = 0;
}

// Build one getaddrinfo 5-tuple: (family, type, proto, canonname, (ip, port)).
static mp_obj_t addrinfo_entry(int family, int type, int proto, mp_obj_t ipstr, mp_int_t port) {
    mp_obj_t sockaddr[2] = { ipstr, mp_obj_new_int(port) };
    mp_obj_t e[5] = {
        MP_OBJ_NEW_SMALL_INT(family),
        MP_OBJ_NEW_SMALL_INT(type),
        MP_OBJ_NEW_SMALL_INT(proto),
        MP_OBJ_NEW_QSTR(MP_QSTR_),      // canonname (empty)
        mp_obj_new_tuple(2, sockaddr),
    };
    return mp_obj_new_tuple(5, e);
}

// ── DNS resolution via EFI_DNS4 ──────────────────────────────────────────────
static mp_obj_t dns_resolve(const char *host, size_t hlen, mp_int_t port, int type, int proto) {
    UINTN count = 0;
    EFI_HANDLE *hb = NULL;
    if (EFI_ERROR(BS->LocateHandleBuffer(ByProtocol, (EFI_GUID *)&g_dns4_sb, NULL, &count, &hb)) ||
        hb == NULL || count == 0) {
        mp_raise_OSError(MP_ENODEV);    // no DNS-capable interface (stack not up?)
    }
    EFI_HANDLE nic = hb[0];
    BS->FreePool(hb);

    EFI_HANDLE child = NULL;
    if (EFI_ERROR(net_sb_create_child(nic, &g_dns4_sb, &child))) {
        mp_raise_OSError(MP_EIO);
    }

    net_op_t op;
    op.event = NULL;
    mp_obj_t result = MP_OBJ_NULL;
    EFI_DNS4_PROTOCOL *dns = NULL;

    // Pull the station IP (+ mask) and the DNS server(s) to use. A global override
    // (network.ipconfig(dns=...)) wins; otherwise take the DHCP-assigned resolver
    // from IP4Config2. Configure DNS4 explicitly (more robust than UseDefaultSetting).
    EFI_IPv4_ADDRESS station, mask, dnssrv[4];
    memset(&station, 0, sizeof(station));
    memset(&mask, 0, sizeof(mask));
    UINTN dnscount = 0;
    EFI_IP4_CONFIG2_PROTOCOL *cfg4 = NULL;
    if (!EFI_ERROR(BS->HandleProtocol(nic, (EFI_GUID *)&g_ip4cfg2, (void **)&cfg4))) {
        UINTN sz = sizeof(dnssrv);
        if (!EFI_ERROR(cfg4->GetData(cfg4, Ip4Config2DataTypeDnsServer, &sz, dnssrv))) {
            dnscount = sz / sizeof(EFI_IPv4_ADDRESS);
        }
        uint8_t ibuf[512];
        UINTN isz = sizeof(ibuf);
        if (!EFI_ERROR(cfg4->GetData(cfg4, Ip4Config2DataTypeInterfaceInfo, &isz, ibuf))) {
            EFI_IP4_CONFIG2_INTERFACE_INFO *ii = (EFI_IP4_CONFIG2_INTERFACE_INFO *)ibuf;
            station = ii->StationAddress;
            mask = ii->SubnetMask;
        }
    }
    UINT8 ov[4];
    if (net_get_dns_override(ov)) {
        memcpy(dnssrv[0].Addr, ov, 4);
        dnscount = 1;
    }

    if (!EFI_ERROR(BS->HandleProtocol(child, (EFI_GUID *)&g_dns4, (void **)&dns))) {
        EFI_DNS4_CONFIG_DATA cfg;
        memset(&cfg, 0, sizeof(cfg));
        cfg.DnsServerListCount = dnscount;
        cfg.DnsServerList = dnscount ? dnssrv : NULL;
        cfg.UseDefaultSetting = dnscount ? 0 : 1;
        cfg.EnableDnsCache = 1;
        cfg.Protocol = 17;              // UDP
        cfg.StationIp = station;
        cfg.SubnetMask = mask;
        cfg.RetryCount = 3;
        if (!EFI_ERROR(dns->Configure(dns, &cfg)) && !EFI_ERROR(net_op_init(&op))) {
            EFI_DNS4_COMPLETION_TOKEN tok;
            memset(&tok, 0, sizeof(tok));
            tok.Event = op.event;
            CHAR16 hostw[256];
            utf8_to_utf16(host, hlen, hostw, MP_ARRAY_SIZE(hostw));
            if (!EFI_ERROR(dns->HostNameToIp(dns, hostw, &tok)) &&
                net_op_wait(&op, 10000) == 0 && !EFI_ERROR(tok.Status) && tok.RspData != NULL) {
                EFI_DNS4_HOST_TO_ADDR_DATA *h = tok.RspData;
                mp_obj_t lst = mp_obj_new_list(0, NULL);
                for (UINT32 i = 0; i < h->IpCount; i++) {
                    mp_obj_list_append(lst, addrinfo_entry(MOD_AF_INET, type, proto,
                        ipv4_to_str(h->IpList[i].Addr), port));
                }
                result = lst;
                if (h->IpList != NULL) {
                    BS->FreePool(h->IpList);
                }
                BS->FreePool(h);
            }
        }
    }

    net_op_close(&op);
    net_sb_destroy_child(nic, &g_dns4_sb, child);
    if (result == MP_OBJ_NULL) {
        mp_raise_OSError(MP_EHOSTUNREACH);   // resolution failed / timed out
    }
    return result;
}

// getaddrinfo(host, port, af=0, type=0, proto=0, flags=0) -> [(fam, type, proto, canon, (ip, port)), ...]
static mp_obj_t socket_getaddrinfo(size_t n_args, const mp_obj_t *args) {
    size_t hlen;
    const char *host = mp_obj_str_get_data(args[0], &hlen);
    mp_int_t port = mp_obj_get_int(args[1]);
    int type = (n_args > 3 && mp_obj_get_int(args[3])) ? (int)mp_obj_get_int(args[3]) : MOD_SOCK_STREAM;
    int proto = (n_args > 4) ? (int)mp_obj_get_int(args[4]) : 0;

    UINT8 ip[4];
    if (parse_numeric_ipv4(host, hlen, ip)) {
        mp_obj_t entry = addrinfo_entry(MOD_AF_INET, type, proto, ipv4_to_str(ip), port);
        mp_obj_t lst = mp_obj_new_list(1, &entry);
        return lst;
    }
    return dns_resolve(host, hlen, port, type, proto);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_getaddrinfo_obj, 2, 6, socket_getaddrinfo);

// ── socket type ──────────────────────────────────────────────────────────────
// UDP (SOCK_DGRAM) over EFI_UDP4 and TCP (SOCK_STREAM) over EFI_TCP4. Blocking /
// settimeout semantics ride net_op_wait (Ctrl-C stays live).
typedef struct _socket_obj_t {
    mp_obj_base_t base;
    uint8_t type;                 // MOD_SOCK_*
    bool closed;
    bool connected;
    int32_t timeout_ms;           // -1 = block forever, 0 = non-blocking, >0 = ms
    EFI_HANDLE nic;
    EFI_HANDLE child;             // UDP4 / TCP4 / IP4 child handle
    EFI_UDP4_PROTOCOL *udp;
    EFI_TCP4_PROTOCOL *tcp;
    EFI_IP4_PROTOCOL *ip4;        // SOCK_RAW (raw IP / ICMP)
    uint8_t proto;                // IP protocol number for SOCK_RAW (e.g. IPPROTO_ICMP)
    uint16_t local_port;          // bind() port (for a TCP listener)
    bool listening;               // listen() was called: this is a passive socket
    UINT8 peer_ip[4];
    uint16_t peer_port;
    // Persistent TCP accept, for select/asyncio readiness on a listener: one Accept
    // is kept outstanding; its notify wakes the idle loop, and poll reports readable
    // when an incoming connection is pending (accept() then harvests it, no blocking).
    net_op_t acc_op;
    bool acc_armed;               // an Accept is outstanding on acc_op
    EFI_TCP4_LISTEN_TOKEN acc_tok; // must persist while the Accept is outstanding
    // Non-blocking connect in progress (asyncio: connect() raises EINPROGRESS, then
    // poll(WR) completes it). Token must persist while the Connect is outstanding.
    bool connecting;
    net_op_t conn_op;
    EFI_TCP4_CONNECTION_TOKEN conn_tok;
    // Persistent TCP receive, for select/asyncio readiness (MP_STREAM_POLL): one
    // Receive is kept outstanding; its notify wakes the idle loop, and poll reports
    // readable when buffered data (or EOF) is available.
    net_op_t rx_op;
    bool rx_armed;                // a Receive is outstanding on rx_op
    bool rx_eof;                  // peer closed (FIN) or the connection errored
    uint8_t *rx_buf;              // AllocatePool'd receive buffer
    uint32_t rx_cap;             // buffer capacity
    uint32_t rx_len;             // valid bytes after a completed Receive
    uint32_t rx_off;             // consumed offset
    EFI_TCP4_IO_TOKEN rx_tok;     // must persist while the Receive is outstanding
    EFI_TCP4_RECEIVE_DATA rx_data;
} socket_obj_t;

static const mp_obj_type_t socket_type;

static void socket_check_open(socket_obj_t *s) {
    if (s->closed) {
        mp_raise_OSError(MP_EBADF);
    }
}

// Bind + configure the UDP4 child on the first DHCP-configured NIC (ephemeral port).
static void udp_bring_up(socket_obj_t *s, uint16_t station_port) {
    if (s->udp != NULL) {
        return;
    }
    UINTN count = 0;
    EFI_HANDLE *hb = NULL;
    if (EFI_ERROR(BS->LocateHandleBuffer(ByProtocol, (EFI_GUID *)&g_udp4_sb, NULL, &count, &hb)) ||
        hb == NULL || count == 0) {
        mp_raise_OSError(MP_ENODEV);
    }
    s->nic = hb[0];
    BS->FreePool(hb);
    if (EFI_ERROR(net_sb_create_child(s->nic, &g_udp4_sb, &s->child)) ||
        EFI_ERROR(BS->HandleProtocol(s->child, (EFI_GUID *)&g_udp4, (void **)&s->udp))) {
        mp_raise_OSError(MP_EIO);
    }
    EFI_UDP4_CONFIG_DATA cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.UseDefaultAddress = 1;    // use the DHCP-assigned station address
    cfg.StationPort = station_port;
    cfg.TimeToLive = 64;
    if (EFI_ERROR(s->udp->Configure(s->udp, &cfg))) {
        mp_raise_OSError(MP_ENODEV);   // interface not up (no DHCP lease yet?)
    }
}

// Transmit `buf`/`len` to (ip, port); block per the socket timeout. Returns bytes sent.
static mp_uint_t udp_send(socket_obj_t *s, const void *buf, mp_uint_t len,
    const UINT8 ip[4], uint16_t port, int *errcode) {
    udp_bring_up(s, 0);
    EFI_UDP4_SESSION_DATA sess;
    memset(&sess, 0, sizeof(sess));
    memcpy(sess.DestinationAddress.Addr, ip, 4);
    sess.DestinationPort = port;
    EFI_UDP4_TRANSMIT_DATA tx;
    memset(&tx, 0, sizeof(tx));
    tx.UdpSessionData = &sess;
    tx.DataLength = len;
    tx.FragmentCount = 1;
    tx.FragmentTable[0].FragmentLength = len;
    tx.FragmentTable[0].FragmentBuffer = (void *)buf;

    net_op_t op;
    op.event = NULL;
    if (EFI_ERROR(net_op_init(&op))) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }
    EFI_UDP4_COMPLETION_TOKEN tok;
    memset(&tok, 0, sizeof(tok));
    tok.Event = op.event;
    tok.Packet.TxData = &tx;
    mp_uint_t ret = MP_STREAM_ERROR;
    EFI_STATUS st = s->udp->Transmit(s->udp, &tok);
    if (EFI_ERROR(st)) {
        *errcode = MP_EIO;
    } else if (net_op_wait(&op, s->timeout_ms) != 0) {
        s->udp->Cancel(s->udp, &tok);
        *errcode = s->timeout_ms == 0 ? MP_EAGAIN : MP_ETIMEDOUT;
    } else if (EFI_ERROR(tok.Status)) {
        *errcode = MP_EIO;
    } else {
        ret = len;
    }
    net_op_close(&op);
    return ret;
}

// Receive one datagram (up to `len`). On success returns bytes read and, if `from`
// is non-NULL, fills the source ip/port. Blocks per the socket timeout.
static mp_uint_t udp_recv(socket_obj_t *s, void *buf, mp_uint_t len,
    UINT8 from_ip[4], uint16_t *from_port, int *errcode) {
    udp_bring_up(s, 0);
    net_op_t op;
    op.event = NULL;
    if (EFI_ERROR(net_op_init(&op))) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }
    EFI_UDP4_COMPLETION_TOKEN tok;
    memset(&tok, 0, sizeof(tok));
    tok.Event = op.event;
    mp_uint_t ret = MP_STREAM_ERROR;
    EFI_STATUS st = s->udp->Receive(s->udp, &tok);
    if (EFI_ERROR(st)) {
        *errcode = MP_EIO;
    } else if (net_op_wait(&op, s->timeout_ms) != 0) {
        s->udp->Cancel(s->udp, &tok);
        *errcode = s->timeout_ms == 0 ? MP_EAGAIN : MP_ETIMEDOUT;
    } else if (EFI_ERROR(tok.Status) || tok.Packet.RxData == NULL) {
        *errcode = MP_EIO;
    } else {
        EFI_UDP4_RECEIVE_DATA *rx = tok.Packet.RxData;
        mp_uint_t total = rx->DataLength < len ? rx->DataLength : len;
        mp_uint_t off = 0;
        for (UINT32 f = 0; f < rx->FragmentCount && off < total; f++) {
            mp_uint_t flen = rx->FragmentTable[f].FragmentLength;
            mp_uint_t take = flen < (total - off) ? flen : (total - off);
            memcpy((uint8_t *)buf + off, rx->FragmentTable[f].FragmentBuffer, take);
            off += take;
        }
        if (from_ip != NULL) {
            memcpy(from_ip, rx->UdpSession.SourceAddress.Addr, 4);
            *from_port = rx->UdpSession.SourcePort;
        }
        if (rx->RecycleSignal != NULL) {
            BS->SignalEvent(rx->RecycleSignal);   // release the firmware buffer
        }
        ret = off;
    }
    net_op_close(&op);
    return ret;
}

// ── Raw IP (SOCK_RAW) over EFI_IP4 — e.g. ICMP echo/ping in Python ────────────
static void ip4_bring_up(socket_obj_t *s) {
    if (s->ip4 != NULL) {
        return;
    }
    UINTN count = 0;
    EFI_HANDLE *hb = NULL;
    if (EFI_ERROR(BS->LocateHandleBuffer(ByProtocol, (EFI_GUID *)&g_ip4_sb, NULL, &count, &hb)) ||
        hb == NULL || count == 0) {
        mp_raise_OSError(MP_ENODEV);
    }
    s->nic = hb[0];
    BS->FreePool(hb);
    if (EFI_ERROR(net_sb_create_child(s->nic, &g_ip4_sb, &s->child)) ||
        EFI_ERROR(BS->HandleProtocol(s->child, (EFI_GUID *)&g_ip4, (void **)&s->ip4))) {
        mp_raise_OSError(MP_EIO);
    }
    EFI_IP4_CONFIG_DATA cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.DefaultProtocol = s->proto;   // e.g. 1 = ICMP
    cfg.AcceptIcmpErrors = 1;
    cfg.UseDefaultAddress = 1;
    cfg.TimeToLive = 64;
    if (EFI_ERROR(s->ip4->Configure(s->ip4, &cfg))) {
        mp_raise_OSError(MP_ENODEV);
    }
}

static mp_uint_t ip4_send(socket_obj_t *s, const void *buf, mp_uint_t len, const UINT8 ip[4], int *errcode) {
    ip4_bring_up(s);
    EFI_IP4_TRANSMIT_DATA tx;
    memset(&tx, 0, sizeof(tx));
    memcpy(tx.DestinationAddress.Addr, ip, 4);
    tx.TotalDataLength = len;
    tx.FragmentCount = 1;
    tx.FragmentTable[0].FragmentLength = len;
    tx.FragmentTable[0].FragmentBuffer = (void *)buf;
    net_op_t op;
    op.event = NULL;
    if (EFI_ERROR(net_op_init(&op))) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }
    EFI_IP4_COMPLETION_TOKEN tok;
    memset(&tok, 0, sizeof(tok));
    tok.Event = op.event;
    tok.Packet.TxData = &tx;
    int to = s->timeout_ms == 0 ? 10000 : s->timeout_ms;
    mp_uint_t ret = MP_STREAM_ERROR;
    EFI_STATUS st = s->ip4->Transmit(s->ip4, &tok);
    if (EFI_ERROR(st)) {
        *errcode = MP_EIO;
    } else if (net_op_wait(&op, to) != 0) {
        s->ip4->Cancel(s->ip4, &tok);
        *errcode = MP_ETIMEDOUT;
    } else if (EFI_ERROR(tok.Status)) {
        *errcode = MP_EIO;
    } else {
        ret = len;
    }
    net_op_close(&op);
    return ret;
}

static mp_uint_t ip4_recv(socket_obj_t *s, void *buf, mp_uint_t len, UINT8 from_ip[4], int *errcode) {
    ip4_bring_up(s);
    net_op_t op;
    op.event = NULL;
    if (EFI_ERROR(net_op_init(&op))) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }
    EFI_IP4_COMPLETION_TOKEN tok;
    memset(&tok, 0, sizeof(tok));
    tok.Event = op.event;
    mp_uint_t ret = MP_STREAM_ERROR;
    EFI_STATUS st = s->ip4->Receive(s->ip4, &tok);
    if (EFI_ERROR(st)) {
        *errcode = MP_EIO;
    } else if (net_op_wait(&op, s->timeout_ms) != 0) {
        s->ip4->Cancel(s->ip4, &tok);
        *errcode = s->timeout_ms == 0 ? MP_EAGAIN : MP_ETIMEDOUT;
    } else if (EFI_ERROR(tok.Status) || tok.Packet.RxData == NULL) {
        *errcode = MP_EIO;
    } else {
        EFI_IP4_RECEIVE_DATA *rx = tok.Packet.RxData;
        mp_uint_t total = rx->DataLength < len ? rx->DataLength : len;
        mp_uint_t off = 0;
        for (UINT32 f = 0; f < rx->FragmentCount && off < total; f++) {
            mp_uint_t flen = rx->FragmentTable[f].FragmentLength;
            mp_uint_t take = flen < (total - off) ? flen : (total - off);
            memcpy((uint8_t *)buf + off, rx->FragmentTable[f].FragmentBuffer, take);
            off += take;
        }
        if (from_ip != NULL) {
            // The IPv4 header's source address is at byte offset 12.
            const uint8_t *hdr = (const uint8_t *)rx->Header;
            if (hdr != NULL && rx->HeaderLength >= 16) {
                memcpy(from_ip, hdr + 12, 4);
            } else {
                memset(from_ip, 0, 4);
            }
        }
        if (rx->RecycleSignal != NULL) {
            BS->SignalEvent(rx->RecycleSignal);
        }
        ret = off;
    }
    net_op_close(&op);
    return ret;
}

// ── TCP (SOCK_STREAM) over EFI_TCP4 ──────────────────────────────────────────
static void tcp_post_recv(socket_obj_t *s);
static void tcp_post_accept(socket_obj_t *s);
static void tcp_finish_connect(socket_obj_t *s);

// Service-bind a TCP4 child, configure an active (client) open, and complete the
// connection handshake. Blocks per the socket timeout.
static void tcp_connect(socket_obj_t *s, const UINT8 ip[4], uint16_t port) {
    UINTN count = 0;
    EFI_HANDLE *hb = NULL;
    if (EFI_ERROR(BS->LocateHandleBuffer(ByProtocol, (EFI_GUID *)&g_tcp4_sb, NULL, &count, &hb)) ||
        hb == NULL || count == 0) {
        mp_raise_OSError(MP_ENODEV);
    }
    s->nic = hb[0];
    BS->FreePool(hb);
    if (EFI_ERROR(net_sb_create_child(s->nic, &g_tcp4_sb, &s->child)) ||
        EFI_ERROR(BS->HandleProtocol(s->child, (EFI_GUID *)&g_tcp4, (void **)&s->tcp))) {
        mp_raise_OSError(MP_EIO);
    }
    EFI_TCP4_CONFIG_DATA cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.TimeToLive = 64;
    cfg.AccessPoint.UseDefaultAddress = 1;    // DHCP-assigned station address
    cfg.AccessPoint.ActiveFlag = 1;           // client
    memcpy(cfg.AccessPoint.RemoteAddress.Addr, ip, 4);
    cfg.AccessPoint.RemotePort = port;
    if (EFI_ERROR(s->tcp->Configure(s->tcp, &cfg))) {
        mp_raise_OSError(MP_ENODEV);          // no bound address yet (interface down?)
    }
    if (EFI_ERROR(net_op_init(&s->conn_op))) {
        mp_raise_OSError(MP_EIO);
    }
    memset(&s->conn_tok, 0, sizeof(s->conn_tok));
    s->conn_tok.CompletionToken.Event = s->conn_op.event;
    EFI_STATUS st = s->tcp->Connect(s->tcp, &s->conn_tok);
    if (EFI_ERROR(st)) {
        net_op_close(&s->conn_op);
        mp_raise_OSError(MP_EHOSTUNREACH);
    }
    if (s->timeout_ms == 0) {
        // Non-blocking: leave the Connect outstanding; poll(WR) finishes it (asyncio).
        s->connecting = true;
        mp_raise_OSError(MP_EINPROGRESS);
    }
    if (net_op_wait(&s->conn_op, s->timeout_ms) != 0) {
        s->tcp->Cancel(s->tcp, &s->conn_tok.CompletionToken);
        net_op_close(&s->conn_op);
        mp_raise_OSError(MP_ETIMEDOUT);
    }
    EFI_STATUS status = s->conn_tok.CompletionToken.Status;
    net_op_close(&s->conn_op);
    if (EFI_ERROR(status)) {
        mp_raise_OSError(status == EFI_CONNECTION_REFUSED ? MP_ECONNREFUSED : MP_EHOSTUNREACH);
    }
    tcp_finish_connect(s);
}

static mp_uint_t tcp_send(socket_obj_t *s, const void *buf, mp_uint_t len, int *errcode) {
    if (s->tcp == NULL) {
        *errcode = MP_ENOTCONN;
        return MP_STREAM_ERROR;
    }
    EFI_TCP4_TRANSMIT_DATA tx;
    memset(&tx, 0, sizeof(tx));
    tx.Push = 1;
    tx.DataLength = len;
    tx.FragmentCount = 1;
    tx.FragmentTable[0].FragmentLength = len;
    tx.FragmentTable[0].FragmentBuffer = (void *)buf;
    net_op_t op;
    op.event = NULL;
    if (EFI_ERROR(net_op_init(&op))) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }
    EFI_TCP4_IO_TOKEN tok;
    memset(&tok, 0, sizeof(tok));
    tok.CompletionToken.Event = op.event;
    tok.Packet.TxData = &tx;
    // A TCP Transmit completes when the data is accepted into the send buffer (not
    // when ACKed) — fast for normal writes. So we block on it even on a non-blocking
    // socket (using a generous bound), rather than returning EAGAIN and dropping it.
    int send_to = s->timeout_ms == 0 ? 10000 : s->timeout_ms;
    mp_uint_t ret = MP_STREAM_ERROR;
    EFI_STATUS st = s->tcp->Transmit(s->tcp, &tok);
    if (EFI_ERROR(st)) {
        *errcode = MP_EIO;
    } else if (net_op_wait(&op, send_to) != 0) {
        s->tcp->Cancel(s->tcp, &tok.CompletionToken);
        *errcode = MP_ETIMEDOUT;
    } else if (EFI_ERROR(tok.CompletionToken.Status)) {
        *errcode = MP_ECONNRESET;
    } else {
        ret = len;
    }
    net_op_close(&op);
    return ret;
}

// Finalise a completed connection: allocate the rx buffer and arm the persistent
// Receive. Used by both the blocking connect and the poll(WR)-completed async path.
static void tcp_finish_connect(socket_obj_t *s) {
    s->connected = true;
    if (EFI_ERROR(BS->AllocatePool(EfiBootServicesData, s->rx_cap, (void **)&s->rx_buf))) {
        s->rx_buf = NULL;
    }
    tcp_post_recv(s);
}

// Keep exactly one Receive outstanding so poll() can report readability and its
// notify wakes the idle loop. Fresh completion op per Receive (avoids event reuse).
static void tcp_post_recv(socket_obj_t *s) {
    if (s->rx_eof || s->rx_armed || s->tcp == NULL || s->rx_buf == NULL) {
        return;
    }
    net_op_close(&s->rx_op);
    if (EFI_ERROR(net_op_init(&s->rx_op))) {
        return;
    }
    memset(&s->rx_data, 0, sizeof(s->rx_data));
    s->rx_data.DataLength = s->rx_cap;
    s->rx_data.FragmentCount = 1;
    s->rx_data.FragmentTable[0].FragmentLength = s->rx_cap;
    s->rx_data.FragmentTable[0].FragmentBuffer = s->rx_buf;
    memset(&s->rx_tok, 0, sizeof(s->rx_tok));
    s->rx_tok.CompletionToken.Event = s->rx_op.event;
    s->rx_tok.Packet.RxData = &s->rx_data;
    EFI_STATUS st = s->tcp->Receive(s->tcp, &s->rx_tok);
    if (st == EFI_CONNECTION_FIN) {
        s->rx_eof = true;
    } else if (!EFI_ERROR(st)) {
        s->rx_armed = true;
    }
}

// Non-blocking: if the outstanding Receive has completed, absorb its result into
// the socket's rx buffer (or mark EOF). Does not block.
static void tcp_harvest(socket_obj_t *s) {
    if (!s->rx_armed || !s->rx_op.done) {
        return;
    }
    s->rx_armed = false;
    EFI_STATUS status = s->rx_tok.CompletionToken.Status;
    if (status == EFI_CONNECTION_FIN || EFI_ERROR(status)) {
        s->rx_eof = true;                 // FIN, RESET, or error -> end of stream
        s->rx_len = s->rx_off = 0;
    } else {
        s->rx_len = s->rx_data.DataLength;
        s->rx_off = 0;
    }
}

static mp_uint_t tcp_recv(socket_obj_t *s, void *buf, mp_uint_t len, int *errcode) {
    if (s->tcp == NULL) {
        *errcode = MP_ENOTCONN;
        return MP_STREAM_ERROR;
    }
    for (;;) {
        tcp_harvest(s);
        if (s->rx_off < s->rx_len) {                  // buffered data available
            mp_uint_t avail = s->rx_len - s->rx_off;
            mp_uint_t take = avail < len ? avail : len;
            memcpy(buf, s->rx_buf + s->rx_off, take);
            s->rx_off += take;
            if (s->rx_off >= s->rx_len) {
                tcp_post_recv(s);                     // consumed: re-arm for more
            }
            return take;
        }
        if (s->rx_eof) {
            return 0;                                 // EOF -> empty recv
        }
        if (!s->rx_armed) {
            tcp_post_recv(s);
            if (s->rx_eof) {
                return 0;
            }
            if (!s->rx_armed) {
                *errcode = MP_EIO;                    // couldn't post a Receive
                return MP_STREAM_ERROR;
            }
        }
        if (s->timeout_ms == 0) {
            *errcode = MP_EAGAIN;                     // non-blocking, nothing ready
            return MP_STREAM_ERROR;
        }
        // Block until the outstanding Receive completes (leaves it posted on
        // timeout so the next call/poll can pick it up).
        if (net_op_wait(&s->rx_op, s->timeout_ms) != 0) {
            *errcode = MP_ETIMEDOUT;
            return MP_STREAM_ERROR;
        }
    }
}

// Dispatch send/recv by socket type (UDP to the connected peer, or TCP stream).
static mp_uint_t sock_write(socket_obj_t *s, const void *buf, mp_uint_t len, int *errcode) {
    if (s->type == MOD_SOCK_STREAM) {
        return tcp_send(s, buf, len, errcode);
    }
    if (!s->connected) {
        *errcode = MP_ENOTCONN;
        return MP_STREAM_ERROR;
    }
    if (s->type == MOD_SOCK_RAW) {
        return ip4_send(s, buf, len, s->peer_ip, errcode);
    }
    return udp_send(s, buf, len, s->peer_ip, s->peer_port, errcode);
}

static mp_uint_t sock_read(socket_obj_t *s, void *buf, mp_uint_t len, int *errcode) {
    if (s->type == MOD_SOCK_STREAM) {
        return tcp_recv(s, buf, len, errcode);
    }
    if (s->type == MOD_SOCK_RAW) {
        return ip4_recv(s, buf, len, NULL, errcode);
    }
    return udp_recv(s, buf, len, NULL, NULL, errcode);
}

// Parse a (ip_str, port) sockaddr into ip[4]/port. The ip must be numeric (names
// are resolved with getaddrinfo first).
static void parse_sockaddr(mp_obj_t addr, UINT8 ip[4], uint16_t *port) {
    mp_obj_t *items;
    mp_obj_get_array_fixed_n(addr, 2, &items);
    size_t l;
    const char *s = mp_obj_str_get_data(items[0], &l);
    if (!parse_numeric_ipv4(s, l, ip)) {
        mp_raise_ValueError(MP_ERROR_TEXT("address must be a numeric IP (resolve with getaddrinfo)"));
    }
    *port = (uint16_t)mp_obj_get_int(items[1]);
}

static mp_obj_t socket_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 3, false);
    int af = n_args > 0 ? mp_obj_get_int(args[0]) : MOD_AF_INET;
    int stype = n_args > 1 ? mp_obj_get_int(args[1]) : MOD_SOCK_STREAM;
    // Unsupported family/type are reported as OSError (like a real socket() syscall:
    // EAFNOSUPPORT / EINVAL), not NotImplementedError — CPython and the mainline ports
    // raise OSError here, and callers (e.g. socket_badconstructor) catch it as such.
    if (af != MOD_AF_INET) {
        mp_raise_OSError(MP_EAFNOSUPPORT);
    }
    if (stype != MOD_SOCK_DGRAM && stype != MOD_SOCK_STREAM && stype != MOD_SOCK_RAW) {
        mp_raise_OSError(MP_EINVAL);
    }
    socket_obj_t *s = mp_obj_malloc_with_finaliser(socket_obj_t, type);
    s->type = (uint8_t)stype;
    s->proto = (n_args > 2) ? (uint8_t)mp_obj_get_int(args[2]) : 0;
    s->closed = false;
    s->connected = false;
    s->timeout_ms = -1;           // blocking by default
    s->nic = NULL;
    s->child = NULL;
    s->udp = NULL;
    s->tcp = NULL;
    s->ip4 = NULL;
    s->local_port = 0;
    s->peer_port = 0;
    s->listening = false;
    s->connecting = false;
    s->conn_op.event = NULL;
    s->acc_op.event = NULL;
    s->acc_armed = false;
    s->rx_op.event = NULL;
    s->rx_armed = false;
    s->rx_eof = false;
    s->rx_buf = NULL;
    s->rx_cap = 2048;
    s->rx_len = 0;
    s->rx_off = 0;
    return MP_OBJ_FROM_PTR(s);
}

static mp_obj_t socket_close(mp_obj_t self_in) {
    socket_obj_t *s = MP_OBJ_TO_PTR(self_in);
    if (s->closed) {
        return mp_const_none;
    }
    if (s->tcp != NULL) {
        // Cancel any in-flight connect and the persistent Receive so firmware stops
        // writing our buffers before we free them.
        if (s->connecting) {
            s->tcp->Cancel(s->tcp, &s->conn_tok.CompletionToken);
            s->connecting = false;
        }
        net_op_close(&s->conn_op);
        if (s->rx_armed) {
            s->tcp->Cancel(s->tcp, &s->rx_tok.CompletionToken);
            s->rx_armed = false;
        }
        net_op_close(&s->rx_op);
        if (s->acc_armed) {
            s->tcp->Cancel(s->tcp, &s->acc_tok.CompletionToken);
            s->acc_armed = false;
        }
        net_op_close(&s->acc_op);
        // Graceful TCP close: post a Close token and wait briefly, then reset.
        net_op_t op;
        op.event = NULL;
        if (!EFI_ERROR(net_op_init(&op))) {
            EFI_TCP4_CLOSE_TOKEN ct;
            memset(&ct, 0, sizeof(ct));
            ct.CompletionToken.Event = op.event;
            if (!EFI_ERROR(s->tcp->Close(s->tcp, &ct))) {
                net_op_wait(&op, 2000);
            }
            net_op_close(&op);
        }
        s->tcp->Configure(s->tcp, NULL);
        s->tcp = NULL;
        if (s->child != NULL) {
            net_sb_destroy_child(s->nic, &g_tcp4_sb, s->child);
        }
    } else if (s->udp != NULL) {
        s->udp->Configure(s->udp, NULL);
        s->udp = NULL;
        if (s->child != NULL) {
            net_sb_destroy_child(s->nic, &g_udp4_sb, s->child);
        }
    } else if (s->ip4 != NULL) {
        s->ip4->Configure(s->ip4, NULL);
        s->ip4 = NULL;
        if (s->child != NULL) {
            net_sb_destroy_child(s->nic, &g_ip4_sb, s->child);
        }
    }
    if (s->rx_buf != NULL) {
        BS->FreePool(s->rx_buf);
        s->rx_buf = NULL;
    }
    s->child = NULL;
    s->closed = true;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(socket_close_obj, socket_close);

static mp_obj_t socket_bind(mp_obj_t self_in, mp_obj_t addr_in) {
    socket_obj_t *s = MP_OBJ_TO_PTR(self_in);
    socket_check_open(s);
    UINT8 ip[4];
    uint16_t port;
    parse_sockaddr(addr_in, ip, &port);
    s->local_port = port;
    if (s->type == MOD_SOCK_DGRAM) {
        udp_bring_up(s, port);    // UDP: (re)configure with the requested local port
    }
    // TCP: the port is applied in listen(); RAW ignores it.
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(socket_bind_obj, socket_bind);

// listen(): configure the TCP4 child as a passive (server) endpoint on local_port.
static mp_obj_t socket_listen(size_t n_args, const mp_obj_t *args) {
    socket_obj_t *s = MP_OBJ_TO_PTR(args[0]);
    socket_check_open(s);
    if (s->type != MOD_SOCK_STREAM) {
        mp_raise_OSError(MP_EOPNOTSUPP);
    }
    UINTN count = 0;
    EFI_HANDLE *hb = NULL;
    if (EFI_ERROR(BS->LocateHandleBuffer(ByProtocol, (EFI_GUID *)&g_tcp4_sb, NULL, &count, &hb)) ||
        hb == NULL || count == 0) {
        mp_raise_OSError(MP_ENODEV);
    }
    s->nic = hb[0];
    BS->FreePool(hb);
    if (EFI_ERROR(net_sb_create_child(s->nic, &g_tcp4_sb, &s->child)) ||
        EFI_ERROR(BS->HandleProtocol(s->child, (EFI_GUID *)&g_tcp4, (void **)&s->tcp))) {
        mp_raise_OSError(MP_EIO);
    }
    EFI_TCP4_CONFIG_DATA cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.TimeToLive = 64;
    cfg.AccessPoint.UseDefaultAddress = 1;
    cfg.AccessPoint.ActiveFlag = 0;               // passive (listen)
    cfg.AccessPoint.StationPort = s->local_port;
    if (EFI_ERROR(s->tcp->Configure(s->tcp, &cfg))) {
        mp_raise_OSError(MP_EADDRINUSE);
    }
    s->listening = true;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_listen_obj, 1, 2, socket_listen);

// Keep exactly one Accept outstanding so poll() can report an incoming connection
// and its notify wakes the idle loop. Fresh completion op per Accept.
static void tcp_post_accept(socket_obj_t *s) {
    if (s->acc_armed || s->tcp == NULL || !s->listening) {
        return;
    }
    net_op_close(&s->acc_op);
    if (EFI_ERROR(net_op_init(&s->acc_op))) {
        return;
    }
    memset(&s->acc_tok, 0, sizeof(s->acc_tok));
    s->acc_tok.CompletionToken.Event = s->acc_op.event;
    if (!EFI_ERROR(s->tcp->Accept(s->tcp, &s->acc_tok))) {
        s->acc_armed = true;
    }
}

// accept(): return (conn_socket, (ip, port)) for the next incoming connection.
// Blocks per the socket timeout; with timeout 0 (non-blocking / asyncio) it raises
// EAGAIN while no connection is pending, leaving the persistent Accept armed so a
// later poll() sees it. One Accept is always kept outstanding for pollability.
static mp_obj_t socket_accept(mp_obj_t self_in) {
    socket_obj_t *s = MP_OBJ_TO_PTR(self_in);
    socket_check_open(s);
    if (s->type != MOD_SOCK_STREAM || s->tcp == NULL || !s->listening) {
        mp_raise_OSError(MP_EINVAL);
    }
    tcp_post_accept(s);
    if (!s->acc_armed) {
        mp_raise_OSError(MP_EIO);
    }
    if (!s->acc_op.done) {
        if (s->timeout_ms == 0) {
            mp_raise_OSError(MP_EAGAIN);      // leave it armed for the next poll()
        }
        if (net_op_wait(&s->acc_op, s->timeout_ms) != 0) {
            mp_raise_OSError(MP_ETIMEDOUT);   // leave it armed; the Accept lives on
        }
    }
    s->acc_armed = false;
    EFI_STATUS status = s->acc_tok.CompletionToken.Status;
    EFI_HANDLE new_child = s->acc_tok.NewChildHandle;
    net_op_close(&s->acc_op);
    tcp_post_accept(s);                        // re-arm so the listener stays pollable
    if (EFI_ERROR(status) || new_child == NULL) {
        mp_raise_OSError(MP_ECONNABORTED);
    }
    EFI_TCP4_PROTOCOL *ntcp = NULL;
    if (EFI_ERROR(BS->HandleProtocol(new_child, (EFI_GUID *)&g_tcp4, (void **)&ntcp))) {
        net_sb_destroy_child(s->nic, &g_tcp4_sb, new_child);
        mp_raise_OSError(MP_EIO);
    }
    // The peer address is in the accepted child's config data.
    UINT8 peer_ip[4] = {0, 0, 0, 0};
    uint16_t peer_port = 0;
    EFI_TCP4_CONFIG_DATA cd;
    memset(&cd, 0, sizeof(cd));
    if (!EFI_ERROR(ntcp->GetModeData(ntcp, NULL, &cd, NULL, NULL, NULL))) {
        memcpy(peer_ip, cd.AccessPoint.RemoteAddress.Addr, 4);
        peer_port = cd.AccessPoint.RemotePort;
    }
    socket_obj_t *conn = mp_obj_malloc_with_finaliser(socket_obj_t, &socket_type);
    memset(conn, 0, sizeof(*conn));
    conn->base.type = &socket_type;
    conn->type = MOD_SOCK_STREAM;
    conn->timeout_ms = -1;
    conn->nic = s->nic;
    conn->child = new_child;
    conn->tcp = ntcp;
    conn->rx_cap = 2048;
    memcpy(conn->peer_ip, peer_ip, 4);
    conn->peer_port = peer_port;
    tcp_finish_connect(conn);     // alloc rx buffer + arm the persistent Receive
    conn->connected = true;
    mp_obj_t addr[2] = { ipv4_to_str(peer_ip), mp_obj_new_int(peer_port) };
    mp_obj_t ret[2] = { MP_OBJ_FROM_PTR(conn), mp_obj_new_tuple(2, addr) };
    return mp_obj_new_tuple(2, ret);
}
static MP_DEFINE_CONST_FUN_OBJ_1(socket_accept_obj, socket_accept);

static mp_obj_t socket_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    socket_obj_t *s = MP_OBJ_TO_PTR(self_in);
    socket_check_open(s);
    parse_sockaddr(addr_in, s->peer_ip, &s->peer_port);
    if (s->type == MOD_SOCK_STREAM) {
        tcp_connect(s, s->peer_ip, s->peer_port);   // real 3-way handshake
    } else {
        s->connected = true;                         // UDP: just record the default peer
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(socket_connect_obj, socket_connect);

static mp_obj_t socket_sendto(mp_obj_t self_in, mp_obj_t data_in, mp_obj_t addr_in) {
    socket_obj_t *s = MP_OBJ_TO_PTR(self_in);
    socket_check_open(s);
    UINT8 ip[4];
    uint16_t port;
    parse_sockaddr(addr_in, ip, &port);
    mp_buffer_info_t bi;
    mp_get_buffer_raise(data_in, &bi, MP_BUFFER_READ);
    int err;
    mp_uint_t sent = s->type == MOD_SOCK_RAW
        ? ip4_send(s, bi.buf, bi.len, ip, &err)
        : udp_send(s, bi.buf, bi.len, ip, port, &err);
    if (sent == MP_STREAM_ERROR) {
        mp_raise_OSError(err);
    }
    return mp_obj_new_int(sent);
}
static MP_DEFINE_CONST_FUN_OBJ_3(socket_sendto_obj, socket_sendto);

static mp_obj_t socket_send(mp_obj_t self_in, mp_obj_t data_in) {
    socket_obj_t *s = MP_OBJ_TO_PTR(self_in);
    socket_check_open(s);
    mp_buffer_info_t bi;
    mp_get_buffer_raise(data_in, &bi, MP_BUFFER_READ);
    int err;
    mp_uint_t sent = sock_write(s, bi.buf, bi.len, &err);
    if (sent == MP_STREAM_ERROR) {
        mp_raise_OSError(err);
    }
    return mp_obj_new_int(sent);
}
static MP_DEFINE_CONST_FUN_OBJ_2(socket_send_obj, socket_send);

static mp_obj_t socket_recvfrom(mp_obj_t self_in, mp_obj_t len_in) {
    socket_obj_t *s = MP_OBJ_TO_PTR(self_in);
    socket_check_open(s);
    mp_int_t n = mp_obj_get_int(len_in);
    vstr_t vstr;
    vstr_init_len(&vstr, n);
    UINT8 from_ip[4];
    uint16_t from_port = 0;
    int err;
    mp_uint_t got = s->type == MOD_SOCK_RAW
        ? ip4_recv(s, vstr.buf, n, from_ip, &err)
        : udp_recv(s, vstr.buf, n, from_ip, &from_port, &err);
    if (got == MP_STREAM_ERROR) {
        vstr_clear(&vstr);
        mp_raise_OSError(err);
    }
    mp_obj_t data = mp_obj_new_bytes((const byte *)vstr.buf, got);
    vstr_clear(&vstr);
    mp_obj_t src[2] = { ipv4_to_str(from_ip), mp_obj_new_int(from_port) };
    mp_obj_t ret[2] = { data, mp_obj_new_tuple(2, src) };
    return mp_obj_new_tuple(2, ret);
}
static MP_DEFINE_CONST_FUN_OBJ_2(socket_recvfrom_obj, socket_recvfrom);

static mp_obj_t socket_recv(mp_obj_t self_in, mp_obj_t len_in) {
    socket_obj_t *s = MP_OBJ_TO_PTR(self_in);
    socket_check_open(s);
    mp_int_t n = mp_obj_get_int(len_in);
    vstr_t vstr;
    vstr_init_len(&vstr, n);
    int err;
    mp_uint_t got = sock_read(s, vstr.buf, n, &err);
    if (got == MP_STREAM_ERROR) {
        vstr_clear(&vstr);
        mp_raise_OSError(err);
    }
    mp_obj_t data = mp_obj_new_bytes((const byte *)vstr.buf, got);
    vstr_clear(&vstr);
    return data;
}
static MP_DEFINE_CONST_FUN_OBJ_2(socket_recv_obj, socket_recv);

static mp_obj_t socket_settimeout(mp_obj_t self_in, mp_obj_t timeout_in) {
    socket_obj_t *s = MP_OBJ_TO_PTR(self_in);
    if (timeout_in == mp_const_none) {
        s->timeout_ms = -1;
    } else {
        s->timeout_ms = (int32_t)(mp_obj_get_float(timeout_in) * 1000.0f);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(socket_settimeout_obj, socket_settimeout);

static mp_obj_t socket_setblocking(mp_obj_t self_in, mp_obj_t flag_in) {
    socket_obj_t *s = MP_OBJ_TO_PTR(self_in);
    s->timeout_ms = mp_obj_is_true(flag_in) ? -1 : 0;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(socket_setblocking_obj, socket_setblocking);

// setsockopt(level, optname, value): accepted for CPython/asyncio compatibility
// (asyncio.start_server sets SO_REUSEADDR). The EFI TCP4 stack rebinds a released
// station port on its own, so there is nothing to do; other options are ignored.
static mp_obj_t socket_setsockopt(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    (void)args;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_setsockopt_obj, 4, 4, socket_setsockopt);

// Stream protocol: read == recv, write == send-to-connected-peer.
static mp_uint_t socket_stream_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    socket_obj_t *s = MP_OBJ_TO_PTR(self_in);
    if (s->closed) {
        *errcode = MP_EBADF;
        return MP_STREAM_ERROR;
    }
    return sock_read(s, buf, size, errcode);
}

static mp_uint_t socket_stream_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    socket_obj_t *s = MP_OBJ_TO_PTR(self_in);
    if (s->closed) {
        *errcode = MP_EBADF;
        return MP_STREAM_ERROR;
    }
    return sock_write(s, buf, size, errcode);
}

static mp_uint_t socket_stream_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    socket_obj_t *s = MP_OBJ_TO_PTR(self_in);
    if (request == MP_STREAM_CLOSE) {
        socket_close(self_in);
        return 0;
    }
    if (request == MP_STREAM_POLL) {
        // Readiness for select / asyncio. TCP readability is driven by the
        // persistent Receive (its notify wakes the idle loop); we report writable
        // optimistically (send blocks per timeout).
        // A closed socket is an invalid descriptor: report POLLNVAL, as a real
        // poll() would for a dead fd (mainline ports get this from the host poll()).
        if (s->closed) {
            return MP_STREAM_POLL_NVAL;
        }
        mp_uint_t flags = arg;
        mp_uint_t ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && s->type == MOD_SOCK_STREAM && !s->closed && s->listening) {
            // A listener is "readable" when an incoming connection is pending, i.e.
            // the persistent Accept has completed; accept() then harvests it.
            tcp_post_accept(s);             // arm so a connection wakes the loop
            if (s->acc_armed && s->acc_op.done) {
                ret |= MP_STREAM_POLL_RD;
            }
        } else if ((flags & MP_STREAM_POLL_RD) && s->type == MOD_SOCK_STREAM && !s->closed && !s->connecting) {
            tcp_harvest(s);
            if (s->rx_off < s->rx_len || s->rx_eof) {
                ret |= MP_STREAM_POLL_RD;
            } else if (!s->rx_armed) {
                tcp_post_recv(s);            // arm so arriving data wakes the loop
            }
        }
        if (flags & MP_STREAM_POLL_WR) {
            if (s->connecting) {
                // Finish an async connect: writable once the handshake completes.
                if (s->conn_op.done) {
                    s->connecting = false;
                    EFI_STATUS status = s->conn_tok.CompletionToken.Status;
                    net_op_close(&s->conn_op);
                    if (!EFI_ERROR(status)) {
                        tcp_finish_connect(s);
                        ret |= MP_STREAM_POLL_WR;
                    } else {
                        ret |= MP_STREAM_POLL_HUP;   // connect failed
                    }
                }
            } else if (!s->closed) {
                ret |= MP_STREAM_POLL_WR;
            }
        }
        return ret;
    }
    *errcode = MP_EINVAL;
    return MP_STREAM_ERROR;
}

static const mp_rom_map_elem_t socket_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&socket_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&socket_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_bind), MP_ROM_PTR(&socket_bind_obj) },
    { MP_ROM_QSTR(MP_QSTR_listen), MP_ROM_PTR(&socket_listen_obj) },
    { MP_ROM_QSTR(MP_QSTR_accept), MP_ROM_PTR(&socket_accept_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&socket_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&socket_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendto), MP_ROM_PTR(&socket_sendto_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&socket_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_recvfrom), MP_ROM_PTR(&socket_recvfrom_obj) },
    { MP_ROM_QSTR(MP_QSTR_settimeout), MP_ROM_PTR(&socket_settimeout_obj) },
    { MP_ROM_QSTR(MP_QSTR_setblocking), MP_ROM_PTR(&socket_setblocking_obj) },
    { MP_ROM_QSTR(MP_QSTR_setsockopt), MP_ROM_PTR(&socket_setsockopt_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&mp_stream___exit___obj) },
};
static MP_DEFINE_CONST_DICT(socket_locals_dict, socket_locals_dict_table);

static const mp_stream_p_t socket_stream_p = {
    .read = socket_stream_read,
    .write = socket_stream_write,
    .ioctl = socket_stream_ioctl,
};

static MP_DEFINE_CONST_OBJ_TYPE(
    socket_type,
    MP_QSTR_socket,
    MP_TYPE_FLAG_NONE,
    make_new, socket_make_new,
    protocol, &socket_stream_p,
    locals_dict, &socket_locals_dict
    );

// ── Module table ─────────────────────────────────────────────────────────────
static const mp_rom_map_elem_t socket_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_socket) },
    { MP_ROM_QSTR(MP_QSTR_socket), MP_ROM_PTR(&socket_type) },
    { MP_ROM_QSTR(MP_QSTR_getaddrinfo), MP_ROM_PTR(&socket_getaddrinfo_obj) },
    { MP_ROM_QSTR(MP_QSTR_AF_INET), MP_ROM_INT(MOD_AF_INET) },
    { MP_ROM_QSTR(MP_QSTR_AF_INET6), MP_ROM_INT(MOD_AF_INET6) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_STREAM), MP_ROM_INT(MOD_SOCK_STREAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_DGRAM), MP_ROM_INT(MOD_SOCK_DGRAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_RAW), MP_ROM_INT(MOD_SOCK_RAW) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_IP), MP_ROM_INT(MOD_IPPROTO_IP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_ICMP), MP_ROM_INT(MOD_IPPROTO_ICMP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_TCP), MP_ROM_INT(MOD_IPPROTO_TCP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_UDP), MP_ROM_INT(MOD_IPPROTO_UDP) },
    { MP_ROM_QSTR(MP_QSTR_SOL_SOCKET), MP_ROM_INT(MOD_SOL_SOCKET) },
    { MP_ROM_QSTR(MP_QSTR_SO_REUSEADDR), MP_ROM_INT(MOD_SO_REUSEADDR) },
};
static MP_DEFINE_CONST_DICT(socket_globals, socket_globals_table);

const mp_obj_module_t mp_module_socket = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&socket_globals,
};
MP_REGISTER_MODULE(MP_QSTR_socket, mp_module_socket);
