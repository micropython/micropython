# basic multiplication
print(b'0' * 5)

# check negative, 0, positive; lhs and rhs multiplication
for i in (-4, -2, 0, 2, 4):
    print(i * b'12')
    print(b'12' * i)

# check that we don't modify existing object
a = b'123'
c = a * 3
print(a, c)
