import sstruct

desc = {
    "s0": sstruct.UINT16 | 0,
    "sub": (0, {
        "b0": sstruct.UINT8 | 0,
        "b1": sstruct.UINT8 | 1,
    }),
    "arr": (sstruct.ARRAY | 0, sstruct.UINT8 | 2),
    "arr2": (sstruct.ARRAY | 0, 2, {"b": sstruct.UINT8 | 0}),
    "bitf0": sstruct.BFUINT16 | 0 | 0 << 17 | 8 << 22,
    "bitf1": sstruct.BFUINT16 | 0 | 8 << 17 | 8 << 22,

    "bf0": sstruct.BFUINT16 | 0 | 0 << 17 | 4 << 22,
    "bf1": sstruct.BFUINT16 | 0 | 4 << 17 | 4 << 22,
    "bf2": sstruct.BFUINT16 | 0 | 8 << 17 | 4 << 22,
    "bf3": sstruct.BFUINT16 | 0 | 12 << 17 | 4 << 22,

    "ptr": (sstruct.PTR | 0, sstruct.UINT8),
    "ptr2": (sstruct.PTR | 0, {"b": sstruct.UINT8 | 0}),
}

data = bytearray(b"01")

S = sstruct.sstruct(desc, sstruct.addressof(data), sstruct.LITTLE_ENDIAN)

#print(S)
print(hex(S.s0))
assert hex(S.s0) == "0x3130"
#print(S.sub.b0)
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
