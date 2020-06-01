"""Element-by-element functions

These functions can operate on numbers, 1-D arrays, or 2-D arrays by
applying the function to every element in the array.  This is typically
much more efficient than expressing the same operation as a Python loop."""

def acos():
    """Computes the inverse cosine function"""
    ...

def acosh():
    """Computes the inverse hyperbolic cosine function"""
    ...

def asin():
    """Computes the inverse sine function"""
    ...

def asinh():
    """Computes the inverse hyperbolic sine function"""
    ...

def around(a, *, decimals):
    """Returns a new float array in which each element is rounded to
       ``decimals`` places."""
    ...

def atan():
    """Computes the inverse tangent function; the return values are in the
       range [-pi/2,pi/2]."""
    ...

def atan2(y,x):
    """Computes the inverse tangent function of y/x; the return values are in
       the range [-pi, pi]."""
    ...

def atanh():
    """Computes the inverse hyperbolic tangent function"""
    ...

def ceil():
    """Rounds numbers up to the next whole number"""
    ...

def cos():
    """Computes the cosine function"""
    ...

def erf():
    """Computes the error function, which has applications in statistics"""
    ...

def erfc():
    """Computes the complementary error function, which has applications in statistics"""
    ...

def exp():
    """Computes the exponent function."""
    ...

def expm1():
    """Computes $e^x-1$.  In certain applications, using this function preserves numeric accuracy better than the `exp` function."""
    ...

def floor():
    """Rounds numbers up to the next whole number"""
    ...

def gamma():
    """Computes the gamma function"""
    ...

def lgamma():
    """Computes the natural log of the gamma function"""
    ...

def log():
    """Computes the natural log"""
    ...

def log10():
    """Computes the log base 10"""
    ...

def log2():
    """Computes the log base 2"""
    ...

def sin():
    """Computes the sine"""
    ...

def sinh():
    """Computes the hyperbolic sine"""
    ...

def sqrt():
    """Computes the square root"""
    ...

def tan():
    """Computes the tangent"""
    ...

def tanh():
    """Computes the hyperbolic tangent"""
    ...

def vectorize(f, *, otypes=None):
    """
    :param callable f: The function to wrap
    :param otypes: List of array types that may be returned by the function.  None is intepreted to mean the return value is float.

    Wrap a Python function ``f`` so that it can be applied to arrays.

    The callable must return only values of the types specified by otypes, or the result is undefined."""
    ...
