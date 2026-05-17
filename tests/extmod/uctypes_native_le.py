# This test is exactly like uctypes_le.py, but uses native structure layout.
# Codepaths for packed vs native structures are different. This test only works
# on little-endian machine (no matter if 32 or 64 bit).
import sys

try:
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

if sys.byteorder != "little":
    print("SKIP")
    raise SystemExit


desc = {
    "s0": uctypes.UINT16 | 0,
    "sub": (0, {"b0": uctypes.UINT8 | 0, "b1": uctypes.UINT8 | 1}),
    "arr": (uctypes.ARRAY | 0, uctypes.UINT8 | 2),
    "arr2": (uctypes.ARRAY | 0, 2, {"b": uctypes.UINT8 | 0}),
    "bitf0": uctypes.BFUINT16 | 0 | 0 << uctypes.BF_POS | 8 << uctypes.BF_LEN,
    "bitf1": uctypes.BFUINT16 | 0 | 8 << uctypes.BF_POS | 8 << uctypes.BF_LEN,
    "bf0": uctypes.BFUINT16 | 0 | 0 << uctypes.BF_POS | 4 << uctypes.BF_LEN,
    "bf1": uctypes.BFUINT16 | 0 | 4 << uctypes.BF_POS | 4 << uctypes.BF_LEN,
    "bf2": uctypes.BFUINT16 | 0 | 8 << uctypes.BF_POS | 4 << uctypes.BF_LEN,
    "bf3": uctypes.BFUINT16 | 0 | 12 << uctypes.BF_POS | 4 << uctypes.BF_LEN,
    "ptr": (uctypes.PTR | 0, uctypes.UINT8),
    "ptr2": (uctypes.PTR | 0, {"b": uctypes.UINT8 | 0}),
}

data = bytearray(b"01")

S = uctypes.struct(uctypes.addressof(data), desc, uctypes.NATIVE)

# print(S)
print(hex(S.s0))
assert hex(S.s0) == "0x3130"
# print(S.sub.b0)
print(S.sub.b0, S.sub.b1)
assert S.sub.b0, S.sub.b1 == (0x30, 0x31)

try:
    S[0]
    assert False, "Can't index struct"
except TypeError:
    print("TypeError")

print("arr:", S.arr[0], S.arr[1])
assert (S.arr[0], S.arr[1]) == (0x30, 0x31)

print("arr of struct:", S.arr2[0].b, S.arr2[1].b)
assert (S.arr2[0].b, S.arr2[1].b) == (0x30, 0x31)


try:
    S.arr[2]
    assert False, "Out of bounds index"
except IndexError:
    print("IndexError")

print("bf:", S.bitf0, S.bitf1)
assert (S.bitf0, S.bitf1) == (0x30, 0x31)

print("bf 4bit:", S.bf3, S.bf2, S.bf1, S.bf0)
assert (S.bf3, S.bf2, S.bf1, S.bf0) == (3, 1, 3, 0)

# Write access

S.sub.b0 = ord("2")
print(data)
assert bytes(data) == b"21"

S.bf3 = 5
print(data)
assert bytes(data) == b"2Q"

desc2 = {
    "bf8": uctypes.BFUINT8 | 0 | 0 << uctypes.BF_POS | 4 << uctypes.BF_LEN,
    "bf32": uctypes.BFUINT32 | 0 | 20 << uctypes.BF_POS | 4 << uctypes.BF_LEN,
}

data2 = bytearray(b"0123")

S2 = uctypes.struct(uctypes.addressof(data2), desc2, uctypes.NATIVE)

# bitfield using uint8 as base type
S2.bf8 = 5
print(data2)
assert bytes(data2) == b"5123"

# bitfield using uint32 as base type
S2.bf32 = 5
print(data2)
assert bytes(data2) == b"51R3"
