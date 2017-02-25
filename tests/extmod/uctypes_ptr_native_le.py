import sys
try:
    import uctypes
except ImportError:
    print("SKIP")
    sys.exit()

if sys.byteorder != "little":
    print("SKIP")
    sys.exit()


desc = {
    "ptr": (uctypes.PTR | 0, uctypes.UINT8),
    "ptr16": (uctypes.PTR | 0, uctypes.UINT16),
    "ptr2": (uctypes.PTR | 0, {"b": uctypes.UINT8 | 0}),
}

bytes = b"01"

addr = uctypes.addressof(bytes)
buf = addr.to_bytes(uctypes.sizeof(desc), "little")

S = uctypes.struct(uctypes.addressof(buf), desc, uctypes.NATIVE)

print(S.ptr[0])
assert S.ptr[0] == ord("0")
print(S.ptr[1])
assert S.ptr[1] == ord("1")
print(hex(S.ptr16[0]))
assert hex(S.ptr16[0]) == "0x3130"
print(S.ptr2[0].b, S.ptr2[1].b)
print (S.ptr2[0].b, S.ptr2[1].b)
print(hex(S.ptr16[0]))
assert (S.ptr2[0].b, S.ptr2[1].b) == (48, 49)
