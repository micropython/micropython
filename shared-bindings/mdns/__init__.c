// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdarg.h>

#include "py/objexcept.h"
#include "py/runtime.h"
#include "shared-bindings/mdns/__init__.h"
#include "shared-bindings/mdns/Server.h"
#include "shared-bindings/mdns/RemoteService.h"

//| """Multicast Domain Name Service
//|
//| The `mdns` module provides basic support for multicast domain name services.
//| Basic use provides hostname resolution under the .local TLD. This module
//| also supports DNS Service Discovery that allows for discovering other hosts
//| that provide a desired service."""

static const mp_rom_map_elem_t mdns_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),             MP_ROM_QSTR(MP_QSTR_mdns) },
    { MP_ROM_QSTR(MP_QSTR_Server),               MP_ROM_PTR(&mdns_server_type) },
    { MP_ROM_QSTR(MP_QSTR_RemoteService),        MP_ROM_PTR(&mdns_remoteservice_type) },
};

static MP_DEFINE_CONST_DICT(mdns_module_globals, mdns_module_globals_table);

const mp_obj_module_t mdns_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mdns_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_mdns, mdns_module);
