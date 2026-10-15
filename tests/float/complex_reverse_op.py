# test complex interacting with special reverse methods
# requires: reverse-op


class A:
    def __radd__(self, x):
        print("__radd__")
        return 2


print(1j + A())
