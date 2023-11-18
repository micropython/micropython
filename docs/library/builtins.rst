:mod:`builtins` -- builtin functions and exceptions
===================================================

..  module:: builtins
    :synopsis: builtin Python functions

All builtin functions and exceptions are described here. They are also
available via the ``builtins`` module.

For more information about built-ins, see the following CPython documentation:

* `Builtin CPython Functions <https://docs.python.org/3/library/functions.html>`_
* `Builtin CPython Exceptions <https://docs.python.org/3/library/exceptions.html>`_
* `Builtin CPython Constants <https://docs.python.org/3/library/constants.html>`_

.. note:: Not all of these functions, types, exceptions, and constants are turned
    on in all CircuitPython ports, for space reasons.

Functions and types
-------------------

.. function:: abs()

.. function:: all()

.. function:: any()

.. function:: bin()

.. class:: bool()

.. class:: bytearray()

.. class:: bytes()

    |see_cpython| `python:bytes`.

.. function:: callable()

.. function:: chr()

.. function:: classmethod()

.. function:: compile()

.. class:: complex()

.. function:: delattr(obj, name)

   The argument *name* should be a string, and this function deletes the named
   attribute from the object given by *obj*.

.. class:: dict()

.. function:: dir()

.. function:: divmod()

.. function:: enumerate()

.. function:: eval()

.. function:: exec()

.. function:: filter()

.. class:: float()

.. class:: frozenset()

`frozenset()` is not enabled on the smallest CircuitPython boards for space reasons.

.. function:: getattr()

.. function:: globals()

.. function:: hasattr()

.. function:: hash()

.. function:: hex()

.. function:: id()

.. function:: input()

.. class:: int()

   .. classmethod:: from_bytes(bytes, byteorder)

      In CircuitPython, the ``byteorder`` parameter must be positional (this is
      compatible with CPython).

   .. method:: to_bytes(size, byteorder)

      In CircuitPython, the ``byteorder`` parameter must be positional (this is
      compatible with CPython).

.. function:: isinstance()

.. function:: issubclass()

.. function:: iter()

.. function:: len()

.. class:: list()

.. function:: locals()

.. function:: map()

.. function:: max()

.. class:: memoryview()

.. function:: min()

.. function:: next()

.. class:: object()

.. function:: oct()

.. function:: open()

.. function:: ord()

.. function:: pow()

.. function:: print()

.. function:: property()

.. function:: range()

.. function:: repr()

.. function:: reversed()

`reversed()` is not enabled on the smallest CircuitPython boards for space reasons.

.. function:: round()

.. class:: set()

.. function:: setattr()

.. class:: slice()

   The *slice* builtin is the type that slice objects have.

.. function:: sorted()

.. function:: staticmethod()

.. class:: str()

.. function:: sum()

.. function:: super()

.. class:: tuple()

.. function:: type()

.. function:: zip()


Exceptions
----------

.. exception:: ArithmeticError

.. exception:: AssertionError

.. exception:: AttributeError

.. exception:: BaseException

.. exception:: BrokenPipeError

.. exception:: ConnectionError

.. exception:: EOFError

.. exception:: Exception

.. exception:: ImportError

.. exception:: IndentationError

.. exception:: IndexError

.. exception:: KeyboardInterrupt

.. exception:: KeyError

.. exception:: LookupError

.. exception:: MemoryError

.. exception:: NameError

.. exception:: NotImplementedError

.. exception:: OSError

.. exception:: OverflowError

.. exception:: RuntimeError

.. exception:: ReloadException

   `ReloadException` is used internally to deal with soft restarts.

   Not a part of the CPython standard library

.. exception:: StopAsyncIteration

.. exception:: StopIteration

.. exception:: SyntaxError

.. exception:: SystemExit

    |see_cpython| `python:SystemExit`.

.. exception:: TimeoutError

.. exception:: TypeError

    |see_cpython| `python:TypeError`.

.. exception:: UnicodeError

.. exception:: ValueError

.. exception:: ZeroDivisionError

Constants
---------

.. data:: Ellipsis

.. data:: NotImplemented
