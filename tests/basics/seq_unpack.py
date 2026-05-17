# Basics
a, b = 1, 2
print(a, b)
a, b = (1, 2)
print(a, b)
(a, b) = 1, 2
print(a, b)
(a, b) = (1, 2)
print(a, b)

# Tuples/lists are optimized
a, b = [1, 2]
print(a, b)
[a, b] = 100, 200
print(a, b)

# optimised 3-way swap
a = 1
b = 2
c = 3
a, b, c = b, c, a
print(a, b, c)

try:
    a, b, c = (1, 2)
except ValueError:
    print("ValueError")
try:
    a, b, c = [1, 2, 3, 4]
except ValueError:
    print("ValueError")

# Generic iterable object
a, b, c = range(3)
print(a, b, c)
try:
    a, b, c = range(2)
except ValueError:
    print("ValueError")
try:
    a, b, c = range(4)
except ValueError:
    print("ValueError")
