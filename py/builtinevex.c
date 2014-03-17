#include <stdint.h>

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

STATIC mp_obj_t parse_compile_execute(mp_obj_t o_in, mp_parse_input_kind_t parse_input_kind) {
    uint str_len;
    const char *str = mp_obj_str_get_data(o_in, &str_len);

    // create the lexer
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_string_gt_, str, str_len, 0);
    qstr source_name = mp_lexer_source_name(lex);

    // parse the string
    mp_parse_error_kind_t parse_error_kind;
    mp_parse_node_t pn = mp_parse(lex, parse_input_kind, &parse_error_kind);
    mp_lexer_free(lex);

    if (pn == MP_PARSE_NODE_NULL) {
        // parse error; raise exception
        nlr_jump(mp_parse_make_exception(parse_error_kind));
    }

    // compile the string
    mp_obj_t module_fun = mp_compile(pn, source_name, false);
    mp_parse_node_free(pn);

    if (module_fun == mp_const_none) {
        // TODO handle compile error correctly
        return mp_const_none;
    }

    // complied successfully, execute it
    return rt_call_function_0(module_fun);
}

STATIC mp_obj_t mp_builtin_eval(mp_obj_t o_in) {
    return parse_compile_execute(o_in, MP_PARSE_EVAL_INPUT);
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_eval_obj, mp_builtin_eval);

STATIC mp_obj_t mp_builtin_exec(uint n_args, const mp_obj_t *args) {
    // Unconditional getting/setting assumes that these operations
    // are cheap, which is the case when this comment was written.
    mp_map_t *old_globals = rt_globals_get();
    mp_map_t *old_locals = rt_locals_get();
    if (n_args > 1) {
        mp_obj_t globals = args[1];
        mp_obj_t locals;
        if (n_args > 2) {
            locals = args[2];
        } else {
            locals = globals;
        }
        rt_globals_set(mp_obj_dict_get_map(globals));
        rt_locals_set(mp_obj_dict_get_map(locals));
    }
    mp_obj_t res = parse_compile_execute(args[0], MP_PARSE_FILE_INPUT);
    // TODO if the above call throws an exception, then we never get to reset the globals/locals
    rt_globals_set(old_globals);
    rt_locals_set(old_locals);
    return res;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_exec_obj, 1, 3, mp_builtin_exec);
