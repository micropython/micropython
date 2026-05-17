print(dict([(1, "foo")]))
d = dict([("foo", "foo2"), ("bar", "baz")])
print(sorted(d.keys()))
print(sorted(d.values()))

try:
    dict(((1,),))
except ValueError:
    print("ValueError")

try:
    dict(((1, 2, 3),))
except ValueError:
    print("ValueError")
