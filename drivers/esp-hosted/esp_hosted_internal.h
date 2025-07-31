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
 * ESP-Hosted internal.
 */

#ifndef MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_INTERNAL_H
#define MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_INTERNAL_H

#define ESP_FRAME_MAX_SIZE          (1600)
#define ESP_FRAME_MAX_PAYLOAD       (ESP_FRAME_MAX_SIZE - sizeof(esp_header_t))
#define ESP_FRAME_FLAGS_FRAGMENT    (1 << 0)

#define ESP_STATE_NUM_ITFS          (2)
#define ESP_STATE_BUF_SIZE          (ESP_FRAME_MAX_SIZE * 2)
#define ESP_STACK_CAPACITY          (32)
#define ESP_SYNC_REQ_TIMEOUT        (5000)

#define TLV_HEADER_TYPE_EP          (1)
#define TLV_HEADER_TYPE_DATA        (2)
#define TLV_HEADER_EP_RESP          "ctrlResp"
#define TLV_HEADER_EP_EVENT         "ctrlEvnt"

typedef enum {
    ESP_HOSTED_FLAGS_RESET          = (0 << 0),
    ESP_HOSTED_FLAGS_INIT           = (1 << 0),
    ESP_HOSTED_FLAGS_ACTIVE         = (1 << 1),
    ESP_HOSTED_FLAGS_STATIC_IP      = (1 << 2),
    ESP_HOSTED_FLAGS_AP_STARTED     = (1 << 3),
    ESP_HOSTED_FLAGS_STA_CONNECTED  = (1 << 4),
} esp_hosted_flags_t;

typedef enum {
    ESP_PACKET_TYPE_EVENT,
} esp_hosted_priv_packet_t;

typedef enum {
    ESP_PRIV_EVENT_INIT,
} esp_hosted_priv_event_t;

typedef struct esp_hosted_stack {
    size_t capacity;
    size_t top;
    void *buff[ESP_STACK_CAPACITY];
} esp_hosted_stack_t;

typedef struct esp_hosted_state {
    uint8_t chip_id;
    uint8_t spi_clk;
    uint8_t chip_flags;
    uint8_t flags;
    uint16_t seq_num;
    uint32_t last_hb_ms;
    struct netif netif[ESP_STATE_NUM_ITFS];
    struct dhcp dhcp_client;
    dhcp_server_t dhcp_server;
    esp_hosted_stack_t stack;
    uint8_t buf[ESP_STATE_BUF_SIZE];
} esp_hosted_state_t;

typedef struct __attribute__((packed)) {
    uint8_t ep_type;
    uint16_t ep_length;
    uint8_t ep_value[8];
    uint8_t data_type;
    uint16_t data_length;
    uint8_t data[];
} tlv_header_t;

typedef struct __attribute__((packed)) {
    uint8_t event_type;
    uint8_t event_len;
    uint8_t event_data[];
} esp_event_t;

typedef struct __attribute__((packed)) {
    uint8_t if_type    : 4;
    uint8_t if_num     : 4;
    uint8_t flags;
    uint16_t len;
    uint16_t offset;
    uint16_t checksum;
    uint16_t seq_num;
    uint8_t reserved2;
    union {
        uint8_t hci_pkt_type;
        uint8_t priv_pkt_type;
    };
    uint8_t payload[];
} esp_header_t;

uint16_t esp_hosted_checksum(esp_header_t *esp_header);
#endif // MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_INTERNAL_H
