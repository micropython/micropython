/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Sokolovsky
 * Copyright (c) 2015 Josef Gajdusek
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
#include <string.h>
#include <stdbool.h>

#include "py/nlr.h"
#include "py/obj.h"
#include "py/gc.h"
#include "py/runtime.h"
#include MICROPY_HAL_H
#include "netutils.h"
#include "queue.h"
#include "user_interface.h"
#include "espconn.h"
#include "ip_addr.h"
#include "spi_flash.h"
#include "utils.h"

#define MODESP_INCLUDE_CONSTANTS (1)

static void error_check(bool status, const char *msg) {
    if (!status) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, msg));
    }
}

STATIC void esp_wifi_scan_cb(scaninfo *si, STATUS status) {
    //printf("in pyb_scan_cb: %d, si=%p, si->pbss=%p\n", status, si, si->pbss);
    struct bss_info *bs;
    if (si->pbss) {
        STAILQ_FOREACH(bs, si->pbss, next) {
            mp_obj_tuple_t *t = mp_obj_new_tuple(6, NULL);
            t->items[0] = mp_obj_new_bytes(bs->ssid, strlen((char*)bs->ssid));
            t->items[1] = mp_obj_new_bytes(bs->bssid, sizeof(bs->bssid));
            t->items[2] = MP_OBJ_NEW_SMALL_INT(bs->channel);
            t->items[3] = MP_OBJ_NEW_SMALL_INT(bs->rssi);
            t->items[4] = MP_OBJ_NEW_SMALL_INT(bs->authmode);
            t->items[5] = MP_OBJ_NEW_SMALL_INT(bs->is_hidden);
            call_function_1_protected(MP_STATE_PORT(scan_cb_obj), t);
        }
    }
}

STATIC mp_obj_t esp_wifi_scan(mp_obj_t cb_in) {
    MP_STATE_PORT(scan_cb_obj) = cb_in;
    wifi_set_opmode(STATION_MODE);
    wifi_station_scan(NULL, (scan_done_cb_t)esp_wifi_scan_cb);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_wifi_scan_obj, esp_wifi_scan);

STATIC mp_obj_t esp_wifi_connect(mp_uint_t n_args, const mp_obj_t *args) {
    struct station_config config = {{0}};
    mp_uint_t len;
    const char *p;

    p = mp_obj_str_get_data(args[0], &len);
    memcpy(config.ssid, p, len);
    p = mp_obj_str_get_data(args[1], &len);
    memcpy(config.password, p, len);

    error_check(wifi_station_set_config(&config), "Cannot set STA config");
    error_check(wifi_station_connect(), "Cannot connect to AP");

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_wifi_connect_obj, esp_wifi_connect);

STATIC mp_obj_t esp_wifi_disconnect() {
    error_check(wifi_station_disconnect(), "Cannot disconnect from AP");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_wifi_disconnect_obj, esp_wifi_disconnect);

STATIC mp_obj_t esp_wifi_status() {
    return MP_OBJ_NEW_SMALL_INT(wifi_station_get_connect_status());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_wifi_status_obj, esp_wifi_status);

STATIC mp_obj_t esp_wifi_phy_mode(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_obj_new_int(wifi_get_phy_mode());
    } else {
        wifi_set_phy_mode(mp_obj_get_int(args[0]));
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_wifi_phy_mode_obj, 0, 1, esp_wifi_phy_mode);

STATIC mp_obj_t esp_wifi_sleep_type(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_obj_new_int(wifi_get_sleep_type());
    } else {
        wifi_set_sleep_type(mp_obj_get_int(args[0]));
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_wifi_sleep_type_obj, 0, 1, esp_wifi_sleep_type);

STATIC mp_obj_t esp_wifi_mac(mp_uint_t n_args, const mp_obj_t *args) {
    uint8_t mac[6];
    if (n_args == 0) {
        wifi_get_macaddr(STATION_IF, mac);
        return mp_obj_new_bytes(mac, sizeof(mac));
    } else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);

        if (bufinfo.len != 6) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                "invalid buffer length"));
        }

        wifi_set_macaddr(STATION_IF, bufinfo.buf);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_wifi_mac_obj, 0, 1, esp_wifi_mac);

STATIC const mp_map_elem_t esp_wifi_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_esp_dot_wifi) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), (mp_obj_t)&esp_wifi_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect), (mp_obj_t)&esp_wifi_disconnect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_scan), (mp_obj_t)&esp_wifi_scan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_status), (mp_obj_t)&esp_wifi_status_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mac), (mp_obj_t)&esp_wifi_mac_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_phy_mode), (mp_obj_t)&esp_wifi_phy_mode_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep_type), (mp_obj_t)&esp_wifi_sleep_type_obj },

#if MODESP_INCLUDE_CONSTANTS
    { MP_OBJ_NEW_QSTR(MP_QSTR_MODE_11B),
        MP_OBJ_NEW_SMALL_INT(PHY_MODE_11B) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MODE_11G),
        MP_OBJ_NEW_SMALL_INT(PHY_MODE_11G) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MODE_11N),
        MP_OBJ_NEW_SMALL_INT(PHY_MODE_11N) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SLEEP_NONE),
        MP_OBJ_NEW_SMALL_INT(NONE_SLEEP_T) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SLEEP_LIGHT),
        MP_OBJ_NEW_SMALL_INT(LIGHT_SLEEP_T) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SLEEP_MODEM),
        MP_OBJ_NEW_SMALL_INT(MODEM_SLEEP_T) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_STAT_IDLE),
        MP_OBJ_NEW_SMALL_INT(STATION_IDLE)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_STAT_CONNECTING),
        MP_OBJ_NEW_SMALL_INT(STATION_CONNECTING)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_STAT_WRONG_PASSWORD),
        MP_OBJ_NEW_SMALL_INT(STATION_WRONG_PASSWORD)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_STAT_NO_AP_FOUND),
        MP_OBJ_NEW_SMALL_INT(STATION_NO_AP_FOUND)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_STAT_CONNECT_FAIL),
        MP_OBJ_NEW_SMALL_INT(STATION_CONNECT_FAIL)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_STAT_GOT_IP),
        MP_OBJ_NEW_SMALL_INT(STATION_GOT_IP)},
#endif
};

STATIC MP_DEFINE_CONST_DICT(esp_wifi_module_globals, esp_wifi_module_globals_table);

const mp_obj_module_t esp_wifi_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_esp_dot_wifi,
    .globals = (mp_obj_dict_t*)&esp_wifi_module_globals,
};
