/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
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

#include <string.h>

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/mdns/__init__.h"
#include "shared-bindings/mdns/Server.h"
#include "shared-bindings/util.h"

#if CIRCUITPY_WEB_WORKFLOW
#include "supervisor/shared/web_workflow/web_workflow.h"
#endif

//| class Server:
//|     """The MDNS Server responds to queries for this device's information and allows for querying
//|     other devices."""
//|

//|     def __init__(self, network_interface: wifi.Radio) -> None:
//|         """
//|         Constructs or returns the mdns.Server for the given network_interface. (CircuitPython
//|         may already be using it.) Only native interfaces are currently supported.
//|         """
//|         ...
STATIC mp_obj_t mdns_server_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_network_interface };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_network_interface, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    #if CIRCUITPY_WEB_WORKFLOW
    mdns_server_obj_t *web_workflow_mdns = supervisor_web_workflow_mdns(args[ARG_network_interface].u_obj);
    if (web_workflow_mdns != NULL) {
        return web_workflow_mdns;
    }
    #endif

    mdns_server_obj_t *self = m_new_obj_with_finaliser(mdns_server_obj_t);
    self->base.type = &mdns_server_type;
    common_hal_mdns_server_construct(self, args[ARG_network_interface].u_obj);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Stops the server"""
//|         ...
STATIC mp_obj_t mdns_server_obj_deinit(mp_obj_t self_in) {
    mdns_server_obj_t *self = (mdns_server_obj_t *)self_in;
    common_hal_mdns_server_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mdns_server_deinit_obj, mdns_server_obj_deinit);

STATIC void check_for_deinit(mdns_server_obj_t *self) {
    if (common_hal_mdns_server_deinited(self)) {
        raise_deinited_error();
    }
}

//|     hostname: str
//|     """Hostname resolvable as ``<hostname>.local`` in addition to ``circuitpython.local``. Make
//|        sure this is unique across all devices on the network. It defaults to ``cpy-######``
//|        where ``######`` is the hex digits of the last three bytes of the mac address."""
STATIC mp_obj_t mdns_server_get_hostname(mp_obj_t self) {
    check_for_deinit(self);
    const char *hostname = common_hal_mdns_server_get_hostname(self);
    return mp_obj_new_str(hostname, strlen(hostname));
}
MP_DEFINE_CONST_FUN_OBJ_1(mdns_server_get_hostname_obj, mdns_server_get_hostname);

static mp_obj_t mdns_server_set_hostname(mp_obj_t self, mp_obj_t hostname) {
    check_for_deinit(self);
    common_hal_mdns_server_set_hostname(self, mp_obj_str_get_str(hostname));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mdns_server_set_hostname_obj, mdns_server_set_hostname);

MP_PROPERTY_GETSET(mdns_server_hostname_obj,
    (mp_obj_t)&mdns_server_get_hostname_obj,
    (mp_obj_t)&mdns_server_set_hostname_obj);

//|     instance_name: str
//|     """Human readable name to describe the device."""
STATIC mp_obj_t mdns_server_get_instance_name(mp_obj_t self) {
    check_for_deinit(self);
    const char *instance_name = common_hal_mdns_server_get_instance_name(self);
    return mp_obj_new_str(instance_name, strlen(instance_name));
}
MP_DEFINE_CONST_FUN_OBJ_1(mdns_server_get_instance_name_obj, mdns_server_get_instance_name);

STATIC mp_obj_t mdns_server_set_instance_name(mp_obj_t self, mp_obj_t new_instance_name) {
    check_for_deinit(self);
    common_hal_mdns_server_set_instance_name(self, mp_obj_str_get_str(new_instance_name));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mdns_server_set_instance_name_obj, mdns_server_set_instance_name);

MP_PROPERTY_GETSET(mdns_server_instance_name_obj,
    (mp_obj_t)&mdns_server_get_instance_name_obj,
    (mp_obj_t)&mdns_server_set_instance_name_obj);


//|     def find(
//|         self, service_type: str, protocol: str, *, timeout: float = 1
//|     ) -> Tuple[RemoteService]:
//|         """Find all locally available remote services with the given service type and protocol.
//|
//|         This doesn't allow for direct hostname lookup. To do that, use
//|         `socketpool.SocketPool.getaddrinfo()`.
//|
//|         :param str service_type: The service type such as "_http"
//|         :param str protocol: The service protocol such as "_tcp"
//|         :param float/int timeout: Time to wait for responses"""
//|         ...
STATIC mp_obj_t _mdns_server_find(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mdns_server_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);

    enum { ARG_service_type, ARG_protocol, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_service_type, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_protocol, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(1)} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_float_t timeout = mp_obj_get_float(args[ARG_timeout].u_obj);
    const char *service_type = mp_obj_str_get_str(args[ARG_service_type].u_obj);
    const char *protocol = mp_obj_str_get_str(args[ARG_protocol].u_obj);

    return common_hal_mdns_server_find(self, service_type, protocol, timeout);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mdns_server_find_obj, 1, _mdns_server_find);

//|     def advertise_service(self, *, service_type: str, protocol: str, port: int) -> None:
//|         """Respond to queries for the given service with the given port.
//|
//|         ``service_type`` and ``protocol`` can only occur on one port. Any call after the first
//|         will update the entry's port.
//|
//|         If web workflow is active, the port it uses can't also be used to advertise a service.
//|
//|         **Limitations**: Publishing up to 32 TXT records is only supported on the RP2040 Pico W board at
//|         this time.
//|
//|         :param str service_type: The service type such as "_http"
//|         :param str protocol: The service protocol such as "_tcp"
//|         :param int port: The port used by the service
//|         :param Sequence[str] txt_records: An optional sequence of strings to serve as TXT records along with the service
//|         """
//|         ...
//|
STATIC mp_obj_t mdns_server_advertise_service(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mdns_server_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);

    enum { ARG_service_type, ARG_protocol, ARG_port, ARG_txt_records };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_service_type, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_protocol, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_port, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_txt_records, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const char *service_type = mp_obj_str_get_str(args[ARG_service_type].u_obj);
    const char *protocol = mp_obj_str_get_str(args[ARG_protocol].u_obj);

    const mp_obj_t txt_records = args[ARG_txt_records].u_obj;
    const size_t num_txt_records = txt_records == mp_const_none
        ? 0
        : (size_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len(txt_records));

    const char *txt_records_array[num_txt_records];
    for (size_t i = 0; i < num_txt_records; i++) {
        mp_obj_t txt_record = mp_obj_subscr(txt_records, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL);
        if (!mp_obj_is_str_or_bytes(txt_record)) {
            mp_raise_ValueError(MP_ERROR_TEXT("Failed to add service TXT record; non-string or bytes found in txt_records"));
        }
        txt_records_array[i] = mp_obj_str_get_str(txt_record);
    }

    common_hal_mdns_server_advertise_service(self, service_type, protocol, args[ARG_port].u_int, txt_records_array, num_txt_records);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mdns_server_advertise_service_obj, 1, mdns_server_advertise_service);

STATIC const mp_rom_map_elem_t mdns_server_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_hostname),          MP_ROM_PTR(&mdns_server_hostname_obj) },
    { MP_ROM_QSTR(MP_QSTR_instance_name),     MP_ROM_PTR(&mdns_server_instance_name_obj) },

    { MP_ROM_QSTR(MP_QSTR_find),              MP_ROM_PTR(&mdns_server_find_obj) },
    { MP_ROM_QSTR(MP_QSTR_advertise_service), MP_ROM_PTR(&mdns_server_advertise_service_obj) },

    { MP_ROM_QSTR(MP_QSTR___del__),           MP_ROM_PTR(&mdns_server_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),            MP_ROM_PTR(&mdns_server_deinit_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mdns_server_locals_dict, mdns_server_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mdns_server_type,
    MP_QSTR_Server,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, mdns_server_make_new,
    locals_dict, &mdns_server_locals_dict
    );
