# test builtin hash function

class A:
    def __hash__(self):
        return 123
    def __repr__(self):
        return "a instance"

print(hash(A()))
print({A():1})
