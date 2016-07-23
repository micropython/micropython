#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/parse.h"
#include "py/mphal.h"
#include "lib/utils/pyexec.h"

#include "lib/fatfs/ff.h"

#include "modpyb.h"

#include "chip.h"
#include "board.h"

#include "bootSys.h"

// maximum heap for device with 32k RAM o RAM2 (40K block)
static char *stack_top;
//static __DATA(RAM2) char heap[32*1024];

#define HEAP_START ((uint8_t*) 0x20000000)
#define HEAP_SIZE  (64*1024)
#define HEAP_END   (HEAP_START + HEAP_SIZE)

static FATFS fatfs0;


static const char fresh_main_py[] =
"# main.py -- put your code here!\r\n"
;

static const char fresh_readme_txt[] =
"print('Welcome to Micropython on EDU-CIAA-NXP');\r\n"
"import os\r\n"
"import sys\r\n"
"import pyb\r\n"
"import gc\r\n"
;

void init_flash_fs(uint reset_mode) {
    // try to mount the flash
    FRESULT res = f_mount(&fatfs0, "/flash", 1);

    if (reset_mode == 3 || res == FR_NO_FILESYSTEM) {
        // no filesystem, or asked to reset it, so create a fresh one

        res = f_mkfs("/flash", 0, 0);
        if (res == FR_OK) {
            // success creating fresh LFS
        } else {
            __BKPT(0);
        }

        // set label
        f_setlabel("/flash/pybflash");

        // create empty main.py
        FIL fp;
        f_open(&fp, "/flash/main.py", FA_WRITE | FA_CREATE_ALWAYS);
        UINT n;
        f_write(&fp, fresh_main_py, sizeof(fresh_main_py) - 1 /* don't count null terminator */, &n);
        // TODO check we could write n bytes
        f_close(&fp);

        // keep LED on for at least 200ms
        mp_hal_milli_delay(200);
    } else if (res == FR_OK) {
        // mount sucessful
    } else {
        __BKPT(0);
    }

    // The current directory is used as the boot up directory.
    // It is set to the internal flash filesystem by default.
    f_chdrive("/flash");
}

#ifdef TESTING
    #include "testing/mainTest.c"
    #include "testing/pythonTest.c" // do_str function
#endif

int main(int argc, char **argv) {
    int stack_dummy;
soft_reset:
    stack_top = (char*)&stack_dummy;
    // memset(heap, 0, sizeof(heap));
    // gc_init(heap, heap + sizeof(heap));
    memset(HEAP_START, 0, HEAP_SIZE);
    gc_init(HEAP_START, HEAP_END);

    mp_init();
    mp_hal_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_flash_slash_lib));
    mp_obj_list_init(mp_sys_argv, 0);

    init_flash_fs(0);

    #ifdef TESTING
        // Run C tests
        startTesting();
        // Run Python tests
        do_str("import testing_MainTest\r\nm=testing_MainTest.MainTest()\r\nm.run()\r\n\0",MP_PARSE_FILE_INPUT);
        return 0;
    #endif

	// check new script from IDE
	boot();
	//__________________________


    if (!pyexec_file("/flash/Main.py")) {
        mp_hal_stdout_tx_strn("\nFATAL ERROR:\n", 0);
    }

    // Main script is finished, so now go into REPL mode.
    // The REPL mode can change, or it can request a soft reset.
    for (;;) {
        if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
            if (pyexec_raw_repl() != 0) {
                break;
            }
        } else {
            if (pyexec_friendly_repl() != 0) {
                break;
            }
        }
    }

    printf("soft reboot\n");
    goto soft_reset;
    return 0;
}


void gc_collect(void) {
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
    //gc_dump_info();
}

void NORETURN __fatal_error(const char *msg) {
    while (1);
}

void nlr_jump_fail(void *val) {
    printf("FATAL: uncaught exception %p\n", val);
    __fatal_error("");
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif

/*
<<<<<<< HEAD

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



=======
>>>>>>> master
*/

