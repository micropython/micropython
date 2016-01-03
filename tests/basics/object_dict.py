import sys

class Foo:

    def __init__(self):
        self.a = 1
        self.b = "bar"

o = Foo()
if not hasattr(o, "__dict__"):
    print("SKIP")
    sys.exit()

print(o.__dict__ == {'a': 1, 'b': 'bar'})
