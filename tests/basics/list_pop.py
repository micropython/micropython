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

# popping such that list storage shrinks (tests implementation detail of MicroPython)
l = list(range(20))
for i in range(len(l)):
    l.pop()
print(l)
