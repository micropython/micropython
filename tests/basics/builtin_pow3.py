# test builtin pow() with integral values
# 3 arg version

try:
    print(pow(3, 4, 7))
except NotImplementedError:
    print("SKIP")
    raise SystemExit

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
