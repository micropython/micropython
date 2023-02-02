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

#include "bindings/espnow/ESPNowPacket.h"
#include "shared-bindings/wifi/__init__.h"

#include "common-hal/espnow/ESPNow.h"

#include "mphalport.h"

#include "esp_now.h"

#define ESPNOW_MAGIC 0x99

// The maximum length of an espnow packet (bytes)
#define MAX_PACKET_LEN (sizeof(espnow_packet_t) + ESP_NOW_MAX_DATA_LEN)

// Enough for 2 full-size packets: 2 * (6 + 7 + 250) = 526 bytes
// Will allocate an additional 7 bytes for buffer overhead
#define DEFAULT_RECV_BUFFER_SIZE (2 * MAX_PACKET_LEN)

// Time to wait (millisec) for responses from sent packets: (2 seconds).
#define DEFAULT_SEND_TIMEOUT_MS (2 * 1000)

// Number of milliseconds to wait for pending responses to sent packets.
// This is a fallback which should never be reached.
#define PENDING_RESPONSES_TIMEOUT_MS    100
#define PENDING_RESPONSES_BUSY_POLL_MS  10

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

static void check_esp_err(esp_err_t status) {
    if (status != ESP_OK) {
        mp_raise_RuntimeError(translate("an error occured"));
    }
}

// Return a pointer to the ESPNow module singleton
static espnow_obj_t *_get_singleton(void) {
    return MP_STATE_PORT(espnow_singleton);
}

// --- The ESP-NOW send and recv callback routines ---

// Callback triggered when a sent packet is acknowledged by the peer (or not).
// Just count the number of responses and number of failures.
// These are used in the send() logic.
static void send_cb(const uint8_t *mac, esp_now_send_status_t status) {
    espnow_obj_t *self = _get_singleton();
    self->tx_responses++;
    if (status != ESP_NOW_SEND_SUCCESS) {
        self->tx_failures++;
    }
}

static inline int8_t _get_rssi_from_wifi_packet(const uint8_t *msg);

// Callback triggered when an ESP-NOW packet is received.
// Write the peer MAC address and the message into the recv_buffer as an ESPNow packet.
// If the buffer is full, drop the message and increment the dropped count.
static void recv_cb(const uint8_t *mac, const uint8_t *msg, int msg_len) {
    espnow_obj_t *self = _get_singleton();
    ringbuf_t *buf = self->recv_buffer;

    if (sizeof(espnow_packet_t) + msg_len > ringbuf_num_empty(buf)) {
        self->rx_failures++;
        return;
    }

    espnow_header_t header;
    header.magic = ESPNOW_MAGIC;
    header.msg_len = msg_len;
    header.rssi = _get_rssi_from_wifi_packet(msg);
    header.time_ms = mp_hal_ticks_ms();

    ringbuf_put_n(buf, (uint8_t *)&header, sizeof(header));
    ringbuf_put_n(buf, mac, ESP_NOW_ETH_ALEN);
    ringbuf_put_n(buf, msg, msg_len);

    self->rx_packets++;
}

bool common_hal_espnow_deinited(espnow_obj_t *self) {
    return self == NULL || self->recv_buffer == NULL;
}

// Initialize the ESP-NOW software stack,
// register callbacks and allocate the recv data buffers.
void common_hal_espnow_init(espnow_obj_t *self) {
    if (!common_hal_espnow_deinited(self)) {
        return;
    }

    self->recv_buffer = m_new_obj(ringbuf_t);
    if (!ringbuf_alloc(self->recv_buffer, self->recv_buffer_size, true)) {
        m_malloc_fail(self->recv_buffer_size);
    }

    if (!common_hal_wifi_radio_get_enabled(&common_hal_wifi_radio_obj)) {
        common_hal_wifi_init(false);
        common_hal_wifi_radio_set_enabled(&common_hal_wifi_radio_obj, true);
    }

    check_esp_err(esp_wifi_config_espnow_rate(ESP_IF_WIFI_STA, self->phy_rate));
    check_esp_err(esp_wifi_config_espnow_rate(ESP_IF_WIFI_AP, self->phy_rate));

    check_esp_err(esp_now_init());
    check_esp_err(esp_now_register_send_cb(send_cb));
    check_esp_err(esp_now_register_recv_cb(recv_cb));
}

// De-initialize the ESP-NOW software stack,
// disable callbacks and deallocate the recv data buffers.
void common_hal_espnow_deinit(espnow_obj_t *self) {
    if (common_hal_espnow_deinited(self)) {
        return;
    }

    check_esp_err(esp_now_unregister_send_cb());
    check_esp_err(esp_now_unregister_recv_cb());
    check_esp_err(esp_now_deinit());

    self->recv_buffer->buf = NULL;
    self->recv_buffer = NULL;
    // self->peers_count = 0; // esp_now_deinit() removes all peers.
    self->tx_packets = self->tx_responses;
}

void espnow_reset(void) {
    common_hal_espnow_deinit(_get_singleton());
    MP_STATE_PORT(espnow_singleton) = NULL;
}

void common_hal_espnow_set_buffer_size(espnow_obj_t *self, mp_int_t value) {
    self->recv_buffer_size = mp_arg_validate_int_min(value, MAX_PACKET_LEN, MP_QSTR_buffer_size);
};

void common_hal_espnow_set_phy_rate(espnow_obj_t *self, mp_int_t value) {
    self->phy_rate = mp_arg_validate_int_range(value, 0, WIFI_PHY_RATE_MAX - 1, MP_QSTR_phy_rate);
};

void common_hal_espnow_set_pmk(espnow_obj_t *self, const uint8_t *key) {
    check_esp_err(esp_now_set_pmk(key));
}

// --- Maintaining the peer table and reading RSSI values ---

// We maintain a peers table for several reasons, to:
// - support monitoring the RSSI values for all peers; and
// - to return unique bytestrings for each peer which supports more efficient
//   application memory usage and peer handling.

// Get the RSSI value from the wifi packet header
static inline int8_t _get_rssi_from_wifi_packet(const uint8_t *msg) {
    // Warning: Secret magic to get the rssi from the wifi packet header
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
    return wifi_packet->rx_ctrl.rssi;
}

// Lookup a peer in the peers table and return a reference to the item in the peers_table.
// Add peer to the table if it is not found (may alloc memory). Will not return NULL.
static mp_map_elem_t *_lookup_add_peer(espnow_obj_t *self, const uint8_t *peer) {
    // We do not want to allocate any new memory in the case that the peer
    // already exists in the peers_table (which is almost all the time).
    // So, we use a byte string on the stack and look that up in the dict.
    mp_map_t *map = mp_obj_dict_get_map(self->peers_table);
    mp_obj_str_t peer_obj = {{&mp_type_bytes}, 0, ESP_NOW_ETH_ALEN, peer};
    mp_map_elem_t *item = mp_map_lookup(map, &peer_obj, MP_MAP_LOOKUP);
    if (item == NULL) {
        // If not found, add the peer using a new bytestring
        map->is_fixed = 0;      // Allow to modify the dict
        mp_obj_t new_peer = mp_obj_new_bytes(peer, ESP_NOW_ETH_ALEN);
        item = mp_map_lookup(map, new_peer, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
        item->value = mp_obj_new_list(2, NULL);
        map->is_fixed = 1;      // Relock the dict
    }
    return item;
}

// Update the peers table with the new rssi value from a received packet and
// return a reference to the item in the peers_table.
static void _update_rssi(espnow_obj_t *self, const uint8_t *peer, int8_t rssi, uint32_t time_ms) {
    // Lookup the peer in the device table
    mp_map_elem_t *item = _lookup_add_peer(self, peer);
    mp_obj_list_t *list = MP_OBJ_TO_PTR(item->value);
    list->items[0] = MP_OBJ_NEW_SMALL_INT(rssi);
    list->items[1] = mp_obj_new_int(time_ms);
}

// --- Send and Receive ESP-NOW data ---

// Used by espnow_send() for sends() with sync==True.
// Wait till all pending sent packet responses have been received.
// ie. self->tx_responses == self->tx_packets.
static void _wait_for_pending_responses(espnow_obj_t *self) {
    mp_uint_t t, start = mp_hal_ticks_ms();
    while (self->tx_responses < self->tx_packets) {
        if ((t = mp_hal_ticks_ms() - start) > PENDING_RESPONSES_TIMEOUT_MS) {
            mp_raise_OSError(MP_ETIMEDOUT);
        }
        if (t > PENDING_RESPONSES_BUSY_POLL_MS) {
            // After 10ms of busy waiting give other tasks a look in.
            RUN_BACKGROUND_TASKS;
        }
    }
}

mp_obj_t common_hal_espnow_send(espnow_obj_t *self, const bool sync, const uint8_t *mac, const mp_buffer_info_t *message) {
    if (sync) {
        // Flush out any pending responses.
        // If the last call was sync == False there may be outstanding responses
        // still to be received (possible many if we just had a burst of unsync send()s).
        // We need to wait for all pending responses if this call has sync = True.
        _wait_for_pending_responses(self);
    }

    // Send the packet - try, try again if internal esp-now buffers are full.
    esp_err_t err;
    size_t saved_failures = self->tx_failures;
    mp_uint_t start = mp_hal_ticks_ms();

    while ((ESP_ERR_ESPNOW_NO_MEM == (err = esp_now_send(mac, message->buf, message->len))) &&
           (mp_hal_ticks_ms() - start) <= DEFAULT_SEND_TIMEOUT_MS) {
        RUN_BACKGROUND_TASKS;
    }
    check_esp_err(err);

    // Increment the sent packet count.
    // If mac == NULL msg will be sent to all peers EXCEPT any broadcast or multicast addresses.
    self->tx_packets += ((mac == NULL) ? ((mp_obj_list_t *)self->peers->list)->len : 1);

    if (sync) {
        // Wait for and tally all the expected responses from peers
        _wait_for_pending_responses(self);
    }

    // Return False if sync and any peers did not respond.
    return mp_obj_new_bool(!(sync && self->tx_failures != saved_failures));
}

mp_obj_t common_hal_espnow_recv(espnow_obj_t *self) {
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

    // Update rssi value in the peer device table
    _update_rssi(self, mac_buf, header.rssi, header.time_ms);

    mp_obj_t elems[4] = {
        mp_obj_new_bytes(mac_buf, ESP_NOW_ETH_ALEN),
        mp_obj_new_bytes(msg_buf, msg_len),
        MP_OBJ_NEW_SMALL_INT(header.rssi),
        mp_obj_new_int(header.time_ms),
    };

    return namedtuple_make_new((const mp_obj_type_t *)&espnow_packet_type_obj, 4, 0, elems);
}
