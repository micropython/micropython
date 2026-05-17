# test binary operations on range objects; (in)equality only

# this "feature test" actually tests the implementation but is the best we can do
if range(1) != range(1):
    print("SKIP")
    raise SystemExit

# basic (in)equality
print(range(1) == range(1))
print(range(1) != range(1))
print(range(1) != range(2))

# empty range
print(range(0) == range(0))
print(range(1, 0) == range(0))
print(range(1, 4, -1) == range(6, 3))

# 1 element range
print(range(1, 4, 10) == range(1, 4, 10))
print(range(1, 4, 10) == range(1, 4, 20))
print(range(1, 4, 10) == range(1, 8, 20))

# more than 1 element
print(range(0, 3, 2) == range(0, 3, 2))
print(range(0, 3, 2) == range(0, 4, 2))
print(range(0, 3, 2) == range(0, 5, 2))

# unsupported binary op
try:
    range(1) + 10
except TypeError:
    print('TypeError')
