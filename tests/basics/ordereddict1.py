try:
    from collections import OrderedDict
except ImportError:
    try:
        from ucollections import OrderedDict
    except ImportError:
        print("SKIP")
        import sys
        sys.exit()

d = OrderedDict([(10, 20), ("b", 100), (1, 2)])
print(list(d.keys()))
print(list(d.values()))
del d["b"]
print(list(d.keys()))
print(list(d.values()))
