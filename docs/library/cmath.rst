:mod:`cmath` -- mathematical functions for complex numbers
==========================================================

.. module:: cmath
   :synopsis: mathematical functions for complex numbers

|see_cpython_module| :mod:`python:cmath`.

The ``cmath`` module provides some basic mathematical functions for
working with complex numbers.

Availability: not available on WiPy and ESP8266. Floating point support
required for this module.

Functions
---------

.. function:: cos(z)

   Return the cosine of ``z``.

.. function:: exp(z)

   Return the exponential of ``z``.

.. function:: log(z)

   Return the natural logarithm of ``z``.  The branch cut is along the negative real axis.

.. function:: log10(z)

   Return the base-10 logarithm of ``z``.  The branch cut is along the negative real axis.

.. function:: phase(z)

   Returns the phase of the number ``z``, in the range (-pi, +pi].

.. function:: polar(z)

   Returns, as a tuple, the polar form of ``z``.

.. function:: rect(r, phi)

   Returns the complex number with modulus ``r`` and phase ``phi``.

.. function:: sin(z)

   Return the sine of ``z``.

.. function:: sqrt(z)

   Return the square-root of ``z``.

Constants
---------

.. data:: e

   base of the natural logarithm

.. data:: pi

   the ratio of a circle's circumference to its diameter
