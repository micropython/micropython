"""Manipulate numeric data similar to numpy

`ulab` is a numpy-like module for micropython, meant to simplify and
speed up common mathematical operations on arrays. The primary goal was to
implement a small subset of numpy that might be useful in the context of a
microcontroller. This means low-level data processing of linear (array) and
two-dimensional (matrix) data.

`ulab` is adapted from micropython-ulab, and the original project's
documentation can be found at
https://micropython-ulab.readthedocs.io/en/latest/

`ulab` is modeled after numpy, and aims to be a compatible subset where
possible.  Numpy's documentation can be found at
https://docs.scipy.org/doc/numpy/index.html"""


class array:
    """1- and 2- dimensional array"""
    def __init__(self, values, *, dtype=float):
        """:param sequence values: Sequence giving the initial content of the array.
          :param dtype: The type of array values, ``int8``, ``uint8``, ``int16``, ``uint16``, or ``float``

          The `values` sequence can either be another ~ulab.array, sequence of numbers
          (in which case a 1-dimensional array is created), or a sequence where each
          subsequence has the same length (in which case a 2-dimensional array is
          created).

          Passing a ~ulab.array and a different dtype can be used to convert an array
          from one dtype to another.

          In many cases, it is more convenient to create an array from a function
          like `zeros` or `linspace`.

          `ulab.array` implements the buffer protocol, so it can be used in many
          places an `array.array` can be used."""
        ...

    shape: tuple = ...
    """The size of the array, a tuple of length 1 or 2"""

    size: int = ...
    """The number of elements in the array"""

    itemsize: int = ...
    """The number of elements in the array"""

    def flatten(self, *, order='C'):
        """:param order: Whether to flatten by rows ('C') or columns ('F')

           Returns a new `ulab.array` object which is always 1 dimensional.
           If order is 'C' (the default", then the data is ordered in rows;
           If it is 'F', then the data is ordered in columns.  "C" and "F" refer
           to the typical storage organization of the C and Fortran languages."""
        ...

    def sort(self, *, axis=1):
        """:param axis: Whether to sort elements within rows (0), columns (1), or elements (None)"""
        ...

    def transpose(self):
        """Swap the rows and columns of a 2-dimensional array"""
        ...

    def  __add__(self):
        """Adds corresponding elements of the two arrays, or adds a number to all
           elements of the array.  If both arguments are arrays, their sizes must match."""
        ...

    def  __sub__(self):
        """Subtracts corresponding elements of the two arrays, or adds a number to all
           elements of the array.  If both arguments are arrays, their sizes must match."""
        ...

    def  __mul__(self):
        """Multiplies corresponding elements of the two arrays, or multiplies
           all elements of the array by a number.  If both arguments are arrays,
           their sizes must match."""
        ...

    def __div__(self):
        """Multiplies corresponding elements of the two arrays, or divides
           all elements of the array by a number.  If both arguments are arrays,
           their sizes must match."""
        ...

    def __pow__():
        """Computes the power (x**y) of corresponding elements of the the two arrays,
           or one number and one array.  If both arguments are arrays, their sizes
           must match."""
        ...

    def __getitem__():
        """Retrieve an element of the array."""
        ...

    def __setitem__():
        """Set an element of the array."""
        ...

int8 = ...
"""Type code for signed integers in the range -128 .. 127 inclusive, like the 'b' typecode of `array.array`"""

int16 = ...
"""Type code for signed integers in the range -32768 .. 32767 inclusive, like the 'h' typecode of `array.array`"""

float = ...
"""Type code for floating point values, like the 'f' typecode of `array.array`"""

uint8 = ...
"""Type code for unsigned integers in the range 0 .. 255 inclusive, like the 'H' typecode of `array.array`"""

uint16 = ...
"""Type code for unsigned integers in the range 0 .. 65535 inclusive, like the 'h' typecode of `array.array`"""

def ones(shape, *, dtype=float):
    """
    .. param: shape
       Shape of the array, either an integer (for a 1-D array) or a tuple of 2 integers (for a 2-D array)

    .. param: dtype
       Type of values in the array

    Return a new array of the given shape with all elements set to 1."""
    ...

def zeros(shape, *, dtype):
    """
    .. param: shape
       Shape of the array, either an integer (for a 1-D array) or a tuple of 2 integers (for a 2-D array)

    .. param: dtype
       Type of values in the array

    Return a new array of the given shape with all elements set to 0."""
    ...


def eye(size, *, dtype=float):
    """Return a new square array of size, with the diagonal elements set to 1
       and the other elements set to 0."""
    ...

def linspace(start, stop, *, dtype=float, num=50, endpoint=True):
    """
    .. param: start

      First value in the array

    .. param: stop

      Final value in the array

    .. param int: num

      Count of values in the array

    .. param: dtype

      Type of values in the array

    .. param bool: endpoint

      Whether the ``stop`` value is included.  Note that even when
      endpoint=True, the exact ``stop`` value may not be included due to the
      inaccuracy of floating point arithmetic.

    Return a new 1-D array with ``num`` elements ranging from ``start`` to ``stop`` linearly."""
    ...
