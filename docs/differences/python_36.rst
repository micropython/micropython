.. _python_36:

Python 3.6
==========

Python 3.6 beta 1 was released on 12 Sep 2016, and a summary of the new features can be found here:

  +-----------------------------------------------------------------------------------------------------------+--------------+
  | **New Syntax Features:**                                                                                  | **Status**   |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | `PEP 498 <https://www.python.org/dev/peps/pep-0498/>`_ | Literal String Formatting                        |              |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | `PEP 515 <https://www.python.org/dev/peps/pep-0515/>`_ | Underscores in Numeric Literals                  |              |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | `PEP 525 <https://www.python.org/dev/peps/pep-0525/>`_ | Asynchronous Generators                          |              |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | `PEP 526 <https://www.python.org/dev/peps/pep-0526/>`_ | Syntax for Variable Annotations (provisional)    |              |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | `PEP 530 <https://www.python.org/dev/peps/pep-0530/>`_ | Asynchronous Comprehensions                      |              |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | **New Built-in Features:**                                                                                               |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | `PEP 468 <https://www.python.org/dev/peps/pep-0468/>`_ | Preserving the order of *kwargs* in a function   |              |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | `PEP 487 <https://www.python.org/dev/peps/pep-0487/>`_ | Simpler customization of class creation          |              |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | `PEP 520 <https://www.python.org/dev/peps/pep-0520/>`_ | Preserving Class Attribute Definition Order      |              |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | **Standard Library Changes:**                                                                                            |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | `PEP 495 <https://www.python.org/dev/peps/pep-0495/>`_ | Local Time Disambiguation                        |              |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | `PEP 506 <https://www.python.org/dev/peps/pep-0506/>`_ | Adding A Secrets Module To The Standard Library  |              |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | `PEP 519 <https://www.python.org/dev/peps/pep-0519/>`_ | Adding a file system path protocol               |              |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | **CPython internals:**                                                                                                   |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | `PEP 509 <https://www.python.org/dev/peps/pep-0509/>`_ | Add a private version to dict                    |              |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | `PEP 523 <https://www.python.org/dev/peps/pep-0523/>`_ | Adding a frame evaluation API to CPython         |              |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | **Linux/Window Changes**                                                                                                 |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | `PEP 524 <https://www.python.org/dev/peps/pep-0524/>`_ | Make os.urandom() blocking on Linux              |              |
  |                                                        | (during system startup)                          |              |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | `PEP 528 <https://www.python.org/dev/peps/pep-0528/>`_ | Change Windows console encoding to UTF-8         |              |
  +--------------------------------------------------------+--------------------------------------------------+--------------+
  | `PEP 529 <https://www.python.org/dev/peps/pep-0529/>`_ | Change Windows filesystem encoding to UTF-8      |              |
  +--------------------------------------------------------+--------------------------------------------------+--------------+

Other Language Changes:

  +-------------------------------------------------------------------------------------------------------------+---------------+
  | A *global* or *nonlocal* statement must now textually appear before the first use of the affected name in   |               |
  | the same scope. Previously this was a SyntaxWarning.                                                        |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | It is now possible to set a special method to None to indicate that the corresponding operation is not      |               |
  | available. For example, if a class sets *__iter__()* to *None* , the class is not iterable.                 |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Long sequences of repeated traceback lines are now abbreviated as *[Previous line repeated {count} more     |               |
  | times]*                                                                                                     |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Import now raises the new exception *ModuleNotFoundError* when it cannot find a module. Code that currently |               |
  | checks for ImportError (in try-except) will still work.                                                     |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Class methods relying on zero-argument *super()* will now work correctly when called from metaclass methods |               |
  | during class creation.                                                                                      |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+

Changes to built-in modules:

  +--------------------------------------------------------------------------------------------------------------+----------------+
  | `array <https://docs.python.org/3.6/whatsnew/3.6.html#array>`_                                               |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | Exhausted iterators of *array.array* will now stay exhausted even if the iterated array is extended.         |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | `binascii <https://docs.python.org/3.6/whatsnew/3.6.html#binascii>`_                                         |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The b2a_base64() function now accepts an optional newline keyword argument to control whether the newline    | Completed      |
  | character is appended to the return value                                                                    |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | `cmath <https://docs.python.org/3.6/whatsnew/3.6.html#cmath>`_                                               |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The new cmath.tau (τ) constant has been added                                                                |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | New constants: *cmath.inf* and *cmath.nan* to match *math.inf* and *math.nan* , and also *cmath.infj* and    |                |
  | *cmath.nanj* to match the format used by complex repr                                                        |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | `collections <https://docs.python.org/3.6/whatsnew/3.6.html#collections>`_                                                    |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The new Collection abstract base class has been added to represent sized iterable container classes          |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The new *Reversible* abstract base class represents iterable classes that also provide the *__reversed__()*  |                |
  | method.                                                                                                      |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The new *AsyncGenerator* abstract base class represents asynchronous generators.                             |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The *namedtuple()* function now accepts an optional keyword argument module, which, when specified, is used  |                |
  | for the *__module__* attribute of the returned named tuple class.                                            |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The verbose and rename arguments for *namedtuple()* are now keyword-only.                                    |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | Recursive *collections.deque* instances can now be pickled.                                                  |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | `hashlib <https://docs.python.org/3.6/whatsnew/3.6.html#hashlib>`_                                                            |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | BLAKE2 hash functions were added to the module. *blake2b()* and *blake2s()* are always available and support |                |
  | the full feature set of BLAKE2.                                                                              |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The SHA-3 hash functions *sha3_224()*, *sha3_256()*, *sha3_384()*, *sha3_512()*, and *SHAKE* hash functions  |                |
  | *shake_128()* and *shake_256()* were added.                                                                  |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The password-based key derivation function *scrypt()* is now available with OpenSSL 1.1.0 and newer.         |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | `json <https://docs.python.org/3.6/whatsnew/3.6.html#json>`_                                                                  |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | *json.load()* and *json.loads()* now support binary input. Encoded JSON should be represented using either   |                |
  | UTF-8, UTF-16, or UTF-32.                                                                                    |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | `math <https://docs.python.org/3.6/whatsnew/3.6.html#math>`_                                                                  |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The new math.tau (τ) constant has been added                                                                 | Completed      |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | `os <https://docs.python.org/3.6/whatsnew/3.6.html#os>`_                                                                      |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | A new *close()* method allows explicitly closing a *scandir()* iterator. The *scandir()* iterator now        |                |
  | supports the context manager protocol.                                                                       |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | On Linux, *os.urandom()* now blocks until the system urandom entropy pool is initialized to increase the     |                |
  | security.                                                                                                    |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The Linux *getrandom()* syscall (get random bytes) is now exposed as the new *os.getrandom()* function.      |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | `re <https://docs.python.org/3.6/whatsnew/3.6.html#re>`_                                                                      |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | Added support of modifier spans in regular expressions. Examples: *'(?i:p)ython'* matches 'python' and       |                |
  | 'Python', but not 'PYTHON'; *'(?i)g(?-i:v)r'* matches *'GvR'* and *'gvr'*, but not *'GVR'* .                 |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | Match object groups can be accessed by *__getitem__*, which is equivalent to *group()*. So *mo['name']* is   |                |
  | now equivalent to *mo.group('name')*.                                                                        |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | Match objects now support index-like objects as group indices.                                               |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | `socket <https://docs.python.org/3.6/whatsnew/3.6.html#socket>`_                                                              |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The *ioctl()* function now supports the *SIO_LOOPBACK_FAST_PATH* control code.                               |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The *getsockopt()* constants *SO_DOMAIN* , *SO_PROTOCOL*, *SO_PEERSEC* , and *SO_PASSSEC* are now supported. |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The *setsockopt()* now supports the *setsockopt(level, optname, None, optlen: int)* form.                    |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The socket module now supports the address family *AF_ALG* to interface with Linux Kernel crypto API.        |                |
  | *ALG_*, *SOL_ALG* and *sendmsg_afalg()* were added.                                                          |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | New Linux constants *TCP_USER_TIMEOUT* and *TCP_CONGESTION* were added.                                      |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | `ssl <https://docs.python.org/3.6/whatsnew/3.6.html#ssl>`_                                                                    |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | ssl supports OpenSSL 1.1.0. The minimum recommend version is 1.0.2.                                          |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | 3DES has been removed from the default cipher suites and ChaCha20 Poly1305 cipher suites have been added.    |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | *SSLContext* has better default configuration for options and ciphers.                                       |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | SSL session can be copied from one client-side connection to another with the new *SSLSession* class. TLS    |                |
  | session resumption can speed up the initial handshake, reduce latency and improve performance.               |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The new *get_ciphers()* method can be used to get a list of enabled ciphers in order of cipher priority.     |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | All constants and flags have been converted to *IntEnum* and *IntFlags*.                                     |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | Server and client-side specific TLS protocols for *SSLContext* were added.                                   |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | Added *SSLContext.post_handshake_auth* to enable and *ssl.SSLSocket.verify_client_post_handshake()* to       |                |
  | initiate TLS 1.3 post-handshake authentication.                                                              |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | `struct <https://docs.python.org/3.6/whatsnew/3.6.html#struct>`_                                             |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | now supports IEEE 754 half-precision floats via the 'e' format specifier.                                    |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | `sys <https://docs.python.org/3.6/whatsnew/3.6.html#sys>`_                                                   |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The new *getfilesystemencodeerrors()* function returns the name of the error mode used to convert between    |                |
  | Unicode filenames and bytes filenames.                                                                       |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | `zlib <https://docs.python.org/3.6/whatsnew/3.6.html#zlib>`_                                                 |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
  | The *compress()* and *decompress()* functions now accept keyword arguments                                   |                |
  +--------------------------------------------------------------------------------------------------------------+----------------+
