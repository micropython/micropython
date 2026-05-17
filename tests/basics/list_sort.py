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

# test large lists (should not stack overflow)
l = list(range(200))
l.sort()
print(l[0], l[-1])
l.sort(reverse=True)
print(l[0], l[-1])

# test user-defined ordering
class A:
    def __init__(self, x):
        self.x = x
    def __lt__(self, other):
        return self.x > other.x
    def __repr__(self):
        return str(self.x)
l = [A(5), A(2), A(1), A(3), A(4)]
print(l)
l.sort()
print(l)
l.sort(reverse=True)
print(l)
