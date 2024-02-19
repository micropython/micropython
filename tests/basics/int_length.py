# CIRCUITPY-CHANGE
# test bit_length for various sizes of ints

for x in range(-10, 10):
    print(x.bit_length())

for i in [2, 3, 15, 16, 17, 31, 32, 33, 63, 64, 234]:
    a = 2**i
    print((a - 1).bit_length())
    print((1 - a).bit_length())
    print(a.bit_length())
    print((-a).bit_length())
    print((a + 1).bit_length())
    print((-a - 1).bit_length())
    # Ensure transitioning between small and large int representations
    # still work.
    print((a - a).bit_length())
    print((0 * a).bit_length())
    print((0 * a - 1).bit_length())
    print((0 * a + 1).bit_length())
    print((2 * a).bit_length())
    print((2 * a + 1).bit_length())
    print((2 * a - 1).bit_length())
