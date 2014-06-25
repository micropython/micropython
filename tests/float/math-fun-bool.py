# Test the bool functions from math

try:
    from math import isfinite, isnan, isinf
except ImportError:
    print("SKIP")
    import sys
    sys.exit()

test_values = [1, 0, -1, 1.0, 0.0, -1.0, float('NaN'), float('Inf'), 
               -float('NaN'), -float('Inf')]

functions = [isfinite, isnan, isinf]

for val in test_values:
    for f in functions:
        print(f(val))
