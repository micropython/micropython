# test round() with floats that return large integers
# requires: float int-big

for x in (-1e25, 1e25):
    print("%.3g" % round(x))
