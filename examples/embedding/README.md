Example of embedding MicroPython in a standalone C application
==============================================================

This directory contains a simple example of how to embed MicroPython in an
existing C application.

A C application is represented here by the file `main.c`.  It executes two
simple Python scripts which print things to the standard output.

Building the example
--------------------

To build the example project, based on `main.c`, use:

    $ make

This will first build the micropython embedded sources using the
`micropython_embed.mk` sub-make, then compile those sources together with `main.c`
using the `makefile.mk` sub-make. (It's done this way, so that the second sub-make
can discover sources that might not exist until after the first when building from
clean.)

That will create an executable called `embed` which you can run:

    $ ./embed

Building the embed port manually
--------------------------------

You can also building the embed port directly using:

    $ make -f micropython_embed.mk

This will generate the `micropython_embed` directory which is a self-contained
copy of MicroPython suitable for embedding.  The .c files in this directory need
to be compiled into your project, in whatever way your project can do that.  The
example here uses make and the provided `makefile.mk`.

    $ make -f makefile.mk

Out of tree build
-----------------

This example is set up to work out of the box, being part of the MicroPython
tree.  Your application will be outside of this tree, but the only thing you
need to do for that is to change `MICROPYTHON_TOP` (found in `micropython_embed.mk`)
to point to the location of the MicroPython repository.  The MicroPython
repository may, for example, be a git submodule in your project.
