import sys

class Foo:
    a = 1
    def b():
        pass

if not hasattr(Foo, "__dict__"):
    print("SKIP")
    sys.exit()

print(Foo.__dict__ == {'a': 1, 'b': Foo.b})
