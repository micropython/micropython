This directory shows the best practices for using MicroPython hardware API
(`machine` module). `machine` module strives to provide consistent API
across various boards, with the aim to enable writing portable applications,
which would work from a board to board, from a system to another systems.
This is inherently a hard problem, because hardware is different from one
board type to another, and even from exemplar of board to another. For
example, if your app requires an external LED, one user may connect it
to one GPIO pin, while another user may find it much more convenient to
use another pin. This of course applies to relays, buzzers, sensors, etc.

With complications above in mind, it's still possible to write portable
applications by using "low[est] denominator" subset of hardware API and
following simple rules outlined below. The applications won't be able
to rely on advanced hardware capabilities of a particular board and
will be limited to generic capabilities, but it's still possible to
write many useful applications in such a way, with the obvious benefit of
"write once - run everywhere" approach (only configuration for a particular
board is required).

The key to this approach is splitting your application into (at least)
2 parts:

* main application logic
* hardware configuration

The key point is that hardware configuration should be a separate file
(module in Python terms). A good name would be `hwconfig.py`, and that's
how we'll call it from now on. Another key point is that main application
should never instantiate (construct) hardware objects directly. Instead,
they should be defined in `hwconfig.py`, and main application should
import and reference hardware objects via this module. The simplest
application of this idea would look like:

`hwconfig.py`:

    from machine import Pin

    LED = Pin("A3", Pin.OUT)

`app.py`:

    from hwconfig import *
    import time

    while True:
        LED.value(1)
        time.sleep_ms(500)
        LED.value(0)
        time.sleep_ms(500)


To deploy this application to a particular board, a user will need:

1. Edit `hwconfig.py` to adjust Pin and other hardware peripheral
   parameters and locations.
2. Actually deploy `hwconfig.py` and `app.py` to a board (e.g. copy to
   board's filesystem, or build new firmware with these modules frozen
   into it).

Note that there's no need to edit the main application code! (Which may
be complex, while `hwconfig.py` should usually remain short enough, and
focused solely on hardware configuration).

An obvious improvement to this approach is the following. There're few
well-known boards which run MicroPython, and most of them include an
onboard LED. So, to help users of these boards to do configuration
quickly (that's especially important for novice users, for who may
be stumped by the need to reach out to a board reference to find LED
pin assignments), `hwconfig.py` your application ships may include
commented out sections with working configurations for different
boards. The step 1 above then will be:

1. Look thru `hwconfig.py` to find a section which either exactly
   matches your board, or the closest to it. Uncomment, and if any
   adjustments required, apply them.

It's important to keep in mind that adjustments may be always required,
and that there may be users whose configuration doesn't match any of
the available. So, always include a section or instructions for them.
Consider for example that even on a supported board, user may want to
blink not an on-board LED, but the one they connected externally.
MicroPython's Hardware API offers portability not just among "supported"
boards, but to any board at all, so make sure users can enjoy it.

There's next step of improvement to make. While having one `hwconfig.py`
with many sections would work for smaller projects with few hardware
objects, it may become more cumbersome to maintain both on programmer's
and user's sides for larger projects. Then instead of single
`hwconfig.py` file, you can provide few "template" ones for well-known
boards:

* `hwconfig_pyboard.py`
* `hwconfig_wipy.py`
* `hwconfig_esp8266.py`
* etc.

Then step 1 above will be:

1. Look thru available `hwconfig_*.py` files and find one which matches
   your board the best, then rename to `hwconfig.py` and make adjustments,
   if any.

Again, please keep in mind that there may be users whose hardware will be
completely unlike you heard of. Give them some helpful hints too, perhaps
provide `hwconfig_custom.py` with some instructions.

That's where we stop with improvements to the "separate file for hardware
configuration" idea, as it is already pretty flexible and viable. An
application in this directory shows it in practice, using slightly less
trivial example than just a blinking LED: `soft_pwm.py` implements a
software PWM (pulse width modulation) to produce an LED fade-in/fade-out
effect - without any dependence on hardware PWM availability.

Note that improvements to board configuration handling may continue further.
For example, one may invent a "configuration manager" helper module which will
try to detect current board (among well-known ones), and load appropriate
`hwconfig_*.py` - this assumes that a user would lazily deploy them all
(or that application will be automatically installed, e.g. using MicroPython's
`mip` package manager). The key point in this case remains the same as
elaborated above - always assume there can, and will be a custom configuration,
and it should be well supported. So, any automatic detection should be
overridable by a user, and instructions how to do so are among the most
important you may provide for your application.

By following these best practices, you will use MicroPython at its full
potential, and let users enjoy it too. Good luck!
