# check that consts are not replaced in anything except standalone identifiers

from micropython import const

X = const(1)
Y = const(2)
Z = const(3)

# import that uses a constant
import micropython as X

print(globals()["X"])


# function name that matches a constant
def X():
    print("function X", X)


globals()["X"]()


# arguments that match a constant
def f(X, *Y, **Z):
    pass


f(1)


# class name that matches a constant
class X:
    def f(self):
        print("class X", X)


globals()["X"]().f()


# constant within a class
class A:
    C1 = const(4)

    def X(self):
        print("method X", Y, C1, self.C1)


A().X()
