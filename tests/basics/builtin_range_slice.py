# test builtin range type slicing difficult cases
# These should pass if
#define MICROPY_PY_BUILTINS_SLICE_RANGES_CORRECTLY (1)

try:
    range(4)[::0]
except ValueError:
    print("ValueError")
else:
    print("SKIP")
    import sys
    sys.exit(0)

print(range(1,4)[::-1])
print(range(7,-2,-4)[2:-2:])
print(range(7,-2,-4)[2:-2:-3])

