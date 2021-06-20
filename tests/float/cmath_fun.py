# test the functions imported from cmath

try:
    from cmath import *
except ImportError:
    print("SKIP")
    raise SystemExit

# make sure these constants exist in cmath
print("%.5g" % e)
print("%.5g" % pi)

test_values_non_zero = []
base_values = (0.0, 0.5, 1.2345, 10.0)
for r in base_values:
    for i in base_values:
        if r != 0.0 or i != 0.0:
            test_values_non_zero.append(complex(r, i))
        if r != 0.0:
            test_values_non_zero.append(complex(-r, i))
        if i != 0.0:
            test_values_non_zero.append(complex(r, -i))
        if r != 0.0 and i != 0.0:
            test_values_non_zero.append(complex(-r, -i))
test_values = [complex(0.0, 0.0)] + test_values_non_zero
print(test_values)

functions = [
    ("phase", phase, test_values),
    ("polar", polar, test_values),
    (
        "rect",
        rect,
        ((0, 0), (0, 1), (0, -1), (1, 0), (-1, 0), (1, 1), (-1, 1), (1, -1), (123.0, -456.0)),
    ),
    ("exp", exp, test_values),
    ("log", log, test_values_non_zero),
    ("sqrt", sqrt, test_values),
    ("cos", cos, test_values),
    ("sin", sin, test_values),
]

for f_name, f, test_vals in functions:
    print(f_name)
    for val in test_vals:
        if type(val) == tuple:
            ret = f(*val)
        else:
            ret = f(val)
        if type(ret) == float:
            print("%.5g" % ret)
        elif type(ret) == tuple:
            print("%.5g %.5g" % ret)
        else:
            # some test (eg cmath.sqrt(-0.5)) disagree with CPython with tiny real part
            real = ret.real
            if abs(real) < 1e-6:
                real = 0.0
            print("complex(%.5g, %.5g)" % (real, ret.imag))

# test invalid type passed to cmath function
try:
    log([])
except TypeError:
    print("TypeError")
