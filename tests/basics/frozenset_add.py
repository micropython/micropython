try:
    frozenset
except NameError:
    print("SKIP")
    import sys
    sys.exit()

s = frozenset({1, 2, 3, 4})
try:
    print(s.add(5))
except AttributeError:
    print("AttributeError")

try:
    print(s.update([5]))
except AttributeError:
    print("AttributeError")
