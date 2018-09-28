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

    # subscript special methods wrapped in staticmethod
    @staticmethod
    def __getitem__(item):
        print('static get', item)
        return 'item'
    @staticmethod
    def __setitem__(item, value):
        print('static set', item, value)
    @staticmethod
    def __delitem__(item):
        print('static del', item)

c = C()

c.f(0)
c.g(0)
c - 1
c + 2
print(c[1])
c[1] = 2
del c[3]
