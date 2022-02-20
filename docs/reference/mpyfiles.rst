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

* Bytecode features used in the .mpy file: there are two bytecode features
  which must match between the file and the system: unicode support and
  inline caching of map lookups in the bytecode.

* Small integer bits: the .mpy file will require a minimum number of bits in
  a small integer and the system loading it must support at least this many
  bits.

* Qstr compression window size: the .mpy file will require a minimum window
  size for qstr decompression and the system loading it must have a window
  greater or equal to this size.

* Native architecture: if the .mpy file contains native machine code then
  it will specify the architecture of that machine code and the system
  loading it must support execution of that architecture's code.

If a MicroPython system supports importing .mpy files then the
``sys.implementation.mpy`` field will exist and return an integer which
encodes the version (lower 8 bits), features and native architecture.

Trying to import an .mpy file that fails one of the first four tests will
raise ``ValueError('incompatible .mpy file')``.  Trying to import an .mpy
file that fails the native architecture test (if it contains native machine
code) will raise ``ValueError('incompatible .mpy arch')``.

If importing an .mpy file fails then try the following:

* Determine the .mpy version and flags supported by your MicroPython system
  by executing::

    import sys
    sys_mpy = sys.implementation.mpy
    arch = [None, 'x86', 'x64',
        'armv6', 'armv6m', 'armv7m', 'armv7em', 'armv7emsp', 'armv7emdp',
        'xtensa', 'xtensawin'][sys_mpy >> 10]
    print('mpy version:', sys_mpy & 0xff)
    print('mpy flags:', end='')
    if arch:
        print(' -march=' + arch, end='')
    if not sys_mpy & 0x200:
        print(' -mno-unicode', end='')
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
v1.12 and up        5
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
4 to 5              5716c5cf65e9b2cb46c2906f40302401bdd27517
3 to 4              9a5f92ea72754c01cc03e5efcdfe94021120531e
2 to 3              ff93fd4f50321c6190e1659b19e64fef3045a484
1 to 2              dd11af209d226b7d18d5148b239662e30ed60bad
0 to 1              6a11048af1d01c78bdacddadd1b72dc7ba7c6478
initial version 0   d8c834c95d506db979ec871417de90b7951edc30
=================== ========================================

Binary encoding of .mpy files
-----------------------------

MicroPython .mpy files are a binary container format with code objects
stored internally in a nested hierarchy.  To keep files small while still
providing a large range of possible values it uses the concept of a
variably-encoded-unsigned-integer (vuint) in many places.  Similar to utf-8
encoding, this encoding stores 7 bits per byte with the 8th bit (MSB) set
if one or more bytes follow.  The bits of the unsigned integer are stored
in the vuint in LSB form.

The top-level of an .mpy file consists of two parts:

* The header.

* The raw-code for the outer scope of the module.
  This outer scope is executed when the .mpy file is imported.

The header
~~~~~~~~~~

The .mpy header is:

======  ================================
size    field
======  ================================
byte    value 0x4d (ASCII 'M')
byte    .mpy version number
byte    feature flags
byte    number of bits in a small int
vuint   size of qstr window
======  ================================

Raw code elements
~~~~~~~~~~~~~~~~~

A raw-code element contains code, either bytecode or native machine code.  Its
contents are:

======  ================================
size    field
======  ================================
vuint   type and size
...     code (bytecode or machine code)
vuint   number of constant objects
vuint   number of sub-raw-code elements
...     constant objects
...     sub-raw-code elements
======  ================================

The first vuint in a raw-code element encodes the type of code stored in this
element (the two least-significant bits), and the decompressed length of the code
(the amount of RAM to allocate for it).

Following the vuint comes the code itself.  In the case of bytecode it also contains
compressed qstr values.

Following the code comes a vuint counting the number of constant objects, and
another vuint counting the number of sub-raw-code elements.

The constant objects are then stored next.

Finally any sub-raw-code elements are stored, recursively.
