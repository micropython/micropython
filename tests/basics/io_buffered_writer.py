import uio as io

try:
    io.BytesIO
    io.BufferedWriter
except AttributeError:
    print("SKIP")
    raise SystemExit

bts = io.BytesIO()
buf = io.BufferedWriter(bts, 8)

buf.write(b"foobar")
print(bts.getvalue())
buf.write(b"foobar")
# CPython has different flushing policy, so value below is different
print(bts.getvalue())
buf.flush()
print(bts.getvalue())
buf.flush()
print(bts.getvalue())

# special case when alloc is a factor of total buffer length
bts = io.BytesIO()
buf = io.BufferedWriter(bts, 1)
buf.write(b"foo")
print(bts.getvalue())
