print("foobar".startswith("foo"))
print("foobar".startswith("Foo"))
print("foobar".startswith("foo1"))
print("foobar".startswith("foobar"))
print("foobar".startswith(""))

print("1foobar".startswith("foo", 1))
print("1foo".startswith("foo", 1))
print("1foo".startswith("1foo", 1))
print("1fo".startswith("foo", 1))
print("1fo".startswith("foo", 10))

print("1foobar".startswith("foo", 1, 5))
print("1foobar".startswith("foo", 1, 4))
print("1foobar".startswith("foo", 1, 3))
print("1foobar".startswith("oo", 2, 4))
print("1foobar".startswith("o", 3, 4))
print("1foobar".startswith("o", 4, 4))
print("1foobar".startswith("o", 5, 4))

print("foobar".startswith("foo", None, None))
print("foobar".startswith("foo", None, 3))
print("foobar".startswith("foo", None, 2))
print("foobar".startswith("bar", 3, None))


print("foobar".startswith(("foo", "sth")))
print("foobar".startswith(("sth", "foo")))
print("foobar".startswith(("sth", "foo2")))
print("foobar".startswith(("foo", )))

try:
    "foobar".startswith(1)
except TypeError:
    print("TypeError")
