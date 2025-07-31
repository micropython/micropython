# test general errors with uctypes

try:
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

data = bytearray(b"01234567")

# first argument isn't an integer
try:
    uctypes.struct(data, {})
except TypeError:
    print("TypeError")

# del subscr not supported
S = uctypes.struct(uctypes.addressof(data), {})
try:
    del S[0]
except TypeError:
    print("TypeError")

# list is an invalid descriptor
S = uctypes.struct(uctypes.addressof(data), [])
try:
    S.x
except TypeError:
    print("TypeError")

# can't access attribute with invalid descriptor
S = uctypes.struct(uctypes.addressof(data), {"x": []})
try:
    S.x
except TypeError:
    print("TypeError")

# can't assign to aggregate
S = uctypes.struct(uctypes.addressof(data), {"x": (uctypes.ARRAY | 0, uctypes.INT8 | 2)})
try:
    S.x = 1
except TypeError:
    print("TypeError")

# unsupported unary op
try:
    hash(S)
except TypeError:
    print("TypeError")
