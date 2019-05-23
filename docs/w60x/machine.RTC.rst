.. currentmodule:: machine
.. _machine.RTC:

class RTC -- real time clock
============================

The RTC is and independent clock that keeps track of the date
and time.

Example usage::

    rtc = machine.RTC()
    rtc.init((2014, 5, 1, 4, 13, 0, 0, 0))
    print(rtc.now())


Constructors
------------

.. class:: RTC()

   Create an RTC object.

Methods
-------

.. method:: RTC.init(datetime)

   Initialise the RTC. Datetime is a tuple of the form:
   
      ``(year, month, day[, hour[, minute[, second[, microsecond[, tzinfo]]]]])``

.. method:: RTC.now()

   Get get the current datetime tuple.

.. method:: RTC.deinit()

   Resets the RTC to the time of January 1, 2015 and starts running it again.

.. method:: RTC.alarm(time)

   Set the RTC alarm. Time might be either a millisecond value to program the alarm to
   current time + time_in_ms in the future, or a datetimetuple. If the time passed is in
   milliseconds.

.. method:: RTC.cancel()

   Cancel a running alarm.

.. method:: RTC.irq(handler=None)

   Create an irq object triggered by a real time clock alarm.

      - ``handler`` is the function to be called when the callback is triggered.
