import sys

class Foo:
    a = 1
    def b():
        pass

if not hasattr(Foo, "__dict__"):
    print("SKIP")
    sys.exit()

print(Foo.__dict__ == {'a': 1, 'b': Foo.b, '__qualname__': 'Foo', '__module__': '__main__'})
# Also test C implemented class without `locals_attr`
print(type.__dict__ == {})
