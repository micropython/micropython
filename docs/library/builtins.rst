:mod:`builtins` -- builtin functions and exceptions
===================================================

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

    |see_cpython| `python:bytearray`.

.. class:: bytes()

    |see_cpython| `python:bytes`.

    .. method:: bytes.decode(encoding='utf-8', errors='strict')

        Decode the bytes object to a string using the specified *encoding*.

        MicroPython supports the following encodings:

        - ``'utf-8'`` or ``'utf8'`` - UTF-8 encoding (default)
        - ``'ascii'`` - ASCII encoding (subset of UTF-8)

        The *errors* parameter controls how decoding errors are handled:

        - ``'strict'`` - Raise a ``UnicodeError`` on invalid UTF-8 (default)
        - ``'ignore'`` - Skip invalid bytes (requires ``MICROPY_PY_BUILTINS_BYTES_DECODE_IGNORE``)
        - ``'replace'`` - Replace invalid bytes with U+FFFD � (requires ``MICROPY_PY_BUILTINS_BYTES_DECODE_REPLACE``)

        .. note::
            Error handler support depends on build configuration. On constrained
            systems, only ``'strict'`` mode may be available.

        Example::

            >>> b'\\xc2\\xa9 2024'.decode('utf-8')  # © symbol
            '© 2024'
            >>> b'hello\\xffworld'.decode('utf-8', 'ignore')  # Skip invalid bytes
            'helloworld'

        Raises ``LookupError`` if the encoding is not supported, or
        ``UnicodeError`` if the data contains invalid UTF-8 and ``errors='strict'``.

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

      .. note:: The optional ``signed`` kwarg from CPython is not supported.
                MicroPython currently converts negative integers as signed,
                and positive as unsigned. (:ref:`Details <cpydiff_types_int_to_bytes>`.)

.. function:: isinstance()

.. function:: issubclass()

.. function:: iter()

.. function:: len()

.. class:: list()

.. function:: locals()

.. function:: map()

.. function:: max()

.. class:: memoryview()

    |see_cpython| `python:memoryview`.

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

    .. method:: str.encode(encoding='utf-8')

        Encode the string to bytes using the specified *encoding*.

        MicroPython supports the following encodings:

        - ``'utf-8'`` or ``'utf8'`` - UTF-8 encoding (default)
        - ``'ascii'`` - ASCII encoding (subset of UTF-8)

        Example::

            >>> '© 2024'.encode('utf-8')  # Copyright symbol
            b'\\xc2\\xa9 2024'

        Raises ``LookupError`` if the encoding is not supported.

    .. method:: str.center(width)

        Return a centered string of length *width*. Padding is done using spaces.

        When Unicode support is enabled (``MICROPY_PY_BUILTINS_STR_UNICODE``), this
        method counts Unicode characters rather than bytes, ensuring proper alignment
        for multi-byte UTF-8 characters.

        Example::

            >>> 'café'.center(10)  # é is 2 bytes in UTF-8
            '   café   '

String Formatting
-----------------

MicroPython supports Unicode in string formatting when ``MICROPY_PY_BUILTINS_STR_UNICODE``
is enabled.

The ``%c`` and ``{:c}`` format specifiers accept Unicode codepoints in the range 0 to 0x10FFFF
(1,114,111) and properly encode multi-byte UTF-8 characters.

Example::

    >>> '%c' % 0x1F389    # 🎉 emoji
    '🎉'
    >>> '{:c}'.format(0x4E2D)  # 中 (Chinese character)
    '中'

Invalid character codes raise ``ValueError``::

    >>> '%c' % -1
    ValueError: %c arg not in range(0x110000)

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

   |see_cpython| `python:KeyboardInterrupt`.

   See also in the context of :ref:`soft_bricking`.

.. exception:: KeyError

.. exception:: MemoryError

.. exception:: NameError

.. exception:: NotImplementedError

.. exception:: OSError

.. exception:: RuntimeError

.. exception:: StopIteration

.. exception:: SyntaxError

.. exception:: SystemExit

    |see_cpython| `python:SystemExit`.

    On non-embedded ports (i.e. Windows and Unix), an unhandled ``SystemExit``
    exits the MicroPython process in a similar way to CPython.

    On embedded ports, an unhandled ``SystemExit`` currently causes a
    :ref:`soft_reset` of MicroPython.

.. exception:: TypeError

    |see_cpython| `python:TypeError`.

.. exception:: ValueError

.. exception:: ZeroDivisionError
