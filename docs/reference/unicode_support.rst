.. _unicode_support:

Unicode Support
===============

MicroPython provides Unicode support for strings, with the level of support
depending on the build configuration.

Character Encoding
------------------

MicroPython uses UTF-8 encoding for all strings. When Unicode support is enabled
(``MICROPY_PY_BUILTINS_STR_UNICODE``), strings can contain any valid Unicode
character from U+0000 to U+10FFFF.

ASCII characters (0-127) are stored in a single byte, making them as memory-efficient
as on systems without Unicode support. Multi-byte UTF-8 characters use 2-4 bytes
depending on the codepoint:

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
    b'\\xe6\\x97\\xa5\\xe6\\x9c\\xac\\xe8\\xaa\\x9e'
    >>> b'\\xe6\\x97\\xa5\\xe6\\x9c\\xac\\xe8\\xaa\\x9e'.decode('utf-8')
    '日本語'

Error Handling
~~~~~~~~~~~~~~

When decoding bytes that contain invalid UTF-8 sequences, the ``errors`` parameter
of :meth:`bytes.decode` controls the behavior:

- ``'strict'`` (default): Raise ``UnicodeError``
- ``'ignore'``: Skip invalid bytes (requires ``MICROPY_PY_BUILTINS_BYTES_DECODE_IGNORE``)
- ``'replace'``: Replace invalid bytes with U+FFFD � (requires ``MICROPY_PY_BUILTINS_BYTES_DECODE_REPLACE``)

Example::

    >>> # Strict mode (default) raises an error
    >>> b'hello\\xffworld'.decode('utf-8')
    UnicodeError: invalid UTF-8
    
    >>> # Ignore mode skips invalid bytes
    >>> b'hello\\xffworld'.decode('utf-8', 'ignore')
    'helloworld'
    
    >>> # Replace mode substitutes replacement character
    >>> b'hello\\xffworld'.decode('utf-8', 'replace')
    'hello�world'

For memory-conscious applications, consider using ``'ignore'`` mode when processing
untrusted or partially corrupted data, as it avoids raising exceptions while still
recovering valid text.

String Methods
--------------

When Unicode support is enabled, string methods operate on characters rather than bytes:

- :meth:`str.center` - Counts Unicode characters for width calculation
- ``len(s)`` - Returns number of characters (not bytes)
- String indexing and slicing work on character boundaries
- No support for display width calculations (East Asian width, combining characters, etc.)

Example::

    >>> s = 'Hello 世界'
    >>> len(s)           # 8 characters
    8
    >>> len(s.encode())  # 12 bytes
    12
    >>> s.center(12)     # Centered by character count
    '  Hello 世界  '

String Formatting
-----------------

The ``%c`` format specifier and ``{:c}`` format code support full Unicode:

- Accepts codepoints from 0 to 0x10FFFF
- Properly encodes multi-byte UTF-8 characters
- Raises ``ValueError`` for invalid codepoints

Example::

    >>> '%c' % 65           # ASCII
    'A'
    >>> '%c' % 0x03B1       # Greek α
    'α'
    >>> '%c' % 0x1F600      # Emoji 😀
    '😀'
    >>> '{:c}'.format(0x4E2D)  # Chinese 中
    '中'
    
    >>> # Invalid codepoint
    >>> '%c' % 0x110000
    ValueError: %c arg not in range(0x110000)

F-strings also support the ``:c`` format code::

    >>> codepoint = 0x2665  # Heart suit ♥
    >>> f'I {codepoint:c} Python'
    'I ♥ Python'

Build Configuration
-------------------

Unicode features are controlled by several build-time flags in ``mpconfigport.h``:

``MICROPY_PY_BUILTINS_STR_UNICODE``
    Enable Unicode string support. When enabled, strings can contain any valid
    Unicode character and string operations work on character boundaries rather
    than byte boundaries.

    Default: Enabled at ``MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES`` and above.

``MICROPY_PY_BUILTINS_STR_UNICODE_CHECK``
    Enable UTF-8 validation during string operations. When disabled, string
    operations may produce incorrect results with invalid UTF-8 sequences.

    Default: Follows ``MICROPY_PY_BUILTINS_STR_UNICODE`` setting.

``MICROPY_PY_BUILTINS_BYTES_DECODE_IGNORE``
    Enable the ``'ignore'`` error handler for :meth:`bytes.decode`. When enabled,
    invalid UTF-8 bytes can be skipped during decoding.

    Default: Enabled at ``MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES`` and above.

``MICROPY_PY_BUILTINS_BYTES_DECODE_REPLACE``
    Enable the ``'replace'`` error handler for :meth:`bytes.decode`. When enabled,
    invalid UTF-8 bytes are replaced with U+FFFD (�) during decoding.

    Note: Only has effect when ``MICROPY_PY_BUILTINS_BYTES_DECODE_IGNORE`` is enabled.

    Default: Follows ``MICROPY_PY_BUILTINS_BYTES_DECODE_IGNORE`` setting.

Example Configuration
~~~~~~~~~~~~~~~~~~~~~

For a constrained port with limited flash, disable error handlers::

    #define MICROPY_PY_BUILTINS_BYTES_DECODE_IGNORE (0)
    #define MICROPY_PY_BUILTINS_BYTES_DECODE_REPLACE (0)

For a port with more resources, enable all Unicode features::

    #define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)
    // This automatically enables:
    // - MICROPY_PY_BUILTINS_STR_UNICODE
    // - MICROPY_PY_BUILTINS_BYTES_DECODE_IGNORE
    // - MICROPY_PY_BUILTINS_BYTES_DECODE_REPLACE

Limitations
-----------

MicroPython's Unicode support has some limitations compared to CPython:

- Only UTF-8 and ASCII encodings are supported
- No support for Unicode normalization
- No locale-aware string operations
- The ``errors`` parameter accepts only positional arguments (not keyword arguments)
- String methods like ``upper()``, ``lower()``, etc. work correctly only for ASCII
- The MicroPython interactive REPL and ``input()`` function currently have very limited 
  Unicode support. 
  A possible workaround is to use utf-8 encoded python scripts containing the unicode
  text, and run them using ``mpremote run <script>``.
