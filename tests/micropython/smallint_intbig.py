import struct
import array

zero = 0
i = struct.unpack("Q", b"\0" * 8)[0]
print(i is zero)
i = struct.unpack("q", b"\0" * 8)[0]
print(i is zero)

i = array.array("Q", [0])[0]
print(i is zero)
i = array.array("q", [0])[0]
print(i is zero)

i = pow(3, 3, 3)
print(i is zero)
