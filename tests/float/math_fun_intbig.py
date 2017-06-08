# test the math functions that return ints, with very large results

try:
    import math
except ImportError:
    print("SKIP")
    raise SystemExit

for fun in (math.ceil, math.floor, math.trunc):
    for x in (-1e25, 1e25):
        print('%.3g' % fun(x))
