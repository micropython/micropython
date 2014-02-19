#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "lexer.h"
#include "lexerunix.h"
#include "parse.h"
#include "obj.h"
#include "parsehelper.h"
#include "compile.h"
#include "runtime0.h"
#include "runtime.h"
#include "map.h"
#include "builtin.h"

#define PATH_SEP_CHAR '/'

mp_obj_t sys_path;

mp_import_stat_t stat_dir_or_file(vstr_t *path) {
    //printf("stat %s\n", vstr_str(path));
    mp_import_stat_t stat = mp_import_stat(vstr_str(path));
    if (stat == MP_IMPORT_STAT_DIR) {
        return stat;
    }
    vstr_add_str(path, ".py");
    stat = mp_import_stat(vstr_str(path));
    if (stat == MP_IMPORT_STAT_FILE) {
        return stat;
    }
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_import_stat_t find_file(const char *file_str, uint file_len, vstr_t *dest) {
    // extract the list of paths
    uint path_num = 0;
    mp_obj_t *path_items;
    if (sys_path != MP_OBJ_NULL) {
        mp_obj_list_get(sys_path, &path_num, &path_items);
    }

    if (path_num == 0) {
        // sys_path is empty, so just use the given file name
        vstr_add_strn(dest, file_str, file_len);
        return stat_dir_or_file(dest);
    } else {
        // go through each path looking for a directory or file
        for (int i = 0; i < path_num; i++) {
            vstr_reset(dest);
            uint p_len;
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
}

void do_load(mp_obj_t module_obj, vstr_t *file) {
    // create the lexer
    mp_lexer_t *lex = mp_lexer_new_from_file(vstr_str(file));

    if (lex == NULL) {
        // we verified the file exists using stat, but lexer could still fail
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ImportError, "ImportError: No module named '%s'", vstr_str(file)));
    }

    qstr source_name = mp_lexer_source_name(lex);

    // save the old context
    mp_map_t *old_locals = rt_locals_get();
    mp_map_t *old_globals = rt_globals_get();

    // set the new context
    rt_locals_set(mp_obj_module_get_globals(module_obj));
    rt_globals_set(mp_obj_module_get_globals(module_obj));

    // parse the imported script
    mp_parse_error_kind_t parse_error_kind;
    mp_parse_node_t pn = mp_parse(lex, MP_PARSE_FILE_INPUT, &parse_error_kind);
    mp_lexer_free(lex);

    if (pn == MP_PARSE_NODE_NULL) {
        // parse error; clean up and raise exception
        rt_locals_set(old_locals);
        rt_globals_set(old_globals);
        nlr_jump(mp_parse_make_exception(parse_error_kind));
    }

    // compile the imported script
    mp_obj_t module_fun = mp_compile(pn, source_name, false);
    mp_parse_node_free(pn);

    if (module_fun == mp_const_none) {
        // TODO handle compile error correctly
        rt_locals_set(old_locals);
        rt_globals_set(old_globals);
        return;
    }

    // complied successfully, execute it
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        rt_call_function_0(module_fun);
        nlr_pop();
    } else {
        // exception; restore context and re-raise same exception
        rt_locals_set(old_locals);
        rt_globals_set(old_globals);
        nlr_jump(nlr.ret_val);
    }
    rt_locals_set(old_locals);
    rt_globals_set(old_globals);
}

mp_obj_t mp_builtin___import__(int n_args, mp_obj_t *args) {
    /*
    printf("import:\n");
    for (int i = 0; i < n_args; i++) {
        printf("  ");
        mp_obj_print(args[i], PRINT_REPR);
        printf("\n");
    }
    */

    mp_obj_t fromtuple = mp_const_none;
    if (n_args >= 4) {
        fromtuple = args[3];
    }

    uint mod_len;
    const char *mod_str = (const char*)mp_obj_str_get_data(args[0], &mod_len);

    // check if module already exists
    mp_obj_t module_obj = mp_obj_module_get(mp_obj_str_get_qstr(args[0]));
    if (module_obj != MP_OBJ_NULL) {
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
        return mp_obj_module_get(pkg_name);
    }

    uint last = 0;
    VSTR_FIXED(path, MICROPY_PATH_MAX)
    module_obj = MP_OBJ_NULL;
    mp_obj_t top_module_obj = MP_OBJ_NULL;
    mp_obj_t outer_module_obj = MP_OBJ_NULL;
    uint i;
    for (i = 1; i <= mod_len; i++) {
        if (i == mod_len || mod_str[i] == '.') {
            // create a qstr for the module name up to this depth
            qstr mod_name = qstr_from_strn(mod_str, i);

            // find the file corresponding to the module name
            mp_import_stat_t stat;
            if (vstr_len(&path) == 0) {
                // first module in the dotted-name; search for a directory or file
                stat = find_file(mod_str, i, &path);
            } else {
                // latter module in the dotted-name; append to path
                vstr_add_char(&path, PATH_SEP_CHAR);
                vstr_add_strn(&path, mod_str + last, i - last);
                stat = stat_dir_or_file(&path);
            }

            // fail if we couldn't find the file
            if (stat == MP_IMPORT_STAT_NO_EXIST) {
                nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ImportError, "ImportError: No module named '%s'", qstr_str(mod_name)));
            }

            module_obj = mp_obj_module_get(mod_name);
            if (module_obj == MP_OBJ_NULL) {
                // module not already loaded, so load it!

                module_obj = mp_obj_new_module(mod_name);

                if (stat == MP_IMPORT_STAT_DIR) {
                    vstr_add_char(&path, PATH_SEP_CHAR);
                    vstr_add_str(&path, "__init__.py");
                    if (mp_import_stat(vstr_str(&path)) != MP_IMPORT_STAT_FILE) {
                        vstr_cut_tail(&path, sizeof("/__init__.py") - 1); // cut off /__init__.py
                        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ImportError,
                            "Per PEP-420 a dir without __init__.py (%s) is a namespace package; "
                            "namespace packages are not supported", vstr_str(&path)));
                    }
                    do_load(module_obj, &path);
                    vstr_cut_tail(&path, sizeof("/__init__.py") - 1); // cut off /__init__.py
                } else { // MP_IMPORT_STAT_FILE
                    do_load(module_obj, &path);
                    // TODO: We cannot just break here, at the very least, we must execute
                    // trailer code below. But otherwise if there're remaining components,
                    // that would be (??) object path within module, not modules path within FS.
                    // break;
                }
            }
            if (outer_module_obj != MP_OBJ_NULL) {
                qstr s = qstr_from_strn(mod_str + last, i - last);
                rt_store_attr(outer_module_obj, s, module_obj);
            }
            outer_module_obj = module_obj;
            if (top_module_obj == MP_OBJ_NULL) {
                top_module_obj = module_obj;
            }
            last = i + 1;
        }
    }

    if (i < mod_len) {
        // we loaded a package, now need to load objects from within that package
        // TODO
        assert(0);
    }

    // If fromlist is not empty, return leaf module
    if (fromtuple != mp_const_none) {
        return module_obj;
    }
    // Otherwise, we need to return top-level package
    return top_module_obj;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin___import___obj, 1, 5, mp_builtin___import__);
