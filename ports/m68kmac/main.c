#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "Multiverse.h"
#include "macutil.h"
#include "uart_core.h"

#include "py/builtin.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/stackctrl.h"
#include "shared/runtime/pyexec.h"
#include "shared/runtime/gchelper.h"
#include "shared/readline/readline.h"
#include "shared/runtime/interrupt_char.h"
#include "extmod/vfs.h"
#include "extmod/vfs_posix.h"

#if MICROPY_ENABLE_COMPILER
static int execute_from_lexer(mp_obj_t source, mp_parse_input_kind_t input_kind, bool is_repl) {
    mp_hal_set_interrupt_char(CHAR_CTRL_C);

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // create lexer based on source kind
        mp_lexer_t *lex;
        lex = mp_lexer_new_from_file(mp_obj_str_get_qstr(source));
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, is_repl);
        mp_call_function_0(module_fun);
        mp_hal_set_interrupt_char(-1);
        mp_handle_pending(true);
        nlr_pop();
        return 0;
    } else {
        // uncaught exception
        mp_hal_set_interrupt_char(-1);
        mp_handle_pending(false);
        mp_obj_base_t *exc = nlr.ret_val;
        mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(exc));
        (void)mp_hal_stdin_rx_chr();
        return 1;
    }
    mp_printf(&mp_plat_print, "Press any key to exit");
}
#endif

void LMSetApplLimit_checked(Ptr val) {
    if (val > LMGetHeapEnd()) {
        LMSetApplLimit(val);
    }
}

int main(int argc, char **argv) {
    // Enlarge stack to at least 32kB
    LMSetApplLimit_checked(LMGetCurStackBase() - 32768);

    // Set MP stack limits, reserving 1kB stack for internal use
    mp_stack_set_top(LMGetCurStackBase());
    mp_stack_set_limit(LMGetApplLimit() + 1024);

    // Use half the largest possible block as MicroPython heap
    Size growmax;
    (void)MaxMem(&growmax);
    size_t heap_size = growmax / 2;
    unsigned char *heap = (unsigned char *)NewPtr(growmax / 2);

    // Check if we were given a file ...
    short message, count;
    CountAppFiles(&message, &count);
    AppFile file = {};
    if (count) {
        GetAppFiles(1, &file);
    }

    gc_init(heap, heap + heap_size);
    mp_init();

    {
        // Mount the host FS at the root of our internal VFS
        mp_obj_t args[2] = {
            MP_OBJ_TYPE_GET_SLOT(&mp_type_vfs_mac, make_new)(&mp_type_vfs_mac, 0, 0, NULL),
            MP_OBJ_NEW_QSTR(MP_QSTR__slash_),
        };
        mp_vfs_mount(2, args, (mp_map_t *)&mp_const_empty_map);

        // Make sure the root that was just mounted is the current VFS (it's always at
        // the end of the linked list).  Can't use chdir('/') because that will change
        // the current path within the VfsPosix object.
        MP_STATE_VM(vfs_cur) = MP_STATE_VM(vfs_mount_table);
        while (MP_STATE_VM(vfs_cur)->next != NULL) {
            MP_STATE_VM(vfs_cur) = MP_STATE_VM(vfs_cur)->next;
        }
    }

    if (count) {
        mp_obj_t filename = new_str_from_pstr(file.fName);
        execute_from_lexer(filename, MP_PARSE_FILE_INPUT, false);
    } else {
        #if MICROPY_REPL_EVENT_DRIVEN
        pyexec_event_repl_init();
        for (;;) {
            int c = mp_hal_stdin_rx_chr();
            if (pyexec_event_repl_process_char(c)) {
                break;
            }
        }
        #else
        pyexec_friendly_repl();
        #endif
    }
    mp_deinit();
    return 0;
}

void MP_NORETURN __fatal_error(const char *msg);

void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    #if MICROPY_PY_THREAD
    mp_thread_gc_others();
    #endif
    gc_collect_end();
}

void nlr_jump_fail(void *val) {
    __fatal_error("nlr jump fail");
}

void MP_NORETURN __fatal_error(const char *msg) {
    mp_printf(&mp_plat_print, "Fatal: %s\n", msg);
    while (1) {
        ;
    }
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif

#define TICK_TO_MS(x) ((x + 2) * 50 / 3)
#define TICK_TO_US(x) ((x + 2) * 50000 / 3)
#define MS_TO_TICK(x) ((x + 25) * 3 / 50)
#define US_TO_TICK(x) ((x + 25000) * 3 / 50000)

void mp_hal_delay_ms(mp_uint_t delay) {
    long unused;
    Delay(MS_TO_TICK(delay), &unused);
}

void mp_hal_delay_us(mp_uint_t delay) {
    long unused;
    Delay(US_TO_TICK(delay), &unused);
}

mp_uint_t mp_hal_ticks_ms(void) {
    return TICK_TO_MS(TickCount());
}

mp_uint_t mp_hal_ticks_us(void) {
    return TICK_TO_US(TickCount());
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return 0;
}
