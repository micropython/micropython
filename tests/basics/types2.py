# Types are hashable
print(hash(type) != 0)
print(hash(int) != 0)
print(hash(list) != 0)
class Foo: pass
print(hash(Foo) != 0)

print(int == int)
print(int != list)

d = {}
d[int] = list
d[list] = int
print(len(d))
