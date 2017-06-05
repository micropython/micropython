def Fun(a=42):
    loc = locals()
    print(loc['a'])
    return loc['a']


class A:

    def __init__(self):
        pass

    def Fun(self, a=42):
        loc = locals()
        print(loc['a'])
        return loc['a']

Fun() == 42

a = A()
a.Fun() == 42
