:mod:`utime` -- time related functions
======================================

.. module:: utime
   :synopsis: time related functions

|see_cpython_module| :mod:`python:time`.

The ``utime`` module provides functions for getting the current time and date,
measuring time intervals, and for delays.

**Time Epoch**: The Unix port uses the POSIX standard epoch of
1970-01-01 00:00:00 UTC. However, all other ports use an epoch of
2000-01-01 00:00:00 UTC. The reason for the unconventional epoch
is that, at least until 2034, the time fits into a 31-bit "small int"
and thus a call to `time.time()` does not involve memory allocation.

**Maintaining actual calendar date/time** is port-dependent.
* On operating systems, such as Unix, that manage time on their own
  MicroPython simply makes the appropriate system calls to retrieve
  time and it cannot set the time.
* On ports with an RTOS that can manage time, the Real Time Clock (RTC)
  is managed by the RTOS and MicroPython leverages the RTOS' functionality
  to retrieve and set time. On those ports the ``machine.RTC`` object
  should not be used to set/get time, however the `set_time()` and
  `adjtime()` methods in this module may be used.
* On baremetal ports the Real Time Clock (RTC) must be initialized by
  the application using either the ``machine.RTC`` class or the `set_time()`
  method. On those ports `adjtime()`` and time zones are not available
  and `localtime()` as well as `gmtime()` return the same time as set
  in the RTC.

Functions
---------

.. function:: localtime([secs])

   Convert a time expressed in seconds since the Epoch (see above) into a 9-tuple which
   contains: (year, month, mday, hour, minute, second, weekday, yearday, isdst)
   If secs is not provided or None, then the current time from the RTC is used.

   * year includes the century (for example 2014).
   * month   is 1-12
   * mday    is 1-31
   * hour    is 0-23
   * minute  is 0-59
   * second  is 0-59
   * weekday is 0-6 for Mon-Sun
   * yearday is 1-366
   * isdst   is 0=no daylight savings in effect, 1=dst in effect, -1=unknown

.. function:: gmtime([secs])

   Gmtime is identical to localtime except that it returns UTC time.

.. function:: mktime()

   This is inverse function of localtime. Its argument is a full 9-tuple
   which expresses a time as per localtime. The values for weekday and yearday are
   ignored.
   It returns an integer which is the number of seconds since the Epoch.

.. function:: tzset(zone)

   Set the conversion rules between UTC time and local time used by `mktime()` and
   `localtime()`.
   The argument is a string that specifies the name of the zones with/without daylight
   savings, the standard offset to UTC, and the start/end times of daylight savings.
   The full rules are described in CPython's `time` module, except that there is no
   zoneinfo database.

   As an example, the America/Los_Angeles time zone can be specified as
   `PST+8PDT,M3.2.0/2,M11.1.0/2` where:
   * PST is the standard time zone name
   * +8 is the offset to convert from PST to UTC
   * PDT is the daylight savings time zone name
   * M3.2.0/2 designates that daylight savings starts at 2am ("/2") on sunday (".0")
     of the second week (".2") of the third month ("M3.")
   * M11.1.0/2 designates that daylight savings ends at 2am on sunday of the first week
     of the eleventh month

   See https://sites.google.com/a/usapiens.com/opnode/time-zones for a lit of zone specs.

.. function:: set_time(secs)

   Sets the current time to the specified number of seconds since the Epoch.

   `set_time()` is an extension to CPython's time module.

.. function:: adjtime(microseconds)

   Gradually adjusts the current time by the number of microseconds specified
   and returns the adjustment that still remains if one is in progress. The
   semantics are the same as for the POSIX adjtime call.

   There is an implementation-dependent maximum adjustment, but in general, if
   the step is large set_time should be used.

   `adjtime()` is an extension to CPython's time module.

.. function:: sleep(seconds)

   Sleep for the given number of seconds. Some boards may accept *seconds* as a
   floating-point number to sleep for a fractional number of seconds. Note that
   other boards may not accept a floating-point argument, for compatibility with
   them use `sleep_ms()` and `sleep_us()` functions.

.. function:: sleep_ms(ms)

   Delay for given number of milliseconds, should be positive or 0.

.. function:: sleep_us(us)

   Delay for given number of microseconds, should be positive or 0.

.. function:: ticks_ms()

    Returns an increasing millisecond counter with an arbitrary reference point, that
    wraps around after some value.

    The wrap-around value is not explicitly exposed, but we will
    refer to it as *TICKS_MAX* to simplify discussion. Period of the values is
    *TICKS_PERIOD = TICKS_MAX + 1*. *TICKS_PERIOD* is guaranteed to be a power of
    two, but otherwise may differ from port to port. The same period value is used
    for all of `ticks_ms()`, `ticks_us()`, `ticks_cpu()` functions (for
    simplicity). Thus, these functions will return a value in range [*0* ..
    *TICKS_MAX*], inclusive, total *TICKS_PERIOD* values. Note that only
    non-negative values are used. For the most part, you should treat values returned
    by these functions as opaque. The only operations available for them are
    `ticks_diff()` and `ticks_add()` functions described below.

    Note: Performing standard mathematical operations (+, -) or relational
    operators (<, <=, >, >=) directly on these value will lead to invalid
    result. Performing mathematical operations and then passing their results
    as arguments to `ticks_diff()` or `ticks_add()` will also lead to
    invalid results from the latter functions.

.. function:: ticks_us()

   Just like `ticks_ms()` above, but in microseconds.

.. function:: ticks_cpu()

   Similar to `ticks_ms()` and `ticks_us()`, but with the highest possible resolution
   in the system. This is usually CPU clocks, and that's why the function is named that
   way. But it doesn't have to be a CPU clock, some other timing source available in a
   system (e.g. high-resolution timer) can be used instead. The exact timing unit
   (resolution) of this function is not specified on ``utime`` module level, but
   documentation for a specific port may provide more specific information. This
   function is intended for very fine benchmarking or very tight real-time loops.
   Avoid using it in portable code.

   Availability: Not every port implements this function.


.. function:: ticks_add(ticks, delta)

   Offset ticks value by a given number, which can be either positive or negative.
   Given a *ticks* value, this function allows to calculate ticks value *delta*
   ticks before or after it, following modular-arithmetic definition of tick values
   (see `ticks_ms()` above). *ticks* parameter must be a direct result of call
   to `ticks_ms()`, `ticks_us()`, or `ticks_cpu()` functions (or from previous
   call to `ticks_add()`). However, *delta* can be an arbitrary integer number
   or numeric expression. `ticks_add()` is useful for calculating deadlines for
   events/tasks. (Note: you must use `ticks_diff()` function to work with
   deadlines.)

   Examples::

        # Find out what ticks value there was 100ms ago
        print(ticks_add(time.ticks_ms(), -100))

        # Calculate deadline for operation and test for it
        deadline = ticks_add(time.ticks_ms(), 200)
        while ticks_diff(deadline, time.ticks_ms()) > 0:
            do_a_little_of_something()

        # Find out TICKS_MAX used by this port
        print(ticks_add(0, -1))


.. function:: ticks_diff(ticks1, ticks2)

   Measure ticks difference between values returned from `ticks_ms()`, `ticks_us()`,
   or `ticks_cpu()` functions, as a signed value which may wrap around.

   The argument order is the same as for subtraction
   operator, ``ticks_diff(ticks1, ticks2)`` has the same meaning as ``ticks1 - ticks2``.
   However, values returned by `ticks_ms()`, etc. functions may wrap around, so
   directly using subtraction on them will produce incorrect result. That is why
   `ticks_diff()` is needed, it implements modular (or more specifically, ring)
   arithmetics to produce correct result even for wrap-around values (as long as they not
   too distant inbetween, see below). The function returns **signed** value in the range
   [*-TICKS_PERIOD/2* .. *TICKS_PERIOD/2-1*] (that's a typical range definition for
   two's-complement signed binary integers). If the result is negative, it means that
   *ticks1* occurred earlier in time than *ticks2*. Otherwise, it means that
   *ticks1* occurred after *ticks2*. This holds **only** if *ticks1* and *ticks2*
   are apart from each other for no more than *TICKS_PERIOD/2-1* ticks. If that does
   not hold, incorrect result will be returned. Specifically, if two tick values are
   apart for *TICKS_PERIOD/2-1* ticks, that value will be returned by the function.
   However, if *TICKS_PERIOD/2* of real-time ticks has passed between them, the
   function will return *-TICKS_PERIOD/2* instead, i.e. result value will wrap around
   to the negative range of possible values.

   Informal rationale of the constraints above: Suppose you are locked in a room with no
   means to monitor passing of time except a standard 12-notch clock. Then if you look at
   dial-plate now, and don't look again for another 13 hours (e.g., if you fall for a
   long sleep), then once you finally look again, it may seem to you that only 1 hour
   has passed. To avoid this mistake, just look at the clock regularly. Your application
   should do the same. "Too long sleep" metaphor also maps directly to application
   behavior: don't let your application run any single task for too long. Run tasks
   in steps, and do time-keeping inbetween.

   `ticks_diff()` is designed to accommodate various usage patterns, among them:

   * Polling with timeout. In this case, the order of events is known, and you will deal
     only with positive results of `ticks_diff()`::

        # Wait for GPIO pin to be asserted, but at most 500us
        start = time.ticks_us()
        while pin.value() == 0:
            if time.ticks_diff(time.ticks_us(), start) > 500:
                raise TimeoutError

   * Scheduling events. In this case, `ticks_diff()` result may be negative
     if an event is overdue::

        # This code snippet is not optimized
        now = time.ticks_ms()
        scheduled_time = task.scheduled_time()
        if ticks_diff(scheduled_time, now) > 0:
            print("Too early, let's nap")
            sleep_ms(ticks_diff(scheduled_time, now))
            task.run()
        elif ticks_diff(scheduled_time, now) == 0:
            print("Right at time!")
            task.run()
        elif ticks_diff(scheduled_time, now) < 0:
            print("Oops, running late, tell task to run faster!")
            task.run(run_faster=true)

   Note: Do not pass `time()` values to `ticks_diff()`, you should use
   normal mathematical operations on them. But note that `time()` may (and will)
   also overflow. This is known as https://en.wikipedia.org/wiki/Year_2038_problem .


.. function:: time()

   Returns the number of seconds, as an integer, since the Epoch, assuming that
   underlying RTC is set and maintained as described above. If an RTC is not set, this
   function returns number of seconds since a port-specific reference point in time (for
   embedded boards without a battery-backed RTC, usually since power up or reset). If you
   want to develop portable MicroPython application, you should not rely on this function
   to provide higher than second precision. If you need higher precision, use
   `ticks_ms()` and `ticks_us()` functions, if you need calendar time,
   `localtime()` without an argument is a better choice.

   .. admonition:: Difference to CPython
      :class: attention

      In CPython, this function returns number of
      seconds since Unix epoch, 1970-01-01 00:00 UTC, as a floating-point,
      usually having microsecond precision. With MicroPython, some ports
      use the same Epoch, and if floating-point precision allows,
      return sub-second precision. Embedded hardware usually doesn't have
      floating-point precision to represent both long time ranges and subsecond
      precision, so they use an integer value with second precision. Some embedded
      hardware also lacks battery-powered RTC, and returns the number of seconds
      since the last power-up or from another relative, hardware-specific, point
      (e.g. reset).
