# test errors from bad operations (unary, binary, etc)
try:
    memoryview
except:
    print("SKIP")
    raise SystemExit

# unsupported binary operators
try:
    m = memoryview(bytearray())
    m += bytearray()
except TypeError:
    print('TypeError')
