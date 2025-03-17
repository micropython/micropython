# Test deflate.DeflateIO compression, with out-of-memory errors.

try:
    # Check if deflate is available.
    import deflate
    import io
except ImportError:
    print("SKIP")
    raise SystemExit

# Check if compression is enabled.
if not hasattr(deflate.DeflateIO, "write"):
    print("SKIP")
    raise SystemExit

# Create a compressor object.
b = io.BytesIO()
g = deflate.DeflateIO(b, deflate.RAW, 15)

# Then, use up most of the heap.
l = []
while True:
    try:
        l.append(bytearray(1000))
    except:
        break
l.pop()

# Try to compress.  This will try to allocate a large window and fail.
try:
    g.write('test')
except MemoryError:
    print("MemoryError")

# Should still be able to close the stream.
g.close()

# The underlying output stream should be unchanged.
print(b.getvalue())
