PCNT
====

The Counter and Encoder uses the ESP32 Pulse Counter (PCNT) hardware peripheral,
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

There is only one interrupt for the peripheral, and that is managed inside the module.
The user has no interrupt interface, and no interrupts are generated on each pulse.
Interrupts arrive when the 16 bit hardware counter buffer overflows, so this module has a tiny interrupt footprint
while providing support for up to 8 simultaneous counters (Encoder or Counter objects).

.. _pcnt.Counter:

Counter
=======

The Pulse Counter service.

Constructor
-----------

.. class:: Counter(pulse_pin, \*, direction=1, edge=Counter.RISING, filter=12787, scale=1.0)

    Counter start to count immediately. Filtering is enabled.

    - *pulse_pin* is the input :ref:`machine.Pin <machine.Pin>` to be monitored

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

      - *scale*\=value. Sets the scale value. The default value is 1.

Methods
-------

.. method:: Counter.init(keyword_arguments)

   Modify settings for the Counter object.  See the above constructor for details
   about the parameters.

.. method:: Counter.deinit()

    Free the input pins and counter.

.. method:: Counter.value()

    Return current 64-bit signed counter value.

.. method:: Counter.value(value)

    Set the counter value, *value* is 64-bit signed integer.

.. method:: Counter.pause()

.. method:: Counter.resume()

.. method:: Counter.filter([value])

    Set filter value. 0 disable filtering.
    Return current filter value.

::

    from machine import Counter, Pin

    try:
        cnt = Counter(Pin(17, mode=Pin.IN), direction=Pin(16, mode=Pin.IN))

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


.. _pcnt.Encoder:

Encoder
=======

This class provides an Quadrature Incremental Encoder service.
See `Quadrature encoder outputs.
<https://en.wikipedia.org/wiki/Incremental_encoder#Quadrature_outputs>`_

.. image:: img/quad.png
    :width: 397px

Constructor
-----------

.. class:: Encoder(a_pin, b_pin, \*, x124=4, filter=12787, scale=1.0)

    Encoder start to count immediately. Filtering is enabled.

    - *a_pin*, *b_pin* are input pins :ref:`machine.Pin <machine.Pin>` for monitoring of quadrature encoder pulses

    Keyword arguments:

      - *x124*\=value. Possible values is 1, 2, 4.
        When more Encoder resolution is needed, it is possible for the encoder to count the leading
        and trailing edges of the quadrature encoder’s pulse train from one channel,
        which doubles (x2) the number of pulses. Counting both leading and trailing edges
        of both channels (A and B channels) of a quadrature encoder will quadruple (x4) the number of pulses:

          - 1 - count the leading(or trailing) edges from one channel.
          - 2 - count the leading and trailing edges from one channel.
          - 4 - count both leading and trailing edges of both channels.

    These keywords are the same as the Counter keywords, see above:
      - *filter*\=value
      - *scale*\=value

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
        enc = Encoder(Pin(17, mode=Pin.IN), Pin(16, mode=Pin.IN))

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
