"""
categories: Core,Classes
description: Micropython calls __init_subclass__ directly for all direct base classes.
cause: Micropython can't rely on __init_subclass__ implementations calling super().__init_subclass__() to recurse through ancestor classes like CPython.
workaround: Omit calling super().__init_subclass__() in __init_subclass__ implementations.
"""


# In CPython, only the first base__init_subclass__
class A1:
    a = "A1"

    def __init_subclass__(cls):
        print("A1 init_subclass", cls.__name__)
        cls.aa = "AA"

    print("class A1")


class B1:
    b = "B1"

    def __init_subclass__(cls):
        print("B1 init_subclass", cls.__name__)
        cls.bb = "BB"

    print("class B1")


class C1(A1, B1):
    c = "C1"
    print("class C1")


# In CPython it's specified to call super().__init_subclass__() in __init_subclass__.
# But the presence of super() makes the invocation of __init_subclass__ fail.
class A2:
    a = "A2"

    @classmethod
    def __init_subclass__(cls):
        super().__init_subclass__()
        print("A2 init_subclass", cls.__name__)
        cls.aa = "AA"

    print("class A2")


class B2:
    b = "B2"

    @classmethod
    def __init_subclass__(cls):
        super().__init_subclass__()
        print("B2 init_subclass", cls.__name__)
        cls.bb = "BB"

    print("class B2")


class C2(A2, B2):
    c = "C2"
    print("class C2")


def print_check_all_attrs(obj, name: str, attrs: "list[str]"):
    for attr in attrs:
        print(name, attr, getattr(obj, attr, "missing"))


print_check_all_attrs(A1, "A1", ["a", "aa", "b", "bb", "c"])
print_check_all_attrs(B1, "B1", ["a", "aa", "b", "bb", "c"])
print_check_all_attrs(C1, "C1", ["a", "aa", "b", "bb", "c"])

print_check_all_attrs(A2, "A2", ["a", "aa", "b", "bb", "c"])
print_check_all_attrs(B2, "B2", ["a", "aa", "b", "bb", "c"])
print_check_all_attrs(C2, "C2", ["a", "aa", "b", "bb", "c"])
