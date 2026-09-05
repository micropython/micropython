try:
    bytearray.removeprefix
except AttributeError:
    print("SKIP")
    raise SystemExit

print(bytearray(b"foobar").removeprefix(b"bar"))
print(bytearray(b"foobar").removeprefix(b"foo"))
print(bytearray(b"foobar").removeprefix(b"foobar"))
print(bytearray(b"foobar").removeprefix(b"foobar1"))
print(bytearray(b"foobar").removeprefix(b"fooba"))
print(bytearray(b"foobar").removeprefix(b"oobar"))
print(bytearray(b"foobar").removeprefix(b"foo1"))
print(bytearray(b"foobar").removeprefix(b"f"))
print(bytearray(b"foobar").removeprefix(b""))

print(bytearray(b"f").removeprefix(b"foo"))
print(bytearray(b"f").removeprefix(b"f"))
print(bytearray(b"f").removeprefix(b""))
print(bytearray(b"").removeprefix(b"foo"))
print(bytearray(b"").removeprefix(b"f"))
print(bytearray(b"").removeprefix(b""))

print(bytearray(b"\xaa \xaa \xaa").removeprefix(b"\xaa"))
print(bytearray(b"\xaa").removeprefix(b"\xaa"))

try:
    bytearray(b"foobar").removeprefix(1)
except TypeError:
    print("TypeError")

try:
    bytearray(b"foobar").removeprefix("f")
except TypeError:
    print("TypeError")
