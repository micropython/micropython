Example of embedding MicroPython in a standalone C application (full)
=====================================================================

This directory contains a simple example of how to embed a full-featured
version of MicroPython in an existing C application.
See also _embedding_ for a more minimal version.

A C application is represented here by the file `main.c`.  It executes two
simple Python scripts which print things to the standard output.
Functions used by the MicroPython core that need to be provided by the
application are implemented in `mphal.c`.

Building the example
--------------------

First build the embed port using:

    $ make -C mpy-cross
    $ cd examples/embedding-full
    $ make -f micropython_embed.mk submodules
    $ make -f micropython_embed.mk

This will generate the `micropython_embed` directory which is a self-contained
copy of MicroPython suitable for embedding.  The .c files in this directory need
to be compiled into your project, in whatever way your project can do that.  The
example here uses make and a provided `Makefile`.

To build the example project, based on `main.c`, use:

    $ make

That will create an executable called `embed` which you can run:

    $ ./embed

Out of tree build
-----------------

This example is set up to work out of the box, being part of the MicroPython
tree.  Your application will be outside of this tree, but the only thing you
need to do for that is to change `MICROPYTHON_TOP` (found in `micropython_embed.mk`)
to point to the location of the MicroPython repository.  The MicroPython
repository may, for example, be a git submodule in your project.
