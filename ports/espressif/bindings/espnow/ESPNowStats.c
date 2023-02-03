/*
 * This file is part of the CircuitPython project, https://github.com/adafruit/circuitpython
 *
 * The MIT License (MIT)
 *
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

#include "bindings/espnow/ESPNowStats.h"
#include "py/objproperty.h"

//| class ESPNowStats:
//|     """Provide some useful packet tx/rx statistics."""
//|

//|     success: int
//|     """The number of successes. (read-only)
//|
//|     * In case of transmit ``TX``:
//|         The number of tx packets received by the peer(s) ``ESP_NOW_SEND_SUCCESS``.
//|
//|     * In case of receive ``RX``:
//|         The number of rx packets captured in the buffer."""
//|
STATIC mp_obj_t espnow_stats_get_success(const mp_obj_t self_in) {
    espnow_stats_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(self->success);
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_stats_get_success_obj, espnow_stats_get_success);

MP_PROPERTY_GETTER(espnow_stats_success_obj,
    (mp_obj_t)&espnow_stats_get_success_obj);

//|     failure: int
//|     """The number of failures. (read-only)
//|
//|     * In case of transmit ``TX``:
//|         The number of failed tx packets ``ESP_NOW_SEND_FAIL``.
//|
//|     * In case of receive ``RX``:
//|         The number of dropped rx packets due to buffer overflow."""
//|
STATIC mp_obj_t espnow_stats_get_failure(const mp_obj_t self_in) {
    espnow_stats_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(self->failure);
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_stats_get_failure_obj, espnow_stats_get_failure);

MP_PROPERTY_GETTER(espnow_stats_failure_obj,
    (mp_obj_t)&espnow_stats_get_failure_obj);

STATIC const mp_rom_map_elem_t espnow_stats_locals_dict_table[] = {
    // Peer parameters
    { MP_ROM_QSTR(MP_QSTR_success), MP_ROM_PTR(&espnow_stats_success_obj) },
    { MP_ROM_QSTR(MP_QSTR_failure), MP_ROM_PTR(&espnow_stats_failure_obj) },
};
STATIC MP_DEFINE_CONST_DICT(espnow_stats_locals_dict, espnow_stats_locals_dict_table);

espnow_stats_obj_t *espnow_stats_new(void) {
    espnow_stats_obj_t *self = m_new_obj(espnow_stats_obj_t);
    self->base.type = &espnow_stats_type;
    return self;
}

const mp_obj_type_t espnow_stats_type = {
    { &mp_type_type },
    .name = MP_QSTR_ESPNowStats,
    .locals_dict = (mp_obj_t)&espnow_stats_locals_dict,
};
