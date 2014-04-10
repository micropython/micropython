# test static and class methods

class C:
    @staticmethod
    def f(rhs):
        print('f', rhs)
    @classmethod
    def g(self, rhs):
        print('g', rhs)

    # builtin wrapped in staticmethod
    @staticmethod
    def __sub__(rhs):
        print('sub', rhs)
    # builtin wrapped in classmethod
    @classmethod
    def __add__(self, rhs):
        print('add', rhs)

c = C()

c.f(0)
c.g(0)
c - 1
c + 2
