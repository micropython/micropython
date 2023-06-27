# Test MicroPython-specific extensions to GzipFile.

try:
    # Check if gzip is available.
    import gzip
    import io
except ImportError:
    print("SKIP")
    raise SystemExit

# zlib.compress(b'micropython hello world hello world micropython', wbits=9)
data = b'\x18\x95\xcb\xcdL.\xca/\xa8,\xc9\xc8\xcfS\xc8H\xcd\xc9\xc9W(\xcf/\xcaIAa\xe7"\xd4\x00\x00\xbc\xfa\x12\x91'

# hist buf exactly the right size for wbits.
buf = io.BytesIO(data)
inp = gzip.GzipFile(fileobj=buf, mode="rb", wbits=9, hist=bytearray(1 << 9))
print(inp.read())

# hist buf bigger than necessary for wbits.
buf = io.BytesIO(data)
inp = gzip.GzipFile(fileobj=buf, mode="rb", wbits=9, hist=bytearray((1 << 9) + 22))
print(inp.read())

# hist buf too small for wbits.
buf = io.BytesIO(data)
try:
    inp = gzip.GzipFile(fileobj=buf, mode="rb", wbits=9, hist=bytearray((1 << 9) - 22))
except ValueError:
    print("ValueError")

# Window too small for this input.
try:
    buf = io.BytesIO(data)
    inp = gzip.GzipFile(fileobj=buf, mode="rb", wbits=5)
    print(inp.read())
except OSError:
    print("OSError")

# Minimum wbits for this input.
buf = io.BytesIO(data)
inp = gzip.GzipFile(fileobj=buf, mode="rb", wbits=6, hist=bytearray(64))
print(inp.read())

# Minimum wbits for this input.
buf = io.BytesIO(data)
try:
    inp = gzip.GzipFile(fileobj=buf, mode="rb", wbits=6, hist=bytearray(37))
except ValueError:
    print("ValueError")

# Correct size but not writeable.
buf = io.BytesIO(data)
try:
    inp = gzip.GzipFile(fileobj=buf, mode="rb", wbits=6, hist=bytes(64))
except TypeError:
    print("TypeError")

# Default value for hist kwarg.
buf = io.BytesIO(data)
inp = gzip.GzipFile(fileobj=buf, mode="rb", wbits=6, hist=None)
print(inp.read())

# Invalid mode.
buf = io.BytesIO()
try:
    gzip.GzipFile(fileobj=buf, mode="abc")
except ValueError:
    print("ValueError")
