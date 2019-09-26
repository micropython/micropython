/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 "Matt Trentini" <matt.trentini@gmail.com>
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

#include "py/runtime.h"

#include "driver/rmt.h"

#include "py/runtime.h"
#include "modmachine.h"
#include "mphalport.h"

// Forward declaration
extern const mp_obj_type_t esp32_rmt_type;

typedef struct _esp32_rmt_obj_t {
    mp_obj_base_t base;
    uint8_t channel_id;
    gpio_num_t pin;
    uint8_t clock_divider;
} esp32_rmt_obj_t;


// TODO
//
//   o Write up documentation
//   o Check all the input parameters
//   o Return exceptions when errors occur
//   o Manage an array of channels (so user can't create the same one repeatedly)
//     - Check if channel is initialised (in particular: after deinit)
//   o Investigate: Send without blocking? (Need an 'is_sending' method)
//     - Memory management becomes significantly more complex. Need to ensure memory is not freed before it's used.
//     - Also need to take care that memory is not freed by the MicroPython GC.
//   o Add debug option? Emit printf debug messages?
//     - Idea: Generate timing diagram (maybe with https://wavedrom.com/ ?) Extension.
//   o Consider: Auto-manage RMT channels? Currently user chooses a channel
//     - Alternative: Give the user the next available channel
//   o Raise exception if pulses is not a tuple
//     - Consider: Using iterables. Will make memory management much trickier though
//     - Consider: Allowing lists as well as tuples
//   o Do we need to allow the option to use multiple memory blocks?
//   o Consider supporting other ways to specify pulses:
//     - Fixed interval (specify pattern of 1's/0's)
//     - Specify both value and ticks
//     - See PyCom RMT docs
//   o Add carrier modulation methods
//   o Add rx support

// ESP-IDF RMT:
// Espressif examples: https://github.com/espressif/arduino-esp32/tree/master/libraries/ESP32/examples/RMT

STATIC mp_obj_t esp32_rmt_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // fixme: check arguments
    // 1 <= channel_id <= 8
    // 1 <= clock_divider <= 255
    mp_arg_check_num(n_args, n_kw, 3, MP_OBJ_FUN_ARGS_MAX, true);
    mp_uint_t channel_id = mp_obj_get_int(args[0]);
    gpio_num_t pin_id = machine_pin_get_id(args[1]);
    mp_uint_t clock_divider = mp_obj_get_int(args[2]);
    // fixme: should have an idle_level...

    esp32_rmt_obj_t *self = m_new_obj(esp32_rmt_obj_t);
    self->base.type = &esp32_rmt_type;
    self->channel_id = channel_id;
    self->pin = pin_id;
    self->clock_divider = clock_divider;

    rmt_config_t config;
    config.rmt_mode = RMT_MODE_TX;
    config.channel = (rmt_channel_t) self->channel_id;
    config.gpio_num = self->pin;
    config.mem_block_num = 1;
    config.tx_config.loop_en = 0;

    config.tx_config.carrier_en = 0;
    config.tx_config.idle_output_en = 0;
    config.tx_config.idle_level = 0;
    config.tx_config.carrier_duty_percent = 0;
    config.tx_config.carrier_freq_hz = 0;
    config.tx_config.carrier_level = 1;

    config.clk_div = self->clock_divider;

    // fixme: Error handling should be nicer; raise an exception
    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    return MP_OBJ_FROM_PTR(self);
}


STATIC void esp32_rmt_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "RMT(channel=%u, pin=%u, clock_divider=%u)", self->channel_id, self->pin, self->clock_divider);
}


STATIC mp_obj_t esp32_rmt_deinit(mp_obj_t self_in) {
    // fixme: check for valid channel. Return exception if error occurs.
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    rmt_driver_uninstall(self->channel_id);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_rmt_deinit_obj, esp32_rmt_deinit);

static float resolution_for_divider(uint8_t clock_divider)
{
    return 1.0 / (APB_CLK_FREQ / clock_divider);
}

// Return the resolution (shortest possible pulse), in seconds
STATIC mp_obj_t esp32_rmt_resolution(mp_obj_t self_in) {
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(resolution_for_divider(self->clock_divider));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_rmt_resolution_obj, esp32_rmt_resolution);


// Return the longest pulse duration
STATIC mp_obj_t esp32_rmt_max_duration(mp_obj_t self_in) {
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(resolution_for_divider(self->clock_divider) * 32768); // 2^15 is the maximum number of bits for ticks
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_rmt_max_duration_obj, esp32_rmt_max_duration);


STATIC mp_obj_t esp32_rmt_send_pulses(mp_obj_t self_in, mp_obj_t pulses, mp_obj_t start_level) {
    esp32_rmt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t level = mp_obj_get_int(start_level);
    // fixme: Check inputs
    // start_level => [0,1]

    mp_uint_t pulses_length = 0;
    mp_obj_t* pulses_ptr = NULL;

    // fixme: Handle if not a tuple
    if(MP_OBJ_IS_TYPE(pulses, &mp_type_tuple) == true) {
        mp_obj_tuple_get(pulses, &pulses_length, &pulses_ptr);
    } else if(MP_OBJ_IS_TYPE(pulses, &mp_type_list) == true) {
        mp_obj_list_get(pulses, &pulses_length, &pulses_ptr);
    } else {
        mp_raise_TypeError("Pulses must be defined as a tuple or list");
    }

    mp_uint_t num_items = (pulses_length / 2) + (pulses_length % 2);
    rmt_item32_t* items = (rmt_item32_t*)m_malloc(num_items * sizeof(rmt_item32_t));

    //printf("num_items=%d pulses_length=%d\n\n", num_items, pulses_length);

    for (mp_uint_t item_index = 0; item_index < num_items; item_index++)
    {
        mp_uint_t pulse_index = item_index * 2;
        items[item_index].duration0 = mp_obj_get_int(pulses_ptr[pulse_index++]);
        items[item_index].level0 = level++; // Note that level _could_ wrap.
        //printf("duration=%d level=%d\n", items[item_index].duration0, items[item_index].level0);
        if (pulse_index < pulses_length)
        {
            items[item_index].duration1 = mp_obj_get_int(pulses_ptr[pulse_index]);
            items[item_index].level1 = level++;
            //printf("duration=%d level=%d\n", items[item_index].duration1, items[item_index].level1);
        }
    }
    ESP_ERROR_CHECK(rmt_write_items(self->channel_id, items, num_items, true));

    m_free(items);

    // if (result != ESP_OK) {
    //     nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "rmt_write_items failed!"));
    // }

    return MP_OBJ_NEW_SMALL_INT(num_items);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp32_rmt_send_pulses_obj, esp32_rmt_send_pulses);


STATIC const mp_rom_map_elem_t esp32_rmt_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_send_pulses), MP_ROM_PTR(&esp32_rmt_send_pulses_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&esp32_rmt_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_resolution), MP_ROM_PTR(&esp32_rmt_resolution_obj) },
    { MP_ROM_QSTR(MP_QSTR_max_duration), MP_ROM_PTR(&esp32_rmt_max_duration_obj) },
};
STATIC MP_DEFINE_CONST_DICT(esp32_rmt_locals_dict, esp32_rmt_locals_dict_table);

const mp_obj_type_t esp32_rmt_type = {
    { &mp_type_type },
    .name = MP_QSTR_RMT,
    .print = esp32_rmt_print,
    .make_new = esp32_rmt_make_new,
    .locals_dict = (mp_obj_dict_t *)&esp32_rmt_locals_dict,
};
