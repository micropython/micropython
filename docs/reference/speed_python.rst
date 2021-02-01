.. _speed_python:

Maximising MicroPython speed
============================

.. contents::

This tutorial describes ways of improving the performance of MicroPython code.
Optimisations involving other languages are covered elsewhere, namely the use
of modules written in C and the MicroPython inline assembler.

The process of developing high performance code comprises the following stages
which should be performed in the order listed.

* Design for speed.
* Code and debug.

Optimisation steps:

* Identify the slowest section of code.
* Improve the efficiency of the Python code.
* Use the native code emitter.
* Use the viper code emitter.
* Use hardware-specific optimisations.

Designing for speed
-------------------

Performance issues should be considered at the outset. This involves taking a view
on the sections of code which are most performance critical and devoting particular
attention to their design. The process of optimisation begins when the code has
been tested: if the design is correct at the outset optimisation will be
straightforward and may actually be unnecessary.

Algorithms
~~~~~~~~~~

The most important aspect of designing any routine for performance is ensuring that
the best algorithm is employed. This is a topic for textbooks rather than for a 
MicroPython guide but spectacular performance gains can sometimes be achieved
by adopting algorithms known for their efficiency.

RAM allocation
~~~~~~~~~~~~~~

To design efficient MicroPython code it is necessary to have an understanding of the
way the interpreter allocates RAM. When an object is created or grows in size
(for example where an item is appended to a list) the necessary RAM is allocated
from a block known as the heap. This takes a significant amount of time;
further it will on occasion trigger a process known as garbage collection which
can take several milliseconds.

Consequently the performance of a function or method can be improved if an object is created
once only and not permitted to grow in size. This implies that the object persists
for the duration of its use: typically it will be instantiated in a class constructor
and used in various methods.

This is covered in further detail :ref:`Controlling garbage collection <controlling_gc>` below.

Buffers
~~~~~~~

An example of the above is the common case where a buffer is required, such as one
used for communication with a device. A typical driver will create the buffer in the
constructor and use it in its I/O methods which will be called repeatedly.

The MicroPython libraries typically provide support for pre-allocated buffers. For
example, objects which support stream interface (e.g., file or UART) provide ``read()``
method which allocates new buffer for read data, but also a ``readinto()`` method
to read data into an existing buffer.

Floating point
~~~~~~~~~~~~~~

Some MicroPython ports allocate floating point numbers on heap. Some other ports
may lack dedicated floating-point coprocessor, and perform arithmetic operations
on them in "software" at considerably lower speed than on integers. Where
performance is important, use integer operations and restrict the use of floating
point to sections of the code where performance is not paramount. For example,
capture ADC readings as integers values to an array in one quick go, and only then
convert them to floating-point numbers for signal processing.

Arrays
~~~~~~

Consider the use of the various types of array classes as an alternative to lists.
The `array` module supports various element types with 8-bit elements supported
by Python's built in `bytes` and `bytearray` classes. These data structures all store
elements in contiguous memory locations. Once again to avoid memory allocation in critical
code these should be pre-allocated and passed as arguments or as bound objects.

When passing slices of objects such as `bytearray` instances, Python creates
a copy which involves allocation of the size proportional to the size of slice.
This can be alleviated using a `memoryview` object. `memoryview` itself
is allocated on heap, but is a small, fixed-size object, regardless of the size
of slice it points too.

.. code:: python

    ba = bytearray(10000)  # big array
    func(ba[30:2000])      # a copy is passed, ~2K new allocation
    mv = memoryview(ba)    # small object is allocated
    func(mv[30:2000])      # a pointer to memory is passed

A `memoryview` can only be applied to objects supporting the buffer protocol - this
includes arrays but not lists. Small caveat is that while memoryview object is live,
it also keeps alive the original buffer object. So, a memoryview isn't a universal
panacea. For instance, in the example above, if you are done with 10K buffer and
just need those bytes 30:2000 from it, it may be better to make a slice, and let
the 10K buffer go (be ready for garbage collection), instead of making a
long-living memoryview and keeping 10K blocked for GC.

Nonetheless, `memoryview` is indispensable for advanced preallocated buffer
management. ``readinto()`` method discussed above puts data at the beginning
of buffer and fills in entire buffer. What if you need to put data in the
middle of existing buffer? Just create a memoryview into the needed section
of buffer and pass it to ``readinto()``.

Identifying the slowest section of code
---------------------------------------

This is a process known as profiling and is covered in textbooks and
(for standard Python) supported by various software tools. For the type of
smaller embedded application likely to be running on MicroPython platforms
the slowest function or method can usually be established by judicious use
of the timing ``ticks`` group of functions documented in `utime`.
Code execution time can be measured in ms, us, or CPU cycles.

The following enables any function or method to be timed by adding an
``@timed_function`` decorator:

.. code:: python

    def timed_function(f, *args, **kwargs):
        myname = str(f).split(' ')[1]
        def new_func(*args, **kwargs):
            t = utime.ticks_us()
            result = f(*args, **kwargs)
            delta = utime.ticks_diff(utime.ticks_us(), t)
            print('Function {} Time = {:6.3f}ms'.format(myname, delta/1000))
            return result
        return new_func

MicroPython code improvements
-----------------------------

The const() declaration
~~~~~~~~~~~~~~~~~~~~~~~

MicroPython provides a ``const()`` declaration. This works in a similar way
to ``#define`` in C in that when the code is compiled to bytecode the compiler
substitutes the numeric value for the identifier. This avoids a dictionary
lookup at runtime. The argument to ``const()`` may be anything which, at
compile time, evaluates to an integer e.g. ``0x100`` or ``1 << 8``.

.. _Caching:

Caching object references
~~~~~~~~~~~~~~~~~~~~~~~~~~

Where a function or method repeatedly accesses objects performance is improved
by caching the object in a local variable:

.. code:: python

    class foo(object):
        def __init__(self):
            self.ba = bytearray(100)
        def bar(self, obj_display):
            ba_ref = self.ba
            fb = obj_display.framebuffer
            # iterative code using these two objects

This avoids the need repeatedly to look up ``self.ba`` and ``obj_display.framebuffer``
in the body of the method ``bar()``.

.. _controlling_gc:

Controlling garbage collection
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When memory allocation is required, MicroPython attempts to locate an adequately
sized block on the heap. This may fail, usually because the heap is cluttered
with objects which are no longer referenced by code. If a failure occurs, the
process known as garbage collection reclaims the memory used by these redundant
objects and the allocation is then tried again - a process which can take several
milliseconds.

There may be benefits in pre-empting this by periodically issuing `gc.collect()`.
Firstly doing a collection before it is actually required is quicker - typically on the
order of 1ms if done frequently. Secondly you can determine the point in code
where this time is used rather than have a longer delay occur at random points,
possibly in a speed critical section. Finally performing collections regularly
can reduce fragmentation in the heap. Severe fragmentation can lead to
non-recoverable allocation failures.

The Native code emitter
-----------------------

This causes the MicroPython compiler to emit native CPU opcodes rather than
bytecode. It covers the bulk of the MicroPython functionality, so most functions will require
no adaptation (but see below). It is invoked by means of a function decorator:

.. code:: python

    @micropython.native
    def foo(self, arg):
        buf = self.linebuf # Cached object
        # code

There are certain limitations in the current implementation of the native code emitter. 

* Context managers are not supported (the ``with`` statement).
* Generators are not supported.
* If ``raise`` is used an argument must be supplied.

The trade-off for the improved performance (roughly twice as fast as bytecode) is an
increase in compiled code size.

The Viper code emitter
----------------------

The optimisations discussed above involve standards-compliant Python code. The 
Viper code emitter is not fully compliant. It supports special Viper native data types
in pursuit of performance. Integer processing is non-compliant because it uses machine
words: arithmetic on 32 bit hardware is performed modulo 2**32.

Like the Native emitter Viper produces machine instructions but further optimisations
are performed, substantially increasing performance especially for integer arithmetic and
bit manipulations. It is invoked using a decorator:

.. code:: python

    @micropython.viper
    def foo(self, arg: int) -> int:
        # code

As the above fragment illustrates it is beneficial to use Python type hints to assist the Viper optimiser. 
Type hints provide information on the data types of arguments and of the return value; these
are a standard Python language feature formally defined here `PEP0484 <https://www.python.org/dev/peps/pep-0484/>`_.
Viper supports its own set of types namely ``int``, ``uint`` (unsigned integer), ``ptr``, ``ptr8``,
``ptr16`` and ``ptr32``. The ``ptrX`` types are discussed below. Currently the ``uint`` type serves
a single purpose: as a type hint for a function return value. If such a function returns ``0xffffffff``
Python will interpret the result as 2**32 -1 rather than as -1.

In addition to the restrictions imposed by the native emitter the following constraints apply:

* Functions may have up to four arguments.
* Default argument values are not permitted.
* Floating point may be used but is not optimised.

Viper provides pointer types to assist the optimiser. These comprise

* ``ptr`` Pointer to an object.
* ``ptr8`` Points to a byte.
* ``ptr16`` Points to a 16 bit half-word.
* ``ptr32`` Points to a 32 bit machine word.

The concept of a pointer may be unfamiliar to Python programmers. It has similarities
to a Python `memoryview` object in that it provides direct access to data stored in memory.
Items are accessed using subscript notation, but slices are not supported: a pointer can return
a single item only. Its purpose is to provide fast random access to data stored in contiguous
memory locations - such as data stored in objects which support the buffer protocol, and
memory-mapped peripheral registers in a microcontroller. It should be noted that programming
using pointers is hazardous: bounds checking is not performed and the compiler does nothing to
prevent buffer overrun errors.

Typical usage is to cache variables:

.. code:: python

    @micropython.viper
    def foo(self, arg: int) -> int:
        buf = ptr8(self.linebuf) # self.linebuf is a bytearray or bytes object
        for x in range(20, 30):
            bar = buf[x] # Access a data item through the pointer
            # code omitted

In this instance the compiler "knows" that ``buf`` is the address of an array of bytes;
it can emit code to rapidly compute the address of ``buf[x]`` at runtime. Where casts are
used to convert objects to Viper native types these should be performed at the start of
the function rather than in critical timing loops as the cast operation can take several
microseconds. The rules for casting are as follows:

* Casting operators are currently: ``int``, ``bool``, ``uint``, ``ptr``, ``ptr8``, ``ptr16`` and ``ptr32``.
* The result of a cast will be a native Viper variable.
* Arguments to a cast can be a Python object or a native Viper variable.
* If argument is a native Viper variable, then cast is a no-op (i.e. costs nothing at runtime)
  that just changes the type (e.g. from ``uint`` to ``ptr8``) so that you can then store/load
  using this pointer.
* If the argument is a Python object and the cast is ``int`` or ``uint``, then the Python object
  must be of integral type and the value of that integral object is returned.
* The argument to a bool cast must be integral type (boolean or integer); when used as a return
  type the viper function will return True or False objects.
* If the argument is a Python object and the cast is ``ptr``, ``ptr``, ``ptr16`` or ``ptr32``,
  then the Python object must either have the buffer protocol (in which case a pointer to the
  start of the buffer is returned) or it must be of integral type (in which case the value of
  that integral object is returned).

Writing to a pointer which points to a read-only object will lead to undefined behaviour.

The following example illustrates the use of a ``ptr16`` cast to toggle pin X1 ``n`` times:

.. code:: python

    BIT0 = const(1)
    @micropython.viper
    def toggle_n(n: int):
        odr = ptr16(stm.GPIOA + stm.GPIO_ODR)
        for _ in range(n):
            odr[0] ^= BIT0

A detailed technical description of the three code emitters may be found
on Kickstarter here `Note 1 <https://www.kickstarter.com/projects/214379695/micro-python-python-for-microcontrollers/posts/664832>`_
and here `Note 2 <https://www.kickstarter.com/projects/214379695/micro-python-python-for-microcontrollers/posts/665145>`_

Accessing hardware directly
---------------------------

.. note::

    Code examples in this section are given for the Pyboard. The techniques
    described however may be applied to other MicroPython ports too.

This comes into the category of more advanced programming and involves some knowledge
of the target MCU. Consider the example of toggling an output pin on the Pyboard. The
standard approach would be to write

.. code:: python

    mypin.value(mypin.value() ^ 1) # mypin was instantiated as an output pin

This involves the overhead of two calls to the :class:`~machine.Pin` instance's :meth:`~machine.Pin.value()`
method. This overhead can be eliminated by performing a read/write to the relevant bit
of the chip's GPIO port output data register (odr). To facilitate this the ``stm``
module provides a set of constants providing the addresses of the relevant registers.
A fast toggle of pin ``P4`` (CPU pin ``A14``) - corresponding to the green LED -
can be performed as follows:

.. code:: python

    import machine
    import stm

    BIT14 = const(1 << 14)
    machine.mem16[stm.GPIOA + stm.GPIO_ODR] ^= BIT14
