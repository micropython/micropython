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
  | `PEP 634 <https://www.python.org/dev/peps/pep-0634/>`_ | Structural Pattern Matching: Specification         | Not          |
  |                                                        |                                                    | implemented  |
  |                                                        |                                                    | [#spm]_      |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 635 <https://www.python.org/dev/peps/pep-0635/>`_ | Structural Pattern Matching: Motivation and        | Not          |
  |                                                        | Rationale                                          | implemented  |
  |                                                        |                                                    | [#spm]_      |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 636 <https://www.python.org/dev/peps/pep-0636/>`_ | Structural Pattern Matching: Tutorial              | Not          |
  |                                                        |                                                    | implemented  |
  |                                                        |                                                    | [#spm]_      |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `bpo-12782                                             | Parenthesized context managers are now officially  | Not          |
  | <https://github.com/python/cpython/issues/56991>`_     | allowed                                            | implemented  |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | **New features in the standard library**                                                                                   |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 618 <https://www.python.org/dev/peps/pep-0618/>`_ | Add Optional Length-Checking To zip                | Not          |
  |                                                        |                                                    | implemented  |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | **Interpreter improvements**                                                                                               |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 626 <https://www.python.org/dev/peps/pep-0626/>`_ | Precise line numbers for debugging and other tools | Not          |
  |                                                        |                                                    | implemented  |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | **New typing features**                                                                                                    |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 604 <https://www.python.org/dev/peps/pep-0604/>`_ | Allow writing union types as X | Y                 | Not          |
  |                                                        |                                                    | implemented  |
  |                                                        |                                                    | [#ann]_      |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 613 <https://www.python.org/dev/peps/pep-0613/>`_ | Explicit Type Aliases                              | Not          |
  |                                                        |                                                    | implemented  |
  |                                                        |                                                    | [#ann]_      |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 612 <https://www.python.org/dev/peps/pep-0612/>`_ | Parameter Specification Variables                  | Not          |
  |                                                        |                                                    | implemented  |
  |                                                        |                                                    | [#ann]_      |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | **Important deprecations, removals or restrictions**                                                                       |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 644 <https://www.python.org/dev/peps/pep-0644/>`_ | Require OpenSSL 1.1.1 or newer                     | Not relevant |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 632 <https://www.python.org/dev/peps/pep-0632/>`_ | Deprecate distutils module.                        | Not relevant |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 623 <https://www.python.org/dev/peps/pep-0623/>`_ | Deprecate and prepare for the removal of the wstr  | Not relevant |
  |                                                        | member in PyUnicodeObject.                         |              |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 624 <https://www.python.org/dev/peps/pep-0624/>`_ | Remove Py_UNICODE encoder APIs                     | Not relevant |
  +--------------------------------------------------------+----------------------------------------------------+--------------+
  | `PEP 597 <https://www.python.org/dev/peps/pep-0597/>`_ | Add optional EncodingWarning                       | Not relevant |
  +--------------------------------------------------------+----------------------------------------------------+--------------+


Other Language Changes:

.. table::
  :widths: 90 10

  +-------------------------------------------------------------------------------------------------------------+---------------+
  | The :class:`int` type has a new method :meth:`int.bit_count`, returning the                                 | Not           |
  | number of ones in the binary expansion of a given integer, also known                                       | implemented   |
  | as the population count.                                                                                    |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | The views returned by :meth:`dict.keys`, :meth:`dict.values` and                                            | Not           |
  | :meth:`dict.items` now all have a ``mapping`` attribute that gives a                                        | implemented   |
  | :class:`types.MappingProxyType` object wrapping the original                                                |               |
  | dictionary.                                                                                                 |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | :pep:`618`: The :func:`zip` function now has an optional ``strict`` flag, used                              | Not           |
  | to require that all the iterables have an equal length.                                                     | implemented   |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Builtin and extension functions that take integer arguments no longer accept                                | Not           |
  | :class:`~decimal.Decimal`\ s, :class:`~fractions.Fraction`\ s and other                                     | implemented   |
  | objects that can be converted to integers only with a loss (e.g. that have                                  |               |
  | the :meth:`~object.__int__` method but do not have the                                                      |               |
  | :meth:`~object.__index__` method).                                                                          |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | If :func:`object.__ipow__` returns :const:`NotImplemented`, the operator will                               | Complete      |
  | correctly fall back to :func:`object.__pow__` and :func:`object.__rpow__` as expected.                      |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Assignment expressions can now be used unparenthesized within set literals                                  | Complete      |
  | and set comprehensions, as well as in sequence indexes (but not slices).                                    |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Functions have a new ``__builtins__`` attribute which is used to look for                                   | Not           |
  | builtin symbols when a function is executed, instead of looking into                                        | implemented   |
  | ``__globals__['__builtins__']``. The attribute is initialized from                                          |               |
  | ``__globals__["__builtins__"]`` if it exists, else from the current builtins.                               |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Two new builtin functions -- :func:`aiter` and :func:`anext` have been added                                | Not           |
  | to provide asynchronous counterparts to :func:`iter` and :func:`next`,                                      | implemented   |
  | respectively.                                                                                               |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Static methods (:func:`@staticmethod <staticmethod>`) and class methods                                     | Not           |
  | (:func:`@classmethod <classmethod>`) now inherit the method attributes                                      | implemented   |
  | (``__module__``, ``__name__``, ``__qualname__``, ``__doc__``,                                               |               |
  | ``__annotations__``) and have a new ``__wrapped__`` attribute.                                              |               |
  | Moreover, static methods are now callable as regular functions.                                             |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Annotations for complex targets (everything beside ``simple name`` targets                                  | Partial       |
  | defined by :pep:`526`) no longer cause any runtime effects with ``from __future__ import annotations``.     | [#ann]_       |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Class and module objects now lazy-create empty annotations dicts on demand.                                 | Not           |
  | The annotations dicts are stored in the object’s ``__dict__`` for                                           | implemented   |
  | backwards compatibility.  This improves the best practices for working                                      |               |
  | with ``__annotations__``.                                                                                   |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Annotations consist of ``yield``, ``yield from``, ``await`` or named expressions                            | Not           |
  | are now forbidden under ``from __future__ import annotations`` due to their side                            | implemented   |
  | effects.                                                                                                    |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Usage of unbound variables, ``super()`` and other expressions that might                                    | Partial       |
  | alter the processing of symbol table as annotations are now rendered                                        | [#ann]_       |
  | effectless under ``from __future__ import annotations``.                                                    |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | Hashes of NaN values of both :class:`float` type and                                                        | Not           |
  | :class:`decimal.Decimal` type now depend on object identity. Formerly, they                                 | implemented   |
  | always hashed to ``0`` even though NaN values are not equal to one another.                                 |               |
  | This caused potentially quadratic runtime behavior due to excessive hash                                    |               |
  | collisions when creating dictionaries and sets containing multiple NaNs.                                    |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | A :exc:`SyntaxError` (instead of a :exc:`NameError`) will be raised when deleting                           | Complete      |
  | the :const:`__debug__` constant.                                                                            |               |
  +-------------------------------------------------------------------------------------------------------------+---------------+
  | :exc:`SyntaxError` exceptions now have ``end_lineno`` and                                                   | Not           |
  | ``end_offset`` attributes.  They will be ``None`` if not determined.                                        | implemented   |
  +-------------------------------------------------------------------------------------------------------------+---------------+

Changes to built-in modules:

.. table::
  :widths: 90 10

  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `asyncio <https://docs.python.org/3/whatsnew/3.10.html#asyncio>`_                                                             |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add missing :meth:`~asyncio.events.AbstractEventLoop.connect_accepted_socket`                                 | Not           |
  | method.                                                                                                       | implemented   |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `array <https://docs.python.org/3/whatsnew/3.10.html#array>`_                                                                 |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The :meth:`~array.array.index` method of :class:`array.array` now has                                         | Not           |
  | optional *start* and *stop* parameters.                                                                       | implemented   |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `gc <https://docs.python.org/3/whatsnew/3.10.html#gc>`_                                                                       |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add audit hooks for :func:`gc.get_objects`, :func:`gc.get_referrers` and                                      | Not relevant  |
  | :func:`gc.get_referents`.                                                                                     |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `hashlib <https://docs.python.org/3/whatsnew/3.10.html#hashlib>`_                                                             |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The hashlib module requires OpenSSL 1.1.1 or newer.                                                           | Not relevant  |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The hashlib module has preliminary support for OpenSSL 3.0.0.                                                 | Not relevant  |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The pure-Python fallback of :func:`~hashlib.pbkdf2_hmac` is deprecated. In                                    | Not relevant  |
  | the future PBKDF2-HMAC will only be available when Python has been built with                                 |               |
  | OpenSSL support.                                                                                              |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `os <https://docs.python.org/3/whatsnew/3.10.html#os>`_                                                                       |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add :func:`os.cpu_count()` support for VxWorks RTOS.                                                          | Not relevant  |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add a new function :func:`os.eventfd` and related helpers to wrap the                                         | Not relevant  |
  | ``eventfd2`` syscall on Linux.                                                                                |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add :func:`os.splice()` that allows to move data between two file                                             | Not relevant  |
  | descriptors without copying between kernel address space and user                                             |               |
  | address space, where one of the file descriptors must refer to a                                              |               |
  | pipe.                                                                                                         |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add :data:`~os.O_EVTONLY`, :data:`~os.O_FSYNC`, :data:`~os.O_SYMLINK`                                         | Not relevant  |
  | and :data:`~os.O_NOFOLLOW_ANY` for macOS.                                                                     |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `platform <https://docs.python.org/3/whatsnew/3.10.html#platform>`_                                                           |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add :func:`platform.freedesktop_os_release()` to retrieve operation system                                    | Not relevant  |
  | identification from `freedesktop.org os-release                                                               |               |
  | <https://www.freedesktop.org/software/systemd/man/os-release.html>`_ standard file.                           |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `socket <https://docs.python.org/3/whatsnew/3.10.html#socket>`_                                                               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The exception :exc:`socket.timeout` is now an alias of :exc:`TimeoutError`.                                   | Not           |
  |                                                                                                               | implemented   |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add option to create MPTCP sockets with ``IPPROTO_MPTCP``.                                                    | Not           |
  |                                                                                                               | implemented   |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add ``IP_RECVTOS`` option to receive the type of service (ToS) or DSCP/ECN fields.                            | Not           |
  |                                                                                                               | implemented   |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `ssl <https://docs.python.org/3/whatsnew/3.10.html#ssl>`_                                                                     |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The ssl module requires OpenSSL 1.1.1 or newer.                                                               | Not relevant  |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The ssl module has preliminary support for OpenSSL 3.0.0 and new option                                       | Not relevant  |
  | :data:`~ssl.OP_IGNORE_UNEXPECTED_EOF`.                                                                        |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Deprecated function and use of deprecated constants now result in                                             | Not relevant  |
  | a :exc:`DeprecationWarning`. :attr:`ssl.SSLContext.options` has                                               |               |
  | :data:`~ssl.OP_NO_SSLv2` and :data:`~ssl.OP_NO_SSLv3` set by default and                                      |               |
  | therefore cannot warn about setting the flag again.                                                           |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The ssl module now has more secure default settings. Ciphers without forward                                  | Not relevant  |
  | secrecy or SHA-1 MAC are disabled by default. Security level 2 prohibits                                      |               |
  | weak RSA, DH, and ECC keys with less than 112 bits of security.                                               |               |
  | :class:`~ssl.SSLContext` defaults to minimum protocol version TLS 1.2.                                        |               |
  | Settings are based on Hynek Schlawack's research.                                                             |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The deprecated protocols SSL 3.0, TLS 1.0, and TLS 1.1 are no longer                                          | Partial       |
  | officially supported. Python does not block them actively. However                                            | [#tls]_       |
  | OpenSSL build options, distro configurations, vendor patches, and cipher                                      |               |
  | suites may prevent a successful handshake.                                                                    |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add a *timeout* parameter to the :func:`ssl.get_server_certificate` function.                                 | Not           |
  |                                                                                                               | implemented   |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | The ssl module uses heap-types and multi-phase initialization.                                                | Not relevant  |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | A new verify flag :data:`~ssl.VERIFY_X509_PARTIAL_CHAIN` has been added.                                      | Not relevant  |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `sys <https://docs.python.org/3/whatsnew/3.10.html#sys>`_                                                                     |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add :data:`sys.orig_argv` attribute: the list of the original command line                                    | Not           |
  | arguments passed to the Python executable.                                                                    | implemented   |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | Add :data:`sys.stdlib_module_names`, containing the list of the standard library                              | Not           |
  | module names.                                                                                                 | implemented   |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | `_thread <https://docs.python.org/3/whatsnew/3.10.html#_thread>`_                                                             |
  +---------------------------------------------------------------------------------------------------------------+---------------+
  | :func:`_thread.interrupt_main` now takes an optional signal number to                                         | Not relevant  |
  | simulate (the default is still :data:`signal.SIGINT`).                                                        |               |
  +---------------------------------------------------------------------------------------------------------------+---------------+

.. rubric:: Notes

.. [#spm] The structural pattern matching feature is discussed in `issue #7847 <https://github.com/micropython/micropython/issues/7847>`_.

.. [#ann] MicroPython parses type annotations but never evaluates them, so annotations produce no runtime
   effects and no ``__annotations__`` attributes are created.  There is also no ``typing`` module and no
   ``__future__`` module, so these features cannot be used other than as inert annotations.

.. [#tls] MicroPython's ``ssl`` module is implemented on top of Mbed TLS (axTLS on a few ports), not
   OpenSSL.  Ports using Mbed TLS 3.x support TLS 1.2 and later only, so the deprecated protocols are
   already unavailable there, but ports using axTLS support only the older protocols.

