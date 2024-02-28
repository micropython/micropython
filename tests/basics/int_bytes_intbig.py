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

if sys.implementation.name == "micropython":
    # MicroPython int.to_bytes behaves as if signed=True, always
    def to_bytes_signed(i, l, e):
        return i.to_bytes(l, e)
else:

    def to_bytes_signed(i, l, e):
        return i.to_bytes(l, e, signed=True)


print(to_bytes_signed(-ib, 20, "big"))
