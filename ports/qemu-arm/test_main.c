#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <setjmp.h>

#include "py/obj.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py/gc.h"
#include "py/mperrno.h"

#include "tinytest.h"
#include "tinytest_macros.h"

#define HEAP_SIZE (100 * 1024)

#include "genhdr/tests.h"

int main() {
    mp_stack_ctrl_init();
    mp_stack_set_limit(10240);
    static uint32_t heap[HEAP_SIZE / sizeof(uint32_t)];
    upytest_set_heap(heap, (char*)heap + HEAP_SIZE);
    int r = tinytest_main(0, NULL, groups);
    printf("status: %d\n", r);
    return r;
}

void gc_collect(void) {
    gc_collect_start();

    // get the registers and the sp
    // TODO get registers
    volatile mp_uint_t dummy;
    void *sp = (void*)&dummy;

    // trace the stack, including the registers (since they live on the stack in this function)
    gc_collect_root((void**)sp, ((uint32_t)MP_STATE_THREAD(stack_top) - (uint32_t)sp) / sizeof(uint32_t));

    gc_collect_end();
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val) {
    printf("uncaught NLR\n");
    exit(1);
}
