
:mod:`ulab` --- Manipulate numeric data similar to numpy
========================================================

.. module:: ulab
   :synopsis: Manipulate numeric data similar to numpy

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
https://docs.scipy.org/doc/numpy/index.html

.. contents::

.. attribute:: __version__

The closest corresponding version of micropython-ulab

ulab.array -- 1- and 2- dimensional array
-----------------------------------------

.. class:: ulab.array(values, \*, dtype=float)

  :param sequence values: Sequence giving the initial content of the array.
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
  places an `array.array` can be used.

   .. attribute:: shape

      The size of the array, a tuple of length 1 or 2

   .. attribute:: size

      The number of elements in the array

   .. attribute:: itemsize

      The number of elements in the array

   .. method:: flatten(\*, order='C')

      :param order: Whether to flatten by rows ('C') or columns ('F')

      Returns a new `ulab.array` object which is always 1 dimensional.
      If order is 'C' (the default", then the data is ordered in rows;
      If it is 'F', then the data is ordered in columns.  "C" and "F" refer
      to the typical storage organization of the C and Fortran languages.

   .. method:: sort(\*, axis=1)

      :param axis: Whether to sort elements within rows (0), columns (1), or elements (None)

   .. method:: transpose()

      Swap the rows and columns of a 2-dimensional array

   .. method:: __add__()

      Adds corresponding elements of the two arrays, or adds a number to all
      elements of the array.  If both arguments are arrays, their sizes must match.

   .. method:: __sub__()

      Subtracts corresponding elements of the two arrays, or adds a number to all
      elements of the array.  If both arguments are arrays, their sizes must match.

   .. method:: __mul__()

      Multiplies corresponding elements of the two arrays, or multiplies
      all elements of the array by a number.  If both arguments are arrays,
      their sizes must match.

   .. method:: __div__()

      Multiplies corresponding elements of the two arrays, or divides
      all elements of the array by a number.  If both arguments are arrays,
      their sizes must match.

   .. method:: __pow__()

      Computes the power (x**y) of corresponding elements of the the two arrays,
      or one number and one array.  If both arguments are arrays, their sizes
      must match.

   .. method:: __getitem__()

      Retrieve an element of the array.

   .. method:: __setitem__()

      Set an element of the array.

Array type codes
----------------
.. attribute:: int8

   Type code for signed integers in the range -128 .. 127 inclusive, like the 'b' typecode of `array.array`

.. attribute:: int16

   Type code for signed integers in the range -32768 .. 32767 inclusive, like the 'h' typecode of `array.array`

.. attribute:: float

   Type code for floating point values, like the 'f' typecode of `array.array`

.. attribute:: uint8

   Type code for unsigned integers in the range 0 .. 255 inclusive, like the 'H' typecode of `array.array`

.. attribute:: uint16

   Type code for unsigned integers in the range 0 .. 65535 inclusive, like the 'h' typecode of `array.array`


Basic Array defining functions
------------------------------

.. method:: ones(shape, \*, dtype=float)

   .. param: shape
      Shape of the array, either an integer (for a 1-D array) or a tuple of 2 integers (for a 2-D array)

   .. param: dtype
      Type of values in the array

   Return a new array of the given shape with all elements set to 1.

.. method:: zeros

   .. param: shape
      Shape of the array, either an integer (for a 1-D array) or a tuple of 2 integers (for a 2-D array)

   .. param: dtype
      Type of values in the array

   Return a new array of the given shape with all elements set to 0.


.. method:: eye(size, \*, dtype=float)

   Return a new square array of size, with the diagonal elements set to 1
   and the other elements set to 0.

.. method:: linspace(start, stop, \*, dtype=float, num=50, endpoint=True)

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

   Return a new 1-D array with ``num`` elements ranging from ``start`` to ``stop`` linearly.


:mod:`ulab.compare` --- Comparison functions
============================================

.. module::ulab.compare

.. method:: clip(x1, x2, x3)

   Constrain the values from ``x1`` to be between ``x2`` and ``x3``.
   ``x2`` is assumed to be less than or equal to ``x3``.

   Arguments may be ulab arrays or numbers.  All array arguments
   must be the same size.  If the inputs are all scalars, a 1-element
   array is returned.

   Shorthand for ``ulab.maximum(x2, ulab.minimum(x1, x3))``

.. method:: maximum(x1, x2)

   Compute the element by element maximum of the arguments.

   Arguments may be ulab arrays or numbers.  All array arguments
   must be the same size.  If the inputs are both scalars, a number is
   returned

.. method:: minimum(x1, x2)

   Compute the element by element minimum of the arguments.

   Arguments may be ulab arrays or numbers.  All array arguments
   must be the same size.  If the inputs are both scalars, a number is
   returned


:mod:`ulab.vector` --- Element-by-element functions
===================================================

.. module:: ulab.vector

These functions can operate on numbers, 1-D arrays, or 2-D arrays by
applying the function to every element in the array.  This is typically
much more efficient than expressing the same operation as a Python loop.

.. method:: acos

   Computes the inverse cosine function

.. method:: acosh

   Computes the inverse hyperbolic cosine function

.. method:: asin

   Computes the inverse sine function

.. method:: asinh

   Computes the inverse hyperbolic sine function

.. method:: around(a, \*, decimals)

   Returns a new float array in which each element is rounded to
   ``decimals`` places.

.. method:: atan

   Computes the inverse tangent function; the return values are in the
   range [-pi/2,pi/2].

.. method:: atan2(y,x)

   Computes the inverse tangent function of y/x; the return values are in
   the range [-pi, pi].

.. method:: atanh

   Computes the inverse hyperbolic tangent function

.. method:: ceil

   Rounds numbers up to the next whole number

.. method:: cos

   Computes the cosine function

.. method:: erf

   Computes the error function, which has applications in statistics

.. method:: erfc

   Computes the complementary error function, which has applications in statistics

.. method:: exp

   Computes the exponent function.

.. method:: expm1

   Computes $e^x-1$.  In certain applications, using this function preserves numeric accuracy better than the `exp` function.

.. method:: floor

   Rounds numbers up to the next whole number

.. method:: gamma

   Computes the gamma function

.. method:: lgamma

   Computes the natural log of the gamma function

.. method:: log

   Computes the natural log

.. method:: log10

   Computes the log base 10

.. method:: log2

   Computes the log base 2

.. method:: sin

   Computes the sine

.. method:: sinh

   Computes the hyperbolic sine

.. method:: sqrt

   Computes the square root

.. method:: tan

   Computes the tangent

.. method:: tanh

   Computes the hyperbolic tangent

:mod:`ulab.linalg` - Linear algebra functions
=============================================

.. module:: ulab.linalg

.. method:: cholesky(A)

   :param ~ulab.array A: a positive definite, symmetric square matrix
   :return ~ulab.array L: a square root matrix in the lower triangular form
   :raises ValueError: If the input does not fulfill the necessary conditions

   The returned matrix satisfies the equation m=LL*

.. method:: det

   :param: m, a square matrix
   :return float: The determinant of the matrix

   Computes the eigenvalues and eigenvectors of a square matrix

.. method:: dot(m1, m2)

   :param ~ulab.array m1: a matrix
   :param ~ulab.array m2: a matrix

   Computes the matrix product of two matrices

   **WARNING:** Unlike ``numpy``, this function cannot be used to compute the dot product of two vectors

.. method:: eig(m)

   :param m: a square matrix
   :return tuple (eigenvectors, eigenvalues):

   Computes the eigenvalues and eigenvectors of a square matrix

.. method:: inv(m)

   :param ~ulab.array m: a square matrix
   :return: The inverse of the matrix, if it exists
   :raises ValueError: if the matrix is not invertible

   Computes the inverse of a square matrix

.. method:: size(array)

   Return the total number of elements in the array, as an integer.

.. method:: trace(m)

   :param m: a square matrix

   Compute the trace of the matrix, the sum of its diagonal elements.

:mod:`ulab.filter` --- Filtering functions
==========================================

.. module:: ulab.filter

.. method:: convolve(r, c=None)

   :param ulab.array a:
   :param ulab.array v:

   Returns the discrete, linear convolution of two one-dimensional sequences.
   The result is always an array of float.  Only the ``full`` mode is supported,
   and the ``mode`` named parameter of numpy is not accepted. Note that all other
   modes can be had by slicing a ``full`` result.

   Convolution filters can implement high pass, low pass, band pass, etc.,
   filtering operations.  Convolution filters are typically constructed ahead
   of time.  This can be done using desktop python with scipy, or on web pages
   such as https://fiiir.com/

   Convolution is most time-efficient when both inputs are of float type.

:mod:`ulab.fft` --- Frequency-domain functions
==============================================

.. module:: ulab.fft

.. method:: fft(r, c=None)

   :param ulab.array r: A 1-dimension array of values whose size is a power of 2
   :param ulab.array c: An optional 1-dimension array of values whose size is a power of 2, giving the complex part of the value
   :return tuple (r, c): The real and complex parts of the FFT

   Perform a Fast Fourier Transform from the time domain into the frequency domain

    See also ~ulab.extras.spectrum, which computes the magnitude of the fft,
    rather than separately returning its real and imaginary parts.

.. method:: ifft(r, c=None)

   :param ulab.array r: A 1-dimension array of values whose size is a power of 2
   :param ulab.array c: An optional 1-dimension array of values whose size is a power of 2, giving the complex part of the value
   :return tuple (r, c): The real and complex parts of the inverse FFT

   Perform an Inverse Fast Fourier Transform from the frequeny domain into the time domain

:mod:`ulab.numerical` --- Numerical and Statistical functions
=============================================================

.. module:: ulab.numerical

Most of these functions take an "axis" argument, which indicates whether to
operate over the flattened array (None), rows (0), or columns (1).

.. method:: argmax(array, \*, axis=None)

   Return the index of the maximum element of the 1D array

.. method:: argmin(array, \*, axis=None)

   Return the index of the minimum element of the 1D array

.. method:: argsort(array, \*, axis=None)

   Returns an array which gives indices into the input array from least to greatest.

.. method:: diff(array, \*, axis=1)

   Return the numerical derivative of successive elements of the array, as
   an array.  axis=None is not supported.

.. method:: flip(array, \*, axis=None)

   Returns a new array that reverses the order of the elements along the
   given axis, or along all axes if axis is None.

.. method:: max(array, \*, axis=None)

   Return the maximum element of the 1D array

.. method:: mean(array, \*, axis=None)

   Return the mean element of the 1D array, as a number if axis is None, otherwise as an array.

.. method:: min(array, \*, axis=None)

   Return the minimum element of the 1D array

.. method:: roll(array, distance, \*, axis=None)

   Shift the content of a vector by the positions given as the second
   argument. If the ``axis`` keyword is supplied, the shift is applied to
   the given axis.  The array is modified in place.

.. method:: std(array, \*, axis=None)

   Return the standard deviation of the array, as a number if axis is None, otherwise as an array.

.. method:: sum(array, \*, axis=None)

   Return the sum of the array, as a number if axis is None, otherwise as an array.

.. method:: sort(array, \*, axis=0)

   Sort the array along the given axis, or along all axes if axis is None.
   The array is modified in place.

:mod:`ulab.poly` --- Polynomial functions
=========================================

.. module:: ulab.poly

.. method:: polyfit([x, ] y, degree)

   Return a polynomial of given degree that approximates the function
   f(x)=y.  If x is not supplied, it is the range(len(y)).

.. method:: polyval(p, x)

   Evaluate the polynomial p at the points x.  x must be an array.

:mod:`ulab.extras` --- Additional functions not in numpy
========================================================

.. method:: spectrum(r):

   :param ulab.array r: A 1-dimension array of values whose size is a power of 2

   Computes the spectrum of the input signal.  This is the absolute value of the (complex-valued) fft of the signal.

   This function is similar to scipy's ``scipy.signal.spectrogram``.
