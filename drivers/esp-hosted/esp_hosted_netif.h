/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Arduino SA
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
 * ESP-Hosted LWIP netif functions.
 */

#ifndef MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_NETIF_H
#define MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_NETIF_H

typedef struct esp_hosted_state esp_hosted_state_t;
int esp_hosted_netif_init(esp_hosted_state_t *state, uint32_t itf);
int esp_hosted_netif_deinit(esp_hosted_state_t *state, uint32_t itf);
int esp_hosted_netif_input(esp_hosted_state_t *state, uint32_t itf, const void *buf, size_t len);
err_t esp_hosted_netif_output(struct netif *netif, struct pbuf *p);

#endif // MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_NETIF_H
