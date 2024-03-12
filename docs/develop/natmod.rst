.. _natmod:

Native machine code in .mpy files
=================================

This section describes how to build and work with .mpy files that contain native
machine code from a language other than Python.  This allows you to
write code in a language like C, compile and link it into a .mpy file, and then
import this file like a normal Python module.  This can be used for implementing
functionality which is performance critical, or for including an existing
library written in another language.

One of the main advantages of using native .mpy files is that native machine code
can be imported by a script dynamically, without the need to rebuild the main
MicroPython firmware.  This is in contrast to :ref:`cmodules` which also allows
defining custom modules in C but they must be compiled into the main firmware image.

The focus here is on using C to build native modules, but in principle any
language which can be compiled to stand-alone machine code can be put into a
.mpy file.

A native .mpy module is built using the ``mpy_ld.py`` tool, which is found in the
``tools/`` directory of the project.  This tool takes a set of object files
(.o files) and links them together to create a native .mpy files.  It requires
CPython 3 and the library pyelftools v0.25 or greater.

Supported features and limitations
----------------------------------

A .mpy file can contain MicroPython bytecode and/or native machine code.  If it
contains native machine code then the .mpy file has a specific architecture
associated with it.  Current supported architectures are (these are the valid
options for the ``ARCH`` variable, see below):

* ``x86`` (32 bit)
* ``x64`` (64 bit x86)
* ``armv6m`` (ARM Thumb, eg Cortex-M0)
* ``armv7m`` (ARM Thumb 2, eg Cortex-M3)
* ``armv7emsp`` (ARM Thumb 2, single precision float, eg Cortex-M4F, Cortex-M7)
* ``armv7emdp`` (ARM Thumb 2, double precision float, eg Cortex-M7)
* ``xtensa`` (non-windowed, eg ESP8266)
* ``xtensawin`` (windowed with window size 8, eg ESP32)

When compiling and linking the native .mpy file the architecture must be chosen
and the corresponding file can only be imported on that architecture.  For more
details about .mpy files see :ref:`mpy_files`.

Native code must be compiled as position independent code (PIC) and use a global
offset table (GOT), although the details of this varies from architecture to
architecture.  When importing .mpy files with native code the import machinery
is able to do some basic relocation of the native code.  This includes
relocating text, rodata and BSS sections.

Supported features of the linker and dynamic loader are:

* executable code (text)
* read-only data (rodata), including strings and constant data (arrays, structs, etc)
* zeroed data (BSS)
* pointers in text to text, rodata and BSS
* pointers in rodata to text, rodata and BSS

The known limitations are:

* data sections are not supported; workaround: use BSS data and initialise the
  data values explicitly

* static BSS variables are not supported; workaround: use global BSS variables

So, if your C code has writable data, make sure the data is defined globally,
without an initialiser, and only written to within functions.

Linker limitation: the native module is not linked against the symbol table of the
full MicroPython firmware.  Rather, it is linked against an explicit table of exported
symbols found in ``mp_fun_table`` (in ``py/nativeglue.h``), that is fixed at firmware
build time.  It is thus not possible to simply call some arbitrary HAL/OS/RTOS/system
function, for example.

New symbols can be added to the end of the table and the firmware rebuilt.
The symbols also need to be added to ``tools/mpy_ld.py``'s ``fun_table`` dict in the
same location.  This allows ``mpy_ld.py`` to be able to pick the new symbols up and
provide relocations for them when the mpy is imported.  Finally, if the symbol is a
function, a macro or stub should be added to ``py/dynruntime.h`` to make it easy to
call the function.

Defining a native module
------------------------

A native .mpy module is defined by a set of files that are used to build the .mpy.
The filesystem layout consists of two main parts, the source files and the Makefile:

* In the simplest case only a single C source file is required, which contains all
  the code that will be compiled into the .mpy module.  This C source code must
  include the ``py/dynruntime.h`` file to access the MicroPython dynamic API, and
  must at least define a function called ``mpy_init``.  This function will be the
  entry point of the module, called when the module is imported.

  The module can be split into multiple C source files if desired.  Parts of the
  module can also be implemented in Python.  All source files should be listed in
  the Makefile, by adding them to the ``SRC`` variable (see below).  This includes
  both C source files as well as any Python files which will be included in the
  resulting .mpy file.

* The ``Makefile`` contains the build configuration for the module and list the
  source files used to build the .mpy module.  It should define ``MPY_DIR`` as the
  location of the MicroPython repository (to find header files, the relevant Makefile
  fragment, and the ``mpy_ld.py`` tool), ``MOD`` as the name of the module, ``SRC``
  as the list of source files, optionally specify the machine architecture via ``ARCH``,
  and then include ``py/dynruntime.mk``.

Minimal example
---------------

This section provides a fully working example of a simple module named ``factorial``.
This module provides a single function ``factorial.factorial(x)`` which computes the
factorial of the input and returns the result.

Directory layout::

    factorial/
    ├── factorial.c
    └── Makefile

The file ``factorial.c`` contains:

.. code-block:: c

    // Include the header file to get access to the MicroPython API
    #include "py/dynruntime.h"

    // Helper function to compute factorial
    static mp_int_t factorial_helper(mp_int_t x) {
        if (x == 0) {
            return 1;
        }
        return x * factorial_helper(x - 1);
    }

    // This is the function which will be called from Python, as factorial(x)
    static mp_obj_t factorial(mp_obj_t x_obj) {
        // Extract the integer from the MicroPython input object
        mp_int_t x = mp_obj_get_int(x_obj);
        // Calculate the factorial
        mp_int_t result = factorial_helper(x);
        // Convert the result to a MicroPython integer object and return it
        return mp_obj_new_int(result);
    }
    // Define a Python reference to the function above
    static MP_DEFINE_CONST_FUN_OBJ_1(factorial_obj, factorial);

    // This is the entry point and is called when the module is imported
    mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
        // This must be first, it sets up the globals dict and other things
        MP_DYNRUNTIME_INIT_ENTRY

        // Make the function available in the module's namespace
        mp_store_global(MP_QSTR_factorial, MP_OBJ_FROM_PTR(&factorial_obj));

        // This must be last, it restores the globals dict
        MP_DYNRUNTIME_INIT_EXIT
    }

The file ``Makefile`` contains:

.. code-block:: make

    # Location of top-level MicroPython directory
    MPY_DIR = ../../..

    # Name of module
    MOD = factorial

    # Source files (.c or .py)
    SRC = factorial.c

    # Architecture to build for (x86, x64, armv6m, armv7m, xtensa, xtensawin)
    ARCH = x64

    # Include to get the rules for compiling and linking the module
    include $(MPY_DIR)/py/dynruntime.mk

Compiling the module
--------------------

The prerequisite tools needed to build a native .mpy file are:

* The MicroPython repository (at least the ``py/`` and ``tools/`` directories).
* CPython 3, and the library pyelftools (eg ``pip install 'pyelftools>=0.25'``).
* GNU make.
* A C compiler for the target architecture (if C source is used).
* Optionally ``mpy-cross``, built from the MicroPython repository (if .py source is used).

Be sure to select the correct ``ARCH`` for the target you are going to run on.
Then build with::

    $ make

Without modifying the Makefile you can specify the target architecture via::

    $ make ARCH=armv7m

Module usage in MicroPython
---------------------------

Once the module is built there should be a file called ``factorial.mpy``.  Copy
this so it is accessible on the filesystem of your MicroPython system and can be
found in the import path.  The module can now be accessed in Python just like any
other module, for example::

    import factorial
    print(factorial.factorial(10))
    # should display 3628800

Further examples
----------------

See ``examples/natmod/`` for further examples which show many of the available
features of native .mpy modules.  Such features include:

* using multiple C source files
* including Python code alongside C code
* rodata and BSS data
* memory allocation
* use of floating point
* exception handling
* including external C libraries
