# test the math functions that return ints

try:
    import math
except ImportError:
    print("SKIP")
    import sys
    sys.exit()

for fun in (math.ceil, math.floor, math.trunc):
    for x in (-1.6, -0.2, 0, 0.6, 1.4, float('inf'), float('nan')):
        try:
            print(fun(x))
        except (ValueError, OverflowError) as e:
            print(type(e))
