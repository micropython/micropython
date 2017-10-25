# test [0,-0,1,-1] edge cases of bignum

import skip_if
skip_if.no_bigint()

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

# comparison of 0/-0/+0
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
