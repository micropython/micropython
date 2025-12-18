# Test that the slice-on-stack optimisation does not break various uses of slice
# (see MP_TYPE_FLAG_SUBSCR_ALLOWS_STACK_SLICE type option).
#
# Note: this test has a corresponding .py.exp file because hashing slice objects
# was not allowed in CPython until 3.12.

try:
    from collections import OrderedDict
except ImportError:
    print("SKIP")
    raise SystemExit

# Attempt to index with a slice, error should contain the slice (failed key).
try:
    dict()[:]
except KeyError as e:
    print("KeyError", e.args)

# Put a slice and another object into an OrderedDict, and retrieve them.
x = OrderedDict()
x[:"a"] = 1
x["b"] = 2
print(list(x.keys()), list(x.values()))
