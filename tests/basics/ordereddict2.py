try:
    from collections import OrderedDict
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    {'a': None}.move_to_end('a')
except (TypeError, AttributeError):
    print("Exception")

d = OrderedDict(a=1, b=2, c=3)
d.move_to_end('a')
print(list(d.items()))

d = OrderedDict(a=1, b=2, c=3)
d.move_to_end('b')
print(list(d.items()))

d = OrderedDict(a=1, b=2, c=3)
d.move_to_end('c')
print(list(d.items()))

try:
    d.move_to_end('x')
except KeyError:
    print("KeyError")

d = OrderedDict(a=1, b=2, c=3)
d.move_to_end('a', last=False)
print(list(d.items()))

d = OrderedDict(a=1, b=2, c=3)
d.move_to_end('b', last=False)
print(list(d.items()))

d = OrderedDict(a=1, b=2, c=3)
d.move_to_end('c', last=False)
print(list(d.items()))

try:
    d.move_to_end('x', last=False)
except KeyError:
    print("KeyError")
