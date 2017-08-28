/**
 * @file module_sync.c
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
    return (mp_const_none);
}

static MP_DEFINE_CONST_FUN_OBJ_0(module_init_obj, module_init);

/**
 * A table of all the modules' global objects.
 */
static const mp_rom_map_elem_t module_sync_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sync) },
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&module_init_obj) },

    /* Module classes. */
#if CONFIG_PUMBAA_CLASS_EVENT == 1
    { MP_ROM_QSTR(MP_QSTR_Event), MP_ROM_PTR(&module_sync_class_event) },
#endif
#if CONFIG_PUMBAA_CLASS_QUEUE == 1
    { MP_ROM_QSTR(MP_QSTR_Queue), MP_ROM_PTR(&module_sync_class_queue) },
#endif
};

static MP_DEFINE_CONST_DICT(module_sync_globals, module_sync_globals_table);

const mp_obj_module_t module_sync = {
    { &mp_type_module },
    .globals = (mp_obj_t)&module_sync_globals,
};
