class RTC -- real time clock
============================

The RTC is and independent clock that keeps track of the date
and time.

Example usage::

    rtc = pyb.RTC()
    rtc.datetime((2014, 5, 1, 4, 13, 0, 0, 0))
    print(rtc.datetime())


Constructors
------------

.. class:: pyb.RTC()

   Create an RTC object.


Methods
-------

.. method:: rtc.datetime([datetimetuple])

   Get or set the date and time of the RTC.
   
   With no arguments, this method returns an 8-tuple with the current
   date and time.  With 1 argument (being an 8-tuple) it sets the date
   and time.
   
   The 8-tuple has the following format:
   
       (year, month, day, weekday, hours, minutes, seconds, subseconds)
   
   ``weekday`` is 1-7 for Monday through Sunday.
   
   ``subseconds`` counts down from 255 to 0

.. method:: rtc.wakeup(timeout, callback=None)

   Set the RTC wakeup timer to trigger repeatedly at every ``timeout``
   milliseconds.  This trigger can wake the pyboard from both the sleep
   states: :meth:`pyb.stop` and :meth:`pyb.standby`.

   If ``timeout`` is ``None`` then the wakeup timer is disabled.

   If ``callback`` is given then it is executed at every trigger of the
   wakeup timer.  ``callback`` must take exactly one argument.

.. method:: rtc.info()

   Get information about the startup time and reset source.
   
    - The lower 0xffff are the number of milliseconds the RTC took to
      start up.
    - Bit 0x10000 is set if a power-on reset occurred.
    - Bit 0x20000 is set if an external reset occurred
