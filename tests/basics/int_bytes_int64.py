import sys

# Depending on the port, the numbers in this test may be implemented as "small"
# native 64 bit ints, arbitrary precision large ints, or large integers using 64-bit
# long longs.

try:
    x = int.from_bytes(b"\x6F\xAB\xCD\x12\x34\x56\x78\xFB", "big")
except OverflowError:
    print("SKIP")  # Port can't represent this size of integer at all
    raise SystemExit

print(hex(x))
b = x.to_bytes(8, "little")
print(b)
print(x.to_bytes(8, "big"))

# padding in output
print(x.to_bytes(20, "little"))
print(x.to_bytes(20, "big"))

# check that extra zero bytes don't change the internal int value
print(int.from_bytes(b + bytes(10), "little") == x)

# can't write to a zero-length bytes object
try:
    x.to_bytes(0, "little")
except OverflowError:
    print("OverflowError")

# or one that it too short
try:
    x.to_bytes(7, "big")
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


print(to_bytes_compat(-x, 8, "little"))
print(to_bytes_compat(-x, 20, "big"))
print(to_bytes_compat(-x, 20, "little"))
