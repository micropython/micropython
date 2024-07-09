// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/terminalio/__init__.h"
#include "shared-bindings/terminalio/Terminal.h"
#include "supervisor/shared/display.h"

#include "py/runtime.h"

//| """Displays text in a TileGrid
//|
//| The `terminalio` module contains classes to display a character stream on a display. The built
//| in font is available as ``terminalio.FONT``.
//|
//| .. note:: This module does not give access to the
//|     `REPL <https://learn.adafruit.com/welcome-to-circuitpython/interacting-with-the-serial-console>`_.
//|
//| """
//|
//| FONT: fontio.BuiltinFont
//| """The built in font"""
static const mp_rom_map_elem_t terminalio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_terminalio) },
    { MP_ROM_QSTR(MP_QSTR_Terminal),   MP_OBJ_FROM_PTR(&terminalio_terminal_type) },
    { MP_ROM_QSTR(MP_QSTR_FONT), MP_ROM_PTR(&supervisor_terminal_font) },
};


static MP_DEFINE_CONST_DICT(terminalio_module_globals, terminalio_module_globals_table);

const mp_obj_module_t terminalio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&terminalio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_terminalio, terminalio_module);
