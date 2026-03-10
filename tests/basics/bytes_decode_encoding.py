# Test bytes.decode() and str.encode() with encoding parameter validation

# Check if decode method is available (requires MICROPY_CPYTHON_COMPAT)
try:
    b''.decode()
except AttributeError:
    print("SKIP")
    raise SystemExit

# Test valid encodings for bytes.decode()
# utf-8 (default)
print(b'hello'.decode('utf-8'))
print(b'hello'.decode('utf8'))

# ascii (subset of utf-8)
print(b'hello'.decode('ascii'))

# Test valid encoding for str.encode()
print('hello'.encode('utf-8'))
print('hello'.encode('utf8'))
print('hello'.encode('ascii'))

# Test with bytearray
print(bytearray(b'test').decode('utf-8'))

# Test that UTF-8 still works correctly with non-ASCII characters
# © symbol (U+00A9)
print(b'\xc2\xa9'.decode('utf-8'))
print('©'.encode('utf-8'))

# Test emoji 👍 (U+1F44D) 
print(b'\xf0\x9f\x91\x8d'.decode('utf-8'))
print('👍'.encode('utf-8'))

# Test invalid encodings for bytes.decode()
# These should raise LookupError
invalid_encodings = ['latin-1', 'latin1', 'utf-16', 'utf-32', 'iso-8859-1', 'cp1252']

for encoding in invalid_encodings:
    try:
        b'hello'.decode(encoding)
        print('UNEXPECTED:', encoding, 'should raise LookupError')
    except LookupError as e:
        print('LookupError:', encoding)

# Test invalid encodings for str.encode()
for encoding in invalid_encodings:
    try:
        'hello'.encode(encoding)
        print('UNEXPECTED:', encoding, 'should raise LookupError')
    except LookupError as e:
        print('LookupError:', encoding)
