d = {1:2, 3:4}
print(len(d))
d.update(["ab"])
print(d[1])
print(d[3])
print(d["a"])
print(len(d))
d.update([(1,4)])
print(d[1])
print(len(d))

# using keywords
d.update(a=5)
print(d['a'])
d.update([(1,5)], b=6)
print(d[1], d['b'])
