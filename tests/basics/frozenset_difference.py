try:
    frozenset
except NameError:
    print("SKIP")
    raise SystemExit

l = [1, 2, 3, 4]
s = frozenset(l)
outs = [s.difference(),
        s.difference(frozenset({1})),
        s.difference(frozenset({1}), [1, 2]),
        s.difference(frozenset({1}), {1, 2}, {2, 3})]
for out in outs:
    print(type(out), sorted(out))

s = frozenset(l)
try:
    print(s.difference_update({1}))
except AttributeError:
    print("AttributeError")
