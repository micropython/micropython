# test [0,1,-1] edge cases of bignum

long_zero = (2**64) >> 65
long_neg_zero = -long_zero
long_one = long_zero + 1
long_neg_one = -long_one

cases = [long_zero, long_neg_zero, long_one, long_neg_one]

print(cases)
print([-c for c in cases])
print([~c for c in cases])
print([c >> 1 for c in cases])
print([c << 1 for c in cases])

# comparison of 0
print(long_zero == 0)
print(long_neg_zero == 0)
print(long_one - 1 == 0)
print(long_neg_one + 1 == 0)
print(long_zero < 1)
print(long_zero < -1)
print(long_zero > 1)
print(long_zero > -1)
print(long_neg_zero < 1)
print(long_neg_zero < -1)
print(long_neg_zero > 1)
print(long_neg_zero > -1)

# generate zeros that involve negative numbers
large = 1 << 70
large_plus_one = large + 1
zeros = (
    large - large,
    -large + large,
    large + -large,
    -(large - large),
    large - large_plus_one + 1,
    -large & (large - large),
    -large ^ -large,
    -large * (large - large),
    (large - large) // -large,
    -large // -large_plus_one,
    -(large + large) % large,
    (large + large) % -large,
    -(large + large) % -large,
)
print(zeros)

# compute arithmetic operations that may have problems with -0
# (this checks that -0 is never generated in the zeros tuple)
cases = (0, 1, -1) + zeros
for lhs in cases:
    print("-{} = {}".format(lhs, -lhs))
    print("~{} = {}".format(lhs, ~lhs))
    print("{} >> 1 = {}".format(lhs, lhs >> 1))
    print("{} << 1 = {}".format(lhs, lhs << 1))
    for rhs in cases:
        print("{} == {} = {}".format(lhs, rhs, lhs == rhs))
        print("{} + {} = {}".format(lhs, rhs, lhs + rhs))
        print("{} - {} = {}".format(lhs, rhs, lhs - rhs))
        print("{} * {} = {}".format(lhs, rhs, lhs * rhs))
        print("{} | {} = {}".format(lhs, rhs, lhs | rhs))
        print("{} & {} = {}".format(lhs, rhs, lhs & rhs))
        print("{} ^ {} = {}".format(lhs, rhs, lhs ^ rhs))
