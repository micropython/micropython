.. _python_314:

Python 3.14
===========

Python 3.14.0 (final) was released on the 7 October 2025.  Unlike earlier
releases there is no "features for 3.14" PEP equivalent to :pep:`619`; the
closest analogue is `PEP 745 <https://peps.python.org/pep-0745/>`_, which
defines only the release schedule.  A detailed description of the changes can
be found in `What's New in Python 3.14
<https://docs.python.org/3/whatsnew/3.14.html>`_.

The headline result for MicroPython is :pep:`750` template strings
(t-strings), which are fully implemented -- an unusual case of MicroPython
picking up a brand new CPython feature almost immediately.  MicroPython's
version lag is not uniform across features: a specific contributor and use
case drove early adoption of t-strings, while considerably older additions
such as :pep:`634` structural pattern matching (3.10) and :pep:`654` exception
groups (3.11) remain unimplemented.

Unlike the pages for Python 3.5 to 3.10, this page also covers modules that
MicroPython ships only in `micropython-lib
<https://github.com/micropython/micropython-lib>`_ rather than compiling into
the firmware.  Such rows are marked with a note; those modules are optional
packages installed with ``mip`` or ``mpremote mip``, not built-in modules.

.. table::
  :widths: 20 60 20

  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | **New syntax features**                                                                                                       | **Status**                 |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | `PEP 750 <https://peps.python.org/pep-750/>`_            | Template string literals (t-strings)                               | Complete [#tstr]_          |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | `PEP 758 <https://peps.python.org/pep-758/>`_            | Allow ``except`` and ``except*`` expressions without parentheses   | Not implemented [#exc]_    |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | `PEP 765 <https://peps.python.org/pep-765/>`_            | Disallow ``return``, ``break`` and ``continue`` that exit a        | Not implemented [#warn]_   |
  |                                                          | ``finally`` block                                                  |                            |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | **New features in the standard library**                                                                                                                   |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | `PEP 784 <https://peps.python.org/pep-784/>`_            | Adding Zstandard to the standard library                           | Not implemented [#zstd]_   |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | `PEP 749 <https://peps.python.org/pep-749/>`_            | Implementing PEP 649, including the new ``annotationlib`` module   | Not implemented [#ann]_    |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | `PEP 734 <https://peps.python.org/pep-734/>`_            | Multiple interpreters in the standard library                      | Not implemented [#interp]_ |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | **Interpreter improvements**                                                                                                                               |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | `PEP 649 <https://peps.python.org/pep-649/>`_            | Deferred evaluation of annotations, so that ``from __future__      | Not implemented [#ann]_    |
  |                                                          | import annotations`` is no longer needed                           |                            |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | `PEP 768 <https://peps.python.org/pep-768/>`_            | Safe external debugger interface for CPython                       | Not relevant               |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | `PEP 741 <https://peps.python.org/pep-741/>`_            | Python configuration C API                                         | Not relevant               |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | `PEP 779 <https://peps.python.org/pep-779/>`_            | Free-threaded Python is officially supported                       | Not relevant               |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | `PEP 776 <https://peps.python.org/pep-776/>`_            | Emscripten support (tier 3)                                        | Not relevant [#wasm]_      |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | `Tail-calling interpreter                                | A new type of interpreter that uses tail calls between small C     | Not relevant               |
  | <https://docs.python.org/3/whatsnew/3.14.html>`_         | functions, 3-5% faster on average                                  |                            |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | `REPL improvements                                       | The default interactive shell now highlights Python syntax as it   | Partial [#repl]_           |
  | <https://docs.python.org/3/whatsnew/3.14.html>`_         | is typed, and ``import`` statements offer auto-completion of       |                            |
  |                                                          | module names                                                       |                            |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | `Error messages                                          | Further improvements to error messages, including suggestions for  | Not implemented [#errmsg]_ |
  | <https://docs.python.org/3/whatsnew/3.14.html>`_         | misspelled keyword arguments and clearer reports of unbalanced     |                            |
  |                                                          | brackets                                                           |                            |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | **Important deprecations, removals or restrictions**                                                                                                       |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+
  | `PEP 761 <https://peps.python.org/pep-761/>`_            | Discontinuation of PGP signatures for CPython release artifacts    | Not relevant               |
  +----------------------------------------------------------+--------------------------------------------------------------------+----------------------------+


Other Language Changes:

.. table::
  :widths: 90 10

  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | All Windows code pages are now supported as ``cpXXX`` codecs on Windows.                                       | Not relevant                 |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Mixed-mode arithmetic between a real number and a :class:`complex` number now follows the recommendations of   | Not implemented [#cplx]_     |
  | C99 Annex G, so that infinities and NaNs propagate correctly.                                                  |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Incorrect usage of ``await`` and asynchronous comprehensions is now detected even when the enclosing code is   | Not implemented [#opt]_      |
  | optimized away by the ``-O`` command line option.                                                              |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Slots of C types are no longer replaced with wrapped versions when a pure Python subclass is created.          | Not relevant                 |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | :meth:`bytes.fromhex` and :meth:`bytearray.fromhex` now accept ASCII :class:`bytes` and bytes-like objects in  | Complete [#hex]_             |
  | addition to :class:`str`.                                                                                      |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Added the :meth:`float.from_number` and :meth:`complex.from_number` constructors.                              | Not implemented              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | The ``,`` and ``_`` thousands separators are now also supported in the fractional part when formatting a       | Not implemented [#sep]_      |
  | :class:`float`.                                                                                                |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | :func:`int` no longer delegates to the :meth:`~object.__trunc__` method.                                       | Complete [#trunc]_           |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | :func:`map` has a new keyword-only ``strict`` flag, used to require that all the iterables have an equal       | Not implemented              |
  | length.                                                                                                        |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | :class:`memoryview` now supports subscription, making it a generic type (``memoryview[int]``).                 | Not implemented              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Using :const:`NotImplemented` in a boolean context now raises :exc:`TypeError`; previously this emitted a      | Not implemented              |
  | :exc:`DeprecationWarning`.                                                                                     |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | The three-argument form of :func:`pow` now tries :meth:`~object.__rpow__` if necessary.  MicroPython's         | Not implemented              |
  | three-argument :func:`pow` accepts integers only and otherwise raises :exc:`TypeError`.                        |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | :class:`super` objects are now copyable and pickleable.                                                        | Not relevant                 |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | ``-X importtime=2`` now also reports modules that were already present in :data:`sys.modules`.                 | Not implemented              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | The ``-c`` command line option now automatically dedents its code argument.  The unix port accepts ``-c`` but  | Not implemented              |
  | rejects indented code with an :exc:`IndentationError`.                                                         |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | The ``-J`` command line option is no longer reserved for use by Jython.                                        | Not relevant                 |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | The cyclic garbage collector became incremental in 3.14.0 and was reverted to a generational design in 3.14.5. | Not relevant [#gc]_          |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+

Changes to built-in modules:

.. table::
  :widths: 90 10

  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | `asyncio <https://docs.python.org/3/whatsnew/3.14.html#asyncio>`_                                                                             |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | :func:`asyncio.create_task`, :meth:`asyncio.TaskGroup.create_task` and :func:`asyncio.eager_task_factory` now  | Not implemented              |
  | accept arbitrary keyword arguments, which are passed on to the :class:`asyncio.Task` constructor.              |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Added :func:`asyncio.capture_call_graph` and :func:`asyncio.print_call_graph` for introspecting the call graph | Not implemented              |
  | of a running task.                                                                                             |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | asyncio can now be used as a command line tool (``python -m asyncio ps PID`` and ``pstree PID``) to inspect a  | Not relevant                 |
  | running process.                                                                                               |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | `base64 <https://docs.python.org/3/whatsnew/3.14.html#base64>`_                                                                               |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Added :func:`base64.z85encode` and :func:`base64.z85decode` for Z85 data encoding.                             | Not implemented [#mplib]_    |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | `errno <https://docs.python.org/3/whatsnew/3.14.html#errno>`_                                                                                 |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Added the :data:`errno.EHWPOISON` error code.  MicroPython exposes only a small curated subset of the          | Not implemented              |
  | ``errno`` values.                                                                                              |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | `gzip and zlib <https://docs.python.org/3/whatsnew/3.14.html#gzip>`_                                                                          |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | The :mod:`gzip` and :mod:`zlib` modules are now also available as ``compression.gzip`` and                     | Not implemented [#zstd]_     |
  | ``compression.zlib``.                                                                                          |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | `heapq <https://docs.python.org/3/whatsnew/3.14.html#heapq>`_                                                                                 |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Added the max-heap variants ``heapify_max``, ``heappush_max``, ``heappop_max``, ``heapreplace_max`` and        | Not implemented              |
  | ``heappushpop_max``.                                                                                           |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | `io <https://docs.python.org/3/whatsnew/3.14.html#io>`_                                                                                       |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Calling ``read()`` on a non-blocking text stream may now raise :exc:`BlockingIOError` rather than returning an | Not implemented [#iolib]_    |
  | empty string.                                                                                                  |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Added the ``io.Reader`` and ``io.Writer`` protocols for static typing.                                         | Not relevant [#ann]_         |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | `json <https://docs.python.org/3/whatsnew/3.14.html#json>`_                                                                                   |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Serialization errors now carry exception notes identifying the offending object.  MicroPython does not         | Not implemented              |
  | implement the :pep:`678` ``add_note()`` mechanism that this relies on.                                         |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | The :mod:`json` module is now usable as a command line tool, with colourised output.                           | Not relevant                 |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | `math <https://docs.python.org/3/whatsnew/3.14.html#math>`_                                                                                   |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Added :func:`math.isnormal` and :func:`math.issubnormal`.                                                      | Not implemented              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Domain errors raised by the :mod:`math` module now carry more detailed messages.                               | Not implemented [#errmsg]_   |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | `operator <https://docs.python.org/3/whatsnew/3.14.html#operator>`_                                                                           |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Added :func:`operator.is_none` and :func:`operator.is_not_none`.                                               | Not implemented [#mplib]_    |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | `os <https://docs.python.org/3/whatsnew/3.14.html#os>`_                                                                                       |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Added :func:`os.readinto` to read into a pre-allocated writable buffer from a file descriptor.                 | Not implemented [#readinto]_ |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Added :func:`os.reload_environ` to refresh :data:`os.environ`.  MicroPython has no :data:`os.environ`, only    | Not relevant                 |
  | :func:`os.getenv` and :func:`os.putenv` on ports that support them.                                            |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Added the ``SCHED_DEADLINE`` and ``SCHED_NORMAL`` constants.                                                   | Not relevant                 |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | `os.path <https://docs.python.org/3/whatsnew/3.14.html#os-path>`_                                                                             |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | :func:`os.path.realpath` now accepts ``strict=ALLOW_MISSING`` to ignore missing final components.  The         | Not implemented [#mplib]_    |
  | micropython-lib ``os-path`` package provides no ``realpath`` at all.                                           |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | `pathlib <https://docs.python.org/3/whatsnew/3.14.html#pathlib>`_                                                                             |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Added :meth:`!Path.copy`, :meth:`!Path.copy_into`, :meth:`!Path.move` and :meth:`!Path.move_into`, together    | Not implemented [#mplib]_    |
  | with the :attr:`!Path.info` attribute.                                                                         |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | `sys <https://docs.python.org/3/whatsnew/3.14.html#sys>`_                                                                                     |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Added :func:`sys.remote_exec` to execute code in a remote process (:pep:`768`).                                | Not relevant                 |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+
  | Added the ``sys.flags.thread_inherit_context`` and ``sys.flags.context_aware_warnings`` flags.  MicroPython    | Not relevant                 |
  | has no ``sys.flags`` at all.                                                                                   |                              |
  +----------------------------------------------------------------------------------------------------------------+------------------------------+

.. rubric:: Notes

.. [#tstr] Template strings are implemented in full, including the ``t``
   string prefix, the ``string.templatelib`` module with its ``Template`` and
   ``Interpolation`` types, iteration, concatenation, nested quotes and the
   ``{expr=}`` debug form.  They require ``MICROPY_PY_TSTRINGS``, which is
   enabled by default at the full-features ROM level: the alif, mimxrt and
   samd (SAMD51 only) ports, the unix coverage variant and the webassembly
   pyscript variant.  The zephyr port enables them when explicitly configured
   for the full-features (or higher) ROM level via Kconfig, which is not the
   default.  ``mpy-cross`` always accepts t-string syntax, so a ``.mpy`` file
   using t-strings can be cross-compiled even for a target port whose own
   interpreter would reject the syntax.  Two deviations from CPython remain:
   the ``!a`` conversion is not supported, and there is no :func:`format`
   built-in, so processing code should use :meth:`str.format` instead.  See
   :mod:`string.templatelib` for details.

.. [#exc] MicroPython's grammar accepts only a single expression after
   ``except``, so ``except A, B:`` is a :exc:`SyntaxError`; the parenthesized
   form ``except (A, B):`` works as usual.  The ``except*`` syntax is a
   :pep:`654` (Python 3.11) feature that MicroPython does not implement at
   all, so the :pep:`758` relaxation is doubly inapplicable there.

.. [#warn] MicroPython has a warning category hierarchy and an internal
   ``mp_warning()`` mechanism, but the compiler emits no ``SyntaxWarning``
   diagnostics.  ``return``, ``break`` and ``continue`` inside a ``finally``
   block are accepted silently and swallow any in-flight exception, exactly as
   in earlier CPython versions.

.. [#zstd] MicroPython has no Zstandard support and no ``compression``
   namespace package.  Its compression story is the built-in :mod:`deflate`
   module, which offers ``DeflateIO`` with ``RAW``, ``ZLIB``, ``GZIP`` and
   ``AUTO`` formats.  micropython-lib layers thin ``gzip`` and ``zlib``
   compatibility shims on top of it.

.. [#ann] MicroPython parses type annotations but never evaluates them, so
   annotations produce no runtime effects and no ``__annotations__``
   attributes are created.  The lazy-evaluation semantics of :pep:`649` are
   therefore largely moot, but there is no ``annotationlib`` module, no real
   ``__annotations__`` dictionary and no ``typing`` module, so none of this
   can be used other than as inert annotations.

.. [#interp] MicroPython supports multiple independent VM instances at the C
   level (the interpreter state lives in a single ``mp_state_ctx`` structure,
   which the embed port relies on), but there is no Python-level API to
   create or communicate with them, so nothing equivalent to
   ``concurrent.interpreters`` exists.

.. [#wasm] :pep:`776` concerns the support tier CPython assigns to its own
   Emscripten builds, so it is not something MicroPython can implement.
   MicroPython has independently targeted Emscripten for some time through its
   ``webassembly`` port.

.. [#repl] MicroPython's REPL already offers tab completion, and completion
   after ``import`` specifically suggests built-in module names.  There is no
   syntax highlighting.  Both depend on ``MICROPY_HELPER_REPL``, which is
   enabled from the extra-features ROM level upwards.

.. [#errmsg] MicroPython's exception messages are deliberately terse to save
   ROM.  ``MICROPY_ERROR_REPORTING`` selects between four levels (none, terse,
   normal and detailed), with detailed used from the full-features ROM level
   upwards, but even that level does not attempt CPython's suggestion-style
   diagnostics.

.. [#cplx] MicroPython promotes the real operand to a complex number and then
   applies the textbook formula, with no special handling of infinities or
   NaNs.  For example ``complex(float('inf'), 0) * 2`` evaluates to
   ``(inf+nanj)`` rather than the ``(inf+0j)`` that C99 Annex G requires.

.. [#opt] MicroPython's ``-O`` option sets the optimisation level used for
   ``assert`` removal and ``__debug__``.  An ``assert`` whose expression
   contains a misplaced ``await`` is reported as a :exc:`SyntaxError` at the
   default level, but is discarded without complaint under ``-O``.

.. [#hex] MicroPython accepts any object supporting the buffer protocol here,
   which makes it broader than CPython 3.14: :class:`str`, :class:`bytes`,
   :class:`bytearray` and :class:`memoryview` arguments all work.

.. [#sep] MicroPython honours the ``,`` and ``_`` separators only when
   formatting integers.  They are ignored entirely for :class:`float` values,
   in the integer part as well as the fractional part, so ``'{:,.2f}'`` yields
   an ungrouped result.

.. [#trunc] MicroPython has never consulted ``__trunc__``, so its behaviour
   already matches Python 3.14.  Note that MicroPython does not implement
   ``__index__`` as a distinct protocol either: :func:`int` and sequence
   subscripting both go through ``__int__``, and an object defining only
   ``__index__`` raises :exc:`TypeError`.

.. [#gc] MicroPython's collector is a non-generational mark-and-sweep
   collector, so neither the incremental nor the generational design applies.
   :func:`gc.collect` takes no generation argument.

.. [#mplib] This module is not built into MicroPython firmware.  It is
   provided as an optional pure-Python package in `micropython-lib
   <https://github.com/micropython/micropython-lib>`_, installable with
   ``mip``, and generally implements only a commonly used subset of the
   CPython API.

.. [#iolib] MicroPython has no text-mode/binary-mode stream distinction of the
   kind this change describes.  Non-blocking streams conventionally return
   ``None`` from ``read()`` when no data is available rather than raising.
   :exc:`BlockingIOError` does exist as an :exc:`OSError` subclass.

.. [#readinto] Buffer-oriented reads are idiomatic in MicroPython, and stream
   objects such as files and :class:`io.BytesIO` do provide a ``readinto()``
   method.  What is missing is the file-descriptor level :func:`os.readinto`.

