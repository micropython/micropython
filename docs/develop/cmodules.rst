.. _cmodules:

MicroPython external C modules
==============================

When developing modules for use with MicroPython you may find you run into
limitations with the Python environment, often due to an inability to access
certain hardware resources or Python speed limitations.

If your limitations can't be resolved with suggestions in :ref:`speed_python`,
writing some or all of your module in C is a viable option.

If your module is designed to access or work with commonly available
hardware or libraries please consider implementing it inside the MicroPython
source tree alongside similar modules and submitting it as a pull request.
If however you're targeting obscure or proprietary systems it may make
more sense to keep this external to the main MicroPython repository.

This chapter describes how to compile such external modules into the
MicroPython executable or firmware image.

An alternative approach is to use :ref:`natmod` which allows writing custom C
code that is placed in a .mpy file, which can be imported dynamically in to
a running MicroPython system without the need to recompile the main firmware.


Structure of an external C module
---------------------------------

A MicroPython user C module is a directory with the following files:

* ``*.c`` and/or ``*.h`` source code files for your module.

  These will typically include the low level functionality being implemented and
  the MicroPython binding functions to expose the functions and module(s).

  Currently the best reference for writing these functions/modules is
  to find similar modules within the MicroPython tree and use them as examples.

* ``micropython.mk`` contains the Makefile fragment for this module.

  ``$(USERMOD_DIR)`` is available in ``micropython.mk`` as the path to your
  module directory. As it's redefined for each c module, is should be expanded
  in your ``micropython.mk`` to a local make variable,
  eg ``EXAMPLE_MOD_DIR := $(USERMOD_DIR)``

  Your ``micropython.mk`` must add your modules C files relative to your
  expanded copy of ``$(USERMOD_DIR)`` to ``SRC_USERMOD``, eg
  ``SRC_USERMOD += $(EXAMPLE_MOD_DIR)/example.c``

  If you have custom ``CFLAGS`` settings or include folders to define, these
  should be added to ``CFLAGS_USERMOD``.

  See below for full usage example.


Basic Example
-------------

This simple module named ``example`` provides a single function
``example.add_ints(a, b)`` which adds the two integer args together and returns
the result.

Directory::

    example/
    ├── example.c
    └── micropython.mk


``example.c``

.. code-block:: c

    // Include required definitions first.
    #include "py/obj.h"
    #include "py/runtime.h"
    #include "py/builtin.h"

    // This is the function which will be called from Python as example.add_ints(a, b).
    STATIC mp_obj_t example_add_ints(mp_obj_t a_obj, mp_obj_t b_obj) {
        // Extract the ints from the micropython input objects
        int a = mp_obj_get_int(a_obj);
        int b = mp_obj_get_int(b_obj);

        // Calculate the addition and convert to MicroPython object.
        return mp_obj_new_int(a + b);
    }
    // Define a Python reference to the function above
    STATIC MP_DEFINE_CONST_FUN_OBJ_2(example_add_ints_obj, example_add_ints);

    // Define all properties of the example module.
    // Table entries are key/value pairs of the attribute name (a string)
    // and the MicroPython object reference.
    // All identifiers and strings are written as MP_QSTR_xxx and will be
    // optimized to word-sized integers by the build system (interned strings).
    STATIC const mp_rom_map_elem_t example_module_globals_table[] = {
        { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_example) },
        { MP_ROM_QSTR(MP_QSTR_add_ints), MP_ROM_PTR(&example_add_ints_obj) },
    };
    STATIC MP_DEFINE_CONST_DICT(example_module_globals, example_module_globals_table);

    // Define module object.
    const mp_obj_module_t example_user_cmodule = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t*)&example_module_globals,
    };

    // Register the module to make it available in Python
    MP_REGISTER_MODULE(MP_QSTR_example, example_user_cmodule, MODULE_EXAMPLE_ENABLED);


``micropython.mk``

.. code-block:: make

    EXAMPLE_MOD_DIR := $(USERMOD_DIR)

    # Add all C files to SRC_USERMOD.
    SRC_USERMOD += $(EXAMPLE_MOD_DIR)/example.c

    # We can add our module folder to include paths if needed
    # This is not actually needed in this example.
    CFLAGS_USERMOD += -I$(EXAMPLE_MOD_DIR)

Finally you will need to define ``MODULE_EXAMPLE_ENABLED`` to 1. This
can be done by adding ``CFLAGS_EXTRA=-DMODULE_EXAMPLE_ENABLED=1`` to
the ``make`` command, or editing ``mpconfigport.h`` or
``mpconfigboard.h`` to add

.. code-block:: c

    #define MODULE_EXAMPLE_ENABLED (1)

Note that the exact method depends on the port as they have different
structures. If not done correctly it will compile but importing will
fail to find the module.


Compiling the cmodule into MicroPython
--------------------------------------

To build such a module, compile MicroPython (see `getting started
<https://github.com/micropython/micropython/wiki/Getting-Started>`_) with an
extra ``make`` flag named ``USER_C_MODULES`` set to the directory containing
all modules you want included (not to the module itself). For example:


Directory::

    my_project/
    ├── modules/
    │   └──example/
    │       ├──example.c
    │       └──micropython.mk
    └── micropython/
        ├──ports/
       ... ├──stm32/
          ...

Building for stm32 port:

.. code-block:: bash

    cd my_project/micropython/ports/stm32
    make USER_C_MODULES=../../../modules CFLAGS_EXTRA=-DMODULE_EXAMPLE_ENABLED=1 all


Module usage in MicroPython
---------------------------

Once built into your copy of MicroPython, the module implemented
in ``example.c`` above can now be accessed in Python just
like any other builtin module, eg

.. code-block:: python

    import example
    print(example.add_ints(1, 3))
    # should display 4
