/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
#include "drivers/dht/dht.h"
#include "modrp2.h"

#if MICROPY_PY_NETWORK_CYW43
#include "lib/cyw43-driver/src/cyw43_country.h"

extern uint32_t cyw43_country_code;

STATIC mp_obj_t rp2_country(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        char code[2] = {cyw43_country_code, cyw43_country_code >> 8};
        return mp_obj_new_str(code, 2);
    } else {
        size_t len;
        const char *str = mp_obj_str_get_data(args[0], &len);
        if (len != 2) {
            mp_raise_ValueError(NULL);
        }
        cyw43_country_code = CYW43_COUNTRY(str[0], str[1], 0);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_country_obj, 0, 1, rp2_country);
#endif

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/sio.h"

// Picoboard has a button attached to the flash CS pin, which the bootrom
// checks, and jumps straight to the USB bootcode if the button is pressed
// (pulling flash CS low). We can check this pin in by jumping to some code in
// SRAM (so that the XIP interface is not required), floating the flash CS
// pin, and observing whether it is pulled low.
//
// This doesn't work if others are trying to access flash at the same time,
// e.g. XIP streamer, or the other core.
// https://github.com/raspberrypi/pico-examples/blob/master/picoboard/button/button.c

bool __no_inline_not_in_flash_func(get_bootsel_button)() {
    const uint CS_PIN_INDEX = 1;

    // Must disable interrupts, as interrupt handlers may be in flash, and we
    // are about to temporarily disable flash access!
    uint32_t flags = save_and_disable_interrupts();

    // Set chip select to Hi-Z
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
        GPIO_OVERRIDE_LOW << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
            IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    // Note we can't call into any sleep functions in flash right now
    for (volatile int i = 0; i < 1000; ++i) {;
    }

    // The HI GPIO registers in SIO can observe and control the 6 QSPI pins.
    // Note the button pulls the pin *low* when pressed.
    bool button_state = !(sio_hw->gpio_hi_in & (1u << CS_PIN_INDEX));

    // Need to restore the state of chip select, else we are going to have a
    // bad time when we return to code in flash!
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
        GPIO_OVERRIDE_NORMAL << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
            IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    restore_interrupts(flags);

    return button_state;
}

STATIC mp_obj_t rp2_bootsel_button(void) {
    return MP_OBJ_NEW_SMALL_INT(get_bootsel_button());
}
MP_DEFINE_CONST_FUN_OBJ_0(rp2_bootsel_button_obj, rp2_bootsel_button);


STATIC const mp_rom_map_elem_t rp2_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_rp2) },
    { MP_ROM_QSTR(MP_QSTR_Flash),               MP_ROM_PTR(&rp2_flash_type) },
    { MP_ROM_QSTR(MP_QSTR_PIO),                 MP_ROM_PTR(&rp2_pio_type) },
    { MP_ROM_QSTR(MP_QSTR_StateMachine),        MP_ROM_PTR(&rp2_state_machine_type) },

    { MP_ROM_QSTR(MP_QSTR_dht_readinto),        MP_ROM_PTR(&dht_readinto_obj) },

    #if MICROPY_PY_NETWORK_CYW43
    { MP_ROM_QSTR(MP_QSTR_country), MP_ROM_PTR(&rp2_country_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_bootsel_button),   MP_ROM_PTR(&rp2_bootsel_button_obj) },
};
STATIC MP_DEFINE_CONST_DICT(rp2_module_globals, rp2_module_globals_table);

const mp_obj_module_t mp_module_rp2 = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rp2_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR__rp2, mp_module_rp2);
