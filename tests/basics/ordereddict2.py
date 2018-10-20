# Call to OrderedDict constructor can be optimized and treated as
# construction of literal OrderedDict object. This tests that there're
# no obvious artifacts in such optimization.
try:
    from collections import OrderedDict
except ImportError:
    try:
        from ucollections import OrderedDict
    except ImportError:
        print("SKIP")
        raise SystemExit

print(OrderedDict())
print(OrderedDict({}))
print(OrderedDict([(1, 2)]))
print(OrderedDict({1: 2}))
#print(OrderedDict({1: 2, 3: 4}))
