/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Armink (armink.ztl@gmail.com)
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
#include <rtthread.h>
#ifdef RT_USING_DFS
    #include <dfs_posix.h>
#endif
#include <py/compile.h>
#include <py/runtime.h>
#include <py/repl.h>
#include <py/gc.h>
#include <py/mperrno.h>
#include <py/stackctrl.h>
#include <py/frozenmod.h>
#include <shared/readline/readline.h>
#include <shared/runtime/pyexec.h>
#include "mpgetcharport.h"
#include "mpputsnport.h"

#define THREAD_STACK_NO_SYNC   4096
#define THREAD_STACK_WITH_SYNC 8192

#if MICROPY_ENABLE_COMPILER
void do_str(const char *src, mp_parse_input_kind_t input_kind)
{
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0)
    {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    }
    else
    {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}
#endif

#ifdef RT_USING_DFS
static int mp_sys_resource_bak(struct dfs_fdtable **table_bak)
{
    struct dfs_fdtable *fd_table;
    struct dfs_fdtable *fd_table_bak;
    struct dfs_fd **fds;

    fd_table = dfs_fdtable_get();
    if (!fd_table)
    {
        return RT_FALSE;
    }

    fd_table_bak = (struct dfs_fdtable *)rt_malloc(sizeof(struct dfs_fdtable));
    if (!fd_table_bak)
    {
        goto _exit_tab;
    }

    fds = (struct dfs_fd **)rt_malloc((int)fd_table->maxfd * sizeof(struct dfs_fd *));
    if (!fds)
    {
        goto _exit_fds;
    }
    else
    {
        rt_memcpy(fds, fd_table->fds, (int)fd_table->maxfd * sizeof(struct dfs_fd *));
        fd_table_bak->maxfd = (int)fd_table->maxfd;
        fd_table_bak->fds = fds;
    }

    *table_bak = fd_table_bak;

    return RT_TRUE;

_exit_fds:
    rt_free(fd_table_bak);

_exit_tab:
    return RT_FALSE;
}

static void mp_sys_resource_gc(struct dfs_fdtable *fd_table_bak)
{
    struct dfs_fdtable *fd_table;

    if (!fd_table_bak) return;

    fd_table = dfs_fdtable_get();

    for (int i = 0; i < fd_table->maxfd; i++)
    {
        if (fd_table->fds[i] != RT_NULL)
        {
            if ((i < fd_table_bak->maxfd && fd_table_bak->fds[i] == RT_NULL) || (i >= fd_table_bak->maxfd))
            {
                close(i + DFS_FD_OFFSET);
            }
        }
    }

    rt_free(fd_table_bak->fds);
    rt_free(fd_table_bak);
}
#endif

static void *stack_top = RT_NULL;
static char *heap = RT_NULL;

void mpy_main(const char *filename)
{
    int stack_dummy;
    int stack_size_check;
    stack_top = (void *)&stack_dummy;

#ifdef RT_USING_DFS
    struct dfs_fdtable *fd_table_bak = NULL;
    mp_sys_resource_bak(&fd_table_bak);
#endif

    mp_getchar_init();
    mp_putsn_init();

#if defined(MICROPYTHON_USING_FILE_SYNC_VIA_IDE)
    stack_size_check = THREAD_STACK_WITH_SYNC;
#else
    stack_size_check = THREAD_STACK_NO_SYNC;
#endif

    if (rt_thread_self()->stack_size < stack_size_check)
    {
        mp_printf(&mp_plat_print, "The stack (%.*s) size for executing MicroPython must be >= %d\n", RT_NAME_MAX, rt_thread_self()->name, stack_size_check);
    }

#if MICROPY_PY_THREAD
    mp_thread_init(rt_thread_self()->stack_addr, ((rt_uint32_t)stack_top - (rt_uint32_t)rt_thread_self()->stack_addr) / 4);
#endif

    mp_stack_set_top(stack_top);
    // Make MicroPython's stack limit somewhat smaller than full stack available
    mp_stack_set_limit(rt_thread_self()->stack_size - 1024);

#if MICROPY_ENABLE_GC
    heap = rt_malloc(MICROPY_HEAP_SIZE);
    if (!heap)
    {
        mp_printf(&mp_plat_print, "No memory for MicroPython Heap!\n");
        return;
    }
    gc_init(heap, heap + MICROPY_HEAP_SIZE);
#endif

    /* MicroPython initialization */
    mp_init();

    /* system path initialization */
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    mp_obj_list_append(mp_sys_path, mp_obj_new_str(MICROPY_PY_PATH_FIRST, strlen(MICROPY_PY_PATH_FIRST)));
    mp_obj_list_append(mp_sys_path, mp_obj_new_str(MICROPY_PY_PATH_SECOND, strlen(MICROPY_PY_PATH_SECOND)));
    mp_obj_list_init(mp_sys_argv, 0);
    readline_init0();

    if (filename)
    {
#ifndef MICROPYTHON_USING_UOS
        mp_printf(&mp_plat_print, "Please enable uos module in sys module option first.\n");
#else
        pyexec_file(filename);
#endif
    }
    else
    {
#ifdef MICROPYTHON_USING_UOS
        // run boot-up scripts
        void *frozen_data;
        const char *_boot_file = "_boot.py", *boot_file = "boot.py", *main_file = MICROPY_MAIN_PY_PATH;
        if (mp_find_frozen_module(_boot_file, strlen(_boot_file), &frozen_data) != MP_FROZEN_NONE)
        {
            pyexec_frozen_module(_boot_file);
        }
        if (!access(boot_file, 0))
        {
            pyexec_file(boot_file);
        }
        // run main scripts
        if (!access(main_file, 0))
        {
            if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL)
            {
                pyexec_file(main_file);
            }
        }
#endif /* MICROPYTHON_USING_UOS */

        mp_printf(&mp_plat_print, "\n");
        for (;;)
        {
            if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL)
            {
                if (pyexec_raw_repl() != 0)
                {
                    break;
                }
            }
            else
            {
                if (pyexec_friendly_repl() != 0)
                {
                    break;
                }
            }
        }
    }

    gc_sweep_all();

    mp_deinit();

#if MICROPY_PY_THREAD
    mp_thread_deinit();
#endif

    rt_free(heap);

    mp_putsn_deinit();
    mp_getchar_deinit();

#ifdef RT_USING_DFS
    mp_sys_resource_gc(fd_table_bak);
#endif
}

#if !MICROPY_PY_MODUOS_FILE
mp_import_stat_t mp_import_stat(const char *path)
{
    return MP_IMPORT_STAT_NO_EXIST;
}
#endif

NORETURN void nlr_jump_fail(void *val)
{
    mp_printf(MICROPY_ERROR_PRINTER, "nlr_jump_fail\n");
    while (1);
}

#ifndef NDEBUG
NORETURN void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr)
{
    mp_printf(MICROPY_ERROR_PRINTER, "Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    while (1);
}
#endif

#include <stdarg.h>

int DEBUG_printf(const char *format, ...)
{
    static char log_buf[512];
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    /* must use vprintf to print */
    rt_vsprintf(log_buf, format, args);
    mp_printf(&mp_plat_print, "%s", log_buf);
    va_end(args);

    return 0;
}

#ifndef MICROPYTHON_USING_UOS
mp_lexer_t *mp_lexer_new_from_file(const char *filename)
{
    mp_raise_OSError(MP_ENOENT);
}
#endif

#if defined(RT_USING_FINSH) && defined(FINSH_USING_MSH)
#include <finsh.h>
static void python(uint8_t argc, char **argv)
{
    if (argc > 1)
    {
        mpy_main(argv[1]);
    }
    else
    {
        mpy_main(NULL);
    }
}
MSH_CMD_EXPORT(python, MicroPython: `python [file.py]` execute python script);
#endif /* defined(RT_USING_FINSH) && defined(FINSH_USING_MSH) */
