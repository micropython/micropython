import sys

print((2**64).to_bytes(9, "little"))
print((-(2**64)).to_bytes(9, "little", signed=True))
print((2**64).to_bytes(9, "big"))
print((-(2**64)).to_bytes(9, "big", signed=True))

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

# or one that is too short
try:
    ib.to_bytes(18, "big")
except OverflowError:
    print("OverflowError")

# including when signed
try:
    ib.to_bytes(18, "big", signed=True)
except OverflowError:
    print("OverflowError")

# negative representations

# negative numbers should raise an error if signed=False
try:
    (-(2**64)).to_bytes(9, "little", signed=False)
except OverflowError:
    print("OverflowError")


print((-ib).to_bytes(20, "big", signed=True))
print((ib * -ib).to_bytes(40, "big", signed=True))

# cases where an additional byte is needed for sign bit

MAX_U24 = (2**24) - 1
MAX_U32 = (2**32) - 1
MAX_U64 = (2**64) - 1

for ib, nbytes in (
    (-127, 1),
    (255, 1),
    (-255, 1),
    (65535, 2),
    (-65535, 2),
    (-65534, 2),
    (MAX_U24, 3),
    (-MAX_U24, 3),
    (1 - MAX_U24, 3),
    (MAX_U32, 4),
    (-MAX_U32, 4),
    (1 - MAX_U32, 4),
    (2 - MAX_U32, 4),
    (MAX_U64, 8),
    (-MAX_U64, 8),
    (1 - MAX_U64, 8),
    (2 - MAX_U64, 8),
    (2 * MAX_U64, 8),
    (-2 * MAX_U64, 8),
):
    print("ib", hex(ib), "nbytes", nbytes, ":")
    for signed in False, True:
        for endian in "little", "big":
            for nbytes_offs in (-1, 0, 1):
                try:
                    as_bytes = ib.to_bytes(nbytes + nbytes_offs, endian, signed=signed)
                except OverflowError:
                    as_bytes = "OverflowError"
                print(as_bytes, "signed", signed, "endian", endian, "nbytes_offs", nbytes_offs)
