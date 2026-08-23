try:
    from collections import OrderedDict
except ImportError:
    print("SKIP")
    raise SystemExit

d = OrderedDict.fromkeys("abcdefg")
print(type(d).__name__)
print("".join(d))
print(list(d.keys()))

d2 = OrderedDict.fromkeys([1, 2, 3], 42)
print(list(d2.values()))

plain = dict.fromkeys([3, 1, 2])
print(type(plain).__name__)
