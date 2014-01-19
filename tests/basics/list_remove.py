a = [1, 2, 3]
print(a.remove(2))
print(a)
try:
    a.remove(2)
except ValueError:
    print("Raised ValueError")
else:
    raise AssertionError("Did not raise ValueError")
