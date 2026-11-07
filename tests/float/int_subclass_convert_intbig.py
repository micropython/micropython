# test conversion of int subclass instances holding a big int to float
# requires: int-big


class MyInt(int):
    pass


# values too large for a small int are stored as a heap int object;
# conversion must give the same result as for a plain int
print(float(MyInt(2**70)) == float(2**70))
print(float(MyInt(-(2**70))) == float(-(2**70)))
print(float(MyInt(12345678901234567890)) == float(12345678901234567890))

# use a variable so the addition is a runtime operation on both sides;
# written inline, mpy-cross folds it into a float constant, and the text
# form of that constant may not parse back to the same value on targets
# whose float parser is not correctly rounded
big = 2**70
print(1.5 + MyInt(big) == 1.5 + big)
