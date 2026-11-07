# test math functions converting int subclass instances to float

try:
    import math
except ImportError:
    print("SKIP")
    raise SystemExit


class MyInt(int):
    pass


print(math.sqrt(MyInt(4)))

# errors from the converted value behave as for a plain int
try:
    math.sqrt(MyInt(-1))
except ValueError:
    print("ValueError")
