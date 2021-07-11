.. currentmodule:: machine
.. _machine.RTC:

class RTC -- real time clock
============================

The RTC is an independent clock that keeps track of the date
and time.

Example usage::

    rtc = machine.RTC()
    rtc.datetime((2020, 1, 21, 2, 10, 32, 36, 0))
    print(rtc.datetime())


Constructors
------------

.. class:: RTC(id=0, ...)

   Create an RTC object. See init for parameters of initialization.

Methods
-------

.. method:: RTC.datetime([datetimetuple])

   Get or set the date and time of the RTC.

   With no arguments, this method returns an 8-tuple with the current
   date and time.  With 1 argument (being an 8-tuple) it sets the date
   and time.

   The 8-tuple has the following format:

       (year, month, day, weekday, hours, minutes, seconds, subseconds)

   The meaning of the ``subseconds`` field is hardware dependent.

.. method:: RTC.init(datetime)

   Initialise the RTC. Datetime is a tuple of the form:

      ``(year, month, day[, hour[, minute[, second[, microsecond[, tzinfo]]]]])``

.. method:: RTC.now()

   Get get the current datetime tuple.

.. method:: RTC.deinit()

   Resets the RTC to the time of January 1, 2015 and starts running it again.

.. method:: RTC.alarm(id, time, *, repeat=False)

   Set the RTC alarm. Time might be either a millisecond value to program the alarm to
   current time + time_in_ms in the future, or a datetimetuple. If the time passed is in
   milliseconds, repeat can be set to ``True`` to make the alarm periodic.

.. method:: RTC.alarm_left(alarm_id=0)

   Get the number of milliseconds left before the alarm expires.

.. method:: RTC.cancel(alarm_id=0)

   Cancel a running alarm.

.. method:: RTC.irq(*, trigger, handler=None, wake=machine.IDLE)

   Create an irq object triggered by a real time clock alarm.

      - ``trigger`` must be ``RTC.ALARM0``
      - ``handler`` is the function to be called when the callback is triggered.
      - ``wake`` specifies the sleep mode from where this interrupt can wake
        up the system.

Constants
---------

.. data:: RTC.ALARM0

    irq trigger source
