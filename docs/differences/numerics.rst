.. _numerics:

In Python 3.x, floating point numbers are implemented using double 
precision. This provides roughly 53 significant bits (17 digits or so)
which means that many programmers can often (but not always) simply
ignore the issue of limited floating-point precision without much
risk of being caught out. The cost of putting floating point numbers
into the heap is considered to be high enough that the slightly 
higher performance that single precision numbers would provide is
reckoned to be immaterial.

MicroPython, on the other hand, typically runs on much lower performance
hardware that typically lacks double precision floating point hardware
and is commonly memory-constrained. There is also the perception that
the microcontrollers that are typically used with MicroPython typically
aren't doing much with numerical computation. As such, the design 
decision was made that floating point numbers would be implemented using
single precision.

Unfortunately, the risk of being surprised by this lower level of precision
is much higher than with double precision. The problem typically shows up
if you are subtracting nearly equal values or if you are adding small
values to larger ones. To take a simple example, suppose we are building
a radio that is representing the reception frequency in kHz with three 
digits of precision. If we tune upwards 0.001kHz at a time, then 
starting a frequency of 17123.453kHz the next frequency will be 
17123.45kHz which is probably very surprising to most users of the radio.

This can seem like a small issue, but similar cases have resulted in errors
of hundreds of miles in geopositioning or several minutes in synchronizing
time using NTP.

You may be able to work around this limitation on precision by rewriting
your algorithms. Another approach is to use integers so that you get a 
full 31 bits of precision instead 23. There are also libraries that support
(arbitrary precision decimal floating point)[https://github.com/mpy-dev/micropython-decimal-number].

Another intermediate approach is to use two single precision numbers to 
represent a number with roughly double precision. This can be done by
using the trick that if `a` and `b` are single precision floating point
numbers where `a >> b` then `a + b` computed in single precision will lose
bits. On the other hand, if we take `s1 = a + b` and `s2 = s1 - a` then
`s1 + s2` would be the answer that we want (if we could compute it precisely).
Thus, we can build a data structure to keep both `s1` and `s2` and then overload
the arithmetic operations to always keep two values that together represent the
double precision result we are looking for.

In the extreme case, you can recompile MicroPython to use double precision
floating point arithmetic.