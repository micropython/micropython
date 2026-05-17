# test complex interacting with special methods


class A:
    def __add__(self, x):
        print("__add__")
        return 1


print(A() + 1j)
