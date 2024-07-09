// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/enum.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/max3421e/__init__.h"
#include "shared-bindings/max3421e/Max3421E.h"

//| """Provide USB host via a connected MAX3421E chip.
//|
//| Here is how to test with the MAX3421E featherwing:
//|
//| .. code-block:: python
//|
//|    import board
//|    import max3421e
//|    import time
//|    import usb
//|
//|    spi = board.SPI()
//|    cs = board.D10
//|    irq = board.D9
//|
//|    host_chip = max3421e.Max3421E(spi, chip_select=cs, irq=irq)
//|
//|    while True:
//|       print("Finding devices:")
//|       for device in usb.core.find(find_all=True):
//|           print(f"{device.idVendor:04x}:{device.idProduct:04x}: {device.manufacturer} {device.product}")
//|       time.sleep(5)
//|
//| """

static const mp_rom_map_elem_t max3421e_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_max3421e) },

    { MP_ROM_QSTR(MP_QSTR_Max3421E), MP_ROM_PTR(&max3421e_max3421e_type) },
};
static MP_DEFINE_CONST_DICT(max3421e_module_globals, max3421e_module_globals_table);

const mp_obj_module_t max3421e_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&max3421e_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_max3421e, max3421e_module);
