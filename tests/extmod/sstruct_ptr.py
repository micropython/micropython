import sstruct

desc = {
    "ptr": (sstruct.PTR | 0, sstruct.UINT8),
    "ptr16": (sstruct.PTR | 0, sstruct.UINT16),
    "ptr2": (sstruct.PTR | 0, {"b": sstruct.UINT8 | 0}),
}

bytes = b"01"

addr = sstruct.addressof(bytes)
buf = addr.to_bytes(4)

S = sstruct.sstruct(desc, sstruct.addressof(buf))

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
