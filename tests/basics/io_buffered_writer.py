try:
    import io

    io.BytesIO
    io.BufferedWriter
except (AttributeError, ImportError):
    print('SKIP')
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

# hashing a BufferedWriter
print(type(hash(buf)))

# Test failing flush()
class MyIO(io.IOBase):
    def __init__(self):
        self.count = 0

    def write(self, buf):
        self.count += 1
        if self.count < 3:
            return None
        print("writing", buf)
        return len(buf)


buf = io.BufferedWriter(MyIO(), 8)

buf.write(b"foobar")

for _ in range(4):
    try:
        buf.flush()
        print("flushed")
    except OSError:
        print("OSError")
