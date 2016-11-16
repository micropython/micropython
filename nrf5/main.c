/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Glenn Ruben Bakke
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/lexer.h"
#include "py/parse.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py/gc.h"
#include "py/compile.h"
#include "lib/utils/pyexec.h"
#include "readline.h"
#include "gccollect.h"
#include "led.h"
#include "uart.h"
#include "nrf.h"

void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
    if (lex == NULL) {
        printf("MemoryError: lexer could not allocate memory\n");
        return;
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        qstr source_name = lex->source_name;
        mp_parse_tree_t pn = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&pn, source_name, MP_EMIT_OPT_NONE, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}

extern uint32_t _heap_start;
extern uint32_t _heap_end;

int main(int argc, char **argv) {
    
    // Stack limit should be less than real stack size, so we have a chance
    // to recover from limit hit.  (Limit is measured in bytes.)
    mp_stack_set_limit((char*)&_ram_end - (char*)&_heap_end - 1024);

    led_init();

    gc_init(&_heap_start, &_heap_end);
    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    mp_obj_list_init(mp_sys_argv, 0);

    readline_init0();

    uart_init0();
    {
        mp_obj_t args[2] = {
            MP_OBJ_NEW_SMALL_INT(PYB_UART_1),
            MP_OBJ_NEW_SMALL_INT(115200),
        };
        MP_STATE_PORT(pyb_stdio_uart) = pyb_uart_type.make_new((mp_obj_t)&pyb_uart_type, MP_ARRAY_SIZE(args), 0, args);
    }

#if MICROPY_HW_LED_TRICOLOR
    do_str("import pyb\r\n" \
           "pyb.LED(1).on()",
           MP_PARSE_FILE_INPUT);
#else

    do_str("import pyb\r\n" \
           "pyb.LED(1).on()\r\n" \
           "pyb.LED(3).on()",
           MP_PARSE_FILE_INPUT);
#endif

    // Main script is finished, so now go into REPL mode.
    // The REPL mode can change, or it can request a soft reset.
    for (;;) {
        if (pyexec_friendly_repl() != 0) {
            break;
        }
    }

    mp_deinit();

    return 0;
}

void HardFault_Handler(void)
{
#if NRF52
	static volatile uint32_t reg;
	static volatile uint32_t reg2;
	static volatile uint32_t bfar;
	reg = SCB->HFSR;
	reg2 = SCB->CFSR;
	bfar = SCB->BFAR;
	for (int i = 0; i < 0; i++)
	{
		(void)reg;
		(void)reg2;
		(void)bfar;
	}
#endif
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

void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}

void _start(void) {main(0, NULL);}
