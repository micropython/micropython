# test using an OrderedDict as the locals to construct a class

try:
    from collections import OrderedDict
except ImportError:
    print("SKIP")
    raise SystemExit

if not hasattr(int, "__dict__"):
    print("SKIP")
    raise SystemExit


A = type("A", (), OrderedDict(a=1, b=2, c=3, d=4, e=5))
print([k for k in A.__dict__.keys() if not k.startswith("_")])
