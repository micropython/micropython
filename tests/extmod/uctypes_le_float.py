try:
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

desc = {
    "f32": uctypes.FLOAT32 | 0,
    "f64": uctypes.FLOAT64 | 0,
    "uf64": uctypes.FLOAT64 | 2,  # unaligned
}

data = bytearray(10)

S = uctypes.struct(uctypes.addressof(data), desc, uctypes.LITTLE_ENDIAN)

S.f32 = 12.34
print("%.4f" % S.f32)

S.f64 = 12.34
print("%.4f" % S.f64)

S.uf64 = 12.34
print("%.4f" % S.uf64)

# array of float/double
desc = {
    "af32": (uctypes.ARRAY | 0, uctypes.FLOAT32 | 2),
    "af64": (uctypes.ARRAY | 0, uctypes.FLOAT64 | 2),
}
data = bytearray(16)
S = uctypes.struct(uctypes.addressof(data), desc, uctypes.LITTLE_ENDIAN)

S.af32[0] = 1
S.af32[1] = 2
print("%.4f %.4f" % (S.af32[0], S.af32[1]), data)

S.af64[0] = 1
S.af64[1] = 2
print("%.4f %.4f" % (S.af64[0], S.af64[1]), data)
