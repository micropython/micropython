a = b"123"
print(a)
print(str(a))
print(repr(a))
print(a[0], a[2])
print(a[-1])

s = 0
for i in a:
    s += i
print(s)
