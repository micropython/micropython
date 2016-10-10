:mod:`cmath` --- Mathematical functions for complex numbers
===========================================================

.. module:: cmath
   :synopsis: Mathematical functions for complex numbers.

This module provides access to mathematical functions for complex
numbers. The functions in this module accept integers, floating-point
numbers or complex numbers as arguments. They will also accept any
Python object that has either a ``__complex__()`` or a ``__float__()``
method: these methods are used to convert the object to a complex or
floating-point number, respectively, and the function is then applied
to the result of the conversion.

----------------------------------------------


.. function:: phase(x)

   Return the phase of `x` (also known as the argument of `x`), as a
   float. ``phase(x)`` is equivalent to ``math.atan2(x.imag,
   x.real)``. The result lies in the range `[-π, π]`, and the branch
   cut for this operation lies along the negative real axis,
   continuous from above. On systems with support for signed zeros
   (which includes most systems in current use), this means that the
   sign of the result is the same as the sign of ``x.imag``, even when
   ``x.imag`` is zero:

   .. code-block:: python

      >>> phase(complex(-1.0, 0.0))
      3.141592
      >>> phase(complex(-1.0, -0.0))
      -3.14159


.. function:: polar(x)

   Return the representation of `x` in polar coordinates. Returns a
   pair ``(r, phi)`` where r is the modulus of `x` and phi is the
   phase of x. ``polar(x)`` is equivalent to ``(abs(x), phase(x))``.


.. function:: rect(r, phi)

    Return the complex number x with polar coordinates `r` and
    `phi`. Equivalent to ``r * (math.cos(phi) + math.sin(phi)*1j)``.


.. function:: exp(x)

   Return the exponential value ``e**x``.


.. function:: log(x)

   Returns the natural logarithm of `x`. There is one branch cut, from
   0 along the negative real axis to -∞, continuous from above.


.. function:: sqrt(x)

   Return the square root of `x`. This has the same branch cut as
   ``log()``.


.. function:: cos(x)

   Return the cosine of `x`.


.. function:: sin(x)

   Return the sine of `x`.


.. function:: e

   The mathematical constant e, as a float.


.. function:: pi

   The mathematical constant π, as a float.
