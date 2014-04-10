l = [1, 3, 2, 5]

print(l)
print(sorted(l))
l.sort()
print(l)
print(l == sorted(l))

print(sorted(l, key=lambda x: -x))
l.sort(key=lambda x: -x)
print(l)
print(l == sorted(l, key=lambda x: -x))

print(sorted(l, key=lambda x: -x, reverse=True))
l.sort(key=lambda x: -x, reverse=True)
print(l)
print(l == sorted(l, key=lambda x: -x, reverse=True))

print(sorted(l, reverse=True))
l.sort(reverse=True)
print(l)
print(l == sorted(l, reverse=True))

print(sorted(l, reverse=False))
l.sort(reverse=False)
print(l)
print(l == sorted(l, reverse=False))

