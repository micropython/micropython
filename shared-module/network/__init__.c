/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nick Moore
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

#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"

#include "supervisor/shared/tick.h"

#include "shared-bindings/random/__init__.h"

#include "shared-module/network/__init__.h"

// mod_network_nic_list needs to be declared in mpconfigport.h


void network_module_init(void) {
    mp_obj_list_init(&MP_STATE_PORT(mod_network_nic_list), 0);
}

void network_module_deinit(void) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mod_network_nic_list).len; i++) {
        mp_obj_t nic = MP_STATE_PORT(mod_network_nic_list).items[i];
        mod_network_nic_type_t *nic_type = (mod_network_nic_type_t *)mp_obj_get_type(nic);
        if (nic_type->deinit != NULL) {
            nic_type->deinit(nic);
        }
    }
    mp_obj_list_set_len(&MP_STATE_PORT(mod_network_nic_list), 0);
}

void network_module_background(void) {
    static uint32_t next_tick = 0;
    uint32_t this_tick = supervisor_ticks_ms32();
    if (this_tick < next_tick) {
        return;
    }
    next_tick = this_tick + 1000;

    for (mp_uint_t i = 0; i < MP_STATE_PORT(mod_network_nic_list).len; i++) {
        mp_obj_t nic = MP_STATE_PORT(mod_network_nic_list).items[i];
        mod_network_nic_type_t *nic_type = (mod_network_nic_type_t *)mp_obj_get_type(nic);
        if (nic_type->timer_tick != NULL) {
            nic_type->timer_tick(nic);
        }
    }
}

void network_module_register_nic(mp_obj_t nic) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mod_network_nic_list).len; i++) {
        if (MP_STATE_PORT(mod_network_nic_list).items[i] == nic) {
            // nic already registered
            return;
        }
    }
    // nic not registered so add to list
    mp_obj_list_append(MP_OBJ_FROM_PTR(&MP_STATE_PORT(mod_network_nic_list)), nic);
}

mp_obj_t network_module_find_nic(const uint8_t *ip) {
    // find a NIC that is suited to given IP address
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mod_network_nic_list).len; i++) {
        mp_obj_t nic = MP_STATE_PORT(mod_network_nic_list).items[i];
        // TODO check IP suitability here
        // mod_network_nic_type_t *nic_type = (mod_network_nic_type_t*)mp_obj_get_type(nic);
        return nic;
    }

    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, translate("no available NIC")));
}

void network_module_create_random_mac_address(uint8_t *mac) {
    uint32_t rb1 = shared_modules_random_getrandbits(24);
    uint32_t rb2 = shared_modules_random_getrandbits(24);
    // first octet has multicast bit (0) cleared and local bit (1) set
    // everything else is just set randomly
    mac[0] = ((uint8_t)(rb1 >> 16) & 0xfe) | 0x02;
    mac[1] = (uint8_t)(rb1 >> 8);
    mac[2] = (uint8_t)(rb1);
    mac[3] = (uint8_t)(rb2 >> 16);
    mac[4] = (uint8_t)(rb2 >> 8);
    mac[5] = (uint8_t)(rb2);
}

uint16_t network_module_create_random_source_tcp_port(void) {
    return 0xc000 | shared_modules_random_getrandbits(14);
}
