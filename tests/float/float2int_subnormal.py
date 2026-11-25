# Test that subnormal float32s are correctly converted to int (esp. on LONGLONG)
# requires: float

e = 1e-40
print(e != 0)
print(int(e))

e = -1e-40
print(e != 0)
print(int(e))
