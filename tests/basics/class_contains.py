# A contains everything
class A:
    def __contains__(self, key):
        return True

a = A()
print(True in a)
print(1 in a)
print(() in a)

# B contains given things
class B:
    def __init__(self, items):
        self.items = items
    def __contains__(self, key):
        return key in self.items

b = B([])
print(1 in b)
b = B([1, 2])
print(1 in b)
print(2 in b)
print(3 in b)


class C:
    def __contains__(self, arg):
        return arg


print(C().__contains__(0))
print(C().__contains__(1))
print(C().__contains__(''))
print(C().__contains__('foo'))
print(C().__contains__(None))

print(0 in C())
print(1 in C())
print('' in C())
print('foo' in C())
print(None in C())

print(0 not in C())
print(1 not in C())
print('' not in C())
print('foo' not in C())
print(None not in C())
