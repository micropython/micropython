/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nick Moore
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

#include "freertos/queue.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"

#include "modnetwork.h"

NORETURN void _esp_espnow_exceptions(esp_err_t e) {
   switch (e) {
      case ESP_ERR_ESPNOW_NOT_INIT:
        mp_raise_msg(&mp_type_OSError, "ESP-Now Not Initialized");
      case ESP_ERR_ESPNOW_ARG:
        mp_raise_msg(&mp_type_OSError, "ESP-Now Invalid Argument");
      case ESP_ERR_ESPNOW_NO_MEM:
        mp_raise_msg(&mp_type_OSError, "ESP-Now Out Of Mem");
      case ESP_ERR_ESPNOW_FULL:
        mp_raise_msg(&mp_type_OSError, "ESP-Now Peer List Full");
      case ESP_ERR_ESPNOW_NOT_FOUND:
        mp_raise_msg(&mp_type_OSError, "ESP-Now Peer Not Found");
      case ESP_ERR_ESPNOW_INTERNAL:
        mp_raise_msg(&mp_type_OSError, "ESP-Now Internal");
      case ESP_ERR_ESPNOW_EXIST:
        mp_raise_msg(&mp_type_OSError, "ESP-Now Peer Exists");
      default:
        nlr_raise(mp_obj_new_exception_msg_varg(
          &mp_type_RuntimeError, "ESP-Now Unknown Error 0x%04x", e
        ));
   }
}

static inline void esp_espnow_exceptions(esp_err_t e) {
    if (e != ESP_OK) _esp_espnow_exceptions(e);
}

static inline void _get_bytes(mp_obj_t str, size_t len, uint8_t *dst) {
    size_t str_len;
    const char *data = mp_obj_str_get_data(str, &str_len);	
    if (str_len != len) mp_raise_ValueError("bad len");
    memcpy(dst, data, len);
}

// this is crap of course but lets try it

typedef struct {
	uint8_t macaddr[ESP_NOW_ETH_ALEN];
	uint16_t len;
	uint8_t data[ESP_NOW_MAX_DATA_LEN];
} esp_now_queue_t;

QueueHandle_t esp_now_queue;

STATIC mp_obj_t espnow_recv() {
    static esp_now_queue_t queue_item = { 0 };
    int r = xQueueReceive(esp_now_queue, &queue_item, 0);
    if (r != pdTRUE) return mp_const_none;
    mp_obj_tuple_t *msg = mp_obj_new_tuple(2, NULL);
    msg->items[0] = mp_obj_new_bytes(queue_item.macaddr, ESP_NOW_ETH_ALEN);
    msg->items[1] = mp_obj_new_bytes(queue_item.data, queue_item.len);
    return msg;
}

MP_DEFINE_CONST_FUN_OBJ_0(espnow_recv_obj, espnow_recv);

void recv_cb(const uint8_t *macaddr, const uint8_t *data, int len) 
{
    // this is double copying, perhaps I should be just queueing the pointers
    static esp_now_queue_t queue_item = { 0 };
    queue_item.len = len;
    memcpy(queue_item.macaddr, macaddr, ESP_NOW_ETH_ALEN);
    memcpy(queue_item.data, data, len);
    xQueueSend(esp_now_queue, &queue_item, 0);
} 

static int initialized = 0;

STATIC mp_obj_t espnow_init() {
    if (!initialized) {
        esp_now_init();
	esp_now_queue = xQueueCreate(5, sizeof(esp_now_queue_t));
        initialized = 1;
	esp_now_register_recv_cb(recv_cb);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(espnow_init_obj, espnow_init);

STATIC mp_obj_t espnow_deinit() {
    if (initialized) {
        esp_now_deinit();
	vQueueDelete(esp_now_queue);
        initialized = 0;
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(espnow_deinit_obj, espnow_deinit);

STATIC mp_obj_t espnow_set_pmk(mp_obj_t pmk) {
    uint8_t buf[ESP_NOW_KEY_LEN];
    _get_bytes(pmk, ESP_NOW_KEY_LEN, buf);
    esp_espnow_exceptions(esp_now_set_pmk(buf));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_set_pmk_obj, espnow_set_pmk);

STATIC mp_obj_t espnow_add_peer(size_t n_args, const mp_obj_t *args) {
    esp_now_peer_info_t peer = {0};
    // leaving channel as 0 for autodetect
    peer.ifidx = ((wlan_if_obj_t *)MP_OBJ_TO_PTR(args[0]))->if_id;
    _get_bytes(args[1], ESP_NOW_ETH_ALEN, peer.peer_addr);
    if (n_args > 2) {
        _get_bytes(args[2], ESP_NOW_KEY_LEN, peer.lmk);
        peer.encrypt = 1;
    }
    esp_espnow_exceptions(esp_now_add_peer(&peer));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espnow_add_peer_obj, 2, 3, espnow_add_peer);

STATIC mp_obj_t espnow_send(mp_obj_t addr, mp_obj_t msg) {
    mp_uint_t len1;
    const uint8_t *buf1 = (const uint8_t *)mp_obj_str_get_data(addr, &len1);
    mp_uint_t len2;
    const uint8_t *buf2 = (const uint8_t *)mp_obj_str_get_data(msg, &len2);
    if (len1 != ESP_NOW_ETH_ALEN) mp_raise_ValueError("addr invalid");
    if (len2 > ESP_NOW_MAX_DATA_LEN) mp_raise_ValueError("Msg too long");
    esp_espnow_exceptions(esp_now_send(buf1, buf2, len2));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(espnow_send_obj, espnow_send);

STATIC mp_obj_t espnow_send_all(mp_obj_t msg) {
    mp_uint_t len;
    const uint8_t *buf = (const uint8_t *)mp_obj_str_get_data(msg, &len);
    if (len > ESP_NOW_MAX_DATA_LEN) mp_raise_ValueError("Msg too long");
    esp_espnow_exceptions(esp_now_send(NULL, buf, len));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_send_all_obj, espnow_send_all);

STATIC const mp_rom_map_elem_t espnow_globals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&espnow_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&espnow_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pmk), MP_ROM_PTR(&espnow_set_pmk_obj) },
    { MP_ROM_QSTR(MP_QSTR_add_peer), MP_ROM_PTR(&espnow_add_peer_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&espnow_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_send_all), MP_ROM_PTR(&espnow_send_all_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&espnow_recv_obj) },
};
STATIC MP_DEFINE_CONST_DICT(espnow_globals_dict, espnow_globals_dict_table);

const mp_obj_module_t mp_module_esp_espnow = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&espnow_globals_dict,
};
