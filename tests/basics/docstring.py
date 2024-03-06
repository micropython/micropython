# Test docstrings with the __doc__ special attribute.

# This is the module's docstring.
"""Module docstring"""


def func_no_docstring():
    pass


def func_with_docstring():
    """Function docstring"""


class A:
    """Class docstring"""

    def method(self):
        """Method docstring"""


if not hasattr(A, "__doc__"):
    print("SKIP")
    raise SystemExit

print(func_no_docstring.__doc__)
print(func_with_docstring.__doc__)

print(A.__doc__)
print(A().__doc__)
print(A.method.__doc__)
print(A().method.__doc__)

print(globals()["__doc__"])
