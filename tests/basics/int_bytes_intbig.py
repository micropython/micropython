import sys

print((2**64).to_bytes(9, "little"))
print((2**64).to_bytes(9, "big"))

b = bytes(range(20))

il = int.from_bytes(b, "little")
ib = int.from_bytes(b, "big")
print(il)
print(ib)
print(il.to_bytes(20, "little"))
print(ib.to_bytes(20, "big"))

# check padding comes out correctly
print(il.to_bytes(40, "little"))
print(ib.to_bytes(40, "big"))

# check that extra zero bytes don't change the internal int value
print(int.from_bytes(b + bytes(10), "little") == int.from_bytes(b, "little"))

# can't write to a zero-length bytes object
try:
    ib.to_bytes(0, "little")
except OverflowError:
    print("OverflowError")

# or one that it too short
try:
    ib.to_bytes(18, "big")
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


print(to_bytes_compat(-ib, 20, "big"))
print(to_bytes_compat(ib * -ib, 40, "big"))

# case where an additional byte is needed for sign bit
ib = (2**64) - 1
print(ib.to_bytes(8, "little"))

ib *= -1

try:
    print(to_bytes_compat(ib, 8, "little"))
except OverflowError:
    print("OverflowError")

print(to_bytes_compat(ib, 9, "little"))
print(to_bytes_compat(ib, 9, "big"))
