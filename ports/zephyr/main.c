/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2016 Linaro Limited
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

#include <zephyr.h>
#ifdef CONFIG_NETWORKING
#include <net/net_context.h>
#endif

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/stackctrl.h"
#include "lib/utils/pyexec.h"
#include "lib/mp-readline/readline.h"

void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
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
static char heap[MICROPY_HEAP_SIZE];

void init_zephyr(void) {
    // TODO: Make addresses configurable
    #ifdef CONFIG_NETWORKING
    if (net_if_get_default() == NULL) {
        // If there's no default networking interface,
        // there's nothing to configure.
        return;
    }
    #endif
    #ifdef CONFIG_NET_IPV4
    static struct in_addr in4addr_my = {{{192, 0, 2, 1}}};
    net_if_ipv4_addr_add(net_if_get_default(), &in4addr_my, NET_ADDR_MANUAL, 0);
    static struct in_addr in4netmask_my = {{{255, 255, 255, 0}}};
    net_if_ipv4_set_netmask(net_if_get_default(), &in4netmask_my);
    static struct in_addr in4gw_my = {{{192, 0, 2, 2}}};
    net_if_ipv4_set_gw(net_if_get_default(), &in4gw_my);
    #endif
    #ifdef CONFIG_NET_IPV6
    // 2001:db8::1
    static struct in6_addr in6addr_my = {{{0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}}};
    net_if_ipv6_addr_add(net_if_get_default(), &in6addr_my, NET_ADDR_MANUAL, 0);
    #endif
}

int real_main(void) {
    int stack_dummy;
    stack_top = (char*)&stack_dummy;
    mp_stack_set_top(stack_top);
    // Make MicroPython's stack limit somewhat smaller than full stack available
    mp_stack_set_limit(CONFIG_MAIN_STACK_SIZE - 512);

    init_zephyr();

soft_reset:
    #if MICROPY_ENABLE_GC
    gc_init(heap, heap + sizeof(heap));
    #endif
    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    mp_obj_list_init(mp_sys_argv, 0);

    #if MICROPY_MODULE_FROZEN
    pyexec_frozen_module("main.py");
    #endif

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
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
    //gc_dump_info();
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

NORETURN void nlr_jump_fail(void *val) {
    while (1);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif
