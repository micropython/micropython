# inheritance

class A:
    def a():
        print('A.a() called')

class B(A):
    pass

print(type(A))
print(type(B))

print(issubclass(A, A))
print(issubclass(A, B))
print(issubclass(B, A))
print(issubclass(B, B))

print(isinstance(A(), A))
print(isinstance(A(), B))
print(isinstance(B(), A))
print(isinstance(B(), B))

A.a()
B.a()
