.. _python_36:

Python 3.6
==========

Python 3.6 beta 1 was released on 12 Sep 2016, and a summary of the new features can be found here:

.. table::
  :widths: 30 50 20

  +-----------------------------------------------------------------------------------------------------------+-----------------+
  | **New Syntax Features**                                                                                   | **Status**      |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | `PEP 498 <https://www.python.org/dev/peps/pep-0498/>`_ | Literal String Formatting                        | Partial         |
  |                                                        |                                                  | [#fstring]_     |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | `PEP 515 <https://www.python.org/dev/peps/pep-0515/>`_ | Underscores in Numeric Literals                  | Complete        |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | `PEP 525 <https://www.python.org/dev/peps/pep-0525/>`_ | Asynchronous Generators                          | Not implemented |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | `PEP 526 <https://www.python.org/dev/peps/pep-0526/>`_ | Syntax for Variable Annotations (provisional)    | Partial         |
  |                                                        |                                                  | [#annotations]_ |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | `PEP 530 <https://www.python.org/dev/peps/pep-0530/>`_ | Asynchronous Comprehensions                      | Not implemented |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | **New Built-in Features**                                                                                                   |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | `PEP 468 <https://www.python.org/dev/peps/pep-0468/>`_ | Preserving the order of *kwargs* in a function   | Not implemented |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | `PEP 487 <https://www.python.org/dev/peps/pep-0487/>`_ | Simpler customization of class creation          | Partial         |
  |                                                        |                                                  | [#setname]_     |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | `PEP 520 <https://www.python.org/dev/peps/pep-0520/>`_ | Preserving Class Attribute Definition Order      | Not implemented |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | **Standard Library Changes**                                                                                                |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | `PEP 495 <https://www.python.org/dev/peps/pep-0495/>`_ | Local Time Disambiguation                        | Not implemented |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | `PEP 506 <https://www.python.org/dev/peps/pep-0506/>`_ | Adding A Secrets Module To The Standard Library  | Not implemented |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | `PEP 519 <https://www.python.org/dev/peps/pep-0519/>`_ | Adding a file system path protocol               | Not implemented |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | **CPython Internals**                                                                                                       |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | `PEP 509 <https://www.python.org/dev/peps/pep-0509/>`_ | Add a private version to dict                    | Won't do        |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | `PEP 523 <https://www.python.org/dev/peps/pep-0523/>`_ | Adding a frame evaluation API to CPython         | Not relevant    |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | **Linux/Window Changes**                                                                                                    |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | `PEP 524 <https://www.python.org/dev/peps/pep-0524/>`_ | Make ``os.urandom()`` blocking on Linux          | Complete        |
  |                                                        | (during system startup)                          |                 |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | `PEP 528 <https://www.python.org/dev/peps/pep-0528/>`_ | Change Windows console encoding to UTF-8         | Not relevant    |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+
  | `PEP 529 <https://www.python.org/dev/peps/pep-0529/>`_ | Change Windows filesystem encoding to UTF-8      | Not relevant    |
  +--------------------------------------------------------+--------------------------------------------------+-----------------+

Other Language Changes:

.. table::
  :widths: 90 10

  +-------------------------------------------------------------------------------------------------------------+-----------------+
  | A *global* or *nonlocal* statement must now textually appear before the first use of the affected name in   | Complete        |
  | the same scope. Previously this was a SyntaxWarning.                                                        |                 |
  +-------------------------------------------------------------------------------------------------------------+-----------------+
  | It is now possible to set a special method to None to indicate that the corresponding operation is not      | Partial         |
  | available. For example, if a class sets *__iter__()* to *None* , the class is not iterable.                 | [#specialnone]_ |
  +-------------------------------------------------------------------------------------------------------------+-----------------+
  | Long sequences of repeated traceback lines are now abbreviated as *[Previous line repeated {count} more     | Not implemented |
  | times]*                                                                                                     |                 |
  +-------------------------------------------------------------------------------------------------------------+-----------------+
  | Import now raises the new exception *ModuleNotFoundError* when it cannot find a module. Code that currently | Not implemented |
  | checks for ImportError (in try-except) will still work.                                                     |                 |
  +-------------------------------------------------------------------------------------------------------------+-----------------+
  | Class methods relying on zero-argument *super()* will now work correctly when called from metaclass methods | Not implemented |
  | during class creation.                                                                                      |                 |
  +-------------------------------------------------------------------------------------------------------------+-----------------+

Changes to built-in modules:

.. table::
  :widths: 90 10

  +--------------------------------------------------------------------------------------------------------------+------------------+
  | `array <https://docs.python.org/3.6/whatsnew/3.6.html#array>`_                                                                  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | Exhausted iterators of *array.array* will now stay exhausted even if the iterated array is extended.         | Not implemented  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | `binascii <https://docs.python.org/3.6/whatsnew/3.6.html#binascii>`_                                                            |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The b2a_base64() function now accepts an optional newline keyword argument to control whether the newline    | Complete         |
  | character is appended to the return value                                                                    |                  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | `cmath <https://docs.python.org/3.6/whatsnew/3.6.html#cmath>`_                                                                  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The new cmath.tau (τ) constant has been added                                                                | Not implemented  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | New constants: *cmath.inf* and *cmath.nan* to match *math.inf* and *math.nan* , and also *cmath.infj* and    | Not implemented  |
  | *cmath.nanj* to match the format used by complex repr                                                        |                  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | `collections <https://docs.python.org/3.6/whatsnew/3.6.html#collections>`_                                                      |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The new Collection abstract base class has been added to represent sized iterable container classes          | Not implemented  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The new *Reversible* abstract base class represents iterable classes that also provide the *__reversed__()*  | Not implemented  |
  | method.                                                                                                      |                  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The new *AsyncGenerator* abstract base class represents asynchronous generators.                             | Not implemented  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The *namedtuple()* function now accepts an optional keyword argument module, which, when specified, is used  | Not implemented  |
  | for the *__module__* attribute of the returned named tuple class.                                            |                  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The verbose and rename arguments for *namedtuple()* are now keyword-only.                                    | Not implemented  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | Recursive *collections.deque* instances can now be pickled.                                                  | Not implemented  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | `hashlib <https://docs.python.org/3.6/whatsnew/3.6.html#hashlib>`_                                                              |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | BLAKE2 hash functions were added to the module. *blake2b()* and *blake2s()* are always available and support | Not implemented  |
  | the full feature set of BLAKE2.                                                                              |                  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The SHA-3 hash functions *sha3_224()*, *sha3_256()*, *sha3_384()*, *sha3_512()*, and *SHAKE* hash functions  | Not implemented  |
  | *shake_128()* and *shake_256()* were added.                                                                  |                  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The password-based key derivation function *scrypt()* is now available with OpenSSL 1.1.0 and newer.         | Not implemented  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | `json <https://docs.python.org/3.6/whatsnew/3.6.html#json>`_                                                                    |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | *json.load()* and *json.loads()* now support binary input. Encoded JSON should be represented using either   | Partial          |
  | UTF-8, UTF-16, or UTF-32.                                                                                    | [#jsonbytes]_    |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | `math <https://docs.python.org/3.6/whatsnew/3.6.html#math>`_                                                                    |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The new math.tau (τ) constant has been added                                                                 | Complete         |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | `os <https://docs.python.org/3.6/whatsnew/3.6.html#os>`_                                                                        |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | A new *close()* method allows explicitly closing a *scandir()* iterator. The *scandir()* iterator now        | Not implemented  |
  | supports the context manager protocol.                                                                       |                  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | On Linux, *os.urandom()* now blocks until the system urandom entropy pool is initialized to increase the     | Complete         |
  | security.                                                                                                    |                  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The Linux *getrandom()* syscall (get random bytes) is now exposed as the new *os.getrandom()* function.      | Not implemented  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | `re <https://docs.python.org/3.6/whatsnew/3.6.html#re>`_                                                                        |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | Added support of modifier spans in regular expressions. Examples: *'(?i:p)ython'* matches 'python' and       | Not implemented  |
  | 'Python', but not 'PYTHON'; *'(?i)g(?-i:v)r'* matches *'GvR'* and *'gvr'*, but not *'GVR'*.                  |                  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | Match object groups can be accessed by *__getitem__*, which is equivalent to *group()*. So *mo['name']* is   | Not implemented  |
  | now equivalent to *mo.group('name')*.                                                                        |                  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | Match objects now support index-like objects as group indices.                                               | Not implemented  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | `socket <https://docs.python.org/3.6/whatsnew/3.6.html#socket>`_                                                                |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The *ioctl()* function now supports the *SIO_LOOPBACK_FAST_PATH* control code.                               | Not implemented  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The *getsockopt()* constants *SO_DOMAIN* , *SO_PROTOCOL*, *SO_PEERSEC* , and *SO_PASSSEC* are now supported. | Not implemented  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The *setsockopt()* now supports the *setsockopt(level, optname, None, optlen: int)* form.                    | Not implemented  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The socket module now supports the address family *AF_ALG* to interface with Linux Kernel crypto API.        | Not implemented  |
  | *ALG_*, *SOL_ALG* and *sendmsg_afalg()* were added.                                                          |                  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | New Linux constants *TCP_USER_TIMEOUT* and *TCP_CONGESTION* were added.                                      | Not implemented  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | `ssl <https://docs.python.org/3.6/whatsnew/3.6.html#ssl>`_                                                                      |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | ssl supports OpenSSL 1.1.0. The minimum recommend version is 1.0.2.                                          | Not relevant     |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | 3DES has been removed from the default cipher suites and ChaCha20 Poly1305 cipher suites have been added.    | Not relevant     |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | *SSLContext* has better default configuration for options and ciphers.                                       | Not relevant     |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | SSL session can be copied from one client-side connection to another with the new *SSLSession* class. TLS    | Not implemented  |
  | session resumption can speed up the initial handshake, reduce latency and improve performance.               |                  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The new *get_ciphers()* method can be used to get a list of enabled ciphers in order of cipher priority.     | Partial          |
  |                                                                                                              | [#getciphers]_   |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | All constants and flags have been converted to *IntEnum* and *IntFlags*.                                     | Not implemented  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | Server and client-side specific TLS protocols for *SSLContext* were added.                                   | Complete         |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | Added *SSLContext.post_handshake_auth* to enable and *ssl.SSLSocket.verify_client_post_handshake()* to       | Not implemented  |
  | initiate TLS 1.3 post-handshake authentication.                                                              |                  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | `struct <https://docs.python.org/3.6/whatsnew/3.6.html#struct>`_                                                                |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | now supports IEEE 754 half-precision floats via the 'e' format specifier.                                    | Complete         |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | `sys <https://docs.python.org/3.6/whatsnew/3.6.html#sys>`_                                                                      |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The new *getfilesystemencodeerrors()* function returns the name of the error mode used to convert between    | Not relevant     |
  | Unicode filenames and bytes filenames.                                                                       |                  |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | `zlib <https://docs.python.org/3.6/whatsnew/3.6.html#zlib>`_                                                                    |
  +--------------------------------------------------------------------------------------------------------------+------------------+
  | The *compress()* and *decompress()* functions now accept keyword arguments                                   | Not implemented  |
  +--------------------------------------------------------------------------------------------------------------+------------------+

.. rubric:: Notes

.. [#fstring] f-strings are parsed by a simplified pre-processor rather than a
   full parser.  Concatenation with an adjacent literal containing braces is not
   supported, expressions containing unbalanced braces or brackets are not
   supported, and the ``!a`` conversion is not implemented because
   :func:`ascii` is not available.
.. [#annotations] Annotations are parsed but discarded: no ``__annotations__``
   attribute is created for modules, classes or functions.  MicroPython also
   accepts annotations on expressions that CPython rejects.
.. [#setname] Currently, only :func:`__set_name__` is implemented.
.. [#specialnone] MicroPython does not treat ``None`` as "operation not
   available"; it simply attempts the call and raises ``TypeError: 'NoneType'
   object isn't callable``.  This gives the right outcome for ``__iter__`` and
   ``__hash__``, but differs where CPython falls back to another protocol, e.g.
   with ``__len__ = None`` CPython's ``bool(obj)`` returns ``True`` whereas
   MicroPython raises ``TypeError``.
.. [#jsonbytes] Binary input is accepted, but only UTF-8 encoded data;
   UTF-16 and UTF-32 are not supported.
.. [#getciphers] ``SSLContext.get_ciphers()`` returns a list of cipher-suite
   name strings rather than CPython's list of dictionaries describing each
   cipher.
