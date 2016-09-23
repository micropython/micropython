:mod:`machine` --- functions related to the board
=================================================

.. module:: machine
   :synopsis: functions related to the board

The ``machine`` module contains specific functions related to the board.

Reset related functions
-----------------------

.. function:: reset()

   Resets the device in a manner similar to pushing the external RESET
   button.

.. function:: reset_cause()

   Get the reset cause. See :ref:`constants <machine_constants>` for the possible return values.

Interrupt related functions
---------------------------

.. function:: disable_irq()

   Disable interrupt requests.
   Returns the previous IRQ state which should be considered an opaque value.
   This return value should be passed to the ``enable_irq`` function to restore
   interrupts to their original state, before ``disable_irq`` was called.

.. function:: enable_irq(state)

   Re-enable interrupt requests.
   The ``state`` parameter should be the value that was returned from the most
   recent call to the ``disable_irq`` function.

Power related functions
-----------------------

.. function:: freq()

    .. only:: not port_wipy

        Returns CPU frequency in hertz.

    .. only:: port_wipy

        Returns a tuple of clock frequencies: ``(sysclk,)``
        These correspond to:

        - sysclk: frequency of the CPU

.. function:: idle()

   Gates the clock to the CPU, useful to reduce power consumption at any time during
   short or long periods. Peripherals continue working and execution resumes as soon
   as any interrupt is triggered (on many ports this includes system timer
   interrupt occurring at regular intervals on the order of millisecond).

.. function:: sleep()

   Stops the CPU and disables all peripherals except for WLAN. Execution is resumed from
   the point where the sleep was requested. For wake up to actually happen, wake sources
   should be configured first.

.. function:: deepsleep()

   Stops the CPU and all peripherals (including networking interfaces, if any). Execution
   is resumed from the main script, just as with a reset. The reset cause can be checked
   to know that we are coming from ``machine.DEEPSLEEP``. For wake up to actually happen,
   wake sources should be configured first, like ``Pin`` change or ``RTC`` timeout.

.. only:: port_wipy

    .. function:: wake_reason()

        Get the wake reason. See :ref:`constants <machine_constants>` for the possible return values.

Miscellaneous functions
-----------------------

.. only:: port_wipy

    .. function:: main(filename)

        Set the filename of the main script to run after boot.py is finished.  If
        this function is not called then the default file main.py will be executed.

        It only makes sense to call this function from within boot.py.

    .. function:: rng()

        Return a 24-bit software generated random number.

.. function:: unique_id()

   Returns a byte string with a unique identifier of a board/SoC. It will vary
   from a board/SoC instance to another, if underlying hardware allows. Length
   varies by hardware (so use substring of a full value if you expect a short
   ID). In some MicroPython ports, ID corresponds to the network MAC address.

.. function:: time_pulse_us(pin, pulse_level, timeout_us=1000000)

   Time a pulse on the given `pin`, and return the duration of the pulse in
   microseconds.  The `pulse_level` argument should be 0 to time a low pulse
   or 1 to time a high pulse.

   The function first waits while the pin input is different to the `pulse_level`
   parameter, then times the duration that the pin is equal to `pulse_level`.
   If the pin is already equal to `pulse_level` then timing starts straight away.

   The function will raise an OSError with ETIMEDOUT if either of the waits is
   longer than the given timeout value (which is in microseconds).

.. _machine_constants:

Constants
---------

.. data:: machine.IDLE
.. data:: machine.SLEEP
.. data:: machine.DEEPSLEEP

    irq wake values

.. data:: machine.PWRON_RESET
.. data:: machine.HARD_RESET
.. data:: machine.WDT_RESET
.. data:: machine.DEEPSLEEP_RESET
.. data:: machine.SOFT_RESET

    reset causes

.. data:: machine.WLAN_WAKE
.. data:: machine.PIN_WAKE
.. data:: machine.RTC_WAKE

    wake reasons

Classes
-------

.. toctree::
   :maxdepth: 1

   machine.ADC.rst
   machine.I2C.rst
   machine.Pin.rst
   machine.RTC.rst
   machine.SD.rst
   machine.SPI.rst
   machine.Timer.rst
   machine.UART.rst
   machine.WDT.rst
