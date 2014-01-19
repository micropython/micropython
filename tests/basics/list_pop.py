# list poppin'
a = [1, 2, 3]
print(a.pop())
print(a.pop())
print(a.pop())
try:
    print(a.pop())
except IndexError:
    print("IndexError raised")
else:
    raise AssertionError("No IndexError raised")
