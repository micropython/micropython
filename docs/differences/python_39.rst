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
  | `PEP 585 <https://www.python.org/dev/peps/pep-0584/>`_ | Type hinting generics in standard collections      |                      |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+
  | `PEP 593 <https://www.python.org/dev/peps/pep-0593/>`_ | Flexible function and variable annotations         |                      |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+
  | `PEP 602 <https://www.python.org/dev/peps/pep-0602/>`_ | CPython adopts an annual release cycle. Instead of | Not relevant         |
  |                                                        | annual, aiming for two month release cycle         |                      |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+
  | `PEP 614 <https://www.python.org/dev/peps/pep-0614/>`_ | Relaxed grammar restrictions on decorators         |                      |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+
  | `PEP 615 <https://www.python.org/dev/peps/pep-0615/>`_ | The IANA Time Zone Database is now present in the  |                      |
  |                                                        | standard library in the zoneinfo module            |                      |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+
  | `PEP 616 <https://www.python.org/dev/peps/pep-0616/>`_ | String methods to remove prefixes and suffixes     |                      |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+
  | `PEP 617 <https://www.python.org/dev/peps/pep-0617/>`_ | CPython now uses a new parser based on PEG         | Not relevant         |
  +--------------------------------------------------------+----------------------------------------------------+----------------------+

Other Language Changes:

.. table::
  :widths: 90 10

  +-------------------------------------------------------------------------------------------------------------+---------------+
  | *__import__()* now raises *ImportError* instead of *ValueError*                                             | Complete      |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Python now gets the absolute path of the script filename specified on the command line (ex: *python3*       |               |
  | *script.py*): the *__file__* attribute of the *__main__* module became an absolute path, rather than a      |               |
  | relative path                                                                                               |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | By default, for best performance, the errors argument is only checked at the first encoding/decoding error  |               |
  | and the encoding argument is sometimes ignored for empty strings                                            |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | *"".replace("", s, n)* now returns *s* instead of an empty string for all non-zero n. It is now consistent  |               |
  | with *"".replace("", s)*                                                                                    |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Any valid expression can now be used as a decorator. Previously, the grammar was much more restrictive      |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Parallel running of *aclose()* / *asend()* / *athrow()* is now prohibited, and *ag_running* now reflects    |               |
  | the actual running status of the async generator                                                            |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Unexpected errors in calling the *__iter__* method are no longer masked by TypeError in the in operator and |               |
  | functions contains(), indexOf() and countOf() of the operator module                                        |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Unparenthesized lambda expressions can no longer be the expression part in an if clause in comprehensions   |               |
  | and generator expressions                                                                                   |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+

Changes to built-in modules:

.. table::
  :widths: 90 10

  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `asyncio <https://docs.python.org/3/whatsnew/3.9.html#asyncio>`_                                                              |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Due to significant security concerns, the reuse_address parameter of *asyncio.loop.create_datagram_endpoint()*|               |
  | is no longer supported                                                                                        |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Added a new coroutine *shutdown_default_executor()* that schedules a shutdown for the default executor that   |               |
  | waits on the *ThreadPoolExecutor* to finish closing. Also, *asyncio.run()* has been updated to use the new    |               |
  | coroutine.                                                                                                    |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Added *asyncio.PidfdChildWatcher*, a Linux-specific child watcher implementation that polls process file      |               |
  | descriptors                                                                                                   |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | added a new *coroutine asyncio.to_thread()*                                                                   |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | When cancelling the task due to a timeout, *asyncio.wait_for()* will now wait until the cancellation is       |               |
  | complete also in the case when timeout is <= 0, like it does with positive timeouts                           |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | *asyncio* now raises *TyperError* when calling incompatible methods with an *ssl.SSLSocket* socket            |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `gc <https://docs.python.org/3/whatsnew/3.9.html#gc>`_                                                                        |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Garbage collection does not block on resurrected objects                                                      |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Added a new function *gc.is_finalized()* to check if an object has been finalized by the garbage collector    |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `math <https://docs.python.org/3/whatsnew/3.9.html#math>`_                                                                    |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Expanded the *math.gcd()* function to handle multiple arguments. Formerly, it only supported two arguments    |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Added *math.lcm()*: return the least common multiple of specified arguments                                   |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Added *math.nextafter()*: return the next floating-point value after x towards y                              |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Added *math.ulp()*: return the value of the least significant bit of a float                                  |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `os <https://docs.python.org/3/whatsnew/3.9.html#os>`_                                                                        |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Exposed the Linux-specific *os.pidfd_open()* and *os.P_PIDFD*                                                 |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The *os.unsetenv()* function is now also available on Windows                                                 | Complete      |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The *os.putenv()* and *os.unsetenv()* functions are now always available                                      | Complete      |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  |  Added *os.waitstatus_to_exitcode()* function: convert a wait status to an exit code                          |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `random <https://docs.python.org/3/whatsnew/3.9.html#random>`_                                                                |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Added a new *random.Random.randbytes* method: generate random bytes                                           |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `sys <https://docs.python.org/3/whatsnew/3.9.html#sys>`_                                                                      |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Added a new *sys.platlibdir* attribute: name of the platform-specific library directory                       |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Previously, *sys.stderr* was block-buffered when non-interactive. Now stderr defaults to always being         |               |
  | line-buffered                                                                                                 |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+

.. rubric:: Notes

.. [#pep584] PEP 584 ``dict`` union operator is only available on MicroPython builds with ``MICROPY_CPYTHON_COMPAT`` enabled.
