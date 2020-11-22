The PCNT and QUAD counters uses the ESP32 pulse counter hardware peripheral, 
see Espressif's `ESP-IDF Pulse Counter documentation.
<https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/pcnt.html>`_

For the counter not to miss any pulses, the pulse duration should be longer than one ESP32 APB_CLK cycle (1 / 80 MHz = 12.5 ns).
The pulses are sampled on the edges of the APB_CLK clock and may be missed, if fall between the edges.
With ideal input signal maximum frequency of measured pulses is APB_CLK / 2 = 80 MHz / 2 = 40 MHz.

The inputs have optional filters that can be used to discard unwanted glitches in the signal.
The length of ignored pulses is provided in APB_CLK clock cycles. 
* Note: Filter value is a 10-bit value, so the maximum filter value should be limited to 1023.
Maximum filtered glitches delay is 1023 * 12.5 ns = 12.7875 us.
Big filter make cutbacks the input frequency: 1 / (12.7875 us * 2) = 39.1 kHz.
* Note: Do not neglect circuitry methods to reduce noise (right powering and grounding, filtering, shielding, 
short conductors, twisted pair cable, differential signals, etc.).

There is only one interrupt for the peripheral, and that is managed by the pcnt module.
The user has no interrupt interface, and no interrupts are generated on each pulse.
Interrupts arrive when the 16 bit hardware counter buffer overflows, so this library has a tiny interrupt footprint
while providing support for up to 8 simultaneous counters.

.. _pcnt.PCNT:

PCNT
----

The PCNT .

.. class:: PCNT(edge: Edge, pulse_pin_number, dir_pin_number=None, pin_pull: PinPull=DOWN)

    Counter start to count immediately. Filter value initialized by 1023. Filtering is enabled.

.. method:: PCNT.__del__()

    Free the input pins and counter.

.. method:: PCNT.count()

    Return current counter value.

.. method:: PCNT.clear()

.. method:: PCNT.count_and_clear()

.. method:: PCNT.pause()

.. method:: PCNT.resume()

.. method:: PCNT.set_count(new_value)
    
    Set the counter value, new_value is 64-bit signed integer.

.. method:: PCNT.set_filter_value(filter_val)

    Set filter value.
    
.. method:: PCNT.get_filter_value()

    Return current filter value.

The filter is put into operation / suspended by calling pcnt_filter_enable() / pcnt_filter_disable().

.. method:: PCNT.filter_enable()

.. method:: PCNT.filter_disable()

.. _pcnt.QUAD:

QUAD
----

See `Quadrature encoder outputs.
<https://en.wikipedia.org/wiki/Incremental_encoder#Quadrature_outputs>`_

.. class:: QUAD(clock_multiplier:ClockMultiplier, aPinNumber, bPinNumber, pin_pull:PinPull=DOWN)

The QUAD counter has the same methods as the PCNT counter and 
differs only in the constructor and internal counter initialization.

Enumarations
------------

.. class:: pcnt.Edge()

   Which edges of the input signal will be counted by PCNT.

.. data:: Edge.RAISE
          Edge.FALL
          Edge.BOTH

.. class:: pcnt.PinPull()

   The counter input pins have an internal weak pull-up/pull-down resistors. 

.. data:: PinPull.NONE
          PinPull.DOWN
          PinPull.UP

   Note: Default value is PinPull.DOWN to prevent erroneous ticking when disconnected.

.. class:: ClockMultiplier()

   When more QUAD resolution is needed, it is possible for the counter to count the leading 
   and trailing edges of the quadrature encoder’s pulse train from one channel, 
   which doubles (x2) the number of pulses. Counting both leading and trailing edges 
   of both channels (A and B channels) of a quadrature encoder will quadruple (x4) the number of pulses.

.. image:: img/quad.png
    :width: 397px

.. data:: ClockMultiplier.X1
          ClockMultiplier.X2
          ClockMultiplier.X4

   |    X1 - Count the leading(or trailing) edges from one channel.
   |    X2 - Count the leading and trailing edges from one channel.
   |    X4 - Count both leading and trailing edges of both channels.

::

    import pcnt

    try:
        cnt = pcnt.QUAD(pcnt.ClockMultiplier.X4, 17, 16, PinPull.UP)

        flt = cnt.get_filter_value()  # return current filter value.
        cnt.set_filter_value(100)     # filter delay is 
        cnt.filter_disable()          #
        cnt.filter_enable()           #
        c = cnt.count_and_clear()     # get counter and clear it
        cnt.clear()
        cnt.pause()
        cnt.resume()
        cnt.set_count(12345)          # set the counter value

        _c = None
        while True:
            c = cnt.count()  # get the counter value
            if _c != c:
                _c = c
                print('Counter =', c)
    finally:
        cnt.__del__()  # free the input pins and counter.
