MicroPython embed port
======================

This is a port of MicroPython that outputs a set of .c and .h files for embedding
into a wider project.  This port essentially targets the C language, instead of a
particular hardware architecture or platform.

To use this port in a project there are three main steps:

1. Provide configuration for the project via an `mpconfigport.h` file.

2. Build this embed port against that configuration, using the provided `embed.mk`.
   The output is a set of self-contained source files for building MicroPython.
   These files can be placed outside this repository.

3. Build the project.  This requires compiling all .c files from the above step.

See `examples/embedding` for an example.
