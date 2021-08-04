:mode:`urandom` -- generates random numbers
===========================================

.. module:: urandom
   :synopsis: random numbers

This module implements pseudo-random number generator. The functions that
these module provides are bond methods of a hidden number of :random.Random:
class.   

Given below are the methods that this module provides.

  .. method:: urandom 

    Returns an n-bit pseudo-random number(output is determinsitic).

  .. method:: urandom.getrandbits(n)

    Returns an integer with 'n' random bits (1<=n<=32)

  .. method:: urandom.seed(n)
  
    initialises random number generator module with n.

  .. method:: urandom.randint(a,b)
    
    returns a random interger in the range [a,b].
    
  .. method:: urandom.randrange(stop)
  
    returns a random number from the range [0,stop)

  .. method:: urandom.randrange(start,stop)
    
    returns a random number from range [start,stop]

  .. method:: urandom.randrange(start,stop,step)
  
    returns a random numner from range [start,stop] after skipping *step*
    numbers                    

  .. method:: urandom.random()
  
    returns the next floating point number in the range [0.0,1.0]

  .. method:: urandom.uniform(a,b)
  
    returns a random floating point number N such that a<=N<=b for a<=b and
    b<=N<=a for b<a    
