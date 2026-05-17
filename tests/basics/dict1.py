# basic dictionary

d = {}
print(d)
d[2] = 123
print(d)
d = {1:2}
d[3] = 3
print(len(d), d[1], d[3])
d[1] = 0
print(len(d), d[1], d[3])
print(str(d) == '{1: 0, 3: 3}' or str(d) == '{3: 3, 1: 0}')

x = 1
while x < 100:
    d[x] = x
    x += 1
print(d[50])

# equality operator on dicts of different size
print({} == {1:1})

# equality operator on dicts of same size but with different keys
print({1:1} == {2:1})

# 0 replacing False's item
d = {}
d[False] = 'false'
d[0] = 'zero'
print(d)

# False replacing 0's item
d = {}
d[0] = 'zero'
d[False] = 'false'
print(d)

# 1 replacing True's item
d = {}
d[True] = 'true'
d[1] = 'one'
print(d)

# True replacing 1's item
d = {}
d[1] = 'one'
d[True] = 'true'
print(d)

# mixed bools and integers
d = {False:10, True:11, 2:12}
print(d[0], d[1], d[2])

# value not found
try:
    {}[0]
except KeyError as er:
    print('KeyError', er, er.args)

# unsupported unary op
try:
    +{}
except TypeError:
    print('TypeError')

# unsupported binary op
try:
    {} + {}
except TypeError:
    print('TypeError')
