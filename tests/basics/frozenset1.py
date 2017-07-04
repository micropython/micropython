# basic sets

try:
    frozenset
except NameError:
    print("SKIP")
    raise SystemExit

s = frozenset()
print(s)

s = frozenset({1})
print(s)

s = frozenset({3, 4, 3, 1})
print(sorted(s))

# frozensets are hashable unlike sets
print({frozenset("1"): 2})
