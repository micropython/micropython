def report(s):
    l = list(s)
    l.sort()
    print(l)

s = {1}
s.update()
report(s)
s.update([2])
report(s)
s.update([1,3], [2,2,4])
report(s)
