:mod:`micropython` -- access and control MicroPython internals
==============================================================

.. module:: micropython
   :synopsis: access and control MicroPython internals

Functions
---------

.. function:: const(expr)

   Declares a compile-time constant for bytecode optimization. The compiler performs
   constant folding, replacing all references to the constant with its literal value
   in the bytecode, eliminating runtime dictionary lookups.

   **Usage**::

    from micropython import const

    CONST_X = const(123)
    CONST_Y = const(2 * CONST_X + 1)

   **How it works:** This function is recognized directly by the MicroPython parser
   as a builtin optimization. During compilation, when the parser sees the pattern
   ``IDENTIFIER = const(EXPR)``, it evaluates the expression at compile time and
   substitutes the constant value directly at each usage site. For example,
   ``print(CONST_X)`` compiles to the equivalent of ``print(123)`` with the value
   embedded in the bytecode. No import is required in MicroPython.

   **Important:** The parser only recognizes the bare name ``const``. Using
   ``micropython.const()`` (with module prefix) or an aliased import (e.g.,
   ``from micropython import const as c``) will not trigger the optimization
   because the parser specifically looks for the unqualified name ``const`` in the
   pattern ``IDENTIFIER = const(EXPR)``.

   **Scope:** Constants should only be declared at module level (global scope).
   While ``const()`` can technically be used inside functions or classes, this
   creates module-level constants regardless of the syntactic location, violating
   normal Python scoping rules and causing confusing behavior. Always use ``const()``
   at module level.

   **Storage and visibility:**

   By default (e.g. ``X = const(1)``), even though the constant value is inlined at
   all usage sites within the defining module, the variable ``X`` is still created as
   a global variable in the module's dictionary. This means the variable can be
   accessed from other modules (e.g. ``import mymodule; print(mymodule.X)``). Other
   modules can even reassign this global variable, but this does not affect the
   inlined constant values within the defining module where ``const()`` replaced all
   references with the literal value.

   This default behavior occupies at least 2 machine words of RAM for the dictionary
   entry. To save this RAM, prefix the variable name with an underscore (e.g.
   ``_X = const(1)``). This prevents the variable from being stored in the global
   dictionary, making it inaccessible from other modules while still inlining the
   value at all usage sites within the defining module. Use this for internal
   constants.

   **Valid expressions:** The expression must be evaluable at compile time. Supported
   types include integers, floats, strings, bytes, booleans (``True``, ``False``),
   ``None``, ellipsis (``...``), and tuples of constants (if ``MICROPY_COMP_CONST_TUPLE``
   is enabled). The expression can use arithmetic, bitwise, and logical operators, and
   can reference other already-defined constants. Using runtime variables or function
   calls raises ``SyntaxError: not a constant``.

   Examples of valid expressions::

    BUFFER_SIZE = const(1024)
    BUFFER_MASK = const(BUFFER_SIZE - 1)
    FLAGS = const(0x1 | 0x2)
    REGISTER_ADDR = const(0x40000000)
    _INTERNAL_MAX = const(100)

   **Limitations:**

   - Cannot be redefined within the same module
   - Does not enforce immutability (constants can be reassigned from other modules);
     for stricter immutability checking, consider using ``typing.Final`` with static
     type checkers
   - Currently incompatible with type hints: ``X: int = const(1)`` is not optimized
   - Only substitutes standalone identifier references; does not substitute when the
     identifier appears after a dot (``obj.CONST``), as a function/class name, as a
     parameter name, or on the left side of an assignment

   **CPython compatibility:** While MicroPython recognizes ``const`` as a builtin
   parser optimization, it is also exported from the :mod:`micropython` module (as
   an identity function) to allow scripts to run on CPython without errors. The
   typical pattern for cross-platform compatibility is::

    try:
        from micropython import const
    except ImportError:
        def const(x):
            return x

.. function:: opt_level([level])

   If *level* is given then this function sets the optimisation level for subsequent
   compilation of scripts, and returns ``None``.  Otherwise it returns the current
   optimisation level.

   The optimisation level controls the following compilation features:

   - Assertions: at level 0 assertion statements are enabled and compiled into the
     bytecode; at levels 1 and higher assertions are not compiled.
   - Built-in ``__debug__`` variable: at level 0 this variable expands to ``True``;
     at levels 1 and higher it expands to ``False``.
   - Source-code line numbers: at levels 0, 1 and 2 source-code line number are
     stored along with the bytecode so that exceptions can report the line number
     they occurred at; at levels 3 and higher line numbers are not stored.

   The default optimisation level is usually level 0.

.. function:: alloc_emergency_exception_buf(size)

   Allocate *size* bytes of RAM for the emergency exception buffer (a good
   size is around 100 bytes).  The buffer is used to create exceptions in cases
   when normal RAM allocation would fail (eg within an interrupt handler) and
   therefore give useful traceback information in these situations.

   A good way to use this function is to put it at the start of your main script
   (eg ``boot.py`` or ``main.py``) and then the emergency exception buffer will be active
   for all the code following it.

.. function:: mem_info([verbose])

   Print information about currently used memory.  If the *verbose* argument
   is given then extra information is printed.

   The information that is printed is implementation dependent, but currently
   includes the amount of stack and heap used.  In verbose mode it prints out
   the entire heap indicating which blocks are used and which are free.

.. function:: qstr_info([verbose])

   Print information about currently interned strings.  If the *verbose*
   argument is given then extra information is printed.

   The information that is printed is implementation dependent, but currently
   includes the number of interned strings and the amount of RAM they use.  In
   verbose mode it prints out the names of all RAM-interned strings.

.. function:: stack_use()

   Return an integer representing the current amount of stack that is being
   used.  The absolute value of this is not particularly useful, rather it
   should be used to compute differences in stack usage at different points.

.. function:: heap_lock()
.. function:: heap_unlock()
.. function:: heap_locked()

   Lock or unlock the heap.  When locked no memory allocation can occur and a
   `MemoryError` will be raised if any heap allocation is attempted.
   `heap_locked()` returns a true value if the heap is currently locked.

   These functions can be nested, ie `heap_lock()` can be called multiple times
   in a row and the lock-depth will increase, and then `heap_unlock()` must be
   called the same number of times to make the heap available again.

   Both `heap_unlock()` and `heap_locked()` return the current lock depth
   (after unlocking for the former) as a non-negative integer, with 0 meaning
   the heap is not locked.

   If the REPL becomes active with the heap locked then it will be forcefully
   unlocked.

   Note: `heap_locked()` is not enabled on most ports by default,
   requires ``MICROPY_PY_MICROPYTHON_HEAP_LOCKED``.

.. function:: kbd_intr(chr)

   Set the character that will raise a `KeyboardInterrupt` exception.  By
   default this is set to 3 during script execution, corresponding to Ctrl-C.
   Passing -1 to this function will disable capture of Ctrl-C, and passing 3
   will restore it.

   This function can be used to prevent the capturing of Ctrl-C on the
   incoming stream of characters that is usually used for the REPL, in case
   that stream is used for other purposes.

.. function:: schedule(func, arg)

   Schedule the function *func* to be executed "very soon".  The function
   is passed the value *arg* as its single argument.  "Very soon" means that
   the MicroPython runtime will do its best to execute the function at the
   earliest possible time, given that it is also trying to be efficient, and
   that the following conditions hold:

   - A scheduled function will never preempt another scheduled function.
   - Scheduled functions are always executed "between opcodes" which means
     that all fundamental Python operations (such as appending to a list)
     are guaranteed to be atomic.
   - A given port may define "critical regions" within which scheduled
     functions will never be executed.  Functions may be scheduled within
     a critical region but they will not be executed until that region
     is exited.  An example of a critical region is a preempting interrupt
     handler (an IRQ).

   A use for this function is to schedule a callback from a preempting IRQ.
   Such an IRQ puts restrictions on the code that runs in the IRQ (for example
   the heap may be locked) and scheduling a function to call later will lift
   those restrictions.

   On multi-threaded ports, the scheduled function's behaviour depends on
   whether the Global Interpreter Lock (GIL) is enabled for the specific port:

   - If GIL is enabled, the function can preempt any thread and run in its
     context.
   - If GIL is disabled, the function will only preempt the main thread and run
     in its context.

   Note: If `schedule()` is called from a preempting IRQ, when memory
   allocation is not allowed and the callback to be passed to `schedule()` is
   a bound method, passing this directly will fail. This is because creating a
   reference to a bound method causes memory allocation. A solution is to
   create a reference to the method in the class constructor and to pass that
   reference to `schedule()`. This is discussed in detail here
   :ref:`reference documentation <isr_rules>` under "Creation of Python
   objects".

   There is a finite queue to hold the scheduled functions and `schedule()`
   will raise a `RuntimeError` if the queue is full.

Classes
-------

.. class:: RingIO(size)
.. class:: RingIO(buffer)
   :noindex:

   Provides a fixed-size ringbuffer for bytes with a stream interface. Can be
   considered like a fifo queue variant of `io.BytesIO`.

   When created with integer size a suitable buffer will be allocated.
   Alternatively a `bytearray` or similar buffer protocol object can be provided
   to the constructor for in-place use.

   The classic ringbuffer algorithm is used which allows for any size buffer
   to be used however one byte will be consumed for tracking. If initialised
   with an integer size this will be accounted for, for example ``RingIO(16)``
   will allocate a 17 byte buffer internally so it can hold 16 bytes of data.
   When passing in a pre-allocated buffer however one byte less than its
   original length will be available for storage, eg. ``RingIO(bytearray(16))``
   will only hold 15 bytes of data.

   A RingIO instance can be IRQ / thread safe when used to pass data in a single
   direction eg. when written to in an IRQ and read from in a non-IRQ function
   (or vice versa). This does not hold if you try to eg. write to a single instance
   from both IRQ and non-IRQ code, this would often cause data corruption.

    .. method:: RingIO.any()

        Returns an integer counting the number of characters that can be read.

    .. method:: RingIO.read([nbytes])

        Read available characters. This is a non-blocking function. If ``nbytes``
        is specified then read at most that many bytes, otherwise read as much
        data as possible.

        Return value: a bytes object containing the bytes read. Will be
        zero-length bytes object if no data is available.

    .. method:: RingIO.readline([nbytes])

        Read a line, ending in a newline character or return if one exists in
        the buffer, else return available bytes in buffer. If ``nbytes`` is
        specified then read at most that many bytes.

        Return value: a bytes object containing the line read.

    .. method:: RingIO.readinto(buf[, nbytes])

        Read available bytes into the provided ``buf``.  If ``nbytes`` is
        specified then read at most that many bytes.  Otherwise, read at
        most ``len(buf)`` bytes.

        Return value: Integer count of the number of bytes read into ``buf``.

    .. method:: RingIO.write(buf)

        Non-blocking write of bytes from ``buf`` into the ringbuffer, limited
        by the available space in the ringbuffer.

        Return value: Integer count of bytes written.

    .. method:: RingIO.close()

        No-op provided as part of standard `stream` interface. Has no effect
        on data in the ringbuffer.
