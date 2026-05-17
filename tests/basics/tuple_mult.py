# basic multiplication
print((0,) * 5)

# check negative, 0, positive; lhs and rhs multiplication
for i in (-4, -2, 0, 2, 4):
    print(i * (1, 2))
    print((1, 2) * i)

# check that we don't modify existing tuple
a = (1, 2, 3)
c = a * 3
print(a, c)

# inplace multiplication
a = (1, 2)
a *= 2
print(a)

# unsupported type on RHS
try:
    () * None
except TypeError:
    print('TypeError')
