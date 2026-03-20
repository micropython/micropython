# Test bytes.decode() with error handlers

# Check if decode method is available (requires MICROPY_CPYTHON_COMPAT)
try:
    b''.decode()
except AttributeError:
    print("SKIP")
    raise SystemExit

# Check if error handlers are available (requires MICROPY_PY_BUILTINS_BYTES_DECODE_IGNORE)
# When feature is disabled, invalid UTF-8 raises UnicodeError even with 'ignore'
# When feature is enabled, invalid UTF-8 with 'ignore' returns a string
try:
    result = b'\xff'.decode('utf-8', 'ignore')
    # If we get here, feature is available
except UnicodeError:
    # Feature not available - 'ignore' was ignored, strict mode was used
    print("SKIP")
    raise SystemExit

# Test ignore mode with invalid UTF-8
print(repr(b'\xff\xfe'.decode('utf-8', 'ignore')))

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
print(repr(b'hello'.decode('utf-8', 'ignore')))

# Test valid UTF-8 with default mode
print(repr(b'hello'.decode('utf-8')))

# Test mixed valid and invalid UTF-8
print(repr(b'hello\xffworld'.decode('utf-8', 'ignore')))

# Test multiple invalid bytes
print(repr(b'\x80\x81\x82'.decode('utf-8', 'ignore')))

# Test invalid continuation byte
print(repr(b'\xc0\x20'.decode('utf-8', 'ignore')))

# Test incomplete sequence at end
print(repr(b'hello\xc0'.decode('utf-8', 'ignore')))

# Test valid multi-byte UTF-8 (© symbol)
print(repr(b'\xc2\xa9'.decode('utf-8', 'ignore')))

# Test bytearray support
print(repr(bytearray(b'\xff\xfe').decode('utf-8', 'ignore')))

# Test replace mode - should either work or raise NotImplementedError
try:
    result = b'\xff\xfe'.decode('utf-8', 'replace')
    print(repr(result))
except NotImplementedError:
    print('NotImplementedError')

# Test replace with valid UTF-8
try:
    result = b'hello'.decode('utf-8', 'replace')
    print(repr(result))
except NotImplementedError:
    print('NotImplementedError')

# Test replace with mixed content
try:
    result = b'hello\xffworld'.decode('utf-8', 'replace')
    print(repr(result))
except NotImplementedError:
    print('NotImplementedError')
