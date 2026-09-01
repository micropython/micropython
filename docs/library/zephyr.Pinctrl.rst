.. currentmodule:: zephyr
.. _zephyr.Pinctrl:

class Pinctrl -- configure pin functions at runtime
===================================================

Uses `Zephyr Dynamic Pinctrl API <https://docs.zephyrproject.org/latest/doxygen/html/group__pinctrl__interface__dynamic.html>`_.

This class allows configuring pin functions at runtime, either via the Dynamic Pinctrl API when device deinit is supported,
or by abusing the pinctrl API to force reconfiguration via an 'illegal' mode.

Constructors
------------

.. class:: Pinctrl(func[, mode[, pull[, drive[, schmitt_enable[, slew]]]]])

   Gets an object defining a specific pin configuration.

   - ``func`` may be a :ref:`zephyr.Pinctrl.Pinmux <zephyr.Pinctrl.Pinmux>` instance, or, depending on the Pinmux capabilities a string or an integer corresponding to a Pinmux.
   - ``mode`` is the same as :ref:`machine.Pin <machine.Pin>`'s mode, but supports only ``Pin.IN`` and ``Pin.OUT``.
   - ``pull`` is the same as :ref:`machine.Pin <machine.Pin>`'s pull.
   - ``drive`` is the pin's drive strength in microAmperes (uA), if supported.
   - ``schmitt_enable`` set whether the schmitt trigger device on the pin is enabled.
   - ``slew`` is an integer that defines the pin's slew rate. It is platform-specific.

Methods
-------

.. method:: Pinctrl.pinmux()

    Returns the Pinmux instance used by this Pinctrl.


Classes
-------

.. _zephyr.Pinctrl.Pinmux:
.. class:: Pinctrl.Pinmux(func)

   Gets an object describing a pin function. The implementation is platform-specific.

   ``func`` may be a string or an integer corresponding to a function.

Attributes
----------

.. data:: Pinctrl.MUXES

    If the platform supports it, this contains a list of all possible Pinmux instances.
