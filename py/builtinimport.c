#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "lexer.h"
#include "lexerunix.h"
#include "parse.h"
#include "obj.h"
#include "compile.h"
#include "runtime0.h"
#include "runtime.h"
#include "map.h"
#include "builtin.h"

mp_obj_t mp_builtin___import__(int n, mp_obj_t *args) {
    /*
    printf("import:\n");
    for (int i = 0; i < n; i++) {
        printf("  ");
        mp_obj_print(args[i]);
        printf("\n");
    }
    */

    // find the file to import
    qstr mod_name = mp_obj_get_qstr(args[0]);
    mp_lexer_t *lex = mp_import_open_file(mod_name);
    if (lex == NULL) {
        // TODO handle lexer error correctly
        return mp_const_none;
    }

    // create a new module object
    mp_obj_t module_obj = mp_obj_new_module(mp_obj_get_qstr(args[0]));

    // save the old context
    mp_map_t *old_locals = rt_locals_get();
    mp_map_t *old_globals = rt_globals_get();

    // set the new context
    rt_locals_set(mp_obj_module_get_globals(module_obj));
    rt_globals_set(mp_obj_module_get_globals(module_obj));

    // parse the imported script
    mp_parse_node_t pn = mp_parse(lex, MP_PARSE_FILE_INPUT);
    mp_lexer_free(lex);

    if (pn == MP_PARSE_NODE_NULL) {
        // TODO handle parse error correctly
        rt_locals_set(old_locals);
        rt_globals_set(old_globals);
        return mp_const_none;
    }

    mp_obj_t module_fun = mp_compile(pn, false);

    if (module_fun == mp_const_none) {
        // TODO handle compile error correctly
        rt_locals_set(old_locals);
        rt_globals_set(old_globals);
        return mp_const_none;
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

    return module_obj;
}
