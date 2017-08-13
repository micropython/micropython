import uio as io

try:
    io.BytesIO
    io.BufferedWriter
except AttributeError:
    import sys
    print('SKIP')
    sys.exit()

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
