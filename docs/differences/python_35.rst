.. _python_35:

Python 3.5
==========

Below is a list of finalised/accepted PEPs for Python 3.5 grouped into their impact to MicroPython.

.. table::
  :widths: 30 50 20

  +--------------------------------------------------------------------------------------------------------------+--------------------+
  | **Extensions to the syntax**                                                                                 | **Status**         |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 448 <https://www.python.org/dev/peps/pep-0448/>`_ | Additional unpacking generalizations                | Partial [#f1]_     |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 465 <https://www.python.org/dev/peps/pep-0465/>`_ | A new matrix multiplication operator                | Complete           |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 492 <https://www.python.org/dev/peps/pep-0492/>`_ | Coroutines with ``async`` and ``await`` syntax      | Partial [#f2]_     |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | **Extensions and changes to runtime**                                                                                             |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 461 <https://www.python.org/dev/peps/pep-0461/>`_ | % formatting for binary strings                     | Partial [#f3]_     |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 475 <https://www.python.org/dev/peps/pep-0475/>`_ | Retrying system calls that fail with ``EINTR``      | Complete           |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 479 <https://www.python.org/dev/peps/pep-0479/>`_ | Change ``StopIteration`` handling inside generators | Complete           |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | **Standard library changes**                                                                                                      |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 471 <https://www.python.org/dev/peps/pep-0471/>`_ | ``os.scandir()``                                    | Not implemented    |
  |                                                        |                                                     | [#f4]_             |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 485 <https://www.python.org/dev/peps/pep-0485/>`_ | ``math.isclose()``, a function for testing          | Complete           |
  |                                                        | approximate equality                                |                    |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | **Miscellaneous changes**                                                                                                         |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 441 <https://www.python.org/dev/peps/pep-0441/>`_ | Improved Python zip application support             | Not implemented    |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 486 <https://www.python.org/dev/peps/pep-0486/>`_ | Make the Python Launcher aware of virtual           | Not relevant       |
  |                                                        | environments                                        |                    |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 484 <https://www.python.org/dev/peps/pep-0484/>`_ | Type hints (advisory only)                          | Complete [#fth]_   |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 488 <https://www.python.org/dev/peps/pep-0488/>`_ | Elimination of PYO files                            | Not relevant       |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 489 <https://www.python.org/dev/peps/pep-0489/>`_ | Redesigning extension module loading                | Not relevant       |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+

Other Language Changes:

.. table::
  :widths: 90 10

  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Added the *namereplace* error handlers. The *backslashreplace* error handlers now work with decoding and  | Not           |
  | translating.                                                                                              | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Property docstrings are now writable. This is especially useful for collections.namedtuple() docstrings   | Not           |
  |                                                                                                           | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Circular imports involving relative imports are now supported.                                            | Complete      |
  +-----------------------------------------------------------------------------------------------------------+---------------+


New Modules:

* `typing <https://docs.python.org/3/whatsnew/3.5.html#typing>`_

* `zipzap <https://docs.python.org/3/whatsnew/3.5.html#zipapp>`_


Changes to built-in modules:

.. table::
  :widths: 90 10

  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `collections <https://docs.python.org/3/whatsnew/3.5.html#collections>`_                                                  |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *OrderedDict* class is now implemented in C, which makes it 4 to 100 times faster.                    | Complete      |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | *OrderedDict.items()* , *OrderedDict.keys()* , *OrderedDict.values()* views now support reversed()        | Not           |
  | iteration.                                                                                                | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The deque class now defines *index()*, *insert()*, and *copy()*, and supports the + and * operators.      | Not           |
  |                                                                                                           | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Docstrings produced by namedtuple() can now be updated.                                                   | Not           |
  |                                                                                                           | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The UserString class now implements the *__getnewargs__()*, *__rmod__()*, *casefold()*, *format_map()*,   | Not           |
  | *isprintable()*, and *maketrans()* methods to match the corresponding methods of str.                     | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `heapq <https://docs.python.org/3/whatsnew/3.5.html#heapq>`_                                                              |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Element comparison in *merge()* can now be customized by passing a key function in a new optional key     | Not           |
  | keyword argument, and a new optional *reverse* keyword argument can be used to reverse element comparison | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `io <https://docs.python.org/3/whatsnew/3.5.html#io>`_                                                                    |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | A new *BufferedIOBase.readinto1()* method, that uses at most one call to the underlying raw stream's      | Not           |
  | *RawIOBase.read()* or *RawIOBase.readinto()* methods                                                      | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `json <https://docs.python.org/3/whatsnew/3.5.html#json>`_                                                                |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | JSON decoder now raises JSONDecodeError instead of ValueError to provide better context information about | Not           |
  | the error.                                                                                                | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `math <https://docs.python.org/3/whatsnew/3.5.html#math>`_                                                                |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Two new constants have been added to the math module: *inf* and *nan*.                                    | Complete      |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | A new function *isclose()* provides a way to test for approximate equality.                               | Complete      |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | A new *gcd()* function has been added. The *fractions.gcd()* function is now deprecated.                  | Not           |
  |                                                                                                           | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `os <https://docs.python.org/3/whatsnew/3.5.html#os>`_                                                                    |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The new *scandir()* function returning an iterator of DirEntry objects has been added.                    | Not           |
  |                                                                                                           | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *urandom()* function now uses the *getrandom()* syscall on Linux 3.17 or newer, and *getentropy()* on | Complete      |
  | OpenBSD 5.6 and newer, removing the need to use /dev/urandom and avoiding failures due to potential file  |               |
  | descriptor exhaustion.                                                                                    |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | New *get_blocking()* and *set_blocking()* functions allow getting and setting a file descriptor's blocking| Not           |
  | mode (O_NONBLOCK.)                                                                                        | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | There is a new *os.path.commonpath()* function returning the longest common sub-path of each passed       | Not           |
  | pathname                                                                                                  | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `re <https://docs.python.org/3/whatsnew/3.5.html#re>`_                                                                    |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | References and conditional references to groups with fixed length are now allowed in lookbehind assertions| Not           |
  |                                                                                                           | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The number of capturing groups in regular expressions is no longer limited to 100.                        | Not           |
  |                                                                                                           | implemented   |
  |                                                                                                           | [#f5]_        |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *sub()* and *subn()* functions now replace unmatched groups with empty strings instead of raising an  | Partial [#f6]_|
  | exception.                                                                                                |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *re.error* exceptions have new attributes, msg, pattern, pos, lineno, and colno, that provide better  | Not           |
  | context information about the error                                                                       | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `socket <https://docs.python.org/3/whatsnew/3.5.html#socket>`_                                                            |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Functions with timeouts now use a monotonic clock, instead of a system clock.                             | Complete      |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | A new *socket.sendfile()* method allows sending a file over a socket by using the high-performance        | Not           |
  | *os.sendfile()* function on UNIX, resulting in uploads being from 2 to 3 times faster than when using     | implemented   |
  | plain *socket.send()*                                                                                     |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *socket.sendall()* method no longer resets the socket timeout every time bytes are received or sent.  | Partial [#f7]_|
  | The socket timeout is now the maximum total duration to send all data.                                    |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The backlog argument of the *socket.listen()* method is now optional. By default it is set to SOMAXCONN or| Partial [#f8]_|
  | to 128, whichever is less.                                                                                |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `ssl <https://docs.python.org/3/whatsnew/3.5.html#ssl>`_                                                                  |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Memory BIO Support                                                                                        | Not           |
  |                                                                                                           | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Application-Layer Protocol Negotiation Support                                                            | Not           |
  |                                                                                                           | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | There is a new *SSLSocket.version()* method to query the actual protocol version in use.                  | Not           |
  |                                                                                                           | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The SSLSocket class now implements a *SSLSocket.sendfile()* method.                                       | Not           |
  |                                                                                                           | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *SSLSocket.send()* method now raises either the *ssl.SSLWantReadError* or *ssl.SSLWantWriteError*     | Partial [#f9]_|
  | exception on a non-blocking socket if the operation would block. Previously, it would return 0.           |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *cert_time_to_seconds()* function now interprets the input time as UTC and not as local time, per RFC | Not           |
  | 5280. Additionally, the return value is always an int.                                                    | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | New *SSLObject.shared_ciphers()* and *SSLSocket.shared_ciphers()* methods return the list of ciphers sent | Not           |
  | by the client during the handshake.                                                                       | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *SSLSocket.do_handshake()*, *SSLSocket.read()*, *SSLSocket.shutdown()*, and *SSLSocket.write()*       | Not           |
  | methods of the SSLSocket class no longer reset the socket timeout every time bytes are received or sent.  | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *match_hostname()* function now supports matching of IP addresses.                                    | Not           |
  |                                                                                                           | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `sys <https://docs.python.org/3/whatsnew/3.5.html#sys>`_                                                                  |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | A new *set_coroutine_wrapper()* function allows setting a global hook that will be called whenever a      | Won't do      |
  | coroutine object is created by an async def function. A corresponding *get_coroutine_wrapper()* can be    | [#f10]_       |
  | used to obtain a currently set wrapper.                                                                   |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | A new *is_finalizing()* function can be used to check if the Python interpreter is shutting down.         | Not           |
  |                                                                                                           | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `time <https://docs.python.org/3/whatsnew/3.5.html#time>`_                                                                |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *monotonic()* function is now always available                                                        | Not           |
  |                                                                                                           | implemented   |
  +-----------------------------------------------------------------------------------------------------------+---------------+

.. rubric:: Notes

.. [#f1] Unpacking generalizations are supported in function calls (several
   ``*args`` and ``**kwargs`` may be given), but not in list, tuple, set or
   dict displays: ``[*a, *b]`` and ``{**a, **b}`` raise ``SyntaxError``.
.. [#f2] ``async def``, ``await``, ``async for`` and ``async with`` are all
   supported, but the ``__await__`` special method is not, so only coroutines
   and generators can be awaited.
.. [#f3] ``bytes`` supports the pre-existing conversions (``%s``, ``%d``,
   ``%x``, ``%c``, ...), but the new ``%b`` and ``%a`` conversions are missing
   and ``bytearray`` does not implement ``%`` at all.
.. [#f4] Neither ``os.scandir()`` nor ``os.DirEntry`` exist.  MicroPython
   instead provides ``os.ilistdir()``, which serves the same purpose of
   iterating a directory without extra ``stat()`` calls.
.. [#f5] The bundled ``re1.5`` engine rejects patterns with more than 63
   capturing groups with ``ValueError: regex too complex``.
.. [#f6] ``re.sub()`` does replace unmatched groups with empty strings, but
   ``re.subn()`` is not implemented.
.. [#f7] On the unix port ``sendall()`` issues a single ``send()``, so the
   timeout is never restarted.  On lwip-based ports ``sendall()`` writes in
   chunks and restarts the timeout for each chunk, so it is not a maximum
   total duration.
.. [#f8] The *backlog* argument is optional on all ports, but only the unix
   port defaults to ``min(SOMAXCONN, 128)``; other ports default to
   ``MICROPY_PY_SOCKET_LISTEN_BACKLOG_DEFAULT``, which is 2.
.. [#f9] A write that would block raises ``OSError(EAGAIN)`` rather than
   returning 0, but ``ssl.SSLWantReadError`` and ``ssl.SSLWantWriteError`` do
   not exist.
.. [#f10] These functions were deprecated in CPython 3.7 and removed in
   CPython 3.8.
.. [#fth] The MicroPython parser correct ignores all type hints. However, the ``typing`` module is not built-in.
