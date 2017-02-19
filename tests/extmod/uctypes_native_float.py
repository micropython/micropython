try:
    import uctypes
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

desc = {
    "f32": uctypes.FLOAT32 | 0,
    "f64": uctypes.FLOAT64 | 0,
}

data = bytearray(8)

S = uctypes.struct(uctypes.addressof(data), desc, uctypes.NATIVE)

S.f32 = 12.34
print('%.4f' % S.f32)

S.f64 = 12.34
print('%.4f' % S.f64)
