from synthio import *

for values in ((-1, 4, 9), (4, -1, 9), (9, 4, -1), (0, 0, 0), (3, 2, 1), (2, 1), (1,)):
    for op in MathOperation.__dict__.values():
        o = Math(op, *values)
        print(op, o)
        print(lfo_tick(o))
    print()
