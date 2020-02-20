#! /usr/bin/python3

# Linking table for platform-specific functions.
#
# The entry points below list a number of functions in the MP firmware that are to be exposed to
# native modules. This is typically used to expose ESP-IDF functions so they can be called directly
# from C code in dynamically loaded native modules. The native module can then #include the esp-idf
# header files as usual and call the functions normally.

# TODO: this needs to be loaded from a file, not hard-coded here
entry_points = [
    ("esp_chip_info", "P", ""),
    ("esp_get_idf_version", "", "s"),
    #("pcnt_unit_config",
    #("pcnt_get_counter_value",
    #("pcnt_counter_pause",
    #("pcnt_counter_resume",
    #("pcnt_counter_clear",
    #("pcnt_intr_disable",
    ]

imports = [
    "esp_err.h",
    ]

type_map = { "I": "uint", "i": "int", "P": "void*", "s": "char*", "": "void" }

import sys

if len(sys.argv) > 1 and sys.argv[1] == '--tab':
    # Generate platform relocation table compiled into the MP firmware
    print('\t.file\t"plat_relo.py"')
    print('\t.section\t.rodata.plat_relo_tab,"a",@progbits')
    print('\t.align\t4')
    print('\t.global\tplat_relo_tab')
    print('\t.type\tplat_relo_tab, @object')
    print('plat_relo_tab:')
    for ep in entry_points:
        print('\t.word\t' + ep)
    print('\t.size\tplat_relo_tab, .-plat_relo_tab')

elif len(sys.argv) > 1 and sys.argv[1] == '--module':
    # Generate espidf module to expose functions to python
    print('''\
#include <assert.h>
#include <string.h>

#include "py/runtime.h"

#if MICROPY_PY_PLAT_RELO

// ESP-IDF imports
''', end='')
    for im in imports:
        print("#include <{}>".format(im))
    for name, args, res in entry_points:
        print()
        print("STATIC mp_obj_t espidf_{}(".format(name), end='')
        print(", ".join(["mp_obj_t arg{}".format(ix) for ix in range(len(args))]), end='')
        print(") {")
        for ix, fmt in enumerate(args):
            print("\t// convert arg{}".format(ix))
            if fmt == "P":
                print("\tmp_buffer_info_t buf{};".format(ix))
                print("\tmp_get_buffer_raise(arg{}, &buf{}, MP_BUFFER_RW);".format(ix, ix))
        print("\t// call")
        if res == "":
            print("\t{}(".format(name), end='')
            print(", ".join([ "arg{}".format(ix) for ix in range(len(args)) ]), end='')
            print(");")
            print("\tmp_obj_t ret = mp_const_none;")
        else:
            print("\t{} ret = {}(".format(type_map[res], name))
        print("\treturn ret;\n}")
        if len(args) < 4:
            print("STATIC MP_DEFINE_CONST_FUN_OBJ_{}(espidf_{}_obj, espidf_{});".format(
                len(args), name, name))
        else:
            print("STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espidf_{}_obj, {}, {}, espidf_{});".format(
                len(args), len(args), name, name))
    print()
    print("STATIC const mp_rom_map_elem_t mp_module_espidf_globals_table[] = {")
    print("\t{ MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_espidf) },")
    for name, _, _ in entry_points:
        print("\t{{ MP_ROM_QSTR(MP_QSTR_{}), MP_ROM_PTR(&espidf_{}) }},".format(name, name))
    print('''\
};
STATIC MP_DEFINE_CONST_DICT(mp_module_espidf_globals, mp_module_espidf_globals_table);

const mp_obj_module_t mp_module_espidf = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_espidf_globals,
};

#endif
''', end='')


