:mod:`pyb` --- functions related to the pyboard
===============================================

.. module:: pyb
   :synopsis: functions related to the pyboard

The ``pyb`` module contains specific functions related to the pyboard.

Time related functions
----------------------

.. function:: delay(ms)

   Delay for the given number of milliseconds.

.. function:: udelay(us)

   Delay for the given number of microseconds.

.. function:: millis()

   Returns the number of milliseconds since the board was last reset.
   
   The result is always a micropython smallint (31-bit signed number), so
   after 2^30 milliseconds (about 12.4 days) this will start to return
   negative numbers.

.. function:: micros()

   Returns the number of microseconds since the board was last reset.
   
   The result is always a micropython smallint (31-bit signed number), so
   after 2^30 microseconds (about 17.8 minutes) this will start to return
   negative numbers.

.. function:: elapsed_millis(start)

   Returns the number of milliseconds which have elapsed since ``start``.
   
   This function takes care of counter wrap, and always returns a positive
   number. This means it can be used to measure periods upto about 12.4 days.
   
   Example::

       start = pyb.millis()
       while pyb.elapsed_millis(start) < 1000:
           # Perform some operation

.. function:: elapsed_micros(start)

   Returns the number of microseconds which have elapsed since ``start``.
   
   This function takes care of counter wrap, and always returns a positive
   number. This means it can be used to measure periods upto about 17.8 minutes.
   
   Example::

       start = pyb.micros()
       while pyb.elapsed_micros(start) < 1000:
           # Perform some operation
           pass

Reset related functions
-----------------------

.. function:: hard_reset()

   Resets the pyboard in a manner similar to pushing the external RESET
   button.

.. function:: bootloader()

   Activate the bootloader without BOOT\* pins.

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

.. function:: freq([sys_freq])

   If given no arguments, returns a tuple of clock frequencies:
   (SYSCLK, HCLK, PCLK1, PCLK2).
   
   If given an argument, sets the system frequency to that value in Hz.
   Eg freq(120000000) gives 120MHz.  Note that not all values are
   supported and the largest supported frequency not greater than
   the given sys_freq will be selected.

   Supported frequencies are (in MHz): 8, 16, 24, 30, 32, 36, 40, 42, 48,
   54, 56, 60, 64, 72, 84, 96, 108, 120, 144, 168.

   8MHz uses the HSE (external crystal) directly and 16MHz uses the HSI
   (internal oscillator) directly.  The higher frequencies use the HSE to
   drive the PLL (phase locked loop), and then use the output of the PLL.

   Note that if you change the frequency while the USB is enabled then
   the USB may become unreliable.  It is best to change the frequency
   in boot.py, before the USB peripheral is started.  Also note that
   frequencies below 36MHz do not allow the USB to function correctly.

.. function:: wfi()

   Wait for an interrupt.
   This executies a ``wfi`` instruction which reduces power consumption
   of the MCU until an interrupt occurs, at which point execution continues.

.. function:: standby()


.. function:: stop()

Miscellaneous functions
-----------------------

.. function:: have_cdc()

   Return True if USB is connected as a serial device, False otherwise.

.. function:: hid((buttons, x, y, z))

   Takes a 4-tuple (or list) and sends it to the USB host (the PC) to
   signal a HID mouse-motion event.

.. function:: info([dump_alloc_table])

   Print out lots of information about the board.

.. function:: repl_uart(uart)

   Get or set the UART object that the REPL is repeated on.

.. function:: rng()

   Return a 30-bit hardware generated random number.

.. function:: sync()

   Sync all file systems.

.. function:: unique_id()

   Returns a string of 12 bytes (96 bits), which is the unique ID for the MCU.

Classes
-------

.. toctree::
   :maxdepth: 1

   pyb.Accel.rst
   pyb.ADC.rst
   pyb.CAN.rst
   pyb.DAC.rst
   pyb.ExtInt.rst
   pyb.I2C.rst
   pyb.LCD.rst
   pyb.LED.rst
   pyb.Pin.rst
   pyb.RTC.rst
   pyb.Servo.rst
   pyb.SPI.rst
   pyb.Switch.rst
   pyb.Timer.rst
   pyb.UART.rst
   pyb.USB_VCP.rst
