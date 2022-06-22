/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2020 Nick Moore
 * Copyright (c) 2018 shawwwn <shawwwn1@gmail.com>
 * Copyright (c) 2020-2021 Glenn Moloney @glenn20
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


#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "esp_log.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/objarray.h"
#include "py/stream.h"
#include "py/binary.h"

#include "mpconfigport.h"
#include "mphalport.h"
#include "modnetwork.h"
#include "shared/runtime/ring_buffer.h"
#include "esp_espnow.h"

// Relies on gcc Variadic Macros and Statement Expressions
#define NEW_TUPLE(...) \
    ({mp_obj_t _z[] = {__VA_ARGS__}; mp_obj_new_tuple(MP_ARRAY_SIZE(_z), _z); })

static const uint8_t ESPNOW_MAGIC = 0x99;

// ESPNow packet format for the receive buffer.
// Use this for peeking at the header of the next packet in the buffer.
typedef struct {
    uint8_t magic;              // = ESPNOW_MAGIC
    uint8_t msg_len;            // Length of the message
    uint32_t timestamp_ms;      // Timestamp (ms) when packet is received
    int8_t rssi;                // RSSI value (dBm) (-127 to 0)
} __attribute__((packed)) espnow_hdr_t;

typedef struct {
    espnow_hdr_t hdr;           // The header
    uint8_t peer[6];            // Peer address
    uint8_t msg[0];             // Message is up to 250 bytes
} __attribute__((packed)) espnow_pkt_t;

// The maximum length of an espnow packet (bytes)
static const size_t MAX_PACKET_LEN = (
    (sizeof(espnow_pkt_t) + ESP_NOW_MAX_DATA_LEN));

// Enough for 2 full-size packets: 2 * (6 + 7 + 250) = 526 bytes
// Will allocate an additional 7 bytes for buffer overhead
static const size_t DEFAULT_RECV_BUFFER_SIZE = (2 * MAX_PACKET_LEN);

// Default timeout (millisec) to wait for incoming ESPNow messages (5 minutes).
static const size_t DEFAULT_RECV_TIMEOUT_MS = (5 * 60 * 1000);

// Time to wait (millisec) for responses from sent packets: (2 seconds).
static const size_t DEFAULT_SEND_TIMEOUT_MS = (2 * 1000);

// Number of milliseconds to wait (mp_hal_wait_ms()) in each loop
// while waiting for send or receive packet.
// Needs to be >15ms to permit yield to other tasks.
static const size_t BUSY_WAIT_MS = 25;

// RECV_DATA event code for irq function
static const size_t EVENT_RECV_DATA = 1;

// The data structure for the espnow_singleton.
typedef struct _esp_espnow_obj_t {
    mp_obj_base_t base;
    buffer_t recv_buffer;           // A buffer for received packets
    size_t recv_buffer_size;        // The size of the recv_buffer
    size_t recv_timeout_ms;         // Timeout for recv()/irecv()
    volatile size_t rx_packets;     // # of received packets
    size_t dropped_rx_pkts;         // # of dropped packets (buffer full)
    size_t tx_packets;              // # of sent packets
    volatile size_t tx_responses;   // # of sent packet responses received
    volatile size_t tx_failures;    // # of sent packet responses failed
    size_t peer_count;              // Cache the # of peers for send(sync=True)
    mp_obj_t recv_cb;               // Callback when a packet is received
    mp_obj_tuple_t *irecv_tuple;    // Preallocated tuple for irecv()
    mp_obj_t peers_table;           // A dictionary of discovered peers
} esp_espnow_obj_t;

const mp_obj_type_t esp_espnow_type;

// ### Initialisation and Config functions
//

#define INITIALISED         (1)

// Return a pointer to the ESPNow module singleton
// If state == INITIALISED check the device has been initialised.
// Raises OSError if not initialised and state == INITIALISED.
static esp_espnow_obj_t *_get_singleton(int state) {
    esp_espnow_obj_t *self = MP_STATE_PORT(espnow_singleton);
    // assert(self);
    if (state == INITIALISED && self->recv_buffer == NULL) {
        // Throw an espnow not initialised error
        check_esp_err(ESP_ERR_ESPNOW_NOT_INIT);
    }
    return self;
}

// Allocate and initialise the ESPNow module as a singleton.
// Returns the initialised espnow_singleton.
STATIC mp_obj_t espnow_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {

    // The espnow_singleton must be defined in MICROPY_PORT_ROOT_POINTERS
    // (see mpconfigport.h) to prevent memory allocated here from being
    // garbage collected.
    // NOTE: on soft reset the espnow_singleton MUST be set to NULL and the
    // ESP-NOW functions de-initialised (see main.c).
    esp_espnow_obj_t *self = MP_STATE_PORT(espnow_singleton);
    if (self != NULL) {
        return self;
    }
    self = m_new_obj(esp_espnow_obj_t);
    self->base.type = &esp_espnow_type;
    self->recv_buffer_size = DEFAULT_RECV_BUFFER_SIZE;
    self->recv_timeout_ms = DEFAULT_RECV_TIMEOUT_MS;
    self->recv_buffer = NULL;       // Buffer is allocated in espnow_init()

    // Allocate and initialise the "callee-owned" tuples for irecv().
    // Build a tuple (peer, msg). Where peer will be a reference
    // to the peer mac address in the peers table and msg is a byte string
    // to hold a copy of the message from the packet buffer.
    byte tmp[ESP_NOW_MAX_DATA_LEN] = {0};
    self->irecv_tuple = NEW_TUPLE(
        mp_const_none,
        mp_obj_new_bytes(tmp, MP_ARRAY_SIZE(tmp)));

    self->recv_cb = mp_const_none;
    self->peers_table = mp_obj_new_dict(0);
    // Prevent user code modifying the dict
    mp_obj_dict_get_map(self->peers_table)->is_fixed = 1;

    // Set the global singleton pointer for the espnow protocol.
    MP_STATE_PORT(espnow_singleton) = self;

    return self;
}

// Forward declare the send and recv ESPNow callbacks
STATIC void send_cb(const uint8_t *mac_addr, esp_now_send_status_t status);

STATIC void recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len);

// ESPNow.init(): Initialise the data buffers and ESP-NOW functions.
// Initialise the Espressif ESPNOW software stack, register callbacks and
// allocate the recv data buffers.
// Returns None.
static mp_obj_t espnow_init(mp_obj_t _) {
    esp_espnow_obj_t *self = _get_singleton(0);
    if (self->recv_buffer == NULL) {    // Already initialised
        self->recv_buffer = buffer_init(self->recv_buffer_size);
        self->recv_buffer_size = buffer_size(self->recv_buffer);

        esp_initialise_wifi();  // Call the wifi init code in network_wlan.c
        check_esp_err(esp_now_init());
        check_esp_err(esp_now_register_recv_cb(recv_cb));
        check_esp_err(esp_now_register_send_cb(send_cb));
    }
    return mp_const_none;
}

// ESPNow.deinit(): De-initialise the ESPNOW software stack, disable callbacks
// and deallocate the recv data buffers.
// Note: this function is called from main.c:mp_task() to cleanup before soft
// reset, so cannot be declared STATIC and must guard against self == NULL;.
mp_obj_t espnow_deinit(mp_obj_t _) {
    esp_espnow_obj_t *self = _get_singleton(0);
    if (self != NULL && self->recv_buffer != NULL) {
        check_esp_err(esp_now_unregister_recv_cb());
        check_esp_err(esp_now_unregister_send_cb());
        check_esp_err(esp_now_deinit());
        buffer_release(self->recv_buffer);
        self->recv_buffer = NULL;
        self->peer_count = 0; // esp_now_deinit() removes all peers.
        self->tx_packets = self->tx_responses;
    }
    return mp_const_none;
}

STATIC mp_obj_t espnow_active(size_t n_args, const mp_obj_t *args) {
    esp_espnow_obj_t *self = _get_singleton(0);
    if (n_args > 1) {
        if (mp_obj_is_true(args[1])) {
            espnow_init(self);
        } else {
            espnow_deinit(self);
        }
    }
    return self->recv_buffer != NULL ? mp_const_true : mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espnow_active_obj, 1, 2, espnow_active);

// ESPNow.config(['param'|param=value, ..])
// Get or set configuration values. Supported config params:
//    buffer: size of buffer for rx packets (default=514 bytes)
//    timeout: Default read timeout (default=300,000 milliseconds)
STATIC mp_obj_t espnow_config(
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    esp_espnow_obj_t *self = _get_singleton(0);
    enum { ARG_get, ARG_buffer, ARG_timeout, ARG_rate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_get, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_buffer, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_buffer].u_int >= 0) {
        self->recv_buffer_size = args[ARG_buffer].u_int;
    }
    if (args[ARG_timeout].u_int >= 0) {
        self->recv_timeout_ms = args[ARG_timeout].u_int;
    }
    if (args[ARG_rate].u_int >= 0) {
        #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 0)
        esp_initialise_wifi();  // Call the wifi init code in network_wlan.c
        check_esp_err(esp_wifi_config_espnow_rate(
            ESP_IF_WIFI_STA, args[ARG_rate].u_int));
        check_esp_err(esp_wifi_config_espnow_rate(
            ESP_IF_WIFI_AP, args[ARG_rate].u_int));
        #else
        mp_raise_ValueError(MP_ERROR_TEXT("rate option not supported"));
        #endif
    }
    if (args[ARG_get].u_obj == MP_OBJ_NULL) {
        return mp_const_none;
    }
#define QS(x) (uintptr_t)MP_OBJ_NEW_QSTR(x)
    // Return the value of the requested parameter
    uintptr_t name = (uintptr_t)args[ARG_get].u_obj;
    if (name == QS(MP_QSTR_buffer)) {
        return mp_obj_new_int(self->recv_buffer_size);
    } else if (name == QS(MP_QSTR_timeout)) {
        return mp_obj_new_int(self->recv_timeout_ms);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
    }
#undef QS

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(espnow_config_obj, 1, espnow_config);

// ESPNow.irq(recv_cb)
// Set callback function to be invoked when a message is received.
STATIC mp_obj_t espnow_irq(mp_obj_t _, mp_obj_t recv_cb) {
    esp_espnow_obj_t *self = _get_singleton(0);
    if (recv_cb != mp_const_none && !mp_obj_is_callable(recv_cb)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid handler"));
    }
    self->recv_cb = recv_cb;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_irq_obj, espnow_irq);

// ESPnow.stats(): Provide some useful stats.
// Returns a tuple of:
//   (tx_pkts, tx_responses, tx_failures, rx_pkts, dropped_rx_pkts)
STATIC mp_obj_t espnow_stats(mp_obj_t _) {
    const esp_espnow_obj_t *self = _get_singleton(0);
    return NEW_TUPLE(
        mp_obj_new_int(self->tx_packets),
        mp_obj_new_int(self->tx_responses),
        mp_obj_new_int(self->tx_failures),
        mp_obj_new_int(self->rx_packets),
        mp_obj_new_int(self->dropped_rx_pkts));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_stats_obj, espnow_stats);

// ### Maintaining the peer table and reading RSSI values
//
// We maintain a peers table for several reasons, to:
// - support monitoring the RSSI values for all peers; and
// - to return unique bytestrings for each peer which supports more efficient
//   application memory usage and peer handling.
// - In future we can also efficiently support recv buffers for each host.

// Get the RSSI value from the wifi packet header
static inline int8_t _get_rssi_from_wifi_pkt(const uint8_t *msg) {
    // This struct is solely to support backtracking to get the rssi of msgs
    // See espnow.c:espnow_recv_cb() at https://github.com/espressif/esp-now/
    // All we actually use is sizeof(espnow_frame_format_t).
    // Could be replaced with:
    //   static const size_t sizeof_espnow_frame = 39;
    typedef struct
    {
        uint16_t frame_head;
        uint16_t duration;
        uint8_t destination_address[6];
        uint8_t source_address[6];
        uint8_t broadcast_address[6];
        uint16_t sequence_control;

        uint8_t category_code;
        uint8_t organization_identifier[3]; // 0x18fe34
        uint8_t random_values[4];
        struct {
            uint8_t element_id;                 // 0xdd
            uint8_t lenght;                     //
            uint8_t organization_identifier[3]; // 0x18fe34
            uint8_t type;                       // 4
            uint8_t version;
            uint8_t body[0];
        } vendor_specific_content;
    } __attribute__((packed)) espnow_frame_format_t;

    // Warning: Secret magic to get the rssi from the wifi packet header
    // See espnow.c:espnow_recv_cb() at https://github.com/espressif/esp-now/
    // In the wifi packet the msg comes after a wifi_promiscuous_pkt_t
    // and a espnow_frame_format_t.
    // Backtrack to get a pointer to the wifi_promiscuous_pkt_t.
    wifi_promiscuous_pkt_t *wifi_pkt = (wifi_promiscuous_pkt_t *)(
        msg - sizeof(espnow_frame_format_t) - sizeof(wifi_promiscuous_pkt_t));

    #if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 2, 0)
    return wifi_pkt->rx_ctrl.rssi - 100;  // Offset rssi for IDF 4.0.2
    #else
    return wifi_pkt->rx_ctrl.rssi;
    #endif
}

// Lookup a peer in the peers table and return a reference to the item in the
// peers_table. Add peer to the table if it is not found (may alloc memory).
// Will not return NULL.
static mp_map_elem_t *_lookup_add_peer(
    esp_espnow_obj_t *self, const uint8_t *peer) {

    // We do not want to allocate any new memory in the case that the peer
    // already exists in the peers_table (which is almost all the time).
    // So, we use a byte string on the stack and look that up in the dict.
    // Warning - depends on internal representation of mp_obj_str_t.
    mp_obj_str_t peer_obj = {  // A byte string on the stack holding peer addr
        .base = {&mp_type_bytes},
        .hash = 0, // Don't pre-compute the hash as mp_map_lookup recomputes it
        .len = ESP_NOW_ETH_ALEN,
        .data = peer, // Points to memory on the stack
    };
    mp_map_t *map = mp_obj_dict_get_map(self->peers_table);
    // First lookup the peer without allocating new memory
    mp_map_elem_t *item = mp_map_lookup(map, &peer_obj, MP_MAP_LOOKUP);
    if (item == NULL) {
        // If not found, add the peer using a new bytestring
        map->is_fixed = 0;      // Allow to modify the dict
        item = mp_map_lookup(map,
            mp_obj_new_bytes(peer, ESP_NOW_ETH_ALEN),
            MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
        map->is_fixed = 1;      // Relock the dict
    }
    return item;
}

// Update the peers table with the new rssi value from a received pkt and
// return a reference to the item in the peers_table.
static mp_map_elem_t *_update_rssi(
    esp_espnow_obj_t *self, const uint8_t *peer,
    int8_t rssi, uint32_t timestamp_ms) {

    // Lookup the peer in the device table
    mp_map_elem_t *item = _lookup_add_peer(self, peer);

    // Ensure the item value is a list with at least 2 entries
    mp_obj_list_t *list = MP_OBJ_TO_PTR(item->value);
    if (list == NULL ||
        !mp_obj_is_type(list, &mp_type_list) ||
        list->len < 2) {
        item->value = mp_obj_new_list(2,
            (mp_obj_t [2]) {mp_const_none, mp_const_none});
        list = MP_OBJ_TO_PTR(item->value);
    }

    // Update the values
    // item->key = peer MAC Address (byte string)
    // item->value = [rssi, timestamp_ms]
    list->items[0] = MP_OBJ_NEW_SMALL_INT(rssi);
    list->items[1] = MP_OBJ_NEW_SMALL_INT(timestamp_ms);

    return item;
}

// ### Handling espnow packets in the recv buffer
//

// ### Send and Receive ESP_Now data
//

// Wait for a message to arrive in the buffer and read out the packet header.
// Also checks the packet header and updates the rssi values in the device
// table. This is the code that is common to espnow_irecv() and espnow_recv().
// Returns: Length of the message to read (or 0 on timeout)
// Raises: ValueError if the header format is bad.
static int _recv_hdr(size_t n_args, const mp_obj_t *args, mp_obj_t *peer) {

    esp_espnow_obj_t *self = _get_singleton(INITIALISED);

    size_t timeout_ms = (
        (n_args > 1) ? mp_obj_get_int(args[1]) : self->recv_timeout_ms);

    // Read the packet header from the incoming buffer
    espnow_pkt_t pkt;
    if (!buffer_recv(self->recv_buffer, &pkt, sizeof(pkt), timeout_ms)) {
        return 0;    // Timeout waiting for packet
    }
    // Check the message packet header format
    if (pkt.hdr.magic != ESPNOW_MAGIC ||
        pkt.hdr.msg_len > ESP_NOW_MAX_DATA_LEN) {
        mp_raise_ValueError(MP_ERROR_TEXT("ESP-Now: Bad packet"));
    }

    // Update the rssi value in device table and use the dict key as peer
    *peer = _update_rssi(
        self, pkt.peer, pkt.hdr.rssi, pkt.hdr.timestamp_ms)->key;

    return pkt.hdr.msg_len;
}

// The tuple returned by recv()/irecv() on timeout: (None, None)
static const mp_rom_obj_tuple_t none_tuple = {
    {&mp_type_tuple}, 2, {mp_const_none, mp_const_none}
};

// ESPNow.irecv([timeout]):
// Like ESPNow.recv() but returns a "callee-owned" tuple of byte strings.
// This provides an allocation-free way to read successive messages.
// Beware: the tuple and bytestring storage is re-used between all calls
// to irecv().
// Takes an optional "timeout" argument in milliseconds.
// Default timeout is set with ESPNow.config(timeout=milliseconds).
// Returns (None, None) on timeout.
STATIC mp_obj_t espnow_irecv(size_t n_args, const mp_obj_t *args) {
    esp_espnow_obj_t *self = _get_singleton(INITIALISED);

    // Initialise the callee-owned irecv_tuple in case exception is raised
    self->irecv_tuple->items[0] = mp_const_none;
    mp_obj_str_t *msg = MP_OBJ_TO_PTR(self->irecv_tuple->items[1]);
    msg->hash = 0;
    msg->len = 0;
    msg->len = _recv_hdr(n_args, args, &self->irecv_tuple->items[0]);
    if (msg->len == 0) {
        return MP_OBJ_FROM_PTR(&none_tuple);    // Timeout waiting for packet
    }

    // Now read the message into the byte string.
    if (!buffer_get(self->recv_buffer, (byte *)msg->data, msg->len)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Buffer error"));
    }

    return MP_OBJ_FROM_PTR(self->irecv_tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espnow_irecv_obj, 1, 2, espnow_irecv);

// ESPNow.recv([timeout]):
// Returns a tuple of byte strings: (peer_addr, message) where peer_addr is
// the MAC address of the sending peer.
// Takes an optional "timeout" argument in milliseconds.
// Default timeout is set with ESPNow.config(timeout=milliseconds).
// Return (None, None) on timeout.
STATIC mp_obj_t espnow_recv(size_t n_args, const mp_obj_t *args) {
    esp_espnow_obj_t *self = _get_singleton(INITIALISED);

    mp_obj_t peer;
    int msg_len = _recv_hdr(n_args, args, &peer);
    if (msg_len == 0) {
        return MP_OBJ_FROM_PTR(&none_tuple);    // Timeout waiting for packet
    }

    // Allocate vstr as new storage buffers for the message.
    // The storage will be handed over to mp_obj_new_str_from_vstr() below.
    vstr_t msg;
    vstr_init_len(&msg, msg_len);
    // Now read the peer_address and message into the byte strings.
    if (!buffer_get(self->recv_buffer, msg.buf, msg_len)) {
        vstr_clear(&msg);
        mp_raise_ValueError(MP_ERROR_TEXT("Buffer error"));
    }

    // Create and return a tuple of byte strings: (mac_addr, message)
    return NEW_TUPLE(peer, mp_obj_new_str_from_vstr(&mp_type_bytes, &msg));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espnow_recv_obj, 1, 2, espnow_recv);

// Test if data is available to read from the buffers
STATIC mp_obj_t espnow_any(const mp_obj_t _) {
    esp_espnow_obj_t *self = _get_singleton(INITIALISED);

    return buffer_empty(self->recv_buffer) ? mp_const_false : mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_any_obj, espnow_any);

// Used by _do_espnow_send() for sends() with sync==True.
// Wait till all pending sent packet responses have been received.
// ie. self->tx_responses == self->tx_packets.
// Return the number of responses where status != ESP_NOW_SEND_SUCCESS.
static void _wait_for_pending_responses(esp_espnow_obj_t *self) {
    int64_t start = mp_hal_ticks_ms();
    // Note: the send timeout is just a fallback - in normal operation
    // we should never reach that timeout.
    while (self->tx_responses < self->tx_packets &&
           (mp_hal_ticks_ms() - start) <= DEFAULT_SEND_TIMEOUT_MS) {
        // Won't yield unless delay > portTICK_PERIOD_MS (10ms)
        mp_hal_delay_ms(BUSY_WAIT_MS);
    }
    if (self->tx_responses != self->tx_packets) {
        mp_raise_ValueError(MP_ERROR_TEXT("Send timeout on synch."));
    }
}

// Send an ESPNow message to the peer_addr and optionally wait for the
// send response.
// Returns the number of "Not received" responses (which may be more than
// one if the send is to all peers).
static int _do_espnow_send(
    esp_espnow_obj_t *self, const uint8_t *peer_addr,
    const uint8_t *message, size_t length, bool sync) {

    if (sync) {
        // Flush out any pending responses.
        // If the last call was sync==False there may be outstanding responses
        // still to be received (possible many if we just had a burst of
        // unsync send()s). We need to wait for all pending responses if this
        // call has sync=True.
        _wait_for_pending_responses(self);
    }
    int saved_failures = self->tx_failures;
    // Send the packet - try, try again if internal esp-now buffers are full.
    esp_err_t err;
    int64_t start = mp_hal_ticks_ms();
    while ((ESP_ERR_ESPNOW_NO_MEM ==
            (err = esp_now_send(peer_addr, message, length))) &&
           (mp_hal_ticks_ms() - start) <= DEFAULT_SEND_TIMEOUT_MS) {
        // Won't yield unless delay > portTICK_PERIOD_MS (10ms)
        mp_hal_delay_ms(BUSY_WAIT_MS);
    }
    check_esp_err(err);           // Will raise OSError if e != ESP_OK
    // Increment the sent packet count. If peer_addr==NULL msg will be
    // sent to all peers EXCEPT any broadcast or multicast addresses.
    self->tx_packets += ((peer_addr == NULL) ? self->peer_count : 1);
    if (sync) {
        // Wait for and tally all the expected responses from peers
        _wait_for_pending_responses(self);
    }
    // Return number of non-responsive peers.
    return self->tx_failures - saved_failures;
}

// Return C pointer to byte memory string/bytes/bytearray in obj.
// Raise ValueError if the length does not match expected len.
static const uint8_t *_get_bytes_len(mp_obj_t obj, size_t len) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(obj, &bufinfo, MP_BUFFER_READ);
    if (bufinfo.len != len) {
        mp_raise_ValueError(MP_ERROR_TEXT("wrong length"));
    }
    return (const uint8_t *)bufinfo.buf;
}

// Return C pointer to the MAC address.
// Raise ValueError if mac_addr is wrong type or is not 6 bytes long.
static const uint8_t *_get_peer(mp_obj_t mac_addr) {
    return mp_obj_is_true(mac_addr)
        ? _get_bytes_len(mac_addr, ESP_NOW_ETH_ALEN) : NULL;
}

// ESPNow.send(peer_addr, message, [sync (=true)])
// ESPNow.send(message)
// Send a message to the peer's mac address. Optionally wait for a response.
// If peer_addr == None or any non-true value, send to all registered peers.
// If sync == True, wait for response after sending.
// Returns:
//   True  if sync==False and message sent successfully.
//   True  if sync==True and message is received successfully by all recipients
//   False if sync==True and message is not received by at least one recipient
// Raises: EAGAIN if the internal espnow buffers are full.
STATIC mp_obj_t espnow_send(size_t n_args, const mp_obj_t *args) {
    esp_espnow_obj_t *self = _get_singleton(INITIALISED);
    // Check the various combinations of input arguments
    mp_obj_t peer = (n_args > 2) ? args[1] : mp_const_none;
    mp_obj_t msg = (n_args > 2) ? args[2] : (n_args == 2) ? args[1] : MP_OBJ_NULL;
    mp_obj_t sync = (n_args > 3) ? args[3] : mp_const_true;

    // Get a pointer to the data buffer of the message
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(msg, &bufinfo, MP_BUFFER_READ);

    int failed_responses = _do_espnow_send(
        self, _get_peer(peer), bufinfo.buf, bufinfo.len, mp_obj_is_true(sync));
    return (failed_responses == 0) ? mp_const_true : mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espnow_send_obj, 2, 4, espnow_send);

// ### The ESP_Now send and recv callback routines
//

// Callback triggered when a sent packet is acknowledged by the peer (or not).
// Just count the number of responses and number of failures.
// These are used in the send() logic.
STATIC void send_cb(
    const uint8_t *mac_addr, esp_now_send_status_t status) {

    esp_espnow_obj_t *self = _get_singleton(0);
    self->tx_responses++;
    if (status != ESP_NOW_SEND_SUCCESS) {
        self->tx_failures++;
    }
}

static mp_obj_t espnow_recv_cb_wrapper(mp_obj_t _) {
    esp_espnow_obj_t *self = _get_singleton(0);
    mp_obj_t args[] = {self, MP_OBJ_NEW_SMALL_INT(0)}; // Set timeout to 0
    mp_obj_t data = mp_call_function_n_kw(      // Read a message from buffer
        MP_OBJ_FROM_PTR(&espnow_irecv_obj), 2, 0, args);
    mp_call_function_2(
        self->recv_cb, MP_OBJ_NEW_SMALL_INT(EVENT_RECV_DATA), data);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_recv_cb_wrapper_obj, espnow_recv_cb_wrapper);

// Callback triggered when an ESP-Now packet is received.
// Write the peer MAC address and the message into the recv_buffer as an
// ESPNow packet.
// If the buffer is full, drop the message and increment the dropped count.
// Schedules the user callback if one has been registered (ESPNow.config()).
STATIC void recv_cb(
    const uint8_t *mac_addr, const uint8_t *msg, int msg_len) {

    esp_espnow_obj_t *self = _get_singleton(0);
    buffer_t buf = self->recv_buffer;
    if (sizeof(espnow_pkt_t) + msg_len >= buffer_free(buf)) {
        self->dropped_rx_pkts++;
        return;
    }
    espnow_hdr_t header;
    header.magic = ESPNOW_MAGIC;
    header.msg_len = msg_len;
    header.rssi = _get_rssi_from_wifi_pkt(msg);
    header.timestamp_ms = mp_hal_ticks_ms();

    buffer_put(buf, &header, sizeof(header));
    buffer_put(buf, mac_addr, ESP_NOW_ETH_ALEN);
    buffer_put(buf, msg, msg_len);
    self->rx_packets++;
    if (self->recv_cb != mp_const_none) {
        mp_sched_schedule(MP_OBJ_FROM_PTR(&espnow_recv_cb_wrapper_obj), self);
    }
}

// ### Peer Management Functions
//

// Set the ESP-NOW Primary Master Key (pmk) (for encrypted communications).
// Raise OSError if ESP-NOW functions are not initialised.
// Raise ValueError if key is not a bytes-like object exactly 16 bytes long.
STATIC mp_obj_t espnow_set_pmk(mp_obj_t _, mp_obj_t key) {
    check_esp_err(esp_now_set_pmk(_get_bytes_len(key, ESP_NOW_KEY_LEN)));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_set_pmk_obj, espnow_set_pmk);

// Common code for add_peer() and mod_peer() to process the args and kw_args:
// Raise ValueError if the LMK is not a bytes-like object of exactly 16 bytes.
// Raise TypeError if invalid keyword args or too many positional args.
// Return true if all args parsed correctly.
STATIC bool _update_peer_info(
    esp_now_peer_info_t *peer, size_t n_args,
    const mp_obj_t *pos_args, mp_map_t *kw_args) {

    enum { ARG_lmk, ARG_channel, ARG_ifidx, ARG_encrypt };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_lmk, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_channel, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_ifidx, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_encrypt, MP_ARG_BOOL, {.u_bool = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    if (args[ARG_lmk].u_obj != MP_OBJ_NULL) {
        mp_obj_t obj = args[ARG_lmk].u_obj;
        peer->encrypt = mp_obj_is_true(obj);
        if (peer->encrypt) {
            // Key must be 16 bytes in length.
            memcpy(peer->lmk,
                _get_bytes_len(obj, ESP_NOW_KEY_LEN),
                ESP_NOW_KEY_LEN);
        }
    }
    if (args[ARG_channel].u_int != -1) {
        peer->channel = args[ARG_channel].u_int;
    }
    if (args[ARG_ifidx].u_int != -1) {
        peer->ifidx = args[ARG_ifidx].u_int;
    }
    if (args[ARG_encrypt].u_obj != MP_OBJ_NULL) {
        peer->encrypt = args[ARG_encrypt].u_bool;
    }
    return true;
}

// Update the cached peer count in self->peer_count;
// The peer_count ignores broadcast and multicast addresses and is used for the
// send() logic and is updated from add_peer(), mod_peer() and del_peer().
STATIC void _update_peer_count() {
    esp_espnow_obj_t *self = _get_singleton(INITIALISED);

    esp_now_peer_info_t peer = {0};
    bool from_head = true;
    int count = 0;
    // esp_now_fetch_peer() skips over any broadcast or multicast addresses
    while (esp_now_fetch_peer(from_head, &peer) == ESP_OK) {
        from_head = false;
        if (++count >= ESP_NOW_MAX_TOTAL_PEER_NUM) {
            break;          // Should not happen
        }
    }
    self->peer_count = count;
}

// ESPNow.add_peer(peer_mac, [lmk, [channel, [ifidx, [encrypt]]]]) or
// ESPNow.add_peer(peer_mac, [lmk=b'0123456789abcdef'|b''|None|False],
//          [channel=1..11|0], [ifidx=0|1], [encrypt=True|False])
// Positional args set to None will be left at defaults.
// Raise OSError if ESPNow.init() has not been called.
// Raise ValueError if mac or LMK are not bytes-like objects or wrong length.
// Raise TypeError if invalid keyword args or too many positional args.
// Return None.
STATIC mp_obj_t espnow_add_peer(
    size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {

    esp_now_peer_info_t peer = {0};
    memcpy(peer.peer_addr, _get_peer(args[1]), ESP_NOW_ETH_ALEN);
    _update_peer_info(&peer, n_args - 2, args + 2, kw_args);

    check_esp_err(esp_now_add_peer(&peer));
    _update_peer_count();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(espnow_add_peer_obj, 2, espnow_add_peer);

// ESPNow.mod_peer(peer_mac, [lmk, [channel, [ifidx, [encrypt]]]]) or
// ESPNow.mod_peer(peer_mac, [lmk=b'0123456789abcdef'|b''|None|False],
//          [channel=1..11|0], [ifidx=0|1], [encrypt=True|False])
// Positional args set to None will be left at current values.
// Raise OSError if ESPNow.init() has not been called.
// Raise ValueError if mac or LMK are not bytes-like objects or wrong length.
// Raise TypeError if invalid keyword args or too many positional args.
// Return None.
STATIC mp_obj_t espnow_mod_peer(
    size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {

    esp_now_peer_info_t peer = {0};
    memcpy(peer.peer_addr, _get_peer(args[1]), ESP_NOW_ETH_ALEN);
    check_esp_err(esp_now_get_peer(peer.peer_addr, &peer));

    _update_peer_info(&peer, n_args - 2, args + 2, kw_args);

    check_esp_err(esp_now_mod_peer(&peer));
    _update_peer_count();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(espnow_mod_peer_obj, 2, espnow_mod_peer);

// Convert a peer_info struct to python tuple
// Used by espnow_get_peer() and espnow_get_peers()
static mp_obj_t peer_info_to_tuple(const esp_now_peer_info_t *peer) {
    return NEW_TUPLE(
        mp_obj_new_bytes(peer->peer_addr, MP_ARRAY_SIZE(peer->peer_addr)),
        mp_obj_new_bytes(peer->lmk, MP_ARRAY_SIZE(peer->lmk)),
        mp_obj_new_int(peer->channel),
        mp_obj_new_int(peer->ifidx),
        (peer->encrypt) ? mp_const_true : mp_const_false);
}

// ESPNow.get_peer(peer_mac): Get the peer info for peer_mac as a tuple.
// Raise OSError if ESPNow.init() has not been called.
// Raise ValueError if mac or LMK are not bytes-like objects or wrong length.
// Return a tuple of (peer_addr, lmk, channel, ifidx, encrypt).
STATIC mp_obj_t espnow_get_peer(mp_obj_t _, mp_obj_t arg1) {
    esp_now_peer_info_t peer = {0};
    memcpy(peer.peer_addr, _get_peer(arg1), ESP_NOW_ETH_ALEN);

    check_esp_err(esp_now_get_peer(peer.peer_addr, &peer));

    return peer_info_to_tuple(&peer);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_get_peer_obj, espnow_get_peer);

// ESPNow.del_peer(peer_mac): Unregister peer_mac.
// Raise OSError if ESPNow.init() has not been called.
// Raise ValueError if peer is not a bytes-like objects or wrong length.
// Return None.
STATIC mp_obj_t espnow_del_peer(mp_obj_t _, mp_obj_t peer) {
    uint8_t peer_addr[ESP_NOW_ETH_ALEN];
    memcpy(peer_addr, _get_peer(peer), ESP_NOW_ETH_ALEN);

    check_esp_err(esp_now_del_peer(peer_addr));
    _update_peer_count();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_del_peer_obj, espnow_del_peer);

// ESPNow.get_peers(): Fetch peer_info records for all registered ESPNow peers.
// Raise OSError if ESPNow.init() has not been called.
// Return a tuple of tuples:
//     ((peer_addr, lmk, channel, ifidx, encrypt),
//      (peer_addr, lmk, channel, ifidx, encrypt), ...)
STATIC mp_obj_t espnow_get_peers(mp_obj_t _) {
    esp_espnow_obj_t *self = _get_singleton(INITIALISED);

    // Build and initialise the peer info tuple.
    mp_obj_tuple_t *peerinfo_tuple = mp_obj_new_tuple(self->peer_count, NULL);
    esp_now_peer_info_t peer = {0};
    for (int i = 0; i < peerinfo_tuple->len; i++) {
        int status = esp_now_fetch_peer((i == 0), &peer);
        peerinfo_tuple->items[i] =
            (status == ESP_OK ? peer_info_to_tuple(&peer) : mp_const_none);
    }

    return peerinfo_tuple;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_get_peers_obj, espnow_get_peers);

// ESPNow.espnow_peer_count(): Get the number of registered peers.
// Raise OSError if ESPNow.init() has not been called.
// Return a tuple of (num_total_peers, num_encrypted_peers).
STATIC mp_obj_t espnow_peer_count(mp_obj_t _) {
    esp_now_peer_num_t peer_num = {0};
    check_esp_err(esp_now_get_peer_num(&peer_num));

    return NEW_TUPLE(
        mp_obj_new_int(peer_num.total_num),
        mp_obj_new_int(peer_num.encrypt_num));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_peer_count_obj, espnow_peer_count);

STATIC const mp_rom_map_elem_t esp_espnow_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&espnow_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&espnow_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&espnow_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_stats), MP_ROM_PTR(&espnow_stats_obj) },

    // Send and receive messages
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&espnow_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_irecv), MP_ROM_PTR(&espnow_irecv_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&espnow_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&espnow_any_obj) },

    // Peer management functions
    { MP_ROM_QSTR(MP_QSTR_set_pmk), MP_ROM_PTR(&espnow_set_pmk_obj) },
    { MP_ROM_QSTR(MP_QSTR_add_peer), MP_ROM_PTR(&espnow_add_peer_obj) },
    { MP_ROM_QSTR(MP_QSTR_mod_peer), MP_ROM_PTR(&espnow_mod_peer_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_peer), MP_ROM_PTR(&espnow_get_peer_obj) },
    { MP_ROM_QSTR(MP_QSTR_del_peer), MP_ROM_PTR(&espnow_del_peer_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_peers), MP_ROM_PTR(&espnow_get_peers_obj) },
    { MP_ROM_QSTR(MP_QSTR_peer_count), MP_ROM_PTR(&espnow_peer_count_obj) },
};
STATIC MP_DEFINE_CONST_DICT(esp_espnow_locals_dict, esp_espnow_locals_dict_table);

STATIC const mp_rom_map_elem_t espnow_globals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_espnow) },
    { MP_ROM_QSTR(MP_QSTR_ESPNow), MP_ROM_PTR(&esp_espnow_type) },
    { MP_ROM_QSTR(MP_QSTR_MAX_DATA_LEN), MP_ROM_INT(ESP_NOW_MAX_DATA_LEN)},
    { MP_ROM_QSTR(MP_QSTR_KEY_LEN), MP_ROM_INT(ESP_NOW_KEY_LEN)},
    { MP_ROM_QSTR(MP_QSTR_MAX_TOTAL_PEER_NUM), MP_ROM_INT(ESP_NOW_MAX_TOTAL_PEER_NUM)},
    { MP_ROM_QSTR(MP_QSTR_MAX_ENCRYPT_PEER_NUM), MP_ROM_INT(ESP_NOW_MAX_ENCRYPT_PEER_NUM)},
    { MP_ROM_QSTR(MP_QSTR_RECV_DATA), MP_ROM_INT(EVENT_RECV_DATA)},
};
STATIC MP_DEFINE_CONST_DICT(espnow_globals_dict, espnow_globals_dict_table);

// ### Dummy Buffer Protocol support
// ...so asyncio can poll.ipoll() on this device

// Support ioctl(MP_STREAM_POLL, ) for asyncio
STATIC mp_uint_t espnow_stream_ioctl(mp_obj_t self_in, mp_uint_t request,
    uintptr_t arg, int *errcode) {
    if (request != MP_STREAM_POLL) {
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }
    esp_espnow_obj_t *self = _get_singleton(0);
    return (self->recv_buffer == NULL) ? 0 :  // If not initialised
           arg & (
               // If no data in the buffer, unset the Read ready flag
               (buffer_empty(self->recv_buffer) ? 0: MP_STREAM_POLL_RD) |
               // If still waiting for responses, unset the Write ready flag
               (self->tx_responses < self->tx_packets ? 0: MP_STREAM_POLL_WR));
}

STATIC const mp_stream_p_t espnow_stream_p = {
    .ioctl = espnow_stream_ioctl,
};

// Return reference to the dictionary of peers we have seen:
//   {peer1: (rssi, time_sec), peer2: (rssi, time_msec), ...}
// where:
//   peerX is a byte string containing the 6-byte mac address of the peer,
//   rssi is the wifi signal strength from the last msg received
//       (in dBm from -127 to 0)
//   time_sec is the time in milliseconds since device last booted.
STATIC void espnow_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    esp_espnow_obj_t *self = _get_singleton(0);
    if (dest[0] != MP_OBJ_NULL) {   // Only allow "Load" operation
        return;
    }
    if (attr == MP_QSTR_peers) {
        dest[0] = self->peers_table;
        return;
    }
    dest[1] = MP_OBJ_SENTINEL;  // Attribute not found
}

// Iterating over ESPNow returns tuples of (peer_addr, message)...
STATIC mp_obj_t espnow_iternext(mp_obj_t self_in) {
    esp_espnow_obj_t *self = _get_singleton(0);
    return (self->recv_buffer != NULL)
                ? espnow_irecv(1, &self_in)
                : MP_OBJ_STOP_ITERATION;
}

const mp_obj_type_t esp_espnow_type = {
    { &mp_type_type },
    .name = MP_QSTR_ESPNow,
    .make_new = espnow_make_new,
    .attr = espnow_attr,
    .getiter = mp_identity_getiter,
    .iternext = espnow_iternext,
    .protocol = &espnow_stream_p,
    .locals_dict = (mp_obj_t)&esp_espnow_locals_dict,
};

const mp_obj_module_t mp_module_esp_espnow = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&espnow_globals_dict,
};
