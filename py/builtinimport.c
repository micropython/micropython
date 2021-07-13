/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013-2019 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
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
#include "py/gc_long_lived.h"
#include "py/gc.h"
#include "py/objmodule.h"
#include "py/persistentcode.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/frozenmod.h"

#include "supervisor/shared/translate.h"

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_PRINT (0)
#define DEBUG_printf(...) (void)0
#endif

#if MICROPY_ENABLE_EXTERNAL_IMPORT

#define PATH_SEP_CHAR '/'

bool mp_obj_is_package(mp_obj_t module) {
    mp_obj_t dest[2];
    mp_load_method_maybe(module, MP_QSTR___path__, dest);
    return dest[0] != MP_OBJ_NULL;
}

// Stat either frozen or normal module by a given path
// (whatever is available, if at all).
STATIC mp_import_stat_t mp_import_stat_any(const char *path) {
    #if MICROPY_MODULE_FROZEN
    if (strncmp(MP_FROZEN_FAKE_DIR_SLASH,
        path,
        MP_FROZEN_FAKE_DIR_SLASH_LENGTH) == 0) {
        mp_import_stat_t st = mp_frozen_stat(path + MP_FROZEN_FAKE_DIR_SLASH_LENGTH);
        if (st != MP_IMPORT_STAT_NO_EXIST) {
            return st;
        }
    }
    #endif
    return mp_import_stat(path);
}

STATIC mp_import_stat_t stat_file_py_or_mpy(vstr_t *path) {
    mp_import_stat_t stat = mp_import_stat_any(vstr_null_terminated_str(path));
    if (stat == MP_IMPORT_STAT_FILE) {
        return stat;
    }

    #if MICROPY_PERSISTENT_CODE_LOAD
    vstr_ins_byte(path, path->len - 2, 'm');
    stat = mp_import_stat_any(vstr_null_terminated_str(path));
    if (stat == MP_IMPORT_STAT_FILE) {
        return stat;
    }
    #endif

    return MP_IMPORT_STAT_NO_EXIST;
}

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

STATIC mp_import_stat_t find_file(const char *file_str, uint file_len, vstr_t *dest) {
    #if MICROPY_PY_SYS
    // extract the list of paths
    size_t path_num;
    mp_obj_t *path_items;
    mp_obj_list_get(mp_sys_path, &path_num, &path_items);

    if (path_num == 0) {
    #endif
    // mp_sys_path is empty, so just use the given file name
    vstr_add_strn(dest, file_str, file_len);
    return stat_dir_or_file(dest);
    #if MICROPY_PY_SYS
} else {
    // go through each path looking for a directory or file
    for (size_t i = 0; i < path_num; i++) {
        vstr_reset(dest);
        size_t p_len;
        const char *p = mp_obj_str_get_data(path_items[i], &p_len);
        if (p_len > 0) {
            vstr_add_strn(dest, p, p_len);
            vstr_add_char(dest, PATH_SEP_CHAR);
        }
        vstr_add_strn(dest, file_str, file_len);
        mp_import_stat_t stat = stat_dir_or_file(dest);
        if (stat != MP_IMPORT_STAT_NO_EXIST) {
            return stat;
        }
    }

    // could not find a directory or file
    return MP_IMPORT_STAT_NO_EXIST;
}
    #endif
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
    mp_obj_module_set_globals(module_obj, make_dict_long_lived(mod_globals, 10));
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
        mp_obj_module_set_globals(module_obj,
            make_dict_long_lived(mp_obj_module_get_globals(module_obj), 10));
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

    #if MICROPY_MODULE_FROZEN || MICROPY_MODULE_FROZEN_MPY
    if (strncmp(MP_FROZEN_FAKE_DIR_SLASH,
        file_str,
        MP_FROZEN_FAKE_DIR_SLASH_LENGTH) == 0) {
        // If we support frozen modules (either as str or mpy) then try to find the
        // requested filename in the list of frozen module filenames.
        #if MICROPY_MODULE_FROZEN
        void *modref;
        int frozen_type = mp_find_frozen_module(file_str + MP_FROZEN_FAKE_DIR_SLASH_LENGTH, file->len - MP_FROZEN_FAKE_DIR_SLASH_LENGTH, &modref);
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

    }
    #endif // MICROPY_MODULE_FROZEN || MICROPY_MODULE_FROZEN_MPY

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
    mp_raise_ImportError(MP_ERROR_TEXT("script compilation not supported"));
    #endif
}

STATIC void chop_component(const char *start, const char **end) {
    const char *p = *end;
    while (p > start) {
        if (*--p == '.') {
            *end = p;
            return;
        }
    }
    *end = p;
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

    mp_obj_t module_name = args[0];
    mp_obj_t fromtuple = mp_const_none;
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
        // What we want to do here is to take name of current module,
        // chop <level> trailing components, and concatenate with passed-in
        // module name, thus resolving relative import name into absolute.
        // This even appears to be correct per
        // http://legacy.python.org/dev/peps/pep-0328/#relative-imports-and-name
        // "Relative imports use a module's __name__ attribute to determine that
        // module's position in the package hierarchy."
        level--;
        mp_obj_t this_name_q = mp_obj_dict_get(MP_OBJ_FROM_PTR(mp_globals_get()), MP_OBJ_NEW_QSTR(MP_QSTR___name__));
        assert(this_name_q != MP_OBJ_NULL);
        #if MICROPY_CPYTHON_COMPAT
        if (MP_OBJ_QSTR_VALUE(this_name_q) == MP_QSTR___main__) {
            // This is a module run by -m command-line switch, get its real name from backup attribute
            this_name_q = mp_obj_dict_get(MP_OBJ_FROM_PTR(mp_globals_get()), MP_OBJ_NEW_QSTR(MP_QSTR___main__));
        }
        #endif
        mp_map_t *globals_map = &mp_globals_get()->map;
        mp_map_elem_t *elem = mp_map_lookup(globals_map, MP_OBJ_NEW_QSTR(MP_QSTR___path__), MP_MAP_LOOKUP);
        bool is_pkg = (elem != NULL);

        #if DEBUG_PRINT
        DEBUG_printf("Current module/package: ");
        mp_obj_print(this_name_q, PRINT_REPR);
        DEBUG_printf(", is_package: %d", is_pkg);
        DEBUG_printf("\n");
        #endif

        size_t this_name_l;
        const char *this_name = mp_obj_str_get_data(this_name_q, &this_name_l);

        const char *p = this_name + this_name_l;
        if (!is_pkg) {
            // We have module, but relative imports are anchored at package, so
            // go there.
            chop_component(this_name, &p);
        }

        while (level--) {
            chop_component(this_name, &p);
        }

        // We must have some component left over to import from
        if (p == this_name) {
            mp_raise_ImportError(MP_ERROR_TEXT("cannot perform relative import"));
        }

        uint new_mod_l = (mod_len == 0 ? (size_t)(p - this_name) : (size_t)(p - this_name) + 1 + mod_len);
        char *new_mod = mp_local_alloc(new_mod_l);
        memcpy(new_mod, this_name, p - this_name);
        if (mod_len != 0) {
            new_mod[p - this_name] = '.';
            memcpy(new_mod + (p - this_name) + 1, mod_str, mod_len);
        }

        qstr new_mod_q = qstr_from_strn(new_mod, new_mod_l);
        mp_local_free(new_mod);
        DEBUG_printf("Resolved base name for relative import: '%s'\n", qstr_str(new_mod_q));
        module_name = MP_OBJ_NEW_QSTR(new_mod_q);
        mod_str = qstr_str(new_mod_q);
        mod_len = new_mod_l;
    }

    if (mod_len == 0) {
        mp_raise_ValueError(NULL);
    }

    // check if module already exists
    qstr module_name_qstr = mp_obj_str_get_qstr(module_name);
    mp_obj_t module_obj = mp_module_get(module_name_qstr);
    if (module_obj != MP_OBJ_NULL) {
        DEBUG_printf("Module already loaded\n");
        // If it's not a package, return module right away
        char *p = strchr(mod_str, '.');
        if (p == NULL) {
            return module_obj;
        }
        // If fromlist is not empty, return leaf module
        if (fromtuple != mp_const_none) {
            return module_obj;
        }
        // Otherwise, we need to return top-level package
        qstr pkg_name = qstr_from_strn(mod_str, p - mod_str);
        return mp_module_get(pkg_name);
    }
    DEBUG_printf("Module not yet loaded\n");

    uint last = 0;
    VSTR_FIXED(path, MICROPY_ALLOC_PATH_MAX)
    module_obj = MP_OBJ_NULL;
    mp_obj_t top_module_obj = MP_OBJ_NULL;
    mp_obj_t outer_module_obj = MP_OBJ_NULL;
    uint i;
    for (i = 1; i <= mod_len; i++) {
        if (i == mod_len || mod_str[i] == '.') {
            // create a qstr for the module name up to this depth
            qstr mod_name = qstr_from_strn(mod_str, i);
            DEBUG_printf("Processing module: %s\n", qstr_str(mod_name));
            DEBUG_printf("Previous path: =%.*s=\n", vstr_len(&path), vstr_str(&path));

            // find the file corresponding to the module name
            mp_import_stat_t stat;
            if (vstr_len(&path) == 0) {
                // first module in the dotted-name; search for a directory or file
                DEBUG_printf("Find file =%.*s=\n", vstr_len(&path), vstr_str(&path));
                stat = find_file(mod_str, i, &path);
            } else {
                // latter module in the dotted-name; append to path
                vstr_add_char(&path, PATH_SEP_CHAR);
                vstr_add_strn(&path, mod_str + last, i - last);
                stat = stat_dir_or_file(&path);
            }
            DEBUG_printf("Current path: %.*s\n", vstr_len(&path), vstr_str(&path));

            if (stat == MP_IMPORT_STAT_NO_EXIST) {
                // This is just the module name after the previous .
                qstr current_module_name = qstr_from_strn(mod_str + last, i - last);
                mp_map_elem_t *el = NULL;
                if (outer_module_obj == MP_OBJ_NULL) {
                    el = mp_map_lookup((mp_map_t *)&mp_builtin_module_map,
                        MP_OBJ_NEW_QSTR(current_module_name),
                        MP_MAP_LOOKUP);
                } else {
                    el = mp_map_lookup(&((mp_obj_module_t *)outer_module_obj)->globals->map,
                        MP_OBJ_NEW_QSTR(current_module_name),
                        MP_MAP_LOOKUP);
                }

                if (el != NULL && mp_obj_is_type(el->value, &mp_type_module)) {
                    module_obj = el->value;
                    mp_module_call_init(mod_name, module_obj);
                } else {
                    // couldn't find the file, so fail
                    #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
                    mp_raise_ImportError(MP_ERROR_TEXT("module not found"));
                    #else
                    mp_raise_msg_varg(&mp_type_ImportError,
                        MP_ERROR_TEXT("no module named '%q'"), mod_name);
                    #endif
                }
            } else {
                // found the file, so get the module
                module_obj = mp_module_get(mod_name);
            }

            if (module_obj == MP_OBJ_NULL) {
                // module not already loaded, so load it!

                module_obj = mp_obj_new_module(mod_name);

                // if args[3] (fromtuple) has magic value False, set up
                // this module for command-line "-m" option (set module's
                // name to __main__ instead of real name). Do this only
                // for *modules* however - packages never have their names
                // replaced, instead they're -m'ed using a special __main__
                // submodule in them. (This all apparently is done to not
                // touch package name itself, which is important for future
                // imports).
                if (i == mod_len && fromtuple == mp_const_false && stat != MP_IMPORT_STAT_DIR) {
                    mp_obj_module_t *o = MP_OBJ_TO_PTR(module_obj);
                    mp_obj_dict_store(MP_OBJ_FROM_PTR(o->globals), MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR___main__));
                    #if MICROPY_CPYTHON_COMPAT
                    // Store module as "__main__" in the dictionary of loaded modules (returned by sys.modules).
                    mp_obj_dict_store(MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_loaded_modules_dict)), MP_OBJ_NEW_QSTR(MP_QSTR___main__), module_obj);
                    // Store real name in "__main__" attribute. Chosen semi-randonly, to reuse existing qstr's.
                    mp_obj_dict_store(MP_OBJ_FROM_PTR(o->globals), MP_OBJ_NEW_QSTR(MP_QSTR___main__), MP_OBJ_NEW_QSTR(mod_name));
                    #endif
                }

                if (stat == MP_IMPORT_STAT_DIR) {
                    DEBUG_printf("%.*s is dir\n", vstr_len(&path), vstr_str(&path));
                    // https://docs.python.org/3/reference/import.html
                    // "Specifically, any module that contains a __path__ attribute is considered a package."
                    mp_store_attr(module_obj, MP_QSTR___path__, mp_obj_new_str(vstr_str(&path), vstr_len(&path)));
                    size_t orig_path_len = path.len;
                    vstr_add_char(&path, PATH_SEP_CHAR);
                    vstr_add_str(&path, "__init__.py");
                    if (stat_file_py_or_mpy(&path) != MP_IMPORT_STAT_FILE) {
                        // mp_warning("%s is imported as namespace package", vstr_str(&path));
                    } else {
                        do_load(module_obj, &path);
                    }
                    path.len = orig_path_len;
                } else { // MP_IMPORT_STAT_FILE
                    do_load(module_obj, &path);
                    // This should be the last component in the import path.  If there are
                    // remaining components then it's an ImportError because the current path
                    // (the module that was just loaded) is not a package.  This will be caught
                    // on the next iteration because the file will not exist.
                }

                // Loading a module thrashes the heap significantly so we explicitly clean up
                // afterwards.
                gc_collect();
            }
            if (outer_module_obj != MP_OBJ_NULL && VERIFY_PTR(outer_module_obj)) {
                qstr s = qstr_from_strn(mod_str + last, i - last);
                mp_store_attr(outer_module_obj, s, module_obj);
                // The above store can cause a dictionary rehash and new allocation. So,
                // lets make sure the globals dictionary is still long lived.
                mp_obj_module_set_globals(outer_module_obj,
                    make_dict_long_lived(mp_obj_module_get_globals(outer_module_obj), 10));
            }
            outer_module_obj = module_obj;
            if (top_module_obj == MP_OBJ_NULL) {
                top_module_obj = module_obj;
            }
            last = i + 1;
        }
    }

    // If fromlist is not empty, return leaf module
    if (fromtuple != mp_const_none) {
        return module_obj;
    }
    // Otherwise, we need to return top-level package
    return top_module_obj;
}

#else // MICROPY_ENABLE_EXTERNAL_IMPORT

mp_obj_t mp_builtin___import__(size_t n_args, const mp_obj_t *args) {
    // Check that it's not a relative import
    if (n_args >= 5 && MP_OBJ_SMALL_INT_VALUE(args[4]) != 0) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("relative import"));
    }

    // Check if module already exists, and return it if it does
    qstr module_name_qstr = mp_obj_str_get_qstr(args[0]);
    mp_obj_t module_obj = mp_module_get(module_name_qstr);
    if (module_obj != MP_OBJ_NULL) {
        return module_obj;
    }

    #if MICROPY_MODULE_WEAK_LINKS
    // Check if there is a weak link to this module
    module_obj = mp_module_search_umodule(qstr_str(module_name_qstr));
    if (module_obj != MP_OBJ_NULL) {
        // Found weak-linked module
        mp_module_call_init(module_name_qstr, module_obj);
        return module_obj;
    }
    #endif

    // Couldn't find the module, so fail
    #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
    mp_raise_msg(&mp_type_ImportError, MP_ERROR_TEXT("module not found"));
    #else
    mp_raise_msg_varg(&mp_type_ImportError, MP_ERROR_TEXT("no module named '%q'"), module_name_qstr);
    #endif
}

#endif // MICROPY_ENABLE_EXTERNAL_IMPORT

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin___import___obj, 1, 5, mp_builtin___import__);
