#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "uart.h"

#include "irq.h"

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"

#define SP_START 0xe000000
#define HEAP_START 0x1000000

void setSP ( int );
unsigned int getSP ();

extern unsigned char interactive_py[];
extern unsigned int interactive_py_len;

void do_str(const char *src, mp_parse_input_kind_t input_kind, const char* srcname, size_t len) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, len, 0);
    if (lex == NULL) {
        return;
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        qstr source_name = srcname;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NONE, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}

unsigned int heap_end;

int main(int argc, char **argv) {
    uart_init();
    //timer_init();
    //enable_timer_irq ();
    
    printf ("INIT !-- Stack: 0x%08x\t Heap: 0x%08x --!\n", getSP(), heap_end );

    printf ( " ! -- My Source: \n" );
    int line = 1;
    printf ("%02d: ", line );
    for ( int i=0; i<interactive_py_len; i++)
    {
        if ( interactive_py[i] == '\n' )
        {
            line ++;
            printf ("\n%02d: ", line );
        }
        else if (interactive_py[i] == '\r')
        {
        }
        else
        {
            uart_send( interactive_py[i] );
        }
    }

    setSP ( SP_START );
    heap_end = HEAP_START;
    //heap_start = HEAP_START;

    int x = 0x800000;
    while ( x > 0 )
        x--;

    mp_init();
    
    printf ("Starting Python-code\n");
            
/*    do_str("\n\
import C;\n\
import Cdebug;\n\
\n\
def gpio_pinmode ( pin, mode ): \n\
    print (\"gpio_pinmode (\" + str(pin) + \" ,\" + str(mode) + \")\");\n\
    if ( mode >= 0 and mode <= 3 and pin >= 0 and pin <= 53 ):\n\
        ptr = C.RawPtr ( 0x20200000 );\n\
        while (pin >= 10) :\n\
            pin = pin - 10;\n\
            ptr.offset(4);\n\
            print ('b')\n\
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
    print ('gpio_write (' + str(pin) + ', ' + str(val) +')');\n\
    ptr = C.RawPtr ( 0x20200000 );\n\
    if ((val==0 or val==1) and (pin>=0 and pin<=53)) :\n\
        print ('after ifs (' + str(pin) + ')');\n\
        if (pin > 31) :\n\
            pin=pin-32;\n\
            ptr.offset(4);\n\
        if (val == 0) :\n\
            ptr.offset(40);\n\
            print ( ptr );\n\
        else :\n\
            ptr.offset(24);\n\
            print ( ptr ); \n\
        v = 1 << pin;\n\
        ptr.write ( v );\n\
        print ('Hello');\n\
\n\
gpio_pinmode ( 16, 1 );\n\
\
res = gpio_write ( 16, 0 );\n\
print ('after write');\n\
Cdebug.bsod();\n\
res = gpio_write ( 16, 1 );\n\
print ('BOAH');\n\
", MP_PARSE_FILE_INPUT);*/

    do_str ( interactive_py, MP_PARSE_FILE_INPUT, "interactive.py", (size_t) interactive_py_len );

    unsigned int sp = getSP();
    printf ("SP: %08x\n", sp );

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

