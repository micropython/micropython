try:
    frozenset
except NameError:
    print("SKIP")
    raise SystemExit

sets = [
    frozenset(), frozenset({1}), frozenset({1, 2}), frozenset({1, 2, 3}), frozenset({2, 3}),
    frozenset({2, 3, 5}), frozenset({5}), frozenset({7})
]
for s in sets:
    for t in sets:
        print(sorted(s), '|', sorted(t), '=', sorted(s | t))
        print(sorted(s), '^', sorted(t), '=', sorted(s ^ t))
        print(sorted(s), '&', sorted(t), '=', sorted(s & t))
        print(sorted(s), '-', sorted(t), '=', sorted(s - t))
        u = s.copy()
        u |= t
        print(sorted(s), "|=", sorted(t), '-->', sorted(u))
        u = s.copy()
        u ^= t
        print(sorted(s), "^=", sorted(t), '-->', sorted(u))
        u = s.copy()
        u &= t
        print(sorted(s), "&=", sorted(t), "-->", sorted(u))
        u = s.copy()
        u -= t
        print(sorted(s), "-=", sorted(t), "-->", sorted(u))

        print(sorted(s), '==', sorted(t), '=', s == t)
        print(sorted(s), '!=', sorted(t), '=', s != t)
        print(sorted(s), '>', sorted(t), '=', s > t)
        print(sorted(s), '>=', sorted(t), '=', s >= t)
        print(sorted(s), '<', sorted(t), '=', s < t)
        print(sorted(s), '<=', sorted(t), '=', s <= t)
