/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/mdns/RemoteService.h"

//| class RemoteService:
//|     """Encapsulates information about a remote service that was found during a search. This
//|     object may only be created by a `mdns.Server`. It has no user-visible constructor."""
//|

//|     def __init__(self) -> None:
//|         """Cannot be instantiated directly. Use `mdns.Server.find`."""
//|         ...

//|     hostname: str
//|     """The hostname of the device (read-only),."""
STATIC mp_obj_t mdns_remoteservice_get_hostname(mp_obj_t self_in) {
    mdns_remoteservice_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const char *hostname = common_hal_mdns_remoteservice_get_hostname(self);
    return mp_obj_new_str(hostname, strlen(hostname));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mdns_remoteservice_get_hostname_obj, mdns_remoteservice_get_hostname);

MP_PROPERTY_GETTER(mdns_remoteservice_hostname_obj,
    (mp_obj_t)&mdns_remoteservice_get_hostname_obj);

//|     instance_name: str
//|     """The human readable instance name for the service. (read-only)"""
STATIC mp_obj_t remoteservice_get_instance_name(mp_obj_t self_in) {
    mdns_remoteservice_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const char *instance_name = common_hal_mdns_remoteservice_get_instance_name(self);
    return mp_obj_new_str(instance_name, strlen(instance_name));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mdns_remoteservice_get_instance_name_obj, remoteservice_get_instance_name);

MP_PROPERTY_GETTER(mdns_remoteservice_instance_name_obj,
    (mp_obj_t)&mdns_remoteservice_get_instance_name_obj);

//|     service_type: str
//|     """The service type string such as ``_http``. (read-only)"""
STATIC mp_obj_t remoteservice_get_service_type(mp_obj_t self_in) {
    mdns_remoteservice_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const char *service_type = common_hal_mdns_remoteservice_get_service_type(self);
    return mp_obj_new_str(service_type, strlen(service_type));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mdns_remoteservice_get_service_type_obj, remoteservice_get_service_type);

MP_PROPERTY_GETTER(mdns_remoteservice_service_type_obj,
    (mp_obj_t)&mdns_remoteservice_get_service_type_obj);

//|     protocol: str
//|     """The protocol string such as ``_tcp``. (read-only)"""
STATIC mp_obj_t remoteservice_get_protocol(mp_obj_t self_in) {
    mdns_remoteservice_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const char *protocol = common_hal_mdns_remoteservice_get_protocol(self);
    return mp_obj_new_str(protocol, strlen(protocol));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mdns_remoteservice_get_protocol_obj, remoteservice_get_protocol);

MP_PROPERTY_GETTER(mdns_remoteservice_protocol_obj,
    (mp_obj_t)&mdns_remoteservice_get_protocol_obj);

//|     port: int
//|     """Port number used for the service. (read-only)"""
STATIC mp_obj_t remoteservice_get_port(mp_obj_t self_in) {
    mdns_remoteservice_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_mdns_remoteservice_get_port(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mdns_remoteservice_get_port_obj, remoteservice_get_port);

MP_PROPERTY_GETTER(mdns_remoteservice_port_obj,
    (mp_obj_t)&mdns_remoteservice_get_port_obj);

//|     ipv4_address: Optional[ipaddress.IPv4Address]
//|     """IP v4 Address of the remote service. None if no A records are found."""
STATIC mp_obj_t _mdns_remoteservice_get_ipv4_address(mp_obj_t self) {
    return common_hal_mdns_remoteservice_get_ipv4_address(self);

}
MP_DEFINE_CONST_FUN_OBJ_1(mdns_remoteservice_get_ipv4_address_obj, _mdns_remoteservice_get_ipv4_address);

MP_PROPERTY_GETTER(mdns_remoteservice_ipv4_address_obj,
    (mp_obj_t)&mdns_remoteservice_get_ipv4_address_obj);

//|     def __del__(self) -> None:
//|         """Deletes the RemoteService object."""
//|         ...
//|
STATIC mp_obj_t mdns_remoteservice_obj_deinit(mp_obj_t self_in) {
    mdns_remoteservice_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_mdns_remoteservice_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mdns_remoteservice_deinit_obj, mdns_remoteservice_obj_deinit);

STATIC const mp_rom_map_elem_t mdns_remoteservice_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_hostname),       MP_ROM_PTR(&mdns_remoteservice_hostname_obj) },
    { MP_ROM_QSTR(MP_QSTR_instance_name),  MP_ROM_PTR(&mdns_remoteservice_instance_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_service_type),   MP_ROM_PTR(&mdns_remoteservice_service_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_protocol),       MP_ROM_PTR(&mdns_remoteservice_protocol_obj) },
    { MP_ROM_QSTR(MP_QSTR_port),           MP_ROM_PTR(&mdns_remoteservice_port_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipv4_address),   MP_ROM_PTR(&mdns_remoteservice_ipv4_address_obj) },

    { MP_ROM_QSTR(MP_QSTR___del__),        MP_ROM_PTR(&mdns_remoteservice_deinit_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mdns_remoteservice_locals_dict, mdns_remoteservice_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mdns_remoteservice_type,
    MP_QSTR_RemoteService,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &mdns_remoteservice_locals_dict
    );
