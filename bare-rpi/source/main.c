#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <pios/uart.h>
#include <pios/jtag.h>
//#include <pios/timer.h>

#include "irq.h"

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"

#include "py/misc.h"

void setSP ( int );
unsigned int getSP ();

extern unsigned char source_interactive_py[];
extern unsigned int source_interactive_py_len;

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
   /* pios_arm_timer_setLoad ( 0x4000 );
    pios_arm_timer_init ( true, PIOS_ARM_TIMER_PRESCALE_256, true );
    enable_timer_irq();*/
                    
    /*mp_stack_ctrl_init();
    mp_stack_set_top ( &heap_end );
    mp_stack_set_limit ( 0x700000 ); */
    
    mp_verbose_flag = 2000;
    
    pios_jtag_init();
    
    
    //volatile int b=0;
    //while ( b == 0 );
        
    printf ("INIT !-- Stack: 0x%08x\t Heap: 0x%8p, 0x%8p --!\n", getSP(), &heap_start, &heap_end );
    gc_init ( &heap_start, &heap_end );
    mp_init();
/*
    do_str("print('hello world!')", MP_PARSE_SINGLE_INPUT);
    do_str("x=1", MP_PARSE_FILE_INPUT);
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
    do_str("x=1\nif (x>2) : \n    print (x)\nelse:\n    print (2)", MP_PARSE_FILE_INPUT);*/
    /*do_str("i = 1\nwhile (i < 5) : \n    pass\n    i=i+1", MP_PARSE_FILE_INPUT);
    do_str("i = 1\nwhile (i < 5) : \n    print (i)\n    i=i+1", MP_PARSE_FILE_INPUT);
    do_str("k=range(10)", MP_PARSE_FILE_INPUT);
    do_str("k=range(10)\nprint(k)", MP_PARSE_FILE_INPUT);
    do_str("for i in [1,2,3]:\n    pass", MP_PARSE_FILE_INPUT);
    do_str("for i in [1,2,3]:\n    print(i)", MP_PARSE_FILE_INPUT);
    do_str("for i in range(10):\n    pass", MP_PARSE_FILE_INPUT);*/
    do_str("for i in range(10):\n    print(i)", MP_PARSE_FILE_INPUT);
    //do_str("for i in range(10):\n    print(i,eol='')", MP_PARSE_FILE_INPUT);
   /* 
    printf ( " ! -- My Source: \n" );
    int line = 1;
    printf ("%02d: ", line );
    for ( int i=0; i<source_interactive_py_len; i++)
    {
        if ( source_interactive_py[i] == '\n' )
        {
            line ++;
            printf ("\n%02d: ", line );
        }
        else if (source_interactive_py[i] == '\r')
        {
        }
        else
        {
            pios_uart_putchar( source_interactive_py[i] );
        }
    }
    
    pios_uart_putchar('\n');
    
    printf ("Starting Python-code\n");*/

    do_str("\n\
import C;\n\
import Cdebug;\n\
\n\
def gpio_pinmode ( pin, mode ): \n\
    if ( mode >= 0 and mode <= 3 and pin >= 0 and pin <= 53 ):\n\
        ptr = C.RawPtr ( 0x20200000 );\n\
        while (pin >= 10) :\n\
            pin = pin - 10;\n\
            ptr.offset(4);\n\
        \n\
        pin = pin * 3;\n\
        mode = mode << pin;\n\
        \n\
        mask = 7 << pin;\n\
        mask = ~mask;\n\
        \n\
        val = ptr.read();\n\
        val = (val & mask) | mode;\n\
        ptr.write ( val );\n\
\n\
def gpio_write ( pin, val ):\n\
    ptr = C.RawPtr ( 0x20200000 );\n\
    if ((val==0 or val==1) and (pin>=0 and pin<=53)) :\n\
        if (pin > 31) :\n\
            pin=pin-32;\n\
            ptr.offset(4);\n\
        if (val == 0) :\n\
            ptr.offset(40);\n\
        else :\n\
            ptr.offset(28);\n\
        v = 1 << pin;\n\
        ptr.write ( v );\n\
\n\
gpio_pinmode ( 16, 1 );\n\
while (1):\n\
    res = gpio_write ( 16, 0 );\n\
    x = 0x8000\n\
    while ( x > 0 ):\n\
        x = x - 1\n\
    res = gpio_write ( 16, 1 );\n\
    x = 0x8000\n\
    while ( x > 0 ):\n\
        x = x - 1\n\
", MP_PARSE_FILE_INPUT);
/*
    do_str ( (char*) source_interactive_py, MP_PARSE_FILE_INPUT );

    unsigned int sp = getSP();
    printf ("SP: %08x\n", sp );

    mp_deinit();*/
    
    printf ("Finished, exiting!\n");
    while (1);
    
    return 0;
}

//mp_uint_t gc_helper_get_regs_and_sp(mp_uint_t *regs);
void gc_collect(void) {
    /*#if MICROPY_ENABLE_GC==1
    gc_collect_start();

    // get the registers and the sp
    mp_uint_t regs[10];
    mp_uint_t sp = gc_helper_get_regs_and_sp(regs);

    // trace the stack, including the registers (since they live on the stack in this function)
    gc_collect_root((void**)sp, ((uint32_t)HEAP_START - sp) / sizeof(uint32_t));

    // end the GC
    gc_collect_end();
    #endif*/
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
}

void NORETURN __fatal_error(const char *msg) {
    printf ("Oops. That should not have happened.\n");
    while ( 1 );
    //blinkloop();
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf(" ? -- Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    int x = 0x8000;
    while ( x > 0 )
        x--;
    //printf (" ? -- Restarting");
    
    //main (0,0 );
    while (1);
    //__fatal_error("Assertion failed");
}
#endif

