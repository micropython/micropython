try:
    from collections import OrderedDict
except ImportError:
    try:
        from ucollections import OrderedDict
    except ImportError:
        print("SKIP")
        raise SystemExit

x = OrderedDict()
y = OrderedDict()
x['a'] = 1
x['b'] = 2
y['a'] = 1
y['b'] = 2
print(x)
print(y)
print(x == y)

z = OrderedDict()
z['b'] = 2
z['a'] = 1
print(y)
print(z)
print(y == z)

del z['b']
z['b'] = 2
print(y)
print(z)
print(y == z)

del x['a']
del y['a']
print(x)
print(y)
print(x == y)

del z['b']
del y['b']
print(y)
print(z)
print(y == z)
