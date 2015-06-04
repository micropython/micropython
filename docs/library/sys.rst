:mod:`sys` -- system specific functions
=======================================

.. module:: sys
   :synopsis: system specific functions

Functions
---------

.. function:: exit([retval])

   Raise a ``SystemExit`` exception.  If an argument is given, it is the
   value given to ``SystemExit``.

.. function:: print_exception(exc, [file])

   Print exception with a traceback to a file-like object ``file`` (or
   ``sys.stdout`` by default).

   .. admonition:: Difference to CPython
      :class: attention

      This function appears in the ``traceback`` module in CPython.

Constants
---------

.. data:: argv

   a mutable list of arguments this program started with

.. data:: byteorder

   the byte order of the system ("little" or "big")

.. data:: path

   a mutable list of directories to search for imported modules

.. data:: platform

   The platform that MicroPython is running on.  This is "pyboard" on the
   pyboard and provides a robust way of determining if a script is running
   on the pyboard or not.

.. data:: stderr

   standard error (connected to USB VCP, and optional UART object)

.. data:: stdin

   standard input (connected to USB VCP, and optional UART object)

.. data:: stdout

   standard output (connected to USB VCP, and optional UART object)

.. data:: version

   Python language version that this implementation conforms to, as a string

.. data:: version_info

   Python language version that this implementation conforms to, as a tuple of ints
