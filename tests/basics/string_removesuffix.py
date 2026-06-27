try:
    str.removesuffix
except AttributeError:
    print("SKIP")
    raise SystemExit

print("foobar".removesuffix("bar"))
print("foobar".removesuffix("foo"))
print("foobar".removesuffix("foobar"))
print("foobar".removesuffix("foobar1"))
print("foobar".removesuffix("fooba"))
print("foobar".removesuffix("oobar"))
print("foobar".removesuffix("foo1"))
print("foobar".removesuffix("r"))
print("foobar".removesuffix(""))

print("f".removesuffix("foo"))
print("f".removesuffix("f"))
print("f".removesuffix(""))
print("".removesuffix("foo"))
print("".removesuffix("f"))
print("".removesuffix(""))

try:
    "foobar".removesuffix(1)
except TypeError:
    print("TypeError")
