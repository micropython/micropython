l = [1, 3, 2, 5]
print(l)
l.sort()
print(l)
l.sort(key=lambda x: -x)
print(l)
l.sort(key=lambda x: -x, reverse=True)
print(l)
l.sort(reverse=True)
print(l)
l.sort(reverse=False)
print(l)

