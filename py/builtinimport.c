/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
 * Copyright (c) 2020 Jim Mussared
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

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "py/compile.h"
#include "py/objmodule.h"
#include "py/persistentcode.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/frozenmod.h"

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_PRINT (0)
#define DEBUG_printf(...) (void)0
#endif

bool mp_obj_is_package(mp_obj_t module) {
    mp_obj_t dest[2];
    mp_load_method_maybe(module, MP_QSTR___path__, dest);
    return dest[0] != MP_OBJ_NULL;
}

#if MICROPY_ENABLE_EXTERNAL_IMPORT

#define PATH_SEP_CHAR "/"

// Try and find exactly "foo/bar.py" (or "foo/bar.mpy").
// Stat either frozen or normal module by a given path
// (whatever is available, if at all).
STATIC mp_import_stat_t mp_import_stat_any(const char *path) {
    #if MICROPY_MODULE_FROZEN
    mp_import_stat_t st = mp_frozen_stat(path);
    if (st != MP_IMPORT_STAT_NO_EXIST) {
        return st;
    }
    #endif
    return mp_import_stat(path);
}

// Try and find "foo/bar.py", fall back to "foo/bar.mpy".
STATIC mp_import_stat_t stat_file_py_or_mpy(vstr_t *path) {
    mp_import_stat_t stat = mp_import_stat_any(vstr_null_terminated_str(path));
    if (stat == MP_IMPORT_STAT_FILE) {
        return stat;
    }

    #if MICROPY_PERSISTENT_CODE_LOAD
    // Insert an 'm' into the '.py'.
    vstr_ins_byte(path, path->len - 2, 'm');
    stat = mp_import_stat_any(vstr_null_terminated_str(path));
    if (stat == MP_IMPORT_STAT_FILE) {
        return stat;
    }
    #endif

    return MP_IMPORT_STAT_NO_EXIST;
}

// Try and find "foo/bar" (a directory) or "foo/bar.(m)py".
STATIC mp_import_stat_t stat_dir_or_file(vstr_t *path) {
    mp_import_stat_t stat = mp_import_stat_any(vstr_null_terminated_str(path));
    DEBUG_printf("stat %s: %d\n", vstr_str(path), stat);
    if (stat == MP_IMPORT_STAT_DIR) {
        return stat;
    }

    // not a directory, add .py and try as a file
    vstr_add_str(path, ".py");
    return stat_file_py_or_mpy(path);
}

// This forwards to stat_dir_or_file, but tries in each location in sys.path.
STATIC mp_import_stat_t stat_dir_or_file_in_sys_path(qstr mod_name, vstr_t *dest) {
    DEBUG_printf("stat_dir_or_file_in_sys_path: '%s'\n", qstr_str(mod_name));
    #if MICROPY_PY_SYS
    // extract the list of paths
    size_t path_num;
    mp_obj_t *path_items;
    mp_obj_list_get(mp_sys_path, &path_num, &path_items);

    if (path_num != 0) {
        // go through each path looking for a directory or file
        for (size_t i = 0; i < path_num; i++) {
            vstr_reset(dest);
            size_t p_len;
            const char *p = mp_obj_str_get_data(path_items[i], &p_len);
            if (p_len > 0) {
                vstr_add_strn(dest, p, p_len);
                vstr_add_char(dest, PATH_SEP_CHAR[0]);
            }
            vstr_add_str(dest, qstr_str(mod_name));
            mp_import_stat_t stat = stat_dir_or_file(dest);
            if (stat != MP_IMPORT_STAT_NO_EXIST) {
                return stat;
            }
        }

        // could not find a directory or file
        return MP_IMPORT_STAT_NO_EXIST;
    }
    #endif

    // mp_sys_path is empty, so just use the given file name
    vstr_add_str(dest, qstr_str(mod_name));
    return stat_dir_or_file(dest);
}

#if MICROPY_MODULE_FROZEN_STR || MICROPY_ENABLE_COMPILER
STATIC void do_load_from_lexer(mp_obj_t module_obj, mp_lexer_t *lex) {
    #if MICROPY_PY___FILE__
    qstr source_name = lex->source_name;
    mp_store_attr(module_obj, MP_QSTR___file__, MP_OBJ_NEW_QSTR(source_name));
    #endif

    // parse, compile and execute the module in its context
    mp_obj_dict_t *mod_globals = mp_obj_module_get_globals(module_obj);
    mp_parse_compile_execute(lex, MP_PARSE_FILE_INPUT, mod_globals, mod_globals);
}
#endif

#if MICROPY_PERSISTENT_CODE_LOAD || MICROPY_MODULE_FROZEN_MPY
STATIC void do_execute_raw_code(mp_obj_t module_obj, mp_raw_code_t *raw_code, const char *source_name) {
    (void)source_name;

    #if MICROPY_PY___FILE__
    mp_store_attr(module_obj, MP_QSTR___file__, MP_OBJ_NEW_QSTR(qstr_from_str(source_name)));
    #endif

    // execute the module in its context
    mp_obj_dict_t *mod_globals = mp_obj_module_get_globals(module_obj);

    // save context
    mp_obj_dict_t *volatile old_globals = mp_globals_get();
    mp_obj_dict_t *volatile old_locals = mp_locals_get();

    // set new context
    mp_globals_set(mod_globals);
    mp_locals_set(mod_globals);

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t module_fun = mp_make_function_from_raw_code(raw_code, MP_OBJ_NULL, MP_OBJ_NULL);
        mp_call_function_0(module_fun);

        // finish nlr block, restore context
        nlr_pop();
        mp_globals_set(old_globals);
        mp_locals_set(old_locals);
    } else {
        // exception; restore context and re-raise same exception
        mp_globals_set(old_globals);
        mp_locals_set(old_locals);
        nlr_jump(nlr.ret_val);
    }
}
#endif

STATIC void do_load(mp_obj_t module_obj, vstr_t *file) {
    #if MICROPY_MODULE_FROZEN || MICROPY_ENABLE_COMPILER || (MICROPY_PERSISTENT_CODE_LOAD && MICROPY_HAS_FILE_READER)
    char *file_str = vstr_null_terminated_str(file);
    #endif

    // If we support frozen modules (either as str or mpy) then try to find the
    // requested filename in the list of frozen module filenames.
    #if MICROPY_MODULE_FROZEN
    void *modref;
    int frozen_type = mp_find_frozen_module(file_str, file->len, &modref);
    #endif

    // If we support frozen str modules and the compiler is enabled, and we
    // found the filename in the list of frozen files, then load and execute it.
    #if MICROPY_MODULE_FROZEN_STR
    if (frozen_type == MP_FROZEN_STR) {
        do_load_from_lexer(module_obj, modref);
        return;
    }
    #endif

    // If we support frozen mpy modules and we found a corresponding file (and
    // its data) in the list of frozen files, execute it.
    #if MICROPY_MODULE_FROZEN_MPY
    if (frozen_type == MP_FROZEN_MPY) {
        do_execute_raw_code(module_obj, modref, file_str);
        return;
    }
    #endif

    // If we support loading .mpy files then check if the file extension is of
    // the correct format and, if so, load and execute the file.
    #if MICROPY_HAS_FILE_READER && MICROPY_PERSISTENT_CODE_LOAD
    if (file_str[file->len - 3] == 'm') {
        mp_raw_code_t *raw_code = mp_raw_code_load_file(file_str);
        do_execute_raw_code(module_obj, raw_code, file_str);
        return;
    }
    #endif

    // If we can compile scripts then load the file and compile and execute it.
    #if MICROPY_ENABLE_COMPILER
    {
        mp_lexer_t *lex = mp_lexer_new_from_file(file_str);
        do_load_from_lexer(module_obj, lex);
        return;
    }
    #else
    // If we get here then the file was not frozen and we can't compile scripts.
    mp_raise_msg(&mp_type_ImportError, "script compilation not supported");
    #endif
}

STATIC void evaluate_relative_import(mp_int_t level, size_t *mod_len, const char **mod_str) {
    // What we want to do here is to take name of current module,
    // chop <level> trailing components, and concatenate with the passed-in
    // module name, thus resolving relative import name into absolute.
    // This even appears to be correct per
    // http://legacy.python.org/dev/peps/pep-0328/#relative-imports-and-name
    // "Relative imports use a module's __name__ attribute to determine that
    // module's position in the package hierarchy.".

    // For example, level=3, mod_str="foo.bar", __name__="a.b.c.d" --> "a.foo.bar"

    mp_obj_t this_name_obj = mp_obj_dict_get(MP_OBJ_FROM_PTR(mp_globals_get()), MP_OBJ_NEW_QSTR(MP_QSTR___name__));
    assert(this_name_obj != MP_OBJ_NULL);
    #if MICROPY_ENABLE_MODULE_OVERRIDE_MAIN && MICROPY_CPYTHON_COMPAT
    if (MP_OBJ_QSTR_VALUE(this_name_obj) == MP_QSTR___main__) {
        // This is a module run by -m command-line switch, get its real name from backup attribute
        this_name_obj = mp_obj_dict_get(MP_OBJ_FROM_PTR(mp_globals_get()), MP_OBJ_NEW_QSTR(MP_QSTR___main__));
    }
    #endif
    mp_map_t *globals_map = &mp_globals_get()->map;
    mp_map_elem_t *elem = mp_map_lookup(globals_map, MP_OBJ_NEW_QSTR(MP_QSTR___path__), MP_MAP_LOOKUP);
    bool is_pkg = (elem != NULL);

    #if DEBUG_PRINT
    DEBUG_printf("Current module/package: ");
    mp_obj_print(this_name_obj, PRINT_REPR);
    DEBUG_printf(", is_package: %d", is_pkg);
    DEBUG_printf("\n");
    #endif

    size_t this_name_l;
    const char *this_name = mp_obj_str_get_data(this_name_obj, &this_name_l);

    const char *p = this_name + this_name_l;
    if (is_pkg) {
        // If it's relative to a package, then take off one fewer level.
        --level;
    }

    // Walk back 'level' dots.
    while (level && p > this_name) {
        if (*--p == '.') {
            --level;
        }
    }

    // We must have some component left over to import from
    if (p == this_name) {
        mp_raise_ValueError("cannot perform relative import");
    }

    // New length is len("<chopped path>.<mod_str>").
    // Might be one byte more than we need if mod_str is empty (for the extra .).
    uint new_mod_l = (size_t)(p - this_name) + 1 + *mod_len;
    char *new_mod = mp_local_alloc(new_mod_l);
    memcpy(new_mod, this_name, p - this_name);

    // Only append ".<mod_str>" if there was one).
    if (*mod_len != 0) {
        new_mod[p - this_name] = '.';
        memcpy(new_mod + (p - this_name) + 1, *mod_str, *mod_len);
    } else {
        --new_mod_l;
    }

    // Copy into a QSTR.
    qstr new_mod_q = qstr_from_strn(new_mod, new_mod_l);
    mp_local_free(new_mod);

    DEBUG_printf("Resolved base name for relative import: '%s'\n", qstr_str(new_mod_q));
    *mod_str = qstr_str(new_mod_q);
    *mod_len = new_mod_l;
}

STATIC mp_obj_t process_import_at_level(qstr full_mod_name, qstr level_mod_name, mp_obj_t outer_module_obj, vstr_t *path, bool override_main) {
    mp_import_stat_t stat = MP_IMPORT_STAT_NO_EXIST;
    bool store_on_parent = true;

    // Exact-match built-in (or already-loaded) takes priority.
    mp_obj_t module_obj = mp_module_get_loaded_or_builtin(full_mod_name);

    // Even if we find the module, go through the motions of searching for it
    // because we may actually be in the process of importing a sub-module.
    // So we need to (re-)find the correct path to be finding the sub-module
    // on the next iteration of process_level.

    // find the file corresponding to the module name
    if (outer_module_obj == MP_OBJ_NULL) {
        DEBUG_printf("Searching for top-level module\n");

        // First module in the dotted-name; search for a directory or file.
        // Because it's the top level, we try all sys.path entries.
        stat = stat_dir_or_file_in_sys_path(full_mod_name, path);

        // If the module doesn't exist on the filesystem, and it's not a builtin,
        // try the weak link instead.
        #if MICROPY_MODULE_WEAK_LINKS
        if (module_obj == MP_OBJ_NULL && stat == MP_IMPORT_STAT_NO_EXIST) {
            char *umodule_buf = vstr_str(path);
            umodule_buf[0] = 'u';
            strcpy(umodule_buf + 1, qstr_str(level_mod_name));
            qstr umodule_name = qstr_from_str(umodule_buf);
            module_obj = mp_module_get_loaded_or_builtin(umodule_name);
        }
        #endif
    } else {
        DEBUG_printf("Searching for sub-module\n");

        if (module_obj == MP_OBJ_NULL) {
            // See if the outer module has this module as an attr.
            // e.g. built-in modules can add modules to their globals dict
            // to provide something that behaves like built-in packages.
            mp_obj_t dest[2] = { MP_OBJ_NULL };
            mp_type_module.attr(outer_module_obj, level_mod_name, dest);
            if (dest[0] != MP_OBJ_NULL) {
                // Ensure that it's actually a module.
                if (mp_obj_get_type(dest[0]) == &mp_type_module) {
                    module_obj = dest[0];
                    store_on_parent = false;
                }
            }
        }

        // Add the current part of the module name to the path.
        vstr_add_char(path, PATH_SEP_CHAR[0]);
        vstr_add_str(path, qstr_str(level_mod_name));
        // Because it's not top level, we already know which path the parent was found in.
        stat = stat_dir_or_file(path);
    }
    DEBUG_printf("Current path: %.*s\n", (int)vstr_len(path), vstr_str(path));

    if (module_obj == MP_OBJ_NULL) {
        // Not a built-in and not already-loaded.

        if (stat == MP_IMPORT_STAT_NO_EXIST) {
            // And the file wasn't found -- fail.
            if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                mp_raise_msg(&mp_type_ImportError, "module not found");
            } else {
                mp_raise_msg_varg(&mp_type_ImportError, "no module named '%q'", full_mod_name);
            }
        }

        // Not a built-in but found on the filesystem, try and load it.

        DEBUG_printf("Found path: %.*s\n", (int)vstr_len(path), vstr_str(path));

        // Prepare for loading from the filesystem. Create a new shell module.
        module_obj = mp_obj_new_module(full_mod_name);

        #if MICROPY_ENABLE_MODULE_OVERRIDE_MAIN
        // if args[3] (fromtuple) has magic value False, set up
        // this module for command-line "-m" option (set module's
        // name to __main__ instead of real name). Do this only
        // for *modules* however - packages never have their names
        // replaced, instead they're -m'ed using a special __main__
        // submodule in them. (This all apparently is done to not
        // touch package name itself, which is important for future
        // imports).
        if (override_main && stat != MP_IMPORT_STAT_DIR) {
            mp_obj_module_t *o = MP_OBJ_TO_PTR(module_obj);
            mp_obj_dict_store(MP_OBJ_FROM_PTR(o->globals), MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR___main__));
            #if MICROPY_CPYTHON_COMPAT
            // Store module as "__main__" in the dictionary of loaded modules (returned by sys.modules).
            mp_obj_dict_store(MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_loaded_modules_dict)), MP_OBJ_NEW_QSTR(MP_QSTR___main__), module_obj);
            // Store real name in "__main__" attribute. Chosen semi-randonly, to reuse existing qstr's.
            mp_obj_dict_store(MP_OBJ_FROM_PTR(o->globals), MP_OBJ_NEW_QSTR(MP_QSTR___main__), MP_OBJ_NEW_QSTR(full_mod_name));
            #endif
        }
        #endif // MICROPY_ENABLE_MODULE_OVERRIDE_MAIN

        if (stat == MP_IMPORT_STAT_DIR) {
            // Directory -- execute "path/__init__.py".
            DEBUG_printf("%.*s is dir\n", (int)vstr_len(path), vstr_str(path));
            // https://docs.python.org/3/reference/import.html
            // "Specifically, any module that contains a __path__ attribute is considered a package."
            mp_store_attr(module_obj, MP_QSTR___path__, mp_obj_new_str(vstr_str(path), vstr_len(path)));
            size_t orig_path_len = path->len;
            vstr_add_str(path, PATH_SEP_CHAR "__init__.py");
            if (stat_file_py_or_mpy(path) != MP_IMPORT_STAT_FILE) {
                //mp_warning("%s is imported as namespace package", vstr_str(&path));
            } else {
                do_load(module_obj, path);
            }
            path->len = orig_path_len;
        } else { // MP_IMPORT_STAT_FILE
            // File -- execute "path.(m)py".
            do_load(module_obj, path);
            // This should be the last component in the import path.  If there are
            // remaining components then it's an ImportError because the current path
            // (the module that was just loaded) is not a package.  This will be caught
            // on the next iteration because the file will not exist.
        }
    }

    if (store_on_parent && outer_module_obj != MP_OBJ_NULL) {
        // If it's a sub-module (not a built-in one), then make it available on
        // the parent module.
        mp_store_attr(outer_module_obj, level_mod_name, module_obj);
    }

    return module_obj;
}

mp_obj_t mp_builtin___import__(size_t n_args, const mp_obj_t *args) {
    #if DEBUG_PRINT
    DEBUG_printf("__import__:\n");
    for (size_t i = 0; i < n_args; i++) {
        DEBUG_printf("  ");
        mp_obj_print(args[i], PRINT_REPR);
        DEBUG_printf("\n");
    }
    #endif

    // This is the import path, with any leading dots stripped.
    // "import foo.bar" --> module_name="foo.bar"
    // "from foo.bar import baz" --> module_name="foo.bar"
    // "from . import foo" --> module_name=""
    // "from ...foo.bar import baz" --> module_name="foo.bar"
    mp_obj_t module_name = args[0];

    // These are the imported names.
    // i.e. "from foo.bar import baz, zap" --> fromtuple=("baz", "zap",)
    mp_obj_t fromtuple = mp_const_none;

    // Level is the number of leading dots in a relative import.
    // i.e. "from . import foo" --> level=1
    // i.e. "from ...foo.bar import baz" --> level=3
    mp_int_t level = 0;

    if (n_args >= 4) {
        fromtuple = args[3];
        if (n_args >= 5) {
            level = MP_OBJ_SMALL_INT_VALUE(args[4]);
            if (level < 0) {
                mp_raise_ValueError(NULL);
            }
        }
    }

    size_t mod_len;
    const char *mod_str = mp_obj_str_get_data(module_name, &mod_len);

    if (level != 0) {
        // Turn "foo.bar" into "<current module minus 3 components>.foo.bar".
        evaluate_relative_import(level, &mod_len, &mod_str);
    }

    if (mod_len == 0) {
        mp_raise_ValueError(NULL);
    }

    DEBUG_printf("Starting module search for '%s'\n", mod_str);

    uint last = 0;
    VSTR_FIXED(path, MICROPY_ALLOC_PATH_MAX)
    mp_obj_t top_module_obj = MP_OBJ_NULL;
    mp_obj_t outer_module_obj = MP_OBJ_NULL;
    for (uint i = 1; i <= mod_len; i++) {
        if (i == mod_len || mod_str[i] == '.') {
            // The module name up to this depth (e.g. foo.bar.baz).
            qstr full_mod_name = qstr_from_strn(mod_str, i);
            // The current level name (e.g. baz).
            qstr level_mod_name = qstr_from_strn(mod_str + last, i - last);

            DEBUG_printf("Processing module: '%s' at level '%s'\n", qstr_str(full_mod_name), qstr_str(level_mod_name));
            DEBUG_printf("Previous path: =%.*s=\n", (int)vstr_len(&path), vstr_str(&path));

            mp_obj_t module_obj = process_import_at_level(full_mod_name, level_mod_name, outer_module_obj, &path, i == mod_len && fromtuple == mp_const_false);

            outer_module_obj = module_obj;
            if (top_module_obj == MP_OBJ_NULL) {
                top_module_obj = module_obj;
            }

            last = i + 1;
        }
    }

    // If fromlist is not empty, return leaf module
    if (fromtuple != mp_const_none) {
        return outer_module_obj;
    }
    // Otherwise, we need to return top-level package
    return top_module_obj;
}

#else // MICROPY_ENABLE_EXTERNAL_IMPORT

mp_obj_t mp_builtin___import__(size_t n_args, const mp_obj_t *args) {
    // Check that it's not a relative import
    if (n_args >= 5 && MP_OBJ_SMALL_INT_VALUE(args[4]) != 0) {
        mp_raise_NotImplementedError("relative import");
    }

    // Check if module already exists, and return it if it does
    qstr module_name_qstr = mp_obj_str_get_qstr(args[0]);
    mp_obj_t module_obj = mp_module_get_loaded_or_builtin(module_name_qstr);
    if (module_obj != MP_OBJ_NULL) {
        return module_obj;
    }

    #if MICROPY_MODULE_WEAK_LINKS
    // Check if there is a weak link to this module
    char umodule_buf[MICROPY_ALLOC_PATH_MAX];
    umodule_buf[0] = 'u';
    strcpy(umodule_buf + 1, args[0]);
    qstr umodule_name_qstr = qstr_from_str(umodule_buf);
    module_obj = mp_module_get_loaded_or_builtin(umodule_name_qstr);
    if (module_obj != MP_OBJ_NULL) {
        return module_obj;
    }
    #endif

    // Couldn't find the module, so fail
    if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
        mp_raise_msg(&mp_type_ImportError, "module not found");
    } else {
        mp_raise_msg_varg(&mp_type_ImportError, "no module named '%q'", module_name_qstr);
    }
}

#endif // MICROPY_ENABLE_EXTERNAL_IMPORT

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin___import___obj, 1, 5, mp_builtin___import__);
