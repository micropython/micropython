"""
categories: Core,Classes
description: When inheriting from multiple classes super() only calls one class
cause: See :ref:`cpydiff_core_class_mro`
workaround: See :ref:`cpydiff_core_class_mro`
"""


class A:
    def __init__(self):
        print("A.__init__")


class B(A):
    def __init__(self):
        print("B.__init__")
        super().__init__()


class C(A):
    def __init__(self):
        print("C.__init__")
        super().__init__()


class D(B, C):
    def __init__(self):
        print("D.__init__")
        super().__init__()


D()
