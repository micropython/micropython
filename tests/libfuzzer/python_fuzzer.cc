/*#include <stdint.h>
#include <stdio.h>
#include <string.h>*/


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

extern "C" {
#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/obj.h"
#include "lib/utils/pyexec.h"
#include "py/stackctrl.h"
}


// modelled after micropython/minimal/main.c
// and micropython/unix/main.c
extern "C" int LLVMFuzzerTestOneInput(const unsigned char *data, size_t size) {    
    
    //Initialize stuff (?)
    mp_stack_ctrl_init();

    mp_stack_set_limit(40000 * (BYTES_PER_WORD / 4));

    #if MICROPY_ENABLE_GC
    // Heap size of GC heap (if enabled)
    // Make it larger on a 64 bit machine, because pointers are larger.
    long heap_size = 1024*1024 * (sizeof(mp_uint_t) / 4);
    char *heap = (char*)malloc(heap_size);
    gc_init(heap, heap + heap_size);
    #endif

    mp_init();

    char *fakeargv[] = {"fuzzer", "foo", "bar"};
    mp_obj_list_init((mp_obj_list_t*)MP_OBJ_TO_PTR(mp_sys_argv), 0);
    for (int i = 0; i < 3; i++) {
        mp_obj_list_append(mp_sys_argv, MP_OBJ_NEW_QSTR(qstr_from_str(fakeargv[i])));
    }

    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, (const char*)data, size, 0);

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_FILE_INPUT);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NONE, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        //mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
        mp_deinit();
        #if MICROPY_ENABLE_GC && !defined(NDEBUG)
        free(heap);
        #endif
        return 0;
    }
    mp_deinit();
    #if MICROPY_ENABLE_GC && !defined(NDEBUG)
    free(heap);
    #endif
    return 0;
}
