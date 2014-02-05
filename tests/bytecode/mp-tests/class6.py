class A:
    def f(self):
        pass

class B(A):
    def f(self):
        super().f()
