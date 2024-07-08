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

# MicroPython int.to_bytes() behaves as if signed=True for negative numbers
if "micropython" in repr(sys.implementation):

    def to_bytes_compat(i, l, e):
        return i.to_bytes(l, e)
else:
    # Implement MicroPython compatible behaviour for CPython
    def to_bytes_compat(i, l, e):
        return i.to_bytes(l, e, signed=i < 0)


print(to_bytes_compat(-1, 1, "little"))
print(to_bytes_compat(-1, 3, "little"))
print(to_bytes_compat(-1, 1, "big"))
print(to_bytes_compat(-1, 3, "big"))
print(to_bytes_compat(-128, 1, "big"))
print(to_bytes_compat(-32768, 2, "big"))
print(to_bytes_compat(-(1 << 23), 3, "big"))

try:
    print(to_bytes_compat(-129, 1, "big"))
except OverflowError:
    print("OverflowError")

try:
    print(to_bytes_compat(-32769, 2, "big"))
except OverflowError:
    print("OverflowError")

try:
    print(to_bytes_compat(-(1 << 23) - 1, 2, "big"))
except OverflowError:
    print("OverflowError")
