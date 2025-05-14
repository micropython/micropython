print("foobar".endswith("bar"))
print("foobar".endswith("baR"))
print("foobar".endswith("bar1"))
print("foobar".endswith("foobar"))
print("foobar".endswith(""))
print("foobar".endswith("foobarbaz"))

print("foobar".endswith("bar", 3))
print("foobar".endswith("bar", 4))
print("foobar".endswith("foo", 0, 3))
print("foobar".endswith("foo", 0, 4))
print("foobar".endswith("foo", 1, 3))
print("foobar".endswith("foo", 1, 3))
print("foobar".endswith("oo", 1, 3))
print("foobar".endswith("o", 2, 3))
print("foobar".endswith("o", 3, 3))
print("foobar".endswith("o", 4, 3))

print("foobar".endswith("bar", None, None))
print("foobar".endswith("bar", None, 3))
print("foobar".endswith("bar", 3, None))
print("foobar".endswith("bar", 2, None))
print("foobar".endswith("foo", None, 3))

print("foobar".endswith(("bar", "foo")))
print("foobar".endswith(("foo", "bar")))
print("foobar".endswith(("foo", "bar1")))
print("foobar".endswith(("bar", )))
print("foobar".endswith(("foo", )))

try:
    "foobar".endswith(1)
except TypeError:
    print("TypeError")
