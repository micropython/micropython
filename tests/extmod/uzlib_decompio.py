try:
    import uzlib as zlib
    import uio as io
except ImportError:
    print("SKIP")
    raise SystemExit


# Raw DEFLATE bitstream
buf = io.BytesIO(b"\xcbH\xcd\xc9\xc9\x07\x00")
inp = zlib.DecompIO(buf, -8)
print(buf.seek(0, 1))
print(inp.read(1))
print(buf.seek(0, 1))
print(inp.read(2))
print(inp.read())
print(buf.seek(0, 1))
print(inp.read(1))
print(inp.read())
print(buf.seek(0, 1))


# zlib bitstream (with 256 byte window size)
inp = zlib.DecompIO(io.BytesIO(b"\x08\x9930\xa0=\x00\x00\xb3q\x12\xc1"))
print(inp.read(10))
print(inp.read())

# zlib bitstream, wrong checksum
inp = zlib.DecompIO(io.BytesIO(b"\x08\x9930\xa0=\x00\x00\xb3q\x12\xc0"))
try:
    print(inp.read())
except OSError as e:
    print(repr(e))

inp = zlib.DecompIO(io.BytesIO(b"x\x9c+.)JM\xcc5\x04\x00\x0b\xe0\x02\xbe"))
assert inp.read() == b"stream1"
inp.set_stream(io.BytesIO(b"x\x9c+.)JM\xcc5\x02\x00\x0b\xe1\x02\xbf"))
assert inp.read() == b"stream2"
