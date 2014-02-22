class Base:

    def meth(self):
        print("in Base meth")

class Sub(Base):

    def meth(self):
        print("in Sub meth")
        return super().meth()

a = Sub()
a.meth()
