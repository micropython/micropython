.. _porting_to_a_board:

Porting MicroPython
====================

The MicroPython project contains several ports to different microcontroller families and
architectures. The project repository has a `ports <https://github.com/micropython/micropython/tree/master/ports>`_
directory containing a subdirectory for each supported port.

A port will typically contain definitions for multiple "boards", each of which is a specific piece of
hardware that that port can run on (i.e. a development kit or device).

The
`minimal port <https://github.com/micropython/micropython/tree/master/ports/minimal>`_ is 
available as a simplified reference implementation of a MicroPython port. 
In general, starting a port requires:

- Setting up the toolchain (configuring Makefiles, etc)
- Implementing boot configuration and CPU initialization
- Initialising basic drivers required for development and debugging (e.g. GPIO, UART).
- Performing the board-specific configurations.
- Implementing the port-specific modules.


The minimal MicroPython firmware
---------------------------------

The start of porting MicroPython to a new board is by integrating the minimal firmware.
For this walkthrough, create a subdirectory for the new port in the ``ports`` directory:

.. code-block:: bash

   cd ports
   mkdir example_port

The minimal MicroPython firmware specifications are implemented in the main port file i.e ``main.c``:

.. code-block:: c

    #include "py/compile.h"
    #include "py/runtime.h"
    #include "py/repl.h"
    #include "py/gc.h"
    #include "lib/utils/pyexec.h"

    #include <stdio.h>

    static char heap[2048];

    int main (int argc, char **argv) {
        gc_init(heap, heap + sizeof(heap));

        mp_init();

        pyexec_friendly_repl();

        mp_deinit();
        return 0;
    }
    

    void nlr_jump_fail(void *val) {
        while (1) {
            ;
        }
    }

    void NORETURN __fatal_error(const char *msg) {
        while (1) {
            ;
        }
    }

    mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
        mp_raise_OSError(MP_ENOENT);
    }

    mp_import_stat_t mp_import_stat(const char *path) {
        return MP_IMPORT_STAT_NO_EXIST;
    }

    mp_obj_t mp_builtin_open(uint n_args, const mp_obj_t *args, mp_map_t *kwargs) {
        return mp_const_none;
    }
    MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);


We also need a makefile at this point for the port:

.. code-block:: Makefile

    include ../../py/mkenv.mk

    CROSS = 0

    # Include py core make definitions
    include $(TOP)/py/py.mk

    LIBS =

    SRC_C = \
        main.c \

    $(BUILD)/firmware.elf: $(OBJ)
        $(ECHO) "LINK $@"
        $(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
        $(Q)$(SIZE) $@

    $(BUILD)/firmware.bin: $(BUILD)/firmware.elf
        $(Q)$(OBJCOPY) -O binary $^ $@

    $(BUILD)/firmware.hex: $(BUILD)/firmware.elf
        $(Q)$(OBJCOPY) -O ihex -R .eeprom $< $@

    include $(TOP)/py/mkrules.mk

MicroPython Configurations
---------------------------

After integrating the minimal firmware, the next step is to create the MicroPython
configurations files for the port. The configurations are implemented in files named
``mpconfigport.h`` and ``mphalport.h``.

The following is an example of a ``mpconfigport.h`` file:

.. code-block:: c

    #include <stdint.h>

    #define MICROPY_QSTR_BYTES_IN_HASH  (1)
    #define MICROPY_QSTR_EXTRA_POOL     mp_qstr_frozen_const_pool
    #define MICROPY_ALLOC_PATH_MAX      (256)
    #define MICROPY_ALLOC_PARSE_CHUNK_INIT (16)
    #define MICROPY_EMIT_X64            (0)
    #define MICROPY_EMIT_THUMB          (0)
    #define MICROPY_EMIT_INLINE_THUMB   (0)
    #define MICROPY_COMP_MODULE_CONST   (0)
    #define MICROPY_COMP_CONST          (0)
    #define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (0)
    #define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (0)
    #define MICROPY_MEM_STATS           (0)
    #define MICROPY_DEBUG_PRINTERS      (0)
    #define MICROPY_ENABLE_GC           (1)
    #define MICROPY_GC_ALLOC_THRESHOLD  (0)
    #define MICROPY_REPL_EVENT_DRIVEN   (0)
    #define MICROPY_HELPER_REPL         (1)
    #define MICROPY_HELPER_LEXER_UNIX   (0)
    #define MICROPY_ENABLE_SOURCE_LINE  (0)
    #define MICROPY_ENABLE_DOC_STRING   (0)
    #define MICROPY_ERROR_REPORTING     (MICROPY_ERROR_REPORTING_TERSE)
    #define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG (0)
    #define MICROPY_PY_ASYNC_AWAIT      (0)
    #define MICROPY_PY_BUILTINS_BYTEARRAY (1)
    #define MICROPY_PY_BUILTINS_MEMORYVIEW (0)
    #define MICROPY_PY_BUILTINS_ENUMERATE (0)
    #define MICROPY_PY_BUILTINS_FILTER  (0)
    #define MICROPY_PY_BUILTINS_FROZENSET (0)
    #define MICROPY_PY_BUILTINS_REVERSED (0)
    #define MICROPY_PY_BUILTINS_SET     (0)
    #define MICROPY_PY_BUILTINS_SLICE   (0)
    #define MICROPY_PY_BUILTINS_PROPERTY (0)
    #define MICROPY_PY_BUILTINS_MIN_MAX (0)
    #define MICROPY_PY___FILE__         (0)
    #define MICROPY_PY_GC               (0)
    #define MICROPY_PY_ARRAY            (1)
    #define MICROPY_PY_ATTRTUPLE        (0)
    #define MICROPY_PY_COLLECTIONS      (0)
    #define MICROPY_PY_MATH             (0)
    #define MICROPY_PY_CMATH            (0)
    #define MICROPY_PY_IO               (0)
    #define MICROPY_PY_STRUCT           (0)
    #define MICROPY_PY_SYS              (0)
    #define MICROPY_MODULE_FROZEN_MPY   (1)
    #define MICROPY_CPYTHON_COMPAT      (0)
    #define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_NONE)
    #define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)

    // type definitions for the specific machine
    #define BYTES_PER_WORD (4)

    #define MICROPY_MAKE_POINTER_CALLABLE(p) ((void*)((mp_uint_t)(p) | 1))

    // This port is intended to be 32-bit, but unfortunately, int32_t for
    // different targets may be defined in different ways - either as int
    // or as long. This requires different printf formatting specifiers
    // to print such value. So, we avoid int32_t and use int directly.
    #define UINT_FMT "%u"
    #define INT_FMT "%d"
    typedef int mp_int_t; // must be pointer size
    typedef unsigned mp_uint_t; // must be pointer size

    typedef long mp_off_t;

    #define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

    // extra built in names to add to the global namespace
    #define MICROPY_PORT_BUILTINS \
        { MP_OBJ_NEW_QSTR(MP_QSTR_open), (mp_obj_t)&mp_builtin_open_obj },

    // We need to provide a declaration/definition of alloca()
    #include <alloca.h>

    #define MICROPY_HW_BOARD_NAME "example-board""
    #define MICROPY_HW_MCU_NAME   "unknown-cpu"

    #ifdef __linux__
    #define MICROPY_MIN_USE_STDOUT (1)
    #endif

    #ifdef __thumb__
    #define MICROPY_MIN_USE_CORTEX_CPU (1)
    #define MICROPY_MIN_USE_STM32_MCU (1)
    #endif

    #define MP_STATE_PORT MP_STATE_VM

    #define MICROPY_PORT_ROOT_POINTERS \
        const char *readline_hist[8];

    #endif

This configuration file contains machine-specific configurations including aspects like if different
MicroPython features should be enabled i.e ``#define MICROPY_ENABLE_GC (1) ``. Making this Setting
``0`` disables the feature.

Other configurations include type definitions, root pointers, board name, microcontroller name
etc.

Similarly, an example ``mphalport.h`` file looks like this:

.. code-block:: c

    static inline mp_uint_t mp_hal_ticks_ms(void) { return 0; }
    static inline void mp_hal_set_interrupt_char(char c) {}

Support for standard input/output
----------------------------------

Now add a file ``uart_core.c`` to allow for input and output:

.. code-block:: c

    #include <unistd.h>
    #include "py/mpconfig.h"


    // Receive single character
    int mp_hal_stdin_rx_chr(void) {
        unsigned char c = 0;
        #if MICROPY_MIN_USE_STDOUT
        int r = read(STDIN_FILENO, &c, 1);
        (void)r;
        #endif
        return c;
    }

    // Send string of given length
    void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
        #if MICROPY_MIN_USE_STDOUT
        int r = write(STDOUT_FILENO, str, len);
        (void)r;
        #endif
    }

These input and output functions have to be modified depending on the
specific board API. This example uses the standard input/output stream.

Adding a module to a port
--------------------------

To support an existing MicroPython module like ``pyb``, first, add the module
definition in a file ``modpyb.c``:

.. code-block:: c

    #include "py/mpconfig.h"
    #include "py/obj.h"

    STATIC const mp_map_elem_t pyb_module_globals_table[] = {
        { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_pyb) },
    };

    STATIC MP_DEFINE_CONST_DICT(pyb_module_globals, pyb_module_globals_table);

    const mp_obj_module_t pyb_module = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t*)&pyb_module_globals,
    };

Modify the configuration file ``mpconfigport.h`` accordingly:

.. code-block:: c

    // extra built-in modules to add to the list of known ones
    extern const struct _mp_obj_module_t pyb_module;

    #define MICROPY_PORT_BUILTIN_MODULES \
        { MP_OBJ_NEW_QSTR(MP_QSTR_pyb), (mp_obj_t)&pyb_module },

If you ported correctly, then you should have the standard command-line interpreter:

.. code-block:: bash

    >>> 3
    3
    >>> print(“Hello World!”)
    Hello World!
    >>> import pyb
    >>> 
