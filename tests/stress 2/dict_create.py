# create a large dictionary

d = {}
x = 1
while x < 1000:
    d[x] = x
    x += 1
print(d[500])
