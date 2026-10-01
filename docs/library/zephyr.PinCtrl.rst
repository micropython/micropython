.. currentmodule:: zephyr
.. _zephyr.PinCtrl:

class PinCtrl -- configure pin functions at runtime
===================================================

Uses `Zephyr Dynamic PinCtrl API <https://docs.zephyrproject.org/latest/doxygen/html/group__pinctrl__interface__dynamic.html>`_.

This class allows configuring pin functions at runtime, either via the Dynamic PinCtrl API when device deinit is supported,
or by abusing the pinctrl API to force reconfiguration via an 'illegal' mode.

Constructors
------------

.. class:: Pinctrl(func, mode=0, pull=None, drive=0, schmitt=False, slew=0)

   Gets an object defining a specific pin configuration.

   - ``func`` may be a :ref:`zephyr.PinMux <zephyr.PinMux>` instance, or, depending on the PinMux capabilities a string or an integer corresponding to a PinMux.
   - ``mode`` is the same as :ref:`machine.Pin <machine.Pin>`'s mode, but supports only ``Pin.IN`` and ``Pin.OUT``.
   - ``pull`` is the same as :ref:`machine.Pin <machine.Pin>`'s pull.
   - ``drive`` is the pin's drive strength in microAmperes (uA), if supported.
   - ``schmitt`` set whether the schmitt trigger device on the pin is enabled.
   - ``slew`` is an integer that defines the pin's slew rate. It is platform-specific.

Methods
-------

.. method:: PinCtrl.pinmux()

    Returns the :ref:`zephyr.PinMux <zephyr.PinMux>` instance used by this PinCtrl.

Attributes
----------

.. data:: PinCtrl.MUXES

    If the platform supports it, this contains a list of all possible :ref:`zephyr.PinMux <zephyr.PinMux>` instances.
