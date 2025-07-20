try:
    from collections import OrderedDict
except ImportError:
    print("SKIP")
    raise SystemExit

d = OrderedDict()

# ordered dict must reject nonhashable items, including slices
# This test operates with a .exp file because Python 3.12 made slice objects
# hashable.
try:
    d[:'a'] = 123
except:
    print("exception")

try:
    d[[]] = 123
except:
    print("exception")
print(len(d))
