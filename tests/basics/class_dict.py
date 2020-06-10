
class Foo:
    a = 1
    b = "bar"

if not hasattr(Foo, "__dict__"):
    print("SKIP")
    raise SystemExit

print(Foo.__dict__ == {'a': 1, 'b': 'bar'})
