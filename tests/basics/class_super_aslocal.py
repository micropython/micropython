# test using the name "super" as a local variable

class A:
    def foo(self):
        super = [1, 2]
        super.pop()
        print(super)

A().foo()
