try:
    import uctypes
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

desc = {
    "f32": uctypes.FLOAT32 | 0,
    "f64": uctypes.FLOAT64 | 0,
    "uf64": uctypes.FLOAT64 | 2, # unaligned
}

data = bytearray(10)

S = uctypes.struct(uctypes.addressof(data), desc, uctypes.LITTLE_ENDIAN)

S.f32 = 12.34
print('%.4f' % S.f32)

S.f64 = 12.34
print('%.4f' % S.f64)

S.uf64 = 12.34
print('%.4f' % S.uf64)
