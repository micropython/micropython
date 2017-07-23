try:
    from collections import OrderedDict
except ImportError:
    try:
        from ucollections import OrderedDict
    except ImportError:
        print("SKIP")
        raise SystemExit

d = OrderedDict([(10, 20), ("b", 100), (1, 2)])
print(len(d))
print(list(d.keys()))
print(list(d.values()))
del d["b"]
print(len(d))
print(list(d.keys()))
print(list(d.values()))

# access remaining elements after deleting
print(d[10], d[1])

# add an element after deleting
d["abc"] = 123
print(len(d))
print(list(d.keys()))
print(list(d.values()))
