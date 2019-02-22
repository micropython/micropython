/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include "py/mphal.h"
#include "lib/netutils/netutils.h"

static uint32_t get_be16(const uint8_t *buf) {
    return buf[0] << 8 | buf[1];
}

static uint32_t get_be32(const uint8_t *buf) {
    return buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
}

static void dump_hex_bytes(const mp_print_t *print, size_t len, const uint8_t *buf) {
    for (size_t i = 0; i < len; ++i) {
        mp_printf(print, " %02x", buf[i]);
    }
}

static const char *ethertype_str(uint16_t type) {
    // A value between 0x0000 - 0x05dc (inclusive) indicates a length, not type
    switch (type) {
        case 0x0800: return "IPv4";
        case 0x0806: return "ARP";
        case 0x86dd: return "IPv6";
        default: return NULL;
    }
}

void netutils_ethernet_trace(const mp_print_t *print, size_t len, const uint8_t *buf, unsigned int flags) {
    mp_printf(print, "[% 8d] ETH%cX len=%u", mp_hal_ticks_ms(), flags & NETUTILS_TRACE_IS_TX ? 'T' : 'R', len);
    mp_printf(print, " dst=%02x:%02x:%02x:%02x:%02x:%02x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
    mp_printf(print, " src=%02x:%02x:%02x:%02x:%02x:%02x", buf[6], buf[7], buf[8], buf[9], buf[10], buf[11]);

    const char *ethertype = ethertype_str(buf[12] << 8 | buf[13]);
    if (ethertype) {
        mp_printf(print, " type=%s", ethertype);
    } else {
        mp_printf(print, " type=0x%04x", buf[12] << 8 | buf[13]);
    }
    if (len > 14) {
        len -= 14;
        buf += 14;
        if (buf[-2] == 0x08 && buf[-1] == 0x00 && buf[0] == 0x45) {
            // IPv4 packet
            len = get_be16(buf + 2);
            mp_printf(print, " srcip=%u.%u.%u.%u dstip=%u.%u.%u.%u",
                buf[12], buf[13], buf[14], buf[15],
                buf[16], buf[17], buf[18], buf[19]);
            uint8_t prot = buf[9];
            buf += 20;
            len -= 20;
            if (prot == 6) {
                // TCP packet
                uint16_t srcport = get_be16(buf);
                uint16_t dstport = get_be16(buf + 2);
                uint32_t seqnum = get_be32(buf + 4);
                uint32_t acknum = get_be32(buf + 8);
                uint16_t dataoff_flags = get_be16(buf + 12);
                uint16_t winsz = get_be16(buf + 14);
                mp_printf(print, " TCP srcport=%u dstport=%u seqnum=%u acknum=%u dataoff=%u flags=%x winsz=%u",
                    srcport, dstport, (unsigned)seqnum, (unsigned)acknum, dataoff_flags >> 12, dataoff_flags & 0x1ff, winsz);
                buf += 20;
                len -= 20;
                if (dataoff_flags >> 12 > 5) {
                    mp_printf(print, " opts=");
                    size_t opts_len = ((dataoff_flags >> 12) - 5) * 4;
                    dump_hex_bytes(print, opts_len, buf);
                    buf += opts_len;
                    len -= opts_len;
                }
            } else if (prot == 17) {
                // UDP packet
                uint16_t srcport = get_be16(buf);
                uint16_t dstport = get_be16(buf + 2);
                mp_printf(print, " UDP srcport=%u dstport=%u", srcport, dstport);
                len = get_be16(buf + 4);
                buf += 8;
                if ((srcport == 67 && dstport == 68) || (srcport == 68 && dstport == 67)) {
                    // DHCP
                    if (srcport == 67) {
                        mp_printf(print, " DHCPS");
                    } else {
                        mp_printf(print, " DHCPC");
                    }
                    dump_hex_bytes(print, 12 + 16 + 16 + 64, buf);
                    size_t n = 12 + 16 + 16 + 64 + 128;
                    len -= n;
                    buf += n;
                    mp_printf(print, " opts:");
                    switch (buf[6]) {
                        case 1: mp_printf(print, " DISCOVER"); break;
                        case 2: mp_printf(print, " OFFER"); break;
                        case 3: mp_printf(print, " REQUEST"); break;
                        case 4: mp_printf(print, " DECLINE"); break;
                        case 5: mp_printf(print, " ACK"); break;
                        case 6: mp_printf(print, " NACK"); break;
                        case 7: mp_printf(print, " RELEASE"); break;
                        case 8: mp_printf(print, " INFORM"); break;
                    }
                }
            } else {
                // Non-UDP packet
                mp_printf(print, " prot=%u", prot);
            }
        } else if (buf[-2] == 0x86 && buf[-1] == 0xdd && (buf[0] >> 4) == 6) {
            // IPv6 packet
            uint32_t h = get_be32(buf);
            uint16_t l = get_be16(buf + 4);
            mp_printf(print, " tclass=%u flow=%u len=%u nexthdr=%u hoplimit=%u", (unsigned)((h >> 20) & 0xff), (unsigned)(h & 0xfffff), l, buf[6], buf[7]);
            mp_printf(print, " srcip=");
            dump_hex_bytes(print, 16, buf + 8);
            mp_printf(print, " dstip=");
            dump_hex_bytes(print, 16, buf + 24);
            buf += 40;
            len -= 40;
        }
        if (flags & NETUTILS_TRACE_PAYLOAD) {
            mp_printf(print, " data=");
            dump_hex_bytes(print, len, buf);
        }
    }
    if (flags & NETUTILS_TRACE_NEWLINE) {
        mp_printf(print, "\n");
    }
}
