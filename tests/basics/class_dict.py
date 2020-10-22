# test __dict__ attribute of a class

if not hasattr(int, "__dict__"):
    print("SKIP")
    raise SystemExit


# dict of a built-in type
print("from_bytes" in int.__dict__)


# dict of a user class
class Foo:
    a = 1
    b = "bar"


d = Foo.__dict__
print(d["a"], d["b"])


# dict of a class that has no locals_dict (return empty dict).
d = type(type('')).__dict__
print(d is not None)
