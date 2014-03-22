# Tests the functions imported from math

from math import *

test_values = [-100., -1.23456, -1, -0.5, 0.0, 0.5, 1.23456, 100.]
p_test_values = [0.1, 0.5, 1.23456]
unit_range_test_values = [-1., -0.75, -0.5, -0.25, 0., 0.25, 0.5, 0.75, 1.]

functions = [(sqrt, p_test_values),
             (exp, test_values),
             (expm1, test_values),
             (log, p_test_values),
             (log2, p_test_values),
             (log10, p_test_values),   
             (cosh, test_values), 
             (sinh, test_values), 
             (tanh, test_values),
             (acosh, [1.0, 5.0, 1.0]),
             (asinh, test_values),
             (atanh, [-0.99, -0.5, 0.0, 0.5, 0.99]), 
             (cos, test_values), 
             (sin, test_values), 
             (tan, test_values),
             (acos, unit_range_test_values),
             (asin, unit_range_test_values),
             (atan, test_values),
             (ceil, test_values),
             (fabs, test_values),
             (floor, test_values),
             #(frexp, test_values),
             (trunc, test_values)
            ]

for function, test_vals in functions:
    for value in test_vals:
        print("{:8.7f}".format(function(value)))

binary_functions = [(copysign, [(23., 42.), (-23., 42.), (23., -42.), 
                                (-23., -42.), (1., 0.0), (1., -0.0)])
                   ]

for function, test_vals in binary_functions:
    for value1, value2 in test_vals:
        print("{:8.7f}".format(function(value1, value2)))


