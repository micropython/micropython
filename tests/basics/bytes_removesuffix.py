try:
    bytes.removesuffix
except AttributeError:
    print("SKIP")
    raise SystemExit

print(b"foobar".removesuffix(b"bar"))
print(b"foobar".removesuffix(b"foo"))
print(b"foobar".removesuffix(b"foobar"))
print(b"foobar".removesuffix(b"foobar1"))
print(b"foobar".removesuffix(b"fooba"))
print(b"foobar".removesuffix(b"oobar"))
print(b"foobar".removesuffix(b"foo1"))
print(b"foobar".removesuffix(b"r"))
print(b"foobar".removesuffix(b""))

print(b"f".removesuffix(b"foo"))
print(b"f".removesuffix(b"f"))
print(b"f".removesuffix(b""))
print(b"".removesuffix(b"foo"))
print(b"".removesuffix(b"f"))
print(b"".removesuffix(b""))

print(b"\xaa \xaa \xaa".removesuffix(b"\xaa"))
print(b"\xaa".removesuffix(b"\xaa"))

try:
    b"foobar".removesuffix(1)
except TypeError:
    print("TypeError")

try:
    b"foobar".removesuffix("r")
except TypeError:
    print("TypeError")
