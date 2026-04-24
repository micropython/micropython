# test builtin pow() with integral values
# 3 arg version

try:
    print(pow(3, 4, 7))
except NotImplementedError:
    print("SKIP")
    raise SystemExit

# test some edge cases
print(pow(1, 1, 1))
print(pow(0, 1, 1))
print(pow(1, 0, 1))
print(pow(1, 0, 2))

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

# bool acts as int in all three positions (CPython compat)
print(pow(True, 1, 1))
print(pow(1, True, 1))
print(pow(1, 1, True))
print(pow(False, 5, 7))
print(pow(2, False, 7))
print(pow(2, 5, True))
