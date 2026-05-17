# test list.__iadd__ and list.extend (they are equivalent)

l = [1, 2]
l.extend([])
print(l)

l.extend([3])
print(l)

l.extend([4, 5])
print(l)

l.extend(range(6, 10))
print(l)

l.extend("abc")
print(l)

l = [1, 2]
l += []
print(l)

l += [3]
print(l)

l += [4, 5]
print(l)

l += range(6, 10)
print(l)

l += "abc"
print(l)
