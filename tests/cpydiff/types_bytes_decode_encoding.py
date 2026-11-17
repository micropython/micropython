"""
categories: Types,bytes
description: bytes.decode() only supports encoding arguments 'utf8', 'utf-8' and 'ascii'. Other encodings like 'latin-1' are not supported. Other string forms such as 'UTF8' are not supported.
cause: MicroPython is optimised for embedded systems and only includes UTF-8 and ASCII codec support with simple matching to save memory and code size. `The same restriction applies to str constructor <cpydiff_types_str_encoding>`. See also `unicode_support`.
workaround: Convert data to UTF-8 before processing, or implement custom encoding/decoding if needed. Ensure encoding argument is one of the accepted forms.
"""

# Both CPython and MicroPython support utf8 and ascii encodings
print(b"caf\xc3\xa9".decode("utf8"))  # codespell:ignore caf
print(b"cafe".decode("ascii"))

# MicroPython does not support additional encodings
try:
    b"\xe9".decode("latin-1")  # 'é' in latin-1
    print("latin-1 supported")
except (ValueError, NotImplementedError, LookupError) as e:
    print("latin-1 not supported:", type(e).__name__)

try:
    b"\x80".decode("cp1252")  # Euro sign in cp1252
    print("cp1252 supported")
except (ValueError, NotImplementedError, LookupError) as e:
    print("cp1252 not supported:", type(e).__name__)

# Encoding arguments must match exactly in MicroPython
try:
    b"hello".decode("ASCII")
    print("Capital letters ASCII supported")
except (ValueError, NotImplementedError, LookupError) as e:
    print("Capital letters ASCII not supported:", type(e).__name__)
