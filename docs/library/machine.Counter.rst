.. currentmodule:: machine
.. _machine.Counter:

class Counter -- hardware counter
=================================

A hardware counter counts the transitions or pulses on an input pin and accumulates the count into a
hardware register. This register can be read, reset, and typically thresholds can be set to trigger
interrupts when specific values are reached.

Counters are related to timers and in some
microprocessors use the same hardware. From a usage perspective, counters count
edges (or pulses) that happen at arbitrary points in time and the resulting register
values over time are expressed simply as counts whereas timers tend to count
regular clock pulses and the resulting register values are expressed as units of time.

Most hardware counter units can count up or down, can count on positive or negative
incoming edges and can reset at/to some max value. On many microprocessors the input
can be fed through a prescaler which divides the incoming pulses by a power of two.
It is also common to find a filter which can cause short pulses due to glitches to be
ignored.

Example usage::

    from machine import Counter, Pin
    ctr = Counter(0, pin=4)
    sleep(10)
    print ctr.value

Some use-cases for using a Counter are: counting motor shaft revolutions and deriving
rotational velocity, measuring wind
speed using an anemometer that produces N pulses per revolution, measuring rain using a
tipping bucket.

Availability of this class: esp32.

Constructors
------------

.. class:: Counter(id, ...)

   Construct a Counter object for a specific hardware counter unit identified
   by ``id``. Values 0, 1, etc. are commonly used.

   With no additional parameters, the Counter object is created but the hardware is not
   initialised (it has the settings from the last initialisation, if any).
   If extra arguments are given, the hardware is initialised.
   See ``init`` for parameters of initialisation.

Methods
-------

.. method:: Counter.init(pin, direction=Counter.UP, edge=Counter.RISING, limit=None, reset=True)

   Initialise a counter by connecting it to an input pin.
   The input pin can be specified as a pin number (int), a pin name (str), or a Pin object
   (however, some ports may limit the choice, the ESP32 in particular).
   Optionally specify the
   counting direction and whether the rising or falling edge should be counted.
   Additional keyword parameters can customize the counter:
     - ``direction``: Counter.UP, Counter,DOWN
     - ``edge``: Counter.RISING, Counter.FALLING, Counter.BOTH
     - ``limit``: when counting up the register is reset to zero at the next edge after
       it reaches the limit. This means that a limit of ``N`` produces the N+1 values 0..N.
       When counting down the register is set to the limit value at the next edge after it
       reaches zero.
     - ``reset``: if True init resets the counter, else it leaves the current register value
       unchanged which can be useful to preserve counts when an application restarts.

   ESP32 notes:
     - Only supports pin numbers for now.
     - Supports the following parameters: direction, edge, limit, and reset (**verify**).
     - The ESP32's counter units support additional features (e.g. a control
       pin and intermediate trigger values) but these are currently not supported.
     - When counting down the ESP32's hardware starts at zero, counts to a negative limit, then
       resets back to zero. This class shifts the values such that the counter counts down 
       to zero from the limit, which is the more typical implementation on most microprocessors.

.. method:: Counter.deinit()

   Dissociates the counter unit from any Pin and disables any interrupt.

.. method:: Counter.pause()

   Disables the counter unit without changing its configuration or losing its value.
   If the unit is already paused this is a no-op.

.. method:: Counter.resume()

   Resumes the counter unit after a pause.
   If the unit is already running this is a no-op.

.. method:: Counter.irq(trigger=Counter.ZERO, priority=1, handler=None, wake=machine.IDLE)

   Configure an interrupt handler to be called when the trigger condition is met.
   The interrupt handler is called from a hardware interrupt and may not allocate
   memory; see :ref:`isr_rules`.

   The arguments are:

   - ``trigger``: the condition that triggers an interrupt.
     ``Counter.ZERO``: when the counter reaches or is reset to zero.
     ``Counter.LIMIT``: when the counter reaches or is reset to the limit value.
     These values can be OR’ed together to trigger on multiple events.
   - ``priority``: the priority level of the interrupt, the values are port-specific,
     but higher values always represent higher priorities.
   - ``handler``: the function to be called when the interrupt triggers. The handler
     must take exactly one argument which is the Counter instance.
   - ``wake``: selects the power mode in which this interrupt can wake up the system.
     It can be machine.IDLE, machine.SLEEP or machine.DEEPSLEEP.
     These values can also be OR’ed together to make a pin generate interrupts in more
     than one power mode.

   ESP32 notes:
   - The ESP32's 8 units with two channels each are exposed as 16 units such that ids
     0 and 1 correspond to unit 0, ids 2 and 3 to unit 1, etc.
   - Supported triggers are ZERO and LIMIT.
   - The priority is ignored (**verify**).
   - For wake only machine.IDLE is supported.
   - The counter unit samples the input at the APB clock frequency, which is 80Mhz or 12.5ns,
     this places a lower bound on the width of pulses that can reliably be detected.
     (If variable frequency is enabled for low power operation using machine.freq then
     the APB clock may be as low as set with that method's min_freq parameter.)

   This method returns a callback object.

.. method:: Counter.value([x])

   Reads or writes the current value of the counter.

.. method:: Counter.direction([x])

   Reads or changes the counting direction.

.. method:: Counter.limit([x])

   Reads or changes the limit to which the counter counts up or from which it counts down.

Constants
---------

.. data:: Counter.UP
          Counter.DOWN

   Selects the counting direction.

.. data:: Counter.RISING
          Counter.FALLING
          Counter.BOTH

   Selects the input pin transition/edge that is counted.

.. data:: Counter.ZERO
          Counter.LIMIT

   Selects the state that causes an interrupt, see the description of the Counter.irq method.
