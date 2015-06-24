Assembler Directives
====================

Labels
------

* label(INNER1)

This defines a label for use in a branch instruction. Thus elsewhere in the code a ``b(INNER1)``
will cause execution to continue with the instruction after the label directive.

Defining inline data
--------------------

The following assembler directives facilitate embedding data in an assembler code block.

* data(size, d0, d1 .. dn)

The data directive creates n array of data values in memory. The first argument specifies the
size in bytes of the subsequent arguments. Hence the first statement below will cause the
assembler to put three bytes (with values 2, 3 and 4) into consecutive memory locations
while the second will cause it to emit two four byte words.

::

    data(1, 2, 3, 4)
    data(4, 2, 100000)

Data values longer than a single byte are stored in memory in little-endian format.

* align(nBytes)

Align the following instruction to an nBytes value. ARM Thumb-2 instructions must be two
byte aligned, hence it's advisable to issue ``align(2)`` after ``data`` directives and
prior to any subsequent code. This ensures that the code will run irrespective of the
size of the data array.
