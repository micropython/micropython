import sys

print((10).to_bytes(1, "little"))
print((111111).to_bytes(4, "little"))
print((100).to_bytes(10, "little"))
print(int.from_bytes(b"\x00\x01\0\0\0\0\0\0", "little"))
print(int.from_bytes(b"\x01\0\0\0\0\0\0\0", "little"))
print(int.from_bytes(b"\x00\x01\0\0\0\0\0\0", "little"))

# check that extra zero bytes don't change the internal int value
print(int.from_bytes(bytes(20), "little") == 0)
print(int.from_bytes(b"\x01" + bytes(20), "little") == 1)

# big-endian conversion
print((10).to_bytes(1, "big"))
print((100).to_bytes(10, "big"))
print(int.from_bytes(b"\0\0\0\0\0\0\0\0\0\x01", "big"))
print(int.from_bytes(b"\x01\0", "big"))

# negative number of bytes should raise an error
try:
    (1).to_bytes(-1, "little")
except ValueError:
    print("ValueError")

# zero byte destination should also raise an error
try:
    (1).to_bytes(0, "little")
except OverflowError:
    print("OverflowError")

# except for converting 0 to a zero-length byte array
print((0).to_bytes(0, "big"))

# byte length can fit the integer directly
print((0xFF).to_bytes(1, "little"))
print((0xFF).to_bytes(1, "big"))
print((0xEFF).to_bytes(2, "little"))
print((0xEFF).to_bytes(2, "big"))
print((0xCDEFF).to_bytes(3, "little"))
print((0xCDEFF).to_bytes(3, "big"))

# OverFlowError if not big enough

try:
    (0x123).to_bytes(1, "big")
except OverflowError:
    print("OverflowError")

try:
    (0x12345).to_bytes(2, "big")
except OverflowError:
    print("OverflowError")

try:
    (0x1234567).to_bytes(3, "big")
except OverflowError:
    print("OverflowError")


# negative representations

print((-1).to_bytes(1, "little", signed=True))
print((-1).to_bytes(3, "little", signed=True))
print((-1).to_bytes(1, "big", signed=True))
print((-1).to_bytes(3, "big", signed=True))
print((-128).to_bytes(1, "big", signed=True))
print((-32768).to_bytes(2, "big", signed=True))
print((-(1 << 23)).to_bytes(3, "big", signed=True))

try:
    print((-129).to_bytes(1, "big", signed=True))
except OverflowError:
    print("OverflowError")

try:
    print((-32769).to_bytes(2, "big", signed=True))
except OverflowError:
    print("OverflowError")

try:
    print(((-1 << 23) - 1).to_bytes(2, "big", signed=True))
except OverflowError:
    print("OverflowError")
