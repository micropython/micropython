
class Foo:

    def __init__(self):
        self.a = 1
        self.b = "bar"

o = Foo()
if not hasattr(o, "__dict__"):
    print("SKIP")
    raise SystemExit

print(o.__dict__ == {'a': 1, 'b': 'bar'})


try:
    print(len(o.__dict__.items()) == len(o.__dict__))
    print(len(o.__dict__.keys()) == len(o.__dict__))
    print(len(o.__dict__.values()) == len(o.__dict__))
except TypeError:
    print("TypeError")
