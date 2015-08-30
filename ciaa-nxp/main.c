#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"

#include "modpyb.h"

#include "ProgramScript.c" // generated with py2c.py

// maximum heap for device with 8k RAM
static char heap[4*1024];


void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
    if (lex == NULL) {
        return;
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        qstr source_name = lex->source_name;
        mp_parse_node_t pn = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(pn, source_name, MP_EMIT_OPT_NONE, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}

int main(int argc, char **argv) {
    Board_Init();
    Board_Buttons_Init();

    mp_hal_stdout_tx_str("Inicio de main");

    int i;
    for(i=0;i<sizeof(heap);i++)
   	heap[i]=0;

    int stack_dummy=0;
    MP_STATE_VM(stack_top) = (char*)&stack_dummy;
    gc_init(heap, heap + sizeof(heap));

    mp_init();
    //do_str("print('hello world!', list(x+1 for x in range(10)), end='eol\\n')", MP_PARSE_SINGLE_INPUT);
    //do_str("for i in range(10):\n  print(i)", MP_PARSE_FILE_INPUT);
    do_str(programScript, MP_PARSE_FILE_INPUT);
    mp_deinit();
    return 0;
}


void gc_collect(void) {
    mp_hal_stdout_tx_str("Se ejecuta GC!!!!!!!!!!\n");

    void* dummy = 0x10000000;
    gc_collect_start();

    mp_hal_stdout_tx_str("Se ejecuto start");
    char aux[100];
    sprintf(aux,"heap: 0x%x\n",heap);
    mp_hal_stdout_tx_str(aux);


    // Node: stack is ascending
    //gc_collect_root(&dummy, ((mp_uint_t)&dummy - (mp_uint_t)MP_STATE_VM(stack_top)) / sizeof(mp_uint_t));
    //gc_collect_root((void**)&_sbss, ((uint32_t)&_ebss - (uint32_t)&_sbss) / sizeof(uint32_t));

    void* _ram_start = 0x10000000;
    void* _heap_start = heap;

    uint32_t len = ((uint32_t)_heap_start - (uint32_t)_ram_start) / sizeof(uint32_t);
    sprintf(aux,"len: 0x%x\n",len);
    mp_hal_stdout_tx_str(aux);

    gc_collect_root((void**)_ram_start, len );

    mp_hal_stdout_tx_str("Se ejecuto root\n");

    gc_collect_end();

    mp_hal_stdout_tx_str("Se ejecuto end\n");

}


mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    return NULL;
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(uint n_args, const mp_obj_t *args, mp_map_t *kwargs) {
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

#if 0
int _lseek() {return 0;}
int _read() {return 0;}
int _write() {return 0;}
int _close() {return 0;}
void _exit(int x) {for(;;){}}
int _sbrk() {return 0;}
int _kill() {return 0;}
int _getpid() {return 0;}
int _fstat() {return 0;}
int _isatty() {return 0;}
#endif

#if 0
void *malloc(size_t n) {return NULL;}
void *calloc(size_t nmemb, size_t size) {return NULL;}
void *realloc(void *ptr, size_t size) {return NULL;}
void free(void *p) {}
int printf(const char *m, ...) {return 0;}
void *memcpy(void *dest, const void *src, size_t n) {return NULL;}
int memcmp(const void *s1, const void *s2, size_t n) {return 0;}
void *memmove(void *dest, const void *src, size_t n) {return NULL;}
void *memset(void *s, int c, size_t n) {return NULL;}
int strcmp(const char *s1, const char* s2) {return 0;}
int strncmp(const char *s1, const char* s2, size_t n) {return 0;}
size_t strlen(const char *s) {return 0;}
char *strcat(char *dest, const char *src) {return NULL;}
char *strchr(const char *dest, int c) {return NULL;}
#include <stdarg.h>
int vprintf(const char *format, va_list ap) {return 0;}
int vsnprintf(char *str,  size_t  size,  const  char  *format, va_list ap) {return 0;}

#undef putchar
int putchar(int c) {return 0;}
int puts(const char *s) {return 0;}

void _start(void) {main(0, NULL);}
#endif
