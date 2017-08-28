:mod:`time` ---  Time access and conversions
============================================

.. module:: time
   :synopsis: Time access and conversions.

This module provides various time-related functions.

----------------------------------------------


.. function:: time.sleep(secs)

   Suspend execution of the calling thread for the given number of
   seconds. The argument may be a floating point number to indicate a
   more precise sleep time. The actual suspension time may be less
   than that requested because any caught signal will terminate the
   `sleep()` following execution of that signal’s catching
   routine. Also, the suspension time may be longer than requested by
   an arbitrary amount because of the scheduling of other activity in
   the system.


.. function:: time.sleep_ms(msecs)

   Same as `sleep()` but sleep for `msecs` number of milliseconds.
              

.. function:: time.sleep_us(usecs)

   Same as `sleep()` but sleep for `usecs` number of microseconds.

              
.. function:: time.time()

   Return the time in seconds since the epoch as a floating point
   number. Note that even though the time is always returned as a
   floating point number, not all systems provide time with a better
   precision than 1 second. While this function normally returns
   non-decreasing values, it can return a lower value than a previous
   call if the system clock has been set back between the two calls.

