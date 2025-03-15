# Test parsing ints.

try:
    bytearray
    memoryview
except NameError:
    print("SKIP")
    raise SystemExit

print(int(b"123"))
print(int(bytearray(b"123")))
print(int(memoryview(b"123")))
