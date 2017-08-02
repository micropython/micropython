#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "uart.h"
#include "jtag.h"

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"

#include "py/misc.h"

unsigned int getSP ();

void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        mp_parse_compile_execute ( lex, input_kind, mp_globals_get(), mp_locals_get() );
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}

extern char heap_end, heap_start;

int main(int argc, char **argv) {    
    pios_uart_init();
    
    #if MICROPY_DEBUG_VERBOSE
    mp_verbose_flag = 2000;
    #else
    mp_verbose_flag=0;
    #endif

    // pios_jtag_init();
    
    #if MICROPY_DEBUG_VERBOSE
    printf ("INIT !-- Stack: 0x%08x\t Heap: 0x%8p, 0x%8p --!\n", getSP(), &heap_start, &heap_end );
    #endif
    gc_init ( &heap_start, &heap_end );
    mp_init();

    do_str("print('hello world!')", MP_PARSE_SINGLE_INPUT);
    do_str("x=1\nprint(x)", MP_PARSE_FILE_INPUT);
    do_str("l=list()", MP_PARSE_FILE_INPUT);
    do_str("l=list()\nprint(l)", MP_PARSE_FILE_INPUT);
    do_str("l=[1,2,3]", MP_PARSE_FILE_INPUT);
    do_str("print('hello world!')", MP_PARSE_SINGLE_INPUT);
    do_str("x=1\ny=2\nprint (x<y)", MP_PARSE_FILE_INPUT);
    do_str("x=1\ny=2\nprint (x&y)", MP_PARSE_FILE_INPUT);
    do_str("x=1\ny=2\nprint ((x<y) && (y<x))", MP_PARSE_FILE_INPUT);
    do_str("x=1\nif (x<2) : \n    print (x)", MP_PARSE_FILE_INPUT);
    do_str("x=1\nif (x<2) : \n    print (x)\nelse:\n    print (2)", MP_PARSE_FILE_INPUT);
    do_str("x=1\nif (x>2) : \n    print (x)\nelse:\n    print (2)", MP_PARSE_FILE_INPUT);
    do_str("i = 1\nwhile (i < 5) : \n    pass\n    i=i+1", MP_PARSE_FILE_INPUT);
    do_str("i = 1\nwhile (i < 5) : \n    print (i)\n    i=i+1", MP_PARSE_FILE_INPUT);
    do_str("k=range(10)", MP_PARSE_FILE_INPUT);
    do_str("k=range(10)\nprint(k)", MP_PARSE_FILE_INPUT);
    do_str("for i in [1,2,3]:\n    pass", MP_PARSE_FILE_INPUT);
    do_str("for i in [1,2,3]:\n    print(i)", MP_PARSE_FILE_INPUT);
    do_str("for i in range(10):\n    pass", MP_PARSE_FILE_INPUT);
    do_str("for i in range(10):\n    print(i)", MP_PARSE_FILE_INPUT);
    do_str("for i in range(10):\n    print(i,eol='')", MP_PARSE_FILE_INPUT);
    
    mp_deinit();
   
    while (1);
    return 0;
}

void gc_collect(void) {
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    return NULL;
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val) {
    while (1);
}

void NORETURN __fatal_error(const char *msg) {
    while ( 1 );
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf(" ? -- Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    while (1);
}
#endif

