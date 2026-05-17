.. _python_35:

Python 3.5
==========

Below is a list of finalised/accepted PEPs for Python 3.5 grouped into their impact to MicroPython.

.. table::
  :widths: 30 50 20

  +--------------------------------------------------------------------------------------------------------------+--------------------+
  | **Extensions to the syntax**                                                                                 | **Status**         |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 448 <https://www.python.org/dev/peps/pep-0448/>`_ | Additional unpacking generalizations                | Partial            |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 465 <https://www.python.org/dev/peps/pep-0465/>`_ | A new matrix multiplication operator                | Complete           |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 492 <https://www.python.org/dev/peps/pep-0492/>`_ | Coroutines with ``async`` and ``await`` syntax      | Complete           |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | **Extensions and changes to runtime**                                                                                             |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 461 <https://www.python.org/dev/peps/pep-0461/>`_ | % formatting for binary strings                     | Complete           |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 475 <https://www.python.org/dev/peps/pep-0475/>`_ | Retrying system calls that fail with ``EINTR``      | Complete           |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 479 <https://www.python.org/dev/peps/pep-0479/>`_ | Change ``StopIteration`` handling inside generators | Complete           |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | **Standard library changes**                                                                                                      |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 471 <https://www.python.org/dev/peps/pep-0471/>`_ | ``os.scandir()``                                    |                    |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 485 <https://www.python.org/dev/peps/pep-0485/>`_ | ``math.isclose()``, a function for testing          | Complete           |
  |                                                        | approximate equality                                |                    |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | **Miscellaneous changes**                                                                                                         |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 441 <https://www.python.org/dev/peps/pep-0441/>`_ | Improved Python zip application support             |                    |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 486 <https://www.python.org/dev/peps/pep-0486/>`_ | Make the Python Launcher aware of virtual           | Not relevant       |
  |                                                        | environments                                        |                    |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 484 <https://www.python.org/dev/peps/pep-0484/>`_ | Type hints (advisory only)                          | Complete [#fth]_   |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 488 <https://www.python.org/dev/peps/pep-0488/>`_ | Elimination of PYO files                            | Not relevant       |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+
  | `PEP 489 <https://www.python.org/dev/peps/pep-0489/>`_ | Redesigning extension module loading                |                    |
  +--------------------------------------------------------+-----------------------------------------------------+--------------------+

Other Language Changes:

.. table::
  :widths: 90 10

  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Added the *namereplace* error handlers. The *backslashreplace* error handlers now work with decoding and  |               |
  | translating.                                                                                              |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Property docstrings are now writable. This is especially useful for collections.namedtuple() docstrings   |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Circular imports involving relative imports are now supported.                                            |               |
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
  | The *OrderedDict* class is now implemented in C, which makes it 4 to 100 times faster.                    |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | *OrderedDict.items()* , *OrderedDict.keys()* , *OrderedDict.values()* views now support reversed()        |               |
  | iteration.                                                                                                |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The deque class now defines *index()*, *insert()*, and *copy()*, and supports the + and * operators.      |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Docstrings produced by namedtuple() can now be updated.                                                   |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The UserString class now implements the *__getnewargs__()*, *__rmod__()*, *casefold()*, *format_map()*,   |               |
  | *isprintable()*, and *maketrans()* methods to match the corresponding methods of str.                     |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `heapq <https://docs.python.org/3/whatsnew/3.5.html#heapq>`_                                                              |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Element comparison in *merge()* can now be customized by passing a key function in a new optional key     |               |
  | keyword argument, and a new optional *reverse* keyword argument can be used to reverse element comparison |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `io <https://docs.python.org/3/whatsnew/3.5.html#io>`_                                                                    |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | A new *BufferedIOBase.readinto1()* method, that uses at most one call to the underlying raw stream's      |               |
  | *RawIOBase.read()* or *RawIOBase.readinto()* methods                                                      |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `json <https://docs.python.org/3/whatsnew/3.5.html#json>`_                                                |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | JSON decoder now raises JSONDecodeError instead of ValueError to provide better context information about |               |
  | the error.                                                                                                |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `math <https://docs.python.org/3/whatsnew/3.5.html#math>`_                                                                |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Two new constants have been added to the math module: *inf* and *nan*.                                    | Complete      |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | A new function *isclose()* provides a way to test for approximate equality.                               |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | A new *gcd()* function has been added. The *fractions.gcd()* function is now deprecated.                  |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `os <https://docs.python.org/3/whatsnew/3.5.html#os>`_                                                                    |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The new *scandir()* function returning an iterator of DirEntry objects has been added.                    |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *urandom()* function now uses the *getrandom()* syscall on Linux 3.17 or newer, and *getentropy()* on |               |
  | OpenBSD 5.6 and newer, removing the need to use /dev/urandom and avoiding failures due to potential file  |               |
  | descriptor exhaustion.                                                                                    |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | New *get_blocking()* and *set_blocking()* functions allow getting and setting a file descriptor's blocking|               |
  | mode (O_NONBLOCK.)                                                                                        |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | There is a new *os.path.commonpath()* function returning the longest common sub-path of each passed       |               |
  | pathname                                                                                                  |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `re <https://docs.python.org/3/whatsnew/3.5.html#re>`_                                                    |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | References and conditional references to groups with fixed length are now allowed in lookbehind assertions|               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The number of capturing groups in regular expressions is no longer limited to 100.                        |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *sub()* and *subn()* functions now replace unmatched groups with empty strings instead of raising an  |               |
  | exception.                                                                                                |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *re.error* exceptions have new attributes, msg, pattern, pos, lineno, and colno, that provide better  |               |
  | context information about the error                                                                       |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `socket <https://docs.python.org/3/whatsnew/3.5.html#socket>`_                                                            |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Functions with timeouts now use a monotonic clock, instead of a system clock.                             |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | A new *socket.sendfile()* method allows sending a file over a socket by using the high-performance        |               |
  | *os.sendfile()* function on UNIX, resulting in uploads being from 2 to 3 times faster than when using     |               |
  | plain *socket.send()*                                                                                     |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *socket.sendall()* method no longer resets the socket timeout every time bytes are received or sent.  |               |
  | The socket timeout is now the maximum total duration to send all data.                                    |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The backlog argument of the *socket.listen()* method is now optional. By default it is set to SOMAXCONN or| Complete      |
  | to 128, whichever is less.                                                                                |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `ssl <https://docs.python.org/3/whatsnew/3.5.html#ssl>`_                                                                  |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Memory BIO Support                                                                                        |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | Application-Layer Protocol Negotiation Support                                                            |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | There is a new *SSLSocket.version()* method to query the actual protocol version in use.                  |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The SSLSocket class now implements a *SSLSocket.sendfile()* method.                                       |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *SSLSocket.send()* method now raises either the *ssl.SSLWantReadError* or *ssl.SSLWantWriteError*     |               |
  | exception on a non-blocking socket if the operation would block. Previously, it would return 0.           |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *cert_time_to_seconds()* function now interprets the input time as UTC and not as local time, per RFC |               |
  | 5280. Additionally, the return value is always an int.                                                    |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | New *SSLObject.shared_ciphers()* and *SSLSocket.shared_ciphers()* methods return the list of ciphers sent |               |
  | by the client during the handshake.                                                                       |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *SSLSocket.do_handshake()*, *SSLSocket.read()*, *SSLSocket.shutdown()*, and *SSLSocket.write()*       |               |
  | methods of the SSLSocket class no longer reset the socket timeout every time bytes are received or sent.  |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *match_hostname()* function now supports matching of IP addresses.                                    |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `sys <https://docs.python.org/3/whatsnew/3.5.html#sys>`_                                                                  |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | A new *set_coroutine_wrapper()* function allows setting a global hook that will be called whenever a      |               |
  | coroutine object is created by an async def function. A corresponding *get_coroutine_wrapper()* can be    |               |
  | used to obtain a currently set wrapper.                                                                   |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | A new *is_finalizing()* function can be used to check if the Python interpreter is shutting down.         |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | `time <https://docs.python.org/3/whatsnew/3.5.html#time>`_                                                                |
  +-----------------------------------------------------------------------------------------------------------+---------------+
  | The *monotonic()* function is now always available                                                        |               |
  +-----------------------------------------------------------------------------------------------------------+---------------+

.. rubric:: Notes

.. [#fth] The MicroPython parser correct ignores all type hints. However, the ``typing`` module is not built-in.
