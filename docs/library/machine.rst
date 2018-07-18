:mod:`machine` --- functions related to the hardware
====================================================

.. module:: machine
   :synopsis: functions related to the hardware

The ``machine`` module contains specific functions related to the hardware
on a particular board. Most functions in this module allow to achieve direct
and unrestricted access to and control of hardware blocks on a system
(like CPU, timers, buses, etc.). Used incorrectly, this can lead to
malfunction, lockups, crashes of your board, and in extreme cases, hardware
damage.

.. _machine_callbacks:

A note of callbacks used by functions and class methods of :mod:`machine` module:
all these callbacks should be considered as executing in an interrupt context.
This is true for both physical devices with IDs >= 0 and "virtual" devices
with negative IDs like -1 (these "virtual" devices are still thin shims on
top of real hardware and real hardware interrupts). See :ref:`isr_rules`.

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
   This return value should be passed to the `enable_irq()` function to restore
   interrupts to their original state, before `disable_irq()` was called.

.. function:: enable_irq(state)

   Re-enable interrupt requests.
   The *state* parameter should be the value that was returned from the most
   recent call to the `disable_irq()` function.

Power related functions
-----------------------

.. function:: freq()

    Returns CPU frequency in hertz.

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
   to know that we are coming from `machine.DEEPSLEEP`. For wake up to actually happen,
   wake sources should be configured first, like `Pin` change or `RTC` timeout.

.. function:: wake_reason()

   Get the wake reason. See :ref:`constants <machine_constants>` for the possible return values.

   Availability: ESP32, WiPy.

Miscellaneous functions
-----------------------

.. only:: port_wipy

    .. function:: rng()

        Return a 24-bit software generated random number.

.. function:: unique_id()

   Returns a byte string with a unique identifier of a board/SoC. It will vary
   from a board/SoC instance to another, if underlying hardware allows. Length
   varies by hardware (so use substring of a full value if you expect a short
   ID). In some MicroPython ports, ID corresponds to the network MAC address.

.. function:: time_pulse_us(pin, pulse_level, timeout_us=1000000)

   Time a pulse on the given *pin*, and return the duration of the pulse in
   microseconds.  The *pulse_level* argument should be 0 to time a low pulse
   or 1 to time a high pulse.

   If the current input value of the pin is different to *pulse_level*,
   the function first (*) waits until the pin input becomes equal to *pulse_level*,
   then (**) times the duration that the pin is equal to *pulse_level*.
   If the pin is already equal to *pulse_level* then timing starts straight away.

   The function will return -2 if there was timeout waiting for condition marked
   (*) above, and -1 if there was timeout during the main measurement, marked (**)
   above. The timeout is the same for both cases and given by *timeout_us* (which
   is in microseconds).

.. _machine_constants:

Constants
---------

.. data:: machine.IDLE
          machine.SLEEP
          machine.DEEPSLEEP

    IRQ wake values.

.. data:: machine.PWRON_RESET
          machine.HARD_RESET
          machine.WDT_RESET
          machine.DEEPSLEEP_RESET
          machine.SOFT_RESET

    Reset causes.

.. data:: machine.WLAN_WAKE
          machine.PIN_WAKE
          machine.RTC_WAKE

    Wake-up reasons.

Classes
-------

.. only:: not port_wipy

 .. toctree::
   :maxdepth: 1

   machine.Pin.rst
   machine.Signal.rst
   machine.UART.rst
   machine.SPI.rst
   machine.I2C.rst
   machine.RTC.rst
   machine.Timer.rst
   machine.WDT.rst

.. only:: port_wipy

 .. toctree::
   :maxdepth: 1

   machine.Pin.rst
   machine.UART.rst
   machine.SPI.rst
   machine.I2C.rst
   machine.RTC.rst
   machine.Timer.rst
   machine.WDT.rst
   machine.ADC.rst
   machine.SD.rst
