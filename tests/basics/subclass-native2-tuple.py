class Base1:
    def __init__(self, *args):
        print("Base1.__init__", args)

class Ctuple1(Base1, tuple):
    pass

a = Ctuple1()
print(len(a))
a = Ctuple1([1, 2, 3])
print(len(a))

print("---")

class Ctuple2(tuple, Base1):
    pass

a = Ctuple2()
print(len(a))
a = Ctuple2([1, 2, 3])
print(len(a))
