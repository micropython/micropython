# test complex interacting with special reverse methods


class A:
    def __radd__(self, x):
        print("__radd__")
        return 2


print(1j + A())
