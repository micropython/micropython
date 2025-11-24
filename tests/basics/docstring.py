# Test docstrings with the __doc__ special attribute.

# This is the module's docstring.
"""Module docstring"""


class A:
    """Class docstring"""


if not hasattr(A, "__doc__"):
    print("SKIP")
    raise SystemExit

print(A.__doc__)
print(A().__doc__)

print(globals()["__doc__"])
