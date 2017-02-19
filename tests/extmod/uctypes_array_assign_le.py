try:
    import uctypes
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

desc = {
    # arr is array at offset 0, of UINT8 elements, array size is 2
    "arr": (uctypes.ARRAY | 0, uctypes.UINT8 | 2),
    # arr2 is array at offset 0, size 2, of structures defined recursively
    "arr2": (uctypes.ARRAY | 0, 2, {"b": uctypes.UINT8 | 0}),
    "arr3": (uctypes.ARRAY | 2, uctypes.UINT16 | 2),

    # aligned
    "arr5": (uctypes.ARRAY | 0, uctypes.UINT32 | 1),
    # unaligned
    "arr6": (uctypes.ARRAY | 1, uctypes.UINT32 | 1),

    "arr7": (uctypes.ARRAY | 0, 1, {"l": uctypes.UINT32 | 0}),
    "arr8": (uctypes.ARRAY | 1, 1, {"l": uctypes.UINT32 | 0})
}

data = bytearray(5)

S = uctypes.struct(uctypes.addressof(data), desc,  uctypes.LITTLE_ENDIAN)

# assign byte
S.arr[0] = 0x11
print(hex(S.arr[0]))
assert hex(S.arr[0]) == "0x11"

# assign word
S.arr3[0] = 0x2233
print(hex(S.arr3[0]))
assert hex(S.arr3[0]) == "0x2233"

# assign word, with index
S.arr3[1] = 0x4455
print(hex(S.arr3[1]))
assert hex(S.arr3[1]) == "0x4455"

# assign long, aligned
S.arr5[0] = 0x66778899
print(hex(S.arr5[0]))
assert hex(S.arr5[0]) == "0x66778899"

print(S.arr5[0] == S.arr7[0].l)
assert S.arr5[0] == S.arr7[0].l


# assign long, unaligned
S.arr6[0] = 0xAABBCCDD
print(hex(S.arr6[0]))
assert hex(S.arr6[0]) == "0xaabbccdd"

print(S.arr6[0] == S.arr8[0].l)
assert S.arr6[0] == S.arr8[0].l

