# test [0,-0,1,-1] edge cases of bignum

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
