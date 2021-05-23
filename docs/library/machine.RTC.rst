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

.. method:: RTC.memory([data])

   Stores ``data`` (byte literal) into RTC memory, which is kept stored in deep sleep mode of the ESP8266 or ESP32.
   The data is stored together with a magic word to detect that the RTC memory is valid.
   An uninitialized or cleared RTC memory has no magic word and will deliver ``b''``.
   Without ``data`` the method delivers the RTC memory content.
   In the ESP8266 are max. 492 bytes and in the ESP32 are max. 2048 Bytes storeable by this method.

   Example::

      import machine
      rtc = machine.RTC()
      writedata = b'test'
      rtc.memory(writedata) # this command writes writedata into the RTC memory
      readdata = rtc.memory() # this command puts the RTC memory into readdata
      print(readdata)

   Availability: ESP8266, ESP32

.. method:: RTC.usermem()

   This is similar to the RTC.memory() above, but returns a bytearray object whose data
   gets stored directly in the user memory area.

   **The bytearray uses the same memory area as** ``RTC.memory()``, so a little care is required when using both methods.

   Advantages over ``RTC.memory()``:

   - quick access to the entire available RTC slow memory user area
   - Python access to details of the memory area, e.g. by using ``uctypes.sizeof()``
     and ``uctypes.addressof()``
   - data loggers and similar applications don't need to copy the entire (up to 3.5k)
     memory area

   The data is stored together with a magic word to detect that the RTC memory is valid.
   An uninitialized or cleared RTC memory has no magic word; the bytearray returned
   by ``RTC.usermem()`` will contain binary zeroes.

   Example::

      from machine import RTC
      import uctypes

      RESERVED = 64       # leave a little space for RTC.memory()

      STRUCT = {
        "data1":        0 | uctypes.UINT32,
        "data2":        0 | uctypes.UINT32,
        }

      # get the RTC memory area
      rtc_mem = RTC().usermem()

      data = uctypes.struct(uctypes.addressof(rtc_mem) + RESERVED, STRUCT)
      print(data.data1)   # get data1 out of RTC memory
      data.data1 = 4712   # set data1 in RTC memory

   Availability: ESP32

Constants
---------

.. data:: RTC.ALARM0

    irq trigger source


Compile-time options
--------------------

These can be set e.g. in ``ports/esp32/boards/XXX/mpconfigboard.h``

``#define MICROPY_HW_RTC_USER_MEM_MAX     2048``

sets the entire size of the user memory. This can be increased up to ~3500 bytes on the ESP32.

A value of 0 (zero) disables ``RTC.memory()`` as well as ``RTC.usermem()``


