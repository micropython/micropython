# list count tests
a = [1, 2, 3]
a = a + a + a
b = [0, 0, a, 0, a, 0]
print(a.count(2))
print(b.count(a))
