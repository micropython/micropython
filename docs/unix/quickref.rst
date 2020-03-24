.. _unix_quickref:

Quick reference for the UNIX and Windows ports
==============================================

Command line options
--------------------

Usage::

    micropython [ -h ] [ -i ] [ -O<level> ] [ -v ] [ -X <option> ] [ -c <command> | -m <module> | <script> ] [ <args> ]


Invocation options:

.. option:: -c <command>

   Runs the code in ``<command>``. The code can be one or more Python statements.

.. option:: -m <module>

   Runs the module ``<module>``. The module must be in ``sys.path``.

.. option:: <script>

   Runs the file ``<script>``. The script must be a valid MicroPython source
   code file.

If none of the 3 options above are given, then MicroPython is run in an
interactive REPL mode.


.. option:: <args>

    Any additional arguments after the module or script will be passed to
    ``sys.argv`` (not supported with the :option:`-c` option).


General options:

.. option:: -h

    Prints a help message containing the command line usage and exits.

.. option:: -i

    Enables inspection. When this flag is set, MicroPython will enter the
    interactive REPL mode after the command, module or script has finished.
    This can be useful for debugging the state after an unhandled exception.
    Also see the :envvar:`MICROPYINSPECT` environment variable.

.. option:: -O | -O<level> | -OO...

    Sets the optimization level. The ``O`` can be followed by a number or can
    be repeated multiple times to indicate the level. E.g. ``-O3`` is the same
    as ``-OOO``.

.. option:: -v

    Increases the verbosity level. This option can be given multiple times.
    This option only has an effect if ``MICROPY_DEBUG_PRINTERS`` was enabled
    when MicroPython itself was compiled.

.. option:: -X <option>

    Specifies additional implementation-specific options. Possible options are:

    - ``-X compile-only`` compiles the command, module or script but does not
      run it.
    - ``-X emit={bytecode,native,viper}`` sets the default code emitter. Native
      emitters may not be available depending on the settings when MicroPython
      itself was compiled.
    - ``-X heapsize=<n>[w][K|M]`` sets the heap size for the garbage collector.
      The suffix ``w`` means words instead of bytes. ``K`` means x1024 and ``M``
      means x1024x1024.



Environment variables
---------------------

.. envvar:: MICROPYPATH

    Overrides the default search path for MicroPython libraries. ``MICROPYPATH``
    should be set to a colon (semicolon for Windows port) separated list of
    directories. If ``MICROPYPATH`` is not defined, the search path will be
    ``~/.micropython/lib:/usr/lib/micropython`` (``~/.micropython/lib`` for Windows port)
    or the value of the ``MICROPY_PY_SYS_PATH_DEFAULT`` option if it was set
    when MicroPython itself was compiled.

.. envvar:: MICROPYINSPECT

    Enables inspection. If ``MICROPYINSPECT`` is set to a non-empty string, it
    has the same effect as setting the :option:`-i` command line option.
