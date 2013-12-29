class A(B):
    def f():
        super.a()

class B(C):
    def g():
        def h():
            super.a()

super.a()

def i():
    super.a()

def j():
    def k():
        super.a()
