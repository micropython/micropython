.. _pyboard_tutorial_switch:

The Switch, callbacks and interrupts
====================================

The pyboard has 2 small switches, labelled USR and RST.  The RST switch
is a hard-reset switch, and if you press it then it restarts the pyboard
from scratch, equivalent to turning the power off then back on.

The USR switch is for general use, and is controlled via a Switch object.
To make a switch object do::

    >>> sw = pyb.Switch()

Remember that you may need to type ``import pyb`` if you get an error that
the name ``pyb`` does not exist.

With the switch object you can get its status::

    >>> sw.value()
    False

This will print ``False`` if the switch is not held, or ``True`` if it is held.
Try holding the USR switch down while running the above command.

There is also a shorthand notation to get the switch status, by "calling" the
switch object::

    >>> sw()
    False

Switch callbacks
----------------

The switch is a very simple object, but it does have one advanced feature:
the ``sw.callback()`` function.  The callback function sets up something to
run when the switch is pressed, and uses an interrupt.  It's probably best
to start with an example before understanding how interrupts work.  Try
running the following at the prompt::

    >>> sw.callback(lambda:print('press!'))

This tells the switch to print ``press!`` each time the switch is pressed
down.  Go ahead and try it: press the USR switch and watch the output on
your PC.  Note that this print will interrupt anything you are typing, and
is an example of an interrupt routine running asynchronously.

As another example try::

    >>> sw.callback(lambda:pyb.LED(1).toggle())

This will toggle the red LED each time the switch is pressed.  And it will
even work while other code is running.

To disable the switch callback, pass ``None`` to the callback function::

    >>> sw.callback(None)

You can pass any function (that takes zero arguments) to the switch callback.
Above we used the ``lambda`` feature of Python to create an anonymous function
on the fly.  But we could equally do::

    >>> def f():
    ...   pyb.LED(1).toggle()
    ...
    >>> sw.callback(f)

This creates a function called ``f`` and assigns it to the switch callback.
You can do things this way when your function is more complicated than a
``lambda`` will allow.

Note that your callback functions must not allocate any memory (for example
they cannot create a tuple or list).  Callback functions should be relatively
simple.  If you need to make a list, make it beforehand and store it in a
global variable (or make it local and close over it).  If you need to do
a long, complicated calculation, then use the callback to set a flag which
some other code then responds to.

Technical details of interrupts
-------------------------------

Let's step through the details of what is happening with the switch
callback.  When you register a function with ``sw.callback()``, the switch
sets up an external interrupt trigger (falling edge) on the pin that the
switch is connected to.  This means that the microcontroller will listen
on the pin for any changes, and the following will occur:

1. When the switch is pressed a change occurs on the pin (the pin goes
   from low to high), and the microcontroller registers this change.
2. The microcontroller finishes executing the current machine instruction,
   stops execution, and saves its current state (pushes the registers on
   the stack).  This has the effect of pausing any code, for example your
   running Python script.
3. The microcontroller starts executing the special interrupt handler
   associated with the switch's external trigger.  This interrupt handler
   get the function that you registered with ``sw.callback()`` and executes
   it.
4. Your callback function is executed until it finishes, returning control
   to the switch interrupt handler.
5. The switch interrupt handler returns, and the microcontroller is
   notified that the interrupt has been dealt with.
6. The microcontroller restores the state that it saved in step 2.
7. Execution continues of the code that was running at the beginning.  Apart
   from the pause, this code does not notice that it was interrupted.

The above sequence of events gets a bit more complicated when multiple
interrupts occur at the same time.  In that case, the interrupt with the
highest priority goes first, then the others in order of their priority.
The switch interrupt is set at the lowest priority.

Further reading
---------------

For further information about using hardware interrupts see
:ref:`writing interrupt handlers <isr_rules>`.
