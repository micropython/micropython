# Extended stream operations on io.BytesIO
try:
    import uio as io
except ImportError:
    import io

a = io.BytesIO()
print(a.seek(8))
a.write(b"123")
print(a.getvalue())

print(a.seek(0, 1))

print(a.seek(-1, 2))
a.write(b"0")
print(a.getvalue())

a.flush()
print(a.getvalue())

a.seek(0)
arr = bytearray(10)
print(a.readinto(arr))
print(arr)
