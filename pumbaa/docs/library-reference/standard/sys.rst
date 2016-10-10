:mod:`sys` --- System-specific parameters and functions
=======================================================

.. module:: sys
   :synopsis: System-specific parameters and functions.

This module provides access to some variables used or maintained by
the interpreter and to functions that interact strongly with the
interpreter. It is always available.

----------------------------------------------

.. function:: sys.exit([arg])

   Exit from Python. This is implemented by raising the SystemExit
   exception, so cleanup actions specified by finally clauses of try
   statements are honored, and it is possible to intercept the exit
   attempt at an outer level.


.. function:: sys.print_exception()


.. data:: sys.path

   A list of strings that specifies the search path for modules.


.. data:: sys.argv

   The list of command line arguments passed to a Python
   script. ``argv[0]`` is the script name (it is operating system
   dependent whether this is a full pathname or not).


.. data:: sys.version

   A string containing the version number of the Python
   interpreter. Do not extract version information out of it, rather,
   use `version_info` and the functions provided by the platform
   module.


.. data:: sys.version_info

   A tuple containing the three components of the version number:
   major, minor and micro. The `version_info` value corresponding to
   the Python version 3.4 is (3, 4, 0).


.. data:: sys.implementation

   An object containing information about the implementation of the
   currently running Python interpreter. The following attributes are
   required to exist in all Python implementations.

   `name` is the implementation’s identifier, e.g. 'micropython'. The
   actual string is defined by the Python implementation, but it is
   guaranteed to be lower case.

   `version` is a named tuple, in the same format as
   `sys.version_info`. It represents the version of the Python
   implementation. This has a distinct meaning from the specific
   version of the Python language to which the currently running
   interpreter conforms, which `sys.version_info` represents. For
   example, for Micropython 1.8 `sys.implementation` might be `(1, 8,
   0)`, whereas `sys.version_info` would be `(3, 4, 0)`.


.. data:: sys.platform

   This string contains a platform identifier that can be used to
   append platform-specific components to `sys.path`, for instance.


.. data:: sys.byteorder

   An indicator of the native byte order. This will have the value
   ``'big'`` on big-endian (most-significant byte first) platforms,
   and ``'little'`` on little-endian (least-significant byte first)
   platforms.


.. data:: sys.modules

   This is a dictionary that maps module names to modules which have
   already been loaded. This can be manipulated to force reloading of
   modules and other tricks. However, replacing the dictionary will
   not necessarily work as expected and deleting essential items from
   the dictionary may cause Python to fail.
