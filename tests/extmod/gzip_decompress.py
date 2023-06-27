try:
    # Check if gzip is available.
    import gzip
    import io
except ImportError:
    print("SKIP")
    raise SystemExit

# gzip.compress(b'micropython hello world hello world micropython')
data = b'\x1f\x8b\x08\x00\xa2\xb2\x99d\x02\xff\xcb\xcdL.\xca/\xa8,\xc9\xc8\xcfS\xc8H\xcd\xc9\xc9W(\xcf/\xcaIAa\xe7"\xd4\x00\x00"\xeb\xc4\x98/\x00\x00\x00'

buf = io.BytesIO(data)
inp = gzip.GzipFile(fileobj=buf, mode="rb")
print(inp.read(1))
print(inp.read(1))
print(inp.read(2))
print(inp.read())
print(inp.read(1))
print(inp.read())

# Invalid compression algorithm.
buf = io.BytesIO(b"\x1f\x8b\x06\x00\xa2\xb2\x99d\x02\xff\xcb\xcdL.\xca/\xa8,\xc9\xc8\xcf")
try:
    inp = gzip.GzipFile(fileobj=buf, mode="rb")
    print(inp.read(1))
except OSError:
    print("OSError")

# Not a gzip file.
buf = io.BytesIO(b"\xaa\xbb\x06\x00\xa2\xb2\x99d\x02\xff\xcb\xcdL.\xca/\xa8,\xc9\xc8\xcf")
try:
    inp = gzip.GzipFile(fileobj=buf, mode="rb")
    print(inp.read(1))
except OSError:
    print("OSError")

# gzip.compress(b'micropython') (with checksum modified)
buf = io.BytesIO(
    b"\x1f\x8b\x08\x00\xca\xb6\x99d\x02\xff\xcb\xcdL.\xca/\xa8,\xc9\xc8\xcf\x03\x00AAAA\x0b\x00\x00\x00"
)
inp = gzip.GzipFile(fileobj=buf, mode="rb")
try:
    print(inp.read())
except OSError:
    print("OSError")

# Currently unsupported by uzlib -- see the end of uzlib_uncompress_chksum().
# # gzip.compress(b'micropython') (with length modified)
# buf = io.BytesIO(b'\x1f\x8b\x08\x00\xca\xb6\x99d\x02\xff\xcb\xcdL.\xca/\xa8,\xc9\xc8\xcf\x03\x00\xf2KF>\xaa\x00\x00\x00')
# inp = gzip.GzipFile(fileobj=buf, mode="rb")
# try:
#     print(inp.read())
# except OSError:
#     print("OSError")

# Reading from closed stream (or a stream that fails the read).
buf = io.BytesIO()
buf.close()
try:
    inp = gzip.GzipFile(fileobj=buf, mode="rb")
    inp.read(3)
except (OSError, ValueError):
    print("OSError")

# Reading from closed stream (or a stream that fails the read).
buf = io.BytesIO(b"\x1f\x8b")
try:
    inp = gzip.GzipFile(fileobj=buf, mode="rb")
    inp.read(3)
except EOFError:
    print("EOFError")

# Gzip header with extra flags (FCOMMENT FNAME FEXTRA FHCRC) enabled.
buf = io.BytesIO(
    b"\x1f\x8b\x08\x1e}\x9a\x9bd\x02\x00\x00\x00\x00\x00\x00\xff\xcb\xcdL.\xca/\xa8,\xc9\xc8\xcf\x03\x00\xf2KF>\x0b\x00\x00\x00"
)
inp = gzip.GzipFile(fileobj=buf, mode="rb")
print(inp.read())
