.. _python_312:

Python 3.12
===========

Python 3.12.0 (final) was released on the 2 October 2023.  Unlike earlier releases there is no
PEP enumerating the features for 3.12, so the list below follows the release highlights and the
detailed description of the changes in
`What's New in Python 3.12 <https://docs.python.org/3/whatsnew/3.12.html>`_.

The headline language change, :pep:`701`, is largely supported by MicroPython: f-strings may reuse
the enclosing quote character, nest arbitrarily, contain backslashes, span multiple lines and
contain comments.  MicroPython does not track CPython versions strictly and occasionally implements
a newer feature early -- t-strings (:pep:`750`, a Python 3.14 feature) are also implemented, gated
behind ``MICROPY_PY_TSTRINGS``.

Unlike the pages for Python 3.5 to 3.10, this page also covers modules that are not built into the
firmware but are distributed as pure-Python packages in `micropython-lib
<https://github.com/micropython/micropython-lib>`__ (for example ``itertools``, ``pathlib``,
``os.path``, ``shutil``, ``inspect``, ``types``, ``tarfile``, ``tempfile`` and ``unittest``).  Rows
concerning those modules are marked with a footnote, because they are optional installable packages
with a reduced API rather than part of the interpreter.

.. table::
  :widths: 20 60 20

  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | **New syntax features**                                                                                                 | **Status**                |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `PEP 695 <https://peps.python.org/pep-0695/>`_         | Type Parameter Syntax and the ``type`` statement               | Not implemented [#p695]_  |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | **New grammar features**                                                                                                                            |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `PEP 701 <https://peps.python.org/pep-0701/>`_         | Syntactic formalization of f-strings                           | Partial [#f701]_          |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | **Interpreter improvements**                                                                                                                        |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `PEP 684 <https://peps.python.org/pep-0684/>`_         | A unique per-interpreter GIL                                   | Not relevant              |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `PEP 669 <https://peps.python.org/pep-0669/>`_         | Low impact monitoring for CPython                              | Not implemented [#trace]_ |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `gh-98763                                              | Improved ``Did you mean ...`` suggestions for                  | Not implemented           |
  | <https://github.com/python/cpython/issues/98763>`_     | :exc:`NameError`, :exc:`ImportError` and :exc:`SyntaxError`    |                           |
  |                                                        | exceptions                                                     |                           |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | **Python data model improvements**                                                                                                                  |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `PEP 688 <https://peps.python.org/pep-0688/>`_         | Making the buffer protocol accessible in Python                | Not implemented [#buf]_   |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | **Security improvements**                                                                                                                           |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `gh-99108                                              | Replace the builtin :mod:`hashlib` implementations of SHA1,    | Not relevant              |
  | <https://github.com/python/cpython/issues/99108>`_     | SHA3, SHA2-384, SHA2-512 and MD5 with formally verified code   |                           |
  |                                                        | from the HACL* project                                         |                           |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | **CPython implementation improvements**                                                                                                             |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `PEP 709 <https://peps.python.org/pep-0709/>`_         | Comprehension inlining                                         | Not implemented [#p709]_  |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `gh-96123                                              | CPython support for the Linux ``perf`` profiler                | Not relevant              |
  | <https://github.com/python/cpython/issues/96123>`_     |                                                                |                           |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `gh-91079                                              | Implement stack overflow protection on supported platforms     | Complete [#stk]_          |
  | <https://github.com/python/cpython/issues/91079>`_     |                                                                |                           |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | **C API improvements**                                                                                                                              |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `PEP 697 <https://peps.python.org/pep-0697/>`_         | Unstable C API tier                                            | Not relevant              |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `PEP 683 <https://peps.python.org/pep-0683/>`_         | Immortal objects, using a fixed refcount                       | Not relevant              |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | **New typing features**                                                                                                                             |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `PEP 692 <https://peps.python.org/pep-0692/>`_         | Using TypedDict to annotate ``**kwargs``                       | Not implemented [#ann]_   |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `PEP 698 <https://peps.python.org/pep-0698/>`_         | The ``@typing.override`` decorator                             | Not implemented [#ann]_   |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | **Important deprecations, removals or restrictions**                                                                                                |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `PEP 623 <https://peps.python.org/pep-0623/>`_         | Remove ``wstr`` from Unicode objects in Python's C API         | Not relevant              |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `PEP 632 <https://peps.python.org/pep-0632/>`_         | Remove the ``distutils`` package                               | Not relevant              |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `PEP 594 <https://peps.python.org/pep-0594/>`_         | Remove the ``asynchat``, ``asyncore`` and ``smtpd`` dead       | Not relevant              |
  |                                                        | batteries; the ``imp`` module and several                      |                           |
  |                                                        | :class:`unittest.TestCase` method aliases are also removed     |                           |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+
  | `gh-95299                                              | Do not pre-install ``setuptools`` in virtual environments      | Not relevant              |
  | <https://github.com/python/cpython/issues/95299>`_     | created with ``venv``                                          |                           |
  +--------------------------------------------------------+----------------------------------------------------------------+---------------------------+


Other Language Changes:

.. table::
  :widths: 90 10

  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | The parser now raises :exc:`SyntaxError` when parsing source code containing null bytes.            | Not implemented [#nul]_   |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | A backslash-character pair that is not a valid escape sequence now generates a                      | Not implemented [#warn]_  |
  | :exc:`SyntaxWarning`, instead of a :exc:`DeprecationWarning`.  For example,                         |                           |
  | ``re.compile("\d+\.\d+")`` now emits a :exc:`SyntaxWarning`.                                        |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Octal escapes with a value larger than ``0o377`` (e.g. ``"\477"``), deprecated in Python 3.11, now  | Not implemented [#warn]_  |
  | produce a :exc:`SyntaxWarning` instead of a :exc:`DeprecationWarning`.                              |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Variables used in the target part of comprehensions that are not stored to can now be used in       | Not implemented           |
  | assignment expressions (``:=``).  For example, in ``[(b := 1) for a, b.prop in some_iter]``, the    |                           |
  | assignment to ``b`` is now allowed.                                                                 |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Exceptions raised in a class or type's :meth:`~object.__set_name__` method are no longer wrapped by | Partial [#note]_          |
  | a :exc:`RuntimeError`.  Context information is added to the exception as a :pep:`678` note.         |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | When a ``try-except*`` construct handles the entire :exc:`ExceptionGroup` and raises one other      | Not implemented           |
  | exception, that exception is no longer wrapped in an :exc:`ExceptionGroup`.                         |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | The Garbage Collector now runs only on the eval breaker mechanism of the Python bytecode evaluation | Not relevant              |
  | loop instead of on object allocations.                                                              |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | All builtin and extension callables expecting boolean parameters now accept arguments of any type   | Complete                  |
  | instead of just :class:`bool` and :class:`int`.                                                     |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :class:`memoryview` now supports the half-float type (the ``"e"`` format code).                     | Not implemented [#mv]_    |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :class:`slice` objects are now hashable, allowing them to be used as dict keys and set items.       | Not implemented [#slice]_ |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :func:`sum` now uses Neumaier summation to improve accuracy and commutativity when summing floats   | Not implemented           |
  | or mixed ints and floats.                                                                           |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :func:`ast.parse` now raises :exc:`SyntaxError` instead of :exc:`ValueError` when parsing source    | Not relevant [#nomod]_    |
  | code containing null bytes.                                                                         |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :pep:`706`: The extraction methods in :mod:`tarfile`, and :func:`shutil.unpack_archive`, have a new | Not implemented [#mplib]_ |
  | ``filter`` argument that allows limiting tar features that may be surprising or dangerous.          |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :class:`types.MappingProxyType` instances are now hashable if the underlying mapping is hashable.   | Not implemented [#mplib]_ |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add support for the ``perf`` profiler through the new environment variable                          | Not relevant              |
  | :envvar:`PYTHONPERFSUPPORT`, the ``-X perf`` command-line option, and the new                       |                           |
  | :func:`sys.activate_stack_trampoline`, :func:`sys.deactivate_stack_trampoline` and                  |                           |
  | :func:`sys.is_stack_trampoline_active` functions.                                                   |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+

Changes to built-in modules:

.. table::
  :widths: 90 10

  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `array <https://docs.python.org/3/whatsnew/3.12.html#array>`_                                                                   |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | The :class:`array.array` class now supports subscripting, making it a generic type.                 | Not implemented           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `asyncio <https://docs.python.org/3/whatsnew/3.12.html#asyncio>`_                                                               |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | The performance of writing to sockets in :mod:`asyncio` has been significantly improved;            | Not relevant              |
  | unnecessary copying is avoided and ``sendmsg()`` is used if the platform supports it.               |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :func:`asyncio.eager_task_factory` and :func:`asyncio.create_eager_task_factory` to allow       | Not implemented           |
  | opting an event loop in to eager task execution.                                                    |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | On Linux, :mod:`asyncio` uses :class:`asyncio.PidfdChildWatcher` by default if                      | Not relevant              |
  | :func:`os.pidfd_open` is available and functional, and the event loop now selects the best          |                           |
  | available child watcher for each platform.                                                          |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add a ``loop_factory`` parameter to :func:`asyncio.run` to allow specifying a custom event loop     | Not implemented           |
  | factory.                                                                                            |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add a C implementation of :func:`asyncio.current_task` for a 4x-6x speedup.                         | Not relevant [#aio]_      |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :func:`asyncio.iscoroutine` now returns ``False`` for generators, as :mod:`asyncio` does not        | Not implemented           |
  | support legacy generator-based coroutines.                                                          |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :func:`asyncio.wait` and :func:`asyncio.as_completed` now accept generators yielding tasks.         | Not implemented           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `calendar <https://docs.python.org/3/whatsnew/3.12.html#calendar>`_                                                             |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add the enums :class:`calendar.Month` and :class:`calendar.Day` defining months of the year and     | Not implemented [#nomod]_ |
  | days of the week.                                                                                   |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `csv <https://docs.python.org/3/whatsnew/3.12.html#csv>`_                                                                       |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :const:`csv.QUOTE_NOTNULL` and :const:`csv.QUOTE_STRINGS` flags to provide finer grained        | Not implemented [#nomod]_ |
  | control of ``None`` and empty strings by reader and writer objects.                                 |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `dis <https://docs.python.org/3/whatsnew/3.12.html#dis>`_                                                                       |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Pseudo instruction opcodes are now exposed in the :mod:`dis` module, along with the new             | Not relevant [#nomod]_    |
  | :data:`dis.hasarg` and :data:`dis.hasexc` collections.                                              |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `fractions <https://docs.python.org/3/whatsnew/3.12.html#fractions>`_                                                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Objects of type :class:`fractions.Fraction` now support float-style formatting.                     | Not implemented [#nomod]_ |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `importlib.resources <https://docs.python.org/3/whatsnew/3.12.html#importlib-resources>`_                                       |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :func:`importlib.resources.as_file` now supports resource directories, and the first parameter of   | Not relevant [#nomod]_    |
  | :func:`importlib.resources.files` is renamed to ``anchor``.                                         |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `inspect <https://docs.python.org/3/whatsnew/3.12.html#inspect>`_                                                               |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :func:`inspect.markcoroutinefunction` to mark sync functions that return a coroutine for use    | Not implemented [#mplib]_ |
  | with :func:`inspect.iscoroutinefunction`.                                                           |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :func:`inspect.getasyncgenstate` and :func:`inspect.getasyncgenlocals` for determining the      | Not implemented [#mplib]_ |
  | current state of asynchronous generators.                                                           |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | The performance of :func:`inspect.getattr_static` has been considerably improved.                   | Not relevant [#mplib]_    |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `itertools <https://docs.python.org/3/whatsnew/3.12.html#itertools>`_                                                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :func:`itertools.batched` for collecting into even-sized tuples where the last batch may be     | Not implemented [#mplib]_ |
  | shorter than the rest.                                                                              |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `math <https://docs.python.org/3/whatsnew/3.12.html#math>`_                                                                     |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :func:`math.sumprod` for computing a sum of products.                                           | Not implemented           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Extend :func:`math.nextafter` to include a ``steps`` argument for moving up or down multiple steps  | Not implemented [#nfa]_   |
  | at a time.                                                                                          |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `os <https://docs.python.org/3/whatsnew/3.12.html#os>`_                                                                         |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :data:`os.PIDFD_NONBLOCK` to open a file descriptor for a process with :func:`os.pidfd_open` in | Not relevant              |
  | non-blocking mode.                                                                                  |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :class:`os.DirEntry` now includes an :meth:`os.DirEntry.is_junction` method to check if the entry   | Not relevant              |
  | is a junction.                                                                                      |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :func:`os.listdrives`, :func:`os.listvolumes` and :func:`os.listmounts` functions on Windows.   | Not relevant              |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :func:`os.stat` and :func:`os.lstat` are now more accurate on Windows, filling ``st_birthtime`` and | Not relevant              |
  | widening ``st_dev``/``st_ino``.                                                                     |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `os.path <https://docs.python.org/3/whatsnew/3.12.html#os-path>`_                                                               |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :func:`os.path.isjunction` to check if a given path is a junction.                              | Not relevant [#mplib]_    |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :func:`os.path.splitroot` to split a path into a triad (drive, root, tail).                     | Not implemented [#mplib]_ |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `pathlib <https://docs.python.org/3/whatsnew/3.12.html#pathlib>`_                                                               |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add support for subclassing :class:`pathlib.PurePath` and :class:`pathlib.Path`; subclasses may     | Not implemented [#mplib]_ |
  | override the new :meth:`pathlib.PurePath.with_segments` method to pass information between path     |                           |
  | instances.                                                                                          |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :meth:`pathlib.Path.walk` for walking directory trees and generating all file or directory      | Not implemented [#mplib]_ |
  | names within them, similar to :func:`os.walk`.                                                      |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add a ``walk_up`` optional parameter to :meth:`pathlib.PurePath.relative_to` to allow the insertion | Not implemented [#mplib]_ |
  | of ``..`` entries in the result.                                                                    |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :meth:`pathlib.Path.is_junction` as a proxy to :func:`os.path.isjunction`.                      | Not relevant [#mplib]_    |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add a ``case_sensitive`` optional parameter to :meth:`pathlib.Path.glob`,                           | Not implemented [#mplib]_ |
  | :meth:`pathlib.Path.rglob` and :meth:`pathlib.PurePath.match`.                                      |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `platform <https://docs.python.org/3/whatsnew/3.12.html#platform>`_                                                             |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add support for detecting Windows 11 and Windows Server releases past 2012.                         | Not relevant              |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `pdb <https://docs.python.org/3/whatsnew/3.12.html#pdb>`_                                                                       |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add convenience variables to hold values temporarily for a debug session and provide quick access   | Not relevant [#nomod]_    |
  | to values like the current frame or the return value.                                               |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `random <https://docs.python.org/3/whatsnew/3.12.html#random>`_                                                                 |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :func:`random.binomialvariate`.                                                                 | Not implemented           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add a default of ``lambd=1.0`` to :func:`random.expovariate`.                                       | Not implemented [#exv]_   |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `shutil <https://docs.python.org/3/whatsnew/3.12.html#shutil>`_                                                                 |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :func:`shutil.make_archive` now passes the ``root_dir`` argument to custom archivers which support  | Not implemented [#mplib]_ |
  | it, and no longer temporarily changes the current working directory.                                |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :func:`shutil.rmtree` now accepts a new argument ``onexc`` which is an error handler like           | Not implemented [#mplib]_ |
  | ``onerror`` but which expects an exception instance rather than a ``(typ, val, tb)`` triplet.       |                           |
  | ``onerror`` is deprecated.                                                                          |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :func:`shutil.which` now consults ``PATHEXT``, calls ``NeedCurrentDirectoryForExePathW`` and        | Not relevant [#mplib]_    |
  | prefers a ``PATHEXT`` match over a direct match elsewhere in the search path, on Windows.           |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `sqlite3 <https://docs.python.org/3/whatsnew/3.12.html#sqlite3>`_                                                               |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add a command-line interface, the :attr:`sqlite3.Connection.autocommit` attribute and               | Not relevant [#sqlite]_   |
  | ``autocommit`` parameter, an ``entrypoint`` parameter to :meth:`sqlite3.Connection.load_extension`, |                           |
  | and :meth:`sqlite3.Connection.getconfig` and :meth:`sqlite3.Connection.setconfig`.                  |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `statistics <https://docs.python.org/3/whatsnew/3.12.html#statistics>`_                                                         |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Extend :func:`statistics.correlation` to include a ``ranked`` method for computing the Spearman     | Not implemented [#nomod]_ |
  | correlation of ranked data.                                                                         |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `sys <https://docs.python.org/3/whatsnew/3.12.html#sys>`_                                                                       |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add the :mod:`sys.monitoring` namespace to expose the new :pep:`669` monitoring API.                | Not implemented [#trace]_ |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :func:`sys.activate_stack_trampoline`, :func:`sys.deactivate_stack_trampoline` and              | Not relevant              |
  | :func:`sys.is_stack_trampoline_active`.                                                             |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :data:`sys.last_exc`, which holds the last unhandled exception that was raised.  The legacy     | Not implemented           |
  | :data:`sys.last_type`, :data:`sys.last_value` and :data:`sys.last_traceback` are deprecated.        |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :func:`sys._current_exceptions` now returns a mapping from thread-id to an exception instance,      | Not implemented           |
  | rather than to a ``(typ, exc, tb)`` tuple.                                                          |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | The recursion limit set by :func:`sys.setrecursionlimit` now applies only to Python code; builtin   | Not implemented [#stk]_   |
  | functions are protected by a different mechanism.                                                   |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `tempfile <https://docs.python.org/3/whatsnew/3.12.html#tempfile>`_                                                             |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | The :func:`tempfile.NamedTemporaryFile` function has a new optional parameter ``delete_on_close``.  | Not implemented [#mplib]_ |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :func:`tempfile.mkdtemp` now always returns an absolute path, even if the argument provided to the  | Not implemented [#mplib]_ |
  | ``dir`` parameter is a relative path.                                                               |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `threading <https://docs.python.org/3/whatsnew/3.12.html#threading>`_                                                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :func:`threading.settrace_all_threads` and :func:`threading.setprofile_all_threads` that allow  | Not implemented [#trace]_ |
  | setting tracing and profiling functions in all running threads in addition to the calling one.      |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `tkinter <https://docs.python.org/3/whatsnew/3.12.html#tkinter>`_                                                               |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :meth:`tkinter.Canvas.coords` now flattens its arguments.                                           | Not relevant [#nomod]_    |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `tokenize <https://docs.python.org/3/whatsnew/3.12.html#tokenize>`_                                                             |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | The :mod:`tokenize` module includes the changes introduced in :pep:`701`.                           | Not relevant [#nomod]_    |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `types <https://docs.python.org/3/whatsnew/3.12.html#types>`_                                                                   |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add :func:`types.get_original_bases` to allow for further introspection of user-defined generic     | Not implemented [#mplib]_ |
  | types when subclassed.                                                                              |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `typing <https://docs.python.org/3/whatsnew/3.12.html#typing>`_                                                                 |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | :func:`isinstance` checks against runtime-checkable protocols now use                               | Not implemented [#ann]_   |
  | :func:`inspect.getattr_static` rather than :func:`hasattr`, the members of a runtime-checkable      |                           |
  | protocol are now frozen when the class is created, and such checks are significantly faster.        |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | All :class:`typing.TypedDict` and :class:`typing.NamedTuple` classes now have the                   | Not implemented [#ann]_   |
  | ``__orig_bases__`` attribute.                                                                       |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add a ``frozen_default`` parameter to :func:`@typing.dataclass_transform                            | Not implemented [#ann]_   |
  | <typing.dataclass_transform>`.                                                                      |                           |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `unicodedata <https://docs.python.org/3/whatsnew/3.12.html#unicodedata>`_                                                       |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | The Unicode database has been updated to version 15.0.0.                                            | Not relevant [#nomod]_    |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `unittest <https://docs.python.org/3/whatsnew/3.12.html#unittest>`_                                                             |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add a ``--durations`` command line option, showing the N slowest test cases.                        | Not implemented [#mplib]_ |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | `uuid <https://docs.python.org/3/whatsnew/3.12.html#uuid>`_                                                                     |
  +-----------------------------------------------------------------------------------------------------+---------------------------+
  | Add a command-line interface.                                                                       | Not implemented [#nomod]_ |
  +-----------------------------------------------------------------------------------------------------+---------------------------+

.. rubric:: Notes

.. [#p695] None of the type parameter syntax is recognised: ``type X = int``, ``def f[T]()`` and
   ``class C[T]`` all raise :exc:`SyntaxError`, and there is no ``__type_params__`` attribute on
   functions, classes or type aliases.  See also [#ann]_.

.. [#f701] MicroPython's f-string support is implemented in the lexer, which rewrites the literal
   into a ``str.format()`` call.  Most of what :pep:`701` allows already works: replacement fields
   may reuse the enclosing quote character (including deeply nested f-strings), may contain
   backslashes and escape sequences, may span multiple lines, and may contain ``#`` comments.  The
   lexer is however not fully aware of Python syntax inside ``{...}``, so an expression containing a
   ``}`` or ``:`` character inside a nested string literal -- for example ``f'{"}"}'`` or
   ``f'{"a:b"}'`` -- is still rejected with a :exc:`SyntaxError`.  Adjacent-literal concatenation
   where the non-f-string part contains braces is also unsupported.  See
   ``tests/cpydiff/core_fstring_parser.py`` and ``tests/cpydiff/core_fstring_concat.py``.

.. [#p709] MicroPython compiles comprehensions into a separate implicit function, so a comprehension
   still appears as its own frame (``<listcomp>``, ``<dictcomp>``, ``<setcomp>``, ``<genexpr>``) in
   tracebacks, and comprehension iteration variables are not visible in the enclosing scope.

.. [#trace] MicroPython has no ``sys.monitoring`` namespace and no ``threading`` tracing hooks.  The
   closest analogue is ``sys.settrace()``, which is gated behind ``MICROPY_PY_SYS_SETTRACE`` and is
   disabled by default on all ports (it is enabled only in the unix ``coverage`` variant).

.. [#buf] MicroPython has a full C-level buffer protocol, used by :class:`memoryview`,
   :class:`bytearray`, :mod:`array` and many built-in modules, but it is not reachable from Python
   code: defining ``__buffer__``/``__release_buffer__`` on a class has no effect and
   ``memoryview(obj)`` raises :exc:`TypeError`.  There is also no ``collections.abc`` namespace and
   therefore no ``collections.abc.Buffer``.

.. [#stk] MicroPython protects against stack overflow with its own stack-limit check
   (``MICROPY_STACK_CHECK``), raising ``RuntimeError: maximum recursion depth exceeded``.  The limit
   is a byte budget configured by the port rather than a frame count, so there is no
   ``sys.setrecursionlimit()`` or ``sys.getrecursionlimit()``.

.. [#nul] MicroPython does not reject null bytes in source code.  A null byte inside a comment or a
   string literal is accepted and passed through; only a null byte appearing where a token is
   expected produces a :exc:`SyntaxError`, and then only incidentally.

.. [#warn] MicroPython has no ``warnings`` module and issues no compile-time diagnostics, so neither
   invalid escape sequences nor out-of-range octal escapes are reported.  The resulting values match
   CPython: ``"\d"`` is a two-character string and ``"\777"`` is ``"\u01ff"``.  A bytes literal
   such as ``b"\777"`` is a :exc:`SyntaxError`, as in CPython.

.. [#note] MicroPython never wrapped :meth:`~object.__set_name__` exceptions in a
   :exc:`RuntimeError`, so the exception already propagates unchanged.  However :pep:`678` exception
   notes are not supported at all -- there is no ``add_note()`` method and no ``__notes__``
   attribute -- so the added context information is missing.

.. [#mv] MicroPython's :class:`memoryview` has no ``cast()`` method, so no format code can be
   reinterpreted, half-float or otherwise.

.. [#slice] :class:`slice` objects are not hashable in MicroPython.  In addition, ``slice`` cannot
   be instantiated directly (``slice(1, 2)`` raises ``TypeError: can't create 'slice' instances``);
   slice objects only ever arise from subscript syntax.

.. [#aio] MicroPython's :mod:`asyncio` is already implemented partly in C (the scheduler, task queue
   and ``current_task()``), so this particular CPython optimisation does not apply.

.. [#exv] :func:`random.expovariate` is not provided by MicroPython's :mod:`random` module at all,
   so the new default is moot.

.. [#nfa] :func:`math.nextafter` is not provided by MicroPython's :mod:`math` module at all, so the
   new ``steps`` argument is moot.

.. [#sqlite] MicroPython has no :mod:`sqlite3` module.  A ``sqlite3`` FFI wrapper exists in the
   ``unix-ffi`` section of `micropython-lib
   <https://github.com/micropython/micropython-lib>`__, but it is a thin binding to the system
   library and implements none of these APIs.

.. [#nomod] MicroPython provides no equivalent module, neither built into the firmware nor as a
   `micropython-lib <https://github.com/micropython/micropython-lib>`__ package, so there is nothing
   for this change to apply to.

.. [#mplib] This module is not built into MicroPython.  It is available only as a pure-Python
   package in `micropython-lib <https://github.com/micropython/micropython-lib>`__, installable with
   ``mip`` or ``mpremote mip install``.  These packages implement a deliberately reduced subset of
   the CPython API, so features added to CPython are generally absent unless they are needed on a
   microcontroller.

.. [#ann] MicroPython parses type annotations but never evaluates them, so annotations produce no
   runtime effects and no ``__annotations__`` attributes are created.  There is also no ``typing``
   module and no ``__future__`` module, so these features cannot be used other than as inert
   annotations.
