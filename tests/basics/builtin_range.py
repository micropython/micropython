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
print(range(4, 1)[:])
print(range(4, 1)[0:])
print(range(4, 1)[1:])
print(range(4, 1)[:-1])
print(range(7, -2, -4)[:])
print(range(1, 100, 5)[5:15:3])
print(range(1, 100, 5)[15:5:-3])
print(range(100, 1, -5)[5:15:3])
print(range(100, 1, -5)[15:5:-3])
print(range(1, 100, 5)[5:15:-3])
print(range(1, 100, 5)[15:5:3])
print(range(100, 1, -5)[5:15:-3])
print(range(100, 1, -5)[15:5:3])
print(range(1, 100, 5)[5:15:3])
print(range(1, 100, 5)[15:5:-3])
print(range(1, 100, -5)[5:15:3])
print(range(1, 100, -5)[15:5:-3])
print(range(1, 100, 5)[5:15:-3])
print(range(1, 100, 5)[15:5:3])
print(range(1, 100, -5)[5:15:-3])
print(range(1, 100, -5)[15:5:3])

for i in range(24, 67):
    k = (1 << i) - 2
    try:
        if k == 65:
            raise OverflowError("fake overflow error to stop cpython")
        k0 = range(k, k+1)[0]
        if k0 != k:
            print(f"Range element {k} vs {k0}")
            break
        k0 = len(range(0, k))
        if k0 != k:
            print(f"Range length {k} vs {k0}")
            break
        k0 = len(range(-k, 0))
        if k0 != k:
            print(f"Range length {k} vs {k0}")
            break
        k = -k
        k0 = range(k, k+1)[0]
        if k0 != k:
            print(f"Range element {k} vs {k0}")
            break
    except OverflowError:
        print("range element/length test OK")
        break

# for this case uPy gives a different stop value but the listed elements are still correct
print(list(range(7, -2, -4)[2:-2:]))

# zero step
try:
    range(1, 2, 0)
except ValueError:
    print("ValueError")

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
