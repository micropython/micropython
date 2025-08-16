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
#if _WIZCHIP_ == W5500

#include "lib/wiznet5k/Ethernet/socket.h"
#include "netif/etharp.h"

static inline int32_t wiznet_macraw_send_pbuf(struct pbuf * head, const uint8_t sn) {
    const uint32_t txbuf_block = WIZCHIP_TXBUF_BLOCK(0) << 3;
    uint16_t ptr = getSn_TX_WR(sn);
    uint32_t addrsel = 0;

    size_t left_to_send = head->tot_len;
    struct pbuf *p = head;
    while (p != NULL && left_to_send > 0) {
        addrsel = ((uint32_t)ptr << 8) + txbuf_block;
        WIZCHIP_WRITE_BUF(addrsel, p->payload, p->len);

        ptr += p->len;
        left_to_send -= p->len;
        p = p->next;
    }
   
    setSn_TX_WR(sn, ptr);
}

#endif
#endif