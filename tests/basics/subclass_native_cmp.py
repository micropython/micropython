# Test calling non-special method inherited from native type

class mytuple(tuple):
    pass

t = mytuple((1, 2, 3))
print(t)
print(t == (1, 2, 3))
print((1, 2, 3) == t)

print(t < (1, 2, 3), t < (1, 2, 4))
print((1, 2, 3) <= t, (1, 2, 4) < t)
