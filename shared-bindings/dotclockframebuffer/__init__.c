/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>

#include "py/enum.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/dotclockframebuffer/__init__.h"
#include "shared-bindings/dotclockframebuffer/DotClockFramebuffer.h"

//| """Native helpers for driving parallel displays"""
//|
//| Length = typing.Literal[1, 2]
//|
//| def ioexpander_send_init_sequence(
//|     bus: busio.I2C,
//|     init_sequence: ReadableBuffer,
//|     *,
//|     i2c_init_sequence: ReadableBuffer,
//|     i2c_address: int,
//|     gpio_address: int,
//|     gpio_data_len: Length,
//|     gpio_data: int,
//|     cs_bit: int,
//|     mosi_bit: int,
//|     clk_bit: int,
//|     reset_bit: Optional[int],
//| ):
//|     """Send a displayio-style initialization sequence over an I2C I/O expander
//|
//|     This function is highly generic in order to support various I/O expanders.
//|     What's assumed is that all the GPIO can be updated by writing to a single I2C register.
//|     Normal output polarity is assumed (CS and CLK are active low, data is not inverted).
//|     Only 8-bit I2C addresses are supported.
//|     8- and 16-bit I2C addresses and data registers are supported.
//|     The Data/Command bit is sent as part of the serial data.
//|
//|     Normally this function is used via a convenience library that is specific to the display & I/O expander in use.
//|
//|     If the board has an integrated I/O expander, ``**board.TFT_IO_EXPANDER`` expands to the proper arguments starting with ``gpio_address``.
//|     Note that this may include the ``i2c_init_sequence`` argument which can change the direction & value of I/O expander pins.
//|     If this is undesirable, take a copy of ``TFT_IO_EXPANDER`` and change or remove the ``i2c_init_sequence`` key.
//|
//|     If the board has an integrated display that requires an initialization sequence, ``board.TFT_INIT_SEQUENCE`` is the initialization string for the display.
//|
//|     :param busio.I2C bus: The I2C bus where the I/O expander resides
//|     :param int busio.i2c_address: The I2C bus address of the I/O expander
//|     :param ReadableBuffer init_sequence: The initialization sequence to send to the display
//|     :param int gpio_address: The address portion of the I2C transaction (1 byte)
//|     :param int gpio_data_len: The size of the data portion of the I2C transaction, 1 or 2 bytes
//|     :param int gpio_data: The output value for all GPIO bits other than cs, mosi, and clk (needed because GPIO expanders may be unable to read back the current output value)
//|     :param int cs_bit: The bit number (from 0 to 7, or from 0 to 15) of the chip select bit in the GPIO register
//|     :param int mosi_value: The bit number (from 0 to 7, or from 0 to 15) of the data out bit in the GPIO register
//|     :param int clk_value: The bit number (from 0 to 7, or from 0 to 15) of the clock out bit in the GPIO register
//|     :param Optional[int] reset_value: The bit number (from 0 to 7, or from 0 to 15) of the display reset bit in the GPIO register
//|     :param Optional[ReadableBuffer] i2c_init_sequence: An initialization sequence to send to the I2C expander
//|     """
//|

STATIC mp_obj_t ioexpander_send_init_sequence(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bus, ARG_init_sequence, ARG_i2c_address, ARG_gpio_address, ARG_gpio_data_len, ARG_gpio_data, ARG_cs_bit, ARG_mosi_bit, ARG_clk_bit, ARG_reset_bit, ARG_i2c_init_sequence, NUM_ARGS };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bus, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_init_sequence, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_i2c_address, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_gpio_address, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_gpio_data_len, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_gpio_data, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_cs_bit, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_mosi_bit, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_clk_bit, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_reset_bit, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = MP_ROM_NONE } },
        { MP_QSTR_i2c_init_sequence, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = MP_ROM_NONE } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t bus_obj = mp_arg_validate_type(args[ARG_bus].u_obj, &busio_i2c_type, MP_QSTR_bus);
    mp_int_t i2c_address = args[ARG_i2c_address].u_int;
    mp_int_t gpio_address = args[ARG_gpio_address].u_int;
    mp_int_t gpio_data_len = args[ARG_gpio_data_len].u_int;
    mp_int_t gpio_data = args[ARG_gpio_data].u_int;
    mp_int_t cs_bit = args[ARG_cs_bit].u_int;
    mp_int_t mosi_bit = args[ARG_mosi_bit].u_int;
    mp_int_t clk_bit = args[ARG_clk_bit].u_int;

    mp_buffer_info_t bufinfo_display_init_sequence;
    mp_get_buffer_raise(args[ARG_init_sequence].u_obj, &bufinfo_display_init_sequence, MP_BUFFER_READ);

    mp_arg_validate_int_range(i2c_address, 0, 127, MP_QSTR_i2c_address);
    mp_arg_validate_int_range(gpio_data_len, 1, 2, MP_QSTR_gpio_dat_len);

    int max_bit = gpio_data_len * 8 - 1;
    mp_arg_validate_int_range(cs_bit, 0, max_bit, MP_QSTR_cs_bit);
    mp_arg_validate_int_range(mosi_bit, 0, max_bit, MP_QSTR_mosi_bit);
    mp_arg_validate_int_range(clk_bit, 0, max_bit, MP_QSTR_clk_bit);
    mp_arg_validate_int_range(gpio_data, 0, (1 << (max_bit * 8)) - 1, MP_QSTR_gpio_data);
    mp_int_t reset_mask = 0;
    if (args[ARG_reset_bit].u_obj != MP_ROM_NONE) {
        mp_int_t reset_bit = mp_arg_validate_int_range(mp_arg_validate_type_int(args[ARG_reset_bit].u_obj, MP_QSTR_reset_bit), 0, max_bit, MP_QSTR_reset_bit);
        reset_mask = 0x100 << reset_bit;
    }

    mp_buffer_info_t bufinfo_i2c_init_sequence = {};
    if (args[ARG_i2c_init_sequence].u_obj != mp_const_none) {
        mp_get_buffer_raise(args[ARG_i2c_init_sequence].u_obj, &bufinfo_i2c_init_sequence, MP_BUFFER_READ);
    }

    dotclockframebuffer_ioexpander_spi_bus b = {
        .bus = bus_obj,
        .i2c_device_address = i2c_address,
        .i2c_write_size = 1 + gpio_data_len,
        // ASSERT(LITTLE_ENDIAN, "don't have to differentiate 8- vs 16-bits here")
        .addr_reg_shadow = { .u32 = gpio_address | (gpio_data << 8) },
        .cs_mask = 0x100 << cs_bit,
            .mosi_mask = 0x100 << mosi_bit,
            .clk_mask = 0x100 << clk_bit,
            .reset_mask = reset_mask,
    };

    dotclockframebuffer_ioexpander_send_init_sequence(&b, &bufinfo_i2c_init_sequence, &bufinfo_display_init_sequence);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(ioexpander_send_init_sequence_obj, 0, ioexpander_send_init_sequence);

STATIC const mp_rom_map_elem_t dotclockframebuffer_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_dotclockframebuffer) },
    { MP_ROM_QSTR(MP_QSTR_DotClockFramebuffer), MP_ROM_PTR(&dotclockframebuffer_framebuffer_type) },
    { MP_ROM_QSTR(MP_QSTR_ioexpander_send_init_sequence), MP_ROM_PTR(&ioexpander_send_init_sequence_obj) },
};

STATIC MP_DEFINE_CONST_DICT(dotclockframebuffer_module_globals, dotclockframebuffer_module_globals_table);

const mp_obj_module_t dotclockframebuffer_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&dotclockframebuffer_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_dotclockframebuffer, dotclockframebuffer_module);
