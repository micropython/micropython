a = [1, 2, 3]
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
    raise AssertionError("Did not raise ValueError")
