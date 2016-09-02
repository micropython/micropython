try:
    import zlib
except ImportError:
    import uzlib as zlib
import uio as io


# Raw DEFLATE bitstream
buf = io.BytesIO(b'\xcbH\xcd\xc9\xc9\x07\x00')
inp = zlib.DecompIO(buf)
print(buf.seek(0, 1))
print(inp.read(1))
print(buf.seek(0, 1))
print(inp.read(2))
print(inp.read())
print(buf.seek(0, 1))
print(inp.read(1))
print(inp.read())
print(buf.seek(0, 1))
