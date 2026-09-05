.. _python_37:

Python 3.7
==========

New Features:

.. table::
  :widths: 20 60 20

  +--------------------------------------------------------+--------------------------------------------------+--------------------------------------+
  | **Feature**                                                                                               | **Status**                           |
  +--------------------------------------------------------+--------------------------------------------------+--------------------------------------+
  | `PEP 538 <https://www.python.org/dev/peps/pep-0538/>`_ | Coercing the legacy C locale to a UTF-8 based    | Not relevant                         |
  |                                                        | locale                                           |                                      |
  +--------------------------------------------------------+--------------------------------------------------+--------------------------------------+
  | `PEP 539 <https://www.python.org/dev/peps/pep-0539/>`_ | A New C-API for Thread-Local Storage in CPython  | Not relevant                         |
  +--------------------------------------------------------+--------------------------------------------------+--------------------------------------+
  | `PEP 540 <https://www.python.org/dev/peps/pep-0540/>`_ | UTF-8 mode                                       | Not relevant                         |
  +--------------------------------------------------------+--------------------------------------------------+--------------------------------------+
  | `PEP 552 <https://www.python.org/dev/peps/pep-0552/>`_ | Deterministic pyc                                | Not relevant                         |
  +--------------------------------------------------------+--------------------------------------------------+--------------------------------------+
  | `PEP 553 <https://www.python.org/dev/peps/pep-0553/>`_ | Built-in ``breakpoint()``                        | Not implemented                      |
  +--------------------------------------------------------+--------------------------------------------------+--------------------------------------+
  | `PEP 557 <https://www.python.org/dev/peps/pep-0557/>`_ | Data Classes                                     | Not implemented                      |
  +--------------------------------------------------------+--------------------------------------------------+--------------------------------------+
  | `PEP 560 <https://www.python.org/dev/peps/pep-0560/>`_ | Core support for typing module and generic types | Not implemented                      |
  +--------------------------------------------------------+--------------------------------------------------+--------------------------------------+
  | `PEP 562 <https://www.python.org/dev/peps/pep-0562/>`_ | Module ``__getattr__`` and ``__dir__``           | Partial [#fmodattr]_                 |
  +--------------------------------------------------------+--------------------------------------------------+--------------------------------------+
  | `PEP 563 <https://www.python.org/dev/peps/pep-0563/>`_ | Postponed Evaluation of Annotations              | Partial [#fannot]_                   |
  +--------------------------------------------------------+--------------------------------------------------+--------------------------------------+
  | `PEP 564 <https://www.python.org/dev/peps/pep-0564/>`_ | Time functions with nanosecond resolution        | Partial [#ftimenanosec]_             |
  +--------------------------------------------------------+--------------------------------------------------+--------------------------------------+
  | `PEP 565 <https://www.python.org/dev/peps/pep-0565/>`_ | Show DeprecationWarning in ``__main__``          | Not implemented                      |
  +--------------------------------------------------------+--------------------------------------------------+--------------------------------------+
  | `PEP 567 <https://www.python.org/dev/peps/pep-0567/>`_ | Context Variables                                | Not implemented                      |
  +--------------------------------------------------------+--------------------------------------------------+--------------------------------------+

Other Language Changes:

.. table::
  :widths: 90 10

  +-----------------------------------------------------------------------------------------------------------------+----------------+
  | ``async`` and ``await`` are now reserved keywords                                                               | Complete       |
  +-----------------------------------------------------------------------------------------------------------------+----------------+
  | ``dict`` objects must preserve insertion-order                                                                  | Not implemented|
  +-----------------------------------------------------------------------------------------------------------------+----------------+
  | More than 255 arguments can now be passed to a function; a function can now have more than 255 parameters       | Not implemented|
  +-----------------------------------------------------------------------------------------------------------------+----------------+
  | ``bytes.fromhex()`` and ``bytearray.fromhex()`` now ignore all ASCII whitespace, not only spaces                | Complete       |
  +-----------------------------------------------------------------------------------------------------------------+----------------+
  | ``str``, ``bytes``, and ``bytearray`` gained support for the new ``isascii()`` method, which can be used to     | Not implemented|
  | test if a string or bytes contain only the ASCII characters                                                     |                |
  +-----------------------------------------------------------------------------------------------------------------+----------------+
  | ``ImportError`` now displays module name and module ``__file__`` path when ``from ... import ...`` fails        | Not implemented|
  +-----------------------------------------------------------------------------------------------------------------+----------------+
  | Circular imports involving absolute imports with binding a submodule to a name are now supported                | Complete       |
  +-----------------------------------------------------------------------------------------------------------------+----------------+
  | ``object.__format__(x, '')`` is now equivalent to ``str(x)`` rather than ``format(str(self), '')``              | Partial        |
  |                                                                                                                 | [#fformat]_    |
  +-----------------------------------------------------------------------------------------------------------------+----------------+
  | In order to better support dynamic creation of stack traces, ``types.TracebackType`` can now be                 | Not implemented|
  | instantiated from Python code, and the ``tb_next`` attribute on tracebacks is now writable                      |                |
  +-----------------------------------------------------------------------------------------------------------------+----------------+
  | When using the ``-m`` switch, ``sys.path[0]`` is now eagerly expanded to the full starting directory path,      | Not implemented|
  | rather than being left as the empty directory (which allows imports from the current working directory          |                |
  | at the time when an import occurs)                                                                              |                |
  +-----------------------------------------------------------------------------------------------------------------+----------------+
  | The new ``-X importtime`` option or the ``PYTHONPROFILEIMPORTTIME`` environment variable can be used to         | Not relevant   |
  | show the timing of each module import                                                                           |                |
  +-----------------------------------------------------------------------------------------------------------------+----------------+

Changes to built-in modules:

.. table::
  :widths: 90 10

  +------------------------------------------------------------------------------------------------------------+----------------+
  | `asyncio <https://docs.python.org/3/whatsnew/3.7.html#asyncio>`_                                                            |
  +------------------------------------------------------------------------------------------------------------+----------------+
  | Too many to list                                                                                           | Partial        |
  |                                                                                                            | [#fasyncio]_   |
  +------------------------------------------------------------------------------------------------------------+----------------+
  | `gc <https://docs.python.org/3/whatsnew/3.7.html#gc>`_                                                                      |
  +------------------------------------------------------------------------------------------------------------+----------------+
  | New features include *gc.freeze()*, *gc.unfreeze()*, *gc-get_freeze_count*                                 | Not implemented|
  +------------------------------------------------------------------------------------------------------------+----------------+
  | `math <https://docs.python.org/3/whatsnew/3.7.html#math>`_                                                                  |
  +------------------------------------------------------------------------------------------------------------+----------------+
  | math.remainder() added to implement IEEE 754-style remainder                                               | Not implemented|
  +------------------------------------------------------------------------------------------------------------+----------------+
  | `re <https://docs.python.org/3/whatsnew/3.7.html#re>`_                                                                      |
  +------------------------------------------------------------------------------------------------------------+----------------+
  | A number of tidy up features including better support for splitting on empty strings and copy support for  | Not implemented|
  | compiled expressions and match objects                                                                     | [#fre]_        |
  +------------------------------------------------------------------------------------------------------------+----------------+
  | `sys <https://docs.python.org/3/whatsnew/3.7.html#sys>`_                                                                    |
  +------------------------------------------------------------------------------------------------------------+----------------+
  | sys.breakpointhook() added. sys.get(/set)_coroutine_origin_tracking_depth() added                          | Not implemented|
  +------------------------------------------------------------------------------------------------------------+----------------+
  | `time <https://docs.python.org/3/whatsnew/3.7.html#time>`_                                                                  |
  +------------------------------------------------------------------------------------------------------------+----------------+
  | Mostly updates to support nanosecond resolution in PEP564, see above                                       | Partial        |
  |                                                                                                            | [#ftime]_      |
  +------------------------------------------------------------------------------------------------------------+----------------+

.. rubric:: Notes

.. [#fmodattr] Module-level ``__getattr__`` is supported on builds with
   ``MICROPY_MODULE_GETATTR`` enabled (the default from the "core features"
   ROM level upwards).  Module-level ``__dir__`` is ignored: ``dir()`` on a
   module does not dispatch to it.

.. [#fannot] MicroPython's parser discards annotations, so they are never
   evaluated -- which gives the same practical result as PEP 563.  They are not
   stored either: there is no ``__annotations__`` attribute on modules, classes
   or functions, and ``__future__`` is not a built-in module.

.. [#ftimenanosec] Only :func:`time.time_ns` is implemented.

.. [#fformat] MicroPython has no ``format()`` built-in and does not implement
   the ``__format__`` special method.  ``str.format()`` with an empty format
   specification already uses ``str(x)``, which matches the Python 3.7
   behaviour, but a non-empty specification raises ``TypeError`` for objects
   that define only ``__str__``.

.. [#fasyncio] Of the Python 3.7 :mod:`asyncio` additions, MicroPython
   implements ``asyncio.run()``, ``asyncio.create_task()``,
   ``asyncio.current_task()`` and ``StreamWriter.wait_closed()``.  It does not
   implement ``asyncio.all_tasks()``, ``asyncio.get_running_loop()``,
   ``asyncio.BufferedProtocol``, ``contextvars`` support,
   ``Server.serve_forever()``, ``Server.start_serving()``,
   ``Server.is_serving()``, ``loop.sock_sendfile()`` or ``loop.start_tls()``.

.. [#fre] Neither of these changes applies: MicroPython has no module-level
   ``re.split()`` (only the ``regex.split()`` method), splitting on a pattern
   that can match an empty string returns the subject unsplit, and there is no
   ``copy`` module with which to copy compiled patterns or match objects.

.. [#ftime] Only :func:`time.time_ns` is implemented.
   :func:`time.clock_gettime_ns`, :func:`time.monotonic_ns`,
   :func:`time.perf_counter_ns`, :func:`time.process_time_ns` and
   :func:`time.thread_time_ns` are not.
