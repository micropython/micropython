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

#include "esp_now.h"

#include "py/runtime.h"
#include "py/objarray.h"
#include "py/objproperty.h"
#include "py/stream.h"
#include "py/ringbuf.h"

#include "mphalport.h"

#include "bindings/espnow/__init__.h"
#include "bindings/espnow/Now.h"

#include "shared-bindings/util.h"
#include "shared-bindings/wifi/__init__.h"

// Relies on gcc Variadic Macros and Statement Expressions
#define NEW_TUPLE(...) ({mp_obj_t _z[] = {__VA_ARGS__}; mp_obj_new_tuple(MP_ARRAY_SIZE(_z), _z);})

#define ESPNOW_MAGIC 0x99

// The maximum length of an espnow packet (bytes)
#define MAX_PACKET_LEN (sizeof(espnow_pkt_t) + ESP_NOW_MAX_DATA_LEN)

// Enough for 2 full-size packets: 2 * (6 + 7 + 250) = 526 bytes
// Will allocate an additional 7 bytes for buffer overhead
#define DEFAULT_RECV_BUFFER_SIZE (2 * MAX_PACKET_LEN)

// Default timeout (millisec) to wait for incoming ESPNow messages (5 minutes).
#define DEFAULT_RECV_TIMEOUT_MS (5 * 60 * 1000)

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
} __attribute__((packed)) espnow_hdr_t;

typedef struct {
    espnow_hdr_t hdr;           // The header
    uint8_t peer[6];            // Peer address
    uint8_t msg[0];             // Message is up to 250 bytes
} __attribute__((packed)) espnow_pkt_t;

// The data structure for the espnow_singleton.
typedef struct _espnow_obj_t {
    mp_obj_base_t base;
    ringbuf_t *recv_buffer;         // A buffer for received packets
    size_t recv_buffer_size;        // The size of the recv_buffer
    size_t recv_timeout_ms;         // Timeout for recv()
    volatile size_t rx_packets;     // # of received packets
    volatile size_t rx_failures;    // # of dropped packets (buffer full)
    size_t tx_packets;              // # of sent packets
    volatile size_t tx_responses;   // # of sent packet responses received
    volatile size_t tx_failures;    // # of sent packet responses failed
    size_t num_peers;               // Cache the # of peers for send(sync=True)
    mp_obj_t peers_table;           // A dictionary of discovered peers
} espnow_obj_t;

static void check_esp_err(esp_err_t status) {
    if (status != ESP_OK) {
        mp_raise_RuntimeError(translate("an error occured"));
    }
}

// --- Initialisation and Config functions ---

// Return a pointer to the ESPNow module singleton
// If state == INITIALISED check the device has been initialised.
// Raises OSError if not initialised and state == INITIALISED.
static espnow_obj_t *_get_singleton(void) {
    return MP_STATE_PORT(espnow_singleton);
}

static bool espnow_deinited(espnow_obj_t *self) {
    return self->recv_buffer == NULL;
}

// Return a pointer to the ESPNow module singleton
// If state == INITIALISED check the device has been initialised.
// Raises OSError if not initialised and state == INITIALISED.
static void check_for_deinit(espnow_obj_t *self) {
    if (espnow_deinited(self)) {
        raise_deinited_error();
    }
}

//| class Now:
//|     def __init__(self) -> Now:
//|     """
//|         Allocate and initialise the ESPNow module as a singleton.
//|         Returns the initialised espnow_singleton.
//|     """
//|     ...
//|
STATIC mp_obj_t espnow_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    espnow_obj_t *self = _get_singleton();

    if (self != NULL) {
        return self;
    }

    self = m_new_obj(espnow_obj_t);
    self->base.type = &espnow_type;
    self->recv_buffer_size = DEFAULT_RECV_BUFFER_SIZE;
    self->recv_timeout_ms = DEFAULT_RECV_TIMEOUT_MS;
    self->recv_buffer = NULL;
    self->peers_table = mp_obj_new_dict(0);

    // Prevent user code modifying the dict
    mp_obj_dict_get_map(self->peers_table)->is_fixed = 1;

    // Set the global singleton pointer for the espnow protocol.
    MP_STATE_PORT(espnow_singleton) = self;

    return self;
}

// --- The ESP-NOW send and recv callback routines ---

// Callback triggered when a sent packet is acknowledged by the peer (or not).
// Just count the number of responses and number of failures.
// These are used in the send() logic.
static void send_cb(const uint8_t *mac_addr, esp_now_send_status_t status) {
    espnow_obj_t *self = _get_singleton();
    self->tx_responses++;
    if (status != ESP_NOW_SEND_SUCCESS) {
        self->tx_failures++;
    }
}

static inline int8_t _get_rssi_from_wifi_pkt(const uint8_t *msg);

// Callback triggered when an ESP-Now packet is received.
// Write the peer MAC address and the message into the recv_buffer as an ESPNow packet.
// If the buffer is full, drop the message and increment the dropped count.
// Schedules the user callback if one has been registered (ESPNow.config()).
static void recv_cb(const uint8_t *mac_addr, const uint8_t *msg, int msg_len) {
    espnow_obj_t *self = _get_singleton();
    ringbuf_t *buf = self->recv_buffer;

    // TODO: Test this works with ">".
    if (sizeof(espnow_pkt_t) + msg_len >= ringbuf_free(buf)) {
        self->rx_failures++;
        return;
    }

    espnow_hdr_t header;
    header.magic = ESPNOW_MAGIC;
    header.msg_len = msg_len;
    header.rssi = _get_rssi_from_wifi_pkt(msg);
    header.time_ms = mp_hal_ticks_ms();

    ringbuf_write(buf, &header, sizeof(header));
    ringbuf_write(buf, mac_addr, ESP_NOW_ETH_ALEN);
    ringbuf_write(buf, msg, msg_len);

    self->rx_packets++;
}

static void _wifi_init(void) {
    if (!common_hal_wifi_radio_get_enabled(&common_hal_wifi_radio_obj)) {
        common_hal_wifi_init(false);
        common_hal_wifi_radio_set_enabled(&common_hal_wifi_radio_obj, true);
    }
}

//|     def init() -> None:
//|     """
//|         Initialise the data buffers and ESP-NOW functions.
//|         Initialise the Espressif ESP-NOW software stack, register callbacks
//|         and allocate the recv data buffers.
//|     """
//|     ...
//|
static void espnow_init(espnow_obj_t *self) {
    if (espnow_deinited(self)) {    // Already initialised
        self->recv_buffer = m_new_obj(ringbuf_t);
        if (!ringbuf_alloc(self->recv_buffer, self->recv_buffer_size, true)) {
            m_malloc_fail(self->recv_buffer_size);
        }

        _wifi_init();   // Call the wifi init code

        check_esp_err(esp_now_init());
        check_esp_err(esp_now_register_send_cb(send_cb));
        check_esp_err(esp_now_register_recv_cb(recv_cb));
    }
}

//|     def deinit() -> None:
//|     """
//|         De-initialise the ESP-NOW software stack, disable callbacks
//|         and deallocate the recv data buffers.
//|     """
//|     ...
//|
static void espnow_deinit(espnow_obj_t *self) {
    if (self != NULL && !espnow_deinited(self)) {
        check_esp_err(esp_now_unregister_send_cb());
        check_esp_err(esp_now_unregister_recv_cb());
        check_esp_err(esp_now_deinit());
        self->recv_buffer->buf = NULL;
        self->recv_buffer = NULL;
        self->num_peers = 0; // esp_now_deinit() removes all peers.
        self->tx_packets = self->tx_responses;
    }
}

void espnow_reset(void) {
    espnow_deinit(_get_singleton());
    MP_STATE_PORT(espnow_singleton) = NULL;
}

//|     def active(state: bool) -> bool:
//|     """
//|         Initialise or de-initialise the ESPNow communication protocol
//|         depending on the value of the flag optional argument.
//|     """
//|     ...
//|
STATIC mp_obj_t espnow_active(size_t n_args, const mp_obj_t *args) {
    espnow_obj_t *self = args[0];
    if (n_args > 1) {
        if (mp_obj_is_true(args[1])) {
            espnow_init(self);
        } else {
            espnow_deinit(self);
        }
    }
    return mp_obj_new_bool(!espnow_deinited(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espnow_active_obj, 1, 2, espnow_active);

//|     def config(['param'|param=value, ..]) -> int:
//|     """
//|         Get or set configuration values. Supported config params:
//|         buffer: size of buffer for rx packets (default=514 bytes)
//|         timeout: Default read timeout (default=300,000 milliseconds)
//|     """
//|     ...
//|
STATIC mp_obj_t espnow_config(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    espnow_obj_t *self = pos_args[0];
    enum { ARG_get, ARG_buffer, ARG_timeout, ARG_rate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_buffer, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_buffer].u_int >= 0) {
        self->recv_buffer_size = args[ARG_buffer].u_int;
    }

    if (args[ARG_timeout].u_int >= 0) {
        self->recv_timeout_ms = args[ARG_timeout].u_int;
    }

    if (args[ARG_rate].u_int >= 0) {
        _wifi_init();  // Call the wifi init code
        check_esp_err(esp_wifi_config_espnow_rate(ESP_IF_WIFI_STA, args[ARG_rate].u_int));
        check_esp_err(esp_wifi_config_espnow_rate(ESP_IF_WIFI_AP, args[ARG_rate].u_int));
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

//|     stats: Tuple[int, int, int, int, int]
//|     """Provide some useful stats.
//|        Returns a tuple of (tx_packets, tx_responses, tx_failures, rx_packets, rx_failures).
//|     """
//|
STATIC mp_obj_t espnow_get_stats(mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return NEW_TUPLE(
        mp_obj_new_int(self->tx_packets),
        mp_obj_new_int(self->tx_responses),
        mp_obj_new_int(self->tx_failures),
        mp_obj_new_int(self->rx_packets),
        mp_obj_new_int(self->rx_failures));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_get_stats_obj, espnow_get_stats);

MP_PROPERTY_GETTER(espnow_stats_obj,
    (mp_obj_t)&espnow_get_stats_obj);

// --- Maintaining the peer table and reading RSSI values ---
//
// We maintain a peers table for several reasons, to:
// - support monitoring the RSSI values for all peers; and
// - to return unique bytestrings for each peer which supports more efficient
//   application memory usage and peer handling.

// Get the RSSI value from the wifi packet header
static inline int8_t _get_rssi_from_wifi_pkt(const uint8_t *msg) {
    // Warning: Secret magic to get the rssi from the wifi packet header
    // See espnow.c:espnow_recv_cb() at https://github.com/espressif/esp-now/
    // In the wifi packet the msg comes after a wifi_promiscuous_pkt_t
    // and a espnow_frame_format_t.
    // Backtrack to get a pointer to the wifi_promiscuous_pkt_t.
    static const size_t sizeof_espnow_frame_format = 39;
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    wifi_promiscuous_pkt_t *wifi_pkt = (wifi_promiscuous_pkt_t *)(
        msg - sizeof_espnow_frame_format - sizeof(wifi_promiscuous_pkt_t));
    #pragma GCC diagnostic pop
    return wifi_pkt->rx_ctrl.rssi;
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

// Update the peers table with the new rssi value from a received pkt and
// return a reference to the item in the peers_table.
static mp_map_elem_t *_update_rssi(espnow_obj_t *self, const uint8_t *peer, int8_t rssi, uint32_t time_ms) {
    // Lookup the peer in the device table
    mp_map_elem_t *item = _lookup_add_peer(self, peer);
    mp_obj_list_t *list = MP_OBJ_TO_PTR(item->value);
    list->items[0] = MP_OBJ_NEW_SMALL_INT(rssi);
    list->items[1] = mp_obj_new_int(time_ms);
    return item;
}

// --- Handling espnow packets in the recv buffer ---

// --- Send and Receive ESP_Now data ---

// Return C pointer to byte memory string/bytes/bytearray in obj.
// Raise ValueError if the length does not match expected len.
static uint8_t *_get_bytes_len_rw(mp_obj_t obj, size_t len, mp_uint_t rw) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(obj, &bufinfo, rw);
    mp_arg_validate_length(bufinfo.len, len, MP_QSTR_bytes);
    return (uint8_t *)bufinfo.buf;
}

static uint8_t *_get_bytes_len(mp_obj_t obj, size_t len) {
    return _get_bytes_len_rw(obj, len, MP_BUFFER_READ);
}

static uint8_t *_get_bytes_len_w(mp_obj_t obj, size_t len) {
    return _get_bytes_len_rw(obj, len, MP_BUFFER_WRITE);
}

// Return C pointer to the MAC address.
// Raise ValueError if mac_addr is wrong type or is not 6 bytes long.
static const uint8_t *_get_peer(mp_obj_t mac_addr) {
    return mp_obj_is_true(mac_addr) ? _get_bytes_len(mac_addr, ESP_NOW_ETH_ALEN) : NULL;
}

// Copy data from the ring buffer - wait if buffer is empty up to timeout_ms
static int ringbuf_read_wait(ringbuf_t *r, void *data, size_t len, int timeout_ms) {
    int64_t end = mp_hal_ticks_ms() + timeout_ms;
    int status = 0;
    while (
        ((status = ringbuf_read(r, data, len)) == 0) &&
        (end - (int64_t)mp_hal_ticks_ms()) >= 0) {
        RUN_BACKGROUND_TASKS;
    }
    return status;
}

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

//|     def send(peer_addr: bytes = None, message: Union[bytes, str], sync: bool = True) -> bool:
//|     """
//|         Send a message to the peer's mac address. Optionally wait for a response.
//|         If peer_addr == None or any non-true value, send to all registered peers.
//|         If sync == True, wait for response after sending.
//|         If size is provided it should be the number of bytes in message to send().
//|
//|         Returns:
//|             True  if sync == False and message sent successfully.
//|             True  if sync == True and message is received successfully by all recipients
//|             False if sync == True and message is not received by at least one recipient
//|
//|         Raises: EAGAIN if the internal espnow buffers are full.
//|     """
//|     ...
//|
STATIC mp_obj_t espnow_send(size_t n_args, const mp_obj_t *args) {
    espnow_obj_t *self = args[0];
    check_for_deinit(self);

    // Check the various combinations of input arguments
    const uint8_t *peer = (n_args > 2) ? _get_peer(args[1]) : NULL;
    mp_obj_t msg = (n_args > 2) ? args[2] : (n_args == 2) ? args[1] : MP_OBJ_NULL;
    bool sync = n_args <= 3 || args[3] == mp_const_none || mp_obj_is_true(args[3]);

    // Get a pointer to the data buffer of the message
    mp_buffer_info_t message;
    mp_get_buffer_raise(msg, &message, MP_BUFFER_READ);

    if (sync) {
        // Flush out any pending responses.
        // If the last call was sync==False there may be outstanding responses
        // still to be received (possible many if we just had a burst of
        // unsync send()s). We need to wait for all pending responses if this
        // call has sync=True.
        _wait_for_pending_responses(self);
    }

    size_t saved_failures = self->tx_failures;
    // Send the packet - try, try again if internal esp-now buffers are full.
    esp_err_t err;
    int64_t start = mp_hal_ticks_ms();
    while ((ESP_ERR_ESPNOW_NO_MEM == (err = esp_now_send(peer, message.buf, message.len))) &&
           (mp_hal_ticks_ms() - start) <= DEFAULT_SEND_TIMEOUT_MS) {
        RUN_BACKGROUND_TASKS;
    }
    check_esp_err(err);           // Will raise OSError if e != ESP_OK

    // Increment the sent packet count. If peer_addr==NULL msg will be
    // sent to all peers EXCEPT any broadcast or multicast addresses.
    self->tx_packets += ((peer == NULL) ? self->num_peers : 1);
    if (sync) {
        // Wait for and tally all the expected responses from peers
        _wait_for_pending_responses(self);
    }

    // Return False if sync and any peers did not respond.
    return mp_obj_new_bool(!(sync && self->tx_failures != saved_failures));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espnow_send_obj, 2, 4, espnow_send);

//|     def recv(buffers: List[bytearray(6), bytearray(250), ...], timeout: int) -> Optional[int]:
//|     """
//|         Waits for an espnow message and copies the peer_addr and message into the buffers list.
//|
//|         If buffers is 2 elements long, the peer_mac and message will be
//|         loaded into the 1st and 2nd elements.
//|         If buffers is 4 elements long, the rssi and timestamp values will be
//|         loaded into the 3rd and 4th elements.
//|
//|         Default timeout is set with ESPNow.config(timeout=milliseconds).
//|         Returns None on timeout otherwise length of the message.
//|     """
//|     ...
//|
STATIC mp_obj_t espnow_recv(size_t n_args, const mp_obj_t *args) {
    espnow_obj_t *self = args[0];
    check_for_deinit(self);

    size_t timeout_ms = ((n_args > 2 && args[2] != mp_const_none)
            ? (size_t)mp_obj_get_int(args[2]) : self->recv_timeout_ms);

    mp_obj_list_t *list = MP_OBJ_TO_PTR(args[1]);
    if (!mp_obj_is_type(list, &mp_type_list) || list->len < 2) {
        mp_arg_error_invalid(MP_QSTR_buffers);
    }

    mp_obj_array_t *msg = MP_OBJ_TO_PTR(list->items[1]);
    if (mp_obj_is_type(msg, &mp_type_bytearray)) {
        msg->len += msg->free;   // Make all the space in msg array available
        msg->free = 0;
    }

    uint8_t peer_buf[ESP_NOW_ETH_ALEN];
    uint8_t *msg_buf = _get_bytes_len_w(msg, ESP_NOW_MAX_DATA_LEN);

    // Read the packet header from the incoming buffer
    espnow_hdr_t hdr;
    if (ringbuf_read_wait(self->recv_buffer, &hdr, sizeof(hdr), timeout_ms) < 1) {
        return MP_OBJ_NEW_SMALL_INT(0);    // Timeout waiting for packet
    }
    int msg_len = hdr.msg_len;

    // Check the message packet header format and read the message data
    if (hdr.magic != ESPNOW_MAGIC ||
        msg_len > ESP_NOW_MAX_DATA_LEN ||
        ringbuf_read(self->recv_buffer, peer_buf, ESP_NOW_ETH_ALEN) < 1 ||
        ringbuf_read(self->recv_buffer, msg_buf, msg_len) < 1) {
        mp_arg_error_invalid(MP_QSTR_buffer);
    }
    if (mp_obj_is_type(msg, &mp_type_bytearray)) {
        // Set the length of the message bytearray.
        size_t size = msg->len + msg->free;
        msg->len = msg_len;
        msg->free = size - msg_len;
    }

    // Update rssi value in the peer device table
    mp_map_elem_t *entry = _update_rssi(self, peer_buf, hdr.rssi, hdr.time_ms);
    list->items[0] = entry->key;  // Set first element of list to peer
    if (list->len >= 4) {
        list->items[2] = MP_OBJ_NEW_SMALL_INT(hdr.rssi);
        list->items[3] = mp_obj_new_int(hdr.time_ms);
    }

    return MP_OBJ_NEW_SMALL_INT(msg_len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espnow_recv_obj, 2, 3, espnow_recv);

//|     def any(self) -> bool:
//|     """Test if data is available to read from the buffers.
//|     """
//|     ...
//|
STATIC mp_obj_t espnow_any(const mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return ringbuf_avail(self->recv_buffer) ? mp_const_true : mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_any_obj, espnow_any);

// --- Peer Management Functions ---

//|     def set_pmk(pmk: bytes(16)) -> None:
//|     """Set the ESP-NOW Primary Master Key (pmk) (for encrypted communications).
//|     """
//|     ...
//|
STATIC mp_obj_t espnow_set_pmk(mp_obj_t self_in, mp_obj_t key) {
    check_esp_err(esp_now_set_pmk(_get_bytes_len(key, ESP_NOW_KEY_LEN)));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_set_pmk_obj, espnow_set_pmk);

// Common code for add_peer() and mod_peer() to process the args and kw_args.
static void _update_peer_info(esp_now_peer_info_t *peer, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_lmk, ARG_channel, ARG_ifidx, ARG_encrypt };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_lmk,      MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_channel,  MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_ifidx,    MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_encrypt,  MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_lmk].u_obj != mp_const_none) {
        mp_obj_t obj = args[ARG_lmk].u_obj;
        peer->encrypt = mp_obj_is_true(obj);
        if (peer->encrypt) {
            // Key must be 16 bytes in length.
            memcpy(peer->lmk, _get_bytes_len(obj, ESP_NOW_KEY_LEN), ESP_NOW_KEY_LEN);
        }
    }

    if (args[ARG_channel].u_obj != mp_const_none) {
        peer->channel = mp_obj_get_int(args[ARG_channel].u_obj);
    }

    if (args[ARG_ifidx].u_obj != mp_const_none) {
        peer->ifidx = mp_obj_get_int(args[ARG_ifidx].u_obj);
    }

    if (args[ARG_encrypt].u_obj != mp_const_none) {
        peer->encrypt = mp_obj_is_true(args[ARG_encrypt].u_obj);
    }
}

// Update the cached peer count in self->num_peers;
// The num_peers ignores broadcast and multicast addresses and is used for the
// send() logic and is updated from add_peer(), mod_peer() and del_peer().
static void _update_peer_count(espnow_obj_t *self) {
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

    self->num_peers = count;
}

// ESPNow.add_peer(peer_mac, [lmk=b'0123456789abcdef'|b''|None|False], [channel=1..11|0], [ifidx=0|1], [encrypt=True|False])

//|     def add_peer(self, peer_mac: bytes(6), lmk: bytes, channel: int, ifidx: int, encrypt: bool) -> None:
//|     """
//|         Positional args set to None will be left at defaults.
//|         Raise ValueError if mac or LMK are not bytes-like objects or wrong length.
//|         Raise TypeError if invalid keyword args or too many positional args.
//|     """
//|     ...
//|
STATIC mp_obj_t espnow_add_peer(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    espnow_obj_t *self = args[0];
    check_for_deinit(self);

    esp_now_peer_info_t peer = {0};
    memcpy(peer.peer_addr, _get_peer(args[1]), ESP_NOW_ETH_ALEN);

    _update_peer_info(&peer, n_args - 2, args + 2, kw_args);
    check_esp_err(esp_now_add_peer(&peer));
    _update_peer_count(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(espnow_add_peer_obj, 2, espnow_add_peer);

// ESPNow.mod_peer(self, peer_mac, [lmk=b'0123456789abcdef'|b''|None|False], [channel=1..11|0], [ifidx=0|1], [encrypt=True|False])

//|     def mod_peer(self, peer_mac: bytes(6), lmk: bytes, channel: int, ifidx: int, encrypt: bool) -> None:
//|     """
//|         Positional args set to None will be left at current values.
//|         Raise ValueError if mac or LMK are not bytes-like objects or wrong length.
//|         Raise TypeError if invalid keyword args or too many positional args.
//|     """
//|     ...
//|
STATIC mp_obj_t espnow_mod_peer(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    espnow_obj_t *self = args[0];
    check_for_deinit(self);

    esp_now_peer_info_t peer = {0};
    memcpy(peer.peer_addr, _get_peer(args[1]), ESP_NOW_ETH_ALEN);
    check_esp_err(esp_now_get_peer(peer.peer_addr, &peer));

    _update_peer_info(&peer, n_args - 2, args + 2, kw_args);
    check_esp_err(esp_now_mod_peer(&peer));
    _update_peer_count(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(espnow_mod_peer_obj, 2, espnow_mod_peer);

//|     def del_peer(peer_mac: bytes(6)) -> None:
//|     """Un-register peer_mac.
//|     """
//|     ...
//|
STATIC mp_obj_t espnow_del_peer(mp_obj_t self_in, mp_obj_t peer) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    uint8_t peer_addr[ESP_NOW_ETH_ALEN];
    memcpy(peer_addr, _get_peer(peer), ESP_NOW_ETH_ALEN);

    check_esp_err(esp_now_del_peer(peer_addr));
    _update_peer_count(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_del_peer_obj, espnow_del_peer);

// Convert a peer_info struct to python tuple
// Used by espnow_get_peer() and espnow_get_peers()
static mp_obj_t _peer_info_to_tuple(const esp_now_peer_info_t *peer) {
    return NEW_TUPLE(
        mp_obj_new_bytes(peer->peer_addr, MP_ARRAY_SIZE(peer->peer_addr)),
        mp_obj_new_bytes(peer->lmk, MP_ARRAY_SIZE(peer->lmk)),
        mp_obj_new_int(peer->channel),
        mp_obj_new_int(peer->ifidx),
        (peer->encrypt) ? mp_const_true : mp_const_false);
}

//|     def get_peer(self, peer_mac: bytes(6)) -> Tuple[bytes, int, int, bool]:
//|     """
//|         Get the peer info for peer_mac as a tuple.
//|         Raise ValueError if mac or LMK are not bytes-like objects or wrong length.
//|         Returns a tuple of (peer_addr, lmk, channel, ifidx, encrypt).
//|     """
//|     ...
//|
STATIC mp_obj_t espnow_get_peer(mp_obj_t self_in, mp_obj_t peer_mac) {
    esp_now_peer_info_t peer = {0};
    memcpy(peer.peer_addr, _get_peer(peer_mac), ESP_NOW_ETH_ALEN);
    check_esp_err(esp_now_get_peer(peer.peer_addr, &peer));
    return _peer_info_to_tuple(&peer);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_get_peer_obj, espnow_get_peer);

//|     def get_peers(self) -> Tuple[Tuple[bytes, bytes, int, int, bool], ...]:
//|     """
//|         Fetch peer_info records for all registered ESPNow peers.
//|         Returns a tuple of tuples: ((peer_addr, lmk, channel, ifidx, encrypt), ...)
//|     """
//|     ...
//|
STATIC mp_obj_t espnow_get_peers(mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    // Build and initialise the peer info tuple.
    mp_obj_tuple_t *peerinfo_tuple = mp_obj_new_tuple(self->num_peers, NULL);
    esp_now_peer_info_t peer = {0};

    for (size_t i = 0; i < peerinfo_tuple->len; i++) {
        esp_err_t status = esp_now_fetch_peer((i == 0), &peer);
        peerinfo_tuple->items[i] = (status == ESP_OK ? _peer_info_to_tuple(&peer) : mp_const_none);
    }

    return peerinfo_tuple;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_get_peers_obj, espnow_get_peers);

//|     num_peers: Tuple[int, int]
//|     """The number of registered peers in a tuple of (num_total_peers, num_encrypted_peers). (read-only)
//|     """
//|
STATIC mp_obj_t espnow_get_num_peers(mp_obj_t self_in) {
    esp_now_peer_num_t peer_num = {0};
    check_esp_err(esp_now_get_peer_num(&peer_num));
    return NEW_TUPLE(
        mp_obj_new_int(peer_num.total_num),
        mp_obj_new_int(peer_num.encrypt_num));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_get_num_peers_obj, espnow_get_num_peers);

MP_PROPERTY_GETTER(espnow_num_peers_obj,
    (mp_obj_t)&espnow_get_num_peers_obj);

STATIC const mp_rom_map_elem_t espnow_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active),      MP_ROM_PTR(&espnow_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_config),      MP_ROM_PTR(&espnow_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_stats),       MP_ROM_PTR(&espnow_stats_obj) },

    // Send and receive messages
    { MP_ROM_QSTR(MP_QSTR_send),        MP_ROM_PTR(&espnow_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv),        MP_ROM_PTR(&espnow_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_any),         MP_ROM_PTR(&espnow_any_obj) },

    // Peer management functions
    { MP_ROM_QSTR(MP_QSTR_set_pmk),     MP_ROM_PTR(&espnow_set_pmk_obj) },
    { MP_ROM_QSTR(MP_QSTR_add_peer),    MP_ROM_PTR(&espnow_add_peer_obj) },
    { MP_ROM_QSTR(MP_QSTR_mod_peer),    MP_ROM_PTR(&espnow_mod_peer_obj) },
    { MP_ROM_QSTR(MP_QSTR_del_peer),    MP_ROM_PTR(&espnow_del_peer_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_peer),    MP_ROM_PTR(&espnow_get_peer_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_peers),   MP_ROM_PTR(&espnow_get_peers_obj) },
    { MP_ROM_QSTR(MP_QSTR_num_peers),   MP_ROM_PTR(&espnow_num_peers_obj) },
};
STATIC MP_DEFINE_CONST_DICT(espnow_locals_dict, espnow_locals_dict_table);

// --- Dummy Buffer Protocol support ---
// ...so asyncio can poll.ipoll() on this device

// Support ioctl(MP_STREAM_POLL, ) for asyncio
STATIC mp_uint_t espnow_stream_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    if (request != MP_STREAM_POLL) {
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }

    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return (espnow_deinited(self)) ? 0 :  // If not initialised
           arg ^ (
               // If no data in the buffer, unset the Read ready flag
               ((ringbuf_avail(self->recv_buffer) == 0) ? MP_STREAM_POLL_RD : 0) |
               // If still waiting for responses, unset the Write ready flag
               ((self->tx_responses < self->tx_packets) ? MP_STREAM_POLL_WR : 0));
}

STATIC const mp_stream_p_t espnow_stream_p = {
    .ioctl = espnow_stream_ioctl,
};

//  Return reference to the dictionary of peers we have seen:
//  {peer1: (rssi, time), peer2: (rssi, time), ...}
//  where:
//      peerX is a byte string containing the 6-byte mac address of the peer,
//      rssi is the wifi signal strength from the last msg received (in dBm from -127 to 0)
//      time is the time in milliseconds since device last booted.
STATIC void espnow_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] != MP_OBJ_NULL) {   // Only allow "Load" operation
        return;
    }
    if (attr == MP_QSTR_peers_table) {
        dest[0] = self->peers_table;
        return;
    }
    dest[1] = MP_OBJ_SENTINEL;      // Attribute not found
}

const mp_obj_type_t espnow_type = {
    { &mp_type_type },
    .name = MP_QSTR_NOW,
    .make_new = espnow_make_new,
    .locals_dict = (mp_obj_t)&espnow_locals_dict,
    .attr = espnow_attr,
    .flags = MP_TYPE_FLAG_EXTENDED,
    MP_TYPE_EXTENDED_FIELDS(
        .protocol = &espnow_stream_p,
        ),
};
