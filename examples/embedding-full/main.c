/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2022-2023 Damien P. George
 */

#include "port/micropython_embed.h"
#include "py/stackctrl.h"

// This is example 1 script, which will be compiled and executed.
static const char *example_1 =
    "print('hello world!', list(x + 1.5 for x in range(10)), end='eol\\n')";

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
    // Skip testing stdin when using the POSIX implementation, i.e. it is
    // connected to actual stdin, because that makes the program wait for input,
    // which can be inconvenient or confusing (giving the appearance of being
    // stuck). There is no harm enabling it if you remember to provide some
    // input.
#if !MICROPY_VFS_POSIX
    "print('sys.stdin.read(3):', repr(sys.stdin.read(3)))\n"
    "print('sys.stdin.buffer.read(3):', repr(sys.stdin.buffer.read(3)))\n"
#endif
    "sys.stdout.write('hello stdout text\\n')\n"
    "sys.stdout.buffer.write(b'hello stdout binary\\n')\n"
    "sys.stdout.write('hello stderr text\\n')\n"
    "sys.stdout.buffer.write(b'hello stderr binary\\n')\n"
    "import os\n"
    "help(os)\n"
    "print('os.uname():', os.uname())\n"
    "import random\n"
    "help(random)\n"
    "import time\n"
    "help(time)\n"
    "print('time.gmtime(736622952) = 2023-05-05T17:29:12Z:', time.gmtime(736622952))\n"
    "import math\n"
    "help(math)\n"
    "import uctypes\n"
    "help(uctypes)\n"
    "import frozenhello\n"
    "help(frozenhello)\n"
    "print('frozenhello.hello():', frozenhello.hello())\n"
    "import c_hello\n"
    "help(c_hello)\n"
    "print('c_hello.hello():', c_hello.hello())\n"
    "import fsimage, vfs\n"
    "vfs.mount(fsimage.BlockDev(), '/lfs', readonly=True)\n"
    "print('os.listdir(\\'/lfs\\'):', os.listdir('/lfs'))\n"
    "with open('/lfs/lfshello.py', 'rb') as f:\n"
    "    print('lfshello.py:', f.read())\n"
    "sys.path.append('/lfs')\n"
    "import lfshello\n"
    "help(lfshello)\n"
    "print('lfshello.hello():', lfshello.hello())\n"
    "vfs.mount(vfs.VfsPosix('.'), '/posix')\n"
    "print('os.listdir(\\'/posix\\'):', os.listdir('/posix'))\n"
    "with open('/posix/posixhello.py', 'wb') as f:\n"
    "    f.write(b'def hello():\\n    return \"Hello from a POSIX file!\"\\n')\n"
    "with open('/posix/posixhello.py', 'rb') as f:\n"
    "    print('posixhello.py:', f.read())\n"
    "sys.path.append('/posix')\n"
    "import posixhello\n"
    "help(posixhello)\n"
    "print('posixhello.hello():', posixhello.hello())\n"
    "os.unlink('/posix/posixhello.py')"
    "\n"
    "print('finish')\n"
    ;

// This array is the MicroPython GC heap.
static char heap[16 * 1024];

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
