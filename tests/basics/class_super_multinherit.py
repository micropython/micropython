# test super with multiple inheritance

class A:
    def foo(self):
        print('A.foo')

class B:
    def foo(self):
        print('B.foo')

class C(A, B):
    def foo(self):
        print('C.foo')
        super().foo()

C().foo()
