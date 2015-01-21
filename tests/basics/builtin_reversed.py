# test the builtin reverse() function

# list
print(list(reversed([])))
print(list(reversed([1])))
print(list(reversed([1, 2, 3])))

# tuple
print(list(reversed(())))
print(list(reversed((1, 2, 3))))

# string
for c in reversed('ab'):
    print(c)

# bytes
for b in reversed(b'1234'):
    print(b)

# range
for i in reversed(range(3)):
    print(i)

# user object
class A:
    def __init__(self):
        pass
    def __len__(self):
        return 3
    def __getitem__(self, pos):
        return pos + 1
for a in reversed(A()):
    print(a)

# user object with __reversed__
class B:
    def __reversed__(self):
        return [1, 2, 3]
print(reversed(B()))
