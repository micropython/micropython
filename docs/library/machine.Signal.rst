.. currentmodule:: machine
.. _machine.Signal:

class Signal -- control and sense external I/O devices
======================================================

The Signal class is a simple extension of Pin class. Unlike Pin, which
can be only in "absolute" 0 and 1 states, a Signal can be in "asserted"
(on) or "deasserted" (off) states, while being inverted (active-low) or
not. Summing up, it adds logical inversion support to Pin functionality.
While this may seem a simple addition, it is exactly what is needed to
support wide array of simple digital devices in a way portable across
different boards, which is one of the major MicroPython goals. Regardless
whether different users have an active-high or active-low LED, a normally
open or normally closed relay - you can develop single, nicely looking
application which works with each of them, and capture hardware
configuration differences in few lines on the config file of your app.

Following is the guide when Signal vs Pin should be used:

* Use Signal: If you want to control a simple on/off (including software
  PWM!) devices like LEDs, multi-segment indicators, relays, buzzers, or
  read simple binary sensors, like normally open or normally closed buttons,
  pulled high or low, Reed switches, moisture/flame detectors, etc. etc.
  Summing up, if you have a real physical device/sensor requiring GPIO
  access, you likely should use a Signal.

* Use Pin: If you implement a higher-level protocol or bus to communicate
  with more complex devices.

The split between Pin and Signal come from the usecases above and the
architecture of MicroPython: Pin offers the lowest overhead, which may
be important when bit-banging protocols. But Signal adds additional
flexibility on top of Pin, at the cost of minor overhead (much smaller
than if you implemented active-high vs active-low device differences in
Python manually!). Also, Pin is low-level object which needs to be
implemented for each support board, while Signal is a high-level object
which comes for free once Pin is implemented.

If in doubt, give the Signal a try! Once again, it is developed to save
developers from the need to handle unexciting differences like active-low
vs active-high signals, and allow other users to share and enjoy your
application, instead of being frustrated by the fact that it doesn't
work for them simply because their LEDs or relays are wired in a slightly
different way.

Constructors
------------

.. class:: Signal(pin_obj, invert=False)
           Signal(pin_arguments..., \*, invert=False)

   Create a Signal object. There're two ways to create it:

   * By wrapping existing Pin object - universal method which works for
     any board.
   * By passing required Pin parameters directly to Signal constructor,
     skipping the need to create intermediate Pin object. Available on
     many, but not all boards.

   The arguments are:

     - ``pin_obj`` is existing Pin object.

     - ``pin_arguments`` are the same arguments as can be passed to Pin constructor.

     - ``invert`` - if True, the signal will be inverted (active low).

Methods
-------

.. method:: Signal.value([x])

   This method allows to set and get the value of the signal, depending on whether
   the argument ``x`` is supplied or not.

   If the argument is omitted then this method gets the signal level, 1 meaning
   signal is asserted (active) and 0 - signal inactive.

   If the argument is supplied then this method sets the signal level. The
   argument ``x`` can be anything that converts to a boolean. If it converts
   to ``True``, the signal is active, otherwise it is inactive.

   Correspondence between signal being active and actual logic level on the
   underlying pin depends on whether signal is inverted (active-low) or not.
   For non-inverted signal, active status corresponds to logical 1, inactive -
   to logical 0. For inverted/active-low signal, active status corresponds
   to logical 0, while inactive - to logical 1.

.. method:: Signal.on()

   Activate signal.

.. method:: Signal.off()

   Deactivate signal.
