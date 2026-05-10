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

# Slice-as-key in OrderedDict: when backed by the ordered hash table
# (MICROPY_PY_MAP_ORDERED=1) slices are not hashable so TypeError is raised.
# When backed by linear scan (MAP_ORDERED=0) slices work as keys.
x = OrderedDict()
try:
    x[:"a"] = 1
    x["b"] = 2
    # Linear scan path: verify keys and values are correct.
    assert list(x.keys()) == [slice(None, "a", None), "b"]
    assert list(x.values()) == [1, 2]
    print("slice key OK")
except TypeError:
    # Hash table path: slices not hashable, this is expected.
    print("slice key TypeError")
