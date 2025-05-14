# Make sure that write operations on io.BytesIO don't
# change original object it was constructed from.
import io
b = b"foobar"

a = io.BytesIO(b)
a.write(b"1")
print(b)
print(a.getvalue())

b = bytearray(b"foobar")

a = io.BytesIO(b)
a.write(b"1")
print(b)
print(a.getvalue())
