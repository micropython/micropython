# Tests the functions imported from math

try:
    from math import *
except ImportError:
    print("SKIP")
    import sys
    sys.exit()

test_values = [-100., -1.23456, -1, -0.5, 0.0, 0.5, 1.23456, 100.]
test_values_small = [-10., -1.23456, -1, -0.5, 0.0, 0.5, 1.23456, 10.] # so we don't overflow 32-bit precision
p_test_values = [0.1, 0.5, 1.23456]
unit_range_test_values = [-1., -0.75, -0.5, -0.25, 0., 0.25, 0.5, 0.75, 1.]

functions = [('sqrt', sqrt, p_test_values),
             ('exp', exp, test_values_small),
             ('expm1', expm1, test_values_small),
             ('log', log, p_test_values),
             ('log2', log2, p_test_values),
             ('log10', log10, p_test_values),
             ('cosh', cosh, test_values_small),
             ('sinh', sinh, test_values_small),
             ('tanh', tanh, test_values_small),
             ('acosh', acosh, [1.0, 5.0, 1.0]),
             ('asinh', asinh, test_values),
             ('atanh', atanh, [-0.99, -0.5, 0.0, 0.5, 0.99]),
             ('cos', cos, test_values),
             ('sin', sin, test_values),
             ('tan', tan, test_values),
             ('acos', acos, unit_range_test_values),
             ('asin', asin, unit_range_test_values),
             ('atan', atan, test_values),
             ('ceil', ceil, test_values),
             ('fabs', fabs, test_values),
             ('floor', floor, test_values),
             ('trunc', trunc, test_values)
            ]

for function_name, function, test_vals in functions:
    print(function_name)
    for value in test_vals:
        print("{:.5g}".format(function(value)))

tuple_functions = [('frexp', frexp, test_values),
                   ('modf', modf, test_values),
                  ]

for function_name, function, test_vals in tuple_functions:
    print(function_name)
    for value in test_vals:
        x, y = function(value)
        print("{:.5g} {:.5g}".format(x, y))

binary_functions = [('copysign', copysign, [(23., 42.), (-23., 42.), (23., -42.),
                                (-23., -42.), (1., 0.0), (1., -0.0)])
                   ]

for function_name, function, test_vals in binary_functions:
    print(function_name)
    for value1, value2 in test_vals:
        print("{:.7g}".format(function(value1, value2)))
