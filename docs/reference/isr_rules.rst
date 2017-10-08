.. _isr_rules:

Writing interrupt handlers
==========================

On suitable hardware MicroPython offers the ability to write interrupt handlers in Python. Interrupt handlers
- also known as interrupt service routines (ISR's) - are defined as callback functions. These are executed
in response to an event such as a timer trigger or a voltage change on a pin. Such events can occur at any point
in the execution of the program code. This carries significant consequences, some specific to the MicroPython
language. Others are common to all systems capable of responding to real time events. This document covers
the language specific issues first, followed by a brief introduction to real time programming for those new to it.

This introduction uses vague terms like "slow" or "as fast as possible". This is deliberate, as speeds are
application dependent. Acceptable durations for an ISR are dependent on the rate at which interrupts occur,
the nature of the main program, and the presence of other concurrent events.

Tips and recommended practices
------------------------------

This summarises the points detailed below and lists the principal recommendations for interrupt handler code.

* Keep the code as short and simple as possible.
* Avoid memory allocation: no appending to lists or insertion into dictionaries, no floating point.
* Consider using ``micropython.schedule`` to work around the above constraint.
* Where an ISR returns multiple bytes use a pre-allocated ``bytearray``. If multiple integers are to be
  shared between an ISR and the main program consider an array (``array.array``).
* Where data is shared between the main program and an ISR, consider disabling interrupts prior to accessing
  the data in the main program and re-enabling them immediately afterwards (see Critical Sections).
* Allocate an emergency exception buffer (see below).


MicroPython Issues
------------------

The emergency exception buffer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If an error occurs in an ISR, MicroPython is unable to produce an error report unless a special buffer is created
for the purpose. Debugging is simplified if the following code is included in any program using interrupts.

.. code:: python

    import micropython
    micropython.alloc_emergency_exception_buf(100)

Simplicity
~~~~~~~~~~

For a variety of reasons it is important to keep ISR code as short and simple as possible. It should do only what
has to be done immediately after the event which caused it: operations which can be deferred should be delegated
to the main program loop. Typically an ISR will deal with the hardware device which caused the interrupt, making
it ready for the next interrupt to occur. It will communicate with the main loop by updating shared data to indicate
that the interrupt has occurred, and it will return. An ISR should return control to the main loop as quickly
as possible. This is not a specific MicroPython issue so is covered in more detail :ref:`below <ISR>`.

Communication between an ISR and the main program
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Normally an ISR needs to communicate with the main program. The simplest means of doing this is via one or more
shared data objects, either declared as global or shared via a class (see below). There are various restrictions
and hazards around doing this, which are covered in more detail below. Integers, ``bytes`` and ``bytearray`` objects
are commonly used for this purpose along with arrays (from the array module) which can store various data types.

The use of object methods as callbacks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

MicroPython supports this powerful technique which enables an ISR to share instance variables with the underlying
code. It also enables a class implementing a device driver to support multiple device instances. The following
example causes two LED's to flash at different rates.

.. code:: python

    import pyb, micropython
    micropython.alloc_emergency_exception_buf(100)
    class Foo(object):
        def __init__(self, timer, led):
            self.led = led
            timer.callback(self.cb)
        def cb(self, tim):
            self.led.toggle()

    red = Foo(pyb.Timer(4, freq=1), pyb.LED(1))
    greeen = Foo(pyb.Timer(2, freq=0.8), pyb.LED(2))

In this example the ``red`` instance associates timer 4 with LED 1: when a timer 4 interrupt occurs ``red.cb()``
is called causing LED 1 to change state. The ``green`` instance operates similarly: a timer 2 interrupt
results in the execution of ``green.cb()`` and toggles LED 2. The use of instance methods confers two
benefits. Firstly a single class enables code to be shared between multiple hardware instances. Secondly, as
a bound method the callback function's first argument is ``self``. This enables the callback to access instance
data and to save state between successive calls. For example, if the class above had a variable ``self.count``
set to zero in the constructor, ``cb()`` could increment the counter. The ``red`` and ``green`` instances would
then maintain independent counts of the number of times each LED had changed state.

Creation of Python objects
~~~~~~~~~~~~~~~~~~~~~~~~~~

ISR's cannot create instances of Python objects. This is because MicroPython needs to allocate memory for the
object from a store of free memory block called the heap. This is not permitted in an interrupt handler because
heap allocation is not re-entrant. In other words the interrupt might occur when the main program is part way
through performing an allocation - to maintain the integrity of the heap the interpreter disallows memory
allocations in ISR code.

A consequence of this is that ISR's can't use floating point arithmetic; this is because floats are Python objects. Similarly
an ISR can't append an item to a list. In practice it can be hard to determine exactly which code constructs will
attempt to perform memory allocation and provoke an error message: another reason for keeping ISR code short and simple.

One way to avoid this issue is for the ISR to use pre-allocated buffers. For example a class constructor
creates a ``bytearray`` instance and a boolean flag. The ISR method assigns data to locations in the buffer and sets
the flag. The memory allocation occurs in the main program code when the object is instantiated rather than in the ISR.

The MicroPython library I/O methods usually provide an option to use a pre-allocated buffer. For
example ``pyb.i2c.recv()`` can accept a mutable buffer as its first argument: this enables its use in an ISR.

A means of creating an object without employing a class or globals is as follows:

.. code:: python

    def set_volume(t, buf=bytearray(3)):
        buf[0] = 0xa5
        buf[1] = t >> 4
        buf[2] = 0x5a
        return buf

The compiler instantiates the default ``buf`` argument when the function is
loaded for the first time (usually when the module it's in is imported).

Use of Python objects
~~~~~~~~~~~~~~~~~~~~~

A further restriction on objects arises because of the way Python works. When an ``import`` statement is executed the
Python code is compiled to bytecode, with one line of code typically mapping to multiple bytecodes. When the code
runs the interpreter reads each bytecode and executes it as a series of machine code instructions. Given that an
interrupt can occur at any time between machine code instructions, the original line of Python code may be only
partially executed. Consequently a Python object such as a set, list or dictionary modified in the main loop
may lack internal consistency at the moment the interrupt occurs.

A typical outcome is as follows. On rare occasions the ISR will run at the precise moment in time when the object
is partially updated. When the ISR tries to read the object, a crash results. Because such problems typically occur
on rare, random occasions they can be hard to diagnose. There are ways to circumvent this issue, described in
:ref:`Critical Sections <Critical>` below.

It is important to be clear about what constitutes the modification of an object. An alteration to a built-in type
such as a dictionary is problematic. Altering the contents of an array or bytearray is not. This is because bytes
or words are written as a single machine code instruction which is not interruptible: in the parlance of real time
programming the write is atomic. A user defined object might instantiate an integer, array or bytearray. It is valid
for both the main loop and the ISR to alter the contents of these.

MicroPython supports integers of arbitrary precision. Values between 2**30 -1 and -2**30 will be stored in
a single machine word. Larger values are stored as Python objects. Consequently changes to long integers cannot
be considered atomic. The use of long integers in ISR's is unsafe because memory allocation may be
attempted as the variable's value changes.

Overcoming the float limitation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In general it is best to avoid using floats in ISR code: hardware devices normally handle integers and conversion
to floats is normally done in the main loop. However there are a few DSP algorithms which require floating point.
On platforms with hardware floating point (such as the Pyboard) the inline ARM Thumb assembler can be used to work
round this limitation. This is because the processor stores float values in a machine word; values can be shared
between the ISR and main program code via an array of floats.

Using micropython.schedule
~~~~~~~~~~~~~~~~~~~~~~~~~~

This function enables an ISR to schedule a callback for execution "very soon". The callback is queued for
execution which will take place at a time when the heap is not locked. Hence it can create Python objects
and use floats. The callback is also guaranteed to run at a time when the main program has completed any
update of Python objects, so the callback will not encounter partially updated objects.

Typical usage is to handle sensor hardware. The ISR acquires data from the hardware and enables it to
issue a further interrupt. It then schedules a callback to process the data.

Scheduled callbacks should comply with the principles of interrupt handler design outlined below. This is to
avoid problems resulting from I/O activity and the modification of shared data which can arise in any code
which pre-empts the main program loop.

Execution time needs to be considered in relation to the frequency with which interrupts can occur. If an
interrupt occurs while the previous callback is executing, a further instance of the callback will be queued
for execution; this will run after the current instance has completed. A sustained high interrupt repetition
rate therefore carries a risk of unconstrained queue growth and eventual failure with a ``RuntimeError``.

Exceptions
----------

If an ISR raises an exception it will not propagate to the main loop. The interrupt will be disabled unless the
exception is handled by the ISR code.

General Issues
--------------

This is merely a brief introduction to the subject of real time programming. Beginners should note
that design errors in real time programs can lead to faults which are particularly hard to diagnose. This is because
they can occur rarely and at intervals which are essentially random. It is crucial to get the initial design right and
to anticipate issues before they arise. Both interrupt handlers and the main program need to be designed
with an appreciation of the following issues.

.. _ISR:

Interrupt Handler Design
~~~~~~~~~~~~~~~~~~~~~~~~

As mentioned above, ISR's should be designed to be as simple as possible. They should always return in a short,
predictable period of time. This is important because when the ISR is running, the main loop is not: inevitably
the main loop experiences pauses in its execution at random points in the code. Such pauses can be a source of hard
to diagnose bugs particularly if their duration is long or variable. In order to understand the implications of
ISR run time, a basic grasp of interrupt priorities is required.

Interrupts are organised according to a priority scheme. ISR code may itself be interrupted by a higher priority
interrupt. This has implications if the two interrupts share data (see Critical Sections below). If such an interrupt
occurs it interposes a delay into the ISR code. If a lower priority interrupt occurs while the ISR is running, it
will be delayed until the ISR is complete: if the delay is too long, the lower priority interrupt may fail. A
further issue with slow ISR's is the case where a second interrupt of the same type occurs during its execution.
The second interrupt will be handled on termination of the first. However if the rate of incoming interrupts
consistently exceeds the capacity of the ISR to service them the outcome will not be a happy one.

Consequently looping constructs should be avoided or minimised. I/O to devices other than to the interrupting device
should normally be avoided: I/O such as disk access, ``print`` statements and UART access is relatively slow, and
its duration may vary. A further issue here is that filesystem functions are not reentrant: using filesystem I/O
in an ISR and the main program would be hazardous. Crucially ISR code should not wait on an event. I/O is acceptable
if the code can be guaranteed to return in a predictable period, for example toggling a pin or LED. Accessing the
interrupting device via I2C or SPI may be necessary but the time taken for such accesses should be calculated or
measured and its impact on the application assessed.

There is usually a need to share data between the ISR and the main loop. This may be done either through global
variables or via class or instance variables. Variables are typically integer or boolean types, or integer or byte
arrays (a pre-allocated integer array offers faster access than a list). Where multiple values are modified by
the ISR it is necessary to consider the case where the interrupt occurs at a time when the main program has
accessed some, but not all, of the values. This can lead to inconsistencies.

Consider the following design. An ISR stores incoming data in a bytearray, then adds the number of bytes
received to an integer representing total bytes ready for processing. The main program reads the number of bytes,
processes the bytes, then clears down the number of bytes ready. This will work until an interrupt occurs just
after the main program has read the number of bytes. The ISR puts the added data into the buffer and updates
the number received, but the main program has already read the number, so processes the data originally received.
The newly arrived bytes are lost.

There are various ways of avoiding this hazard, the simplest being to use a circular buffer. If it is not possible
to use a structure with inherent thread safety other ways are described below.

Reentrancy
~~~~~~~~~~

A potential hazard may occur if a function or method is shared between the main program and one or more ISR's or
between multiple ISR's. The issue here is that the function may itself be interrupted and a further instance of
that function run. If this is to occur, the function must be designed to be reentrant. How this is done is an
advanced topic beyond the scope of this tutorial.

.. _Critical:

Critical Sections
~~~~~~~~~~~~~~~~~

An example of a critical section of code is one which accesses more than one variable which can be affected by an ISR. If
the interrupt happens to occur between accesses to the individual variables, their values will be inconsistent. This is
an instance of a hazard known as a race condition: the ISR and the main program loop race to alter the variables. To
avoid inconsistency a means must be employed to ensure that the ISR does not alter the values for the duration of
the critical section. One way to achieve this is to issue ``pyb.disable_irq()`` before the start of the section, and
``pyb.enable_irq()`` at the end. Here is an example of this approach:

.. code:: python

    import pyb, micropython, array
    micropython.alloc_emergency_exception_buf(100)

    class BoundsException(Exception):
        pass

    ARRAYSIZE = const(20)
    index = 0
    data = array.array('i', 0 for x in range(ARRAYSIZE))

    def callback1(t):
        global data, index
        for x in range(5):
            data[index] = pyb.rng() # simulate input
            index += 1
            if index >= ARRAYSIZE:
                raise BoundsException('Array bounds exceeded')

    tim4 = pyb.Timer(4, freq=100, callback=callback1)

    for loop in range(1000):
        if index > 0:
            irq_state = pyb.disable_irq() # Start of critical section
            for x in range(index):
                print(data[x])
            index = 0
            pyb.enable_irq(irq_state) # End of critical section
            print('loop {}'.format(loop))
        pyb.delay(1)

    tim4.callback(None)

A critical section can comprise a single line of code and a single variable. Consider the following code fragment.

.. code:: python

    count = 0
    def cb(): # An interrupt callback
        count +=1
    def main():
        # Code to set up the interrupt callback omitted
        while True:
            count += 1

This example illustrates a subtle source of bugs. The line ``count += 1`` in the main loop carries a specific race
condition hazard known as a read-modify-write. This is a classic cause of bugs in real time systems. In the main loop
MicroPython reads the value of ``t.counter``, adds 1 to it, and writes it back. On rare occasions the  interrupt occurs
after the read and before the write. The interrupt modifies ``t.counter`` but its change is overwritten by the main
loop when the ISR returns. In a real system this could lead to rare, unpredictable failures.

As mentioned above, care should be taken if an instance of a Python built in type is modified in the main code and
that instance is accessed in an ISR. The code performing the modification should be regarded as a critical
section to ensure that the instance is in a valid state when the ISR runs.

Particular care needs to be taken if a dataset is shared between different ISR's. The hazard here is that the higher
priority interrupt may occur when the lower priority one has partially updated the shared data. Dealing with this
situation is an advanced topic beyond the scope of this introduction other than to note that mutex objects described
below can sometimes be used.

Disabling interrupts for the duration of a critical section is the usual and simplest way to proceed, but it disables
all interrupts rather than merely the one with the potential to cause problems. It is generally undesirable to disable
an interrupt for long. In the case of timer interrupts it introduces variability to the time when a callback occurs.
In the case of device interrupts, it can lead to the device being serviced too late with possible loss of data or
overrun errors in the device hardware. Like ISR's, a critical section in the main code should have a short, predictable
duration.

An approach to dealing with critical sections which radically reduces the time for which interrupts are disabled is to
use an object termed a mutex (name derived from the notion of mutual exclusion). The main program locks the mutex
before running the critical section and unlocks it at the end. The ISR tests whether the mutex is locked. If it is,
it avoids the critical section and returns. The design challenge is defining what the ISR should do in the event
that access to the critical variables is denied. A simple example of a mutex may be found
`here <https://github.com/peterhinch/micropython-samples.git>`_. Note that the mutex code does disable interrupts,
but only for the duration of eight machine instructions: the benefit of this approach is that other interrupts are
virtually unaffected.

Interrupts and the REPL
~~~~~~~~~~~~~~~~~~~~~~~

Interrupt handlers, such as those associated with timers, can continue to run
after a program terminates.  This may produce unexpected results where you might
have expected the object raising the callback to have gone out of scope.  For
example on the Pyboard:

.. code:: python

    def bar():
        foo = pyb.Timer(2, freq=4, callback=lambda t: print('.', end=''))

    bar()

This continues to run until the timer is explicitly disabled or the board is
reset with ``ctrl D``.
