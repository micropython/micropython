try:
    import uctypes
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

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
    "sub": (4, {
        "b0": uctypes.UINT8 | 0,
        "b1": uctypes.UINT8 | 1,
    }),
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
