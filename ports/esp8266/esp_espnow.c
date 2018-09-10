/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nick Moore
 * Copyright (c) 2018 shawwwn <shawwwn1@gmail.com>
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

#include "c_types.h"
#include "espnow.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"

#define ESP_NOW_ETH_ALEN             6         /*!< Length of ESPNOW peer MAC address */
#define ESP_NOW_KEY_LEN              16        /*!< Length of ESPNOW peer local master key */
#define ESP_NOW_MAX_DATA_LEN         250       /*!< Maximum length of ESPNOW data which is sent very time */

enum mt_tx_status {
    MT_TX_STATUS_OK = 0,
    MT_TX_STATUS_FAILED,
};

static inline void esp_espnow_exceptions(int e) {
    if (e != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "ESP-Now Unknown Error 0x%04x", e));
    }
}

#define ESPNOW_EXCEPTIONS(x) do { esp_espnow_exceptions(x); } while (0);

#define ESP_NOW_PEER_EXIST(addr_buf) do {                                                           \
    int e = esp_now_is_peer_exist(addr_buf);                                                        \
    if (e == 0) {                                                                                   \
        mp_raise_msg(&mp_type_OSError, "ESP-Now Peer Not Found");                                   \
    } else if (e < 0) {                                                                             \
        esp_espnow_exceptions(e);                                                                   \
    }                                                                                               \
} while (0);                                                                                        \

static inline void _get_bytes(mp_obj_t str, size_t len, uint8_t *dst) {
    size_t str_len;
    const char *data = mp_obj_str_get_data(str, &str_len);  
    if (str_len != len) mp_raise_ValueError("bad len");
    memcpy(dst, data, len);
}

static mp_obj_t send_cb_obj = mp_const_none;
static mp_obj_t recv_cb_obj = mp_const_none;

STATIC void send_cb(u8 *mac_addr, u8 status)
{
    if (send_cb_obj != mp_const_none) {
        mp_obj_tuple_t *msg = mp_obj_new_tuple(2, NULL);
        msg->items[0] = mp_obj_new_bytes(mac_addr, ESP_NOW_ETH_ALEN);
        msg->items[1] = (status == MT_TX_STATUS_OK) ? mp_const_true : mp_const_false;
        mp_sched_schedule(send_cb_obj, msg);
    }
}

STATIC void recv_cb(u8 *mac_addr, u8 *data, u8 len)
{
    if (recv_cb_obj != mp_const_none) {
        mp_obj_tuple_t *msg = mp_obj_new_tuple(2, NULL);
        msg->items[0] = mp_obj_new_bytes(mac_addr, ESP_NOW_ETH_ALEN);
        msg->items[1] = mp_obj_new_bytes(data, len);
        mp_sched_schedule(recv_cb_obj, msg);
    }
} 

static int initialized = 0;

STATIC mp_obj_t espnow_init() {
    if (!initialized) {
        ESPNOW_EXCEPTIONS(esp_now_init());
        ESPNOW_EXCEPTIONS(esp_now_set_self_role(ESP_NOW_ROLE_COMBO));
        initialized = 1;

        ESPNOW_EXCEPTIONS(esp_now_register_recv_cb(recv_cb));
        ESPNOW_EXCEPTIONS(esp_now_register_send_cb(send_cb));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(espnow_init_obj, espnow_init);

STATIC mp_obj_t espnow_deinit() {
    if (initialized) {
        ESPNOW_EXCEPTIONS(esp_now_deinit());
        initialized = 0;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(espnow_deinit_obj, espnow_deinit);

STATIC mp_obj_t espnow_on_send(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return send_cb_obj;
    }

    send_cb_obj = args[0];
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espnow_on_send_obj, 0, 1, espnow_on_send);

STATIC mp_obj_t espnow_on_recv(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return recv_cb_obj;
    }

    recv_cb_obj = args[0];
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espnow_on_recv_obj, 0, 1, espnow_on_recv);

// pmk(primary_key)
STATIC mp_obj_t espnow_pmk(mp_obj_t key) {
    uint8_t key_buf[ESP_NOW_KEY_LEN];
    _get_bytes(key, ESP_NOW_KEY_LEN, key_buf);
    ESPNOW_EXCEPTIONS(esp_now_set_kok(key_buf, ESP_NOW_KEY_LEN));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_pmk_obj, espnow_pmk);

// lmk(peer_mac, local_key)
STATIC mp_obj_t espnow_lmk(mp_obj_t addr, mp_obj_t key) {
    mp_uint_t addr_len;
    u8 *addr_buf = (u8 *)mp_obj_str_get_data(addr, &addr_len);
    if (addr_len != ESP_NOW_ETH_ALEN) mp_raise_ValueError("addr invalid");
    
    if (key == mp_const_none) {
        ESPNOW_EXCEPTIONS(esp_now_set_peer_key(addr_buf, NULL, 0));
    } else {
        u8 key_buf[ESP_NOW_KEY_LEN];
        _get_bytes(key, ESP_NOW_KEY_LEN, key_buf);
        ESPNOW_EXCEPTIONS(esp_now_set_peer_key(addr_buf, key_buf, ESP_NOW_KEY_LEN));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_lmk_obj, espnow_lmk);

// add_peer(peer_mac, [local_key])
STATIC mp_obj_t espnow_add_peer(size_t n_args, const mp_obj_t *args) {
    u8 peer_addr[ESP_NOW_ETH_ALEN];
    _get_bytes(args[0], ESP_NOW_ETH_ALEN, peer_addr);
    u8 *lmk_buf = NULL;

    if (n_args > 1) {
        mp_uint_t lmk_len;
        lmk_buf = (u8 *)mp_obj_str_get_data(args[1], &lmk_len);;
        if (lmk_len != ESP_NOW_KEY_LEN) mp_raise_ValueError("key invalid");
    }

    uint8_t channel = 0;

    ESPNOW_EXCEPTIONS(esp_now_add_peer(peer_addr, ESP_NOW_ROLE_COMBO, channel, lmk_buf, ESP_NOW_KEY_LEN));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espnow_add_peer_obj, 1, 2, espnow_add_peer);

// del_peer(peer_mac)
STATIC mp_obj_t espnow_del_peer(mp_obj_t addr) {
    mp_uint_t addr_len;
    u8 *addr_buf = (u8 *)mp_obj_str_get_data(addr, &addr_len);
    if (addr_len != ESP_NOW_ETH_ALEN) mp_raise_ValueError("addr invalid");
    ESP_NOW_PEER_EXIST(addr_buf);
    ESPNOW_EXCEPTIONS(esp_now_del_peer(addr_buf));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_del_peer_obj, espnow_del_peer);

STATIC mp_obj_t espnow_send(mp_obj_t addr, mp_obj_t msg) {
    mp_uint_t addr_len;
    u8 *addr_buf;
    mp_uint_t msg_len;
    u8 *msg_buf = (u8 *)mp_obj_str_get_data(msg, &msg_len);
    if (msg_len > ESP_NOW_MAX_DATA_LEN) mp_raise_ValueError("msg too long");

    if (addr == mp_const_none) {
        // send to all
        ESPNOW_EXCEPTIONS(esp_now_send(NULL, msg_buf, msg_len));
    } else {
        // send to one
        addr_buf = (u8 *)mp_obj_str_get_data(addr, &addr_len);
        if (addr_len != ESP_NOW_ETH_ALEN) mp_raise_ValueError("addr invalid");
        ESPNOW_EXCEPTIONS(esp_now_send(addr_buf, msg_buf, msg_len));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_send_obj, espnow_send);

STATIC mp_obj_t espnow_peer_count() {
    uint8_t all_cnt, encryp_cnt;
    ESPNOW_EXCEPTIONS(esp_now_get_cnt_info(&all_cnt, &encryp_cnt));

    mp_obj_t tuple[2] = {
        mp_obj_new_int(all_cnt),
        mp_obj_new_int(encryp_cnt),
    };
    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(espnow_peer_count_obj, espnow_peer_count);

STATIC ICACHE_FLASH_ATTR mp_obj_t espnow_version() {
    return mp_obj_new_int(0); // 0 for esp8266, 1 for esp32
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(espnow_version_obj, espnow_version);

STATIC const mp_rom_map_elem_t espnow_globals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_espnow) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&espnow_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&espnow_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_pmk), MP_ROM_PTR(&espnow_pmk_obj) },
    { MP_ROM_QSTR(MP_QSTR_lmk), MP_ROM_PTR(&espnow_lmk_obj) },
    { MP_ROM_QSTR(MP_QSTR_add_peer), MP_ROM_PTR(&espnow_add_peer_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&espnow_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_on_send), MP_ROM_PTR(&espnow_on_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_on_recv), MP_ROM_PTR(&espnow_on_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_peer_count), MP_ROM_PTR(&espnow_peer_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_version), MP_ROM_PTR(&espnow_version_obj) },
};
STATIC MP_DEFINE_CONST_DICT(espnow_globals_dict, espnow_globals_dict_table);

const mp_obj_module_t mp_module_esp_espnow = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&espnow_globals_dict,
};
