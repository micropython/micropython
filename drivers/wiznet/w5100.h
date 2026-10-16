/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Jared Hancock <jared@greezybacon.me>
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

#include <stdint.h>
#include "py/runtime.h"

#if MICROPY_PY_NETWORK_WIZNET5K && MICROPY_PY_LWIP
#if _WIZCHIP_ == W5100 || _WIZCHIP_ == W5100S

#include "lib/wiznet5k/Ethernet/socket.h"
#include "netif/etharp.h"

static inline int32_t wiznet_macraw_send_pbuf(struct pbuf * head, const uint8_t sn) {
    const uint16_t txbase = getSn_TxBASE(sn);
    const uint16_t txmax = getSn_TxMAX(sn);
    const uint16_t txmask = txmax - 1;

    uint16_t ptr;
    uint16_t dst_mask;
    uint16_t dst_ptr;
    uint16_t offset;

    ptr = getSn_TX_WR(sn);

    size_t left_to_send = head->tot_len;
    struct pbuf *p = head;
    while (p != NULL && left_to_send > 0) {
        dst_mask = ptr & txmask;
        dst_ptr = txbase + dst_mask;
  
        if (dst_mask + p->len > txmax) {
            // Packet will wrap over the end of the chip buffer. Send to the end
            // of the chip buffer and manually wrap around.
            offset = txmax - dst_mask;
            WIZCHIP_WRITE_BUF(dst_ptr, p->payload, offset);
            dst_ptr = txbase;
        } else {
            offset = 0;
        }
        WIZCHIP_WRITE_BUF(dst_ptr, p->payload + offset, p->len - offset);
        ptr += p->len;
        left_to_send -= p->len;
        p = p->next;
    }

    setSn_TX_WR(0, ptr);  
    return head->tot_len;
}

#endif
#endif
