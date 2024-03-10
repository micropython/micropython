# Test dictionary union.
#
# The tests sort the resulting dictionaries for now, since map insertion
# order is not implemented in MicroPython.

try:
    {} | {}
except TypeError:
    print("SKIP")
    raise SystemExit


def print_sorted_dict(d):
    print(sorted(d.items()))


def test_union(a, b):
    print_sorted_dict(a | b)
    print_sorted_dict(b | a)
    a |= a
    print_sorted_dict(a)
    a |= b
    print_sorted_dict(a)


d = {}
e = {}
test_union(d, e)

d = {1: "apple"}
e = {1: "cheese"}
test_union(d, e)

d = {"spam": 1, "eggs": 2, "cheese": 3}
e = {"cheese": "cheddar", "aardvark": "Ethel"}
test_union(d, e)
