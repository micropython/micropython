:mod:`machine` --- functions related to the board
=================================================

.. module:: machine
   :synopsis: functions related to the board

The ``machine`` module contains specific functions related to the board.

Reset related functions
-----------------------

.. function:: reset()

   Resets the WiPy in a manner similar to pushing the external RESET
   button.

.. function:: reset_cause()

   Get the reset cause. See :ref:`constants <machine_constants>` for the possible return values.

Interrupt related functions
---------------------------

.. function:: disable_irq()

   Disable interrupt requests.
   Returns the previous IRQ state: ``False``/``True`` for disabled/enabled IRQs
   respectively.  This return value can be passed to enable_irq to restore
   the IRQ to its original state.

.. function:: enable_irq(state=True)

   Enable interrupt requests.
   If ``state`` is ``True`` (the default value) then IRQs are enabled.
   If ``state`` is ``False`` then IRQs are disabled.  The most common use of
   this function is to pass it the value returned by ``disable_irq`` to
   exit a critical section.

Power related functions
-----------------------

.. function:: freq()

   Returns a tuple of clock frequencies: ``(sysclk,)``
   These correspond to:

      - sysclk: frequency of the CPU

.. function:: idle()

   Gates the clock to the CPU, useful to reduce power consumption at any time during
   short or long periods. Peripherals continue working and execution resumes as soon
   as any interrupt is triggered (including the systick which has a period of 1ms).
   Current consumption is reduced to ~12mA (in WLAN STA mode)

.. function:: sleep()

   Stops the CPU and disables all peripherals except for WLAN. Execution is resumed from
   the point where the sleep was requested. Wake sources are ``Pin``, ``RTC`` and ``WLAN``.
   Current consumption is reduced to 950uA (in WLAN STA mode).

.. function:: deepsleep()

   Stops the CPU and all peripherals including WLAN. Execution is resumed from main, just
   as with a reset. The reset cause can be checked to know that we are coming from
   from ``machine.DEEPSLEEP``. Wake sources are ``Pin`` and ``RTC``. Current consumption 
   is reduced to ~5uA.

.. function:: wake_reason()

   Get the wake reason. See :ref:`constants <machine_constants>` for the possible return values.

Miscellaneous functions
-----------------------

.. function:: main(filename)

   Set the filename of the main script to run after boot.py is finished.  If
   this function is not called then the default file main.py will be executed.

   It only makes sense to call this function from within boot.py.

.. function:: rng()

   Return a 24-bit software generated random number.

.. function:: unique_id()

   Returns a string of 6 bytes (48 bits), which is the unique ID of the MCU.
   This also corresponds to the ``MAC address`` of the WiPy.

.. _machine_constants:

Constants
---------

.. data:: machine.IDLE
.. data:: machine.SLEEP
.. data:: machine.DEEPSLEEP

    irq wake values

.. data:: machine.POWER_ON
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
