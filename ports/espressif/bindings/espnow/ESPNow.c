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
#include "bindings/espnow/ESPNow.h"

#include "shared-bindings/util.h"

#include "common-hal/espnow/ESPNow.h"

static void check_esp_err(esp_err_t status) {
    if (status != ESP_OK) {
        mp_raise_RuntimeError(translate("an error occured"));
    }
}

// --- Initialisation and Config functions ---

static void check_for_deinit(espnow_obj_t *self) {
    if (common_hal_espnow_deinited(self)) {
        raise_deinited_error();
    }
}

//| class ESPNow:
//|     """Provides access to the ESP-NOW protocol."""
//|
//|     def __init__(self, buffer_size: Optional[int], phy_rate: Optional[int]) -> None:
//|         """Allocate and initialize `ESPNow` instance as a singleton.
//|
//|         :param int buffer_size: The size of the internal ring buffer. Default: 526 bytes.
//|         :param int phy_rate: The ESP-NOW physical layer rate. Default: 1 Mbps."""
//|         ...
STATIC mp_obj_t espnow_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_buffer_size, ARG_phy_rate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer_size,  MP_ARG_INT, { .u_int = 526 } },
        { MP_QSTR_phy_rate,     MP_ARG_INT, { .u_int = WIFI_PHY_RATE_1M_L } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    espnow_obj_t *self = MP_STATE_PORT(espnow_singleton);

    if (self != NULL) {
        mp_raise_RuntimeError(translate("Already running"));
    }

    self = m_new_obj(espnow_obj_t);
    self->base.type = &espnow_type;

    common_hal_espnow_set_buffer_size(self, args[ARG_buffer_size].u_int);
    common_hal_espnow_set_phy_rate(self, args[ARG_phy_rate].u_int);

    self->peers_table = mp_obj_new_dict(0);

    // Prevent user code modifying the dict
    mp_obj_dict_get_map(self->peers_table)->is_fixed = 1;

    // Set the global singleton pointer for the espnow protocol.
    MP_STATE_PORT(espnow_singleton) = self;

    common_hal_espnow_init(self);

    return self;
}

// Return C pointer to byte memory string/bytes/bytearray in obj.
// Raise ValueError if the length does not match expected len.
static const uint8_t *_get_bytes_len(mp_obj_t obj, size_t len, mp_uint_t rw) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(obj, &bufinfo, rw);
    mp_arg_validate_length(bufinfo.len, len, MP_QSTR_buffer);
    return (uint8_t *)bufinfo.buf;
}

//|     def set_pmk(self, pmk: ReadableBuffer) -> None:
//|         """Set the ESP-NOW Primary Master Key (pmk) for encrypted communications.
//|
//|         :param ReadableBuffer pmk: The ESP-NOW Primary Master Key (length = 16 bytes)."""
//|         ...
STATIC mp_obj_t espnow_set_pmk(mp_obj_t self_in, mp_obj_t key) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_espnow_set_pmk(self, _get_bytes_len(key, ESP_NOW_KEY_LEN, MP_BUFFER_READ));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_set_pmk_obj, espnow_set_pmk);

//|     active: bool
//|     """Initialize or de-initialize the `ESPNow` communication protocol."""
//|
STATIC mp_obj_t espnow_get_active(const mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(!common_hal_espnow_deinited(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_get_active_obj, espnow_get_active);

STATIC mp_obj_t espnow_set_active(const mp_obj_t self_in, const mp_obj_t value) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_is_true(value) ? common_hal_espnow_init(self) : common_hal_espnow_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(espnow_set_active_obj, espnow_set_active);

MP_PROPERTY_GETSET(espnow_active_obj,
    (mp_obj_t)&espnow_get_active_obj,
    (mp_obj_t)&espnow_set_active_obj);


//|     buffer_size: int
//|     """The size of the internal ring buffer."""
//|
STATIC mp_obj_t espnow_get_buffer_size(const mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->recv_buffer_size);
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_get_buffer_size_obj, espnow_get_buffer_size);

STATIC mp_obj_t espnow_set_buffer_size(const mp_obj_t self_in, const mp_obj_t value) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_espnow_set_buffer_size(self, mp_obj_get_int(value));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(espnow_set_buffer_size_obj, espnow_set_buffer_size);

MP_PROPERTY_GETSET(espnow_buffer_size_obj,
    (mp_obj_t)&espnow_get_buffer_size_obj,
    (mp_obj_t)&espnow_set_buffer_size_obj);

//|     phy_rate: int
//|     """The ESP-NOW physical layer rate."""
//|
STATIC mp_obj_t espnow_get_phy_rate(const mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->phy_rate);
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_get_phy_rate_obj, espnow_get_phy_rate);

STATIC mp_obj_t espnow_set_phy_rate(const mp_obj_t self_in, const mp_obj_t value) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_espnow_set_phy_rate(self, mp_obj_get_int(value));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(espnow_set_phy_rate_obj, espnow_set_phy_rate);

MP_PROPERTY_GETSET(espnow_phy_rate_obj,
    (mp_obj_t)&espnow_get_phy_rate_obj,
    (mp_obj_t)&espnow_set_phy_rate_obj);

//|     stats: Tuple[int, int, int, int, int]
//|     """Provide some useful stats in a `tuple` of
//|        (tx_packets, tx_responses, tx_failures, rx_packets, rx_failures). (read-only)"""
//|
STATIC mp_obj_t espnow_get_stats(mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_TUPLE(
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

// --- Handling espnow packets in the recv buffer ---

// --- Send and Receive ESP-NOW data ---

// Return C pointer to the MAC address.
// Raise ValueError if mac is wrong type or is not 6 bytes long.
static const uint8_t *_get_peer_addr(mp_obj_t mac) {
    return mp_obj_is_true(mac) ? _get_bytes_len(mac, ESP_NOW_ETH_ALEN, MP_BUFFER_READ) : NULL;
}

//|     def send(
//|         self,
//|         message: ReadableBuffer,
//|         mac: Optional[ReadableBuffer],
//|         sync: bool = True,
//|     ) -> bool:
//|         """Send a message to the peer's mac address. Optionally wait for a response.
//|
//|         :param ReadableBuffer message: The message to send (length <= 250 bytes).
//|         :param ReadableBuffer mac: The peer's address (length = 6 bytes). If `None` or any non-true value, send to all registered peers.
//|         :param bool sync: If `True`, wait for response from peer(s) after sending.
//|
//|         :returns:
//|             `True` if sync == `False` and message sent successfully.
//|             `True` if sync == `True` and message is received successfully by all recipients
//|             `False` if sync == `True` and message is not received by at least one recipient
//|
//|         :raises EAGAIN: if the internal espnow buffers are full."""
//|         ...
STATIC mp_obj_t espnow_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_message, ARG_mac, ARG_sync };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_message,  MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_mac,      MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_sync,     MP_ARG_BOOL, { .u_bool = mp_const_true } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    espnow_obj_t *self = pos_args[0];
    check_for_deinit(self);

    const bool sync = mp_obj_is_true(args[ARG_sync].u_obj);
    const uint8_t *peer_addr = _get_peer_addr(args[ARG_mac].u_obj);

    // Get a pointer to the data buffer of the message
    mp_buffer_info_t message;
    mp_get_buffer_raise(args[ARG_message].u_obj, &message, MP_BUFFER_READ);

    return common_hal_espnow_send(self, sync, peer_addr, &message);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(espnow_send_obj, 2, espnow_send);

//|     def recv(self) -> Optional[ESPNowPacket]:
//|         """Receive a message from the peer(s).
//|
//|         :returns: An `ESPNowPacket` if available in the buffer, otherwise `None`."""
//|         ...
STATIC mp_obj_t espnow_recv(mp_obj_t self_in, mp_obj_t buffers) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    return common_hal_espnow_recv(self);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_recv_obj, espnow_recv);

// --- Peer Management Functions ---

// Common code for add_peer() and mod_peer() to process the args.
static void _update_peer_info(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args, bool modify) {
    enum { ARG_mac, ARG_lmk, ARG_channel, ARG_interface, ARG_encrypt };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mac,      MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_lmk,      MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_channel,  MP_ARG_INT, { .u_obj = mp_const_none } },
        { MP_QSTR_interface,MP_ARG_INT, { .u_obj = mp_const_none } },
        { MP_QSTR_encrypt,  MP_ARG_BOOL,{ .u_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    esp_now_peer_info_t peer = {0};
    memcpy(peer.peer_addr, _get_peer_addr(args[ARG_mac].u_obj), ESP_NOW_ETH_ALEN);

    if (modify) {
        if (esp_now_get_peer(peer.peer_addr, &peer) != ESP_OK) {
            mp_raise_RuntimeError(translate("peer not found"));
        }
    } else {
        if (esp_now_is_peer_exist(peer.peer_addr)) {
            mp_raise_RuntimeError(translate("peer already exists"));
        }
        peer.channel = 0;
        peer.ifidx = WIFI_IF_STA;
        peer.encrypt = false;
    }

    const mp_obj_t channel = args[ARG_channel].u_obj;
    if (channel != mp_const_none) {
        peer.channel = mp_arg_validate_int_range(mp_obj_get_int(channel), 0, 14, MP_QSTR_channel);
    }

    const mp_obj_t interface = args[ARG_interface].u_obj;
    if (interface != mp_const_none) {
        peer.ifidx = (wifi_interface_t)mp_arg_validate_int_range(mp_obj_get_int(interface), 0, 1, MP_QSTR_interface);
    }

    const mp_obj_t encrypt = args[ARG_encrypt].u_obj;
    if (encrypt != mp_const_none) {
        peer.encrypt = mp_obj_is_true(encrypt);
    }

    const mp_obj_t lmk = args[ARG_lmk].u_obj;
    if (lmk != mp_const_none) {
        memcpy(peer.lmk, _get_bytes_len(lmk, ESP_NOW_KEY_LEN, MP_BUFFER_READ), ESP_NOW_KEY_LEN);
    } else if (peer.encrypt) {
        mp_raise_ValueError_varg(translate("%q is %q"), MP_QSTR_lmk, MP_QSTR_None);
    }

    check_esp_err((modify) ? esp_now_mod_peer(&peer) : esp_now_add_peer(&peer));
}

// Update the cached peer count in self->peers_count;
// The peers_count ignores broadcast and multicast addresses and is used for the
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

    self->peers_count = count;
}

//|     def add_peer(
//|         self,
//|         mac: ReadableBuffer,
//|         lmk: Optional[ReadableBuffer],
//|         channel: int = 0,
//|         interface: int = 0,
//|         encrypt: bool = False,
//|     ) -> None:
//|         """Add peer.
//|
//|         :param ReadableBuffer mac: The mac address of the peer.
//|         :param ReadableBuffer lmk: The Local Master Key (lmk) of the peer.
//|         :param int channel: The peer's channel. Default: 0 ie. use the current channel.
//|         :param int interface: The WiFi interface to use. Default: 0 ie. STA.
//|         :param bool encrypt: Whether or not to use encryption."""
//|         ...
STATIC mp_obj_t espnow_add_peer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    espnow_obj_t *self = pos_args[0];
    check_for_deinit(self);

    _update_peer_info(n_args, pos_args, kw_args, false);
    _update_peer_count(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(espnow_add_peer_obj, 2, espnow_add_peer);

//|     def mod_peer(
//|         self,
//|         mac: ReadableBuffer,
//|         lmk: Optional[ReadableBuffer],
//|         channel: int = 0,
//|         interface: int = 0,
//|         encrypt: bool = False,
//|     ) -> None:
//|         """Modify peer.
//|
//|         :param ReadableBuffer mac: The mac address of the peer.
//|         :param ReadableBuffer lmk: The Local Master Key (lmk) of the peer.
//|         :param int channel: The peer's channel. Default: 0 ie. use the current channel.
//|         :param int interface: The WiFi interface to use. Default: 0 ie. STA.
//|         :param bool encrypt: Whether or not to use encryption."""
//|         ...
STATIC mp_obj_t espnow_mod_peer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    espnow_obj_t *self = pos_args[0];
    check_for_deinit(self);

    _update_peer_info(n_args, pos_args, kw_args, true);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(espnow_mod_peer_obj, 2, espnow_mod_peer);

//|     def del_peer(self, mac: ReadableBuffer) -> None:
//|         """Delete peer.
//|
//|         :param ReadableBuffer mac: The mac address of the peer."""
//|         ...
STATIC mp_obj_t espnow_del_peer(mp_obj_t self_in, mp_obj_t mac) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    uint8_t peer_addr[ESP_NOW_ETH_ALEN];
    memcpy(peer_addr, _get_peer_addr(mac), ESP_NOW_ETH_ALEN);

    check_esp_err(esp_now_del_peer(peer_addr));
    _update_peer_count(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_del_peer_obj, espnow_del_peer);

// Convert a peer_info struct to python tuple
// Used by espnow_get_peer() and espnow_get_peers()
static mp_obj_t _peer_info_to_tuple(const esp_now_peer_info_t *peer) {
    return MP_OBJ_NEW_TUPLE(
        mp_obj_new_bytes(peer->peer_addr, MP_ARRAY_SIZE(peer->peer_addr)),
        mp_obj_new_bytes(peer->lmk, MP_ARRAY_SIZE(peer->lmk)),
        mp_obj_new_int(peer->channel),
        mp_obj_new_int(peer->ifidx),
        mp_obj_new_bool(peer->encrypt));
}

//|     def get_peer(self, mac: ReadableBuffer) -> Tuple[bytes, int, int, bool]:
//|         """Get the peer info for mac as a `tuple`.
//|
//|         :param ReadableBuffer mac: The mac address of the peer.
//|
//|         :returns: A `tuple` of (mac, lmk, channel, interface, encrypt)."""
//|         ...
STATIC mp_obj_t espnow_get_peer(mp_obj_t self_in, mp_obj_t mac) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    esp_now_peer_info_t peer = {0};
    memcpy(peer.peer_addr, _get_peer_addr(mac), ESP_NOW_ETH_ALEN);
    check_esp_err(esp_now_get_peer(peer.peer_addr, &peer));

    return _peer_info_to_tuple(&peer);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_get_peer_obj, espnow_get_peer);

// --- Peer Related Properties ---

//|     peers: Tuple[Tuple[bytes, bytes, int, int, bool], ...]
//|     """The peer info records for all registered `ESPNow` peers. (read-only)
//|
//|     A `tuple` of tuples: ((mac, lmk, channel, interface, encrypt), ...)."""
//|
STATIC mp_obj_t espnow_get_peers(mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    // Build and initialize the peer info tuple.
    mp_obj_tuple_t *peerinfo_tuple = mp_obj_new_tuple(self->peers_count, NULL);
    esp_now_peer_info_t peer = {0};

    for (size_t i = 0; i < peerinfo_tuple->len; i++) {
        esp_err_t status = esp_now_fetch_peer((i == 0), &peer);
        peerinfo_tuple->items[i] = (status == ESP_OK ? _peer_info_to_tuple(&peer) : mp_const_none);
    }

    return peerinfo_tuple;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_get_peers_obj, espnow_get_peers);

MP_PROPERTY_GETTER(espnow_peers_obj,
    (mp_obj_t)&espnow_get_peers_obj);

//|     peers_count: Tuple[int, int]
//|     """The number of registered peers in a `tuple` of (num_total_peers, num_encrypted_peers). (read-only)"""
//|
STATIC mp_obj_t espnow_get_peers_count(mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    esp_now_peer_num_t peer_num = {0};
    check_esp_err(esp_now_get_peer_num(&peer_num));
    return MP_OBJ_NEW_TUPLE(
        mp_obj_new_int(peer_num.total_num),
        mp_obj_new_int(peer_num.encrypt_num));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_get_peers_count_obj, espnow_get_peers_count);

MP_PROPERTY_GETTER(espnow_peers_count_obj,
    (mp_obj_t)&espnow_get_peers_count_obj);

//|     peers_table: Dict[bytes, List[int]]
//|     """The dictionary of peers we have seen. (read-only)
//|
//|     A `dict` of {peer: [rssi, time], ...}
//|
//|     where:
//|         peer is a byte string containing the 6-byte mac address of the peer.
//|         rssi is the wifi signal strength from the last msg received (in dBm from -127 to 0).
//|         time is the time in milliseconds since device last booted."""
//|
STATIC mp_obj_t espnow_get_peers_table(mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return self->peers_table;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_get_peers_table_obj, espnow_get_peers_table);

MP_PROPERTY_GETTER(espnow_peers_table_obj,
    (mp_obj_t)&espnow_get_peers_table_obj);

STATIC const mp_rom_map_elem_t espnow_locals_dict_table[] = {
    // Config parameters
    { MP_ROM_QSTR(MP_QSTR_set_pmk),     MP_ROM_PTR(&espnow_set_pmk_obj) },
    { MP_ROM_QSTR(MP_QSTR_active),      MP_ROM_PTR(&espnow_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_buffer_size), MP_ROM_PTR(&espnow_buffer_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_phy_rate),    MP_ROM_PTR(&espnow_phy_rate_obj) },
    { MP_ROM_QSTR(MP_QSTR_stats),       MP_ROM_PTR(&espnow_stats_obj) },

    // Send and receive messages
    { MP_ROM_QSTR(MP_QSTR_send),        MP_ROM_PTR(&espnow_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv),        MP_ROM_PTR(&espnow_recv_obj) },

    // Peer management functions
    { MP_ROM_QSTR(MP_QSTR_add_peer),    MP_ROM_PTR(&espnow_add_peer_obj) },
    { MP_ROM_QSTR(MP_QSTR_mod_peer),    MP_ROM_PTR(&espnow_mod_peer_obj) },
    { MP_ROM_QSTR(MP_QSTR_del_peer),    MP_ROM_PTR(&espnow_del_peer_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_peer),    MP_ROM_PTR(&espnow_get_peer_obj) },

    // Peer related properties
    { MP_ROM_QSTR(MP_QSTR_peers),       MP_ROM_PTR(&espnow_peers_obj) },
    { MP_ROM_QSTR(MP_QSTR_peers_count), MP_ROM_PTR(&espnow_peers_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_peers_table), MP_ROM_PTR(&espnow_peers_table_obj) },
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
    return (common_hal_espnow_deinited(self)) ? 0 :  // If not initialized
           arg ^ (
               // If no data in the buffer, unset the Read ready flag
               ((!ringbuf_num_filled(self->recv_buffer)) ? MP_STREAM_POLL_RD : 0) |
               // If still waiting for responses, unset the Write ready flag
               ((self->tx_responses < self->tx_packets) ? MP_STREAM_POLL_WR : 0));
}

STATIC const mp_stream_p_t espnow_stream_p = {
    .ioctl = espnow_stream_ioctl,
};

//|     def __bool__(self) -> bool:
//|         """``True`` if `len()` is greater than zero.
//|         This is an easy way to check if the buffer is empty.
//|         """
//|         ...
//|     def __len__(self) -> int:
//|         """Return the number of `bytes` available to read. Used to implement ``len()``."""
//|         ...
STATIC mp_obj_t espnow_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t len = ringbuf_num_filled(self->recv_buffer);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(len != 0);
        case MP_UNARY_OP_LEN:
            return mp_obj_new_int_from_uint(len);
        default:
            return MP_OBJ_NULL;      // op not supported
    }
}

const mp_obj_type_t espnow_type = {
    { &mp_type_type },
    .name = MP_QSTR_ESPNow,
    .make_new = espnow_make_new,
    .locals_dict = (mp_obj_t)&espnow_locals_dict,
    .flags = MP_TYPE_FLAG_EXTENDED,
    MP_TYPE_EXTENDED_FIELDS(
        .protocol = &espnow_stream_p,
        .unary_op = &espnow_unary_op
        ),
};
