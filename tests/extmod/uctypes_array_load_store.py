# Test uctypes array, load and store, with array size > 1

try:
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

N = 5

for endian in ("NATIVE", "LITTLE_ENDIAN", "BIG_ENDIAN"):
    for type_ in ("INT8", "UINT8", "INT16", "UINT16", "INT32", "UINT32", "INT64", "UINT64"):
        desc = {"arr": (uctypes.ARRAY | 0, getattr(uctypes, type_) | N)}
        sz = uctypes.sizeof(desc)
        data = bytearray(sz)
        s = uctypes.struct(uctypes.addressof(data), desc, getattr(uctypes, endian))
        for i in range(N):
            s.arr[i] = i - 2
        print(endian, type_, sz, *(s.arr[i] for i in range(N)))
