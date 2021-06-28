/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Mark Komus
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

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/adafruit_bus_device/SPIDevice.h"
#include "shared-bindings/util.h"
#include "shared-module/adafruit_bus_device/SPIDevice.h"
#include "common-hal/digitalio/DigitalInOut.h"
#include "shared-bindings/digitalio/DigitalInOut.h"


#include "lib/utils/buffer_helper.h"
#include "lib/utils/context_manager_helpers.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"


//| class SPIDevice:
//|     """SPI Device Manager"""
//|
//|     def __init__(self, spi: busio.SPI, chip_select: microcontroller.Pin, *, baudrate: int = 100000, polarity: int = 0, phase: int = 0, extra_clocks : int = 0) -> None:
//|
//|         """
//|         Represents a single SPI device and manages locking the bus and the device address.
//|
//|         :param ~busio.SPI spi: The SPI bus the device is on
//|         :param ~digitalio.DigitalInOut chip_select: The chip select pin object that implements the DigitalInOut API.
//|         :param int extra_clocks: The minimum number of clock cycles to cycle the bus after CS is high. (Used for SD cards.)
//|
//|         Example::
//|
//|             import busio
//|             import digitalio
//|             from board import *
//|             from adafruit_bus_device.spi_device import SPIDevice
//|             with busio.SPI(SCK, MOSI, MISO) as spi_bus:
//|                 cs = digitalio.DigitalInOut(D10)
//|                 device = SPIDevice(spi_bus, cs)
//|                 bytes_read = bytearray(4)
//|                 # The object assigned to spi in the with statements below
//|                 # is the original spi_bus object. We are using the busio.SPI
//|                 # operations busio.SPI.readinto() and busio.SPI.write().
//|                 with device as spi:
//|                     spi.readinto(bytes_read)
//|                 # A second transaction
//|                 with device as spi:
//|                     spi.write(bytes_read)"""
//|     ...
//|
STATIC mp_obj_t adafruit_bus_device_spidevice_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    adafruit_bus_device_spidevice_obj_t *self = m_new_obj(adafruit_bus_device_spidevice_obj_t);
    self->base.type = &adafruit_bus_device_spidevice_type;
    enum { ARG_spi, ARG_chip_select, ARG_baudrate, ARG_polarity, ARG_phase, ARG_extra_clocks };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_spi, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_chip_select, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 100000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_extra_clocks, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    busio_spi_obj_t *spi = args[ARG_spi].u_obj;

    common_hal_adafruit_bus_device_spidevice_construct(MP_OBJ_TO_PTR(self), spi, args[ARG_chip_select].u_obj, args[ARG_baudrate].u_int, args[ARG_polarity].u_int,
        args[ARG_phase].u_int, args[ARG_extra_clocks].u_int);

    if (args[ARG_chip_select].u_obj != MP_OBJ_NULL) {
        digitalinout_result_t result = common_hal_digitalio_digitalinout_switch_to_output(MP_OBJ_TO_PTR(args[ARG_chip_select].u_obj),
            true, DRIVE_MODE_PUSH_PULL);
        if (result == DIGITALINOUT_INPUT_ONLY) {
            mp_raise_NotImplementedError(translate("Pin is input only"));
        }
    }

    return (mp_obj_t)self;
}

//|     def __enter__(self) -> busio.SPI:
//|         """Starts a SPI transaction by configuring the SPI and asserting chip select."""
//|         ...
//|
STATIC mp_obj_t adafruit_bus_device_spidevice_obj___enter__(mp_obj_t self_in) {
    adafruit_bus_device_spidevice_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_adafruit_bus_device_spidevice_enter(self);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adafruit_bus_device_spidevice___enter___obj, adafruit_bus_device_spidevice_obj___enter__);


//|     def __exit__(self) -> None:
//|         """Ends a SPI transaction by deasserting chip select. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t adafruit_bus_device_spidevice_obj___exit__(size_t n_args, const mp_obj_t *args) {
    common_hal_adafruit_bus_device_spidevice_exit(MP_OBJ_TO_PTR(args[0]));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(adafruit_bus_device_spidevice___exit___obj, 4, 4, adafruit_bus_device_spidevice_obj___exit__);

STATIC const mp_rom_map_elem_t adafruit_bus_device_spidevice_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&adafruit_bus_device_spidevice___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&adafruit_bus_device_spidevice___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(adafruit_bus_device_spidevice_locals_dict, adafruit_bus_device_spidevice_locals_dict_table);

const mp_obj_type_t adafruit_bus_device_spidevice_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPIDevice,
    .make_new = adafruit_bus_device_spidevice_make_new,
    .locals_dict = (mp_obj_dict_t *)&adafruit_bus_device_spidevice_locals_dict,
};
