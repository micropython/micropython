# basic class

def go():
    class C:
        def f():
            print(1)

        def g(self):
            print(2)

        def set(self, value):
            self.value = value

        def print(self):
            print(self.value)

    C.f()
    C()
    C().g()

    o = C()
    o.set(3)
    o.print()

    C.set(o, 4)
    C.print(o)

go()
