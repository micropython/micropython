.. currentmodule:: machine
.. _machine.RTC:

class RTC -- real time clock
============================

The RTC is and independent clock that keeps track of the date
and time.

Example usage::

    rtc = machine.RTC()
    rtc.datetime((2014, 5, 1, 0, 4, 13, 0, 0))
    print(rtc.datetime())


Constructors
------------

.. class:: RTC()

   Create an RTC object.

Methods
-------

.. method:: RTC.datetime([datetime])

   Get or set the current datetime. Datetime is a tuple of the form:

      ``(year, month, day[, weekday[, hour[, minute[, second[, microsecond]]]]])``

.. method:: RTC.memory([buffer])

    Get or set the contents of non-volatile memory.

.. method:: RTC.alarm(id, time)

   Set the RTC alarm. Time is a millisecond value to program the alarm to
   current time + time_in_ms in the future.

.. method:: RTC.irq(\*, trigger, wake=machine.IDLE)

   Create an irq object triggered by a real time clock alarm.

      - ``trigger`` must be ``RTC.ALARM0``
      - ``wake`` specifies the sleep mode from where this interrupt can wake
        up the system.

Constants
---------

.. data:: RTC.ALARM0

    irq trigger source
