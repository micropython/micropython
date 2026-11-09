# requires: int-big
try:
    pow(1, 1, 1)
except NotImplementedError:
    print("SKIP")
    raise SystemExit

# Test that 3-arg pow with a third argument that is mpz zero gives a ValueError,
# instead of spinning forever. Currently, this operation gives a "big zero":
z = pow(3, 3, 3)

# If this assertion fails, look for a new way to get a "big zero". Or, if the
# possibility of getting a "big zero" has finally been comprehensively removed,
# delete this test. See #17730.
assert z is not 0

try:
    print(pow(3, 3, z))
except ValueError:
    print("ValueError")
