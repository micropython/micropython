:mod:`sys` -- system specific functions
=======================================

.. module:: sys
   :synopsis: system specific functions

|see_cpython_module| :mod:`python:sys`.

Functions
---------

.. function:: exit(retval=0, /)

   Terminate current program with a given exit code. Underlyingly, this
   function raises a `SystemExit` exception. If an argument is given, its
   value given as an argument to `SystemExit`.

   On embedded ports (i.e. all ports but Windows and Unix), an unhandled
   `SystemExit` currently causes a :ref:`soft_reset` of MicroPython.

.. function:: atexit(func)

   Register *func* to be called upon termination.  *func* must be a callable
   that takes no arguments, or ``None`` to disable the call.  The ``atexit``
   function will return the previous value set by this function, which is
   initially ``None``.

   .. admonition:: Difference to CPython
      :class: attention

      This function is a MicroPython extension intended to provide similar
      functionality to the :mod:`atexit` module in CPython.

.. function:: print_exception(exc, file=sys.stdout, /)

   Print exception with a traceback to a file-like object *file* (or
   `sys.stdout` by default).

   .. admonition:: Difference to CPython
      :class: attention

      This is simplified version of a function which appears in the
      ``traceback`` module in CPython. Unlike ``traceback.print_exception()``,
      this function takes just exception value instead of exception type,
      exception value, and traceback object; *file* argument should be
      positional; further arguments are not supported. CPython-compatible
      ``traceback`` module can be found in `micropython-lib`.

.. function:: settrace(tracefunc)

   Enable tracing of bytecode execution.  For details see the `CPython
   documentation <https://docs.python.org/3/library/sys.html#sys.settrace>`_.

   This function requires a custom MicroPython build as it is typically not
   present in pre-built firmware (due to it affecting performance).  The relevant
   configuration option is *MICROPY_PY_SYS_SETTRACE*.

Constants
---------

.. data:: argv

   A mutable list of arguments the current program was started with.

.. data:: byteorder

   The byte order of the system (``"little"`` or ``"big"``).

.. data:: implementation

   Object with information about the current Python implementation. For
   MicroPython, it has following attributes:

   * *name* - string "micropython"
   * *version* - tuple (major, minor, micro, releaselevel), e.g. (1, 22, 0, '')
   * *_machine* - string describing the underlying machine
   * *_mpy* - supported mpy file-format version (optional attribute)
   * *_build* - string that can help identify the configuration that
     MicroPython was built with
   * *_thread* - optional string attribute, exists if the target has threading
     and is either "GIL" or "unsafe"

   This object is the recommended way to distinguish MicroPython from other
   Python implementations (note that it still may not exist in the very
   minimal ports).

   Starting with version 1.22.0-preview, the fourth node *releaselevel* in
   *implementation.version* is either an empty string or ``"preview"``.

   The *_build* entry was added in version 1.25.0 and is a hyphen-separated
   set of elements.  New elements may be appended in the future so it's best to
   access this field using ``sys.implementation._build.split("-")``.  The
   elements that are currently used are:

   * On the unix, webassembly and windows ports the first element is the variant
     name, for example ``'standard'``.
   * On microcontroller targets, the first element is the board name and the second
     element (if present) is the board variant, for example ``'RPI_PICO2-RISCV'``

   The *_thread* entry was added in version 1.26.0 and if it exists then the
   target has the ``_thread`` module.  If the target enables the GIL (global
   interpreter lock) then this attribute is ``"GIL"``.  Otherwise the attribute
   is ``"unsafe"`` and the target has threading but does not enable the GIL,
   and mutable Python objects (such as `bytearray`, `list` and `dict`) that are
   shared amongst threads must be protected explicitly by locks such as
   ``_thread.allocate_lock``.

   .. admonition:: Difference to CPython
      :class: attention

      CPython mandates more attributes for this object, but the actual useful
      bare minimum is implemented in MicroPython.

.. data:: maxsize

   Maximum value which a native integer type can hold on the current platform,
   or maximum value representable by MicroPython integer type, if it's smaller
   than platform max value (that is the case for MicroPython ports without
   long int support).

   This attribute is useful for detecting "bitness" of a platform (32-bit vs
   64-bit, etc.). It's recommended to not compare this attribute to some
   value directly, but instead count number of bits in it::

    bits = 0
    v = sys.maxsize
    while v:
        bits += 1
        v >>= 1
    if bits > 32:
        # 64-bit (or more) platform
        ...
    else:
        # 32-bit (or less) platform
        # Note that on 32-bit platform, value of bits may be less than 32
        # (e.g. 31) due to peculiarities described above, so use "> 16",
        # "> 32", "> 64" style of comparisons.

.. data:: modules

   Dictionary of loaded modules. On some ports, it may not include builtin
   modules.

.. data:: path

   A mutable list of directories to search for imported modules.

   .. admonition:: Difference to CPython
      :class: attention

      On MicroPython, an entry with the value ``".frozen"`` will indicate that import
      should search :term:`frozen modules <frozen module>` at that point in the search.
      If no frozen module is found then search will *not* look for a directory called
      ``.frozen``, instead it will continue with the next entry in ``sys.path``.

.. data:: platform

   The platform that MicroPython is running on. For OS/RTOS ports, this is
   usually an identifier of the OS, e.g. ``"linux"``. For baremetal ports it
   is an identifier of a board, e.g. ``"pyboard"`` for the original MicroPython
   reference board. It thus can be used to distinguish one board from another.
   If you need to check whether your program runs on MicroPython (vs other
   Python implementation), use `sys.implementation` instead.

.. data:: ps1
          ps2

   Mutable attributes holding strings, which are used for the REPL prompt.  The defaults
   give the standard Python prompt of ``>>>`` and ``...``.

.. data:: stderr

   Standard error `stream`.

.. data:: stdin

   Standard input `stream`.

.. data:: stdout

   Standard output `stream`.

.. data:: tracebacklimit

   A mutable attribute holding an integer value which is the maximum number of traceback
   entries to store in an exception.  Set to 0 to disable adding tracebacks.  Defaults
   to 1000.

   Note: this is not available on all ports.

.. data:: version

   Python language version that this implementation conforms to, as a string.

.. data:: version_info

   Python language version that this implementation conforms to, as a tuple of ints.

    .. admonition:: Difference to CPython
      :class: attention

      Only the first three version numbers (major, minor, micro) are supported and
      they can be referenced only by index, not by name.
