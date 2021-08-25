:mode:`urandom` -- generates random numbers
===========================================

.. module:: urandom
   :synopsis: random numbers

This module implements a pseudo-random number generator.

Function for integers
---------------------
 
  .. method:: urandom.getrandbits(n)

    Returns an integer with *n* random bits (0<=n<=32).
    
  .. method:: urandom.randint(a, b)
    
    Returns a random integer in the range [*a*, *b*].
    
  .. method:: urandom.randrange(stop)
  
    Returns a random integer from the range [0, *stop*).

Other Functions
---------------
  
  .. method:: urandom.choice(sequence)
   
    Chooses multiple random items from a *sequence* (list  or 
    any data structure).
    
  .. method:: urandom.seed(n=None, /)
  
    Initialises the random number generator module with n. The None case only 
    works if :MICROPY_PY_URANDOM_SEED_INIT_FUNC: is defined in the port, 
    otherwise raises ValueError. When no argument (or *None*) is passed in it
    will (if supported by the port) initialise the PRNG with a true random 
    number (usually a hardware generated random number).

  .. method:: urandom.randrange(start, stop)
    
    Returns a random number from range [*start*, *stop*).

  .. method:: urandom.randrange(start, stop[, step])
  
    Returns a random number from range [*start*, *stop*] in steps of *step*. 
    For instance, calling urandom.randrange(1, 10, 2) will return 
    odd numbers between 1 and 9 inclusive.

  .. method:: urandom.random()
  
    Returns a floating point number in the range [0.0, 1.0).

  .. method:: urandom.uniform(a, b)
  
    Returns a random floating point number N such that a<=N<=b for *a* <=*b* and 
    *b* <= N <= *a* for *b* < *a*.  
    
.. note:: 

   The :randrange: , :randint: and :choice: may not be available on some ports if 
   the :MICROPY_PY_URANDOM_EXTRA_FUNCS: configuration option is disabled.
   
