# test errors from bad operations (unary, binary, etc)
try:
    memoryview
except:
    print("SKIP")
    raise SystemExit

# unsupported binary operators
try:
    memoryview(b"") + b""
except TypeError:
    print("TypeError")

try:
    memoryview(b"") + memoryview(b"")
except TypeError:
    print("TypeError")

try:
    m = memoryview(bytearray())
    m += bytearray()
except TypeError:
    print("TypeError")
