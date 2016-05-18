:mod:`utime` -- time related functions
======================================

.. module:: utime
   :synopsis: time related functions

The ``utime`` module provides functions for getting the current time and date,
measuring time intervals, and for delays.

**Time Epoch**: Unix port uses standard for POSIX systems epoch of
1970-01-01 00:00:00 UTC. However, embedded ports use epoch of
2000-01-01 00:00:00 UTC.

**Maintaining actual calendar date/time**: This requires a
Real Time Clock (RTC). On systems with underlying OS (including some
RTOS), an RTC may be implicit. Setting and maintaining actual calendar
time is responsibility of OS/RTOS and is done outside of MicroPython,
it just uses OS API to query date/time. On baremetal ports however
system time depends on ``machine.RTC()`` object. The current calendar time
may be set using ``machine.RTC().datetime(tuple)`` function, and maintained
by following means:

* By a backup battery (which may be an additional, optional component for
  a particular board).
* Using networked time protocol (requires setup by a port/user).
* Set manually by a user on each power-up (many boards then maintain
  RTC time across hard resets, though some may require setting it again
  in such case).

If actual calendar time is not maintained with a system/MicroPython RTC,
functions below which require reference to current absolute time may
behave not as expected.

Functions
---------

.. function:: localtime([secs])

   Convert a time expressed in seconds since the Epoch (see above) into an 8-tuple which
   contains: (year, month, mday, hour, minute, second, weekday, yearday)
   If secs is not provided or None, then the current time from the RTC is used.

   * year includes the century (for example 2014).
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

.. only:: port_unix or port_pyboard or port_esp8266

    .. function:: sleep(seconds)
    
       Sleep for the given number of seconds.  Seconds can be a floating-point number to
       sleep for a fractional number of seconds. Note that other MicroPython ports may
       not accept floating-point argument, for compatibility with them use ``sleep_ms()``
       and ``sleep_us()`` functions.

.. only:: port_wipy

    .. function:: sleep(seconds)
    
       Sleep for the given number of seconds.

.. only:: port_unix or port_pyboard or port_wipy or port_esp8266

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

.. only:: port_wipy or port_pyboard

    .. function::  ticks_cpu()

       Similar to ``ticks_ms`` and ``ticks_us``, but with higher resolution (usually CPU clocks).

.. only:: port_unix or port_pyboard or port_wipy or port_esp8266

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

   Returns the number of seconds, as an integer, since the Epoch, assuming that underlying
   RTC is set and maintained as decsribed above. If an RTC is not set, this function returns
   number of seconds since a port-specific reference point in time (for embedded boards without
   a battery-backed RTC, usually since power up or reset). If you want to develop portable
   MicroPython application, you should not rely on this function to provide higher than second
   precision. If you need higher precision, use ``ticks_ms()`` and ``ticks_us()`` functions,
   if you need calendar time, ``localtime()`` without an argument is a better choice.

   .. admonition:: Difference to CPython
      :class: attention

      In CPython, this function returns number of
      seconds since Unix epoch, 1970-01-01 00:00 UTC, as a floating-point,
      usually having microsecond precision. With MicroPython, only Unix port
      uses the same Epoch, and if floating-point precision allows,
      returns sub-second precision. Embedded hardware usually doesn't have
      floating-point precision to represent both long time ranges and subsecond
      precision, so they use integer value with second precision. Some embedded
      hardware also lacks battery-powered RTC, so returns number of seconds
      since last power-up or from other relative, hardware-specific point
      (e.g. reset).
