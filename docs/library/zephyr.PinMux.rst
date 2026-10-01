.. currentmodule:: zephyr
.. _zephyr.PinMux:

class PinMux -- function + pin opaque class for PinCtrl
=======================================================

This class allows storing and manipulating dts-side pin functions at runtime for use with the PinCtrl class.

Constructors
------------

.. class:: PinMux(func)

   Gets an object describing a pin function. The implementation is platform-specific.

   ``func`` may be a string or an integer corresponding to a function.
