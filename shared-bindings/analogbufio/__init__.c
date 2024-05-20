// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Lee Atkinson, MeanStride Technology, Inc.
//
// SPDX-License-Identifier: MIT
#include <stdint.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/analogbufio/__init__.h"
#include "shared-bindings/analogbufio/BufferedIn.h"

//| """Analog Buffered IO Hardware Support
//|
//| The `analogbufio` module contains classes to provide access to analog-to-digital
//| conversion and digital-to-analog (DAC) for multiple value transfer.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| TODO: For the essentials of `analogbufio`, see the `CircuitPython Essentials
//| Learn guide <https://learn.adafruit.com/circuitpython-essentials/circuitpython-analogbufio>`_
//|
//| TODO: For more information on using `analogbufio`, see `this additional Learn guide
//| <https://learn.adafruit.com/circuitpython-advanced-analog-inputs-and-outputs>`_
//| """

static const mp_rom_map_elem_t analogbufio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_analogbufio) },
    { MP_ROM_QSTR(MP_QSTR_BufferedIn),   MP_ROM_PTR(&analogbufio_bufferedin_type) },
};

static MP_DEFINE_CONST_DICT(analogbufio_module_globals, analogbufio_module_globals_table);

const mp_obj_module_t analogbufio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&analogbufio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_analogbufio, analogbufio_module);
