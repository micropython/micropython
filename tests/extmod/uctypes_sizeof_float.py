try:
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

print(uctypes.sizeof({"f": uctypes.FLOAT32}))
print(uctypes.sizeof({"f": uctypes.FLOAT64}))
print(uctypes.sizeof({"f": (uctypes.ARRAY | 0, uctypes.FLOAT32 | 2)}))
print(uctypes.sizeof({"f": (uctypes.ARRAY | 0, uctypes.FLOAT64 | 2)}))
