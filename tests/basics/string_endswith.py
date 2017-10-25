print("foobar".endswith("bar"))
print("foobar".endswith("baR"))
print("foobar".endswith("bar1"))
print("foobar".endswith("foobar"))
print("foobar".endswith(""))
print("foobar".endswith("foobarbaz"))

#print("1foobar".startswith("foo", 1))
#print("1foo".startswith("foo", 1))
#print("1foo".startswith("1foo", 1))
#print("1fo".startswith("foo", 1))
#print("1fo".startswith("foo", 10))

try:
    "foobar".endswith(1)
except TypeError:
    print("TypeError")
