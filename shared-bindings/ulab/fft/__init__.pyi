"""Frequency-domain functions"""

def fft(r, c=None):
    """
    :param ulab.array r: A 1-dimension array of values whose size is a power of 2
    :param ulab.array c: An optional 1-dimension array of values whose size is a power of 2, giving the complex part of the value
    :return tuple (r, c): The real and complex parts of the FFT

    Perform a Fast Fourier Transform from the time domain into the frequency domain

    See also ~ulab.extras.spectrum, which computes the magnitude of the fft,
    rather than separately returning its real and imaginary parts."""
    ...

def ifft(r, c=None):
    """
    :param ulab.array r: A 1-dimension array of values whose size is a power of 2
    :param ulab.array c: An optional 1-dimension array of values whose size is a power of 2, giving the complex part of the value
    :return tuple (r, c): The real and complex parts of the inverse FFT

    Perform an Inverse Fast Fourier Transform from the frequeny domain into the time domain"""
    ...
