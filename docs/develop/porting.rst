.. _porting_to_a_board:

Porting MicroPython
===================

The MicroPython project contains several ports to different microcontroller families and
architectures. The project repository has a `ports <https://github.com/micropython/micropython/tree/master/ports>`_
directory containing a subdirectory for each supported port.

A port will typically contain definitions for multiple "boards", each of which is a specific piece of
hardware that that port can run on, e.g. a development kit or device.

The `minimal port <https://github.com/micropython/micropython/tree/master/ports/minimal>`_ is
available as a simplified reference implementation of a MicroPython port.  It can run on both the
host system and an STM32F4xx MCU.

In general, starting a port requires:

- Setting up the toolchain (configuring Makefiles, etc).
- Implementing boot configuration and CPU initialization.
- Initialising basic drivers required for development and debugging (e.g. GPIO, UART).
- Performing the board-specific configurations.
- Implementing the port-specific modules.

Minimal MicroPython firmware
----------------------------

The best way to start porting MicroPython to a new board is by integrating a minimal
MicroPython interpreter.  For this walkthrough, create a subdirectory for the new
port in the ``ports`` directory:

.. code-block:: bash

   $ cd ports
   $ mkdir example_port

The basic MicroPython firmware is implemented in the main port file, e.g ``main.c``:

.. code-block:: c

   #include "py/compile.h"
   #include "py/gc.h"
   #include "py/mperrno.h"
   #include "py/stackctrl.h"
   #include "shared/runtime/gchelper.h"
   #include "shared/runtime/pyexec.h"

   // Allocate memory for the MicroPython GC heap.
   static char heap[4096];

   int main(int argc, char **argv) {
       // Initialise the MicroPython runtime.
       mp_stack_ctrl_init();
       gc_init(heap, heap + sizeof(heap));
       mp_init();

       // Start a normal REPL; will exit when ctrl-D is entered on a blank line.
       pyexec_friendly_repl();

       // Deinitialise the runtime.
       gc_sweep_all();
       mp_deinit();
       return 0;
   }

   // Handle uncaught exceptions (should never be reached in a correct C implementation).
   void nlr_jump_fail(void *val) {
       for (;;) {
       }
   }

   // Do a garbage collection cycle.
   void gc_collect(void) {
       gc_collect_start();
       gc_helper_collect_regs_and_stack();
       gc_collect_end();
   }

   // There is no filesystem so stat'ing returns nothing.
   mp_import_stat_t mp_import_stat(const char *path) {
       return MP_IMPORT_STAT_NO_EXIST;
   }

   // There is no filesystem so opening a file raises an exception.
   mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
       mp_raise_OSError(MP_ENOENT);
   }

We also need a Makefile at this point for the port:

.. code-block:: Makefile

   # Include the core environment definitions; this will set $(TOP).
   include ../../py/mkenv.mk

   # Include py core make definitions.
   include $(TOP)/py/py.mk
   include $(TOP)/extmod/extmod.mk

   # Set CFLAGS and libraries.
   CFLAGS += -I. -I$(BUILD) -I$(TOP)
   LIBS += -lm

   # Define the required source files.
   SRC_C = \
       main.c \
       mphalport.c \
       shared/readline/readline.c \
       shared/runtime/gchelper_generic.c \
       shared/runtime/pyexec.c \
       shared/runtime/stdout_helpers.c \

   # Define the required object files.
   OBJ = $(PY_CORE_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

   # Define the top-level target, the main firmware.
   all: $(BUILD)/firmware.elf

   # Define how to build the firmware.
   $(BUILD)/firmware.elf: $(OBJ)
       $(ECHO) "LINK $@"
       $(Q)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
       $(Q)$(SIZE) $@

   # Include remaining core make rules.
   include $(TOP)/py/mkrules.mk

Remember to use proper tabs to indent the Makefile.

MicroPython Configurations
--------------------------

After integrating the minimal code above, the next step is to create the MicroPython
configuration files for the port. The compile-time configurations are specified in
``mpconfigport.h`` and additional hardware-abstraction functions, such as time keeping,
in ``mphalport.h``.

The following is an example of an ``mpconfigport.h`` file:

.. code-block:: c

   #include <stdint.h>

   // Python internal features.
   #define MICROPY_ENABLE_GC                       (1)
   #define MICROPY_HELPER_REPL                     (1)
   #define MICROPY_ERROR_REPORTING                 (MICROPY_ERROR_REPORTING_TERSE)
   #define MICROPY_FLOAT_IMPL                      (MICROPY_FLOAT_IMPL_FLOAT)

   // Enable u-modules to be imported with their standard name, like sys.
   #define MICROPY_MODULE_WEAK_LINKS               (1)

   // Fine control over Python builtins, classes, modules, etc.
   #define MICROPY_PY_ASYNC_AWAIT                  (0)
   #define MICROPY_PY_BUILTINS_SET                 (0)
   #define MICROPY_PY_ATTRTUPLE                    (0)
   #define MICROPY_PY_COLLECTIONS                  (0)
   #define MICROPY_PY_MATH                         (0)
   #define MICROPY_PY_IO                           (0)
   #define MICROPY_PY_STRUCT                       (0)

   // Type definitions for the specific machine.

   typedef intptr_t mp_int_t; // must be pointer size
   typedef uintptr_t mp_uint_t; // must be pointer size
   typedef long mp_off_t;

   // We need to provide a declaration/definition of alloca().
   #include <alloca.h>

   // Define the port's name and hardware.
   #define MICROPY_HW_BOARD_NAME "example-board"
   #define MICROPY_HW_MCU_NAME   "unknown-cpu"

   #define MP_STATE_PORT MP_STATE_VM

This configuration file contains machine-specific configurations including aspects like if different
MicroPython features should be enabled e.g. ``#define MICROPY_ENABLE_GC (1)``. Making this Setting
``(0)`` disables the feature.

Other configurations include type definitions, root pointers, board name, microcontroller name
etc.

Similarly, an minimal example ``mphalport.h`` file looks like this:

.. code-block:: c

   static inline void mp_hal_set_interrupt_char(char c) {}

Support for standard input/output
---------------------------------

MicroPython requires at least a way to output characters, and to have a REPL it also
requires a way to input characters. Functions for this can be implemented in the file
``mphalport.c``, for example:

.. code-block:: c

   #include <unistd.h>
   #include "py/mpconfig.h"

   // Receive single character, blocking until one is available.
   int mp_hal_stdin_rx_chr(void) {
       unsigned char c = 0;
       int r = read(STDIN_FILENO, &c, 1);
       (void)r;
       return c;
   }

   // Send the string of given length.
   void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
       int r = write(STDOUT_FILENO, str, len);
       (void)r;
   }

These input and output functions have to be modified depending on the
specific board API. This example uses the standard input/output stream.

Building and running
--------------------

At this stage the directory of the new port should contain::

    ports/example_port/
    ├── main.c
    ├── Makefile
    ├── mpconfigport.h
    ├── mphalport.c
    └── mphalport.h

The port can now be built by running ``make`` (or otherwise, depending on your system).

If you are using the default compiler settings in the Makefile given above then this
will create an executable called ``build/firmware.elf`` which can be executed directly.
To get a functional REPL you may need to first configure the terminal to raw mode:

.. code-block:: bash

   $ stty raw opost -echo
   $ ./build/firmware.elf

That should give a MicroPython REPL.  You can then run commands like:

.. code-block:: bash

   MicroPython v1.13 on 2021-01-01; example-board with unknown-cpu
   >>> import sys
   >>> sys.implementation
   ('micropython', (1, 13, 0))
   >>>

Use Ctrl-D to exit, and then run ``reset`` to reset the terminal.

Adding a module to the port
---------------------------

To add a custom module like ``myport``, first add the module definition in a file
``modmyport.c``:

.. code-block:: c

   #include "py/runtime.h"

   STATIC mp_obj_t myport_info(void) {
       mp_printf(&mp_plat_print, "info about my port\n");
       return mp_const_none;
   }
   STATIC MP_DEFINE_CONST_FUN_OBJ_0(myport_info_obj, myport_info);

   STATIC const mp_rom_map_elem_t myport_module_globals_table[] = {
       { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_myport) },
       { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&myport_info_obj) },
   };
   STATIC MP_DEFINE_CONST_DICT(myport_module_globals, myport_module_globals_table);

   const mp_obj_module_t myport_module = {
       .base = { &mp_type_module },
       .globals = (mp_obj_dict_t *)&myport_module_globals,
   };

   MP_REGISTER_MODULE(MP_QSTR_myport, myport_module);

You will also need to edit the Makefile to add ``modmyport.c`` to the ``SRC_C`` list, and
a new line adding the same file to ``SRC_QSTR`` (so qstrs are searched for in this new file),
like this:

.. code-block:: Makefile

   SRC_C = \
       main.c \
       modmyport.c \
       mphalport.c \
       ...

   SRC_QSTR += modmyport.c

If all went correctly then, after rebuilding, you should be able to import the new module:

.. code-block:: bash

    >>> import myport
    >>> myport.info()
    info about my port
    >>>
