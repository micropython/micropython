class Switch -- switch object
=============================

A Switch object is used to control a push-button switch.

Usage::

     sw = pyb.Switch()       # create a switch object
     sw()                    # get state (True if pressed, False otherwise)
     sw.callback(f)          # register a callback to be called when the
                             #   switch is pressed down
     sw.callback(None)       # remove the callback

Example::

     pyb.Switch().callback(lambda: pyb.LED(1).toggle())


Constructors
------------

.. class:: pyb.Switch()

   Create and return a switch object.


Methods
-------

.. method:: switch()

   Return the switch state: ``True`` if pressed down, ``False`` otherwise.

.. method:: switch.callback(fun)

   Register the given function to be called when the switch is pressed down.
   If ``fun`` is ``None``, then it disables the callback.
