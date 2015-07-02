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

.. function:: time()

   Returns the number of seconds, as an integer, since 1/1/2000.
