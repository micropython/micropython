"""
categories: Types,bytes
description: bytes.decode() only supports 'utf-8' and 'ascii' encodings, not other encodings like 'latin-1'
cause: MicroPython is optimized for embedded systems and only includes UTF-8 and ASCII codec support to save memory. Other encodings would require additional codec tables.
workaround: Convert data to UTF-8 before processing, or implement custom encoding/decoding if needed.
"""

# CPython supports many encodings, MicroPython only utf-8 and ascii
try:
    b'\xe9'.decode('latin-1')  # 'é' in latin-1
    print("latin-1 supported")
except (ValueError, NotImplementedError, LookupError) as e:
    print("latin-1 not supported:", type(e).__name__)

try:
    b'\x80'.decode('cp1252')  # Euro sign in cp1252
    print("cp1252 supported")
except (ValueError, NotImplementedError, LookupError) as e:
    print("cp1252 not supported:", type(e).__name__)
