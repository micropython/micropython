# test complex interacting with special methods


class A:
    def __add__(self, x):
        print("__add__")
        return 1

    def __radd__(self, x):
        print("__radd__")
        return 2


print(A() + 1j)
print(1j + A())
