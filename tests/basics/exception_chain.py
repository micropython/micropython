# Exception chaining is not supported, but check that basic
# exception works as expected.
try:
    raise Exception from None
except Exception:
    print("Caught Exception")
