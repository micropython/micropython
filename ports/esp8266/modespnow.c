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

#include "py/runtime.h"

#if MICROPY_ESPNOW

#include "c_types.h"
#include "espnow.h"

#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/qstr.h"
#include "py/objstr.h"
#include "py/objarray.h"
#include "py/stream.h"
#include "py/binary.h"
#include "py/ringbuf.h"

#include "mpconfigport.h"

#include "modespnow.h"

// For the esp8266
#define ESP_NOW_MAX_DATA_LEN (250)
#define ESP_NOW_KEY_LEN (16)
#define ESP_NOW_ETH_ALEN (6)
#define ESP_NOW_SEND_SUCCESS (0)
#define ESP_ERR_ESPNOW_NO_MEM (-77777)
#define ESP_OK (0)
#define ESP_NOW_MAX_TOTAL_PEER_NUM (20)
#define ESP_NOW_MAX_ENCRYPT_PEER_NUM (6)
#define ESP_ERR_ESPNOW_NOT_INIT (0x300 + 100 + 1)
typedef int esp_err_t;

static const uint8_t ESPNOW_MAGIC = 0x99;

// Use this for peeking at the header of the next packet in the buffer.
typedef struct {
    uint8_t magic;              // = ESPNOW_MAGIC
    uint8_t msg_len;            // Length of the message
} __attribute__((packed)) espnow_hdr_t;

// ESPNow packet format for the receive buffer.
typedef struct {
    espnow_hdr_t hdr;           // The header
    uint8_t peer[6];            // Peer address
    uint8_t msg[0];             // Message is up to 250 bytes
} __attribute__((packed)) espnow_pkt_t;

// The maximum length of an espnow packet (bytes)
static const size_t MAX_PACKET_LEN = (
    sizeof(espnow_pkt_t) + ESP_NOW_MAX_DATA_LEN);

// Enough for 2 full-size packets: 2 * (6 + 2 + 250) = 516 bytes
// Will allocate an additional 7 bytes for buffer overhead
#define DEFAULT_RECV_BUFFER_SIZE \
    (2 * (sizeof(espnow_pkt_t) + ESP_NOW_MAX_DATA_LEN))

// Default timeout (millisec) to wait for incoming ESPNow messages (5 minutes).
#define DEFAULT_RECV_TIMEOUT_MS (5 * 60 * 1000)

// Number of milliseconds to wait for pending responses to sent packets.
// This is a fallback which should never be reached.
#define PENDING_RESPONSES_TIMEOUT_MS 100

// The data structure for the espnow_singleton.
typedef struct _esp_espnow_obj_t {
    mp_obj_base_t base;
    ringbuf_t *recv_buffer;         // A buffer for received packets
    size_t recv_buffer_size;        // Size of recv buffer
    size_t recv_timeout_ms;         // Timeout for irecv()
    size_t tx_packets;              // Count of sent packets
    volatile size_t tx_responses;   // # of sent packet responses received
    volatile size_t tx_failures;    // # of sent packet responses failed
} esp_espnow_obj_t;

// Initialised below.
const mp_obj_type_t esp_espnow_type;

static esp_espnow_obj_t espnow_singleton = {
    .base.type = &esp_espnow_type,
    .recv_buffer = NULL,
    .recv_buffer_size = DEFAULT_RECV_BUFFER_SIZE,
    .recv_timeout_ms = DEFAULT_RECV_TIMEOUT_MS,
};

// ### Initialisation and Config functions
//

static void check_esp_err(int e) {
    if (e != 0) {
        mp_raise_OSError(e);
    }
}

// Return a pointer to the ESPNow module singleton
// If state == INITIALISED check the device has been initialised.
// Raises OSError if not initialised and state == INITIALISED.
static esp_espnow_obj_t *_get_singleton() {
    return &espnow_singleton;
}

static esp_espnow_obj_t *_get_singleton_initialised() {
    esp_espnow_obj_t *self = _get_singleton();
    if (self->recv_buffer == NULL) {
        // Throw an espnow not initialised error
        check_esp_err(ESP_ERR_ESPNOW_NOT_INIT);
    }
    return self;
}

// Allocate and initialise the ESPNow module as a singleton.
// Returns the initialised espnow_singleton.
STATIC mp_obj_t espnow_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {

    return _get_singleton();
}

// Forward declare the send and recv ESPNow callbacks
STATIC void send_cb(uint8_t *mac_addr, uint8_t status);

STATIC void recv_cb(uint8_t *mac_addr, uint8_t *data, uint8_t len);

// ESPNow.deinit(): De-initialise the ESPNOW software stack, disable callbacks
// and deallocate the recv data buffers.
// Note: this function is called from main.c:mp_task() to cleanup before soft
// reset, so cannot be declared STATIC and must guard against self == NULL;.
mp_obj_t espnow_deinit(mp_obj_t _) {
    esp_espnow_obj_t *self = _get_singleton();
    if (self->recv_buffer != NULL) {
        // esp_now_unregister_recv_cb();
        esp_now_deinit();
        self->recv_buffer->buf = NULL;
        self->recv_buffer = NULL;
        self->tx_packets = self->tx_responses;
    }
    MP_STATE_PORT(espnow_buffer) = NULL;
    return mp_const_none;
}

// ESPNow.active(): Initialise the data buffers and ESP-NOW functions.
// Initialise the Espressif ESPNOW software stack, register callbacks and
// allocate the recv data buffers.
// Returns True if interface is active, else False.
STATIC mp_obj_t espnow_active(size_t n_args, const mp_obj_t *args) {
    esp_espnow_obj_t *self = args[0];
    if (n_args > 1) {
        if (mp_obj_is_true(args[1])) {
            if (self->recv_buffer == NULL) {    // Already initialised
                self->recv_buffer = m_new_obj(ringbuf_t);
                ringbuf_alloc(self->recv_buffer, self->recv_buffer_size);
                MP_STATE_PORT(espnow_buffer) = self->recv_buffer;
                esp_now_init();
                esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
                esp_now_register_recv_cb(recv_cb);
                esp_now_register_send_cb(send_cb);
            }
        } else {
            espnow_deinit(self);
        }
    }
    return mp_obj_new_bool(self->recv_buffer != NULL);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espnow_active_obj, 1, 2, espnow_active);

// ESPNow.config(): Initialise the data buffers and ESP-NOW functions.
// Initialise the Espressif ESPNOW software stack, register callbacks and
// allocate the recv data buffers.
// Returns True if interface is active, else False.
STATIC mp_obj_t espnow_config(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    esp_espnow_obj_t *self = _get_singleton();
    enum { ARG_rxbuf, ARG_timeout_ms };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout_ms, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    if (args[ARG_rxbuf].u_int >= 0) {
        self->recv_buffer_size = args[ARG_rxbuf].u_int;
    }
    if (args[ARG_timeout_ms].u_int >= 0) {
        self->recv_timeout_ms = args[ARG_timeout_ms].u_int;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(espnow_config_obj, 1, espnow_config);

// ### The ESP_Now send and recv callback routines
//

// Callback triggered when a sent packet is acknowledged by the peer (or not).
// Just count the number of responses and number of failures.
// These are used in the send()/write() logic.
STATIC void send_cb(uint8_t *mac_addr, uint8_t status) {
    esp_espnow_obj_t *self = _get_singleton();
    self->tx_responses++;
    if (status != ESP_NOW_SEND_SUCCESS) {
        self->tx_failures++;
    }
}

// Callback triggered when an ESP-Now packet is received.
// Write the peer MAC address and the message into the recv_buffer as an
// ESPNow packet.
// If the buffer is full, drop the message and increment the dropped count.
// Schedules the user callback if one has been registered (ESPNow.config()).
STATIC void recv_cb(uint8_t *mac_addr, uint8_t *msg, uint8_t msg_len) {
    esp_espnow_obj_t *self = _get_singleton();
    ringbuf_t *buf = self->recv_buffer;
    // TODO: Test this works with ">".
    if (buf == NULL || sizeof(espnow_pkt_t) + msg_len >= ringbuf_free(buf)) {
        return;
    }
    espnow_hdr_t header;
    header.magic = ESPNOW_MAGIC;
    header.msg_len = msg_len;

    ringbuf_put_bytes(buf, (uint8_t *)&header, sizeof(header));
    ringbuf_put_bytes(buf, mac_addr, ESP_NOW_ETH_ALEN);
    ringbuf_put_bytes(buf, msg, msg_len);
}

// Return C pointer to byte memory string/bytes/bytearray in obj.
// Raise ValueError if the length does not match expected len.
static uint8_t *_get_bytes_len_rw(mp_obj_t obj, size_t len, mp_uint_t rw) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(obj, &bufinfo, rw);
    if (bufinfo.len != len) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid buffer length"));
    }
    return (uint8_t *)bufinfo.buf;
}

static uint8_t *_get_bytes_len(mp_obj_t obj, size_t len) {
    return _get_bytes_len_rw(obj, len, MP_BUFFER_READ);
}

static uint8_t *_get_bytes_len_w(mp_obj_t obj, size_t len) {
    return _get_bytes_len_rw(obj, len, MP_BUFFER_WRITE);
}

// ### Handling espnow packets in the recv buffer
//

// Copy data from the ring buffer - wait if buffer is empty up to timeout_ms
//    0: Success
//   -1: Not enough data available to complete read (try again later)
//   -2: Requested read is larger than buffer - will never succeed
static int ringbuf_get_bytes_wait(ringbuf_t *r, uint8_t *data, size_t len, mp_int_t timeout_ms) {
    mp_uint_t start = mp_hal_ticks_ms();
    int status = 0;
    while (((status = ringbuf_get_bytes(r, data, len)) == -1)
           && (timeout_ms < 0 || (mp_uint_t)(mp_hal_ticks_ms() - start) < (mp_uint_t)timeout_ms)) {
        MICROPY_EVENT_POLL_HOOK;
    }
    return status;
}

// ESPNow.recvinto([timeout_ms, []]):
// Returns a list of byte strings: (peer_addr, message) where peer_addr is
// the MAC address of the sending peer.
// Arguments:
//      timeout_ms: timeout in milliseconds (or None).
//      buffers: list of bytearrays to store values: [peer, message].
// Default timeout is set with ESPNow.config(timeout=milliseconds).
// Return (None, None) on timeout.
STATIC mp_obj_t espnow_recvinto(size_t n_args, const mp_obj_t *args) {
    esp_espnow_obj_t *self = _get_singleton_initialised();

    size_t timeout_ms = ((n_args > 2 && args[2] != mp_const_none)
            ? mp_obj_get_int(args[2]) : self->recv_timeout_ms);

    mp_obj_list_t *list = MP_OBJ_TO_PTR(args[1]);
    if (!mp_obj_is_type(list, &mp_type_list) || list->len < 2) {
        mp_raise_ValueError(MP_ERROR_TEXT("ESPNow.recvinto(): Invalid argument"));
    }
    mp_obj_array_t *msg = MP_OBJ_TO_PTR(list->items[1]);
    size_t msg_size = msg->len + msg->free;
    if (mp_obj_is_type(msg, &mp_type_bytearray)) {
        msg->len = msg_size;   // Make all the space in msg array available
        msg->free = 0;
    }
    uint8_t *peer_buf = _get_bytes_len_w(list->items[0], ESP_NOW_ETH_ALEN);
    uint8_t *msg_buf = _get_bytes_len_w(msg, ESP_NOW_MAX_DATA_LEN);

    // Read the packet header from the incoming buffer
    espnow_hdr_t hdr;
    if (ringbuf_get_bytes_wait(self->recv_buffer, (uint8_t *)&hdr, sizeof(hdr), timeout_ms) < 0) {
        return MP_OBJ_NEW_SMALL_INT(0);    // Timeout waiting for packet
    }
    int msg_len = hdr.msg_len;

    // Check the message packet header format and read the message data
    if (hdr.magic != ESPNOW_MAGIC
        || msg_len > ESP_NOW_MAX_DATA_LEN
        || ringbuf_get_bytes(self->recv_buffer, peer_buf, ESP_NOW_ETH_ALEN) < 0
        || ringbuf_get_bytes(self->recv_buffer, msg_buf, msg_len) < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("ESPNow.recv(): buffer error"));
    }
    if (mp_obj_is_type(msg, &mp_type_bytearray)) {
        // Set the length of the message bytearray.
        msg->len = msg_len;
        msg->free = msg_size - msg_len;
    }

    return MP_OBJ_NEW_SMALL_INT(msg_len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espnow_recvinto_obj, 2, 3, espnow_recvinto);

// Used by espnow_send() for sends() with sync==True.
// Wait till all pending sent packet responses have been received.
// ie. self->tx_responses == self->tx_packets.
// Return the number of responses where status != ESP_NOW_SEND_SUCCESS.
static void _wait_for_pending_responses(esp_espnow_obj_t *self) {
    for (int i = 0; i < PENDING_RESPONSES_TIMEOUT_MS; i++) {
        if (self->tx_responses >= self->tx_packets) {
            return;
        }
        mp_hal_delay_ms(1);  // Allow other tasks to run
    }
    // Note: the loop timeout is just a fallback - in normal operation
    // we should never reach that timeout.
}

// ESPNow.send(peer_addr, message, [sync (=true)])
// ESPNow.send(message)
// Send a message to the peer's mac address. Optionally wait for a response.
// If sync == True, wait for response after sending.
// Returns:
//   True  if sync==False and message sent successfully.
//   True  if sync==True and message is received successfully by all recipients
//   False if sync==True and message is not received by at least one recipient
// Raises: EAGAIN if the internal espnow buffers are full.
STATIC mp_obj_t espnow_send(size_t n_args, const mp_obj_t *args) {
    esp_espnow_obj_t *self = _get_singleton_initialised();

    bool sync = n_args <= 3 || args[3] == mp_const_none || mp_obj_is_true(args[3]);
    // Get a pointer to the buffer of obj
    mp_buffer_info_t message;
    mp_get_buffer_raise(args[2], &message, MP_BUFFER_READ);

    // Bugfix: esp_now_send() generates a panic if message buffer points
    // to an address in ROM (eg. a statically interned QSTR).
    // Fix: if message is not in gc pool, copy to a temp buffer.
    static char temp[ESP_NOW_MAX_DATA_LEN];  // Static to save code space
    byte *p = (byte *)message.buf;
    // if (p < MP_STATE_MEM(area.gc_pool_start) || MP_STATE_MEM(area.gc_pool_end) < p) {
    if (MP_STATE_MEM(area.gc_pool_end) < p) {
        // If buffer is not in GC pool copy from ROM to stack
        memcpy(temp, message.buf, message.len);
        message.buf = temp;
    }

    if (sync) {
        // If the last call was sync==False there may be outstanding responses.
        // We need to wait for all pending responses if this call has sync=True.
        _wait_for_pending_responses(self);
    }
    int saved_failures = self->tx_failures;

    check_esp_err(
        esp_now_send(_get_bytes_len(args[1], ESP_NOW_ETH_ALEN), message.buf, message.len));
    self->tx_packets++;
    if (sync) {
        // Wait for message to be received by peer
        _wait_for_pending_responses(self);
    }
    // Return False if sync and any peers did not respond.
    return mp_obj_new_bool(!(sync && self->tx_failures != saved_failures));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espnow_send_obj, 3, 4, espnow_send);

// ### Peer Management Functions
//

// Set the ESP-NOW Primary Master Key (pmk) (for encrypted communications).
// Raise OSError if not initialised.
// Raise ValueError if key is not a bytes-like object exactly 16 bytes long.
STATIC mp_obj_t espnow_set_pmk(mp_obj_t _, mp_obj_t key) {
    check_esp_err(esp_now_set_kok(_get_bytes_len(key, ESP_NOW_KEY_LEN), ESP_NOW_KEY_LEN));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_set_pmk_obj, espnow_set_pmk);

// ESPNow.add_peer(peer_mac, [lmk, [channel, [ifidx, [encrypt]]]])
// Positional args set to None will be left at defaults.
// Raise OSError if not initialised.
// Raise ValueError if mac or LMK are not bytes-like objects or wrong length.
// Raise TypeError if invalid keyword args or too many positional args.
// Return None.
STATIC mp_obj_t espnow_add_peer(size_t n_args, const mp_obj_t *args) {
    check_esp_err(
        esp_now_add_peer(
            _get_bytes_len(args[1], ESP_NOW_ETH_ALEN),
            ESP_NOW_ROLE_COMBO,
            (n_args > 3) ? mp_obj_get_int(args[3]) : 0,
            (n_args > 2) ? _get_bytes_len(args[2], ESP_NOW_KEY_LEN) : NULL,
            ESP_NOW_KEY_LEN));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espnow_add_peer_obj, 2, 4, espnow_add_peer);

// ESPNow.del_peer(peer_mac): Unregister peer_mac.
// Raise OSError if not initialised.
// Raise ValueError if peer is not a bytes-like objects or wrong length.
// Return None.
STATIC mp_obj_t espnow_del_peer(mp_obj_t _, mp_obj_t peer) {
    esp_now_del_peer(_get_bytes_len(peer, ESP_NOW_ETH_ALEN));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_del_peer_obj, espnow_del_peer);

STATIC const mp_rom_map_elem_t esp_espnow_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&espnow_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&espnow_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_recvinto), MP_ROM_PTR(&espnow_recvinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&espnow_send_obj) },

    // Peer management functions
    { MP_ROM_QSTR(MP_QSTR_set_pmk), MP_ROM_PTR(&espnow_set_pmk_obj) },
    { MP_ROM_QSTR(MP_QSTR_add_peer), MP_ROM_PTR(&espnow_add_peer_obj) },
    { MP_ROM_QSTR(MP_QSTR_del_peer), MP_ROM_PTR(&espnow_del_peer_obj) },
};
STATIC MP_DEFINE_CONST_DICT(esp_espnow_locals_dict, esp_espnow_locals_dict_table);

STATIC const mp_rom_map_elem_t espnow_globals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__espnow) },
    { MP_ROM_QSTR(MP_QSTR_ESPNowBase), MP_ROM_PTR(&esp_espnow_type) },
    { MP_ROM_QSTR(MP_QSTR_MAX_DATA_LEN), MP_ROM_INT(ESP_NOW_MAX_DATA_LEN)},
    { MP_ROM_QSTR(MP_QSTR_ADDR_LEN), MP_ROM_INT(ESP_NOW_ETH_ALEN)},
    { MP_ROM_QSTR(MP_QSTR_KEY_LEN), MP_ROM_INT(ESP_NOW_KEY_LEN)},
    { MP_ROM_QSTR(MP_QSTR_MAX_TOTAL_PEER_NUM), MP_ROM_INT(ESP_NOW_MAX_TOTAL_PEER_NUM)},
    { MP_ROM_QSTR(MP_QSTR_MAX_ENCRYPT_PEER_NUM), MP_ROM_INT(ESP_NOW_MAX_ENCRYPT_PEER_NUM)},
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
    esp_espnow_obj_t *self = _get_singleton();
    return (self->recv_buffer == NULL) ? 0 :  // If not initialised
           arg ^ ((ringbuf_avail(self->recv_buffer) == 0) ? MP_STREAM_POLL_RD : 0);
}

STATIC const mp_stream_p_t espnow_stream_p = {
    .ioctl = espnow_stream_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    esp_espnow_type,
    MP_QSTR_ESPNowBase,
    MP_TYPE_FLAG_NONE,
    make_new, espnow_make_new,
    protocol, &espnow_stream_p,
    locals_dict, &esp_espnow_locals_dict
    );

const mp_obj_module_t mp_module_espnow = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&espnow_globals_dict,
};

MP_REGISTER_MODULE(MP_QSTR__espnow, mp_module_espnow);
MP_REGISTER_ROOT_POINTER(void *espnow_buffer);
#endif
