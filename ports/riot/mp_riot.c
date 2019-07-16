#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"

#include "thread.h"
#include "xtimer.h"

#define MP_RIOT_TICKLEN 10000U

static void _mp_riot_tick(void *arg)
{
    mp_handle_pending();
    xtimer_set(arg, MP_RIOT_TICKLEN);
}

static xtimer_t _tick_timer = { .callback = _mp_riot_tick, .arg=&_tick_timer };

void mp_do_str(const char *src, size_t len) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, len, 0);
    if (lex == NULL) {
        printf("MemoryError: lexer could not allocate memory\n");
        return;
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_FILE_INPUT);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NONE, false);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}

void mp_riot_init(char* heap, size_t heap_size) {
    #if MICROPY_ENABLE_GC
    gc_init(heap, heap + heap_size);
    #endif

    mp_init();
    //_mp_riot_tick(&_tick_timer);
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    puts("mp_lexer_new_from_file() stub");
    return NULL;
}

mp_import_stat_t mp_import_stat(const char *path) {
    puts("mp_import_stat() stub");
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(uint n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    puts("mp_builtin_open() stub");
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val) {
    printf("micropython: FATAL: uncaught exception %p\n", val);
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));
    while(1) {}
}

static void _call_from_isr(mp_obj_t callback)
{
    char *saved_stack_top = MP_STATE_THREAD(stack_top);
    size_t saved_stack_limit = MP_STATE_THREAD(stack_limit);
    MP_STATE_THREAD(stack_top) = thread_isr_stack_start() + ISR_STACKSIZE;
    MP_STATE_THREAD(stack_limit) = ISR_STACKSIZE;

    gc_lock();
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_call_function_0(callback);
        nlr_pop();
    } else {
        puts("micropython: uncaught exception in ISR");
        mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
    }
    gc_unlock();

    MP_STATE_THREAD(stack_top) = saved_stack_top;
    MP_STATE_THREAD(stack_limit) = saved_stack_limit;
}

void mp_riot_isr_callback(void *arg)
{
    _call_from_isr(*(mp_obj_t *)arg);
}
