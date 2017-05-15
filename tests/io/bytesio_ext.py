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

# negative seek should limit offset to 0
a = io.BytesIO()
a.seek(10)
if a.seek(-100, 1) != 0:
	print('Negative seek allowed, now at', a.seek(0, 1))
else:
    a.write(b'123')
    print(a.getvalue())
