Example of embedding MicroPython in a standalone C application
==============================================================

This directory contains a simple example of how to embed MicroPython in an
existing C application.

A C application is represented here by the file `main.c`.  It executes two
simple Python scripts which print things to the standard output.

Building the example
--------------------

First build the embed port using:

    $ make -f micropython_embed.mk

This will generate the `micropython_embed` directory which is a self-contained
copy of MicroPython suitable for embedding.  The .c files in this directory need
to be compiled into your project, in whatever way your project can do that.  The
example here uses make and a provided `Makefile`.

To build the example project, based on `main.c`, use:

    $ make

That will create an executable called `embed` which you can run:

    $ ./embed

Building the example with CMake
-------------------------------

As an alternative to the two-step `make` flow above, this example can also be
built directly with CMake.  In this mode MicroPython is compiled from the
repository sources as a library and linked into the application; there is no
separate `micropython_embed` generation step.

    $ cmake -S . -B build
    $ cmake --build build
    $ ./build/embed-test

See `CMakeLists.txt` for how the port is consumed.  The key steps are pointing
`MICROPY_EMBED_CONFIG_DIR` at the directory containing `mpconfigport.h`, calling
`find_package(micropython-embed ...)`, and linking against the
`micropython::micropython` target.  By default a static library is produced;
set `MICROPY_EMBED_LIBRARY_TYPE` to `SHARED` to build a shared library instead.

Out of tree build
-----------------

This example is set up to work out of the box, being part of the MicroPython
tree.  Your application will be outside of this tree, but the only thing you
need to do for that is to change `MICROPYTHON_TOP` (found in `micropython_embed.mk`)
to point to the location of the MicroPython repository.  The MicroPython
repository may, for example, be a git submodule in your project.
