d = dict.fromkeys([1, 2, 3, 4])
l = list(d.keys())
l.sort()
print(l)

d = dict.fromkeys([1, 2, 3, 4], 42)
l = list(d.values())
l.sort()
print(l)

# argument to fromkeys has no __len__
d = dict.fromkeys(reversed(range(1)))
print(d)
