"""Numerical and Statistical functions

Most of these functions take an "axis" argument, which indicates whether to
operate over the flattened array (None), rows (0), or columns (1)."""

def argmax(array, *, axis=None):
    """Return the index of the maximum element of the 1D array"""
    ...

def argmin(array, *, axis=None):
    """Return the index of the minimum element of the 1D array"""
    ...

def argsort(array, *, axis=None):
    """Returns an array which gives indices into the input array from least to greatest."""
    ...

def diff(array, *, axis=1):
    """Return the numerical derivative of successive elements of the array, as
       an array.  axis=None is not supported."""
    ...

def flip(array, *, axis=None):
    """Returns a new array that reverses the order of the elements along the
       given axis, or along all axes if axis is None."""
    ...

def max(array, *, axis=None):
    """Return the maximum element of the 1D array"""
    ...

def mean(array, *, axis=None):
    """Return the mean element of the 1D array, as a number if axis is None, otherwise as an array."""
    ...

def min(array, *, axis=None):
    """Return the minimum element of the 1D array"""
    ...

def roll(array, distance, *, axis=None):
    """Shift the content of a vector by the positions given as the second
       argument. If the ``axis`` keyword is supplied, the shift is applied to
       the given axis.  The array is modified in place."""
    ...

def std(array, *, axis=None):
    """Return the standard deviation of the array, as a number if axis is None, otherwise as an array."""
    ...

def sum(array, *, axis=None):
    """Return the sum of the array, as a number if axis is None, otherwise as an array."""
    ...

def sort(array, *, axis=0):
    """Sort the array along the given axis, or along all axes if axis is None.
       The array is modified in place."""
    ...
