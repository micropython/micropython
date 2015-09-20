# test floating point floor divide and modulus
# it has some tricky corner cases

# pyboard has 32-bit floating point and gives different (but still
# correct) answers for certain combinations of divmod arguments.

def test(x, y):
    div, mod = divmod(x, y)
    print(div == x // y, mod == x % y, abs(div * y + mod - x) < 1e-6)

test(1.23456, 0.7)
test(-1.23456, 0.7)
test(1.23456, -0.7)
test(-1.23456, -0.7)

a = 1.23456
b = 0.7
test(a, b)
test(a, -b)
test(-a, b)
test(-a, -b)

for i in range(25):
    x = (i - 12.5) / 6
    for j in range(25):
        y = (j - 12.5) / 6
        test(x, y)

# test division by zero error
try:
    divmod(1.0, 0)
except ZeroDivisionError:
    print('ZeroDivisionError')
