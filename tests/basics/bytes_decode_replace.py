# Test bytes.decode() with error handler 'replace'

# Check if decode method is available (requires MICROPY_CPYTHON_COMPAT)
try:
    b''.decode()
except AttributeError:
    print("SKIP")
    raise SystemExit

# Check if error handlers are available (requires MICROPY_PY_BUILTINS_BYTES_DECODE_REPLACE)
# When feature is disabled, invalid UTF-8 raises UnicodeError even with 'replace'
# When feature is enabled, invalid UTF-8 with 'replace' returns a string
try:
    result = b'\xff'.decode('utf-8', 'replace')
    # If we get here, feature is available
except (UnicodeError, LookupError):
    # Feature not available - 'replace' was ignored, strict mode was used
    print("SKIP")
    raise SystemExit

# Test replace mode with invalid UTF-8
print(repr(b'\xff\xfe'.decode('utf-8', 'replace')))

# Test strict mode (default) with invalid UTF-8
try:
    b'\xff\xfe'.decode('utf-8')
    print('UNEXPECTED')
except UnicodeError:
    print('UnicodeError')

# Test strict mode (explicit) with invalid UTF-8
try:
    b'\xff\xfe'.decode('utf-8', 'strict')
    print('UNEXPECTED')
except UnicodeError:
    print('UnicodeError')

# Test with valid UTF-8
print(repr(b'hello'.decode('utf-8', 'replace')))

# Test valid UTF-8 with default mode
print(repr(b'hello'.decode('utf-8')))

# Test mixed valid and invalid UTF-8
print(repr(b'hello\xffworld'.decode('utf-8', 'replace')))

# Test multiple invalid bytes
print(repr(b'\x80\x81\x82'.decode('utf-8', 'replace')))

# Test invalid continuation byte
print(repr(b'\xc0\x20'.decode('utf-8', 'replace')))

# Test incomplete sequence at end
print(repr(b'hello\xc0'.decode('utf-8', 'replace')))

# Test valid multi-byte UTF-8 (© symbol)
print(repr(b'\xc2\xa9'.decode('utf-8', 'replace')))

# Test bytearray support
print(repr(bytearray(b'\xff\xfe').decode('utf-8', 'replace')))

# Test replace mode - should either work or raise NotImplementedError
try:
    result = b'\xff\xfe'.decode('utf-8', 'replace')
    print(repr(result))
except LookupError:
    print('LookupError')

# Test replace with valid UTF-8
try:
    result = b'hello'.decode('utf-8', 'replace')
    print(repr(result))
except LookupError:
    print('LookupError')

# Test replace with mixed content
try:
    result = b'hello\xffworld'.decode('utf-8', 'replace')
    print(repr(result))
except LookupError:
    print('LookupError')

# Additional tests for continuation byte validation and incomplete sequences

# Test 3-byte UTF-8 sequence - valid (e.g., U+4E00 - 一)
print(repr(b'\xe4\xb8\x80'.decode('utf-8', 'replace')))

# Test 4-byte UTF-8 sequence - valid (e.g., U+1F600 - 😀)
print(repr(b'\xf0\x9f\x98\x80'.decode('utf-8', 'replace')))

# Test incomplete 3-byte sequence (missing 2 continuation bytes)
print(repr(b'\xe4'.decode('utf-8', 'replace')))

# Test incomplete 3-byte sequence (missing 1 continuation byte)
print(repr(b'\xe4\xb8'.decode('utf-8', 'replace')))

# Test incomplete 4-byte sequence (missing 3 continuation bytes)
print(repr(b'\xf0'.decode('utf-8', 'replace')))

# Test incomplete 4-byte sequence (missing 2 continuation bytes)
print(repr(b'\xf0\x9f'.decode('utf-8', 'replace')))

# Test incomplete 4-byte sequence (missing 1 continuation byte)
print(repr(b'\xf0\x9f\x98'.decode('utf-8', 'replace')))

# Test 3-byte sequence with invalid continuation byte (first byte invalid)
print(repr(b'\xe4\x20\x80'.decode('utf-8', 'replace')))

# Test 3-byte sequence with invalid continuation byte (second byte invalid)
print(repr(b'\xe4\xb8\x20'.decode('utf-8', 'replace')))

# Test 4-byte sequence with invalid continuation bytes
print(repr(b'\xf0\x20\x98\x80'.decode('utf-8', 'replace')))
print(repr(b'\xf0\x9f\x20\x80'.decode('utf-8', 'replace')))
print(repr(b'\xf0\x9f\x98\x20'.decode('utf-8', 'replace')))

# Test mixed valid and incomplete sequences
print(repr(b'hello\xe4world'.decode('utf-8', 'replace')))
print(repr(b'hello\xf0world'.decode('utf-8', 'replace')))

# Test multiple incomplete sequences in a row
print(repr(b'\xe4\xf0\xe4'.decode('utf-8', 'replace')))
