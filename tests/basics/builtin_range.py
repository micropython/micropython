# test builtin range type

# print
print(range(4))

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
