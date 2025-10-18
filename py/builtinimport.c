/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
 * Copyright (c) 2021 Jim Mussared
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

#if MICROPY_ENABLE_EXTERNAL_IMPORT

// Must be a string of one byte.
#define PATH_SEP_CHAR "/"

// Virtual sys.path entry that maps to the frozen modules.
#define MP_FROZEN_PATH_PREFIX ".frozen/"

// Wrapper for mp_import_stat (which is provided by the port, and typically
// uses mp_vfs_import_stat) to also search frozen modules. Given an exact
// path to a file or directory (e.g. "foo/bar", foo/bar.py" or "foo/bar.mpy"),
// will return whether the path is a file, directory, or doesn't exist.
static mp_import_stat_t stat_path(vstr_t *path) {
    const char *str = vstr_null_terminated_str(path);
    #if MICROPY_MODULE_FROZEN
    // Only try and load as a frozen module if it starts with .frozen/.
    const int frozen_path_prefix_len = strlen(MP_FROZEN_PATH_PREFIX);
    if (strncmp(str, MP_FROZEN_PATH_PREFIX, frozen_path_prefix_len) == 0) {
        // Just stat (which is the return value), don't get the data.
        return mp_find_frozen_module(str + frozen_path_prefix_len, NULL, NULL);
    }
    #endif
    return mp_import_stat(str);
}

// Stat a given filesystem path to a .py file. If the file does not exist,
// then attempt to stat the corresponding .mpy file, and update the path
// argument. This is the logic that makes .py files take precedent over .mpy
// files. This uses stat_path above, rather than mp_import_stat directly, so
// that the .frozen path prefix is handled.
static mp_import_stat_t stat_file_py_or_mpy(vstr_t *path) {
    mp_import_stat_t stat = stat_path(path);
    if (stat == MP_IMPORT_STAT_FILE) {
        return stat;
    }

    #if MICROPY_PERSISTENT_CODE_LOAD
    // Didn't find .py -- try the .mpy instead by inserting an 'm' into the '.py'.
    // Note: There's no point doing this if it's a frozen path, but adding the check
    // would be extra code, and no harm letting mp_find_frozen_module fail instead.
    vstr_ins_byte(path, path->len - 2, 'm');
    stat = stat_path(path);
    if (stat == MP_IMPORT_STAT_FILE) {
        return stat;
    }
    #endif

    return MP_IMPORT_STAT_NO_EXIST;
}

// Given an import path (e.g. "foo/bar"), try and find "foo/bar" (a directory)
// or "foo/bar.(m)py" in either the filesystem or frozen modules. If the
// result is a file, the path argument will be updated to include the file
// extension.
static mp_import_stat_t stat_module(vstr_t *path) {
    mp_import_stat_t stat = stat_path(path);
    DEBUG_printf("stat %s: %d\n", vstr_str(path), stat);
    if (stat == MP_IMPORT_STAT_DIR) {
        return stat;
    }

    // Not a directory, add .py and try as a file.
    vstr_add_str(path, ".py");
    return stat_file_py_or_mpy(path);
}

// Given a top-level module name, try and find it in each of the sys.path
// entries. Note: On success, the dest argument will be updated to the matching
// path (i.e. "<entry>/mod_name(.py)").
static mp_import_stat_t stat_top_level(qstr mod_name, vstr_t *dest) {
    DEBUG_printf("stat_top_level: '%s'\n", qstr_str(mod_name));
    #if MICROPY_PY_SYS && MICROPY_PY_SYS_PATH
    size_t path_num;
    mp_obj_t *path_items;
    mp_obj_get_array(mp_sys_path, &path_num, &path_items);

    // go through each sys.path entry, trying to import "<entry>/<mod_name>".
    for (size_t i = 0; i < path_num; i++) {
        vstr_reset(dest);
        size_t p_len;
        const char *p = mp_obj_str_get_data(path_items[i], &p_len);
        if (p_len > 0) {
            // Add the path separator (unless the entry is "", i.e. cwd).
            vstr_add_strn(dest, p, p_len);
            vstr_add_char(dest, PATH_SEP_CHAR[0]);
        }
        vstr_add_str(dest, qstr_str(mod_name));
        mp_import_stat_t stat = stat_module(dest);
        if (stat != MP_IMPORT_STAT_NO_EXIST) {
            return stat;
        }
    }

    // sys.path was empty or no matches, do not search the filesystem or
    // frozen code.
    return MP_IMPORT_STAT_NO_EXIST;

    #else

    // mp_sys_path is not enabled, so just stat the given path directly.
    vstr_add_str(dest, qstr_str(mod_name));
    return stat_module(dest);

    #endif
}

#if MICROPY_MODULE_FROZEN_STR || MICROPY_ENABLE_COMPILER
static void do_load_from_lexer(mp_module_context_t *context, mp_lexer_t *lex) {
    #if MICROPY_MODULE___FILE__
    qstr source_name = lex->source_name;
    mp_store_attr(MP_OBJ_FROM_PTR(&context->module), MP_QSTR___file__, MP_OBJ_NEW_QSTR(source_name));
    #endif

    // parse, compile and execute the module in its context
    mp_obj_dict_t *mod_globals = context->module.globals;
    mp_parse_compile_execute(lex, MP_PARSE_FILE_INPUT, mod_globals, mod_globals);
}
#endif

#if (MICROPY_HAS_FILE_READER && MICROPY_PERSISTENT_CODE_LOAD) || MICROPY_MODULE_FROZEN_MPY
static void do_execute_proto_fun(const mp_module_context_t *context, mp_proto_fun_t proto_fun, qstr source_name) {
    #if MICROPY_MODULE___FILE__
    mp_store_attr(MP_OBJ_FROM_PTR(&context->module), MP_QSTR___file__, MP_OBJ_NEW_QSTR(source_name));
    #else
    (void)source_name;
    #endif

    // execute the module in its context
    mp_obj_dict_t *mod_globals = context->module.globals;

    // save context
    nlr_jump_callback_node_globals_locals_t ctx;
    ctx.globals = mp_globals_get();
    ctx.locals = mp_locals_get();

    // set new context
    mp_globals_set(mod_globals);
    mp_locals_set(mod_globals);

    // set exception handler to restore context if an exception is raised
    nlr_push_jump_callback(&ctx.callback, mp_globals_locals_set_from_nlr_jump_callback);

    // make and execute the function
    mp_obj_t module_fun = mp_make_function_from_proto_fun(proto_fun, context, NULL);
    mp_call_function_0(module_fun);

    // deregister exception handler and restore context
    nlr_pop_jump_callback(true);
}
#endif

static void do_load(mp_module_context_t *module_obj, vstr_t *file) {
    #if MICROPY_MODULE_FROZEN || MICROPY_ENABLE_COMPILER || (MICROPY_PERSISTENT_CODE_LOAD && MICROPY_HAS_FILE_READER)
    const char *file_str = vstr_null_terminated_str(file);
    #endif

    // If we support frozen modules (either as str or mpy) then try to find the
    // requested filename in the list of frozen module filenames.
    #if MICROPY_MODULE_FROZEN
    void *modref;
    int frozen_type;
    const int frozen_path_prefix_len = strlen(MP_FROZEN_PATH_PREFIX);
    if (strncmp(file_str, MP_FROZEN_PATH_PREFIX, frozen_path_prefix_len) == 0) {
        mp_find_frozen_module(file_str + frozen_path_prefix_len, &frozen_type, &modref);

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
            const mp_frozen_module_t *frozen = modref;
            module_obj->constants = frozen->constants;
            #if MICROPY_MODULE___FILE__
            qstr frozen_file_qstr = qstr_from_str(file_str + frozen_path_prefix_len);
            #else
            qstr frozen_file_qstr = MP_QSTRnull;
            #endif
            do_execute_proto_fun(module_obj, frozen->proto_fun, frozen_file_qstr);
            return;
        }
        #endif
    }

    #endif // MICROPY_MODULE_FROZEN

    qstr file_qstr = qstr_from_str(file_str);

    // If we support loading .mpy files then check if the file extension is of
    // the correct format and, if so, load and execute the file.
    #if MICROPY_HAS_FILE_READER && MICROPY_PERSISTENT_CODE_LOAD
    if (file_str[file->len - 3] == 'm') {
        mp_compiled_module_t cm;
        cm.context = module_obj;
        mp_raw_code_load_file(file_qstr, &cm);
        do_execute_proto_fun(cm.context, cm.rc, file_qstr);
        return;
    }
    #endif

    // If we can compile scripts then load the file and compile and execute it.
    #if MICROPY_ENABLE_COMPILER
    {
        mp_lexer_t *lex = mp_lexer_new_from_file(file_qstr);
        do_load_from_lexer(module_obj, lex);
        return;
    }
    #else
    // If we get here then the file was not frozen and we can't compile scripts.
    mp_raise_msg(&mp_type_ImportError, MP_ERROR_TEXT("script compilation not supported"));
    #endif
}

// Convert a relative (to the current module) import, going up "level" levels,
// into an absolute import.
static void evaluate_relative_import(mp_int_t level, const char **module_name, size_t *module_name_len) {
    // What we want to do here is to take the name of the current module,
    // remove <level> trailing components, and concatenate the passed-in
    // module name.
    // For example, level=3, module_name="foo.bar", __name__="a.b.c.d" --> "a.foo.bar"
    // "Relative imports use a module's __name__ attribute to determine that
    // module's position in the package hierarchy."
    // http://legacy.python.org/dev/peps/pep-0328/#relative-imports-and-name

    mp_obj_t current_module_name_obj = mp_obj_dict_get(MP_OBJ_FROM_PTR(mp_globals_get()), MP_OBJ_NEW_QSTR(MP_QSTR___name__));
    assert(current_module_name_obj != MP_OBJ_NULL);

    #if MICROPY_MODULE_OVERRIDE_MAIN_IMPORT && MICROPY_CPYTHON_COMPAT
    if (MP_OBJ_QSTR_VALUE(current_module_name_obj) == MP_QSTR___main__) {
        // This is a module loaded by -m command-line switch (e.g. unix port),
        // and so its __name__ has been set to "__main__". Get its real name
        // that we stored during import in the __main__ attribute.
        current_module_name_obj = mp_obj_dict_get(MP_OBJ_FROM_PTR(mp_globals_get()), MP_OBJ_NEW_QSTR(MP_QSTR___main__));
    }
    #endif

    // If we have a __path__ in the globals dict, then we're a package.
    bool is_pkg = mp_map_lookup(&mp_globals_get()->map, MP_OBJ_NEW_QSTR(MP_QSTR___path__), MP_MAP_LOOKUP);

    #if DEBUG_PRINT
    DEBUG_printf("Current module/package: ");
    mp_obj_print_helper(MICROPY_DEBUG_PRINTER, current_module_name_obj, PRINT_REPR);
    DEBUG_printf(", is_package: %d", is_pkg);
    DEBUG_printf("\n");
    #endif

    size_t current_module_name_len;
    const char *current_module_name = mp_obj_str_get_data(current_module_name_obj, &current_module_name_len);

    const char *p = current_module_name + current_module_name_len;
    if (is_pkg) {
        // If we're evaluating relative to a package, then take off one fewer
        // level (i.e. the relative search starts inside the package, rather
        // than as a sibling of the package).
        --level;
    }

    // Walk back 'level' dots (or run out of path).
    while (level && p > current_module_name) {
        if (*--p == '.') {
            --level;
        }
    }

    // We must have some component left over to import from.
    if (p == current_module_name) {
        mp_raise_msg(&mp_type_ImportError, MP_ERROR_TEXT("can't perform relative import"));
    }

    // New length is len("<chopped path>.<module_name>"). Note: might be one byte
    // more than we need if module_name is empty (for the extra . we will
    // append).
    uint new_module_name_len = (size_t)(p - current_module_name) + 1 + *module_name_len;
    char *new_mod = mp_local_alloc(new_module_name_len);
    memcpy(new_mod, current_module_name, p - current_module_name);

    // Only append ".<module_name>" if there was one).
    if (*module_name_len != 0) {
        new_mod[p - current_module_name] = '.';
        memcpy(new_mod + (p - current_module_name) + 1, *module_name, *module_name_len);
    } else {
        --new_module_name_len;
    }

    // Copy into a QSTR.
    qstr new_mod_q = qstr_from_strn(new_mod, new_module_name_len);
    mp_local_free(new_mod);

    DEBUG_printf("Resolved base name for relative import: '%s'\n", qstr_str(new_mod_q));
    *module_name = qstr_str(new_mod_q);
    *module_name_len = new_module_name_len;
}

typedef struct _nlr_jump_callback_node_unregister_module_t {
    nlr_jump_callback_node_t callback;
    qstr name;
} nlr_jump_callback_node_unregister_module_t;

static void unregister_module_from_nlr_jump_callback(void *ctx_in) {
    nlr_jump_callback_node_unregister_module_t *ctx = ctx_in;
    mp_map_t *mp_loaded_modules_map = &MP_STATE_VM(mp_loaded_modules_dict).map;
    mp_map_lookup(mp_loaded_modules_map, MP_OBJ_NEW_QSTR(ctx->name), MP_MAP_LOOKUP_REMOVE_IF_FOUND);
}

// Load a module at the specified absolute path, possibly as a submodule of the given outer module.
// full_mod_name:    The full absolute path up to this level (e.g. "foo.bar.baz").
// level_mod_name:   The final component of the path (e.g. "baz").
// outer_module_obj: The parent module (we need to store this module as an
//                   attribute on it) (or MP_OBJ_NULL for top-level).
// override_main:    Whether to set the __name__ to "__main__" (and use __main__
//                   for the actual path).
static mp_obj_t process_import_at_level(qstr full_mod_name, qstr level_mod_name, mp_obj_t outer_module_obj, bool override_main) {
    // Immediately return if the module at this level is already loaded.
    mp_map_elem_t *elem;

    #if MICROPY_PY_SYS && MICROPY_PY_SYS_PATH
    // If sys.path is empty, the intention is to force using a built-in. This
    // means we should also ignore any loaded modules with the same name
    // which may have come from the filesystem.
    size_t path_num;
    mp_obj_t *path_items;
    mp_obj_get_array(mp_sys_path, &path_num, &path_items);
    if (path_num)
    #endif
    {
        elem = mp_map_lookup(&MP_STATE_VM(mp_loaded_modules_dict).map, MP_OBJ_NEW_QSTR(full_mod_name), MP_MAP_LOOKUP);
        if (elem) {
            return elem->value;
        }
    }

    VSTR_FIXED(path, MICROPY_ALLOC_PATH_MAX);
    mp_import_stat_t stat = MP_IMPORT_STAT_NO_EXIST;
    mp_obj_t module_obj;

    if (outer_module_obj == MP_OBJ_NULL) {
        // First module in the dotted-name path.
        DEBUG_printf("Searching for top-level module\n");

        // An import of a non-extensible built-in will always bypass the
        // filesystem. e.g. `import micropython` or `import pyb`. So try and
        // match a non-extensible built-ins first.
        module_obj = mp_module_get_builtin(level_mod_name, false);
        if (module_obj != MP_OBJ_NULL) {
            return module_obj;
        }

        // Next try the filesystem. Search for a directory or file relative to
        // all the locations in sys.path.
        stat = stat_top_level(level_mod_name, &path);

        #if MICROPY_HAVE_REGISTERED_EXTENSIBLE_MODULES
        // If filesystem failed, now try and see if it matches an extensible
        // built-in module.
        if (stat == MP_IMPORT_STAT_NO_EXIST) {
            module_obj = mp_module_get_builtin(level_mod_name, true);
            if (module_obj != MP_OBJ_NULL) {
                return module_obj;
            }
        }
        #endif
    } else {
        DEBUG_printf("Searching for sub-module\n");

        #if MICROPY_MODULE_BUILTIN_SUBPACKAGES
        // If the outer module is a built-in (because its map is in ROM), then
        // treat it like a package if it contains this submodule in its
        // globals dict.
        mp_obj_module_t *mod = MP_OBJ_TO_PTR(outer_module_obj);
        if (mod->globals->map.is_fixed) {
            elem = mp_map_lookup(&mod->globals->map, MP_OBJ_NEW_QSTR(level_mod_name), MP_MAP_LOOKUP);
            // Also verify that the entry in the globals dict is in fact a module.
            if (elem && mp_obj_is_type(elem->value, &mp_type_module)) {
                return elem->value;
            }
        }
        #endif

        // If the outer module is a package, it will have __path__ set.
        // We can use that as the path to search inside.
        mp_obj_t dest[2];
        mp_load_method_maybe(outer_module_obj, MP_QSTR___path__, dest);
        if (dest[0] != MP_OBJ_NULL) {
            // e.g. __path__ will be "<matched search path>/foo/bar"
            vstr_add_str(&path, mp_obj_str_get_str(dest[0]));

            // Add the level module name to the path to get "<matched search path>/foo/bar/baz".
            vstr_add_char(&path, PATH_SEP_CHAR[0]);
            vstr_add_str(&path, qstr_str(level_mod_name));

            stat = stat_module(&path);
        }
    }

    // Not already loaded, and not a built-in, so look at the stat result from the filesystem/frozen.

    if (stat == MP_IMPORT_STAT_NO_EXIST) {
        // Not found -- fail.
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_raise_msg(&mp_type_ImportError, MP_ERROR_TEXT("module not found"));
        #else
        mp_raise_msg_varg(&mp_type_ImportError, MP_ERROR_TEXT("no module named '%q'"), full_mod_name);
        #endif
    }

    // Module was found on the filesystem/frozen, try and load it.
    DEBUG_printf("Found path to load: %.*s\n", (int)vstr_len(&path), vstr_str(&path));

    // Prepare for loading from the filesystem. Create a new shell module
    // and register it in sys.modules.  Also make sure we remove it if
    // there is any problem below.
    module_obj = mp_obj_new_module(full_mod_name);
    nlr_jump_callback_node_unregister_module_t ctx;
    ctx.name = full_mod_name;
    nlr_push_jump_callback(&ctx.callback, unregister_module_from_nlr_jump_callback);

    #if MICROPY_MODULE_OVERRIDE_MAIN_IMPORT
    // If this module is being loaded via -m on unix, then
    // override __name__ to "__main__". Do this only for *modules*
    // however - packages never have their names replaced, instead
    // they're -m'ed using a special __main__ submodule in them. (This all
    // apparently is done to not touch the package name itself, which is
    // important for future imports).
    if (override_main && stat != MP_IMPORT_STAT_DIR) {
        mp_obj_module_t *o = MP_OBJ_TO_PTR(module_obj);
        mp_obj_dict_store(MP_OBJ_FROM_PTR(o->globals), MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR___main__));
        #if MICROPY_CPYTHON_COMPAT
        // Store module as "__main__" in the dictionary of loaded modules (returned by sys.modules).
        mp_obj_dict_store(MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_loaded_modules_dict)), MP_OBJ_NEW_QSTR(MP_QSTR___main__), module_obj);
        // Store real name in "__main__" attribute. Need this for
        // resolving relative imports later. "__main__ was chosen
        // semi-randonly, to reuse existing qstr's.
        mp_obj_dict_store(MP_OBJ_FROM_PTR(o->globals), MP_OBJ_NEW_QSTR(MP_QSTR___main__), MP_OBJ_NEW_QSTR(full_mod_name));
        #endif
    }
    #endif // MICROPY_MODULE_OVERRIDE_MAIN_IMPORT

    if (stat == MP_IMPORT_STAT_DIR) {
        // Directory (i.e. a package).
        DEBUG_printf("%.*s is dir\n", (int)vstr_len(&path), vstr_str(&path));

        // Store the __path__ attribute onto this module.
        // https://docs.python.org/3/reference/import.html
        // "Specifically, any module that contains a __path__ attribute is considered a package."
        // This gets used later to locate any subpackages of this module.
        mp_store_attr(module_obj, MP_QSTR___path__, mp_obj_new_str(vstr_str(&path), vstr_len(&path)));
        size_t orig_path_len = path.len;
        vstr_add_str(&path, PATH_SEP_CHAR "__init__.py");

        // execute "path/__init__.py" (if available).
        if (stat_file_py_or_mpy(&path) == MP_IMPORT_STAT_FILE) {
            do_load(MP_OBJ_TO_PTR(module_obj), &path);
        } else {
            // No-op. Nothing to load.
            // mp_warning("%s is imported as namespace package", vstr_str(&path));
        }
        // Remove /__init__.py suffix from path.
        path.len = orig_path_len;
    } else { // MP_IMPORT_STAT_FILE
        // File -- execute "path.(m)py".
        do_load(MP_OBJ_TO_PTR(module_obj), &path);
        // Note: This should be the last component in the import path. If
        // there are remaining components then in the next call to
        // process_import_at_level will detect that it doesn't have
        // a __path__ attribute, and not attempt to stat it.
    }

    if (outer_module_obj != MP_OBJ_NULL) {
        // If it's a sub-module then make it available on the parent module.
        mp_store_attr(outer_module_obj, level_mod_name, module_obj);
    }

    nlr_pop_jump_callback(false);

    return module_obj;
}

mp_obj_t mp_builtin___import___default(size_t n_args, const mp_obj_t *args) {
    #if DEBUG_PRINT
    DEBUG_printf("__import__:\n");
    for (size_t i = 0; i < n_args; i++) {
        DEBUG_printf("  ");
        mp_obj_print_helper(MICROPY_DEBUG_PRINTER, args[i], PRINT_REPR);
        DEBUG_printf("\n");
    }
    #endif

    // This is the import path, with any leading dots stripped.
    // "import foo.bar" --> module_name="foo.bar"
    // "from foo.bar import baz" --> module_name="foo.bar"
    // "from . import foo" --> module_name=""
    // "from ...foo.bar import baz" --> module_name="foo.bar"
    mp_obj_t module_name_obj = args[0];

    // These are the imported names.
    // i.e. "from foo.bar import baz, zap" --> fromtuple=("baz", "zap",)
    // Note: There's a special case on the Unix port, where this is set to mp_const_false which means that it's __main__.
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

    size_t module_name_len;
    const char *module_name = mp_obj_str_get_data(module_name_obj, &module_name_len);

    if (level != 0) {
        // Turn "foo.bar" with level=3 into "<current module 3 components>.foo.bar".
        // Current module name is extracted from globals().__name__.
        evaluate_relative_import(level, &module_name, &module_name_len);
        // module_name is now an absolute module path.
    }

    if (module_name_len == 0) {
        mp_raise_ValueError(NULL);
    }

    DEBUG_printf("Starting module search for '%s'\n", module_name);

    mp_obj_t top_module_obj = MP_OBJ_NULL;
    mp_obj_t outer_module_obj = MP_OBJ_NULL;

    // Iterate the absolute path, finding the end of each component of the path.
    // foo.bar.baz
    //    ^   ^   ^
    size_t current_component_start = 0;
    for (size_t i = 1; i <= module_name_len; i++) {
        if (i == module_name_len || module_name[i] == '.') {
            // The module name up to this depth (e.g. foo.bar.baz).
            qstr full_mod_name = qstr_from_strn(module_name, i);
            // The current level name (e.g. baz).
            qstr level_mod_name = qstr_from_strn(module_name + current_component_start, i - current_component_start);

            DEBUG_printf("Processing module: '%s' at level '%s'\n", qstr_str(full_mod_name), qstr_str(level_mod_name));

            #if MICROPY_MODULE_OVERRIDE_MAIN_IMPORT
            // On unix, if this is being loaded via -m (indicated by sentinel
            // fromtuple=mp_const_false), then handle that if it's the final
            // component.
            bool override_main = (i == module_name_len && fromtuple == mp_const_false);
            #else
            bool override_main = false;
            #endif

            // Import this module.
            mp_obj_t module_obj = process_import_at_level(full_mod_name, level_mod_name, outer_module_obj, override_main);

            // Set this as the parent module, and remember the top-level module if it's the first.
            outer_module_obj = module_obj;
            if (top_module_obj == MP_OBJ_NULL) {
                top_module_obj = module_obj;
            }

            current_component_start = i + 1;
        }
    }

    if (fromtuple != mp_const_none) {
        // If fromtuple is not empty, return leaf module
        return outer_module_obj;
    } else {
        // Otherwise, we need to return top-level package
        return top_module_obj;
    }
}

#else // MICROPY_ENABLE_EXTERNAL_IMPORT

mp_obj_t mp_builtin___import___default(size_t n_args, const mp_obj_t *args) {
    // Check that it's not a relative import.
    if (n_args >= 5 && MP_OBJ_SMALL_INT_VALUE(args[4]) != 0) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("relative import"));
    }

    // Check if the module is already loaded.
    mp_map_elem_t *elem = mp_map_lookup(&MP_STATE_VM(mp_loaded_modules_dict).map, args[0], MP_MAP_LOOKUP);
    if (elem) {
        return elem->value;
    }

    // Try the name directly as a non-extensible built-in (e.g. `micropython`).
    qstr module_name_qstr = mp_obj_str_get_qstr(args[0]);
    mp_obj_t module_obj = mp_module_get_builtin(module_name_qstr, false);
    if (module_obj != MP_OBJ_NULL) {
        return module_obj;
    }
    #if MICROPY_HAVE_REGISTERED_EXTENSIBLE_MODULES
    // Now try as an extensible built-in (e.g. `time`).
    module_obj = mp_module_get_builtin(module_name_qstr, true);
    if (module_obj != MP_OBJ_NULL) {
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
