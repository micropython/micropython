.. _python_311:

Python 3.11
===========

Python 3.11.0 (final) was released on the 24 October 2022.  Unlike Python 3.10 there is no PEP
enumerating the features for this release -- `PEP 664 <https://peps.python.org/pep-0664/>`_ only
defines the release schedule -- so the tables below follow `What's New in Python 3.11
<https://docs.python.org/3/whatsnew/3.11.html>`_ instead.

Unlike the pages for Python 3.5 to 3.10, this page also covers modules that MicroPython provides
only as pure-Python packages in `micropython-lib
<https://github.com/micropython/micropython-lib>`_, such as ``contextlib``, ``datetime``,
``functools``, ``inspect``, ``locale``, ``logging``, ``operator``, ``pathlib``, ``shutil``,
``string``, ``threading``, ``traceback``, ``unittest`` and ``warnings``.  Those packages are
installed with ``mip`` or frozen into a firmware build, they are not part of the interpreter
itself.  Rows that concern them are marked with [#mplib]_, or their note names the micropython-lib
package explicitly.

Note that MicroPython's coverage of CPython does not advance strictly in version order.  Some much
later features are already available -- for example :pep:`750` template strings from Python 3.14
are implemented on builds with ``MICROPY_PY_TSTRINGS`` enabled -- while several 3.11 features such
as ``except*`` are not.

.. table::
  :widths: 20 60 20

  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | **New syntax features**                                                                               | **Status**                   |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | `PEP 654 <https://peps.python.org/pep-0654/>`_       | Exception Groups and ``except*``               | Not implemented              |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | **New built-in features**                                                                                                            |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | `PEP 678 <https://peps.python.org/pep-0678/>`_       | Enriching Exceptions with Notes                | Not implemented              |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | **New standard library modules**                                                                                                     |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | `PEP 680 <https://peps.python.org/pep-0680/>`_       | ``tomllib``: Support for Parsing TOML in the   | Not implemented              |
  |                                                      | Standard Library                               |                              |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | **Interpreter improvements**                                                                                                         |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | `PEP 657 <https://peps.python.org/pep-0657/>`_       | Fine-grained Error Locations in Tracebacks     | Not implemented [#pep657]_   |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | `gh-57684                                            | New ``-P`` command line option and             | Not implemented [#safepath]_ |
  | <https://github.com/python/cpython/issues/57684>`_   | ``PYTHONSAFEPATH`` environment variable to     |                              |
  |                                                      | disable automatically prepending potentially   |                              |
  |                                                      | unsafe paths to ``sys.path``                   |                              |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | **New typing features**                                                                                                              |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | `PEP 646 <https://peps.python.org/pep-0646/>`_       | Variadic Generics                              | Not implemented [#ann]_      |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | `PEP 655 <https://peps.python.org/pep-0655/>`_       | Marking individual TypedDict items as required | Not implemented [#ann]_      |
  |                                                      | or potentially-missing                         |                              |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | `PEP 673 <https://peps.python.org/pep-0673/>`_       | Self type                                      | Not implemented [#ann]_      |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | `PEP 675 <https://peps.python.org/pep-0675/>`_       | Arbitrary Literal String Type                  | Not implemented [#ann]_      |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | `PEP 681 <https://peps.python.org/pep-0681/>`_       | Data Class Transforms                          | Not implemented [#ann]_      |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | **Important deprecations, removals or restrictions**                                                                                 |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | `PEP 594 <https://peps.python.org/pep-0594/>`_       | Many legacy standard library modules have been | Not implemented [#pep594]_   |
  |                                                      | deprecated and will be removed in Python 3.13  |                              |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | `PEP 624 <https://peps.python.org/pep-0624/>`_       | Py_UNICODE encoder APIs have been removed      | Not relevant                 |
  +------------------------------------------------------+------------------------------------------------+------------------------------+
  | `PEP 670 <https://peps.python.org/pep-0670/>`_       | Macros converted to static inline functions    | Not relevant                 |
  +------------------------------------------------------+------------------------------------------------+------------------------------+


Other Language Changes:

.. table::
  :widths: 90 10

  +---------------------------------------------------------------------------------------------------+----------------------------------+
  | Starred unpacking expressions can now be used in :keyword:`for` statements.                       | Not implemented [#starunpack]_   |
  +---------------------------------------------------------------------------------------------------+----------------------------------+
  | Asynchronous :ref:`comprehensions <comprehensions>` are now allowed inside comprehensions in      | Not implemented [#asyncgen]_     |
  | :ref:`asynchronous functions <async def>`. Outer comprehensions implicitly become asynchronous in |                                  |
  | this case.                                                                                        |                                  |
  +---------------------------------------------------------------------------------------------------+----------------------------------+
  | A :exc:`TypeError` is now raised instead of an :exc:`AttributeError` in :keyword:`with`           | Not implemented [#withtype]_     |
  | statements and :meth:`contextlib.ExitStack.enter_context` for objects that do not support the     |                                  |
  | context manager protocol, and in :keyword:`async with` statements and                             |                                  |
  | :meth:`contextlib.AsyncExitStack.enter_async_context` for objects not supporting the asynchronous |                                  |
  | context manager protocol.                                                                         |                                  |
  +---------------------------------------------------------------------------------------------------+----------------------------------+
  | Added :meth:`object.__getstate__`, which provides the default implementation of the               | Not implemented [#getstate]_     |
  | ``__getstate__()`` method.  Copying and pickling instances of subclasses of builtin types now     |                                  |
  | also copies and pickles instance attributes implemented as slots.                                 |                                  |
  +---------------------------------------------------------------------------------------------------+----------------------------------+
  | A ``"z"`` option was added to the format specification mini-language that coerces negative to     | Not implemented                  |
  | positive zero after rounding to the format precision.  See :pep:`682` for more details.           |                                  |
  +---------------------------------------------------------------------------------------------------+----------------------------------+
  | Bytes are no longer accepted on :data:`sys.path`.                                                 | Not implemented [#syspathbytes]_ |
  +---------------------------------------------------------------------------------------------------+----------------------------------+
  | :meth:`int.to_bytes` and :meth:`int.from_bytes` gained default argument values, so ``length``     | Complete [#notwhatsnew]_         |
  | defaults to ``1`` and ``byteorder`` defaults to ``"big"``.                                        |                                  |
  +---------------------------------------------------------------------------------------------------+----------------------------------+

Changes to built-in modules:

.. table::
  :widths: 90 10

  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `asyncio <https://docs.python.org/3/whatsnew/3.11.html#asyncio>`_                                                                    |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Added the :class:`~asyncio.TaskGroup` class, an asynchronous context manager holding a group of  | Not implemented                   |
  | tasks that will wait for all of them upon exit.                                                  |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Added :func:`~asyncio.timeout`, an asynchronous context manager for setting a timeout on         | Not implemented                   |
  | asynchronous operations.                                                                         |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Added the :class:`~asyncio.Runner` class, which exposes the machinery used by                    | Not implemented                   |
  | :func:`~asyncio.run`.                                                                            |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Added the :class:`~asyncio.Barrier` class to the synchronization primitives, and the related     | Not implemented                   |
  | :exc:`~asyncio.BrokenBarrierError` exception.                                                    |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Added keyword argument *all_errors* to :meth:`asyncio.loop.create_connection` so that multiple   | Not implemented                   |
  | connection errors can be raised as an :exc:`ExceptionGroup`.                                     |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Added the :meth:`asyncio.StreamWriter.start_tls` method for upgrading existing stream-based      | Not implemented                   |
  | connections to TLS.                                                                              |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Added raw datagram socket functions to the event loop: :meth:`~asyncio.loop.sock_sendto`,        | Not implemented                   |
  | :meth:`~asyncio.loop.sock_recvfrom` and :meth:`~asyncio.loop.sock_recvfrom_into`.                |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Added :meth:`~asyncio.Task.cancelling` and :meth:`~asyncio.Task.uncancel` methods to             | Not implemented                   |
  | :class:`~asyncio.Task`.                                                                          |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `binascii <https://docs.python.org/3/library/binascii.html>`_                                                                        |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | :func:`binascii.a2b_base64` gained a *strict_mode* keyword argument that rejects invalid padding | Not implemented [#notwhatsnew]_   |
  | and non-base64 characters.                                                                       |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `contextlib <https://docs.python.org/3/whatsnew/3.11.html#contextlib>`_                                                              |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Added non parallel-safe :func:`~contextlib.chdir` context manager to change the current working  | Not implemented [#mplib]_         |
  | directory and then restore it on exit.                                                           |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `datetime <https://docs.python.org/3/whatsnew/3.11.html#datetime>`_                                                                  |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Add :const:`datetime.UTC`, a convenience alias for :attr:`datetime.timezone.utc`.                | Not implemented [#mplib]_         |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | :meth:`datetime.date.fromisoformat`, :meth:`datetime.time.fromisoformat` and                     | Not implemented [#mplib]_         |
  | :meth:`datetime.datetime.fromisoformat` can now be used to parse most ISO 8601 formats.          |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `functools <https://docs.python.org/3/whatsnew/3.11.html#functools>`_                                                                |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | :func:`functools.singledispatch` now supports :class:`types.UnionType` and :data:`typing.Union`  | Not implemented [#mplib]_ [#ann]_ |
  | as annotations to the dispatch argument.                                                         |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `gzip <https://docs.python.org/3/whatsnew/3.11.html#gzip>`_                                                                          |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | The :func:`gzip.compress` function is now faster when used with the ``mtime=0`` argument as it   | Not relevant [#gzipmtime]_        |
  | delegates the compression entirely to a single :func:`zlib.compress` operation.                  |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `hashlib <https://docs.python.org/3/whatsnew/3.11.html#hashlib>`_                                                                    |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | :func:`hashlib.blake2b` and :func:`hashlib.blake2s` now prefer libb2 over Python's vendored      | Not relevant [#hashalgo]_         |
  | copy.                                                                                            |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | The internal ``_sha3`` module with SHA3 and SHAKE algorithms now uses *tiny_sha3* instead of the | Not relevant [#hashalgo]_         |
  | *Keccak Code Package* to reduce code and binary size.                                            |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Add :func:`hashlib.file_digest`, a helper function for efficient hashing of files or file-like   | Not implemented                   |
  | objects.                                                                                         |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `inspect <https://docs.python.org/3/whatsnew/3.11.html#inspect>`_                                                                    |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Add :func:`~inspect.getmembers_static` to return all members without triggering dynamic lookup   | Not implemented [#mplib]_         |
  | via the descriptor protocol.                                                                     |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Add :func:`~inspect.ismethodwrapper` for checking if the type of an object is a                  | Not implemented [#mplib]_         |
  | :class:`~types.MethodWrapperType`.                                                               |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Change the frame-related functions to return new :class:`~inspect.FrameInfo` and                 | Not implemented [#mplib]_         |
  | :class:`~inspect.Traceback` class instances that include the extended :pep:`657` position        |                                   |
  | information.                                                                                     |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `locale <https://docs.python.org/3/whatsnew/3.11.html#locale>`_                                                                      |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Add :func:`locale.getencoding` to get the current locale encoding.                               | Not implemented [#mplib]_         |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `logging <https://docs.python.org/3/whatsnew/3.11.html#logging>`_                                                                    |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Added :func:`~logging.getLevelNamesMapping` to return a mapping from logging level names to the  | Not implemented [#mplib]_         |
  | values of their corresponding logging levels.                                                    |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Added a :meth:`~logging.handlers.SysLogHandler.createSocket` method to                           | Not relevant [#mplib]_            |
  | :class:`~logging.handlers.SysLogHandler`.                                                        |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `math <https://docs.python.org/3/whatsnew/3.11.html#math>`_                                                                          |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Add :func:`math.exp2`: return 2 raised to the power of x.                                        | Not implemented                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Add :func:`math.cbrt`: return the cube root of x.                                                | Not implemented                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | The behaviour of two :func:`math.pow` corner cases was changed, for consistency with the IEEE    | Complete [#mathpow]_              |
  | 754 specification.  The operations ``math.pow(0.0, -math.inf)`` and ``math.pow(-0.0,             |                                   |
  | -math.inf)`` now return ``inf``.  Previously they raised :exc:`ValueError`.                      |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | The :data:`math.nan` value is now always available.                                              | Complete                          |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `operator <https://docs.python.org/3/whatsnew/3.11.html#operator>`_                                                                  |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | A new function ``operator.call`` has been added, such that ``operator.call(obj, *args, **kwargs) | Not implemented [#mplib]_         |
  | == obj(*args, **kwargs)``.                                                                       |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `os <https://docs.python.org/3/whatsnew/3.11.html#os>`_                                                                              |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | On Windows, :func:`os.urandom` now uses ``BCryptGenRandom()``, instead of ``CryptGenRandom()``   | Not relevant                      |
  | which is deprecated.                                                                             |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | As of 3.11.10, :func:`os.mkdir` and :func:`os.makedirs` on Windows now support passing a *mode*  | Not relevant                      |
  | value of ``0o700`` to apply access control to the new directory.                                 |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `pathlib <https://docs.python.org/3/whatsnew/3.11.html#pathlib>`_                                                                    |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | :meth:`~pathlib.Path.glob` and :meth:`~pathlib.Path.rglob` return only directories if *pattern*  | Not implemented [#glob]_          |
  | ends with a pathname components separator.                                                       |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `re <https://docs.python.org/3/whatsnew/3.11.html#re>`_                                                                              |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Atomic grouping (``(?>...)``) and possessive quantifiers (``*+``, ``++``, ``?+``, ``{m,n}+``)    | Not implemented [#re]_            |
  | are now supported in regular expressions.                                                        |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `shutil <https://docs.python.org/3/whatsnew/3.11.html#shutil>`_                                                                      |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Add optional parameter *dir_fd* in :func:`shutil.rmtree`.                                        | Not implemented [#mplib]_         |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `socket <https://docs.python.org/3/whatsnew/3.11.html#socket>`_                                                                      |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Add CAN Socket support for NetBSD.                                                               | Not relevant                      |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | :func:`~socket.create_connection` has an option to raise, in case of failure to connect, an      | Not implemented                   |
  | :exc:`ExceptionGroup` containing all errors instead of only raising the last error.              |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `string <https://docs.python.org/3/whatsnew/3.11.html#string>`_                                                                      |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Add :meth:`~string.Template.get_identifiers` and :meth:`~string.Template.is_valid` to            | Not implemented [#template]_      |
  | :class:`string.Template`.                                                                        |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `sys <https://docs.python.org/3/whatsnew/3.11.html#sys>`_                                                                            |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | :func:`sys.exc_info` now derives the ``type`` and ``traceback`` fields from the ``value`` (the   | Complete [#excinfo]_              |
  | exception instance), so when an exception is modified while it is being handled, the changes are |                                   |
  | reflected in the results of subsequent calls to ``exc_info()``.                                  |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Add :func:`sys.exception` which returns the active exception instance (equivalent to             | Not implemented                   |
  | ``sys.exc_info()[1]``).                                                                          |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Add the ``sys.flags.safe_path`` flag.                                                            | Not implemented [#safepath]_      |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `threading <https://docs.python.org/3/whatsnew/3.11.html#threading>`_                                                                |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | On Unix, if the ``sem_clockwait()`` function is available in the C library, the                  | Not relevant [#locktimeout]_      |
  | :meth:`threading.Lock.acquire` method now uses the monotonic clock for the timeout, rather than  |                                   |
  | using the system clock, to not be affected by system clock changes.                              |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `time <https://docs.python.org/3/whatsnew/3.11.html#time>`_                                                                          |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | On Unix, :func:`time.sleep` now uses the ``clock_nanosleep()`` or ``nanosleep()`` function, if   | Not implemented [#sleep]_         |
  | available, which has a resolution of 1 nanosecond, rather than using ``select()`` which has a    |                                   |
  | resolution of 1 microsecond.                                                                     |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | On Windows 8.1 and newer, :func:`time.sleep` now uses a waitable timer based on high-resolution  | Not implemented [#sleep]_         |
  | timers which has a resolution of 100 nanoseconds.  Previously, it had a resolution of 1          |                                   |
  | millisecond.                                                                                     |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `traceback <https://docs.python.org/3/whatsnew/3.11.html#traceback>`_                                                                |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Add :func:`traceback.StackSummary.format_frame_summary` to allow users to override which frames  | Not implemented [#tbmod]_         |
  | appear in the traceback, and how they are formatted.                                             |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Add :func:`traceback.TracebackException.print`, which prints the formatted                       | Not implemented [#tbmod]_         |
  | :exc:`~traceback.TracebackException` instance to a file.                                         |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `unicodedata <https://docs.python.org/3/whatsnew/3.11.html#unicodedata>`_                                                            |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | The Unicode database has been updated to version 14.0.0.                                         | Not relevant [#unicodedata]_      |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `unittest <https://docs.python.org/3/whatsnew/3.11.html#unittest>`_                                                                  |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | Added methods :meth:`~unittest.TestCase.enterContext` and                                        | Not implemented [#mplib]_         |
  | :meth:`~unittest.TestCase.enterClassContext`, method                                             |                                   |
  | :meth:`~unittest.IsolatedAsyncioTestCase.enterAsyncContext` and function                         |                                   |
  | :func:`unittest.enterModuleContext`.                                                             |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | `warnings <https://docs.python.org/3/whatsnew/3.11.html#warnings>`_                                                                  |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+
  | :func:`warnings.catch_warnings` now accepts arguments for :func:`warnings.simplefilter`,         | Not implemented [#warnmod]_       |
  | providing a more concise way to locally ignore warnings or convert them to errors.               |                                   |
  +--------------------------------------------------------------------------------------------------+-----------------------------------+

Deprecations.  MicroPython has no warning categories and no warning
machinery [#warnmod]_, so none of these produce a diagnostic:

.. table::
  :widths: 90 10

  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | Chaining :class:`classmethod` descriptors is now deprecated.  It can no longer be used to wrap other    | Not relevant [#cmchain]_   |
  | descriptors such as :class:`property`.                                                                  |                            |
  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | Octal escapes in string and bytes literals with values larger than ``0o377`` (255 in decimal) now       | Not implemented [#octal]_  |
  | produce a :exc:`DeprecationWarning`.                                                                    |                            |
  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | The delegation of :func:`int` to :meth:`~object.__trunc__` is now deprecated.                           | Not relevant [#trunc]_     |
  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | :pep:`594` led to the deprecation of ``aifc``, ``audioop``, ``cgi``, ``cgitb``, ``chunk``, ``crypt``,   | Not implemented [#pep594]_ |
  | ``imghdr``, ``mailcap``, ``msilib``, ``nis``, ``nntplib``, ``ossaudiodev``, ``pipes``, ``sndhdr``,      |                            |
  | ``spwd``, ``sunau``, ``telnetlib``, ``uu`` and ``xdrlib``.                                              |                            |
  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | The ``asynchat``, ``asyncore`` and ``smtpd`` modules have been deprecated since at least Python 3.6,    | Not relevant               |
  | and will be removed in Python 3.12.                                                                     |                            |
  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | The ``lib2to3`` package and ``2to3`` tool are now deprecated.                                           | Not relevant               |
  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | Undocumented modules ``sre_compile``, ``sre_constants`` and ``sre_parse`` are now deprecated.           | Not relevant               |
  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | Several long-deprecated :mod:`configparser` names (``SafeConfigParser``, ``ParsingError.filename``,     | Not relevant               |
  | ``RawConfigParser.readfp()`` and ``LegacyInterpolation``) are now scheduled for removal.                |                            |
  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | The older set of :mod:`importlib.resources` functions were deprecated in favour of the replacements     | Not relevant               |
  | added in Python 3.9.                                                                                    |                            |
  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | The :func:`locale.getdefaultlocale` and ``locale.resetlocale()`` functions are deprecated.              | Not relevant               |
  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | Stricter rules will now be applied for numerical group references and group names in regular            | Not relevant [#re]_        |
  | expressions.                                                                                            |                            |
  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | In the :mod:`re` module, the ``re.template()`` function and the corresponding ``re.TEMPLATE`` and       | Not relevant               |
  | ``re.T`` flags are deprecated.                                                                          |                            |
  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | ``turtle.settiltangle()`` has been deprecated since Python 3.1; it now emits a deprecation warning.     | Not relevant               |
  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | :class:`typing.Text` is now deprecated, as is the keyword argument syntax for constructing              | Not relevant [#ann]_       |
  | :data:`typing.TypedDict` types.                                                                         |                            |
  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | ``webbrowser.MacOSX`` is deprecated.                                                                    | Not relevant               |
  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | The behavior of returning a value from a :class:`~unittest.TestCase` and                                | Not implemented [#mplib]_  |
  | :class:`~unittest.IsolatedAsyncioTestCase` test method (other than the default ``None`` value) is now   |                            |
  | deprecated.                                                                                             |                            |
  +---------------------------------------------------------------------------------------------------------+----------------------------+
  | Deprecated the not-formally-documented :mod:`unittest` functions ``findTestCases()``, ``makeSuite()``,  | Not relevant               |
  | ``getTestCaseNames()`` and ``TestProgram.usageExit()``.                                                 |                            |
  +---------------------------------------------------------------------------------------------------------+----------------------------+

Removals:

.. table::
  :widths: 90 10

  +-----------------------------------------------------------------------------------------------------+--------------------------------+
  | Removed the ``@asyncio.coroutine`` decorator enabling legacy generator-based coroutines to be       | Not relevant                   |
  | compatible with :keyword:`async` / :keyword:`await` code, and ``asyncio.coroutines.CoroWrapper``.   |                                |
  +-----------------------------------------------------------------------------------------------------+--------------------------------+
  | The *reuse_address* parameter of :meth:`asyncio.loop.create_datagram_endpoint` is now entirely      | Not relevant                   |
  | removed.                                                                                            |                                |
  +-----------------------------------------------------------------------------------------------------+--------------------------------+
  | Removed the ``binhex`` module and the related ``binascii`` functions ``a2b_hqx()``, ``b2a_hqx()``,  | Not relevant [#binhex]_        |
  | ``rlecode_hqx()`` and ``rldecode_hqx()``.  The ``binascii.crc_hqx()`` function remains available.   |                                |
  +-----------------------------------------------------------------------------------------------------+--------------------------------+
  | Removed the :mod:`distutils` ``bdist_msi`` command.                                                 | Not relevant                   |
  +-----------------------------------------------------------------------------------------------------+--------------------------------+
  | Removed the :meth:`~object.__getitem__` methods of ``xml.dom.pulldom.DOMEventStream``,              | Not relevant                   |
  | ``wsgiref.util.FileWrapper`` and ``fileinput.FileInput``.                                           |                                |
  +-----------------------------------------------------------------------------------------------------+--------------------------------+
  | Removed the deprecated :mod:`gettext` functions ``lgettext()``, ``ldgettext()``, ``lngettext()``,   | Not relevant                   |
  | ``ldngettext()`` and ``bind_textdomain_codeset()``, and related methods and parameters.             |                                |
  +-----------------------------------------------------------------------------------------------------+--------------------------------+
  | Removed ``inspect.getargspec()``, ``inspect.formatargspec()`` and the undocumented                  | Not implemented [#getargspec]_ |
  | ``Signature.from_builtin()`` and ``Signature.from_function()`` methods.                             |                                |
  +-----------------------------------------------------------------------------------------------------+--------------------------------+
  | Removed the :meth:`~object.__class_getitem__` method from ``pathlib.PurePath``.                     | Not relevant                   |
  +-----------------------------------------------------------------------------------------------------+--------------------------------+
  | Removed the ``MailmanProxy`` class in the ``smtpd`` module.                                         | Not relevant                   |
  +-----------------------------------------------------------------------------------------------------+--------------------------------+
  | Removed the deprecated ``split()`` method of ``_tkinter.TkappType``.                                | Not relevant                   |
  +-----------------------------------------------------------------------------------------------------+--------------------------------+
  | Removed namespace package support from :mod:`unittest` discovery.                                   | Not relevant                   |
  +-----------------------------------------------------------------------------------------------------+--------------------------------+
  | Removed the undocumented private ``float.__set_format__()`` method.                                 | Not relevant                   |
  +-----------------------------------------------------------------------------------------------------+--------------------------------+
  | The ``--experimental-isolated-subinterpreters`` configure flag has been removed.                    | Not relevant                   |
  +-----------------------------------------------------------------------------------------------------+--------------------------------+
  | Pynche, the Pythonically Natural Color and Hue Editor, has been moved out of ``Tools/scripts``.     | Not relevant                   |
  +-----------------------------------------------------------------------------------------------------+--------------------------------+

.. rubric:: Notes

.. [#mplib] This module is not built into MicroPython.  It is provided as a pure-Python package by
   `micropython-lib <https://github.com/micropython/micropython-lib>`_, which can be installed with
   ``mip`` or frozen into a firmware build.  These packages implement a useful subset of the CPython
   module rather than the full API.

.. [#ann] MicroPython parses type annotations but never evaluates them, so annotations produce no
   runtime effects and no ``__annotations__`` attributes are created.  There is no ``typing`` module,
   and micropython-lib's ``__future__`` package only defines the feature-flag constants -- the
   compiler ignores them.  These features can therefore only be used as inert annotations.

.. [#pep657] MicroPython tracebacks list only the file name, line number and function name for each
   frame.  Neither the source line nor the fine-grained column information added by :pep:`657` is
   available, because the source text is not retained after compilation.

.. [#safepath] MicroPython has no ``-P`` option, no ``PYTHONSAFEPATH`` handling and no ``sys.flags``
   object at all.  The unix port does prepend the script's directory to ``sys.path``, so it is
   subject to the same module shadowing that ``-P`` was added to avoid.

.. [#pep594] Of the modules deprecated by :pep:`594`, only ``uu`` exists for MicroPython, as a
   micropython-lib package.  It is not deprecated there.  None of the others were ever implemented.

.. [#starunpack] MicroPython does not support starred unpacking in displays at all (the :pep:`448`
   feature from Python 3.5), so neither ``for x in *a, *b:`` nor ``(*a, *b)`` can be compiled; both
   raise ``SyntaxError: *x must be assignment target``.

.. [#asyncgen] MicroPython does not support asynchronous generators at all.  An ``async def``
   function containing ``yield`` produces an ordinary generator with no ``__aiter__`` method, so it
   cannot be used with ``async for``, and asynchronous comprehensions cannot be written in the first
   place.

.. [#withtype] MicroPython still raises ``AttributeError`` for objects that do not implement the
   context manager protocol, both for ``with``/``async with`` statements and for micropython-lib's
   ``contextlib.ExitStack.enter_context()``.

.. [#getstate] MicroPython has no ``object.__getstate__()``, and neither ``copy`` nor ``pickle``
   support ``__slots__``.  The ``copy`` and ``pickle`` modules are micropython-lib packages.

.. [#syspathbytes] MicroPython still accepts ``bytes`` entries on ``sys.path`` and imports from them
   successfully; no error or warning is produced.

.. [#notwhatsnew] This change is not listed in *What's New in Python 3.11* but is recorded as a
   3.11 change in the CPython library reference.

.. [#gzipmtime] micropython-lib's ``gzip.compress()`` has no *mtime* parameter at all; it always
   delegates to the built-in ``deflate`` module, which writes a fixed gzip header.

.. [#hashalgo] MicroPython's ``hashlib`` provides only MD5, SHA-1 and SHA-256 (with SHA-224, SHA-384
   and SHA-512 available from micropython-lib).  There is no BLAKE2 or SHA-3 implementation, and no
   OpenSSL dependency.

.. [#glob] micropython-lib's ``pathlib.Path.glob()`` and ``rglob()`` support only a single ``*``
   wildcard and always yield entries of any kind, so a trailing separator has no special meaning.

.. [#re] MicroPython's ``re`` module is based on `re1.5 <https://github.com/pfalcon/re1.5>`_, a
   compact backtracking engine that is unrelated to CPython's.  It has no atomic groups, possessive
   quantifiers, named groups or group references; unsupported syntax raises ``ValueError: regex too
   complex``.

.. [#template] micropython-lib's ``string`` package provides only the character-class constants and
   a ``translate()`` helper; there is no ``Template`` class.

.. [#excinfo] MicroPython stores only the exception instance, so ``sys.exc_info()`` has always
   derived the type from the value, matching the Python 3.11 behaviour.  MicroPython exceptions have
   no ``__traceback__`` attribute, however, and the third element of the tuple is always ``None``.

.. [#mathpow] MicroPython's ``math.pow()`` delegates to the C library ``pow()`` and has never had
   CPython's pre-3.11 special case, so these corner cases have always returned ``inf``.

.. [#locktimeout] MicroPython's ``_thread`` lock has no timeout support: ``acquire()`` accepts a
   timeout argument for compatibility but silently ignores it, so a blocking acquire never times
   out.  micropython-lib's ``threading`` module provides only ``Thread``, not ``Lock``.

.. [#sleep] MicroPython's ``time.sleep()`` resolution is port-specific.  The unix port still uses
   ``select()`` and the windows port uses ``Sleep()``, which has millisecond resolution.  All ports
   additionally provide ``time.sleep_ms()`` and ``time.sleep_us()``.

.. [#tbmod] micropython-lib's ``traceback`` module provides only the module-level ``format_*()`` and
   ``print_*()`` functions; there are no ``StackSummary``, ``FrameSummary`` or ``TracebackException``
   classes.

.. [#unicodedata] MicroPython has no ``unicodedata`` module and no Unicode character database.  Only
   the ASCII range is handled for case conversion and character classification.

.. [#warnmod] micropython-lib's ``warnings`` module provides only a ``warn()`` function that prints
   to ``stderr``.  There is no filter machinery, no ``catch_warnings()`` and no warning category
   classes -- ``Warning`` and ``DeprecationWarning`` are not built in -- so none of the deprecations
   listed below produce a warning in MicroPython.

.. [#cmchain] MicroPython never supported chaining ``classmethod`` with another descriptor.
   ``@classmethod`` applied to a ``property`` yields a bound method wrapping the property object
   rather than the property's value, so it matches neither the pre-3.11 nor the deprecated
   behaviour.

.. [#octal] MicroPython has no warning machinery, so no diagnostic is produced.  In string literals
   MicroPython produces the same character as CPython (``"\777"`` is ``chr(0o777)``); in bytes
   literals it is stricter than CPython and rejects escapes above ``\377`` with a ``SyntaxError``
   rather than truncating them.

.. [#trunc] MicroPython has never delegated ``int()`` to ``__trunc__()``.  Calling ``int(a)`` when
   ``type(a)`` implements only ``__trunc__()`` raises ``TypeError``.

.. [#binhex] MicroPython's ``binascii`` implements only ``a2b_base64()``, ``b2a_base64()``,
   ``crc32()``, ``hexlify()`` and ``unhexlify()``.  The ``hqx`` functions and ``crc_hqx()`` were
   never implemented, and there is no ``binhex`` module.

.. [#getargspec] micropython-lib's ``inspect`` still defines ``getargspec()``, although it is a stub
   that raises ``NotImplementedError`` when called.  ``formatargspec()`` and the ``Signature``
   class methods were never implemented.
