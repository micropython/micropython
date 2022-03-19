/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014-2017 Paul Sokolovsky
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
#include <signal.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/stackctrl.h"
#include "py/mphal.h"
#include "py/mpthread.h"
#include "extmod/misc.h"
#include "extmod/moduplatform.h"
#include "extmod/vfs.h"
#include "extmod/vfs_posix.h"
#include "genhdr/mpversion.h"
#include "input.h"

// Command line options, with their defaults
STATIC uint emit_opt = MP_EMIT_OPT_NONE;

#if MICROPY_ENABLE_GC
// Heap size of GC heap (if enabled)
// Make it larger on a 64 bit machine, because pointers are larger.
long heap_size = 1024 * 1024 * (sizeof(mp_uint_t) / 4);
#endif

STATIC void stderr_print_strn(void *env, const char *str, size_t len) {
    (void)env;
    ssize_t ret;
    MP_HAL_RETRY_SYSCALL(ret, write(STDERR_FILENO, str, len), {});
    mp_uos_dupterm_tx_strn(str, len);
}

const mp_print_t mp_stderr_print = {NULL, stderr_print_strn};

#define FORCED_EXIT (0x100)
// If exc is SystemExit, return value where FORCED_EXIT bit set,
// and lower 8 bits are SystemExit value. For all other exceptions,
// return 1.
STATIC int handle_uncaught_exception(mp_obj_base_t *exc) {
    // check for SystemExit
    if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(exc->type), MP_OBJ_FROM_PTR(&mp_type_SystemExit))) {
        // None is an exit value of 0; an int is its value; anything else is 1
        mp_obj_t exit_val = mp_obj_exception_get_value(MP_OBJ_FROM_PTR(exc));
        mp_int_t val = 0;
        if (exit_val != mp_const_none && !mp_obj_get_int_maybe(exit_val, &val)) {
            val = 1;
        }
        return FORCED_EXIT | (val & 255);
    }

    // Report all other exceptions
    mp_obj_print_exception(&mp_stderr_print, MP_OBJ_FROM_PTR(exc));
    return 1;
}

#if MICROPY_USE_READLINE == 1
#include "shared/readline/readline.h"
#else
STATIC char *strjoin(const char *s1, int sep_char, const char *s2) {
    int l1 = strlen(s1);
    int l2 = strlen(s2);
    char *s = malloc(l1 + l2 + 2);
    memcpy(s, s1, l1);
    if (sep_char != 0) {
        s[l1] = sep_char;
        l1 += 1;
    }
    memcpy(s + l1, s2, l2);
    s[l1 + l2] = 0;
    return s;
}
#endif

STATIC void set_sys_argv(char *argv[], int argc, int start_arg) {
    for (int i = start_arg; i < argc; i++) {
        mp_obj_list_append(mp_sys_argv, MP_OBJ_NEW_QSTR(qstr_from_str(argv[i])));
    }
}

#ifdef _WIN32
#define PATHLIST_SEP_CHAR ';'
#else
#define PATHLIST_SEP_CHAR ':'
#endif

MP_NOINLINE int main_(int argc, char **argv);

int main(int argc, char **argv) {
    #if MICROPY_PY_THREAD
    mp_thread_init();
    #endif
    // We should capture stack top ASAP after start, and it should be
    // captured guaranteedly before any other stack variables are allocated.
    // For this, actual main (renamed main_) should not be inlined into
    // this function. main_() itself may have other functions inlined (with
    // their own stack variables), that's why we need this main/main_ split.
    mp_stack_ctrl_init();
    return main_(argc, argv);
}

// Similar to 'pyhton -m $(PYPROJ_LIBMAIN), but seperate from other command line parsing'
int call_libmain(char *libname, int argc, char **argv)
{
    mp_obj_t import_args[4];
    import_args[0] = mp_obj_new_str(libname, strlen(libname));
    import_args[1] = import_args[2] = mp_const_none;
    // Ask __import__ to handle imported module specially - set its __name__
    // to __main__, and also return this leaf module, not top-level package
    // containing it.
    import_args[3] = mp_const_false;

    // Start arg will be the first argument after the binary name
    set_sys_argv(argv, argc, 1);

    mp_obj_t mod;
    nlr_buf_t nlr;

    // Allocating subpkg_tried on the stack can lead to compiler warnings about this
    // variable being clobbered when nlr is implemented using setjmp/longjmp.  Its
    // value must be preserved across calls to setjmp/longjmp.
    static bool subpkg_tried;
    subpkg_tried = false;

reimport:
    if (nlr_push(&nlr) == 0) {
        mod = mp_builtin___import__(MP_ARRAY_SIZE(import_args), import_args);
        nlr_pop();
    } else {
        // uncaught exception
        return handle_uncaught_exception(nlr.ret_val) & 0xff;
    }

    if (mp_obj_is_package(mod) && !subpkg_tried) {
        subpkg_tried = true;
        vstr_t vstr;
        int len = strlen(libname);
        vstr_init(&vstr, len + sizeof(".__main__"));
        vstr_add_strn(&vstr, libname, len);
        vstr_add_strn(&vstr, ".__main__", sizeof(".__main__") - 1);
        import_args[0] = mp_obj_new_str_from_vstr(&mp_type_str, &vstr);
        goto reimport;
    }

    // All good :)
    return 0;
}

MP_NOINLINE int main_(int argc, char **argv) {
    #ifdef SIGPIPE
    // Do not raise SIGPIPE, instead return EPIPE. Otherwise, e.g. writing
    // to peer-closed socket will lead to sudden termination of MicroPython
    // process. SIGPIPE is particularly nasty, because unix shell doesn't
    // print anything for it, so the above looks like completely sudden and
    // silent termination for unknown reason. Ignoring SIGPIPE is also what
    // CPython does. Note that this may lead to problems using MicroPython
    // scripts as pipe filters, but again, that's what CPython does. So,
    // scripts which want to follow unix shell pipe semantics (where SIGPIPE
    // means "pipe was requested to terminate, it's not an error"), should
    // catch EPIPE themselves.
    signal(SIGPIPE, SIG_IGN);
    #endif

    // Define a reasonable stack limit to detect stack overflow.
    mp_uint_t stack_limit = 40000 * (sizeof(void *) / 4);
    #if defined(__arm__) && !defined(__thumb2__)
    // ARM (non-Thumb) architectures require more stack.
    stack_limit *= 2;
    #endif
    mp_stack_set_limit(stack_limit);

    #if MICROPY_ENABLE_GC
    char *heap = malloc(heap_size);
    gc_init(heap, heap + heap_size);
    #endif

    #if MICROPY_ENABLE_PYSTACK
    static mp_obj_t pystack[1024];
    mp_pystack_init(pystack, &pystack[MP_ARRAY_SIZE(pystack)]);
    #endif

    mp_init();

    #if MICROPY_EMIT_NATIVE
    // Set default emitter options
    MP_STATE_VM(default_emit_opt) = emit_opt;
    #else
    (void)emit_opt;
    #endif

    #if MICROPY_VFS_POSIX
    {
        // Mount the host FS at the root of our internal VFS
        mp_obj_t args[2] = {
            mp_type_vfs_posix.make_new(&mp_type_vfs_posix, 0, 0, NULL),
            MP_OBJ_NEW_QSTR(MP_QSTR__slash_),
        };
        mp_vfs_mount(2, args, (mp_map_t *)&mp_const_empty_map);
        MP_STATE_VM(vfs_cur) = MP_STATE_VM(vfs_mount_table);
    }
    #endif

    char *home = getenv("HOME");
    char *path = getenv("MICROPYPATH");
    if (path == NULL) {
        path = MICROPY_PY_SYS_PATH_DEFAULT;
    }
    size_t path_num = 1; // [0] is for current dir (or base dir of the script)
    if (*path == PATHLIST_SEP_CHAR) {
        path_num++;
    }
    for (char *p = path; p != NULL; p = strchr(p, PATHLIST_SEP_CHAR)) {
        path_num++;
        if (p != NULL) {
            p++;
        }
    }
    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_path), path_num);
    mp_obj_t *path_items;
    mp_obj_list_get(mp_sys_path, &path_num, &path_items);
    path_items[0] = MP_OBJ_NEW_QSTR(MP_QSTR_);
    {
        char *p = path;
        for (mp_uint_t i = 1; i < path_num; i++) {
            char *p1 = strchr(p, PATHLIST_SEP_CHAR);
            if (p1 == NULL) {
                p1 = p + strlen(p);
            }
            if (p[0] == '~' && p[1] == '/' && home != NULL) {
                // Expand standalone ~ to $HOME
                int home_l = strlen(home);
                vstr_t vstr;
                vstr_init(&vstr, home_l + (p1 - p - 1) + 1);
                vstr_add_strn(&vstr, home, home_l);
                vstr_add_strn(&vstr, p + 1, p1 - p - 1);
                path_items[i] = mp_obj_new_str_from_vstr(&mp_type_str, &vstr);
            } else {
                path_items[i] = mp_obj_new_str_via_qstr(p, p1 - p);
            }
            p = p1 + 1;
        }
    }

    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_argv), 0);

    #if defined(MICROPY_UNIX_COVERAGE)
    {
        MP_DECLARE_CONST_FUN_OBJ_0(extra_coverage_obj);
        MP_DECLARE_CONST_FUN_OBJ_0(extra_cpp_coverage_obj);
        mp_store_global(MP_QSTR_extra_coverage, MP_OBJ_FROM_PTR(&extra_coverage_obj));
        mp_store_global(MP_QSTR_extra_cpp_coverage, MP_OBJ_FROM_PTR(&extra_cpp_coverage_obj));
    }
    #endif

    // #--------------------------------------------#
    // This is where our code is called
    int ret = call_libmain(PYPROJ_LIBMAIN, argc, argv);
    // #--------------------------------------------#

    #if MICROPY_PY_SYS_SETTRACE
    MP_STATE_THREAD(prof_trace_callback) = MP_OBJ_NULL;
    #endif

    #if MICROPY_PY_SYS_ATEXIT
    // Beware, the sys.settrace callback should be disabled before running sys.atexit.
    if (mp_obj_is_callable(MP_STATE_VM(sys_exitfunc))) {
        mp_call_function_0(MP_STATE_VM(sys_exitfunc));
    }
    #endif

    #if MICROPY_PY_MICROPYTHON_MEM_INFO
    if (mp_verbose_flag) {
        mp_micropython_mem_info(0, NULL);
    }
    #endif

    #if MICROPY_PY_BLUETOOTH
    void mp_bluetooth_deinit(void);
    mp_bluetooth_deinit();
    #endif

    #if MICROPY_PY_THREAD
    mp_thread_deinit();
    #endif

    #if defined(MICROPY_UNIX_COVERAGE)
    gc_sweep_all();
    #endif

    mp_deinit();

    #if MICROPY_ENABLE_GC && !defined(NDEBUG)
    // We don't really need to free memory since we are about to exit the
    // process, but doing so helps to find memory leaks.
    free(heap);
    #endif

    return ret & 0xff;
}

#if !MICROPY_VFS
uint mp_import_stat(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            return MP_IMPORT_STAT_DIR;
        } else if (S_ISREG(st.st_mode)) {
            return MP_IMPORT_STAT_FILE;
        }
    }
    return MP_IMPORT_STAT_NO_EXIST;
}

#if MICROPY_PY_IO
// Factory function for I/O stream classes, only needed if generic VFS subsystem isn't used.
// Note: buffering and encoding are currently ignored.
mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kwargs) {
    enum { ARG_file, ARG_mode };
    STATIC const mp_arg_t allowed_args[] = {
        { MP_QSTR_file, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mode, MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_QSTR(MP_QSTR_r)} },
        { MP_QSTR_buffering, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_encoding, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kwargs, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    return mp_vfs_posix_file_open(&mp_type_textio, args[ARG_file].u_obj, args[ARG_mode].u_obj);
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);
#endif
#endif

#ifdef DEBUG
#define DEBUG_ERR(...) fprintf(stderr, #__VA_ARGS__);
#else
#define DEBUG_ERR(...)
#endif

void nlr_jump_fail(void *val) {
    DEBUG_ERR("FATAL: uncaught NLR %p\n", val);
    exit(1);
}
