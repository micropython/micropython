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

.. class:: Counter(id, src=None, \*, edge=Counter.RISING, direction=Counter.UP, filter_ns=0, scale=1)

    The Counter starts to count immediately. Filtering is disabled.

      - *id*. Values of *id* depend on a particular port and its hardware.
        Values 0, 1, etc. are commonly used to select hardware block #0, #1, etc.

      - *src* is the pulse input :ref:`machine.Pin <machine.Pin>` to be monitored.
        *src* is required in the constructor.

      - *direction* specifies the direction to count. Values for this include the constants

        - Counter.UP - (default value) and
        - Counter.DOWN to control the direction by software or
        - :ref:`machine.Pin <machine.Pin>` object to control the direction externally. If ``Pin.value()``:
           - 0 - count down,
           - 1 - count up.

      - *edge* specifies which edges of the input signal will be counted by Counter:

        - Counter.RISING : raise edges,
        - Counter.FALLING : fall edges,
        - Counter.RISING | Counter.FALLING : both edges.

      - *filter_ns* specifies a ns-value for the minimal time a signal has to be stable
        at the input to be recognized. The largest value is 12787ns (1023 * 1000000000 / APB_CLK_FREQ).
        The default is 0 – no filter.

      - *scale* sets the scale value. The default value is 1. You may treat the scale
        factor as **click per count**, **mm per count**, **inch per count** etc.

Methods
-------

.. method:: Counter.init(*, src, ...)

   Modify the settings of the Counter object. See the **Constructor** for details about the parameters.

.. method:: Counter.deinit()

   Stops the Counter, disables interrupts and releases hardware resources used by the counter.
   A Soft Reset involve deinitializing all Encoder objects.

.. method:: Counter.filter([value])

   Get, and optionally set, the filter value. 0 disable filtering.

.. method:: Counter.value([value])

   Get, and optionally set, the counter *value* as a signed 64-bit integer.

.. method:: Counter.scaled([value])

   Get, and optionally set, the current scaled value of the Counter as a float.

   Pseudocode is::

    def scaled(self, scaled=None):
        _scaled = self._value * self.scale
        if scaled is not None:
            self._value = round(scaled / self.scale)
        return _scaled

.. method:: Counter.irq(handler=None, trigger=Counter.IRQ_MATCH1 | Counter.IRQ_MATCH2 | Counter.IRQ_ZERO, value=0)

   -*handler* specifies a function is called when the respective *trigger* event happens.
    The callback function *handler* receives a single argument, which is the Counter object.
    All events may share the same callback or have separate callbacks.
    The callback will be disabled, when called with handler=None. Counter.irq() disable all callbacks.
    The event which triggers the callback can be identified with the ``Counter.status()`` method.
    The Counter object which triggers the callback can be identified with the ``Counter.id()`` method.

   -*trigger* events may be:

    - Counter.IRQ_MATCH1 triggered when the counter matches the match1 value.
    - Counter.IRQ_MATCH2 triggered when the counter matches the match2 value.
    - Counter.IRQ_ZERO triggered when the counter matches the 0.

    The default is - trigger=Counter.IRQ_MATCH1 | Counter.IRQ_MATCH2 | Counter.IRQ_ZERO.
    The events are triggered when the counter value and match value are identical, but
    callbacks have always a latency.

   - *value* sets a counter match1/match2 value. When the counter matches these values,
     a callback function can be called. They are 0 by default.

Attention: ``Counter.irq()`` resets counter to 0.

.. method:: Counter.status()

   Returns the event status flags of the recent handled Counter interrupt as a bitmap.

.. method:: Counter.id()

   Returns id number.

.. method:: Counter.pause()

.. method:: Counter.resume()

Constants
---------

.. data:: Counter.UP
          Counter.DOWN

   Selects the counter direction.

.. data:: Counter.RISING
          Counter.FALLING

   Selects the counted edges.

.. data:: Counter.IRQ_MATCH1
          Counter.IRQ_MATCH2
          Counter.IRQ_ZERO

   Selects callback triggers.

::

    from machine import Counter, Pin

    try:
        def irq_handler(self):
            print('irq_handler()', self.id(), self.status(), self.value())

        cnt = Counter(0, src=Pin(17, mode=Pin.IN), direction=Pin(16, mode=Pin.IN))

        cnt.pause()
        flt = cnt.filter()  # return current filter value.
        cnt.filter(10_000)  # filter delay is 10ms
        c = cnt.value(0)  # get current counter value, set the counter value to 0
        cnt.irq(irq_handler, Counter.IRQ_ZERO)  # set irq handler
        cnt.resume()

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

.. class:: Encoder(id, phase_a=None, phase_b=None, \*, x124=4, filter_ns=0, scale=1, match1=0, match2=0)

    The Encoder starts to count immediately. Filtering is disabled.

      - *id*. Values of *id* depend on a particular port and its hardware.
        Values 0, 1, etc. are commonly used to select hardware block #0, #1, etc.

      - *phase_a*, *phase_b* are input pins :ref:`machine.Pin <machine.Pin>` for monitoring of quadrature encoder pulses.
        They are required in the constructor.

      - *x124* is a hardware multiplier, possible values is 1, 2, 4. The default value is 4.
        More info in `Quadrature decoder state table <https://en.wikipedia.org/wiki/Incremental_encoder#Quadrature_decoder>`_.
        When more Encoder resolution is needed, it is possible for the encoder to count the leading
        and trailing edges of the quadrature encoder’s pulse train from one channel,
        which doubles (x2) the number of pulses. Counting both leading and trailing edges
        of both channels (A and B channels) of a quadrature encoder will quadruple (x4) the number of pulses:

          - 1 - count the leading(or trailing) edges from one phase channel.
          - 2 - count the leading and trailing edges from one phase channel.
          - 4 - count both leading and trailing edges of both phase channels.

      - *scale* sets the scale value. The default value is 1. You may treat the scale
        factor as **click per impulse**, **revolution per impulse**, **angle per impulse** etc.
        Hint: Set scale factor to 1/4 to balance the multiplier x124=4.

    These keywords are the same as the Counter keywords, see above:
      - *filter_ns*
      - *match1*
      - *match2*

Methods
-------

.. method:: Encoder.init(*, phase_a, ...)

   Modify the settings of the Encoder object. See the **Constructor** for details about the parameters.

The Encoder has the same methods as the Counter and differs only
in the constructor and internal hardware PCNT initialization.

Constants
---------

.. data:: Encoder.IRQ_MATCH1
          Encoder.IRQ_MATCH2
          Encoder.IRQ_ZERO

   Selects callback triggers.

::

    from machine import Encoder, Pin

    try:
        n = 0
        def irq_handler1(self):
            n -= 1
            print('irq_handler1()', self.id(), self.value(), n)

        def irq_handler2(self):
            n += 1
            print('irq_handler2()', self.id(), self.value(), n)

        enc = Encoder(0, phase_a=Pin(17, mode=Pin.IN), phase_b=Pin(16, mode=Pin.IN), match1=-1000, match2=1000)

        enc.pause()
        flt = enc.filter()  # return current filter value.
        enc.filter(10_000)  # filter delay is 10ms
        c = enc.value(0)  # get current encoder value, set the encoder value to 0
        enc.irq(irq_handler1, Encoder.IRQ_MATCH1)  # set irq handler
        enc.irq(irq_handler2, Encoder.IRQ_MATCH2)  # set irq handler
        enc.resume()

        _c = None
        while True:
            c = enc.value()  # get the encoder value
            if _c != c:
                _c = c
                print('Encoder =', c)
    finally:
        enc.deinit()  # free the input pins and encoder.
