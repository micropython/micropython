/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2022-2023 Damien P. George
 */

#include "port/micropython_embed.h"
#include "py/stackctrl.h"

// This is example 1 script, which will be compiled and executed.
static const char *example_1 =
    "print('hello world!', list(x + 1 for x in range(10)), end='eol\\n')";

// This is example 2 script, which will be compiled and executed.
static const char *example_2 =
    "for i in range(10):\n"
    "    print('iter {:08}'.format(i))\n"
    "\n"
    "try:\n"
    "    1//0\n"
    "except Exception as er:\n"
    "    print('caught exception', repr(er))\n"
    "\n"
    "import gc\n"
    "print('run GC collect')\n"
    "gc.collect()\n"
    "\n"
    "print('help(\\'modules\\'):')\n"
    "help('modules')\n"
    "import sys\n"
    "help(sys)\n"
    "import os\n"
    "help(os)\n"
    "print('os.uname():', os.uname())\n"
    "import random\n"
    "help(random)\n"
    "import time\n"
    "help(time)\n"
    "import frozenhello\n"
    "help(frozenhello)\n"
    "print('frozenhello.hello():', frozenhello.hello())\n"
    "import c_hello\n"
    "help(c_hello)\n"
    "print('c_hello.hello():', c_hello.hello())\n"
    "\n"
    "print('finish')\n"
    ;

// This array is the MicroPython GC heap.
static char heap[8 * 1024];

int main() {
#if MICROPY_STACK_CHECK
    // Set the stack limit, otherwise the default is zero and we will end up in
    // nlr_jump_fail() immediately.
    mp_stack_set_limit(10240);
#endif
    // Initialise MicroPython.
    //
    // Note: &stack_top below should be good enough for many cases.
    // However, depending on environment, there might be more appropriate
    // ways to get the stack top value.
    // eg. pthread_get_stackaddr_np, pthread_getattr_np,
    // __builtin_frame_address/__builtin_stack_address, etc.
    int stack_top;
    mp_embed_init(&heap[0], sizeof(heap), &stack_top);

    // Run the example scripts (they will be compiled first).
    mp_embed_exec_str(example_1);
    mp_embed_exec_str(example_2);

    // Deinitialise MicroPython.
    mp_embed_deinit();

    return 0;
}
