.. _constrained:

MicroPython on microcontrollers
===============================

MicroPython is designed to be capable of running on microcontrollers. These
have hardware limitations which may be unfamiliar to programmers more familiar
with conventional computers. In particular the amount of RAM and nonvolatile
"disk" (flash memory) storage is limited. This tutorial offers ways to make
the most of the limited resources. Because MicroPython runs on controllers
based on a variety of architectures, the methods presented are generic: in some
cases it will be necessary to obtain detailed information from platform specific
documentation.

Flash memory
------------

On the Pyboard the simple way to address the limited capacity is to fit a micro
SD card. In some cases this is impractical, either because the device does not
have an SD card slot or for reasons of cost or power consumption; hence the
on-chip flash must be used. The firmware including the MicroPython subsystem is
stored in the onboard flash. The remaining capacity is available for use. For
reasons connected with the physical architecture of the flash memory part of
this capacity may be inaccessible as a filesystem. In such cases this space may
be employed by incorporating user modules into a firmware build which is then
flashed to the device.

There are two ways to achieve this: frozen modules and frozen bytecode. Frozen
modules store the Python source with the firmware. Frozen bytecode uses the
cross compiler to convert the source to bytecode which is then stored with the
firmware. In either case the module may be accessed with an import statement:

.. code::

    import mymodule

The procedure for producing frozen modules and bytecode is platform dependent;
instructions for building the firmware can be found in the README files in the
relevant part of the source tree.

In general terms the steps are as follows:

* Clone the MicroPython `repository <https://github.com/micropython/micropython>`_.
* Acquire the (platform specific) toolchain to build the firmware.
* Build the cross compiler.
* Place the modules to be frozen in a specified directory (dependent on whether
  the module is to be frozen as source or as bytecode).
* Build the firmware. A specific command may be required to build frozen
  code of either type - see the platform documentation.
* Flash the firmware to the device.

RAM
---

When reducing RAM usage there are two phases to consider: compilation and
execution. In addition to memory consumption, there is also an issue known as
heap fragmentation. In general terms it is best to minimise the repeated
creation and destruction of objects. The reason for this is covered in the
section covering the `heap`_.

Compilation phase
~~~~~~~~~~~~~~~~~

When a module is imported, MicroPython compiles the code to bytecode which is
then executed by the MicroPython virtual machine (VM). The bytecode is stored
in RAM. The compiler itself requires RAM, but this becomes available for use
when the compilation has completed.

If a number of modules have already been imported the situation can arise where
there is insufficient RAM to run the compiler. In this case the import
statement will produce a memory exception.

If a module instantiates global objects on import it will consume RAM at the
time of import, which is then unavailable for the compiler to use on subsequent
imports. In general it is best to avoid code which runs on import; a better
approach is to have initialisation code which is run by the application after
all modules have been imported. This maximises the RAM available to the
compiler.

If RAM is still insufficient to compile all modules one solution is to
precompile modules. MicroPython has a cross compiler capable of compiling Python
modules to bytecode (see the README in the mpy-cross directory). The resulting
bytecode file has a .mpy extension; it may be copied to the filesystem and
imported in the usual way. Alternatively some or all modules may be implemented
as frozen bytecode: on most platforms this saves even more RAM as the bytecode
is run directly from flash rather than being stored in RAM.

Execution phase
~~~~~~~~~~~~~~~

There are a number of coding techniques for reducing RAM usage.

**Constants**

MicroPython provides a ``const`` keyword which may be used as follows:

.. code::

    from micropython import const
    ROWS = const(33)
    _COLS = const(0x10)
    a = ROWS
    b = _COLS

In both instances where the constant is assigned to a variable the compiler
will avoid coding a lookup to the name of the constant by substituting its
literal value. This saves bytecode and hence RAM. However the ``ROWS`` value
will occupy at least two machine words, one each for the key and value in the
globals dictionary. The presence in the dictionary is necessary because another
module might import or use it. This RAM can be saved by prepending the name
with an underscore as in ``_COLS``: this symbol is not visible outside the
module so will not occupy RAM.

The argument to ``const()`` may be anything which, at compile time, evaluates
to an integer e.g. ``0x100`` or ``1 << 8``. It can even include other const
symbols that have already been defined, e.g. ``1 << BIT``.

**Constant data structures**

Where there is a substantial volume of constant data and the platform supports
execution from Flash, RAM may be saved as follows. The data should be located in
Python modules and frozen as bytecode. The data must be defined as `bytes`
objects. The compiler 'knows' that `bytes` objects are immutable and ensures
that the objects remain in flash memory rather than being copied to RAM. The
`struct` module can assist in converting between `bytes` types and other
Python built-in types.

When considering the implications of frozen bytecode, note that in Python
strings, floats, bytes, integers and complex numbers are immutable. Accordingly
these will be frozen into flash. Thus, in the line

.. code::

    mystring = "The quick brown fox"

the actual string "The quick brown fox" will reside in flash. At runtime a
reference to the string is assigned to the *variable* ``mystring``. The reference
occupies a single machine word. In principle a long integer could be used to
store constant data:

.. code::

    bar = 0xDEADBEEF0000DEADBEEF

As in the string example, at runtime a reference to the arbitrarily large
integer is assigned to the variable ``bar``. That reference occupies a
single machine word. 

It might be expected that tuples of integers could be employed for the purpose
of storing constant data with minimal RAM use. With the current compiler this
is ineffective (the code works, but RAM is not saved).

.. code::

    foo = (1, 2, 3, 4, 5, 6, 100000)

At runtime the tuple will be located in RAM. This may be subject to future
improvement.

**Needless object creation**

There are a number of situations where objects may unwittingly be created and
destroyed. This can reduce the usability of RAM through fragmentation. The
following sections discuss instances of this.

**String concatenation**

Consider the following code fragments which aim to produce constant strings:

.. code::

    var = "foo" + "bar"
    var1 = "foo" "bar"
    var2 = """\
    foo\
    bar"""

Each produces the same outcome, however the first needlessly creates two string
objects at runtime, allocates more RAM for concatenation before producing the
third. The others perform the concatenation at compile time which is more
efficient, reducing fragmentation.

Where strings must be dynamically created before being fed to a stream such as
a file it will save RAM if this is done in a piecemeal fashion. Rather than
creating a large string object, create a substring and feed it to the stream
before dealing with the next.

The best way to create dynamic strings is by means of the string ``format()``
method:

.. code::

    var = "Temperature {:5.2f} Pressure {:06d}\n".format(temp, press)

**Buffers**

When accessing devices such as instances of UART, I2C and SPI interfaces, using
pre-allocated buffers avoids the creation of needless objects. Consider these
two loops:

.. code::

    while True:
        var = spi.read(100)
        # process data

    buf = bytearray(100)
    while True:
        spi.readinto(buf)
        # process data in buf

The first creates a buffer on each pass whereas the second re-uses a pre-allocated
buffer; this is both faster and more efficient in terms of memory fragmentation.

**Bytes are smaller than ints**

On most platforms an integer consumes four bytes. Consider the two calls to the
function ``foo()``:

.. code::

    def foo(bar):
        for x in bar:
            print(x)
    foo((1, 2, 0xff))
    foo(b'\1\2\xff')

In the first call a tuple of integers is created in RAM. The second efficiently
creates a `bytes` object consuming the minimum amount of RAM. If the module
were frozen as bytecode, the `bytes` object would reside in flash.

**Strings Versus Bytes**

Python3 introduced Unicode support. This introduced a distinction between a
string and an array of bytes. MicroPython ensures that Unicode strings take no
additional space so long as all characters in the string are ASCII (i.e. have
a value < 126). If values in the full 8-bit range are required `bytes` and
`bytearray` objects can be used to ensure that no additional space will be
required. Note that most string methods (e.g. :meth:`str.strip()`) apply also to `bytes`
instances so the process of eliminating Unicode can be painless.

.. code::

    s = 'the quick brown fox'   # A string instance
    b = b'the quick brown fox'  # A bytes instance

Where it is necessary to convert between strings and bytes the :meth:`str.encode`
and the :meth:`bytes.decode` methods can be used. Note that both strings and bytes
are immutable. Any operation which takes as input such an object and produces
another implies at least one RAM allocation to produce the result. In the
second line below a new bytes object is allocated. This would also occur if ``foo``
were a string.

.. code::

    foo = b'   empty whitespace'
    foo = foo.lstrip()

**Runtime compiler execution**

The Python funcitons `eval` and `exec` invoke the compiler at runtime, which
requires significant amounts of RAM. Note that the ``pickle`` library from
`micropython-lib` employs `exec`. It may be more RAM efficient to use the
`json` library for object serialisation.

**Storing strings in flash**

Python strings are immutable hence have the potential to be stored in read only
memory. The compiler can place in flash strings defined in Python code. As with
frozen modules it is necessary to have a copy of the source tree on the PC and
the toolchain to build the firmware. The procedure will work even if the
modules have not been fully debugged, so long as they can be imported and run.

After importing the modules, execute:

.. code::

    micropython.qstr_info(1)

Then copy and paste all the Q(xxx) lines into a text editor. Check for and
remove lines which are obviously invalid. Open the file qstrdefsport.h which
will be found in ports/stm32 (or the equivalent directory for the architecture in
use). Copy and paste the corrected lines at the end of the file. Save the file,
rebuild and flash the firmware. The outcome can be checked by importing the
modules and again issuing:

.. code::

    micropython.qstr_info(1)

The Q(xxx) lines should be gone.

.. _heap:

The heap
--------

When a running program instantiates an object the necessary RAM is allocated
from a fixed size pool known as the heap. When the object goes out of scope (in
other words becomes inaccessible to code) the redundant object is known as
"garbage". A process known as "garbage collection" (GC) reclaims that memory,
returning it to the free heap. This process runs automatically, however it can
be invoked directly by issuing `gc.collect()`.

The discourse on this is somewhat involved. For a 'quick fix' issue the
following periodically:

.. code::

    gc.collect()
    gc.threshold(gc.mem_free() // 4 + gc.mem_alloc())

Fragmentation
~~~~~~~~~~~~~

Say a program creates an object ``foo``, then an object ``bar``. Subsequently
``foo`` goes out of scope but ``bar`` remains. The RAM used by ``foo`` will be
reclaimed by GC. However if ``bar`` was allocated to a higher address, the
RAM reclaimed from ``foo`` will only be of use for objects no bigger than
``foo``. In a complex or long running program the heap can become fragmented:
despite there being a substantial amount of RAM available, there is insufficient
contiguous space to allocate a particular object, and the program fails with a
memory error.

The techniques outlined above aim to minimise this. Where large permanent buffers
or other objects are required it is best to instantiate these early in the
process of program execution before fragmentation can occur. Further improvements
may be made by monitoring the state of the heap and by controlling GC; these are
outlined below.

Reporting
~~~~~~~~~

A number of library functions are available to report on memory allocation and
to control GC. These are to be found in the `gc` and `micropython` modules.
The following example may be pasted at the REPL (``ctrl e`` to enter paste mode,
``ctrl d`` to run it).

.. code::

    import gc
    import micropython
    gc.collect()
    micropython.mem_info()
    print('-----------------------------')
    print('Initial free: {} allocated: {}'.format(gc.mem_free(), gc.mem_alloc()))
    def func():
        a = bytearray(10000)
    gc.collect()
    print('Func definition: {} allocated: {}'.format(gc.mem_free(), gc.mem_alloc()))
    func()
    print('Func run free: {} allocated: {}'.format(gc.mem_free(), gc.mem_alloc()))
    gc.collect()
    print('Garbage collect free: {} allocated: {}'.format(gc.mem_free(), gc.mem_alloc()))
    print('-----------------------------')
    micropython.mem_info(1)

Methods employed above:

* `gc.collect()` Force a garbage collection. See footnote.
* `micropython.mem_info()` Print a summary of RAM utilisation.
* `gc.mem_free()` Return the free heap size in bytes.
* `gc.mem_alloc()` Return the number of bytes currently allocated.
* ``micropython.mem_info(1)`` Print a table of heap utilisation (detailed below).

The numbers produced are dependent on the platform, but it can be seen that
declaring the function uses a small amount of RAM in the form of bytecode
emitted by the compiler (the RAM used by the compiler has been reclaimed).
Running the function uses over 10KiB, but on return ``a`` is garbage because it
is out of scope and cannot be referenced. The final `gc.collect()` recovers
that memory.

The final output produced by ``micropython.mem_info(1)`` will vary in detail but
may be interpreted as follows:

====== =================
Symbol Meaning
====== =================
   .   free block
   h   head block
   =   tail block
   m   marked head block
   T   tuple
   L   list
   D   dict
   F   float
   B   byte code
   M   module
====== =================

Each letter represents a single block of memory, a block being 16 bytes. So each
line of the heap dump represents 0x400 bytes or 1KiB of RAM.

Control of garbage collection
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A GC can be demanded at any time by issuing `gc.collect()`. It is advantageous
to do this at intervals, firstly to pre-empt fragmentation and secondly for
performance. A GC can take several milliseconds but is quicker when there is
little work to do (about 1ms on the Pyboard). An explicit call can minimise that
delay while ensuring it occurs at points in the program when it is acceptable.

Automatic GC is provoked under the following circumstances. When an attempt at
allocation fails, a GC is performed and the allocation re-tried. Only if this
fails is an exception raised. Secondly an automatic GC will be triggered if the
amount of free RAM falls below a threshold. This threshold can be adapted as
execution progresses:

.. code::

    gc.collect()
    gc.threshold(gc.mem_free() // 4 + gc.mem_alloc())

This will provoke a GC when more than 25% of the currently free heap becomes
occupied.

In general modules should instantiate data objects at runtime using constructors
or other initialisation functions. The reason is that if this occurs on
initialisation the compiler may be starved of RAM when subsequent modules are
imported. If modules do instantiate data on import then `gc.collect()` issued
after the import will ameliorate the problem.

String operations
-----------------

MicroPython handles strings in an efficient manner and understanding this can
help in designing applications to run on microcontrollers. When a module
is compiled, strings which occur multiple times are stored once only, a process
known as string interning. In MicroPython an interned string is known as a ``qstr``.
In a module imported normally that single instance will be located in RAM, but
as described above, in modules frozen as bytecode it will be located in flash.

String comparisons are also performed efficiently using hashing rather than
character by character. The penalty for using strings rather than integers may
hence be small both in terms of performance and RAM usage - a fact which may
come as a surprise to C programmers.

Postscript
----------

MicroPython passes, returns and (by default) copies objects by reference. A
reference occupies a single machine word so these processes are efficient in
RAM usage and speed.

Where variables are required whose size is neither a byte nor a machine word
there are standard libraries which can assist in storing these efficiently and
in performing conversions. See the `array`, `struct` and `uctypes`
modules.

Footnote: gc.collect() return value
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

On Unix and Windows platforms the `gc.collect()` method returns an integer
which signifies the number of distinct memory regions that were reclaimed in the
collection (more precisely, the number of heads that were turned into frees). For
efficiency reasons bare metal ports do not return this value.
