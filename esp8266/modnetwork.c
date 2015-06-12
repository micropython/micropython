/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Sokolovsky
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
#include <errno.h>

#include "py/nlr.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "netutils.h"
#include "queue.h"
#include "user_interface.h"
#include "espconn.h"
#include "ip_addr.h"
#include "spi_flash.h"
#include "utils.h"

void error_check(bool status, const char *msg);
extern const mp_obj_module_t network_module;

STATIC mp_obj_t get_module() {
    return (mp_obj_t)&network_module;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(get_module_obj, get_module);

STATIC mp_obj_t esp_connect(mp_uint_t n_args, const mp_obj_t *args) {
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
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_connect_obj, 2, 6, esp_connect);

STATIC mp_obj_t esp_disconnect() {
    error_check(wifi_station_disconnect(), "Cannot disconnect from AP");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_disconnect_obj, esp_disconnect);

STATIC const mp_map_elem_t mp_module_network_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_network) },
    // MicroPython "network" module interface requires it to contains classes
    // to instantiate. But as we have just a static network interace,
    // use module as a "class", and just make all methods module-global
    // functions.
    { MP_OBJ_NEW_QSTR(MP_QSTR_WLAN), (mp_obj_t)&get_module_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), (mp_obj_t)&esp_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect), (mp_obj_t)&esp_disconnect_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_network_globals, mp_module_network_globals_table);

const mp_obj_module_t network_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_network,
    .globals = (mp_obj_dict_t*)&mp_module_network_globals,
};
