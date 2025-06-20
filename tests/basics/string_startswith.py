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

try:
    "foobar".startswith(1)
except TypeError:
    print("TypeError")
