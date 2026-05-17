# This tests that recursion with iternext doesn't lead to segfault.
#
# This test segfaults CPython, but that's not a bug as CPython doesn't enforce
# limits on C recursion - see
# https://github.com/python/cpython/issues/58218#issuecomment-1093570209
try:
    enumerate
    filter
    map
    max
    zip
except:
    print("SKIP")
    raise SystemExit


# Progressively build a bigger nested iterator structure (10 at a time for speed),
# and then try to evaluate it via tuple(x) which makes deep recursive function calls.
#
# Eventually this should raise a RuntimeError as MicroPython runs out of stack.
# It shouldn't ever raise a MemoryError, if it does then somehow MicroPython has
# run out of heap (for the nested structure) before running out of stack.
def recurse_iternext(nested_fn):
    x = (1, 2)
    while True:
        for _ in range(10):
            x = nested_fn(x)
        try:
            tuple(x)
        except RuntimeError:
            print("RuntimeError")
            break


# Test on various nested iterator structures
for nested_fn in [enumerate, lambda x: filter(None, x), lambda x: map(max, x, ()), zip]:
    recurse_iternext(nested_fn)
