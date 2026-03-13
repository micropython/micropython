try:
    bytes.removeprefix
except AttributeError:
    print("SKIP")
    raise SystemExit

print(b"foobar".removeprefix(b"bar"))
print(b"foobar".removeprefix(b"foo"))
print(b"foobar".removeprefix(b"foobar"))
print(b"foobar".removeprefix(b"foobar1"))
print(b"foobar".removeprefix(b"fooba"))
print(b"foobar".removeprefix(b"oobar"))
print(b"foobar".removeprefix(b"foo1"))
print(b"foobar".removeprefix(b"f"))
print(b"foobar".removeprefix(b""))

print(b"f".removeprefix(b"foo"))
print(b"f".removeprefix(b"f"))
print(b"f".removeprefix(b""))
print(b"".removeprefix(b"foo"))
print(b"".removeprefix(b"f"))
print(b"".removeprefix(b""))

print(b"\xaa \xaa \xaa".removeprefix(b"\xaa"))
print(b"\xaa".removeprefix(b"\xaa"))

try:
    b"foobar".removeprefix(1)
except TypeError:
    print("TypeError")

try:
    b"foobar".removeprefix("f")
except TypeError:
    print("TypeError")
