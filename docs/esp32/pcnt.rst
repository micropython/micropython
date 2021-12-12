PCNT - Counter and Encoder
==========================

The Counter and Encoder use the ESP32 Pulse Counter (PCNT) hardware peripheral,
see Espressif's `ESP-IDF Pulse Counter documentation.
<https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/pcnt.html>`_

For the counter not to miss any pulses, the pulse duration should be longer than one ESP32 APB_CLK cycle (1 / 80 MHz = 12.5 ns).
The pulses are sampled on the edges of the APB_CLK clock and may be missed if fall between the edges.
With ideal input signal maximum frequency of measured pulses is APB_CLK / 2 = 80 MHz / 2 = 40 MHz.

The inputs have optional filters that can be used to discard unwanted glitches in the signal.
The length of ignored pulses is provided in APB_CLK clock cycles.
* Note: Filter value is a 10-bit value, so the maximum filter value should be limited to 1023.
Maximum filtered glitches delay is 1023 * 12.5 ns = 12.7875 us.
Big filter make cutbacks the input frequency: 1 / (12.7875 us * 2) = 39.1 kHz.
* Note: Do not neglect circuitry methods to reduce noise (right powering and grounding, filtering, shielding,
short conductors, twisted pair cable, differential signals, etc.).

There is only one interrupt for the peripheral, and that is managed inside the module.
The user has no interrupt interface, and no interrupts are generated on each pulse.
Interrupts arrive when the 16-bit hardware counter buffer overflows, so this module has a tiny interrupt footprint
while providing support for up to 8 simultaneous counters (Encoder or Counter objects).

.. _esp32_machine.Counter:

Counter
=======

The Pulse Counter service.

Constructor
-----------

.. class:: Counter(id, src=machine.Pin, \*, direction=1, edge=Counter.RISING, filter=12787, scale=1)

    The Counter starts to count immediately. Filtering is enabled.
    Construct and return a new Counter object using the following parameters:

      - *id*. Values of *id* depend on a particular port and its hardware.
        Values 0, 1, etc. are commonly used to select hardware block #0, #1, etc.

      - *src* is the pulse input :ref:`machine.Pin <machine.Pin>` to be monitored
        The keyword may be omitted.

    Keyword arguments:

      - *direction*\=value. Specifying the direction of counting. Suitable values are:

        - if value == 0 or False: count down
        - if value != 0 or True: count up
        - a :ref:`machine.Pin <machine.Pin>` object. The level at that pin controls
          the counting direction:

            - if Pin.value() == 0: count down
            - if Pin.value() == 1: count up

      - *edge*\=value.  Which edges of the input signal will be counted by Counter:

        - Counter.RISING : raise edges
        - Counter.FALLING : fall edges
        - Counter.RISING | Counter.FALLING : both edges

      - *filter*\=value. Specifies a ns-value for the minimal time a signal has to be stable
        at the input to be recognized. The largest value is 12787ns (1023 * 1000000000 / APB_CLK_FREQ).
        A value of 0 sets the filter is switched off.

      - *scale*\=value. Sets the scale value. The default value is 1. You may treat the scale
        factor as **click per count**, **mm per count**, **inch per count** etc.

Methods
-------

.. method:: Counter.init(keyword_arguments)

   Modify settings for the Counter object.  See the above constructor for details
   about the parameters.

.. method:: Counter.deinit()

   Free the input pins and counter.

.. method:: Counter.value([value])

   Get (and optional set) the Counter value as a signed integer.
   With no argument, the actual Counter value is returned.

   With a single *value* argument the Counter is set to that value.

-.. method:: Counter.scaled([value])

   Get (and optional set) the current scaled value of the Counter as a signed integer.
   With no argument, the actual scaled value is returned.

   With a single *value* argument the scaled value of Counter is set to that value.

   Pseudocode is::

    def scaled(self, scaled=None):
        _scaled = self._value * self.scale
        if scaled is not None:
            self._value = round(scaled / self.scale)
        return _scaled

.. method:: Counter.pause()

.. method:: Counter.resume()

.. method:: Counter.filter([value])

   Set filter value. 0 disable filtering.
   Return current filter value.

::

    from machine import Counter, Pin

    try:
        cnt = Counter(0, Pin(17, mode=Pin.IN), direction=Pin(16, mode=Pin.IN))

        flt = cnt.filter()  # return current filter value.
        cnt.filter(10_000)  # filter delay is 10ms
        cnt.pause()
        cnt.resume()
        c = cnt.value(12345)  # get current counter value, set the counter value

        _c = None
        while True:
            c = cnt.value()  # get the counter value
            if _c != c:
                _c = c
                print('Counter =', c)
    finally:
        cnt.deinit()  # free the input pins and counter.


.. _esp32_machine.Encoder:

Encoder
=======

This class provides a Quadrature Incremental Encoder service.
See `Quadrature encoder outputs.
<https://en.wikipedia.org/wiki/Incremental_encoder#Quadrature_outputs>`_

.. image:: img/quad.png
    :width: 397px

Constructor
-----------

.. class:: Encoder(id, phase_a=machine.Pin, phase_b=machine.Pin, \*, x124=4, filter=12787, scale=1)

    The Encoder starts to count immediately. Filtering is enabled.
    Construct and return a new quadrature encoder object using the following parameters:

      - *id*. Values of *id* depend on a particular port and its hardware.
        Values 0, 1, etc. are commonly used to select hardware block #0, #1, etc.

      - *phase_a*, *phase_b* are input pins :ref:`machine.Pin <machine.Pin>` for monitoring of quadrature encoder pulses.
        The keywords may be omitted.

    Keyword arguments:

      - *x124*\=value. Hardware multiplier, possible values is 1, 2, 4. The default value is 4.
        More info in `Quadrature decoder state table <https://en.wikipedia.org/wiki/Incremental_encoder#Quadrature_decoder>`_.
        When more Encoder resolution is needed, it is possible for the encoder to count the leading
        and trailing edges of the quadrature encoder’s pulse train from one channel,
        which doubles (x2) the number of pulses. Counting both leading and trailing edges
        of both channels (A and B channels) of a quadrature encoder will quadruple (x4) the number of pulses:

          - 1 - count the leading(or trailing) edges from one phase channel.
          - 2 - count the leading and trailing edges from one phase channel.
          - 4 - count both leading and trailing edges of both phase channels.

      - *scale*\=value. Sets the scale value. The default value is 1. You may treat the scale
        factor as **click per impulse**, **revolution per impulse**, **angle per impulse** etc.
        Hint: Set scale factor to 1/4 to balance the multiplier x124=4.

    This keyword is the same as the Counter keyword, see above:
      - *filter*\=value

Methods
-------

.. method:: Encoder.init(keyword_arguments)

   Modify settings for the Encoder object.  See the above constructor for details
   about the parameters.

The Encoder has the same methods as the Counter and differs only
in the constructor and internal hardware PCNT initialization.

::

    from machine import Encoder, Pin

    try:
        enc = Encoder(0, Pin(17, mode=Pin.IN), Pin(16, mode=Pin.IN))

        flt = enc.filter()  # return current filter value.
        enc.filter(10_000)  # filter delay is 10ms
        enc.pause()
        enc.resume()
        c = enc.value(12345)  # get current encoder value, set the encoder value

        _c = None
        while True:
            c = enc.value()  # get the encoder value
            if _c != c:
                _c = c
                print('Encoder =', c)
    finally:
        encoder.deinit()  # free the input pins and encoder.
