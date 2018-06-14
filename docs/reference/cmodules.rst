Extending MicroPython with C
============================

Some specialized code would be unacceptably slow or needs to access hardware in
a way that cannot be done from MicroPython. Therefore, it supports a way of
extending the language with custom modules written in C. But before you consider
writing a module in C, please take a look at :ref:`speed_python`.

`Unlike CPython <https://docs.python.org/3/extending/building.html>`_, these
modules are (currently) embedded directly in the program image instead of being
dynamically loaded. This requires a `custom build of MicroPython
<https://github.com/micropython/micropython/wiki/Getting-Started>`_.


Writing a module
----------------

A module is a directory with the following files:

  * ``micropython.mk``, which contains the Makefile fragment for this module.
  * All C files you would like included.

Put the required build commands in ``micropython.mk``. For a simple module, you
will only have to add the file paths to ``SRC_MOD``, which will include these C
files in the build:

.. highlight:: make
.. code::

    # Add all C files to SRC_MOD.
    SRC_MOD += $(USER_C_MODULES)/example/example.c

This is a very bare bones module named ``example`` that provides
``example.double(x)``. Note that the name of the module must be equal to the
directory name and is also used in the name of the ``mp_obj_module_t`` object at
the bottom.

.. highlight:: c
.. code::

    // Include required definitions first.
    #include "py/obj.h"
    #include "py/runtime.h"

    // This is the function you will call using example.double(n).
    STATIC mp_obj_t example_double(mp_obj_t x_obj) {
        // Check input value and convert it to a C type.
        if (!MP_OBJ_IS_SMALL_INT(x_obj)) {
            mp_raise_ValueError("x is not a small int");
        }
        int x = mp_obj_int_get_truncated(x_obj);

        // Calculate the double, and convert back to MicroPython object.
        return mp_obj_new_int(x + x);
    }
    STATIC MP_DEFINE_CONST_FUN_OBJ_1(example_double_obj, example_double);

    // Define all properties of the example module, which currently are the name (a
    // string) and a function.
    // All identifiers and strings are written as MP_QSTR_xxx and will be
    // optimized to word-sized integers by the build system (interned strings).
    STATIC const mp_rom_map_elem_t example_module_globals_table[] = {
        { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_example) },
        { MP_ROM_QSTR(MP_QSTR_double), MP_ROM_PTR(&example_double_obj) },
    };
    STATIC MP_DEFINE_CONST_DICT(example_module_globals, example_module_globals_table);

    // Define module object.
    const mp_obj_module_t example_user_cmodule = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t*)&example_module_globals,
    };


Using a module
--------------

To build such a module, compile MicroPython (see `getting started
<https://github.com/micropython/micropython/wiki/Getting-Started>`_) with an
extra ``make`` flag named ``USER_C_MODULES`` set to the directory containing
all modules you want included (not to the module itself!). For example:

.. highlight:: shell
.. code::

    $ make USER_C_MODULES=path-to-modules-folder all
