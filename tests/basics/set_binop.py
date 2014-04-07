def r(s):
    l = list(s)
    l.sort()
    return l
sets = [set(), {1}, {1, 2}, {1, 2, 3}, {2, 3}, {2, 3, 5}, {5}, {7}]
for s in sets:
    for t in sets:
        print(r(s), '|', r(t), '=', r(s | t))
        print(r(s), '^', r(t), '=', r(s ^ t))
        print(r(s), '&', r(t), '=', r(s & t))
        print(r(s), '-', r(t), '=', r(s - t))
        u = s.copy()
        u |= t
        print(r(s), "|=", r(t), '-->', r(u))
        u = s.copy()
        u ^= t
        print(r(s), "^=", r(t), '-->', r(u))
        u = s.copy()
        u &= t
        print(r(s), "&=", r(t), "-->", r(u))
        u = s.copy()
        u -= t
        print(r(s), "-=", r(t), "-->", r(u))

        print(r(s), '==', r(t), '=', s == t)
        print(r(s), '!=', r(t), '=', s != t)
        print(r(s), '>', r(t), '=', s > t)
        print(r(s), '>=', r(t), '=', s >= t)
        print(r(s), '<', r(t), '=', s < t)
        print(r(s), '<=', r(t), '=', s <= t)
