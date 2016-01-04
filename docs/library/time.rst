:mod:`time` -- time related functions
=====================================

.. module:: time
   :synopsis: time related functions

The ``time`` module provides functions for getting the current time and date,
and for sleeping.

Functions
---------

.. function:: localtime([secs])

   Convert a time expressed in seconds since Jan 1, 2000 into an 8-tuple which
   contains: (year, month, mday, hour, minute, second, weekday, yearday)
   If secs is not provided or None, then the current time from the RTC is used.
   year includes the century (for example 2014).

   * month   is 1-12
   * mday    is 1-31
   * hour    is 0-23
   * minute  is 0-59
   * second  is 0-59
   * weekday is 0-6 for Mon-Sun
   * yearday is 1-366

.. function:: mktime()

   This is inverse function of localtime. It's argument is a full 8-tuple
   which expresses a time as per localtime. It returns an integer which is
   the number of seconds since Jan 1, 2000.

.. only:: port_pyboard

    .. function:: sleep(seconds)
    
       Sleep for the given number of seconds.  Seconds can be a floating-point number to
       sleep for a fractional number of seconds.

.. only:: port_esp8266 or port_wipy

    .. function:: sleep(seconds)
    
       Sleep for the given number of seconds.

.. only:: port_wipy or port_pyboard

    .. function::  sleep_ms(ms)

       Delay for given number of milliseconds, should be positive or 0.

    .. function::  sleep_us(us)

       Delay for given number of microseconds, should be positive or 0

    .. function::  ticks_ms()

        Returns an increasing millisecond counter with arbitrary reference point, 
        that wraps after some (unspecified) value. The value should be treated as 
        opaque, suitable for use only with ticks_diff().

    .. function::  ticks_us()

       Just like ``ticks_ms`` above, but in microseconds.

    .. function::  ticks_cpu()

       Similar to ``ticks_ms`` and ``ticks_us``, but with higher resolution (usually CPU clocks).

    .. function::  ticks_diff(old, new)

       Measure period between consecutive calls to ticks_ms(), ticks_us(), or ticks_cpu(). 
       The value returned by these functions may wrap around at any time, so directly 
       subtracting them is not supported. ticks_diff() should be used instead. "old" value should 
       actually precede "new" value in time, or result is undefined. This function should not be
       used to measure arbitrarily long periods of time (because ticks_*() functions wrap around 
       and usually would have short period). The expected usage pattern is implementing event 
       polling with timeout::

            # Wait for GPIO pin to be asserted, but at most 500us
            start = time.ticks_us()
            while pin.value() == 0:
                if time.ticks_diff(start, time.ticks_us()) > 500:
                    raise TimeoutError

.. function:: time()

   Returns the number of seconds, as an integer, since 1/1/2000.
