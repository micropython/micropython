:mod:`math` -- mathematical functions
=====================================

.. module:: math
   :synopsis: mathematical functions

|see_cpython_module| :mod:`python:math`.

The ``math`` module provides some basic mathematical functions for
working with floating-point numbers.

*Note:* On the pyboard, floating-point numbers have 32-bit precision.

Availability: not available on WiPy. Floating point support required
for this module.

Functions
---------

.. function:: acos(x)

   Return the inverse cosine of ``x``.

.. function:: acosh(x)

   Return the inverse hyperbolic cosine of ``x``.

.. function:: asin(x)

   Return the inverse sine of ``x``.

.. function:: asinh(x)

   Return the inverse hyperbolic sine of ``x``.

.. function:: atan(x)

   Return the inverse tangent of ``x``.

.. function:: atan2(y, x)

   Return the principal value of the inverse tangent of ``y/x``.

.. function:: atanh(x)

   Return the inverse hyperbolic tangent of ``x``.

.. function:: ceil(x)

   Return an integer, being ``x`` rounded towards positive infinity.

.. function:: copysign(x, y)

   Return ``x`` with the sign of ``y``.

.. function:: cos(x)

   Return the cosine of ``x``.

.. function:: cosh(x)

   Return the hyperbolic cosine of ``x``.

.. function:: degrees(x)

   Return radians ``x`` converted to degrees.

.. function:: erf(x)

   Return the error function of ``x``.

.. function:: erfc(x)

   Return the complementary error function of ``x``.

.. function:: exp(x)

   Return the exponential of ``x``.

.. function:: expm1(x)

   Return ``exp(x) - 1``.

.. function:: fabs(x)

   Return the absolute value of ``x``.

.. function:: floor(x)

   Return an integer, being ``x`` rounded towards negative infinity.

.. function:: fmod(x, y)

   Return the remainder of ``x/y``.

.. function:: frexp(x)

   Decomposes a floating-point number into its mantissa and exponent.
   The returned value is the tuple ``(m, e)`` such that ``x == m * 2**e``
   exactly.  If ``x == 0`` then the function returns ``(0.0, 0)``, otherwise
   the relation ``0.5 <= abs(m) < 1`` holds.

.. function:: gamma(x)

   Return the gamma function of ``x``.

.. function:: isfinite(x)

   Return ``True`` if ``x`` is finite.

.. function:: isinf(x)

   Return ``True`` if ``x`` is infinite.

.. function:: isnan(x)

   Return ``True`` if ``x`` is not-a-number

.. function:: ldexp(x, exp)

   Return ``x * (2**exp)``.

.. function:: lgamma(x)

   Return the natural logarithm of the gamma function of ``x``.

.. function:: log(x)
              log(x, base)

   With one argument, return the natural logarithm of *x*.

   With two arguments, return the logarithm of *x* to the given *base*.

.. function:: log10(x)

   Return the base-10 logarithm of ``x``.

.. function:: log2(x)

   Return the base-2 logarithm of ``x``.

.. function:: modf(x)

   Return a tuple of two floats, being the fractional and integral parts of
   ``x``.  Both return values have the same sign as ``x``.

.. function:: pow(x, y)

   Returns ``x`` to the power of ``y``.

.. function:: radians(x)

   Return degrees ``x`` converted to radians.

.. function:: sin(x)

   Return the sine of ``x``.

.. function:: sinh(x)

   Return the hyperbolic sine of ``x``.

.. function:: sqrt(x)

   Return the square root of ``x``.

.. function:: tan(x)

   Return the tangent of ``x``.

.. function:: tanh(x)

   Return the hyperbolic tangent of ``x``.

.. function:: trunc(x)

   Return an integer, being ``x`` rounded towards 0.

Constants
---------

.. data:: e

   base of the natural logarithm

.. data:: pi

   the ratio of a circle's circumference to its diameter
