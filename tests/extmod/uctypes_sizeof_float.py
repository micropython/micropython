try:
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

print(uctypes.sizeof({"f": uctypes.FLOAT32}))
print(uctypes.sizeof({"f": uctypes.FLOAT64}))
