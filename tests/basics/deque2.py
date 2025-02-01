try:
    from collections import deque
except ImportError:
    print("SKIP")
    raise SystemExit

# Initial sequence is supported
d = deque([1, 2, 3], 10)

# iteration over sequence
for x in d:
    print(x)

# Iterables larger than maxlen have the beginnings removed, also tests
# iteration through conversion to list
d = deque([1, 2, 3, 4, 5], 3)
print(list(d))

# Empty iterables are also supported
deque([], 10)

# Extending deques with iterables
d.extend([6, 7])
print(list(d))

# Accessing queue elements via index
d = deque((0, 1, 2, 3), 5)
print(d[0], d[1], d[-1])

# Writing queue elements via index
d[3] = 5
print(d[3])

# Access the last element via index, when the last element is at various locations
d = deque((), 2)
for i in range(4):
    d.append(i)
    print(i, d[-1])

# Write the last element then access all elements from the end
d[-1] = 4
print(d[-2], d[-1])

# Accessing indices out of bounds raises IndexError
try:
    d[4]
except IndexError:
    print("IndexError")

try:
    d[4] = 0
except IndexError:
    print("IndexError")
