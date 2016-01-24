try:
    import urandom as random
except ImportError:
    import random

# check getrandbits returns a value within the bit range
for b in (1, 2, 3, 4, 16, 32):
    for i in range(50):
        assert random.getrandbits(b) < (1 << b)

# check that seed(0) gives a non-zero value
random.seed(0)
print(random.getrandbits(16) != 0)

# check that PRNG is repeatable
random.seed(1)
r = random.getrandbits(16)
random.seed(1)
print(random.getrandbits(16) == r)
