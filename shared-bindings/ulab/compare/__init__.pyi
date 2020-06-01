"""Comparison functions"""

def clip(x1, x2, x3):
    """
    Constrain the values from ``x1`` to be between ``x2`` and ``x3``.
    ``x2`` is assumed to be less than or equal to ``x3``.

    Arguments may be ulab arrays or numbers.  All array arguments
    must be the same size.  If the inputs are all scalars, a 1-element
    array is returned.

    Shorthand for ``ulab.maximum(x2, ulab.minimum(x1, x3))``"""
    ...

def maximum(x1, x2):
    """
    Compute the element by element maximum of the arguments.

    Arguments may be ulab arrays or numbers.  All array arguments
    must be the same size.  If the inputs are both scalars, a number is
    returned"""
    ...

def minimum(x1, x2):
    """Compute the element by element minimum of the arguments.

    Arguments may be ulab arrays or numbers.  All array arguments
    must be the same size.  If the inputs are both scalars, a number is
    returned"""
    ...

def equal(x1, x2):
    """Return an array of bool which is true where x1[i] == x2[i] and false elsewhere"""
    ...

def not_equal(x1, x2):
    """Return an array of bool which is false where x1[i] == x2[i] and true elsewhere"""
    ...
