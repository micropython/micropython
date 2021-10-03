# test urandom.seed() without any arguments

try:
    import urandom as random
except ImportError:
    try:
        import random
    except ImportError:
        print("SKIP")
        raise SystemExit

try:
    random.seed()
except ValueError:
    # no default seed on this platform
    print("SKIP")
    raise SystemExit


def rng_seq():
    return [random.getrandbits(16) for _ in range(10)]


# seed with default and check that doesn't produce the same RNG sequence
random.seed()
seq = rng_seq()
random.seed()
print(seq == rng_seq())
random.seed(None)
print(seq == rng_seq())
