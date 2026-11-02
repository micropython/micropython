# test __dict__ attribute ordering of a class

if not hasattr(int, "__dict__"):
    print("SKIP")
    raise SystemExit

from collections import OrderedDict

# dict of a user class
class Foo:
    abcd = 1
    efgh = 2
    ijkl = 3

# dict of a user class
class Foo2:
    ijkl = 3
    abcd = 1
    efgh = 2

print([k for k in Foo.__dict__.keys() if not k.startswith('_')])
print([k for k in Foo2.__dict__.keys() if not k.startswith('_')])


Foo3 = type('Foo3', (object,), OrderedDict(abcd=1, efgh=2, ijkl=3))
print([k for k in Foo3.__dict__.keys() if not k.startswith('_')])
