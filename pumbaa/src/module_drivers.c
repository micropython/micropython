/**
 * @file module_drivers.c
 *
 * @section License
 * Copyright (C) 2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Pumbaa project.
 */

#include "pumbaa.h"

/**
 * Function called when this module is imported.
 */
static mp_obj_t module_init(void)
{
#if CONFIG_PUMBAA_CLASS_PIN == 1
    pin_module_init();
#endif
#if CONFIG_PUMBAA_CLASS_EXTI == 1
    exti_module_init();
#endif

    return (mp_const_none);
}

static MP_DEFINE_CONST_FUN_OBJ_0(module_init_obj, module_init);

/**
 * A table of all the modules' global objects.
 */
static const mp_rom_map_elem_t module_drivers_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_drivers) },
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&module_init_obj) },

    /* Module classes. */
#if CONFIG_PUMBAA_CLASS_DAC == 1
    { MP_ROM_QSTR(MP_QSTR_Dac), MP_ROM_PTR(&module_drivers_class_dac) },
#endif
#if CONFIG_PUMBAA_CLASS_EXTI == 1
    { MP_ROM_QSTR(MP_QSTR_Exti), MP_ROM_PTR(&module_drivers_class_exti) },
#endif
#if CONFIG_PUMBAA_CLASS_PIN == 1
    { MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&module_drivers_class_pin) },
#endif
#if CONFIG_PUMBAA_CLASS_SD == 1
    { MP_ROM_QSTR(MP_QSTR_Sd), MP_ROM_PTR(&module_drivers_class_sd) },
#endif
#if CONFIG_PUMBAA_CLASS_SPI == 1
    { MP_ROM_QSTR(MP_QSTR_Spi), MP_ROM_PTR(&module_drivers_class_spi) },
#endif
#if CONFIG_PUMBAA_CLASS_I2C_SOFT == 1
    { MP_ROM_QSTR(MP_QSTR_I2CSoft), MP_ROM_PTR(&module_drivers_class_i2c_soft) },
#endif
};

static MP_DEFINE_CONST_DICT(module_drivers_globals, module_drivers_globals_table);

const mp_obj_module_t module_drivers = {
    { &mp_type_module },
    .globals = (mp_obj_t)&module_drivers_globals,
};
