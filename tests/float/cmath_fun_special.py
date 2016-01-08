# test the special functions imported from cmath

try:
    from cmath import *
    log10
except (ImportError, NameError):
    print("SKIP")
    import sys
    sys.exit()

test_values_non_zero = []
base_values = (0.0, 0.5, 1.2345, 10.)
for r in base_values:
    for i in base_values:
        if r != 0. or i != 0.:
            test_values_non_zero.append(complex(r, i))
        if r != 0.:
            test_values_non_zero.append(complex(-r, i))
        if i != 0.:
            test_values_non_zero.append(complex(r, -i))
        if r != 0. and i != 0.:
            test_values_non_zero.append(complex(-r, -i))

functions = [
    ('log10', log10, test_values_non_zero),
]

for f_name, f, test_vals in functions:
    print(f_name)
    for val in test_vals:
        ret = f(val)
        print("complex(%.5g, %.5g)" % (ret.real, ret.imag))
