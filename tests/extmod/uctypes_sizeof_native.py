try:
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

S1 = {}
assert uctypes.sizeof(S1) == 0

S2 = {"a": uctypes.UINT8 | 0}
assert uctypes.sizeof(S2) == 1

S3 = {
    "a": uctypes.UINT8 | 0,
    "b": uctypes.UINT8 | 1,
}
assert uctypes.sizeof(S3) == 2

S4 = {
    "a": uctypes.UINT8 | 0,
    "b": uctypes.UINT32 | 4,
    "c": uctypes.UINT8 | 8,
}
assert uctypes.sizeof(S4) == 12

S5 = {
    "a": uctypes.UINT8 | 0,
    "b": uctypes.UINT32 | 4,
    "c": uctypes.UINT8 | 8,
    "d": uctypes.UINT32 | 0,
    "sub": (4, {"b0": uctypes.UINT8 | 0, "b1": uctypes.UINT8 | 1}),
}

assert uctypes.sizeof(S5) == 12

s5 = uctypes.struct(0, S5)
assert uctypes.sizeof(s5) == 12
assert uctypes.sizeof(s5.sub) == 2

S6 = {
    "ptr": (uctypes.PTR | 0, uctypes.UINT8),
}
# As if there're no other arch bitnesses
assert uctypes.sizeof(S6) in (4, 8)

S7 = {
    "arr": (uctypes.ARRAY | 0, uctypes.UINT8 | 5),
}
assert uctypes.sizeof(S7) == 5

S8 = {
    "arr": (uctypes.ARRAY | 0, 3, {"a": uctypes.UINT32 | 0, "b": uctypes.UINT8 | 4}),
}
assert uctypes.sizeof(S8) == 24

# A nested struct is aligned by its own fields, not by fields of the enclosing struct
S9_32 = {"v": uctypes.INT32 | 0}
S9_16 = {"v": uctypes.INT16 | 0}
S9 = {"v1": (0, S9_32), "v2": (4, S9_32), "v3": (8, S9_16), "v4": (10, S9_16)}
assert uctypes.sizeof(S9) == 12

s9 = uctypes.struct(0, S9)
assert uctypes.sizeof(s9.v3) == 2

S10 = {
    "arr": (uctypes.ARRAY | 0, 2, S9),
}
assert uctypes.sizeof(S10) == 24

s10 = uctypes.struct(0, S10)
assert uctypes.addressof(s10.arr[1]) == 12
