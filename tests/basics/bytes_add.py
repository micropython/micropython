# test bytes + other

print(b"123" + b"456")
print(b"123" + bytearray(2))

import array

print(b"123" + array.array('i', [1]))
print(b"\x01\x02" + array.array('b', [1, 2]))
