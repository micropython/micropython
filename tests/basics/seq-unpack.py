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
