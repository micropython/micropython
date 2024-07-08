.. _mpy_files:

MicroPython .mpy files
======================

MicroPython defines the concept of an .mpy file which is a binary container
file format that holds precompiled code, and which can be imported like a
normal .py module.  The file ``foo.mpy`` can be imported via ``import foo``,
as long as ``foo.mpy`` can be found in the usual way by the import machinery.
Usually, each directory listed in ``sys.path`` is searched in order.  When
searching a particular directory ``foo.py`` is looked for first and if that
is not found then ``foo.mpy`` is looked for, then the search continues in the
next directory if neither is found.  As such, ``foo.py`` will take precedence
over ``foo.mpy``.

These .mpy files can contain bytecode which is usually generated from Python
source files (.py files) via the ``mpy-cross`` program.  For some architectures
an .mpy file can also contain native machine code, which can be generated in
a variety of ways, most notably from C source code.

Versioning and compatibility of .mpy files
------------------------------------------

A given .mpy file may or may not be compatible with a given MicroPython system.
Compatibility is based on the following:

* Version of the .mpy file: the version of the file must match the version
  supported by the system loading it.

* Sub-version of the .mpy file: if the .mpy file contains native machine code
  then the sub-version of the file must match the version support by the
  system loading it.  Otherwise, if there is no native machine code in the .mpy
  file, then the sub-version is ignored when loading.

* Small integer bits: the .mpy file will require a minimum number of bits in
  a small integer and the system loading it must support at least this many
  bits.

* Native architecture: if the .mpy file contains native machine code then
  it will specify the architecture of that machine code and the system
  loading it must support execution of that architecture's code.

If a MicroPython system supports importing .mpy files then the
``sys.implementation._mpy`` field will exist and return an integer which
encodes the version (lower 8 bits), features and native architecture.

Trying to import an .mpy file that fails one of the first four tests will
raise ``ValueError('incompatible .mpy file')``.  Trying to import an .mpy
file that fails the native architecture test (if it contains native machine
code) will raise ``ValueError('incompatible .mpy arch')``.

If importing an .mpy file fails then try the following:

* Determine the .mpy version and flags supported by your MicroPython system
  by executing::

    import sys
    sys_mpy = sys.implementation._mpy
    arch = [None, 'x86', 'x64',
        'armv6', 'armv6m', 'armv7m', 'armv7em', 'armv7emsp', 'armv7emdp',
        'xtensa', 'xtensawin', 'rv32imc'][sys_mpy >> 10]
    print('mpy version:', sys_mpy & 0xff)
    print('mpy sub-version:', sys_mpy >> 8 & 3)
    print('mpy flags:', end='')
    if arch:
        print(' -march=' + arch, end='')
    print()

* Check the validity of the .mpy file by inspecting the first two bytes of
  the file.  The first byte should be an uppercase 'M' and the second byte
  will be the version number, which should match the system version from above.
  If it doesn't match then rebuild the .mpy file.

* Check if the system .mpy version matches the version emitted by ``mpy-cross``
  that was used to build the .mpy file, found by ``mpy-cross --version``.
  If it doesn't match then recompile ``mpy-cross`` from the Git repository
  checked out at the tag (or hash) reported by ``mpy-cross --version``.

* Make sure you are using the correct ``mpy-cross`` flags, found by the code
  above, or by inspecting the ``MPY_CROSS_FLAGS`` Makefile variable for the
  port that you are using.

The following table shows the correspondence between MicroPython release
and .mpy version.

=================== ============
MicroPython release .mpy version
=================== ============
v1.23.0 and up      6.3
v1.22.x             6.2
v1.20 - v1.21.0     6.1
v1.19.x             6
v1.12 - v1.18       5
v1.11               4
v1.9.3 - v1.10      3
v1.9 - v1.9.2       2
v1.5.1 - v1.8.7     0
=================== ============

For completeness, the next table shows the Git commit of the main
MicroPython repository at which the .mpy version was changed.

=================== ========================================
.mpy version change Git commit
=================== ========================================
6.2 to 6.3          bdbc869f9ea200c0d28b2bc7bfb60acd9d884e1b
6.1 to 6.2          6967ff3c581a66f73e9f3d78975f47528db39980
6 to 6.1            d94141e1473aebae0d3c63aeaa8397651ad6fa01
5 to 6              f2040bfc7ee033e48acef9f289790f3b4e6b74e5
4 to 5              5716c5cf65e9b2cb46c2906f40302401bdd27517
3 to 4              9a5f92ea72754c01cc03e5efcdfe94021120531e
2 to 3              ff93fd4f50321c6190e1659b19e64fef3045a484
1 to 2              dd11af209d226b7d18d5148b239662e30ed60bad
0 to 1              6a11048af1d01c78bdacddadd1b72dc7ba7c6478
initial version 0   d8c834c95d506db979ec871417de90b7951edc30
=================== ========================================

Binary encoding of .mpy files
-----------------------------

MicroPython .mpy files are a binary container format with code objects (bytecode
and native machine code) stored internally in a nested hierarchy.  The code for
the outer module is stored first, and then its children follow.  Each child may
have further children, for example in the case of a class having methods, or a
function defining a lambda or comprehension.  To keep files small while still
providing a large range of possible values it uses the concept of a
variably-encoded-unsigned-integer (vuint) in many places.  Similar to utf-8
encoding, this encoding stores 7 bits per byte with the 8th bit (MSB) set
if one or more bytes follow.  The bits of the unsigned integer are stored
in the vuint in LSB form.

The top-level of an .mpy file consists of three parts:

* The header.

* The global qstr and constant tables.

* The raw-code for the outer scope of the module.
  This outer scope is executed when the .mpy file is imported.

You can inspect the contents of a .mpy file by using ``mpy-tool.py``, for
example (run from the root of the main MicroPython repository)::

    $ ./tools/mpy-tool.py -xd myfile.mpy

The header
~~~~~~~~~~

The .mpy header is:

======  ================================
size    field
======  ================================
byte    value 0x4d (ASCII 'M')
byte    .mpy major version number
byte    native arch and minor version number (was feature flags in older versions)
byte    number of bits in a small int
======  ================================

The global qstr and constant tables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

An .mpy file contains a single qstr table, and a single constant object table.
These are global to the .mpy file, they are referenced by all nested raw-code
objects.  The qstr table maps internal qstr number (internal to the .mpy file)
to the resolved qstr number of the runtime that the .mpy file is imported into.
This links the .mpy file with the rest of the system that it executes within.
The constant object table is populated with references to all constant objects
that the .mpy file needs.

======  ================================
size    field
======  ================================
vuint   number of qstrs
vuint   number of constant objects
...     qstr data
...     encoded constant objects
======  ================================

Raw code elements
~~~~~~~~~~~~~~~~~

A raw-code element contains code, either bytecode or native machine code.  Its
contents are:

======  ================================
size    field
======  ================================
vuint   type, size and whether there are sub-raw-code elements
...     code (bytecode or machine code)
vuint   number of sub-raw-code elements (only if non-zero)
...     sub-raw-code elements
======  ================================

The first vuint in a raw-code element encodes the type of code stored in this
element (the two least-significant bits), whether this raw-code has any
children (the third least-significant bit), and the length of the code that
follows (the amount of RAM to allocate for it).

Following the vuint comes the code itself.  Unless the code type is viper code
with relocations, this code is constant data and does not need to be modified.

If this raw-code has any children (as indicated by a bit in the first vuint),
following the code comes a vuint counting the number of sub-raw-code elements.

Finally any sub-raw-code elements are stored, recursively.
