import sys
import uctypes

if sys.byteorder != "little":
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
    "arr7": (uctypes.ARRAY | 0, 1, {"l": uctypes.UINT32 | 0}),

    "arr9": (uctypes.ARRAY | 0, uctypes.INT8 | 1),
    "arr10": (uctypes.ARRAY | 0, uctypes.INT64 | 1),
    "arr11": (uctypes.ARRAY | 0, uctypes.UINT32 | 1),
    "arr12": (uctypes.ARRAY | 0, uctypes.INT16 | 1),
    "arr13": (uctypes.ARRAY | 0, uctypes.INT32 | 1),
    "arr14": (uctypes.ARRAY | 0, uctypes.UINT64| 1),
    "arr15": (uctypes.ARRAY | 1, 1, {"l": {}})
}

data = bytearray(8)

S = uctypes.struct(uctypes.addressof(data), desc)

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

# lines 368, 326
S.arr9[0] = 0x11
print(hex(S.arr9[0]))
assert hex(S.arr9[0]) == "0x11"

# lines 338, 380, 385
S.arr10[0] = 0x11
print(hex(S.arr10[0]))
assert hex(S.arr10[0]) == "0x11"

# lines 376
S.arr11[0] = 0x11
print(hex(S.arr11[0]))
assert hex(S.arr11[0]) == "0x11"

# lines 372, 330
S.arr12[0] = 0x11
print(hex(S.arr12[0]))
assert hex(S.arr12[0]) == "0x11"

# lines 330
S.arr13[0] = 0x11
print(hex(S.arr13[0]))
assert hex(S.arr13[0]) == "0x11"

# lines 336
S.arr14[0] = 0x11
print(hex(S.arr14[0]))
assert hex(S.arr14[0]) == "0x11"

#lines 543
try:
    print(S.arr9[None])
except IndexError:
    print("IndexError")

# lines 120, 245
try:
    S.arr15[0].l = 0x11
except TypeError:
    print("TypeError")

# lines 574
try:
    S.arr15[0] = 0x11
except TypeError:
    print("TypeError")
