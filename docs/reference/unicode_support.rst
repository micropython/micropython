.. _unicode_support:

Unicode Support
===============

MicroPython provides Unicode support for strings.  All Tier 1, 2 and 3 ports
have Unicode support enabled by default, but it is possible to change that with
a different build configuration.

Terminology
-----------

This document uses the following Unicode terms:

- **Code point**: a single Unicode value in the range U+0000 to U+10FFFF, for
  example U+0041 ``A`` or U+1F600 😀.  MicroPython strings are sequences of
  code points.
- **Character**: informally used to mean a code point.  Be aware that a
  user-perceived character (a *grapheme*) may consist of several code points,
  such as a base letter followed by combining marks.
- **Byte**: a single 8-bit value.  In UTF-8 each code point is stored as one to
  four bytes (see below).

Operations such as ``len()``, indexing and slicing act on code points, not on
graphemes or display width, so a base letter followed by a combining mark counts
as two code points.

Character Encoding
------------------

MicroPython uses UTF-8 encoding for all strings. When Unicode support is enabled
(``MICROPY_PY_BUILTINS_STR_UNICODE``), strings can contain any valid Unicode code
point from U+0000 to U+10FFFF.

ASCII characters (0-127) are stored in a single byte, making them as memory-efficient
as on systems without Unicode support. Multi-byte UTF-8 code points use 2-4 bytes
depending on the code point:

- U+0000 to U+007F: 1 byte (ASCII)
- U+0080 to U+07FF: 2 bytes
- U+0800 to U+FFFF: 3 bytes
- U+10000 to U+10FFFF: 4 bytes

Encoding and Decoding
----------------------

The :meth:`bytes.decode` and :meth:`str.encode` methods support the following encodings:

- UTF-8 (``'utf-8'`` or ``'utf8'``)
- ASCII (``'ascii'``)

Other encodings (such as ``'latin-1'``, ``'utf-16'``, etc.) are not supported and
will raise ``LookupError``.

Example::

    >>> '日本語'.encode('utf-8')
    b'\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e'
    >>> b'\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e'.decode('utf-8')
    '日本語'

Error Handling
~~~~~~~~~~~~~~

When decoding bytes that contain invalid UTF-8 sequences, the ``errors`` parameter
of :meth:`bytes.decode` controls the behavior:

- ``'strict'`` (default): Raise ``UnicodeError``
- ``'ignore'``: Skip invalid bytes (requires ``MICROPY_PY_BUILTINS_BYTES_DECODE_ERRORS``)
- ``'replace'``: Replace invalid bytes with U+FFFD � (requires ``MICROPY_PY_BUILTINS_BYTES_DECODE_ERRORS``)

Example::

    >>> # Strict mode (default) raises an error
    >>> b'hello\xffworld'.decode('utf-8')
    UnicodeError: invalid UTF-8

    >>> # Ignore mode skips invalid bytes
    >>> b'hello\xffworld'.decode('utf-8', 'ignore')
    'helloworld'

    >>> # Replace mode substitutes replacement character
    >>> b'hello\xffworld'.decode('utf-8', 'replace')
    'hello�world'

For memory-conscious applications, consider using ``'ignore'`` mode when processing
untrusted or partially corrupted data, as it avoids raising exceptions while still
recovering valid text.

The same ``errors`` handling applies when decoding any bytes-like object, including
via the ``str()`` constructor (for example ``str(buf, 'utf-8', 'replace')`` where
``buf`` is a ``bytes``, ``bytearray``, ``memoryview`` or ``array`` object).


String Methods
--------------

When Unicode support is enabled, string methods operate on code points rather than bytes:

- :meth:`str.center` - Counts code points for width calculation
- ``len(s)`` - Returns number of code points (not bytes)
- String indexing and slicing work on code-point boundaries
- No support for display width calculations (East Asian width, combining characters, etc.)

Example::

    >>> s = 'Hello 世界'
    >>> len(s)           # 8 code points
    8
    >>> len(s.encode())  # 12 bytes
    12
    >>> s.center(12)     # Centered by code-point count
    '  Hello 世界  '

String Formatting
-----------------

The ``%c`` format specifier and ``{:c}`` format code support full Unicode:

- Accepts code points from 0 to 0x10FFFF
- Properly encodes multi-byte UTF-8 code points
- Raises ``ValueError`` for invalid code points

Example::

    >>> '%c' % 65           # ASCII
    'A'
    >>> '%c' % 0x03B1       # Greek α
    'α'
    >>> '%c' % 0x1F600      # Emoji 😀
    '😀'
    >>> '{:c}'.format(0x4E2D)  # Chinese 中
    '中'

    >>> # Invalid code point
    >>> '%c' % 0x110000
    ValueError: %c arg not in range(0x110000)

F-strings also support the ``:c`` format code::

    >>> code_point = 0x2665  # Heart suit ♥
    >>> f'I {code_point:c} Python'
    'I ♥ Python'

Build Configuration
-------------------

Unicode features are controlled by several build-time flags in ``mpconfigport.h``:

``MICROPY_PY_BUILTINS_STR_UNICODE``
    Enable Unicode string support. When enabled, strings can contain any valid
    Unicode character and string operations work on character boundaries rather
    than byte boundaries.

    Default: Enabled at ``MICROPY_CONFIG_ROM_LEVEL_BASIC_FEATURES`` and above.

    Enabled on all Tier 1, 2 and 3 ports.

``MICROPY_PY_BUILTINS_STR_UNICODE_CHECK``
    Enable UTF-8 validation during string operations. When disabled, string
    operations may produce incorrect results with invalid UTF-8 sequences.

    Default: Follows ``MICROPY_PY_BUILTINS_STR_UNICODE`` setting.

    Enabled on all Tier 1, 2 and 3 ports.

``MICROPY_PY_BUILTINS_BYTES_DECODE_ERRORS``
    Enable the ``'ignore'`` and ``'replace'`` error handlers for
    :meth:`bytes.decode`. When enabled, invalid UTF-8 bytes can be either
    skipped (``'ignore'``) or replaced with U+FFFD (``'replace'``).

    Default: Enabled at ``MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES`` and above.

    Enabled on alif, esp32, esp8266, mimxrt, renesas-ra, rp2, samd (SAMD51 only),
    stm32, unix and webassembly ports.

Example Configuration
~~~~~~~~~~~~~~~~~~~~~

For a constrained port with limited flash, disable error handlers::

    #define MICROPY_PY_BUILTINS_BYTES_DECODE_ERRORS (0)

For a port with more resources, enable all Unicode features::

    #define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)
    // This automatically enables:
    // - MICROPY_PY_BUILTINS_STR_UNICODE
    // - MICROPY_PY_BUILTINS_BYTES_DECODE_ERRORS

Limitations
-----------

MicroPython's Unicode support has some limitations compared to CPython:

- Only UTF-8 and ASCII encodings are supported
- No support for Unicode normalization
- No locale-aware string operations
- The ``errors`` parameter accepts only positional arguments (not keyword arguments)
- String methods like ``upper()``, ``lower()``, etc. work correctly only for ASCII
- The MicroPython interactive REPL and ``input()`` function currently have limited
  Unicode support.  The line editor is unaware of the displayed width of characters:
  wide characters (for example many CJK characters) take two terminal columns, while a
  grapheme cluster (a base code point plus combining marks, or an emoji sequence) can
  span several code points yet occupy a single column.  Because editing tracks code
  points rather than displayed columns, line-editing keys such as backspace and the
  left/right arrows may leave the cursor misaligned with the text shown on screen.
  A workaround is to place the Unicode text in a UTF-8 encoded MicroPython script and
  run it using ``mpremote run <script.py>``.
