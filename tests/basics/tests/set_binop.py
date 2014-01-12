def r(s):
    l = list(s)
    l.sort()
    print(l)
s = {1, 2}
t = {2, 3}
r(s | t)
r(s ^ t)
r(s & t)
r(s - t)
u = s.copy()
u |= t
r(u)
u = s.copy()
u ^= t
r(u)
u = s.copy()
u &= t
r(u)
u = s.copy()
u -= t
r(u)

print(s == t)
print(s != t)
print(s > t)
print(s >= t)
print(s < t)
print(s <= t)
