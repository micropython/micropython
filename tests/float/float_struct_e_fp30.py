# Test struct pack/unpack with 'e' typecode.

try:
    import struct
except ImportError:
    print("SKIP")
    raise SystemExit

test_values = (
    1e-7,
    2e-7,
    1e-6,
    1e-5,
    1e-4,
    1e-3,
    1e-2,
    0.1,
    0,
    1,
    2,
    4,
    8,
    10,
    100,
    1e3,
    1e4,
    6e4,
    float("inf"),
)

for j in test_values:
    for i in (j, -j):
        x = struct.pack("<e", i)
        v = struct.unpack("<e", x)[0]
        print("%.7f %s %.5f %s" % (i, x, v, i == v))

# In CPython, packing a float that doesn't fit into a half-float raises OverflowError.
# But in MicroPython it does not, but rather stores the value as inf.
# This test is here for coverage.
try:
    struct.pack("e", 1e15)
except OverflowError:
    pass
