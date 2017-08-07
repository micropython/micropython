try:
    frozenset
except NameError:
    print("SKIP")
    raise SystemExit

s = frozenset({1, 2, 3, 4})
t = s.copy()
print(type(t))
for i in s, t:
    print(sorted(i))
