a = [1, 2, 3]
print(a.index(1))
print(a.index(2))
print(a.index(3))
print(a.index(3, 2))
print(a.index(1, -100))
print(a.index(1, False))

try:
    print(a.index(1, True))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print(a.index(3, 2, 2))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

a = a + a
b = [0, 0, a]
print(a.index(2))
print(b.index(a))
print(a.index(2, 2))

try:
    a.index(2, 2, 2)
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

# 3rd argument to index greater than length of list
print([1, 2].index(1, 0, 4))
