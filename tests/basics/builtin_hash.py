# test builtin hash function

print(hash(False))
print(hash(True))
print({():1}) # hash tuple
print({1 << 66:1}) # hash big int
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
