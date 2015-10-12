#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "stmhal/pyexec.h"
#include "ciaanxp_mphal.h"

#include "modpyb.h"
#include "ciaanxp_mphal.h"

//prueba uart, sacar
#include "chip.h"
#include "board.h"

#define __SECTION_EXT(type, bank, name) __attribute__ ((section("." #type ".$" #bank "." #name)))
#define __SECTION(type, bank) __attribute__ ((section("." #type ".$" #bank)))

#define __DATA_EXT(bank, name) __SECTION_EXT(data, bank, name)
#define __DATA(bank) __SECTION(data, bank)

// maximum heap for device with 32k RAM o RAM2 (40K block)
static char *stack_top;
static __DATA(RAM2) char heap[32*1024];

//extern const char programScript[];

void callb(void* arg)
{
    mp_hal_stdout_tx_strn(arg, strlen(arg));
}

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
    int stack_dummy;
    stack_top = (char*)&stack_dummy;

	memset(heap, 0, sizeof(heap));
    gc_init(heap, heap + sizeof(heap));

    mp_init();
    mp_hal_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    mp_obj_list_init(mp_sys_argv, 0);

	 Board_TIMER_Init();
	 Board_TIMER_EnableTimerAsTimer(2, 0,404000000);
	 Board_TIMER_SetCallback(2,callb,"tick desde callback t2\n");

         Board_TIMER_EnableTimerAsTimer(0, 0,104000000);
         Board_TIMER_SetCallback(0,callb,"tick desde callback t0\n");

         Board_TIMER_EnableTimerAsTimer(1, 0,204000000);
         Board_TIMER_SetCallback(1,callb,"tick desde callback t1\n");

	while(1)
	{
		//char aux[100];
		//sprintf(aux,"timer:0x%x\n\0",Chip_TIMER_ReadCount(LPC_TIMER2));
		//mp_hal_stdout_tx_strn(aux, strlen(aux));
		//mp_hal_milli_delay(500);
	}


    if (!pyexec_file("/Main.py")) {
		mp_hal_stdout_tx_strn("\nFATAL ERROR:\n", 0);
    }

    pyexec_friendly_repl();
    mp_deinit();return 0;
}


void gc_collect(void) {
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
    //gc_dump_info();
}

#if 0
mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    return NULL;
}
#endif

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
