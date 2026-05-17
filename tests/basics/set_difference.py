l = [1, 2, 3, 4]
s = set(l)
outs = [s.difference(),
        s.difference({1}),
        s.difference({1}, [1, 2]),
        s.difference({1}, {1, 2}, {2, 3})]
for out in outs:
    print(sorted(out))

s = set(l)
print(s.difference_update())
print(sorted(s))
print(s.difference_update({1}))
print(sorted(s))
print(s.difference_update({1}, [2]))
print(sorted(s))

s.difference_update(s)
print(s)
