# test builtin range type

# print
print(range(4))

# bool
print(bool(range(0)))
print(bool(range(10)))

# len
print(len(range(0)))
print(len(range(4)))
print(len(range(1, 4)))
print(len(range(1, 4, 2)))
print(len(range(1, 4, -1)))
print(len(range(4, 1, -1)))
print(len(range(4, 1, -2)))

# subscr
print(range(4)[0])
print(range(4)[1])
print(range(4)[-1])

# slice
print(range(4)[0:])
print(range(4)[1:])
print(range(4)[1:2])
print(range(4)[1:3])
print(range(4)[1::2])
print(range(4)[1:-2:2])
print(range(1, 4)[:])
print(range(1, 4)[0:])
print(range(1, 4)[1:])
print(range(1, 4)[:-1])
print(range(7, -2, -4)[:])

# bad unary op
try:
    -range(1)
except TypeError:
    print("TypeError")

# bad subscription (can't store)
try:
    range(1)[0] = 1
except TypeError:
    print("TypeError")
