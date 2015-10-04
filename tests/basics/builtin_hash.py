# test builtin hash function

print(hash(False))
print(hash(True))
print({():1}) # hash tuple
print({1 << 66:1}) # hash big int
print({-(1 << 66):2}) # hash negative big int
print(hash in {hash:1}) # hash function

try:
    hash([])
except TypeError:
    print("TypeError")

class A:
    def __hash__(self):
        return 123
    def __repr__(self):
        return "a instance"

print(hash(A()))
print({A():1})

# all user-classes have default __hash__
class B:
    pass
hash(B())

# if __eq__ is defined then default __hash__ is not used
class C:
    def __eq__(self, another):
        return True
try:
    hash(C())
except TypeError:
    print("TypeError")

# __hash__ must return an int
class D:
    def __hash__(self):
        return None
try:
    hash(D())
except TypeError:
    print("TypeError")

# __hash__ returning a bool should be converted to an int
class E:
    def __hash__(self):
        return True
print(hash(E()))

# __hash__ returning a large number should be truncated
class F:
    def __hash__(self):
        return 1 << 70 | 1
print(hash(F()) != 0)
