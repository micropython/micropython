# dict constructor

d = dict()
print(d)

d = dict({1:2})
print(d)

d = dict(a=1)
print(d)

d = dict({1:2}, a=3)
print(d[1], d['a'])

d = dict([(1, 2)], a=3, b=4)
print(d[1], d['a'], d['b'])
