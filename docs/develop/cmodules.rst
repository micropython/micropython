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
MicroPython executable or firmware image.  Both Make and CMake build
tools are supported, and when writing an external module it's a good idea to
add the build files for both of these tools so the module can be used on all
ports.  But when compiling a particular port you will only need to use one
method of building, either Make or CMake.

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

  Your ``micropython.mk`` must add your modules source files to the
  ``SRC_USERMOD_C`` or ``SRC_USERMOD_LIB_C`` variables. The former will be
  processed for ``MP_QSTR_`` and ``MP_REGISTER_MODULE`` definitions, the latter
  will not (e.g. helpers and library code that isn't MicroPython-specific).
  These paths should include your expanded copy of ``$(USERMOD_DIR)``, e.g.::

    SRC_USERMOD_C += $(EXAMPLE_MOD_DIR)/modexample.c
    SRC_USERMOD_LIB_C += $(EXAMPLE_MOD_DIR)/utils/algorithm.c

  Similarly, use ``SRC_USERMOD_CXX`` and ``SRC_USERMOD_LIB_CXX`` for C++
  source files. If you want to include assembly files use ``SRC_USERMOD_LIB_ASM``.

  If you have custom compiler options (like ``-I`` to add directories to search
  for header files), these should be added to ``CFLAGS_USERMOD`` for C code
  and to ``CXXFLAGS_USERMOD`` for C++ code.

* ``micropython.cmake`` contains the CMake configuration for this module.

  In ``micropython.cmake``, you may use ``${CMAKE_CURRENT_LIST_DIR}`` as the path to
  the current module.

  Your ``micropython.cmake`` should define an ``INTERFACE`` library and associate
  your source files, compile definitions and include directories with it.
  The library should then be linked to the ``usermod`` target.

  .. code-block:: cmake

      add_library(usermod_cexample INTERFACE)

      target_sources(usermod_cexample INTERFACE
          ${CMAKE_CURRENT_LIST_DIR}/examplemodule.c
      )

      target_include_directories(usermod_cexample INTERFACE
          ${CMAKE_CURRENT_LIST_DIR}
      )

      target_link_libraries(usermod INTERFACE usermod_cexample)


  See below for full usage example.


Basic example
-------------

The ``cexample`` module provides examples for a function and a class. The
``cexample.add_ints(a, b)`` function adds two integer args together and returns
the result. The ``cexample.Timer()`` type creates timers that can be used to
measure the elapsed time since the object is instantiated.

The module can be found in the MicroPython source tree
`in the examples directory <https://github.com/micropython/micropython/tree/master/examples/usercmodule/cexample>`_
and has a source file and a Makefile fragment with content as described above::

    micropython/
    └──examples/
       └──usercmodule/
          └──cexample/
             ├── examplemodule.c
             ├── micropython.mk
             └── micropython.cmake


Refer to the comments in these files for additional explanation.
Next to the ``cexample`` module there's also ``cppexample`` which
works in the same way but shows one way of mixing C and C++ code
in MicroPython.


Compiling the cmodule into MicroPython
--------------------------------------

To build such a module, compile MicroPython (see `getting started
<https://github.com/micropython/micropython/wiki/Getting-Started>`_),
applying 2 modifications:

1. Set the build-time flag ``USER_C_MODULES`` to point to the modules
   you want to include.  For ports that use Make this variable should be a
   directory which is searched automatically for modules.  For ports that
   use CMake this variable should be a file which includes the modules to
   build.  See below for details.

2. Enable the modules by setting the corresponding C preprocessor macro to
   1.  This is only needed if the modules you are building are not
   automatically enabled.

For building the example modules which come with MicroPython,
set ``USER_C_MODULES`` to the ``examples/usercmodule`` directory for Make,
or to ``examples/usercmodule/micropython.cmake`` for CMake.

For example, here's how the to build the unix port with the example modules:

.. code-block:: bash

    cd micropython/ports/unix
    make USER_C_MODULES=../../examples/usercmodule

You may need to run ``make clean`` once at the start when including new
user modules in the build.  The build output will show the modules found::

    ...
    Including User C Module from ../../examples/usercmodule/cexample
    Including User C Module from ../../examples/usercmodule/cppexample
    ...

For a CMake-based port such as rp2, this will look a little different (note
that CMake is actually invoked by ``make``):

.. code-block:: bash

    cd micropython/ports/rp2
    make USER_C_MODULES=../../examples/usercmodule/micropython.cmake

Again, you may need to run ``make clean`` first for CMake to pick up the
user modules.  The CMake build output lists the modules by name::

    ...
    Including User C Module(s) from ../../examples/usercmodule/micropython.cmake
    Found User C Module(s): usermod_cexample, usermod_cppexample
    ...

The contents of the top-level ``micropython.cmake`` can be used to control which
modules are enabled.

For your own projects it's more convenient to keep custom code out of the main
MicroPython source tree, so a typical project directory structure will look
like this::

      my_project/
      ├── modules/
      │   ├── example1/
      │   │   ├── example1.c
      │   │   ├── micropython.mk
      │   │   └── micropython.cmake
      │   ├── example2/
      │   │   ├── example2.c
      │   │   ├── micropython.mk
      │   │   └── micropython.cmake
      │   └── micropython.cmake
      └── micropython/
          ├──ports/
         ... ├──stm32/
            ...

When building with Make set ``USER_C_MODULES`` to the ``my_project/modules``
directory.  For example, building the stm32 port:

.. code-block:: bash

    cd my_project/micropython/ports/stm32
    make USER_C_MODULES=../../../modules

When building with CMake the top level ``micropython.cmake`` -- found directly
in the ``my_project/modules`` directory -- should ``include`` all of the modules
you want to have available:

  .. code-block:: cmake

      include(${CMAKE_CURRENT_LIST_DIR}/example1/micropython.cmake)
      include(${CMAKE_CURRENT_LIST_DIR}/example2/micropython.cmake)

Then build with:

.. code-block:: bash

    cd my_project/micropython/ports/esp32
    make USER_C_MODULES=../../../../modules/micropython.cmake

Note that the esp32 port needs the extra ``..`` for relative paths due to the
location of its main ``CMakeLists.txt`` file.   You can also specify absolute
paths to ``USER_C_MODULES``.

All modules specified by the ``USER_C_MODULES`` variable (either found in this
directory when using Make, or added via ``include`` when using CMake) will be
compiled, but only those which are enabled will be available for importing.
User modules are usually enabled by default (this is decided by the developer
of the module), in which case there is nothing more to do than set ``USER_C_MODULES``
as described above.

If a module is not enabled by default then the corresponding C preprocessor macro
must be enabled.  This macro name can be found by searching for the ``MP_REGISTER_MODULE``
line in the module's source code (it usually appears at the end of the main source file).
This macro should be surrounded by a ``#if X`` / ``#endif`` pair, and the configuration
option ``X`` must be set to 1 using ``CFLAGS_EXTRA`` to make the module available.  If
there is no ``#if X`` / ``#endif`` pair then the module is enabled by default.

For example, the ``examples/usercmodule/cexample`` module is enabled by default so
has the following line in its source code:

  .. code-block:: c

      MP_REGISTER_MODULE(MP_QSTR_cexample, example_user_cmodule);

Alternatively, to make this module disabled by default but selectable through
a preprocessor configuration option, it would be:

  .. code-block:: c

      #if MODULE_CEXAMPLE_ENABLED
      MP_REGISTER_MODULE(MP_QSTR_cexample, example_user_cmodule);
      #endif

In this case the module is enabled by adding ``CFLAGS_EXTRA=-DMODULE_CEXAMPLE_ENABLED=1``
to the ``make`` command, or editing ``mpconfigport.h`` or ``mpconfigboard.h`` to add

  .. code-block:: c

      #define MODULE_CEXAMPLE_ENABLED (1)

Note that the exact method depends on the port as they have different
structures.  If not done correctly it will compile but importing will
fail to find the module.


Module usage in MicroPython
---------------------------

Once built into your copy of MicroPython, the module
can now be accessed in Python just like any other builtin module, e.g.

.. code-block:: python

    import cexample
    print(cexample.add_ints(1, 3))
    # should display 4

.. code-block:: python

    from cexample import Timer
    from time import sleep_ms

    watch = Timer()
    sleep_ms(1000)
    print(watch.time())
    # should display approximately 1000
