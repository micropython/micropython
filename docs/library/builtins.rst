Builtin functions and exceptions
================================

All builtin functions and exceptions are described here. They are also
available via ``builtins`` module.

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

.. function:: getattr()

.. function:: globals()

.. function:: hasattr()

.. function:: hash()

.. function:: hex()

.. function:: id()

.. function:: input()

.. class:: int()

   .. classmethod:: from_bytes(bytes, byteorder)

      In MicroPython, `byteorder` parameter must be positional (this is
      compatible with CPython).

   .. method:: to_bytes(size, byteorder)

      In MicroPython, `byteorder` parameter must be positional (this is
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

.. exception:: AssertionError

.. exception:: AttributeError

.. exception:: Exception

.. exception:: ImportError

.. exception:: IndexError

.. exception:: KeyboardInterrupt

.. exception:: KeyError

.. exception:: MemoryError

.. exception:: NameError

.. exception:: NotImplementedError

.. exception:: OSError

    |see_cpython| `python:OSError`. MicroPython doesn't implement ``errno``
    attribute, instead use the standard way to access exception arguments:
    ``exc.args[0]``.

.. exception:: RuntimeError

.. exception:: StopIteration

.. exception:: SyntaxError

.. exception:: SystemExit

    |see_cpython| `python:SystemExit`.

.. exception:: TypeError

    |see_cpython| `python:TypeError`.

.. exception:: ValueError

.. exception:: ZeroDivisionError
