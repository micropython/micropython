.. _cmodules:

MicroPython external C modules
==============================

When developing modules for use with MicroPython you may find you run into
limitations with the Python environment, often due to an inability to access
certain hardware resources or Python speed limitations.

If your limitations can't be resolved with suggestions in :ref:`speed_python`,
writing some or all of your module in C (and/or C++ if implemented for your port)
is a viable option.

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

* ``*.c`` / ``*.cpp`` / ``*.h`` source code files for your module.

  These will typically include the low level functionality being implemented and
  the MicroPython binding functions to expose the functions and module(s).

  Currently the best reference for writing these functions/modules is
  to find similar modules within the MicroPython tree and use them as examples.

* ``micropython.mk`` contains the Makefile fragment for this module.

  ``$(USERMOD_DIR)`` is available in ``micropython.mk`` as the path to your
  module directory. As it's redefined for each c module, is should be expanded
  in your ``micropython.mk`` to a local make variable,
  eg ``EXAMPLE_MOD_DIR := $(USERMOD_DIR)``

  Your ``micropython.mk`` must add your modules source files relative to your
  expanded copy of ``$(USERMOD_DIR)`` to ``SRC_USERMOD``, eg
  ``SRC_USERMOD += $(EXAMPLE_MOD_DIR)/example.c``

  If you have custom compiler options (like ``-I`` to add directories to search
  for header files), these should be added to ``CFLAGS_USERMOD`` for C code
  and to ``CXXFLAGS_USERMOD`` for C++ code.

  See below for full usage example.


Basic example
-------------

This simple module named ``cexample`` provides a single function
``cexample.add_ints(a, b)`` which adds the two integer args together and returns
the result. It can be found in the MicroPython source tree
`in the examples directory <https://github.com/micropython/micropython/tree/master/examples/usercmodule/cexample>`_
and has a source file and a Makefile fragment with content as descibed above::

    micropython/
    └──examples/
       └──usercmodule/
          └──cexample/
             ├── examplemodule.c
             └── micropython.mk

Refer to the comments in these 2 files for additional explanation.
Next to the ``cexample`` module there's also ``cppexample`` which
works in the same way but shows one way of mixing C and C++ code
in MicroPython.


Compiling the cmodule into MicroPython
--------------------------------------

To build such a module, compile MicroPython (see `getting started
<https://github.com/micropython/micropython/wiki/Getting-Started>`_),
applying 2 modifications:

- an extra ``make`` flag named ``USER_C_MODULES`` set to the directory
  containing all modules you want included (not to the module itself).
  For building the example modules which come with MicroPython,
  set ``USER_C_MODULES`` to the ``examples/usercmodule`` directory.
  For your own projects it's more convenient to keep custom code out of
  the main source tree so a typical project directory structure will look
  like this::

      my_project/
      ├── modules/
      │   └──example1/
      │       ├──example1.c
      │       └──micropython.mk
      │   └──example2/
      │       ├──example2.c
      │       └──micropython.mk
      └── micropython/
          ├──ports/
         ... ├──stm32/
            ...


  with ``USER_C_MODULES`` set to the ``my_project/modules`` directory.

- all modules found in this directory will be compiled, but only those
  which are explicitly enabled will be availabe for importing. Enabling a
  module is done by setting the preprocessor define from its module
  registration to 1. For example if the source code defines the module with

  .. code-block:: c

      MP_REGISTER_MODULE(MP_QSTR_cexample, example_user_cmodule, MODULE_CEXAMPLE_ENABLED);


  then ``MODULE_CEXAMPLE_ENABLED`` has to be set to 1 to make the module available.
  This can be done by adding ``CFLAGS_EXTRA=-DMODULE_CEXAMPLE_ENABLED=1`` to
  the ``make`` command, or editing ``mpconfigport.h`` or ``mpconfigboard.h``
  to add

  .. code-block:: c

      #define MODULE_CEXAMPLE_ENABLED (1)


  Note that the exact method depends on the port as they have different
  structures. If not done correctly it will compile but importing will
  fail to find the module.

To sum up, here's how the ``cexample`` module from the ``examples/usercmodule``
directory can be built for the unix port:

.. code-block:: bash

    cd micropython/ports/unix
    make USER_C_MODULES=../../examples/usercmodule CFLAGS_EXTRA=-DMODULE_CEXAMPLE_ENABLED=1 all

The build output will show the modules found::

    ...
    Including User C Module from ../../examples/usercmodule/cexample
    Including User C Module from ../../examples/usercmodule/cppexample
    ...


Or for your own project with a directory structure as shown above,
including both modules and building the stm32 port for example:

.. code-block:: bash

    cd my_project/micropython/ports/stm32
    make USER_C_MODULES=../../../modules \
      CFLAGS_EXTRA="-DMODULE_EXAMPLE1_ENABLED=1 -DMODULE_EXAMPLE2_ENABLED=1" all


Module usage in MicroPython
---------------------------

Once built into your copy of MicroPython, the module
can now be accessed in Python just like any other builtin module, e.g.

.. code-block:: python

    import cexample
    print(cexample.add_ints(1, 3))
    # should display 4
