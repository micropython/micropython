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

// A lightweight shim on the stock WIZnet IOLibrary driver which supports
// reading and writing directly into LwIP PBUFs. It's specifically optimized
// based on the assumed usage of for a single socket on the chip configured in
// MACRAW mode. 

#if MICROPY_PY_NETWORK_WIZNET5K && MICROPY_PY_LWIP

#include "lib/wiznet5k/Ethernet/socket.h"
#include "lwip/init.h"
#include "netif/etharp.h"

#include "drivers/wiznet/w5100.h"
#include "drivers/wiznet/w5500.h"

#ifndef printf
#include "py/runtime.h"
#define printf(...) mp_printf(MP_PYTHON_PRINTER, __VA_ARGS__)
#endif

static bool send_is_pending = false;

void wiznet_macraw_reset(uint8_t sn) {
    send_is_pending = false;

    uint8_t addr[4] = {1, 1, 1, 1};
    setSn_DIPR(sn, addr);
    setSn_DPORT(sn, 1111);
}

int32_t wiznet_macraw_send(struct pbuf *head, uint8_t sn) {
    if ((getSn_MR(sn) & 0x0F) != Sn_MR_MACRAW) {
        return SOCKERR_SOCKMODE;
    }
    if (getSn_SR(sn) != SOCK_MACRAW) {
        return SOCKERR_SOCKSTATUS;
    }
      
    while (getSn_TX_FSR(sn) < head->tot_len) {
        WIZCHIP_YIELD();
    }

    // According to the datasheet, "HOST must execute next SEND Command after
    // Sn_IR [SENDOK] is set to ‘1’". Await previous send to complete. Then
    // clear the interrupt bit. There's a potential for a threading/multicore
    // lockup here if this is interrupted and another packet is sent AND the
    // flag is cleared BEFORE returning back to this context. Therefore, we use
    // a variable local to the MCU to indicate we should expect the SENDOK flag
    // to show up on the WIZnet in addition to waiting for the flag.
    while (send_is_pending && (getSn_IR(sn) & Sn_IR_SENDOK) == 0);
    send_is_pending = false;
    setSn_IR(sn, Sn_IR_SENDOK);

    // Actually send the PBUF to the device. This varies slightly from device to
    // device and so is implemented in a breakout.
    int32_t sent = wiznet_macraw_send_pbuf(head, sn);

    // Commit and send written data
    setSn_CR(sn, Sn_CR_SEND);
    while(getSn_CR(sn));

    // Wait for this send to complete before sending more data
    send_is_pending = true;
    return sent;
}

int32_t wiznet_macraw_recv(struct pbuf **pbuf, uint8_t sn) {
    // Ensure that for early exits, it's indicated that the pbuf is invalid
    *pbuf = NULL;

    if ((getSn_MR(sn) & 0x0F) != Sn_MR_MACRAW) {
        return SOCKERR_SOCKMODE;
    }

    // Fetch the size of the next packet, but don't commit the read in case
    // there isn't sufficient memory for a receive buffer.
    uint8_t header[2];
    wiz_recv_data(sn, header, 2);

    uint16_t size = (header[0] << 8) + header[1] - 2;

    if (size > 1514) {
        printf("!!! Invalid receive size of %d bytes\n", size);
        WIZCHIP_EXPORT(close)(sn);
        return SOCKFATAL_PACKLEN;
    }

    // Before consuming the packet size field from the device attempt to
    // allocate a pbuf
    struct pbuf *head = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);
    if (head == NULL) {
        return SOCKERR_BUFFER;
    }

    // Proceed to fetch the data. Commit the read of the header. After this we
    // must read `size` bytes or the state of the data in the chip will be
    // wrong.
    setSn_CR(sn, Sn_CR_RECV);
    while (getSn_CR(sn));

    struct pbuf *p = head;
    uint16_t to_copy = size;
    while (to_copy) {
        wiz_recv_data(sn, p->payload, MIN(to_copy, p->len));
        setSn_CR(sn, Sn_CR_RECV);
        while (getSn_CR(sn));

        to_copy -= p->len;
        p = p->next;
    }

   *pbuf = head;
   return (int32_t) size;
}

#endif