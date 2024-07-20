"""
categories: Core,Classes
description: Micropython can't recurse through base classes for __init_subclass__.
cause: Micropython doesn't support super() inside classmethods.
workaround: Unknown
"""


class A:
    a = "A"

    def __init_subclass__(cls):
        print("A init_subclass", cls.__name__)
        cls.aa = "AA"

    print("class A")


class B(A):
    b = "B"

    def __init_subclass__(cls):
        super().__init_subclass__()
        print("B init_subclass", cls.__name__)
        cls.bb = "BB"

    print("class B")


class C(B):
    c = "C"
    print("class C")


def print_check_all_attrs(obj, name: str, attrs: "list[str]"):
    for attr in attrs:
        print(name, attr, getattr(obj, attr, "missing"))


print_check_all_attrs(A, "A", ["a", "aa", "b", "bb", "c"])
print_check_all_attrs(B, "B", ["a", "aa", "b", "bb", "c"])
print_check_all_attrs(C, "C", ["a", "aa", "b", "bb", "c"])
