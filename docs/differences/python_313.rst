.. _python_313:

Python 3.13
===========

Python 3.13.0 (final) was released on the 7 October 2024.  Unlike earlier
releases, Python 3.13 has no "features for 3.13" PEP; `PEP 719
<https://www.python.org/dev/peps/pep-0719/>`_ defines the release schedule only.
A detailed description of the changes can be found in
`What's New in Python 3.13 <https://docs.python.org/3.13/whatsnew/3.13.html>`_.

Python 3.13 added no new syntax to the language, so there is no "new syntax
features" section below.  Its headline features -- the free-threaded build, the
experimental JIT, the new interactive interpreter and mobile platform support --
are almost entirely concerned with the internals of CPython or with host
operating systems, and so have little bearing on MicroPython.

Unlike the pages for earlier versions, this page also covers modules that
MicroPython provides only through `micropython-lib
<https://github.com/micropython/micropython-lib>`_ rather than compiling into
the firmware; such rows are marked with a footnote.

.. table::
  :widths: 20 60 20

  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | **Interpreter improvements**                                                                                      | **Status**               |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `PEP 703 <https://www.python.org/dev/peps/pep-0703/>`_       | Making the Global Interpreter Lock Optional in     | Not relevant [#gil]_     |
  |                                                              | CPython                                            |                          |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `PEP 744 <https://www.python.org/dev/peps/pep-0744/>`_       | An experimental just-in-time (JIT) compiler        | Not relevant [#nat]_     |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `PEP 667 <https://www.python.org/dev/peps/pep-0667/>`_       | Consistent views of namespaces                     | Not implemented [#loc]_  |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `gh-111201                                                   | A better interactive interpreter, based on code    | Partial [#repl]_         |
  | <https://github.com/python/cpython/issues/111201>`_          | from the PyPy project                              |                          |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `gh-112730                                                   | Improved error messages: colourised tracebacks and | Not implemented [#err]_  |
  | <https://github.com/python/cpython/issues/112730>`_          | suggestions for mistyped names                     |                          |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | **Platform support**                                                                                                                         |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `PEP 730 <https://www.python.org/dev/peps/pep-0730/>`_       | Support for mobile platforms: Apple iOS is now a   | Not relevant             |
  |                                                              | tier 3 platform                                    |                          |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `PEP 738 <https://www.python.org/dev/peps/pep-0738/>`_       | Support for mobile platforms: Android is now a     | Not relevant             |
  |                                                              | tier 3 platform                                    |                          |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `PEP 11 <https://www.python.org/dev/peps/pep-0011/>`_        | ``wasm32-wasi`` is promoted to a tier 2 platform   | Not relevant [#wasm]_    |
  |                                                              | and ``wasm32-emscripten`` is demoted to tier 3     |                          |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | **New typing features**                                                                                                                      |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `PEP 696 <https://www.python.org/dev/peps/pep-0696/>`_       | Type defaults for type parameters                  | Not implemented [#ann]_  |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `PEP 702 <https://www.python.org/dev/peps/pep-0702/>`_       | Marking deprecations using the type system         | Not implemented [#warn]_ |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `PEP 705 <https://www.python.org/dev/peps/pep-0705/>`_       | TypedDict: read-only items                         | Not implemented [#ann]_  |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `PEP 742 <https://www.python.org/dev/peps/pep-0742/>`_       | Narrowing types with ``TypeIs``                    | Not implemented [#ann]_  |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | **Important deprecations, removals or restrictions**                                                                                         |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `PEP 594 <https://www.python.org/dev/peps/pep-0594/>`_       | Remove 'dead batteries' from the standard library  | Not relevant [#lib]_     |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `gh-104780                                                   | The **2to3** program and the ``lib2to3`` module    | Not relevant             |
  | <https://github.com/python/cpython/issues/104780>`_          | have been removed                                  |                          |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `gh-89519                                                    | Support for chained :func:`classmethod`            | Complete [#cm]_          |
  | <https://github.com/python/cpython/issues/89519>`_           | descriptors has been removed; they can no longer   |                          |
  |                                                              | wrap other descriptors such as :func:`property`    |                          |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `gh-79932                                                    | Calling ``frame.clear()`` on a suspended frame now | Not relevant [#loc]_     |
  | <https://github.com/python/cpython/issues/79932>`_           | raises :exc:`RuntimeError`                         |                          |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+
  | `Removed modules and APIs                                    | A number of long-deprecated APIs in host-oriented  | Not relevant             |
  | <https://docs.python.org/3.13/whatsnew/3.13.html#removed>`_  | modules such as ``configparser``, ``locale``,      |                          |
  |                                                              | ``tkinter``, ``typing`` and ``importlib.metadata`` |                          |
  |                                                              | have been removed                                  |                          |
  +--------------------------------------------------------------+----------------------------------------------------+--------------------------+


Other Language Changes:

.. table::
  :widths: 90 10

  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | The compiler now strips common leading whitespace from every line in a docstring, reducing the size of the  | Not relevant [#doc]_     |
  | bytecode cache.                                                                                             |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Annotation scopes within class scopes can now contain lambdas and comprehensions. Comprehensions that are   | Not implemented [#ann]_  |
  | located within class scopes are not inlined into their parent scope.                                        |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Future statements are no longer triggered by relative imports of the ``__future__`` module, meaning that    | Not relevant [#fut]_     |
  | statements of the form ``from .__future__ import ...`` are now simply standard relative imports, with no    |                          |
  | special features activated.                                                                                 |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | ``global`` declarations are now permitted in ``except`` blocks when that global is used in the ``else``     | Complete                 |
  | block. Previously this raised an erroneous :exc:`SyntaxError`.                                              |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add ``PYTHON_FROZEN_MODULES``, a new environment variable that determines whether frozen modules are        | Not relevant [#frz]_     |
  | ignored by the import machinery, equivalent to the ``-X frozen_modules`` command-line option.               |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add support for the perf profiler working without frame pointers through the new environment variable       | Not relevant             |
  | ``PYTHON_PERF_JIT_SUPPORT`` and command-line option ``-X perf_jit``.                                        |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | The location of a ``.python_history`` file can be changed via the new ``PYTHON_HISTORY`` environment        | Not relevant [#repl]_    |
  | variable.                                                                                                   |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Classes have a new ``__static_attributes__`` attribute, populated by the compiler with a tuple of the       | Not implemented          |
  | class's attribute names which are assigned through ``self.<name>`` from any function in its body.           |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | The compiler now creates a ``__firstlineno__`` attribute on classes with the line number of the first line  | Not implemented          |
  | of the class definition.                                                                                    |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | The :func:`exec` and :func:`eval` builtins now accept the *globals* and *locals* arguments as keywords.     | Not implemented          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | The :func:`compile` builtin now accepts a new flag, ``ast.PyCF_OPTIMIZED_AST``, which is similar to         | Not relevant [#ast]_     |
  | ``ast.PyCF_ONLY_AST`` except that the returned AST is optimized according to the value of the *optimize*    |                          |
  | argument.                                                                                                   |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add a ``__name__`` attribute on :func:`property` objects.                                                   | Not implemented          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add ``PythonFinalizationError``, a new exception derived from :exc:`RuntimeError` and used to signal when   | Not implemented          |
  | operations are blocked during finalization.                                                                 |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Allow the *count* argument of :meth:`str.replace` to be a keyword.                                          | Not implemented          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Many functions now emit a warning if a boolean value is passed as a file descriptor argument.               | Not relevant [#warn]_    |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Added ``name`` and ``mode`` attributes for compressed and archived file-like objects in the ``bz2``,        | Not relevant [#arch]_    |
  | ``lzma``, ``tarfile`` and ``zipfile`` modules.                                                              |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+

Changes to built-in and library modules:

.. table::
  :widths: 90 10

  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `argparse <https://docs.python.org/3.13/whatsnew/3.13.html#argparse>`_                                                                 |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add the *deprecated* parameter to the :meth:`~argparse.ArgumentParser.add_argument` and ``add_parser()``    | Not implemented [#lib]_  |
  | methods, to enable deprecating command-line options, positional arguments and subcommands.                  |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `array <https://docs.python.org/3.13/whatsnew/3.13.html#array>`_                                                                       |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add the ``'w'`` type code (``Py_UCS4``) for Unicode characters. It should be used instead of the deprecated | Not implemented [#arr]_  |
  | ``'u'`` type code.                                                                                          |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Register :class:`array.array` as a :class:`~collections.abc.MutableSequence` by implementing the            | Not implemented          |
  | ``clear()`` method.                                                                                         |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `asyncio <https://docs.python.org/3.13/whatsnew/3.13.html#asyncio>`_                                                                   |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | :func:`asyncio.as_completed` now returns an object that is both an asynchronous iterator and a plain        | Not implemented          |
  | iterator of awaitables.                                                                                     |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | ``asyncio.loop.create_unix_server()`` will now automatically remove the Unix socket when the server is      | Not relevant             |
  | closed.                                                                                                     |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | ``DatagramTransport.sendto()`` will now send zero-length datagrams if called with an empty bytes object,    | Not relevant [#dgram]_   |
  | and transport flow control now accounts for the datagram header.                                            |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add ``Queue.shutdown`` and ``QueueShutDown`` to manage queue termination.                                   | Not implemented [#aioq]_ |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add the ``Server.close_clients()`` and ``Server.abort_clients()`` methods, which more forcefully close an   | Not implemented          |
  | asyncio server.                                                                                             |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Accept a tuple of separators in ``StreamReader.readuntil()``, stopping when any one of them is encountered. | Not implemented [#ru]_   |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Several improvements to ``TaskGroup``: better behaviour when an external cancellation collides with an      | Not implemented [#tg]_   |
  | internal one, preservation of the cancellation count, closing the coroutine when ``create_task()`` is       |                          |
  | called on an inactive group, and a new ``**kwargs`` argument passed through to the task constructor.        |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `base64 <https://docs.python.org/3.13/whatsnew/3.13.html#base64>`_                                                                     |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add ``z85encode()`` and ``z85decode()`` functions for encoding :class:`bytes` as Z85 data and decoding      | Not implemented [#lib]_  |
  | Z85-encoded data to ``bytes``.                                                                              |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `copy <https://docs.python.org/3.13/whatsnew/3.13.html#copy>`_                                                                         |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | The new ``copy.replace()`` function and the ``__replace__()`` protocol make creating modified copies of     | Not implemented [#lib]_  |
  | objects much simpler.                                                                                       |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `dbm <https://docs.python.org/3.13/whatsnew/3.13.html#dbm>`_                                                                           |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add ``dbm.sqlite3``, a new module which implements an SQLite backend, and make it the default ``dbm``       | Not relevant [#btree]_   |
  | backend. Also allow removing all items through the new ``clear()`` methods of the GDBM and NDBM database    |                          |
  | objects.                                                                                                    |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `glob <https://docs.python.org/3.13/whatsnew/3.13.html#glob>`_                                                                         |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add ``translate()``, a function to convert a path specification with shell-style wildcards to a regular     | Not implemented [#ffi]_  |
  | expression.                                                                                                 |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `io <https://docs.python.org/3.13/whatsnew/3.13.html#io>`_                                                                             |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | The :class:`io.IOBase` finalizer now logs any errors raised by the ``close()`` method with                  | Not implemented          |
  | ``sys.unraisablehook``. Previously, errors were ignored silently by default.                                |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `ipaddress <https://docs.python.org/3.13/whatsnew/3.13.html#ipaddress>`_                                                               |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add the ``IPv4Address.ipv6_mapped`` property, which returns the IPv4-mapped IPv6 address.                   | Not implemented [#mod]_  |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Fix ``is_global`` and ``is_private`` behaviour in ``IPv4Address``, ``IPv6Address``, ``IPv4Network`` and     | Not implemented [#mod]_  |
  | ``IPv6Network``.                                                                                            |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `itertools <https://docs.python.org/3.13/whatsnew/3.13.html#itertools>`_                                                               |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | ``batched()`` has a new *strict* parameter, which raises a :exc:`ValueError` if the final batch is shorter  | Not implemented [#bat]_  |
  | than the specified batch size.                                                                              |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `math <https://docs.python.org/3.13/whatsnew/3.13.html#math>`_                                                                         |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | The new function :func:`math.fma` performs fused multiply-add operations, computing ``x * y + z`` with only | Not implemented          |
  | a single round.                                                                                             |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `mimetypes <https://docs.python.org/3.13/whatsnew/3.13.html#mimetypes>`_                                                               |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add the ``guess_file_type()`` function to guess a MIME type from a filesystem path. Using paths with        | Not implemented [#mod]_  |
  | ``guess_type()`` is now soft deprecated.                                                                    |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `os <https://docs.python.org/3.13/whatsnew/3.13.html#os>`_                                                                             |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add the ``process_cpu_count()`` function to get the number of logical CPU cores usable by the calling       | Not implemented          |
  | thread of the current process.                                                                              |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | ``cpu_count()`` and ``process_cpu_count()`` can be overridden through the new environment variable          | Not relevant             |
  | ``PYTHON_CPU_COUNT`` or the new command-line option ``-X cpu_count``.                                       |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add a low level interface to Linux's *timer file descriptors* via ``timerfd_create()`` and related          | Not relevant             |
  | functions and constants.                                                                                    |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | ``lchmod()``, ``fchmod()`` and the *follow_symlinks* argument of ``chmod()`` are now available on Windows,  | Not relevant             |
  | and ``mkdir()`` and ``makedirs()`` now support passing a *mode* value of ``0o700``.                         |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | ``posix_spawn()`` now accepts ``None`` for the *env* argument, and can use the ``POSIX_SPAWN_CLOSEFROM``    | Not relevant             |
  | attribute in the *file_actions* parameter.                                                                  |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `os.path <https://docs.python.org/3.13/whatsnew/3.13.html#os-path>`_                                                                   |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add ``isreserved()`` to check if a path is reserved on the current system. This function is only available  | Not relevant             |
  | on Windows.                                                                                                 |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | On Windows, ``isabs()`` no longer considers paths starting with exactly one slash to be absolute, and       | Not relevant             |
  | ``realpath()`` now resolves MS-DOS style file names even if the file is not accessible.                     |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `pathlib <https://docs.python.org/3.13/whatsnew/3.13.html#pathlib>`_                                                                   |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add ``UnsupportedOperation``, which is raised instead of :exc:`NotImplementedError` when a path operation   | Not implemented [#lib]_  |
  | isn't supported.                                                                                            |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add ``Path.from_uri()``, a new constructor for creating ``Path`` objects from 'file' URIs, and              | Not implemented [#lib]_  |
  | ``PurePath.full_match()`` for matching paths with shell-style wildcards.                                    |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add the ``PurePath.parser`` class attribute to store the implementation of :mod:`os.path` used for          | Not implemented [#lib]_  |
  | low-level path parsing and joining.                                                                         |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add the *recurse_symlinks* keyword-only argument to ``Path.glob()`` and ``rglob()``, which now also return  | Not implemented [#lib]_  |
  | files as well as directories when given a pattern ending with ``**``.                                       |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add the *follow_symlinks* keyword-only argument to ``Path.is_file()``, ``Path.is_dir()``, ``Path.owner()``  | Not implemented [#lib]_  |
  | and ``Path.group()``.                                                                                       |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `queue <https://docs.python.org/3.13/whatsnew/3.13.html#queue>`_                                                                       |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add ``Queue.shutdown`` and ``ShutDown`` to manage queue termination.                                        | Not implemented [#q]_    |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `random <https://docs.python.org/3.13/whatsnew/3.13.html#random>`_                                                                     |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add a command-line interface.                                                                               | Not relevant             |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `re <https://docs.python.org/3.13/whatsnew/3.13.html#re>`_                                                                             |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Rename ``re.error`` to ``PatternError`` for improved clarity. ``re.error`` is kept for backward             | Not implemented [#re]_   |
  | compatibility.                                                                                              |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `shutil <https://docs.python.org/3.13/whatsnew/3.13.html#shutil>`_                                                                     |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Support the *dir_fd* and *follow_symlinks* keyword arguments in ``chown()``.                                | Not relevant             |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `site <https://docs.python.org/3.13/whatsnew/3.13.html#site>`_                                                                         |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | ``.pth`` files are now decoded using UTF-8 first, and then with the locale encoding if UTF-8 decoding       | Not relevant             |
  | fails.                                                                                                      |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `sqlite3 <https://docs.python.org/3.13/whatsnew/3.13.html#sqlite3>`_                                                                   |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | A :exc:`ResourceWarning` is now emitted if a ``Connection`` object is not closed explicitly, and a *filter* | Not relevant [#ffi]_     |
  | keyword-only parameter was added to ``Connection.iterdump()``.                                              |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `ssl <https://docs.python.org/3.13/whatsnew/3.13.html#ssl>`_                                                                           |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | The ``create_default_context()`` API now includes ``VERIFY_X509_PARTIAL_CHAIN`` and ``VERIFY_X509_STRICT``  | Not relevant [#tls]_     |
  | in its default flags.                                                                                       |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `statistics <https://docs.python.org/3.13/whatsnew/3.13.html#statistics>`_                                                             |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add ``kde()`` for kernel density estimation and ``kde_random()`` for sampling from an estimated probability | Not implemented [#mod]_  |
  | density function.                                                                                           |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `subprocess <https://docs.python.org/3.13/whatsnew/3.13.html#subprocess>`_                                                             |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | The ``subprocess`` module now uses the ``posix_spawn()`` function in more situations.                       | Not relevant [#mod]_     |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `sys <https://docs.python.org/3.13/whatsnew/3.13.html#sys>`_                                                                           |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add the ``sys._is_interned()`` function to test if a string was interned. This function is not guaranteed   | Not implemented [#int]_  |
  | to exist in all implementations of Python.                                                                  |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `tempfile <https://docs.python.org/3.13/whatsnew/3.13.html#tempfile>`_                                                                 |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | On Windows, the default mode ``0o700`` used by ``tempfile.mkdtemp()`` now limits access to the new          | Not relevant             |
  | directory.                                                                                                  |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `time <https://docs.python.org/3.13/whatsnew/3.13.html#time>`_                                                                         |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | On Windows, :func:`time.monotonic` and :func:`time.time` now use higher resolution clocks, giving a         | Not relevant             |
  | resolution of 1 microsecond instead of 15.6 milliseconds.                                                   |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `traceback <https://docs.python.org/3.13/whatsnew/3.13.html#traceback>`_                                                               |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add the ``exc_type_str`` attribute to ``TracebackException`` and deprecate the ``exc_type`` attribute. Add  | Not implemented [#tb]_   |
  | the *save_exc_type* parameter to indicate whether ``exc_type`` should be saved.                             |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add a new *show_group* keyword-only parameter to ``TracebackException.format_exception_only()`` to          | Not implemented [#tb]_   |
  | recursively format the nested exceptions of a ``BaseExceptionGroup``.                                       |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `types <https://docs.python.org/3.13/whatsnew/3.13.html#types>`_                                                                       |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | ``SimpleNamespace`` can now take a single positional argument to initialise the namespace's arguments. This | Not implemented [#ty]_   |
  | argument must either be a mapping or an iterable of key-value pairs.                                        |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `typing <https://docs.python.org/3.13/whatsnew/3.13.html#typing>`_                                                                     |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add ``ReadOnly``, ``TypeIs``, ``NoDefault``, ``get_protocol_members()`` and ``is_protocol()``, and allow    | Not implemented [#ann]_  |
  | ``ClassVar`` to be nested in ``Final`` and vice versa.                                                      |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `warnings <https://docs.python.org/3.13/whatsnew/3.13.html#warnings>`_                                                                 |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | The new ``warnings.deprecated()`` decorator provides a way to communicate deprecations to a static type     | Not implemented [#warn]_ |
  | checker and to warn on usage of deprecated classes and functions.                                           |                          |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | `zipimport <https://docs.python.org/3.13/whatsnew/3.13.html#zipimport>`_                                                               |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+
  | Add support for ZIP64 format files.                                                                         | Not relevant [#arch]_    |
  +-------------------------------------------------------------------------------------------------------------+--------------------------+

.. rubric:: Notes

.. [#gil] MicroPython has no CPython-style global interpreter lock to remove.  Ports
   that support threading (``MICROPY_PY_THREAD``) either run threads on a single core
   under a scheduler lock, or use a port-specific mutex around the interpreter and
   garbage collector; the trade-offs are quite different to those of CPython.

.. [#nat] MicroPython has no tracing JIT.  It does provide ahead-of-time native code
   generation through the ``@micropython.native``, ``@micropython.viper`` and
   ``@micropython.asm_thumb`` decorators, and ``.mpy`` files may contain
   pre-compiled native code, but these are opt-in per function rather than an
   automatic optimisation of hot loops.

.. [#loc] MicroPython does not implement :pep:`667`, and its starting point differs
   from CPython's in any case: :func:`locals` returns the *globals* dictionary when
   called inside a function, because local variables live on the stack and are not
   reified into a dictionary.  Frame objects are only available when
   ``MICROPY_PY_SYS_SETTRACE`` is enabled, and those frames expose ``f_globals``,
   ``f_code``, ``f_lasti`` and ``f_back`` but no ``f_locals`` and no ``clear()``
   method, so neither the new write-through proxy nor the new :exc:`RuntimeError`
   applies.

.. [#repl] MicroPython has its own REPL implementation (``MICROPY_HELPER_REPL``) which
   already provides several of the features listed here, including line editing with
   Emacs-style key bindings, word-wise cursor movement, an in-memory command history
   (``MICROPY_READLINE_HISTORY_SIZE``, eight entries by default), automatic
   indentation of continuation lines, tab completion of names, and a paste mode
   entered with ``Ctrl-E``.  What it does not provide is colour output, the ``F1``,
   ``F2`` and ``F3`` bindings, a history file such as ``.python_history``, or bare
   ``exit``, ``quit`` and ``help`` commands -- ``exit`` and ``quit`` are not builtins
   in MicroPython, so they raise :exc:`NameError` unless imported from :mod:`sys`.

.. [#err] MicroPython deliberately trades error message quality for code size.  The
   ``MICROPY_ERROR_REPORTING`` option selects between ``NONE``, ``TERSE``, ``NORMAL``
   and ``DETAILED``; even ``DETAILED``, the default on the unix port, does not attempt
   the "did you mean" suggestions or the colourised output added in CPython 3.13, and
   most microcontroller ports default to ``NORMAL`` or ``TERSE``.

.. [#wasm] MicroPython has its own ``webassembly`` port, built with Emscripten and
   published as the ``@micropython/micropython-webassembly-pyscript`` package.  It is
   independent of CPython's WebAssembly work, so CPython's platform tier changes do not
   affect it.

.. [#ann] MicroPython parses type annotations but never evaluates them, so annotations
   produce no runtime effects and no ``__annotations__`` attributes are created.  There
   is also no ``typing`` module, so these features cannot be used other than as inert
   annotations.

.. [#warn] MicroPython core has no warnings machinery at all: there is no ``warnings``
   module built into the firmware and no :exc:`DeprecationWarning`,
   :exc:`SyntaxWarning` or :exc:`ResourceWarning` exception types.  The ``warnings``
   module in micropython-lib [#lib]_ provides only a ``warn()`` function that prints its
   argument.  Every CPython change described as "now emits a warning" therefore has no
   MicroPython counterpart.

.. [#lib] This module is not built into MicroPython firmware.  It is a pure-Python
   package distributed through `micropython-lib
   <https://github.com/micropython/micropython-lib>`_ and must be installed explicitly,
   for example with ``mpremote mip install <name>``.

.. [#ffi] This module is only available in the ``unix-ffi`` section of micropython-lib.
   It depends on FFI access to the host C library and so cannot be used on
   microcontroller ports.

.. [#mod] MicroPython has no counterpart to this module, in either the firmware or
   micropython-lib.

.. [#cm] MicroPython never supported chaining a :func:`classmethod` around another
   descriptor: applying ``@classmethod`` to a :func:`property` yields a bound method
   wrapping the property object rather than invoking it.  CPython 3.13 removing the
   feature therefore brings CPython into line with MicroPython's existing behaviour.

.. [#doc] The point is moot for MicroPython, which goes further than the CPython
   optimisation: ``MICROPY_ENABLE_DOC_STRING`` defaults to ``0``, so docstrings are
   discarded by the compiler entirely and no ``__doc__`` attributes are created.

.. [#fut] MicroPython's compiler has no notion of future statements at all, so neither
   ``from __future__ import ...`` nor ``from .__future__ import ...`` activates any
   special behaviour.  micropython-lib [#lib]_ ships a stub ``__future__`` module so
   that such imports do not fail.

.. [#frz] MicroPython freezes modules into the firmware at build time as a core part of
   how ports are built, and there is no run-time option to ignore them.

.. [#ast] MicroPython has no ``ast`` module and does not expose an abstract syntax tree;
   :func:`compile` (where enabled) accepts only the standard modes and no flags
   argument.

.. [#arch] MicroPython's compression support is provided by the ``deflate`` module,
   which offers a single ``DeflateIO`` stream wrapper, and by the ``tarfile`` and
   ``zlib`` modules in micropython-lib [#lib]_.  There is no ``bz2``, ``lzma``,
   ``zipfile`` or ``zipimport`` module.

.. [#arr] MicroPython's :mod:`array` module supports the type codes ``b``, ``B``, ``h``,
   ``H``, ``i``, ``I``, ``l``, ``L``, ``q``, ``Q``, ``f`` and ``d``.  It has never
   supported ``u``, so the deprecation itself has no effect, but the replacement ``w``
   type code is not implemented either.

.. [#dgram] MicroPython's ``asyncio`` has no transport and protocol layer, so there is
   no ``DatagramTransport``; datagrams are sent with the ``socket`` module directly.

.. [#aioq] MicroPython's ``asyncio`` has no ``Queue`` class at all.  ``ThreadSafeFlag``
   and ``Event`` are the usual means of signalling between tasks.

.. [#ru] MicroPython's ``asyncio.StreamReader`` provides ``read()``, ``readline()`` and
   ``readexactly()``, but no ``readuntil()``.

.. [#tg] MicroPython's ``asyncio`` has no ``TaskGroup``; ``gather()`` is the closest
   equivalent.

.. [#bat] This is a refinement of ``itertools.batched()``, which was itself added in
   CPython 3.12 and is not implemented by MicroPython, so the *strict* parameter has
   nothing to apply to.

.. [#q] MicroPython has no ``queue`` module, in either the firmware or micropython-lib.
   :class:`collections.deque` and ``asyncio.ThreadSafeFlag`` are the usual substitutes.

.. [#re] MicroPython's :mod:`re` module defines neither ``re.error`` nor
   ``PatternError``; a bad pattern raises a plain :exc:`ValueError`.  Code that needs to
   be portable should therefore catch :exc:`ValueError`, which both spellings of the
   CPython exception derive from.

.. [#tls] MicroPython's ``ssl`` module is implemented on top of Mbed TLS (axTLS on a few
   ports), not OpenSSL, and does not provide ``create_default_context()`` or the
   ``VERIFY_X509_*`` constants.  Certificate verification is configured through the
   ``cert_reqs`` and ``cadata`` arguments to ``ssl.SSLContext`` instead.

.. [#int] MicroPython interns every string that is used as an identifier, as a "qstr" in
   a global pool, and :func:`sys.intern` is available when
   ``MICROPY_PY_SYS_INTERN`` is enabled.  There is no way to query whether a given
   string has been interned.

.. [#tb] The ``traceback`` module in micropython-lib [#lib]_ provides only
   ``format_tb()``, ``format_exception()``, ``format_exception_only()``,
   ``print_exception()``, ``print_exc()`` and ``format_exc()``.  There is no
   ``TracebackException`` class for these changes to apply to.

.. [#ty] The ``types`` module in micropython-lib [#lib]_ defines ``SimpleNamespace`` as
   ``None``, along with ``CodeType``, ``MappingProxyType``, ``TracebackType`` and
   ``FrameType``, because MicroPython does not expose the corresponding objects.

.. [#btree] MicroPython's nearest equivalent to ``dbm`` is the ``btree`` module, which
   wraps the BerkeleyDB 1.xx library and provides a persistent key/value store with a
   dict-like interface.
