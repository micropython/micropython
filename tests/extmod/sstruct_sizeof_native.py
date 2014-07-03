import sstruct

S1 = {}
assert sstruct.sizeof(S1) == 0

S2 = {"a": sstruct.UINT8 | 0}
assert sstruct.sizeof(S2) == 1

S3 = {
    "a": sstruct.UINT8 | 0,
    "b": sstruct.UINT8 | 1,
}
assert sstruct.sizeof(S3) == 2

S4 = {
    "a": sstruct.UINT8 | 0,
    "b": sstruct.UINT32 | 4,
    "c": sstruct.UINT8 | 8,
}
assert sstruct.sizeof(S4) == 12

S5 = {
    "a": sstruct.UINT8 | 0,
    "b": sstruct.UINT32 | 4,
    "c": sstruct.UINT8 | 8,
    "d": sstruct.UINT32 | 0,
    "sub": (4, {
        "b0": sstruct.UINT8 | 0,
        "b1": sstruct.UINT8 | 1,
    }),
}

assert sstruct.sizeof(S5) == 12

s5 = sstruct.sstruct(S5, 0)
assert sstruct.sizeof(s5) == 12
assert sstruct.sizeof(s5.sub) == 2

S6 = {
    "ptr": (sstruct.PTR | 0, sstruct.UINT8),
}
# As if there're no other arch bitnesses
assert sstruct.sizeof(S6) in (4, 8)

S7 = {
    "arr": (sstruct.ARRAY | 0, sstruct.UINT8 | 5),
}
assert sstruct.sizeof(S7) == 5

S8 = {
    "arr": (sstruct.ARRAY | 0, 3, {"a": sstruct.UINT32 | 0, "b": sstruct.UINT8 | 4}),
}
assert sstruct.sizeof(S8) == 24
