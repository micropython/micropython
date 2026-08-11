.. _python_39:

Python 3.9
==========

Python 3.9.0 (final) was released on the 5th October 2020. The Features for 3.9 are
defined in `PEP 596 <https://www.python.org/dev/peps/pep-0596/#features-for-3-9>`_
and a detailed description of the changes can be found in
`What's New in Python 3.9 <https://docs.python.org/3/whatsnew/3.9.html>`_

.. table::
  :widths: 20 60 20

  +--------------------------------------------------------+----------------------------------------------------+----------------------+
  | **Features**                                           |                                                    | **Status**           |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+
  | `PEP 573 <https://www.python.org/dev/peps/pep-0573/>`_ | Fast access to module state from methods of C      | Not relevant         |
  |                                                        | extension types                                    |                      |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+
  | `PEP 584 <https://www.python.org/dev/peps/pep-0584/>`_ | Union operators added to dict                      | Complete [#pep584]_  |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+
  | `PEP 585 <https://www.python.org/dev/peps/pep-0584/>`_ | Type hinting generics in standard collections      | Not implemented      |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+
  | `PEP 593 <https://www.python.org/dev/peps/pep-0593/>`_ | Flexible function and variable annotations         | Not implemented      |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+
  | `PEP 602 <https://www.python.org/dev/peps/pep-0602/>`_ | CPython adopts an annual release cycle. Instead of | Not relevant         |
  |                                                        | annual, aiming for two month release cycle         |                      |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+
  | `PEP 614 <https://www.python.org/dev/peps/pep-0614/>`_ | Relaxed grammar restrictions on decorators         | Not implemented      |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+
  | `PEP 615 <https://www.python.org/dev/peps/pep-0615/>`_ | The IANA Time Zone Database is now present in the  | Not implemented      |
  |                                                        | standard library in the zoneinfo module            |                      |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+
  | `PEP 616 <https://www.python.org/dev/peps/pep-0616/>`_ | String methods to remove prefixes and suffixes     | Not implemented      |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+
  | `PEP 617 <https://www.python.org/dev/peps/pep-0617/>`_ | CPython now uses a new parser based on PEG         | Not relevant         |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+

Other Language Changes:

.. table::
  :widths: 90 10

  +-------------------------------------------------------------------------------------------------------------+----------------------+
  | *__import__()* now raises *ImportError* instead of *ValueError*                                             | Complete             |
  +-------------------------------------------------------------------------------------------------------------+----------------------+
  | Python now gets the absolute path of the script filename specified on the command line (ex: *python3*       | Not implemented      |
  | *script.py*): the *__file__* attribute of the *__main__* module became an absolute path, rather than a      |                      |
  | relative path                                                                                               |                      |
  +-------------------------------------------------------------------------------------------------------------+----------------------+
  | By default, for best performance, the errors argument is only checked at the first encoding/decoding error  | Partial [#encargs]_  |
  | and the encoding argument is sometimes ignored for empty strings                                            |                      |
  +-------------------------------------------------------------------------------------------------------------+----------------------+
  | *"".replace("", s, n)* now returns *s* instead of an empty string for all non-zero n. It is now consistent  | Complete             |
  | with *"".replace("", s)*                                                                                    |                      |
  +-------------------------------------------------------------------------------------------------------------+----------------------+
  | Any valid expression can now be used as a decorator. Previously, the grammar was much more restrictive      | Not implemented      |
  +-------------------------------------------------------------------------------------------------------------+----------------------+
  | Parallel running of *aclose()* / *asend()* / *athrow()* is now prohibited, and *ag_running* now reflects    | Not implemented      |
  | the actual running status of the async generator                                                            | [#asyncgen]_         |
  +-------------------------------------------------------------------------------------------------------------+----------------------+
  | Unexpected errors in calling the *__iter__* method are no longer masked by TypeError in the in operator and | Partial [#itermask]_ |
  | functions contains(), indexOf() and countOf() of the operator module                                        |                      |
  +-------------------------------------------------------------------------------------------------------------+----------------------+
  | Unparenthesized lambda expressions can no longer be the expression part in an if clause in comprehensions   | Not implemented      |
  | and generator expressions                                                                                   |                      |
  +-------------------------------------------------------------------------------------------------------------+----------------------+

Changes to built-in modules:

.. table::
  :widths: 90 10

  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | `asyncio <https://docs.python.org/3/whatsnew/3.9.html#asyncio>`_                                                                     |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | Due to significant security concerns, the reuse_address parameter of *asyncio.loop.create_datagram_endpoint()*| Not relevant         |
  | is no longer supported                                                                                        |                      |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | Added a new coroutine *shutdown_default_executor()* that schedules a shutdown for the default executor that   | Not relevant         |
  | waits on the *ThreadPoolExecutor* to finish closing. Also, *asyncio.run()* has been updated to use the new    |                      |
  | coroutine.                                                                                                    |                      |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | Added *asyncio.PidfdChildWatcher*, a Linux-specific child watcher implementation that polls process file      | Not relevant         |
  | descriptors                                                                                                   |                      |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | added a new *coroutine asyncio.to_thread()*                                                                   | Not implemented      |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | When cancelling the task due to a timeout, *asyncio.wait_for()* will now wait until the cancellation is       | Complete             |
  | complete also in the case when timeout is <= 0, like it does with positive timeouts                           |                      |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | *asyncio* now raises *TyperError* when calling incompatible methods with an *ssl.SSLSocket* socket            | Not relevant         |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | `gc <https://docs.python.org/3/whatsnew/3.9.html#gc>`_                                                                               |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | Garbage collection does not block on resurrected objects                                                      | Not relevant         |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | Added a new function *gc.is_finalized()* to check if an object has been finalized by the garbage collector    | Not implemented      |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | `math <https://docs.python.org/3/whatsnew/3.9.html#math>`_                                                                           |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | Expanded the *math.gcd()* function to handle multiple arguments. Formerly, it only supported two arguments    | Not implemented      |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | Added *math.lcm()*: return the least common multiple of specified arguments                                   | Not implemented      |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | Added *math.nextafter()*: return the next floating-point value after x towards y                              | Not implemented      |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | Added *math.ulp()*: return the value of the least significant bit of a float                                  | Not implemented      |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | `os <https://docs.python.org/3/whatsnew/3.9.html#os>`_                                                                               |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | Exposed the Linux-specific *os.pidfd_open()* and *os.P_PIDFD*                                                 | Not relevant         |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | The *os.unsetenv()* function is now also available on Windows                                                 | Complete             |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | The *os.putenv()* and *os.unsetenv()* functions are now always available                                      | Partial [#osenv]_    |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  |  Added *os.waitstatus_to_exitcode()* function: convert a wait status to an exit code                          | Not implemented      |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | `random <https://docs.python.org/3/whatsnew/3.9.html#random>`_                                                                       |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | Added a new *random.Random.randbytes* method: generate random bytes                                           | Not implemented      |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | `sys <https://docs.python.org/3/whatsnew/3.9.html#sys>`_                                                                             |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | Added a new *sys.platlibdir* attribute: name of the platform-specific library directory                       | Not relevant         |
  +---------------------------------------------------------------------------------------------------------------+----------------------+
  | Previously, *sys.stderr* was block-buffered when non-interactive. Now stderr defaults to always being         | Not relevant         |
  | line-buffered                                                                                                 | [#stderr]_           |
  +---------------------------------------------------------------------------------------------------------------+----------------------+

.. rubric:: Notes

.. [#pep584] PEP 584 ``dict`` union operator is only available on MicroPython builds with ``MICROPY_CPYTHON_COMPAT`` enabled.

.. [#encargs] MicroPython also only inspects the ``errors`` argument after a decoding error has been detected, but unlike
   CPython the ``encoding`` argument is always validated and raises ``LookupError`` for an unknown encoding, including for
   empty strings. Support for the ``errors`` argument requires ``MICROPY_PY_BUILTINS_BYTES_DECODE_ERRORS``, which is only
   enabled at the "extra features" ROM level and above.

.. [#asyncgen] MicroPython does not support asynchronous generators at all, not just the restriction added in Python 3.9. An
   ``async def`` function containing ``yield`` produces an ordinary generator with no ``__aiter__`` method, so it cannot be
   used with ``async for``, and there are no ``aclose()``, ``asend()``, ``athrow()`` or ``ag_running`` attributes.

.. [#itermask] The ``in`` operator propagates exceptions raised by ``__iter__`` rather than masking them as ``TypeError``.
   However MicroPython has no built-in ``operator`` module, and the ``operator`` module provided by micropython-lib does not
   implement ``contains()``, ``indexOf()`` or ``countOf()``.

.. [#osenv] ``os.putenv()`` and ``os.unsetenv()`` are only available on ports that enable
   ``MICROPY_PY_OS_GETENV_PUTENV_UNSETENV`` (currently the ``unix`` and ``windows`` ports). Bare-metal ports have no process
   environment, so these functions are not provided there.

.. [#stderr] MicroPython's ``sys.stderr`` is unbuffered and writes to the same underlying stream as ``sys.stdout``, so the
   distinction between block-buffered and line-buffered output does not apply.
