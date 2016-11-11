#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "lib/utils/pyexec.h"

#include "emscripten.h"

// TODO: make this work properly with emscripten
#ifdef _WIN32
#define PATHLIST_SEP_CHAR ';'
#else
#define PATHLIST_SEP_CHAR ':'
#endif

void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
    if (lex == NULL) {
        printf("MemoryError: lexer could not allocate memory\n");
        return;
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NONE, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}

static char *stack_top;

#if MICROPY_ENABLE_GC
static char heap[2048];
#endif

void mp_js_init() {
    gc_init(heap, heap + sizeof(heap));
    mp_init();

    char *home = getenv("HOME");
    char *path = getenv("MICROPYPATH");
    if (path == NULL) {
        #ifdef MICROPY_PY_SYS_PATH_DEFAULT
        path = MICROPY_PY_SYS_PATH_DEFAULT;
        #else
        path = "~/.micropython/lib:/usr/lib/micropython";
        #endif
    }
    mp_uint_t path_num = 1; // [0] is for current dir (or base dir of the script)
    for (char *p = path; p != NULL; p = strchr(p, PATHLIST_SEP_CHAR)) {
        path_num++;
        if (p != NULL) {
            p++;
        }
    }
    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_path), path_num);
    mp_obj_t *path_items;
    mp_obj_list_get(mp_sys_path, &path_num, &path_items);
    path_items[0] = MP_OBJ_NEW_QSTR(MP_QSTR_);
    {
    char *p = path;
    for (mp_uint_t i = 1; i < path_num; i++) {
        char *p1 = strchr(p, PATHLIST_SEP_CHAR);
        if (p1 == NULL) {
            p1 = p + strlen(p);
        }
        if (p[0] == '~' && p[1] == '/' && home != NULL) {
            // Expand standalone ~ to $HOME
            int home_l = strlen(home);
            vstr_t vstr;
            vstr_init(&vstr, home_l + (p1 - p - 1) + 1);
            vstr_add_strn(&vstr, home, home_l);
            vstr_add_strn(&vstr, p + 1, p1 - p - 1);
            path_items[i] = mp_obj_new_str_from_vstr(&mp_type_str, &vstr);
        } else {
            path_items[i] = MP_OBJ_NEW_QSTR(qstr_from_strn(p, p1 - p));
        }
        p = p1 + 1;
    }
    }
}

void mp_js_run(const char * code) {
    do_str(code, MP_PARSE_FILE_INPUT);
}

void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
    gc_dump_info();
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    // to do this by calling out to node.js code, we'll have to do something like 
    // fs.readFileSync(filename, 'utf-8')
    // and then put that back into the emscripten heap, so that it's a pointer we can access from C
    // and then use that to produce a micropython lexer as above.

    // eurgh this is disgusting, let's just implement this in JS

    const char *code_buf = (const char *)EM_ASM_INT({
        try {
            var file = UTF8ToString($0);
            var fs = require('fs');

            var code = fs.readFileSync(file, 'utf-8');
            ret = Runtime.stackAlloc((code.length << 2) + 1);
            writeStringToMemory(code, ret);
            return ret;
        } catch (e) {
            return 0;
        }
    }, filename);

    if (code_buf == 0) {
        return NULL;
    } else {
        mp_lexer_t* lex = mp_lexer_new_from_str_len(qstr_from_str(filename), code_buf, strlen(code_buf), 0);
        return lex;
    }
}

mp_import_stat_t mp_import_stat(const char *path) {
    int s = EM_ASM_INT({
        try {
            var fs = require('fs');
            var stat = fs.statSync(UTF8ToString($0));
            return stat.isDirectory() ? 1 : 0;
        } catch(e) {
            return -1;
        }
    }, path);

    if (s == -1) {
        return MP_IMPORT_STAT_NO_EXIST;
    } else if (s == 1) {
        return MP_IMPORT_STAT_DIR;
    } else {
        return MP_IMPORT_STAT_FILE;
    }
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    printf("open a file!\r\n");
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val) {
}

void NORETURN __fatal_error(const char *msg) {
    while (1);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif

