def report(s):
    l = list(s)
    l.sort()
    print(l)

s = {1, 2, 3, 4}
report(s)
report(s.intersection({1, 3}))
report(s.intersection([3, 4]))

print(s.intersection_update([1]))
report(s)
