print(b"1" == 1)
print(b"123" == bytearray(b"123"))
print(b'123' < bytearray(b"124"))
print(b'123' > bytearray(b"122"))
print(bytearray(b"23") in b"1234")

import array

print(array.array('b', [1, 2]) in b'\x01\x02\x03')
# CPython gives False here
#print(b"\x01\x02\x03" == array.array("B", [1, 2, 3]))
