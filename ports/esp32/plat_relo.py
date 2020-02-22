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
    ("esp_err_to_name", "I", "s"),
    ("pcnt_unit_config", "P", "I"),
    ("pcnt_get_counter_value", "IP", "I"),
    ("pcnt_counter_pause", "I", "I"),
    ("pcnt_counter_resume", "I", "I"),
    ("pcnt_counter_clear", "I", "I"),
    ("pcnt_intr_disable", "I", "I"),
    ]

imports = [
    "esp_err.h",
    "esp_idf_version.h",
    "esp_system.h",
    "driver/pcnt.h",
    ]

type_map = { "I": "uint", "i": "int", "H": "uint16", "h": "int16", "B": "uint8", "b": "int8",
        "P": "void*", "s": "char*", "": "void" }

return_map = { "I": "mp_obj_new_int({})", "s": "mp_obj_new_str({0}, strlen({0}))", "": "{}" }

import sys

if len(sys.argv) > 1 and sys.argv[1] == '--tab':
    # Generate platform relocation table compiled into the MP firmware
    print('\t.file\t"plat_relo.py"')
    print('\t.section\t.rodata.plat_relo_tab,"a",@progbits')
    print('\t.align\t4')
    print('\t.global\tplat_relo_tab')
    print('\t.type\tplat_relo_tab, @object')
    print('plat_relo_tab:')
    for name, _, _ in entry_points:
        print('\t.word\t' + name)
    print('\t.size\tplat_relo_tab, .-plat_relo_tab')

elif len(sys.argv) > 1 and (sys.argv[1] == '--module' or sys.argv[1] == '--builtin'):
    # Generate espidf module to expose functions to python
    builtin = sys.argv[1] == '--builtin'
    print('#include <assert.h>')
    print('#include <string.h>')
    if builtin:
        print('#include "py/runtime.h"')
    else:
        print('#include "py/dynruntime.h"')

    print('// ESP-IDF imports')
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
                print("\tmp_buffer_info_t val{}_buf;".format(ix))
                print("\tmp_get_buffer_raise(arg{0}, &val{0}_buf, MP_BUFFER_RW);".format(ix))
                print("\tvoid *val{0} = (void *)(val{0}_buf.buf);".format(ix))

            elif fmt in frozenset(["I", "i", "H", "h", "B", "b"]):
                print("\t{1} val{0} = ({1})mp_obj_get_int(arg{0});".format(ix, type_map[fmt]))
        print("\t// call")
        if res == "":
            print("\tmp_obj_t ret = mp_const_none;")
            print("\t{}(".format(name), end='')
        else:
            print("\tconst {} ret = {}(".format(type_map[res], name), end='')
        print(", ".join([ "val{}".format(ix) for ix in range(len(args)) ]), end='')
        print(");")
        print("\treturn " + return_map[res].format('ret') + ";\n}")
        if len(args) < 4:
            print("STATIC MP_DEFINE_CONST_FUN_OBJ_{}(espidf_{}_obj, espidf_{});".format(
                len(args), name, name))
        else:
            print("STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espidf_{}_obj, {}, {}, espidf_{});".format(
                len(args), len(args), name, name))
    print()
    if sys.argv[1] == '--builtin':
        # generate module built into the firmware
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
    else:
        # generate dynamically loadable module
        print("mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {")
        print("\tMP_DYNRUNTIME_INIT_ENTRY")
        for name, _, _ in entry_points:
            print("\tmp_store_global(MP_QSTR_{0}, MP_OBJ_FROM_PTR(&espidf_{0}_obj));".format(name))
        print("\tMP_DYNRUNTIME_INIT_EXIT")
        print("}")
