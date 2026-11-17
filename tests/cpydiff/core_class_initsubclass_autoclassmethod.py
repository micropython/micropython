"""
categories: Core,Classes
description: ``__init_subclass__`` isn't an implicit classmethod.
cause: MicroPython does not currently implement PEP 487. ``__init_subclass__`` is not currently in the list of special-cased class/static methods.
workaround: Decorate declarations of ``__init_subclass__`` with ``@classmethod``.
"""


def regularize_spelling(text, prefix="bound_"):
    # for regularizing across the CPython "method" vs MicroPython "bound_method" spelling for the type of a bound classmethod
    if text.startswith(prefix):
        return text[len(prefix) :]
    return text


class A:
    def __init_subclass__(cls):
        pass

    @classmethod
    def manual_decorated(cls):
        pass


a = type(A.__init_subclass__).__name__
b = type(A.manual_decorated).__name__

print(regularize_spelling(a))
print(regularize_spelling(b))
if a != b:
    print("FAIL")
