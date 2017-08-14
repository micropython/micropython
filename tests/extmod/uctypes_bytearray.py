try:
    import uctypes
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

desc = {
    "arr": (uctypes.ARRAY | 0, uctypes.UINT8 | 2),
    "arr2": (uctypes.ARRAY | 2, uctypes.INT8 | 2),
}

data = bytearray(b"01234567")

S = uctypes.struct(uctypes.addressof(data), desc, uctypes.LITTLE_ENDIAN)

# Arrays of UINT8 are accessed as bytearrays
print(S.arr)
# But not INT8, because value range is different
print(type(S.arr2))
