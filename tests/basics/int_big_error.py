# test errors operating on bignum

i = 1 << 65

try:
    i << -1
except ValueError:
    print("ValueError")

try:
    len(i)
except TypeError:
    print("TypeError")

try:
    1 in i
except TypeError:
    print("TypeError")
