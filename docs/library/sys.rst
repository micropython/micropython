:mod:`sys` -- system specific functions
=======================================

.. module:: sys
   :synopsis: system specific functions

Functions
---------

.. function:: exit(retval=0)

   Terminate current program with a given exit code. Underlyingly, this
   function raise as ``SystemExit`` exception. If an argument is given, its
   value given as an argument to ``SystemExit``.

.. function:: print_exception(exc, file=sys.stdout)

   Print exception with a traceback to a file-like object `file` (or
   ``sys.stdout`` by default).

   .. admonition:: Difference to CPython
      :class: attention

      This function appears in the ``traceback`` module in CPython.

Constants
---------

.. data:: argv

   A mutable list of arguments the current program was started with.

.. data:: byteorder

   The byte order of the system ("little" or "big").

.. data:: implementation

   Object with information about the current Python implementation. For
   MicroPython, it has following attributes:

   * `name` - string "micropython"
   * `version` - tuple (major, minor, micro), e.g. (1, 7, 0)

   This object is the recommended way to distinguish MicroPython from other
   Python implementations (note that it still may not exist in the very
   minimal ports).

   .. admonition:: Difference to CPython
      :class: attention

      CPython mandates more attributes for this object, but the actual useful
      bare minimum is implemented in MicroPython.

.. data:: modules

   Dictionary of loaded modules. On some ports, it may not include builtin
   modules.

.. data:: path

   A mutable list of directories to search for imported modules.

.. data:: platform

   The platform that MicroPython is running on.  This is "pyboard" on the
   pyboard and provides a robust way of determining if a script is running
   on the pyboard or not.

.. data:: stderr

   Standard error stream.

.. data:: stdin

   Standard input stream.

.. data:: stdout

   Standard output stream.

.. data:: version

   Python language version that this implementation conforms to, as a string.

.. data:: version_info

   Python language version that this implementation conforms to, as a tuple of ints.
