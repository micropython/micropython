# Test docstrings with the __doc__ special attribute.

# This is the module's docstring.
"""module docstring"""


def func_no_docstring():
    pass


def func_with_docstring():
    """func_with_docstring docstring"""


def func_with_docstring_and_statement():
    """func_with_docstring_and_statement docstring"""
    return 1


class A:
    """A docstring"""

    def method(self):
        """A.method docstring"""


if not hasattr(A, "__doc__"):
    print("SKIP")
    raise SystemExit

print(func_no_docstring.__doc__)
print(func_with_docstring.__doc__)
print(func_with_docstring_and_statement.__doc__)

print(A.__doc__)
print(A().__doc__)
print(A.method.__doc__)
print(A().method.__doc__)

print(globals()["__doc__"])
