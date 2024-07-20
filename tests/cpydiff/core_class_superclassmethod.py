"""
categories: Core,Classes
description: super inside a classmethod defers to type rather than parent class.
cause: Unknown
workaround: Unknown
"""


class A:
    @classmethod
    def f(cls):
        print("Af", cls.__name__)

    @classmethod
    def g(cls):
        print("Ag", cls.__name__)


class B(A):
    @classmethod
    def f(cls):
        print("Bf", cls.__name__)
        super().f()

    @classmethod
    def g(cls):
        print("Bg", cls.__name__)
        super(B, cls).g()


class C(B):
    @classmethod
    def f(cls):
        print("Cf", cls.__name__)
        super().f()

    @classmethod
    def g(cls):
        print("Cg", cls.__name__)
        super(C, cls).g()


c = C()
c.f()
c.g()
