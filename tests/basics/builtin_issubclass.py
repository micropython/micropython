# test builtin issubclass

class A:
    pass

print(issubclass(A, A))
print(issubclass(A, (A,)))

try:
    issubclass(A, 1)
except TypeError:
    print('TypeError')

try:
    issubclass('a', 1)
except TypeError:
    print('TypeError')
