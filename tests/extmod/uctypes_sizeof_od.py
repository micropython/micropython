try:
    from collections import OrderedDict
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

desc = OrderedDict(
    {
        # arr is array at offset 0, of UINT8 elements, array size is 2
        "arr": (uctypes.ARRAY | 0, uctypes.UINT8 | 2),
        # arr2 is array at offset 0, size 2, of structures defined recursively
        "arr2": (uctypes.ARRAY | 0, 2, {"b": uctypes.UINT8 | 0}),
        "arr3": (uctypes.ARRAY | 2, uctypes.UINT16 | 2),
        "arr4": (uctypes.ARRAY | 0, 2, {"b": uctypes.UINT8 | 0, "w": uctypes.UINT16 | 1}),
        "sub": (
            0,
            {
                "b1": uctypes.BFUINT8 | 0 | 4 << uctypes.BF_POS | 4 << uctypes.BF_LEN,
                "b2": uctypes.BFUINT8 | 0 | 0 << uctypes.BF_POS | 4 << uctypes.BF_LEN,
            },
        ),
    }
)

data = bytearray(b"01234567")

S = uctypes.struct(uctypes.addressof(data), desc, uctypes.LITTLE_ENDIAN)

print(uctypes.sizeof(S.arr))
assert uctypes.sizeof(S.arr) == 2

print(uctypes.sizeof(S.arr2))
assert uctypes.sizeof(S.arr2) == 2

print(uctypes.sizeof(S.arr3))

try:
    print(uctypes.sizeof(S.arr3[0]))
except TypeError:
    print("TypeError")

print(uctypes.sizeof(S.arr4))
assert uctypes.sizeof(S.arr4) == 6

print(uctypes.sizeof(S.sub))
assert uctypes.sizeof(S.sub) == 1
