/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/objarray.h"

#include "shared-bindings/sdcardio/SDCard.h"
#include "shared-module/sdcardio/SDCard.h"
#include "common-hal/busio/SPI.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/flash.h"

//| class SDCard:
//|     """SD Card Block Interface
//|
//|     Controls an SD card over SPI.  This built-in module has higher read
//|     performance than the library adafruit_sdcard, but it is only compatible with
//|     `busio.SPI`, not `bitbangio.SPI`.  Usually an SDCard object is used
//|     with ``storage.VfsFat`` to allow file I/O to an SD card."""
//|
//|     def __init__(self, bus: busio.SPI, cs: microcontroller.Pin, baudrate: int = 8000000) -> None:
//|         """Construct an SPI SD Card object with the given properties
//|
//|         :param busio.SPI spi: The SPI bus
//|         :param microcontroller.Pin cs: The chip select connected to the card
//|         :param int baudrate: The SPI data rate to use after card setup
//|
//|         Note that during detection and configuration, a hard-coded low baudrate is used.
//|         Data transfers use the specified baurate (rounded down to one that is supported by
//|         the microcontroller)
//|
//|         .. important::
//|            If the same SPI bus is shared with other peripherals, it is important that
//|            the SD card be initialized before accessing any other peripheral on the bus.
//|            Failure to do so can prevent the SD card from being recognized until it is
//|            powered off or re-inserted.
//|
//|         Example usage:
//|
//|         .. code-block:: python
//|
//|             import os
//|
//|             import board
//|             import sdcardio
//|             import storage
//|
//|             sd = sdcardio.SDCard(board.SPI(), board.SD_CS)
//|             vfs = storage.VfsFat(sd)
//|             storage.mount(vfs, '/sd')
//|             os.listdir('/sd')"""

STATIC mp_obj_t sdcardio_sdcard_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_spi, ARG_cs, ARG_baudrate, ARG_sdio, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_spi, MP_ARG_OBJ, {.u_obj = mp_const_none } },
        { MP_QSTR_cs, MP_ARG_OBJ, {.u_obj = mp_const_none } },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 8000000} },
        { MP_QSTR_sdio, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_int = 8000000} },
    };
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    busio_spi_obj_t *spi = validate_obj_is_spi_bus(args[ARG_spi].u_obj);
    mcu_pin_obj_t *cs = validate_obj_is_free_pin(args[ARG_cs].u_obj);

    sdcardio_sdcard_obj_t *self = m_new_obj(sdcardio_sdcard_obj_t);
    self->base.type = &sdcardio_SDCard_type;

    common_hal_sdcardio_sdcard_construct(self, spi, cs, args[ARG_baudrate].u_int);

    return self;
}


//|     def count(self) -> int:
//|         """Returns the total number of sectors
//|
//|         Due to technical limitations, this is a function and not a property.
//|
//|         :return: The number of 512-byte blocks, as a number"""
//|
mp_obj_t sdcardio_sdcard_count(mp_obj_t self_in) {
    sdcardio_sdcard_obj_t *self = (sdcardio_sdcard_obj_t *)self_in;
    return mp_obj_new_int_from_ull(common_hal_sdcardio_sdcard_get_blockcount(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(sdcardio_sdcard_count_obj, sdcardio_sdcard_count);

//|     def deinit(self) -> None:
//|         """Disable permanently.
//|
//|         :return: None"""
//|
mp_obj_t sdcardio_sdcard_deinit(mp_obj_t self_in) {
    sdcardio_sdcard_obj_t *self = (sdcardio_sdcard_obj_t *)self_in;
    common_hal_sdcardio_sdcard_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(sdcardio_sdcard_deinit_obj, sdcardio_sdcard_deinit);


//|     def readblocks(self, start_block: int, buf: WriteableBuffer) -> None:
//|
//|         """Read one or more blocks from the card
//|
//|         :param int start_block: The block to start reading from
//|         :param ~_typing.WriteableBuffer buf: The buffer to write into.  Length must be multiple of 512.
//|
//|         :return: None"""
//|

mp_obj_t sdcardio_sdcard_readblocks(mp_obj_t self_in, mp_obj_t start_block_in, mp_obj_t buf_in) {
    uint32_t start_block = mp_obj_get_int(start_block_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);
    sdcardio_sdcard_obj_t *self = (sdcardio_sdcard_obj_t *)self_in;
    int result = common_hal_sdcardio_sdcard_readblocks(self, start_block, &bufinfo);
    if (result < 0) {
        mp_raise_OSError(-result);
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_3(sdcardio_sdcard_readblocks_obj, sdcardio_sdcard_readblocks);

//|     def writeblocks(self, start_block: int, buf: ReadableBuffer) -> None:
//|
//|         """Write one or more blocks to the card
//|
//|         :param int start_block: The block to start writing from
//|         :param ~_typing.ReadableBuffer buf: The buffer to read from.  Length must be multiple of 512.
//|
//|         :return: None"""
//|

mp_obj_t sdcardio_sdcard_writeblocks(mp_obj_t self_in, mp_obj_t start_block_in, mp_obj_t buf_in) {
    uint32_t start_block = mp_obj_get_int(start_block_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    sdcardio_sdcard_obj_t *self = (sdcardio_sdcard_obj_t *)self_in;
    int result = common_hal_sdcardio_sdcard_writeblocks(self, start_block, &bufinfo);
    if (result < 0) {
        mp_raise_OSError(-result);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(sdcardio_sdcard_writeblocks_obj, sdcardio_sdcard_writeblocks);

STATIC const mp_rom_map_elem_t sdcardio_sdcard_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&sdcardio_sdcard_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&sdcardio_sdcard_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&sdcardio_sdcard_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&sdcardio_sdcard_writeblocks_obj) },
};
STATIC MP_DEFINE_CONST_DICT(sdcardio_sdcard_locals_dict, sdcardio_sdcard_locals_dict_table);

const mp_obj_type_t sdcardio_SDCard_type = {
    { &mp_type_type },
    .name = MP_QSTR_SDCard,
    .make_new = sdcardio_sdcard_make_new,
    .locals_dict = (mp_obj_dict_t *)&sdcardio_sdcard_locals_dict,
};
