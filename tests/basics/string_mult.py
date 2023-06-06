# basic multiplication
print('0' * 5)

# check negative, 0, positive; lhs and rhs multiplication
for i in (-4, -2, 0, 2, 4):
    print(i * '12')
    print('12' * i)

# check that we don't modify existing object
a = '123'
c = a * 3
print(a, c)

# check inplace multiplication
a = '456'
a *= 3
print(a)

# check reverse inplace multiplication
a = 3
a *= '789'
print(a)
