try:
    # Check if gzip is available.
    import gzip
    import io
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    # Check if compression is enabled.
    b = io.BytesIO()
    g = gzip.GzipFile(fileobj=b, mode="wb")
except ValueError:
    print("SKIP")
    raise SystemExit

# Simple compression & decompression.
b = io.BytesIO()
g = gzip.GzipFile(fileobj=b, mode="wb")
for i in range(10):
    g.write(b"micropython")
g.close()
result = b.getvalue()

b = io.BytesIO(result)
g = gzip.GzipFile(fileobj=b, mode="rb")
print(g.read())

# Same, but using a context manager.
b = io.BytesIO()
with gzip.GzipFile(fileobj=b, mode="wb") as g:
    for i in range(10):
        g.write(b"micropython")
print(b.getvalue() == result)

# Writing to closed stream (or a stream that fails the write).
b = io.BytesIO()
g = gzip.GzipFile(fileobj=b, mode="wb")
g.write(b"micropython")
b.close()
try:
    g.write(b"micropython")
except (OSError, ValueError):
    print("OSError")

# Write to a read-mode GzipFile.
data = b'\x1f\x8b\x08\x00\xa2\xb2\x99d\x02\xff\xcb\xcdL.\xca/\xa8,\xc9\xc8\xcfS\xc8H\xcd\xc9\xc9W(\xcf/\xcaIAa\xe7"\xd4\x00\x00"\xeb\xc4\x98/\x00\x00\x00'
buf = io.BytesIO(data)
inp = gzip.GzipFile(fileobj=buf, mode="rb")
try:
    inp.write(b"hello")
except OSError:
    print("OSError")

# Read from a write-mode GzipFile.
buf = io.BytesIO()
inp = gzip.GzipFile(fileobj=buf, mode="wb")
try:
    inp.read(3)
except OSError:
    print("OSError")
