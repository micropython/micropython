try:
    bytearray.removesuffix
except AttributeError:
    print("SKIP")
    raise SystemExit

print(bytearray(b"foobar").removesuffix(b"bar"))
print(bytearray(b"foobar").removesuffix(b"foo"))
print(bytearray(b"foobar").removesuffix(b"foobar"))
print(bytearray(b"foobar").removesuffix(b"foobar1"))
print(bytearray(b"foobar").removesuffix(b"fooba"))
print(bytearray(b"foobar").removesuffix(b"oobar"))
print(bytearray(b"foobar").removesuffix(b"foo1"))
print(bytearray(b"foobar").removesuffix(b"r"))
print(bytearray(b"foobar").removesuffix(b""))

print(bytearray(b"f").removesuffix(b"foo"))
print(bytearray(b"f").removesuffix(b"f"))
print(bytearray(b"f").removesuffix(b""))
print(bytearray(b"").removesuffix(b"foo"))
print(bytearray(b"").removesuffix(b"f"))
print(bytearray(b"").removesuffix(b""))

print(bytearray(b"\xaa \xaa \xaa").removesuffix(b"\xaa"))
print(bytearray(b"\xaa").removesuffix(b"\xaa"))

try:
    bytearray(b"foobar").removesuffix(1)
except TypeError:
    print("TypeError")

try:
    bytearray(b"foobar").removesuffix("r")
except TypeError:
    print("TypeError")
