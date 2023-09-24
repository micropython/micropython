/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2020 Nick Moore
 * Copyright (c) 2018 shawwwn <shawwwn1@gmail.com>
 * Copyright (c) 2020-2021 Glenn Moloney @glenn20
 * Copyright (c) 2023 MicroDev
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

#include "py/mperrno.h"
#include "py/runtime.h"

#include "bindings/espidf/__init__.h"
#include "bindings/espnow/ESPNowPacket.h"

#include "shared-bindings/wifi/__init__.h"

#include "common-hal/espnow/ESPNow.h"

#include "mphalport.h"

#include "esp_now.h"

#define ESPNOW_MAGIC 0x99

// TODO: deinit wifi?

// The min/max length of an espnow packet (bytes)
#define MIN_PACKET_LEN (sizeof(espnow_packet_t))
#define MAX_PACKET_LEN (sizeof(espnow_packet_t) + ESP_NOW_MAX_DATA_LEN)

// Enough for 2 full-size packets: 2 * (6 + 7 + 250) = 526 bytes
// Will allocate an additional 7 bytes for buffer overhead
#define DEFAULT_RECV_BUFFER_SIZE (2 * MAX_PACKET_LEN)

// Time to wait (millisec) for responses from sent packets: (2 seconds).
#define DEFAULT_SEND_TIMEOUT_MS (2000)

// ESPNow packet format for the receive buffer.
// Use this for peeking at the header of the next packet in the buffer.
typedef struct {
    uint8_t magic;              // = ESPNOW_MAGIC
    uint8_t msg_len;            // Length of the message
    uint32_t time_ms;           // Timestamp (ms) when packet is received
    int8_t rssi;                // RSSI value (dBm) (-127 to 0)
} __attribute__((packed)) espnow_header_t;

typedef struct {
    espnow_header_t header;     // The header
    uint8_t peer[6];            // Peer address
    uint8_t msg[0];             // Message is up to 250 bytes
} __attribute__((packed)) espnow_packet_t;

// --- The ESP-NOW send and recv callback routines ---

// Callback triggered when a sent packet is acknowledged by the peer (or not).
// Just count the number of responses and number of failures.
// These are used in the send() logic.
static void send_cb(const uint8_t *mac, esp_now_send_status_t status) {
    espnow_obj_t *self = MP_STATE_PORT(espnow_singleton);
    if (status == ESP_NOW_SEND_SUCCESS) {
        self->send_success++;
    } else {
        self->send_failure++;
    }
}

// Callback triggered when an ESP-NOW packet is received.
// Write the peer MAC address and the message into the recv_buffer as an ESPNow packet.
// If the buffer is full, drop the message and increment the dropped count.
static void recv_cb(const esp_now_recv_info_t *esp_now_info, const uint8_t *msg, int msg_len) {
    espnow_obj_t *self = MP_STATE_PORT(espnow_singleton);
    ringbuf_t *buf = self->recv_buffer;

    if (sizeof(espnow_packet_t) + msg_len > ringbuf_num_empty(buf)) {
        self->read_failure++;
        return;
    }

    // Get the RSSI value from the wifi packet header
    // Secret magic to get the rssi from the wifi packet header
    // See espnow.c:espnow_recv_cb() at https://github.com/espressif/esp-now/
    // In the wifi packet the msg comes after a wifi_promiscuous_pkt_t
    // and a espnow_frame_format_t.
    // Backtrack to get a pointer to the wifi_promiscuous_pkt_t.
    #define SIZEOF_ESPNOW_FRAME_FORMAT 39
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    wifi_promiscuous_pkt_t *wifi_packet = (wifi_promiscuous_pkt_t *)(
        msg - SIZEOF_ESPNOW_FRAME_FORMAT - sizeof(wifi_promiscuous_pkt_t));
    #pragma GCC diagnostic pop

    espnow_header_t header;
    header.magic = ESPNOW_MAGIC;
    header.msg_len = msg_len;
    header.rssi = wifi_packet->rx_ctrl.rssi;
    header.time_ms = mp_hal_ticks_ms();

    ringbuf_put_n(buf, (uint8_t *)&header, sizeof(header));
    ringbuf_put_n(buf, esp_now_info->src_addr, ESP_NOW_ETH_ALEN);
    ringbuf_put_n(buf, msg, msg_len);

    self->read_success++;
}

bool common_hal_espnow_deinited(espnow_obj_t *self) {
    return self == NULL || self->recv_buffer == NULL;
}

// Construct the ESPNow object
void common_hal_espnow_construct(espnow_obj_t *self, mp_int_t buffer_size, mp_int_t phy_rate) {
    common_hal_espnow_set_phy_rate(self, phy_rate);
    self->recv_buffer_size = mp_arg_validate_int_min(buffer_size, MIN_PACKET_LEN, MP_QSTR_buffer_size);
    self->peers = espnow_peers_new();
    common_hal_espnow_init(self);
}

// Initialize the ESP-NOW software stack,
// register callbacks and allocate the recv data buffers.
void common_hal_espnow_init(espnow_obj_t *self) {
    if (!common_hal_espnow_deinited(self)) {
        return;
    }

    self->recv_buffer = m_new_obj(ringbuf_t);
    if (!ringbuf_alloc(self->recv_buffer, self->recv_buffer_size /*, true*/)) {
        m_malloc_fail(self->recv_buffer_size);
    }

    if (!common_hal_wifi_radio_get_enabled(&common_hal_wifi_radio_obj)) {
        common_hal_wifi_init(false);
        common_hal_wifi_radio_set_enabled(&common_hal_wifi_radio_obj, true);
    }

    CHECK_ESP_RESULT(esp_wifi_config_espnow_rate(ESP_IF_WIFI_STA, self->phy_rate));
    CHECK_ESP_RESULT(esp_wifi_config_espnow_rate(ESP_IF_WIFI_AP, self->phy_rate));

    CHECK_ESP_RESULT(esp_now_init());
    CHECK_ESP_RESULT(esp_now_register_send_cb(send_cb));
    CHECK_ESP_RESULT(esp_now_register_recv_cb(recv_cb));
}

// De-initialize the ESP-NOW software stack,
// disable callbacks and deallocate the recv data buffer.
void common_hal_espnow_deinit(espnow_obj_t *self) {
    if (common_hal_espnow_deinited(self)) {
        return;
    }

    CHECK_ESP_RESULT(esp_now_unregister_send_cb());
    CHECK_ESP_RESULT(esp_now_unregister_recv_cb());
    CHECK_ESP_RESULT(esp_now_deinit());

    self->recv_buffer->buf = NULL;
    self->recv_buffer = NULL;
}

void espnow_reset(void) {
    common_hal_espnow_deinit(MP_STATE_PORT(espnow_singleton));
    MP_STATE_PORT(espnow_singleton) = NULL;
}

void common_hal_espnow_set_phy_rate(espnow_obj_t *self, mp_int_t value) {
    self->phy_rate = mp_arg_validate_int_range(value, 0, WIFI_PHY_RATE_MAX - 1, MP_QSTR_phy_rate);
};

void common_hal_espnow_set_pmk(espnow_obj_t *self, const uint8_t *key) {
    CHECK_ESP_RESULT(esp_now_set_pmk(key));
}

// --- Send and Receive ESP-NOW data ---


mp_obj_t common_hal_espnow_send(espnow_obj_t *self, const mp_buffer_info_t *message, const uint8_t *mac) {
    // Send the packet - keep trying until timeout if the internal esp-now buffers are full.
    esp_err_t err;
    mp_uint_t start = mp_hal_ticks_ms();

    while ((ESP_ERR_ESPNOW_NO_MEM == (err = esp_now_send(mac, message->buf, message->len))) &&
           (mp_hal_ticks_ms() - start) <= DEFAULT_SEND_TIMEOUT_MS) {
        RUN_BACKGROUND_TASKS;
    }
    CHECK_ESP_RESULT(err);

    return mp_const_none;
}

mp_obj_t common_hal_espnow_read(espnow_obj_t *self) {
    if (!ringbuf_num_filled(self->recv_buffer)) {
        return mp_const_none;
    }

    // Read the packet header from the incoming buffer
    espnow_header_t header;
    if (ringbuf_get_n(self->recv_buffer, (uint8_t *)&header, sizeof(header)) != sizeof(header)) {
        mp_arg_error_invalid(MP_QSTR_buffer);
    }

    uint8_t msg_len = header.msg_len;

    uint8_t mac_buf[ESP_NOW_ETH_ALEN];
    uint8_t msg_buf[msg_len];

    // Check the message packet header format and read the message data
    if (header.magic != ESPNOW_MAGIC ||
        msg_len > ESP_NOW_MAX_DATA_LEN ||
        ringbuf_get_n(self->recv_buffer, mac_buf, ESP_NOW_ETH_ALEN) != ESP_NOW_ETH_ALEN ||
        ringbuf_get_n(self->recv_buffer, msg_buf, msg_len) != msg_len) {
        mp_arg_error_invalid(MP_QSTR_buffer);
    }

    mp_obj_t elems[4] = {
        mp_obj_new_bytes(mac_buf, ESP_NOW_ETH_ALEN),
        mp_obj_new_bytes(msg_buf, msg_len),
        MP_OBJ_NEW_SMALL_INT(header.rssi),
        mp_obj_new_int(header.time_ms),
    };

    return namedtuple_make_new((const mp_obj_type_t *)&espnow_packet_type_obj, 4, 0, elems);
}
