try:
    str.removeprefix
except AttributeError:
    print("SKIP")
    raise SystemExit

print("foobar".removeprefix("bar"))
print("foobar".removeprefix("foo"))
print("foobar".removeprefix("foobar"))
print("foobar".removeprefix("foobar1"))
print("foobar".removeprefix("fooba"))
print("foobar".removeprefix("oobar"))
print("foobar".removeprefix("foo1"))
print("foobar".removeprefix("f"))
print("foobar".removeprefix(""))

print("f".removeprefix("foo"))
print("f".removeprefix("f"))
print("f".removeprefix(""))
print("".removeprefix("foo"))
print("".removeprefix("f"))
print("".removeprefix(""))

try:
    "foobar".removeprefix(1)
except TypeError:
    print("TypeError")
