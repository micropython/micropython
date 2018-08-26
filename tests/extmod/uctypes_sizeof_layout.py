try:
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

desc = {
    "f1": 0 | uctypes.UINT32,
    "f2": 4 | uctypes.UINT8,
}


# uctypes.NATIVE is default
print(uctypes.sizeof(desc) == uctypes.sizeof(desc, uctypes.NATIVE))

# Here we assume that that we run on a platform with convential ABI
# (which rounds up structure size based on max alignment). For platforms
# where that doesn't hold, this tests should be just disabled in the runner.
print(uctypes.sizeof(desc, uctypes.NATIVE) > uctypes.sizeof(desc, uctypes.LITTLE_ENDIAN))

# When taking sizeof of instantiated structure, layout type param
# is prohibited (because structure already has its layout type).
s = uctypes.struct(0, desc, uctypes.LITTLE_ENDIAN)
try:
    uctypes.sizeof(s, uctypes.LITTLE_ENDIAN)
except TypeError:
    print("TypeError")
