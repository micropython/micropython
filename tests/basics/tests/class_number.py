# test class with __add__ and __sub__ methods

class C:
    def __init__(self, value):
        self.value = value

    def __add__(self, rhs):
        print(self.value, '+', rhs)

    def __sub__(self, rhs):
        print(self.value, '-', rhs)

c = C(0)
c + 1
c - 2
