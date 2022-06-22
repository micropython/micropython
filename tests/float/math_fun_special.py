# test the special functions imported from math

try:
    from math import *

    erf
except (ImportError, NameError):
    print("SKIP")
    raise SystemExit

test_values = [
    -8.0,
    -2.5,
    -1,
    -0.5,
    0.0,
    0.5,
    2.5,
    8.0,
]
pos_test_values = [
    0.001,
    0.1,
    0.5,
    1.0,
    1.5,
    10.0,
]

functions = [
    ("expm1", expm1, test_values),
    ("log2", log2, test_values),
    ("log10", log10, test_values),
    ("cosh", cosh, test_values),
    ("sinh", sinh, test_values),
    ("tanh", tanh, [-1e6, -100] + test_values + [100, 1e6]),
    ("acosh", acosh, [1.0, 5.0, 1.0]),
    ("asinh", asinh, test_values),
    ("atanh", atanh, [-0.99, -0.5, 0.0, 0.5, 0.99]),
    ("erf", erf, test_values),
    ("erfc", erfc, test_values),
    ("gamma", gamma, pos_test_values),
    ("lgamma", lgamma, pos_test_values + [50.0, 100.0]),
]

for function_name, function, test_vals in functions:
    print(function_name)
    for value in test_vals:
        try:
            print("{:.4g}".format(function(value)))
        except ValueError as e:
            print(str(e))


# Tests for gcd mostly duplicated from
# https://github.com/python/cpython/blob/main/Lib/test/test_math.py


def assertRaises(exception, func, *args):
    try:
        print(func(*args))
        assert False
    except exception:
        pass


print(gcd(0, 0))
print(gcd(1, 0))
print(gcd(-1, 0))
print(gcd(0, 1))
print(gcd(0, -1))
print(gcd(7, 1))
print(gcd(7, -1))
print(gcd(-23, 15))
print(gcd(120, 84))
print(gcd(84, -120))
print(gcd(1216342683557601535506311712, 436522681849110124616458784))

x = 434610456570399902378880679233098819019853229470286994367836600566
y = 1064502245825115327754847244914921553977
for c in (652560, 576559230871654959816130551884856912003141446781646602790216406874):
    a = x * c
    b = y * c
    print(gcd(a, b))
    print(gcd(b, a))
    print(gcd(-a, b))
    print(gcd(b, -a))
    print(gcd(a, -b))
    print(gcd(-b, a))
    print(gcd(-a, -b))
    print(gcd(-b, -a))

# In MP minimum argument count is 2
# self.assertEqual(gcd(), 0)
# self.assertEqual(gcd(120), 120)
# self.assertEqual(gcd(-120), 120)

print(gcd(120, 84, 102))
print(gcd(120, 1, 84))
print(gcd(6, 30, 40, -60, 20, 40))
print(gcd(12345678912345678912345678911561561658135153135135135, 123456))

assertRaises(TypeError, gcd, 120.0)
assertRaises(TypeError, gcd, 120.0, 84)
assertRaises(TypeError, gcd, 120, 84.0)
assertRaises(TypeError, gcd, 120, 1, 84.0)
