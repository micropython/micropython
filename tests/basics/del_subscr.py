l = [1, 2, 3]
print(l)
del l[0]
print(l)
del l[-1]
print(l)

d = {1:2, 3:4, 5:6}
del d[1]
del d[3]
print(d)
del d[5]
print(d)

# delete nested subscr
d = {0:{0:0}}
del d[0][0]
print(d)
