:mod:`random` --- Generate pseudo-random numbers
================================================

.. module:: random
   :synopsis: Generate pseudo-random numbers.

This module implements pseudo-random number generators for various
distributions.

----------------------------------------------


.. function:: random.seed(a=None)

   Initialize the random number generator.

   If `a` is omitted or ``None``, the current system time is used. If
   randomness sources are provided by the operating system, they are
   used instead of the system time (see the ``os.urandom()`` function
   for details on availability).

   If `a` is an int, it is used directly.


.. function:: random.getrandbits(k)

   Returns a Python integer with `k` random bits. This method is
   supplied with the MersenneTwister generator and some other
   generators may also provide it as an optional part of the API. When
   available, ``getrandbits()`` enables ``randrange()`` to handle
   arbitrarily large ranges.


.. function:: random.randrange(stop)
.. function:: random.randrange(start, stop[, step])

   Return a randomly selected element from ``range(start, stop,
   step)``. This is equivalent to ``choice(range(start, stop,
   step))``, but doesn’t actually build a range object.

   The positional argument pattern matches that of
   ``range()``. Keyword arguments should not be used because the
   function may use them in unexpected ways.

   .. code-block:: python

      >>> random.randrange(10)           # Integer from 0 to 9
      7
      >>> random.randrange(0, 101, 2)    # Even integer from 0 to 100
      26


.. function:: random.randint(a, b)

   Return a random integer N such that `a` <= N <= `b`. Alias for
   ``randrange(a, b+1)``.


.. function:: random.choice(seq)

   Return a random element from the non-empty sequence `seq`. If `seq`
   is empty, raises IndexError.

   .. code-block:: python

      >>> random.choice('abcdefghij')    # Single random element
      'c'


.. function:: random.random()

   Return the next random floating point number in the range ``[0.0,
   1.0]``.

   .. code-block:: python

      >>> random.random()                # Random float x, 0.0 <= x < 1.0
      0.374448


.. function:: random.uniform(a, b)

   Return a random floating point number N such that `a` <= N <= `b`
   for `a` <= `b` and `b` <= N <= `a` for `b` < `a`.

   The end-point value `b` may or may not be included in the range
   depending on floating-point rounding in the equation ``a + (b-a) *
   random()``.

   .. code-block:: python

      >>> random.uniform(1, 10)          # Random float x, 1.0 <= x < 10.0
      1.180014
