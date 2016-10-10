:mod:`math` --- Mathematical functions
======================================

.. module:: math
   :synopsis: Mathematical functions.

This module provides access to the mathematical functions defined by
the C standard.

----------------------------------------------


.. function:: math.ceil(x)

   Return the ceiling of `x`, the smallest integer greater than or
   equal to `x`. If `x` is not a float, delegates to ``x.__ceil__()``,
   which should return an Integral value.


.. function:: math.copysign(x, y)

   Return a float with the magnitude (absolute value) of `x` but the
   sign of `y`. On platforms that support signed zeros, ``copysign(1.0,
   -0.0)`` returns -1.0.


.. function:: math.fabs(x)

   Return the absolute value of `x`.


.. function:: math.floor(x)

   Return the floor of `x`, the largest integer less than or equal to
   `x`. If `x` is not a float, delegates to ``x.__floor__()``, which
   should return an Integral value.


.. function:: math.fmod(x, y)

   Return ``fmod(x, y)``, as defined by the platform C library. Note
   that the Python expression ``x % y`` may not return the same
   result. The intent of the C standard is that ``fmod(x, y)`` be
   exactly (mathematically; to infinite precision) equal to ``x -
   n*y`` for some integer n such that the result has the same sign as
   `x` and magnitude less than ``abs(y)``. Python’s ``x % y`` returns
   a result with the sign of `y` instead, and may not be exactly
   computable for float arguments. For example, ``fmod(-1e-100,
   1e100)`` is ``-1e-100``, but the result of Python’s ``-1e-100 %
   1e100`` is ``1e100-1e-100``, which cannot be represented exactly as
   a float, and rounds to the surprising ``1e100``. For this reason,
   function ``fmod()`` is generally preferred when working with
   floats, while Python’s ``x % y`` is preferred when working with
   integers.


.. function:: math.frexp(x)

   Return the mantissa and exponent of `x` as the pair ``(m, e)``. m
   is a float and e is an integer such that ``x == m * 2**e``
   exactly. If `x` is zero, returns ``(0.0, 0)``, otherwise ``0.5 <=
   abs(m) < 1``. This is used to “pick apart” the internal
   representation of a float in a portable way.


.. function:: math.isinf(x)

   Return ``True`` if `x` is a positive or negative infinity, and
   ``False`` otherwise.


.. function:: math.isnan(x)

   Return ``True`` if `x` is a NaN (not a number), and ``False`` otherwise.


.. function:: math.ldexp(x, i)

   Return ``x * (2**i)``. This is essentially the inverse of function `frexp()`.


.. function:: math.modf(x)

   Return the fractional and integer parts of `x`. Both results carry
   the sign of `x` and are floats.


.. function:: math.trunc(x)

   Return the Real value `x` truncated to an Integral (usually an
   integer). Delegates to ``x.__trunc__()``.


.. function:: math.log(x[, base])

   With one argument, return the natural logarithm of `x` (to base e).

   With two arguments, return the logarithm of `x` to the given
   `base`, calculated as ``log(x)/log(base)``.


.. function:: math.pow(x, y)

   Return `x` raised to the power `y`. If both `x` and `y` are finite,
   `x` is negative, and `y` is not an integer then ``pow(x, y)`` is
   undefined, and raises ValueError.

   Unlike the built-in ** operator, ``math.pow()`` converts both its
   arguments to type float. Use ** or the built-in ``pow()`` function for
   computing exact integer powers.


.. function:: math.sqrt(x)

   Return the square root of `x`.


.. function:: math.acos(x)

   Return the arc cosine of `x`, in radians.


.. function:: math.asin(x)

   Return the arc sine of `x`, in radians.


.. function:: math.atan(x)

   Return the arc tangent of `x`, in radians.


.. function:: math.atan2(y, x)

   Return ``atan(y/x)``, in radians. The result is between -π and
   π. The vector in the plane from the origin to point ``(x, y)``
   makes this angle with the positive X axis. The point of ``atan2()``
   is that the signs of both inputs are known to it, so it can compute
   the correct quadrant for the angle. For example, ``atan(1)`` and
   ``atan2(1, 1)`` are both ``pi/4``, but ``atan2(-1, -1)`` is
   ``-3*pi/4``.


.. function:: math.cos(x)

   Return the cosine of `x` radians.


.. function:: math.sin(x)

   Return the sine of `x` radians.


.. function:: math.tan(x)

   Return the tangent of `x` radians.


.. function:: math.degrees(x)

   Convert angle `x` from radians to degrees.


.. function:: math.radians(x)

   Convert angle `x` from degrees to radians.


.. function:: math.pi

   The mathematical constant π = 3.141592..., to available precision.


.. function:: math.e

   The mathematical constant e = 2.718281..., to available precision.
