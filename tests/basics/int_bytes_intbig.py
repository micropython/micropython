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

print((-ib).to_bytes(20, "big", signed=True))
print((ib * -ib).to_bytes(40, "big", signed=True))

# case where an additional byte is needed for sign bit
ib = (2**64) - 1
print(ib.to_bytes(8, "little"))

ib *= -1

try:
    print((ib).to_bytes(8, "little", signed=True))
except OverflowError:
    print("OverflowError")

print((ib).to_bytes(9, "little", signed=True))
print((ib).to_bytes(9, "big", signed=True))
