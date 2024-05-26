// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#include "shared-module/atexit/__init__.h"

//| """Atexit Module
//|
//| This module defines functions to register and unregister cleanup functions.
//| Functions thus registered are automatically executed upon normal vm termination.
//|
//| These functions are run in the reverse order in which they were registered;
//| if you register ``A``, ``B``, and ``C``, they will be run in the order ``C``, ``B``, ``A``.
//|
//| |see_cpython_module| :mod:`cpython:atexit`.
//| """
//| ...
//|

//| def register(
//|     func: Callable[..., Any], *args: Optional[Any], **kwargs: Optional[Any]
//| ) -> Callable[..., Any]:
//|     """Register func as a function to be executed at termination.
//|
//|     Any optional arguments that are to be passed to func must be passed as arguments to `register()`.
//|     It is possible to register the same function and arguments more than once.
//|
//|     At normal program termination (for instance, if `sys.exit()` is called or the vm execution completes),
//|     all functions registered are called in last in, first out order.
//|
//|     If an exception is raised during execution of the exit handler,
//|     a traceback is printed (unless `SystemExit` is raised) and the execution stops.
//|
//|     This function returns func, which makes it possible to use it as a decorator.
//|
//|     """
//|     ...
//|
static mp_obj_t atexit_register(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    shared_module_atexit_register(pos_args[0], (n_args - 1), ((n_args > 1) ? &pos_args[1] : NULL), kw_args);
    return pos_args[0];
}
static MP_DEFINE_CONST_FUN_OBJ_KW(atexit_register_obj, 1, atexit_register);

//| def unregister(func: Callable[..., Any]) -> None:
//|     """Remove func from the list of functions to be run at termination.
//|
//|     `unregister()` silently does nothing if func was not previously registered. If func has been registered more than once,
//|     every occurrence of that function in the atexit call stack will be removed.
//|
//|     """
//|     ...
//|
static mp_obj_t atexit_unregister(const mp_obj_t self_in) {
    shared_module_atexit_unregister(&self_in);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(atexit_unregister_obj, atexit_unregister);

static const mp_rom_map_elem_t atexit_module_globals_table[] = {
    // module name
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_atexit) },
    // module functions
    { MP_ROM_QSTR(MP_QSTR_register), MP_ROM_PTR(&atexit_register_obj) },
    { MP_ROM_QSTR(MP_QSTR_unregister), MP_ROM_PTR(&atexit_unregister_obj) },
};
static MP_DEFINE_CONST_DICT(atexit_module_globals, atexit_module_globals_table);

const mp_obj_module_t atexit_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&atexit_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_atexit, atexit_module);
