/**
 * @file main.c
 *
 * @section License
 * Copyright (C) 2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Pumbaa project.
 */

#include "pumbaa.h"

extern char *stack_top_p;
static char heap[CONFIG_PUMBAA_HEAP_SIZE];

/**
 * Print a message after a script has been executed.
 */
static void print_exit_message(int res, const char *prefix_p)
{
    if (res == 1) {
        std_printf(FSTR("%s exited normally.\r\n\r\n"), prefix_p);
    } else if (res & PYEXEC_FORCED_EXIT) {
        std_printf(FSTR("%s forced exit.\r\n\r\n"), prefix_p);
    } else {
        std_printf(FSTR("%s exited abnormally.\r\n\r\n"), prefix_p);
    }
}

/**
 * The entry function for a Pumbaa application.
 */
int main()
{
    int stack_dummy;
    int res;

    /* Start the system. */
    sys_start();

    std_printf(sys_get_info());
    std_printf(FSTR("\r\n"));

    /* Initialize the thread module. */
#if MICROPY_PY_THREAD == 1
    module_thread_init();
#endif
    
    stack_top_p = (char*)&stack_dummy;
    mp_stack_set_limit(40000 * (BYTES_PER_WORD / 4));
    gc_init(heap, heap + sizeof(heap));
    mp_init();

    /* Initialize the keyboard interrupt object. */
    MP_STATE_VM(keyboard_interrupt_obj) =
        mp_obj_new_exception(&mp_type_KeyboardInterrupt);

    /* Initialize sys.path and sys.argv. */
    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_path), 0);
    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_argv), 0);

    /* 1. Execute the file main.py. */
    std_printf(FSTR("Executing file 'main.py'.\r\n"));
    res = pyexec_file("main.py");
    print_exit_message(res, "File 'main.py'");

    /* 2. Execute the frozen module main.py. */
    std_printf(FSTR("Executing frozen module 'main.py'.\r\n"));
    res = pyexec_frozen_module("main.py");
    print_exit_message(res, "Frozen module 'main.py'");

#if CONFIG_PUMBAA_MAIN_FRIENDLY_REPL == 1
    /* 3. Execute the interactive shell. */
    res = pyexec_friendly_repl();
    print_exit_message(res, "Interactive shell");
#endif

    return (res != 1);
}
