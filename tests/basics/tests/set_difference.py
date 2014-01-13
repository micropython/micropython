def report(s):
    l = list(s)
    l.sort()
    print(l)

l = [1, 2, 3, 4]
s = set(l)
outs = [s.difference(),
        s.difference({1}),
        s.difference({1}, [1, 2]),
        s.difference({1}, {1, 2}, {2, 3})]
for out in outs:
    report(out)

s = set(l)
print(s.difference_update())
report(s)
print(s.difference_update({1}))
report(s)
print(s.difference_update({1}, [2]))
report(s)
