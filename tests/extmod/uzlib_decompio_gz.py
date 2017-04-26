try:
    import uzlib as zlib
    import uio as io
except ImportError:
    import sys
    print("SKIP")
    sys.exit()


# gzip bitstream
buf = io.BytesIO(b'\x1f\x8b\x08\x08\x99\x0c\xe5W\x00\x03hello\x00\xcbH\xcd\xc9\xc9\x07\x00\x86\xa6\x106\x05\x00\x00\x00')
inp = zlib.DecompIO(buf, 16 + 8)
print(buf.seek(0, 1))
print(inp.read(1))
print(buf.seek(0, 1))
print(inp.read(2))
print(inp.read())
print(buf.seek(0, 1))
print(inp.read(1))
print(inp.read())
print(buf.seek(0, 1))

# Check FHCRC field
buf = io.BytesIO(b'\x1f\x8b\x08\x02\x99\x0c\xe5W\x00\x03\x00\x00\xcbH\xcd\xc9\xc9\x07\x00\x86\xa6\x106\x05\x00\x00\x00')
inp = zlib.DecompIO(buf, 16 + 8)
print(inp.read())

# Check FEXTRA field
buf = io.BytesIO(b'\x1f\x8b\x08\x04\x99\x0c\xe5W\x00\x03\x01\x00X\xcbH\xcd\xc9\xc9\x07\x00\x86\xa6\x106\x05\x00\x00\x00')
inp = zlib.DecompIO(buf, 16 + 8)
print(inp.read())

# broken header
buf = io.BytesIO(b'\x1f\x8c\x08\x08\x99\x0c\xe5W\x00\x03hello\x00\xcbH\xcd\xc9\xc9\x07\x00\x86\xa6\x106\x05\x00\x00\x00')
try:
    inp = zlib.DecompIO(buf, 16 + 8)
except ValueError:
    print("ValueError")

# broken crc32
buf = io.BytesIO(b'\x1f\x8b\x08\x08\x99\x0c\xe5W\x00\x03hello\x00\xcbH\xcd\xc9\xc9\x07\x00\x86\xa7\x106\x05\x00\x00\x00')
inp = zlib.DecompIO(buf, 16 + 8)
try:
    inp.read(6)
except OSError as e:
    print(repr(e))

# broken uncompressed size - not checked so far
#buf = io.BytesIO(b'\x1f\x8b\x08\x08\x99\x0c\xe5W\x00\x03hello\x00\xcbH\xcd\xc9\xc9\x07\x00\x86\xa6\x106\x06\x00\x00\x00')
#inp = zlib.DecompIO(buf, 16 + 8)
#inp.read(6)
