# test containment operator on subclass of a native type

class mylist(list):
    pass

class mydict(dict):
    pass

class mybytes(bytes):
    pass

l = mylist([1, 2, 3])
print(0 in l)
print(1 in l)

d = mydict({1:1, 2:2})
print(0 in l)
print(1 in l)

b = mybytes(b'1234')
print(0 in b)
print(b'1' in b)
