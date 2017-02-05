# test builtin pow() with integral values
# 3 arg version

try:
    print(pow(3, 4, 7))
except NotImplementedError:
    import sys
    print("SKIP")
    sys.exit()

print(pow(555557, 1000002, 1000003))

# 3 arg pow is defined to only work on integers
try:
    print(pow("x", 5, 6))
except TypeError:
    print("TypeError expected")

try:
    print(pow(4, "y", 6))
except TypeError:
    print("TypeError expected")

try:
    print(pow(4, 5, "z"))
except TypeError:
    print("TypeError expected")

# Tests for 3 arg pow with large values

# This value happens to be prime
x = 0xd48a1e2a099b1395895527112937a391d02d4a208bce5d74b281cf35a57362502726f79a632f063a83c0eba66196712d963aa7279ab8a504110a668c0fc38a7983c51e6ee7a85cae87097686ccdc359ee4bbf2c583bce524e3f7836bded1c771a4efcb25c09460a862fc98e18f7303df46aaeb34da46b0c4d61d5cd78350f3edb60e6bc4befa712a849
y = 0x3accf60bb1a5365e4250d1588eb0fe6cd81ad495e9063f90880229f2a625e98c59387238670936afb2cafc5b79448e4414d6cd5e9901aa845aa122db58ddd7b9f2b17414600a18c47494ed1f3d49d005a5

print(hex(pow(2, 200, x))) # Should not overflow, just 1 << 200
print(hex(pow(2, x-1, x))) # Should be 1, since x is prime
print(hex(pow(y, x-1, x))) # Should be 1, since x is prime
print(hex(pow(y, y-1, x))) # Should be a 'big value'
print(hex(pow(y, y-1, y))) # Should be a 'big value'
