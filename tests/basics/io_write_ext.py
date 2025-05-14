# This tests extended (MicroPython-specific) form of write:
# write(buf, len) and write(buf, offset, len)
import io

try:
    io.BytesIO
except AttributeError:
    print('SKIP')
    raise SystemExit

buf = io.BytesIO()

buf.write(b"foo", 2)
print(buf.getvalue())

buf.write(b"foo", 100)
print(buf.getvalue())

buf.write(b"foobar", 1, 3)
print(buf.getvalue())

buf.write(b"foobar", 1, 100)
print(buf.getvalue())

buf.write(b"foobar", 100, 100)
print(buf.getvalue())
