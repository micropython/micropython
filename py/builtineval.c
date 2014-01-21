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
#include "compile.h"
#include "runtime0.h"
#include "runtime.h"
#include "map.h"
#include "builtin.h"

static mp_obj_t mp_builtin_eval(mp_obj_t o_in) {
    uint str_len;
    const byte *str = qstr_data(mp_obj_get_qstr(o_in), &str_len);

    // create the lexer
    mp_lexer_t *lex = mp_lexer_new_from_str_len("<string>", (const char*)str, str_len, 0);

    // parse the string
    qstr parse_exc_id;
    const char *parse_exc_msg;
    mp_parse_node_t pn = mp_parse(lex, MP_PARSE_EVAL_INPUT, &parse_exc_id, &parse_exc_msg);
    qstr source_name = mp_lexer_source_name(lex);
    mp_lexer_free(lex);

    if (pn == MP_PARSE_NODE_NULL) {
        // parse error; raise exception
        nlr_jump(mp_obj_new_exception_msg(parse_exc_id, parse_exc_msg));
    }

    // compile the string
    mp_obj_t module_fun = mp_compile(pn, source_name, false);

    if (module_fun == mp_const_none) {
        // TODO handle compile error correctly
        return mp_const_none;
    }

    // complied successfully, execute it
    return rt_call_function_0(module_fun);
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_eval_obj, mp_builtin_eval);
