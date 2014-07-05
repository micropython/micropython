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
