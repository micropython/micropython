"""Filtering functions"""

def convolve(r, c=None):
    """
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

    Convolution is most time-efficient when both inputs are of float type."""
    ...
