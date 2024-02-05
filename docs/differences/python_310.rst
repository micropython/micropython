.. _python_310:

Python 3.10
===========

Python 3.10.0 (final) was released on the 4 October 2021. The Features for 3.10 are
defined in `PEP 619 <https://www.python.org/dev/peps/pep-0619/#features-for-3-10>`_
and a detailed description of the changes can be found in
`What's New in Python 3.10 <https://docs.python.org/3/whatsnew/3.10.html>`_.

.. table::
  :widths: 20 60 20

  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | **New syntax features**                                                                                     | **Status**   |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 634 <https://www.python.org/dev/peps/pep-0634/>`_ | Structural Pattern Matching: Specification         | [#spm]_      |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 635 <https://www.python.org/dev/peps/pep-0635/>`_ | Structural Pattern Matching: Motivation and        | [#spm]_      |
  |                                                        | Rationale                                          |              |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 636 <https://www.python.org/dev/peps/pep-0636/>`_ | Structural Pattern Matching: Tutorial              | [#spm]_      |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `bpo-12782                                             | Parenthesized context managers are now officially  |              |
  | <https://github.com/python/cpython/issues/56991>`_     | allowed                                            |              |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | **New features in the standard library**                                                                                   |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 618 <https://www.python.org/dev/peps/pep-0618/>`_ | Add Optional Length-Checking To zip                |              |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | **Interpreter improvements**                                                                                               |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 626 <https://www.python.org/dev/peps/pep-0626/>`_ | Precise line numbers for debugging and other tools |              |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | **New typing features**                                                                                                    |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 604 <https://www.python.org/dev/peps/pep-0604/>`_ | Allow writing union types as X | Y                 |              |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 613 <https://www.python.org/dev/peps/pep-0613/>`_ | Explicit Type Aliases                              |              |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 612 <https://www.python.org/dev/peps/pep-0612/>`_ | Parameter Specification Variables                  |              |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | **Important deprecations, removals or restrictions**                                                                       |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 644 <https://www.python.org/dev/peps/pep-0644/>`_ | Require OpenSSL 1.1.1 or newer                     |              |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 632 <https://www.python.org/dev/peps/pep-0632/>`_ | Deprecate distutils module.                        | Not relevant |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 623 <https://www.python.org/dev/peps/pep-0623/>`_ | Deprecate and prepare for the removal of the wstr  | Not relevant |
  |                                                        | member in PyUnicodeObject.                         |              |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 624 <https://www.python.org/dev/peps/pep-0624/>`_ | Remove Py_UNICODE encoder APIs                     | Not relevant |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 597 <https://www.python.org/dev/peps/pep-0597/>`_ | Add optional EncodingWarning                       |              |
  +--------------------------------------------------------+----------------------------------------------------+--------------+


Other Language Changes:

.. table::
  :widths: 90 10

  +-------------------------------------------------------------------------------------------------------------+---------------+
  | The :class:`int` type has a new method :meth:`int.bit_count`, returning the                                 |               |
  | number of ones in the binary expansion of a given integer, also known                                       |               |
  | as the population count.                                                                                    |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | The views returned by :meth:`dict.keys`, :meth:`dict.values` and                                            |               |
  | :meth:`dict.items` now all have a ``mapping`` attribute that gives a                                        |               |
  | :class:`types.MappingProxyType` object wrapping the original                                                |               |
  | dictionary.                                                                                                 |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | :pep:`618`: The :func:`zip` function now has an optional ``strict`` flag, used                              |               |
  | to require that all the iterables have an equal length.                                                     |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Builtin and extension functions that take integer arguments no longer accept                                |               |
  | :class:`~decimal.Decimal`\ s, :class:`~fractions.Fraction`\ s and other                                     |               |
  | objects that can be converted to integers only with a loss (e.g. that have                                  |               |
  | the :meth:`~object.__int__` method but do not have the                                                      |               |
  | :meth:`~object.__index__` method).                                                                          |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | If :func:`object.__ipow__` returns :const:`NotImplemented`, the operator will                               |               |
  | correctly fall back to :func:`object.__pow__` and :func:`object.__rpow__` as expected.                      |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Assignment expressions can now be used unparenthesized within set literals                                  |               |
  | and set comprehensions, as well as in sequence indexes (but not slices).                                    |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Functions have a new ``__builtins__`` attribute which is used to look for                                   |               |
  | builtin symbols when a function is executed, instead of looking into                                        |               |
  | ``__globals__['__builtins__']``. The attribute is initialized from                                          |               |
  | ``__globals__["__builtins__"]`` if it exists, else from the current builtins.                               |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Two new builtin functions -- :func:`aiter` and :func:`anext` have been added                                |               |
  | to provide asynchronous counterparts to :func:`iter` and :func:`next`,                                      |               |
  | respectively.                                                                                               |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Static methods (:func:`@staticmethod <staticmethod>`) and class methods                                     |               |
  | (:func:`@classmethod <classmethod>`) now inherit the method attributes                                      |               |
  | (``__module__``, ``__name__``, ``__qualname__``, ``__doc__``,                                               |               |
  | ``__annotations__``) and have a new ``__wrapped__`` attribute.                                              |               |
  | Moreover, static methods are now callable as regular functions.                                             |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Annotations for complex targets (everything beside ``simple name`` targets                                  |               |
  | defined by :pep:`526`) no longer cause any runtime effects with ``from __future__ import annotations``.     |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Class and module objects now lazy-create empty annotations dicts on demand.                                 |               |
  | The annotations dicts are stored in the object’s ``__dict__`` for                                           |               |
  | backwards compatibility.  This improves the best practices for working                                      |               |
  | with ``__annotations__``.                                                                                   |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Annotations consist of ``yield``, ``yield from``, ``await`` or named expressions                            |               |
  | are now forbidden under ``from __future__ import annotations`` due to their side                            |               |
  | effects.                                                                                                    |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Usage of unbound variables, ``super()`` and other expressions that might                                    |               |
  | alter the processing of symbol table as annotations are now rendered                                        |               |
  | effectless under ``from __future__ import annotations``.                                                    |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Hashes of NaN values of both :class:`float` type and                                                        |               |
  | :class:`decimal.Decimal` type now depend on object identity. Formerly, they                                 |               |
  | always hashed to ``0`` even though NaN values are not equal to one another.                                 |               |
  | This caused potentially quadratic runtime behavior due to excessive hash                                    |               |
  | collisions when creating dictionaries and sets containing multiple NaNs.                                    |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | A :exc:`SyntaxError` (instead of a :exc:`NameError`) will be raised when deleting                           |               |
  | the :const:`__debug__` constant.                                                                            |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | :exc:`SyntaxError` exceptions now have ``end_lineno`` and                                                   |               |
  | ``end_offset`` attributes.  They will be ``None`` if not determined.                                        |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+

Changes to built-in modules:

.. table::
  :widths: 90 10

  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `asyncio <https://docs.python.org/3/whatsnew/3.10.html#asyncio>`_                                                             |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add missing :meth:`~asyncio.events.AbstractEventLoop.connect_accepted_socket`                                 |               |
  | method.                                                                                                       |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `array <https://docs.python.org/3/whatsnew/3.10.html#array>`_                                                                 |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The :meth:`~array.array.index` method of :class:`array.array` now has                                         |               |
  | optional *start* and *stop* parameters.                                                                       |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `gc <https://docs.python.org/3/whatsnew/3.10.html#gc>`_                                                                       |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add audit hooks for :func:`gc.get_objects`, :func:`gc.get_referrers` and                                      |               |
  | :func:`gc.get_referents`.                                                                                     |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `hashlib <https://docs.python.org/3/whatsnew/3.10.html#hashlib>`_                                                             |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The hashlib module requires OpenSSL 1.1.1 or newer.                                                           |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The hashlib module has preliminary support for OpenSSL 3.0.0.                                                 |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The pure-Python fallback of :func:`~hashlib.pbkdf2_hmac` is deprecated. In                                    |               |
  | the future PBKDF2-HMAC will only be available when Python has been built with                                 |               |
  | OpenSSL support.                                                                                              |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `os <https://docs.python.org/3/whatsnew/3.10.html#os>`_                                                                       |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add :func:`os.cpu_count()` support for VxWorks RTOS.                                                          |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add a new function :func:`os.eventfd` and related helpers to wrap the                                         |               |
  | ``eventfd2`` syscall on Linux.                                                                                |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add :func:`os.splice()` that allows to move data between two file                                             |               |
  | descriptors without copying between kernel address space and user                                             |               |
  | address space, where one of the file descriptors must refer to a                                              |               |
  | pipe.                                                                                                         |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add :data:`~os.O_EVTONLY`, :data:`~os.O_FSYNC`, :data:`~os.O_SYMLINK`                                         |               |
  | and :data:`~os.O_NOFOLLOW_ANY` for macOS.                                                                     |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `platform <https://docs.python.org/3/whatsnew/3.10.html#platform>`_                                                           |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add :func:`platform.freedesktop_os_release()` to retrieve operation system                                    |               |
  | identification from `freedesktop.org os-release                                                               |               |
  | <https://www.freedesktop.org/software/systemd/man/os-release.html>`_ standard file.                           |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `socket <https://docs.python.org/3/whatsnew/3.10.html#socket>`_                                                               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The exception :exc:`socket.timeout` is now an alias of :exc:`TimeoutError`.                                   |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add option to create MPTCP sockets with ``IPPROTO_MPTCP``.                                                    |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add ``IP_RECVTOS`` option to receive the type of service (ToS) or DSCP/ECN fields.                            |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `ssl <https://docs.python.org/3/whatsnew/3.10.html#ssl>`_                                                                     |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The ssl module requires OpenSSL 1.1.1 or newer.                                                               |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The ssl module has preliminary support for OpenSSL 3.0.0 and new option                                       |               |
  | :data:`~ssl.OP_IGNORE_UNEXPECTED_EOF`.                                                                        |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Deprecated function and use of deprecated constants now result in                                             |               |
  | a :exc:`DeprecationWarning`. :attr:`ssl.SSLContext.options` has                                               |               |
  | :data:`~ssl.OP_NO_SSLv2` and :data:`~ssl.OP_NO_SSLv3` set by default and                                      |               |
  | therefore cannot warn about setting the flag again.                                                           |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The ssl module now has more secure default settings. Ciphers without forward                                  |               |
  | secrecy or SHA-1 MAC are disabled by default. Security level 2 prohibits                                      |               |
  | weak RSA, DH, and ECC keys with less than 112 bits of security.                                               |               |
  | :class:`~ssl.SSLContext` defaults to minimum protocol version TLS 1.2.                                        |               |
  | Settings are based on Hynek Schlawack's research.                                                             |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The deprecated protocols SSL 3.0, TLS 1.0, and TLS 1.1 are no longer                                          |               |
  | officially supported. Python does not block them actively. However                                            |               |
  | OpenSSL build options, distro configurations, vendor patches, and cipher                                      |               |
  | suites may prevent a successful handshake.                                                                    |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add a *timeout* parameter to the :func:`ssl.get_server_certificate` function.                                 |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The ssl module uses heap-types and multi-phase initialization.                                                |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | A new verify flag :data:`~ssl.VERIFY_X509_PARTIAL_CHAIN` has been added.                                      |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `sys <https://docs.python.org/3/whatsnew/3.10.html#sys>`_                                                                     |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add :data:`sys.orig_argv` attribute: the list of the original command line                                    |               |
  | arguments passed to the Python executable.                                                                    |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add :data:`sys.stdlib_module_names`, containing the list of the standard library                              |               |
  | module names.                                                                                                 |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `_thread <https://docs.python.org/3/whatsnew/3.10.html#_thread>`_                                                             |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | :func:`_thread.interrupt_main` now takes an optional signal number to                                         |               |
  | simulate (the default is still :data:`signal.SIGINT`).                                                        |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+

.. rubric:: Notes

.. [#spm] The structural pattern matching feature is discussed in `issue #7847 <https://github.com/micropython/micropython/issues/7847>`_.

